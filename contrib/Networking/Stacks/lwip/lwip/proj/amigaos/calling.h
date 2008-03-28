#ifndef _CALLING_H
#define _CALLING_H

#ifndef EXEC_PORTS_H
#include <exec/ports.h>
#endif

#define LIBMSG_ACCEPT 1
#define LIBMSG_BIND 2
#define LIBMSG_CLOSESOCKET 3
#define LIBMSG_CONNECT 4
#define LIBMSG_GETPEERNAME 5
#define LIBMSG_GETSOCKNAME 6
#define LIBMSG_GETSOCKOPT 7
#define LIBMSG_IOCTLSOCKET 8
#define LIBMSG_LISTEN 9
#define LIBMSG_RECV 10
#define LIBMSG_RECVFROM 11
#define LIBMSG_SEND 12
#define LIBMSG_SENDTO 13
#define LIBMSG_SETSOCKOPT 14
#define LIBMSG_SHUTDOWN 15
#define LIBMSG_SOCKET 16

#ifndef __AROS__
int CallStackFunction(struct SocketBase_intern *SocketBase, int type, int argc, ...);
#endif
struct LibMsg
{
	struct Message msg;
	int type;
	int argc;
	void *args[10];
	int retval;
	int err;
};

#endif
