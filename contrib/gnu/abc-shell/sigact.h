/* NAME:
 *      sigact.h - sigaction et al
 *
 * SYNOPSIS:
 *      #include "sigact.h"
 *
 * DESCRIPTION:
 *      This header is the interface to a fake sigaction(2) 
 *      implementation. It provides a POSIX compliant interface 
 *      to whatever signal handling mechanisms are available.
 *      It also provides a _Signal() function that is implemented 
 *      in terms of sigaction().
 *      If not using signal(2) as part of the underlying 
 *      implementation (USE_SIGNAL or USE_SIGMASK), and 
 *      NO_SIGNAL is not defined, it also provides a signal() 
 *      function that calls _Signal(). 
 *      
 * SEE ALSO:
 *      sigact.c
 */
/*
 * RCSid:
 *      $Id$
 */
/* Changes to sigact.h for pdksh, Michael Rendell <michael@cs.mun.ca>:
 *      - changed SIG_HDLR to void for use with GNU autoconf
 *      - ifdef'd out ARGS(), volatile and const initializations
 *      - ifdef'd out sigset_t definition - let autoconf handle it
 *      - ifdef out routines not used in ksh if IS_KSH is defined
 *        (same in sigact.c).
 */
#ifndef _SIGACT_H
#define _SIGACT_H

/*
 * if you want to install this header as signal.h,
 * modify this to pick up the original signal.h
 */
#ifndef SIGKILL
# include <signal.h>
#endif

#ifdef __amigaos4__
# include <signal.h>
#endif

#ifndef SIG_ERR
# define SIG_ERR  ((handler_t) -1)
#endif
#ifndef BADSIG
# define BADSIG  SIG_ERR
#endif
    
#ifndef SA_NOCLDSTOP
/* we assume we need the fake sigaction */
/* sa_flags */
#define SA_NOCLDSTOP    1               /* don't send SIGCHLD on child stop */
#define SA_RESTART      2               /* re-start I/O */

/* sigprocmask flags */
#ifdef AMIGA                            /* AmigaOS4 changes by Thomas Frieden */
#undef SIG_BLOCK
#undef SIG_UNBLOCK
#undef SIG_SETMASK
#endif

#define SIG_BLOCK       1
#define SIG_UNBLOCK     2
#define SIG_SETMASK     4

#if !defined(__sys_stdtypes_h) && !defined(CLIB2)
typedef unsigned int sigset_t;
#endif

/*
 * POSIX sa_handler should return void, but since we are
 * implementing in terms of something else, it may
 * be appropriate to use the normal void return type
 */
struct sigaction
{
  handler_t     sa_handler;
  sigset_t      sa_mask;
  int           sa_flags;
};

#if !defined(__AROS__)
int     sigaction(int, struct sigaction *, struct sigaction *);
#endif
int     sigaddset(sigset_t *, int);
int     sigemptyset(sigset_t *);
#ifndef CLIB2
#ifdef AMIGA            /* AmigaOS4 changes by Thomas Frieden */
int     sigprocmask(int, const sigset_t *, sigset_t *);
#else
int     sigprocmask(int, sigset_t *, sigset_t *);
#endif
#endif
int     sigsuspend(sigset_t *);
        
#ifndef sigmask
# define sigmask(s)     (1<<((s)-1))    /* convert SIGnum to mask */
#endif
#if !defined(NSIG) && defined(_NSIG)
# define NSIG _NSIG
#endif
#endif /* ! SA_NOCLDSTOP */
#endif /* _SIGACT_H */
