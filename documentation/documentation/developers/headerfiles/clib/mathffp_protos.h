#ifndef CLIB_MATHFFP_PROTOS_H
#define CLIB_MATHFFP_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/mathffp/mathffp.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LP1(LONG, SPFix,
         AROS_LPA(LONG, fnum, D0),
         LIBBASETYPEPTR, MathBase, 5, Mathffp
);
AROS_LP1(LONG, SPFlt,
         AROS_LPA(LONG, inum, D0),
         LIBBASETYPEPTR, MathBase, 6, Mathffp
);
AROS_LP2(LONG, SPCmp,
         AROS_LPA(LONG, fnum1, D0),
         AROS_LPA(LONG, fnum2, D1),
         LIBBASETYPEPTR, MathBase, 7, Mathffp
);
AROS_LP1(LONG, SPTst,
         AROS_LPA(LONG, fnum, D1),
         LIBBASETYPEPTR, MathBase, 8, Mathffp
);
AROS_LP1(LONG, SPAbs,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathBase, 9, Mathffp
);
AROS_LP1(LONG, SPNeg,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathBase, 10, Mathffp
);
AROS_LP2(LONG, SPAdd,
         AROS_LPA(LONG, fnum1, D1),
         AROS_LPA(LONG, fnum2, D0),
         LIBBASETYPEPTR, MathBase, 11, Mathffp
);
AROS_LP2(LONG, SPSub,
         AROS_LPA(LONG, fnum1, D0),
         AROS_LPA(LONG, fnum2, D1),
         LIBBASETYPEPTR, MathBase, 12, Mathffp
);
AROS_LP2(LONG, SPMul,
         AROS_LPA(LONG, fnum1, D1),
         AROS_LPA(LONG, fnum2, D0),
         LIBBASETYPEPTR, MathBase, 13, Mathffp
);
AROS_LP2(LONG, SPDiv,
         AROS_LPA(LONG, fnum1, D1),
         AROS_LPA(LONG, fnum2, D0),
         LIBBASETYPEPTR, MathBase, 14, Mathffp
);
AROS_LP1(LONG, SPFloor,
         AROS_LPA(LONG, y, D0),
         LIBBASETYPEPTR, MathBase, 15, Mathffp
);
AROS_LP1(LONG, SPCeil,
         AROS_LPA(LONG, y, D0),
         LIBBASETYPEPTR, MathBase, 16, Mathffp
);

__END_DECLS

#endif /* CLIB_MATHFFP_PROTOS_H */
