/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include "glx_hostlib.h"
#include "arosmesaapim.h"
#include <aros/debug.h>

#define HOSTGL_PRE                                          \
    HostGL_Lock();                                          \
    HostGL_SetGlobalGLXContext();

#define HOSTGL_POST                                         \
    HostGL_UnLock();

void mglClearIndex (GLfloat c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearIndex", FindTask(NULL)));
    GLCALL(glClearIndex, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearColor", FindTask(NULL)));
    GLCALL(glClearColor, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClear (GLbitfield mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClear", FindTask(NULL)));
    GLCALL(glClear, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexMask (GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexMask", FindTask(NULL)));
    GLCALL(glIndexMask, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorMask", FindTask(NULL)));
    GLCALL(glColorMask, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAlphaFunc (GLenum func, GLclampf ref)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAlphaFunc", FindTask(NULL)));
    GLCALL(glAlphaFunc, func, ref);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFunc (GLenum sfactor, GLenum dfactor)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFunc", FindTask(NULL)));
    GLCALL(glBlendFunc, sfactor, dfactor);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLogicOp (GLenum opcode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLogicOp", FindTask(NULL)));
    GLCALL(glLogicOp, opcode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCullFace (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCullFace", FindTask(NULL)));
    GLCALL(glCullFace, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFrontFace (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFrontFace", FindTask(NULL)));
    GLCALL(glFrontFace, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointSize (GLfloat size)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointSize", FindTask(NULL)));
    GLCALL(glPointSize, size);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLineWidth (GLfloat width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLineWidth", FindTask(NULL)));
    GLCALL(glLineWidth, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLineStipple (GLint factor, GLushort pattern)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLineStipple", FindTask(NULL)));
    GLCALL(glLineStipple, factor, pattern);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPolygonMode (GLenum face, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPolygonMode", FindTask(NULL)));
    GLCALL(glPolygonMode, face, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPolygonOffset (GLfloat factor, GLfloat units)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPolygonOffset", FindTask(NULL)));
    GLCALL(glPolygonOffset, factor, units);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPolygonStipple (const GLubyte * mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPolygonStipple", FindTask(NULL)));
    GLCALL(glPolygonStipple, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPolygonStipple (GLubyte * mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPolygonStipple", FindTask(NULL)));
    GLCALL(glGetPolygonStipple, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEdgeFlag (GLboolean flag)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEdgeFlag", FindTask(NULL)));
    GLCALL(glEdgeFlag, flag);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEdgeFlagv (const GLboolean * flag)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEdgeFlagv", FindTask(NULL)));
    GLCALL(glEdgeFlagv, flag);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glScissor", FindTask(NULL)));
    GLCALL(glScissor, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClipPlane (GLenum plane, const GLdouble * equation)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClipPlane", FindTask(NULL)));
    GLCALL(glClipPlane, plane, equation);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetClipPlane (GLenum plane, GLdouble * equation)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetClipPlane", FindTask(NULL)));
    GLCALL(glGetClipPlane, plane, equation);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawBuffer (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawBuffer", FindTask(NULL)));
    GLCALL(glDrawBuffer, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglReadBuffer (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glReadBuffer", FindTask(NULL)));
    GLCALL(glReadBuffer, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEnable (GLenum cap)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnable", FindTask(NULL)));
    GLCALL(glEnable, cap);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisable (GLenum cap)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisable", FindTask(NULL)));
    GLCALL(glDisable, cap);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsEnabled (GLenum cap)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsEnabled", FindTask(NULL)));
    _ret = GLCALL(glIsEnabled, cap);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglEnableClientState (GLenum cap)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnableClientState", FindTask(NULL)));
    GLCALL(glEnableClientState, cap);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisableClientState (GLenum cap)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisableClientState", FindTask(NULL)));
    GLCALL(glDisableClientState, cap);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBooleanv (GLenum pname, GLboolean * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBooleanv", FindTask(NULL)));
    GLCALL(glGetBooleanv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetDoublev (GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetDoublev", FindTask(NULL)));
    GLCALL(glGetDoublev, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetFloatv (GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFloatv", FindTask(NULL)));
    GLCALL(glGetFloatv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetIntegerv (GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetIntegerv", FindTask(NULL)));
    GLCALL(glGetIntegerv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPushAttrib (GLbitfield mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPushAttrib", FindTask(NULL)));
    GLCALL(glPushAttrib, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPopAttrib ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPopAttrib", FindTask(NULL)));
    GLCALL(glPopAttrib);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPushClientAttrib (GLbitfield mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPushClientAttrib", FindTask(NULL)));
    GLCALL(glPushClientAttrib, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPopClientAttrib ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPopClientAttrib", FindTask(NULL)));
    GLCALL(glPopClientAttrib);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglRenderMode (GLenum mode)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRenderMode", FindTask(NULL)));
    _ret = GLCALL(glRenderMode, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLenum mglGetError ()
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetError", FindTask(NULL)));
    _ret = GLCALL(glGetError);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

const GLubyte * mglGetString (GLenum name)
{
    const GLubyte * _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetString", FindTask(NULL)));
    _ret = GLCALL(glGetString, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglFinish ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFinish", FindTask(NULL)));
    GLCALL(glFinish);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFlush ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFlush", FindTask(NULL)));
    GLCALL(glFlush);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglHint (GLenum target, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glHint", FindTask(NULL)));
    GLCALL(glHint, target, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearDepth (GLclampd depth)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearDepth", FindTask(NULL)));
    GLCALL(glClearDepth, depth);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDepthFunc (GLenum func)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDepthFunc", FindTask(NULL)));
    GLCALL(glDepthFunc, func);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDepthMask (GLboolean flag)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDepthMask", FindTask(NULL)));
    GLCALL(glDepthMask, flag);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDepthRange (GLclampd near_val, GLclampd far_val)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDepthRange", FindTask(NULL)));
    GLCALL(glDepthRange, near_val, far_val);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearAccum", FindTask(NULL)));
    GLCALL(glClearAccum, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAccum (GLenum op, GLfloat value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAccum", FindTask(NULL)));
    GLCALL(glAccum, op, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMatrixMode (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMatrixMode", FindTask(NULL)));
    GLCALL(glMatrixMode, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glOrtho", FindTask(NULL)));
    GLCALL(glOrtho, left, right, bottom, top, near_val, far_val);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFrustum", FindTask(NULL)));
    GLCALL(glFrustum, left, right, bottom, top, near_val, far_val);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glViewport", FindTask(NULL)));
    GLCALL(glViewport, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPushMatrix ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPushMatrix", FindTask(NULL)));
    GLCALL(glPushMatrix);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPopMatrix ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPopMatrix", FindTask(NULL)));
    GLCALL(glPopMatrix);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadIdentity ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadIdentity", FindTask(NULL)));
    GLCALL(glLoadIdentity);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadMatrixd", FindTask(NULL)));
    GLCALL(glLoadMatrixd, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadMatrixf", FindTask(NULL)));
    GLCALL(glLoadMatrixf, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultMatrixd", FindTask(NULL)));
    GLCALL(glMultMatrixd, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultMatrixf", FindTask(NULL)));
    GLCALL(glMultMatrixf, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRotated", FindTask(NULL)));
    GLCALL(glRotated, angle, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRotatef", FindTask(NULL)));
    GLCALL(glRotatef, angle, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglScaled (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glScaled", FindTask(NULL)));
    GLCALL(glScaled, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglScalef (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glScalef", FindTask(NULL)));
    GLCALL(glScalef, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTranslated (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTranslated", FindTask(NULL)));
    GLCALL(glTranslated, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTranslatef", FindTask(NULL)));
    GLCALL(glTranslatef, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsList (GLuint list)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsList", FindTask(NULL)));
    _ret = GLCALL(glIsList, list);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglDeleteLists (GLuint list, GLsizei range)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteLists", FindTask(NULL)));
    GLCALL(glDeleteLists, list, range);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLuint mglGenLists (GLsizei range)
{
    GLuint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenLists", FindTask(NULL)));
    _ret = GLCALL(glGenLists, range);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglNewList (GLuint list, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNewList", FindTask(NULL)));
    GLCALL(glNewList, list, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndList ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndList", FindTask(NULL)));
    GLCALL(glEndList);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCallList (GLuint list)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCallList", FindTask(NULL)));
    GLCALL(glCallList, list);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCallLists (GLsizei n, GLenum type, const GLvoid * lists)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCallLists", FindTask(NULL)));
    GLCALL(glCallLists, n, type, lists);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglListBase (GLuint base)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glListBase", FindTask(NULL)));
    GLCALL(glListBase, base);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBegin (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBegin", FindTask(NULL)));
    GLCALL(glBegin, mode);
    D(bug("...exit\n"));
    /* glBegin/glEnd must be atomic */
}

void mglEnd ()
{
    /* glBegin/glEnd must be atomic */
    D(bug("TASK: 0x%x, glEnd", FindTask(NULL)));
    GLCALL(glEnd);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2d", FindTask(NULL)));
    GLCALL(glVertex2d, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2f", FindTask(NULL)));
    GLCALL(glVertex2f, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2i (GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2i", FindTask(NULL)));
    GLCALL(glVertex2i, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2s", FindTask(NULL)));
    GLCALL(glVertex2s, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3d", FindTask(NULL)));
    GLCALL(glVertex3d, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3f", FindTask(NULL)));
    GLCALL(glVertex3f, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3i", FindTask(NULL)));
    GLCALL(glVertex3i, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3s", FindTask(NULL)));
    GLCALL(glVertex3s, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4d", FindTask(NULL)));
    GLCALL(glVertex4d, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4f", FindTask(NULL)));
    GLCALL(glVertex4f, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4i (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4i", FindTask(NULL)));
    GLCALL(glVertex4i, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4s", FindTask(NULL)));
    GLCALL(glVertex4s, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2dv", FindTask(NULL)));
    GLCALL(glVertex2dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2fv", FindTask(NULL)));
    GLCALL(glVertex2fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2iv", FindTask(NULL)));
    GLCALL(glVertex2iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex2sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex2sv", FindTask(NULL)));
    GLCALL(glVertex2sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3dv", FindTask(NULL)));
    GLCALL(glVertex3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3fv", FindTask(NULL)));
    GLCALL(glVertex3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3iv", FindTask(NULL)));
    GLCALL(glVertex3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex3sv", FindTask(NULL)));
    GLCALL(glVertex3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4dv", FindTask(NULL)));
    GLCALL(glVertex4dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4fv", FindTask(NULL)));
    GLCALL(glVertex4fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4iv", FindTask(NULL)));
    GLCALL(glVertex4iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertex4sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertex4sv", FindTask(NULL)));
    GLCALL(glVertex4sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3b", FindTask(NULL)));
    GLCALL(glNormal3b, nx, ny, nz);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3d", FindTask(NULL)));
    GLCALL(glNormal3d, nx, ny, nz);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3f", FindTask(NULL)));
    GLCALL(glNormal3f, nx, ny, nz);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3i (GLint nx, GLint ny, GLint nz)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3i", FindTask(NULL)));
    GLCALL(glNormal3i, nx, ny, nz);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3s", FindTask(NULL)));
    GLCALL(glNormal3s, nx, ny, nz);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3bv (const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3bv", FindTask(NULL)));
    GLCALL(glNormal3bv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3dv", FindTask(NULL)));
    GLCALL(glNormal3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3fv", FindTask(NULL)));
    GLCALL(glNormal3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3iv", FindTask(NULL)));
    GLCALL(glNormal3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormal3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormal3sv", FindTask(NULL)));
    GLCALL(glNormal3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexd (GLdouble c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexd", FindTask(NULL)));
    GLCALL(glIndexd, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexf (GLfloat c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexf", FindTask(NULL)));
    GLCALL(glIndexf, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexi (GLint c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexi", FindTask(NULL)));
    GLCALL(glIndexi, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexs (GLshort c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexs", FindTask(NULL)));
    GLCALL(glIndexs, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexub (GLubyte c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexub", FindTask(NULL)));
    GLCALL(glIndexub, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexdv (const GLdouble * c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexdv", FindTask(NULL)));
    GLCALL(glIndexdv, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexfv (const GLfloat * c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexfv", FindTask(NULL)));
    GLCALL(glIndexfv, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexiv (const GLint * c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexiv", FindTask(NULL)));
    GLCALL(glIndexiv, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexsv (const GLshort * c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexsv", FindTask(NULL)));
    GLCALL(glIndexsv, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexubv (const GLubyte * c)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexubv", FindTask(NULL)));
    GLCALL(glIndexubv, c);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3b", FindTask(NULL)));
    GLCALL(glColor3b, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3d", FindTask(NULL)));
    GLCALL(glColor3d, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3f", FindTask(NULL)));
    GLCALL(glColor3f, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3i (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3i", FindTask(NULL)));
    GLCALL(glColor3i, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3s (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3s", FindTask(NULL)));
    GLCALL(glColor3s, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3ub", FindTask(NULL)));
    GLCALL(glColor3ub, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3ui (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3ui", FindTask(NULL)));
    GLCALL(glColor3ui, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3us (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3us", FindTask(NULL)));
    GLCALL(glColor3us, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4b", FindTask(NULL)));
    GLCALL(glColor4b, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4d", FindTask(NULL)));
    GLCALL(glColor4d, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4f", FindTask(NULL)));
    GLCALL(glColor4f, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4i", FindTask(NULL)));
    GLCALL(glColor4i, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4s", FindTask(NULL)));
    GLCALL(glColor4s, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4ub", FindTask(NULL)));
    GLCALL(glColor4ub, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4ui", FindTask(NULL)));
    GLCALL(glColor4ui, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4us", FindTask(NULL)));
    GLCALL(glColor4us, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3bv (const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3bv", FindTask(NULL)));
    GLCALL(glColor3bv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3dv", FindTask(NULL)));
    GLCALL(glColor3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3fv", FindTask(NULL)));
    GLCALL(glColor3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3iv", FindTask(NULL)));
    GLCALL(glColor3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3sv", FindTask(NULL)));
    GLCALL(glColor3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3ubv (const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3ubv", FindTask(NULL)));
    GLCALL(glColor3ubv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3uiv (const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3uiv", FindTask(NULL)));
    GLCALL(glColor3uiv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor3usv (const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor3usv", FindTask(NULL)));
    GLCALL(glColor3usv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4bv (const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4bv", FindTask(NULL)));
    GLCALL(glColor4bv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4dv", FindTask(NULL)));
    GLCALL(glColor4dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4fv", FindTask(NULL)));
    GLCALL(glColor4fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4iv", FindTask(NULL)));
    GLCALL(glColor4iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4sv", FindTask(NULL)));
    GLCALL(glColor4sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4ubv (const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4ubv", FindTask(NULL)));
    GLCALL(glColor4ubv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4uiv (const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4uiv", FindTask(NULL)));
    GLCALL(glColor4uiv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColor4usv (const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColor4usv", FindTask(NULL)));
    GLCALL(glColor4usv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1d (GLdouble s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1d", FindTask(NULL)));
    GLCALL(glTexCoord1d, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1f (GLfloat s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1f", FindTask(NULL)));
    GLCALL(glTexCoord1f, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1i (GLint s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1i", FindTask(NULL)));
    GLCALL(glTexCoord1i, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1s (GLshort s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1s", FindTask(NULL)));
    GLCALL(glTexCoord1s, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2d (GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2d", FindTask(NULL)));
    GLCALL(glTexCoord2d, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2f (GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2f", FindTask(NULL)));
    GLCALL(glTexCoord2f, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2i (GLint s, GLint t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2i", FindTask(NULL)));
    GLCALL(glTexCoord2i, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2s (GLshort s, GLshort t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2s", FindTask(NULL)));
    GLCALL(glTexCoord2s, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3d", FindTask(NULL)));
    GLCALL(glTexCoord3d, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3f", FindTask(NULL)));
    GLCALL(glTexCoord3f, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3i (GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3i", FindTask(NULL)));
    GLCALL(glTexCoord3i, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3s (GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3s", FindTask(NULL)));
    GLCALL(glTexCoord3s, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4d", FindTask(NULL)));
    GLCALL(glTexCoord4d, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4f", FindTask(NULL)));
    GLCALL(glTexCoord4f, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4i", FindTask(NULL)));
    GLCALL(glTexCoord4i, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4s", FindTask(NULL)));
    GLCALL(glTexCoord4s, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1dv", FindTask(NULL)));
    GLCALL(glTexCoord1dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1fv", FindTask(NULL)));
    GLCALL(glTexCoord1fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1iv", FindTask(NULL)));
    GLCALL(glTexCoord1iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord1sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord1sv", FindTask(NULL)));
    GLCALL(glTexCoord1sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2dv", FindTask(NULL)));
    GLCALL(glTexCoord2dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2fv", FindTask(NULL)));
    GLCALL(glTexCoord2fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2iv", FindTask(NULL)));
    GLCALL(glTexCoord2iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord2sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord2sv", FindTask(NULL)));
    GLCALL(glTexCoord2sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3dv", FindTask(NULL)));
    GLCALL(glTexCoord3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3fv", FindTask(NULL)));
    GLCALL(glTexCoord3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3iv", FindTask(NULL)));
    GLCALL(glTexCoord3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord3sv", FindTask(NULL)));
    GLCALL(glTexCoord3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4dv", FindTask(NULL)));
    GLCALL(glTexCoord4dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4fv", FindTask(NULL)));
    GLCALL(glTexCoord4fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4iv", FindTask(NULL)));
    GLCALL(glTexCoord4iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoord4sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoord4sv", FindTask(NULL)));
    GLCALL(glTexCoord4sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2d", FindTask(NULL)));
    GLCALL(glRasterPos2d, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2f", FindTask(NULL)));
    GLCALL(glRasterPos2f, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2i (GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2i", FindTask(NULL)));
    GLCALL(glRasterPos2i, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2s", FindTask(NULL)));
    GLCALL(glRasterPos2s, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3d", FindTask(NULL)));
    GLCALL(glRasterPos3d, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3f", FindTask(NULL)));
    GLCALL(glRasterPos3f, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3i", FindTask(NULL)));
    GLCALL(glRasterPos3i, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3s", FindTask(NULL)));
    GLCALL(glRasterPos3s, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4d", FindTask(NULL)));
    GLCALL(glRasterPos4d, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4f", FindTask(NULL)));
    GLCALL(glRasterPos4f, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4i", FindTask(NULL)));
    GLCALL(glRasterPos4i, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4s", FindTask(NULL)));
    GLCALL(glRasterPos4s, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2dv", FindTask(NULL)));
    GLCALL(glRasterPos2dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2fv", FindTask(NULL)));
    GLCALL(glRasterPos2fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2iv", FindTask(NULL)));
    GLCALL(glRasterPos2iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos2sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos2sv", FindTask(NULL)));
    GLCALL(glRasterPos2sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3dv", FindTask(NULL)));
    GLCALL(glRasterPos3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3fv", FindTask(NULL)));
    GLCALL(glRasterPos3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3iv", FindTask(NULL)));
    GLCALL(glRasterPos3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos3sv", FindTask(NULL)));
    GLCALL(glRasterPos3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4dv", FindTask(NULL)));
    GLCALL(glRasterPos4dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4fv", FindTask(NULL)));
    GLCALL(glRasterPos4fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4iv", FindTask(NULL)));
    GLCALL(glRasterPos4iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRasterPos4sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRasterPos4sv", FindTask(NULL)));
    GLCALL(glRasterPos4sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectd", FindTask(NULL)));
    GLCALL(glRectd, x1, y1, x2, y2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectf", FindTask(NULL)));
    GLCALL(glRectf, x1, y1, x2, y2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRecti", FindTask(NULL)));
    GLCALL(glRecti, x1, y1, x2, y2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRects", FindTask(NULL)));
    GLCALL(glRects, x1, y1, x2, y2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectdv (const GLdouble * v1, const GLdouble * v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectdv", FindTask(NULL)));
    GLCALL(glRectdv, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectfv (const GLfloat * v1, const GLfloat * v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectfv", FindTask(NULL)));
    GLCALL(glRectfv, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectiv (const GLint * v1, const GLint * v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectiv", FindTask(NULL)));
    GLCALL(glRectiv, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRectsv (const GLshort * v1, const GLshort * v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRectsv", FindTask(NULL)));
    GLCALL(glRectsv, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexPointer", FindTask(NULL)));
    GLCALL(glVertexPointer, size, type, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormalPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormalPointer", FindTask(NULL)));
    GLCALL(glNormalPointer, type, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorPointer", FindTask(NULL)));
    GLCALL(glColorPointer, size, type, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexPointer", FindTask(NULL)));
    GLCALL(glIndexPointer, type, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoordPointer", FindTask(NULL)));
    GLCALL(glTexCoordPointer, size, type, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEdgeFlagPointer (GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEdgeFlagPointer", FindTask(NULL)));
    GLCALL(glEdgeFlagPointer, stride, ptr);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPointerv (GLenum pname, GLvoid * * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPointerv", FindTask(NULL)));
    GLCALL(glGetPointerv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglArrayElement (GLint i)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glArrayElement", FindTask(NULL)));
    GLCALL(glArrayElement, i);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawArrays", FindTask(NULL)));
    GLCALL(glDrawArrays, mode, first, count);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElements", FindTask(NULL)));
    GLCALL(glDrawElements, mode, count, type, indices);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglInterleavedArrays (GLenum format, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glInterleavedArrays", FindTask(NULL)));
    GLCALL(glInterleavedArrays, format, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglShadeModel (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glShadeModel", FindTask(NULL)));
    GLCALL(glShadeModel, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightf (GLenum light, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightf", FindTask(NULL)));
    GLCALL(glLightf, light, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLighti (GLenum light, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLighti", FindTask(NULL)));
    GLCALL(glLighti, light, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightfv (GLenum light, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightfv", FindTask(NULL)));
    GLCALL(glLightfv, light, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightiv (GLenum light, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightiv", FindTask(NULL)));
    GLCALL(glLightiv, light, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetLightfv (GLenum light, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetLightfv", FindTask(NULL)));
    GLCALL(glGetLightfv, light, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetLightiv (GLenum light, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetLightiv", FindTask(NULL)));
    GLCALL(glGetLightiv, light, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightModelf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightModelf", FindTask(NULL)));
    GLCALL(glLightModelf, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightModeli (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightModeli", FindTask(NULL)));
    GLCALL(glLightModeli, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightModelfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightModelfv", FindTask(NULL)));
    GLCALL(glLightModelfv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLightModeliv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLightModeliv", FindTask(NULL)));
    GLCALL(glLightModeliv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMaterialf", FindTask(NULL)));
    GLCALL(glMaterialf, face, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMateriali (GLenum face, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMateriali", FindTask(NULL)));
    GLCALL(glMateriali, face, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMaterialfv (GLenum face, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMaterialfv", FindTask(NULL)));
    GLCALL(glMaterialfv, face, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMaterialiv (GLenum face, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMaterialiv", FindTask(NULL)));
    GLCALL(glMaterialiv, face, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMaterialfv (GLenum face, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMaterialfv", FindTask(NULL)));
    GLCALL(glGetMaterialfv, face, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMaterialiv (GLenum face, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMaterialiv", FindTask(NULL)));
    GLCALL(glGetMaterialiv, face, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorMaterial (GLenum face, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorMaterial", FindTask(NULL)));
    GLCALL(glColorMaterial, face, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelZoom", FindTask(NULL)));
    GLCALL(glPixelZoom, xfactor, yfactor);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelStoref (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelStoref", FindTask(NULL)));
    GLCALL(glPixelStoref, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelStorei (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelStorei", FindTask(NULL)));
    GLCALL(glPixelStorei, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTransferf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTransferf", FindTask(NULL)));
    GLCALL(glPixelTransferf, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTransferi (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTransferi", FindTask(NULL)));
    GLCALL(glPixelTransferi, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelMapfv", FindTask(NULL)));
    GLCALL(glPixelMapfv, map, mapsize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelMapuiv", FindTask(NULL)));
    GLCALL(glPixelMapuiv, map, mapsize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelMapusv (GLenum map, GLsizei mapsize, const GLushort * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelMapusv", FindTask(NULL)));
    GLCALL(glPixelMapusv, map, mapsize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPixelMapfv (GLenum map, GLfloat * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPixelMapfv", FindTask(NULL)));
    GLCALL(glGetPixelMapfv, map, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPixelMapuiv (GLenum map, GLuint * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPixelMapuiv", FindTask(NULL)));
    GLCALL(glGetPixelMapuiv, map, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPixelMapusv (GLenum map, GLushort * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPixelMapusv", FindTask(NULL)));
    GLCALL(glGetPixelMapusv, map, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBitmap", FindTask(NULL)));
    GLCALL(glBitmap, width, height, xorig, yorig, xmove, ymove, bitmap);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glReadPixels", FindTask(NULL)));
    GLCALL(glReadPixels, x, y, width, height, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawPixels", FindTask(NULL)));
    GLCALL(glDrawPixels, width, height, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyPixels", FindTask(NULL)));
    GLCALL(glCopyPixels, x, y, width, height, type);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilFunc", FindTask(NULL)));
    GLCALL(glStencilFunc, func, ref, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilMask (GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilMask", FindTask(NULL)));
    GLCALL(glStencilMask, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilOp", FindTask(NULL)));
    GLCALL(glStencilOp, fail, zfail, zpass);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearStencil (GLint s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearStencil", FindTask(NULL)));
    GLCALL(glClearStencil, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGend", FindTask(NULL)));
    GLCALL(glTexGend, coord, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGenf", FindTask(NULL)));
    GLCALL(glTexGenf, coord, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGeni (GLenum coord, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGeni", FindTask(NULL)));
    GLCALL(glTexGeni, coord, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGendv (GLenum coord, GLenum pname, const GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGendv", FindTask(NULL)));
    GLCALL(glTexGendv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGenfv (GLenum coord, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGenfv", FindTask(NULL)));
    GLCALL(glTexGenfv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexGeniv (GLenum coord, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexGeniv", FindTask(NULL)));
    GLCALL(glTexGeniv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexGendv (GLenum coord, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexGendv", FindTask(NULL)));
    GLCALL(glGetTexGendv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexGenfv (GLenum coord, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexGenfv", FindTask(NULL)));
    GLCALL(glGetTexGenfv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexGeniv (GLenum coord, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexGeniv", FindTask(NULL)));
    GLCALL(glGetTexGeniv, coord, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexEnvf", FindTask(NULL)));
    GLCALL(glTexEnvf, target, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexEnvi (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexEnvi", FindTask(NULL)));
    GLCALL(glTexEnvi, target, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexEnvfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexEnvfv", FindTask(NULL)));
    GLCALL(glTexEnvfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexEnviv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexEnviv", FindTask(NULL)));
    GLCALL(glTexEnviv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexEnvfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexEnvfv", FindTask(NULL)));
    GLCALL(glGetTexEnvfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexEnviv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexEnviv", FindTask(NULL)));
    GLCALL(glGetTexEnviv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterf", FindTask(NULL)));
    GLCALL(glTexParameterf, target, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameteri (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameteri", FindTask(NULL)));
    GLCALL(glTexParameteri, target, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterfv", FindTask(NULL)));
    GLCALL(glTexParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameteriv", FindTask(NULL)));
    GLCALL(glTexParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterfv", FindTask(NULL)));
    GLCALL(glGetTexParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameteriv", FindTask(NULL)));
    GLCALL(glGetTexParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexLevelParameterfv", FindTask(NULL)));
    GLCALL(glGetTexLevelParameterfv, target, level, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexLevelParameteriv", FindTask(NULL)));
    GLCALL(glGetTexLevelParameteriv, target, level, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexImage1D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexImage1D", FindTask(NULL)));
    GLCALL(glTexImage1D, target, level, internalFormat, width, border, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexImage2D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexImage2D", FindTask(NULL)));
    GLCALL(glTexImage2D, target, level, internalFormat, width, height, border, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexImage", FindTask(NULL)));
    GLCALL(glGetTexImage, target, level, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenTextures (GLsizei n, GLuint * textures)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenTextures", FindTask(NULL)));
    GLCALL(glGenTextures, n, textures);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteTextures (GLsizei n, const GLuint * textures)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteTextures", FindTask(NULL)));
    GLCALL(glDeleteTextures, n, textures);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindTexture (GLenum target, GLuint texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindTexture", FindTask(NULL)));
    GLCALL(glBindTexture, target, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPrioritizeTextures (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPrioritizeTextures", FindTask(NULL)));
    GLCALL(glPrioritizeTextures, n, textures, priorities);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglAreTexturesResident (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAreTexturesResident", FindTask(NULL)));
    _ret = GLCALL(glAreTexturesResident, n, textures, residences);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsTexture (GLuint texture)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsTexture", FindTask(NULL)));
    _ret = GLCALL(glIsTexture, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage1D", FindTask(NULL)));
    GLCALL(glTexSubImage1D, target, level, xoffset, width, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage2D", FindTask(NULL)));
    GLCALL(glTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexImage1D", FindTask(NULL)));
    GLCALL(glCopyTexImage1D, target, level, internalformat, x, y, width, border);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexImage2D", FindTask(NULL)));
    GLCALL(glCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage1D", FindTask(NULL)));
    GLCALL(glCopyTexSubImage1D, target, level, xoffset, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage2D", FindTask(NULL)));
    GLCALL(glCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMap1d", FindTask(NULL)));
    GLCALL(glMap1d, target, u1, u2, stride, order, points);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMap1f", FindTask(NULL)));
    GLCALL(glMap1f, target, u1, u2, stride, order, points);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMap2d", FindTask(NULL)));
    GLCALL(glMap2d, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMap2f", FindTask(NULL)));
    GLCALL(glMap2f, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMapdv (GLenum target, GLenum query, GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMapdv", FindTask(NULL)));
    GLCALL(glGetMapdv, target, query, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMapfv (GLenum target, GLenum query, GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMapfv", FindTask(NULL)));
    GLCALL(glGetMapfv, target, query, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMapiv (GLenum target, GLenum query, GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMapiv", FindTask(NULL)));
    GLCALL(glGetMapiv, target, query, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord1d (GLdouble u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord1d", FindTask(NULL)));
    GLCALL(glEvalCoord1d, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord1f (GLfloat u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord1f", FindTask(NULL)));
    GLCALL(glEvalCoord1f, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord1dv (const GLdouble * u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord1dv", FindTask(NULL)));
    GLCALL(glEvalCoord1dv, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord1fv (const GLfloat * u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord1fv", FindTask(NULL)));
    GLCALL(glEvalCoord1fv, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord2d (GLdouble u, GLdouble v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord2d", FindTask(NULL)));
    GLCALL(glEvalCoord2d, u, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord2f (GLfloat u, GLfloat v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord2f", FindTask(NULL)));
    GLCALL(glEvalCoord2f, u, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord2dv (const GLdouble * u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord2dv", FindTask(NULL)));
    GLCALL(glEvalCoord2dv, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalCoord2fv (const GLfloat * u)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalCoord2fv", FindTask(NULL)));
    GLCALL(glEvalCoord2fv, u);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapGrid1d", FindTask(NULL)));
    GLCALL(glMapGrid1d, un, u1, u2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapGrid1f", FindTask(NULL)));
    GLCALL(glMapGrid1f, un, u1, u2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapGrid2d", FindTask(NULL)));
    GLCALL(glMapGrid2d, un, u1, u2, vn, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapGrid2f", FindTask(NULL)));
    GLCALL(glMapGrid2f, un, u1, u2, vn, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalPoint1 (GLint i)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalPoint1", FindTask(NULL)));
    GLCALL(glEvalPoint1, i);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalPoint2 (GLint i, GLint j)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalPoint2", FindTask(NULL)));
    GLCALL(glEvalPoint2, i, j);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalMesh1", FindTask(NULL)));
    GLCALL(glEvalMesh1, mode, i1, i2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEvalMesh2", FindTask(NULL)));
    GLCALL(glEvalMesh2, mode, i1, i2, j1, j2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogf", FindTask(NULL)));
    GLCALL(glFogf, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogi (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogi", FindTask(NULL)));
    GLCALL(glFogi, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogfv", FindTask(NULL)));
    GLCALL(glFogfv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogiv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogiv", FindTask(NULL)));
    GLCALL(glFogiv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFeedbackBuffer (GLsizei size, GLenum type, GLfloat * buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFeedbackBuffer", FindTask(NULL)));
    GLCALL(glFeedbackBuffer, size, type, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPassThrough (GLfloat token)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPassThrough", FindTask(NULL)));
    GLCALL(glPassThrough, token);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSelectBuffer (GLsizei size, GLuint * buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSelectBuffer", FindTask(NULL)));
    GLCALL(glSelectBuffer, size, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglInitNames ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glInitNames", FindTask(NULL)));
    GLCALL(glInitNames);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadName (GLuint name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadName", FindTask(NULL)));
    GLCALL(glLoadName, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPushName (GLuint name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPushName", FindTask(NULL)));
    GLCALL(glPushName, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPopName ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPopName", FindTask(NULL)));
    GLCALL(glPopName);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawRangeElements", FindTask(NULL)));
    GLCALL(glDrawRangeElements, mode, start, end, count, type, indices);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexImage3D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexImage3D", FindTask(NULL)));
    GLCALL(glTexImage3D, target, level, internalFormat, width, height, depth, border, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage3D", FindTask(NULL)));
    GLCALL(glTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage3D", FindTask(NULL)));
    GLCALL(glCopyTexSubImage3D, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTable", FindTask(NULL)));
    GLCALL(glColorTable, target, internalformat, width, format, type, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorSubTable", FindTask(NULL)));
    GLCALL(glColorSubTable, target, start, count, format, type, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableParameteriv", FindTask(NULL)));
    GLCALL(glColorTableParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableParameterfv", FindTask(NULL)));
    GLCALL(glColorTableParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyColorSubTable", FindTask(NULL)));
    GLCALL(glCopyColorSubTable, target, start, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyColorTable", FindTask(NULL)));
    GLCALL(glCopyColorTable, target, internalformat, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTable", FindTask(NULL)));
    GLCALL(glGetColorTable, target, format, type, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameterfv", FindTask(NULL)));
    GLCALL(glGetColorTableParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameteriv", FindTask(NULL)));
    GLCALL(glGetColorTableParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquation (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquation", FindTask(NULL)));
    GLCALL(glBlendEquation, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendColor", FindTask(NULL)));
    GLCALL(glBlendColor, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glHistogram", FindTask(NULL)));
    GLCALL(glHistogram, target, width, internalformat, sink);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResetHistogram (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResetHistogram", FindTask(NULL)));
    GLCALL(glResetHistogram, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogram", FindTask(NULL)));
    GLCALL(glGetHistogram, target, reset, format, type, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogramParameterfv", FindTask(NULL)));
    GLCALL(glGetHistogramParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogramParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogramParameteriv", FindTask(NULL)));
    GLCALL(glGetHistogramParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMinmax (GLenum target, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMinmax", FindTask(NULL)));
    GLCALL(glMinmax, target, internalformat, sink);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResetMinmax (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResetMinmax", FindTask(NULL)));
    GLCALL(glResetMinmax, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmax", FindTask(NULL)));
    GLCALL(glGetMinmax, target, reset, format, types, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmaxParameterfv", FindTask(NULL)));
    GLCALL(glGetMinmaxParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmaxParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmaxParameteriv", FindTask(NULL)));
    GLCALL(glGetMinmaxParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionFilter1D", FindTask(NULL)));
    GLCALL(glConvolutionFilter1D, target, internalformat, width, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionFilter2D", FindTask(NULL)));
    GLCALL(glConvolutionFilter2D, target, internalformat, width, height, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameterf (GLenum target, GLenum pname, GLfloat params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameterf", FindTask(NULL)));
    GLCALL(glConvolutionParameterf, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameterfv", FindTask(NULL)));
    GLCALL(glConvolutionParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameteri (GLenum target, GLenum pname, GLint params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameteri", FindTask(NULL)));
    GLCALL(glConvolutionParameteri, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameteriv", FindTask(NULL)));
    GLCALL(glConvolutionParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyConvolutionFilter1D", FindTask(NULL)));
    GLCALL(glCopyConvolutionFilter1D, target, internalformat, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyConvolutionFilter2D", FindTask(NULL)));
    GLCALL(glCopyConvolutionFilter2D, target, internalformat, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionFilter", FindTask(NULL)));
    GLCALL(glGetConvolutionFilter, target, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionParameterfv", FindTask(NULL)));
    GLCALL(glGetConvolutionParameterfv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionParameteriv", FindTask(NULL)));
    GLCALL(glGetConvolutionParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSeparableFilter2D", FindTask(NULL)));
    GLCALL(glSeparableFilter2D, target, internalformat, width, height, format, type, row, column);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSeparableFilter", FindTask(NULL)));
    GLCALL(glGetSeparableFilter, target, format, type, row, column, span);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglActiveTexture (GLenum texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glActiveTexture", FindTask(NULL)));
    GLCALL(glActiveTexture, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClientActiveTexture (GLenum texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClientActiveTexture", FindTask(NULL)));
    GLCALL(glClientActiveTexture, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage1D", FindTask(NULL)));
    GLCALL(glCompressedTexImage1D, target, level, internalformat, width, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage2D", FindTask(NULL)));
    GLCALL(glCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage3D", FindTask(NULL)));
    GLCALL(glCompressedTexImage3D, target, level, internalformat, width, height, depth, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage1D", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage1D, target, level, xoffset, width, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage2D", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage3D", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCompressedTexImage (GLenum target, GLint lod, GLvoid * img)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCompressedTexImage", FindTask(NULL)));
    GLCALL(glGetCompressedTexImage, target, lod, img);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1d (GLenum target, GLdouble s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1d", FindTask(NULL)));
    GLCALL(glMultiTexCoord1d, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1dv", FindTask(NULL)));
    GLCALL(glMultiTexCoord1dv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1f (GLenum target, GLfloat s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1f", FindTask(NULL)));
    GLCALL(glMultiTexCoord1f, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1fv", FindTask(NULL)));
    GLCALL(glMultiTexCoord1fv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1i (GLenum target, GLint s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1i", FindTask(NULL)));
    GLCALL(glMultiTexCoord1i, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1iv", FindTask(NULL)));
    GLCALL(glMultiTexCoord1iv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1s (GLenum target, GLshort s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1s", FindTask(NULL)));
    GLCALL(glMultiTexCoord1s, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1sv", FindTask(NULL)));
    GLCALL(glMultiTexCoord1sv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2d", FindTask(NULL)));
    GLCALL(glMultiTexCoord2d, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2dv", FindTask(NULL)));
    GLCALL(glMultiTexCoord2dv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2f", FindTask(NULL)));
    GLCALL(glMultiTexCoord2f, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2fv", FindTask(NULL)));
    GLCALL(glMultiTexCoord2fv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2i (GLenum target, GLint s, GLint t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2i", FindTask(NULL)));
    GLCALL(glMultiTexCoord2i, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2iv", FindTask(NULL)));
    GLCALL(glMultiTexCoord2iv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2s (GLenum target, GLshort s, GLshort t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2s", FindTask(NULL)));
    GLCALL(glMultiTexCoord2s, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2sv", FindTask(NULL)));
    GLCALL(glMultiTexCoord2sv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3d", FindTask(NULL)));
    GLCALL(glMultiTexCoord3d, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3dv", FindTask(NULL)));
    GLCALL(glMultiTexCoord3dv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3f", FindTask(NULL)));
    GLCALL(glMultiTexCoord3f, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3fv", FindTask(NULL)));
    GLCALL(glMultiTexCoord3fv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3i", FindTask(NULL)));
    GLCALL(glMultiTexCoord3i, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3iv", FindTask(NULL)));
    GLCALL(glMultiTexCoord3iv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3s", FindTask(NULL)));
    GLCALL(glMultiTexCoord3s, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3sv", FindTask(NULL)));
    GLCALL(glMultiTexCoord3sv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4d", FindTask(NULL)));
    GLCALL(glMultiTexCoord4d, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4dv", FindTask(NULL)));
    GLCALL(glMultiTexCoord4dv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4f", FindTask(NULL)));
    GLCALL(glMultiTexCoord4f, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4fv", FindTask(NULL)));
    GLCALL(glMultiTexCoord4fv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4i (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4i", FindTask(NULL)));
    GLCALL(glMultiTexCoord4i, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4iv", FindTask(NULL)));
    GLCALL(glMultiTexCoord4iv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4s", FindTask(NULL)));
    GLCALL(glMultiTexCoord4s, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4sv", FindTask(NULL)));
    GLCALL(glMultiTexCoord4sv, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadTransposeMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadTransposeMatrixd", FindTask(NULL)));
    GLCALL(glLoadTransposeMatrixd, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadTransposeMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadTransposeMatrixf", FindTask(NULL)));
    GLCALL(glLoadTransposeMatrixf, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultTransposeMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultTransposeMatrixd", FindTask(NULL)));
    GLCALL(glMultTransposeMatrixd, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultTransposeMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultTransposeMatrixf", FindTask(NULL)));
    GLCALL(glMultTransposeMatrixf, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSampleCoverage (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSampleCoverage", FindTask(NULL)));
    GLCALL(glSampleCoverage, value, invert);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglActiveTextureARB (GLenum texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glActiveTextureARB", FindTask(NULL)));
    GLCALL(glActiveTextureARB, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClientActiveTextureARB (GLenum texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClientActiveTextureARB", FindTask(NULL)));
    GLCALL(glClientActiveTextureARB, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1dARB (GLenum target, GLdouble s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1dARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1dARB, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1dvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1dvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1fARB (GLenum target, GLfloat s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1fARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1fARB, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1fvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1fvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1iARB (GLenum target, GLint s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1iARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1iARB, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1ivARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1ivARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1sARB (GLenum target, GLshort s)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1sARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1sARB, target, s);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord1svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord1svARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord1svARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2dARB (GLenum target, GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2dARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2dARB, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2dvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2dvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2fARB (GLenum target, GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2fARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2fARB, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2fvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2fvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2iARB (GLenum target, GLint s, GLint t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2iARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2iARB, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2ivARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2ivARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2sARB (GLenum target, GLshort s, GLshort t)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2sARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2sARB, target, s, t);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord2svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord2svARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord2svARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3dARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3dARB, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3dvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3dvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3fARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3fARB, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3fvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3fvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3iARB (GLenum target, GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3iARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3iARB, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3ivARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3ivARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3sARB (GLenum target, GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3sARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3sARB, target, s, t, r);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord3svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord3svARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord3svARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4dARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4dARB, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4dvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4dvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4fARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4fARB, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4fvARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4fvARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4iARB (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4iARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4iARB, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4ivARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4ivARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4sARB (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4sARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4sARB, target, s, t, r, q);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiTexCoord4svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiTexCoord4svARB", FindTask(NULL)));
    GLCALL(glMultiTexCoord4svARB, target, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncSeparate", FindTask(NULL)));
    GLCALL(glBlendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordf (GLfloat coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordf", FindTask(NULL)));
    GLCALL(glFogCoordf, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordfv (const GLfloat * coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordfv", FindTask(NULL)));
    GLCALL(glFogCoordfv, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordd (GLdouble coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordd", FindTask(NULL)));
    GLCALL(glFogCoordd, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoorddv (const GLdouble * coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoorddv", FindTask(NULL)));
    GLCALL(glFogCoorddv, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordPointer (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordPointer", FindTask(NULL)));
    GLCALL(glFogCoordPointer, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiDrawArrays (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiDrawArrays", FindTask(NULL)));
    GLCALL(glMultiDrawArrays, mode, first, count, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiDrawElements (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiDrawElements", FindTask(NULL)));
    GLCALL(glMultiDrawElements, mode, count, type, indices, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterf", FindTask(NULL)));
    GLCALL(glPointParameterf, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfv", FindTask(NULL)));
    GLCALL(glPointParameterfv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameteri (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameteri", FindTask(NULL)));
    GLCALL(glPointParameteri, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameteriv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameteriv", FindTask(NULL)));
    GLCALL(glPointParameteriv, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3b", FindTask(NULL)));
    GLCALL(glSecondaryColor3b, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3bv (const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3bv", FindTask(NULL)));
    GLCALL(glSecondaryColor3bv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3d", FindTask(NULL)));
    GLCALL(glSecondaryColor3d, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3dv", FindTask(NULL)));
    GLCALL(glSecondaryColor3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3f", FindTask(NULL)));
    GLCALL(glSecondaryColor3f, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3fv", FindTask(NULL)));
    GLCALL(glSecondaryColor3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3i (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3i", FindTask(NULL)));
    GLCALL(glSecondaryColor3i, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3iv", FindTask(NULL)));
    GLCALL(glSecondaryColor3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3s (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3s", FindTask(NULL)));
    GLCALL(glSecondaryColor3s, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3sv", FindTask(NULL)));
    GLCALL(glSecondaryColor3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ub", FindTask(NULL)));
    GLCALL(glSecondaryColor3ub, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ubv (const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ubv", FindTask(NULL)));
    GLCALL(glSecondaryColor3ubv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ui (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ui", FindTask(NULL)));
    GLCALL(glSecondaryColor3ui, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3uiv (const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3uiv", FindTask(NULL)));
    GLCALL(glSecondaryColor3uiv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3us (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3us", FindTask(NULL)));
    GLCALL(glSecondaryColor3us, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3usv (const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3usv", FindTask(NULL)));
    GLCALL(glSecondaryColor3usv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColorPointer", FindTask(NULL)));
    GLCALL(glSecondaryColorPointer, size, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2d", FindTask(NULL)));
    GLCALL(glWindowPos2d, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2dv", FindTask(NULL)));
    GLCALL(glWindowPos2dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2f", FindTask(NULL)));
    GLCALL(glWindowPos2f, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2fv", FindTask(NULL)));
    GLCALL(glWindowPos2fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2i (GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2i", FindTask(NULL)));
    GLCALL(glWindowPos2i, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2iv", FindTask(NULL)));
    GLCALL(glWindowPos2iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2s", FindTask(NULL)));
    GLCALL(glWindowPos2s, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2sv", FindTask(NULL)));
    GLCALL(glWindowPos2sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3d", FindTask(NULL)));
    GLCALL(glWindowPos3d, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3dv (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3dv", FindTask(NULL)));
    GLCALL(glWindowPos3dv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3f", FindTask(NULL)));
    GLCALL(glWindowPos3f, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3fv (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3fv", FindTask(NULL)));
    GLCALL(glWindowPos3fv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3i", FindTask(NULL)));
    GLCALL(glWindowPos3i, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3iv (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3iv", FindTask(NULL)));
    GLCALL(glWindowPos3iv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3s", FindTask(NULL)));
    GLCALL(glWindowPos3s, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3sv (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3sv", FindTask(NULL)));
    GLCALL(glWindowPos3sv, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenQueries (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenQueries", FindTask(NULL)));
    GLCALL(glGenQueries, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteQueries (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteQueries", FindTask(NULL)));
    GLCALL(glDeleteQueries, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsQuery (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsQuery", FindTask(NULL)));
    _ret = GLCALL(glIsQuery, id);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBeginQuery (GLenum target, GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginQuery", FindTask(NULL)));
    GLCALL(glBeginQuery, target, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndQuery (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndQuery", FindTask(NULL)));
    GLCALL(glEndQuery, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryiv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryiv", FindTask(NULL)));
    GLCALL(glGetQueryiv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjectiv (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjectiv", FindTask(NULL)));
    GLCALL(glGetQueryObjectiv, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjectuiv (GLuint id, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjectuiv", FindTask(NULL)));
    GLCALL(glGetQueryObjectuiv, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBuffer (GLenum target, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBuffer", FindTask(NULL)));
    GLCALL(glBindBuffer, target, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteBuffers (GLsizei n, const GLuint * buffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteBuffers", FindTask(NULL)));
    GLCALL(glDeleteBuffers, n, buffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenBuffers (GLsizei n, GLuint * buffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenBuffers", FindTask(NULL)));
    GLCALL(glGenBuffers, n, buffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsBuffer (GLuint buffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsBuffer", FindTask(NULL)));
    _ret = GLCALL(glIsBuffer, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBufferData (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBufferData", FindTask(NULL)));
    GLCALL(glBufferData, target, size, data, usage);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBufferSubData", FindTask(NULL)));
    GLCALL(glBufferSubData, target, offset, size, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferSubData", FindTask(NULL)));
    GLCALL(glGetBufferSubData, target, offset, size, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLvoid* mglMapBuffer (GLenum target, GLenum access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapBuffer", FindTask(NULL)));
    _ret = GLCALL(glMapBuffer, target, access);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglUnmapBuffer (GLenum target)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUnmapBuffer", FindTask(NULL)));
    _ret = GLCALL(glUnmapBuffer, target);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetBufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferParameteriv", FindTask(NULL)));
    GLCALL(glGetBufferParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBufferPointerv (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferPointerv", FindTask(NULL)));
    GLCALL(glGetBufferPointerv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationSeparate", FindTask(NULL)));
    GLCALL(glBlendEquationSeparate, modeRGB, modeAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawBuffers (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawBuffers", FindTask(NULL)));
    GLCALL(glDrawBuffers, n, bufs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilOpSeparate", FindTask(NULL)));
    GLCALL(glStencilOpSeparate, face, sfail, dpfail, dppass);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilFuncSeparate", FindTask(NULL)));
    GLCALL(glStencilFuncSeparate, face, func, ref, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilMaskSeparate (GLenum face, GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilMaskSeparate", FindTask(NULL)));
    GLCALL(glStencilMaskSeparate, face, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAttachShader (GLuint program, GLuint shader)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAttachShader", FindTask(NULL)));
    GLCALL(glAttachShader, program, shader);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindAttribLocation (GLuint program, GLuint index, const GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindAttribLocation", FindTask(NULL)));
    GLCALL(glBindAttribLocation, program, index, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompileShader (GLuint shader)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompileShader", FindTask(NULL)));
    GLCALL(glCompileShader, shader);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLuint mglCreateProgram ()
{
    GLuint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCreateProgram", FindTask(NULL)));
    _ret = GLCALL(glCreateProgram);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLuint mglCreateShader (GLenum type)
{
    GLuint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCreateShader", FindTask(NULL)));
    _ret = GLCALL(glCreateShader, type);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglDeleteProgram (GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteProgram", FindTask(NULL)));
    GLCALL(glDeleteProgram, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteShader (GLuint shader)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteShader", FindTask(NULL)));
    GLCALL(glDeleteShader, shader);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDetachShader (GLuint program, GLuint shader)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDetachShader", FindTask(NULL)));
    GLCALL(glDetachShader, program, shader);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisableVertexAttribArray (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisableVertexAttribArray", FindTask(NULL)));
    GLCALL(glDisableVertexAttribArray, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEnableVertexAttribArray (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnableVertexAttribArray", FindTask(NULL)));
    GLCALL(glEnableVertexAttribArray, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetActiveAttrib", FindTask(NULL)));
    GLCALL(glGetActiveAttrib, program, index, bufSize, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetActiveUniform", FindTask(NULL)));
    GLCALL(glGetActiveUniform, program, index, bufSize, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei * count, GLuint * obj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetAttachedShaders", FindTask(NULL)));
    GLCALL(glGetAttachedShaders, program, maxCount, count, obj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetAttribLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetAttribLocation", FindTask(NULL)));
    _ret = GLCALL(glGetAttribLocation, program, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetProgramiv (GLuint program, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramiv", FindTask(NULL)));
    GLCALL(glGetProgramiv, program, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramInfoLog", FindTask(NULL)));
    GLCALL(glGetProgramInfoLog, program, bufSize, length, infoLog);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetShaderiv (GLuint shader, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetShaderiv", FindTask(NULL)));
    GLCALL(glGetShaderiv, shader, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetShaderInfoLog", FindTask(NULL)));
    GLCALL(glGetShaderInfoLog, shader, bufSize, length, infoLog);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetShaderSource", FindTask(NULL)));
    GLCALL(glGetShaderSource, shader, bufSize, length, source);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetUniformLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformLocation", FindTask(NULL)));
    _ret = GLCALL(glGetUniformLocation, program, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetUniformfv (GLuint program, GLint location, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformfv", FindTask(NULL)));
    GLCALL(glGetUniformfv, program, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetUniformiv (GLuint program, GLint location, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformiv", FindTask(NULL)));
    GLCALL(glGetUniformiv, program, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribdv (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribdv", FindTask(NULL)));
    GLCALL(glGetVertexAttribdv, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribfv (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribfv", FindTask(NULL)));
    GLCALL(glGetVertexAttribfv, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribiv (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribiv", FindTask(NULL)));
    GLCALL(glGetVertexAttribiv, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribPointerv", FindTask(NULL)));
    GLCALL(glGetVertexAttribPointerv, index, pname, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsProgram (GLuint program)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsProgram", FindTask(NULL)));
    _ret = GLCALL(glIsProgram, program);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsShader (GLuint shader)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsShader", FindTask(NULL)));
    _ret = GLCALL(glIsShader, shader);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglLinkProgram (GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLinkProgram", FindTask(NULL)));
    GLCALL(glLinkProgram, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglShaderSource (GLuint shader, GLsizei count, const GLchar *  * string, const GLint * length)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glShaderSource", FindTask(NULL)));
    GLCALL(glShaderSource, shader, count, string, length);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUseProgram (GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUseProgram", FindTask(NULL)));
    GLCALL(glUseProgram, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1f (GLint location, GLfloat v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1f", FindTask(NULL)));
    GLCALL(glUniform1f, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2f", FindTask(NULL)));
    GLCALL(glUniform2f, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3f", FindTask(NULL)));
    GLCALL(glUniform3f, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4f", FindTask(NULL)));
    GLCALL(glUniform4f, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1i (GLint location, GLint v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1i", FindTask(NULL)));
    GLCALL(glUniform1i, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2i (GLint location, GLint v0, GLint v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2i", FindTask(NULL)));
    GLCALL(glUniform2i, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3i", FindTask(NULL)));
    GLCALL(glUniform3i, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4i", FindTask(NULL)));
    GLCALL(glUniform4i, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1fv", FindTask(NULL)));
    GLCALL(glUniform1fv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2fv", FindTask(NULL)));
    GLCALL(glUniform2fv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3fv", FindTask(NULL)));
    GLCALL(glUniform3fv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4fv", FindTask(NULL)));
    GLCALL(glUniform4fv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1iv", FindTask(NULL)));
    GLCALL(glUniform1iv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2iv", FindTask(NULL)));
    GLCALL(glUniform2iv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3iv", FindTask(NULL)));
    GLCALL(glUniform3iv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4iv", FindTask(NULL)));
    GLCALL(glUniform4iv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix2fv", FindTask(NULL)));
    GLCALL(glUniformMatrix2fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix3fv", FindTask(NULL)));
    GLCALL(glUniformMatrix3fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix4fv", FindTask(NULL)));
    GLCALL(glUniformMatrix4fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglValidateProgram (GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glValidateProgram", FindTask(NULL)));
    GLCALL(glValidateProgram, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1d (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1d", FindTask(NULL)));
    GLCALL(glVertexAttrib1d, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1dv", FindTask(NULL)));
    GLCALL(glVertexAttrib1dv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1f (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1f", FindTask(NULL)));
    GLCALL(glVertexAttrib1f, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1fv", FindTask(NULL)));
    GLCALL(glVertexAttrib1fv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1s (GLuint index, GLshort x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1s", FindTask(NULL)));
    GLCALL(glVertexAttrib1s, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1sv", FindTask(NULL)));
    GLCALL(glVertexAttrib1sv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2d (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2d", FindTask(NULL)));
    GLCALL(glVertexAttrib2d, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2dv", FindTask(NULL)));
    GLCALL(glVertexAttrib2dv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2f", FindTask(NULL)));
    GLCALL(glVertexAttrib2f, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2fv", FindTask(NULL)));
    GLCALL(glVertexAttrib2fv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2s (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2s", FindTask(NULL)));
    GLCALL(glVertexAttrib2s, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2sv", FindTask(NULL)));
    GLCALL(glVertexAttrib2sv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3d", FindTask(NULL)));
    GLCALL(glVertexAttrib3d, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3dv", FindTask(NULL)));
    GLCALL(glVertexAttrib3dv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3f", FindTask(NULL)));
    GLCALL(glVertexAttrib3f, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3fv", FindTask(NULL)));
    GLCALL(glVertexAttrib3fv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3s", FindTask(NULL)));
    GLCALL(glVertexAttrib3s, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3sv", FindTask(NULL)));
    GLCALL(glVertexAttrib3sv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nbv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nbv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nbv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Niv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Niv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Niv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nsv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nsv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nsv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nub", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nub, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nubv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nubv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nuiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nuiv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nuiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4Nusv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4Nusv", FindTask(NULL)));
    GLCALL(glVertexAttrib4Nusv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4bv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4bv", FindTask(NULL)));
    GLCALL(glVertexAttrib4bv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4d", FindTask(NULL)));
    GLCALL(glVertexAttrib4d, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4dv", FindTask(NULL)));
    GLCALL(glVertexAttrib4dv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4f", FindTask(NULL)));
    GLCALL(glVertexAttrib4f, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4fv", FindTask(NULL)));
    GLCALL(glVertexAttrib4fv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4iv", FindTask(NULL)));
    GLCALL(glVertexAttrib4iv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4s", FindTask(NULL)));
    GLCALL(glVertexAttrib4s, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4sv", FindTask(NULL)));
    GLCALL(glVertexAttrib4sv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4ubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4ubv", FindTask(NULL)));
    GLCALL(glVertexAttrib4ubv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4uiv", FindTask(NULL)));
    GLCALL(glVertexAttrib4uiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4usv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4usv", FindTask(NULL)));
    GLCALL(glVertexAttrib4usv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribPointer", FindTask(NULL)));
    GLCALL(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix2x3fv", FindTask(NULL)));
    GLCALL(glUniformMatrix2x3fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix3x2fv", FindTask(NULL)));
    GLCALL(glUniformMatrix3x2fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix2x4fv", FindTask(NULL)));
    GLCALL(glUniformMatrix2x4fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix4x2fv", FindTask(NULL)));
    GLCALL(glUniformMatrix4x2fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix3x4fv", FindTask(NULL)));
    GLCALL(glUniformMatrix3x4fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix4x3fv", FindTask(NULL)));
    GLCALL(glUniformMatrix4x3fv, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadTransposeMatrixfARB (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadTransposeMatrixfARB", FindTask(NULL)));
    GLCALL(glLoadTransposeMatrixfARB, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLoadTransposeMatrixdARB (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadTransposeMatrixdARB", FindTask(NULL)));
    GLCALL(glLoadTransposeMatrixdARB, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultTransposeMatrixfARB (const GLfloat * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultTransposeMatrixfARB", FindTask(NULL)));
    GLCALL(glMultTransposeMatrixfARB, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultTransposeMatrixdARB (const GLdouble * m)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultTransposeMatrixdARB", FindTask(NULL)));
    GLCALL(glMultTransposeMatrixdARB, m);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSampleCoverageARB (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSampleCoverageARB", FindTask(NULL)));
    GLCALL(glSampleCoverageARB, value, invert);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage3DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage3DARB", FindTask(NULL)));
    GLCALL(glCompressedTexImage3DARB, target, level, internalformat, width, height, depth, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage2DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage2DARB", FindTask(NULL)));
    GLCALL(glCompressedTexImage2DARB, target, level, internalformat, width, height, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexImage1DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexImage1DARB", FindTask(NULL)));
    GLCALL(glCompressedTexImage1DARB, target, level, internalformat, width, border, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage3DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage3DARB", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage3DARB, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage2DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage2DARB", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage2DARB, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompressedTexSubImage1DARB (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompressedTexSubImage1DARB", FindTask(NULL)));
    GLCALL(glCompressedTexSubImage1DARB, target, level, xoffset, width, format, imageSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCompressedTexImageARB (GLenum target, GLint level, GLvoid * img)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCompressedTexImageARB", FindTask(NULL)));
    GLCALL(glGetCompressedTexImageARB, target, level, img);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfARB (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfARB", FindTask(NULL)));
    GLCALL(glPointParameterfARB, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfvARB (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfvARB", FindTask(NULL)));
    GLCALL(glPointParameterfvARB, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2dARB (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2dARB", FindTask(NULL)));
    GLCALL(glWindowPos2dARB, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2dvARB (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2dvARB", FindTask(NULL)));
    GLCALL(glWindowPos2dvARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2fARB (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2fARB", FindTask(NULL)));
    GLCALL(glWindowPos2fARB, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2fvARB (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2fvARB", FindTask(NULL)));
    GLCALL(glWindowPos2fvARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2iARB (GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2iARB", FindTask(NULL)));
    GLCALL(glWindowPos2iARB, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2ivARB (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2ivARB", FindTask(NULL)));
    GLCALL(glWindowPos2ivARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2sARB (GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2sARB", FindTask(NULL)));
    GLCALL(glWindowPos2sARB, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2svARB (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2svARB", FindTask(NULL)));
    GLCALL(glWindowPos2svARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3dARB (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3dARB", FindTask(NULL)));
    GLCALL(glWindowPos3dARB, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3dvARB (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3dvARB", FindTask(NULL)));
    GLCALL(glWindowPos3dvARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3fARB (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3fARB", FindTask(NULL)));
    GLCALL(glWindowPos3fARB, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3fvARB (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3fvARB", FindTask(NULL)));
    GLCALL(glWindowPos3fvARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3iARB (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3iARB", FindTask(NULL)));
    GLCALL(glWindowPos3iARB, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3ivARB (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3ivARB", FindTask(NULL)));
    GLCALL(glWindowPos3ivARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3sARB (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3sARB", FindTask(NULL)));
    GLCALL(glWindowPos3sARB, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3svARB (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3svARB", FindTask(NULL)));
    GLCALL(glWindowPos3svARB, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1dARB (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1dARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1dARB, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1dvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1dvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1fARB (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1fARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1fARB, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1fvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1fvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1sARB (GLuint index, GLshort x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1sARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1sARB, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1svARB", FindTask(NULL)));
    GLCALL(glVertexAttrib1svARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2dARB (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2dARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2dARB, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2dvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2dvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2fARB (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2fARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2fARB, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2fvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2fvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2sARB (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2sARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2sARB, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2svARB", FindTask(NULL)));
    GLCALL(glVertexAttrib2svARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3dARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3dARB, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3dvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3dvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3fARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3fARB, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3fvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3fvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3sARB (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3sARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3sARB, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3svARB", FindTask(NULL)));
    GLCALL(glVertexAttrib3svARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NbvARB (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NbvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NbvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NivARB (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NivARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NivARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NsvARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NsvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NsvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NubARB (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NubARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NubARB, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NubvARB (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NubvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NubvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NuivARB (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NuivARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NuivARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4NusvARB (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4NusvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4NusvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4bvARB (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4bvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4bvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4dARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4dARB, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4dvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4dvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4fARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4fARB, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4fvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4fvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4ivARB (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4ivARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4ivARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4sARB (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4sARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4sARB, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4svARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4svARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4ubvARB (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4ubvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4ubvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4uivARB (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4uivARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4uivARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4usvARB (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4usvARB", FindTask(NULL)));
    GLCALL(glVertexAttrib4usvARB, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribPointerARB (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribPointerARB", FindTask(NULL)));
    GLCALL(glVertexAttribPointerARB, index, size, type, normalized, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEnableVertexAttribArrayARB (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnableVertexAttribArrayARB", FindTask(NULL)));
    GLCALL(glEnableVertexAttribArrayARB, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisableVertexAttribArrayARB (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisableVertexAttribArrayARB", FindTask(NULL)));
    GLCALL(glDisableVertexAttribArrayARB, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramStringARB (GLenum target, GLenum format, GLsizei len, const GLvoid * string)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramStringARB", FindTask(NULL)));
    GLCALL(glProgramStringARB, target, format, len, string);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindProgramARB (GLenum target, GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindProgramARB", FindTask(NULL)));
    GLCALL(glBindProgramARB, target, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteProgramsARB (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteProgramsARB", FindTask(NULL)));
    GLCALL(glDeleteProgramsARB, n, programs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenProgramsARB (GLsizei n, GLuint * programs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenProgramsARB", FindTask(NULL)));
    GLCALL(glGenProgramsARB, n, programs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramEnvParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramEnvParameter4dARB", FindTask(NULL)));
    GLCALL(glProgramEnvParameter4dARB, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramEnvParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramEnvParameter4dvARB", FindTask(NULL)));
    GLCALL(glProgramEnvParameter4dvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramEnvParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramEnvParameter4fARB", FindTask(NULL)));
    GLCALL(glProgramEnvParameter4fARB, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramEnvParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramEnvParameter4fvARB", FindTask(NULL)));
    GLCALL(glProgramEnvParameter4fvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramLocalParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramLocalParameter4dARB", FindTask(NULL)));
    GLCALL(glProgramLocalParameter4dARB, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramLocalParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramLocalParameter4dvARB", FindTask(NULL)));
    GLCALL(glProgramLocalParameter4dvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramLocalParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramLocalParameter4fARB", FindTask(NULL)));
    GLCALL(glProgramLocalParameter4fARB, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramLocalParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramLocalParameter4fvARB", FindTask(NULL)));
    GLCALL(glProgramLocalParameter4fvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramEnvParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramEnvParameterdvARB", FindTask(NULL)));
    GLCALL(glGetProgramEnvParameterdvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramEnvParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramEnvParameterfvARB", FindTask(NULL)));
    GLCALL(glGetProgramEnvParameterfvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramLocalParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramLocalParameterdvARB", FindTask(NULL)));
    GLCALL(glGetProgramLocalParameterdvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramLocalParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramLocalParameterfvARB", FindTask(NULL)));
    GLCALL(glGetProgramLocalParameterfvARB, target, index, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramivARB", FindTask(NULL)));
    GLCALL(glGetProgramivARB, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramStringARB (GLenum target, GLenum pname, GLvoid * string)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramStringARB", FindTask(NULL)));
    GLCALL(glGetProgramStringARB, target, pname, string);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribdvARB (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribdvARB", FindTask(NULL)));
    GLCALL(glGetVertexAttribdvARB, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribfvARB (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribfvARB", FindTask(NULL)));
    GLCALL(glGetVertexAttribfvARB, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribivARB (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribivARB", FindTask(NULL)));
    GLCALL(glGetVertexAttribivARB, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribPointervARB (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribPointervARB", FindTask(NULL)));
    GLCALL(glGetVertexAttribPointervARB, index, pname, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsProgramARB (GLuint program)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsProgramARB", FindTask(NULL)));
    _ret = GLCALL(glIsProgramARB, program);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindBufferARB (GLenum target, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferARB", FindTask(NULL)));
    GLCALL(glBindBufferARB, target, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteBuffersARB (GLsizei n, const GLuint * buffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteBuffersARB", FindTask(NULL)));
    GLCALL(glDeleteBuffersARB, n, buffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenBuffersARB (GLsizei n, GLuint * buffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenBuffersARB", FindTask(NULL)));
    GLCALL(glGenBuffersARB, n, buffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsBufferARB (GLuint buffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsBufferARB", FindTask(NULL)));
    _ret = GLCALL(glIsBufferARB, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBufferDataARB", FindTask(NULL)));
    GLCALL(glBufferDataARB, target, size, data, usage);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBufferSubDataARB", FindTask(NULL)));
    GLCALL(glBufferSubDataARB, target, offset, size, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferSubDataARB", FindTask(NULL)));
    GLCALL(glGetBufferSubDataARB, target, offset, size, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLvoid* mglMapBufferARB (GLenum target, GLenum access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapBufferARB", FindTask(NULL)));
    _ret = GLCALL(glMapBufferARB, target, access);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglUnmapBufferARB (GLenum target)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUnmapBufferARB", FindTask(NULL)));
    _ret = GLCALL(glUnmapBufferARB, target);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetBufferParameterivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferParameterivARB", FindTask(NULL)));
    GLCALL(glGetBufferParameterivARB, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBufferPointervARB (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferPointervARB", FindTask(NULL)));
    GLCALL(glGetBufferPointervARB, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenQueriesARB (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenQueriesARB", FindTask(NULL)));
    GLCALL(glGenQueriesARB, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteQueriesARB (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteQueriesARB", FindTask(NULL)));
    GLCALL(glDeleteQueriesARB, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsQueryARB (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsQueryARB", FindTask(NULL)));
    _ret = GLCALL(glIsQueryARB, id);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBeginQueryARB (GLenum target, GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginQueryARB", FindTask(NULL)));
    GLCALL(glBeginQueryARB, target, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndQueryARB (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndQueryARB", FindTask(NULL)));
    GLCALL(glEndQueryARB, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryivARB", FindTask(NULL)));
    GLCALL(glGetQueryivARB, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjectivARB (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjectivARB", FindTask(NULL)));
    GLCALL(glGetQueryObjectivARB, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjectuivARB (GLuint id, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjectuivARB", FindTask(NULL)));
    GLCALL(glGetQueryObjectuivARB, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteObjectARB (GLhandleARB obj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteObjectARB", FindTask(NULL)));
    GLCALL(glDeleteObjectARB, obj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLhandleARB mglGetHandleARB (GLenum pname)
{
    GLhandleARB _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHandleARB", FindTask(NULL)));
    _ret = GLCALL(glGetHandleARB, pname);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglDetachObjectARB (GLhandleARB containerObj, GLhandleARB attachedObj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDetachObjectARB", FindTask(NULL)));
    GLCALL(glDetachObjectARB, containerObj, attachedObj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLhandleARB mglCreateShaderObjectARB (GLenum shaderType)
{
    GLhandleARB _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCreateShaderObjectARB", FindTask(NULL)));
    _ret = GLCALL(glCreateShaderObjectARB, shaderType);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglShaderSourceARB (GLhandleARB shaderObj, GLsizei count, const GLcharARB *  * string, const GLint * length)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glShaderSourceARB", FindTask(NULL)));
    GLCALL(glShaderSourceARB, shaderObj, count, string, length);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCompileShaderARB (GLhandleARB shaderObj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCompileShaderARB", FindTask(NULL)));
    GLCALL(glCompileShaderARB, shaderObj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLhandleARB mglCreateProgramObjectARB ()
{
    GLhandleARB _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCreateProgramObjectARB", FindTask(NULL)));
    _ret = GLCALL(glCreateProgramObjectARB);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglAttachObjectARB (GLhandleARB containerObj, GLhandleARB obj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAttachObjectARB", FindTask(NULL)));
    GLCALL(glAttachObjectARB, containerObj, obj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLinkProgramARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLinkProgramARB", FindTask(NULL)));
    GLCALL(glLinkProgramARB, programObj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUseProgramObjectARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUseProgramObjectARB", FindTask(NULL)));
    GLCALL(glUseProgramObjectARB, programObj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglValidateProgramARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glValidateProgramARB", FindTask(NULL)));
    GLCALL(glValidateProgramARB, programObj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1fARB (GLint location, GLfloat v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1fARB", FindTask(NULL)));
    GLCALL(glUniform1fARB, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2fARB (GLint location, GLfloat v0, GLfloat v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2fARB", FindTask(NULL)));
    GLCALL(glUniform2fARB, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3fARB", FindTask(NULL)));
    GLCALL(glUniform3fARB, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4fARB", FindTask(NULL)));
    GLCALL(glUniform4fARB, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1iARB (GLint location, GLint v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1iARB", FindTask(NULL)));
    GLCALL(glUniform1iARB, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2iARB (GLint location, GLint v0, GLint v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2iARB", FindTask(NULL)));
    GLCALL(glUniform2iARB, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3iARB (GLint location, GLint v0, GLint v1, GLint v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3iARB", FindTask(NULL)));
    GLCALL(glUniform3iARB, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4iARB (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4iARB", FindTask(NULL)));
    GLCALL(glUniform4iARB, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1fvARB", FindTask(NULL)));
    GLCALL(glUniform1fvARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2fvARB", FindTask(NULL)));
    GLCALL(glUniform2fvARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3fvARB", FindTask(NULL)));
    GLCALL(glUniform3fvARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4fvARB", FindTask(NULL)));
    GLCALL(glUniform4fvARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1ivARB", FindTask(NULL)));
    GLCALL(glUniform1ivARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2ivARB", FindTask(NULL)));
    GLCALL(glUniform2ivARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3ivARB", FindTask(NULL)));
    GLCALL(glUniform3ivARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4ivARB", FindTask(NULL)));
    GLCALL(glUniform4ivARB, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix2fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix2fvARB", FindTask(NULL)));
    GLCALL(glUniformMatrix2fvARB, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix3fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix3fvARB", FindTask(NULL)));
    GLCALL(glUniformMatrix3fvARB, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniformMatrix4fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniformMatrix4fvARB", FindTask(NULL)));
    GLCALL(glUniformMatrix4fvARB, location, count, transpose, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetObjectParameterfvARB (GLhandleARB obj, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetObjectParameterfvARB", FindTask(NULL)));
    GLCALL(glGetObjectParameterfvARB, obj, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetObjectParameterivARB (GLhandleARB obj, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetObjectParameterivARB", FindTask(NULL)));
    GLCALL(glGetObjectParameterivARB, obj, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetInfoLogARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * infoLog)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetInfoLogARB", FindTask(NULL)));
    GLCALL(glGetInfoLogARB, obj, maxLength, length, infoLog);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetAttachedObjectsARB (GLhandleARB containerObj, GLsizei maxCount, GLsizei * count, GLhandleARB * obj)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetAttachedObjectsARB", FindTask(NULL)));
    GLCALL(glGetAttachedObjectsARB, containerObj, maxCount, count, obj);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetUniformLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformLocationARB", FindTask(NULL)));
    _ret = GLCALL(glGetUniformLocationARB, programObj, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetActiveUniformARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetActiveUniformARB", FindTask(NULL)));
    GLCALL(glGetActiveUniformARB, programObj, index, maxLength, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetUniformfvARB (GLhandleARB programObj, GLint location, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformfvARB", FindTask(NULL)));
    GLCALL(glGetUniformfvARB, programObj, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetUniformivARB (GLhandleARB programObj, GLint location, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformivARB", FindTask(NULL)));
    GLCALL(glGetUniformivARB, programObj, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetShaderSourceARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * source)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetShaderSourceARB", FindTask(NULL)));
    GLCALL(glGetShaderSourceARB, obj, maxLength, length, source);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindAttribLocationARB (GLhandleARB programObj, GLuint index, const GLcharARB * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindAttribLocationARB", FindTask(NULL)));
    GLCALL(glBindAttribLocationARB, programObj, index, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetActiveAttribARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetActiveAttribARB", FindTask(NULL)));
    GLCALL(glGetActiveAttribARB, programObj, index, maxLength, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetAttribLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetAttribLocationARB", FindTask(NULL)));
    _ret = GLCALL(glGetAttribLocationARB, programObj, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglDrawBuffersARB (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawBuffersARB", FindTask(NULL)));
    GLCALL(glDrawBuffersARB, n, bufs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsRenderbuffer (GLuint renderbuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsRenderbuffer", FindTask(NULL)));
    _ret = GLCALL(glIsRenderbuffer, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindRenderbuffer (GLenum target, GLuint renderbuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindRenderbuffer", FindTask(NULL)));
    GLCALL(glBindRenderbuffer, target, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteRenderbuffers (GLsizei n, const GLuint * renderbuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteRenderbuffers", FindTask(NULL)));
    GLCALL(glDeleteRenderbuffers, n, renderbuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenRenderbuffers (GLsizei n, GLuint * renderbuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenRenderbuffers", FindTask(NULL)));
    GLCALL(glGenRenderbuffers, n, renderbuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRenderbufferStorage", FindTask(NULL)));
    GLCALL(glRenderbufferStorage, target, internalformat, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetRenderbufferParameteriv", FindTask(NULL)));
    GLCALL(glGetRenderbufferParameteriv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsFramebuffer (GLuint framebuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsFramebuffer", FindTask(NULL)));
    _ret = GLCALL(glIsFramebuffer, framebuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindFramebuffer (GLenum target, GLuint framebuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindFramebuffer", FindTask(NULL)));
    GLCALL(glBindFramebuffer, target, framebuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteFramebuffers (GLsizei n, const GLuint * framebuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteFramebuffers", FindTask(NULL)));
    GLCALL(glDeleteFramebuffers, n, framebuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenFramebuffers (GLsizei n, GLuint * framebuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenFramebuffers", FindTask(NULL)));
    GLCALL(glGenFramebuffers, n, framebuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLenum mglCheckFramebufferStatus (GLenum target)
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCheckFramebufferStatus", FindTask(NULL)));
    _ret = GLCALL(glCheckFramebufferStatus, target);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglFramebufferTexture1D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture1D", FindTask(NULL)));
    GLCALL(glFramebufferTexture1D, target, attachment, textarget, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture2D", FindTask(NULL)));
    GLCALL(glFramebufferTexture2D, target, attachment, textarget, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTexture3D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture3D", FindTask(NULL)));
    GLCALL(glFramebufferTexture3D, target, attachment, textarget, texture, level, zoffset);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferRenderbuffer", FindTask(NULL)));
    GLCALL(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFramebufferAttachmentParameteriv", FindTask(NULL)));
    GLCALL(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenerateMipmap (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenerateMipmap", FindTask(NULL)));
    GLCALL(glGenerateMipmap, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlitFramebuffer", FindTask(NULL)));
    GLCALL(glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRenderbufferStorageMultisample", FindTask(NULL)));
    GLCALL(glRenderbufferStorageMultisample, target, samples, internalformat, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTextureLayer", FindTask(NULL)));
    GLCALL(glFramebufferTextureLayer, target, attachment, texture, level, layer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendColorEXT", FindTask(NULL)));
    GLCALL(glBlendColorEXT, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPolygonOffsetEXT", FindTask(NULL)));
    GLCALL(glPolygonOffsetEXT, factor, bias);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexImage3DEXT (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexImage3DEXT", FindTask(NULL)));
    GLCALL(glTexImage3DEXT, target, level, internalformat, width, height, depth, border, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage3DEXT", FindTask(NULL)));
    GLCALL(glTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage1DEXT", FindTask(NULL)));
    GLCALL(glTexSubImage1DEXT, target, level, xoffset, width, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexSubImage2DEXT", FindTask(NULL)));
    GLCALL(glTexSubImage2DEXT, target, level, xoffset, yoffset, width, height, format, type, pixels);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexImage1DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexImage1DEXT", FindTask(NULL)));
    GLCALL(glCopyTexImage1DEXT, target, level, internalformat, x, y, width, border);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexImage2DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexImage2DEXT", FindTask(NULL)));
    GLCALL(glCopyTexImage2DEXT, target, level, internalformat, x, y, width, height, border);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage1DEXT", FindTask(NULL)));
    GLCALL(glCopyTexSubImage1DEXT, target, level, xoffset, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage2DEXT", FindTask(NULL)));
    GLCALL(glCopyTexSubImage2DEXT, target, level, xoffset, yoffset, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyTexSubImage3DEXT", FindTask(NULL)));
    GLCALL(glCopyTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglAreTexturesResidentEXT (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAreTexturesResidentEXT", FindTask(NULL)));
    _ret = GLCALL(glAreTexturesResidentEXT, n, textures, residences);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindTextureEXT (GLenum target, GLuint texture)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindTextureEXT", FindTask(NULL)));
    GLCALL(glBindTextureEXT, target, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteTexturesEXT (GLsizei n, const GLuint * textures)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteTexturesEXT", FindTask(NULL)));
    GLCALL(glDeleteTexturesEXT, n, textures);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenTexturesEXT (GLsizei n, GLuint * textures)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenTexturesEXT", FindTask(NULL)));
    GLCALL(glGenTexturesEXT, n, textures);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsTextureEXT (GLuint texture)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsTextureEXT", FindTask(NULL)));
    _ret = GLCALL(glIsTextureEXT, texture);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglPrioritizeTexturesEXT (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPrioritizeTexturesEXT", FindTask(NULL)));
    GLCALL(glPrioritizeTexturesEXT, n, textures, priorities);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglArrayElementEXT (GLint i)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glArrayElementEXT", FindTask(NULL)));
    GLCALL(glArrayElementEXT, i);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorPointerEXT", FindTask(NULL)));
    GLCALL(glColorPointerEXT, size, type, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawArraysEXT (GLenum mode, GLint first, GLsizei count)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawArraysEXT", FindTask(NULL)));
    GLCALL(glDrawArraysEXT, mode, first, count);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEdgeFlagPointerEXT (GLsizei stride, GLsizei count, const GLboolean * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEdgeFlagPointerEXT", FindTask(NULL)));
    GLCALL(glEdgeFlagPointerEXT, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPointervEXT (GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPointervEXT", FindTask(NULL)));
    GLCALL(glGetPointervEXT, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglIndexPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIndexPointerEXT", FindTask(NULL)));
    GLCALL(glIndexPointerEXT, type, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglNormalPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glNormalPointerEXT", FindTask(NULL)));
    GLCALL(glNormalPointerEXT, type, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexCoordPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexCoordPointerEXT", FindTask(NULL)));
    GLCALL(glTexCoordPointerEXT, size, type, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexPointerEXT", FindTask(NULL)));
    GLCALL(glVertexPointerEXT, size, type, stride, count, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationEXT (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationEXT", FindTask(NULL)));
    GLCALL(glBlendEquationEXT, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfEXT (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfEXT", FindTask(NULL)));
    GLCALL(glPointParameterfEXT, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfvEXT (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfvEXT", FindTask(NULL)));
    GLCALL(glPointParameterfvEXT, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableEXT (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableEXT", FindTask(NULL)));
    GLCALL(glColorTableEXT, target, internalFormat, width, format, type, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableEXT (GLenum target, GLenum format, GLenum type, GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableEXT", FindTask(NULL)));
    GLCALL(glGetColorTableEXT, target, format, type, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameterivEXT", FindTask(NULL)));
    GLCALL(glGetColorTableParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameterfvEXT", FindTask(NULL)));
    GLCALL(glGetColorTableParameterfvEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglLockArraysEXT (GLint first, GLsizei count)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLockArraysEXT", FindTask(NULL)));
    GLCALL(glLockArraysEXT, first, count);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUnlockArraysEXT ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUnlockArraysEXT", FindTask(NULL)));
    GLCALL(glUnlockArraysEXT);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawRangeElementsEXT (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawRangeElementsEXT", FindTask(NULL)));
    GLCALL(glDrawRangeElementsEXT, mode, start, end, count, type, indices);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3bEXT (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3bEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3bEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3bvEXT (const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3bvEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3bvEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3dEXT (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3dEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3dEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3dvEXT (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3dvEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3dvEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3fEXT (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3fEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3fEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3fvEXT (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3fvEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3fvEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3iEXT (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3iEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3iEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ivEXT (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ivEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3ivEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3sEXT (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3sEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3sEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3svEXT (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3svEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3svEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ubEXT (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ubEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3ubEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3ubvEXT (const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3ubvEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3ubvEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3uiEXT (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3uiEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3uiEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3uivEXT (const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3uivEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3uivEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3usEXT (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3usEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3usEXT, red, green, blue);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColor3usvEXT (const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColor3usvEXT", FindTask(NULL)));
    GLCALL(glSecondaryColor3usvEXT, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSecondaryColorPointerEXT (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSecondaryColorPointerEXT", FindTask(NULL)));
    GLCALL(glSecondaryColorPointerEXT, size, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiDrawArraysEXT (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiDrawArraysEXT", FindTask(NULL)));
    GLCALL(glMultiDrawArraysEXT, mode, first, count, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiDrawElementsEXT (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiDrawElementsEXT", FindTask(NULL)));
    GLCALL(glMultiDrawElementsEXT, mode, count, type, indices, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordfEXT (GLfloat coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordfEXT", FindTask(NULL)));
    GLCALL(glFogCoordfEXT, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordfvEXT (const GLfloat * coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordfvEXT", FindTask(NULL)));
    GLCALL(glFogCoordfvEXT, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoorddEXT (GLdouble coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoorddEXT", FindTask(NULL)));
    GLCALL(glFogCoorddEXT, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoorddvEXT (const GLdouble * coord)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoorddvEXT", FindTask(NULL)));
    GLCALL(glFogCoorddvEXT, coord);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFogCoordPointerEXT (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFogCoordPointerEXT", FindTask(NULL)));
    GLCALL(glFogCoordPointerEXT, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncSeparateEXT", FindTask(NULL)));
    GLCALL(glBlendFuncSeparateEXT, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFlushVertexArrayRangeNV ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFlushVertexArrayRangeNV", FindTask(NULL)));
    GLCALL(glFlushVertexArrayRangeNV);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexArrayRangeNV (GLsizei length, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexArrayRangeNV", FindTask(NULL)));
    GLCALL(glVertexArrayRangeNV, length, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerParameterfvNV (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerParameterfvNV", FindTask(NULL)));
    GLCALL(glCombinerParameterfvNV, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerParameterfNV (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerParameterfNV", FindTask(NULL)));
    GLCALL(glCombinerParameterfNV, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerParameterivNV (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerParameterivNV", FindTask(NULL)));
    GLCALL(glCombinerParameterivNV, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerParameteriNV (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerParameteriNV", FindTask(NULL)));
    GLCALL(glCombinerParameteriNV, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerInputNV (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerInputNV", FindTask(NULL)));
    GLCALL(glCombinerInputNV, stage, portion, variable, input, mapping, componentUsage);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCombinerOutputNV (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCombinerOutputNV", FindTask(NULL)));
    GLCALL(glCombinerOutputNV, stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFinalCombinerInputNV (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFinalCombinerInputNV", FindTask(NULL)));
    GLCALL(glFinalCombinerInputNV, variable, input, mapping, componentUsage);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCombinerInputParameterfvNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCombinerInputParameterfvNV", FindTask(NULL)));
    GLCALL(glGetCombinerInputParameterfvNV, stage, portion, variable, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCombinerInputParameterivNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCombinerInputParameterivNV", FindTask(NULL)));
    GLCALL(glGetCombinerInputParameterivNV, stage, portion, variable, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCombinerOutputParameterfvNV (GLenum stage, GLenum portion, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCombinerOutputParameterfvNV", FindTask(NULL)));
    GLCALL(glGetCombinerOutputParameterfvNV, stage, portion, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetCombinerOutputParameterivNV (GLenum stage, GLenum portion, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetCombinerOutputParameterivNV", FindTask(NULL)));
    GLCALL(glGetCombinerOutputParameterivNV, stage, portion, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetFinalCombinerInputParameterfvNV (GLenum variable, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFinalCombinerInputParameterfvNV", FindTask(NULL)));
    GLCALL(glGetFinalCombinerInputParameterfvNV, variable, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetFinalCombinerInputParameterivNV (GLenum variable, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFinalCombinerInputParameterivNV", FindTask(NULL)));
    GLCALL(glGetFinalCombinerInputParameterivNV, variable, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResizeBuffersMESA ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResizeBuffersMESA", FindTask(NULL)));
    GLCALL(glResizeBuffersMESA);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2dMESA (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2dMESA", FindTask(NULL)));
    GLCALL(glWindowPos2dMESA, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2dvMESA", FindTask(NULL)));
    GLCALL(glWindowPos2dvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2fMESA (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2fMESA", FindTask(NULL)));
    GLCALL(glWindowPos2fMESA, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2fvMESA", FindTask(NULL)));
    GLCALL(glWindowPos2fvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2iMESA (GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2iMESA", FindTask(NULL)));
    GLCALL(glWindowPos2iMESA, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2ivMESA (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2ivMESA", FindTask(NULL)));
    GLCALL(glWindowPos2ivMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2sMESA (GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2sMESA", FindTask(NULL)));
    GLCALL(glWindowPos2sMESA, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos2svMESA (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos2svMESA", FindTask(NULL)));
    GLCALL(glWindowPos2svMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3dMESA (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3dMESA", FindTask(NULL)));
    GLCALL(glWindowPos3dMESA, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3dvMESA", FindTask(NULL)));
    GLCALL(glWindowPos3dvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3fMESA (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3fMESA", FindTask(NULL)));
    GLCALL(glWindowPos3fMESA, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3fvMESA", FindTask(NULL)));
    GLCALL(glWindowPos3fvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3iMESA (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3iMESA", FindTask(NULL)));
    GLCALL(glWindowPos3iMESA, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3ivMESA (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3ivMESA", FindTask(NULL)));
    GLCALL(glWindowPos3ivMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3sMESA (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3sMESA", FindTask(NULL)));
    GLCALL(glWindowPos3sMESA, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos3svMESA (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos3svMESA", FindTask(NULL)));
    GLCALL(glWindowPos3svMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4dMESA (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4dMESA", FindTask(NULL)));
    GLCALL(glWindowPos4dMESA, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4dvMESA", FindTask(NULL)));
    GLCALL(glWindowPos4dvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4fMESA (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4fMESA", FindTask(NULL)));
    GLCALL(glWindowPos4fMESA, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4fvMESA", FindTask(NULL)));
    GLCALL(glWindowPos4fvMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4iMESA (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4iMESA", FindTask(NULL)));
    GLCALL(glWindowPos4iMESA, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4ivMESA (const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4ivMESA", FindTask(NULL)));
    GLCALL(glWindowPos4ivMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4sMESA (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4sMESA", FindTask(NULL)));
    GLCALL(glWindowPos4sMESA, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglWindowPos4svMESA (const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWindowPos4svMESA", FindTask(NULL)));
    GLCALL(glWindowPos4svMESA, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglAreProgramsResidentNV (GLsizei n, const GLuint * programs, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAreProgramsResidentNV", FindTask(NULL)));
    _ret = GLCALL(glAreProgramsResidentNV, n, programs, residences);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindProgramNV (GLenum target, GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindProgramNV", FindTask(NULL)));
    GLCALL(glBindProgramNV, target, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteProgramsNV (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteProgramsNV", FindTask(NULL)));
    GLCALL(glDeleteProgramsNV, n, programs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglExecuteProgramNV (GLenum target, GLuint id, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glExecuteProgramNV", FindTask(NULL)));
    GLCALL(glExecuteProgramNV, target, id, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenProgramsNV (GLsizei n, GLuint * programs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenProgramsNV", FindTask(NULL)));
    GLCALL(glGenProgramsNV, n, programs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramParameterdvNV (GLenum target, GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramParameterdvNV", FindTask(NULL)));
    GLCALL(glGetProgramParameterdvNV, target, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramParameterfvNV (GLenum target, GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramParameterfvNV", FindTask(NULL)));
    GLCALL(glGetProgramParameterfvNV, target, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramivNV (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramivNV", FindTask(NULL)));
    GLCALL(glGetProgramivNV, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramStringNV (GLuint id, GLenum pname, GLubyte * program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramStringNV", FindTask(NULL)));
    GLCALL(glGetProgramStringNV, id, pname, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTrackMatrixivNV (GLenum target, GLuint address, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTrackMatrixivNV", FindTask(NULL)));
    GLCALL(glGetTrackMatrixivNV, target, address, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribdvNV (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribdvNV", FindTask(NULL)));
    GLCALL(glGetVertexAttribdvNV, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribfvNV (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribfvNV", FindTask(NULL)));
    GLCALL(glGetVertexAttribfvNV, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribivNV (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribivNV", FindTask(NULL)));
    GLCALL(glGetVertexAttribivNV, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribPointervNV (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribPointervNV", FindTask(NULL)));
    GLCALL(glGetVertexAttribPointervNV, index, pname, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsProgramNV (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsProgramNV", FindTask(NULL)));
    _ret = GLCALL(glIsProgramNV, id);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglLoadProgramNV (GLenum target, GLuint id, GLsizei len, const GLubyte * program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glLoadProgramNV", FindTask(NULL)));
    GLCALL(glLoadProgramNV, target, id, len, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameter4dNV (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameter4dNV", FindTask(NULL)));
    GLCALL(glProgramParameter4dNV, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameter4dvNV (GLenum target, GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameter4dvNV", FindTask(NULL)));
    GLCALL(glProgramParameter4dvNV, target, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameter4fNV (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameter4fNV", FindTask(NULL)));
    GLCALL(glProgramParameter4fNV, target, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameter4fvNV (GLenum target, GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameter4fvNV", FindTask(NULL)));
    GLCALL(glProgramParameter4fvNV, target, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameters4dvNV (GLenum target, GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameters4dvNV", FindTask(NULL)));
    GLCALL(glProgramParameters4dvNV, target, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameters4fvNV (GLenum target, GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameters4fvNV", FindTask(NULL)));
    GLCALL(glProgramParameters4fvNV, target, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRequestResidentProgramsNV (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRequestResidentProgramsNV", FindTask(NULL)));
    GLCALL(glRequestResidentProgramsNV, n, programs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTrackMatrixNV (GLenum target, GLuint address, GLenum matrix, GLenum transform)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTrackMatrixNV", FindTask(NULL)));
    GLCALL(glTrackMatrixNV, target, address, matrix, transform);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribPointerNV (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribPointerNV", FindTask(NULL)));
    GLCALL(glVertexAttribPointerNV, index, fsize, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1dNV (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1dNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1dNV, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1dvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1dvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1fNV (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1fNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1fNV, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1fvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1fvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1sNV (GLuint index, GLshort x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1sNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1sNV, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib1svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib1svNV", FindTask(NULL)));
    GLCALL(glVertexAttrib1svNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2dNV (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2dNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2dNV, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2dvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2dvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2fNV (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2fNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2fNV, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2fvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2fvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2sNV (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2sNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2sNV, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib2svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib2svNV", FindTask(NULL)));
    GLCALL(glVertexAttrib2svNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3dNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3dNV, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3dvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3dvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3fNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3fNV, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3fvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3fvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3sNV (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3sNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3sNV, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib3svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib3svNV", FindTask(NULL)));
    GLCALL(glVertexAttrib3svNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4dNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4dNV, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4dvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4dvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4fNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4fNV, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4fvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4fvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4sNV (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4sNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4sNV, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4svNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4svNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4ubNV (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4ubNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4ubNV, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttrib4ubvNV (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttrib4ubvNV", FindTask(NULL)));
    GLCALL(glVertexAttrib4ubvNV, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs1dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs1dvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs1dvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs1fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs1fvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs1fvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs1svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs1svNV", FindTask(NULL)));
    GLCALL(glVertexAttribs1svNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs2dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs2dvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs2dvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs2fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs2fvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs2fvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs2svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs2svNV", FindTask(NULL)));
    GLCALL(glVertexAttribs2svNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs3dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs3dvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs3dvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs3fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs3fvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs3fvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs3svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs3svNV", FindTask(NULL)));
    GLCALL(glVertexAttribs3svNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs4dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs4dvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs4dvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs4fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs4fvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs4fvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs4svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs4svNV", FindTask(NULL)));
    GLCALL(glVertexAttribs4svNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribs4ubvNV (GLuint index, GLsizei count, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribs4ubvNV", FindTask(NULL)));
    GLCALL(glVertexAttribs4ubvNV, index, count, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexBumpParameterivATI (GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexBumpParameterivATI", FindTask(NULL)));
    GLCALL(glTexBumpParameterivATI, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexBumpParameterfvATI (GLenum pname, const GLfloat * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexBumpParameterfvATI", FindTask(NULL)));
    GLCALL(glTexBumpParameterfvATI, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexBumpParameterivATI (GLenum pname, GLint * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexBumpParameterivATI", FindTask(NULL)));
    GLCALL(glGetTexBumpParameterivATI, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexBumpParameterfvATI (GLenum pname, GLfloat * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexBumpParameterfvATI", FindTask(NULL)));
    GLCALL(glGetTexBumpParameterfvATI, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLuint mglGenFragmentShadersATI (GLuint range)
{
    GLuint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenFragmentShadersATI", FindTask(NULL)));
    _ret = GLCALL(glGenFragmentShadersATI, range);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindFragmentShaderATI (GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindFragmentShaderATI", FindTask(NULL)));
    GLCALL(glBindFragmentShaderATI, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteFragmentShaderATI (GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteFragmentShaderATI", FindTask(NULL)));
    GLCALL(glDeleteFragmentShaderATI, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBeginFragmentShaderATI ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginFragmentShaderATI", FindTask(NULL)));
    GLCALL(glBeginFragmentShaderATI);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndFragmentShaderATI ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndFragmentShaderATI", FindTask(NULL)));
    GLCALL(glEndFragmentShaderATI);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPassTexCoordATI (GLuint dst, GLuint coord, GLenum swizzle)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPassTexCoordATI", FindTask(NULL)));
    GLCALL(glPassTexCoordATI, dst, coord, swizzle);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSampleMapATI (GLuint dst, GLuint interp, GLenum swizzle)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSampleMapATI", FindTask(NULL)));
    GLCALL(glSampleMapATI, dst, interp, swizzle);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorFragmentOp1ATI", FindTask(NULL)));
    GLCALL(glColorFragmentOp1ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorFragmentOp2ATI", FindTask(NULL)));
    GLCALL(glColorFragmentOp2ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorFragmentOp3ATI", FindTask(NULL)));
    GLCALL(glColorFragmentOp3ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAlphaFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAlphaFragmentOp1ATI", FindTask(NULL)));
    GLCALL(glAlphaFragmentOp1ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAlphaFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAlphaFragmentOp2ATI", FindTask(NULL)));
    GLCALL(glAlphaFragmentOp2ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglAlphaFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glAlphaFragmentOp3ATI", FindTask(NULL)));
    GLCALL(glAlphaFragmentOp3ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSetFragmentShaderConstantATI (GLuint dst, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSetFragmentShaderConstantATI", FindTask(NULL)));
    GLCALL(glSetFragmentShaderConstantATI, dst, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameteriNV (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameteriNV", FindTask(NULL)));
    GLCALL(glPointParameteriNV, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterivNV (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterivNV", FindTask(NULL)));
    GLCALL(glPointParameterivNV, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawBuffersATI (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawBuffersATI", FindTask(NULL)));
    GLCALL(glDrawBuffersATI, n, bufs);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramNamedParameter4fNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramNamedParameter4fNV", FindTask(NULL)));
    GLCALL(glProgramNamedParameter4fNV, id, len, name, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramNamedParameter4dNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramNamedParameter4dNV", FindTask(NULL)));
    GLCALL(glProgramNamedParameter4dNV, id, len, name, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramNamedParameter4fvNV (GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramNamedParameter4fvNV", FindTask(NULL)));
    GLCALL(glProgramNamedParameter4fvNV, id, len, name, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramNamedParameter4dvNV (GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramNamedParameter4dvNV", FindTask(NULL)));
    GLCALL(glProgramNamedParameter4dvNV, id, len, name, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramNamedParameterfvNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramNamedParameterfvNV", FindTask(NULL)));
    GLCALL(glGetProgramNamedParameterfvNV, id, len, name, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetProgramNamedParameterdvNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetProgramNamedParameterdvNV", FindTask(NULL)));
    GLCALL(glGetProgramNamedParameterdvNV, id, len, name, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsRenderbufferEXT (GLuint renderbuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsRenderbufferEXT", FindTask(NULL)));
    _ret = GLCALL(glIsRenderbufferEXT, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindRenderbufferEXT", FindTask(NULL)));
    GLCALL(glBindRenderbufferEXT, target, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteRenderbuffersEXT (GLsizei n, const GLuint * renderbuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteRenderbuffersEXT", FindTask(NULL)));
    GLCALL(glDeleteRenderbuffersEXT, n, renderbuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenRenderbuffersEXT (GLsizei n, GLuint * renderbuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenRenderbuffersEXT", FindTask(NULL)));
    GLCALL(glGenRenderbuffersEXT, n, renderbuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRenderbufferStorageEXT", FindTask(NULL)));
    GLCALL(glRenderbufferStorageEXT, target, internalformat, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetRenderbufferParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetRenderbufferParameterivEXT", FindTask(NULL)));
    GLCALL(glGetRenderbufferParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsFramebufferEXT (GLuint framebuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsFramebufferEXT", FindTask(NULL)));
    _ret = GLCALL(glIsFramebufferEXT, framebuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindFramebufferEXT", FindTask(NULL)));
    GLCALL(glBindFramebufferEXT, target, framebuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteFramebuffersEXT (GLsizei n, const GLuint * framebuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteFramebuffersEXT", FindTask(NULL)));
    GLCALL(glDeleteFramebuffersEXT, n, framebuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenFramebuffersEXT (GLsizei n, GLuint * framebuffers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenFramebuffersEXT", FindTask(NULL)));
    GLCALL(glGenFramebuffersEXT, n, framebuffers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLenum mglCheckFramebufferStatusEXT (GLenum target)
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCheckFramebufferStatusEXT", FindTask(NULL)));
    _ret = GLCALL(glCheckFramebufferStatusEXT, target);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglFramebufferTexture1DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture1DEXT", FindTask(NULL)));
    GLCALL(glFramebufferTexture1DEXT, target, attachment, textarget, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture2DEXT", FindTask(NULL)));
    GLCALL(glFramebufferTexture2DEXT, target, attachment, textarget, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTexture3DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture3DEXT", FindTask(NULL)));
    GLCALL(glFramebufferTexture3DEXT, target, attachment, textarget, texture, level, zoffset);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferRenderbufferEXT", FindTask(NULL)));
    GLCALL(glFramebufferRenderbufferEXT, target, attachment, renderbuffertarget, renderbuffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetFramebufferAttachmentParameterivEXT (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFramebufferAttachmentParameterivEXT", FindTask(NULL)));
    GLCALL(glGetFramebufferAttachmentParameterivEXT, target, attachment, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenerateMipmapEXT (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenerateMipmapEXT", FindTask(NULL)));
    GLCALL(glGenerateMipmapEXT, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTextureLayerEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTextureLayerEXT", FindTask(NULL)));
    GLCALL(glFramebufferTextureLayerEXT, target, attachment, texture, level, layer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLvoid* mglMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMapBufferRange", FindTask(NULL)));
    _ret = GLCALL(glMapBufferRange, target, offset, length, access);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFlushMappedBufferRange", FindTask(NULL)));
    GLCALL(glFlushMappedBufferRange, target, offset, length);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindVertexArray (GLuint array)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindVertexArray", FindTask(NULL)));
    GLCALL(glBindVertexArray, array);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteVertexArrays (GLsizei n, const GLuint * arrays)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteVertexArrays", FindTask(NULL)));
    GLCALL(glDeleteVertexArrays, n, arrays);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenVertexArrays (GLsizei n, GLuint * arrays)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenVertexArrays", FindTask(NULL)));
    GLCALL(glGenVertexArrays, n, arrays);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsVertexArray (GLuint array)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsVertexArray", FindTask(NULL)));
    _ret = GLCALL(glIsVertexArray, array);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyBufferSubData", FindTask(NULL)));
    GLCALL(glCopyBufferSubData, readTarget, writeTarget, readOffset, writeOffset, size);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLsync mglFenceSync (GLenum condition, GLbitfield flags)
{
    GLsync _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFenceSync", FindTask(NULL)));
    _ret = GLCALL(glFenceSync, condition, flags);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsSync (GLsync sync)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsSync", FindTask(NULL)));
    _ret = GLCALL(glIsSync, sync);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglDeleteSync (GLsync sync)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteSync", FindTask(NULL)));
    GLCALL(glDeleteSync, sync);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLenum mglClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClientWaitSync", FindTask(NULL)));
    _ret = GLCALL(glClientWaitSync, sync, flags, timeout);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glWaitSync", FindTask(NULL)));
    GLCALL(glWaitSync, sync, flags, timeout);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetInteger64v (GLenum pname, GLint64 * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetInteger64v", FindTask(NULL)));
    GLCALL(glGetInteger64v, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSynciv", FindTask(NULL)));
    GLCALL(glGetSynciv, sync, pname, bufSize, length, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProvokingVertexEXT (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProvokingVertexEXT", FindTask(NULL)));
    GLCALL(glProvokingVertexEXT, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElementsBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElementsBaseVertex", FindTask(NULL)));
    GLCALL(glDrawElementsBaseVertex, mode, count, type, indices, basevertex);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawRangeElementsBaseVertex (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawRangeElementsBaseVertex", FindTask(NULL)));
    GLCALL(glDrawRangeElementsBaseVertex, mode, start, end, count, type, indices, basevertex);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiDrawElementsBaseVertex (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount, const GLint * basevertex)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiDrawElementsBaseVertex", FindTask(NULL)));
    GLCALL(glMultiDrawElementsBaseVertex, mode, count, type, indices, primcount, basevertex);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProvokingVertex (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProvokingVertex", FindTask(NULL)));
    GLCALL(glProvokingVertex, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglRenderbufferStorageMultisampleEXT (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glRenderbufferStorageMultisampleEXT", FindTask(NULL)));
    GLCALL(glRenderbufferStorageMultisampleEXT, target, samples, internalformat, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorMaskIndexedEXT (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorMaskIndexedEXT", FindTask(NULL)));
    GLCALL(glColorMaskIndexedEXT, index, r, g, b, a);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBooleanIndexedvEXT (GLenum target, GLuint index, GLboolean * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBooleanIndexedvEXT", FindTask(NULL)));
    GLCALL(glGetBooleanIndexedvEXT, target, index, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetIntegerIndexedvEXT (GLenum target, GLuint index, GLint * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetIntegerIndexedvEXT", FindTask(NULL)));
    GLCALL(glGetIntegerIndexedvEXT, target, index, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEnableIndexedEXT (GLenum target, GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnableIndexedEXT", FindTask(NULL)));
    GLCALL(glEnableIndexedEXT, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisableIndexedEXT (GLenum target, GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisableIndexedEXT", FindTask(NULL)));
    GLCALL(glDisableIndexedEXT, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsEnabledIndexedEXT (GLenum target, GLuint index)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsEnabledIndexedEXT", FindTask(NULL)));
    _ret = GLCALL(glIsEnabledIndexedEXT, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBeginConditionalRenderNV (GLuint id, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginConditionalRenderNV", FindTask(NULL)));
    GLCALL(glBeginConditionalRenderNV, id, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndConditionalRenderNV ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndConditionalRenderNV", FindTask(NULL)));
    GLCALL(glEndConditionalRenderNV);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLenum mglObjectPurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glObjectPurgeableAPPLE", FindTask(NULL)));
    _ret = GLCALL(glObjectPurgeableAPPLE, objectType, name, option);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLenum mglObjectUnpurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glObjectUnpurgeableAPPLE", FindTask(NULL)));
    _ret = GLCALL(glObjectUnpurgeableAPPLE, objectType, name, option);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetObjectParameterivAPPLE (GLenum objectType, GLuint name, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetObjectParameterivAPPLE", FindTask(NULL)));
    GLCALL(glGetObjectParameterivAPPLE, objectType, name, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBeginTransformFeedback (GLenum primitiveMode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginTransformFeedback", FindTask(NULL)));
    GLCALL(glBeginTransformFeedback, primitiveMode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndTransformFeedback ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndTransformFeedback", FindTask(NULL)));
    GLCALL(glEndTransformFeedback);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferRange", FindTask(NULL)));
    GLCALL(glBindBufferRange, target, index, buffer, offset, size);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBufferBase (GLenum target, GLuint index, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferBase", FindTask(NULL)));
    GLCALL(glBindBufferBase, target, index, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTransformFeedbackVaryings", FindTask(NULL)));
    GLCALL(glTransformFeedbackVaryings, program, count, varyings, bufferMode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTransformFeedbackVarying", FindTask(NULL)));
    GLCALL(glGetTransformFeedbackVarying, program, index, bufSize, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawArraysInstanced", FindTask(NULL)));
    GLCALL(glDrawArraysInstanced, mode, first, count, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElementsInstanced", FindTask(NULL)));
    GLCALL(glDrawElementsInstanced, mode, count, type, indices, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawArraysInstancedARB (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawArraysInstancedARB", FindTask(NULL)));
    GLCALL(glDrawArraysInstancedARB, mode, first, count, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElementsInstancedARB (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElementsInstancedARB", FindTask(NULL)));
    GLCALL(glDrawElementsInstancedARB, mode, count, type, indices, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramParameteriARB (GLuint program, GLenum pname, GLint value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramParameteriARB", FindTask(NULL)));
    GLCALL(glProgramParameteriARB, program, pname, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTextureARB (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTextureARB", FindTask(NULL)));
    GLCALL(glFramebufferTextureARB, target, attachment, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTextureFaceARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTextureFaceARB", FindTask(NULL)));
    GLCALL(glFramebufferTextureFaceARB, target, attachment, texture, level, face);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindTransformFeedback (GLenum target, GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindTransformFeedback", FindTask(NULL)));
    GLCALL(glBindTransformFeedback, target, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteTransformFeedbacks (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteTransformFeedbacks", FindTask(NULL)));
    GLCALL(glDeleteTransformFeedbacks, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenTransformFeedbacks (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenTransformFeedbacks", FindTask(NULL)));
    GLCALL(glGenTransformFeedbacks, n, ids);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsTransformFeedback (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsTransformFeedback", FindTask(NULL)));
    _ret = GLCALL(glIsTransformFeedback, id);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglPauseTransformFeedback ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPauseTransformFeedback", FindTask(NULL)));
    GLCALL(glPauseTransformFeedback);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResumeTransformFeedback ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResumeTransformFeedback", FindTask(NULL)));
    GLCALL(glResumeTransformFeedback);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawTransformFeedback (GLenum mode, GLuint id)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawTransformFeedback", FindTask(NULL)));
    GLCALL(glDrawTransformFeedback, mode, id);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawArraysInstancedEXT (GLenum mode, GLint start, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawArraysInstancedEXT", FindTask(NULL)));
    GLCALL(glDrawArraysInstancedEXT, mode, start, count, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElementsInstancedEXT (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElementsInstancedEXT", FindTask(NULL)));
    GLCALL(glDrawElementsInstancedEXT, mode, count, type, indices, primcount);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBeginTransformFeedbackEXT (GLenum primitiveMode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginTransformFeedbackEXT", FindTask(NULL)));
    GLCALL(glBeginTransformFeedbackEXT, primitiveMode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndTransformFeedbackEXT ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndTransformFeedbackEXT", FindTask(NULL)));
    GLCALL(glEndTransformFeedbackEXT);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBufferRangeEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferRangeEXT", FindTask(NULL)));
    GLCALL(glBindBufferRangeEXT, target, index, buffer, offset, size);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBufferOffsetEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferOffsetEXT", FindTask(NULL)));
    GLCALL(glBindBufferOffsetEXT, target, index, buffer, offset);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindBufferBaseEXT (GLenum target, GLuint index, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindBufferBaseEXT", FindTask(NULL)));
    GLCALL(glBindBufferBaseEXT, target, index, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTransformFeedbackVaryingsEXT (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTransformFeedbackVaryingsEXT", FindTask(NULL)));
    GLCALL(glTransformFeedbackVaryingsEXT, program, count, varyings, bufferMode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTransformFeedbackVaryingEXT (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTransformFeedbackVaryingEXT", FindTask(NULL)));
    GLCALL(glGetTransformFeedbackVaryingEXT, program, index, bufSize, length, size, type, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEGLImageTargetTexture2DOES", FindTask(NULL)));
    GLCALL(glEGLImageTargetTexture2DOES, target, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEGLImageTargetRenderbufferStorageOES", FindTask(NULL)));
    GLCALL(glEGLImageTargetRenderbufferStorageOES, target, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorMaski", FindTask(NULL)));
    GLCALL(glColorMaski, index, r, g, b, a);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBooleani_v (GLenum target, GLuint index, GLboolean * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBooleani_v", FindTask(NULL)));
    GLCALL(glGetBooleani_v, target, index, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetIntegeri_v (GLenum target, GLuint index, GLint * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetIntegeri_v", FindTask(NULL)));
    GLCALL(glGetIntegeri_v, target, index, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEnablei (GLenum target, GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEnablei", FindTask(NULL)));
    GLCALL(glEnablei, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDisablei (GLenum target, GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDisablei", FindTask(NULL)));
    GLCALL(glDisablei, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsEnabledi (GLenum target, GLuint index)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsEnabledi", FindTask(NULL)));
    _ret = GLCALL(glIsEnabledi, target, index);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglClampColor (GLenum target, GLenum clamp)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClampColor", FindTask(NULL)));
    GLCALL(glClampColor, target, clamp);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBeginConditionalRender (GLuint id, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBeginConditionalRender", FindTask(NULL)));
    GLCALL(glBeginConditionalRender, id, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglEndConditionalRender ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glEndConditionalRender", FindTask(NULL)));
    GLCALL(glEndConditionalRender);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribIPointer", FindTask(NULL)));
    GLCALL(glVertexAttribIPointer, index, size, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribIiv (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribIiv", FindTask(NULL)));
    GLCALL(glGetVertexAttribIiv, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribIuiv", FindTask(NULL)));
    GLCALL(glGetVertexAttribIuiv, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1i (GLuint index, GLint x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1i", FindTask(NULL)));
    GLCALL(glVertexAttribI1i, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2i (GLuint index, GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2i", FindTask(NULL)));
    GLCALL(glVertexAttribI2i, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3i", FindTask(NULL)));
    GLCALL(glVertexAttribI3i, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4i", FindTask(NULL)));
    GLCALL(glVertexAttribI4i, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1ui (GLuint index, GLuint x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1ui", FindTask(NULL)));
    GLCALL(glVertexAttribI1ui, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2ui (GLuint index, GLuint x, GLuint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2ui", FindTask(NULL)));
    GLCALL(glVertexAttribI2ui, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3ui", FindTask(NULL)));
    GLCALL(glVertexAttribI3ui, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4ui", FindTask(NULL)));
    GLCALL(glVertexAttribI4ui, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1iv", FindTask(NULL)));
    GLCALL(glVertexAttribI1iv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2iv", FindTask(NULL)));
    GLCALL(glVertexAttribI2iv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3iv", FindTask(NULL)));
    GLCALL(glVertexAttribI3iv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4iv", FindTask(NULL)));
    GLCALL(glVertexAttribI4iv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1uiv", FindTask(NULL)));
    GLCALL(glVertexAttribI1uiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2uiv", FindTask(NULL)));
    GLCALL(glVertexAttribI2uiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3uiv", FindTask(NULL)));
    GLCALL(glVertexAttribI3uiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4uiv", FindTask(NULL)));
    GLCALL(glVertexAttribI4uiv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4bv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4bv", FindTask(NULL)));
    GLCALL(glVertexAttribI4bv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4sv", FindTask(NULL)));
    GLCALL(glVertexAttribI4sv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4ubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4ubv", FindTask(NULL)));
    GLCALL(glVertexAttribI4ubv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4usv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4usv", FindTask(NULL)));
    GLCALL(glVertexAttribI4usv, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetUniformuiv (GLuint program, GLint location, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformuiv", FindTask(NULL)));
    GLCALL(glGetUniformuiv, program, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindFragDataLocation (GLuint program, GLuint color, const GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindFragDataLocation", FindTask(NULL)));
    GLCALL(glBindFragDataLocation, program, color, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetFragDataLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFragDataLocation", FindTask(NULL)));
    _ret = GLCALL(glGetFragDataLocation, program, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglUniform1ui (GLint location, GLuint v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1ui", FindTask(NULL)));
    GLCALL(glUniform1ui, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2ui (GLint location, GLuint v0, GLuint v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2ui", FindTask(NULL)));
    GLCALL(glUniform2ui, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3ui", FindTask(NULL)));
    GLCALL(glUniform3ui, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4ui", FindTask(NULL)));
    GLCALL(glUniform4ui, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1uiv", FindTask(NULL)));
    GLCALL(glUniform1uiv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2uiv", FindTask(NULL)));
    GLCALL(glUniform2uiv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3uiv", FindTask(NULL)));
    GLCALL(glUniform3uiv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4uiv", FindTask(NULL)));
    GLCALL(glUniform4uiv, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterIiv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterIiv", FindTask(NULL)));
    GLCALL(glTexParameterIiv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterIuiv (GLenum target, GLenum pname, const GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterIuiv", FindTask(NULL)));
    GLCALL(glTexParameterIuiv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterIiv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterIiv", FindTask(NULL)));
    GLCALL(glGetTexParameterIiv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterIuiv (GLenum target, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterIuiv", FindTask(NULL)));
    GLCALL(glGetTexParameterIuiv, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearBufferiv", FindTask(NULL)));
    GLCALL(glClearBufferiv, buffer, drawbuffer, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearBufferuiv", FindTask(NULL)));
    GLCALL(glClearBufferuiv, buffer, drawbuffer, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearBufferfv", FindTask(NULL)));
    GLCALL(glClearBufferfv, buffer, drawbuffer, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearBufferfi", FindTask(NULL)));
    GLCALL(glClearBufferfi, buffer, drawbuffer, depth, stencil);
    D(bug("...exit\n"));
    HOSTGL_POST
}

const GLubyte * mglGetStringi (GLenum name, GLuint index)
{
    const GLubyte * _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetStringi", FindTask(NULL)));
    _ret = GLCALL(glGetStringi, name, index);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglTexBuffer (GLenum target, GLenum internalformat, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexBuffer", FindTask(NULL)));
    GLCALL(glTexBuffer, target, internalformat, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPrimitiveRestartIndex (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPrimitiveRestartIndex", FindTask(NULL)));
    GLCALL(glPrimitiveRestartIndex, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetInteger64i_v (GLenum target, GLuint index, GLint64 * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetInteger64i_v", FindTask(NULL)));
    GLCALL(glGetInteger64i_v, target, index, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetBufferParameteri64v", FindTask(NULL)));
    GLCALL(glGetBufferParameteri64v, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTexture", FindTask(NULL)));
    GLCALL(glFramebufferTexture, target, attachment, texture, level);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribDivisor (GLuint index, GLuint divisor)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribDivisor", FindTask(NULL)));
    GLCALL(glVertexAttribDivisor, index, divisor);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPrimitiveRestartNV ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPrimitiveRestartNV", FindTask(NULL)));
    GLCALL(glPrimitiveRestartNV);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPrimitiveRestartIndexNV (GLuint index)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPrimitiveRestartIndexNV", FindTask(NULL)));
    GLCALL(glPrimitiveRestartIndexNV, index);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1iEXT (GLuint index, GLint x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1iEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI1iEXT, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2iEXT (GLuint index, GLint x, GLint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2iEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI2iEXT, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3iEXT (GLuint index, GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3iEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI3iEXT, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4iEXT (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4iEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4iEXT, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1uiEXT (GLuint index, GLuint x)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1uiEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI1uiEXT, index, x);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2uiEXT (GLuint index, GLuint x, GLuint y)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2uiEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI2uiEXT, index, x, y);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3uiEXT (GLuint index, GLuint x, GLuint y, GLuint z)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3uiEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI3uiEXT, index, x, y, z);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4uiEXT (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4uiEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4uiEXT, index, x, y, z, w);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1ivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI1ivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2ivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI2ivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3ivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI3ivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4ivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4ivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI1uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI1uivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI1uivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI2uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI2uivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI2uivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI3uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI3uivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI3uivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4uivEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4uivEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4bvEXT (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4bvEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4bvEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4svEXT (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4svEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4svEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4ubvEXT (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4ubvEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4ubvEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribI4usvEXT (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribI4usvEXT", FindTask(NULL)));
    GLCALL(glVertexAttribI4usvEXT, index, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribIPointerEXT (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribIPointerEXT", FindTask(NULL)));
    GLCALL(glVertexAttribIPointerEXT, index, size, type, stride, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribIivEXT (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribIivEXT", FindTask(NULL)));
    GLCALL(glGetVertexAttribIivEXT, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetVertexAttribIuivEXT (GLuint index, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetVertexAttribIuivEXT", FindTask(NULL)));
    GLCALL(glGetVertexAttribIuivEXT, index, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetUniformuivEXT (GLuint program, GLint location, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetUniformuivEXT", FindTask(NULL)));
    GLCALL(glGetUniformuivEXT, program, location, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindFragDataLocationEXT (GLuint program, GLuint color, const GLchar * name)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindFragDataLocationEXT", FindTask(NULL)));
    GLCALL(glBindFragDataLocationEXT, program, color, name);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLint mglGetFragDataLocationEXT (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFragDataLocationEXT", FindTask(NULL)));
    _ret = GLCALL(glGetFragDataLocationEXT, program, name);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglUniform1uiEXT (GLint location, GLuint v0)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1uiEXT", FindTask(NULL)));
    GLCALL(glUniform1uiEXT, location, v0);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2uiEXT (GLint location, GLuint v0, GLuint v1)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2uiEXT", FindTask(NULL)));
    GLCALL(glUniform2uiEXT, location, v0, v1);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3uiEXT", FindTask(NULL)));
    GLCALL(glUniform3uiEXT, location, v0, v1, v2);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4uiEXT", FindTask(NULL)));
    GLCALL(glUniform4uiEXT, location, v0, v1, v2, v3);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform1uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform1uivEXT", FindTask(NULL)));
    GLCALL(glUniform1uivEXT, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform2uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform2uivEXT", FindTask(NULL)));
    GLCALL(glUniform2uivEXT, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform3uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform3uivEXT", FindTask(NULL)));
    GLCALL(glUniform3uivEXT, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUniform4uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUniform4uivEXT", FindTask(NULL)));
    GLCALL(glUniform4uivEXT, location, count, value);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterIivEXT (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterIivEXT", FindTask(NULL)));
    GLCALL(glTexParameterIivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexParameterIuivEXT (GLenum target, GLenum pname, const GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexParameterIuivEXT", FindTask(NULL)));
    GLCALL(glTexParameterIuivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterIivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterIivEXT", FindTask(NULL)));
    GLCALL(glGetTexParameterIivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterIuivEXT (GLenum target, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterIuivEXT", FindTask(NULL)));
    GLCALL(glGetTexParameterIuivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearColorIiEXT (GLint red, GLint green, GLint blue, GLint alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearColorIiEXT", FindTask(NULL)));
    GLCALL(glClearColorIiEXT, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearColorIuiEXT (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearColorIuiEXT", FindTask(NULL)));
    GLCALL(glClearColorIuiEXT, red, green, blue, alpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglUseShaderProgramEXT (GLenum type, GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glUseShaderProgramEXT", FindTask(NULL)));
    GLCALL(glUseShaderProgramEXT, type, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglActiveProgramEXT (GLuint program)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glActiveProgramEXT", FindTask(NULL)));
    GLCALL(glActiveProgramEXT, program);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLuint mglCreateShaderProgramEXT (GLenum type, const GLchar * string)
{
    GLuint _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCreateShaderProgramEXT", FindTask(NULL)));
    _ret = GLCALL(glCreateShaderProgramEXT, type, string);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglProgramEnvParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramEnvParameters4fvEXT", FindTask(NULL)));
    GLCALL(glProgramEnvParameters4fvEXT, target, index, count, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglProgramLocalParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glProgramLocalParameters4fvEXT", FindTask(NULL)));
    GLCALL(glProgramLocalParameters4fvEXT, target, index, count, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationSeparateATI (GLenum modeRGB, GLenum modeA)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationSeparateATI", FindTask(NULL)));
    GLCALL(glBlendEquationSeparateATI, modeRGB, modeA);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogramEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogramEXT", FindTask(NULL)));
    GLCALL(glGetHistogramEXT, target, reset, format, type, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogramParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogramParameterfvEXT", FindTask(NULL)));
    GLCALL(glGetHistogramParameterfvEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetHistogramParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetHistogramParameterivEXT", FindTask(NULL)));
    GLCALL(glGetHistogramParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmaxEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmaxEXT", FindTask(NULL)));
    GLCALL(glGetMinmaxEXT, target, reset, format, type, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmaxParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmaxParameterfvEXT", FindTask(NULL)));
    GLCALL(glGetMinmaxParameterfvEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetMinmaxParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetMinmaxParameterivEXT", FindTask(NULL)));
    GLCALL(glGetMinmaxParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglHistogramEXT (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glHistogramEXT", FindTask(NULL)));
    GLCALL(glHistogramEXT, target, width, internalformat, sink);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMinmaxEXT (GLenum target, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMinmaxEXT", FindTask(NULL)));
    GLCALL(glMinmaxEXT, target, internalformat, sink);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResetHistogramEXT (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResetHistogramEXT", FindTask(NULL)));
    GLCALL(glResetHistogramEXT, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglResetMinmaxEXT (GLenum target)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glResetMinmaxEXT", FindTask(NULL)));
    GLCALL(glResetMinmaxEXT, target);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionFilter1DEXT", FindTask(NULL)));
    GLCALL(glConvolutionFilter1DEXT, target, internalformat, width, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionFilter2DEXT", FindTask(NULL)));
    GLCALL(glConvolutionFilter2DEXT, target, internalformat, width, height, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameterfEXT (GLenum target, GLenum pname, GLfloat params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameterfEXT", FindTask(NULL)));
    GLCALL(glConvolutionParameterfEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameterfvEXT (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameterfvEXT", FindTask(NULL)));
    GLCALL(glConvolutionParameterfvEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameteriEXT (GLenum target, GLenum pname, GLint params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameteriEXT", FindTask(NULL)));
    GLCALL(glConvolutionParameteriEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglConvolutionParameterivEXT (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glConvolutionParameterivEXT", FindTask(NULL)));
    GLCALL(glConvolutionParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyConvolutionFilter1DEXT", FindTask(NULL)));
    GLCALL(glCopyConvolutionFilter1DEXT, target, internalformat, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyConvolutionFilter2DEXT", FindTask(NULL)));
    GLCALL(glCopyConvolutionFilter2DEXT, target, internalformat, x, y, width, height);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionFilterEXT", FindTask(NULL)));
    GLCALL(glGetConvolutionFilterEXT, target, format, type, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionParameterfvEXT", FindTask(NULL)));
    GLCALL(glGetConvolutionParameterfvEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetConvolutionParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetConvolutionParameterivEXT", FindTask(NULL)));
    GLCALL(glGetConvolutionParameterivEXT, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSeparableFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSeparableFilterEXT", FindTask(NULL)));
    GLCALL(glGetSeparableFilterEXT, target, format, type, row, column, span);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSeparableFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSeparableFilter2DEXT", FindTask(NULL)));
    GLCALL(glSeparableFilter2DEXT, target, internalformat, width, height, format, type, row, column);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableSGI (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableSGI", FindTask(NULL)));
    GLCALL(glColorTableSGI, target, internalformat, width, format, type, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableParameterfvSGI (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableParameterfvSGI", FindTask(NULL)));
    GLCALL(glColorTableParameterfvSGI, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorTableParameterivSGI (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorTableParameterivSGI", FindTask(NULL)));
    GLCALL(glColorTableParameterivSGI, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyColorTableSGI (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyColorTableSGI", FindTask(NULL)));
    GLCALL(glCopyColorTableSGI, target, internalformat, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableSGI (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableSGI", FindTask(NULL)));
    GLCALL(glGetColorTableSGI, target, format, type, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameterfvSGI (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameterfvSGI", FindTask(NULL)));
    GLCALL(glGetColorTableParameterfvSGI, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetColorTableParameterivSGI (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetColorTableParameterivSGI", FindTask(NULL)));
    GLCALL(glGetColorTableParameterivSGI, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTexGenSGIX (GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTexGenSGIX", FindTask(NULL)));
    GLCALL(glPixelTexGenSGIX, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTexGenParameteriSGIS (GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTexGenParameteriSGIS", FindTask(NULL)));
    GLCALL(glPixelTexGenParameteriSGIS, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTexGenParameterivSGIS (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTexGenParameterivSGIS", FindTask(NULL)));
    GLCALL(glPixelTexGenParameterivSGIS, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTexGenParameterfSGIS (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTexGenParameterfSGIS", FindTask(NULL)));
    GLCALL(glPixelTexGenParameterfSGIS, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPixelTexGenParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPixelTexGenParameterfvSGIS", FindTask(NULL)));
    GLCALL(glPixelTexGenParameterfvSGIS, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPixelTexGenParameterivSGIS (GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPixelTexGenParameterivSGIS", FindTask(NULL)));
    GLCALL(glGetPixelTexGenParameterivSGIS, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetPixelTexGenParameterfvSGIS (GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetPixelTexGenParameterfvSGIS", FindTask(NULL)));
    GLCALL(glGetPixelTexGenParameterfvSGIS, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSampleMaskSGIS (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSampleMaskSGIS", FindTask(NULL)));
    GLCALL(glSampleMaskSGIS, value, invert);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplePatternSGIS (GLenum pattern)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplePatternSGIS", FindTask(NULL)));
    GLCALL(glSamplePatternSGIS, pattern);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfSGIS (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfSGIS", FindTask(NULL)));
    GLCALL(glPointParameterfSGIS, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglPointParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glPointParameterfvSGIS", FindTask(NULL)));
    GLCALL(glPointParameterfvSGIS, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglColorSubTableEXT (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glColorSubTableEXT", FindTask(NULL)));
    GLCALL(glColorSubTableEXT, target, start, count, format, type, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglCopyColorSubTableEXT (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glCopyColorSubTableEXT", FindTask(NULL)));
    GLCALL(glCopyColorSubTableEXT, target, start, x, y, width);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncSeparateINGR (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncSeparateINGR", FindTask(NULL)));
    GLCALL(glBlendFuncSeparateINGR, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiModeDrawArraysIBM (const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiModeDrawArraysIBM", FindTask(NULL)));
    GLCALL(glMultiModeDrawArraysIBM, mode, first, count, primcount, modestride);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglMultiModeDrawElementsIBM (const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glMultiModeDrawElementsIBM", FindTask(NULL)));
    GLCALL(glMultiModeDrawElementsIBM, mode, count, type, indices, primcount, modestride);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSampleMaskEXT (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSampleMaskEXT", FindTask(NULL)));
    GLCALL(glSampleMaskEXT, value, invert);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplePatternEXT (GLenum pattern)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplePatternEXT", FindTask(NULL)));
    GLCALL(glSamplePatternEXT, pattern);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteFencesNV (GLsizei n, const GLuint * fences)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteFencesNV", FindTask(NULL)));
    GLCALL(glDeleteFencesNV, n, fences);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenFencesNV (GLsizei n, GLuint * fences)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenFencesNV", FindTask(NULL)));
    GLCALL(glGenFencesNV, n, fences);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsFenceNV (GLuint fence)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsFenceNV", FindTask(NULL)));
    _ret = GLCALL(glIsFenceNV, fence);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

GLboolean mglTestFenceNV (GLuint fence)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTestFenceNV", FindTask(NULL)));
    _ret = GLCALL(glTestFenceNV, fence);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetFenceivNV (GLuint fence, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetFenceivNV", FindTask(NULL)));
    GLCALL(glGetFenceivNV, fence, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFinishFenceNV (GLuint fence)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFinishFenceNV", FindTask(NULL)));
    GLCALL(glFinishFenceNV, fence);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSetFenceNV (GLuint fence, GLenum condition)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSetFenceNV", FindTask(NULL)));
    GLCALL(glSetFenceNV, fence, condition);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglActiveStencilFaceEXT (GLenum face)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glActiveStencilFaceEXT", FindTask(NULL)));
    GLCALL(glActiveStencilFaceEXT, face);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBindVertexArrayAPPLE (GLuint array)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindVertexArrayAPPLE", FindTask(NULL)));
    GLCALL(glBindVertexArrayAPPLE, array);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteVertexArraysAPPLE (GLsizei n, const GLuint * arrays)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteVertexArraysAPPLE", FindTask(NULL)));
    GLCALL(glDeleteVertexArraysAPPLE, n, arrays);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenVertexArraysAPPLE (GLsizei n, GLuint * arrays)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenVertexArraysAPPLE", FindTask(NULL)));
    GLCALL(glGenVertexArraysAPPLE, n, arrays);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsVertexArrayAPPLE (GLuint array)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsVertexArrayAPPLE", FindTask(NULL)));
    _ret = GLCALL(glIsVertexArrayAPPLE, array);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglStencilOpSeparateATI (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilOpSeparateATI", FindTask(NULL)));
    GLCALL(glStencilOpSeparateATI, face, sfail, dpfail, dppass);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglStencilFuncSeparateATI (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glStencilFuncSeparateATI", FindTask(NULL)));
    GLCALL(glStencilFuncSeparateATI, frontfunc, backfunc, ref, mask);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDepthBoundsEXT (GLclampd zmin, GLclampd zmax)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDepthBoundsEXT", FindTask(NULL)));
    GLCALL(glDepthBoundsEXT, zmin, zmax);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationSeparateEXT", FindTask(NULL)));
    GLCALL(glBlendEquationSeparateEXT, modeRGB, modeAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlitFramebufferEXT", FindTask(NULL)));
    GLCALL(glBlitFramebufferEXT, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjecti64vEXT (GLuint id, GLenum pname, GLint64EXT * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjecti64vEXT", FindTask(NULL)));
    GLCALL(glGetQueryObjecti64vEXT, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetQueryObjectui64vEXT (GLuint id, GLenum pname, GLuint64EXT * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetQueryObjectui64vEXT", FindTask(NULL)));
    GLCALL(glGetQueryObjectui64vEXT, id, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBufferParameteriAPPLE (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBufferParameteriAPPLE", FindTask(NULL)));
    GLCALL(glBufferParameteriAPPLE, target, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFlushMappedBufferRangeAPPLE (GLenum target, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFlushMappedBufferRangeAPPLE", FindTask(NULL)));
    GLCALL(glFlushMappedBufferRangeAPPLE, target, offset, size);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTextureRangeAPPLE (GLenum target, GLsizei length, const GLvoid * pointer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTextureRangeAPPLE", FindTask(NULL)));
    GLCALL(glTextureRangeAPPLE, target, length, pointer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetTexParameterPointervAPPLE (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetTexParameterPointervAPPLE", FindTask(NULL)));
    GLCALL(glGetTexParameterPointervAPPLE, target, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClampColorARB (GLenum target, GLenum clamp)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClampColorARB", FindTask(NULL)));
    GLCALL(glClampColorARB, target, clamp);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglFramebufferTextureLayerARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glFramebufferTextureLayerARB", FindTask(NULL)));
    GLCALL(glFramebufferTextureLayerARB, target, attachment, texture, level, layer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglVertexAttribDivisorARB (GLuint index, GLuint divisor)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glVertexAttribDivisorARB", FindTask(NULL)));
    GLCALL(glVertexAttribDivisorARB, index, divisor);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTexBufferARB (GLenum target, GLenum internalformat, GLuint buffer)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTexBufferARB", FindTask(NULL)));
    GLCALL(glTexBufferARB, target, internalformat, buffer);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDrawElementsInstancedBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount, GLint basevertex)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDrawElementsInstancedBaseVertex", FindTask(NULL)));
    GLCALL(glDrawElementsInstancedBaseVertex, mode, count, type, indices, primcount, basevertex);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationiARB (GLuint buf, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationiARB", FindTask(NULL)));
    GLCALL(glBlendEquationiARB, buf, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationSeparateiARB (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationSeparateiARB", FindTask(NULL)));
    GLCALL(glBlendEquationSeparateiARB, buf, modeRGB, modeAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFunciARB (GLuint buf, GLenum src, GLenum dst)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFunciARB", FindTask(NULL)));
    GLCALL(glBlendFunciARB, buf, src, dst);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncSeparateiARB (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncSeparateiARB", FindTask(NULL)));
    GLCALL(glBlendFuncSeparateiARB, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGenSamplers (GLsizei count, GLuint * samplers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGenSamplers", FindTask(NULL)));
    GLCALL(glGenSamplers, count, samplers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDeleteSamplers (GLsizei count, const GLuint * samplers)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDeleteSamplers", FindTask(NULL)));
    GLCALL(glDeleteSamplers, count, samplers);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLboolean mglIsSampler (GLuint sampler)
{
    GLboolean _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glIsSampler", FindTask(NULL)));
    _ret = GLCALL(glIsSampler, sampler);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglBindSampler (GLuint unit, GLuint sampler)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBindSampler", FindTask(NULL)));
    GLCALL(glBindSampler, unit, sampler);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameteri (GLuint sampler, GLenum pname, GLint param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameteri", FindTask(NULL)));
    GLCALL(glSamplerParameteri, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameteriv (GLuint sampler, GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameteriv", FindTask(NULL)));
    GLCALL(glSamplerParameteriv, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameterf", FindTask(NULL)));
    GLCALL(glSamplerParameterf, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameterfv", FindTask(NULL)));
    GLCALL(glSamplerParameterfv, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameterIiv", FindTask(NULL)));
    GLCALL(glSamplerParameterIiv, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint * param)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glSamplerParameterIuiv", FindTask(NULL)));
    GLCALL(glSamplerParameterIuiv, sampler, pname, param);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSamplerParameteriv", FindTask(NULL)));
    GLCALL(glGetSamplerParameteriv, sampler, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSamplerParameterIiv", FindTask(NULL)));
    GLCALL(glGetSamplerParameterIiv, sampler, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSamplerParameterfv", FindTask(NULL)));
    GLCALL(glGetSamplerParameterfv, sampler, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetSamplerParameterIuiv", FindTask(NULL)));
    GLCALL(glGetSamplerParameterIuiv, sampler, pname, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglReleaseShaderCompiler ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glReleaseShaderCompiler", FindTask(NULL)));
    GLCALL(glReleaseShaderCompiler);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglShaderBinary (GLsizei count, const GLuint * shaders, GLenum binaryformat, const GLvoid * binary, GLsizei length)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glShaderBinary", FindTask(NULL)));
    GLCALL(glShaderBinary, count, shaders, binaryformat, binary, length);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetShaderPrecisionFormat", FindTask(NULL)));
    GLCALL(glGetShaderPrecisionFormat, shadertype, precisiontype, range, precision);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglDepthRangef (GLclampf n, GLclampf f)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glDepthRangef", FindTask(NULL)));
    GLCALL(glDepthRangef, n, f);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglClearDepthf (GLclampf d)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glClearDepthf", FindTask(NULL)));
    GLCALL(glClearDepthf, d);
    D(bug("...exit\n"));
    HOSTGL_POST
}

GLenum mglGetGraphicsResetStatusARB ()
{
    GLenum _ret;
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetGraphicsResetStatusARB", FindTask(NULL)));
    _ret = GLCALL(glGetGraphicsResetStatusARB);
    D(bug("...exit\n"));
    HOSTGL_POST
    return _ret;
}

void mglGetnMapdvARB (GLenum target, GLenum query, GLsizei bufSize, GLdouble * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnMapdvARB", FindTask(NULL)));
    GLCALL(glGetnMapdvARB, target, query, bufSize, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnMapfvARB (GLenum target, GLenum query, GLsizei bufSize, GLfloat * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnMapfvARB", FindTask(NULL)));
    GLCALL(glGetnMapfvARB, target, query, bufSize, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnMapivARB (GLenum target, GLenum query, GLsizei bufSize, GLint * v)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnMapivARB", FindTask(NULL)));
    GLCALL(glGetnMapivARB, target, query, bufSize, v);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnPixelMapfvARB (GLenum map, GLsizei bufSize, GLfloat * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnPixelMapfvARB", FindTask(NULL)));
    GLCALL(glGetnPixelMapfvARB, map, bufSize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnPixelMapuivARB (GLenum map, GLsizei bufSize, GLuint * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnPixelMapuivARB", FindTask(NULL)));
    GLCALL(glGetnPixelMapuivARB, map, bufSize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnPixelMapusvARB (GLenum map, GLsizei bufSize, GLushort * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnPixelMapusvARB", FindTask(NULL)));
    GLCALL(glGetnPixelMapusvARB, map, bufSize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnPolygonStippleARB (GLsizei bufSize, GLubyte * pattern)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnPolygonStippleARB", FindTask(NULL)));
    GLCALL(glGetnPolygonStippleARB, bufSize, pattern);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnColorTableARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * table)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnColorTableARB", FindTask(NULL)));
    GLCALL(glGetnColorTableARB, target, format, type, bufSize, table);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnConvolutionFilterARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * image)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnConvolutionFilterARB", FindTask(NULL)));
    GLCALL(glGetnConvolutionFilterARB, target, format, type, bufSize, image);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnSeparableFilterARB (GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, GLvoid * row, GLsizei columnBufSize, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnSeparableFilterARB", FindTask(NULL)));
    GLCALL(glGetnSeparableFilterARB, target, format, type, rowBufSize, row, columnBufSize, column, span);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnHistogramARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnHistogramARB", FindTask(NULL)));
    GLCALL(glGetnHistogramARB, target, reset, format, type, bufSize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnMinmaxARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnMinmaxARB", FindTask(NULL)));
    GLCALL(glGetnMinmaxARB, target, reset, format, type, bufSize, values);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnTexImageARB (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid * img)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnTexImageARB", FindTask(NULL)));
    GLCALL(glGetnTexImageARB, target, level, format, type, bufSize, img);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglReadnPixelsARB (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLvoid * data)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glReadnPixelsARB", FindTask(NULL)));
    GLCALL(glReadnPixelsARB, x, y, width, height, format, type, bufSize, data);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnCompressedTexImageARB (GLenum target, GLint lod, GLsizei bufSize, GLvoid * img)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnCompressedTexImageARB", FindTask(NULL)));
    GLCALL(glGetnCompressedTexImageARB, target, lod, bufSize, img);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnUniformfvARB (GLuint program, GLint location, GLsizei bufSize, GLfloat * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnUniformfvARB", FindTask(NULL)));
    GLCALL(glGetnUniformfvARB, program, location, bufSize, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnUniformivARB (GLuint program, GLint location, GLsizei bufSize, GLint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnUniformivARB", FindTask(NULL)));
    GLCALL(glGetnUniformivARB, program, location, bufSize, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnUniformuivARB (GLuint program, GLint location, GLsizei bufSize, GLuint * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnUniformuivARB", FindTask(NULL)));
    GLCALL(glGetnUniformuivARB, program, location, bufSize, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglGetnUniformdvARB (GLuint program, GLint location, GLsizei bufSize, GLdouble * params)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glGetnUniformdvARB", FindTask(NULL)));
    GLCALL(glGetnUniformdvARB, program, location, bufSize, params);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncIndexedAMD (GLuint buf, GLenum src, GLenum dst)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncIndexedAMD", FindTask(NULL)));
    GLCALL(glBlendFuncIndexedAMD, buf, src, dst);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendFuncSeparateIndexedAMD (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendFuncSeparateIndexedAMD", FindTask(NULL)));
    GLCALL(glBlendFuncSeparateIndexedAMD, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationIndexedAMD (GLuint buf, GLenum mode)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationIndexedAMD", FindTask(NULL)));
    GLCALL(glBlendEquationIndexedAMD, buf, mode);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglBlendEquationSeparateIndexedAMD (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glBlendEquationSeparateIndexedAMD", FindTask(NULL)));
    GLCALL(glBlendEquationSeparateIndexedAMD, buf, modeRGB, modeAlpha);
    D(bug("...exit\n"));
    HOSTGL_POST
}

void mglTextureBarrierNV ()
{
    HOSTGL_PRE
    D(bug("TASK: 0x%x, glTextureBarrierNV", FindTask(NULL)));
    GLCALL(glTextureBarrierNV);
    D(bug("...exit\n"));
    HOSTGL_POST
}

