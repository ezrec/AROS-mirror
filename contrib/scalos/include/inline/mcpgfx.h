#ifndef _INLINE_MCPGFX_H
#define _INLINE_MCPGFX_H

#ifndef CLIB_MCPGFX_PROTOS_H
#define CLIB_MCPGFX_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

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

#ifndef MCPGFX_BASE_NAME
#define MCPGFX_BASE_NAME MCPGfxBase
#endif

#define mcpPaintSysIGad(sysImageObject, drawInfo, gadgetNumber, width, height) \
	LP5NR(0x1e, mcpPaintSysIGad, APTR, sysImageObject, a0, struct DrawInfo *, drawInfo, a1, LONG, gadgetNumber, d0, LONG, width, d1, LONG, height, d2, \
	, MCPGFX_BASE_NAME)

#define mcpRectFillA(rp, x1, y1, x2, y2, tagList) \
	LP6NR(0x24, mcpRectFillA, struct RastPort *, rp, a0, LONG, x1, d0, LONG, y1, d1, LONG, x2, d2, LONG, y2, d3, CONST struct TagItem *, tagList, a1, \
	, MCPGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define mcpRectFill(rp, x1, y1, x2, y2, tags...) \
	({ULONG _tags[] = {tags}; mcpRectFillA((rp), (x1), (y1), (x2), (y2), (CONST struct TagItem *) _tags);})
#endif

#define mcpDrawFrameA(rp, x1, y1, x2, y2, tagList) \
	LP6NR(0x2a, mcpDrawFrameA, struct RastPort *, rp, a0, LONG, x1, d0, LONG, y1, d1, LONG, x2, d2, LONG, y2, d3, struct TagItem *, tagList, a1, \
	, MCPGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define mcpDrawFrame(rp, x1, y1, x2, y2, tags...) \
	({ULONG _tags[] = {tags}; mcpDrawFrameA((rp), (x1), (y1), (x2), (y2), (struct TagItem *) _tags);})
#endif

#define mcpGetExtDrawInfo(screen, drawInfo) \
	LP2(0x30, struct ExtDrawInfo *, mcpGetExtDrawInfo, struct Screen *, screen, a0, struct DrawInfo *, drawInfo, a1, \
	, MCPGFX_BASE_NAME)

#define mcpGetFrameSize(drawInfo, frameType) \
	LP2(0x36, struct FrameSize *, mcpGetFrameSize, struct DrawInfo *, drawInfo, a0, LONG, frameType, d0, \
	, MCPGFX_BASE_NAME)

#define mcpSetGFXAttrsA(tagList) \
	LP1NR(0x3c, mcpSetGFXAttrsA, struct TagItem *, tagList, a0, \
	, MCPGFX_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define mcpSetGFXAttrs(tags...) \
	({ULONG _tags[] = {tags}; mcpSetGFXAttrsA((struct TagItem *) _tags);})
#endif

#endif /*  _INLINE_MCPGFX_H  */
