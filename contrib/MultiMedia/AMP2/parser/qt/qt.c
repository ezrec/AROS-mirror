/*
 *
 * qt.c
 *
 */

#include <stdio.h>
#include <stdlib.h>

#if defined(__AROS__)
#include "aros-inc.h"
#endif

#include "../../common/core.h"
#include "../../common/ampio.h"
#include "../../main/main.h"
#include "../../main/buffer.h"

#include "quicktime.h"

extern long quicktime_is_keyframe(quicktime_t *file, long frame, int track);

extern unsigned long get_plugin_custom(unsigned long type, unsigned long fourcc); /* FIXME: Remove */
extern unsigned long samples_to_bytes;

typedef struct {
  unsigned long id;
  /* Video */
  int stream;
  int width;
  int height;
  int depth;
  double framerate;
} video_info;

typedef struct {
  unsigned long id;
  /* Audio */
  int stream;
  int rate;
  int bits;
  int channels;
} audio_info;

static video_info vi;
static audio_info ai;

typedef struct {
  longest position;
  long length;
  int type;
  double timestamp;
  int is_keyframe;
} qt_node;

static qt_node *qt_index;

#define QT_VIDEO BUFFER_VIDEO
#define QT_AUDIO BUFFER_AUDIO

int build_index(quicktime_t *file, int track, int video, int audio)
{
  qt_node video_node;
  qt_node audio_node;

  long video_length;
  long audio_length;
  long total_length;
  long total_samples = 0;
  int index = 0;
  int is_keyframe = 0;

  int video_index = 0;
  int audio_index = 0;

  memset(&video_node, 0, sizeof(qt_node));
  memset(&audio_node, 0, sizeof(qt_node));

  video_length = quicktime_video_length(file, track);
  audio_length = quicktime_audio_length(file, track);

  total_length = 0;
  if (audio) {
    total_length += audio_length;
    audio_node.type = -1; /* Need to get node */
  }
  if (video) {
    total_length += video_length;
    video_node.type = -1; /* Need to get node */
  }

  qt_index = malloc(total_length * sizeof(qt_node));

  while (index < total_length) {

    /* Get video node */
    if ((video) && (video_index < video_length) && (video_node.type == -1)) {
      long bytes;

      bytes = quicktime_frame_size(file, file->vtracks[track].current_position, track);
      quicktime_set_video_position(file, file->vtracks[track].current_position, track);
      is_keyframe = quicktime_is_keyframe(file, file->vtracks[track].current_position, track);

      video_node.position = file->file_position;
      video_node.length = bytes;
      video_node.type = QT_VIDEO;
      video_node.timestamp = (double)video_index / vi.framerate;
      video_node.is_keyframe = is_keyframe;

      file->vtracks[track].current_position++;
      video_index++;
    }

    /* Get audio node */
    if ((audio) && (audio_index < audio_length) && (audio_node.type == -1)) {
      long bytes;

      bytes = quicktime_audio_frame_size(file, 0);

      audio_node.position = file->file_position;
      audio_node.length = bytes;
      audio_node.type = QT_AUDIO;
      audio_node.timestamp = (double)total_samples / (double)ai.rate;
      audio_node.is_keyframe = 0; /* FIXME: Is this always the case ? */

      /* FIXME: This must be done nicer */
      if (samples_to_bytes > 1) {
        bytes *= samples_to_bytes;
      }

      if (ai.channels == 2) {
        bytes /= 2;
      }

      if (ai.bits == 16) {
        bytes /= 2;
      }

      total_samples += bytes;

      audio_index++;
    }

    /* Choose node to add to index */
    if ((video_node.type != -1) && (audio_node.type != -1)) {
      if (video_node.position < audio_node.position) {
        qt_index[index] = video_node;
        video_node.type = -1; /* Need to get node */
      } else {
        qt_index[index] = audio_node;
        audio_node.type = -1; /* Need to get node */
      }
    } else {
      if (video_node.type != -1) {
        qt_index[index] = video_node;
        video_node.type = -1; /* Need to get node */
      } else if (audio_node.type != -1) {
        qt_index[index] = audio_node;
        audio_node.type = -1; /* Need to get node */
      } else {
        break; /* No more nodes */
      }
    }
    index++;
  }

#if 0
{
  int i;
  for (i=0; i<index; i++) {
    printf("%s %ld -> %ld %ld %f\n", (qt_index[i].type == QT_VIDEO) ? "VIDEO" : "AUDIO",
                              (long)qt_index[i].position, (long)qt_index[i].position + qt_index[i].length,
                              qt_index[i].length, qt_index[i].timestamp);
  }
}
#endif

  return index;
}

int qt_parser(char *filename, int video, int audio)
{
  int decoders_running = 0;
  long audio_length = 0;
  long video_length = 0;
  int i, index = 0, len, has_keyframes = 0;
  quicktime_t *file;

  if(!(file = quicktime_open(filename))) {
    printf("could not open file\n");
    return 0;
  }

  if(quicktime_audio_tracks(file)) {
    audio_length = quicktime_audio_length(file, 0);

    ai.id = *(unsigned long *)quicktime_audio_compressor(file, 0);
    ai.rate = quicktime_sample_rate(file, 0);
    ai.bits = quicktime_audio_bits(file, 0);
    ai.channels = quicktime_track_channels(file, 0);

    debug_printf("%dx%dx%d\n", ai.rate, ai.bits, ai.channels);

    samples_to_bytes = get_plugin_custom(TYPE_AUDIO | SUBTYPE_NONE, ai.id);

    if (samples_to_bytes == 0) {
      printf("No suitable audio plugin available\n");
      audio = 0;
    }

    samples_to_bytes = GET_CUSTOM(samples_to_bytes);

    debug_printf("audio length: %d, samples_to_bytes: %ld\n", (int)audio_length, samples_to_bytes);
  } else {
    audio = 0;
  }

  if(quicktime_video_tracks(file)) {
    video_length = quicktime_video_length(file, 0);

    vi.id = *(unsigned long *)quicktime_video_compressor(file, 0);
    vi.width = quicktime_video_width(file, 0);
    vi.height = quicktime_video_height(file, 0);
    vi.depth = quicktime_video_depth(file, 0);
    vi.framerate = quicktime_frame_rate(file, 0);

    has_keyframes = quicktime_has_keyframes(file, 0);

    debug_printf("%dx%dx%d @ %f : %d\n", vi.width, vi.height, vi.depth, vi.framerate, has_keyframes);

    if (get_plugin_custom(TYPE_VIDEO | SUBTYPE_NONE, vi.id) == 0) {
      printf("No suitable video plugin available\n");
      video = 0;
    }

    debug_printf("video length: %d\n", (int)video_length);
  } else {
    video = 0;
  }

  if ((audio == 0) && (video == 0)) {
    printf("Neither video nor audio can be played\n");
    quicktime_close(file);
    return 0;
  }

  index = build_index(file, 0, video, audio);
  quicktime_close(file);

  if (index <= 0) {
    return 0;
  }

  if (ampio_fopen(filename) != 1) {
    return 0;
  }

  /* Restrict index to filesize */
  len = ampio_length();
  for (i=0; i<index; i++) {
    if ((qt_index[i].position + qt_index[i].length) > len) {
      index = i - 1; /* Stop at the frame bofore the "after end" one */
      break;
    }
  }

  /* Play the movie */
  for (i=0; i<index; i++) {
#if 0
    volatile unsigned char *lmb = (unsigned char *)0xbfe001;
    if ((*lmb & 0x40) == 0) { /* LMB */
      video_fempty();
      audio_fempty();
      break;
    }
#else
    /* FIXME: The "every 4th write" hack is no good */
    if ((decoders_running == 1) && ((i & 3) == 0)) {
      int check, set, pos;

      if (video) {
        pos = video_ftell();
      } else {
        pos = audio_ftell();
      }

      set = (pos * 65536) / index;
      check = core_input(set);

      if ((check >= 0) && (has_keyframes)) {
        i = (index * check) / 100; /* MAX 100 % */

        /* Search to next keyframe */
        while (qt_index[i].is_keyframe == 0) {
          i++;
          if (i >= index) {
            break;
          }
        }

        if (audio) {
          audio_fseek();
        }

        if (video) {
          video_fseek();
        }
      } else if (check == INPUT_QUIT) {
        video_fempty();
        audio_fempty();
        break;
      }
    }
#endif

    if ((i >= index) || (i < 0)) {
      break;
    }

    ampio_fseek((int)qt_index[i].position, SEEK_SET);

    while (amp_fwrite(qt_index[i].type, NULL, qt_index[i].length, qt_index[i].timestamp, i) == BUF_FALSE) {
      if (decoders_running == 0) {
        decoders_running = 1;

        plugin_init();

        if (video) {
          if (plugin_video_init(SUBTYPE_NONE, vi.id, vi.width, vi.height, vi.depth, vi.framerate) != CORE_OK) {
            video = 0;
          }
        }

        if (audio) {
          if (plugin_audio_init(SUBTYPE_NONE, ai.id, ai.rate, ai.bits, ai.channels) != CORE_OK) {
            audio = 0;
          }
        }

        if ((audio == 0) && (video == 0)) {
          goto exit; /* FIXME */
        }

        plugin_start();
      }
    }
  }

  if (decoders_running) {
    plugin_exit();
  }

exit:

  ampio_fclose();

  return 0;
}
