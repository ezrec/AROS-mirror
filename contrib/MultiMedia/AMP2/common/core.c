/*
 *
 * core.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#if !defined(__AROS__)
#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/graphics.h>
#include <powerup/ppcproto/dos.h>

#include <powerpc/powerpc_protos.h>
#else

#include "aros-inc.h"

#endif

#include <amiaudio.h>
#include "core.h"
#include "../main/main.h"
#include "../main/prefs.h"
#include "../refresh/refresh.h"
#include "../amigaos/video.h"
#include "../main/buffer.h"
#include "../common/plugin.h"
#include "../common/amp_plugin.h"

static struct SignalSemaphorePPC *semaphore = NULL;
static int do_flip, do_pause;
static int w, h, lw;

static struct {
  unsigned long video_type;
  unsigned long video_fourcc;
  unsigned long audio_type;
  unsigned long audio_fourcc;
  int video_task_running;
  int audio_task_running;
  amiaudio_t *amiaudio;
} local;

/* Variables protected by the semaphore -> */

static struct timeval st = { 0, 0 };

/* <- Variables protected by the semaphore */

/* Task handling */

static struct TaskPPC *audio_decoder_task = NULL;
static struct TaskPPC *video_decoder_task = NULL;
static struct TaskPPC *main_task = NULL;

static unsigned long amp_signals[3] = { 0, 0, 0 };

#define SIGNAL_OK    (amp_signals[0])
#define SIGNAL_ERROR (amp_signals[1])
#define SIGNAL_QUIT  (amp_signals[2])

static char audio_decoder_name[32];
static char video_decoder_name[32];

#define STACKSIZE (256*1024) /* 256kb should be enough */

static amp_plugin_type *audio_pin = NULL;
static amp_plugin_type *video_pin = NULL;

static unsigned long refresh_mode = 0;

static int video_decoder_ok = 0;
static int audio_decoder_ok = 0;

/* Functions */

int amp_video_init(int width, int height, int line_width, unsigned long cmode)
{
  char txt[16];

  w = width;
  h = height;
  lw = line_width;

  /* Title */
  sprintf(txt, " [%dx%d]", w, h);
  strcat(prefs.window_title, txt);

  verbose_printf("VIDEO VERBOSE: '%s'\n", video_pin->data->verbose);

  if (prefs.speedtest & 0x02) { /* NULL */
    goto done; /* FIXME: Bad! */
  }

  refresh_mode = 0;
  if (cmode == CMODE_YUV420) {
    refresh_mode = REFRESH_YUV420;
  } else if (cmode == CMODE_ARGB32) {
    refresh_mode = REFRESH_ARGB32;
  } else if (cmode == CMODE_CHUNKY) {
    refresh_mode = REFRESH_LUT8;
  } else {
    return PLUGIN_FAIL;
  }

  if (refresh_open(w, h, lw, refresh_mode, video_pin->data->framerate) < 0) {
    refresh_close();
    return PLUGIN_FAIL;
  }

done:
  SetSignalPPC(0, SIGNAL_OK);
  SignalPPC(main_task, SIGNAL_OK);
  WaitPPC(SIGNAL_OK);

  video_decoder_ok = 1; /* FIXME */

  return PLUGIN_OK;
}

/* Allow the playback to be this much too early/late */
#define MAXDIFF 0.06

/* Maximum time to wait when too early */
#define MAXWAIT 1.00 /* Has to be BIG for RM and such... */

/* FIXME: Remove !!! */
unsigned char **pre_src;
double pre_sync_time;

int amp_video_refresh(unsigned char *src[], double sync_time, double frame_time)
{
  struct timeval dt = { 0, 0 };
  double delta, time;
  int speedtest = prefs.speedtest, speedret = (prefs.speedtest >> 3) & 0x01;

  if (speedtest & 0x02) { /* NULL */
    return speedret;
  }

  if (audio_pin != NULL) {
    time = sync_time;
  } else {
    time = frame_time;
  }

  /* FIXME: !!! */
  pre_src = src;
  pre_sync_time = sync_time;

  refresh_image(src, sync_time);

  if (speedtest & 0x01) { /* ANY SPEEDTEST */
    return speedret;
  }

  if (do_flip) {
    do_flip = 0;
    debug_printf("FLIP\n");

    refresh_close();
    if (prefs.window == PREFS_OFF) {
      prefs.window = PREFS_ON;
    } else {
      prefs.window = PREFS_OFF;
    }
    if (refresh_open(w, h, lw, refresh_mode, video_pin->data->framerate) < 0) {
      SignalPPC(main_task, SIGNAL_ERROR);
    } else {
      SignalPPC(main_task, SIGNAL_OK);
    }
  }

  if (do_pause) {
    struct timeval ct = { 0, 0 };
    debug_printf("PAUSE\n");

    GetSysTimePPC(&dt);
    SubTimePPC(&dt, &st);

    /* Wait for audio task to pause, important! */
    if (audio_pin != NULL) {
      WaitPPC(SIGNAL_OK);
    }

    for (;;) {
      int check = check_window(-1, 1);
      if (check == CHECK_CONT) {
        break;
      }
    }

    GetSysTimePPC(&ct);
    SubTimePPC(&ct, &dt);
    st = ct; /* Set start time to what it should be */

    /* Reset this variable before starting the audio/main task to not screw things up */
    do_pause = 0;

    if (audio_pin != NULL) {
      SignalPPC(audio_decoder_task, SIGNAL_OK);
    }

    SignalPPC(main_task, SIGNAL_OK);

    return 0; /* Don't try to sync after this mess, wait until next frame */
  }

  GetSysTimePPC(&dt);

  ObtainSemaphorePPC(semaphore);
  SubTimePPC(&dt, &st);
  ReleaseSemaphorePPC(semaphore);

  delta = dt.tv_sec+(dt.tv_usec/1000000.0);

  /* If we are mote than 5 sec off, something must be wrong, so let's just resync */
  if (fabs(delta - time) > 5.0) {
    struct timeval tt = { 0, 0 };

    tt.tv_sec = time;
    tt.tv_usec = (time - (double)tt.tv_sec) * 1000000;

    GetSysTimePPC(&dt);

    ObtainSemaphorePPC(semaphore);
    SubTimePPC(&dt, &tt);
    st = dt; /* Set start time to what it should be */
    ReleaseSemaphorePPC(semaphore);
  } if (delta < time) {
    /* We are too early */
    double wait = time - delta;

    /* Compare to maximum wait time */
    if (wait > (MAXWAIT + MAXDIFF)) {
      wait = (MAXWAIT + MAXDIFF); /* This may happend when seeking */
    }

    /* Are we fast enough to wait ? */
    if (wait > MAXDIFF) {
      wait -= MAXDIFF;
      WaitTime(0, (wait * 1000000.0));
    }
  } else {
    /* We are too late */
    double late = delta - time;

    /* Only skip when we are noticably too late */
    if (late > MAXDIFF) {
      return 1; /* Do skip frames */
    }
  }

  return 0; /* Do not skip frames */
}

int amp_video_refresh_palette(unsigned long *palette, int entries)
{
  return 0;
}

int amp_video_refresh_yuv2rgb(int crv, int cgu, int cgv, int cbu, int cym, int cya, int cuv)
{
  refresh_yuv2rgb(crv, cgu, cgv, cbu, cym, cya, cuv);

  return 0;
}

static int amp_video_stop(unsigned long total_frames, unsigned long skiped_frames)
{
  struct timeval dt = { 0, 0 };

  if ((prefs.speedtest & 0x01) && (total_frames > 0)) {
    GetSysTimePPC(&dt);
    SubTimePPC(&dt, &st);
    printf("Time: %.3f, Frames: %ld\n", (double)dt.tv_sec + ((double)dt.tv_usec / 1000000.0), total_frames);
    printf("FPS: %.3f\n", (double)total_frames / ((double)dt.tv_sec + ((double)dt.tv_usec / 1000000.0)));
  }

  verbose_printf("Frames: %d of which %d were skiped\n", total_frames, skiped_frames);

  return 0;
}

/* This routine set the global time, this way it doesn't matter
   if the audio is played a little too fast/slow and it keeps video
   and audio synced when the stream has timestamps. */

static void audio_sync(u32 time)
{
  struct timeval dt = { 0, 0 }, tt = { 0, 0 };
  double sync_time;

  sync_time = (double)time / (double)TIMEBASE;

/*  printf("sync: %ld, %f\n", time, sync_time); */

  tt.tv_sec = sync_time;
  tt.tv_usec = (sync_time - (double)tt.tv_sec) * 1000000;

  GetSysTimePPC(&dt);

  ObtainSemaphorePPC(semaphore);
  SubTimePPC(&dt, &tt);
  st = dt; /* Set start time to what it should be */
  ReleaseSemaphorePPC(semaphore);
}

int amp_audio_init(int rate, int bits, int channels)
{
  int mode = 0x00;

  if (bits == 16) {
    mode |= MODE_16;
  }

  if (channels == 2) {
    mode |= MODE_STEREO;
  }

  verbose_printf("AUDIO VERBOSE: '%s'\n", audio_pin->data->verbose);

  /* Create default output to atleast get a window with the close gadget visible */
  if (strlen(audio_pin->data->verbose) == 0) {
    sprintf(audio_pin->data->verbose, "Playing a %d Hz %d bit %s stream",
                                      rate, bits, (channels == 2) ? "stereo" : "mono");
  }

  audio_init(local.amiaudio);
  audio_open(local.amiaudio, rate, mode, rate / 4, 4, audio_sync); /* FIXME: Return value */

  if (video_pin == NULL) {
    open_audio_window(audio_pin->data->verbose);
  }

  SetSignalPPC(0, SIGNAL_OK);
  SignalPPC(main_task, SIGNAL_OK);
  WaitPPC(SIGNAL_OK);

  audio_decoder_ok = 1; /* FIXME */

  return PLUGIN_OK;
}

int amp_audio_refresh(void *src, int length, double sync_time)
{
  if (prefs.speedtest) {
    return PLUGIN_OK; /* Don't play or sync */
  }

  if (length > 0) {
    u32 time = (sync_time * (double)TIMEBASE);
    audio_play_bytes(local.amiaudio, src, length, time);
  }

  if (do_pause) {
    audio_pause(local.amiaudio);
    if (video_pin != NULL) {
      SignalPPC(video_decoder_task, SIGNAL_OK);
      WaitPPC(SIGNAL_OK);
    } else {
      debug_printf("PAUSE\n");
      for (;;) {
        int check = check_window(-1, 1);
        if (check == CHECK_CONT) {
          break;
        }
      }
      SignalPPC(main_task, SIGNAL_OK);
      do_pause = 0;
    }
  }

  return PLUGIN_OK;
}

static int amp_refresh_flip(void)
{
  /* The main task uses the window structure, so stop it until it's ready! */
  SetSignalPPC(0, SIGNAL_OK | SIGNAL_ERROR);
  do_flip = 1;
  if (WaitPPC(SIGNAL_OK | SIGNAL_ERROR) == SIGNAL_ERROR) {
    return -1;
  } else {
    return 0;
  }
}

static void amp_pause(void)
{
  /* The main task may steal IDCMP messages, so stop it until we continue */
  SetSignalPPC(0, SIGNAL_OK);
  do_pause = 1;
  WaitPPC(SIGNAL_OK);
}

int core_input(int slider_position)
{
  int check;

  if (prefs.speedtest & 0x01) {
    volatile unsigned char *lmb = (unsigned char *)0xbfe001;
    if ((*lmb & 0x40) == 0) { /* LMB */
      return INPUT_QUIT;
    } else {
      return INPUT_OK;
    }
  }

  check = check_window(slider_position, 0);

  if (check >= 0) {
    return check;
  } else if (check == CHECK_QUIT) {
    return INPUT_QUIT;
  } else if (check == CHECK_PAUSE) {
    amp_pause();
  } else if (check == CHECK_FLIP) {
    amp_refresh_flip();
  }

  return INPUT_OK;
}

static void video_decoder(void)
{
  unsigned char *buffer;
  double sync_time;

  SetSignalPPC(0, SIGNAL_QUIT);

  if (video_pin->ident->init(local.video_type, local.video_fourcc) == PLUGIN_OK) {
    for (;;) {
      int length = video_fread(&buffer, &sync_time);
      if (length >= 0) {
        if (video_pin->ident->decode(buffer, length, sync_time) != PLUGIN_OK) {
          break;
        }
      }

      if (length == BUF_EOF) {
        break;
      }
    }
    amp_video_stop(video_pin->data->total_frames, video_pin->data->skiped_frames);
    video_pin->ident->exit();
  }

  if (video_decoder_ok == 1) {
    WaitPPC(SIGNAL_QUIT);
  }

  refresh_close(); /* Free any allocated video resources */

  SignalPPC(main_task, SIGNAL_ERROR); /* Just in case */
}

static void audio_decoder(void)
{
  unsigned char *buffer;
  double sync_time;

  SetSignalPPC(0, SIGNAL_QUIT);

  if (audio_pin->ident->init(local.audio_type, local.audio_fourcc) == PLUGIN_OK) {
    for (;;) {
      int length = audio_fread(&buffer, &sync_time);
      if (length >= 0) {
        if (audio_pin->ident->decode(buffer, length, sync_time) != PLUGIN_OK) {
          break;
        }
      }

      if (length == BUF_EOF) {
        break;
      }
    }
    audio_pin->ident->exit();
  }

  if (audio_decoder_ok == 1) {
    WaitPPC(SIGNAL_QUIT);
  }

  audio_close(local.amiaudio); /* Free any allocated audio resources */
  if (video_pin == NULL) {
    refresh_close(); /* Free any allocated video resources */
  }

  SignalPPC(main_task, SIGNAL_ERROR); /* Just in case */
}

static unsigned long local_id = 0, local_subid = 0;

int plugin_set_id(unsigned long id, unsigned long subid)
{
  local_id = id;
  local_subid = subid;

  return 0;
}

int plugin_video_init(unsigned long type, unsigned long fourcc, int width, int height, int depth, double framerate)
{
  struct TagItem ti[]={{TASKATTR_CODE, (ULONG)video_decoder},
                       {TASKATTR_NAME, (ULONG)video_decoder_name},
                       {TASKATTR_STACKSIZE, STACKSIZE},
                       {TASKATTR_INHERITR2, TRUE},
                       {NULL, 0}};

  /* Store type and fourcc */
  local.video_type = type;
  local.video_fourcc = fourcc;
  local.video_task_running = 0;

  type |= TYPE_VIDEO;

  if (prefs.colormode == PREFS_GRAY) {
    type |= FEATURE_GRAY;
  }

  video_pin = wait_for_plugin_init(type, fourcc);

  if (video_pin != NULL) {
    video_pin->data->width = width;
    video_pin->data->height = height;
    video_pin->data->depth = depth;
    video_pin->data->framerate = framerate;
    video_pin->data->gray = (prefs.colormode == PREFS_GRAY);
  } else {
    amp_printf("Failed to get video plugin\n");
    video_fempty();
    return CORE_FAIL;
  }

  /* Hack */
  video_pin->data->id = local_id;
  video_pin->data->subid = local_subid;

  /* Task */
  SetSignalPPC(0, SIGNAL_OK | SIGNAL_ERROR);

  /* Create task */
  video_decoder_task = CreateTaskPPC(ti);
  if (video_decoder_task == NULL) {
    amp_printf("Failed to create video decoder task\n");
    video_fempty();
    return CORE_FAIL;
  }
  if (WaitPPC(SIGNAL_OK | SIGNAL_ERROR) == SIGNAL_ERROR) {
    amp_printf("Failed to start video decoder task\n");
    while (FindTaskPPC(video_decoder_name) != NULL) {
      WaitTime(0, 100000);
    }
    video_fempty();
    return CORE_FAIL;
  }

  local.video_task_running = 1;

  return CORE_OK;
}

int plugin_audio_init(unsigned long type, unsigned long fourcc, int rate, int bits, int channels)
{
  struct TagItem ti[]={{TASKATTR_CODE, (ULONG)audio_decoder},
                       {TASKATTR_NAME, (ULONG)audio_decoder_name},
                       {TASKATTR_STACKSIZE, STACKSIZE},
                       {TASKATTR_INHERITR2, TRUE},
                       {NULL, 0}};

  /* Store type and fourcc */
  local.audio_type = type;
  local.audio_fourcc = fourcc;
  local.audio_task_running = 0;
  local.amiaudio = AUDIO_NEW(AUDIO_AHI, (prefs.ahi == PREFS_ON),
                             AUDIO_STEREO, (prefs.stereo == PREFS_ON),
                             AUDIO_PAULA_56KHZ, (prefs.khz56 == PREFS_ON),
                             AUDIO_PAULA_14BIT, (prefs.audiomode == PREFS_14BIT),
                             AUDIO_PAULA_CALIB, (u32)prefs.calibration);

  type |= TYPE_AUDIO;

  audio_pin = wait_for_plugin_init(type, fourcc);

  if (audio_pin != NULL) {
    audio_pin->data->rate = rate;
    audio_pin->data->bits = bits;
    audio_pin->data->channels = channels;
    audio_pin->data->divisor = prefs.divisor;
    audio_pin->data->desired_bits = (audio_init(local.amiaudio) & MODE_16) ? 16 : 8; /* FIXME: correct ? */
    audio_pin->data->desired_channels = (prefs.stereo == PREFS_ON) ? 2 : 1;
  } else {
    amp_printf("Failed to get audio plugin\n");
    audio_fempty();
    return CORE_FAIL;
  }

  /* Hack */
  video_pin->data->id = local_id;
  video_pin->data->subid = local_subid;

  /* Task */
  SetSignalPPC(0, SIGNAL_OK | SIGNAL_ERROR);

  /* Create task */
  audio_decoder_task = CreateTaskPPC(ti);
  if (audio_decoder_task == NULL) {
    amp_printf("Failed to create audio decoder task\n");
    audio_fempty();
    return CORE_FAIL;
  }
  if (WaitPPC(SIGNAL_OK | SIGNAL_ERROR) == SIGNAL_ERROR) {
    amp_printf("Failed to start audio decoder task\n");
    while (FindTaskPPC(audio_decoder_name) != NULL) {
      WaitTime(0, 100000);
    }
    audio_fempty();
    return CORE_FAIL;
  }

  local.audio_task_running = 1;

  return CORE_OK;
}

int plugin_init(void)
{
  int i, decoder = 0;

  video_decoder_ok = 0; /* FIXME */
  audio_decoder_ok = 0; /* FIXME */

  main_task = FindTaskPPC(NULL);

  /* Allocate some signals */
  for (i=0; i<3; i++) {
    amp_signals[i] = AllocSignalPPC(-1);
    SetSignalPPC(0, amp_signals[i]);
  }

  semaphore = (struct SignalSemaphorePPC *)malloc(sizeof(struct SignalSemaphorePPC));
  if (semaphore == NULL) {
    return CORE_FAIL;
  }
  memset(semaphore, 0, sizeof(struct SignalSemaphorePPC));
#if defined(__AROS__)
  InitSemaphore(semaphore);
#else
  if (InitSemaphorePPC(semaphore) != SSPPC_SUCCESS) {
    free(semaphore);
    return CORE_FAIL;
  }
#endif

  do_flip = 0;
  do_pause = 0;

  /* Get decoder names */
  do {
    decoder++;
    sprintf(audio_decoder_name, "Audio_Decoder.%04d", decoder);
    sprintf(video_decoder_name, "Video_Decoder.%04d", decoder);
  } while ((FindTaskPPC(audio_decoder_name) != NULL) ||
           (FindTaskPPC(video_decoder_name) != NULL));

  memset(&local, 0, sizeof(local));

  amp_plugin_init(); /* FIXME: Return value */

  return CORE_OK;
}

int plugin_start(void)
{
  ObtainSemaphorePPC(semaphore);
  GetSysTimePPC(&st);
  ReleaseSemaphorePPC(semaphore);

  if (local.video_task_running) {
    SignalPPC(video_decoder_task, SIGNAL_OK); /* FIXME */
  }

  if (local.audio_task_running) {
    SignalPPC(audio_decoder_task, SIGNAL_OK); /* FIXME */
  }

  return CORE_OK;
}

int plugin_exit(void)
{
  int i;

  /* End of file */
  video_feof();
  audio_feof();

  /* Wait for tasks to stop */
  if (FindTaskPPC(video_decoder_name) != NULL) {
    SignalPPC(video_decoder_task, SIGNAL_QUIT); /* FIXME */
  }
  while (FindTaskPPC(video_decoder_name) != NULL) {
    WaitTime(0, 100000);
  }

  if (FindTaskPPC(audio_decoder_name) != NULL) {
    SignalPPC(audio_decoder_task, SIGNAL_QUIT); /* FIXME */
  }
  while (FindTaskPPC(audio_decoder_name) != NULL) {
    WaitTime(0, 100000);
  }

  if (video_pin != NULL) {
    wait_for_plugin_exit(video_pin);
    video_pin = NULL;
  }

  if (audio_pin != NULL) {
    wait_for_plugin_exit(audio_pin);
    audio_pin = NULL;
  }

  amp_plugin_exit(); /* FIXME */

  FreeSemaphorePPC(semaphore);
  free(semaphore);

  /* Free the signals */
  for (i=0; i<3; i++) {
    FreeSignalPPC(amp_signals[i]);
  }

  return CORE_OK;
}
