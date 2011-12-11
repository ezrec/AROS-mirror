#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "xget()" */
ULONG xget( Object *obj,
            ULONG attr )
{
    ULONG b = 0;

    GetAttr(attr, obj, &b);

    return b;
}
/* \\\ */

