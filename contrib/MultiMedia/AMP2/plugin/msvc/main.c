/*
 *
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <plugin.h>

/*
    Microsoft Video 1 Decoder

    (C) 2001 Mike Melanson

    The description of the algorithm you can read here:
      http://www.pcisys.net/~melanson/codecs/

    Heavily optimized by Mathias "AmiDog" Roslund
*/

#define LE_16(val) ({ unsigned short x = *(unsigned short *)(val); x = (x << 8) | (x >> 8); x; })

#define RGB555_TO_RGB888(argb, c) \
  argb = ((c & 0x7c00) << 9) | ((c & 0x03e0) << 6) | ((c & 0x001f) << 3);

#define DECODE_RGB555_TO_RGB888(x) \
  RGB555_TO_RGB888(x.argb[1], x.c1); \
  RGB555_TO_RGB888(x.argb[0], x.c2);

#define CRAM16(x, y) \
  *(unsigned long *)&decoded[pixel_ptr] = quad[x][y].argb[flags & 1]; \
  pixel_ptr += 4; \
  flags >>= 1;

#define CRAM8(x, y) \
  decoded[pixel_ptr++] = quad[x][y].b[flags & 1]; \
  flags >>= 1;

static struct {
  unsigned short c1, c2;
  unsigned long argb[2];
  unsigned char b[2];
} quad[2][2];

void AVI_Decode_Video1_16(unsigned char *encoded, int encoded_size, unsigned char *decoded, int width, int height)
{
  int block_ptr, pixel_ptr;
  int total_blocks;
  int pixel_y;  // pixel height iterator
  int block_x, block_y;  // block width and height iterators
  int blocks_wide, blocks_high;  // width and height in 4x4 blocks
  int block_inc;
  int row_dec;

  // decoding parameters
  int stream_ptr;
  unsigned char byte_a, byte_b;
  unsigned short flags;
  int skip_blocks;

  stream_ptr = 0;
  skip_blocks = 0;
  blocks_wide = width / 4;
  blocks_high = height / 4;
  total_blocks = blocks_wide * blocks_high;
  block_inc = 4 * 4;
  row_dec = (width + 4) * 4;

  for (block_y = blocks_high; block_y > 0; block_y--)
  {
    block_ptr = ((block_y * 4) - 1) * (width * 4);
    for (block_x = blocks_wide; block_x > 0; block_x--)
    {
      // check if this block should be skipped
      if (skip_blocks > 0)
      {
        block_ptr += block_inc;
        skip_blocks--;
        total_blocks--;
        continue;
      }

      pixel_ptr = block_ptr;

      // get the next two bytes in the encoded data stream
      byte_a = encoded[stream_ptr++];
      byte_b = encoded[stream_ptr++];

      // check if the decode is finished
      if ((byte_a == 0) && (byte_b == 0) && (total_blocks == 0))
        return;

      // check if this is a skip code
      else if ((byte_b & 0xFC) == 0x84)
      {
        // but don't count the current block
        skip_blocks = ((byte_b - 0x84) << 8) + byte_a - 1;
      }

      // check if this is in the 2- or 8-color classes
      else if (byte_b < 0x80)
      {
        flags = (byte_b << 8) | byte_a;

        quad[0][0].c1 = LE_16(&encoded[stream_ptr]);
        stream_ptr += 2;
        quad[0][0].c2 = LE_16(&encoded[stream_ptr]);
        stream_ptr += 2;

        DECODE_RGB555_TO_RGB888(quad[0][0]);

        if (quad[0][0].c1 & 0x8000)
        {
          // 8-color encoding
          quad[1][0].c1 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;
          quad[1][0].c2 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;
          quad[0][1].c1 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;
          quad[0][1].c2 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;
          quad[1][1].c1 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;
          quad[1][1].c2 = LE_16(&encoded[stream_ptr]);
          stream_ptr += 2;

          DECODE_RGB555_TO_RGB888(quad[0][1]);
          DECODE_RGB555_TO_RGB888(quad[1][0]);
          DECODE_RGB555_TO_RGB888(quad[1][1]);

          for (pixel_y = 0; pixel_y < 4; pixel_y++)
          {
            CRAM16(0, pixel_y >> 1);
            CRAM16(0, pixel_y >> 1);
            CRAM16(1, pixel_y >> 1);
            CRAM16(1, pixel_y >> 1);
            pixel_ptr -= row_dec;
          }
        }
        else
        {
          // 2-color encoding
          for (pixel_y = 0; pixel_y < 4; pixel_y++)
          {
            CRAM16(0, 0);
            CRAM16(0, 0);
            CRAM16(0, 0);
            CRAM16(0, 0);
            pixel_ptr -= row_dec;
          }
        }
      }

      // otherwise, it's a 1-color block
      else
      {
        unsigned long argb, c;

        c = (byte_b << 8) | byte_a;
        RGB555_TO_RGB888(argb, c);

        for (pixel_y = 0; pixel_y < 4; pixel_y++)
        {
          *(unsigned long *)&decoded[pixel_ptr] = argb;
          pixel_ptr += 4;
          *(unsigned long *)&decoded[pixel_ptr] = argb;
          pixel_ptr += 4;
          *(unsigned long *)&decoded[pixel_ptr] = argb;
          pixel_ptr += 4;
          *(unsigned long *)&decoded[pixel_ptr] = argb;
          pixel_ptr += 4;
          pixel_ptr -= row_dec;
        }
      }

      block_ptr += block_inc;
      total_blocks--;
    }
  }
}

void AVI_Decode_Video1_8(unsigned char *encoded, int encoded_size, unsigned char *decoded, int width, int height)
{
  int block_ptr, pixel_ptr;
  int total_blocks;
  int pixel_y;  // pixel height iterator
  int block_x, block_y;  // block width and height iterators
  int blocks_wide, blocks_high;  // width and height in 4x4 blocks
  int block_inc;
  int row_dec;

  // decoding parameters
  int stream_ptr;
  unsigned char byte_a, byte_b;
  unsigned short flags;
  int skip_blocks;

  stream_ptr = 0;
  skip_blocks = 0;
  blocks_wide = width / 4;
  blocks_high = height / 4;
  total_blocks = blocks_wide * blocks_high;
  block_inc = 4 * 1;
  row_dec = (width + 4) * 1;

  for (block_y = blocks_high; block_y > 0; block_y--)
  {
    block_ptr = ((block_y * 4) - 1) * (width * 1);
    for (block_x = blocks_wide; block_x > 0; block_x--)
    {
      // check if this block should be skipped
      if (skip_blocks > 0)
      {
        block_ptr += block_inc;
        skip_blocks--;
        total_blocks--;
        continue;
      }

      pixel_ptr = block_ptr;

      // get the next two bytes in the encoded data stream
      byte_a = encoded[stream_ptr++];
      byte_b = encoded[stream_ptr++];

      // check if the decode is finished
      if ((byte_a == 0) && (byte_b == 0) && (total_blocks == 0))
        return;

      // check if this is a skip code
      else if ((byte_b & 0xFC) == 0x84)
      {
        // but don't count the current block
        skip_blocks = ((byte_b - 0x84) << 8) + byte_a - 1;
      }

      // check if this is a 2-color block
      else if (byte_b < 0x80)
      {
        flags = (byte_b << 8) | byte_a;

        quad[0][0].b[1] = encoded[stream_ptr++];
        quad[0][0].b[0] = encoded[stream_ptr++];

        // 2-color encoding
        for (pixel_y = 0; pixel_y < 4; pixel_y++)
        {
          CRAM8(0, 0);
          CRAM8(0, 0);
          CRAM8(0, 0);
          CRAM8(0, 0);
          pixel_ptr -= row_dec;
        }
      }

      // check if it's an 8-color block
      else if (byte_b >= 0x90)
      {
        flags = (byte_b << 8) | byte_a;

        quad[0][0].b[1] = encoded[stream_ptr++];
        quad[0][0].b[0] = encoded[stream_ptr++];
        quad[1][0].b[1] = encoded[stream_ptr++];
        quad[1][0].b[0] = encoded[stream_ptr++];

        quad[0][1].b[1] = encoded[stream_ptr++];
        quad[0][1].b[0] = encoded[stream_ptr++];
        quad[1][1].b[1] = encoded[stream_ptr++];
        quad[1][1].b[0] = encoded[stream_ptr++];

        for (pixel_y = 0; pixel_y < 4; pixel_y++)
        {
          CRAM8(0, pixel_y >> 1);
          CRAM8(0, pixel_y >> 1);
          CRAM8(1, pixel_y >> 1);
          CRAM8(1, pixel_y >> 1);
          pixel_ptr -= row_dec;
        }
      }

      // otherwise, it's a 1-color block
      else
      {
        unsigned long c;

        c = byte_a | (byte_a << 8);
        c = c | (c << 16);

        *(unsigned long *)&decoded[pixel_ptr] = c;
        pixel_ptr += 4;
        pixel_ptr -= row_dec;

        *(unsigned long *)&decoded[pixel_ptr] = c;
        pixel_ptr += 4;
        pixel_ptr -= row_dec;

        *(unsigned long *)&decoded[pixel_ptr] = c;
        pixel_ptr += 4;
        pixel_ptr -= row_dec;

        *(unsigned long *)&decoded[pixel_ptr] = c;
        pixel_ptr += 4;
        pixel_ptr -= row_dec;
      }

      block_ptr += block_inc;
      total_blocks--;
    }
  }
}

static unsigned char *buffer = NULL;

static int video_w, video_h;
static int msvc_error = 0;

static double current_time = 0.0;
static double current_frame_time = 0.0;
static double current_video_time = 0.0;

int decode_msvc(unsigned char *buf, unsigned long length, double sync_time)
{
  if (msvc_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    if (amp->depth == 8) {
      AVI_Decode_Video1_8(buf, length, buffer, video_w, video_h);
    } else {
      AVI_Decode_Video1_16(buf, length, buffer, video_w, video_h);
    }

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    amp->video_refresh(&buffer, current_time, current_video_time);
    amp->total_frames++;

    current_time += current_frame_time;
    current_video_time += current_frame_time;
  }

  return PLUGIN_OK;
}

int init_msvc(unsigned long type, unsigned long fourcc)
{
  int width, height;

  width = amp->width;
  height = amp->height;

  buffer = malloc(width * height * 4);
  memset(buffer, 0, width * height * 4);

  video_w = width;
  video_h = height;

  amp->total_frames = 0;
  amp->skiped_frames = 0;

  msvc_error = 0;

  current_time = 0.0;
  current_frame_time = 1.0 / amp->framerate;
  current_video_time = 0.0;

  if (amp->depth == 8) {
    if (amp->video_init(width, height, width, CMODE_CHUNKY) != PLUGIN_OK) {
      msvc_error = 1;
      return PLUGIN_FAIL;
    }
  } else {
    if (amp->video_init(width, height, width, CMODE_ARGB32) != PLUGIN_OK) {
      msvc_error = 1;
      return PLUGIN_FAIL;
    }
  }

  return PLUGIN_OK;
}

int exit_msvc(void)
{
  if (buffer != NULL) {
    free(buffer);
    buffer = NULL;
  }

  return PLUGIN_OK;
}

VERSION("MSVC v1.01 (020520)");

IDENT_INIT
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_MSVC, init_msvc, decode_msvc, exit_msvc)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_msvc, init_msvc, decode_msvc, exit_msvc)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_CRAM, init_msvc, decode_msvc, exit_msvc)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_cram, init_msvc, decode_msvc, exit_msvc)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_WHAM, init_msvc, decode_msvc, exit_msvc)
IDENT_ADD((TYPE_VIDEO | SUBTYPE_NONE), FOURCC_wham, init_msvc, decode_msvc, exit_msvc)
IDENT_EXIT

