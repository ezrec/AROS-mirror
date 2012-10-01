/*
 * cpumeter_task.c
 *
 *  Created on: Oct 1, 2012
 *      Author: michal
 */


#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define DEBUG 0
#include <aros/debug.h>

#include <exec/ports.h>
#include <exec/errors.h>
#include <exec/io.h>
#include <devices/timer.h>
#include <aros/libcall.h>
#include <resources/processor.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/hostlib.h>

#include "processor_intern.h"
#include "processor_arch_intern.h"

struct HostInterface {
    int     (*open)(const char *path, int flags, ...);
    int     (*close)(int fd);
    ssize_t (*read)(int fd, void *buffer, size_t size);
    int     *(*error)();
};

static const char *libcSymbols[] =
{
    "open",
    "close",
    "read",
    "__errno_location",
    NULL
};

struct statistics {
    UQUAD user;
    UQUAD nice;
    UQUAD system;
    UQUAD idle;
};

void cpumeter_task(struct X86ProcessorInformation **sysproc, int ncpu)
{
    APTR HostLibBase;
    APTR handle;
    struct HostInterface *hi;
    ULONG r;
    struct MsgPort *tport;
    struct timerequest *treq;
    char *buffer;
    char *ptr;
    char tmpbuf[10];
    int fd;
    int *errno_loc;

    struct statistics oldstats[ncpu];
    struct statistics newstats[ncpu];

    buffer = AllocVec(4096, MEMF_CLEAR);

    HostLibBase = OpenResource("hostlib.resource");

    handle = HostLib_Open("libc.so.6", NULL);

    if (!handle)
        return;

    hi = (struct HostInterface *)HostLib_GetInterface(handle, libcSymbols, &r);

    if ((!hi) || r)
        return;

    tport = CreateMsgPort();
    treq = (struct timerequest *)CreateIORequest(tport, sizeof(struct timerequest));

    HostLib_Lock();
    Forbid();
    errno_loc = hi->error();
    fd = hi->open("/proc/stat", 0, 0);
    if (fd != -1)
    {
        hi->read(fd, buffer, 4095);
        hi->close(fd);
    }
    Permit();
    HostLib_Unlock();

    if (fd != -1)
    {
        int i;
        for (i=0; i < ncpu; i++)
        {
            snprintf(tmpbuf, 9, "cpu%d", i);

            ptr = strstr(buffer, tmpbuf);
            if (ptr)
            {
                int tmp;
                sscanf(ptr, "cpu%d %Ld %Ld %Ld %Ld", &tmp,
                        &oldstats[i].user, &oldstats[i].nice,
                        &oldstats[i].system, &oldstats[i].idle);
            }
        }
    }

    OpenDevice("timer.device", UNIT_VBLANK, (struct IORequest *)treq, 0);

    do {
        treq->tr_node.io_Command = TR_ADDREQUEST;
        treq->tr_time.tv_micro = 0;
        treq->tr_time.tv_sec = 1;

        DoIO((struct IORequest *)treq);

        HostLib_Lock();
        Forbid();
        fd = hi->open("/proc/stat", 0, 0);
        if (fd != -1)
        {
            hi->read(fd, buffer, 4095);
            hi->close(fd);
        }
        Permit();
        HostLib_Unlock();

        if (fd != -1)
        {
            int i;
            D(bug("CPUStat: "));
            for (i=0; i < ncpu; i++)
            {
                UQUAD user, nice, system, idle;
                UQUAD total, usage;

                snprintf(tmpbuf, 9, "cpu%d", i);

                ptr = strstr(buffer, tmpbuf);
                if (ptr)
                {
                    int tmp;
                    sscanf(ptr, "cpu%d %Ld %Ld %Ld %Ld", &tmp,
                            &newstats[i].user, &newstats[i].nice,
                            &newstats[i].system, &newstats[i].idle);

                    user = newstats[i].user - oldstats[i].user;
                    nice = newstats[i].nice - oldstats[i].nice;
                    idle = newstats[i].idle - oldstats[i].idle;
                    system = newstats[i].system - oldstats[i].system;

                    CopyMem(newstats, oldstats, sizeof(newstats));

                    total = user+nice+idle+system;
                    usage = ((total - idle) * 1000 + 5) / total;
                    sysproc[i]->CpuUsage = (ULONG)usage;

                    D(bug("cpu%d: %d   ", i, sysproc[i]->CpuUsage));
                }
            }
            D(bug("\n"));
        }

    } while(1);
}
