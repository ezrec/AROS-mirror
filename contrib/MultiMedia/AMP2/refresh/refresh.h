/*
 *
 * refresh.h
 *
 */

#ifndef REFRESH_H
#define REFRESH_H

/* Modes */

#define REFRESH_YUV420 1
#define REFRESH_ARGB32 2
#define REFRESH_LUT8   3

/* Allocate buffers and initialize tables */
extern int refresh_init(void);

/* Open a screen/window */
extern int refresh_open(int w, int h, int l_w, unsigned long mode, double framerate);

/* Refresh the display */
extern void refresh_image(unsigned char *src[3], double image_time);

/* Call this when the palette have changed */
extern void refresh_palette(unsigned long *palette, int entries);

/* Call this to setup YUV (YCrCb) to RGB conversion */
extern void refresh_yuv2rgb(int crv, int cgu, int cgv, int cbu, int cym, int cya, int cuv);

/* Close the screen/window */
extern void refresh_close(void);

/* Perform a (speed)test */
extern void refresh_test(char *filename);

/* Free resources */
extern void refresh_exit(void);

#endif
