/*
**  CDXLPlay (C) 2009 Fredrik Wikstrom
**/

#ifndef CDXLPLAY_H
#define CDXLPLAY_H

#include <SDL/SDL.h>
#ifdef __AROS__
#include <GL/gl.h>
#else
#include <GL/GL.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "endian.h"
#include "lists.h"
#include "cdxl.h"
#include "p2c.h"

#define PROGNAME "CDXLPlay"
#define VSTRING "1.4"

#define NULL ((void *)0)
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

struct cdxl_file {
	char *filename;
	FILE *file;
	int num_frames;
	struct list list;
	int padded_width;
	uint8_t *chunky;
	int has_audio;
};

struct cdxl_frame {
	struct node node;
	int index;
	PAN pan;
	off_t offset;
	int size;
	int cmap_size;
	int plane_size;
	int video_size;
	int audio_size;
	int is_ham;
	int is_stereo;
	uint8_t *buffer;
};

struct audio_node {
	struct node node;
	int frame;
	int8_t *pcm;
	int apos;
	int alen;
};

struct player_data {
	const char *filename;
	uint32_t sdl_video_flags;
	SDL_Surface *screen;
	SDL_Surface *surf;
	struct cdxl_file *cdxl;
	struct cdxl_frame *frame;
	SDL_TimerID timer_id;
	struct cdxl_frame *aframe;
	struct list audio_list;
	SDL_mutex *audio_mutex;
	SDL_cond *audio_cond;
	int audio_enable;
	int aprebuf;
	int fps;
	int freq;
	int loop;
	int width, height;
	int pixel_ratio;
	int status;
	struct timeval tv;
};

enum {
	PAUSED,
	PLAYING
};

/* main.c */
int main (int argc, char *argv[]);

/* options.c */
int get_options (int argc, char *argv[], struct player_data *pd);
void free_options (void);

/* cdxlplay.c */
struct cdxl_file *open_cdxl(const char *filename);
void close_cdxl(struct cdxl_file *cdxl);
int read_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame);
void free_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame);
int decode_cdxl_frame(struct cdxl_file *cdxl, struct cdxl_frame *frame, SDL_Surface *surf);

/* scale.c */
void ScaleSurface(SDL_Surface *source, SDL_Surface *dest, uint32_t pix_ratio);

/* opengl.c */
void GLInit(int w, int h);
void GLExit();
void GLScaleSurface(SDL_Surface *source, SDL_Surface *dest, uint32_t pix_ratio);

#endif
