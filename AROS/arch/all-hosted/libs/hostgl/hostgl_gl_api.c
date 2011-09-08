/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "glx_hostlib.h"
#include "arosmesaapim.h"

void mglClearIndex (GLfloat c)
{
    GLCALL(glClearIndex, c);
}

void mglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glClearColor, red, green, blue, alpha);
}

void mglClear (GLbitfield mask)
{
    GLCALL(glClear, mask);
}

void mglIndexMask (GLuint mask)
{
    GLCALL(glIndexMask, mask);
}

void mglColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    GLCALL(glColorMask, red, green, blue, alpha);
}

void mglAlphaFunc (GLenum func, GLclampf ref)
{
    GLCALL(glAlphaFunc, func, ref);
}

void mglBlendFunc (GLenum sfactor, GLenum dfactor)
{
    GLCALL(glBlendFunc, sfactor, dfactor);
}

void mglLogicOp (GLenum opcode)
{
    GLCALL(glLogicOp, opcode);
}

void mglCullFace (GLenum mode)
{
    GLCALL(glCullFace, mode);
}

void mglFrontFace (GLenum mode)
{
    GLCALL(glFrontFace, mode);
}

void mglPointSize (GLfloat size)
{
    GLCALL(glPointSize, size);
}

void mglLineWidth (GLfloat width)
{
    GLCALL(glLineWidth, width);
}

void mglLineStipple (GLint factor, GLushort pattern)
{
    GLCALL(glLineStipple, factor, pattern);
}

void mglPolygonMode (GLenum face, GLenum mode)
{
    GLCALL(glPolygonMode, face, mode);
}

void mglPolygonOffset (GLfloat factor, GLfloat units)
{
    GLCALL(glPolygonOffset, factor, units);
}

void mglPolygonStipple (const GLubyte * mask)
{
    GLCALL(glPolygonStipple, mask);
}

void mglGetPolygonStipple (GLubyte * mask)
{
    GLCALL(glGetPolygonStipple, mask);
}

void mglEdgeFlag (GLboolean flag)
{
    GLCALL(glEdgeFlag, flag);
}

void mglEdgeFlagv (const GLboolean * flag)
{
    GLCALL(glEdgeFlagv, flag);
}

void mglScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glScissor, x, y, width, height);
}

void mglClipPlane (GLenum plane, const GLdouble * equation)
{
    GLCALL(glClipPlane, plane, equation);
}

void mglGetClipPlane (GLenum plane, GLdouble * equation)
{
    GLCALL(glGetClipPlane, plane, equation);
}

void mglDrawBuffer (GLenum mode)
{
    GLCALL(glDrawBuffer, mode);
}

void mglReadBuffer (GLenum mode)
{
    GLCALL(glReadBuffer, mode);
}

void mglEnable (GLenum cap)
{
    GLCALL(glEnable, cap);
}

void mglDisable (GLenum cap)
{
    GLCALL(glDisable, cap);
}

GLboolean mglIsEnabled (GLenum cap)
{
    return GLCALL(glIsEnabled, cap);
}

void mglEnableClientState (GLenum cap)
{
    GLCALL(glEnableClientState, cap);
}

void mglDisableClientState (GLenum cap)
{
    GLCALL(glDisableClientState, cap);
}

void mglGetBooleanv (GLenum pname, GLboolean * params)
{
    GLCALL(glGetBooleanv, pname, params);
}

void mglGetDoublev (GLenum pname, GLdouble * params)
{
    GLCALL(glGetDoublev, pname, params);
}

void mglGetFloatv (GLenum pname, GLfloat * params)
{
    GLCALL(glGetFloatv, pname, params);
}

void mglGetIntegerv (GLenum pname, GLint * params)
{
    GLCALL(glGetIntegerv, pname, params);
}

void mglPushAttrib (GLbitfield mask)
{
    GLCALL(glPushAttrib, mask);
}

void mglPopAttrib ()
{
    GLCALL(glPopAttrib);
}

void mglPushClientAttrib (GLbitfield mask)
{
    GLCALL(glPushClientAttrib, mask);
}

void mglPopClientAttrib ()
{
    GLCALL(glPopClientAttrib);
}

GLint mglRenderMode (GLenum mode)
{
    return GLCALL(glRenderMode, mode);
}

GLenum mglGetError ()
{
    return GLCALL(glGetError);
}

const GLubyte * mglGetString (GLenum name)
{
    return GLCALL(glGetString, name);
}

void mglFinish ()
{
    GLCALL(glFinish);
}

void mglFlush ()
{
    GLCALL(glFlush);
}

void mglHint (GLenum target, GLenum mode)
{
    GLCALL(glHint, target, mode);
}

void mglClearDepth (GLclampd depth)
{
    GLCALL(glClearDepth, depth);
}

void mglDepthFunc (GLenum func)
{
    GLCALL(glDepthFunc, func);
}

void mglDepthMask (GLboolean flag)
{
    GLCALL(glDepthMask, flag);
}

void mglDepthRange (GLclampd near_val, GLclampd far_val)
{
    GLCALL(glDepthRange, near_val, far_val);
}

void mglClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCALL(glClearAccum, red, green, blue, alpha);
}

void mglAccum (GLenum op, GLfloat value)
{
    GLCALL(glAccum, op, value);
}

void mglMatrixMode (GLenum mode)
{
    GLCALL(glMatrixMode, mode);
}

void mglOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    GLCALL(glOrtho, left, right, bottom, top, near_val, far_val);
}

void mglFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    GLCALL(glFrustum, left, right, bottom, top, near_val, far_val);
}

void mglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glViewport, x, y, width, height);
}

void mglPushMatrix ()
{
    GLCALL(glPushMatrix);
}

void mglPopMatrix ()
{
    GLCALL(glPopMatrix);
}

void mglLoadIdentity ()
{
    GLCALL(glLoadIdentity);
}

void mglLoadMatrixd (const GLdouble * m)
{
    GLCALL(glLoadMatrixd, m);
}

void mglLoadMatrixf (const GLfloat * m)
{
    GLCALL(glLoadMatrixf, m);
}

void mglMultMatrixd (const GLdouble * m)
{
    GLCALL(glMultMatrixd, m);
}

void mglMultMatrixf (const GLfloat * m)
{
    GLCALL(glMultMatrixf, m);
}

void mglRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glRotated, angle, x, y, z);
}

void mglRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glRotatef, angle, x, y, z);
}

void mglScaled (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glScaled, x, y, z);
}

void mglScalef (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glScalef, x, y, z);
}

void mglTranslated (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glTranslated, x, y, z);
}

void mglTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glTranslatef, x, y, z);
}

GLboolean mglIsList (GLuint list)
{
    return GLCALL(glIsList, list);
}

void mglDeleteLists (GLuint list, GLsizei range)
{
    GLCALL(glDeleteLists, list, range);
}

GLuint mglGenLists (GLsizei range)
{
    return GLCALL(glGenLists, range);
}

void mglNewList (GLuint list, GLenum mode)
{
    GLCALL(glNewList, list, mode);
}

void mglEndList ()
{
    GLCALL(glEndList);
}

void mglCallList (GLuint list)
{
    GLCALL(glCallList, list);
}

void mglCallLists (GLsizei n, GLenum type, const GLvoid * lists)
{
    GLCALL(glCallLists, n, type, lists);
}

void mglListBase (GLuint base)
{
    GLCALL(glListBase, base);
}

void mglBegin (GLenum mode)
{
    GLCALL(glBegin, mode);
}

void mglEnd ()
{
    GLCALL(glEnd);
}

void mglVertex2d (GLdouble x, GLdouble y)
{
    GLCALL(glVertex2d, x, y);
}

void mglVertex2f (GLfloat x, GLfloat y)
{
    GLCALL(glVertex2f, x, y);
}

void mglVertex2i (GLint x, GLint y)
{
    GLCALL(glVertex2i, x, y);
}

void mglVertex2s (GLshort x, GLshort y)
{
    GLCALL(glVertex2s, x, y);
}

void mglVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertex3d, x, y, z);
}

void mglVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertex3f, x, y, z);
}

void mglVertex3i (GLint x, GLint y, GLint z)
{
    GLCALL(glVertex3i, x, y, z);
}

void mglVertex3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertex3s, x, y, z);
}

void mglVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertex4d, x, y, z, w);
}

void mglVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertex4f, x, y, z, w);
}

void mglVertex4i (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertex4i, x, y, z, w);
}

void mglVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertex4s, x, y, z, w);
}

void mglVertex2dv (const GLdouble * v)
{
    GLCALL(glVertex2dv, v);
}

void mglVertex2fv (const GLfloat * v)
{
    GLCALL(glVertex2fv, v);
}

void mglVertex2iv (const GLint * v)
{
    GLCALL(glVertex2iv, v);
}

void mglVertex2sv (const GLshort * v)
{
    GLCALL(glVertex2sv, v);
}

void mglVertex3dv (const GLdouble * v)
{
    GLCALL(glVertex3dv, v);
}

void mglVertex3fv (const GLfloat * v)
{
    GLCALL(glVertex3fv, v);
}

void mglVertex3iv (const GLint * v)
{
    GLCALL(glVertex3iv, v);
}

void mglVertex3sv (const GLshort * v)
{
    GLCALL(glVertex3sv, v);
}

void mglVertex4dv (const GLdouble * v)
{
    GLCALL(glVertex4dv, v);
}

void mglVertex4fv (const GLfloat * v)
{
    GLCALL(glVertex4fv, v);
}

void mglVertex4iv (const GLint * v)
{
    GLCALL(glVertex4iv, v);
}

void mglVertex4sv (const GLshort * v)
{
    GLCALL(glVertex4sv, v);
}

void mglNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
    GLCALL(glNormal3b, nx, ny, nz);
}

void mglNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
    GLCALL(glNormal3d, nx, ny, nz);
}

void mglNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    GLCALL(glNormal3f, nx, ny, nz);
}

void mglNormal3i (GLint nx, GLint ny, GLint nz)
{
    GLCALL(glNormal3i, nx, ny, nz);
}

void mglNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
    GLCALL(glNormal3s, nx, ny, nz);
}

void mglNormal3bv (const GLbyte * v)
{
    GLCALL(glNormal3bv, v);
}

void mglNormal3dv (const GLdouble * v)
{
    GLCALL(glNormal3dv, v);
}

void mglNormal3fv (const GLfloat * v)
{
    GLCALL(glNormal3fv, v);
}

void mglNormal3iv (const GLint * v)
{
    GLCALL(glNormal3iv, v);
}

void mglNormal3sv (const GLshort * v)
{
    GLCALL(glNormal3sv, v);
}

void mglIndexd (GLdouble c)
{
    GLCALL(glIndexd, c);
}

void mglIndexf (GLfloat c)
{
    GLCALL(glIndexf, c);
}

void mglIndexi (GLint c)
{
    GLCALL(glIndexi, c);
}

void mglIndexs (GLshort c)
{
    GLCALL(glIndexs, c);
}

void mglIndexub (GLubyte c)
{
    GLCALL(glIndexub, c);
}

void mglIndexdv (const GLdouble * c)
{
    GLCALL(glIndexdv, c);
}

void mglIndexfv (const GLfloat * c)
{
    GLCALL(glIndexfv, c);
}

void mglIndexiv (const GLint * c)
{
    GLCALL(glIndexiv, c);
}

void mglIndexsv (const GLshort * c)
{
    GLCALL(glIndexsv, c);
}

void mglIndexubv (const GLubyte * c)
{
    GLCALL(glIndexubv, c);
}

void mglColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glColor3b, red, green, blue);
}

void mglColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glColor3d, red, green, blue);
}

void mglColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glColor3f, red, green, blue);
}

void mglColor3i (GLint red, GLint green, GLint blue)
{
    GLCALL(glColor3i, red, green, blue);
}

void mglColor3s (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glColor3s, red, green, blue);
}

void mglColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glColor3ub, red, green, blue);
}

void mglColor3ui (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glColor3ui, red, green, blue);
}

void mglColor3us (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glColor3us, red, green, blue);
}

void mglColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    GLCALL(glColor4b, red, green, blue, alpha);
}

void mglColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    GLCALL(glColor4d, red, green, blue, alpha);
}

void mglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCALL(glColor4f, red, green, blue, alpha);
}

void mglColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
    GLCALL(glColor4i, red, green, blue, alpha);
}

void mglColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    GLCALL(glColor4s, red, green, blue, alpha);
}

void mglColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    GLCALL(glColor4ub, red, green, blue, alpha);
}

void mglColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    GLCALL(glColor4ui, red, green, blue, alpha);
}

void mglColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    GLCALL(glColor4us, red, green, blue, alpha);
}

void mglColor3bv (const GLbyte * v)
{
    GLCALL(glColor3bv, v);
}

void mglColor3dv (const GLdouble * v)
{
    GLCALL(glColor3dv, v);
}

void mglColor3fv (const GLfloat * v)
{
    GLCALL(glColor3fv, v);
}

void mglColor3iv (const GLint * v)
{
    GLCALL(glColor3iv, v);
}

void mglColor3sv (const GLshort * v)
{
    GLCALL(glColor3sv, v);
}

void mglColor3ubv (const GLubyte * v)
{
    GLCALL(glColor3ubv, v);
}

void mglColor3uiv (const GLuint * v)
{
    GLCALL(glColor3uiv, v);
}

void mglColor3usv (const GLushort * v)
{
    GLCALL(glColor3usv, v);
}

void mglColor4bv (const GLbyte * v)
{
    GLCALL(glColor4bv, v);
}

void mglColor4dv (const GLdouble * v)
{
    GLCALL(glColor4dv, v);
}

void mglColor4fv (const GLfloat * v)
{
    GLCALL(glColor4fv, v);
}

void mglColor4iv (const GLint * v)
{
    GLCALL(glColor4iv, v);
}

void mglColor4sv (const GLshort * v)
{
    GLCALL(glColor4sv, v);
}

void mglColor4ubv (const GLubyte * v)
{
    GLCALL(glColor4ubv, v);
}

void mglColor4uiv (const GLuint * v)
{
    GLCALL(glColor4uiv, v);
}

void mglColor4usv (const GLushort * v)
{
    GLCALL(glColor4usv, v);
}

void mglTexCoord1d (GLdouble s)
{
    GLCALL(glTexCoord1d, s);
}

void mglTexCoord1f (GLfloat s)
{
    GLCALL(glTexCoord1f, s);
}

void mglTexCoord1i (GLint s)
{
    GLCALL(glTexCoord1i, s);
}

void mglTexCoord1s (GLshort s)
{
    GLCALL(glTexCoord1s, s);
}

void mglTexCoord2d (GLdouble s, GLdouble t)
{
    GLCALL(glTexCoord2d, s, t);
}

void mglTexCoord2f (GLfloat s, GLfloat t)
{
    GLCALL(glTexCoord2f, s, t);
}

void mglTexCoord2i (GLint s, GLint t)
{
    GLCALL(glTexCoord2i, s, t);
}

void mglTexCoord2s (GLshort s, GLshort t)
{
    GLCALL(glTexCoord2s, s, t);
}

void mglTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glTexCoord3d, s, t, r);
}

void mglTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glTexCoord3f, s, t, r);
}

void mglTexCoord3i (GLint s, GLint t, GLint r)
{
    GLCALL(glTexCoord3i, s, t, r);
}

void mglTexCoord3s (GLshort s, GLshort t, GLshort r)
{
    GLCALL(glTexCoord3s, s, t, r);
}

void mglTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glTexCoord4d, s, t, r, q);
}

void mglTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glTexCoord4f, s, t, r, q);
}

void mglTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glTexCoord4i, s, t, r, q);
}

void mglTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glTexCoord4s, s, t, r, q);
}

void mglTexCoord1dv (const GLdouble * v)
{
    GLCALL(glTexCoord1dv, v);
}

void mglTexCoord1fv (const GLfloat * v)
{
    GLCALL(glTexCoord1fv, v);
}

void mglTexCoord1iv (const GLint * v)
{
    GLCALL(glTexCoord1iv, v);
}

void mglTexCoord1sv (const GLshort * v)
{
    GLCALL(glTexCoord1sv, v);
}

void mglTexCoord2dv (const GLdouble * v)
{
    GLCALL(glTexCoord2dv, v);
}

void mglTexCoord2fv (const GLfloat * v)
{
    GLCALL(glTexCoord2fv, v);
}

void mglTexCoord2iv (const GLint * v)
{
    GLCALL(glTexCoord2iv, v);
}

void mglTexCoord2sv (const GLshort * v)
{
    GLCALL(glTexCoord2sv, v);
}

void mglTexCoord3dv (const GLdouble * v)
{
    GLCALL(glTexCoord3dv, v);
}

void mglTexCoord3fv (const GLfloat * v)
{
    GLCALL(glTexCoord3fv, v);
}

void mglTexCoord3iv (const GLint * v)
{
    GLCALL(glTexCoord3iv, v);
}

void mglTexCoord3sv (const GLshort * v)
{
    GLCALL(glTexCoord3sv, v);
}

void mglTexCoord4dv (const GLdouble * v)
{
    GLCALL(glTexCoord4dv, v);
}

void mglTexCoord4fv (const GLfloat * v)
{
    GLCALL(glTexCoord4fv, v);
}

void mglTexCoord4iv (const GLint * v)
{
    GLCALL(glTexCoord4iv, v);
}

void mglTexCoord4sv (const GLshort * v)
{
    GLCALL(glTexCoord4sv, v);
}

void mglRasterPos2d (GLdouble x, GLdouble y)
{
    GLCALL(glRasterPos2d, x, y);
}

void mglRasterPos2f (GLfloat x, GLfloat y)
{
    GLCALL(glRasterPos2f, x, y);
}

void mglRasterPos2i (GLint x, GLint y)
{
    GLCALL(glRasterPos2i, x, y);
}

void mglRasterPos2s (GLshort x, GLshort y)
{
    GLCALL(glRasterPos2s, x, y);
}

void mglRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glRasterPos3d, x, y, z);
}

void mglRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glRasterPos3f, x, y, z);
}

void mglRasterPos3i (GLint x, GLint y, GLint z)
{
    GLCALL(glRasterPos3i, x, y, z);
}

void mglRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glRasterPos3s, x, y, z);
}

void mglRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glRasterPos4d, x, y, z, w);
}

void mglRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glRasterPos4f, x, y, z, w);
}

void mglRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glRasterPos4i, x, y, z, w);
}

void mglRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glRasterPos4s, x, y, z, w);
}

void mglRasterPos2dv (const GLdouble * v)
{
    GLCALL(glRasterPos2dv, v);
}

void mglRasterPos2fv (const GLfloat * v)
{
    GLCALL(glRasterPos2fv, v);
}

void mglRasterPos2iv (const GLint * v)
{
    GLCALL(glRasterPos2iv, v);
}

void mglRasterPos2sv (const GLshort * v)
{
    GLCALL(glRasterPos2sv, v);
}

void mglRasterPos3dv (const GLdouble * v)
{
    GLCALL(glRasterPos3dv, v);
}

void mglRasterPos3fv (const GLfloat * v)
{
    GLCALL(glRasterPos3fv, v);
}

void mglRasterPos3iv (const GLint * v)
{
    GLCALL(glRasterPos3iv, v);
}

void mglRasterPos3sv (const GLshort * v)
{
    GLCALL(glRasterPos3sv, v);
}

void mglRasterPos4dv (const GLdouble * v)
{
    GLCALL(glRasterPos4dv, v);
}

void mglRasterPos4fv (const GLfloat * v)
{
    GLCALL(glRasterPos4fv, v);
}

void mglRasterPos4iv (const GLint * v)
{
    GLCALL(glRasterPos4iv, v);
}

void mglRasterPos4sv (const GLshort * v)
{
    GLCALL(glRasterPos4sv, v);
}

void mglRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    GLCALL(glRectd, x1, y1, x2, y2);
}

void mglRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    GLCALL(glRectf, x1, y1, x2, y2);
}

void mglRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    GLCALL(glRecti, x1, y1, x2, y2);
}

void mglRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    GLCALL(glRects, x1, y1, x2, y2);
}

void mglRectdv (const GLdouble * v1, const GLdouble * v2)
{
    GLCALL(glRectdv, v1, v2);
}

void mglRectfv (const GLfloat * v1, const GLfloat * v2)
{
    GLCALL(glRectfv, v1, v2);
}

void mglRectiv (const GLint * v1, const GLint * v2)
{
    GLCALL(glRectiv, v1, v2);
}

void mglRectsv (const GLshort * v1, const GLshort * v2)
{
    GLCALL(glRectsv, v1, v2);
}

void mglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glVertexPointer, size, type, stride, ptr);
}

void mglNormalPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glNormalPointer, type, stride, ptr);
}

void mglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glColorPointer, size, type, stride, ptr);
}

void mglIndexPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glIndexPointer, type, stride, ptr);
}

void mglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glTexCoordPointer, size, type, stride, ptr);
}

void mglEdgeFlagPointer (GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glEdgeFlagPointer, stride, ptr);
}

void mglGetPointerv (GLenum pname, GLvoid * * params)
{
    GLCALL(glGetPointerv, pname, params);
}

void mglArrayElement (GLint i)
{
    GLCALL(glArrayElement, i);
}

void mglDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    GLCALL(glDrawArrays, mode, first, count);
}

void mglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawElements, mode, count, type, indices);
}

void mglInterleavedArrays (GLenum format, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glInterleavedArrays, format, stride, pointer);
}

void mglShadeModel (GLenum mode)
{
    GLCALL(glShadeModel, mode);
}

void mglLightf (GLenum light, GLenum pname, GLfloat param)
{
    GLCALL(glLightf, light, pname, param);
}

void mglLighti (GLenum light, GLenum pname, GLint param)
{
    GLCALL(glLighti, light, pname, param);
}

void mglLightfv (GLenum light, GLenum pname, const GLfloat * params)
{
    GLCALL(glLightfv, light, pname, params);
}

void mglLightiv (GLenum light, GLenum pname, const GLint * params)
{
    GLCALL(glLightiv, light, pname, params);
}

void mglGetLightfv (GLenum light, GLenum pname, GLfloat * params)
{
    GLCALL(glGetLightfv, light, pname, params);
}

void mglGetLightiv (GLenum light, GLenum pname, GLint * params)
{
    GLCALL(glGetLightiv, light, pname, params);
}

void mglLightModelf (GLenum pname, GLfloat param)
{
    GLCALL(glLightModelf, pname, param);
}

void mglLightModeli (GLenum pname, GLint param)
{
    GLCALL(glLightModeli, pname, param);
}

void mglLightModelfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glLightModelfv, pname, params);
}

void mglLightModeliv (GLenum pname, const GLint * params)
{
    GLCALL(glLightModeliv, pname, params);
}

void mglMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    GLCALL(glMaterialf, face, pname, param);
}

void mglMateriali (GLenum face, GLenum pname, GLint param)
{
    GLCALL(glMateriali, face, pname, param);
}

void mglMaterialfv (GLenum face, GLenum pname, const GLfloat * params)
{
    GLCALL(glMaterialfv, face, pname, params);
}

void mglMaterialiv (GLenum face, GLenum pname, const GLint * params)
{
    GLCALL(glMaterialiv, face, pname, params);
}

void mglGetMaterialfv (GLenum face, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMaterialfv, face, pname, params);
}

void mglGetMaterialiv (GLenum face, GLenum pname, GLint * params)
{
    GLCALL(glGetMaterialiv, face, pname, params);
}

void mglColorMaterial (GLenum face, GLenum mode)
{
    GLCALL(glColorMaterial, face, mode);
}

void mglPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    GLCALL(glPixelZoom, xfactor, yfactor);
}

void mglPixelStoref (GLenum pname, GLfloat param)
{
    GLCALL(glPixelStoref, pname, param);
}

void mglPixelStorei (GLenum pname, GLint param)
{
    GLCALL(glPixelStorei, pname, param);
}

void mglPixelTransferf (GLenum pname, GLfloat param)
{
    GLCALL(glPixelTransferf, pname, param);
}

void mglPixelTransferi (GLenum pname, GLint param)
{
    GLCALL(glPixelTransferi, pname, param);
}

void mglPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat * values)
{
    GLCALL(glPixelMapfv, map, mapsize, values);
}

void mglPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint * values)
{
    GLCALL(glPixelMapuiv, map, mapsize, values);
}

void mglPixelMapusv (GLenum map, GLsizei mapsize, const GLushort * values)
{
    GLCALL(glPixelMapusv, map, mapsize, values);
}

void mglGetPixelMapfv (GLenum map, GLfloat * values)
{
    GLCALL(glGetPixelMapfv, map, values);
}

void mglGetPixelMapuiv (GLenum map, GLuint * values)
{
    GLCALL(glGetPixelMapuiv, map, values);
}

void mglGetPixelMapusv (GLenum map, GLushort * values)
{
    GLCALL(glGetPixelMapusv, map, values);
}

void mglBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap)
{
    GLCALL(glBitmap, width, height, xorig, yorig, xmove, ymove, bitmap);
}

void mglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
    GLCALL(glReadPixels, x, y, width, height, format, type, pixels);
}

void mglDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glDrawPixels, width, height, format, type, pixels);
}

void mglCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    GLCALL(glCopyPixels, x, y, width, height, type);
}

void mglStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    GLCALL(glStencilFunc, func, ref, mask);
}

void mglStencilMask (GLuint mask)
{
    GLCALL(glStencilMask, mask);
}

void mglStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    GLCALL(glStencilOp, fail, zfail, zpass);
}

void mglClearStencil (GLint s)
{
    GLCALL(glClearStencil, s);
}

void mglTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    GLCALL(glTexGend, coord, pname, param);
}

void mglTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    GLCALL(glTexGenf, coord, pname, param);
}

void mglTexGeni (GLenum coord, GLenum pname, GLint param)
{
    GLCALL(glTexGeni, coord, pname, param);
}

void mglTexGendv (GLenum coord, GLenum pname, const GLdouble * params)
{
    GLCALL(glTexGendv, coord, pname, params);
}

void mglTexGenfv (GLenum coord, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexGenfv, coord, pname, params);
}

void mglTexGeniv (GLenum coord, GLenum pname, const GLint * params)
{
    GLCALL(glTexGeniv, coord, pname, params);
}

void mglGetTexGendv (GLenum coord, GLenum pname, GLdouble * params)
{
    GLCALL(glGetTexGendv, coord, pname, params);
}

void mglGetTexGenfv (GLenum coord, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexGenfv, coord, pname, params);
}

void mglGetTexGeniv (GLenum coord, GLenum pname, GLint * params)
{
    GLCALL(glGetTexGeniv, coord, pname, params);
}

void mglTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    GLCALL(glTexEnvf, target, pname, param);
}

void mglTexEnvi (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glTexEnvi, target, pname, param);
}

void mglTexEnvfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexEnvfv, target, pname, params);
}

void mglTexEnviv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexEnviv, target, pname, params);
}

void mglGetTexEnvfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexEnvfv, target, pname, params);
}

void mglGetTexEnviv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexEnviv, target, pname, params);
}

void mglTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    GLCALL(glTexParameterf, target, pname, param);
}

void mglTexParameteri (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glTexParameteri, target, pname, param);
}

void mglTexParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexParameterfv, target, pname, params);
}

void mglTexParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameteriv, target, pname, params);
}

void mglGetTexParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexParameterfv, target, pname, params);
}

void mglGetTexParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameteriv, target, pname, params);
}

void mglGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexLevelParameterfv, target, level, pname, params);
}

void mglGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint * params)
{
    GLCALL(glGetTexLevelParameteriv, target, level, pname, params);
}

void mglTexImage1D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage1D, target, level, internalFormat, width, border, format, type, pixels);
}

void mglTexImage2D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage2D, target, level, internalFormat, width, height, border, format, type, pixels);
}

void mglGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels)
{
    GLCALL(glGetTexImage, target, level, format, type, pixels);
}

void mglGenTextures (GLsizei n, GLuint * textures)
{
    GLCALL(glGenTextures, n, textures);
}

void mglDeleteTextures (GLsizei n, const GLuint * textures)
{
    GLCALL(glDeleteTextures, n, textures);
}

void mglBindTexture (GLenum target, GLuint texture)
{
    GLCALL(glBindTexture, target, texture);
}

void mglPrioritizeTextures (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    GLCALL(glPrioritizeTextures, n, textures, priorities);
}

GLboolean mglAreTexturesResident (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    return GLCALL(glAreTexturesResident, n, textures, residences);
}

GLboolean mglIsTexture (GLuint texture)
{
    return GLCALL(glIsTexture, texture);
}

void mglTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage1D, target, level, xoffset, width, format, type, pixels);
}

void mglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void mglCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    GLCALL(glCopyTexImage1D, target, level, internalformat, x, y, width, border);
}

void mglCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    GLCALL(glCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
}

void mglCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyTexSubImage1D, target, level, xoffset, x, y, width);
}

void mglCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height);
}

void mglMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points)
{
    GLCALL(glMap1d, target, u1, u2, stride, order, points);
}

void mglMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points)
{
    GLCALL(glMap1f, target, u1, u2, stride, order, points);
}

void mglMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points)
{
    GLCALL(glMap2d, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

void mglMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points)
{
    GLCALL(glMap2f, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

void mglGetMapdv (GLenum target, GLenum query, GLdouble * v)
{
    GLCALL(glGetMapdv, target, query, v);
}

void mglGetMapfv (GLenum target, GLenum query, GLfloat * v)
{
    GLCALL(glGetMapfv, target, query, v);
}

void mglGetMapiv (GLenum target, GLenum query, GLint * v)
{
    GLCALL(glGetMapiv, target, query, v);
}

void mglEvalCoord1d (GLdouble u)
{
    GLCALL(glEvalCoord1d, u);
}

void mglEvalCoord1f (GLfloat u)
{
    GLCALL(glEvalCoord1f, u);
}

void mglEvalCoord1dv (const GLdouble * u)
{
    GLCALL(glEvalCoord1dv, u);
}

void mglEvalCoord1fv (const GLfloat * u)
{
    GLCALL(glEvalCoord1fv, u);
}

void mglEvalCoord2d (GLdouble u, GLdouble v)
{
    GLCALL(glEvalCoord2d, u, v);
}

void mglEvalCoord2f (GLfloat u, GLfloat v)
{
    GLCALL(glEvalCoord2f, u, v);
}

void mglEvalCoord2dv (const GLdouble * u)
{
    GLCALL(glEvalCoord2dv, u);
}

void mglEvalCoord2fv (const GLfloat * u)
{
    GLCALL(glEvalCoord2fv, u);
}

void mglMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    GLCALL(glMapGrid1d, un, u1, u2);
}

void mglMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    GLCALL(glMapGrid1f, un, u1, u2);
}

void mglMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
    GLCALL(glMapGrid2d, un, u1, u2, vn, v1, v2);
}

void mglMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
    GLCALL(glMapGrid2f, un, u1, u2, vn, v1, v2);
}

void mglEvalPoint1 (GLint i)
{
    GLCALL(glEvalPoint1, i);
}

void mglEvalPoint2 (GLint i, GLint j)
{
    GLCALL(glEvalPoint2, i, j);
}

void mglEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    GLCALL(glEvalMesh1, mode, i1, i2);
}

void mglEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    GLCALL(glEvalMesh2, mode, i1, i2, j1, j2);
}

void mglFogf (GLenum pname, GLfloat param)
{
    GLCALL(glFogf, pname, param);
}

void mglFogi (GLenum pname, GLint param)
{
    GLCALL(glFogi, pname, param);
}

void mglFogfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glFogfv, pname, params);
}

void mglFogiv (GLenum pname, const GLint * params)
{
    GLCALL(glFogiv, pname, params);
}

void mglFeedbackBuffer (GLsizei size, GLenum type, GLfloat * buffer)
{
    GLCALL(glFeedbackBuffer, size, type, buffer);
}

void mglPassThrough (GLfloat token)
{
    GLCALL(glPassThrough, token);
}

void mglSelectBuffer (GLsizei size, GLuint * buffer)
{
    GLCALL(glSelectBuffer, size, buffer);
}

void mglInitNames ()
{
    GLCALL(glInitNames);
}

void mglLoadName (GLuint name)
{
    GLCALL(glLoadName, name);
}

void mglPushName (GLuint name)
{
    GLCALL(glPushName, name);
}

void mglPopName ()
{
    GLCALL(glPopName);
}

void mglDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawRangeElements, mode, start, end, count, type, indices);
}

void mglTexImage3D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage3D, target, level, internalFormat, width, height, depth, border, format, type, pixels);
}

void mglTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void mglCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage3D, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

void mglColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTable, target, internalformat, width, format, type, table);
}

void mglColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    GLCALL(glColorSubTable, target, start, count, format, type, data);
}

void mglColorTableParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glColorTableParameteriv, target, pname, params);
}

void mglColorTableParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glColorTableParameterfv, target, pname, params);
}

void mglCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorSubTable, target, start, x, y, width);
}

void mglCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorTable, target, internalformat, x, y, width);
}

void mglGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    GLCALL(glGetColorTable, target, format, type, table);
}

void mglGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfv, target, pname, params);
}

void mglGetColorTableParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameteriv, target, pname, params);
}

void mglBlendEquation (GLenum mode)
{
    GLCALL(glBlendEquation, mode);
}

void mglBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glBlendColor, red, green, blue, alpha);
}

void mglHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    GLCALL(glHistogram, target, width, internalformat, sink);
}

void mglResetHistogram (GLenum target)
{
    GLCALL(glResetHistogram, target);
}

void mglGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetHistogram, target, reset, format, type, values);
}

void mglGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetHistogramParameterfv, target, pname, params);
}

void mglGetHistogramParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetHistogramParameteriv, target, pname, params);
}

void mglMinmax (GLenum target, GLenum internalformat, GLboolean sink)
{
    GLCALL(glMinmax, target, internalformat, sink);
}

void mglResetMinmax (GLenum target)
{
    GLCALL(glResetMinmax, target);
}

void mglGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid * values)
{
    GLCALL(glGetMinmax, target, reset, format, types, values);
}

void mglGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMinmaxParameterfv, target, pname, params);
}

void mglGetMinmaxParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetMinmaxParameteriv, target, pname, params);
}

void mglConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter1D, target, internalformat, width, format, type, image);
}

void mglConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter2D, target, internalformat, width, height, format, type, image);
}

void mglConvolutionParameterf (GLenum target, GLenum pname, GLfloat params)
{
    GLCALL(glConvolutionParameterf, target, pname, params);
}

void mglConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glConvolutionParameterfv, target, pname, params);
}

void mglConvolutionParameteri (GLenum target, GLenum pname, GLint params)
{
    GLCALL(glConvolutionParameteri, target, pname, params);
}

void mglConvolutionParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glConvolutionParameteriv, target, pname, params);
}

void mglCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyConvolutionFilter1D, target, internalformat, x, y, width);
}

void mglCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyConvolutionFilter2D, target, internalformat, x, y, width, height);
}

void mglGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    GLCALL(glGetConvolutionFilter, target, format, type, image);
}

void mglGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetConvolutionParameterfv, target, pname, params);
}

void mglGetConvolutionParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetConvolutionParameteriv, target, pname, params);
}

void mglSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    GLCALL(glSeparableFilter2D, target, internalformat, width, height, format, type, row, column);
}

void mglGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    GLCALL(glGetSeparableFilter, target, format, type, row, column, span);
}

void mglActiveTexture (GLenum texture)
{
    GLCALL(glActiveTexture, texture);
}

void mglClientActiveTexture (GLenum texture)
{
    GLCALL(glClientActiveTexture, texture);
}

void mglCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage1D, target, level, internalformat, width, border, imageSize, data);
}

void mglCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
}

void mglCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage3D, target, level, internalformat, width, height, depth, border, imageSize, data);
}

void mglCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage1D, target, level, xoffset, width, format, imageSize, data);
}

void mglCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void mglCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void mglGetCompressedTexImage (GLenum target, GLint lod, GLvoid * img)
{
    GLCALL(glGetCompressedTexImage, target, lod, img);
}

void mglMultiTexCoord1d (GLenum target, GLdouble s)
{
    GLCALL(glMultiTexCoord1d, target, s);
}

void mglMultiTexCoord1dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord1dv, target, v);
}

void mglMultiTexCoord1f (GLenum target, GLfloat s)
{
    GLCALL(glMultiTexCoord1f, target, s);
}

void mglMultiTexCoord1fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord1fv, target, v);
}

void mglMultiTexCoord1i (GLenum target, GLint s)
{
    GLCALL(glMultiTexCoord1i, target, s);
}

void mglMultiTexCoord1iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord1iv, target, v);
}

void mglMultiTexCoord1s (GLenum target, GLshort s)
{
    GLCALL(glMultiTexCoord1s, target, s);
}

void mglMultiTexCoord1sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord1sv, target, v);
}

void mglMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t)
{
    GLCALL(glMultiTexCoord2d, target, s, t);
}

void mglMultiTexCoord2dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord2dv, target, v);
}

void mglMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t)
{
    GLCALL(glMultiTexCoord2f, target, s, t);
}

void mglMultiTexCoord2fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord2fv, target, v);
}

void mglMultiTexCoord2i (GLenum target, GLint s, GLint t)
{
    GLCALL(glMultiTexCoord2i, target, s, t);
}

void mglMultiTexCoord2iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord2iv, target, v);
}

void mglMultiTexCoord2s (GLenum target, GLshort s, GLshort t)
{
    GLCALL(glMultiTexCoord2s, target, s, t);
}

void mglMultiTexCoord2sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord2sv, target, v);
}

void mglMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glMultiTexCoord3d, target, s, t, r);
}

void mglMultiTexCoord3dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord3dv, target, v);
}

void mglMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glMultiTexCoord3f, target, s, t, r);
}

void mglMultiTexCoord3fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord3fv, target, v);
}

void mglMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r)
{
    GLCALL(glMultiTexCoord3i, target, s, t, r);
}

void mglMultiTexCoord3iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord3iv, target, v);
}

void mglMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r)
{
    GLCALL(glMultiTexCoord3s, target, s, t, r);
}

void mglMultiTexCoord3sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord3sv, target, v);
}

void mglMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glMultiTexCoord4d, target, s, t, r, q);
}

void mglMultiTexCoord4dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord4dv, target, v);
}

void mglMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glMultiTexCoord4f, target, s, t, r, q);
}

void mglMultiTexCoord4fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord4fv, target, v);
}

void mglMultiTexCoord4i (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glMultiTexCoord4i, target, s, t, r, q);
}

void mglMultiTexCoord4iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord4iv, target, v);
}

void mglMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glMultiTexCoord4s, target, s, t, r, q);
}

void mglMultiTexCoord4sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord4sv, target, v);
}

void mglLoadTransposeMatrixd (const GLdouble * m)
{
    GLCALL(glLoadTransposeMatrixd, m);
}

void mglLoadTransposeMatrixf (const GLfloat * m)
{
    GLCALL(glLoadTransposeMatrixf, m);
}

void mglMultTransposeMatrixd (const GLdouble * m)
{
    GLCALL(glMultTransposeMatrixd, m);
}

void mglMultTransposeMatrixf (const GLfloat * m)
{
    GLCALL(glMultTransposeMatrixf, m);
}

void mglSampleCoverage (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleCoverage, value, invert);
}

void mglActiveTextureARB (GLenum texture)
{
    GLCALL(glActiveTextureARB, texture);
}

void mglClientActiveTextureARB (GLenum texture)
{
    GLCALL(glClientActiveTextureARB, texture);
}

void mglMultiTexCoord1dARB (GLenum target, GLdouble s)
{
    GLCALL(glMultiTexCoord1dARB, target, s);
}

void mglMultiTexCoord1dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord1dvARB, target, v);
}

void mglMultiTexCoord1fARB (GLenum target, GLfloat s)
{
    GLCALL(glMultiTexCoord1fARB, target, s);
}

void mglMultiTexCoord1fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord1fvARB, target, v);
}

void mglMultiTexCoord1iARB (GLenum target, GLint s)
{
    GLCALL(glMultiTexCoord1iARB, target, s);
}

void mglMultiTexCoord1ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord1ivARB, target, v);
}

void mglMultiTexCoord1sARB (GLenum target, GLshort s)
{
    GLCALL(glMultiTexCoord1sARB, target, s);
}

void mglMultiTexCoord1svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord1svARB, target, v);
}

void mglMultiTexCoord2dARB (GLenum target, GLdouble s, GLdouble t)
{
    GLCALL(glMultiTexCoord2dARB, target, s, t);
}

void mglMultiTexCoord2dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord2dvARB, target, v);
}

void mglMultiTexCoord2fARB (GLenum target, GLfloat s, GLfloat t)
{
    GLCALL(glMultiTexCoord2fARB, target, s, t);
}

void mglMultiTexCoord2fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord2fvARB, target, v);
}

void mglMultiTexCoord2iARB (GLenum target, GLint s, GLint t)
{
    GLCALL(glMultiTexCoord2iARB, target, s, t);
}

void mglMultiTexCoord2ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord2ivARB, target, v);
}

void mglMultiTexCoord2sARB (GLenum target, GLshort s, GLshort t)
{
    GLCALL(glMultiTexCoord2sARB, target, s, t);
}

void mglMultiTexCoord2svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord2svARB, target, v);
}

void mglMultiTexCoord3dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glMultiTexCoord3dARB, target, s, t, r);
}

void mglMultiTexCoord3dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord3dvARB, target, v);
}

void mglMultiTexCoord3fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glMultiTexCoord3fARB, target, s, t, r);
}

void mglMultiTexCoord3fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord3fvARB, target, v);
}

void mglMultiTexCoord3iARB (GLenum target, GLint s, GLint t, GLint r)
{
    GLCALL(glMultiTexCoord3iARB, target, s, t, r);
}

void mglMultiTexCoord3ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord3ivARB, target, v);
}

void mglMultiTexCoord3sARB (GLenum target, GLshort s, GLshort t, GLshort r)
{
    GLCALL(glMultiTexCoord3sARB, target, s, t, r);
}

void mglMultiTexCoord3svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord3svARB, target, v);
}

void mglMultiTexCoord4dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glMultiTexCoord4dARB, target, s, t, r, q);
}

void mglMultiTexCoord4dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord4dvARB, target, v);
}

void mglMultiTexCoord4fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glMultiTexCoord4fARB, target, s, t, r, q);
}

void mglMultiTexCoord4fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord4fvARB, target, v);
}

void mglMultiTexCoord4iARB (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glMultiTexCoord4iARB, target, s, t, r, q);
}

void mglMultiTexCoord4ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord4ivARB, target, v);
}

void mglMultiTexCoord4sARB (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glMultiTexCoord4sARB, target, s, t, r, q);
}

void mglMultiTexCoord4svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord4svARB, target, v);
}

void mglBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void mglFogCoordf (GLfloat coord)
{
    GLCALL(glFogCoordf, coord);
}

void mglFogCoordfv (const GLfloat * coord)
{
    GLCALL(glFogCoordfv, coord);
}

void mglFogCoordd (GLdouble coord)
{
    GLCALL(glFogCoordd, coord);
}

void mglFogCoorddv (const GLdouble * coord)
{
    GLCALL(glFogCoorddv, coord);
}

void mglFogCoordPointer (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glFogCoordPointer, type, stride, pointer);
}

void mglMultiDrawArrays (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    GLCALL(glMultiDrawArrays, mode, first, count, primcount);
}

void mglMultiDrawElements (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    GLCALL(glMultiDrawElements, mode, count, type, indices, primcount);
}

void mglPointParameterf (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterf, pname, param);
}

void mglPointParameterfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfv, pname, params);
}

void mglPointParameteri (GLenum pname, GLint param)
{
    GLCALL(glPointParameteri, pname, param);
}

void mglPointParameteriv (GLenum pname, const GLint * params)
{
    GLCALL(glPointParameteriv, pname, params);
}

void mglSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glSecondaryColor3b, red, green, blue);
}

void mglSecondaryColor3bv (const GLbyte * v)
{
    GLCALL(glSecondaryColor3bv, v);
}

void mglSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glSecondaryColor3d, red, green, blue);
}

void mglSecondaryColor3dv (const GLdouble * v)
{
    GLCALL(glSecondaryColor3dv, v);
}

void mglSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glSecondaryColor3f, red, green, blue);
}

void mglSecondaryColor3fv (const GLfloat * v)
{
    GLCALL(glSecondaryColor3fv, v);
}

void mglSecondaryColor3i (GLint red, GLint green, GLint blue)
{
    GLCALL(glSecondaryColor3i, red, green, blue);
}

void mglSecondaryColor3iv (const GLint * v)
{
    GLCALL(glSecondaryColor3iv, v);
}

void mglSecondaryColor3s (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glSecondaryColor3s, red, green, blue);
}

void mglSecondaryColor3sv (const GLshort * v)
{
    GLCALL(glSecondaryColor3sv, v);
}

void mglSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glSecondaryColor3ub, red, green, blue);
}

void mglSecondaryColor3ubv (const GLubyte * v)
{
    GLCALL(glSecondaryColor3ubv, v);
}

void mglSecondaryColor3ui (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glSecondaryColor3ui, red, green, blue);
}

void mglSecondaryColor3uiv (const GLuint * v)
{
    GLCALL(glSecondaryColor3uiv, v);
}

void mglSecondaryColor3us (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glSecondaryColor3us, red, green, blue);
}

void mglSecondaryColor3usv (const GLushort * v)
{
    GLCALL(glSecondaryColor3usv, v);
}

void mglSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glSecondaryColorPointer, size, type, stride, pointer);
}

void mglWindowPos2d (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2d, x, y);
}

void mglWindowPos2dv (const GLdouble * v)
{
    GLCALL(glWindowPos2dv, v);
}

void mglWindowPos2f (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2f, x, y);
}

void mglWindowPos2fv (const GLfloat * v)
{
    GLCALL(glWindowPos2fv, v);
}

void mglWindowPos2i (GLint x, GLint y)
{
    GLCALL(glWindowPos2i, x, y);
}

void mglWindowPos2iv (const GLint * v)
{
    GLCALL(glWindowPos2iv, v);
}

void mglWindowPos2s (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2s, x, y);
}

void mglWindowPos2sv (const GLshort * v)
{
    GLCALL(glWindowPos2sv, v);
}

void mglWindowPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3d, x, y, z);
}

void mglWindowPos3dv (const GLdouble * v)
{
    GLCALL(glWindowPos3dv, v);
}

void mglWindowPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3f, x, y, z);
}

void mglWindowPos3fv (const GLfloat * v)
{
    GLCALL(glWindowPos3fv, v);
}

void mglWindowPos3i (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3i, x, y, z);
}

void mglWindowPos3iv (const GLint * v)
{
    GLCALL(glWindowPos3iv, v);
}

void mglWindowPos3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3s, x, y, z);
}

void mglWindowPos3sv (const GLshort * v)
{
    GLCALL(glWindowPos3sv, v);
}

void mglGenQueries (GLsizei n, GLuint * ids)
{
    GLCALL(glGenQueries, n, ids);
}

void mglDeleteQueries (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteQueries, n, ids);
}

GLboolean mglIsQuery (GLuint id)
{
    return GLCALL(glIsQuery, id);
}

void mglBeginQuery (GLenum target, GLuint id)
{
    GLCALL(glBeginQuery, target, id);
}

void mglEndQuery (GLenum target)
{
    GLCALL(glEndQuery, target);
}

void mglGetQueryiv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryiv, target, pname, params);
}

void mglGetQueryObjectiv (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryObjectiv, id, pname, params);
}

void mglGetQueryObjectuiv (GLuint id, GLenum pname, GLuint * params)
{
    GLCALL(glGetQueryObjectuiv, id, pname, params);
}

void mglBindBuffer (GLenum target, GLuint buffer)
{
    GLCALL(glBindBuffer, target, buffer);
}

void mglDeleteBuffers (GLsizei n, const GLuint * buffers)
{
    GLCALL(glDeleteBuffers, n, buffers);
}

void mglGenBuffers (GLsizei n, GLuint * buffers)
{
    GLCALL(glGenBuffers, n, buffers);
}

GLboolean mglIsBuffer (GLuint buffer)
{
    return GLCALL(glIsBuffer, buffer);
}

void mglBufferData (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
    GLCALL(glBufferData, target, size, data, usage);
}

void mglBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
    GLCALL(glBufferSubData, target, offset, size, data);
}

void mglGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data)
{
    GLCALL(glGetBufferSubData, target, offset, size, data);
}

GLvoid* mglMapBuffer (GLenum target, GLenum access)
{
    return GLCALL(glMapBuffer, target, access);
}

GLboolean mglUnmapBuffer (GLenum target)
{
    return GLCALL(glUnmapBuffer, target);
}

void mglGetBufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetBufferParameteriv, target, pname, params);
}

void mglGetBufferPointerv (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetBufferPointerv, target, pname, params);
}

void mglBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparate, modeRGB, modeAlpha);
}

void mglDrawBuffers (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffers, n, bufs);
}

void mglStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    GLCALL(glStencilOpSeparate, face, sfail, dpfail, dppass);
}

void mglStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    GLCALL(glStencilFuncSeparate, face, func, ref, mask);
}

void mglStencilMaskSeparate (GLenum face, GLuint mask)
{
    GLCALL(glStencilMaskSeparate, face, mask);
}

void mglAttachShader (GLuint program, GLuint shader)
{
    GLCALL(glAttachShader, program, shader);
}

void mglBindAttribLocation (GLuint program, GLuint index, const GLchar * name)
{
    GLCALL(glBindAttribLocation, program, index, name);
}

void mglCompileShader (GLuint shader)
{
    GLCALL(glCompileShader, shader);
}

GLuint mglCreateProgram ()
{
    return GLCALL(glCreateProgram);
}

GLuint mglCreateShader (GLenum type)
{
    return GLCALL(glCreateShader, type);
}

void mglDeleteProgram (GLuint program)
{
    GLCALL(glDeleteProgram, program);
}

void mglDeleteShader (GLuint shader)
{
    GLCALL(glDeleteShader, shader);
}

void mglDetachShader (GLuint program, GLuint shader)
{
    GLCALL(glDetachShader, program, shader);
}

void mglDisableVertexAttribArray (GLuint index)
{
    GLCALL(glDisableVertexAttribArray, index);
}

void mglEnableVertexAttribArray (GLuint index)
{
    GLCALL(glEnableVertexAttribArray, index);
}

void mglGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetActiveAttrib, program, index, bufSize, length, size, type, name);
}

void mglGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetActiveUniform, program, index, bufSize, length, size, type, name);
}

void mglGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei * count, GLuint * obj)
{
    GLCALL(glGetAttachedShaders, program, maxCount, count, obj);
}

GLint mglGetAttribLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetAttribLocation, program, name);
}

void mglGetProgramiv (GLuint program, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramiv, program, pname, params);
}

void mglGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    GLCALL(glGetProgramInfoLog, program, bufSize, length, infoLog);
}

void mglGetShaderiv (GLuint shader, GLenum pname, GLint * params)
{
    GLCALL(glGetShaderiv, shader, pname, params);
}

void mglGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    GLCALL(glGetShaderInfoLog, shader, bufSize, length, infoLog);
}

void mglGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source)
{
    GLCALL(glGetShaderSource, shader, bufSize, length, source);
}

GLint mglGetUniformLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetUniformLocation, program, name);
}

void mglGetUniformfv (GLuint program, GLint location, GLfloat * params)
{
    GLCALL(glGetUniformfv, program, location, params);
}

void mglGetUniformiv (GLuint program, GLint location, GLint * params)
{
    GLCALL(glGetUniformiv, program, location, params);
}

void mglGetVertexAttribdv (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdv, index, pname, params);
}

void mglGetVertexAttribfv (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfv, index, pname, params);
}

void mglGetVertexAttribiv (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribiv, index, pname, params);
}

void mglGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointerv, index, pname, pointer);
}

GLboolean mglIsProgram (GLuint program)
{
    return GLCALL(glIsProgram, program);
}

GLboolean mglIsShader (GLuint shader)
{
    return GLCALL(glIsShader, shader);
}

void mglLinkProgram (GLuint program)
{
    GLCALL(glLinkProgram, program);
}

void mglShaderSource (GLuint shader, GLsizei count, const GLchar *  * string, const GLint * length)
{
    GLCALL(glShaderSource, shader, count, string, length);
}

void mglUseProgram (GLuint program)
{
    GLCALL(glUseProgram, program);
}

void mglUniform1f (GLint location, GLfloat v0)
{
    GLCALL(glUniform1f, location, v0);
}

void mglUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    GLCALL(glUniform2f, location, v0, v1);
}

void mglUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLCALL(glUniform3f, location, v0, v1, v2);
}

void mglUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLCALL(glUniform4f, location, v0, v1, v2, v3);
}

void mglUniform1i (GLint location, GLint v0)
{
    GLCALL(glUniform1i, location, v0);
}

void mglUniform2i (GLint location, GLint v0, GLint v1)
{
    GLCALL(glUniform2i, location, v0, v1);
}

void mglUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    GLCALL(glUniform3i, location, v0, v1, v2);
}

void mglUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLCALL(glUniform4i, location, v0, v1, v2, v3);
}

void mglUniform1fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform1fv, location, count, value);
}

void mglUniform2fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform2fv, location, count, value);
}

void mglUniform3fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform3fv, location, count, value);
}

void mglUniform4fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform4fv, location, count, value);
}

void mglUniform1iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform1iv, location, count, value);
}

void mglUniform2iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform2iv, location, count, value);
}

void mglUniform3iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform3iv, location, count, value);
}

void mglUniform4iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform4iv, location, count, value);
}

void mglUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2fv, location, count, transpose, value);
}

void mglUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3fv, location, count, transpose, value);
}

void mglUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4fv, location, count, transpose, value);
}

void mglValidateProgram (GLuint program)
{
    GLCALL(glValidateProgram, program);
}

void mglVertexAttrib1d (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1d, index, x);
}

void mglVertexAttrib1dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dv, index, v);
}

void mglVertexAttrib1f (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1f, index, x);
}

void mglVertexAttrib1fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fv, index, v);
}

void mglVertexAttrib1s (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1s, index, x);
}

void mglVertexAttrib1sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1sv, index, v);
}

void mglVertexAttrib2d (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2d, index, x, y);
}

void mglVertexAttrib2dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dv, index, v);
}

void mglVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2f, index, x, y);
}

void mglVertexAttrib2fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fv, index, v);
}

void mglVertexAttrib2s (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2s, index, x, y);
}

void mglVertexAttrib2sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2sv, index, v);
}

void mglVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3d, index, x, y, z);
}

void mglVertexAttrib3dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dv, index, v);
}

void mglVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3f, index, x, y, z);
}

void mglVertexAttrib3fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fv, index, v);
}

void mglVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3s, index, x, y, z);
}

void mglVertexAttrib3sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3sv, index, v);
}

void mglVertexAttrib4Nbv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4Nbv, index, v);
}

void mglVertexAttrib4Niv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4Niv, index, v);
}

void mglVertexAttrib4Nsv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4Nsv, index, v);
}

void mglVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4Nub, index, x, y, z, w);
}

void mglVertexAttrib4Nubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4Nubv, index, v);
}

void mglVertexAttrib4Nuiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4Nuiv, index, v);
}

void mglVertexAttrib4Nusv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4Nusv, index, v);
}

void mglVertexAttrib4bv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4bv, index, v);
}

void mglVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4d, index, x, y, z, w);
}

void mglVertexAttrib4dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dv, index, v);
}

void mglVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4f, index, x, y, z, w);
}

void mglVertexAttrib4fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fv, index, v);
}

void mglVertexAttrib4iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4iv, index, v);
}

void mglVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4s, index, x, y, z, w);
}

void mglVertexAttrib4sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4sv, index, v);
}

void mglVertexAttrib4ubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubv, index, v);
}

void mglVertexAttrib4uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4uiv, index, v);
}

void mglVertexAttrib4usv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4usv, index, v);
}

void mglVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
}

void mglUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2x3fv, location, count, transpose, value);
}

void mglUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3x2fv, location, count, transpose, value);
}

void mglUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2x4fv, location, count, transpose, value);
}

void mglUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4x2fv, location, count, transpose, value);
}

void mglUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3x4fv, location, count, transpose, value);
}

void mglUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4x3fv, location, count, transpose, value);
}

void mglLoadTransposeMatrixfARB (const GLfloat * m)
{
    GLCALL(glLoadTransposeMatrixfARB, m);
}

void mglLoadTransposeMatrixdARB (const GLdouble * m)
{
    GLCALL(glLoadTransposeMatrixdARB, m);
}

void mglMultTransposeMatrixfARB (const GLfloat * m)
{
    GLCALL(glMultTransposeMatrixfARB, m);
}

void mglMultTransposeMatrixdARB (const GLdouble * m)
{
    GLCALL(glMultTransposeMatrixdARB, m);
}

void mglSampleCoverageARB (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleCoverageARB, value, invert);
}

void mglCompressedTexImage3DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage3DARB, target, level, internalformat, width, height, depth, border, imageSize, data);
}

void mglCompressedTexImage2DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage2DARB, target, level, internalformat, width, height, border, imageSize, data);
}

void mglCompressedTexImage1DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage1DARB, target, level, internalformat, width, border, imageSize, data);
}

void mglCompressedTexSubImage3DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage3DARB, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

void mglCompressedTexSubImage2DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage2DARB, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

void mglCompressedTexSubImage1DARB (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage1DARB, target, level, xoffset, width, format, imageSize, data);
}

void mglGetCompressedTexImageARB (GLenum target, GLint level, GLvoid * img)
{
    GLCALL(glGetCompressedTexImageARB, target, level, img);
}

void mglPointParameterfARB (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfARB, pname, param);
}

void mglPointParameterfvARB (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvARB, pname, params);
}

void mglWindowPos2dARB (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2dARB, x, y);
}

void mglWindowPos2dvARB (const GLdouble * v)
{
    GLCALL(glWindowPos2dvARB, v);
}

void mglWindowPos2fARB (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2fARB, x, y);
}

void mglWindowPos2fvARB (const GLfloat * v)
{
    GLCALL(glWindowPos2fvARB, v);
}

void mglWindowPos2iARB (GLint x, GLint y)
{
    GLCALL(glWindowPos2iARB, x, y);
}

void mglWindowPos2ivARB (const GLint * v)
{
    GLCALL(glWindowPos2ivARB, v);
}

void mglWindowPos2sARB (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2sARB, x, y);
}

void mglWindowPos2svARB (const GLshort * v)
{
    GLCALL(glWindowPos2svARB, v);
}

void mglWindowPos3dARB (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3dARB, x, y, z);
}

void mglWindowPos3dvARB (const GLdouble * v)
{
    GLCALL(glWindowPos3dvARB, v);
}

void mglWindowPos3fARB (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3fARB, x, y, z);
}

void mglWindowPos3fvARB (const GLfloat * v)
{
    GLCALL(glWindowPos3fvARB, v);
}

void mglWindowPos3iARB (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3iARB, x, y, z);
}

void mglWindowPos3ivARB (const GLint * v)
{
    GLCALL(glWindowPos3ivARB, v);
}

void mglWindowPos3sARB (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3sARB, x, y, z);
}

void mglWindowPos3svARB (const GLshort * v)
{
    GLCALL(glWindowPos3svARB, v);
}

void mglVertexAttrib1dARB (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1dARB, index, x);
}

void mglVertexAttrib1dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dvARB, index, v);
}

void mglVertexAttrib1fARB (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1fARB, index, x);
}

void mglVertexAttrib1fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fvARB, index, v);
}

void mglVertexAttrib1sARB (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1sARB, index, x);
}

void mglVertexAttrib1svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1svARB, index, v);
}

void mglVertexAttrib2dARB (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2dARB, index, x, y);
}

void mglVertexAttrib2dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dvARB, index, v);
}

void mglVertexAttrib2fARB (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2fARB, index, x, y);
}

void mglVertexAttrib2fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fvARB, index, v);
}

void mglVertexAttrib2sARB (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2sARB, index, x, y);
}

void mglVertexAttrib2svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2svARB, index, v);
}

void mglVertexAttrib3dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3dARB, index, x, y, z);
}

void mglVertexAttrib3dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dvARB, index, v);
}

void mglVertexAttrib3fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3fARB, index, x, y, z);
}

void mglVertexAttrib3fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fvARB, index, v);
}

void mglVertexAttrib3sARB (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3sARB, index, x, y, z);
}

void mglVertexAttrib3svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3svARB, index, v);
}

void mglVertexAttrib4NbvARB (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4NbvARB, index, v);
}

void mglVertexAttrib4NivARB (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4NivARB, index, v);
}

void mglVertexAttrib4NsvARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4NsvARB, index, v);
}

void mglVertexAttrib4NubARB (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4NubARB, index, x, y, z, w);
}

void mglVertexAttrib4NubvARB (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4NubvARB, index, v);
}

void mglVertexAttrib4NuivARB (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4NuivARB, index, v);
}

void mglVertexAttrib4NusvARB (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4NusvARB, index, v);
}

void mglVertexAttrib4bvARB (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4bvARB, index, v);
}

void mglVertexAttrib4dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4dARB, index, x, y, z, w);
}

void mglVertexAttrib4dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dvARB, index, v);
}

void mglVertexAttrib4fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4fARB, index, x, y, z, w);
}

void mglVertexAttrib4fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fvARB, index, v);
}

void mglVertexAttrib4ivARB (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4ivARB, index, v);
}

void mglVertexAttrib4sARB (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4sARB, index, x, y, z, w);
}

void mglVertexAttrib4svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4svARB, index, v);
}

void mglVertexAttrib4ubvARB (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubvARB, index, v);
}

void mglVertexAttrib4uivARB (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4uivARB, index, v);
}

void mglVertexAttrib4usvARB (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4usvARB, index, v);
}

void mglVertexAttribPointerARB (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointerARB, index, size, type, normalized, stride, pointer);
}

void mglEnableVertexAttribArrayARB (GLuint index)
{
    GLCALL(glEnableVertexAttribArrayARB, index);
}

void mglDisableVertexAttribArrayARB (GLuint index)
{
    GLCALL(glDisableVertexAttribArrayARB, index);
}

void mglProgramStringARB (GLenum target, GLenum format, GLsizei len, const GLvoid * string)
{
    GLCALL(glProgramStringARB, target, format, len, string);
}

void mglBindProgramARB (GLenum target, GLuint program)
{
    GLCALL(glBindProgramARB, target, program);
}

void mglDeleteProgramsARB (GLsizei n, const GLuint * programs)
{
    GLCALL(glDeleteProgramsARB, n, programs);
}

void mglGenProgramsARB (GLsizei n, GLuint * programs)
{
    GLCALL(glGenProgramsARB, n, programs);
}

void mglProgramEnvParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramEnvParameter4dARB, target, index, x, y, z, w);
}

void mglProgramEnvParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    GLCALL(glProgramEnvParameter4dvARB, target, index, params);
}

void mglProgramEnvParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramEnvParameter4fARB, target, index, x, y, z, w);
}

void mglProgramEnvParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    GLCALL(glProgramEnvParameter4fvARB, target, index, params);
}

void mglProgramLocalParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramLocalParameter4dARB, target, index, x, y, z, w);
}

void mglProgramLocalParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    GLCALL(glProgramLocalParameter4dvARB, target, index, params);
}

void mglProgramLocalParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramLocalParameter4fARB, target, index, x, y, z, w);
}

void mglProgramLocalParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    GLCALL(glProgramLocalParameter4fvARB, target, index, params);
}

void mglGetProgramEnvParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    GLCALL(glGetProgramEnvParameterdvARB, target, index, params);
}

void mglGetProgramEnvParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    GLCALL(glGetProgramEnvParameterfvARB, target, index, params);
}

void mglGetProgramLocalParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    GLCALL(glGetProgramLocalParameterdvARB, target, index, params);
}

void mglGetProgramLocalParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    GLCALL(glGetProgramLocalParameterfvARB, target, index, params);
}

void mglGetProgramivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramivARB, target, pname, params);
}

void mglGetProgramStringARB (GLenum target, GLenum pname, GLvoid * string)
{
    GLCALL(glGetProgramStringARB, target, pname, string);
}

void mglGetVertexAttribdvARB (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdvARB, index, pname, params);
}

void mglGetVertexAttribfvARB (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfvARB, index, pname, params);
}

void mglGetVertexAttribivARB (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribivARB, index, pname, params);
}

void mglGetVertexAttribPointervARB (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointervARB, index, pname, pointer);
}

GLboolean mglIsProgramARB (GLuint program)
{
    return GLCALL(glIsProgramARB, program);
}

void mglBindBufferARB (GLenum target, GLuint buffer)
{
    GLCALL(glBindBufferARB, target, buffer);
}

void mglDeleteBuffersARB (GLsizei n, const GLuint * buffers)
{
    GLCALL(glDeleteBuffersARB, n, buffers);
}

void mglGenBuffersARB (GLsizei n, GLuint * buffers)
{
    GLCALL(glGenBuffersARB, n, buffers);
}

GLboolean mglIsBufferARB (GLuint buffer)
{
    return GLCALL(glIsBufferARB, buffer);
}

void mglBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage)
{
    GLCALL(glBufferDataARB, target, size, data, usage);
}

void mglBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
    GLCALL(glBufferSubDataARB, target, offset, size, data);
}

void mglGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid * data)
{
    GLCALL(glGetBufferSubDataARB, target, offset, size, data);
}

GLvoid* mglMapBufferARB (GLenum target, GLenum access)
{
    return GLCALL(glMapBufferARB, target, access);
}

GLboolean mglUnmapBufferARB (GLenum target)
{
    return GLCALL(glUnmapBufferARB, target);
}

void mglGetBufferParameterivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetBufferParameterivARB, target, pname, params);
}

void mglGetBufferPointervARB (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetBufferPointervARB, target, pname, params);
}

void mglGenQueriesARB (GLsizei n, GLuint * ids)
{
    GLCALL(glGenQueriesARB, n, ids);
}

void mglDeleteQueriesARB (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteQueriesARB, n, ids);
}

GLboolean mglIsQueryARB (GLuint id)
{
    return GLCALL(glIsQueryARB, id);
}

void mglBeginQueryARB (GLenum target, GLuint id)
{
    GLCALL(glBeginQueryARB, target, id);
}

void mglEndQueryARB (GLenum target)
{
    GLCALL(glEndQueryARB, target);
}

void mglGetQueryivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryivARB, target, pname, params);
}

void mglGetQueryObjectivARB (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryObjectivARB, id, pname, params);
}

void mglGetQueryObjectuivARB (GLuint id, GLenum pname, GLuint * params)
{
    GLCALL(glGetQueryObjectuivARB, id, pname, params);
}

void mglDeleteObjectARB (GLhandleARB obj)
{
    GLCALL(glDeleteObjectARB, obj);
}

GLhandleARB mglGetHandleARB (GLenum pname)
{
    return GLCALL(glGetHandleARB, pname);
}

void mglDetachObjectARB (GLhandleARB containerObj, GLhandleARB attachedObj)
{
    GLCALL(glDetachObjectARB, containerObj, attachedObj);
}

GLhandleARB mglCreateShaderObjectARB (GLenum shaderType)
{
    return GLCALL(glCreateShaderObjectARB, shaderType);
}

void mglShaderSourceARB (GLhandleARB shaderObj, GLsizei count, const GLcharARB *  * string, const GLint * length)
{
    GLCALL(glShaderSourceARB, shaderObj, count, string, length);
}

void mglCompileShaderARB (GLhandleARB shaderObj)
{
    GLCALL(glCompileShaderARB, shaderObj);
}

GLhandleARB mglCreateProgramObjectARB ()
{
    return GLCALL(glCreateProgramObjectARB);
}

void mglAttachObjectARB (GLhandleARB containerObj, GLhandleARB obj)
{
    GLCALL(glAttachObjectARB, containerObj, obj);
}

void mglLinkProgramARB (GLhandleARB programObj)
{
    GLCALL(glLinkProgramARB, programObj);
}

void mglUseProgramObjectARB (GLhandleARB programObj)
{
    GLCALL(glUseProgramObjectARB, programObj);
}

void mglValidateProgramARB (GLhandleARB programObj)
{
    GLCALL(glValidateProgramARB, programObj);
}

void mglUniform1fARB (GLint location, GLfloat v0)
{
    GLCALL(glUniform1fARB, location, v0);
}

void mglUniform2fARB (GLint location, GLfloat v0, GLfloat v1)
{
    GLCALL(glUniform2fARB, location, v0, v1);
}

void mglUniform3fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLCALL(glUniform3fARB, location, v0, v1, v2);
}

void mglUniform4fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLCALL(glUniform4fARB, location, v0, v1, v2, v3);
}

void mglUniform1iARB (GLint location, GLint v0)
{
    GLCALL(glUniform1iARB, location, v0);
}

void mglUniform2iARB (GLint location, GLint v0, GLint v1)
{
    GLCALL(glUniform2iARB, location, v0, v1);
}

void mglUniform3iARB (GLint location, GLint v0, GLint v1, GLint v2)
{
    GLCALL(glUniform3iARB, location, v0, v1, v2);
}

void mglUniform4iARB (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLCALL(glUniform4iARB, location, v0, v1, v2, v3);
}

void mglUniform1fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform1fvARB, location, count, value);
}

void mglUniform2fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform2fvARB, location, count, value);
}

void mglUniform3fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform3fvARB, location, count, value);
}

void mglUniform4fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform4fvARB, location, count, value);
}

void mglUniform1ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform1ivARB, location, count, value);
}

void mglUniform2ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform2ivARB, location, count, value);
}

void mglUniform3ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform3ivARB, location, count, value);
}

void mglUniform4ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform4ivARB, location, count, value);
}

void mglUniformMatrix2fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2fvARB, location, count, transpose, value);
}

void mglUniformMatrix3fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3fvARB, location, count, transpose, value);
}

void mglUniformMatrix4fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4fvARB, location, count, transpose, value);
}

void mglGetObjectParameterfvARB (GLhandleARB obj, GLenum pname, GLfloat * params)
{
    GLCALL(glGetObjectParameterfvARB, obj, pname, params);
}

void mglGetObjectParameterivARB (GLhandleARB obj, GLenum pname, GLint * params)
{
    GLCALL(glGetObjectParameterivARB, obj, pname, params);
}

void mglGetInfoLogARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * infoLog)
{
    GLCALL(glGetInfoLogARB, obj, maxLength, length, infoLog);
}

void mglGetAttachedObjectsARB (GLhandleARB containerObj, GLsizei maxCount, GLsizei * count, GLhandleARB * obj)
{
    GLCALL(glGetAttachedObjectsARB, containerObj, maxCount, count, obj);
}

GLint mglGetUniformLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    return GLCALL(glGetUniformLocationARB, programObj, name);
}

void mglGetActiveUniformARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    GLCALL(glGetActiveUniformARB, programObj, index, maxLength, length, size, type, name);
}

void mglGetUniformfvARB (GLhandleARB programObj, GLint location, GLfloat * params)
{
    GLCALL(glGetUniformfvARB, programObj, location, params);
}

void mglGetUniformivARB (GLhandleARB programObj, GLint location, GLint * params)
{
    GLCALL(glGetUniformivARB, programObj, location, params);
}

void mglGetShaderSourceARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * source)
{
    GLCALL(glGetShaderSourceARB, obj, maxLength, length, source);
}

void mglBindAttribLocationARB (GLhandleARB programObj, GLuint index, const GLcharARB * name)
{
    GLCALL(glBindAttribLocationARB, programObj, index, name);
}

void mglGetActiveAttribARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    GLCALL(glGetActiveAttribARB, programObj, index, maxLength, length, size, type, name);
}

GLint mglGetAttribLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    return GLCALL(glGetAttribLocationARB, programObj, name);
}

void mglDrawBuffersARB (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffersARB, n, bufs);
}

GLboolean mglIsRenderbuffer (GLuint renderbuffer)
{
    return GLCALL(glIsRenderbuffer, renderbuffer);
}

void mglBindRenderbuffer (GLenum target, GLuint renderbuffer)
{
    GLCALL(glBindRenderbuffer, target, renderbuffer);
}

void mglDeleteRenderbuffers (GLsizei n, const GLuint * renderbuffers)
{
    GLCALL(glDeleteRenderbuffers, n, renderbuffers);
}

void mglGenRenderbuffers (GLsizei n, GLuint * renderbuffers)
{
    GLCALL(glGenRenderbuffers, n, renderbuffers);
}

void mglRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorage, target, internalformat, width, height);
}

void mglGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetRenderbufferParameteriv, target, pname, params);
}

GLboolean mglIsFramebuffer (GLuint framebuffer)
{
    return GLCALL(glIsFramebuffer, framebuffer);
}

void mglBindFramebuffer (GLenum target, GLuint framebuffer)
{
    GLCALL(glBindFramebuffer, target, framebuffer);
}

void mglDeleteFramebuffers (GLsizei n, const GLuint * framebuffers)
{
    GLCALL(glDeleteFramebuffers, n, framebuffers);
}

void mglGenFramebuffers (GLsizei n, GLuint * framebuffers)
{
    GLCALL(glGenFramebuffers, n, framebuffers);
}

GLenum mglCheckFramebufferStatus (GLenum target)
{
    return GLCALL(glCheckFramebufferStatus, target);
}

void mglFramebufferTexture1D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture1D, target, attachment, textarget, texture, level);
}

void mglFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture2D, target, attachment, textarget, texture, level);
}

void mglFramebufferTexture3D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    GLCALL(glFramebufferTexture3D, target, attachment, textarget, texture, level, zoffset);
}

void mglFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    GLCALL(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
}

void mglGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    GLCALL(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params);
}

void mglGenerateMipmap (GLenum target)
{
    GLCALL(glGenerateMipmap, target);
}

void mglBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    GLCALL(glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void mglRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageMultisample, target, samples, internalformat, width, height);
}

void mglFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GLCALL(glFramebufferTextureLayer, target, attachment, texture, level, layer);
}

void mglBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glBlendColorEXT, red, green, blue, alpha);
}

void mglPolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    GLCALL(glPolygonOffsetEXT, factor, bias);
}

void mglTexImage3DEXT (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage3DEXT, target, level, internalformat, width, height, depth, border, format, type, pixels);
}

void mglTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

void mglTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage1DEXT, target, level, xoffset, width, format, type, pixels);
}

void mglTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage2DEXT, target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void mglCopyTexImage1DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    GLCALL(glCopyTexImage1DEXT, target, level, internalformat, x, y, width, border);
}

void mglCopyTexImage2DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    GLCALL(glCopyTexImage2DEXT, target, level, internalformat, x, y, width, height, border);
}

void mglCopyTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyTexSubImage1DEXT, target, level, xoffset, x, y, width);
}

void mglCopyTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage2DEXT, target, level, xoffset, yoffset, x, y, width, height);
}

void mglCopyTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

GLboolean mglAreTexturesResidentEXT (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    return GLCALL(glAreTexturesResidentEXT, n, textures, residences);
}

void mglBindTextureEXT (GLenum target, GLuint texture)
{
    GLCALL(glBindTextureEXT, target, texture);
}

void mglDeleteTexturesEXT (GLsizei n, const GLuint * textures)
{
    GLCALL(glDeleteTexturesEXT, n, textures);
}

void mglGenTexturesEXT (GLsizei n, GLuint * textures)
{
    GLCALL(glGenTexturesEXT, n, textures);
}

GLboolean mglIsTextureEXT (GLuint texture)
{
    return GLCALL(glIsTextureEXT, texture);
}

void mglPrioritizeTexturesEXT (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    GLCALL(glPrioritizeTexturesEXT, n, textures, priorities);
}

void mglArrayElementEXT (GLint i)
{
    GLCALL(glArrayElementEXT, i);
}

void mglColorPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glColorPointerEXT, size, type, stride, count, pointer);
}

void mglDrawArraysEXT (GLenum mode, GLint first, GLsizei count)
{
    GLCALL(glDrawArraysEXT, mode, first, count);
}

void mglEdgeFlagPointerEXT (GLsizei stride, GLsizei count, const GLboolean * pointer)
{
    GLCALL(glEdgeFlagPointerEXT, stride, count, pointer);
}

void mglGetPointervEXT (GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetPointervEXT, pname, params);
}

void mglIndexPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glIndexPointerEXT, type, stride, count, pointer);
}

void mglNormalPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glNormalPointerEXT, type, stride, count, pointer);
}

void mglTexCoordPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glTexCoordPointerEXT, size, type, stride, count, pointer);
}

void mglVertexPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glVertexPointerEXT, size, type, stride, count, pointer);
}

void mglBlendEquationEXT (GLenum mode)
{
    GLCALL(glBlendEquationEXT, mode);
}

void mglPointParameterfEXT (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfEXT, pname, param);
}

void mglPointParameterfvEXT (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvEXT, pname, params);
}

void mglColorTableEXT (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTableEXT, target, internalFormat, width, format, type, table);
}

void mglGetColorTableEXT (GLenum target, GLenum format, GLenum type, GLvoid * data)
{
    GLCALL(glGetColorTableEXT, target, format, type, data);
}

void mglGetColorTableParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameterivEXT, target, pname, params);
}

void mglGetColorTableParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfvEXT, target, pname, params);
}

void mglLockArraysEXT (GLint first, GLsizei count)
{
    GLCALL(glLockArraysEXT, first, count);
}

void mglUnlockArraysEXT ()
{
    GLCALL(glUnlockArraysEXT);
}

void mglDrawRangeElementsEXT (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawRangeElementsEXT, mode, start, end, count, type, indices);
}

void mglSecondaryColor3bEXT (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glSecondaryColor3bEXT, red, green, blue);
}

void mglSecondaryColor3bvEXT (const GLbyte * v)
{
    GLCALL(glSecondaryColor3bvEXT, v);
}

void mglSecondaryColor3dEXT (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glSecondaryColor3dEXT, red, green, blue);
}

void mglSecondaryColor3dvEXT (const GLdouble * v)
{
    GLCALL(glSecondaryColor3dvEXT, v);
}

void mglSecondaryColor3fEXT (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glSecondaryColor3fEXT, red, green, blue);
}

void mglSecondaryColor3fvEXT (const GLfloat * v)
{
    GLCALL(glSecondaryColor3fvEXT, v);
}

void mglSecondaryColor3iEXT (GLint red, GLint green, GLint blue)
{
    GLCALL(glSecondaryColor3iEXT, red, green, blue);
}

void mglSecondaryColor3ivEXT (const GLint * v)
{
    GLCALL(glSecondaryColor3ivEXT, v);
}

void mglSecondaryColor3sEXT (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glSecondaryColor3sEXT, red, green, blue);
}

void mglSecondaryColor3svEXT (const GLshort * v)
{
    GLCALL(glSecondaryColor3svEXT, v);
}

void mglSecondaryColor3ubEXT (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glSecondaryColor3ubEXT, red, green, blue);
}

void mglSecondaryColor3ubvEXT (const GLubyte * v)
{
    GLCALL(glSecondaryColor3ubvEXT, v);
}

void mglSecondaryColor3uiEXT (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glSecondaryColor3uiEXT, red, green, blue);
}

void mglSecondaryColor3uivEXT (const GLuint * v)
{
    GLCALL(glSecondaryColor3uivEXT, v);
}

void mglSecondaryColor3usEXT (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glSecondaryColor3usEXT, red, green, blue);
}

void mglSecondaryColor3usvEXT (const GLushort * v)
{
    GLCALL(glSecondaryColor3usvEXT, v);
}

void mglSecondaryColorPointerEXT (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glSecondaryColorPointerEXT, size, type, stride, pointer);
}

void mglMultiDrawArraysEXT (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    GLCALL(glMultiDrawArraysEXT, mode, first, count, primcount);
}

void mglMultiDrawElementsEXT (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    GLCALL(glMultiDrawElementsEXT, mode, count, type, indices, primcount);
}

void mglFogCoordfEXT (GLfloat coord)
{
    GLCALL(glFogCoordfEXT, coord);
}

void mglFogCoordfvEXT (const GLfloat * coord)
{
    GLCALL(glFogCoordfvEXT, coord);
}

void mglFogCoorddEXT (GLdouble coord)
{
    GLCALL(glFogCoorddEXT, coord);
}

void mglFogCoorddvEXT (const GLdouble * coord)
{
    GLCALL(glFogCoorddvEXT, coord);
}

void mglFogCoordPointerEXT (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glFogCoordPointerEXT, type, stride, pointer);
}

void mglBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparateEXT, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void mglFlushVertexArrayRangeNV ()
{
    GLCALL(glFlushVertexArrayRangeNV);
}

void mglVertexArrayRangeNV (GLsizei length, const GLvoid * pointer)
{
    GLCALL(glVertexArrayRangeNV, length, pointer);
}

void mglCombinerParameterfvNV (GLenum pname, const GLfloat * params)
{
    GLCALL(glCombinerParameterfvNV, pname, params);
}

void mglCombinerParameterfNV (GLenum pname, GLfloat param)
{
    GLCALL(glCombinerParameterfNV, pname, param);
}

void mglCombinerParameterivNV (GLenum pname, const GLint * params)
{
    GLCALL(glCombinerParameterivNV, pname, params);
}

void mglCombinerParameteriNV (GLenum pname, GLint param)
{
    GLCALL(glCombinerParameteriNV, pname, param);
}

void mglCombinerInputNV (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    GLCALL(glCombinerInputNV, stage, portion, variable, input, mapping, componentUsage);
}

void mglCombinerOutputNV (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum)
{
    GLCALL(glCombinerOutputNV, stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum);
}

void mglFinalCombinerInputNV (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    GLCALL(glFinalCombinerInputNV, variable, input, mapping, componentUsage);
}

void mglGetCombinerInputParameterfvNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat * params)
{
    GLCALL(glGetCombinerInputParameterfvNV, stage, portion, variable, pname, params);
}

void mglGetCombinerInputParameterivNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint * params)
{
    GLCALL(glGetCombinerInputParameterivNV, stage, portion, variable, pname, params);
}

void mglGetCombinerOutputParameterfvNV (GLenum stage, GLenum portion, GLenum pname, GLfloat * params)
{
    GLCALL(glGetCombinerOutputParameterfvNV, stage, portion, pname, params);
}

void mglGetCombinerOutputParameterivNV (GLenum stage, GLenum portion, GLenum pname, GLint * params)
{
    GLCALL(glGetCombinerOutputParameterivNV, stage, portion, pname, params);
}

void mglGetFinalCombinerInputParameterfvNV (GLenum variable, GLenum pname, GLfloat * params)
{
    GLCALL(glGetFinalCombinerInputParameterfvNV, variable, pname, params);
}

void mglGetFinalCombinerInputParameterivNV (GLenum variable, GLenum pname, GLint * params)
{
    GLCALL(glGetFinalCombinerInputParameterivNV, variable, pname, params);
}

void mglResizeBuffersMESA ()
{
    GLCALL(glResizeBuffersMESA);
}

void mglWindowPos2dMESA (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2dMESA, x, y);
}

void mglWindowPos2dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos2dvMESA, v);
}

void mglWindowPos2fMESA (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2fMESA, x, y);
}

void mglWindowPos2fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos2fvMESA, v);
}

void mglWindowPos2iMESA (GLint x, GLint y)
{
    GLCALL(glWindowPos2iMESA, x, y);
}

void mglWindowPos2ivMESA (const GLint * v)
{
    GLCALL(glWindowPos2ivMESA, v);
}

void mglWindowPos2sMESA (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2sMESA, x, y);
}

void mglWindowPos2svMESA (const GLshort * v)
{
    GLCALL(glWindowPos2svMESA, v);
}

void mglWindowPos3dMESA (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3dMESA, x, y, z);
}

void mglWindowPos3dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos3dvMESA, v);
}

void mglWindowPos3fMESA (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3fMESA, x, y, z);
}

void mglWindowPos3fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos3fvMESA, v);
}

void mglWindowPos3iMESA (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3iMESA, x, y, z);
}

void mglWindowPos3ivMESA (const GLint * v)
{
    GLCALL(glWindowPos3ivMESA, v);
}

void mglWindowPos3sMESA (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3sMESA, x, y, z);
}

void mglWindowPos3svMESA (const GLshort * v)
{
    GLCALL(glWindowPos3svMESA, v);
}

void mglWindowPos4dMESA (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glWindowPos4dMESA, x, y, z, w);
}

void mglWindowPos4dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos4dvMESA, v);
}

void mglWindowPos4fMESA (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glWindowPos4fMESA, x, y, z, w);
}

void mglWindowPos4fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos4fvMESA, v);
}

void mglWindowPos4iMESA (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glWindowPos4iMESA, x, y, z, w);
}

void mglWindowPos4ivMESA (const GLint * v)
{
    GLCALL(glWindowPos4ivMESA, v);
}

void mglWindowPos4sMESA (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glWindowPos4sMESA, x, y, z, w);
}

void mglWindowPos4svMESA (const GLshort * v)
{
    GLCALL(glWindowPos4svMESA, v);
}

GLboolean mglAreProgramsResidentNV (GLsizei n, const GLuint * programs, GLboolean * residences)
{
    return GLCALL(glAreProgramsResidentNV, n, programs, residences);
}

void mglBindProgramNV (GLenum target, GLuint id)
{
    GLCALL(glBindProgramNV, target, id);
}

void mglDeleteProgramsNV (GLsizei n, const GLuint * programs)
{
    GLCALL(glDeleteProgramsNV, n, programs);
}

void mglExecuteProgramNV (GLenum target, GLuint id, const GLfloat * params)
{
    GLCALL(glExecuteProgramNV, target, id, params);
}

void mglGenProgramsNV (GLsizei n, GLuint * programs)
{
    GLCALL(glGenProgramsNV, n, programs);
}

void mglGetProgramParameterdvNV (GLenum target, GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetProgramParameterdvNV, target, index, pname, params);
}

void mglGetProgramParameterfvNV (GLenum target, GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetProgramParameterfvNV, target, index, pname, params);
}

void mglGetProgramivNV (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramivNV, id, pname, params);
}

void mglGetProgramStringNV (GLuint id, GLenum pname, GLubyte * program)
{
    GLCALL(glGetProgramStringNV, id, pname, program);
}

void mglGetTrackMatrixivNV (GLenum target, GLuint address, GLenum pname, GLint * params)
{
    GLCALL(glGetTrackMatrixivNV, target, address, pname, params);
}

void mglGetVertexAttribdvNV (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdvNV, index, pname, params);
}

void mglGetVertexAttribfvNV (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfvNV, index, pname, params);
}

void mglGetVertexAttribivNV (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribivNV, index, pname, params);
}

void mglGetVertexAttribPointervNV (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointervNV, index, pname, pointer);
}

GLboolean mglIsProgramNV (GLuint id)
{
    return GLCALL(glIsProgramNV, id);
}

void mglLoadProgramNV (GLenum target, GLuint id, GLsizei len, const GLubyte * program)
{
    GLCALL(glLoadProgramNV, target, id, len, program);
}

void mglProgramParameter4dNV (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramParameter4dNV, target, index, x, y, z, w);
}

void mglProgramParameter4dvNV (GLenum target, GLuint index, const GLdouble * v)
{
    GLCALL(glProgramParameter4dvNV, target, index, v);
}

void mglProgramParameter4fNV (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramParameter4fNV, target, index, x, y, z, w);
}

void mglProgramParameter4fvNV (GLenum target, GLuint index, const GLfloat * v)
{
    GLCALL(glProgramParameter4fvNV, target, index, v);
}

void mglProgramParameters4dvNV (GLenum target, GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glProgramParameters4dvNV, target, index, count, v);
}

void mglProgramParameters4fvNV (GLenum target, GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glProgramParameters4fvNV, target, index, count, v);
}

void mglRequestResidentProgramsNV (GLsizei n, const GLuint * programs)
{
    GLCALL(glRequestResidentProgramsNV, n, programs);
}

void mglTrackMatrixNV (GLenum target, GLuint address, GLenum matrix, GLenum transform)
{
    GLCALL(glTrackMatrixNV, target, address, matrix, transform);
}

void mglVertexAttribPointerNV (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointerNV, index, fsize, type, stride, pointer);
}

void mglVertexAttrib1dNV (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1dNV, index, x);
}

void mglVertexAttrib1dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dvNV, index, v);
}

void mglVertexAttrib1fNV (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1fNV, index, x);
}

void mglVertexAttrib1fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fvNV, index, v);
}

void mglVertexAttrib1sNV (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1sNV, index, x);
}

void mglVertexAttrib1svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1svNV, index, v);
}

void mglVertexAttrib2dNV (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2dNV, index, x, y);
}

void mglVertexAttrib2dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dvNV, index, v);
}

void mglVertexAttrib2fNV (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2fNV, index, x, y);
}

void mglVertexAttrib2fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fvNV, index, v);
}

void mglVertexAttrib2sNV (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2sNV, index, x, y);
}

void mglVertexAttrib2svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2svNV, index, v);
}

void mglVertexAttrib3dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3dNV, index, x, y, z);
}

void mglVertexAttrib3dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dvNV, index, v);
}

void mglVertexAttrib3fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3fNV, index, x, y, z);
}

void mglVertexAttrib3fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fvNV, index, v);
}

void mglVertexAttrib3sNV (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3sNV, index, x, y, z);
}

void mglVertexAttrib3svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3svNV, index, v);
}

void mglVertexAttrib4dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4dNV, index, x, y, z, w);
}

void mglVertexAttrib4dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dvNV, index, v);
}

void mglVertexAttrib4fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4fNV, index, x, y, z, w);
}

void mglVertexAttrib4fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fvNV, index, v);
}

void mglVertexAttrib4sNV (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4sNV, index, x, y, z, w);
}

void mglVertexAttrib4svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4svNV, index, v);
}

void mglVertexAttrib4ubNV (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4ubNV, index, x, y, z, w);
}

void mglVertexAttrib4ubvNV (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubvNV, index, v);
}

void mglVertexAttribs1dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs1dvNV, index, count, v);
}

void mglVertexAttribs1fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs1fvNV, index, count, v);
}

void mglVertexAttribs1svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs1svNV, index, count, v);
}

void mglVertexAttribs2dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs2dvNV, index, count, v);
}

void mglVertexAttribs2fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs2fvNV, index, count, v);
}

void mglVertexAttribs2svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs2svNV, index, count, v);
}

void mglVertexAttribs3dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs3dvNV, index, count, v);
}

void mglVertexAttribs3fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs3fvNV, index, count, v);
}

void mglVertexAttribs3svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs3svNV, index, count, v);
}

void mglVertexAttribs4dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs4dvNV, index, count, v);
}

void mglVertexAttribs4fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs4fvNV, index, count, v);
}

void mglVertexAttribs4svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs4svNV, index, count, v);
}

void mglVertexAttribs4ubvNV (GLuint index, GLsizei count, const GLubyte * v)
{
    GLCALL(glVertexAttribs4ubvNV, index, count, v);
}

void mglTexBumpParameterivATI (GLenum pname, const GLint * param)
{
    GLCALL(glTexBumpParameterivATI, pname, param);
}

void mglTexBumpParameterfvATI (GLenum pname, const GLfloat * param)
{
    GLCALL(glTexBumpParameterfvATI, pname, param);
}

void mglGetTexBumpParameterivATI (GLenum pname, GLint * param)
{
    GLCALL(glGetTexBumpParameterivATI, pname, param);
}

void mglGetTexBumpParameterfvATI (GLenum pname, GLfloat * param)
{
    GLCALL(glGetTexBumpParameterfvATI, pname, param);
}

GLuint mglGenFragmentShadersATI (GLuint range)
{
    return GLCALL(glGenFragmentShadersATI, range);
}

void mglBindFragmentShaderATI (GLuint id)
{
    GLCALL(glBindFragmentShaderATI, id);
}

void mglDeleteFragmentShaderATI (GLuint id)
{
    GLCALL(glDeleteFragmentShaderATI, id);
}

void mglBeginFragmentShaderATI ()
{
    GLCALL(glBeginFragmentShaderATI);
}

void mglEndFragmentShaderATI ()
{
    GLCALL(glEndFragmentShaderATI);
}

void mglPassTexCoordATI (GLuint dst, GLuint coord, GLenum swizzle)
{
    GLCALL(glPassTexCoordATI, dst, coord, swizzle);
}

void mglSampleMapATI (GLuint dst, GLuint interp, GLenum swizzle)
{
    GLCALL(glSampleMapATI, dst, interp, swizzle);
}

void mglColorFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    GLCALL(glColorFragmentOp1ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod);
}

void mglColorFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    GLCALL(glColorFragmentOp2ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
}

void mglColorFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    GLCALL(glColorFragmentOp3ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
}

void mglAlphaFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    GLCALL(glAlphaFragmentOp1ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod);
}

void mglAlphaFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    GLCALL(glAlphaFragmentOp2ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
}

void mglAlphaFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    GLCALL(glAlphaFragmentOp3ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
}

void mglSetFragmentShaderConstantATI (GLuint dst, const GLfloat * value)
{
    GLCALL(glSetFragmentShaderConstantATI, dst, value);
}

void mglPointParameteriNV (GLenum pname, GLint param)
{
    GLCALL(glPointParameteriNV, pname, param);
}

void mglPointParameterivNV (GLenum pname, const GLint * params)
{
    GLCALL(glPointParameterivNV, pname, params);
}

void mglDrawBuffersATI (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffersATI, n, bufs);
}

void mglProgramNamedParameter4fNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramNamedParameter4fNV, id, len, name, x, y, z, w);
}

void mglProgramNamedParameter4dNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramNamedParameter4dNV, id, len, name, x, y, z, w);
}

void mglProgramNamedParameter4fvNV (GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v)
{
    GLCALL(glProgramNamedParameter4fvNV, id, len, name, v);
}

void mglProgramNamedParameter4dvNV (GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v)
{
    GLCALL(glProgramNamedParameter4dvNV, id, len, name, v);
}

void mglGetProgramNamedParameterfvNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat * params)
{
    GLCALL(glGetProgramNamedParameterfvNV, id, len, name, params);
}

void mglGetProgramNamedParameterdvNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble * params)
{
    GLCALL(glGetProgramNamedParameterdvNV, id, len, name, params);
}

GLboolean mglIsRenderbufferEXT (GLuint renderbuffer)
{
    return GLCALL(glIsRenderbufferEXT, renderbuffer);
}

void mglBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
    GLCALL(glBindRenderbufferEXT, target, renderbuffer);
}

void mglDeleteRenderbuffersEXT (GLsizei n, const GLuint * renderbuffers)
{
    GLCALL(glDeleteRenderbuffersEXT, n, renderbuffers);
}

void mglGenRenderbuffersEXT (GLsizei n, GLuint * renderbuffers)
{
    GLCALL(glGenRenderbuffersEXT, n, renderbuffers);
}

void mglRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageEXT, target, internalformat, width, height);
}

void mglGetRenderbufferParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetRenderbufferParameterivEXT, target, pname, params);
}

GLboolean mglIsFramebufferEXT (GLuint framebuffer)
{
    return GLCALL(glIsFramebufferEXT, framebuffer);
}

void mglBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
    GLCALL(glBindFramebufferEXT, target, framebuffer);
}

void mglDeleteFramebuffersEXT (GLsizei n, const GLuint * framebuffers)
{
    GLCALL(glDeleteFramebuffersEXT, n, framebuffers);
}

void mglGenFramebuffersEXT (GLsizei n, GLuint * framebuffers)
{
    GLCALL(glGenFramebuffersEXT, n, framebuffers);
}

GLenum mglCheckFramebufferStatusEXT (GLenum target)
{
    return GLCALL(glCheckFramebufferStatusEXT, target);
}

void mglFramebufferTexture1DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture1DEXT, target, attachment, textarget, texture, level);
}

void mglFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture2DEXT, target, attachment, textarget, texture, level);
}

void mglFramebufferTexture3DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    GLCALL(glFramebufferTexture3DEXT, target, attachment, textarget, texture, level, zoffset);
}

void mglFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    GLCALL(glFramebufferRenderbufferEXT, target, attachment, renderbuffertarget, renderbuffer);
}

void mglGetFramebufferAttachmentParameterivEXT (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    GLCALL(glGetFramebufferAttachmentParameterivEXT, target, attachment, pname, params);
}

void mglGenerateMipmapEXT (GLenum target)
{
    GLCALL(glGenerateMipmapEXT, target);
}

void mglFramebufferTextureLayerEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GLCALL(glFramebufferTextureLayerEXT, target, attachment, texture, level, layer);
}

GLvoid* mglMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    return GLCALL(glMapBufferRange, target, offset, length, access);
}

void mglFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length)
{
    GLCALL(glFlushMappedBufferRange, target, offset, length);
}

void mglBindVertexArray (GLuint array)
{
    GLCALL(glBindVertexArray, array);
}

void mglDeleteVertexArrays (GLsizei n, const GLuint * arrays)
{
    GLCALL(glDeleteVertexArrays, n, arrays);
}

void mglGenVertexArrays (GLsizei n, GLuint * arrays)
{
    GLCALL(glGenVertexArrays, n, arrays);
}

GLboolean mglIsVertexArray (GLuint array)
{
    return GLCALL(glIsVertexArray, array);
}

void mglCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    GLCALL(glCopyBufferSubData, readTarget, writeTarget, readOffset, writeOffset, size);
}

GLsync mglFenceSync (GLenum condition, GLbitfield flags)
{
    return GLCALL(glFenceSync, condition, flags);
}

GLboolean mglIsSync (GLsync sync)
{
    return GLCALL(glIsSync, sync);
}

void mglDeleteSync (GLsync sync)
{
    GLCALL(glDeleteSync, sync);
}

GLenum mglClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    return GLCALL(glClientWaitSync, sync, flags, timeout);
}

void mglWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GLCALL(glWaitSync, sync, flags, timeout);
}

void mglGetInteger64v (GLenum pname, GLint64 * params)
{
    GLCALL(glGetInteger64v, pname, params);
}

void mglGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values)
{
    GLCALL(glGetSynciv, sync, pname, bufSize, length, values);
}

void mglProvokingVertexEXT (GLenum mode)
{
    GLCALL(glProvokingVertexEXT, mode);
}

void mglDrawElementsBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    GLCALL(glDrawElementsBaseVertex, mode, count, type, indices, basevertex);
}

void mglDrawRangeElementsBaseVertex (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    GLCALL(glDrawRangeElementsBaseVertex, mode, start, end, count, type, indices, basevertex);
}

void mglMultiDrawElementsBaseVertex (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount, const GLint * basevertex)
{
    GLCALL(glMultiDrawElementsBaseVertex, mode, count, type, indices, primcount, basevertex);
}

void mglProvokingVertex (GLenum mode)
{
    GLCALL(glProvokingVertex, mode);
}

void mglRenderbufferStorageMultisampleEXT (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageMultisampleEXT, target, samples, internalformat, width, height);
}

void mglColorMaskIndexedEXT (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    GLCALL(glColorMaskIndexedEXT, index, r, g, b, a);
}

void mglGetBooleanIndexedvEXT (GLenum target, GLuint index, GLboolean * data)
{
    GLCALL(glGetBooleanIndexedvEXT, target, index, data);
}

void mglGetIntegerIndexedvEXT (GLenum target, GLuint index, GLint * data)
{
    GLCALL(glGetIntegerIndexedvEXT, target, index, data);
}

void mglEnableIndexedEXT (GLenum target, GLuint index)
{
    GLCALL(glEnableIndexedEXT, target, index);
}

void mglDisableIndexedEXT (GLenum target, GLuint index)
{
    GLCALL(glDisableIndexedEXT, target, index);
}

GLboolean mglIsEnabledIndexedEXT (GLenum target, GLuint index)
{
    return GLCALL(glIsEnabledIndexedEXT, target, index);
}

void mglBeginConditionalRenderNV (GLuint id, GLenum mode)
{
    GLCALL(glBeginConditionalRenderNV, id, mode);
}

void mglEndConditionalRenderNV ()
{
    GLCALL(glEndConditionalRenderNV);
}

GLenum mglObjectPurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    return GLCALL(glObjectPurgeableAPPLE, objectType, name, option);
}

GLenum mglObjectUnpurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    return GLCALL(glObjectUnpurgeableAPPLE, objectType, name, option);
}

void mglGetObjectParameterivAPPLE (GLenum objectType, GLuint name, GLenum pname, GLint * params)
{
    GLCALL(glGetObjectParameterivAPPLE, objectType, name, pname, params);
}

void mglBeginTransformFeedback (GLenum primitiveMode)
{
    GLCALL(glBeginTransformFeedback, primitiveMode);
}

void mglEndTransformFeedback ()
{
    GLCALL(glEndTransformFeedback);
}

void mglBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glBindBufferRange, target, index, buffer, offset, size);
}

void mglBindBufferBase (GLenum target, GLuint index, GLuint buffer)
{
    GLCALL(glBindBufferBase, target, index, buffer);
}

void mglTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    GLCALL(glTransformFeedbackVaryings, program, count, varyings, bufferMode);
}

void mglGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetTransformFeedbackVarying, program, index, bufSize, length, size, type, name);
}

void mglDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstanced, mode, first, count, primcount);
}

void mglDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstanced, mode, count, type, indices, primcount);
}

void mglDrawArraysInstancedARB (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstancedARB, mode, first, count, primcount);
}

void mglDrawElementsInstancedARB (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstancedARB, mode, count, type, indices, primcount);
}

void mglProgramParameteriARB (GLuint program, GLenum pname, GLint value)
{
    GLCALL(glProgramParameteriARB, program, pname, value);
}

void mglFramebufferTextureARB (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTextureARB, target, attachment, texture, level);
}

void mglFramebufferTextureFaceARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face)
{
    GLCALL(glFramebufferTextureFaceARB, target, attachment, texture, level, face);
}

void mglBindTransformFeedback (GLenum target, GLuint id)
{
    GLCALL(glBindTransformFeedback, target, id);
}

void mglDeleteTransformFeedbacks (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteTransformFeedbacks, n, ids);
}

void mglGenTransformFeedbacks (GLsizei n, GLuint * ids)
{
    GLCALL(glGenTransformFeedbacks, n, ids);
}

GLboolean mglIsTransformFeedback (GLuint id)
{
    return GLCALL(glIsTransformFeedback, id);
}

void mglPauseTransformFeedback ()
{
    GLCALL(glPauseTransformFeedback);
}

void mglResumeTransformFeedback ()
{
    GLCALL(glResumeTransformFeedback);
}

void mglDrawTransformFeedback (GLenum mode, GLuint id)
{
    GLCALL(glDrawTransformFeedback, mode, id);
}

void mglDrawArraysInstancedEXT (GLenum mode, GLint start, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstancedEXT, mode, start, count, primcount);
}

void mglDrawElementsInstancedEXT (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstancedEXT, mode, count, type, indices, primcount);
}

void mglBeginTransformFeedbackEXT (GLenum primitiveMode)
{
    GLCALL(glBeginTransformFeedbackEXT, primitiveMode);
}

void mglEndTransformFeedbackEXT ()
{
    GLCALL(glEndTransformFeedbackEXT);
}

void mglBindBufferRangeEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glBindBufferRangeEXT, target, index, buffer, offset, size);
}

void mglBindBufferOffsetEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset)
{
    GLCALL(glBindBufferOffsetEXT, target, index, buffer, offset);
}

void mglBindBufferBaseEXT (GLenum target, GLuint index, GLuint buffer)
{
    GLCALL(glBindBufferBaseEXT, target, index, buffer);
}

void mglTransformFeedbackVaryingsEXT (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    GLCALL(glTransformFeedbackVaryingsEXT, program, count, varyings, bufferMode);
}

void mglGetTransformFeedbackVaryingEXT (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetTransformFeedbackVaryingEXT, program, index, bufSize, length, size, type, name);
}

void mglEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image)
{
    GLCALL(glEGLImageTargetTexture2DOES, target, image);
}

void mglEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image)
{
    GLCALL(glEGLImageTargetRenderbufferStorageOES, target, image);
}

void mglColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    GLCALL(glColorMaski, index, r, g, b, a);
}

void mglGetBooleani_v (GLenum target, GLuint index, GLboolean * data)
{
    GLCALL(glGetBooleani_v, target, index, data);
}

void mglGetIntegeri_v (GLenum target, GLuint index, GLint * data)
{
    GLCALL(glGetIntegeri_v, target, index, data);
}

void mglEnablei (GLenum target, GLuint index)
{
    GLCALL(glEnablei, target, index);
}

void mglDisablei (GLenum target, GLuint index)
{
    GLCALL(glDisablei, target, index);
}

GLboolean mglIsEnabledi (GLenum target, GLuint index)
{
    return GLCALL(glIsEnabledi, target, index);
}

void mglClampColor (GLenum target, GLenum clamp)
{
    GLCALL(glClampColor, target, clamp);
}

void mglBeginConditionalRender (GLuint id, GLenum mode)
{
    GLCALL(glBeginConditionalRender, id, mode);
}

void mglEndConditionalRender ()
{
    GLCALL(glEndConditionalRender);
}

void mglVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribIPointer, index, size, type, stride, pointer);
}

void mglGetVertexAttribIiv (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribIiv, index, pname, params);
}

void mglGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint * params)
{
    GLCALL(glGetVertexAttribIuiv, index, pname, params);
}

void mglVertexAttribI1i (GLuint index, GLint x)
{
    GLCALL(glVertexAttribI1i, index, x);
}

void mglVertexAttribI2i (GLuint index, GLint x, GLint y)
{
    GLCALL(glVertexAttribI2i, index, x, y);
}

void mglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z)
{
    GLCALL(glVertexAttribI3i, index, x, y, z);
}

void mglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertexAttribI4i, index, x, y, z, w);
}

void mglVertexAttribI1ui (GLuint index, GLuint x)
{
    GLCALL(glVertexAttribI1ui, index, x);
}

void mglVertexAttribI2ui (GLuint index, GLuint x, GLuint y)
{
    GLCALL(glVertexAttribI2ui, index, x, y);
}

void mglVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z)
{
    GLCALL(glVertexAttribI3ui, index, x, y, z);
}

void mglVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    GLCALL(glVertexAttribI4ui, index, x, y, z, w);
}

void mglVertexAttribI1iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI1iv, index, v);
}

void mglVertexAttribI2iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI2iv, index, v);
}

void mglVertexAttribI3iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI3iv, index, v);
}

void mglVertexAttribI4iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI4iv, index, v);
}

void mglVertexAttribI1uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI1uiv, index, v);
}

void mglVertexAttribI2uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI2uiv, index, v);
}

void mglVertexAttribI3uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI3uiv, index, v);
}

void mglVertexAttribI4uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI4uiv, index, v);
}

void mglVertexAttribI4bv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttribI4bv, index, v);
}

void mglVertexAttribI4sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttribI4sv, index, v);
}

void mglVertexAttribI4ubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttribI4ubv, index, v);
}

void mglVertexAttribI4usv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttribI4usv, index, v);
}

void mglGetUniformuiv (GLuint program, GLint location, GLuint * params)
{
    GLCALL(glGetUniformuiv, program, location, params);
}

void mglBindFragDataLocation (GLuint program, GLuint color, const GLchar * name)
{
    GLCALL(glBindFragDataLocation, program, color, name);
}

GLint mglGetFragDataLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetFragDataLocation, program, name);
}

void mglUniform1ui (GLint location, GLuint v0)
{
    GLCALL(glUniform1ui, location, v0);
}

void mglUniform2ui (GLint location, GLuint v0, GLuint v1)
{
    GLCALL(glUniform2ui, location, v0, v1);
}

void mglUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GLCALL(glUniform3ui, location, v0, v1, v2);
}

void mglUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GLCALL(glUniform4ui, location, v0, v1, v2, v3);
}

void mglUniform1uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform1uiv, location, count, value);
}

void mglUniform2uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform2uiv, location, count, value);
}

void mglUniform3uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform3uiv, location, count, value);
}

void mglUniform4uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform4uiv, location, count, value);
}

void mglTexParameterIiv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameterIiv, target, pname, params);
}

void mglTexParameterIuiv (GLenum target, GLenum pname, const GLuint * params)
{
    GLCALL(glTexParameterIuiv, target, pname, params);
}

void mglGetTexParameterIiv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameterIiv, target, pname, params);
}

void mglGetTexParameterIuiv (GLenum target, GLenum pname, GLuint * params)
{
    GLCALL(glGetTexParameterIuiv, target, pname, params);
}

void mglClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint * value)
{
    GLCALL(glClearBufferiv, buffer, drawbuffer, value);
}

void mglClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint * value)
{
    GLCALL(glClearBufferuiv, buffer, drawbuffer, value);
}

void mglClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat * value)
{
    GLCALL(glClearBufferfv, buffer, drawbuffer, value);
}

void mglClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    GLCALL(glClearBufferfi, buffer, drawbuffer, depth, stencil);
}

const GLubyte * mglGetStringi (GLenum name, GLuint index)
{
    return GLCALL(glGetStringi, name, index);
}

void mglTexBuffer (GLenum target, GLenum internalformat, GLuint buffer)
{
    GLCALL(glTexBuffer, target, internalformat, buffer);
}

void mglPrimitiveRestartIndex (GLuint index)
{
    GLCALL(glPrimitiveRestartIndex, index);
}

void mglGetInteger64i_v (GLenum target, GLuint index, GLint64 * data)
{
    GLCALL(glGetInteger64i_v, target, index, data);
}

void mglGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 * params)
{
    GLCALL(glGetBufferParameteri64v, target, pname, params);
}

void mglFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture, target, attachment, texture, level);
}

void mglVertexAttribDivisor (GLuint index, GLuint divisor)
{
    GLCALL(glVertexAttribDivisor, index, divisor);
}

void mglPrimitiveRestartNV ()
{
    GLCALL(glPrimitiveRestartNV);
}

void mglPrimitiveRestartIndexNV (GLuint index)
{
    GLCALL(glPrimitiveRestartIndexNV, index);
}

void mglVertexAttribI1iEXT (GLuint index, GLint x)
{
    GLCALL(glVertexAttribI1iEXT, index, x);
}

void mglVertexAttribI2iEXT (GLuint index, GLint x, GLint y)
{
    GLCALL(glVertexAttribI2iEXT, index, x, y);
}

void mglVertexAttribI3iEXT (GLuint index, GLint x, GLint y, GLint z)
{
    GLCALL(glVertexAttribI3iEXT, index, x, y, z);
}

void mglVertexAttribI4iEXT (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertexAttribI4iEXT, index, x, y, z, w);
}

void mglVertexAttribI1uiEXT (GLuint index, GLuint x)
{
    GLCALL(glVertexAttribI1uiEXT, index, x);
}

void mglVertexAttribI2uiEXT (GLuint index, GLuint x, GLuint y)
{
    GLCALL(glVertexAttribI2uiEXT, index, x, y);
}

void mglVertexAttribI3uiEXT (GLuint index, GLuint x, GLuint y, GLuint z)
{
    GLCALL(glVertexAttribI3uiEXT, index, x, y, z);
}

void mglVertexAttribI4uiEXT (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    GLCALL(glVertexAttribI4uiEXT, index, x, y, z, w);
}

void mglVertexAttribI1ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI1ivEXT, index, v);
}

void mglVertexAttribI2ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI2ivEXT, index, v);
}

void mglVertexAttribI3ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI3ivEXT, index, v);
}

void mglVertexAttribI4ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI4ivEXT, index, v);
}

void mglVertexAttribI1uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI1uivEXT, index, v);
}

void mglVertexAttribI2uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI2uivEXT, index, v);
}

void mglVertexAttribI3uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI3uivEXT, index, v);
}

void mglVertexAttribI4uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI4uivEXT, index, v);
}

void mglVertexAttribI4bvEXT (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttribI4bvEXT, index, v);
}

void mglVertexAttribI4svEXT (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttribI4svEXT, index, v);
}

void mglVertexAttribI4ubvEXT (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttribI4ubvEXT, index, v);
}

void mglVertexAttribI4usvEXT (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttribI4usvEXT, index, v);
}

void mglVertexAttribIPointerEXT (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribIPointerEXT, index, size, type, stride, pointer);
}

void mglGetVertexAttribIivEXT (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribIivEXT, index, pname, params);
}

void mglGetVertexAttribIuivEXT (GLuint index, GLenum pname, GLuint * params)
{
    GLCALL(glGetVertexAttribIuivEXT, index, pname, params);
}

void mglGetUniformuivEXT (GLuint program, GLint location, GLuint * params)
{
    GLCALL(glGetUniformuivEXT, program, location, params);
}

void mglBindFragDataLocationEXT (GLuint program, GLuint color, const GLchar * name)
{
    GLCALL(glBindFragDataLocationEXT, program, color, name);
}

GLint mglGetFragDataLocationEXT (GLuint program, const GLchar * name)
{
    return GLCALL(glGetFragDataLocationEXT, program, name);
}

void mglUniform1uiEXT (GLint location, GLuint v0)
{
    GLCALL(glUniform1uiEXT, location, v0);
}

void mglUniform2uiEXT (GLint location, GLuint v0, GLuint v1)
{
    GLCALL(glUniform2uiEXT, location, v0, v1);
}

void mglUniform3uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GLCALL(glUniform3uiEXT, location, v0, v1, v2);
}

void mglUniform4uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GLCALL(glUniform4uiEXT, location, v0, v1, v2, v3);
}

void mglUniform1uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform1uivEXT, location, count, value);
}

void mglUniform2uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform2uivEXT, location, count, value);
}

void mglUniform3uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform3uivEXT, location, count, value);
}

void mglUniform4uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform4uivEXT, location, count, value);
}

void mglTexParameterIivEXT (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameterIivEXT, target, pname, params);
}

void mglTexParameterIuivEXT (GLenum target, GLenum pname, const GLuint * params)
{
    GLCALL(glTexParameterIuivEXT, target, pname, params);
}

void mglGetTexParameterIivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameterIivEXT, target, pname, params);
}

void mglGetTexParameterIuivEXT (GLenum target, GLenum pname, GLuint * params)
{
    GLCALL(glGetTexParameterIuivEXT, target, pname, params);
}

void mglClearColorIiEXT (GLint red, GLint green, GLint blue, GLint alpha)
{
    GLCALL(glClearColorIiEXT, red, green, blue, alpha);
}

void mglClearColorIuiEXT (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    GLCALL(glClearColorIuiEXT, red, green, blue, alpha);
}

void mglUseShaderProgramEXT (GLenum type, GLuint program)
{
    GLCALL(glUseShaderProgramEXT, type, program);
}

void mglActiveProgramEXT (GLuint program)
{
    GLCALL(glActiveProgramEXT, program);
}

GLuint mglCreateShaderProgramEXT (GLenum type, const GLchar * string)
{
    return GLCALL(glCreateShaderProgramEXT, type, string);
}

void mglProgramEnvParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    GLCALL(glProgramEnvParameters4fvEXT, target, index, count, params);
}

void mglProgramLocalParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    GLCALL(glProgramLocalParameters4fvEXT, target, index, count, params);
}

void mglBlendEquationSeparateATI (GLenum modeRGB, GLenum modeA)
{
    GLCALL(glBlendEquationSeparateATI, modeRGB, modeA);
}

void mglGetHistogramEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetHistogramEXT, target, reset, format, type, values);
}

void mglGetHistogramParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetHistogramParameterfvEXT, target, pname, params);
}

void mglGetHistogramParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetHistogramParameterivEXT, target, pname, params);
}

void mglGetMinmaxEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetMinmaxEXT, target, reset, format, type, values);
}

void mglGetMinmaxParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMinmaxParameterfvEXT, target, pname, params);
}

void mglGetMinmaxParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetMinmaxParameterivEXT, target, pname, params);
}

void mglHistogramEXT (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    GLCALL(glHistogramEXT, target, width, internalformat, sink);
}

void mglMinmaxEXT (GLenum target, GLenum internalformat, GLboolean sink)
{
    GLCALL(glMinmaxEXT, target, internalformat, sink);
}

void mglResetHistogramEXT (GLenum target)
{
    GLCALL(glResetHistogramEXT, target);
}

void mglResetMinmaxEXT (GLenum target)
{
    GLCALL(glResetMinmaxEXT, target);
}

void mglConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter1DEXT, target, internalformat, width, format, type, image);
}

void mglConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter2DEXT, target, internalformat, width, height, format, type, image);
}

void mglConvolutionParameterfEXT (GLenum target, GLenum pname, GLfloat params)
{
    GLCALL(glConvolutionParameterfEXT, target, pname, params);
}

void mglConvolutionParameterfvEXT (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glConvolutionParameterfvEXT, target, pname, params);
}

void mglConvolutionParameteriEXT (GLenum target, GLenum pname, GLint params)
{
    GLCALL(glConvolutionParameteriEXT, target, pname, params);
}

void mglConvolutionParameterivEXT (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glConvolutionParameterivEXT, target, pname, params);
}

void mglCopyConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyConvolutionFilter1DEXT, target, internalformat, x, y, width);
}

void mglCopyConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyConvolutionFilter2DEXT, target, internalformat, x, y, width, height);
}

void mglGetConvolutionFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    GLCALL(glGetConvolutionFilterEXT, target, format, type, image);
}

void mglGetConvolutionParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetConvolutionParameterfvEXT, target, pname, params);
}

void mglGetConvolutionParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetConvolutionParameterivEXT, target, pname, params);
}

void mglGetSeparableFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    GLCALL(glGetSeparableFilterEXT, target, format, type, row, column, span);
}

void mglSeparableFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    GLCALL(glSeparableFilter2DEXT, target, internalformat, width, height, format, type, row, column);
}

void mglColorTableSGI (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTableSGI, target, internalformat, width, format, type, table);
}

void mglColorTableParameterfvSGI (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glColorTableParameterfvSGI, target, pname, params);
}

void mglColorTableParameterivSGI (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glColorTableParameterivSGI, target, pname, params);
}

void mglCopyColorTableSGI (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorTableSGI, target, internalformat, x, y, width);
}

void mglGetColorTableSGI (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    GLCALL(glGetColorTableSGI, target, format, type, table);
}

void mglGetColorTableParameterfvSGI (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfvSGI, target, pname, params);
}

void mglGetColorTableParameterivSGI (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameterivSGI, target, pname, params);
}

void mglPixelTexGenSGIX (GLenum mode)
{
    GLCALL(glPixelTexGenSGIX, mode);
}

void mglPixelTexGenParameteriSGIS (GLenum pname, GLint param)
{
    GLCALL(glPixelTexGenParameteriSGIS, pname, param);
}

void mglPixelTexGenParameterivSGIS (GLenum pname, const GLint * params)
{
    GLCALL(glPixelTexGenParameterivSGIS, pname, params);
}

void mglPixelTexGenParameterfSGIS (GLenum pname, GLfloat param)
{
    GLCALL(glPixelTexGenParameterfSGIS, pname, param);
}

void mglPixelTexGenParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    GLCALL(glPixelTexGenParameterfvSGIS, pname, params);
}

void mglGetPixelTexGenParameterivSGIS (GLenum pname, GLint * params)
{
    GLCALL(glGetPixelTexGenParameterivSGIS, pname, params);
}

void mglGetPixelTexGenParameterfvSGIS (GLenum pname, GLfloat * params)
{
    GLCALL(glGetPixelTexGenParameterfvSGIS, pname, params);
}

void mglSampleMaskSGIS (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleMaskSGIS, value, invert);
}

void mglSamplePatternSGIS (GLenum pattern)
{
    GLCALL(glSamplePatternSGIS, pattern);
}

void mglPointParameterfSGIS (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfSGIS, pname, param);
}

void mglPointParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvSGIS, pname, params);
}

void mglColorSubTableEXT (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    GLCALL(glColorSubTableEXT, target, start, count, format, type, data);
}

void mglCopyColorSubTableEXT (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorSubTableEXT, target, start, x, y, width);
}

void mglBlendFuncSeparateINGR (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparateINGR, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void mglMultiModeDrawArraysIBM (const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride)
{
    GLCALL(glMultiModeDrawArraysIBM, mode, first, count, primcount, modestride);
}

void mglMultiModeDrawElementsIBM (const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride)
{
    GLCALL(glMultiModeDrawElementsIBM, mode, count, type, indices, primcount, modestride);
}

void mglSampleMaskEXT (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleMaskEXT, value, invert);
}

void mglSamplePatternEXT (GLenum pattern)
{
    GLCALL(glSamplePatternEXT, pattern);
}

void mglDeleteFencesNV (GLsizei n, const GLuint * fences)
{
    GLCALL(glDeleteFencesNV, n, fences);
}

void mglGenFencesNV (GLsizei n, GLuint * fences)
{
    GLCALL(glGenFencesNV, n, fences);
}

GLboolean mglIsFenceNV (GLuint fence)
{
    return GLCALL(glIsFenceNV, fence);
}

GLboolean mglTestFenceNV (GLuint fence)
{
    return GLCALL(glTestFenceNV, fence);
}

void mglGetFenceivNV (GLuint fence, GLenum pname, GLint * params)
{
    GLCALL(glGetFenceivNV, fence, pname, params);
}

void mglFinishFenceNV (GLuint fence)
{
    GLCALL(glFinishFenceNV, fence);
}

void mglSetFenceNV (GLuint fence, GLenum condition)
{
    GLCALL(glSetFenceNV, fence, condition);
}

void mglActiveStencilFaceEXT (GLenum face)
{
    GLCALL(glActiveStencilFaceEXT, face);
}

void mglBindVertexArrayAPPLE (GLuint array)
{
    GLCALL(glBindVertexArrayAPPLE, array);
}

void mglDeleteVertexArraysAPPLE (GLsizei n, const GLuint * arrays)
{
    GLCALL(glDeleteVertexArraysAPPLE, n, arrays);
}

void mglGenVertexArraysAPPLE (GLsizei n, GLuint * arrays)
{
    GLCALL(glGenVertexArraysAPPLE, n, arrays);
}

GLboolean mglIsVertexArrayAPPLE (GLuint array)
{
    return GLCALL(glIsVertexArrayAPPLE, array);
}

void mglStencilOpSeparateATI (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    GLCALL(glStencilOpSeparateATI, face, sfail, dpfail, dppass);
}

void mglStencilFuncSeparateATI (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
    GLCALL(glStencilFuncSeparateATI, frontfunc, backfunc, ref, mask);
}

void mglDepthBoundsEXT (GLclampd zmin, GLclampd zmax)
{
    GLCALL(glDepthBoundsEXT, zmin, zmax);
}

void mglBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparateEXT, modeRGB, modeAlpha);
}

void mglBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    GLCALL(glBlitFramebufferEXT, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void mglGetQueryObjecti64vEXT (GLuint id, GLenum pname, GLint64EXT * params)
{
    GLCALL(glGetQueryObjecti64vEXT, id, pname, params);
}

void mglGetQueryObjectui64vEXT (GLuint id, GLenum pname, GLuint64EXT * params)
{
    GLCALL(glGetQueryObjectui64vEXT, id, pname, params);
}

void mglBufferParameteriAPPLE (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glBufferParameteriAPPLE, target, pname, param);
}

void mglFlushMappedBufferRangeAPPLE (GLenum target, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glFlushMappedBufferRangeAPPLE, target, offset, size);
}

void mglTextureRangeAPPLE (GLenum target, GLsizei length, const GLvoid * pointer)
{
    GLCALL(glTextureRangeAPPLE, target, length, pointer);
}

void mglGetTexParameterPointervAPPLE (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetTexParameterPointervAPPLE, target, pname, params);
}

