#include <devices/timer.h>
#include <proto/exec.h>
#include <SDI/SDI_compiler.h>
#include "tblib.h"

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
        (void)test;
    }
}
/* \\\ */

