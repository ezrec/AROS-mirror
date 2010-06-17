#ifndef DEFINES_POPUPMENU_H
#define DEFINES_POPUPMENU_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/popupmenu/popupmenu.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for popupmenu
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __PM_MakeMenuA_WB(__PopupMenuBase, __arg1) \
        AROS_LC1(struct PopupMenu *, PM_MakeMenuA, \
                  AROS_LCA(struct TagItem *,(__arg1),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 5, Popupmenu)

#define PM_MakeMenuA(arg1) \
    __PM_MakeMenuA_WB(PopupMenuBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_MakeMenu(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_MakeMenuA((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_MakeItemA_WB(__PopupMenuBase, __arg1) \
        AROS_LC1(struct PopupMenu *, PM_MakeItemA, \
                  AROS_LCA(struct TagItem *,(__arg1),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 6, Popupmenu)

#define PM_MakeItemA(arg1) \
    __PM_MakeItemA_WB(PopupMenuBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_MakeItem(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_MakeItemA((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_FreePopupMenu_WB(__PopupMenuBase, __arg1) \
        AROS_LC1NR(void, PM_FreePopupMenu, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 7, Popupmenu)

#define PM_FreePopupMenu(arg1) \
    __PM_FreePopupMenu_WB(PopupMenuBase, (arg1))

#define __PM_OpenPopupMenuA_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(ULONG, PM_OpenPopupMenuA, \
                  AROS_LCA(struct Window *,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A2), \
        struct PopupMenuBase *, (__PopupMenuBase), 8, Popupmenu)

#define PM_OpenPopupMenuA(arg1, arg2) \
    __PM_OpenPopupMenuA_WB(PopupMenuBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_OpenPopupMenu(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_OpenPopupMenuA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_MakeIDListA_WB(__PopupMenuBase, __arg1) \
        AROS_LC1(struct PM_IDLst *, PM_MakeIDListA, \
                  AROS_LCA(struct TagItem *,(__arg1),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 9, Popupmenu)

#define PM_MakeIDListA(arg1) \
    __PM_MakeIDListA_WB(PopupMenuBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_MakeIDList(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_MakeIDListA((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_ItemChecked_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(BOOL, PM_ItemChecked, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct PopupMenuBase *, (__PopupMenuBase), 10, Popupmenu)

#define PM_ItemChecked(arg1, arg2) \
    __PM_ItemChecked_WB(PopupMenuBase, (arg1), (arg2))

#define __PM_GetItemAttrsA_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(LONG, PM_GetItemAttrsA, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A2), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 11, Popupmenu)

#define PM_GetItemAttrsA(arg1, arg2) \
    __PM_GetItemAttrsA_WB(PopupMenuBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_GetItemAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_GetItemAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_SetItemAttrsA_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(LONG, PM_SetItemAttrsA, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A2), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 12, Popupmenu)

#define PM_SetItemAttrsA(arg1, arg2) \
    __PM_SetItemAttrsA_WB(PopupMenuBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_SetItemAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_SetItemAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_FindItem_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(struct PopupMenu *, PM_FindItem, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D1), \
        struct PopupMenuBase *, (__PopupMenuBase), 13, Popupmenu)

#define PM_FindItem(arg1, arg2) \
    __PM_FindItem_WB(PopupMenuBase, (arg1), (arg2))

#define __PM_AlterState_WB(__PopupMenuBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, PM_AlterState, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A1), \
                  AROS_LCA(struct PM_IDLst *,(__arg2),A2), \
                  AROS_LCA(UWORD,(__arg3),D1), \
        struct PopupMenuBase *, (__PopupMenuBase), 14, Popupmenu)

#define PM_AlterState(arg1, arg2, arg3) \
    __PM_AlterState_WB(PopupMenuBase, (arg1), (arg2), (arg3))

#define __PM_OBSOLETEFilterIMsgA_WB(__PopupMenuBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(APTR, PM_OBSOLETEFilterIMsgA, \
                  AROS_LCA(struct Window *,(__arg1),A1), \
                  AROS_LCA(struct PopupMenu *,(__arg2),A2), \
                  AROS_LCA(struct IntuiMessage *,(__arg3),A3), \
                  AROS_LCA(struct TagItem *,(__arg4),A5), \
        struct PopupMenuBase *, (__PopupMenuBase), 15, Popupmenu)

#define PM_OBSOLETEFilterIMsgA(arg1, arg2, arg3, arg4) \
    __PM_OBSOLETEFilterIMsgA_WB(PopupMenuBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_OBSOLETEFilterIMsg(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_OBSOLETEFilterIMsgA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_ExLstA_WB(__PopupMenuBase, __arg1) \
        AROS_LC1(struct PM_IDLst *, PM_ExLstA, \
                  AROS_LCA(ULONG *,(__arg1),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 16, Popupmenu)

#define PM_ExLstA(arg1) \
    __PM_ExLstA_WB(PopupMenuBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_ExLst(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_ExLstA((ULONG *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_FilterIMsgA_WB(__PopupMenuBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(APTR, PM_FilterIMsgA, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct PopupMenu *,(__arg2),A1), \
                  AROS_LCA(struct IntuiMessage *,(__arg3),A2), \
                  AROS_LCA(struct TagItem *,(__arg4),A3), \
        struct PopupMenuBase *, (__PopupMenuBase), 17, Popupmenu)

#define PM_FilterIMsgA(arg1, arg2, arg3, arg4) \
    __PM_FilterIMsgA_WB(PopupMenuBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_FilterIMsg(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_FilterIMsgA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_InsertMenuItemA_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(LONG, PM_InsertMenuItemA, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 18, Popupmenu)

#define PM_InsertMenuItemA(arg1, arg2) \
    __PM_InsertMenuItemA_WB(PopupMenuBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_InsertMenuItem(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_InsertMenuItemA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_RemoveMenuItem_WB(__PopupMenuBase, __arg1, __arg2) \
        AROS_LC2(struct PopupMenu *, PM_RemoveMenuItem, \
                  AROS_LCA(struct PopupMenu *,(__arg1),A0), \
                  AROS_LCA(struct PopupMenu *,(__arg2),A1), \
        struct PopupMenuBase *, (__PopupMenuBase), 19, Popupmenu)

#define PM_RemoveMenuItem(arg1, arg2) \
    __PM_RemoveMenuItem_WB(PopupMenuBase, (arg1), (arg2))

#define __PM_AbortHook_WB(__PopupMenuBase, __arg1) \
        AROS_LC1(BOOL, PM_AbortHook, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct PopupMenuBase *, (__PopupMenuBase), 20, Popupmenu)

#define PM_AbortHook(arg1) \
    __PM_AbortHook_WB(PopupMenuBase, (arg1))

#define __PM_GetVersion_WB(__PopupMenuBase) \
        AROS_LC0(STRPTR, PM_GetVersion, \
        struct PopupMenuBase *, (__PopupMenuBase), 21, Popupmenu)

#define PM_GetVersion() \
    __PM_GetVersion_WB(PopupMenuBase)

#define __PM_ReloadPrefs_WB(__PopupMenuBase) \
        AROS_LC0NR(void, PM_ReloadPrefs, \
        struct PopupMenuBase *, (__PopupMenuBase), 22, Popupmenu)

#define PM_ReloadPrefs() \
    __PM_ReloadPrefs_WB(PopupMenuBase)

#define __PM_LayoutMenuA_WB(__PopupMenuBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, PM_LayoutMenuA, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(struct PopupMenu *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct PopupMenuBase *, (__PopupMenuBase), 23, Popupmenu)

#define PM_LayoutMenuA(arg1, arg2, arg3) \
    __PM_LayoutMenuA_WB(PopupMenuBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(POPUPMENU_NO_INLINE_STDARG)
#define PM_LayoutMenu(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PM_LayoutMenuA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PM_FreeIDList_WB(__PopupMenuBase, __arg1) \
        AROS_LC1NR(void, PM_FreeIDList, \
                  AROS_LCA(struct PM_IDLst *,(__arg1),A0), \
        struct PopupMenuBase *, (__PopupMenuBase), 25, Popupmenu)

#define PM_FreeIDList(arg1) \
    __PM_FreeIDList_WB(PopupMenuBase, (arg1))

__END_DECLS

#endif /* DEFINES_POPUPMENU_H*/
