#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

extern int frameskip,autoframeskip;
extern int scanlines, use_tweaked, video_sync, wait_vsync, use_triplebuf;
extern int stretch, use_mmx, use_dirty;
extern int vgafreq, always_synced, skiplines, skipcolumns;
extern float osd_gamma_correction;
extern int gfx_mode, gfx_width, gfx_height;
extern int center_x, center_y;

extern int brightness;

extern struct osd_bitmap *scrbitmap;
extern struct Window *win;
