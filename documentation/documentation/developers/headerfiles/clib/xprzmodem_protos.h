#ifndef CLIB_XPRZMODEM_PROTOS_H
#define CLIB_XPRZMODEM_PROTOS_H

/*
    *** Automatically generated from 'lib.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
struct XPR_IO;
AROS_LP1(long, XProtocolCleanup,
         AROS_LPA(struct XPR_IO*, xio, A0),
         LIBBASETYPEPTR, XProtocolBase, 5, XProtocol
);
AROS_LP1(long, XProtocolSetup,
         AROS_LPA(struct XPR_IO *, xio, A0),
         LIBBASETYPEPTR, XProtocolBase, 6, XProtocol
);
AROS_LP1(long, XProtocolSend,
         AROS_LPA(struct XPR_IO *, xio, A0),
         LIBBASETYPEPTR, XProtocolBase, 7, XProtocol
);
AROS_LP1(long, XProtocolReceive,
         AROS_LPA(struct XPR_IO *, xio, A0),
         LIBBASETYPEPTR, XProtocolBase, 8, XProtocol
);
AROS_LP4(long, XProtocolHostMon,
         AROS_LPA(struct XPR_IO *, xio, A0),
         AROS_LPA(char *, serbuff, A1),
         AROS_LPA(long, actual, D0),
         AROS_LPA(long, maxsize, D1),
         LIBBASETYPEPTR, XProtocolBase, 9, XProtocol
);
AROS_LP4(long, XProtocolUserMon,
         AROS_LPA(struct XPR_IO *, xio, A0),
         AROS_LPA(char *, serbuff, A1),
         AROS_LPA(long, actual, D0),
         AROS_LPA(long, maxsize, D1),
         LIBBASETYPEPTR, XProtocolBase, 10, XProtocol
);

#endif /* CLIB_XPRZMODEM_PROTOS_H */
