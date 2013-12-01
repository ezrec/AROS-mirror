// glowiconobject-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>


#include "GlowIconObject.h"

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

struct Library *aroscbase;

// Standard library functions

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libbase, 1, GlowIconObject
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, GlowIconObject
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, GlowIconObject
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, GlowIconObject
);

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	GlowIconObject_1_Openlib,
	GlowIconObject_2_Closelib,
	GlowIconObject_3_Expungelib,
	GlowIconObject_4_Extfunclib,
	Dummy_0_ObtainInfoEngine,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct GlowIconObjectDtLibBase),
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
	0,
	libName,
	libIdString,
	inittab
	};

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

//----------------------------------------------------------------------------

static AROS_UFH3(struct Library *, Initlib,
		 AROS_UFHA(struct Library *, libbase, D0),
		 AROS_UFHA(struct SegList *, seglist, A0),
		 AROS_UFHA(struct ExecBase *, sysbase, A6)
)
{
	AROS_USERFUNC_INIT

	struct GlowIconObjectDtLibBase *dtLib = (struct GlowIconObjectDtLibBase *) libbase;

	SysBase = sysbase;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 10;
	dtLib->nib_SegList = seglist;

	aroscbase = OpenLibrary("arosc.library", 0);

	if (!aroscbase || !InitDatatype(dtLib))
		{
		GlowIconObject_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
		dtLib = NULL;
		}

	return dtLib ? &dtLib->nib_ClassLibrary.cl_Lib : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, GlowIconObject
)
{
	AROS_LIBFUNC_INIT

	struct GlowIconObjectDtLibBase *dtLib = (struct GlowIconObjectDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(dtLib))
		{
		GlowIconObject_2_Closelib(&dtLib->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &dtLib->nib_ClassLibrary.cl_Lib;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, GlowIconObject
)
{
	AROS_LIBFUNC_INIT

	struct GlowIconObjectDtLibBase *dtLib = (struct GlowIconObjectDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (dtLib->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return GlowIconObject_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, GlowIconObject
)
{
	AROS_LIBFUNC_INIT

	struct GlowIconObjectDtLibBase *dtLib = (struct GlowIconObjectDtLibBase *) libbase;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		ULONG size = dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize + dtLib->nib_ClassLibrary.cl_Lib.lib_PosSize;
		UBYTE *ptr = (UBYTE *) dtLib - dtLib->nib_ClassLibrary.cl_Lib.lib_NegSize;
		struct SegList *libseglist = dtLib->nib_SegList;

		Remove((struct Node *) dtLib);
		CloseDatatype(dtLib);
		FreeMem(ptr,size);

		CloseLibrary(aroscbase);

		return libseglist;
		}

	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags |= LIBF_DELEXP;

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, GlowIconObject
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}
