#ifndef _INLINE_CYBERGRAPHICS_H
#define _INLINE_CYBERGRAPHICS_H

#ifndef CLIB_CYBERGRAPHICS_PROTOS_H
#define CLIB_CYBERGRAPHICS_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif

#ifndef CYBERGRAPHICS_BASE_NAME
#define CYBERGRAPHICS_BASE_NAME CyberGfxBase
#endif

#define IsCyberModeID(displayID) \
	LP1(0x36, BOOL, IsCyberModeID, ULONG, displayID, d0, \
	, CYBERGRAPHICS_BASE_NAME)

#define BestCModeIDTagList(bestModeIDTags) \
	LP1(0x3c, ULONG, BestCModeIDTagList, struct TagItem *, bestModeIDTags, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define BestCModeIDTags(tags...) \
	({ULONG _tags[] = {tags}; BestCModeIDTagList((struct TagItem *) _tags);})
#endif

#define CModeRequestTagList(modeRequest, modeRequestTags) \
	LP2(0x42, ULONG, CModeRequestTagList, APTR, modeRequest, a0, struct TagItem *, modeRequestTags, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define CModeRequestTags(modeRequest, tags...) \
	({ULONG _tags[] = {tags}; CModeRequestTagList((modeRequest), (struct TagItem *) _tags);})
#endif

#define AllocCModeListTagList(modeListTags) \
	LP1(0x48, struct List *, AllocCModeListTagList, struct TagItem *, modeListTags, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define AllocCModeListTags(tags...) \
	({ULONG _tags[] = {tags}; AllocCModeListTagList((struct TagItem *) _tags);})
#endif

#define FreeCModeList(modeList) \
	LP1NR(0x4e, FreeCModeList, struct List *, modeList, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#define ScalePixelArray(srcRect, srcW, srcH, srcMod, rp, destX, destY, destW, destH, srcFormat) \
	LP10(0x5a, LONG, ScalePixelArray, APTR, srcRect, a0, ULONG, srcW, d0, ULONG, srcH, d1, ULONG, srcMod, d2, struct RastPort *, rp, a1, ULONG, destX, d3, ULONG, destY, d4, ULONG, destW, d5, ULONG, destH, d6, ULONG, srcFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define GetCyberMapAttr(cyberGfxBitmap, cyberAttrTag) \
	LP2(0x60, ULONG, GetCyberMapAttr, struct BitMap *, cyberGfxBitmap, a0, ULONG, cyberAttrTag, d0, \
	, CYBERGRAPHICS_BASE_NAME)

#define GetCyberIDAttr(cyberIDAttr, cyberDisplayModeID) \
	LP2(0x66, ULONG, GetCyberIDAttr, ULONG, cyberIDAttr, d0, ULONG, cyberDisplayModeID, d1, \
	, CYBERGRAPHICS_BASE_NAME)

#define ReadRGBPixel(rp, x, y) \
	LP3(0x6c, ULONG, ReadRGBPixel, struct RastPort *, rp, a1, ULONG, x, d0, ULONG, y, d1, \
	, CYBERGRAPHICS_BASE_NAME)

#define WriteRGBPixel(rp, x, y, argb) \
	LP4(0x72, LONG, WriteRGBPixel, struct RastPort *, rp, a1, ULONG, x, d0, ULONG, y, d1, ULONG, argb, d2, \
	, CYBERGRAPHICS_BASE_NAME)

#define ReadPixelArray(destRect, destX, destY, destMod, rp, srcX, srcY, sizeX, sizeY, destFormat) \
	LP10(0x78, ULONG, ReadPixelArray, APTR, destRect, a0, ULONG, destX, d0, ULONG, destY, d1, ULONG, destMod, d2, struct RastPort *, rp, a1, ULONG, srcX, d3, ULONG, srcY, d4, ULONG, sizeX, d5, ULONG, sizeY, d6, ULONG, destFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define WritePixelArray(srcRect, srcX, srcY, srcMod, rp, destX, destY, sizeX, sizeY, srcFormat) \
	LP10(0x7e, ULONG, WritePixelArray, APTR, srcRect, a0, ULONG, srcX, d0, ULONG, srcY, d1, ULONG, srcMod, d2, struct RastPort *, rp, a1, ULONG, destX, d3, ULONG, destY, d4, ULONG, sizeX, d5, ULONG, sizeY, d6, ULONG, srcFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define MovePixelArray(srcX, srcY, rp, destX, destY, sizeX, sizeY) \
	LP7(0x84, ULONG, MovePixelArray, ULONG, srcX, d0, ULONG, srcY, d1, struct RastPort *, rp, a1, ULONG, destX, d2, ULONG, destY, d3, ULONG, sizeX, d4, ULONG, sizeY, d5, \
	, CYBERGRAPHICS_BASE_NAME)

#define InvertPixelArray(rp, destX, destY, sizeX, sizeY) \
	LP5(0x90, ULONG, InvertPixelArray, struct RastPort *, rp, a1, ULONG, destX, d0, ULONG, destY, d1, ULONG, sizeX, d2, ULONG, sizeY, d3, \
	, CYBERGRAPHICS_BASE_NAME)

#define FillPixelArray(rp, destX, destY, sizeX, sizeY, aRGB) \
	LP6(0x96, ULONG, FillPixelArray, struct RastPort *, rp, a1, ULONG, destX, d0, ULONG, destY, d1, ULONG, sizeX, d2, ULONG, sizeY, d3, ULONG, aRGB, d4, \
	, CYBERGRAPHICS_BASE_NAME)

#define DoCDrawMethodTagList(hook, rp, tagList) \
	LP3NR(0x9c, DoCDrawMethodTagList, struct Hook *, hook, a0, struct RastPort *, rp, a1, struct TagItem *, tagList, a2, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define DoCDrawMethodTags(hook, rp, tags...) \
	({ULONG _tags[] = {tags}; DoCDrawMethodTagList((hook), (rp), (struct TagItem *) _tags);})
#endif

#define CVideoCtrlTagList(viewPort, tagList) \
	LP2NR(0xa2, CVideoCtrlTagList, struct ViewPort *, viewPort, a0, struct TagItem *, tagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define CVideoCtrlTags(viewPort, tags...) \
	({ULONG _tags[] = {tags}; CVideoCtrlTagList((viewPort), (struct TagItem *) _tags);})
#endif

#define LockBitMapTagList(bitMap, tagList) \
	LP2(0xa8, APTR, LockBitMapTagList, APTR, bitMap, a0, struct TagItem *, tagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define LockBitMapTags(bitMap, tags...) \
	({ULONG _tags[] = {tags}; LockBitMapTagList((bitMap), (struct TagItem *) _tags);})
#endif

#define UnLockBitMap(handle) \
	LP1NR(0xae, UnLockBitMap, APTR, handle, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#define UnLockBitMapTagList(handle, tagList) \
	LP2NR(0xb4, UnLockBitMapTagList, APTR, handle, a0, struct TagItem *, tagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define UnLockBitMapTags(handle, tags...) \
	({ULONG _tags[] = {tags}; UnLockBitMapTagList((handle), (struct TagItem *) _tags);})
#endif

#define ExtractColor(rp, bitMap, colour, srcX, srcY, width, height) \
	LP7(0xba, ULONG, ExtractColor, struct RastPort *, rp, a0, struct BitMap *, bitMap, a1, ULONG, colour, d0, ULONG, srcX, d1, ULONG, srcY, d2, ULONG, width, d3, ULONG, height, d4, \
	, CYBERGRAPHICS_BASE_NAME)

#define WriteLUTPixelArray(srcRect, srcX, srcY, srcMod, rp, colorTab, destX, destY, sizeX, sizeY, cTFormat) \
	LP11(0xc6, ULONG, WriteLUTPixelArray, APTR, srcRect, a0, ULONG, srcX, d0, ULONG, srcY, d1, ULONG, srcMod, d2, struct RastPort *, rp, a1, APTR, colorTab, a2, ULONG, destX, d3, ULONG, destY, d4, ULONG, sizeX, d5, ULONG, sizeY, d6, ULONG, cTFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define WritePixelArrayAlpha(srcRect, srcX, srcY, srcMod, rp, destX, destY, sizeX, sizeY, globalAlpha) \
	LP10(0xd8, ULONG, WritePixelArrayAlpha, APTR, srcRect, a0, ULONG, srcX, d0, ULONG, srcY, d1, ULONG, srcMod, d2, struct RastPort *, rp, a1, ULONG, destX, d3, ULONG, destY, d4, ULONG, sizeX, d5, ULONG, sizeY, d6, ULONG, globalAlpha, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define BltTemplateAlpha(source, xSrc, srcMod, destRP, xDest, yDest, xSize, ySize) \
	LP8NR(0xde, BltTemplateAlpha, UBYTE *, source, a0, LONG, xSrc, d0, LONG, srcMod, d1, struct RastPort *, destRP, a1, ULONG, xDest, d2, ULONG, yDest, d3, ULONG, xSize, d4, ULONG, ySize, d5, \
	, CYBERGRAPHICS_BASE_NAME)

#define ProcessPixelArray(rp, destX, destY, sizeX, sizeY, operation, value, taglist) \
	LP8NR(0xe4, ProcessPixelArray, struct RastPort *, rp, a1, ULONG, destX, d0, ULONG, destY, d1, ULONG, sizeX, d2, ULONG, sizeY, d3, ULONG, operation, d4, LONG, value, d5, struct TagItem *, taglist, a2, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define ProcessPixelArrayTags(rp, destX, destY, sizeX, sizeY, operation, value, tags...) \
	({ULONG _tags[] = {tags}; ProcessPixelArray((rp), (destX), (destY), (sizeX), (sizeY), (operation), (value), (struct TagItem *) _tags);})
#endif

#define BltBitMapAlpha(srcBitMap, srcX, srcY, destBitMap, destX, destY, sizeX, sizeY, taglist) \
	LP9(0xea, ULONG, BltBitMapAlpha, struct BitMap *, srcBitMap, a0, LONG, srcX, d0, LONG, srcY, d1, struct BitMap *, destBitMap, a1, LONG, destX, d2, LONG, destY, d3, LONG, sizeX, d4, LONG, sizeY, d5, struct TagItem *, taglist, a2, \
	, CYBERGRAPHICS_BASE_NAME)

#define BltBitMapRastPortAlpha(srcBitMap, srcX, srcY, destRP, destX, destY, sizeX, sizeY, taglist) \
	LP9(0xf0, ULONG, BltBitMapRastPortAlpha, struct BitMap *, srcBitMap, a0, LONG, srcX, d0, LONG, srcY, d1, struct RastPort *, destRP, a1, LONG, destX, d2, LONG, destY, d3, LONG, sizeX, d4, LONG, sizeY, d5, struct TagItem *, taglist, a2, \
	, CYBERGRAPHICS_BASE_NAME)

#define ScalePixelArrayAlpha(srcRect, srcW, srcH, srcMod, rp, destX, destY, destW, destH, globalAlpha) \
	LP10(0x102, LONG, ScalePixelArrayAlpha, APTR, srcRect, a0, ULONG, srcW, d0, ULONG, srcH, d1, ULONG, srcMod, d2, struct RastPort *, rp, a1, ULONG, destX, d3, ULONG, destY, d4, ULONG, destW, d5, ULONG, destH, d6, ULONG, globalAlpha, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#endif /*  _INLINE_CYBERGRAPHICS_H  */
