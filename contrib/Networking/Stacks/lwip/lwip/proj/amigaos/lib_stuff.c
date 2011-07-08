#include <string.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>

#include "globaldata.h"
#include "localdata.h"

#include "socket_intern.h"

#define LIBNAME "bsdsocket.library"

#define MYDEBUG
#include "debug.h"

/* from sys_arch.c */
struct ThreadData *Thread_Alloc(void);
void Thread_Free(struct ThreadData *data);

#ifdef __AROS__

AROS_UFH3(struct SocketBase_intern *,LIB_init,
        AROS_LHA(struct SocketBase_intern *, SocketBase, D0),
        AROS_LHA(ULONG, seglist, A0),
	AROS_LHA(struct ExecBase *, sysbase, A6))


#else
__asm struct SocketBase_intern *LIB_init(register __d0 struct SocketBase_intern *SocketBase, register __a0 ULONG seglist, register __a6 struct ExecBase *sysbase)
#endif
{
    SocketBase->sysbase = sysbase;
    SocketBase->library.lib_Node.ln_Type = NT_LIBRARY;
    SocketBase->library.lib_Node.ln_Name = LIBNAME;
    SocketBase->library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    SocketBase->library.lib_Version = 1;
    SocketBase->library.lib_Revision = 0;
    SocketBase->library.lib_IdString = LIBNAME;

    SocketBase->orgbase = SocketBase;
    SocketBase->data = NULL;

    if (!(SocketBase->gldata = GlobalData_New(&SocketBase->library)))
    {
    	D(bug(LIBNAME ": Init failed\n"));
#warning free Library base here
	return NULL;
    }

    D(bug(LIBNAME ": Init succesfull\n"));
    return SocketBase;
}

#ifndef __AROS__
__asm struct SocketBase_intern *LIB_open(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_LH1(struct SocketBase_intern *, LIB_open,
    AROS_LHA(ULONG, version, D0),
    struct SocketBase_intern *, SocketBase, 1, Socket)
#endif
{
    void *newlib;
    struct SocketBase_intern *orgbase = SocketBase->orgbase;

    /* Increase the library's open counter */
    orgbase->library.lib_OpenCnt++;

    /* Clear delayed expunges (standard procedure) */
    orgbase->library.lib_Flags &= ~LIBF_DELEXP;

    newlib = AllocVec(orgbase->library.lib_NegSize + orgbase->library.lib_PosSize, MEMF_PUBLIC|MEMF_CLEAR);
    if (!newlib)
    {
      orgbase->library.lib_OpenCnt--;
      return NULL;
    }

    /* Copy over data */
    memcpy(newlib, (char*)orgbase - orgbase->library.lib_NegSize, orgbase->library.lib_NegSize + orgbase->library.lib_PosSize);

    D(bug(LIBNAME ": openlib() allocated a new library base at 0x%lx (taskname=%s)\n",newlib,FindTask(NULL)->tc_Node.ln_Name));
    SocketBase = (void*)(((char*)newlib) + orgbase->library.lib_NegSize);

    if (!(SocketBase->data = Thread_Alloc()))
    {
	FreeVec(newlib);
	orgbase->library.lib_OpenCnt--;
	return NULL;
    }

    if (!(SocketBase->locdata = LocalData_New(&SocketBase->library)))
    {
	Thread_Free(SocketBase->data);
	FreeVec(newlib);
	orgbase->library.lib_OpenCnt--;
	return NULL;
    }

    return SocketBase; /* return library base */
}

#ifndef __AROS__
__asm ULONG LIB_expunge(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_LH0(struct SocketBase_intern *, LIB_expunge,
    struct SocketBase_intern *, SocketBase, 1, Socket)
#endif
{
    long size;

    if (SocketBase->library.lib_OpenCnt == 0)
    {
	/* remove us from the list */
	Remove((struct Node *)SocketBase);

        /* Remove GlobalData */
        GlobalData_Dispose(&SocketBase->library,GLDATA(SocketBase));

	/* get size to FreeMem() */
	size = SocketBase->library.lib_NegSize + SocketBase->library.lib_PosSize;

	/* FreeMem() */
	FreeMem((char *)SocketBase - SocketBase->library.lib_NegSize, size);

    } else
    {
  	/* we are opened */
	SocketBase->library.lib_Flags |= LIBF_DELEXP;
    }
    return NULL;
}

#ifndef __AROS__
__asm ULONG LIB_close(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_LH0(struct SocketBase_intern *, LIB_close,
    struct SocketBase_intern *, SocketBase, 1, Socket)
#endif
{
    ULONG ret = NULL;

    D(bug("Socket_close\n"));

    if (SocketBase != SocketBase->orgbase)
    {
	struct SocketBase_intern *orgbase = SocketBase->orgbase;
	LocalData_Dispose(&SocketBase->library,SocketBase->locdata);
	Thread_Free(SocketBase->data);
	FreeVec((char *)SocketBase - SocketBase->library.lib_NegSize);
	SocketBase = orgbase;
    }

    /* Decrease the library's open counter */
    SocketBase->library.lib_OpenCnt--;

    if (!SocketBase->library.lib_OpenCnt)
    {
	/* not opened any more */
	if (SocketBase->library.lib_Flags & LIBF_DELEXP)
	{
	    /* There is a delayed expunge waiting */
#ifndef __AROS__
	    ret = LIB_expunge(SocketBase);
#else
	    ret = AROS_LC0(BPTR, LIB_expunge, struct SocketBase_intern *, SocketBase, 3, Socket);
#endif
	}
    }
    return ret;
}


int LIB_reserved(void)
{
    return 0;
}


static struct Library *socketlib;

#ifndef __AROS__
#define AROS_SLIB_ENTRY(a,b,c) a
#endif

extern void AROS_SLIB_ENTRY(LIB_open, Socket, 1)(void);
extern void AROS_SLIB_ENTRY(LIB_close, Socket, 2)(void);
extern void AROS_SLIB_ENTRY(LIB_expunge, Socket, 3)(void);
extern void AROS_SLIB_ENTRY(LIB_socket, Socket, 5)(void);
extern void AROS_SLIB_ENTRY(LIB_bind, Socket, 6)(void);
extern void AROS_SLIB_ENTRY(LIB_listen, Socket, 7)(void);
extern void AROS_SLIB_ENTRY(LIB_accept, Socket, 8)(void);
extern void AROS_SLIB_ENTRY(LIB_connect, Socket, 9)(void);
extern void AROS_SLIB_ENTRY(LIB_sendto, Socket, 10)(void);
extern void AROS_SLIB_ENTRY(LIB_send, Socket, 11)(void);
extern void AROS_SLIB_ENTRY(LIB_recvfrom, Socket, 12)(void);
extern void AROS_SLIB_ENTRY(LIB_recv, Socket, 13)(void);
extern void AROS_SLIB_ENTRY(LIB_shutdown, Socket, 14)(void);
extern void AROS_SLIB_ENTRY(LIB_setsockopt, Socket, 15)(void);
extern void AROS_SLIB_ENTRY(LIB_getsockopt, Socket, 16)(void);
extern void AROS_SLIB_ENTRY(LIB_getsockname, Socket, 17)(void);
extern void AROS_SLIB_ENTRY(LIB_getpeername, Socket, 18)(void);
extern void AROS_SLIB_ENTRY(LIB_IoctlSocket, Socket, 19)(void);
extern void AROS_SLIB_ENTRY(LIB_CloseSocket, Socket, 20)(void);
extern void AROS_SLIB_ENTRY(LIB_WaitSelect, Socket, 21)(void);
extern void AROS_SLIB_ENTRY(LIB_SetSocketSignals, Socket, 22)(void);
extern void AROS_SLIB_ENTRY(LIB_getdtablesize, Socket, 23)(void);
extern void AROS_SLIB_ENTRY(LIB_ObtainSocket, Socket, 24)(void);
extern void AROS_SLIB_ENTRY(LIB_ReleaseSocket, Socket, 25)(void);
extern void AROS_SLIB_ENTRY(LIB_ReleaseCopyOfSocket, Socket, 26)(void);
extern void AROS_SLIB_ENTRY(LIB_Errno, Socket, 27)(void);
extern void AROS_SLIB_ENTRY(LIB_SetErrnoPtr, Socket, 28)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_NtoA, Socket, 29)(void);
extern void AROS_SLIB_ENTRY(LIB_inet_addr, Socket, 30)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_LnaOf, Socket, 31)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_NetOf, Socket, 32)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_MakeAddr, Socket, 33)(void);
extern void AROS_SLIB_ENTRY(LIB_inet_network, Socket, 34)(void);
extern void AROS_SLIB_ENTRY(LIB_gethostbyname, Socket, 35)(void);

static void *function_array[] =
{
  AROS_SLIB_ENTRY(LIB_open, Socket, 1),
  AROS_SLIB_ENTRY(LIB_close, Socket, 2),
  AROS_SLIB_ENTRY(LIB_expunge, Socket, 3),
  LIB_reserved,
  AROS_SLIB_ENTRY(LIB_socket, Socket, 5),
  AROS_SLIB_ENTRY(LIB_bind, Socket, 6),
  AROS_SLIB_ENTRY(LIB_listen, Socket, 7),
  AROS_SLIB_ENTRY(LIB_accept, Socket, 8),
  AROS_SLIB_ENTRY(LIB_connect, Socket, 9),
  AROS_SLIB_ENTRY(LIB_sendto, Socket, 10),
  AROS_SLIB_ENTRY(LIB_send, Socket, 11),
  AROS_SLIB_ENTRY(LIB_recvfrom, Socket, 12),
  AROS_SLIB_ENTRY(LIB_recv, Socket, 13),
  AROS_SLIB_ENTRY(LIB_shutdown, Socket, 14),
  AROS_SLIB_ENTRY(LIB_setsockopt, Socket, 15),
  AROS_SLIB_ENTRY(LIB_getsockopt, Socket, 16),
  AROS_SLIB_ENTRY(LIB_getsockname, Socket, 17),         /* -102 */
  AROS_SLIB_ENTRY(LIB_getpeername, Socket, 18),         /* -108 */
  AROS_SLIB_ENTRY(LIB_IoctlSocket, Socket, 19),         /* -114 */
  AROS_SLIB_ENTRY(LIB_CloseSocket, Socket, 20),         /* -120 */
  AROS_SLIB_ENTRY(LIB_WaitSelect, Socket, 21),          /* -126 */
  AROS_SLIB_ENTRY(LIB_SetSocketSignals, Socket, 22),    /* -132 */
  AROS_SLIB_ENTRY(LIB_getdtablesize, Socket, 23),       /* -138 */
  AROS_SLIB_ENTRY(LIB_ObtainSocket, Socket, 24),        /* -144 */
  AROS_SLIB_ENTRY(LIB_ReleaseSocket, Socket, 25),       /* -150 */
  AROS_SLIB_ENTRY(LIB_ReleaseCopyOfSocket, Socket, 26), /* -156 */
  AROS_SLIB_ENTRY(LIB_Errno, Socket, 27),               /* -162 */
  AROS_SLIB_ENTRY(LIB_SetErrnoPtr, Socket, 28),         /* -168 */
  AROS_SLIB_ENTRY(LIB_Inet_NtoA, Socket, 29),           /* -174 */
  AROS_SLIB_ENTRY(LIB_inet_addr, Socket, 30),           /* -180 */
  AROS_SLIB_ENTRY(LIB_Inet_LnaOf, Socket, 31),          /* -186 */
  AROS_SLIB_ENTRY(LIB_Inet_NetOf, Socket, 32),          /* -192 */
  AROS_SLIB_ENTRY(LIB_Inet_MakeAddr, Socket, 33),       /* -198 */
  AROS_SLIB_ENTRY(LIB_inet_network, Socket, 34),        /* -204 */
  AROS_SLIB_ENTRY(LIB_gethostbyname, Socket, 35),       /* -210 */
  (void*)~0,
};

/* Must use the global sysbase */
#undef SysBase

static struct Library *dummy = NULL;

int LIB_add(void)
{
    socketlib = MakeLibrary(function_array,NULL,(ULONG (* const )())LIB_init,sizeof(struct SocketBase_intern),NULL);
    if (socketlib)
    {
	AddLibrary(socketlib);
	/* keep an opener to prevent flushing */
	dummy = OpenLibrary(LIBNAME, 0);
	if (!dummy)
	    return 0;
	return 1;
    }
    return 0;
}

int LIB_remove(void)
{
    if (dummy)
	CloseLibrary(dummy);
    if (socketlib)
    {
	RemLibrary(socketlib);
    }
    return 1;
}

