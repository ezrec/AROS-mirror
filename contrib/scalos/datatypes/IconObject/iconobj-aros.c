// iconobj-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>

#include <limits.h>


#include "iconobj.h"

//----------------------------------------------------------------------------

extern struct ExecBase *SysBase;

//----------------------------------------------------------------------------

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libbase, 1, IconObj
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, IconObj
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, IconObj
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, IconObj
);

//----------------------------------------------------------------------------

/* OS3.x Library */

static APTR functable[] =
	{
	IconObj_1_Openlib,
	IconObj_2_Closelib,
	IconObj_3_Expungelib,
	IconObj_4_Extfunclib,
	Dummy_0_ObtainInfoEngine,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct IconObjectDtLibBase),
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

static AROS_UFH3(struct Library *, Initlib,
		 AROS_UFHA(struct Library *, libbase, D0),
		 AROS_UFHA(struct SegList *, seglist, A0),
		 AROS_UFHA(struct ExecBase *, sysbase, A6)
)
{
	AROS_USERFUNC_INIT

	struct IconObjectDtLibBase *dtLib = (struct IconObjectDtLibBase *) libbase;

	SysBase = sysbase;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = SCHAR_MIN;
	dtLib->nib_SegList = seglist;

	if (!InitDatatype(dtLib))
		{
		IconObj_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
		dtLib = NULL;
		}

	return dtLib ? &dtLib->nib_ClassLibrary.cl_Lib : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, IconObj
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectDtLibBase *dtLib = (struct IconObjectDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(dtLib))
		{
		IconObj_2_Closelib(&dtLib->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &dtLib->nib_ClassLibrary.cl_Lib;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, IconObj
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectDtLibBase *dtLib = (struct IconObjectDtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (dtLib->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return IconObj_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, IconObj
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectDtLibBase *dtLib = (struct IconObjectDtLibBase *) libbase;

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

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, IconObj
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}
