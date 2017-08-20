#ifndef DOPUS_STRINGDATA_H
#define DOPUS_STRINGDATA_H

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

/* Structure used by programs internally to define the default strings */

struct DefaultString {
    UWORD string_id;                /* Unique ID number for this string */
    char *string;                    /* Pointer to the string itself */
};


/* Data structure passed to the string routines */

struct StringData {
    const struct DefaultString             /* Pointer to array of default strings */
        *default_table;
    int string_count;                /* Number of strings in the table */

    char **string_table;             /* This will point to the table of
                                        string pointers to actually use */

    char *string_buffer;             /* Buffer of loaded string file */
    ULONG string_size;               /* Size of buffer */

    int min_version;                 /* Minimum version to recognise */

    struct Library *LocaleBase;      // JRZ
    struct Catalog *catalog;         // JRZ
};


/* IFF definitions for the external string file */

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

#endif /* DOPUS_STRINGDATA_H */
