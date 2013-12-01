/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_TTENGINE_H
#define _PPCINLINE_TTENGINE_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef TTENGINE_BASE_NAME
#define TTENGINE_BASE_NAME TTEngineBase
#endif /* !TTENGINE_BASE_NAME */

#define TT_SetAttrsA(__p0, __p1) \
	LP2(54, ULONG , TT_SetAttrsA, \
		struct RastPort *, __p0, a1, \
		struct TagItem *, __p1, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_ObtainFamilyListA(__p0) \
	LP1(120, STRPTR *, TT_ObtainFamilyListA, \
		struct TagItem *, __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_TextExtent(__p0, __p1, __p2, __p3) \
	LP4NR(72, TT_TextExtent, \
		struct RastPort *, __p0, a1, \
		APTR , __p1, a0, \
		WORD , __p2, d0, \
		struct TextExtent *, __p3, a2, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_FreeFamilyList(__p0) \
	LP1NR(126, TT_FreeFamilyList, \
		STRPTR *, __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_SetFont(__p0, __p1) \
	LP2(36, BOOL , TT_SetFont, \
		struct RastPort *, __p0, a1, \
		APTR , __p1, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_DoneRastPort(__p0) \
	LP1NR(96, TT_DoneRastPort, \
		struct RastPort *, __p0, a1, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_Text(__p0, __p1, __p2) \
	LP3NR(48, TT_Text, \
		struct RastPort *, __p0, a1, \
		APTR , __p1, a0, \
		ULONG , __p2, d0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_TextLength(__p0, __p1, __p2) \
	LP3(66, ULONG , TT_TextLength, \
		struct RastPort *, __p0, a1, \
		APTR , __p1, a0, \
		ULONG , __p2, d0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_TextFit(__p0, __p1, __p2, __p3, __p4, __p5, __p6, __p7) \
	LP8(78, ULONG , TT_TextFit, \
		struct RastPort *, __p0, a1, \
		APTR , __p1, a0, \
		UWORD , __p2, d0, \
		struct TextExtent *, __p3, a2, \
		struct TextExtent *, __p4, a3, \
		WORD , __p5, d1, \
		UWORD , __p6, d2, \
		UWORD , __p7, d3, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_RequestA(__p0, __p1) \
	LP2(108, struct TagItem *, TT_RequestA, \
		APTR , __p0, a0, \
		struct TagItem *, __p1, a1, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_OpenFontA(__p0) \
	LP1(30, APTR , TT_OpenFontA, \
		struct TagItem *, __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_FreeRequest(__p0) \
	LP1NR(114, TT_FreeRequest, \
		APTR , __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_CloseFont(__p0) \
	LP1NR(42, TT_CloseFont, \
		APTR , __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_GetAttrsA(__p0, __p1) \
	LP2(60, ULONG , TT_GetAttrsA, \
		struct RastPort *, __p0, a1, \
		struct TagItem *, __p1, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_GetPixmapA(__p0, __p1, __p2, __p3) \
	LP4(84, struct TT_Pixmap *, TT_GetPixmapA, \
		APTR , __p0, a1, \
		APTR , __p1, a2, \
		ULONG , __p2, d0, \
		struct TagItem *, __p3, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_FreePixmap(__p0) \
	LP1NR(90, TT_FreePixmap, \
		struct TT_Pixmap *, __p0, a0, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#define TT_AllocRequest() \
	LP0(102, APTR , TT_AllocRequest, \
		, TTENGINE_BASE_NAME, 0, 0, 0, 0, 0, 0)

#ifdef USE_INLINE_STDARG

#include <stdarg.h>

#define TT_SetAttrs(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_SetAttrsA(__p0, (struct TagItem *)_tags);})

#define TT_ObtainFamilyList(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_ObtainFamilyListA((struct TagItem *)_tags);})

#define TT_Request(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_RequestA(__p0, (struct TagItem *)_tags);})

#define TT_OpenFont(...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_OpenFontA((struct TagItem *)_tags);})

#define TT_GetAttrs(__p0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_GetAttrsA(__p0, (struct TagItem *)_tags);})

#define TT_GetPixmap(__p0, __p1, __p2, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; \
	TT_GetPixmapA(__p0, __p1, __p2, (struct TagItem *)_tags);})

#endif

#endif /* !_PPCINLINE_TTENGINE_H */
