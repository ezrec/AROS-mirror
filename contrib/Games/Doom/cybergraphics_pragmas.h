#ifndef _PPCPRAGMA_CYBERGRAPHICS_H
#define _PPCPRAGMA_CYBERGRAPHICS_H

#ifdef __GNUC__
#ifndef _PPCINLINE__CYBERGRAPHICS_H
#include <powerup/ppcinline/cybergraphics.h>
#endif
#else

#ifndef POWERUP_PPCLIB_INTERFACE_H
#include <powerup/ppclib/interface.h>
#endif

#ifndef POWERUP_GCCLIB_PROTOS_H
#include <powerup/gcclib/powerup_protos.h>
#endif

#ifndef NO_PPCINLINE_STDARG
#define NO_PPCINLINE_STDARG
#endif/* SAS C PPC inlines */

#ifndef CYBERGFX_BASE_NAME
#define CYBERGFX_BASE_NAME CyberGfxBase
#endif /* !CYBERGFX_BASE_NAME */

#define AllocCModeListTagList(tagList) _AllocCModeListTagList(CYBERGFX_BASE_NAME, tagList)

static __inline struct List *
_AllocCModeListTagList(void *CyberGfxBase, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a1			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0x48);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((APTR)PPCCallOS(&MyCaos));
}

#ifndef NO_PPCINLINE_STDARG
#define AllocCModeListTags(tags...) \
	({ULONG _tags[] = { tags }; AllocCModeListTagList((struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define BestCModeIDTagList(tagList) _BestCModeIDTagList(CYBERGFX_BASE_NAME, tagList)

static __inline ULONG
_BestCModeIDTagList(void *CyberGfxBase, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0x3c);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#ifndef NO_PPCINLINE_STDARG
#define BestCModeIDTags(tags...) \
	({ULONG _tags[] = { tags }; BestCModeIDTagList((struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define CModeRequestTagList(requester, tagList) _CModeRequestTagList(CYBERGFX_BASE_NAME, requester, tagList)

static __inline ULONG
_CModeRequestTagList(void *CyberGfxBase, struct Requester *requester, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) requester;
	MyCaos.a1			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0x42);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#ifndef NO_PPCINLINE_STDARG
#define CModeRequestTags(a0, tags...) \
	({ULONG _tags[] = { tags }; CModeRequestTagList((a0), (struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define CVideoCtrlTagList(viewPort, tagList) _CVideoCtrlTagList(CYBERGFX_BASE_NAME, viewPort, tagList)

static __inline void
_CVideoCtrlTagList(void *CyberGfxBase, struct ViewPort *viewPort, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) viewPort;
	MyCaos.a1			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0xa2);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	PPCCallOS(&MyCaos);
}

#ifndef NO_PPCINLINE_STDARG
#define CVideoCtrlTags(a0, tags...) \
	({ULONG _tags[] = { tags }; CVideoCtrlTagList((a0), (struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define DoCDrawMethodTagList(hook, rastPort, tagList) _DoCDrawMethodTagList(CYBERGFX_BASE_NAME, hook, rastPort, tagList)

static __inline void
_DoCDrawMethodTagList(void *CyberGfxBase, struct Hook *hook, struct RastPort *rastPort, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) hook;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.a2			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0x9c);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	PPCCallOS(&MyCaos);
}

#ifndef NO_PPCINLINE_STDARG
#define DoCDrawMethodTags(a0, a1, tags...) \
	({ULONG _tags[] = { tags }; DoCDrawMethodTagList((a0), (a1), (struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define FillPixelArray(rastPort, destX, destY, sizeX, sizeY, ARGB) _FillPixelArray(CYBERGFX_BASE_NAME, rastPort, destX, destY, sizeX, sizeY, ARGB)

static __inline ULONG
_FillPixelArray(void *CyberGfxBase, struct RastPort *rastPort, UWORD destX, UWORD destY, UWORD sizeX, UWORD sizeY, ULONG ARGB)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) destX;
	MyCaos.d1			=	(ULONG) destY;
	MyCaos.d2			=	(ULONG) sizeX;
	MyCaos.d3			=	(ULONG) sizeY;
	MyCaos.d4			=	ARGB;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x96);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define FreeCModeList(modeList) _FreeCModeList(CYBERGFX_BASE_NAME, modeList)

static __inline void
_FreeCModeList(void *CyberGfxBase, struct List *modeList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) modeList;
	MyCaos.caos_Un.Offset		=	(-0x4e);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	PPCCallOS(&MyCaos);
}

#define GetCyberIDAttr(attribute, displayModeID) _GetCyberIDAttr(CYBERGFX_BASE_NAME, attribute, displayModeID)

static __inline ULONG
_GetCyberIDAttr(void *CyberGfxBase, ULONG attribute, ULONG displayModeID)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	attribute;
	MyCaos.d1			=	displayModeID;
	MyCaos.caos_Un.Offset		=	(-0x66);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define GetCyberMapAttr(bitMap, attribute) _GetCyberMapAttr(CYBERGFX_BASE_NAME, bitMap, attribute)

static __inline ULONG
_GetCyberMapAttr(void *CyberGfxBase, struct BitMap *bitMap, ULONG attribute)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d1			=	attribute;
	MyCaos.a0			=	(ULONG) bitMap;
	MyCaos.caos_Un.Offset		=	(-0x60);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define InvertPixelArray(rastPort, destX, destY, sizeX, sizeY) _InvertPixelArray(CYBERGFX_BASE_NAME, rastPort, destX, destY, sizeX, sizeY)

static __inline ULONG
_InvertPixelArray(void *CyberGfxBase, struct RastPort *rastPort, UWORD destX, UWORD destY, UWORD sizeX, UWORD sizeY)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) destX;
	MyCaos.d1			=	(ULONG) destY;
	MyCaos.d2			=	(ULONG) sizeX;
	MyCaos.d3			=	(ULONG) sizeY;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x90);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define IsCyberModeID(displayModeID) _IsCyberModeID(CYBERGFX_BASE_NAME, displayModeID)

static __inline BOOL
_IsCyberModeID(void *CyberGfxBase, ULONG displayModeID)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	displayModeID;
	MyCaos.caos_Un.Offset		=	(-0x36);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((BOOL)PPCCallOS(&MyCaos));
}

#define LockBitMapTagList(bitMap, tagList) _LockBitMapTagList(CYBERGFX_BASE_NAME, bitMap, tagList)

static __inline APTR
_LockBitMapTagList(void *CyberGfxBase, struct BitMap *bitMap, struct TagItem *tagList)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) bitMap;
	MyCaos.a1			=	(ULONG) tagList;
	MyCaos.caos_Un.Offset		=	(-0xa8);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((APTR)PPCCallOS(&MyCaos));
}

#ifndef NO_PPCINLINE_STDARG
#define LockBitMapTags(a0, tags...) \
	({ULONG _tags[] = { tags }; LockBitMapTagList((a0), (struct TagItem *)_tags);})
#endif /* !NO_PPCINLINE_STDARG */

#define MovePixelArray(srcX, srcY, rastPort, destX, destY, sizeX, sizeY) _MovePixelArray(CYBERGFX_BASE_NAME, srcX, srcY, rastPort, destX, destY, sizeX, sizeY)

static __inline ULONG
_MovePixelArray(void *CyberGfxBase, UWORD srcX, UWORD srcY, struct RastPort *rastPort, UWORD destX, UWORD destY, UWORD sizeX, UWORD sizeY)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) srcX;
	MyCaos.d1			=	(ULONG) srcY;
	MyCaos.d2			=	(ULONG) destX;
	MyCaos.d3			=	(ULONG) destY;
	MyCaos.d4			=	(ULONG) sizeX;
	MyCaos.d5			=	(ULONG) sizeY;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x84);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define ReadPixelArray(destRect, destX, destY, destMod, rastPort, srcX, srcY, sizeX, sizeY, destFormat) _ReadPixelArray(CYBERGFX_BASE_NAME, destRect, destX, destY, destMod, rastPort, srcX, srcY, sizeX, sizeY, destFormat)

static __inline ULONG
_ReadPixelArray(void *CyberGfxBase, APTR destRect, UWORD destX, UWORD destY, UWORD destMod, struct RastPort *rastPort, UWORD srcX, UWORD srcY, UWORD sizeX, UWORD sizeY, UBYTE destFormat)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) destX;
	MyCaos.d1			=	(ULONG) destY;
	MyCaos.d2			=	(ULONG) destMod;
	MyCaos.d3			=	(ULONG) srcX;
	MyCaos.d4			=	(ULONG) srcY;
	MyCaos.d5			=	(ULONG) sizeX;
	MyCaos.d6			=	(ULONG) sizeY;
	MyCaos.d7			=	(ULONG) destFormat;
	MyCaos.a0			=	(ULONG) destRect;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x78);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define ReadRGBPixel(rastPort, x, y) _ReadRGBPixel(CYBERGFX_BASE_NAME, rastPort, x, y)

static __inline ULONG
_ReadRGBPixel(void *CyberGfxBase, struct RastPort *rastPort, UWORD x, UWORD y)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) x;
	MyCaos.d1			=	(ULONG) y;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x6c);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define ScalePixelArray(srcRect, srcW, srcH, srcMod, rastPort, destX, destY, destW, destH, srcFormat) _ScalePixelArray(CYBERGFX_BASE_NAME, srcRect, srcW, srcH, srcMod, rastPort, destX, destY, destW, destH, srcFormat)

static __inline LONG
_ScalePixelArray(void *CyberGfxBase, APTR srcRect, UWORD srcW, UWORD srcH, UWORD srcMod, struct RastPort *rastPort, UWORD destX, UWORD destY, UWORD destW, UWORD destH, UBYTE srcFormat)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) srcW;
	MyCaos.d1			=	(ULONG) srcH;
	MyCaos.d2			=	(ULONG) srcMod;
	MyCaos.d3			=	(ULONG) destX;
	MyCaos.d4			=	(ULONG) destY;
	MyCaos.d5			=	(ULONG) destW;
	MyCaos.d6			=	(ULONG) destH;
	MyCaos.d7			=	(ULONG) srcFormat;
	MyCaos.a0			=	(ULONG) srcRect;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x5a);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((LONG)PPCCallOS(&MyCaos));
}

#define UnLockBitMap(handle) _UnLockBitMap(CYBERGFX_BASE_NAME, handle)

static __inline void
_UnLockBitMap(void *CyberGfxBase, APTR handle)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.a0			=	(ULONG) handle;
	MyCaos.caos_Un.Offset		=	(-0xae);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	PPCCallOS(&MyCaos);
}

#define WritePixelArray(srcRect, srcX, srcY, srcMod, rastPort, destX, destY, sizeX, sizeY, srcFormat) _WritePixelArray(CYBERGFX_BASE_NAME, srcRect, srcX, srcY, srcMod, rastPort, destX, destY, sizeX, sizeY, srcFormat)

static __inline ULONG
_WritePixelArray(void *CyberGfxBase, APTR srcRect, UWORD srcX, UWORD srcY, UWORD srcMod, struct RastPort *rastPort, UWORD destX, UWORD destY, UWORD sizeX, UWORD sizeY, UBYTE srcFormat)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) srcX;
	MyCaos.d1			=	(ULONG) srcY;
	MyCaos.d2			=	(ULONG) srcMod;
	MyCaos.d3			=	(ULONG) destX;
	MyCaos.d4			=	(ULONG) destY;
	MyCaos.d5			=	(ULONG) sizeX;
	MyCaos.d6			=	(ULONG) sizeY;
	MyCaos.d7			=	(ULONG) srcFormat;
	MyCaos.a0			=	(ULONG) srcRect;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x7e);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((ULONG)PPCCallOS(&MyCaos));
}

#define WriteRGBPixel(rastPort, x, y, color) _WriteRGBPixel(CYBERGFX_BASE_NAME, rastPort, x, y, color)

static __inline LONG
_WriteRGBPixel(void *CyberGfxBase, struct RastPort *rastPort, UWORD x, UWORD y, ULONG color)
{
struct Caos	MyCaos;
	MyCaos.M68kCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.M68kStart		=	NULL;
//	MyCaos.M68kSize			=	0;
	MyCaos.PPCCacheMode		=	IF_CACHEFLUSHALL;
//	MyCaos.PPCStart			=	NULL;
//	MyCaos.PPCSize			=	0;
	MyCaos.d0			=	(ULONG) x;
	MyCaos.d1			=	(ULONG) y;
	MyCaos.d2			=	color;
	MyCaos.a1			=	(ULONG) rastPort;
	MyCaos.caos_Un.Offset		=	(-0x72);
	MyCaos.a6			=	(ULONG) CyberGfxBase;
	return((LONG)PPCCallOS(&MyCaos));
}

#endif /* SASC Pragmas */
#endif /* !_PPCPRAGMA_CYBERGRAPHICS_H */
