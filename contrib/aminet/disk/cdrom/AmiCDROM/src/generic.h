/* generic.h: */

#ifndef _GENERIC_H_
#define _GENERIC_H_

#include "cdrom.h"

typedef unsigned long t_ulong;
typedef unsigned short t_ushort;
typedef unsigned char t_uchar;
typedef int t_bool;

typedef struct path_node *t_path_list;

typedef enum protocol {
  PRO_UNKNOWN,
  PRO_ISO,
  PRO_HIGH_SIERRA,
  PRO_ROCK,
  PRO_HFS
} t_protocol;

typedef struct VOLUME {
  CDROM		 *cd;			/* Pointer to CDROM structure	*/
  t_protocol	 protocol;		/* Protocol used		*/
  struct handler *handler;		/* Pointer to handler struct	*/
  void		 *vol_info;		/* Depends on protocol		*/
  size_t	 vol_info_size;		/* Size of vol_info structure	*/
  t_bool	 mixed_char_filenames;	/* Filenames may contain upper
  					   and lower case characters    */
  /* for use by the device handler: */
#if 0
  t_bool	 valid;			/* Is the volume valid?		*/
#endif
  int		 locks;			/* Number of locks on this vol. */
  int		 file_handles;		/* Number of file handles on    */
  					/* this volume			*/
  struct DeviceList *devlist;		/* Associated DOS device list   */
} VOLUME;

typedef struct CDROM_OBJ {
  t_bool		directory_f;	/* TRUE iff object is a directory     */
  t_bool		symlink_f;	/* TRUE iff object is a symbolic link */
  VOLUME		*volume;	/* Pointer to volume node	      */
  unsigned long		pos;		/* Current position (for read & seek) */
  t_path_list		pathlist;	/* List containing full path name     */
  void			*obj_info;	/* Depends on protocol		      */
} CDROM_OBJ;

typedef struct CDROM_INFO {
  t_bool		directory_f;	/* TRUE if object is a directory     */
  t_bool		symlink_f;	/* TRUE if object is a symbolic link */
  int			name_length;	/* length of file name		     */
  char			name[256];	/* file name			     */
  unsigned long		date;		/* creation date		     */
  unsigned long		file_length;	/* length of file		     */
  void			*suppl_info;	/* supplementary information	     */
} CDROM_INFO;

/* Codes: M=mandatory,
 *        O=optional (may be NULL)
 */

typedef struct handler {
  /*M*/ void       (*close_vol_info)(struct ACDRBase *, VOLUME *);
  /*M*/ CDROM_OBJ *(*open_top_level_directory)(struct ACDRBase *, VOLUME *);
  /*M*/ CDROM_OBJ *(*open_obj_in_directory)(struct ACDRBase *, CDROM_OBJ *, char *);
  /*M*/ CDROM_OBJ *(*find_parent)(struct ACDRBase *, CDROM_OBJ *);
  /*M*/ void       (*close_obj)(struct ACDRBase *, CDROM_OBJ *);
  /*M*/ int        (*read_from_file)(struct ACDRBase *, CDROM_OBJ *, char *, int);
  /*M*/ t_bool     (*cdrom_info)(struct ACDRBase *, CDROM_OBJ *, CDROM_INFO *);
  /*M*/ t_bool     (*examine_next)(struct ACDRBase *, CDROM_OBJ *, CDROM_INFO *, unsigned long *);
  /*M*/ void      *(*clone_obj_info)(struct ACDRBase *, void *);
  /*M*/ t_bool     (*is_top_level_obj)(CDROM_OBJ *);
  /*M*/ t_bool     (*same_objects)(CDROM_OBJ *, CDROM_OBJ *);
  /*O*/	t_ulong   (*creation_date)(struct ACDRBase *acdrbase, VOLUME *);
  /*M*/ t_ulong    (*volume_size)(VOLUME *);
  /*M*/ void       (*volume_id)(struct ACDRBase *, VOLUME *, char *, int);
  /*M*/ t_ulong    (*location)(CDROM_OBJ *);
  /*M*/ t_ulong    (*file_length)(CDROM_OBJ *);
  /*M*/ t_ulong    (*block_size)(VOLUME *);
} t_handler;

#define ISOERR_NO_MEMORY        1       /* out of memory                */
#define ISOERR_SCSI_ERROR       2       /* scsi command return with err */
#define ISOERR_NO_PVD           3       /* prim volume descr not found  */
#define ISOERR_ILLEGAL_NAME     4       /* illegal path name            */
#define ISOERR_NO_SUCH_RECORD   5       /* no such record in path table */
#define ISOERR_NOT_FOUND        6       /* file not found               */
#define ISOERR_OFF_BOUNDS       7       /* bad seek operation           */
#define ISOERR_BAD_ARGUMENTS    8       /* bad arguments                */
#define ISOERR_IS_SYMLINK       9       /* cannot open symbolic links   */
#define ISOERR_INTERNAL        10       /* reason unknown               */

#define SEEK_FROM_START         -1      /* values for                   */
#define SEEK_FROM_CURRENT_POS   0       /* the 'Seek_Position'          */
#define SEEK_FROM_END           1       /* function                     */

extern int iso_errno;

t_protocol Which_Protocol
	(
		struct ACDRBase *,
		CDROM *p_cdrom,
		t_bool p_use_rock_ridge,
		int *p_skip,
		t_ulong *p_offset
	);

VOLUME *Open_Volume(struct ACDRBase *, CDROM *p_cdrom, t_bool p_use_rock_ridge);
void Close_Volume(struct ACDRBase *, VOLUME *p_volume);

CDROM_OBJ *Open_Top_Level_Directory(struct ACDRBase *, VOLUME *p_volume);
CDROM_OBJ *Open_Object
	(
		struct ACDRBase *,
		CDROM_OBJ *p_current_dir,
		char *p_path_name
	);
void Close_Object(struct ACDRBase *, CDROM_OBJ *p_object);

int Read_From_File
	(
		struct ACDRBase *,
		CDROM_OBJ *p_file,
		char *p_buffer,
		int p_buffer_length
	);

int CDROM_Info(struct ACDRBase *, CDROM_OBJ *p_obj, CDROM_INFO *p_info);
t_bool Examine_Next
	(
		struct ACDRBase *,
		CDROM_OBJ *p_dir,
		CDROM_INFO *p_info,
		unsigned long *p_offset
	);

CDROM_OBJ *Clone_Object(struct ACDRBase *, CDROM_OBJ *p_object);
CDROM_OBJ *Find_Parent(struct ACDRBase *, CDROM_OBJ *p_object);

t_bool Is_Top_Level_Object (CDROM_OBJ *p_object);

int Seek_Position (CDROM_OBJ *p_object, long p_offset, int p_mode);

t_bool Same_Objects (CDROM_OBJ *p_object1, CDROM_OBJ *p_object2);
t_ulong Volume_Creation_Date(struct ACDRBase *, VOLUME *p_volume);
t_ulong Volume_Size (VOLUME *p_volume);
t_ulong Block_Size (VOLUME *p_volume);
void Volume_ID
	(
		struct ACDRBase *,
		VOLUME *p_volume,
		char *p_buffer,
		int p_buf_length
	);
t_ulong Location (CDROM_OBJ *p_object);
int Full_Path_Name
	(
		struct ACDRBase *,
		CDROM_OBJ *p_obj,
		char *p_buf,
		int p_buf_length
	);

int Strncasecmp(struct ACDRBase *, char *p_str1, char *p_str2, int p_length);
#warning "this should go into path.h!"
t_path_list Append_Path_List(struct ACDRBase *, t_path_list, char *);
t_path_list Copy_Path_List (t_path_list, int);
void Free_Path_List(struct ACDRBase *, t_path_list);
t_bool Path_Name_From_Path_List(struct ACDRBase *, t_path_list, char*, int);

#endif /* _GENERIC_H_ */
