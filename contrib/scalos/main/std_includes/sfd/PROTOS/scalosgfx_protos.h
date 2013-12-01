#ifndef  CLIB_SCALOSGFX_PROTOS_H
#define  CLIB_SCALOSGFX_PROTOS_H

/*
**	$Id$
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
struct ScalosBitMapAndColor *ScalosGfxCreateEmptySAC( VOID );
struct ScalosBitMapAndColor *ScalosGfxCreateSAC( ULONG width, ULONG height, ULONG depth, struct BitMap *friendBM, struct TagItem *tagList );
struct ScalosBitMapAndColor *ScalosGfxCreateSACTags( ULONG width, ULONG height, ULONG depth, struct BitMap *friendBM, ULONG firstTag, ... );
VOID ScalosGfxFreeSAC( struct ScalosBitMapAndColor *sac );
struct gfxARGB *ScalosGfxCreateARGB( ULONG width, ULONG height, struct TagItem *tagList );
struct gfxARGB *ScalosGfxCreateARGBTags( ULONG width, ULONG height, ULONG firstTag, ... );
VOID ScalosGfxFreeARGB( struct gfxARGB **argb );
VOID ScalosGfxARGBSetAlpha( struct ARGBHeader *src, ULONG alpha );
VOID ScalosGfxARGBSetAlphaMask( struct ARGBHeader *argbh, PLANEPTR maskPlane );
struct gfxARGB *ScalosGfxCreateARGBFromBitMap( struct BitMap *bm, ULONG width, ULONG height, ULONG numberOfColors, CONST ULONG *colorTable, PLANEPTR maskPlane );
VOID ScalosGfxFillARGBFromBitMap( struct ARGBHeader *argbh, struct BitMap *srcBM, PLANEPTR maskPlane );
VOID ScalosGfxWriteARGBToBitMap( struct ARGBHeader *argbh, struct BitMap *bm, ULONG numberOfColors, CONST ULONG *colorTable );
struct ScalosBitMapAndColor *ScalosGfxMedianCut( struct ARGBHeader *argbh, ULONG depth, struct TagItem *tagList );
struct ScalosBitMapAndColor *ScalosGfxMedianCutTags( struct ARGBHeader *argbh, ULONG depth, ULONG firstTag, ... );
struct gfxARGB *ScalosGfxScaleARGBArray( CONST struct ARGBHeader *src, ULONG *destWidth, ULONG *destHeight, struct TagItem *tagList );
struct gfxARGB *ScalosGfxScaleARGBArrayTags( CONST struct ARGBHeader *src, ULONG *destWidth, ULONG *destHeight, ULONG firstTag, ... );
struct BitMap *ScalosGfxScaleBitMap( struct ScaleBitMapArg *sbma, struct TagItem *tagList );
struct BitMap *ScalosGfxScaleBitMapTags( struct ScaleBitMapArg *sbma, ULONG firstTag, ... );
VOID ScalosGfxCalculateScaleAspect( ULONG sourceWidth, ULONG sourceHeight, ULONG *destWidth, ULONG *destHeight );
VOID ScalosGfxBlitARGB( struct ARGBHeader *destARGB, CONST struct ARGBHeader *srcARGB, LONG destLeft, LONG destTop, LONG srcLeft, LONG srcTop, LONG width, LONG height );
VOID ScalosGfxFillRectARGB( struct ARGBHeader *destARGB, CONST struct gfxARGB *fillARGB, LONG left, LONG top, LONG width, LONG height );
VOID ScalosGfxSetARGB( struct ARGBHeader *destARGB, CONST struct gfxARGB *fillARGB );
BOOL ScalosGfxNewColorMap( struct ScalosBitMapAndColor *sac, CONST ULONG *colorMap, ULONG colorEntries );
VOID ScalosGfxARGBRectMult( struct RastPort *rp, CONST struct gfxARGB *numerator, CONST struct gfxARGB *denominator, LONG xMin, LONG yMin, LONG xMax, LONG yMax );
VOID ScalosGfxBlitARGBAlpha( struct RastPort *rp, CONST struct ARGBHeader *srcH, ULONG destLeft, ULONG destTop, ULONG srcLeft, ULONG srcTop, ULONG width, ULONG height );
VOID ScalosGfxBlitARGBAlphaTagList( struct RastPort *rp, CONST struct ARGBHeader *srcH, ULONG destLeft, ULONG destTop, CONST struct IBox *srcSize, struct TagItem *tagList );
VOID ScalosGfxBlitARGBAlphaTags( struct RastPort *rp, CONST struct ARGBHeader *srcH, ULONG destLeft, ULONG destTop, CONST struct IBox *srcSize, ULONG firstTag, ... );
VOID ScalosGfxBlitIcon( struct RastPort *rpBackground, struct RastPort *rpIcon, ULONG left, ULONG top, ULONG width, ULONG height, struct TagItem *tagList );
VOID ScalosGfxBlitIconTags( struct RastPort *rpBackground, struct RastPort *rpIcon, ULONG left, ULONG top, ULONG width, ULONG height, ULONG firstTag, ... );
/* --- function in v42 or higher */
BOOL ScalosGfxDrawGradient( struct ARGBHeader *dest, LONG left, LONG top, LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop, ULONG gradType );
BOOL ScalosGfxDrawGradientRastPort( struct RastPort *rp, LONG left, LONG top, LONG width, LONG height, struct gfxARGB *start, struct gfxARGB *stop, ULONG gradType );
VOID ScalosGfxDrawLine( struct ARGBHeader *dest, LONG fromX, LONG fromY, LONG toX, LONG toY, CONST struct gfxARGB *lineColor );
VOID ScalosGfxDrawLineRastPort( struct RastPort *rp, LONG fromX, LONG fromY, LONG toX, LONG toY, CONST struct gfxARGB *lineColor );
VOID ScalosGfxDrawEllipse( struct ARGBHeader *dest, LONG xCenter, LONG yCenter, LONG radiusX, LONG radiusy, LONG segment, CONST struct gfxARGB *color1, CONST struct gfxARGB *color2 );
VOID ScalosGfxDrawEllipseRastPort( struct RastPort *rp, LONG xCenter, LONG yCenter, LONG radiusX, LONG radiusy, LONG segment, CONST struct gfxARGB *color1, CONST struct gfxARGB *color2 );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_SCALOSGFX_PROTOS_H */
