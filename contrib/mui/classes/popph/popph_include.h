/*
** $Id: popph_include.h,v 1.4 1999/11/25 18:38:56 carlos Exp $
**
** © 1999 Marcin Orlowski <carlos@amiga.com.pl>
**
*/

/*** includes ***/

#include <string.h>
#include <stdlib.h>

#include <exec/memory.h>
#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <clib/muimaster_protos.h>
#include <pragmas/muimaster_pragmas.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>

#include <clib/iffparse_protos.h>
#include <devices/clipboard.h>


#if MUIMASTER_VLATEST <= 14
#include <mui/mui33_mcc.h>
#endif


#define MYDEBUG 0
#include "debug.h"

#if MUIMASTER_VLATEST < 20
#include "muiundoc.h"
#endif

#include "//vapor/textinput/textinput_mcc.h"
#include <mui/betterstring_mcc.h>



