/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _BSDSOCKET_H_
#define _BSDSOCKET_H_

#include <Generic/Types.h>
#include <libdata/bsdsocket/socket.h>
#include <libclass/exec.h>

#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>

   BEGINDECL(BSDSocket, "bsdsocket.library")
      FUNC3(int32,            socket,                    5, int32,            domain,     d0,   int32,                  type,    d1,         int32,   protocol,   d2);
      FUNC3(int32,            bind,                      6, int32,            socket,     d0,   const struct sockaddr*, name,    a0,         int32,   namelen,    d1);
      FUNC2(int32,            listen,                    7, int32,            socket,     d0,   int32,                  backlog, d1);
      FUNC3(int32,            accept,                    8, int32,            socket,     d0,   struct sockaddr*,       addr,    a0,         int32*,  addrlen,    a1);
      FUNC3(int32,            connect,                   9, int32,            socket,     d0,   const struct sockaddr*, name,    a0,         int32,   namelen,    d1);
      FUNC6(int32,            sendto,                   10, int32,            socket,     d0,   const void*,            buffer,  a0,         int32,   buflen,     d1,   uint32,  flags,      d2,   const struct sockaddr*, dest,       a1,   int32,   destlen, d3);
      FUNC4(int32,            send,                     11, int32,            socket,     d0,   const void*,            buffer,  a0,         int32,   buflen,     d1,   uint32,  flags,      d2);
      FUNC6(int32,            recvfrom,                 12, int32,            socket,     d0,   void*,                  buffer,  a0,         int32,   buflen,     d1,   uint32,  flags,      d2,   struct sockaddr*,       src,        a1,   int32*,  srclen,  a2);
      FUNC4(int32,            recv,                     13, int32,            socket,     d0,   void*,                  buffer,  a0,         int32,   buflen,     d1,   uint32,  flags,      d2);
      FUNC2(int32,            shutdown,                 14, int32,            socket,     d0,   int32,                  how,     d1);
      FUNC5(int32,            setsockopt,               15, int32,            socket,     d0,   int32,                  level,   d1,         int32,   optname,    d2,   void*,   optval,     a0,   int32,                  optlen,     d3);
      FUNC5(int32,            getsockopt,               16, int32,            socket,     d0,   int32,                  level,   d1,         int32,   optname,    d2,   void*,   optval,     a0,   int32*,                 optlen,     a1);
      FUNC3(int32,            getsocketname,            17, int32,            socket,     d0,   struct sockaddr*,       name,    a0,         int32*,  namelen,    a1);
      FUNC3(int32,            getpeername,              18, int32,            socket,     d0,   struct sockaddr*,       name,    a0,         int32*,  namelen,    a1);
      FUNC3(int32,            IoctlSocket,              19, int32,            socket,     d0,   uint32,                 request, d1,         void*,   arg,        a0); 
      FUNC1(int32,            CloseSocket,              20, int32,            socket,     d0);
      FUNC6(int32,            WaitSelect,               21, int32,            nfds,       d0,   fd_set*,                readfds, a0,         fd_set*, writefds,   a1,   fd_set*, exceptfds,  a2,   struct timeval*,        timeout,    a3,   uint32*, signals, d1);
      PROC3(                  SetSocketSignals,         22, uint32,           intmask,    d0,   uint32,                 iomask,  d1,         uint32,  urgentmask, d2);
      FUNC0(int32,            getdtablesize,            23);
      FUNC4(int32,            ObtainSocket,             24, int32,            id,         d0,   int32,                  domain,  d1,         int32,   type,       d2,   int32,   protocol,   d3);
      FUNC2(int32,            ReleaseSocket,            25, int32,            socket,     d0,   int32,                  id,      d1);
      FUNC2(int32,            ReleaseCopyOfSocket,      26, int32,            socket,     d0,   int32,                  id,      d1);
      FUNC0(int32,            Errno,                    27);
      PROC2(                  SetErrnoPtr,              28, void*,            errnoptr,   a0,   int32,                  size,    d0);
      FUNC1(char*,            Inet_NtoA,                29, uint32,           ip,         d0);
      FUNC1(uint32,           inet_addr,                30, const char*,      cp,         a0);
      FUNC1(uint32,           Inet_LnaOf,               31, uint32,           in,         d0);
      FUNC1(uint32,           Inet_NetOf,               32, uint32,           in,         d0);
      FUNC2(uint32,           Inet_MakeAddr,            33, uint32,           net,        d0,   uint32,                 host,    d1);
      FUNC1(uint32,           inet_network,             34, const char*,      cp,         a0);
      FUNC1(struct hostent*,  gethostbyname,            35, const char*,      name,       a0);
      FUNC3(struct hostent*,  gethostbyaddr,            36, const char*,      addr,       a0,   int32,                  len,     d0,         int32,   type,       d1);
      FUNC1(struct netent*,   getnetbyname,             37, const char*,      name,       a0);
      FUNC2(struct netent*,   getnetbyaddr,             38, uint32,           net,        d0,   int32,                  type,    d1);
      FUNC2(struct servent*,  getservbyname,            39, const char*,      name,       a0,   const char*,            proto,   a1);
      FUNC2(struct servent*,  getservbyport,            40, int32,            port,       d0,   const char*,            proto,   a0);
      FUNC1(struct protoent*, getprotobyname,           41, const char*,      name,       a0);
      FUNC1(struct protoent*, getprotobynumber,         42, int32,            proto,      d0);
      PROC3(                  vsyslog,                  43, int32,            level,      d0,   const char*,            format,  a0,         void*,   args,       a1);
#ifdef __AMIGAOS4__
      PROC0(                  reserved0,                43);
#endif
      FUNC2(int32,            Dup2Socket,               44, int32,            oldsocket,  d0,   int32,                  newsock, d1);
      FUNC3(int32,            sendmsg,                  45, int32,            socket,     d0,   const struct msghdr*,   msg,     a0,         uint32,  flags,      d1);
      FUNC3(int32,            recvmsg,                  46, int32,            socket,     d0,   const struct msghdr*,   msg,     a0,         uint32,  flags,      d1);
      FUNC2(int32,            gethostname,              47, char*,            name,       a0,   int32,                  namelen, d0);
      FUNC0(uint32,           gethostid,                48);
      FUNC1(int32,            SocketBaseTagList,        49, struct TagItem*,  tags,       a0);
#ifdef __AMIGAOS4__
      PROC0(                  reserved1,                49);
#endif
      FUNC1(int32,            GetSocketEvents,          50, uint32*,          eventptr,   a0);
#ifdef __AMIGAOS4__
      // A BUNCH of AmigaOS4 functions follow.
#endif
   ENDDECL

#undef MOS_DIRECT_OS

#endif /*_BSDSOCKET_H_*/
