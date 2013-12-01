/* Automatically generated header! Do not edit! */

#ifndef _INLINE_MCPGFX_LIB_SFD_H
#define _INLINE_MCPGFX_LIB_SFD_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef MCPGFX_LIB_SFD_BASE_NAME
#define MCPGFX_LIB_SFD_BASE_NAME MCPGfxBase
#endif /* !MCPGFX_LIB_SFD_BASE_NAME */

#define mcpPaintSysIGad(___sysImageObject, ___drawInfo, ___gadgetNumber, ___width, ___height) __mcpPaintSysIGad_WB(MCPGFX_LIB_SFD_BASE_NAME, ___sysImageObject, ___drawInfo, ___gadgetNumber, ___width, ___height)
#define __mcpPaintSysIGad_WB(___base, ___sysImageObject, ___drawInfo, ___gadgetNumber, ___width, ___height) \
	AROS_LC5NR(VOID, mcpPaintSysIGad, \
	AROS_LCA(APTR, (___sysImageObject), A0), \
	AROS_LCA(struct DrawInfo *, (___drawInfo), A1), \
	AROS_LCA(WORD, (___gadgetNumber), D0), \
	AROS_LCA(WORD, (___width), D1), \
	AROS_LCA(WORD, (___height), D2), \
	struct Library *, (___base), 5, Mcpgfx_lib_sfd)

#define mcpRectFillA(___rp, ___x1, ___y1, ___x2, ___y2, ___tagList) __mcpRectFillA_WB(MCPGFX_LIB_SFD_BASE_NAME, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList)
#define __mcpRectFillA_WB(___base, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList) \
	AROS_LC6NR(VOID, mcpRectFillA, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(WORD, (___x1), D0), \
	AROS_LCA(WORD, (___y1), D1), \
	AROS_LCA(WORD, (___x2), D2), \
	AROS_LCA(WORD, (___y2), D3), \
	AROS_LCA(CONST struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 6, Mcpgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define mcpRectFill(___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ...) __mcpRectFill_WB(MCPGFX_LIB_SFD_BASE_NAME, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ## __VA_ARGS__)
#define __mcpRectFill_WB(___base, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ...) \
	({IPTR _tags[] = { (IPTR) ___tagList, ## __VA_ARGS__ }; __mcpRectFillA_WB((___base), (___rp), (___x1), (___y1), (___x2), (___y2), (CONST struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define mcpDrawFrameA(___rp, ___x1, ___y1, ___x2, ___y2, ___tagList) __mcpDrawFrameA_WB(MCPGFX_LIB_SFD_BASE_NAME, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList)
#define __mcpDrawFrameA_WB(___base, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList) \
	AROS_LC6NR(VOID, mcpDrawFrameA, \
	AROS_LCA(struct RastPort *, (___rp), A0), \
	AROS_LCA(WORD, (___x1), D0), \
	AROS_LCA(WORD, (___y1), D1), \
	AROS_LCA(WORD, (___x2), D2), \
	AROS_LCA(WORD, (___y2), D3), \
	AROS_LCA(struct TagItem *, (___tagList), A1), \
	struct Library *, (___base), 7, Mcpgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define mcpDrawFrame(___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ...) __mcpDrawFrame_WB(MCPGFX_LIB_SFD_BASE_NAME, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ## __VA_ARGS__)
#define __mcpDrawFrame_WB(___base, ___rp, ___x1, ___y1, ___x2, ___y2, ___tagList, ...) \
	({IPTR _tags[] = { (IPTR) ___tagList, ## __VA_ARGS__ }; __mcpDrawFrameA_WB((___base), (___rp), (___x1), (___y1), (___x2), (___y2), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#define mcpGetExtDrawInfo(___screen, ___drawInfo) __mcpGetExtDrawInfo_WB(MCPGFX_LIB_SFD_BASE_NAME, ___screen, ___drawInfo)
#define __mcpGetExtDrawInfo_WB(___base, ___screen, ___drawInfo) \
	AROS_LC2(struct ExtDrawInfo	*, mcpGetExtDrawInfo, \
	AROS_LCA(struct Screen *, (___screen), A0), \
	AROS_LCA(struct DrawInfo *, (___drawInfo), A1), \
	struct Library *, (___base), 8, Mcpgfx_lib_sfd)

#define mcpGetFrameSize(___drawInfo, ___frameType) __mcpGetFrameSize_WB(MCPGFX_LIB_SFD_BASE_NAME, ___drawInfo, ___frameType)
#define __mcpGetFrameSize_WB(___base, ___drawInfo, ___frameType) \
	AROS_LC2(struct FrameSize	*, mcpGetFrameSize, \
	AROS_LCA(struct DrawInfo *, (___drawInfo), A0), \
	AROS_LCA(WORD, (___frameType), D0), \
	struct Library *, (___base), 9, Mcpgfx_lib_sfd)

#define mcpSetGFXAttrsA(___tagList) __mcpSetGFXAttrsA_WB(MCPGFX_LIB_SFD_BASE_NAME, ___tagList)
#define __mcpSetGFXAttrsA_WB(___base, ___tagList) \
	AROS_LC1NR(VOID, mcpSetGFXAttrsA, \
	AROS_LCA(struct TagItem *, (___tagList), A0), \
	struct Library *, (___base), 10, Mcpgfx_lib_sfd)

#ifndef NO_INLINE_STDARG
#define mcpSetGFXAttrs(___tagList, ...) __mcpSetGFXAttrs_WB(MCPGFX_LIB_SFD_BASE_NAME, ___tagList, ## __VA_ARGS__)
#define __mcpSetGFXAttrs_WB(___base, ___tagList, ...) \
	({IPTR _tags[] = { (IPTR) ___tagList, ## __VA_ARGS__ }; __mcpSetGFXAttrsA_WB((___base), (struct TagItem *) _tags); })
#endif /* !NO_INLINE_STDARG */

#endif /* !_INLINE_MCPGFX_LIB_SFD_H */
