/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * paula.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <dos/dostags.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/audio.h>
#if !defined(__AROS__)
#include <hardware/dmabits.h>
#include <hardware/adkbits.h>
#endif
#include <hardware/custom.h>
#include <hardware/intbits.h>
#include <graphics/gfxbase.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <amiaudio.h>

#if defined(__AROS__)
#include <string.h>
#endif

#include "paula.h"
#include "paula_int.h"

#if 1

#define PRINTF(a...)
#define DEBUG(a...)

#else

#define PRINTF(a...)
//#define PRINTF(a...) printf(a)

//#define DEBUG(a...)
#define DEBUG(a...) printf(a)

#endif

struct audio_s {
  audio_68k_t *audio_68k;

  u32 frequency;
  u32 mode;
  u32 fragsize;
  u32 frags;
  u32 current_time;
  u32 write_buffer;
  void (*audio_sync)(u32 time);
  u32 real_frequency;

  /* conversion */
  u32 l_buf;
  u32 l_buf2;
  u32 r_buf;
  u32 r_buf2;
  u32 pos_buf;
  u32 buffer;
  u32 samples_to_bytes;
  u32 in_buffer;

  /* downsample */
  u32 downsample;
  u32 down;
  u32 down_mask;

  /* tasks 'n' stuff */
  u8 task_name[32];
#ifdef __PPC__
  struct TaskPPC *main_task;
  struct TaskPPC *audio_task;
#else
  struct Task *main_task;
  struct Task *audio_task;
  struct Process *audio_process;
#endif

  /* 14bit conversion */
  u8 *additive_array;
  u16 *conversion_table;
};

#define MALLOC(x) AllocVec((x), MEMF_CHIP|MEMF_PUBLIC|MEMF_CLEAR);
#define FREE(x) if (x != NULL) { FreeVec(x); x = NULL; }

/* hack */
#ifndef __PPC__
#define SetSignalPPC(a...) SetSignal(a)
#define SignalPPC(a...) Signal(a)
#define WaitPPC(a...) Wait(a)
#define FindTaskPPC(a...) FindTask(a)
#endif

static void build_table(audio_t *audio, u8 *calib)
{
  BPTR fp = NULL;
  int len = 0, i;

  /* create 14bit conversion tables */
  audio->additive_array = AllocVec(256, MEMF_PUBLIC|MEMF_CLEAR);
  audio->conversion_table = AllocVec(65536*2, MEMF_PUBLIC|MEMF_CLEAR);

  if ((calib != NULL) && (calib[0] != '\0')) {
    fp = Open(calib, MODE_OLDFILE);
    if (fp != NULL) {
      len = FRead(fp, audio->additive_array, 1, 256);
      Close(fp);
    }

    if (len != 256) {
      DEBUG("Failed to load 14BIT calibration file (%s)\n", calib);
    }
  }

  if (len != 256) {
    for (i=0; i<255; i++) {
      audio->additive_array[i] = 0x55;
    }
    audio->additive_array[255] = 0x7f;
  }

  DEBUG("14BIT\n");

  /* FIXME: This is a direct port from 68k asm and is not very nicely written */
  {
    unsigned char *a0;
    signed char *a1, *a2;
    int sum_pos, sum_neg, hi, lo, counter, val, stretch;

    a0 = (unsigned char *)audio->conversion_table;
    a1 = (signed char *)audio->additive_array;

    sum_pos = sum_neg = 0;
    for (i=0; i<128; i++) {
      sum_pos += a1[128 + i];
      sum_neg += a1[i];
    }

    a2 = a1 + 128;

    // Part 1

    stretch = 32768;
    a1 = a2;
    i = 32768-1;

    hi = 0;
    lo = 0;
    counter = 0;

    val = *a1++;
    counter += val;
    do {
      if (counter > 0) {
        *a0++ = hi;
        *a0++ = lo;

        stretch -= sum_pos;
        if (stretch < 0) {
          stretch += 32768;
          lo += 1;
          counter -= 1;
        }
        i--;
      } else {
        hi += 1;
        lo -= val;

        val = *a1++;
        counter += val;
      }
    } while (i > -1);

    // Part 2

    stretch = 32768;
    a0 += 2*32768;
    a1 = a2;
    i = 32768-1;

    hi = -1;
    lo = -1;
    counter = 0;

    val = *--a1;
    counter += val;
    lo += val;
    do {
      if (counter > 0) {
        *--a0 = lo;
        *--a0 = hi;

        stretch -= sum_neg;
        if (stretch < 0) {
          stretch += 32768;
          lo -= 1;
          counter -= 1;
        }
        i--;
      } else {
        hi -= 1;

        val = *--a1;
        counter += val;
        lo += val;
      }
    } while (i > -1);
  }
}

/* to task:
 * SIGBREAKF_CTRL_C -
 * SIGBREAKF_CTRL_D wake up
 * SIGBREAKF_CTRL_E exit
 * SIGBREAKF_CTRL_F -
 *
 * from task:
 * SIGBREAKF_CTRL_C -
 * SIGBREAKF_CTRL_D wake up
 * SIGBREAKF_CTRL_E error
 * SIGBREAKF_CTRL_F -
 */

static void make_task_name(audio_t *audio)
{
  int task_number = 0;

  for (;;) {
    sprintf(audio->task_name, "AudioServerTask%04d", task_number);

    if (FindTaskPPC(audio->task_name) == NULL) {
      break;
    } else {
      task_number++;
    }
  }
}

#if defined(__PPC__)
static void paula_task(register audio_t *audio asm ("3"))
#elif defined(__AROS__)
static void paula_task( u8 *str, s32 length)
#else
static void paula_task(register u8 *str asm ("a0"), register s32 length asm ("d0"))
#endif
{
  volatile unsigned char *filter = (unsigned char *)0xbfe001;
  struct Custom *custom = (struct Custom *)0xdff000;
  int dma_channels, channels, period, channelnr = 0;
  unsigned long audio_clock, audio_device = 1;
  struct Interrupt AudioInt, *OldInt = NULL;
  struct IOAudio *audioio = NULL;
  struct MsgPort *audioport = NULL;
  UBYTE channelalloc[2] = { 0, 0 };
  struct Library *GfxBase = NULL;
  short OldINTENA = 0;

#ifndef __PPC__
  audio_t *audio = NULL;
  s32 addr, mult;

  mult = 1;
  addr = 0;
  while (length > 0) {
    addr += (str[length - 1] - '0') * mult;
    mult *= 10;
    length--;
  }

  audio = (audio_t *)addr;
#endif

  audio->audio_68k->sig_num = AllocSignal(-1);

  audio->audio_68k->sig_mask = 1L << audio->audio_68k->sig_num;
  audio->audio_68k->sig_task = FindTaskPPC(NULL);

  audio_clock = 3546895; /* PAL */

  GfxBase = OpenLibrary("graphics.library", 39);
  if (GfxBase != NULL) {
    if (((struct GfxBase *)GfxBase)->DisplayFlags & REALLY_PAL) {
      audio_clock = 3546895; /* PAL */
    } else {
      audio_clock = 3579545; /* NTSC */
    }
  }

  if (audio->mode & MODE_STEREO) {
    channels = 2;
  } else {
    channels = 1;
  }

  period = audio_clock / audio->frequency;
  audio->real_frequency = audio_clock / period; /* actual frequency */

  if (audio->audio_68k->bits14) {
    channelalloc[0] = ( 1 + 2 + 4 + 8 );
    channelalloc[1] = ( 1 + 2 + 4 + 8 );
  } else {
    channelalloc[0] = ( 1 + 2 );
    channelalloc[1] = ( 4 + 8 );
  }

  /* open audio->device */
  if ((audioport = CreateMsgPort())) {
    if ((audioio = CreateIORequest(audioport, sizeof(struct IOAudio)))) {
      audioio->ioa_Request.io_Message.mn_Node.ln_Pri = ADALLOC_MAXPREC;
      audioio->ioa_Request.io_Command                = ADCMD_ALLOCATE;
      audioio->ioa_Request.io_Flags                  = ADIOF_NOWAIT;
      audioio->ioa_AllocKey                          = 0;
      audioio->ioa_Data                              = channelalloc;
      audioio->ioa_Length                            = sizeof(channelalloc);
      audio_device = OpenDevice(AUDIONAME, 0, &audioio->ioa_Request, 0);
    }
  }

  if (audio_device == 0) {
    audio_buffer_t *buffer;

    buffer = &audio->audio_68k->audio_buffers[audio->audio_68k->max_68k];

    dma_channels = ((ULONG)audioio->ioa_Request.io_Unit) & 0x0f;

    AudioInt.is_Node.ln_Type = NT_INTERRUPT;
    AudioInt.is_Node.ln_Pri = 0;
    AudioInt.is_Data = audio->audio_68k;
    AudioInt.is_Code = (void(*)())paula_interrupt;

    switch (dma_channels & 9) {
      case 1: channelnr = 0; break;
      case 2: channelnr = 1; break;
      case 4: channelnr = 2; break;
      case 8: channelnr = 3; break;
    }

    /* 0 : LEFT   (MSB)
     * 1 : RIGHT  (MSB)
     * 2 : RIGHT  (LSB)
     * 3 : LEFT   (LSB)
     */

    if (audio->audio_68k->bits14) {
      /* disable audio and any modulation for our channels */
#if !defined(__AROS__)
      custom->dmacon = DMAF_AUDIO;
      custom->adkcon = ADKF_USE3PN|ADKF_USE2P3|ADKF_USE1P2|ADKF_USE0P1|
                       ADKF_USE3VN|ADKF_USE2V3|ADKF_USE1V2|ADKF_USE0V1;
#endif

      if (channels == 2) {
        custom->aud[0].ac_ptr = (UWORD *)buffer->left_0;
        custom->aud[1].ac_ptr = (UWORD *)buffer->right_0;
        custom->aud[2].ac_ptr = (UWORD *)buffer->right_1;
        custom->aud[3].ac_ptr = (UWORD *)buffer->left_1;

        audio->audio_68k->left_base_0 = &custom->aud[0].ac_ptr;
        audio->audio_68k->left_base_1 = &custom->aud[3].ac_ptr;
        audio->audio_68k->right_base_0 = &custom->aud[1].ac_ptr;
        audio->audio_68k->right_base_1 = &custom->aud[2].ac_ptr;
      } else {
        custom->aud[0].ac_ptr = (UWORD *)buffer->left_0;
        custom->aud[1].ac_ptr = (UWORD *)buffer->left_0;
        custom->aud[2].ac_ptr = (UWORD *)buffer->left_1;
        custom->aud[3].ac_ptr = (UWORD *)buffer->left_1;

        audio->audio_68k->left_base_0 = &custom->aud[0].ac_ptr;
        audio->audio_68k->left_base_1 = &custom->aud[3].ac_ptr;
        audio->audio_68k->right_base_0 = &custom->aud[1].ac_ptr;
        audio->audio_68k->right_base_1 = &custom->aud[2].ac_ptr;
      }

      custom->aud[0].ac_vol =
      custom->aud[1].ac_vol = 64;
      custom->aud[2].ac_vol =
      custom->aud[3].ac_vol = 1;

      custom->aud[0].ac_len =
      custom->aud[1].ac_len =
      custom->aud[2].ac_len =
      custom->aud[3].ac_len = audio->fragsize / 2; /* wordlength */

      custom->aud[0].ac_per =
      custom->aud[1].ac_per =
      custom->aud[2].ac_per =
      custom->aud[3].ac_per = period;

      audio->audio_68k->int_mask = INTF_AUD0|INTF_AUD1|INTF_AUD2|INTF_AUD3;
    } else {
      int l, r;

      if ((dma_channels & 9) == 8) { /* 1 or 8 */
        l = 3;
      } else {
        l = 0;
      }

      if ((dma_channels & 6) == 4) { /* 2 or 4 */
        r = 2;
      } else {
        r = 1;
      }

      if (channels == 2) {
        custom->aud[l].ac_ptr = (UWORD *)buffer->left_0;
        custom->aud[r].ac_ptr = (UWORD *)buffer->right_0;
      } else {
        custom->aud[l].ac_ptr = (UWORD *)buffer->left_0;
        custom->aud[r].ac_ptr = (UWORD *)buffer->left_0;
      }

      audio->audio_68k->left_base_0 = &custom->aud[l].ac_ptr;
      audio->audio_68k->right_base_0 = &custom->aud[r].ac_ptr;

      custom->aud[l].ac_vol =
      custom->aud[r].ac_vol = 64;

      custom->aud[l].ac_len =
      custom->aud[r].ac_len = audio->fragsize / 2; /* wordlength */

      custom->aud[l].ac_per =
      custom->aud[r].ac_per = period;

      if (channelnr == 0) {
        audio->audio_68k->int_mask = INTF_AUD0|INTF_AUD1;
      } else {
        audio->audio_68k->int_mask = INTF_AUD2|INTF_AUD3;
      }
    }

    OldInt = SetIntVector(channelnr+7,&AudioInt);
    OldINTENA = *(short *)0xdff01c;
    *(short *)0xdff09a = OldINTENA | (0xc000 | (0x0080 << channelnr));

    /* start playing using the dma channels */
#if !defined(__AROS__)
    custom->dmacon = DMAF_SETCLR|dma_channels;
#endif

    /* Turn off audio filter */
    *filter |= 0x02;

DEBUG("PAULA open\n");

    SignalPPC(audio->main_task, SIGBREAKF_CTRL_D);

    for (;;) {
      u32 signals;
      /* get signal from 68k interrupt */
      signals = WaitPPC(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | audio->audio_68k->sig_mask);

      if (signals & audio->audio_68k->sig_mask) {
        audio_buffer_t *buffer = audio->audio_68k->pre_buffer;
        s32 in_buffer = 0;

        if (buffer->number != audio->frags) {
          in_buffer = (audio->write_buffer - buffer->number) - 1;
          if (in_buffer < 0) {
            in_buffer += audio->frags;
          }
        }
        audio->in_buffer = (in_buffer * audio->fragsize);

        /* sync */
        if (buffer->number != audio->frags) {
          if (audio->audio_sync != NULL) {
            audio->audio_sync(buffer->time);
          }
        }

        SignalPPC(audio->main_task, SIGBREAKF_CTRL_D);
      }

      if (signals & SIGBREAKF_CTRL_E) {
        break;
      }
    }
  } else {
DEBUG("failed to open PAULA\n");
  }

  if (OldINTENA != 0) {
    *(short *)0xdff09a = (OldINTENA | 0x8000);
  }
  if (OldInt != NULL) {
    SetIntVector(channelnr+7,OldInt);
  }
  if (audio_device == 0) {
    CloseDevice(&audioio->ioa_Request);
  }
  if (audioio != NULL) {
    DeleteIORequest(&audioio->ioa_Request);
  }
  if (audioport != NULL) {
    DeleteMsgPort(audioport);
  }

  if (GfxBase != NULL) {
    CloseLibrary(GfxBase);
  }

DEBUG("PAULA closed\n");

  FreeSignal(audio->audio_68k->sig_num);

  /* send error signal in case we failed */
  SignalPPC(audio->main_task, SIGBREAKF_CTRL_E);
}

audio_t *paula_open(u32 frequency, u32 mode, u32 fragsize, u32 frags, u32 samples_to_bytes, void (*audio_sync)(u32 time), u32 khz56, u32 bits14, u8 *calibration)
{
  u32 i, signals, down, max;
  audio_t *audio;

  /* alloc structure */
  audio = malloc(sizeof(audio_t));
  if (audio == NULL) {
    return NULL;
  }
  memset(audio, 0, sizeof(audio_t));

  /* downsample */
  max = (khz56) ? 56000 : 28000;
  down = (frequency + max - 1) / max;

  /* multiple of 2 */
  i = 0;
  while ((1 << i) < down) {
    i++;
  }
  down = (1 << i);

  /* store downsample */
  audio->downsample = i;
  audio->down = 0;
  audio->down_mask = down - 1;

  /* adjustments */
  frequency >>= audio->downsample;
  fragsize >>= audio->downsample;

  /* allocate 68k structure */
  audio->audio_68k = MALLOC(sizeof(audio_68k_t));
  if (audio->audio_68k == NULL) {
    goto fail;
  }

  /* store settings */
  audio->frequency = frequency;
  audio->mode = mode;
  audio->fragsize = fragsize;
  audio->frags = frags;
  audio->samples_to_bytes = samples_to_bytes;
  audio->audio_sync = audio_sync;

  if (mode & MODE_16) {
    audio->audio_68k->bits14 = bits14;
  } else {
    audio->audio_68k->bits14 = 0;
  }

  if (audio->audio_68k->bits14) {
    build_table(audio, calibration);
  }

DEBUG("alloc buffers\n");

  frags++; /* add room for silent frag */

  /* allocate buffers */
  audio->audio_68k->audio_buffers = MALLOC(frags * sizeof(audio_buffer_t));
  if (audio->audio_68k->audio_buffers == NULL) {
    goto fail;
  }
  for (i=0; i<frags; i++) {
    audio->audio_68k->audio_buffers[i].number = i;

    switch (audio->mode) {
      case AUDIO_M8S:
        audio->audio_68k->audio_buffers[i].left_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].left_0 == NULL) {
          goto fail;
        }
        audio->audio_68k->audio_buffers[i].right_0 = audio->audio_68k->audio_buffers[i].left_0;
        break;

      case AUDIO_M16S:
        audio->audio_68k->audio_buffers[i].left_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].left_0 == NULL) {
          goto fail;
        }
        audio->audio_68k->audio_buffers[i].right_0 = audio->audio_68k->audio_buffers[i].left_0;
        if (audio->audio_68k->bits14) {
          audio->audio_68k->audio_buffers[i].left_1 = MALLOC(fragsize);
          if (audio->audio_68k->audio_buffers[i].left_1 == NULL) {
            goto fail;
          }
          audio->audio_68k->audio_buffers[i].right_1 = audio->audio_68k->audio_buffers[i].left_1;
        }
        break;

      case AUDIO_S8S:
        audio->audio_68k->audio_buffers[i].left_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].left_0 == NULL) {
          goto fail;
        }
        audio->audio_68k->audio_buffers[i].right_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].right_0 == NULL) {
          goto fail;
        }
        break;

      case AUDIO_S16S:
        audio->audio_68k->audio_buffers[i].left_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].left_0 == NULL) {
          goto fail;
        }
        audio->audio_68k->audio_buffers[i].right_0 = MALLOC(fragsize);
        if (audio->audio_68k->audio_buffers[i].right_0 == NULL) {
          goto fail;
        }
        if (audio->audio_68k->bits14) {
          audio->audio_68k->audio_buffers[i].left_1 = MALLOC(fragsize);
          if (audio->audio_68k->audio_buffers[i].left_1 == NULL) {
            goto fail;
          }
          audio->audio_68k->audio_buffers[i].right_1 = MALLOC(fragsize);
          if (audio->audio_68k->audio_buffers[i].right_1 == NULL) {
            goto fail;
          }
        }
        break;
    }

    audio->audio_68k->audio_buffers[i].size = 0;
  }

  /* init */
  audio->audio_68k->max_68k = audio->frags;
  audio->audio_68k->pre_buffer = &audio->audio_68k->audio_buffers[audio->audio_68k->max_68k];
  audio->audio_68k->cur_68k = audio->audio_68k->max_68k;

  audio->audio_68k->playing_68k = audio->audio_68k->cur_68k;

  /* get task pointer */
  audio->main_task = FindTaskPPC(NULL);

  /* remove signals */
  SetSignalPPC(0, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);

DEBUG("create task\n");

  if (1) {
#ifdef __PPC__
    struct TagItem ti[]={{TASKATTR_CODE, (ULONG)paula_task},
                         {TASKATTR_NAME, (ULONG)audio->task_name},
                         {TASKATTR_INHERITR2, TRUE},
                         {TASKATTR_STACKSIZE, 4 * 65536}, /* 64KB should be enough */
                         {TASKATTR_PRI, 10},
                         {TASKATTR_R3, (ULONG)audio},
                         {TAG_DONE, 0}};
#else
    u8 addr[16];
    struct TagItem ti[]={{NP_Entry, (ULONG)paula_task},
                         {NP_Name, (ULONG)audio->task_name},
                         {NP_StackSize, 4 * 65536}, /* 64KB should be enough */
                         {NP_Priority, 10},
                         {NP_Arguments, (ULONG)addr},
                         {TAG_DONE, 0}};
    sprintf(addr, "%d", (int)audio);
#endif

    Forbid();
    make_task_name(audio);
#ifdef __PPC__
    audio->audio_task = CreateTaskPPC(ti);
#else
    audio->audio_process = CreateNewProc(ti);
    audio->audio_task = (struct Task *)audio->audio_process;
#endif
    Permit();
    if (audio->audio_task == NULL) {
      goto fail;
    }
  }

#ifdef __PPC__
  SetNiceValue(audio->audio_task, -20); /* give highest priority */
#endif

DEBUG("success, waiting...\n");

  signals = WaitPPC(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
  if (signals & SIGBREAKF_CTRL_E) {
    DEBUG("failed to start audio task...\n");

    /* wait for task to exit */
    while (FindTaskPPC(audio->task_name) != NULL) {
    }

    audio->audio_task = NULL;
    goto fail;
  }

DEBUG("done! task is running!\n");

  return audio;

fail:

DEBUG("paula open failed\n");

  paula_close(audio); /* free any allocated resources */

  return NULL;
}

static void convert_samples(audio_t *audio, audio_buffer_t *buffer, void *data, int samples)
{
  u8 *cptr = data;
  u16 *sptr = data, val;
  u32 i;

  /* Convert from 16bit to 8bit when required and add to buffer(s) */
  if (audio->mode == AUDIO_M8S) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        audio->l_buf <<= 8;
        audio->l_buf |= cptr[0];
        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      cptr += 1; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  } else if (audio->mode == AUDIO_S8S) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        audio->l_buf <<= 8;
        audio->l_buf |= cptr[0];
        audio->r_buf <<= 8;
        audio->r_buf |= cptr[1];
        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          *((unsigned long *)(buffer->right_0 + audio->buffer)) = audio->r_buf;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      cptr += 2; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  } else if ((audio->mode == AUDIO_M16S) && (audio->audio_68k->bits14)) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        val = sptr[0];
        val = audio->conversion_table[val];

        audio->l_buf <<= 8;
        audio->l_buf |= val >> 8;
        audio->l_buf2 <<= 8;
        audio->l_buf2 |= val & 0xff;

        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          *((unsigned long *)(buffer->left_1 + audio->buffer)) = audio->l_buf2;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      sptr += 1; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  } else if ((audio->mode == AUDIO_S16S) && (audio->audio_68k->bits14)) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        val = sptr[0];
        val = audio->conversion_table[val];

        audio->l_buf <<= 8;
        audio->l_buf |= val >> 8;
        audio->l_buf2 <<= 8;
        audio->l_buf2 |= val & 0xff;

        val = sptr[1];
        val = audio->conversion_table[val];

        audio->r_buf <<= 8;
        audio->r_buf |= val >> 8;
        audio->r_buf2 <<= 8;
        audio->r_buf2 |= val & 0xff;

        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          *((unsigned long *)(buffer->left_1 + audio->buffer)) = audio->l_buf2;
          *((unsigned long *)(buffer->right_0 + audio->buffer)) = audio->r_buf;
          *((unsigned long *)(buffer->right_1 + audio->buffer)) = audio->r_buf2;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      sptr += 2; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  } else if (audio->mode == AUDIO_M16S) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        audio->l_buf <<= 8;
        audio->l_buf |= sptr[0] >> 8;
        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      sptr += 1; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  } else if (audio->mode == AUDIO_S16S) {
    for (i=0; i<samples; i++) {
      if (audio->down == 0) {
        audio->l_buf <<= 8;
        audio->l_buf |= sptr[0] >> 8;
        audio->r_buf <<= 8;
        audio->r_buf |= sptr[1] >> 8;
        audio->pos_buf++;

        if (audio->pos_buf >= 4) {
          *((unsigned long *)(buffer->left_0 + audio->buffer)) = audio->l_buf;
          *((unsigned long *)(buffer->right_0 + audio->buffer)) = audio->r_buf;
          audio->pos_buf = 0;
          audio->buffer += 4;
        }
      }
      sptr += 2; /* next sample */
      audio->down = (audio->down + 1) & audio->down_mask; /* downsample */
    }
  }

  /* reset write position */
  if (audio->buffer >= audio->fragsize) {
    audio->buffer = 0;
  }
}

void paula_play(audio_t *audio, void *data, u32 size, u32 time)
{
  u32 used, copy, max, signals;

  if (audio->audio_task == NULL) {
    return; /* not open */
  }

  if (time != NOTIME) {
    audio->current_time = time;
  }

  time = audio->current_time;

  audio->audio_68k->play = 1; /* start playing */

  while (size > 0) {
PRINTF("audio_play: %ld\n", size);

    for (;;) {
      used = audio->audio_68k->audio_buffers[audio->write_buffer].used;

      if (used) {
PRINTF("used: %ld\n", audio->write_buffer);
        signals = WaitPPC(SIGBREAKF_CTRL_D);
PRINTF("got signals: %08lx\n", signals);
      } else {
        break;
      }
    }

PRINTF("buffer free: %ld\n", audio->write_buffer);

    if (audio->audio_68k->audio_buffers[audio->write_buffer].size == audio->fragsize) {
      audio->audio_68k->audio_buffers[audio->write_buffer].size = 0;
    }

    if (audio->audio_68k->audio_buffers[audio->write_buffer].size == 0) {
      audio->audio_68k->audio_buffers[audio->write_buffer].time = time;
    }

    max = audio->fragsize - audio->audio_68k->audio_buffers[audio->write_buffer].size;
    max <<= audio->downsample;

    copy = size;
    if (copy > max) {
      copy = max;
    }

    convert_samples(audio, &audio->audio_68k->audio_buffers[audio->write_buffer], data, copy);

    audio->audio_68k->audio_buffers[audio->write_buffer].size += (copy >> audio->downsample);
    data += (copy << audio->samples_to_bytes);
    size -= copy;

    time += (copy * TIMEBASE) / audio->frequency;

    if (audio->audio_68k->audio_buffers[audio->write_buffer].size == audio->fragsize) {
      audio->audio_68k->audio_buffers[audio->write_buffer].used = 1;
      audio->write_buffer++;
      if (audio->write_buffer >= audio->frags) {
        audio->write_buffer = 0;
      }
    }
  }

  audio->current_time = time;
}

u32 paula_in_buffer(audio_t *audio)
{
  if (audio->audio_task == NULL) {
    return 0; /* not open */
  }

  return audio->in_buffer << audio->downsample;
}

void paula_pause(audio_t *audio)
{
  if (audio->audio_task == NULL) {
    return; /* not open */
  }

  audio->audio_68k->play = 0; /* stop playing */
}

void paula_close(audio_t *audio)
{
  u32 i, frags;
  u32 signals;

  if (audio == NULL) {
    return;
  }

  if (audio->audio_task != NULL) {
DEBUG("stop server\n");
    /* stop server */
    SignalPPC(audio->audio_task, SIGBREAKF_CTRL_E);

DEBUG("wait for signal\n");
    /* wait for signal */
    do {
      signals = WaitPPC(SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);
    } while (!(signals & SIGBREAKF_CTRL_E));

DEBUG("wait for exit\n");
    /* wait for task to exit */
    while (FindTaskPPC(audio->task_name) != NULL) {
    }

    audio->audio_task = NULL;
  }

  if (audio->audio_68k != NULL) {
DEBUG("free buffers\n");
    frags = audio->frags + 1;
    if (audio->audio_68k->audio_buffers != NULL) {
      for (i=0; i<frags; i++) {
        switch (audio->mode) {
          case AUDIO_M8S:
            FREE(audio->audio_68k->audio_buffers[i].left_0);
            break;

          case AUDIO_M16S:
            FREE(audio->audio_68k->audio_buffers[i].left_0);
            if (audio->audio_68k->bits14) {
              FREE(audio->audio_68k->audio_buffers[i].left_1);
            }
            break;

          case AUDIO_S8S:
            FREE(audio->audio_68k->audio_buffers[i].left_0);
            FREE(audio->audio_68k->audio_buffers[i].right_0);
            break;

          case AUDIO_S16S:
            FREE(audio->audio_68k->audio_buffers[i].left_0);
            FREE(audio->audio_68k->audio_buffers[i].right_0);
            if (audio->audio_68k->bits14) {
              FREE(audio->audio_68k->audio_buffers[i].left_1);
              FREE(audio->audio_68k->audio_buffers[i].right_1);
            }
            break;
        }
      }
      FREE(audio->audio_68k->audio_buffers);
    }
    FREE(audio->audio_68k);
  }

  /* free 14bit conversion tables */
  FREE(audio->additive_array);
  FREE(audio->conversion_table);

  /* free structure */
  free(audio);
}
