// tabsize ts=4

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/powerpc.h>
#include <intuition/classusr.h>
#include <exec/memory.h>
#include <proto/powerpc.h>
#include <powerpc/powerpc.h>
#include <string.h>

#include "Scalos.h"
#include "PPCRootClass.h"
#include "SubRoutinesPPC.h"
#include "Debug.h"

#include "scalos_protos.h"

// replaces missing WarpOS functions
#define ObtainSemaphoreSharedPPC ObtainSemaphorePPC
#define AttemptSemaphoreSharedPPC AttemptSemaphorePPC


static ULONG Root_New(struct SC_Class *cl, Object *obj, struct opSet *msg, struct RootInst *inst)
{
	if ((inst = SC_AllocVecPPC(((struct SC_Class *) obj)->InstOffset + ((struct SC_Class *) obj)->InstSize + sizeof(struct RootInst), MEMF_CLEAR | MEMF_ANY)))
	{
		inst->oclass = ((struct SC_Class *) obj);
		obj = (Object *) (((struct RootInst *) inst) + 1);
		if (SC_DoMethodPPC(obj, SCCM_Init,msg->ops_AttrList))
		{
			SCOCLASS(obj)->ObjectCount += 1;
			SetCache(CACHE_DCACHEFLUSH, inst, sizeof(struct RootInst));
			return((ULONG) obj);
		}
		SC_DoClassMethodPPC(cl, NULL, (ULONG) obj, SCCM_Exit);
		SC_FreeVecPPC(inst);
	}
	return(NULL);
}

static ULONG Root_Init(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
	inst = myRootInst(obj);

	NewList((struct List *) &inst->objectlist);
	NewList((struct List *) &inst->notilist);
	InitSemaphorePPC(&inst->objectlistsem);
	InitSemaphorePPC(&inst->notilistsem);
	return(TRUE);
}

static ULONG Root_Dispose(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
	SC_DoMethodPPC(obj, SCCM_Exit);
	SCOCLASS(obj)->ObjectCount -= 1;
	SC_FreeVecPPC(myRootInst(obj));
	return(0);
}

static ULONG Root_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
	struct MinNode *node;

	inst = myRootInst(obj);
	ObtainSemaphorePPC(&inst->objectlistsem);
	while (!(IsMinListEmpty(&inst->objectlist)))
	{
		node = inst->objectlist.mlh_Head;
		RemovePPC((struct Node *) node);
		SC_DoMethodPPC(SCBASEOBJECT(node), OM_DISPOSE);
	}
	ReleaseSemaphorePPC(&inst->objectlistsem);

	ObtainSemaphorePPC(&inst->notilistsem);
	FreeAllNodesPPC(&inst->notilist);
	ReleaseSemaphorePPC(&inst->notilistsem);
	return(0);
}

static ULONG Root_AddMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct RootInst *inst)
{
	inst = myRootInst(obj);

	ObtainSemaphorePPC(&inst->objectlistsem);
	AddTailPPC((struct List *) &inst->objectlist, (struct Node *) &_SCOBJECT(msg->opam_Object)->sco_Node);
	ReleaseSemaphorePPC(&inst->objectlistsem);
	return(0);
}

static ULONG Root_RemMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct RootInst *inst)
{
	inst = myRootInst(obj);

	ObtainSemaphorePPC(&inst->objectlistsem);
	RemovePPC((struct Node *) &_SCOBJECT(obj)->sco_Node);
	ReleaseSemaphorePPC(&inst->objectlistsem);
	return(0);
}

static ULONG Root_Notify(struct SC_Class *cl, Object *obj, struct SCCP_Notify *msg, struct RootInst *inst)
{
	struct NotifyNode *buffer;
	ULONG cpsize = (msg->numargs)*sizeof(ULONG) + sizeof(struct NotifyNode) - 3*sizeof(ULONG); // complete parameters length

	inst = myRootInst(obj);
	ObtainSemaphorePPC(&inst->notilistsem);
	if ((buffer = (struct NotifyNode *) AllocNodePPC(&inst->notilist,cpsize)))
		memcpy(&buffer->TriggerAttr, &msg->TriggerAttr, cpsize - sizeof(struct MinNode));
	ReleaseSemaphorePPC(&inst->notilistsem);
	return TRUE;
}

static void Root_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct RootInst *inst)
{
	struct	TagItem *tmptaglist = msg->ops_AttrList;
	struct	TagItem **taglist = &tmptaglist;
	struct	TagItem *tag;
	struct	NotifyNode *node;
	ULONG	tmpvalue;
	ULONG	tmparray[10];
	ULONG	*srcarray;
	ULONG	*destarray;
	int		i;

	inst = myRootInst(obj);
	ObtainSemaphoreSharedPPC(&inst->notilistsem);
	while ((tag = NextTagItemPPC(taglist))) // search in taglist
	{
		// look for a attribute that we should trigger
		for (node = (struct NotifyNode *) inst->notilist.mlh_Head; node->node.mln_Succ; node = (struct NotifyNode *) node->node.mln_Succ)
		{
			if (tag->ti_Tag == node->TriggerAttr) // if found
			{
				if ( !(SC_GetAttrPPC(tag->ti_Tag,obj,&tmpvalue)) || (tag->ti_Data != tmpvalue) ) // if current attribute from object doesn`t exist or the attribute has been changed

					/* execute only if attr-value has changed */
					if ((node->TriggerVal == SCCV_EveryTime) || (tag->ti_Data == node->TriggerVal)) // SCCV_EveryTime: notify everytime the values changes
					{
						if (node->arg_method == SCCM_Set)
						{
							if (node->arg_value == SCCV_TriggerValue)
								SC_SetAttrsPPC(node->DestObject, node->arg_attr, tag->ti_Data, TAG_DONE);
							else
								SC_SetAttrsPPC(node->DestObject, node->arg_attr, node->arg_value);
						}
						else
						{
							srcarray = &node->arg_method;
							if (node->numargs > 10)
								destarray = SC_AllocVecPPC(node->numargs * sizeof(ULONG), MEMF_PUBLIC);
							else
								destarray = (ULONG *) &tmparray;
							for (i = 0; i < node->numargs; i++)
							{
								if (srcarray[i] == SCCV_TriggerValue)
									destarray[i] = tag->ti_Data;
								else
									destarray[i] = srcarray[i];
							}
							SC_DoMethodAPPC(node->DestObject, (Msg) destarray);
							if (node->numargs > 10)
								SC_FreeVecPPC(destarray);
						}
					}
			}
		}
	}
	ReleaseSemaphorePPC(&inst->notilistsem);
}

static struct MinList *Root_LockObjectList(struct SC_Class *cl, Object *obj, struct SCCP_LockObjectList *msg, struct RootInst *inst)
{
	inst = myRootInst(obj);
	switch (msg->locktype)
	{
		case SCCV_LockExclusive :
			ObtainSemaphorePPC(&inst->objectlistsem);
			return(&inst->objectlist);

		case SCCV_LockShared :
			ObtainSemaphoreSharedPPC(&inst->objectlistsem);
			return(&inst->objectlist);

		case SCCV_LockAttempt :
			if (AttemptSemaphorePPC(&inst->objectlistsem))
				return(&inst->objectlist);
			else
				return(NULL);

		case SCCV_LockAttemptShared :
			if (AttemptSemaphoreSharedPPC(&inst->objectlistsem))
				return(&inst->objectlist);
			else
				return(NULL);
	}
}

static void Root_UnlockObjectList(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
	inst = myRootInst(obj);
	ReleaseSemaphorePPC(&inst->objectlistsem);
}

static ULONG Root_Default(struct SC_Class *cl, Object *obj, Msg msg, struct RootInst *inst)
{
	return(0);
}

struct SC_MethodData PPCRootMethods[] = 
{
	{ OM_NEW, (ULONG) Root_New, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
	{ OM_DISPOSE, (ULONG) Root_Dispose, sizeof(ULONG), 0, NULL },
	{ OM_SET, (ULONG) Root_Set, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
	{ OM_GET, NULL, sizeof(struct opGet), SCMDF_FULLFLUSH, NULL },
	{ OM_ADDMEMBER, (ULONG) Root_AddMember, sizeof(struct opMember), 0, NULL },
	{ OM_REMMEMBER, (ULONG) Root_RemMember, sizeof(struct opMember), 0, NULL },
	{ SCCM_Init, (ULONG) Root_Init, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
	{ SCCM_Exit, (ULONG) Root_Exit, sizeof(ULONG), 0, NULL },
	{ SCCM_Notify, (ULONG) Root_Notify, sizeof(struct SCCP_Notify) + sizeof(ULONG)*32, 0, NULL },	// maximal 32 args with notifies
	{ SCCM_LockObjectList, (ULONG) Root_LockObjectList, sizeof(struct SCCP_LockObjectList), 0, NULL },
	{ SCCM_UnlockObjectList, (ULONG) Root_UnlockObjectList, sizeof(ULONG), 0, NULL },
	{ SCMETHOD_DONE, (ULONG) Root_Default, 0, 0, NULL }
};

