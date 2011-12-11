#include <exec/types.h>
#include <exec/ports.h>
#include <dos/var.h>
#include <proto/dos.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "GetEnvVar()" */
LONG GetEnvVar( STRPTR name,
                STRPTR buffer,
                ULONG size,
                ULONG flags )
{
    TEXT varName[128];
    LONG len;

    flags |= GVF_GLOBAL_ONLY;
    _sprintf(varName, "ENV:%s", name);
    len = GetVar(varName, buffer, size, flags);
    if (len <= 0) {
        _snprintf(varName, sizeof(varName), "ENVARC:%s", name);
        len = GetVar(varName, buffer, size, flags);
    }

    return len;
}
/* \\\ */

