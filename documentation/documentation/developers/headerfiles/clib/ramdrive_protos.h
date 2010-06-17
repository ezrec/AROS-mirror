#ifndef CLIB_RAMDRIVE_PROTOS_H
#define CLIB_RAMDRIVE_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/devs/ramdrive.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LP0(STRPTR, killrad0,
         LIBBASETYPEPTR, RamdriveBase, 7, Ramdrive
);
AROS_LP1(STRPTR, killrad,
         AROS_LPA(ULONG, unit, D0),
         LIBBASETYPEPTR, RamdriveBase, 8, Ramdrive
);

__END_DECLS

#endif /* CLIB_RAMDRIVE_PROTOS_H */
