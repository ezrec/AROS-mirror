/* cdrom.h: */

#ifndef _CDROM_H_
#define _CDROM_H_

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <devices/scsidisk.h>

#define VERSION "CDROM-Handler 1.15 (03.11.94)"

#define SCSI_BUFSIZE 2048

typedef enum model {
  MODEL_ANY,
  MODEL_TOSHIBA_3401,
  MODEL_CDU_8002,
} t_model;

typedef struct CDROM {
  unsigned char 	*buffer;
  unsigned char		**buffers;
  unsigned char 	*buffer_data;
  unsigned char		sense[20];
  t_model		model;
  short			use_trackdisk;
  short			lun;
  short			std_buffers;
  short			file_buffers;
  unsigned long		t_changeint;
  unsigned long		t_changeint2;
  long 			*current_sectors;
  unsigned long		*last_used;
  unsigned long		tick;
  struct MsgPort 	*port;
  struct IOStdReq	*scsireq;
  struct SCSICmd	cmd;
  unsigned short	block_length;
  unsigned short	blocking_factor;
  short			device_open;
} CDROM;

typedef struct inquiry_data {
  char		peripheral_type;
  char		modifier;
  char		version;
  char		flags1;
  char		additional_length;
  char		reserved[2];
  char		flags2;
  char		vendor[8];
  char		product[16];
  char		revision[4];
} t_inquiry_data;

typedef struct toc_header {
  unsigned short length;
  unsigned char  first_track;
  unsigned char  last_track;
} t_toc_header;

typedef struct toc_data {
  char		reserved1;
  unsigned char flags;
  unsigned char	track_number;
  char		reserved2;
  unsigned long address;
} t_toc_data;

CDROM *Open_CDROM
	(
		char *p_device,
		int p_scsi_id,
		int p_use_trackdisk,
		unsigned long p_memory_type,
		int p_std_buffers,
		int p_file_buffers
	);
int Read_Sector(CDROM *p_cd, long p_sector);
int Read_Contiguous_Sectors
	(
		CDROM *p_cd,
		long p_sector,
		long p_last_sector
	);
void Cleanup_CDROM(CDROM *p_cd);
int Test_Unit_Ready(CDROM *p_cd);
int Mode_Select(CDROM *p_cd, int p_on, int p_block_length);
int Inquire(CDROM *p_cd, t_inquiry_data *p_data);
int Is_XA_Mode_Disk(CDROM *p_cd);
t_toc_data *Read_TOC
	(
		CDROM *p_cd,
		t_toc_header *p_toc_header
	);
int Has_Audio_Tracks(CDROM *p_cd);
int Data_Tracks(CDROM *p_cd, unsigned long** p_buf);
void block2msf (unsigned long blk, unsigned char *msf);
int Start_Play_Audio(CDROM *p_cd);
int Stop_Play_Audio(CDROM *p_cd);
int Block_Length(CDROM *p_cd);
void Clear_Sector_Buffers (CDROM *p_cd);
int Find_Last_Session(CDROM *p_cd, unsigned long *p_result);

extern int g_cdrom_errno;
extern int g_ignore_blocklength;

enum {
  CDROMERR_OK = 0,	/* no error			*/
  CDROMERR_NO_MEMORY,	/* out of memory		*/
  CDROMERR_MSGPORT,	/* cannot create message port	*/
  CDROMERR_IOREQ,	/* cannot create I/O request	*/
  CDROMERR_DEVICE,	/* cannot open scsi device	*/
  CDROMERR_BLOCKSIZE	/* illegal blocksize		*/
};

#endif /* _CDROM_H_ */
