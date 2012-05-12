/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef MACHINE_STDARG_H
#define MACHINE_STDARG_H

#include <stdarg.h>

typedef va_list __va_list;
#define __va_start(a,f) va_start(a,f)
#define __va_end(a)     va_end(a)
#define __va_arg(a,t)   va_arg(a,t)

#endif /* MACHINE_STDARG_H */
