#ifndef DEFINES_MATHTRANS_H
#define DEFINES_MATHTRANS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathtrans/mathtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mathtrans
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __SPAtan_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPAtan, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 5, MathTrans)

#define SPAtan(arg1) \
    __SPAtan_WB(MathTransBase, (arg1))

#define __SPSin_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPSin, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 6, MathTrans)

#define SPSin(arg1) \
    __SPSin_WB(MathTransBase, (arg1))

#define __SPCos_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPCos, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 7, MathTrans)

#define SPCos(arg1) \
    __SPCos_WB(MathTransBase, (arg1))

#define __SPTan_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPTan, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 8, MathTrans)

#define SPTan(arg1) \
    __SPTan_WB(MathTransBase, (arg1))

#define __SPSincos_WB(__MathTransBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPSincos, \
                  AROS_LCA(IPTR *,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathTransBase), 9, MathTrans)

#define SPSincos(arg1, arg2) \
    __SPSincos_WB(MathTransBase, (arg1), (arg2))

#define __SPSinh_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPSinh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 10, MathTrans)

#define SPSinh(arg1) \
    __SPSinh_WB(MathTransBase, (arg1))

#define __SPCosh_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPCosh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 11, MathTrans)

#define SPCosh(arg1) \
    __SPCosh_WB(MathTransBase, (arg1))

#define __SPTanh_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPTanh, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 12, MathTrans)

#define SPTanh(arg1) \
    __SPTanh_WB(MathTransBase, (arg1))

#define __SPExp_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPExp, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 13, MathTrans)

#define SPExp(arg1) \
    __SPExp_WB(MathTransBase, (arg1))

#define __SPLog_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPLog, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 14, MathTrans)

#define SPLog(arg1) \
    __SPLog_WB(MathTransBase, (arg1))

#define __SPPow_WB(__MathTransBase, __arg1, __arg2) \
        AROS_LC2(LONG, SPPow, \
                  AROS_LCA(LONG,(__arg1),D1), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__MathTransBase), 15, MathTrans)

#define SPPow(arg1, arg2) \
    __SPPow_WB(MathTransBase, (arg1), (arg2))

#define __SPSqrt_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPSqrt, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 16, MathTrans)

#define SPSqrt(arg1) \
    __SPSqrt_WB(MathTransBase, (arg1))

#define __SPTieee_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPTieee, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 17, MathTrans)

#define SPTieee(arg1) \
    __SPTieee_WB(MathTransBase, (arg1))

#define __SPFieee_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPFieee, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 18, MathTrans)

#define SPFieee(arg1) \
    __SPFieee_WB(MathTransBase, (arg1))

#define __SPAsin_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPAsin, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 19, MathTrans)

#define SPAsin(arg1) \
    __SPAsin_WB(MathTransBase, (arg1))

#define __SPAcos_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPAcos, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 20, MathTrans)

#define SPAcos(arg1) \
    __SPAcos_WB(MathTransBase, (arg1))

#define __SPLog10_WB(__MathTransBase, __arg1) \
        AROS_LC1(LONG, SPLog10, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__MathTransBase), 21, MathTrans)

#define SPLog10(arg1) \
    __SPLog10_WB(MathTransBase, (arg1))

__END_DECLS

#endif /* DEFINES_MATHTRANS_H*/
