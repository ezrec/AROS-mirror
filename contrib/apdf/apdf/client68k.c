/*************************************************************************\
 *                                                                       *
 * client68k.c                                                           *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include "AComm.h"

#define DB(x) //x

extern BOOL error_flag;

struct MyMessage {
    struct Message msg;
    void *data;
    size_t len;
    int id;
};

static void* m68k_MyAllocSharedMem(struct server_info *sv,size_t sz) {
    char *p=AllocPooled(sv->pool,sz+=sizeof(size_t));
    if(p) {
	*(size_t*)p=sz;
	p+=sizeof(size_t);
    }
    return p;
}

static void m68k_MyFreeSharedMem(struct server_info *sv,void* p) {
    if(p) {
	p=(size_t*)p-1;
	FreePooled(sv->pool,p,*(size_t*)p);
    }
}


static int m68k_exchange_msg(struct comm_info *ci,struct msg_base *msg,size_t len,int id) {
    struct MyMessage *m=ci->msg;
    DB(printf("exchange_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(ci->sv->port,&m->msg);
    WaitPort(ci->sv->reply_port);
    GetMsg(ci->sv->reply_port);
    DB(printf("success=%d\n",msg?msg->success:1);)
    if(msg && msg->error)
	error_flag=TRUE;
    return msg?msg->success:1;
}

static void m68k_exchange_raw_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    struct MyMessage *m=ci->msg;
    DB(printf("exchange_raw_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(ci->sv->port,&m->msg);
    WaitPort(ci->sv->reply_port);
    GetMsg(ci->sv->reply_port);
}

/*static void m68k_init_exchange(struct comm_info *ci) {
    DB(printf("init_exchange(%lx)\n",ci);)
}*/

static void m68k_send_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    struct MyMessage *m=ci->msg;
    DB(printf("send_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(ci->sv->port,&m->msg);
}

static void *m68k_get_msg(struct server_info *sv,void **data,size_t *len,int *id) {
    struct MyMessage *msg;
    msg=(struct MyMessage *)GetMsg(sv->reply_port);
    if(msg) {
	*data=msg->data;
	*len=msg->len;
	*id=msg->id;
    }
    DB(printf("get_msg()=(%lx,%lx,%ld,%ld)\n",msg,*data,*len,*id);)
    return msg;
}

static void m68k_reply_msg(void *msg) {
    DB(printf("reply_msg(%lx)\n",msg);)
    ReplyMsg(msg);
}

/*static void m68k_end_exchange(struct comm_info *ci) {
    DB(printf("end_exchange(%lx)\n",ci);)
}*/

static struct comm_info *m68k_create_comm_info(struct server_info *sv,size_t sz) {
    struct comm_info *ci;
    if(ci=(*sv->MyAllocSharedMem)(sv,sizeof(struct comm_info *)+sizeof(struct MyMessage)+sz)) {
	ci->sv=sv;
	ci->msg=ci+1;
	ci->cmddata=(struct MyMessage*)ci->msg+1;
	memset(ci->msg,0,sizeof(struct MyMessage));
	((struct MyMessage*)ci->msg)->msg.mn_ReplyPort=(struct MsgPort*)sv->reply_port;
	((struct MyMessage*)ci->msg)->msg.mn_Length=sz;
    }
    return ci;
}

static void m68k_delete_comm_info(struct comm_info* ci) {
    m68k_MyFreeSharedMem(ci->sv,ci);
}

void m68k_delete_server(struct server_info *sv) {
    struct comm_info *ci=sv->extra;
    if(sv->process) {
	struct Message *msg;
	((struct MyMessage *)ci->msg)->data=NULL;
	((struct MyMessage *)ci->msg)->len=0;
	((struct MyMessage *)ci->msg)->id=MSGID_QUIT;
	PutMsg(sv->port,ci->msg);
	WaitPort(sv->reply_port);
	msg=GetMsg(sv->reply_port); /* cmdmsg */
	DB(if(msg!=ci->msg)printf("***Wrong message 1\n");)
	WaitPort(sv->reply_port);
	msg=GetMsg(sv->reply_port); /* startupmsg */
	DB(if(msg!=sv->startupmsg)printf("***Wrong message 2\n");)
	sv->process=NULL;
    }
    m68k_delete_comm_info(ci);
    sv->extra=NULL;
    DeleteMsgPort(sv->reply_port);
    sv->reply_port=NULL;
    if(sv->pool) {
	DeletePool(sv->pool);
	sv->pool=NULL;
    }
}

static SAVEDS void launcher(void) {
    struct Process *proc=(struct Process *)FindTask(NULL);
    struct MyMessage *msg;
    BPTR seglist;
    BPTR oldinput,input;

    WaitPort(&proc->pr_MsgPort);
    msg=(struct MyMessage *)GetMsg(&proc->pr_MsgPort);
    seglist=(BPTR)msg->data;
    msg->data=NULL;
    proc->pr_Task.tc_UserData=msg;

    input=Open("NIL:",MODE_OLDFILE);
    if(input) {
	oldinput=SelectInput(input);
	RunCommand(seglist,50000,"\n",1);
	SelectInput(oldinput);
	Close(input);
    }

    Forbid();
    ReplyMsg(&msg->msg);
}

static volatile int abort_flag;
static volatile int draw_state;

int create_m68k_server(struct server_info *sv,BPTR seglist) {
    struct Process *proc,*subproc;
    memset(sv,0,sizeof(*sv));
    sv->MyAllocSharedMem=m68k_MyAllocSharedMem;
    sv->MyFreeSharedMem=m68k_MyFreeSharedMem;
    sv->exchange_msg=m68k_exchange_msg;
    sv->exchange_raw_msg=m68k_exchange_raw_msg;
    //sv->init_exchange=m68k_init_exchange;
    sv->send_msg=m68k_send_msg;
    sv->get_msg=m68k_get_msg;
    sv->reply_msg=m68k_reply_msg;
    //sv->end_exchange=m68k_end_exchange;
    sv->destroy=m68k_delete_server;
    sv->create_comm_info=m68k_create_comm_info;
    sv->delete_comm_info=m68k_delete_comm_info;

    sv->pool=CreatePool(MEMF_PUBLIC,10000,4000);
    if(!sv->pool) {
	DB(printf("Can't create shared pool.\n");)
	return 0;
    }

    sv->reply_port=CreateMsgPort();
    if(!sv->reply_port) {
	DB(printf("Can't create port.\n");)
	m68k_delete_server(sv);
	return 0;
    }

    proc=(struct Process *)FindTask(NULL);

    /*if(ci.msg=MyAllocSharedMem(sizeof(struct MyMessage))) {
	memset(ci.msg,0,sizeof(struct MyMessage));
	((struct MyMessage *)ci.msg)->msg.mn_Length=sizeof(*ci.msg);
	((struct MyMessage *)ci.msg)->msg.mn_ReplyPort=ci.reply_port;
    }*/
    if(sv->startupmsg=m68k_MyAllocSharedMem(sv,sizeof(struct MyMessage))) {
	memset(sv->startupmsg,0,sizeof(struct MyMessage));
	((struct MyMessage *)sv->startupmsg)->msg.mn_Length=sizeof(struct MyMessage);
	((struct MyMessage *)sv->startupmsg)->msg.mn_ReplyPort=sv->reply_port;
	((struct MyMessage *)sv->startupmsg)->data=(void*)seglist;
    }

    sv->extra=m68k_create_comm_info(sv,0);
    //ci.cmddata=MyAllocSharedMem(sizeof(union msg_max));

    if(!sv->startupmsg || !sv->extra/*!ci.msg || !ci.cmddata*/) {
	DB(printf("Can't create message.\n");)
	m68k_delete_server(sv);
	return 0;
    }

    sv->process=subproc=CreateNewProcTags(
	NP_Entry,(ULONG)launcher,
	NP_Name,(ULONG)"PDF decoder",
	NP_Priority,-1,
	NP_Cli,TRUE,
	NP_Input,Input(),
	NP_CloseInput,FALSE,
	NP_Output,Output(),
	NP_CloseOutput,FALSE,
	/* libnix startup code does not initialize pr_CES */
	/* when started from Workbench. Use Output() instead. */
	NP_Error,proc->pr_CLI?proc->pr_CES:Output(),
	NP_CloseError,FALSE,
	TAG_END);
    if(!subproc) {
	DB(printf("Can't create server task.\n");)
	m68k_delete_server(sv);
	return 0;
    }
    DB(printf("client: replyport=%lx, startupmsg=%lx\n",sv->reply_port,sv->startupmsg);)
    PutMsg(&subproc->pr_MsgPort,sv->startupmsg);
    WaitPort(sv->reply_port);
    GetMsg(sv->reply_port);
    sv->port=((struct MyMessage *)sv->startupmsg)->data;
    DB(printf("client: got reply, server port=%lx\n",sv->port);)
    if(!sv->port) {
	sv->process=NULL;
	DB(printf("Can't create server port.\n");)
	m68k_delete_server(sv);
	return 0;
    }
    DB(printf("client: resending startupmsg\n");)
    PutMsg(sv->port,sv->startupmsg);
    sv->sigbit=((struct MsgPort*)sv->reply_port)->mp_SigBit;
    sv->abort_flag_ptr=&abort_flag;
    sv->draw_state_ptr=&draw_state;
    return 1;
}

