#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include "SDI_compiler.h"

/* /// "_strdup()" */
STRPTR _strdup( STRPTR str )
{
    STRPTR tmp;

    if ((tmp = AllocVec(strlen(str) + 1, MEMF_CLEAR | MEMF_PUBLIC)) != NULL) {
        strcpy(tmp, str);
    }

    return tmp;
}
/* \\\ */

