#ifndef DEFINES_REQTOOLS_H
#define DEFINES_REQTOOLS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/reqtools/reqtools.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for reqtools
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __rtAllocRequestA_WB(__ReqToolsBase, __arg1, __arg2) \
        AROS_LC2(APTR, rtAllocRequestA, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct TagItem *,(__arg2),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 5, ReqTools)

#define rtAllocRequestA(arg1, arg2) \
    __rtAllocRequestA_WB(ReqToolsBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtAllocRequest(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtAllocRequestA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtFreeRequest_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtFreeRequest, \
                  AROS_LCA(APTR,(__arg1),A1), \
        struct ReqToolsBase *, (__ReqToolsBase), 6, ReqTools)

#define rtFreeRequest(arg1) \
    __rtFreeRequest_WB(ReqToolsBase, (arg1))

#define __rtFreeReqBuffer_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtFreeReqBuffer, \
                  AROS_LCA(APTR,(__arg1),A1), \
        struct ReqToolsBase *, (__ReqToolsBase), 7, ReqTools)

#define rtFreeReqBuffer(arg1) \
    __rtFreeReqBuffer_WB(ReqToolsBase, (arg1))

#define __rtChangeReqAttrA_WB(__ReqToolsBase, __arg1, __arg2) \
        AROS_LC2(LONG, rtChangeReqAttrA, \
                  AROS_LCA(APTR,(__arg1),A1), \
                  AROS_LCA(struct TagItem *,(__arg2),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 8, ReqTools)

#define rtChangeReqAttrA(arg1, arg2) \
    __rtChangeReqAttrA_WB(ReqToolsBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtChangeReqAttr(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtChangeReqAttrA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtFileRequestA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(APTR, rtFileRequestA, \
                  AROS_LCA(struct rtFileRequester *,(__arg1),A1), \
                  AROS_LCA(char *,(__arg2),A2), \
                  AROS_LCA(char *,(__arg3),A3), \
                  AROS_LCA(struct TagItem *,(__arg4),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 9, ReqTools)

#define rtFileRequestA(arg1, arg2, arg3, arg4) \
    __rtFileRequestA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtFileRequest(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtFileRequestA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtFreeFileList_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtFreeFileList, \
                  AROS_LCA(struct rtFileList *,(__arg1),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 10, ReqTools)

#define rtFreeFileList(arg1) \
    __rtFreeFileList_WB(ReqToolsBase, (arg1))

#define __rtEZRequestA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(ULONG, rtEZRequestA, \
                  AROS_LCA(char *,(__arg1),A1), \
                  AROS_LCA(char *,(__arg2),A2), \
                  AROS_LCA(struct rtReqInfo *,(__arg3),A3), \
                  AROS_LCA(APTR,(__arg4),A4), \
                  AROS_LCA(struct TagItem *,(__arg5),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 11, ReqTools)

#define rtEZRequestA(arg1, arg2, arg3, arg4, arg5) \
    __rtEZRequestA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtEZRequest(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtEZRequestA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtGetStringA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(ULONG, rtGetStringA, \
                  AROS_LCA(UBYTE *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(char *,(__arg3),A2), \
                  AROS_LCA(struct rtReqInfo *,(__arg4),A3), \
                  AROS_LCA(struct TagItem *,(__arg5),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 12, ReqTools)

#define rtGetStringA(arg1, arg2, arg3, arg4, arg5) \
    __rtGetStringA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtGetString(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtGetStringA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtGetLongA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(ULONG, rtGetLongA, \
                  AROS_LCA(ULONG *,(__arg1),A1), \
                  AROS_LCA(char *,(__arg2),A2), \
                  AROS_LCA(struct rtReqInfo *,(__arg3),A3), \
                  AROS_LCA(struct TagItem *,(__arg4),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 13, ReqTools)

#define rtGetLongA(arg1, arg2, arg3, arg4) \
    __rtGetLongA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtGetLong(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtGetLongA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtInternalGetPasswordA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(BOOL, rtInternalGetPasswordA, \
                  AROS_LCA(UBYTE *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D1), \
                  AROS_LCA(PWCALLBACKFUNPTR,(__arg3),D2), \
                  AROS_LCA(struct rtReqInfo *,(__arg4),A3), \
                  AROS_LCA(struct TagItem *,(__arg5),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 14, ReqTools)

#define rtInternalGetPasswordA(arg1, arg2, arg3, arg4, arg5) \
    __rtInternalGetPasswordA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtInternalGetPassword(arg1, arg2, arg3, arg4, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtInternalGetPasswordA((arg1), (arg2), (arg3), (arg4), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtInternalEnterPasswordA_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, rtInternalEnterPasswordA, \
                  AROS_LCA(UBYTE *,(__arg1),A1), \
                  AROS_LCA(PWCALLBACKFUNPTR,(__arg2),D2), \
                  AROS_LCA(struct rtReqInfo *,(__arg3),A3), \
                  AROS_LCA(struct TagItem *,(__arg4),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 15, ReqTools)

#define rtInternalEnterPasswordA(arg1, arg2, arg3, arg4) \
    __rtInternalEnterPasswordA_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtInternalEnterPassword(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtInternalEnterPasswordA((arg1), (arg2), (arg3), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtFontRequestA_WB(__ReqToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, rtFontRequestA, \
                  AROS_LCA(struct rtFontRequester *,(__arg1),A1), \
                  AROS_LCA(char *,(__arg2),A3), \
                  AROS_LCA(struct TagItem *,(__arg3),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 16, ReqTools)

#define rtFontRequestA(arg1, arg2, arg3) \
    __rtFontRequestA_WB(ReqToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtFontRequest(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtFontRequestA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtPaletteRequestA_WB(__ReqToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, rtPaletteRequestA, \
                  AROS_LCA(char *,(__arg1),A2), \
                  AROS_LCA(struct rtReqInfo *,(__arg2),A3), \
                  AROS_LCA(struct TagItem *,(__arg3),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 17, ReqTools)

#define rtPaletteRequestA(arg1, arg2, arg3) \
    __rtPaletteRequestA_WB(ReqToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtPaletteRequest(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtPaletteRequestA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtReqHandlerA_WB(__ReqToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, rtReqHandlerA, \
                  AROS_LCA(struct rtHandlerInfo *,(__arg1),A1), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(struct TagItem *,(__arg3),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 18, ReqTools)

#define rtReqHandlerA(arg1, arg2, arg3) \
    __rtReqHandlerA_WB(ReqToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtReqHandler(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtReqHandlerA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtSetWaitPointer_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtSetWaitPointer, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 19, ReqTools)

#define rtSetWaitPointer(arg1) \
    __rtSetWaitPointer_WB(ReqToolsBase, (arg1))

#define __rtGetVScreenSize_WB(__ReqToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, rtGetVScreenSize, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
                  AROS_LCA(ULONG *,(__arg2),A1), \
                  AROS_LCA(ULONG *,(__arg3),A2), \
        struct ReqToolsBase *, (__ReqToolsBase), 20, ReqTools)

#define rtGetVScreenSize(arg1, arg2, arg3) \
    __rtGetVScreenSize_WB(ReqToolsBase, (arg1), (arg2), (arg3))

#define __rtSetReqPosition_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, rtSetReqPosition, \
                  AROS_LCA(ULONG,(__arg1),D0), \
                  AROS_LCA(struct NewWindow *,(__arg2),A0), \
                  AROS_LCA(struct Screen *,(__arg3),A1), \
                  AROS_LCA(struct Window *,(__arg4),A2), \
        struct ReqToolsBase *, (__ReqToolsBase), 21, ReqTools)

#define rtSetReqPosition(arg1, arg2, arg3, arg4) \
    __rtSetReqPosition_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4))

#define __rtSpread_WB(__ReqToolsBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, rtSpread, \
                  AROS_LCA(ULONG *,(__arg1),A0), \
                  AROS_LCA(ULONG *,(__arg2),A1), \
                  AROS_LCA(ULONG,(__arg3),D0), \
                  AROS_LCA(ULONG,(__arg4),D1), \
                  AROS_LCA(ULONG,(__arg5),D2), \
                  AROS_LCA(ULONG,(__arg6),D3), \
        struct ReqToolsBase *, (__ReqToolsBase), 22, ReqTools)

#define rtSpread(arg1, arg2, arg3, arg4, arg5, arg6) \
    __rtSpread_WB(ReqToolsBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __rtScreenToFrontSafely_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtScreenToFrontSafely, \
                  AROS_LCA(struct Screen *,(__arg1),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 23, ReqTools)

#define rtScreenToFrontSafely(arg1) \
    __rtScreenToFrontSafely_WB(ReqToolsBase, (arg1))

#define __rtScreenModeRequestA_WB(__ReqToolsBase, __arg1, __arg2, __arg3) \
        AROS_LC3(ULONG, rtScreenModeRequestA, \
                  AROS_LCA(struct rtScreenModeRequester *,(__arg1),A1), \
                  AROS_LCA(char *,(__arg2),A3), \
                  AROS_LCA(struct TagItem *,(__arg3),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 24, ReqTools)

#define rtScreenModeRequestA(arg1, arg2, arg3) \
    __rtScreenModeRequestA_WB(ReqToolsBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(REQTOOLS_NO_INLINE_STDARG)
#define rtScreenModeRequest(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    rtScreenModeRequestA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __rtCloseWindowSafely_WB(__ReqToolsBase, __arg1) \
        AROS_LC1NR(void, rtCloseWindowSafely, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 25, ReqTools)

#define rtCloseWindowSafely(arg1) \
    __rtCloseWindowSafely_WB(ReqToolsBase, (arg1))

#define __rtLockWindow_WB(__ReqToolsBase, __arg1) \
        AROS_LC1(APTR, rtLockWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
        struct ReqToolsBase *, (__ReqToolsBase), 26, ReqTools)

#define rtLockWindow(arg1) \
    __rtLockWindow_WB(ReqToolsBase, (arg1))

#define __rtUnlockWindow_WB(__ReqToolsBase, __arg1, __arg2) \
        AROS_LC2NR(void, rtUnlockWindow, \
                  AROS_LCA(struct Window *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
        struct ReqToolsBase *, (__ReqToolsBase), 27, ReqTools)

#define rtUnlockWindow(arg1, arg2) \
    __rtUnlockWindow_WB(ReqToolsBase, (arg1), (arg2))

#define __rtLockPrefs_WB(__ReqToolsBase) \
        AROS_LC0(struct ReqToolsPrefs *, rtLockPrefs, \
        struct ReqToolsBase *, (__ReqToolsBase), 28, ReqTools)

#define rtLockPrefs() \
    __rtLockPrefs_WB(ReqToolsBase)

#define __rtUnlockPrefs_WB(__ReqToolsBase) \
        AROS_LC0NR(void, rtUnlockPrefs, \
        struct ReqToolsBase *, (__ReqToolsBase), 29, ReqTools)

#define rtUnlockPrefs() \
    __rtUnlockPrefs_WB(ReqToolsBase)

__END_DECLS

#endif /* DEFINES_REQTOOLS_H*/
