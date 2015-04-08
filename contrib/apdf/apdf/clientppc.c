/*************************************************************************\
 *                                                                       *
 * clientppc.c                                                           *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifdef HAVE_POWERUP
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <exec/memory.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <intuition/classusr.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/memory.h>
#include <powerup/ppclib/tasks.h>
struct ModuleArgs;    /* avoid warnings */
struct PPCObjectInfo; /* avoid warnings */
#undef NO_INLINE_STDARG
#ifdef MORPHOS
#   include <proto/ppc.h>
#else
#   include <powerup/proto/ppc.h>
#endif
#include "AComm.h"
#include "Apdf.h"

#define DB(x) //x

void kprintf(const char*,...);

static struct StartupData *startup_data;
extern BOOL error_flag;

static void* ppc_MyAllocSharedMem(struct server_info *sv,size_t sz) {
    if(sv->pool) {
	/* the memory allocated by PPCAllocVecPooled() isn't
	   correctly aligned for shared use. */
	char *p=PPCAllocVecPooled(sv->pool,sz+=sizeof(char*)+32);
	char **q=(char**)(((ULONG)p+sizeof(char*)+31)&~31);
	q[-1]=p;
	return q;
    } else
	return PPCAllocVec(sz,MEMF_PUBLIC);
}

static void ppc_MyFreeSharedMem(struct server_info *sv,void* p) {
    if(p) {
	if(sv->pool)
	    PPCFreeVecPooled(sv->pool,((char**)p)[-1]);
	else
	    PPCFreeVec(p);
    }
}


static int ppc_exchange_msg(struct comm_info *ci,struct msg_base *msg,size_t len,int id) {
    DB(kprintf("exchange_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    PPCSendMessage(ci->sv->port,ci->msg,msg,len,id);
    PPCWaitPort(ci->sv->reply_port);
    PPCGetMessage(ci->sv->reply_port);
    DB(kprintf("success=%ld\n",msg?msg->success:1);)
    if(msg && msg->error)
	error_flag=TRUE;
    return msg?msg->success:1;
}

static void ppc_exchange_raw_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    DB(kprintf("exchange_raw_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    PPCSendMessage(ci->sv->port,ci->msg,msg,len,id);
    PPCWaitPort(ci->sv->reply_port);
    PPCGetMessage(ci->sv->reply_port);
}

/*static void ppc_init_exchange(struct comm_info *ci) {
    DB(printf("init_exchange(%lx)\n",ci);)
}*/

static void ppc_send_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    DB(kprintf("send_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    PPCSendMessage(ci->sv->port,ci->msg,msg,len,id);
}

static void *ppc_get_msg(struct server_info *sv,void **data,size_t *len,int *id) {
    void *msg;
    msg=PPCGetMessage(sv->reply_port);
    if(msg) {
	*data=(void*)PPCGetMessageAttr(msg,PPCMSGTAG_DATA);
	*len=PPCGetMessageAttr(msg,PPCMSGTAG_DATALENGTH);
	*id=PPCGetMessageAttr(msg,PPCMSGTAG_MSGID);
    }
    DB(kprintf("get_msg()=(%lx,%lx,%ld,%ld)\n",msg,*data,*len,*id);)
    return msg;
}

static void ppc_reply_msg(void *msg) {
    DB(kprintf("reply_msg(%lx)\n",msg);)
    PPCReplyMessage(msg);
}

/*static void ppc_end_exchange(struct comm_info *ci) {
    DB(printf("end_exchange(%lx)\n",ci);)
}*/

static struct comm_info *ppc_create_comm_info(struct server_info *sv,size_t sz) {
    struct comm_info *ci;
    if(ci=MyAllocMem(sizeof(struct comm_info))) {
	ci->sv=sv;
	if(ci->msg=PPCCreateMessage(sv->reply_port,sz)) {
	    ci->cmddata=NULL;
	    if(!sz || (ci->cmddata=(*sv->MyAllocSharedMem)(sv,sz)))
		return ci;
	    PPCDeleteMessage(ci->msg);
	}
	MyFreeMem(ci);
    }
    return NULL;
}

static void ppc_delete_comm_info(struct comm_info* ci) {
    if(ci->cmddata)
	(*ci->sv->MyFreeSharedMem)(ci->sv,ci->cmddata);
    PPCDeleteMessage(ci->msg);
    MyFreeMem(ci);
}


void ppc_delete_server(struct server_info *sv) {
    if(sv->process) {
	void* msg;
	PPCSendMessage(sv->port,sv->extra,NULL,0,MSGID_QUIT);
	PPCWaitPort(sv->reply_port);
	msg=PPCGetMessage(sv->reply_port); /* cmdmsg */
	DB(if(msg!=sv->extra)kprintf("***Wrong message 1\n");)
	PPCWaitPort(sv->reply_port);
	msg=PPCGetMessage(sv->reply_port); /* startupmsg */
	DB(if(msg!=sv->startupmsg)kprintf("***Wrong message 2\n");)
	sv->process=NULL;
    }
    if(sv->extra) {
	PPCDeleteMessage(sv->extra);
	sv->extra=NULL;
    }
    if(sv->startupmsg) {
	PPCDeleteMessage(sv->startupmsg);
	sv->startupmsg=NULL;
    }
    if(startup_data) {
	ppc_MyFreeSharedMem(sv,startup_data);
	startup_data=NULL;
    }
    if(sv->reply_port) {
	PPCDeletePort(sv->reply_port);
	sv->reply_port=NULL;
    }
    if(sv->pool) {
	PPCDeletePool(sv->pool);
	sv->pool=NULL;
    }
    if(sv->abort_flag_ptr) {
	PPCFreeMem((void*)sv->abort_flag_ptr,sizeof(int)*2);
	sv->abort_flag_ptr=NULL;
    }
}

int create_ppc_server(struct server_info *sv,void *elfobject) {

    memset(sv,0,sizeof(*sv));
    sv->MyAllocSharedMem=ppc_MyAllocSharedMem;
    sv->MyFreeSharedMem=ppc_MyFreeSharedMem;
    sv->exchange_msg=ppc_exchange_msg;
    sv->exchange_raw_msg=ppc_exchange_raw_msg;
    //sv->init_exchange=ppc_init_exchange;
    sv->send_msg=ppc_send_msg;
    sv->get_msg=ppc_get_msg;
    sv->reply_msg=ppc_reply_msg;
    //sv->end_exchange=ppc_end_exchange;
    sv->destroy=ppc_delete_server;
    sv->create_comm_info=ppc_create_comm_info;
    sv->delete_comm_info=ppc_delete_comm_info;

    sv->pool=PPCCreatePool(MEMF_PUBLIC,10000,4000);
    /*if(!sv->pool) {  ppclibemu currently fails here.
	DB(printf("Can't create shared pool.\n");)
	return 0;
    }*/

    sv->reply_port=PPCCreatePort(NULL);
    if(!sv->reply_port) {
	DB(kprintf("Can't create port.\n");)
	ppc_delete_server(sv);
	return 0;
    }

    sv->abort_flag_ptr=PPCAllocMem(sizeof(int)*2,
				   MEMF_WRITETHROUGHM68K|
				   MEMF_WRITETHROUGHPPC|
				   MEMF_NOCACHESYNCM68K|
				   MEMF_NOCACHESYNCPPC);
    sv->draw_state_ptr=sv->abort_flag_ptr+1;

    startup_data=ppc_MyAllocSharedMem(sv,sizeof(struct StartupData));
    if(startup_data) {
	struct Process *proc;
	startup_data->port=sv->reply_port;
	startup_data->in=Input();
	startup_data->out=Output();
	/* libnix startup code does not initialize pr_CES */
	/* when started from Workbench. Use Output() instead. */
	proc=(struct Process *)FindTask(NULL);
	startup_data->err=proc->pr_CLI?proc->pr_CES:Output();
	startup_data->status=0;
	startup_data->flags=0;
	/* The ppc process should inherit the current directory, */
	/* but this doesn't work with ppclibemu (yet?) */
	startup_data->dir=CurrentDir(0);
	startup_data->task=proc;
	CurrentDir(startup_data->dir);
	sv->startupmsg=PPCCreateMessage(sv->reply_port,sizeof(struct StartupData));
    }
    sv->extra=PPCCreateMessage(sv->reply_port,0);

    if(!startup_data || !sv->startupmsg || !sv->extra ||!sv->abort_flag_ptr) {
	DB(kprintf("Can't create message.\n");)
	ppc_delete_server(sv);
	return 0;
    }

    sv->process=PPCCreateTaskTags(elfobject,
	PPCTASKTAG_NAME,(ULONG)"PDF decoder",
	PPCTASKTAG_STACKSIZE,80000,
	PPCTASKTAG_STARTUP_MSG,(ULONG)sv->startupmsg,
	PPCTASKTAG_STARTUP_MSGDATA,(ULONG)startup_data,
	PPCTASKTAG_STARTUP_MSGLENGTH,sizeof(struct StartupData),
	PPCTASKTAG_MSGPORT,TRUE,
	NP_Input,startup_data->in,
	NP_CloseInput,FALSE,
	NP_Output,startup_data->out,
	NP_CloseOutput,FALSE,
	NP_Error,startup_data->err,
	NP_CloseError,FALSE,
	TAG_END);
    if(!sv->process) {
	DB(kprintf("Can't create PPC task.\n");)
	ppc_delete_server(sv);
	return 0;
    }
    sv->port=(void*)PPCGetTaskAttrsTags(sv->process,PPCTASKINFOTAG_MSGPORT,NULL,TAG_END);
    sv->sigbit=SIGBREAKB_CTRL_F;
    return 1;
}

#endif

