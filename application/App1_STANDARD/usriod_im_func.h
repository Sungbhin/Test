#ifndef _USR_IOD_IM_FUNC_H
#define _USR_IOD_IM_FUNC_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif
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
/*  File                : usriod_im_func.h                                   */
/*  Version             : V1.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  header file to usriod_im_func.c.                                         */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/


PNIO_ANNOTATION*  UsrIod_BuildDeviceAnnotation (void);

    
    
PNIO_UINT32  Im0_read_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im1_read_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im2_read_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im3_read_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im4_read_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im0_write_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im1_write_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im2_write_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im3_write_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);

PNIO_UINT32  Im4_write_Handler 
		(
            PNIO_UINT32         Api,			// api number
			PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		);


/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2010 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif

