/*******************************************************************************
 * FILENAME: Sockets.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the Win32 version of the sockets interface in it.
 *
 * COPYRIGHT:
 *    Copyright 28 Apr 2026 Paul Hutchinson.
 *
 *    This program is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation, either version 3 of the License, or (at your
 *    option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program. If not, see https://www.gnu.org/licenses/.
 *
 * CREATED BY:
 *    Paul Hutchinson (28 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/Sockets.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct Win32Socket
{
    SOCKET fd;
};

/*** FUNCTION PROTOTYPES      ***/
static bool InitWinsock(void);

/*** VARIABLE DEFINITIONS     ***/
static bool m_WinsockInited=false;

/*******************************************************************************
 * NAME:
 *    InitWinsock
 *
 * SYNOPSIS:
 *    static bool InitWinsock(void);
 *
 * PARAMETERS:
 *    None.
 *
 * FUNCTION:
 *    Lazy-init for Winsock.  Calls WSAStartup() the first time we need it.
 *    The matching WSACleanup() is left to process exit.
 *
 * RETURNS:
 *    true if Winsock is ready to use, false if WSAStartup() failed.
 ******************************************************************************/
static bool InitWinsock(void)
{
    WSADATA wsaData;

    if(m_WinsockInited)
        return true;

    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
        return false;

    m_WinsockInited=true;
    return true;
}

/*******************************************************************************
 * NAME:
 *    OpenSocket
 *
 * SYNOPSIS:
 *    struct OSSocket *OpenSocket(const char *Dest,unsigned int Port,
 *          bool (*AbortCheck)(void));
 *
 * PARAMETERS:
 *    Dest [I] -- The server name to open (can be an IP address)
 *    Port [I] -- The port to open the connection on
 *    AbortCheck [I] -- This function is called when trying to connect to
 *                      the server.  It returns true if the caller of
 *                      OpenSocket() wants to abort.  This will be call over and
 *                      over while trying to connect.
 *
 * FUNCTION:
 *    This function opens an connects to a server (or at least tries)
 *
 * RETURNS:
 *    A handle to the OS socket or NULL if there was an error.
 *
 * LIMITATIONS:
 *    This is a blocking call
 *
 * SEE ALSO:
 *    SendSocket(), ReadSocket(), AvailSocket()
 ******************************************************************************/
struct OSSocket *OpenSocket(const char *Dest,unsigned int Port,
        bool (*AbortCheck)(void))
{
    struct Win32Socket *NewSocket;
    char PortStr[100];
    struct addrinfo hints,*res,*p;
    int status;
    u_long nonblocking;
    fd_set writefds;
    struct timeval tv;
    int sel;
    int so_error;
    int len;

    NewSocket=NULL;
    res=NULL;
    try
    {
        if(!InitWinsock())
            throw(0);

        NewSocket=new struct Win32Socket;
        NewSocket->fd=INVALID_SOCKET;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM;

        sprintf(PortStr,"%d",Port);
        status=getaddrinfo(Dest,PortStr,&hints,&res);
        if(status!=0)
            throw(0);

        for(p=res;p!=NULL;p=p->ai_next)
        {
            NewSocket->fd=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if(NewSocket->fd==INVALID_SOCKET)
            {
                /* Failed to get a socket of this type, move on */
                continue;
            }

            /* Connect to this server with abort */

            /* Set non-blocking */
            nonblocking=1;
            if(ioctlsocket(NewSocket->fd,FIONBIO,&nonblocking)!=0)
                throw(0);

            connect(NewSocket->fd,p->ai_addr,(int)p->ai_addrlen);

            /* Wait for the connect or an abort */
            for(;;)
            {
                FD_ZERO(&writefds);
                FD_SET(NewSocket->fd,&writefds);
                tv.tv_sec=0;
                tv.tv_usec=100000;

                /* nfds is ignored on Windows */
                sel=select(0,NULL,&writefds,NULL,&tv);
                if(sel>0)
                {
                    len=sizeof(so_error);
                    getsockopt(NewSocket->fd,SOL_SOCKET,SO_ERROR,
                            (char *)&so_error,&len);
                    if(so_error!=0)
                    {
                        /* Connection failed */
                        closesocket(NewSocket->fd);
                        NewSocket->fd=INVALID_SOCKET;
                    }
                    break;
                }
                if(AbortCheck!=NULL && AbortCheck())
                    throw(0);
            }
            if(NewSocket->fd==INVALID_SOCKET)
            {
                /* Failed, move on */
                continue;
            }

            /* Ok, we have an open connection */
            break;
        }
        if(NewSocket->fd==INVALID_SOCKET)
        {
            /* No connection */
            throw(0);
        }

        /* Clear the non-blocking */
        nonblocking=0;
        if(ioctlsocket(NewSocket->fd,FIONBIO,&nonblocking)!=0)
            throw(0);
    }
    catch(...)
    {
        if(NewSocket!=NULL)
        {
            if(NewSocket->fd!=INVALID_SOCKET)
                closesocket(NewSocket->fd);
            delete NewSocket;
        }
        return NULL;
    }

    if(res!=NULL)
        freeaddrinfo(res);

    return (struct OSSocket *)NewSocket;
}

bool SendSocket(struct OSSocket *Sock,const void *Buffer,unsigned int Bytes)
{
    struct Win32Socket *TheSocket=(struct Win32Socket *)Sock;

    if(send(TheSocket->fd,(const char *)Buffer,(int)Bytes,0)!=(int)Bytes)
        return false;
    return true;
}

/*******************************************************************************
 * NAME:
 *    ReadSocket
 *
 * SYNOPSIS:
 *    int ReadSocket(struct OSSocket *Sock,void *Buffer,unsigned int MaxBytes);
 *
 * PARAMETERS:
 *    Sock [I] -- The socket to work on
 *    Buffer [O] -- The buffer to fill
 *    MaxBytes [I] -- The number of bytes to try to read from the socket
 *
 * FUNCTION:
 *    This function reads bytes from an open socket.  It has a 1 second timeout
 *    on the read (if the read doesn't return ANY bytes it will wait up to
 *    1 second to get some).
 *
 * RETURNS:
 *    The number of bytes read or:
 *       0 -- Timeout error
 *      -1 -- Connection closed
 *      -2 -- An error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int ReadSocket(struct OSSocket *Sock,void *Buffer,unsigned int MaxBytes)
{
    struct Win32Socket *TheSocket=(struct Win32Socket *)Sock;
    int Bytes;
    DWORD timeout;
    int err;

    /* On Windows SO_RCVTIMEO takes a DWORD of milliseconds, not a timeval */
    timeout=1000;
    setsockopt(TheSocket->fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,
            sizeof(timeout));

    Bytes=recv(TheSocket->fd,(char *)Buffer,(int)MaxBytes,0);
    if(Bytes==0)
        return -1;
    if(Bytes<0)
    {
        err=WSAGetLastError();
        /* Timeout errors = 0 */
        if(err==WSAETIMEDOUT || err==WSAEWOULDBLOCK)
            return 0;

        /* General error */
        return -2;
    }

    return Bytes;
}

unsigned int AvailSocket(struct OSSocket *Sock)
{
    struct Win32Socket *TheSocket=(struct Win32Socket *)Sock;
    u_long Bytes;

    if(ioctlsocket(TheSocket->fd,FIONREAD,&Bytes)!=0)
        return 0;

    return (unsigned int)Bytes;
}

void CloseSocket(struct OSSocket *Sock)
{
    struct Win32Socket *TheSocket=(struct Win32Socket *)Sock;

    if(TheSocket->fd!=INVALID_SOCKET)
        closesocket(TheSocket->fd);

    delete TheSocket;
}
