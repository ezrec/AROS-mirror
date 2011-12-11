#include <devices/timer.h>
#include <proto/exec.h>
#include <SDI/SDI_compiler.h>
#include "tblib.h"

/* /// "StartTimer()" */
void StartTimer( struct TimeRequest *treq,
                 ULONG secs,
                 ULONG mics )
{
    treq->Request.io_Command = TR_ADDREQUEST;
    treq->Time.Seconds = secs;
    treq->Time.Microseconds = mics;
    SendIO((struct IORequest *)treq);
}
/* \\\ */

