#ifndef EXTERNS_H

#define EXTERNS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avcodec.h"
#include "avformat.h"
#include "SDL.h"

extern int use_audio, audio_sample_rate, audio_channels, frame_width,
    frame_height, verbose, bytes_in_buffer, benchmark_mode, debugging,
    no_video;
extern unsigned long sample_frame;
extern double frame_rate;

#define AUDIOPORT_BUFFERSIZE 0x100000

extern void os_delay(int);
extern unsigned long get_elapsed_samples(void);
extern void handle_events(void);
extern void display_picture(AVPicture *);
extern void init_system(char *);
extern int write_audio(char *, int);
extern void start_audio(void);
extern void stop_audio(void);
extern int play_decode_stream(AVFormatContext *);
#endif

