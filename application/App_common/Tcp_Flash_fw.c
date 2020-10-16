/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2010 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : Tcp_flash_fw.c                                     */
/*  Version             : V4.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Usr functions, called by pnio stack in case of events and alarms         */
/*                                                                           */
/*  THIS MODULE HAS TO BE MODIFIED BY THE PNIO USER                          */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*---------------------------------------------------------------------------*/
    #include "compiler.h"
#ifndef LSAS_CFG_USE_EXTERNAL_TCPIP_STACK
    #include "usriod_cfg.h"
    #include "pniousrd.h"
    #include "pnio_trace.h"
    #include "os.h"
    #include "os_taskprio.h"
    #include "nv_data.h"
    #include "bspadapt.h"
    #include "usriod_utils.h"
    #include "Tcp_IF.h"
 
    #define LTRC_ACT_MODUL_ID   11

    #define TCP_FLASHLOAD_PORTNUMBER    999

    extern PNIO_UINT32 TcpFlashFirmware (void* pBuf, PNIO_UINT32 BufSize);

    typedef struct 
    {
        int StartTag;
        int FileSize;
        int FileCheckSum;
    }   IMG_FILE_HEADER;


    static int LocSockId, RemSockId;
    static PNIO_UINT32 TskId_TcpFlash;
    

    PNIO_UINT32 TcpReceiveAndFlashFirmware  (void)
    {
	    // *-------------------------------------------
	    // *   receive flash image by tcp
	    // *-------------------------------------------
        IMG_FILE_HEADER Hdr;
        PNIO_UINT8* pBuf;
        PNIO_UINT32 NumOfBytes;
        PNIO_INT32  SumBytes = 0;
        PNIO_UINT32 Status = 0;

        PNIO_printf ( (PNIO_INT8*) "\n--------------------------------------------------\n");

        // ***** init tcp interface ******
        PNIO_printf ( (PNIO_INT8*) "TCP interface init ... ");
        LocSockId = tcp_if_inits();
        if (LocSockId == TCP_SOCK_ERROR)
        {
        	PNIO_printf ( (PNIO_INT8*) "ERROR cannot create socket\n");
            return (PNIO_NOT_OK);
        }
    	PNIO_printf ( (PNIO_INT8*) "OK\n");

        // ***** connect to client ******
        PNIO_printf ( (PNIO_INT8*) "TCP interface wait on connection ... ");
        RemSockId = tcp_if_connectS (LocSockId, 0, TCP_FLASHLOAD_PORTNUMBER);
        if (RemSockId == TCP_SOCK_ERROR)
        {
        	PNIO_printf ( (PNIO_INT8*) "ERROR during TCP connect\n");
            return (PNIO_NOT_OK);
        }
        PNIO_printf ( (PNIO_INT8*) "OK, established\n");


        // ***** read data header  ******
        PNIO_printf ( (PNIO_INT8*) "Receive firmware image header ... ");
        NumOfBytes = tcp_if_receive (RemSockId, 
                                     (PNIO_UINT8*) &Hdr,
                                     sizeof (Hdr));
        if (NumOfBytes != sizeof (Hdr))
        {
        	PNIO_printf ( (PNIO_INT8*) "ERROR image size = %d\n", NumOfBytes);
            tcp_if_disconnect (RemSockId);
            tcp_if_close (LocSockId);
            return (PNIO_NOT_OK);
        }
    	PNIO_printf ( (PNIO_INT8*) "Image size = %d\n", Hdr.FileSize);


        // ***** alloc memory for file data ******
        PNIO_printf ( (PNIO_INT8*) "Allocate memory ... ");
        if ( OsAllocF ((void**)&pBuf, Hdr.FileSize) != PNIO_OK)
        {
        	PNIO_printf ( (PNIO_INT8*) "ERROR OsAlloc\n");
            tcp_if_disconnect (RemSockId);
            tcp_if_close (LocSockId);
            return (PNIO_NOT_OK);
        }
       	PNIO_printf ( (PNIO_INT8*) "done\n");


        // ***** read the image file data ******
        {
       		PNIO_UINT8 ProgressPrint = 0;
       		PNIO_printf ( (PNIO_INT8*) "Receive firmware image data\n");
       		while (SumBytes < Hdr.FileSize)
       		{
                NumOfBytes = tcp_if_receive ( RemSockId, 
                                              (PNIO_UINT8*) (pBuf+SumBytes),
                                              Hdr.FileSize - SumBytes);

                if (!ProgressPrint) { PNIO_printf ( (PNIO_INT8*) "."); }
                ProgressPrint = (ProgressPrint==10) ? 0 : ProgressPrint+1;
                SumBytes += NumOfBytes;
       		}
            PNIO_printf ( (PNIO_INT8*) "\n finished, %d bytes received\n", SumBytes);
        }

        // **** create checksum *****
        {
            int CheckSum = 0;
            int i;
            for (i = 0; i < Hdr.FileSize; i++)
            {
            	CheckSum += (int) *(pBuf+i);
            }
            CheckSum = -CheckSum;

            if (CheckSum != Hdr.FileCheckSum)
            {
            	PNIO_printf ( (PNIO_INT8*) "ERROR: Wrong Checksum = 0x%x (0x%x)\n",
                             CheckSum, Hdr.FileCheckSum);
                tcp_if_disconnect (RemSockId);
                tcp_if_close (LocSockId);
                OsFree (pBuf);
                return (PNIO_NOT_OK);
            }
            PNIO_printf ( (PNIO_INT8*) "Checksum = 0x%x OK\n", CheckSum);
        }

        // ***** now erase flash and program the firmware ****
        PNIO_printf ( (PNIO_INT8*) "\n!!! DO NOT SWITCH POWER OFF !!!\n");

        PNIO_printf ( (PNIO_INT8*) "\nERASE & PROGRAM FLASH:\n");
        Status = TcpFlashFirmware (pBuf, Hdr.FileSize);
        {
            PNIO_UINT8 buffer[40]; 
            if (Status == PNIO_NOT_OK)
            {
            	PNIO_sprintf  (buffer,  (PNIO_INT8*) "\nERROR at firmware flashing\n");
            }
            else
            {
            	PNIO_sprintf  (buffer,  (PNIO_INT8*) "\nOK, Flashing firmware finished\n");
            }

            PNIO_printf ( (PNIO_INT8*) "%s", buffer);
            NumOfBytes = tcp_if_send (RemSockId, &buffer[0], sizeof (buffer));
            if (NumOfBytes < sizeof (buffer))
            {
            	PNIO_printf ( (PNIO_INT8*) "\nERROR sending firmware update status!\n");
            }
            if (Status == PNIO_OK)
            {
            	PNIO_printf ( (PNIO_INT8*) "-----------------------------------------------\n");
            	PNIO_printf ( (PNIO_INT8*) "=> RESET your system, to start new firmware ...\n");
            	PNIO_printf ( (PNIO_INT8*) "-----------------------------------------------\n");
            }
        }

        // ***** disconnect and free memory ****
        OsWait_ms (2000);
        tcp_if_disconnect (RemSockId);
        tcp_if_close (LocSockId);
        OsFree (pBuf);
        return (PNIO_OK);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Task_TcpFlash (void) 
    // *    
    // *----------------------------------------------------------------*
    // *  cyclic exchange of IO data
    // *    
    // *  This task performs the cyclic IO data exchange in an endless   
    // *  loop.  Every IO data exchange (one data read and one data write) 
    // *         is triggered by a semaphore, that is set in PNIO_cbf_trigger_io_exchange()
    // *    
    // *----------------------------------------------------------------*
    // *  Input:    ----    
    // *  Output:   ----
    // *          
    // *----------------------------------------------------------------*
    static PNIO_INT32 Task_TcpFlash(void)
    {
        PNIO_UINT32 Status;
        
        // *----------------------------------------------------------
        // * Synchronization to parent process
        // *----------------------------------------------------------
        OsWaitOnEnable();           // must be first call in every task
        PNIO_printf ("AutoFlash Task started...\n");
        
        while (1)
        {
        	Status = TcpReceiveAndFlashFirmware ();
        	if (Status == PNIO_OK)
        	{
        		PNIO_printf ("new firmware flashed, reenable AutoFlash again..\n");
        	}
        	else
        	{
        		PNIO_printf ("###Error flashing firmware, terminate task..\n");
        		return (0);
        	}
        }
        return (0); /*lint !e527 Unreachable code */
    }
    
    void TcpAutoFlashEnable (void)
    {
        PNIO_UINT32 Status;
        
        // *----------------------------------------------------------
        // * TCP Flash task
        // *----------------------------------------------------------
        Status = OsCreateThread ((void  (*)(void))Task_TcpFlash, (PNIO_UINT8*)"Pnio_TcpFlash", TASK_PRIO_LOW, &TskId_TcpFlash);
 		PNIO_APP_ASSERT(Status == PNIO_OK, "ERROR: OsCreateThread\n");
        Status = OsStartThread (TskId_TcpFlash);
 		PNIO_APP_ASSERT(Status == PNIO_OK, "ERROR: OsStartThread\n");
   }


#endif
/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2010 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
