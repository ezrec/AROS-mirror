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
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <datatypes/datatypesclass.h>
#include <proto/all.h>

#include "dopusbase.h"
#include "dopuspragmas.h"
#include "requesters.h"
#include "dopusmessage.h"
#include "stringdata.h"
#include "printstrings.h"

extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct Library *WorkbenchBase;
extern struct Library *PPBase;

struct Directory {
	struct Directory *last,*next;
	char name[32];
	int type;
	int size;
	int subtype;
	int protection;
	char *comment;
	char *dispstr;
	char protbuf[12],datebuf[20];
	int selected;
	char *description;
	int userdata,userdata2;
	struct DateStamp date;
};

#define PRINTFLAG_EJECT    1
#define PRINTFLAG_FILE     2

#define HEADFOOTFLAG_TITLE 1
#define HEADFOOTFLAG_DATE  2
#define HEADFOOTFLAG_PAGE  4

#define PITCH_PICA         0
#define PITCH_ELITE        1
#define PITCH_FINE         2

#define PITCH_COUNT        3

#define STYLE_NORMAL       0
#define STYLE_BOLD         1
#define STYLE_ITALICS      2
#define STYLE_UNDERLINED   3
#define STYLE_DOUBLESTRIKE 4
#define STYLE_SHADOW       5

#define STYLE_COUNT        6

#define QUALITY_DRAFT      0
#define QUALITY_NLQ        1

#define HEADER             0
#define FOOTER             1

typedef struct HeadFootData {
	char text_style;
	char headfoot_flags;
	char headfoot_title[40];
} HeaderFooter;

typedef struct MyPrinterData {
	USHORT top_margin;
	USHORT bottom_margin;
	USHORT left_margin;
	USHORT right_margin;
	char tab_size;
	char print_pitch;
	char text_quality;
	char print_flags;
	char output_file[256];
	HeaderFooter headfoot[2];
} PrintData;

#define DOUBLESTRIKE_OFF "\x1b[3\"z"
#define SHADOW_OFF       "\x1b[5\"z"

extern struct DefaultString default_strings[];

#define STRING_VERSION 1

#include "functions.h"
