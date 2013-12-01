#ifndef PRAGMAS_SCALOSGFX_PRAGMAS_H
#define PRAGMAS_SCALOSGFX_PRAGMAS_H

/*
**	$Id$
**
**	Direct ROM interface (pragma) definitions.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#if defined(LATTICE) || defined(__SASC) || defined(_DCC)
#ifndef __CLIB_PRAGMA_LIBCALL
#define __CLIB_PRAGMA_LIBCALL
#endif /* __CLIB_PRAGMA_LIBCALL */
#else /* __MAXON__, __STORM__ or AZTEC_C */
#ifndef __CLIB_PRAGMA_AMICALL
#define __CLIB_PRAGMA_AMICALL
#endif /* __CLIB_PRAGMA_AMICALL */
#endif /* */

#if defined(__SASC) || defined(__STORM__)
#ifndef __CLIB_PRAGMA_TAGCALL
#define __CLIB_PRAGMA_TAGCALL
#endif /* __CLIB_PRAGMA_TAGCALL */
#endif /* __MAXON__, __STORM__ or AZTEC_C */

#ifndef CLIB_SCALOSGFX_PROTOS_H
#include <clib/scalosgfx_protos.h>
#endif /* CLIB_SCALOSGFX_PROTOS_H */

#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxCreateEmptySAC 1e 00
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxCreateSAC 24 9821005
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxCreateSACTags 24 9821005
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x24, ScalosGfxCreateSACTags(d0,d1,d2,a0,a1))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxFreeSAC 2a 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxCreateARGB 30 81003
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxCreateARGBTags 30 81003
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x30, ScalosGfxCreateARGBTags(d0,d1,a0))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxFreeARGB 36 801
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxARGBSetAlpha 3c 0802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxARGBSetAlphaMask 42 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxCreateARGBFromBitMap 48 A9210806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxFillARGBFromBitMap 4e A9803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxWriteARGBToBitMap 54 A09804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxMedianCut 5a 90803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxMedianCutTags 5a 90803
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x5a, ScalosGfxMedianCutTags(a0,d0,a1))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxScaleARGBArray 60 BA9804
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxScaleARGBArrayTags 60 BA9804
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x60, ScalosGfxScaleARGBArrayTags(a0,a1,a2,a3))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxScaleBitMap 66 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxScaleBitMapTags 66 9802
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x66, ScalosGfxScaleBitMapTags(a0,a1))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxCalculateScaleAspect 6c 981004
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxBlitARGB 72 5432109808
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxFillRectARGB 78 32109806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxSetARGB 7e 9802
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxNewColorMap 84 09803
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxARGBRectMult 8a 3210A9807
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxBlitARGBAlpha 90 5432109808
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxBlitARGBAlphaTagList 96 AB109806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxBlitARGBAlphaTags 96 AB109806
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x96, ScalosGfxBlitARGBAlphaTags(a0,a1,d0,d1,a3,a2))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxBlitIcon 9c A32109807
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_TAGCALL
 #ifdef __CLIB_PRAGMA_LIBCALL
  #pragma tagcall ScalosGfxBase ScalosGfxBlitIconTags 9c A32109807
 #endif /* __CLIB_PRAGMA_LIBCALL */
 #ifdef __CLIB_PRAGMA_AMICALL
  #pragma tagcall(ScalosGfxBase, 0x9c, ScalosGfxBlitIconTags(a0,a1,d0,d1,d2,d3,a2))
 #endif /* __CLIB_PRAGMA_AMICALL */
#endif /* __CLIB_PRAGMA_TAGCALL */
/* --- function in v42 or higher */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawGradient a2 4A93210808
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawGradientRastPort a8 4A93210808
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawLine ae 93210806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawLineRastPort b4 93210806
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawEllipse ba A943210808
#endif /* __CLIB_PRAGMA_LIBCALL */
#ifdef __CLIB_PRAGMA_LIBCALL
 #pragma libcall ScalosGfxBase ScalosGfxDrawEllipseRastPort c0 A943210808
#endif /* __CLIB_PRAGMA_LIBCALL */

#endif /* PRAGMAS_SCALOSGFX_PRAGMAS_H */
