/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_SYSTM_H
#define SYS_SYSTM_H

#include <string.h>
#include <stdint.h>

#include <proto/exec.h>
#include <exec/rawfmt.h>
#include <exec/alerts.h>

#include <sys/conf.h>
#include <sys/lock.h>
#include <sys/libkern.h>
#include <machine/stdarg.h>
#include <machine/atomic.h>

int ksnprintf(char *buff, size_t len, const char *fmt, ...);
int kvprintf(const char *fmt, __va_list args);
int kprintf(const char *fmt, ...);
int ksprintf(char *buf, const char *fmt, ...);
int kvsnprintf(char *, size_t, const char *, __va_list);
int kvcprintf (char const *, void (*)(int, void*), void *, int, __va_list);
int kvasnrprintf(char **, size_t, int, const char *, __va_list);
int kvsnrprintf (char *, size_t, int, const char *, __va_list);

#define panic(fmt,args...) do { kprintf(fmt, ##args); Alert(AT_DeadEnd); } while (0)


static inline int copyin(const void *udaddr, void *kaddr, size_t len)
{
    CopyMem(udaddr, kaddr, len);
    return 0;
}

static inline int copyout(const void *kaddr, void *udaddr, size_t len)
{
    CopyMem(kaddr, udaddr, len);
    return 0;
}

#ifdef DEBUG
#define bootverbose     DEBUG
#else
#define bootverbose     0
#endif

#undef KASSERT

#define KASSERT(exp,msg) do { if (exp) panic msg; } while (0)
#define KKASSERT(exp)    KASSERT(exp, ("%s: Panic at %s:%d\n", __func__, __FILE__, __LINE__))

void DELAY(int usec);

#define PINTERLOCKED    0x00000400      /* interlocked tsleep */

typedef void timeout_t (void *);

void wakeup (const volatile void *chan);
void tsleep_interlock (const volatile void *, int);
int  tsleep (const volatile void *, int, const char *, int);
int  ssleep (const volatile void *, struct spinlock *, int, const char *, int);

char *kgetenv (const char *name);
int  lksleep (const volatile void *, struct lock *, int, const char *, int);

static inline void setsoftcambio (void) { }

int minor(cdev_t cdev);

#define securelevel     0

#endif /* SYS_SYSTM_H */
