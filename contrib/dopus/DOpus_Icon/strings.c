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

#include "iconinfo.h"

char **string_table;

struct DefaultString
	default_strings[]={
		{STR_REMAP_COLORS,
		"Remap colors"},

		{STR_SAVE,
		"Save"},

		{STR_SKIP,
		"Skip"},

		{STR_CANCEL,
		"Cancel"},

		{STR_DELETE,
		"Delete"},

		{STR_NEW,
		"New"},

		{STR_TOOLTYPES,
		"Tool types:"},

		{STR_COMMENT,
		"Comment:"},

		{STR_DEFAULT_TOOL,
		"Default tool:"},

		{STR_VALIDATING,
		"Validating"},

		{STR_WRITE_PROTECTED,
		"Write protected"},

		{STR_READ_WRITE,
		"Read/Write"},

		{STR_SCRIPT,
		"_Script"},

		{STR_ARCHIVED,
		"_Archived"},

		{STR_READABLE,
		"_Readable"},

		{STR_WRITABLE,
		"_Writable"},

		{STR_EXECUTABLE,
		"_Executable"},

		{STR_DELETABLE,
		"_Deletable"},

		{STR_BLOCKS,
		"Blocks"},

		{STR_USED,
		"Used"},

		{STR_FREE,
		"Free"},

		{STR_BLOCK_SIZE,
		"Block size"},

		{STR_CREATED,
		"Created"},

		{STR_LAST_CHANGED,
		"Last changed"},

		{STR_BYTES,
		"Bytes"},

		{STR_STACK,
		"Stack"},

		{STR_DISK,
		"Disk"},

		{STR_DRAWER,
		"Drawer"},

		{STR_TOOL,
		"Tool"},

		{STR_PROJECT,
		"Project"},

		{STR_TRASH,
		"Trash"},

		{STR_DEVICE,
		"Device"},

		{STR_KICK,
		"Kick"},

		{STR_APPICON,
		"AppIcon"},

		{STR_ICON,
		"Icon"},

		{STR_STRING_COUNT,
		NULL}};

#ifdef STRINGS_ONLY
char string_type[]="DOpus_Print strings";
char *string_save="DM_Icon_%s.STR";
#endif
