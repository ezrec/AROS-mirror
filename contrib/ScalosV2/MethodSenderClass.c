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
#include <clib/powerpc_protos.h>
#include <clib/utility_protos.h>
#include <intuition/classusr.h>
#include <exec/ports.h>

#include "Scalos.h"
#include "ScalosIntern.h"
#include "MethodSenderClass.h"
#include "SubRoutines.h"
#include "Scalos_protos.h"
#include "CompilerSupport.h"
#include "ThreadRootClass.h"

/****** MethodSender.scalos/--background ************************************
*
*   MethodSender class is used to send methods to objects that run on a
*   different thread.
*
*****************************************************************************
*/

ULONG PutMethod(struct MethodSenderInst *inst, Msg msg, struct MsgPort *replyport)
{
        if (inst->destobject)
        {
                struct SCMSGP_Method *smsg;
                struct SC_MethodData *mdata;
                struct SC_Class *curcl = SCOCLASS(inst->destobject);
                struct SC_MethodData *usemdata = NULL;

                do
                {
                        if (mdata = (struct SC_MethodData *) curcl->Methods)
                        {
                                while (mdata->MethodID != SCMETHOD_DONE)
                                {
                                        if ((mdata->MethodID == msg->MethodID) && mdata->Size)
                                        {
                                                usemdata = mdata;
                                                break;
                                        }
                                        mdata++;
                                }
                        }
                }
                while ( inst->destobject && !(usemdata) && (curcl = curcl->Super));

                if (usemdata && !(usemdata->Flags & SCMDF_DIRECTMETHOD))
                {
                        if (smsg = (struct SCMSGP_Method *) SC_AllocMsg(SCMSG_METHOD,sizeof(struct SCMSGP_Method) + usemdata->Size))
                        {
                                smsg->scmsg.execmsg.mn_ReplyPort = replyport;
                                smsg->methoddata = usemdata;
                                CopyMem(msg,&smsg->methodarg1, usemdata->Size);
                                PutMsg(myThreadRootInst(inst->destobject)->msgport, (struct Message *) smsg);
                                return(TRUE);
                        }
                }
        }
        return(FALSE);
}

static ULONG SendMethod(struct MethodSenderInst *inst, Msg msg)
{
        ULONG ret = 0;

        if (inst->destobject)
        {
                struct SCMSGP_Method *smsg;
                struct SC_MethodData *mdata;
                struct SC_Class *curcl = SCOCLASS(inst->destobject);
                struct SC_MethodData *usemdata = NULL;
                struct MsgPort *replyport;

                do
                {
                        if (mdata = (struct SC_MethodData *) curcl->Methods)
                        {
                                while (mdata->MethodID != SCMETHOD_DONE)
                                {
                                        if ((mdata->MethodID == msg->MethodID) && mdata->Size)
                                        {
                                                usemdata = mdata;
                                                break;
                                        }
                                        mdata++;
                                }
                        }
                }
                while (!(usemdata) && (curcl = curcl->Super));

                if ((usemdata) && (usemdata->Flags & SCMDF_DIRECTMETHOD))
                        return(SC_DoMethodA(inst->destobject,msg));

                if (usemdata && (replyport = CreateMsgPort()))
                {
                        if (smsg = (struct SCMSGP_Method *) SC_AllocMsg(SCMSG_METHOD,sizeof(struct SCMSGP_Method) + usemdata->Size))
                        {
                                smsg->scmsg.execmsg.mn_ReplyPort = replyport;
                                smsg->methoddata = usemdata;
                                CopyMem(msg,&smsg->methodarg1, usemdata->Size);
                                PutMsg(myThreadRootInst(inst->destobject)->msgport, (struct Message *) smsg);
                                while (!GetMsg(replyport))
                                        WaitPort(replyport);
                                ret = smsg->scmsg.returnvalue;
                                SC_FreeMsg((struct SC_Message *) smsg);
                        }
                        DeleteMsgPort(replyport);
                }
        }
        return(ret);
}


SAVEDS ASM ULONG MethodSenderDispatcher( REG(a0) struct SC_Class *cl, REG(a1) Msg msg, REG(a2) Object *obj )
{
        struct MethodSenderInst *inst = SCINST_DATA(cl,obj);

        switch (msg->MethodID)
        {
                case OM_NEW :
                        if (obj = (Object *) SC_DoSuperMethodA(cl,obj, msg))
                        {
                                inst = SCINST_DATA(cl,obj);
                                inst->senderid = SCALOS_SENDERID;
                                inst->destobject = (Object *) GetTagData(SCCA_MethodSender_DestObject,0,((struct opSet *) msg)->ops_AttrList);
                                return((ULONG) obj);
                        }
                        return(NULL);

                case OM_DISPOSE :
                        SendMethod(inst,msg);
                case SCCM_Init :
                case SCCM_Exit :
                        return(SC_DoSuperMethodA(cl, obj, msg));

                case OM_GET :
                        if (((struct opGet *) msg)->opg_AttrID == SCCA_MethodSender_DestObject)
                        {
                                *(((struct opGet *) msg)->opg_Storage) = (ULONG) inst->destobject;
                                return(TRUE);
                        }
                        else
                                return(SendMethod(inst,msg));

                case OM_SET :
                        inst->destobject = (Object *) GetTagData(SCCA_MethodSender_DestObject,(ULONG) inst->destobject,((struct opSet *) msg)->ops_AttrList);
                default :
                        return(SendMethod(inst,msg));
        }
}

