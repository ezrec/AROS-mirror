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

/* Flags relating to configuration options */

#ifndef DOPUS_CONFIGFLAGS
#define DOPUS_CONFIGFLAGS

/* CopyFlags */

#define COPY_DATE  1
#define COPY_PROT  2
#define COPY_NOTE  4
#define COPY_ARC   8
#define COPY_CHECK 16

/* DeleteFlags */

#define DELETE_ASK    1
#define DELETE_FILES  2
#define DELETE_DIRS   4
#define DELETE_SET    8

/* ErrorFlags */

#define ERROR_ENABLE_DOS  1
#define ERROR_ENABLE_OPUS 2

/* GeneralFlags */

#define GENERAL_DISPLAYINFO 2
#define GENERAL_DOUBLECLICK 8
#define GENERAL_ACTIVATE    64
#define GENERAL_DRAG        128

/* IconFlags */

#define ICONFLAG_MAKEDIRICON   1
#define ICONFLAG_DOUNTOICONS   2
#define ICONFLAG_AUTOSELECT    4

/* ReplaceFlags */

#define REPLACE_ALWAYS 1
#define REPLACE_NEVER  2
#define REPLACE_OLDER  4
#define REPLACE_ASK    8

/* SortFlags */

#define SORT_LREVERSE    1
#define SORT_RREVERSE    2

/* UpdateFlags */

#define UPDATE_FREE               1
#define UPDATE_SCROLL             2
#define UPDATE_REDRAW             4
#define UPDATE_NOTIFY             8
#define UPDATE_LEFTJUSTIFY       16
#define UPDATE_PROGRESSINDICATOR 32
#define UPDATE_PROGRESSIND_COPY  64

/* ScreenFlags */

#define SCRFLAGS_DEFWIDTH   1
#define SCRFLAGS_DEFHEIGHT  2
#define SCRFLAGS_HALFHEIGHT 4

/* DirFlags */

#define DIRFLAGS_FINDEMPTY      1
#define DIRFLAGS_EXALL          2
#define DIRFLAGS_AUTODISKC      4
#define DIRFLAGS_AUTODISKL      8
#define DIRFLAGS_CHECKBUFS     16
#define DIRFLAGS_REREADOLD     32
#define DIRFLAGS_SMARTPARENT   64
#define DIRFLAGS_EXPANDPATHS  128

/* ViewBits */

#define VIEWBITS_SHOWBLACK   1
#define VIEWBITS_FILTEROFF   2
#define VIEWBITS_8BITSPERGUN 4
#define VIEWBITS_PLAYLOOP    8
#define VIEWBITS_TEXTBORDERS 16
#define VIEWBITS_PAUSEANIMS  32
#define VIEWBITS_BESTMODEID  64

/* IconType */

#define ICON_MEMORY    1
#define ICON_CPU       2
#define ICON_DATE      4
#define ICON_TIME      8
#define ICON_NOWINDOW  16
#define ICON_APPICON   32
#define ICON_BYTES     64
#define ICON_C_AND_F   128

/* ScrClkType */

#define SCRCLOCK_MEMORY    1
#define SCRCLOCK_CPU       2
#define SCRCLOCK_DATE      4
#define SCRCLOCK_TIME      8
#define SCRCLOCK_BYTES     64
#define SCRCLOCK_C_AND_F   128

/* ShowFree */

#define SHOWFREE_BYTES   1
#define SHOWFREE_KILO    2
#define SHOWFREE_BLOCKS  4
#define SHOWFREE_PERCENT 8

/* DateFormat */

#define DATE_DOS   1
#define DATE_INT   2
#define DATE_USA   4
#define DATE_AUS   8
#define DATE_SUBST 16
#define DATE_12HOUR 32

/* AddIconFlags */

#define ADDICONFLAGS_USEDEFAULTICONS 1

/* GeneralScreenFlags */

#define SCR_GENERAL_WINBORDERS  1
#define SCR_GENERAL_TINYGADS    2
#define SCR_GENERAL_GADSLIDERS  4
#define SCR_GENERAL_INDICATERMB 8
#define SCR_GENERAL_NEWLOOKPROP 16
#define SCR_GENERAL_REQDRAG     32
#define SCR_GENERAL_NEWLOOKMENU 64
#define SCR_GENERAL_TITLESTATUS 128

/* LoadExternal */

#define LOAD_DISK    1
#define LOAD_PRINT   2
#define LOAD_ICON    4
#define LOAD_CONFIG  65536

/* HotkeyFlags */

#define HOTKEY_USEMMB 1

#endif
