#include <aros/kernel.h>
#include <aros/libcall.h>
#include <sys/stat.h>
#include <proto/exec.h>
#include <fcntl.h>
#include <string.h>

#include <aros/debug.h>

#include <kernel_base.h>

#include "kernel_intern.h"

/*****************************************************************************

    NAME */
#include <proto/kernel.h>

AROS_LH0(unsigned int, KrnGetCPUCount,

/*  SYNOPSIS */

/*  LOCATION */
	struct KernelBase *, KernelBase, 36, Kernel)

/*  FUNCTION
	Return total number of processors in the system

    INPUTS
	None

    RESULT
	Number of running CPUs in this system

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    static int cpucount = 0;

    if (cpucount == 0)
    {
        int fd;
        char *mem = AllocVec(65536, MEMF_CLEAR);
        char *ptr = mem;

        if (mem)
        {
            fd = KernelBase->kb_PlatformData->iface->open("/proc/cpuinfo", O_RDONLY);

            if (fd != -1)
            {
                KernelBase->kb_PlatformData->iface->read(fd, mem, 65535);
                KernelBase->kb_PlatformData->iface->close(fd);
            }

            while((ptr = strstr(ptr, "processor\t:"))!=NULL)
            {
                cpucount++;
                ptr++;
            }

            FreeVec(mem);
        }
    }

    return cpucount;

    AROS_LIBFUNC_EXIT
}
