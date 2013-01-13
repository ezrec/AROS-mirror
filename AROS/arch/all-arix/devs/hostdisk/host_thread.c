#define DEBUG 1
#include <aros/debug.h>
#include <exec/types.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>

#include "hostdisk_host.h"
#include "host_thread.h"

#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>

/* Required to build with proper syscal numbers! Why!?? */
#if defined __i386__
#include <i386-linux-gnu/asm/unistd.h>
#elif defined __arm__
#include <arm-linux-gnueabihf/asm/unistd.h>
#endif

/* Empty signal handler which does pretty nothing */
#ifdef SA_SIGINFO
#undef sa_handler
void sa_handler(int sig, siginfo_t *si, void *data)
#else
void handler(int sig)
#endif
{
}


/* I *do not* want to use any AROS library in this piece of code! */
void clear(void *addr, int size)
{
    char *ptr = (char*)addr;
    while(size--)
        *ptr++=0;
}

#if 0
void do_kill(int pid, int sig)
{
    asm volatile("call *%%gs:0x10"::"a"(__NR_kill),"b"(pid),"c"(sig));
}

int do_read(int fd, void *buffer, int size)
{
    int retval;
    asm volatile("call *%%gs:0x10":"=a"(retval):"0"(__NR_read),"b"(fd),"c"(buffer),"d"(size));
    return retval;
}

int do_write(int fd, void *buffer, int size)
{
    int retval;
    asm volatile("call *%%gs:0x10":"=a"(retval):"0"(__NR_write),"b"(fd),"c"(buffer),"d"(size));
    return retval;
}

int do_fsync(int fd)
{
    int retval;
    asm volatile("call *%%gs:0x10":"=a"(retval):"0"(__NR_fsync),"b"(fd));
    return retval;
}

int do_sigsuspend(sigset_t *sigs)
{
    int retval;
    asm volatile("call *%%gs:0x10":"=a"(retval):"0"(__NR_rt_sigsuspend),"b"(sigs),"c"(8));
    return retval;
}

int do_getppid()
{
    int retval;
    asm volatile("call *%%gs:0x10":"=a"(retval):"0"(__NR_getppid));
    return retval;
}

#endif


/* The child process cloned from AROS */
int host_thread(struct ThreadData *td)
{
    struct HostInterface *iface = td->td_iface;
    volatile struct HostMMIO *mmio = td->td_mmio;

    int parent_pid;
    sigset_t sigs;
    int i;

    struct sigaction sa;

    /* Suicide, when AROS dies */
    iface->syscall(SYS_prctl, PR_SET_PDEATHSIG, SIGKILL, 0, 0, 0);

    /*
     * The AROS process is cloned in disabled state. Nevertheless, mask all
     * signals again.
     */
    iface->sigfillset(&sigs);
    iface->sigprocmask(2, &sigs, NULL);

    AROS_HOST_BARRIER

    /*
     * Set up empty signal handler. Without it, sending a signal to this process
     * might kill the init (pid=1) :)
     */
    clear(&sa, sizeof(sa));
#ifdef SA_SIGINFO
    sa.sa_sigaction = sa_handler;
#else
    sa.sa_handler = handler;
#endif

    for (i=0; i < 32; i++)
        iface->sigaction(i, &sa, NULL);

    /*
     * Enable SIGUSR2 only
     */
    iface->sigfillset(&sigs);
    iface->sigdelset(&sigs, 12);

    AROS_HOST_BARRIER

    /*
     * Find out the PID of parent (that is, AROS process)
     */
    parent_pid = iface->syscall(SYS_getppid);

    /*
     * AROS handler installed by hostdisk will know the child
     * up and awaiting commands
     */
    mmio->mmio_IRQ = 1;				// Yup! That's me!
    SignalParent(parent_pid);

    /* Endless loop starts here */
    do {
        /*
         * If there is any work to do, AROS will set up virtual MMIO range
         * and will signal the child process. Therefore, in order to save the
         * cpu cycles we will wait here a while.
         */
        iface->syscall(SYS_rt_sigsuspend, &sigs, _NSIG/8);

        AROS_HOST_BARRIER

        __sync_synchronize();

        /* Handle two probably "slow" commands */
        switch(mmio->mmio_Command)
        {
            case CMD_READ:
                /* Do the read */
                mmio->mmio_Ret = iface->syscall(SYS_read, mmio->mmio_File, mmio->mmio_Buffer, mmio->mmio_Size);
                AROS_HOST_BARRIER
                mmio->mmio_IRQ = 1;			// Set IRQ pending flag
                mmio->mmio_Command = 0;		// Not really needed
                __sync_synchronize();

                SignalParent(parent_pid);
                AROS_HOST_BARRIER
                break;

            case CMD_WRITE:
                /* Do the write */
                mmio->mmio_Ret = iface->syscall(SYS_write, mmio->mmio_File, mmio->mmio_Buffer, mmio->mmio_Size);
                AROS_HOST_BARRIER
                mmio->mmio_IRQ = 1;			// Set IRQ pending flag
                mmio->mmio_Command = 0;		// Not really needed
                __sync_synchronize();

                SignalParent(parent_pid);
                AROS_HOST_BARRIER
                break;

            case CMD_FLUSH:
                /* Do flush */
                mmio->mmio_Ret = iface->syscall(SYS_fsync, mmio->mmio_File);
                AROS_HOST_BARRIER
                mmio->mmio_IRQ = 1;			// Set IRQ pending flag
                mmio->mmio_Command = 0;		// Not really needed
                __sync_synchronize();

                SignalParent(parent_pid);
                AROS_HOST_BARRIER
                break;

            default:
                break;
        }

    } while(mmio->mmio_Command != -1);	// command = -1 means, get out of here!

    mmio->mmio_IRQ = 1;
    __sync_synchronize();

    SignalParent(parent_pid);
    AROS_HOST_BARRIER

    return 0;
}
