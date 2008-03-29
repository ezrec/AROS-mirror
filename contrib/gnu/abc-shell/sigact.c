/* NAME:
 *      sigact.c - fake sigaction(2)
 *
 * SYNOPSIS:
 *      #include "sigact.h"
 * 
 *      int sigaction(int sig, struct sigaction *act, 
 *                      struct sigaction *oact);
 *      int sigaddset(sigset_t *mask, int sig);
 *      int sigdelset(sigset_t *mask, int sig);
 *      int sigemptyset(sigset_t *mask);
 *      int sigfillset(sigset_t *mask);
 *      int sigismember(sigset_t *mask, int sig);
 *      int sigpending(sigset_t *set);
 *      int sigprocmask(int how, sigset_t *set, sigset_t *oset);
 *      int sigsuspend(sigset_t *mask);
 *      
 *      void (*_Signal(int sig, void (*disp)(int)))(int);
 *
 * DESCRIPTION:
 *      This is a fake sigaction implementation.  It uses 
 *      sigsetmask(2) et al or sigset(2) and friends if 
 *      available, otherwise it just uses signal(2).  If it 
 *      thinks sigaction(2) really exists it compiles to "almost" 
 *      nothing. 
 *      
 *      In any case it provides a _Signal() function that is 
 *      implemented in terms of sigaction().
 *      If not using signal(2) as part of the underlying 
 *      implementation (USE_SIGNAL or USE_SIGMASK), and 
 *      NO_SIGNAL is not defined, it also provides a signal() 
 *      function that calls _Signal(). 
 *
 *      The need for all this mucking about is the problems 
 *      caused by mixing various signal handling mechanisms in 
 *      the one process.  This module allows for a consistent 
 *      POSIX compliant interface to whatever is actually 
 *      available. 
 *      
 *      sigaction() allows the caller to examine and/or set the 
 *      action to be associated with a given signal. "act" and 
 *      "oact" are pointers to 'sigaction structs':
 *.nf
 * 
 *      struct sigaction 
 *      {
 *             void  (*sa_handler)();
 *             sigset_t  sa_mask;
 *             int       sa_flags;
 *      };
 *.fi
 * 
 *      void is normally 'void' in the POSIX implementation 
 *      and for most current systems.  On some older UNIX 
 *      systems, signal handlers do not return 'void', so  
 *      this implementation keeps 'sa_handler' inline with the 
 *      hosts normal signal handling conventions.
 *      'sa_mask' controls which signals will be blocked while 
 *      the selected signal handler is active.  It is not used 
 *      in this implementation.
 *      'sa_flags' controls various semantics such as whether 
 *      system calls should be automagically restarted 
 *      (SA_RESTART) etc.  It is not used in this 
 *      implementation. 
 *      Either "act" or "oact" may be NULL in which case the 
 *      appropriate operation is skipped.
 *      
 *      sigaddset() adds "sig" to the sigset_t pointed to by "mask".
 *      
 *      sigdelset() removes "sig" from the sigset_t pointed to 
 *      by "mask". 
 *      
 *      sigemptyset() makes the sigset_t pointed to by "mask" empty.
 *      
 *      sigfillset() makes the sigset_t pointed to by "mask" 
 *      full ie. match all signals.
 *      
 *      sigismember() returns true if "sig" is found in "*mask".
 *      
 *      sigpending() is supposed to return "set" loaded with the 
 *      set of signals that are blocked and pending for the 
 *      calling process.  It does nothing in this impementation.
 *      
 *      sigprocmask() is used to examine and/or change the 
 *      signal mask for the calling process.  Either "set" or 
 *      "oset" may be NULL in which case the appropriate 
 *      operation is skipped.  "how" may be one of SIG_BLOCK, 
 *      SIG_UNBLOCK or SIG_SETMASK.  If this package is built 
 *      with USE_SIGNAL, then this routine achieves nothing.
 *      
 *      sigsuspend() sets the signal mask to "*mask" and waits 
 *      for a signal to be delivered after which the previous 
 *      mask is restored.
 *      
 *      
 * RETURN VALUE:
 *      0==success, -1==failure
 *
 * BUGS:
 *      Since we fake most of this, don't expect fancy usage to 
 *      work.
 *
 * AUTHOR:
 *      Simon J. Gerraty <sjg@zen.void.oz.au>
 */     
/* COPYRIGHT:
 *      @(#)Copyright (c) 1992 Simon J. Gerraty
 *
 *      This is free software.  It comes with NO WARRANTY.
 *      Permission to use, modify and distribute this source code
 *      is granted subject to the following conditions.
 *      1/ that that the above copyright notice and this notice 
 *      are preserved in all copies and that due credit be given 
 *      to the author.  
 *      2/ that any changes to this code are clearly commented 
 *      as such so that the author does get blamed for bugs 
 *      other than his own.
 *      
 *      Please send copies of changes and bug-fixes to:
 *      sjg@zen.void.oz.au
 *
 */
/* Changes to sigact.c for pdksh, Michael Rendell <michael@cs.mun.ca>:
 *      - sigsuspend(): pass *mask to bsd4.2 sigpause instead of mask.
 *      - changed SIG_HDLR to void for use with GNU autoconf
 *      - include sh.h instead of signal.h (to get *_SIGNALS macros)
 *      - changed if !SA_NOCLDSTOP ... to USE_FAKE_SIGACT to avoid confusion
 *      - set the USE_* defines using the *_SIGNALS defines from autoconf
 *      - sigaction(): if using BSD signals, use sigvec() (used to use
 *        signal()) and set the SV_INTERRUPT flag (POSIX says syscalls
 *        are interrupted and pdksh needs this behaviour).
 *      - define IS_KSH before including anything; ifdef out routines
 *        not used in ksh if IS_KSH is defined (same in sigact.h).
 *      - use ARGS() instead of __P()
 *      - sigaction(),sigsuspend(),_Signal(),signal(): use handler_t typedef
 *        instead of explicit type.
 */

/*
    #include <signal.h>
*/
#define IS_KSH
#include "sh.h"

/*
    #ifndef __P
    # if defined(__STDC__) || defined(__cplusplus)
    #   define  __P(p)  p
    # else
    #   define  __P(p)  ()
    # endif
    #endif
*/


/*
 * some systems have a faulty sigaction() implementation!
 * Allow us to bypass it.
 * Or they may have installed sigact.h as signal.h which is why 
 * we have SA_NOCLDSTOP defined.
 */

#include "sigact.h"

int
#if defined(__AROS__)
sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
#else
sigaction(int sig, struct sigaction *act, struct sigaction *oact)
#endif
{
  handler_t oldh;

  if (act)
  {
    oldh = signal(sig, act->sa_handler);
  }
  else
  {
    if (oact)
    {      
      oldh = signal(sig, SIG_IGN);
      if (oldh != SIG_IGN && oldh !=  SIG_ERR)
      {
        (void) signal(sig, oldh);
      }
    }
  }
  if (oact)
  {
    oact->sa_handler = oldh;
  }
  return 0;                             /* hey we're faking it */
}

#if !defined(NEWLIB) && !defined(__AROS__)
int sigaddset(sigset_t *mask, int sig)
{
  *mask |= sigmask(sig);
  return 0;
}

int
sigemptyset(sigset_t *mask)
{
  *mask = 0;
  return 0;
}
#endif

#if !defined(CLIB2) && !defined(__AROS__)
int
sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
  static sigset_t sm;
  static int once = 0;

  if (!once)
  {
    /*
     * initally we clear sm,
     * there after, it represents the last
     * thing we did.
     */
    once++;
    sm = 0;
  }
  
  if (oset)
    *oset = sm;
  if (set)
  {
    switch (how)
    {
    case SIG_BLOCK:
      sm |= *set;
      break;
    case SIG_UNBLOCK:
      sm &= ~(*set);
      break;
    case SIG_SETMASK:
      sm = *set;
      break;
    }
  }
  return 0;
}
#endif

#if !defined(void)
# define void void
#endif
#if !defined(SIG_ERR)
# define SIG_ERR        (void (*)())-1
#endif

