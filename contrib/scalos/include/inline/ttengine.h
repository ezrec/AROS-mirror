#ifndef _INLINE_TTENGINE_H
#define _INLINE_TTENGINE_H

#ifndef CLIB_TTENGINE_PROTOS_H
#define CLIB_TTENGINE_PROTOS_H
#endif

#ifndef __INLINE_MACROS_H
#include <inline/macros.h>
#endif

#ifndef  EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef  UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef  LIBRARIES_TTENGINE_H
#include <libraries/ttengine.h>
#endif
#ifndef  GRAPHICS_TEXT_H
#include <graphics/text.h>
#endif

#ifndef TTENGINE_BASE_NAME
#define TTENGINE_BASE_NAME TTEngineBase
#endif

#define TT_OpenFontA(taglist) \
	LP1(0x1e, APTR, TT_OpenFontA, struct TagItem *, taglist, a0, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_OpenFont(tags...) \
	({ULONG _tags[] = {tags}; TT_OpenFontA((struct TagItem *) _tags);})
#endif

#define TT_SetFont(rp, font) \
	LP2(0x24, BOOL, TT_SetFont, struct RastPort *, rp, a1, APTR, font, a0, \
	, TTENGINE_BASE_NAME)

#define TT_CloseFont(font) \
	LP1NR(0x2a, TT_CloseFont, APTR, font, a0, \
	, TTENGINE_BASE_NAME)

#define TT_Text(rp, string, count) \
	LP3NR(0x30, TT_Text, struct RastPort *, rp, a1, APTR, string, a0, ULONG, count, d0, \
	, TTENGINE_BASE_NAME)

#define TT_SetAttrsA(rp, taglist) \
	LP2(0x36, ULONG, TT_SetAttrsA, struct RastPort *, rp, a1, struct TagItem *, taglist, a0, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_SetAttrs(rp, tags...) \
	({ULONG _tags[] = {tags}; TT_SetAttrsA((rp), (struct TagItem *) _tags);})
#endif

#define TT_GetAttrsA(rp, taglist) \
	LP2(0x3c, ULONG, TT_GetAttrsA, struct RastPort *, rp, a1, struct TagItem *, taglist, a0, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_GetAttrs(rp, tags...) \
	({ULONG _tags[] = {tags}; TT_GetAttrsA((rp), (struct TagItem *) _tags);})
#endif

#define TT_TextLength(rp, string, count) \
	LP3(0x42, ULONG, TT_TextLength, struct RastPort *, rp, a1, APTR, string, a0, ULONG, count, d0, \
	, TTENGINE_BASE_NAME)

#define TT_TextExtent(rp, string, count, te) \
	LP4NR(0x48, TT_TextExtent, struct RastPort *, rp, a1, APTR, string, a0, LONG, count, d0, struct TextExtent *, te, a2, \
	, TTENGINE_BASE_NAME)

#define TT_TextFit(rp, string, count, te, tec, dir, cwidth, cheight) \
	LP8(0x4e, ULONG, TT_TextFit, struct RastPort *, rp, a1, APTR, string, a0, ULONG, count, d0, struct TextExtent *, te, a2, struct TextExtent *, tec, a3, LONG, dir, d1, ULONG, cwidth, d2, ULONG, cheight, d3, \
	, TTENGINE_BASE_NAME)

#define TT_GetPixmapA(font, string, count, taglist) \
	LP4(0x54, struct TT_Pixmap *, TT_GetPixmapA, APTR, font, a1, APTR, string, a2, ULONG, count, d0, struct TagItem *, taglist, a0, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_GetPixmap(font, string, count, tags...) \
	({ULONG _tags[] = {tags}; TT_GetPixmapA((font), (string), (count), (struct TagItem *) _tags);})
#endif

#define TT_FreePixmap(pixmap) \
	LP1NR(0x5a, TT_FreePixmap, struct TT_Pixmap *, pixmap, a0, \
	, TTENGINE_BASE_NAME)

#define TT_DoneRastPort(rp) \
	LP1NR(0x60, TT_DoneRastPort, struct RastPort *, rp, a1, \
	, TTENGINE_BASE_NAME)

#define TT_AllocRequest() \
	LP0(0x66, APTR, TT_AllocRequest, \
	, TTENGINE_BASE_NAME)

#define TT_RequestA(request, taglist) \
	LP2(0x6c, struct TagItem*, TT_RequestA, APTR, request, a0, struct TagItem *, taglist, a1, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_Request(request, tags...) \
	({ULONG _tags[] = {tags}; TT_RequestA((request), (struct TagItem *) _tags);})
#endif

#define TT_FreeRequest(request) \
	LP1NR(0x72, TT_FreeRequest, APTR, request, a0, \
	, TTENGINE_BASE_NAME)

#define TT_ObtainFamilyListA(taglist) \
	LP1(0x78, STRPTR *, TT_ObtainFamilyListA, struct TagItem *, taglist, a0, \
	, TTENGINE_BASE_NAME)

#ifndef NO_INLINE_STDARG
#define TT_ObtainFamilyList(tags...) \
	({ULONG _tags[] = {tags}; TT_ObtainFamilyListA((struct TagItem *) _tags);})
#endif

#define TT_FreeFamilyList(list) \
	LP1NR(0x7e, TT_FreeFamilyList, STRPTR *, list, a0, \
	, TTENGINE_BASE_NAME)

#endif /*  _INLINE_TTENGINE_H  */
