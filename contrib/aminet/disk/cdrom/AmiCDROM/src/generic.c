/* generic.c:
 *
 * Generic interface to the protocol handlers.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 22-May-94   fmu   Performance improvement for lock+filehandle processing.
 * 01-Jan-94   fmu   Added multisession support.
 * 29-Nov-93   fmu   New function Block_Size().
 * 15-Nov-93   fmu   Missing return value for 'Location' inserted.
 * 10-Nov-93   fmu   Added HFS/ISO-9660-lookup reversing in Which_Protocol.
 */

#include <stdlib.h>
#include <string.h>

#include <aros/debug.h>
#include <exec/types.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/utility.h>

#include "cdrom.h"
#include "generic.h"
#include "iso9660.h"
#include "rock.h"
#include "hfs.h"
#include "params.h"

#include "baseredef.h"

#define HAN(vol,func) (*(vol)->handler->func)
#define HANX(vol,func) ((vol)->handler->func)

t_bool g_hfs_first = FALSE;

/* Strncasecmp works like 'strncmp' but ignores case differences.
 */

int Strncasecmp
	(
		struct ACDRBase *acdrbase,
		char *p_str1,
		char *p_str2,
		int p_length
	)
{
int i;
int len = 0;
  
	while (len < p_length && *p_str1 && *p_str2)
	{
		if ((i = ToLower (*p_str1++) - ToLower (*p_str2++)))
			return i;
		len++;
	}
	return (len == p_length) ? 0 : ToLower (*p_str1) - ToLower (*p_str2);
}

/* WhichProtocol - examines which protocol is used.
 *
 * Input Variables:
 *  p_cdrom - CDROM descriptor
 *  p_use_rock_ridge - flag for the ROCKRIDGE startup option
 *
 * Output Variables:
 *  p_skip - skip length for RockRidge disks
 *  p_offset - block offset of last session for multisession disks (ISO only)
 *
 * Result:
 *  PRO_ISO, PRO_HIGH_SIERRA, PRO_RR, PRO_HFS or PRO_UNKNOWN.
 */

t_protocol Which_Protocol
	(
		struct ACDRBase *acdrbase,
		CDROM *p_cdrom,
		t_bool p_use_rock_ridge,
		int *p_skip,
		t_ulong *p_offset
	)
{

	if (p_cdrom->model == MODEL_TOSHIBA_3401)
	{
		if (Is_XA_Mode_Disk(acdrbase, p_cdrom))
			Mode_Select(acdrbase, p_cdrom, 0x81, 2048);
		else
			Mode_Select(acdrbase, p_cdrom, 0x00, 2048);
	}

	if (g_hfs_first && Uses_HFS_Protocol(acdrbase, p_cdrom, p_skip))
		return PRO_HFS;

	if (Uses_Iso_Protocol(acdrbase, p_cdrom, p_offset))
	{
		if (p_use_rock_ridge)
		{
		VOLUME tmp_vol; /* temporary volume descriptor */
		t_bool rr;

			tmp_vol.cd = p_cdrom;
			tmp_vol.protocol = PRO_ISO;
			Iso_Init_Vol_Info(acdrbase, &tmp_vol, 0, *p_offset);
			rr = Uses_Rock_Ridge_Protocol(acdrbase, &tmp_vol, p_skip);
			HAN(&tmp_vol, close_vol_info)(acdrbase, &tmp_vol);
			return rr ? PRO_ROCK : PRO_ISO;
		}
		else
			return PRO_ISO;
	}
  
	if (Uses_High_Sierra_Protocol(acdrbase, p_cdrom))
		return PRO_HIGH_SIERRA;

	if (!g_hfs_first && Uses_HFS_Protocol(acdrbase, p_cdrom, p_skip))
		return PRO_HFS;

	return PRO_UNKNOWN;
}

VOLUME *Open_Volume
	(
		struct ACDRBase *acdrbase,
		CDROM *p_cdrom,
		t_bool p_use_rock_ridge
	)
{
VOLUME *res;
int skip;
t_ulong offset;
    
	res = AllocMem (sizeof (VOLUME), MEMF_PUBLIC);
	if (!res)
	{
		iso_errno = ISOERR_NO_MEMORY;
		return NULL;
	}

	res->cd = p_cdrom;
  
	res->locks = 0;        /* may be modified by application program */
	res->file_handles = 0; /* may be modified by application program */
	res->protocol = Which_Protocol
		(
			acdrbase,
			p_cdrom,
			p_use_rock_ridge,
			&skip,
			&offset
		);
	if (res->protocol == PRO_UNKNOWN)
	{
		/* give it a second try: */
		res->protocol = Which_Protocol
			(
				acdrbase,
				p_cdrom,
				p_use_rock_ridge,
				&skip,
				&offset
			);
	}

	switch (res->protocol)
	{
	case PRO_ROCK:
	case PRO_ISO:
		if (!Iso_Init_Vol_Info(acdrbase, res, skip, offset))
		{
			FreeMem (res, sizeof (VOLUME));
			return NULL;
		}
		break;
	case PRO_HFS:
		if (!HFS_Init_Vol_Info(acdrbase, res, skip))
		{
			FreeMem (res, sizeof (VOLUME));
			return NULL;
		}
		break;
	default:
		FreeMem (res, sizeof (VOLUME));
		return NULL;
	}

	return res;
}

void Close_Volume(struct ACDRBase *acdrbase, VOLUME *p_volume) {
	HAN(p_volume, close_vol_info)(acdrbase, p_volume);
	FreeMem (p_volume, sizeof (VOLUME));
}

CDROM_OBJ *Open_Top_Level_Directory
	(
		struct ACDRBase *acdrbase,
		VOLUME *p_volume
	)
{
CDROM_OBJ * result = HAN
	(
		p_volume, open_top_level_directory)(acdrbase, p_volume
	);
	if (result)
	result->pathlist = NULL;
	return result;
}

CDROM_OBJ *Open_Object
	(
		struct ACDRBase *acdrbase,
		CDROM_OBJ *p_current_dir,
		char *p_path_name
	)
{
char *cp = p_path_name;
CDROM_OBJ *obj, *new;
VOLUME *vol = p_current_dir->volume;
char *np;
char name[100];

	if (*cp == ':')
	{
		obj = HAN
			(
				vol, open_top_level_directory)(acdrbase, p_current_dir->volume
			);
		if (obj)
			obj->pathlist = NULL;
		cp++;
	}
	else
	{
		obj = Clone_Object(acdrbase, p_current_dir);
		if (!obj)
		{
			iso_errno = ISOERR_NO_MEMORY;
			return NULL;
		}
		while (*cp == '/')
		{
			if (Is_Top_Level_Object(obj))
			{
kprintf("returning error\n");
				iso_errno = ISOERR_NOT_FOUND;
				return NULL;
			}
			new = HAN(vol, find_parent)(acdrbase, obj);
			if (!new)
				return NULL;
			new->pathlist = Copy_Path_List (obj->pathlist, TRUE);
			Close_Object(acdrbase, obj);
			obj = new;
			cp++;
		}
	}

	while (*cp)
	{
		for (np = name; *cp && *cp != '/'; )
			*np++ = *cp++;
		*np = 0;

		if (*cp)
			cp++;

		new = HAN(vol, open_obj_in_directory)(acdrbase, obj, name);
		if (new)
		{
			new->pathlist = Append_Path_List(acdrbase, obj->pathlist, name);
			Close_Object(acdrbase, obj);
			if (*cp && new->symlink_f)
			{
				HAN(vol, close_obj)(acdrbase, new);
				iso_errno = ISOERR_IS_SYMLINK;
				return NULL;
			}
			if (*cp && !new->directory_f)
			{
				HAN(vol, close_obj)(acdrbase, new);
				iso_errno = ISOERR_NOT_FOUND;
				return NULL;
			}
			obj = new;
		}
		else
		{
			Close_Object(acdrbase, obj);
			return NULL;
		}
	}

	return obj;
}

void Close_Object(struct ACDRBase *acdrbase, CDROM_OBJ *p_object)
{
	Free_Path_List(acdrbase, p_object->pathlist);
	HAN(p_object->volume, close_obj)(acdrbase, p_object);
}

int Read_From_File
	(
		struct ACDRBase *acdrbase,
		CDROM_OBJ *p_file,
		char *p_buffer,
		int p_buffer_length
	)
{
	return HAN
				(
					p_file->volume, read_from_file
				)
				(acdrbase, p_file, p_buffer, p_buffer_length);
}

int CDROM_Info
	(
		struct ACDRBase *acdrbase,
		CDROM_OBJ *p_obj,
		CDROM_INFO *p_info
	)
{
	return HAN(p_obj->volume, cdrom_info)(acdrbase, p_obj, p_info);
}

t_bool Examine_Next
	(
		struct ACDRBase *acdrbase,
		CDROM_OBJ *p_dir,
		CDROM_INFO *p_info,
		unsigned long *p_offset
	)
{
	return HAN(p_dir->volume, examine_next)(acdrbase, p_dir, p_info, p_offset);
}

CDROM_OBJ *Clone_Object(struct ACDRBase *acdrbase, CDROM_OBJ *p_object) {
CDROM_OBJ *new = AllocMem (sizeof (CDROM_OBJ), MEMF_PUBLIC);

	if (!new)
		return NULL;

	CopyMem(p_object, new, sizeof (CDROM_OBJ));
	new->obj_info =
		HAN(p_object->volume,clone_obj_info)(acdrbase, p_object->obj_info);
	if (!new->obj_info)
	{
		FreeMem (new, sizeof (CDROM_OBJ));
		return NULL;
	}

	new->pathlist = Copy_Path_List (p_object->pathlist, FALSE);

	return new;
}

CDROM_OBJ *Find_Parent(struct ACDRBase *acdrbase, CDROM_OBJ *p_object) {
CDROM_OBJ *result = HAN(p_object->volume, find_parent)(acdrbase, p_object);

	if (result)
		result->pathlist = Copy_Path_List (p_object->pathlist, TRUE);
	return result;
}

t_bool Is_Top_Level_Object (CDROM_OBJ *p_object)
{
  return HAN(p_object->volume, is_top_level_obj)(p_object);
}

t_bool Same_Objects (CDROM_OBJ *p_object1, CDROM_OBJ *p_object2)
{
  if (p_object1->volume != p_object2->volume)
    return 0;
  return HAN(p_object1->volume, same_objects)(p_object1, p_object2);
}

t_ulong Volume_Creation_Date(struct ACDRBase *acdrbase, VOLUME *p_volume) {
	if (!HANX(p_volume, creation_date))
		return 0;
	return HAN(p_volume, creation_date)(acdrbase, p_volume);
}

t_ulong Volume_Size (VOLUME *p_volume)
{
  return HAN(p_volume, volume_size)(p_volume);
}

t_ulong Block_Size (VOLUME *p_volume)
{
  return HAN(p_volume, block_size)(p_volume);
}

void Volume_ID
	(
		struct ACDRBase *acdrbase,
		VOLUME *p_volume,
		char *p_buffer,
		int p_buf_length
	)
{
  HAN(p_volume, volume_id)(acdrbase, p_volume, p_buffer, p_buf_length);
}

t_ulong Location (CDROM_OBJ *p_object)
{
  return HAN(p_object->volume, location)(p_object);
}

/* Find a position in a file.
 */

int Seek_Position (CDROM_OBJ *p_object, long p_offset, int p_mode)
{
  t_ulong new_pos;
  t_ulong max_len = HAN(p_object->volume, file_length)(p_object);

  if (p_object->directory_f) {
    iso_errno = ISOERR_BAD_ARGUMENTS;
    return 0;
  }
  
  switch (p_mode) {
  case SEEK_FROM_START:
    if (p_offset < 0 || p_offset > max_len) {
      iso_errno = ISOERR_OFF_BOUNDS;
      return 0;
    }
    new_pos = p_offset;
    break;
  case SEEK_FROM_CURRENT_POS:
    if ((p_offset < 0 && -p_offset > p_object->pos) ||
    	(p_offset > 0 && p_object->pos + p_offset > max_len)) {
      iso_errno = ISOERR_OFF_BOUNDS;
      return 0;
    }
    new_pos = p_object->pos + p_offset;
    break;
  case SEEK_FROM_END:
    if (p_offset > 0 || -p_offset > max_len) {
      iso_errno = ISOERR_OFF_BOUNDS;
      return 0;
    }
    new_pos = max_len + p_offset;
    break;
  default:
    iso_errno = ISOERR_BAD_ARGUMENTS;
    return 0;    
  }
  p_object->pos = new_pos;
  return 1;
}

/* Find path name of an object:
 *
 */

int Full_Path_Name
	(
		struct ACDRBase *acdrbase,
		CDROM_OBJ *p_obj,
		char *p_buf,
		int p_buf_length
	)
{
  return Path_Name_From_Path_List
				(
					acdrbase, p_obj->pathlist, p_buf, p_buf_length
				);
}
