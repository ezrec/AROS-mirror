/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef LWIP_ARCH_CC_H
#define LWIP_ARCH_CC_H

#include <aros/debug.h>
#include <aros/system.h>

#include <exec/types.h>
#include <exec/alerts.h>

#include <stdlib.h> /* rand() */
#include <sys/time.h>

#define LWIP_TIMEVAL_PRIVATE            0

typedef BYTE s8_t;
typedef UBYTE u8_t;
typedef WORD s16_t;
typedef UWORD u16_t;
typedef LONG s32_t;
typedef ULONG u32_t;
typedef QUAD s64_t;
typedef UQUAD u64_t;

typedef IPTR mem_ptr_t;

#define U16_F "hu"
#define S16_F "d"
#define X16_F "hx"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "uz"

#define LITTLE_ENDIAN      1234
#define BIG_ENDIAN         4321

#if AROS_BIG_ENDIAN
#   define BYTE_ORDER  BIG_ENDIAN
#else
#   define BYTE_ORDER  LITTLE_ENDIAN
#endif

#define LWIP_CHKSUM_ALGORITHM   3   /* 32-bit alignment */

#define PACK_STRUCT_FIELD(x) x __attribute__((packed))
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#define LWIP_PLATFORM_DIAG(x)   bug(x)
#define LWIP_PLATFORM_ASSERT(x) do { bug(x); Alert(AT_DeadEnd | AG_IOError); } while (0)

#define LWIP_RAND(x)    rand()

#include "arch/compat.h"

#endif /* LWIP_ARCH_CC_H */
