#include <string.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>

#include "globaldata.h"
#include "localdata.h"

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/* from sys_arch.c */
struct ThreadData *Thread_Alloc(void);
void Thread_Free(struct ThreadData *data);

#ifdef __AROS

AROS_UFH4(struct SocketBase_intern *,LIB_init,
        AROS_LHA(struct SocketBase_intern *, SocketBase, D0),
        AROS_LHA(ULONG, seglist, A0),
        AROS_LHA(long, len, D1),
	AROS_LHA(struct ExecBase *, sysbase, A6))


#else
__asm struct SocketBase_intern *LIB_init(register __d0 struct SocketBase_intern *SocketBase, register __a0 ULONG seglist, register __a6 struct ExecBase *sysbase)
#endif
{
    SocketBase->sysbase = sysbase;
    SocketBase->library.lib_Node.ln_Type = NT_LIBRARY;
    SocketBase->library.lib_Node.ln_Name =  "bsdsocket.library";
    SocketBase->library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    SocketBase->library.lib_Version = 1;
    SocketBase->library.lib_Revision = 0;
    SocketBase->library.lib_IdString = "bsdsocket.library";

    SocketBase->orgbase = SocketBase;
    SocketBase->data = NULL;

    if (!(SocketBase->gldata = GlobalData_New(&SocketBase->library)))
    {
    	D(bug("bsdsocket.library: Init failed\n"));
#warning free Library base here
	return NULL;
    }

    D(bug("bsdsocket.library: Init succesfull\n"));
    return SocketBase;
}

#ifndef _AROS
__asm struct SocketBase_intern *LIB_open(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_UFH1(struct SocketBase_intern *, LIB_open,
        AROS_LHA(struct SocketBase_intern *, SocketBase, A6))
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

    D(bug("bsdsocket.library: openlib() allocated a new library base at 0x%lx (taskname=%s)\n",newlib,FindTask(NULL)->tc_Node.ln_Name));
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

#ifndef __AROS
__asm ULONG LIB_expunge(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_UFH1(ULONG,LIB_expunge,
        AROS_LHA(struct SocketBase_intern *, SocketBase, A6))
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

#ifndef __AROS
__asm ULONG LIB_close(register __a6 struct SocketBase_intern *SocketBase)
#else
AROS_UFH1(ULONG,LIB_close,
        AROS_LHA(struct SocketBase_intern *, SocketBase, D0))
#endif
{
    ULONG ret = NULL;

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
	    ret = LIB_expunge(SocketBase);
	}
    }
    return ret;
}


int LIB_reserved(void)
{
    return 0;
}


static struct Library *socketlib;

extern void AROS_SLIB_ENTRY(LIB_socket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_bind, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_listen, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_accept, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_connect, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_sendto, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_send, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_recvfrom, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_recv, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_shutdown, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_setsockopt, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_getsockopt, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_getsockname, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_getpeername, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_IoctlSocket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_CloseSocket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_WaitSelect, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_SetSocketSignals, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_getdtablesize, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_ObtainSocket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_ReleaseSocket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_ReleaseCopyOfSocket, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_Errno, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_SetErrnoPtr, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_NtoA, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_inet_addr, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_LnaOf, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_NetOf, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_Inet_MakeAddr, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_inet_network, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_gethostbyname, Socket)(void);
extern void AROS_SLIB_ENTRY(LIB_SocketBaseTagList, Socket)(void); /* -294 */

static void *function_array[] =
{
  LIB_open,
  LIB_close,
  LIB_expunge,
  LIB_reserved,
  AROS_SLIB_ENTRY(LIB_socket, Socket),
  AROS_SLIB_ENTRY(LIB_bind, Socket),
  AROS_SLIB_ENTRY(LIB_listen, Socket),
  AROS_SLIB_ENTRY(LIB_accept, Socket),
  AROS_SLIB_ENTRY(LIB_connect, Socket),
  AROS_SLIB_ENTRY(LIB_sendto, Socket),
  AROS_SLIB_ENTRY(LIB_send, Socket),
  AROS_SLIB_ENTRY(LIB_recvfrom, Socket),
  AROS_SLIB_ENTRY(LIB_recv, Socket),
  AROS_SLIB_ENTRY(LIB_shutdown, Socket),
  AROS_SLIB_ENTRY(LIB_setsockopt, Socket),
  AROS_SLIB_ENTRY(LIB_getsockopt, Socket),
  AROS_SLIB_ENTRY(LIB_getsockname, Socket),         /* -102 */
  AROS_SLIB_ENTRY(LIB_getpeername, Socket),         /* -108 */
  AROS_SLIB_ENTRY(LIB_IoctlSocket, Socket),         /* -114 */
  AROS_SLIB_ENTRY(LIB_CloseSocket, Socket),         /* -120 */
  AROS_SLIB_ENTRY(LIB_WaitSelect, Socket),          /* -126 */
  AROS_SLIB_ENTRY(LIB_SetSocketSignals, Socket),    /* -132 */
  AROS_SLIB_ENTRY(LIB_getdtablesize, Socket),       /* -138 */
  AROS_SLIB_ENTRY(LIB_ObtainSocket, Socket),        /* -144 */
  AROS_SLIB_ENTRY(LIB_ReleaseSocket, Socket),       /* -150 */
  AROS_SLIB_ENTRY(LIB_ReleaseCopyOfSocket, Socket), /* -156 */
  AROS_SLIB_ENTRY(LIB_Errno, Socket),               /* -162 */
  AROS_SLIB_ENTRY(LIB_SetErrnoPtr, Socket),         /* -168 */
  AROS_SLIB_ENTRY(LIB_Inet_NtoA, Socket),           /* -174 */
  AROS_SLIB_ENTRY(LIB_inet_addr, Socket),           /* -180 */
  AROS_SLIB_ENTRY(LIB_Inet_LnaOf, Socket),          /* -186 */
  AROS_SLIB_ENTRY(LIB_Inet_NetOf, Socket),          /* -192 */
  AROS_SLIB_ENTRY(LIB_Inet_MakeAddr, Socket),       /* -198 */
  AROS_SLIB_ENTRY(LIB_inet_network, Socket),        /* -204 */
  AROS_SLIB_ENTRY(LIB_gethostbyname, Socket),       /* -210 */
  (void*)~0,
};

/* Must use the global sysbase */
#undef SysBase

int LIB_add(void)
{
    socketlib = MakeLibrary(function_array,NULL,(ULONG (* const )())LIB_init,sizeof(struct SocketBase_intern),NULL);
    if (socketlib)
    {
	AddLibrary(socketlib);
	return 1;
    }
}

int LIB_remove(void)
{
    if (socketlib)
    {
	RemLibrary(socketlib);
    }
    return 1;
}

