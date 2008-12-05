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

(11) USEDCLASSES   - Other custom classes required by this class (NULL terminated string array)
(12) USEDCLASSESP  - Preferences classes (.mcp) required by this class (NULL terminated string array)
(13) SHORTHELP     - .mcp help text for prefs program's listview.

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

(11) If your class requires other mcc custom classes, list them in the
static array USEDCLASSES like this: 
const STRPTR USEDCLASSES[] = { "Busy.mcc", "Listtree.mcc", NULL };

(12) If your class has one (or more) preferences classes, list them in
the array USEDCLASSESP like this:
const STRPTR USEDCLASSESP[] = { "Myclass.mcp", "Popxxx.mcp", NULL };

(13) If you want MUI to display additional help text (besides name, 
version and copyright) when the mouse pointer is over your mcp entry 
in the prefs listview:
#define SHORTHELP "ANSI display for terminal programs."

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

---
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
	PreClassInit
	PostClassExit
	ClassInit
	ClassExit
to point to custom functions. These functions need to have the prototypes
	BOOL ClassInitFunc(struct Library *base);
	VOID ClassExitFunc(struct Library *base);
and will be called right after the class has been created and right
before the class is being deleted. If your init func returns FALSE,
the custom class will be unloaded immediately.

	BOOL PreClassInitFunc(void);
	VOID PostClassExitFunc(void);

These functions will be called BEFORE the class is created and AFTER the
class is deleted, if something depends on it for example. MUIMasterBase
is open then.

Define the minimum version of muimaster.libray in MASTERVERSION. If you
don't define MASTERVERSION, it will default to MUIMASTER_VMIN from the
mui.h include file.

This code automatically defines and initializes the following variables:
	struct Library *MUIMasterBase;
	struct Library *SysBase;
	struct Library *UtilityBase;
	struct DosLibrary *DOSBase;
	struct GfxBase *GfxBase;
	struct IntuitionBase *IntuitionBase;
	struct Library *MUIClassBase;       // your classes library base
	struct MUI_CustomClass *ThisClass;  // your custom class
	struct MUI_CustomClass *ThisClassP; // your preferences class

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
	ULONG ASM SAVEDS _Dispatcher(REG(a0) struct IClass *cl GNUCREG(a0),
	                             REG(a2) Object *obj GNUCREG(a2),
	                             REG(a1) Msg msg GNUCREG(a1) )
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

/* MorphOS relevant includes... */
#ifdef MORPHOS
#include <emul/emulinterface.h>
#include <emul/emulregs.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif


/* a few other includes... */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/semaphores.h>
#include <exec/resident.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <proto/exec.h>

#ifdef SAVEDS
#undef SAVEDS
#endif

#ifdef __SASC

#	define REG( r )				register __ ## r
#	define GNUCREG( r )
#	define SAVEDS				__saveds
#	define ASM					__asm
#	define REGARGS				__regargs
#	define STDARGS				__stdargs
#	define STACKARGS			__stdargs
#	define ALIGNED				__aligned
#	define FAR					__far
#	define INLINE				__inline

#else
#	ifdef __GNUC__
#    ifdef MORPHOS
#		define REG( r )
#		define GNUCREG( r )
#		define SAVEDS
#		define ASM
#		define REGARGS			__regargs
#		define STDARGS			__stdargs
#		define SACKARGS			__stdargs
#		define ALIGNED			__aligned
#		define FAR				__far
#		define INLINE			__inline
#		define NO_PRAGMAS
#		define USE_GLUE
#    else

#		define REG( r )
#		define GNUCREG( r )		__asm( #r )
#		define SAVEDS			__saveds
#		define ASM
#		define REGARGS			__regargs
#		define STDARGS			__stdargs
#		define STACKARGS		__stdargs
#		define ALIGNED			__aligned
#		define FAR				__far
#		define INLINE
#		define NO_PRAGMAS
#    endif

#	else /* Any other compiler. */

#		define REG( r )
#		define GNUCREG( r )
#		define SAVEDS
#		define ASM
#		define REGARGS
#		define STDARGS
#		define SACKARGS
#		define ALIGNED
#		define FAR
#		define INLINE

#	endif /* __GNUC__ */
#endif /* __SASC */




/* The name of the class will also become the name of the library. */
/* We need a pointer to this string in our ROMTag (see below). */

static const char UserLibName[] = CLASS;


/* Here's our global data, described above. */

struct Library *MUIClassBase;
struct Library *MUIMasterBase;
struct ExecBase *SysBase;
struct Library *UtilityBase;
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

#ifdef SUPERCLASS
struct MUI_CustomClass *ThisClass;
#endif

#ifdef SUPERCLASSP
struct MUI_CustomClass *ThisClassP;
#endif

#ifdef __SASC

//ULONG XCEXIT       = NULL; /* these symbols may be referenced by    */
//ULONG _XCEXIT      = NULL; /* some functions of sc.lib, but should  */
ULONG ONBREAK      = NULL; /* never be used inside a shared library */
ULONG _ONBREAK     = NULL;
ULONG base         = NULL;
ULONG _base        = NULL;
ULONG ProgramName  = NULL;
ULONG _ProgramName = NULL;
ULONG StackPtr     = NULL;
ULONG _StackPtr    = NULL;
ULONG oserr        = NULL;
ULONG _oserr       = NULL;
ULONG OSERR        = NULL;
ULONG _OSERR       = NULL;

void __regargs __chkabort(void) {}	/* a shared library cannot be    */
void __regargs _CXBRK(void)     {}	/* CTRL-C aborted when doing I/O */

#else
 #ifdef __GNUC__
  /* these one are needed copies for libnix.a */
  struct DosLibrary *__DOSBase = NULL;
  struct UtilityBase *__UtilityBase = NULL;

  #ifndef MORPHOS
   #ifndef _STDLIB_H_
    void __regargs exit(void) {}
   #endif
   LONG _WBenchMsg = 0;
  #endif
 #endif /* __GNUC__ */

#endif /* __SASC */

/* Our library structure, consisting of a struct Library, a segment pointer */
/* and a semaphore. We need the semaphore to protect init/exit stuff in our */
/* open/close functions */

struct LibraryHeader
{
	struct Library         lh_Library;
	BPTR                   lh_Segment;
	struct SignalSemaphore lh_Semaphore;
	struct StackSwapStruct *lh_Stack;
};


/* Prototypes for all library functions */

#ifdef MORPHOS
struct LibraryHeader * LibInit   (struct Library *lib, APTR segment, struct ExecBase *SysBase);
APTR                   LibExpunge(void);
struct LibraryHeader * LibOpen   (void);
APTR                   LibClose  (void);
LONG                   LibNull   (void);
ULONG                  MCC_Query (void);
#else
struct LibraryHeader * ASM LibInit   (REG(a0) BPTR Segment GNUCREG(a0));
BPTR                   ASM LibExpunge(REG(a6) struct LibraryHeader *base GNUCREG(a6));
struct LibraryHeader * ASM LibOpen   (REG(a6) struct LibraryHeader *base GNUCREG(a6));
BPTR                   ASM LibClose  (REG(a6) struct LibraryHeader *base GNUCREG(a6));
LONG                   ASM LibNull   (VOID);
ULONG           SAVEDS ASM MCC_Query (REG(d0) LONG which GNUCREG(d0));
#endif


/* Prototypes for a few sub-functions */

#ifdef MORPHOS
BOOL UserLibInit   (void);
BOOL UserLibExpunge(void);
BOOL UserLibOpen   (void);
BOOL UserLibClose  (void);
#else
BOOL ASM UserLibInit   (REG(a6) struct Library *base GNUCREG(a6));
BOOL ASM UserLibExpunge(REG(a6) struct Library *base GNUCREG(a6));
BOOL ASM UserLibOpen   (REG(a6) struct Library *base GNUCREG(a6));
BOOL ASM UserLibClose  (REG(a6) struct Library *base GNUCREG(a6));
#endif


/* This is the librarie's jump table */

static const APTR LibVectors[] =
{
#ifdef MORPHOS
	(APTR)FUNCARRAY_32BIT_NATIVE,
#endif
	LibOpen,
	LibClose,
	LibExpunge,
	LibNull,
	MCC_Query,
	(APTR)-1
};


#ifdef MORPHOS
unsigned long __amigappc__ = 1;
#endif


/* Dummy entry point and LibNull() function all in one */

LONG ASM LibNull(VOID)
{
	return(NULL);
}


/* Here's what makes us a library. */

static const struct Resident ROMTag =
{
	RTC_MATCHWORD,
	&ROMTag,
	&ROMTag + 1,
#ifdef MORPHOS
	RTF_PPC,
#else
	0,
#endif
	VERSION,
	NT_LIBRARY,
	0,
	UserLibName,
	UserLibID,
	LibInit
};



/******************************************************************************/
/* Standard Library Functions, all of them are called in Forbid() state.      */
/******************************************************************************/

#ifdef MORPHOS
struct LibraryHeader *LibInit( struct Library *lib, APTR Segment, struct ExecBase *sb )
{
#else
struct LibraryHeader * ASM SAVEDS LibInit( REG(a0) BPTR Segment GNUCREG(a0) )
{
	struct ExecBase *sb = *((struct ExecBase **)4);
#endif

	struct LibraryHeader *base;
	static struct StackSwapStruct *stack = NULL;

	SysBase = sb;

	D(bug( "Start...\n" ) );

#if defined( CLASS_STACKSWAP ) && !defined( MORPHOS )
	if ( !( stack = AllocMem( sizeof( struct StackSwapStruct ) + 8192, MEMF_PUBLIC | MEMF_CLEAR ) ) )
		return( NULL );

	stack->stk_Lower	= (APTR)( (ULONG)stack + sizeof( struct StackSwapStruct ) );
	stack->stk_Upper	= (ULONG)( (ULONG)stack->stk_Lower + 8192 );
	stack->stk_Pointer	= (APTR)stack->stk_Upper;

	D(bug( "Before StackSwap()\n" ) );
	StackSwap( stack );
#endif

	#ifdef __SASC
	#ifdef _M68040
	if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68040)) return(NULL);
	#endif
	#ifdef _M68030
	if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68030)) return(NULL);
	#endif
	#ifdef _M68020
	if (!(((struct ExecBase *)SysBase)->AttnFlags & AFF_68020)) return(NULL);
	#endif
	#endif

	if (base = (struct LibraryHeader *)MakeLibrary((APTR)LibVectors,NULL,NULL,sizeof(struct LibraryHeader),NULL))
	{
		D(bug( "After MakeLibrary()\n" ) );

		base->lh_Library.lib_Node.ln_Type = NT_LIBRARY;
		base->lh_Library.lib_Node.ln_Name = (char *)UserLibName;
		base->lh_Library.lib_Flags        = LIBF_CHANGED | LIBF_SUMUSED;
		base->lh_Library.lib_Version      = VERSION;
		base->lh_Library.lib_Revision     = REVISION;
		base->lh_Library.lib_IdString     = (char *)UserLibID;

		base->lh_Segment	= Segment;
		base->lh_Stack		= stack;

#ifndef MORPHOS
		InitSemaphore(&base->lh_Semaphore);
#endif

		D(bug( "AddLibrary()\n") );
		AddLibrary((struct Library *)base);
	}
	else
	{
		D(bug("\7MakeLibrary() failed\n") );
	}

#if defined( CLASS_STACKSWAP ) && !defined( MORPHOS )
	StackSwap( base->lh_Stack );
	FreeMem( base->lh_Stack, sizeof( struct StackSwapStruct ) + 8192 );
	D(bug( "After second StackSwap()\n" ) );
#endif

	return(base);
}


#ifdef MORPHOS
APTR LibExpunge( void )
{
	struct LibraryHeader *base = (void *)REG_A6;
#else
BPTR ASM SAVEDS LibExpunge(REG(a6) struct LibraryHeader *base GNUCREG(a6))
{
	struct Library *SysBase = *((struct Library **)4);
#endif
	BPTR rc;

	D(bug( "OpenCount = %ld\n", base->lh_Library.lib_OpenCnt ) );

	if (base->lh_Library.lib_OpenCnt)
	{
		base->lh_Library.lib_Flags |= LIBF_DELEXP;
		D(bug("Setting LIBF_DELEXP\n") );
		return(NULL);
	}

	Remove((struct Node *)base);

	rc = base->lh_Segment;
	FreeMem((BYTE *)base - base->lh_Library.lib_NegSize,base->lh_Library.lib_NegSize + base->lh_Library.lib_PosSize);

	return(rc);
}


#ifdef MORPHOS
struct LibraryHeader *LibOpen( void )
{
	struct LibraryHeader *base = (void *)REG_A6;
#else
struct LibraryHeader * ASM SAVEDS LibOpen( REG(a6) struct LibraryHeader *base GNUCREG(a6) )
{
	struct Library *SysBase = *((struct Library **)4);
#endif

	struct LibraryHeader *rc;

#ifndef MORPHOS
	ObtainSemaphore(&base->lh_Semaphore);
#endif

	base->lh_Library.lib_OpenCnt++;
	base->lh_Library.lib_Flags &= ~LIBF_DELEXP;

	D(bug( "OpenCount = %ld\n", base->lh_Library.lib_OpenCnt ) );

#ifdef MORPHOS
	if (UserLibOpen())
#else
	if (UserLibOpen((struct Library *)base))
#endif
	{
		#ifdef CLASS_VERSIONFAKE
		base->lh_Library.lib_Version	= (UWORD)MUIMasterBase->lib_Version;
		base->lh_Library.lib_Revision	= (UWORD)MUIMasterBase->lib_Revision;
		#endif

		rc = base;
	}
	else
	{
		rc = NULL;
		base->lh_Library.lib_OpenCnt--;
		D(bug("\7UserLibOpen() failed\n") );
	}

#ifndef MORPHOS
	ReleaseSemaphore(&base->lh_Semaphore);
#endif

	return(rc);
}


#ifdef MORPHOS
APTR LibClose( void )
{
	struct LibraryHeader *base = (struct LibraryHeader *)REG_A6;
#else
BPTR ASM SAVEDS LibClose(REG(a6) struct LibraryHeader *base GNUCREG(a6))
{
	struct Library *SysBase = *((struct Library **)4);
#endif

	BPTR rc = NULL;

#ifndef MORPHOS
	ObtainSemaphore(&base->lh_Semaphore);
#endif

	D(bug( "OpenCount = %ld %s\n", base->lh_Library.lib_OpenCnt, base->lh_Library.lib_OpenCnt == 0 ? "\7ERROR" : "" ) );


#ifdef MORPHOS
	UserLibClose();
#else
	UserLibClose((struct Library *)base);
#endif

	if (--base->lh_Library.lib_OpenCnt == 0)
	{

#ifndef EXPUNGE_AT_LAST_CLOSE
		if (base->lh_Library.lib_Flags & LIBF_DELEXP)
#endif

#ifdef MORPHOS
			rc = LibExpunge();
#else
			rc = LibExpunge(base);
#endif
	}

#ifndef MORPHOS
	ReleaseSemaphore(&base->lh_Semaphore);
#endif

	return(rc);
}


#ifdef MORPHOS

#ifdef SUPERCLASS
ULONG _Dispatcher();
struct EmulLibEntry _Dispatcher_gate = { TRAP_LIB, 0, (void (*)(void)) _Dispatcher };
#endif

#ifdef SUPERCLASSP
ULONG _DispatcherP();
struct EmulLibEntry _DispatcherP_gate = { TRAP_LIB, 0, (void (*)(void)) _DispatcherP };
#endif

BOOL UserLibOpen( void )
{
	struct Library *base = (void *)REG_A6;

#else
BOOL ASM SAVEDS UserLibOpen(REG(a6) struct Library *base GNUCREG(a6))
{
	#ifdef SUPERCLASS
#ifdef __GNUC__
	ULONG ASM _Dispatcher(void);
#else
	ULONG ASM SAVEDS _Dispatcher( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif
	#endif

	#ifdef SUPERCLASSP
#ifdef __GNUC__
	ULONG ASM _DispatcherP(void);
#else
	ULONG ASM SAVEDS _DispatcherP( REG(a0) struct IClass *cl GNUCREG(a0), REG(a2) Object *obj GNUCREG(a2), REG(a1) Msg msg GNUCREG(a1) );
#endif
	#endif

#endif

	BOOL PreClassInitFunc(void);
	BOOL ClassInitFunc(struct Library *base);

	D(bug( "OpenCount = %ld\n", base->lib_OpenCnt ) );

	if (base->lib_OpenCnt!=1)
		return(TRUE);

	#ifndef MASTERVERSION
	#define MASTERVERSION MUIMASTER_VMIN
	#endif

	if (MUIMasterBase = OpenLibrary("muimaster.library",MASTERVERSION))
	{
		#ifdef PreClassInit
		PreClassInitFunc();
		#endif

		#ifdef SUPERCLASS
#ifdef MORPHOS
		ThisClass = MUI_CreateCustomClass(base,SUPERCLASS,NULL,sizeof(struct Data),&_Dispatcher_gate);
#else
		ThisClass = MUI_CreateCustomClass(base,SUPERCLASS,NULL,sizeof(struct Data),_Dispatcher);
#endif

		if ( ThisClass )
		#endif
		{
			#ifdef SUPERCLASSP
#ifdef MORPHOS
			if (ThisClassP = MUI_CreateCustomClass(base,SUPERCLASSP,NULL,sizeof(struct DataP),&_DispatcherP_gate))
#else
			if (ThisClassP = MUI_CreateCustomClass(base,SUPERCLASSP,NULL,sizeof(struct DataP),_DispatcherP))
#endif
			#endif
			{
				#ifdef SUPERCLASS
				#define THISCLASS ThisClass
				#else
				#define THISCLASS ThisClassP
				#endif

				UtilityBase   = (struct Library *)THISCLASS->mcc_UtilityBase;
				DOSBase       = (struct DosLibrary *)THISCLASS->mcc_DOSBase;
				GfxBase       = (struct GfxBase *)THISCLASS->mcc_GfxBase;
				IntuitionBase = (struct IntuitionBase *)THISCLASS->mcc_IntuitionBase;

				#ifdef __GNUC__
				__DOSBase		= DOSBase;
				__UtilityBase	= UtilityBase;
				#endif /* __GNUC__ */

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

	D(bug("fail.: %08lx %s\n",base,base->lib_Node.ln_Name) );

	return(FALSE);
}


#ifdef MORPHOS
BOOL UserLibClose( void )
{
	struct Library *base = (void *)REG_A6;
#else
BOOL ASM SAVEDS UserLibClose(REG(a6) struct Library *base GNUCREG(a6))
{
#endif

	VOID PostClassExitFunc(void);
	VOID ClassExitFunc(struct Library *base);

	D(bug( "OpenCount = %ld\n", base->lib_OpenCnt ) );

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

		#ifdef PostClassExit
		PostClassExitFunc();
		#endif

		if (MUIMasterBase)
		{
			CloseLibrary(MUIMasterBase);
			MUIMasterBase = NULL;
		}
	}

	return(TRUE);
}


#ifdef MORPHOS
ULONG MCC_Query( void )
{
	LONG which = (LONG)REG_D0;
#else
ULONG SAVEDS ASM MCC_Query( REG(d0) LONG which GNUCREG(d0) )
{
#endif

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

		#ifdef INFOCLASS
		case 4:
		{
			return(TRUE);
		}
		#endif

		#ifdef USEDCLASSES
		case 5:
		{
			return((ULONG)USEDCLASSES);
		}
		#endif

		#ifdef USEDCLASSESP
		case 6:
		{
			return((ULONG)USEDCLASSESP);
		}
		#endif

		#ifdef SHORTHELP
		case 7:
		{
			return((ULONG)SHORTHELP);
		}
		#endif
	}
	return(NULL);
}
