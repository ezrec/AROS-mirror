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

#if !defined(_AMGLUT_LOOP_C)
#define _AMGLUT_LOOP_C
#endif

#include <string.h>

#if defined(AROSSHAREDGLUT)
#include <proto/glut.h>
#endif

#include "internal.h"

#include <aros/debug.h>

extern struct AROSMesaGLUT_TaskNode     *_glut_findtask(struct Task *);
extern struct GLUTwindow                *_glut_get_IDCMPWindow(struct AROSMesaGLUT_TaskNode *, struct Window *);

#if defined(AROSSHAREDGLUT)

#warning "Making Shared Library.."

#else
static int              looping = 0;
ULONG                   _glut_TimeEvent_Count = 0;
GLboolean               __glut_global_idle = GL_TRUE;
extern struct MsgPort   *_glut_global_msgport;
#endif

void glutLeaveMainLoop(void)
{
    D(bug("[AMGLUT] In glutLeaveMainLoop()\n"));
    looping--;
}

GLboolean _glut_get_newmenu_params(GLUTmenu *search_menu, GLUTselectCB *menu_func, int *menu_value, IPTR menu_UD)
{
  GLUTmenuentry *menuentrynode = NULL;

  *menu_func = search_menu->_gmn_CB;
  D(bug("[AMGLUT] _glut_get_newmenu_params: menu callback @ %x\n", search_menu->_gmn_CB));

  ForeachNode(&search_menu->_gmn_Entries, menuentrynode)
  {
    if (menuentrynode->_gme_type == GLUT_MENU_SUBMENU_ITEM)
    {
      if ((_glut_get_newmenu_params((GLUTmenu *)menuentrynode->_gme_value, menu_func, menu_value, menu_UD)))
      {
        return GL_TRUE;
      }
    }
    if (menu_UD == (IPTR)menuentrynode)
    {
      D(bug("[AMGLUT] _glut_get_newmenu_params: menuNode_UD == menuentrynode '%s'\n", menuentrynode->_gme_label));
      *menu_value = menuentrynode->_gme_value;
      return GL_TRUE;
    }
  }

  return GL_FALSE;
}

void APIENTRY
_glut_DoTimeEvents (void)
{
  int i;
  for (i = 0; i < MAX_TIMER_CB; i++)
  {
    int time = glutGet(GLUT_ELAPSED_TIME);
    GLUTSShotCB *cb = &_glut_timer_cb[i];
    if (cb->func && (time >= cb->time)){
      cb->func(cb->value);
      cb->func = NULL;  /* Clear call after performing action */
      _glut_TimeEvent_Count = _glut_TimeEvent_Count - 1;
    }
  }
}

void APIENTRY
glutMainLoopEvent (void)
{
  GLUTwindow            *w = NULL;

  struct Window         *MessageHandler_Window = NULL;
  int                   window_MessageID = 0;
  struct IntuiMessage   *imsg = NULL;
  static int            tmp_msg_mouseX = 0;
  static int            tmp_msg_mouseY = 0;
  UWORD                 tmp_msg_code = 0;
  ULONG                 tmp_msg_qual = 0;

  D(bug("[AMGLUT] In glutMainLoopEvent()\n"));

  GLboolean glutwin_NeedsFlush;
  GLboolean glutwin_ChangedVis;

  struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

    while ((imsg = (struct IntuiMessage *)GetMsg(__glutTask->AMGLUTTN_MsgPort)) != NULL)
    {
      glutwin_NeedsFlush = GL_FALSE;
      glutwin_ChangedVis = GL_FALSE;
      w = NULL;

      MessageHandler_Window = imsg->IDCMPWindow;           
      window_MessageID = imsg->Class;
            
      tmp_msg_mouseX = imsg->MouseX;
      tmp_msg_mouseY = imsg->MouseY;
      tmp_msg_code = imsg->Code;
      tmp_msg_qual = imsg->Qualifier;
      (void)tmp_msg_qual; // Unused

      if ((window_MessageID != GLUT_MSG_WINDOWVIRGIN) && (window_MessageID != GLUT_MSG_WINDOWACTIVE))
      {
        ReplyMsg((struct Message *)imsg);
      }
      else
      {
        Remove((struct Node *)&((struct Message *)imsg)->mn_Node);
      }

      if (MessageHandler_Window != NULL)
      {
        w = _glut_get_IDCMPWindow(__glutTask, MessageHandler_Window);
        if (w != NULL)
        {
          int plainKey = tmp_msg_code;
          int specialKey = GL_FALSE;

          w->amglutwin_FlagIdle = GL_TRUE;

          glutSetWindow(w->amglutwin_num);

          switch (window_MessageID)
          {
            case GLUT_MSG_WINDOWVIRGIN:
              {
                D(bug("[AMGLUT] glutMainLoopEvent: First loop of window %d\n", w->amglutwin_num));

                glAMakeCurrent(w->amglutwin_context);
                w->amglutwin_FlagReDisplay = GL_TRUE;

                glutwin_ChangedVis = GL_TRUE;

                if (w->reshape)
                {
                  D(bug("[AMGLUT] glutMainLoopEvent: Calling w->reshape\n"));

                  w->amglutwin_FlagIdle = GL_FALSE;
                  w->reshape(w->amglutwin_width, w->amglutwin_height);
                }

                break;
              }
            case IDCMP_MENUPICK:
              {
                struct MenuItem *menuNode = NULL;
                while ( tmp_msg_code != MENUNULL) {
                  menuNode = ItemAddress(w->amglutwin_WindowMenuStrip, tmp_msg_code);
                  IPTR menuNode_UD = (IPTR)GTMENUITEM_USERDATA(menuNode);
                  GLUTselectCB menu_func = NULL;
                  int menu_value = 0;

                  if (w->amglutwin_WindowGlutMenu != NULL)
                  {
                    if ((_glut_get_newmenu_params((GLUTmenu *)w->amglutwin_WindowGlutMenu, &menu_func, &menu_value, menuNode_UD)))
                    {
                      w->amglutwin_FlagIdle = GL_FALSE;
                      if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                      D(bug("[AMGLUT] glutMainLoopEvent: Calling menu callback func @ %x with value %d\n", menu_func, menu_value));

                      menu_func(menu_value);
                    }
                  }
                  tmp_msg_code = menuNode->NextSelect;
                }
                break;
              }
            case IDCMP_NEWSIZE:
              {
                w->amglutwin_width = w->amglutwin_Window->Width - w->amglutwin_Window->BorderLeft - w->amglutwin_Window->BorderRight;
                w->amglutwin_height = w->amglutwin_Window->Height - w->amglutwin_Window->BorderTop - w->amglutwin_Window->BorderBottom;
                  
                if (w->reshape) {
                  D(bug("[AMGLUT] glutMainLoopEvent: Calling w->reshape\n"));

                  w->amglutwin_FlagIdle = GL_FALSE;
                  glutwin_NeedsFlush = GL_TRUE;
                  if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                  w->reshape(w->amglutwin_width, w->amglutwin_height);
                }
                break;
              }
            case IDCMP_RAWKEY:
              {
                D(bug("[AMGLUT] glutMainLoopEvent: ########### RAWKEY\n"));

                if (tmp_msg_code & IECODE_UP_PREFIX)
                {
                  //A key has been released -> check if we need to process it.
                  break;
                }
                if (plainKey == CURSORLEFT) specialKey = GLUT_KEY_LEFT;
                else if (plainKey == CURSORRIGHT) specialKey = GLUT_KEY_RIGHT;
                else if (plainKey == CURSORUP) specialKey = GLUT_KEY_UP;
                else if (plainKey == CURSORDOWN) specialKey = GLUT_KEY_DOWN;
                else plainKey = GL_FALSE;
              }
            case IDCMP_VANILLAKEY:
              {
                if (plainKey != GL_FALSE)
                {
                  if (specialKey == GL_FALSE)
                  {
                    D(bug("[AMGLUT] glutMainLoopEvent: ########### VANILLAKEY\n"));

/*                  if (plainKey == KEY_F1) specialKey = GLUT_KEY_F1;
                    else if (plainKey == KEY_F2) specialKey = GLUT_KEY_F2;
                    else if (plainKey == KEY_F3) specialKey = GLUT_KEY_F3;
                    else if (plainKey == KEY_F4) specialKey = GLUT_KEY_F4;
                    else if (plainKey == KEY_F5) specialKey = GLUT_KEY_F5;
                    else if (plainKey == KEY_F6) specialKey = GLUT_KEY_F6;
                    else if (plainKey == KEY_F7) specialKey = GLUT_KEY_F7;
                    else if (plainKey == KEY_F8) specialKey = GLUT_KEY_F8;
                    else if (plainKey == KEY_F9) specialKey = GLUT_KEY_F9;
                    else if (plainKey == KEY_F10) specialKey = GLUT_KEY_F10;
                    else if (plainKey == KEY_F11) specialKey = GLUT_KEY_F11;
                    else if (plainKey == KEY_F12) specialKey = GLUT_KEY_F12;
                    else if (plainKey == KEY_LEFT) specialKey = GLUT_KEY_LEFT;
                    else if (plainKey == KEY_UP) specialKey = GLUT_KEY_UP;
                    else if (plainKey == KEY_RIGHT) specialKey = GLUT_KEY_RIGHT;
                    else if (plainKey == KEY_DOWN) specialKey = GLUT_KEY_DOWN;
                    else if (plainKey == KEY_PGUP) specialKey = GLUT_KEY_PAGE_UP;
                    else if (plainKey == KEY_PGDN) specialKey = GLUT_KEY_PAGE_DOWN;
                    else if (plainKey == KEY_HOME) specialKey = GLUT_KEY_HOME;
                    else if (plainKey == KEY_END) specialKey = GLUT_KEY_END;
                    else if (plainKey == KEY_INSERT) specialKey = GLUT_KEY_INSERT;*/
                  }

                  if (specialKey != GL_FALSE)
                  {
                    if (w->special) {
                      D(bug("[AMGLUT] glutMainLoopEvent: Calling w->special\n"));

                      w->amglutwin_FlagIdle = GL_FALSE;
                      w->amglutwin_mouseX = tmp_msg_mouseX;
                      w->amglutwin_mouseY = tmp_msg_mouseY;
                      if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                      w->special(specialKey, w->amglutwin_mouseX, w->amglutwin_mouseY);
                    }
                  }
                  else
                  {
                    if (w->keyboard) {
                      D(bug("[AMGLUT] glutMainLoopEvent: Calling w->keyboard\n"));

                      w->amglutwin_FlagIdle = GL_FALSE;
                      w->amglutwin_mouseX = tmp_msg_mouseX;
                      w->amglutwin_mouseY = tmp_msg_mouseY;
                      if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                      w->keyboard(plainKey & 0xFF, w->amglutwin_mouseX, w->amglutwin_mouseY);
                    }
                  }
                }
                break;
              }
            case IDCMP_MOUSEMOVE:
              {
                if ((w->amglutwin_mouseX != tmp_msg_mouseX)||(w->amglutwin_mouseY != tmp_msg_mouseY))
                {
                  w->amglutwin_mouseX = tmp_msg_mouseX;
                  w->amglutwin_mouseY = tmp_msg_mouseY;
                  if (w->motion)
                  {
                    D(bug("[AMGLUT] glutMainLoopEvent: Calling w->motion\n"));

                    w->amglutwin_FlagIdle = GL_FALSE;
                    if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                    w->motion(w->amglutwin_mouseX, w->amglutwin_mouseY);
                  }
                }
                break;
              }
            case IDCMP_MOUSEBUTTONS:
              {
                if ((w->amglutwin_WindowMenuStrip) &&
                    ((tmp_msg_code == MENUDOWN)||(tmp_msg_code == MENUUP)))
                {
                  break;
                }

                if ((w->amglutwin_mouseX != tmp_msg_mouseX)||(w->amglutwin_mouseY != tmp_msg_mouseY))
                {
                  w->amglutwin_mouseX = tmp_msg_mouseX;
                  w->amglutwin_mouseY = tmp_msg_mouseY;
                  if (w->passive)
                  {
                    D(bug("[AMGLUT] glutMainLoopEvent: Calling w->passive\n"));

                    w->amglutwin_FlagIdle = GL_FALSE;
                    if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                    w->passive(w->amglutwin_mouseX, w->amglutwin_mouseY);
                    break;
                  }
                }

                if (w->mouse)
                {
                  int mousebutt_state = GLUT_DOWN;
                  int mousebutt_button = GLUT_LEFT_BUTTON;

                  D(bug("[AMGLUT] glutMainLoopEvent: Calling w->mouse\n"));

                  if (tmp_msg_code == SELECTUP) mousebutt_state = GLUT_UP;
                  if (tmp_msg_code == MIDDLEDOWN) mousebutt_button = GLUT_MIDDLE_BUTTON;
                  if (tmp_msg_code == MENUDOWN) mousebutt_button = GLUT_RIGHT_BUTTON;
                  if (tmp_msg_code == MIDDLEUP)
                  {
                    mousebutt_state = GLUT_UP;
                    mousebutt_button = GLUT_MIDDLE_BUTTON;
                  }
                  if (tmp_msg_code == MENUUP)
                  {
                    mousebutt_state = GLUT_UP;
                    mousebutt_button = GLUT_RIGHT_BUTTON;
                  }

                  w->amglutwin_FlagIdle = GL_FALSE;
                  if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                  w->mouse(mousebutt_button, mousebutt_state, w->amglutwin_mouseX, w->amglutwin_mouseY);
                }
                break;
              }
            case IDCMP_REFRESHWINDOW:
              {
                w->amglutwin_FlagIdle = GL_FALSE;
                glutwin_NeedsFlush = GL_TRUE;
                break;
              }
            case IDCMP_CLOSEWINDOW:
              {
                if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
                if (w->destroy) {
                  w->amglutwin_FlagIdle = GL_FALSE;
                  w->destroy();
                }
                glutDestroyWindow(w->amglutwin_num);
                glutLeaveMainLoop();
                goto mainloop_end;
                break;
              }
            default:
                break;
          }

          if ((glutwin_ChangedVis == GL_TRUE) && (w->visibility)) {
             D(bug("[AMGLUT] glutMainLoopEvent: Calling w->visibility\n"));

             if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
// TODO: Set the correct visibility here! (always sets visible atm!)
             w->amglutwin_FlagReDisplay = GL_TRUE;
             w->visibility(GLUT_VISIBLE);
          }

          if ((w->amglutwin_FlagReDisplay == GL_TRUE)&& (w->display))
          {
            D(bug("[AMGLUT] glutMainLoopEvent: Calling w->display\n"));

            w->amglutwin_FlagReDisplay = GL_FALSE;
            if (w != __glutTask->AMGLUTTN_WindowCurrent) glAMakeCurrent(w->amglutwin_context);
            w->display(); 
          }
          
          if (w->amglutwin_FlagIdle == GL_FALSE)
          {
            w->amglutwin_FlagReDisplay = GL_TRUE;
            __glutTask->AMGLUTTN_TaskIdle = GL_FALSE;
            w->amglutwin_FlagIdle = GL_TRUE;
          }
          
          if ((glutwin_NeedsFlush == GL_TRUE)) glFlush();
        } /* w != NULL */
      }
    }

  if ((__glutTask->AMGLUTTN_TaskIdle == GL_TRUE) && (_glut_idle_func != NULL))
  {
    w = __glutTask->AMGLUTTN_WindowCurrent;


    D(bug("[AMGLUT] glutMainLoopEvent: Calling global idle func\n"));

    glAMakeCurrent(__glutTask->AMGLUTTN_WindowCurrent->amglutwin_context);
    _glut_idle_func();
    glutPostRedisplay();
    w->amglutwin_InternalMessage->Class = GLUT_MSG_WINDOWACTIVE;
  }
  else
  {  
    if (w == NULL) w = __glutTask->AMGLUTTN_WindowCurrent;
    if (__glutTask->AMGLUTTN_TaskIdle == GL_FALSE)
    {
      w->amglutwin_InternalMessage->Class = GLUT_MSG_WINDOWACTIVE;   
    }
    else
    {
      w->amglutwin_InternalMessage->Class = 0;
    }
    __glutTask->AMGLUTTN_TaskIdle = GL_TRUE;
  }
  

  if ((w != NULL) && (w->amglutwin_InternalMessage->Class != 0))
  {
    PutMsg(w->amglutwin_Window->UserPort, (struct Message *)w->amglutwin_InternalMessage);
  }

mainloop_end:
  /* If were are here from 'case IDCMP_CLOSEWINDOW' the window is already destroyed */
  if (_glut_TimeEvent_Count > 0)
  {
    _glut_DoTimeEvents();
  }
  return;
}


void APIENTRY
glutMainLoop (void)
{
  looping++;
  D(bug("[AMGLUT] In glutMainLoop()\n"));

  while (looping)
  {
    struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
    Wait(1 << __glutTask->AMGLUTTN_MsgPort->mp_SigBit);
    glutMainLoopEvent();
  }
}
