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

/* /// "AbortTimer()" */
void AbortTimer( struct TimeRequest *treq )
{
    if (!CheckIO((struct IORequest *)treq)) {
        AbortIO((struct IORequest *)treq);
    }
    WaitIO((struct IORequest *)treq);
    GetMsg(treq->Request.io_Message.mn_ReplyPort);
    SetSignal(0, 1 << treq->Request.io_Message.mn_ReplyPort->mp_SigBit);
}
/* \\\ */

