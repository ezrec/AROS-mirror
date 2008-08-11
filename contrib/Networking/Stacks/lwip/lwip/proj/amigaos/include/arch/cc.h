/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id$
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#ifndef BYTE_ORDER
# ifndef __AROS__
#  define BYTE_ORDER BIG_ENDIAN
# else
#  if AROS_BIG_ENDIAN
#   define BYTE_ORDER BIG_ENDIAN
#  else
#   define BYTE_ORDER LITTLE_ENDIAN
#  endif /* AROS_BIG_ENDIAN */
# endif /* __AROS__ */
#endif /* BYTE_ORDER */

#ifndef __AROS__
void kprintf(const char *fmt,...);
#endif
void mykprintf(const char *fmt, ...);

typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
/*typedef unsigned   short   u8_t;
  typedef signed     short   s8_t; */
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

typedef u32_t mem_ptr_t;

#ifdef __AROS__
#define PACK_STRUCT_FIELD(x) x __attribute__((packed))
#define PACK_STRUCT_STRUCT __attribute__((packed))
#else
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#endif

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

/* prototypes for printf() and abort() */
#include <stdio.h>
#ifndef __AROS__
#include <stdlib.h>
#endif
/* Plaform specific diagnostic output */

#ifndef __AROS__
#define LWIP_PLATFORM_DIAG(x)	do {mykprintf x;} while(0)
#else
#include <aros/debug.h>
#define LWIP_PLATFORM_DIAG(x)	do {kprintf x;} while(0)
#endif

#define LWIP_PLATFORM_ASSERT(x) do {kprintf("Assertion \"%s\" failed at line %ld in %s\n", \
                                     x, __LINE__, __FILE__); /*fflush(NULL); abort();*/} while(0)

#define SYS_ARCH_DECL_PROTECT(x)
#define SYS_ARCH_PROTECT(x)
#define SYS_ARCH_UNPROTECT(x)

#endif /* __ARCH_CC_H__ */
