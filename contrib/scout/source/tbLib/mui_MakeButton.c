#include <exec/ports.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <clib/alib_protos.h>
#include "tblib.h"
#include <SDI/SDI_compiler.h>

/* /// "MakeButton()" */
APTR MakeButton( CONST_STRPTR txt )
{
    APTR obj;

    if ((obj = MUI_MakeObject(MUIO_Button, txt)) != NULL) {
        set(obj, MUIA_CycleChain, TRUE);
    }

    return obj;
}
/* \\\ */

