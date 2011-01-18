/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_SOCKET_H
#define _PPCINLINE_SOCKET_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef SOCKET_BASE_NAME
#define SOCKET_BASE_NAME SocketBase
#endif /* !SOCKET_BASE_NAME */

#define SetErrnoPtr(__p0, __p1) \
	LP2(168, LONG , SetErrnoPtr, \
		void *, __p0, a0, \
		LONG , __p1, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define recv(__p0, __p1, __p2, __p3) \
	LP4(78, LONG , recv, \
		LONG , __p0, d0, \
		UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostbyaddr(__p0, __p1, __p2) \
	LP3(216, struct hostent *, gethostbyaddr, \
		const UBYTE *, __p0, a0, \
		LONG , __p1, d0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SocketBaseTagList(__p0) \
	LP1(294, LONG , SocketBaseTagList, \
		struct TagItem *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define setsockopt(__p0, __p1, __p2, __p3, __p4) \
	LP5(90, LONG , setsockopt, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		const void *, __p3, a0, \
		LONG , __p4, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getprotobyname(__p0) \
	LP1(246, struct protoent *, getprotobyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define WaitSelect(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(126, LONG , WaitSelect, \
		LONG , __p0, d0, \
		fd_set *, __p1, a0, \
		fd_set *, __p2, a1, \
		fd_set *, __p3, a2, \
		struct timeval *, __p4, a3, \
		ULONG *, __p5, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostname(__p0, __p1) \
	LP2(282, LONG , gethostname, \
		STRPTR , __p0, a0, \
		LONG , __p1, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getdtablesize() \
	LP0(138, LONG , getdtablesize, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define listen(__p0, __p1) \
	LP2(42, LONG , listen, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getsockname(__p0, __p1, __p2) \
	LP3(102, LONG , getsockname, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostbyname(__p0) \
	LP1(210, struct hostent *, gethostbyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define shutdown(__p0, __p1) \
	LP2(84, LONG , shutdown, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getservbyname(__p0, __p1) \
	LP2(234, struct servent *, getservbyname, \
		const UBYTE *, __p0, a0, \
		const UBYTE *, __p1, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define bind(__p0, __p1, __p2) \
	LP3(36, LONG , bind, \
		LONG , __p0, d0, \
		const struct sockaddr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getnetbyaddr(__p0, __p1) \
	LP2(228, struct netent *, getnetbyaddr, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getservbyport(__p0, __p1) \
	LP2(240, struct servent *, getservbyport, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define recvfrom(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(72, LONG , recvfrom, \
		LONG , __p0, d0, \
		UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		struct sockaddr *, __p4, a1, \
		LONG *, __p5, a2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define SetSocketSignals(__p0, __p1, __p2) \
	LP3NR(132, SetSocketSignals, \
		ULONG , __p0, d0, \
		ULONG , __p1, d1, \
		ULONG , __p2, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define socket(__p0, __p1, __p2) \
	LP3(30, LONG , socket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define connect(__p0, __p1, __p2) \
	LP3(54, LONG , connect, \
		LONG , __p0, d0, \
		const struct sockaddr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_NetOf(__p0) \
	LP1(192, ULONG , Inet_NetOf, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define sendmsg(__p0, __p1, __p2) \
	LP3(270, LONG , sendmsg, \
		LONG , __p0, d0, \
		struct msghdr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReleaseCopyOfSocket(__p0, __p1) \
	LP2(156, LONG , ReleaseCopyOfSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_NtoA(__p0) \
	LP1(174, char *, Inet_NtoA, \
		ULONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Errno() \
	LP0(162, LONG , Errno, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define inet_addr(__p0) \
	LP1(180, ULONG , inet_addr, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getsockopt(__p0, __p1, __p2, __p3, __p4) \
	LP5(96, LONG , getsockopt, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		void *, __p3, a0, \
		LONG *, __p4, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getnetbyname(__p0) \
	LP1(222, struct netent *, getnetbyname, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Dup2Socket(__p0, __p1) \
	LP2(264, LONG , Dup2Socket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ObtainSocket(__p0, __p1, __p2, __p3) \
	LP4(144, LONG , ObtainSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		LONG , __p2, d2, \
		LONG , __p3, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define sendto(__p0, __p1, __p2, __p3, __p4, __p5) \
	LP6(60, LONG , sendto, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		const struct sockaddr *, __p4, a1, \
		LONG , __p5, d3, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_MakeAddr(__p0, __p1) \
	LP2(198, ULONG , Inet_MakeAddr, \
		ULONG , __p0, d0, \
		ULONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define send(__p0, __p1, __p2, __p3) \
	LP4(66, LONG , send, \
		LONG , __p0, d0, \
		const UBYTE *, __p1, a0, \
		LONG , __p2, d1, \
		LONG , __p3, d2, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define CloseSocket(__p0) \
	LP1(120, LONG , CloseSocket, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define IoctlSocket(__p0, __p1, __p2) \
	LP3(114, LONG , IoctlSocket, \
		LONG , __p0, d0, \
		ULONG , __p1, d1, \
		char *, __p2, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define ReleaseSocket(__p0, __p1) \
	LP2(150, LONG , ReleaseSocket, \
		LONG , __p0, d0, \
		LONG , __p1, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define gethostid() \
	LP0(288, ULONG , gethostid, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getpeername(__p0, __p1, __p2) \
	LP3(108, LONG , getpeername, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define accept(__p0, __p1, __p2) \
	LP3(48, LONG , accept, \
		LONG , __p0, d0, \
		struct sockaddr *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define recvmsg(__p0, __p1, __p2) \
	LP3(276, LONG , recvmsg, \
		LONG , __p0, d0, \
		struct msghdr *, __p1, a0, \
		LONG , __p2, d1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define vsyslog(__p0, __p1, __p2) \
	LP3NR(258, vsyslog, \
		ULONG , __p0, d0, \
		const char *, __p1, a0, \
		LONG *, __p2, a1, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define getprotobynumber(__p0) \
	LP1(252, struct protoent *, getprotobynumber, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define inet_network(__p0) \
	LP1(204, ULONG , inet_network, \
		const UBYTE *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define GetSocketEvents(__p0) \
	LP1(300, LONG , GetSocketEvents, \
		ULONG *, __p0, a0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define Inet_LnaOf(__p0) \
	LP1(186, ULONG , Inet_LnaOf, \
		LONG , __p0, d0, \
		, SOCKET_BASE_NAME, 0, 0, 0, 0, 0, 0)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)

#include <stdarg.h>

#define syslog(__p0, __p1, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	vsyslog(__p0, __p1, (LONG *)_tags);})

#define SocketBaseTags(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	SocketBaseTagList((struct TagItem *)_tags);})

#endif

#endif /* !_PPCINLINE_SOCKET_H */
