/*---------------------------------------------------------------------------*/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All Rights Reserved.      */
/*  This program is protected by German copyright law and international      */
/*  treaties.                                                                */
/*  The use of this software including but not limited to its Source Code    */
/*  is subject to restrictions as agreed in the license agreement between    */
/*  you and Siemens.                                                         */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*---------------------------------------------------------------------------*/
/*  Project             : PROFInet Runtime Software                          */
/*  File                : Tcp_IF.h                                           */
/*  Version             : V4.3                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Example : TCP INTERFACE                                                  */
/*            performs point to point TCP connection to a remote device      */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*---------------------------------------------------------------------------*/

#ifndef _TCP_IF_H
#define _TCP_IF_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif
// *---------------------------------------------------------------------------*/
// *  TCP_IF.h
// *  
// *  
// *  
// *  
// *  
// *---------------------------------------------------------------------------*/
#define TCP_SOCK_ERROR   -1


int tcp_if_inits        (void);
unsigned int tcp_if_connectC    (int LocalSockId,  unsigned int   RemIpAddr,  unsigned int Port);
         int tcp_if_connectS    (int LocalSockId,  unsigned int   RemIpAddr,  unsigned int Port);
unsigned int tcp_if_send        (int RemoteSockId, unsigned char* pDat,       unsigned int DatLen);
unsigned int tcp_if_receive     (int RemoteSockId, unsigned char* pDat,       unsigned int MaxDatLen);
unsigned int tcp_if_disconnect  (int RemoteSockId);  
unsigned int tcp_if_close       (int LocalSockId);




#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif  // __cplusplus

#endif  // _T_UTILS_H

