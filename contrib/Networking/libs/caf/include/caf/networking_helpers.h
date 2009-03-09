/*
 * NETWORKING_HELPERS.H, Copyright (C) Stefano Crosara 2006
 *
 * Implements some functions that help in network programmin
 *
*/

#ifndef NETWORKING_HELPERS_H
#define NETWORKING_HELPERS_H

//#ifdef __AROS__
#ifndef __WIN32__
typedef int SOCKET;
#define SOCKVALID(x) 	(x != INVALID_SOCKET)
#define SOCKINVALID(x) 	(x == INVALID_SOCKET)
enum { INVALID_SOCKET = -1 };
#endif

/*
#ifdef __MORPHOS__
typedef void *SOCKET;
#define SOCKVALID(x) 	((int)x != INVALID_SOCKET)
#define SOCKINVALID(x) 	((int)x == INVALID_SOCKET)
#define INVALID_SOCKET ((void*)-1)
#endif
*/

enum {
#ifndef WIN32
	SOCKET_ERROR = -1,
#endif
	SOCKET_TIMEVAL_EXPIRED,
	SOCKET_SELECT_OK
};

bool SetBlocking(SOCKET socket, bool b_block);
int ReadSocket(void *in_dest, SOCKET socket, int maxlen);
int CanWriteSocket(SOCKET socket, int wait_sec, int wait_usec);
int CanReadSocket(SOCKET socket, int wait_sec, int wait_usec);
bool WriteSocketNB(SOCKET socket, void *in_buffer, int len, int wait_sec, int wait_usec);
bool ReadSocketNB(void *in_dest, SOCKET socket, int maxlen, int wait_sec, int wait_usec);
int GetLastNetErr();

#endif

