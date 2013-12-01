/* Automatically generated header! Do not edit! */

#ifndef _INLINE_SCALOSGFX_LIB_SFD_H
#define _INLINE_SCALOSGFX_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef SCALOSGFX_LIB_SFD_BASE_NAME
#define SCALOSGFX_LIB_SFD_BASE_NAME ScalosGfxBase
#endif /* !SCALOSGFX_LIB_SFD_BASE_NAME */

#define ScalosGfxCreateEmptySAC() __ScalosGfxCreateEmptySAC_WB(SCALOSGFX_LIB_SFD_BASE_NAME)
#define __ScalosGfxCreateEmptySAC_WB(___base) \
	AROS_LC0(struct ScalosBitMapAndColor *, ScalosGfxCreateEmptySAC, \
	struct Library *, (___base), 5, Scalosgfx_lib_sfd)

#define ScalosGfxCreateSAC(___width, ___height, ___depth, ___friendBM, ___tagList) __ScalosGfxCreateSAC_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___width, ___height, ___depth, ___friendBM, ___tagList)
#define __ScalosGfxCreateSAC_WB(___base, ___width, ___height, ___depth, ___friendBM, ___tagList) \
	AROS_LC5(struct ScalosBitMapAndColor *, ScalosGfxCreateSAC, \
	AROS_LCA(ULONG, (___width), D0), \
	AROS_LCA(ULONG, (___height), D1), \
	AROS_LCA(ULONG, (___depth), D2), \
	AROS_LCA(struct BitMap *, (___friendBM), A0), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 6, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxCreateSACTags(___width, ___height, ___depth, ___friendBM, ___firstTag, ...) __ScalosGfxCreateSACTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___width, ___height, ___depth, ___friendBM, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxCreateSACTags_WB(___base, ___width, ___height, ___depth, ___friendBM, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxCreateSAC_WB((___base), (___width), (___height), (___depth), (___friendBM), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxFreeSAC(___sac) __ScalosGfxFreeSAC_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___sac)
#define __ScalosGfxFreeSAC_WB(___base, ___sac) \
	AROS_LC1NR(VOID, ScalosGfxFreeSAC, \
	AROS_LCA(struct ScalosBitMapAndColor *, (___sac), A0), \
	struct Library *, (___base), 7, Scalosgfx_lib_sfd)

#define ScalosGfxCreateARGB(___width, ___height, ___tagList) __ScalosGfxCreateARGB_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___width, ___height, ___tagList)
#define __ScalosGfxCreateARGB_WB(___base, ___width, ___height, ___tagList) \
	AROS_LC3(struct gfxARGB *, ScalosGfxCreateARGB, \
	AROS_LCA(ULONG, (___width), D0), \
	AROS_LCA(ULONG, (___height), D1), \
	AROS_LCA(struct TagItem *, (___tagList), A0), \
	struct Library *, (___base), 8, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxCreateARGBTags(___width, ___height, ___firstTag, ...) __ScalosGfxCreateARGBTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___width, ___height, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxCreateARGBTags_WB(___base, ___width, ___height, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxCreateARGB_WB((___base), (___width), (___height), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxFreeARGB(___argb) __ScalosGfxFreeARGB_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argb)
#define __ScalosGfxFreeARGB_WB(___base, ___argb) \
	AROS_LC1NR(VOID, ScalosGfxFreeARGB, \
	AROS_LCA(struct gfxARGB **, (___argb), A0), \
	struct Library *, (___base), 9, Scalosgfx_lib_sfd)

#define ScalosGfxARGBSetAlpha(___src, ___alpha) __ScalosGfxARGBSetAlpha_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___src, ___alpha)
#define __ScalosGfxARGBSetAlpha_WB(___base, ___src, ___alpha) \
	AROS_LC2NR(VOID, ScalosGfxARGBSetAlpha, \
	AROS_LCA(struct ARGBHeader *, (___src), A0), \
	AROS_LCA(UBYTE, (___alpha), D0), \
	struct Library *, (___base), 10, Scalosgfx_lib_sfd)

#define ScalosGfxARGBSetAlphaMask(___argbh, ___maskPlane) __ScalosGfxARGBSetAlphaMask_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argbh, ___maskPlane)
#define __ScalosGfxARGBSetAlphaMask_WB(___base, ___argbh, ___maskPlane) \
	AROS_LC2NR(VOID, ScalosGfxARGBSetAlphaMask, \
	AROS_LCA(struct ARGBHeader *, (___argbh), A0), \
	AROS_LCA(PLANEPTR, (___maskPlane), A1), \
	struct Library *, (___base), 11, Scalosgfx_lib_sfd)

#define ScalosGfxCreateARGBFromBitMap(___bm, ___width, ___height, ___numberOfColors, ___colorTable, ___maskPlane) __ScalosGfxCreateARGBFromBitMap_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___bm, ___width, ___height, ___numberOfColors, ___colorTable, ___maskPlane)
#define __ScalosGfxCreateARGBFromBitMap_WB(___base, ___bm, ___width, ___height, ___numberOfColors, ___colorTable, ___maskPlane) \
	AROS_LC6(struct gfxARGB *, ScalosGfxCreateARGBFromBitMap, \
	AROS_LCA(struct BitMap *, (___bm), A0), \
	AROS_LCA(ULONG, (___width), D0), \
	AROS_LCA(ULONG, (___height), D1), \
	AROS_LCA(ULONG, (___numberOfColors), D2), \
	AROS_LCA(const ULONG *, (___colorTable), A1), \
	AROS_LCA(PLANEPTR, (___maskPlane), A2), \
	struct Library *, (___base), 12, Scalosgfx_lib_sfd)

#define ScalosGfxFillARGBFromBitMap(___argbh, ___srcBM, ___maskPlane) __ScalosGfxFillARGBFromBitMap_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argbh, ___srcBM, ___maskPlane)
#define __ScalosGfxFillARGBFromBitMap_WB(___base, ___argbh, ___srcBM, ___maskPlane) \
	AROS_LC3NR(VOID, ScalosGfxFillARGBFromBitMap, \
	AROS_LCA(struct ARGBHeader *, (___argbh), A0), \
	AROS_LCA(struct BitMap *, (___srcBM), A1), \
	AROS_LCA(PLANEPTR, (___maskPlane), A2), \
	struct Library *, (___base), 13, Scalosgfx_lib_sfd)

#define ScalosGfxWriteARGBToBitMap(___argbh, ___bm, ___numberOfColors, ___colorTable) __ScalosGfxWriteARGBToBitMap_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argbh, ___bm, ___numberOfColors, ___colorTable)
#define __ScalosGfxWriteARGBToBitMap_WB(___base, ___argbh, ___bm, ___numberOfColors, ___colorTable) \
	AROS_LC4NR(VOID, ScalosGfxWriteARGBToBitMap, \
	AROS_LCA(struct ARGBHeader *, (___argbh), A0), \
	AROS_LCA(struct BitMap *, (___bm), A1), \
	AROS_LCA(ULONG, (___numberOfColors), D0), \
	AROS_LCA(const ULONG *, (___colorTable), A2), \
	struct Library *, (___base), 14, Scalosgfx_lib_sfd)

#define ScalosGfxMedianCut(___argbh, ___depth, ___tagList) __ScalosGfxMedianCut_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argbh, ___depth, ___tagList)
#define __ScalosGfxMedianCut_WB(___base, ___argbh, ___depth, ___tagList) \
	AROS_LC3(struct ScalosBitMapAndColor *, ScalosGfxMedianCut, \
	AROS_LCA(struct ARGBHeader *, (___argbh), A0), \
	AROS_LCA(ULONG, (___depth), D0), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 15, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxMedianCutTags(___argbh, ___depth, ___firstTag, ...) __ScalosGfxMedianCutTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___argbh, ___depth, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxMedianCutTags_WB(___base, ___argbh, ___depth, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxMedianCut_WB((___base), (___argbh), (___depth), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxScaleARGBArray(___src, ___destWidth, ___destHeight, ___tagList) __ScalosGfxScaleARGBArray_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___src, ___destWidth, ___destHeight, ___tagList)
#define __ScalosGfxScaleARGBArray_WB(___base, ___src, ___destWidth, ___destHeight, ___tagList) \
	AROS_LC4(struct gfxARGB *, ScalosGfxScaleARGBArray, \
	AROS_LCA(const struct ARGBHeader *, (___src), A0), \
	AROS_LCA(ULONG *, (___destWidth), A1), \
	AROS_LCA(ULONG *, (___destHeight), A2), \
	AROS_LCA(struct TagItem *, (___tagList), A3), \
	struct Library *, (___base), 16, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxScaleARGBArrayTags(___src, ___destWidth, ___destHeight, ___firstTag, ...) __ScalosGfxScaleARGBArrayTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___src, ___destWidth, ___destHeight, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxScaleARGBArrayTags_WB(___base, ___src, ___destWidth, ___destHeight, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxScaleARGBArray_WB((___base), (___src), (___destWidth), (___destHeight), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxScaleBitMap(___sbma, ___tagList) __ScalosGfxScaleBitMap_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___sbma, ___tagList)
#define __ScalosGfxScaleBitMap_WB(___base, ___sbma, ___tagList) \
	AROS_LC2(struct BitMap *, ScalosGfxScaleBitMap, \
	AROS_LCA(struct ScaleBitMapArg *, (___sbma), A0), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 17, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxScaleBitMapTags(___sbma, ___firstTag, ...) __ScalosGfxScaleBitMapTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___sbma, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxScaleBitMapTags_WB(___base, ___sbma, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxScaleBitMap_WB((___base), (___sbma), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxCalculateScaleAspect(___sourceWidth, ___sourceHeight, ___destWidth, ___destHeight) __ScalosGfxCalculateScaleAspect_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___sourceWidth, ___sourceHeight, ___destWidth, ___destHeight)
#define __ScalosGfxCalculateScaleAspect_WB(___base, ___sourceWidth, ___sourceHeight, ___destWidth, ___destHeight) \
	AROS_LC4NR(VOID, ScalosGfxCalculateScaleAspect, \
	AROS_LCA(ULONG, (___sourceWidth), D0), \
	AROS_LCA(ULONG, (___sourceHeight), D1), \
	AROS_LCA(ULONG *, (___destWidth), A0), \
	AROS_LCA(ULONG *, (___destHeight), A1), \
	struct Library *, (___base), 18, Scalosgfx_lib_sfd)

#define ScalosGfxBlitARGB(___destARGB, ___srcARGB, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height) __ScalosGfxBlitARGB_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___destARGB, ___srcARGB, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height)
#define __ScalosGfxBlitARGB_WB(___base, ___destARGB, ___srcARGB, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height) \
	AROS_LC8NR(VOID, ScalosGfxBlitARGB, \
	AROS_LCA(struct ARGBHeader *, (___destARGB), A0), \
	AROS_LCA(const struct ARGBHeader *, (___srcARGB), A1), \
	AROS_LCA(LONG, (___destLeft), D0), \
	AROS_LCA(LONG, (___destTop), D1), \
	AROS_LCA(LONG, (___srcLeft), D2), \
	AROS_LCA(LONG, (___srcTop), D3), \
	AROS_LCA(LONG, (___width), D4), \
	AROS_LCA(LONG, (___height), D5), \
	struct Library *, (___base), 19, Scalosgfx_lib_sfd)

#define ScalosGfxFillRectARGB(___destARGB, ___fillARGB, ___left, ___top, ___width, ___height) __ScalosGfxFillRectARGB_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___destARGB, ___fillARGB, ___left, ___top, ___width, ___height)
#define __ScalosGfxFillRectARGB_WB(___base, ___destARGB, ___fillARGB, ___left, ___top, ___width, ___height) \
	AROS_LC6NR(VOID, ScalosGfxFillRectARGB, \
	AROS_LCA(struct ARGBHeader *, (___destARGB), A0), \
	AROS_LCA(const struct gfxARGB *, (___fillARGB), A1), \
	AROS_LCA(LONG, (___left), D0), \
	AROS_LCA(LONG, (___top), D1), \
	AROS_LCA(LONG, (___width), D2), \
	AROS_LCA(LONG, (___height), D3), \
	struct Library *, (___base), 20, Scalosgfx_lib_sfd)

#define ScalosGfxSetARGB(___destARGB, ___fillARGB) __ScalosGfxSetARGB_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___destARGB, ___fillARGB)
#define __ScalosGfxSetARGB_WB(___base, ___destARGB, ___fillARGB) \
	AROS_LC2NR(VOID, ScalosGfxSetARGB, \
	AROS_LCA(struct ARGBHeader *, (___destARGB), A0), \
	AROS_LCA(const struct gfxARGB *, (___fillARGB), A1), \
	struct Library *, (___base), 21, Scalosgfx_lib_sfd)

#define ScalosGfxNewColorMap(___sac, ___colorMap, ___colorEntries) __ScalosGfxNewColorMap_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___sac, ___colorMap, ___colorEntries)
#define __ScalosGfxNewColorMap_WB(___base, ___sac, ___colorMap, ___colorEntries) \
	AROS_LC3(BOOL, ScalosGfxNewColorMap, \
	AROS_LCA(struct ScalosBitMapAndColor *, (___sac), A0), \
	AROS_LCA(const ULONG *, (___colorMap), A1), \
	AROS_LCA(ULONG, (___colorEntries), D0), \
	struct Library *, (___base), 22, Scalosgfx_lib_sfd)

#define ScalosGfxARGBRectMult(___rp, ___numerator, ___denominator, ___xMin, ___yMin, ___xMax, ___yMax) __ScalosGfxARGBRectMult_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___numerator, ___denominator, ___xMin, ___yMin, ___xMax, ___yMax)
#define __ScalosGfxARGBRectMult_WB(___base, ___rp, ___numerator, ___denominator, ___xMin, ___yMin, ___xMax, ___yMax) \
	AROS_LC7NR(VOID, ScalosGfxARGBRectMult, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(const struct gfxARGB *, (___numerator), A1), \
	AROS_LCA(const struct gfxARGB *, (___denominator), A2), \
	AROS_LCA(WORD, (___xMin), D0), \
	AROS_LCA(WORD, (___yMin), D1), \
	AROS_LCA(WORD, (___xMax), D2), \
	AROS_LCA(WORD, (___yMax), D3), \
	struct Library *, (___base), 23, Scalosgfx_lib_sfd)

#define ScalosGfxBlitARGBAlpha(___rp, ___srcH, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height) __ScalosGfxBlitARGBAlpha_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___srcH, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height)
#define __ScalosGfxBlitARGBAlpha_WB(___base, ___rp, ___srcH, ___destLeft, ___destTop, ___srcLeft, ___srcTop, ___width, ___height) \
	AROS_LC8NR(VOID, ScalosGfxBlitARGBAlpha, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(const struct ARGBHeader *, (___srcH), A1), \
	AROS_LCA(ULONG, (___destLeft), D0), \
	AROS_LCA(ULONG, (___destTop), D1), \
	AROS_LCA(ULONG, (___srcLeft), D2), \
	AROS_LCA(ULONG, (___srcTop), D3), \
	AROS_LCA(ULONG, (___width), D4), \
	AROS_LCA(ULONG, (___height), D5), \
	struct Library *, (___base), 24, Scalosgfx_lib_sfd)

#define ScalosGfxBlitARGBAlphaTagList(___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___tagList) __ScalosGfxBlitARGBAlphaTagList_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___tagList)
#define __ScalosGfxBlitARGBAlphaTagList_WB(___base, ___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___tagList) \
	AROS_LC6NR(VOID, ScalosGfxBlitARGBAlphaTagList, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(const struct ARGBHeader *, (___srcH), A1), \
	AROS_LCA(ULONG, (___destLeft), D0), \
	AROS_LCA(ULONG, (___destTop), D1), \
	AROS_LCA(const struct IBox *, (___srcSize), A3), \
	AROS_LCA(struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 25, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxBlitARGBAlphaTags(___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___firstTag, ...) __ScalosGfxBlitARGBAlphaTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxBlitARGBAlphaTags_WB(___base, ___rp, ___srcH, ___destLeft, ___destTop, ___srcSize, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxBlitARGBAlphaTagList_WB((___base), (___rp), (___srcH), (___destLeft), (___destTop), (___srcSize), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxBlitIcon(___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___tagList) __ScalosGfxBlitIcon_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___tagList)
#define __ScalosGfxBlitIcon_WB(___base, ___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___tagList) \
	AROS_LC7NR(VOID, ScalosGfxBlitIcon, \
	AROS_LCA(struct RastPort *, (___rpBackground), A0), \
	AROS_LCA(struct RastPort *, (___rpIcon), A1), \
	AROS_LCA(ULONG, (___left), D0), \
	AROS_LCA(ULONG, (___top), D1), \
	AROS_LCA(ULONG, (___width), D2), \
	AROS_LCA(ULONG, (___height), D3), \
	AROS_LCA(struct TagItem *, (___tagList), A2), \
	struct Library *, (___base), 26, Scalosgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define ScalosGfxBlitIconTags(___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___firstTag, ...) __ScalosGfxBlitIconTags_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___firstTag, ## __VA_ARGS__)
#define __ScalosGfxBlitIconTags_WB(___base, ___rpBackground, ___rpIcon, ___left, ___top, ___width, ___height, ___firstTag, ...) \
	({IPTR _tags[] = { (IPTR) ___firstTag, ## __VA_ARGS__ }; __ScalosGfxBlitIcon_WB((___base), (___rpBackground), (___rpIcon), (___left), (___top), (___width), (___height), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define ScalosGfxDrawGradient(___dest, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType) __ScalosGfxDrawGradient_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___dest, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType)
#define __ScalosGfxDrawGradient_WB(___base, ___dest, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType) \
	AROS_LC8(BOOL, ScalosGfxDrawGradient, \
	AROS_LCA(struct ARGBHeader *, (___dest), A0), \
	AROS_LCA(LONG, (___left), D0), \
	AROS_LCA(LONG, (___top), D1), \
	AROS_LCA(LONG, (___width), D2), \
	AROS_LCA(LONG, (___height), D3), \
	AROS_LCA(struct gfxARGB *, (___start), A1), \
	AROS_LCA(struct gfxARGB *, (___stop), A2), \
	AROS_LCA(ULONG, (___gradType), D4), \
	struct Library *, (___base), 27, Scalosgfx_lib_sfd)

#define ScalosGfxDrawGradientRastPort(___rp, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType) __ScalosGfxDrawGradientRastPort_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType)
#define __ScalosGfxDrawGradientRastPort_WB(___base, ___rp, ___left, ___top, ___width, ___height, ___start, ___stop, ___gradType) \
	AROS_LC8(BOOL, ScalosGfxDrawGradientRastPort, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(LONG, (___left), D0), \
	AROS_LCA(LONG, (___top), D1), \
	AROS_LCA(LONG, (___width), D2), \
	AROS_LCA(LONG, (___height), D3), \
	AROS_LCA(struct gfxARGB *, (___start), A1), \
	AROS_LCA(struct gfxARGB *, (___stop), A2), \
	AROS_LCA(ULONG, (___gradType), D4), \
	struct Library *, (___base), 28, Scalosgfx_lib_sfd)

#define ScalosGfxDrawLine(___dest, ___fromX, ___fromY, ___toX, ___toY, ___lineColor) __ScalosGfxDrawLine_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___dest, ___fromX, ___fromY, ___toX, ___toY, ___lineColor)
#define __ScalosGfxDrawLine_WB(___base, ___dest, ___fromX, ___fromY, ___toX, ___toY, ___lineColor) \
	AROS_LC6NR(VOID, ScalosGfxDrawLine, \
	AROS_LCA(struct ARGBHeader *, (___dest), A0), \
	AROS_LCA(LONG, (___fromX), D0), \
	AROS_LCA(LONG, (___fromY), D1), \
	AROS_LCA(LONG, (___toX), D2), \
	AROS_LCA(LONG, (___toY), D3), \
	AROS_LCA(const struct gfxARGB *, (___lineColor), A1), \
	struct Library *, (___base), 29, Scalosgfx_lib_sfd)

#define ScalosGfxDrawLineRastPort(___rp, ___fromX, ___fromY, ___toX, ___toY, ___lineColor) __ScalosGfxDrawLineRastPort_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___fromX, ___fromY, ___toX, ___toY, ___lineColor)
#define __ScalosGfxDrawLineRastPort_WB(___base, ___rp, ___fromX, ___fromY, ___toX, ___toY, ___lineColor) \
	AROS_LC6NR(VOID, ScalosGfxDrawLineRastPort, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(LONG, (___fromX), D0), \
	AROS_LCA(LONG, (___fromY), D1), \
	AROS_LCA(LONG, (___toX), D2), \
	AROS_LCA(LONG, (___toY), D3), \
	AROS_LCA(const struct gfxARGB *, (___lineColor), A1), \
	struct Library *, (___base), 30, Scalosgfx_lib_sfd)

#define ScalosGfxDrawEllipse(___dest, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2) __ScalosGfxDrawEllipse_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___dest, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2)
#define __ScalosGfxDrawEllipse_WB(___base, ___dest, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2) \
	AROS_LC8NR(VOID, ScalosGfxDrawEllipse, \
	AROS_LCA(struct ARGBHeader *, (___dest), A0), \
	AROS_LCA(LONG, (___xCenter), D0), \
	AROS_LCA(LONG, (___yCenter), D1), \
	AROS_LCA(LONG, (___radiusX), D2), \
	AROS_LCA(LONG, (___radiusy), D3), \
	AROS_LCA(WORD, (___segment), D4), \
	AROS_LCA(const struct gfxARGB *, (___color1), A1), \
	AROS_LCA(const struct gfxARGB *, (___color2), A2), \
	struct Library *, (___base), 31, Scalosgfx_lib_sfd)

#define ScalosGfxDrawEllipseRastPort(___rp, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2) __ScalosGfxDrawEllipseRastPort_WB(SCALOSGFX_LIB_SFD_BASE_NAME, ___rp, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2)
#define __ScalosGfxDrawEllipseRastPort_WB(___base, ___rp, ___xCenter, ___yCenter, ___radiusX, ___radiusy, ___segment, ___color1, ___color2) \
	AROS_LC8NR(VOID, ScalosGfxDrawEllipseRastPort, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(LONG, (___xCenter), D0), \
	AROS_LCA(LONG, (___yCenter), D1), \
	AROS_LCA(LONG, (___radiusX), D2), \
	AROS_LCA(LONG, (___radiusy), D3), \
	AROS_LCA(WORD, (___segment), D4), \
	AROS_LCA(const struct gfxARGB *, (___color1), A1), \
	AROS_LCA(const struct gfxARGB *, (___color2), A2), \
	struct Library *, (___base), 32, Scalosgfx_lib_sfd)

#endif /* !_INLINE_SCALOSGFX_LIB_SFD_H */
