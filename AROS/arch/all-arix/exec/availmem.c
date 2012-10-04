/*
 * availmem.c
 *
 *  Created on: Oct 4, 2012
 *      Author: michal
 */


#include <exec/memory.h>

#include "exec_intern.h"

#include <sys/types.h>
#include <string.h>

int sscanf(const char *s1, const char *s2, ...);

/*****************************************************************************

    NAME */

    AROS_LH1(ULONG, AvailMem,

/*  SYNOPSIS */
    AROS_LHA(ULONG, attributes, D1),

/*  LOCATION */
    struct ExecBase *, SysBase, 36, Exec)

/*  FUNCTION
    Return either the total available memory or the largest available
    chunk of a given type of memory.

    INPUTS
    attributes - The same attributes you would give to AllocMem().

    RESULT
    Either the total of the available memory or the largest chunk if
    MEMF_LARGEST is set in the attributes.

    NOTES
    Due to the nature of multitasking the returned value may already
    be obsolete when this function returns.

    EXAMPLE
    Print the total available memory.

    printf("Free memory: %lu bytes\n", AvailMem(0));

    Print the size of the largest chunk of chip memory.

    printf("Largest chipmem chunk: %lu bytes\n",
           AvailMem(MEMF_CHIP|MEMF_LARGEST));

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    static char minfo[4096];
    int fd;

    ULONG ret = 0;

    fd = PD(SysBase).SysIFace->open("/proc/meminfo", 0);
    if (fd != -1)
    {
        char *str;
        size_t siz = PD(SysBase).SysIFace->read(fd, minfo, 4095);
        minfo[siz] = 0;
        PD(SysBase).SysIFace->close(fd);

        if (siz > 0 && (attributes & MEMF_CHIP))
        {
            if (attributes & MEMF_TOTAL)
            {
                str = strstr(minfo, "MemTotal:");
                if (str)
                {
                    unsigned long long tmp;
                    sscanf(str, "MemTotal: %Lu kB", &tmp);
                    ret = tmp << 10;
                }
            }
            else
            {
                str = strstr(minfo, "MemFree:");
                if (str)
                {
                    unsigned long long tmp;
                    sscanf(str, "MemFree: %Lu kB", &tmp);
                    ret = tmp;
                }

                str = strstr(minfo, "Buffers:");
                if (str)
                {
                    unsigned long long tmp;
                    sscanf(str, "Buffers: %Lu kB", &tmp);
                    ret += tmp;
                }

                str = strstr(minfo, "Cached:");
                if (str)
                {
                    unsigned long long tmp;
                    sscanf(str, "Cached: %Lu kB", &tmp);
                    ret += tmp;
                }

                ret <<= 10;
            }
        }
    }

    return ret;

    AROS_LIBFUNC_EXIT
} /* AvailMem */

