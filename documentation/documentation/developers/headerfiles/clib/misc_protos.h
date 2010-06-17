#ifndef CLIB_MISC_PROTOS_H
#define CLIB_MISC_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/rom/misc/misc.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LP2(char *, AllocMiscResource,
         AROS_LPA(ULONG, unitNum, D0),
         AROS_LPA(char *, name, A0),
         LIBBASETYPEPTR, MiscBase, 1, Misc
);
AROS_LP1(void, FreeMiscResource,
         AROS_LPA(ULONG, unitNum, D0),
         LIBBASETYPEPTR, MiscBase, 2, Misc
);

__END_DECLS

#endif /* CLIB_MISC_PROTOS_H */
