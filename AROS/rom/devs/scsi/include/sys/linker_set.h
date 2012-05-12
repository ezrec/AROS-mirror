/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef LINKER_SET_H
#define LINKER_SET_H

#include <aros/symbolsets.h>

DECLARESET(modmetadata_set)

#define DATA_SET(set, sym)      ADD2SET(sym, set, 0)

#endif /* LINKER_SET_H */
