/****************************************************************************
*                aros.c
*
*  This module implements AROS specific routines.
*
*  Copyright © 2001, The AROS Development Team. All rights reserved.
*
*****************************************************************************/

#include "frame.h"
#include "povproto.h"  /* For Status_Info */
#include "povray.h"    /* For Stop_Flag */

/* The following includes are newly required by Mike Fleetwood's modifications */
#include "optin.h"     /* For parse_ini_file() */
#include <stdlib.h>    /* For getenv() */
#include <string.h>    /* For strncpy(), strncat() */
/* That's the end of the newly required includes */

#include <sys/time.h>  /* For gettimeofday() */
#include <signal.h>

#undef SIGTERM
#undef SIGFPE
#undef SIGPIPE
#undef SIGINT

void UNIX_Abort_Start();

/*****************************************************************************
* Local preprocessor defines
******************************************************************************/



/*****************************************************************************
* Local typedefs
******************************************************************************/



/*****************************************************************************
* Local variables
******************************************************************************/
/*
 * Variables used by the histogram timers
 */
#if PRECISION_TIMER_AVAILABLE
static struct timeval hstart, hstop;
#endif


/*****************************************************************************
* Static functions
******************************************************************************/



/*****************************************************************************
*
* FUNCTION
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR
*
* DESCRIPTION
*
*    The error handler for floating point exceptions.  Not totally critical,
*    but can avoid aborting a trace that gets a floating-point error during
*    the render.  It tries to return a reasonable value for the errors.
*    However, this slows down the rendering noticably.
*
* CHANGES
*
******************************************************************************/

#ifdef UNDERFLOW
int matherr(x)
struct exception *x;
{
  switch (x->type)
  {
    case DOMAIN:
    case OVERFLOW:

      x->retval = 1.0e17;

      break;

    case SING:
    case UNDERFLOW:

      x->retval = 0.0;

      break;
  }

  return (1);
}

#endif



#if PRECISION_TIMER_AVAILABLE

/*****************************************************************************
*
* FUNCTION  UNIX_Timer_Start
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Andreas Dilger   Oct 1995
*
* DESCRIPTION  Starts the histogram timer
*
* CHANGES
*
******************************************************************************/

void UNIX_Timer_Start()
{
  gettimeofday(&hstart, (struct timezone *)NULL);
}


/*****************************************************************************
*
* FUNCTION  UNIX_Timer_Stop
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Andreas Dilger   Oct 1995
*
* DESCRIPTION  Stops the histogram timer
*
* CHANGES
*
******************************************************************************/

void UNIX_Timer_Stop()
{
  gettimeofday(&hstop, (struct timezone *)NULL);
}


/*****************************************************************************
*
* FUNCTION  UNIX_Timer_Count
*
* INPUT
*
* OUTPUT
*
* RETURNS  The elapsed real time between start and stop in 10^-5s increments
*
* AUTHOR  Andreas Dilger   Oct 1995
*
* DESCRIPTION  The elapsed wall-clock time between the last two calls to
*              accumulate_histogram is measured in tens of microseconds.
*              Using microseconds for the histogram timer would only allow
*              1.2 hours per histogram bucket (ie 2^32 x 10^-6s in an unsigned
*              32 bit long), which may not be enough in some rare cases :-)
*              Note that since this uses wall-clock time rather than CPU
*              time, the histogram will have noise in it because of the
*              task switching going on.
*
* CHANGES
*
******************************************************************************/

int UNIX_Timer_Count()
{
  return ((hstop.tv_sec - hstart.tv_sec) * 100000 +
          (hstop.tv_usec - hstart.tv_usec + 5) / 10);
}

#endif /* PRECISION_TIMER_AVAILABLE */


/*****************************************************************************
*
* FUNCTION  UNIX_Abort_Handler
*
* INPUT  signum - signal number, or 0 if not called by a signal
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Andreas Dilger   Oct 1995
*
* DESCRIPTION
*      This is the signal handler.  If it is called by a signal (signal > 0),
*      then it will call set the global "Stop_Flag", and the rendering will
*      be aborted when the current pixel is complete.  We re-initialize the
*      signal handler for those older systems that reset the signal handlers
*      when they are called.  If we are really quitting (signal = 0) or this
*      routine has been called many times, we set the signal handlers back to
*      their default action, so that further siganls are handled normally.
*      This is so we don't get caught in a loop if we are handling a signal
*      caused by the shutdown process (ie file output to a pipe that's closed).
*
* CHANGES
*
*   Sept 1, 1996   Don't write to stderr if there have been many interrupts,
*                  so we don't write to a PIPE that's closed and cause more
*                  interrupts. [AED]
*
******************************************************************************/

void UNIX_Abort_Handler(signum)
int signum;
{
  UNIX_Abort_Start();

  if (signum > 0 && ++Stop_Flag < 5)
  {
    switch (signum)
    {
#ifdef SIGINT
      case SIGINT: Status_Info("\nGot %d SIGINT.", Stop_Flag); break;
#endif
#ifdef SIGPIPE
      case SIGPIPE: Status_Info("\nGot %d SIGPIPE.", Stop_Flag); break;
#endif
#ifdef SIGTERM
      case SIGTERM: Status_Info("\nGot %d SIGTERM.", Stop_Flag); break;
#endif
      default: Status_Info("\nGot %d unknown signal (%d).", Stop_Flag, signum); break;
    }
  }

  if (signum == 0 || Stop_Flag > 10)
  {
#ifdef SIGTERM
    signal(SIGTERM, SIG_DFL);
#endif
#ifdef SIGINT
    signal(SIGINT, SIG_DFL);
#endif
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_DFL);
#endif
  }
}


/*****************************************************************************
*
* FUNCTION  UNIX_Abort_Start
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Andreas Dilger   Oct 1995
*
* DESCRIPTION  Initializes the signal handlers
*
* CHANGES
*
*   Apr 1996: Add SIGFPE to be ignored, bacause of floating point exceptions.
*             This isn't a great solution, but I don't know of another way to
*             prevent core dumps on Linux systems that use older versions of
*             libm. [AED]
*
******************************************************************************/

void UNIX_Abort_Start()
{
#ifdef SIGTERM
  signal(SIGTERM, UNIX_Abort_Handler);
#endif
#ifdef SIGINT
  signal(SIGINT, UNIX_Abort_Handler);
#endif
#ifdef SIGPIPE
  signal(SIGPIPE, UNIX_Abort_Handler);
#endif
#if !defined(OVERFLOW) && defined(SIGFPE) /* avoid floating point exceptions */
  signal(SIGFPE, SIG_IGN);
#endif
}

/*****************************************************************************
*
* FUNCTION  UNIX_Process_Env
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Mike Fleetwood  Jan 1999
*
* DESCRIPTION  Process a default INI file pointed to by the environment
*              variable POVINI.  Leaves the global variable Option_String_Ptr
*              set to non-NULL if and only if the environment variable POVINI
*              points to a file which was read.
*
* CHANGES
*
******************************************************************************/

void UNIX_Process_Env(void)
{
  if ((Option_String_Ptr = getenv("POVINI")) != NULL)
  {
    if (!parse_ini_file(Option_String_Ptr))
    {
      Warning(0.0, "Could not find '%s' as specified in POVINI environment.\n",
	      Option_String_Ptr);
      Option_String_Ptr = NULL;
    }
  }
  return;
}

/*****************************************************************************
*
* FUNCTION  UNIX_Process_Povray_Ini
*
* INPUT
*
* OUTPUT
*
* RETURNS
*
* AUTHOR  Mike Fleetwood  Jan 1999
*
* DESCRIPTION  If UNIX_Process_Env() processed a default INI file from the
*              POVINI environment variable then do nothing; otherwise search
*              for a default INI file, reading the first one found, from the
*              list:
*                  ./povray.ini
*                  $HOME/.povrayrc
*                  POV_LIB_DIR/povray.ini
*
* CHANGES
*
******************************************************************************/

#define FILE_NAME_MAX POV_NAME_MAX
void UNIX_Process_Povray_Ini(void)
{
  char ini_name[FILE_NAME_MAX];

  if (Option_String_Ptr != NULL)
    return;
  if (parse_ini_file("povray.ini"))
    return;
  if ((Option_String_Ptr = getenv("HOME")) != NULL)
  {
    strncpy(ini_name, Option_String_Ptr, FILE_NAME_MAX);
    strncat(ini_name, "/.povrayrc", FILE_NAME_MAX);
    if (parse_ini_file(ini_name))
      return;
    else
      Option_String_Ptr = NULL;
  }
#ifdef POV_LIB_DIR
  strncpy(ini_name, POV_LIB_DIR, FILE_NAME_MAX);
  strncat(ini_name, "/povray.ini", FILE_NAME_MAX);
  parse_ini_file(ini_name);
#endif
  return;
}

