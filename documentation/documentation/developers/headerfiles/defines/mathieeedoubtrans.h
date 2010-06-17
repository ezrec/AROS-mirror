#ifndef DEFINES_MATHIEEEDOUBTRANS_H
#define DEFINES_MATHIEEEDOUBTRANS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathieeedoubtrans/mathieeedoubtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mathieeedoubtrans
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __IEEEDPAtan_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPAtan, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 5, MathIeeeDoubTrans)

#define IEEEDPAtan(arg1) \
    __IEEEDPAtan_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPSin_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPSin, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 6, MathIeeeDoubTrans)

#define IEEEDPSin(arg1) \
    __IEEEDPSin_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPCos_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPCos, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 7, MathIeeeDoubTrans)

#define IEEEDPCos(arg1) \
    __IEEEDPCos_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPSincos_WB(__MathIeeeDoubTransBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPSincos, \
                  AROS_LCA(double *,(__arg1),A0), \
                  AROS_LCA(double,(__arg2),D0), \
        struct Library *, (__MathIeeeDoubTransBase), 8, MathIeeeDoubTrans)

#define IEEEDPSincos(arg1, arg2) \
    __IEEEDPSincos_WB(MathIeeeDoubTransBase, (arg1), (arg2))

#define __IEEEDPSinh_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPSinh, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 9, MathIeeeDoubTrans)

#define IEEEDPSinh(arg1) \
    __IEEEDPSinh_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPCosh_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPCosh, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 10, MathIeeeDoubTrans)

#define IEEEDPCosh(arg1) \
    __IEEEDPCosh_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPTanh_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPTanh, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 11, MathIeeeDoubTrans)

#define IEEEDPTanh(arg1) \
    __IEEEDPTanh_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPExp_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPExp, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 12, MathIeeeDoubTrans)

#define IEEEDPExp(arg1) \
    __IEEEDPExp_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPLog_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPLog, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 13, MathIeeeDoubTrans)

#define IEEEDPLog(arg1) \
    __IEEEDPLog_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPPow_WB(__MathIeeeDoubTransBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPPow, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubTransBase), 14, MathIeeeDoubTrans)

#define IEEEDPPow(arg1, arg2) \
    __IEEEDPPow_WB(MathIeeeDoubTransBase, (arg1), (arg2))

#define __IEEEDPSqrt_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPSqrt, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 15, MathIeeeDoubTrans)

#define IEEEDPSqrt(arg1) \
    __IEEEDPSqrt_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPTieee_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(LONG, IEEEDPTieee, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 16, MathIeeeDoubTrans)

#define IEEEDPTieee(arg1) \
    __IEEEDPTieee_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPFieee_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPFieee, \
                  AROS_LCA(LONG,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 17, MathIeeeDoubTrans)

#define IEEEDPFieee(arg1) \
    __IEEEDPFieee_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPAsin_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPAsin, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 18, MathIeeeDoubTrans)

#define IEEEDPAsin(arg1) \
    __IEEEDPAsin_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPAcos_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPAcos, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 19, MathIeeeDoubTrans)

#define IEEEDPAcos(arg1) \
    __IEEEDPAcos_WB(MathIeeeDoubTransBase, (arg1))

#define __IEEEDPLog10_WB(__MathIeeeDoubTransBase, __arg1) \
        AROS_LC1(double, IEEEDPLog10, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubTransBase), 20, MathIeeeDoubTrans)

#define IEEEDPLog10(arg1) \
    __IEEEDPLog10_WB(MathIeeeDoubTransBase, (arg1))

__END_DECLS

#endif /* DEFINES_MATHIEEEDOUBTRANS_H*/
