#include <intuition/intuitionbase.h>
#include <libraries/commodities.h>
#include <intuition/gadgetclass.h>
#include <libraries/expansion.h>
#include <libraries/gadtools.h>
#include <devices/trackdisk.h>
#include <graphics/gfxbase.h>
#include <devices/keyboard.h>
#include <libraries/locale.h>
#include <dos/filehandler.h>
#include <exec/interrupts.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <exec/devices.h>
#ifndef _AROS
#include <rexx/storage.h>
#include <rexx/rxslib.h>
#include <rexx/errors.h>
#endif
#include <dos/dostags.h>
#include <exec/memory.h>

#include <clib/commodities_protos.h>
#ifndef _AROS
#include <clib/rexxsyslib_protos.h>
#endif
#include <clib/intuition_protos.h>
#include <clib/expansion_protos.h>
#include <clib/graphics_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/locale_protos.h>
#include <clib/exec_protos.h>
#include <clib/alib_protos.h>
#include <clib/dos_protos.h>

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#ifndef _AROS
#include <dos.h>
#endif

#include "ShutdownExtra.h"
