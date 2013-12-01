// pngicondt-classic.c
// $Date$
// $Revision$


#include <clib/arossupport_protos.h>

#include <exec/types.h>
#include <exec/initializers.h>
#include <exec/resident.h>

#include <proto/exec.h>


#include "PNGIconDt.h"

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

struct Library *aroscbase;

//----------------------------------------------------------------------------

// Standard library functions

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libbase, 1, PNGIconObject
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, PNGIconObject
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, PNGIconObject
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, PNGIconObject
);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	PNGIconObject_1_Openlib,
	PNGIconObject_2_Closelib,
	PNGIconObject_3_Expungelib,
	PNGIconObject_4_Extfunclib,
	Dummy_0_ObtainInfoEngine,
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
	RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	12,
	libName,
	libIdString,
	inittab
	};

//----------------------------------------------------------------------------

static AROS_UFH3(struct Library *, Initlib,
		 AROS_UFHA(struct Library *, libbase, D0),
		 AROS_UFHA(struct SegList *, seglist, A0),
		 AROS_UFHA(struct ExecBase *, sysbase, A6)
)
{
	AROS_USERFUNC_INIT

	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	SysBase = sysbase;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 12;
	PngDtLibBase->nib_SegList = seglist;

	aroscbase = OpenLibrary("arosc.library", 0);

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	if (!aroscbase || !InitDatatype(PngDtLibBase))
		{
		PNGIconObject_3_Expungelib(&PngDtLibBase->nib_ClassLibrary.cl_Lib, &PngDtLibBase->nib_ClassLibrary.cl_Lib);
		PngDtLibBase = NULL;
		}

	return PngDtLibBase ? &PngDtLibBase->nib_ClassLibrary.cl_Lib : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, PNGIconObject
)
{
	AROS_LIBFUNC_INIT

	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(PngDtLibBase))
		{
		PNGIconObject_2_Closelib(&PngDtLibBase->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &PngDtLibBase->nib_ClassLibrary.cl_Lib;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, PNGIconObject
)
{
	AROS_LIBFUNC_INIT

	struct PngIconObjectDtLibBase *PngDtLibBase = (struct PngIconObjectDtLibBase *) libbase;

	d1(kprintf("%s/%s/%ld: libbase=%08lx\n", __FILE__, __FUNC__, __LINE__, libbase));

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return PNGIconObject_3_Expungelib(&PngDtLibBase->nib_ClassLibrary.cl_Lib, &PngDtLibBase->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, PNGIconObject
)
{
	AROS_LIBFUNC_INIT

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

		CloseLibrary(aroscbase);

		return libseglist;
		}

	PngDtLibBase->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, PNGIconObject
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}

//-----------------------------------------------------------------------------
