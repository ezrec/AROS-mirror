#include <exec/memory.h>
#include <exec/semaphores.h>

#ifdef __MAXON__
#include <pragma/exec_lib.h>
#else
#include <proto/exec.h>
#endif

#include <clib/alib_protos.h>

#include "global.h"
#include "reqlist.h"

static struct SignalSemaphore sem;
static struct SignalSemaphore memsem;
static APTR mempool;
static struct List reqlist;

void InitReqList(void)
{
	InitSemaphore(&sem);
	InitSemaphore(&memsem);
	NewList(&reqlist);
	
	if (!(mempool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR,sizeof(struct ReqNode) * 5,sizeof(struct ReqNode) * 5)))
	{
		Cleanup("InitReqList: Out of memory!");
	}
	
	progport.reqlist = &reqlist;
	progport.reqlistsem = &sem;
	
}

void CleanupReqList(void)
{
	if (mempool) DeletePool(mempool);
}

struct ReqNode *NewReqNode(void)
{
	struct ReqNode *rc;
	
	ObtainSemaphore(&memsem);
	rc = (struct ReqNode *)AllocPooled(mempool,sizeof(struct ReqNode));
	ReleaseSemaphore(&memsem);

	return rc;
}

void FreeReqNode(struct ReqNode *node)
{
	/* must be removed! */
	FreePooled(mempool,node,sizeof(struct ReqNode));
}

void AddReqNode(struct ReqNode *node)
{
	ObtainSemaphore(&sem);
	AddTail(&reqlist,&node->node);
	node->flags |= REQF_INLIST;
	ReleaseSemaphore(&sem);
}

void RemoveReqNode(struct ReqNode *node)
{
	ObtainSemaphore(&sem);
	Remove(&node->node);
	node->flags &= (~REQF_INLIST);
	ReleaseSemaphore(&sem);
}

struct ReqNode *FindReqNode(struct Window *win)
{
	struct ReqNode *node,*succ,*rc = 0;

	ObtainSemaphoreShared(&sem);
	
	node = (struct ReqNode *)reqlist.lh_Head;
	while ((succ = (struct ReqNode *)node->node.ln_Succ))
	{
		if (node->win == win)
		{
			rc = node;
			break;
		}
		
		node = succ;
	}
	
	ReleaseSemaphore(&sem);
	
	return rc;
}


BOOL ReqListEmpty(void)
{
	BOOL rc;

	ObtainSemaphoreShared(&sem);
	rc = IsListEmpty(&reqlist);	
	ReleaseSemaphore(&sem);
	
	return rc;
}

