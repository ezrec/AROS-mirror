/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_PARAM_H
#define SYS_PARAM_H

#include <machine/aros.h>

#define MAXPATHLEN      PATH_MAX

#define PCATCH  0x00000100      /* tsleep checks signals */

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define roundup2(x, y)  (((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#define powerof2(x)     ((((x)-1)&(x))==0)
#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))

static inline int ffs(int mask) { return __builtin_ffs(mask); }

#define ntohs(x)        AROS_LE2WORD(x)

#endif /* SYS_PARAM_H */
