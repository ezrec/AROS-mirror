/*------------------------------------------------------------------------

  The main include files used by the program.  "includes.c" uses this to
  generate a GST of the include files to speed up compilation.

------------------------------------------------------------------------*/

#include <exec/types.h>
#include <exec/memory.h>

/* MUI */
#include <libraries/mui.h>

/* System */
#include <dos/dos.h>
#include <graphics/gfxmacros.h>
#include <workbench/workbench.h>
#include <exec/tasks.h>
#include <intuition/intuitionbase.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/cghooks.h>
#include <intuition/icclass.h>
#include <libraries/gadtools.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>
#include <datatypes/textclass.h>

/* Prototypes */
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include "clib/muimaster_protos.h"
#include <clib/diskfont_protos.h>
#include <clib/asl_protos.h>
#include <clib/utility_protos.h>
#include <clib/layers_protos.h>
#include <clib/datatypes_protos.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* Pragmas */
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include "pragmas/muimaster_pragmas.h"
#include <pragmas/asl_pragmas.h>
#include <pragmas/datatypes_pragmas.h>

#ifndef __AROS__
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#else
#define REG(x) /* */
#define ASM /* */
#define SAVEDS /* */
#define __far /* */
#define __regargs /* */
#endif

