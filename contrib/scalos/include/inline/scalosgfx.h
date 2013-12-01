#ifndef _INLINE_SCALOSGFX_H
#define _INLINE_SCALOSGFX_H

#ifndef CLIB_SCALOSGFX_PROTOS_H
#define CLIB_SCALOSGFX_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

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

#ifndef SCALOSGFX_BASE_NAME
#define SCALOSGFX_BASE_NAME ScalosGfxBase
#endif

#define ScalosGfxCreateEmptySAC() \
	LP0(0x1e, struct ScalosBitMapAndColor *, ScalosGfxCreateEmptySAC, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxCreateSAC(width, height, depth, friendBM, tagList) \
	LP5(0x24, struct ScalosBitMapAndColor *, ScalosGfxCreateSAC, ULONG, width, d0, ULONG, height, d1, ULONG, depth, d2, struct BitMap *, friendBM, a0, struct TagItem *, tagList, a1, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxCreateSACTags(width, height, depth, friendBM, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxCreateSAC((width), (height), (depth), (friendBM), (struct TagItem *) _tags);})
#endif

#define ScalosGfxFreeSAC(sac) \
	LP1NR(0x2a, ScalosGfxFreeSAC, struct ScalosBitMapAndColor *, sac, a0, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxCreateARGB(width, height, tagList) \
	LP3(0x30, struct gfxARGB *, ScalosGfxCreateARGB, ULONG, width, d0, ULONG, height, d1, struct TagItem *, tagList, a0, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxCreateARGBTags(width, height, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxCreateARGB((width), (height), (struct TagItem *) _tags);})
#endif

#define ScalosGfxFreeARGB(argb) \
	LP1NR(0x36, ScalosGfxFreeARGB, struct gfxARGB **, argb, a0, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxARGBSetAlpha(src, alpha) \
	LP2NR(0x3c, ScalosGfxARGBSetAlpha, struct ARGBHeader *, src, a0, ULONG, alpha, d0, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxARGBSetAlphaMask(argbh, maskPlane) \
	LP2NR(0x42, ScalosGfxARGBSetAlphaMask, struct ARGBHeader *, argbh, a0, PLANEPTR, maskPlane, a1, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxCreateARGBFromBitMap(bm, width, height, numberOfColors, colorTable, maskPlane) \
	LP6(0x48, struct gfxARGB *, ScalosGfxCreateARGBFromBitMap, struct BitMap *, bm, a0, ULONG, width, d0, ULONG, height, d1, ULONG, numberOfColors, d2, const ULONG *, colorTable, a1, PLANEPTR, maskPlane, a2, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxFillARGBFromBitMap(argbh, srcBM, maskPlane) \
	LP3NR(0x4e, ScalosGfxFillARGBFromBitMap, struct ARGBHeader *, argbh, a0, struct BitMap *, srcBM, a1, PLANEPTR, maskPlane, a2, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxWriteARGBToBitMap(argbh, bm, numberOfColors, colorTable) \
	LP4NR(0x54, ScalosGfxWriteARGBToBitMap, struct ARGBHeader *, argbh, a0, struct BitMap *, bm, a1, ULONG, numberOfColors, d0, const ULONG *, colorTable, a2, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxMedianCut(argbh, depth, tagList) \
	LP3(0x5a, struct ScalosBitMapAndColor *, ScalosGfxMedianCut, struct ARGBHeader *, argbh, a0, ULONG, depth, d0, struct TagItem *, tagList, a1, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxMedianCutTags(argbh, depth, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxMedianCut((argbh), (depth), (struct TagItem *) _tags);})
#endif

#define ScalosGfxScaleARGBArray(src, destWidth, destHeight, tagList) \
	LP4(0x60, struct gfxARGB *, ScalosGfxScaleARGBArray, const struct ARGBHeader *, src, a0, ULONG *, destWidth, a1, ULONG *, destHeight, a2, struct TagItem *, tagList, a3, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxScaleARGBArrayTags(src, destWidth, destHeight, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxScaleARGBArray((src), (destWidth), (destHeight), (struct TagItem *) _tags);})
#endif

#define ScalosGfxScaleBitMap(sbma, tagList) \
	LP2(0x66, struct BitMap *, ScalosGfxScaleBitMap, struct ScaleBitMapArg *, sbma, a0, struct TagItem *, tagList, a1, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxScaleBitMapTags(sbma, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxScaleBitMap((sbma), (struct TagItem *) _tags);})
#endif

#define ScalosGfxCalculateScaleAspect(sourceWidth, sourceHeight, destWidth, destHeight) \
	LP4NR(0x6c, ScalosGfxCalculateScaleAspect, ULONG, sourceWidth, d0, ULONG, sourceHeight, d1, ULONG *, destWidth, a0, ULONG *, destHeight, a1, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxBlitARGB(destARGB, srcARGB, destLeft, destTop, srcLeft, srcTop, width, height) \
	LP8NR(0x72, ScalosGfxBlitARGB, struct ARGBHeader *, destARGB, a0, const struct ARGBHeader *, srcARGB, a1, LONG, destLeft, d0, LONG, destTop, d1, LONG, srcLeft, d2, LONG, srcTop, d3, LONG, width, d4, LONG, height, d5, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxFillRectARGB(destARGB, fillARGB, left, top, width, height) \
	LP6NR(0x78, ScalosGfxFillRectARGB, struct ARGBHeader *, destARGB, a0, const struct gfxARGB *, fillARGB, a1, LONG, left, d0, LONG, top, d1, LONG, width, d2, LONG, height, d3, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxSetARGB(destARGB, fillARGB) \
	LP2NR(0x7e, ScalosGfxSetARGB, struct ARGBHeader *, destARGB, a0, const struct gfxARGB *, fillARGB, a1, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxNewColorMap(sac, colorMap, colorEntries) \
	LP3(0x84, BOOL, ScalosGfxNewColorMap, struct ScalosBitMapAndColor *, sac, a0, const ULONG *, colorMap, a1, ULONG, colorEntries, d0, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxARGBRectMult(rp, numerator, denominator, xMin, yMin, xMax, yMax) \
	LP7NR(0x8a, ScalosGfxARGBRectMult, struct RastPort *, rp, a0, const struct gfxARGB *, numerator, a1, const struct gfxARGB *, denominator, a2, LONG, xMin, d0, LONG, yMin, d1, LONG, xMax, d2, LONG, yMax, d3, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxBlitARGBAlpha(rp, srcH, destLeft, destTop, srcLeft, srcTop, width, height) \
	LP8NR(0x90, ScalosGfxBlitARGBAlpha, struct RastPort *, rp, a0, const struct ARGBHeader *, srcH, a1, ULONG, destLeft, d0, ULONG, destTop, d1, ULONG, srcLeft, d2, ULONG, srcTop, d3, ULONG, width, d4, ULONG, height, d5, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxBlitARGBAlphaTagList(rp, srcH, destLeft, destTop, srcSize, tagList) \
	LP6NR(0x96, ScalosGfxBlitARGBAlphaTagList, struct RastPort *, rp, a0, const struct ARGBHeader *, srcH, a1, ULONG, destLeft, d0, ULONG, destTop, d1, const struct IBox *, srcSize, a3, struct TagItem *, tagList, a2, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxBlitARGBAlphaTags(rp, srcH, destLeft, destTop, srcSize, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxBlitARGBAlphaTagList((rp), (srcH), (destLeft), (destTop), (srcSize), (struct TagItem *) _tags);})
#endif

#define ScalosGfxBlitIcon(rpBackground, rpIcon, left, top, width, height, tagList) \
	LP7NR(0x9c, ScalosGfxBlitIcon, struct RastPort *, rpBackground, a0, struct RastPort *, rpIcon, a1, ULONG, left, d0, ULONG, top, d1, ULONG, width, d2, ULONG, height, d3, struct TagItem *, tagList, a2, \
	, SCALOSGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ScalosGfxBlitIconTags(rpBackground, rpIcon, left, top, width, height, tags...) \
	({ULONG _tags[] = {tags}; ScalosGfxBlitIcon((rpBackground), (rpIcon), (left), (top), (width), (height), (struct TagItem *) _tags);})
#endif

#define ScalosGfxDrawGradient(dest, left, top, width, height, start, stop, gradType) \
	LP8(0xa2, BOOL, ScalosGfxDrawGradient, struct ARGBHeader *, dest, a0, LONG, left, d0, LONG, top, d1, LONG, width, d2, LONG, height, d3, struct gfxARGB *, start, a1, struct gfxARGB *, stop, a2, ULONG, gradType, d4, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxDrawGradientRastPort(rp, left, top, width, height, start, stop, gradType) \
	LP8(0xa8, BOOL, ScalosGfxDrawGradientRastPort, struct RastPort *, rp, a0, LONG, left, d0, LONG, top, d1, LONG, width, d2, LONG, height, d3, struct gfxARGB *, start, a1, struct gfxARGB *, stop, a2, ULONG, gradType, d4, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxDrawLine(dest, fromX, fromY, toX, toY, lineColor) \
	LP6NR(0xae, ScalosGfxDrawLine, struct ARGBHeader *, dest, a0, LONG, fromX, d0, LONG, fromY, d1, LONG, toX, d2, LONG, toY, d3, const struct gfxARGB *, lineColor, a1, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxDrawLineRastPort(rp, fromX, fromY, toX, toY, lineColor) \
	LP6NR(0xb4, ScalosGfxDrawLineRastPort, struct RastPort *, rp, a0, LONG, fromX, d0, LONG, fromY, d1, LONG, toX, d2, LONG, toY, d3, const struct gfxARGB *, lineColor, a1, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxDrawEllipse(dest, xCenter, yCenter, radiusX, radiusy, segment, color1, color2) \
	LP8NR(0xba, ScalosGfxDrawEllipse, struct ARGBHeader *, dest, a0, LONG, xCenter, d0, LONG, yCenter, d1, LONG, radiusX, d2, LONG, radiusy, d3, LONG, segment, d4, const struct gfxARGB *, color1, a1, const struct gfxARGB *, color2, a2, \
	, SCALOSGFX_BASE_NAME)

#define ScalosGfxDrawEllipseRastPort(rp, xCenter, yCenter, radiusX, radiusy, segment, color1, color2) \
	LP8NR(0xc0, ScalosGfxDrawEllipseRastPort, struct RastPort *, rp, a0, LONG, xCenter, d0, LONG, yCenter, d1, LONG, radiusX, d2, LONG, radiusy, d3, LONG, segment, d4, const struct gfxARGB *, color1, a1, const struct gfxARGB *, color2, a2, \
	, SCALOSGFX_BASE_NAME)

#endif /*  _INLINE_SCALOSGFX_H  */
