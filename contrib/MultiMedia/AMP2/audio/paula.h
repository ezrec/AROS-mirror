/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * paula.h
 *
 */

#ifndef PAULA_H
#define PAULA_H

#include "audio_main.h"

extern audio_t *paula_open(u32 frequency, u32 mode, u32 fragsize, u32 frags, u32 samples_to_bytes, void (*audio_sync)(u32 time), u32 khz56, u32 bits14, u8 *calibration);
extern void paula_play(audio_t *audio, void *data, u32 samples, u32 time);
extern u32 paula_in_buffer(audio_t *audio);
extern void paula_pause(audio_t *audio);
extern void paula_close(audio_t *audio);

#endif
