#include <exec/types.h>
#include <proto/exec.h>
#include <string.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "parse_string()" */
ULONG parse_string( STRPTR str,
                    STRPTR delim,
                    STRPTR *argv,
                    ULONG max_arg )
{
    STRPTR p, val;
    ULONG arg_cnt;

    arg_cnt = 0;
    for (p = str; p != NULL; ) {
        while ((val = (STRPTR)_strsep((STRPTR *)&p, delim)) != NULL && *val == '\0');
        argv[arg_cnt] = val;
        if (++arg_cnt >= max_arg) break;
    }
    if (arg_cnt < max_arg) argv[arg_cnt] = NULL;

    return arg_cnt;
}
/* \\\ */

