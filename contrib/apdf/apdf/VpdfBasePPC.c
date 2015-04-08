/*************************************************************************\
 *                                                                       *
 * VpdfBasePPC.c                                                         *
 *                                                                       *
 * Copyright 2000-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#define classlib base->u.apdf_Base2
#undef _NO_PPCINLINE

#include "config.h"
#include <stddef.h>
#include <string.h>
#include <exec/types.h>
#include <exec/resident.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include "VpdfBase.h"
#include "Apdf.h"
#include <ppcinline/macros.h>

#define DB(x)   //x

#define SysBase         base->apdf_SysBase
#define DOSBase         base->apdf_DOSBase
#define UtilityBase     base->apdf_UtilityBase
#define IntuitionBase   base->apdf_IntuitionBase

int LibReserved(void) {
    return 0;
}

ULONG __amigappc__=1;

struct ApdfBase *LibInit(struct ApdfBase *base,BPTR,
			 struct Library *sysbase);
struct ApdfBase *LibOpen(void);
BPTR LibClose(void);
BPTR LibExpunge();
BPTR LibExpungeFunc(struct ApdfBase *base);

struct TagItem *vplug_Query(void);
APTR vplug_ProcessURLMethod(void);
APTR vplug_GetURLData(void);
STRPTR vplug_GetURLMIMEType(void);
void vplug_FreeURLData(void);
APTR vplug_GetClass(void);
BOOL vplug_Setup(void);
void vplug_FinalSetup(void);
void vplug_Cleanup(void);
void vplug_Hook_Prefs(void);
BOOL vplug_GetInfo(void);

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
    RTF_PPC|RTF_AUTOINIT,
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
    (APTR)FUNCARRAY_32BIT_NATIVE,
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
#define INITAPTR_ENTRY(x)   UWORD a##x;UWORD b##x;APTR __attribute__((aligned(2))) c##x
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
} __attribute__((packed)) LibInitData={
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
    (APTR)&LibInit
};



struct ApdfBase *LibInit(struct ApdfBase *base,
			 BPTR seglist,
			 struct Library *sysbase) {
    base->apdf_SegList=seglist;
    base->apdf_SysBase=sysbase;
    InitSemaphore(&base->apdf_Lock);
    base->u.apdf_Base2=NULL;

    if(sysbase->lib_Version>=39)
	return base;

    return NULL;
}


struct ApdfBase *LibOpen(void) {
    struct ApdfBase *base=(APTR)REG_A6;
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


BPTR LibClose(void) {
    struct ApdfBase *base=(APTR)REG_A6;
    BPTR retval=0;

    ObtainSemaphore(&base->apdf_Lock);

    if(base->apdf_Lib.lib_OpenCnt>0)
	--base->apdf_Lib.lib_OpenCnt;

    if(base->apdf_Lib.lib_OpenCnt==0) {
	CloseLibrary(&base->u.apdf_Base2->apdf_Lib);
	base->u.apdf_Base2=NULL;
	if(base->apdf_Lib.lib_Flags&LIBF_DELEXP)
	    retval=LibExpungeFunc(base);
    }

    ReleaseSemaphore(&base->apdf_Lock);
    return retval;
}


BPTR LibExpungeFunc(struct ApdfBase *base) {
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
BPTR LibExpunge() {
    struct ApdfBase *base=(APTR)REG_A6;
    return LibExpungeFunc(base);
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
struct TagItem *vplug_Query(void) {
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
APTR vplug_ProcessURLMethod(void) {
    STRPTR url=(STRPTR)REG_A0;
    DB(kprintf("*** VPLUG_ProcessURLMethod(%s)\n",url);)
    return NULL;
}

APTR vplug_GetURLData(void) {
    APTR handle=(APTR)REG_A0;
    DB(kprintf("*** VPLUG_GetURLData\n");)
    return NULL;
}

STRPTR vplug_GetURLMIMEType(void) {
    APTR handle=(APTR)REG_A0;
    DB(kprintf("*** VPLUG_GetURLMIMEType\n");)
    return NULL;
}

void vplug_FreeURLData(void) {
    APTR handle=(APTR)REG_A0;
    DB(kprintf("*** VPLUG_GetURLData\n");)
}

/* Embedding objects */
APTR vplug_GetClass(void) {
    STRPTR mimetype=(STRPTR)REG_A0;
    struct ApdfBase *base=(APTR)REG_A6;
    APTR cl=NULL;
    DB(kprintf("*** VPLUG_GetClass(%s)\n",mimetype);)
    open_lib(base);
    if(base->u.apdf_Base2)
	cl=VPLUG_GetClass(mimetype);
    return cl;
}

/* Called after plugin is loaded */
BOOL vplug_Setup(void) {
    struct vplug_functable *table=(APTR)REG_A0;
    struct ApdfBase *base=(APTR)REG_A6;
    DB(kprintf("*** VPLUG_Setup\n");)
    if(base->u.apdf_Base2)
	VPLUG_Setup(table);
    else
	base->apdf_FuncTable=table;
    return TRUE;
}

/* Called after all plugins are loaded and _Setup()ed */
void vplug_FinalSetup(void) {
    DB(kprintf("*** VPLUG_FinalSetup\n");)
}

/* Called before plugin is unloaded */
void vplug_Cleanup(void) {
    DB(kprintf("*** VPLUG_Cleanup\n");)
}

/* Prefs callback hook */
void vplug_Hook_Prefs(void) {
    ULONG methodid=(ULONG)REG_D0;
    struct vplug_prefs *prefs=(APTR)REG_A0;
    struct ApdfBase *base=(APTR)REG_A6;
    DB(kprintf("*** VPLUG_Hook_Prefs(%d)\n",methodid);)
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


BOOL vplug_GetInfo(void) {
    struct VPlugInfo *info=(APTR)REG_A0;
    APTR nethandle=(APTR)REG_A1;
    DB(kprintf("*** VPLUG_GetInfo\n");)
    return TRUE;
}

