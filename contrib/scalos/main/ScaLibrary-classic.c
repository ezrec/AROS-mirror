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


//===== Library initialization table ==========================

#if defined(__MORPHOS__)
#pragma pack(2)
#endif

struct ScalosLibInit
{
	UWORD ln_Type_Init;      UWORD ln_Type_Offset;      UWORD ln_Type_Contents;
	UWORD ln_Name_Init;      UWORD ln_Name_Offset;      ULONG ln_Name_Contents;
	UWORD lib_Flags_Init;    UWORD lib_Flags_Offset;    UWORD lib_Flags_Contents;
	UWORD lib_Version_Init;  UWORD lib_Version_Offset;  UWORD lib_Version_Contents;
	UWORD lib_Revision_Init; UWORD lib_Revision_Offset; UWORD lib_Revision_Contents;
	UWORD lib_IdString_Init; UWORD lib_IdString_Offset; ULONG lib_IdString_Contents;
	UWORD lib_RelStr_Init; 	 UWORD lib_RelStr_Offset;   ULONG lib_RelStr_Contents;
	ULONG ENDMARK;
};

#if defined(__MORPHOS__)
#pragma pack()
#endif

#if 0
static UWORD ScalosLibInitTable[] =
	{
	INITBYTE(OFFSET(Node, ln_Type), NT_LIBRARY),
	INITLONG(OFFSET(Node, ln_Name), (ULONG) scalosLibName),
	INITBYTE(OFFSET(Library, lib_Flags), LIBF_SUMUSED),
	INITWORD(OFFSET(Library, lib_Version), VERSION),
	INITWORD(OFFSET(Library, lib_Revision), REVISION),
	INITLONG(OFFSET(Library, lib_IdString), (ULONG) libIdString),
	INITLONG(OFFSET(ScalosBase, scb_Revision), (ULONG) ScalosRevision),
	0
	};
#else
static struct ScalosLibInit ScalosLibInitTable =
{
	INITBYTE(OFFSET(Node,		ln_Type),      NT_LIBRARY),
	0xc000, (UWORD) OFFSET(Node,	ln_Name),      (ULONG) scalosLibName,
	INITBYTE(OFFSET(Library,	lib_Flags),    LIBF_SUMUSED | LIBF_CHANGED),
	INITWORD(OFFSET(Library,	lib_Version),  VERSION),
	INITWORD(OFFSET(Library,	lib_Revision), REVISION),
	0xc000, (UWORD) OFFSET(Library,	lib_IdString), (ULONG) libIdString,
	0xc000, (UWORD) OFFSET(ScalosBase, scb_Revision), (ULONG) ScalosRevision,
	(ULONG) 0
};
#endif

//===== Library initialization functions ======================

static LIBFUNC_P2_PROTO(struct Library *, sca_LibOpen,
	D0, ULONG, version,
	A6, struct Library *, libBase);
static LIBFUNC_P1_PROTO(BPTR, sca_LibClose,
	A6, struct Library *, libBase);
static LIBFUNC_P1_PROTO(BPTR, sca_LibExpunge,
	A6, struct Library *, libBase);
static LIBFUNC_P1_PROTO(void, sca_LibNull,
	A6, struct Library *, libBase);

static LIBFUNC_P2(struct Library *, sca_LibOpen,
	D0, ULONG, version,
	A6, struct Library *, libBase)
{
	(void) version;

	libBase->lib_OpenCnt++;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount++;

	return libBase;
}
LIBFUNC_END

static LIBFUNC_P1(BPTR, sca_LibClose,
	A6, struct Library *, libBase)
{
	libBase->lib_OpenCnt--;

	d1(KPrintF("%s/%s/%ld: OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, libBase->lib_OpenCnt));

	if (fInitializingPlugins)
		ScaLibPluginOpenCount--;

	return NULL;
}
LIBFUNC_END

static LIBFUNC_P1(BPTR, sca_LibExpunge,
	A6, struct Library *, libBase)
{
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
}
LIBFUNC_END

static LIBFUNC_P1(void, sca_LibNull,
	A6, struct Library *, libBase)
{
	(void) libBase;
}
LIBFUNC_END


//===== Library function list =================================

APTR ScalosLibFunctions[] =
{
#ifdef __MORPHOS__
	(APTR) FUNCARRAY_32BIT_NATIVE,
#endif
	sca_LibOpen,
	sca_LibClose,
	sca_LibExpunge,
	sca_LibNull,

	sca_WBStart,
	sca_SortNodes,
	sca_NewAddAppIcon,
	sca_RemoveAppObject,
	sca_NewAddAppWindow,
	sca_NewAddAppMenuItem,
	sca_AllocStdNode,
	sca_AllocNode,
	sca_FreeNode,
	sca_FreeAllNodes,
	sca_MoveNode,
	sca_SwapNodes,
	sca_OpenIconWindow,
	sca_LockWindowList,
	sca_UnLockWindowList,
	sca_AllocMessage,
	sca_FreeMessage,
	sca_InitDrag,
	sca_EndDrag,
	sca_AddBob,
	sca_DrawDrag,
	sca_UpdateIcon,
	sca_MakeWBArgs,
	sca_FreeWBArgs,
	sca_RemapBitmap,
	sca_ScreenTitleMsg,
	sca_MakeScalosClass,
	sca_FreeScalosClass,
	sca_NewScalosObject,
	sca_DisposeScalosObject,
	sca_ScalosControl,
	sca_GetDefIconObject,
	sca_OpenDrawerByName,
	sca_CountWBArgs,
	sca_GetDefIconObjectA,
	sca_LockDrag,
	sca_UnlockDrag,

	(APTR) ~0
};

struct ScalosBase *sca_MakeLibrary(void)
{
	return (struct ScalosBase *) MakeLibrary(ScalosLibFunctions,
		&ScalosLibInitTable,
		NULL,
		sizeof(struct ScalosBase), NULL);
}


