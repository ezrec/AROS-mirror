#ifndef DEFINES_ICON_H
#define DEFINES_ICON_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/icon/icon.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for icon
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __FreeFreeList_WB(__IconBase, __arg1) \
        AROS_LC1NR(void, FreeFreeList, \
                  AROS_LCA(struct FreeList *,(__arg1),A0), \
        struct Library *, (__IconBase), 9, Icon)

#define FreeFreeList(arg1) \
    __FreeFreeList_WB(IconBase, (arg1))

#define __AddFreeList_WB(__IconBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, AddFreeList, \
                  AROS_LCA(struct FreeList *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(unsigned long,(__arg3),A2), \
        struct Library *, (__IconBase), 12, Icon)

#define AddFreeList(arg1, arg2, arg3) \
    __AddFreeList_WB(IconBase, (arg1), (arg2), (arg3))

#define __GetDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1(struct DiskObject *, GetDiskObject, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
        struct Library *, (__IconBase), 13, Icon)

#define GetDiskObject(arg1) \
    __GetDiskObject_WB(IconBase, (arg1))

#define __PutDiskObject_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(BOOL, PutDiskObject, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
                  AROS_LCA(struct DiskObject *,(__arg2),A1), \
        struct Library *, (__IconBase), 14, Icon)

#define PutDiskObject(arg1, arg2) \
    __PutDiskObject_WB(IconBase, (arg1), (arg2))

#define __FreeDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1NR(void, FreeDiskObject, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
        struct Library *, (__IconBase), 15, Icon)

#define FreeDiskObject(arg1) \
    __FreeDiskObject_WB(IconBase, (arg1))

#define __FindToolType_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(UBYTE *, FindToolType, \
                  AROS_LCA(const STRPTR *,(__arg1),A0), \
                  AROS_LCA(const STRPTR,(__arg2),A1), \
        struct Library *, (__IconBase), 16, Icon)

#define FindToolType(arg1, arg2) \
    __FindToolType_WB(IconBase, (arg1), (arg2))

#define __MatchToolValue_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(BOOL, MatchToolValue, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
        struct Library *, (__IconBase), 17, Icon)

#define MatchToolValue(arg1, arg2) \
    __MatchToolValue_WB(IconBase, (arg1), (arg2))

#define __BumpRevision_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(UBYTE *, BumpRevision, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
        struct Library *, (__IconBase), 18, Icon)

#define BumpRevision(arg1, arg2) \
    __BumpRevision_WB(IconBase, (arg1), (arg2))

#define __GetDefDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1(struct DiskObject *, GetDefDiskObject, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__IconBase), 20, Icon)

#define GetDefDiskObject(arg1) \
    __GetDefDiskObject_WB(IconBase, (arg1))

#define __PutDefDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1(BOOL, PutDefDiskObject, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
        struct Library *, (__IconBase), 21, Icon)

#define PutDefDiskObject(arg1) \
    __PutDefDiskObject_WB(IconBase, (arg1))

#define __GetDiskObjectNew_WB(__IconBase, __arg1) \
        AROS_LC1(struct DiskObject *, GetDiskObjectNew, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
        struct Library *, (__IconBase), 22, Icon)

#define GetDiskObjectNew(arg1) \
    __GetDiskObjectNew_WB(IconBase, (arg1))

#define __DeleteDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1(BOOL, DeleteDiskObject, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct Library *, (__IconBase), 23, Icon)

#define DeleteDiskObject(arg1) \
    __DeleteDiskObject_WB(IconBase, (arg1))

#define __DupDiskObjectA_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(struct DiskObject *, DupDiskObjectA, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__IconBase), 25, Icon)

#define DupDiskObjectA(arg1, arg2) \
    __DupDiskObjectA_WB(IconBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define DupDiskObject(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    DupDiskObjectA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __IconControlA_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(ULONG, IconControlA, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__IconBase), 26, Icon)

#define IconControlA(arg1, arg2) \
    __IconControlA_WB(IconBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define IconControl(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    IconControlA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __DrawIconStateA_WB(__IconBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, DrawIconStateA, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct DiskObject *,(__arg2),A1), \
                  AROS_LCA(STRPTR,(__arg3),A2), \
                  AROS_LCA(LONG,(__arg4),D0), \
                  AROS_LCA(LONG,(__arg5),D1), \
                  AROS_LCA(ULONG,(__arg6),D2), \
                  AROS_LCA(struct TagItem *,(__arg7),A3), \
        struct Library *, (__IconBase), 27, Icon)

#define DrawIconStateA(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __DrawIconStateA_WB(IconBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define DrawIconState(arg1, arg2, arg3, arg4, arg5, arg6, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    DrawIconStateA((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetIconRectangleA_WB(__IconBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(BOOL, GetIconRectangleA, \
                  AROS_LCA(struct RastPort *,(__arg1),A0), \
                  AROS_LCA(struct DiskObject *,(__arg2),A1), \
                  AROS_LCA(STRPTR,(__arg3),A2), \
                  AROS_LCA(struct Rectangle *,(__arg4),A3), \
                  AROS_LCA(struct TagItem *,(__arg5),A4), \
        struct Library *, (__IconBase), 28, Icon)

#define GetIconRectangleA(arg1, arg2, arg3, arg4, arg5) \
    __GetIconRectangleA_WB(IconBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define GetIconRectangle(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetIconRectangleA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __NewDiskObject_WB(__IconBase, __arg1) \
        AROS_LC1(struct DiskObject *, NewDiskObject, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__IconBase), 29, Icon)

#define NewDiskObject(arg1) \
    __NewDiskObject_WB(IconBase, (arg1))

#define __GetIconTagList_WB(__IconBase, __arg1, __arg2) \
        AROS_LC2(struct DiskObject *, GetIconTagList, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
                  AROS_LCA(const struct TagItem *,(__arg2),A1), \
        struct Library *, (__IconBase), 30, Icon)

#define GetIconTagList(arg1, arg2) \
    __GetIconTagList_WB(IconBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define GetIconTags(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetIconTagList((arg1), (const struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __PutIconTagList_WB(__IconBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, PutIconTagList, \
                  AROS_LCA(CONST_STRPTR,(__arg1),A0), \
                  AROS_LCA(struct DiskObject *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__IconBase), 31, Icon)

#define PutIconTagList(arg1, arg2, arg3) \
    __PutIconTagList_WB(IconBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define PutIconTags(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    PutIconTagList((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __LayoutIconA_WB(__IconBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, LayoutIconA, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
                  AROS_LCA(struct Screen *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__IconBase), 32, Icon)

#define LayoutIconA(arg1, arg2, arg3) \
    __LayoutIconA_WB(IconBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(ICON_NO_INLINE_STDARG)
#define LayoutIcon(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    LayoutIconA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __ChangeToSelectedIconColor_WB(__IconBase, __arg1) \
        AROS_LC1NR(void, ChangeToSelectedIconColor, \
                  AROS_LCA(struct ColorRegister *,(__arg1),A0), \
        struct Library *, (__IconBase), 33, Icon)

#define ChangeToSelectedIconColor(arg1) \
    __ChangeToSelectedIconColor_WB(IconBase, (arg1))

__END_DECLS

#endif /* DEFINES_ICON_H*/
