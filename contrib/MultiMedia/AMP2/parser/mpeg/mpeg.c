/*
 *
 * mpeg.c
 *
 */

/* FIXME: These functions suxx */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else
#include "aros-inc.h"
#endif

#include "mpeg.h"
#include "../../common/core.h"
#include "../../common/ampio.h"
#include "../../main/main.h"
#include "../../main/buffer.h"
#include "../../main/prefs.h"

/* General global variables */

static int audio_read = FALSE, video_read = FALSE, ac3_read = FALSE;
static int no_audio = FALSE, no_video = FALSE, no_ac3 = FALSE;
static int decoders_running = FALSE;
static int seek_to_position = -1;

int start_decoders(void)
{
  if (decoders_running == TRUE) {
    return TRUE;
  }

  decoders_running = TRUE;

  plugin_init(); /* FIXME: Return value */

  if (video_read == TRUE) {
    if (plugin_video_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0, 0.0) != CORE_OK) {
      no_video = TRUE;
    }
  } else {
    no_video = TRUE; /* Just in case */
  }

  if (audio_read == TRUE) {
    if (plugin_audio_init(SUBTYPE_MPEGX, FOURCC_NONE, 0, 0, 0) != CORE_OK) {
      no_audio = TRUE;
    }
  } else {
    no_audio = TRUE; /* Just in case */
  }

  if (ac3_read == TRUE) {
    if (plugin_audio_init(SUBTYPE_A52, FOURCC_NONE, 0, 0, 0) != CORE_OK) {
      no_ac3 = TRUE;
    }
  } else {
    no_ac3 = TRUE; /* Just in case */
  }

  /* Did all fail ? */

  if ((no_video == TRUE) && (no_audio == TRUE) && (no_ac3 == TRUE)) {
    return FALSE;
  }

  plugin_start();

  return TRUE;
}

int exit_demuxer(void)
{
  static struct timeval st = { 0, 0 };
  struct timeval ct, dt;
  static int old_pos = 0;
  int check, set = -1;

  if (prefs.speedtest & 0x01) {
    volatile unsigned char *lmb = (unsigned char *)0xbfe001;
    if ((*lmb & 0x40) == 0) { /* LMB */
      return TRUE;
    } else {
      return FALSE;
    }
  }

  if (decoders_running == FALSE) {
    return FALSE;
  }

  /* Only check for IDCMP once every 1/5 seconds */
  GetSysTimePPC(&ct);
  dt = ct;
  SubTimePPC(&dt, &st);

  if ((dt.tv_sec == 0) && (dt.tv_usec < 200000)) {
    return FALSE;
  }

  st = ct;

  /* Calculate new slider position */
  if ((prefs.window == PREFS_ON) || (no_video == TRUE)) {
    int pos;

    if (no_video == TRUE) {
      pos = audio_ftell();
    } else {
      pos = video_ftell();
    }

    if (pos != old_pos) {
      long long length;

      length = (long long)ampio_length();

      set = (int)(((long long)pos * (long long)65535) / length);

      /* Just in case */
      if (set < 0) {
        set = 0;
      } else if (set > 65535) {
        set = 65535;
      }

      old_pos = pos;
    }
  }

  check = core_input(set);

  if (check >= 0) {
    long long length, position;

    length = (long long)ampio_length();

    position = (long long)check; /* MAX 100 % */

    seek_to_position = (int)((length * position) / (long long)100);
  } else if (check == INPUT_QUIT) {
    video_fempty();
    audio_fempty();
    return TRUE;
  }

  return FALSE;
}

#define AUDIO_STREAM_ID 0xc0
#define VIDEO_STREAM_ID 0xe0
#define AC3_STREAM_ID   0xbd
#define SUB_STREAM_ID   0xff /* HACK */

#define PACK_START_CODE          0x000001ba
#define SYSTEM_HEADER_START_CODE 0x000001bb
#define ISO_11172_END_CODE       0x000001b9

#define PACK_HEADER_SIZE 8

static unsigned char *inputBuffer = NULL;

int ReadFirstStartCode(unsigned int *startCode)
{
  if (ampio_fread((unsigned char *)startCode, 4) != 4) {
    return 1;
  }

  for (;;) {
    unsigned char byte;

    /* MPEG system/video stream */
    if ((*startCode & 0xffffff00) == 0x00000100) {
      return 0;
    }

    if (ampio_fread(&byte, 1) != 1) {
      return 1;
    }

    *startCode = (*startCode<<8)|byte;
  }

  return 0;
}

int ReadStartCode(unsigned int *startCode)
{
  if (ampio_fread((unsigned char *)startCode, 4) != 4) {
    return 1;
  }

  for (;;) {
    unsigned char byte;

    /* MPEG system/video stream */
    if ((*startCode & 0xffffff00) == 0x00000100) {
      return 0;
    }

    if (ampio_fread(&byte, 1) != 1) {
      return 1;
    }

    *startCode = (*startCode<<8)|byte;
  }

  return 0;
}

int ReadSystemHeader()
{
  unsigned short headerSize;

  if (ampio_fread((char *)&headerSize, 2) != 2) {
    return 1;
  }

  if (ampio_fseek(headerSize, SEEK_CUR) == 0) {
    return 1;
  }

  return 0;
}

double Buf2PTS(unsigned char *buf)
{
  unsigned long long_pts;
  double pts;

  /* The PTS is 33 bits but I just care about the 31 most significant
     bits to keep the result fit in a 32 bit signed variable since the
     conversion to double appears to require it, atleast I got negative
     values when using the 32 least significant bits instead */

  /* 28-30 */ long_pts = (((buf[0] >> 1) & 0x07) << 28);
  /* 20-27 */ long_pts |= (buf[1] << 20);
  /* 13-19 */ long_pts |= ((buf[2] >> 1) << 13);
  /*  5-12 */ long_pts |= (buf[3] << 5);
  /*  0-4  */ long_pts |= (buf[4] >> 3);

  /* 90000 >> 2 = 22500 because (33 bits >> 2 bits = 31 bits) */

  pts = (double)long_pts / (double) 22500;

  /* This should never happend but is here just in case */
  if (pts < 0.0) {
    pts = -1.0;
  }

  return pts;
}

int ReadPacket(unsigned char packetID)
{
  int packetDataLength, pos, file_pos;
  unsigned short packetLength;
  unsigned char nextByte;
  double pts = -1.0;

  if (ampio_fread((unsigned char *)&packetLength, 2) != 2) {
    return 1;
  }

  /* Skip everything but audio, video and ac3 streams */
  if ((packetID != AUDIO_STREAM_ID) && (packetID != VIDEO_STREAM_ID) && (packetID != AC3_STREAM_ID)) {
    if (ampio_fseek(packetLength, SEEK_CUR) == 0) {
      return 1;
    }
    return 0;
  }

  file_pos = ampio_ftell(); /* may give a few bytes wrong below, but who cares ? */

  if (ampio_fread(inputBuffer, packetLength) != packetLength) {
    return 1;
  }

  pos = 0;

  if ((inputBuffer[pos] >> 6) == 0x02) {
    /* Parse MPEG2 PES packet */
    int PES_packet_length, PTS_DTS_flags;

    PTS_DTS_flags = inputBuffer[pos+1] >> 6;
    PES_packet_length = inputBuffer[pos+2];

    if (PTS_DTS_flags == 0x02) {
      /* Get PTS */
      pts = Buf2PTS(&inputBuffer[pos+3]);
    } else if (PTS_DTS_flags == 0x03) {
      /* Get DTS so I don't need to bother about frame reordering and such */
      pts = Buf2PTS(&inputBuffer[pos+8]);
    }
    pos += PES_packet_length + 3;
  } else {
    while (inputBuffer[pos] & 0x80) {
      pos++;
    }

    nextByte = inputBuffer[pos];

    if ((nextByte >> 6) == 0x01) {
      pos += 2;
      nextByte = inputBuffer[pos];
    }

    if ((nextByte >> 4) == 0x02) {
      /* Get PTS */
      pts = Buf2PTS(&inputBuffer[pos+0]);
      pos += 5;
    } else if ((nextByte >> 4) == 0x03) {
      /* Get DTS so I don't need to bother about frame reordering and such */
      pts = Buf2PTS(&inputBuffer[pos+5]);
      pos += 10;
    } else {
      pos += 1;
    }
  }

  /* Special AC3 handling */
  if ((packetID == AC3_STREAM_ID) && (inputBuffer[pos] >= 0x80) && (inputBuffer[pos] <= 0x87)) {
    if (no_ac3 == TRUE) return 0;
    if (inputBuffer[pos] == prefs.ac3_track) {
      pos += 4;
    } else {
      return 0;
    }
  }

  /* Special SUB handling */
  else if ((packetID == AC3_STREAM_ID) && (inputBuffer[pos] >= 0x20) && (inputBuffer[pos] <= 0x3f)) {
    if (inputBuffer[pos] == prefs.subtitle_track) {
      packetID = SUB_STREAM_ID; /* HACK */
      pos += 1;
    } else {
      return 0;
    }
  }

  packetDataLength = packetLength - pos;

  if (packetID == VIDEO_STREAM_ID) {
    if (no_video == TRUE) return 0;
    video_read = TRUE;
    if (exit_demuxer() == TRUE) {
      return 1;
    }
    while (video_fwrite(inputBuffer + pos, packetDataLength, pts, file_pos) == BUF_FALSE) {
      /* Buffer full, can't write */
      if (start_decoders() == FALSE) {
        return 1;
      }
    }
  }
  else if (packetID == AUDIO_STREAM_ID) {
    if (no_audio == TRUE) return 0;
    audio_read = TRUE;
    no_ac3 = TRUE; /* Just in case */
    if (exit_demuxer() == TRUE) {
      return 1;
    }
    while (audio_fwrite(inputBuffer + pos, packetDataLength, pts, file_pos) == BUF_FALSE) {
      /* Buffer full, can't write */
      if (start_decoders() == FALSE) {
        return 1;
      }
    }
  }
  else if (packetID == AC3_STREAM_ID) {
    if (no_ac3 == TRUE) return 0;
    ac3_read = TRUE;
    no_audio = TRUE; /* Just in case */
    if (exit_demuxer() == TRUE) {
      return 1;
    }
    while (audio_fwrite(inputBuffer + pos, packetDataLength, pts, file_pos) == BUF_FALSE) {
      /* Buffer full, can't write */
      if (start_decoders() == FALSE) {
        return 1;
      }
    }
  }
  else if (packetID == SUB_STREAM_ID) {
    if (exit_demuxer() == TRUE) {
      return 1;
    }
    while (subtitle_fwrite(inputBuffer + pos, packetDataLength, pts, file_pos) == BUF_FALSE) {
    }
  }

  return 0;
}

void split_stream()
{
  unsigned int startCode;

  inputBuffer = malloc(65536); /* Maximum value with 16 bits */
  if (inputBuffer == NULL) {
    return;
  }

  if (ReadFirstStartCode(&startCode) != 0) {
    amp_printf("Unable to read initial pack start code\n");
    return;
  }

    while (ampio_feof() == FALSE) {
      if (seek_to_position != -1) {
        if (ampio_fseek(seek_to_position, SEEK_SET) == 0) {
          break;
        }
        seek_to_position = -1;

        if (video_read == TRUE) {
          video_fseek();
        }
        if ((audio_read == TRUE) || (ac3_read == TRUE)) {
          audio_fseek();
        }
        if (prefs.subtitle_track) {
          subtitle_fseek();
        }
      }

      /* Search the next start code */
      if (ReadStartCode(&startCode) != 0) {
        break;
      }

      if (startCode == PACK_START_CODE) {
        if (ampio_fseek(PACK_HEADER_SIZE, SEEK_CUR) == 0) {
          break;
        }
      } else if (startCode == SYSTEM_HEADER_START_CODE) {
        if (ReadSystemHeader() != 0) {
          break;
        }
      } else if (startCode == ISO_11172_END_CODE) {
        /* Just ignore the end code */
      } else {
        if (ReadPacket(startCode & 0xff) != 0) {
          break;
        }
      }
    }
    if (decoders_running == FALSE) {
      start_decoders();
    }
}

int mpeg_system_parser(char *filename, int video, int audio)
{
  if (ampio_fopen(filename) != 1) {
    return 0;
  }

  if (audio == 0) {
    no_audio = TRUE;
    no_ac3 = TRUE;
  }

  if (video == 0) {
    no_video = TRUE;
  }

  /* This has to be done before starting the MPEGV decoder */
  if (prefs.subtitle_track) {
    subtitle_fopen();
  }

  split_stream();

  if (prefs.subtitle_track) {
    subtitle_feof();
  }

  plugin_exit();

  if (prefs.subtitle_track) {
    subtitle_fclose();
  }

  ampio_fclose();

  return 0;
}
