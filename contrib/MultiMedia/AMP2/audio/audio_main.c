/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * main.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <amiaudio.h>

#include "audio_main.h"
#include "ahi.h"
#include "paula.h"

#define PREFS_PAULA (0)
#define PREFS_AHI   (1)

#define PREFS_OFF (0)
#define PREFS_ON  (1)

#define PREFS_PAULA_8BIT  (0)
#define PREFS_PAULA_14BIT (1)

typedef struct prefs_s {
  u32 audio, stereo, paula_mode, paula_56khz;
  u8 calibration[1024];
  u32 samples_to_bytes;
} prefs_t;

struct amiaudio_s {
  prefs_t prefs;
  audio_t *audio;
};

u32 audio_init(amiaudio_t *audio)
{
  u32 mode = 0;

  if (audio->prefs.stereo == PREFS_ON) {
    mode |= MODE_STEREO;
  }

  if ((audio->prefs.audio == PREFS_AHI) || (audio->prefs.paula_mode == PREFS_PAULA_14BIT)) {
    mode |= MODE_16;
  }

  return mode;
}

static const u8 *modes_txt[4] = { "M8S", "M16S", "S8S", "S16S" };
static const u8 *audio_txt[2] = { "PAULA", "AHI" };

s32 audio_open(amiaudio_t *audio, u32 frequency, u32 mode, u32 fragsize, u32 frags, void (*audio_sync)(u32 time))
{
  u32 copy;

  printf("%s audio mode %s @ %ldHz\n", audio_txt[audio->prefs.audio], modes_txt[mode], frequency);

  /* fragsize */
  if ((fragsize < 64) || (fragsize > 65536)) {
    return -1;
  }

  /* frags */
  if ((frags < 3) || (frags > 256)) {
    return -1;
  }

  /* samples to bytes */
  copy = 1;
  if (mode & MODE_16) {
    copy <<= 1;
  }
  if (mode & MODE_STEREO) {
    copy <<= 1;
  }
  audio->prefs.samples_to_bytes = (copy >> 1);

  /* multiple of 4 */
  fragsize = (fragsize + 3) & ~3;

  if (audio->prefs.audio == PREFS_AHI) {
    audio->audio = ahi_open(frequency, mode, fragsize, frags, audio->prefs.samples_to_bytes, audio_sync);
  } else {
    audio->audio = paula_open(frequency, mode, fragsize, frags, audio->prefs.samples_to_bytes, audio_sync, audio->prefs.paula_56khz, audio->prefs.paula_mode, audio->prefs.calibration);
  }

  if (audio->audio == NULL) {
    return -1;
  }

  return 0;
}

void audio_play(amiaudio_t *audio, void *data, u32 size, u32 time)
{
  if (audio->prefs.audio == PREFS_AHI) {
    ahi_play(audio->audio, data, size, time);
  } else {
    paula_play(audio->audio, data, size, time);
  }
}

u32 audio_in_buffer(amiaudio_t *audio)
{
  if (audio->prefs.audio == PREFS_AHI) {
    return ahi_in_buffer(audio->audio);
  } else {
    return paula_in_buffer(audio->audio);
  }
}

void audio_pause(amiaudio_t *audio)
{
  if (audio->prefs.audio == PREFS_AHI) {
    ahi_pause(audio->audio);
  } else {
    paula_pause(audio->audio);
  }
}

void audio_close(amiaudio_t *audio)
{
  if (audio->prefs.audio == PREFS_AHI) {
    ahi_close(audio->audio);
  } else {
    paula_close(audio->audio);
  }
}

void audio_play_bytes(amiaudio_t *audio, void *data, u32 size, u32 time)
{
  audio_play(audio, data, size >> audio->prefs.samples_to_bytes, time);
}

static s32 prefs_set(prefs_t *prefs, u32 tag, u32 value)
{
  switch (tag) {
    case AUDIO_DONE:
      break;

    case AUDIO_STEREO:
      prefs->stereo = (value == TRUE) ? PREFS_ON : PREFS_OFF;
      break;

    case AUDIO_AHI:
      if (value == TRUE) prefs->audio = PREFS_AHI;
      break;

    case AUDIO_PAULA:
      if (value == TRUE) prefs->audio = PREFS_PAULA;
      break;

    case AUDIO_PAULA_56KHZ:
      prefs->paula_56khz = (value == TRUE) ? PREFS_ON : PREFS_OFF;
      break;

    case AUDIO_PAULA_14BIT:
      prefs->paula_mode = (value == TRUE) ? PREFS_PAULA_14BIT : PREFS_PAULA_8BIT;
      break;

    case AUDIO_PAULA_CALIB:
      strcpy(prefs->calibration, (u8 *)value);
      break;

    default:
      return -1; /* unknown tag */
      break;
  }

  return 0;
}

static s32 prefs_set_tags(prefs_t *prefs, u32 *tags)
{
  if (tags == NULL) {
    return 0; /* use defaults */
  }

  while (tags[0] != AUDIO_DONE) {
    if (prefs_set(prefs, tags[0], tags[1]) < 0) {
      return -1;
    }
    tags += 2;
  }

  return 0;
}

amiaudio_t *audio_new(u32 *tags)
{
  amiaudio_t *audio;

  audio = malloc(sizeof(amiaudio_t));
  if (audio == NULL) {
    return NULL;
  }

  memset(audio, 0, sizeof(amiaudio_t));
  prefs_set_tags(&audio->prefs, tags);

  return audio;
}

s32 audio_set(amiaudio_t *audio, u32 tag, u32 value)
{
  return prefs_set(&audio->prefs, tag, value);
}

void audio_dispose(amiaudio_t *audio)
{
  if (audio != NULL) {
    free(audio);
  }
}

s32 audio_setup(void)
{
  return 0;
}

void audio_cleanup(void)
{
}
