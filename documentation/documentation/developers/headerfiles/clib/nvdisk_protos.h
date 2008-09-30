#ifndef CLIB_NVDISK_PROTOS_H
#define CLIB_NVDISK_PROTOS_H

/*
    *** Automatically generated from 'nvdisk.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <libraries/nonvolatile.h>
AROS_LP2(APTR, ReadData,
         AROS_LPA(STRPTR, appName, A0),
         AROS_LPA(STRPTR, itemName, A1),
         LIBBASETYPEPTR, nvdBase, 5, NVDisk
);
AROS_LP4(LONG, WriteData,
         AROS_LPA(STRPTR, appName, A0),
         AROS_LPA(STRPTR, itemName, A1),
         AROS_LPA(APTR, data, A2),
         AROS_LPA(LONG, length, A3),
         LIBBASETYPEPTR, nvdBase, 6, NVDisk
);
AROS_LP2(BOOL, DeleteData,
         AROS_LPA(STRPTR, appName, A0),
         AROS_LPA(STRPTR, itemName, A1),
         LIBBASETYPEPTR, nvdBase, 7, NVDisk
);
AROS_LP1(BOOL, MemInfo,
         AROS_LPA(struct NVInfo *, nvInfo, A0),
         LIBBASETYPEPTR, nvdBase, 8, NVDisk
);
AROS_LP3(BOOL, SetProtection,
         AROS_LPA(STRPTR, appName, A0),
         AROS_LPA(STRPTR, itemName, A1),
         AROS_LPA(LONG, mask, D0),
         LIBBASETYPEPTR, nvdBase, 9, NVDisk
);
AROS_LP1(struct MinList *, GetItemList,
         AROS_LPA(STRPTR, appName, A0),
         LIBBASETYPEPTR, nvdBase, 10, NVDisk
);

#endif /* CLIB_NVDISK_PROTOS_H */
