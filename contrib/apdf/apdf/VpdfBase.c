/*************************************************************************\
 *                                                                       *
 * VpdfBase.c                                                            *
 *                                                                       *
 * Copyright 1999-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#ifdef __SASC
#   define SAVEDS    //__saveds confuses data=faronly
#   define ASM       __asm
#   define REG(r,a)  register __ ## r a
#elif defined(__GNUC__)
#   define SAVEDS    /*__geta4*/
#   define ASM
#   define REG(r,a)  a __asm(#r)
#endif

#define classlib base->u.apdf_Base2

#include <stddef.h>
#include <string.h>
#include <exec/types.h>
#include <exec/resident.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
/*#include <libraries/mui.h>*/
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
/*#include <proto/muimaster.h>*/
#include "VpdfBase.h"
#include "Apdf.h"

#define DB(x)   //x

#define SysBase         base->apdf_SysBase
#define DOSBase         base->apdf_DOSBase
#define UtilityBase     base->apdf_UtilityBase
#define IntuitionBase   base->apdf_IntuitionBase
#define MUIMasterBase   base->MUIMasterBase

int LibReserved(void) {
    return 0;
}

struct ApdfBase * ASM LibInit(REG(a0,BPTR seglist),
			      REG(d0,struct ApdfBase *base),
			      REG(a6,struct Library *sysbase));
struct ApdfBase * ASM LibOpen(REG(a6,struct ApdfBase *base));
BPTR ASM LibClose(REG(a6,struct ApdfBase *base));
BPTR ASM LibExpunge(REG(a6,struct ApdfBase *base));

struct TagItem * ASM vplug_Query(REG(a6,struct ApdfBase *base));
APTR ASM vplug_ProcessURLMethod(REG(a0,STRPTR url),
				REG(a6,struct ApdfBase *base));
APTR ASM vplug_GetURLData(REG(a0,APTR handle),
			  REG(a6,struct ApdfBase *base));
STRPTR ASM vplug_GetURLMIMEType(REG(a0,APTR handle),
				REG(a6,struct ApdfBase *base));
void ASM vplug_FreeURLData(REG(a0,APTR handle),
			   REG(a6,struct ApdfBase *base));
APTR ASM vplug_GetClass(REG(a0,STRPTR mimetype),
			REG(a6,struct ApdfBase *base));
BOOL ASM vplug_Setup(REG(a0,struct vplug_functable *table),
		     REG(a6,struct ApdfBase *base));
void ASM vplug_FinalSetup(REG(a6,struct ApdfBase *base));
void ASM vplug_Cleanup(REG(a6,struct ApdfBase *base));
void ASM vplug_Hook_Prefs(REG(d0,ULONG methodid),
			  REG(a0,struct vplug_prefs *prefs),
			  REG(a6,struct ApdfBase *base));
BOOL ASM vplug_GetInfo(REG(a0,struct VPlugInfo *),REG(a1,APTR nethandle),REG(a6,struct ApdfBase *));

APTR getclass(void);
BOOL init(void);
BOOL cleanup(void);
void set_table(struct vplug_functable *);
void create_prefs(struct vplug_prefs *);
void delete_prefs(struct vplug_prefs *);
void use_prefs(struct vplug_prefs *);
void load_prefs(struct vplug_prefs *);
void save_prefs(struct vplug_prefs *);

static const APTR LibInitTable[];
static const char LibName[];
static const char LibId[];

static const struct Resident romtag={
    RTC_MATCHWORD,
    (struct Resident *)&romtag,
    (APTR)((&romtag)+1),
    RTF_AUTOINIT,
    LIB_VERSION,
    NT_LIBRARY,
    0,
    (char*)LibName,
    (char*)LibId,
    (APTR)LibInitTable
};

static const char LibName[]="Vpdf.VPlug";
static const char LibId[]=vpdfVerString;




static const APTR LibFuncTable[]={
    LibOpen,
    LibClose,
    LibExpunge,
    LibReserved,
    vplug_Query,
    vplug_ProcessURLMethod,
    vplug_GetURLData,
    vplug_GetURLMIMEType,
    vplug_FreeURLData,
    vplug_GetClass,
    vplug_Setup,
    vplug_Cleanup,
    vplug_FinalSetup,
    vplug_Hook_Prefs,
    vplug_GetInfo,
    (APTR)-1
};

#define INITBYTE_ENTRY(x)   UWORD a##x;UWORD b##x;UBYTE c##x;UBYTE d##x
#define INITWORD_ENTRY(x)   UWORD a##x;UWORD b##x;UWORD c##x
#define INITAPTR_ENTRY(x)   UWORD a##x;UWORD b##x;APTR c##x
#define INITBYTE(S,E,x)     0xe000,offsetof(struct S,E),(x),0
#define INITWORD(S,E,x)     0xd000,offsetof(struct S,E),(x)
#define INITAPTR(S,E,x)     0xc000,offsetof(struct S,E),(x)

static const struct {
    INITBYTE_ENTRY(0);
    INITAPTR_ENTRY(1);
    INITBYTE_ENTRY(2);
    INITWORD_ENTRY(3);
    INITWORD_ENTRY(4);
    INITAPTR_ENTRY(5);
    UWORD end;
} LibInitData={
    INITBYTE(Node,ln_Type,NT_LIBRARY),
    INITAPTR(Node,ln_Name,(APTR)LibName),
    INITBYTE(Library,lib_Flags,LIBF_SUMUSED|LIBF_CHANGED),
    INITWORD(Library,lib_Version,LIB_VERSION),
    INITWORD(Library,lib_Revision,LIB_REVISION),
    INITAPTR(Library,lib_IdString,(APTR)LibId),
    0
};

static const APTR LibInitTable[]={
    (APTR)((sizeof(struct ApdfBase)+3)&~3),
    (APTR)LibFuncTable,
    (APTR)&LibInitData,
    (APTR)LibInit
};



struct ApdfBase * SAVEDS ASM LibInit(REG(a0,BPTR seglist),
				     REG(d0,struct ApdfBase *base),
				     REG(a6,struct Library *sysbase)) {
    base->apdf_SegList=seglist;
    base->apdf_SysBase=sysbase;
    InitSemaphore(&base->apdf_Lock);
    base->u.apdf_Base2=NULL;

    if(sysbase->lib_Version>=39)
	return base;

    return NULL;
}

struct ApdfBase * SAVEDS ASM LibOpen(REG(a6,struct ApdfBase *base)) {
    struct ApdfBase *retval=NULL;

    ObtainSemaphore(&base->apdf_Lock);

    /*
     *  Make sure that this library is opened only once at
     *  a given time. Voyager won't do that anyway, but just
     *  in case...
     */
    if(base->apdf_Lib.lib_OpenCnt==0) {
	++base->apdf_Lib.lib_OpenCnt;
	base->apdf_Lib.lib_Flags&=~LIBF_DELEXP;
	base->apdf_FuncTable=NULL;
	base->apdf_LoadPrefsFlag=FALSE;
	retval=base;
    }

    ReleaseSemaphore(&base->apdf_Lock);
    return retval;
}

BPTR SAVEDS ASM LibClose(REG(a6,struct ApdfBase *base)) {
    BPTR retval=0;

    ObtainSemaphore(&base->apdf_Lock);

    if(base->apdf_Lib.lib_OpenCnt>0)
	--base->apdf_Lib.lib_OpenCnt;

    if(base->apdf_Lib.lib_OpenCnt==0) {
	CloseLibrary(&base->u.apdf_Base2->apdf_Lib);
	base->u.apdf_Base2=NULL;
	if(base->apdf_Lib.lib_Flags&LIBF_DELEXP)
	    retval=LibExpunge(base);
    }

    ReleaseSemaphore(&base->apdf_Lock);
    return retval;
}

BPTR SAVEDS ASM LibExpunge(REG(a6,struct ApdfBase *base)) {
    BPTR ret=0;
    base->apdf_Lib.lib_Flags|=LIBF_DELEXP;
    if(base->apdf_Lib.lib_OpenCnt==0) {
	ret=base->apdf_SegList;
	Remove(&base->apdf_Lib.lib_Node);
	FreeMem((UBYTE *)base-base->apdf_Lib.lib_NegSize,
		base->apdf_Lib.lib_NegSize+base->apdf_Lib.lib_PosSize);
    }
    return ret;
}


static void open_lib(struct ApdfBase *base) {
    if(!base->u.apdf_Base2) {
	base->u.apdf_Base2=(APTR)OpenLibrary("PROGDIR:plugins/Vpdf.module",0);
	if(base->u.apdf_Base2) {
	    if(base->apdf_FuncTable)
		VPLUG_Setup(base->apdf_FuncTable);
	    if(base->apdf_LoadPrefsFlag)
		VPLUG_Hook_Prefs(VPLUGPREFS_Load,NULL);
	}
    }
}


/* Get information */
struct TagItem * SAVEDS ASM vplug_Query(REG(a6,struct ApdfBase *base)) {
    static struct TagItem tags[]={
	{VPLUG_Query_Version,LIB_VERSION},
	{VPLUG_Query_Revision,LIB_REVISION},
	{VPLUG_Query_Copyright,(ULONG)"(C) 1999-2002 Emmanuel Lesueur"},/***/
	{VPLUG_Query_Infostring,(ULONG)"Voyager PDF plugin"},
	{VPLUG_Query_APIVersion,3},
	{VPLUG_Query_HasPrefs,TRUE},
	{VPLUG_Query_PluginID,(ULONG)"Vpdf"},
	{VPLUG_Query_RegisterMIMEType,(ULONG)"application/pdf"},
	{VPLUG_Query_RegisterMIMEType,(ULONG)"application/vnd.fdf"},
	{VPLUG_Query_RegisterMIMEExtension,(ULONG)"pdf"},
	{VPLUG_Query_RegisterMIMEExtension,(ULONG)"fdf"},
	{TAG_END}
    };
    DB(kprintf("*** VPLUG_Query\n");)
    return tags;
}


/* URL processing */
APTR SAVEDS ASM vplug_ProcessURLMethod(REG(a0,STRPTR url),
				       REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_ProcessURLMethod(%s)\n",url);)
    return NULL;
}

APTR SAVEDS ASM vplug_GetURLData(REG(a0,APTR handle),
				 REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_GetURLData\n");)
    return NULL;
}

STRPTR SAVEDS ASM vplug_GetURLMIMEType(REG(a0,APTR handle),
				       REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_GetURLMIMEType\n");)
    return NULL;
}

void SAVEDS ASM vplug_FreeURLData(REG(a0,APTR handle),
				  REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_GetURLData\n");)
}

/* Embedding objects */
APTR SAVEDS ASM vplug_GetClass(REG(a0,STRPTR mimetype),
			       REG(a6,struct ApdfBase *base)) {
    APTR cl=NULL;
    DB(kprintf("*** VPLUG_GetClass(%s)\n",mimetype);)
    open_lib(base);
    if(base->u.apdf_Base2)
	cl=VPLUG_GetClass(mimetype);
    return cl;
}

/* Called after plugin is loaded */
BOOL SAVEDS ASM vplug_Setup(REG(a0,struct vplug_functable *table),
			    REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_Setup\n");)
    if(base->u.apdf_Base2)
	VPLUG_Setup(table);
    else
	base->apdf_FuncTable=table;
    return TRUE;
}

/* Called after all plugins are loaded and _Setup()ed */
void SAVEDS ASM vplug_FinalSetup(REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_FinalSetup\n");)
}

/* Called before plugin is unloaded */
void SAVEDS ASM vplug_Cleanup(REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_Cleanup\n");)
}

/* Prefs callback hook */
void SAVEDS ASM vplug_Hook_Prefs(REG(d0,ULONG methodid),
				 REG(a0,struct vplug_prefs *prefs),
				 REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_Hook_Prefs(%ld)\n",methodid);)
    switch(methodid) {
      case VPLUGPREFS_Create:
	prefs->object=NULL;
      case VPLUGPREFS_Dispose:
      case VPLUGPREFS_Use:
      case VPLUGPREFS_Save:
	open_lib(base);
	if(base->u.apdf_Base2)
	    VPLUG_Hook_Prefs(methodid,prefs);
	break;

      case VPLUGPREFS_Load:
	if(base->u.apdf_Base2)
	    VPLUG_Hook_Prefs(methodid,prefs);
	else
	    base->apdf_LoadPrefsFlag=TRUE;
	break;
    }
}


BOOL SAVEDS ASM vplug_GetInfo(REG(a0,struct VPlugInfo *info),
			      REG(a1,APTR nethandle),
			      REG(a6,struct ApdfBase *base)) {
    DB(kprintf("*** VPLUG_GetInfo\n");)
    return TRUE;
}

