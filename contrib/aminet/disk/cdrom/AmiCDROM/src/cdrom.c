/* cdrom.c:
 *
 * Basic interaction with the CDROM drive.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 07-Jul-02 sheutlin  various changes when porting to AROS
 *                     - global variables are now in a struct Globals *global
 * 02-Sep-94   fmu   Display READ TOC for Apple CD 150 drives.
 * 01-Sep-94   fmu   Workaround for bad NEC 3X READ TOC command in 
 *		     Has_Audio_Tracks() and Data_Tracks().
 * 20-Aug-94   fmu   New function Find_Last_Session ().
 * 23-Jul-94   fmu   Last index modified from 99 to 1 in Start_Play_Audio().
 * 18-May-94   fmu   New drive model: MODEL_CDU_8002 (= Apple CD 150).
 * 17-May-94   fmu   Sense length 20 instead of 18 (needed by ALF controller).
 * 17-Feb-94   fmu   Added support for Toshiba 4101.
 * 06-Feb-94   dmb   - Fixed bug in Test_Unit_Ready() trackdisk support.
 *                   - Fixed bug in Open_CDROM (size of request)
 *                   - Added function Clear_Sector_Buffers().
 * 01-Jan-94   fmu   Added function Data_Tracks() for multisession support.
 * 11-Dec-93   fmu   - Memory type can now be chosen by the user.
 *                   - Addional parameter p_direction for Do_SCSI_Command().
 *                   - Start_Play_Audio() now plays all tracks.
 *                   - Mode_Select() instead of Select_XA_Mode().
 *                   - Support for CDROM drives with 512, 1024 or 2048 bytes
 *                     per block.
 * 06-Dec-93   fmu   New drive type DRIVE_SCSI_2.
 * 09-Nov-93   fmu   Added Select_XA_Mode.
 * 23-Oct-93   fmu   Open_CDROM now returns an error code that tell what
 *                   went wrong.
 * 09-Oct-93   fmu   SAS/C support added.
 * 03-Oct-93   fmu   New buffering algorithm.
 * 27-Sep-93   fmu   Added support for multi-LUN devices.
 * 24-Sep-93   fmu   - SCSI buffers may now reside in fast or chip memory.
 *                   - TD_CHANGESTATE instead of CMD_READ in Test_Unit_Ready
 */

#include <proto/alib.h>
#include <proto/exec.h>
#ifdef __AROS__
#include <aros/debug.h>
#endif
#include <devices/trackdisk.h>
#include <limits.h>
#include <string.h>

#include "cdrom.h"
#include "globals.h"

#include "clib_stuff.h"

extern struct Globals *global;

#ifdef SysBase
#	undef SysBase
#endif
#define SysBase global->SysBase

void Determine_Drive_Type(CDROM *p_cd) {
t_inquiry_data data;
char buf[33];
  
	p_cd->scsi_compliance = 1;
	p_cd->model = MODEL_ANY;

	if (!Inquire (p_cd, &data))
		return;

	if ((data.version & 0x7) >= 2)
		p_cd->scsi_compliance = 2;

	if (StrNCmp (data.vendor, "TOSHIBA", 7) == 0)
	{
		CopyMem(data.product, buf, 32);
		buf[32] = 0;
		if (StrStr (buf, "3401") || StrStr (buf, "4101"))
			p_cd->model = MODEL_TOSHIBA_3401;
	}
	else if (StrNCmp (data.vendor, "SONY", 4) == 0)
	{
		CopyMem(data.product, buf, 32);
		buf[32] = 0;
		if (StrStr (buf, "CDU-8002"))
			p_cd->model = MODEL_CDU_8002;
	}
}

CDROM *Open_CDROM
	(
		char *p_device,
		int p_scsi_id,
		int p_use_trackdisk,
		unsigned long p_memory_type,
		int p_std_buffers,
		int p_file_buffers
	)
{
CDROM *cd;
int i;
int bufs = p_std_buffers + p_file_buffers + 1;

	global->g_cdrom_errno = CDROMERR_NO_MEMORY;

	cd = AllocMem (sizeof (CDROM), MEMF_PUBLIC | MEMF_CLEAR | p_memory_type);

	if (!cd)
		return NULL;

	cd->std_buffers = p_std_buffers;
	cd->file_buffers = p_file_buffers;

	cd->buffer_data = AllocMem (SCSI_BUFSIZE * bufs + 15, MEMF_PUBLIC | p_memory_type);
        if (!cd->buffer_data)
	{
		Cleanup_CDROM(cd);
		return NULL;
	}

	cd->buffers = AllocMem (sizeof (unsigned char *) * bufs, MEMF_PUBLIC);
	if (!cd->buffers)
	{
		Cleanup_CDROM(cd);
		return NULL;
	}

	cd->current_sectors = AllocMem (sizeof (long) * bufs, MEMF_PUBLIC);
	if (!cd->current_sectors)
	{
		Cleanup_CDROM(cd);
		return NULL;
	}

	cd->last_used = AllocMem (sizeof (unsigned long) * p_std_buffers, MEMF_PUBLIC | MEMF_CLEAR);
	if (!cd->last_used)
	{
		Cleanup_CDROM(cd);
		return NULL;
	}

	/*
		make the buffer quad-word aligned. This greatly helps 
		performance on '040-powered systems with DMA SCSI
		controllers.
	*/

	cd->buffers[0] = (UBYTE *)(((ULONG)cd->buffer_data + 15) & ~15);

	for (i=1; i<bufs; i++)
		cd->buffers[i] = cd->buffers[0] + i * SCSI_BUFSIZE;

	cd->port = CreateMsgPort ();
	if (!cd->port)
	{
		global->g_cdrom_errno = CDROMERR_MSGPORT;
		Cleanup_CDROM(cd);
		return NULL;
	}

	cd->scsireq = (struct IOStdReq *)CreateIORequest (cd->port, sizeof (struct IOExtTD));
	if (!cd->scsireq)
	{
		global->g_cdrom_errno = CDROMERR_IOREQ;
		Cleanup_CDROM(cd);
		return NULL;
	}

	if (OpenDevice ((UBYTE *) p_device, p_scsi_id, (struct IORequest *) cd->scsireq, 0))
	{
		global->g_cdrom_errno = CDROMERR_DEVICE;
		Cleanup_CDROM(cd);
		return NULL;
	}

	cd->device_open = TRUE;

	for (i=0; i<bufs; i++)
		cd->current_sectors[i] = -1;

	if (p_use_trackdisk)
	{
		cd->scsireq->io_Command = CMD_CLEAR;
		DoIO ((struct IORequest *) cd->scsireq);
	}

	cd->use_trackdisk = p_use_trackdisk;
	cd->t_changeint = -1;
	cd->t_changeint2 = -2;

	/* The LUN is the 2nd digit of the SCSI id number: */
	cd->lun = (p_scsi_id / 10) % 10;

	/* 'tick' is incremented every time a sector is accessed. */
	cd->tick = 0;

	global->g_cdrom_errno = 0;

	Determine_Drive_Type(cd);

	if (global->g_ignore_blocklength)
	{
		cd->block_length = 0;
		cd->blocking_factor = 0;
	}
	else
	{
		cd->block_length = Block_Length(cd);
		switch (cd->block_length)
		{
		case 2048:
			cd->blocking_factor = 0;
			break;
		case 1024:
			cd->blocking_factor = 1;
			break;
		case 512:
			cd->blocking_factor = 2;
			break;
		case 0:
			cd->blocking_factor = 0;
			break;
		default:
			global->g_cdrom_errno = CDROMERR_BLOCKSIZE;
			Cleanup_CDROM(cd);
			return NULL;
		}
	}
	return cd;
}

int Do_SCSI_Command
	(
		CDROM *p_cd,
		unsigned char *p_buf,
		long p_buf_length,
		unsigned char *p_command,
		int p_length,
		int p_direction
	)
{
int bufs = p_cd->std_buffers + p_cd->file_buffers + 1;

	p_cd->scsireq->io_Length   = sizeof (struct SCSICmd);
	p_cd->scsireq->io_Data     = (APTR) &p_cd->cmd;
	p_cd->scsireq->io_Command  = HD_SCSICMD;

	p_cd->cmd.scsi_Data        = (UWORD *) p_buf;
	p_cd->cmd.scsi_Length      = p_buf_length;
	p_cd->cmd.scsi_Flags       = SCSIF_AUTOSENSE | p_direction;
	p_cd->cmd.scsi_SenseData   = (UBYTE *) p_cd->sense;
	p_cd->cmd.scsi_SenseLength = 20;
	p_cd->cmd.scsi_SenseActual = 0;
	p_cd->cmd.scsi_Command     = (UBYTE *) p_command;
	p_cd->cmd.scsi_CmdLength   = p_length;

	p_command[1] |= p_cd->lun << 5;

	DoIO ((struct IORequest *) p_cd->scsireq);
	if (p_cd->cmd.scsi_Status)
	{
	int i;
		for (i=0; i<bufs; i++)
			p_cd->current_sectors[i] = -1;
		return 0;
	}
	else
		return 1;
}

int Read_From_Drive
	(
		CDROM *p_cd,
		unsigned char *p_buf,
		long p_buf_length,
		long p_sector,
		int p_number_of_sectors
	)
{
static unsigned char cmd[10] = { 0x28, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int bufs = p_cd->std_buffers + p_cd->file_buffers + 1;

	if (p_cd->use_trackdisk)
	{
		p_cd->scsireq->io_Length   = 2048 * p_number_of_sectors;
		p_cd->scsireq->io_Data     = (APTR) p_buf;
		p_cd->scsireq->io_Offset   = (ULONG) p_sector * 2048;
		p_cd->scsireq->io_Command  = CMD_READ;

		DoIO ((struct IORequest *) p_cd->scsireq);
		if (p_cd->scsireq->io_Error)
		{
		int i;
			for (i=0; i<bufs; i++)
				p_cd->current_sectors[i] = -1;
			return 0;
		}
		else
			return 1;
	}
	else
	{
	long sector = p_sector << p_cd->blocking_factor;
		cmd[5] = sector & 0xff;
		cmd[4] = (sector >> 8) & 0xff;
		cmd[3] = (sector >> 16) & 0x1f;

		cmd[8] = p_number_of_sectors << p_cd->blocking_factor;

		return Do_SCSI_Command
					(p_cd, p_buf, p_buf_length, cmd, sizeof (cmd), SCSIF_READ);
	}
}

/* Read one sector from the CDROM drive.
 */

int Read_Sector (CDROM *p_cd, long p_sector) {
int status;
int i;
int maxbuf = p_cd->std_buffers;
int loc;

	p_cd->tick++;

	for (i=0; i<maxbuf; i++)
		if (p_cd->current_sectors[i] == p_sector)
		{
			p_cd->buffer = p_cd->buffers[i];
			p_cd->last_used[i] = p_cd->tick;
			return 1;
		}

	/* find an empty buffer position: */
	for (loc=0; loc<maxbuf; loc++)
		if (p_cd->current_sectors[loc] == -1)
			break;

	if (loc==maxbuf)
	{
		/*
			no free buffer position; remove the buffer that is unused
			for the longest time:
		*/
	unsigned long oldest_tick = ULONG_MAX;
	unsigned long tick;

		for (loc=0, i=0; i<maxbuf; i++)
		{
			tick = p_cd->last_used[i];
			if (tick < oldest_tick)
				loc = i, oldest_tick = tick;
		}
	}

	status = Read_From_Drive(p_cd,p_cd->buffers[loc],SCSI_BUFSIZE, p_sector, 1);
	if (status)
	{
		p_cd->current_sectors[loc] = p_sector;
		p_cd->buffer = p_cd->buffers[loc];
		p_cd->last_used[loc] = p_cd->tick;
	}

	return status;
}

/* Read_Contiguous_Sectors uses the 'file buffers' instead of the
 * 'standard buffers'. Additionaly, more than one sector may be read
 * with a single SCSI command. This may cause a substantial increase
 * in speed when reading large files.
 */

int Read_Contiguous_Sectors
	(
		CDROM *p_cd,
		long p_sector,
		long p_last_sector
	)
{
  int status;
  int i;
  int maxbuf = p_cd->std_buffers + p_cd->file_buffers;
  int len;

  for (i=p_cd->std_buffers; i<maxbuf; i++)
    if (p_cd->current_sectors[i] == p_sector) {
      p_cd->buffer = p_cd->buffers[i];
      return 1;
    }

  if (p_last_sector <= p_sector)
    len = 1;
  else {
    len = p_last_sector - p_sector + 1;
    if (len > p_cd->file_buffers)
      len = p_cd->file_buffers;
    if (len > 255)
      len = 255;
  }

  status = Read_From_Drive
		(p_cd,p_cd->buffers[p_cd->std_buffers],SCSI_BUFSIZE *len, p_sector, len);
  if (status) {
    long sector = p_sector;
    for (i=p_cd->std_buffers; len; i++, len--)
      p_cd->current_sectors[i] = sector++;
    p_cd->buffer = p_cd->buffers[p_cd->std_buffers];
  }

  return status;
}

int Test_Unit_Ready(CDROM *p_cd) {

	if (p_cd->use_trackdisk)
	{
		p_cd->scsireq->io_Command = TD_CHANGENUM;
		if (!DoIO ((struct IORequest *) p_cd->scsireq))
		{
			if (p_cd->scsireq->io_Error==0)
				p_cd->t_changeint = p_cd->scsireq->io_Actual;
		}
		p_cd->scsireq->io_Command = TD_CHANGESTATE;
		if (!DoIO ((struct IORequest *) p_cd->scsireq))
		{
			if (p_cd->scsireq->io_Error==0 && p_cd->scsireq->io_Actual==0)
				return TRUE;
		}
		return FALSE;
	}
	else
	{
	int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;
	static unsigned char cmd[6] = { 0, 0, 0, 0, 0, 0 };

		return Do_SCSI_Command
					(p_cd,p_cd->buffers[dummy_buf],SCSI_BUFSIZE, cmd, 6, SCSIF_READ);
	}
}

int Is_XA_Mode_Disk(CDROM *p_cd) {
static unsigned char cmd[10] = { 0xC7, 3, 0, 0, 0, 0, 0, 0, 0, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;

	if (!Do_SCSI_Command(p_cd,p_cd->buffers[dummy_buf],4,cmd,10,SCSIF_READ))
		return FALSE;
	return *(p_cd->buffers[dummy_buf]) == 0x20;
}

int Mode_Select
	(
		CDROM *p_cd,
		int p_mode,
		int p_block_length
	)
{
static unsigned char cmd[6] = { 0x15, 0x10, 0, 0, 12, 0 };
static unsigned char mode[12] =
	{ 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;

	if (p_cd->use_trackdisk)
		return FALSE;

	p_cd->block_length = p_block_length;
	switch (p_cd->block_length)
	{
	case 2048:
		p_cd->blocking_factor = 0;
		break;
	case 1024:
		p_cd->blocking_factor = 1;
		break;
	case 512:
		p_cd->blocking_factor = 2;
		break;
	}

	mode[4] = p_mode;
	mode[9] = p_block_length >> 16;
	mode[10] = (p_block_length >> 8) & 0xff;
	mode[11] = p_block_length & 0xff;

	CopyMem(mode, p_cd->buffers[dummy_buf], sizeof (mode));
	return Do_SCSI_Command
				(p_cd,p_cd->buffers[dummy_buf],sizeof (mode),cmd,6,SCSIF_WRITE);
}

int Inquire (CDROM *p_cd, t_inquiry_data *p_data)
{
static unsigned char cmd[6] = { 0x12, 0, 0, 0, 96, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;
  
	if (p_cd->use_trackdisk)
		return FALSE;
	if (!Do_SCSI_Command(p_cd,p_cd->buffers[dummy_buf],96,cmd,6,SCSIF_READ))
		return FALSE;

	CopyMem(p_cd->buffers[dummy_buf], p_data, sizeof (*p_data));
	return 1;
}

#define TOC_SIZE 804

t_toc_data *Read_TOC
	(
		CDROM *p_cd,
		t_toc_header *p_toc_header
	)
{
static unsigned char cmd[10] =
	{ 0x43, 0, 0, 0, 0, 0, 0, TOC_SIZE >> 8, TOC_SIZE & 0xff, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;

	if (p_cd->use_trackdisk)
		return NULL;

	if (p_cd->model == MODEL_CDU_8002) /* READ TOC not supported by this drive */
		return NULL;

	if (
			!Do_SCSI_Command
				(p_cd,p_cd->buffers[dummy_buf],TOC_SIZE,cmd,10,SCSIF_READ)
		)
		return NULL;

	CopyMem(p_cd->buffers[dummy_buf], p_toc_header, sizeof (*p_toc_header));
	return (t_toc_data *) (p_cd->buffers[dummy_buf] + 4);
}

int Has_Audio_Tracks(CDROM *p_cd) {
t_toc_header hdr;
t_toc_data *toc;
int i, len;

	
	toc = Read_TOC (p_cd, &hdr);
	if (!toc)
		return FALSE;

	len = hdr.length / 8;
	for (i=0; i<len; i++)
	{
		if (toc[i].track_number <= 99 && !(toc[i].flags & 4))
			return toc[i].track_number;
	}
	return FALSE;
}

/*
 * Create a buffer containing the start addresses of all data tracks
 * on the disk.
 *
 * Returns:
 *  number of tracks or -1 on error.
 */

int Data_Tracks(CDROM *p_cd, unsigned long** p_buf) {
int cnt=0;
t_toc_header hdr;
t_toc_data *toc;
int i, j, len;

	toc = Read_TOC(p_cd, &hdr);
	if (!toc)
		return -1;

	len = hdr.length / 8;

	/* count number of data tracks: */
	for (i=0; i<len; i++)
		if (toc[i].track_number <= 99 && (toc[i].flags & 4))
			cnt++;

	if (cnt == 0)
		return 0;

	/* allocate memory for output buffer: */
	*p_buf = (unsigned long*) AllocVec (cnt * sizeof (unsigned long*), MEMF_PUBLIC);
	if (!*p_buf)
		return -1;

  /* fill output buffer: */
	for (i=0, j=0; i<len; i++)
		if (toc[i].track_number <= 99 && (toc[i].flags & 4))
			(*p_buf)[j++] = toc[i].address;

	return cnt;
}

int Start_Play_Audio(CDROM *p_cd) {
static unsigned char cmd[10] = { 0x48, 0, 0, 0, 0, 1, 0, 99, 1, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;

	if (
			p_cd->use_trackdisk ||
			(p_cd->scsi_compliance == 1 && p_cd->model == MODEL_ANY)
		)
		return FALSE;

	if (p_cd->model == MODEL_CDU_8002)
		cmd[0] = 0xC9;

	cmd[4] = Has_Audio_Tracks(p_cd);

	return Do_SCSI_Command(p_cd,p_cd->buffers[dummy_buf],0,cmd,10,SCSIF_WRITE);
}

int Stop_Play_Audio(CDROM *p_cd) {
static unsigned char cmd[6] = { 0x1B, 0, 0, 0, 0, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;

	if (p_cd->use_trackdisk)
	return FALSE;

	return Do_SCSI_Command(p_cd,p_cd->buffers[dummy_buf],0,cmd,6,SCSIF_WRITE);
}

int Block_Length(CDROM *p_cd) {
static unsigned char cmd[6] = { 0x1A, 0, 1, 0, 100, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;
unsigned char *buf = p_cd->buffers[dummy_buf];

	if (p_cd->use_trackdisk)
		return 0;

	if (!Do_SCSI_Command(p_cd,buf,100,cmd,6,SCSIF_READ))
		return 0;

	if (buf[3] == 0)
		return 0;

	return (buf[9]<<16) + (buf[10]<<8) + buf[11];
}

void Cleanup_CDROM (CDROM *p_cd) {
int bufs = p_cd->std_buffers + p_cd->file_buffers + 1;

	if (p_cd->device_open)
		CloseDevice ((struct IORequest *) p_cd->scsireq);
	if (p_cd->scsireq)
		DeleteIORequest((struct IORequest *)p_cd->scsireq);
	if (p_cd->port)
		DeleteMsgPort (p_cd->port);
	if (p_cd->last_used)
		FreeMem (p_cd->last_used, sizeof (unsigned long) * p_cd->std_buffers);
	if (p_cd->current_sectors)
		FreeMem (p_cd->current_sectors, sizeof (long) * bufs);
	if (p_cd->buffers)
		FreeMem (p_cd->buffers, sizeof (unsigned char *) * bufs);
	if (p_cd->buffer_data)
		FreeMem (p_cd->buffer_data, SCSI_BUFSIZE * bufs + 15);
	FreeMem (p_cd, sizeof (CDROM));
}

void Clear_Sector_Buffers (CDROM *p_cd)
{
  int i;
  int bufs = p_cd->std_buffers + p_cd->file_buffers + 1;

  for (i=0; i<bufs; i++)
    p_cd->current_sectors[i] = -1;
}

/* Finds offset of last session. (Not supported by all CDROM drives)
 *
 * Returns: - FALSE if there is no special SCSI command to determine the
 *            offset of the last session.
 *          - TRUE if the offset of the last session has been determined.
 */

int Find_Last_Session(CDROM *p_cd, unsigned long *p_result)
{
static unsigned char cmd[10] = { 0xC7, 3, 0, 0, 0, 0, 0, 0, 0, 0 };
int dummy_buf = p_cd->std_buffers + p_cd->file_buffers;
unsigned char *buf = p_cd->buffers[dummy_buf];
int min, sec, frame;

	if (p_cd->use_trackdisk)
		return FALSE;

	if (p_cd->model != MODEL_TOSHIBA_3401)
		return FALSE;

	if (
			!Do_SCSI_Command
				(
					p_cd,
					p_cd->buffers[dummy_buf],
					SCSI_BUFSIZE,
					cmd,
					sizeof (cmd),
					SCSIF_READ
				)
		) 
 		return FALSE;

	min = (buf[1] & 0xF) + ((buf[1] & 0xF0) >> 4) * 10;
	sec = (buf[2] & 0xF) + ((buf[2] & 0xF0) >> 4) * 10;
	frame = (buf[3] & 0xF) + ((buf[3] & 0xF0) >> 4) * 10;
	*p_result = frame + sec * 75 + min * (75 * 60) - 150;
	return TRUE;
}
