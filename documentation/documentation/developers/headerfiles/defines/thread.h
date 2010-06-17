#ifndef DEFINES_THREAD_H
#define DEFINES_THREAD_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/thread/thread.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for thread
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __CreateThread_WB(__ThreadBase, __arg1, __arg2) \
        AROS_LC2(uint32_t, CreateThread, \
                  AROS_LCA(ThreadEntryFunction,(__arg1),A0), \
                  AROS_LCA(void *,(__arg2),A1), \
        struct Library *, (__ThreadBase), 5, Thread)

#define CreateThread(arg1, arg2) \
    __CreateThread_WB(ThreadBase, (arg1), (arg2))

#define __WaitThread_WB(__ThreadBase, __arg1, __arg2) \
        AROS_LC2(BOOL, WaitThread, \
                  AROS_LCA(uint32_t,(__arg1),D0), \
                  AROS_LCA(void **,(__arg2),A0), \
        struct Library *, (__ThreadBase), 6, Thread)

#define WaitThread(arg1, arg2) \
    __WaitThread_WB(ThreadBase, (arg1), (arg2))

#define __WaitAllThreads_WB(__ThreadBase) \
        AROS_LC0NR(void, WaitAllThreads, \
        struct Library *, (__ThreadBase), 7, Thread)

#define WaitAllThreads() \
    __WaitAllThreads_WB(ThreadBase)

#define __DetachThread_WB(__ThreadBase, __arg1) \
        AROS_LC1(BOOL, DetachThread, \
                  AROS_LCA(uint32_t,(__arg1),D0), \
        struct Library *, (__ThreadBase), 8, Thread)

#define DetachThread(arg1) \
    __DetachThread_WB(ThreadBase, (arg1))

#define __CurrentThread_WB(__ThreadBase) \
        AROS_LC0(uint32_t, CurrentThread, \
        struct Library *, (__ThreadBase), 9, Thread)

#define CurrentThread() \
    __CurrentThread_WB(ThreadBase)

#define __CreateMutex_WB(__ThreadBase) \
        AROS_LC0(void *, CreateMutex, \
        struct Library *, (__ThreadBase), 10, Thread)

#define CreateMutex() \
    __CreateMutex_WB(ThreadBase)

#define __DestroyMutex_WB(__ThreadBase, __arg1) \
        AROS_LC1(BOOL, DestroyMutex, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 11, Thread)

#define DestroyMutex(arg1) \
    __DestroyMutex_WB(ThreadBase, (arg1))

#define __LockMutex_WB(__ThreadBase, __arg1) \
        AROS_LC1NR(void, LockMutex, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 12, Thread)

#define LockMutex(arg1) \
    __LockMutex_WB(ThreadBase, (arg1))

#define __TryLockMutex_WB(__ThreadBase, __arg1) \
        AROS_LC1(BOOL, TryLockMutex, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 13, Thread)

#define TryLockMutex(arg1) \
    __TryLockMutex_WB(ThreadBase, (arg1))

#define __UnlockMutex_WB(__ThreadBase, __arg1) \
        AROS_LC1NR(void, UnlockMutex, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 14, Thread)

#define UnlockMutex(arg1) \
    __UnlockMutex_WB(ThreadBase, (arg1))

#define __CreateCondition_WB(__ThreadBase) \
        AROS_LC0(void *, CreateCondition, \
        struct Library *, (__ThreadBase), 15, Thread)

#define CreateCondition() \
    __CreateCondition_WB(ThreadBase)

#define __DestroyCondition_WB(__ThreadBase, __arg1) \
        AROS_LC1(BOOL, DestroyCondition, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 16, Thread)

#define DestroyCondition(arg1) \
    __DestroyCondition_WB(ThreadBase, (arg1))

#define __WaitCondition_WB(__ThreadBase, __arg1, __arg2) \
        AROS_LC2(BOOL, WaitCondition, \
                  AROS_LCA(void *,(__arg1),A0), \
                  AROS_LCA(void *,(__arg2),A1), \
        struct Library *, (__ThreadBase), 17, Thread)

#define WaitCondition(arg1, arg2) \
    __WaitCondition_WB(ThreadBase, (arg1), (arg2))

#define __SignalCondition_WB(__ThreadBase, __arg1) \
        AROS_LC1NR(void, SignalCondition, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 18, Thread)

#define SignalCondition(arg1) \
    __SignalCondition_WB(ThreadBase, (arg1))

#define __BroadcastCondition_WB(__ThreadBase, __arg1) \
        AROS_LC1NR(void, BroadcastCondition, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 19, Thread)

#define BroadcastCondition(arg1) \
    __BroadcastCondition_WB(ThreadBase, (arg1))

#define __ExitThread_WB(__ThreadBase, __arg1) \
        AROS_LC1NR(void, ExitThread, \
                  AROS_LCA(void *,(__arg1),A0), \
        struct Library *, (__ThreadBase), 20, Thread)

#define ExitThread(arg1) \
    __ExitThread_WB(ThreadBase, (arg1))

__END_DECLS

#endif /* DEFINES_THREAD_H*/
