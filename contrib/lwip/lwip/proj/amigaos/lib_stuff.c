#include <proto/exec.h>

#include "socket_intern.h"

__asm struct SocketBase_intern *LIB_init(register __d0 struct SocketBase_intern *SocketBase, register __a0 ULONG seglist, register __a6 struct ExecBase *sysbase)
{
  SocketBase->sysbase = sysbase;
  SocketBase->library.lib_Node.ln_Type = NT_LIBRARY;
  SocketBase->library.lib_Node.ln_Name =  "bsdsocket.library";
  SocketBase->library.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
  SocketBase->library.lib_Version = 1;
  SocketBase->library.lib_Revision = 0;
  SocketBase->library.lib_IdString = "bsdsocket.library";
  return SocketBase;
}

__asm struct SocketBase_intern *LIB_open(register __a6 struct SocketBase_intern *base)
{
  /* Increase the library's open counter */
  base->library.lib_OpenCnt++;

  /* Clear delayed expunges (standard procedure) */
  base->library.lib_Flags &= ~LIBF_DELEXP;

  return base; /* return library base */
}

__asm ULONG LIB_expunge(register __a6 struct SocketBase_intern *SocketBase)
{
  ULONG ret = NULL;
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


__asm ULONG LIB_close(register __a6 struct SocketBase_intern *base)
{
  ULONG ret = NULL;

  /* Decrease the library's open counter */
  base->library.lib_OpenCnt--;

  if (!base->library.lib_OpenCnt)
  {
    /* not opened any more */
    if (base->library.lib_Flags & LIBF_DELEXP)
    {
      /* There is a delayed expunge waiting */
      ret = LIB_expunge(base);
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
  LIB_getsockname,
  LIB_getpeername,
  LIB_IoctlSocket,
  LIB_CloseSocket,
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

