/*
 *
 * plugin.h
 *
 */

#ifndef PLUGIN_H
#define PLUGIN_H

/* Types and FOURCCs */
#include "core_types.h"

/* Result codes from init, decode and exit functions */
#define PLUGIN_OK   (0)
#define PLUGIN_FAIL (1)

/* Ident */
typedef struct {
  unsigned long type;
  unsigned long fourcc;
  int (*init) (unsigned long type, unsigned long fourcc);
  int (*decode) (unsigned char *buf, unsigned long length, double sync_time);
  int (*exit) (void);
} plugin_ident;

#define IDENT_INIT \
plugin_ident plugin_idents[] = { \
{ 0x24414d50 /* $AMP */, 0x50494e32 /* PIN2 */, NULL, NULL, NULL}, /* Has to be first */

#define IDENT_ADD(type, fourcc, init, decode, exit) \
{ type, fourcc, init, decode, exit},

#define IDENT_EXIT \
{ 0, 0, NULL, NULL, NULL}};

/* Plugin */
typedef struct {
  plugin_ident *ident;

  /* Video */
  int width;
  int height;
  int depth;
  double framerate;
  int gray;

  /* Verbose, Video */
  unsigned long total_frames;
  unsigned long skiped_frames;

  /* Audio */
  int rate;
  int bits;
  int channels;
  int divisor;
  int desired_bits;
  int desired_channels;

  /* Verbose */
  char verbose[64]; /* Not valid until video_init or audio_init is called by the plugin */

  /* Refresh functions supplied by AMP */
  int (*video_init) (int width, int height, int line_width, unsigned long cmode);
  int (*video_refresh) (unsigned char *data[3], double sync_time, double frame_time);
  int (*video_refresh_palette) (unsigned long *palette, int entries); /* ARGB32 palette */
  int (*video_refresh_yuv2rgb) (int crv, int cgu, int cgv, int cbu, int cym, int cya, int cuv);
  int (*audio_init) (int rate, int bits, int channels);
  int (*audio_refresh) (void *src, int length, double sync_time);

  /* Special */
  char *plugin_path;
  unsigned long id;
  unsigned long subid;
} plugin_data;

/* To access AMP functions */
extern plugin_data *amp;

/* Allocate and free aligned memory */
extern void *mallocalign (unsigned long size);
extern void freealign (void *buffer);

/* The only things the plugin need to initialize */
extern plugin_ident plugin_idents[];
extern const char *version;

#define VERSION(a) const char *version = "$VER: " a

#endif
