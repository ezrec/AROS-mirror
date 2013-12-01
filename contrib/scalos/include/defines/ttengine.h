/* Automatically generated header! Do not edit! */

#ifndef _INLINE_TTENGINE_H
#define _INLINE_TTENGINE_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef TTENGINE_BASE_NAME
#define TTENGINE_BASE_NAME TTEngineBase
#endif /* !TTENGINE_BASE_NAME */

#define TT_OpenFontA(___taglist) __TT_OpenFontA_WB(TTENGINE_BASE_NAME, ___taglist)
#define __TT_OpenFontA_WB(___base, ___taglist) \
	AROS_LC1(APTR, TT_OpenFontA, \
	AROS_LCA(struct TagItem *, (___taglist), A0), \
	struct Library*, (___base), 5, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_OpenFont(___tag1, ...) __TT_OpenFont_WB(TTENGINE_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __TT_OpenFont_WB(___base, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_OpenFontA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_SetFont(___rp, ___font) __TT_SetFont_WB(TTENGINE_BASE_NAME, ___rp, ___font)
#define __TT_SetFont_WB(___base, ___rp, ___font) \
	AROS_LC2(BOOL, TT_SetFont, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(APTR, (___font), A0), \
	struct Library*, (___base), 6, Ttengine)

#define TT_CloseFont(___font) __TT_CloseFont_WB(TTENGINE_BASE_NAME, ___font)
#define __TT_CloseFont_WB(___base, ___font) \
	AROS_LC1(VOID, TT_CloseFont, \
	AROS_LCA(APTR, (___font), A0), \
	struct Library*, (___base), 7, Ttengine)

#define TT_Text(___rp, ___string, ___count) __TT_Text_WB(TTENGINE_BASE_NAME, ___rp, ___string, ___count)
#define __TT_Text_WB(___base, ___rp, ___string, ___count) \
	AROS_LC3(VOID, TT_Text, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(APTR, (___string), A0), \
	AROS_LCA(ULONG, (___count), D0), \
	struct Library*, (___base), 8, Ttengine)

#define TT_SetAttrsA(___rp, ___taglist) __TT_SetAttrsA_WB(TTENGINE_BASE_NAME, ___rp, ___taglist)
#define __TT_SetAttrsA_WB(___base, ___rp, ___taglist) \
	AROS_LC2(ULONG, TT_SetAttrsA, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(struct TagItem *, (___taglist), A0), \
	struct Library*, (___base), 9, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_SetAttrs(___rp, ___tag1, ...) __TT_SetAttrs_WB(TTENGINE_BASE_NAME, ___rp, ___tag1, ## __VA_ARGS__)
#define __TT_SetAttrs_WB(___base, ___rp, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_SetAttrsA_WB((___base), (___rp), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_GetAttrsA(___rp, ___taglist) __TT_GetAttrsA_WB(TTENGINE_BASE_NAME, ___rp, ___taglist)
#define __TT_GetAttrsA_WB(___base, ___rp, ___taglist) \
	AROS_LC2(ULONG, TT_GetAttrsA, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(struct TagItem *, (___taglist), A0), \
	struct Library*, (___base), 10, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_GetAttrs(___rp, ___tag1, ...) __TT_GetAttrs_WB(TTENGINE_BASE_NAME, ___rp, ___tag1, ## __VA_ARGS__)
#define __TT_GetAttrs_WB(___base, ___rp, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_GetAttrsA_WB((___base), (___rp), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_TextLength(___rp, ___string, ___count) __TT_TextLength_WB(TTENGINE_BASE_NAME, ___rp, ___string, ___count)
#define __TT_TextLength_WB(___base, ___rp, ___string, ___count) \
	AROS_LC3(ULONG, TT_TextLength, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(APTR, (___string), A0), \
	AROS_LCA(ULONG, (___count), D0), \
	struct Library*, (___base), 11, Ttengine)

#define TT_TextExtent(___rp, ___string, ___count, ___te) __TT_TextExtent_WB(TTENGINE_BASE_NAME, ___rp, ___string, ___count, ___te)
#define __TT_TextExtent_WB(___base, ___rp, ___string, ___count, ___te) \
	AROS_LC4(VOID, TT_TextExtent, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(APTR, (___string), A0), \
	AROS_LCA(WORD, (___count), D0), \
	AROS_LCA(struct TextExtent *, (___te), A2), \
	struct Library*, (___base), 12, Ttengine)

#define TT_TextFit(___rp, ___string, ___count, ___te, ___tec, ___dir, ___cwidth, ___cheight) __TT_TextFit_WB(TTENGINE_BASE_NAME, ___rp, ___string, ___count, ___te, ___tec, ___dir, ___cwidth, ___cheight)
#define __TT_TextFit_WB(___base, ___rp, ___string, ___count, ___te, ___tec, ___dir, ___cwidth, ___cheight) \
	AROS_LC8(ULONG, TT_TextFit, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	AROS_LCA(APTR, (___string), A0), \
	AROS_LCA(UWORD, (___count), D0), \
	AROS_LCA(struct TextExtent *, (___te), A2), \
	AROS_LCA(struct TextExtent *, (___tec), A3), \
	AROS_LCA(WORD, (___dir), D1), \
	AROS_LCA(UWORD, (___cwidth), D2), \
	AROS_LCA(UWORD, (___cheight), D3), \
	struct Library*, (___base), 13, Ttengine)

#define TT_GetPixmapA(___font, ___string, ___count, ___taglist) __TT_GetPixmapA_WB(TTENGINE_BASE_NAME, ___font, ___string, ___count, ___taglist)
#define __TT_GetPixmapA_WB(___base, ___font, ___string, ___count, ___taglist) \
	AROS_LC4(struct TT_Pixmap *, TT_GetPixmapA, \
	AROS_LCA(APTR, (___font), A1), \
	AROS_LCA(APTR, (___string), A2), \
	AROS_LCA(ULONG, (___count), D0), \
	AROS_LCA(struct TagItem *, (___taglist), A0), \
	struct Library*, (___base), 14, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_GetPixmap(___font, ___string, ___count, ___tag1, ...) __TT_GetPixmap_WB(TTENGINE_BASE_NAME, ___font, ___string, ___count, ___tag1, ## __VA_ARGS__)
#define __TT_GetPixmap_WB(___base, ___font, ___string, ___count, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_GetPixmapA_WB((___base), (___font), (___string), (___count), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_FreePixmap(___pixmap) __TT_FreePixmap_WB(TTENGINE_BASE_NAME, ___pixmap)
#define __TT_FreePixmap_WB(___base, ___pixmap) \
	AROS_LC1(VOID, TT_FreePixmap, \
	AROS_LCA(struct TT_Pixmap *, (___pixmap), A0), \
	struct Library*, (___base), 15, Ttengine)

#define TT_DoneRastPort(___rp) __TT_DoneRastPort_WB(TTENGINE_BASE_NAME, ___rp)
#define __TT_DoneRastPort_WB(___base, ___rp) \
	AROS_LC1(VOID, TT_DoneRastPort, \
	AROS_LCA(struct RastPort *, (___rp), A1), \
	struct Library*, (___base), 16, Ttengine)

#define TT_AllocRequest() __TT_AllocRequest_WB(TTENGINE_BASE_NAME)
#define __TT_AllocRequest_WB(___base) \
	AROS_LC0(APTR, TT_AllocRequest, \
	struct Library*, (___base), 17, Ttengine)

#define TT_RequestA(___request, ___taglist) __TT_RequestA_WB(TTENGINE_BASE_NAME, ___request, ___taglist)
#define __TT_RequestA_WB(___base, ___request, ___taglist) \
	AROS_LC2(struct TagItem*, TT_RequestA, \
	AROS_LCA(APTR, (___request), A0), \
	AROS_LCA(struct TagItem *, (___taglist), A1), \
	struct Library*, (___base), 18, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_Request(___request, ___tag1, ...) __TT_Request_WB(TTENGINE_BASE_NAME, ___request, ___tag1, ## __VA_ARGS__)
#define __TT_Request_WB(___base, ___request, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_RequestA_WB((___base), (___request), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_FreeRequest(___request) __TT_FreeRequest_WB(TTENGINE_BASE_NAME, ___request)
#define __TT_FreeRequest_WB(___base, ___request) \
	AROS_LC1(VOID, TT_FreeRequest, \
	AROS_LCA(APTR, (___request), A0), \
	struct Library*, (___base), 19, Ttengine)

#define TT_ObtainFamilyListA(___taglist) __TT_ObtainFamilyListA_WB(TTENGINE_BASE_NAME, ___taglist)
#define __TT_ObtainFamilyListA_WB(___base, ___taglist) \
	AROS_LC1(STRPTR *, TT_ObtainFamilyListA, \
	AROS_LCA(struct TagItem *, (___taglist), A0), \
	struct Library*, (___base), 20, Ttengine)

#ifndef NO_INLINE_STDARG
#define TT_ObtainFamilyList(___tag1, ...) __TT_ObtainFamilyList_WB(TTENGINE_BASE_NAME, ___tag1, ## __VA_ARGS__)
#define __TT_ObtainFamilyList_WB(___base, ___tag1, ...) \
	({ULONG _tags[] = { (ULONG) ___tag1, ## __VA_ARGS__ }; __TT_ObtainFamilyListA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define TT_FreeFamilyList(___list) __TT_FreeFamilyList_WB(TTENGINE_BASE_NAME, ___list)
#define __TT_FreeFamilyList_WB(___base, ___list) \
	AROS_LC1(VOID, TT_FreeFamilyList, \
	AROS_LCA(STRPTR *, (___list), A0), \
	struct Library*, (___base), 21, Ttengine)

#endif /* !_INLINE_TTENGINE_H */
