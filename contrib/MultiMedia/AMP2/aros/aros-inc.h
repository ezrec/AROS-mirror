//Includes required to compile under AROS

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/asl.h>
#include <proto/gadtools.h>

#include <proto/timer.h>

#include <libraries/asl.h>

#include <string.h>

#include <aros/macros.h>

//extern void GetSysTime(struct Library *, struct timeval * );
//extern void SubTime(struct Library *, struct timeval *, struct timeval *);

// AMP uses PPC defines, so patch them with ours

#ifndef AROS_AMPSUPPORT
extern struct Task *CreateTask( struct TagItem *);
#endif

#ifndef GOT_TASKTAGS
#define GOT_TASKTAGS
#define TAGBASE					0x1202020
#define TASKATTR_CODE			(TAGBASE + 0x01)
#define TASKATTR_NAME			(TAGBASE + 0x02)
#define TASKATTR_STACKSIZE		(TAGBASE + 0x03)
#define TASKATTR_INHERITR2		(TAGBASE + 0x04)
#endif /* GOT_TASKTAGS */

#define AllocVecPPC(x,y,z) AllocVec(x,y)
#define FreeVecPPC FreeVec

#define PutMsgPPC PutMsg
#define WaitPortPPC WaitPort
#define GetMsgPPC GetMsg
#define ReplyMsgPPC ReplyMsg
#define FindPortPPC FindPort
#define CreateMsgPortPPC CreateMsgPort
#define AddPortPPC AddPort
#define RemPortPPC RemPort
#define DeleteMsgPortPPC DeleteMsgPort

#define MsgPortPPC MsgPort

#define InitSemaphorePPC InitSemaphore
#define SignalSemaphorePPC SignalSemaphore
#define ObtainSemaphorePPC ObtainSemaphore
#define ReleaseSemaphorePPC ReleaseSemaphore

#define FreeSemaphorePPC(sem)
#define WaitTime(x,y) { int tmp = y ; while (tmp >= 0) tmp--; }
#define SSPPC_SUCCESS TRUE

#define AllocSignalPPC AllocSignal
#define FreeSignalPPC FreeSignal
#define SetSignalPPC SetSignal
#define SignalPPC Signal
#define WaitPPC Wait
#define GetSysTimePPC GetSysTime
#define SubTimePPC SubTime
#define CreateTaskPPC CreateTask
#define FindTaskPPC FindTask

