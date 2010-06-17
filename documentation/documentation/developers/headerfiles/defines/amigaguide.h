#ifndef DEFINES_AMIGAGUIDE_H
#define DEFINES_AMIGAGUIDE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/amigaguide/amigaguide.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for amigaguide
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __LockAmigaGuideBase_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1(LONG, LockAmigaGuideBase, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
        struct Library *, (__AmigaGuideBase), 6, AmigaGuide)

#define LockAmigaGuideBase(arg1) \
    __LockAmigaGuideBase_WB(AmigaGuideBase, (arg1))

#define __UnlockAmigaGuideBase_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1NR(void, UnlockAmigaGuideBase, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__AmigaGuideBase), 7, AmigaGuide)

#define UnlockAmigaGuideBase(arg1) \
    __UnlockAmigaGuideBase_WB(AmigaGuideBase, (arg1))

#define __OpenAmigaGuideA_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(AMIGAGUIDECONTEXT, OpenAmigaGuideA, \
                  AROS_LCA(struct NewAmigaGuide *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__AmigaGuideBase), 9, AmigaGuide)

#define OpenAmigaGuideA(arg1, arg2) \
    __OpenAmigaGuideA_WB(AmigaGuideBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define OpenAmigaGuide(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenAmigaGuideA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __OpenAmigaGuideAsyncA_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(AMIGAGUIDECONTEXT, OpenAmigaGuideAsyncA, \
                  AROS_LCA(struct NewAmigaGuide *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),D0), \
        struct Library *, (__AmigaGuideBase), 10, AmigaGuide)

#define OpenAmigaGuideAsyncA(arg1, arg2) \
    __OpenAmigaGuideAsyncA_WB(AmigaGuideBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define OpenAmigaGuideAsync(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    OpenAmigaGuideAsyncA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __CloseAmigaGuide_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1NR(void, CloseAmigaGuide, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
        struct Library *, (__AmigaGuideBase), 11, AmigaGuide)

#define CloseAmigaGuide(arg1) \
    __CloseAmigaGuide_WB(AmigaGuideBase, (arg1))

#define __AmigaGuideSignal_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1(ULONG, AmigaGuideSignal, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
        struct Library *, (__AmigaGuideBase), 12, AmigaGuide)

#define AmigaGuideSignal(arg1) \
    __AmigaGuideSignal_WB(AmigaGuideBase, (arg1))

#define __GetAmigaGuideMsg_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1(struct AmigaGuideMsg *, GetAmigaGuideMsg, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
        struct Library *, (__AmigaGuideBase), 13, AmigaGuide)

#define GetAmigaGuideMsg(arg1) \
    __GetAmigaGuideMsg_WB(AmigaGuideBase, (arg1))

#define __ReplyAmigaGuideMsg_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1NR(void, ReplyAmigaGuideMsg, \
                  AROS_LCA(struct AmigaGuideMsg *,(__arg1),A0), \
        struct Library *, (__AmigaGuideBase), 14, AmigaGuide)

#define ReplyAmigaGuideMsg(arg1) \
    __ReplyAmigaGuideMsg_WB(AmigaGuideBase, (arg1))

#define __SetAmigaGuideContextA_WB(__AmigaGuideBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, SetAmigaGuideContextA, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(struct TagItem *,(__arg3),D1), \
        struct Library *, (__AmigaGuideBase), 15, AmigaGuide)

#define SetAmigaGuideContextA(arg1, arg2, arg3) \
    __SetAmigaGuideContextA_WB(AmigaGuideBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define SetAmigaGuideContext(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetAmigaGuideContextA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SendAmigaGuideContextA_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(BOOL, SendAmigaGuideContextA, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),D0), \
        struct Library *, (__AmigaGuideBase), 16, AmigaGuide)

#define SendAmigaGuideContextA(arg1, arg2) \
    __SendAmigaGuideContextA_WB(AmigaGuideBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define SendAmigaGuideContext(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SendAmigaGuideContextA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SendAmigaGuideCmdA_WB(__AmigaGuideBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, SendAmigaGuideCmdA, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),D0), \
                  AROS_LCA(struct TagItem *,(__arg3),D1), \
        struct Library *, (__AmigaGuideBase), 17, AmigaGuide)

#define SendAmigaGuideCmdA(arg1, arg2, arg3) \
    __SendAmigaGuideCmdA_WB(AmigaGuideBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define SendAmigaGuideCmd(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SendAmigaGuideCmdA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SetAmigaGuideAttrsA_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(LONG, SetAmigaGuideAttrsA, \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__AmigaGuideBase), 18, AmigaGuide)

#define SetAmigaGuideAttrsA(arg1, arg2) \
    __SetAmigaGuideAttrsA_WB(AmigaGuideBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define SetAmigaGuideAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetAmigaGuideAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetAmigaGuideAttr_WB(__AmigaGuideBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, GetAmigaGuideAttr, \
                  AROS_LCA(Tag,(__arg1),D0), \
                  AROS_LCA(AMIGAGUIDECONTEXT,(__arg2),A0), \
                  AROS_LCA(ULONG *,(__arg3),A1), \
        struct Library *, (__AmigaGuideBase), 19, AmigaGuide)

#define GetAmigaGuideAttr(arg1, arg2, arg3) \
    __GetAmigaGuideAttr_WB(AmigaGuideBase, (arg1), (arg2), (arg3))

#define __LoadXRef_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(LONG, LoadXRef, \
                  AROS_LCA(BPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
        struct Library *, (__AmigaGuideBase), 21, AmigaGuide)

#define LoadXRef(arg1, arg2) \
    __LoadXRef_WB(AmigaGuideBase, (arg1), (arg2))

#define __ExpungeXRef_WB(__AmigaGuideBase) \
        AROS_LC0NR(void, ExpungeXRef, \
        struct Library *, (__AmigaGuideBase), 22, AmigaGuide)

#define ExpungeXRef() \
    __ExpungeXRef_WB(AmigaGuideBase)

#define __AddAmigaGuideHostA_WB(__AmigaGuideBase, __arg1, __arg2, __arg3) \
        AROS_LC3(AMIGAGUIDEHOST, AddAmigaGuideHostA, \
                  AROS_LCA(struct Hook *,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),D0), \
                  AROS_LCA(struct TagItem *,(__arg3),A1), \
        struct Library *, (__AmigaGuideBase), 23, AmigaGuide)

#define AddAmigaGuideHostA(arg1, arg2, arg3) \
    __AddAmigaGuideHostA_WB(AmigaGuideBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define AddAmigaGuideHost(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    AddAmigaGuideHostA((arg1), (arg2), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __RemoveAmigaGuideHostA_WB(__AmigaGuideBase, __arg1, __arg2) \
        AROS_LC2(LONG, RemoveAmigaGuideHostA, \
                  AROS_LCA(AMIGAGUIDEHOST,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__AmigaGuideBase), 24, AmigaGuide)

#define RemoveAmigaGuideHostA(arg1, arg2) \
    __RemoveAmigaGuideHostA_WB(AmigaGuideBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(AMIGAGUIDE_NO_INLINE_STDARG)
#define RemoveAmigaGuideHost(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    RemoveAmigaGuideHostA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __GetAmigaGuideString_WB(__AmigaGuideBase, __arg1) \
        AROS_LC1(STRPTR, GetAmigaGuideString, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__AmigaGuideBase), 35, AmigaGuide)

#define GetAmigaGuideString(arg1) \
    __GetAmigaGuideString_WB(AmigaGuideBase, (arg1))

__END_DECLS

#endif /* DEFINES_AMIGAGUIDE_H*/
