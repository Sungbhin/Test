/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2014 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : iodapi_event.c                                     */
/*  Version             : V4.2                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PNIO event handler for this application example                          */
/*                                                                           */
/*  performs user functions, called by stack in case of events and alarms    */
/*                                                                           */
/*  To use this application example, set #define EXAMPL_DEV_CONFIG_VERSION 1 */
/*  in file \application\usriod_app.h                                        */
/*                                                                           */
/*  THIS MODULE HAS TO BE MODIFIED BY THE PNIO USER                          */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*---------------------------------------------------------------------------*/
#include "compiler.h"
#include "usriod_cfg.h"
#include "usrapp_cfg.h"   // example application spec. configuration settings

#if (EXAMPL_DEV_CONFIG_VERSION == 1)
    #include "pniousrd.h"
    #include "iodapi_event.h"
    #include "os.h"
    #include "usriod_im_func.h"
    #include "nv_data.h"
    #include "bspadapt.h"
    #include "PnUsr_Api.h"
    #include "usriod_PE.h"
    #include "usriod_utils.h"

    /*=======================================================================================================*/
    /*lint -esym(438,DevHndl)  438 Last value assigned to variable 'DevHndl' (defined at line 1197) not used */
   	/*=======================================================================================================*/

	// *=======================================================
    // *  defines
    // *=======================================================
    #define EXAMPLE_SUBSTITUTE_VALUE    0xab  // example: substituted value for output (data consumer)
    #define EXAMPLE_IN_VALUE		    0x11  // example: default input value (data provider)

    #define DEMO_RECORD  "ABCDEFGH"


    // *=======================================================
    // *  extern  functions
    // *=======================================================

    // *=======================================================
    // *  extern  data
    // *=======================================================

    // *=======================================================
    // *  static  data
    // *=======================================================

    // *=======================================================
    // *  public data
    // *=======================================================
    #if  EXAMPLE_IPAR_SERVER
       // ***** EXAMPLE IPAR:  upload&storage alarm ********
       PNIO_UINT32  Example_iParData [NUMOF_SLOTS][NUMOF_SUBSLOTS]; // 4 byte example data per subslot
    #endif


    // ***** asynchronous record rd/wr only
    USR_REC_READ_RQ   PnioRecReadRq;
    USR_REC_WRITE_RQ  PnioRecWriteRq;



    // *** AR properties


    // *** Output Data (IO Controller ==> IO Device) and substituted values for output data ***
    PNIO_UINT8     OutData		  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1][NUMOF_BYTES_PER_SUBSLOT];
#if !INCLUDE_REC8028_8029_HANDLING
    PNIO_UINT8     OutSubstVal	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1][NUMOF_BYTES_PER_SUBSLOT];
#endif
    PNIO_UINT16    OutDatLen	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     OutDatIocs	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     OutDatIops	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     OutDatIops_old [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];

    // *** Input Data  (IO Device ==> IO Controller)
    PNIO_UINT8     InData		  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1][NUMOF_BYTES_PER_SUBSLOT];
    PNIO_UINT16    InDatLen		  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     InDatIops	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     InDatIocs	  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];
    PNIO_UINT8     InDatIocs_old  [NUMOF_SLOTS][NUMOF_SUBSLOTS+1];


    // *----------------------------------------------------------------*
    // *  get subslot index from slot and subslot                       *
    // *----------------------------------------------------------------*
    PNIO_UINT32 PnUsr_get_subslot_index(PNIO_UINT32 slot, PNIO_UINT32 subslot)
    {
    	if (slot) // PERI
    	{
    		return subslot;
    	}
    	else // HEAD
    	{
    		switch (subslot)
    		{
    		case 0x0001:	return 1;
    		case 0x8000:	return 2;
    		case 0x8001:	return 3;
    		case 0x8002:	return 4;
    		case 0x8003:	return 5;
    		case 0x8004:	return 6;
    		default: 		return 0;
    		}
    	}
    }

    // *----------------------------------------------------------------*
    // *
    // *  PnUsr_cbf_iodapi_event_varinit (void)
    // *
    // *  initialize static variables (must be called first during startup )
    // *----------------------------------------------------------------*
    void PnUsr_cbf_iodapi_event_varinit (PNIO_SUB_LIST_ENTRY* pIoSubList,
    									 PNIO_UINT32          NumOfSubListEntries)
    {
    	PNIO_UINT32 sublistIndex;
    	PNIO_UINT32 subslotIndex;

        OsMemSet (&InData[0][0][0],       EXAMPLE_IN_VALUE,   sizeof (InData));           // io data (input)
	    OsMemSet (&InDatIops[0][0],       PNIO_S_GOOD,        sizeof (InDatIops));        // local provider status
	    OsMemSet (&InDatIocs[0][0],       PNIO_S_GOOD,        sizeof (InDatIocs));        // remote consumer status
	    OsMemSet (&InDatIocs_old[0][0],   PNIO_S_GOOD,        sizeof (InDatIocs_old));    // old value of remote consumer status

	    OsMemSet (&OutData[0][0][0],      0,                  sizeof (OutData));          // io data (output)
	    OsMemSet (&OutDatIocs[0][0],      PNIO_S_GOOD,        sizeof (OutDatIocs));       // local consumer status
	    OsMemSet (&OutDatIops[0][0],      PNIO_S_GOOD,        sizeof (OutDatIops));       // remote provider status
	    OsMemSet (&OutDatIops_old[0][0],  PNIO_S_GOOD,        sizeof (OutDatIops_old));   // old value of remote provider status


    	for (sublistIndex = 0; sublistIndex < NumOfSubListEntries; sublistIndex++)
	    {
        	subslotIndex = PnUsr_get_subslot_index(pIoSubList->Slot, pIoSubList->Subslot);
        	if (subslotIndex)
        	{
        		// --- inputs -----------------------------
        		InDatLen   [pIoSubList->Slot][subslotIndex]  = (PNIO_UINT16) pIoSubList->InDatLen;
        		// --- outputs -----------------------------
				OutDatLen   [pIoSubList->Slot][subslotIndex]  = (PNIO_UINT16) pIoSubList->OutDatLen;
        	}
	    	pIoSubList++;
	    }
    }



    // *----------------------------------------------------------------*
    // *
    // *  ChangeAllInputData (DiffVal)
    // *
    // *  prints all input and output data, which are used by an
    // *  application relation with data length <> 0
    // *
    // *----------------------------------------------------------------*
    void ChangeAllInputData (PNIO_INT8 DiffVal)
    {
	    PNIO_INT32 SlotNum;// loop var for slot-number
	    PNIO_INT32 SubNum;	// loop var for subslot-number
	    PNIO_INT32 IoInd;	// index of an io-data byte in a subslot

	    // *---------------------------------------------------------------
	    // *  print input data
	    // *---------------------------------------------------------------
	    for (SlotNum = 0; SlotNum < NUMOF_SLOTS; SlotNum++)			// loop over all slots
	    {
		    for (SubNum = 0; SubNum < NUMOF_SUBSLOTS; SubNum++)	// loop over all subslots
		    {
		    	if (InDatLen[SlotNum][SubNum] != 0)		// data used by io ar == data_len is <> 0??
		    	{
		    		for (IoInd=0; IoInd < InDatLen[SlotNum][SubNum]; IoInd++)
		    		{
		    			InData[SlotNum][SubNum][IoInd] += DiffVal;
		    		}
		    	}
		    }
	    }
    }


    // *----------------------------------------------------------------*
    // *
    // *  PrintAllUsedIoData ()
    // *
    // *  prints all input and output data, which are used by an
    // *  application relation with data length <> 0
    // *
    // *----------------------------------------------------------------*
    void PrintAllUsedIoData (void)
    {
	    PNIO_INT32 SlotNum;// loop var for slot-number
	    PNIO_INT32 SubNum;	// loop var for subslot-number
	    PNIO_INT32 IoInd;	// index of an io-data byte in a subslot

	    PNIO_INT32 DataAvailable = 0;

	    // *---------------------------------------------------------------
	    // *  print input data
	    // *---------------------------------------------------------------
	    for (SlotNum = 0; SlotNum < NUMOF_SLOTS; SlotNum++)			// loop over all slots
	    {
		    for (SubNum = 0; SubNum < NUMOF_SUBSLOTS; SubNum++)	// loop over all subslots
		    {
		      if (InDatLen[SlotNum][SubNum] != 0)		// data used by io ar == data_len is <> 0??
		      {
			    DataAvailable = 1;
			    PNIO_printf ( (PNIO_INT8*) "##DatIn Slot=%d  Sub=%d length=%d : ",
				        SlotNum,					// slot num
					    SubNum,						// subslot num
					    InDatLen[SlotNum][SubNum]);	// data length, defined in step 7 hw-config

			    for (IoInd=0; IoInd < InDatLen[SlotNum][SubNum]; IoInd++)
			    {
				    PNIO_printf ( (PNIO_INT8*) "0x%02x ", InData[SlotNum][SubNum][IoInd]);
			    }
			    PNIO_printf ( (PNIO_INT8*) "\n");
		      }
		    }
	    }

	    // *---------------------------------------------------------------
	    // *  print output data
	    // *---------------------------------------------------------------
	    for (SlotNum = 0; SlotNum < NUMOF_SLOTS; SlotNum++)			// loop over all slots
	    {
		    for (SubNum = 0; SubNum < NUMOF_SUBSLOTS; SubNum++)	// loop over all subslots
		    {
		      if (OutDatLen[SlotNum][SubNum] != 0)		// data used by io ar == data_len is <> 0??
		      {
			    DataAvailable = 1;
			    PNIO_printf ( (PNIO_INT8*) "##DatOut Slot=%d  Sub=%d length=%d : ",
				        SlotNum,					// slot num
					    SubNum,						// subslot num
					    OutDatLen[SlotNum][SubNum]);// data length, defined in step 7 hw-config

			    for (IoInd=0; IoInd < OutDatLen[SlotNum][SubNum]; IoInd++)
			    {
				    PNIO_printf ( (PNIO_INT8*) "0x%02X ", OutData[SlotNum][SubNum][IoInd]);
			    }
			    PNIO_printf ( (PNIO_INT8*) "\n");
		      }
		    }
	    }

	    if (DataAvailable == 0)
		    PNIO_printf ( (PNIO_INT8*) "no io data available in any active IOCR\n");

    }

    // *----------------------------------------------------------------*
    // *
    // *  UsrSetIops (slot_num, subslot_num, Iops
    // *----------------------------------------------------------------*
    // *
    // *  sets the IO provider state for the specified slot/subslot
    // *
    // *  Input:	   PNIO_UINT32     slot_num,	// [in]  slot number of module
    // *			   PNIO_UINT32     subslot_num,	// [in]  subslot number of submodule
    // *			   PNIO_UINT8      Iops			// [in]  io provider state
    // *
    // *	Output:	   ---
    // *
    // *
    // *----------------------------------------------------------------*
    void UsrSetIops (PNIO_UINT32  SlotNum, PNIO_UINT32  SubslotNum, PNIO_UINT8    Iops)
    {
        InDatIops [SlotNum][SubslotNum] = Iops;
    }



    // *****************************************************************************************
    // *************************** read and write input data ***********************************
    // *****************************************************************************************

    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_data_write (...)
    // *
    // *----------------------------------------------------------------*
    // *
    // *  Passes the input data from the application to the stack.
    // *  The application reads the data from the specified input module
    // *  and handles them to the stack.
    // *  function UsrReadInputData() is called by the pnio stack.
    // *
    // *  Input:	   PNIO_UINT32    DevHndl,		// [in]  Device Handle
    // *			   PNIO_DEV_ADDR  *pAddr,		// [in]  location (slot, subslot)
    // *			   PNIO_UINT32    data_len,	    // [in] data length
    // *			   PNIO_UINT8     *pBuffer      // [in,out] pointer to the input data
    // *			   PNIO_UINT8     Iocs			// [in]  (io controller) consumer status
    // *
    // *	Output:	   return   io-provider-status  // PNIO_S_GOOD, PNIO_S_BAD
    // *
    // *
    // *----------------------------------------------------------------*
    PNIO_IOXS    PNIO_cbf_data_write
		    (PNIO_UINT32	DevHndl,		// [in]  device handle
		     PNIO_DEV_ADDR	*pAddr,			// geographical or logical address
		     PNIO_UINT32 	BufLen,			// [in]	 length of the submodule input data
		     PNIO_UINT8     *pBuffer, 		// [in,out] Ptr to data buffer to write to
		     PNIO_IOXS	    Iocs            // [in]  remote (io controller) consumer status
            )
    {
        PNIO_UINT32 slot_num	= pAddr->Geo.Slot;
	    PNIO_UINT32 subslot_num = pAddr->Geo.Subslot;

	    PNIO_UNUSED_ARG (DevHndl);
        OsMemCpy (pBuffer, &InData[slot_num][subslot_num][0], BufLen);
        InDatIocs [slot_num][subslot_num] = Iocs;    // consumer status (of remote io controller)

        // *---- check provider status and notify modifications ------*
        if (InDatIocs_old [slot_num][subslot_num] != Iocs)
        {
             PNIO_printf ( (PNIO_INT8*) "new IO controller input consumer status (ICS) = 0x%x in slot %d, subslot %d\n",
		              Iocs, slot_num, subslot_num);
             InDatIocs_old [slot_num][subslot_num] = Iocs;
        }
        return ( (PNIO_IOXS)InDatIops [slot_num][subslot_num]);	// return local provider state
    }

    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_data_read (...)
    // *
    // *----------------------------------------------------------------*
    // *
    // *  Passes the output data from the stack to the application.
    // *  The application takes the data and writes them to the specified
    // *  output module.
    // *  function UsrWriteOutputData() is called by the pnio stack.
    // *
    // *  Input:
    // *			   PNIO_UINT32    DevHndl       // [in] device handle
    // *			   PNIO_DEV_ADDR  *pAddr,		// [in]  location (slot, subslot)
    // *			   PNIO_UINT32    data_len,	    // [in] data length
    // *			   PNIO_UINT8*    pBuffer       // [in] pointer to the input data
    // *			   PNIO_UINT8     IoPs			// [in]  (io controller) provider status
    // *
    // *	Output:	   return   io-consumer-status  // IOCS_STATE_GOOD, IOCS_STATE_BAD
    // *
    // *
    // *----------------------------------------------------------------*
    PNIO_IOXS     PNIO_cbf_data_read
		    (PNIO_UINT32	DevHndl,		// [in]  device handle
		     PNIO_DEV_ADDR	*pAddr,			// [in]  geographical or logical address
		     PNIO_UINT32 	BufLen,			// [in]  length of the submodule input data
		     PNIO_UINT8*    pBuffer, 		// [in]  Ptr to data buffer to read from
		     PNIO_IOXS	    Iops            // [in]  (io controller) provider status
            )
    {
        PNIO_UINT32 slot_num	= pAddr->Geo.Slot;
        PNIO_UINT32 subslot_num = pAddr->Geo.Subslot;

        PNIO_UNUSED_ARG (DevHndl);

        // save last data block
        OsMemCpy (&OutData[slot_num][subslot_num][0], pBuffer, BufLen);

        OutDatIops [slot_num][subslot_num] = Iops;    // provider status (of remote io controller)

        // *---- check provider status and notify modifications ------*
        if (OutDatIops_old [slot_num][subslot_num] != Iops)
        {
            PNIO_printf ( (PNIO_INT8*) "new IO controller output provider status (OPS) = 0x%x in slot %d, subslot %d\n",
                           Iops, slot_num, subslot_num);
            OutDatIops_old [slot_num][subslot_num] = Iops;
        }

        return ( (PNIO_IOXS)OutDatIocs [slot_num][subslot_num]);	// consumer state (of local io device)
    }



    // ****************************************************************************************
    // ******************* connect indication and ownership indication  ***********************
    // ****************************************************************************************


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_ar_connect_ind (pAR)
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The AR Info Indication of a module
    // *
    // *    Input:	   PNIO_UINT32    pAr;		// ID of the Application Relation
    // *    Output:	   ---
    // *
    // *
    // *----------------------------------------------------------------*
    void PNIO_cbf_ar_connect_ind
	    (
	        PNIO_UINT32	    DevHndl,	        // [in] handle for a multidevice
	        PNIO_AR_TYPE    ArType,             // [in] type of AR (see cm_ar_type_enum)
	        PNIO_UINT32     ArNum,              // [in] AR number  (device access: ArNumber = 3)
            PNIO_UINT16     SendClock,          // [in] sendclock
            PNIO_UINT16     RedRatioIocrIn,     // [in] reduction ratio of input IOCR
            PNIO_UINT16     RedRatioIocrOut     // [in] reduction ratio of output IOCR
        )
    {
	    PNIO_UNUSED_ARG (DevHndl);
        NumOfAr++;      // increment number of running ARs

	    PNIO_printf ( (PNIO_INT8*) "##CONNECT_IND AR=%d AR type=%d sendclock=%d, reduction_ratio_in=%d, reduction_ratio_out=%d, \n",
                      ArNum,
			          ArType,
                      SendClock,
                      RedRatioIocrIn, RedRatioIocrOut);
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_ar_ownership_ind (pAR)
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The AR ownership Indication of a module
    // *
    // *  Input:   PNIO_UINT32 pAr;		                ID of the Application Relation
    // *
    // *  Output:  PNIO_EXP*   pOwnSub->OwnSessionKey   set to 0 only if ownership is rejected
    // *			                                    else keep unchanged
    // *                       pOwnSub->IsWrongSubmod   set to PNIO_TRUE only if wrong module,
    // *			                                    else keep unchanged
    // *
    // *----------------------------------------------------------------*
    PNIO_VOID   PNIO_cbf_ar_ownership_ind
		    (
			    PNIO_UINT32		DevHndl,
                PNIO_UINT32     ArNum,          // AR number 1....NumOfAR
			    PNIO_EXP*   	pOwnSub     	// [in] expected configuration in ownership indication
            )
    {
	    PNIO_UINT32 i;

	    PNIO_UNUSED_ARG (DevHndl);

	    PNIO_printf ( (PNIO_INT8*) "##OWNERSHIP_IND AR = %d number of submodules = %d\n",
                     ArNum, pOwnSub->NumOfPluggedSub);

        for (i = 0; i < pOwnSub->NumOfPluggedSub; i++)
        {
            PNIO_printf ( (PNIO_INT8*) "  Api=%d Slot=%d Sub=%d ModID=%d SubID=%d OwnSessKey=%d isWrong=%d\n",
                 pOwnSub->Sub[i].ApiNum,
                 pOwnSub->Sub[i].SlotNum,
                 pOwnSub->Sub[i].SubNum,
                 pOwnSub->Sub[i].ModIdent,
                 pOwnSub->Sub[i].SubIdent,
                 pOwnSub->Sub[i].OwnSessionKey,
                 pOwnSub->Sub[i].IsWrongSubmod
                );

            #if 0
                // *------------------------------------------------
                // *  Only if submodule ownership shall be rejected,
                // *  application has to set the OwnSessionKey to 0,
                // *  otherwise keep unchanged
                // *------------------------------------------------
                pOwnSub->Sub[i].OwnSessionKey = 0;     // reject ownership
            #endif
            #if 0
                // *------------------------------------------------
                // *  If real and expected module/submodule ID are
                // *  not equal, the stack presets IsWrongSubmod = TRUE.
                // *  No IO data exchange is possible with this submodule.
                // *  if the submodule is a compatible substitute module,
                // *  set IsWrongSubmod = FALSE, otherwise keep unchanged.
                // *------------------------------------------------
                pOwnSub->Sub[i].IsWrongSubmod = PNIO_FALSE;     // is a compatible substitute-submodule
            #endif

        }  // end for (i = 0; i < pOwnSub->NumOfPluggedSub...
    }



    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_param_end_ind ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The PNIO controller has sent an "PARAM END" event
    // *
    // *  Input:	DevHndl        Device handle
    // *            ArNum          AR number (1...N)
    // *            SessionKey     session key
    // *            Api            API (valid only, if SubslotNum <> 0)
    // *            SlotNum        SlotNum (valid only, if SubslotNum <> 0)
    // *            SubslotNum     == 0:    param end for all submodules,
    // *                           <> 0:    param end only for this submodule
    // *            MoreFollows    PNIO_TRUE: more param end ind follow, PNIO_FALSE: last one
    // *  Output:   return         PNIO_SUBMOD_STATE_RUN               module works properly and provides valid data, stack will create appl-ready now
    // *                           PNIO_SUBMOD_STATE_STOP              module has problem and can't provide valid data, stack will create appl-ready now (mod-diffblock entry)
    // *                           PNIO_SUBMOD_STATE_APPL_RDY_FOLLOWS  module parameterization is still running, application will notify appl-ready later to stack
    // *
    // *----------------------------------------------------------------*
    PNIO_SUBMOD_STATE  PNIO_cbf_param_end_ind (PNIO_UINT32 DevHndl,
                                         PNIO_UINT16 ArNum,
                                         PNIO_UINT16 SessionKey,
                                         PNIO_UINT32 Api,
                                         PNIO_UINT16 SlotNum,
                                         PNIO_UINT16 SubslotNum,
                                         PNIO_BOOL   MoreFollows)

    {
        if (SubslotNum == 0)
        { // ****param end for all submodules ****
    	    PNIO_printf ( (PNIO_INT8*) "##PARAM END  for all submodules, ArNum=%d, Session=%d \n",
                         ArNum, SessionKey);
        }
        else
        { // ****param end for one submodule ****
    	    PNIO_printf ( (PNIO_INT8*) "##PARAM END for Api=%d, Slot=%d, Sub=%d, ArNum=%d, Session=%d \n",
                         Api, SlotNum, SubslotNum, ArNum, SessionKey);
        }

        return (PNIO_SUBMOD_STATE_RUN);	    // system generates automatically "application ready"-telegram
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_ready_for_input_update_ind ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The PNIO controller has sent an "READY FOR INPUT UPDATE" event
    // *
    // *  NOTE:    Strongly avoid a reentrant call of UsrDbai_initiate_data_write()
    // *           or UsrDbai_initiate_data_read(), also PNIO_initiate_data_read()
    // *           or PNIO_initiate_data_write().
    // *
    // *  Input:	DevHndl        Device handle
    // *            ArNum          AR number (1...N)
    // *            InpUpdState    input update state (AR_STARTUP or AR_INDATA )
    // *  Output:   ---
    // *
    // *----------------------------------------------------------------*
    PNIO_VOID  PNIO_cbf_ready_for_input_update_ind (PNIO_UINT32 DevHndl,
                                                    PNIO_UINT16 ArNum,
                                                    PNIO_INP_UPDATE_STATE InpUpdState)
    {
        PNIO_UINT32 Status = PNIO_OK;

	    // *-----------------------------------------------------------------------
        // *    perform a first data exchange, to set IOPS and IOCS to STATE_GOOD
        // *    necessary to set IO controller state to RUN without error.
	    // *-----------------------------------------------------------------------
		Status =  PNIO_initiate_data_read (DevHndl);

		{ // Example: APDU status can read here
			PNIO_UINT32 ApduStatus = PNIO_get_last_apdu_status (PNIO_SINGLE_DEVICE_HNDL, ArNum);
			PNIO_UNUSED_ARG(ApduStatus);
		} /*lint !e438 last value assigned is not used */

		if (PNIO_OK == Status)
		{
			Status =  PNIO_initiate_data_write (DevHndl);
			if (PNIO_OK != Status)
			{
				PNIO_printf ( "##Error IO Data Exchange in PNIO_cbf_ready_for_input_update_ind()\n");
			}
		}

        PnUsr_ActivateIoDatXch();

	    // *-----------------------------------------------------------------------
        // *    notify, if first "readyForInputUpdate" after AR start or
        // *    because of replugging a submodule.
	    // *-----------------------------------------------------------------------
        if (InpUpdState == PNIO_AR_STARTUP)
        {
    		PNIO_printf ( "##READY FOR INPUT UPDATE DURING AR_STARTUP ARnum=%d\n", ArNum);
        }
        else
        {
    		PNIO_printf ( "##READY FOR INPUT UPDATE DURING AR INDATA  ARnum=%d\n", ArNum);
        }
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_ar_indata_ind ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The pnio stack notifies the user, that after start of data
    // *  exchange  a first set of input data have been received. That means,
    // *  actual input data are valid.
    // *
    // *  Input:	DevHndl        Device handle
    // *            ArNum          AR number (1...N)
    // *            SessionKey     0  (not used here)
    // *  Output:   ---
    // *
    // *----------------------------------------------------------------*
    void PNIO_cbf_ar_indata_ind (PNIO_UINT32 DevHndl,
                                 PNIO_UINT16 ArNum,
                                 PNIO_UINT16 SessionKey)
    {
	    PNIO_UNUSED_ARG (DevHndl);
	    PNIO_printf ( (PNIO_INT8*) "##AR IN-Data event indication received, ArNum = %xh, Session = %xh\n",
				      ArNum, SessionKey);
    }



    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_ar_disconn_ind ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The Device is offline. Reason code is provided by the context
    // *  manager of the pnio stack.
    // *
    // *  Input:	DevHndl        Device handle
    // *            ArNum          AR number (1...N)
    // *            SessionKey     0  (not used here)
    // *            ReasonCode	   reason code (see PNIO_AR_REASON)
    // *  Output:   ---
    // *
    // *----------------------------------------------------------------*
    void PNIO_cbf_ar_disconn_ind	 (PNIO_UINT32	 DevHndl,
                                      PNIO_UINT16    ArNum,
                                      PNIO_UINT16    SessionKey,
								      PNIO_AR_REASON ReasonCode)
    {
	    PNIO_UNUSED_ARG (DevHndl);
	    PNIO_printf ( (PNIO_INT8*) "##AR Offline indication ,ArNum = %xh, Session = %xh, Reason = %xh\n",
		              ArNum, SessionKey, ReasonCode);

        if (NumOfAr)
            NumOfAr--;      // decrement number of running ARs

        // if (NumOfAr == 0)
        //    done by stack      Bsp_EbSetLed (EB_LED_BUS_FAULT,  1);   // no more ARs running, set red LED,
    }



    // *****************************************************************************************
    // *************************** ALARM INDICATIONS  ******************************************
    // *****************************************************************************************
    void    PNIO_cbf_dev_alarm_ind (PNIO_UINT32			DevHndl,
								    PNIO_DEV_ALARM_DATA	*pAlarm)
    {
	    PNIO_printf ( (PNIO_INT8*) "##APDU alarm indication, Dev=%d, Slot=%d, Subslot=%d, AR=%d, Sess=%d\n",
                    DevHndl,
                    pAlarm->SlotNum,
                    pAlarm->SubslotNum,
                    pAlarm->ArNum,
                    pAlarm->SessionKey
                );

        PNIO_printf ( (PNIO_INT8*) "  Prio=%xh, Type=%xh, DiagCH = %d, DiagGen=%d, Tag=%d, DatLen=%d\n",
                    pAlarm->AlarmPriority,
                    pAlarm->AlarmType,
                    pAlarm->DiagChannelAvailable,
                    pAlarm->DiagGenericAvailable,
                    pAlarm->UserStructIdent,
                    pAlarm->UserAlarmDataLength
                );
    }


    // *****************************************************************************************
    // ************************  Request callback function  ************************
    // *****************************************************************************************
    void PNIO_cbf_async_req_done   (PNIO_UINT32     DevHndl,       // device handle
                                    PNIO_UINT32     ArNum,         // AR number (1...N)
                                    PNIO_ALARM_TYPE AlarmType,     // alarm type
                                    PNIO_UINT32     Api,           // API number
                                    PNIO_DEV_ADDR   *pAddr,        // location (slot, subslot)
                                    PNIO_UINT32     Status,        // status
									PNIO_UINT16		Diag_tag)
    {
       PNIO_UNUSED_ARG (DevHndl);
    	if( 0 == ArNum )
    	{
    		PNIO_printf( "Asynchronous request not sent - no existing AR. Alarmtype=%d Api=%d Slot=%d Subslot=%d\n",
    				AlarmType, Api, pAddr->Geo.Slot, pAddr->Geo.Subslot);
    	}
    	else
    	{
       PNIO_printf ( (PNIO_INT8*) "Asynchronous request ArNum=%d Alarmtype=%d Api=%d Slot=%d Subslot=%d Status = %x\n",
		              ArNum, AlarmType, Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, Status);
    	}
       PNIO_printf( "User tag %x\n", Diag_tag );
    }

    // *****************************************************************************************
    // *************************** RECORD INDICATIONS  *****************************************
    // *****************************************************************************************


    // *----------------------------------------------------------------*
    // *
    // *  PnUsr_cbf_rec_read
    // *
    // *----------------------------------------------------------------*
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32  PnUsr_cbf_rec_read
	(
		PNIO_UINT32			DevHndl,        // device handle
		PNIO_UINT32			Api,            // application process identifier
        PNIO_UINT16         ArNum,			// ar - number
        PNIO_UINT16 		SessionKey,	    // ar session number
		PNIO_UINT32			SequenceNum,    // CLRPC sequence number
		PNIO_DEV_ADDR		*pAddr,			// geographical or logical address
		PNIO_UINT32			RecordIndex,    // record index
		PNIO_UINT32			*pBufLen,		// [in, out] in: length to read, out: length, read by user
		PNIO_UINT8			*pBuffer,		// [in] buffer pointer
		PNIO_ERR_STAT		*pPnioState		// [out] 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
	)
    {
	    PNIO_UINT32   Status	    = PNIO_OK;

	    PNIO_UINT32 slot_nr		    = pAddr->Geo.Slot;
	    PNIO_UINT32 subslot_nr	    = pAddr->Geo.Subslot;
        PNIO_UINT32 DoHexdumpData   = PNIO_FALSE;

	    PNIO_UNUSED_ARG (DevHndl);
	    PNIO_UNUSED_ARG (Api);
	    PNIO_UNUSED_ARG (ArNum);
	    PNIO_UNUSED_ARG (SessionKey);
	    PNIO_UNUSED_ARG (SequenceNum);

	    // *----------------------------------------------*
	    // *  handle special indizes for I&M
	    // *----------------------------------------------*
        switch (RecordIndex)
        {
	        // *----------------------------------------------*
	        // *  read IO data fro submodule
	        // *----------------------------------------------*
            #if !INCLUDE_REC8028_8029_HANDLING
                case 0x8028: // **** read io input data from submodule *****
		                PNIO_printf ( (PNIO_INT8*) "##READ IO INPUT DATA, Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
					                 Api, slot_nr, subslot_nr, *pBufLen, SequenceNum);

                        if (pAddr->Geo.Slot != 0)   // not DAP --> normal input, output or input/output modules
                        {
                            Status = RecInpDatObjElement_Handler
                                         ( Api,                                                 // api number
                                           pAddr,                                               // slot/subslot number
                                           pBufLen,                                             // total response buffer length
                                           pBuffer,                                             // pointer to response buffer
                                           pPnioState,                                          // request stat (see pnio spec.)
                                           InDatLen[pAddr->Geo.Slot][pAddr->Geo.Subslot],       // length of io input-data block
                                           &(InData[pAddr->Geo.Slot][pAddr->Geo.Subslot][0]),   // pointer to input data block
                                           InDatIocs[pAddr->Geo.Slot][pAddr->Geo.Subslot],      // iocs value for input data block
                                           InDatIops[pAddr->Geo.Slot][pAddr->Geo.Subslot]       // iops value for input data block
                                          );
                        }
                        else
                        {  // DAP --> no input or output data in this example
                            PNIO_UINT8 Dummy = 0;
                            Status = RecInpDatObjElement_Handler
                                         ( Api,                                                 // api number
                                           pAddr,                                               // slot/subslot number
                                           pBufLen,                                             // total response buffer length
                                           pBuffer,                                             // pointer to response buffer
                                           pPnioState,                                          // request stat (see pnio spec.)
                                           0,                                                   // length of io input-data block
                                           &Dummy,                                              // any dummy pointer
                                           PNIO_S_GOOD,                                         // we assume: always good in this example
                                           PNIO_S_GOOD                                          // we assume: always good in this example
                                          );
                        }
                     break;

                case 0x8029: // **** read io output data from submodule + substituted values *****
		                PNIO_printf ( (PNIO_INT8*) "##READ IO OUTPUT DATA and SUBSTITUTED VALUES, Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
					                 Api, slot_nr, subslot_nr, *pBufLen, SequenceNum);
                        if (pAddr->Geo.Slot != 0)   // not DAP --> normal input, output or input/output modules
                        {
                            Status = RecOutDatObjElement_Handler
                                         ( Api,                                                   // api number
                                           pAddr,                                                 // slot/subslot number
                                           pBufLen,                                               // total response buffer length
                                           pBuffer,                                               // pointer to response buffer
                                           pPnioState,                                            // request stat (see pnio spec.)
                                           OutDatLen[pAddr->Geo.Slot][pAddr->Geo.Subslot],        // length of io input-data block
                                           &(OutData[pAddr->Geo.Slot][pAddr->Geo.Subslot][0]),    // pointer to input data block
                                           OutDatIocs[pAddr->Geo.Slot][pAddr->Geo.Subslot],       // iocs value for input data block
                                           OutDatIops[pAddr->Geo.Slot][pAddr->Geo.Subslot],       // iops value for input data block
                                           &(OutSubstVal[pAddr->Geo.Slot][pAddr->Geo.Subslot][0]),// pointer to substituted values                                          // pointer to substituted values for output data
                                           2,                                                     // SubstitutionMode   = 0: ZERO,  1: last value, 2: replacement value (see pnio-spec)
                                           0                                                      // SubstitutionActive = 0: output values active, 1: substituted values active
                                          );
                        }
                        else
                        {  // PDEV port: no io data, iops = iocs = GOOD
                            PNIO_UINT8 Dummy = 0;
                            Status = RecOutDatObjElement_Handler
                                         ( Api,                                                   // api number
                                           pAddr,                                                 // slot/subslot number
                                           pBufLen,                                               // total response buffer length
                                           pBuffer,                                               // pointer to response buffer
                                           pPnioState,                                            // request stat (see pnio spec.)
                                           0,                                                     // length of io input-data block
                                           &Dummy,                                                // any dummy pointer to input data block
                                           PNIO_S_GOOD,                                           // we assume: always good in this example
                                           PNIO_S_GOOD,                                           // we assume: always good in this example
                                           &Dummy,                                                // dummy pointer to input data block
                                           2,                                                     // SubstitutionMode   = 0: ZERO,  1: last value, 2: replacement value (see pnio-spec)
                                           0                                                      // SubstitutionActive = 0: output values active, 1: substituted values active
                                          );
                        }
                     break;
            #endif

	        // *----------------------------------------------*
	        // *  user defined records  index 0..7fffh
	        // *----------------------------------------------*
            case 2: // **** user specific demo record *****
		            PNIO_printf ( (PNIO_INT8*) "##READ Demo-Record , Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
					             Api, slot_nr, subslot_nr, *pBufLen, SequenceNum);

                    if (*pBufLen > sizeof(DEMO_RECORD))
                        *pBufLen = sizeof (DEMO_RECORD);

                    OsMemCpy (pBuffer, DEMO_RECORD, *pBufLen);
                    break;

            case 1: // **** user specific startup record *****
                    PNIO_printf ( (PNIO_INT8*) "##READ startup-Record (here: demo record is returned), Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
                                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen, SequenceNum);
                    if (*pBufLen > sizeof(DEMO_RECORD))
                        *pBufLen = sizeof (DEMO_RECORD);

                    OsMemCpy (pBuffer, DEMO_RECORD, *pBufLen);
                    break;

			#if EXAMPLE_IPAR_SERVER
			case EXAMPL_IPAR_REC_IND: // **** read example data for the iPar Server *****

					 *pBufLen = 4;
					 OsMemCpy ( pBuffer,
								&Example_iParData[slot_nr][subslot_nr],
								*pBufLen);
					 PNIO_printf ( (PNIO_INT8*) "##READ iPar example data, Api=%d Slot=%d Subslot=%d Len=%d, RecordIndex=%d\n",
								  Api, slot_nr, subslot_nr, *pBufLen, RecordIndex);
					 DoHexdumpData = PNIO_TRUE;
				 break;
			#endif

            default:
                // *----------------------------------------------*
		        // *  all record number < 0x7fff are user specific,
		        // *  but we intercept the simatic specific rec-read
                // *  request on slot=0, subslot=1, index={0,1}, which
                // *  is not supported here.
		        // *----------------------------------------------*
            	if ( RecordIndex <= 0x7fff)
            	{
            		if (   ( (pAddr->Geo.Slot == 0) && (pAddr->Geo.Subslot == 1))
            			|| (pAddr->Geo.Subslot >= 200))
            		{   // ** we do not support that request, may be a SIEMENS-specific call      **
            			// ** we respond with an adequate error message, specified in IEC 61158-6 **
            			pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
            			pPnioState->ErrDecode = 0x80;  // PNIORW
            			pPnioState->ErrCode1  = 0xa9;  // example: Error Class 10 = application, ErrorNr 9 = "feature not supported"
            			return (PNIO_NOT_OK);
            		}
            		else
            		{
            			// *----------------------------------------------*
            			// *  read manufacturer specific record data,
            			// *  implemented by the user
            			// *----------------------------------------------*
            			PNIO_UINT8    ReadRecDummyData[] = {"**Data1234 ReadRecord**"};

            			// **** copy dummy data into the buffer, set data-size ***

            			if (*pBufLen > sizeof (ReadRecDummyData))
            			{
            				*pBufLen = sizeof (ReadRecDummyData);
            			}
            			OsMemCpy (pBuffer, ReadRecDummyData, *pBufLen);
            			PNIO_printf ( (PNIO_INT8*) "##READ_REC RQ,  Api =%d Slot=%d Subslot=%d Index=0x%x, Len=%d, Sequ_nr=%d\n",
				                        Api, slot_nr, subslot_nr, RecordIndex, *pBufLen, SequenceNum);
            		}
            	}
            	else
            	{
		            // *----------------------------------------------*
		            // *  all other record indizes >= 0x8000
                    // *  may be imlemented by user, otherwise
                    // *  we return "invalid index"
		            // *----------------------------------------------*
		            PNIO_printf ( (PNIO_INT8*) "##READ_REC RQ, Api=%d Slot=%d Subslot=%d Index=0x%x, Len=%d, Sequ_nr=%d\n",
					             Api, slot_nr, subslot_nr, RecordIndex, *pBufLen, SequenceNum);

	                // *** if an error occured, it must be specified according IEC 61158-6
                    pPnioState->ErrCode   = 0xde;  // IODReadRes with ErrorDecode = PNIORW
                    pPnioState->ErrDecode = 0x80;  // PNIORW
                    pPnioState->ErrCode1  = 0xb0;  // example: Error Class 11 = access, ErrorNr 0 = "invalid index"
                    pPnioState->ErrCode2  = 0;     // here dont care
	                pPnioState->AddValue1 = 0;     // here dont care
	                pPnioState->AddValue2 = 0;     // here dont care
	                return (PNIO_NOT_OK);
	            }
                break;
        }


        // **** print data as hexdump ****
        if (DoHexdumpData)
        {
            PNIO_UINT32 i;
            PNIO_UINT8* p8 = (PNIO_UINT8*) pBuffer;
            PNIO_printf ( (PNIO_INT8*) "##REC_DATA = ");
            for (i=0; i < *pBufLen; i++)
            {
                PNIO_UINT32  Val;
                Val = (PNIO_UINT32 ) *(p8+i);;
                PNIO_printf ( (PNIO_INT8*) "0x%02x ", Val);
            }

            PNIO_printf ( (PNIO_INT8*) "\n\n");
        }



      return (Status);
    }



    // *----------------------------------------------------------------*
    // *
    // *  PnUsr_cbf_rec_write ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32  PnUsr_cbf_rec_write
	(
		PNIO_UINT32			DevHndl,        // device handle
		PNIO_UINT32			Api,            // application process identifier
        PNIO_UINT16         ArNum,          // ar - number
        PNIO_UINT16 		SessionKey,	    // ar session number
		PNIO_UINT32			SequenceNum,    // CLRPC sequence number
		PNIO_DEV_ADDR		*pAddr,			// geographical or logical address
		PNIO_UINT32			RecordIndex,    // record index
		PNIO_UINT32			*pBufLen,   	// [in, out] in: length to write, out: length, written by user
		PNIO_UINT8			*pBuffer,		// [in] buffer pointer
		PNIO_ERR_STAT		*pPnioState		// 4 byte PNIOStatus (ErrCode, ErrDecode, ErrCode1, ErrCode2), see IEC61158-6
	)

    {
	    // **** copy dummy data into the buffer, set data-size ***
	    PNIO_UINT8      WriteRecDummyData[50];
        PNIO_UINT32     i;
	    PNIO_UINT32     Status = PNIO_OK;
        PNIO_UINT32     DoHexdumpData = PNIO_TRUE;

	    PNIO_UNUSED_ARG (DevHndl);
	    PNIO_UNUSED_ARG (Api);
	    PNIO_UNUSED_ARG (ArNum);
	    PNIO_UNUSED_ARG (SessionKey);
	    PNIO_UNUSED_ARG (SequenceNum);

	    // *** check data size (accepted data >= provided data ??) ***
	    if (*pBufLen > sizeof (WriteRecDummyData))
	    {
	       PNIO_printf ("Error: provided record data (%d) index = %d  > expected data (%d)\n",
	                    RecordIndex,
	                    *pBufLen,
	                    sizeof (WriteRecDummyData));
	       return (PNIO_NOT_OK);
	    }

        switch (RecordIndex)
        {
            // *----------------------------------------------*
	        // *  user defined records  index 0..7fffh
	        // *----------------------------------------------*
            #if EXAMPLE_IPAR_SERVER
                case EXAMPL_IPAR_REC_IND: // **** read example data for the iPar Server *****
		                PNIO_printf ( (PNIO_INT8*) "##download iPar example, Api=%d Slot=%d Subslot=%d Len=%d, RecordIndex=%d\n",
					                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen, RecordIndex);

                        // ***** save record data for the specified slot/subslot ****
                        OsMemCpy (&Example_iParData[pAddr->Geo.Slot][pAddr->Geo.Subslot],
				                  pBuffer,
				                  4);
                     break;
            #endif
            case 2: // **** user specific demo record *****
		            PNIO_printf ( (PNIO_INT8*) "##WRITE Demo-Record , Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
					             Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen, SequenceNum);
                    break;

            case 1: // **** user specific startup record *****
                    PNIO_printf ( (PNIO_INT8*) "##WRITE startup-Record from GSD file , Api=%d Slot=%d Subslot=%d Len=%d, Sequ_nr=%d\n",
                                 Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, *pBufLen, SequenceNum);
                    break;
            default:
                 Status = PNIO_NOT_OK;
                 break;
        }



        if (Status == PNIO_OK)
        {
	        // *** copy the record data into a buffer for further use **
	        OsMemCpy (WriteRecDummyData,		// destination pointer for record data
			        pBuffer,				// source pointer for record data
			        *pBufLen);			    // length of the accepted data


            // **** print data as hexdump ****
            if (DoHexdumpData)
            {
	            PNIO_printf ( (PNIO_INT8*) "##WRITE_REC RQ, Api=%d Slot=%d Subslot=%d Index=0x%x, Len=%d, Sequ_nr=%d\n",
				             Api, pAddr->Geo.Slot, pAddr->Geo.Subslot, RecordIndex, *pBufLen, SequenceNum);

                PNIO_printf ( (PNIO_INT8*) "##REC_DATA = ");
                for (i=0; i < *pBufLen; i++)
                {
                    PNIO_UINT32  Val;
                    Val = (PNIO_UINT32 ) WriteRecDummyData[i];
                    PNIO_printf ( (PNIO_INT8*) "0x%02x ", Val);
                }

                PNIO_printf ( (PNIO_INT8*) "\n\n");
            }
        }
        else
	    {// *** if an error occured, it must be specify  according to IEC 61158-6
            pPnioState->ErrCode   = 0xdf;  // IODWriteRes with ErrorDecode = PNIORW
            pPnioState->ErrDecode = 0x80;  // PNIORW
            pPnioState->ErrCode1  = 0xb6;  // example: Error Class 11 = Access, ErrorNr 6 = "access denied"
            pPnioState->ErrCode2  = 0;     // here dont care
            pPnioState->AddValue1 = 0;     // here dont care
            pPnioState->AddValue2 = 0;     // here dont care
	    }

        return (Status);   // OK: function executed, NOT_OK: error occured
    }





    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_save_station_name ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The DCP package of the pnio stack has got a "set station name"
    // *  request from the pnio controller and notifies the user by this
    // *  function.
    // *  The user has to save this name in a non volatile memory (for example
    // *  a nv-ram), if the Remanent-parameter is <> 0.
    // *
    // *  manager of the pnio stack.
    // *
    // *  Input:	PNIO_UINT8      *pStationName   name string,
    // * 			PNIO_UINT16     NameLength	   length of the name string
    // * 			PNIO_UINT8      Remanent        <> 0: save remanent
    // *
    // *
    // *  Output:   return          PNIO_OK: function completed successfully
    // *	                        PNIO_NOT_OK: function could not be completed
    // *----------------------------------------------------------------*
    PNIO_UINT32  PNIO_cbf_save_station_name
                        (	PNIO_INT8    *pStationName,
						    PNIO_UINT16   NameLength,
						    PNIO_UINT8   Remanent )
    {
       if (Remanent)
       {
	      PNIO_printf ( (PNIO_INT8*) "##Save remanent: ");

          // **** store data in non volatile memory ****
          Bsp_nv_data_store (PNIO_NVDATA_DEVICENAME,    // nv data type: device name
                             pStationName,              // source pointer to the devicename
                             NameLength);               // length of the device name
       }

       else
       {
           PNIO_printf ( (PNIO_INT8*) "Station Name = %.*s  received, Length=%d  Remanent = %d\n",
	                     NameLength, pStationName, NameLength, Remanent);

           Bsp_nv_data_store (PNIO_NVDATA_DEVICENAME,    // nv data type: device name
                              "",                        // source pointer to the devicename
                              0);               // length of the device name
       }

       // *-----------------------------------------------------------------*
       // * return PNIO_OK:     name modification is accepted by application
       // *        PNIO_NOT_OK: name modification is denied   by application
       // *-----------------------------------------------------------------*
       return (PNIO_OK);
    }



    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_save_ip_addr()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  The DCP package of the pnio stack has got a "set ip address"
    // *  request from the pnio controller and notifies the user by this
    // *  function.
    // *  The user has to save the ip address, subnet mask and default router
    // *  address in a non volatile memory (for example a nv-ram), if the
    // *  Remanent-parameter is <> 0.
    // *
    // *  manager of the pnio stack.
    // *
    // *  Input:	PNIO_UINT32      NewIpAddr      new ip addres
    // * 			PNIO_UINT32      SubnetMask     Subnet mask
    // * 			PNIO_UINT32      DefRouterAddr  default gateway
    // * 			PNIO_UINT8       Remanent       <> 0: save remanent,
    // *                                            == 0: do not save remanent
    // *
    // *
    // *  Output:   return		    PNIO_OK: function completed successfully
    // *	                        PNIO_NOT_OK: function could not be completed
    // *
    // *----------------------------------------------------------------*
    PNIO_UINT32 PNIO_cbf_save_ip_addr
                                 (  PNIO_UINT32 NewIpAddr,
                                    PNIO_UINT32 SubnetMask,
                                    PNIO_UINT32 DefRouterAddr,
                                    PNIO_UINT8  Remanent)
    {
       NV_IP_SUITE  *IpSuite;

       OsAllocFX( (void **) &IpSuite, sizeof(NV_IP_SUITE), 0);

       if (Remanent)
       {
          PNIO_printf ( (PNIO_INT8*) "##Remanent address was stored \n");

          // **** save ip suite in non volatile memory ****
          IpSuite->IpAddr     = NewIpAddr;
          IpSuite->SubnetMask = SubnetMask;
          IpSuite->DefRouter  = DefRouterAddr;
          Bsp_nv_data_store (PNIO_NVDATA_IPSUITE,      // nv data type: ip suite
                             IpSuite,                  // source pointer
                             sizeof (NV_IP_SUITE));               // length of the device name

       }
       else
       {
            // *-----------------------------------------------------
            // *  according to the PNIO SPEC an already (remanent)
            // *  stored IP address must be deleted, if a new ip address
            // *  is set non remanent.
            // *-----------------------------------------------------
            IpSuite->IpAddr     = 0;
            IpSuite->SubnetMask = 0;
            IpSuite->DefRouter  = 0;
            Bsp_nv_data_store (PNIO_NVDATA_IPSUITE,       // nv data type: ip suite
                               IpSuite,                  // source pointer
                               sizeof (NV_IP_SUITE));     // length of the device name

            PNIO_printf ( (PNIO_INT8*) "Remanent address was deleted \n");
       }

       OsFree(IpSuite);

       return (PNIO_OK);

    }

    /**
     *  @brief there is new ip data
     *
     *
     *  information about new ip, mask and gateway
     *	only printed to console to show the data
     *
     */
    PNIO_UINT32 PNIO_cbf_report_new_ip_addr( PNIO_UINT32 NewIpAddr,
    								    	 PNIO_UINT32 SubnetMask,
											 PNIO_UINT32 DefRouterAddr)
    {
    	PNIO_printf("IP address = %xh, Subnet mask = %xh, Default router = %xh\n", NewIpAddr, SubnetMask, DefRouterAddr);

    	return (PNIO_OK);
    }

    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_start_led_blink ()
    // *
    // *----------------------------------------------------------------*
    // *
    // *  Start blinking with LED after DCP Request (received from the
    // *  engineering tool
    // *  function.
    // *  The user has to start blinking with the specified LED
    // *
    // *  Input:		PNIO_UINT32    DevHndl     Device handle
    // *                PNIO_UINT32    PortNum     port number  1..N
    // *                PNIO_UINT32    frequency   blinking frequency
    // *  Output:       return         must be PNIO_OK
    // *----------------------------------------------------------------*
    PNIO_UINT32 PNIO_cbf_start_led_blink (PNIO_UINT32 DevHndl,
                                          PNIO_UINT32 PortNum,
                                          PNIO_UINT32 frequency)
    {
        PNIO_UNUSED_ARG (DevHndl);
        PNIO_printf ( (PNIO_INT8*) "##LED Blink START, Port = %d, frequency = %d Hz \n", PortNum, frequency);
        BspLed_StartLedBlink(frequency);
        return (PNIO_OK);    // must be PNIO_OK, other's not possible
    }



    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_stop_led_blink ()
    // *
    // *----------------------------------------------------------------*
    // *  Stop blinking with LED (started after DCP Request)
    // *
    // *  Input:		PNIO_UINT32    DevHndl     Device handle
    // *                PNIO_UINT32    PortNum     port number  1..N
    // *  Output:       return         must be PNIO_OK
    // *----------------------------------------------------------------*
    PNIO_UINT32 PNIO_cbf_stop_led_blink (PNIO_UINT32 DevHndl,
                                         PNIO_UINT32 PortNum)
    {
        PNIO_UNUSED_ARG (DevHndl);
        PNIO_printf ( (PNIO_INT8*) "##LED Blink STOP, Port = %d\n", PortNum);
        BspLed_StopLedBlink ();
        return (PNIO_OK);    // must be PNIO_OK, other's not possible
    }


    // *----------------------------------------------------------------*
    // *
    // *  PNIO_cbf_reset_factory_settings ()
    // *
    // *----------------------------------------------------------------*
    // *  advises the application, to reset to factory settings
    // *
    // *  Input:	PNIO_UINT32     DevHndl     Device handle
    // *  Input:	PNIO_RTF_OPTION RtfOption   specifies, which data have
    // *                                        to be resetted
    // *  Output:   return          must be PNIO_OK
    // *----------------------------------------------------------------*
    PNIO_UINT32 PNIO_cbf_reset_factory_settings (PNIO_UINT32 DevHndl, PNIO_RTF_OPTION RtfOption)
    {
        PNIO_UNUSED_ARG (DevHndl);

        Bsp_nv_data_clear (RtfOption);

        return (PNIO_OK);    // must be PNIO_OK, other's not possible

    }


    #if INCLUDE_REC8028_8029_HANDLING
        // **** special case:   read record index 0x8029  (read output data)
        PNIO_IOXS    PNIO_cbf_substval_out_read  // read substitution values for output submodule
                (
                 PNIO_UINT32    DevHndl,            // [in] Handle for Multidevice
                 PNIO_DEV_ADDR  *pAddr,             // [in] geographical or logical address
                 PNIO_UINT32    BufLen,             // [in] length of the submodule output substitute data
                 PNIO_UINT8     *pBuffer,           // [in] Ptr to submodule output substitute data
                 PNIO_UINT16*   pSubstMode,         // [in, out - BIG ENDIAN] SubstitutionMode: 0=ZERO or inactive (default), 1:LastValue, 2:Replacement value SubstitutionMode: 0=ZERO or inactive, 1:LastValue, 2:Replacement value
                 PNIO_UINT16*   pSubstActive        // [in, out - BIG ENDIAN] SubstituteActiveFlag:  0=operation, 1=substitute. default value is 0: if (IOPS & IOCS = GOOD), else: 1
                )
        {

           // *** set the substitute data  ***
           OsMemSet (pBuffer, 0xab, BufLen);    // example: all substitute data are 0xab (the user will change that...)

           // *** *pSubstMode = 2;      // we accept the preset value from the stack (set mode to "replace"), so nothing to do here"
           //*pSubstMode = 2;

           // *** we accept the preset value from stack:  "0 = Operation", if (remoteIOPS == GOOD and localIOCS == GOOD),  else "1 = Substitute"
           //*pSubstActive = REC8029_SUBSTSTATE_OPERATION;

           // we assume, the output data set is valid
           return (PNIO_S_GOOD);
        }
    #endif

/**
 * @brief User functionality as reaction to new ARFSU
 *
 * @param[in]		  ARFSU_enabled      		PNIO_ARFSU_ENABLED, PNIO_ARFSU_DISABLED
 * @param[in]         ARFSU_changed        		PNIO_ARFSU_CHANGED, PNIO_ARFSU_NOT_CHANGED
 *
 * @return            void
 *
 * 	This function is called when ARFSU write record is recieved
 * 	Function is ready for user functionality
 * 	Informs user if ARFSU UUID was changed or not -> if parameterization was changed
 *
 */
PNIO_VOID PNIO_cbf_report_ARFSU_record(
		PNIO_UINT8			ARFSU_enabled,
		PNIO_UINT8			ARFSU_changed
		)
{
	if( PNIO_ARFSU_CHANGED == ARFSU_changed )
	{
		PNIO_printf( "New ARFSU UUID -> parameterization of device was changed\n" );
	}
}


#endif

/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2014 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
