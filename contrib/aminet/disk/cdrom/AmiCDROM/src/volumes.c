/* volumes.c:
 *
 * Volume management.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 19-Sep-94   fmu   Fixed bug in Reinstall_Locks()
 * 22-May-94   fmu   Performance improvement for lock+filehandle processing.
 */

#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/utility.h>

#include "cdrom.h"
#include "device.h"
#include "devsupp.h"
#include "generic.h"

#include "clib_stuff.h"
#include "baseredef.h"

extern char *g_vol_name;
extern VOLUME *g_volume;
extern CDROM_OBJ *g_top_level_obj;
extern struct DeviceList *DevList;

/*  Associate p_lock with the pathname of the locked object on the current
 *  volume.
 */

typedef struct fh_node {
  char *vol_name;
  CDROM_OBJ* obj;
  struct DeviceList* devlist;
  struct fh_node *next;
} t_fh_node;

t_fh_node *g_fh_list = NULL;

/*  Associate p_obj with the pathname of the associated disk object on the current
 *  volume.
 */

void Register_File_Handle(struct ACDRBase *acdrbase, CDROM_OBJ *p_obj)
{
t_fh_node *new;

	new = (t_fh_node*) AllocMem (sizeof (t_fh_node), MEMF_PUBLIC);
	if (!new)
	{
		return;
	}

	new->vol_name = (char*) AllocMem (StrLen (g_vol_name+1) + 1, MEMF_PUBLIC);
	if (!new->vol_name)
	{
		FreeMem (new, sizeof (t_fh_node));
		return;
	}
	StrCpy (new->vol_name, g_vol_name+1);

	new->obj = p_obj;
	new->devlist = DevList;
	new->next = g_fh_list;
	g_fh_list = new;

}

/*  Remove the entry for the file handle p_obj in the list g_fh_list.
 */

void Unregister_File_Handle(struct ACDRBase *acdrbase, CDROM_OBJ *p_obj) {
t_fh_node *ptr, *old;
  
	for (ptr=g_fh_list, old = NULL; ptr; old = ptr, ptr = ptr->next)
		if (ptr->obj == p_obj && StrCmp (g_vol_name+1, ptr->vol_name) == 0)
		{
			if (old)
				old->next = ptr->next;
			else
				g_fh_list = ptr->next;
			FreeMem (ptr->vol_name, StrLen (ptr->vol_name) + 1);
			FreeMem (ptr, sizeof (t_fh_node));
			return;
		}
}

/*  Update the volume pointer for all CDROM_OBJs which are used as
 *  filehandles for the current volume.
 *
 *  Returns the number of file handles on the volume.
 */

int Reinstall_File_Handles (void)
{
  t_fh_node *ptr;
  int result = 0;

  for (ptr=g_fh_list; ptr; ptr = ptr->next) {
    if (StrCmp (g_vol_name+1, ptr->vol_name) == 0) {
      result++;
      ptr->obj->volume = g_volume;
    }
  }
  return result;
}

struct DeviceList *Find_Dev_List (CDROM_OBJ *p_obj) {
t_fh_node *ptr;

	for (ptr=g_fh_list; ptr; ptr = ptr->next)
	{
		if (ptr->obj == p_obj)
		{
			return ptr->devlist;
		}
	}
	return NULL;
}

typedef struct vol_reg_node {
  char *name;
  struct DeviceList *volume;
  struct vol_reg_node *next;
} t_vol_reg_node;

t_vol_reg_node *g_volume_list = NULL;

/*  Register a volume node as owned by this handler.
 */

void Register_Volume_Node(struct ACDRBase *acdrbase, struct DeviceList *p_volume) {
t_vol_reg_node *new;
int len;
  
	new = (t_vol_reg_node*) AllocMem (sizeof (t_vol_reg_node), MEMF_PUBLIC);
	if (!new)
		return;

	new->volume = p_volume;
	len = AROS_BSTR_StrLen(p_volume->dl_OldName);
	new->name = (char*) AllocMem (len + 1, MEMF_PUBLIC);
	if (!new)
	{
		FreeMem (new, sizeof (t_vol_reg_node));
		return;
	}
	CopyMem((char*) AROS_BSTR_ADDR(p_volume->dl_OldName), new->name, len);
	new->name[len] = 0;
	new->next = g_volume_list;
	g_volume_list = new;
}

/*  Remove the registration for the volume node.
 */

void Unregister_Volume_Node(struct ACDRBase *acdrbase, struct DeviceList *p_volume) {
t_vol_reg_node *ptr, *old;

	for (ptr=g_volume_list, old=NULL; ptr; old=ptr, ptr=ptr->next)
	{
		if (p_volume == ptr->volume)
		{
			if (old)
				old->next = ptr->next;
			else
				g_volume_list = ptr->next;
			FreeMem (ptr->name, StrLen (ptr->name) + 1);
			FreeMem (ptr, sizeof (t_vol_reg_node));
			return;
		}
	}
}

/*  Find a volume node with a matching name.
 */

struct DeviceList *Find_Volume_Node(struct ACDRBase *acdrbase, char *p_name) {
t_vol_reg_node *ptr;

	for (ptr=g_volume_list; ptr; ptr=ptr->next)
	{
		if (Stricmp (ptr->name, p_name) == 0)
			return ptr->volume;
	}
	return NULL;
}
