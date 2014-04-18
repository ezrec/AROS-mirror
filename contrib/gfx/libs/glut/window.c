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

#if !defined(_AMGLUT_WINDOW_C)
#define _AMGLUT_WINDOW_C
#endif

#include <stdio.h>

#include "internal.h"

#include <aros/debug.h>

extern struct AROSMesaGLUT_TaskNode *_glut_findtask(struct Task *thisTask);
extern struct MsgPort	        *_glut_init_global_msgport(void);

static GLuint swaptime, swapcount;

extern struct MsgPort	*_glut_global_msgport;

/** New Window Lists - use these in future instead of _glut_windows[] **/
IPTR           _glut_windows_count = 0;
struct List    _glut_windows_list;

static GLUTwindow *
_glut_window (struct AROSMesaGLUT_TaskNode *__glutTask, int win)
{
  GLUTwindow *found_GLUTwin = NULL;

  if (win > 0)
  {
    ForeachNode(&__glutTask->AMGLUTTN_WindowList, found_GLUTwin)
    {
       if (found_GLUTwin->amglutwin_num == win) return found_GLUTwin;
    }
  }
  return NULL;
}

GLUTwindow *
_glut_get_IDCMPWindow(struct AROSMesaGLUT_TaskNode *__glutTask, struct Window *IDCMP_Window)
{
  GLUTwindow *found_GLUTwin = NULL;

  if (IDCMP_Window != NULL)
  {
    ForeachNode(&__glutTask->AMGLUTTN_WindowList, found_GLUTwin)
    {
       if (found_GLUTwin->amglutwin_Window == IDCMP_Window) return found_GLUTwin;
    }
  }
  return NULL;
}

ULONG __glut_set_windowIDCMP(struct AROSMesaGLUT_TaskNode *__glutTask, int win)
{
  ULONG new_mask;

  new_mask = IDCMP_CLOSEWINDOW;

  if (win > 0)
  {
    GLUTwindow *_glut_setwindow = _glut_window(__glutTask, win);
    if (_glut_setwindow != NULL)
    {
      _glut_setwindow->amglutwin_WindowIDCMPMask = new_mask;
      /* Set IDCMP for All functions which need to be notified on a resize/shape change */
      if ((_glut_setwindow->reshape != NULL) || (_glut_setwindow->display != NULL))
      {
        _glut_setwindow->amglutwin_WindowIDCMPMask = (_glut_setwindow->amglutwin_WindowIDCMPMask | IDCMP_NEWSIZE);
      }
      /* Set IDCMP for All functions which need keyboard input */
      if ((_glut_setwindow->keyboardUp != NULL) || (_glut_setwindow->keyboard != NULL) ||
          (_glut_setwindow->special != NULL) || (_glut_setwindow->specialUp != NULL))
      {
        _glut_setwindow->amglutwin_WindowIDCMPMask = (_glut_setwindow->amglutwin_WindowIDCMPMask | IDCMP_RAWKEY | IDCMP_VANILLAKEY);
      }
      /* Set IDCMP For All functions which need mouse buttons */
      if ((_glut_setwindow->mouse != NULL) || (_glut_setwindow->passive != NULL))
      {
        _glut_setwindow->amglutwin_WindowIDCMPMask = (_glut_setwindow->amglutwin_WindowIDCMPMask | IDCMP_MOUSEBUTTONS);
      }
      /* Set IDCMP For All functions which need mouse movement */
      if ((_glut_setwindow->motion != NULL) || (_glut_setwindow->passive != NULL))
      {
        _glut_setwindow->amglutwin_WindowIDCMPMask =( _glut_setwindow->amglutwin_WindowIDCMPMask | IDCMP_MOUSEMOVE);
      }
      /* Menus */
      if ((_glut_setwindow->amglutwin_WindowMenuStrip != NULL) || (_glut_setwindow->amglutwin_WindowGlutMenu != NULL))
      {
        _glut_setwindow->amglutwin_WindowIDCMPMask = (_glut_setwindow->amglutwin_WindowIDCMPMask | IDCMP_MENUPICK);
      }
      new_mask = _glut_setwindow->amglutwin_WindowIDCMPMask;
    }
  }

  return new_mask;
}

ULONG __glut_modify_windowIDCMP(struct AROSMesaGLUT_TaskNode *__glutTask, int win)
{
  if (win > 0)
  {
    GLUTwindow *_glut_setwindow = _glut_window(__glutTask, win);
    if (_glut_setwindow != NULL) {
      ULONG new_mask = __glut_set_windowIDCMP(__glutTask, win);
      
      ModifyIDCMP(_glut_setwindow->amglutwin_Window, new_mask);
      if ((new_mask & IDCMP_MENUPICK))
      {
        D(bug("[AMGLUT] __glut_modify_windowIDCMP: Clearing WFLG_RMBTRAP\n"));

        _glut_setwindow->amglutwin_Window->Flags = _glut_setwindow->amglutwin_Window->Flags & ~WFLG_RMBTRAP;
      }
      return new_mask;
    }
  }
  return 0;
};

int APIENTRY
glutCreateWindow (const char *title)
{
   D(bug("[AMGLUT] In glutCreateWindow()\n"));


  GLUTwindow *w = NULL;      
  
  if ((w = (GLUTwindow *)AllocVec(sizeof(GLUTwindow), MEMF_CLEAR|MEMF_PUBLIC)) == NULL) {
    return 0;
  }

  w->amglutwin_Task = FindTask(NULL);
  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(w->amglutwin_Task);

  w->amglutwin_num = ++__glutTask->AMGLUTTN_WindowCount;

  if (!(w->amglutwin_Screen = LockPubScreen("AROSMesaGL")))
    w->amglutwin_Screen = LockPubScreen(NULL);

  if (w->amglutwin_Screen)
  {
        /* open the window on the public screen */
        w->amglutwin_Window = OpenWindowTags(NULL,
              WA_Left,                    _glut_default.x,
              WA_Top,                     _glut_default.y,
              WA_InnerWidth,              _glut_default.width,
              WA_InnerHeight,             _glut_default.height,
              WA_DragBar,                 TRUE,
              WA_CloseGadget,             TRUE,
              WA_SizeGadget,              TRUE,
              WA_DepthGadget,             TRUE,
              WA_SmartRefresh,            TRUE,
              WA_ReportMouse,             TRUE,
              WA_NoCareRefresh,           TRUE,
              WA_RMBTrap,                 TRUE, /* Detect right mouse events, no Menus */
              WA_SizeBBottom,             TRUE,
              WA_MinWidth,                100,
              WA_MinHeight,               30,
              WA_MaxWidth,                -1,
              WA_MaxHeight,               -1,
              WA_IDCMP,                   0,
              WA_Flags,                   WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_ACTIVATE,
              WA_Title,                   title,
              WA_PubScreen,               w->amglutwin_Screen,
              TAG_END);

        /* Unlock the screen.  The window now acts as a lock on
         * the screen, and we do not need the screen after the
         * window has been closed.  */
        UnlockPubScreen(NULL, w->amglutwin_Screen);

        if (w->amglutwin_Window)
        {

          D(bug("[AMGLUT] glutCreateWindow: Window UserPort = %x\n", w->amglutwin_Window->UserPort));

          w->amglutwin_Window->UserPort = __glutTask->AMGLUTTN_MsgPort;
          D(bug("[AMGLUT] glutCreateWindow: changed to %x\n", w->amglutwin_Window->UserPort));


          __glut_modify_windowIDCMP(__glutTask, w->amglutwin_num);
        
          w->amglutwin_InternalMessage = AllocVec(sizeof(struct IntuiMessage), MEMF_CLEAR|MEMF_PUBLIC);
          w->amglutwin_InternalMessage->IDCMPWindow  = w->amglutwin_Window;
          w->amglutwin_InternalMessage->Class        = GLUT_MSG_WINDOWVIRGIN;
          ((struct Message *)w->amglutwin_InternalMessage)->mn_ReplyPort = w->amglutwin_Window->UserPort;

          D(bug("[AMGLUT] glutCreateWindow: Allocated internal window message @ %x\n", w->amglutwin_InternalMessage));


          if ((w->amglutwin_context = glACreateContextTags(
              GLA_Window, (IPTR)w->amglutwin_Window,
              GLA_DoubleBuf, (_glut_default.mode & GLUT_DOUBLE),
              GLA_Left, w->amglutwin_Window->BorderLeft,
              GLA_Top, w->amglutwin_Window->BorderTop,
              GLA_Right, w->amglutwin_Window->BorderRight,
              GLA_Bottom, w->amglutwin_Window->BorderBottom,
              GLA_RGBMode, _glut_default.mode & GLUT_RGB,
              GLA_NoStencil, !(_glut_default.mode & GLUT_STENCIL),
              GLA_NoAccum, !(_glut_default.mode & GLUT_ACCUM),
              GLA_NoDepth, !(_glut_default.mode & GLUT_DEPTH),
              GLA_AlphaFlag, (_glut_default.mode & GLUT_ALPHA),
              TAG_DONE,0)) == NULL)
          {
            D(bug("[AMGLUT] In glutCreateWindow: ERROR: Failed to create a GL Context\n"));

            FreeVec(w);
            return 0;
          }

          __glutTask->AMGLUTTN_WindowCurrent = w;
          __glutTask->AMGLUTTN_WindowCurrent = w;
          w->amglutwin_Virgin = GL_TRUE;
          w->amglutwin_xpos = _glut_default.x;
          w->amglutwin_ypos = _glut_default.y;
          w->amglutwin_width =  _glut_default.width;
          w->amglutwin_height = _glut_default.height;

          glAMakeCurrent(w->amglutwin_context);

          D(bug("[AMGLUT] In glutCreateWindow: Window successfully opened\n"));


          AddTail((struct List *)&__glutTask->AMGLUTTN_WindowList, (struct Node *)&w->amglutwin_Node);
          
          PutMsg(w->amglutwin_Window->UserPort, (struct Message *)w->amglutwin_InternalMessage);
          return __glutTask->AMGLUTTN_WindowCount;
        }
  }
  return 0;
}


int APIENTRY
glutCreateSubWindow (int win, int x, int y, int width, int height)
{
   D(bug("[AMGLUT] In glutCreateSubWindow()\n"));
   return GL_FALSE;
}


void APIENTRY
glutDestroyWindow (int win)
{
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   GLUTwindow *w = _glut_window(__glutTask, win);
   D(bug("[AMGLUT] In glutDestroyWindow(id:%d)\n", win));

   if (w != NULL) {
      Remove((struct Node *)&w->amglutwin_Node);
      if (w->destroy) {
         w->destroy();
      }

	   if (w->amglutwin_context) 
		{
		  glADestroyContext(w->amglutwin_context);
		  w->amglutwin_context = NULL;
		}

	   if (w->amglutwin_Window)
		{
// TODO: We should strip old messages here ..!

        w->amglutwin_Window->UserPort = NULL;		
		  ModifyIDCMP(w->amglutwin_Window, 0L);

		  CloseWindow(w->amglutwin_Window);
		  w->amglutwin_Window = NULL;
		}
      FreeVec(w);
   }
}


void APIENTRY
glutPostRedisplay (void)
{
   D(bug("[AMGLUT] In glutPostRedisplay()\n"));

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->amglutwin_FlagReDisplay = GL_TRUE;
}


void APIENTRY
glutSwapBuffers (void)
{
  D(bug("[AMGLUT] In glutSwapBuffers()\n"));

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

  if (__glutTask->AMGLUTTN_WindowCurrent)
  {
    if (__glutTask->AMGLUTTN_WindowCurrent->show_mouse) {
      /* XXX scare mouse */
    }

    glFlush();
    glASwapBuffers(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context);

    if (__glutTask->AMGLUTTN_WindowCurrent->show_mouse) {
      /* XXX unscare mouse */
    }

    if (_glut_fps) {
      GLint t = glutGet(GLUT_ELAPSED_TIME);
      swapcount++;
      if (swaptime == 0)
         swaptime = t;
      else if (t - swaptime > _glut_fps) {
         double time = 0.001 * (t - swaptime);
         double fps = (double)swapcount / time;
         fprintf(stderr, "GLUT: %d frames in %.2f seconds = %.2f FPS\n", swapcount, time, fps);
         swaptime = t;
         swapcount = 0;
      }
    }
  }
}


int APIENTRY
glutGetWindow (void)
{
  D(bug("[AMGLUT] In glutGetWindow()\n"));

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
  if (__glutTask->AMGLUTTN_WindowCurrent)
   return __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num;
   
  return 0;
}


void APIENTRY
glutSetWindow (int win)
{
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   GLUTwindow *w = _glut_window(__glutTask, win);
   
   D(bug("[AMGLUT] In glutSetWindow(id %d)\n", win));

  if (w != NULL)
  {
    __glutTask->AMGLUTTN_WindowCurrent = w;
 
    if (__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context != glAGetCurrentContext())
    {
      glAMakeCurrent(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context);
    }
  }
}


void APIENTRY
glutSetWindowTitle (const char *title)
{
  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
  
  D(bug("[AMGLUT] In glutSetWindowTitle('%s')\n", title));
  
  if (__glutTask->AMGLUTTN_WindowCurrent != NULL)
  {
      SetWindowTitles(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_Window, title, ((UBYTE *)~0L));
  }
}


void APIENTRY
glutSetIconTitle (const char *title)
{
  D(bug("[AMGLUT] In glutSetIconTitle('%s')\n", title));
}


void APIENTRY
glutPositionWindow (int x, int y)
{
  D(bug("[AMGLUT] In glutPositionWindow(x:%d, y:%d)\n", x, y));
/*   if (DMesaMoveBuffer(x, y)) {
      __glutTask->AMGLUTTN_WindowCurrent->xpos = x;
      __glutTask->AMGLUTTN_WindowCurrent->ypos = y;
   }*/
}


void APIENTRY
glutReshapeWindow (int width, int height)
{ 
    D(bug("[AMGLUT] In glutReshapeWindow(width:%d, height:%d)\n", width, height));

    struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
    if (__glutTask->AMGLUTTN_WindowCurrent->reshape) 
    {
        __glutTask->AMGLUTTN_WindowCurrent->reshape(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_width, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height);
    } else 
    {
        glViewport(0, 0, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_width, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height);
    }
}


void APIENTRY
glutFullScreen (void)
{
  D(bug("[AMGLUT] In glutFullScreen()\n"));
}


void APIENTRY
glutPopWindow (void)
{
  D(bug("[AMGLUT] In glutPopWindow()\n"));
}


void APIENTRY
glutPushWindow (void)
{
  D(bug("[AMGLUT] In glutPushWindow()\n"));
}


void APIENTRY
glutIconifyWindow (void)
{
  D(bug("[AMGLUT] In glutIconifyWindow()\n"));
}


void APIENTRY
glutShowWindow (void)
{
  D(bug("[AMGLUT] In glutShowWindow()\n"));
}


void APIENTRY
glutHideWindow (void)
{
  D(bug("[AMGLUT] In glutHideWindow()\n"));
}


void APIENTRY
glutCloseFunc (GLUTdestroyCB destroy)
{
   D(bug("[AMGLUT] In glutCloseFunc()\n"));

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->destroy = destroy;
}


void APIENTRY
glutPostWindowRedisplay (int win)
{
   D(bug("[AMGLUT] In glutPostWindowRedisplay(id=%d)\n", win));

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   GLUTwindow *w = _glut_window(__glutTask, win);
   if (w != NULL) {
      w->amglutwin_FlagReDisplay = GL_TRUE;
   }
}


void * APIENTRY
glutGetWindowData (void)
{
   D(bug("[AMGLUT] In glutGetWindowData()\n"));

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   return __glutTask->AMGLUTTN_WindowCurrent->data;
}


void APIENTRY
glutSetWindowData (void *data)
{
   D(bug("[AMGLUT] In glutSetWindowData( data = %x)\n", data));

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->data = data;
}
