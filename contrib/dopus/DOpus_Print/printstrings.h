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

#ifndef PRINT_STRINGS
#define PRINT_STRINGS

enum {
	STR_SELECT_FILE,
	STR_ABORT,
	STR_CONTINUE,
	STR_OKAY,
	STR_CANCEL,
	STR_FILE,
	STR_TOP_MARGIN,
	STR_BOTTOM_MARGIN,
	STR_LEFT_MARGIN,
	STR_RIGHT_MARGIN,
	STR_TAB_SIZE,
	STR_PRINT_PITCH,
	STR_QUALITY,
	STR_EJECT,
	STR_CONFIGURATION,
	STR_TITLE,
	STR_DATE,
	STR_PAGE_NUMBER,
	STR_TEXT_STYLE,
	STR_PRINTER,
	STR_PRINT,
	STR_EXIT,
	STR_OUTPUT_TO,
	STR_PICA,
	STR_ELITE,
	STR_FINE,
	STR_NORMAL,
	STR_BOLD,
	STR_ITALICS,
	STR_UNDERLINED,
	STR_DOUBLESTRIKE,
	STR_SHADOW,
	STR_DRAFT,
	STR_LETTER,
	STR_HEADER,
	STR_FOOTER,
	STR_SIZE,
	STR_PROTECTION,
	STR_COMMENT,
	STR_FILETYPE,
	STR_PRINT_DOT,
	STR_PRINTDIR,
	STR_UNABLE_TO_OPEN_FILE,
	STR_UNABLE_TO_OPEN_OUTPUT,
	STR_UNABLE_TO_OPEN_PRINTER,
	STR_ERROR_PRINTING_DIR,
	STR_INCORRECT_PASSWORD,
	STR_PRINT_ERROR,
	STR_REALLY_ABORT,
	STR_CREATING_FILE,
	STR_OPENING_PRINTER,
	STR_LOADING_FILE,
	STR_PRINTING_FILE,
	STR_PRESS_ESCAPE,
	STR_PAGE,
	STR_COMPLETE,

	STR_STRING_COUNT};

extern char **string_table;

#endif
