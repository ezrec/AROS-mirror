/*
    Copyright (C) 1997 AROS - The Amiga Research OS
    $Id$

    Desc: CGFX Library
    Lang: english
*/

#define NO_MEMORY_ALLOCATION_DEBUG_ALIASING

#include "include/classdefs.h"

/*
 * Global data (written to once at initalization time).
 */
BPTR           SegList = 0;
#ifndef ENHANCED
makeproto BOOL OS30    = FALSE;
#endif

struct Library       *BGUIBase      = NULL;
struct DosLibrary    *DOSBase       = NULL;
struct ExecBase      *SysBase       = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase       *GfxBase       = NULL;
struct Library       *GadToolsBase  = NULL; /* Menu stuff. */
struct Library       *UtilityBase   = NULL;
struct Library       *LayersBase    = NULL;
struct Library       *AslBase       = NULL;
struct Library       *CxBase        = NULL;
struct Library       *KeymapBase    = NULL;
struct Library       *IFFParseBase  = NULL;
struct Library       *DataTypesBase = NULL;
struct Library       *LocaleBase    = NULL;
struct Library       *WorkbenchBase = NULL;
struct Catalog       *Catalog       = NULL;

struct Task          *InputDevice   = NULL;
APTR                  InputStack    = NULL;

#ifdef SysBase
#   undef SysBase
#endif

/* Customize libheader.c */
#define LC_SYSBASE_FIELD(lib)   SysBase
#define LC_SEGLIST_FIELD(lib)   SegList
#define LC_LIBBASESIZE		sizeof(struct Library)
#define LC_LIBHEADERTYPEPTR	LIBBASETYPEPTR
#define LC_LIB_FIELD(lib)       lib
#define LC_STATIC_OPENLIB
#define LC_STATIC_CLOSELIB
#define LC_STATIC_EXPUNGELIB
#define LC_STATIC_INITLIB

#include <libcore/libheader.c>

/********************************************************************************************/

SAVEDS STATIC VOID CloseLibs(void)
{
    if (LocaleBase)      CloseLibrary(LocaleBase);
    if (KeymapBase)      CloseLibrary(KeymapBase);
    if (CxBase)          CloseLibrary(CxBase);
    if (AslBase)         CloseLibrary(AslBase);
    if (LayersBase)      CloseLibrary(LayersBase);
    if (UtilityBase)     CloseLibrary(UtilityBase);
    if (GadToolsBase)    CloseLibrary(GadToolsBase);
    if (DataTypesBase)   CloseLibrary(DataTypesBase);
    if (IFFParseBase)    CloseLibrary(IFFParseBase);
    if (WorkbenchBase)   CloseLibrary(WorkbenchBase);
    if (GfxBase)         CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase)   CloseLibrary((struct Library *)IntuitionBase);
    if (DOSBase)         CloseLibrary((struct Library *)DOSBase);
}

/********************************************************************************************/

static ULONG SAVEDS STDARGS LC_BUILDNAME(L_InitLib) (LIBBASETYPEPTR LIBBASE)
{
    struct Library       *lib;

    /*
     * Open up system libraries.
     */
    #ifdef ENHANCED
    DOSBase        = (struct DosLibrary *)   OpenLibrary("dos.library",       39);
    IntuitionBase  = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
    GfxBase        = (struct GfxBase *)      OpenLibrary("graphics.library",  39);
    #else
    OS30           = (((struct Library *)SysBase)->lib_Version >= 39);

    DOSBase        = (struct DosLibrary *)   OpenLibrary("dos.library",       37);
    IntuitionBase  = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
    GfxBase        = (struct GfxBase *)      OpenLibrary("graphics.library",  37);
    #endif

    GadToolsBase   = OpenLibrary("gadtools.library",      37);
    UtilityBase    = OpenLibrary("utility.library",       37);
    LayersBase     = OpenLibrary("layers.library",        37);
    AslBase        = OpenLibrary("asl.library",           37);
    CxBase         = OpenLibrary("commodities.library",   37);
    KeymapBase     = OpenLibrary("keymap.library",        37);
    IFFParseBase   = OpenLibrary("iffparse.library",      37);
    WorkbenchBase  = OpenLibrary("workbench.library",     37);
    LocaleBase     = OpenLibrary("locale.library",        38);
    DataTypesBase  = OpenLibrary("datatypes.library",     39);

    InitInputStack();

 #ifdef OUTDATE_BUILD
       if(DOSBase)
       {
          static struct DateStamp expiry_date=
          {
             OUTDATE_BUILD,0,0
          };
          struct DateStamp today;

          DateStamp(&today);
          if(CompareDates(&today,&expiry_date)<0)
          {
	     /* if we return FALSE here, Expunge is called by libheader.c and so
		the libs are close there */

             /* CloseLibs(); */
             return FALSE;
          }
       }
 #endif
    /*
     * All libraries open?
     */
    if (DOSBase && IntuitionBase && GfxBase && GadToolsBase && UtilityBase && LayersBase && IFFParseBase && WorkbenchBase
	&& AslBase && CxBase && KeymapBase)
    {
          /*
           * Initialize the task-list.
           */
          InitTaskList();

          return TRUE;

    }

    /* if we return FALSE here, Expunge is called by libheader.c and so
       the libs are close there */

    /* CloseLibs(); */

    return FALSE;
}

/********************************************************************************************/

static void SAVEDS STDARGS LC_BUILDNAME(L_ExpungeLib) (LIBBASETYPEPTR LIBBASE)
{
    if (Catalog)
        CloseCatalog(Catalog);

    FreeInputStack();    
	
    /*
     * Close system libraries.
     */
    CloseLibs();
    
    FreeTaskList();
}

/********************************************************************************************/

static ULONG SAVEDS STDARGS LC_BUILDNAME(L_OpenLib) (LC_LIBBASETYPEPTR lh)
{
    UWORD       tc;

    /*
     * Add this task to the list.
     */
    if ((tc = AddTaskMember()) != TASK_FAILED)
    {
	/*
	 * Increase open counter when necessary.
	 */
	
	#if 0
	if (tc == TASK_ADDED)
            lib->lib_OpenCnt++;*/
	#else
	/* libheader.c always incs opencnt, so we must do this a bit different */
	if (tc != TASK_ADDED) lh->lib_OpenCnt--;
	#endif
	return TRUE;
    }

    return FALSE;
}

/********************************************************************************************/

static void  SAVEDS STDARGS LC_BUILDNAME(L_CloseLib) (LC_LIBBASETYPEPTR lh)
{
#warning real BGUI CloseLib (in lib.c) checks if it is safe to closelib or not and if it is not it does not close (dec usecount and maybe expunge)
   /*
    * Remove the task from the member list.
    */
   FreeTaskMember();
}

