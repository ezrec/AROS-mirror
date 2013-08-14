#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#include <aros/config.h>
#include <utility/hooks.h>

/* Android is not a true Linux ;-) */
#ifdef HOST_OS_android
#undef HOST_OS_linux
#endif

#ifdef HOST_OS_linux
#define LIBC_NAME "libc.so.6"
#define LIBPTHREAD_NAME "libpthread.so.0"
#endif

#ifdef HOST_OS_darwin
#define LIBC_NAME "libSystem.dylib"
#endif

#ifndef LIBC_NAME
#define LIBC_NAME "libc.so"
#endif

/* 
 * On Darwin sigset manipulation functions are redefined as macros, so they are slightly renamed here.
 * However they still present in libc as normal functions.
 */
struct KernelInterface
{
    int     (*raise)(int sig);
    int     (*sigprocmask)(int how, const sigset_t *set, sigset_t *oldset);
    int     (*sigsuspend)(const sigset_t *mask);
    int     (*sigaction)(int signum, const struct sigaction *act, struct sigaction *oldact);
    int     (*mprotect)(const void *addr, size_t len, int prot);
    ssize_t (*read)(int fd, void *buf, size_t count);
    int	    (*fcntl)(int fd, int cmd, ...);
    void *  (*mmap)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    int     (*munmap)(void *addr, size_t length);
    int    *(*__error)(void);
    int     (*sigwait)(const sigset_t *restrict set, int *restrict sig);
#ifndef HOST_OS_android
    int     (*SigEmptySet)(sigset_t *set);
    int     (*SigFillSet)(sigset_t *set);
    int     (*SigAddSet)(sigset_t *set, int signum);
    int     (*SigDelSet)(sigset_t *set, int signum);
#endif
#if AROS_SMP
    int     (*pthread_create)(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
    pthread_t (*pthread_self)(void);
    int     (*pthread_kill)(pthread_t thread, int sig);
    int     (*pthread_cond_init)(pthread_cond_t *cond, const pthread_condattr_t *attr);
    int     (*pthread_cond_broadcast)(pthread_cond_t *cond);
    int     (*pthread_cond_signal)(pthread_cond_t *cond);
    int     (*pthread_cond_wait)(pthread_cond_t *cond, pthread_mutex_t *mutex);
    int     (*pthread_mutex_init)(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    int     (*pthread_mutex_lock)(pthread_mutex_t *mutex);
    int     (*pthread_mutex_unlock)(pthread_mutex_t *mutex);
    int     (*pthread_key_create)(pthread_key_t *key, void (*destructor)(void *));
    int     (*pthread_setspecific)(pthread_key_t key, const void *value);
    void *  (*pthread_getspecific)(pthread_key_t key);
    ssize_t (*write)(int fd, const void *buff, size_t count);
    int     (*pipe2)(int fd[2], int flags);
#endif
};

/*
 * Android's Bionic doesn't have these functions.
 * They are simply inlined in headers.
 */
#ifdef HOST_OS_android
#define SIGEMPTYSET sigemptyset
#define SIGFILLSET  sigfillset
#define SIGADDSET   sigaddset
#define SIGDELSET   sigdelset
#else
#define SIGEMPTYSET(x)   pd->iface->SigEmptySet(x); AROS_HOST_BARRIER
#define SIGFILLSET(x)    pd->iface->SigFillSet(x); AROS_HOST_BARRIER
#define SIGADDSET(x,s)   pd->iface->SigAddSet(x,s); AROS_HOST_BARRIER
#define SIGDELSET(x,s)   pd->iface->SigDelSet(x,s); AROS_HOST_BARRIER
#endif

struct PlatformData
{
    int			   *errnoPtr;
    unsigned int         threads;
    sigset_t sig_int_mask;
    BOOL irq_enable;
    struct KrnUnixThread {
        pthread_t tid;
        APTR storage;        /* per-CPU storage */
        struct Hook *hook;   /* Hook for entry point */
        APTR message;
        unsigned int SupervisorCount;
        int signal[2];
        enum { STATE_STOPPED = 0, STATE_IDLE = -1, STATE_RUNNING = 1 } state;
        pthread_cond_t state_cond;
    } *thread;
    pthread_mutex_t forbid_mutex;
    int forbid_cpu;
    pthread_key_t key_cpu;
    pthread_key_t key_storage;
    struct KernelInterface *iface;
};

struct SignalTranslation
{
    short sig;
    short AmigaTrap;
    short CPUTrap;
};

extern struct SignalTranslation const sigs[];

void cpu_DispatchContext(struct Task *task, regs_t *regs, struct PlatformData *pdata);


