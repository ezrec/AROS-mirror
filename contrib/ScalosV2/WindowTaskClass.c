// :ts=4 (Tabsize)

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
#include <clib/intuition_protos.h>
#include <clib/utility_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <exec/lists.h>
#include "debug.h"

#include "Scalos.h"
#include "WindowTaskClass.h"
#include "ScalosIntern.h"
#include "SubRoutines.h"

#include "scalos_protos.h"

static void WinTask_settags(struct TagItem *taglist, struct WindowTaskInst *inst, struct SC_Class *cl, Object *obj)
{
        struct TagItem **tags;
        struct TagItem *tag;

        tags = &taglist;
        while(tag = NextTagItem(tags))
        {
                switch (tag->ti_Tag)
                {
                        case SCCA_WindowTask_Window :
                                // add a window as child of this windowtask
                                SC_DoSuperMethod(cl, obj, OM_ADDMEMBER, tag->ti_Data);
                                break;
                }
        }
}


static ULONG WinTask_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct WindowTaskInst *inst)
{
        struct List *objlist;

        if (!(SC_DoSuperMethodA(cl,obj, (Msg) msg)))
                return(FALSE);

        WinTask_settags(msg->ops_AttrList, inst, cl, obj);

        if (objlist = (struct List *) SC_DoMethod(obj, SCCM_LockObjectList,SCCV_LockShared))
        {
                if (IsListEmpty(objlist))
                {
                        SC_DoMethod(obj, SCCM_UnlockObjectList);
                        return(FALSE);
                }
                SC_DoMethod(obj, SCCM_UnlockObjectList);
        }

        if (inst->msgport = CreateMsgPort())
                return(TRUE);

        // tell the objects that initialized before us to clean up
        // these are the objects of classes that are more close to the root of the tree than we are
        SC_DoClassMethod(cl, NULL, (ULONG) obj, SCCM_Exit);
        return(FALSE);
}

/* 
 * release resources
 */
ULONG WinTask_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct WindowTaskInst *inst)
{
        if (inst->msgport)
                DeleteMsgPort(inst->msgport);
        return(SC_DoSuperMethodA(cl,obj, msg));
}

/*
 * help function
 */
static void WinTask_InternReply(struct SC_Message *smsg)
{
        if (smsg->execmsg.mn_Node.ln_Type == NT_REPLYMSG)
                SC_FreeMsg(smsg);
        else
        {
                if (smsg->execmsg.mn_ReplyPort != NULL)
                        ReplyMsg((struct Message *) smsg);
                else
                        SC_FreeMsg(smsg);
        }
}

/*
 * all idcmp messages for the windows and some internal msgs are catched here in the msg loop
 * in the first case the msg is sent to the appropriate window
 * in the later case some special actions are performed
 */
ULONG WinTask_Input(struct SC_Class *cl, Object *obj, struct SCCP_WindowTask_Input *msg, struct WindowTaskInst *inst)
{
        struct Message *message;
        ULONG msgid;

        *(msg->signal) = (1<<(inst->msgport->mp_SigBit));
        while (message = GetMsg(inst->msgport))
        {
                DEBUG("Message received\n");
                if (msgid = SC_IsScalosMsg(message))
                {
                        DEBUG1("Internal Message, ID: %ld\n", msgid);
                        switch (msgid)
                        {
                                // quit the application
                                case SCMSG_QUIT :
                                        WinTask_InternReply((struct SC_Message *) message);
                                        return(TRUE);
                                default :
                                        WinTask_InternReply((struct SC_Message *) message);
                        }
                }
        }
        return(FALSE);
}

/*
 * quit the complete application
 */
ULONG WinTask_Return(struct SC_Class *cl, Object *obj, Msg msg, struct WindowTaskInst *inst)
{
        struct SC_Message *smsg;

        if (smsg = SC_AllocMsg(SCMSG_QUIT,sizeof(struct SC_Message)))
        {
                PutMsg(inst->msgport, (struct Message *) smsg);
                DEBUG("Sending quit msg\n");
        }

        return TRUE;
}
                

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData WindowTaskMethods[] =
{
        { SCCM_Init,(ULONG) WinTask_Init, 0, 0, NULL },
        { SCCM_Exit,(ULONG) WinTask_Exit, 0, 0, NULL },
        { SCCM_WindowTask_Input,(ULONG) WinTask_Input, sizeof(struct SCCP_WindowTask_Input), 0, NULL },
        { SCCM_WindowTask_Return,(ULONG) WinTask_Return, sizeof(ULONG), 0, NULL },
        { SCMETHOD_DONE, NULL, 0, 0, NULL }
};

