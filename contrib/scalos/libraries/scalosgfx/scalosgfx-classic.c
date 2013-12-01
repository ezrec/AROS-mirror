// scalosgfx-classic.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>

#include <exec/resident.h>

#include <proto/exec.h>

#include "scalosgfx.h"

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
	LIBScalosGfxCreateEmptySAC,
	LIBScalosGfxCreateSAC,
	LIBScalosGfxFreeSAC,
	LIBScalosGfxCreateARGB,
	LIBScalosGfxFreeARGB,
	LIBScalosGfxARGBSetAlpha,
	LIBScalosGfxARGBSetAlphaMask,
	LIBScalosGfxCreateARGBFromBitMap,
	LIBScalosGfxFillARGBFromBitMap,
	LIBScalosGfxWriteARGBToBitMap,
	LIBScalosGfxMedianCut,
	LIBScalosGfxScaleARGBArray,
	LIBScalosGfxScaleBitMap,
	LIBScalosGfxCalculateScaleAspect,
	LIBScalosGfxBlitARGB,
	LIBScalosGfxFillRectARGB,
	LIBScalosGfxSetARGB,
	LIBScalosGfxNewColorMap,
	LIBScalosGfxARGBRectMult,
	LIBScalosGfxBlitARGBAlpha,
	LIBScalosGfxBlitARGBAlphaTagList,
	LIBScalosGfxBlitIcon,
	LIBScalosGfxDrawGradient,
	LIBScalosGfxDrawGradientRastPort,
	LIBScalosGfxDrawLine,
	LIBScalosGfxDrawLineRastPort,
	LIBScalosGfxDrawEllipse,
	LIBScalosGfxDrawEllipseRastPort,
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
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	/* store pointer to execbase for global access */
	SysBase = sysbase;

	ScalosGfxLibBase->sgb_LibNode.lib_Revision = LIB_REVISION;
	ScalosGfxLibBase->sgb_SegList = seglist;

	ScalosGfxLibBase->sgb_Initialized = FALSE;

	if (!ScalosGfxInit(ScalosGfxLibBase))
		{
		CALLLIBFUNC(Expungelib, &ScalosGfxLibBase->sgb_LibNode);
		ScalosGfxLibBase = NULL;
		}

	return ScalosGfxLibBase ? &ScalosGfxLibBase->sgb_LibNode : NULL;
}


static LIBFUNC(Openlib, libbase, struct Library *)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt++;
	ScalosGfxLibBase->sgb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!ScalosGfxLibBase->sgb_Initialized)
		{
		if (!ScalosGfxOpen(ScalosGfxLibBase))
			{
			CALLLIBFUNC(Closelib, &ScalosGfxLibBase->sgb_LibNode);
			return NULL;
			}

		ScalosGfxLibBase->sgb_Initialized = TRUE;
		}

	return &ScalosGfxLibBase->sgb_LibNode;
}
LIBFUNC_END


static LIBFUNC(Closelib, libbase, struct SegList *)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt--;

	if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		if (ScalosGfxLibBase->sgb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return CALLLIBFUNC(Expungelib, &ScalosGfxLibBase->sgb_LibNode);
			}
		}

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Expungelib, libbase, struct SegList *)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;

	if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		ULONG size = ScalosGfxLibBase->sgb_LibNode.lib_NegSize + ScalosGfxLibBase->sgb_LibNode.lib_PosSize;
		UBYTE *ptr = (UBYTE *) ScalosGfxLibBase - ScalosGfxLibBase->sgb_LibNode.lib_NegSize;
		struct SegList *libseglist = ScalosGfxLibBase->sgb_SegList;

		Remove((struct Node *) ScalosGfxLibBase);
		ScalosGfxCleanup(ScalosGfxLibBase);
		FreeMem(ptr,size);

		return libseglist;
		}

	ScalosGfxLibBase->sgb_LibNode.lib_Flags |= LIBF_DELEXP;

	return NULL;
}
LIBFUNC_END


static LIBFUNC(Extfunclib, libbase, ULONG)
{
	(void) libbase;

	return 0;
}
LIBFUNC_END

//----------------------------------------------------------------------------

