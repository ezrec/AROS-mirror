/*
 * BGUICLASS_LIB.C
 *
 * (C) Copyright 1996 Ian J. Einman.
 *     All Rights Reserved.
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include <stddef.h>
#include <stdlib.h>

#include <libraries/bgui.h>

/*
 * Compiler specific stuff.
 */
#ifdef _DCC
#define SAVEDS    __geta4
#define ASM
#define REG(x)    __ ## x
#else
#define SAVEDS    __saveds
#define ASM       __asm
#define REG(x)    register __ ## x
#endif

/*
 * Global data (written to once at initalization time).
 */
static BPTR           SegList = 0;

struct Library       *BGUIBase      = NULL;
struct DosLibrary    *DOSBase       = NULL;
struct ExecBase      *SysBase       = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase       *GfxBase       = NULL;
struct Library       *UtilityBase   = NULL;
struct Library       *LayersBase    = NULL;

/*
 * Prototypes of class functions.
 */
extern ASM Class *BGUI_ClassInit( void );
extern ASM BOOL BGUI_ClassFree( void );

extern UBYTE _LibID[];         /* ID string              */
extern UBYTE _LibName[];       /* Name string            */
extern UWORD _LibVersion;      /* Version of library     */
extern UWORD _LibRevision;     /* Revision of library    */

SAVEDS ASM struct Library *LibInit(REG(d0) struct Library *lib, REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib);
SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver);
SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib);
SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib);
SAVEDS LONG LibVoid(void);

/*
 * Library function table.
 */
static const LONG Vectors[] = {
   /*
    * System interface.
    */
   (LONG)LibOpen,
   (LONG)LibClose,
   (LONG)LibExpunge,
   (LONG)LibVoid,
   /*
    * Table end marker.
    */
   -1
};

/*
 * Close opened libraries.
 */
STATIC VOID CloseLibs(void)
{
   if (BGUIBase)      CloseLibrary(BGUIBase);
   if (LayersBase)    CloseLibrary(LayersBase);
   if (UtilityBase)   CloseLibrary(UtilityBase);
   if (GfxBase)       CloseLibrary((struct Library *)GfxBase);
   if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
   if (DOSBase)       CloseLibrary((struct Library *)DOSBase);
}

ULONG _LibInit[4] =
{
   (ULONG)sizeof(struct BGUIClassBase),
   (ULONG)Vectors,
   (ULONG)NULL,
   (ULONG)LibInit
};

#define bcb ((struct BGUIClassBase *)lib)
/*
 * Library initialization.
 */
SAVEDS ASM struct Library *LibInit(REG(d0) struct Library *lib, REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib)
{
   /*
    * Globally assign SysBase and SegList.
    */
   SysBase = syslib;
   SegList = segment;

   /*
    * Open up system libraries.
    */
   DOSBase        = (struct DosLibrary *)   OpenLibrary("dos.library",       37);
   IntuitionBase  = (struct IntuitionBase *)OpenLibrary("intuition.library", 37);
   GfxBase        = (struct GfxBase *)      OpenLibrary("graphics.library",  37);
   UtilityBase    = OpenLibrary("utility.library",       37);
   LayersBase     = OpenLibrary("layers.library",        37);
   BGUIBase       = OpenLibrary("bgui.library",          41);

   /*
    * All libraries open?
    */
   if (DOSBase && IntuitionBase && GfxBase && UtilityBase && LayersBase && BGUIBase)
   {
      if (bcb->bcb_Class = BGUI_ClassInit())
      {
         /*
          * Initialize library structure.
          */
         lib->lib_Node.ln_Type = NT_LIBRARY;
         lib->lib_Node.ln_Name = (UBYTE *)_LibName;
         lib->lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
         lib->lib_Version      = _LibVersion;
         lib->lib_Revision     = _LibRevision;
         lib->lib_IdString     = (APTR)(_LibID + 6);

         CloseLibrary(BGUIBase);
         BGUIBase=NULL;
         return lib;
      };
   };
   CloseLibs();
   return NULL;
}

/*
 * Open library.
 */
SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver)
{
   /*
    * Increase open counter when necessary.
    */
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

/*
 * Close library.
 */
SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib)
{
   /*
    * Of course we do not expunge
    * when we still have accessors.
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
SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib)
{
   /*
    * No expunge when we still
    * have accessors.
    */
   if (lib->lib_OpenCnt)
   {
      /*
       * Set delayed expunge flag.
       */
      lib->lib_Flags |= LIBF_DELEXP;
      return NULL;
   }

   /*
    * Free the class.
    */
   if (bcb->bcb_Class
   && (BGUIBase
   || (BGUIBase=OpenLibrary("bgui.library",41))))
      BGUI_ClassFree();
   
   /*
    * Close system libraries.
    */
   CloseLibs();

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
SAVEDS LONG LibVoid(void)
{
   return 0;
}
