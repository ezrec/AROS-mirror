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
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <intuition/intuitionbase.h>
#include <intuition/sghooks.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/trackdisk.h>
#include <datatypes/datatypesclass.h>
#include <proto/all.h>

#include "dopusbase.h"
#include "dopuspragmas.h"
#include "requesters.h"
#include "stringdata.h"

#include "dopusmessage.h"

#include "functions.h"
#include "diskstrings.h"

extern struct DOpusBase *DOpusBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *IconBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;

#define ERROR_FAILED  -1
#define ERROR_MEMORY  -2
#define ERROR_BITMAP  -3
#define ERROR_VERIFY  -4
#define ERROR_ABORTED -5

struct BitmapBlock {
	ULONG Checksum;
	ULONG Bitmap[127];
};

struct BitmapExtension {
	ULONG BitmapPointers[127];
	ULONG	BitmapExtension;
};

struct RootDirectory {
	ULONG	PrimaryType;
	ULONG	HeaderKey;
	ULONG	WhoKnows0;
	ULONG	HashTableSize;
	ULONG	WhoKnows1;
	ULONG	Checksum;
	ULONG HashTable[72];
	ULONG	BitmapFlag;
	ULONG	BitmapPointers[25];
	ULONG	BitmapExtension;
	struct DateStamp LastRootChange;
	UBYTE	DiskName[32];
	ULONG	WhoKnows2[2];
	struct DateStamp LastDiskChange;
	struct DateStamp CreationDate;
	ULONG	NextHash;
	ULONG	ParentDir;
	ULONG	WhoKnows3;
	ULONG	SecondaryType;
};

extern ULONG BitTable[32];

extern struct TagItem format_name_gadget[];
extern struct TagItem format_ffs_gadget[];
extern struct TagItem format_international_gadget[];
extern struct TagItem format_caching_gadget[];
extern struct TagItem format_trashcan_gadget[];
extern struct TagItem format_verify_gadget[];
extern struct TagItem format_format_gadget[];
extern struct TagItem format_quickformat_gadget[];
extern struct TagItem format_cancel_gadget[];

extern struct TagItem diskcopy_verify_gadget[];
extern struct TagItem diskcopy_bumpnames_gadget[];
extern struct TagItem diskcopy_diskcopy_gadget[];
extern struct TagItem diskcopy_cancel_gadget[];
extern struct TagItem diskcopy_source_list[];
extern struct TagItem diskcopy_destination_list[];

extern struct TagItem install_ffs_gadget[];
extern struct TagItem install_install_gadget[];
extern struct TagItem install_noboot_gadget[];
extern struct TagItem install_cancel_gadget[];

extern struct DiskObject trashcanicon_icon;

extern ULONG bootblock_13[13],bootblock_20[24];

struct DeviceHandle {
	struct MsgPort *device_port;
	struct IOExtTD *device_req;
	struct FileSysStartupMsg *startup;
	struct DosEnvec *dosenvec;
};

extern struct DefaultString default_strings[];

#define STRING_VERSION 1
