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

#include <aros/debug.h>

extern struct AROSMesaGLUT_TaskNode     *_glut_findtask(struct Task *);

GLUTSShotCB               _glut_timer_cb[MAX_TIMER_CB];

GLUTidleCB                _glut_idle_func = NULL;

void APIENTRY
glutDisplayFunc (GLUTdisplayCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutDisplayFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->display = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}

void APIENTRY
glutReshapeFunc (GLUTreshapeCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutReshapeFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->reshape = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutKeyboardFunc (GLUTkeyboardCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutKeyboardFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->keyboard = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutMouseFunc (GLUTmouseCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutMouseFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
        D(bug("[AMGLUT] glutMouseFunc: win = %d\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num));
       __glutTask->AMGLUTTN_WindowCurrent->mouse = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutMotionFunc (GLUTmotionCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutMotionFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->motion = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutPassiveMotionFunc (GLUTpassiveCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutPassiveMotionFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->passive = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutEntryFunc (GLUTentryCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutEntryFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->entry = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutVisibilityFunc (GLUTvisibilityCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutVisibilityFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->visibility = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutWindowStatusFunc (GLUTwindowStatusCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutWindowStatusFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->windowStatus = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutIdleFunc (GLUTidleCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutIdleFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       _glut_idle_func = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutTimerFunc (unsigned int millis, GLUTtimerCB func, int value)
{
   int i;

   D(bug("[AMGLUT] In glutTimerFunc()\n"));

   if (millis > 0) {
      for (i = 0; i < MAX_TIMER_CB; i++) {
         GLUTSShotCB *cb = &_glut_timer_cb[i];
         if (cb->func == NULL) {
            cb->value = value;
            cb->func = func;
            cb->time = glutGet(GLUT_ELAPSED_TIME) + millis;
            _glut_TimeEvent_Count = _glut_TimeEvent_Count + 1;
            break;
         }
      }
   }
}


void APIENTRY
glutSpecialFunc (GLUTspecialCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutSpecialFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->special = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutSpaceballMotionFunc (GLUTspaceMotionCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutSpaceballMotionFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->spaceMotion = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutSpaceballRotateFunc (GLUTspaceRotateCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutSpaceballRotateFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->spaceRotate = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutSpaceballButtonFunc (GLUTspaceButtonCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutSpaceballButtonFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->spaceButton = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutDialsFunc (GLUTdialsCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutDialsFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->dials = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutButtonBoxFunc (GLUTbuttonBoxCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutButtonBoxFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->buttonBox = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutTabletMotionFunc (GLUTtabletMotionCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutTabletMotionFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->tabletMotion = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutTabletButtonFunc (GLUTtabletButtonCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutTabletButtonFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->tabletButton = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutJoystickFunc (GLUTjoystickCB func, int interval)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

   D(bug("[AMGLUT] In glutJoystickFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->joystick = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutKeyboardUpFunc (GLUTkeyboardCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

    D(bug("[AMGLUT] In glutKeyboardUpFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->keyboardUp = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}


void APIENTRY
glutSpecialUpFunc (GLUTspecialCB func)
{
    struct AROSMesaGLUT_TaskNode *__glutTask;

    D(bug("[AMGLUT] In glutSpecialUpFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->specialUp = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);

    }
}


void APIENTRY
glutMouseWheelFunc (GLUTmouseWheelCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask;

    D(bug("[AMGLUT] In glutMouseWheelFunc()\n"));

    if ((__glutTask = _glut_findtask(FindTask(NULL))) && __glutTask->AMGLUTTN_WindowCurrent)
    {
       __glutTask->AMGLUTTN_WindowCurrent->mouseWheel = func;
       __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
    }
}
