/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * paula_int.c
 *
 */

#include <proto/exec.h>
#include <hardware/custom.h>

#include "paula_int.h"

#if defined(__AROS__)
void paula_interrupt(struct Custom *custom, audio_68k_t *audio, void *SysBase)
#else
void paula_interrupt(register struct Custom *custom asm("a0"), register audio_68k_t *audio asm ("a1"), register void *SysBase asm ("a6"))
#endif
{
  audio_buffer_t *buffer;
  int cur_68k, new_68k;

  cur_68k = audio->cur_68k;
  new_68k = cur_68k + 1;
  if (new_68k >= audio->max_68k) {
    new_68k = 0;
  }

  audio->pre_buffer->used = 0; /* pre buffer no longer used */
  audio->pre_buffer = &audio->audio_buffers[audio->playing_68k];

  buffer = &audio->audio_buffers[new_68k];

  if (audio->play & buffer->used) {
    audio->cur_68k = new_68k; /* save until next time */
  } else {
    new_68k = audio->max_68k;
    buffer = &audio->audio_buffers[new_68k];
  }

  audio->playing_68k = new_68k;

  // setup registers

  *audio->left_base_0 = buffer->left_0;
  *audio->right_base_0 = buffer->right_0;

  if (audio->bits14) {
    *audio->left_base_1 = buffer->left_1;
    *audio->right_base_1 = buffer->right_1;
  }

  // signal task

  Signal(audio->sig_task, audio->sig_mask);

  // ack interrupt

  custom->intreq = audio->int_mask;
}
