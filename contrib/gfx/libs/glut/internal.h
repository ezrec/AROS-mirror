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

 
#ifndef INTERNAL_H_included
#define INTERNAL_H_included

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <devices/inputevent.h>
#include <graphics/rpattr.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>

#if defined(__cplusplus) || defined(c_plusplus)
#define class c_class
#endif

#include <GL/gla.h>
#include <GL/gl.h>

#define MAX_WINDOWS     15 /* Obsolete */
#define MAX_TIMER_CB    8  /* Obsolete */
#define RESERVED_COLORS 0  /* Unused */

#if !defined(AROSSHAREDGLUT)
#include <GL/glut.h>

#if !defined(AROSMESAGLUT_HAVE_CALLBACKS)
#define AROSMESAGLUT_HAVE_CALLBACKS
/* GLUT  function types */
typedef void (GLUTCALLBACK *GLUTdisplayCB) (void);
typedef void (GLUTCALLBACK *GLUTreshapeCB) (int, int);
typedef void (GLUTCALLBACK *GLUTkeyboardCB) (unsigned char, int, int);
typedef void (GLUTCALLBACK *GLUTmouseCB) (int, int, int, int);
typedef void (GLUTCALLBACK *GLUTmotionCB) (int, int);
typedef void (GLUTCALLBACK *GLUTpassiveCB) (int, int);
typedef void (GLUTCALLBACK *GLUTentryCB) (int);
typedef void (GLUTCALLBACK *GLUTvisibilityCB) (int);
typedef void (GLUTCALLBACK *GLUTwindowStatusCB) (int);
typedef void (GLUTCALLBACK *GLUTidleCB) (void);
typedef void (GLUTCALLBACK *GLUTtimerCB) (int);
typedef void (GLUTCALLBACK *GLUTmenuStateCB) (int);  /* DEPRECATED. */
typedef void (GLUTCALLBACK *GLUTmenuStatusCB) (int, int, int);
typedef void (GLUTCALLBACK *GLUTselectCB) (int);
typedef void (GLUTCALLBACK *GLUTspecialCB) (int, int, int);
typedef void (GLUTCALLBACK *GLUTspaceMotionCB) (int, int, int);
typedef void (GLUTCALLBACK *GLUTspaceRotateCB) (int, int, int);
typedef void (GLUTCALLBACK *GLUTspaceButtonCB) (int, int);
typedef void (GLUTCALLBACK *GLUTdialsCB) (int, int);
typedef void (GLUTCALLBACK *GLUTbuttonBoxCB) (int, int);
typedef void (GLUTCALLBACK *GLUTtabletMotionCB) (int, int);
typedef void (GLUTCALLBACK *GLUTtabletButtonCB) (int, int, int, int);
typedef void (GLUTCALLBACK *GLUTjoystickCB) (unsigned int, int, int, int);

typedef void (GLUTCALLBACK *GLUTdestroyCB) (void);
typedef void (GLUTCALLBACK *GLUTmouseWheelCB) (int, int, int, int);
typedef void (GLUTCALLBACK *GLUTmenuDestroyCB) (void);
#endif
#else
#include <proto/glut.h>
#endif

typedef struct {
   GLuint bpp, alpha;
   GLuint depth, stencil;
   GLuint accum;

   GLint geometry[2];
   GLuint refresh;

   GLint flags;
} GLUTvisual;

typedef struct {
   GLint x, y;
   GLint width, height;
   GLuint mode;
} GLUTdefault;

typedef struct {
   void (*func) (int);
   int value;
   int time;
} GLUTSShotCB;

#define GLUT_MSG_WINDOWVIRGIN ~0
#define GLUT_MSG_WINDOWACTIVE ~1

typedef struct GLUTwindow {
/***** Window ID and AROS data */
   struct Node            amglutwin_Node;
   int                    amglutwin_num;                         /* window id */
   ULONG                  amglutwin_TYPE;
#define AMGLUT_WIN_NORMAL  0x02
#define AMGLUT_WIN_SUBWIN  0x03
   struct Task            *amglutwin_Task;
   struct Screen          *amglutwin_Screen;                     /* Points to real AROS Screen */

   struct Window          *amglutwin_Window;                     /* Points to real AROS window */
   ULONG                  amglutwin_WindowIDCMPMask;             /* */
   struct IntuiMessage    *amglutwin_InternalMessage;

   struct Menu            *amglutwin_WindowMenuStrip;            /* */   
   APTR                   amglutwin_WindowGlutMenu;              /* */


   GLenum                 amglutwin_type;
   GLenum                 amglutwin_curLevel;
   GLboolean              amglutwin_FlagReDisplay;
   GLboolean              amglutwin_FlagIdle;

/***** GL Related items */
   GLAContext             amglutwin_context;

/***** GLUT settable or visible window state. */
   GLboolean              amglutwin_Virgin;
   UWORD                  amglutwin_LastKey;
   UWORD                  amglutwin_LastSpecialKey;
   int                    amglutwin_LastModifier;
   int                    amglutwin_mouseX;
   int                    amglutwin_mouseY;
   int                    amglutwin_xpos;
   int                    amglutwin_ypos;
   int                    amglutwin_width;        /* window width in pixels */
   int                    amglutwin_height;       /* window height in pixels */

/***** Old Vars from dosglut (remove?)*/
   int                    show_mouse;

/***** Per-window callbacks. */
   GLUTdisplayCB          display;      /* redraw */
   GLUTreshapeCB          reshape;      /* resize (width,height) */
   GLUTmouseCB            mouse;        /* mouse (button,state,x,y) */
   GLUTmotionCB           motion;       /* motion (x,y) */
   GLUTpassiveCB          passive;      /* passive motion (x,y) */
   GLUTentryCB            entry;        /* window entry/exit (state) */
   GLUTkeyboardCB         keyboard;     /* keyboard (ASCII,x,y) */
   GLUTkeyboardCB         keyboardUp;   /* keyboard up (ASCII,x,y) */
   GLUTwindowStatusCB     windowStatus; /* window status */
   GLUTvisibilityCB       visibility;   /* visibility */
   GLUTspecialCB          special;      /* special key */
   GLUTspecialCB          specialUp;    /* special up key */
   GLUTbuttonBoxCB        buttonBox;    /* button box */
   GLUTdialsCB            dials;        /* dials */
   GLUTspaceMotionCB      spaceMotion;  /* Spaceball motion */
   GLUTspaceRotateCB      spaceRotate;  /* Spaceball rotate */
   GLUTspaceButtonCB      spaceButton;  /* Spaceball button */
   GLUTtabletMotionCB     tabletMotion; /* tablet motion */
   GLUTtabletButtonCB     tabletButton; /* tablet button */
   GLUTjoystickCB         joystick;     /* joystick */

   GLUTdestroyCB          destroy;      /* destroy */
   GLUTmouseWheelCB       mouseWheel;   /* mouse wheel */

   /* specific data */
   void *data;
} GLUTwindow;

typedef struct _glut_menu_node
{
  struct Node    _gmn_Node;
  int            _gmn_ID;
  GLUTselectCB   _gmn_CB;
  int            _gmn_Entry_Count;
  struct List    _gmn_Entries;
}GLUTmenu;

typedef struct _glut_menu_entries
{
  struct Node    _gme_Node;
  ULONG          _gme_type;
#define  GLUT_MENU_ENTRY_ITEM    0x01
#define  GLUT_MENU_SUBMENU_ITEM  0x02
  char           *_gme_label;
  SIPTR           _gme_value;
}GLUTmenuentry;

typedef struct {
   int width, height;
   int xorig, yorig;
   int xmove;
   const unsigned char *bitmap;
} GLUTBitmapChar;

typedef struct {
   const char *name;
   int height;
   int num;
   const GLUTBitmapChar *const *table;
} GLUTBitmapFont;

typedef struct {
   const GLfloat x, y;
} GLUTStrokeVertex;

typedef struct {
   const unsigned num;
   const GLUTStrokeVertex *vertex;
} GLUTStrokeStrip;

typedef struct {
   const GLfloat right;
   const unsigned num;
   const GLUTStrokeStrip *strip;
} GLUTStrokeChar;

typedef struct {
   const char *name;
   const unsigned num;
   const GLUTStrokeChar *const *table;
   const GLfloat height;
   const GLfloat descent;
} GLUTStrokeFont;

extern GLUTvisual        _glut_visual;
extern GLUTdefault       _glut_default;

extern GLuint            _glut_fps;
extern GLUTidleCB        _glut_idle_func;
extern GLUTmenuStatusCB  _glut_menu_status_func;
extern GLUTSShotCB       _glut_timer_cb[];

struct AROSMesaGLUT_TaskNode
{
  struct Node       AMGLUTTN_Node;
  struct Task       *AMGLUTTN_Task;
  struct MsgPort    *AMGLUTTN_MsgPort;
  char              *AMGLUTTN_ProgramName;
  GLUTwindow        *AMGLUTTN_WindowCurrent;
  IPTR              AMGLUTTN_WindowCount;
  struct List       AMGLUTTN_WindowList;
  ULONG             AMGLUTTN_TimeEventCount;
  GLboolean         AMGLUTTN_TaskIdle;
};

#if !defined(_AMGLUT_WINDOW_C)
/* Protos for arosmesaglut/window.c */
extern ULONG             __glut_modify_windowIDCMP(struct AROSMesaGLUT_TaskNode *__glutTask, int win);
#endif
#if !defined(_AMGLUT_LOOP_C)
/* Protos for arosmesaglut/loop.c */
extern ULONG             _glut_TimeEvent_Count;
#endif

extern int _glut_mouse;                  /* number of buttons, if mouse installed */
extern int _glut_mouse_x, _glut_mouse_y; /* mouse coords, relative to current win */

extern void _glut_mouse_init (void);
extern void _glut_fatal(char *format,...);
extern void * _glut_font (void * font);

#endif
