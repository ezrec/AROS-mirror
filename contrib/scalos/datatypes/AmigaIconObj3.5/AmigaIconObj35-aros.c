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
		 struct Library *, libbase, 1, AmigaIconObject35
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, AmigaIconObject35
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, AmigaIconObject35
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, AmigaIconObject35
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
	AmigaIconObject35_1_Openlib,
	AmigaIconObject35_2_Closelib,
	AmigaIconObject35_3_Expungelib,
	AmigaIconObject35_4_Extfunclib,
	Dummy_0_ObtainInfoEngine,
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

	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	SysBase = sysbase;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Revision = LIB_REVISION;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Node.ln_Pri = 7;
	dtLib->nib_SegList = seglist;

	aroscbase = OpenLibrary("arosc.library", 0);

	if (!aroscbase || !InitDatatype(dtLib))
		{
		AmigaIconObject35_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
		dtLib = NULL;
		}

	return dtLib ? &dtLib->nib_ClassLibrary.cl_Lib : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, AmigaIconObject35
)
{
	AROS_LIBFUNC_INIT

	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt++;
	dtLib->nib_ClassLibrary.cl_Lib.lib_Flags &= ~LIBF_DELEXP;

	if (!OpenDatatype(dtLib))
		{
		AmigaIconObject35_2_Closelib(&dtLib->nib_ClassLibrary.cl_Lib);
		return NULL;
		}

	return &dtLib->nib_ClassLibrary.cl_Lib;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, AmigaIconObject35
)
{
	AROS_LIBFUNC_INIT

	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

	dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt--;

	if (0 == dtLib->nib_ClassLibrary.cl_Lib.lib_OpenCnt)
		{
		if (dtLib->nib_ClassLibrary.cl_Lib.lib_Flags & LIBF_DELEXP)
			{
			return AmigaIconObject35_3_Expungelib(&dtLib->nib_ClassLibrary.cl_Lib, &dtLib->nib_ClassLibrary.cl_Lib);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, AmigaIconObject35
)
{
	AROS_LIBFUNC_INIT

	struct AmigaIconObj35DtLibBase *dtLib = (struct AmigaIconObj35DtLibBase *) libbase;

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
		__unused struct Library *, libbase, 4, AmigaIconObject35
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}
