//========================================================================
//
// libppc.c
//
// Copyright 2000-2001 Emmanuel Lesueur
//
//========================================================================

/*#include <exec/types.h>
#include <exec/libraries.h>
struct TagItem;
struct ExAllData;
struct ExAllControl;
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/gcclib/powerup_protos.h>*/

struct ExecBase *SysBase;

char* __LIB_LibTable[]={
    "getBase",0
};

extern void (*funcTable[])();

unsigned __LIB_Version=1;
unsigned __LIB_Revision=0;

void (**getBase(void))() {
    return funcTable;
}

void* __LIB_Init(void* object) {
    SysBase = *(struct ExecBase**)4;
    return object;
}

