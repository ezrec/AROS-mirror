/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * paula_int.h
 *
 */

#ifndef PAULA_INT_H
#define PAULA_INT_H

#include <amitypes.h>

typedef struct audio_buffer_s {
  void *left_0;
  void *left_1;
  void *right_0;
  void *right_1;

  u32 number;
  u32 size;
  u32 time;
  u32 used;
} audio_buffer_t;

typedef struct audio_68k_s {
  audio_buffer_t *audio_buffers;
  audio_buffer_t *pre_buffer;

  UWORD **left_base_0;
  UWORD **left_base_1;
  UWORD **right_base_0;
  UWORD **right_base_1;

  u32 cur_68k;
  u32 max_68k;
  u32 playing_68k;
  u32 bits14;

  u32 sig_num;
  u32 sig_mask;
  void *sig_task;
  u32 int_mask;

  u32 play;
} audio_68k_t;

extern void paula_interrupt();

#endif
