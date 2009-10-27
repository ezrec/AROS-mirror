#include <devices/timer.h>
#include <proto/exec.h>
#include "SDI_compiler.h"

#if !defined(__amigaos4__) || !defined(__NEW_TIMEVAL_DEFINITION_USED__)
struct TimeVal
{
  ULONG Seconds;
  ULONG Microseconds;
};

struct TimeRequest
{
  struct IORequest Request;
  struct TimeVal   Time;
};
#endif

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

