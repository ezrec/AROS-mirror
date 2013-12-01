#ifndef  CLIB_GUIGFX_PROTOS_H
#define  CLIB_GUIGFX_PROTOS_H

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

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
APTR MakePictureA( APTR array, ULONG width, ULONG height, struct TagItem *tags );
APTR MakePicture( APTR array, ULONG width, ULONG height, ULONG firstTag, ... );
APTR LoadPictureA( STRPTR filename, struct TagItem *tags );
APTR LoadPicture( STRPTR filename, ULONG firstTag, ... );
APTR ReadPictureA( struct RastPort *rp, struct ColorMap *colormap, ULONG x, ULONG y, ULONG width, ULONG height, struct TagItem *tags );
APTR ReadPicture( struct RastPort *rp, struct ColorMap *colormap, ULONG x, ULONG y, ULONG width, ULONG height, ULONG firstTag, ... );
APTR ClonePictureA( APTR pic, struct TagItem *tags );
APTR ClonePicture( APTR pic, ULONG firstTag, ... );
VOID DeletePicture( APTR pic );
APTR AddPictureA( APTR psm, APTR pic, struct TagItem *tags );
APTR AddPicture( APTR psm, APTR pic, ULONG firstTag, ... );
APTR AddPaletteA( APTR psm, APTR palette, struct TagItem *tags );
APTR AddPalette( APTR psm, APTR palette, ULONG firstTag, ... );
APTR AddPixelArrayA( APTR psm, APTR array, ULONG width, ULONG height, struct TagItem *tags );
APTR AddPixelArray( APTR psm, APTR array, ULONG width, ULONG height, ULONG firstTag, ... );
VOID RemColorHandle( APTR colorhandle );
APTR CreatePenShareMapA( struct TagItem *tags );
APTR CreatePenShareMap( Tag tags, ... );
VOID DeletePenShareMap( APTR psm );
APTR ObtainDrawHandleA( APTR psm, struct RastPort *rp, struct ColorMap *cm, struct TagItem *tags );
APTR ObtainDrawHandle( APTR psm, struct RastPort *rp, struct ColorMap *cm, ULONG firstTag, ... );
VOID ReleaseDrawHandle( APTR drawhandle );
BOOL DrawPictureA( APTR drawhandle, APTR pic, ULONG x, ULONG y, struct TagItem *tags );
BOOL DrawPicture( APTR drawhandle, APTR pic, ULONG x, ULONG y, ULONG firstTag, ... );
BOOL MapPaletteA( APTR drawhandle, APTR palette, UBYTE *pentab, struct TagItem *tags );
BOOL MapPalette( APTR drawhandle, APTR palette, UBYTE *pentab, ULONG firstTag, ... );
LONG MapPenA( APTR drawhandle, ULONG rgb, struct TagItem *tags );
LONG MapPen( APTR drawhandle, ULONG rgb, ULONG firstTag, ... );
struct BitMap *CreatePictureBitMapA( APTR drawhandle, APTR pic, struct TagItem *tags );
struct BitMap *CreatePictureBitMap( APTR drawhandle, APTR pic, ULONG firstTag, ... );
ULONG DoPictureMethodA( APTR pic, ULONG method, ULONG *arguments );
ULONG DoPictureMethod( APTR pic, ULONG method, ULONG firstTag, ... );
ULONG GetPictureAttrsA( APTR pic, struct TagItem *tags );
ULONG GetPictureAttrs( APTR pic, ULONG firstTag, ... );
ULONG LockPictureA( APTR pic, ULONG mode, ULONG *args );
ULONG LockPicture( APTR pic, ULONG mode, ULONG firstTag, ... );
VOID UnLockPicture( APTR pic, ULONG mode );
BOOL IsPictureA( STRPTR filename, struct TagItem *tags );
BOOL IsPicture( STRPTR filename, ULONG firstTag, ... );
APTR CreateDirectDrawHandleA( APTR drawhandle, ULONG sw, ULONG sh, ULONG dw, ULONG dh, struct TagItem *tags );
APTR CreateDirectDrawHandle( APTR drawhandle, ULONG sw, ULONG sh, ULONG dw, ULONG dh, ULONG firstTag, ... );
VOID DeleteDirectDrawHandle( APTR ddh );
BOOL DirectDrawTrueColorA( APTR ddh, ULONG *array, ULONG x, ULONG y, struct TagItem *tags );
BOOL DirectDrawTrueColor( APTR ddh, ULONG *array, ULONG x, ULONG y, ULONG firstTag, ... );
BOOL CreatePictureMaskA( APTR pic, UBYTE *mask, ULONG maskwidth, struct TagItem *tags );
BOOL CreatePictureMask( APTR pic, UBYTE *mask, ULONG maskwidth, ULONG firstTag, ... );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_GUIGFX_PROTOS_H */
