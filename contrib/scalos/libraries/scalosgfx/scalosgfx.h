// scalosgfx.h
// $Date$
// $Revision$


#ifndef SCALOSGFX_H
#define SCALOSGFX_H

#include <exec/types.h>
#include <exec/lists.h>
#include <exec/libraries.h>
#include <graphics/gfx.h>
#include <scalos/scalosgfx.h>

#include <defs.h>

#include "scalosgfx_base.h"

//----------------------------------------------------------------------------

#define	LIB_VERSION	42
#define	LIB_REVISION	2

//----------------------------------------------------------------------------

#if defined(__MORPHOS__) && !defined(ProcessPixelArrayTags)
#define ProcessPixelArrayTags(rp, destX, destY, sizeX, sizeY, operation, value, tags...) \
	({ULONG _tags[] = {tags}; ProcessPixelArray((rp), (destX), (destY), (sizeX), (sizeY), (operation), (value), (struct TagItem *) _tags);})
#endif //defined(__MORPHOS__) && !defined(ProcessPixelArrayTags)

//----------------------------------------------------------------------------

// RGB16:  rrrr rggg  gggb bbbb
// BGR16:  bbbb bggg  gggr rrrr
// RGB15:  0rrr rrgg  gggb bbbb
// GBR15:  0bbb bbgg  gggr rrrr

// RGB16PC:  gggb bbbb  rrrr rggg
// BGR16PC:  gggr rrrr  bbbb bggg
// RGB15PC:  gggb bbbb  0rrr rrgg
// GBR15PC:  gggr rrrr  0bbb bbgg

#define	GET_RED_RGB16(p)	((*((UWORD *) (p)) & 0xf800) >> 8)
#define	GET_GREEN_RGB16(p)	((*((UWORD *) (p)) & 0x07e0) >> 3)
#define	GET_BLUE_RGB16(p)	((*((UWORD *) (p)) & 0x001f) << 3)

#define	GET_RED_BGR16(p)	((*((UWORD *) (p)) & 0x001f) << 3)
#define	GET_GREEN_BGR16(p)	((*((UWORD *) (p)) & 0x07e0) >> 3)
#define	GET_BLUE_BGR16(p)	((*((UWORD *) (p)) & 0xf800) >> 8)

#define	GET_RED_RGB15(p)	((*((UWORD *) (p)) & 0x7c00) >> 7)
#define	GET_GREEN_RGB15(p)	((*((UWORD *) (p)) & 0x03e0) >> 2)
#define	GET_BLUE_RGB15(p)	((*((UWORD *) (p)) & 0x001f) << 3)

#define	GET_RED_BGR15(p)	((*((UWORD *) (p)) & 0x001f) << 3)
#define	GET_GREEN_BGR15(p)	((*((UWORD *) (p)) & 0x03e0) >> 2)
#define	GET_BLUE_BGR15(p)	((*((UWORD *) (p)) & 0x7c00) >> 7)

#define	SET_RED_RGB16(r)	(((r) << 8) & 0xf800)
#define	SET_GREEN_RGB16(g)	(((g) << 3) & 0x07e0)
#define	SET_BLUE_RGB16(b)	(((b) >> 3) & 0x001f)

#define	SET_RED_BGR16(r)	(((r) >> 3) & 0x001f)
#define	SET_GREEN_BGR16(g)	(((g) << 3) & 0x07e0)
#define	SET_BLUE_BGR16(b)	(((b) << 8) & 0xf800)

#define	SET_RED_RGB15(r)	(((r) << 7) & 0x7c00)
#define	SET_GREEN_RGB15(g)	(((g) << 2) & 0x03e0)
#define	SET_BLUE_RGB15(b)	(((b) >> 3) & 0x001f)

#define	SET_RED_BGR15(r)	(((r) >> 3) & 0x001f)
#define	SET_GREEN_BGR15(g)	(((g) << 2) & 0x03e0)
#define	SET_BLUE_BGR15(b)	(((b) << 7) & 0x7c00)

#define	GET_RED_RGB16PC(p)	((*((UWORD *) (p)) & 0x00f8)     )
#define	GET_GREEN_RGB16PC(p)	(((*((UWORD *) (p)) & 0x0007) << 5) | ((*((UWORD *) (p)) & 0xe000) >> 11))
#define	GET_BLUE_RGB16PC(p)	((*((UWORD *) (p)) & 0x1f00) >> 5)

#define	GET_RED_BGR16PC(p)	((*((UWORD *) (p)) & 0x1f00) >> 5)
#define	GET_GREEN_BGR16PC(p)    (((*((UWORD *) (p)) & 0x0007) << 5) | ((*((UWORD *) (p)) & 0xe000) >> 11))
#define	GET_BLUE_BGR16PC(p)	((*((UWORD *) (p)) & 0x00f8)     )

#define	GET_RED_RGB15PC(p)	((*((UWORD *) (p)) & 0x007c) << 1)
#define	GET_GREEN_RGB15PC(p)    (((*((UWORD *) (p)) & 0x0003) << 6) | ((*((UWORD *) (p)) & 0xe000) >> 10))
#define	GET_BLUE_RGB15PC(p)	((*((UWORD *) (p)) & 0x1f00) >> 5)

#define	GET_RED_BGR15PC(p)	((*((UWORD *) (p)) & 0x1f00) >> 5)
#define	GET_GREEN_BGR15PC(p)    (((*((UWORD *) (p)) & 0x0003) << 6) | ((*((UWORD *) (p)) & 0xe000) >> 10))
#define	GET_BLUE_BGR15PC(p)	((*((UWORD *) (p)) & 0x007c) << 1)

#define	SET_RED_RGB16PC(r)	(((r)     ) & 0x00f8)
#define	SET_GREEN_RGB16PC(g)	((((g) >> 5) & 0x0007) | (((g) << 11) & 0xe000))
#define	SET_BLUE_RGB16PC(b)	(((b) << 5) & 0x1f00)

#define	SET_RED_BGR16PC(r)	(((r) << 5) & 0x1f00)
#define	SET_GREEN_BGR16PC(g)    ((((g) >> 5) & 0x0007) | (((g) << 11) & 0xe000))
#define	SET_BLUE_BGR16PC(b)	(((b)     ) & 0x00f8)

#define	SET_RED_RGB15PC(r)	(((r) >> 1) & 0x007c)
#define	SET_GREEN_RGB15PC(g)    ((((g) >> 6) & 0x0003) | (((g) << 10) & 0xe000))
#define	SET_BLUE_RGB15PC(b)	(((b) << 5) & 0x1f00)

#define	SET_RED_BGR15PC(r)	(((r) << 5) & 0x1f00)
#define	SET_GREEN_BGR15PC(g)    ((((g) >> 6) & 0x0003) | (((g) << 10) & 0xe000))
#define	SET_BLUE_BGR15PC(b)	(((b) >> 1) & 0x007c)


#define	min(a, b)	((a) < (b) ? (a) : (b))

//----------------------------------------------------------------------------

extern char ALIGNED libName[];
extern char ALIGNED libIdString[];

extern struct ExecBase *SysBase;
extern struct Library *CyberGfxBase;

//----------------------------------------------------------------------------

BOOL ScalosGfxInit(struct ScalosGfxBase *ScalosGfxBase);
BOOL ScalosGfxOpen(struct ScalosGfxBase *ScalosGfxBase);
void ScalosGfxCleanup(struct ScalosGfxBase *ScalosGfxBase);

LIBFUNC_P1_PROTO(struct ScalosBitMapAndColor *, LIBScalosGfxCreateEmptySAC,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P6_PROTO(struct ScalosBitMapAndColor *, LIBScalosGfxCreateSAC,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, depth,
	A0, struct BitMap *, friendBM,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P2_PROTO(VOID, LIBScalosGfxFreeSAC,
	A0, struct ScalosBitMapAndColor *, sac,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P4_PROTO(struct gfxARGB *, LIBScalosGfxCreateARGB,
	D0, ULONG, width,
	D1, ULONG, height,
	A0, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P2_PROTO(VOID, LIBScalosGfxFreeARGB,
	A0, struct gfxARGB **, argb,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P3_PROTO(VOID, LIBScalosGfxARGBSetAlpha,
	A0, struct ARGBHeader *, argbh,
	D0, UBYTE, alpha,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P3_PROTO(VOID, LIBScalosGfxARGBSetAlphaMask,
	A0, struct ARGBHeader *, argbh,
	A1, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P7_PROTO(struct gfxARGB *, LIBScalosGfxCreateARGBFromBitMap,
	A0, struct BitMap *,bm,
	D0, ULONG, width,
	D1, ULONG, height,
	D2, ULONG, numberOfColors,
	A1, const ULONG *, colorTable,
	A2, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P4_PROTO(VOID, LIBScalosGfxFillARGBFromBitMap,
	A0, struct ARGBHeader *, argbh,
	A1, struct BitMap *, srcBM,
	A2, PLANEPTR, maskPlane,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P5_PROTO(VOID, LIBScalosGfxWriteARGBToBitMap,
	A0, struct ARGBHeader *, argbh,
	A1, struct BitMap *, bm,
	D0, ULONG, numberOfColors,
	A2, const ULONG *, colorTable,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P4_PROTO(struct ScalosBitMapAndColor *, LIBScalosGfxMedianCut,
	A0, struct ARGBHeader *, argbh,
	D0, ULONG, depth,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P5_PROTO(struct gfxARGB *, LIBScalosGfxScaleARGBArray,
	A0, const struct ARGBHeader *, src,
	A1, ULONG *, destWidth,
	A2, ULONG *, destHeight,
	A3, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P3_PROTO(struct BitMap *, LIBScalosGfxScaleBitMap,
	A0, struct ScaleBitMapArg *, sbma,
	A1, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P5_PROTO(VOID, LIBScalosGfxCalculateScaleAspect,
	D0, ULONG, sourceWidth,
	D1, ULONG, sourceHeight,
	A0, ULONG *, destWidth,
	A1, ULONG *, destHeight,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(VOID, LIBScalosGfxBlitARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct ARGBHeader *, SrcARGB,
	D0, LONG, DestLeft,
	D1, LONG, DestTop,
	D2, LONG, SrcLeft,
	D3, LONG, SrcTop,
	D4, LONG, Width,
	D5, LONG, Height,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P7_PROTO(VOID, LIBScalosGfxFillRectARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct gfxARGB *, fillARGB,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P3_PROTO(VOID, LIBScalosGfxSetARGB,
	A0, struct ARGBHeader *, DestARGB,
	A1, const struct gfxARGB *, fillARGB,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P4_PROTO(BOOL, LIBScalosGfxNewColorMap,
	A0, struct ScalosBitMapAndColor *, sac,
	A1, const ULONG *, colorMap,
	D0, ULONG, colorEntries,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P8_PROTO(VOID, LIBScalosGfxARGBRectMult,
	A0, struct RastPort *, rp,
	A1, const struct ARGB *, numerator,
	A2, const struct ARGB *, denominator,
	D0, WORD, xMin,
	D1, WORD, yMin,
	D2, WORD, xMax,
	D3, WORD, yMax,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(VOID, LIBScalosGfxBlitARGBAlpha,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	D2, ULONG, srcLeft,
	D3, ULONG, srcTop,
	D4, ULONG, width,
	D5, ULONG, height,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P7_PROTO(VOID, LIBScalosGfxBlitARGBAlphaTagList,
	A0, struct RastPort *, rp,
	A1, const struct ARGBHeader *, srcH,
	D0, ULONG, destLeft,
	D1, ULONG, destTop,
	A3, const struct IBox *, srcSize,
	A2, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P8_PROTO(VOID, LIBScalosGfxBlitIcon,
	A0, struct RastPort *, rpBackground,
	A1, struct RastPort *, rpIcon,
	D0, ULONG, left,
	D1, ULONG, top,
	D2, ULONG, width,
	D3, ULONG, height,
	A2, struct TagItem *, tagList,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(BOOL, LIBScalosGfxDrawGradient,
	A0, struct ARGBHeader *, dest,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A1, struct gfxARGB *, start,
	A2, struct gfxARGB *, stop,
	D4, ULONG, gradType,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(BOOL, LIBScalosGfxDrawGradientRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, left,
	D1, LONG, top,
	D2, LONG, width,
	D3, LONG, height,
	A1, struct gfxARGB *, start,
	A2, struct gfxARGB *, stop,
	D4, ULONG, gradType,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P7_PROTO(VOID, LIBScalosGfxDrawLine,
	A0, struct ARGBHeader *, dest,
	D0, LONG, fromX,
	D1, LONG, fromY,
	D2, LONG, toX,
	D3, LONG, toY,
	A1, const struct gfxARGB *, lineColor,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P7_PROTO(VOID, LIBScalosGfxDrawLineRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, fromX,
	D1, LONG, fromY,
	D2, LONG, toX,
	D3, LONG, toY,
	A1, const struct gfxARGB *, lineColor,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(VOID, LIBScalosGfxDrawEllipse,
	A0, struct ARGBHeader *, dest,
	D0, LONG, xCenter,
	D1, LONG, yCenter,
	D2, LONG, radiusX,
	D3, LONG, radiusY,
	D4, WORD, segment,
	A1, const struct gfxARGB *, color1,
	A2, const struct gfxARGB *, color2,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
LIBFUNC_P9_PROTO(VOID, LIBScalosGfxDrawEllipseRastPort,
	A0, struct RastPort *, rp,
	D0, LONG, xCenter,
	D1, LONG, yCenter,
	D2, LONG, radiusX,
	D3, LONG, radiusY,
	D4, WORD, segment,
	A1, const struct gfxARGB *, color1,
	A2, const struct gfxARGB *, color2,
	A6, struct ScalosGfxBase *, ScalosGfxBase);
APTR ScalosGfxAllocVecPooled(struct ScalosGfxBase *ScalosGfxBase, ULONG Size);
void ScalosGfxFreeVecPooled(struct ScalosGfxBase *ScalosGfxBase, APTR mem);

//----------------------------------------------------------------------------

// defined in argb.c
///
void ARGBSetAlpha(struct ARGB *argb, ULONG Width, ULONG Height, UBYTE Alpha);
void ARGBSetAlphaFromMask(struct ARGBHeader *argbh, PLANEPTR MaskPlane);
struct ARGB *CreateARGBFromBitMap(struct BitMap *bm,
	ULONG Width, ULONG Height,
	ULONG NumberOfColors, const ULONG *ColorTable, PLANEPTR MaskPlane,
	struct ScalosGfxBase *ScalosGfxBase);
void FreeARGB(struct ARGB **argb, struct ScalosGfxBase *ScalosGfxBase);
struct ARGB *AllocARGB(ULONG Width, ULONG Height, struct ScalosGfxBase *ScalosGfxBase);
void FillARGBFromBitMap(struct ARGBHeader *argbh,
	struct BitMap *srcBM, PLANEPTR MaskPlane);
void WriteARGBToBitMap(struct ARGB *argb, struct BitMap *bm,
	ULONG Width, ULONG Height,
	ULONG NumberOfColors, const ULONG *ColorTable,
        struct ScalosGfxBase *ScalosGfxBase);
struct ScalosBitMapAndColor *AllocEmptySAC(struct ScalosGfxBase *ScalosGfxBase);
struct ScalosBitMapAndColor *AllocSAC(ULONG Width, ULONG Height, ULONG Depth,
	struct BitMap *FriendBM, struct TagItem *TagList,
	struct ScalosGfxBase *ScalosGfxBase);
void FreeSAC(struct ScalosBitMapAndColor *sac, struct ScalosGfxBase *ScalosGfxBase);
void BlitARGB(struct ARGBHeader *DestARGB, const struct ARGBHeader *SrcARGB,
	LONG DestLeft, LONG DestTop, LONG SrcLeft, LONG SrcTop,
	LONG Width, LONG Height);
void FillARGB(struct ARGBHeader *DestARGB, const struct ARGB *fillARGB,
	LONG left, LONG tTop, LONG width, LONG height);
void SetARGB(struct ARGBHeader *DestARGB, const struct ARGB *fillARGB);
BOOL SetNewSacColorMap(struct ScalosBitMapAndColor *sac, const ULONG *colorMap,
	ULONG colorEntries, struct ScalosGfxBase *ScalosGfxBase);
///

//----------------------------------------------------------------------------

// defined in BitMapScale.c
///
struct ARGB *ScaleARGBArray(const struct ARGBHeader *src,
	ULONG *DestWidth, ULONG *DestHeight, ULONG Flags,
        struct ScalosGfxBase *ScalosGfxBase);
struct BitMap *ScaleBitMap(struct BitMap *SourceBM,
	ULONG SourceWidth, ULONG SourceHeight,
	ULONG *DestWidth, ULONG *DestHeight,
	ULONG NumberOfColors, const ULONG *ColorTable,
	ULONG Flags, struct BitMap *ScreenBM,
        struct ScalosGfxBase *ScalosGfxBase);
void CalculateScaleAspect(ULONG SourceWidth, ULONG SourceHeight,
	ULONG *DestWidth, ULONG *DestHeight);
///

//----------------------------------------------------------------------------

// defined in Dither.c
///
struct ScalosBitMapAndColor *MedianCut(struct ARGBHeader *argbh,
	ULONG Depth, ULONG newcolors,
        BOOL floyd, struct BitMap *FriendBM,
	struct ScalosGfxBase *ScalosGfxBase);
///

//----------------------------------------------------------------------------

// defined in Render.c
///
BOOL ScaDrawGradient(struct ARGBHeader *dest, LONG left, LONG top,
	LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop,
	ULONG gradType, struct ScalosGfxBase *ScalosGfxBase);
BOOL DrawGradientRastPort(struct RastPort *rp, LONG left, LONG top,
	LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop,
	ULONG gradType, struct ScalosGfxBase *ScalosGfxBase);
void DrawLine(struct ARGBHeader *argbh, LONG X0, LONG Y0, LONG X1, LONG Y1, struct ARGB Color32);
void DrawLineRastPort(struct RastPort *rp, LONG X0, LONG Y0, LONG X1, LONG Y1, struct ARGB Color32);
void DrawARGBEllipse(struct ARGBHeader *argbh,
	LONG xc, LONG yc, LONG rx, LONG ry,
	WORD Segment, struct ARGB rgbColor1, struct ARGB rgbColor2);
void DrawARGBEllipseRastPort(struct RastPort *rp,
	LONG xc, LONG yc, LONG rx, LONG ry,
	WORD Segment, struct ARGB rgbColor1, struct ARGB rgbColor2);
///

//----------------------------------------------------------------------------

// defined in blit.c
///
void ARGBRectMult(struct RastPort *rp,
	struct ARGB Numerator, struct ARGB Denominator,
	WORD xMin, WORD yMin, WORD xMax, WORD yMax,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitARGBAlpha(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitARGBAlphaKT(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	const struct ARGB *K, ULONG Transparency,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitARGBKT(struct RastPort *rp, const struct ARGBHeader *SrcH,
	ULONG DestLeft, ULONG DestTop,
	ULONG SrcLeft, ULONG SrcTop,
	ULONG Width, ULONG Height,
	const struct ARGB *K, ULONG Transparency,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitTransparentK(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	const struct ARGB *K,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitTransparent(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	ULONG Trans,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitTransparentAlpha(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	ULONG Trans, const UBYTE *Alpha,
	struct ScalosGfxBase *ScalosGfxBase);
void BlitTransparentAlphaK(struct RastPort *rpBackground, struct RastPort *rpIcon,
	ULONG Left, ULONG Top, ULONG Width, ULONG Height,
	const struct ARGB *K, const UBYTE *Alpha,
	struct ScalosGfxBase *ScalosGfxBase);
///
//----------------------------------------------------------------------------

#ifndef __AROS__
extern ULONG HookEntry();
#endif

//----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	{ Forbid(); x; Permit(); }

#ifdef __AROS__
#include <clib/arossupport_protos.h>
#define KPrintF kprintf
#else
// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);
#endif

//----------------------------------------------------------------------------

#endif /* SCALOSGFX_H */
