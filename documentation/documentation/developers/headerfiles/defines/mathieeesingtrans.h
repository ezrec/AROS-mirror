#ifndef DEFINES_MATHIEEESINGTRANS_H
#define DEFINES_MATHIEEESINGTRANS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathieeesingtrans/mathieeesingtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mathieeesingtrans
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __IEEESPAtan_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPAtan, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 5, MathIeeeSingTrans)

#define IEEESPAtan(arg1) \
    __IEEESPAtan_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPSin_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPSin, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 6, MathIeeeSingTrans)

#define IEEESPSin(arg1) \
    __IEEESPSin_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPCos_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPCos, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 7, MathIeeeSingTrans)

#define IEEESPCos(arg1) \
    __IEEESPCos_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPTan_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPTan, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 8, MathIeeeSingTrans)

#define IEEESPTan(arg1) \
    __IEEESPTan_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPSincos_WB(__MathIeeeSingTransBase, __arg1, __arg2) \
        AROS_LC2(LONG, IEEESPSincos, \
                  AROS_LCA(LONG *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathIeeeSingTransBase), 9, MathIeeeSingTrans)

#define IEEESPSincos(arg1, arg2) \
    __IEEESPSincos_WB(MathIeeeSingTransBase, (arg1), (arg2))

#define __IEEESPSinh_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPSinh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 10, MathIeeeSingTrans)

#define IEEESPSinh(arg1) \
    __IEEESPSinh_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPCosh_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPCosh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 11, MathIeeeSingTrans)

#define IEEESPCosh(arg1) \
    __IEEESPCosh_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPTanh_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPTanh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 12, MathIeeeSingTrans)

#define IEEESPTanh(arg1) \
    __IEEESPTanh_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPExp_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPExp, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 13, MathIeeeSingTrans)

#define IEEESPExp(arg1) \
    __IEEESPExp_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPLog_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPLog, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 14, MathIeeeSingTrans)

#define IEEESPLog(arg1) \
    __IEEESPLog_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPPow_WB(__MathIeeeSingTransBase, __arg1, __arg2) \
        AROS_LC2(LONG, IEEESPPow, \
                  AROS_LCA(LONG,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathIeeeSingTransBase), 15, MathIeeeSingTrans)

#define IEEESPPow(arg1, arg2) \
    __IEEESPPow_WB(MathIeeeSingTransBase, (arg1), (arg2))

#define __IEEESPSqrt_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPSqrt, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 16, MathIeeeSingTrans)

#define IEEESPSqrt(arg1) \
    __IEEESPSqrt_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPTieee_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPTieee, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 17, MathIeeeSingTrans)

#define IEEESPTieee(arg1) \
    __IEEESPTieee_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPFieee_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPFieee, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 18, MathIeeeSingTrans)

#define IEEESPFieee(arg1) \
    __IEEESPFieee_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPAsin_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPAsin, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 19, MathIeeeSingTrans)

#define IEEESPAsin(arg1) \
    __IEEESPAsin_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPAcos_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPAcos, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 20, MathIeeeSingTrans)

#define IEEESPAcos(arg1) \
    __IEEESPAcos_WB(MathIeeeSingTransBase, (arg1))

#define __IEEESPLog10_WB(__MathIeeeSingTransBase, __arg1) \
        AROS_LC1(LONG, IEEESPLog10, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathIeeeSingTransBase), 21, MathIeeeSingTrans)

#define IEEESPLog10(arg1) \
    __IEEESPLog10_WB(MathIeeeSingTransBase, (arg1))

__END_DECLS

#endif /* DEFINES_MATHIEEESINGTRANS_H*/
