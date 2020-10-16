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
/*  File                : usriod_im_func.c                                   */
/*  Version             : V1.0                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  This module contains an example code how to handle pnio I&M (identifica- */
/*  tion and maintanance) functions. Support of I&M0..I&M4 is mandatory for  */
/*  the DAP. All other I&M is optional.                                       */
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

#if (INCLUDE_IM0_4_HANDLING == 0)  // IM0..4 not handled inside PN stack--> do it here
    #include "usriod_cfg.h"
    #include "os.h"
    #include "pniousrd.h"
    #include "iodapi_event.h"
    #include "usriod_im_func.h"
    #include "usriod_cfg.h"
    #include "nv_data.h"

    // ********* external data *************

    // *-----------------------------------------------------------------------
    // *    some defines for IM0 data, that may be unchanged
    // *   (user configurated defines for IM0  see useriod_cfg.h)
    // *-----------------------------------------------------------------------
    #define IM0_VERSMAJ             1       // must be 01 in this version
    #define IM0_VERSMIN             1       // must be 01 in this version
    #define IM0_FILTER_DATA_DAP   0x1e      // additional support of  IM1...IM4
    #define IM0_FILTER_DATA_NODAP 0x00      // IM0 only for all submoduls except DAP 

    // *----------------------------------------------------------------*
    // *    
    // *  Im0_read_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record request on RecordIndex 0xaff0,
    // *  which is specified as a "I&M0" request.
    // *  If an error occures, the error structure PNIO_ERR_STAT is filled.  
    // *  See PNIO specification for more details, how to fill the error   
    // *  structure.  
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im0_read_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        IM0_STRUCT*     pIm0 = (IM0_STRUCT*)    pBuffer;

	    PNIO_printf ( (PNIO_INT8*) "##READ IM0 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
				     Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);

        // **** check buffer length, set total data length ***
        if (*pBufLen <  sizeof (IM0_STRUCT))        // plausibility check
        { // ***** error *****
            pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xc0;  // example: Error Class 12 = resource, ErrorNr  = "write constrain conflict"
            return (PNIO_NOT_OK);
        }
        else
            *pBufLen = sizeof (IM0_STRUCT);


        // **** fill IM0 blockheader ****
        pIm0->BlockHeader.Type      = OsHtons (0x0020);              // must be 0x0020
        pIm0->BlockHeader.Len       = OsHtons (sizeof (IM0_STRUCT) - sizeof (REC_IO_BLOCKHDR) + 2); // blocklength, header excluded
        pIm0->BlockHeader.Version   = OsHtons (0x0100);              // must be 0x0100 (see PNIO specification)

        // **** fill IM0 vendor ID ****
        pIm0->IM0.VendorId              = OsHtons (IOD_CFG_VENDOR_ID);              // vendor ID

        // **** fill IM0 order ID  and serial number****
        OsMemCpy (pIm0->IM0.OrderId, 
                DevAnnotation.OrderId,
                sizeof (pIm0->IM0.OrderId));

        OsMemCpy (pIm0->IM0.SerNum, 
                IM0_SERIAL_NUMBER,
                sizeof (pIm0->IM0.SerNum));

        // * -----------------------------------------------------
        // * fill IM0  hardware- and software revision
        // * 
        // * NOTE:  HwRevision in IM0 is defined as BIG ENDIAN.
        // * =====  HwRevision in DevAnnotation is defined as
        // *        machine dependant endian, because the stack 
        // *        performs swapping of DevAnnotation.
        // * -----------------------------------------------------
        pIm0->IM0.HwRevision     = OsHtons (DevAnnotation.HwRevision);           // defined as big endian in PNIO spec.

        pIm0->IM0.SwRevision.srp = DevAnnotation.SwRevisionPrefix;               // 0.. 0xff
        pIm0->IM0.SwRevision.fe  = (PNIO_UINT8) DevAnnotation.SwRevision1;       // 0.. 0xff
        pIm0->IM0.SwRevision.bf  = (PNIO_UINT8) DevAnnotation.SwRevision2;       // 0.. 0xff
        pIm0->IM0.SwRevision.ic  = (PNIO_UINT8) DevAnnotation.SwRevision3;       // 0.. 0xff

        // **** fill other elements of IM0 data structure ****
        pIm0->IM0.Revcnt         = OsHtons (IM0_REVCNT);                     // 0.. 0xffff
        pIm0->IM0.ProfId         = OsHtons (IM0_PROFID);                     // 0.. 0xffff, Using shall be defined by profiles 
        pIm0->IM0.ProfSpecTyp    = OsHtons (IM0_PROFSPECTYP);                // if ProfId = 0, ProfSpecTyp must be 1..6, e.g. 3="io module"
        pIm0->IM0.VersMaj        = IM0_VERSMAJ;                              // must be 01 in this version
        pIm0->IM0.VersMin        = IM0_VERSMIN;                              // must be 01 in this version

        if ((pAddr->Geo.Slot == 0) && (pAddr->Geo.Subslot == 1))
        {
            pIm0->IM0.ImXSupported   = OsHtons (IM0_FILTER_DATA_DAP);                // IM0 filter data 
        }
        else
        {
            pIm0->IM0.ImXSupported   = OsHtons (IM0_FILTER_DATA_NODAP);                // IM0 filter data 
        }

        // *** clear error structure ***
        OsMemSet (pPnioState, 0, sizeof (PNIO_ERR_STAT));
        return (PNIO_OK);
    }



    // *----------------------------------------------------------------*
    // *    
    // *  Im1_read_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record read request on RecordIndex 0xaff1 
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im1_read_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        IM1_STRUCT*     pIm = (IM1_STRUCT*)    pBuffer;
        IM1_DATA*       pImDatTmp;
        PNIO_UINT32     ImDatLen;
        PNIO_UINT32     Status;

	    PNIO_printf ( (PNIO_INT8*) "##READ IM1 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
				     Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);

        // **** check buffer length, set total data length ***
        if (*pBufLen <  sizeof (IM1_STRUCT))        // plausibility check
        { // ***** error *****
            pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xc0;  // example: Error Class 12 = resource, ErrorNr  = "write constrain conflict"
            return (PNIO_NOT_OK);
        }
        else
            *pBufLen = sizeof (IM1_STRUCT);


        // **** fill IM1 blockheader ****
        pIm->BlockHeader.Type      = OsHtons (0x0021);              // must be 0x0021
        pIm->BlockHeader.Len       = OsHtons (sizeof (IM1_STRUCT) - sizeof (REC_IO_BLOCKHDR) + 2); // blocklength, header excluded
        pIm->BlockHeader.Version   = OsHtons (0x0100);              // must be 0x0100 (see PNIO specification)

	    // *--------------------------------------------------------------
	    // * read IM1 data from NV memory and save in pIm1
	    // *--------------------------------------------------------------
        Status = Bsp_nv_data_restore (PNIO_NVDATA_IM1, // data type
                                      (PNIO_VOID**) &pImDatTmp,            // data pointer (allocated by
                                      &ImDatLen);

        OsMemCpy (&pIm->IM1, 
                  pImDatTmp,
                  sizeof (IM1_DATA));
	    Bsp_nv_data_memfree (pImDatTmp);


        // *** clear error structure ***
        OsMemSet (pPnioState, 0, sizeof (PNIO_ERR_STAT));
        return (PNIO_OK);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im2_read_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record read request on RecordIndex 0xaff2
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im2_read_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        IM2_STRUCT*     pIm = (IM2_STRUCT*)    pBuffer;
        IM2_DATA*       pImDatTmp;
        PNIO_UINT32     ImDatLen;
        PNIO_UINT32     Status;

	    PNIO_printf ( (PNIO_INT8*) "##READ IM2 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
				     Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);

        // **** check buffer length, set total data length ***
        if (*pBufLen <  sizeof (IM2_STRUCT))        // plausibility check
        { // ***** error *****
            pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xc0;  // example: Error Class 12 = resource, ErrorNr  = "write constrain conflict"
            return (PNIO_NOT_OK);
        }
        else
            *pBufLen = sizeof (IM2_STRUCT);


        // **** fill IM2 blockheader ****
        pIm->BlockHeader.Type      = OsHtons (0x0022);              // must be 0x0022
        pIm->BlockHeader.Len       = OsHtons (sizeof (IM2_STRUCT) - sizeof (REC_IO_BLOCKHDR) + 2); // blocklength, header excluded
        pIm->BlockHeader.Version   = OsHtons (0x0100);              // must be 0x0100 (see PNIO specification)

	    // *--------------------------------------------------------------
	    // * read IM2 data from NV memory and save in pIm2
	    // *--------------------------------------------------------------
        Status = Bsp_nv_data_restore (PNIO_NVDATA_IM2, // data type
                                      (PNIO_VOID**) &pImDatTmp,            // data pointer (allocated by
                                      &ImDatLen);

        OsMemCpy (&pIm->IM2, 
                  pImDatTmp,
                  sizeof (IM2_DATA));
	    Bsp_nv_data_memfree (pImDatTmp);


        // *** clear error structure ***
        OsMemSet (pPnioState, 0, sizeof (PNIO_ERR_STAT));
        return (PNIO_OK);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im3_read_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record read request on RecordIndex 0xaff3
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im3_read_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        IM3_STRUCT*     pIm = (IM3_STRUCT*)    pBuffer;
        IM3_DATA*       pImDatTmp;
        PNIO_UINT32     ImDatLen;
        PNIO_UINT32     Status;

	    PNIO_printf ( (PNIO_INT8*) "##READ IM3 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
				     Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);

        // **** check buffer length, set total data length ***
        if (*pBufLen <  sizeof (IM3_STRUCT))        // plausibility check
        { // ***** error *****
            pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xc0;  // example: Error Class 12 = resource, ErrorNr  = "write constrain conflict"
            return (PNIO_NOT_OK);
        }
        else
            *pBufLen = sizeof (IM3_STRUCT);


        // **** fill IM3 blockheader ****
        pIm->BlockHeader.Type      = OsHtons (0x0023);              // must be 0x0023
        pIm->BlockHeader.Len       = OsHtons (sizeof (IM3_STRUCT) - sizeof (REC_IO_BLOCKHDR) + 2); // blocklength, header excluded
        pIm->BlockHeader.Version   = OsHtons (0x0100);              // must be 0x0100 (see PNIO specification)

	    // *--------------------------------------------------------------
	    // * read IM2 data from NV memory and save in pIm2
	    // *--------------------------------------------------------------
        Status = Bsp_nv_data_restore (PNIO_NVDATA_IM3, // data type
                                      (PNIO_VOID**) &pImDatTmp,            // data pointer (allocated by
                                      &ImDatLen);

        OsMemCpy (&pIm->IM3, 
                  pImDatTmp,
                  sizeof (IM3_DATA));
	    Bsp_nv_data_memfree (pImDatTmp);


        // *** clear error structure ***
        OsMemSet (pPnioState, 0, sizeof (PNIO_ERR_STAT));
        return (PNIO_OK);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im4_read_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record read request on RecordIndex 0xaff4
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im4_read_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        IM4_STRUCT*     pIm = (IM4_STRUCT*)    pBuffer;
        IM4_DATA*       pImDatTmp;
        PNIO_UINT32     ImDatLen;
        PNIO_UINT32     Status;

	    PNIO_printf ( (PNIO_INT8*) "##READ IM4 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
				     Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);

        // **** check buffer length, set total data length ***
        if (*pBufLen <  sizeof (IM4_STRUCT))        // plausibility check
        { // ***** error *****
            pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xc0;  // example: Error Class 12 = resource, ErrorNr  = "write constrain conflict"
            return (PNIO_NOT_OK);
        }
        else
            *pBufLen = sizeof (IM4_STRUCT);


        // **** fill IM4 blockheader ****
        pIm->BlockHeader.Type      = OsHtons (0x0024);              // must be 0x0024
        pIm->BlockHeader.Len       = OsHtons (sizeof (IM4_STRUCT) - sizeof (REC_IO_BLOCKHDR) + 2); // blocklength, header excluded
        pIm->BlockHeader.Version   = OsHtons (0x0100);              // must be 0x0100 (see PNIO specification)

	    // *--------------------------------------------------------------
	    // * read IM2 data from NV memory and save in pIm2
	    // *--------------------------------------------------------------
        Status = Bsp_nv_data_restore (PNIO_NVDATA_IM4, // data type
                                      (PNIO_VOID**) &pImDatTmp,            // data pointer (allocated by
                                      &ImDatLen);

        OsMemCpy (&pIm->IM4, 
                  pImDatTmp,
                  sizeof (IM4_DATA));
	    Bsp_nv_data_memfree (pImDatTmp);


        // *** clear error structure ***
        OsMemSet (pPnioState, 0, sizeof (PNIO_ERR_STAT));
        return (PNIO_OK);
    }



    // *----------------------------------------------------------------*
    // *    
    // *  Im0_write_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record write request on RecordIndex 0xaff0
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im0_write_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        // *** return error:  IM0 is readable only ***
        pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
        pPnioState->ErrDecode = 0x80;  // PNIORW
        pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
        pPnioState->ErrCode2  = 0;     // here dont care
        pPnioState->AddValue1 = 0;     // here dont care
        pPnioState->AddValue2 = 0;     // here dont care
        return (PNIO_NOT_OK);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im1_write_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record write request on RecordIndex 0xaff1
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im1_write_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to write, out: length, write by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        PNIO_UINT32 Status;

        if (*pBufLen > sizeof (IM1_DATA))
            *pBufLen = sizeof (IM1_DATA);

        Status =  Bsp_nv_data_store (PNIO_NVDATA_IM1,       // nv data type: device name
                                     pBuffer,               // source pointer to the devicename
                                     *pBufLen);               // length of the device name

        if (Status == PNIO_OK)
        {
            PNIO_printf ( (PNIO_INT8*) "##WRITE IM1 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }
        else
        {
            // *** return error ***
            pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
            pPnioState->ErrCode2  = 0;     // here dont care
            pPnioState->AddValue1 = 0;     // here dont care
            pPnioState->AddValue2 = 0;     // here dont care
            PNIO_printf ( (PNIO_INT8*) "##ERROR WRITE IM1 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }

        return (Status);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im2_write_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record write request on RecordIndex 0xaff2
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im2_write_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        PNIO_UINT32 Status;

        if (*pBufLen > sizeof (IM2_DATA))
            *pBufLen = sizeof (IM2_DATA);

        Status =  Bsp_nv_data_store (PNIO_NVDATA_IM2,       // nv data type: device name
                                     pBuffer,               // source pointer to the devicename
                                     *pBufLen);               // length of the device name

        if (Status == PNIO_OK)
        {
            PNIO_printf ( (PNIO_INT8*) "##WRITE IM2 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }
        else
        {
            // *** return error ***
            pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
            pPnioState->ErrCode2  = 0;     // here dont care
            pPnioState->AddValue1 = 0;     // here dont care
            pPnioState->AddValue2 = 0;     // here dont care
            PNIO_printf ( (PNIO_INT8*) "##ERROR WRITE IM2 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }

        return (Status);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im3_write_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record write request on RecordIndex 0xaff3
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im3_write_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        PNIO_UINT32 Status;

        if (*pBufLen > sizeof (IM3_DATA))
            *pBufLen = sizeof (IM3_DATA);

        Status =  Bsp_nv_data_store (PNIO_NVDATA_IM3,       // nv data type: device name
                                     pBuffer,               // source pointer to the devicename
                                     *pBufLen);               // length of the device name

        if (Status == PNIO_OK)
        {
            PNIO_printf ( (PNIO_INT8*) "##WRITE IM3 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }
        else
        {
            // *** return error ***
            pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
            pPnioState->ErrCode2  = 0;     // here dont care
            pPnioState->AddValue1 = 0;     // here dont care
            pPnioState->AddValue2 = 0;     // here dont care
            PNIO_printf ( (PNIO_INT8*) "##ERROR WRITE IM3 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }

        return (Status);
    }


    // *----------------------------------------------------------------*
    // *    
    // *  Im4_write_Handler  (....)
    // *----------------------------------------------------------------*
    // *  this function handles a record write request on RecordIndex 0xaff4
    // *----------------------------------------------------------------*
    PNIO_UINT32  Im4_write_Handler 
		    (
                PNIO_UINT32         Api,			// api number
			    PNIO_DEV_ADDR		*pAddr, 		// geographical or logical address
			    PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user	
			    PNIO_UINT8			*pBuffer,		// [in] buffer pointer
			    PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
		    )
    {
        PNIO_UINT32 Status;

        if (*pBufLen > sizeof (IM4_DATA))
            *pBufLen = sizeof (IM1_DATA);

        Status =  Bsp_nv_data_store (PNIO_NVDATA_IM4,       // nv data type: device name
                                     pBuffer,               // source pointer to the devicename
                                     *pBufLen);               // length of the device name

        if (Status == PNIO_OK)
        {
            PNIO_printf ( (PNIO_INT8*) "##WRITE IM4 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }
        else
        {
            // *** return error ***
            pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
            pPnioState->ErrCode2  = 0;     // here dont care
            pPnioState->AddValue1 = 0;     // here dont care
            pPnioState->AddValue2 = 0;     // here dont care
            PNIO_printf ( (PNIO_INT8*) "##ERROR WRITE IM4 Data, Api=%d Slot=%d Subslot=%d Len=%d\n",
		                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen);
        }

        return (Status);
    }

#endif //INCLUDE_IM0_4_HANDLING

/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2012 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/

