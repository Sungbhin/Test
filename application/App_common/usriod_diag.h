#ifndef _USR_IOD_DIAG_H
#define _USR_IOD_DIAG_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif
/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : usriod_diag.h                                      */
/*  Version             : V4.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  header file to UsrIodDiag.c. It contains an example, how to handle       */
/*  an standard channel diagnostic alarm                                     */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/



PNIO_UINT32 UsrChanDiag     (PNIO_UINT16 AlarmState,        // DIAG_CHANPROP_SPEC_ERR_APP, DIAG_CHANPROP_SPEC_ERR_DISAPP
                             PNIO_UINT32 SlotNum,	        // slot number
                             PNIO_UINT32 SubNum,	        // subslot number
                             PNIO_UINT16 ChanNum,	        // channel number
                             PNIO_UINT16 ErrorNum,	        // error number, see PNIO specification coding of "ChannelErrorType"
                             PNIO_UINT16 ChanDir,	        // channel direction (DIAG_CHANPROP_DIR_IN, DIAG_CHANPROP_DIR_OUT,...)
                             PNIO_UINT16 ChanTyp, 	        // channel type (DIAG_CHANPROP_TYPE_BYTE, DIAG_CHANPROP_TYPE_WORD,...)
							 PNIO_UINT16 DiagTag); 		    // user defined diag tag != 0

PNIO_UINT32 UsrExtDiag     ( PNIO_UINT16 AlarmState,        // DIAG_CHANPROP_SPEC_ERR_APP, DIAG_CHANPROP_SPEC_ERR_DISAPP, DIAG_CHANPROP_SPEC_ERR_DISAPP_MORE
                             PNIO_UINT32 SlotNum,	        // slot number
                             PNIO_UINT32 SubNum,	        // subslot number
                             PNIO_UINT16 ChanNum,	        // see PNIO specification coding of "ChannelNumber"
                             PNIO_UINT16 ChanErrType,       // see PNIO specification coding of "ChannelErrorType"
                             PNIO_UINT16 ExtChanErrType,    // see PNIO specification coding of "ExtChannelErrorType"
                             PNIO_UINT32 ExtChanAddValue,   // see PNIO specification coding of "ExtChannelAddValue"
                             PNIO_UINT16 ChanDir,	        // channel direction (DIAG_CHANPROP_DIR_IN, DIAG_CHANPROP_DIR_OUT,...)
                             PNIO_UINT16 ChanTyp, 	        // channel type (DIAG_CHANPROP_TYPE_BYTE, DIAG_CHANPROP_TYPE_WORD,...)
							 PNIO_UINT16 DiagTag); 		    // user defined diag tag != 0

PNIO_UINT32 UsrGenericDiag  (PNIO_UINT16 AlarmState,        // DIAG_CHANPROP_SPEC_ERR_APP, DIAG_CHANPROP_SPEC_ERR_DISAPP, DIAG_CHANPROP_SPEC_ERR_DISAPP_MORE
                             PNIO_UINT32 SlotNum,	        // slot number
                             PNIO_UINT32 SubNum,	        // subslot number
                             PNIO_UINT16 ChanNum,	        // channel number
                             PNIO_VOID*  pInfoData,         // user defined generic diagnostic data
                             PNIO_UINT32 InfoDataLen,       // length of generic diagnostic data
                             PNIO_UINT16 DiagTag,           // user defined diag tag, used as reference
                             PNIO_UINT16 UserStructIdent,   // manufacturer specific, 0...0x7fff, see IEC 61158
                             PNIO_UINT16 ChanDir,	        // channel direction (DIAG_CHANPROP_DIR_IN, DIAG_CHANPROP_DIR_OUT,...)
                             PNIO_UINT16 ChanTyp);	        // channel type (DIAG_CHANPROP_TYPE_BYTE, DIAG_CHANPROP_TYPE_WORD,...)


/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif

