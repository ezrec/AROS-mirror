/*
 *
 * video.h
 *
 */

#ifndef AMIGAOS_VIDEO_H
#define AMIGAOS_VIDEO_H

/* Video */
extern int open_screen(int width, int height, int mode);
extern void set_palette(int colors);
extern void close_screen(void);

/* Audio */
extern void open_audio_window(char *text);

/* Video & Audio */
#define CHECK_OK    -1
#define CHECK_QUIT  -2
#define CHECK_PAUSE -3
#define CHECK_CONT  -4
#define CHECK_FLIP  -5
extern int check_window(int new_position, int do_wait);

#endif
