/*
 * AMP2 - Copyright (c) 2000 Mathias Roslund
 *
 * ahi.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <dos/dostags.h>
#include <exec/exec.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <devices/ahi.h>
#include <devices/audio.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <amiaudio.h>
#include "ahi.h"

#if 1

#define PRINTF(a...)
#define DEBUG(a...)

#else

#ifdef __PPC__

#define PRINTF(a...)
//#define PRINTF(a...) printf(a)

//#define DEBUG(a...)
#define DEBUG(a...) printf(a)

#else

#define PRINTF(a...)
#define DEBUG(a...)

#endif

#endif

typedef struct audio_buffer_s {
  void *buffer;
  u32 size;
  u32 time;
  u32 used;
} audio_buffer_t;

struct audio_s {
  u32 frequency;
  u32 mode;
  u32 fragsize;
  u32 frags;
  audio_buffer_t *audio_buffers;
  u32 current_time;
  u32 read_buffer;
  u32 previous_read_buffer;
  u32 write_buffer;
  void (*audio_sync)(u32 time);

  u32 play;
  u32 samples_to_bytes;
  u32 in_buffer;

  /* tasks 'n' stuff */
  u8 task_name[32];
#ifdef __PPC__
  struct TaskPPC *main_task;
  struct TaskPPC *audio_task;
  struct SignalSemaphorePPC *semaphore;
#else
  struct Task *main_task;
  struct Task *audio_task;
  struct SignalSemaphore *semaphore;
  struct Process *audio_process;
#endif
};

#define MALLOC(x) ({ u32 size = (x); void *ptr = malloc(size); memset(ptr, 0, size); ptr; })
#define FREE(x) if (x != NULL) { free(x); x = NULL; }

/* hack */
#if defined(__AROS__)
#include "string.h"
#endif

#ifndef __PPC__
#define InitSemaphorePPC(a...) InitSemaphore(a)
#define ObtainSemaphorePPC(a...) ObtainSemaphore(a)
#define ReleaseSemaphorePPC(a...) ReleaseSemaphore(a)
#define FreeSemaphorePPC(a...) /* does not exist on 68k */
#define SetSignalPPC(a...) SetSignal(a)
#define SignalPPC(a...) Signal(a)
#define WaitPPC(a...) Wait(a)
#define FindTaskPPC(a...) FindTask(a)
#endif

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

#define FLAG_EXIT (1 << 0)

#if defined(__PPC__)
static void ahi_task(register audio_t *audio asm ("3"))
#elif defined(__AROS__)
static void ahi_task( u8 *str,  s32 length)
#else
static void ahi_task(register u8 *str asm ("a0"), register s32 length asm ("d0"))
#endif
{
  struct AHIRequest *AHIIO = NULL, *AHIIO1 = NULL, *AHIIO2 = NULL;
  struct MsgPort *AHIMP1 = NULL, *AHIMP2 = NULL;
  u32 device, signals, used, flags;
  struct AHIRequest *link = NULL;

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

DEBUG("task started\n");

  flags = 0;
  signals = 0;
  device = 1;

  if ((AHIMP1 = CreateMsgPort())) {
    if ((AHIMP2 = CreateMsgPort())) {
      if ((AHIIO1 = (struct AHIRequest *)CreateIORequest(AHIMP1, sizeof(struct AHIRequest)))) {
        if ((AHIIO2 = (struct AHIRequest *)CreateIORequest(AHIMP2, sizeof(struct AHIRequest)))) {
          device = OpenDevice(AHINAME, 0, (struct IORequest *)AHIIO1, NULL);
        }
      }
    }
  }

  if (device == 0) {
DEBUG("AHI open\n");
    *AHIIO2 = *AHIIO1;
    AHIIO1->ahir_Std.io_Message.mn_ReplyPort = AHIMP1;
    AHIIO2->ahir_Std.io_Message.mn_ReplyPort = AHIMP2;
    AHIIO1->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
    AHIIO2->ahir_Std.io_Message.mn_Node.ln_Pri = 127;
    AHIIO = AHIIO1;

PRINTF("open done\n");

    SignalPPC(audio->main_task, SIGBREAKF_CTRL_D);

    for (;;) {
      if (flags & FLAG_EXIT) {
        break;
      }

      ObtainSemaphorePPC(audio->semaphore);
      used = audio->audio_buffers[audio->read_buffer].used;
      ReleaseSemaphorePPC(audio->semaphore);

      if (audio->play & used) {

PRINTF("playing buffer: %d (%d)\n", audio->read_buffer, length);

        AHIIO->ahir_Std.io_Command = CMD_WRITE;
        AHIIO->ahir_Std.io_Data    = audio->audio_buffers[audio->read_buffer].buffer;
        AHIIO->ahir_Std.io_Length  = audio->audio_buffers[audio->read_buffer].size;
        AHIIO->ahir_Std.io_Offset  = 0;
        AHIIO->ahir_Frequency      = audio->frequency;
        AHIIO->ahir_Type           = audio->mode;
        AHIIO->ahir_Volume         = 0x10000; // Volume:  0dB
        AHIIO->ahir_Position       = 0x08000; // Panning: center
        AHIIO->ahir_Link           = link;

        SendIO((struct IORequest *)AHIIO);

        if (link != NULL) {
          u32 sigbit = 1L << link->ahir_Std.io_Message.mn_ReplyPort->mp_SigBit;
          s32 in_buffer = 0;

PRINTF("want: %08lx\n", sigbit);

          for (;;) {
            signals = WaitPPC(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D | sigbit);
PRINTF("got %08lx\n", signals);
            if (signals & SIGBREAKF_CTRL_E) {
              flags |= FLAG_EXIT;
            }

            /* break when we got what we want */
            if (signals & sigbit) {
              break;
            }
          }

          WaitIO((struct IORequest *)link);

PRINTF("free buffer: %d\n", audio->previous_read_buffer);

          /* sync */
          if (audio->audio_sync != NULL) {
            audio->audio_sync(audio->audio_buffers[audio->read_buffer].time);
          }

          ObtainSemaphorePPC(audio->semaphore);
          audio->audio_buffers[audio->previous_read_buffer].used = 0;

          /* in buffer */
          in_buffer = (audio->write_buffer - audio->read_buffer) - 1;
          if (in_buffer < 0) {
            in_buffer += audio->frags;
          }
          audio->in_buffer = (in_buffer * audio->fragsize);
          ReleaseSemaphorePPC(audio->semaphore);

          SignalPPC(audio->main_task, SIGBREAKF_CTRL_D);
        }

        link = AHIIO;

        if (AHIIO == AHIIO1) {
          AHIIO = AHIIO2;
        } else {
          AHIIO = AHIIO1;
        }

        audio->previous_read_buffer = audio->read_buffer;
        audio->read_buffer++;
        if (audio->read_buffer >= audio->frags) {
          audio->read_buffer = 0;
        }
      } else {
PRINTF("nothing to play... waiting...\n");
        signals = WaitPPC(SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_D);
PRINTF("got %08lx\n", signals);
        if (signals & SIGBREAKF_CTRL_E) {
          flags |= FLAG_EXIT;
        }
      }
    }
  } else {
DEBUG("AHI failed\n");
  }

DEBUG("wait for last buffer\n");

  if (link != NULL) {
    u32 sigbit = 1L << link->ahir_Std.io_Message.mn_ReplyPort->mp_SigBit;

PRINTF("want: %08lx\n", sigbit);

    signals = WaitPPC(sigbit);

PRINTF("got %08lx\n", signals);

    WaitIO((struct IORequest *)link);
  }

DEBUG("close ahi\n");

  if (device == 0) {
    CloseDevice((struct IORequest *)AHIIO1);
  }
  if (AHIIO1 != NULL) {
    DeleteIORequest(&AHIIO1->ahir_Std);
  }
  if (AHIIO2 != NULL) {
    DeleteIORequest(&AHIIO2->ahir_Std);
  }
  if (AHIMP1 != NULL) {
    DeleteMsgPort(AHIMP1);
  }
  if (AHIMP2 != NULL) {
    DeleteMsgPort(AHIMP2);
  }

DEBUG("AHI closed\n");

  /* send error signal in case we failed */
  SignalPPC(audio->main_task, SIGBREAKF_CTRL_E);
}

audio_t *ahi_open(u32 frequency, u32 mode, u32 fragsize, u32 frags, u32 samples_to_bytes, void (*audio_sync)(u32 time))
{
  u32 i, signals;
  audio_t *audio;

  /* alloc structure */
  audio = malloc(sizeof(audio_t));
  if (audio == NULL) {
    return NULL;
  }
  memset(audio, 0, sizeof(audio_t));

  /* samples to bytes */
  fragsize <<= samples_to_bytes;

  /* store settings */
  audio->frequency = frequency;
  audio->mode = mode;
  audio->fragsize = fragsize;
  audio->frags = frags;
  audio->samples_to_bytes = samples_to_bytes;
  audio->audio_sync = audio_sync;

DEBUG("alloc buffers\n");

  /* allocate buffers */
  audio->audio_buffers = MALLOC(audio->frags * sizeof(audio_buffer_t));
  if (audio->audio_buffers == NULL) {
    goto fail;
  }
  for (i=0; i<audio->frags; i++) {
    audio->audio_buffers[i].buffer = MALLOC(fragsize);
    if (audio->audio_buffers[i].buffer == NULL) {
      goto fail;
    }
    audio->audio_buffers[i].size = 0;
  }

DEBUG("get self\n");

  /* get task pointer */
  audio->main_task = FindTaskPPC(NULL);

DEBUG("remove signals\n");

  /* remove signals */
  SetSignalPPC(0, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E);

DEBUG("allocate semaphore\n");

  /* allocate semaphore */
#ifdef __PPC__
  audio->semaphore = (struct SignalSemaphorePPC *)MALLOC(sizeof(struct SignalSemaphorePPC));
  if (audio->semaphore == NULL) {
    goto fail;
  }
  memset(audio->semaphore, 0, sizeof(struct SignalSemaphorePPC));
  if (InitSemaphorePPC(audio->semaphore) != SSPPC_SUCCESS) {
    FREE(audio->semaphore);
    goto fail;
  }
#else
  audio->semaphore = (struct SignalSemaphore *)MALLOC(sizeof(struct SignalSemaphore));
  if (audio->semaphore == NULL) {
    goto fail;
  }
  memset(audio->semaphore, 0, sizeof(struct SignalSemaphore));
  InitSemaphorePPC(audio->semaphore);
#endif

DEBUG("create task\n");

  if (1) {
#ifdef __PPC__
    struct TagItem ti[]={{TASKATTR_CODE, (ULONG)ahi_task},
                         {TASKATTR_NAME, (ULONG)audio->task_name},
                         {TASKATTR_INHERITR2, TRUE},
                         {TASKATTR_STACKSIZE, 4 * 65536}, /* 64KB should be enough */
                         {TASKATTR_PRI, 10},
                         {TASKATTR_R3, (ULONG)audio},
                         {TAG_DONE, 0}};
#else
    u8 addr[16];
    struct TagItem ti[]={{NP_Entry, (ULONG)ahi_task},
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

DEBUG("ahi open failed\n");

  ahi_close(audio); /* free any allocated resources */

  return NULL;
}

void ahi_play(audio_t *audio, void *data, u32 size, u32 time)
{
  u32 used, copy, max;

  if (audio->audio_task == NULL) {
    return; /* not open */
  }

  if (time != NOTIME) {
    audio->current_time = time;
  }

  time = audio->current_time;

  size <<= audio->samples_to_bytes;

  audio->play = 1; /* start playing */

  while (size > 0) {

PRINTF("audio_play: %d\n", size);
    for (;;) {
      ObtainSemaphorePPC(audio->semaphore);
      used = audio->audio_buffers[audio->write_buffer].used;
      ReleaseSemaphorePPC(audio->semaphore);

      if (used) {
        PRINTF("used: %d\n", audio->write_buffer);
        WaitPPC(SIGBREAKF_CTRL_D);
      } else {
        break;
      }
    }

PRINTF("buffer free: %d\n", audio->write_buffer);

    ObtainSemaphorePPC(audio->semaphore);

    if (audio->audio_buffers[audio->write_buffer].size == audio->fragsize) {
      audio->audio_buffers[audio->write_buffer].size = 0;
    }

    if (audio->audio_buffers[audio->write_buffer].size == 0) {
      audio->audio_buffers[audio->write_buffer].time = time;
    }

    max = audio->fragsize - audio->audio_buffers[audio->write_buffer].size;

    copy = size;
    if (copy > max) {
      copy = max;
    }

    memcpy(audio->audio_buffers[audio->write_buffer].buffer + audio->audio_buffers[audio->write_buffer].size, data, copy);
    audio->audio_buffers[audio->write_buffer].size += copy;
    data += copy;
    size -= copy;

    time += ((copy >> audio->samples_to_bytes) * TIMEBASE) / audio->frequency;

    if (audio->audio_buffers[audio->write_buffer].size == audio->fragsize) {
      audio->audio_buffers[audio->write_buffer].used = 1;
      audio->write_buffer++;
      if (audio->write_buffer >= audio->frags) {
        audio->write_buffer = 0;
      }
    }

PRINTF("release\n");
    ReleaseSemaphorePPC(audio->semaphore);

PRINTF("signal\n");
    SignalPPC(audio->audio_task, SIGBREAKF_CTRL_D); /* FIXME: only signal when successfull write */
  }

  audio->current_time = time;
}

u32 ahi_in_buffer(audio_t *audio)
{
  if (audio->audio_task == NULL) {
    return 0; /* not open */
  }

  return audio->in_buffer;
}

void ahi_pause(audio_t *audio)
{
  if (audio->audio_task == NULL) {
    return; /* not open */
  }

  audio->play = 0; /* stop playing */
}

void ahi_close(audio_t *audio)
{
  u32 i, signals;

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

DEBUG("free semaphore\n");
  /* free semaphore */
  if (audio->semaphore != NULL) {
    FreeSemaphorePPC(audio->semaphore);
    FREE(audio->semaphore);
  }

DEBUG("free buffers\n");
  if (audio->audio_buffers != NULL) {
    for (i=0; i<audio->frags; i++) {
      FREE(audio->audio_buffers[i].buffer);
    }
    FREE(audio->audio_buffers);
  }

  /* free structure */
  free(audio);
}
