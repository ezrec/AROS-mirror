/*
 * Copyright (C) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 * Copyright (C) 2005 Neil Cafferkey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
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

#include <stdio.h>
/*#include <string.h>*/

#if 1 /* NC */
#include <signal.h>		/* from the C compilers includes */
#endif

#include <kern/amiga_includes.h>

#include <proto/dos.h>

#include <kern/amiga_time.h>
#include <api/amiga_api.h>
#include <kern/amiga_log.h>
/* #include <net/if_sana.h> */

/*#include <bsdsocket.library_rev.h>*/

ULONG sana_init(void);
void sana_deinit(void);
BOOL sana_poll(void);


#include <kern/amiga_main_protos.h>
#include <kern/amiga_config.h>
#include <kern/amiga_netdb.h>
#include <kern/kern_malloc_protos.h>

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
  sig = 0, signalmask = 0, timermask = 0, 
  breakmask = 0;

UBYTE *taskname = NULL;
/*BPTR db_lock = NULL;*/
ULONG EnableDebug = 0;
BOOL  initialized = FALSE;

TEXT interfaces_path[FILENAME_MAX];
TEXT netdb_path[FILENAME_MAX];
TEXT db_path[FILENAME_MAX];
TEXT config_path[FILENAME_MAX];
TEXT hequiv_path[FILENAME_MAX];
TEXT inetdconf_path[FILENAME_MAX];

STRPTR version = "$VER: bsdsocket 3.0 (13.12.2004)";

int
main(int argc, char *argv[])
{
  BYTE oldpri;
  STRPTR oldname;
  int retval;

#ifndef __AROS__
  SysBase = *(struct ExecBase **)4;
#endif

  /*
   * Disable CTRL-C(D) Break signal.
   */
  signal(SIGINT, SIG_IGN);

  /*
   * Initialize AmiTCP_Task to point the Task structure of this task.
   */
  AmiTCP_Task = FindTask(NULL);

  /*
   * Get a lock on the 'db' directory so we don't need an assign.
   */

  NameFromLock(ParentDir(GetProgramDir()), interfaces_path, FILENAME_MAX);
  strcpy(netdb_path, interfaces_path);
  strcpy(db_path, interfaces_path);
  strcpy(config_path, interfaces_path);
  strcpy(hequiv_path, interfaces_path);
  strcpy(inetdconf_path, interfaces_path);
  AddPart(interfaces_path, _PATH_SANA2CONFIG, FILENAME_MAX);
  AddPart(netdb_path, _PATH_NETDB, FILENAME_MAX);
  AddPart(db_path, _PATH_DB, FILENAME_MAX);
  AddPart(config_path, _PATH_CONFIG, FILENAME_MAX);
  AddPart(hequiv_path, _PATH_HEQUIV, FILENAME_MAX);
  AddPart(inetdconf_path, _PATH_INETDCONF, FILENAME_MAX);



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
    if (!taskname) {
#ifdef DEBUG
      if (nthLibrary) {
	if (taskname = bsd_malloc(16, M_CFGVAR, M_WAITOK)) {
	  strcpy(taskname, "bsdsocket.library");
	  taskname[6] = '.'; taskname[7] = '0' + nthLibrary;
	}
      } else {
#endif
	taskname = "bsdsocket.library";
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
    signalmask = timermask | breakmask | sanamask;

    /*
     * Now when everything else is successfully initialized,
     * let the timeouts roll!
     */
    timer_send();

    for(;;) {
      /*
       * Sleep until we are signalled.
       */
      sig = Wait(signalmask);

      do {
	if (sig & sanamask) {
	  if (!sana_poll())
	    sig &= ~sanamask;
	}

	if (sig & timermask) {
	  if (!timer_poll()) 
	    sig &= ~timermask;
	}

	sig |= SetSignal(0L, signalmask) & signalmask;
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
   * Free memory pool.
   */
  malloc_deinit();

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
