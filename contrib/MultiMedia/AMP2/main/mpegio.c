/*
 *
 * mpegio.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <devices/trackdisk.h>
#include <devices/scsidisk.h>

#if !defined(__AROS__)
#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcinline/alib.h>
#else

#include "aros-inc.h"

#endif

#include "mpegio.h"
#include "mount.h"
#include "../main/main.h"
#include "../main/prefs.h"
#include "../amigaos/req.h"

/* Extern functions */

extern void dvd_main(void);
extern void vcd_main(int min_sector, int max_sector, int sector_size);
extern void svcd_main(int min_sector, int max_sector, int sector_size);

#define MPEGIO_NONE 0x00
#define MPEGIO_VCD  0x02 /* MPEG1 */
#define MPEGIO_CDI  0x04
#define MPEGIO_DVD  0x08
#define MPEGIO_SVD  0x10 /* MPEG2 */

#define MPEGIO_CD (MPEGIO_VCD | MPEGIO_CDI | MPEGIO_DVD | MPEGIO_SVD)

#define VCD_SECTOR_SIZE 2324
#define DVD_SECTOR_SIZE 2048
#define RAW_SECTOR_SIZE 2352
#define SECTORS_AT_ONCE 16
#define MPEGIO_BUFFER_SIZE (RAW_SECTOR_SIZE*SECTORS_AT_ONCE)

static unsigned char *mpegio_buffer = NULL;
static int input = MPEGIO_NONE;
static int mpegio_filesize = 0;
static int mpeg_sector_size = VCD_SECTOR_SIZE;

/* DVD variables and functions */

/* VCD variables and functions */

#define SCSI_READ_CD     0xbe
#define SCSI_READ_TOC    0x43
#define SCSI_READ10      0x28
#define SCSI_MODE_SEL6   0x15
#define SCSI_INQUIRY     0x12

#define SENSELEN 32
static unsigned char sensebuf[SENSELEN];

static struct MsgPort *CDMP = NULL;
static struct IOExtTD *CDIO = NULL;
static int cd_error = 1; /* Must be non-zero */
static int min_frame = 0;
static int current_frame = 0;
static int max_frame = 0;
static int buf_frame = 0;
static int buf_frame_size = 0;

typedef struct {
  char reserved1;
  unsigned char flags;
  unsigned char track_number;
  char reserved2;
  long address;
} TOCENTRY;

typedef struct {
  unsigned short size;
  unsigned char first;
  unsigned char last;
  TOCENTRY track[100];
} TOC;

static TOC toc;

int cd_read(unsigned char *dst, int sector, int length)
{
  CDIO->iotd_Req.io_Data    = dst;
  CDIO->iotd_Req.io_Command = CMD_READ;
  CDIO->iotd_Req.io_Length  = (length * 2048);
  CDIO->iotd_Req.io_Offset  = (sector * 2048);
  DoIO((struct IORequest *)CDIO);
  if (CDIO->iotd_Req.io_Error != 0) {
    return -1;
  }
  return 0;
}

void ExecSCSICmd(unsigned char *cmd,int cmdsize,unsigned char *data,int datasize, unsigned char direction)
{
  struct SCSICmd scmd;

  CDIO->iotd_Req.io_Command = HD_SCSICMD;
  CDIO->iotd_Req.io_Data    = &scmd;
  CDIO->iotd_Req.io_Length  = sizeof(scmd);

  scmd.scsi_Data        = (APTR)data;
  scmd.scsi_Length      = datasize;
  scmd.scsi_SenseActual = 0;
  scmd.scsi_SenseData   = sensebuf;
  scmd.scsi_SenseLength = SENSELEN;
  scmd.scsi_Command     = cmd;
  scmd.scsi_CmdLength   = cmdsize;
  scmd.scsi_Flags       = SCSIF_AUTOSENSE | direction;

  DoIO((struct IORequest *)CDIO);
}

void ExecReadTOC(void)
{
  unsigned char cdb[10];

  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_READ_TOC;
  cdb[7] = sizeof(toc)>>8;
  cdb[8] = sizeof(toc)&255;

  ExecSCSICmd(cdb,sizeof(cdb),(unsigned char *)&toc,sizeof(toc),SCSIF_READ);
}

/* Used by the UDF parser */

#define PRECACHE 32

static unsigned char *precache_buf;
static int precache_lb;

int UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data)
{
  int res = block_count;

  while (block_count > 0) {
    int offset = lb_number - precache_lb;

    if ((offset < 0) || (offset > (PRECACHE - 1))) {
      unsigned char cdb[10];

      offset = 0;
      precache_lb = lb_number;
      memset(cdb,0,sizeof(cdb));

      cdb[0] = SCSI_READ10;
      cdb[2] = precache_lb>>24;
      cdb[3] = precache_lb>>16;
      cdb[4] = precache_lb>>8;
      cdb[5] = precache_lb;
      cdb[6] = (PRECACHE)>>16; /* read x frame(s) */
      cdb[7] = (PRECACHE)>>8;
      cdb[8] = (PRECACHE);

      ExecSCSICmd(cdb,sizeof(cdb),precache_buf,(2048*PRECACHE),SCSIF_READ);

      if (CDIO->iotd_Req.io_Error != 0) {
        return -1;
      }
    }

    memcpy(data, precache_buf + (2048*offset), 2048);
    lb_number++;
    block_count--;
    data += 2048;
  }

  return res;
}

void (*ExecRead)(unsigned long frame,unsigned char *buf,int frames);

void ExecReadMMC(unsigned long frame,unsigned char *buf,int frames)
{
  unsigned char cdb[12];

  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_READ_CD;
  cdb[2] = frame>>24;
  cdb[3] = frame>>16;
  cdb[4] = frame>>8;
  cdb[5] = frame;
  cdb[8] = frames; /* read x frame(s) */
  cdb[9] = 0x10; /* User data, 2328 bytes */

  ExecSCSICmd(cdb,sizeof(cdb),buf,(frames*2328),SCSIF_READ);
}

void ExecReadStandard(unsigned long frame,unsigned char *buf,int frames)
{
  unsigned char cdb[10];

  memset(cdb,0,sizeof(cdb));

  cdb[0] = SCSI_READ10;
  cdb[2] = frame>>24;
  cdb[3] = frame>>16;
  cdb[4] = frame>>8;
  cdb[5] = frame;
  cdb[8] = frames; /* read x frame(s) */

  ExecSCSICmd(cdb,sizeof(cdb),buf,(frames*2324),SCSIF_READ);
}

char vendor[9], product[17], revision[5];

void GetBrand(void)
{
  unsigned char cdb[6], buf[36];

  memset(cdb,0,sizeof(cdb));
  memset(buf,0,sizeof(buf));

  cdb[0] = SCSI_INQUIRY;
  cdb[4] = sizeof(buf);

  ExecSCSICmd(cdb,sizeof(cdb),buf,sizeof(buf),SCSIF_READ);

  amp_printf("Type: %d\n", buf[0]);

  strncpy(vendor, &buf[8], 8);
  vendor[8] = '\0';
  amp_printf("Vendor: %s\n", vendor);

  strncpy(product, &buf[16], 16);
  product[16] = '\0';
  amp_printf("Product: %s\n", product);

  strncpy(revision, &buf[32], 4);
  revision[4] = '\0';
  amp_printf("Revision: %s\n", revision);
}

/* For Toshiba/DEC and HP drives */

/*
  reserved, Medium type=0, Dev spec Parm = 0, block descriptor len 0 oder 8,
  Density (cd format) 
  (0=YellowBook, XA Mode 2=81h, XA Mode1=83h and raw audio via SCSI=82h),
  # blks msb, #blks, #blks lsb, reserved,
  blocksize, blocklen msb, blocklen lsb,
*/

void ExecModeSelect(unsigned char density)
{
  unsigned char cdb[6],param[12];

  memset(cdb,0,sizeof(cdb));
  memset(param,0,sizeof(param));

  cdb[0] = SCSI_MODE_SEL6;
  cdb[1] = 0x10; /* SCSI-2 Page format */
  cdb[4] = sizeof(param);

  param[3] = 8; /* size of block descpritor */
  param[4] = density; /* Read above */
  param[10] = (mpeg_sector_size >> 8) & 0xff;
  param[11] = (mpeg_sector_size >> 0) & 0xff;

  ExecSCSICmd(cdb,sizeof(cdb),param,sizeof(param),SCSIF_WRITE);
}

int SetupVCD(void)
{
  unsigned char data[RAW_SECTOR_SIZE];
  int mode = 0x00;

  buf_frame_size = 2328;
  ExecRead = ExecReadMMC; /* Set default using READ_CD */

  ExecRead(min_frame, data, 1);
  if (CDIO->iotd_Req.io_Error == 0) {
    amp_printf("READ_CD Supported!\n");
    return 0;
  }

  amp_printf("READ_CD NOT Supported!\n");

  buf_frame_size = 2324;
  ExecRead = ExecReadStandard; /* Use READ10 command */

  if (!memcmp(vendor, "TOSHIBA", 7) || !memcmp(vendor, "IBM", 3) || !memcmp(vendor, "DEC", 3)) {
    amp_printf("TOSHIBA, IBM or DEC\n");
    mode = 0x81;
  }

  amp_printf("Trying to set mode: %02x, sector size: %d\n", mode, mpeg_sector_size);
  ExecModeSelect(mode);

  ExecRead(min_frame, data, 1);
  if (CDIO->iotd_Req.io_Error != 0) {
    amp_printf("CD reading didn't work!\n");
    return -1;
  }

  return 0;
}

void mpegio_fclose(void)
{
  if (cd_error == 0) {
    CloseDevice(CDIO);
    cd_error = 1; /* Must be non-zero */
  }
  if (CDIO != NULL) {
    DeleteIORequest(CDIO);
    CDIO = NULL;
  }
  if (CDMP != NULL) {
    DeleteMsgPort(CDMP);
    CDMP = NULL;
  }
  if (mpegio_buffer != NULL) {
    free(mpegio_buffer);
    mpegio_buffer = NULL;
  }
  if (precache_buf != NULL) {
    free(precache_buf);
    precache_buf = NULL;
  }
}

int mpegio_fopen(char *filename)
{
  if ((strcasecmp(filename, "vcd") == 0) ||
      (strcasecmp(filename, "cdi") == 0) ||
      (strcasecmp(filename, "dvd") == 0)) {
    input = MPEGIO_CD;
  } else {
    return FALSE;
  }

  precache_lb = -PRECACHE;
  precache_buf = malloc(2048 * PRECACHE);
  if (precache_buf == NULL) {
    return FALSE;
  }
  memset(precache_buf, 0, (2048 * PRECACHE));

  /* Open CD device and atempt to autodetect CD type */
  if (input & MPEGIO_CD) {
    CDMP = CreateMsgPort();
    if (CDMP != NULL) {
      CDIO = (struct IOExtTD *)CreateIORequest(CDMP, sizeof(struct IOExtTD));
      if (CDIO != NULL) {
        cd_error = OpenDevice(prefs.device, prefs.unit, (struct IORequest *)CDIO, 0);
        if (cd_error != 0) {
          amp_printf("could not open device %s unit %d\n", prefs.device, prefs.unit);
          return FALSE;
        } else {
          memset(&toc, 0, sizeof(toc));
          ExecReadTOC();
          input = mount_cd(&min_frame, &max_frame);
          if (input == MPEGIO_NONE) {
            amp_printf("unknown CD found at %s unit %d\n", prefs.device, prefs.unit);
            return FALSE;
          }
        }
      }
    }
  }

  if ((input == MPEGIO_VCD) || (input == MPEGIO_CDI) || (input == MPEGIO_SVD)) {
    mpeg_sector_size = VCD_SECTOR_SIZE;
    current_frame = min_frame;
    mpegio_filesize = max_frame - min_frame; /* Filesize in frames */
    buf_frame = 0;

    /* Experimental stuff */
    GetBrand();
    if (SetupVCD() < 0) {
      return FALSE;
    }
    if (input == MPEGIO_VCD) {
      strcat(prefs.window_title, "VCD");
    } else if (input == MPEGIO_SVD) {
      strcat(prefs.window_title, "SVCD");
    } else {
      strcat(prefs.window_title, "CD-i");
    }

    if (input == MPEGIO_SVD) {
      svcd_main(min_frame, max_frame, buf_frame_size);
    } else {
      vcd_main(min_frame, max_frame, buf_frame_size);
    }
    return TRUE;
  } else if (input == MPEGIO_DVD) {
    prefs.osd = PREFS_ON; /* enable DVD menus */
    dvd_main();
    return TRUE;
  }

  return FALSE;
}

/* Plugin startup */

void mpeg_plugin(char *filename)
{
  /* FIXME: Return value ? */
  mpegio_fopen(filename);

  mpegio_fclose();
}
