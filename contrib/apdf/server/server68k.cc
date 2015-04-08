//========================================================================
//
// server68k.cc
//
// Copyright 1999-2001 Emmanuel Lesueur
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

#ifdef mc68060

asm("
	.global ___muldi3
___muldi3:
	moveml %d2-%d7,%sp@-
	movel   %sp@(32),%d3 /* d3.l = a | b */
	movel   %sp@(36),%d4
	movel   %sp@(40),%d5 /* d5.l = c | d */
	movel   %d3,%d7     /* d7.l = a | b */
	movel   %d3,%d6     /* d6.w = b */
	swap    %d7         /* d7.w = a */
	muluw   %d5,%d3     /* d3.l = bd */
	movel   %d7,%d0     /* d0.w = a */
	muluw   %d5,%d7     /* d7.l = ad */
	swap    %d5         /* d5.w = c */
	movel   %d7,%d1     /* d1.l = ad */
	swap    %d1         /* d1.l = low(ad) | high(ad) */
	muluw   %d5,%d6     /* d6.l = bc */
	clrw    %d7         /* d7.l = high(ad) | 0 */
	muluw   %d5,%d0     /* d0.l = ac */
	movel   %d6,%d2     /* d2.l = bc */
	swap    %d6         /* d6.l = low(bc) | high(bc) */
	clrw    %d2         /* d2.l = high(bc) | 0 */
	clrw    %d1         /* d1.l = low(ad) | 0 */
	swap    %d7         /* d7.l = 0 | high(ad) */
	clrw    %d6         /* d6.l = low(bc) | 0 */
	swap    %d2         /* d2.l = 0 | high(bc) */
	addl    %d6,%d1     /* d1.l = low(ad+bc) | 0 */
	addxl   %d2,%d7     /* d7.l = high(ad+bc) */
	addl    %d3,%d1     /* d1.l = low(ad+bc) + bd */
	addxl   %d7,%d0     /* d0.l = ac + high(ad+bc) */
	movel   %sp@(32),%d3
	movel   %sp@(28),%d2
	movel   %sp@(40),%d5
	mulsl   %d4,%d3
	mulsl   %d5,%d2
	addl    %d3,%d0
	addl    %d2,%d0
	moveml  %sp@+,%d2-%d7
	rts
");

#endif

static comm_info* m68k_create_comm_info(size_t sz) {
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

static void m68k_delete_comm_info(comm_info* ci) {
    FreeVec(ci->msg);
    free(ci);
}

static int m68k_exchange_msg(struct comm_info *ci,struct msg_base *msg,size_t len,int id) {
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

static void m68k_send_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    MyMessage* m=(MyMessage*)ci->msg;
    DB(printf("SV: send_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    m->data=msg;
    m->len=len;
    m->id=id;
    PutMsg(port,&m->msg);
}

static void *m68k_get_msg(struct server_info *sv,void **data,size_t *len,int *id) {
    MyMessage* msg;
    WaitPort(reply_port);
    msg=(MyMessage*)GetMsg(reply_port);
    *data=msg->data;
    *len=msg->len;
    *id=msg->id;
    DB(printf("SV: get_msg(%lx)=(%lx,%lx,%ld,%ld)\n",sv,msg,*data,*len,*id);)
    return msg;
}

static void m68k_reply_msg(void *msg) {
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
    create_comm_info=m68k_create_comm_info;
    delete_comm_info=m68k_delete_comm_info;
    exchange_msg=m68k_exchange_msg;
    //send_msg=m68k_send_msg;
    get_msg=m68k_get_msg;
    reply_msg=m68k_reply_msg;

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

#ifdef DEBUGMEM
extern "C" void printunfreed();
extern "C" void call_printunfreed() {
    printunfreed();
}
__asm__(".stabs \"___EXIT_LIST__\",22,0,0,_call_printunfreed");
__asm__(".stabs \"___EXIT_LIST__\",20,0,0,108");
#endif

extern "C" void meminit();
extern "C" void memcleanup();
extern "C" void call_meminit() {
    meminit();
}
extern "C" void call_memcleanup() {
    memcleanup();
}

__asm__(".stabs \"___INIT_LIST__\",22,0,0,_call_meminit");
__asm__(".stabs \"___INIT_LIST__\",20,0,0,78");

__asm__(".stabs \"___EXIT_LIST__\",22,0,0,_call_memcleanup");
__asm__(".stabs \"___EXIT_LIST__\",20,0,0,78");
#endif

