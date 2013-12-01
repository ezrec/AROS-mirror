// ScaLibrary-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <proto/exec.h>
#include "debug.h"
#include <proto/scalos.h>


#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"
#include "ScaLibrary.h"
#include "Scalos_rev.h"


//===== Library initialization functions ======================

static AROS_LD1 (struct Library *, sca_LibOpen,
		 AROS_LPA (__unused ULONG, version, D0),
		 struct Library *, libBase, 1, Scalos
);
static AROS_LD0 (struct SegList *, sca_LibClose,
		 struct Library *, libBase, 2, Scalos
);
static AROS_LD1 (struct SegList *, sca_LibExpunge,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libBase, 3, Scalos
);
static AROS_LD0 (void, sca_LibNull,
		 __unused struct Library *, libBase, 4, Scalos
);


static AROS_LH1(struct Library *, sca_LibOpen,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libBase, 1, Scalos
)
{
	AROS_LIBFUNC_INIT

	(void) version;

	libBase->lib_OpenCnt++;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount++;

	return libBase;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, sca_LibClose,
		struct Library *, libBase, 2, Scalos
)
{
	AROS_LIBFUNC_INIT

	libBase->lib_OpenCnt--;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount--;

	return NULL;

	AROS_LIBFUNC_EXIT
}

static AROS_LH1(struct SegList *, sca_LibExpunge,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libBase, 3, Scalos
)
{
	AROS_LIBFUNC_INIT

	d1(KPrintF("%s/%s/%ld: libBase=%08lx  OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase, libBase->lib_OpenCnt));

	if (0 == libBase->lib_OpenCnt)
		{
		ULONG size = libBase->lib_NegSize + libBase->lib_PosSize;
		UBYTE *ptr = (UBYTE *) libBase - libBase->lib_NegSize;

		// remove library from exec lib list
		Remove(&libBase->lib_Node);

		FreeMem(ptr, size);
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(void, sca_LibNull, __unused struct Library *, libBase, 4, Scalos)
{
	AROS_LIBFUNC_INIT

	(void) libBase;

	AROS_LIBFUNC_EXIT
}


//===== Library function list =================================

APTR ScalosLibFunctions[] =
{
	Scalos_1_sca_LibOpen,
	Scalos_2_sca_LibClose,
	Scalos_3_sca_LibExpunge,
	Scalos_4_sca_LibNull,

	ScalosBase_0_sca_WBStart,
	ScalosBase_0_sca_SortNodes,
	ScalosBase_0_sca_NewAddAppIcon,
	ScalosBase_0_sca_RemoveAppObject,
	ScalosBase_0_sca_NewAddAppWindow,
	ScalosBase_0_sca_NewAddAppMenuItem,
	ScalosBase_0_sca_AllocStdNode,
	ScalosBase_0_sca_AllocNode,
	ScalosBase_0_sca_FreeNode,
	ScalosBase_0_sca_FreeAllNodes,
	ScalosBase_0_sca_MoveNode,
	ScalosBase_0_sca_SwapNodes,
	ScalosBase_0_sca_OpenIconWindow,
	ScalosBase_0_sca_LockWindowList,
	ScalosBase_0_sca_UnLockWindowList,
	ScalosBase_0_sca_AllocMessage,
	ScalosBase_0_sca_FreeMessage,
	ScalosBase_0_sca_InitDrag,
	ScalosBase_0_sca_EndDrag,
	ScalosBase_0_sca_AddBob,
	ScalosBase_0_sca_DrawDrag,
	ScalosBase_0_sca_UpdateIcon,
	ScalosBase_0_sca_MakeWBArgs,
	ScalosBase_0_sca_FreeWBArgs,
	ScalosBase_0_sca_RemapBitmap,
	ScalosBase_0_sca_ScreenTitleMsg,
	ScalosBase_0_sca_MakeScalosClass,
	ScalosBase_0_sca_FreeScalosClass,
	ScalosBase_0_sca_NewScalosObject,
	ScalosBase_0_sca_DisposeScalosObject,
	ScalosBase_0_sca_ScalosControl,
	ScalosBase_0_sca_GetDefIconObject,
	ScalosBase_0_sca_OpenDrawerByName,
	in_0_sca_CountWBArgs,
	ScalosBase_0_sca_GetDefIconObjectA,
	ScalosBase_0_sca_LockDrag,
	ScalosBase_0_sca_UnlockDrag,

	(APTR) ~0
};

struct ScalosBase *sca_MakeLibrary(void)
{
	struct Library *newBase = MakeLibrary(ScalosLibFunctions,
					      NULL,
					      NULL,
					      sizeof(struct ScalosBase), NULL);

	newBase->lib_Node.ln_Type = NT_LIBRARY;
	newBase->lib_Node.ln_Name = (STRPTR) scalosLibName;
	newBase->lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
	newBase->lib_Version = VERSION;
	newBase->lib_Revision = REVISION;
	newBase->lib_IdString = (STRPTR) libIdString;
	((struct ScalosBase *)newBase)->scb_Revision = ScalosRevision;

	return (struct ScalosBase *)newBase;
}
