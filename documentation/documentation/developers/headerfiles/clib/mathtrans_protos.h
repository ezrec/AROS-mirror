#ifndef CLIB_MATHTRANS_PROTOS_H
#define CLIB_MATHTRANS_PROTOS_H

/*
    *** Automatically generated from 'mathtrans.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
AROS_LP1(LONG, SPAtan,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 5, MathTrans
);
AROS_LP1(LONG, SPSin,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 6, MathTrans
);
AROS_LP1(LONG, SPCos,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 7, MathTrans
);
AROS_LP1(LONG, SPTan,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 8, MathTrans
);
AROS_LP2(LONG, SPSincos,
         AROS_LPA(IPTR *, pfnum2, D1),
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 9, MathTrans
);
AROS_LP1(LONG, SPSinh,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 10, MathTrans
);
AROS_LP1(LONG, SPCosh,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 11, MathTrans
);
AROS_LP1(LONG, SPTanh,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 12, MathTrans
);
AROS_LP1(LONG, SPExp,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 13, MathTrans
);
AROS_LP1(LONG, SPLog,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 14, MathTrans
);
AROS_LP2(LONG, SPPow,
         AROS_LPA(LONG, fnum1, D1),
         AROS_LPA(LONG, fnum2, D0),
         LIBBASETYPEPTR, MathTransBase, 15, MathTrans
);
AROS_LP1(LONG, SPSqrt,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 16, MathTrans
);
AROS_LP1(LONG, SPTieee,
         AROS_LPA(LONG, fnum, D0),
         LIBBASETYPEPTR, MathTransBase, 17, MathTrans
);
AROS_LP1(LONG, SPFieee,
         AROS_LPA(LONG, ieeenum, D0),
         LIBBASETYPEPTR, MathTransBase, 18, MathTrans
);
AROS_LP1(LONG, SPAsin,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 19, MathTrans
);
AROS_LP1(LONG, SPAcos,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 20, MathTrans
);
AROS_LP1(LONG, SPLog10,
         AROS_LPA(LONG, fnum1, D0),
         LIBBASETYPEPTR, MathTransBase, 21, MathTrans
);

#endif /* CLIB_MATHTRANS_PROTOS_H */
