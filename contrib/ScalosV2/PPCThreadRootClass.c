// tabsize ts=4

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/powerpc.h>
#include <intuition/classusr.h>
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/powerpc.h>
#include <powerpc/powerpc.h>
#include <string.h>

#include "Scalos.h"
#include "ScalosIntern.h"
#include "PPCRootClass.h"
#include "SubRoutinesPPC.h"
#include "PPCThreadRootClass.h"
#include "MethodSenderClass.h"

#include "CompilerSupport.h"
#include "Debug.h"

#include "scalos_protos.h"

// replaces missing WarpOS functions
#define ObtainSemaphoreSharedPPC ObtainSemaphorePPC
#define AttemptSemaphoreSharedPPC AttemptSemaphorePPC

static char deftaskname[] = "private_object";

/*
*--------------------------- ThreadTask -------------------------------
*
*	This is the taskfunction of the object-thread. It will receive the
*	startup message and then initiate the object. After this it will go
*	into the input loop.
*
*/

static void SAVEDS Thread_Task(void)
{
	struct SCMSGP_Startup *startmsg;
	Object *threadobj;
	struct MsgPortPPC *msgport;
	struct TaskPPC *owntask = FindTaskPPC(NULL);

	LockTaskList();
	msgport = owntask->tp_Msgport;
	UnLockTaskList();

	while (!(startmsg = (struct SCMSGP_Startup *) GetMsgPPC(msgport)))
		WaitPortPPC(msgport);

	threadobj = startmsg->threadobj;
	if ((startmsg->scmsg.returnvalue = SC_DoMethodPPC(threadobj, SCCM_Init, startmsg->taglist)))
	{
		if (!(msgport = (struct MsgPortPPC *) getPPC(threadobj, SCCA_MsgPort)))
		{
			SC_DoMethodPPC(threadobj, SCCM_Exit);
			startmsg->scmsg.returnvalue = NULL;
			ReplyMsgPPC((struct Message *) startmsg);
			return;
		}

		ReplyMsgPPC((struct Message *) startmsg);
		while (!(SC_DoMethodPPC(threadobj, SCCM_Input)))
		{
			WaitPortPPC(msgport);
		}
	}
	else
		ReplyMsgPPC((struct Message *) startmsg);

	return;
}



/*--------------------------- Functions --------------------------------*/

static ULONG ThreadRoot_New(struct SC_Class *cl, Object *obj, struct opSet *msg, struct ThreadRootInst *inst)
{
	struct SCMSGP_Startup *startmsg;
	struct TaskPPC *proc;
	struct MsgPortPPC *replyport;
	Object *senderobj;
	char *taskname = (char *) &deftaskname;

	DEBUGPPC("1\n");

	if (((struct SC_Class *) obj)->ClassName)
		taskname = ((struct SC_Class *) obj)->ClassName;

	// the sender object that you get back
	// methodsender is directly derived from root so that Object * for this
	// object is the pointer to the beginning of its instance data

	if (!(startmsg = (struct SCMSGP_Startup *) SC_AllocMsgPPC(SCMSG_STARTUP,sizeof(struct SCMSGP_Startup))))
		return(NULL);

	DEBUGPPC("2\n");
	if ((replyport = CreateMsgPortPPC())) // only for startup msg
	{
		startmsg->taglist = msg->ops_AttrList;
		SetReplyPortPPC((struct Message *) startmsg, replyport);

		// make object

	DEBUGPPC("3\n");
		if ((inst = SC_AllocVecPPC(((struct SC_Class *) obj)->InstOffset + ((struct SC_Class *) obj)->InstSize + sizeof(struct ThreadRootInst), MEMF_CLEAR | MEMF_ANY)))
		{
			// set class pointer for the real object that we create

			inst->oclass = (struct SC_Class *) obj;

			// now we can execute methods of this object

			// get "public" handle on baseclass instance from real beginning of obj data

			obj = (Object *) (((struct ThreadRootInst *) inst) + 1);

			// set obj pointer of real object in msg

			startmsg->threadobj = obj;

			// tell sender obj where to send its methods

	DEBUGPPC("4\n");
			if ((senderobj = SC_NewObjectPPC(NULL, SCC_METHODSENDER_NAME,SCCA_MethodSender_DestObject, obj, TAG_DONE)))
			{
	DEBUGPPC("5\n");
				if ((proc = CreateTaskPPCTags(TASKATTR_CODE,Thread_Task, TASKATTR_NAME,taskname, TASKATTR_STACKSIZE,16384, TAG_DONE)))
				{
					struct MsgPortPPC *msgsendmsgport;
					LockTaskList();
					msgsendmsgport = proc->tp_Msgport;
					UnLockTaskList();

	DEBUGPPC("6\n");
					PutMsgPPC(msgsendmsgport, (struct Message *) startmsg);		// send StartupMessage
					while (!GetMsgPPC(replyport))
						WaitPortPPC(replyport);					// wait for the reply

	DEBUGPPC("7\n");
					// the first received message must be our startupmessage
					if (startmsg->scmsg.returnvalue)
					{
	DEBUGPPC("8\n");
						DeleteMsgPortPPC(replyport);
						SC_FreeMsgPPC((struct SC_Message *) startmsg);
						SCOCLASS(obj)->ObjectCount += 1;
						SetCache(CACHE_DCACHEFLUSH, inst, sizeof(struct ThreadRootInst));
						return((ULONG) senderobj);
					}
				}
				SC_FreeVecPPC(inst);
				SC_SetAttrsPPC(senderobj,SCCA_MethodSender_DestObject,NULL,TAG_DONE);
				SC_DisposeObjectPPC(senderobj);
			}
		}
		DeleteMsgPortPPC(replyport);
	}
	SC_FreeMsgPPC((struct SC_Message *) startmsg);
	return(NULL);
}

static ULONG ThreadRoot_Dispose(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
	SC_DoMethodPPC(obj, SCCM_Exit);
	SCOCLASS(obj)->ObjectCount -= 1;
	SC_FreeVecPPC(myThreadRootInst(obj));
	return(0);
}

/****** ThreadRoot.scalos/SCCM_Init *****************************************
*
*  NAME
*	SCCM_Init
*
*  SYNOPSIS
*	BOOL SC_DoMethod(obj,SCCM_Init,struct TagItem *taglist);
*
*  FUNCTION
*	This is the standard init function. It replaces the old OM_NEW method.
*	The Rootclass will initialize the lists (notification list and object
*	list) and create a messageport.
*
*  INPUTS
*	taglist - the taglist for the init method attributes
*
*  RESULT
*	TRUE if the init method was successfull.
*
*  NOTES
*	The method will be called in the object that runs on a new thread.
*	NEVER USE OM_NEW! You'll get the wrong object and instance data!
*
*  SEE ALSO
*	SCCM_Exit
*
*****************************************************************************
*/

static ULONG ThreadRoot_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init * msg, struct ThreadRootInst *inst)
{
	inst = myThreadRootInst(obj);

	NewList((struct List *) &inst->objectlist);
	NewList((struct List *) &inst->notilist);
	if (InitSemaphorePPC(&inst->objectlistsem))
	{
		if (InitSemaphorePPC(&inst->notilistsem))
		{
			if ((inst->msgport = CreateMsgPortPPC()))
				return(TRUE);
			else
				RemSemaphorePPC(&inst->notilistsem);
		}
		RemSemaphorePPC(&inst->objectlistsem);
	}
	return(FALSE);
}

/****** ThreadRoot.scalos/SCCM_Exit *****************************************
*
*  NAME
*	SCCM_Exit
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_Exit);
*
*  FUNCTION
*	This is the replacement for OM_DISPOSE. Here your object should free all
*	resources.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*	SCCM_Exit will be called from the thread of the object.
*
*  SEE ALSO
*	SCCM_Init
*
*****************************************************************************
*/

static ULONG ThreadRoot_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
	struct MinNode *node;

	inst = myThreadRootInst(obj);
	ObtainSemaphorePPC(&inst->objectlistsem);
	while (!(IsListEmpty((struct List *) &inst->objectlist)))
	{
		node = inst->objectlist.mlh_Head;
		RemovePPC((struct Node *) node);
		SC_DoMethodPPC(SCBASEOBJECT(node), OM_DISPOSE);
	}
	ReleaseSemaphorePPC(&inst->objectlistsem);
	ObtainSemaphorePPC(&inst->notilistsem);
	FreeAllNodesPPC(&inst->notilist);
	ReleaseSemaphorePPC(&inst->notilistsem);
	DeleteMsgPortPPC(inst->msgport);
	return(0);
}

/****** ThreadRoot.scalos/OM_ADDMEMBER **************************************
*
*  NAME
*	OM_ADDMEMBER
*
*  SYNOPSIS
*	SC_DoMethod(obj,OM_ADDMEMBER,Object *childobj);
*
*  FUNCTION
*	Adds an object to the child list of this object.
*
*  INPUTS
*	childobj - Object to add.
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*	OM_REMMEMBER
*
*****************************************************************************
*/

static ULONG ThreadRoot_AddMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct ThreadRootInst *inst)
{
	inst = myThreadRootInst(obj);

	ObtainSemaphorePPC(&inst->objectlistsem);
	AddTailPPC((struct List *) &inst->objectlist, (struct Node *) &_SCOBJECT(msg->opam_Object)->sco_Node);
	ReleaseSemaphorePPC(&inst->objectlistsem);
	return(0);
}

/****** ThreadRoot.scalos/OM_REMMEMBER **************************************
*
*  NAME
*	OM_REMMEMBER
*
*  SYNOPSIS
*	SC_DoMethod(obj,OM_REMMEMBER,Object *childobj);
*
*  FUNCTION
*	Removes an object from the child list of this object.
*
*  INPUTS
*	childobj - Object to remove.
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*	OM_ADDMEMBER
*
*****************************************************************************
*/

static ULONG ThreadRoot_RemMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct ThreadRootInst *inst)
{
	inst = myThreadRootInst(obj);

	ObtainSemaphorePPC(&inst->objectlistsem);
	RemovePPC((struct Node *) &_SCOBJECT(obj)->sco_Node);
	ReleaseSemaphorePPC(&inst->objectlistsem);
	return(0);
}

/****** ThreadRoot.scalos/SCCM_Notify ***************************************
*
*  NAME
*	SCCM_Notify
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_Notify,ULONG TriggerAttr, ULONG TriggerVal, Object *DestObj, ULONG NumArgs, ...);
*
*  FUNCTION
*	This method adds a notify to the notification list. 
*	If an attribute was set, the Rootclass will check the notification list
*	for an attribute to trigger. If so and the attribute has changed and
*	is the same as the trigger value the Rootclass will call a method in
*	the given DestObj. Attributes in the DestObj can be set with the SCCM_Set
*	method.
*	SCCV_TriggerValue reflects the state of the attribute that triggers the
*	destination attribute, if any.
*	If you want to change an attribute as the result of a notification you
*	can use SCCV_TriggerValue, which reflects the state of the source
*	attribute TriggerAttr as the value for the destination attribute to
*	change the attribute of the destination object to the same value.
*	Of course the possible values of the source attribute should make sense
*	to the destination attribute.
*
*  INPUTS
*	TriggerAttr - an attribute to trigger
*	TriggerVal - the value when the notify should be done or
*	             SCCV_EveryTime if the notify should be done when every the
*	             triggerattr was changed
*	DestObj - destination object to do the notify method
*	NumArgs - number of following arguments
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/

static ULONG ThreadRoot_Notify(struct SC_Class *cl, Object *obj, struct SCCP_Notify *msg, struct ThreadRootInst *inst)
{
	struct NotifyNode *buffer;
	ULONG cpsize = (msg->numargs)*sizeof(ULONG) + sizeof(struct NotifyNode) - 3*sizeof(ULONG); // complete parameters length

	inst = myThreadRootInst(obj);
	ObtainSemaphorePPC(&inst->notilistsem);
	if ((buffer = (struct NotifyNode *) AllocNodePPC(&inst->notilist,cpsize)))
		memcpy(&buffer->TriggerAttr, &msg->TriggerAttr, cpsize - sizeof(struct MinNode));
	ReleaseSemaphorePPC(&inst->notilistsem);
	return(TRUE);
}

void ThreadRoot_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct ThreadRootInst *inst)
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

	inst = myThreadRootInst(obj);
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

/****** ThreadRoot.scalos/SCCM_Input ****************************************
*
*  NAME
*	SCCM_Input
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_Input);
*
*  FUNCTION
*	This method manages the input handling of the thread. It's a simple
*	message loop which understands some internal messages too.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*	If your method needs some time to be executed you should call this
*	method from time to time to avoid locking situations so that a
*	potential sender won`t lock because you don`t receive its message
*	(generated from its method).
*
*  SEE ALSO
*
*****************************************************************************
*
* InternReply - checks the message before reply. If it's already a reply or
*               it has no replyport, then it will free the message. 
*/

static void InternReply(struct Message *msg)
{
	if ((msg->mn_Node.ln_Type == NT_REPLYMSG) || (!(msg->mn_ReplyPort)))
		SC_FreeMsgPPC((struct SC_Message *) msg);
	else
		ReplyMsgPPC(msg);
}

static ULONG ThreadRoot_Input(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
	struct	Message *message;
	ULONG	msgtype;

	inst = myThreadRootInst(obj);
	while ((message = GetMsgPPC((struct MsgPortPPC *) inst->msgport)))
	{
		if (message->mn_Node.ln_Type == NT_MESSAGE)
		{
			if ((msgtype = SC_IsScalosMsgPPC(message)))
			{
				if (msgtype == SCMSG_METHOD) // internal msg for a method
				{
					((struct SC_Message *) message)->returnvalue = SC_DoMethodAPPC(obj, (Msg) &((struct SCMSGP_Method *) message)->methodarg1);
					if (((struct SCMSGP_Method *) message)->methodarg1 == OM_DISPOSE)
					{
						InternReply(message);
						return(TRUE);
					}
					InternReply(message);
				}
				else
				{
					SC_DoMethodPPC(obj, SCCM_MessageReceived, message);
					InternReply(message);
				}
			}
			else
			{
				SC_DoMethodPPC(obj, SCCM_MessageReceived, message);
				ReplyMsgPPC(message);
			}
		}
		else
		{
			SC_DoMethodPPC(obj, SCCM_ReplyReceived, message);
		}
	}
	return(FALSE);
}


/****** ThreadRoot.scalos/SCCM_MessageReceived ******************************
*
*  NAME
*	SCCM_MessageReceived
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_MessageReceived, struct Message *message);
*
*  FUNCTION
*	Your object will get this method if the thread has received a message
*	that scalos doesn`t handle in SCCM_Input.
*
*  INPUTS
*	message - exec message which was received by your thread
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/

/****** ThreadRoot.scalos/SCCM_ReplyReceived ********************************
*
*  NAME
*	SCCM_ReplyReceived
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_ReplyReceived, struct Message *message);
*
*  FUNCTION
*	Your object will get this method if the thread has received an unknown
*	(non scalos) message and it is a reply message. If this is your message,
*	free the data and don't call your superclass for this method.
*
*  INPUTS
*	message - exec message which was received by your thread
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*
*****************************************************************************
*/
static void ThreadRoot_ReplyReceived(struct SC_Class *cl, Object *obj, struct SCCP_ReplyReceived *msg, struct ThreadRootInst *inst)
{
	if (SC_IsScalosMsgPPC(msg->message))
		SC_FreeMsgPPC((struct SC_Message *) msg->message);
}

/****** ThreadRoot.scalos/SCCM_LockObjectList *******************************
*
*  NAME
*	SCCM_LockObjectList
*
*  SYNOPSIS
*	struct MinList *SC_DoMethod(obj,SCCM_LockObjectList, ULONG locktype);
*
*  FUNCTION
*	Using this method you can access the objectlist of an object. This list
*	is fully semaphore protect.
*	The return value can be NULL even for exclusive or shared locks, because
*	the root class may provide a semaphore timeout in future.
*
*  INPUTS
*	locktype - one for these values :
*	             SCCV_LockExclusive - for write access to the list, only one
*	                                  task can have a lock in the at one time
*	             SCCV_LockShared - for read access to the list, multiple
*	                               tasks can have read access on list
*	             SCCV_LockAttempt - try to lock the list for a exclusiv lock,
*	                                no waiting will be done
*	             SCCV_LockAttemptShared - try to lock the list for a shared
*	                                      lock
*
*  RESULT
*	The objectlist or NULL if the lock fails.
*
*  NOTES
*	If this function was successful you have to release the lock using the
*	SCCM_UnlockObjectList method.
*
*  SEE ALSO
*	SCCM_UnlockObjectList, OM_ADDMEMBER, OM_REMMEMBER
*
*****************************************************************************
*/

static struct MinList *ThreadRoot_LockObjectList(struct SC_Class *cl, Object *obj, struct SCCP_LockObjectList *msg, struct ThreadRootInst *inst)
{
	inst = myThreadRootInst(obj);
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

/****** ThreadRoot.scalos/SCCM_UnlockObjectList *****************************
*
*  NAME
*	SCCM_UnlockObjectList
*
*  SYNOPSIS
*	SC_DoMethod(obj,SCCM_UnlockObjectList);
*
*  FUNCTION
*	Release the lock on the objectlist which was made using
*	SCCM_LockObjectList.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*	SCCM_LockObjectList, OM_ADDMEMBER, OM_REMMEMBER
*
*****************************************************************************
*/

static void ThreadRoot_UnlockObjectList(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
	inst = myThreadRootInst(obj);
	ReleaseSemaphorePPC(&inst->objectlistsem);
}

/* ----------------------------------------------------------------------- */

static ULONG ThreadRoot_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct ThreadRootInst *inst )
{
	inst = myThreadRootInst(obj);

	if (msg->opg_AttrID == SCCA_MsgPort)
	{
		*(msg->opg_Storage) = (ULONG) inst->msgport;
		return( TRUE );
	}
	return(FALSE);
}

/* ----------------------------------------------------------------------- */

static ULONG ThreadRoot_Default(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
	return(0);
}

struct SC_MethodData PPCThreadRootMethods[] = 
{
	{ OM_NEW, (ULONG) ThreadRoot_New, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
	{ OM_DISPOSE, (ULONG) ThreadRoot_Dispose, sizeof(ULONG), 0, NULL },
	{ OM_SET, (ULONG) ThreadRoot_Set, sizeof(struct opSet), SCMDF_FULLFLUSH, NULL },
	{ OM_GET, (ULONG) ThreadRoot_Get, sizeof(struct opGet), SCMDF_FULLFLUSH, NULL },
	{ OM_ADDMEMBER, (ULONG) ThreadRoot_AddMember, sizeof(struct opMember), 0, NULL },
	{ OM_REMMEMBER, (ULONG) ThreadRoot_RemMember, sizeof(struct opMember), 0, NULL },
	{ SCCM_Init, (ULONG) ThreadRoot_Init, sizeof(struct SCCP_Init), SCMDF_FULLFLUSH, NULL },
	{ SCCM_Exit, (ULONG) ThreadRoot_Exit, sizeof(ULONG), 0, NULL },
	{ SCCM_Notify, (ULONG) ThreadRoot_Notify, sizeof(struct SCCP_Notify) + sizeof(ULONG)*32, 0, NULL },	// maximal 32 args with notifies
	{ SCCM_Input, (ULONG) ThreadRoot_Input, sizeof(ULONG), 0, NULL },
	{ SCCM_MessageReceived, NULL, sizeof(struct SCCP_MessageReceived), 0, NULL },
	{ SCCM_ReplyReceived, (ULONG) ThreadRoot_ReplyReceived, sizeof(struct SCCP_ReplyReceived), 0, NULL },
	{ SCCM_LockObjectList, (ULONG) ThreadRoot_LockObjectList, sizeof(struct SCCP_LockObjectList), 0, NULL },
	{ SCCM_UnlockObjectList, (ULONG) ThreadRoot_UnlockObjectList, sizeof(ULONG), 0, NULL },
	{ SCMETHOD_DONE, (ULONG) ThreadRoot_Default, 0, 0, NULL }
};

