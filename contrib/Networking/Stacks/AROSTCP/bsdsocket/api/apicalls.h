/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#ifndef API_APICALLS_H
#define API_APICALLS_H

typedef VOID (* REGARGFUN f_void)(APTR args, ...);



#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef SYS_TIME_H
#include <sys/time.h>
#endif

#include <stdarg.h>

#ifndef IN_H
#include <netinet/in.h>
#endif
/*#endif*/

#include <api/amiga_api.h>


extern REGARGFUN LONG CloseSocket(
   REG(d0, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG connect(
   REG(d0, LONG),
   REG(a0, struct sockaddr *),
   REG(d1, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG recv(
   REG(d0, LONG),
   REG(a0, char *),
   REG(d1, LONG),
   REG(d2, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG send(
   REG(d0, LONG),
   REG(a0, char *),
   REG(d1, LONG),
   REG(d2, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG sendto(
   REG(d0, LONG),
   REG(a0, char *),
   REG(d1, LONG),
   REG(d2, LONG),
   REG(a1, struct sockaddr *),
   REG(d3, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG socket(
   REG(d0, LONG),
   REG(d1, LONG),
   REG(d2, LONG),
   REG(a6, struct SocketBase *));


extern REGARGFUN LONG WaitSelect(
   REG(d0, ULONG),
   REG(a0, fd_set *),
   REG(a1, fd_set *),
   REG(a2, fd_set *),
   REG(a3, struct timeval *),
   REG(d1, ULONG *),
   REG(a6, struct SocketBase *));


extern REGARGFUN struct hostent * gethostbyaddr(
   REG(a0, const UBYTE *),
   REG(d0, int),
   REG(d1, int),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG gethostname(
   REG(a0, STRPTR),
   REG(d0, LONG),
   REG(a6, struct SocketBase *));

extern REGARGFUN LONG SetErrnoPtr(
   REG(a0, VOID *),
   REG(d0, UBYTE),
   REG(a6, struct SocketBase *));




#endif /* API_APICALLS_H */
