/*
 *
 * osd.h
 *
 */

#ifndef OSD_H
#define OSD_H

typedef struct {
  int width, height;
  unsigned char *data;
  int space, kern;
} osd_char;

extern osd_char osd_chars[256];

extern void osd_init(int w, int h, int l_w, int mode);
extern void osd_init_palette(unsigned long *argb, unsigned long *ayuv, int entries);
extern void (*osd_refresh)(unsigned char *image[3], unsigned char *osd, int x, int y, int w, int h, int l_w);
extern void osd_exit(void);

extern void subtitle_init(double framerate);
extern void subtitle_refresh(unsigned char *src[], double image_time);
extern void subtitle_exit(void);

#endif
