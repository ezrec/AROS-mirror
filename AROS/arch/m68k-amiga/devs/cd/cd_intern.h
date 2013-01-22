/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef CD_INTERN_H
#define CD_INTERN_H

#include <exec/devices.h>
#include <exec/io.h>

#include LC_LIBDEFS_FILE

struct cdBase {
    struct Device cb_Device;
    struct List cb_Units;
    ULONG       cb_MaxUnit;
};

struct cdUnitOps {
    CONST_STRPTR  uo_Name;
    LONG        (*uo_DoIO)(struct IOStdReq *io, APTR priv);
    VOID        (*uo_Expunge)(APTR priv);
};

LONG cdAddUnit(LIBBASETYPE *cb, const struct cdUnitOps *ops, APTR priv);

#define IOF_ABORT       (1 << 7)

#endif /* CD_INTERN_H */
