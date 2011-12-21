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

#include "diskop.h"

char **string_table;

struct DefaultString
	default_strings[]={
		{STR_FORMAT_NAME,
		"_Name"},

		{STR_FORMAT_FFS,
		"_Fast File System"},

		{STR_FORMAT_INTERNATIONAL,
		"_International Mode"},

		{STR_FORMAT_CACHING,
		"_Directory Caching"},

		{STR_FORMAT_TRASHCAN,
		"_Put Trashcan"},

		{STR_FORMAT_VERIFY,
		"_Verify"},

		{STR_FORMAT_FORMAT,
		"Format"},

		{STR_FORMAT_QUICKFORMAT,
		"Quick Format"},

		{STR_FORMAT_EXIT,
		"Exit"},

		{STR_FORMAT_EMPTY,
		"Empty"},

		{STR_FORMAT_INFODISPLAY,
		"%ld tracks, %ld bytes/trk, %s"},

		{STR_FORMAT_FORMATTING,
		"Formatting %ld, %ld to go"},

		{STR_FORMAT_VERIFYING,
		"Verifying  %ld, %ld to go"},

		{STR_FORMAT_INITIALISING,
		"Initialising disk..."},

		{STR_FORMAT_FORMATERROR,
		"Format error on track %ld"},

		{STR_FORMAT_ERRORVERIFYING,
		"Error verifying track %ld"},

		{STR_FORMAT_VERIFYERROR,
		"Verify error on track %ld"},

		{STR_SUCCESS,
		"Operation successful"},

		{STR_FAILED_ERROR,
		"Operation failed"},

		{STR_MEMORY_ERROR,
		"Not enough memory"},

		{STR_BITMAP_ERROR,
		"Bitmap error"},

		{STR_DEVICE_ERROR,
		"Device error"},

		{STR_NODEVICE_ERROR,
		"Couldn't open device"},

		{STR_RETRY,
		"Retry"},

		{STR_NODISKPRESENT,
		"No disk present in %s"},

		{STR_DISKWRITEPROTECTED,
		"Disk in %s is write protected"},

		{STR_ABORTED,
		"Aborted..."},

		{STR_FORMAT_MAKINGTRASHCAN,
		"Creating Trashcan..."},

		{STR_DISK_NOT_BLANK,
		"Disk in drive %s is not blank!\n\nVolume \"%s\" contains %s.\n\n"
		"All data will be erased.\nChoose Proceed to continue the %s."},

		{STR_PROCEED,
		"Proceed"},

		{STR_CHECKING_DESTINATION,
		"Checking destination disk..."},

		{STR_DISKCOPY_DISKCOPY,
		"Diskcopy"},

		{STR_DISKCOPY_FROM,
		"From..."},

		{STR_DISKCOPY_TO,
		"To..."},

		{STR_DISKCOPY_INFODISPLAY,
		"%s, %s used"},

		{STR_DISKCOPY_NODISK,
		"No disk info available"},

		{STR_CHECKING_DISKS,
		"Checking disk(s)..."},

		{STR_DISKCOPY_READING,
		"Reading    %ld, %ld to go"},

		{STR_DISKCOPY_WRITING,
		"Writing    %ld, %ld to go"},

		{STR_DISKCOPY_READERROR,
		"Read error on track %ld"},

		{STR_DISKCOPY_WRITEERROR,
		"Write error on track %ld"},

		{STR_DISKCOPY_INSERTSOURCE,
		"Insert source disk in %s"},

		{STR_DISKCOPY_INSERTDEST,
		"Insert destination disk in %s"},

		{STR_INSTALL_INSTALL,
		"Install"},

		{STR_INSTALL_NOBOOT,
		"No Boot"},

		{STR_INSTALL_NOINFO,
		"Bootblock info unavailable"},

		{STR_INSTALL_NOTBOOTABLE,
		"Disk is not bootable"},

		{STR_INSTALL_NORMAL,
		"Normal %s %s bootblock"},

		{STR_INSTALL_NONSTANDARD,
		"Non-standard bootblock"},

		{STR_INSTALL_ERRORWRITING,
		"Error writing bootblock!"},

		{STR_INVALID_DISK,
		"Disk is unsuitable for installing"},

		{STR_INSTALL_INSTALLINGDISK,
		"Installing disk..."},

		{STR_CANCEL,
		"Cancel"},

		{STR_BUMP_NAMES,
		"_Bump name"},

		{STR_BUMPING_NAME,
		"Bumping disk name..."},

		{STR_DISKCOPY_CHECK,
		"Check"},

		{STR_STRING_COUNT,NULL}};

#ifdef STRINGS_ONLY
char string_type[]="DOpus_Disk strings";
char *string_save="DM_Disk_%s.STR";
#endif
