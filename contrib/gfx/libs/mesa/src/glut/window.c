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

extern struct AROSMesaGLUT_TaskNode *_glut_findtask(struct Task *thisTask);
extern struct MsgPort	        *_glut_init_global_msgport(void);

static GLuint swaptime, swapcount;
static IPTR visual = (IPTR)NULL;

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
      if ((_glut_setwindow->amglutwin_WindowMenuStrip != NULL) || (_glut_setwindow->amglutwin_WindowGlutMenu != (IPTR)NULL))
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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] __glut_modify_windowIDCMP: Clearing WFLG_RMBTRAP\n"));
#endif
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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow()\n"));
#endif

  GLUTwindow *w = NULL;      
  GLenum   RGB_Flag  = GL_TRUE;

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

#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] glutCreateWindow: Window UserPort = %x\n", w->amglutwin_Window->UserPort));
#endif

          w->amglutwin_Window->UserPort = __glutTask->AMGLUTTN_MsgPort;
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] glutCreateWindow: changed to %x\n", w->amglutwin_Window->UserPort));
#endif

          __glut_modify_windowIDCMP(__glutTask, w->amglutwin_num);
        
          w->amglutwin_InternalMessage = AllocVec(sizeof(struct IntuiMessage), MEMF_CLEAR|MEMF_PUBLIC);
          w->amglutwin_InternalMessage->IDCMPWindow  = w->amglutwin_Window;
          w->amglutwin_InternalMessage->Class        = GLUT_MSG_WINDOWVIRGIN;
          ((struct Message *)w->amglutwin_InternalMessage)->mn_ReplyPort = w->amglutwin_Window->UserPort;

#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] glutCreateWindow: Allocated internal window message @ %x\n", w->amglutwin_InternalMessage));
#endif

          if (_glut_default.mode & GLUT_INDEX)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: Indexed Color (IGNORED)\n"));
#endif
            RGB_Flag = GL_FALSE;
          }

          w->amglutwin_FlagDB = GL_FALSE;
          if (_glut_default.mode & GLUT_DOUBLE)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: Double Buffered\n"));
#endif
            w->amglutwin_FlagDB = GL_TRUE;
          }

          w->amglutwin_cliprect.MinX = w->amglutwin_Window->BorderLeft;   /* AROS: Clip the rastport to the visible area */
          w->amglutwin_cliprect.MinY = w->amglutwin_Window->BorderTop;
          w->amglutwin_cliprect.MaxX = -w->amglutwin_Window->BorderRight;
          w->amglutwin_cliprect.MaxY = -w->amglutwin_Window->BorderBottom;

#if 0
          /* Allocate the rendering Context. */
          if ((w->amglutwin_context = AROSMesaCreateContextTags(
//              AMA_RastPort, (IPTR)w->amglutwin_Window->RPort,
//              AMA_DoubleBuf, w->amglutwin_FlagDB,
              TAG_DONE,0)) == NULL)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: ERROR: Failed to create a GL Context\n"));
#endif
            FreeVec(w);
            return 0;
          }

//            if (win.type & TK_OVERLAY)
//            {
//              w->amglutwin_ctx_overlay_visual = AROSMesaCreateVisualTags(
//                w->amglutwin_context,
//                AMA_RastPort,                   (IPTR)w->amglutwin_Window->RPort,
//                hiddMesaGL_TAG_RastVisible,     GL_FALSE,
//                AMA_DoubleBuf,                  GL_FALSE,
//                RPTAG_ClipRectangle,            &w->amglutwin_cliprect,
//                TAG_DONE,0);
//            }

          if ((w->amglutwin_ctx_def_visual = AROSMesaCreateVisualTags(
              w->amglutwin_context,
//              AMA_RastPort,                   (IPTR)w->amglutwin_Window->RPort,
              RPTAG_ClipRectangle,            &w->amglutwin_cliprect,
//              AMA_DoubleBuf,                  w->amglutwin_FlagDB,
              TAG_DONE,0)) == (IPTR)NULL)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: ERROR: Failed to create a GL Visual\n"));
#endif
            return 0;
          }
          w->amglutwin_ctx_visual = w->amglutwin_ctx_def_visual;

          AROSMesaInitContext(w->amglutwin_context, w->amglutwin_ctx_visual);

//            if (win.type & TK_OVERLAY)
//            {
//              w->amglutwin_ctx_overlay_buffer = AROSMesaCreateBuffers(w->amglutwin_context, w->amglutwin_ctx_overlay_visual);
//            }
          if ((w->amglutwin_ctx_def_buffer = AROSMesaCreateBuffers(w->amglutwin_context, w->amglutwin_ctx_visual)) == (IPTR)NULL)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: ERROR: Failed to create a GL Buffers\n"));
#endif
            return 0;
          }

//           win.cur_level = TK_RGB;

#endif /* 0 */


          if ((w->amglutwin_context = AROSMesaCreateContextTags(
              AMA_Window, (IPTR)w->amglutwin_Window,
//              AMA_DoubleBuf, w->amglutwin_FlagDB,
              TAG_DONE,0)) == NULL)
          {
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: ERROR: Failed to create a GL Context\n"));
#endif
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

//          AROSMesaMakeCurrent(w->amglutwin_context, w->amglutwin_ctx_visual);
          AROSMesaMakeCurrent(w->amglutwin_context);

#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateWindow: Window successfully opened\n"));
#endif

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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCreateSubWindow()\n"));
#endif
   return GL_FALSE;
}


void APIENTRY
glutDestroyWindow (int win)
{
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   GLUTwindow *w = _glut_window(__glutTask, win);
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutDestroyWindow(id:%d)\n", win));
#endif
   if (w != NULL) {
      Remove((struct Node *)&w->amglutwin_Node);
      if (w->destroy) {
         w->destroy();
      }

	   if (w->amglutwin_context) 
		{
//      AROSMesaMakeCurrent(NULL, NULL);
//      AROSMesaDestroyBuffers(w->buffer);
//      AROSMesaDestroyContext(w->context);
		  AROSMesaDestroyContext(w->amglutwin_context);
		  w->amglutwin_context = NULL;
		}

	   if (w->amglutwin_Window)
		{
#warning "TODO: We should strip old messages here ..!"

        w->amglutwin_Window->UserPort = NULL;		
		  ModifyIDCMP(w->amglutwin_Window, 0L);

		  CloseWindow(w->amglutwin_Window);
		  w->amglutwin_Window = NULL;
		}
      FreeVec(w);
//      _glut_windows[win - 1] = NULL;
   }
}


void APIENTRY
glutPostRedisplay (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPostRedisplay()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->amglutwin_FlagReDisplay = GL_TRUE;
}


void APIENTRY
glutSwapBuffers (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSwapBuffers()\n"));
#endif
  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

  if (__glutTask->AMGLUTTN_WindowCurrent)
  {
    if (__glutTask->AMGLUTTN_WindowCurrent->show_mouse) {
      /* XXX scare mouse */
    }

    glFlush();
    AROSMesaSwapBuffers(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context);

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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGetWindow()\n"));
#endif
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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSetWindow(id %d)\n", win));
#endif
  if (w != NULL)
  {
    __glutTask->AMGLUTTN_WindowCurrent = w;
//    if (__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context != AROSMesaGetCurrentContext())
    {
//      AROSMesaMakeCurrent(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_ctx_visual);
      AROSMesaMakeCurrent(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context);
    }
  }
}


void APIENTRY
glutSetWindowTitle (const char *title)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSetWindowTitle('%s')\n", title));
#endif
}


void APIENTRY
glutSetIconTitle (const char *title)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSetIconTitle('%s')\n", title));
#endif
}


void APIENTRY
glutPositionWindow (int x, int y)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPositionWindow(x:%d, y:%d)\n", x, y));
#endif
/*   if (DMesaMoveBuffer(x, y)) {
      __glutTask->AMGLUTTN_WindowCurrent->xpos = x;
      __glutTask->AMGLUTTN_WindowCurrent->ypos = y;
   }*/
}


void APIENTRY
glutReshapeWindow (int width, int height)
{ 
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutReshapeWindow(width:%d, height:%d)\n", width, height));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
/*   if (DMesaResizeBuffer(width, height)) {
      __glutTask->AMGLUTTN_WindowCurrent->amglutwin_width = width;
      __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height = height;*/
      if (__glutTask->AMGLUTTN_WindowCurrent->reshape) {
         __glutTask->AMGLUTTN_WindowCurrent->reshape(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_width, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height);
      } else {
         glViewport(0, 0, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_width, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_height);
      }
/*   }*/
}


void APIENTRY
glutFullScreen (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutFullScreen()\n"));
#endif
}


void APIENTRY
glutPopWindow (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPopWindow()\n"));
#endif
}


void APIENTRY
glutPushWindow (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPushWindow()\n"));
#endif
}


void APIENTRY
glutIconifyWindow (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutIconifyWindow()\n"));
#endif
}


void APIENTRY
glutShowWindow (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutShowWindow()\n"));
#endif
}


void APIENTRY
glutHideWindow (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutHideWindow()\n"));
#endif
}


void APIENTRY
glutCloseFunc (GLUTdestroyCB destroy)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutCloseFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->destroy = destroy;
}


void APIENTRY
glutPostWindowRedisplay (int win)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPostWindowRedisplay(id=%d)\n", win));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   GLUTwindow *w = _glut_window(__glutTask, win);
   if (w != NULL) {
      w->amglutwin_FlagReDisplay = GL_TRUE;
   }
}


void * APIENTRY
glutGetWindowData (void)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutGetWindowData()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   return __glutTask->AMGLUTTN_WindowCurrent->data;
}


void APIENTRY
glutSetWindowData (void *data)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSetWindowData( data = %x)\n", data));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->data = data;
}
