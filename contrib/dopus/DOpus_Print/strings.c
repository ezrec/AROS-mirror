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

#include "print.h"

char **string_table;

struct DefaultString
	default_strings[]={
		{STR_SELECT_FILE,
		"Select file"},

		{STR_ABORT,
		"Abort"},

		{STR_CONTINUE,
		"Continue"},

		{STR_OKAY,
		"Okay"},

		{STR_CANCEL,
		"Cancel"},

		{STR_FILE,
		"File"},

		{STR_TOP_MARGIN,
		"Top Margin"},

		{STR_BOTTOM_MARGIN,
		"Bottom Margin"},

		{STR_LEFT_MARGIN,
		"Left Margin"},

		{STR_RIGHT_MARGIN,
		"Right Margin"},

		{STR_TAB_SIZE,
		"Tab size"},

		{STR_PRINT_PITCH,
		"Print pitch"},

		{STR_QUALITY,
		"Quality"},

		{STR_EJECT,
		"Eject final page"},

		{STR_CONFIGURATION,
		"configuration..."},

		{STR_TITLE,
		"Title"},

		{STR_DATE,
		"Date"},

		{STR_PAGE_NUMBER,
		"Page no."},

		{STR_TEXT_STYLE,
		"Text style"},

		{STR_PRINTER,
		"Printer"},

		{STR_PRINT,
		"Print"},

		{STR_EXIT,
		"Exit"},

		{STR_OUTPUT_TO,
		"Output to..."},

		{STR_PICA,
		"Pica"},

		{STR_ELITE,
		"Elite"},

		{STR_FINE,
		"Fine"},

		{STR_NORMAL,
		"Normal"},

		{STR_BOLD,
		"Bold"},

		{STR_ITALICS,
		"Italics"},

		{STR_UNDERLINED,
		"UnLined"},

		{STR_DOUBLESTRIKE,
		"DbStrik"},

		{STR_SHADOW,
		"Shadow"},

		{STR_DRAFT,
		"Draft"},

		{STR_LETTER,
		"Letter"},

		{STR_HEADER,
		"Header"},

		{STR_FOOTER,
		"Footer"},

		{STR_SIZE,
		"Size"},

		{STR_PROTECTION,
		"Protection"},

		{STR_COMMENT,
		"Comment"},

		{STR_FILETYPE,
		"File type"},

		{STR_PRINT_DOT,
		"Print..."},

		{STR_PRINTDIR,
		"Printdir"},

		{STR_UNABLE_TO_OPEN_FILE,
		"Unable to open file. Try again?"},

		{STR_UNABLE_TO_OPEN_OUTPUT,
		"Unable to open output file. Try again?"},

		{STR_UNABLE_TO_OPEN_PRINTER,
		"Unable to open printer. Try again?"},

		{STR_ERROR_PRINTING_DIR,
		"Error printing directory. Try again?"},

		{STR_INCORRECT_PASSWORD,
		"Incorrect password entered. Try again?"},

		{STR_PRINT_ERROR,
		"Print error! Try again?"},

		{STR_REALLY_ABORT,
		"Really abort print operation?"},

		{STR_CREATING_FILE,
		"Creating file"},

		{STR_OPENING_PRINTER,
		"Opening printer"},

		{STR_LOADING_FILE,
		"Loading file.."},

		{STR_PRINTING_FILE,
		"Printing file..."},

		{STR_PRESS_ESCAPE,
		"Press ESC to abort"},

		{STR_PAGE,
		"Page"},

		{STR_COMPLETE,
		"complete"},

		{STR_STRING_COUNT,
		NULL}};

#ifdef STRINGS_ONLY
char string_type[]="DOpus_Print strings";
char *string_save="DM_Print_%s.STR";
#endif
