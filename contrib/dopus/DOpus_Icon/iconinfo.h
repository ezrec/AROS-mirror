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

#include <fctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <proto/all.h>

#include "dopusbase.h"
#include "dopuspragmas.h"
#include "requesters.h"
#include "dopusmessage.h"
#include "stringdata.h"
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
	II_DELETE,                              /* Delete ToolType */
	II_REMAP};                              /* Remap colours 1.3/2.0 */

extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct Library *LayersBase;
extern struct Library *SysBase;

extern char protect_flags[6];
extern char *specific_gadtext[5][6];
extern char *icon_type_names[];
extern char protect_keys[6];

extern struct DefaultString default_strings[];

#define STRING_VERSION 1

#include "functions.h"

struct DOpusStartup {
	struct WBStartup wbstartup;
	int retcode;
};

