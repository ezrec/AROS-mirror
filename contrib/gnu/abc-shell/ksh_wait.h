/* Wrapper around the ugly sys/wait includes/ifdefs */
/* $Id$ */

/* Get rid of system macros (which probably use union wait) */
#undef WIFCORED
#undef WIFEXITED
#undef WEXITSTATUS
#undef WIFSIGNALED
#undef WTERMSIG
#undef WIFSTOPPED
#undef WSTOPSIG

typedef int WAIT_T;

#define WIFCORED(s)     ((s) & 0x80)
#define WSTATUS(s)      (s)

#define WIFEXITED(s)    (((s) & 0xff) == 0)
#define WEXITSTATUS(s)  (((s) >> 8) & 0xff)
#define WIFSIGNALED(s)  (((s) & 0xff) != 0 && ((s) & 0xff) != 0x7f)
#define WTERMSIG(s)     ((s) & 0x7f)
#define WIFSTOPPED(s)   (((s) & 0xff) == 0x7f)
#define WSTOPSIG(s)     (((s) >> 8) & 0xff)

#define ksh_waitpid(p, s, o)    waitpid((p), (s), (o))
