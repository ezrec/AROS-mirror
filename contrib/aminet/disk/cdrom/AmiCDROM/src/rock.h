/* rock.h: */

#ifndef _ROCK_H
#define _ROCK_H

#include "cdrom.h"
#include "iso9660.h"

t_bool Uses_Rock_Ridge_Protocol
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		int *p_skip
	);
int Get_RR_File_Name
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		directory_record *p_dir,
		char *p_buf,
		int p_buf_len
	);
int Is_A_Symbolic_Link
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		directory_record *p_dir
	);
t_bool Get_Link_Name
	(
		struct ACDRBase *,
		CDROM_OBJ *p_obj,
		char *p_buf,
		int p_buf_len
	);
int Has_System_Use_Field
	(
		struct ACDRBase *acdrbase,
		VOLUME *p_volume,
		directory_record *p_dir,
		char *p_name
	);
long RR_Child_Link
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		directory_record *p_dir
	);
long RR_Parent_Link
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		directory_record *p_dir
	);

#endif /* _ROCK_H */
