#ifndef __glutaros_h__
#define __glutaros_h__

/*
    Copyright © 2004, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

#include "aros_x11.h"
#include "aros_glx.h"

/* "equivalent" cursors - eventually, the X glyphs should be
   converted to aros cursors -- then they will look the same */
#define XC_arrow               0x00;//IDC_ARROW
#define XC_top_left_arrow      0x01;//IDC_ARROW
#define XC_hand1               0x02;//IDC_SIZEALL
#define XC_pirate              0x03;//IDC_NO
#define XC_question_arrow      0x04;//IDC_HELP
#define XC_exchange            0x05;//IDC_NO
#define XC_spraycan            0x06;//IDC_SIZEALL
#define XC_watch               0x07;//IDC_WAIT
#define XC_xterm               0x08;//IDC_IBEAM
#define XC_crosshair           0x09;//IDC_CROSS
#define XC_sb_v_double_arrow   0x0a;//IDC_SIZENS
#define XC_sb_h_double_arrow   0x0b;//IDC_SIZEWE
#define XC_top_side            0x0c;//IDC_UPARROW
#define XC_bottom_side         0x0d;//IDC_SIZENS
#define XC_left_side           0x0e;//IDC_SIZEWE
#define XC_right_side          0x0f;//IDC_SIZEWE
#define XC_top_left_corner     0x10;//IDC_SIZENWSE
#define XC_top_right_corner    0x11;//IDC_SIZENESW
#define XC_bottom_right_corner 0x12;//IDC_SIZENWSE
#define XC_bottom_left_corner  0x13;//IDC_SIZENESW

#define XA_STRING 0

/* Function prototypes */
/*int
gettimeofday(struct timeval* tp, void* tzp);*/

void*
__glutFont(void *font);

int
__glutGetTransparentPixel(Display * dpy, XVisualInfo * vinfo);

void
__glutAdjustCoords(Window parent, int* x, int* y, int* width, int* height);

#endif /* __glutaros_h__ */
