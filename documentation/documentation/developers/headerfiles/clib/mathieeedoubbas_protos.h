#ifndef CLIB_MATHIEEEDOUBBAS_PROTOS_H
#define CLIB_MATHIEEEDOUBBAS_PROTOS_H

/*
    *** Automatically generated from 'mathieeedoubbas.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
AROS_LPQUAD1(LONG, IEEEDPFix,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 5, MathIeeeDoubBas
);
AROS_LPQUAD1(double, IEEEDPFlt,
         AROS_LPAQUAD(LONG, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 6, MathIeeeDoubBas
);
AROS_LPQUAD2(LONG, IEEEDPCmp,
         AROS_LPAQUAD(double, y, D0, D1),
         AROS_LPAQUAD(double, z, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 7, MathIeeeDoubBas
);
AROS_LPQUAD1(LONG, IEEEDPTst,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 8, MathIeeeDoubBas
);
AROS_LPQUAD1(double, IEEEDPAbs,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 9, MathIeeeDoubBas
);
AROS_LPQUAD1(double, IEEEDPNeg,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 10, MathIeeeDoubBas
);
AROS_LPQUAD2(double, IEEEDPAdd,
         AROS_LPAQUAD(double, y, D0, D1),
         AROS_LPAQUAD(double, z, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 11, MathIeeeDoubBas
);
AROS_LPQUAD2(double, IEEEDPSub,
         AROS_LPAQUAD(double, y, D0, D1),
         AROS_LPAQUAD(double, z, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 12, MathIeeeDoubBas
);
AROS_LPQUAD2(double, IEEEDPMul,
         AROS_LPAQUAD(double, y, D0, D1),
         AROS_LPAQUAD(double, z, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 13, MathIeeeDoubBas
);
AROS_LPQUAD2(double, IEEEDPDiv,
         AROS_LPAQUAD(double, y, D0, D1),
         AROS_LPAQUAD(double, z, D2, D3),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 14, MathIeeeDoubBas
);
AROS_LPQUAD1(double, IEEEDPFloor,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 15, MathIeeeDoubBas
);
AROS_LPQUAD1(double, IEEEDPCeil,
         AROS_LPAQUAD(double, y, D0, D1),
         LIBBASETYPEPTR, MathIeeeDoubBasBase, 16, MathIeeeDoubBas
);

#endif /* CLIB_MATHIEEEDOUBBAS_PROTOS_H */
