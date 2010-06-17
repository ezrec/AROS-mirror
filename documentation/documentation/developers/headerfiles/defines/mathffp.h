#ifndef DEFINES_MATHFFP_H
#define DEFINES_MATHFFP_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/mathffp/mathffp.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mathffp
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __SPFix_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPFix, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 5, Mathffp)

#define SPFix(arg1) \
    __SPFix_WB(MathBase, (arg1))

#define __SPFlt_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPFlt, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 6, Mathffp)

#define SPFlt(arg1) \
    __SPFlt_WB(MathBase, (arg1))

#define __SPCmp_WB(__MathBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPCmp, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
        struct Library *, (__MathBase), 7, Mathffp)

#define SPCmp(arg1, arg2) \
    __SPCmp_WB(MathBase, (arg1), (arg2))

#define __SPTst_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPTst, \
                  AROS_LCA(LONG,(__arg1),D1), \
        struct Library *, (__MathBase), 8, Mathffp)

#define SPTst(arg1) \
    __SPTst_WB(MathBase, (arg1))

#define __SPAbs_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPAbs, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 9, Mathffp)

#define SPAbs(arg1) \
    __SPAbs_WB(MathBase, (arg1))

#define __SPNeg_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPNeg, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 10, Mathffp)

#define SPNeg(arg1) \
    __SPNeg_WB(MathBase, (arg1))

#define __SPAdd_WB(__MathBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPAdd, \
                  AROS_LCA(LONG,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathBase), 11, Mathffp)

#define SPAdd(arg1, arg2) \
    __SPAdd_WB(MathBase, (arg1), (arg2))

#define __SPSub_WB(__MathBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPSub, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
        struct Library *, (__MathBase), 12, Mathffp)

#define SPSub(arg1, arg2) \
    __SPSub_WB(MathBase, (arg1), (arg2))

#define __SPMul_WB(__MathBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPMul, \
                  AROS_LCA(LONG,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathBase), 13, Mathffp)

#define SPMul(arg1, arg2) \
    __SPMul_WB(MathBase, (arg1), (arg2))

#define __SPDiv_WB(__MathBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPDiv, \
                  AROS_LCA(LONG,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathBase), 14, Mathffp)

#define SPDiv(arg1, arg2) \
    __SPDiv_WB(MathBase, (arg1), (arg2))

#define __SPFloor_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPFloor, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 15, Mathffp)

#define SPFloor(arg1) \
    __SPFloor_WB(MathBase, (arg1))

#define __SPCeil_WB(__MathBase, __arg1) \
        AROS_LC1(LONG, SPCeil, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathBase), 16, Mathffp)

#define SPCeil(arg1) \
    __SPCeil_WB(MathBase, (arg1))

__END_DECLS

#endif /* DEFINES_MATHFFP_H*/
