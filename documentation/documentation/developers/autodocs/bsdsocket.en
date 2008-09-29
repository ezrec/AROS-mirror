=========
bsdsocket
=========

.. This document is automatically generated. Don't edit it!

`Index <index>`_

----------

======================================= ======================================= ======================================= ======================================= 
`accept()`_                             `bind()`_                               `CloseSocket()`_                        `connect()`_                            
`Dup2Socket()`_                         `Errno()`_                              `getdtablesize()`_                      `gethostbyaddr()`_                      
`gethostbyname()`_                      `gethostid()`_                          `gethostname()`_                        `getnetbyaddr()`_                       
`getnetbyname()`_                       `getpeername()`_                        `getprotobyname()`_                     `getprotobynumber()`_                   
`getservbyname()`_                      `getservbyport()`_                      `GetSocketEvents()`_                    `getsockname()`_                        
`getsockopt()`_                         `inet_addr()`_                          `Inet_LnaOf()`_                         `Inet_MakeAddr()`_                      
`Inet_NetOf()`_                         `inet_network()`_                       `Inet_NtoA()`_                          `IoctlSocket()`_                        
`listen()`_                             `ObtainSocket()`_                       `recv()`_                               `recvfrom()`_                           
`recvmsg()`_                            `ReleaseCopyOfSocket()`_                `ReleaseSocket()`_                      `send()`_                               
`sendmsg()`_                            `sendto()`_                             `SetErrnoPtr()`_                        `SetSocketSignals()`_                   
`setsockopt()`_                         `shutdown()`_                           `socket()`_                             `SocketBaseTagList()`_                  
`vsyslog()`_                            `WaitSelect()`_                         
======================================= ======================================= ======================================= ======================================= 

-----------

accept()
========

Synopsis
~~~~~~~~
::

 int accept(
          int s,
          struct sockaddr * addr,
          int * addrlen );


----------

bind()
======

Synopsis
~~~~~~~~
::

 int bind(
          int s,
          struct sockaddr * name,
          int namelen );


----------

CloseSocket()
=============

Synopsis
~~~~~~~~
::

 int CloseSocket(
          int s );


----------

connect()
=========

Synopsis
~~~~~~~~
::

 int connect(
          int s,
          struct sockaddr * name,
          int namelen );


----------

Dup2Socket()
============

Synopsis
~~~~~~~~
::

 int Dup2Socket(
          int fd1,
          int fd2 );


----------

Errno()
=======

Synopsis
~~~~~~~~
::

 LONG Errno();


----------

getdtablesize()
===============

Synopsis
~~~~~~~~
::

 int getdtablesize();


----------

gethostbyaddr()
===============

Synopsis
~~~~~~~~
::

 struct hostent * gethostbyaddr(
          char * addr,
          int len,
          int type );


----------

gethostbyname()
===============

Synopsis
~~~~~~~~
::

 struct hostent * gethostbyname(
          char * name );


----------

gethostid()
===========

Synopsis
~~~~~~~~
::

 long gethostid();


----------

gethostname()
=============

Synopsis
~~~~~~~~
::

 int gethostname(
          char * name,
          int namelen );


----------

getnetbyaddr()
==============

Synopsis
~~~~~~~~
::

 struct netent * getnetbyaddr(
          long net,
          int type );


----------

getnetbyname()
==============

Synopsis
~~~~~~~~
::

 struct netent * getnetbyname(
          char * name );


----------

getpeername()
=============

Synopsis
~~~~~~~~
::

 int getpeername(
          int s,
          struct sockaddr * name,
          int * namelen );


----------

getprotobyname()
================

Synopsis
~~~~~~~~
::

 struct protoent * getprotobyname(
          char * name );


----------

getprotobynumber()
==================

Synopsis
~~~~~~~~
::

 struct protoent * getprotobynumber(
          int proto );


----------

getservbyname()
===============

Synopsis
~~~~~~~~
::

 struct servent * getservbyname(
          char * name,
          char * proto );


----------

getservbyport()
===============

Synopsis
~~~~~~~~
::

 struct servent * getservbyport(
          int port,
          char * proto );


----------

GetSocketEvents()
=================

Synopsis
~~~~~~~~
::

 LONG GetSocketEvents(
          ULONG * eventsp );


----------

getsockname()
=============

Synopsis
~~~~~~~~
::

 int getsockname(
          int s,
          struct sockaddr * name,
          int * namelen );


----------

getsockopt()
============

Synopsis
~~~~~~~~
::

 int getsockopt(
          int s,
          int level,
          int optname,
          void * optval,
          void * optlen );


----------

inet_addr()
===========

Synopsis
~~~~~~~~
::

 unsigned long inet_addr(
          const char * cp );


----------

Inet_LnaOf()
============

Synopsis
~~~~~~~~
::

 unsigned long Inet_LnaOf(
          unsigned long in );


----------

Inet_MakeAddr()
===============

Synopsis
~~~~~~~~
::

 unsigned long Inet_MakeAddr(
          int net,
          int lna );


----------

Inet_NetOf()
============

Synopsis
~~~~~~~~
::

 unsigned long Inet_NetOf(
          unsigned long in );


----------

inet_network()
==============

Synopsis
~~~~~~~~
::

 unsigned long inet_network(
          const char * cp );


----------

Inet_NtoA()
===========

Synopsis
~~~~~~~~
::

 char * Inet_NtoA(
          unsigned long in );
 
 char * Inet_Nto(
          TAG tag, ... );


----------

IoctlSocket()
=============

Synopsis
~~~~~~~~
::

 int IoctlSocket(
          int s,
          unsigned long request,
          char * argp );


----------

listen()
========

Synopsis
~~~~~~~~
::

 int listen(
          int s,
          int backlog );


----------

ObtainSocket()
==============

Synopsis
~~~~~~~~
::

 LONG ObtainSocket(
          LONG id,
          LONG domain,
          LONG type,
          LONG protocol );


----------

recv()
======

Synopsis
~~~~~~~~
::

 int recv(
          int s,
          void * buf,
          int len,
          int flags );


----------

recvfrom()
==========

Synopsis
~~~~~~~~
::

 int recvfrom(
          int s,
          void * buf,
          int len,
          int flags,
          struct sockaddr * from,
          int * fromlen );


----------

recvmsg()
=========

Synopsis
~~~~~~~~
::

 int recvmsg(
          int s,
          struct msghdr * msg,
          int flags );


----------

ReleaseCopyOfSocket()
=====================

Synopsis
~~~~~~~~
::

 LONG ReleaseCopyOfSocket(
          LONG sd,
          LONG id );


----------

ReleaseSocket()
===============

Synopsis
~~~~~~~~
::

 LONG ReleaseSocket(
          LONG sd,
          LONG id );


----------

send()
======

Synopsis
~~~~~~~~
::

 int send(
          int s,
          const void * msg,
          int len,
          int flags );


----------

sendmsg()
=========

Synopsis
~~~~~~~~
::

 int sendmsg(
          int s,
          const struct msghdr * msg,
          int flags );


----------

sendto()
========

Synopsis
~~~~~~~~
::

 int sendto(
          int s,
          const void * msg,
          int len,
          int flags,
          const struct sockaddr * to,
          int tolen );


----------

SetErrnoPtr()
=============

Synopsis
~~~~~~~~
::

 void SetErrnoPtr(
          void * ptr,
          int size );


----------

SetSocketSignals()
==================

Synopsis
~~~~~~~~
::

 void SetSocketSignals(
          ULONG intrmask,
          ULONG iomask,
          ULONG urgmask );


----------

setsockopt()
============

Synopsis
~~~~~~~~
::

 int setsockopt(
          int s,
          int level,
          int optname,
          void * optval,
          int optlen );


----------

shutdown()
==========

Synopsis
~~~~~~~~
::

 int shutdown(
          int s,
          int how );


----------

socket()
========

Synopsis
~~~~~~~~
::

 int socket(
          int domain,
          int type,
          int protocol );


----------

SocketBaseTagList()
===================

Synopsis
~~~~~~~~
::

 ULONG SocketBaseTagList(
          struct TagItem * tagList );
 
 ULONG SocketBaseTags(
          TAG tag, ... );


----------

vsyslog()
=========

Synopsis
~~~~~~~~
::

 void vsyslog(
          int level,
          const char * format,
          LONG * args );


----------

WaitSelect()
============

Synopsis
~~~~~~~~
::

 int WaitSelect(
          int nfds,
          fd_set * readfds,
          fd_set * writefds,
          fd_set * exceptfds,
          struct timeval * timeout,
          ULONG * sigmask );


