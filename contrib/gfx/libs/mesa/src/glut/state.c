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

#include <stdio.h>

#include "internal.h"

extern struct AROSMesaGLUT_TaskNode     *_glut_findtask(struct Task *);

static int timer_installed;
#include <time.h>

static struct timeval time_start;

int APIENTRY
glutGet (GLenum type)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGet(type:%d) ", type));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

   switch (type) {
      case GLUT_WINDOW_X:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_X = %d\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_xpos));
#endif
         return __glutTask->AMGLUTTN_WindowCurrent->amglutwin_xpos;
      }
      case GLUT_WINDOW_Y:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_Y = %d\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_ypos));
#endif
         return __glutTask->AMGLUTTN_WindowCurrent->amglutwin_ypos;
      }
      case GLUT_WINDOW_WIDTH:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_WIDTH = %d\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_width));
#endif
         return __glutTask->AMGLUTTN_WindowCurrent->amglutwin_width;
      }
      case GLUT_WINDOW_HEIGHT:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_HEIGHT = %d\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height));
#endif
         return __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height;
      }
      case GLUT_WINDOW_STENCIL_SIZE:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_STENCIL_SIZE = %d\n", _glut_visual.stencil));
#endif
         return _glut_visual.stencil;
      }
      case GLUT_WINDOW_DEPTH_SIZE:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_DEPTH_SIZE = %d\n", _glut_visual.depth));
#endif
         return _glut_visual.depth;
      }
      case GLUT_WINDOW_RGBA:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_RGBA ="));
if (!(_glut_default.mode & GLUT_INDEX))
{
D(bug(" TRUE\n"));
}
else
{
D(bug(" FALSE\n"));
}
#endif
         return !(_glut_default.mode & GLUT_INDEX);
      }
      case GLUT_WINDOW_COLORMAP_SIZE:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_WINDOW_COLORMAP_SIZE\n"));
#endif
         return (_glut_default.mode & GLUT_INDEX) ? (256 - RESERVED_COLORS) : 0;
      }
      case GLUT_SCREEN_WIDTH:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_SCREEN_WIDTH = %d\n", _glut_visual.geometry[0]));
#endif
         return _glut_visual.geometry[0];
      }
      case GLUT_SCREEN_HEIGHT:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_SCREEN_HEIGHT = %d\n", _glut_visual.geometry[1]));
#endif
         return _glut_visual.geometry[1];
      }
      case GLUT_INIT_WINDOW_X:
         return _glut_default.x;
      case GLUT_INIT_WINDOW_Y:
         return _glut_default.y;
      case GLUT_INIT_WINDOW_WIDTH:
         return _glut_default.width;
      case GLUT_INIT_WINDOW_HEIGHT:
         return _glut_default.height;
      case GLUT_INIT_DISPLAY_MODE:
         return _glut_default.mode;
      case GLUT_ELAPSED_TIME:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("GLUT_ELAPSED_TIME\n"));
#endif
         if (!timer_installed) {
            timer_installed = GL_TRUE;
/*            gettimeofday(&then, NULL);*/
            CurrentTime(&time_start.tv_sec, &time_start.tv_usec);
            return 0;
         } else {
            struct timeval time_current;
/*            gettimeofday(&now, NULL);
            return (now.tv_usec - then.tv_usec) / 1000 +
                   (now.tv_sec  - then.tv_sec)  * 1000;*/
            CurrentTime(&time_current.tv_sec, &time_current.tv_usec);
            return (time_current.tv_usec - time_start.tv_usec) / 1000 +
                   (time_current.tv_sec  - time_start.tv_sec)  * 1000;
         }
      }
      default:
      {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("UNKNOWN VALUE!\n"));
#endif
         return -1;
      }
   }
}


int APIENTRY
glutDeviceGet (GLenum type)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutDeviceGet()\n"));
#endif
   switch (type) {
      case GLUT_HAS_KEYBOARD:
         return GL_TRUE;
      case GLUT_HAS_MOUSE:
         return GL_TRUE;
      case GLUT_NUM_MOUSE_BUTTONS:
         return 3;
      case GLUT_HAS_SPACEBALL:
      case GLUT_HAS_DIAL_AND_BUTTON_BOX:
      case GLUT_HAS_TABLET:
         return GL_FALSE;
      case GLUT_NUM_SPACEBALL_BUTTONS:
      case GLUT_NUM_BUTTON_BOX_BUTTONS:
      case GLUT_NUM_DIALS:
      case GLUT_NUM_TABLET_BUTTONS:
         return 0;
      default:
         return -1;
   }
}


int APIENTRY
glutGetModifiers (void)
{
   int mod = 0;
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGetModifiers()\n"));
#endif
//   int shifts = pc_keyshifts();

/*   if (shifts & (KB_SHIFT_FLAG | KB_CAPSLOCK_FLAG)) {
      mod |= GLUT_ACTIVE_SHIFT;
   }

   if (shifts & KB_ALT_FLAG) {
      mod |= GLUT_ACTIVE_ALT;
   }

   if (shifts & KB_CTRL_FLAG) {
      mod |= GLUT_ACTIVE_CTRL;
   }*/

   return mod;
}


void APIENTRY
glutReportErrors (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutReportErrors()\n"));
#endif
   /* reports all the OpenGL errors that happened till now */
}


/* GAME MODE
 * Hack alert: incomplete... what is GameMode, anyway?
 */
static GLint game;
static GLboolean game_possible;
static GLboolean game_active;
static GLuint game_width;
static GLuint game_height;
static GLuint game_bpp;
static GLuint game_refresh;


void APIENTRY
glutGameModeString (const char *string)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGameModeString()\n"));
#endif
   if (sscanf(string, "%ux%u:%u@%u", &game_width, &game_height, &game_bpp, &game_refresh) == 4) {
      game_possible = GL_TRUE;
   }
}


int APIENTRY
glutGameModeGet (GLenum mode)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGameModeGet()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   switch (mode) {
      case GLUT_GAME_MODE_ACTIVE:
         return game_active;
      case GLUT_GAME_MODE_POSSIBLE:
         return game_possible && !__glutTask->AMGLUTTN_WindowCurrent;
      case GLUT_GAME_MODE_WIDTH:
         return game_active ? (int)game_width : -1;
      case GLUT_GAME_MODE_HEIGHT:
         return game_active ? (int)game_height : -1;
      case GLUT_GAME_MODE_PIXEL_DEPTH:
         return game_active ? (int)game_bpp : -1;
      case GLUT_GAME_MODE_REFRESH_RATE:
         return game_active ? (int)game_refresh : -1;
      default:
         return -1;
   }
}


int APIENTRY
glutEnterGameMode (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutEnterGameMode()\n"));
#endif
   if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
      _glut_visual.bpp = game_bpp;
      _glut_visual.refresh = game_refresh;

      glutInitWindowSize(game_width, game_height);

      if ((game = glutCreateWindow("<game>")) > 0) {
         game_active = GL_TRUE;
      }

      return game;
   } else {
      return 0;
   }
}


void GLUTAPIENTRY
glutLeaveGameMode (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutLeaveGameMode()\n"));
#endif
   if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE)) {
      game_active = GL_FALSE;

      glutDestroyWindow(game);
   }
}
