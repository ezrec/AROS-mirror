#ifndef DEFINES_GADTOOLS_H
#define DEFINES_GADTOOLS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/gadtools/gadtools.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for gadtools
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __CreateGadgetA_WB(__GadToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(struct Gadget *, CreateGadgetA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct Gadget *,(__arg2),A0), \
                  AROS_LCA(struct NewGadget *,(__arg3),A1), \
                  AROS_LCA(struct TagItem *,(__arg4),A2), \
        struct Library *, (__GadToolsBase), 5, GadTools)

#define CreateGadgetA(arg1, arg2, arg3, arg4) \
    __CreateGadgetA_WB(GadToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define CreateGadget(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    CreateGadgetA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __FreeGadgets_WB(__GadToolsBase, __arg1) \
        AROS_LC1NR(void, FreeGadgets, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 6, GadTools)

#define FreeGadgets(arg1) \
    __FreeGadgets_WB(GadToolsBase, (arg1))

#define __GT_SetGadgetAttrsA_WB(__GadToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, GT_SetGadgetAttrsA, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(struct TagItem *,(__arg4),A3), \
        struct Library *, (__GadToolsBase), 7, GadTools)

#define GT_SetGadgetAttrsA(arg1, arg2, arg3, arg4) \
    __GT_SetGadgetAttrsA_WB(GadToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define GT_SetGadgetAttrs(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GT_SetGadgetAttrsA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __CreateMenusA_WB(__GadToolsBase, __arg1, __arg2) \
        AROS_LC2(struct Menu *, CreateMenusA, \
                  AROS_LCA(struct NewMenu *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__GadToolsBase), 8, GadTools)

#define CreateMenusA(arg1, arg2) \
    __CreateMenusA_WB(GadToolsBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define CreateMenus(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    CreateMenusA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __FreeMenus_WB(__GadToolsBase, __arg1) \
        AROS_LC1NR(void, FreeMenus, \
                  AROS_LCA(struct Menu *,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 9, GadTools)

#define FreeMenus(arg1) \
    __FreeMenus_WB(GadToolsBase, (arg1))

#define __LayoutMenuItemsA_WB(__GadToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, LayoutMenuItemsA, \
                  AROS_LCA(struct MenuItem *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__GadToolsBase), 10, GadTools)

#define LayoutMenuItemsA(arg1, arg2, arg3) \
    __LayoutMenuItemsA_WB(GadToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define LayoutMenuItems(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    LayoutMenuItemsA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __LayoutMenusA_WB(__GadToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, LayoutMenusA, \
                  AROS_LCA(struct Menu *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__GadToolsBase), 11, GadTools)

#define LayoutMenusA(arg1, arg2, arg3) \
    __LayoutMenusA_WB(GadToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define LayoutMenus(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    LayoutMenusA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GT_GetIMsg_WB(__GadToolsBase, __arg1) \
        AROS_LC1(struct IntuiMessage *, GT_GetIMsg, \
                  AROS_LCA(struct MsgPort *,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 12, GadTools)

#define GT_GetIMsg(arg1) \
    __GT_GetIMsg_WB(GadToolsBase, (arg1))

#define __GT_ReplyIMsg_WB(__GadToolsBase, __arg1) \
        AROS_LC1NR(void, GT_ReplyIMsg, \
                  AROS_LCA(struct IntuiMessage *,(__arg1),A1), \
        struct Library *, (__GadToolsBase), 13, GadTools)

#define GT_ReplyIMsg(arg1) \
    __GT_ReplyIMsg_WB(GadToolsBase, (arg1))

#define __GT_RefreshWindow_WB(__GadToolsBase, __arg1, __arg2) \
        AROS_LC2NR(void, GT_RefreshWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct Requester *,(__arg2),A1), \
        struct Library *, (__GadToolsBase), 14, GadTools)

#define GT_RefreshWindow(arg1, arg2) \
    __GT_RefreshWindow_WB(GadToolsBase, (arg1), (arg2))

#define __GT_BeginRefresh_WB(__GadToolsBase, __arg1) \
        AROS_LC1NR(void, GT_BeginRefresh, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 15, GadTools)

#define GT_BeginRefresh(arg1) \
    __GT_BeginRefresh_WB(GadToolsBase, (arg1))

#define __GT_EndRefresh_WB(__GadToolsBase, __arg1, __arg2) \
        AROS_LC2NR(void, GT_EndRefresh, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(BOOL,(__arg2),D0), \
        struct Library *, (__GadToolsBase), 16, GadTools)

#define GT_EndRefresh(arg1, arg2) \
    __GT_EndRefresh_WB(GadToolsBase, (arg1), (arg2))

#define __GT_FilterIMsg_WB(__GadToolsBase, __arg1) \
        AROS_LC1(struct IntuiMessage *, GT_FilterIMsg, \
                  AROS_LCA(struct IntuiMessage *,(__arg1),A1), \
        struct Library *, (__GadToolsBase), 17, GadTools)

#define GT_FilterIMsg(arg1) \
    __GT_FilterIMsg_WB(GadToolsBase, (arg1))

#define __GT_PostFilterIMsg_WB(__GadToolsBase, __arg1) \
        AROS_LC1(struct IntuiMessage *, GT_PostFilterIMsg, \
                  AROS_LCA(struct IntuiMessage *,(__arg1),A1), \
        struct Library *, (__GadToolsBase), 18, GadTools)

#define GT_PostFilterIMsg(arg1) \
    __GT_PostFilterIMsg_WB(GadToolsBase, (arg1))

#define __CreateContext_WB(__GadToolsBase, __arg1) \
        AROS_LC1(struct Gadget *, CreateContext, \
                  AROS_LCA(struct Gadget **,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 19, GadTools)

#define CreateContext(arg1) \
    __CreateContext_WB(GadToolsBase, (arg1))

#define __DrawBevelBoxA_WB(__GadToolsBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, DrawBevelBoxA, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(WORD,(__arg2),D0), \
                  AROS_LCA(WORD,(__arg3),D1), \
                  AROS_LCA(WORD,(__arg4),D2), \
                  AROS_LCA(WORD,(__arg5),D3), \
                  AROS_LCA(struct TagItem *,(__arg6),A1), \
        struct Library *, (__GadToolsBase), 20, GadTools)

#define DrawBevelBoxA(arg1, arg2, arg3, arg4, arg5, arg6) \
    __DrawBevelBoxA_WB(GadToolsBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define DrawBevelBox(arg1, arg2, arg3, arg4, arg5, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    DrawBevelBoxA((arg1), (arg2), (arg3), (arg4), (arg5), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetVisualInfoA_WB(__GadToolsBase, __arg1, __arg2) \
        AROS_LC2(APTR, GetVisualInfoA, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__GadToolsBase), 21, GadTools)

#define GetVisualInfoA(arg1, arg2) \
    __GetVisualInfoA_WB(GadToolsBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define GetVisualInfo(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetVisualInfoA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __FreeVisualInfo_WB(__GadToolsBase, __arg1) \
        AROS_LC1NR(void, FreeVisualInfo, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__GadToolsBase), 22, GadTools)

#define FreeVisualInfo(arg1) \
    __FreeVisualInfo_WB(GadToolsBase, (arg1))

#define __GT_GetGadgetAttrsA_WB(__GadToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, GT_GetGadgetAttrsA, \
                  AROS_LCA(struct Gadget *,(__arg1),A0), \
                  AROS_LCA(struct Window *,(__arg2),A1), \
                  AROS_LCA(struct Requester *,(__arg3),A2), \
                  AROS_LCA(struct TagItem *,(__arg4),A3), \
        struct Library *, (__GadToolsBase), 29, GadTools)

#define GT_GetGadgetAttrsA(arg1, arg2, arg3, arg4) \
    __GT_GetGadgetAttrsA_WB(GadToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(GADTOOLS_NO_INLINE_STDARG)
#define GT_GetGadgetAttrs(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GT_GetGadgetAttrsA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

__END_DECLS

#endif /* DEFINES_GADTOOLS_H*/
