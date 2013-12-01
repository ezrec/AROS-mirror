#ifndef  CLIB_MCPGFX_PROTOS_H
#define  CLIB_MCPGFX_PROTOS_H

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
#ifndef  LIBRARIES_MCPGFX_H
#include <libraries/mcpgfx.h>
#endif
#ifndef  GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif
#ifndef  INTUITION_SCREENS_H
#include <intuition/screens.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
VOID mcpPaintSysIGad( APTR sysImageObject, struct DrawInfo *drawInfo, LONG gadgetNumber, LONG width, LONG height );
VOID mcpRectFillA( struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2, CONST struct TagItem *tagList );
VOID mcpRectFill( struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2, Tag tagList, ... );
VOID mcpDrawFrameA( struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2, struct TagItem *tagList );
VOID mcpDrawFrame( struct RastPort *rp, LONG x1, LONG y1, LONG x2, LONG y2, Tag tagList, ... );
struct ExtDrawInfo *mcpGetExtDrawInfo( struct Screen *screen, struct DrawInfo *drawInfo );
struct FrameSize *mcpGetFrameSize( struct DrawInfo *drawInfo, LONG frameType );
VOID mcpSetGFXAttrsA( struct TagItem *tagList );
VOID mcpSetGFXAttrs( Tag tagList, ... );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_MCPGFX_PROTOS_H */
