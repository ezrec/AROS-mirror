#ifndef DEFINES_GRAPHICS_PROTOS_H
#define DEFINES_GRAPHICS_PROTOS_H

/*
    (C) 1995-98 AROS - The Amiga Replacement OS
    $Id$

    Desc: Prototypes for Graphics.library
    Lang: english
*/

#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif
#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif

/* Defines */

#define AddAnimOb(anOb, anKey, rp) \
	AROS_LC3(void, AddAnimOb, \
	AROS_LCA(struct AnimOb *  , anOb,  A0), \
	AROS_LCA(struct AnimOb ** , anKey, A1), \
	AROS_LCA(struct RastPort *, rp ,   A2), \
	struct GfxBase *, GfxBase, 26, Graphics)

#define AddBob(Bob, rp) \
	AROS_LC2(void, AddBob, \
	AROS_LCA(struct Bob *     , Bob, A0), \
	AROS_LCA(struct RastPort *, rp , A1), \
	struct GfxBase *, GfxBase, 16, Graphics)

#define AddFont(textFont) \
	AROS_LC1(void, AddFont, \
	AROS_LCA(struct TextFont *, textFont, A1), \
	struct GfxBase *, GfxBase, 80, Graphics)

#define AddVSprite(vs, rp) \
	AROS_LC2(void, AddVSprite, \
	AROS_LCA(struct VSprite *, vs, A0), \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 17, Graphics)

#define AllocBitMap(sizex, sizey, depth, flags, friend_bitmap) \
	AROS_LC5(struct BitMap *, AllocBitMap, \
	AROS_LCA(ULONG          , sizex, D0), \
	AROS_LCA(ULONG          , sizey, D1), \
	AROS_LCA(ULONG          , depth, D2), \
	AROS_LCA(ULONG          , flags, D3), \
	AROS_LCA(struct BitMap *, friend_bitmap, A0), \
	struct GfxBase *, GfxBase, 153, Graphics)

#define AllocRaster(width, height) \
	AROS_LC2(PLANEPTR, AllocRaster, \
	AROS_LCA(ULONG          , width, D0), \
	AROS_LCA(ULONG          , height, D1), \
	struct GfxBase *, GfxBase, 82, Graphics)

#define AllocSpriteDataA(bitmap, taglist) \
	AROS_LC2(struct ExtSprite *, AllocSpriteDataA, \
	AROS_LCA(struct BitMap * , bitmap , A2), \
	AROS_LCA(struct TagItem *, taglist, A1), \
	struct GfxBase *, GfxBase, 170, Graphics)

#define AndRectRegion(region, rectangle) \
	AROS_LC2(void, AndRectRegion, \
	AROS_LCA(struct Region    *, region, A0), \
	AROS_LCA(struct Rectangle *, rectangle, A1), \
	struct GfxBase *, GfxBase, 84, Graphics)

#define Animate(anKey, rp) \
	AROS_LC2(void, Animate, \
	AROS_LCA(struct AnimOb ** , anKey, A0), \
	AROS_LCA(struct RastPort *, rp   , A1), \
	struct GfxBase *, GfxBase, 27, Graphics)

#define AskFont(rp, textAttr) \
	AROS_LC2(void, AskFont, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(struct TextAttr *, textAttr, A0), \
	struct GfxBase *, GfxBase, 79, Graphics)

#define BitMapScale(bitScaleArgs) \
	AROS_LC1(void, BitMapScale, \
	AROS_LCA(struct BitScaleArgs *, bitScaleArgs, A0), \
	struct GfxBase *, GfxBase, 113, Graphics)

#define BltBitMap(srcBitMap, xSrc, ySrc, destBitMap, xDest, yDest, xSize, ySize, minterm, mask, tempA) \
	AROS_LC11(LONG, BltBitMap, \
	AROS_LCA(struct BitMap *, srcBitMap, A0), \
	AROS_LCA(LONG           , xSrc, D0), \
	AROS_LCA(LONG           , ySrc, D1), \
	AROS_LCA(struct BitMap *, destBitMap, A1), \
	AROS_LCA(LONG           , xDest, D2), \
	AROS_LCA(LONG           , yDest, D3), \
	AROS_LCA(LONG           , xSize, D4), \
	AROS_LCA(LONG           , ySize, D5), \
	AROS_LCA(ULONG          , minterm, D6), \
	AROS_LCA(ULONG          , mask, D7), \
	AROS_LCA(PLANEPTR       , tempA, A2), \
	struct GfxBase *, GfxBase, 5, Graphics)

#define BltClear(memblock, bytecount, flags) \
	AROS_LC3(void, BltClear, \
	AROS_LCA(void *, memblock , A1), \
	AROS_LCA(ULONG , bytecount, D0), \
	AROS_LCA(ULONG , flags    , D1), \
	struct GfxBase *, GfxBase, 50, Graphics)

#define ClearRectRegion(region, rectangle) \
	AROS_LC2(BOOL, ClearRectRegion, \
	AROS_LCA(struct Region    *, region   , A0), \
	AROS_LCA(struct Rectangle *, rectangle, A1), \
	struct GfxBase *, GfxBase, 87, Graphics)

#define ClearRegion(region) \
	AROS_LC1(void, ClearRegion, \
	AROS_LCA(struct Region *, region, A0), \
	struct GfxBase *, GfxBase, 88, Graphics)

#define CloneRastPort(rp) \
	AROS_LC1(struct RastPort *, CloneRastPort, \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 178, Graphics)

#define CloseFont(textFont) \
	AROS_LC1(void, CloseFont, \
	AROS_LCA(struct TextFont *, textFont, A1), \
	struct GfxBase *, GfxBase, 13, Graphics)

#define CreateRastPort() \
	AROS_LC0(struct RastPort *, CreateRastPort, \
	struct GfxBase *, GfxBase, 177, Graphics)

#define DeinitRastPort(rp) \
	AROS_LC1(void, DeinitRastPort, \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 179, Graphics)

#define DisposeRegion(region) \
	AROS_LC1(void, DisposeRegion, \
	AROS_LCA(struct Region *, region, A0), \
	struct GfxBase *, GfxBase, 89, Graphics)

#define Draw(rp, x, y) \
	AROS_LC3(void, Draw, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , x, D0), \
	AROS_LCA(LONG             , y, D1), \
	struct GfxBase *, GfxBase, 41, Graphics)

#define DrawEllipse(rp, xCenter, yCenter, a, b) \
	AROS_LC5(void, DrawEllipse, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , xCenter, D0), \
	AROS_LCA(LONG             , yCenter, D1), \
	AROS_LCA(LONG             , a, D2), \
	AROS_LCA(LONG             , b, D3), \
	struct GfxBase *, GfxBase, 30, Graphics)

#define EraseRect(rp, xMin, yMin, xMax, yMax) \
	AROS_LC5(void, EraseRect, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , xMin, D0), \
	AROS_LCA(LONG             , yMin, D1), \
	AROS_LCA(LONG             , xMax, D2), \
	AROS_LCA(LONG             , yMax, D3), \
	struct GfxBase *, GfxBase, 135, Graphics)

#define ExtendFont(font, fontTags) \
	AROS_LC2(ULONG, ExtendFont, \
	AROS_LCA(struct TextFont *, font, A0), \
	AROS_LCA(struct TagItem  *, fontTags, A1), \
	struct GfxBase *, GfxBase, 136, Graphics)

#define FreeBitMap(bm) \
	AROS_LC1(void, FreeBitMap, \
	AROS_LCA(struct BitMap *, bm, A0), \
	struct GfxBase *, GfxBase, 154, Graphics)

#define FreeColorMap(colormap) \
	AROS_LC1(void, FreeColorMap, \
	AROS_LCA(struct ColorMap *, colormap, A0), \
	struct GfxBase *, GfxBase, 96, Graphics)

#define FreeGBuffers(anOb, rp, db) \
	AROS_LC3(void, FreeGBuffers, \
	AROS_LCA(struct AnimOb *  , anOb, A0), \
	AROS_LCA(struct RastPort *, rp  , A1), \
	AROS_LCA(BOOL             , db  , D0), \
	struct GfxBase *, GfxBase, 100, Graphics)

#define FreeRaster(p, width, height) \
	AROS_LC3(void, FreeRaster, \
	AROS_LCA(PLANEPTR, p,      A0), \
	AROS_LCA(ULONG,    width,  D0), \
	AROS_LCA(ULONG,    height, D1), \
	struct GfxBase *, GfxBase, 83, Graphics)

#define FreeRastPort(rp) \
	AROS_LC1(void, FreeRastPort, \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 180, Graphics)

#define FreeSprite(pick) \
	AROS_LC1(void, FreeSprite, \
	AROS_LCA(WORD, pick, D0), \
	struct GfxBase *, GfxBase, 69, Graphics)

#define FreeSpriteData(extsp) \
	AROS_LC1(void, FreeSpriteData, \
	AROS_LCA(struct ExtSprite *, extsp, A2), \
	struct GfxBase *, GfxBase, 172, Graphics)

#define GetAPen(rp) \
	AROS_LC1(ULONG, GetAPen, \
	AROS_LCA(struct RastPort *, rp, A0), \
	struct GfxBase *, GfxBase, 143, Graphics)

#define GetBitMapAttr(bitmap, attribute) \
	AROS_LC2(IPTR, GetBitMapAttr, \
	AROS_LCA(struct BitMap *, bitmap, A0), \
	AROS_LCA(ULONG          , attribute, D1), \
	struct GfxBase *, GfxBase, 160, Graphics)

#define GetBPen(rp) \
	AROS_LC1(ULONG, GetBPen, \
	AROS_LCA(struct RastPort *, rp, A0), \
	struct GfxBase *, GfxBase, 144, Graphics)

#define GetColorMap(entries) \
	AROS_LC1(struct ColorMap *, GetColorMap, \
	AROS_LCA(ULONG, entries, D0), \
	struct GfxBase *, GfxBase, 95, Graphics)

#define GetDrMd(rp) \
	AROS_LC1(ULONG, GetDrMd, \
	AROS_LCA(struct RastPort *, rp, A0), \
	struct GfxBase *, GfxBase, 145, Graphics)

#define GetExtSpriteA(sprite, tags) \
	AROS_LC2(BOOL, GetExtSpriteA, \
	AROS_LCA(struct ExtSprite *, sprite, A2), \
	AROS_LCA(struct TagItem   *, tags  , A1), \
	struct GfxBase *, GfxBase, 155, Graphics)

#define GetGBuffers(anOb, rp, db) \
	AROS_LC3(BOOL, GetGBuffers, \
	AROS_LCA(struct AnimOb *  , anOb, A0), \
	AROS_LCA(struct RastPort *, rp  , A1), \
	AROS_LCA(BOOL             , db  , D0), \
	struct GfxBase *, GfxBase, 28, Graphics)

#define GetOutlinePen(rp) \
	AROS_LC1(ULONG, GetOutlinePen, \
	AROS_LCA(struct RastPort *, rp, A0), \
	struct GfxBase *, GfxBase, 146, Graphics)

#define GetSprite(sprite, pick) \
	AROS_LC2(void, GetSprite, \
	AROS_LCA(struct SimpleSprite *, sprite, A0), \
	AROS_LCA(WORD                 , pick  , D0), \
	struct GfxBase *, GfxBase, 68, Graphics)

#define GfxAssociate(pointer, node) \
	AROS_LC2( void , GfxAssociate, \
	AROS_LCA( void *, pointer, A0), \
	AROS_LCA( struct ExtendedNode *, node, A1), \
	struct GfxBase *, GfxBase, 112, Graphics)

#define GfxFree(node) \
	AROS_LC1( void , GfxFree, \
	AROS_LCA(struct ExtendedNode *, node, A0), \
	struct GfxBase *, GfxBase, 111, Graphics)

#define GfxLookUp(pointer) \
	AROS_LC1( struct ExtendedNode *, GfxLookUp, \
	AROS_LCA( void *, pointer, A0), \
	struct GfxBase *, GfxBase, 117, Graphics)

#define GfxNew(node_type) \
	AROS_LC1(struct ExtendedNode *, GfxNew, \
	AROS_LCA( ULONG, node_type, D0), \
	struct GfxBase *, GfxBase, 110, Graphics)

#define InitArea(areainfo, buffer, maxvectors) \
	AROS_LC3(void, InitArea, \
	AROS_LCA(struct AreaInfo *, areainfo,   A0), \
	AROS_LCA(void *           , buffer,     A1), \
	AROS_LCA(SHORT            , maxvectors, D0), \
	struct GfxBase *, GfxBase, 47, Graphics)

#define InitGels(head, tail, GInfo) \
	AROS_LC3(void, InitGels, \
	AROS_LCA(struct VSprite * , head , A0), \
	AROS_LCA(struct VSprite * , tail , A1), \
	AROS_LCA(struct GelsInfo *, GInfo, A2), \
	struct GfxBase *, GfxBase, 20, Graphics)

#define InitGMasks(anOb) \
	AROS_LC1(void, InitGMasks, \
	AROS_LCA(struct AnimOb *, anOb, A0), \
	struct GfxBase *, GfxBase, 29, Graphics)

#define InitMasks(vs) \
	AROS_LC1(void, InitMasks, \
	AROS_LCA(struct VSprite *, vs, A0), \
	struct GfxBase *, GfxBase, 21, Graphics)

#define InitRastPort(rp) \
	AROS_LC1(BOOL, InitRastPort, \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 33, Graphics)

#define InitView(view) \
	AROS_LC1(void, InitView, \
	AROS_LCA(struct View *, view, A1), \
	struct GfxBase *, GfxBase, 60, Graphics)

#define InitVPort(vp) \
	AROS_LC1(void, InitVPort, \
	AROS_LCA(struct ViewPort *, vp, A0), \
	struct GfxBase *, GfxBase, 34, Graphics)

#define LoadRGB32(vp, table) \
	AROS_LC2(void, LoadRGB32, \
	AROS_LCA(struct ViewPort *, vp, A0), \
	AROS_LCA(ULONG           *, table, A1), \
	struct GfxBase *, GfxBase, 147, Graphics)

#define LoadRGB4(vp, colors, count) \
	AROS_LC3(void, LoadRGB4, \
	AROS_LCA(struct ViewPort *, vp, A0), \
	AROS_LCA(UWORD           *, colors, A1), \
	AROS_LCA(LONG             , count, D0), \
	struct GfxBase *, GfxBase, 32, Graphics)

#define Move(rp, x, y) \
	AROS_LC3(void, Move, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , x, D0), \
	AROS_LCA(LONG             , y, D1), \
	struct GfxBase *, GfxBase, 40, Graphics)

#define NewRegion() \
	AROS_LC0(struct Region *, NewRegion, \
	struct GfxBase *, GfxBase, 86, Graphics)

#define OpenFont(textAttr) \
	AROS_LC1(struct TextFont *, OpenFont, \
	AROS_LCA(struct TextAttr *, textAttr, A0), \
	struct GfxBase *, GfxBase, 12, Graphics)

#define OrRectRegion(region, rectangle) \
	AROS_LC2(BOOL, OrRectRegion, \
	AROS_LCA(struct Region    *, region,    A0), \
	AROS_LCA(struct Rectangle *, rectangle, A1), \
	struct GfxBase *, GfxBase, 85, Graphics)

#define PolyDraw(rp, count, polyTable) \
	AROS_LC3(void, PolyDraw, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , count, D0), \
	AROS_LCA(WORD            *, polyTable, A0), \
	struct GfxBase *, GfxBase, 56, Graphics)

#define ReadPixel(rp, x, y) \
	AROS_LC3(ULONG, ReadPixel, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , x, D0), \
	AROS_LCA(LONG             , y, D1), \
	struct GfxBase *, GfxBase, 53, Graphics)

#define RectFill(rp, xMin, yMin, xMax, yMax) \
	AROS_LC5(void, RectFill, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , xMin, D0), \
	AROS_LCA(LONG             , yMin, D1), \
	AROS_LCA(LONG             , xMax, D2), \
	AROS_LCA(LONG             , yMax, D3), \
	struct GfxBase *, GfxBase, 51, Graphics)

#define RemFont(textFont) \
	AROS_LC1(void, RemFont, \
	AROS_LCA(struct TextFont *, textFont, A1), \
	struct GfxBase *, GfxBase, 81, Graphics)

#define RemVSprite(vs) \
	AROS_LC1(void, RemVSprite, \
	AROS_LCA(struct VSprite *, vs, A0), \
	struct GfxBase *, GfxBase, 23, Graphics)

#define ScrollRaster(rp, dx, dy, xMin, yMin, xMax, yMax) \
	AROS_LC7(void, ScrollRaster, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , dx, D0), \
	AROS_LCA(LONG             , dy, D1), \
	AROS_LCA(LONG             , xMin, D2), \
	AROS_LCA(LONG             , yMin, D3), \
	AROS_LCA(LONG             , xMax, D4), \
	AROS_LCA(LONG             , yMax, D5), \
	struct GfxBase *, GfxBase, 66, Graphics)

#define SetABPenDrMd(rp, apen, bpen, drawMode) \
	AROS_LC4(void, SetABPenDrMd, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(ULONG            , apen, D0), \
	AROS_LCA(ULONG            , bpen, D1), \
	AROS_LCA(ULONG            , drawMode, D2), \
	struct GfxBase *, GfxBase, 149, Graphics)

#define SetAPen(rp, pen) \
	AROS_LC2(void, SetAPen, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(ULONG            , pen, D0), \
	struct GfxBase *, GfxBase, 57, Graphics)

#define SetBPen(rp, pen) \
	AROS_LC2(void, SetBPen, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(ULONG            , pen, D0), \
	struct GfxBase *, GfxBase, 58, Graphics)

#define SetCollision(num, routine, GInfo) \
	AROS_LC3(void, SetCollision, \
	AROS_LCA(ULONG            , num          , D0), \
	AROS_LCA(void             , (* routine)(), A0), \
	AROS_LCA(struct GelsInfo *, GInfo        , A1), \
	struct GfxBase *, GfxBase, 24, Graphics)

#define SetDrMd(rp, drawMode) \
	AROS_LC2(void, SetDrMd, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(ULONG            , drawMode, D0), \
	struct GfxBase *, GfxBase, 59, Graphics)

#define SetFont(rp, textFont) \
	AROS_LC2(void, SetFont, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(struct TextFont *, textFont, A0), \
	struct GfxBase *, GfxBase, 11, Graphics)

#define SetOutlinePen(rp, pen) \
	AROS_LC2(ULONG, SetOutlinePen, \
	AROS_LCA(struct RastPort *, rp, A0), \
	AROS_LCA(ULONG,             pen, D0), \
	struct GfxBase *, GfxBase, 163, Graphics)

#define SetRast(rp, pen) \
	AROS_LC2(void, SetRast, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(ULONG            , pen, D0), \
	struct GfxBase *, GfxBase, 39, Graphics)

#define SetRGB32(vp, n, r, g, b) \
	AROS_LC5(void, SetRGB32, \
	AROS_LCA(struct ViewPort *, vp, A0), \
	AROS_LCA(ULONG            , n, D0), \
	AROS_LCA(ULONG            , r, D1), \
	AROS_LCA(ULONG            , g, D2), \
	AROS_LCA(ULONG            , b, D3), \
	struct GfxBase *, GfxBase, 142, Graphics)

#define SetRPAttrsA(rp, tags) \
	AROS_LC2(void, SetRPAttrsA, \
	AROS_LCA(struct RastPort *, rp, A0), \
	AROS_LCA(struct TagItem  *, tags, A1), \
	struct GfxBase *, GfxBase, 173, Graphics)

#define SetWriteMask(rp, mask) \
	AROS_LC2(ULONG, SetWriteMask, \
	AROS_LCA(struct RastPort *, rp,   A0), \
	AROS_LCA(ULONG            , mask, D0), \
	struct GfxBase *, GfxBase, 164, Graphics)

#define SortGList(rp) \
	AROS_LC1(void, SortGList, \
	AROS_LCA(struct RastPort *, rp, A1), \
	struct GfxBase *, GfxBase, 25, Graphics)

#define StripFont(font) \
	AROS_LC1(void, StripFont, \
	AROS_LCA(struct TextFont *, font, A0), \
	struct GfxBase *, GfxBase, 137, Graphics)

#define Text(rp, string, count) \
	AROS_LC3(void, Text, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(STRPTR           , string, A0), \
	AROS_LCA(ULONG            , count, D0), \
	struct GfxBase *, GfxBase, 10, Graphics)

#define TextExtent(rp, string, count, textExtent) \
	AROS_LC4(void, TextExtent, \
	AROS_LCA(struct RastPort   *, rp, A1), \
	AROS_LCA(STRPTR             , string, A0), \
	AROS_LCA(ULONG              , count, D0), \
	AROS_LCA(struct TextExtent *, textExtent, A2), \
	struct GfxBase *, GfxBase, 115, Graphics)

#define TextFit(rp, string, strLen, textExtent, constrainingExtent, strDirection, constrainingBitWidth, constrainingBitHeight) \
	AROS_LC8(ULONG, TextFit, \
	AROS_LCA(struct RastPort   *, rp, A1), \
	AROS_LCA(STRPTR             , string, A0), \
	AROS_LCA(ULONG              , strLen, D0), \
	AROS_LCA(struct TextExtent *, textExtent, A2), \
	AROS_LCA(struct TextExtent *, constrainingExtent, A3), \
	AROS_LCA(LONG               , strDirection, D1), \
	AROS_LCA(ULONG              , constrainingBitWidth, D2), \
	AROS_LCA(ULONG              , constrainingBitHeight, D3), \
	struct GfxBase *, GfxBase, 116, Graphics)

#define TextLength(rp, string, count) \
	AROS_LC3(WORD, TextLength, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(STRPTR           , string, A0), \
	AROS_LCA(ULONG            , count, D0), \
	struct GfxBase *, GfxBase, 9, Graphics)

#define WaitTOF() \
	AROS_LC0(void, WaitTOF, \
	struct GfxBase *, GfxBase, 45, Graphics)

#define WeighTAMatch(reqTextAttr, targetTextAttr, targetTags) \
	AROS_LC3(WORD, WeighTAMatch, \
	AROS_LCA(struct TextAttr *, reqTextAttr, A0), \
	AROS_LCA(struct TextAttr *, targetTextAttr, A1), \
	AROS_LCA(struct TagItem  *, targetTags, A2), \
	struct GfxBase *, GfxBase, 134, Graphics)

#define WritePixel(rp, x, y) \
	AROS_LC3(LONG, WritePixel, \
	AROS_LCA(struct RastPort *, rp, A1), \
	AROS_LCA(LONG             , x, D0), \
	AROS_LCA(LONG             , y, D1), \
	struct GfxBase *, GfxBase, 54, Graphics)

#define WritePixelArray8(rp, xstart, ystart, xstop, ystop, array, temprp) \
	AROS_LC7(LONG, WritePixelArray8, \
	AROS_LCA(struct RastPort *, rp, A0), \
	AROS_LCA(ULONG            , xstart, D0), \
	AROS_LCA(ULONG            , ystart, D1), \
	AROS_LCA(ULONG            , xstop, D2), \
	AROS_LCA(ULONG            , ystop, D3), \
	AROS_LCA(UBYTE           *, array, A2), \
	AROS_LCA(struct RastPort *, temprp, A1), \
	struct GfxBase *, GfxBase, 131, Graphics)

#define XorRectRegion(region, rectangle) \
	AROS_LC2(BOOL, XorRectRegion, \
	AROS_LCA(struct Region    *, region,    A0), \
	AROS_LCA(struct Rectangle *, rectangle, A1), \
	struct GfxBase *, GfxBase, 93, Graphics)

#endif /* DEFINES_GRAPHICS_PROTOS_H */
