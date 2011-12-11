#include <exec/exec.h>
#include <exec/ports.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_compiler.h>

/* /// "FreeSig()" */
void FreeSig( BYTE sig )
{
    if (sig > SIGBREAKB_CTRL_F) FreeSignal(sig);
}
/* \\\ */

