#ifndef DEFINES_MATHIEEEDOUBBAS_H
#define DEFINES_MATHIEEEDOUBBAS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathieeedoubbas/mathieeedoubbas.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mathieeedoubbas
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __IEEEDPFix_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(LONG, IEEEDPFix, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 5, MathIeeeDoubBas)

#define IEEEDPFix(arg1) \
    __IEEEDPFix_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPFlt_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(double, IEEEDPFlt, \
                  AROS_LCA(LONG,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 6, MathIeeeDoubBas)

#define IEEEDPFlt(arg1) \
    __IEEEDPFlt_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPCmp_WB(__MathIeeeDoubBasBase, __arg1, __arg2) \
        AROS_LC2(LONG, IEEEDPCmp, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubBasBase), 7, MathIeeeDoubBas)

#define IEEEDPCmp(arg1, arg2) \
    __IEEEDPCmp_WB(MathIeeeDoubBasBase, (arg1), (arg2))

#define __IEEEDPTst_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(LONG, IEEEDPTst, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 8, MathIeeeDoubBas)

#define IEEEDPTst(arg1) \
    __IEEEDPTst_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPAbs_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(double, IEEEDPAbs, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 9, MathIeeeDoubBas)

#define IEEEDPAbs(arg1) \
    __IEEEDPAbs_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPNeg_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(double, IEEEDPNeg, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 10, MathIeeeDoubBas)

#define IEEEDPNeg(arg1) \
    __IEEEDPNeg_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPAdd_WB(__MathIeeeDoubBasBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPAdd, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubBasBase), 11, MathIeeeDoubBas)

#define IEEEDPAdd(arg1, arg2) \
    __IEEEDPAdd_WB(MathIeeeDoubBasBase, (arg1), (arg2))

#define __IEEEDPSub_WB(__MathIeeeDoubBasBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPSub, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubBasBase), 12, MathIeeeDoubBas)

#define IEEEDPSub(arg1, arg2) \
    __IEEEDPSub_WB(MathIeeeDoubBasBase, (arg1), (arg2))

#define __IEEEDPMul_WB(__MathIeeeDoubBasBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPMul, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubBasBase), 13, MathIeeeDoubBas)

#define IEEEDPMul(arg1, arg2) \
    __IEEEDPMul_WB(MathIeeeDoubBasBase, (arg1), (arg2))

#define __IEEEDPDiv_WB(__MathIeeeDoubBasBase, __arg1, __arg2) \
        AROS_LC2(double, IEEEDPDiv, \
                  AROS_LCA(double,(__arg1),D0/D1), \
                  AROS_LCA(double,(__arg2),D2/D3), \
        struct Library *, (__MathIeeeDoubBasBase), 14, MathIeeeDoubBas)

#define IEEEDPDiv(arg1, arg2) \
    __IEEEDPDiv_WB(MathIeeeDoubBasBase, (arg1), (arg2))

#define __IEEEDPFloor_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(double, IEEEDPFloor, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 15, MathIeeeDoubBas)

#define IEEEDPFloor(arg1) \
    __IEEEDPFloor_WB(MathIeeeDoubBasBase, (arg1))

#define __IEEEDPCeil_WB(__MathIeeeDoubBasBase, __arg1) \
        AROS_LC1(double, IEEEDPCeil, \
                  AROS_LCA(double,(__arg1),D0/D1), \
        struct Library *, (__MathIeeeDoubBasBase), 16, MathIeeeDoubBas)

#define IEEEDPCeil(arg1) \
    __IEEEDPCeil_WB(MathIeeeDoubBasBase, (arg1))

__END_DECLS

#endif /* DEFINES_MATHIEEEDOUBBAS_H*/
