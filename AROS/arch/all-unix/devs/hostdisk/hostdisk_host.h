/*
 * avoid conflicts between our __unused define and the ones that might come in
 * via sys/stat.h
 */
#undef __unused

#ifdef HOST_LONG_ALIGNED
#pragma pack(4)
#endif

#define timeval sys_timeval

#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#undef timeval

#pragma pack()

typedef int file_t;

#define INVALID_HANDLE_VALUE -1

/* Android is not a true Linux ;-) */
#ifdef HOST_OS_android
#undef HOST_OS_linux
#endif

#if defined(HOST_OS_linux) || defined(HOST_OS_arix)
#define LIBC_NAME "libc.so.6"
#else
#endif

#ifdef HOST_OS_darwin
#define LIBC_NAME "libSystem.dylib"
#define DISK_DEVICE "/dev/disk%ld"
#define DISK_BASE   0
#endif

#ifndef LIBC_NAME
#define LIBC_NAME "libc.so"
#endif

#ifndef DISK_DEVICE
#define DISK_DEVICE "/dev/hd%lc"
#define DISK_BASE   'a'
#endif

/* AROS includes don't define struct stat64, this shuts up warning when compiling host-independent part */
struct stat64;

struct HostInterface
{
    int            (*open)(char *path, int oflag, ...);
    int            (*close)(int filedes);
    ssize_t        (*read)(int fildes, void *buf, size_t nbyte);
    ssize_t        (*write)(int fildes, const void *buf, size_t nbyte);
    int            (*ioctl)(int d, int request, ...);
    int            (*fsync)(int filedes);
#ifdef HOST_LONG_ALIGNED
    off_t          (*lseek)(int fildes, unsigned long offset_l, unsigned long offset_h, int whence);
#else
    off_t          (*lseek)(int fildes, off_t offset, int whence);
#endif
    int           *(*__error)(void);
#if defined(HOST_OS_linux) || defined(HOST_OS_arix)
    int            (*__fxstat64)(int ver, int fd, struct stat64 *buf);
    #define fstat64(fd, buf) __fxstat64(_STAT_VER, fd, buf)
#else
    int            (*fstat64)(int fd, struct stat64 *buf);
#endif
    int            (*stat64)(const char *path, struct stat64 *buf);

    /* For the AMP async IO we need some more clib functions here */
    int            (*sigprocmask)(int how, const sigset_t *set, sigset_t *oldset);
    int            (*sigsuspend)(const sigset_t *mask);
    int            (*sigaction)(int signum, const struct sigaction *act, struct sigaction *oldact);
    int            (*sigemptyset)(sigset_t *set);
    int            (*sigfillset)(sigset_t *set);
    int            (*sigaddset)(sigset_t *set, int signum);
    int            (*sigdelset)(sigset_t *set, int signum);
    int            (*clone)(int (*fn)(void *), void *child_stack, int flags, void *arg, ...);
    int            (*kill)(int pid, int sig);
    int            (*getppid)();
};

#ifdef HOST_LONG_ALIGNED
/*
 * Somewhat dirty hack to adjust data packing to iOS ARM ABI.
 * Perhaps this can be done in a cleaner and more CPU-abstract way.
 * FIXME: Always assuming little-endian CPU
 */
#define LSeek(fildes, offset, offset_high, whence) hdskBase->iface->lseek(fildes, offset, offset_high, whence)
#else
#define LSeek(fildes, offset, offset_high, whence) hdskBase->iface->lseek(fildes, (UQUAD)offset | (UQUAD)offset_high << 32, whence)
#endif

struct HostDiskBase;
struct DriveGeometry;

/* This routine is specific to a particular UNIX variant */
ULONG Host_DeviceGeometry(int file, struct DriveGeometry *dg, struct HostDiskBase *hdskBase);
