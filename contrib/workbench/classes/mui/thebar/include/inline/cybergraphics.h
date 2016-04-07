#ifndef _INLINE_CYBERGRAPHICS_H
#define _INLINE_CYBERGRAPHICS_H

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef CYBERGRAPHICS_BASE_NAME
#define CYBERGRAPHICS_BASE_NAME CyberGfxBase
#endif

#define IsCyberModeID(displayID) \
	LP1(0x36, BOOL, IsCyberModeID, ULONG, displayID, d0, \
	, CYBERGRAPHICS_BASE_NAME)

#define BestCModeIDTagList(BestModeIDTags) \
	LP1(0x3c, ULONG, BestCModeIDTagList, struct TagItem *, BestModeIDTags, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define BestCModeIDTags(tags...) \
	({ULONG _tags[] = {tags}; BestCModeIDTagList((struct TagItem *) _tags);})
#endif

#define CModeRequestTagList(ModeRequest, ModeRequestTags) \
	LP2(0x42, ULONG, CModeRequestTagList, APTR, ModeRequest, a0, struct TagItem *, ModeRequestTags, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define CModeRequestTags(ModeRequest, tags...) \
	({ULONG _tags[] = {tags}; CModeRequestTagList((ModeRequest), (struct TagItem *) _tags);})
#endif

#define AllocCModeListTagList(ModeListTags) \
	LP1(0x48, struct List *, AllocCModeListTagList, struct TagItem *, ModeListTags, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define AllocCModeListTags(tags...) \
	({ULONG _tags[] = {tags}; AllocCModeListTagList((struct TagItem *) _tags);})
#endif

#define FreeCModeList(ModeList) \
	LP1NR(0x4e, FreeCModeList, struct List *, ModeList, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#define ScalePixelArray(srcRect, SrcW, SrcH, SrcMod, a1arg, DestX, DestY, DestW, DestH, SrcFormat) \
	LP10(0x5a, LONG, ScalePixelArray, APTR, srcRect, a0, UWORD, SrcW, d0, UWORD, SrcH, d1, UWORD, SrcMod, d2, struct RastPort *, a1arg, a1, UWORD, DestX, d3, UWORD, DestY, d4, UWORD, DestW, d5, UWORD, DestH, d6, UBYTE, SrcFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define GetCyberMapAttr(CyberGfxBitmap, CyberAttrTag) \
	LP2(0x60, ULONG, GetCyberMapAttr, struct BitMap *, CyberGfxBitmap, a0, ULONG, CyberAttrTag, d0, \
	, CYBERGRAPHICS_BASE_NAME)

#define GetCyberIDAttr(CyberIDAttr, CyberDisplayModeID) \
	LP2(0x66, ULONG, GetCyberIDAttr, ULONG, CyberIDAttr, d0, ULONG, CyberDisplayModeID, d1, \
	, CYBERGRAPHICS_BASE_NAME)

#define ReadRGBPixel(a1arg, x, y) \
	LP3(0x6c, ULONG, ReadRGBPixel, struct RastPort *, a1arg, a1, UWORD, x, d0, UWORD, y, d1, \
	, CYBERGRAPHICS_BASE_NAME)

#define WriteRGBPixel(a1arg, x, y, argb) \
	LP4(0x72, LONG, WriteRGBPixel, struct RastPort *, a1arg, a1, UWORD, x, d0, UWORD, y, d1, ULONG, argb, d2, \
	, CYBERGRAPHICS_BASE_NAME)

#define ReadPixelArray(destRect, destX, destY, destMod, a1arg, SrcX, SrcY, SizeX, SizeY, DestFormat) \
	LP10(0x78, ULONG, ReadPixelArray, APTR, destRect, a0, UWORD, destX, d0, UWORD, destY, d1, UWORD, destMod, d2, struct RastPort *, a1arg, a1, UWORD, SrcX, d3, UWORD, SrcY, d4, UWORD, SizeX, d5, UWORD, SizeY, d6, UBYTE, DestFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define WritePixelArray(srcRect, SrcX, SrcY, SrcMod, a1arg, DestX, DestY, SizeX, SizeY, SrcFormat) \
	LP10(0x7e, ULONG, WritePixelArray, APTR, srcRect, a0, UWORD, SrcX, d0, UWORD, SrcY, d1, UWORD, SrcMod, d2, struct RastPort *, a1arg, a1, UWORD, DestX, d3, UWORD, DestY, d4, UWORD, SizeX, d5, UWORD, SizeY, d6, UBYTE, SrcFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#define MovePixelArray(SrcX, SrcY, a1arg, DestX, DestY, SizeX, SizeY) \
	LP7(0x84, ULONG, MovePixelArray, UWORD, SrcX, d0, UWORD, SrcY, d1, struct RastPort *, a1arg, a1, UWORD, DestX, d2, UWORD, DestY, d3, UWORD, SizeX, d4, UWORD, SizeY, d5, \
	, CYBERGRAPHICS_BASE_NAME)

#define InvertPixelArray(a1arg, DestX, DestY, SizeX, SizeY) \
	LP5(0x90, ULONG, InvertPixelArray, struct RastPort *, a1arg, a1, UWORD, DestX, d0, UWORD, DestY, d1, UWORD, SizeX, d2, UWORD, SizeY, d3, \
	, CYBERGRAPHICS_BASE_NAME)

#define FillPixelArray(a1arg, DestX, DestY, SizeX, SizeY, ARGB) \
	LP6(0x96, ULONG, FillPixelArray, struct RastPort *, a1arg, a1, UWORD, DestX, d0, UWORD, DestY, d1, UWORD, SizeX, d2, UWORD, SizeY, d3, ULONG, ARGB, d4, \
	, CYBERGRAPHICS_BASE_NAME)

#define DoCDrawMethodTagList(Hook, a1arg, TagList) \
	LP3NR(0x9c, DoCDrawMethodTagList, struct Hook *, Hook, a0, struct RastPort *, a1arg, a1, struct TagItem *, TagList, a2, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define DoCDrawMethodTags(Hook, a1arg, tags...) \
	({ULONG _tags[] = {tags}; DoCDrawMethodTagList((Hook), (a1arg), (struct TagItem *) _tags);})
#endif

#define CVideoCtrlTagList(ViewPort, TagList) \
	LP2NR(0xa2, CVideoCtrlTagList, struct ViewPort *, ViewPort, a0, struct TagItem *, TagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define CVideoCtrlTags(ViewPort, tags...) \
	({ULONG _tags[] = {tags}; CVideoCtrlTagList((ViewPort), (struct TagItem *) _tags);})
#endif

#define LockBitMapTagList(BitMap, TagList) \
	LP2(0xa8, APTR, LockBitMapTagList, APTR, BitMap, a0, struct TagItem *, TagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define LockBitMapTags(BitMap, tags...) \
	({ULONG _tags[] = {tags}; LockBitMapTagList((BitMap), (struct TagItem *) _tags);})
#endif

#define UnLockBitMap(Handle) \
	LP1NR(0xae, UnLockBitMap, APTR, Handle, a0, \
	, CYBERGRAPHICS_BASE_NAME)

#define UnLockBitMapTagList(Handle, TagList) \
	LP2NR(0xb4, UnLockBitMapTagList, APTR, Handle, a0, struct TagItem *, TagList, a1, \
	, CYBERGRAPHICS_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define UnLockBitMapTags(Handle, tags...) \
	({ULONG _tags[] = {tags}; UnLockBitMapTagList((Handle), (struct TagItem *) _tags);})
#endif

#define ExtractColor(a0arg, BitMap, Colour, SrcX, SrcY, Width, Height) \
	LP7(0xba, ULONG, ExtractColor, struct RastPort *, a0arg, a0, struct BitMap *, BitMap, a1, ULONG, Colour, d0, ULONG, SrcX, d1, ULONG, SrcY, d2, ULONG, Width, d3, ULONG, Height, d4, \
	, CYBERGRAPHICS_BASE_NAME)

#define WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, a1arg, ColorTab, DestX, DestY, SizeX, SizeY, CTFormat) \
	LP11(0xc6, ULONG, WriteLUTPixelArray, APTR, srcRect, a0, UWORD, SrcX, d0, UWORD, SrcY, d1, UWORD, SrcMod, d2, struct RastPort *, a1arg, a1, APTR, ColorTab, a2, UWORD, DestX, d3, UWORD, DestY, d4, UWORD, SizeX, d5, UWORD, SizeY, d6, UBYTE, CTFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#endif /*  _INLINE_CYBERGRAPHICS_H  */
