/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "glx_hostlib.h"
#include "arosmesaapim.h"
#include "hostgl_intern.h"

void mglClearIndex (GLfloat c)
{
    HOSTGL_PRE
    GLCALL(glClearIndex, c);
    HOSTGL_POST
}

void mglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    GLCALL(glClearColor, red, green, blue, alpha);
    HOSTGL_POST
}

void mglClear (GLbitfield mask)
{
    HOSTGL_PRE
    GLCALL(glClear, mask);
    HOSTGL_POST
}

void mglIndexMask (GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glIndexMask, mask);
    HOSTGL_POST
}

void mglColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    HOSTGL_PRE
    GLCALL(glColorMask, red, green, blue, alpha);
    HOSTGL_POST
}

void mglAlphaFunc (GLenum func, GLclampf ref)
{
    HOSTGL_PRE
    GLCALL(glAlphaFunc, func, ref);
    HOSTGL_POST
}

void mglBlendFunc (GLenum sfactor, GLenum dfactor)
{
    HOSTGL_PRE
    GLCALL(glBlendFunc, sfactor, dfactor);
    HOSTGL_POST
}

void mglLogicOp (GLenum opcode)
{
    HOSTGL_PRE
    GLCALL(glLogicOp, opcode);
    HOSTGL_POST
}

void mglCullFace (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glCullFace, mode);
    HOSTGL_POST
}

void mglFrontFace (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glFrontFace, mode);
    HOSTGL_POST
}

void mglPointSize (GLfloat size)
{
    HOSTGL_PRE
    GLCALL(glPointSize, size);
    HOSTGL_POST
}

void mglLineWidth (GLfloat width)
{
    HOSTGL_PRE
    GLCALL(glLineWidth, width);
    HOSTGL_POST
}

void mglLineStipple (GLint factor, GLushort pattern)
{
    HOSTGL_PRE
    GLCALL(glLineStipple, factor, pattern);
    HOSTGL_POST
}

void mglPolygonMode (GLenum face, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glPolygonMode, face, mode);
    HOSTGL_POST
}

void mglPolygonOffset (GLfloat factor, GLfloat units)
{
    HOSTGL_PRE
    GLCALL(glPolygonOffset, factor, units);
    HOSTGL_POST
}

void mglPolygonStipple (const GLubyte * mask)
{
    HOSTGL_PRE
    GLCALL(glPolygonStipple, mask);
    HOSTGL_POST
}

void mglGetPolygonStipple (GLubyte * mask)
{
    HOSTGL_PRE
    GLCALL(glGetPolygonStipple, mask);
    HOSTGL_POST
}

void mglEdgeFlag (GLboolean flag)
{
    HOSTGL_PRE
    GLCALL(glEdgeFlag, flag);
    HOSTGL_POST
}

void mglEdgeFlagv (const GLboolean * flag)
{
    HOSTGL_PRE
    GLCALL(glEdgeFlagv, flag);
    HOSTGL_POST
}

void mglScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glScissor, x, y, width, height);
    HOSTGL_POST
}

void mglClipPlane (GLenum plane, const GLdouble * equation)
{
    HOSTGL_PRE
    GLCALL(glClipPlane, plane, equation);
    HOSTGL_POST
}

void mglGetClipPlane (GLenum plane, GLdouble * equation)
{
    HOSTGL_PRE
    GLCALL(glGetClipPlane, plane, equation);
    HOSTGL_POST
}

void mglDrawBuffer (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glDrawBuffer, mode);
    HOSTGL_POST
}

void mglReadBuffer (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glReadBuffer, mode);
    HOSTGL_POST
}

void mglEnable (GLenum cap)
{
    HOSTGL_PRE
    GLCALL(glEnable, cap);
    HOSTGL_POST
}

void mglDisable (GLenum cap)
{
    HOSTGL_PRE
    GLCALL(glDisable, cap);
    HOSTGL_POST
}

GLboolean mglIsEnabled (GLenum cap)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsEnabled, cap);
    HOSTGL_POST
    return _ret;
}

void mglEnableClientState (GLenum cap)
{
    HOSTGL_PRE
    GLCALL(glEnableClientState, cap);
    HOSTGL_POST
}

void mglDisableClientState (GLenum cap)
{
    HOSTGL_PRE
    GLCALL(glDisableClientState, cap);
    HOSTGL_POST
}

void mglGetBooleanv (GLenum pname, GLboolean * params)
{
    HOSTGL_PRE
    GLCALL(glGetBooleanv, pname, params);
    HOSTGL_POST
}

void mglGetDoublev (GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetDoublev, pname, params);
    HOSTGL_POST
}

void mglGetFloatv (GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetFloatv, pname, params);
    HOSTGL_POST
}

void mglGetIntegerv (GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetIntegerv, pname, params);
    HOSTGL_POST
}

void mglPushAttrib (GLbitfield mask)
{
    HOSTGL_PRE
    GLCALL(glPushAttrib, mask);
    HOSTGL_POST
}

void mglPopAttrib ()
{
    HOSTGL_PRE
    GLCALL(glPopAttrib);
    HOSTGL_POST
}

void mglPushClientAttrib (GLbitfield mask)
{
    HOSTGL_PRE
    GLCALL(glPushClientAttrib, mask);
    HOSTGL_POST
}

void mglPopClientAttrib ()
{
    HOSTGL_PRE
    GLCALL(glPopClientAttrib);
    HOSTGL_POST
}

GLint mglRenderMode (GLenum mode)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glRenderMode, mode);
    HOSTGL_POST
    return _ret;
}

GLenum mglGetError ()
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetError);
    HOSTGL_POST
    return _ret;
}

const GLubyte * mglGetString (GLenum name)
{
    const GLubyte * _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetString, name);
    HOSTGL_POST
    return _ret;
}

void mglFinish ()
{
    HOSTGL_PRE
    GLCALL(glFinish);
    HOSTGL_POST
}

void mglFlush ()
{
    HOSTGL_PRE
    GLCALL(glFlush);
    HOSTGL_POST
}

void mglHint (GLenum target, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glHint, target, mode);
    HOSTGL_POST
}

void mglClearDepth (GLclampd depth)
{
    HOSTGL_PRE
    GLCALL(glClearDepth, depth);
    HOSTGL_POST
}

void mglDepthFunc (GLenum func)
{
    HOSTGL_PRE
    GLCALL(glDepthFunc, func);
    HOSTGL_POST
}

void mglDepthMask (GLboolean flag)
{
    HOSTGL_PRE
    GLCALL(glDepthMask, flag);
    HOSTGL_POST
}

void mglDepthRange (GLclampd near_val, GLclampd far_val)
{
    HOSTGL_PRE
    GLCALL(glDepthRange, near_val, far_val);
    HOSTGL_POST
}

void mglClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    HOSTGL_PRE
    GLCALL(glClearAccum, red, green, blue, alpha);
    HOSTGL_POST
}

void mglAccum (GLenum op, GLfloat value)
{
    HOSTGL_PRE
    GLCALL(glAccum, op, value);
    HOSTGL_POST
}

void mglMatrixMode (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glMatrixMode, mode);
    HOSTGL_POST
}

void mglOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    HOSTGL_PRE
    GLCALL(glOrtho, left, right, bottom, top, near_val, far_val);
    HOSTGL_POST
}

void mglFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    HOSTGL_PRE
    GLCALL(glFrustum, left, right, bottom, top, near_val, far_val);
    HOSTGL_POST
}

void mglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glViewport, x, y, width, height);
    HOSTGL_POST
}

void mglPushMatrix ()
{
    HOSTGL_PRE
    GLCALL(glPushMatrix);
    HOSTGL_POST
}

void mglPopMatrix ()
{
    HOSTGL_PRE
    GLCALL(glPopMatrix);
    HOSTGL_POST
}

void mglLoadIdentity ()
{
    HOSTGL_PRE
    GLCALL(glLoadIdentity);
    HOSTGL_POST
}

void mglLoadMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glLoadMatrixd, m);
    HOSTGL_POST
}

void mglLoadMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glLoadMatrixf, m);
    HOSTGL_POST
}

void mglMultMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glMultMatrixd, m);
    HOSTGL_POST
}

void mglMultMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glMultMatrixf, m);
    HOSTGL_POST
}

void mglRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glRotated, angle, x, y, z);
    HOSTGL_POST
}

void mglRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glRotatef, angle, x, y, z);
    HOSTGL_POST
}

void mglScaled (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glScaled, x, y, z);
    HOSTGL_POST
}

void mglScalef (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glScalef, x, y, z);
    HOSTGL_POST
}

void mglTranslated (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glTranslated, x, y, z);
    HOSTGL_POST
}

void mglTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glTranslatef, x, y, z);
    HOSTGL_POST
}

GLboolean mglIsList (GLuint list)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsList, list);
    HOSTGL_POST
    return _ret;
}

void mglDeleteLists (GLuint list, GLsizei range)
{
    HOSTGL_PRE
    GLCALL(glDeleteLists, list, range);
    HOSTGL_POST
}

GLuint mglGenLists (GLsizei range)
{
    GLuint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGenLists, range);
    HOSTGL_POST
    return _ret;
}

void mglNewList (GLuint list, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glNewList, list, mode);
    HOSTGL_POST
}

void mglEndList ()
{
    HOSTGL_PRE
    GLCALL(glEndList);
    HOSTGL_POST
}

void mglCallList (GLuint list)
{
    HOSTGL_PRE
    GLCALL(glCallList, list);
    HOSTGL_POST
}

void mglCallLists (GLsizei n, GLenum type, const GLvoid * lists)
{
    HOSTGL_PRE
    GLCALL(glCallLists, n, type, lists);
    HOSTGL_POST
}

void mglListBase (GLuint base)
{
    HOSTGL_PRE
    GLCALL(glListBase, base);
    HOSTGL_POST
}

void mglBegin (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBegin, mode);
    HOSTGL_POST
}

void mglEnd ()
{
    HOSTGL_PRE
    GLCALL(glEnd);
    HOSTGL_POST
}

void mglVertex2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glVertex2d, x, y);
    HOSTGL_POST
}

void mglVertex2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glVertex2f, x, y);
    HOSTGL_POST
}

void mglVertex2i (GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glVertex2i, x, y);
    HOSTGL_POST
}

void mglVertex2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glVertex2s, x, y);
    HOSTGL_POST
}

void mglVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glVertex3d, x, y, z);
    HOSTGL_POST
}

void mglVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glVertex3f, x, y, z);
    HOSTGL_POST
}

void mglVertex3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glVertex3i, x, y, z);
    HOSTGL_POST
}

void mglVertex3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glVertex3s, x, y, z);
    HOSTGL_POST
}

void mglVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glVertex4d, x, y, z, w);
    HOSTGL_POST
}

void mglVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glVertex4f, x, y, z, w);
    HOSTGL_POST
}

void mglVertex4i (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    GLCALL(glVertex4i, x, y, z, w);
    HOSTGL_POST
}

void mglVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glVertex4s, x, y, z, w);
    HOSTGL_POST
}

void mglVertex2dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertex2dv, v);
    HOSTGL_POST
}

void mglVertex2fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertex2fv, v);
    HOSTGL_POST
}

void mglVertex2iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertex2iv, v);
    HOSTGL_POST
}

void mglVertex2sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertex2sv, v);
    HOSTGL_POST
}

void mglVertex3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertex3dv, v);
    HOSTGL_POST
}

void mglVertex3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertex3fv, v);
    HOSTGL_POST
}

void mglVertex3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertex3iv, v);
    HOSTGL_POST
}

void mglVertex3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertex3sv, v);
    HOSTGL_POST
}

void mglVertex4dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertex4dv, v);
    HOSTGL_POST
}

void mglVertex4fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertex4fv, v);
    HOSTGL_POST
}

void mglVertex4iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertex4iv, v);
    HOSTGL_POST
}

void mglVertex4sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertex4sv, v);
    HOSTGL_POST
}

void mglNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
    HOSTGL_PRE
    GLCALL(glNormal3b, nx, ny, nz);
    HOSTGL_POST
}

void mglNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
    HOSTGL_PRE
    GLCALL(glNormal3d, nx, ny, nz);
    HOSTGL_POST
}

void mglNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    HOSTGL_PRE
    GLCALL(glNormal3f, nx, ny, nz);
    HOSTGL_POST
}

void mglNormal3i (GLint nx, GLint ny, GLint nz)
{
    HOSTGL_PRE
    GLCALL(glNormal3i, nx, ny, nz);
    HOSTGL_POST
}

void mglNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
    HOSTGL_PRE
    GLCALL(glNormal3s, nx, ny, nz);
    HOSTGL_POST
}

void mglNormal3bv (const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glNormal3bv, v);
    HOSTGL_POST
}

void mglNormal3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glNormal3dv, v);
    HOSTGL_POST
}

void mglNormal3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glNormal3fv, v);
    HOSTGL_POST
}

void mglNormal3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glNormal3iv, v);
    HOSTGL_POST
}

void mglNormal3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glNormal3sv, v);
    HOSTGL_POST
}

void mglIndexd (GLdouble c)
{
    HOSTGL_PRE
    GLCALL(glIndexd, c);
    HOSTGL_POST
}

void mglIndexf (GLfloat c)
{
    HOSTGL_PRE
    GLCALL(glIndexf, c);
    HOSTGL_POST
}

void mglIndexi (GLint c)
{
    HOSTGL_PRE
    GLCALL(glIndexi, c);
    HOSTGL_POST
}

void mglIndexs (GLshort c)
{
    HOSTGL_PRE
    GLCALL(glIndexs, c);
    HOSTGL_POST
}

void mglIndexub (GLubyte c)
{
    HOSTGL_PRE
    GLCALL(glIndexub, c);
    HOSTGL_POST
}

void mglIndexdv (const GLdouble * c)
{
    HOSTGL_PRE
    GLCALL(glIndexdv, c);
    HOSTGL_POST
}

void mglIndexfv (const GLfloat * c)
{
    HOSTGL_PRE
    GLCALL(glIndexfv, c);
    HOSTGL_POST
}

void mglIndexiv (const GLint * c)
{
    HOSTGL_PRE
    GLCALL(glIndexiv, c);
    HOSTGL_POST
}

void mglIndexsv (const GLshort * c)
{
    HOSTGL_PRE
    GLCALL(glIndexsv, c);
    HOSTGL_POST
}

void mglIndexubv (const GLubyte * c)
{
    HOSTGL_PRE
    GLCALL(glIndexubv, c);
    HOSTGL_POST
}

void mglColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    GLCALL(glColor3b, red, green, blue);
    HOSTGL_POST
}

void mglColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    GLCALL(glColor3d, red, green, blue);
    HOSTGL_POST
}

void mglColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    GLCALL(glColor3f, red, green, blue);
    HOSTGL_POST
}

void mglColor3i (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    GLCALL(glColor3i, red, green, blue);
    HOSTGL_POST
}

void mglColor3s (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    GLCALL(glColor3s, red, green, blue);
    HOSTGL_POST
}

void mglColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    GLCALL(glColor3ub, red, green, blue);
    HOSTGL_POST
}

void mglColor3ui (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    GLCALL(glColor3ui, red, green, blue);
    HOSTGL_POST
}

void mglColor3us (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    GLCALL(glColor3us, red, green, blue);
    HOSTGL_POST
}

void mglColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4b, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4d, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4f, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4i, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4s, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4ub, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4ui, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    HOSTGL_PRE
    GLCALL(glColor4us, red, green, blue, alpha);
    HOSTGL_POST
}

void mglColor3bv (const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glColor3bv, v);
    HOSTGL_POST
}

void mglColor3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glColor3dv, v);
    HOSTGL_POST
}

void mglColor3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glColor3fv, v);
    HOSTGL_POST
}

void mglColor3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glColor3iv, v);
    HOSTGL_POST
}

void mglColor3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glColor3sv, v);
    HOSTGL_POST
}

void mglColor3ubv (const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glColor3ubv, v);
    HOSTGL_POST
}

void mglColor3uiv (const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glColor3uiv, v);
    HOSTGL_POST
}

void mglColor3usv (const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glColor3usv, v);
    HOSTGL_POST
}

void mglColor4bv (const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glColor4bv, v);
    HOSTGL_POST
}

void mglColor4dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glColor4dv, v);
    HOSTGL_POST
}

void mglColor4fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glColor4fv, v);
    HOSTGL_POST
}

void mglColor4iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glColor4iv, v);
    HOSTGL_POST
}

void mglColor4sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glColor4sv, v);
    HOSTGL_POST
}

void mglColor4ubv (const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glColor4ubv, v);
    HOSTGL_POST
}

void mglColor4uiv (const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glColor4uiv, v);
    HOSTGL_POST
}

void mglColor4usv (const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glColor4usv, v);
    HOSTGL_POST
}

void mglTexCoord1d (GLdouble s)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1d, s);
    HOSTGL_POST
}

void mglTexCoord1f (GLfloat s)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1f, s);
    HOSTGL_POST
}

void mglTexCoord1i (GLint s)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1i, s);
    HOSTGL_POST
}

void mglTexCoord1s (GLshort s)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1s, s);
    HOSTGL_POST
}

void mglTexCoord2d (GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2d, s, t);
    HOSTGL_POST
}

void mglTexCoord2f (GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2f, s, t);
    HOSTGL_POST
}

void mglTexCoord2i (GLint s, GLint t)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2i, s, t);
    HOSTGL_POST
}

void mglTexCoord2s (GLshort s, GLshort t)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2s, s, t);
    HOSTGL_POST
}

void mglTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3d, s, t, r);
    HOSTGL_POST
}

void mglTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3f, s, t, r);
    HOSTGL_POST
}

void mglTexCoord3i (GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3i, s, t, r);
    HOSTGL_POST
}

void mglTexCoord3s (GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3s, s, t, r);
    HOSTGL_POST
}

void mglTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4d, s, t, r, q);
    HOSTGL_POST
}

void mglTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4f, s, t, r, q);
    HOSTGL_POST
}

void mglTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4i, s, t, r, q);
    HOSTGL_POST
}

void mglTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4s, s, t, r, q);
    HOSTGL_POST
}

void mglTexCoord1dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1dv, v);
    HOSTGL_POST
}

void mglTexCoord1fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1fv, v);
    HOSTGL_POST
}

void mglTexCoord1iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1iv, v);
    HOSTGL_POST
}

void mglTexCoord1sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord1sv, v);
    HOSTGL_POST
}

void mglTexCoord2dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2dv, v);
    HOSTGL_POST
}

void mglTexCoord2fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2fv, v);
    HOSTGL_POST
}

void mglTexCoord2iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2iv, v);
    HOSTGL_POST
}

void mglTexCoord2sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord2sv, v);
    HOSTGL_POST
}

void mglTexCoord3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3dv, v);
    HOSTGL_POST
}

void mglTexCoord3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3fv, v);
    HOSTGL_POST
}

void mglTexCoord3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3iv, v);
    HOSTGL_POST
}

void mglTexCoord3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord3sv, v);
    HOSTGL_POST
}

void mglTexCoord4dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4dv, v);
    HOSTGL_POST
}

void mglTexCoord4fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4fv, v);
    HOSTGL_POST
}

void mglTexCoord4iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4iv, v);
    HOSTGL_POST
}

void mglTexCoord4sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glTexCoord4sv, v);
    HOSTGL_POST
}

void mglRasterPos2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2d, x, y);
    HOSTGL_POST
}

void mglRasterPos2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2f, x, y);
    HOSTGL_POST
}

void mglRasterPos2i (GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2i, x, y);
    HOSTGL_POST
}

void mglRasterPos2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2s, x, y);
    HOSTGL_POST
}

void mglRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3d, x, y, z);
    HOSTGL_POST
}

void mglRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3f, x, y, z);
    HOSTGL_POST
}

void mglRasterPos3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3i, x, y, z);
    HOSTGL_POST
}

void mglRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3s, x, y, z);
    HOSTGL_POST
}

void mglRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4d, x, y, z, w);
    HOSTGL_POST
}

void mglRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4f, x, y, z, w);
    HOSTGL_POST
}

void mglRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4i, x, y, z, w);
    HOSTGL_POST
}

void mglRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4s, x, y, z, w);
    HOSTGL_POST
}

void mglRasterPos2dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2dv, v);
    HOSTGL_POST
}

void mglRasterPos2fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2fv, v);
    HOSTGL_POST
}

void mglRasterPos2iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2iv, v);
    HOSTGL_POST
}

void mglRasterPos2sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos2sv, v);
    HOSTGL_POST
}

void mglRasterPos3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3dv, v);
    HOSTGL_POST
}

void mglRasterPos3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3fv, v);
    HOSTGL_POST
}

void mglRasterPos3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3iv, v);
    HOSTGL_POST
}

void mglRasterPos3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos3sv, v);
    HOSTGL_POST
}

void mglRasterPos4dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4dv, v);
    HOSTGL_POST
}

void mglRasterPos4fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4fv, v);
    HOSTGL_POST
}

void mglRasterPos4iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4iv, v);
    HOSTGL_POST
}

void mglRasterPos4sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glRasterPos4sv, v);
    HOSTGL_POST
}

void mglRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    HOSTGL_PRE
    GLCALL(glRectd, x1, y1, x2, y2);
    HOSTGL_POST
}

void mglRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    HOSTGL_PRE
    GLCALL(glRectf, x1, y1, x2, y2);
    HOSTGL_POST
}

void mglRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    HOSTGL_PRE
    GLCALL(glRecti, x1, y1, x2, y2);
    HOSTGL_POST
}

void mglRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    HOSTGL_PRE
    GLCALL(glRects, x1, y1, x2, y2);
    HOSTGL_POST
}

void mglRectdv (const GLdouble * v1, const GLdouble * v2)
{
    HOSTGL_PRE
    GLCALL(glRectdv, v1, v2);
    HOSTGL_POST
}

void mglRectfv (const GLfloat * v1, const GLfloat * v2)
{
    HOSTGL_PRE
    GLCALL(glRectfv, v1, v2);
    HOSTGL_POST
}

void mglRectiv (const GLint * v1, const GLint * v2)
{
    HOSTGL_PRE
    GLCALL(glRectiv, v1, v2);
    HOSTGL_POST
}

void mglRectsv (const GLshort * v1, const GLshort * v2)
{
    HOSTGL_PRE
    GLCALL(glRectsv, v1, v2);
    HOSTGL_POST
}

void mglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glVertexPointer, size, type, stride, ptr);
    HOSTGL_POST
}

void mglNormalPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glNormalPointer, type, stride, ptr);
    HOSTGL_POST
}

void mglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glColorPointer, size, type, stride, ptr);
    HOSTGL_POST
}

void mglIndexPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glIndexPointer, type, stride, ptr);
    HOSTGL_POST
}

void mglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glTexCoordPointer, size, type, stride, ptr);
    HOSTGL_POST
}

void mglEdgeFlagPointer (GLsizei stride, const GLvoid * ptr)
{
    HOSTGL_PRE
    GLCALL(glEdgeFlagPointer, stride, ptr);
    HOSTGL_POST
}

void mglGetPointerv (GLenum pname, GLvoid * * params)
{
    HOSTGL_PRE
    GLCALL(glGetPointerv, pname, params);
    HOSTGL_POST
}

void mglArrayElement (GLint i)
{
    HOSTGL_PRE
    GLCALL(glArrayElement, i);
    HOSTGL_POST
}

void mglDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    HOSTGL_PRE
    GLCALL(glDrawArrays, mode, first, count);
    HOSTGL_POST
}

void mglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    GLCALL(glDrawElements, mode, count, type, indices);
    HOSTGL_POST
}

void mglInterleavedArrays (GLenum format, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glInterleavedArrays, format, stride, pointer);
    HOSTGL_POST
}

void mglShadeModel (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glShadeModel, mode);
    HOSTGL_POST
}

void mglLightf (GLenum light, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glLightf, light, pname, param);
    HOSTGL_POST
}

void mglLighti (GLenum light, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glLighti, light, pname, param);
    HOSTGL_POST
}

void mglLightfv (GLenum light, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glLightfv, light, pname, params);
    HOSTGL_POST
}

void mglLightiv (GLenum light, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glLightiv, light, pname, params);
    HOSTGL_POST
}

void mglGetLightfv (GLenum light, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetLightfv, light, pname, params);
    HOSTGL_POST
}

void mglGetLightiv (GLenum light, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetLightiv, light, pname, params);
    HOSTGL_POST
}

void mglLightModelf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glLightModelf, pname, param);
    HOSTGL_POST
}

void mglLightModeli (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glLightModeli, pname, param);
    HOSTGL_POST
}

void mglLightModelfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glLightModelfv, pname, params);
    HOSTGL_POST
}

void mglLightModeliv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glLightModeliv, pname, params);
    HOSTGL_POST
}

void mglMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glMaterialf, face, pname, param);
    HOSTGL_POST
}

void mglMateriali (GLenum face, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glMateriali, face, pname, param);
    HOSTGL_POST
}

void mglMaterialfv (GLenum face, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glMaterialfv, face, pname, params);
    HOSTGL_POST
}

void mglMaterialiv (GLenum face, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glMaterialiv, face, pname, params);
    HOSTGL_POST
}

void mglGetMaterialfv (GLenum face, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetMaterialfv, face, pname, params);
    HOSTGL_POST
}

void mglGetMaterialiv (GLenum face, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetMaterialiv, face, pname, params);
    HOSTGL_POST
}

void mglColorMaterial (GLenum face, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glColorMaterial, face, mode);
    HOSTGL_POST
}

void mglPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    HOSTGL_PRE
    GLCALL(glPixelZoom, xfactor, yfactor);
    HOSTGL_POST
}

void mglPixelStoref (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPixelStoref, pname, param);
    HOSTGL_POST
}

void mglPixelStorei (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glPixelStorei, pname, param);
    HOSTGL_POST
}

void mglPixelTransferf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPixelTransferf, pname, param);
    HOSTGL_POST
}

void mglPixelTransferi (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glPixelTransferi, pname, param);
    HOSTGL_POST
}

void mglPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat * values)
{
    HOSTGL_PRE
    GLCALL(glPixelMapfv, map, mapsize, values);
    HOSTGL_POST
}

void mglPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint * values)
{
    HOSTGL_PRE
    GLCALL(glPixelMapuiv, map, mapsize, values);
    HOSTGL_POST
}

void mglPixelMapusv (GLenum map, GLsizei mapsize, const GLushort * values)
{
    HOSTGL_PRE
    GLCALL(glPixelMapusv, map, mapsize, values);
    HOSTGL_POST
}

void mglGetPixelMapfv (GLenum map, GLfloat * values)
{
    HOSTGL_PRE
    GLCALL(glGetPixelMapfv, map, values);
    HOSTGL_POST
}

void mglGetPixelMapuiv (GLenum map, GLuint * values)
{
    HOSTGL_PRE
    GLCALL(glGetPixelMapuiv, map, values);
    HOSTGL_POST
}

void mglGetPixelMapusv (GLenum map, GLushort * values)
{
    HOSTGL_PRE
    GLCALL(glGetPixelMapusv, map, values);
    HOSTGL_POST
}

void mglBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap)
{
    HOSTGL_PRE
    GLCALL(glBitmap, width, height, xorig, yorig, xmove, ymove, bitmap);
    HOSTGL_POST
}

void mglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glReadPixels, x, y, width, height, format, type, pixels);
    HOSTGL_POST
}

void mglDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glDrawPixels, width, height, format, type, pixels);
    HOSTGL_POST
}

void mglCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    HOSTGL_PRE
    GLCALL(glCopyPixels, x, y, width, height, type);
    HOSTGL_POST
}

void mglStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glStencilFunc, func, ref, mask);
    HOSTGL_POST
}

void mglStencilMask (GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glStencilMask, mask);
    HOSTGL_POST
}

void mglStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    HOSTGL_PRE
    GLCALL(glStencilOp, fail, zfail, zpass);
    HOSTGL_POST
}

void mglClearStencil (GLint s)
{
    HOSTGL_PRE
    GLCALL(glClearStencil, s);
    HOSTGL_POST
}

void mglTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    HOSTGL_PRE
    GLCALL(glTexGend, coord, pname, param);
    HOSTGL_POST
}

void mglTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glTexGenf, coord, pname, param);
    HOSTGL_POST
}

void mglTexGeni (GLenum coord, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glTexGeni, coord, pname, param);
    HOSTGL_POST
}

void mglTexGendv (GLenum coord, GLenum pname, const GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glTexGendv, coord, pname, params);
    HOSTGL_POST
}

void mglTexGenfv (GLenum coord, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glTexGenfv, coord, pname, params);
    HOSTGL_POST
}

void mglTexGeniv (GLenum coord, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glTexGeniv, coord, pname, params);
    HOSTGL_POST
}

void mglGetTexGendv (GLenum coord, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexGendv, coord, pname, params);
    HOSTGL_POST
}

void mglGetTexGenfv (GLenum coord, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexGenfv, coord, pname, params);
    HOSTGL_POST
}

void mglGetTexGeniv (GLenum coord, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexGeniv, coord, pname, params);
    HOSTGL_POST
}

void mglTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glTexEnvf, target, pname, param);
    HOSTGL_POST
}

void mglTexEnvi (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glTexEnvi, target, pname, param);
    HOSTGL_POST
}

void mglTexEnvfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glTexEnvfv, target, pname, params);
    HOSTGL_POST
}

void mglTexEnviv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glTexEnviv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexEnvfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexEnvfv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexEnviv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexEnviv, target, pname, params);
    HOSTGL_POST
}

void mglTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glTexParameterf, target, pname, param);
    HOSTGL_POST
}

void mglTexParameteri (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glTexParameteri, target, pname, param);
    HOSTGL_POST
}

void mglTexParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglTexParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexLevelParameterfv, target, level, pname, params);
    HOSTGL_POST
}

void mglGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexLevelParameteriv, target, level, pname, params);
    HOSTGL_POST
}

void mglTexImage1D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexImage1D, target, level, internalFormat, width, border, format, type, pixels);
    HOSTGL_POST
}

void mglTexImage2D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexImage2D, target, level, internalFormat, width, height, border, format, type, pixels);
    HOSTGL_POST
}

void mglGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glGetTexImage, target, level, format, type, pixels);
    HOSTGL_POST
}

void mglGenTextures (GLsizei n, GLuint * textures)
{
    HOSTGL_PRE
    GLCALL(glGenTextures, n, textures);
    HOSTGL_POST
}

void mglDeleteTextures (GLsizei n, const GLuint * textures)
{
    HOSTGL_PRE
    GLCALL(glDeleteTextures, n, textures);
    HOSTGL_POST
}

void mglBindTexture (GLenum target, GLuint texture)
{
    HOSTGL_PRE
    GLCALL(glBindTexture, target, texture);
    HOSTGL_POST
}

void mglPrioritizeTextures (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    HOSTGL_PRE
    GLCALL(glPrioritizeTextures, n, textures, priorities);
    HOSTGL_POST
}

GLboolean mglAreTexturesResident (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glAreTexturesResident, n, textures, residences);
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsTexture (GLuint texture)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsTexture, texture);
    HOSTGL_POST
    return _ret;
}

void mglTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage1D, target, level, xoffset, width, format, type, pixels);
    HOSTGL_POST
}

void mglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels);
    HOSTGL_POST
}

void mglCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    HOSTGL_PRE
    GLCALL(glCopyTexImage1D, target, level, internalformat, x, y, width, border);
    HOSTGL_POST
}

void mglCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    HOSTGL_PRE
    GLCALL(glCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
    HOSTGL_POST
}

void mglCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage1D, target, level, xoffset, x, y, width);
    HOSTGL_POST
}

void mglCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height);
    HOSTGL_POST
}

void mglMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points)
{
    HOSTGL_PRE
    GLCALL(glMap1d, target, u1, u2, stride, order, points);
    HOSTGL_POST
}

void mglMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points)
{
    HOSTGL_PRE
    GLCALL(glMap1f, target, u1, u2, stride, order, points);
    HOSTGL_POST
}

void mglMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points)
{
    HOSTGL_PRE
    GLCALL(glMap2d, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    HOSTGL_POST
}

void mglMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points)
{
    HOSTGL_PRE
    GLCALL(glMap2f, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
    HOSTGL_POST
}

void mglGetMapdv (GLenum target, GLenum query, GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glGetMapdv, target, query, v);
    HOSTGL_POST
}

void mglGetMapfv (GLenum target, GLenum query, GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glGetMapfv, target, query, v);
    HOSTGL_POST
}

void mglGetMapiv (GLenum target, GLenum query, GLint * v)
{
    HOSTGL_PRE
    GLCALL(glGetMapiv, target, query, v);
    HOSTGL_POST
}

void mglEvalCoord1d (GLdouble u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord1d, u);
    HOSTGL_POST
}

void mglEvalCoord1f (GLfloat u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord1f, u);
    HOSTGL_POST
}

void mglEvalCoord1dv (const GLdouble * u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord1dv, u);
    HOSTGL_POST
}

void mglEvalCoord1fv (const GLfloat * u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord1fv, u);
    HOSTGL_POST
}

void mglEvalCoord2d (GLdouble u, GLdouble v)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord2d, u, v);
    HOSTGL_POST
}

void mglEvalCoord2f (GLfloat u, GLfloat v)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord2f, u, v);
    HOSTGL_POST
}

void mglEvalCoord2dv (const GLdouble * u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord2dv, u);
    HOSTGL_POST
}

void mglEvalCoord2fv (const GLfloat * u)
{
    HOSTGL_PRE
    GLCALL(glEvalCoord2fv, u);
    HOSTGL_POST
}

void mglMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    HOSTGL_PRE
    GLCALL(glMapGrid1d, un, u1, u2);
    HOSTGL_POST
}

void mglMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    HOSTGL_PRE
    GLCALL(glMapGrid1f, un, u1, u2);
    HOSTGL_POST
}

void mglMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
    HOSTGL_PRE
    GLCALL(glMapGrid2d, un, u1, u2, vn, v1, v2);
    HOSTGL_POST
}

void mglMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    GLCALL(glMapGrid2f, un, u1, u2, vn, v1, v2);
    HOSTGL_POST
}

void mglEvalPoint1 (GLint i)
{
    HOSTGL_PRE
    GLCALL(glEvalPoint1, i);
    HOSTGL_POST
}

void mglEvalPoint2 (GLint i, GLint j)
{
    HOSTGL_PRE
    GLCALL(glEvalPoint2, i, j);
    HOSTGL_POST
}

void mglEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    HOSTGL_PRE
    GLCALL(glEvalMesh1, mode, i1, i2);
    HOSTGL_POST
}

void mglEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    HOSTGL_PRE
    GLCALL(glEvalMesh2, mode, i1, i2, j1, j2);
    HOSTGL_POST
}

void mglFogf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glFogf, pname, param);
    HOSTGL_POST
}

void mglFogi (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glFogi, pname, param);
    HOSTGL_POST
}

void mglFogfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glFogfv, pname, params);
    HOSTGL_POST
}

void mglFogiv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glFogiv, pname, params);
    HOSTGL_POST
}

void mglFeedbackBuffer (GLsizei size, GLenum type, GLfloat * buffer)
{
    HOSTGL_PRE
    GLCALL(glFeedbackBuffer, size, type, buffer);
    HOSTGL_POST
}

void mglPassThrough (GLfloat token)
{
    HOSTGL_PRE
    GLCALL(glPassThrough, token);
    HOSTGL_POST
}

void mglSelectBuffer (GLsizei size, GLuint * buffer)
{
    HOSTGL_PRE
    GLCALL(glSelectBuffer, size, buffer);
    HOSTGL_POST
}

void mglInitNames ()
{
    HOSTGL_PRE
    GLCALL(glInitNames);
    HOSTGL_POST
}

void mglLoadName (GLuint name)
{
    HOSTGL_PRE
    GLCALL(glLoadName, name);
    HOSTGL_POST
}

void mglPushName (GLuint name)
{
    HOSTGL_PRE
    GLCALL(glPushName, name);
    HOSTGL_POST
}

void mglPopName ()
{
    HOSTGL_PRE
    GLCALL(glPopName);
    HOSTGL_POST
}

void mglDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    GLCALL(glDrawRangeElements, mode, start, end, count, type, indices);
    HOSTGL_POST
}

void mglTexImage3D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexImage3D, target, level, internalFormat, width, height, depth, border, format, type, pixels);
    HOSTGL_POST
}

void mglTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    HOSTGL_POST
}

void mglCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage3D, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    HOSTGL_POST
}

void mglColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glColorTable, target, internalformat, width, format, type, table);
    HOSTGL_POST
}

void mglColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glColorSubTable, target, start, count, format, type, data);
    HOSTGL_POST
}

void mglColorTableParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glColorTableParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglColorTableParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glColorTableParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyColorSubTable, target, start, x, y, width);
    HOSTGL_POST
}

void mglCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyColorTable, target, internalformat, x, y, width);
    HOSTGL_POST
}

void mglGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glGetColorTable, target, format, type, table);
    HOSTGL_POST
}

void mglGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglGetColorTableParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglBlendEquation (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBlendEquation, mode);
    HOSTGL_POST
}

void mglBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    GLCALL(glBlendColor, red, green, blue, alpha);
    HOSTGL_POST
}

void mglHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    GLCALL(glHistogram, target, width, internalformat, sink);
    HOSTGL_POST
}

void mglResetHistogram (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glResetHistogram, target);
    HOSTGL_POST
}

void mglGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetHistogram, target, reset, format, type, values);
    HOSTGL_POST
}

void mglGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetHistogramParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglGetHistogramParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetHistogramParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglMinmax (GLenum target, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    GLCALL(glMinmax, target, internalformat, sink);
    HOSTGL_POST
}

void mglResetMinmax (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glResetMinmax, target);
    HOSTGL_POST
}

void mglGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetMinmax, target, reset, format, types, values);
    HOSTGL_POST
}

void mglGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetMinmaxParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglGetMinmaxParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetMinmaxParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glConvolutionFilter1D, target, internalformat, width, format, type, image);
    HOSTGL_POST
}

void mglConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glConvolutionFilter2D, target, internalformat, width, height, format, type, image);
    HOSTGL_POST
}

void mglConvolutionParameterf (GLenum target, GLenum pname, GLfloat params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameterf, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameteri (GLenum target, GLenum pname, GLint params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameteri, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyConvolutionFilter1D, target, internalformat, x, y, width);
    HOSTGL_POST
}

void mglCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyConvolutionFilter2D, target, internalformat, x, y, width, height);
    HOSTGL_POST
}

void mglGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionFilter, target, format, type, image);
    HOSTGL_POST
}

void mglGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionParameterfv, target, pname, params);
    HOSTGL_POST
}

void mglGetConvolutionParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    HOSTGL_PRE
    GLCALL(glSeparableFilter2D, target, internalformat, width, height, format, type, row, column);
    HOSTGL_POST
}

void mglGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    GLCALL(glGetSeparableFilter, target, format, type, row, column, span);
    HOSTGL_POST
}

void mglActiveTexture (GLenum texture)
{
    HOSTGL_PRE
    GLCALL(glActiveTexture, texture);
    HOSTGL_POST
}

void mglClientActiveTexture (GLenum texture)
{
    HOSTGL_PRE
    GLCALL(glClientActiveTexture, texture);
    HOSTGL_POST
}

void mglCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage1D, target, level, internalformat, width, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage3D, target, level, internalformat, width, height, depth, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage1D, target, level, xoffset, width, format, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    HOSTGL_POST
}

void mglGetCompressedTexImage (GLenum target, GLint lod, GLvoid * img)
{
    HOSTGL_PRE
    GLCALL(glGetCompressedTexImage, target, lod, img);
    HOSTGL_POST
}

void mglMultiTexCoord1d (GLenum target, GLdouble s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1d, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1dv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1f (GLenum target, GLfloat s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1f, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1fv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1i (GLenum target, GLint s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1i, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1iv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1s (GLenum target, GLshort s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1s, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1sv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2d, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2dv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2f, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2fv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2i (GLenum target, GLint s, GLint t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2i, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2iv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2s (GLenum target, GLshort s, GLshort t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2s, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2sv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3d, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3dv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3f, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3fv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3i, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3iv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3s, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3sv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4d, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4dv (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4dv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4f, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4fv (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4fv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4i (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4i, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4iv (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4iv, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4s, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4sv (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4sv, target, v);
    HOSTGL_POST
}

void mglLoadTransposeMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glLoadTransposeMatrixd, m);
    HOSTGL_POST
}

void mglLoadTransposeMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glLoadTransposeMatrixf, m);
    HOSTGL_POST
}

void mglMultTransposeMatrixd (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glMultTransposeMatrixd, m);
    HOSTGL_POST
}

void mglMultTransposeMatrixf (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glMultTransposeMatrixf, m);
    HOSTGL_POST
}

void mglSampleCoverage (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    GLCALL(glSampleCoverage, value, invert);
    HOSTGL_POST
}

void mglActiveTextureARB (GLenum texture)
{
    HOSTGL_PRE
    GLCALL(glActiveTextureARB, texture);
    HOSTGL_POST
}

void mglClientActiveTextureARB (GLenum texture)
{
    HOSTGL_PRE
    GLCALL(glClientActiveTextureARB, texture);
    HOSTGL_POST
}

void mglMultiTexCoord1dARB (GLenum target, GLdouble s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1dARB, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1dvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1fARB (GLenum target, GLfloat s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1fARB, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1fvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1iARB (GLenum target, GLint s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1iARB, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1ivARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord1sARB (GLenum target, GLshort s)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1sARB, target, s);
    HOSTGL_POST
}

void mglMultiTexCoord1svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord1svARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2dARB (GLenum target, GLdouble s, GLdouble t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2dARB, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2dvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2fARB (GLenum target, GLfloat s, GLfloat t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2fARB, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2fvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2iARB (GLenum target, GLint s, GLint t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2iARB, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2ivARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord2sARB (GLenum target, GLshort s, GLshort t)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2sARB, target, s, t);
    HOSTGL_POST
}

void mglMultiTexCoord2svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord2svARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3dARB, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3dvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3fARB, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3fvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3iARB (GLenum target, GLint s, GLint t, GLint r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3iARB, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3ivARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord3sARB (GLenum target, GLshort s, GLshort t, GLshort r)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3sARB, target, s, t, r);
    HOSTGL_POST
}

void mglMultiTexCoord3svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord3svARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4dARB, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4dvARB (GLenum target, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4dvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4fARB, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4fvARB (GLenum target, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4fvARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4iARB (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4iARB, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4ivARB (GLenum target, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4ivARB, target, v);
    HOSTGL_POST
}

void mglMultiTexCoord4sARB (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4sARB, target, s, t, r, q);
    HOSTGL_POST
}

void mglMultiTexCoord4svARB (GLenum target, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glMultiTexCoord4svARB, target, v);
    HOSTGL_POST
}

void mglBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    HOSTGL_POST
}

void mglFogCoordf (GLfloat coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoordf, coord);
    HOSTGL_POST
}

void mglFogCoordfv (const GLfloat * coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoordfv, coord);
    HOSTGL_POST
}

void mglFogCoordd (GLdouble coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoordd, coord);
    HOSTGL_POST
}

void mglFogCoorddv (const GLdouble * coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoorddv, coord);
    HOSTGL_POST
}

void mglFogCoordPointer (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glFogCoordPointer, type, stride, pointer);
    HOSTGL_POST
}

void mglMultiDrawArrays (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glMultiDrawArrays, mode, first, count, primcount);
    HOSTGL_POST
}

void mglMultiDrawElements (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glMultiDrawElements, mode, count, type, indices, primcount);
    HOSTGL_POST
}

void mglPointParameterf (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPointParameterf, pname, param);
    HOSTGL_POST
}

void mglPointParameterfv (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfv, pname, params);
    HOSTGL_POST
}

void mglPointParameteri (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glPointParameteri, pname, param);
    HOSTGL_POST
}

void mglPointParameteriv (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameteriv, pname, params);
    HOSTGL_POST
}

void mglSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3b, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3bv (const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3bv, v);
    HOSTGL_POST
}

void mglSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3d, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3dv, v);
    HOSTGL_POST
}

void mglSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3f, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3fv, v);
    HOSTGL_POST
}

void mglSecondaryColor3i (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3i, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3iv, v);
    HOSTGL_POST
}

void mglSecondaryColor3s (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3s, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3sv, v);
    HOSTGL_POST
}

void mglSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ub, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3ubv (const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ubv, v);
    HOSTGL_POST
}

void mglSecondaryColor3ui (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ui, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3uiv (const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3uiv, v);
    HOSTGL_POST
}

void mglSecondaryColor3us (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3us, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3usv (const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3usv, v);
    HOSTGL_POST
}

void mglSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColorPointer, size, type, stride, pointer);
    HOSTGL_POST
}

void mglWindowPos2d (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2d, x, y);
    HOSTGL_POST
}

void mglWindowPos2dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2dv, v);
    HOSTGL_POST
}

void mglWindowPos2f (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2f, x, y);
    HOSTGL_POST
}

void mglWindowPos2fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2fv, v);
    HOSTGL_POST
}

void mglWindowPos2i (GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2i, x, y);
    HOSTGL_POST
}

void mglWindowPos2iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2iv, v);
    HOSTGL_POST
}

void mglWindowPos2s (GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2s, x, y);
    HOSTGL_POST
}

void mglWindowPos2sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2sv, v);
    HOSTGL_POST
}

void mglWindowPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3d, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3dv (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3dv, v);
    HOSTGL_POST
}

void mglWindowPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3f, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3fv (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3fv, v);
    HOSTGL_POST
}

void mglWindowPos3i (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3i, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3iv (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3iv, v);
    HOSTGL_POST
}

void mglWindowPos3s (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3s, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3sv (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3sv, v);
    HOSTGL_POST
}

void mglGenQueries (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glGenQueries, n, ids);
    HOSTGL_POST
}

void mglDeleteQueries (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glDeleteQueries, n, ids);
    HOSTGL_POST
}

GLboolean mglIsQuery (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsQuery, id);
    HOSTGL_POST
    return _ret;
}

void mglBeginQuery (GLenum target, GLuint id)
{
    HOSTGL_PRE
    GLCALL(glBeginQuery, target, id);
    HOSTGL_POST
}

void mglEndQuery (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glEndQuery, target);
    HOSTGL_POST
}

void mglGetQueryiv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryiv, target, pname, params);
    HOSTGL_POST
}

void mglGetQueryObjectiv (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjectiv, id, pname, params);
    HOSTGL_POST
}

void mglGetQueryObjectuiv (GLuint id, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjectuiv, id, pname, params);
    HOSTGL_POST
}

void mglBindBuffer (GLenum target, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glBindBuffer, target, buffer);
    HOSTGL_POST
}

void mglDeleteBuffers (GLsizei n, const GLuint * buffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteBuffers, n, buffers);
    HOSTGL_POST
}

void mglGenBuffers (GLsizei n, GLuint * buffers)
{
    HOSTGL_PRE
    GLCALL(glGenBuffers, n, buffers);
    HOSTGL_POST
}

GLboolean mglIsBuffer (GLuint buffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsBuffer, buffer);
    HOSTGL_POST
    return _ret;
}

void mglBufferData (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
    HOSTGL_PRE
    GLCALL(glBufferData, target, size, data, usage);
    HOSTGL_POST
}

void mglBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glBufferSubData, target, offset, size, data);
    HOSTGL_POST
}

void mglGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glGetBufferSubData, target, offset, size, data);
    HOSTGL_POST
}

GLvoid* mglMapBuffer (GLenum target, GLenum access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    _ret = GLCALL(glMapBuffer, target, access);
    HOSTGL_POST
    return _ret;
}

GLboolean mglUnmapBuffer (GLenum target)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glUnmapBuffer, target);
    HOSTGL_POST
    return _ret;
}

void mglGetBufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetBufferParameteriv, target, pname, params);
    HOSTGL_POST
}

void mglGetBufferPointerv (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    GLCALL(glGetBufferPointerv, target, pname, params);
    HOSTGL_POST
}

void mglBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationSeparate, modeRGB, modeAlpha);
    HOSTGL_POST
}

void mglDrawBuffers (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    GLCALL(glDrawBuffers, n, bufs);
    HOSTGL_POST
}

void mglStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    HOSTGL_PRE
    GLCALL(glStencilOpSeparate, face, sfail, dpfail, dppass);
    HOSTGL_POST
}

void mglStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glStencilFuncSeparate, face, func, ref, mask);
    HOSTGL_POST
}

void mglStencilMaskSeparate (GLenum face, GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glStencilMaskSeparate, face, mask);
    HOSTGL_POST
}

void mglAttachShader (GLuint program, GLuint shader)
{
    HOSTGL_PRE
    GLCALL(glAttachShader, program, shader);
    HOSTGL_POST
}

void mglBindAttribLocation (GLuint program, GLuint index, const GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glBindAttribLocation, program, index, name);
    HOSTGL_POST
}

void mglCompileShader (GLuint shader)
{
    HOSTGL_PRE
    GLCALL(glCompileShader, shader);
    HOSTGL_POST
}

GLuint mglCreateProgram ()
{
    GLuint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCreateProgram);
    HOSTGL_POST
    return _ret;
}

GLuint mglCreateShader (GLenum type)
{
    GLuint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCreateShader, type);
    HOSTGL_POST
    return _ret;
}

void mglDeleteProgram (GLuint program)
{
    HOSTGL_PRE
    GLCALL(glDeleteProgram, program);
    HOSTGL_POST
}

void mglDeleteShader (GLuint shader)
{
    HOSTGL_PRE
    GLCALL(glDeleteShader, shader);
    HOSTGL_POST
}

void mglDetachShader (GLuint program, GLuint shader)
{
    HOSTGL_PRE
    GLCALL(glDetachShader, program, shader);
    HOSTGL_POST
}

void mglDisableVertexAttribArray (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glDisableVertexAttribArray, index);
    HOSTGL_POST
}

void mglEnableVertexAttribArray (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glEnableVertexAttribArray, index);
    HOSTGL_POST
}

void mglGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glGetActiveAttrib, program, index, bufSize, length, size, type, name);
    HOSTGL_POST
}

void mglGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glGetActiveUniform, program, index, bufSize, length, size, type, name);
    HOSTGL_POST
}

void mglGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei * count, GLuint * obj)
{
    HOSTGL_PRE
    GLCALL(glGetAttachedShaders, program, maxCount, count, obj);
    HOSTGL_POST
}

GLint mglGetAttribLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetAttribLocation, program, name);
    HOSTGL_POST
    return _ret;
}

void mglGetProgramiv (GLuint program, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramiv, program, pname, params);
    HOSTGL_POST
}

void mglGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    HOSTGL_PRE
    GLCALL(glGetProgramInfoLog, program, bufSize, length, infoLog);
    HOSTGL_POST
}

void mglGetShaderiv (GLuint shader, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetShaderiv, shader, pname, params);
    HOSTGL_POST
}

void mglGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    HOSTGL_PRE
    GLCALL(glGetShaderInfoLog, shader, bufSize, length, infoLog);
    HOSTGL_POST
}

void mglGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source)
{
    HOSTGL_PRE
    GLCALL(glGetShaderSource, shader, bufSize, length, source);
    HOSTGL_POST
}

GLint mglGetUniformLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetUniformLocation, program, name);
    HOSTGL_POST
    return _ret;
}

void mglGetUniformfv (GLuint program, GLint location, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformfv, program, location, params);
    HOSTGL_POST
}

void mglGetUniformiv (GLuint program, GLint location, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformiv, program, location, params);
    HOSTGL_POST
}

void mglGetVertexAttribdv (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribdv, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribfv (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribfv, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribiv (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribiv, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribPointerv, index, pname, pointer);
    HOSTGL_POST
}

GLboolean mglIsProgram (GLuint program)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsProgram, program);
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsShader (GLuint shader)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsShader, shader);
    HOSTGL_POST
    return _ret;
}

void mglLinkProgram (GLuint program)
{
    HOSTGL_PRE
    GLCALL(glLinkProgram, program);
    HOSTGL_POST
}

void mglShaderSource (GLuint shader, GLsizei count, const GLchar *  * string, const GLint * length)
{
    HOSTGL_PRE
    GLCALL(glShaderSource, shader, count, string, length);
    HOSTGL_POST
}

void mglUseProgram (GLuint program)
{
    HOSTGL_PRE
    GLCALL(glUseProgram, program);
    HOSTGL_POST
}

void mglUniform1f (GLint location, GLfloat v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1f, location, v0);
    HOSTGL_POST
}

void mglUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2f, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3f, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4f, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1i (GLint location, GLint v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1i, location, v0);
    HOSTGL_POST
}

void mglUniform2i (GLint location, GLint v0, GLint v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2i, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3i, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4i, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1fv, location, count, value);
    HOSTGL_POST
}

void mglUniform2fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2fv, location, count, value);
    HOSTGL_POST
}

void mglUniform3fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3fv, location, count, value);
    HOSTGL_POST
}

void mglUniform4fv (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4fv, location, count, value);
    HOSTGL_POST
}

void mglUniform1iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1iv, location, count, value);
    HOSTGL_POST
}

void mglUniform2iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2iv, location, count, value);
    HOSTGL_POST
}

void mglUniform3iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3iv, location, count, value);
    HOSTGL_POST
}

void mglUniform4iv (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4iv, location, count, value);
    HOSTGL_POST
}

void mglUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix2fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix3fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix4fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglValidateProgram (GLuint program)
{
    HOSTGL_PRE
    GLCALL(glValidateProgram, program);
    HOSTGL_POST
}

void mglVertexAttrib1d (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1d, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1dv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1f (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1f, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1fv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1s (GLuint index, GLshort x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1s, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1sv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2d (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2d, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2dv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2f, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2fv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2s (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2s, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2sv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3d, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3dv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3f, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3fv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3s, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3sv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Nbv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nbv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Niv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Niv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Nsv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nsv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nub, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4Nubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nubv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Nuiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nuiv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4Nusv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4Nusv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4bv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4bv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4d, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4dv (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4dv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4f, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4fv (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4fv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4iv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4s, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4sv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4ubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4ubv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4uiv, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4usv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4usv, index, v);
    HOSTGL_POST
}

void mglVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
    HOSTGL_POST
}

void mglUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix2x3fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix3x2fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix2x4fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix4x2fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix3x4fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix4x3fv, location, count, transpose, value);
    HOSTGL_POST
}

void mglLoadTransposeMatrixfARB (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glLoadTransposeMatrixfARB, m);
    HOSTGL_POST
}

void mglLoadTransposeMatrixdARB (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glLoadTransposeMatrixdARB, m);
    HOSTGL_POST
}

void mglMultTransposeMatrixfARB (const GLfloat * m)
{
    HOSTGL_PRE
    GLCALL(glMultTransposeMatrixfARB, m);
    HOSTGL_POST
}

void mglMultTransposeMatrixdARB (const GLdouble * m)
{
    HOSTGL_PRE
    GLCALL(glMultTransposeMatrixdARB, m);
    HOSTGL_POST
}

void mglSampleCoverageARB (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    GLCALL(glSampleCoverageARB, value, invert);
    HOSTGL_POST
}

void mglCompressedTexImage3DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage3DARB, target, level, internalformat, width, height, depth, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexImage2DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage2DARB, target, level, internalformat, width, height, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexImage1DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexImage1DARB, target, level, internalformat, width, border, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage3DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage3DARB, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage2DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage2DARB, target, level, xoffset, yoffset, width, height, format, imageSize, data);
    HOSTGL_POST
}

void mglCompressedTexSubImage1DARB (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glCompressedTexSubImage1DARB, target, level, xoffset, width, format, imageSize, data);
    HOSTGL_POST
}

void mglGetCompressedTexImageARB (GLenum target, GLint level, GLvoid * img)
{
    HOSTGL_PRE
    GLCALL(glGetCompressedTexImageARB, target, level, img);
    HOSTGL_POST
}

void mglPointParameterfARB (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfARB, pname, param);
    HOSTGL_POST
}

void mglPointParameterfvARB (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfvARB, pname, params);
    HOSTGL_POST
}

void mglWindowPos2dARB (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2dARB, x, y);
    HOSTGL_POST
}

void mglWindowPos2dvARB (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2dvARB, v);
    HOSTGL_POST
}

void mglWindowPos2fARB (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2fARB, x, y);
    HOSTGL_POST
}

void mglWindowPos2fvARB (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2fvARB, v);
    HOSTGL_POST
}

void mglWindowPos2iARB (GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2iARB, x, y);
    HOSTGL_POST
}

void mglWindowPos2ivARB (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2ivARB, v);
    HOSTGL_POST
}

void mglWindowPos2sARB (GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2sARB, x, y);
    HOSTGL_POST
}

void mglWindowPos2svARB (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2svARB, v);
    HOSTGL_POST
}

void mglWindowPos3dARB (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3dARB, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3dvARB (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3dvARB, v);
    HOSTGL_POST
}

void mglWindowPos3fARB (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3fARB, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3fvARB (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3fvARB, v);
    HOSTGL_POST
}

void mglWindowPos3iARB (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3iARB, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3ivARB (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3ivARB, v);
    HOSTGL_POST
}

void mglWindowPos3sARB (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3sARB, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3svARB (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3svARB, v);
    HOSTGL_POST
}

void mglVertexAttrib1dARB (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1dARB, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1dvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1fARB (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1fARB, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1fvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1sARB (GLuint index, GLshort x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1sARB, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1svARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2dARB (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2dARB, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2dvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2fARB (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2fARB, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2fvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2sARB (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2sARB, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2svARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3dARB, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3dvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3fARB, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3fvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3sARB (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3sARB, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3svARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NbvARB (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NbvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NivARB (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NivARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NsvARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NsvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NubARB (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NubARB, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4NubvARB (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NubvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NuivARB (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NuivARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4NusvARB (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4NusvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4bvARB (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4bvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4dARB, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4dvARB (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4dvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4fARB, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4fvARB (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4fvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4ivARB (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4ivARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4sARB (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4sARB, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4svARB (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4svARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4ubvARB (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4ubvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4uivARB (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4uivARB, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4usvARB (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4usvARB, index, v);
    HOSTGL_POST
}

void mglVertexAttribPointerARB (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribPointerARB, index, size, type, normalized, stride, pointer);
    HOSTGL_POST
}

void mglEnableVertexAttribArrayARB (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glEnableVertexAttribArrayARB, index);
    HOSTGL_POST
}

void mglDisableVertexAttribArrayARB (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glDisableVertexAttribArrayARB, index);
    HOSTGL_POST
}

void mglProgramStringARB (GLenum target, GLenum format, GLsizei len, const GLvoid * string)
{
    HOSTGL_PRE
    GLCALL(glProgramStringARB, target, format, len, string);
    HOSTGL_POST
}

void mglBindProgramARB (GLenum target, GLuint program)
{
    HOSTGL_PRE
    GLCALL(glBindProgramARB, target, program);
    HOSTGL_POST
}

void mglDeleteProgramsARB (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    GLCALL(glDeleteProgramsARB, n, programs);
    HOSTGL_POST
}

void mglGenProgramsARB (GLsizei n, GLuint * programs)
{
    HOSTGL_PRE
    GLCALL(glGenProgramsARB, n, programs);
    HOSTGL_POST
}

void mglProgramEnvParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glProgramEnvParameter4dARB, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramEnvParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glProgramEnvParameter4dvARB, target, index, params);
    HOSTGL_POST
}

void mglProgramEnvParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glProgramEnvParameter4fARB, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramEnvParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glProgramEnvParameter4fvARB, target, index, params);
    HOSTGL_POST
}

void mglProgramLocalParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glProgramLocalParameter4dARB, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramLocalParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glProgramLocalParameter4dvARB, target, index, params);
    HOSTGL_POST
}

void mglProgramLocalParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glProgramLocalParameter4fARB, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramLocalParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glProgramLocalParameter4fvARB, target, index, params);
    HOSTGL_POST
}

void mglGetProgramEnvParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramEnvParameterdvARB, target, index, params);
    HOSTGL_POST
}

void mglGetProgramEnvParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramEnvParameterfvARB, target, index, params);
    HOSTGL_POST
}

void mglGetProgramLocalParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramLocalParameterdvARB, target, index, params);
    HOSTGL_POST
}

void mglGetProgramLocalParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramLocalParameterfvARB, target, index, params);
    HOSTGL_POST
}

void mglGetProgramivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramivARB, target, pname, params);
    HOSTGL_POST
}

void mglGetProgramStringARB (GLenum target, GLenum pname, GLvoid * string)
{
    HOSTGL_PRE
    GLCALL(glGetProgramStringARB, target, pname, string);
    HOSTGL_POST
}

void mglGetVertexAttribdvARB (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribdvARB, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribfvARB (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribfvARB, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribivARB (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribivARB, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribPointervARB (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribPointervARB, index, pname, pointer);
    HOSTGL_POST
}

GLboolean mglIsProgramARB (GLuint program)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsProgramARB, program);
    HOSTGL_POST
    return _ret;
}

void mglBindBufferARB (GLenum target, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glBindBufferARB, target, buffer);
    HOSTGL_POST
}

void mglDeleteBuffersARB (GLsizei n, const GLuint * buffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteBuffersARB, n, buffers);
    HOSTGL_POST
}

void mglGenBuffersARB (GLsizei n, GLuint * buffers)
{
    HOSTGL_PRE
    GLCALL(glGenBuffersARB, n, buffers);
    HOSTGL_POST
}

GLboolean mglIsBufferARB (GLuint buffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsBufferARB, buffer);
    HOSTGL_POST
    return _ret;
}

void mglBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage)
{
    HOSTGL_PRE
    GLCALL(glBufferDataARB, target, size, data, usage);
    HOSTGL_POST
}

void mglBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glBufferSubDataARB, target, offset, size, data);
    HOSTGL_POST
}

void mglGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glGetBufferSubDataARB, target, offset, size, data);
    HOSTGL_POST
}

GLvoid* mglMapBufferARB (GLenum target, GLenum access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    _ret = GLCALL(glMapBufferARB, target, access);
    HOSTGL_POST
    return _ret;
}

GLboolean mglUnmapBufferARB (GLenum target)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glUnmapBufferARB, target);
    HOSTGL_POST
    return _ret;
}

void mglGetBufferParameterivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetBufferParameterivARB, target, pname, params);
    HOSTGL_POST
}

void mglGetBufferPointervARB (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    GLCALL(glGetBufferPointervARB, target, pname, params);
    HOSTGL_POST
}

void mglGenQueriesARB (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glGenQueriesARB, n, ids);
    HOSTGL_POST
}

void mglDeleteQueriesARB (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glDeleteQueriesARB, n, ids);
    HOSTGL_POST
}

GLboolean mglIsQueryARB (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsQueryARB, id);
    HOSTGL_POST
    return _ret;
}

void mglBeginQueryARB (GLenum target, GLuint id)
{
    HOSTGL_PRE
    GLCALL(glBeginQueryARB, target, id);
    HOSTGL_POST
}

void mglEndQueryARB (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glEndQueryARB, target);
    HOSTGL_POST
}

void mglGetQueryivARB (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryivARB, target, pname, params);
    HOSTGL_POST
}

void mglGetQueryObjectivARB (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjectivARB, id, pname, params);
    HOSTGL_POST
}

void mglGetQueryObjectuivARB (GLuint id, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjectuivARB, id, pname, params);
    HOSTGL_POST
}

void mglDeleteObjectARB (GLhandleARB obj)
{
    HOSTGL_PRE
    GLCALL(glDeleteObjectARB, obj);
    HOSTGL_POST
}

GLhandleARB mglGetHandleARB (GLenum pname)
{
    GLhandleARB _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetHandleARB, pname);
    HOSTGL_POST
    return _ret;
}

void mglDetachObjectARB (GLhandleARB containerObj, GLhandleARB attachedObj)
{
    HOSTGL_PRE
    GLCALL(glDetachObjectARB, containerObj, attachedObj);
    HOSTGL_POST
}

GLhandleARB mglCreateShaderObjectARB (GLenum shaderType)
{
    GLhandleARB _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCreateShaderObjectARB, shaderType);
    HOSTGL_POST
    return _ret;
}

void mglShaderSourceARB (GLhandleARB shaderObj, GLsizei count, const GLcharARB *  * string, const GLint * length)
{
    HOSTGL_PRE
    GLCALL(glShaderSourceARB, shaderObj, count, string, length);
    HOSTGL_POST
}

void mglCompileShaderARB (GLhandleARB shaderObj)
{
    HOSTGL_PRE
    GLCALL(glCompileShaderARB, shaderObj);
    HOSTGL_POST
}

GLhandleARB mglCreateProgramObjectARB ()
{
    GLhandleARB _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCreateProgramObjectARB);
    HOSTGL_POST
    return _ret;
}

void mglAttachObjectARB (GLhandleARB containerObj, GLhandleARB obj)
{
    HOSTGL_PRE
    GLCALL(glAttachObjectARB, containerObj, obj);
    HOSTGL_POST
}

void mglLinkProgramARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    GLCALL(glLinkProgramARB, programObj);
    HOSTGL_POST
}

void mglUseProgramObjectARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    GLCALL(glUseProgramObjectARB, programObj);
    HOSTGL_POST
}

void mglValidateProgramARB (GLhandleARB programObj)
{
    HOSTGL_PRE
    GLCALL(glValidateProgramARB, programObj);
    HOSTGL_POST
}

void mglUniform1fARB (GLint location, GLfloat v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1fARB, location, v0);
    HOSTGL_POST
}

void mglUniform2fARB (GLint location, GLfloat v0, GLfloat v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2fARB, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3fARB, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4fARB, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1iARB (GLint location, GLint v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1iARB, location, v0);
    HOSTGL_POST
}

void mglUniform2iARB (GLint location, GLint v0, GLint v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2iARB, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3iARB (GLint location, GLint v0, GLint v1, GLint v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3iARB, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4iARB (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4iARB, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1fvARB, location, count, value);
    HOSTGL_POST
}

void mglUniform2fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2fvARB, location, count, value);
    HOSTGL_POST
}

void mglUniform3fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3fvARB, location, count, value);
    HOSTGL_POST
}

void mglUniform4fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4fvARB, location, count, value);
    HOSTGL_POST
}

void mglUniform1ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1ivARB, location, count, value);
    HOSTGL_POST
}

void mglUniform2ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2ivARB, location, count, value);
    HOSTGL_POST
}

void mglUniform3ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3ivARB, location, count, value);
    HOSTGL_POST
}

void mglUniform4ivARB (GLint location, GLsizei count, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4ivARB, location, count, value);
    HOSTGL_POST
}

void mglUniformMatrix2fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix2fvARB, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix3fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix3fvARB, location, count, transpose, value);
    HOSTGL_POST
}

void mglUniformMatrix4fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glUniformMatrix4fvARB, location, count, transpose, value);
    HOSTGL_POST
}

void mglGetObjectParameterfvARB (GLhandleARB obj, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetObjectParameterfvARB, obj, pname, params);
    HOSTGL_POST
}

void mglGetObjectParameterivARB (GLhandleARB obj, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetObjectParameterivARB, obj, pname, params);
    HOSTGL_POST
}

void mglGetInfoLogARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * infoLog)
{
    HOSTGL_PRE
    GLCALL(glGetInfoLogARB, obj, maxLength, length, infoLog);
    HOSTGL_POST
}

void mglGetAttachedObjectsARB (GLhandleARB containerObj, GLsizei maxCount, GLsizei * count, GLhandleARB * obj)
{
    HOSTGL_PRE
    GLCALL(glGetAttachedObjectsARB, containerObj, maxCount, count, obj);
    HOSTGL_POST
}

GLint mglGetUniformLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetUniformLocationARB, programObj, name);
    HOSTGL_POST
    return _ret;
}

void mglGetActiveUniformARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    HOSTGL_PRE
    GLCALL(glGetActiveUniformARB, programObj, index, maxLength, length, size, type, name);
    HOSTGL_POST
}

void mglGetUniformfvARB (GLhandleARB programObj, GLint location, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformfvARB, programObj, location, params);
    HOSTGL_POST
}

void mglGetUniformivARB (GLhandleARB programObj, GLint location, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformivARB, programObj, location, params);
    HOSTGL_POST
}

void mglGetShaderSourceARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * source)
{
    HOSTGL_PRE
    GLCALL(glGetShaderSourceARB, obj, maxLength, length, source);
    HOSTGL_POST
}

void mglBindAttribLocationARB (GLhandleARB programObj, GLuint index, const GLcharARB * name)
{
    HOSTGL_PRE
    GLCALL(glBindAttribLocationARB, programObj, index, name);
    HOSTGL_POST
}

void mglGetActiveAttribARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    HOSTGL_PRE
    GLCALL(glGetActiveAttribARB, programObj, index, maxLength, length, size, type, name);
    HOSTGL_POST
}

GLint mglGetAttribLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetAttribLocationARB, programObj, name);
    HOSTGL_POST
    return _ret;
}

void mglDrawBuffersARB (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    GLCALL(glDrawBuffersARB, n, bufs);
    HOSTGL_POST
}

GLboolean mglIsRenderbuffer (GLuint renderbuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsRenderbuffer, renderbuffer);
    HOSTGL_POST
    return _ret;
}

void mglBindRenderbuffer (GLenum target, GLuint renderbuffer)
{
    HOSTGL_PRE
    GLCALL(glBindRenderbuffer, target, renderbuffer);
    HOSTGL_POST
}

void mglDeleteRenderbuffers (GLsizei n, const GLuint * renderbuffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteRenderbuffers, n, renderbuffers);
    HOSTGL_POST
}

void mglGenRenderbuffers (GLsizei n, GLuint * renderbuffers)
{
    HOSTGL_PRE
    GLCALL(glGenRenderbuffers, n, renderbuffers);
    HOSTGL_POST
}

void mglRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glRenderbufferStorage, target, internalformat, width, height);
    HOSTGL_POST
}

void mglGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetRenderbufferParameteriv, target, pname, params);
    HOSTGL_POST
}

GLboolean mglIsFramebuffer (GLuint framebuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsFramebuffer, framebuffer);
    HOSTGL_POST
    return _ret;
}

void mglBindFramebuffer (GLenum target, GLuint framebuffer)
{
    HOSTGL_PRE
    GLCALL(glBindFramebuffer, target, framebuffer);
    HOSTGL_POST
}

void mglDeleteFramebuffers (GLsizei n, const GLuint * framebuffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteFramebuffers, n, framebuffers);
    HOSTGL_POST
}

void mglGenFramebuffers (GLsizei n, GLuint * framebuffers)
{
    HOSTGL_PRE
    GLCALL(glGenFramebuffers, n, framebuffers);
    HOSTGL_POST
}

GLenum mglCheckFramebufferStatus (GLenum target)
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCheckFramebufferStatus, target);
    HOSTGL_POST
    return _ret;
}

void mglFramebufferTexture1D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture1D, target, attachment, textarget, texture, level);
    HOSTGL_POST
}

void mglFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture2D, target, attachment, textarget, texture, level);
    HOSTGL_POST
}

void mglFramebufferTexture3D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture3D, target, attachment, textarget, texture, level, zoffset);
    HOSTGL_POST
}

void mglFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    HOSTGL_PRE
    GLCALL(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
    HOSTGL_POST
}

void mglGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params);
    HOSTGL_POST
}

void mglGenerateMipmap (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glGenerateMipmap, target);
    HOSTGL_POST
}

void mglBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    HOSTGL_PRE
    GLCALL(glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    HOSTGL_POST
}

void mglRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glRenderbufferStorageMultisample, target, samples, internalformat, width, height);
    HOSTGL_POST
}

void mglFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTextureLayer, target, attachment, texture, level, layer);
    HOSTGL_POST
}

void mglBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    HOSTGL_PRE
    GLCALL(glBlendColorEXT, red, green, blue, alpha);
    HOSTGL_POST
}

void mglPolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    HOSTGL_PRE
    GLCALL(glPolygonOffsetEXT, factor, bias);
    HOSTGL_POST
}

void mglTexImage3DEXT (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexImage3DEXT, target, level, internalformat, width, height, depth, border, format, type, pixels);
    HOSTGL_POST
}

void mglTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
    HOSTGL_POST
}

void mglTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage1DEXT, target, level, xoffset, width, format, type, pixels);
    HOSTGL_POST
}

void mglTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    HOSTGL_PRE
    GLCALL(glTexSubImage2DEXT, target, level, xoffset, yoffset, width, height, format, type, pixels);
    HOSTGL_POST
}

void mglCopyTexImage1DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    HOSTGL_PRE
    GLCALL(glCopyTexImage1DEXT, target, level, internalformat, x, y, width, border);
    HOSTGL_POST
}

void mglCopyTexImage2DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    HOSTGL_PRE
    GLCALL(glCopyTexImage2DEXT, target, level, internalformat, x, y, width, height, border);
    HOSTGL_POST
}

void mglCopyTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage1DEXT, target, level, xoffset, x, y, width);
    HOSTGL_POST
}

void mglCopyTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage2DEXT, target, level, xoffset, yoffset, x, y, width, height);
    HOSTGL_POST
}

void mglCopyTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, x, y, width, height);
    HOSTGL_POST
}

GLboolean mglAreTexturesResidentEXT (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glAreTexturesResidentEXT, n, textures, residences);
    HOSTGL_POST
    return _ret;
}

void mglBindTextureEXT (GLenum target, GLuint texture)
{
    HOSTGL_PRE
    GLCALL(glBindTextureEXT, target, texture);
    HOSTGL_POST
}

void mglDeleteTexturesEXT (GLsizei n, const GLuint * textures)
{
    HOSTGL_PRE
    GLCALL(glDeleteTexturesEXT, n, textures);
    HOSTGL_POST
}

void mglGenTexturesEXT (GLsizei n, GLuint * textures)
{
    HOSTGL_PRE
    GLCALL(glGenTexturesEXT, n, textures);
    HOSTGL_POST
}

GLboolean mglIsTextureEXT (GLuint texture)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsTextureEXT, texture);
    HOSTGL_POST
    return _ret;
}

void mglPrioritizeTexturesEXT (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    HOSTGL_PRE
    GLCALL(glPrioritizeTexturesEXT, n, textures, priorities);
    HOSTGL_POST
}

void mglArrayElementEXT (GLint i)
{
    HOSTGL_PRE
    GLCALL(glArrayElementEXT, i);
    HOSTGL_POST
}

void mglColorPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glColorPointerEXT, size, type, stride, count, pointer);
    HOSTGL_POST
}

void mglDrawArraysEXT (GLenum mode, GLint first, GLsizei count)
{
    HOSTGL_PRE
    GLCALL(glDrawArraysEXT, mode, first, count);
    HOSTGL_POST
}

void mglEdgeFlagPointerEXT (GLsizei stride, GLsizei count, const GLboolean * pointer)
{
    HOSTGL_PRE
    GLCALL(glEdgeFlagPointerEXT, stride, count, pointer);
    HOSTGL_POST
}

void mglGetPointervEXT (GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    GLCALL(glGetPointervEXT, pname, params);
    HOSTGL_POST
}

void mglIndexPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glIndexPointerEXT, type, stride, count, pointer);
    HOSTGL_POST
}

void mglNormalPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glNormalPointerEXT, type, stride, count, pointer);
    HOSTGL_POST
}

void mglTexCoordPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glTexCoordPointerEXT, size, type, stride, count, pointer);
    HOSTGL_POST
}

void mglVertexPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexPointerEXT, size, type, stride, count, pointer);
    HOSTGL_POST
}

void mglBlendEquationEXT (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationEXT, mode);
    HOSTGL_POST
}

void mglPointParameterfEXT (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfEXT, pname, param);
    HOSTGL_POST
}

void mglPointParameterfvEXT (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfvEXT, pname, params);
    HOSTGL_POST
}

void mglColorTableEXT (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glColorTableEXT, target, internalFormat, width, format, type, table);
    HOSTGL_POST
}

void mglGetColorTableEXT (GLenum target, GLenum format, GLenum type, GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableEXT, target, format, type, data);
    HOSTGL_POST
}

void mglGetColorTableParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameterivEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetColorTableParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameterfvEXT, target, pname, params);
    HOSTGL_POST
}

void mglLockArraysEXT (GLint first, GLsizei count)
{
    HOSTGL_PRE
    GLCALL(glLockArraysEXT, first, count);
    HOSTGL_POST
}

void mglUnlockArraysEXT ()
{
    HOSTGL_PRE
    GLCALL(glUnlockArraysEXT);
    HOSTGL_POST
}

void mglDrawRangeElementsEXT (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    HOSTGL_PRE
    GLCALL(glDrawRangeElementsEXT, mode, start, end, count, type, indices);
    HOSTGL_POST
}

void mglSecondaryColor3bEXT (GLbyte red, GLbyte green, GLbyte blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3bEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3bvEXT (const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3bvEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3dEXT (GLdouble red, GLdouble green, GLdouble blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3dEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3dvEXT (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3dvEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3fEXT (GLfloat red, GLfloat green, GLfloat blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3fEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3fvEXT (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3fvEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3iEXT (GLint red, GLint green, GLint blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3iEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3ivEXT (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ivEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3sEXT (GLshort red, GLshort green, GLshort blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3sEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3svEXT (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3svEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3ubEXT (GLubyte red, GLubyte green, GLubyte blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ubEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3ubvEXT (const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3ubvEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3uiEXT (GLuint red, GLuint green, GLuint blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3uiEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3uivEXT (const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3uivEXT, v);
    HOSTGL_POST
}

void mglSecondaryColor3usEXT (GLushort red, GLushort green, GLushort blue)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3usEXT, red, green, blue);
    HOSTGL_POST
}

void mglSecondaryColor3usvEXT (const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColor3usvEXT, v);
    HOSTGL_POST
}

void mglSecondaryColorPointerEXT (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glSecondaryColorPointerEXT, size, type, stride, pointer);
    HOSTGL_POST
}

void mglMultiDrawArraysEXT (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glMultiDrawArraysEXT, mode, first, count, primcount);
    HOSTGL_POST
}

void mglMultiDrawElementsEXT (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glMultiDrawElementsEXT, mode, count, type, indices, primcount);
    HOSTGL_POST
}

void mglFogCoordfEXT (GLfloat coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoordfEXT, coord);
    HOSTGL_POST
}

void mglFogCoordfvEXT (const GLfloat * coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoordfvEXT, coord);
    HOSTGL_POST
}

void mglFogCoorddEXT (GLdouble coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoorddEXT, coord);
    HOSTGL_POST
}

void mglFogCoorddvEXT (const GLdouble * coord)
{
    HOSTGL_PRE
    GLCALL(glFogCoorddvEXT, coord);
    HOSTGL_POST
}

void mglFogCoordPointerEXT (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glFogCoordPointerEXT, type, stride, pointer);
    HOSTGL_POST
}

void mglBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncSeparateEXT, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    HOSTGL_POST
}

void mglFlushVertexArrayRangeNV ()
{
    HOSTGL_PRE
    GLCALL(glFlushVertexArrayRangeNV);
    HOSTGL_POST
}

void mglVertexArrayRangeNV (GLsizei length, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexArrayRangeNV, length, pointer);
    HOSTGL_POST
}

void mglCombinerParameterfvNV (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glCombinerParameterfvNV, pname, params);
    HOSTGL_POST
}

void mglCombinerParameterfNV (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glCombinerParameterfNV, pname, param);
    HOSTGL_POST
}

void mglCombinerParameterivNV (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glCombinerParameterivNV, pname, params);
    HOSTGL_POST
}

void mglCombinerParameteriNV (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glCombinerParameteriNV, pname, param);
    HOSTGL_POST
}

void mglCombinerInputNV (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    HOSTGL_PRE
    GLCALL(glCombinerInputNV, stage, portion, variable, input, mapping, componentUsage);
    HOSTGL_POST
}

void mglCombinerOutputNV (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum)
{
    HOSTGL_PRE
    GLCALL(glCombinerOutputNV, stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum);
    HOSTGL_POST
}

void mglFinalCombinerInputNV (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    HOSTGL_PRE
    GLCALL(glFinalCombinerInputNV, variable, input, mapping, componentUsage);
    HOSTGL_POST
}

void mglGetCombinerInputParameterfvNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetCombinerInputParameterfvNV, stage, portion, variable, pname, params);
    HOSTGL_POST
}

void mglGetCombinerInputParameterivNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetCombinerInputParameterivNV, stage, portion, variable, pname, params);
    HOSTGL_POST
}

void mglGetCombinerOutputParameterfvNV (GLenum stage, GLenum portion, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetCombinerOutputParameterfvNV, stage, portion, pname, params);
    HOSTGL_POST
}

void mglGetCombinerOutputParameterivNV (GLenum stage, GLenum portion, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetCombinerOutputParameterivNV, stage, portion, pname, params);
    HOSTGL_POST
}

void mglGetFinalCombinerInputParameterfvNV (GLenum variable, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetFinalCombinerInputParameterfvNV, variable, pname, params);
    HOSTGL_POST
}

void mglGetFinalCombinerInputParameterivNV (GLenum variable, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetFinalCombinerInputParameterivNV, variable, pname, params);
    HOSTGL_POST
}

void mglResizeBuffersMESA ()
{
    HOSTGL_PRE
    GLCALL(glResizeBuffersMESA);
    HOSTGL_POST
}

void mglWindowPos2dMESA (GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2dMESA, x, y);
    HOSTGL_POST
}

void mglWindowPos2dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2dvMESA, v);
    HOSTGL_POST
}

void mglWindowPos2fMESA (GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2fMESA, x, y);
    HOSTGL_POST
}

void mglWindowPos2fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2fvMESA, v);
    HOSTGL_POST
}

void mglWindowPos2iMESA (GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2iMESA, x, y);
    HOSTGL_POST
}

void mglWindowPos2ivMESA (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2ivMESA, v);
    HOSTGL_POST
}

void mglWindowPos2sMESA (GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2sMESA, x, y);
    HOSTGL_POST
}

void mglWindowPos2svMESA (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos2svMESA, v);
    HOSTGL_POST
}

void mglWindowPos3dMESA (GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3dMESA, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3dvMESA, v);
    HOSTGL_POST
}

void mglWindowPos3fMESA (GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3fMESA, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3fvMESA, v);
    HOSTGL_POST
}

void mglWindowPos3iMESA (GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3iMESA, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3ivMESA (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3ivMESA, v);
    HOSTGL_POST
}

void mglWindowPos3sMESA (GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3sMESA, x, y, z);
    HOSTGL_POST
}

void mglWindowPos3svMESA (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos3svMESA, v);
    HOSTGL_POST
}

void mglWindowPos4dMESA (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4dMESA, x, y, z, w);
    HOSTGL_POST
}

void mglWindowPos4dvMESA (const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4dvMESA, v);
    HOSTGL_POST
}

void mglWindowPos4fMESA (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4fMESA, x, y, z, w);
    HOSTGL_POST
}

void mglWindowPos4fvMESA (const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4fvMESA, v);
    HOSTGL_POST
}

void mglWindowPos4iMESA (GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4iMESA, x, y, z, w);
    HOSTGL_POST
}

void mglWindowPos4ivMESA (const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4ivMESA, v);
    HOSTGL_POST
}

void mglWindowPos4sMESA (GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4sMESA, x, y, z, w);
    HOSTGL_POST
}

void mglWindowPos4svMESA (const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glWindowPos4svMESA, v);
    HOSTGL_POST
}

GLboolean mglAreProgramsResidentNV (GLsizei n, const GLuint * programs, GLboolean * residences)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glAreProgramsResidentNV, n, programs, residences);
    HOSTGL_POST
    return _ret;
}

void mglBindProgramNV (GLenum target, GLuint id)
{
    HOSTGL_PRE
    GLCALL(glBindProgramNV, target, id);
    HOSTGL_POST
}

void mglDeleteProgramsNV (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    GLCALL(glDeleteProgramsNV, n, programs);
    HOSTGL_POST
}

void mglExecuteProgramNV (GLenum target, GLuint id, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glExecuteProgramNV, target, id, params);
    HOSTGL_POST
}

void mglGenProgramsNV (GLsizei n, GLuint * programs)
{
    HOSTGL_PRE
    GLCALL(glGenProgramsNV, n, programs);
    HOSTGL_POST
}

void mglGetProgramParameterdvNV (GLenum target, GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramParameterdvNV, target, index, pname, params);
    HOSTGL_POST
}

void mglGetProgramParameterfvNV (GLenum target, GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramParameterfvNV, target, index, pname, params);
    HOSTGL_POST
}

void mglGetProgramivNV (GLuint id, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramivNV, id, pname, params);
    HOSTGL_POST
}

void mglGetProgramStringNV (GLuint id, GLenum pname, GLubyte * program)
{
    HOSTGL_PRE
    GLCALL(glGetProgramStringNV, id, pname, program);
    HOSTGL_POST
}

void mglGetTrackMatrixivNV (GLenum target, GLuint address, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTrackMatrixivNV, target, address, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribdvNV (GLuint index, GLenum pname, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribdvNV, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribfvNV (GLuint index, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribfvNV, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribivNV (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribivNV, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribPointervNV (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribPointervNV, index, pname, pointer);
    HOSTGL_POST
}

GLboolean mglIsProgramNV (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsProgramNV, id);
    HOSTGL_POST
    return _ret;
}

void mglLoadProgramNV (GLenum target, GLuint id, GLsizei len, const GLubyte * program)
{
    HOSTGL_PRE
    GLCALL(glLoadProgramNV, target, id, len, program);
    HOSTGL_POST
}

void mglProgramParameter4dNV (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glProgramParameter4dNV, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramParameter4dvNV (GLenum target, GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glProgramParameter4dvNV, target, index, v);
    HOSTGL_POST
}

void mglProgramParameter4fNV (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glProgramParameter4fNV, target, index, x, y, z, w);
    HOSTGL_POST
}

void mglProgramParameter4fvNV (GLenum target, GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glProgramParameter4fvNV, target, index, v);
    HOSTGL_POST
}

void mglProgramParameters4dvNV (GLenum target, GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glProgramParameters4dvNV, target, index, count, v);
    HOSTGL_POST
}

void mglProgramParameters4fvNV (GLenum target, GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glProgramParameters4fvNV, target, index, count, v);
    HOSTGL_POST
}

void mglRequestResidentProgramsNV (GLsizei n, const GLuint * programs)
{
    HOSTGL_PRE
    GLCALL(glRequestResidentProgramsNV, n, programs);
    HOSTGL_POST
}

void mglTrackMatrixNV (GLenum target, GLuint address, GLenum matrix, GLenum transform)
{
    HOSTGL_PRE
    GLCALL(glTrackMatrixNV, target, address, matrix, transform);
    HOSTGL_POST
}

void mglVertexAttribPointerNV (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribPointerNV, index, fsize, type, stride, pointer);
    HOSTGL_POST
}

void mglVertexAttrib1dNV (GLuint index, GLdouble x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1dNV, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1dvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1fNV (GLuint index, GLfloat x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1fNV, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1fvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib1sNV (GLuint index, GLshort x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1sNV, index, x);
    HOSTGL_POST
}

void mglVertexAttrib1svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib1svNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2dNV (GLuint index, GLdouble x, GLdouble y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2dNV, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2dvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2fNV (GLuint index, GLfloat x, GLfloat y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2fNV, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2fvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib2sNV (GLuint index, GLshort x, GLshort y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2sNV, index, x, y);
    HOSTGL_POST
}

void mglVertexAttrib2svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib2svNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3dNV, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3dvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3fNV, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3fvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib3sNV (GLuint index, GLshort x, GLshort y, GLshort z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3sNV, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttrib3svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib3svNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4dNV, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4dvNV (GLuint index, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4dvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4fNV, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4fvNV (GLuint index, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4fvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4sNV (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4sNV, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4svNV (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4svNV, index, v);
    HOSTGL_POST
}

void mglVertexAttrib4ubNV (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4ubNV, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttrib4ubvNV (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttrib4ubvNV, index, v);
    HOSTGL_POST
}

void mglVertexAttribs1dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs1dvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs1fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs1fvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs1svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs1svNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs2dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs2dvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs2fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs2fvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs2svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs2svNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs3dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs3dvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs3fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs3fvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs3svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs3svNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs4dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs4dvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs4fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs4fvNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs4svNV (GLuint index, GLsizei count, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs4svNV, index, count, v);
    HOSTGL_POST
}

void mglVertexAttribs4ubvNV (GLuint index, GLsizei count, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribs4ubvNV, index, count, v);
    HOSTGL_POST
}

void mglTexBumpParameterivATI (GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    GLCALL(glTexBumpParameterivATI, pname, param);
    HOSTGL_POST
}

void mglTexBumpParameterfvATI (GLenum pname, const GLfloat * param)
{
    HOSTGL_PRE
    GLCALL(glTexBumpParameterfvATI, pname, param);
    HOSTGL_POST
}

void mglGetTexBumpParameterivATI (GLenum pname, GLint * param)
{
    HOSTGL_PRE
    GLCALL(glGetTexBumpParameterivATI, pname, param);
    HOSTGL_POST
}

void mglGetTexBumpParameterfvATI (GLenum pname, GLfloat * param)
{
    HOSTGL_PRE
    GLCALL(glGetTexBumpParameterfvATI, pname, param);
    HOSTGL_POST
}

GLuint mglGenFragmentShadersATI (GLuint range)
{
    GLuint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGenFragmentShadersATI, range);
    HOSTGL_POST
    return _ret;
}

void mglBindFragmentShaderATI (GLuint id)
{
    HOSTGL_PRE
    GLCALL(glBindFragmentShaderATI, id);
    HOSTGL_POST
}

void mglDeleteFragmentShaderATI (GLuint id)
{
    HOSTGL_PRE
    GLCALL(glDeleteFragmentShaderATI, id);
    HOSTGL_POST
}

void mglBeginFragmentShaderATI ()
{
    HOSTGL_PRE
    GLCALL(glBeginFragmentShaderATI);
    HOSTGL_POST
}

void mglEndFragmentShaderATI ()
{
    HOSTGL_PRE
    GLCALL(glEndFragmentShaderATI);
    HOSTGL_POST
}

void mglPassTexCoordATI (GLuint dst, GLuint coord, GLenum swizzle)
{
    HOSTGL_PRE
    GLCALL(glPassTexCoordATI, dst, coord, swizzle);
    HOSTGL_POST
}

void mglSampleMapATI (GLuint dst, GLuint interp, GLenum swizzle)
{
    HOSTGL_PRE
    GLCALL(glSampleMapATI, dst, interp, swizzle);
    HOSTGL_POST
}

void mglColorFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    HOSTGL_PRE
    GLCALL(glColorFragmentOp1ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod);
    HOSTGL_POST
}

void mglColorFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    HOSTGL_PRE
    GLCALL(glColorFragmentOp2ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
    HOSTGL_POST
}

void mglColorFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    HOSTGL_PRE
    GLCALL(glColorFragmentOp3ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
    HOSTGL_POST
}

void mglAlphaFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    HOSTGL_PRE
    GLCALL(glAlphaFragmentOp1ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod);
    HOSTGL_POST
}

void mglAlphaFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    HOSTGL_PRE
    GLCALL(glAlphaFragmentOp2ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
    HOSTGL_POST
}

void mglAlphaFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    HOSTGL_PRE
    GLCALL(glAlphaFragmentOp3ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
    HOSTGL_POST
}

void mglSetFragmentShaderConstantATI (GLuint dst, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glSetFragmentShaderConstantATI, dst, value);
    HOSTGL_POST
}

void mglPointParameteriNV (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glPointParameteriNV, pname, param);
    HOSTGL_POST
}

void mglPointParameterivNV (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameterivNV, pname, params);
    HOSTGL_POST
}

void mglDrawBuffersATI (GLsizei n, const GLenum * bufs)
{
    HOSTGL_PRE
    GLCALL(glDrawBuffersATI, n, bufs);
    HOSTGL_POST
}

void mglProgramNamedParameter4fNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    HOSTGL_PRE
    GLCALL(glProgramNamedParameter4fNV, id, len, name, x, y, z, w);
    HOSTGL_POST
}

void mglProgramNamedParameter4dNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    HOSTGL_PRE
    GLCALL(glProgramNamedParameter4dNV, id, len, name, x, y, z, w);
    HOSTGL_POST
}

void mglProgramNamedParameter4fvNV (GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glProgramNamedParameter4fvNV, id, len, name, v);
    HOSTGL_POST
}

void mglProgramNamedParameter4dvNV (GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glProgramNamedParameter4dvNV, id, len, name, v);
    HOSTGL_POST
}

void mglGetProgramNamedParameterfvNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramNamedParameterfvNV, id, len, name, params);
    HOSTGL_POST
}

void mglGetProgramNamedParameterdvNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetProgramNamedParameterdvNV, id, len, name, params);
    HOSTGL_POST
}

GLboolean mglIsRenderbufferEXT (GLuint renderbuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsRenderbufferEXT, renderbuffer);
    HOSTGL_POST
    return _ret;
}

void mglBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
    HOSTGL_PRE
    GLCALL(glBindRenderbufferEXT, target, renderbuffer);
    HOSTGL_POST
}

void mglDeleteRenderbuffersEXT (GLsizei n, const GLuint * renderbuffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteRenderbuffersEXT, n, renderbuffers);
    HOSTGL_POST
}

void mglGenRenderbuffersEXT (GLsizei n, GLuint * renderbuffers)
{
    HOSTGL_PRE
    GLCALL(glGenRenderbuffersEXT, n, renderbuffers);
    HOSTGL_POST
}

void mglRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glRenderbufferStorageEXT, target, internalformat, width, height);
    HOSTGL_POST
}

void mglGetRenderbufferParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetRenderbufferParameterivEXT, target, pname, params);
    HOSTGL_POST
}

GLboolean mglIsFramebufferEXT (GLuint framebuffer)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsFramebufferEXT, framebuffer);
    HOSTGL_POST
    return _ret;
}

void mglBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
    HOSTGL_PRE
    GLCALL(glBindFramebufferEXT, target, framebuffer);
    HOSTGL_POST
}

void mglDeleteFramebuffersEXT (GLsizei n, const GLuint * framebuffers)
{
    HOSTGL_PRE
    GLCALL(glDeleteFramebuffersEXT, n, framebuffers);
    HOSTGL_POST
}

void mglGenFramebuffersEXT (GLsizei n, GLuint * framebuffers)
{
    HOSTGL_PRE
    GLCALL(glGenFramebuffersEXT, n, framebuffers);
    HOSTGL_POST
}

GLenum mglCheckFramebufferStatusEXT (GLenum target)
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCheckFramebufferStatusEXT, target);
    HOSTGL_POST
    return _ret;
}

void mglFramebufferTexture1DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture1DEXT, target, attachment, textarget, texture, level);
    HOSTGL_POST
}

void mglFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture2DEXT, target, attachment, textarget, texture, level);
    HOSTGL_POST
}

void mglFramebufferTexture3DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture3DEXT, target, attachment, textarget, texture, level, zoffset);
    HOSTGL_POST
}

void mglFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    HOSTGL_PRE
    GLCALL(glFramebufferRenderbufferEXT, target, attachment, renderbuffertarget, renderbuffer);
    HOSTGL_POST
}

void mglGetFramebufferAttachmentParameterivEXT (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetFramebufferAttachmentParameterivEXT, target, attachment, pname, params);
    HOSTGL_POST
}

void mglGenerateMipmapEXT (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glGenerateMipmapEXT, target);
    HOSTGL_POST
}

void mglFramebufferTextureLayerEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTextureLayerEXT, target, attachment, texture, level, layer);
    HOSTGL_POST
}

GLvoid* mglMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    GLvoid* _ret;
    HOSTGL_PRE
    _ret = GLCALL(glMapBufferRange, target, offset, length, access);
    HOSTGL_POST
    return _ret;
}

void mglFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length)
{
    HOSTGL_PRE
    GLCALL(glFlushMappedBufferRange, target, offset, length);
    HOSTGL_POST
}

void mglBindVertexArray (GLuint array)
{
    HOSTGL_PRE
    GLCALL(glBindVertexArray, array);
    HOSTGL_POST
}

void mglDeleteVertexArrays (GLsizei n, const GLuint * arrays)
{
    HOSTGL_PRE
    GLCALL(glDeleteVertexArrays, n, arrays);
    HOSTGL_POST
}

void mglGenVertexArrays (GLsizei n, GLuint * arrays)
{
    HOSTGL_PRE
    GLCALL(glGenVertexArrays, n, arrays);
    HOSTGL_POST
}

GLboolean mglIsVertexArray (GLuint array)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsVertexArray, array);
    HOSTGL_POST
    return _ret;
}

void mglCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    HOSTGL_PRE
    GLCALL(glCopyBufferSubData, readTarget, writeTarget, readOffset, writeOffset, size);
    HOSTGL_POST
}

GLsync mglFenceSync (GLenum condition, GLbitfield flags)
{
    GLsync _ret;
    HOSTGL_PRE
    _ret = GLCALL(glFenceSync, condition, flags);
    HOSTGL_POST
    return _ret;
}

GLboolean mglIsSync (GLsync sync)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsSync, sync);
    HOSTGL_POST
    return _ret;
}

void mglDeleteSync (GLsync sync)
{
    HOSTGL_PRE
    GLCALL(glDeleteSync, sync);
    HOSTGL_POST
}

GLenum mglClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glClientWaitSync, sync, flags, timeout);
    HOSTGL_POST
    return _ret;
}

void mglWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    HOSTGL_PRE
    GLCALL(glWaitSync, sync, flags, timeout);
    HOSTGL_POST
}

void mglGetInteger64v (GLenum pname, GLint64 * params)
{
    HOSTGL_PRE
    GLCALL(glGetInteger64v, pname, params);
    HOSTGL_POST
}

void mglGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values)
{
    HOSTGL_PRE
    GLCALL(glGetSynciv, sync, pname, bufSize, length, values);
    HOSTGL_POST
}

void mglProvokingVertexEXT (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glProvokingVertexEXT, mode);
    HOSTGL_POST
}

void mglDrawElementsBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    HOSTGL_PRE
    GLCALL(glDrawElementsBaseVertex, mode, count, type, indices, basevertex);
    HOSTGL_POST
}

void mglDrawRangeElementsBaseVertex (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    HOSTGL_PRE
    GLCALL(glDrawRangeElementsBaseVertex, mode, start, end, count, type, indices, basevertex);
    HOSTGL_POST
}

void mglMultiDrawElementsBaseVertex (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount, const GLint * basevertex)
{
    HOSTGL_PRE
    GLCALL(glMultiDrawElementsBaseVertex, mode, count, type, indices, primcount, basevertex);
    HOSTGL_POST
}

void mglProvokingVertex (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glProvokingVertex, mode);
    HOSTGL_POST
}

void mglRenderbufferStorageMultisampleEXT (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glRenderbufferStorageMultisampleEXT, target, samples, internalformat, width, height);
    HOSTGL_POST
}

void mglColorMaskIndexedEXT (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    HOSTGL_PRE
    GLCALL(glColorMaskIndexedEXT, index, r, g, b, a);
    HOSTGL_POST
}

void mglGetBooleanIndexedvEXT (GLenum target, GLuint index, GLboolean * data)
{
    HOSTGL_PRE
    GLCALL(glGetBooleanIndexedvEXT, target, index, data);
    HOSTGL_POST
}

void mglGetIntegerIndexedvEXT (GLenum target, GLuint index, GLint * data)
{
    HOSTGL_PRE
    GLCALL(glGetIntegerIndexedvEXT, target, index, data);
    HOSTGL_POST
}

void mglEnableIndexedEXT (GLenum target, GLuint index)
{
    HOSTGL_PRE
    GLCALL(glEnableIndexedEXT, target, index);
    HOSTGL_POST
}

void mglDisableIndexedEXT (GLenum target, GLuint index)
{
    HOSTGL_PRE
    GLCALL(glDisableIndexedEXT, target, index);
    HOSTGL_POST
}

GLboolean mglIsEnabledIndexedEXT (GLenum target, GLuint index)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsEnabledIndexedEXT, target, index);
    HOSTGL_POST
    return _ret;
}

void mglBeginConditionalRenderNV (GLuint id, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBeginConditionalRenderNV, id, mode);
    HOSTGL_POST
}

void mglEndConditionalRenderNV ()
{
    HOSTGL_PRE
    GLCALL(glEndConditionalRenderNV);
    HOSTGL_POST
}

GLenum mglObjectPurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glObjectPurgeableAPPLE, objectType, name, option);
    HOSTGL_POST
    return _ret;
}

GLenum mglObjectUnpurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glObjectUnpurgeableAPPLE, objectType, name, option);
    HOSTGL_POST
    return _ret;
}

void mglGetObjectParameterivAPPLE (GLenum objectType, GLuint name, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetObjectParameterivAPPLE, objectType, name, pname, params);
    HOSTGL_POST
}

void mglBeginTransformFeedback (GLenum primitiveMode)
{
    HOSTGL_PRE
    GLCALL(glBeginTransformFeedback, primitiveMode);
    HOSTGL_POST
}

void mglEndTransformFeedback ()
{
    HOSTGL_PRE
    GLCALL(glEndTransformFeedback);
    HOSTGL_POST
}

void mglBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    GLCALL(glBindBufferRange, target, index, buffer, offset, size);
    HOSTGL_POST
}

void mglBindBufferBase (GLenum target, GLuint index, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glBindBufferBase, target, index, buffer);
    HOSTGL_POST
}

void mglTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    HOSTGL_PRE
    GLCALL(glTransformFeedbackVaryings, program, count, varyings, bufferMode);
    HOSTGL_POST
}

void mglGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glGetTransformFeedbackVarying, program, index, bufSize, length, size, type, name);
    HOSTGL_POST
}

void mglDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawArraysInstanced, mode, first, count, primcount);
    HOSTGL_POST
}

void mglDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawElementsInstanced, mode, count, type, indices, primcount);
    HOSTGL_POST
}

void mglDrawArraysInstancedARB (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawArraysInstancedARB, mode, first, count, primcount);
    HOSTGL_POST
}

void mglDrawElementsInstancedARB (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawElementsInstancedARB, mode, count, type, indices, primcount);
    HOSTGL_POST
}

void mglProgramParameteriARB (GLuint program, GLenum pname, GLint value)
{
    HOSTGL_PRE
    GLCALL(glProgramParameteriARB, program, pname, value);
    HOSTGL_POST
}

void mglFramebufferTextureARB (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTextureARB, target, attachment, texture, level);
    HOSTGL_POST
}

void mglFramebufferTextureFaceARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTextureFaceARB, target, attachment, texture, level, face);
    HOSTGL_POST
}

void mglBindTransformFeedback (GLenum target, GLuint id)
{
    HOSTGL_PRE
    GLCALL(glBindTransformFeedback, target, id);
    HOSTGL_POST
}

void mglDeleteTransformFeedbacks (GLsizei n, const GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glDeleteTransformFeedbacks, n, ids);
    HOSTGL_POST
}

void mglGenTransformFeedbacks (GLsizei n, GLuint * ids)
{
    HOSTGL_PRE
    GLCALL(glGenTransformFeedbacks, n, ids);
    HOSTGL_POST
}

GLboolean mglIsTransformFeedback (GLuint id)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsTransformFeedback, id);
    HOSTGL_POST
    return _ret;
}

void mglPauseTransformFeedback ()
{
    HOSTGL_PRE
    GLCALL(glPauseTransformFeedback);
    HOSTGL_POST
}

void mglResumeTransformFeedback ()
{
    HOSTGL_PRE
    GLCALL(glResumeTransformFeedback);
    HOSTGL_POST
}

void mglDrawTransformFeedback (GLenum mode, GLuint id)
{
    HOSTGL_PRE
    GLCALL(glDrawTransformFeedback, mode, id);
    HOSTGL_POST
}

void mglDrawArraysInstancedEXT (GLenum mode, GLint start, GLsizei count, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawArraysInstancedEXT, mode, start, count, primcount);
    HOSTGL_POST
}

void mglDrawElementsInstancedEXT (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    HOSTGL_PRE
    GLCALL(glDrawElementsInstancedEXT, mode, count, type, indices, primcount);
    HOSTGL_POST
}

void mglBeginTransformFeedbackEXT (GLenum primitiveMode)
{
    HOSTGL_PRE
    GLCALL(glBeginTransformFeedbackEXT, primitiveMode);
    HOSTGL_POST
}

void mglEndTransformFeedbackEXT ()
{
    HOSTGL_PRE
    GLCALL(glEndTransformFeedbackEXT);
    HOSTGL_POST
}

void mglBindBufferRangeEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    GLCALL(glBindBufferRangeEXT, target, index, buffer, offset, size);
    HOSTGL_POST
}

void mglBindBufferOffsetEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset)
{
    HOSTGL_PRE
    GLCALL(glBindBufferOffsetEXT, target, index, buffer, offset);
    HOSTGL_POST
}

void mglBindBufferBaseEXT (GLenum target, GLuint index, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glBindBufferBaseEXT, target, index, buffer);
    HOSTGL_POST
}

void mglTransformFeedbackVaryingsEXT (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    HOSTGL_PRE
    GLCALL(glTransformFeedbackVaryingsEXT, program, count, varyings, bufferMode);
    HOSTGL_POST
}

void mglGetTransformFeedbackVaryingEXT (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glGetTransformFeedbackVaryingEXT, program, index, bufSize, length, size, type, name);
    HOSTGL_POST
}

void mglEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image)
{
    HOSTGL_PRE
    GLCALL(glEGLImageTargetTexture2DOES, target, image);
    HOSTGL_POST
}

void mglEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image)
{
    HOSTGL_PRE
    GLCALL(glEGLImageTargetRenderbufferStorageOES, target, image);
    HOSTGL_POST
}

void mglColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    HOSTGL_PRE
    GLCALL(glColorMaski, index, r, g, b, a);
    HOSTGL_POST
}

void mglGetBooleani_v (GLenum target, GLuint index, GLboolean * data)
{
    HOSTGL_PRE
    GLCALL(glGetBooleani_v, target, index, data);
    HOSTGL_POST
}

void mglGetIntegeri_v (GLenum target, GLuint index, GLint * data)
{
    HOSTGL_PRE
    GLCALL(glGetIntegeri_v, target, index, data);
    HOSTGL_POST
}

void mglEnablei (GLenum target, GLuint index)
{
    HOSTGL_PRE
    GLCALL(glEnablei, target, index);
    HOSTGL_POST
}

void mglDisablei (GLenum target, GLuint index)
{
    HOSTGL_PRE
    GLCALL(glDisablei, target, index);
    HOSTGL_POST
}

GLboolean mglIsEnabledi (GLenum target, GLuint index)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsEnabledi, target, index);
    HOSTGL_POST
    return _ret;
}

void mglClampColor (GLenum target, GLenum clamp)
{
    HOSTGL_PRE
    GLCALL(glClampColor, target, clamp);
    HOSTGL_POST
}

void mglBeginConditionalRender (GLuint id, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBeginConditionalRender, id, mode);
    HOSTGL_POST
}

void mglEndConditionalRender ()
{
    HOSTGL_PRE
    GLCALL(glEndConditionalRender);
    HOSTGL_POST
}

void mglVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribIPointer, index, size, type, stride, pointer);
    HOSTGL_POST
}

void mglGetVertexAttribIiv (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribIiv, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribIuiv, index, pname, params);
    HOSTGL_POST
}

void mglVertexAttribI1i (GLuint index, GLint x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1i, index, x);
    HOSTGL_POST
}

void mglVertexAttribI2i (GLuint index, GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2i, index, x, y);
    HOSTGL_POST
}

void mglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3i, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4i, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttribI1ui (GLuint index, GLuint x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1ui, index, x);
    HOSTGL_POST
}

void mglVertexAttribI2ui (GLuint index, GLuint x, GLuint y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2ui, index, x, y);
    HOSTGL_POST
}

void mglVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3ui, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4ui, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttribI1iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1iv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI2iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2iv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI3iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3iv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4iv (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4iv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI1uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1uiv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI2uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2uiv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI3uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3uiv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4uiv (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4uiv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4bv (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4bv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4sv (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4sv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4ubv (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4ubv, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4usv (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4usv, index, v);
    HOSTGL_POST
}

void mglGetUniformuiv (GLuint program, GLint location, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformuiv, program, location, params);
    HOSTGL_POST
}

void mglBindFragDataLocation (GLuint program, GLuint color, const GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glBindFragDataLocation, program, color, name);
    HOSTGL_POST
}

GLint mglGetFragDataLocation (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetFragDataLocation, program, name);
    HOSTGL_POST
    return _ret;
}

void mglUniform1ui (GLint location, GLuint v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1ui, location, v0);
    HOSTGL_POST
}

void mglUniform2ui (GLint location, GLuint v0, GLuint v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2ui, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3ui, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4ui, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1uiv, location, count, value);
    HOSTGL_POST
}

void mglUniform2uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2uiv, location, count, value);
    HOSTGL_POST
}

void mglUniform3uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3uiv, location, count, value);
    HOSTGL_POST
}

void mglUniform4uiv (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4uiv, location, count, value);
    HOSTGL_POST
}

void mglTexParameterIiv (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameterIiv, target, pname, params);
    HOSTGL_POST
}

void mglTexParameterIuiv (GLenum target, GLenum pname, const GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameterIuiv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameterIiv (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterIiv, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameterIuiv (GLenum target, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterIuiv, target, pname, params);
    HOSTGL_POST
}

void mglClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint * value)
{
    HOSTGL_PRE
    GLCALL(glClearBufferiv, buffer, drawbuffer, value);
    HOSTGL_POST
}

void mglClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glClearBufferuiv, buffer, drawbuffer, value);
    HOSTGL_POST
}

void mglClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat * value)
{
    HOSTGL_PRE
    GLCALL(glClearBufferfv, buffer, drawbuffer, value);
    HOSTGL_POST
}

void mglClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    HOSTGL_PRE
    GLCALL(glClearBufferfi, buffer, drawbuffer, depth, stencil);
    HOSTGL_POST
}

const GLubyte * mglGetStringi (GLenum name, GLuint index)
{
    const GLubyte * _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetStringi, name, index);
    HOSTGL_POST
    return _ret;
}

void mglTexBuffer (GLenum target, GLenum internalformat, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glTexBuffer, target, internalformat, buffer);
    HOSTGL_POST
}

void mglPrimitiveRestartIndex (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glPrimitiveRestartIndex, index);
    HOSTGL_POST
}

void mglGetInteger64i_v (GLenum target, GLuint index, GLint64 * data)
{
    HOSTGL_PRE
    GLCALL(glGetInteger64i_v, target, index, data);
    HOSTGL_POST
}

void mglGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 * params)
{
    HOSTGL_PRE
    GLCALL(glGetBufferParameteri64v, target, pname, params);
    HOSTGL_POST
}

void mglFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTexture, target, attachment, texture, level);
    HOSTGL_POST
}

void mglVertexAttribDivisor (GLuint index, GLuint divisor)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribDivisor, index, divisor);
    HOSTGL_POST
}

void mglPrimitiveRestartNV ()
{
    HOSTGL_PRE
    GLCALL(glPrimitiveRestartNV);
    HOSTGL_POST
}

void mglPrimitiveRestartIndexNV (GLuint index)
{
    HOSTGL_PRE
    GLCALL(glPrimitiveRestartIndexNV, index);
    HOSTGL_POST
}

void mglVertexAttribI1iEXT (GLuint index, GLint x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1iEXT, index, x);
    HOSTGL_POST
}

void mglVertexAttribI2iEXT (GLuint index, GLint x, GLint y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2iEXT, index, x, y);
    HOSTGL_POST
}

void mglVertexAttribI3iEXT (GLuint index, GLint x, GLint y, GLint z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3iEXT, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttribI4iEXT (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4iEXT, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttribI1uiEXT (GLuint index, GLuint x)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1uiEXT, index, x);
    HOSTGL_POST
}

void mglVertexAttribI2uiEXT (GLuint index, GLuint x, GLuint y)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2uiEXT, index, x, y);
    HOSTGL_POST
}

void mglVertexAttribI3uiEXT (GLuint index, GLuint x, GLuint y, GLuint z)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3uiEXT, index, x, y, z);
    HOSTGL_POST
}

void mglVertexAttribI4uiEXT (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4uiEXT, index, x, y, z, w);
    HOSTGL_POST
}

void mglVertexAttribI1ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1ivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI2ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2ivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI3ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3ivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4ivEXT (GLuint index, const GLint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4ivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI1uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI1uivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI2uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI2uivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI3uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI3uivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4uivEXT (GLuint index, const GLuint * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4uivEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4bvEXT (GLuint index, const GLbyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4bvEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4svEXT (GLuint index, const GLshort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4svEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4ubvEXT (GLuint index, const GLubyte * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4ubvEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribI4usvEXT (GLuint index, const GLushort * v)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribI4usvEXT, index, v);
    HOSTGL_POST
}

void mglVertexAttribIPointerEXT (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribIPointerEXT, index, size, type, stride, pointer);
    HOSTGL_POST
}

void mglGetVertexAttribIivEXT (GLuint index, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribIivEXT, index, pname, params);
    HOSTGL_POST
}

void mglGetVertexAttribIuivEXT (GLuint index, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetVertexAttribIuivEXT, index, pname, params);
    HOSTGL_POST
}

void mglGetUniformuivEXT (GLuint program, GLint location, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetUniformuivEXT, program, location, params);
    HOSTGL_POST
}

void mglBindFragDataLocationEXT (GLuint program, GLuint color, const GLchar * name)
{
    HOSTGL_PRE
    GLCALL(glBindFragDataLocationEXT, program, color, name);
    HOSTGL_POST
}

GLint mglGetFragDataLocationEXT (GLuint program, const GLchar * name)
{
    GLint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetFragDataLocationEXT, program, name);
    HOSTGL_POST
    return _ret;
}

void mglUniform1uiEXT (GLint location, GLuint v0)
{
    HOSTGL_PRE
    GLCALL(glUniform1uiEXT, location, v0);
    HOSTGL_POST
}

void mglUniform2uiEXT (GLint location, GLuint v0, GLuint v1)
{
    HOSTGL_PRE
    GLCALL(glUniform2uiEXT, location, v0, v1);
    HOSTGL_POST
}

void mglUniform3uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    HOSTGL_PRE
    GLCALL(glUniform3uiEXT, location, v0, v1, v2);
    HOSTGL_POST
}

void mglUniform4uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    HOSTGL_PRE
    GLCALL(glUniform4uiEXT, location, v0, v1, v2, v3);
    HOSTGL_POST
}

void mglUniform1uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform1uivEXT, location, count, value);
    HOSTGL_POST
}

void mglUniform2uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform2uivEXT, location, count, value);
    HOSTGL_POST
}

void mglUniform3uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform3uivEXT, location, count, value);
    HOSTGL_POST
}

void mglUniform4uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    HOSTGL_PRE
    GLCALL(glUniform4uivEXT, location, count, value);
    HOSTGL_POST
}

void mglTexParameterIivEXT (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameterIivEXT, target, pname, params);
    HOSTGL_POST
}

void mglTexParameterIuivEXT (GLenum target, GLenum pname, const GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glTexParameterIuivEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameterIivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterIivEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetTexParameterIuivEXT (GLenum target, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterIuivEXT, target, pname, params);
    HOSTGL_POST
}

void mglClearColorIiEXT (GLint red, GLint green, GLint blue, GLint alpha)
{
    HOSTGL_PRE
    GLCALL(glClearColorIiEXT, red, green, blue, alpha);
    HOSTGL_POST
}

void mglClearColorIuiEXT (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    HOSTGL_PRE
    GLCALL(glClearColorIuiEXT, red, green, blue, alpha);
    HOSTGL_POST
}

void mglUseShaderProgramEXT (GLenum type, GLuint program)
{
    HOSTGL_PRE
    GLCALL(glUseShaderProgramEXT, type, program);
    HOSTGL_POST
}

void mglActiveProgramEXT (GLuint program)
{
    HOSTGL_PRE
    GLCALL(glActiveProgramEXT, program);
    HOSTGL_POST
}

GLuint mglCreateShaderProgramEXT (GLenum type, const GLchar * string)
{
    GLuint _ret;
    HOSTGL_PRE
    _ret = GLCALL(glCreateShaderProgramEXT, type, string);
    HOSTGL_POST
    return _ret;
}

void mglProgramEnvParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glProgramEnvParameters4fvEXT, target, index, count, params);
    HOSTGL_POST
}

void mglProgramLocalParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glProgramLocalParameters4fvEXT, target, index, count, params);
    HOSTGL_POST
}

void mglBlendEquationSeparateATI (GLenum modeRGB, GLenum modeA)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationSeparateATI, modeRGB, modeA);
    HOSTGL_POST
}

void mglGetHistogramEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetHistogramEXT, target, reset, format, type, values);
    HOSTGL_POST
}

void mglGetHistogramParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetHistogramParameterfvEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetHistogramParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetHistogramParameterivEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetMinmaxEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetMinmaxEXT, target, reset, format, type, values);
    HOSTGL_POST
}

void mglGetMinmaxParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetMinmaxParameterfvEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetMinmaxParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetMinmaxParameterivEXT, target, pname, params);
    HOSTGL_POST
}

void mglHistogramEXT (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    GLCALL(glHistogramEXT, target, width, internalformat, sink);
    HOSTGL_POST
}

void mglMinmaxEXT (GLenum target, GLenum internalformat, GLboolean sink)
{
    HOSTGL_PRE
    GLCALL(glMinmaxEXT, target, internalformat, sink);
    HOSTGL_POST
}

void mglResetHistogramEXT (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glResetHistogramEXT, target);
    HOSTGL_POST
}

void mglResetMinmaxEXT (GLenum target)
{
    HOSTGL_PRE
    GLCALL(glResetMinmaxEXT, target);
    HOSTGL_POST
}

void mglConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glConvolutionFilter1DEXT, target, internalformat, width, format, type, image);
    HOSTGL_POST
}

void mglConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glConvolutionFilter2DEXT, target, internalformat, width, height, format, type, image);
    HOSTGL_POST
}

void mglConvolutionParameterfEXT (GLenum target, GLenum pname, GLfloat params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameterfEXT, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameterfvEXT (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameterfvEXT, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameteriEXT (GLenum target, GLenum pname, GLint params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameteriEXT, target, pname, params);
    HOSTGL_POST
}

void mglConvolutionParameterivEXT (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glConvolutionParameterivEXT, target, pname, params);
    HOSTGL_POST
}

void mglCopyConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyConvolutionFilter1DEXT, target, internalformat, x, y, width);
    HOSTGL_POST
}

void mglCopyConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    HOSTGL_PRE
    GLCALL(glCopyConvolutionFilter2DEXT, target, internalformat, x, y, width, height);
    HOSTGL_POST
}

void mglGetConvolutionFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionFilterEXT, target, format, type, image);
    HOSTGL_POST
}

void mglGetConvolutionParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionParameterfvEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetConvolutionParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetConvolutionParameterivEXT, target, pname, params);
    HOSTGL_POST
}

void mglGetSeparableFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    GLCALL(glGetSeparableFilterEXT, target, format, type, row, column, span);
    HOSTGL_POST
}

void mglSeparableFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    HOSTGL_PRE
    GLCALL(glSeparableFilter2DEXT, target, internalformat, width, height, format, type, row, column);
    HOSTGL_POST
}

void mglColorTableSGI (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glColorTableSGI, target, internalformat, width, format, type, table);
    HOSTGL_POST
}

void mglColorTableParameterfvSGI (GLenum target, GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glColorTableParameterfvSGI, target, pname, params);
    HOSTGL_POST
}

void mglColorTableParameterivSGI (GLenum target, GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glColorTableParameterivSGI, target, pname, params);
    HOSTGL_POST
}

void mglCopyColorTableSGI (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyColorTableSGI, target, internalformat, x, y, width);
    HOSTGL_POST
}

void mglGetColorTableSGI (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableSGI, target, format, type, table);
    HOSTGL_POST
}

void mglGetColorTableParameterfvSGI (GLenum target, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameterfvSGI, target, pname, params);
    HOSTGL_POST
}

void mglGetColorTableParameterivSGI (GLenum target, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetColorTableParameterivSGI, target, pname, params);
    HOSTGL_POST
}

void mglPixelTexGenSGIX (GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glPixelTexGenSGIX, mode);
    HOSTGL_POST
}

void mglPixelTexGenParameteriSGIS (GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glPixelTexGenParameteriSGIS, pname, param);
    HOSTGL_POST
}

void mglPixelTexGenParameterivSGIS (GLenum pname, const GLint * params)
{
    HOSTGL_PRE
    GLCALL(glPixelTexGenParameterivSGIS, pname, params);
    HOSTGL_POST
}

void mglPixelTexGenParameterfSGIS (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPixelTexGenParameterfSGIS, pname, param);
    HOSTGL_POST
}

void mglPixelTexGenParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glPixelTexGenParameterfvSGIS, pname, params);
    HOSTGL_POST
}

void mglGetPixelTexGenParameterivSGIS (GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetPixelTexGenParameterivSGIS, pname, params);
    HOSTGL_POST
}

void mglGetPixelTexGenParameterfvSGIS (GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetPixelTexGenParameterfvSGIS, pname, params);
    HOSTGL_POST
}

void mglSampleMaskSGIS (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    GLCALL(glSampleMaskSGIS, value, invert);
    HOSTGL_POST
}

void mglSamplePatternSGIS (GLenum pattern)
{
    HOSTGL_PRE
    GLCALL(glSamplePatternSGIS, pattern);
    HOSTGL_POST
}

void mglPointParameterfSGIS (GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfSGIS, pname, param);
    HOSTGL_POST
}

void mglPointParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glPointParameterfvSGIS, pname, params);
    HOSTGL_POST
}

void mglColorSubTableEXT (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glColorSubTableEXT, target, start, count, format, type, data);
    HOSTGL_POST
}

void mglCopyColorSubTableEXT (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    HOSTGL_PRE
    GLCALL(glCopyColorSubTableEXT, target, start, x, y, width);
    HOSTGL_POST
}

void mglBlendFuncSeparateINGR (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncSeparateINGR, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    HOSTGL_POST
}

void mglMultiModeDrawArraysIBM (const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride)
{
    HOSTGL_PRE
    GLCALL(glMultiModeDrawArraysIBM, mode, first, count, primcount, modestride);
    HOSTGL_POST
}

void mglMultiModeDrawElementsIBM (const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride)
{
    HOSTGL_PRE
    GLCALL(glMultiModeDrawElementsIBM, mode, count, type, indices, primcount, modestride);
    HOSTGL_POST
}

void mglSampleMaskEXT (GLclampf value, GLboolean invert)
{
    HOSTGL_PRE
    GLCALL(glSampleMaskEXT, value, invert);
    HOSTGL_POST
}

void mglSamplePatternEXT (GLenum pattern)
{
    HOSTGL_PRE
    GLCALL(glSamplePatternEXT, pattern);
    HOSTGL_POST
}

void mglDeleteFencesNV (GLsizei n, const GLuint * fences)
{
    HOSTGL_PRE
    GLCALL(glDeleteFencesNV, n, fences);
    HOSTGL_POST
}

void mglGenFencesNV (GLsizei n, GLuint * fences)
{
    HOSTGL_PRE
    GLCALL(glGenFencesNV, n, fences);
    HOSTGL_POST
}

GLboolean mglIsFenceNV (GLuint fence)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsFenceNV, fence);
    HOSTGL_POST
    return _ret;
}

GLboolean mglTestFenceNV (GLuint fence)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glTestFenceNV, fence);
    HOSTGL_POST
    return _ret;
}

void mglGetFenceivNV (GLuint fence, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetFenceivNV, fence, pname, params);
    HOSTGL_POST
}

void mglFinishFenceNV (GLuint fence)
{
    HOSTGL_PRE
    GLCALL(glFinishFenceNV, fence);
    HOSTGL_POST
}

void mglSetFenceNV (GLuint fence, GLenum condition)
{
    HOSTGL_PRE
    GLCALL(glSetFenceNV, fence, condition);
    HOSTGL_POST
}

void mglActiveStencilFaceEXT (GLenum face)
{
    HOSTGL_PRE
    GLCALL(glActiveStencilFaceEXT, face);
    HOSTGL_POST
}

void mglBindVertexArrayAPPLE (GLuint array)
{
    HOSTGL_PRE
    GLCALL(glBindVertexArrayAPPLE, array);
    HOSTGL_POST
}

void mglDeleteVertexArraysAPPLE (GLsizei n, const GLuint * arrays)
{
    HOSTGL_PRE
    GLCALL(glDeleteVertexArraysAPPLE, n, arrays);
    HOSTGL_POST
}

void mglGenVertexArraysAPPLE (GLsizei n, GLuint * arrays)
{
    HOSTGL_PRE
    GLCALL(glGenVertexArraysAPPLE, n, arrays);
    HOSTGL_POST
}

GLboolean mglIsVertexArrayAPPLE (GLuint array)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsVertexArrayAPPLE, array);
    HOSTGL_POST
    return _ret;
}

void mglStencilOpSeparateATI (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    HOSTGL_PRE
    GLCALL(glStencilOpSeparateATI, face, sfail, dpfail, dppass);
    HOSTGL_POST
}

void mglStencilFuncSeparateATI (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
    HOSTGL_PRE
    GLCALL(glStencilFuncSeparateATI, frontfunc, backfunc, ref, mask);
    HOSTGL_POST
}

void mglDepthBoundsEXT (GLclampd zmin, GLclampd zmax)
{
    HOSTGL_PRE
    GLCALL(glDepthBoundsEXT, zmin, zmax);
    HOSTGL_POST
}

void mglBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationSeparateEXT, modeRGB, modeAlpha);
    HOSTGL_POST
}

void mglBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    HOSTGL_PRE
    GLCALL(glBlitFramebufferEXT, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
    HOSTGL_POST
}

void mglGetQueryObjecti64vEXT (GLuint id, GLenum pname, GLint64EXT * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjecti64vEXT, id, pname, params);
    HOSTGL_POST
}

void mglGetQueryObjectui64vEXT (GLuint id, GLenum pname, GLuint64EXT * params)
{
    HOSTGL_PRE
    GLCALL(glGetQueryObjectui64vEXT, id, pname, params);
    HOSTGL_POST
}

void mglBufferParameteriAPPLE (GLenum target, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glBufferParameteriAPPLE, target, pname, param);
    HOSTGL_POST
}

void mglFlushMappedBufferRangeAPPLE (GLenum target, GLintptr offset, GLsizeiptr size)
{
    HOSTGL_PRE
    GLCALL(glFlushMappedBufferRangeAPPLE, target, offset, size);
    HOSTGL_POST
}

void mglTextureRangeAPPLE (GLenum target, GLsizei length, const GLvoid * pointer)
{
    HOSTGL_PRE
    GLCALL(glTextureRangeAPPLE, target, length, pointer);
    HOSTGL_POST
}

void mglGetTexParameterPointervAPPLE (GLenum target, GLenum pname, GLvoid *  * params)
{
    HOSTGL_PRE
    GLCALL(glGetTexParameterPointervAPPLE, target, pname, params);
    HOSTGL_POST
}

void mglClampColorARB (GLenum target, GLenum clamp)
{
    HOSTGL_PRE
    GLCALL(glClampColorARB, target, clamp);
    HOSTGL_POST
}

void mglFramebufferTextureLayerARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    HOSTGL_PRE
    GLCALL(glFramebufferTextureLayerARB, target, attachment, texture, level, layer);
    HOSTGL_POST
}

void mglVertexAttribDivisorARB (GLuint index, GLuint divisor)
{
    HOSTGL_PRE
    GLCALL(glVertexAttribDivisorARB, index, divisor);
    HOSTGL_POST
}

void mglTexBufferARB (GLenum target, GLenum internalformat, GLuint buffer)
{
    HOSTGL_PRE
    GLCALL(glTexBufferARB, target, internalformat, buffer);
    HOSTGL_POST
}

void mglDrawElementsInstancedBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount, GLint basevertex)
{
    HOSTGL_PRE
    GLCALL(glDrawElementsInstancedBaseVertex, mode, count, type, indices, primcount, basevertex);
    HOSTGL_POST
}

void mglBlendEquationiARB (GLuint buf, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationiARB, buf, mode);
    HOSTGL_POST
}

void mglBlendEquationSeparateiARB (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationSeparateiARB, buf, modeRGB, modeAlpha);
    HOSTGL_POST
}

void mglBlendFunciARB (GLuint buf, GLenum src, GLenum dst)
{
    HOSTGL_PRE
    GLCALL(glBlendFunciARB, buf, src, dst);
    HOSTGL_POST
}

void mglBlendFuncSeparateiARB (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncSeparateiARB, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    HOSTGL_POST
}

void mglGenSamplers (GLsizei count, GLuint * samplers)
{
    HOSTGL_PRE
    GLCALL(glGenSamplers, count, samplers);
    HOSTGL_POST
}

void mglDeleteSamplers (GLsizei count, const GLuint * samplers)
{
    HOSTGL_PRE
    GLCALL(glDeleteSamplers, count, samplers);
    HOSTGL_POST
}

GLboolean mglIsSampler (GLuint sampler)
{
    GLboolean _ret;
    HOSTGL_PRE
    _ret = GLCALL(glIsSampler, sampler);
    HOSTGL_POST
    return _ret;
}

void mglBindSampler (GLuint unit, GLuint sampler)
{
    HOSTGL_PRE
    GLCALL(glBindSampler, unit, sampler);
    HOSTGL_POST
}

void mglSamplerParameteri (GLuint sampler, GLenum pname, GLint param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameteri, sampler, pname, param);
    HOSTGL_POST
}

void mglSamplerParameteriv (GLuint sampler, GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameteriv, sampler, pname, param);
    HOSTGL_POST
}

void mglSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameterf, sampler, pname, param);
    HOSTGL_POST
}

void mglSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat * param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameterfv, sampler, pname, param);
    HOSTGL_POST
}

void mglSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint * param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameterIiv, sampler, pname, param);
    HOSTGL_POST
}

void mglSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint * param)
{
    HOSTGL_PRE
    GLCALL(glSamplerParameterIuiv, sampler, pname, param);
    HOSTGL_POST
}

void mglGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetSamplerParameteriv, sampler, pname, params);
    HOSTGL_POST
}

void mglGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetSamplerParameterIiv, sampler, pname, params);
    HOSTGL_POST
}

void mglGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetSamplerParameterfv, sampler, pname, params);
    HOSTGL_POST
}

void mglGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetSamplerParameterIuiv, sampler, pname, params);
    HOSTGL_POST
}

void mglReleaseShaderCompiler ()
{
    HOSTGL_PRE
    GLCALL(glReleaseShaderCompiler);
    HOSTGL_POST
}

void mglShaderBinary (GLsizei count, const GLuint * shaders, GLenum binaryformat, const GLvoid * binary, GLsizei length)
{
    HOSTGL_PRE
    GLCALL(glShaderBinary, count, shaders, binaryformat, binary, length);
    HOSTGL_POST
}

void mglGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision)
{
    HOSTGL_PRE
    GLCALL(glGetShaderPrecisionFormat, shadertype, precisiontype, range, precision);
    HOSTGL_POST
}

void mglDepthRangef (GLclampf n, GLclampf f)
{
    HOSTGL_PRE
    GLCALL(glDepthRangef, n, f);
    HOSTGL_POST
}

void mglClearDepthf (GLclampf d)
{
    HOSTGL_PRE
    GLCALL(glClearDepthf, d);
    HOSTGL_POST
}

GLenum mglGetGraphicsResetStatusARB ()
{
    GLenum _ret;
    HOSTGL_PRE
    _ret = GLCALL(glGetGraphicsResetStatusARB);
    HOSTGL_POST
    return _ret;
}

void mglGetnMapdvARB (GLenum target, GLenum query, GLsizei bufSize, GLdouble * v)
{
    HOSTGL_PRE
    GLCALL(glGetnMapdvARB, target, query, bufSize, v);
    HOSTGL_POST
}

void mglGetnMapfvARB (GLenum target, GLenum query, GLsizei bufSize, GLfloat * v)
{
    HOSTGL_PRE
    GLCALL(glGetnMapfvARB, target, query, bufSize, v);
    HOSTGL_POST
}

void mglGetnMapivARB (GLenum target, GLenum query, GLsizei bufSize, GLint * v)
{
    HOSTGL_PRE
    GLCALL(glGetnMapivARB, target, query, bufSize, v);
    HOSTGL_POST
}

void mglGetnPixelMapfvARB (GLenum map, GLsizei bufSize, GLfloat * values)
{
    HOSTGL_PRE
    GLCALL(glGetnPixelMapfvARB, map, bufSize, values);
    HOSTGL_POST
}

void mglGetnPixelMapuivARB (GLenum map, GLsizei bufSize, GLuint * values)
{
    HOSTGL_PRE
    GLCALL(glGetnPixelMapuivARB, map, bufSize, values);
    HOSTGL_POST
}

void mglGetnPixelMapusvARB (GLenum map, GLsizei bufSize, GLushort * values)
{
    HOSTGL_PRE
    GLCALL(glGetnPixelMapusvARB, map, bufSize, values);
    HOSTGL_POST
}

void mglGetnPolygonStippleARB (GLsizei bufSize, GLubyte * pattern)
{
    HOSTGL_PRE
    GLCALL(glGetnPolygonStippleARB, bufSize, pattern);
    HOSTGL_POST
}

void mglGetnColorTableARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * table)
{
    HOSTGL_PRE
    GLCALL(glGetnColorTableARB, target, format, type, bufSize, table);
    HOSTGL_POST
}

void mglGetnConvolutionFilterARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * image)
{
    HOSTGL_PRE
    GLCALL(glGetnConvolutionFilterARB, target, format, type, bufSize, image);
    HOSTGL_POST
}

void mglGetnSeparableFilterARB (GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, GLvoid * row, GLsizei columnBufSize, GLvoid * column, GLvoid * span)
{
    HOSTGL_PRE
    GLCALL(glGetnSeparableFilterARB, target, format, type, rowBufSize, row, columnBufSize, column, span);
    HOSTGL_POST
}

void mglGetnHistogramARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetnHistogramARB, target, reset, format, type, bufSize, values);
    HOSTGL_POST
}

void mglGetnMinmaxARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    HOSTGL_PRE
    GLCALL(glGetnMinmaxARB, target, reset, format, type, bufSize, values);
    HOSTGL_POST
}

void mglGetnTexImageARB (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid * img)
{
    HOSTGL_PRE
    GLCALL(glGetnTexImageARB, target, level, format, type, bufSize, img);
    HOSTGL_POST
}

void mglReadnPixelsARB (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLvoid * data)
{
    HOSTGL_PRE
    GLCALL(glReadnPixelsARB, x, y, width, height, format, type, bufSize, data);
    HOSTGL_POST
}

void mglGetnCompressedTexImageARB (GLenum target, GLint lod, GLsizei bufSize, GLvoid * img)
{
    HOSTGL_PRE
    GLCALL(glGetnCompressedTexImageARB, target, lod, bufSize, img);
    HOSTGL_POST
}

void mglGetnUniformfvARB (GLuint program, GLint location, GLsizei bufSize, GLfloat * params)
{
    HOSTGL_PRE
    GLCALL(glGetnUniformfvARB, program, location, bufSize, params);
    HOSTGL_POST
}

void mglGetnUniformivARB (GLuint program, GLint location, GLsizei bufSize, GLint * params)
{
    HOSTGL_PRE
    GLCALL(glGetnUniformivARB, program, location, bufSize, params);
    HOSTGL_POST
}

void mglGetnUniformuivARB (GLuint program, GLint location, GLsizei bufSize, GLuint * params)
{
    HOSTGL_PRE
    GLCALL(glGetnUniformuivARB, program, location, bufSize, params);
    HOSTGL_POST
}

void mglGetnUniformdvARB (GLuint program, GLint location, GLsizei bufSize, GLdouble * params)
{
    HOSTGL_PRE
    GLCALL(glGetnUniformdvARB, program, location, bufSize, params);
    HOSTGL_POST
}

void mglBlendFuncIndexedAMD (GLuint buf, GLenum src, GLenum dst)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncIndexedAMD, buf, src, dst);
    HOSTGL_POST
}

void mglBlendFuncSeparateIndexedAMD (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendFuncSeparateIndexedAMD, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
    HOSTGL_POST
}

void mglBlendEquationIndexedAMD (GLuint buf, GLenum mode)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationIndexedAMD, buf, mode);
    HOSTGL_POST
}

void mglBlendEquationSeparateIndexedAMD (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    HOSTGL_PRE
    GLCALL(glBlendEquationSeparateIndexedAMD, buf, modeRGB, modeAlpha);
    HOSTGL_POST
}

void mglTextureBarrierNV ()
{
    HOSTGL_PRE
    GLCALL(glTextureBarrierNV);
    HOSTGL_POST
}

