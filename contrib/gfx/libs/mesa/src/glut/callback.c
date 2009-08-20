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

GLUTSShotCB               _glut_timer_cb[MAX_TIMER_CB];

GLUTidleCB                _glut_idle_func = NULL;

void APIENTRY
glutDisplayFunc (GLUTdisplayCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutDisplayFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->display = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutReshapeFunc (GLUTreshapeCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutReshapeFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->reshape = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutKeyboardFunc (GLUTkeyboardCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutKeyboardFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->keyboard = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutMouseFunc (GLUTmouseCB func)
{
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutMouseFunc(win:%d)\n", __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num));
#endif
   __glutTask->AMGLUTTN_WindowCurrent->mouse = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutMotionFunc (GLUTmotionCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutMotionFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->motion = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutPassiveMotionFunc (GLUTpassiveCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutPassiveMotionFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->passive = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutEntryFunc (GLUTentryCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutEntryFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->entry = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutVisibilityFunc (GLUTvisibilityCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutVisibilityFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->visibility = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutWindowStatusFunc (GLUTwindowStatusCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutWindowStatusFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->windowStatus = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutIdleFunc (GLUTidleCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutIdleFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   _glut_idle_func = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutTimerFunc (unsigned int millis, GLUTtimerCB func, int value)
{
   int i;

#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutTimerFunc()\n"));
#endif

   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

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
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSpecialFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->special = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutSpaceballMotionFunc (GLUTspaceMotionCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSpaceballMotionFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->spaceMotion = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutSpaceballRotateFunc (GLUTspaceRotateCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSpaceballRotateFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->spaceRotate = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutSpaceballButtonFunc (GLUTspaceButtonCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSpaceballButtonFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->spaceButton = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutDialsFunc (GLUTdialsCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutDialsFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->dials = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutButtonBoxFunc (GLUTbuttonBoxCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutButtonBoxFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->buttonBox = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutTabletMotionFunc (GLUTtabletMotionCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutTabletMotionFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->tabletMotion = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutTabletButtonFunc (GLUTtabletButtonCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutTabletButtonFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->tabletButton = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutJoystickFunc (GLUTjoystickCB func, int interval)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutJoystickFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->joystick = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutKeyboardUpFunc (GLUTkeyboardCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutKeyboardUpFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->keyboardUp = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutSpecialUpFunc (GLUTspecialCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutSpecialUpFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->specialUp = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}


void APIENTRY
glutMouseWheelFunc (GLUTmouseWheelCB func)
{
#if defined(DEBUG_AROSMESAGLUT) && defined(DEBUG_AROSMESAGLUTFUNCS)
D(bug("[AMGLUT] In glutMouseWheelFunc()\n"));
#endif
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));
   __glutTask->AMGLUTTN_WindowCurrent->mouseWheel = func;
   __glut_modify_windowIDCMP(__glutTask, __glutTask->AMGLUTTN_WindowCurrent->amglutwin_num);
}
