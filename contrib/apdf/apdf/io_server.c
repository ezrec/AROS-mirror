/*************************************************************************\
 *                                                                       *
 * io_server.c                                                           *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <intuition/classusr.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#ifdef HAVE_POWERUP
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/memory.h>
#include <powerup/ppclib/tasks.h>
struct ModuleArgs;    /* avoid warnings */
struct PPCObjectInfo; /* avoid warnings */
#undef NO_INLINE_STDARG
#ifdef __MORPHOS__
#   include <proto/ppc.h>
#else
#   include <powerup/proto/ppc.h>
#endif
#endif
#include "Apdf.h"
#include "AComm.h"
#include "io_server.h"

#define DB(x)       //x
#define Static      static
#define MEMDBG(x)   //x

typedef void MyMsgPort;

struct MyIoHandle {
    struct MyIoHandle *next;
    struct MyIoHandle *prev;
    BPTR file;
};

static struct MyIoHandle *head;
static struct MyIoHandle *tail;

static MyMsgPort *port;
static struct Task *parent;
static struct Process *serverProc;


static MyMsgPort *myCreatePort(void) {
#ifdef HAVE_POWERUP
    if (PPCLibBase)
	return PPCCreatePort(NULL);
    else
#endif
	return CreateMsgPort();
}

static void myDeletePort(MyMsgPort *port) {
#ifdef HAVE_POWERUP
    if (PPCLibBase)
	PPCDeletePort(port);
    else
#endif
	DeleteMsgPort(port);
}

static struct IoMsg *myGetMsg(MyMsgPort *port) {
    struct IoMsg *ioMsg;
    void *msg;
#ifdef HAVE_POWERUP
    if (PPCLibBase) {
	PPCWaitPort(port);
	msg = PPCGetMessage(port);
	ioMsg = (struct IoMsg *)PPCGetMessageAttr(msg, PPCMSGTAG_DATA);
    } else
#endif
	   {
	WaitPort(port);
	msg = GetMsg(port);
	ioMsg = (struct IoMsg *)((struct Message *)msg + 1);
    }
    ioMsg->container = msg;
    return ioMsg;
}

static void myReplyMsg(struct IoMsg *msg) {
#ifdef HAVE_POWERUP
    if (PPCLibBase)
	PPCReplyMessage(msg->container);
    else
#endif
	ReplyMsg(msg->container);
}

static void *myGetRawMsg(MyMsgPort *port, void **container) {
    void *rawMsg;
    void *msg;
#ifdef HAVE_POWERUP
    if (PPCLibBase) {
	PPCWaitPort(port);
	msg = PPCGetMessage(port);
	rawMsg = (struct IoMsg *)PPCGetMessageAttr(msg, PPCMSGTAG_DATA);
    } else
#endif
	   {
	WaitPort(port);
	msg = GetMsg(port);
	rawMsg = *(void **)((struct Message *)msg + 1);
    }
    *container = msg;
    return rawMsg;
}

static void myReplyRawMsg(void *container) {
#ifdef HAVE_POWERUP
    if (PPCLibBase)
	PPCReplyMessage(container);
    else
#endif
	ReplyMsg(container);
}


#ifdef __MORPHOS__
Static void ioServerFunc(void);
struct EmulLibEntry ioServer = {
    TRAP_LIBNR, 0, (void(*)())ioServerFunc
};
Static void ioServerFunc(void)
#else
Static void ioServer(void)
#endif
{
    int running = 1;
    void *pool;
    struct Task *parentTask = parent;

    pool = CreatePool(MEMF_ANY, 4000, 1000);
    if (pool)
	port = myCreatePort();
    Signal(parentTask, SIGBREAKF_CTRL_F);

    if (port) {

      while (parent && parent != (void*)-1)
	Wait(SIGBREAKF_CTRL_F);

      if (parent != (void *)-1)
	do {
	    struct IoMsg *msg = myGetMsg(port);

	    switch (msg->id) {
	      case ioIdQuit:
		running = 0;
		msg->success = 1;
		break;

	      case ioIdOpen: {
		struct IoOpenMsg *openMsg = (struct IoOpenMsg *)msg;
		struct MyIoHandle *handle = AllocPooled(pool, sizeof(struct MyIoHandle));
		if (handle) {
		    int ok = 0;
		    handle->file = Open(openMsg->fileName, MODE_OLDFILE);
		    if (handle->file) {
			struct FileInfoBlock* fib = AllocDosObject(DOS_FIB, NULL);
			if (fib) {
			    if (ExamineFH(handle->file, fib)) {
				openMsg->size = fib->fib_Size;
				ok = 1;
			    }
			    FreeDosObject(DOS_FIB, fib);
			}
		    }
		    if (!ok) {
			if (handle->file)
			    Close(handle->file);
			FreePooled(pool, handle, sizeof(struct MyIoHandle));
			handle = NULL;
		    } else {
			handle->prev = tail;
			handle->next = NULL;
			if (tail)
			    tail->next = handle;
			else
			    head = handle;
			tail = handle;
		    }
		}
		openMsg->ioMsg.handle = handle;
		openMsg->ioMsg.success = handle != NULL;
		break;
	      }

	      case ioIdClose: {
		struct MyIoHandle *handle = msg->handle;
		if (handle->next)
		    handle->next->prev = handle->prev;
		else
		    tail = handle->prev;
		if (handle->prev)
		    handle->prev->next = handle->next;
		else
		    head = handle->next;
		Close(handle->file);
		FreePooled(pool, handle, sizeof(struct MyIoHandle));
		msg->success = 1;
		break;
	      }

	      case ioIdPreRead:
		msg->success = 1;
		break;

	      case ioIdRead: {
		struct IoReadMsg *readMsg = (struct IoReadMsg *)msg;
		struct MyIoHandle *handle = readMsg->ioMsg.handle;
		void *buf;
		void *container;
		buf = myGetRawMsg(port, &container);
		msg->success = Seek(handle->file, readMsg->offset, OFFSET_BEGINNING) != -1;
		if (msg->success) {
		    size_t size = readMsg->size;
		    readMsg->size = Read(handle->file, buf, readMsg->size);
		    msg->success = size = readMsg->size;
		}
		myReplyRawMsg(container);
		break;
	      }

	      case ioIdCurrentDir: {
		struct IoCurrentDirMsg *cdMsg = (struct IoCurrentDirMsg *)msg;
		cdMsg->dir = CurrentDir(cdMsg->dir);
		msg->success = 1;
		break;
	      }

	      /*case ioIdAbort:
		msg->success = 1;
		break;*/
	    }

	    myReplyMsg(msg);
	} while(running);

	/* Close the remaining open files, if any. Allocated memory will be freed
	   by DeletePool(), no need to bother with it. */
	{
	    struct MyIoHandle *handle;
	    for (handle = head; handle; handle = handle->next)
		Close(handle->file);
	}

    }

    DeletePool(pool);
    myDeletePort(port);

    Forbid();
    serverProc = NULL;
    Signal(parentTask, SIGBREAKF_CTRL_F);
}


IoServer initIo(void) {

    parent = FindTask(NULL);
    serverProc = CreateNewProcTags(NP_Entry, &ioServer,
				   NP_Name, "Apdf I/O Server",
				   TAG_END);
    if (serverProc) {
	while (!port && serverProc)
	    Wait(SIGBREAKF_CTRL_F);
    }

    return port;
}


void cleanupIo(IoServer x) {
    if (serverProc && parent) {
	parent = (void *)-1;
	Signal(&serverProc->pr_Task, SIGBREAKF_CTRL_F);
    }

    while(serverProc)
	Wait(SIGBREAKF_CTRL_F);
}

void startServer(IoServer x) {
    parent = NULL;
    Signal(&serverProc->pr_Task, SIGBREAKF_CTRL_F);
}

