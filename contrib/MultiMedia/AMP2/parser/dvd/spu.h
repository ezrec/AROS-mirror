/*
 *
 * spu.h
 *
 */

#ifndef SPU_H
#define SPU_H

extern int spu_setup(int width, int height, int lores);
extern void spu_palette(unsigned char *palette);
extern void spu_render(unsigned char *src[], double video_time);
extern void spu_exit(void);

#endif
