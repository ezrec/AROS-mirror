//========================================================================
//
// servermos.cc
//
// Copyright 1999-2005 Emmanuel Lesueur
//
//========================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define Object ZZObject
#include <exec/nodes.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <proto/exec.h>
#include <proto/dos.h>
#undef Object
#undef Allocate
#include "apdf/AComm.h"

#define DB(x) //x

#ifndef NO_CUSTOM_MEM
void* myAllocMem(size_t sz) {
    return AllocMem(sz,MEMF_ANY);
}

void myFreeMem(void* p,size_t sz) {
    FreeMem(p,sz);
}
#endif

struct MyMessage {
    struct Message msg;
    void *data;
    size_t len;
    int id;
};


static MsgPort *port;
static MsgPort *reply_port;
//void* decryptBase;
//void* unlzwBase;
static BPTR modulesdir;

static comm_info* mos_create_comm_info(size_t sz) {
    comm_info* ci=(comm_info*)malloc(sizeof(comm_info));
    if(ci) {
	ci->msg=AllocVec(sizeof(MyMessage)+sz,MEMF_PUBLIC|MEMF_CLEAR);
	if(!ci->msg) {
	    free(ci);
	    ci=NULL;
	} else {
	    ci->cmddata=(MyMessage*)ci->msg+1;
	    ((MyMessage*)ci->msg)->msg.mn_ReplyPort=reply_port;
	    ((MyMessage*)ci->msg)->msg.mn_Length=sz;
	}
    }
    return ci;
}

static void mos_delete_comm_info(comm_info* ci) {
    FreeVec(ci->msg);
    free(ci);
}

static int mos_exchange_msg(struct comm_info *ci,struct msg_base *msg,size_t len,int id) {
    DB(printf("SV: exchange_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    MyMessage* m=(MyMessage*)ci->msg;
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(port,&m->msg);
    WaitPort(reply_port);
    GetMsg(reply_port);
    DB(printf("success=%d\n",msg?msg->success:1);)
    return msg?msg->success:1;
}

static void mos_send_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    MyMessage* m=(MyMessage*)ci->msg;
    DB(printf("SV: send_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(port,&m->msg);
}

static void *mos_get_msg(struct server_info *sv,void **data,size_t *len,int *id) {
    MyMessage* msg;
    WaitPort(reply_port);
    msg=(MyMessage*)GetMsg(reply_port);
    *data=msg->data;
    *len=msg->len;
    *id=msg->id;
    DB(printf("SV: get_msg(%lx)=(%lx,%lx,%ld,%ld)\n",sv,msg,*data,*len,*id);)
    return msg;
}

static void mos_reply_msg(void *msg) {
    DB(printf("SV: reply_msg(%lx)\n",msg);)
    ReplyMsg((Message*)msg);
}

void signal_msg() {}

void cleanup() {
    DeleteMsgPort(reply_port);
}

#if 0
bool decryptionEnabled() {
    if(!decryptBase) {
	BPTR olddir=CurrentDir(modulesdir);
	decryptBase=OpenLibrary((UBYTE*)"ApdfDecrypt_68k.module",1);
	CurrentDir(olddir);
    }
    return decryptBase!=NULL;
}


void (**openGfxLib(int bpp, void** base))() {
    char name[32];
    BPTR olddir=CurrentDir(modulesdir);
    sprintf(name, "ApdfGfx%d_" PROC ".module", bpp * 8);
    *base = OpenLibrary((UBYTE*)name, 0);
    CurrentDir(olddir);

    if (*base) {
	void (**table)() =
	({
	    register void* const b __asm("a6") = *base;
	    register void (**r)() __asm("d0");
	    __asm volatile ("jsr a6@(-30:W)"
			    : "=r" (r)
			    : "r" (b)
			    : "d0", "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
	    r;
	 });
	if (table)
	    return table;
	CloseLibrary((Library*)*base);
	*base = NULL;
    }

    return NULL;
}

void closeGfxLib(void** base) {
    CloseLibrary((Library*)*base);
    *base = NULL;
}
#endif

int main() {
    create_comm_info=mos_create_comm_info;
    delete_comm_info=mos_delete_comm_info;
    exchange_msg=mos_exchange_msg;
    //send_msg=mos_send_msg;
    get_msg=mos_get_msg;
    reply_msg=mos_reply_msg;

    Task* task=FindTask(NULL);
    MyMessage* startupmsg=(MyMessage*)task->tc_UserData;

    modulesdir=CurrentDir(0);
    CurrentDir(modulesdir);
    port=startupmsg->msg.mn_ReplyPort;
    reply_port=CreateMsgPort();
    DB(printf("server %lx: client_port=%lx reply_port=%lx\n",task,port,reply_port);)

#if 0
#if __GNUC_MINOR__>=95
    unlzwBase=OpenLibrary((UBYTE*)"ApdfUnLZW2_68k.module",1);
#else
    unlzwBase=OpenLibrary((UBYTE*)"ApdfUnLZW_68k.module",1);
#endif
#endif
    if(!reply_port)
	return 0;

    atexit(cleanup);

    startupmsg->data=reply_port;
    ReplyMsg(&startupmsg->msg);
    WaitPort(reply_port);
    GetMsg(reply_port);
    DB(printf("server: got startupmsg\n");)

    comm_info ci;
    ci.msg=NULL;
    ci.cmddata=NULL;

    server(&ci);

    //CloseLibrary((Library*)decryptBase);
    //CloseLibrary((Library*)unlzwBase);

    return 0;
}

#ifndef NO_CUSTOM_MEM

#if 0

#ifdef DEBUGMEM
__asm__(".section \".fini\",\"a\"");
__asm__(".long printunfreed");
__asm__(".long 109");
#endif

__asm__(".section \".init\",\"a\"");
__asm__(".long meminit");
__asm__(".long 78");

__asm__(".section \".fini\",\"a\"");
__asm__(".long memcleanup");
__asm__(".long 78");

#else

#ifdef DEBUGMEM
__asm__(".section \".ctdt\",\"a\"");
__asm__(".long printunfreed");
__asm__(".long -119");
#endif

__asm__(".section \".ctdt\",\"a\"");
__asm__(".long meminit");
__asm__(".long 78");

__asm__(".section \".ctdt\",\"a\"");
__asm__(".long memcleanup");
__asm__(".long -78");

#endif

#endif

