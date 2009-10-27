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

/* /// "TimerDelay()" */
void TimerDelay( struct TimeRequest *treq,
                 ULONG secs,
                 ULONG mics )
{
    if (treq->Request.io_Message.mn_ReplyPort->mp_SigBit != 0xff) { // SigBit ist allokiert
        treq->Request.io_Command = TR_ADDREQUEST;
        treq->Time.Seconds = secs;
        treq->Time.Microseconds = mics;
        DoIO((struct IORequest *)treq);
    } else {
        volatile UBYTE *addr = (UBYTE *)0xbfe001;
        ULONG cnt;
        ULONG max;
        volatile UBYTE test;

        max = secs * 1000000 + mics;
        for (cnt = 0; cnt < max; cnt++) test = *addr;
    }
}
/* \\\ */

