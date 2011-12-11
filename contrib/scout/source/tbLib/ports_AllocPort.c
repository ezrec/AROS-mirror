#include <exec/exec.h>
#include <exec/ports.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_compiler.h>

/* /// "AllocPort()" */
BOOL AllocPort( struct MsgPort *port )
{
    ULONG sig;

    sig = AllocSignal(-1);
    if (sig != -1) {
        port->mp_SigTask = FindTask(NULL);
        port->mp_SigBit = sig;
        port->mp_Flags = PA_SIGNAL;
        port->mp_Node.ln_Type = NT_MSGPORT;
        NewList(&port->mp_MsgList);
        return TRUE;
    }
    return FALSE;
}
/* \\\ */

