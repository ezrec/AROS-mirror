
// 17:45:13, 20 Maj 1999
//  add #define MCC_USES_IFFPARSE to autoinit iffparse.library

// 22:59:21, 17 Grudzieï 1998
//  add #define MCC_USES_LOCALE to autoinit locale.library

/// README
/*****************************************************************************

This code serves as a basis for writing a library-based MUI custom class
(xyz.mcc) and its preferences editor (xyz.mcp).

You need to define a few things in your main source file, then include
this file and then continue with your classes methods. The order is
important, mccheader.c must be the first code-generating module.

Things to be defined before mccheader.c is included:

(1)  UserLibID     - version string for your class. must follow normal $VER: string conventions.
(2)  VERSION       - version number of the class. must match the one given in the $VER: string.
(3)  REVISION      - revision number of the class. must match the one given in the $VER: string.
(4)  CLASS         - Name of your class, ".mcc" or ".mcp" must always be appended.

(5)  SUPERCLASS    - superclass of your class.
(6)  struct Data   - instance data structure.
(7)  _Dispatcher   - your dispatcher function.

(8)  SUPERCLASSP   - Superclass of the preferences class, must be MUIC_Mccprefs or a subclass.
(9)  struct DataP  - instance data structure of preferences class.
(10) _DispatcherP  - dispatcher for the preferences class.

Items (1) to (4) must always be defined. If you create a stand-alone
custom class (*.mcc) without preferences editor, also define (5), (6)
and (7). Name your class and the resulting ouptut file "Myclass.mcc".

If you create a preferences class (*.mcp) for a custom class, define
(8), (9) and (10) instead of (5), (6) and (7). Name your class and the
resulting output file "Myclass.mcp".

If you create a custom class with included preferences editor, define
all the above. Note that in this case, the name of your class and the
resulting output file is always "Myclass.mcc". MUI will automatically
recognize that there's also a "Myclass.mcp" included. Having a builtin
preferences class reduces the need for a second file but increases the
size and memory consuption of the class.

If your class needs custom initialization (e.g. opening other
libraries), you can define
        ClassInit
        ClassExit
to point to custom functions. These functions need to have the prototypes
        BOOL ClassInitFunc(struct Library *base);
        VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

This code automatically defines and initializes the following variables:
        struct Library *MUIMasterBase;
        struct Library *SysBase;
        struct Library *UtilityBase;
        struct Library *DOSBase;
        struct Library *GfxBase;
        struct Library *IntuitionBase;
        struct Library *MUIClassBase;       /* your classes library base */
        struct MUI_CustomClass *ThisClass;  /* your custom class */
        struct MUI_CustomClass *ThisClassP; /* your preferences class */

Example: Myclass.c
        #define CLASS      MUIC_Myclass // name of class, e.g. "Myclass.mcc"
        #define SUPERCLASS MUIC_Area    // name of superclass
        struct Data
        {
                LONG          MyData;
                struct Foobar MyData2;
                // ...
        };
        #define UserLibID "$VER: Myclass.mcc 17.53 (11.11.96)"
        #define VERSION   17
        #define REVISION  53
        #include "mccheader.c"
        ULONG ASM SAVEDS _Dispatcher(REG(a0) struct IClass *cl,
                                         REG(a2) Object *obj,
                                         REG(a1) Msg msg)
        {
                // ...
        }

Compiling and linking with SAS-C can look like this:
        Myclass.mcc: Myclass.c
                sc $(CFLAGS) $*.c OBJNAME $*.o
                slink to $@ from $*.o lib $(LINKERLIBS) $(LINKERFLAGS)

Note well that we don't use SAS library creation feature here, it simply
sucks too much. It's not much more complicated to do the library
initialziation ourselves and we have better control over everything.

Make sure to read the whole source to get some interesting comments
and some understanding on how libraries are created!

*****************************************************************************/
//|

/// Includes

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <clib/exec_protos.h>
#include <pragmas/exec_sysbase_pragmas.h>

#ifdef MCC_USES_ASL
#include <clib/asl_protos.h>
#endif

#ifdef MCC_USES_LOCALE
#include <clib/locale_protos.h>
#endif


#include "debug.h"

//|

/* The name of the class will also become the name of the library. */
/* We need a pointer to this string in our ROMTag (see below). */

static const char UserLibName[] = CLASS;

/// Global Data
/* Here's our global data, described above. */

struct Library *MUIClassBase;
struct Library *MUIMasterBase;
extern struct Library *SysBase;
struct Library *UtilityBase;
struct Library *DOSBase;
struct GfxBase *GfxBase;
struct Library *IntuitionBase;

#ifdef MCC_USES_LOCALE
struct Library *LocaleBase;
struct Locale  *Locale;
#endif

#ifdef MCC_USES_IFFPARSE
struct Library *IFFParseBase;
#endif

#ifdef MCC_USES_DATATYPES
struct Library *DataTypesBase;
#endif

#ifdef MCC_USES_ASL
struct Library *AslBase;
#endif

//|
/// Few #defines etc
#ifdef SUPERCLASS
struct MUI_CustomClass *ThisClass;
#endif

#ifdef SUPERCLASSP
struct MUI_CustomClass *ThisClassP;
#endif

#ifdef _DCC
#define REG(x) __ ## x
#define ASM
#define SAVEDS __geta4
#else
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#endif
//|
/// Library Header
/* Our library structure, consisting of a struct Library, a segment pointer */
/* and a semaphore. We need the semaphore to protect init/exit stuff in our */
/* open/close functions */

struct LibraryHeader
{
        struct Library         lh_Library;
        BPTR                   lh_Segment;
        struct SignalSemaphore lh_Semaphore;
};
//|

/// Prototypes for all library functions

struct LibraryHeader * ASM LibInit   (REG(a0) BPTR Segment);
BPTR                   ASM LibExpunge(REG(a6) struct LibraryHeader *base);
struct LibraryHeader * ASM LibOpen   (REG(a6) struct LibraryHeader *base);
BPTR                   ASM LibClose  (REG(a6) struct LibraryHeader *base);
LONG                   ASM LibNull   (VOID);
ULONG           SAVEDS ASM MCC_Query (REG(d0) LONG which);
//|
/// Prototypes for a few sub-functions

BOOL ASM UserLibInit   (REG(a6) struct Library *base);
BOOL ASM UserLibExpunge(REG(a6) struct Library *base);
BOOL ASM UserLibOpen   (REG(a6) struct Library *base);
BOOL ASM UserLibClose  (REG(a6) struct Library *base);
//|
/// This is the librarie's jump table
static const APTR LibVectors[] =
{
        LibOpen,
        LibClose,
        LibExpunge,
        LibNull,
        MCC_Query,
        (APTR)-1
};
//|
/// Dummy entry point and LibNull() function all in one

LONG ASM LibNull(VOID)
{
        return(NULL);
}
//|
/// Here's what makes us a library

static const struct Resident ROMTag =
{
        RTC_MATCHWORD,
        &ROMTag,
        &ROMTag + 1,
        0,
        VERSION,
        NT_LIBRARY,
        0,
        UserLibName,
        UserLibID,
        LibInit
};
//|

/******************************************************************************/
/* Standard Library Functions, all of them are called in Forbid() state.      */
/******************************************************************************/

/// LibInit
struct LibraryHeader * ASM SAVEDS LibInit(REG(a0) BPTR Segment)
{
struct LibraryHeader *base;

        D(bug(__NAME ": LibInit\n"));

        SysBase = *((struct Library **)4);

                #ifdef _M68040
                if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68040)) return(NULL);
                #endif
                #ifdef _M68030
                if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68030)) return(NULL);
                #endif
                #ifdef _M68020
                if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68020)) return(NULL);
                #endif

        if (base = (struct LibraryHeader *)MakeLibrary((APTR)LibVectors,NULL,NULL,sizeof(struct LibraryHeader),NULL))
                {
                base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
                base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
                base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
                base->lh_Library.lib_Version      = VERSION;
                base->lh_Library.lib_Revision     = REVISION;
                base->lh_Library.lib_IdString     = (char *)UserLibID;

                base->lh_Segment  = Segment;

                InitSemaphore(&base->lh_Semaphore);

                AddLibrary((struct Library *)base);
                }
        else
                {
//   D(DBF_LIB,bug("\7MakeLibrary() failed\n"));
                }

        return(base);
}
//|
/// LibExpunge

BPTR ASM SAVEDS LibExpunge(REG(a6) struct LibraryHeader *base)
{
struct Library *SysBase = *((struct Library **)4);
BPTR rc;

        D(bug(__NAME ": Expunge: OpenCount = %ld\n",base->lh_Library.lib_OpenCnt));

        if (base->lh_Library.lib_OpenCnt)
        {
                base->lh_Library.lib_Flags |= LIBF_DELEXP;
                D(bug(__NAME ": Setting LIBF_DELEXP\n"));
                return(NULL);
        }

        Remove((struct Node *)base);
        rc = base->lh_Segment;
        FreeMem((BYTE *)base - base->lh_Library.lib_NegSize,base->lh_Library.lib_NegSize + base->lh_Library.lib_PosSize);

        return(rc);
}
//|
/// LibOpen
struct LibraryHeader * ASM SAVEDS LibOpen(REG(a6) struct LibraryHeader *base)
{
struct Library *SysBase = *((struct Library **)4);
struct LibraryHeader *rc;

        base->lh_Library.lib_OpenCnt++;
        base->lh_Library.lib_Flags &= ~LIBF_DELEXP;

        D(bug(__NAME ": LibOpen (OC = %ld)\n", base->lh_Library.lib_OpenCnt));

        ObtainSemaphore(&base->lh_Semaphore);

        if (UserLibOpen((struct Library *)base))
        {
                rc = base;
        }
        else
        {
                rc = NULL;
                base->lh_Library.lib_OpenCnt--;
                D(bug(__NAME ": \7UserLibOpen() failed\n"));
        }

        ReleaseSemaphore(&base->lh_Semaphore);

        return(rc);
}
//|
/// LibClose

BPTR ASM SAVEDS LibClose(REG(a6) struct LibraryHeader *base)
{
        struct Library *SysBase = *((struct Library **)4);
        BPTR rc = NULL;

        D(bug(__NAME ": LibClose (OC = %ld) %s\n", base->lh_Library.lib_OpenCnt,base->lh_Library.lib_OpenCnt==0 ? "\7ERROR" : ""));

        ObtainSemaphore(&base->lh_Semaphore);
        UserLibClose((struct Library *)base);
        ReleaseSemaphore(&base->lh_Semaphore);

        if (--base->lh_Library.lib_OpenCnt == 0)
                {
                if (base->lh_Library.lib_Flags & LIBF_DELEXP)
                        {
                        rc = LibExpunge(base);
                        }
                }

        return(rc);
}
//|

/// UserLibOpen

BOOL ASM SAVEDS UserLibOpen(REG(a6) struct Library *base)
{
        ULONG ASM _Dispatcher(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg);
        ULONG ASM _DispatcherP(REG(a0) struct IClass *cl, REG(a2) Object *obj, REG(a1) Msg msg);
        BOOL ClassInitFunc(struct Library *base);

        D(bug(__NAME ": UserLibOpen (OC: %ld)\n",base->lib_OpenCnt));

        if (base->lib_OpenCnt!=1)
                return(TRUE);

        #ifndef MASTERVERSION
        #define MASTERVERSION MUIMASTER_VMIN
        #endif

        if (MUIMasterBase = OpenLibrary("muimaster.library",MASTERVERSION))
        {
                #ifdef SUPERCLASS
                if (ThisClass = MUI_CreateCustomClass(base,SUPERCLASS,NULL,sizeof(struct Data),_Dispatcher))
                #endif
                {
                        #ifdef SUPERCLASSP
                        if (ThisClassP = MUI_CreateCustomClass(base,SUPERCLASSP,NULL,sizeof(struct DataP),_DispatcherP))
                        #endif
                        {
                                #ifdef SUPERCLASS
                                #define THISCLASS ThisClass
                                #else
                                #define THISCLASS ThisClassP
                                #endif

                                UtilityBase   = THISCLASS->mcc_UtilityBase;
                                DOSBase       = (struct DosLibrary *)THISCLASS->mcc_DOSBase;
                                GfxBase       = (struct GfxBase *)THISCLASS->mcc_GfxBase;
                                IntuitionBase = THISCLASS->mcc_IntuitionBase;

                                #ifdef MCC_USES_DATATYPES
                                if(!(DataTypesBase = OpenLibrary("datatypes.library", 38)))
                                        return(FALSE);
                                #endif

                                #ifdef MCC_USES_LOCALE
                                if(((LocaleBase = OpenLibrary("locale.library", 38)) == NULL))
                                       return(FALSE);
                                Locale  = OpenLocale(NULL);
                                #endif               

                                #ifdef MCC_USES_IFFPARSE
                                if(((IFFParseBase = OpenLibrary("iffparse.library", 37)) == NULL))
                                       return(FALSE);
                                #endif

                                #ifdef MCC_USES_ASL
                                if((( AslBase = OpenLibrary("asl.library", 37) ) == NULL))
                                       return(FALSE);
                                #endif


                                #ifndef ClassInit
                                return(TRUE);
                                #else
                                if (ClassInitFunc(base))
                                {
                                        return(TRUE);
                                }

                                #ifdef SUPERCLASSP
                                MUI_DeleteCustomClass(ThisClassP);
                                ThisClassP = NULL;
                                #endif

                                #endif
                        }
                        #ifdef SUPERCLASSP
                        #ifdef SUPERCLASS
                        MUI_DeleteCustomClass(ThisClass);
                        ThisClass = NULL;
                        #endif
                        #endif
                }
                CloseLibrary(MUIMasterBase);
                MUIMasterBase = NULL;
        }
        D(bug(__NAME ": muimaster fail.: %08lx %s\n", base, base->lib_Node.ln_Name));

        return(FALSE);
}
//|
/// UserLibClose
BOOL ASM SAVEDS UserLibClose(REG(a6) struct Library *base)
{
void ClassExitFunc(struct Library *base);

        D(bug(__NAME ": UserLibClose (OC = %ld)\n", base->lib_OpenCnt));

        if (base->lib_OpenCnt==1)
        {
                #ifdef ClassExit
                ClassExitFunc(base);
                #endif

                #ifdef SUPERCLASSP
                if (ThisClassP)
                {
                        MUI_DeleteCustomClass(ThisClassP);
                        ThisClassP = NULL;
                }
                #endif

                #ifdef SUPERCLASS
                if (ThisClass)
                {
                        MUI_DeleteCustomClass(ThisClass);
                        ThisClass = NULL;
                }
                #endif

                #ifdef MCC_USES_IFFPARSE
                if(IFFParseBase)
                   {
                   CloseLibrary(IFFParseBase);
                   }
                #endif

                #ifdef MCC_USES_LOCALE
                if(LocaleBase)
                   {
                   if(Locale)
                       CloseLocale(Locale);

                   CloseLibrary(LocaleBase);
                   }
                #endif 

                #ifdef MCC_USES_DATATYPES
                if (DataTypesBase)
                        {
                        CloseLibrary(DataTypesBase);
                        DataTypesBase = NULL;
                        }
                #endif

                #ifdef MCC_USES_ASL
                if( AslBase )
                   {
                   CloseLibrary( AslBase );
                   AslBase = NULL;
                   }
                #endif


                if (MUIMasterBase)
                {
                        CloseLibrary(MUIMasterBase);
                        MUIMasterBase = NULL;
                }

        }

        return(TRUE);
}
//|
/// MCC_Query
ULONG SAVEDS ASM MCC_Query(REG(d0) LONG which)
{
        switch (which)
        {
                #ifdef SUPERCLASS
                case 0: return((ULONG)ThisClass);
                #endif

                #ifdef SUPERCLASSP
                case 1: return((ULONG)ThisClassP);
                #endif

                #ifdef PREFSIMAGEOBJECT
                case 2:
                {
                        Object *obj = PREFSIMAGEOBJECT;
                        return((ULONG)obj);
                }
                #endif

                #ifdef ONLYGLOBAL
                case 3:
                {
                        return(TRUE);
                }
                #endif
        }
        return(NULL);
}
//|

/// DoSuperNew()

ULONG __stdargs DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
    return(DoSuperMethod(cl, obj, OM_NEW, &tag1, NULL));
}
//|

/*
/// main()
long main()
{
BPTR  Handle;
char  Warning[] = "This custom class is for use with MUI 3.5+ only\n";

        if(Handle = Output())
                {
                Write(Handle, Warning, strlen(Warning));
                }
        return(20);
}
//|
*/
