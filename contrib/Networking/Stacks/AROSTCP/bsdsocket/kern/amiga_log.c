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

/*#include <bsdsocket.library_rev.h>*/

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>

#include <exec/exec.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/dosextens.h>

#include <kern/amiga_includes.h>
#include <sys/syslog.h>
#include <kern/amiga_log.h>
#include <kern/amiga_rexx.h>
#include <utility/date.h>

#include <proto/dos.h>
#include <proto/utility.h>
#define DOSBase logDOSBase
#define UtilityBase logUtilityBase
#undef DOSBase

struct MsgPort *logPort,*logReplyPort;

static struct Library *logDOSBase = NULL;
static struct Library *logUtilityBase = NULL;

extern struct Task *AmiTCP_Task;
extern struct DosLibrary *DOSBase;

extern void REGARGFUN stuffchar();
extern int logname_changed(void *p, LONG new);

static struct log_msg *log_poll(void);
static void log_task(void);
static void log_close(struct log_msg *msg);
static BPTR logOpen(STRPTR name);

static struct log_msg *log_messages = NULL;
static char *log_buffers = NULL;
static LONG log_messages_mem_size, log_buffers_mem_size;
static int GetLogMsgFail;


UBYTE consoledefname[] = _PATH_CON;
UBYTE logfiledefname[] = _PATH_LOG;
STRPTR logfilename = logfiledefname;
STRPTR consolename = consoledefname;


struct log_cnf log_cnf = { LOG_BUFS, LOG_BUF_LEN };

#define SOCKET_VERSION 3

/*
 * Initialization function for the logging subsystem
 */

BOOL 
log_init(void)
{
  struct Message *msg;
  int i;
  ULONG sig;

  if (logReplyPort)
    return(TRUE);		/* We're allready initialized */
  
  /*
   * Allocate buffers for log messages.
   * 
   * Save lengths to static variables, since the configuration variables might
   * change.
   */
  log_messages_mem_size = sizeof(struct log_msg) * log_cnf.log_bufs;
  log_buffers_mem_size = log_cnf.log_bufs * log_cnf.log_buf_len * sizeof(char);
  if (log_messages = AllocMem(log_messages_mem_size, MEMF_CLEAR|MEMF_PUBLIC))
    if (log_buffers = AllocMem(log_buffers_mem_size, MEMF_CLEAR|MEMF_PUBLIC)) {
      logPort = NULL; /* NETTRACE will set this on success */
      GetLogMsgFail = 0;

      if (logReplyPort = CreateMsgPort()) {
	/*
	 * Start the NETTRACE process
	 */
	if (CreateNewProcTags(NP_Entry, (LONG)&log_task,
			      NP_Name, (LONG)LOG_TASK_NAME,
			      NP_Priority, LOG_TASK_PRI,
			      TAG_DONE, NULL)) {
	  for (;;) {
	    /*
	     * Wait for a signal for success or failure
	     */
	    sig = Wait(1<<logReplyPort->mp_SigBit | SIGBREAKF_CTRL_F);

	    if (sig & SIGBREAKF_CTRL_F && logPort == (struct MsgPort *)-1) {
	      /* Initializion failed */
	      logPort = NULL;
	      break;
	    }
	    else if (msg = GetMsg(logReplyPort)) { /* Got message back? */
	      ReplyMsg(msg);
	      logReplyPort->mp_Flags = PA_IGNORE;
	      /* 
	       * Initialize messages
	       */
	      for (i = 0; i < log_cnf.log_bufs; i++) {
		log_messages[i].msg.mn_ReplyPort = logReplyPort;
		log_messages[i].msg.mn_Length = sizeof(struct log_msg);
		log_messages[i].level = 0;
		log_messages[i].string = log_buffers+i*log_cnf.log_buf_len;
		log_messages[i].chars = 0;
		PutMsg(logReplyPort, (struct Message *)&log_messages[i]);
	      }
	      return(TRUE);	/* We're done */
	    }
	  }
	}
      }
    }
  /*
   * Something went wrong
   */
  log_deinit();
  return(FALSE);
}

/*
 * This function may be called only if no other tasks (applications) are 
 * accessing the logging system (the messages, to be exact).
 */
void 
log_deinit(void)
{
  struct log_msg *msg, *dump;

  if (logReplyPort) {		/* We have our port? */
    if (logPort) {		/* Logport exists? (=> NETTRACE is running) */
      /*
       * Turn on signalling on returned messages again
       */
      logReplyPort->mp_Flags = PA_SIGNAL;

      /*
       * Get a free message, Wait() for it if necessary
       */
      while((msg = (struct log_msg *)GetMsg(logReplyPort)) == NULL) 
	Wait(1<<logReplyPort->mp_SigBit);

      /* 
       * Initalize END_MESSAGE
       */
      msg->msg.mn_ReplyPort = logReplyPort;
      msg->msg.mn_Length = sizeof(struct log_msg);
      msg->level = LOG_CLOSE;

      PutMsg(logPort, (struct Message *)msg);
      
      for (;;) {
	dump = (struct log_msg *)GetMsg(logReplyPort);
	if (dump) {
	  if (dump->level == LOG_CLOSE)	/* got the Close message back */
	    break;		/* It was the last one */
	}	      
	else
	  Wait(1<<logReplyPort->mp_SigBit);
      }
    }

    /*
     * ensure that the port is empty
     */
    while(GetMsg(logReplyPort))
      ;
    DeleteMsgPort(logReplyPort);
    logReplyPort = NULL;
  }
  if (log_buffers) {
    FreeMem(log_buffers, log_buffers_mem_size);
    log_buffers = NULL;
  }
  if (log_messages) {
    FreeMem(log_messages, log_messages_mem_size);
    log_messages = NULL;
  }
}

/* A little stub for calling GetMsg w/ error reporting and cheking */
struct log_msg *GetLogMsg(struct MsgPort *port)
{
  struct Message *msg;

  if (port && (msg = GetMsg(port)))  /* Get a message */
    /* We should have a port, if not-> fail */
    return (struct log_msg *)msg; 

  ++GetLogMsgFail;		/* Increment number of failed messages */
  return NULL;
}

/*
 * Functions following these defines may be called from the NETTRACE
 * task ONLY! These defines cause the SAS/C to generate calls to
 * dos.library and utility.library using these bases, respectively.
 */
#define DOSBase logDOSBase
#define UtilityBase logUtilityBase

struct Library *SocketBase = NULL;
struct Task *Nettrace_Task = NULL;

/* Main loop for NETTRACE */
static void SAVEDS
log_task(void)
{
  struct log_msg *initmsg = NULL;
  ULONG rexxmask = 0, logmask = 0, sigmask = 0;

  Nettrace_Task = FindTask(NULL); /* Store task pointer for AmiTCP */

  /* We need our own DosBase */
  if ((logDOSBase = OpenLibrary(DOSNAME, 0L)) == NULL)
    goto fail;

  if ((logUtilityBase = OpenLibrary("utility.library", 37L)) == NULL)
    goto fail;

  /* Allocate message to reply startup */
  if ((initmsg = AllocMem(sizeof(struct log_msg), MEMF_CLEAR|MEMF_PUBLIC))
      == NULL)
    goto fail;

  /* Create our port for log messages */
  if ((logPort = CreateMsgPort()) == NULL)
    goto fail;

  logmask = 1<<logPort->mp_SigBit;

  /*
   * Initialize rexx subsystem
   */
  if (!(rexxmask = rexx_init()))
    goto fail;

  /*
   * Syncronize with AmiTCP/IP
   */
  initmsg->msg.mn_ReplyPort = logPort;
  initmsg->msg.mn_Length = sizeof(struct log_msg);
  PutMsg(logReplyPort, (struct Message *)initmsg);
  do {
    Wait(logmask);
  } while(initmsg != (struct log_msg *)GetMsg(logPort));

  FreeMem(initmsg, sizeof(struct log_msg));
  initmsg = NULL;

  sigmask = logmask | rexxmask | SIGBREAKF_CTRL_F;
  
  /* 
   * Main loop of the NETTRACE
   */
  for (;;) {
    ULONG sig;
    struct log_msg *msg;

    sig = Wait(sigmask);	/* Wait for signals */
    do {
				/* Signal from the AmiTCP/IP: API ready */
      if ((sig & SIGBREAKF_CTRL_F) && (SocketBase == NULL)) {
	sig &= ~SIGBREAKF_CTRL_F;
	/*
	 * Open a base to our own library so that ARexx message handling
	 * can use socket functions.
	 * This name does not work with the "nthLibrary" system
	 */
	if (SocketBase = OpenLibrary("bsdsocket.library", SOCKET_VERSION)) {
	  /*
	   * Make our ARexx port public
	   */
	  rexx_show();
	  sigmask &= ~SIGBREAKF_CTRL_F;
/*SocketBase = NULL;*/
	}
      }

      if (sig & logmask) {	/* Process log messages,
				 * handles all ones pending.
				 */
	if (msg = log_poll()) {	/* Got an LOG_CLOSE-message? */
	  log_close(msg);
	  return;
	}
	sig &= ~logmask;
      }

      if (sig & rexxmask)	/* One rexx message at time */
	if (SocketBase) {
	  if (!rexx_poll()) 
	    sig &= ~rexxmask;
	} else
	  sig &= ~rexxmask;

      sig |= SetSignal(0L, sigmask) & sigmask; /* Signals left? */
    } while (sig);
  }

 fail:				
  /* Initializion Failed */
  if (initmsg)
    FreeMem(initmsg, sizeof(struct log_msg));

  log_close(NULL);

  logPort = (struct MsgPort *)-1;
  /* Inform AmiTCP that we failed */
  Signal(AmiTCP_Task, SIGBREAKF_CTRL_F); 
}

static BPTR confile = NULL;
static BPTR logfile = NULL;
static BOOL fileopenfail = FALSE, conopenfail = FALSE;
static BOOL filewritefail = FALSE, conwritefail = FALSE;

static char *months =
  "Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec";

static char *wdays = 
  "Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat";

static char *levels = 
  "emerg\0"
  "alert\0"
  "crit \0"
  "err  \0"
  "warn \0"
  "note \0"
  "info \0"
  "debug";

/* 
 * Process all pending log messages 
 */
static
struct log_msg *log_poll()
{     
  struct log_msg *msg;
  ULONG where;
  LONG i;
  struct ClockData clockdata;

  /* 28 for date, 14 for level */
# define LEVELBUF 28+14

  UBYTE buffer[LEVELBUF];
  static ULONG TotalFail;

  /* Process all messages */
  while (msg = (struct log_msg *)GetMsg(logPort)) { 
    struct CSource cs;

    /* All except LOG_EMERG to both log and console */
    where = TOLOG;
    if (msg->level != LOG_EMERG)
      where |= TOCONS;

    if (msg->level == LOG_CLOSE) {
      return (msg);
    }

    cs.CS_Buffer = buffer;
    cs.CS_Length = LEVELBUF;
    cs.CS_CurChr = 0;

    Amiga2Date(msg->time, &clockdata);

    csprintf(&cs, 
#ifdef HAVE_TIMEZONES
	     "%s %s %02d %02d:%02d:%02d %s %4d [%s]: ", 
#else
	     "%s %s %02d %02d:%02d:%02d %4d [%s]: ", 
#endif
	     wdays + 4 * clockdata.wday,
	     months + 4 * (clockdata.month - 1),
	     clockdata.mday,
	     clockdata.hour,
	     clockdata.min,
	     clockdata.sec,
#ifdef HAVE_TIMEZONES
	     "UCT",	/* Universal Coordinated Time */
#endif
	     clockdata.year,
	     levels + 6 * ((msg->level <= LOG_DEBUG) ? msg->level : LOG_DEBUG)
	     );

    /* Remove last newline */
    if (msg->chars > 0 && msg->string[msg->chars - 1] == '\n') {
      msg->chars--;
    }

    /* Replace all control chars with space */
    for (i = 0; i < msg->chars; ++i) {
      if ((msg->string)[i] < ' ')
	(msg->string)[i] = ' ';
    }

    if (where & TOCONS) {
      /* If console is not open, open it */
      while (confile == NULL) {
	if ((confile = logOpen(consolename)) == NULL) {
	  if (!conopenfail) /* log only once */
	    log(LOG_ERR,"Opening console log '%s' failed", consolename);
	  if (consolename == consoledefname) {
	    conopenfail = TRUE;	    
	    break;
	  }
	  /* try again with the default name */
	  consolename = consoledefname;
	  conopenfail = conwritefail = FALSE;
	}
      }
      if (confile != NULL) {
	int error = 
	  FPuts(confile, buffer) == -1 ||
	    FWrite(confile, msg->string, msg->chars, 1) != 1 ||
	      FPutC(confile, '\n') == -1;
	Flush(confile);
	if (error && !conwritefail) {	/* To avoid loops */
	  conwritefail = TRUE;
	  log(LOG_ERR, "log: Write failed to console '%s'", consolename);
	}
      }
    }
    if (where & TOLOG) {
      /* If log file is not open, open it */
      while (logfile == NULL) {
	if ((logfile = logOpen(logfilename)) == NULL) {
	  if (!fileopenfail) /* log only once */
	    log(LOG_ERR,"Opening log file '%s' failed", logfilename);
	  if (logfilename == logfiledefname) {
	    fileopenfail = TRUE;	    
	    break;
	  }
	  /* try again with the default name */
	  logfilename = logfiledefname;
	  fileopenfail = filewritefail = FALSE;
	}
      }
      if (logfile != NULL) {
	int error = 
	  FPuts(logfile, buffer) == -1 ||
	    FWrite(logfile, msg->string, msg->chars, 1) != 1 ||
	      FPutC(logfile, '\n') == -1;
	Flush(logfile);
	if (error && !filewritefail) {	/* To avoid loops */
	  filewritefail = TRUE;
	  log(LOG_ERR, "log: Write failed to file '%s'", logfilename);
	}
      }
    }

    ReplyMsg((struct Message *)msg);
    if (GetLogMsgFail != TotalFail) {
      int t = GetLogMsgFail;	/* Check if we have lost messages */

      log(LOG_WARNING,"%ld log messages lost (total %ld lost)\n",
	  t - TotalFail, TotalFail);
      TotalFail = t;
    }
  }
  return(NULL);
}  

/* Close logging subsystem */
static
void log_close(struct log_msg *msg)
{
  rexx_deinit();
  if (confile)
    Close(confile);
  if (logfile)
    Close(logfile);
  if (logUtilityBase) {
    CloseLibrary(logUtilityBase);
    logUtilityBase = NULL;
  }
  if (logDOSBase) {		/* DOS not needed below */
    CloseLibrary(logDOSBase);
    logDOSBase = NULL;
  }
  if (SocketBase) {
    CloseLibrary((struct Library *)SocketBase);
    SocketBase = NULL;
  }
  /*
   * Make sure that we get to end before task switch
   * and do not get messages from interrupts
   */
  Disable();

  if (logPort) {
    struct Message *m;

    while (m = GetMsg(logPort))	/* Check for messages and reply */
      ReplyMsg(m);

    DeleteMsgPort(logPort);	/* Delete port */
    logPort = NULL;
  }

  if (msg)
    ReplyMsg((struct Message *)msg);

  Nettrace_Task = NULL;

  /*
   * Interrupts are left disabled, 
   * they will be enabled again when this process dies 
   */
}

/*
 * Try first open w/ shared lock, then as an old file and finally as a new file
 */
static
BPTR logOpen(STRPTR name)
{
  BPTR file;

  if ((file = Open(name, MODE_READWRITE)) ||
      (file = Open(name, MODE_OLDFILE)))
    Seek(file, 0, OFFSET_END);
  else
    file = Open(name, MODE_NEWFILE);
 
  return file;
}

/*
 * This function might be called by either AmiTCP or NETTRACE. If the
 * call is done by the AmiTCP, no DOS calls may be done, since the
 * DosBase used by these functions is the one of the NETTRACE, and is
 * not initialized at that time!
 */
int logname_changed(void *p, LONG new)
{
  if (p == &logfilename) {	/* Is logname requested? */
     /*
      * logfile may be non-NULL only if the NETTRACE is already initialized
      */
    if (logfile != NULL) {
      Close(logfile);
      logfile = NULL;
    }
    fileopenfail = filewritefail = FALSE;
    /*
     * setvalue() (who called this) will set the new value when we return 
     * TRUE.
     */
    return TRUE;
  }

  if ( p == &consolename ) { /* Name of the console log */
    
    if (confile) { /* only if NETTRACE is already initialized */
      Close(confile);
      confile = NULL;
    }
    conopenfail = conwritefail = FALSE;
    /*
     * setvalue() (who called this) will set the new value when we return 
     * TRUE.
     */
    return TRUE;
  }

  /*
   * Some invalid pointer
   */
  return FALSE;
}
