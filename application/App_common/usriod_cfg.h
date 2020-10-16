#ifndef _USR_IOD_CFG_H
#define _USR_IOD_CFG_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif
/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2013 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : usriod_cfg.h                                       */
/*  Version             : V4.1                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  configuration file for the PNIO stack                                    */
/*  has to be modified by the user.                                          */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "usrapp_cfg.h"   // example application spec. configuration settings

// *-----------------------------------------------------------
// *  Fast-Startup (FSU) Support (defined on application level)
// *  Note: for FSU  the REMA support is mandatory,
// *  i.e. #define INCLUDE_REMA   1   in compiler.h must be set
// *-----------------------------------------------------------
#define IOD_FSU_SUPPORTED   	 1	// 1: activate FSU capability in PNIO stack
#define IOD_FSU_DISABLE_PRINTF   0	// 1: disable printf at startup, to increase startup speed

#if (IOD_FSU_SUPPORTED)
    #if (INCLUDE_REMA != 1)
        #error ("REMA must be supported for FSU !!")
    #endif
#endif


// *--------------------------------------------------------------------
// *   SNMP MIB2 configuration values for
// *    - sysDescr      system description
// *    - interface description
// *    - interface name
// *--------------------------------------------------------------------

#if (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB400)
    // *=============================================
    // * settings for EB400
    // *=============================================

    // *-------------------------
    // *    snmp data
    // *-------------------------
    #define SNMP_INTERFACE_DESCRIPTION  "ERTEC 400 based PNIO device"
    #define SNMP_SYSTEM_DESCRIPTION     "Siemens PNIO device kit software, including ERTEC RT and IRT example"
#endif

#if (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB200)
    // *=============================================
    // * settings for DK_ERTEC and SK_ERTEC
    // *=============================================

    // *-------------------------
    // *    snmp data
    // *-------------------------
    #define SNMP_INTERFACE_DESCRIPTION "ERTEC 200 based PNIO device"
    #define SNMP_SYSTEM_DESCRIPTION    "Siemens PNIO device kit software, including ERTEC RT and IRT example"
#endif

#if (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB200P)
    // *=============================================
    // * settings for EK_ERTEC 200P
    // *=============================================

    // *-------------------------
    // *    snmp data
    // *-------------------------
    #define SNMP_INTERFACE_DESCRIPTION "ERTEC 200P based PNIO device"
    #define SNMP_SYSTEM_DESCRIPTION    "Siemens PNIO evaluation kit ERTEC200P software, including ERTEC RT and IRT example"
    #define SNMP_SYSTEM_CONTACT        "PROFINET competence center ComDeC, +49 911 750 2080, www.siemens.com/ComDeC"
    #define SNMP_SYSTEM_LOCATION       "Germany, 90766 Fuerth, Wuerzburger Strasse 121"
#endif

#if (PNIOD_PLATFORM &  PNIOD_PLATFORM_DKSW)
    // *=============================================
    // * settings for DK Standard Ethernet Controller
    // *=============================================

    // *-------------------------
    // *    snmp data
    // *-------------------------
    #define SNMP_INTERFACE_DESCRIPTION  "DK_SW"
    #define SNMP_SYSTEM_DESCRIPTION     "Siemens DEVKIT DK-SW 6ES7195-3BC00-0YA0"
    #define SNMP_SYSTEM_CONTACT        "PROFINET competence center ComDeC, +49 911 750 2080, www.siemens.com/ComDeC"
    #define SNMP_SYSTEM_LOCATION       "Germany, 90766 Fuerth, Wuerzburger Strasse 121"
#endif


// *---------------------------------------------------------
// * set maximum number of slots subslots,
// * that can be handled by the PNIO stack
// * (defines allocated memory inside the stack for managing
// * slots and subslots)
// *---------------------------------------------------------
#define IOD_CFG_NUMOF_SUBSLOTS		    64


// *-----------------------------------------------------------------------
// *    device annotation
// *
// *-----------------------------------------------------------------------

#if (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB400) // EB400
    #define DEV_ANNOTATION_ORDER_ID     "6ES7 195-3BH00-0YA0"    // example for order ID
#elif  (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB200) // EB200
    #define DEV_ANNOTATION_ORDER_ID     "6ES7 195-3BG00-0YA0"    // example for order ID
#elif  (PNIOD_PLATFORM &  PNIOD_PLATFORM_EB200P) // EB200P
    #define DEV_ANNOTATION_ORDER_ID     "6ES7 195-3BE00-0YA0"    // example for order ID
#elif  (PNIOD_PLATFORM &  PNIOD_PLATFORM_DKSW) // DKSW
    #define DEV_ANNOTATION_ORDER_ID     "6ES7 195-3BC00-0YA0"    // example for order ID
#else
    #error ("no valid platform")
#endif

#define DEV_ANNOTATION_HW_REVISION_ERTEC200P      DEVKIT_HW_REVISION_ERTEC200P       // PNIO_UINT16     HwRevision;          // IM_Hardware_Revision
#define DEV_ANNOTATION_HW_REVISION_ERTEC200P2     DEVKIT_HW_REVISION_ERTEC200P2       // PNIO_UINT16     HwRevision;          // IM_Hardware_Revision
                         // example for HW Revision



// *-----------------------------------------------------------------------
// *    I&M0  data structure definition
// *    I&M1  preset values (after factory reset)
// *    I&M2  preset values (after factory reset)
// *    I&M3  preset values (after factory reset)
// *    I&M4  preset values (after factory reset)
// *    (details see PROFINET IO specification)
// *-----------------------------------------------------------------------
#define IM0_SERIAL_NUMBER      "1234567890      "   // must be 16 Bytes, fillbyte = <blank>

#define IM0_REVCNT              1                   // 0.. 0xffff
#define IM0_PROFID              0                   // 0.. 0xffff, Using shall be defined by profiles
#define IM0_PROFSPECTYP         3                   // if ProfId = 0, ProfSpecTyp must be 1..6, e.g. 3="io module"



/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2013 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif

