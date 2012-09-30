#define DEBUG 1
#include <aros/debug.h>
#include <exec/types.h>
#include <string.h>
#include <signal.h>

#include "hostdisk_host.h"
#include "host_thread.h"

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

/* The child process cloned from AROS */
int host_thread(struct ThreadData *td)
{
	struct HostInterface *iface = td->td_iface;
	volatile struct HostMMIO *mmio = td->td_mmio;

	int parent_pid;
	sigset_t sigs;
	int i;

	struct sigaction sa;

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
	parent_pid = iface->getppid();

	/*
	 * Kill'em! AROS' handler installed by hostdisk will know the child
	 * up and awaiting commands
	 */
	mmio->mmio_IRQ = 1;				// Yup! That's me!
	iface->kill(parent_pid, 12);	// Kill'em

	/* Endless loop starts here */
	do {
		/*
		 * If there is any work to do, AROS will set up virtual MMIO range
		 * and will kill the child process. Therefore, in order to save the
		 * cpu cycles we will wait here a while.
		 */
		iface->sigsuspend(&sigs);

		AROS_HOST_BARRIER

    	__sync_synchronize();

		/* Handle two probably "slow" commands */
		switch(mmio->mmio_Command)
		{
		case CMD_READ:
			/* Do the read */
			mmio->mmio_Ret = iface->read(mmio->mmio_File, mmio->mmio_Buffer, mmio->mmio_Size);
	    	AROS_HOST_BARRIER
	    	mmio->mmio_IRQ = 1;			// Set IRQ pending flag
			mmio->mmio_Command = 0;		// Not really needed
			__sync_synchronize();
			iface->kill(parent_pid, 12);// Kill AROS
	    	AROS_HOST_BARRIER
	    	break;

		case CMD_WRITE:
			/* Do the write */
			mmio->mmio_Ret = iface->write(mmio->mmio_File, mmio->mmio_Buffer, mmio->mmio_Size);
	    	AROS_HOST_BARRIER
	    	mmio->mmio_IRQ = 1;			// Set IRQ pending flag
			mmio->mmio_Command = 0;		// Not really needed
			__sync_synchronize();
			iface->kill(parent_pid, 12);// Kill AROS
	    	AROS_HOST_BARRIER
	    	break;

		case CMD_FLUSH:
			/* Do flush */
			mmio->mmio_Ret = iface->fsync(mmio->mmio_File);
	    	AROS_HOST_BARRIER
	    	mmio->mmio_IRQ = 1;			// Set IRQ pending flag
			mmio->mmio_Command = 0;		// Not really needed
			__sync_synchronize();
			iface->kill(parent_pid, 12);// Kill AROS
	    	AROS_HOST_BARRIER
	    	break;

		default:
			break;
		}

	} while(mmio->mmio_Command != -1);	// command = -1 means, get out of here!

	mmio->mmio_IRQ = 1;
	__sync_synchronize();

	iface->kill(parent_pid, 12);
	AROS_HOST_BARRIER

	return 0;
}
