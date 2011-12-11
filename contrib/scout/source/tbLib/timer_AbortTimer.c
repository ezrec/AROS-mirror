#include <devices/timer.h>
#include <proto/exec.h>
#include <SDI/SDI_compiler.h>
#include "tblib.h"

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

