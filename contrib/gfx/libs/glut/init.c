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


#include <aros/symbolsets.h>
#include <aros/startup.h>
#include <exec/lists.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GL/glut.h"
#include "internal.h"
#include "arosmesaglut_intern.h"

#include <aros/debug.h>

#if !defined(AROSSHAREDGLUT)
struct AROSMesaGLUTBase *AMGLInternalBase;
#endif

GLUTvisual _glut_visual = {
   16, 8, 16, 8, 16, /* bpp, alpha, depth, stencil, accum */

   { 0, 0 }, 0,      /* geometry */

   0                 /* flags */
};

GLUTdefault _glut_default = {
   0, 0,             /* glutInitWindowPosition */
   300, 300,         /* glutInitWindowSize */
   0                 /* glutInitDisplayMode */
};

GLuint _glut_fps = 0;

static char *init_string;

struct AROSMesaGLUT_TaskNode *
_glut_findtask(struct Task *thisTask)
{
  struct AROSMesaGLUT_TaskNode *foundNode = NULL;

  ForeachNode(&AMGLInternalBase->_glutPertasklist, foundNode)
  {
    if (foundNode->AMGLUTTN_Task == thisTask) return foundNode;
  }
  return NULL;
}

void
_glut_fatal (char *format,...)
{
  D(bug("[AMGLUT] In _glut_fatal()\n"));

  va_list args;

   struct Task           *task_Current = FindTask(NULL);
   struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(task_Current);

  va_start(args, format);
  fprintf(stderr, "GLUT: Fatal Error in %s: ",
    __glutTask->AMGLUTTN_ProgramName ? __glutTask->AMGLUTTN_ProgramName : "(unamed)");
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
  exit(1);
}


/* strdup is actually not a standard ANSI C or POSIX routine
 * so implement a private one for GLUT.
 */
static char *
_glut_strdup (const char *string)
{
   if (string != NULL) {
      int len = strlen(string) + 1;
      char *p = malloc(len);
      if (p != NULL) {
         return strcpy(p, string);
      }
   }
   return NULL;
}

#if !defined(AROSSHAREDGLUT)
extern int AMGLInit(struct AROSMesaGLUTBase * lh);
extern int AMGLOpen(struct AROSMesaGLUTBase * lh);

static void initAROSMesaGLUTBase()
{
    /* Trick to build linklib from module sources */
    AMGLInternalBase = AllocVec(sizeof(struct AROSMesaGLUTBase), MEMF_CLEAR|MEMF_PUBLIC);
    D(bug("[AMGLUT] %s: InternalBase @ 0x%p\n", __PRETTY_FUNCTION__, AMGLInternalBase));

    AMGLInit(AMGLInternalBase);
    AMGLOpen(AMGLInternalBase);
}
#endif

void APIENTRY
glutInit (int *argc, char **argv)
{
   char *str;
   const char *env;

   struct Task           *task_Current = FindTask(NULL);
   struct AROSMesaGLUT_TaskNode *__glutTask = NULL;
   
#if !defined(AROSSHAREDGLUT)   
   initAROSMesaGLUTBase();
#endif

   __glutTask = _glut_findtask(task_Current);


   D(bug("[AMGLUT] In glutInit(Task @ %x)\n", task_Current));


   if (__glutTask == NULL)
   {
      D(bug("[AMGLUT] glutInit: No internal task record found!\n"));

      return;
   }

   if ((env = getenv("DMESA_GLUT_BPP")) != NULL) {
      _glut_visual.bpp = atoi(env);
   }
   if ((env = getenv("DMESA_GLUT_ALPHA")) != NULL) {
      _glut_visual.alpha = atoi(env);
   }
   if ((env = getenv("DMESA_GLUT_DEPTH")) != NULL) {
      _glut_visual.depth = atoi(env);
   }
   if ((env = getenv("DMESA_GLUT_STENCIL")) != NULL) {
      _glut_visual.stencil = atoi(env);
   }
   if ((env = getenv("DMESA_GLUT_ACCUM")) != NULL) {
      _glut_visual.accum = atoi(env);
   }
   if ((env = getenv("DMESA_GLUT_REFRESH")) != NULL) {
      _glut_visual.refresh = atoi(env);
   }

   /* Determine program name. */
   str = strrchr(argv[0], '/');
   if (str == NULL) {
      str = argv[0];
   } else {
      str++;
   }
   __glutTask->AMGLUTTN_ProgramName = _glut_strdup(str);

   /* check if GLUT_FPS env var is set */
   if ((env = getenv("GLUT_FPS")) != NULL) {
      if ((_glut_fps = atoi(env)) <= 0) {
         _glut_fps = 5000; /* 5000 milliseconds */
      }
   }

   /* Initialize timer */
   glutGet(GLUT_ELAPSED_TIME);
}


void APIENTRY
glutInitDisplayMode (unsigned int mode)
{
   D(bug("[AMGLUT] In glutInitDisplayMode()\n"));

   _glut_default.mode = mode;
}


void APIENTRY
glutInitWindowPosition (int x, int y)
{
   D(bug("[AMGLUT] In glutInitWindowPosition(x:%d, y:%d)\n", x, y));

   _glut_default.x = x;
   _glut_default.y = y;
}


void APIENTRY
glutInitWindowSize (int width, int height)
{
   D(bug("[AMGLUT] In glutInitWindowSize(width:%d, height:%d)\n", width, height));

   _glut_default.width = width;
   _glut_default.height = height;
}


void APIENTRY
glutInitDisplayString (const char *string)
{
   D(bug("[AMGLUT] In glutInitDisplayString()\n"));

   init_string = _glut_strdup(string);
}


void APIENTRY
glutSetOption (GLenum pname, int value)
{
   D(bug("[AMGLUT] In glutSetOption()\n"));

   switch (pname) {
      case GLUT_INIT_WINDOW_X:
         _glut_default.x = value;
         break;
      case GLUT_INIT_WINDOW_Y:
         _glut_default.y = value;
         break;
   }
}


void APIENTRY
glutForceJoystickFunc (void)
{
   D(bug("[AMGLUT] In glutForceJoystickFunc() #\n"));
}


void APIENTRY
glutIgnoreKeyRepeat (int ignore)
{
   D(bug("[AMGLUT] In glutIgnoreKeyRepeat() #\n"));
}


void APIENTRY
glutSetKeyRepeat (int repeatMode)
{
   D(bug("[AMGLUT] In glutSetKeyRepeat() #\n"));
}


void APIENTRY
glutVideoPan (int x, int y, int w, int h)
{
   D(bug("[AMGLUT] In glutVideoPan() #\n"));
}


int APIENTRY
glutVideoResizeGet( GLenum eWhat )
{
   D(bug("[AMGLUT] In glutVideoResizeGet() #\n"));

   return 0;
}


void APIENTRY
glutSetupVideoResizing (void)
{
   D(bug("[AMGLUT] In glutSetupVideoResizing() #\n"));
}


void APIENTRY
glutStopVideoResizing (void)
{
   D(bug("[AMGLUT] In glutStopVideoResizing() #\n"));
}


void APIENTRY
glutVideoResize (int x, int y, int w, int h)
{
   D(bug("[AMGLUT] In glutVideoResize() #\n"));
}

static void
__glut_exit_atexit (void)
{
    D(bug("[AMGLUT] In __glut_exit_atexit() *#*#\n"));

    struct AROSMesaGLUT_TaskNode *__glutTask = _glut_findtask(FindTask(NULL));

    if (__glutTask->AMGLUTTN_MsgPort != NULL)
    { 
        struct Node * tail = GetTail(&__glutTask->AMGLUTTN_WindowList);
        
        while (tail != NULL)
        {
            /* This call modifies the __glutTask->AMGLUTTN_WindowList. ForeachNode cannot be used */
            glutDestroyWindow(((struct GLUTwindow *)tail)->amglutwin_num);

            tail = GetTail(&__glutTask->AMGLUTTN_WindowList);
        }
        
        DeleteMsgPort(__glutTask->AMGLUTTN_MsgPort);
    }
}

ADD2EXIT(__glut_exit_atexit, 10);
