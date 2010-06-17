#ifndef DEFINES_WORKBENCH_H
#define DEFINES_WORKBENCH_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/workbench/workbench.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for workbench
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __AddAppWindowA_WB(__WorkbenchBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(struct AppWindow *, AddAppWindowA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(struct Window *,(__arg3),A0), \
                  AROS_LCA(struct MsgPort *,(__arg4),A1), \
                  AROS_LCA(struct TagItem *,(__arg5),A2), \
        struct Library *, (__WorkbenchBase), 8, Workbench)

#define AddAppWindowA(arg1, arg2, arg3, arg4, arg5) \
    __AddAppWindowA_WB(WorkbenchBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define AddAppWindow(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddAppWindowA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RemoveAppWindow_WB(__WorkbenchBase, __arg1) \
        AROS_LC1(BOOL, RemoveAppWindow, \
                  AROS_LCA(struct AppWindow *,(__arg1),A0), \
        struct Library *, (__WorkbenchBase), 9, Workbench)

#define RemoveAppWindow(arg1) \
    __RemoveAppWindow_WB(WorkbenchBase, (arg1))

#define __AddAppIconA_WB(__WorkbenchBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7(struct AppIcon *, AddAppIconA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(char *,(__arg3),A0), \
                  AROS_LCA(struct MsgPort *,(__arg4),A1), \
                  AROS_LCA(BPTR,(__arg5),A2), \
                  AROS_LCA(struct DiskObject *,(__arg6),A3), \
                  AROS_LCA(struct TagItem *,(__arg7),A4), \
        struct Library *, (__WorkbenchBase), 10, Workbench)

#define AddAppIconA(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __AddAppIconA_WB(WorkbenchBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define AddAppIcon(arg1, arg2, arg3, arg4, arg5, arg6, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddAppIconA((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RemoveAppIcon_WB(__WorkbenchBase, __arg1) \
        AROS_LC1(BOOL, RemoveAppIcon, \
                  AROS_LCA(struct AppIcon *,(__arg1),A0), \
        struct Library *, (__WorkbenchBase), 11, Workbench)

#define RemoveAppIcon(arg1) \
    __RemoveAppIcon_WB(WorkbenchBase, (arg1))

#define __AddAppMenuItemA_WB(__WorkbenchBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(struct AppMenuItem *, AddAppMenuItemA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(APTR,(__arg3),A0), \
                  AROS_LCA(struct MsgPort *,(__arg4),A1), \
                  AROS_LCA(struct TagItem *,(__arg5),A3), \
        struct Library *, (__WorkbenchBase), 12, Workbench)

#define AddAppMenuItemA(arg1, arg2, arg3, arg4, arg5) \
    __AddAppMenuItemA_WB(WorkbenchBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define AddAppMenuItem(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddAppMenuItemA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RemoveAppMenuItem_WB(__WorkbenchBase, __arg1) \
        AROS_LC1(BOOL, RemoveAppMenuItem, \
                  AROS_LCA(struct AppMenuItem *,(__arg1),A0), \
        struct Library *, (__WorkbenchBase), 13, Workbench)

#define RemoveAppMenuItem(arg1) \
    __RemoveAppMenuItem_WB(WorkbenchBase, (arg1))

#define __WBInfo_WB(__WorkbenchBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, WBInfo, \
                  AROS_LCA(BPTR,(__arg1),A0), \
                  AROS_LCA(CONST_STRPTR,(__arg2),A1), \
                  AROS_LCA(struct Screen *,(__arg3),A2), \
        struct Library *, (__WorkbenchBase), 15, Workbench)

#define WBInfo(arg1, arg2, arg3) \
    __WBInfo_WB(WorkbenchBase, (arg1), (arg2), (arg3))

#define __OpenWorkbenchObjectA_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(BOOL, OpenWorkbenchObjectA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 16, Workbench)

#define OpenWorkbenchObjectA(arg1, arg2) \
    __OpenWorkbenchObjectA_WB(WorkbenchBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define OpenWorkbenchObject(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenWorkbenchObjectA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __CloseWorkbenchObjectA_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(BOOL, CloseWorkbenchObjectA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 17, Workbench)

#define CloseWorkbenchObjectA(arg1, arg2) \
    __CloseWorkbenchObjectA_WB(WorkbenchBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define CloseWorkbenchObject(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    CloseWorkbenchObjectA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __WorkbenchControlA_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(BOOL, WorkbenchControlA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 18, Workbench)

#define WorkbenchControlA(arg1, arg2) \
    __WorkbenchControlA_WB(WorkbenchBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define WorkbenchControl(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    WorkbenchControlA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __AddAppWindowDropZoneA_WB(__WorkbenchBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(struct AppWindowDropZone *, AddAppWindowDropZoneA, \
                  AROS_LCA(struct AppWindow *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
                  AROS_LCA(struct TagItem *,(__arg4),A1), \
        struct Library *, (__WorkbenchBase), 19, Workbench)

#define AddAppWindowDropZoneA(arg1, arg2, arg3, arg4) \
    __AddAppWindowDropZoneA_WB(WorkbenchBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define AddAppWindowDropZone(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddAppWindowDropZoneA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RemoveAppWindowDropZone_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(BOOL, RemoveAppWindowDropZone, \
                  AROS_LCA(struct AppWindow *,(__arg1),A0), \
                  AROS_LCA(struct AppWindowDropZone *,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 20, Workbench)

#define RemoveAppWindowDropZone(arg1, arg2) \
    __RemoveAppWindowDropZone_WB(WorkbenchBase, (arg1), (arg2))

#define __ChangeWorkbenchSelectionA_WB(__WorkbenchBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, ChangeWorkbenchSelectionA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(struct Hook *,(__arg2),A1), \
                  AROS_LCA(struct TagItem *,(__arg3),A2), \
        struct Library *, (__WorkbenchBase), 21, Workbench)

#define ChangeWorkbenchSelectionA(arg1, arg2, arg3) \
    __ChangeWorkbenchSelectionA_WB(WorkbenchBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define ChangeWorkbenchSelection(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    ChangeWorkbenchSelectionA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __MakeWorkbenchObjectVisibleA_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(BOOL, MakeWorkbenchObjectVisibleA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 22, Workbench)

#define MakeWorkbenchObjectVisibleA(arg1, arg2) \
    __MakeWorkbenchObjectVisibleA_WB(WorkbenchBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define MakeWorkbenchObjectVisible(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    MakeWorkbenchObjectVisibleA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RegisterWorkbench_WB(__WorkbenchBase, __arg1) \
        AROS_LC1(BOOL, RegisterWorkbench, \
                  AROS_LCA(struct MsgPort *,(__arg1),A0), \
        struct Library *, (__WorkbenchBase), 23, Workbench)

#define RegisterWorkbench(arg1) \
    __RegisterWorkbench_WB(WorkbenchBase, (arg1))

#define __UnregisterWorkbench_WB(__WorkbenchBase, __arg1) \
        AROS_LC1(BOOL, UnregisterWorkbench, \
                  AROS_LCA(struct MsgPort *,(__arg1),A0), \
        struct Library *, (__WorkbenchBase), 24, Workbench)

#define UnregisterWorkbench(arg1) \
    __UnregisterWorkbench_WB(WorkbenchBase, (arg1))

#define __UpdateWorkbenchObjectA_WB(__WorkbenchBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, UpdateWorkbenchObjectA, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D1), \
                  AROS_LCA(struct TagItem *,(__arg3),A1), \
        struct Library *, (__WorkbenchBase), 25, Workbench)

#define UpdateWorkbenchObjectA(arg1, arg2, arg3) \
    __UpdateWorkbenchObjectA_WB(WorkbenchBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(WORKBENCH_NO_INLINE_STDARG)
#define UpdateWorkbenchObject(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    UpdateWorkbenchObjectA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SendAppWindowMessage_WB(__WorkbenchBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8(BOOL, SendAppWindowMessage, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(char **,(__arg3),A1), \
                  AROS_LCA(UWORD,(__arg4),D1), \
                  AROS_LCA(WORD,(__arg5),D2), \
                  AROS_LCA(WORD,(__arg6),D3), \
                  AROS_LCA(ULONG,(__arg7),D4), \
                  AROS_LCA(ULONG,(__arg8),D5), \
        struct Library *, (__WorkbenchBase), 26, Workbench)

#define SendAppWindowMessage(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __SendAppWindowMessage_WB(WorkbenchBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __GetNextAppIcon_WB(__WorkbenchBase, __arg1, __arg2) \
        AROS_LC2(struct DiskObject *, GetNextAppIcon, \
                  AROS_LCA(struct DiskObject *,(__arg1),A0), \
                  AROS_LCA(char*,(__arg2),A1), \
        struct Library *, (__WorkbenchBase), 27, Workbench)

#define GetNextAppIcon(arg1, arg2) \
    __GetNextAppIcon_WB(WorkbenchBase, (arg1), (arg2))

__END_DECLS

#endif /* DEFINES_WORKBENCH_H*/
