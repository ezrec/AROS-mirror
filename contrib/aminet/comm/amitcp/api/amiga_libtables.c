/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * 
 * Created: Tue Feb 16 14:14:33 1993 too
 * Last modified: Tue Jan 11 21:35:30 1994 too
 * 
 * HISTORY 
 * $Log$
 * Revision 1.2  2001/12/26 19:30:19  henrik
 * Aros version
 *
 * Revision 1.1  2001/12/25 22:21:39  henrik
 *
 * Revision 3.3  1994/01/11  19:36:40  too
 * Replaced SetDtableSize with getdtablesize.
 * Removed some functions now in SocketBaseTagList
 *
 * Revision 3.2  1994/01/08  17:40:09  too
 * Added sendmsg and recvmsg
 *
 * Revision 3.1  1994/01/04  14:26:29  too
 * Added new, release 3 functions (getdtablesize, gethostname,
 * gethostid, GetHErrno, SetNetError and SocketBaseTagList)
 *
 * Revision 1.20  1993/08/12  07:32:27  jraja
 * Changed ioctl to IoctlSocket (too).
 *
 * Revision 1.19  1993/06/12  08:57:05  too
 * Added Du2Socket()
 *
 * Revision 1.18  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 */

#include <conf.h>

#include <exec/types.h>
#include <sys/param.h>
#include <api/amiga_raf.h>
#include <amitcp/cdefs.h>

typedef VOID (*  f_void)();

/*
 * Null used in both function tables
 */
extern VOID  Null(VOID);

/*
 * "declarations" for ExecLibraryList_funcTable functions.
 */ 

extern VOID  _ELL_Open();
extern VOID  _ELL_Expunge();
extern VOID  _UL_Close();

f_void ExecLibraryList_funcTable[] = {
  _ELL_Open,
  _UL_Close,	/* ELL_Close() is never called */ /*But _ELL_Expunge calls UL_Close */
  _ELL_Expunge,
  Null,		/* ELL_Reserved() */
  (f_void)-1
};

/*
 * "declarations" for userLibrary_funcTable functions.
 */ 
extern VOID  _UL_Close();

extern VOID  _socket();
extern VOID  _bind();
extern VOID  _listen();
extern VOID  _accept();
extern VOID  _connect();
extern VOID  _sendto();
extern VOID  _send();
extern VOID  _recvfrom();
extern VOID  _recv();
extern VOID  _shutdown();
extern VOID  _setsockopt();
extern VOID  _getsockopt();
extern VOID  _getsockname();
extern VOID  _getpeername();

extern VOID  _IoctlSocket();
extern VOID  _CloseSocket();
extern VOID  _WaitSelect();
extern VOID  _SetSocketSignals();
extern VOID  _getdtablesize();  /* from V3 on */
/*extern REGARGFUN VOID _SetDTableSize(); */
extern VOID  _ObtainSocket();
extern VOID  _ReleaseSocket();
extern VOID  _ReleaseCopyOfSocket();
extern VOID  _Errno();
extern VOID  _SetErrnoPtr();

extern VOID  _Inet_NtoA();
extern VOID  _inet_addr();
extern VOID  _Inet_LnaOf();
extern VOID  _Inet_NetOf();
extern VOID  _Inet_MakeAddr();
extern VOID  _inet_network();

extern VOID  _gethostbyname();
extern VOID  _gethostbyaddr();
extern VOID  _getnetbyname();
extern VOID  _getnetbyaddr();
extern VOID  _getservbyname();
extern VOID  _getservbyport();
extern VOID  _getprotobyname();
extern VOID  _getprotobynumber();

extern VOID  _Syslog();

/* bsdsocket.library 2 extensions */
extern VOID  _Dup2Socket();

/* bsdsocket.library 3 extensions */
extern VOID  _sendmsg();
extern VOID  _recvmsg();
extern VOID  _gethostname();
extern VOID  _gethostid();
extern VOID  _SocketBaseTagList();

f_void UserLibrary_funcTable[] = {
  (f_void) Null,		/* Open() */
  _UL_Close,
  (f_void) Null,		/* Expunge() */
  (f_void) Null,		/* Reserved() */

  (f_void) _socket,
  (f_void) _bind,
  (f_void) _listen,
  (f_void) _accept,
  (f_void) _connect,
  (f_void) _sendto,
  (f_void) _send,
  (f_void) _recvfrom,
  (f_void) _recv,
  (f_void) _shutdown,
  (f_void) _setsockopt,
  (f_void) _getsockopt,
  (f_void) _getsockname,
  (f_void) _getpeername,

  (f_void) _IoctlSocket,
  (f_void) _CloseSocket,
  (f_void) _WaitSelect,
  (f_void) _SetSocketSignals,
  (f_void) _getdtablesize,	/* from V3 on */
/*  _SetDTableSize, */
  (f_void) _ObtainSocket,
  (f_void) _ReleaseSocket,
  (f_void) _ReleaseCopyOfSocket,
  (f_void) _Errno,
  (f_void) _SetErrnoPtr,

  (f_void) _Inet_NtoA,
  (f_void) _inet_addr,
  (f_void) _Inet_LnaOf,
  (f_void) _Inet_NetOf,
  (f_void) _Inet_MakeAddr,
  (f_void) _inet_network,

  (f_void) _gethostbyname,
  (f_void) _gethostbyaddr,
  (f_void) _getnetbyname,
  (f_void) _getnetbyaddr,
  (f_void) _getservbyname,
  (f_void) _getservbyport,
  (f_void) _getprotobyname,
  (f_void) _getprotobynumber,
  (f_void) _Syslog,
  
  /* bsdsocket.library 2 extensions */
  (f_void) _Dup2Socket,

  /* bsdsocket.library 3 extensions */
  (f_void) _sendmsg,
  (f_void) _recvmsg,
  (f_void) _gethostname,
  (f_void) _gethostid,
  (f_void) _SocketBaseTagList,
  (f_void)-1
};
