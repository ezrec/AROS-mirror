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
#ifdef _AROS
#else
#ifdef _DCC
#define SAVEDS    __geta4
#define ASM
#define REG(x)    __ ## x
#else
#define SAVEDS    __saveds
#define ASM       __asm
#define REG(x)    register __ ## x
#endif
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



#ifdef _AROS

struct Library * LibInit();
AROS_LD1(struct Library *, LibOpen,
    AROS_LHA(ULONG, version, D0),
    struct Library *, lib, 1, BGUIGadget);

AROS_LD0(BPTR, LibClose,
         struct Library *, lib, 2, BGUIGadget);

AROS_LD1(BPTR, LibExpunge,
    AROS_LHA(struct Library *, lib, D0),
    struct ExecBase *, sysBase, 3, BGUIGadget);

AROS_LD0(LONG, LibVoid,
     struct Library *, lib, 4, BGUIGadget);
#else
SAVEDS ASM struct Library *LibInit(REG(d0) struct Library *lib, REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib);
SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver);
SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib);
SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib);
SAVEDS LONG LibVoid(void);
#endif

/*
 * Library function table.
 */
static const LONG Vectors[] = {
   /*
    * System interface.
    */
#ifdef _AROS
   (LONG)BGUIGadget_LibOpen,
   (LONG)BGUIGadget_LibClose,
   (LONG)BGUIGadget_LibExpunge,
   (LONG)BGUIGadget_LibVoid,
#else
   (LONG)LibOpen,
   (LONG)LibClose,
   (LONG)LibExpunge,
   (LONG)LibVoid,
#endif
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
#ifdef _AROS
AROS_UFH3(struct Library *, LibInit,
	AROS_LHA(struct Library *, lib, D0),
	AROS_LHA(BPTR, segment, A0),
	AROS_LHA(struct ExecBase *, syslib, A6))
#else
SAVEDS ASM struct Library *LibInit(REG(d0) struct Library *lib, REG(a0) BPTR segment, REG(a6) struct ExecBase *syslib)
#endif
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
#ifdef _AROS
AROS_LH1(struct Library *, LibOpen,
    AROS_LHA(ULONG, version, D0),
    struct Library *, lib, 1, BGUIGadget)
#else
SAVEDS ASM struct Library *LibOpen(REG(a6) struct Library *lib, REG(d0) ULONG libver)
#endif
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
#ifdef _AROS
AROS_LH0(BPTR, LibClose,
         struct Library *, lib, 2, BGUIGadget)
#else
SAVEDS ASM BPTR LibClose(REG(a6) struct Library *lib)
#endif
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
#ifdef _AROS
      return AROS_UFC2(BPTR, BGUIGadget_LibExpunge,
      		AROS_UFCA(struct Library *, lib, D0),
      		AROS_UFCA(struct ExecBase *, SysBase, A6));
#else
      return LibExpunge(lib);
#endif

   /*
    * Otherwise we remain in memory.
    */
   return NULL;
}

/*
 * Expunge library.
 */
#ifdef _AROS
AROS_LH1(BPTR, LibExpunge,
    AROS_LHA(struct Library *, lib, D0),
    struct ExecBase *, sysBase, 3, BGUIGadget)
#else
SAVEDS ASM BPTR LibExpunge(REG(a6) struct Library *lib)
#endif
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
#ifdef _AROS
AROS_LH0(LONG, LibVoid,
     struct Library *, lib, 4, BGUIGadget)
#else
SAVEDS LONG LibVoid(void)
#endif
{
   return 0;
}
