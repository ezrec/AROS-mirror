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

#include <conf.h>

#include <exec/types.h>
#include <sys/param.h>
#include <api/amiga_raf.h>

typedef VOID (* REGARGFUN f_void)();

/*
 * Null used in both function tables
 */
extern VOID Null(VOID);

/*
 * "declarations" for ExecLibraryList_funcTable functions.
 */ 

extern REGARGFUN VOID ELL_Open();
extern REGARGFUN VOID ELL_Expunge();

f_void ExecLibraryList_funcTable[] = {
  ELL_Open,
  Null,		/* ELL_Close() is never called */
  ELL_Expunge,
  Null,		/* ELL_Reserved() */
  (f_void)-1
};

/*
 * "declarations" for userLibrary_funcTable functions.
 */ 
extern REGARGFUN VOID UL_Close();

extern REGARGFUN VOID socket();
extern REGARGFUN VOID bind();
extern REGARGFUN VOID listen();
extern REGARGFUN VOID accept();
extern REGARGFUN VOID connect();
extern REGARGFUN VOID sendto();
extern REGARGFUN VOID send();
extern REGARGFUN VOID recvfrom();
extern REGARGFUN VOID recv();
extern REGARGFUN VOID shutdown();
extern REGARGFUN VOID setsockopt();
extern REGARGFUN VOID getsockopt();
extern REGARGFUN VOID getsockname();
extern REGARGFUN VOID getpeername();

extern REGARGFUN VOID IoctlSocket();
extern REGARGFUN VOID CloseSocket();
extern REGARGFUN VOID WaitSelect();
extern REGARGFUN VOID SetSocketSignals();
extern REGARGFUN VOID getdtablesize();  /* from V3 on */
/*extern REGARGFUN VOID SetDTableSize(); */
extern REGARGFUN VOID ObtainSocket();
extern REGARGFUN VOID ReleaseSocket();
extern REGARGFUN VOID ReleaseCopyOfSocket();
extern REGARGFUN VOID Errno();
extern REGARGFUN VOID SetErrnoPtr();

extern REGARGFUN VOID Inet_NtoA();
extern REGARGFUN VOID inet_addr();
extern REGARGFUN VOID Inet_LnaOf();
extern REGARGFUN VOID Inet_NetOf();
extern REGARGFUN VOID Inet_MakeAddr();
extern REGARGFUN VOID inet_network();

extern REGARGFUN VOID gethostbyname();
extern REGARGFUN VOID gethostbyaddr();
extern REGARGFUN VOID getnetbyname();
extern REGARGFUN VOID getnetbyaddr();
extern REGARGFUN VOID getservbyname();
extern REGARGFUN VOID getservbyport();
extern REGARGFUN VOID getprotobyname();
extern REGARGFUN VOID getprotobynumber();

extern REGARGFUN VOID Syslog();

/* bsdsocket.library 2 extensions */
extern REGARGFUN VOID Dup2Socket();

/* bsdsocket.library 3 extensions */
extern REGARGFUN VOID sendmsg();
extern REGARGFUN VOID recvmsg();
extern REGARGFUN VOID gethostname();
extern REGARGFUN VOID gethostid();
extern REGARGFUN VOID SocketBaseTagList();

f_void UserLibrary_funcTable[] = {
  (f_void)Null,		/* Open() */
  UL_Close,
  (f_void)Null,		/* Expunge() */
  (f_void)Null,		/* Reserved() */

  socket,
  bind,
  listen,
  accept,
  connect,
  sendto,
  send,
  recvfrom,
  recv,
  shutdown,
  setsockopt,
  getsockopt,
  getsockname,
  getpeername,

  IoctlSocket,
  CloseSocket,
  WaitSelect,
  SetSocketSignals,
  getdtablesize,	/* from V3 on */
/*  SetDTableSize, */
  ObtainSocket,
  ReleaseSocket,
  ReleaseCopyOfSocket,
  Errno,
  SetErrnoPtr,

  Inet_NtoA,
  inet_addr,
  Inet_LnaOf,
  Inet_NetOf,
  Inet_MakeAddr,
  inet_network,

  gethostbyname,
  gethostbyaddr,
  getnetbyname,
  getnetbyaddr,
  getservbyname,
  getservbyport,
  getprotobyname,
  getprotobynumber,
  Syslog,
  
  /* bsdsocket.library 2 extensions */
  Dup2Socket,

  /* bsdsocket.library 3 extensions */
  sendmsg,
  recvmsg,
  gethostname,
  gethostid,
  SocketBaseTagList,
  (f_void)-1
};
