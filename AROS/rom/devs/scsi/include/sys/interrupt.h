/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_INTERRUPT_H
#define SYS_INTERRUPT_H

#include <sys/serialize.h>

typedef void inthand2_t (void *, void *);

#define SWI_CAMBIO      0

void *register_swi(int intr, inthand2_t *handler, void *arg, const char *name, struct lwkt_serialize *serializer, int cpuid); 

#endif /* SYS_INTERRUPT_H */
