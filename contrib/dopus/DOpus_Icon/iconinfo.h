/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include <aros/debug.h>
#if defined(__AROS__)
  #undef  __saveds
  #define __saveds
  #define __chip	__attribute__((section(".data.MEMF_CHIP")))
  #define __aligned	__attribute__((__aligned__(4)))
  #define __asm(A)
  #define __stdargs
  #define __regargs
  #define _exit exit
#elif defined(__PPC__)
  #undef  __saveds
  #define __saveds
  #define __chip
  #define __aligned __attribute__((__aligned__(4)))
  #define lsprintf sprintf
  #define __asm(A)
  #define _exit exit
#endif

//#include <fctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <intuition/iobsolete.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
//#include <proto/all.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/asl.h>
#include <proto/utility.h>
#include <proto/console.h>
#include <proto/locale.h>
#include <proto/dopus.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/workbench.h>
#include <clib/alib_protos.h>

//#include "dopusbase.h"
#include <dopus/dopusbase.h>
#ifndef __AROS__
#include "dopuspragmas.h"
#endif
//#include "requesters.h"
#include <dopus/requesters.h>
#include <dopus/dopusmessage.h>
#include <dopus/stringdata.h>
#include "iconstrings.h"

#define II_TTLINES 5                      /* Number of tooltype lines */

enum {
	II_SAVE,                                /* Save changes to icon */
	II_SKIP,                                /* Leave this icon unchanged */
	II_CANCEL,                              /* Lose changes to icon */
	II_STACK,                               /* Stack size of projects */
	II_COMMENT,                             /* Comment field */
	II_DEFAULTTOOL,                         /* Default tool field */
	II_TOOLTYPE,                            /* ToolType string gadget */
	II_NEW,                                 /* New ToolType */
	II_DELETE};                             /* Delete ToolType */

extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct Library *LayersBase;
//extern struct Library *SysBase;

extern char protect_flags[6];
extern const char *specific_gadtext[5][6];
extern const char *icon_type_names[];
extern char protect_keys[6];

extern struct DefaultString default_strings[];

#define STRING_VERSION 1

#include "functions.h"

struct DOpusStartup {
	struct WBStartup wbstartup;
	int retcode;
};

