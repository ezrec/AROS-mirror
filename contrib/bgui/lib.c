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

/*
 * Library function table.
 */
STATIC const LONG Vectors[] = {
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
makeproto SAVEDS ASM struct Library *LibInit(REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib)
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

/*
 * Open library.
 */
makeproto SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver)
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
}

/*
 * Close library.
 */
makeproto SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib)
{
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
      return LibExpunge(lib);

   /*
    * Otherwise we remain in memory.
    */
   return NULL;
}

/*
 * Expunge library.
 */
makeproto SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib)
{
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
}

/*
 * Reserved routine.
 */
makeproto SAVEDS LONG LibVoid(void)
{
   return 0;
}
