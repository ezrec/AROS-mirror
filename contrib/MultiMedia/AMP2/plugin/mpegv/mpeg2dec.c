/*
 *
 * mpeg2dec.c
 *
 */

#include <stdio.h>

#include <inttypes.h>
#include <plugin.h>

#include "mpeg2_internal.h"
#include "mpeg2.h"
#include "video_out.h"

static mpeg2dec_t mpeg2dec;
static int mpeg2dec_error = 0;

int vo_setup (void)
{
  double mpeg_frame_rate[16] = {
  30.0/1.001, 24.0/1.001, 24.0,       25.0,       30.0/1.001, 30.0,       50.0,       60.0/1.001,
  60.0,       15.0,       30.0/1.001, 30.0/1.001, 30.0/1.001, 30.0/1.001, 30.0/1.001, 30.0/1.001 };
  uint8_t * alloc;
  int i, size;

  if (mpeg2dec_error != 0) {
    return 1;
  }

  mpeg2dec.current_frame_rate = mpeg_frame_rate[mpeg2dec.frame_rate_code];
  mpeg2dec.current_frame_time = 1.0 / mpeg2dec.current_frame_rate;

  sprintf(amp->verbose, "MPEG Video : %dx%d, %.0f kbit/s, %.3f fps", mpeg2dec.picture->picture_width, mpeg2dec.picture->picture_height, mpeg2dec.picture->bitrate * 400.0 / 1000.0, mpeg2dec.current_frame_rate);
  amp->framerate = mpeg2dec.current_frame_rate;

  amp->video_refresh_yuv2rgb(6694222, -1643170, -3409833, 8460885, 4883779, -617108528, 0x80808080);

  if (amp->video_init(mpeg2dec.picture->picture_width, mpeg2dec.picture->picture_height, mpeg2dec.picture->coded_picture_width, CMODE_YUV420) != PLUGIN_OK) {
    mpeg2dec_error = 1;
    return 1;
  }

  mpeg2dec.prediction_index = 1;
  size = mpeg2dec.picture->coded_picture_width * mpeg2dec.picture->coded_picture_height / 4;
  alloc = mallocalign (18 * size);
  if (alloc == NULL) {
    return 1;
  }

  for (i = 0; i < 3; i++) {
    mpeg2dec.frame[i].base[0] = alloc;
    mpeg2dec.frame[i].base[1] = alloc + 4 * size;
    mpeg2dec.frame[i].base[2] = alloc + 5 * size;
    alloc += 6 * size;
  }

  return 0;
}

void vo_close (void)
{
  if (mpeg2dec.frame[0].base[0] != NULL) {
    freealign (mpeg2dec.frame[0].base[0]);
    mpeg2dec.frame[0].base[0] = NULL;
  }
}

int decode_mpeg(unsigned char *buf, unsigned long length, double sync_time)
{
  if (mpeg2dec_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length == 0) {
    mpeg2dec.search_next_i_frame = 4;
  } else {
    mpeg2dec.sync_time = sync_time;
    amp->total_frames += mpeg2_decode_data(&mpeg2dec, buf, buf + length);
  }

  return PLUGIN_OK;
}

int init_mpeg(unsigned long type, unsigned long fourcc)
{
  memset(&mpeg2dec, 0, sizeof(mpeg2dec));
  mpeg2_init(&mpeg2dec);
  mpeg2dec.sync_time = -1.0;
  mpeg2dec.current_sync_time = 0.0;
  mpeg2dec.current_video_time = 0.0;
  mpeg2dec.current_frame_rate = 0.0;
  mpeg2dec.current_frame_time = 0.0;
  mpeg2dec.search_next_i_frame = 4;
  mpeg2dec.picture->gray = amp->gray;

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  mpeg2dec_error = 0;

  return PLUGIN_OK;
}

int exit_mpeg(void)
{
  mpeg2_close(&mpeg2dec);

  return PLUGIN_OK;
}

VERSION("MPEGVX v1.02 (020904)");

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_MPEG1 | FEATURE_GRAY), FOURCC_NONE, init_mpeg, decode_mpeg, exit_mpeg)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_MPEG2 | FEATURE_GRAY), FOURCC_NONE, init_mpeg, decode_mpeg, exit_mpeg)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_MPEGX | FEATURE_GRAY), FOURCC_NONE, init_mpeg, decode_mpeg, exit_mpeg)
IDENT_EXIT

