// scalosgfx-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <aros/libcall.h>

#include <proto/exec.h>

#include "scalosgfx.h"

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
		 struct Library *, libbase, 1, ScalosGfx
);
static AROS_LD0 (struct SegList *, Closelib,
		 struct Library *, base, 2, ScalosGfx
);
static AROS_LD1 (struct SegList *, Expungelib,
		 AROS_LPA(__unused struct Library *, __extrabase, D0),
		 struct Library *, libbase, 3, ScalosGfx
);
static AROS_LD0 (ULONG, Extfunclib,
		 __unused struct Library *, libbase, 4, ScalosGfx
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
	ScalosGfx_1_Openlib,
	ScalosGfx_2_Closelib,
	ScalosGfx_3_Expungelib,
	ScalosGfx_4_Extfunclib,
	ScalosGfxBase_0_LIBScalosGfxCreateEmptySAC,
	ScalosGfxBase_0_LIBScalosGfxCreateSAC,
	ScalosGfxBase_0_LIBScalosGfxFreeSAC,
	ScalosGfxBase_0_LIBScalosGfxCreateARGB,
	ScalosGfxBase_0_LIBScalosGfxFreeARGB,
	ScalosGfxBase_0_LIBScalosGfxARGBSetAlpha,
	ScalosGfxBase_0_LIBScalosGfxARGBSetAlphaMask,
	ScalosGfxBase_0_LIBScalosGfxCreateARGBFromBitMap,
	ScalosGfxBase_0_LIBScalosGfxFillARGBFromBitMap,
	ScalosGfxBase_0_LIBScalosGfxWriteARGBToBitMap,
	ScalosGfxBase_0_LIBScalosGfxMedianCut,
	ScalosGfxBase_0_LIBScalosGfxScaleARGBArray,
	ScalosGfxBase_0_LIBScalosGfxScaleBitMap,
	ScalosGfxBase_0_LIBScalosGfxCalculateScaleAspect,
	ScalosGfxBase_0_LIBScalosGfxBlitARGB,
	ScalosGfxBase_0_LIBScalosGfxFillRectARGB,
	ScalosGfxBase_0_LIBScalosGfxSetARGB,
	ScalosGfxBase_0_LIBScalosGfxNewColorMap,
	ScalosGfxBase_0_LIBScalosGfxARGBRectMult,
	ScalosGfxBase_0_LIBScalosGfxBlitARGBAlpha,
	ScalosGfxBase_0_LIBScalosGfxBlitARGBAlphaTagList,
	ScalosGfxBase_0_LIBScalosGfxBlitIcon,
	ScalosGfxBase_0_LIBScalosGfxDrawGradient,
	ScalosGfxBase_0_LIBScalosGfxDrawGradientRastPort,
	ScalosGfxBase_0_LIBScalosGfxDrawLine,
	ScalosGfxBase_0_LIBScalosGfxDrawLineRastPort,
	ScalosGfxBase_0_LIBScalosGfxDrawEllipse,
	ScalosGfxBase_0_LIBScalosGfxDrawEllipseRastPort,
	(APTR) -1
	};

/* Init table used in library initialization. */
static ULONG inittab[] =
	{
	sizeof(struct ScalosGfxBase),
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

	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	ScalosGfxLibBase->sgb_LibNode.lib_Revision = LIB_REVISION;
	ScalosGfxLibBase->sgb_SegList = seglist;

	ScalosGfxLibBase->sgb_Initialized = FALSE;

	aroscbase = OpenLibrary("arosc.library", 0);

	if (!ScalosGfxInit(ScalosGfxLibBase) || !aroscbase)
		{
		ScalosGfx_3_Expungelib(&ScalosGfxLibBase->sgb_LibNode, &ScalosGfxLibBase->sgb_LibNode);
		ScalosGfxLibBase = NULL;
		}

	return ScalosGfxLibBase ? &ScalosGfxLibBase->sgb_LibNode : NULL;

	AROS_USERFUNC_EXIT
}


static AROS_LH1(struct Library *, Openlib,
		AROS_LHA(__unused ULONG, version, D0),
		struct Library *, libbase, 1, ScalosGfx
)
{
	AROS_LIBFUNC_INIT

	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt++;
	ScalosGfxLibBase->sgb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!ScalosGfxLibBase->sgb_Initialized)
		{
		if (!ScalosGfxOpen(ScalosGfxLibBase))
			{
			ScalosGfx_2_Closelib(&ScalosGfxLibBase->sgb_LibNode);
			return NULL;
			}

		ScalosGfxLibBase->sgb_Initialized = TRUE;
		}

	return &ScalosGfxLibBase->sgb_LibNode;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(struct SegList *, Closelib,
		struct Library *, libbase, 2, ScalosGfx
)
{
	AROS_LIBFUNC_INIT

	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt--;

	if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		if (ScalosGfxLibBase->sgb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return ScalosGfx_3_Expungelib(&ScalosGfxLibBase->sgb_LibNode, &ScalosGfxLibBase->sgb_LibNode);
			}
		}

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH1(struct SegList *, Expungelib,
		AROS_LHA(__unused struct Library *, __extrabase, D0),
		struct Library *, libbase, 3, ScalosGfx
)
{
	AROS_LIBFUNC_INIT

	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		ULONG size = ScalosGfxLibBase->sgb_LibNode.lib_NegSize + ScalosGfxLibBase->sgb_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) ScalosGfxLibBase - ScalosGfxLibBase->sgb_LibNode.lib_NegSize;
		struct SegList *libseglist = ScalosGfxLibBase->sgb_SegList;

		Remove((struct Node *) ScalosGfxLibBase);
		ScalosGfxCleanup(ScalosGfxLibBase);
		FreeMem(ptr,size);

		CloseLibrary(aroscbase);

		return libseglist;
		}

	ScalosGfxLibBase->sgb_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;

	AROS_LIBFUNC_EXIT
}


static AROS_LH0(ULONG, Extfunclib,
		__unused struct Library *, libbase, 4, ScalosGfx
)
{
	AROS_LIBFUNC_INIT

	return 0;

	AROS_LIBFUNC_EXIT
}

//----------------------------------------------------------------------------
