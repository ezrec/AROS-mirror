// pngicondt-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>


#include "PNGIconDt.h"

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

// Standard library functions

static LIB_SAVEDS(struct Library *) LIB_ASM LIB_INTERRUPT Initlib(LIB_REG(d0, struct Library *libbase),
	LIB_REG(a0, struct SegList *seglist), LIB_REG(a6, struct ExecBase *sysbase));

static LIBFUNC_PROTO(Openlib, libbase, struct Library *);
static LIBFUNC_PROTO(Closelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Expungelib, libbase, struct SegList *);
static LIBFUNC_PROTO(Extfunclib, libbase, ULONG);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

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
	sizeof(struct PngIconObjectDtLibBase),
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
	12,
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
	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	SysBase = sysbase;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 12;
	PngDtLibBase->nib_SegList = seglist;

	if (!InitDatatype(PngDtLibBase))
		{
		CALLLIBFUNC(Expungelib, &PngDtLibBase->nib_ClassLibrary.cl_Lib);
		PngDtLibBase = NULL;
		}

	return PngDtLibBase ? &PngDtLibBase->nib_ClassLibrary.cl_Lib : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(PngDtLibBase))
		{
		CALLLIBFUNC(Closelib, &PngDtLibBase->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &PngDtLibBase->nib_ClassLibrary.cl_Lib;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &PngDtLibBase->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	if (0 == PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		ULONG size = PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_NegSize + PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_PosSize;
		UBYTE *ptr = (UBYTE *) PngDtLibBase - PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_NegSize;
		struct SegList *libseglist = PngDtLibBase->nib_SegList;

		Remove((struct Node *) PngDtLibBase);
		CloseDatatype(PngDtLibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	d1(KPrintF("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));
	(void) libbase;
	return 0;
}
LIBFUNC_END

//-----------------------------------------------------------------------------

