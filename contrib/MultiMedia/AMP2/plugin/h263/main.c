/*
 *
 * main.c
 *
 */

/* FFMPEG wrapper */

#include <stdio.h>

#include <plugin.h>

#include "avcodec.h"

static AVCodec *avcodec;
static AVCodecContext codec_context, *c = &codec_context;

int coded_width = 0; /* AmiDog: FIXME!? */

static int h263_error = 0;

static double current_time = 0.0;
static double current_frame_time = 0.0;
static double current_video_time = 0.0;

static unsigned long decoder = 0x00000000;

int DecodeH263(unsigned char *buf, unsigned long length, double sync_time)
{
  AVPicture picture;
  int got_picture, len;

  if (h263_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    len = avcodec_decode_video(c, &picture, &got_picture, buf, length);

    if (len < 0) {
      return PLUGIN_OK; /* Don't let one broken frame ruin our day! */
    }

    if (got_picture) {
      if (sync_time > 0.0) {
        current_time = sync_time;
      }

      /* RM's don't have a constant framerate, so we MUST use the actual sync time */
      if ((decoder == CODEC_ID_RV10) || (decoder == CODEC_ID_RV10)) {
        amp->video_refresh(picture.data, current_time, current_time);
      } else {
        amp->video_refresh(picture.data, current_time, current_video_time);
      }
      amp->total_frames++;

      current_time += current_frame_time;
      current_video_time += current_frame_time;
    }
  }

  return PLUGIN_OK;
}

int SetupH263(unsigned long type, unsigned long fourcc)
{
  /* must be called before using avcodec lib */
  avcodec_init();

  /* register all the codecs (you can also register only the codec
     you wish to have smaller code */
  avcodec_register_all();

  /* put default values */
  memset(c, 0, sizeof(*c));

  c->width = amp->width;
  c->height = amp->height;
  coded_width = amp->width;

  switch (fourcc) {
    case FOURCC_U263:
      decoder = CODEC_ID_H263;
      break;

    case FOURCC_I263:
      decoder = CODEC_ID_H263I;
      break;

    case FOURCC_MJPG:
    case FOURCC_jpeg:
      decoder = CODEC_ID_MJPEG;
      break;

    case FOURCC_DIVX:
    case FOURCC_divx:
    case FOURCC_MPG4:
      decoder = CODEC_ID_MPEG4;
      break;

    case FOURCC_DIV3:
    case FOURCC_MP43:
      decoder = CODEC_ID_MSMPEG4;
      break;

    case FOURCC_RV10:
      decoder = CODEC_ID_RV10;
      c->sub_id = 0;
      break;

    case FOURCC_RV13:
      decoder = CODEC_ID_RV10;
      c->sub_id = 3;
      break;

    default:
      decoder = 0x00000000;
      return PLUGIN_FAIL;
      break;
  }

  avcodec = avcodec_find_decoder(decoder);
  if (!avcodec) {
    return PLUGIN_FAIL;
  }

  /* open it */
  if (avcodec_open(c, avcodec) < 0) {
    return PLUGIN_FAIL;
  }

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  h263_error = 0;

  current_time = 0.0;
  current_frame_time = 1.0 / amp->framerate;
  current_video_time = 0.0;

  amp->video_refresh_yuv2rgb(6694222, -1643170, -3409833, 8460885, 4883779, -617108528, 0x80808080);

  if (amp->video_init(amp->width, amp->height, coded_width, CMODE_YUV420) != PLUGIN_OK) {
    h263_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int ExitH263(void)
{
  avcodec_close(c);

  return PLUGIN_OK;
}

VERSION("H263 v1.02 (020904)");

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_U263, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_I263, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_MJPG, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_jpeg, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_DIVX, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_divx, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_MPG4, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_DIV3, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_MP43, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_RV10, SetupH263, DecodeH263, ExitH263)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_RV13, SetupH263, DecodeH263, ExitH263)
IDENT_EXIT

