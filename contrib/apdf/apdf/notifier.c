/*************************************************************************\
 *                                                                       *
 * notifier.c                                                            *
 *                                                                       *
 * Copyright 2001 Emmanuel Lesueur                                       *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <dos/dos.h>
#include <dos/notify.h>
#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include "notifier.h"

#define DB(x)   //x
#define Static  static


struct NotifierData {
    struct NotifyRequest nr;
};
typedef struct NotifierData NotifierData;

struct NotifierBaseData {
    struct MUI_InputHandlerNode ihn;
};
typedef struct NotifierBaseData NotifierBaseData;

static struct MsgPort *port;
static Object *app;

Static ULONG ntfNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    STRPTR name=(STRPTR)GetTagData(MYATTR_Notifier_Name,NULL,msg->ops_AttrList);
    if(name) {
	if(obj=(Object*)DoSuperMethodA(cl,obj,(Msg)msg)) {
	    NotifierData *dat=INST_DATA(cl,obj);
	    dat->nr.nr_Name=(STRPTR)GetTagData(MYATTR_Notifier_Name,NULL,msg->ops_AttrList);
	    dat->nr.nr_Flags=NRF_SEND_MESSAGE;
	    dat->nr.nr_UserData=(ULONG)obj;
	    dat->nr.nr_stuff.nr_Msg.nr_Port=port;
	    StartNotify(&dat->nr);
	}
    } else
	obj=NULL;
    return (ULONG)obj;
}

Static ULONG ntfDispose(struct IClass *cl,Object *obj,Msg msg) {
    NotifierData *dat=INST_DATA(cl,obj);
    EndNotify(&dat->nr);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG ntfGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    NotifierData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_Notifier_Changed:
	*msg->opg_Storage=FALSE;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(APTR)msg);
}

BEGIN_DISPATCHER(ntfDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return ntfNew      (cl,obj,(APTR)msg);
      case OM_DISPOSE:                   return ntfDispose  (cl,obj,msg);
      //case OM_SET:                       return ntfSet      (cl,obj,(APTR)msg);
      case OM_GET:                       return ntfGet      (cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(ntfDispatcher)


Static ULONG ntbNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    /*struct MsgPort **/port=CreateMsgPort();
    if(port && (obj=(Object*)DoSuperMethodA(cl,obj,(Msg)msg))) {
	NotifierBaseData *dat=INST_DATA(cl,obj);
	//dat->port=port;
	dat->ihn.ihn_Object=obj;
	dat->ihn.ihn_Signals=1<<port->mp_SigBit;
	dat->ihn.ihn_Flags=0;
	dat->ihn.ihn_Method=MYM_NotifierBase_Handle;
	DoMethod3(app,MUIM_Application_AddInputHandler,&dat->ihn);
    }
    else
	obj=NULL;
    return (ULONG)obj;
}

Static ULONG ntbDispose(struct IClass *cl,Object *obj,Msg msg) {
    NotifierBaseData *dat=INST_DATA(cl,obj);
    DoMethod3(app,MUIM_Application_RemInputHandler,&dat->ihn);
    DeleteMsgPort(/*dat->*/port);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG ntbHandle(struct IClass *cl,Object *obj) {
    struct NotifyMessage *nm;
    while(nm=(APTR)GetMsg(port)) {
	Object *o=(APTR)nm->nm_NReq->nr_UserData;
	ReplyMsg(&nm->nm_ExecMessage);
	set(o,MYATTR_Notifier_Changed,TRUE);
    }
}

BEGIN_DISPATCHER(ntbDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return ntbNew      (cl,obj,(APTR)msg);
      case OM_DISPOSE:                   return ntbDispose  (cl,obj,msg);
      case MYM_NotifierBase_Handle:      return ntbHandle   (cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(ntbDispatcher)


struct MUI_CustomClass *notifier_mcc;
static struct MUI_CustomClass *notifierbase_mcc;
static int count;
static Object *notifier_base;

BOOL initNotifier(Object *app1) {
    app=app1;
    if(count++==0) {
	notifierbase_mcc=MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(NotifierBaseData),(APTR)&ntbDispatcher);
	if(notifierbase_mcc) {
	    notifier_base=NewObject(notifierbase_mcc->mcc_Class,NULL,TAG_END);
	    if(notifier_base) {
		notifier_mcc=MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(NotifierData),(APTR)&ntfDispatcher);
		if (notifier_mcc) {
		    return TRUE;
		}
		DisposeObject(notifier_base);
	    }
	    MUI_DeleteCustomClass(notifierbase_mcc);
	}
	notifier_base=NULL;
	count=0;
    }
    return FALSE;
}

void cleanupNotifier(void) {
    if(count && !--count) {
	MUI_DeleteCustomClass(notifier_mcc);
	DisposeObject(notifier_base);
	MUI_DeleteCustomClass(notifierbase_mcc);
    }
}

