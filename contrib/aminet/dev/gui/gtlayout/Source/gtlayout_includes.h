/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

#ifndef _GTLAYOUT_INCLUDES_H
#define _GTLAYOUT_INCLUDES_H 1

/*****************************************************************************/

	// We don't want the old declarations

#define INTUI_V36_NAMES_ONLY

#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>

#include <libraries/gadtools.h>

/*****************************************************************************/

#define USE_BUILTIN_MATH
#include <string.h>

/*****************************************************************************/

#ifndef _AROS
#warning _AROS is not defined!
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/exec_protos.h>

#include <pragmas/intuition_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/exec_sysbase_pragmas.h>

#else
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <proto/keymap.h>
#include <intuition/cghooks.h>
#include <aros/asmcall.h>
#endif

/*****************************************************************************/

#endif	// _GTLAYOUT_INCLUDES_H
