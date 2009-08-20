/*
 * DOS/DJGPP Mesa Utility Toolkit
 * Version:  1.0
 *
 * Copyright (C) 2005  Daniel Borca   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DANIEL BORCA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include "internal.h"

extern struct AROSMesaGLUT_TaskNode     *_glut_findtask(struct Task *);

int _glut_mouse;
int _glut_mouse_x = 0, _glut_mouse_y = 0;

void
_glut_mouse_init (void)
{
#if defined(DEBUG_AROSMESAGLUT)
D(bug("[AMGLUT] In _glut_mouse_init()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

//   if ((_glut_mouse = pc_install_mouse())) {
  {
//      pc_mouse_area(__glutTask->AMGLUTTN_WindowCurrent->xpos, __glutTask->AMGLUTTN_WindowCurrent->ypos, __glutTask->AMGLUTTN_WindowCurrent->xpos + __glutTask->AMGLUTTN_WindowCurrent->width - 1, __glutTask->AMGLUTTN_WindowCurrent->ypos + __glutTask->AMGLUTTN_WindowCurrent->height - 1);

      __glutTask->AMGLUTTN_WindowCurrent->show_mouse = (__glutTask->AMGLUTTN_WindowCurrent->mouse || __glutTask->AMGLUTTN_WindowCurrent->motion || __glutTask->AMGLUTTN_WindowCurrent->passive);
   }
}


void APIENTRY
glutSetCursor (int cursor)
{
#if defined(DEBUG_AROSMESAGLUT)
D(bug("[AMGLUT] In glutSetCursor()\n"));
#endif
   /* XXX completely futile until full mouse support (maybe never) */
}


void APIENTRY
glutWarpPointer (int x, int y)
{
#if defined(DEBUG_AROSMESAGLUT)
D(bug("[AMGLUT] In glutWarpPointer()\n"));
#endif
//   pc_warp_mouse(x, y);
}
