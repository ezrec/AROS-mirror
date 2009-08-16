/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#define GL_GLEXT_PROTOTYPES

#include "GL/gl.h"

    /* 
    Now we have prototypes of mglXXX functions, but
    the defines from gl_mangle.h are still active so
    they will redefine our glXXX functions and we will
    get multiple declaractions. Undef the evil defines
    */

#include "mangle_undef.h"

#include "arosmesa_intern.h"

#include "glapi.h"

AROS_LH1(void, glClearIndex,
    AROS_LHA(GLfloat, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClearIndex(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glClearColor,
    AROS_LHA(GLclampf, red, D0),
    AROS_LHA(GLclampf, green, D1),
    AROS_LHA(GLclampf, blue, D2),
    AROS_LHA(GLclampf, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClearColor(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glClear,
    AROS_LHA(GLbitfield, mask, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClear(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexMask,
    AROS_LHA(GLuint, mask, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexMask(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColorMask,
    AROS_LHA(GLboolean, red, D0),
    AROS_LHA(GLboolean, green, D1),
    AROS_LHA(GLboolean, blue, D2),
    AROS_LHA(GLboolean, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorMask(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glAlphaFunc,
    AROS_LHA(GLenum, func, D0),
    AROS_LHA(GLclampf, ref, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAlphaFunc(func, ref);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBlendFunc,
    AROS_LHA(GLenum, sfactor, D0),
    AROS_LHA(GLenum, dfactor, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendFunc(sfactor, dfactor);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLogicOp,
    AROS_LHA(GLenum, opcode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLogicOp(opcode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glCullFace,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCullFace(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFrontFace,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFrontFace(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPointSize,
    AROS_LHA(GLfloat, size, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointSize(size);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLineWidth,
    AROS_LHA(GLfloat, width, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLineWidth(width);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLineStipple,
    AROS_LHA(GLint, factor, D0),
    AROS_LHA(GLushort, pattern, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLineStipple(factor, pattern);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPolygonMode,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, mode, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPolygonMode(face, mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPolygonOffset,
    AROS_LHA(GLfloat, factor, D0),
    AROS_LHA(GLfloat, units, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPolygonOffset(factor, units);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPolygonStipple,
    AROS_LHA(const GLubyte *, mask, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPolygonStipple(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glGetPolygonStipple,
    AROS_LHA(GLubyte *, mask, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPolygonStipple(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEdgeFlag,
    AROS_LHA(GLboolean, flag, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEdgeFlag(flag);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEdgeFlagv,
    AROS_LHA(const GLboolean *, flag, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEdgeFlagv(flag);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glScissor,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglScissor(x, y, width, height);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glClipPlane,
    AROS_LHA(GLenum, plane, D0),
    AROS_LHA(const GLdouble *, equation, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClipPlane(plane, equation);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetClipPlane,
    AROS_LHA(GLenum, plane, D0),
    AROS_LHA(GLdouble *, equation, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetClipPlane(plane, equation);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDrawBuffer,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawBuffer(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glReadBuffer,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglReadBuffer(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEnable,
    AROS_LHA(GLenum, cap, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEnable(cap);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDisable,
    AROS_LHA(GLenum, cap, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDisable(cap);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsEnabled,
    AROS_LHA(GLenum, cap, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsEnabled(cap);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEnableClientState,
    AROS_LHA(GLenum, cap, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEnableClientState(cap);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDisableClientState,
    AROS_LHA(GLenum, cap, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDisableClientState(cap);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetBooleanv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLboolean *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBooleanv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetDoublev,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLdouble *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetDoublev(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetFloatv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetFloatv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetIntegerv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetIntegerv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPushAttrib,
    AROS_LHA(GLbitfield, mask, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPushAttrib(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glPopAttrib,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPopAttrib();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPushClientAttrib,
    AROS_LHA(GLbitfield, mask, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPushClientAttrib(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glPopClientAttrib,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPopClientAttrib();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLint, glRenderMode,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRenderMode(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(GLenum, glGetError,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetError();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(const GLubyte *, glGetString,
    AROS_LHA(GLenum, name, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetString(name);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glFinish,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFinish();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glFlush,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFlush();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glHint,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, mode, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglHint(target, mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glClearDepth,
    AROS_LHA(GLclampd, depth, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClearDepth(depth);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDepthFunc,
    AROS_LHA(GLenum, func, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDepthFunc(func);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDepthMask,
    AROS_LHA(GLboolean, flag, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDepthMask(flag);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDepthRange,
    AROS_LHA(GLclampd, near_val, D0),
    AROS_LHA(GLclampd, far_val, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDepthRange(near_val, far_val);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glClearAccum,
    AROS_LHA(GLfloat, red, D0),
    AROS_LHA(GLfloat, green, D1),
    AROS_LHA(GLfloat, blue, D2),
    AROS_LHA(GLfloat, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClearAccum(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glAccum,
    AROS_LHA(GLenum, op, D0),
    AROS_LHA(GLfloat, value, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAccum(op, value);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMatrixMode,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMatrixMode(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glOrtho,
    AROS_LHA(GLdouble, left, D0),
    AROS_LHA(GLdouble, right, D1),
    AROS_LHA(GLdouble, bottom, D2),
    AROS_LHA(GLdouble, top, D3),
    AROS_LHA(GLdouble, near_val, D4),
    AROS_LHA(GLdouble, far_val, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglOrtho(left, right, bottom, top, near_val, far_val);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glFrustum,
    AROS_LHA(GLdouble, left, D0),
    AROS_LHA(GLdouble, right, D1),
    AROS_LHA(GLdouble, bottom, D2),
    AROS_LHA(GLdouble, top, D3),
    AROS_LHA(GLdouble, near_val, D4),
    AROS_LHA(GLdouble, far_val, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFrustum(left, right, bottom, top, near_val, far_val);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glViewport,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglViewport(x, y, width, height);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glPushMatrix,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPushMatrix();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glPopMatrix,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPopMatrix();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glLoadIdentity,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadIdentity();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadMatrixd,
    AROS_LHA(const GLdouble *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadMatrixd(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadMatrixf,
    AROS_LHA(const GLfloat *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadMatrixf(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultMatrixd,
    AROS_LHA(const GLdouble *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultMatrixd(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultMatrixf,
    AROS_LHA(const GLfloat *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultMatrixf(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRotated,
    AROS_LHA(GLdouble, angle, D0),
    AROS_LHA(GLdouble, x, D1),
    AROS_LHA(GLdouble, y, D2),
    AROS_LHA(GLdouble, z, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRotated(angle, x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRotatef,
    AROS_LHA(GLfloat, angle, D0),
    AROS_LHA(GLfloat, x, D1),
    AROS_LHA(GLfloat, y, D2),
    AROS_LHA(GLfloat, z, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRotatef(angle, x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glScaled,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglScaled(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glScalef,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglScalef(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTranslated,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTranslated(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTranslatef,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTranslatef(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsList,
    AROS_LHA(GLuint, list, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsList(list);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteLists,
    AROS_LHA(GLuint, list, D0),
    AROS_LHA(GLsizei, range, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteLists(list, range);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLuint, glGenLists,
    AROS_LHA(GLsizei, range, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenLists(range);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glNewList,
    AROS_LHA(GLuint, list, D0),
    AROS_LHA(GLenum, mode, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNewList(list, mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glEndList,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEndList();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glCallList,
    AROS_LHA(GLuint, list, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCallList(list);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glCallLists,
    AROS_LHA(GLsizei, n, D0),
    AROS_LHA(GLenum, type, D1),
    AROS_LHA(const GLvoid *, lists, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCallLists(n, type, lists);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glListBase,
    AROS_LHA(GLuint, base, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglListBase(base);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glBegin,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBegin(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glEnd,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEnd();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertex2d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2d(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertex2f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2f(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertex2i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2i(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertex2s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2s(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertex3d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3d(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertex3f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3f(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertex3i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLint, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3i(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertex3s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    AROS_LHA(GLshort, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3s(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertex4d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    AROS_LHA(GLdouble, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4d(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertex4f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    AROS_LHA(GLfloat, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4f(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertex4i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLint, z, D2),
    AROS_LHA(GLint, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4i(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertex4s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    AROS_LHA(GLshort, z, D2),
    AROS_LHA(GLshort, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4s(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex2dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex2fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex2iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex2sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex2sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex3dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex3fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex3iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex3sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex3sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex4dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex4fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex4iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glVertex4sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertex4sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormal3b,
    AROS_LHA(GLbyte, nx, D0),
    AROS_LHA(GLbyte, ny, D1),
    AROS_LHA(GLbyte, nz, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3b(nx, ny, nz);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormal3d,
    AROS_LHA(GLdouble, nx, D0),
    AROS_LHA(GLdouble, ny, D1),
    AROS_LHA(GLdouble, nz, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3d(nx, ny, nz);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormal3f,
    AROS_LHA(GLfloat, nx, D0),
    AROS_LHA(GLfloat, ny, D1),
    AROS_LHA(GLfloat, nz, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3f(nx, ny, nz);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormal3i,
    AROS_LHA(GLint, nx, D0),
    AROS_LHA(GLint, ny, D1),
    AROS_LHA(GLint, nz, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3i(nx, ny, nz);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormal3s,
    AROS_LHA(GLshort, nx, D0),
    AROS_LHA(GLshort, ny, D1),
    AROS_LHA(GLshort, nz, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3s(nx, ny, nz);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glNormal3bv,
    AROS_LHA(const GLbyte *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3bv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glNormal3dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glNormal3fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glNormal3iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glNormal3sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormal3sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexd,
    AROS_LHA(GLdouble, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexd(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexf,
    AROS_LHA(GLfloat, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexf(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexi,
    AROS_LHA(GLint, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexi(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexs,
    AROS_LHA(GLshort, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexs(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexub,
    AROS_LHA(GLubyte, c, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexub(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexdv,
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexdv(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexfv,
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexfv(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexiv,
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexiv(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexsv,
    AROS_LHA(const GLshort *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexsv(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glIndexubv,
    AROS_LHA(const GLubyte *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexubv(c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3b,
    AROS_LHA(GLbyte, red, D0),
    AROS_LHA(GLbyte, green, D1),
    AROS_LHA(GLbyte, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3b(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3d,
    AROS_LHA(GLdouble, red, D0),
    AROS_LHA(GLdouble, green, D1),
    AROS_LHA(GLdouble, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3d(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3f,
    AROS_LHA(GLfloat, red, D0),
    AROS_LHA(GLfloat, green, D1),
    AROS_LHA(GLfloat, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3f(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3i,
    AROS_LHA(GLint, red, D0),
    AROS_LHA(GLint, green, D1),
    AROS_LHA(GLint, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3i(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3s,
    AROS_LHA(GLshort, red, D0),
    AROS_LHA(GLshort, green, D1),
    AROS_LHA(GLshort, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3s(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3ub,
    AROS_LHA(GLubyte, red, D0),
    AROS_LHA(GLubyte, green, D1),
    AROS_LHA(GLubyte, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3ub(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3ui,
    AROS_LHA(GLuint, red, D0),
    AROS_LHA(GLuint, green, D1),
    AROS_LHA(GLuint, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3ui(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColor3us,
    AROS_LHA(GLushort, red, D0),
    AROS_LHA(GLushort, green, D1),
    AROS_LHA(GLushort, blue, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3us(red, green, blue);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4b,
    AROS_LHA(GLbyte, red, D0),
    AROS_LHA(GLbyte, green, D1),
    AROS_LHA(GLbyte, blue, D2),
    AROS_LHA(GLbyte, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4b(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4d,
    AROS_LHA(GLdouble, red, D0),
    AROS_LHA(GLdouble, green, D1),
    AROS_LHA(GLdouble, blue, D2),
    AROS_LHA(GLdouble, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4d(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4f,
    AROS_LHA(GLfloat, red, D0),
    AROS_LHA(GLfloat, green, D1),
    AROS_LHA(GLfloat, blue, D2),
    AROS_LHA(GLfloat, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4f(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4i,
    AROS_LHA(GLint, red, D0),
    AROS_LHA(GLint, green, D1),
    AROS_LHA(GLint, blue, D2),
    AROS_LHA(GLint, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4i(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4s,
    AROS_LHA(GLshort, red, D0),
    AROS_LHA(GLshort, green, D1),
    AROS_LHA(GLshort, blue, D2),
    AROS_LHA(GLshort, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4s(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4ub,
    AROS_LHA(GLubyte, red, D0),
    AROS_LHA(GLubyte, green, D1),
    AROS_LHA(GLubyte, blue, D2),
    AROS_LHA(GLubyte, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4ub(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4ui,
    AROS_LHA(GLuint, red, D0),
    AROS_LHA(GLuint, green, D1),
    AROS_LHA(GLuint, blue, D2),
    AROS_LHA(GLuint, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4ui(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColor4us,
    AROS_LHA(GLushort, red, D0),
    AROS_LHA(GLushort, green, D1),
    AROS_LHA(GLushort, blue, D2),
    AROS_LHA(GLushort, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4us(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3bv,
    AROS_LHA(const GLbyte *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3bv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3ubv,
    AROS_LHA(const GLubyte *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3ubv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3uiv,
    AROS_LHA(const GLuint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3uiv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor3usv,
    AROS_LHA(const GLushort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor3usv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4bv,
    AROS_LHA(const GLbyte *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4bv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4ubv,
    AROS_LHA(const GLubyte *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4ubv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4uiv,
    AROS_LHA(const GLuint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4uiv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glColor4usv,
    AROS_LHA(const GLushort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColor4usv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1d,
    AROS_LHA(GLdouble, s, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1d(s);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1f,
    AROS_LHA(GLfloat, s, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1f(s);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1i,
    AROS_LHA(GLint, s, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1i(s);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1s,
    AROS_LHA(GLshort, s, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1s(s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexCoord2d,
    AROS_LHA(GLdouble, s, D0),
    AROS_LHA(GLdouble, t, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2d(s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexCoord2f,
    AROS_LHA(GLfloat, s, D0),
    AROS_LHA(GLfloat, t, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2f(s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexCoord2i,
    AROS_LHA(GLint, s, D0),
    AROS_LHA(GLint, t, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2i(s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexCoord2s,
    AROS_LHA(GLshort, s, D0),
    AROS_LHA(GLshort, t, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2s(s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexCoord3d,
    AROS_LHA(GLdouble, s, D0),
    AROS_LHA(GLdouble, t, D1),
    AROS_LHA(GLdouble, r, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3d(s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexCoord3f,
    AROS_LHA(GLfloat, s, D0),
    AROS_LHA(GLfloat, t, D1),
    AROS_LHA(GLfloat, r, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3f(s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexCoord3i,
    AROS_LHA(GLint, s, D0),
    AROS_LHA(GLint, t, D1),
    AROS_LHA(GLint, r, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3i(s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexCoord3s,
    AROS_LHA(GLshort, s, D0),
    AROS_LHA(GLshort, t, D1),
    AROS_LHA(GLshort, r, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3s(s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTexCoord4d,
    AROS_LHA(GLdouble, s, D0),
    AROS_LHA(GLdouble, t, D1),
    AROS_LHA(GLdouble, r, D2),
    AROS_LHA(GLdouble, q, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4d(s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTexCoord4f,
    AROS_LHA(GLfloat, s, D0),
    AROS_LHA(GLfloat, t, D1),
    AROS_LHA(GLfloat, r, D2),
    AROS_LHA(GLfloat, q, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4f(s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTexCoord4i,
    AROS_LHA(GLint, s, D0),
    AROS_LHA(GLint, t, D1),
    AROS_LHA(GLint, r, D2),
    AROS_LHA(GLint, q, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4i(s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTexCoord4s,
    AROS_LHA(GLshort, s, D0),
    AROS_LHA(GLshort, t, D1),
    AROS_LHA(GLshort, r, D2),
    AROS_LHA(GLshort, q, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4s(s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord1sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord1sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord2dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord2fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord2iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord2sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord2sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord3dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord3fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord3iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord3sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord3sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord4dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord4fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord4iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glTexCoord4sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoord4sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRasterPos2d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2d(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRasterPos2f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2f(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRasterPos2i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2i(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRasterPos2s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2s(x, y);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glRasterPos3d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3d(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glRasterPos3f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3f(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glRasterPos3i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLint, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3i(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glRasterPos3s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    AROS_LHA(GLshort, z, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3s(x, y, z);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRasterPos4d,
    AROS_LHA(GLdouble, x, D0),
    AROS_LHA(GLdouble, y, D1),
    AROS_LHA(GLdouble, z, D2),
    AROS_LHA(GLdouble, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4d(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRasterPos4f,
    AROS_LHA(GLfloat, x, D0),
    AROS_LHA(GLfloat, y, D1),
    AROS_LHA(GLfloat, z, D2),
    AROS_LHA(GLfloat, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4f(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRasterPos4i,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLint, z, D2),
    AROS_LHA(GLint, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4i(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRasterPos4s,
    AROS_LHA(GLshort, x, D0),
    AROS_LHA(GLshort, y, D1),
    AROS_LHA(GLshort, z, D2),
    AROS_LHA(GLshort, w, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4s(x, y, z, w);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos2dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos2fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos2iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos2sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos2sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos3dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos3fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos3iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos3sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos3sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos4dv,
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4dv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos4fv,
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4fv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos4iv,
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4iv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glRasterPos4sv,
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRasterPos4sv(v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRectd,
    AROS_LHA(GLdouble, x1, D0),
    AROS_LHA(GLdouble, y1, D1),
    AROS_LHA(GLdouble, x2, D2),
    AROS_LHA(GLdouble, y2, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectd(x1, y1, x2, y2);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRectf,
    AROS_LHA(GLfloat, x1, D0),
    AROS_LHA(GLfloat, y1, D1),
    AROS_LHA(GLfloat, x2, D2),
    AROS_LHA(GLfloat, y2, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectf(x1, y1, x2, y2);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRecti,
    AROS_LHA(GLint, x1, D0),
    AROS_LHA(GLint, y1, D1),
    AROS_LHA(GLint, x2, D2),
    AROS_LHA(GLint, y2, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRecti(x1, y1, x2, y2);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRects,
    AROS_LHA(GLshort, x1, D0),
    AROS_LHA(GLshort, y1, D1),
    AROS_LHA(GLshort, x2, D2),
    AROS_LHA(GLshort, y2, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRects(x1, y1, x2, y2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRectdv,
    AROS_LHA(const GLdouble *, v1, A0),
    AROS_LHA(const GLdouble *, v2, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectdv(v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRectfv,
    AROS_LHA(const GLfloat *, v1, A0),
    AROS_LHA(const GLfloat *, v2, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectfv(v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRectiv,
    AROS_LHA(const GLint *, v1, A0),
    AROS_LHA(const GLint *, v2, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectiv(v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRectsv,
    AROS_LHA(const GLshort *, v1, A0),
    AROS_LHA(const GLshort *, v2, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRectsv(v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexPointer,
    AROS_LHA(GLint, size, D0),
    AROS_LHA(GLenum, type, D1),
    AROS_LHA(GLsizei, stride, D2),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexPointer(size, type, stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glNormalPointer,
    AROS_LHA(GLenum, type, D0),
    AROS_LHA(GLsizei, stride, D1),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormalPointer(type, stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glColorPointer,
    AROS_LHA(GLint, size, D0),
    AROS_LHA(GLenum, type, D1),
    AROS_LHA(GLsizei, stride, D2),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorPointer(size, type, stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glIndexPointer,
    AROS_LHA(GLenum, type, D0),
    AROS_LHA(GLsizei, stride, D1),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexPointer(type, stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTexCoordPointer,
    AROS_LHA(GLint, size, D0),
    AROS_LHA(GLenum, type, D1),
    AROS_LHA(GLsizei, stride, D2),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoordPointer(size, type, stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glEdgeFlagPointer,
    AROS_LHA(GLsizei, stride, D0),
    AROS_LHA(const GLvoid *, ptr, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEdgeFlagPointer(stride, ptr);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetPointerv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLvoid * *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPointerv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glArrayElement,
    AROS_LHA(GLint, i, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglArrayElement(i);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glDrawArrays,
    AROS_LHA(GLenum, mode, D0),
    AROS_LHA(GLint, first, D1),
    AROS_LHA(GLsizei, count, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawArrays(mode, first, count);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glDrawElements,
    AROS_LHA(GLenum, mode, D0),
    AROS_LHA(GLsizei, count, D1),
    AROS_LHA(GLenum, type, D2),
    AROS_LHA(const GLvoid *, indices, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawElements(mode, count, type, indices);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glInterleavedArrays,
    AROS_LHA(GLenum, format, D0),
    AROS_LHA(GLsizei, stride, D1),
    AROS_LHA(const GLvoid *, pointer, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglInterleavedArrays(format, stride, pointer);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glShadeModel,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglShadeModel(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glLightf,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightf(light, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glLighti,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLighti(light, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glLightfv,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightfv(light, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glLightiv,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightiv(light, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetLightfv,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetLightfv(light, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetLightiv,
    AROS_LHA(GLenum, light, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetLightiv(light, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLightModelf,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLfloat, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightModelf(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLightModeli,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLint, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightModeli(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLightModelfv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightModelfv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLightModeliv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLightModeliv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMaterialf,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMaterialf(face, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMateriali,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMateriali(face, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMaterialfv,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMaterialfv(face, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMaterialiv,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMaterialiv(face, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMaterialfv,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMaterialfv(face, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMaterialiv,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMaterialiv(face, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glColorMaterial,
    AROS_LHA(GLenum, face, D0),
    AROS_LHA(GLenum, mode, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorMaterial(face, mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPixelZoom,
    AROS_LHA(GLfloat, xfactor, D0),
    AROS_LHA(GLfloat, yfactor, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelZoom(xfactor, yfactor);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPixelStoref,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLfloat, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelStoref(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPixelStorei,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLint, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelStorei(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPixelTransferf,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLfloat, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelTransferf(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPixelTransferi,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLint, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelTransferi(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPixelMapfv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLsizei, mapsize, D1),
    AROS_LHA(const GLfloat *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelMapfv(map, mapsize, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPixelMapuiv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLsizei, mapsize, D1),
    AROS_LHA(const GLuint *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelMapuiv(map, mapsize, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPixelMapusv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLsizei, mapsize, D1),
    AROS_LHA(const GLushort *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPixelMapusv(map, mapsize, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetPixelMapfv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLfloat *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPixelMapfv(map, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetPixelMapuiv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLuint *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPixelMapuiv(map, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetPixelMapusv,
    AROS_LHA(GLenum, map, D0),
    AROS_LHA(GLushort *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPixelMapusv(map, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glBitmap,
    AROS_LHA(GLsizei, width, D0),
    AROS_LHA(GLsizei, height, D1),
    AROS_LHA(GLfloat, xorig, D2),
    AROS_LHA(GLfloat, yorig, D3),
    AROS_LHA(GLfloat, xmove, D4),
    AROS_LHA(GLfloat, ymove, D5),
    AROS_LHA(const GLubyte *, bitmap, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBitmap(width, height, xorig, yorig, xmove, ymove, bitmap);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glReadPixels,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    AROS_LHA(GLenum, format, D4),
    AROS_LHA(GLenum, type, D5),
    AROS_LHA(GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglReadPixels(x, y, width, height, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glDrawPixels,
    AROS_LHA(GLsizei, width, D0),
    AROS_LHA(GLsizei, height, D1),
    AROS_LHA(GLenum, format, D2),
    AROS_LHA(GLenum, type, D3),
    AROS_LHA(const GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawPixels(width, height, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glCopyPixels,
    AROS_LHA(GLint, x, D0),
    AROS_LHA(GLint, y, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    AROS_LHA(GLenum, type, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyPixels(x, y, width, height, type);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glStencilFunc,
    AROS_LHA(GLenum, func, D0),
    AROS_LHA(GLint, ref, D1),
    AROS_LHA(GLuint, mask, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilFunc(func, ref, mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glStencilMask,
    AROS_LHA(GLuint, mask, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilMask(mask);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glStencilOp,
    AROS_LHA(GLenum, fail, D0),
    AROS_LHA(GLenum, zfail, D1),
    AROS_LHA(GLenum, zpass, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilOp(fail, zfail, zpass);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glClearStencil,
    AROS_LHA(GLint, s, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClearStencil(s);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGend,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLdouble, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGend(coord, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGenf,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGenf(coord, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGeni,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGeni(coord, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGendv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLdouble *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGendv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGenfv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGenfv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexGeniv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexGeniv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexGendv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLdouble *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexGendv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexGenfv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexGenfv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexGeniv,
    AROS_LHA(GLenum, coord, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexGeniv(coord, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexEnvf,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexEnvf(target, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexEnvi,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexEnvi(target, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexEnvfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexEnvfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexEnviv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexEnviv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexEnvfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexEnvfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexEnviv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexEnviv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexParameterf,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexParameterf(target, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexParameteri,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, param, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexParameteri(target, pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glTexParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetTexParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetTexLevelParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, pname, D2),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexLevelParameterfv(target, level, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetTexLevelParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, pname, D2),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexLevelParameteriv(target, level, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glTexImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, internalFormat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLint, border, D4),
    AROS_LHA(GLenum, format, D5),
    AROS_LHA(GLenum, type, D6),
    AROS_LHA(const GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexImage1D(target, level, internalFormat, width, border, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glTexImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, internalFormat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLsizei, height, D4),
    AROS_LHA(GLint, border, D5),
    AROS_LHA(GLenum, format, D6),
    AROS_LHA(GLenum, type, D7),
    AROS_LHA(const GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexImage2D(target, level, internalFormat, width, height, border, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glGetTexImage,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, format, D2),
    AROS_LHA(GLenum, type, D3),
    AROS_LHA(GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexImage(target, level, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenTextures,
    AROS_LHA(GLsizei, n, D0),
    AROS_LHA(GLuint *, textures, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenTextures(n, textures);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteTextures,
    AROS_LHA(GLsizei, n, D0),
    AROS_LHA(const GLuint *, textures, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteTextures(n, textures);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindTexture,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLuint, texture, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindTexture(target, texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPrioritizeTextures,
    AROS_LHA(GLsizei, n, D0),
    AROS_LHA(const GLuint *, textures, A0),
    AROS_LHA(const GLclampf *, priorities, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPrioritizeTextures(n, textures, priorities);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(GLboolean, glAreTexturesResident,
    AROS_LHA(GLsizei, n, D0),
    AROS_LHA(const GLuint *, textures, A0),
    AROS_LHA(GLboolean *, residences, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAreTexturesResident(n, textures, residences);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsTexture,
    AROS_LHA(GLuint, texture, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsTexture(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glTexSubImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLenum, format, D4),
    AROS_LHA(GLenum, type, D5),
    AROS_LHA(const GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage1D(target, level, xoffset, width, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glTexSubImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLsizei, width, D4),
    AROS_LHA(GLsizei, height, D5),
    AROS_LHA(GLenum, format, D6),
    AROS_LHA(GLenum, type, D7),
    AROS_LHA(const GLvoid *, pixels, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCopyTexImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLint, x, D3),
    AROS_LHA(GLint, y, D4),
    AROS_LHA(GLsizei, width, D5),
    AROS_LHA(GLint, border, D6),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexImage1D(target, level, internalformat, x, y, width, border);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCopyTexImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLint, x, D3),
    AROS_LHA(GLint, y, D4),
    AROS_LHA(GLsizei, width, D5),
    AROS_LHA(GLsizei, height, D6),
    AROS_LHA(GLint, border, D7),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexImage2D(target, level, internalformat, x, y, width, height, border);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glCopyTexSubImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, x, D3),
    AROS_LHA(GLint, y, D4),
    AROS_LHA(GLsizei, width, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage1D(target, level, xoffset, x, y, width);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCopyTexSubImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLint, x, D4),
    AROS_LHA(GLint, y, D5),
    AROS_LHA(GLsizei, width, D6),
    AROS_LHA(GLsizei, height, D7),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glMap1d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, u1, D1),
    AROS_LHA(GLdouble, u2, D2),
    AROS_LHA(GLint, stride, D3),
    AROS_LHA(GLint, order, D4),
    AROS_LHA(const GLdouble *, points, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMap1d(target, u1, u2, stride, order, points);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glMap1f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, u1, D1),
    AROS_LHA(GLfloat, u2, D2),
    AROS_LHA(GLint, stride, D3),
    AROS_LHA(GLint, order, D4),
    AROS_LHA(const GLfloat *, points, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMap1f(target, u1, u2, stride, order, points);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glMap2d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, u1, D1),
    AROS_LHA(GLdouble, u2, D2),
    AROS_LHA(GLint, ustride, D3),
    AROS_LHA(GLint, uorder, D4),
    AROS_LHA(GLdouble, v1, D5),
    AROS_LHA(GLdouble, v2, D6),
    AROS_LHA(GLint, vstride, D7),
    AROS_LHA(GLint, vorder, A0),
    AROS_LHA(const GLdouble *, points, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMap2d(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glMap2f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, u1, D1),
    AROS_LHA(GLfloat, u2, D2),
    AROS_LHA(GLint, ustride, D3),
    AROS_LHA(GLint, uorder, D4),
    AROS_LHA(GLfloat, v1, D5),
    AROS_LHA(GLfloat, v2, D6),
    AROS_LHA(GLint, vstride, D7),
    AROS_LHA(GLint, vorder, A0),
    AROS_LHA(const GLfloat *, points, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMap2f(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMapdv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, query, D1),
    AROS_LHA(GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMapdv(target, query, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMapfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, query, D1),
    AROS_LHA(GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMapfv(target, query, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMapiv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, query, D1),
    AROS_LHA(GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMapiv(target, query, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord1d,
    AROS_LHA(GLdouble, u, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord1d(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord1f,
    AROS_LHA(GLfloat, u, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord1f(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord1dv,
    AROS_LHA(const GLdouble *, u, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord1dv(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord1fv,
    AROS_LHA(const GLfloat *, u, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord1fv(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glEvalCoord2d,
    AROS_LHA(GLdouble, u, D0),
    AROS_LHA(GLdouble, v, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord2d(u, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glEvalCoord2f,
    AROS_LHA(GLfloat, u, D0),
    AROS_LHA(GLfloat, v, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord2f(u, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord2dv,
    AROS_LHA(const GLdouble *, u, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord2dv(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalCoord2fv,
    AROS_LHA(const GLfloat *, u, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalCoord2fv(u);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMapGrid1d,
    AROS_LHA(GLint, un, D0),
    AROS_LHA(GLdouble, u1, D1),
    AROS_LHA(GLdouble, u2, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapGrid1d(un, u1, u2);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMapGrid1f,
    AROS_LHA(GLint, un, D0),
    AROS_LHA(GLfloat, u1, D1),
    AROS_LHA(GLfloat, u2, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapGrid1f(un, u1, u2);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glMapGrid2d,
    AROS_LHA(GLint, un, D0),
    AROS_LHA(GLdouble, u1, D1),
    AROS_LHA(GLdouble, u2, D2),
    AROS_LHA(GLint, vn, D3),
    AROS_LHA(GLdouble, v1, D4),
    AROS_LHA(GLdouble, v2, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapGrid2d(un, u1, u2, vn, v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glMapGrid2f,
    AROS_LHA(GLint, un, D0),
    AROS_LHA(GLfloat, u1, D1),
    AROS_LHA(GLfloat, u2, D2),
    AROS_LHA(GLint, vn, D3),
    AROS_LHA(GLfloat, v1, D4),
    AROS_LHA(GLfloat, v2, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapGrid2f(un, u1, u2, vn, v1, v2);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEvalPoint1,
    AROS_LHA(GLint, i, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalPoint1(i);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glEvalPoint2,
    AROS_LHA(GLint, i, D0),
    AROS_LHA(GLint, j, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalPoint2(i, j);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glEvalMesh1,
    AROS_LHA(GLenum, mode, D0),
    AROS_LHA(GLint, i1, D1),
    AROS_LHA(GLint, i2, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalMesh1(mode, i1, i2);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glEvalMesh2,
    AROS_LHA(GLenum, mode, D0),
    AROS_LHA(GLint, i1, D1),
    AROS_LHA(GLint, i2, D2),
    AROS_LHA(GLint, j1, D3),
    AROS_LHA(GLint, j2, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEvalMesh2(mode, i1, i2, j1, j2);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glFogf,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLfloat, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogf(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glFogi,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(GLint, param, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogi(pname, param);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glFogfv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogfv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glFogiv,
    AROS_LHA(GLenum, pname, D0),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogiv(pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glFeedbackBuffer,
    AROS_LHA(GLsizei, size, D0),
    AROS_LHA(GLenum, type, D1),
    AROS_LHA(GLfloat *, buffer, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFeedbackBuffer(size, type, buffer);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPassThrough,
    AROS_LHA(GLfloat, token, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPassThrough(token);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glSelectBuffer,
    AROS_LHA(GLsizei, size, D0),
    AROS_LHA(GLuint *, buffer, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSelectBuffer(size, buffer);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glInitNames,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglInitNames();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadName,
    AROS_LHA(GLuint, name, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadName(name);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glPushName,
    AROS_LHA(GLuint, name, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPushName(name);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glPopName,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPopName();

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glDrawRangeElements,
    AROS_LHA(GLenum, mode, D0),
    AROS_LHA(GLuint, start, D1),
    AROS_LHA(GLuint, end, D2),
    AROS_LHA(GLsizei, count, D3),
    AROS_LHA(GLenum, type, D4),
    AROS_LHA(const GLvoid *, indices, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawRangeElements(mode, start, end, count, type, indices);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glTexImage3D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, internalFormat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLsizei, height, D4),
    AROS_LHA(GLsizei, depth, D5),
    AROS_LHA(GLint, border, D6),
    AROS_LHA(GLenum, format, D7),
    AROS_LHA(GLenum, type, A0),
    AROS_LHA(const GLvoid *, pixels, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH11(void, glTexSubImage3D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLint, zoffset, D4),
    AROS_LHA(GLsizei, width, D5),
    AROS_LHA(GLsizei, height, D6),
    AROS_LHA(GLsizei, depth, D7),
    AROS_LHA(GLenum, format, A0),
    AROS_LHA(GLenum, type, A1),
    AROS_LHA(const GLvoid *, pixels, A2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCopyTexSubImage3D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLint, zoffset, D4),
    AROS_LHA(GLint, x, D5),
    AROS_LHA(GLint, y, D6),
    AROS_LHA(GLsizei, width, D7),
    AROS_LHA(GLsizei, height, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glColorTable,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLenum, format, D3),
    AROS_LHA(GLenum, type, D4),
    AROS_LHA(const GLvoid *, table, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorTable(target, internalformat, width, format, type, table);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glColorSubTable,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLsizei, start, D1),
    AROS_LHA(GLsizei, count, D2),
    AROS_LHA(GLenum, format, D3),
    AROS_LHA(GLenum, type, D4),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorSubTable(target, start, count, format, type, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColorTableParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorTableParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glColorTableParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorTableParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glCopyColorSubTable,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLsizei, start, D1),
    AROS_LHA(GLint, x, D2),
    AROS_LHA(GLint, y, D3),
    AROS_LHA(GLsizei, width, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyColorSubTable(target, start, x, y, width);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glCopyColorTable,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLint, x, D2),
    AROS_LHA(GLint, y, D3),
    AROS_LHA(GLsizei, width, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyColorTable(target, internalformat, x, y, width);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetColorTable,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, format, D1),
    AROS_LHA(GLenum, type, D2),
    AROS_LHA(GLvoid *, table, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTable(target, format, type, table);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetColorTableParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTableParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetColorTableParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTableParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glBlendEquation,
    AROS_LHA(GLenum, mode, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendEquation(mode);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBlendColor,
    AROS_LHA(GLclampf, red, D0),
    AROS_LHA(GLclampf, green, D1),
    AROS_LHA(GLclampf, blue, D2),
    AROS_LHA(GLclampf, alpha, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendColor(red, green, blue, alpha);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glHistogram,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLsizei, width, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLboolean, sink, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglHistogram(target, width, internalformat, sink);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glResetHistogram,
    AROS_LHA(GLenum, target, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglResetHistogram(target);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glGetHistogram,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLboolean, reset, D1),
    AROS_LHA(GLenum, format, D2),
    AROS_LHA(GLenum, type, D3),
    AROS_LHA(GLvoid *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetHistogram(target, reset, format, type, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetHistogramParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetHistogramParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetHistogramParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetHistogramParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMinmax,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLboolean, sink, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMinmax(target, internalformat, sink);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glResetMinmax,
    AROS_LHA(GLenum, target, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglResetMinmax(target);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glGetMinmax,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLboolean, reset, D1),
    AROS_LHA(GLenum, format, D2),
    AROS_LHA(GLenum, types, D3),
    AROS_LHA(GLvoid *, values, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMinmax(target, reset, format, types, values);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMinmaxParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMinmaxParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetMinmaxParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetMinmaxParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glConvolutionFilter1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLenum, format, D3),
    AROS_LHA(GLenum, type, D4),
    AROS_LHA(const GLvoid *, image, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionFilter1D(target, internalformat, width, format, type, image);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glConvolutionFilter2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    AROS_LHA(GLenum, format, D4),
    AROS_LHA(GLenum, type, D5),
    AROS_LHA(const GLvoid *, image, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionFilter2D(target, internalformat, width, height, format, type, image);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glConvolutionParameterf,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat, params, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionParameterf(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glConvolutionParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glConvolutionParameteri,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint, params, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionParameteri(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glConvolutionParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(const GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglConvolutionParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glCopyConvolutionFilter1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLint, x, D2),
    AROS_LHA(GLint, y, D3),
    AROS_LHA(GLsizei, width, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyConvolutionFilter1D(target, internalformat, x, y, width);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glCopyConvolutionFilter2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLint, x, D2),
    AROS_LHA(GLint, y, D3),
    AROS_LHA(GLsizei, width, D4),
    AROS_LHA(GLsizei, height, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyConvolutionFilter2D(target, internalformat, x, y, width, height);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetConvolutionFilter,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, format, D1),
    AROS_LHA(GLenum, type, D2),
    AROS_LHA(GLvoid *, image, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetConvolutionFilter(target, format, type, image);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetConvolutionParameterfv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLfloat *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetConvolutionParameterfv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetConvolutionParameteriv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, pname, D1),
    AROS_LHA(GLint *, params, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetConvolutionParameteriv(target, pname, params);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glSeparableFilter2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, internalformat, D1),
    AROS_LHA(GLsizei, width, D2),
    AROS_LHA(GLsizei, height, D3),
    AROS_LHA(GLenum, format, D4),
    AROS_LHA(GLenum, type, D5),
    AROS_LHA(const GLvoid *, row, A0),
    AROS_LHA(const GLvoid *, column, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSeparableFilter2D(target, internalformat, width, height, format, type, row, column);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glGetSeparableFilter,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLenum, format, D1),
    AROS_LHA(GLenum, type, D2),
    AROS_LHA(GLvoid *, row, A0),
    AROS_LHA(GLvoid *, column, A1),
    AROS_LHA(GLvoid *, span, A2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetSeparableFilter(target, format, type, row, column, span);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glActiveTexture,
    AROS_LHA(GLenum, texture, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglActiveTexture(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glClientActiveTexture,
    AROS_LHA(GLenum, texture, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClientActiveTexture(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCompressedTexImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLint, border, D4),
    AROS_LHA(GLsizei, imageSize, D5),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCompressedTexImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLsizei, height, D4),
    AROS_LHA(GLint, border, D5),
    AROS_LHA(GLsizei, imageSize, D6),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCompressedTexImage3D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLenum, internalformat, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLsizei, height, D4),
    AROS_LHA(GLsizei, depth, D5),
    AROS_LHA(GLint, border, D6),
    AROS_LHA(GLsizei, imageSize, D7),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCompressedTexSubImage1D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLsizei, width, D3),
    AROS_LHA(GLenum, format, D4),
    AROS_LHA(GLsizei, imageSize, D5),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCompressedTexSubImage2D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLsizei, width, D4),
    AROS_LHA(GLsizei, height, D5),
    AROS_LHA(GLenum, format, D6),
    AROS_LHA(GLsizei, imageSize, D7),
    AROS_LHA(const GLvoid *, data, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH11(void, glCompressedTexSubImage3D,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, level, D1),
    AROS_LHA(GLint, xoffset, D2),
    AROS_LHA(GLint, yoffset, D3),
    AROS_LHA(GLint, zoffset, D4),
    AROS_LHA(GLsizei, width, D5),
    AROS_LHA(GLsizei, height, D6),
    AROS_LHA(GLsizei, depth, D7),
    AROS_LHA(GLenum, format, A0),
    AROS_LHA(GLsizei, imageSize, A1),
    AROS_LHA(const GLvoid *, data, A2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetCompressedTexImage,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, lod, D1),
    AROS_LHA(GLvoid *, img, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCompressedTexImage(target, lod, img);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1d(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1dv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1dv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1f(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1fv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1fv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1i,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1i(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1iv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1iv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1s,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1s(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1sv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1sv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2d(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2dv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2dv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2f(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2fv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2fv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2i,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2i(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2iv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2iv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2s,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2s(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2sv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2sv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    AROS_LHA(GLdouble, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3d(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3dv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3dv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    AROS_LHA(GLfloat, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3f(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3fv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3fv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3i,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    AROS_LHA(GLint, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3i(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3iv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3iv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3s,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    AROS_LHA(GLshort, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3s(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3sv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3sv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4d,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    AROS_LHA(GLdouble, r, D3),
    AROS_LHA(GLdouble, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4d(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4dv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4dv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4f,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    AROS_LHA(GLfloat, r, D3),
    AROS_LHA(GLfloat, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4f(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4fv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4fv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4i,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    AROS_LHA(GLint, r, D3),
    AROS_LHA(GLint, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4i(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4iv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4iv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4s,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    AROS_LHA(GLshort, r, D3),
    AROS_LHA(GLshort, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4s(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4sv,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4sv(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadTransposeMatrixd,
    AROS_LHA(const GLdouble *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadTransposeMatrixd(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadTransposeMatrixf,
    AROS_LHA(const GLfloat *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadTransposeMatrixf(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultTransposeMatrixd,
    AROS_LHA(const GLdouble *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultTransposeMatrixd(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultTransposeMatrixf,
    AROS_LHA(const GLfloat *, m, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultTransposeMatrixf(m);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glSampleCoverage,
    AROS_LHA(GLclampf, value, D0),
    AROS_LHA(GLboolean, invert, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSampleCoverage(value, invert);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glActiveTextureARB,
    AROS_LHA(GLenum, texture, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglActiveTextureARB(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glClientActiveTextureARB,
    AROS_LHA(GLenum, texture, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglClientActiveTextureARB(texture);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1dARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1dARB(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1dvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1dvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1fARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1fARB(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1fvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1fvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1iARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1iARB(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1ivARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1ivARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1sARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1sARB(target, s);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord1svARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord1svARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2dARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2dARB(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2dvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2dvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2fARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2fARB(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2fvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2fvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2iARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2iARB(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2ivARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2ivARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glMultiTexCoord2sARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2sARB(target, s, t);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord2svARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord2svARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3dARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    AROS_LHA(GLdouble, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3dARB(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3dvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3dvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3fARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    AROS_LHA(GLfloat, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3fARB(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3fvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3fvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3iARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    AROS_LHA(GLint, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3iARB(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3ivARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3ivARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiTexCoord3sARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    AROS_LHA(GLshort, r, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3sARB(target, s, t, r);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord3svARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord3svARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4dARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLdouble, s, D1),
    AROS_LHA(GLdouble, t, D2),
    AROS_LHA(GLdouble, r, D3),
    AROS_LHA(GLdouble, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4dARB(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4dvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLdouble *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4dvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4fARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLfloat, s, D1),
    AROS_LHA(GLfloat, t, D2),
    AROS_LHA(GLfloat, r, D3),
    AROS_LHA(GLfloat, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4fARB(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4fvARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLfloat *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4fvARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4iARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLint, s, D1),
    AROS_LHA(GLint, t, D2),
    AROS_LHA(GLint, r, D3),
    AROS_LHA(GLint, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4iARB(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4ivARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLint *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4ivARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiTexCoord4sARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(GLshort, s, D1),
    AROS_LHA(GLshort, t, D2),
    AROS_LHA(GLshort, r, D3),
    AROS_LHA(GLshort, q, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4sARB(target, s, t, r, q);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glMultiTexCoord4svARB,
    AROS_LHA(GLenum, target, D0),
    AROS_LHA(const GLshort *, v, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiTexCoord4svARB(target, v);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBlendFuncSeparate,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendFuncSeparate(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoordf,
    AROS_LHA(GLfloat, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordf(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoordfv,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordfv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoordd,
    AROS_LHA(GLdouble, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordd(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoorddv,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoorddv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glFogCoordPointer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLvoid *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordPointer(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiDrawArrays,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint *, b, A0),
    AROS_LHA(GLsizei *, c, A1),
    AROS_LHA(GLsizei, d, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiDrawArrays(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiDrawElements,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLsizei *, b, A0),
    AROS_LHA(GLenum, c, D1),
    AROS_LHA(const GLvoid *  *, d, A1),
    AROS_LHA(GLsizei, e, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiDrawElements(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterf,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterf(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterfv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterfv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameteri,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameteri(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameteriv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameteriv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3b,
    AROS_LHA(GLbyte, a, D0),
    AROS_LHA(GLbyte, b, D1),
    AROS_LHA(GLbyte, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3b(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3bv,
    AROS_LHA(const GLbyte *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3bv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3d,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3d(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3dv,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3dv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3f,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3f(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3fv,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3fv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3i(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3iv,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3iv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3s,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3s(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3sv,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3sv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3ub,
    AROS_LHA(GLubyte, a, D0),
    AROS_LHA(GLubyte, b, D1),
    AROS_LHA(GLubyte, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ub(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3ubv,
    AROS_LHA(const GLubyte *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ubv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3ui,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ui(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3uiv,
    AROS_LHA(const GLuint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3uiv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3us,
    AROS_LHA(GLushort, a, D0),
    AROS_LHA(GLushort, b, D1),
    AROS_LHA(GLushort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3us(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3usv,
    AROS_LHA(const GLushort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3usv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glSecondaryColorPointer,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColorPointer(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2d,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2d(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2dv,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2dv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2f,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2f(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2fv,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2fv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2i(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2iv,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2iv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2s,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2s(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2sv,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2sv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3d,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3d(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3dv,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3dv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3f,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3f(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3fv,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3fv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3i(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3iv,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3iv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3s,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3s(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3sv,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3sv(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenQueries,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenQueries(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteQueries,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteQueries(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsQuery,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsQuery(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBeginQuery,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBeginQuery(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEndQuery,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEndQuery(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryiv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryObjectiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryObjectiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryObjectuiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLuint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryObjectuiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindBuffer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindBuffer(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteBuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteBuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenBuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenBuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsBuffer,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsBuffer(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBufferData,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizeiptr, b, D1),
    AROS_LHA(const GLvoid *, c, A0),
    AROS_LHA(GLenum, d, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBufferData(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBufferSubData,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLintptr, b, D1),
    AROS_LHA(GLsizeiptr, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBufferSubData(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetBufferSubData,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLintptr, b, D1),
    AROS_LHA(GLsizeiptr, c, D2),
    AROS_LHA(GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferSubData(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLvoid*, glMapBuffer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapBuffer(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glUnmapBuffer,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUnmapBuffer(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetBufferParameteriv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferParameteriv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetBufferPointerv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *  *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferPointerv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBlendEquationSeparate,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendEquationSeparate(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDrawBuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLenum *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawBuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glStencilOpSeparate,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilOpSeparate(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glStencilFuncSeparate,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLuint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilFuncSeparate(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glStencilMaskSeparate,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglStencilMaskSeparate(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glAttachShader,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAttachShader(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glBindAttribLocation,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLchar *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindAttribLocation(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glCompileShader,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompileShader(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(GLuint, glCreateProgram,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCreateProgram();

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLuint, glCreateShader,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCreateShader(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDeleteProgram,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteProgram(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDeleteShader,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteShader(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDetachShader,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDetachShader(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDisableVertexAttribArray,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDisableVertexAttribArray(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEnableVertexAttribArray,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEnableVertexAttribArray(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glGetActiveAttrib,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei *, d, A0),
    AROS_LHA(GLint *, e, A1),
    AROS_LHA(GLenum *, f, A2),
    AROS_LHA(GLchar *, g, A3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetActiveAttrib(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glGetActiveUniform,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei *, d, A0),
    AROS_LHA(GLint *, e, A1),
    AROS_LHA(GLenum *, f, A2),
    AROS_LHA(GLchar *, g, A3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetActiveUniform(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetAttachedShaders,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLuint *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetAttachedShaders(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLint, glGetAttribLocation,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLchar *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetAttribLocation(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetProgramInfoLog,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLchar *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramInfoLog(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetShaderiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetShaderiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetShaderInfoLog,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLchar *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetShaderInfoLog(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetShaderSource,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLchar *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetShaderSource(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLint, glGetUniformLocation,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLchar *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformLocation(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetUniformfv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformfv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetUniformiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribdv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribdv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribfv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribfv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribiv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribPointerv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *  *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribPointerv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsProgram,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsProgram(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsShader,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsShader(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLinkProgram,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLinkProgram(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glShaderSource,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLchar *  *, c, A0),
    AROS_LHA(const GLint *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglShaderSource(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glUseProgram,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUseProgram(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glUniform1f,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1f(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2f,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2f(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniform3f,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3f(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glUniform4f,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4f(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glUniform1i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1i(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2i(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniform3i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3i(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glUniform4i,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4i(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform1fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1fv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2fv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform3fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3fv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform4fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4fv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform1iv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1iv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2iv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2iv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform3iv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3iv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform4iv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4iv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix2fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix2fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix3fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix3fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix4fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix4fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glValidateProgram,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglValidateProgram(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1d,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1d(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1dv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1dv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1f,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1f(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1fv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1fv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1s,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1s(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1sv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1sv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2d,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2d(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2dv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2dv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2f,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2f(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2fv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2fv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2s,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2s(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2sv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2sv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3d,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3d(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3dv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3dv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3f,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3f(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3fv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3fv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3s,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3s(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3sv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3sv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Nbv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLbyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nbv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Niv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Niv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Nsv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nsv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4Nub,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLubyte, b, D1),
    AROS_LHA(GLubyte, c, D2),
    AROS_LHA(GLubyte, d, D3),
    AROS_LHA(GLubyte, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nub(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Nubv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLubyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nubv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Nuiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nuiv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4Nusv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLushort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4Nusv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4bv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLbyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4bv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4d,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4d(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4dv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4dv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4f,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4f(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4fv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4fv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4iv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4iv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4s,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    AROS_LHA(GLshort, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4s(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4sv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4sv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4ubv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLubyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4ubv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4uiv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4uiv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4usv,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLushort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4usv(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glVertexAttribPointer,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLboolean, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(const GLvoid *, f, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribPointer(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix2x3fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix2x3fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix3x2fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix3x2fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix2x4fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix2x4fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix4x2fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix4x2fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix3x4fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix3x4fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix4x3fv,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix4x3fv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadTransposeMatrixfARB,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadTransposeMatrixfARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLoadTransposeMatrixdARB,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadTransposeMatrixdARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultTransposeMatrixfARB,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultTransposeMatrixfARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glMultTransposeMatrixdARB,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultTransposeMatrixdARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glSampleCoverageARB,
    AROS_LHA(GLclampf, a, D0),
    AROS_LHA(GLboolean, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSampleCoverageARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCompressedTexImage3DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLint, g, D6),
    AROS_LHA(GLsizei, h, D7),
    AROS_LHA(const GLvoid *, i, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage3DARB(a, b, c, d, e, f, g, h, i);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCompressedTexImage2DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(GLint, f, D5),
    AROS_LHA(GLsizei, g, D6),
    AROS_LHA(const GLvoid *, h, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage2DARB(a, b, c, d, e, f, g, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCompressedTexImage1DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(const GLvoid *, g, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexImage1DARB(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH11(void, glCompressedTexSubImage3DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLsizei, g, D6),
    AROS_LHA(GLsizei, h, D7),
    AROS_LHA(GLenum, i, A0),
    AROS_LHA(GLsizei, j, A1),
    AROS_LHA(const GLvoid *, k, A2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage3DARB(a, b, c, d, e, f, g, h, i, j, k);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCompressedTexSubImage2DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLenum, g, D6),
    AROS_LHA(GLsizei, h, D7),
    AROS_LHA(const GLvoid *, i, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage2DARB(a, b, c, d, e, f, g, h, i);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCompressedTexSubImage1DARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(const GLvoid *, g, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompressedTexSubImage1DARB(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetCompressedTexImageARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLvoid *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCompressedTexImageARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterfARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterfARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterfvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterfvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2dARB,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2dARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2dvARB,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2dvARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2fARB,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2fARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2fvARB,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2fvARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2iARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2ivARB,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2ivARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2sARB,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2sARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2svARB,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2svARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3dARB,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3dARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3dvARB,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3dvARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3fARB,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3fARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3fvARB,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3fvARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3iARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3ivARB,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3ivARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3sARB,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3sARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3svARB,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3svARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1dARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1dARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1dvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1dvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1fARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1fARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1fvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1fvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1sARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1sARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1svARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1svARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2dARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2dARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2dvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2dvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2fARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2fARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2fvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2fvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2sARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2sARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2svARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2svARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3dARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3dARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3dvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3dvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3fARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3fARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3fvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3fvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3sARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3sARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3svARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3svARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NbvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLbyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NbvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NivARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NsvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NsvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4NubARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLubyte, b, D1),
    AROS_LHA(GLubyte, c, D2),
    AROS_LHA(GLubyte, d, D3),
    AROS_LHA(GLubyte, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NubARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NubvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLubyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NubvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NuivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NuivARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4NusvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLushort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4NusvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4bvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLbyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4bvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4dARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4dARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4dvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4dvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4fARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4fARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4fvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4fvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4ivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4ivARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4sARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    AROS_LHA(GLshort, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4sARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4svARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4svARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4ubvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLubyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4ubvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4uivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4uivARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4usvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLushort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4usvARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glVertexAttribPointerARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLboolean, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(const GLvoid *, f, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribPointerARB(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEnableVertexAttribArrayARB,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEnableVertexAttribArrayARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDisableVertexAttribArrayARB,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDisableVertexAttribArrayARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glProgramStringARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramStringARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindProgramARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindProgramARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteProgramsARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteProgramsARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenProgramsARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenProgramsARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramEnvParameter4dARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    AROS_LHA(GLdouble, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramEnvParameter4dARB(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramEnvParameter4dvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramEnvParameter4dvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramEnvParameter4fARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    AROS_LHA(GLfloat, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramEnvParameter4fARB(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramEnvParameter4fvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramEnvParameter4fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramLocalParameter4dARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    AROS_LHA(GLdouble, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramLocalParameter4dARB(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramLocalParameter4dvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramLocalParameter4dvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramLocalParameter4fARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    AROS_LHA(GLfloat, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramLocalParameter4fARB(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramLocalParameter4fvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramLocalParameter4fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramEnvParameterdvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramEnvParameterdvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramEnvParameterfvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramEnvParameterfvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramLocalParameterdvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramLocalParameterdvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramLocalParameterfvARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramLocalParameterfvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramivARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramStringARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramStringARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribdvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribdvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribfvARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribfvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribPointervARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *  *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribPointervARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsProgramARB,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsProgramARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindBufferARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindBufferARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteBuffersARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteBuffersARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenBuffersARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenBuffersARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsBufferARB,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsBufferARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBufferDataARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizeiptrARB, b, D1),
    AROS_LHA(const GLvoid *, c, A0),
    AROS_LHA(GLenum, d, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBufferDataARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBufferSubDataARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLintptrARB, b, D1),
    AROS_LHA(GLsizeiptrARB, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBufferSubDataARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetBufferSubDataARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLintptrARB, b, D1),
    AROS_LHA(GLsizeiptrARB, c, D2),
    AROS_LHA(GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferSubDataARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLvoid*, glMapBufferARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMapBufferARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glUnmapBufferARB,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUnmapBufferARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetBufferParameterivARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferParameterivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetBufferPointervARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *  *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetBufferPointervARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenQueriesARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenQueriesARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteQueriesARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteQueriesARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsQueryARB,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsQueryARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBeginQueryARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBeginQueryARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glEndQueryARB,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEndQueryARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryivARB,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryObjectivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryObjectivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetQueryObjectuivARB,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLuint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetQueryObjectuivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDeleteObjectARB,
    AROS_LHA(GLhandleARB, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteObjectARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLhandleARB, glGetHandleARB,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetHandleARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDetachObjectARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLhandleARB, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDetachObjectARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLhandleARB, glCreateShaderObjectARB,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCreateShaderObjectARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glShaderSourceARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLcharARB *  *, c, A0),
    AROS_LHA(const GLint *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglShaderSourceARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glCompileShaderARB,
    AROS_LHA(GLhandleARB, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCompileShaderARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(GLhandleARB, glCreateProgramObjectARB,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCreateProgramObjectARB();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glAttachObjectARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLhandleARB, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAttachObjectARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glLinkProgramARB,
    AROS_LHA(GLhandleARB, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLinkProgramARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glUseProgramObjectARB,
    AROS_LHA(GLhandleARB, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUseProgramObjectARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glValidateProgramARB,
    AROS_LHA(GLhandleARB, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglValidateProgramARB(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glUniform1fARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1fARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2fARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2fARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniform3fARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3fARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glUniform4fARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4fARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glUniform1iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1iARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2iARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniform3iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3iARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glUniform4iARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4iARB(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform1fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform3fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform4fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4fvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform1ivARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform1ivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform2ivARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform2ivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform3ivARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform3ivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glUniform4ivARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniform4ivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix2fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix2fvARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix3fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix3fvARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glUniformMatrix4fvARB,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLboolean, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUniformMatrix4fvARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetObjectParameterfvARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetObjectParameterfvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetObjectParameterivARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetObjectParameterivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetInfoLogARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLcharARB *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetInfoLogARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetAttachedObjectsARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLhandleARB *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetAttachedObjectsARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLint, glGetUniformLocationARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(const GLcharARB *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformLocationARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glGetActiveUniformARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei *, d, A0),
    AROS_LHA(GLint *, e, A1),
    AROS_LHA(GLenum *, f, A2),
    AROS_LHA(GLcharARB *, g, A3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetActiveUniformARB(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetUniformfvARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformfvARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetUniformivARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetUniformivARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetShaderSourceARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei *, c, A0),
    AROS_LHA(GLcharARB *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetShaderSourceARB(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glBindAttribLocationARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLcharARB *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindAttribLocationARB(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glGetActiveAttribARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei *, d, A0),
    AROS_LHA(GLint *, e, A1),
    AROS_LHA(GLenum *, f, A2),
    AROS_LHA(GLcharARB *, g, A3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetActiveAttribARB(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(GLint, glGetAttribLocationARB,
    AROS_LHA(GLhandleARB, a, D0),
    AROS_LHA(const GLcharARB *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetAttribLocationARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDrawBuffersARB,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLenum *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawBuffersARB(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsRenderbuffer,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsRenderbuffer(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindRenderbuffer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindRenderbuffer(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteRenderbuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteRenderbuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenRenderbuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenRenderbuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRenderbufferStorage,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRenderbufferStorage(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetRenderbufferParameteriv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetRenderbufferParameteriv(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsFramebuffer,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsFramebuffer(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindFramebuffer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindFramebuffer(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteFramebuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteFramebuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenFramebuffers,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenFramebuffers(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLenum, glCheckFramebufferStatus,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCheckFramebufferStatus(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTexture1D,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture1D(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTexture2D,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture2D(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glFramebufferTexture3D,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLint, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture3D(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glFramebufferRenderbuffer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferRenderbuffer(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetFramebufferAttachmentParameteriv,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetFramebufferAttachmentParameteriv(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glGenerateMipmap,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenerateMipmap(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glBlitFramebuffer,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLint, f, D5),
    AROS_LHA(GLint, g, D6),
    AROS_LHA(GLint, h, D7),
    AROS_LHA(GLbitfield, i, A0),
    AROS_LHA(GLenum, j, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlitFramebuffer(a, b, c, d, e, f, g, h, i, j);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glRenderbufferStorageMultisample,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLsizei, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRenderbufferStorageMultisample(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTextureLayer,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTextureLayer(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBlendColorEXT,
    AROS_LHA(GLclampf, a, D0),
    AROS_LHA(GLclampf, b, D1),
    AROS_LHA(GLclampf, c, D2),
    AROS_LHA(GLclampf, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendColorEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPolygonOffsetEXT,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPolygonOffsetEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glTexImage3DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLint, g, D6),
    AROS_LHA(GLenum, h, D7),
    AROS_LHA(GLenum, i, A0),
    AROS_LHA(const GLvoid *, j, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexImage3DEXT(a, b, c, d, e, f, g, h, i, j);

    AROS_LIBFUNC_EXIT
}

AROS_LH11(void, glTexSubImage3DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLsizei, g, D6),
    AROS_LHA(GLsizei, h, D7),
    AROS_LHA(GLenum, i, A0),
    AROS_LHA(GLenum, j, A1),
    AROS_LHA(const GLvoid *, k, A2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage3DEXT(a, b, c, d, e, f, g, h, i, j, k);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glTexSubImage1DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(GLenum, f, D5),
    AROS_LHA(const GLvoid *, g, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage1DEXT(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glTexSubImage2DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLsizei, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLenum, g, D6),
    AROS_LHA(GLenum, h, D7),
    AROS_LHA(const GLvoid *, i, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexSubImage2DEXT(a, b, c, d, e, f, g, h, i);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glCopyTexImage1DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLint, g, D6),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexImage1DEXT(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCopyTexImage2DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    AROS_LHA(GLsizei, g, D6),
    AROS_LHA(GLint, h, D7),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexImage2DEXT(a, b, c, d, e, f, g, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glCopyTexSubImage1DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLsizei, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage1DEXT(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH8(void, glCopyTexSubImage2DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLint, f, D5),
    AROS_LHA(GLsizei, g, D6),
    AROS_LHA(GLsizei, h, D7),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage2DEXT(a, b, c, d, e, f, g, h);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glCopyTexSubImage3DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLint, f, D5),
    AROS_LHA(GLint, g, D6),
    AROS_LHA(GLsizei, h, D7),
    AROS_LHA(GLsizei, i, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCopyTexSubImage3DEXT(a, b, c, d, e, f, g, h, i);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(GLboolean, glAreTexturesResidentEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    AROS_LHA(GLboolean *, c, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAreTexturesResidentEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindTextureEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindTextureEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteTexturesEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteTexturesEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenTexturesEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenTexturesEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsTextureEXT,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsTextureEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPrioritizeTexturesEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    AROS_LHA(const GLclampf *, c, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPrioritizeTexturesEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glArrayElementEXT,
    AROS_LHA(GLint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglArrayElementEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glColorPointerEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(const GLvoid *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorPointerEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glDrawArraysEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawArraysEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glEdgeFlagPointerEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLboolean *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEdgeFlagPointerEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetPointervEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLvoid *  *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetPointervEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glIndexPointerEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIndexPointerEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glNormalPointerEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglNormalPointerEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glTexCoordPointerEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(const GLvoid *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexCoordPointerEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexPointerEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(const GLvoid *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexPointerEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glBlendEquationEXT,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendEquationEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterfEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterfEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterfvEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterfvEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glColorTableEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLenum, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(const GLvoid *, f, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorTableEXT(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetColorTableEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTableEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetColorTableParameterivEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTableParameterivEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetColorTableParameterfvEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetColorTableParameterfvEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glLockArraysEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLockArraysEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glUnlockArraysEXT,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglUnlockArraysEXT();

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glDrawRangeElementsEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(const GLvoid *, f, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawRangeElementsEXT(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3bEXT,
    AROS_LHA(GLbyte, a, D0),
    AROS_LHA(GLbyte, b, D1),
    AROS_LHA(GLbyte, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3bEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3bvEXT,
    AROS_LHA(const GLbyte *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3bvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3dEXT,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3dEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3dvEXT,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3dvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3fEXT,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3fEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3fvEXT,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3fvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3iEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3iEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3ivEXT,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ivEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3sEXT,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3sEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3svEXT,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3svEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3ubEXT,
    AROS_LHA(GLubyte, a, D0),
    AROS_LHA(GLubyte, b, D1),
    AROS_LHA(GLubyte, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ubEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3ubvEXT,
    AROS_LHA(const GLubyte *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3ubvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3uiEXT,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3uiEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3uivEXT,
    AROS_LHA(const GLuint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3uivEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSecondaryColor3usEXT,
    AROS_LHA(GLushort, a, D0),
    AROS_LHA(GLushort, b, D1),
    AROS_LHA(GLushort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3usEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glSecondaryColor3usvEXT,
    AROS_LHA(const GLushort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColor3usvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glSecondaryColorPointerEXT,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLvoid *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSecondaryColorPointerEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glMultiDrawArraysEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint *, b, A0),
    AROS_LHA(GLsizei *, c, A1),
    AROS_LHA(GLsizei, d, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiDrawArraysEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glMultiDrawElementsEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLsizei *, b, A0),
    AROS_LHA(GLenum, c, D1),
    AROS_LHA(const GLvoid *  *, d, A1),
    AROS_LHA(GLsizei, e, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglMultiDrawElementsEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoordfEXT,
    AROS_LHA(GLfloat, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordfEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoordfvEXT,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordfvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoorddEXT,
    AROS_LHA(GLdouble, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoorddEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glFogCoorddvEXT,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoorddvEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glFogCoordPointerEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLvoid *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFogCoordPointerEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glBlendFuncSeparateEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBlendFuncSeparateEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glFlushVertexArrayRangeNV,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFlushVertexArrayRangeNV();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexArrayRangeNV,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLvoid *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexArrayRangeNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glCombinerParameterfvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerParameterfvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glCombinerParameterfNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerParameterfNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glCombinerParameterivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerParameterivNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glCombinerParameteriNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerParameteriNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glCombinerInputNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(GLenum, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerInputNV(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glCombinerOutputNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    AROS_LHA(GLenum, e, D4),
    AROS_LHA(GLenum, f, D5),
    AROS_LHA(GLenum, g, D6),
    AROS_LHA(GLboolean, h, D7),
    AROS_LHA(GLboolean, i, A0),
    AROS_LHA(GLboolean, j, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCombinerOutputNV(a, b, c, d, e, f, g, h, i, j);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glFinalCombinerInputNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFinalCombinerInputNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glGetCombinerInputParameterfvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    AROS_LHA(GLfloat *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCombinerInputParameterfvNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glGetCombinerInputParameterivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    AROS_LHA(GLint *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCombinerInputParameterivNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetCombinerOutputParameterfvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCombinerOutputParameterfvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetCombinerOutputParameterivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetCombinerOutputParameterivNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetFinalCombinerInputParameterfvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetFinalCombinerInputParameterfvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetFinalCombinerInputParameterivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetFinalCombinerInputParameterivNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glResizeBuffersMESA,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglResizeBuffersMESA();

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2dMESA,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2dMESA(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2dvMESA,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2dvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2fMESA,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2fMESA(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2fvMESA,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2fvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2iMESA,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2iMESA(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2ivMESA,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2ivMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glWindowPos2sMESA,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2sMESA(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos2svMESA,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos2svMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3dMESA,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3dMESA(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3dvMESA,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3dvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3fMESA,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3fMESA(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3fvMESA,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3fvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3iMESA,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3iMESA(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3ivMESA,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3ivMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glWindowPos3sMESA,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3sMESA(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos3svMESA,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos3svMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glWindowPos4dMESA,
    AROS_LHA(GLdouble, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4dMESA(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos4dvMESA,
    AROS_LHA(const GLdouble *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4dvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glWindowPos4fMESA,
    AROS_LHA(GLfloat, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4fMESA(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos4fvMESA,
    AROS_LHA(const GLfloat *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4fvMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glWindowPos4iMESA,
    AROS_LHA(GLint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLint, c, D2),
    AROS_LHA(GLint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4iMESA(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos4ivMESA,
    AROS_LHA(const GLint *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4ivMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glWindowPos4sMESA,
    AROS_LHA(GLshort, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4sMESA(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glWindowPos4svMESA,
    AROS_LHA(const GLshort *, a, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglWindowPos4svMESA(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(GLboolean, glAreProgramsResidentNV,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    AROS_LHA(GLboolean *, c, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAreProgramsResidentNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindProgramNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindProgramNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteProgramsNV,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteProgramsNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glExecuteProgramNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglExecuteProgramNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenProgramsNV,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenProgramsNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetProgramParameterdvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLdouble *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramParameterdvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetProgramParameterfvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramParameterfvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramivNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramivNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetProgramStringNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLubyte *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramStringNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetTrackMatrixivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTrackMatrixivNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribdvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribdvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribfvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribfvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribivNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribivNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetVertexAttribPointervNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLvoid *  *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetVertexAttribPointervNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsProgramNV,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsProgramNV(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glLoadProgramNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(const GLubyte *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglLoadProgramNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramParameter4dNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    AROS_LHA(GLdouble, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameter4dNV(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramParameter4dvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameter4dvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glProgramParameter4fNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    AROS_LHA(GLfloat, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameter4fNV(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glProgramParameter4fvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameter4fvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glProgramParameters4dvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(const GLdouble *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameters4dvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glProgramParameters4fvNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(const GLfloat *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramParameters4fvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glRequestResidentProgramsNV,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRequestResidentProgramsNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glTrackMatrixNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLenum, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTrackMatrixNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttribPointerNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLint, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLsizei, d, D3),
    AROS_LHA(const GLvoid *, e, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribPointerNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1dNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1dNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1dvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1fNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1fNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1fvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1sNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1sNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib1svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib1svNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2dNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2dNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2dvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2fNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2fNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2fvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttrib2sNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2sNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib2svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib2svNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3dNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3dNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3dvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3fNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3fNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3fvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glVertexAttrib3sNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3sNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib3svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib3svNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4dNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLdouble, b, D1),
    AROS_LHA(GLdouble, c, D2),
    AROS_LHA(GLdouble, d, D3),
    AROS_LHA(GLdouble, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4dNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLdouble *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4dvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4fNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLfloat, b, D1),
    AROS_LHA(GLfloat, c, D2),
    AROS_LHA(GLfloat, d, D3),
    AROS_LHA(GLfloat, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4fNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4fvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4sNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLshort, b, D1),
    AROS_LHA(GLshort, c, D2),
    AROS_LHA(GLshort, d, D3),
    AROS_LHA(GLshort, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4sNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLshort *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4svNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glVertexAttrib4ubNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLubyte, b, D1),
    AROS_LHA(GLubyte, c, D2),
    AROS_LHA(GLubyte, d, D3),
    AROS_LHA(GLubyte, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4ubNV(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glVertexAttrib4ubvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLubyte *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttrib4ubvNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs1dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs1dvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs1fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs1fvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs1svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLshort *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs1svNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs2dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs2dvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs2fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs2fvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs2svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLshort *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs2svNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs3dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs3dvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs3fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs3fvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs3svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLshort *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs3svNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs4dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLdouble *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs4dvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs4fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLfloat *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs4fvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs4svNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLshort *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs4svNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glVertexAttribs4ubvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglVertexAttribs4ubvNV(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexBumpParameterivATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexBumpParameterivATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glTexBumpParameterfvATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglTexBumpParameterfvATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetTexBumpParameterivATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexBumpParameterivATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGetTexBumpParameterfvATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetTexBumpParameterfvATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLuint, glGenFragmentShadersATI,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenFragmentShadersATI(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glBindFragmentShaderATI,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindFragmentShaderATI(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glDeleteFragmentShaderATI,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteFragmentShaderATI(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glBeginFragmentShaderATI,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBeginFragmentShaderATI();

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, glEndFragmentShaderATI,
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglEndFragmentShaderATI();

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glPassTexCoordATI,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPassTexCoordATI(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glSampleMapATI,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLenum, c, D2),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSampleMapATI(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glColorFragmentOp1ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    AROS_LHA(GLuint, g, D6),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorFragmentOp1ATI(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH10(void, glColorFragmentOp2ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    AROS_LHA(GLuint, g, D6),
    AROS_LHA(GLuint, h, D7),
    AROS_LHA(GLuint, i, A0),
    AROS_LHA(GLuint, j, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorFragmentOp2ATI(a, b, c, d, e, f, g, h, i, j);

    AROS_LIBFUNC_EXIT
}

AROS_LH13(void, glColorFragmentOp3ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    AROS_LHA(GLuint, g, D6),
    AROS_LHA(GLuint, h, D7),
    AROS_LHA(GLuint, i, A0),
    AROS_LHA(GLuint, j, A1),
    AROS_LHA(GLuint, k, A2),
    AROS_LHA(GLuint, l, A3),
    AROS_LHA(GLuint, m, A4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglColorFragmentOp3ATI(a, b, c, d, e, f, g, h, i, j, k, l, m);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glAlphaFragmentOp1ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAlphaFragmentOp1ATI(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH9(void, glAlphaFragmentOp2ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    AROS_LHA(GLuint, g, D6),
    AROS_LHA(GLuint, h, D7),
    AROS_LHA(GLuint, i, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAlphaFragmentOp2ATI(a, b, c, d, e, f, g, h, i);

    AROS_LIBFUNC_EXIT
}

AROS_LH12(void, glAlphaFragmentOp3ATI,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLuint, e, D4),
    AROS_LHA(GLuint, f, D5),
    AROS_LHA(GLuint, g, D6),
    AROS_LHA(GLuint, h, D7),
    AROS_LHA(GLuint, i, A0),
    AROS_LHA(GLuint, j, A1),
    AROS_LHA(GLuint, k, A2),
    AROS_LHA(GLuint, l, A3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglAlphaFragmentOp3ATI(a, b, c, d, e, f, g, h, i, j, k, l);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glSetFragmentShaderConstantATI,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(const GLfloat *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglSetFragmentShaderConstantATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameteriNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameteriNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glPointParameterivNV,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(const GLint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglPointParameterivNV(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDrawBuffersATI,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLenum *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDrawBuffersATI(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glProgramNamedParameter4fNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(GLfloat, d, D2),
    AROS_LHA(GLfloat, e, D3),
    AROS_LHA(GLfloat, f, D4),
    AROS_LHA(GLfloat, g, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramNamedParameter4fNV(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH7(void, glProgramNamedParameter4dNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(GLdouble, d, D2),
    AROS_LHA(GLdouble, e, D3),
    AROS_LHA(GLdouble, f, D4),
    AROS_LHA(GLdouble, g, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramNamedParameter4dNV(a, b, c, d, e, f, g);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glProgramNamedParameter4fvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(const GLfloat *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramNamedParameter4fvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glProgramNamedParameter4dvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(const GLdouble *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglProgramNamedParameter4dvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetProgramNamedParameterfvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(GLfloat *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramNamedParameterfvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetProgramNamedParameterdvNV,
    AROS_LHA(GLuint, a, D0),
    AROS_LHA(GLsizei, b, D1),
    AROS_LHA(const GLubyte *, c, A0),
    AROS_LHA(GLdouble *, d, A1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetProgramNamedParameterdvNV(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsRenderbufferEXT,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsRenderbufferEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindRenderbufferEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindRenderbufferEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteRenderbuffersEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteRenderbuffersEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenRenderbuffersEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenRenderbuffersEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glRenderbufferStorageEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLsizei, c, D2),
    AROS_LHA(GLsizei, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglRenderbufferStorageEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH3(void, glGetRenderbufferParameterivEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLint *, c, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetRenderbufferParameterivEXT(a, b, c);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLboolean, glIsFramebufferEXT,
    AROS_LHA(GLuint, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglIsFramebufferEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glBindFramebufferEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLuint, b, D1),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglBindFramebufferEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glDeleteFramebuffersEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(const GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglDeleteFramebuffersEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH2(void, glGenFramebuffersEXT,
    AROS_LHA(GLsizei, a, D0),
    AROS_LHA(GLuint *, b, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenFramebuffersEXT(a, b);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(GLenum, glCheckFramebufferStatusEXT,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglCheckFramebufferStatusEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTexture1DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture1DEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTexture2DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture2DEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}

AROS_LH6(void, glFramebufferTexture3DEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    AROS_LHA(GLint, e, D4),
    AROS_LHA(GLint, f, D5),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTexture3DEXT(a, b, c, d, e, f);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glFramebufferRenderbufferEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLuint, d, D3),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferRenderbufferEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH4(void, glGetFramebufferAttachmentParameterivEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLenum, c, D2),
    AROS_LHA(GLint *, d, A0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGetFramebufferAttachmentParameterivEXT(a, b, c, d);

    AROS_LIBFUNC_EXIT
}

AROS_LH1(void, glGenerateMipmapEXT,
    AROS_LHA(GLenum, a, D0),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglGenerateMipmapEXT(a);

    AROS_LIBFUNC_EXIT
}

AROS_LH5(void, glFramebufferTextureLayerEXT,
    AROS_LHA(GLenum, a, D0),
    AROS_LHA(GLenum, b, D1),
    AROS_LHA(GLuint, c, D2),
    AROS_LHA(GLint, d, D3),
    AROS_LHA(GLint, e, D4),
    struct Library *, MesaBase, 0, Mesa)
{
    AROS_LIBFUNC_INIT

    PUT_MESABASE_IN_REG

    return mglFramebufferTextureLayerEXT(a, b, c, d, e);

    AROS_LIBFUNC_EXIT
}