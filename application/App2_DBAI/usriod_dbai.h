#ifndef _USRIOD_DBAI_H
#define _USRIOD_DBAI_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif
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
/*  File                : usriod_dbai.h                                      */
/*  Version             : V3.2                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  configuration file for the user application, only needed for the         */
/*  DBAI example when #define  EXAMPL_DEV_CONFIG_VERSION is set to value  11 */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
typedef struct
{
    PNIO_IO_PROP_ENUM   IoProp;
    PNIO_UINT32 Api;
    PNIO_UINT32 Slot;
    PNIO_UINT32 Sub;
    PNIO_UINT32 InDatOffs;
    PNIO_UINT32 OutDatOffs;
    PNIO_UINT32 InDatLen;
    PNIO_UINT32 OutDatLen;
    PNIO_UINT32 InIopsOffs;
    PNIO_UINT32 InIocsOffs;
    PNIO_UINT32 OutIopsOffs;
    PNIO_UINT32 OutIocsOffs;
} USRIOD_SUBMOD;


typedef struct
{
    PNIO_UINT32             IocrValid;
    PNIO_BUFFER_LOCK_TYPE	InCr;
    PNIO_BUFFER_LOCK_TYPE	OutCr;
    PNIO_UINT32             NumOfSub;
    USRIOD_SUBMOD*          pSubList;            
} USRIOD_DBAI_IOCR;


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif

