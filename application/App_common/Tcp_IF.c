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
/*  File                : Tcp_IF.c                                           */
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

#include "compiler.h"
#ifndef LSAS_CFG_USE_EXTERNAL_TCPIP_STACK
    #define SOCKET      PNIO_UINT32
    #include "os_taskprio.h"


    #undef  INVALID_SOCKET
    #undef  SOCKET_ERROR
    #undef  SOL_SOCKET

    #include "tcpapp.h"
    #include "bsdsock.h"
    #include "tcp.h"
    #include "Tcp_IF.h"
	#include "pniousrd.h"

    #define BUFFER_SIZE     1024

    #define LTRC_ACT_MODUL_ID   12


    // *----------------------------------------------------------------*
    // *    
    // *   () tcp_if_inits (void)
    // *    
    // *----------------------------------------------------------------*
    // *  initializes the interface, creates the tcp socket
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     --
    // *----------------------------------------------------------------*
    int tcp_if_inits (void)
    {
        PNIO_INT32       Optval = 1;     //Set an boolean socket option
        PNIO_INT32       Error;
        
	    // *--------------------------------------------------
	    // * create socket
	    // *--------------------------------------------------
	    int LocalSockId = t_socket(AF_INET, SOCK_STREAM, 0);
	    if (LocalSockId == INVALID_SOCKET)
	    {
		    PNIO_printf ( (PNIO_INT8*) "socket() failed");
            LSA_TRACE_00  (TRACE_SUBSYS_APPL_PLATFORM, LSA_TRACE_LEVEL_FATAL, "ERROR socket open\n" );
            return (PNIO_NOT_OK);      
	    }

        Error = t_setsockopt (  LocalSockId, 
                                SOL_SOCKET, 
                                SO_NONBLOCK, 
                                (char*) &Optval,
                                 sizeof(PNIO_INT32)); 
        
        if ( (Error!= 0) || (LocalSockId == INVALID_SOCKET )) 
        {
            // easy...
            LSA_TRACE_00  (TRACE_SUBSYS_APPL_PLATFORM, LSA_TRACE_LEVEL_FATAL, "ERROR set socket options\n" );
            return (PNIO_NOT_OK);      
        }
	    
        return (LocalSockId);
    }


    // *----------------------------------------------------------------*
    // *    
    // *   ()
    // *    
    // *----------------------------------------------------------------*
    // *  
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     --
    // *----------------------------------------------------------------*
    unsigned int tcp_if_connectC (int            LocalSockId,
                                  unsigned int   RemIpAddr,  
                                  unsigned int   Port)
    {
	    struct   sockaddr_in srv_addr;       // server address
        srv_addr.sin_addr.s_addr = (RemIpAddr == 0) ? INADDR_ANY : OsHtonl (RemIpAddr);
        srv_addr.sin_port = OsHtons((PNIO_UINT16)Port);
	    srv_addr.sin_family = AF_INET;
	    
	    // *--------------------------------------------------
	    // * socket an lokalen IP Port (und remote IP Addr.) binden
	    // *--------------------------------------------------
        if (t_connect(LocalSockId,  (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1)
	    {
		    PNIO_printf( (PNIO_INT8*) "connect() failed");
		    return (PNIO_FALSE);
	    }
	    
        return (PNIO_TRUE);
    }

    // *----------------------------------------------------------------*
    // *    
    // *   tcp_if_connectS (unsigned int   RemIpAddr,unsigned int   Port()
    // *    
    // *----------------------------------------------------------------*
    // *  opens a socket for tcp server and waits on connection
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     RemIpAddr    Ip addr in 32 bit network format
    // *             Port         local Tcp Port number
    // *  return:                 PNIO_OK, PNIO_NOT_OK
    // *----------------------------------------------------------------*
    int tcp_if_connectS (int            LocalSockId,
                                  unsigned int   RemIpAddr,  
                                  unsigned int   Port)
    {
        struct   sockaddr_in cli_addr;       // client address
	    struct   sockaddr_in srv_addr;       // server address
        int      cli_size;                   // sizeof (cli_addr)
        int      RemoteSockId;
        srv_addr.sin_addr.s_addr = (RemIpAddr == 0) ? INADDR_ANY : OsHtonl (RemIpAddr);
	    srv_addr.sin_port = OsHtons((PNIO_UINT16)Port);
	    srv_addr.sin_family = AF_INET;

	    // *--------------------------------------------------
	    // * socket an lokalen IP Port (und remote IP Addr.) binden
	    // *--------------------------------------------------
        if (t_bind(LocalSockId,  (struct sockaddr *) &srv_addr, sizeof(srv_addr)) == -1)
	    {
		    PNIO_printf( (PNIO_INT8*) "bind() failed\n");
		    return (TCP_SOCK_ERROR);
	    }

	    // *--------------------------------------------------
	    // * create a number of queues for incoming connection
        // * requests on this port
	    // *--------------------------------------------------
        if (t_listen(LocalSockId, 3) == -1)
	    {
		    PNIO_printf( (PNIO_INT8*) "listen() failed\n");
		    return (TCP_SOCK_ERROR);
	    }

        cli_size = sizeof(cli_addr);

	    // *--------------------------------------------------
	    // * eingehenden Verbindungswunsch akzeptieren
	    // *--------------------------------------------------
		RemoteSockId = -1;
		while (RemoteSockId == -1)
		{
	        RemoteSockId = t_accept(LocalSockId, (struct sockaddr *)&cli_addr, &cli_size);
	        if (RemoteSockId == -1)
	            OsWait_ms (100);
		}

		return (RemoteSockId);
    }

    // *----------------------------------------------------------------*
    // *    
    // *   tcp_if_send   (unsigned char* pDat,  unsigned int DatLen)()
    // *    
    // *----------------------------------------------------------------*
    // *   sends the specified data to the remote device
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     pDat       pointer to data buffer
    // *             DatLen     length of data in bytes
    // *  return                -1:  error   else: length of transferred data
    // *----------------------------------------------------------------*
    unsigned int tcp_if_send   (int RemoteSockId, unsigned char* pDat,  unsigned int DatLen)
    {
        int bytes = 0;
        
        if (DatLen == 0)
            return (0);
        
        while (bytes == 0)
        {
            bytes = t_send(RemoteSockId, (char*)pDat, DatLen, 0);
            if (bytes == 0)
                OsWait_ms (100);
            else
                OsWait_ms (10);
        }
        return (bytes);
    }


    // *----------------------------------------------------------------*
    // *    
    // *   () tcp_if_receive  (unsigned char* pDat, unsigned int MaxDatLen)
    // *    
    // *----------------------------------------------------------------*
    // *  
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     --
    // *----------------------------------------------------------------*
    unsigned int tcp_if_receive  (int RemoteSockId, unsigned char* pDat, unsigned int MaxDatLen)
    {
        int bytes = 0;
        
        if (MaxDatLen == 0)
            return (0);

        while (bytes == 0)
        {
            bytes = t_recv(RemoteSockId, (char*)pDat, MaxDatLen, 0);
            if (bytes == -1)    // invalid value
                bytes = 0;
            
            if (bytes == 0) 
                OsWait_ms (100);
            else
                OsWait_ms (10);
        }
        return (bytes);
    }
 

    // *----------------------------------------------------------------*
    // *    
    // *   () tcp_if_disconnect  (void) 
    // *    
    // *----------------------------------------------------------------*
    // *  
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     --
    // *----------------------------------------------------------------*
    unsigned int tcp_if_disconnect  (int RemoteSockId) 
    { 
       int status = t_socketclose(RemoteSockId);
       if (status == 0)
            return (PNIO_TRUE);
       else
            return (PNIO_FALSE);
    }


    // *----------------------------------------------------------------*
    // *    
    // *   () tcp_if_disconnect  (void) 
    // *    
    // *----------------------------------------------------------------*
    // *  
    // *  
    // *----------------------------------------------------------------*
    // *  Input:     --
    // *----------------------------------------------------------------*
    unsigned int tcp_if_close  (int LocalSockId) 
    { 
       int status = 0;

       status = t_socketclose(LocalSockId);
       if (status == 0)
            return (PNIO_TRUE);
       else
            return (PNIO_FALSE);
    }






#endif
