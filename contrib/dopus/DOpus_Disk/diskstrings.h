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

#ifndef DISK_STRINGS
#define DISK_STRINGS

enum {
	STR_FORMAT_NAME,
	STR_FORMAT_FFS,
	STR_FORMAT_INTERNATIONAL,
	STR_FORMAT_CACHING,
	STR_FORMAT_TRASHCAN,
	STR_FORMAT_VERIFY,
	STR_FORMAT_FORMAT,
	STR_FORMAT_QUICKFORMAT,
	STR_FORMAT_EXIT,
	STR_FORMAT_EMPTY,
	STR_FORMAT_INFODISPLAY,
	STR_FORMAT_FORMATTING,
	STR_FORMAT_VERIFYING,
	STR_FORMAT_INITIALISING,
	STR_FORMAT_FORMATERROR,
	STR_FORMAT_ERRORVERIFYING,
	STR_FORMAT_VERIFYERROR,
	STR_SUCCESS,
	STR_FAILED_ERROR,
	STR_MEMORY_ERROR,
	STR_BITMAP_ERROR,
	STR_DEVICE_ERROR,
	STR_NODEVICE_ERROR,
	STR_RETRY,
	STR_NODISKPRESENT,
	STR_DISKWRITEPROTECTED,
	STR_ABORTED,
	STR_FORMAT_MAKINGTRASHCAN,
	STR_DISK_NOT_BLANK,
	STR_PROCEED,
	STR_CHECKING_DESTINATION,
	STR_DISKCOPY_DISKCOPY,
	STR_DISKCOPY_FROM,
	STR_DISKCOPY_TO,
	STR_DISKCOPY_INFODISPLAY,
	STR_DISKCOPY_NODISK,
	STR_CHECKING_DISKS,
	STR_DISKCOPY_READING,
	STR_DISKCOPY_WRITING,
	STR_DISKCOPY_READERROR,
	STR_DISKCOPY_WRITEERROR,
	STR_DISKCOPY_INSERTSOURCE,
	STR_DISKCOPY_INSERTDEST,
	STR_INSTALL_INSTALL,
	STR_INSTALL_NOBOOT,
	STR_INSTALL_NOINFO,
	STR_INSTALL_NOTBOOTABLE,
	STR_INSTALL_NORMAL,
	STR_INSTALL_NONSTANDARD,
	STR_INSTALL_ERRORWRITING,
	STR_INVALID_DISK,
	STR_INSTALL_INSTALLINGDISK,
	STR_CANCEL,
	STR_BUMP_NAMES,
	STR_BUMPING_NAME,
	STR_DISKCOPY_CHECK,

	STR_STRING_COUNT};

extern char **string_table;

#endif
