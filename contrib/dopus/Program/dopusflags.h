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

/* General-purpose flags and defines */

#ifndef DOPUS_FLAGS
#define DOPUS_FLAGS

/* Search Flags */

#define SEARCH_NOCASE     1
#define SEARCH_WILDCARD   2
#define SEARCH_ONLYWORDS  4

/* AddIcon Icon Types */

#define ICONTYPE_DRAWER  0
#define ICONTYPE_TOOL    1
#define ICONTYPE_PROJECT 2
#define ICONTYPE_TRASH   3

/* Font Positions */

enum {
	FONT_GENERAL,
	FONT_DIRS,
	FONT_TEXT,
	FONT_GADGETS,
	FONT_MENUS,
	FONT_STATUS,
	FONT_NAMES,
	FONT_CLOCK,
	FONT_REQUEST,
	FONT_STRING,
	FONT_ICONIFY,
	FONT_SCREEN,

	FONT_LAST};

/* List Format Display IDs */

#define DISPLAY_NAME     0
#define DISPLAY_SIZE     1
#define DISPLAY_PROTECT  2
#define DISPLAY_DATE     3
#define DISPLAY_COMMENT  4
#define DISPLAY_FILETYPE 5
#define DISPLAY_OWNER    6
#define DISPLAY_GROUP    7
#define DISPLAY_NETPROT  8

#define DISPLAY_LAST     8

/* Separation Methods */

#define SEPARATE_MIX        0
#define SEPARATE_DIRSFIRST  1
#define SEPARATE_FILESFIRST 2

/* Modify Parameters */

enum {
	MOD_COPYFLAGS,
	MOD_DELETEFLAGS,
	MOD_ERRORFLAGS,
	MOD_LISTFORMAT,
	MOD_GENERALFLAGS,
	MOD_ICONFLAGS,
	MOD_REPLACEFLAGS,
	MOD_SORTFLAGS,
	MOD_UPDATEFLAGS,
	MOD_OUTPUTCMD,
	MOD_OUTPUTWINDOW,
	MOD_SCRDEPTH,
	MOD_SCREENFLAGS,
	MOD_SCREENMODE,
	MOD_SCRW,
	MOD_SCRH,
	MOD_DIRFLAGS,
	MOD_HELPFILE,
	MOD_DEFAULTTOOL,
	MOD_SHOWDELAY,
	MOD_VIEWPLAYFLAGS,
	MOD_FADETIME,
	MOD_HIDDENBIT,
	MOD_SHOWPATTERN,
	MOD_HIDEPATTERN,
	MOD_ICONIFYFLAGS,
	MOD_SCREENCLOCKFLAGS,
	MOD_SHOWFREEFLAGS,
	MOD_FONT,
	MOD_BANKNUMBER,
	MOD_DATEFORMAT,
	MOD_GADGETROWS,
	MOD_DISPLAYLENGTH,
	MOD_WINDOWDELTA,
	MOD_SORTMETHOD,
	MOD_SEPARATEMETHOD,
	MOD_FILTER,
	MOD_WINDOWXY,
	MOD_WINDOWSIZE,
	MOD_PUBSCREEN,
	MOD_WINDOWXYWH,
	MOD_SCREENNAME,
	MOD_PORTNAME
};

/* DOpus NewScreen Tags */

#define SCREENTAGS_DISPLAYID   0
#define SCREENTAGS_PENS        1
#define SCREENTAGS_PUBNAME     2
#define SCREENTAGS_OVERSCAN    3
#define SCREENTAGS_AUTOSCROLL  4
#define SCREENTAGS_INTERLEAVED 5

/* System-Signals Recognised By DOpus */

#define INPUTSIG_ABORT      SIGBREAKF_CTRL_C
#define INPUTSIG_TITLECLICK SIGBREAKF_CTRL_D
#define INPUTSIG_UNICONIFY  SIGBREAKF_CTRL_E
#define INPUTSIG_HOTKEY     SIGBREAKF_CTRL_F

/* Positioning Of Status Text */

#define TOPTEXT_CENTER 0
#define TOPTEXT_LEFT   1
#define TOPTEXT_RIGHT  2

/* Flags for startgetdir() */

#define SGDFLAGS_CANMOVEEMPTY 1
#define SGDFLAGS_CANCHECKBUFS 2
#define SGDFLAGS_REREADINGOLD 4

#endif
