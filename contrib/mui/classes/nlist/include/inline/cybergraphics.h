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

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)

#define WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, a1arg, ColorTab, DestX, DestY, SizeX, SizeY, CTFormat) \
	LP11(0xc6, ULONG, WriteLUTPixelArray, APTR, srcRect, a0, UWORD, SrcX, d0, UWORD, SrcY, d1, UWORD, SrcMod, d2, struct RastPort *, a1arg, a1, APTR, ColorTab, a2, UWORD, DestX, d3, UWORD, DestY, d4, UWORD, SizeX, d5, UWORD, SizeY, d6, UBYTE, CTFormat, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#else

#define WriteLUTPixelArray(srcRect, SrcX, SrcY, SrcMod, a1arg, ColorTab, DestX, DestY, SizeX, SizeY, CTFormat) ({ \
  APTR _WriteLUTPixelArray_srcRect = (srcRect); \
  ULONG _WriteLUTPixelArray_SrcX = (SrcX); \
  ULONG _WriteLUTPixelArray_SrcY = (SrcY); \
  ULONG _WriteLUTPixelArray_SrcMod = (SrcMod); \
  struct RastPort * _WriteLUTPixelArray_a1arg = (a1arg); \
  APTR _WriteLUTPixelArray_ColorTab = (ColorTab); \
  ULONG _WriteLUTPixelArray_DestX = (DestX); \
  ULONG _WriteLUTPixelArray_DestY = (DestY); \
  ULONG _WriteLUTPixelArray_SizeX = (SizeX); \
  ULONG _WriteLUTPixelArray_SizeY = (SizeY); \
  ULONG _WriteLUTPixelArray_CTFormat = (CTFormat); \
  ULONG _WriteLUTPixelArray__re = \
  ({ \
  register struct CyberGfxBase * const __WriteLUTPixelArray__bn __asm("a6") = (struct CyberGfxBase *) (CYBERGRAPHICS_BASE_NAME);\
  register ULONG __WriteLUTPixelArray__re __asm("d0"); \
  register APTR __WriteLUTPixelArray_srcRect __asm("a0") = (_WriteLUTPixelArray_srcRect); \
  register ULONG __WriteLUTPixelArray_SrcX __asm("d0") = (_WriteLUTPixelArray_SrcX); \
  register ULONG __WriteLUTPixelArray_SrcY __asm("d1") = (_WriteLUTPixelArray_SrcY); \
  register ULONG __WriteLUTPixelArray_SrcMod __asm("d2") = (_WriteLUTPixelArray_SrcMod); \
  register struct RastPort * __WriteLUTPixelArray_a1arg __asm("a1") = (_WriteLUTPixelArray_a1arg); \
  register APTR __WriteLUTPixelArray_ColorTab __asm("a2") = (_WriteLUTPixelArray_ColorTab); \
  register ULONG __WriteLUTPixelArray_DestX __asm("d3") = (_WriteLUTPixelArray_DestX); \
  register ULONG __WriteLUTPixelArray_DestY __asm("d4") = (_WriteLUTPixelArray_DestY); \
  register ULONG __WriteLUTPixelArray_SizeX __asm("d5") = (_WriteLUTPixelArray_SizeX); \
  register ULONG __WriteLUTPixelArray_SizeY __asm("d6") = (_WriteLUTPixelArray_SizeY); \
  register ULONG __WriteLUTPixelArray_CTFormat __asm("d7") = (_WriteLUTPixelArray_CTFormat); \
  __asm volatile ("jsr a6@(-198:W)" \
  : "=r"(__WriteLUTPixelArray__re) \
  : "r"(__WriteLUTPixelArray__bn), "r"(__WriteLUTPixelArray_srcRect), "r"(__WriteLUTPixelArray_SrcX), "r"(__WriteLUTPixelArray_SrcY), "r"(__WriteLUTPixelArray_SrcMod), "r"(__WriteLUTPixelArray_a1arg), "r"(__WriteLUTPixelArray_ColorTab), "r"(__WriteLUTPixelArray_DestX), "r"(__WriteLUTPixelArray_DestY), "r"(__WriteLUTPixelArray_SizeX), "r"(__WriteLUTPixelArray_SizeY), "r"(__WriteLUTPixelArray_CTFormat)  \
  : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory"); \
  __WriteLUTPixelArray__re; \
  }); \
  _WriteLUTPixelArray__re; \
})

#endif

#define WritePixelArrayAlpha(srcRect, SrcX, SrcY, SrcMod, a1arg, DestX, DestY, SizeX, SizeY, AlphaValue) \
	LP10(0xd8, ULONG, WritePixelArrayAlpha, APTR, srcRect, a0, UWORD, SrcX, d0, UWORD, SrcY, d1, UWORD, SrcMod, d2, struct RastPort *, a1arg, a1, UWORD, DestX, d3, UWORD, DestY, d4, UWORD, SizeX, d5, UWORD, SizeY, d6, ULONG, AlphaValue, d7, \
	, CYBERGRAPHICS_BASE_NAME)

#endif /*  _INLINE_CYBERGRAPHICS_H  */
