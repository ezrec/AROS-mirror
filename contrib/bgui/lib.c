/*
 * @(#) $Header$
 *
 * BGUI library
 * lib.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.13  2004/07/11 17:10:21  stegerg
 * LibInit() function may not leave out D0 param, even if unused,
 * otherwise it crashes on machines where all arguments are passed
 * on stack.
 *
 * Revision 42.12  2004/06/20 12:24:31  verhaegs
 * Use REGFUNC macro's in BGUI source code, not AROS_UFH
 *
 * Revision 42.11  2004/06/19 20:27:48  verhaegs
 * Added AROS_LIBFUNC_INIT/EXIT
 *
 * Revision 42.10  2003/01/18 19:09:58  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.9  2002/03/07 21:33:00  stegerg
 * These used still AROS_LHA for the arguments of libinit,
 * instead of AROS_UFHA.
 *
 * Revision 42.8  2001/01/28 04:53:21  bergers
 * Fixed some compiler complaints (some casts were missing.).
 *
 * Revision 42.7  2000/08/08 20:57:26  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.6  2000/08/08 14:08:00  chodorowski
 * Minor fixes to make BGUI compile on Amiga.
 *
 * Revision 42.5  2000/07/06 16:44:03  stegerg
 * AddTaskMember can now be called. Problem was Cli()->cli_CommandName
 * which BGUI expected to be a BSTR with size in first byte.
 *
 * Revision 42.4  2000/07/03 20:45:44  bergers
 * Update. Test1 now successfully opens the library. In LibOpen the AddTaskMember function seems to trash the stack somehow (return address is trashed) and therefore I had to take it out.
 *
 * Revision 42.3  2000/06/01 01:41:37  bergers
 * Only 2 linker problems left: stch_l & stcu_d. Somebody might want to replace them (embraced by #ifdef __AROS__), please.
 *
 * Revision 42.2  2000/05/31 01:23:10  bergers
 * Changes to make BGUI compilable and linkable.
 *
 * Revision 42.1  2000/05/29 00:40:24  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.0  2000/05/09 22:09:20  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:31  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.4  1999/07/24 17:08:08  mlemos
 * Removed the code that flushed the memory to remove the current catalog.
 *
 * Revision 41.10.2.3  1998/10/01 04:25:58  mlemos
 * Made the library call the functions to allocate and free the memory for the
 * pre-allocated stack memory space for use by the input.device task.
 *
 * Revision 41.10.2.2  1998/06/18 23:14:15  mlemos
 * Added code to outdate a given library build.
 *
 * Revision 41.10.2.1  1998/03/01 02:21:57  mlemos
 * Added new memory allocation debugging functions to the library
 *
 * Revision 41.10  1998/02/25 21:12:24  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:08:46  mlemos
 * Ian sources
 *
 *
 */

#define NO_MEMORY_ALLOCATION_DEBUG_ALIASING

#include "include/classdefs.h"

#ifdef __AROS__
#include <aros/debug.h>
#endif

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
#ifdef __AROS__
struct UtilityBase   *UtilityBase   = NULL;
#else
struct Library       *UtilityBase   = NULL;
#endif
struct Library       *LayersBase    = NULL;
struct Library       *AslBase       = NULL;
struct Library       *CxBase        = NULL;
struct Library       *KeymapBase    = NULL;
struct Library       *IFFParseBase  = NULL;
struct Library       *DataTypesBase = NULL;
struct LocaleBase    *LocaleBase    = NULL;
struct Library       *WorkbenchBase = NULL;
struct Catalog       *Catalog       = NULL;

struct Task          *InputDevice   = NULL;
APTR                  InputStack    = NULL;


#ifdef __AROS__
AROS_LD1(struct Library *, LibOpen,
    AROS_LHA(ULONG, version, D0),
    struct Library *, lib, 1, BGUI);

AROS_LD0(BPTR, LibClose,
         struct Library *, lib, 2, BGUI);

AROS_LD1(BPTR, LibExpunge,
    AROS_LHA(struct Library *, lib, D0),
    struct ExecBase *, sysBase, 3, BGUI);

AROS_LD0(LONG, LibVoid,
     struct Library *, lib, 4, BGUI);

   extern BGUI_BGUI_GetClassPtr();
   extern BGUI_BGUI_NewObjectA();
   extern BGUI_BGUI_RequestA();
   extern BGUI_BGUI_Help();
   extern BGUI_BGUI_LockWindow();
   extern BGUI_BGUI_UnlockWindow();
   extern BGUI_BGUI_DoGadgetMethodA();
   /*
    * Private routines.
    */
   extern BGUI_BGUI_AllocPoolMem();
   extern BGUI_BGUI_FreePoolMem();
   /*
    * Public routines.
    */ 
   extern BGUI_BGUI_AllocBitMap();
   extern BGUI_BGUI_FreeBitMap();
   extern BGUI_BGUI_CreateRPortBitMap();
   extern BGUI_BGUI_FreeRPortBitMap();
   extern BGUI_BGUI_InfoTextSize();
   extern BGUI_BGUI_InfoText();
   extern BGUI_BGUI_GetLocaleStr();
   extern BGUI_BGUI_GetCatalogStr();
   extern BGUI_BGUI_FillRectPattern();
   extern BGUI_BGUI_PostRender();
   extern BGUI_BGUI_MakeClassA();
   extern BGUI_BGUI_FreeClass();
   extern BGUI_BGUI_PackStructureTags();
   extern BGUI_BGUI_UnpackStructureTags();
   /*
    * Private routines.
    */
   extern BGUI_BGUI_GetDefaultTags();
   extern BGUI_BGUI_DefaultPrefs();
   extern BGUI_BGUI_LoadPrefs();
   extern BGUI_BGUI_AllocPoolMemDebug();
   extern BGUI_BGUI_FreePoolMemDebug();
#endif

/*
 * Library function table.
 */
STATIC const LONG Vectors[] = {
#ifdef __AROS__
   /*
    * System interface.
    */
   (LONG)BGUI_LibOpen,
   (LONG)BGUI_LibClose,
   (LONG)BGUI_LibExpunge,
   (LONG)BGUI_LibVoid,
   /*
    * Public routines.
    */
   (LONG)BGUI_BGUI_GetClassPtr,
   (LONG)BGUI_BGUI_NewObjectA,
   (LONG)BGUI_BGUI_RequestA,
   (LONG)BGUI_BGUI_Help,
   (LONG)BGUI_BGUI_LockWindow,
   (LONG)BGUI_BGUI_UnlockWindow,
   (LONG)BGUI_BGUI_DoGadgetMethodA,
   /*
    * Private routines.
    */
   (LONG)BGUI_BGUI_AllocPoolMem,
   (LONG)BGUI_BGUI_FreePoolMem,
   /*
    * Public routines.
    */
   (LONG)BGUI_BGUI_AllocBitMap,
   (LONG)BGUI_BGUI_FreeBitMap,
   (LONG)BGUI_BGUI_CreateRPortBitMap,
   (LONG)BGUI_BGUI_FreeRPortBitMap,
   (LONG)BGUI_BGUI_InfoTextSize,
   (LONG)BGUI_BGUI_InfoText,
   (LONG)BGUI_BGUI_GetLocaleStr,
   (LONG)BGUI_BGUI_GetCatalogStr,
   (LONG)BGUI_BGUI_FillRectPattern,
   (LONG)BGUI_BGUI_PostRender,
   (LONG)BGUI_BGUI_MakeClassA,
   (LONG)BGUI_BGUI_FreeClass,
   (LONG)BGUI_BGUI_PackStructureTags,
   (LONG)BGUI_BGUI_UnpackStructureTags,
   /*
    * Private routines.
    */
   (LONG)BGUI_BGUI_GetDefaultTags,
   (LONG)BGUI_BGUI_DefaultPrefs,
   (LONG)BGUI_BGUI_LoadPrefs,
   (LONG)BGUI_BGUI_AllocPoolMemDebug,
   (LONG)BGUI_BGUI_FreePoolMemDebug,
#else
   /*
    * System interface.
    */
   (LONG)LibOpen,
   (LONG)LibClose,
   (LONG)LibExpunge,
   (LONG)LibVoid,
   /*
    * Public routines.
    */

   (LONG)BGUI_GetClassPtr,
   (LONG)BGUI_NewObjectA,
   (LONG)BGUI_RequestA,
   (LONG)BGUI_Help,
   (LONG)BGUI_LockWindow,
   (LONG)BGUI_UnlockWindow,
   (LONG)BGUI_DoGadgetMethodA,
   /*
    * Private routines.
    */
   (LONG)BGUI_AllocPoolMem,
   (LONG)BGUI_FreePoolMem,
   /*
    * Public routines.
    */
   (LONG)BGUI_AllocBitMap,
   (LONG)BGUI_FreeBitMap,
   (LONG)BGUI_CreateRPortBitMap,
   (LONG)BGUI_FreeRPortBitMap,
   (LONG)BGUI_InfoTextSize,
   (LONG)BGUI_InfoText,
   (LONG)BGUI_GetLocaleStr,
   (LONG)BGUI_GetCatalogStr,
   (LONG)BGUI_FillRectPattern,
   (LONG)BGUI_PostRender,
   (LONG)BGUI_MakeClassA,
   (LONG)BGUI_FreeClass,
   (LONG)BGUI_PackStructureTags,
   (LONG)BGUI_UnpackStructureTags,
   /*
    * Private routines.
    */
   (LONG)BGUI_GetDefaultTags,
   (LONG)BGUI_DefaultPrefs,
   (LONG)BGUI_LoadPrefs,
   (LONG)BGUI_AllocPoolMemDebug,
   (LONG)BGUI_FreePoolMemDebug,
#endif
   /*
    * Table end marker.
    */
   ~0L
};

/*
 * Close opened libraries.
 */
SAVEDS STATIC VOID CloseLibs(void)
{
   if( LocaleBase )      CloseLibrary( (struct Library *) LocaleBase);
   if( KeymapBase )      CloseLibrary( KeymapBase );
   if( CxBase )          CloseLibrary( CxBase );
   if( AslBase )         CloseLibrary( AslBase );
   if( LayersBase )      CloseLibrary( LayersBase );
   if( UtilityBase )     CloseLibrary( (struct Library *) UtilityBase );
   if( GadToolsBase )    CloseLibrary( GadToolsBase );
   if( DataTypesBase )   CloseLibrary( DataTypesBase );
   if( IFFParseBase )    CloseLibrary( IFFParseBase );
   if( WorkbenchBase )   CloseLibrary( WorkbenchBase );
   if( GfxBase )         CloseLibrary( (struct Library *) GfxBase );
   if( IntuitionBase )   CloseLibrary( (struct Library *) IntuitionBase );
   if( DOSBase )         CloseLibrary( (struct Library *) DOSBase );
}

/*
 * Setup localized strings.
 */
makeproto VOID InitLocale(void)
{
   UWORD    num;

   /*
    * locale.library opened?
    */
   if ( LocaleBase 
   &&   Catalog==NULL) {
      /*
       * Obtain lock.
       */
      ObtainSemaphore( &TaskLock );

      /*
       * Open up the catalog.
       */
      if ( Catalog = OpenCatalog( NULL, "BGUI.catalog", OC_BuiltInLanguage, "english", OC_Version, 2, TAG_END )) {
         /*
          * Get strings.
          */
         for ( num = 0; num < NumCatCompStrings; num++ )
            CatCompArray[ num ].cca_Str = GetCatalogStr( Catalog, num, CatCompArray[ num ].cca_Str );
      }

      /*
       * Free access.
       */
      ReleaseSemaphore( &TaskLock );
   }
}

/*
 * Library initialization.
 */
//makeproto SAVEDS ASM struct Library *LibInit(REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib)
#ifndef __AROS__
makeproto SAVEDS
#else
makeproto
#endif
ASM REGFUNC3(struct Library *, LibInit,
    	     REGPARAM(D0, ULONG, dummy),
	     REGPARAM(A0, BPTR, segment),
	     REGPARAM(A6, struct ExecBase *, syslib)
)
{
   struct Library       *lib;

   /*
    * Assign SysBase.
    */

   SysBase  = syslib;

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

   LocaleBase     = (struct LocaleBase *) OpenLibrary("locale.library",        38);

   GadToolsBase   = OpenLibrary("gadtools.library",      37);
   UtilityBase    = (struct UtilityBase *) OpenLibrary("utility.library",       37);
   LayersBase     = OpenLibrary("layers.library",        37);
   AslBase        = OpenLibrary("asl.library",           37);
   CxBase         = OpenLibrary("commodities.library",   37);
   KeymapBase     = OpenLibrary("keymap.library",        37);
   IFFParseBase   = OpenLibrary("iffparse.library",      37);
   WorkbenchBase  = OpenLibrary("workbench.library",     37);
   DataTypesBase  = OpenLibrary("datatypes.library",     39);

#ifdef __AROS__
#warning Commented InitInputStack
#else
   InitInputStack();
#endif

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
            CloseLibs();
            return(NULL);
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
       * Create the library.
       */
      if (BGUIBase = lib = MakeLibrary((APTR)Vectors, NULL, NULL, sizeof(struct Library), NULL))
      {
         /*
          * Initialize library structure.
          */
         lib->lib_Node.ln_Type = NT_LIBRARY;
         lib->lib_Node.ln_Name = (UBYTE *)LibName;
         lib->lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
         lib->lib_Version      = LIBVER;
         lib->lib_Revision     = LIBREV;
         lib->lib_IdString     = (APTR)LibID;

         /*
          * Globally assign the seglist.
          */
         SegList = segment;

         /*
          * Add us to the system.
          */
         AddLibrary(lib);

         /*
          * Initialize the task-list.
          */
         InitTaskList();

         return(lib);
      }
   }
   CloseLibs();
   return NULL;
}
REGFUNC_END

/*
 * Open library.
 */
#ifdef __AROS__
makeproto
AROS_LH1(struct Library *, LibOpen,
    AROS_LHA(ULONG, version, D0),
    struct Library *, lib, 1, BGUI)
#else
makeproto SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver)
#endif
{
   AROS_LIBFUNC_INIT

   UWORD       tc;

   /*
    * Add this task to the list.
    */

   if ((tc = AddTaskMember()) != TASK_FAILED)
   {
      /*
       * Increase open counter when necessary.
       */
      if (tc == TASK_ADDED)
         lib->lib_OpenCnt++;

      /*
       * Clear delayed expunge flag.
       */
      lib->lib_Flags &= ~LIBF_DELEXP;

      /*
       * Return library base or NULL upon failure.
       */
      return lib;
   }

   return NULL;
    
   AROS_LIBFUNC_EXIT
}

/*
 * Close library.
 */
#ifdef __AROS__
AROS_LH0(BPTR, LibClose,
         struct Library *, lib, 2, BGUI)
#else
makeproto SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib)
#endif
{
   AROS_LIBFUNC_INIT

   /*
    * Remove the task from the member list.
    */
   if (!FreeTaskMember())
      return NULL;

   /*
    * Of course we do not expunge when we still have accessors.
    */
   if (lib->lib_OpenCnt && --lib->lib_OpenCnt)
      return NULL;

   /*
    * Delayed expunge pending?
    */
   if (lib->lib_Flags & LIBF_DELEXP)
#ifdef __AROS__
      return AROS_UFC2(BPTR, BGUI_LibExpunge,
                AROS_UFCA(struct Library *, lib, D0),
                AROS_UFCA(struct ExecBase *, SysBase, A6));
#else
      return LibExpunge(lib);
#endif
   /*
    * Otherwise we remain in memory.
    */
   return NULL;
    
   AROS_LIBFUNC_EXIT
}

/*
 * Expunge library.
 */
#ifdef __AROS__
AROS_LH1(BPTR, LibExpunge,
    AROS_LHA(struct Library *, lib, D0),
    struct ExecBase *, sysBase, 3, BGUI)
#else
makeproto SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib)
#endif
{
   AROS_LIBFUNC_INIT

   /*
    * No expunge when we still
    * have accessors or when the classes
    * fail to free.
    */
   if (lib->lib_OpenCnt || (!FreeClasses()))
   {
      /*
       * Set delayed expunge flag.
       */
      lib->lib_Flags |= LIBF_DELEXP;
      return NULL;
   };

   /*
    * Cleanup locale stuff.
    */
   if (Catalog)
      CloseCatalog(Catalog);

#ifdef __AROS__
#warning Commented FreeInputStack
#else
   FreeInputStack();
#endif

   /*
    * Close system libraries.
    */
   CloseLibs();

   FreeTaskList();

   /*
    * Remove us from the system and deallocate
    * the memory we took up.
    */
   Remove(&lib->lib_Node);
   FreeMem((char *)lib - lib->lib_NegSize, lib->lib_NegSize + lib->lib_PosSize);

   /*
    * Return the seglist so that the
    * system can unload us.
    */
   return SegList;
    
   AROS_LIBFUNC_EXIT
}

/*
 * Reserved routine.
 */
#ifdef __AROS__
AROS_LH0(LONG, LibVoid,
     struct Library *, lib, 4, BGUI)
#else
makeproto SAVEDS LONG LibVoid(void)
#endif
{
   AROS_LIBFUNC_INIT

   return 0;

   AROS_LIBFUNC_EXIT
}
