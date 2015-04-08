//========================================================================
//
// serverppc.cc
//
// Copyright 1999-2002 Emmanuel Lesueur
//
//========================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define Object ZZObject
#include <exec/nodes.h>
#include <exec/memory.h>
#include <dos/dos.h>
#include <powerup/ppclib/message.h>
#include <powerup/ppclib/tasks.h>
#include <powerup/ppclib/object.h>
#define strlen ZZstrlen
#include <powerup/gcclib/powerup_protos.h>
#undef strlen
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcproto/dos.h>
#undef Object
#undef Allocate
#include "apdf/AComm.h"
//#include "xpdf/rc4.h"
//#include "xpdf/UnLZW.h"

#define DB(x) //x
extern "C" void dprintf(const char *, ...);
#define printf dprintf

static void* port;
static void* reply_port;
//static void* decrypt_lib;
//static void* unlzw_lib;
static BPTR modulesdir;
static Task* main_task;

#if 0
extern "C" {
  void (*do_rc4ExpandKey)( RC4KEY *rc4, unsigned char const *key, int keylen );
  void (*do_rc4Crypt)( RC4KEY *rc4, unsigned char *data, int len );
  void (*MD5_transform)(unsigned char *, unsigned int *);
  void (*MD5_encode)(unsigned char *, unsigned int *, unsigned int);

  const char *(*do_unlzw)(Pipe* in, Pipe* out, const char *cmd);
}
#endif

#ifndef NO_CUSTOM_MEM
void* myAllocMem(size_t sz) {
    DB(printf("SV: myAllocMem(%u)\n", sz));
    void* p = PPCAllocMem(sz,MEMF_ANY);
    DB(printf("SV: return %p\n", p));
    return p;
}

void myFreeMem(void* p,size_t sz) {
    DB(printf("SV: myFreeMem(%p, %u)\n", p, sz));
    PPCFreeMem(p,sz);
    DB(printf("SV: done\n"));
}
#endif

static comm_info* ppc_create_comm_info(size_t sz) {
    DB(printf("SV: ppc_create_comm_info(%u)\n", sz));
    comm_info* ci=(comm_info*)malloc(sizeof(comm_info));
    if(ci) {
	ci->msg=PPCCreateMessage(reply_port,sz);
	ci->cmddata=PPCAllocVec(sz,MEMF_PUBLIC);
	if(!ci->msg || !ci->cmddata) {
	    if(ci->msg)
		PPCDeleteMessage(ci->msg);
	    if(ci->cmddata)
		PPCFreeVec(ci->cmddata);
	    free(ci);
	    ci=NULL;
	}
    }
    DB(printf("SV: return %p\n", ci));
    return ci;
}

static void ppc_delete_comm_info(comm_info* ci) {
    DB(printf("SV: ppc_delete_comm_info(%p)\n", ci));
    if (ci) {
      PPCDeleteMessage(ci->msg);
      PPCFreeVec(ci->cmddata);
      free(ci);
    }
}

static int ppc_exchange_msg(struct comm_info *ci,struct msg_base *msg,size_t len,int id) {
    DB(printf("SV: exchange_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    PPCSendMessage(port,ci->msg,msg,len,id);
    Signal(main_task,SIGBREAKF_CTRL_F);
    PPCWaitPort(reply_port);
    PPCGetMessage(reply_port);
    DB(printf("success=%d\n",msg?msg->success:1);)
    return msg?msg->success:1;
}

/*static void ppc_send_msg(struct comm_info *ci,void *msg,size_t len,int id) {
    DB(printf("SV: send_msg(%lx,%lx,%ld,%ld)\n",ci,msg,len,id);)
    PPCSendMessage(port,ci->msg,msg,len,id);
}*/

static void *ppc_get_msg(struct server_info *sv,void **data,size_t *len,int *id) {
    DB(printf("SV: get_msg(%p,%p,%p,%p)\n", sv, data, len, id));
    void *msg;
    PPCWaitPort(reply_port);
    msg=PPCGetMessage(reply_port);
    *data=(void*)PPCGetMessageAttr(msg,PPCMSGTAG_DATA);
    *len=PPCGetMessageAttr(msg,PPCMSGTAG_DATALENGTH);
    *id=PPCGetMessageAttr(msg,PPCMSGTAG_MSGID);
    DB(printf("SV: get_msg(%lx)=(%lx,%lx,%ld,%ld)\n",sv,msg,*data,*len,*id);)
    return msg;
}

static void ppc_reply_msg(void *msg) {
    DB(printf("SV: reply_msg(%lx)\n",msg);)
    PPCReplyMessage(msg);
}

void signal_msg() {
    DB(printf("SV: signal()\n");)
    Signal(main_task,SIGBREAKF_CTRL_F);
}

#if 0
bool decryptionEnabled() {
    if(!decrypt_lib) {
	struct TagItem tags[]={
	    {PPCELFLOADTAG_LIBVERSION,1},
	    {TAG_END}
	};
	char buf[256];
	NameFromLock(modulesdir,buf,sizeof(buf));
	AddPart(buf,"ApdfDecrypt_ppc.module",sizeof(buf));
	decrypt_lib=PPCOpenLibrary(buf,tags);
	if(decrypt_lib) {
	    do_rc4ExpandKey=(void (*)( RC4KEY *, unsigned char const *, int))
		PPCGetLibSymbol(decrypt_lib,"rc4ExpandKey");
	    do_rc4Crypt=(void (*)( RC4KEY *, unsigned char *, int))
		PPCGetLibSymbol(decrypt_lib,"rc4Crypt");
	    MD5_transform=(void (*)(unsigned char *, unsigned int *))
		PPCGetLibSymbol(decrypt_lib,"MD5_transform");
	    MD5_encode=(void (*)(unsigned char *, unsigned int *, unsigned int))
		PPCGetLibSymbol(decrypt_lib,"MD5_encode");
	}
    }
    return do_rc4ExpandKey && do_rc4Crypt && MD5_transform && MD5_encode;
}

const char* stub_unlzw(Pipe*,Pipe*,const char*) {
    return "No LZW decompression module.";
}

void (**openGfxLib(int bpp, void** base))() {
    char name[32];
    char buf[256];

    sprintf(name, "ApdfGfx%d_" PROC ".module", bpp * 8);

    NameFromLock(modulesdir, buf, sizeof(buf));
    AddPart(buf, name, sizeof(buf));

    static struct TagItem tags[] = {
	{PPCELFLOADTAG_LIBVERSION, 1},
	{TAG_END}
    };

    *base = PPCOpenLibrary(buf, tags);
    if (*base) {
	void (**(*getBase)(void))();
	getBase = (void (**(*)(void))())PPCGetLibSymbol(*base, "getBase");
	if (getBase)
	    return getBase();
	PPCCloseLibrary(*base);
	*base = NULL;
    }

    return NULL;
}

void closeGfxLib(void** base) {
    PPCCloseLibrary(*base);
    *base = NULL;
}
#endif

int main() {

    DB(printf("SV: main\n");)

    create_comm_info = ppc_create_comm_info;
    delete_comm_info = ppc_delete_comm_info;
    exchange_msg = ppc_exchange_msg;
    get_msg = ppc_get_msg;
    reply_msg = ppc_reply_msg;

    StartupData* sd = (StartupData*)PPCGetTaskAttr(PPCTASKTAG_STARTUP_MSGDATA);
    port = sd->port;
    reply_port = (void*)PPCGetTaskAttr(PPCTASKTAG_MSGPORT);
    main_task = (Task*)sd->task;
    modulesdir = sd->dir;

#if 0
    static struct TagItem tags[]={
	{PPCELFLOADTAG_LIBVERSION, 1},
	{TAG_END}
    };
    {
	char buf[256];
	NameFromLock(modulesdir, buf, sizeof(buf));
#if __GNUC_MINOR__>=95
	AddPart(buf, "ApdfUnLZW2_ppc.module", sizeof(buf));
#else
	AddPart(buf, "ApdfUnLZW_ppc.module", sizeof(buf));
#endif
	unlzw_lib = PPCOpenLibrary(buf, tags);
    }

    if (unlzw_lib)
	do_unlzw = (const char* (*)(Pipe*, Pipe*, const char*))PPCGetLibSymbol(unlzw_lib, "unlzw");
    if (!do_unlzw)
	do_unlzw = &stub_unlzw;
#endif

    comm_info ci;
    ci.msg = NULL;
    ci.cmddata = NULL;
    server(&ci);

#if 0
    if (decrypt_lib)
	PPCCloseLibrary(decrypt_lib);

    if (unlzw_lib)
	PPCCloseLibrary(unlzw_lib);
#endif

    return 0;
}

#ifndef NO_CUSTOM_MEM

#if 0

#ifdef DEBUGMEM
__asm__(".section \".fini\",\"a\"");
__asm__(".long printunfreed");
__asm__(".long 108");
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
__asm__(".long -108");
#endif

__asm__(".section \".ctdt\",\"a\"");
__asm__(".long meminit");
__asm__(".long 78");

__asm__(".section \".ctdt\",\"a\"");
__asm__(".long memcleanup");
__asm__(".long -78");

#endif

#endif

