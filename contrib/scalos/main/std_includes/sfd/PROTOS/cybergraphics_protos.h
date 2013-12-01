#ifndef  CLIB_CYBERGRAPHICS_PROTOS_H
#define  CLIB_CYBERGRAPHICS_PROTOS_H

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

#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
BOOL IsCyberModeID( ULONG displayID );
ULONG BestCModeIDTagList( struct TagItem *bestModeIDTags );
ULONG BestCModeIDTags( Tag bestModeIDTags, ... );
ULONG CModeRequestTagList( APTR modeRequest, struct TagItem *modeRequestTags );
ULONG CModeRequestTags( APTR modeRequest, Tag modeRequestTags, ... );
struct List *AllocCModeListTagList( struct TagItem *modeListTags );
struct List *AllocCModeListTags( Tag modeListTags, ... );
VOID FreeCModeList( struct List *modeList );
LONG ScalePixelArray( APTR srcRect, ULONG srcW, ULONG srcH, ULONG srcMod, struct RastPort *rp, ULONG destX, ULONG destY, ULONG destW, ULONG destH, ULONG srcFormat );
ULONG GetCyberMapAttr( struct BitMap *cyberGfxBitmap, ULONG cyberAttrTag );
ULONG GetCyberIDAttr( ULONG cyberIDAttr, ULONG cyberDisplayModeID );
ULONG ReadRGBPixel( struct RastPort *rp, ULONG x, ULONG y );
LONG WriteRGBPixel( struct RastPort *rp, ULONG x, ULONG y, ULONG argb );
ULONG ReadPixelArray( APTR destRect, ULONG destX, ULONG destY, ULONG destMod, struct RastPort *rp, ULONG srcX, ULONG srcY, ULONG sizeX, ULONG sizeY, ULONG destFormat );
ULONG WritePixelArray( APTR srcRect, ULONG srcX, ULONG srcY, ULONG srcMod, struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG srcFormat );
ULONG MovePixelArray( ULONG srcX, ULONG srcY, struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY );
ULONG InvertPixelArray( struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY );
ULONG FillPixelArray( struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG aRGB );
VOID DoCDrawMethodTagList( struct Hook *hook, struct RastPort *rp, struct TagItem *tagList );
VOID DoCDrawMethodTags( struct Hook *hook, struct RastPort *rp, Tag tagList, ... );
VOID CVideoCtrlTagList( struct ViewPort *viewPort, struct TagItem *tagList );
VOID CVideoCtrlTags( struct ViewPort *viewPort, Tag tagList, ... );
APTR LockBitMapTagList( APTR bitMap, struct TagItem *tagList );
APTR LockBitMapTags( APTR bitMap, Tag tagList, ... );
VOID UnLockBitMap( APTR handle );
VOID UnLockBitMapTagList( APTR handle, struct TagItem *tagList );
VOID UnLockBitMapTags( APTR handle, Tag tagList, ... );
ULONG ExtractColor( struct RastPort *rp, struct BitMap *bitMap, ULONG colour, ULONG srcX, ULONG srcY, ULONG width, ULONG height );
ULONG WriteLUTPixelArray( APTR srcRect, ULONG srcX, ULONG srcY, ULONG srcMod, struct RastPort *rp, APTR colorTab, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG cTFormat );
/* --- function in v43 or higher */
ULONG WritePixelArrayAlpha( APTR srcRect, ULONG srcX, ULONG srcY, ULONG srcMod, struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG globalAlpha );
VOID BltTemplateAlpha( UBYTE *source, LONG xSrc, LONG srcMod, struct RastPort *destRP, ULONG xDest, ULONG yDest, ULONG xSize, ULONG ySize );
VOID ProcessPixelArray( struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG operation, LONG value, struct TagItem *taglist );
VOID ProcessPixelArrayTags( struct RastPort *rp, ULONG destX, ULONG destY, ULONG sizeX, ULONG sizeY, ULONG operation, LONG value, Tag taglist, ... );
/* --- function in v50 or higher */
ULONG BltBitMapAlpha( struct BitMap *srcBitMap, LONG srcX, LONG srcY, struct BitMap *destBitMap, LONG destX, LONG destY, LONG sizeX, LONG sizeY, struct TagItem *taglist );
ULONG BltBitMapRastPortAlpha( struct BitMap *srcBitMap, LONG srcX, LONG srcY, struct RastPort *destRP, LONG destX, LONG destY, LONG sizeX, LONG sizeY, struct TagItem *taglist );
LONG ScalePixelArrayAlpha( APTR srcRect, ULONG srcW, ULONG srcH, ULONG srcMod, struct RastPort *rp, ULONG destX, ULONG destY, ULONG destW, ULONG destH, ULONG globalAlpha );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_CYBERGRAPHICS_PROTOS_H */
