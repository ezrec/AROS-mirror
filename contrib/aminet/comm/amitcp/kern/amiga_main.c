/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>,
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * 
 * $Log$
 * Revision 1.1  2001/12/25 22:28:13  henrik
 * amitcp
 *
 * Revision 3.2  1994/04/02  10:28:28  jraja
 * Removed res_init(), which is done for each SocketBase now.
 * Raised version to beta2.
 *
 * Revision 3.1  1994/03/26  09:45:33  too
 * Added netdb_deinit() call. Raised revision major number to 3.
 *
 * Revision 1.45  1994/01/23  22:33:35  jraja
 * Raised version number to 3.0s.
 *
 * Revision 1.44  1994/01/05  10:25:00  jraja
 * Cosmetic changes.
 *
 * Revision 1.43  1993/12/22  08:49:31  jraja
 * Changed CTRL-C code to try to break applications upto 3 times if necessary.
 *
 * Revision 1.42  1993/12/20  18:02:27  jraja
 * Added include for dos protos&pragmas.
 *
 * Revision 1.41  1993/12/20  08:22:37  jraja
 * Raised revision number to 2.3.
 *
 * Revision 1.40  1993/11/26  16:21:51  too
 * Added task signalling and delaying after receiving break signal
 *
 * Revision 1.39  1993/11/07  00:04:44  ppessi
 * Raised release number to 2.2.
 *
 * Revision 1.38  1993/10/29  02:18:26  ppessi
 * Raised release number to 2.1a.
 *
 * Revision 1.37  1993/10/11  01:43:56  jraja
 * Raised release number to 2.1.
 *
 * Revision 1.36  1993/08/10  16:30:19  too
 * Added version DATE from bumprevved bsdsocket.library_rev.h
 *
 * Revision 1.35  1993/06/04  11:16:15  jraja
 * Fixes for first public release.
 *
 * Revision 1.1  93/02/04  18:55:53  18:55:53  jraja (Jarno Tapio Rajahalme)
 * Initial revision
 * 
 */

#include <conf.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/synch.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/syslog.h>
#include <sys/socket.h>
#include <sys/socketvar.h>

#include <signal.h>		/* from the C compilers includes */

#include <kern/amiga_includes.h>

#if __SASC
#include <proto/dos.h>
#elif __GNUC__
#include <proto/dos.h>
#else
#error Compiler not supported!
#endif

#include <kern/amiga_time.h>
#include <api/amiga_api.h>
#include <kern/amiga_log.h>
/* #include <net/if_sana.h> */

#include <bsdsocket.library_rev.h>

ULONG sana_init(void);
void sana_deinit(void);
BOOL sana_poll(void);


#include <kern/amiga_main_protos.h>
#include <kern/amiga_config.h>
#include <kern/amiga_netdb.h>
/* #include <kern/kern_synch_protos.h>*/
/*
 * include prototypes for initialization functions
 */
#include <kern/uipc_domain_protos.h>    /* domaininit() */

/*
 * The main module of the AMITCP/IP.
 */

/*
 * Global variable so AMITCP/IP task information can be utilized.
 */
struct Task * AmiTCP_Task;

extern struct ExecBase * SysBase;
extern struct Library * MasterSocketBase;
extern WORD nthLibrary;

static ULONG sanamask = 0, 
  sig = 0, sigmask = 0, timermask = 0, 
  breakmask = 0;

UBYTE *taskname = NULL;
ULONG EnableDebug = 0;
BOOL  initialized = FALSE;

STRPTR version = "\0$VER: AmiTCP/IP 3.0\337" "2 (" DATE ")\r\n"
             "Copyright \251 1993-1994 AmiTCP/IP Group.\r\n";

int
main(int argc, char *argv[])
{
  BYTE oldpri;
  STRPTR oldname;
  int retval;

  SysBase = *(struct ExecBase **)4;

  /*
   * Disable CTRL-C(D) Break signal.
   */
  signal(SIGINT, SIG_IGN);

  /*
   * Initialize AmiTCP_Task to point the Task structure of this task.
   */
  AmiTCP_Task = FindTask(NULL);

  /*
   * Save pointer to this tasks old name
   */
  oldname = AmiTCP_Task->tc_Node.ln_Name;

  if (init_all()) {
    /*
     * Set our priority 
     */
    oldpri = SetTaskPri(AmiTCP_Task, 5);

    /*
     * Set our Task name 
     */
    if (!taskname) 
      {
#ifdef DEBUG
      if (nthLibrary) 
	{
	  if ( (taskname = bsd_malloc(16, M_CFGVAR, M_WAITOK)) )
	    {
	      strcpy(taskname, "AmiTCP");
	      taskname[6] = '.'; taskname[7] = '0' + nthLibrary;
	    }
	} 
      else 
	{
#endif
	  taskname = "AmiTCP";
#ifdef DEBUG
	}
#endif
    }
    if (taskname)
      AmiTCP_Task->tc_Node.ln_Name = taskname;

    /* 
     * Global initialization flag;
     */
    initialized = TRUE;

#ifdef DEBUG
    /* 
     * Show our task address
     */
    printf("%s Task address : %lx\n", taskname, (long) AmiTCP_Task);
#endif

    /*
     * Initialize signal mask for the wait
     */
    breakmask = SIGBREAKF_CTRL_C;
    sigmask = timermask | breakmask | sanamask;

    /*
     * Now when everything else is succesfully initialized,
     * let the timeouts roll!
     */
    timer_send();

    for(;;) {
      /*
       * Sleep until we are signalled.
       */
      sig = Wait(sigmask);

      do {
	if (sig & sanamask) {
	  if (!sana_poll())
	    sig &= ~sanamask;
	}

	if (sig & timermask) {
	  if (!timer_poll()) 
	    sig &= ~timermask;
	}

	sig |= SetSignal(0L, sigmask) & sigmask;
      } while (sig && sig != breakmask);

      if (sig & breakmask) {
	int i;
	/*
	 * We got CTRL-C
	 *
	 * NETTRACE task keeps one base open, it is not counted.
	 */
	api_hide();		/* hides the API from users */

	/*
	 * Try three times with a short delay
	 */
	for (i = 0; i < 3 && MasterSocketBase->lib_OpenCnt > 1; i++) {
	  api_sendbreaktotasks(); /* send brk to all tasks w/ SBase open */ 
	  Delay(50);		  /* give tasks time to close socket base */
	}
	if (MasterSocketBase->lib_OpenCnt > 1) {
	  log(LOG_ERR, "Got CTRL-C while %ld %s still open.\n",
	      MasterSocketBase->lib_OpenCnt - 1,
	      (MasterSocketBase->lib_OpenCnt == 2) ? "library" : "libraries");
	  api_show(); /* stopping not successfull, show API to users */ 
	} else {
	  break;
	}
      }
    }
    retval = 0;
  } else
    retval = 20;

  /*
   * free all resources
   */
  deinit_all();
  initialized = FALSE;

  SetTaskPri(AmiTCP_Task, oldpri);
  AmiTCP_Task->tc_Node.ln_Name = oldname;

  return retval;
}

/*
 * Do all initializations
 */
BOOL
init_all(void)
{
  /*
   * Initialize malloc semaphore
   */
  malloc_init();

  /*
   * initialize concurrency control subsystem
   */  
  spl_init();
  
  /*
   * initialize sleep queues
   */
  sleep_init();
  
  /* 
   * Read command line arguments and configuration file
   */
  if (!readconfig())
    return FALSE;

  /*
   * initialize logging system
   */
  if (!log_init())
    return FALSE;

  /* 
   * initialize the mbuf subsystem
   */
  if (!mbinit())
    return FALSE;

  /*
   * initialize timer
   */
  if ((timermask = timer_init()) == 0L)
    return FALSE;

  /*
   * initialize API
   */
  if (!api_init())
    return FALSE;
	
  /*
   * initialize SANA-II subsystem
   */
  if ((sanamask = sana_init()) == 0L)
    return FALSE;
	    
  /*
   * initialize domains (initializes all protocols)
   */
  domaininit();
	    
  /*
   * Initialize NetDataBase
   */
  if (init_netdb() != 0)
    return FALSE;

  /*
   * Make API visible
   */
  if (api_show() == FALSE)
    return FALSE;

  if (Nettrace_Task)
    Signal(Nettrace_Task, SIGBREAKF_CTRL_F);
  else
    return FALSE;

  return TRUE;
}

/*
 * clean up everything
 */
void
deinit_all(void)
{
  /*
   * make sure we are out of critical section
   */
  spl0();

  api_hide();			/* hides the API from users */

  /*
   * Deinitialize network database.
   */
  netdb_deinit();
  
  /*
   * Deinitialize network interfaces
   */
  sana_deinit();

  /*
   * Deinitialize timers
   */
  timer_deinit();

  /*
   * Free all resources allocated by mbufs.
   */
  mbdeinit();

  log_deinit();

  /*
   * Check that there are no libraries open (to our API). We can continue only
   * if all bases are closed.
   */
  api_deinit();  /* NOTICE: this waits until every api user has exited */
}

/*
 * Notification function for taskname
 */ 
int taskname_changed(void *p, LONG new)
{
  UBYTE *newname = (UBYTE *)new;
  
  AmiTCP_Task->tc_Node.ln_Name = newname;
  if (initialized)
    printf("New task name %s\n", newname);

  return TRUE;
}
