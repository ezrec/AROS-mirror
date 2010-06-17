#ifndef CLIB_MATHIEEEDOUBTRANS_PROTOS_H
#define CLIB_MATHIEEEDOUBTRANS_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mathieeedoubtrans/mathieeedoubtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LPQUAD1(double, IEEEDPAtan,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 5, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPSin,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 6, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPCos,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 7, MathIeeeDoubTrans
);
AROS_LP2(double, IEEEDPSincos,
         AROS_LPA(double *, z, A0),
         AROS_LPA(double, y, D0),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 8, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPSinh,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 9, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPCosh,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 10, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPTanh,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 11, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPExp,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 12, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPLog,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 13, MathIeeeDoubTrans
);
AROS_LPQUAD2(double, IEEEDPPow,
         AROS_LPAQUAD(double, x, D0, D1),
         AROS_LPAQUAD(double, y, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 14, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPSqrt,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 15, MathIeeeDoubTrans
);
AROS_LPQUAD1(LONG, IEEEDPTieee,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 16, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPFieee,
         AROS_LPAQUAD(LONG, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 17, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPAsin,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 18, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPAcos,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 19, MathIeeeDoubTrans
);
AROS_LPQUAD1(double, IEEEDPLog10,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubTransBase, 20, MathIeeeDoubTrans
);

__END_DECLS

#endif /* CLIB_MATHIEEEDOUBTRANS_PROTOS_H */
