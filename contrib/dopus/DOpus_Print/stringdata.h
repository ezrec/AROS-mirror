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

#ifndef STRING_DATA_H
#define STRING_DATA_H

struct DefaultString {
	USHORT string_id;
	char *string;
};

struct StringData {
	struct DefaultString *default_table;
	int string_count;

	char **string_table;

	char *string_buffer;
	ULONG string_size;

	int min_version;
};

#define ID_OSTR MAKE_ID('O','S','T','R')
#define ID_STHD MAKE_ID('S','T','H','D')
#define ID_STRN MAKE_ID('S','T','R','N')

typedef struct StringFile_Header {
	ULONG header_id;         /* STHD */
	ULONG header_size;
	ULONG version;
	ULONG stringcount;
	ULONG flags;
	char  language[20];
} StringHeader;

typedef struct StringFile_String {
	ULONG chunk_id;          /* STRN */
  ULONG chunk_size;
  ULONG string_id;
} String;

#endif
