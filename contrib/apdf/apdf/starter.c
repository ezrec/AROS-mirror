/*************************************************************************\
 *                                                                       *
 * starter.c                                                             *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#define __NOLIBBASE
#include "config.h"
#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

asm("
	.text
	bra _start
");

int call_68k(REG(a0, char *line), REG(d0, int len), REG(a1, void *start));
asm("
	.text
_call_68k:
	moveml	d2-d7/a2-a6,sp@-
	jbsr	 (a1)
	moveml	sp@+,d2-d7/a2-a6
	rts
");

/*int call_ppc(REG(a0, char *line), REG(d0, int len), REG(a1, void *start));
asm("
	.text
_call_ppc:
	.short	0xff00		| TRAP_LIB
	.short  0
	.long	trampoline
trampoline:
	.long	0x80020024	| lwz 0,36(2)
	.long	0x80620020	| lwz 3,32(2)
	.long	0x7c0903a6	| mtctr 0
	.long	0x80820000	| lwz 4,0(2)
	.long	0x4e800420	| bctr
");*/


int start(REG(a0, char *line), REG(d0, int len)) {
    struct Library *SysBase = *(struct Library **)4;
    struct Library *DOSBase;
    struct Library *IntuitionBase;
    struct Process *proc = (struct Process *)FindTask(NULL);
    struct Message *wbmsg;
    int ret = RETURN_FAIL;
    BPTR seg = 0;
    /*BOOL morphos = FALSE;*/

    if (proc->pr_CLI)
	wbmsg = NULL;
    else {
	WaitPort(&proc->pr_MsgPort);
	wbmsg = GetMsg(&proc->pr_MsgPort);
    }

    if (DOSBase = OpenLibrary("dos.library", 39)) {

	if (FindResident("MorphOS")) {
	    seg = LoadSeg("PROGDIR:Apdf.morphos");
	    /*morphos = TRUE;
	    seg = *(BPTR *)BADDR(seg);*/
	}

	if (!seg)
	    seg = LoadSeg("PROGDIR:Apdf.amigaos");

	if (seg) {
	    int (*start)(REG(a0, char *line), REG(d0, int len));

	    start = (void *)((BPTR *)BADDR(seg) + 1);

	    if (wbmsg) {
		PutMsg(&proc->pr_MsgPort, wbmsg);
		wbmsg = NULL;
	    }

	    /*if (morphos)
		ret = call_ppc(line, len, start);
	    else*/
		ret = call_68k(line, len, start);

	    UnLoadSeg(seg);
	}

	if (!seg) {
	    if (wbmsg) {
		
		if (IntuitionBase = (APTR)OpenLibrary("intuition.library", 39)) {
		    static struct EasyStruct params = {
			sizeof(struct EasyStruct),
			0,
			"Apdf Error",
			"Can't load \"PROGDIR:Apdf.amigaos\".",
			"Ok"
		    };

		    EasyRequestArgs(NULL, &params, NULL, NULL);

		    CloseLibrary(IntuitionBase);
		}

	    } else
		PutStr("Can't load \"PROGDIR:Apdf.amigaos\".\n");
	}

	CloseLibrary(DOSBase);
    }

    if (wbmsg) {
	Forbid();
	ReplyMsg(wbmsg);
    }

    return ret;
}

