/*
 *
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <plugin.h>

static void (*DecodeTWOS)(void *src, void *dst, unsigned long size);
static unsigned char *audio = NULL;

void DecodeTWOS8Mono(void *src, void *dst, unsigned long size)
{
  unsigned char *from, *to;
  unsigned long s, d;

  s = size;
  from = (unsigned char *)src;
  to = (unsigned char *)dst;

  while(s--) {
    d = *from++;
    if (d & 0x80) d -= 0x100;
    *to++ = d;
  }
}

void DecodeTWOS8Stereo(void *src, void *dst, unsigned long size)
{
  unsigned char *from, *to;
  unsigned long s, d;

  s = size / 2;
  from = (unsigned char *)src;
  to = (unsigned char *)dst;

  while(s--) {
    d = from[0];
    if (d & 0x80) d -= 0x100;
    *to++ = d;

    d = from[1];
    if (d & 0x80) d -= 0x100;
    *to++ = d;
    from += 2;
  }
}

void DecodeTWOS16Mono(void *src, void *dst, unsigned long size)
{
  unsigned short *from, *to;
  unsigned long s, d;

  s = size / 2;
  from = (unsigned short *)src;
  to = (unsigned short *)dst;

  while(s--) {
    d = *from++;
    if (d & 0x8000) d -= 0x10000;
    *to++ = d;
  }
}

void DecodeTWOS16Stereo(void *src, void *dst, unsigned long size)
{
  unsigned short *from, *to;
  unsigned long s, d;

  s = size / 4;
  from = (unsigned short *)src;
  to = (unsigned short *)dst;

  while(s--) {
    d = from[0];
    if (d & 0x8000) d -= 0x10000;
    *to++ = d;

    d = from[1];
    if (d & 0x8000) d -= 0x10000;
    *to++ = d;
    from += 2;
  }
}

static int twos_error = 0;

static double current_time = 0.0;
static int bytes_per_second = 0;
static int bufsize = 0;

int decode_twos(unsigned char *buf, unsigned long length, double sync_time)
{
  if (twos_error != 0) {
    return PLUGIN_FAIL;
  }

  if (length > 0) {
    /* Make sure the buffer is big enough */
    if (length > bufsize) {
      bufsize = length;
      free(audio);
      audio = malloc(bufsize);
      if (audio == NULL) {
        twos_error = 1;
        return PLUGIN_FAIL;
      }
    }

    DecodeTWOS(buf, audio, length);

    if (sync_time > 0.0) {
      current_time = sync_time;
    }

    amp->audio_refresh(audio, length, current_time);

    current_time += (double)length / (double)bytes_per_second;
  }

  return PLUGIN_OK;
}

int init_twos(unsigned long type, unsigned long fourcc)
{
  int bits, channels;

  bits = amp->bits;
  channels = amp->channels;
  bufsize = amp->rate; /* 1 second buffer */

  if ((bits == 16) && (channels == 1)) {
    bufsize *= 2;
    DecodeTWOS = DecodeTWOS16Mono;
  } else if ((bits == 8) && (channels == 2)) {
    bufsize *= 2;
    DecodeTWOS = DecodeTWOS8Stereo;
  } else if ((bits == 16) && (channels == 2)) {
    bufsize *= 4;
    DecodeTWOS = DecodeTWOS16Stereo;
  } else {
    bufsize *= 1;
    DecodeTWOS = DecodeTWOS8Mono;
  }

  twos_error = 0;

  audio = malloc(bufsize);
  if (audio == NULL) {
    twos_error = 1;
    return PLUGIN_FAIL;
  }

  current_time = 0.0;
  bytes_per_second = bufsize;

  if (amp->audio_init(amp->rate, amp->bits, amp->channels) != PLUGIN_OK) {
    twos_error = 1;
    return PLUGIN_FAIL;
  }

  return PLUGIN_OK;
}

int exit_twos(void)
{
  if (audio != NULL) {
    free(audio);
    audio = NULL;
  }

  return PLUGIN_OK;
}

VERSION("TWOS v1.00 (020216)");

IDENT_INIT
IDENT_ADD((TYPE_AUDIO | SUBTYPE_NONE), FOURCC_twos, init_twos, decode_twos, exit_twos)
IDENT_EXIT

