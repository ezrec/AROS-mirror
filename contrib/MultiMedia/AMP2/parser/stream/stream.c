/*
 *
 * stream.c
 *
 */

#include <stdio.h>
#include <sys/time.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else
#include "aros-inc.h"
#endif


#include "../../common/core.h"
#include "../../common/ampio.h"
#include "../../main/buffer.h"

#define WRITE_SIZE (32*1024)

static int generic_parser(char *filename, int buffer, unsigned long type, unsigned long fourcc)
{
  struct timeval st = { 0, 0 };
  struct timeval ct, dt;

  int decoders_running = 0, check, set, res;
  long long length, pos;

  if (ampio_fopen(filename) != 1) {
    return 0;
  }

  /* Get file length */
  length = (long long)ampio_length();

  for (;;) {
    /* End of file */
    if (ampio_feof()) {
      WaitTime(0, 100000); /* 0.10s */
      res = BUF_FALSE; /* Start decoders */
    } else {
      /* Write to buffer */
      res = amp_fwrite(buffer, NULL, WRITE_SIZE, 0.0, ampio_ftell());
    }

    if ((res == BUF_FALSE) && (decoders_running == 0)) {
      decoders_running = 1;

      plugin_init();

      if (buffer == 0) {
        if (plugin_video_init(type, fourcc, 0, 0, 0, 0.0) != CORE_OK) {
          break;
        }
      }

      if (buffer == 1) {
        if (plugin_audio_init(type, fourcc, 0, 0, 0) != CORE_OK) {
          break;
        }
      }

      plugin_start();
    }

    if (decoders_running == 1) {
      /* Only check for IDCMP once every 1/5 seconds */
      GetSysTimePPC(&ct);
      dt = ct;
      SubTimePPC(&dt, &st);

      if ((dt.tv_sec != 0) || (dt.tv_usec > 200000)) {
        st = ct;

        /* Check for input */
        pos = (long long)amp_ftell(buffer);

        set = (int)((pos * (long long)65535) / length);
        check = core_input(set);

        /* Should we seek ? */
        if (check >= 0) {
          pos = (long long)check; /* MAX 100 % */

          set = (int)((pos * length) / (long long)100);

          if (ampio_fseek(set, SEEK_SET) == 1) {
            amp_fseek(buffer);
          }
        } else if (check == INPUT_QUIT) {
          amp_fempty(buffer);
          goto exit;
        }
      }
    }
  }

exit:

  plugin_exit();

  return 0;
}

int mpeg_video_parser(char *filename, int video, int audio)
{
  if (video == 1) {
    generic_parser(filename, BUFFER_VIDEO, SUBTYPE_MPEGX, FOURCC_NONE);
  }

  return 0;
}

int mpeg_audio_parser(char *filename, int video, int audio)
{
  if (audio == 1) {
    generic_parser(filename, BUFFER_AUDIO, SUBTYPE_MPEGX, FOURCC_NONE);
  }

  return 0;
}

int a52_parser(char *filename, int video, int audio)
{
  if (audio == 1) {
    generic_parser(filename, BUFFER_AUDIO, SUBTYPE_A52, FOURCC_NONE);
  }

  return 0;
}
