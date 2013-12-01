#ifndef CLIB_SCALOSGFX_PROTOS_H
#define CLIB_SCALOSGFX_PROTOS_H


/*
**	$VER: scalosgfx_protos.h 5.0 (13.09.2009)
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2009 ©2006 The Scalos Team
**	All Rights Reserved
*/

#ifndef  INTUITION_CLASSUSR_H
#include <intuition/classusr.h>
#endif
#ifndef  SCALOS_SCALOS_H
#include <scalos/scalos.h>
#endif
#ifndef  GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif
#ifndef  SCALOS_SCALOSGFX_H
#include <scalos/scalosgfx.h>
#endif

struct ScalosBitMapAndColor * ScalosGfxCreateEmptySAC(void);
struct ScalosBitMapAndColor * ScalosGfxCreateSAC(ULONG width, ULONG height, ULONG depth,
	struct BitMap * friendBM, struct TagItem * tagList);
struct ScalosBitMapAndColor * ScalosGfxCreateSACTags(ULONG width, ULONG height, ULONG depth,
	struct BitMap * friendBM, ULONG tagList, ...);
VOID ScalosGfxFreeSAC(struct ScalosBitMapAndColor * sac);
struct gfxARGB * ScalosGfxCreateARGB(ULONG width, ULONG height, struct TagItem * tagList);
struct gfxARGB * ScalosGfxCreateARGBTags(ULONG width, ULONG height, ULONG tagList, ...);
VOID ScalosGfxFreeARGB(struct gfxARGB ** argb);
VOID ScalosGfxARGBSetAlpha(struct ARGBHeader * src, ULONG alpha);
VOID ScalosGfxARGBSetAlphaMask(struct ARGBHeader * argbh, PLANEPTR maskPlane);
struct gfxARGB * ScalosGfxCreateARGBFromBitMap(struct BitMap * bm, ULONG width, ULONG height,
	ULONG numberOfColors, const ULONG * colorTable, PLANEPTR maskPlane);
VOID ScalosGfxFillARGBFromBitMap(struct ARGBHeader * argbh, struct BitMap * srcBM, PLANEPTR maskPlane);
VOID ScalosGfxWriteARGBToBitMap(struct ARGBHeader * argbh, struct BitMap * bm, ULONG numberOfColors,
	const ULONG * colorTable);
struct ScalosBitMapAndColor * ScalosGfxMedianCut(struct ARGBHeader * argbh, ULONG depth,
	struct TagItem * tagList);
struct ScalosBitMapAndColor * ScalosGfxMedianCutTags(struct ARGBHeader * argbh, ULONG depth,
	ULONG tagList, ...);
struct gfxARGB * ScalosGfxScaleARGBArray(const struct ARGBHeader * src, ULONG * destWidth,
	ULONG * destHeight, struct TagItem * tagList);
struct gfxARGB * ScalosGfxScaleARGBArrayTags(const struct ARGBHeader * src, ULONG * destWidth,
	ULONG * destHeight, ULONG tagList, ...);
struct BitMap * ScalosGfxScaleBitMap(struct ScaleBitMapArg * sbma, struct TagItem * tagList);
struct BitMap * ScalosGfxScaleBitMapTags(struct ScaleBitMapArg * sbma, ULONG tagList, ...);
VOID ScalosGfxCalculateScaleAspect(ULONG sourceWidth, ULONG sourceHeight, ULONG * destWidth,
	ULONG * destHeight);
VOID ScalosGfxBlitARGB(struct ARGBHeader * destARGB, const struct ARGBHeader * srcARGB,
	LONG destLeft, LONG destTop, LONG srcLeft, LONG srcTop, LONG width,
	LONG height);
VOID ScalosGfxFillRectARGB(struct ARGBHeader * destARGB, const struct gfxARGB * fillARGB,
	LONG left, LONG top, LONG width, LONG height);
VOID ScalosGfxSetARGB(struct ARGBHeader * destARGB, const struct gfxARGB * fillARGB);
BOOL ScalosGfxNewColorMap(struct ScalosBitMapAndColor * sac, const ULONG * colorMap,
	ULONG colorEntries);
VOID ScalosGfxARGBRectMult(struct RastPort * rp, const struct gfxARGB * numerator,
	const struct gfxARGB * denominator, LONG xMin, LONG yMin, LONG xMax,
	LONG yMax);
VOID ScalosGfxBlitARGBAlpha(struct RastPort * rp, const struct ARGBHeader * srcH, ULONG destLeft,
	ULONG destTop, ULONG srcLeft, ULONG srcTop, ULONG width,
	ULONG height);
VOID ScalosGfxBlitARGBAlphaTagList(struct RastPort * rp, const struct ARGBHeader * srcH, ULONG destLeft,
	ULONG destTop, const struct IBox * srcSize,
	struct TagItem * tagList);
VOID ScalosGfxBlitARGBAlphaTags(struct RastPort * rp, const struct ARGBHeader * srcH, ULONG destLeft,
	ULONG destTop, const struct IBox * srcSize, ULONG tagList, ...);
VOID ScalosGfxBlitIcon(struct RastPort * rpBackground, struct RastPort * rpIcon, ULONG left,
	ULONG top, ULONG width, ULONG height, struct TagItem * tagList);
VOID ScalosGfxBlitIconTags(struct RastPort * rpBackground, struct RastPort * rpIcon, ULONG left,
	ULONG top, ULONG width, ULONG height, ULONG tagList, ...);
BOOL ScalosGfxDrawGradient(struct ARGBHeader * dest, LONG left, LONG top, LONG width, LONG height,
	struct gfxARGB * start, struct gfxARGB * stop, ULONG gradType);
BOOL ScalosGfxDrawGradientRastPort(struct RastPort * rp, LONG left, LONG top, LONG width, LONG height,
	struct gfxARGB * start, struct gfxARGB * stop, ULONG gradType);
VOID ScalosGfxDrawLine(struct ARGBHeader * dest, LONG fromX, LONG fromY, LONG toX, LONG toY,
	const struct gfxARGB * lineColor);
VOID ScalosGfxDrawLineRastPort(struct RastPort * rp, LONG fromX, LONG fromY, LONG toX, LONG toY,
	const struct gfxARGB * lineColor);
VOID ScalosGfxDrawEllipse(struct ARGBHeader * dest, LONG xCenter, LONG yCenter, LONG radiusX,
	LONG radiusy, LONG segment, const struct gfxARGB * color1,
	const struct gfxARGB * color2);
VOID ScalosGfxDrawEllipseRastPort(struct RastPort * rp, LONG xCenter, LONG yCenter, LONG radiusX,
	LONG radiusy, LONG segment, const struct gfxARGB * color1,
	const struct gfxARGB * color2);

#endif	/*  CLIB_SCALOSGFX_PROTOS_H  */
