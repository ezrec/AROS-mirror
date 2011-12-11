#include <exec/exec.h>
#include <exec/ports.h>
#include <dos/dos.h>
#include <string.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_compiler.h>

/* /// "SafePutMsg()" */
void SafePutMsg( STRPTR portname,
                 struct Message *msg,
                 BOOL wait4reply )
{
    struct MsgPort lport, *rport, *port;

    rport = msg->mn_ReplyPort;

    memset(&lport, 0x00, sizeof(struct MsgPort));
    lport.mp_Flags = PA_SIGNAL;
    lport.mp_SigBit = SIGB_SINGLE;
    lport.mp_SigTask = FindTask(NULL);
    NewList(&lport.mp_MsgList);

    msg->mn_ReplyPort = &lport;

    Forbid();

    if ((port = FindPort(portname)) != NULL) {
        SetSignal(0, SIGF_SINGLE);
        PutMsg(port, msg);
    }

    Permit();

    if (port && wait4reply) {
        WaitPort(&lport);
        GetMsg(&lport);
    }

    msg->mn_ReplyPort = rport;
}
/* \\\ */

