#include <string.h>

#include <proto/exec.h>

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/* from sys_arch.c */
struct ThreadData *Thread_Alloc(void);
void Thread_Free(struct ThreadData *data);


__asm struct SocketBase_intern *LIB_init(register __d0 struct SocketBase_intern *SocketBase, register __a0 ULONG seglist, register __a6 struct ExecBase *sysbase)
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

  D(bug("bsdsocket.library: Init succesfull\n"));
  return SocketBase;
}

__asm struct SocketBase_intern *LIB_open(register __a6 struct SocketBase_intern *SocketBase)
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

  return SocketBase; /* return library base */
}

__asm ULONG LIB_expunge(register __a6 struct SocketBase_intern *SocketBase)
{
  long size;

  if (SocketBase->library.lib_OpenCnt == 0)
  {
    /* remove us from the list */
    Remove((struct Node *)SocketBase);

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


__asm ULONG LIB_close(register __a6 struct SocketBase_intern *SocketBase)
{
  ULONG ret = NULL;

  if (SocketBase != SocketBase->orgbase)
  {
    struct SocketBase_intern *orgbase = SocketBase->orgbase;
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


extern void LIB_socket(void);
extern void LIB_bind(void);
extern void LIB_listen(void);
extern void LIB_accept(void);
extern void LIB_connect(void);
extern void LIB_sendto(void);
extern void LIB_send(void);
extern void LIB_recvfrom(void);
extern void LIB_recv(void);
extern void LIB_shutdown(void);
extern void LIB_setsockopt(void);
extern void LIB_getsockopt(void);
extern void LIB_getsockname(void);
extern void LIB_getpeername(void);
extern void LIB_IoctlSocket(void);
extern void LIB_CloseSocket(void);
extern void LIB_WaitSelect(void);
extern void LIB_SetSocketSignals(void);
extern void LIB_SocketBaseTagList(void); /* -294 */

static void *function_array[] =
{
  LIB_open,
  LIB_close,
  LIB_expunge,
  LIB_reserved,
  LIB_socket,
  LIB_bind,
  LIB_listen,
  LIB_accept,
  LIB_connect,
  LIB_sendto,
  LIB_send,
  LIB_recvfrom,
  LIB_recv,
  LIB_shutdown,
  LIB_setsockopt,
  LIB_getsockopt,
  LIB_getsockname,         /* -102 */
  LIB_getpeername,         /* -108 */
  LIB_IoctlSocket,         /* -114 */
  LIB_CloseSocket,         /* -120 */
  LIB_WaitSelect,          /* -126 */
  LIB_SetSocketSignals,    /* -132 */
  (void*)~0,
};

/* Must use the global sysbase */
#undef SysBase

int LIB_add(void)
{
  socketlib = MakeLibrary(function_array,NULL,LIB_init,sizeof(struct SocketBase_intern),NULL);
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

