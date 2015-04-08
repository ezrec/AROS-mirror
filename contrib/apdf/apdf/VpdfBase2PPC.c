/*************************************************************************\
 *                                                                       *
 * VpdfBasePPC.c                                                         *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

//#define BUILDPLUGIN
#define NO_PRAGMAS
#define NO_PROTO

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

static const char LibName[]="Vpdf.module";
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
    //dprintf("### init\n");
    base->apdf_SegList=seglist;
    base->apdf_SysBase=sysbase;
    InitSemaphore(&base->apdf_Lock);
    base->u.apdf_MCC=NULL;

    if(sysbase->lib_Version>=39) {
	//if(init())
	    return base;
#if 0
	if(base->apdf_IntuitionBase=OpenLibrary("intuition.library",39)) {
	    if(base->apdf_DOSBase=OpenLibrary("dos.library",39)) {
		if(base->apdf_UtilityBase=OpenLibrary("utility.library",39)) {
		    /*if(MUIMasterBase=OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)) {*/
			return base;
		    /*}
		    CloseLibrary(base->apdf_UtilityBase);*/
		}
		CloseLibrary(base->apdf_DOSBase);
	    }
	    CloseLibrary(base->apdf_IntuitionBase);
	}
#endif
    }
    return NULL;
}


struct ApdfBase *LibOpen(void) {
    struct ApdfBase *base=(APTR)REG_A6;
    struct ApdfBase *retval=NULL;
    //dprintf("### open\n");

    ObtainSemaphore(&base->apdf_Lock);

    /*
     *  Make sure that this library is opened only once at
     *  a given time. Voyager won't do that anyway, but just
     *  in case...
     */
    if(base->apdf_Lib.lib_OpenCnt==0) {
	if(init()) {
	    ++base->apdf_Lib.lib_OpenCnt;
	    base->apdf_Lib.lib_Flags&=~LIBF_DELEXP;
	    retval=base;
	}
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
	if(!cleanup())
	    base->apdf_Lib.lib_Flags|=LIBF_DELEXP;
	else
	    base->u.apdf_MCC=NULL;
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
	if(cleanup()) {
	    base->u.apdf_MCC=NULL;
	    ret=base->apdf_SegList;
	    Remove(&base->apdf_Lib.lib_Node);
	    FreeMem((UBYTE *)base-base->apdf_Lib.lib_NegSize,
		    base->apdf_Lib.lib_NegSize+base->apdf_Lib.lib_PosSize);
	}
    }
    return ret;
}
BPTR LibExpunge() {
    struct ApdfBase *base=(APTR)REG_A6;
    return LibExpungeFunc(base);
}

/* Get information */
struct TagItem *vplug_Query(void) {
    DB(kprintf("*** VPLUG_Query\n");)
    return NULL;
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
    APTR cl;
    DB(kprintf("*** VPLUG_GetClass(%s)\n",mimetype);)
    ObtainSemaphore(&base->apdf_Lock);
    if(!base->u.apdf_MCC)
	base->u.apdf_MCC=getclass();
    cl=base->u.apdf_MCC;
    ReleaseSemaphore(&base->apdf_Lock);
    return cl;
}

/* Called after plugin is loaded */
BOOL vplug_Setup(void) {
    struct vplug_functable *table=(APTR)REG_A0;
    DB(kprintf("*** VPLUG_Setup\n");)
    set_table(table);
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
    DB(kprintf("*** VPLUG_Hook_Prefs(%d)\n",methodid);)
    switch(methodid) {
      case VPLUGPREFS_Create:
	create_prefs(prefs);
	break;
      case VPLUGPREFS_Dispose:
	delete_prefs(prefs);
	break;
      case VPLUGPREFS_Use:
	do_use_prefs(prefs);
	break;
      case VPLUGPREFS_Load:
	do_load_prefs(prefs);
	break;
      case VPLUGPREFS_Save:
	do_save_prefs(prefs);
	break;
    }
}


BOOL vplug_GetInfo(void) {
    struct VPlugInfo *info=(APTR)REG_A0;
    APTR nethandle=(APTR)REG_A1;
    DB(kprintf("*** VPLUG_GetInfo\n");)
    return TRUE;
}

