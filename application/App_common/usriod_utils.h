#ifndef _USR_IOD_UTILS_H
	#define _USR_IOD_UTILS_H

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
	/*  File                : usriod_utils.h                                     */
    /*  Version             : V4.0                                               */
	/*                                                                           */
	/*---------------------------------------------------------------------------*/
	/*                                                                           */
	/*  D e s c r i p t i o n :                                                  */
	/*                                                                           */
	/*  header file to usriod_utils.c.                                           */
	/*---------------------------------------------------------------------------*/
	/*                                                                           */
	/*  H i s t o r y :                                                          */
	/*                                                                           */
	/*  Date        Version        Who  What                                     */
	/*                                                                           */
	/*---------------------------------------------------------------------------*/

	PNIO_UINT32 NumOfAr;    // number of running ARs

	PNIO_UINT32 PrintDevkitVersion (void);

    PNIO_UINT32 usr_rec_check_blockhdr
            (
                REC_IO_BLOCKHDR* pBlockHdr,      // pointer to real blockheader
                PNIO_UINT16         BlockType,      // expected block type
                PNIO_UINT8          BlockLen,       // expected block length
                PNIO_UINT16         BlockVers,      // expected block version
                PNIO_ERR_STAT       *pPnioState     // return 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
            );

	PNIO_UINT32  RecInpDatObjElement_Handler
			(
				PNIO_UINT32         Api,			// api number
				PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
				PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user
				PNIO_UINT8			*pBuffer,		// [in] buffer pointer
				PNIO_ERR_STAT		*pPnioState, 	// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
				PNIO_UINT32         IoDatLen,       // length of input data
				PNIO_UINT8          *pDat,          // pointer to input data
				PNIO_UINT8          iocs,           // iocs value
				PNIO_UINT8          iops            // iops value
			);

	PNIO_UINT32  RecOutDatObjElement_Handler
			(
				PNIO_UINT32         Api,			// api number
				PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
				PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user
				PNIO_UINT8			*pBuffer,		// [in] destination buffer pointer
				PNIO_ERR_STAT		*pPnioState, 	// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
				PNIO_UINT32         IoDatLen,       // length of output data
				PNIO_UINT8          *pDat,          // pointer to output data
				PNIO_UINT8          iocs,           // iocs value
				PNIO_UINT8          iops,           // iops value
				PNIO_UINT8          *pSubstDat,     // pointer to substituted values for output data
				PNIO_UINT16         SubstMode,      // 0: ZERO,  1: last value, 2: replacement value (see pnio-spec)
				PNIO_UINT16         SubstActive     // 0: output values active, 1: substituted values active

			);



    // * ------------------------------------------
    // *  structure of the  PRIVATE DATA in flash
    // *  (this is only an example on EB200/400)
    // *
    // *  OFFSET         CONTENT
    // *  ------         -------
    // *    0...3        Dap module ID
    // *
    // * ------------------------------------------
    // **** structure of private data on EB200/400 ***
    #define NV_PRIVDATA_MODID_OFFS      0                   /* Maximum of the boot line */


    void        StoreDapModuleId 		( PNIO_UINT32 DapModuleId );      /* store DAP module id into flash */
    PNIO_UINT32 RestoreDapModuleId 		( PNIO_UINT32* pDapModuleId );  	/* restore DAP module id from flash */
	void        InputAndStoreMacAddress ( void );                  		/* input mac adress and store into flash */
	PNIO_VOID 	InputAndStoreIpAddress	( PNIO_VOID );					/* input new ip set and store into flash */
	PNIO_VOID 	InputAndStoreDeviceName	( PNIO_VOID );					/* input new device name and store into flash */
	PNIO_UINT32 TrcStoreBuf				( void );

	#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
	}
	#endif

#endif

/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
