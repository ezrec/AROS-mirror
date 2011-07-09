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

   extern VOID BGUI_5_BGUI_GetClassPtr();
   extern VOID BGUI_6_BGUI_NewObjectA();
   extern VOID BGUI_7_BGUI_RequestA();
   extern VOID BGUI_8_BGUI_Help();
   extern VOID BGUI_9_BGUI_LockWindow();
   extern VOID BGUI_10_BGUI_UnlockWindow();
   extern VOID BGUI_11_BGUI_DoGadgetMethodA();
   /*
    * Private routines.
    */
   extern VOID BGUI_12_BGUI_AllocPoolMem();
   extern VOID BGUI_13_BGUI_FreePoolMem();
   /*
    * Public routines.
    */ 
   extern VOID BGUI_14_BGUI_AllocBitMap();
   extern VOID BGUI_15_BGUI_FreeBitMap();
   extern VOID BGUI_16_BGUI_CreateRPortBitMap();
   extern VOID BGUI_17_BGUI_FreeRPortBitMap();
   extern VOID BGUI_18_BGUI_InfoTextSize();
   extern VOID BGUI_19_BGUI_InfoText();
   extern VOID BGUI_20_BGUI_GetLocaleStr();
   extern VOID BGUI_21_BGUI_GetCatalogStr();
   extern VOID BGUI_22_BGUI_FillRectPattern();
   extern VOID BGUI_23_BGUI_PostRender();
   extern VOID BGUI_24_BGUI_MakeClassA();
   extern VOID BGUI_25_BGUI_FreeClass();
   extern VOID BGUI_26_BGUI_PackStructureTags();
   extern VOID BGUI_27_BGUI_UnpackStructureTags();
   /*
    * Private routines.
    */
   extern VOID BGUI_28_BGUI_GetDefaultTags();
   extern VOID BGUI_29_BGUI_DefaultPrefs();
   extern VOID BGUI_30_BGUI_LoadPrefs();
   extern VOID BGUI_31_BGUI_AllocPoolMemDebug();
   extern VOID BGUI_32_BGUI_FreePoolMemDebug();
#endif

/*
 * Library function table.
 */
STATIC const IPTR Vectors[] = {
#ifdef __AROS__
   /*
    * System interface.
    */
   (IPTR)BGUI_1_LibOpen,
   (IPTR)BGUI_2_LibClose,
   (IPTR)BGUI_3_LibExpunge,
   (IPTR)BGUI_4_LibVoid,
   /*
    * Public routines.
    */
   (IPTR)BGUI_5_BGUI_GetClassPtr,
   (IPTR)BGUI_6_BGUI_NewObjectA,
   (IPTR)BGUI_7_BGUI_RequestA,
   (IPTR)BGUI_8_BGUI_Help,
   (IPTR)BGUI_9_BGUI_LockWindow,
   (IPTR)BGUI_10_BGUI_UnlockWindow,
   (IPTR)BGUI_11_BGUI_DoGadgetMethodA,
   /*
    * Private routines.
    */
   (IPTR)BGUI_12_BGUI_AllocPoolMem,
   (IPTR)BGUI_13_BGUI_FreePoolMem,
   /*
    * Public routines.
    */
   (IPTR)BGUI_14_BGUI_AllocBitMap,
   (IPTR)BGUI_15_BGUI_FreeBitMap,
   (IPTR)BGUI_16_BGUI_CreateRPortBitMap,
   (IPTR)BGUI_17_BGUI_FreeRPortBitMap,
   (IPTR)BGUI_18_BGUI_InfoTextSize,
   (IPTR)BGUI_19_BGUI_InfoText,
   (IPTR)BGUI_20_BGUI_GetLocaleStr,
   (IPTR)BGUI_21_BGUI_GetCatalogStr,
   (IPTR)BGUI_22_BGUI_FillRectPattern,
   (IPTR)BGUI_23_BGUI_PostRender,
   (IPTR)BGUI_24_BGUI_MakeClassA,
   (IPTR)BGUI_25_BGUI_FreeClass,
   (IPTR)BGUI_26_BGUI_PackStructureTags,
   (IPTR)BGUI_27_BGUI_UnpackStructureTags,
   /*
    * Private routines.
    */
   (IPTR)BGUI_28_BGUI_GetDefaultTags,
   (IPTR)BGUI_29_BGUI_DefaultPrefs,
   (IPTR)BGUI_30_BGUI_LoadPrefs,
   (IPTR)BGUI_31_BGUI_AllocPoolMemDebug,
   (IPTR)BGUI_32_BGUI_FreePoolMemDebug,
#else
   /*
    * System interface.
    */
   (IPTR)LibOpen,
   (IPTR)LibClose,
   (IPTR)LibExpunge,
   (IPTR)LibVoid,
   /*
    * Public routines.
    */

   (IPTR)BGUI_GetClassPtr,
   (IPTR)BGUI_NewObjectA,
   (IPTR)BGUI_RequestA,
   (IPTR)BGUI_Help,
   (IPTR)BGUI_LockWindow,
   (IPTR)BGUI_UnlockWindow,
   (IPTR)BGUI_DoGadgetMethodA,
   /*
    * Private routines.
    */
   (IPTR)BGUI_AllocPoolMem,
   (IPTR)BGUI_FreePoolMem,
   /*
    * Public routines.
    */
   (IPTR)BGUI_AllocBitMap,
   (IPTR)BGUI_FreeBitMap,
   (IPTR)BGUI_CreateRPortBitMap,
   (IPTR)BGUI_FreeRPortBitMap,
   (IPTR)BGUI_InfoTextSize,
   (IPTR)BGUI_InfoText,
   (IPTR)BGUI_GetLocaleStr,
   (IPTR)BGUI_GetCatalogStr,
   (IPTR)BGUI_FillRectPattern,
   (IPTR)BGUI_PostRender,
   (LONG)BGUI_MakeClassA,
   (IPTR)BGUI_FreeClass,
   (IPTR)BGUI_PackStructureTags,
   (IPTR)BGUI_UnpackStructureTags,
   /*
    * Private routines.
    */
   (IPTR)BGUI_GetDefaultTags,
   (IPTR)BGUI_DefaultPrefs,
   (IPTR)BGUI_LoadPrefs,
   (IPTR)BGUI_AllocPoolMemDebug,
   (IPTR)BGUI_FreePoolMemDebug,
#endif
   /*
    * Table end marker.
    */
   (IPTR)~0L
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
      if ( (Catalog = OpenCatalog( NULL, "BGUI.catalog", OC_BuiltInLanguage, "english", OC_Version, 2, TAG_END ))) {
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
      if ((BGUIBase = lib = MakeLibrary((APTR)Vectors, NULL, NULL, sizeof(struct Library), NULL)))
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
      return BNULL;

   /*
    * Of course we do not expunge when we still have accessors.
    */
   if (lib->lib_OpenCnt && --lib->lib_OpenCnt)
      return BNULL;

   /*
    * Delayed expunge pending?
    */
   if (lib->lib_Flags & LIBF_DELEXP)
#ifdef __AROS__
      REGCALL2(BPTR, BGUI_3_LibExpunge,
                REGPARAM(D0, struct Library *, lib),
                REGPARAM(A6, struct ExecBase *, SysBase));
#else
      return LibExpunge(lib);
#endif
   /*
    * Otherwise we remain in memory.
    */
   return BNULL;
    
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
      return BNULL;
   };

   /*
    * Cleanup locale stuff.
    */
   if (Catalog)
      CloseCatalog(Catalog);

   FreeInputStack();

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
