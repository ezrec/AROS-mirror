/*************************************************************************\
 *                                                                       *
 * comm.c                                                                *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <dos/dos.h>
#include <exec/lists.h>
#include <libraries/mui.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include "comm.h"
#include "AComm.h"

#define DB(x)   //x
#define Static  static

struct CommData {
    struct MUI_InputHandlerNode ihn;
};
typedef struct CommData CommData;


struct async_msg* sent_msg;
struct MinList async_msg_list={
    (struct MinNode *)&async_msg_list.mlh_Tail,
    NULL,
    (struct MinNode *)&async_msg_list
};

static Object* app;
static BOOL pushed;
BOOL error_flag;

BOOL is_sync(void) {
    return sent_msg==NULL;
}

BOOL has_error(void) {
    if(sent_msg==NULL) {
	BOOL f=error_flag;
	error_flag=FALSE;
	return f;
    }
    return FALSE;
}

void abort_msg(void) {
    *sv.abort_flag_ptr=1;
}

void handle_async_msgs1(void) {
    void *msg;
    void *data;
    size_t sz;
    int id;
    while(msg=(*sv.get_msg)(&sv,&data,&sz,&id)) {
	struct async_msg *n=sent_msg;
	DB(if(n->id!=100)kprintf("msg %lx (%ld) replied\n",n,n->id);)
	sent_msg=NULL;
	n->replied=TRUE;
	n->msg=msg;
	n->data=data;
	n->sz=sz;
	n->id=id;
	if(!DoMethod2(n->obj,n->reply_methodid)) {
	    DB(kprintf("aborted\n");)
	    AddHead((struct List *)&async_msg_list,(struct Node*)n);
	    n->replied=FALSE;
	}
    }
}

#if 0
void handle_async_msgs(void) {
    handle_async_msgs1();
    /*if(stopped) {
	Signal(FindTask(NULL),SIGBREAKF_CTRL_D);
	stopped=FALSE;
    }*/
}
#endif

void sync(void) {
    //kprintf("sync\n");
    if(sent_msg) {
	handle_async_msgs1();
	while(sent_msg) {
	    DB(kprintf("sync()\n");)
	    *sv.abort_flag_ptr=1;
	    Wait((1<<sv.sigbit)|SIGBREAKF_CTRL_F);
	    handle_async_msgs1();
	}
	*sv.abort_flag_ptr=0;
    }
    if(!pushed && async_msg_list.mlh_TailPred!=(struct MinNode*)&async_msg_list) {
	pushed=TRUE;
	DB(kprintf("push StartNext\n");)
	DoMethod5(app,MUIM_Application_PushMethod,comm,1,MYM_Comm_StartNext);
    }
}

void send_async_msg(struct async_msg *msg) {
    DB(kprintf("send_async_msg(%lx)\n",msg);)
    msg->replied=FALSE;
    msg->msg=msg->ci->msg;
    AddTail((struct List *)&async_msg_list,(struct Node*)msg);
    if(!pushed) {
	DB(kprintf("push StartNext\n");)
	pushed=TRUE;
	DoMethod5(app,MUIM_Application_PushMethod,comm,1,MYM_Comm_StartNext);
    }
}

void reply_async_msg(struct async_msg *amsg,void* msg) {
    //DB(kprintf("reply_async_msg(%lx,%lx)\n",amsg,msg);)
    amsg->replied=FALSE;
    sent_msg=amsg;
    amsg->msg=msg;
    (*sv.reply_msg)(msg);
}

void abort_async_msg(struct async_msg *amsg) {
    DB(if(!amsg->replied)kprintf("abort_async_msg(%lx)\n",amsg);)
    if(amsg==sent_msg)
	sync();
    if(!amsg->replied) {
	DB(kprintf("removed\n");)
	amsg->replied=TRUE;
	Remove((struct Node *)amsg);
    }
}

Static ULONG commNew(struct IClass *cl,Object *obj,struct opSet *msg) {

    if(obj=(Object*)DoSuperMethodA(cl,obj,(Msg)msg)) {
	CommData *dat=INST_DATA(cl,obj);
	dat->ihn.ihn_Object=obj;
	dat->ihn.ihn_Signals=1<<sv.sigbit;
	dat->ihn.ihn_Flags=0;
	dat->ihn.ihn_Method=MYM_Comm_Handle;
	DoMethod3(app,MUIM_Application_AddInputHandler,&dat->ihn);
    }

    return (ULONG)obj;
}

Static ULONG commDispose(struct IClass *cl,Object *obj,Msg msg) {
    CommData *dat=INST_DATA(cl,obj);
    DoMethod3(app,MUIM_Application_RemInputHandler,&dat->ihn);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG commHandle(struct IClass *cl,Object *obj) {
    //DB(kprintf("Handle\n");)
    handle_async_msgs1();
    if(!sent_msg && !pushed &&
       async_msg_list.mlh_TailPred!=(struct MinNode*)&async_msg_list) {
	DB(kprintf("push StartNext\n");)
	pushed=TRUE;
	DoMethod5(app,MUIM_Application_PushMethod,comm,1,MYM_Comm_StartNext);
    }
    return 0;
}

Static ULONG commStartNext(struct IClass *cl,Object *obj) {
    BOOL retry;
    DB(kprintf("StartNext\n");)
    pushed=FALSE;
    do {
	retry=FALSE;
	if(!sent_msg &&
	   async_msg_list.mlh_TailPred!=(struct MinNode*)&async_msg_list) {
	    sent_msg=(struct async_msg *)async_msg_list.mlh_Head;
	    DB(kprintf("sending %lx (%ld)\n",sent_msg,sent_msg->id);)
	    Remove((struct Node*)sent_msg);
	    *sv.abort_flag_ptr=0;
	    if(!sent_msg->init_methodid ||
	       DoMethod2(sent_msg->obj,sent_msg->init_methodid))
		(*sv.send_msg)(sent_msg->ci,sent_msg->data,sent_msg->sz,sent_msg->id);
	    else {
		sent_msg->replied=TRUE;
		sent_msg=NULL;
		retry=TRUE;
	    }
	}
    } while(retry);
    return 0;
}

BEGIN_DISPATCHER(commDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return commNew      (cl,obj,(APTR)msg);
      case OM_DISPOSE:                   return commDispose  (cl,obj,msg);
      /*case OM_SET:                       return prfSet      (cl,obj,(APTR)msg);
      case OM_GET:                       return prfGet      (cl,obj,(APTR)msg);*/
      case MYM_Comm_Handle:              return commHandle   (cl,obj);
      case MYM_Comm_StartNext:           return commStartNext(cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(commDispatcher)

static struct MUI_CustomClass *comm_mcc;
static int count;
Object *comm;

Object *initComm(Object *app1) {
    app=app1;
    ++count;
    if(!comm &&
       (comm_mcc=MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(CommData),(APTR)&commDispatcher))) {
	comm=NewObject(comm_mcc->mcc_Class,NULL,TAG_END);
	if(!comm) {
	    MUI_DeleteCustomClass(comm_mcc);
	    comm_mcc=NULL;
	}
    }
    return comm;
}

void cleanupComm(void) {
    if(count && !--count) {
	MUI_DisposeObject(comm);
	MUI_DeleteCustomClass(comm_mcc);
	comm=NULL;
	comm_mcc=NULL;
    }
}

