/*------------------------------------------------------------------------

  The main include files used by the program.  "includes.c" uses this to
  generate a GST of the include files to speed up compilation.

------------------------------------------------------------------------*/


/* Prototypes */
#include <proto/gadtools.h>
#include <proto/alib.h>
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
#define MUIMASTER_NAME "muimaster.library"
#include <libraries/mui.h>
#include <classes/application.h>
#include <classes/area.h>
#include <classes/bitmap.h>
#include <classes/bodychunk.h>
#include <classes/cycle.h>
#include <classes/gauge.h>
#include <classes/list.h>
#include <classes/listview.h>
#include <classes/popstring.h>
#include <classes/virtgroup.h>
#include <classes/window.h>
#include <priv/Image.h>

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
#define __stdargs /* */
#define __chip /* */
#endif

