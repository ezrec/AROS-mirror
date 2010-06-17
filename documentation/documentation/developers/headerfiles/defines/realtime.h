#ifndef DEFINES_REALTIME_H
#define DEFINES_REALTIME_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/realtime/realtime.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for realtime
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __LockRealTime_WB(__RealTimeBase, __arg1) \
        AROS_LC1(APTR, LockRealTime, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Library *, (__RealTimeBase), 5, RealTime)

#define LockRealTime(arg1) \
    __LockRealTime_WB(RealTimeBase, (arg1))

#define __UnlockRealTime_WB(__RealTimeBase, __arg1) \
        AROS_LC1NR(void, UnlockRealTime, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__RealTimeBase), 6, RealTime)

#define UnlockRealTime(arg1) \
    __UnlockRealTime_WB(RealTimeBase, (arg1))

#define __CreatePlayerA_WB(__RealTimeBase, __arg1) \
        AROS_LC1(struct Player *, CreatePlayerA, \
                  AROS_LCA(struct TagItem *,(__arg1),A0), \
        struct Library *, (__RealTimeBase), 7, RealTime)

#define CreatePlayerA(arg1) \
    __CreatePlayerA_WB(RealTimeBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(REALTIME_NO_INLINE_STDARG)
#define CreatePlayer(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    CreatePlayerA((struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __DeletePlayer_WB(__RealTimeBase, __arg1) \
        AROS_LC1NR(void, DeletePlayer, \
                  AROS_LCA(struct Player *,(__arg1),A0), \
        struct Library *, (__RealTimeBase), 8, RealTime)

#define DeletePlayer(arg1) \
    __DeletePlayer_WB(RealTimeBase, (arg1))

#define __SetPlayerAttrsA_WB(__RealTimeBase, __arg1, __arg2) \
        AROS_LC2(BOOL, SetPlayerAttrsA, \
                  AROS_LCA(struct Player *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__RealTimeBase), 9, RealTime)

#define SetPlayerAttrsA(arg1, arg2) \
    __SetPlayerAttrsA_WB(RealTimeBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(REALTIME_NO_INLINE_STDARG)
#define SetPlayerAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    SetPlayerAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __SetConductorState_WB(__RealTimeBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, SetConductorState, \
                  AROS_LCA(struct Player *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__RealTimeBase), 10, RealTime)

#define SetConductorState(arg1, arg2, arg3) \
    __SetConductorState_WB(RealTimeBase, (arg1), (arg2), (arg3))

#define __ExternalSync_WB(__RealTimeBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, ExternalSync, \
                  AROS_LCA(struct Player *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__RealTimeBase), 11, RealTime)

#define ExternalSync(arg1, arg2, arg3) \
    __ExternalSync_WB(RealTimeBase, (arg1), (arg2), (arg3))

#define __NextConductor_WB(__RealTimeBase, __arg1) \
        AROS_LC1(struct Conductor *, NextConductor, \
                  AROS_LCA(struct Conductor *,(__arg1),A0), \
        struct Library *, (__RealTimeBase), 12, RealTime)

#define NextConductor(arg1) \
    __NextConductor_WB(RealTimeBase, (arg1))

#define __FindConductor_WB(__RealTimeBase, __arg1) \
        AROS_LC1(struct Conductor *, FindConductor, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
        struct Library *, (__RealTimeBase), 13, RealTime)

#define FindConductor(arg1) \
    __FindConductor_WB(RealTimeBase, (arg1))

#define __GetPlayerAttrsA_WB(__RealTimeBase, __arg1, __arg2) \
        AROS_LC2(BOOL, GetPlayerAttrsA, \
                  AROS_LCA(struct Player *,(__arg1),A0), \
                  AROS_LCA(struct TagItem *,(__arg2),A1), \
        struct Library *, (__RealTimeBase), 14, RealTime)

#define GetPlayerAttrsA(arg1, arg2) \
    __GetPlayerAttrsA_WB(RealTimeBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(REALTIME_NO_INLINE_STDARG)
#define GetPlayerAttrs(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    GetPlayerAttrsA((arg1), (struct TagItem *)__args); \
})
#endif /* !NO_INLINE_STDARG */

__END_DECLS

#endif /* DEFINES_REALTIME_H*/
