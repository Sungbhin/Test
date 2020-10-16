/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2007 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : iodapi_rema.c                                      */
/*  Version             : V4.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  user example code for handling the remanent physical device(PDEV) data.  */
/*                                                                           */
/*  included functions:                                                      */
/*                                                                           */
/*   PNIO_cbf_store_rema_mem() callback function, to hand over all PDEV      */
/*                             record data to the application at once        */
/*                                                                           */
/*   PNIO_restore_rema_mem()   called from the application at system startup,*/
/*                             to hand over all stored pdev records at once  */
/*                             to the PNIO stack                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*---------------------------------------------------------------------------*/

#include "compiler.h"
#if (INCLUDE_REMA == 1)
    #include "pniousrd.h"
    #include "nv_data.h"



    // *=======================================================
    // *  defines
    // *=======================================================



    // *=======================================================
    // *  static  data
    // *=======================================================

    // *=======================================================
    // *  public data
    // *=======================================================


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_restore_rema_mem (...)
    // *
    // *----------------------------------------------------------------*
    // *  This function is called from the PNIO stack, to get the
    // *  rema data block at startup, stored in nv-memory.
    // *  The function writes the pointer to the rema buffer and the
    // *  size of the rema data to the specified addresses.
    // *
    // *
    // *  Input:	   PNIO_UINT32*    pMemSize  // [in, out]size of the rema data buffer
    // *			   PNIO_UINT8**    ppMem     // [in, out]pointer to pointer to record data
    // *
    // *  Output:	   return          PNIO_OK, PNIO_NOT_OK
    // *
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32  PNIO_cbf_restore_rema_mem
		    (
			    PNIO_UINT32*     	pMemSize,       // REMA data size
			    PNIO_UINT8**	    ppMem		    // pointer to pointer to rema data
            )
    {
        PNIO_UINT32 Status;

        // *** store data in non volatile memory ***
        Status = Bsp_nv_data_restore (PNIO_NVDATA_PDEV_RECORD,      // [in]  data type
                                      (PNIO_VOID**) ppMem,          // [out] data pointer (allocated by Bsp_nv_data_restore)
                                      pMemSize);                    // [out] size of allocated memory for pNvPdevData

        if (pMemSize)
            PNIO_printf ("##REMA DATA RESTORE total Bufsize=%d\n", *pMemSize);
        else
            PNIO_printf ("##REMA DATA RESTORE ERROR\n");

        return (Status);
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_store_rema_mem (...)
    // *
    // *----------------------------------------------------------------*
    // *  This function is called from the PNIO stack, if all PDEV records
    // *  have been received and that records have to be saved into an non
    // *  volatile memory. All records are included in a continuous memory,
    // *  parameters are pointer to that memory and total memory size. The
    // *  application only has to take thismemory and store it into the
    // *  nv-memory as it is. Application needs notto analyze that data for storing.
    // *
    // *  At the next system startup the application reads out the total record data
    // *  block from nv-memory and hands it over to the stack as it is by calling
    // *  the function PNIO_restore_rema_mem(PNIO_UINT32 MemSize, PNIO_UINT8* pMem).
    // *
    // *  Input:       PNIO_UINT32,    MemSize         // size of the shadow memory
    // *               PNIO_UINT8*     pMem            // record data
    // *
    // *  Output:      return          PNIO_OK, PNIO_NOT_OK
    // *
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32  PNIO_cbf_store_rema_mem
            (
                PNIO_UINT32         MemSize,        // REMA Opcode
                PNIO_UINT8*         pMem            // request information
            )
    {
        PNIO_UINT32 Status;

        PNIO_printf ("##REMA SHADOW MEM STORE total memsize=%d\n", MemSize);

        // *** store data in non volatile memory ***
        Status = Bsp_nv_data_store (PNIO_NVDATA_PDEV_RECORD, pMem, MemSize);
        return (Status);
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_free_rema_mem (...)
    // *
    // *----------------------------------------------------------------*
    // *  deallocates an earlier allocated rema mem, when it is no more
    // *  needed by  the PNIO stack.
    // *
    // *  Input:       PNIO_UINT8*    pMem     // [pointer to rema data
    // *
    // *  Output:      return          PNIO_OK, PNIO_NOT_OK
    // *
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32  PNIO_cbf_free_rema_mem
            (
                PNIO_UINT8*   pMem           // pointer to pointer to rema data
            )
    {
        return ( Bsp_nv_data_memfree (pMem));
    }



#endif

/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
