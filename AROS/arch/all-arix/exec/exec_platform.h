#ifndef __EXEC_PLATFORM_H
#define __EXEC_PLATFORM_H

#include <sys/types.h>

#ifdef HOST_OS_android
/* Android is not a true Linux ;-) */
#undef HOST_OS_linux

#else

/* On Darwin this definition is required by ucontext.h (which is marked as deprecated) */
#define _XOPEN_SOURCE
#include <ucontext.h>

#endif

#if defined(HOST_OS_linux) || defined(HOST_OS_arix)
#define LIBC_NAME "libc.so.6"
#endif

#ifdef HOST_OS_darwin
#define LIBC_NAME "libSystem.dylib"
#endif

#ifndef LIBC_NAME
#define LIBC_NAME "libc.so"
#endif

struct LibCInterface
{
    void (*exit)(int status);
#ifdef HAVE_SWAPCONTEXT
    int  (*getcontext)(ucontext_t *ucp);
    void (*makecontext)(ucontext_t *ucp, void *func(), int argc, ...);
    int  (*swapcontext)(ucontext_t *oucp, ucontext_t *ucp);
#endif
    int     (*syscall)(int nr,...);
    void *  (*mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    int     (*munmap)(void *addr, size_t length);
    int     (*mincore)(void *addr, size_t length, unsigned char *vec);
    int     (*open)(const char *path, int flags);
    ssize_t (*read)(int fd, void *buf, size_t count);
    int     (*close)(int fd);
    int     (*mlock)(void *addr, size_t len);
    int     (*munlock)(void *addr, size_t len);

};

struct Exec_PlatformData
{
    APTR HostLibBase;
    struct LibCInterface *SysIFace;
};

#define HostLibBase PD(SysBase).HostLibBase

#endif /* __EXEC_PLATFORM_H */
