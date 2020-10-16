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
/*  File                : iodapi_log.c                                       */
/*  Version             : V4.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Logging Output function for pnio                                         */
/*                                                                           */
/*  THIS MODULE HAS TO BE MODIFIED BY THE PNIO USER                          */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/


#include "compiler.h"
#include "pniousrd.h"
#include "pnio_trace.h"
#include "os.h"
#include "bspadapt.h"

// *------------ external data and function ----------*
extern PNIO_UINT32      PnioLogDest;            // destination for logging messages
#if (PNIOD_PLATFORM & PNIOD_PLATFORM_EB200_400)
extern PNIO_UINT32 		TrcStoreBuf     (void);
#endif

// *----------------------------------------------------------------*
// *
// *  PNIO_Log (void)
// *
// *----------------------------------------------------------------*
// *
// *  output of debug logging
// *  necessary for PNIO, it is only used for performance measuring.
// *
// *  Input:    MsgType      PNIO_LOG__ERROR_FATAL
// *							PNIO_LOG__ERROR
// *							PNIO_LOG__IMPORTANT
// *							PNIO_LOG__NORMAL
// *							PNIO_LOG__MEMORY
// *
// *			   PackId		Package ID
// *			   ModId		Module ID
// *            fmt			format string for vfprintf
// *            argptr		pointer to print-arguments
// *
// *  Output:   ----
// *
// *----------------------------------------------------------------*
void PNIO_Log (PNIO_UINT32	  DevHndl,
				PNIO_UINT32   ErrLevel,
				PNIO_UINT32   PackId,
				PNIO_UINT32   ModId,
				PNIO_UINT32   LineNum)
{
	switch (ErrLevel)
	{
		case PNIO_LOG_ERROR_FATAL:
		{
			PNIO_UINT32 DoEndlessLoop = 1;
			if (PnioLogDest == 0)
			{
				PnioLogDest = 1;  // enable Console for Fatal Error print
			}
			PNIO_TrcPrintf ((PNIO_INT8*)"-------------------------------------------------\n");
			PNIO_TrcPrintf ((PNIO_INT8*)"##FATAL ERROR; DO ENDLESS LOOP TO KEEP STACKTRACE\n");
			PNIO_TrcPrintf ((PNIO_INT8*)"##PackId %d, ModId %d Line %d Device %d\n",
							 PackId, ModId, LineNum, DevHndl);
			if (PnioLogDest == 2)
			{
				SaveTraceBuffer();  // print trace buffer content
			}
			/* Upload instrumentation buffer on fatal error
			{
				OsIntrumentationDisable();
				OsPrintTaskProp();
				TcpIntrumentationUpload();
			}
			*/
			#if (PNIOD_PLATFORM & PNIOD_PLATFORM_EBx00)
			{
				#if (PNIOD_PLATFORM & PNIOD_PLATFORM_ECOS_EB200_400)
				PNIO_UINT32* pWatchdogReload0L = (PNIO_UINT32*)0x40002104; // 0x9876affe when loaded from ICE
				PNIO_UINT32* pWatchdogReload0H = (PNIO_UINT32*)0x40002108; // 0x9876affe when loaded from ICE
				#elif (PNIOD_PLATFORM & (PNIOD_PLATFORM_ECOS_EB200P | PNIOD_PLATFORM_POSIX_EB200P))
				PNIO_UINT32* pWatchdogReload0L = (PNIO_UINT32*)U_WDOG__RELD0_LOW;  // 0x9876affe when loaded from ICE
				PNIO_UINT32* pWatchdogReload0H = (PNIO_UINT32*)U_WDOG__RELD0_HIGH; // 0x9876affe when loaded from ICE
				#endif
				if (!(((*pWatchdogReload0L & 0x0000FFFF) == 0xAFFE) && ((*pWatchdogReload0H & 0x0000FFFF) == 0xAFFE)))
				{
					TrcStoreBuf(); // do not store trace buffer when loaded from ICE
					PNIO_ConsolePrintf ((PNIO_INT8*)"-------------------------------------------------\n");
					PNIO_ConsolePrintf ((PNIO_INT8*)"##FATAL ERROR STORED ON FLASH  <<<ENDLESS LOOP>>>\n");
					PNIO_ConsolePrintf ((PNIO_INT8*)"-------------------------------------------------\n");
			   }
				if (((*pWatchdogReload0L & 0x0000FFFF) == 0xCAFE) && ((*pWatchdogReload0H & 0x0000FFFF) == 0xCAFE))
				{
					PNIO_ConsolePrintf ((PNIO_INT8*)"### CONTINUE EXECUTION TO DEBUG FATAL ERROR REASON ... \n");
					DoEndlessLoop = 0;
			   }
			}
			#endif
			PNIO_device_stop(PNIO_SINGLE_DEVICE_HNDL);
			if (DoEndlessLoop)
			{
				OsEnter();
			}
			while (DoEndlessLoop)
			{
			#if (PNIOD_PLATFORM & (PNIOD_PLATFORM_ECOS_EB200P | PNIOD_PLATFORM_POSIX_EB200P))
				PNIO_UINT32 j;
				for (j = PNIO_LED_USER_00; j <= PNIO_LED_USER_15; j++)
				{
					Bsp_EbSetLed (j, 1);
					OsWait_ms (5);  // give time to send printf via UDP (if selected)
				}
				OsWait_ms (300);  // give time to send printf via UDP (if selected)
				for (j = PNIO_LED_USER_15; j >= PNIO_LED_USER_00; j--)
				{
					Bsp_EbSetLed (j, 0);
					OsWait_ms (5);  // give time to send printf via UDP (if selected)
				}
			#else
				Bsp_EbSetLed (PNIO_LED_ERROR, 1);
				Bsp_EbSetLed (PNIO_LED_RUN,   1);
				OsWait_ms (300);  // give time to send printf via UDP (if selected)
				Bsp_EbSetLed (PNIO_LED_ERROR, 0);
				Bsp_EbSetLed (PNIO_LED_RUN,   0);
			#endif
				OsWait_ms (300);  // give time to send printf via UDP (if selected)
			}
			break; /*lint !e527 unreachable code */
		}
		case PNIO_LOG_ERROR:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##ERROR: ");
			break;
		case PNIO_LOG_WARNING_HIGH:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##WARNING HIGH: ");
			break;
		case PNIO_LOG_WARNING:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##WARNING: ");
			break;
		case PNIO_LOG_NOTE_HIGH:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##NOTE HIGH: ");
			break;
		case PNIO_LOG_NOTE:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##NOTE: ");
			break;
		case PNIO_LOG_CHAT_HIGH:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##CHAT HIGH: ");
			break;
		case PNIO_LOG_CHAT:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##CHAT: ");
			break;
		default:
			PNIO_TrcPrintf ( (PNIO_INT8*) "##UNDEFINED LOGMSG TYPE: ");
	}
	PNIO_TrcPrintf( (PNIO_INT8*) " PackId %d, ModId %d Line %d Device %d\n",
				    PackId, ModId, LineNum, DevHndl);
}


/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2007 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/

