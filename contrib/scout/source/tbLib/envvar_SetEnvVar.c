#include <SDI/SDI_compiler.h>
#include <exec/types.h>
#include <exec/ports.h>
#include <dos/var.h>
#include <proto/dos.h>
#include <string.h>
#include "tblib.h"

/* /// "SetEnvVar()" */
BOOL SetEnvVar( STRPTR name,
                STRPTR buffer,
                ULONG size,
                ULONG flags,
                BOOL save )
{
    flags |= GVF_GLOBAL_ONLY;
    if (save) {
        flags |= GVF_SAVE_VAR;
    }

    return (BOOL)SetVar(name, buffer, size, flags);
}
/* \\\ */

