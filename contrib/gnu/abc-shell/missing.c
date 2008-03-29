/*
 * Routines which may be missing on some machines
 */

#include <sys/stat.h>
#include "sh.h"

#if !defined __amigaos4__ && !defined __AROS__
void *
memmove(void *d, const void *s, size_t n)
{
        char *dp = (char *) d, *sp = (char *) s;

        if (n <= 0)
                ;
        else if (dp < sp)
                do
                        *dp++ = *sp++;
                while (--n > 0);
        else if (dp > sp) {
                dp += n;
                sp += n;
                do
                        *--dp = *--sp;
                while (--n > 0);
        }
        return d;
}
#endif

#ifdef AMIGA
INT32
ksh_times(void *tms)
{
        return 0;
}
#else
# include "ksh_time.h"
# include "ksh_times.h"
# include <sys/timeb.h>

INT32
ksh_times(struct tms *tms)
{
        static INT32 base_sec;
        INT32 rv;
        /* Assume times() available, but always returns 0
         * (also assumes ftime() available)
         */
        {
                struct timeb tb;

                if (times(tms) == (INT32) -1)
                        return (INT32) -1;
                ftime(&tb);
                if (base_sec == 0)
                        base_sec = tb.time;
                rv = (tb.time - base_sec) * CLK_TCK;
                rv += tb.millitm * CLK_TCK / 1000;
        }
        return rv;
}

#endif

int
dup2(int oldd, int newd)
{
        int old_errno;

        if (fcntl(oldd, F_GETFL, 0) == -1)
                return -1;      /* errno == EBADF */

        if (oldd == newd)
                return newd;

        old_errno = errno;

        close(newd);    /* in case its open */

        errno = old_errno;

        return fcntl(oldd, F_DUPFD, newd);
}
