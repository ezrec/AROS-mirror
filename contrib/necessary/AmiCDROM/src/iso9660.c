/* iso9660.c:
 *
 * Support for the ISO-9660 filing system.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 20-Aug-94   fmu   Uses function Find_Last_Session() before the traversal
 *                   of the TOC.
 * 17-Jul-94   fmu   Tries to cope with CDROMs which have an incorrect TOC.
 * 17-May-94   fmu   New option MAYBELOWERCASE (=ML).
 * 25-Apr-94   fmu   The extented attribute record length has to be
 *                   considered when reading file sections.
 * 17-Feb-94   fmu   Volume ID must not be longer than 30 characters.
 * 05-Feb-94   fmu   Added support for relocated directories.
 * 07-Jan-94   fmu   Support for drives which don't support the SCSI-2
 *                   READ TOC command.
 * 01-Jan-94   fmu   Added multisession support.
 * 11-Dec-93   fmu   Fixed bug in Iso_Find_Parent().
 * 02-Dec-93   fmu   Bugfix: a logical block of a file extent must not
 *                   necessarily start at a logical sector border.
 * 29-Nov-93   fmu   - New function Iso_Block_Size().
 *                   - Support for variable logical block sizes.
 * 15-Nov-93   fmu   Uses_High_Sierra_Protocol added.
 * 13-Nov-93   fmu   Bad iso_errno return value in Iso_Open_Obj_In_Directory
 *                   corrected.
 * 12-Oct-93   fmu   Adapted to new VOLUME and CDROM_OBJ structures.
 * 24-Sep-93   fmu   Two further bugs in Seek_Position fixed.
 * 16-Sep-93   fmu   Fixed bug in Seek_Position.
 * 16-Sep-93   fmu   Bugfix: Top level object recognition in CDROM_Info
 *                   had to be changed for Rock Ridge disks.
 * 07-Jul-02 sheutlin  various changes when porting to AROS
 *                     - global variables are now in a struct Globals *global
 */


#include <proto/exec.h>
#include <proto/utility.h>
#ifdef __AROS__
#include <aros/debug.h>
#endif
#include <exec/types.h>
#include <exec/memory.h>
#include <utility/date.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cdrom.h"
#include "iso9660.h"
#include "rock.h"
#include "globals.h"

#include "clib_stuff.h"

t_bool Iso_Is_Top_Level_Object (CDROM_OBJ *);

#define VOL(vol,tag) (((t_iso_vol_info *)(vol->vol_info))->tag)
#define OBJ(obj,tag) (((t_iso_obj_info *)(obj->obj_info))->tag)

extern struct Globals *global;

#ifdef SysBase
#	undef SysBase
#endif
#define SysBase global->SysBase
#ifdef UtilityBase
#	undef UtilityBase
#endif
#define UtilityBase global->UtilityBase

/* Check whether the given volume uses the ISO 9660 Protocol.
 * The protocol is identified by the sequence
 *            'C' 'D' '0' '0' '1'
 * in the 2nd..6th byte of sector 16 of a track.
 *
 * All data tracks on the disk are examined.
 *
 * Returns TRUE iff the ISO protocol is used; FALSE otherwise.
 */

t_bool Uses_Iso_Protocol(CDROM *p_cdrom, t_ulong *p_offset) {
int i, len;
t_ulong *buf;

	/*
		If Data_Tracks() returns -1, then the drive probably doesn't support
		the SCSI-2 READ TOC command.
	*/
	if (p_cdrom->use_trackdisk || (len = Data_Tracks(p_cdrom, &buf)) < 0)
	{
		*p_offset = 0;
		if (!Read_Sector(p_cdrom, 16))
			return FALSE;  
		return StrNCmp((char *) p_cdrom->buffer + 1, "CD001", 5) == 0;
	}

	if (len == 0)
		return FALSE;

	/*
		Use a vendor-specific command to find the offset of the last
		session:
	*/
	if (
			Find_Last_Session(p_cdrom, p_offset) &&
			Read_Sector(p_cdrom, 16 + *p_offset) &&
			StrNCmp((char *) p_cdrom->buffer + 1, "CD001", 5) == 0
		)
	{
		FreeVec (buf);
		return TRUE;
	}

	/* Search all data tracks for valid primary volume descriptors: */
	for (i=len-1; i>=0; i--)
	{
		*p_offset = buf[i];
		if (!Read_Sector(p_cdrom, 16 + *p_offset))
			continue;
		if (StrNCmp((char *) p_cdrom->buffer + 1, "CD001", 5) == 0)
		{
			FreeVec (buf);
			return TRUE;
		}
	}

	FreeVec (buf);

	/*
		On some disks, the information in the TOC may not be valid. Therefore
		also check sector 16:
	*/
	if (!Read_Sector(p_cdrom, 16))
		return FALSE;
	if (StrNCmp((char *) p_cdrom->buffer + 1, "CD001", 5) == 0)
	{
		*p_offset = 0;
		return TRUE;
	}

	return FALSE;
}

/* Check whether the given volume uses the High Sierra Protocol.
 * The protocol is identified by the sequence
 *            'C' 'D' 'R' 'O' 'M'
 * in the 10th..14th byte of sector 16.
 *
 * Returns TRUE iff the High Sierra protocol is used; FALSE otherwise.
 */

t_bool Uses_High_Sierra_Protocol(CDROM *p_cdrom) {
	if (!Read_Sector(p_cdrom, 16))
		return FALSE;

	return StrNCmp((char *) p_cdrom->buffer + 9, "CDROM", 5) == 0;
}

t_bool Iso_Init_Vol_Info(VOLUME *p_volume, int p_skip, t_ulong p_offset) {
long loc = 16 + p_offset;
extern t_handler g_iso_handler, g_rr_handler;

	if (p_volume->protocol == PRO_ISO)
		p_volume->handler = &g_iso_handler;
	else
		p_volume->handler = &g_rr_handler;

	p_volume->vol_info = AllocMem (sizeof (t_iso_vol_info), MEMF_PUBLIC);
	if (!p_volume->vol_info)
	{
		global->iso_errno = ISOERR_NO_MEMORY;
		return FALSE;
	}

	for (;;)
	{
		if (!Read_Sector(p_volume->cd, loc))
		{
			global->iso_errno = ISOERR_SCSI_ERROR;
			FreeMem (p_volume->vol_info, sizeof (t_iso_vol_info));
			return FALSE;
		}

		if (p_volume->cd->buffer[0] == 1)
		{
			CopyMem
				(
					p_volume->cd->buffer,
					&VOL(p_volume,pvd),
					sizeof (prim_vol_desc)
				);
			break;
		}

		if (p_volume->cd->buffer[0] == 255 || loc > 1000)
		{
			global->iso_errno = ISOERR_NO_PVD;
			FreeMem (p_volume->vol_info, sizeof (t_iso_vol_info));
			return FALSE;
		}

		loc++;
	}

	VOL(p_volume,skip) = p_skip;

	switch (VOL(p_volume,pvd).block_size)
	{
	case 512:
		VOL(p_volume,blockshift) = 2;
		break;
	case 1024:
		VOL(p_volume,blockshift) = 1;
		break;
	case 2048:
	default:
		VOL(p_volume,blockshift) = 0;
		break;
	}

	/*
		Look at the system ID to find out if the CD is supposed
		to feature proper file names. These are CDs made for use
		with the CDTV and the CD³² (both share the "CDTV" system ID)
		and the "Fresh Fish", "Frozen Fish" and "Gold Fish" CDs
		created by Mkisofs. If any of these IDs is found the
		file name to lower case conversion is temporarily
		disabled if the "ML=MAYBELOWERCASE" option has been selected.
	*/

	if (
			p_volume->protocol == PRO_ROCK ||
			!StrNCmp(VOL(p_volume,pvd).system_id,"CDTV",4) ||
			!StrNCmp(VOL(p_volume,pvd).system_id,"AMIGA",5)
		)
		p_volume->mixed_char_filenames = TRUE;
	else
		p_volume->mixed_char_filenames = FALSE;

	return TRUE;
}

void Iso_Close_Vol_Info(VOLUME *p_volume)
{
	FreeMem (p_volume->vol_info, sizeof (t_iso_vol_info));
}

CDROM_OBJ *Iso_Alloc_Obj(int p_length_of_dir_record) {
CDROM_OBJ *obj;

	obj = AllocMem (sizeof (CDROM_OBJ), MEMF_PUBLIC | MEMF_CLEAR);
	if (!obj)
	{
		global->iso_errno = ISOERR_NO_MEMORY;
		return NULL;
	}

	obj->obj_info = AllocMem (sizeof (t_iso_obj_info), MEMF_PUBLIC);
	if (!obj->obj_info)
	{
		FreeMem (obj, sizeof (CDROM_OBJ));
		return NULL;
	}

	OBJ(obj,dir) = AllocMem (p_length_of_dir_record, MEMF_PUBLIC);
	if (!OBJ(obj,dir))
	{
		global->iso_errno = ISOERR_NO_MEMORY;
		FreeMem (obj->obj_info, sizeof (t_iso_obj_info));
		FreeMem (obj, sizeof (CDROM_OBJ));
		return NULL;
	}

	return obj;
}

/* Get the "CDROM object" for the root directory of the volume.
 */

CDROM_OBJ *Iso_Open_Top_Level_Directory(VOLUME *p_volume) {
CDROM_OBJ *obj;

	obj = Iso_Alloc_Obj(VOL(p_volume,pvd).root.length);
	if (!obj)
		return NULL;

	obj->directory_f = TRUE;
	obj->volume = p_volume;
	obj->pos = 0;
	CopyMem
		(
			&VOL(p_volume,pvd).root,
			OBJ(obj,dir),
			VOL(p_volume,pvd).root.length
		);

	return obj;
}

/* Test on equality of directory names (ignoring case).
 */

int Directory_Names_Equal(char *p_iso_name, int p_length, char *p_name) {
	return (Strncasecmp(p_iso_name, p_name, p_length)==0 && p_name[p_length]==0);
}

/* Compare the name of the directory entry p_iso_name (with length p_length)
 * with the C string p_name, and return 1 iff both strings are equal.
 * NOTE: p_iso_name may be a file name (with version number) or a directory
 *       name (without version number).
 */

int Names_Equal(char *p_iso_name, int p_length, char *p_name) {
int pos;

	if (Strncasecmp(p_iso_name, p_name, p_length) == 0 && p_name[p_length] == 0)
		return TRUE;

	/* compare without version number: */

	for (pos=p_length-1; pos>=0; pos--)
		if (p_iso_name[pos] == ';')
			break;

	if (pos>=0)
		return (Strncasecmp(p_iso_name, p_name, pos) == 0 && p_name[pos] == 0);
	else
		return FALSE;
}

/* Get a record from a directory.
 * p_location is a LOGICAL BLOCK number.
 */

#warning "FIXME: static variable may cause problems with 2 processes started!!!"
directory_record *Get_Directory_Record
	(VOLUME *p_volume, unsigned long p_location, unsigned long p_offset)
{
static unsigned char result[256];
int len;
int loc;
  
	loc = (p_location >> VOL(p_volume,blockshift)) + (p_offset >> 11);
	if (!Read_Sector(p_volume->cd, loc))
	{
		global->iso_errno = ISOERR_SCSI_ERROR;
		return NULL;
	}

	len = p_volume->cd->buffer[p_offset & 2047];
	if (len)
		CopyMem(p_volume->cd->buffer + (p_offset & 2047), result, len);
	else
		result[0] = 0;  /* mark as last record */

	return (directory_record *) result;
}

/* Create a "CDROM object" for the directory which is located
 * at sector p_location.
 */

CDROM_OBJ *Iso_Create_Directory_Obj(VOLUME *p_volume, unsigned long p_location){
directory_record *dir;
unsigned long loc;
int offset = 0;
CDROM_OBJ *obj;
unsigned long len;

	if (p_location == VOL(p_volume,pvd).root.extent_loc)
		 return Iso_Open_Top_Level_Directory(p_volume);

	dir = Get_Directory_Record(p_volume, p_location, 0);
	if (!dir)
		return NULL;

	dir = Get_Directory_Record(p_volume, p_location, dir->length);
	if (!dir)
		return NULL;

	loc = dir->extent_loc;
	len = dir->data_length;
	for (;;)
	{
		if (offset >= len)
			return NULL;
		dir = Get_Directory_Record(p_volume, loc, offset);
		if (!dir)
			return NULL;
		if (!dir->length)
		{
			/* go to next logical sector: */
			offset = (offset & 0xfffff800) + 2048;
			continue;
		}
		if (dir->extent_loc == p_location)
			break;
		offset += dir->length;
	}

	obj = Iso_Alloc_Obj(dir->length);
	if (!obj)
		return NULL;

	obj->directory_f = TRUE;
	obj->volume = p_volume;
	obj->pos = 0;
	CopyMem(dir, OBJ(obj,dir), dir->length);

	return obj;
}


/* Open the object with name p_name in the directory p_dir.
 * p_name must not contain '/' or ':' characters.
 */

CDROM_OBJ *Iso_Open_Obj_In_Directory(CDROM_OBJ *p_dir, char *p_name) {
unsigned long loc =
	OBJ(p_dir,dir)->extent_loc + OBJ(p_dir,dir)->ext_attr_length;
unsigned long len = OBJ(p_dir,dir)->data_length;
directory_record *dir;
int offset;
CDROM_OBJ *obj;
long cl;

	/* skip first two entries: */

	dir = Get_Directory_Record(p_dir->volume, loc, 0);
	if (!dir)
		return NULL;

	offset = dir->length;
	dir = Get_Directory_Record(p_dir->volume, loc, offset);
	if (!dir)
	return NULL;

	offset += dir->length;
	for (;;)
	{
		if (offset >= len)
		{
			global->iso_errno = ISOERR_NOT_FOUND;
			return NULL;
		}
		dir = Get_Directory_Record(p_dir->volume, loc, offset);
		if (!dir)
			return NULL;
		if (!dir->length)
		{
			/* go to next logical sector: */
			offset = (offset & 0xfffff800) + 2048;
			continue;
		}
		if (p_dir->volume->protocol == PRO_ROCK)
		{
		char buf[256];
		int len;

			if (
					(len=Get_RR_File_Name(p_dir->volume,dir,buf,sizeof(buf))) > 0 &&
          		(Strncasecmp(buf, p_name, len) == 0) &&
					p_name[len] == 0
				)
				break;
		}

		if (Names_Equal(dir->file_id, dir->file_id_length, p_name))
			break;
		offset += dir->length;
	}

	if (
			p_dir->volume->protocol == PRO_ROCK &&
      	(cl = RR_Child_Link(p_dir->volume, dir)) >= 0
		)
		return Iso_Create_Directory_Obj(p_dir->volume, cl);

	obj = Iso_Alloc_Obj(dir->length);
	if (!obj)
		return NULL;

	obj->directory_f = (dir->flags & 2);
	if (
			p_dir->volume->protocol == PRO_ROCK &&
			Is_A_Symbolic_Link(p_dir->volume, dir)
		)
	{
		obj->symlink_f = 1;
		obj->directory_f = 0;
	}
	CopyMem(dir, OBJ(obj,dir), dir->length);
	obj->volume = p_dir->volume;
	obj->pos = 0;
	if (!obj->directory_f)
		OBJ(obj,parent_loc) = loc;

	return obj;
}

/* Close a "CDROM object" and deallocate all associated resources.
 */

void Iso_Close_Obj(CDROM_OBJ *p_object) {
	FreeMem (OBJ(p_object,dir), OBJ(p_object,dir)->length);
	FreeMem (p_object->obj_info, sizeof (t_iso_obj_info));
	FreeMem (p_object, sizeof (CDROM_OBJ));
}

/* Read bytes from a file.
 */

int Iso_Read_From_File(CDROM_OBJ *p_file, char *p_buffer, int p_buffer_length) {
unsigned long loc;
int remain_block, remain_file, remain;
int len;
VOLUME *vol = p_file->volume;
CDROM *cd = vol->cd;
int buf_pos = 0;
int todo;
unsigned long last_loc, ext_loc;
short blockshift;
int offset;
unsigned long firstblock;

	if (p_file->pos == OBJ(p_file,dir)->data_length)
	{
		/* at end of file: */
		return 0;
	}

	blockshift = VOL(vol,blockshift);
	/* 'firstblock' is the first logical block of the file section: */
	firstblock =
		OBJ(p_file,dir)->extent_loc + OBJ(p_file,dir)->ext_attr_length;
	/*
		'offset' is the offset of the first logical block of the file
		extent from the first logical (2048-byte-)sector.
	*/
	if (blockshift)
		offset = ((firstblock & ((1<<blockshift)-1))<< (11-blockshift));
	else
	offset = 0;
	/*
		'ext_loc' is the first logical sector of the file extent.
		'loc' is the first logical sector to be read.
		'last_loc' is the last logical sector of the file extent.
	*/
	ext_loc = firstblock >> blockshift;
	loc = ext_loc + ((p_file->pos + offset) >> 11);
	last_loc = ext_loc + ((OBJ(p_file,dir)->data_length + offset - 1) >> 11);
	todo = p_buffer_length;

	offset += p_file->pos;
	offset &= 2047;
	remain_block = 2048 - offset;

	while (todo)
	{
		if (!Read_Contiguous_Sectors(cd, loc, last_loc))
		{
			global->iso_errno = ISOERR_SCSI_ERROR;
			return -1;
		}

		remain_file = OBJ(p_file,dir)->data_length - p_file->pos;
		/* 
			'todo' is the number of bytes in p_buffer which haven't been filled
			       yet.
			'remain' is remaining number of bytes in cd->buffer.
		*/
		remain = (remain_block < remain_file) ? remain_block : remain_file;
		len = (todo < remain) ? todo : remain;
		CopyMem ((APTR) (cd->buffer + offset), (APTR) (p_buffer + buf_pos), len);
		buf_pos += len;
		p_file->pos += len;
		todo -= len;

		if (p_file->pos >= OBJ(p_file,dir)->data_length)
			break;

		remain_block = 2048;
		offset = 0;

		loc++;
	}

	return buf_pos;
}

t_ulong Extract_Date(directory_record *p_dir_record) {
struct ClockData ClockData;

	ClockData.sec   = p_dir_record->second;
	ClockData.min	  = p_dir_record->minute;
	ClockData.hour  = p_dir_record->hour;
	ClockData.mday  = p_dir_record->day;
	ClockData.wday  = 0; /* is ignored by CheckDate() and Date2Amiga() */
	ClockData.month = p_dir_record->month;
	ClockData.year  = p_dir_record->year + 1900;

	if (CheckDate (&ClockData))
		return Date2Amiga (&ClockData);
	else
		return 0;
}

/* Return information on a "CDROM object."
 */

int Iso_CDROM_Info(CDROM_OBJ *p_obj, CDROM_INFO *p_info) {
int len;

	if (Iso_Is_Top_Level_Object (p_obj))
	{
		p_info->name_length = 1;
		p_info->name[0] = ':';
		p_info->directory_f = TRUE;
		p_info->file_length = 0;
		p_info->date = Volume_Creation_Date(p_obj->volume);
	}
	else
	{
		if (
				p_obj->volume->protocol == PRO_ROCK &&
				(
					len = Get_RR_File_Name
						(
							p_obj->volume, OBJ(p_obj,dir),
							p_info->name, sizeof (p_info->name)
						)
				) > 0
			)
		{
			p_info->name_length = len;
		}
		else
		{
			p_info->name_length = OBJ(p_obj,dir)->file_id_length;
			CopyMem(OBJ(p_obj,dir)->file_id, p_info->name, p_info->name_length);
		}
		p_info->directory_f = p_obj->directory_f;
		p_info->symlink_f = p_obj->symlink_f;
		p_info->file_length = OBJ(p_obj,dir)->data_length;
		p_info->date = Extract_Date(OBJ(p_obj,dir));
	}

	return 1;
}

/* Browse all entries in a directory.
 */

int Iso_Examine_Next
	(CDROM_OBJ *p_dir, CDROM_INFO *p_info, unsigned long *p_offset)
{
unsigned long offset;
directory_record *rec;
int len;

	if (!p_dir->directory_f || p_dir->symlink_f)
	{
		global->iso_errno = ISOERR_BAD_ARGUMENTS;
		return 0;
	}

	if (*p_offset == 0)
	{
		/* skip first two directory entries: */

		rec = Get_Directory_Record
			(
				p_dir->volume,
				 OBJ(p_dir,dir)->extent_loc + OBJ(p_dir,dir)->ext_attr_length,
				0
			);
		if (!rec)
			return 0;

		offset = rec->length;

		rec = Get_Directory_Record
			(
				p_dir->volume,
				OBJ(p_dir,dir)->extent_loc + OBJ(p_dir,dir)->ext_attr_length,
				offset
			);
		if (!rec)
			return 0;

		*p_offset = offset + rec->length;
	}

	for (;;)
	{
		if (OBJ(p_dir,dir)->data_length <= *p_offset)
			return 0;

		rec = Get_Directory_Record
					(
						p_dir->volume,
						OBJ(p_dir,dir)->extent_loc + OBJ(p_dir,dir)->ext_attr_length,
						*p_offset
					);
		if (!rec)
			return 0;

		if (rec->length == 0)
		{
			/* go to next logical sector: */
			*p_offset = (*p_offset & 0xfffff800) + 2048;
		}
		else
			break;
	}

	*p_offset += rec->length;

	if (
			p_dir->volume->protocol == PRO_ROCK &&
			(
				len = Get_RR_File_Name
					(p_dir->volume, rec, p_info->name, sizeof (p_info->name))
			) > 0
		)
	{
		p_info->name_length = len;
	}
	else
	{
		p_info->name_length = rec->file_id_length;
		CopyMem(rec->file_id, p_info->name, rec->file_id_length);
	}
	if (
			p_dir->volume->protocol == PRO_ROCK &&
			Is_A_Symbolic_Link(p_dir->volume, rec)
		)
	{
		p_info->symlink_f = 1;
		p_info->directory_f = 0;
	}
	else if
		(
			p_dir->volume->protocol == PRO_ROCK &&
			Has_System_Use_Field(p_dir->volume, rec, "CL")
		)
	{
		p_info->symlink_f = 0;
		p_info->directory_f = 1;
	}
	else
	{
		p_info->symlink_f = 0;
		p_info->directory_f = rec->flags & 2;
	}
	p_info->file_length = rec->data_length;
	p_info->date = Extract_Date(rec);
	p_info->suppl_info = rec;

	return 1;
}

/* Clone a "CDROM object info."
 */

void *Iso_Clone_Obj_Info(void *p_info) {
t_iso_obj_info *info = (t_iso_obj_info *) p_info;
t_iso_obj_info *new;
  
	new = AllocMem (sizeof (t_iso_obj_info), MEMF_PUBLIC);
	if (!new)
		return NULL;

	CopyMem(info, new, sizeof (t_iso_obj_info));

	new->dir = AllocMem (info->dir->length, MEMF_PUBLIC);
	if (!new->dir)
	{
		FreeMem (new, sizeof (t_iso_obj_info));
		return NULL;
	}
	CopyMem(info->dir, new->dir, info->dir->length);

	return new;
}

/* Find parent directory.
 */

CDROM_OBJ *Iso_Find_Parent(CDROM_OBJ *p_object) {
directory_record *dir;
unsigned long dir_loc;
long pl;

	if (p_object->directory_f)
		dir_loc =
			OBJ(p_object,dir)->extent_loc + OBJ(p_object,dir)->ext_attr_length;
	else
		dir_loc = OBJ(p_object,parent_loc);

	dir = Get_Directory_Record(p_object->volume, dir_loc, 0);
	if (!dir)
		return NULL;

	if (p_object->directory_f)
	{
		dir = Get_Directory_Record(p_object->volume, dir_loc, dir->length);
		if (!dir)
			return NULL;
		if (
				p_object->volume->protocol == PRO_ROCK &&
				(pl = RR_Parent_Link(p_object->volume, dir)) >= 0
			)
			return Iso_Create_Directory_Obj(p_object->volume, pl);
	}

	return Iso_Create_Directory_Obj(p_object->volume, dir->extent_loc);
}

/* Test if p_object is the root directory.
 */

t_bool Iso_Is_Top_Level_Object (CDROM_OBJ *p_object)
{
  return p_object->directory_f &&
         OBJ(p_object,dir)->extent_loc ==
	 VOL(p_object->volume,pvd).root.extent_loc;
}

/* Test if two objects are equal.
 */

t_bool Iso_Same_Objects (CDROM_OBJ *p_obj1, CDROM_OBJ *p_obj2)
{
  return (OBJ(p_obj1,dir)->extent_loc ==
	  OBJ(p_obj2,dir)->extent_loc);
}

/*
 * Convert p_num digits into an integer value:
 */

int Digs_To_Int (char *p_digits, int p_num)
{
  int result = 0;
  int i;
  
  for (i=0; i<p_num; i++)
    result = result * 10 + p_digits[i] - '0';
    
  return result;
}

/*
 * Return volume creation date as number of seconds since 1-Jan-1978:
 */

t_ulong Iso_Creation_Date(VOLUME *p_volume) {
struct ClockData ClockData;
char *dt = VOL(p_volume,pvd).vol_creation;

	ClockData.sec   = Digs_To_Int (dt+12, 2);
	ClockData.min	  = Digs_To_Int (dt+10, 2);
	ClockData.hour  = Digs_To_Int (dt+8, 2);
	ClockData.mday  = Digs_To_Int (dt+6, 2);
	ClockData.wday  = 0; /* is ignored by CheckDate() and Date2Amiga() */
	ClockData.month = Digs_To_Int (dt+4, 2);
	ClockData.year  = Digs_To_Int (dt, 4);

	if (CheckDate (&ClockData))
		return Date2Amiga (&ClockData);
	else
		return 0;
}

t_ulong Iso_Volume_Size (VOLUME *p_volume)
{
  return VOL(p_volume,pvd).space_size;
}

t_ulong Iso_Block_Size (VOLUME *p_volume)
{
  return VOL(p_volume,pvd).block_size;
}

void Iso_Volume_ID(VOLUME *p_volume, char *p_buffer, int p_buf_length) {
char *iso_name = VOL(p_volume,pvd).volume_id;
int iso_len;
int len;

	for (iso_len = 30; iso_len; iso_len--)
	{
		if (iso_name[iso_len-1] != ' ')
			break;
	}

	len = (iso_len > p_buf_length-1) ? p_buf_length-1 : iso_len;
	if (len > 0)
		CopyMem(iso_name, p_buffer, len);
	p_buffer[len] = 0;
}

t_ulong Iso_Location (CDROM_OBJ *p_object)
{
  return OBJ(p_object,dir)->extent_loc;
}

t_ulong Iso_File_Length (CDROM_OBJ *p_obj)
{
  return OBJ(p_obj,dir)->data_length;
}

t_handler g_iso_handler = {
  Iso_Close_Vol_Info,
  Iso_Open_Top_Level_Directory,
  Iso_Open_Obj_In_Directory,
  Iso_Find_Parent,
  Iso_Close_Obj,
  Iso_Read_From_File,
  Iso_CDROM_Info,
  Iso_Examine_Next,
  Iso_Clone_Obj_Info,
  Iso_Is_Top_Level_Object,
  Iso_Same_Objects,
  Iso_Creation_Date,
  Iso_Volume_Size,
  Iso_Volume_ID,
  Iso_Location,
  Iso_File_Length,
  Iso_Block_Size
};

t_handler g_rr_handler = {
  Iso_Close_Vol_Info,
  Iso_Open_Top_Level_Directory,
  Iso_Open_Obj_In_Directory,
  Iso_Find_Parent,
  Iso_Close_Obj,
  Iso_Read_From_File,
  Iso_CDROM_Info,
  Iso_Examine_Next,
  Iso_Clone_Obj_Info,
  Iso_Is_Top_Level_Object,
  Iso_Same_Objects,
  Iso_Creation_Date,
  Iso_Volume_Size,
  Iso_Volume_ID,
  Iso_Location,
  Iso_File_Length,
  Iso_Block_Size
};

