#ifndef PRAGMAS_SCALOSGFX_PRAGMAS_H
#define PRAGMAS_SCALOSGFX_PRAGMAS_H

#ifndef CLIB_SCALOSGFX_PROTOS_H
#include <clib/scalosgfx_protos.h>
#endif

#pragma  libcall ScalosGfxBase ScalosGfxCreateEmptySAC 01e 00
#pragma  libcall ScalosGfxBase ScalosGfxCreateSAC     024 9821005
#pragma  libcall ScalosGfxBase ScalosGfxFreeSAC       02a 801
#pragma  libcall ScalosGfxBase ScalosGfxCreateARGB    030 81003
#pragma  libcall ScalosGfxBase ScalosGfxFreeARGB      036 801
#pragma  libcall ScalosGfxBase ScalosGfxARGBSetAlpha  03c 0802
#pragma  libcall ScalosGfxBase ScalosGfxARGBSetAlphaMask 042 9802
#pragma  libcall ScalosGfxBase ScalosGfxCreateARGBFromBitMap 048 a9210806
#pragma  libcall ScalosGfxBase ScalosGfxFillARGBFromBitMap 04e a9803
#pragma  libcall ScalosGfxBase ScalosGfxWriteARGBToBitMap 054 a09804
#pragma  libcall ScalosGfxBase ScalosGfxMedianCut     05a 90803
#pragma  libcall ScalosGfxBase ScalosGfxScaleARGBArray 060 ba9804
#pragma  libcall ScalosGfxBase ScalosGfxScaleBitMap   066 9802
#pragma  libcall ScalosGfxBase ScalosGfxCalculateScaleAspect 06c 981004
#pragma  libcall ScalosGfxBase ScalosGfxBlitARGB      072 5432109808
#pragma  libcall ScalosGfxBase ScalosGfxFillRectARGB  078 32109806
#pragma  libcall ScalosGfxBase ScalosGfxSetARGB       07e 9802
#pragma  libcall ScalosGfxBase ScalosGfxNewColorMap   084 09803
#pragma  libcall ScalosGfxBase ScalosGfxARGBRectMult  08a 3210a9807
#pragma  libcall ScalosGfxBase ScalosGfxBlitARGBAlpha 090 5432109808
#pragma  libcall ScalosGfxBase ScalosGfxBlitARGBAlphaTagList 096 ab109806
#pragma  libcall ScalosGfxBase ScalosGfxBlitIcon      09c a32109807
#pragma  libcall ScalosGfxBase ScalosGfxDrawGradient  0a2 4a93210808
#pragma  libcall ScalosGfxBase ScalosGfxDrawGradientRastPort 0a8 4a93210808
#pragma  libcall ScalosGfxBase ScalosGfxDrawLine      0ae 93210806
#pragma  libcall ScalosGfxBase ScalosGfxDrawLineRastPort 0b4 93210806
#pragma  libcall ScalosGfxBase ScalosGfxDrawEllipse   0ba a943210808
#pragma  libcall ScalosGfxBase ScalosGfxDrawEllipseRastPort 0c0 a943210808
#ifdef __SASC_60
#pragma  tagcall ScalosGfxBase ScalosGfxCreateSACTags 024 9821005
#pragma  tagcall ScalosGfxBase ScalosGfxCreateARGBTags 030 81003
#pragma  tagcall ScalosGfxBase ScalosGfxMedianCutTags 05a 90803
#pragma  tagcall ScalosGfxBase ScalosGfxScaleARGBArrayTags 060 ba9804
#pragma  tagcall ScalosGfxBase ScalosGfxScaleBitMapTags 066 9802
#pragma  tagcall ScalosGfxBase ScalosGfxBlitARGBAlphaTags 096 ab109806
#pragma  tagcall ScalosGfxBase ScalosGfxBlitIconTags  09c a32109807
#endif

#endif	/*  PRAGMAS_SCALOSGFX_PRAGMA_H  */
