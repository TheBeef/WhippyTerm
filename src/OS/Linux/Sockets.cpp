/*******************************************************************************
 * FILENAME: Sockets.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the linux version of the sockets interface in it.
 *
 * COPYRIGHT:
 *    Copyright 23 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (23 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/Sockets.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct LinuxSocket
{
    int fd;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    OpenSocket
 *
 * SYNOPSIS:
 *    struct OSSocket *OpenSocket(const char *Dest,unsigned int Port,
 *          volatile bool *AbortFlag);
 *
 * PARAMETERS:
 *    Dest [I] -- The server name to open (can be an IP address)
 *    Port [I] -- The port to open the connection on
 *    AbortFlag [I] -- This is a pointer to a bool that is used to abort the
 *                     connect().  Set this to false before calling
 *                     this function.  This function will then check this
 *                     var while attempting to connent to the server.  If
 *                     it is set to true then this function is aborted
 *                     and returns fail.
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
        volatile bool *AbortFlag)
{
    struct LinuxSocket *NewSocket;
    char PortStr[100];
    struct addrinfo hints,*res,*p;
    int status;
    int flags;
    fd_set writefds;
    struct timeval tv;
    int sel;
    int so_error;
    socklen_t len;

    NewSocket=NULL;
    try
    {
        NewSocket=new struct LinuxSocket;
        NewSocket->fd=-1;

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
            if(NewSocket->fd==-1)
            {
                /* Failed to get a socket of this type, move on */
                continue;
            }

            /* Connect to this server with abort */

            flags=fcntl(NewSocket->fd,F_GETFL,0);
            if(flags==-1)
                throw(0);
            flags|=O_NONBLOCK;
            fcntl(NewSocket->fd,F_SETFL,flags);
            connect(NewSocket->fd,p->ai_addr,p->ai_addrlen);

            /* Wait for the connect or an abort */
            for(;;)
            {
                FD_ZERO(&writefds);
                FD_SET(NewSocket->fd,&writefds);
                tv.tv_sec=0;
                tv.tv_usec=100000;

                sel=select(NewSocket->fd+1,NULL,&writefds,NULL,&tv);
                if(sel>0)
                {
                    len=sizeof(so_error);
                    getsockopt(NewSocket->fd,SOL_SOCKET,SO_ERROR,&so_error,
                            &len);
                    if(so_error!=0)
                    {
                        /* Connection failed */
                        close(NewSocket->fd);
                        NewSocket->fd=-1;
                    }
                    break;
                }
                if(*AbortFlag)
                    throw(0);
            }
            if(NewSocket->fd==-1)
            {
                /* Failed, move on */
                continue;
            }

            /* Ok, we have an open connection */
            break;
        }
        freeaddrinfo(res);
        if(NewSocket->fd==-1)
        {
            /* No connection */
            throw(0);
        }

        /* Clear the non-blocking */
        flags=fcntl(NewSocket->fd,F_GETFL,0);
        if(flags==-1)
            throw(0);
        flags&=~O_NONBLOCK;
        fcntl(NewSocket->fd,F_SETFL,flags);
    }
    catch(...)
    {
        if(NewSocket!=NULL)
        {
            if(NewSocket->fd>=0)
                close(NewSocket->fd);
            delete NewSocket;
        }
        return NULL;
    }
    return (struct OSSocket *)NewSocket;
}

bool SendSocket(struct OSSocket *Sock,const void *Buffer,unsigned int Bytes)
{
    struct LinuxSocket *TheSocket=(struct LinuxSocket *)Sock;

    if(send(TheSocket->fd,Buffer,Bytes,0)!=Bytes)
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
    struct LinuxSocket *TheSocket=(struct LinuxSocket *)Sock;
    int Bytes;
    struct timeval tv;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    setsockopt(TheSocket->fd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&tv,sizeof(tv));

    Bytes=recv(TheSocket->fd,Buffer,MaxBytes,0);
    if(Bytes==0)
        return -1;
    if(Bytes<0)
    {
        /* Timeout errors = 0 */
        if(errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;

        /* General error */
        return -2;
    }

    return Bytes;
}

unsigned int AvailSocket(struct OSSocket *Sock)
{
    struct LinuxSocket *TheSocket=(struct LinuxSocket *)Sock;
    int Bytes;

    if(ioctl(TheSocket->fd,FIONREAD,&Bytes)!=0)
        return 0;

    if(Bytes<0)
        return 0;

    return Bytes;
}

void CloseSocket(struct OSSocket *Sock)
{
    struct LinuxSocket *TheSocket=(struct LinuxSocket *)Sock;

    if(TheSocket->fd!=-1)
        close(TheSocket->fd);

    delete TheSocket;
}

