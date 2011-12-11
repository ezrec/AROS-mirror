/*
**	Headers.h
**
**	Definition file for precompiled header file
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _HEADERS_H
#define _HEADERS_H 1

	/* The compiler specific definitions */

#include "Compiler.h"

	/* System includes (just a few). */

#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/sghooks.h>
#include <intuition/cghooks.h>
#include <intuition/classes.h>
#include <intuition/icclass.h>

#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>

#include <workbench/workbench.h>

#include <libraries/commodities.h>
#ifndef __AROS__
#include <libraries/translator.h>
#endif
#include <libraries/amigaguide.h>
#include <libraries/gadtools.h>
#include <libraries/iffparse.h>

#include <graphics/videocontrol.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/rpattr.h>
#include <graphics/scale.h>

#include <libraries/locale.h>
#include <libraries/asl.h>

#include <devices/inputevent.h>
#include <devices/clipboard.h>
#include <devices/narrator.h>
#include <devices/console.h>
#include <devices/conunit.h>
#include <devices/serial.h>

#ifdef __AROS__
// FIXME: Some AROS-specific defines that need to go into other files
#define SER_STATF_COMCD		(1<<1)
#define SER_STATF_COMDSR	(1<<2)
#define SER_STATF_COMRTS	(1<<3)
#define SER_STATF_COMCTS	(1<<4)
#define SER_STATF_COMDTR	(1<<5)

#endif

#include <devices/audio.h>
#include <devices/input.h>
#include <devices/timer.h>

#include <hardware/intbits.h>
#include <hardware/blit.h>
#include <hardware/cia.h>

#include <dos/filehandler.h>
#include <dos/dosextens.h>
#include <dos/datetime.h>
#include <dos/doshunks.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>
#include <dos/dosasl.h>
#include <dos/exall.h>
#include <dos/stdio.h>
#include <dos/var.h>

#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/devices.h>
#include <exec/memory.h>
#include <exec/errors.h>

#include <prefs/prefhdr.h>
#include <prefs/serial.h>
#include <prefs/font.h>

#include <utility/date.h>

#include <clib/macros.h>

#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <clib/commodities_protos.h>
#ifndef __AROS__
#include <clib/translator_protos.h>
#endif
#include <clib/rexxsyslib_protos.h>
#include <clib/amigaguide_protos.h>
#include <clib/intuition_protos.h>
#include <clib/datatypes_protos.h>
#include <clib/graphics_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/iffparse_protos.h>
#include <clib/diskfont_protos.h>
#include <clib/console_protos.h>
#include <clib/utility_protos.h>
#include <clib/keymap_protos.h>
#include <clib/layers_protos.h>
#include <clib/locale_protos.h>
#include <clib/timer_protos.h>
#include <clib/alib_protos.h>
#include <clib/icon_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/asl_protos.h>
#include <clib/wb_protos.h>

#ifndef NO_PRAGMAS
#include <pragmas/commodities_pragmas.h>
#include <pragmas/translator_pragmas.h>
#include <pragmas/rexxsyslib_pragmas.h>
#include <pragmas/amigaguide_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/datatypes_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/iffparse_pragmas.h>
#include <pragmas/diskfont_pragmas.h>
#include <pragmas/console_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/keymap_pragmas.h>
#include <pragmas/layers_pragmas.h>
#include <pragmas/locale_pragmas.h>
#include <pragmas/timer_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/wb_pragmas.h>
#endif	/* NO_PRAGMAS */


#ifdef __GNUC__
#ifdef __AROS__
#include <proto/commodities.h>
//#include <proto/translator.h>
#include <proto/rexxsyslib.h>
#include <proto/amigaguide.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/iffparse.h>
#include <proto/diskfont.h>
#include <proto/console.h>
#include <proto/utility.h>
#include <proto/keymap.h>
#include <proto/layers.h>
#include <proto/locale.h>
#include <proto/timer.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/asl.h>
#include <proto/wb.h>
#else
#include <inline/commodities.h>
#include <inline/translator.h>
#include <inline/rexxsyslib.h>
#include <inline/amigaguide.h>
#include <inline/intuition.h>
#include <inline/datatypes.h>
#include <inline/graphics.h>
#include <inline/gadtools.h>
#include <inline/iffparse.h>
#include <inline/diskfont.h>
#include <inline/console.h>
#include <inline/utility.h>
#include <inline/keymap.h>
#include <inline/layers.h>
#include <inline/locale.h>
#include <inline/timer.h>
#include <inline/icon.h>
#include <inline/exec.h>
#include <inline/dos.h>
#include <inline/asl.h>
#include <inline/wb.h>
#endif
#endif	/* __GNUC__ */

#ifdef __AROS__
#undef AllocVecPooled
#undef FreeVecPooled
#endif

	/* Standard "C" includes. */

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>

	/* Include string IDs. */

#define CATCOMP_NUMBERS 1
#include "Strings.h"

	/* Include the OwnDevUnit definitions. */

#include "OwnDevUnit.h"

	/* Include the XPR definitions. */

#ifdef __AROS__
#include <proto/xpr.h>
#else
#include "xproto.h"
#endif

	/* Include the XEM definitions. */

#ifdef __AROS__
#include <xem.h>
#include <proto/xem.h>
#else
#include "xem.h"
#endif

	/* Include the rendezvous interface definitions. */

#include "Rendezvous.h"

	/* The user interface definitions. */

#ifndef __AROS__
#include "gtlayout.h"
#include "gtlayout_protos.h"
#else
#include <gtlayout.h>
#include <proto/gtlayout.h>
#endif

#ifndef NO_PRAGMAS
#include "gtlayout_pragmas.h"
#endif	/* NO_PRAGMAS */

	/* The phonelog datatypes and prototypes. */

#include "PhoneLogGenerator.h"

	/* If desired, include the global definitions and prototypes as well. */

#ifdef INCLUDEALL
#include "Global.h"
#endif	/* INCLUDEALL */

#endif	/* _HEADERS_H */
