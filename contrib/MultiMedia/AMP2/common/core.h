/*
 *
 * core.h
 *
 */

#ifndef CORE_H
#define CORE_H

#include "core_types.h"

/* Generic */
int plugin_init(void);
int plugin_set_id(unsigned long id, unsigned long subid);
int plugin_video_init(unsigned long type, unsigned long fourcc, int width, int height, int depth, double framerate);
int plugin_audio_init(unsigned long type, unsigned long fourcc, int rate, int bits, int channels);
int plugin_start(void);
int plugin_exit(void);

/* Video */
int amp_video_init(int width, int height, int line_width, unsigned long cmode);
int amp_video_refresh(unsigned char *src[], double sync_time, double frame_time);
int amp_video_refresh_palette(unsigned long *palette, int entries);
int amp_video_refresh_yuv2rgb(int crv, int cgu, int cgv, int cbu, int cym, int cya, int cuv);

/* Audio */
int amp_audio_init(int rate, int bits, int channels);
int amp_audio_refresh(void *src, int length, double sync_time);

/* Input */
#define INPUT_OK   -1
#define INPUT_QUIT -2
int core_input(int slider_position);

#endif
