#ifndef CLIB_REXXSYSLIB_PROTOS_H
#define CLIB_REXXSYSLIB_PROTOS_H

/*
    *** Automatically generated from 'rexxsyslib.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <rexx/storage.h>
AROS_LP2(UBYTE *, CreateArgstring,
         AROS_LPA(UBYTE *, string, A0),
         AROS_LPA(ULONG, length, D0),
         LIBBASETYPEPTR, RexxSysBase, 21, RexxSys
);
AROS_LP1(void, DeleteArgstring,
         AROS_LPA(UBYTE *, argstring, A0),
         LIBBASETYPEPTR, RexxSysBase, 22, RexxSys
);
AROS_LP1(ULONG, LengthArgstring,
         AROS_LPA(UBYTE *, argstring, A0),
         LIBBASETYPEPTR, RexxSysBase, 23, RexxSys
);
AROS_LP3(struct RexxMsg *, CreateRexxMsg,
         AROS_LPA(struct MsgPort *, port, A0),
         AROS_LPA(UBYTE *, extension, A1),
         AROS_LPA(UBYTE *, host, D0),
         LIBBASETYPEPTR, RexxSysBase, 24, RexxSys
);
AROS_LP1(void, DeleteRexxMsg,
         AROS_LPA(struct RexxMsg *, packet, A0),
         LIBBASETYPEPTR, RexxSysBase, 25, RexxSys
);
AROS_LP2(void, ClearRexxMsg,
         AROS_LPA(struct RexxMsg *, msgptr, A0),
         AROS_LPA(ULONG, count, D0),
         LIBBASETYPEPTR, RexxSysBase, 26, RexxSys
);
AROS_LP3(BOOL, FillRexxMsg,
         AROS_LPA(struct RexxMsg *, msgptr, A0),
         AROS_LPA(ULONG, count, D0),
         AROS_LPA(ULONG, mask, D1),
         LIBBASETYPEPTR, RexxSysBase, 27, RexxSys
);
AROS_LP1(BOOL, IsRexxMsg,
         AROS_LPA(struct RexxMsg *, msgptr, A0),
         LIBBASETYPEPTR, RexxSysBase, 28, RexxSys
);
AROS_LP1(void, LockRexxBase,
         AROS_LPA(ULONG, resource, D0),
         LIBBASETYPEPTR, RexxSysBase, 75, RexxSys
);
AROS_LP1(void, UnlockRexxBase,
         AROS_LPA(ULONG, resource, D0),
         LIBBASETYPEPTR, RexxSysBase, 76, RexxSys
);

#endif /* CLIB_REXXSYSLIB_PROTOS_H */
