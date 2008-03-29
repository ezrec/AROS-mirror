#ifndef KSH_TIMES_H
# define KSH_TIMES_H

/* Needed for INT32 on some systems (ie, NeXT in non-posix mode) */
#include "ksh_time.h"

#ifdef AMIGA
struct tms {
        INT32 tms_utime;              /* user time */
        INT32 tms_stime;              /* system time */
        INT32 tms_cutime;             /* user time, children */
        INT32 tms_cstime;             /* system time, children */
};
#else
#include <sys/times.h>
#endif

extern INT32    ksh_times(struct tms *);

#ifdef HAVE_TIMES
extern INT32    times(struct tms *);
#endif /* HAVE_TIMES */
#endif /* KSH_TIMES_H */
