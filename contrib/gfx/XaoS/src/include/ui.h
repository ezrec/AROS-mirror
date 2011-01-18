/* 
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright (C) 1996,1997 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef UI_H
#define UI_H
#include "xio.h"
#include "config.h"
#include "zoom.h"
#include "param.h"

#undef BUTTON1
#undef BUTTON2
#undef BUTTON3
#define BUTTON1 256
#define BUTTON2 512
#define BUTTON3 1024
#define UI_C256 1
#define UI_REALCOLOR 2
#define UI_HICOLOR 4
#define UI_TRUECOLOR24 8
#define UI_TRUECOLOR 16
#define UI_FIXEDCOLOR 32
#define UI_GRAYSCALE 64
#define UI_TRUECOLORMI 128
#define UI_LBITMAP 256
#define UI_MBITMAP 512
#define UI_LIBITMAP 1024
#define UI_MIBITMAP 2048


#define NORMALMOUSE 0
#define WAITMOUSE 1
#define REPLAYMOUSE 2


#ifdef _UNDEFINED_
void ui_do_fractal (void);
void ui_savefile (void);
void ui_coloringmode (int);
void ui_mandelbrot (int, int);
void ui_incoloringmode (void);
int ui_autopilot (void);
void ui_help (int);
int ui_mouse (int, int, int, int);
int ui_inverse (int);
#endif
void ui_message (void);
void ui_status (void);
void ui_tbreak (void);
void ui_resize (void);
void ui_call_resize (void);
void ui_quit (int);

xio_path ui_getfile (char *basename, char *extension);
int ui_key (int);
/*zoom_context *ui_getcontext(void); */
typedef unsigned char ui_rgb[4];
typedef ui_rgb *ui_palette;

struct ui_driver
  {
    char *name;
    int (*init) (void);		/*initializing function. recturns 0 if fail */
    void (*getsize) (int *, int *);	/*get current size..in fullscreen versions
					   i.e svga and dos asks user for it */
    void (*processevents) (int, int *, int *, int *, int *);
    /*processevents..calls ui_resize,ui_key
       laso returns possitions of mouse..
       waits for event if first parameter is
       1 */
    void (*getmouse) (int *, int *, int *);
    /*returns current mouse possitions */
    void (*uninit) (void);
    /*called before exit */
    int (*set_color) (int, int, int, int);
    void (*set_range) (ui_palette palette, int, int);
    /*sets palette color and returns number */
    void (*print) (int, int, char *);	/*prints text */
    void (*display) (void);	/*displays bitmap */
    int (*alloc_buffers) (char **buffer1, char **buffer2);	/*makes buffers */
    void (*free_buffers) (char *buffer1, char *buffer2);	/*frees buffers */
    void (*flip_buffers) (void);	/*prints text */
    void (*clrscr) (void);
    void (*mousetype) (int type);
    void (*driveropt) (void);
    void (*flush) (void);
    int textwidth;
    int textheight;		/*width of text */
    char **help;
    int helpsize;
    struct params *params;
    int flags;
    float width, height;
    int maxwidth, maxheight;
    int imagetype;
    int palettestart, paletteend, maxentries;
  };

// void ui_menu (char **text, char *label, int n, void (*callback) (int)) REGISTERS (3);
void ui_menu (char **text, char *label, int n, void (*callback) (int));
#ifndef RANDOM_PALETTE_SIZE
#define RANDOM_PALETTE_SIZE 1	/*FIXME currently ignored */
#define FULLSCREEN 2
#define UPDATE_AFTER_PALETTE 4
#define UPDATE_AFTER_RESIZE 8
#define PALETTE_ROTATION 16
#define ASYNC_PALETTE 32
#define ROTATE_INSIDE_CALCULATION 64
#define PALETTE_REDISPLAYS 128
#define SCREENSIZE 256
#define PIXELSIZE 512
#define RESOLUTION 1024
#endif
#define NOFLUSHDISPLAY 2048

#endif /* UI_H */
