#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "_strsep()" */
STRPTR _strsep( STRPTR *str,
                STRPTR delim )
{
    STRPTR result = NULL;
    ULONG i;

    for (i = 0; i < strlen(delim); i++) {
        STRPTR p;

        p = strchr(*str, delim[i]);
        if (p) {
            result = *str;
            *p++ = 0;
            *str = p;
            break;
        }
    }

    if (result == NULL) {
        result = *str;
        *str = NULL;
    }

    return result;
}
/* \\\ */

