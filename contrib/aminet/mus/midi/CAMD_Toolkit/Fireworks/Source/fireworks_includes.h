
/*        Fireworks include files           */
/* for creating a Global Symbol table (GST) */

#define __USE_SYSBASE 1
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/asl.h>
#include <proto/wb.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/interrupts.h>
#include <hardware/intbits.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <graphics/gfxmacros.h>
#include <graphics/scale.h>
#include <graphics/modeid.h>
#include <graphics/displayinfo.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>
#include <libraries/asl.h>
#include <utility/tagitem.h>

#include <clib/alib_protos.h>

#include <workbench/workbench.h>

#include <datatypes/pictureclass.h>
#include <datatypes/PictureClassExt.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include <midi/camd.h>
#include <midi/mididefs.h>
#include <proto/camd.h>

#ifndef _AROS
#  include "camdlists.h"
#  include "listreq.h"
#endif

#include <pragmas/timer_pragmas.h>
#include <clib/timer_protos.h>
#include <devices/timer.h>

#include <math.h>
