// amigaiconobj35-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>


#include "AmigaIconObj35.h"

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif
	Openlib,
	Closelib,
	Expungelib,
	Extfunclib,
	ObtainInfoEngine,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct AmigaIconObj35DtLibBase),
	(ULONG) functable,
	0,
	(ULONG) Initlib
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else
	RTF_AUTOINIT,
#endif
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	inittab
	};

#ifdef __MORPHOS__
ULONG __abox__=1;
#endif

//----------------------------------------------------------------------------

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase))
{
	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	SysBase = sysbase;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 7;
	dtLib->nib_SegList = seglist;

	if (!InitDatatype(dtLib))
		{
		CALLLIBFUNC(Expungelib, &dtLib->nib_ClassLibrary.cl_Lib);
		dtLib = NULL;
		}

	return dtLib ? &dtLib->nib_ClassLibrary.cl_Lib : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(dtLib))
		{
		CALLLIBFUNC(Closelib, &dtLib->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &dtLib->nib_ClassLibrary.cl_Lib;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (dtLib->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &dtLib->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		ULONG size = dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize + dtLib->nib_ClassLibrary.cl_Lib.lib_PosSize;
		UBYTE *ptr = (UBYTE *) dtLib - dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize;
		struct SegList *libseglist = dtLib->nib_SegList;

		Remove((struct Node *) dtLib);
		CloseDatatype(dtLib);
		FreeMem(ptr,size);

		return libseglist;
		}

	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	d1(kprintf(__FUNC__ "/%ld: libbase=%08lx\n", __LINE__, libbase));
	return 0;
}
LIBFUNC_END


