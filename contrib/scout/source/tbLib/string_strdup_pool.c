#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "_strdup_pool()" */
STRPTR _strdup_pool( CONST_STRPTR str,
                     APTR pool )
{
    STRPTR tmp;

    tmp = tbAllocVecPooled(pool, strlen(str) + 1);
    if (tmp) strcpy(tmp, str);

    return tmp;
}
/* \\\ */

