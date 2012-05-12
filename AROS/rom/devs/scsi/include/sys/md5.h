/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_MD5_H
#define SYS_MD5_H

#include <exec/types.h>

/* Data structure for MD5 (Message-Digest) computation */
typedef struct {
    ULONG buf[4];         /* scratch buffer */
    ULONG i[2];           /* number of _bits_ handled mod 2^64 */
    unsigned char in[64]; /* input buffer */
} MD5_CTX;

void MD5Init(MD5_CTX *mdContext);
void MD5Update(MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
void MD5Final(unsigned char digest[16], MD5_CTX *mdContext);


#endif /* SYS_MD5_H */
