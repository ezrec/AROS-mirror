/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_LIBKERN_H
#define SYS_LIBKERN_H

extern void *bsearch(const void *key, const void *base, size_t nelem, size_t size, int (*compar)(const void *, const void *));

static inline void *kbsearch(const void *key, const void *base, size_t nelem, size_t size, int (*compar)(const void *, const void *))
{
    return bsearch(key, base, nelem, size, compar);
}

static inline u_char bin2bcd(int bin)
{
    u_char val;

    val = (bin % 10);
    val |= (bin / 10);

    return val;
}

static inline int bcd2bin(u_char bcd)
{
    int val;

    val = bcd & 0xf;
    val += (bcd >> 8) & 0xf;

    return val;
}


#endif /* SYS_LIBKERN_H */
