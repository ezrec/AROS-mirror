/*------------------------------------------------------------------------

  The main include files used by the program.  "includes.c" uses this to
  generate a GST of the include files to speed up compilation.

------------------------------------------------------------------------*/


/* Prototypes */
#include <proto/gadtools.h>
#ifdef __AROS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include "proto/muimaster.h"
#include <proto/diskfont.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/layers.h>
#include <proto/datatypes.h>

#include <exec/types.h>
#include <exec/memory.h>

/* MUI */
#define MUI_OBSOLETE

#ifdef USE_ZUNE
#include <mui.h>
#else
#include <libraries/mui.h>
#endif

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
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>
#include <datatypes/textclass.h>

/* ANSI C */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <SDI/SDI_compiler.h>
#include <SDI/SDI_hook.h>

#ifdef __AROS__
#define __far /* */
#define __regargs /* */
#define __stdargs /* */
#define __chip /* */
#endif

