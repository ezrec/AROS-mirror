// tabsize ts=4

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/utility_protos.h>
#include <intuition/classusr.h>
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>

#include "scalos.h"
#include "scalosintern.h"
#include "RootClass.h"
#include "subroutines.h"
#include "ThreadRootClass.h"
#include "MethodSenderClass.h"

#include "CompilerSupport.h"

#include "scalos_protos.h"

static char deftaskname[] = "private_object";

/****** ThreadRoot.scalos/--background **************************************
*
*   ThreadRoot class has the same functionality as Rootclass.
*   It can be regarded as a mirror of Rootclass, because the only difference
*   is that every object of this class or a derived class of this class will
*   run on its own thread.
*
*  USING A SUBCLASS
*   If you do a NewObject() on a ThreadRoot subclass to make a new object it
*   will not return an object of this class!
*   Instead it will return a MethodSender object on success.
*   All direct accesses to the returned object structure itself are forbidden
*   because noone knows what it looks like.
*   Generally you should avoid accesses to the object structure.
*
*   If you make a new class and derive it from ThreadRoot class all objects
*   of this class will run on another thread than the thread that called
*   NewObject() to serve the new object. So if all the new objects of
*   a class should lie on a new thread or on the same is the decision of the
*   programmer of the class. As a user you have no influence on that.
*
*   If you make a SC_DoMethod() to such an object it will create a message
*   and send it to the thread of the object and wait for the reply. This
*   does always work, if the object is running on a separate thread or not
*   doesn`t matter. But this makes the hole procedure synchronous, because
*   the SC_DoMethod() will not return until the method has been performed
*   completely.
*
*   To optimize your code make it run asynchronously. You can use
*   DoMsgMethod() of the support package.
*
*   When using SC_DoMsgMethod() you can check for the pending reply later.
*   When you have got the reply you can check the return value. Than you have
*   to free the message. You always have to receive the reply.
*   This way the methods can run some time in parallel.
*
*   Some methods may only be called directly (synchronously) using
*   SC_DoMethod() like SCCM_Area_Draw. If a method may or may not be called
*   asynchronously is set static in each class for each method and is not the
*   decision of the caller.
*   On the other hand, a method that can be performed asynchronously may
*   always be done synchronously.
*   You will get a failure from SC_DoMsgMethod if the method is a direct
*   method or if the object is running on the same thread as the caller.
*
*   To say it again, DoMsgMethod() only works if the object runs on a
*   different thread than the thread of the caller of this function. It
*   returns FALSE otherwise, so you should do a SC_DoMethod() to call a
*   method of that object on failure of DoMsgMethod().
*
*   Deadlocks are one major problem of threads. To avoid these problems
*   we'll give some orders:
*       - never do a method from within a method, except the object is a
*         child of your object or it's a allowed for that object
*       - use notifies to communicate between objects
*
*  WRITING YOUR OWN SUBCLASS
*   There's nothing completly different in writing a subclass of Root class
*   and ThreadRoot class. If you only allow standard SC_DoMethod calls, you
*   don't have to protect your instance data. If a method of your call may
*   be called asynchronously is part of your code.
*   If you have one or more direct methods in your class, you have to
*   protect your instance data using semaphores. If you lock a semphore
*   longer than for one method you may get deadlocks.
*
*   A normal class that does not allow any direct methods will receive
*   its methods outside of the method processing, so only one method will
*   be performed at a time for one object of such a class. There is no other
*   entry point to the methods so such a class doesn`t need to protect its
*   instance data if SCCM_Input is not used.
*
*   If you make a class without direct methods the called methods may
*   still be called in any order because there may be many callers to your
*   methods. But there are certain classes which have objects that should only
*   be called by special callers in a special order like the methods of
*   Area class.
*   
*   You must not overwrite the OM_NEW method to add some own initializations,
*   instead you have to use SCCM_Init/SCCM_Exit method for this purpose. If
*   you write a class which should be able to get private messages for itself,
*   you can use the messageport of the object itself. Anytime your objects
*   receives an unknown message it will send a SCCM_MessageReceived method
*   to itself. If the message is a reply message it will send
*   SCCM_ReplyReceived.
*
*****************************************************************************
*
*--------------------------- ThreadTask -------------------------------
*
*   This is the taskfunction of the object-thread. It will receive the
*   startup message and then initiate the object. After this it will go
*   into the input loop.
*
*/

static void SAVEDS Thread_Task(void)
{
        struct SCMSGP_Startup *startmsg;
        struct Process *owntask = (struct Process *) FindTask(NULL);
        Object *threadobj;
        struct MsgPort *msgport;

        while (!(startmsg = (struct SCMSGP_Startup *) GetMsg(&owntask->pr_MsgPort)))
                WaitPort(&owntask->pr_MsgPort);

        threadobj = startmsg->threadobj;
        if (startmsg->scmsg.returnvalue = SC_DoMethod(threadobj, SCCM_Init, startmsg->taglist))
        {
                if (!(msgport = (struct MsgPort *) get(threadobj, SCCA_MsgPort)))
                {
                        SC_DoMethod(threadobj, SCCM_Exit);
                        startmsg->scmsg.returnvalue = NULL;
                        ReplyMsg((struct Message *) startmsg);
                        return;
                }

                ReplyMsg((struct Message *) startmsg);
                while (!(SC_DoMethod(threadobj, SCCM_Input)))
                {
                        WaitPort(msgport);
                }
        }
        else
                ReplyMsg((struct Message *) startmsg);

        return;
}


/****** ThreadRoot.scalos/SCCA_MsgPort **************************************
*
*  NAME
*   SCCA_MsgPort -- (V40) ..G (struct MsgPort *)
*
*  FUNCTION
*   Getting this attribute will return the threads messageport.
*
*****************************************************************************
*/

/*--------------------------- Functions --------------------------------*/

static ULONG ThreadRoot_New(struct SC_Class *cl, Object *obj, struct opSet *msg, struct ThreadRootInst *inst)
{
        struct SCMSGP_Startup *startmsg;
        struct Process *proc;
        struct MsgPort *replyport;
        Object *senderobj;
        char *taskname = (char *) &deftaskname;

        if (((struct SC_Class *) obj)->ClassName)
                taskname = ((struct SC_Class *) obj)->ClassName;

        // the sender object that you get back
        // methodsender is directly derived from root so that Object * for this
        // object is the pointer to the beginning of its instance data

        if (!(startmsg = (struct SCMSGP_Startup *) SC_AllocMsg(SCMSG_STARTUP,sizeof(struct SCMSGP_Startup))))
                return(NULL);

        if (replyport = CreateMsgPort()) // only for startup msg
        {
                startmsg->taglist = msg->ops_AttrList;
                startmsg->scmsg.execmsg.mn_ReplyPort = replyport;

                // make object

                if (inst = AllocVec(((struct SC_Class *) obj)->InstOffset + ((struct SC_Class *) obj)->InstSize + sizeof(struct ThreadRootInst), MEMF_CLEAR | MEMF_ANY))
                {
                        // set class pointer for the real object that we create

                        inst->oclass = (struct SC_Class *) obj;

                        // now we can execute methods of this object

                        // get "public" handle on baseclass instance from real beginning of obj data

                        obj = (Object *) (((struct ThreadRootInst *) inst) + 1);

                        // set obj pointer of real object in msg

                        startmsg->threadobj = obj;

                        // tell sender obj where to send its methods

                        if (senderobj = SC_NewObject(NULL, SCC_METHODSENDER_NAME,SCCA_MethodSender_DestObject,obj, TAG_DONE))
                        {
                                if (proc = CreateNewProcTags(NP_Entry,Thread_Task, NP_Name,taskname, NP_StackSize,8192, TAG_DONE))
                                {
                                        PutMsg(&proc->pr_MsgPort, (struct Message *) startmsg);     // send StartupMessage
                                        while (!GetMsg(replyport))
                                                WaitPort(replyport);                    // wait for the reply

                                        // the first received message must be our startupmessage
                                        if (startmsg->scmsg.returnvalue)
                                        {
                                                DeleteMsgPort(replyport);
                                                SC_FreeMsg((struct SC_Message *) startmsg);
                                                SCOCLASS(obj)->ObjectCount += 1;
                                                return((ULONG) senderobj);
                                        }
                                }
                        }
                        FreeVec(inst);
                        SC_SetAttrs(senderobj,SCCA_MethodSender_DestObject,NULL,TAG_DONE);
                        SC_DisposeObject(senderobj);
                }
                DeleteMsgPort(replyport);
        }
        SC_FreeMsg((struct SC_Message *) startmsg);
        return(NULL);
}

static ULONG ThreadRoot_Dispose(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
        SC_DoMethod(obj, SCCM_Exit);
        SCOCLASS(obj)->ObjectCount -= 1;
        FreeVec(myThreadRootInst(obj));
        return(0);
}

/****** ThreadRoot.scalos/SCCM_Init *****************************************
*
*  NAME
*   SCCM_Init
*
*  SYNOPSIS
*   BOOL SC_DoMethod(obj,SCCM_Init,struct TagItem *taglist);
*
*  FUNCTION
*   This is the standard init function. It replaces the old OM_NEW method.
*   The Rootclass will initialize the lists (notification list and object
*   list) and create a messageport.
*
*  INPUTS
*   taglist - the taglist for the init method attributes
*
*  RESULT
*   TRUE if the init method was successfull.
*
*  NOTES
*   The method will be called in the object that runs on a new thread.
*   NEVER USE OM_NEW! You'll get the wrong object and instance data!
*
*  SEE ALSO
*   SCCM_Exit
*
*****************************************************************************
*/

static ULONG ThreadRoot_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init * msg, struct ThreadRootInst *inst)
{
        inst = myThreadRootInst(obj);

        NewList((struct List *) &inst->objectlist);
        NewList((struct List *) &inst->notilist);
        InitSemaphore(&inst->objectlistsem);
        InitSemaphore(&inst->notilistsem);
        if (!(inst->msgport = CreateMsgPort()))
                return(FALSE);
        return(TRUE);
}

/****** ThreadRoot.scalos/SCCM_Exit *****************************************
*
*  NAME
*   SCCM_Exit
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Exit);
*
*  FUNCTION
*   This is the replacement for OM_DISPOSE. Here your object should free all
*   resources.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*   SCCM_Exit will be called from the thread of the object.
*
*  SEE ALSO
*   SCCM_Init
*
*****************************************************************************
*/

static ULONG ThreadRoot_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
        struct MinNode *node;

        inst = myThreadRootInst(obj);
        ObtainSemaphore(&inst->objectlistsem);
        while (!(IsListEmpty((struct List *) &inst->objectlist)))
        {
                node = inst->objectlist.mlh_Head;
                Remove((struct Node *) node);
                SC_DoMethod(SCBASEOBJECT(node), OM_DISPOSE);
        }
        ReleaseSemaphore(&inst->objectlistsem);
        ObtainSemaphore(&inst->notilistsem);
        FreeAllNodes(&inst->notilist);
        ReleaseSemaphore(&inst->notilistsem);
        DeleteMsgPort(inst->msgport);
        return(0);
}

/****** ThreadRoot.scalos/OM_ADDMEMBER **************************************
*
*  NAME
*   OM_ADDMEMBER
*
*  SYNOPSIS
*   SC_DoMethod(obj,OM_ADDMEMBER,Object *childobj);
*
*  FUNCTION
*   Adds an object to the child list of this object.
*
*  INPUTS
*   childobj - Object to add.
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   OM_REMMEMBER
*
*****************************************************************************
*/

static ULONG ThreadRoot_AddMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct ThreadRootInst *inst)
{
        inst = myThreadRootInst(obj);

        ObtainSemaphore(&inst->objectlistsem);
        AddTail((struct List *) &inst->objectlist, (struct Node *) &_SCOBJECT(msg->opam_Object)->sco_Node);
        ReleaseSemaphore(&inst->objectlistsem);
        return(0);
}

/****** ThreadRoot.scalos/OM_REMMEMBER **************************************
*
*  NAME
*   OM_REMMEMBER
*
*  SYNOPSIS
*   SC_DoMethod(obj,OM_REMMEMBER,Object *childobj);
*
*  FUNCTION
*   Removes an object from the child list of this object.
*
*  INPUTS
*   childobj - Object to remove.
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   OM_ADDMEMBER
*
*****************************************************************************
*/

static ULONG ThreadRoot_RemMember(struct SC_Class *cl, Object *obj, struct opMember *msg, struct ThreadRootInst *inst)
{
        inst = myThreadRootInst(obj);

        ObtainSemaphore(&inst->objectlistsem);
        Remove((struct Node *) &_SCOBJECT(obj)->sco_Node);
        ReleaseSemaphore(&inst->objectlistsem);
        return(0);
}

/****** ThreadRoot.scalos/SCCM_Notify ***************************************
*
*  NAME
*   SCCM_Notify
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Notify,ULONG TriggerAttr, ULONG TriggerVal, Object *DestObj, ULONG NumArgs, ...);
*
*  FUNCTION
*   This method adds a notify to the notification list. 
*   If an attribute was set, the Rootclass will check the notification list
*   for an attribute to trigger. If so and the attribute has changed and
*   is the same as the trigger value the Rootclass will call a method in
*   the given destination object DestObj. Attributes in the DestObj can be
*   set with the SCCM_Set method.
*   SCCV_TriggerValue reflects the state of the attribute that triggers the
*   destination attribute, if any.
*   If you want to change an attribute as the result of a notification you
*   can use SCCV_TriggerValue, which reflects the state of the source
*   attribute TriggerAttr as the value for the destination attribute to
*   change the attribute of the destination object to the same value.
*   Of course the possible values of the source attribute should make sense
*   to the destination attribute.
*
*  INPUTS
*   TriggerAttr - an attribute to trigger
*   TriggerVal - the value when the notify should be done or
*                SCCV_EveryTime if the notify should be done when every the
*                triggerattr was changed
*   DestObj - destination object to do the notify method
*   NumArgs - number of following arguments
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
        ObtainSemaphore(&inst->notilistsem);
        if (buffer = (struct NotifyNode *) AllocNode(&inst->notilist,cpsize))
                CopyMem(&msg->TriggerAttr, &buffer->TriggerAttr, cpsize - sizeof(struct MinNode));
        ReleaseSemaphore(&inst->notilistsem);
        return TRUE;
}

void ThreadRoot_Set(struct SC_Class *cl, Object *obj, struct opSet *msg, struct ThreadRootInst *inst)
{
        struct  TagItem *tmptaglist = msg->ops_AttrList;
        struct  TagItem **taglist = &tmptaglist;
        struct  TagItem *tag;
        struct  NotifyNode *node;
        ULONG   tmpvalue;
        ULONG   tmparray[10];
        ULONG   *srcarray;
        ULONG   *destarray;
        int     i;

        inst = myThreadRootInst(obj);
        ObtainSemaphoreShared(&inst->notilistsem);
        while(tag = NextTagItem(taglist)) // search in taglist
        {
                // look for a attribute that we should trigger
                for (node = (struct NotifyNode *) inst->notilist.mlh_Head; node->node.mln_Succ; node = (struct NotifyNode *) node->node.mln_Succ)
                {
                        if (tag->ti_Tag == node->TriggerAttr) // if found
                        {
                                if ( !(SC_GetAttr(tag->ti_Tag,obj,&tmpvalue)) || (tag->ti_Data != tmpvalue) ) // if current attribute from object doesn`t exist or the attribute has been changed

                                        /* execute only if attr-value has changed */
                                        if ((node->TriggerVal == SCCV_EveryTime) || (tag->ti_Data == node->TriggerVal)) // SCCV_EveryTime: notify everytime the values changes
                                        {
                                                if (node->arg_method == SCCM_Set)
                                                {
                                                        if (node->arg_value == SCCV_TriggerValue)
                                                                SC_SetAttrs(node->DestObject, node->arg_attr, tag->ti_Data, TAG_DONE);
                                                        else
                                                                SC_SetAttrs(node->DestObject, node->arg_attr, node->arg_value);
                                                }
                                                else
                                                {
                                                        srcarray = &node->arg_method;
                                                        if (node->numargs > 10)
                                                                destarray = AllocMem(node->numargs * sizeof(ULONG), MEMF_PUBLIC);
                                                        else
                                                                destarray = (ULONG *) &tmparray;
                                                        for (i = 0; i < node->numargs; i++)
                                                        {
                                                                if (srcarray[i] == SCCV_TriggerValue)
                                                                        destarray[i] = tag->ti_Data;
                                                                else
                                                                        destarray[i] = srcarray[i];
                                                        }
                                                        SC_DoMethodA(node->DestObject, (Msg) destarray);
                                                        if (node->numargs > 10)
                                                                FreeMem(destarray, node->numargs * sizeof(ULONG));
                                                }
                                        }
                        }
                }
        }
        ReleaseSemaphore(&inst->notilistsem);
}

/****** ThreadRoot.scalos/SCCM_Input ****************************************
*
*  NAME
*   SCCM_Input
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_Input);
*
*  FUNCTION
*   This method manages the input handling of the thread. It's a simple
*   message loop which understands some internal messages too.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*   If your method needs some time to be executed you should call this
*   method from time to time to avoid locking situations so that a
*   potential sender won`t lock because you don`t receive its message
*   generated indirectly by its method.
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
                SC_FreeMsg((struct SC_Message *) msg);
        else
                ReplyMsg(msg);
}

static ULONG ThreadRoot_Input(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
        struct  Message *message;
        ULONG   msgtype;

        inst = myThreadRootInst(obj);
        while (message = GetMsg(inst->msgport))
        {
                if (message->mn_Node.ln_Type == NT_MESSAGE)
                {
                        if (msgtype = SC_IsScalosMsg(message))
                        {
                                if (msgtype == SCMSG_METHOD) // internal msg for a method
                                {
                                        ((struct SC_Message *) message)->returnvalue = SC_DoMethodA(obj, (Msg) &((struct SCMSGP_Method *) message)->methodarg1);
                                        if (((struct SCMSGP_Method *) message)->methodarg1 == OM_DISPOSE)
                                        {
                                                InternReply(message);
                                                return(TRUE);
                                        }
                                        InternReply(message);
                                }
                                else
                                {
                                        SC_DoMethod(obj, SCCM_MessageReceived, message);
                                        InternReply(message);
                                }
                        }
                        else
                        {
                                SC_DoMethod(obj, SCCM_MessageReceived, message);
                                ReplyMsg(message);
                        }
                }
                else
                {
                        SC_DoMethod(obj, SCCM_ReplyReceived, message);
                }
        }
        return(FALSE);
}


/****** ThreadRoot.scalos/SCCM_MessageReceived ******************************
*
*  NAME
*   SCCM_MessageReceived
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_MessageReceived, struct Message *message);
*
*  FUNCTION
*   Your object will get this method if the thread has received a message
*   that scalos doesn`t handle in SCCM_Input.
*
*  INPUTS
*   message - exec message which was received by your thread
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
*   SCCM_ReplyReceived
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_ReplyReceived, struct Message *message);
*
*  FUNCTION
*   Your object will get this method if the thread has received an unknown
*   (non scalos) message and it is a reply message. If this is your message,
*   free the data and don't call your superclass for this method.
*
*  INPUTS
*   message - exec message which was received by your thread
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
        if (SC_IsScalosMsg(msg->message))
                SC_FreeMsg((struct SC_Message *) msg->message);
}

/****** ThreadRoot.scalos/SCCM_LockObjectList *******************************
*
*  NAME
*   SCCM_LockObjectList
*
*  SYNOPSIS
*   struct MinList *SC_DoMethod(obj,SCCM_LockObjectList, ULONG locktype);
*
*  FUNCTION
*   Using this method you can access the objectlist of an object. This list
*   is fully semaphore protect.
*   The return value can be NULL even for exclusive or shared locks, because
*   the root class may provide a semaphore timeout in the future.
*
*  INPUTS
*   locktype - one for these values :
*                SCCV_LockExclusive - for write access to the list, only one
*                                     task can have a lock in the at one time
*                SCCV_LockShared - for read access to the list, multiple
*                                  tasks can have read access on list
*                SCCV_LockAttempt - try to lock the list for a exclusiv lock,
*                                   no waiting will be done
*                SCCV_LockAttemptShared - try to lock the list for a shared
*                                         lock
*
*  RESULT
*   The objectlist or NULL if the lock fails.
*
*  NOTES
*   If this function was successful you have to release the lock using the
*   SCCM_UnlockObjectList method.
*
*  SEE ALSO
*   SCCM_UnlockObjectList, OM_ADDMEMBER, OM_REMMEMBER
*
*****************************************************************************
*/

static struct MinList *ThreadRoot_LockObjectList(struct SC_Class *cl, Object *obj, struct SCCP_LockObjectList *msg, struct ThreadRootInst *inst)
{
        inst = myThreadRootInst(obj);
        switch (msg->locktype)
        {
                case SCCV_LockExclusive :
                        ObtainSemaphore(&inst->objectlistsem);
                        return(&inst->objectlist);

                case SCCV_LockShared :
                        ObtainSemaphoreShared(&inst->objectlistsem);
                        return(&inst->objectlist);

                case SCCV_LockAttempt :
                        if (AttemptSemaphore(&inst->objectlistsem))
                                return(&inst->objectlist);
                        else
                                return(NULL);

                case SCCV_LockAttemptShared :
                        if (AttemptSemaphoreShared(&inst->objectlistsem))
                                return(&inst->objectlist);
                        else
                                return(NULL);
        }
}

/****** ThreadRoot.scalos/SCCM_UnlockObjectList *****************************
*
*  NAME
*   SCCM_UnlockObjectList
*
*  SYNOPSIS
*   SC_DoMethod(obj,SCCM_UnlockObjectList);
*
*  FUNCTION
*   Release the lock on the objectlist which was made using
*   SCCM_LockObjectList.
*
*  INPUTS
*
*  RESULT
*
*  NOTES
*
*  SEE ALSO
*   SCCM_LockObjectList, OM_ADDMEMBER, OM_REMMEMBER
*
*****************************************************************************
*/

static void ThreadRoot_UnlockObjectList(struct SC_Class *cl, Object *obj, Msg msg, struct ThreadRootInst *inst)
{
        inst = myThreadRootInst(obj);
        ReleaseSemaphore(&inst->objectlistsem);
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

struct SC_MethodData ThreadRootMethods[] = 
{
        { OM_NEW,                   (ULONG) ThreadRoot_New,                 sizeof(struct opSet),                           SCMDF_FULLFLUSH,    NULL },
        { OM_DISPOSE,               (ULONG) ThreadRoot_Dispose,             sizeof(ULONG),                                  0,                  NULL },
        { OM_SET,                   (ULONG) ThreadRoot_Set,                 sizeof(struct opSet),                           SCMDF_FULLFLUSH,    NULL },
        { OM_GET,                   (ULONG) ThreadRoot_Get,                 sizeof(struct opGet),                           SCMDF_FULLFLUSH,    NULL },
        { OM_ADDMEMBER,             (ULONG) ThreadRoot_AddMember,           sizeof(struct opMember),                        0,                  NULL },
        { OM_REMMEMBER,             (ULONG) ThreadRoot_RemMember,           sizeof(struct opMember),                        0,                  NULL },
        { SCCM_Init,                (ULONG) ThreadRoot_Init,                sizeof(struct SCCP_Init),                       SCMDF_FULLFLUSH,    NULL },
        { SCCM_Exit,                (ULONG) ThreadRoot_Exit,                sizeof(ULONG),                                  0,                  NULL },
        { SCCM_Notify,              (ULONG) ThreadRoot_Notify,              sizeof(struct SCCP_Notify) + sizeof(ULONG)*32,  0,                  NULL }, // maximal 32 args with notifies
        { SCCM_Input,               (ULONG) ThreadRoot_Input,               sizeof(ULONG),                                  0,                  NULL },
        { SCCM_MessageReceived,     NULL,                                   sizeof(struct SCCP_MessageReceived),            0,                  NULL },
        { SCCM_ReplyReceived,       (ULONG) ThreadRoot_ReplyReceived,       sizeof(struct SCCP_ReplyReceived),              0,                  NULL },
        { SCCM_LockObjectList,      (ULONG) ThreadRoot_LockObjectList,      sizeof(struct SCCP_LockObjectList),             0,                  NULL },
        { SCCM_UnlockObjectList,    (ULONG) ThreadRoot_UnlockObjectList,    sizeof(ULONG),                                  0,                  NULL },
        { SCMETHOD_DONE,            (ULONG) ThreadRoot_Default,             0,                                              0,                  NULL }
};

