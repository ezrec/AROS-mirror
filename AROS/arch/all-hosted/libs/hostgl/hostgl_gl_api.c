/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "glx_hostlib.h"
#include "arosmesaapim.h"

inline void mglClearIndex (GLfloat c)
{
    GLCALL(glClearIndex, c);
}

inline void mglClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glClearColor, red, green, blue, alpha);
}

inline void mglClear (GLbitfield mask)
{
    GLCALL(glClear, mask);
}

inline void mglIndexMask (GLuint mask)
{
    GLCALL(glIndexMask, mask);
}

inline void mglColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    GLCALL(glColorMask, red, green, blue, alpha);
}

inline void mglAlphaFunc (GLenum func, GLclampf ref)
{
    GLCALL(glAlphaFunc, func, ref);
}

inline void mglBlendFunc (GLenum sfactor, GLenum dfactor)
{
    GLCALL(glBlendFunc, sfactor, dfactor);
}

inline void mglLogicOp (GLenum opcode)
{
    GLCALL(glLogicOp, opcode);
}

inline void mglCullFace (GLenum mode)
{
    GLCALL(glCullFace, mode);
}

inline void mglFrontFace (GLenum mode)
{
    GLCALL(glFrontFace, mode);
}

inline void mglPointSize (GLfloat size)
{
    GLCALL(glPointSize, size);
}

inline void mglLineWidth (GLfloat width)
{
    GLCALL(glLineWidth, width);
}

inline void mglLineStipple (GLint factor, GLushort pattern)
{
    GLCALL(glLineStipple, factor, pattern);
}

inline void mglPolygonMode (GLenum face, GLenum mode)
{
    GLCALL(glPolygonMode, face, mode);
}

inline void mglPolygonOffset (GLfloat factor, GLfloat units)
{
    GLCALL(glPolygonOffset, factor, units);
}

inline void mglPolygonStipple (const GLubyte * mask)
{
    GLCALL(glPolygonStipple, mask);
}

inline void mglGetPolygonStipple (GLubyte * mask)
{
    GLCALL(glGetPolygonStipple, mask);
}

inline void mglEdgeFlag (GLboolean flag)
{
    GLCALL(glEdgeFlag, flag);
}

inline void mglEdgeFlagv (const GLboolean * flag)
{
    GLCALL(glEdgeFlagv, flag);
}

inline void mglScissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glScissor, x, y, width, height);
}

inline void mglClipPlane (GLenum plane, const GLdouble * equation)
{
    GLCALL(glClipPlane, plane, equation);
}

inline void mglGetClipPlane (GLenum plane, GLdouble * equation)
{
    GLCALL(glGetClipPlane, plane, equation);
}

inline void mglDrawBuffer (GLenum mode)
{
    GLCALL(glDrawBuffer, mode);
}

inline void mglReadBuffer (GLenum mode)
{
    GLCALL(glReadBuffer, mode);
}

inline void mglEnable (GLenum cap)
{
    GLCALL(glEnable, cap);
}

inline void mglDisable (GLenum cap)
{
    GLCALL(glDisable, cap);
}

inline GLboolean mglIsEnabled (GLenum cap)
{
    return GLCALL(glIsEnabled, cap);
}

inline void mglEnableClientState (GLenum cap)
{
    GLCALL(glEnableClientState, cap);
}

inline void mglDisableClientState (GLenum cap)
{
    GLCALL(glDisableClientState, cap);
}

inline void mglGetBooleanv (GLenum pname, GLboolean * params)
{
    GLCALL(glGetBooleanv, pname, params);
}

inline void mglGetDoublev (GLenum pname, GLdouble * params)
{
    GLCALL(glGetDoublev, pname, params);
}

inline void mglGetFloatv (GLenum pname, GLfloat * params)
{
    GLCALL(glGetFloatv, pname, params);
}

inline void mglGetIntegerv (GLenum pname, GLint * params)
{
    GLCALL(glGetIntegerv, pname, params);
}

inline void mglPushAttrib (GLbitfield mask)
{
    GLCALL(glPushAttrib, mask);
}

inline void mglPopAttrib ()
{
    GLCALL(glPopAttrib);
}

inline void mglPushClientAttrib (GLbitfield mask)
{
    GLCALL(glPushClientAttrib, mask);
}

inline void mglPopClientAttrib ()
{
    GLCALL(glPopClientAttrib);
}

inline GLint mglRenderMode (GLenum mode)
{
    return GLCALL(glRenderMode, mode);
}

inline GLenum mglGetError ()
{
    return GLCALL(glGetError);
}

inline const GLubyte * mglGetString (GLenum name)
{
    return GLCALL(glGetString, name);
}

inline void mglFinish ()
{
    GLCALL(glFinish);
}

inline void mglFlush ()
{
    GLCALL(glFlush);
}

inline void mglHint (GLenum target, GLenum mode)
{
    GLCALL(glHint, target, mode);
}

inline void mglClearDepth (GLclampd depth)
{
    GLCALL(glClearDepth, depth);
}

inline void mglDepthFunc (GLenum func)
{
    GLCALL(glDepthFunc, func);
}

inline void mglDepthMask (GLboolean flag)
{
    GLCALL(glDepthMask, flag);
}

inline void mglDepthRange (GLclampd near_val, GLclampd far_val)
{
    GLCALL(glDepthRange, near_val, far_val);
}

inline void mglClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCALL(glClearAccum, red, green, blue, alpha);
}

inline void mglAccum (GLenum op, GLfloat value)
{
    GLCALL(glAccum, op, value);
}

inline void mglMatrixMode (GLenum mode)
{
    GLCALL(glMatrixMode, mode);
}

inline void mglOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    GLCALL(glOrtho, left, right, bottom, top, near_val, far_val);
}

inline void mglFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    GLCALL(glFrustum, left, right, bottom, top, near_val, far_val);
}

inline void mglViewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glViewport, x, y, width, height);
}

inline void mglPushMatrix ()
{
    GLCALL(glPushMatrix);
}

inline void mglPopMatrix ()
{
    GLCALL(glPopMatrix);
}

inline void mglLoadIdentity ()
{
    GLCALL(glLoadIdentity);
}

inline void mglLoadMatrixd (const GLdouble * m)
{
    GLCALL(glLoadMatrixd, m);
}

inline void mglLoadMatrixf (const GLfloat * m)
{
    GLCALL(glLoadMatrixf, m);
}

inline void mglMultMatrixd (const GLdouble * m)
{
    GLCALL(glMultMatrixd, m);
}

inline void mglMultMatrixf (const GLfloat * m)
{
    GLCALL(glMultMatrixf, m);
}

inline void mglRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glRotated, angle, x, y, z);
}

inline void mglRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glRotatef, angle, x, y, z);
}

inline void mglScaled (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glScaled, x, y, z);
}

inline void mglScalef (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glScalef, x, y, z);
}

inline void mglTranslated (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glTranslated, x, y, z);
}

inline void mglTranslatef (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glTranslatef, x, y, z);
}

inline GLboolean mglIsList (GLuint list)
{
    return GLCALL(glIsList, list);
}

inline void mglDeleteLists (GLuint list, GLsizei range)
{
    GLCALL(glDeleteLists, list, range);
}

inline GLuint mglGenLists (GLsizei range)
{
    return GLCALL(glGenLists, range);
}

inline void mglNewList (GLuint list, GLenum mode)
{
    GLCALL(glNewList, list, mode);
}

inline void mglEndList ()
{
    GLCALL(glEndList);
}

inline void mglCallList (GLuint list)
{
    GLCALL(glCallList, list);
}

inline void mglCallLists (GLsizei n, GLenum type, const GLvoid * lists)
{
    GLCALL(glCallLists, n, type, lists);
}

inline void mglListBase (GLuint base)
{
    GLCALL(glListBase, base);
}

inline void mglBegin (GLenum mode)
{
    GLCALL(glBegin, mode);
}

inline void mglEnd ()
{
    GLCALL(glEnd);
}

inline void mglVertex2d (GLdouble x, GLdouble y)
{
    GLCALL(glVertex2d, x, y);
}

inline void mglVertex2f (GLfloat x, GLfloat y)
{
    GLCALL(glVertex2f, x, y);
}

inline void mglVertex2i (GLint x, GLint y)
{
    GLCALL(glVertex2i, x, y);
}

inline void mglVertex2s (GLshort x, GLshort y)
{
    GLCALL(glVertex2s, x, y);
}

inline void mglVertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertex3d, x, y, z);
}

inline void mglVertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertex3f, x, y, z);
}

inline void mglVertex3i (GLint x, GLint y, GLint z)
{
    GLCALL(glVertex3i, x, y, z);
}

inline void mglVertex3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertex3s, x, y, z);
}

inline void mglVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertex4d, x, y, z, w);
}

inline void mglVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertex4f, x, y, z, w);
}

inline void mglVertex4i (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertex4i, x, y, z, w);
}

inline void mglVertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertex4s, x, y, z, w);
}

inline void mglVertex2dv (const GLdouble * v)
{
    GLCALL(glVertex2dv, v);
}

inline void mglVertex2fv (const GLfloat * v)
{
    GLCALL(glVertex2fv, v);
}

inline void mglVertex2iv (const GLint * v)
{
    GLCALL(glVertex2iv, v);
}

inline void mglVertex2sv (const GLshort * v)
{
    GLCALL(glVertex2sv, v);
}

inline void mglVertex3dv (const GLdouble * v)
{
    GLCALL(glVertex3dv, v);
}

inline void mglVertex3fv (const GLfloat * v)
{
    GLCALL(glVertex3fv, v);
}

inline void mglVertex3iv (const GLint * v)
{
    GLCALL(glVertex3iv, v);
}

inline void mglVertex3sv (const GLshort * v)
{
    GLCALL(glVertex3sv, v);
}

inline void mglVertex4dv (const GLdouble * v)
{
    GLCALL(glVertex4dv, v);
}

inline void mglVertex4fv (const GLfloat * v)
{
    GLCALL(glVertex4fv, v);
}

inline void mglVertex4iv (const GLint * v)
{
    GLCALL(glVertex4iv, v);
}

inline void mglVertex4sv (const GLshort * v)
{
    GLCALL(glVertex4sv, v);
}

inline void mglNormal3b (GLbyte nx, GLbyte ny, GLbyte nz)
{
    GLCALL(glNormal3b, nx, ny, nz);
}

inline void mglNormal3d (GLdouble nx, GLdouble ny, GLdouble nz)
{
    GLCALL(glNormal3d, nx, ny, nz);
}

inline void mglNormal3f (GLfloat nx, GLfloat ny, GLfloat nz)
{
    GLCALL(glNormal3f, nx, ny, nz);
}

inline void mglNormal3i (GLint nx, GLint ny, GLint nz)
{
    GLCALL(glNormal3i, nx, ny, nz);
}

inline void mglNormal3s (GLshort nx, GLshort ny, GLshort nz)
{
    GLCALL(glNormal3s, nx, ny, nz);
}

inline void mglNormal3bv (const GLbyte * v)
{
    GLCALL(glNormal3bv, v);
}

inline void mglNormal3dv (const GLdouble * v)
{
    GLCALL(glNormal3dv, v);
}

inline void mglNormal3fv (const GLfloat * v)
{
    GLCALL(glNormal3fv, v);
}

inline void mglNormal3iv (const GLint * v)
{
    GLCALL(glNormal3iv, v);
}

inline void mglNormal3sv (const GLshort * v)
{
    GLCALL(glNormal3sv, v);
}

inline void mglIndexd (GLdouble c)
{
    GLCALL(glIndexd, c);
}

inline void mglIndexf (GLfloat c)
{
    GLCALL(glIndexf, c);
}

inline void mglIndexi (GLint c)
{
    GLCALL(glIndexi, c);
}

inline void mglIndexs (GLshort c)
{
    GLCALL(glIndexs, c);
}

inline void mglIndexub (GLubyte c)
{
    GLCALL(glIndexub, c);
}

inline void mglIndexdv (const GLdouble * c)
{
    GLCALL(glIndexdv, c);
}

inline void mglIndexfv (const GLfloat * c)
{
    GLCALL(glIndexfv, c);
}

inline void mglIndexiv (const GLint * c)
{
    GLCALL(glIndexiv, c);
}

inline void mglIndexsv (const GLshort * c)
{
    GLCALL(glIndexsv, c);
}

inline void mglIndexubv (const GLubyte * c)
{
    GLCALL(glIndexubv, c);
}

inline void mglColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glColor3b, red, green, blue);
}

inline void mglColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glColor3d, red, green, blue);
}

inline void mglColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glColor3f, red, green, blue);
}

inline void mglColor3i (GLint red, GLint green, GLint blue)
{
    GLCALL(glColor3i, red, green, blue);
}

inline void mglColor3s (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glColor3s, red, green, blue);
}

inline void mglColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glColor3ub, red, green, blue);
}

inline void mglColor3ui (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glColor3ui, red, green, blue);
}

inline void mglColor3us (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glColor3us, red, green, blue);
}

inline void mglColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha)
{
    GLCALL(glColor4b, red, green, blue, alpha);
}

inline void mglColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    GLCALL(glColor4d, red, green, blue, alpha);
}

inline void mglColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    GLCALL(glColor4f, red, green, blue, alpha);
}

inline void mglColor4i (GLint red, GLint green, GLint blue, GLint alpha)
{
    GLCALL(glColor4i, red, green, blue, alpha);
}

inline void mglColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha)
{
    GLCALL(glColor4s, red, green, blue, alpha);
}

inline void mglColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    GLCALL(glColor4ub, red, green, blue, alpha);
}

inline void mglColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    GLCALL(glColor4ui, red, green, blue, alpha);
}

inline void mglColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha)
{
    GLCALL(glColor4us, red, green, blue, alpha);
}

inline void mglColor3bv (const GLbyte * v)
{
    GLCALL(glColor3bv, v);
}

inline void mglColor3dv (const GLdouble * v)
{
    GLCALL(glColor3dv, v);
}

inline void mglColor3fv (const GLfloat * v)
{
    GLCALL(glColor3fv, v);
}

inline void mglColor3iv (const GLint * v)
{
    GLCALL(glColor3iv, v);
}

inline void mglColor3sv (const GLshort * v)
{
    GLCALL(glColor3sv, v);
}

inline void mglColor3ubv (const GLubyte * v)
{
    GLCALL(glColor3ubv, v);
}

inline void mglColor3uiv (const GLuint * v)
{
    GLCALL(glColor3uiv, v);
}

inline void mglColor3usv (const GLushort * v)
{
    GLCALL(glColor3usv, v);
}

inline void mglColor4bv (const GLbyte * v)
{
    GLCALL(glColor4bv, v);
}

inline void mglColor4dv (const GLdouble * v)
{
    GLCALL(glColor4dv, v);
}

inline void mglColor4fv (const GLfloat * v)
{
    GLCALL(glColor4fv, v);
}

inline void mglColor4iv (const GLint * v)
{
    GLCALL(glColor4iv, v);
}

inline void mglColor4sv (const GLshort * v)
{
    GLCALL(glColor4sv, v);
}

inline void mglColor4ubv (const GLubyte * v)
{
    GLCALL(glColor4ubv, v);
}

inline void mglColor4uiv (const GLuint * v)
{
    GLCALL(glColor4uiv, v);
}

inline void mglColor4usv (const GLushort * v)
{
    GLCALL(glColor4usv, v);
}

inline void mglTexCoord1d (GLdouble s)
{
    GLCALL(glTexCoord1d, s);
}

inline void mglTexCoord1f (GLfloat s)
{
    GLCALL(glTexCoord1f, s);
}

inline void mglTexCoord1i (GLint s)
{
    GLCALL(glTexCoord1i, s);
}

inline void mglTexCoord1s (GLshort s)
{
    GLCALL(glTexCoord1s, s);
}

inline void mglTexCoord2d (GLdouble s, GLdouble t)
{
    GLCALL(glTexCoord2d, s, t);
}

inline void mglTexCoord2f (GLfloat s, GLfloat t)
{
    GLCALL(glTexCoord2f, s, t);
}

inline void mglTexCoord2i (GLint s, GLint t)
{
    GLCALL(glTexCoord2i, s, t);
}

inline void mglTexCoord2s (GLshort s, GLshort t)
{
    GLCALL(glTexCoord2s, s, t);
}

inline void mglTexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glTexCoord3d, s, t, r);
}

inline void mglTexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glTexCoord3f, s, t, r);
}

inline void mglTexCoord3i (GLint s, GLint t, GLint r)
{
    GLCALL(glTexCoord3i, s, t, r);
}

inline void mglTexCoord3s (GLshort s, GLshort t, GLshort r)
{
    GLCALL(glTexCoord3s, s, t, r);
}

inline void mglTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glTexCoord4d, s, t, r, q);
}

inline void mglTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glTexCoord4f, s, t, r, q);
}

inline void mglTexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glTexCoord4i, s, t, r, q);
}

inline void mglTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glTexCoord4s, s, t, r, q);
}

inline void mglTexCoord1dv (const GLdouble * v)
{
    GLCALL(glTexCoord1dv, v);
}

inline void mglTexCoord1fv (const GLfloat * v)
{
    GLCALL(glTexCoord1fv, v);
}

inline void mglTexCoord1iv (const GLint * v)
{
    GLCALL(glTexCoord1iv, v);
}

inline void mglTexCoord1sv (const GLshort * v)
{
    GLCALL(glTexCoord1sv, v);
}

inline void mglTexCoord2dv (const GLdouble * v)
{
    GLCALL(glTexCoord2dv, v);
}

inline void mglTexCoord2fv (const GLfloat * v)
{
    GLCALL(glTexCoord2fv, v);
}

inline void mglTexCoord2iv (const GLint * v)
{
    GLCALL(glTexCoord2iv, v);
}

inline void mglTexCoord2sv (const GLshort * v)
{
    GLCALL(glTexCoord2sv, v);
}

inline void mglTexCoord3dv (const GLdouble * v)
{
    GLCALL(glTexCoord3dv, v);
}

inline void mglTexCoord3fv (const GLfloat * v)
{
    GLCALL(glTexCoord3fv, v);
}

inline void mglTexCoord3iv (const GLint * v)
{
    GLCALL(glTexCoord3iv, v);
}

inline void mglTexCoord3sv (const GLshort * v)
{
    GLCALL(glTexCoord3sv, v);
}

inline void mglTexCoord4dv (const GLdouble * v)
{
    GLCALL(glTexCoord4dv, v);
}

inline void mglTexCoord4fv (const GLfloat * v)
{
    GLCALL(glTexCoord4fv, v);
}

inline void mglTexCoord4iv (const GLint * v)
{
    GLCALL(glTexCoord4iv, v);
}

inline void mglTexCoord4sv (const GLshort * v)
{
    GLCALL(glTexCoord4sv, v);
}

inline void mglRasterPos2d (GLdouble x, GLdouble y)
{
    GLCALL(glRasterPos2d, x, y);
}

inline void mglRasterPos2f (GLfloat x, GLfloat y)
{
    GLCALL(glRasterPos2f, x, y);
}

inline void mglRasterPos2i (GLint x, GLint y)
{
    GLCALL(glRasterPos2i, x, y);
}

inline void mglRasterPos2s (GLshort x, GLshort y)
{
    GLCALL(glRasterPos2s, x, y);
}

inline void mglRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glRasterPos3d, x, y, z);
}

inline void mglRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glRasterPos3f, x, y, z);
}

inline void mglRasterPos3i (GLint x, GLint y, GLint z)
{
    GLCALL(glRasterPos3i, x, y, z);
}

inline void mglRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glRasterPos3s, x, y, z);
}

inline void mglRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glRasterPos4d, x, y, z, w);
}

inline void mglRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glRasterPos4f, x, y, z, w);
}

inline void mglRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glRasterPos4i, x, y, z, w);
}

inline void mglRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glRasterPos4s, x, y, z, w);
}

inline void mglRasterPos2dv (const GLdouble * v)
{
    GLCALL(glRasterPos2dv, v);
}

inline void mglRasterPos2fv (const GLfloat * v)
{
    GLCALL(glRasterPos2fv, v);
}

inline void mglRasterPos2iv (const GLint * v)
{
    GLCALL(glRasterPos2iv, v);
}

inline void mglRasterPos2sv (const GLshort * v)
{
    GLCALL(glRasterPos2sv, v);
}

inline void mglRasterPos3dv (const GLdouble * v)
{
    GLCALL(glRasterPos3dv, v);
}

inline void mglRasterPos3fv (const GLfloat * v)
{
    GLCALL(glRasterPos3fv, v);
}

inline void mglRasterPos3iv (const GLint * v)
{
    GLCALL(glRasterPos3iv, v);
}

inline void mglRasterPos3sv (const GLshort * v)
{
    GLCALL(glRasterPos3sv, v);
}

inline void mglRasterPos4dv (const GLdouble * v)
{
    GLCALL(glRasterPos4dv, v);
}

inline void mglRasterPos4fv (const GLfloat * v)
{
    GLCALL(glRasterPos4fv, v);
}

inline void mglRasterPos4iv (const GLint * v)
{
    GLCALL(glRasterPos4iv, v);
}

inline void mglRasterPos4sv (const GLshort * v)
{
    GLCALL(glRasterPos4sv, v);
}

inline void mglRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    GLCALL(glRectd, x1, y1, x2, y2);
}

inline void mglRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    GLCALL(glRectf, x1, y1, x2, y2);
}

inline void mglRecti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    GLCALL(glRecti, x1, y1, x2, y2);
}

inline void mglRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    GLCALL(glRects, x1, y1, x2, y2);
}

inline void mglRectdv (const GLdouble * v1, const GLdouble * v2)
{
    GLCALL(glRectdv, v1, v2);
}

inline void mglRectfv (const GLfloat * v1, const GLfloat * v2)
{
    GLCALL(glRectfv, v1, v2);
}

inline void mglRectiv (const GLint * v1, const GLint * v2)
{
    GLCALL(glRectiv, v1, v2);
}

inline void mglRectsv (const GLshort * v1, const GLshort * v2)
{
    GLCALL(glRectsv, v1, v2);
}

inline void mglVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glVertexPointer, size, type, stride, ptr);
}

inline void mglNormalPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glNormalPointer, type, stride, ptr);
}

inline void mglColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glColorPointer, size, type, stride, ptr);
}

inline void mglIndexPointer (GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glIndexPointer, type, stride, ptr);
}

inline void mglTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glTexCoordPointer, size, type, stride, ptr);
}

inline void mglEdgeFlagPointer (GLsizei stride, const GLvoid * ptr)
{
    GLCALL(glEdgeFlagPointer, stride, ptr);
}

inline void mglGetPointerv (GLenum pname, GLvoid * * params)
{
    GLCALL(glGetPointerv, pname, params);
}

inline void mglArrayElement (GLint i)
{
    GLCALL(glArrayElement, i);
}

inline void mglDrawArrays (GLenum mode, GLint first, GLsizei count)
{
    GLCALL(glDrawArrays, mode, first, count);
}

inline void mglDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawElements, mode, count, type, indices);
}

inline void mglInterleavedArrays (GLenum format, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glInterleavedArrays, format, stride, pointer);
}

inline void mglShadeModel (GLenum mode)
{
    GLCALL(glShadeModel, mode);
}

inline void mglLightf (GLenum light, GLenum pname, GLfloat param)
{
    GLCALL(glLightf, light, pname, param);
}

inline void mglLighti (GLenum light, GLenum pname, GLint param)
{
    GLCALL(glLighti, light, pname, param);
}

inline void mglLightfv (GLenum light, GLenum pname, const GLfloat * params)
{
    GLCALL(glLightfv, light, pname, params);
}

inline void mglLightiv (GLenum light, GLenum pname, const GLint * params)
{
    GLCALL(glLightiv, light, pname, params);
}

inline void mglGetLightfv (GLenum light, GLenum pname, GLfloat * params)
{
    GLCALL(glGetLightfv, light, pname, params);
}

inline void mglGetLightiv (GLenum light, GLenum pname, GLint * params)
{
    GLCALL(glGetLightiv, light, pname, params);
}

inline void mglLightModelf (GLenum pname, GLfloat param)
{
    GLCALL(glLightModelf, pname, param);
}

inline void mglLightModeli (GLenum pname, GLint param)
{
    GLCALL(glLightModeli, pname, param);
}

inline void mglLightModelfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glLightModelfv, pname, params);
}

inline void mglLightModeliv (GLenum pname, const GLint * params)
{
    GLCALL(glLightModeliv, pname, params);
}

inline void mglMaterialf (GLenum face, GLenum pname, GLfloat param)
{
    GLCALL(glMaterialf, face, pname, param);
}

inline void mglMateriali (GLenum face, GLenum pname, GLint param)
{
    GLCALL(glMateriali, face, pname, param);
}

inline void mglMaterialfv (GLenum face, GLenum pname, const GLfloat * params)
{
    GLCALL(glMaterialfv, face, pname, params);
}

inline void mglMaterialiv (GLenum face, GLenum pname, const GLint * params)
{
    GLCALL(glMaterialiv, face, pname, params);
}

inline void mglGetMaterialfv (GLenum face, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMaterialfv, face, pname, params);
}

inline void mglGetMaterialiv (GLenum face, GLenum pname, GLint * params)
{
    GLCALL(glGetMaterialiv, face, pname, params);
}

inline void mglColorMaterial (GLenum face, GLenum mode)
{
    GLCALL(glColorMaterial, face, mode);
}

inline void mglPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    GLCALL(glPixelZoom, xfactor, yfactor);
}

inline void mglPixelStoref (GLenum pname, GLfloat param)
{
    GLCALL(glPixelStoref, pname, param);
}

inline void mglPixelStorei (GLenum pname, GLint param)
{
    GLCALL(glPixelStorei, pname, param);
}

inline void mglPixelTransferf (GLenum pname, GLfloat param)
{
    GLCALL(glPixelTransferf, pname, param);
}

inline void mglPixelTransferi (GLenum pname, GLint param)
{
    GLCALL(glPixelTransferi, pname, param);
}

inline void mglPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat * values)
{
    GLCALL(glPixelMapfv, map, mapsize, values);
}

inline void mglPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint * values)
{
    GLCALL(glPixelMapuiv, map, mapsize, values);
}

inline void mglPixelMapusv (GLenum map, GLsizei mapsize, const GLushort * values)
{
    GLCALL(glPixelMapusv, map, mapsize, values);
}

inline void mglGetPixelMapfv (GLenum map, GLfloat * values)
{
    GLCALL(glGetPixelMapfv, map, values);
}

inline void mglGetPixelMapuiv (GLenum map, GLuint * values)
{
    GLCALL(glGetPixelMapuiv, map, values);
}

inline void mglGetPixelMapusv (GLenum map, GLushort * values)
{
    GLCALL(glGetPixelMapusv, map, values);
}

inline void mglBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap)
{
    GLCALL(glBitmap, width, height, xorig, yorig, xmove, ymove, bitmap);
}

inline void mglReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels)
{
    GLCALL(glReadPixels, x, y, width, height, format, type, pixels);
}

inline void mglDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glDrawPixels, width, height, format, type, pixels);
}

inline void mglCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    GLCALL(glCopyPixels, x, y, width, height, type);
}

inline void mglStencilFunc (GLenum func, GLint ref, GLuint mask)
{
    GLCALL(glStencilFunc, func, ref, mask);
}

inline void mglStencilMask (GLuint mask)
{
    GLCALL(glStencilMask, mask);
}

inline void mglStencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    GLCALL(glStencilOp, fail, zfail, zpass);
}

inline void mglClearStencil (GLint s)
{
    GLCALL(glClearStencil, s);
}

inline void mglTexGend (GLenum coord, GLenum pname, GLdouble param)
{
    GLCALL(glTexGend, coord, pname, param);
}

inline void mglTexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    GLCALL(glTexGenf, coord, pname, param);
}

inline void mglTexGeni (GLenum coord, GLenum pname, GLint param)
{
    GLCALL(glTexGeni, coord, pname, param);
}

inline void mglTexGendv (GLenum coord, GLenum pname, const GLdouble * params)
{
    GLCALL(glTexGendv, coord, pname, params);
}

inline void mglTexGenfv (GLenum coord, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexGenfv, coord, pname, params);
}

inline void mglTexGeniv (GLenum coord, GLenum pname, const GLint * params)
{
    GLCALL(glTexGeniv, coord, pname, params);
}

inline void mglGetTexGendv (GLenum coord, GLenum pname, GLdouble * params)
{
    GLCALL(glGetTexGendv, coord, pname, params);
}

inline void mglGetTexGenfv (GLenum coord, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexGenfv, coord, pname, params);
}

inline void mglGetTexGeniv (GLenum coord, GLenum pname, GLint * params)
{
    GLCALL(glGetTexGeniv, coord, pname, params);
}

inline void mglTexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    GLCALL(glTexEnvf, target, pname, param);
}

inline void mglTexEnvi (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glTexEnvi, target, pname, param);
}

inline void mglTexEnvfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexEnvfv, target, pname, params);
}

inline void mglTexEnviv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexEnviv, target, pname, params);
}

inline void mglGetTexEnvfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexEnvfv, target, pname, params);
}

inline void mglGetTexEnviv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexEnviv, target, pname, params);
}

inline void mglTexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    GLCALL(glTexParameterf, target, pname, param);
}

inline void mglTexParameteri (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glTexParameteri, target, pname, param);
}

inline void mglTexParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glTexParameterfv, target, pname, params);
}

inline void mglTexParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameteriv, target, pname, params);
}

inline void mglGetTexParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexParameterfv, target, pname, params);
}

inline void mglGetTexParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameteriv, target, pname, params);
}

inline void mglGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat * params)
{
    GLCALL(glGetTexLevelParameterfv, target, level, pname, params);
}

inline void mglGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint * params)
{
    GLCALL(glGetTexLevelParameteriv, target, level, pname, params);
}

inline void mglTexImage1D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage1D, target, level, internalFormat, width, border, format, type, pixels);
}

inline void mglTexImage2D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage2D, target, level, internalFormat, width, height, border, format, type, pixels);
}

inline void mglGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels)
{
    GLCALL(glGetTexImage, target, level, format, type, pixels);
}

inline void mglGenTextures (GLsizei n, GLuint * textures)
{
    GLCALL(glGenTextures, n, textures);
}

inline void mglDeleteTextures (GLsizei n, const GLuint * textures)
{
    GLCALL(glDeleteTextures, n, textures);
}

inline void mglBindTexture (GLenum target, GLuint texture)
{
    GLCALL(glBindTexture, target, texture);
}

inline void mglPrioritizeTextures (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    GLCALL(glPrioritizeTextures, n, textures, priorities);
}

inline GLboolean mglAreTexturesResident (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    return GLCALL(glAreTexturesResident, n, textures, residences);
}

inline GLboolean mglIsTexture (GLuint texture)
{
    return GLCALL(glIsTexture, texture);
}

inline void mglTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage1D, target, level, xoffset, width, format, type, pixels);
}

inline void mglTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage2D, target, level, xoffset, yoffset, width, height, format, type, pixels);
}

inline void mglCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    GLCALL(glCopyTexImage1D, target, level, internalformat, x, y, width, border);
}

inline void mglCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    GLCALL(glCopyTexImage2D, target, level, internalformat, x, y, width, height, border);
}

inline void mglCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyTexSubImage1D, target, level, xoffset, x, y, width);
}

inline void mglCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage2D, target, level, xoffset, yoffset, x, y, width, height);
}

inline void mglMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points)
{
    GLCALL(glMap1d, target, u1, u2, stride, order, points);
}

inline void mglMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points)
{
    GLCALL(glMap1f, target, u1, u2, stride, order, points);
}

inline void mglMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points)
{
    GLCALL(glMap2d, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

inline void mglMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points)
{
    GLCALL(glMap2f, target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

inline void mglGetMapdv (GLenum target, GLenum query, GLdouble * v)
{
    GLCALL(glGetMapdv, target, query, v);
}

inline void mglGetMapfv (GLenum target, GLenum query, GLfloat * v)
{
    GLCALL(glGetMapfv, target, query, v);
}

inline void mglGetMapiv (GLenum target, GLenum query, GLint * v)
{
    GLCALL(glGetMapiv, target, query, v);
}

inline void mglEvalCoord1d (GLdouble u)
{
    GLCALL(glEvalCoord1d, u);
}

inline void mglEvalCoord1f (GLfloat u)
{
    GLCALL(glEvalCoord1f, u);
}

inline void mglEvalCoord1dv (const GLdouble * u)
{
    GLCALL(glEvalCoord1dv, u);
}

inline void mglEvalCoord1fv (const GLfloat * u)
{
    GLCALL(glEvalCoord1fv, u);
}

inline void mglEvalCoord2d (GLdouble u, GLdouble v)
{
    GLCALL(glEvalCoord2d, u, v);
}

inline void mglEvalCoord2f (GLfloat u, GLfloat v)
{
    GLCALL(glEvalCoord2f, u, v);
}

inline void mglEvalCoord2dv (const GLdouble * u)
{
    GLCALL(glEvalCoord2dv, u);
}

inline void mglEvalCoord2fv (const GLfloat * u)
{
    GLCALL(glEvalCoord2fv, u);
}

inline void mglMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    GLCALL(glMapGrid1d, un, u1, u2);
}

inline void mglMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    GLCALL(glMapGrid1f, un, u1, u2);
}

inline void mglMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2)
{
    GLCALL(glMapGrid2d, un, u1, u2, vn, v1, v2);
}

inline void mglMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2)
{
    GLCALL(glMapGrid2f, un, u1, u2, vn, v1, v2);
}

inline void mglEvalPoint1 (GLint i)
{
    GLCALL(glEvalPoint1, i);
}

inline void mglEvalPoint2 (GLint i, GLint j)
{
    GLCALL(glEvalPoint2, i, j);
}

inline void mglEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    GLCALL(glEvalMesh1, mode, i1, i2);
}

inline void mglEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    GLCALL(glEvalMesh2, mode, i1, i2, j1, j2);
}

inline void mglFogf (GLenum pname, GLfloat param)
{
    GLCALL(glFogf, pname, param);
}

inline void mglFogi (GLenum pname, GLint param)
{
    GLCALL(glFogi, pname, param);
}

inline void mglFogfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glFogfv, pname, params);
}

inline void mglFogiv (GLenum pname, const GLint * params)
{
    GLCALL(glFogiv, pname, params);
}

inline void mglFeedbackBuffer (GLsizei size, GLenum type, GLfloat * buffer)
{
    GLCALL(glFeedbackBuffer, size, type, buffer);
}

inline void mglPassThrough (GLfloat token)
{
    GLCALL(glPassThrough, token);
}

inline void mglSelectBuffer (GLsizei size, GLuint * buffer)
{
    GLCALL(glSelectBuffer, size, buffer);
}

inline void mglInitNames ()
{
    GLCALL(glInitNames);
}

inline void mglLoadName (GLuint name)
{
    GLCALL(glLoadName, name);
}

inline void mglPushName (GLuint name)
{
    GLCALL(glPushName, name);
}

inline void mglPopName ()
{
    GLCALL(glPopName);
}

inline void mglDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawRangeElements, mode, start, end, count, type, indices);
}

inline void mglTexImage3D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage3D, target, level, internalFormat, width, height, depth, border, format, type, pixels);
}

inline void mglTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

inline void mglCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage3D, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

inline void mglColorTable (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTable, target, internalformat, width, format, type, table);
}

inline void mglColorSubTable (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    GLCALL(glColorSubTable, target, start, count, format, type, data);
}

inline void mglColorTableParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glColorTableParameteriv, target, pname, params);
}

inline void mglColorTableParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glColorTableParameterfv, target, pname, params);
}

inline void mglCopyColorSubTable (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorSubTable, target, start, x, y, width);
}

inline void mglCopyColorTable (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorTable, target, internalformat, x, y, width);
}

inline void mglGetColorTable (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    GLCALL(glGetColorTable, target, format, type, table);
}

inline void mglGetColorTableParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfv, target, pname, params);
}

inline void mglGetColorTableParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameteriv, target, pname, params);
}

inline void mglBlendEquation (GLenum mode)
{
    GLCALL(glBlendEquation, mode);
}

inline void mglBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glBlendColor, red, green, blue, alpha);
}

inline void mglHistogram (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    GLCALL(glHistogram, target, width, internalformat, sink);
}

inline void mglResetHistogram (GLenum target)
{
    GLCALL(glResetHistogram, target);
}

inline void mglGetHistogram (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetHistogram, target, reset, format, type, values);
}

inline void mglGetHistogramParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetHistogramParameterfv, target, pname, params);
}

inline void mglGetHistogramParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetHistogramParameteriv, target, pname, params);
}

inline void mglMinmax (GLenum target, GLenum internalformat, GLboolean sink)
{
    GLCALL(glMinmax, target, internalformat, sink);
}

inline void mglResetMinmax (GLenum target)
{
    GLCALL(glResetMinmax, target);
}

inline void mglGetMinmax (GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid * values)
{
    GLCALL(glGetMinmax, target, reset, format, types, values);
}

inline void mglGetMinmaxParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMinmaxParameterfv, target, pname, params);
}

inline void mglGetMinmaxParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetMinmaxParameteriv, target, pname, params);
}

inline void mglConvolutionFilter1D (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter1D, target, internalformat, width, format, type, image);
}

inline void mglConvolutionFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter2D, target, internalformat, width, height, format, type, image);
}

inline void mglConvolutionParameterf (GLenum target, GLenum pname, GLfloat params)
{
    GLCALL(glConvolutionParameterf, target, pname, params);
}

inline void mglConvolutionParameterfv (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glConvolutionParameterfv, target, pname, params);
}

inline void mglConvolutionParameteri (GLenum target, GLenum pname, GLint params)
{
    GLCALL(glConvolutionParameteri, target, pname, params);
}

inline void mglConvolutionParameteriv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glConvolutionParameteriv, target, pname, params);
}

inline void mglCopyConvolutionFilter1D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyConvolutionFilter1D, target, internalformat, x, y, width);
}

inline void mglCopyConvolutionFilter2D (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyConvolutionFilter2D, target, internalformat, x, y, width, height);
}

inline void mglGetConvolutionFilter (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    GLCALL(glGetConvolutionFilter, target, format, type, image);
}

inline void mglGetConvolutionParameterfv (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetConvolutionParameterfv, target, pname, params);
}

inline void mglGetConvolutionParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetConvolutionParameteriv, target, pname, params);
}

inline void mglSeparableFilter2D (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    GLCALL(glSeparableFilter2D, target, internalformat, width, height, format, type, row, column);
}

inline void mglGetSeparableFilter (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    GLCALL(glGetSeparableFilter, target, format, type, row, column, span);
}

inline void mglActiveTexture (GLenum texture)
{
    GLCALL(glActiveTexture, texture);
}

inline void mglClientActiveTexture (GLenum texture)
{
    GLCALL(glClientActiveTexture, texture);
}

inline void mglCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage1D, target, level, internalformat, width, border, imageSize, data);
}

inline void mglCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage2D, target, level, internalformat, width, height, border, imageSize, data);
}

inline void mglCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage3D, target, level, internalformat, width, height, depth, border, imageSize, data);
}

inline void mglCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage1D, target, level, xoffset, width, format, imageSize, data);
}

inline void mglCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage2D, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

inline void mglCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage3D, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

inline void mglGetCompressedTexImage (GLenum target, GLint lod, GLvoid * img)
{
    GLCALL(glGetCompressedTexImage, target, lod, img);
}

inline void mglMultiTexCoord1d (GLenum target, GLdouble s)
{
    GLCALL(glMultiTexCoord1d, target, s);
}

inline void mglMultiTexCoord1dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord1dv, target, v);
}

inline void mglMultiTexCoord1f (GLenum target, GLfloat s)
{
    GLCALL(glMultiTexCoord1f, target, s);
}

inline void mglMultiTexCoord1fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord1fv, target, v);
}

inline void mglMultiTexCoord1i (GLenum target, GLint s)
{
    GLCALL(glMultiTexCoord1i, target, s);
}

inline void mglMultiTexCoord1iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord1iv, target, v);
}

inline void mglMultiTexCoord1s (GLenum target, GLshort s)
{
    GLCALL(glMultiTexCoord1s, target, s);
}

inline void mglMultiTexCoord1sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord1sv, target, v);
}

inline void mglMultiTexCoord2d (GLenum target, GLdouble s, GLdouble t)
{
    GLCALL(glMultiTexCoord2d, target, s, t);
}

inline void mglMultiTexCoord2dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord2dv, target, v);
}

inline void mglMultiTexCoord2f (GLenum target, GLfloat s, GLfloat t)
{
    GLCALL(glMultiTexCoord2f, target, s, t);
}

inline void mglMultiTexCoord2fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord2fv, target, v);
}

inline void mglMultiTexCoord2i (GLenum target, GLint s, GLint t)
{
    GLCALL(glMultiTexCoord2i, target, s, t);
}

inline void mglMultiTexCoord2iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord2iv, target, v);
}

inline void mglMultiTexCoord2s (GLenum target, GLshort s, GLshort t)
{
    GLCALL(glMultiTexCoord2s, target, s, t);
}

inline void mglMultiTexCoord2sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord2sv, target, v);
}

inline void mglMultiTexCoord3d (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glMultiTexCoord3d, target, s, t, r);
}

inline void mglMultiTexCoord3dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord3dv, target, v);
}

inline void mglMultiTexCoord3f (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glMultiTexCoord3f, target, s, t, r);
}

inline void mglMultiTexCoord3fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord3fv, target, v);
}

inline void mglMultiTexCoord3i (GLenum target, GLint s, GLint t, GLint r)
{
    GLCALL(glMultiTexCoord3i, target, s, t, r);
}

inline void mglMultiTexCoord3iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord3iv, target, v);
}

inline void mglMultiTexCoord3s (GLenum target, GLshort s, GLshort t, GLshort r)
{
    GLCALL(glMultiTexCoord3s, target, s, t, r);
}

inline void mglMultiTexCoord3sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord3sv, target, v);
}

inline void mglMultiTexCoord4d (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glMultiTexCoord4d, target, s, t, r, q);
}

inline void mglMultiTexCoord4dv (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord4dv, target, v);
}

inline void mglMultiTexCoord4f (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glMultiTexCoord4f, target, s, t, r, q);
}

inline void mglMultiTexCoord4fv (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord4fv, target, v);
}

inline void mglMultiTexCoord4i (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glMultiTexCoord4i, target, s, t, r, q);
}

inline void mglMultiTexCoord4iv (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord4iv, target, v);
}

inline void mglMultiTexCoord4s (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glMultiTexCoord4s, target, s, t, r, q);
}

inline void mglMultiTexCoord4sv (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord4sv, target, v);
}

inline void mglLoadTransposeMatrixd (const GLdouble * m)
{
    GLCALL(glLoadTransposeMatrixd, m);
}

inline void mglLoadTransposeMatrixf (const GLfloat * m)
{
    GLCALL(glLoadTransposeMatrixf, m);
}

inline void mglMultTransposeMatrixd (const GLdouble * m)
{
    GLCALL(glMultTransposeMatrixd, m);
}

inline void mglMultTransposeMatrixf (const GLfloat * m)
{
    GLCALL(glMultTransposeMatrixf, m);
}

inline void mglSampleCoverage (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleCoverage, value, invert);
}

inline void mglActiveTextureARB (GLenum texture)
{
    GLCALL(glActiveTextureARB, texture);
}

inline void mglClientActiveTextureARB (GLenum texture)
{
    GLCALL(glClientActiveTextureARB, texture);
}

inline void mglMultiTexCoord1dARB (GLenum target, GLdouble s)
{
    GLCALL(glMultiTexCoord1dARB, target, s);
}

inline void mglMultiTexCoord1dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord1dvARB, target, v);
}

inline void mglMultiTexCoord1fARB (GLenum target, GLfloat s)
{
    GLCALL(glMultiTexCoord1fARB, target, s);
}

inline void mglMultiTexCoord1fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord1fvARB, target, v);
}

inline void mglMultiTexCoord1iARB (GLenum target, GLint s)
{
    GLCALL(glMultiTexCoord1iARB, target, s);
}

inline void mglMultiTexCoord1ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord1ivARB, target, v);
}

inline void mglMultiTexCoord1sARB (GLenum target, GLshort s)
{
    GLCALL(glMultiTexCoord1sARB, target, s);
}

inline void mglMultiTexCoord1svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord1svARB, target, v);
}

inline void mglMultiTexCoord2dARB (GLenum target, GLdouble s, GLdouble t)
{
    GLCALL(glMultiTexCoord2dARB, target, s, t);
}

inline void mglMultiTexCoord2dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord2dvARB, target, v);
}

inline void mglMultiTexCoord2fARB (GLenum target, GLfloat s, GLfloat t)
{
    GLCALL(glMultiTexCoord2fARB, target, s, t);
}

inline void mglMultiTexCoord2fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord2fvARB, target, v);
}

inline void mglMultiTexCoord2iARB (GLenum target, GLint s, GLint t)
{
    GLCALL(glMultiTexCoord2iARB, target, s, t);
}

inline void mglMultiTexCoord2ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord2ivARB, target, v);
}

inline void mglMultiTexCoord2sARB (GLenum target, GLshort s, GLshort t)
{
    GLCALL(glMultiTexCoord2sARB, target, s, t);
}

inline void mglMultiTexCoord2svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord2svARB, target, v);
}

inline void mglMultiTexCoord3dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
    GLCALL(glMultiTexCoord3dARB, target, s, t, r);
}

inline void mglMultiTexCoord3dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord3dvARB, target, v);
}

inline void mglMultiTexCoord3fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(glMultiTexCoord3fARB, target, s, t, r);
}

inline void mglMultiTexCoord3fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord3fvARB, target, v);
}

inline void mglMultiTexCoord3iARB (GLenum target, GLint s, GLint t, GLint r)
{
    GLCALL(glMultiTexCoord3iARB, target, s, t, r);
}

inline void mglMultiTexCoord3ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord3ivARB, target, v);
}

inline void mglMultiTexCoord3sARB (GLenum target, GLshort s, GLshort t, GLshort r)
{
    GLCALL(glMultiTexCoord3sARB, target, s, t, r);
}

inline void mglMultiTexCoord3svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord3svARB, target, v);
}

inline void mglMultiTexCoord4dARB (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLCALL(glMultiTexCoord4dARB, target, s, t, r, q);
}

inline void mglMultiTexCoord4dvARB (GLenum target, const GLdouble * v)
{
    GLCALL(glMultiTexCoord4dvARB, target, v);
}

inline void mglMultiTexCoord4fARB (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(glMultiTexCoord4fARB, target, s, t, r, q);
}

inline void mglMultiTexCoord4fvARB (GLenum target, const GLfloat * v)
{
    GLCALL(glMultiTexCoord4fvARB, target, v);
}

inline void mglMultiTexCoord4iARB (GLenum target, GLint s, GLint t, GLint r, GLint q)
{
    GLCALL(glMultiTexCoord4iARB, target, s, t, r, q);
}

inline void mglMultiTexCoord4ivARB (GLenum target, const GLint * v)
{
    GLCALL(glMultiTexCoord4ivARB, target, v);
}

inline void mglMultiTexCoord4sARB (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLCALL(glMultiTexCoord4sARB, target, s, t, r, q);
}

inline void mglMultiTexCoord4svARB (GLenum target, const GLshort * v)
{
    GLCALL(glMultiTexCoord4svARB, target, v);
}

inline void mglBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparate, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

inline void mglFogCoordf (GLfloat coord)
{
    GLCALL(glFogCoordf, coord);
}

inline void mglFogCoordfv (const GLfloat * coord)
{
    GLCALL(glFogCoordfv, coord);
}

inline void mglFogCoordd (GLdouble coord)
{
    GLCALL(glFogCoordd, coord);
}

inline void mglFogCoorddv (const GLdouble * coord)
{
    GLCALL(glFogCoorddv, coord);
}

inline void mglFogCoordPointer (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glFogCoordPointer, type, stride, pointer);
}

inline void mglMultiDrawArrays (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    GLCALL(glMultiDrawArrays, mode, first, count, primcount);
}

inline void mglMultiDrawElements (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    GLCALL(glMultiDrawElements, mode, count, type, indices, primcount);
}

inline void mglPointParameterf (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterf, pname, param);
}

inline void mglPointParameterfv (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfv, pname, params);
}

inline void mglPointParameteri (GLenum pname, GLint param)
{
    GLCALL(glPointParameteri, pname, param);
}

inline void mglPointParameteriv (GLenum pname, const GLint * params)
{
    GLCALL(glPointParameteriv, pname, params);
}

inline void mglSecondaryColor3b (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glSecondaryColor3b, red, green, blue);
}

inline void mglSecondaryColor3bv (const GLbyte * v)
{
    GLCALL(glSecondaryColor3bv, v);
}

inline void mglSecondaryColor3d (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glSecondaryColor3d, red, green, blue);
}

inline void mglSecondaryColor3dv (const GLdouble * v)
{
    GLCALL(glSecondaryColor3dv, v);
}

inline void mglSecondaryColor3f (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glSecondaryColor3f, red, green, blue);
}

inline void mglSecondaryColor3fv (const GLfloat * v)
{
    GLCALL(glSecondaryColor3fv, v);
}

inline void mglSecondaryColor3i (GLint red, GLint green, GLint blue)
{
    GLCALL(glSecondaryColor3i, red, green, blue);
}

inline void mglSecondaryColor3iv (const GLint * v)
{
    GLCALL(glSecondaryColor3iv, v);
}

inline void mglSecondaryColor3s (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glSecondaryColor3s, red, green, blue);
}

inline void mglSecondaryColor3sv (const GLshort * v)
{
    GLCALL(glSecondaryColor3sv, v);
}

inline void mglSecondaryColor3ub (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glSecondaryColor3ub, red, green, blue);
}

inline void mglSecondaryColor3ubv (const GLubyte * v)
{
    GLCALL(glSecondaryColor3ubv, v);
}

inline void mglSecondaryColor3ui (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glSecondaryColor3ui, red, green, blue);
}

inline void mglSecondaryColor3uiv (const GLuint * v)
{
    GLCALL(glSecondaryColor3uiv, v);
}

inline void mglSecondaryColor3us (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glSecondaryColor3us, red, green, blue);
}

inline void mglSecondaryColor3usv (const GLushort * v)
{
    GLCALL(glSecondaryColor3usv, v);
}

inline void mglSecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glSecondaryColorPointer, size, type, stride, pointer);
}

inline void mglWindowPos2d (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2d, x, y);
}

inline void mglWindowPos2dv (const GLdouble * v)
{
    GLCALL(glWindowPos2dv, v);
}

inline void mglWindowPos2f (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2f, x, y);
}

inline void mglWindowPos2fv (const GLfloat * v)
{
    GLCALL(glWindowPos2fv, v);
}

inline void mglWindowPos2i (GLint x, GLint y)
{
    GLCALL(glWindowPos2i, x, y);
}

inline void mglWindowPos2iv (const GLint * v)
{
    GLCALL(glWindowPos2iv, v);
}

inline void mglWindowPos2s (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2s, x, y);
}

inline void mglWindowPos2sv (const GLshort * v)
{
    GLCALL(glWindowPos2sv, v);
}

inline void mglWindowPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3d, x, y, z);
}

inline void mglWindowPos3dv (const GLdouble * v)
{
    GLCALL(glWindowPos3dv, v);
}

inline void mglWindowPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3f, x, y, z);
}

inline void mglWindowPos3fv (const GLfloat * v)
{
    GLCALL(glWindowPos3fv, v);
}

inline void mglWindowPos3i (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3i, x, y, z);
}

inline void mglWindowPos3iv (const GLint * v)
{
    GLCALL(glWindowPos3iv, v);
}

inline void mglWindowPos3s (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3s, x, y, z);
}

inline void mglWindowPos3sv (const GLshort * v)
{
    GLCALL(glWindowPos3sv, v);
}

inline void mglGenQueries (GLsizei n, GLuint * ids)
{
    GLCALL(glGenQueries, n, ids);
}

inline void mglDeleteQueries (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteQueries, n, ids);
}

inline GLboolean mglIsQuery (GLuint id)
{
    return GLCALL(glIsQuery, id);
}

inline void mglBeginQuery (GLenum target, GLuint id)
{
    GLCALL(glBeginQuery, target, id);
}

inline void mglEndQuery (GLenum target)
{
    GLCALL(glEndQuery, target);
}

inline void mglGetQueryiv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryiv, target, pname, params);
}

inline void mglGetQueryObjectiv (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryObjectiv, id, pname, params);
}

inline void mglGetQueryObjectuiv (GLuint id, GLenum pname, GLuint * params)
{
    GLCALL(glGetQueryObjectuiv, id, pname, params);
}

inline void mglBindBuffer (GLenum target, GLuint buffer)
{
    GLCALL(glBindBuffer, target, buffer);
}

inline void mglDeleteBuffers (GLsizei n, const GLuint * buffers)
{
    GLCALL(glDeleteBuffers, n, buffers);
}

inline void mglGenBuffers (GLsizei n, GLuint * buffers)
{
    GLCALL(glGenBuffers, n, buffers);
}

inline GLboolean mglIsBuffer (GLuint buffer)
{
    return GLCALL(glIsBuffer, buffer);
}

inline void mglBufferData (GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage)
{
    GLCALL(glBufferData, target, size, data, usage);
}

inline void mglBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data)
{
    GLCALL(glBufferSubData, target, offset, size, data);
}

inline void mglGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data)
{
    GLCALL(glGetBufferSubData, target, offset, size, data);
}

inline GLvoid* mglMapBuffer (GLenum target, GLenum access)
{
    return GLCALL(glMapBuffer, target, access);
}

inline GLboolean mglUnmapBuffer (GLenum target)
{
    return GLCALL(glUnmapBuffer, target);
}

inline void mglGetBufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetBufferParameteriv, target, pname, params);
}

inline void mglGetBufferPointerv (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetBufferPointerv, target, pname, params);
}

inline void mglBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparate, modeRGB, modeAlpha);
}

inline void mglDrawBuffers (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffers, n, bufs);
}

inline void mglStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    GLCALL(glStencilOpSeparate, face, sfail, dpfail, dppass);
}

inline void mglStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask)
{
    GLCALL(glStencilFuncSeparate, face, func, ref, mask);
}

inline void mglStencilMaskSeparate (GLenum face, GLuint mask)
{
    GLCALL(glStencilMaskSeparate, face, mask);
}

inline void mglAttachShader (GLuint program, GLuint shader)
{
    GLCALL(glAttachShader, program, shader);
}

inline void mglBindAttribLocation (GLuint program, GLuint index, const GLchar * name)
{
    GLCALL(glBindAttribLocation, program, index, name);
}

inline void mglCompileShader (GLuint shader)
{
    GLCALL(glCompileShader, shader);
}

inline GLuint mglCreateProgram ()
{
    return GLCALL(glCreateProgram);
}

inline GLuint mglCreateShader (GLenum type)
{
    return GLCALL(glCreateShader, type);
}

inline void mglDeleteProgram (GLuint program)
{
    GLCALL(glDeleteProgram, program);
}

inline void mglDeleteShader (GLuint shader)
{
    GLCALL(glDeleteShader, shader);
}

inline void mglDetachShader (GLuint program, GLuint shader)
{
    GLCALL(glDetachShader, program, shader);
}

inline void mglDisableVertexAttribArray (GLuint index)
{
    GLCALL(glDisableVertexAttribArray, index);
}

inline void mglEnableVertexAttribArray (GLuint index)
{
    GLCALL(glEnableVertexAttribArray, index);
}

inline void mglGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetActiveAttrib, program, index, bufSize, length, size, type, name);
}

inline void mglGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetActiveUniform, program, index, bufSize, length, size, type, name);
}

inline void mglGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei * count, GLuint * obj)
{
    GLCALL(glGetAttachedShaders, program, maxCount, count, obj);
}

inline GLint mglGetAttribLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetAttribLocation, program, name);
}

inline void mglGetProgramiv (GLuint program, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramiv, program, pname, params);
}

inline void mglGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    GLCALL(glGetProgramInfoLog, program, bufSize, length, infoLog);
}

inline void mglGetShaderiv (GLuint shader, GLenum pname, GLint * params)
{
    GLCALL(glGetShaderiv, shader, pname, params);
}

inline void mglGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
    GLCALL(glGetShaderInfoLog, shader, bufSize, length, infoLog);
}

inline void mglGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source)
{
    GLCALL(glGetShaderSource, shader, bufSize, length, source);
}

inline GLint mglGetUniformLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetUniformLocation, program, name);
}

inline void mglGetUniformfv (GLuint program, GLint location, GLfloat * params)
{
    GLCALL(glGetUniformfv, program, location, params);
}

inline void mglGetUniformiv (GLuint program, GLint location, GLint * params)
{
    GLCALL(glGetUniformiv, program, location, params);
}

inline void mglGetVertexAttribdv (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdv, index, pname, params);
}

inline void mglGetVertexAttribfv (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfv, index, pname, params);
}

inline void mglGetVertexAttribiv (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribiv, index, pname, params);
}

inline void mglGetVertexAttribPointerv (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointerv, index, pname, pointer);
}

inline GLboolean mglIsProgram (GLuint program)
{
    return GLCALL(glIsProgram, program);
}

inline GLboolean mglIsShader (GLuint shader)
{
    return GLCALL(glIsShader, shader);
}

inline void mglLinkProgram (GLuint program)
{
    GLCALL(glLinkProgram, program);
}

inline void mglShaderSource (GLuint shader, GLsizei count, const GLchar *  * string, const GLint * length)
{
    GLCALL(glShaderSource, shader, count, string, length);
}

inline void mglUseProgram (GLuint program)
{
    GLCALL(glUseProgram, program);
}

inline void mglUniform1f (GLint location, GLfloat v0)
{
    GLCALL(glUniform1f, location, v0);
}

inline void mglUniform2f (GLint location, GLfloat v0, GLfloat v1)
{
    GLCALL(glUniform2f, location, v0, v1);
}

inline void mglUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLCALL(glUniform3f, location, v0, v1, v2);
}

inline void mglUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLCALL(glUniform4f, location, v0, v1, v2, v3);
}

inline void mglUniform1i (GLint location, GLint v0)
{
    GLCALL(glUniform1i, location, v0);
}

inline void mglUniform2i (GLint location, GLint v0, GLint v1)
{
    GLCALL(glUniform2i, location, v0, v1);
}

inline void mglUniform3i (GLint location, GLint v0, GLint v1, GLint v2)
{
    GLCALL(glUniform3i, location, v0, v1, v2);
}

inline void mglUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLCALL(glUniform4i, location, v0, v1, v2, v3);
}

inline void mglUniform1fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform1fv, location, count, value);
}

inline void mglUniform2fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform2fv, location, count, value);
}

inline void mglUniform3fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform3fv, location, count, value);
}

inline void mglUniform4fv (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform4fv, location, count, value);
}

inline void mglUniform1iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform1iv, location, count, value);
}

inline void mglUniform2iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform2iv, location, count, value);
}

inline void mglUniform3iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform3iv, location, count, value);
}

inline void mglUniform4iv (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform4iv, location, count, value);
}

inline void mglUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2fv, location, count, transpose, value);
}

inline void mglUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3fv, location, count, transpose, value);
}

inline void mglUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4fv, location, count, transpose, value);
}

inline void mglValidateProgram (GLuint program)
{
    GLCALL(glValidateProgram, program);
}

inline void mglVertexAttrib1d (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1d, index, x);
}

inline void mglVertexAttrib1dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dv, index, v);
}

inline void mglVertexAttrib1f (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1f, index, x);
}

inline void mglVertexAttrib1fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fv, index, v);
}

inline void mglVertexAttrib1s (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1s, index, x);
}

inline void mglVertexAttrib1sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1sv, index, v);
}

inline void mglVertexAttrib2d (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2d, index, x, y);
}

inline void mglVertexAttrib2dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dv, index, v);
}

inline void mglVertexAttrib2f (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2f, index, x, y);
}

inline void mglVertexAttrib2fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fv, index, v);
}

inline void mglVertexAttrib2s (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2s, index, x, y);
}

inline void mglVertexAttrib2sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2sv, index, v);
}

inline void mglVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3d, index, x, y, z);
}

inline void mglVertexAttrib3dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dv, index, v);
}

inline void mglVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3f, index, x, y, z);
}

inline void mglVertexAttrib3fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fv, index, v);
}

inline void mglVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3s, index, x, y, z);
}

inline void mglVertexAttrib3sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3sv, index, v);
}

inline void mglVertexAttrib4Nbv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4Nbv, index, v);
}

inline void mglVertexAttrib4Niv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4Niv, index, v);
}

inline void mglVertexAttrib4Nsv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4Nsv, index, v);
}

inline void mglVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4Nub, index, x, y, z, w);
}

inline void mglVertexAttrib4Nubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4Nubv, index, v);
}

inline void mglVertexAttrib4Nuiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4Nuiv, index, v);
}

inline void mglVertexAttrib4Nusv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4Nusv, index, v);
}

inline void mglVertexAttrib4bv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4bv, index, v);
}

inline void mglVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4d, index, x, y, z, w);
}

inline void mglVertexAttrib4dv (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dv, index, v);
}

inline void mglVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4f, index, x, y, z, w);
}

inline void mglVertexAttrib4fv (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fv, index, v);
}

inline void mglVertexAttrib4iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4iv, index, v);
}

inline void mglVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4s, index, x, y, z, w);
}

inline void mglVertexAttrib4sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4sv, index, v);
}

inline void mglVertexAttrib4ubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubv, index, v);
}

inline void mglVertexAttrib4uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4uiv, index, v);
}

inline void mglVertexAttrib4usv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4usv, index, v);
}

inline void mglVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointer, index, size, type, normalized, stride, pointer);
}

inline void mglUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2x3fv, location, count, transpose, value);
}

inline void mglUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3x2fv, location, count, transpose, value);
}

inline void mglUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2x4fv, location, count, transpose, value);
}

inline void mglUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4x2fv, location, count, transpose, value);
}

inline void mglUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3x4fv, location, count, transpose, value);
}

inline void mglUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4x3fv, location, count, transpose, value);
}

inline void mglLoadTransposeMatrixfARB (const GLfloat * m)
{
    GLCALL(glLoadTransposeMatrixfARB, m);
}

inline void mglLoadTransposeMatrixdARB (const GLdouble * m)
{
    GLCALL(glLoadTransposeMatrixdARB, m);
}

inline void mglMultTransposeMatrixfARB (const GLfloat * m)
{
    GLCALL(glMultTransposeMatrixfARB, m);
}

inline void mglMultTransposeMatrixdARB (const GLdouble * m)
{
    GLCALL(glMultTransposeMatrixdARB, m);
}

inline void mglSampleCoverageARB (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleCoverageARB, value, invert);
}

inline void mglCompressedTexImage3DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage3DARB, target, level, internalformat, width, height, depth, border, imageSize, data);
}

inline void mglCompressedTexImage2DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage2DARB, target, level, internalformat, width, height, border, imageSize, data);
}

inline void mglCompressedTexImage1DARB (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexImage1DARB, target, level, internalformat, width, border, imageSize, data);
}

inline void mglCompressedTexSubImage3DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage3DARB, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

inline void mglCompressedTexSubImage2DARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage2DARB, target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

inline void mglCompressedTexSubImage1DARB (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data)
{
    GLCALL(glCompressedTexSubImage1DARB, target, level, xoffset, width, format, imageSize, data);
}

inline void mglGetCompressedTexImageARB (GLenum target, GLint level, GLvoid * img)
{
    GLCALL(glGetCompressedTexImageARB, target, level, img);
}

inline void mglPointParameterfARB (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfARB, pname, param);
}

inline void mglPointParameterfvARB (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvARB, pname, params);
}

inline void mglWindowPos2dARB (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2dARB, x, y);
}

inline void mglWindowPos2dvARB (const GLdouble * v)
{
    GLCALL(glWindowPos2dvARB, v);
}

inline void mglWindowPos2fARB (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2fARB, x, y);
}

inline void mglWindowPos2fvARB (const GLfloat * v)
{
    GLCALL(glWindowPos2fvARB, v);
}

inline void mglWindowPos2iARB (GLint x, GLint y)
{
    GLCALL(glWindowPos2iARB, x, y);
}

inline void mglWindowPos2ivARB (const GLint * v)
{
    GLCALL(glWindowPos2ivARB, v);
}

inline void mglWindowPos2sARB (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2sARB, x, y);
}

inline void mglWindowPos2svARB (const GLshort * v)
{
    GLCALL(glWindowPos2svARB, v);
}

inline void mglWindowPos3dARB (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3dARB, x, y, z);
}

inline void mglWindowPos3dvARB (const GLdouble * v)
{
    GLCALL(glWindowPos3dvARB, v);
}

inline void mglWindowPos3fARB (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3fARB, x, y, z);
}

inline void mglWindowPos3fvARB (const GLfloat * v)
{
    GLCALL(glWindowPos3fvARB, v);
}

inline void mglWindowPos3iARB (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3iARB, x, y, z);
}

inline void mglWindowPos3ivARB (const GLint * v)
{
    GLCALL(glWindowPos3ivARB, v);
}

inline void mglWindowPos3sARB (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3sARB, x, y, z);
}

inline void mglWindowPos3svARB (const GLshort * v)
{
    GLCALL(glWindowPos3svARB, v);
}

inline void mglVertexAttrib1dARB (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1dARB, index, x);
}

inline void mglVertexAttrib1dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dvARB, index, v);
}

inline void mglVertexAttrib1fARB (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1fARB, index, x);
}

inline void mglVertexAttrib1fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fvARB, index, v);
}

inline void mglVertexAttrib1sARB (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1sARB, index, x);
}

inline void mglVertexAttrib1svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1svARB, index, v);
}

inline void mglVertexAttrib2dARB (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2dARB, index, x, y);
}

inline void mglVertexAttrib2dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dvARB, index, v);
}

inline void mglVertexAttrib2fARB (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2fARB, index, x, y);
}

inline void mglVertexAttrib2fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fvARB, index, v);
}

inline void mglVertexAttrib2sARB (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2sARB, index, x, y);
}

inline void mglVertexAttrib2svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2svARB, index, v);
}

inline void mglVertexAttrib3dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3dARB, index, x, y, z);
}

inline void mglVertexAttrib3dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dvARB, index, v);
}

inline void mglVertexAttrib3fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3fARB, index, x, y, z);
}

inline void mglVertexAttrib3fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fvARB, index, v);
}

inline void mglVertexAttrib3sARB (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3sARB, index, x, y, z);
}

inline void mglVertexAttrib3svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3svARB, index, v);
}

inline void mglVertexAttrib4NbvARB (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4NbvARB, index, v);
}

inline void mglVertexAttrib4NivARB (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4NivARB, index, v);
}

inline void mglVertexAttrib4NsvARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4NsvARB, index, v);
}

inline void mglVertexAttrib4NubARB (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4NubARB, index, x, y, z, w);
}

inline void mglVertexAttrib4NubvARB (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4NubvARB, index, v);
}

inline void mglVertexAttrib4NuivARB (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4NuivARB, index, v);
}

inline void mglVertexAttrib4NusvARB (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4NusvARB, index, v);
}

inline void mglVertexAttrib4bvARB (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttrib4bvARB, index, v);
}

inline void mglVertexAttrib4dARB (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4dARB, index, x, y, z, w);
}

inline void mglVertexAttrib4dvARB (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dvARB, index, v);
}

inline void mglVertexAttrib4fARB (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4fARB, index, x, y, z, w);
}

inline void mglVertexAttrib4fvARB (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fvARB, index, v);
}

inline void mglVertexAttrib4ivARB (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttrib4ivARB, index, v);
}

inline void mglVertexAttrib4sARB (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4sARB, index, x, y, z, w);
}

inline void mglVertexAttrib4svARB (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4svARB, index, v);
}

inline void mglVertexAttrib4ubvARB (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubvARB, index, v);
}

inline void mglVertexAttrib4uivARB (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttrib4uivARB, index, v);
}

inline void mglVertexAttrib4usvARB (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttrib4usvARB, index, v);
}

inline void mglVertexAttribPointerARB (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointerARB, index, size, type, normalized, stride, pointer);
}

inline void mglEnableVertexAttribArrayARB (GLuint index)
{
    GLCALL(glEnableVertexAttribArrayARB, index);
}

inline void mglDisableVertexAttribArrayARB (GLuint index)
{
    GLCALL(glDisableVertexAttribArrayARB, index);
}

inline void mglProgramStringARB (GLenum target, GLenum format, GLsizei len, const GLvoid * string)
{
    GLCALL(glProgramStringARB, target, format, len, string);
}

inline void mglBindProgramARB (GLenum target, GLuint program)
{
    GLCALL(glBindProgramARB, target, program);
}

inline void mglDeleteProgramsARB (GLsizei n, const GLuint * programs)
{
    GLCALL(glDeleteProgramsARB, n, programs);
}

inline void mglGenProgramsARB (GLsizei n, GLuint * programs)
{
    GLCALL(glGenProgramsARB, n, programs);
}

inline void mglProgramEnvParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramEnvParameter4dARB, target, index, x, y, z, w);
}

inline void mglProgramEnvParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    GLCALL(glProgramEnvParameter4dvARB, target, index, params);
}

inline void mglProgramEnvParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramEnvParameter4fARB, target, index, x, y, z, w);
}

inline void mglProgramEnvParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    GLCALL(glProgramEnvParameter4fvARB, target, index, params);
}

inline void mglProgramLocalParameter4dARB (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramLocalParameter4dARB, target, index, x, y, z, w);
}

inline void mglProgramLocalParameter4dvARB (GLenum target, GLuint index, const GLdouble * params)
{
    GLCALL(glProgramLocalParameter4dvARB, target, index, params);
}

inline void mglProgramLocalParameter4fARB (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramLocalParameter4fARB, target, index, x, y, z, w);
}

inline void mglProgramLocalParameter4fvARB (GLenum target, GLuint index, const GLfloat * params)
{
    GLCALL(glProgramLocalParameter4fvARB, target, index, params);
}

inline void mglGetProgramEnvParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    GLCALL(glGetProgramEnvParameterdvARB, target, index, params);
}

inline void mglGetProgramEnvParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    GLCALL(glGetProgramEnvParameterfvARB, target, index, params);
}

inline void mglGetProgramLocalParameterdvARB (GLenum target, GLuint index, GLdouble * params)
{
    GLCALL(glGetProgramLocalParameterdvARB, target, index, params);
}

inline void mglGetProgramLocalParameterfvARB (GLenum target, GLuint index, GLfloat * params)
{
    GLCALL(glGetProgramLocalParameterfvARB, target, index, params);
}

inline void mglGetProgramivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramivARB, target, pname, params);
}

inline void mglGetProgramStringARB (GLenum target, GLenum pname, GLvoid * string)
{
    GLCALL(glGetProgramStringARB, target, pname, string);
}

inline void mglGetVertexAttribdvARB (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdvARB, index, pname, params);
}

inline void mglGetVertexAttribfvARB (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfvARB, index, pname, params);
}

inline void mglGetVertexAttribivARB (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribivARB, index, pname, params);
}

inline void mglGetVertexAttribPointervARB (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointervARB, index, pname, pointer);
}

inline GLboolean mglIsProgramARB (GLuint program)
{
    return GLCALL(glIsProgramARB, program);
}

inline void mglBindBufferARB (GLenum target, GLuint buffer)
{
    GLCALL(glBindBufferARB, target, buffer);
}

inline void mglDeleteBuffersARB (GLsizei n, const GLuint * buffers)
{
    GLCALL(glDeleteBuffersARB, n, buffers);
}

inline void mglGenBuffersARB (GLsizei n, GLuint * buffers)
{
    GLCALL(glGenBuffersARB, n, buffers);
}

inline GLboolean mglIsBufferARB (GLuint buffer)
{
    return GLCALL(glIsBufferARB, buffer);
}

inline void mglBufferDataARB (GLenum target, GLsizeiptrARB size, const GLvoid * data, GLenum usage)
{
    GLCALL(glBufferDataARB, target, size, data, usage);
}

inline void mglBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid * data)
{
    GLCALL(glBufferSubDataARB, target, offset, size, data);
}

inline void mglGetBufferSubDataARB (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid * data)
{
    GLCALL(glGetBufferSubDataARB, target, offset, size, data);
}

inline GLvoid* mglMapBufferARB (GLenum target, GLenum access)
{
    return GLCALL(glMapBufferARB, target, access);
}

inline GLboolean mglUnmapBufferARB (GLenum target)
{
    return GLCALL(glUnmapBufferARB, target);
}

inline void mglGetBufferParameterivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetBufferParameterivARB, target, pname, params);
}

inline void mglGetBufferPointervARB (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetBufferPointervARB, target, pname, params);
}

inline void mglGenQueriesARB (GLsizei n, GLuint * ids)
{
    GLCALL(glGenQueriesARB, n, ids);
}

inline void mglDeleteQueriesARB (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteQueriesARB, n, ids);
}

inline GLboolean mglIsQueryARB (GLuint id)
{
    return GLCALL(glIsQueryARB, id);
}

inline void mglBeginQueryARB (GLenum target, GLuint id)
{
    GLCALL(glBeginQueryARB, target, id);
}

inline void mglEndQueryARB (GLenum target)
{
    GLCALL(glEndQueryARB, target);
}

inline void mglGetQueryivARB (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryivARB, target, pname, params);
}

inline void mglGetQueryObjectivARB (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetQueryObjectivARB, id, pname, params);
}

inline void mglGetQueryObjectuivARB (GLuint id, GLenum pname, GLuint * params)
{
    GLCALL(glGetQueryObjectuivARB, id, pname, params);
}

inline void mglDeleteObjectARB (GLhandleARB obj)
{
    GLCALL(glDeleteObjectARB, obj);
}

inline GLhandleARB mglGetHandleARB (GLenum pname)
{
    return GLCALL(glGetHandleARB, pname);
}

inline void mglDetachObjectARB (GLhandleARB containerObj, GLhandleARB attachedObj)
{
    GLCALL(glDetachObjectARB, containerObj, attachedObj);
}

inline GLhandleARB mglCreateShaderObjectARB (GLenum shaderType)
{
    return GLCALL(glCreateShaderObjectARB, shaderType);
}

inline void mglShaderSourceARB (GLhandleARB shaderObj, GLsizei count, const GLcharARB *  * string, const GLint * length)
{
    GLCALL(glShaderSourceARB, shaderObj, count, string, length);
}

inline void mglCompileShaderARB (GLhandleARB shaderObj)
{
    GLCALL(glCompileShaderARB, shaderObj);
}

inline GLhandleARB mglCreateProgramObjectARB ()
{
    return GLCALL(glCreateProgramObjectARB);
}

inline void mglAttachObjectARB (GLhandleARB containerObj, GLhandleARB obj)
{
    GLCALL(glAttachObjectARB, containerObj, obj);
}

inline void mglLinkProgramARB (GLhandleARB programObj)
{
    GLCALL(glLinkProgramARB, programObj);
}

inline void mglUseProgramObjectARB (GLhandleARB programObj)
{
    GLCALL(glUseProgramObjectARB, programObj);
}

inline void mglValidateProgramARB (GLhandleARB programObj)
{
    GLCALL(glValidateProgramARB, programObj);
}

inline void mglUniform1fARB (GLint location, GLfloat v0)
{
    GLCALL(glUniform1fARB, location, v0);
}

inline void mglUniform2fARB (GLint location, GLfloat v0, GLfloat v1)
{
    GLCALL(glUniform2fARB, location, v0, v1);
}

inline void mglUniform3fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
{
    GLCALL(glUniform3fARB, location, v0, v1, v2);
}

inline void mglUniform4fARB (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
    GLCALL(glUniform4fARB, location, v0, v1, v2, v3);
}

inline void mglUniform1iARB (GLint location, GLint v0)
{
    GLCALL(glUniform1iARB, location, v0);
}

inline void mglUniform2iARB (GLint location, GLint v0, GLint v1)
{
    GLCALL(glUniform2iARB, location, v0, v1);
}

inline void mglUniform3iARB (GLint location, GLint v0, GLint v1, GLint v2)
{
    GLCALL(glUniform3iARB, location, v0, v1, v2);
}

inline void mglUniform4iARB (GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
{
    GLCALL(glUniform4iARB, location, v0, v1, v2, v3);
}

inline void mglUniform1fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform1fvARB, location, count, value);
}

inline void mglUniform2fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform2fvARB, location, count, value);
}

inline void mglUniform3fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform3fvARB, location, count, value);
}

inline void mglUniform4fvARB (GLint location, GLsizei count, const GLfloat * value)
{
    GLCALL(glUniform4fvARB, location, count, value);
}

inline void mglUniform1ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform1ivARB, location, count, value);
}

inline void mglUniform2ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform2ivARB, location, count, value);
}

inline void mglUniform3ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform3ivARB, location, count, value);
}

inline void mglUniform4ivARB (GLint location, GLsizei count, const GLint * value)
{
    GLCALL(glUniform4ivARB, location, count, value);
}

inline void mglUniformMatrix2fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix2fvARB, location, count, transpose, value);
}

inline void mglUniformMatrix3fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix3fvARB, location, count, transpose, value);
}

inline void mglUniformMatrix4fvARB (GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
    GLCALL(glUniformMatrix4fvARB, location, count, transpose, value);
}

inline void mglGetObjectParameterfvARB (GLhandleARB obj, GLenum pname, GLfloat * params)
{
    GLCALL(glGetObjectParameterfvARB, obj, pname, params);
}

inline void mglGetObjectParameterivARB (GLhandleARB obj, GLenum pname, GLint * params)
{
    GLCALL(glGetObjectParameterivARB, obj, pname, params);
}

inline void mglGetInfoLogARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * infoLog)
{
    GLCALL(glGetInfoLogARB, obj, maxLength, length, infoLog);
}

inline void mglGetAttachedObjectsARB (GLhandleARB containerObj, GLsizei maxCount, GLsizei * count, GLhandleARB * obj)
{
    GLCALL(glGetAttachedObjectsARB, containerObj, maxCount, count, obj);
}

inline GLint mglGetUniformLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    return GLCALL(glGetUniformLocationARB, programObj, name);
}

inline void mglGetActiveUniformARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    GLCALL(glGetActiveUniformARB, programObj, index, maxLength, length, size, type, name);
}

inline void mglGetUniformfvARB (GLhandleARB programObj, GLint location, GLfloat * params)
{
    GLCALL(glGetUniformfvARB, programObj, location, params);
}

inline void mglGetUniformivARB (GLhandleARB programObj, GLint location, GLint * params)
{
    GLCALL(glGetUniformivARB, programObj, location, params);
}

inline void mglGetShaderSourceARB (GLhandleARB obj, GLsizei maxLength, GLsizei * length, GLcharARB * source)
{
    GLCALL(glGetShaderSourceARB, obj, maxLength, length, source);
}

inline void mglBindAttribLocationARB (GLhandleARB programObj, GLuint index, const GLcharARB * name)
{
    GLCALL(glBindAttribLocationARB, programObj, index, name);
}

inline void mglGetActiveAttribARB (GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei * length, GLint * size, GLenum * type, GLcharARB * name)
{
    GLCALL(glGetActiveAttribARB, programObj, index, maxLength, length, size, type, name);
}

inline GLint mglGetAttribLocationARB (GLhandleARB programObj, const GLcharARB * name)
{
    return GLCALL(glGetAttribLocationARB, programObj, name);
}

inline void mglDrawBuffersARB (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffersARB, n, bufs);
}

inline GLboolean mglIsRenderbuffer (GLuint renderbuffer)
{
    return GLCALL(glIsRenderbuffer, renderbuffer);
}

inline void mglBindRenderbuffer (GLenum target, GLuint renderbuffer)
{
    GLCALL(glBindRenderbuffer, target, renderbuffer);
}

inline void mglDeleteRenderbuffers (GLsizei n, const GLuint * renderbuffers)
{
    GLCALL(glDeleteRenderbuffers, n, renderbuffers);
}

inline void mglGenRenderbuffers (GLsizei n, GLuint * renderbuffers)
{
    GLCALL(glGenRenderbuffers, n, renderbuffers);
}

inline void mglRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorage, target, internalformat, width, height);
}

inline void mglGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetRenderbufferParameteriv, target, pname, params);
}

inline GLboolean mglIsFramebuffer (GLuint framebuffer)
{
    return GLCALL(glIsFramebuffer, framebuffer);
}

inline void mglBindFramebuffer (GLenum target, GLuint framebuffer)
{
    GLCALL(glBindFramebuffer, target, framebuffer);
}

inline void mglDeleteFramebuffers (GLsizei n, const GLuint * framebuffers)
{
    GLCALL(glDeleteFramebuffers, n, framebuffers);
}

inline void mglGenFramebuffers (GLsizei n, GLuint * framebuffers)
{
    GLCALL(glGenFramebuffers, n, framebuffers);
}

inline GLenum mglCheckFramebufferStatus (GLenum target)
{
    return GLCALL(glCheckFramebufferStatus, target);
}

inline void mglFramebufferTexture1D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture1D, target, attachment, textarget, texture, level);
}

inline void mglFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture2D, target, attachment, textarget, texture, level);
}

inline void mglFramebufferTexture3D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    GLCALL(glFramebufferTexture3D, target, attachment, textarget, texture, level, zoffset);
}

inline void mglFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    GLCALL(glFramebufferRenderbuffer, target, attachment, renderbuffertarget, renderbuffer);
}

inline void mglGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    GLCALL(glGetFramebufferAttachmentParameteriv, target, attachment, pname, params);
}

inline void mglGenerateMipmap (GLenum target)
{
    GLCALL(glGenerateMipmap, target);
}

inline void mglBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    GLCALL(glBlitFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

inline void mglRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageMultisample, target, samples, internalformat, width, height);
}

inline void mglFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GLCALL(glFramebufferTextureLayer, target, attachment, texture, level, layer);
}

inline void mglBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    GLCALL(glBlendColorEXT, red, green, blue, alpha);
}

inline void mglPolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    GLCALL(glPolygonOffsetEXT, factor, bias);
}

inline void mglTexImage3DEXT (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexImage3DEXT, target, level, internalformat, width, height, depth, border, format, type, pixels);
}

inline void mglTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

inline void mglTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage1DEXT, target, level, xoffset, width, format, type, pixels);
}

inline void mglTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels)
{
    GLCALL(glTexSubImage2DEXT, target, level, xoffset, yoffset, width, height, format, type, pixels);
}

inline void mglCopyTexImage1DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    GLCALL(glCopyTexImage1DEXT, target, level, internalformat, x, y, width, border);
}

inline void mglCopyTexImage2DEXT (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    GLCALL(glCopyTexImage2DEXT, target, level, internalformat, x, y, width, height, border);
}

inline void mglCopyTexSubImage1DEXT (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyTexSubImage1DEXT, target, level, xoffset, x, y, width);
}

inline void mglCopyTexSubImage2DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage2DEXT, target, level, xoffset, yoffset, x, y, width, height);
}

inline void mglCopyTexSubImage3DEXT (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyTexSubImage3DEXT, target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

inline GLboolean mglAreTexturesResidentEXT (GLsizei n, const GLuint * textures, GLboolean * residences)
{
    return GLCALL(glAreTexturesResidentEXT, n, textures, residences);
}

inline void mglBindTextureEXT (GLenum target, GLuint texture)
{
    GLCALL(glBindTextureEXT, target, texture);
}

inline void mglDeleteTexturesEXT (GLsizei n, const GLuint * textures)
{
    GLCALL(glDeleteTexturesEXT, n, textures);
}

inline void mglGenTexturesEXT (GLsizei n, GLuint * textures)
{
    GLCALL(glGenTexturesEXT, n, textures);
}

inline GLboolean mglIsTextureEXT (GLuint texture)
{
    return GLCALL(glIsTextureEXT, texture);
}

inline void mglPrioritizeTexturesEXT (GLsizei n, const GLuint * textures, const GLclampf * priorities)
{
    GLCALL(glPrioritizeTexturesEXT, n, textures, priorities);
}

inline void mglArrayElementEXT (GLint i)
{
    GLCALL(glArrayElementEXT, i);
}

inline void mglColorPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glColorPointerEXT, size, type, stride, count, pointer);
}

inline void mglDrawArraysEXT (GLenum mode, GLint first, GLsizei count)
{
    GLCALL(glDrawArraysEXT, mode, first, count);
}

inline void mglEdgeFlagPointerEXT (GLsizei stride, GLsizei count, const GLboolean * pointer)
{
    GLCALL(glEdgeFlagPointerEXT, stride, count, pointer);
}

inline void mglGetPointervEXT (GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetPointervEXT, pname, params);
}

inline void mglIndexPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glIndexPointerEXT, type, stride, count, pointer);
}

inline void mglNormalPointerEXT (GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glNormalPointerEXT, type, stride, count, pointer);
}

inline void mglTexCoordPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glTexCoordPointerEXT, size, type, stride, count, pointer);
}

inline void mglVertexPointerEXT (GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid * pointer)
{
    GLCALL(glVertexPointerEXT, size, type, stride, count, pointer);
}

inline void mglBlendEquationEXT (GLenum mode)
{
    GLCALL(glBlendEquationEXT, mode);
}

inline void mglPointParameterfEXT (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfEXT, pname, param);
}

inline void mglPointParameterfvEXT (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvEXT, pname, params);
}

inline void mglColorTableEXT (GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTableEXT, target, internalFormat, width, format, type, table);
}

inline void mglGetColorTableEXT (GLenum target, GLenum format, GLenum type, GLvoid * data)
{
    GLCALL(glGetColorTableEXT, target, format, type, data);
}

inline void mglGetColorTableParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameterivEXT, target, pname, params);
}

inline void mglGetColorTableParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfvEXT, target, pname, params);
}

inline void mglLockArraysEXT (GLint first, GLsizei count)
{
    GLCALL(glLockArraysEXT, first, count);
}

inline void mglUnlockArraysEXT ()
{
    GLCALL(glUnlockArraysEXT);
}

inline void mglDrawRangeElementsEXT (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices)
{
    GLCALL(glDrawRangeElementsEXT, mode, start, end, count, type, indices);
}

inline void mglSecondaryColor3bEXT (GLbyte red, GLbyte green, GLbyte blue)
{
    GLCALL(glSecondaryColor3bEXT, red, green, blue);
}

inline void mglSecondaryColor3bvEXT (const GLbyte * v)
{
    GLCALL(glSecondaryColor3bvEXT, v);
}

inline void mglSecondaryColor3dEXT (GLdouble red, GLdouble green, GLdouble blue)
{
    GLCALL(glSecondaryColor3dEXT, red, green, blue);
}

inline void mglSecondaryColor3dvEXT (const GLdouble * v)
{
    GLCALL(glSecondaryColor3dvEXT, v);
}

inline void mglSecondaryColor3fEXT (GLfloat red, GLfloat green, GLfloat blue)
{
    GLCALL(glSecondaryColor3fEXT, red, green, blue);
}

inline void mglSecondaryColor3fvEXT (const GLfloat * v)
{
    GLCALL(glSecondaryColor3fvEXT, v);
}

inline void mglSecondaryColor3iEXT (GLint red, GLint green, GLint blue)
{
    GLCALL(glSecondaryColor3iEXT, red, green, blue);
}

inline void mglSecondaryColor3ivEXT (const GLint * v)
{
    GLCALL(glSecondaryColor3ivEXT, v);
}

inline void mglSecondaryColor3sEXT (GLshort red, GLshort green, GLshort blue)
{
    GLCALL(glSecondaryColor3sEXT, red, green, blue);
}

inline void mglSecondaryColor3svEXT (const GLshort * v)
{
    GLCALL(glSecondaryColor3svEXT, v);
}

inline void mglSecondaryColor3ubEXT (GLubyte red, GLubyte green, GLubyte blue)
{
    GLCALL(glSecondaryColor3ubEXT, red, green, blue);
}

inline void mglSecondaryColor3ubvEXT (const GLubyte * v)
{
    GLCALL(glSecondaryColor3ubvEXT, v);
}

inline void mglSecondaryColor3uiEXT (GLuint red, GLuint green, GLuint blue)
{
    GLCALL(glSecondaryColor3uiEXT, red, green, blue);
}

inline void mglSecondaryColor3uivEXT (const GLuint * v)
{
    GLCALL(glSecondaryColor3uivEXT, v);
}

inline void mglSecondaryColor3usEXT (GLushort red, GLushort green, GLushort blue)
{
    GLCALL(glSecondaryColor3usEXT, red, green, blue);
}

inline void mglSecondaryColor3usvEXT (const GLushort * v)
{
    GLCALL(glSecondaryColor3usvEXT, v);
}

inline void mglSecondaryColorPointerEXT (GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glSecondaryColorPointerEXT, size, type, stride, pointer);
}

inline void mglMultiDrawArraysEXT (GLenum mode, const GLint * first, const GLsizei * count, GLsizei primcount)
{
    GLCALL(glMultiDrawArraysEXT, mode, first, count, primcount);
}

inline void mglMultiDrawElementsEXT (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount)
{
    GLCALL(glMultiDrawElementsEXT, mode, count, type, indices, primcount);
}

inline void mglFogCoordfEXT (GLfloat coord)
{
    GLCALL(glFogCoordfEXT, coord);
}

inline void mglFogCoordfvEXT (const GLfloat * coord)
{
    GLCALL(glFogCoordfvEXT, coord);
}

inline void mglFogCoorddEXT (GLdouble coord)
{
    GLCALL(glFogCoorddEXT, coord);
}

inline void mglFogCoorddvEXT (const GLdouble * coord)
{
    GLCALL(glFogCoorddvEXT, coord);
}

inline void mglFogCoordPointerEXT (GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glFogCoordPointerEXT, type, stride, pointer);
}

inline void mglBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparateEXT, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

inline void mglFlushVertexArrayRangeNV ()
{
    GLCALL(glFlushVertexArrayRangeNV);
}

inline void mglVertexArrayRangeNV (GLsizei length, const GLvoid * pointer)
{
    GLCALL(glVertexArrayRangeNV, length, pointer);
}

inline void mglCombinerParameterfvNV (GLenum pname, const GLfloat * params)
{
    GLCALL(glCombinerParameterfvNV, pname, params);
}

inline void mglCombinerParameterfNV (GLenum pname, GLfloat param)
{
    GLCALL(glCombinerParameterfNV, pname, param);
}

inline void mglCombinerParameterivNV (GLenum pname, const GLint * params)
{
    GLCALL(glCombinerParameterivNV, pname, params);
}

inline void mglCombinerParameteriNV (GLenum pname, GLint param)
{
    GLCALL(glCombinerParameteriNV, pname, param);
}

inline void mglCombinerInputNV (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    GLCALL(glCombinerInputNV, stage, portion, variable, input, mapping, componentUsage);
}

inline void mglCombinerOutputNV (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum)
{
    GLCALL(glCombinerOutputNV, stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum);
}

inline void mglFinalCombinerInputNV (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage)
{
    GLCALL(glFinalCombinerInputNV, variable, input, mapping, componentUsage);
}

inline void mglGetCombinerInputParameterfvNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat * params)
{
    GLCALL(glGetCombinerInputParameterfvNV, stage, portion, variable, pname, params);
}

inline void mglGetCombinerInputParameterivNV (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint * params)
{
    GLCALL(glGetCombinerInputParameterivNV, stage, portion, variable, pname, params);
}

inline void mglGetCombinerOutputParameterfvNV (GLenum stage, GLenum portion, GLenum pname, GLfloat * params)
{
    GLCALL(glGetCombinerOutputParameterfvNV, stage, portion, pname, params);
}

inline void mglGetCombinerOutputParameterivNV (GLenum stage, GLenum portion, GLenum pname, GLint * params)
{
    GLCALL(glGetCombinerOutputParameterivNV, stage, portion, pname, params);
}

inline void mglGetFinalCombinerInputParameterfvNV (GLenum variable, GLenum pname, GLfloat * params)
{
    GLCALL(glGetFinalCombinerInputParameterfvNV, variable, pname, params);
}

inline void mglGetFinalCombinerInputParameterivNV (GLenum variable, GLenum pname, GLint * params)
{
    GLCALL(glGetFinalCombinerInputParameterivNV, variable, pname, params);
}

inline void mglResizeBuffersMESA ()
{
    GLCALL(glResizeBuffersMESA);
}

inline void mglWindowPos2dMESA (GLdouble x, GLdouble y)
{
    GLCALL(glWindowPos2dMESA, x, y);
}

inline void mglWindowPos2dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos2dvMESA, v);
}

inline void mglWindowPos2fMESA (GLfloat x, GLfloat y)
{
    GLCALL(glWindowPos2fMESA, x, y);
}

inline void mglWindowPos2fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos2fvMESA, v);
}

inline void mglWindowPos2iMESA (GLint x, GLint y)
{
    GLCALL(glWindowPos2iMESA, x, y);
}

inline void mglWindowPos2ivMESA (const GLint * v)
{
    GLCALL(glWindowPos2ivMESA, v);
}

inline void mglWindowPos2sMESA (GLshort x, GLshort y)
{
    GLCALL(glWindowPos2sMESA, x, y);
}

inline void mglWindowPos2svMESA (const GLshort * v)
{
    GLCALL(glWindowPos2svMESA, v);
}

inline void mglWindowPos3dMESA (GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glWindowPos3dMESA, x, y, z);
}

inline void mglWindowPos3dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos3dvMESA, v);
}

inline void mglWindowPos3fMESA (GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glWindowPos3fMESA, x, y, z);
}

inline void mglWindowPos3fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos3fvMESA, v);
}

inline void mglWindowPos3iMESA (GLint x, GLint y, GLint z)
{
    GLCALL(glWindowPos3iMESA, x, y, z);
}

inline void mglWindowPos3ivMESA (const GLint * v)
{
    GLCALL(glWindowPos3ivMESA, v);
}

inline void mglWindowPos3sMESA (GLshort x, GLshort y, GLshort z)
{
    GLCALL(glWindowPos3sMESA, x, y, z);
}

inline void mglWindowPos3svMESA (const GLshort * v)
{
    GLCALL(glWindowPos3svMESA, v);
}

inline void mglWindowPos4dMESA (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glWindowPos4dMESA, x, y, z, w);
}

inline void mglWindowPos4dvMESA (const GLdouble * v)
{
    GLCALL(glWindowPos4dvMESA, v);
}

inline void mglWindowPos4fMESA (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glWindowPos4fMESA, x, y, z, w);
}

inline void mglWindowPos4fvMESA (const GLfloat * v)
{
    GLCALL(glWindowPos4fvMESA, v);
}

inline void mglWindowPos4iMESA (GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glWindowPos4iMESA, x, y, z, w);
}

inline void mglWindowPos4ivMESA (const GLint * v)
{
    GLCALL(glWindowPos4ivMESA, v);
}

inline void mglWindowPos4sMESA (GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glWindowPos4sMESA, x, y, z, w);
}

inline void mglWindowPos4svMESA (const GLshort * v)
{
    GLCALL(glWindowPos4svMESA, v);
}

inline GLboolean mglAreProgramsResidentNV (GLsizei n, const GLuint * programs, GLboolean * residences)
{
    return GLCALL(glAreProgramsResidentNV, n, programs, residences);
}

inline void mglBindProgramNV (GLenum target, GLuint id)
{
    GLCALL(glBindProgramNV, target, id);
}

inline void mglDeleteProgramsNV (GLsizei n, const GLuint * programs)
{
    GLCALL(glDeleteProgramsNV, n, programs);
}

inline void mglExecuteProgramNV (GLenum target, GLuint id, const GLfloat * params)
{
    GLCALL(glExecuteProgramNV, target, id, params);
}

inline void mglGenProgramsNV (GLsizei n, GLuint * programs)
{
    GLCALL(glGenProgramsNV, n, programs);
}

inline void mglGetProgramParameterdvNV (GLenum target, GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetProgramParameterdvNV, target, index, pname, params);
}

inline void mglGetProgramParameterfvNV (GLenum target, GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetProgramParameterfvNV, target, index, pname, params);
}

inline void mglGetProgramivNV (GLuint id, GLenum pname, GLint * params)
{
    GLCALL(glGetProgramivNV, id, pname, params);
}

inline void mglGetProgramStringNV (GLuint id, GLenum pname, GLubyte * program)
{
    GLCALL(glGetProgramStringNV, id, pname, program);
}

inline void mglGetTrackMatrixivNV (GLenum target, GLuint address, GLenum pname, GLint * params)
{
    GLCALL(glGetTrackMatrixivNV, target, address, pname, params);
}

inline void mglGetVertexAttribdvNV (GLuint index, GLenum pname, GLdouble * params)
{
    GLCALL(glGetVertexAttribdvNV, index, pname, params);
}

inline void mglGetVertexAttribfvNV (GLuint index, GLenum pname, GLfloat * params)
{
    GLCALL(glGetVertexAttribfvNV, index, pname, params);
}

inline void mglGetVertexAttribivNV (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribivNV, index, pname, params);
}

inline void mglGetVertexAttribPointervNV (GLuint index, GLenum pname, GLvoid *  * pointer)
{
    GLCALL(glGetVertexAttribPointervNV, index, pname, pointer);
}

inline GLboolean mglIsProgramNV (GLuint id)
{
    return GLCALL(glIsProgramNV, id);
}

inline void mglLoadProgramNV (GLenum target, GLuint id, GLsizei len, const GLubyte * program)
{
    GLCALL(glLoadProgramNV, target, id, len, program);
}

inline void mglProgramParameter4dNV (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramParameter4dNV, target, index, x, y, z, w);
}

inline void mglProgramParameter4dvNV (GLenum target, GLuint index, const GLdouble * v)
{
    GLCALL(glProgramParameter4dvNV, target, index, v);
}

inline void mglProgramParameter4fNV (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramParameter4fNV, target, index, x, y, z, w);
}

inline void mglProgramParameter4fvNV (GLenum target, GLuint index, const GLfloat * v)
{
    GLCALL(glProgramParameter4fvNV, target, index, v);
}

inline void mglProgramParameters4dvNV (GLenum target, GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glProgramParameters4dvNV, target, index, count, v);
}

inline void mglProgramParameters4fvNV (GLenum target, GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glProgramParameters4fvNV, target, index, count, v);
}

inline void mglRequestResidentProgramsNV (GLsizei n, const GLuint * programs)
{
    GLCALL(glRequestResidentProgramsNV, n, programs);
}

inline void mglTrackMatrixNV (GLenum target, GLuint address, GLenum matrix, GLenum transform)
{
    GLCALL(glTrackMatrixNV, target, address, matrix, transform);
}

inline void mglVertexAttribPointerNV (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribPointerNV, index, fsize, type, stride, pointer);
}

inline void mglVertexAttrib1dNV (GLuint index, GLdouble x)
{
    GLCALL(glVertexAttrib1dNV, index, x);
}

inline void mglVertexAttrib1dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib1dvNV, index, v);
}

inline void mglVertexAttrib1fNV (GLuint index, GLfloat x)
{
    GLCALL(glVertexAttrib1fNV, index, x);
}

inline void mglVertexAttrib1fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib1fvNV, index, v);
}

inline void mglVertexAttrib1sNV (GLuint index, GLshort x)
{
    GLCALL(glVertexAttrib1sNV, index, x);
}

inline void mglVertexAttrib1svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib1svNV, index, v);
}

inline void mglVertexAttrib2dNV (GLuint index, GLdouble x, GLdouble y)
{
    GLCALL(glVertexAttrib2dNV, index, x, y);
}

inline void mglVertexAttrib2dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib2dvNV, index, v);
}

inline void mglVertexAttrib2fNV (GLuint index, GLfloat x, GLfloat y)
{
    GLCALL(glVertexAttrib2fNV, index, x, y);
}

inline void mglVertexAttrib2fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib2fvNV, index, v);
}

inline void mglVertexAttrib2sNV (GLuint index, GLshort x, GLshort y)
{
    GLCALL(glVertexAttrib2sNV, index, x, y);
}

inline void mglVertexAttrib2svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib2svNV, index, v);
}

inline void mglVertexAttrib3dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z)
{
    GLCALL(glVertexAttrib3dNV, index, x, y, z);
}

inline void mglVertexAttrib3dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib3dvNV, index, v);
}

inline void mglVertexAttrib3fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z)
{
    GLCALL(glVertexAttrib3fNV, index, x, y, z);
}

inline void mglVertexAttrib3fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib3fvNV, index, v);
}

inline void mglVertexAttrib3sNV (GLuint index, GLshort x, GLshort y, GLshort z)
{
    GLCALL(glVertexAttrib3sNV, index, x, y, z);
}

inline void mglVertexAttrib3svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib3svNV, index, v);
}

inline void mglVertexAttrib4dNV (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glVertexAttrib4dNV, index, x, y, z, w);
}

inline void mglVertexAttrib4dvNV (GLuint index, const GLdouble * v)
{
    GLCALL(glVertexAttrib4dvNV, index, v);
}

inline void mglVertexAttrib4fNV (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glVertexAttrib4fNV, index, x, y, z, w);
}

inline void mglVertexAttrib4fvNV (GLuint index, const GLfloat * v)
{
    GLCALL(glVertexAttrib4fvNV, index, v);
}

inline void mglVertexAttrib4sNV (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w)
{
    GLCALL(glVertexAttrib4sNV, index, x, y, z, w);
}

inline void mglVertexAttrib4svNV (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttrib4svNV, index, v);
}

inline void mglVertexAttrib4ubNV (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w)
{
    GLCALL(glVertexAttrib4ubNV, index, x, y, z, w);
}

inline void mglVertexAttrib4ubvNV (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttrib4ubvNV, index, v);
}

inline void mglVertexAttribs1dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs1dvNV, index, count, v);
}

inline void mglVertexAttribs1fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs1fvNV, index, count, v);
}

inline void mglVertexAttribs1svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs1svNV, index, count, v);
}

inline void mglVertexAttribs2dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs2dvNV, index, count, v);
}

inline void mglVertexAttribs2fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs2fvNV, index, count, v);
}

inline void mglVertexAttribs2svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs2svNV, index, count, v);
}

inline void mglVertexAttribs3dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs3dvNV, index, count, v);
}

inline void mglVertexAttribs3fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs3fvNV, index, count, v);
}

inline void mglVertexAttribs3svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs3svNV, index, count, v);
}

inline void mglVertexAttribs4dvNV (GLuint index, GLsizei count, const GLdouble * v)
{
    GLCALL(glVertexAttribs4dvNV, index, count, v);
}

inline void mglVertexAttribs4fvNV (GLuint index, GLsizei count, const GLfloat * v)
{
    GLCALL(glVertexAttribs4fvNV, index, count, v);
}

inline void mglVertexAttribs4svNV (GLuint index, GLsizei count, const GLshort * v)
{
    GLCALL(glVertexAttribs4svNV, index, count, v);
}

inline void mglVertexAttribs4ubvNV (GLuint index, GLsizei count, const GLubyte * v)
{
    GLCALL(glVertexAttribs4ubvNV, index, count, v);
}

inline void mglTexBumpParameterivATI (GLenum pname, const GLint * param)
{
    GLCALL(glTexBumpParameterivATI, pname, param);
}

inline void mglTexBumpParameterfvATI (GLenum pname, const GLfloat * param)
{
    GLCALL(glTexBumpParameterfvATI, pname, param);
}

inline void mglGetTexBumpParameterivATI (GLenum pname, GLint * param)
{
    GLCALL(glGetTexBumpParameterivATI, pname, param);
}

inline void mglGetTexBumpParameterfvATI (GLenum pname, GLfloat * param)
{
    GLCALL(glGetTexBumpParameterfvATI, pname, param);
}

inline GLuint mglGenFragmentShadersATI (GLuint range)
{
    return GLCALL(glGenFragmentShadersATI, range);
}

inline void mglBindFragmentShaderATI (GLuint id)
{
    GLCALL(glBindFragmentShaderATI, id);
}

inline void mglDeleteFragmentShaderATI (GLuint id)
{
    GLCALL(glDeleteFragmentShaderATI, id);
}

inline void mglBeginFragmentShaderATI ()
{
    GLCALL(glBeginFragmentShaderATI);
}

inline void mglEndFragmentShaderATI ()
{
    GLCALL(glEndFragmentShaderATI);
}

inline void mglPassTexCoordATI (GLuint dst, GLuint coord, GLenum swizzle)
{
    GLCALL(glPassTexCoordATI, dst, coord, swizzle);
}

inline void mglSampleMapATI (GLuint dst, GLuint interp, GLenum swizzle)
{
    GLCALL(glSampleMapATI, dst, interp, swizzle);
}

inline void mglColorFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    GLCALL(glColorFragmentOp1ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod);
}

inline void mglColorFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    GLCALL(glColorFragmentOp2ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
}

inline void mglColorFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMask, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    GLCALL(glColorFragmentOp3ATI, op, dst, dstMask, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
}

inline void mglAlphaFragmentOp1ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod)
{
    GLCALL(glAlphaFragmentOp1ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod);
}

inline void mglAlphaFragmentOp2ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod)
{
    GLCALL(glAlphaFragmentOp2ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod);
}

inline void mglAlphaFragmentOp3ATI (GLenum op, GLuint dst, GLuint dstMod, GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, GLuint arg3Mod)
{
    GLCALL(glAlphaFragmentOp3ATI, op, dst, dstMod, arg1, arg1Rep, arg1Mod, arg2, arg2Rep, arg2Mod, arg3, arg3Rep, arg3Mod);
}

inline void mglSetFragmentShaderConstantATI (GLuint dst, const GLfloat * value)
{
    GLCALL(glSetFragmentShaderConstantATI, dst, value);
}

inline void mglPointParameteriNV (GLenum pname, GLint param)
{
    GLCALL(glPointParameteriNV, pname, param);
}

inline void mglPointParameterivNV (GLenum pname, const GLint * params)
{
    GLCALL(glPointParameterivNV, pname, params);
}

inline void mglDrawBuffersATI (GLsizei n, const GLenum * bufs)
{
    GLCALL(glDrawBuffersATI, n, bufs);
}

inline void mglProgramNamedParameter4fNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    GLCALL(glProgramNamedParameter4fNV, id, len, name, x, y, z, w);
}

inline void mglProgramNamedParameter4dNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    GLCALL(glProgramNamedParameter4dNV, id, len, name, x, y, z, w);
}

inline void mglProgramNamedParameter4fvNV (GLuint id, GLsizei len, const GLubyte * name, const GLfloat * v)
{
    GLCALL(glProgramNamedParameter4fvNV, id, len, name, v);
}

inline void mglProgramNamedParameter4dvNV (GLuint id, GLsizei len, const GLubyte * name, const GLdouble * v)
{
    GLCALL(glProgramNamedParameter4dvNV, id, len, name, v);
}

inline void mglGetProgramNamedParameterfvNV (GLuint id, GLsizei len, const GLubyte * name, GLfloat * params)
{
    GLCALL(glGetProgramNamedParameterfvNV, id, len, name, params);
}

inline void mglGetProgramNamedParameterdvNV (GLuint id, GLsizei len, const GLubyte * name, GLdouble * params)
{
    GLCALL(glGetProgramNamedParameterdvNV, id, len, name, params);
}

inline GLboolean mglIsRenderbufferEXT (GLuint renderbuffer)
{
    return GLCALL(glIsRenderbufferEXT, renderbuffer);
}

inline void mglBindRenderbufferEXT (GLenum target, GLuint renderbuffer)
{
    GLCALL(glBindRenderbufferEXT, target, renderbuffer);
}

inline void mglDeleteRenderbuffersEXT (GLsizei n, const GLuint * renderbuffers)
{
    GLCALL(glDeleteRenderbuffersEXT, n, renderbuffers);
}

inline void mglGenRenderbuffersEXT (GLsizei n, GLuint * renderbuffers)
{
    GLCALL(glGenRenderbuffersEXT, n, renderbuffers);
}

inline void mglRenderbufferStorageEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageEXT, target, internalformat, width, height);
}

inline void mglGetRenderbufferParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetRenderbufferParameterivEXT, target, pname, params);
}

inline GLboolean mglIsFramebufferEXT (GLuint framebuffer)
{
    return GLCALL(glIsFramebufferEXT, framebuffer);
}

inline void mglBindFramebufferEXT (GLenum target, GLuint framebuffer)
{
    GLCALL(glBindFramebufferEXT, target, framebuffer);
}

inline void mglDeleteFramebuffersEXT (GLsizei n, const GLuint * framebuffers)
{
    GLCALL(glDeleteFramebuffersEXT, n, framebuffers);
}

inline void mglGenFramebuffersEXT (GLsizei n, GLuint * framebuffers)
{
    GLCALL(glGenFramebuffersEXT, n, framebuffers);
}

inline GLenum mglCheckFramebufferStatusEXT (GLenum target)
{
    return GLCALL(glCheckFramebufferStatusEXT, target);
}

inline void mglFramebufferTexture1DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture1DEXT, target, attachment, textarget, texture, level);
}

inline void mglFramebufferTexture2DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture2DEXT, target, attachment, textarget, texture, level);
}

inline void mglFramebufferTexture3DEXT (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
    GLCALL(glFramebufferTexture3DEXT, target, attachment, textarget, texture, level, zoffset);
}

inline void mglFramebufferRenderbufferEXT (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
    GLCALL(glFramebufferRenderbufferEXT, target, attachment, renderbuffertarget, renderbuffer);
}

inline void mglGetFramebufferAttachmentParameterivEXT (GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
    GLCALL(glGetFramebufferAttachmentParameterivEXT, target, attachment, pname, params);
}

inline void mglGenerateMipmapEXT (GLenum target)
{
    GLCALL(glGenerateMipmapEXT, target);
}

inline void mglFramebufferTextureLayerEXT (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GLCALL(glFramebufferTextureLayerEXT, target, attachment, texture, level, layer);
}

inline GLvoid* mglMapBufferRange (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
    return GLCALL(glMapBufferRange, target, offset, length, access);
}

inline void mglFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length)
{
    GLCALL(glFlushMappedBufferRange, target, offset, length);
}

inline void mglBindVertexArray (GLuint array)
{
    GLCALL(glBindVertexArray, array);
}

inline void mglDeleteVertexArrays (GLsizei n, const GLuint * arrays)
{
    GLCALL(glDeleteVertexArrays, n, arrays);
}

inline void mglGenVertexArrays (GLsizei n, GLuint * arrays)
{
    GLCALL(glGenVertexArrays, n, arrays);
}

inline GLboolean mglIsVertexArray (GLuint array)
{
    return GLCALL(glIsVertexArray, array);
}

inline void mglCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    GLCALL(glCopyBufferSubData, readTarget, writeTarget, readOffset, writeOffset, size);
}

inline GLsync mglFenceSync (GLenum condition, GLbitfield flags)
{
    return GLCALL(glFenceSync, condition, flags);
}

inline GLboolean mglIsSync (GLsync sync)
{
    return GLCALL(glIsSync, sync);
}

inline void mglDeleteSync (GLsync sync)
{
    GLCALL(glDeleteSync, sync);
}

inline GLenum mglClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    return GLCALL(glClientWaitSync, sync, flags, timeout);
}

inline void mglWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout)
{
    GLCALL(glWaitSync, sync, flags, timeout);
}

inline void mglGetInteger64v (GLenum pname, GLint64 * params)
{
    GLCALL(glGetInteger64v, pname, params);
}

inline void mglGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values)
{
    GLCALL(glGetSynciv, sync, pname, bufSize, length, values);
}

inline void mglProvokingVertexEXT (GLenum mode)
{
    GLCALL(glProvokingVertexEXT, mode);
}

inline void mglDrawElementsBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    GLCALL(glDrawElementsBaseVertex, mode, count, type, indices, basevertex);
}

inline void mglDrawRangeElementsBaseVertex (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex)
{
    GLCALL(glDrawRangeElementsBaseVertex, mode, start, end, count, type, indices, basevertex);
}

inline void mglMultiDrawElementsBaseVertex (GLenum mode, const GLsizei * count, GLenum type, const GLvoid *  * indices, GLsizei primcount, const GLint * basevertex)
{
    GLCALL(glMultiDrawElementsBaseVertex, mode, count, type, indices, primcount, basevertex);
}

inline void mglProvokingVertex (GLenum mode)
{
    GLCALL(glProvokingVertex, mode);
}

inline void mglRenderbufferStorageMultisampleEXT (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    GLCALL(glRenderbufferStorageMultisampleEXT, target, samples, internalformat, width, height);
}

inline void mglColorMaskIndexedEXT (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    GLCALL(glColorMaskIndexedEXT, index, r, g, b, a);
}

inline void mglGetBooleanIndexedvEXT (GLenum target, GLuint index, GLboolean * data)
{
    GLCALL(glGetBooleanIndexedvEXT, target, index, data);
}

inline void mglGetIntegerIndexedvEXT (GLenum target, GLuint index, GLint * data)
{
    GLCALL(glGetIntegerIndexedvEXT, target, index, data);
}

inline void mglEnableIndexedEXT (GLenum target, GLuint index)
{
    GLCALL(glEnableIndexedEXT, target, index);
}

inline void mglDisableIndexedEXT (GLenum target, GLuint index)
{
    GLCALL(glDisableIndexedEXT, target, index);
}

inline GLboolean mglIsEnabledIndexedEXT (GLenum target, GLuint index)
{
    return GLCALL(glIsEnabledIndexedEXT, target, index);
}

inline void mglBeginConditionalRenderNV (GLuint id, GLenum mode)
{
    GLCALL(glBeginConditionalRenderNV, id, mode);
}

inline void mglEndConditionalRenderNV ()
{
    GLCALL(glEndConditionalRenderNV);
}

inline GLenum mglObjectPurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    return GLCALL(glObjectPurgeableAPPLE, objectType, name, option);
}

inline GLenum mglObjectUnpurgeableAPPLE (GLenum objectType, GLuint name, GLenum option)
{
    return GLCALL(glObjectUnpurgeableAPPLE, objectType, name, option);
}

inline void mglGetObjectParameterivAPPLE (GLenum objectType, GLuint name, GLenum pname, GLint * params)
{
    GLCALL(glGetObjectParameterivAPPLE, objectType, name, pname, params);
}

inline void mglBeginTransformFeedback (GLenum primitiveMode)
{
    GLCALL(glBeginTransformFeedback, primitiveMode);
}

inline void mglEndTransformFeedback ()
{
    GLCALL(glEndTransformFeedback);
}

inline void mglBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glBindBufferRange, target, index, buffer, offset, size);
}

inline void mglBindBufferBase (GLenum target, GLuint index, GLuint buffer)
{
    GLCALL(glBindBufferBase, target, index, buffer);
}

inline void mglTransformFeedbackVaryings (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    GLCALL(glTransformFeedbackVaryings, program, count, varyings, bufferMode);
}

inline void mglGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetTransformFeedbackVarying, program, index, bufSize, length, size, type, name);
}

inline void mglDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstanced, mode, first, count, primcount);
}

inline void mglDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstanced, mode, count, type, indices, primcount);
}

inline void mglDrawArraysInstancedARB (GLenum mode, GLint first, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstancedARB, mode, first, count, primcount);
}

inline void mglDrawElementsInstancedARB (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstancedARB, mode, count, type, indices, primcount);
}

inline void mglProgramParameteriARB (GLuint program, GLenum pname, GLint value)
{
    GLCALL(glProgramParameteriARB, program, pname, value);
}

inline void mglFramebufferTextureARB (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTextureARB, target, attachment, texture, level);
}

inline void mglFramebufferTextureFaceARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face)
{
    GLCALL(glFramebufferTextureFaceARB, target, attachment, texture, level, face);
}

inline void mglBindTransformFeedback (GLenum target, GLuint id)
{
    GLCALL(glBindTransformFeedback, target, id);
}

inline void mglDeleteTransformFeedbacks (GLsizei n, const GLuint * ids)
{
    GLCALL(glDeleteTransformFeedbacks, n, ids);
}

inline void mglGenTransformFeedbacks (GLsizei n, GLuint * ids)
{
    GLCALL(glGenTransformFeedbacks, n, ids);
}

inline GLboolean mglIsTransformFeedback (GLuint id)
{
    return GLCALL(glIsTransformFeedback, id);
}

inline void mglPauseTransformFeedback ()
{
    GLCALL(glPauseTransformFeedback);
}

inline void mglResumeTransformFeedback ()
{
    GLCALL(glResumeTransformFeedback);
}

inline void mglDrawTransformFeedback (GLenum mode, GLuint id)
{
    GLCALL(glDrawTransformFeedback, mode, id);
}

inline void mglDrawArraysInstancedEXT (GLenum mode, GLint start, GLsizei count, GLsizei primcount)
{
    GLCALL(glDrawArraysInstancedEXT, mode, start, count, primcount);
}

inline void mglDrawElementsInstancedEXT (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount)
{
    GLCALL(glDrawElementsInstancedEXT, mode, count, type, indices, primcount);
}

inline void mglBeginTransformFeedbackEXT (GLenum primitiveMode)
{
    GLCALL(glBeginTransformFeedbackEXT, primitiveMode);
}

inline void mglEndTransformFeedbackEXT ()
{
    GLCALL(glEndTransformFeedbackEXT);
}

inline void mglBindBufferRangeEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glBindBufferRangeEXT, target, index, buffer, offset, size);
}

inline void mglBindBufferOffsetEXT (GLenum target, GLuint index, GLuint buffer, GLintptr offset)
{
    GLCALL(glBindBufferOffsetEXT, target, index, buffer, offset);
}

inline void mglBindBufferBaseEXT (GLenum target, GLuint index, GLuint buffer)
{
    GLCALL(glBindBufferBaseEXT, target, index, buffer);
}

inline void mglTransformFeedbackVaryingsEXT (GLuint program, GLsizei count, const GLchar *  * varyings, GLenum bufferMode)
{
    GLCALL(glTransformFeedbackVaryingsEXT, program, count, varyings, bufferMode);
}

inline void mglGetTransformFeedbackVaryingEXT (GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
    GLCALL(glGetTransformFeedbackVaryingEXT, program, index, bufSize, length, size, type, name);
}

inline void mglEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image)
{
    GLCALL(glEGLImageTargetTexture2DOES, target, image);
}

inline void mglEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image)
{
    GLCALL(glEGLImageTargetRenderbufferStorageOES, target, image);
}

inline void mglColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
    GLCALL(glColorMaski, index, r, g, b, a);
}

inline void mglGetBooleani_v (GLenum target, GLuint index, GLboolean * data)
{
    GLCALL(glGetBooleani_v, target, index, data);
}

inline void mglGetIntegeri_v (GLenum target, GLuint index, GLint * data)
{
    GLCALL(glGetIntegeri_v, target, index, data);
}

inline void mglEnablei (GLenum target, GLuint index)
{
    GLCALL(glEnablei, target, index);
}

inline void mglDisablei (GLenum target, GLuint index)
{
    GLCALL(glDisablei, target, index);
}

inline GLboolean mglIsEnabledi (GLenum target, GLuint index)
{
    return GLCALL(glIsEnabledi, target, index);
}

inline void mglClampColor (GLenum target, GLenum clamp)
{
    GLCALL(glClampColor, target, clamp);
}

inline void mglBeginConditionalRender (GLuint id, GLenum mode)
{
    GLCALL(glBeginConditionalRender, id, mode);
}

inline void mglEndConditionalRender ()
{
    GLCALL(glEndConditionalRender);
}

inline void mglVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribIPointer, index, size, type, stride, pointer);
}

inline void mglGetVertexAttribIiv (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribIiv, index, pname, params);
}

inline void mglGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint * params)
{
    GLCALL(glGetVertexAttribIuiv, index, pname, params);
}

inline void mglVertexAttribI1i (GLuint index, GLint x)
{
    GLCALL(glVertexAttribI1i, index, x);
}

inline void mglVertexAttribI2i (GLuint index, GLint x, GLint y)
{
    GLCALL(glVertexAttribI2i, index, x, y);
}

inline void mglVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z)
{
    GLCALL(glVertexAttribI3i, index, x, y, z);
}

inline void mglVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertexAttribI4i, index, x, y, z, w);
}

inline void mglVertexAttribI1ui (GLuint index, GLuint x)
{
    GLCALL(glVertexAttribI1ui, index, x);
}

inline void mglVertexAttribI2ui (GLuint index, GLuint x, GLuint y)
{
    GLCALL(glVertexAttribI2ui, index, x, y);
}

inline void mglVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z)
{
    GLCALL(glVertexAttribI3ui, index, x, y, z);
}

inline void mglVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    GLCALL(glVertexAttribI4ui, index, x, y, z, w);
}

inline void mglVertexAttribI1iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI1iv, index, v);
}

inline void mglVertexAttribI2iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI2iv, index, v);
}

inline void mglVertexAttribI3iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI3iv, index, v);
}

inline void mglVertexAttribI4iv (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI4iv, index, v);
}

inline void mglVertexAttribI1uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI1uiv, index, v);
}

inline void mglVertexAttribI2uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI2uiv, index, v);
}

inline void mglVertexAttribI3uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI3uiv, index, v);
}

inline void mglVertexAttribI4uiv (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI4uiv, index, v);
}

inline void mglVertexAttribI4bv (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttribI4bv, index, v);
}

inline void mglVertexAttribI4sv (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttribI4sv, index, v);
}

inline void mglVertexAttribI4ubv (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttribI4ubv, index, v);
}

inline void mglVertexAttribI4usv (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttribI4usv, index, v);
}

inline void mglGetUniformuiv (GLuint program, GLint location, GLuint * params)
{
    GLCALL(glGetUniformuiv, program, location, params);
}

inline void mglBindFragDataLocation (GLuint program, GLuint color, const GLchar * name)
{
    GLCALL(glBindFragDataLocation, program, color, name);
}

inline GLint mglGetFragDataLocation (GLuint program, const GLchar * name)
{
    return GLCALL(glGetFragDataLocation, program, name);
}

inline void mglUniform1ui (GLint location, GLuint v0)
{
    GLCALL(glUniform1ui, location, v0);
}

inline void mglUniform2ui (GLint location, GLuint v0, GLuint v1)
{
    GLCALL(glUniform2ui, location, v0, v1);
}

inline void mglUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GLCALL(glUniform3ui, location, v0, v1, v2);
}

inline void mglUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GLCALL(glUniform4ui, location, v0, v1, v2, v3);
}

inline void mglUniform1uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform1uiv, location, count, value);
}

inline void mglUniform2uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform2uiv, location, count, value);
}

inline void mglUniform3uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform3uiv, location, count, value);
}

inline void mglUniform4uiv (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform4uiv, location, count, value);
}

inline void mglTexParameterIiv (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameterIiv, target, pname, params);
}

inline void mglTexParameterIuiv (GLenum target, GLenum pname, const GLuint * params)
{
    GLCALL(glTexParameterIuiv, target, pname, params);
}

inline void mglGetTexParameterIiv (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameterIiv, target, pname, params);
}

inline void mglGetTexParameterIuiv (GLenum target, GLenum pname, GLuint * params)
{
    GLCALL(glGetTexParameterIuiv, target, pname, params);
}

inline void mglClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint * value)
{
    GLCALL(glClearBufferiv, buffer, drawbuffer, value);
}

inline void mglClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint * value)
{
    GLCALL(glClearBufferuiv, buffer, drawbuffer, value);
}

inline void mglClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat * value)
{
    GLCALL(glClearBufferfv, buffer, drawbuffer, value);
}

inline void mglClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    GLCALL(glClearBufferfi, buffer, drawbuffer, depth, stencil);
}

inline const GLubyte * mglGetStringi (GLenum name, GLuint index)
{
    return GLCALL(glGetStringi, name, index);
}

inline void mglTexBuffer (GLenum target, GLenum internalformat, GLuint buffer)
{
    GLCALL(glTexBuffer, target, internalformat, buffer);
}

inline void mglPrimitiveRestartIndex (GLuint index)
{
    GLCALL(glPrimitiveRestartIndex, index);
}

inline void mglGetInteger64i_v (GLenum target, GLuint index, GLint64 * data)
{
    GLCALL(glGetInteger64i_v, target, index, data);
}

inline void mglGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 * params)
{
    GLCALL(glGetBufferParameteri64v, target, pname, params);
}

inline void mglFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level)
{
    GLCALL(glFramebufferTexture, target, attachment, texture, level);
}

inline void mglVertexAttribDivisor (GLuint index, GLuint divisor)
{
    GLCALL(glVertexAttribDivisor, index, divisor);
}

inline void mglPrimitiveRestartNV ()
{
    GLCALL(glPrimitiveRestartNV);
}

inline void mglPrimitiveRestartIndexNV (GLuint index)
{
    GLCALL(glPrimitiveRestartIndexNV, index);
}

inline void mglVertexAttribI1iEXT (GLuint index, GLint x)
{
    GLCALL(glVertexAttribI1iEXT, index, x);
}

inline void mglVertexAttribI2iEXT (GLuint index, GLint x, GLint y)
{
    GLCALL(glVertexAttribI2iEXT, index, x, y);
}

inline void mglVertexAttribI3iEXT (GLuint index, GLint x, GLint y, GLint z)
{
    GLCALL(glVertexAttribI3iEXT, index, x, y, z);
}

inline void mglVertexAttribI4iEXT (GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    GLCALL(glVertexAttribI4iEXT, index, x, y, z, w);
}

inline void mglVertexAttribI1uiEXT (GLuint index, GLuint x)
{
    GLCALL(glVertexAttribI1uiEXT, index, x);
}

inline void mglVertexAttribI2uiEXT (GLuint index, GLuint x, GLuint y)
{
    GLCALL(glVertexAttribI2uiEXT, index, x, y);
}

inline void mglVertexAttribI3uiEXT (GLuint index, GLuint x, GLuint y, GLuint z)
{
    GLCALL(glVertexAttribI3uiEXT, index, x, y, z);
}

inline void mglVertexAttribI4uiEXT (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    GLCALL(glVertexAttribI4uiEXT, index, x, y, z, w);
}

inline void mglVertexAttribI1ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI1ivEXT, index, v);
}

inline void mglVertexAttribI2ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI2ivEXT, index, v);
}

inline void mglVertexAttribI3ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI3ivEXT, index, v);
}

inline void mglVertexAttribI4ivEXT (GLuint index, const GLint * v)
{
    GLCALL(glVertexAttribI4ivEXT, index, v);
}

inline void mglVertexAttribI1uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI1uivEXT, index, v);
}

inline void mglVertexAttribI2uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI2uivEXT, index, v);
}

inline void mglVertexAttribI3uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI3uivEXT, index, v);
}

inline void mglVertexAttribI4uivEXT (GLuint index, const GLuint * v)
{
    GLCALL(glVertexAttribI4uivEXT, index, v);
}

inline void mglVertexAttribI4bvEXT (GLuint index, const GLbyte * v)
{
    GLCALL(glVertexAttribI4bvEXT, index, v);
}

inline void mglVertexAttribI4svEXT (GLuint index, const GLshort * v)
{
    GLCALL(glVertexAttribI4svEXT, index, v);
}

inline void mglVertexAttribI4ubvEXT (GLuint index, const GLubyte * v)
{
    GLCALL(glVertexAttribI4ubvEXT, index, v);
}

inline void mglVertexAttribI4usvEXT (GLuint index, const GLushort * v)
{
    GLCALL(glVertexAttribI4usvEXT, index, v);
}

inline void mglVertexAttribIPointerEXT (GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid * pointer)
{
    GLCALL(glVertexAttribIPointerEXT, index, size, type, stride, pointer);
}

inline void mglGetVertexAttribIivEXT (GLuint index, GLenum pname, GLint * params)
{
    GLCALL(glGetVertexAttribIivEXT, index, pname, params);
}

inline void mglGetVertexAttribIuivEXT (GLuint index, GLenum pname, GLuint * params)
{
    GLCALL(glGetVertexAttribIuivEXT, index, pname, params);
}

inline void mglGetUniformuivEXT (GLuint program, GLint location, GLuint * params)
{
    GLCALL(glGetUniformuivEXT, program, location, params);
}

inline void mglBindFragDataLocationEXT (GLuint program, GLuint color, const GLchar * name)
{
    GLCALL(glBindFragDataLocationEXT, program, color, name);
}

inline GLint mglGetFragDataLocationEXT (GLuint program, const GLchar * name)
{
    return GLCALL(glGetFragDataLocationEXT, program, name);
}

inline void mglUniform1uiEXT (GLint location, GLuint v0)
{
    GLCALL(glUniform1uiEXT, location, v0);
}

inline void mglUniform2uiEXT (GLint location, GLuint v0, GLuint v1)
{
    GLCALL(glUniform2uiEXT, location, v0, v1);
}

inline void mglUniform3uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2)
{
    GLCALL(glUniform3uiEXT, location, v0, v1, v2);
}

inline void mglUniform4uiEXT (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    GLCALL(glUniform4uiEXT, location, v0, v1, v2, v3);
}

inline void mglUniform1uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform1uivEXT, location, count, value);
}

inline void mglUniform2uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform2uivEXT, location, count, value);
}

inline void mglUniform3uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform3uivEXT, location, count, value);
}

inline void mglUniform4uivEXT (GLint location, GLsizei count, const GLuint * value)
{
    GLCALL(glUniform4uivEXT, location, count, value);
}

inline void mglTexParameterIivEXT (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glTexParameterIivEXT, target, pname, params);
}

inline void mglTexParameterIuivEXT (GLenum target, GLenum pname, const GLuint * params)
{
    GLCALL(glTexParameterIuivEXT, target, pname, params);
}

inline void mglGetTexParameterIivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetTexParameterIivEXT, target, pname, params);
}

inline void mglGetTexParameterIuivEXT (GLenum target, GLenum pname, GLuint * params)
{
    GLCALL(glGetTexParameterIuivEXT, target, pname, params);
}

inline void mglClearColorIiEXT (GLint red, GLint green, GLint blue, GLint alpha)
{
    GLCALL(glClearColorIiEXT, red, green, blue, alpha);
}

inline void mglClearColorIuiEXT (GLuint red, GLuint green, GLuint blue, GLuint alpha)
{
    GLCALL(glClearColorIuiEXT, red, green, blue, alpha);
}

inline void mglUseShaderProgramEXT (GLenum type, GLuint program)
{
    GLCALL(glUseShaderProgramEXT, type, program);
}

inline void mglActiveProgramEXT (GLuint program)
{
    GLCALL(glActiveProgramEXT, program);
}

inline GLuint mglCreateShaderProgramEXT (GLenum type, const GLchar * string)
{
    return GLCALL(glCreateShaderProgramEXT, type, string);
}

inline void mglProgramEnvParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    GLCALL(glProgramEnvParameters4fvEXT, target, index, count, params);
}

inline void mglProgramLocalParameters4fvEXT (GLenum target, GLuint index, GLsizei count, const GLfloat * params)
{
    GLCALL(glProgramLocalParameters4fvEXT, target, index, count, params);
}

inline void mglBlendEquationSeparateATI (GLenum modeRGB, GLenum modeA)
{
    GLCALL(glBlendEquationSeparateATI, modeRGB, modeA);
}

inline void mglGetHistogramEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetHistogramEXT, target, reset, format, type, values);
}

inline void mglGetHistogramParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetHistogramParameterfvEXT, target, pname, params);
}

inline void mglGetHistogramParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetHistogramParameterivEXT, target, pname, params);
}

inline void mglGetMinmaxEXT (GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid * values)
{
    GLCALL(glGetMinmaxEXT, target, reset, format, type, values);
}

inline void mglGetMinmaxParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetMinmaxParameterfvEXT, target, pname, params);
}

inline void mglGetMinmaxParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetMinmaxParameterivEXT, target, pname, params);
}

inline void mglHistogramEXT (GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
    GLCALL(glHistogramEXT, target, width, internalformat, sink);
}

inline void mglMinmaxEXT (GLenum target, GLenum internalformat, GLboolean sink)
{
    GLCALL(glMinmaxEXT, target, internalformat, sink);
}

inline void mglResetHistogramEXT (GLenum target)
{
    GLCALL(glResetHistogramEXT, target);
}

inline void mglResetMinmaxEXT (GLenum target)
{
    GLCALL(glResetMinmaxEXT, target);
}

inline void mglConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter1DEXT, target, internalformat, width, format, type, image);
}

inline void mglConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * image)
{
    GLCALL(glConvolutionFilter2DEXT, target, internalformat, width, height, format, type, image);
}

inline void mglConvolutionParameterfEXT (GLenum target, GLenum pname, GLfloat params)
{
    GLCALL(glConvolutionParameterfEXT, target, pname, params);
}

inline void mglConvolutionParameterfvEXT (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glConvolutionParameterfvEXT, target, pname, params);
}

inline void mglConvolutionParameteriEXT (GLenum target, GLenum pname, GLint params)
{
    GLCALL(glConvolutionParameteriEXT, target, pname, params);
}

inline void mglConvolutionParameterivEXT (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glConvolutionParameterivEXT, target, pname, params);
}

inline void mglCopyConvolutionFilter1DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyConvolutionFilter1DEXT, target, internalformat, x, y, width);
}

inline void mglCopyConvolutionFilter2DEXT (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLCALL(glCopyConvolutionFilter2DEXT, target, internalformat, x, y, width, height);
}

inline void mglGetConvolutionFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * image)
{
    GLCALL(glGetConvolutionFilterEXT, target, format, type, image);
}

inline void mglGetConvolutionParameterfvEXT (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetConvolutionParameterfvEXT, target, pname, params);
}

inline void mglGetConvolutionParameterivEXT (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetConvolutionParameterivEXT, target, pname, params);
}

inline void mglGetSeparableFilterEXT (GLenum target, GLenum format, GLenum type, GLvoid * row, GLvoid * column, GLvoid * span)
{
    GLCALL(glGetSeparableFilterEXT, target, format, type, row, column, span);
}

inline void mglSeparableFilter2DEXT (GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * row, const GLvoid * column)
{
    GLCALL(glSeparableFilter2DEXT, target, internalformat, width, height, format, type, row, column);
}

inline void mglColorTableSGI (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid * table)
{
    GLCALL(glColorTableSGI, target, internalformat, width, format, type, table);
}

inline void mglColorTableParameterfvSGI (GLenum target, GLenum pname, const GLfloat * params)
{
    GLCALL(glColorTableParameterfvSGI, target, pname, params);
}

inline void mglColorTableParameterivSGI (GLenum target, GLenum pname, const GLint * params)
{
    GLCALL(glColorTableParameterivSGI, target, pname, params);
}

inline void mglCopyColorTableSGI (GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorTableSGI, target, internalformat, x, y, width);
}

inline void mglGetColorTableSGI (GLenum target, GLenum format, GLenum type, GLvoid * table)
{
    GLCALL(glGetColorTableSGI, target, format, type, table);
}

inline void mglGetColorTableParameterfvSGI (GLenum target, GLenum pname, GLfloat * params)
{
    GLCALL(glGetColorTableParameterfvSGI, target, pname, params);
}

inline void mglGetColorTableParameterivSGI (GLenum target, GLenum pname, GLint * params)
{
    GLCALL(glGetColorTableParameterivSGI, target, pname, params);
}

inline void mglPixelTexGenSGIX (GLenum mode)
{
    GLCALL(glPixelTexGenSGIX, mode);
}

inline void mglPixelTexGenParameteriSGIS (GLenum pname, GLint param)
{
    GLCALL(glPixelTexGenParameteriSGIS, pname, param);
}

inline void mglPixelTexGenParameterivSGIS (GLenum pname, const GLint * params)
{
    GLCALL(glPixelTexGenParameterivSGIS, pname, params);
}

inline void mglPixelTexGenParameterfSGIS (GLenum pname, GLfloat param)
{
    GLCALL(glPixelTexGenParameterfSGIS, pname, param);
}

inline void mglPixelTexGenParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    GLCALL(glPixelTexGenParameterfvSGIS, pname, params);
}

inline void mglGetPixelTexGenParameterivSGIS (GLenum pname, GLint * params)
{
    GLCALL(glGetPixelTexGenParameterivSGIS, pname, params);
}

inline void mglGetPixelTexGenParameterfvSGIS (GLenum pname, GLfloat * params)
{
    GLCALL(glGetPixelTexGenParameterfvSGIS, pname, params);
}

inline void mglSampleMaskSGIS (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleMaskSGIS, value, invert);
}

inline void mglSamplePatternSGIS (GLenum pattern)
{
    GLCALL(glSamplePatternSGIS, pattern);
}

inline void mglPointParameterfSGIS (GLenum pname, GLfloat param)
{
    GLCALL(glPointParameterfSGIS, pname, param);
}

inline void mglPointParameterfvSGIS (GLenum pname, const GLfloat * params)
{
    GLCALL(glPointParameterfvSGIS, pname, params);
}

inline void mglColorSubTableEXT (GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid * data)
{
    GLCALL(glColorSubTableEXT, target, start, count, format, type, data);
}

inline void mglCopyColorSubTableEXT (GLenum target, GLsizei start, GLint x, GLint y, GLsizei width)
{
    GLCALL(glCopyColorSubTableEXT, target, start, x, y, width);
}

inline void mglBlendFuncSeparateINGR (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(glBlendFuncSeparateINGR, sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

inline void mglMultiModeDrawArraysIBM (const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride)
{
    GLCALL(glMultiModeDrawArraysIBM, mode, first, count, primcount, modestride);
}

inline void mglMultiModeDrawElementsIBM (const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride)
{
    GLCALL(glMultiModeDrawElementsIBM, mode, count, type, indices, primcount, modestride);
}

inline void mglSampleMaskEXT (GLclampf value, GLboolean invert)
{
    GLCALL(glSampleMaskEXT, value, invert);
}

inline void mglSamplePatternEXT (GLenum pattern)
{
    GLCALL(glSamplePatternEXT, pattern);
}

inline void mglDeleteFencesNV (GLsizei n, const GLuint * fences)
{
    GLCALL(glDeleteFencesNV, n, fences);
}

inline void mglGenFencesNV (GLsizei n, GLuint * fences)
{
    GLCALL(glGenFencesNV, n, fences);
}

inline GLboolean mglIsFenceNV (GLuint fence)
{
    return GLCALL(glIsFenceNV, fence);
}

inline GLboolean mglTestFenceNV (GLuint fence)
{
    return GLCALL(glTestFenceNV, fence);
}

inline void mglGetFenceivNV (GLuint fence, GLenum pname, GLint * params)
{
    GLCALL(glGetFenceivNV, fence, pname, params);
}

inline void mglFinishFenceNV (GLuint fence)
{
    GLCALL(glFinishFenceNV, fence);
}

inline void mglSetFenceNV (GLuint fence, GLenum condition)
{
    GLCALL(glSetFenceNV, fence, condition);
}

inline void mglActiveStencilFaceEXT (GLenum face)
{
    GLCALL(glActiveStencilFaceEXT, face);
}

inline void mglBindVertexArrayAPPLE (GLuint array)
{
    GLCALL(glBindVertexArrayAPPLE, array);
}

inline void mglDeleteVertexArraysAPPLE (GLsizei n, const GLuint * arrays)
{
    GLCALL(glDeleteVertexArraysAPPLE, n, arrays);
}

inline void mglGenVertexArraysAPPLE (GLsizei n, GLuint * arrays)
{
    GLCALL(glGenVertexArraysAPPLE, n, arrays);
}

inline GLboolean mglIsVertexArrayAPPLE (GLuint array)
{
    return GLCALL(glIsVertexArrayAPPLE, array);
}

inline void mglStencilOpSeparateATI (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
{
    GLCALL(glStencilOpSeparateATI, face, sfail, dpfail, dppass);
}

inline void mglStencilFuncSeparateATI (GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask)
{
    GLCALL(glStencilFuncSeparateATI, frontfunc, backfunc, ref, mask);
}

inline void mglDepthBoundsEXT (GLclampd zmin, GLclampd zmax)
{
    GLCALL(glDepthBoundsEXT, zmin, zmax);
}

inline void mglBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparateEXT, modeRGB, modeAlpha);
}

inline void mglBlitFramebufferEXT (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    GLCALL(glBlitFramebufferEXT, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

inline void mglGetQueryObjecti64vEXT (GLuint id, GLenum pname, GLint64EXT * params)
{
    GLCALL(glGetQueryObjecti64vEXT, id, pname, params);
}

inline void mglGetQueryObjectui64vEXT (GLuint id, GLenum pname, GLuint64EXT * params)
{
    GLCALL(glGetQueryObjectui64vEXT, id, pname, params);
}

inline void mglBufferParameteriAPPLE (GLenum target, GLenum pname, GLint param)
{
    GLCALL(glBufferParameteriAPPLE, target, pname, param);
}

inline void mglFlushMappedBufferRangeAPPLE (GLenum target, GLintptr offset, GLsizeiptr size)
{
    GLCALL(glFlushMappedBufferRangeAPPLE, target, offset, size);
}

inline void mglTextureRangeAPPLE (GLenum target, GLsizei length, const GLvoid * pointer)
{
    GLCALL(glTextureRangeAPPLE, target, length, pointer);
}

inline void mglGetTexParameterPointervAPPLE (GLenum target, GLenum pname, GLvoid *  * params)
{
    GLCALL(glGetTexParameterPointervAPPLE, target, pname, params);
}

inline void mglClampColorARB (GLenum target, GLenum clamp)
{
    GLCALL(glClampColorARB, target, clamp);
}

inline void mglFramebufferTextureLayerARB (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
    GLCALL(glFramebufferTextureLayerARB, target, attachment, texture, level, layer);
}

inline void mglVertexAttribDivisorARB (GLuint index, GLuint divisor)
{
    GLCALL(glVertexAttribDivisorARB, index, divisor);
}

inline void mglTexBufferARB (GLenum target, GLenum internalformat, GLuint buffer)
{
    GLCALL(glTexBufferARB, target, internalformat, buffer);
}

inline void mglDrawElementsInstancedBaseVertex (GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei primcount, GLint basevertex)
{
    GLCALL(glDrawElementsInstancedBaseVertex, mode, count, type, indices, primcount, basevertex);
}

inline void mglBlendEquationiARB (GLuint buf, GLenum mode)
{
    GLCALL(glBlendEquationiARB, buf, mode);
}

inline void mglBlendEquationSeparateiARB (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparateiARB, buf, modeRGB, modeAlpha);
}

inline void mglBlendFunciARB (GLuint buf, GLenum src, GLenum dst)
{
    GLCALL(glBlendFunciARB, buf, src, dst);
}

inline void mglBlendFuncSeparateiARB (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    GLCALL(glBlendFuncSeparateiARB, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

inline void mglGenSamplers (GLsizei count, GLuint * samplers)
{
    GLCALL(glGenSamplers, count, samplers);
}

inline void mglDeleteSamplers (GLsizei count, const GLuint * samplers)
{
    GLCALL(glDeleteSamplers, count, samplers);
}

inline GLboolean mglIsSampler (GLuint sampler)
{
    return GLCALL(glIsSampler, sampler);
}

inline void mglBindSampler (GLuint unit, GLuint sampler)
{
    GLCALL(glBindSampler, unit, sampler);
}

inline void mglSamplerParameteri (GLuint sampler, GLenum pname, GLint param)
{
    GLCALL(glSamplerParameteri, sampler, pname, param);
}

inline void mglSamplerParameteriv (GLuint sampler, GLenum pname, const GLint * param)
{
    GLCALL(glSamplerParameteriv, sampler, pname, param);
}

inline void mglSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param)
{
    GLCALL(glSamplerParameterf, sampler, pname, param);
}

inline void mglSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat * param)
{
    GLCALL(glSamplerParameterfv, sampler, pname, param);
}

inline void mglSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint * param)
{
    GLCALL(glSamplerParameterIiv, sampler, pname, param);
}

inline void mglSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint * param)
{
    GLCALL(glSamplerParameterIuiv, sampler, pname, param);
}

inline void mglGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint * params)
{
    GLCALL(glGetSamplerParameteriv, sampler, pname, params);
}

inline void mglGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint * params)
{
    GLCALL(glGetSamplerParameterIiv, sampler, pname, params);
}

inline void mglGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat * params)
{
    GLCALL(glGetSamplerParameterfv, sampler, pname, params);
}

inline void mglGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint * params)
{
    GLCALL(glGetSamplerParameterIuiv, sampler, pname, params);
}

inline void mglReleaseShaderCompiler ()
{
    GLCALL(glReleaseShaderCompiler);
}

inline void mglShaderBinary (GLsizei count, const GLuint * shaders, GLenum binaryformat, const GLvoid * binary, GLsizei length)
{
    GLCALL(glShaderBinary, count, shaders, binaryformat, binary, length);
}

inline void mglGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint * range, GLint * precision)
{
    GLCALL(glGetShaderPrecisionFormat, shadertype, precisiontype, range, precision);
}

inline void mglDepthRangef (GLclampf n, GLclampf f)
{
    GLCALL(glDepthRangef, n, f);
}

inline void mglClearDepthf (GLclampf d)
{
    GLCALL(glClearDepthf, d);
}

inline GLenum mglGetGraphicsResetStatusARB ()
{
    return GLCALL(glGetGraphicsResetStatusARB);
}

inline void mglGetnMapdvARB (GLenum target, GLenum query, GLsizei bufSize, GLdouble * v)
{
    GLCALL(glGetnMapdvARB, target, query, bufSize, v);
}

inline void mglGetnMapfvARB (GLenum target, GLenum query, GLsizei bufSize, GLfloat * v)
{
    GLCALL(glGetnMapfvARB, target, query, bufSize, v);
}

inline void mglGetnMapivARB (GLenum target, GLenum query, GLsizei bufSize, GLint * v)
{
    GLCALL(glGetnMapivARB, target, query, bufSize, v);
}

inline void mglGetnPixelMapfvARB (GLenum map, GLsizei bufSize, GLfloat * values)
{
    GLCALL(glGetnPixelMapfvARB, map, bufSize, values);
}

inline void mglGetnPixelMapuivARB (GLenum map, GLsizei bufSize, GLuint * values)
{
    GLCALL(glGetnPixelMapuivARB, map, bufSize, values);
}

inline void mglGetnPixelMapusvARB (GLenum map, GLsizei bufSize, GLushort * values)
{
    GLCALL(glGetnPixelMapusvARB, map, bufSize, values);
}

inline void mglGetnPolygonStippleARB (GLsizei bufSize, GLubyte * pattern)
{
    GLCALL(glGetnPolygonStippleARB, bufSize, pattern);
}

inline void mglGetnColorTableARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * table)
{
    GLCALL(glGetnColorTableARB, target, format, type, bufSize, table);
}

inline void mglGetnConvolutionFilterARB (GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid * image)
{
    GLCALL(glGetnConvolutionFilterARB, target, format, type, bufSize, image);
}

inline void mglGetnSeparableFilterARB (GLenum target, GLenum format, GLenum type, GLsizei rowBufSize, GLvoid * row, GLsizei columnBufSize, GLvoid * column, GLvoid * span)
{
    GLCALL(glGetnSeparableFilterARB, target, format, type, rowBufSize, row, columnBufSize, column, span);
}

inline void mglGetnHistogramARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    GLCALL(glGetnHistogramARB, target, reset, format, type, bufSize, values);
}

inline void mglGetnMinmaxARB (GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid * values)
{
    GLCALL(glGetnMinmaxARB, target, reset, format, type, bufSize, values);
}

inline void mglGetnTexImageARB (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid * img)
{
    GLCALL(glGetnTexImageARB, target, level, format, type, bufSize, img);
}

inline void mglReadnPixelsARB (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, GLvoid * data)
{
    GLCALL(glReadnPixelsARB, x, y, width, height, format, type, bufSize, data);
}

inline void mglGetnCompressedTexImageARB (GLenum target, GLint lod, GLsizei bufSize, GLvoid * img)
{
    GLCALL(glGetnCompressedTexImageARB, target, lod, bufSize, img);
}

inline void mglGetnUniformfvARB (GLuint program, GLint location, GLsizei bufSize, GLfloat * params)
{
    GLCALL(glGetnUniformfvARB, program, location, bufSize, params);
}

inline void mglGetnUniformivARB (GLuint program, GLint location, GLsizei bufSize, GLint * params)
{
    GLCALL(glGetnUniformivARB, program, location, bufSize, params);
}

inline void mglGetnUniformuivARB (GLuint program, GLint location, GLsizei bufSize, GLuint * params)
{
    GLCALL(glGetnUniformuivARB, program, location, bufSize, params);
}

inline void mglGetnUniformdvARB (GLuint program, GLint location, GLsizei bufSize, GLdouble * params)
{
    GLCALL(glGetnUniformdvARB, program, location, bufSize, params);
}

inline void mglBlendFuncIndexedAMD (GLuint buf, GLenum src, GLenum dst)
{
    GLCALL(glBlendFuncIndexedAMD, buf, src, dst);
}

inline void mglBlendFuncSeparateIndexedAMD (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
    GLCALL(glBlendFuncSeparateIndexedAMD, buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

inline void mglBlendEquationIndexedAMD (GLuint buf, GLenum mode)
{
    GLCALL(glBlendEquationIndexedAMD, buf, mode);
}

inline void mglBlendEquationSeparateIndexedAMD (GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
    GLCALL(glBlendEquationSeparateIndexedAMD, buf, modeRGB, modeAlpha);
}

inline void mglTextureBarrierNV ()
{
    GLCALL(glTextureBarrierNV);
}

