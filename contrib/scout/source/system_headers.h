/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 * @author Pavel Fedin <sonic_amiga@mail.ru>
 */

#ifndef _SYSTEM_HEADERS_H
#define _SYSTEM_HEADERS_H 1

#define __USE_SYSBASE

/*****************************************************************************/

#include <exec/types.h>

/*****************************************************************************/

#define USE_BUILTIN_MATH 1
#include <string.h>

/*****************************************************************************/

#if defined(__AROS__)
#include <aros/asmcall.h>
#define MUIMASTER_YES_INLINE_STDARG
#define MUI_OBSOLETE
#define HAVE_MONITORCLASS
#endif
#ifdef __MORPHOS__
#define HAVE_MONITORCLASS
#endif
#include <devices/ahi.h>
#include <devices/audio.h>
#include <devices/cd.h>
#include <devices/clipboard.h>
#include <devices/console.h>
#include <devices/gameport.h>
#include <devices/input.h>
#include <devices/keyboard.h>
#include <devices/newstyle.h>
#include <devices/parallel.h>
#include <devices/printer.h>
#include <devices/sana2.h>
#include <devices/sana2r5.h>
#include <devices/scsidisk.h>
#include <devices/serial.h>
#include <devices/timer.h>
#include <devices/trackdisk.h>
#include <devices/newmouse.h>
#include <dos/datetime.h>
#include <dos/dos.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <dos/rdargs.h>
#if !defined(__amigaos4__) && !defined(__AROS__)
    #include <dos/dos_private.h>
#endif
#include <exec/alerts.h>
#include <exec/execbase.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#if defined(__AROS__)
#include <exec/memheaderext.h>
#endif
#include <exec/ports.h>
#include <exec/resident.h>
#include <exec/semaphores.h>
#include <exec/tasks.h>
#if defined(__AROS__)
    #include <exec/errors.h>
#endif
#include <graphics/displayinfo.h>
#include <graphics/gfx.h>
#include <graphics/gfxbase.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxnodes.h>
#include <graphics/monitor.h>
#if !defined(__amigaos4__)
    #include <cybergraphx/cybergraphics.h>
#endif
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>
#include <intuition/sghooks.h>
#ifdef HAVE_MONITORCLASS
#include <intuition/monitorclass.h>
#endif
#include <libraries/commodities.h>
#if !defined(__AROS__)
    #include <libraries/commodities_private.h>
#endif
#include <libraries/configvars.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <libraries/identify.h>
#include <libraries/locale.h>
#include <libraries/mui.h>
#if defined(__MORPHOS__)
    #include <libraries/openpci.h>
#endif
#include <libraries/usergroup.h>
#if !defined(__amigaos4__)
    #include <libraries/disassembler.h>
#endif
#include <mui/NListtree_mcc.h>
#include <mui/NListview_mcc.h>
#include <mui/NFloattext_mcc.h>
#include <mui/NList_mcc.h>
#include <mui/Urltext_mcc.h>
#include <resources/card.h>
#include <resources/cia.h>
#include <resources/disk.h>
#include <resources/filesysres.h>
#include <resources/misc.h>
#include <rexx/rxslib.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#if defined(__GNUC__)
    #include <sys/time.h>
#endif
#include <sys/types.h>
#if defined(__amigaos4__)
    #include <usb/system.h>
#endif
#include <utility/tagitem.h>
#include <utility/utility.h>
#include <workbench/icon.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#if defined(__MORPHOS__)
    #include <exec/system.h>
    #include <libraries/ppcdiss.h>
#endif
#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
    #include <lvo/exec_lvo.h>
    #include <mmu/mmubase.h>
    #include <mmu/mmutags.h>
#endif

#include <netinet/in.h>

#include <clib/alib_protos.h>
#include <clib/macros.h>
#include <proto/ahi.h>
#include <proto/asl.h>
#include <proto/cia.h>
#include <proto/commodities.h>
#if !defined(__amigaos4__)
    #include <proto/disassembler.h>
#endif
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/layers.h>
#include <proto/locale.h>
#include <proto/misc.h>
#include <proto/muimaster.h>
#if defined(__MORPHOS__)
    #include <proto/ppcdiss.h>
    #include <proto/openpci.h>
    #include <proto/pciids.h>
#endif
#include <proto/rexxsyslib.h>
#include <proto/socket.h>
#include <proto/timer.h>
#include <proto/usergroup.h>
#if defined(__amigaos4__)
    #include <proto/usbsys.h>
#endif
#include <proto/utility.h>
#if !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	#ifndef PATH_MAX
		#define PATH_MAX 1024
	#endif
    #include <proto/mmu.h>
    #include <dos.h>    // for getreg(), etc
#endif

#include <ctype.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

#include <tblib.h>

#if defined(__MORPHOS__)
#define USE_INLINE_STDARG
#endif
#include <proto/identify.h>

#include "scout_cat.h"
#include "scout_defs.h"
#include "scout_logo.h"
#include "scout_net.h"
#include "scout.h"
#include "scout_tags.h"
#include "startup_defs.h"
#include "startup.h"
#include "SmartReadArgs.h"

#include "scout_asm.h"
#include "scout_extras.h"

#include "fontdisplay_class.h"
#include "scout_about.h"
#include "scout_allocations.h"
#include "scout_arexx.h"
#include "scout_assigns.h"
#include "scout_audiomode.h"
#include "scout_audiomode_detail.h"
#include "scout_catalogs.h"
#include "scout_classes.h"
#include "scout_classes_detail.h"
#include "scout_commands.h"
#include "scout_commodity.h"
#include "scout_commodity_detail.h"
#include "scout_customclasses.h"
#include "scout_devices.h"
#include "scout_devices_detail.h"
#include "scout_expansions.h"
#include "scout_expansions_detail.h"
#include "scout_flags.h"
#include "scout_fonts.h"
#include "scout_fonts_detail.h"
#include "scout_functions.h"
#include "scout_inputhandlers.h"
#include "scout_inputhandlers_detail.h"
#include "scout_interrupts.h"
#include "scout_interrupts_detail.h"
#include "scout_libraries.h"
#include "scout_libraries_detail.h"
#include "scout_locks.h"
#include "scout_lowmemory.h"
#include "scout_lowmemory_detail.h"
#include "scout_main.h"
#include "scout_memory.h"
#include "scout_memory_detail.h"
#include "scout_mount.h"
#include "scout_mount_detail.h"
#include "scout_oop.h"
#include "scout_oop_detail.h"
#include "scout_parentwindow.h"
#include "scout_patches.h"
#include "scout_ports.h"
#include "scout_ports_detail.h"
#include "scout_print.h"
#include "scout_priority.h"
#include "scout_resethandlers.h"
#include "scout_resethandlers_detail.h"
#include "scout_residents.h"
#include "scout_residents_detail.h"
#include "scout_resources.h"
#include "scout_resources_detail.h"
#include "scout_screenmode.h"
#include "scout_screenmode_detail.h"
#include "scout_semaphores.h"
#include "scout_signal.h"
#include "scout_system.h"
#include "scout_tasks.h"
#include "scout_tasks_detail.h"
#include "scout_timer.h"
#include "scout_vectors.h"
#include "scout_version.h"
#include "scout_windows.h"
#include "scout_windows_detail.h"
#include "scout_disassembler.h"
#include "disassembler_button.h"
#include "flags_button.h"
#include "task_button.h"
#include "port_button.h"
#include "scout_monitors.h"
#include "scout_monitors_detail.h"
#include "monitor_button.h"
#include "scout_monitorclass.h"
#include "scout_monitorclass_detail.h"

#if defined(__GNUC__)
    #define USE_NATIVE_64BIT_MATH          1
	#ifndef int64
		typedef long long int64;
	#endif
	#ifndef uint64
		typedef unsigned long long uint64;
	#endif
#else
    #undef USE_NATIVE_64BIT_MATH
    #include "i64.h"
#endif

#if !defined(__amigaos4__)
    #include <showpatch.h>
    #include <setman.h>
    #ifndef __AROS__
    #include <patchcontrol.h>
    #endif
    #if !defined(__MORPHOS__) && !defined(__GNUC__)
    #include <pragmas/cia_private_pragmas.h>
    #endif
#endif

#include "hexdump.h"
#include "extra.h"

#if !defined(__amigaos4__) //|| !defined(__NEW_TIMEVAL_DEFINITION_USED__)
#define TIMEVAL(x)  (struct timeval *)(x)
#else
#define TIMEVAL(x)  (x)
#endif

/*****************************************************************************/

#endif /* _SYSTEM_HEADERS_H */
