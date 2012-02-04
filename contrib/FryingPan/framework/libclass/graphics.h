/*
 * Amiga Generic Set - set of libraries and includes to ease sw development for all Amiga platforms
 * Copyright (C) 2001-2011 Tomasz Wiszkowski Tomasz.Wiszkowski at gmail.com.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef _LIBCLASS_GRAPHICS_H_
#define _LIBCLASS_GRAPHICS_H_
#define  CLIB_GRAPHICS_PROTOS_H

#if defined (_PROTO_GRAPHICS_H) || defined (PROTO_GRAPHICS_H)
#error "Please remove proto/graphics includes first!"
#endif

#define _PROTO_GRAPHICS_H
#define PROTO_GRAPHICS_H

#include <libclass/exec.h>
#define MOS_DIRECT_OS
#include <GenericLib/Calls.h>
#undef MOS_DIRECT_OS

extern class GraphicsIFace *Graphics;

#include <Generic/Types.h>

   BEGINDECL(Graphics, "graphics.library")
      FUNC11(uint32, BltBitMap, 5, const struct BitMap *, srcBitMap, a0, int32, xSrc, d0, int32, ySrc, d1, struct BitMap *, destBitMap, a1, int32, xDest, d2, int32, yDest, d3, int32, xSize, d4, int32, ySize, d5, uint32, minterm, d6, uint32, mask, d7, PLANEPTR, tempA, a2);
      PROC8(BltTemplate, 6, const PLANEPTR, source, a0, int32, xSrc, d0, int32, srcMod, d1, struct RastPort *, destRP, a1, int32, xDest, d2, int32, yDest, d3, int32, xSize, d4, int32, ySize, d5);
      PROC1(ClearEOL, 7, struct RastPort *, rp, a1);
      PROC1(ClearScreen, 8, struct RastPort *, rp, a1);
      FUNC3(int32, TextLength, 9, struct RastPort *, rp, a1, const char *, string, a0, uint32, count, d0);
      PROC3(Text, 10, struct RastPort *, rp, a1, const char *, string, a0, uint32, count, d0);
      PROC2(SetFont, 11, struct RastPort *, rp, a1, struct TextFont *, textFont, a0);
      FUNC1(struct TextFont *, OpenFont, 12, const struct TextAttr *, textAttr, a0);
      PROC1(CloseFont, 13, struct TextFont *, textFont, a1);
      FUNC1(uint32, AskSoftStyle, 14, struct RastPort *, rp, a1);
      FUNC3(uint32, SetSoftStyle, 15, struct RastPort *, rp, a1, uint32, style, d0, uint32, enable, d1);
      PROC2(AddBob, 16, struct Bob *, bob, a0, struct RastPort *, rp, a1);
      PROC2(AddVSprite, 17, struct VSprite *, vSprite, a0, struct RastPort *, rp, a1);
      PROC1(DoCollision, 18, struct RastPort *, rp, a1);
      PROC2(DrawGList, 19, struct RastPort *, rp, a1, struct ViewPort *, vp, a0);
      PROC3(InitGels, 20, struct VSprite *, head, a0, struct VSprite *, tail, a1, struct GelsInfo *, gelsInfo, a2);
      PROC1(InitMasks, 21, struct VSprite *, vSprite, a0);
      PROC3(RemIBob, 22, struct Bob *, bob, a0, struct RastPort *, rp, a1, struct ViewPort *, vp, a2);
      PROC1(RemVSprite, 23, struct VSprite *, vSprite, a0);
      PROC3(SetCollision, 0, uint32, num, d0, void *, routine, a0, struct GelsInfo *, gelsInfo, a1);
      PROC1(SortGList, 25, struct RastPort *, rp, a1);
      PROC3(AddAnimOb, 26, struct AnimOb *, anOb, a0, struct AnimOb **, anKey, a1, struct RastPort *, rp, a2);
      PROC2(Animate, 27, struct AnimOb **, anKey, a0, struct RastPort *, rp, a1);
      FUNC3(bool, GetGBuffers, 28, struct AnimOb *, anOb, a0, struct RastPort *, rp, a1, bool, flag, d0);
      PROC1(InitGMasks, 29, struct AnimOb *, anOb, a0);
      PROC5(DrawEllipse, 30, struct RastPort *, rp, a1, int32, xCenter, d0, int32, yCenter, d1, int32, a, d2, int32, b, d3);
      FUNC5(int32, AreaEllipse, 31, struct RastPort *, rp, a1, int32, xCenter, d0, int32, yCenter, d1, int32, a, d2, int32, b, d3);
      PROC3(LoadRGB4, 32, struct ViewPort *, vp, a0, const uint32 *, colors, a1, uint32, count, d0);
      PROC1(InitRastPort, 33, struct RastPort *, rp, a1);
      PROC1(InitVPort, 34, struct ViewPort *, vp, a0);
      FUNC1(uint32, MrgCop, 35, struct View *, view, a1);
      FUNC2(uint32, MakeVPort, 36, struct View *, view, a0, struct ViewPort *, vp, a1);
      PROC1(LoadView, 37, struct View *, view, a1);
      PROC0(WaitBlit, 38);
      PROC2(SetRast, 39, struct RastPort *, rp, a1, uint32, pen, d0);
      PROC3(Move, 40, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      PROC3(Draw, 41, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      FUNC3(int32, AreaMove, 42, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      FUNC3(int32, AreaDraw, 43, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      FUNC1(int32, AreaEnd, 44, struct RastPort *, rp, a1);
      PROC0(WaitTOF, 45);
      PROC1(QBlit, 46, struct bltnode *, blit, a1);
      PROC3(InitArea, 47, struct AreaInfo *, areaInfo, a0, void *, vectorBuffer, a1, int32, maxVectors, d0);
      PROC5(SetRGB4, 48, struct ViewPort *, vp, a0, uint32, colindex, d0, uint32, red, d1, uint32, green, d2, uint32, blue, d3);
      PROC1(QBSBlit, 49, struct bltnode *, blit, a1);
      PROC3(BltClear, 50, PLANEPTR, memBlock, a1, uint32, byteCount, d0, uint32, flags, d1);
      PROC5(RectFill, 51, struct RastPort *, rp, a1, int32, xMin, d0, int32, yMin, d1, int32, xMax, d2, int32, yMax, d3);
      PROC7(BltPattern, 52, struct RastPort *, rp, a1, const PLANEPTR, mask, a0, int32, xMin, d0, int32, yMin, d1, int32, xMax, d2, int32, yMax, d3, uint32, maskBPR, d4);
      FUNC3(int32, ReadPixel, 53, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      FUNC3(int32, WritePixel, 54, struct RastPort *, rp, a1, int32, x, d0, int32, y, d1);
      FUNC4(bool, Flood, 55, struct RastPort *, rp, a1, uint32, mode, d2, int32, x, d0, int32, y, d1);
      PROC3(PolyDraw, 56, struct RastPort *, rp, a1, int32, count, d0, const int32 *, polyTable, a0);
      PROC2(SetAPen, 57, struct RastPort *, rp, a1, uint32, pen, d0);
      PROC2(SetBPen, 58, struct RastPort *, rp, a1, uint32, pen, d0);
      PROC2(SetDrMd, 59, struct RastPort *, rp, a1, uint32, drawMode, d0);
      PROC1(InitView, 60, struct View *, view, a1);
      PROC1(CBump, 61, struct UCopList *, copList, a1);
      PROC3(CMove, 62, struct UCopList *, copList, a1, int32, destoffset, d0, int32, data, d1);
      PROC3(CWait, 63, struct UCopList *, copList, a1, int32, v, d0, int32, h, d1);
      FUNC0(int32, VBeamPos, 64);
      PROC4(InitBitMap, 65, struct BitMap *, bitMap, a0, int32, depth, d0, uint32, width, d1, uint32, height, d2);
      PROC7(ScrollRaster, 66, struct RastPort *, rp, a1, int32, dx, d0, int32, dy, d1, int32, xMin, d2, int32, yMin, d3, int32, xMax, d4, int32, yMax, d5);
      PROC1(WaitBOVP, 67, struct ViewPort *, vp, a0);
      FUNC2(int32, GetSprite, 68, struct SimpleSprite *, sprite, a0, int32, num, d0);
      PROC1(FreeSprite, 69, int32, num, d0);
      PROC3(ChangeSprite, 70, struct ViewPort *, vp, a0, struct SimpleSprite *, sprite, a1, void *, newData, a2);
      PROC4(MoveSprite, 71, struct ViewPort *, vp, a0, struct SimpleSprite *, sprite, a1, int32, x, d0, int32, y, d1);
      PROC1(LockLayerRom, 0, struct Layer *, layer, d0);
      PROC1(UnlockLayerRom, 0, struct Layer *, layer, d0);
      PROC1(SyncSBitMap, 74, struct Layer *, layer, a0);
      PROC1(CopySBitMap, 75, struct Layer *, layer, a0);
      PROC0(OwnBlitter, 76);
      PROC0(DisownBlitter, 77);
      FUNC3(struct TmpRas *, InitTmpRas, 78, struct TmpRas *, tmpRas, a0, PLANEPTR, buffer, a1, int32, size, d0);
      PROC2(AskFont, 79, struct RastPort *, rp, a1, struct TextAttr *, textAttr, a0);
      PROC1(AddFont, 80, struct TextFont *, textFont, a1);
      PROC1(RemFont, 81, struct TextFont *, textFont, a1);
      FUNC2(PLANEPTR, AllocRaster, 82, uint32, width, d0, uint32, height, d1);
      PROC3(FreeRaster, 83, PLANEPTR, p, a0, uint32, width, d0, uint32, height, d1);
      PROC2(AndRectRegion, 84, struct Region *, region, a0, const struct Rectangle *, rectangle, a1);
      FUNC2(bool, OrRectRegion, 85, struct Region *, region, a0, const struct Rectangle *, rectangle, a1);
      FUNC0(struct Region *, NewRegion, 86);
      FUNC2(bool, ClearRectRegion, 87, struct Region *, region, a0, const struct Rectangle *, rectangle, a1);
      PROC1(ClearRegion, 88, struct Region *, region, a0);
      PROC1(DisposeRegion, 89, struct Region *, region, a0);
      PROC1(FreeVPortCopLists, 90, struct ViewPort *, vp, a0);
      PROC1(FreeCopList, 91, struct CopList *, copList, a0);
      PROC9(ClipBlit, 92, struct RastPort *, srcRP, a0, int32, xSrc, d0, int32, ySrc, d1, struct RastPort *, destRP, a1, int32, xDest, d2, int32, yDest, d3, int32, xSize, d4, int32, ySize, d5, uint32, minterm, d6);
      FUNC2(bool, XorRectRegion, 93, struct Region *, region, a0, const struct Rectangle *, rectangle, a1);
      PROC1(FreeCprList, 94, struct cprlist *, cprList, a0);
      FUNC1(struct ColorMap *, GetColorMap, 95, uint32, entries, d0);
      PROC1(FreeColorMap, 96, struct ColorMap *, colorMap, a0);
      FUNC2(int32, GetRGB4, 97, struct ColorMap *, colorMap, a0, uint32, entry, d0);
      PROC1(ScrollVPort, 98, struct ViewPort *, vp, a0);
      FUNC2(struct CopList *, UCopperListInit, 99, struct UCopList *, uCopList, a0, int32, n, d0);
      PROC3(FreeGBuffers, 100, struct AnimOb *, anOb, a0, struct RastPort *, rp, a1, bool, flag, d0);
      FUNC9(bool, BltBitMapRastPort, 101, const struct BitMap *, srcBitMap, a0, int32, xSrc, d0, int32, ySrc, d1, struct RastPort *, destRP, a1, int32, xDest, d2, int32, yDest, d3, int32, xSize, d4, int32, ySize, d5, uint32, minterm, d6);
      FUNC2(bool, OrRegionRegion, 102, const struct Region *, srcRegion, a0, struct Region *, destRegion, a1);
      FUNC2(bool, XorRegionRegion, 103, const struct Region *, srcRegion, a0, struct Region *, destRegion, a1);
      FUNC2(bool, AndRegionRegion, 104, const struct Region *, srcRegion, a0, struct Region *, destRegion, a1);
      PROC5(SetRGB4CM, 105, struct ColorMap *, colorMap, a0, uint32, colindex, d0, uint32, red, d1, uint32, green, d2, uint32, blue, d3);
      PROC10(BltMaskBitMapRastPort, 106, struct BitMap *, srcBitMap, a0, int32, xSrc, d0, int32, ySrc, d1, struct RastPort *, destRP, a1, int32, xDest, d2, int32, yDest, d3, int32, xSize, d4, int32, ySize, d5, uint32, minterm, d6, PLANEPTR, bltMask, a2);
      PROC0(dummy1, 0);
      PROC0(dummy2, 0);
      FUNC1(bool, AttemptLockLayerRom, 109, struct Layer *, layer, d7);
      FUNC1(void *, GfxNew, 110, uint32, gfxNodeType, d0);
      PROC1(GfxFree, 111, struct ExtendedNode *, gfxNodePtr, a0);
      PROC2(GfxAssociate, 112, const void *, associateNode, a0, struct ExtendedNode *, gfxNodePtr, a1);
      PROC1(BitMapScale, 113, struct BitScaleArgs *, bitScaleArgs, a0);
      FUNC3(uint32, ScalerDiv, 114, uint32, factor, d0, uint32, numerator, d1, uint32, denominator, d2);
      PROC4(TextExtent, 115, struct RastPort *, rp, a1, const char *, string, a0, uint32, count, d0, struct TextExtent *, textExtent, a2);
      FUNC8(uint32, TextFit, 116, struct RastPort *, rp, a1, const char *, string, a0, uint32, strLen, d0, struct TextExtent *, textExtent, a2, const struct TextExtent *, constrainingExtent, a3, int32, strDirection, d1, uint32, constrainingBitWidth, d2, uint32, constrainingBitHeight, d3);
      FUNC1(void *, GfxLookUp, 117, const void *, associateNode, a0);
      FUNC2(uint32, VideoControl, 118, struct ColorMap *, colorMap, a0, struct TagItem *, tagarray, a1);
      PROC0(dummy3, 0);
      FUNC2(struct MonitorSpec *, OpenMonitor, 119, const char *, monitorName, a1, uint32, displayID, d0);
      FUNC1(int32, CloseMonitor, 120, struct MonitorSpec *, monitorSpec, a0);
      FUNC1(DisplayInfoHandle, FindDisplayInfo, 121, uint32, displayID, d0);
      FUNC1(uint32, NextDisplayInfo, 122, uint32, displayID, d0);
      PROC0(dummy4, 0);
      PROC0(dummy5, 0);
      PROC0(dummy6, 0);
      FUNC5(uint32, GetDisplayInfoData, 126, const DisplayInfoHandle, handle, a0, void *, buf, a1, uint32, size, d0, uint32, tagID, d1, uint32, displayID, d2);
      PROC2(FontExtent, 127, const struct TextFont *, font, a0, struct TextExtent *, fontExtent, a1);
      FUNC6(int32, ReadPixelLine8, 128, struct RastPort *, rp, a0, uint32, xstart, d0, uint32, ystart, d1, uint32, width, d2, uint8 *, array, a2, struct RastPort *, tempRP, a1);
      FUNC6(int32, WritePixelLine8, 129, struct RastPort *, rp, a0, uint32, xstart, d0, uint32, ystart, d1, uint32, width, d2, uint8 *, array, a2, struct RastPort *, tempRP, a1);
      FUNC7(int32, ReadPixelArray8, 130, struct RastPort *, rp, a0, uint32, xstart, d0, uint32, ystart, d1, uint32, xstop, d2, uint32, ystop, d3, uint8 *, array, a2, struct RastPort *, temprp, a1);
      FUNC7(int32, WritePixelArray8, 131, struct RastPort *, rp, a0, uint32, xstart, d0, uint32, ystart, d1, uint32, xstop, d2, uint32, ystop, d3, uint8 *, array, a2, struct RastPort *, temprp, a1);
      FUNC1(uint32, GetVPModeID, 132, const struct ViewPort *, vp, a0);
      FUNC1(uint32, ModeNotAvailable, 133, uint32, modeID, d0);
      PROC0(dummy7, 0);
      PROC0(dummy8, 0);
      PROC5(EraseRect, 135, struct RastPort *, rp, a1, int32, xMin, d0, int32, yMin, d1, int32, xMax, d2, int32, yMax, d3);
      FUNC2(uint32, ExtendFont, 136, struct TextFont *, font, a0, const struct TagItem *, fontTags, a1);
      PROC0(dummy9, 0);
      PROC1(StripFont, 137, struct TextFont *, font, a0);
      FUNC2(uint32, CalcIVG, 138, struct View *, v, a0, struct ViewPort *, vp, a1);
      FUNC2(int32, AttachPalExtra, 139, struct ColorMap *, cm, a0, struct ViewPort *, vp, a1);
      FUNC5(int32, ObtainBestPenA, 140, struct ColorMap *, cm, a0, uint32, r, d1, uint32, g, d2, uint32, b, d3, const struct TagItem *, tags, a1);
      PROC0(dummy10, 0);
      PROC0(dummy11, 0);
      PROC5(SetRGB32, 142, struct ViewPort *, vp, a0, uint32, n, d0, uint32, r, d1, uint32, g, d2, uint32, b, d3);
      FUNC1(uint32, GetAPen, 143, struct RastPort *, rp, a0);
      FUNC1(uint32, GetBPen, 144, struct RastPort *, rp, a0);
      FUNC1(uint32, GetDrMd, 145, struct RastPort *, rp, a0);
      FUNC1(uint32, GetOutlinePen, 146, struct RastPort *, rp, a0);
      PROC2(LoadRGB32, 147, struct ViewPort *, vp, a0, const uint32 *, table, a1);
      FUNC1(uint32, SetChipRev, 148, uint32, want, d0);
      PROC4(SetABPenDrMd, 149, struct RastPort *, rp, a1, uint32, apen, d0, uint32, bpen, d1, uint32, drawmode, d2);
      PROC4(GetRGB32, 150, const struct ColorMap *, cm, a0, uint32, firstcolor, d0, uint32, ncolors, d1, uint32 *, table, a1);
      PROC0(dummy12, 0);
      PROC0(dummy13, 0);
      FUNC5(struct BitMap *, AllocBitMap, 153, uint32, sizex, d0, uint32, sizey, d1, uint32, depth, d2, uint32, flags, d3, const struct BitMap *, friend_bitmap, a0);
      PROC1(FreeBitMap, 154, struct BitMap *, bm, a0);
      FUNC2(int32, GetExtSpriteA, 155, struct ExtSprite *, ss, a2, const struct TagItem *, tags, a1);
      PROC0(dummy14, 0);
      FUNC3(uint32, CoerceMode, 156, struct ViewPort *, vp, a0, uint32, monitorid, d0, uint32, flags, d1);
      PROC3(ChangeVPBitMap, 157, struct ViewPort *, vp, a0, struct BitMap *, bm, a1, struct DBufInfo *, db, a2);
      PROC2(ReleasePen, 158, struct ColorMap *, cm, a0, int32, n, d0);
      FUNC6(int32, ObtainPen, 159, struct ColorMap *, cm, a0, int32, n, d0, uint32, r, d1, uint32, g, d2, uint32, b, d3, int32, f, d4);
      FUNC2(uint32, GetBitMapAttr, 160, const struct BitMap *, bm, a0, uint32, attrnum, d1);
      FUNC1(struct DBufInfo *, AllocDBufInfo, 161, struct ViewPort *, vp, a0);
      PROC1(FreeDBufInfo, 162, struct DBufInfo *, dbi, a1);
      FUNC2(uint32, SetOutlinePen, 163, struct RastPort *, rp, a0, uint32, pen, d0);
      FUNC2(uint32, SetWriteMask, 164, struct RastPort *, rp, a0, uint32, msk, d0);
      PROC2(SetMaxPen, 165, struct RastPort *, rp, a0, uint32, maxpen, d0);
      PROC5(SetRGB32CM, 166, struct ColorMap *, cm, a0, uint32, n, d0, uint32, r, d1, uint32, g, d2, uint32, b, d3);
      PROC7(ScrollRasterBF, 167, struct RastPort *, rp, a1, int32, dx, d0, int32, dy, d1, int32, xMin, d2, int32, yMin, d3, int32, xMax, d4, int32, yMax, d5);
      FUNC5(uint32, FindColor, 168, struct ColorMap *, cm, a3, uint32, r, d1, uint32, g, d2, uint32, b, d3, int32, maxcolor, d4);
      PROC0(dummy15, 0);
      FUNC2(struct ExtSprite *, AllocSpriteDataA, 170, const struct BitMap *, bm, a2, const struct TagItem *, tags, a1);
      PROC0(dummy16, 0);
      FUNC4(int32, ChangeExtSpriteA, 171, struct ViewPort *, vp, a0, struct ExtSprite *, oldsprite, a1, struct ExtSprite *, newsprite, a2, const struct TagItem *, tags, a3);
      PROC0(dummy17, 0);
      PROC1(FreeSpriteData, 172, struct ExtSprite *, sp, a2);
      PROC2(SetRPAttrsA, 173, struct RastPort *, rp, a0, const struct TagItem *, tags, a1);
      PROC0(dummy18, 0);
      PROC2(GetRPAttrsA, 174, const struct RastPort *, rp, a0, const struct TagItem *, tags, a1);
      PROC0(dummy19, 0);
      FUNC1(uint32, BestModeIDA, 175, const struct TagItem *, tags, a0);
      PROC0(dummy20, 0);
      PROC7(WriteChunkyPixels, 176, struct RastPort *, rp, a0, uint32, xstart, d0, uint32, ystart, d1, uint32, xstop, d2, uint32, ystop, d3, const uint8 *, array, a2, int32, bytesperrow, d4);

   ENDDECL

#endif

