/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * ahi.h
 *
 */

#ifndef AHI_H
#define AHI_H

#include "audio_main.h"

extern audio_t *ahi_open(u32 frequency, u32 mode, u32 fragsize, u32 frags, u32 samples_to_bytes, void (*audio_sync)(u32 time));
extern void ahi_play(audio_t *audio, void *data, u32 samples, u32 time);
extern u32 ahi_in_buffer(audio_t *audio);
extern void ahi_pause(audio_t *audio);
extern void ahi_close(audio_t *audio);

#endif
