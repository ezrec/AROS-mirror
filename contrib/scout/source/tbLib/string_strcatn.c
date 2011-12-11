#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "_strcatn()" */
void _strcatn( STRPTR str,
               CONST_STRPTR cat,
               ULONG maxlen )
{
    ULONG sl, cl;

    sl = strlen(str);
    cl = strlen(cat);

    if (sl + cl >= maxlen) {
        strncat(str, cat, maxlen - sl - 1);
    } else {
        strcat(str, cat);
    }
}
/* \\\ */

