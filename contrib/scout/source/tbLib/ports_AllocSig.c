#include <exec/exec.h>
#include <exec/ports.h>
#include <dos/dos.h>

#include <proto/exec.h>
#include <clib/alib_protos.h>

#include <SDI/SDI_compiler.h>

/* /// "AllocSig()" */
BYTE AllocSig( BYTE instead )
{
    BYTE sig;

    sig = AllocSignal(-1);
    if (sig == -1) sig = instead;
    return sig;
}
/* \\\ */

