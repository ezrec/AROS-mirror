// scalosgfx-aos4.c
// $Date$
// $Revision$


#include <exec/interfaces.h>
#include <exec/exectags.h>
#include <exec/resident.h>

#include <proto/exec.h>
#include <interfaces/scalosgfx.h>

#include <stdarg.h>

#include "scalosgfx.h"

#include <defs.h>

int _start(void)
{
	return -1;
}

extern CONST_APTR VecTable68K[];

/* -------------------- Internal functions  ------------------------- */

static struct Library *Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec);
static BPTR Expungelib(struct LibraryManagerInterface *Self);
static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version);
static BPTR Closelib(struct LibraryManagerInterface *Self);
static ULONG Obtainlib(struct LibraryManagerInterface *Self);
static ULONG Releaselib(struct LibraryManagerInterface *Self);

static LIBFUNC_P5VA_PROTO(struct ScalosBitMapAndColor *, LIBScalosGfxCreateSACTags,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, depth,
	A0, struct BitMap *, friendBM,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P3VA_PROTO(struct gfxARGB *, LIBScalosGfxCreateARGBTags,
	D0, ULONG, width,
	D1, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P3VA_PROTO(struct ScalosBitMapAndColor *, LIBScalosGfxMedianCutTags,
	A0, struct ARGBHeader *, argbh,
	D0, ULONG, depth,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P4VA_PROTO(struct gfxARGB *, LIBScalosGfxScaleARGBArrayTags,
	A0, const struct ARGBHeader *, src,
	A1, ULONG *, destWidth,
	A2, ULONG *, destHeight,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P2VA_PROTO(struct BitMap *, LIBScalosGfxScaleBitMapTags,
	A0, struct ScaleBitMapArg *, sbma,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P6VA_PROTO(VOID, LIBScalosGfxBlitARGBAlphaTags,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	A3, const struct IBox *, srcSize,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
static LIBFUNC_P7VA_PROTO(VOID, LIBScalosGfxBlitIconTags,
	A0, struct RastPort *, rpBackground,
	A1, struct RastPort *, rpIcon,
	D0, ULONG, left,
	D1, ULONG, top,
	D2, ULONG, width,
	D3, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase);

/* OS4.0 Library */

/* ------------------- OS4 Manager Interface ------------------------ */
static const APTR managerfunctable[] =
{
  (APTR)Obtainlib,
  (APTR)Releaselib,
  (APTR)NULL,
  (APTR)NULL,
  (APTR)Openlib,
  (APTR)Closelib,
  (APTR)Expungelib,
  (APTR)NULL,
  (APTR)-1
};

static const struct TagItem managertags[] =
{
  {MIT_Name,             (ULONG)"__library"},
  {MIT_VectorTable,      (ULONG)managerfunctable},
  {MIT_Version,          1},
  {TAG_DONE,             0}
};

/* ---------------------- OS4 Main Interface ------------------------ */
static APTR functable[] =
	{
	Obtainlib,
	Releaselib,
	NULL,
	NULL,
	LIBScalosGfxCreateEmptySAC,
	LIBScalosGfxCreateSAC,
	LIBScalosGfxCreateSACTags,
	LIBScalosGfxFreeSAC,
	LIBScalosGfxCreateARGB,
	LIBScalosGfxCreateARGBTags,
	LIBScalosGfxFreeARGB,
	LIBScalosGfxARGBSetAlpha,
	LIBScalosGfxARGBSetAlphaMask,
	LIBScalosGfxCreateARGBFromBitMap,
	LIBScalosGfxFillARGBFromBitMap,
	LIBScalosGfxWriteARGBToBitMap,
	LIBScalosGfxMedianCut,
	LIBScalosGfxMedianCutTags,
	LIBScalosGfxScaleARGBArray,
	LIBScalosGfxScaleARGBArrayTags,
	LIBScalosGfxScaleBitMap,
	LIBScalosGfxScaleBitMapTags,
	LIBScalosGfxCalculateScaleAspect,
	LIBScalosGfxBlitARGB,
	LIBScalosGfxFillRectARGB,
	LIBScalosGfxSetARGB,
	LIBScalosGfxNewColorMap,
	LIBScalosGfxARGBRectMult,
	LIBScalosGfxBlitARGBAlpha,
	LIBScalosGfxBlitARGBAlphaTagList,
	LIBScalosGfxBlitARGBAlphaTags,
	LIBScalosGfxBlitIcon,
	LIBScalosGfxBlitIconTags,
	LIBScalosGfxDrawGradient,
	LIBScalosGfxDrawGradientRastPort,
	LIBScalosGfxDrawLine,
	LIBScalosGfxDrawLineRastPort,
	LIBScalosGfxDrawEllipse,
	LIBScalosGfxDrawEllipseRastPort,
	(APTR) -1
	};

static const struct TagItem maintags[] =
	{
	{MIT_Name,             (ULONG)"main"},
	{MIT_VectorTable,      (ULONG)functable},
	{MIT_Version,          1},
	{TAG_DONE,             0}
	};

/* Init table used in library initialization. */
static const ULONG interfaces[] =
{
   (ULONG)managertags,
   (ULONG)maintags,
   (ULONG)0
};

static const struct TagItem inittab[] =
	{
	{CLT_DataSize, (ULONG)sizeof(struct ScalosGfxBase)},
	{CLT_Interfaces, (ULONG) interfaces},
	{CLT_Vector68K,  (ULONG)VecTable68K},
	{CLT_InitFunc, (ULONG) Initlib},
	{TAG_DONE, 0}
	};


/* The ROM tag */
struct Resident ALIGNED romtag =
	{
	RTC_MATCHWORD,
	&romtag,
	&romtag + 1,
	RTF_NATIVE | RTF_AUTOINIT,
	LIB_VERSION,
	NT_LIBRARY,
	0,
	libName,
	libIdString,
	(struct TagItem *)inittab
	};

/* ------------------- OS4 Manager Functions ------------------------ */
static struct Library * Initlib(struct Library *libbase, BPTR seglist, struct ExecIFace *pIExec)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) libbase;
	struct ExtendedLibrary *ExtLib = (struct ExtendedLibrary *) ((ULONG)libbase + libbase->lib_PosSize);
	struct LibraryManagerInterface *Self = (struct LibraryManagerInterface *) ExtLib->ILibrary;

	SysBase = (struct ExecBase *)pIExec->Data.LibBase;
	IExec = pIExec;
	ScalosGfxLibBase->sgb_LibNode.lib_Revision = LIB_REVISION;
	ScalosGfxLibBase->sgb_SegList = (struct SegList *)seglist;

	if (!ScalosGfxInit(ScalosGfxLibBase))
		{
		Expungelib(Self);
		ScalosGfxLibBase = NULL;
		}

	return ScalosGfxLibBase ? &ScalosGfxLibBase->sgb_LibNode : NULL;
}

static BPTR Expungelib(struct LibraryManagerInterface *Self)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) Self->Data.LibBase;

	if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		struct SegList *libseglist = ScalosGfxLibBase->sgb_SegList;

		Remove((struct Node *) ScalosGfxLibBase);
		ScalosGfxCleanup(ScalosGfxLibBase);
		DeleteLibrary((struct Library *)ScalosGfxLibBase);

		return (BPTR)libseglist;
		}

	ScalosGfxLibBase->sgb_LibNode.lib_Flags |= LIBF_DELEXP;

	return (BPTR)NULL;
}

static struct LibraryHeader *Openlib(struct LibraryManagerInterface *Self, ULONG version)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) Self->Data.LibBase;

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt++;
	ScalosGfxLibBase->sgb_LibNode.lib_Flags &= ~LIBF_DELEXP;

	if (!ScalosGfxLibBase->sgb_Initialized)
		{
		if (!ScalosGfxOpen(ScalosGfxLibBase))
			{
			Closelib(Self);
			return NULL;
			}

		ScalosGfxLibBase->sgb_Initialized = TRUE;
		}

	return (struct LibraryHeader *)&ScalosGfxLibBase->sgb_LibNode;
}

static BPTR Closelib(struct LibraryManagerInterface *Self)
{
	struct ScalosGfxBase *ScalosGfxLibBase = (struct ScalosGfxBase *) Self->Data.LibBase;

	d1(kprintf("%s/%s/%ld: lib_OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, \
		ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt));

	ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt--;

/*	  if (0 == ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt)
		{
		if (ScalosGfxLibBase->sgb_LibNode.lib_Flags & LIBF_DELEXP)
			{
			return Expungelib(Self);
			}
		}
*/
	d1(kprintf("%s/%s/%ld: lib_OpenCnt=%ld\n", __FILE__, __FUNC__, __LINE__, \
		ScalosGfxLibBase->sgb_LibNode.lib_OpenCnt));

	return (BPTR)NULL;
}

static ULONG Obtainlib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount++);
}

static ULONG Releaselib(struct LibraryManagerInterface *Self)
{
  return(Self->Data.RefCount--);
}


static LIBFUNC_P5VA(struct ScalosBitMapAndColor *, LIBScalosGfxCreateSACTags,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, depth,
	A0, struct BitMap *, friendBM,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
	struct ScalosBitMapAndColor *ret;
  	va_list args;
	va_startlinear(args, friendBM);

	(void)ScalosGfxBase;

	ret = ((struct ScalosGfxIFace *)self)->ScalosGfxCreateSAC(width, height, depth, friendBM, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P3VA(struct gfxARGB *, LIBScalosGfxCreateARGBTags,
	D0, ULONG, width,
	D1, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
	struct gfxARGB *ret;
  	va_list args;
	va_startlinear(args, height);

	(void)ScalosGfxBase;

	ret = ((struct ScalosGfxIFace *)self)->ScalosGfxCreateARGB(width, height, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P3VA(struct ScalosBitMapAndColor *, LIBScalosGfxMedianCutTags,
	A0, struct ARGBHeader *, argbh,
	D0, ULONG, depth,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
	struct ScalosBitMapAndColor *ret;
  	va_list args;
	va_startlinear(args, depth);

	(void)ScalosGfxBase;

	ret = ((struct ScalosGfxIFace *)self)->ScalosGfxMedianCut(argbh, depth, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P4VA(struct gfxARGB *, LIBScalosGfxScaleARGBArrayTags,
	A0, const struct ARGBHeader *, src,
	A1, ULONG *, destWidth,
	A2, ULONG *, destHeight,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
	struct gfxARGB *ret;
  	va_list args;
	va_startlinear(args, destHeight);

	(void)ScalosGfxBase;

	ret = ((struct ScalosGfxIFace *)self)->ScalosGfxScaleARGBArray(src, destWidth, destHeight, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P2VA(struct BitMap *, LIBScalosGfxScaleBitMapTags,
	A0, struct ScaleBitMapArg *, sbma,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
	struct BitMap *ret;
  	va_list args;
	va_startlinear(args, sbma);

	(void)ScalosGfxBase;

	ret = ((struct ScalosGfxIFace *)self)->ScalosGfxScaleBitMap(sbma, va_getlinearva(args, struct TagItem *));

	va_end(args);

	return(ret);
}
LIBFUNC_END

static LIBFUNC_P6VA(VOID, LIBScalosGfxBlitARGBAlphaTags,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	A3, const struct IBox *, srcSize,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
  	va_list args;
	va_startlinear(args, srcSize);

	(void)ScalosGfxBase;

	((struct ScalosGfxIFace *)self)->ScalosGfxBlitARGBAlphaTagList(rp,
		srcH,
		destLeft, destTop,
		srcSize,
		va_getlinearva(args, struct TagItem *));

	va_end(args);
}
LIBFUNC_END

static LIBFUNC_P7VA(VOID, LIBScalosGfxBlitIconTags,
	A0, struct RastPort *, rpBackground,
	A1, struct RastPort *, rpIcon,
	D0, ULONG, left,
	D1, ULONG, top,
	D2, ULONG, width,
	D3, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase)
{
  	va_list args;
	va_startlinear(args, height);

	(void)ScalosGfxBase;

	((struct ScalosGfxIFace *)self)->ScalosGfxBlitIcon(rpBackground,
		rpIcon,
		left, top, width, height,
		va_getlinearva(args, struct TagItem *));

	va_end(args);
}
LIBFUNC_END


