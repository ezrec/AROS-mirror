#ifndef GLOBAL_H
#define GLOBAL_H

//#define VERSION_STRING "1.99"
//#define DAY_STRING "09"
//#define MONTH_STRING "05"
//#define YEAR2_STRING "02"
//#define YEAR4_STRING "2002"

//#define VERSTRING "$VER: ReqAttack " VERSION_STRING " (" DAY_STRING "." MONTH_STRING "." YEAR4_STRING ")"

#ifdef __AROS__
#define __DATE2__ "23.07.2003"
#endif

#define NAME_STRING "ReqAttack"
#define VERSION_STRING "2.00"
#define DATE_STRING __DATE2__
#define DESCRIPTION_STRING ""

#define VERSTRING "\0$VER: "NAME_STRING" "VERSION_STRING" ("DATE_STRING") "DESCRIPTION_STRING
#define RAPREFS "RAPrefsMUI Warp Core"

//#include <clib/ddebug_protos.h>

#define D(x)
#define bug kprintf

#ifdef __MAXON__
#define REG(reg,type,name) register __ ## reg type name
#endif

#ifdef __STORM__
#define REG(reg,type,name) register __ ## reg type name
#endif

#ifdef __GNUC__
//#define REG(reg,type,name) register type name __asm(#reg)
#ifdef __AROS__
#define REG(reg,arg,name) arg name
#else
#define REG(reg, arg, name) arg name __asm(#reg)
#endif
#endif

#ifndef REG
#error "Change REG macro in global.h for your compiler!"
#endif

#ifndef REQATTACK_H
#include "reqattack.h"
#endif

struct ProgPort
{
	struct MsgPort port;
	struct SignalSemaphore *reqlistsem;
	struct List *reqlist;
	WORD configcounter;
	BYTE prefsappactive;
	struct Library *sublibrary;
};

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *DiskfontBase;
extern struct Library *LayersBase;
/*extern struct UtilityBase *UtilityBase;*/
extern struct Library *KeymapBase;
extern struct Library *CyberGfxBase;
extern struct Library *CxBase;
extern struct Library *ReqToolsBase;
extern struct PopupMenuBase *PopupMenuBase;
extern struct Library *DataTypesBase;
extern struct Library *WorkbenchBase;
extern struct Library *IconBase;
//extern struct Library *DOpusBase;

extern struct ProgPort progport;
extern struct RastPort temprp;
extern struct BitMap tempbm;
extern struct SignalSemaphore temprpsem;

extern struct SignalSemaphore scmdsem;
extern struct SignalSemaphore rapsem;

extern BOOL fromwb,cyreq;

extern char *raprefspath;

#ifndef __AROS__
void kprintf(char *s,...);	/* debug.lib */
#endif

void Cleanup(char *msg);

#endif

