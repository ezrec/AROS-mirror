// IconObject-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <aros/libcall.h>

#include <proto/exec.h>
#include <proto/iconobject.h>


#include "IconObject.h"

//----------------------------------------------------------------------------
// Standard library functions

static AROS_UFP3 (struct Library *, Initlib,
		  AROS_UFPA(struct Library *, libbase, D0),
		  AROS_UFPA(struct SegList *, seglist, A0),
		  AROS_UFPA(struct ExecBase *, sysbase, A6)
);
static AROS_LD1 (struct Library *, Openlib,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, base, 1, IconObject
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, IconObject
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, base, 3, IconObject
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, IconObject
);

//----------------------------------------------------------------------------

SAVEDS(LONG) ASM Libstart(void)
{
	return -1;
}

/* OS3.x Library */

static APTR functable[] =
	{
	IconObject_1_Openlib,
	IconObject_2_Closelib,
	IconObject_3_Expungelib,
	IconObject_4_Extfunclib,
	IconObjectBase_0_LIBNewIconObject,
	IconObjectBase_0_LIBDisposeIconObject,
	IconObjectBase_0_LIBGetDefIconObject,
	IconObjectBase_0_LIBPutIconObject,
	IconObjectBase_0_LIBIsIconName,
	IconObjectBase_0_LIBConvert2IconObject,
	IconObjectBase_0_LIBConvert2IconObjectA,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct IconObjectBase),
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

	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	SysBase = sysbase;
	IconObjLibBase->iob_LibNode.lib_Revision = LIB_REVISION;
	IconObjLibBase->iob_SegList = seglist;

	if (!IconObjectInit(IconObjLibBase))
		{
		IconObject_3_Expungelib(&IconObjLibBase->iob_LibNode, &IconObjLibBase->iob_LibNode);
		IconObjLibBase = NULL;
		}

	return IconObjLibBase ? &IconObjLibBase->iob_LibNode : NULL;

	AROS_USERFUNC_EXIT
}

static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, IconObject
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt++;
	IconObjLibBase->iob_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!IconObjectOpen(IconObjLibBase))
		{
		IconObject_2_Closelib(&IconObjLibBase->iob_LibNode);
		return NULL;
		}

	return &IconObjLibBase->iob_LibNode;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, IconObject
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	IconObjLibBase->iob_LibNode.lib_OpenCnt--;

	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		if (IconObjLibBase->iob_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return IconObject_3_Expungelib(&IconObjLibBase->iob_LibNode, &IconObjLibBase->iob_LibNode);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, IconObject
)
{
	AROS_LIBFUNC_INIT

	struct IconObjectBase *IconObjLibBase = (struct IconObjectBase *) libbase;

	if (0 == IconObjLibBase->iob_LibNode.lib_OpenCnt)
		{
		ULONG size = IconObjLibBase->iob_LibNode.lib_NegSize + IconObjLibBase->iob_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) IconObjLibBase - IconObjLibBase->iob_LibNode.lib_NegSize;
		struct SegList *libseglist = IconObjLibBase->iob_SegList;

		Remove((struct Node *) IconObjLibBase);
		IconObjectCleanup(IconObjLibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	IconObjLibBase->iob_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib, __unused struct Library *, libbase, 4, IconObject)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}
