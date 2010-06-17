#ifndef DEFINES_REXXSYSLIB_H
#define DEFINES_REXXSYSLIB_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/rexxsyslib/rexxsyslib.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for rexxsyslib
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __CreateArgstring_WB(__RexxSysBase, __arg1, __arg2) \
        AROS_LC2(UBYTE *, CreateArgstring, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct RxsLib *, (__RexxSysBase), 21, RexxSys)

#define CreateArgstring(arg1, arg2) \
    __CreateArgstring_WB(RexxSysBase, (arg1), (arg2))

#define __DeleteArgstring_WB(__RexxSysBase, __arg1) \
        AROS_LC1NR(void, DeleteArgstring, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct RxsLib *, (__RexxSysBase), 22, RexxSys)

#define DeleteArgstring(arg1) \
    __DeleteArgstring_WB(RexxSysBase, (arg1))

#define __LengthArgstring_WB(__RexxSysBase, __arg1) \
        AROS_LC1(ULONG, LengthArgstring, \
                  AROS_LCA(UBYTE *,(__arg1),A0), \
        struct RxsLib *, (__RexxSysBase), 23, RexxSys)

#define LengthArgstring(arg1) \
    __LengthArgstring_WB(RexxSysBase, (arg1))

#define __CreateRexxMsg_WB(__RexxSysBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct RexxMsg *, CreateRexxMsg, \
                  AROS_LCA(struct MsgPort *,(__arg1),A0), \
                  AROS_LCA(UBYTE *,(__arg2),A1), \
                  AROS_LCA(UBYTE *,(__arg3),D0), \
        struct RxsLib *, (__RexxSysBase), 24, RexxSys)

#define CreateRexxMsg(arg1, arg2, arg3) \
    __CreateRexxMsg_WB(RexxSysBase, (arg1), (arg2), (arg3))

#define __DeleteRexxMsg_WB(__RexxSysBase, __arg1) \
        AROS_LC1NR(void, DeleteRexxMsg, \
                  AROS_LCA(struct RexxMsg *,(__arg1),A0), \
        struct RxsLib *, (__RexxSysBase), 25, RexxSys)

#define DeleteRexxMsg(arg1) \
    __DeleteRexxMsg_WB(RexxSysBase, (arg1))

#define __ClearRexxMsg_WB(__RexxSysBase, __arg1, __arg2) \
        AROS_LC2NR(void, ClearRexxMsg, \
                  AROS_LCA(struct RexxMsg *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct RxsLib *, (__RexxSysBase), 26, RexxSys)

#define ClearRexxMsg(arg1, arg2) \
    __ClearRexxMsg_WB(RexxSysBase, (arg1), (arg2))

#define __FillRexxMsg_WB(__RexxSysBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, FillRexxMsg, \
                  AROS_LCA(struct RexxMsg *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
                  AROS_LCA(ULONG,(__arg3),D1), \
        struct RxsLib *, (__RexxSysBase), 27, RexxSys)

#define FillRexxMsg(arg1, arg2, arg3) \
    __FillRexxMsg_WB(RexxSysBase, (arg1), (arg2), (arg3))

#define __IsRexxMsg_WB(__RexxSysBase, __arg1) \
        AROS_LC1(BOOL, IsRexxMsg, \
                  AROS_LCA(struct RexxMsg *,(__arg1),A0), \
        struct RxsLib *, (__RexxSysBase), 28, RexxSys)

#define IsRexxMsg(arg1) \
    __IsRexxMsg_WB(RexxSysBase, (arg1))

#define __LockRexxBase_WB(__RexxSysBase, __arg1) \
        AROS_LC1NR(void, LockRexxBase, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct RxsLib *, (__RexxSysBase), 75, RexxSys)

#define LockRexxBase(arg1) \
    __LockRexxBase_WB(RexxSysBase, (arg1))

#define __UnlockRexxBase_WB(__RexxSysBase, __arg1) \
        AROS_LC1NR(void, UnlockRexxBase, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct RxsLib *, (__RexxSysBase), 76, RexxSys)

#define UnlockRexxBase(arg1) \
    __UnlockRexxBase_WB(RexxSysBase, (arg1))

__END_DECLS

#endif /* DEFINES_REXXSYSLIB_H*/
