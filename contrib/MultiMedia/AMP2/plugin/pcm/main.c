/*
 *
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <plugin.h>

static void (*DecodePCM)(void *src, void *dst, unsigned long size);
static unsigned char *audio = NULL;

void DecodePCM8Mono(void *src, void *dst, unsigned long size)
{
  unsigned long *from, *to;
  unsigned long s, d;

  s = (size + 3) >> 2;
  from = (unsigned long *)src;
  to = (unsigned long *)dst;

  while(s--) {
    d = *from++;
    d ^= 0x80808080;
    *to++ = d;
  }
}

void DecodePCM8Stereo(void *src, void *dst, unsigned long size)
{
  unsigned long *from, *to;
  unsigned long s, d;

  s = (size + 3) >> 2;
  from = (unsigned long *)src;
  to = (unsigned long *)dst;

  while(s--) {
    d = *from++;
    d ^= 0x80808080;
    *to++ = d;
  }
}

void DecodePCM16Mono(void *src, void *dst, unsigned long size)
{
  unsigned long *from, *to;
  unsigned long s, d;

  s = (size + 3) >> 2;
  from = (unsigned long *)src;
  to = (unsigned long *)dst;

  while(s--) {
    d = *from++;
    /* PC style, ABCD -> BADC */
    d = ((d & 0x00ff00ff) << 8) | ((d & 0xff00ff00) >> 8);
    *to++ = d;
  }
}

void DecodePCM16Stereo(void *src, void *dst, unsigned long size)
{
  unsigned long *from, *to;
  unsigned long s, d;

  s = (size + 3) >> 2;
  from = (unsigned long *)src;
  to = (unsigned long *)dst;

  while(s--) {
    d = *from++;
    /* PC style, ABCD -> BADC */
    d = ((d & 0x00ff00ff) << 8) | ((d & 0xff00ff00) >> 8);
    *to++ = d;
  }
}

static int pcm_error = 0;

static double current_time = 0.0;
static int bytes_per_second = 0;
static int bufsize = 0;

int decode_pcm(unsigned char *buf, unsigned long length, double sync_time)
{
  if (pcm_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    /* Make sure the buffer is big enough */
    if (length > bufsize) {
      bufsize = length;
      bufsize = (bufsize + 3) & ~3;
      free(audio);
      audio = malloc(bufsize);
      if (audio == NULL) {
        pcm_error = 1;
        return PLUGIN_FAIL;
      }
    }

    DecodePCM(buf, audio, length);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    amp->audio_refresh(audio, length, current_time);

    current_time += (double)length / (double)bytes_per_second;
  }

  return PLUGIN_OK;
}

int init_pcm(unsigned long type, unsigned long fourcc)
{
  int bits, channels;

  bits = amp->bits;
  channels = amp->channels;
  bufsize = amp->rate; /* 1 second buffer */
  bufsize = (bufsize + 3) & ~3;

  if ((bits == 16) && (channels == 1)) {
    bufsize *= 2;
    DecodePCM = DecodePCM16Mono;
  } else if ((bits == 8) && (channels == 2)) {
    bufsize *= 2;
    DecodePCM = DecodePCM8Stereo;
  } else if ((bits == 16) && (channels == 2)) {
    bufsize *= 4;
    DecodePCM = DecodePCM16Stereo;
  } else {
    bufsize *= 1;
    DecodePCM = DecodePCM8Mono;
  }

  pcm_error = 0;

  audio = malloc(bufsize);
  if (audio == NULL) {
    pcm_error = 1;
    return PLUGIN_FAIL;
  }

  current_time = 0.0;
  bytes_per_second = bufsize;

  if (amp->audio_init(amp->rate, amp->bits, amp->channels) != PLUGIN_OK) {
    pcm_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_pcm(void)
{
  if (audio != NULL) {
    free(audio);
    audio = NULL;
  }

  return PLUGIN_OK;
}

VERSION("PCM v1.01 (020418)");

IDENT_INIT
IDENT_ADD((TYPE_AUDIO | SUBTYPE_NONE), FOURCC_PCM, init_pcm, decode_pcm, exit_pcm)
IDENT_ADD((TYPE_AUDIO | SUBTYPE_NONE), FOURCC_raw, init_pcm, decode_pcm, exit_pcm)
IDENT_EXIT

