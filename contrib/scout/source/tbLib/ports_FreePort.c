#include <exec/exec.h>
#include <exec/ports.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_compiler.h>

/* /// "FreePort()" */
void FreePort( struct MsgPort *port )
{
    port->mp_Flags = PA_IGNORE;
    port->mp_SigTask = NULL;
    if (port->mp_SigBit != 0xff) FreeSignal(port->mp_SigBit);
}
/* \\\ */

