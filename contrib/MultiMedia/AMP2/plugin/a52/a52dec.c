/*
 *
 * a52dec.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include <plugin.h>

#include "a52.h"

static a52_state_t *state = NULL;

static inline int16_t convert (float * _f)
{
    int32_t * f = (int32_t *) _f;
    int32_t i = f[0];

    if (i > 0x43c07fff)
     return 32767;
    else if (i < 0x43bf8000)
     return -32768;
    else
     return i - 0x43c00000;
}

/*
 * ao_play *should* be called with 1536 samples per channel (2 channels, 16 bits)
 *
 * This gives 6144 bytes, 3072 samples total and 1536 samples / channel, which
 * is really the most interesting value when it comes to timing.
 *
 * 48000Hz is the used rate, and since we want to play about 4 times/second,
 * we get 12000 samples or 48000 bytes (16bit stereo) for each time we play.
 * Getting 6144 bytes for each write gives 48000/6144 = 7.8125 buffers, thus
 * I'm using 8 buffers.
 *
 */

static unsigned long *ac3_buffer = NULL, *ac3_ptr = NULL;
static int buffer = 0, ac3_samples = 0, ac3_error = 0, ac3_rate = 1, ac3_buffers;

static double real_current_time = 0.0;
static double new_current_time = -1.0;
static double previous_new_current_time = -1.0;
static double sync_time = 0.0;

static int ao_is_open = 0;

int ao_open(uint32_t rate)
{
  ac3_rate = rate;
  rate /= amp->divisor;

  if (amp->audio_init(rate, 16, 2) != PLUGIN_OK) {
    ac3_error = 1;
  }

  ac3_buffer = malloc(48000 * 2 * 2); /* 1 sec 16bit stereo */
  ac3_ptr = ac3_buffer;
  ac3_buffers = (rate + 6143) / 6144;

  return 1;
}

void ao_play(void)
{
  if (ac3_error != 0) {
    return;
  }

  if ((new_current_time > 0.0) && (previous_new_current_time != new_current_time)) {
    real_current_time = new_current_time;
  }

  real_current_time += (double)1536.0 / (double)ac3_rate;

  previous_new_current_time = new_current_time;
  new_current_time = -1.0;

  buffer++;
  if (buffer >= ac3_buffers) {
    amp->audio_refresh(ac3_buffer, ac3_samples, sync_time);


    ac3_ptr = ac3_buffer;
    ac3_samples = 0;
    buffer = 0;

    sync_time = real_current_time;
  }
}

void a52_decode_data (uint8_t * start, uint8_t * end)
{
    static uint8_t buf[3840];
    static uint8_t * bufptr = buf;
    static uint8_t * bufpos = buf + 7;

    /*
     * sample_rate and flags are static because this routine could
     * exit between the a52_syncinfo() and the ao_setup(), and we want
     * to have the same values when we get back !
     */

    static int sample_rate;
    static int flags;
    int bit_rate;
    int len;

    while (1) {
     len = end - start;
     if (!len)
         break;
     if (len > bufpos - bufptr)
         len = bufpos - bufptr;
     memcpy (bufptr, start, len);
     bufptr += len;
     start += len;
     if (bufptr == bufpos) {
         if (bufpos == buf + 7) {
          int length;

          length = a52_syncinfo (buf, &flags, &sample_rate, &bit_rate);
          if (!length) {
              for (bufptr = buf; bufptr < buf + 6; bufptr++)
               bufptr[0] = bufptr[1];
              continue;
          }
          bufpos = buf + length;
         } else {
          sample_t level, bias, *samples;
          int i;

          if (ao_is_open == 0) {
            static const uint8_t nfchans_tbl[] = {2, 1, 2, 3, 3, 4, 4, 5, 1, 1, 2};
            sprintf(amp->verbose, "AC3: %d.%d Mode, %d kbit/s, %d Hz",
              nfchans_tbl[flags & A52_CHANNEL_MASK],
              (flags & A52_LFE) >> 4,
              bit_rate / 1000,
              sample_rate);
            ao_open(sample_rate);
            ao_is_open = 1;
          }

          level = 1;
          bias = 384;

          flags = A52_STEREO;
          flags |= A52_ADJUST_LEVEL;

          if (a52_frame (state, buf, &flags, &level, bias))
              goto error;
          for (i = 0; i < 6; i++) {
            int j, step = amp->divisor;
            if (a52_block (state))
              goto error;
            samples = a52_samples (state);
            for (j=0; j<256; j+=step) {
              *ac3_ptr++ = (convert(samples) << 16) | (convert(samples + 256) & 0x0000ffff);
              samples += step;
            }
            ac3_samples += (256 * 2 * 2) / step;
          }
          ao_play ();
error:
          bufptr = buf;
          bufpos = buf + 7;
         }
     }
    }
}

int init_a52 (unsigned long type, unsigned long fourcc)
{
  ao_is_open = 0;
  ac3_error = 0;

  state = a52_init (0);

  if (state == NULL) {
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int decode_a52 (unsigned char *buf, unsigned long length, double sync_time)
{
  if (ac3_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    new_current_time = sync_time;
    a52_decode_data (buf, buf + length);
  }

  return PLUGIN_OK;
}

int exit_a52 (void)
{
  if (state != NULL) {
    a52_free (state);
    state = NULL;
  }

  return PLUGIN_OK;
}

VERSION("A52 v1.01 (020628)");

IDENT_INIT
IDENT_ADD((TYPE_AUDIO | SUBTYPE_A52 | FEATURE_DIVISOR_2 | FEATURE_DIVISOR_4), FOURCC_NONE, init_a52, decode_a52, exit_a52)
IDENT_EXIT

