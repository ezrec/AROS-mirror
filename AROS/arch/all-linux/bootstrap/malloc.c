/*
 * AROS-thread-safe versions of libc memory allocation routines
 * This does not work with Android's bionic.
 */

#ifndef __ANDROID__

#include <proto/exec.h>
#include <sys/types.h>

static int memnest;
#ifndef __i386__
#define MEMLOCK if (SysBase != NULL) Forbid();
#define MEMUNLOCK if (SysBase != NULL) Permit();
#else
/* Hack on i386 to bring %%ebx in a proper state
   FIXME: Is there a better solution ?
*/
static char _lockebxmem[128];
static char *lockebxmem = _lockebxmem;
#define MEMLOCK \
    if (SysBase != NULL) \
    { \
        void *ebx; \
        asm volatile("movl %%ebx, %0; movl %1, %%ebx" : "=r"(ebx) : "m"(lockebxmem)); \
        Forbid(); \
        asm volatile("movl %0, %%ebx" : : "r"(ebx)); \
    }
#define MEMUNLOCK \
    if (SysBase != NULL) \
    { \
        void *ebx; \
        asm volatile("movl %%ebx, %0; movl %1, %%ebx" : "=r"(ebx) : "m"(lockebxmem)); \
        Permit(); \
        asm volatile("movl %0, %%ebx" : : "r"(ebx)); \
    }
#endif /* __i386__ */

extern struct ExecBase *SysBase;
extern void * __libc_malloc(size_t);
extern void __libc_free(void *);
extern void * __libc_calloc(size_t, size_t);
extern void * __libc_realloc(void * mem, size_t newsize);

void * malloc(size_t size)
{
    void *res;

    MEMLOCK
    memnest++;
    res = __libc_malloc(size);
    memnest--;
    MEMUNLOCK

    return res;
}

void free(void * addr)
{
    MEMLOCK
    memnest++;
    __libc_free(addr);
    memnest--;
    MEMUNLOCK
}

void * calloc(size_t n, size_t size)
{
    void *res;

    MEMLOCK
    memnest++;
    res = __libc_calloc(n, size);
    memnest--;
    MEMUNLOCK
    
    return res;
}

#endif
