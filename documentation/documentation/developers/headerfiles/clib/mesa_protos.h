#ifndef CLIB_MESA_PROTOS_H
#define CLIB_MESA_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mesa/src/mesa/arosmesa.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#if defined(USE_MGL_NAMESPACE)
#undef  USE_MGL_NAMESPACE
#endif
#if defined(GL_MESA_trace)
#undef  GL_MESA_trace
#endif
#include <GL/arosmesa.h>

__BEGIN_DECLS

AROSMesaContext AROSMesaCreateContextTags(long Tag1, ...);
AROSMesaContext AROSMesaCreateContext(struct TagItem *tagList);
AROS_LP1(void, AROSMesaDestroyContext,
         AROS_LPA(AROSMesaContext, amesa, A0),
         LIBBASETYPEPTR, MesaBase, 7, Mesa
);
AROS_LP1(void, AROSMesaMakeCurrent,
         AROS_LPA(AROSMesaContext, amesa, A0),
         LIBBASETYPEPTR, MesaBase, 8, Mesa
);
AROS_LP1(void, AROSMesaSwapBuffers,
         AROS_LPA(AROSMesaContext, amesa, A0),
         LIBBASETYPEPTR, MesaBase, 9, Mesa
);
AROS_LP0(AROSMesaContext, AROSMesaGetCurrentContext,
         LIBBASETYPEPTR, MesaBase, 10, Mesa
);
AROS_LP1(void, glClearIndex,
         AROS_LPA(GLfloat, c, D0),
         LIBBASETYPEPTR, MesaBase, 35, Mesa
);
AROS_LP4(void, glClearColor,
         AROS_LPA(GLclampf, red, D0),
         AROS_LPA(GLclampf, green, D1),
         AROS_LPA(GLclampf, blue, D2),
         AROS_LPA(GLclampf, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 36, Mesa
);
AROS_LP1(void, glClear,
         AROS_LPA(GLbitfield, mask, D0),
         LIBBASETYPEPTR, MesaBase, 37, Mesa
);
AROS_LP1(void, glIndexMask,
         AROS_LPA(GLuint, mask, D0),
         LIBBASETYPEPTR, MesaBase, 38, Mesa
);
AROS_LP4(void, glColorMask,
         AROS_LPA(GLboolean, red, D0),
         AROS_LPA(GLboolean, green, D1),
         AROS_LPA(GLboolean, blue, D2),
         AROS_LPA(GLboolean, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 39, Mesa
);
AROS_LP2(void, glAlphaFunc,
         AROS_LPA(GLenum, func, D0),
         AROS_LPA(GLclampf, ref, D1),
         LIBBASETYPEPTR, MesaBase, 40, Mesa
);
AROS_LP2(void, glBlendFunc,
         AROS_LPA(GLenum, sfactor, D0),
         AROS_LPA(GLenum, dfactor, D1),
         LIBBASETYPEPTR, MesaBase, 41, Mesa
);
AROS_LP1(void, glLogicOp,
         AROS_LPA(GLenum, opcode, D0),
         LIBBASETYPEPTR, MesaBase, 42, Mesa
);
AROS_LP1(void, glCullFace,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 43, Mesa
);
AROS_LP1(void, glFrontFace,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 44, Mesa
);
AROS_LP1(void, glPointSize,
         AROS_LPA(GLfloat, size, D0),
         LIBBASETYPEPTR, MesaBase, 45, Mesa
);
AROS_LP1(void, glLineWidth,
         AROS_LPA(GLfloat, width, D0),
         LIBBASETYPEPTR, MesaBase, 46, Mesa
);
AROS_LP2(void, glLineStipple,
         AROS_LPA(GLint, factor, D0),
         AROS_LPA(GLushort, pattern, D1),
         LIBBASETYPEPTR, MesaBase, 47, Mesa
);
AROS_LP2(void, glPolygonMode,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, mode, D1),
         LIBBASETYPEPTR, MesaBase, 48, Mesa
);
AROS_LP2(void, glPolygonOffset,
         AROS_LPA(GLfloat, factor, D0),
         AROS_LPA(GLfloat, units, D1),
         LIBBASETYPEPTR, MesaBase, 49, Mesa
);
AROS_LP1(void, glPolygonStipple,
         AROS_LPA(const GLubyte *, mask, A0),
         LIBBASETYPEPTR, MesaBase, 50, Mesa
);
AROS_LP1(void, glGetPolygonStipple,
         AROS_LPA(GLubyte *, mask, A0),
         LIBBASETYPEPTR, MesaBase, 51, Mesa
);
AROS_LP1(void, glEdgeFlag,
         AROS_LPA(GLboolean, flag, D0),
         LIBBASETYPEPTR, MesaBase, 52, Mesa
);
AROS_LP1(void, glEdgeFlagv,
         AROS_LPA(const GLboolean *, flag, A0),
         LIBBASETYPEPTR, MesaBase, 53, Mesa
);
AROS_LP4(void, glScissor,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         LIBBASETYPEPTR, MesaBase, 54, Mesa
);
AROS_LP2(void, glClipPlane,
         AROS_LPA(GLenum, plane, D0),
         AROS_LPA(const GLdouble *, equation, A0),
         LIBBASETYPEPTR, MesaBase, 55, Mesa
);
AROS_LP2(void, glGetClipPlane,
         AROS_LPA(GLenum, plane, D0),
         AROS_LPA(GLdouble *, equation, A0),
         LIBBASETYPEPTR, MesaBase, 56, Mesa
);
AROS_LP1(void, glDrawBuffer,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 57, Mesa
);
AROS_LP1(void, glReadBuffer,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 58, Mesa
);
AROS_LP1(void, glEnable,
         AROS_LPA(GLenum, cap, D0),
         LIBBASETYPEPTR, MesaBase, 59, Mesa
);
AROS_LP1(void, glDisable,
         AROS_LPA(GLenum, cap, D0),
         LIBBASETYPEPTR, MesaBase, 60, Mesa
);
AROS_LP1(GLboolean, glIsEnabled,
         AROS_LPA(GLenum, cap, D0),
         LIBBASETYPEPTR, MesaBase, 61, Mesa
);
AROS_LP1(void, glEnableClientState,
         AROS_LPA(GLenum, cap, D0),
         LIBBASETYPEPTR, MesaBase, 62, Mesa
);
AROS_LP1(void, glDisableClientState,
         AROS_LPA(GLenum, cap, D0),
         LIBBASETYPEPTR, MesaBase, 63, Mesa
);
AROS_LP2(void, glGetBooleanv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLboolean *, params, A0),
         LIBBASETYPEPTR, MesaBase, 64, Mesa
);
AROS_LP2(void, glGetDoublev,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLdouble *, params, A0),
         LIBBASETYPEPTR, MesaBase, 65, Mesa
);
AROS_LP2(void, glGetFloatv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 66, Mesa
);
AROS_LP2(void, glGetIntegerv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 67, Mesa
);
AROS_LP1(void, glPushAttrib,
         AROS_LPA(GLbitfield, mask, D0),
         LIBBASETYPEPTR, MesaBase, 68, Mesa
);
AROS_LP0(void, glPopAttrib,
         LIBBASETYPEPTR, MesaBase, 69, Mesa
);
AROS_LP1(void, glPushClientAttrib,
         AROS_LPA(GLbitfield, mask, D0),
         LIBBASETYPEPTR, MesaBase, 70, Mesa
);
AROS_LP0(void, glPopClientAttrib,
         LIBBASETYPEPTR, MesaBase, 71, Mesa
);
AROS_LP1(GLint, glRenderMode,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 72, Mesa
);
AROS_LP0(GLenum, glGetError,
         LIBBASETYPEPTR, MesaBase, 73, Mesa
);
AROS_LP1(const GLubyte *, glGetString,
         AROS_LPA(GLenum, name, D0),
         LIBBASETYPEPTR, MesaBase, 74, Mesa
);
AROS_LP0(void, glFinish,
         LIBBASETYPEPTR, MesaBase, 75, Mesa
);
AROS_LP0(void, glFlush,
         LIBBASETYPEPTR, MesaBase, 76, Mesa
);
AROS_LP2(void, glHint,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, mode, D1),
         LIBBASETYPEPTR, MesaBase, 77, Mesa
);
AROS_LP1(void, glClearDepth,
         AROS_LPA(GLclampd, depth, D0),
         LIBBASETYPEPTR, MesaBase, 78, Mesa
);
AROS_LP1(void, glDepthFunc,
         AROS_LPA(GLenum, func, D0),
         LIBBASETYPEPTR, MesaBase, 79, Mesa
);
AROS_LP1(void, glDepthMask,
         AROS_LPA(GLboolean, flag, D0),
         LIBBASETYPEPTR, MesaBase, 80, Mesa
);
AROS_LP2(void, glDepthRange,
         AROS_LPA(GLclampd, near_val, D0),
         AROS_LPA(GLclampd, far_val, D1),
         LIBBASETYPEPTR, MesaBase, 81, Mesa
);
AROS_LP4(void, glClearAccum,
         AROS_LPA(GLfloat, red, D0),
         AROS_LPA(GLfloat, green, D1),
         AROS_LPA(GLfloat, blue, D2),
         AROS_LPA(GLfloat, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 82, Mesa
);
AROS_LP2(void, glAccum,
         AROS_LPA(GLenum, op, D0),
         AROS_LPA(GLfloat, value, D1),
         LIBBASETYPEPTR, MesaBase, 83, Mesa
);
AROS_LP1(void, glMatrixMode,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 84, Mesa
);
AROS_LP6(void, glOrtho,
         AROS_LPA(GLdouble, left, D0),
         AROS_LPA(GLdouble, right, D1),
         AROS_LPA(GLdouble, bottom, D2),
         AROS_LPA(GLdouble, top, D3),
         AROS_LPA(GLdouble, near_val, D4),
         AROS_LPA(GLdouble, far_val, D5),
         LIBBASETYPEPTR, MesaBase, 85, Mesa
);
AROS_LP6(void, glFrustum,
         AROS_LPA(GLdouble, left, D0),
         AROS_LPA(GLdouble, right, D1),
         AROS_LPA(GLdouble, bottom, D2),
         AROS_LPA(GLdouble, top, D3),
         AROS_LPA(GLdouble, near_val, D4),
         AROS_LPA(GLdouble, far_val, D5),
         LIBBASETYPEPTR, MesaBase, 86, Mesa
);
AROS_LP4(void, glViewport,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         LIBBASETYPEPTR, MesaBase, 87, Mesa
);
AROS_LP0(void, glPushMatrix,
         LIBBASETYPEPTR, MesaBase, 88, Mesa
);
AROS_LP0(void, glPopMatrix,
         LIBBASETYPEPTR, MesaBase, 89, Mesa
);
AROS_LP0(void, glLoadIdentity,
         LIBBASETYPEPTR, MesaBase, 90, Mesa
);
AROS_LP1(void, glLoadMatrixd,
         AROS_LPA(const GLdouble *, m, A0),
         LIBBASETYPEPTR, MesaBase, 91, Mesa
);
AROS_LP1(void, glLoadMatrixf,
         AROS_LPA(const GLfloat *, m, A0),
         LIBBASETYPEPTR, MesaBase, 92, Mesa
);
AROS_LP1(void, glMultMatrixd,
         AROS_LPA(const GLdouble *, m, A0),
         LIBBASETYPEPTR, MesaBase, 93, Mesa
);
AROS_LP1(void, glMultMatrixf,
         AROS_LPA(const GLfloat *, m, A0),
         LIBBASETYPEPTR, MesaBase, 94, Mesa
);
AROS_LP4(void, glRotated,
         AROS_LPA(GLdouble, angle, D0),
         AROS_LPA(GLdouble, x, D1),
         AROS_LPA(GLdouble, y, D2),
         AROS_LPA(GLdouble, z, D3),
         LIBBASETYPEPTR, MesaBase, 95, Mesa
);
AROS_LP4(void, glRotatef,
         AROS_LPA(GLfloat, angle, D0),
         AROS_LPA(GLfloat, x, D1),
         AROS_LPA(GLfloat, y, D2),
         AROS_LPA(GLfloat, z, D3),
         LIBBASETYPEPTR, MesaBase, 96, Mesa
);
AROS_LP3(void, glScaled,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         LIBBASETYPEPTR, MesaBase, 97, Mesa
);
AROS_LP3(void, glScalef,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         LIBBASETYPEPTR, MesaBase, 98, Mesa
);
AROS_LP3(void, glTranslated,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         LIBBASETYPEPTR, MesaBase, 99, Mesa
);
AROS_LP3(void, glTranslatef,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         LIBBASETYPEPTR, MesaBase, 100, Mesa
);
AROS_LP1(GLboolean, glIsList,
         AROS_LPA(GLuint, list, D0),
         LIBBASETYPEPTR, MesaBase, 101, Mesa
);
AROS_LP2(void, glDeleteLists,
         AROS_LPA(GLuint, list, D0),
         AROS_LPA(GLsizei, range, D1),
         LIBBASETYPEPTR, MesaBase, 102, Mesa
);
AROS_LP1(GLuint, glGenLists,
         AROS_LPA(GLsizei, range, D0),
         LIBBASETYPEPTR, MesaBase, 103, Mesa
);
AROS_LP2(void, glNewList,
         AROS_LPA(GLuint, list, D0),
         AROS_LPA(GLenum, mode, D1),
         LIBBASETYPEPTR, MesaBase, 104, Mesa
);
AROS_LP0(void, glEndList,
         LIBBASETYPEPTR, MesaBase, 105, Mesa
);
AROS_LP1(void, glCallList,
         AROS_LPA(GLuint, list, D0),
         LIBBASETYPEPTR, MesaBase, 106, Mesa
);
AROS_LP3(void, glCallLists,
         AROS_LPA(GLsizei, n, D0),
         AROS_LPA(GLenum, type, D1),
         AROS_LPA(const GLvoid *, lists, A0),
         LIBBASETYPEPTR, MesaBase, 107, Mesa
);
AROS_LP1(void, glListBase,
         AROS_LPA(GLuint, base, D0),
         LIBBASETYPEPTR, MesaBase, 108, Mesa
);
AROS_LP1(void, glBegin,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 109, Mesa
);
AROS_LP0(void, glEnd,
         LIBBASETYPEPTR, MesaBase, 110, Mesa
);
AROS_LP2(void, glVertex2d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         LIBBASETYPEPTR, MesaBase, 111, Mesa
);
AROS_LP2(void, glVertex2f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         LIBBASETYPEPTR, MesaBase, 112, Mesa
);
AROS_LP2(void, glVertex2i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         LIBBASETYPEPTR, MesaBase, 113, Mesa
);
AROS_LP2(void, glVertex2s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         LIBBASETYPEPTR, MesaBase, 114, Mesa
);
AROS_LP3(void, glVertex3d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         LIBBASETYPEPTR, MesaBase, 115, Mesa
);
AROS_LP3(void, glVertex3f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         LIBBASETYPEPTR, MesaBase, 116, Mesa
);
AROS_LP3(void, glVertex3i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLint, z, D2),
         LIBBASETYPEPTR, MesaBase, 117, Mesa
);
AROS_LP3(void, glVertex3s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         AROS_LPA(GLshort, z, D2),
         LIBBASETYPEPTR, MesaBase, 118, Mesa
);
AROS_LP4(void, glVertex4d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         AROS_LPA(GLdouble, w, D3),
         LIBBASETYPEPTR, MesaBase, 119, Mesa
);
AROS_LP4(void, glVertex4f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         AROS_LPA(GLfloat, w, D3),
         LIBBASETYPEPTR, MesaBase, 120, Mesa
);
AROS_LP4(void, glVertex4i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLint, z, D2),
         AROS_LPA(GLint, w, D3),
         LIBBASETYPEPTR, MesaBase, 121, Mesa
);
AROS_LP4(void, glVertex4s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         AROS_LPA(GLshort, z, D2),
         AROS_LPA(GLshort, w, D3),
         LIBBASETYPEPTR, MesaBase, 122, Mesa
);
AROS_LP1(void, glVertex2dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 123, Mesa
);
AROS_LP1(void, glVertex2fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 124, Mesa
);
AROS_LP1(void, glVertex2iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 125, Mesa
);
AROS_LP1(void, glVertex2sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 126, Mesa
);
AROS_LP1(void, glVertex3dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 127, Mesa
);
AROS_LP1(void, glVertex3fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 128, Mesa
);
AROS_LP1(void, glVertex3iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 129, Mesa
);
AROS_LP1(void, glVertex3sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 130, Mesa
);
AROS_LP1(void, glVertex4dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 131, Mesa
);
AROS_LP1(void, glVertex4fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 132, Mesa
);
AROS_LP1(void, glVertex4iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 133, Mesa
);
AROS_LP1(void, glVertex4sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 134, Mesa
);
AROS_LP3(void, glNormal3b,
         AROS_LPA(GLbyte, nx, D0),
         AROS_LPA(GLbyte, ny, D1),
         AROS_LPA(GLbyte, nz, D2),
         LIBBASETYPEPTR, MesaBase, 135, Mesa
);
AROS_LP3(void, glNormal3d,
         AROS_LPA(GLdouble, nx, D0),
         AROS_LPA(GLdouble, ny, D1),
         AROS_LPA(GLdouble, nz, D2),
         LIBBASETYPEPTR, MesaBase, 136, Mesa
);
AROS_LP3(void, glNormal3f,
         AROS_LPA(GLfloat, nx, D0),
         AROS_LPA(GLfloat, ny, D1),
         AROS_LPA(GLfloat, nz, D2),
         LIBBASETYPEPTR, MesaBase, 137, Mesa
);
AROS_LP3(void, glNormal3i,
         AROS_LPA(GLint, nx, D0),
         AROS_LPA(GLint, ny, D1),
         AROS_LPA(GLint, nz, D2),
         LIBBASETYPEPTR, MesaBase, 138, Mesa
);
AROS_LP3(void, glNormal3s,
         AROS_LPA(GLshort, nx, D0),
         AROS_LPA(GLshort, ny, D1),
         AROS_LPA(GLshort, nz, D2),
         LIBBASETYPEPTR, MesaBase, 139, Mesa
);
AROS_LP1(void, glNormal3bv,
         AROS_LPA(const GLbyte *, v, A0),
         LIBBASETYPEPTR, MesaBase, 140, Mesa
);
AROS_LP1(void, glNormal3dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 141, Mesa
);
AROS_LP1(void, glNormal3fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 142, Mesa
);
AROS_LP1(void, glNormal3iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 143, Mesa
);
AROS_LP1(void, glNormal3sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 144, Mesa
);
AROS_LP1(void, glIndexd,
         AROS_LPA(GLdouble, c, D0),
         LIBBASETYPEPTR, MesaBase, 145, Mesa
);
AROS_LP1(void, glIndexf,
         AROS_LPA(GLfloat, c, D0),
         LIBBASETYPEPTR, MesaBase, 146, Mesa
);
AROS_LP1(void, glIndexi,
         AROS_LPA(GLint, c, D0),
         LIBBASETYPEPTR, MesaBase, 147, Mesa
);
AROS_LP1(void, glIndexs,
         AROS_LPA(GLshort, c, D0),
         LIBBASETYPEPTR, MesaBase, 148, Mesa
);
AROS_LP1(void, glIndexub,
         AROS_LPA(GLubyte, c, D0),
         LIBBASETYPEPTR, MesaBase, 149, Mesa
);
AROS_LP1(void, glIndexdv,
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 150, Mesa
);
AROS_LP1(void, glIndexfv,
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 151, Mesa
);
AROS_LP1(void, glIndexiv,
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 152, Mesa
);
AROS_LP1(void, glIndexsv,
         AROS_LPA(const GLshort *, c, A0),
         LIBBASETYPEPTR, MesaBase, 153, Mesa
);
AROS_LP1(void, glIndexubv,
         AROS_LPA(const GLubyte *, c, A0),
         LIBBASETYPEPTR, MesaBase, 154, Mesa
);
AROS_LP3(void, glColor3b,
         AROS_LPA(GLbyte, red, D0),
         AROS_LPA(GLbyte, green, D1),
         AROS_LPA(GLbyte, blue, D2),
         LIBBASETYPEPTR, MesaBase, 155, Mesa
);
AROS_LP3(void, glColor3d,
         AROS_LPA(GLdouble, red, D0),
         AROS_LPA(GLdouble, green, D1),
         AROS_LPA(GLdouble, blue, D2),
         LIBBASETYPEPTR, MesaBase, 156, Mesa
);
AROS_LP3(void, glColor3f,
         AROS_LPA(GLfloat, red, D0),
         AROS_LPA(GLfloat, green, D1),
         AROS_LPA(GLfloat, blue, D2),
         LIBBASETYPEPTR, MesaBase, 157, Mesa
);
AROS_LP3(void, glColor3i,
         AROS_LPA(GLint, red, D0),
         AROS_LPA(GLint, green, D1),
         AROS_LPA(GLint, blue, D2),
         LIBBASETYPEPTR, MesaBase, 158, Mesa
);
AROS_LP3(void, glColor3s,
         AROS_LPA(GLshort, red, D0),
         AROS_LPA(GLshort, green, D1),
         AROS_LPA(GLshort, blue, D2),
         LIBBASETYPEPTR, MesaBase, 159, Mesa
);
AROS_LP3(void, glColor3ub,
         AROS_LPA(GLubyte, red, D0),
         AROS_LPA(GLubyte, green, D1),
         AROS_LPA(GLubyte, blue, D2),
         LIBBASETYPEPTR, MesaBase, 160, Mesa
);
AROS_LP3(void, glColor3ui,
         AROS_LPA(GLuint, red, D0),
         AROS_LPA(GLuint, green, D1),
         AROS_LPA(GLuint, blue, D2),
         LIBBASETYPEPTR, MesaBase, 161, Mesa
);
AROS_LP3(void, glColor3us,
         AROS_LPA(GLushort, red, D0),
         AROS_LPA(GLushort, green, D1),
         AROS_LPA(GLushort, blue, D2),
         LIBBASETYPEPTR, MesaBase, 162, Mesa
);
AROS_LP4(void, glColor4b,
         AROS_LPA(GLbyte, red, D0),
         AROS_LPA(GLbyte, green, D1),
         AROS_LPA(GLbyte, blue, D2),
         AROS_LPA(GLbyte, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 163, Mesa
);
AROS_LP4(void, glColor4d,
         AROS_LPA(GLdouble, red, D0),
         AROS_LPA(GLdouble, green, D1),
         AROS_LPA(GLdouble, blue, D2),
         AROS_LPA(GLdouble, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 164, Mesa
);
AROS_LP4(void, glColor4f,
         AROS_LPA(GLfloat, red, D0),
         AROS_LPA(GLfloat, green, D1),
         AROS_LPA(GLfloat, blue, D2),
         AROS_LPA(GLfloat, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 165, Mesa
);
AROS_LP4(void, glColor4i,
         AROS_LPA(GLint, red, D0),
         AROS_LPA(GLint, green, D1),
         AROS_LPA(GLint, blue, D2),
         AROS_LPA(GLint, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 166, Mesa
);
AROS_LP4(void, glColor4s,
         AROS_LPA(GLshort, red, D0),
         AROS_LPA(GLshort, green, D1),
         AROS_LPA(GLshort, blue, D2),
         AROS_LPA(GLshort, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 167, Mesa
);
AROS_LP4(void, glColor4ub,
         AROS_LPA(GLubyte, red, D0),
         AROS_LPA(GLubyte, green, D1),
         AROS_LPA(GLubyte, blue, D2),
         AROS_LPA(GLubyte, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 168, Mesa
);
AROS_LP4(void, glColor4ui,
         AROS_LPA(GLuint, red, D0),
         AROS_LPA(GLuint, green, D1),
         AROS_LPA(GLuint, blue, D2),
         AROS_LPA(GLuint, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 169, Mesa
);
AROS_LP4(void, glColor4us,
         AROS_LPA(GLushort, red, D0),
         AROS_LPA(GLushort, green, D1),
         AROS_LPA(GLushort, blue, D2),
         AROS_LPA(GLushort, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 170, Mesa
);
AROS_LP1(void, glColor3bv,
         AROS_LPA(const GLbyte *, v, A0),
         LIBBASETYPEPTR, MesaBase, 171, Mesa
);
AROS_LP1(void, glColor3dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 172, Mesa
);
AROS_LP1(void, glColor3fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 173, Mesa
);
AROS_LP1(void, glColor3iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 174, Mesa
);
AROS_LP1(void, glColor3sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 175, Mesa
);
AROS_LP1(void, glColor3ubv,
         AROS_LPA(const GLubyte *, v, A0),
         LIBBASETYPEPTR, MesaBase, 176, Mesa
);
AROS_LP1(void, glColor3uiv,
         AROS_LPA(const GLuint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 177, Mesa
);
AROS_LP1(void, glColor3usv,
         AROS_LPA(const GLushort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 178, Mesa
);
AROS_LP1(void, glColor4bv,
         AROS_LPA(const GLbyte *, v, A0),
         LIBBASETYPEPTR, MesaBase, 179, Mesa
);
AROS_LP1(void, glColor4dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 180, Mesa
);
AROS_LP1(void, glColor4fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 181, Mesa
);
AROS_LP1(void, glColor4iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 182, Mesa
);
AROS_LP1(void, glColor4sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 183, Mesa
);
AROS_LP1(void, glColor4ubv,
         AROS_LPA(const GLubyte *, v, A0),
         LIBBASETYPEPTR, MesaBase, 184, Mesa
);
AROS_LP1(void, glColor4uiv,
         AROS_LPA(const GLuint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 185, Mesa
);
AROS_LP1(void, glColor4usv,
         AROS_LPA(const GLushort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 186, Mesa
);
AROS_LP1(void, glTexCoord1d,
         AROS_LPA(GLdouble, s, D0),
         LIBBASETYPEPTR, MesaBase, 187, Mesa
);
AROS_LP1(void, glTexCoord1f,
         AROS_LPA(GLfloat, s, D0),
         LIBBASETYPEPTR, MesaBase, 188, Mesa
);
AROS_LP1(void, glTexCoord1i,
         AROS_LPA(GLint, s, D0),
         LIBBASETYPEPTR, MesaBase, 189, Mesa
);
AROS_LP1(void, glTexCoord1s,
         AROS_LPA(GLshort, s, D0),
         LIBBASETYPEPTR, MesaBase, 190, Mesa
);
AROS_LP2(void, glTexCoord2d,
         AROS_LPA(GLdouble, s, D0),
         AROS_LPA(GLdouble, t, D1),
         LIBBASETYPEPTR, MesaBase, 191, Mesa
);
AROS_LP2(void, glTexCoord2f,
         AROS_LPA(GLfloat, s, D0),
         AROS_LPA(GLfloat, t, D1),
         LIBBASETYPEPTR, MesaBase, 192, Mesa
);
AROS_LP2(void, glTexCoord2i,
         AROS_LPA(GLint, s, D0),
         AROS_LPA(GLint, t, D1),
         LIBBASETYPEPTR, MesaBase, 193, Mesa
);
AROS_LP2(void, glTexCoord2s,
         AROS_LPA(GLshort, s, D0),
         AROS_LPA(GLshort, t, D1),
         LIBBASETYPEPTR, MesaBase, 194, Mesa
);
AROS_LP3(void, glTexCoord3d,
         AROS_LPA(GLdouble, s, D0),
         AROS_LPA(GLdouble, t, D1),
         AROS_LPA(GLdouble, r, D2),
         LIBBASETYPEPTR, MesaBase, 195, Mesa
);
AROS_LP3(void, glTexCoord3f,
         AROS_LPA(GLfloat, s, D0),
         AROS_LPA(GLfloat, t, D1),
         AROS_LPA(GLfloat, r, D2),
         LIBBASETYPEPTR, MesaBase, 196, Mesa
);
AROS_LP3(void, glTexCoord3i,
         AROS_LPA(GLint, s, D0),
         AROS_LPA(GLint, t, D1),
         AROS_LPA(GLint, r, D2),
         LIBBASETYPEPTR, MesaBase, 197, Mesa
);
AROS_LP3(void, glTexCoord3s,
         AROS_LPA(GLshort, s, D0),
         AROS_LPA(GLshort, t, D1),
         AROS_LPA(GLshort, r, D2),
         LIBBASETYPEPTR, MesaBase, 198, Mesa
);
AROS_LP4(void, glTexCoord4d,
         AROS_LPA(GLdouble, s, D0),
         AROS_LPA(GLdouble, t, D1),
         AROS_LPA(GLdouble, r, D2),
         AROS_LPA(GLdouble, q, D3),
         LIBBASETYPEPTR, MesaBase, 199, Mesa
);
AROS_LP4(void, glTexCoord4f,
         AROS_LPA(GLfloat, s, D0),
         AROS_LPA(GLfloat, t, D1),
         AROS_LPA(GLfloat, r, D2),
         AROS_LPA(GLfloat, q, D3),
         LIBBASETYPEPTR, MesaBase, 200, Mesa
);
AROS_LP4(void, glTexCoord4i,
         AROS_LPA(GLint, s, D0),
         AROS_LPA(GLint, t, D1),
         AROS_LPA(GLint, r, D2),
         AROS_LPA(GLint, q, D3),
         LIBBASETYPEPTR, MesaBase, 201, Mesa
);
AROS_LP4(void, glTexCoord4s,
         AROS_LPA(GLshort, s, D0),
         AROS_LPA(GLshort, t, D1),
         AROS_LPA(GLshort, r, D2),
         AROS_LPA(GLshort, q, D3),
         LIBBASETYPEPTR, MesaBase, 202, Mesa
);
AROS_LP1(void, glTexCoord1dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 203, Mesa
);
AROS_LP1(void, glTexCoord1fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 204, Mesa
);
AROS_LP1(void, glTexCoord1iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 205, Mesa
);
AROS_LP1(void, glTexCoord1sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 206, Mesa
);
AROS_LP1(void, glTexCoord2dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 207, Mesa
);
AROS_LP1(void, glTexCoord2fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 208, Mesa
);
AROS_LP1(void, glTexCoord2iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 209, Mesa
);
AROS_LP1(void, glTexCoord2sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 210, Mesa
);
AROS_LP1(void, glTexCoord3dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 211, Mesa
);
AROS_LP1(void, glTexCoord3fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 212, Mesa
);
AROS_LP1(void, glTexCoord3iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 213, Mesa
);
AROS_LP1(void, glTexCoord3sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 214, Mesa
);
AROS_LP1(void, glTexCoord4dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 215, Mesa
);
AROS_LP1(void, glTexCoord4fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 216, Mesa
);
AROS_LP1(void, glTexCoord4iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 217, Mesa
);
AROS_LP1(void, glTexCoord4sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 218, Mesa
);
AROS_LP2(void, glRasterPos2d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         LIBBASETYPEPTR, MesaBase, 219, Mesa
);
AROS_LP2(void, glRasterPos2f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         LIBBASETYPEPTR, MesaBase, 220, Mesa
);
AROS_LP2(void, glRasterPos2i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         LIBBASETYPEPTR, MesaBase, 221, Mesa
);
AROS_LP2(void, glRasterPos2s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         LIBBASETYPEPTR, MesaBase, 222, Mesa
);
AROS_LP3(void, glRasterPos3d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         LIBBASETYPEPTR, MesaBase, 223, Mesa
);
AROS_LP3(void, glRasterPos3f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         LIBBASETYPEPTR, MesaBase, 224, Mesa
);
AROS_LP3(void, glRasterPos3i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLint, z, D2),
         LIBBASETYPEPTR, MesaBase, 225, Mesa
);
AROS_LP3(void, glRasterPos3s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         AROS_LPA(GLshort, z, D2),
         LIBBASETYPEPTR, MesaBase, 226, Mesa
);
AROS_LP4(void, glRasterPos4d,
         AROS_LPA(GLdouble, x, D0),
         AROS_LPA(GLdouble, y, D1),
         AROS_LPA(GLdouble, z, D2),
         AROS_LPA(GLdouble, w, D3),
         LIBBASETYPEPTR, MesaBase, 227, Mesa
);
AROS_LP4(void, glRasterPos4f,
         AROS_LPA(GLfloat, x, D0),
         AROS_LPA(GLfloat, y, D1),
         AROS_LPA(GLfloat, z, D2),
         AROS_LPA(GLfloat, w, D3),
         LIBBASETYPEPTR, MesaBase, 228, Mesa
);
AROS_LP4(void, glRasterPos4i,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLint, z, D2),
         AROS_LPA(GLint, w, D3),
         LIBBASETYPEPTR, MesaBase, 229, Mesa
);
AROS_LP4(void, glRasterPos4s,
         AROS_LPA(GLshort, x, D0),
         AROS_LPA(GLshort, y, D1),
         AROS_LPA(GLshort, z, D2),
         AROS_LPA(GLshort, w, D3),
         LIBBASETYPEPTR, MesaBase, 230, Mesa
);
AROS_LP1(void, glRasterPos2dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 231, Mesa
);
AROS_LP1(void, glRasterPos2fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 232, Mesa
);
AROS_LP1(void, glRasterPos2iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 233, Mesa
);
AROS_LP1(void, glRasterPos2sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 234, Mesa
);
AROS_LP1(void, glRasterPos3dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 235, Mesa
);
AROS_LP1(void, glRasterPos3fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 236, Mesa
);
AROS_LP1(void, glRasterPos3iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 237, Mesa
);
AROS_LP1(void, glRasterPos3sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 238, Mesa
);
AROS_LP1(void, glRasterPos4dv,
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 239, Mesa
);
AROS_LP1(void, glRasterPos4fv,
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 240, Mesa
);
AROS_LP1(void, glRasterPos4iv,
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 241, Mesa
);
AROS_LP1(void, glRasterPos4sv,
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 242, Mesa
);
AROS_LP4(void, glRectd,
         AROS_LPA(GLdouble, x1, D0),
         AROS_LPA(GLdouble, y1, D1),
         AROS_LPA(GLdouble, x2, D2),
         AROS_LPA(GLdouble, y2, D3),
         LIBBASETYPEPTR, MesaBase, 243, Mesa
);
AROS_LP4(void, glRectf,
         AROS_LPA(GLfloat, x1, D0),
         AROS_LPA(GLfloat, y1, D1),
         AROS_LPA(GLfloat, x2, D2),
         AROS_LPA(GLfloat, y2, D3),
         LIBBASETYPEPTR, MesaBase, 244, Mesa
);
AROS_LP4(void, glRecti,
         AROS_LPA(GLint, x1, D0),
         AROS_LPA(GLint, y1, D1),
         AROS_LPA(GLint, x2, D2),
         AROS_LPA(GLint, y2, D3),
         LIBBASETYPEPTR, MesaBase, 245, Mesa
);
AROS_LP4(void, glRects,
         AROS_LPA(GLshort, x1, D0),
         AROS_LPA(GLshort, y1, D1),
         AROS_LPA(GLshort, x2, D2),
         AROS_LPA(GLshort, y2, D3),
         LIBBASETYPEPTR, MesaBase, 246, Mesa
);
AROS_LP2(void, glRectdv,
         AROS_LPA(const GLdouble *, v1, A0),
         AROS_LPA(const GLdouble *, v2, A1),
         LIBBASETYPEPTR, MesaBase, 247, Mesa
);
AROS_LP2(void, glRectfv,
         AROS_LPA(const GLfloat *, v1, A0),
         AROS_LPA(const GLfloat *, v2, A1),
         LIBBASETYPEPTR, MesaBase, 248, Mesa
);
AROS_LP2(void, glRectiv,
         AROS_LPA(const GLint *, v1, A0),
         AROS_LPA(const GLint *, v2, A1),
         LIBBASETYPEPTR, MesaBase, 249, Mesa
);
AROS_LP2(void, glRectsv,
         AROS_LPA(const GLshort *, v1, A0),
         AROS_LPA(const GLshort *, v2, A1),
         LIBBASETYPEPTR, MesaBase, 250, Mesa
);
AROS_LP4(void, glVertexPointer,
         AROS_LPA(GLint, size, D0),
         AROS_LPA(GLenum, type, D1),
         AROS_LPA(GLsizei, stride, D2),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 251, Mesa
);
AROS_LP3(void, glNormalPointer,
         AROS_LPA(GLenum, type, D0),
         AROS_LPA(GLsizei, stride, D1),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 252, Mesa
);
AROS_LP4(void, glColorPointer,
         AROS_LPA(GLint, size, D0),
         AROS_LPA(GLenum, type, D1),
         AROS_LPA(GLsizei, stride, D2),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 253, Mesa
);
AROS_LP3(void, glIndexPointer,
         AROS_LPA(GLenum, type, D0),
         AROS_LPA(GLsizei, stride, D1),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 254, Mesa
);
AROS_LP4(void, glTexCoordPointer,
         AROS_LPA(GLint, size, D0),
         AROS_LPA(GLenum, type, D1),
         AROS_LPA(GLsizei, stride, D2),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 255, Mesa
);
AROS_LP2(void, glEdgeFlagPointer,
         AROS_LPA(GLsizei, stride, D0),
         AROS_LPA(const GLvoid *, ptr, A0),
         LIBBASETYPEPTR, MesaBase, 256, Mesa
);
AROS_LP2(void, glGetPointerv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLvoid * *, params, A0),
         LIBBASETYPEPTR, MesaBase, 257, Mesa
);
AROS_LP1(void, glArrayElement,
         AROS_LPA(GLint, i, D0),
         LIBBASETYPEPTR, MesaBase, 258, Mesa
);
AROS_LP3(void, glDrawArrays,
         AROS_LPA(GLenum, mode, D0),
         AROS_LPA(GLint, first, D1),
         AROS_LPA(GLsizei, count, D2),
         LIBBASETYPEPTR, MesaBase, 259, Mesa
);
AROS_LP4(void, glDrawElements,
         AROS_LPA(GLenum, mode, D0),
         AROS_LPA(GLsizei, count, D1),
         AROS_LPA(GLenum, type, D2),
         AROS_LPA(const GLvoid *, indices, A0),
         LIBBASETYPEPTR, MesaBase, 260, Mesa
);
AROS_LP3(void, glInterleavedArrays,
         AROS_LPA(GLenum, format, D0),
         AROS_LPA(GLsizei, stride, D1),
         AROS_LPA(const GLvoid *, pointer, A0),
         LIBBASETYPEPTR, MesaBase, 261, Mesa
);
AROS_LP1(void, glShadeModel,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 262, Mesa
);
AROS_LP3(void, glLightf,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, param, D2),
         LIBBASETYPEPTR, MesaBase, 263, Mesa
);
AROS_LP3(void, glLighti,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, param, D2),
         LIBBASETYPEPTR, MesaBase, 264, Mesa
);
AROS_LP3(void, glLightfv,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 265, Mesa
);
AROS_LP3(void, glLightiv,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 266, Mesa
);
AROS_LP3(void, glGetLightfv,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 267, Mesa
);
AROS_LP3(void, glGetLightiv,
         AROS_LPA(GLenum, light, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 268, Mesa
);
AROS_LP2(void, glLightModelf,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLfloat, param, D1),
         LIBBASETYPEPTR, MesaBase, 269, Mesa
);
AROS_LP2(void, glLightModeli,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLint, param, D1),
         LIBBASETYPEPTR, MesaBase, 270, Mesa
);
AROS_LP2(void, glLightModelfv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 271, Mesa
);
AROS_LP2(void, glLightModeliv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 272, Mesa
);
AROS_LP3(void, glMaterialf,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, param, D2),
         LIBBASETYPEPTR, MesaBase, 273, Mesa
);
AROS_LP3(void, glMateriali,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, param, D2),
         LIBBASETYPEPTR, MesaBase, 274, Mesa
);
AROS_LP3(void, glMaterialfv,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 275, Mesa
);
AROS_LP3(void, glMaterialiv,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 276, Mesa
);
AROS_LP3(void, glGetMaterialfv,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 277, Mesa
);
AROS_LP3(void, glGetMaterialiv,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 278, Mesa
);
AROS_LP2(void, glColorMaterial,
         AROS_LPA(GLenum, face, D0),
         AROS_LPA(GLenum, mode, D1),
         LIBBASETYPEPTR, MesaBase, 279, Mesa
);
AROS_LP2(void, glPixelZoom,
         AROS_LPA(GLfloat, xfactor, D0),
         AROS_LPA(GLfloat, yfactor, D1),
         LIBBASETYPEPTR, MesaBase, 280, Mesa
);
AROS_LP2(void, glPixelStoref,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLfloat, param, D1),
         LIBBASETYPEPTR, MesaBase, 281, Mesa
);
AROS_LP2(void, glPixelStorei,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLint, param, D1),
         LIBBASETYPEPTR, MesaBase, 282, Mesa
);
AROS_LP2(void, glPixelTransferf,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLfloat, param, D1),
         LIBBASETYPEPTR, MesaBase, 283, Mesa
);
AROS_LP2(void, glPixelTransferi,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLint, param, D1),
         LIBBASETYPEPTR, MesaBase, 284, Mesa
);
AROS_LP3(void, glPixelMapfv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLsizei, mapsize, D1),
         AROS_LPA(const GLfloat *, values, A0),
         LIBBASETYPEPTR, MesaBase, 285, Mesa
);
AROS_LP3(void, glPixelMapuiv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLsizei, mapsize, D1),
         AROS_LPA(const GLuint *, values, A0),
         LIBBASETYPEPTR, MesaBase, 286, Mesa
);
AROS_LP3(void, glPixelMapusv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLsizei, mapsize, D1),
         AROS_LPA(const GLushort *, values, A0),
         LIBBASETYPEPTR, MesaBase, 287, Mesa
);
AROS_LP2(void, glGetPixelMapfv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLfloat *, values, A0),
         LIBBASETYPEPTR, MesaBase, 288, Mesa
);
AROS_LP2(void, glGetPixelMapuiv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLuint *, values, A0),
         LIBBASETYPEPTR, MesaBase, 289, Mesa
);
AROS_LP2(void, glGetPixelMapusv,
         AROS_LPA(GLenum, map, D0),
         AROS_LPA(GLushort *, values, A0),
         LIBBASETYPEPTR, MesaBase, 290, Mesa
);
AROS_LP7(void, glBitmap,
         AROS_LPA(GLsizei, width, D0),
         AROS_LPA(GLsizei, height, D1),
         AROS_LPA(GLfloat, xorig, D2),
         AROS_LPA(GLfloat, yorig, D3),
         AROS_LPA(GLfloat, xmove, D4),
         AROS_LPA(GLfloat, ymove, D5),
         AROS_LPA(const GLubyte *, bitmap, A0),
         LIBBASETYPEPTR, MesaBase, 291, Mesa
);
AROS_LP7(void, glReadPixels,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         AROS_LPA(GLenum, format, D4),
         AROS_LPA(GLenum, type, D5),
         AROS_LPA(GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 292, Mesa
);
AROS_LP5(void, glDrawPixels,
         AROS_LPA(GLsizei, width, D0),
         AROS_LPA(GLsizei, height, D1),
         AROS_LPA(GLenum, format, D2),
         AROS_LPA(GLenum, type, D3),
         AROS_LPA(const GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 293, Mesa
);
AROS_LP5(void, glCopyPixels,
         AROS_LPA(GLint, x, D0),
         AROS_LPA(GLint, y, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         AROS_LPA(GLenum, type, D4),
         LIBBASETYPEPTR, MesaBase, 294, Mesa
);
AROS_LP3(void, glStencilFunc,
         AROS_LPA(GLenum, func, D0),
         AROS_LPA(GLint, ref, D1),
         AROS_LPA(GLuint, mask, D2),
         LIBBASETYPEPTR, MesaBase, 295, Mesa
);
AROS_LP1(void, glStencilMask,
         AROS_LPA(GLuint, mask, D0),
         LIBBASETYPEPTR, MesaBase, 296, Mesa
);
AROS_LP3(void, glStencilOp,
         AROS_LPA(GLenum, fail, D0),
         AROS_LPA(GLenum, zfail, D1),
         AROS_LPA(GLenum, zpass, D2),
         LIBBASETYPEPTR, MesaBase, 297, Mesa
);
AROS_LP1(void, glClearStencil,
         AROS_LPA(GLint, s, D0),
         LIBBASETYPEPTR, MesaBase, 298, Mesa
);
AROS_LP3(void, glTexGend,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLdouble, param, D2),
         LIBBASETYPEPTR, MesaBase, 299, Mesa
);
AROS_LP3(void, glTexGenf,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, param, D2),
         LIBBASETYPEPTR, MesaBase, 300, Mesa
);
AROS_LP3(void, glTexGeni,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, param, D2),
         LIBBASETYPEPTR, MesaBase, 301, Mesa
);
AROS_LP3(void, glTexGendv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLdouble *, params, A0),
         LIBBASETYPEPTR, MesaBase, 302, Mesa
);
AROS_LP3(void, glTexGenfv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 303, Mesa
);
AROS_LP3(void, glTexGeniv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 304, Mesa
);
AROS_LP3(void, glGetTexGendv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLdouble *, params, A0),
         LIBBASETYPEPTR, MesaBase, 305, Mesa
);
AROS_LP3(void, glGetTexGenfv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 306, Mesa
);
AROS_LP3(void, glGetTexGeniv,
         AROS_LPA(GLenum, coord, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 307, Mesa
);
AROS_LP3(void, glTexEnvf,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, param, D2),
         LIBBASETYPEPTR, MesaBase, 308, Mesa
);
AROS_LP3(void, glTexEnvi,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, param, D2),
         LIBBASETYPEPTR, MesaBase, 309, Mesa
);
AROS_LP3(void, glTexEnvfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 310, Mesa
);
AROS_LP3(void, glTexEnviv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 311, Mesa
);
AROS_LP3(void, glGetTexEnvfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 312, Mesa
);
AROS_LP3(void, glGetTexEnviv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 313, Mesa
);
AROS_LP3(void, glTexParameterf,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, param, D2),
         LIBBASETYPEPTR, MesaBase, 314, Mesa
);
AROS_LP3(void, glTexParameteri,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, param, D2),
         LIBBASETYPEPTR, MesaBase, 315, Mesa
);
AROS_LP3(void, glTexParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 316, Mesa
);
AROS_LP3(void, glTexParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 317, Mesa
);
AROS_LP3(void, glGetTexParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 318, Mesa
);
AROS_LP3(void, glGetTexParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 319, Mesa
);
AROS_LP4(void, glGetTexLevelParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, pname, D2),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 320, Mesa
);
AROS_LP4(void, glGetTexLevelParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, pname, D2),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 321, Mesa
);
AROS_LP8(void, glTexImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, internalFormat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLint, border, D4),
         AROS_LPA(GLenum, format, D5),
         AROS_LPA(GLenum, type, D6),
         AROS_LPA(const GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 322, Mesa
);
AROS_LP9(void, glTexImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, internalFormat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLsizei, height, D4),
         AROS_LPA(GLint, border, D5),
         AROS_LPA(GLenum, format, D6),
         AROS_LPA(GLenum, type, D7),
         AROS_LPA(const GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 323, Mesa
);
AROS_LP5(void, glGetTexImage,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, format, D2),
         AROS_LPA(GLenum, type, D3),
         AROS_LPA(GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 324, Mesa
);
AROS_LP2(void, glGenTextures,
         AROS_LPA(GLsizei, n, D0),
         AROS_LPA(GLuint *, textures, A0),
         LIBBASETYPEPTR, MesaBase, 325, Mesa
);
AROS_LP2(void, glDeleteTextures,
         AROS_LPA(GLsizei, n, D0),
         AROS_LPA(const GLuint *, textures, A0),
         LIBBASETYPEPTR, MesaBase, 326, Mesa
);
AROS_LP2(void, glBindTexture,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLuint, texture, D1),
         LIBBASETYPEPTR, MesaBase, 327, Mesa
);
AROS_LP3(void, glPrioritizeTextures,
         AROS_LPA(GLsizei, n, D0),
         AROS_LPA(const GLuint *, textures, A0),
         AROS_LPA(const GLclampf *, priorities, A1),
         LIBBASETYPEPTR, MesaBase, 328, Mesa
);
AROS_LP3(GLboolean, glAreTexturesResident,
         AROS_LPA(GLsizei, n, D0),
         AROS_LPA(const GLuint *, textures, A0),
         AROS_LPA(GLboolean *, residences, A1),
         LIBBASETYPEPTR, MesaBase, 329, Mesa
);
AROS_LP1(GLboolean, glIsTexture,
         AROS_LPA(GLuint, texture, D0),
         LIBBASETYPEPTR, MesaBase, 330, Mesa
);
AROS_LP7(void, glTexSubImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLenum, format, D4),
         AROS_LPA(GLenum, type, D5),
         AROS_LPA(const GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 331, Mesa
);
AROS_LP9(void, glTexSubImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLsizei, width, D4),
         AROS_LPA(GLsizei, height, D5),
         AROS_LPA(GLenum, format, D6),
         AROS_LPA(GLenum, type, D7),
         AROS_LPA(const GLvoid *, pixels, A0),
         LIBBASETYPEPTR, MesaBase, 332, Mesa
);
AROS_LP7(void, glCopyTexImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLint, x, D3),
         AROS_LPA(GLint, y, D4),
         AROS_LPA(GLsizei, width, D5),
         AROS_LPA(GLint, border, D6),
         LIBBASETYPEPTR, MesaBase, 333, Mesa
);
AROS_LP8(void, glCopyTexImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLint, x, D3),
         AROS_LPA(GLint, y, D4),
         AROS_LPA(GLsizei, width, D5),
         AROS_LPA(GLsizei, height, D6),
         AROS_LPA(GLint, border, D7),
         LIBBASETYPEPTR, MesaBase, 334, Mesa
);
AROS_LP6(void, glCopyTexSubImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, x, D3),
         AROS_LPA(GLint, y, D4),
         AROS_LPA(GLsizei, width, D5),
         LIBBASETYPEPTR, MesaBase, 335, Mesa
);
AROS_LP8(void, glCopyTexSubImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLint, x, D4),
         AROS_LPA(GLint, y, D5),
         AROS_LPA(GLsizei, width, D6),
         AROS_LPA(GLsizei, height, D7),
         LIBBASETYPEPTR, MesaBase, 336, Mesa
);
AROS_LP6(void, glMap1d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, u1, D1),
         AROS_LPA(GLdouble, u2, D2),
         AROS_LPA(GLint, stride, D3),
         AROS_LPA(GLint, order, D4),
         AROS_LPA(const GLdouble *, points, A0),
         LIBBASETYPEPTR, MesaBase, 337, Mesa
);
AROS_LP6(void, glMap1f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, u1, D1),
         AROS_LPA(GLfloat, u2, D2),
         AROS_LPA(GLint, stride, D3),
         AROS_LPA(GLint, order, D4),
         AROS_LPA(const GLfloat *, points, A0),
         LIBBASETYPEPTR, MesaBase, 338, Mesa
);
AROS_LP10(void, glMap2d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, u1, D1),
         AROS_LPA(GLdouble, u2, D2),
         AROS_LPA(GLint, ustride, D3),
         AROS_LPA(GLint, uorder, D4),
         AROS_LPA(GLdouble, v1, D5),
         AROS_LPA(GLdouble, v2, D6),
         AROS_LPA(GLint, vstride, D7),
         AROS_LPA(GLint, vorder, A0),
         AROS_LPA(const GLdouble *, points, A1),
         LIBBASETYPEPTR, MesaBase, 339, Mesa
);
AROS_LP10(void, glMap2f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, u1, D1),
         AROS_LPA(GLfloat, u2, D2),
         AROS_LPA(GLint, ustride, D3),
         AROS_LPA(GLint, uorder, D4),
         AROS_LPA(GLfloat, v1, D5),
         AROS_LPA(GLfloat, v2, D6),
         AROS_LPA(GLint, vstride, D7),
         AROS_LPA(GLint, vorder, A0),
         AROS_LPA(const GLfloat *, points, A1),
         LIBBASETYPEPTR, MesaBase, 340, Mesa
);
AROS_LP3(void, glGetMapdv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, query, D1),
         AROS_LPA(GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 341, Mesa
);
AROS_LP3(void, glGetMapfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, query, D1),
         AROS_LPA(GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 342, Mesa
);
AROS_LP3(void, glGetMapiv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, query, D1),
         AROS_LPA(GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 343, Mesa
);
AROS_LP1(void, glEvalCoord1d,
         AROS_LPA(GLdouble, u, D0),
         LIBBASETYPEPTR, MesaBase, 344, Mesa
);
AROS_LP1(void, glEvalCoord1f,
         AROS_LPA(GLfloat, u, D0),
         LIBBASETYPEPTR, MesaBase, 345, Mesa
);
AROS_LP1(void, glEvalCoord1dv,
         AROS_LPA(const GLdouble *, u, A0),
         LIBBASETYPEPTR, MesaBase, 346, Mesa
);
AROS_LP1(void, glEvalCoord1fv,
         AROS_LPA(const GLfloat *, u, A0),
         LIBBASETYPEPTR, MesaBase, 347, Mesa
);
AROS_LP2(void, glEvalCoord2d,
         AROS_LPA(GLdouble, u, D0),
         AROS_LPA(GLdouble, v, D1),
         LIBBASETYPEPTR, MesaBase, 348, Mesa
);
AROS_LP2(void, glEvalCoord2f,
         AROS_LPA(GLfloat, u, D0),
         AROS_LPA(GLfloat, v, D1),
         LIBBASETYPEPTR, MesaBase, 349, Mesa
);
AROS_LP1(void, glEvalCoord2dv,
         AROS_LPA(const GLdouble *, u, A0),
         LIBBASETYPEPTR, MesaBase, 350, Mesa
);
AROS_LP1(void, glEvalCoord2fv,
         AROS_LPA(const GLfloat *, u, A0),
         LIBBASETYPEPTR, MesaBase, 351, Mesa
);
AROS_LP3(void, glMapGrid1d,
         AROS_LPA(GLint, un, D0),
         AROS_LPA(GLdouble, u1, D1),
         AROS_LPA(GLdouble, u2, D2),
         LIBBASETYPEPTR, MesaBase, 352, Mesa
);
AROS_LP3(void, glMapGrid1f,
         AROS_LPA(GLint, un, D0),
         AROS_LPA(GLfloat, u1, D1),
         AROS_LPA(GLfloat, u2, D2),
         LIBBASETYPEPTR, MesaBase, 353, Mesa
);
AROS_LP6(void, glMapGrid2d,
         AROS_LPA(GLint, un, D0),
         AROS_LPA(GLdouble, u1, D1),
         AROS_LPA(GLdouble, u2, D2),
         AROS_LPA(GLint, vn, D3),
         AROS_LPA(GLdouble, v1, D4),
         AROS_LPA(GLdouble, v2, D5),
         LIBBASETYPEPTR, MesaBase, 354, Mesa
);
AROS_LP6(void, glMapGrid2f,
         AROS_LPA(GLint, un, D0),
         AROS_LPA(GLfloat, u1, D1),
         AROS_LPA(GLfloat, u2, D2),
         AROS_LPA(GLint, vn, D3),
         AROS_LPA(GLfloat, v1, D4),
         AROS_LPA(GLfloat, v2, D5),
         LIBBASETYPEPTR, MesaBase, 355, Mesa
);
AROS_LP1(void, glEvalPoint1,
         AROS_LPA(GLint, i, D0),
         LIBBASETYPEPTR, MesaBase, 356, Mesa
);
AROS_LP2(void, glEvalPoint2,
         AROS_LPA(GLint, i, D0),
         AROS_LPA(GLint, j, D1),
         LIBBASETYPEPTR, MesaBase, 357, Mesa
);
AROS_LP3(void, glEvalMesh1,
         AROS_LPA(GLenum, mode, D0),
         AROS_LPA(GLint, i1, D1),
         AROS_LPA(GLint, i2, D2),
         LIBBASETYPEPTR, MesaBase, 358, Mesa
);
AROS_LP5(void, glEvalMesh2,
         AROS_LPA(GLenum, mode, D0),
         AROS_LPA(GLint, i1, D1),
         AROS_LPA(GLint, i2, D2),
         AROS_LPA(GLint, j1, D3),
         AROS_LPA(GLint, j2, D4),
         LIBBASETYPEPTR, MesaBase, 359, Mesa
);
AROS_LP2(void, glFogf,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLfloat, param, D1),
         LIBBASETYPEPTR, MesaBase, 360, Mesa
);
AROS_LP2(void, glFogi,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(GLint, param, D1),
         LIBBASETYPEPTR, MesaBase, 361, Mesa
);
AROS_LP2(void, glFogfv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 362, Mesa
);
AROS_LP2(void, glFogiv,
         AROS_LPA(GLenum, pname, D0),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 363, Mesa
);
AROS_LP3(void, glFeedbackBuffer,
         AROS_LPA(GLsizei, size, D0),
         AROS_LPA(GLenum, type, D1),
         AROS_LPA(GLfloat *, buffer, A0),
         LIBBASETYPEPTR, MesaBase, 364, Mesa
);
AROS_LP1(void, glPassThrough,
         AROS_LPA(GLfloat, token, D0),
         LIBBASETYPEPTR, MesaBase, 365, Mesa
);
AROS_LP2(void, glSelectBuffer,
         AROS_LPA(GLsizei, size, D0),
         AROS_LPA(GLuint *, buffer, A0),
         LIBBASETYPEPTR, MesaBase, 366, Mesa
);
AROS_LP0(void, glInitNames,
         LIBBASETYPEPTR, MesaBase, 367, Mesa
);
AROS_LP1(void, glLoadName,
         AROS_LPA(GLuint, name, D0),
         LIBBASETYPEPTR, MesaBase, 368, Mesa
);
AROS_LP1(void, glPushName,
         AROS_LPA(GLuint, name, D0),
         LIBBASETYPEPTR, MesaBase, 369, Mesa
);
AROS_LP0(void, glPopName,
         LIBBASETYPEPTR, MesaBase, 370, Mesa
);
AROS_LP6(void, glDrawRangeElements,
         AROS_LPA(GLenum, mode, D0),
         AROS_LPA(GLuint, start, D1),
         AROS_LPA(GLuint, end, D2),
         AROS_LPA(GLsizei, count, D3),
         AROS_LPA(GLenum, type, D4),
         AROS_LPA(const GLvoid *, indices, A0),
         LIBBASETYPEPTR, MesaBase, 371, Mesa
);
AROS_LP10(void, glTexImage3D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, internalFormat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLsizei, height, D4),
         AROS_LPA(GLsizei, depth, D5),
         AROS_LPA(GLint, border, D6),
         AROS_LPA(GLenum, format, D7),
         AROS_LPA(GLenum, type, A0),
         AROS_LPA(const GLvoid *, pixels, A1),
         LIBBASETYPEPTR, MesaBase, 372, Mesa
);
AROS_LP11(void, glTexSubImage3D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLint, zoffset, D4),
         AROS_LPA(GLsizei, width, D5),
         AROS_LPA(GLsizei, height, D6),
         AROS_LPA(GLsizei, depth, D7),
         AROS_LPA(GLenum, format, A0),
         AROS_LPA(GLenum, type, A1),
         AROS_LPA(const GLvoid *, pixels, A2),
         LIBBASETYPEPTR, MesaBase, 373, Mesa
);
AROS_LP9(void, glCopyTexSubImage3D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLint, zoffset, D4),
         AROS_LPA(GLint, x, D5),
         AROS_LPA(GLint, y, D6),
         AROS_LPA(GLsizei, width, D7),
         AROS_LPA(GLsizei, height, A0),
         LIBBASETYPEPTR, MesaBase, 374, Mesa
);
AROS_LP6(void, glColorTable,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLenum, format, D3),
         AROS_LPA(GLenum, type, D4),
         AROS_LPA(const GLvoid *, table, A0),
         LIBBASETYPEPTR, MesaBase, 375, Mesa
);
AROS_LP6(void, glColorSubTable,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLsizei, start, D1),
         AROS_LPA(GLsizei, count, D2),
         AROS_LPA(GLenum, format, D3),
         AROS_LPA(GLenum, type, D4),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 376, Mesa
);
AROS_LP3(void, glColorTableParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 377, Mesa
);
AROS_LP3(void, glColorTableParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 378, Mesa
);
AROS_LP5(void, glCopyColorSubTable,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLsizei, start, D1),
         AROS_LPA(GLint, x, D2),
         AROS_LPA(GLint, y, D3),
         AROS_LPA(GLsizei, width, D4),
         LIBBASETYPEPTR, MesaBase, 379, Mesa
);
AROS_LP5(void, glCopyColorTable,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLint, x, D2),
         AROS_LPA(GLint, y, D3),
         AROS_LPA(GLsizei, width, D4),
         LIBBASETYPEPTR, MesaBase, 380, Mesa
);
AROS_LP4(void, glGetColorTable,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, format, D1),
         AROS_LPA(GLenum, type, D2),
         AROS_LPA(GLvoid *, table, A0),
         LIBBASETYPEPTR, MesaBase, 381, Mesa
);
AROS_LP3(void, glGetColorTableParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 382, Mesa
);
AROS_LP3(void, glGetColorTableParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 383, Mesa
);
AROS_LP1(void, glBlendEquation,
         AROS_LPA(GLenum, mode, D0),
         LIBBASETYPEPTR, MesaBase, 384, Mesa
);
AROS_LP4(void, glBlendColor,
         AROS_LPA(GLclampf, red, D0),
         AROS_LPA(GLclampf, green, D1),
         AROS_LPA(GLclampf, blue, D2),
         AROS_LPA(GLclampf, alpha, D3),
         LIBBASETYPEPTR, MesaBase, 385, Mesa
);
AROS_LP4(void, glHistogram,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLsizei, width, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLboolean, sink, D3),
         LIBBASETYPEPTR, MesaBase, 386, Mesa
);
AROS_LP1(void, glResetHistogram,
         AROS_LPA(GLenum, target, D0),
         LIBBASETYPEPTR, MesaBase, 387, Mesa
);
AROS_LP5(void, glGetHistogram,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLboolean, reset, D1),
         AROS_LPA(GLenum, format, D2),
         AROS_LPA(GLenum, type, D3),
         AROS_LPA(GLvoid *, values, A0),
         LIBBASETYPEPTR, MesaBase, 388, Mesa
);
AROS_LP3(void, glGetHistogramParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 389, Mesa
);
AROS_LP3(void, glGetHistogramParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 390, Mesa
);
AROS_LP3(void, glMinmax,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLboolean, sink, D2),
         LIBBASETYPEPTR, MesaBase, 391, Mesa
);
AROS_LP1(void, glResetMinmax,
         AROS_LPA(GLenum, target, D0),
         LIBBASETYPEPTR, MesaBase, 392, Mesa
);
AROS_LP5(void, glGetMinmax,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLboolean, reset, D1),
         AROS_LPA(GLenum, format, D2),
         AROS_LPA(GLenum, types, D3),
         AROS_LPA(GLvoid *, values, A0),
         LIBBASETYPEPTR, MesaBase, 393, Mesa
);
AROS_LP3(void, glGetMinmaxParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 394, Mesa
);
AROS_LP3(void, glGetMinmaxParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 395, Mesa
);
AROS_LP6(void, glConvolutionFilter1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLenum, format, D3),
         AROS_LPA(GLenum, type, D4),
         AROS_LPA(const GLvoid *, image, A0),
         LIBBASETYPEPTR, MesaBase, 396, Mesa
);
AROS_LP7(void, glConvolutionFilter2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         AROS_LPA(GLenum, format, D4),
         AROS_LPA(GLenum, type, D5),
         AROS_LPA(const GLvoid *, image, A0),
         LIBBASETYPEPTR, MesaBase, 397, Mesa
);
AROS_LP3(void, glConvolutionParameterf,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat, params, D2),
         LIBBASETYPEPTR, MesaBase, 398, Mesa
);
AROS_LP3(void, glConvolutionParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 399, Mesa
);
AROS_LP3(void, glConvolutionParameteri,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint, params, D2),
         LIBBASETYPEPTR, MesaBase, 400, Mesa
);
AROS_LP3(void, glConvolutionParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(const GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 401, Mesa
);
AROS_LP5(void, glCopyConvolutionFilter1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLint, x, D2),
         AROS_LPA(GLint, y, D3),
         AROS_LPA(GLsizei, width, D4),
         LIBBASETYPEPTR, MesaBase, 402, Mesa
);
AROS_LP6(void, glCopyConvolutionFilter2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLint, x, D2),
         AROS_LPA(GLint, y, D3),
         AROS_LPA(GLsizei, width, D4),
         AROS_LPA(GLsizei, height, D5),
         LIBBASETYPEPTR, MesaBase, 403, Mesa
);
AROS_LP4(void, glGetConvolutionFilter,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, format, D1),
         AROS_LPA(GLenum, type, D2),
         AROS_LPA(GLvoid *, image, A0),
         LIBBASETYPEPTR, MesaBase, 404, Mesa
);
AROS_LP3(void, glGetConvolutionParameterfv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLfloat *, params, A0),
         LIBBASETYPEPTR, MesaBase, 405, Mesa
);
AROS_LP3(void, glGetConvolutionParameteriv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, pname, D1),
         AROS_LPA(GLint *, params, A0),
         LIBBASETYPEPTR, MesaBase, 406, Mesa
);
AROS_LP8(void, glSeparableFilter2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, internalformat, D1),
         AROS_LPA(GLsizei, width, D2),
         AROS_LPA(GLsizei, height, D3),
         AROS_LPA(GLenum, format, D4),
         AROS_LPA(GLenum, type, D5),
         AROS_LPA(const GLvoid *, row, A0),
         AROS_LPA(const GLvoid *, column, A1),
         LIBBASETYPEPTR, MesaBase, 407, Mesa
);
AROS_LP6(void, glGetSeparableFilter,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLenum, format, D1),
         AROS_LPA(GLenum, type, D2),
         AROS_LPA(GLvoid *, row, A0),
         AROS_LPA(GLvoid *, column, A1),
         AROS_LPA(GLvoid *, span, A2),
         LIBBASETYPEPTR, MesaBase, 408, Mesa
);
AROS_LP1(void, glActiveTexture,
         AROS_LPA(GLenum, texture, D0),
         LIBBASETYPEPTR, MesaBase, 409, Mesa
);
AROS_LP1(void, glClientActiveTexture,
         AROS_LPA(GLenum, texture, D0),
         LIBBASETYPEPTR, MesaBase, 410, Mesa
);
AROS_LP7(void, glCompressedTexImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLint, border, D4),
         AROS_LPA(GLsizei, imageSize, D5),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 411, Mesa
);
AROS_LP8(void, glCompressedTexImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLsizei, height, D4),
         AROS_LPA(GLint, border, D5),
         AROS_LPA(GLsizei, imageSize, D6),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 412, Mesa
);
AROS_LP9(void, glCompressedTexImage3D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLenum, internalformat, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLsizei, height, D4),
         AROS_LPA(GLsizei, depth, D5),
         AROS_LPA(GLint, border, D6),
         AROS_LPA(GLsizei, imageSize, D7),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 413, Mesa
);
AROS_LP7(void, glCompressedTexSubImage1D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLsizei, width, D3),
         AROS_LPA(GLenum, format, D4),
         AROS_LPA(GLsizei, imageSize, D5),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 414, Mesa
);
AROS_LP9(void, glCompressedTexSubImage2D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLsizei, width, D4),
         AROS_LPA(GLsizei, height, D5),
         AROS_LPA(GLenum, format, D6),
         AROS_LPA(GLsizei, imageSize, D7),
         AROS_LPA(const GLvoid *, data, A0),
         LIBBASETYPEPTR, MesaBase, 415, Mesa
);
AROS_LP11(void, glCompressedTexSubImage3D,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, level, D1),
         AROS_LPA(GLint, xoffset, D2),
         AROS_LPA(GLint, yoffset, D3),
         AROS_LPA(GLint, zoffset, D4),
         AROS_LPA(GLsizei, width, D5),
         AROS_LPA(GLsizei, height, D6),
         AROS_LPA(GLsizei, depth, D7),
         AROS_LPA(GLenum, format, A0),
         AROS_LPA(GLsizei, imageSize, A1),
         AROS_LPA(const GLvoid *, data, A2),
         LIBBASETYPEPTR, MesaBase, 416, Mesa
);
AROS_LP3(void, glGetCompressedTexImage,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, lod, D1),
         AROS_LPA(GLvoid *, img, A0),
         LIBBASETYPEPTR, MesaBase, 417, Mesa
);
AROS_LP2(void, glMultiTexCoord1d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         LIBBASETYPEPTR, MesaBase, 418, Mesa
);
AROS_LP2(void, glMultiTexCoord1dv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 419, Mesa
);
AROS_LP2(void, glMultiTexCoord1f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         LIBBASETYPEPTR, MesaBase, 420, Mesa
);
AROS_LP2(void, glMultiTexCoord1fv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 421, Mesa
);
AROS_LP2(void, glMultiTexCoord1i,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         LIBBASETYPEPTR, MesaBase, 422, Mesa
);
AROS_LP2(void, glMultiTexCoord1iv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 423, Mesa
);
AROS_LP2(void, glMultiTexCoord1s,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         LIBBASETYPEPTR, MesaBase, 424, Mesa
);
AROS_LP2(void, glMultiTexCoord1sv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 425, Mesa
);
AROS_LP3(void, glMultiTexCoord2d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         LIBBASETYPEPTR, MesaBase, 426, Mesa
);
AROS_LP2(void, glMultiTexCoord2dv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 427, Mesa
);
AROS_LP3(void, glMultiTexCoord2f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         LIBBASETYPEPTR, MesaBase, 428, Mesa
);
AROS_LP2(void, glMultiTexCoord2fv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 429, Mesa
);
AROS_LP3(void, glMultiTexCoord2i,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         LIBBASETYPEPTR, MesaBase, 430, Mesa
);
AROS_LP2(void, glMultiTexCoord2iv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 431, Mesa
);
AROS_LP3(void, glMultiTexCoord2s,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         LIBBASETYPEPTR, MesaBase, 432, Mesa
);
AROS_LP2(void, glMultiTexCoord2sv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 433, Mesa
);
AROS_LP4(void, glMultiTexCoord3d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         AROS_LPA(GLdouble, r, D3),
         LIBBASETYPEPTR, MesaBase, 434, Mesa
);
AROS_LP2(void, glMultiTexCoord3dv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 435, Mesa
);
AROS_LP4(void, glMultiTexCoord3f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         AROS_LPA(GLfloat, r, D3),
         LIBBASETYPEPTR, MesaBase, 436, Mesa
);
AROS_LP2(void, glMultiTexCoord3fv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 437, Mesa
);
AROS_LP4(void, glMultiTexCoord3i,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         AROS_LPA(GLint, r, D3),
         LIBBASETYPEPTR, MesaBase, 438, Mesa
);
AROS_LP2(void, glMultiTexCoord3iv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 439, Mesa
);
AROS_LP4(void, glMultiTexCoord3s,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         AROS_LPA(GLshort, r, D3),
         LIBBASETYPEPTR, MesaBase, 440, Mesa
);
AROS_LP2(void, glMultiTexCoord3sv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 441, Mesa
);
AROS_LP5(void, glMultiTexCoord4d,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         AROS_LPA(GLdouble, r, D3),
         AROS_LPA(GLdouble, q, D4),
         LIBBASETYPEPTR, MesaBase, 442, Mesa
);
AROS_LP2(void, glMultiTexCoord4dv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 443, Mesa
);
AROS_LP5(void, glMultiTexCoord4f,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         AROS_LPA(GLfloat, r, D3),
         AROS_LPA(GLfloat, q, D4),
         LIBBASETYPEPTR, MesaBase, 444, Mesa
);
AROS_LP2(void, glMultiTexCoord4fv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 445, Mesa
);
AROS_LP5(void, glMultiTexCoord4i,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         AROS_LPA(GLint, r, D3),
         AROS_LPA(GLint, q, D4),
         LIBBASETYPEPTR, MesaBase, 446, Mesa
);
AROS_LP2(void, glMultiTexCoord4iv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 447, Mesa
);
AROS_LP5(void, glMultiTexCoord4s,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         AROS_LPA(GLshort, r, D3),
         AROS_LPA(GLshort, q, D4),
         LIBBASETYPEPTR, MesaBase, 448, Mesa
);
AROS_LP2(void, glMultiTexCoord4sv,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 449, Mesa
);
AROS_LP1(void, glLoadTransposeMatrixd,
         AROS_LPA(const GLdouble *, m, A0),
         LIBBASETYPEPTR, MesaBase, 450, Mesa
);
AROS_LP1(void, glLoadTransposeMatrixf,
         AROS_LPA(const GLfloat *, m, A0),
         LIBBASETYPEPTR, MesaBase, 451, Mesa
);
AROS_LP1(void, glMultTransposeMatrixd,
         AROS_LPA(const GLdouble *, m, A0),
         LIBBASETYPEPTR, MesaBase, 452, Mesa
);
AROS_LP1(void, glMultTransposeMatrixf,
         AROS_LPA(const GLfloat *, m, A0),
         LIBBASETYPEPTR, MesaBase, 453, Mesa
);
AROS_LP2(void, glSampleCoverage,
         AROS_LPA(GLclampf, value, D0),
         AROS_LPA(GLboolean, invert, D1),
         LIBBASETYPEPTR, MesaBase, 454, Mesa
);
AROS_LP1(void, glActiveTextureARB,
         AROS_LPA(GLenum, texture, D0),
         LIBBASETYPEPTR, MesaBase, 455, Mesa
);
AROS_LP1(void, glClientActiveTextureARB,
         AROS_LPA(GLenum, texture, D0),
         LIBBASETYPEPTR, MesaBase, 456, Mesa
);
AROS_LP2(void, glMultiTexCoord1dARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         LIBBASETYPEPTR, MesaBase, 457, Mesa
);
AROS_LP2(void, glMultiTexCoord1dvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 458, Mesa
);
AROS_LP2(void, glMultiTexCoord1fARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         LIBBASETYPEPTR, MesaBase, 459, Mesa
);
AROS_LP2(void, glMultiTexCoord1fvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 460, Mesa
);
AROS_LP2(void, glMultiTexCoord1iARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         LIBBASETYPEPTR, MesaBase, 461, Mesa
);
AROS_LP2(void, glMultiTexCoord1ivARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 462, Mesa
);
AROS_LP2(void, glMultiTexCoord1sARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         LIBBASETYPEPTR, MesaBase, 463, Mesa
);
AROS_LP2(void, glMultiTexCoord1svARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 464, Mesa
);
AROS_LP3(void, glMultiTexCoord2dARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         LIBBASETYPEPTR, MesaBase, 465, Mesa
);
AROS_LP2(void, glMultiTexCoord2dvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 466, Mesa
);
AROS_LP3(void, glMultiTexCoord2fARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         LIBBASETYPEPTR, MesaBase, 467, Mesa
);
AROS_LP2(void, glMultiTexCoord2fvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 468, Mesa
);
AROS_LP3(void, glMultiTexCoord2iARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         LIBBASETYPEPTR, MesaBase, 469, Mesa
);
AROS_LP2(void, glMultiTexCoord2ivARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 470, Mesa
);
AROS_LP3(void, glMultiTexCoord2sARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         LIBBASETYPEPTR, MesaBase, 471, Mesa
);
AROS_LP2(void, glMultiTexCoord2svARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 472, Mesa
);
AROS_LP4(void, glMultiTexCoord3dARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         AROS_LPA(GLdouble, r, D3),
         LIBBASETYPEPTR, MesaBase, 473, Mesa
);
AROS_LP2(void, glMultiTexCoord3dvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 474, Mesa
);
AROS_LP4(void, glMultiTexCoord3fARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         AROS_LPA(GLfloat, r, D3),
         LIBBASETYPEPTR, MesaBase, 475, Mesa
);
AROS_LP2(void, glMultiTexCoord3fvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 476, Mesa
);
AROS_LP4(void, glMultiTexCoord3iARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         AROS_LPA(GLint, r, D3),
         LIBBASETYPEPTR, MesaBase, 477, Mesa
);
AROS_LP2(void, glMultiTexCoord3ivARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 478, Mesa
);
AROS_LP4(void, glMultiTexCoord3sARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         AROS_LPA(GLshort, r, D3),
         LIBBASETYPEPTR, MesaBase, 479, Mesa
);
AROS_LP2(void, glMultiTexCoord3svARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 480, Mesa
);
AROS_LP5(void, glMultiTexCoord4dARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLdouble, s, D1),
         AROS_LPA(GLdouble, t, D2),
         AROS_LPA(GLdouble, r, D3),
         AROS_LPA(GLdouble, q, D4),
         LIBBASETYPEPTR, MesaBase, 481, Mesa
);
AROS_LP2(void, glMultiTexCoord4dvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLdouble *, v, A0),
         LIBBASETYPEPTR, MesaBase, 482, Mesa
);
AROS_LP5(void, glMultiTexCoord4fARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLfloat, s, D1),
         AROS_LPA(GLfloat, t, D2),
         AROS_LPA(GLfloat, r, D3),
         AROS_LPA(GLfloat, q, D4),
         LIBBASETYPEPTR, MesaBase, 483, Mesa
);
AROS_LP2(void, glMultiTexCoord4fvARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLfloat *, v, A0),
         LIBBASETYPEPTR, MesaBase, 484, Mesa
);
AROS_LP5(void, glMultiTexCoord4iARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLint, s, D1),
         AROS_LPA(GLint, t, D2),
         AROS_LPA(GLint, r, D3),
         AROS_LPA(GLint, q, D4),
         LIBBASETYPEPTR, MesaBase, 485, Mesa
);
AROS_LP2(void, glMultiTexCoord4ivARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLint *, v, A0),
         LIBBASETYPEPTR, MesaBase, 486, Mesa
);
AROS_LP5(void, glMultiTexCoord4sARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(GLshort, s, D1),
         AROS_LPA(GLshort, t, D2),
         AROS_LPA(GLshort, r, D3),
         AROS_LPA(GLshort, q, D4),
         LIBBASETYPEPTR, MesaBase, 487, Mesa
);
AROS_LP2(void, glMultiTexCoord4svARB,
         AROS_LPA(GLenum, target, D0),
         AROS_LPA(const GLshort *, v, A0),
         LIBBASETYPEPTR, MesaBase, 488, Mesa
);
AROS_LP4(void, glBlendFuncSeparate,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         LIBBASETYPEPTR, MesaBase, 489, Mesa
);
AROS_LP1(void, glFogCoordf,
         AROS_LPA(GLfloat, a, D0),
         LIBBASETYPEPTR, MesaBase, 490, Mesa
);
AROS_LP1(void, glFogCoordfv,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 491, Mesa
);
AROS_LP1(void, glFogCoordd,
         AROS_LPA(GLdouble, a, D0),
         LIBBASETYPEPTR, MesaBase, 492, Mesa
);
AROS_LP1(void, glFogCoorddv,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 493, Mesa
);
AROS_LP3(void, glFogCoordPointer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLvoid *, c, A0),
         LIBBASETYPEPTR, MesaBase, 494, Mesa
);
AROS_LP4(void, glMultiDrawArrays,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint *, b, A0),
         AROS_LPA(GLsizei *, c, A1),
         AROS_LPA(GLsizei, d, D1),
         LIBBASETYPEPTR, MesaBase, 495, Mesa
);
AROS_LP5(void, glMultiDrawElements,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLsizei *, b, A0),
         AROS_LPA(GLenum, c, D1),
         AROS_LPA(const GLvoid *  *, d, A1),
         AROS_LPA(GLsizei, e, D2),
         LIBBASETYPEPTR, MesaBase, 496, Mesa
);
AROS_LP2(void, glPointParameterf,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 497, Mesa
);
AROS_LP2(void, glPointParameterfv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 498, Mesa
);
AROS_LP2(void, glPointParameteri,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 499, Mesa
);
AROS_LP2(void, glPointParameteriv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 500, Mesa
);
AROS_LP3(void, glSecondaryColor3b,
         AROS_LPA(GLbyte, a, D0),
         AROS_LPA(GLbyte, b, D1),
         AROS_LPA(GLbyte, c, D2),
         LIBBASETYPEPTR, MesaBase, 501, Mesa
);
AROS_LP1(void, glSecondaryColor3bv,
         AROS_LPA(const GLbyte *, a, A0),
         LIBBASETYPEPTR, MesaBase, 502, Mesa
);
AROS_LP3(void, glSecondaryColor3d,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 503, Mesa
);
AROS_LP1(void, glSecondaryColor3dv,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 504, Mesa
);
AROS_LP3(void, glSecondaryColor3f,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 505, Mesa
);
AROS_LP1(void, glSecondaryColor3fv,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 506, Mesa
);
AROS_LP3(void, glSecondaryColor3i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 507, Mesa
);
AROS_LP1(void, glSecondaryColor3iv,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 508, Mesa
);
AROS_LP3(void, glSecondaryColor3s,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 509, Mesa
);
AROS_LP1(void, glSecondaryColor3sv,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 510, Mesa
);
AROS_LP3(void, glSecondaryColor3ub,
         AROS_LPA(GLubyte, a, D0),
         AROS_LPA(GLubyte, b, D1),
         AROS_LPA(GLubyte, c, D2),
         LIBBASETYPEPTR, MesaBase, 511, Mesa
);
AROS_LP1(void, glSecondaryColor3ubv,
         AROS_LPA(const GLubyte *, a, A0),
         LIBBASETYPEPTR, MesaBase, 512, Mesa
);
AROS_LP3(void, glSecondaryColor3ui,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         LIBBASETYPEPTR, MesaBase, 513, Mesa
);
AROS_LP1(void, glSecondaryColor3uiv,
         AROS_LPA(const GLuint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 514, Mesa
);
AROS_LP3(void, glSecondaryColor3us,
         AROS_LPA(GLushort, a, D0),
         AROS_LPA(GLushort, b, D1),
         AROS_LPA(GLushort, c, D2),
         LIBBASETYPEPTR, MesaBase, 515, Mesa
);
AROS_LP1(void, glSecondaryColor3usv,
         AROS_LPA(const GLushort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 516, Mesa
);
AROS_LP4(void, glSecondaryColorPointer,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 517, Mesa
);
AROS_LP2(void, glWindowPos2d,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 518, Mesa
);
AROS_LP1(void, glWindowPos2dv,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 519, Mesa
);
AROS_LP2(void, glWindowPos2f,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 520, Mesa
);
AROS_LP1(void, glWindowPos2fv,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 521, Mesa
);
AROS_LP2(void, glWindowPos2i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 522, Mesa
);
AROS_LP1(void, glWindowPos2iv,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 523, Mesa
);
AROS_LP2(void, glWindowPos2s,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 524, Mesa
);
AROS_LP1(void, glWindowPos2sv,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 525, Mesa
);
AROS_LP3(void, glWindowPos3d,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 526, Mesa
);
AROS_LP1(void, glWindowPos3dv,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 527, Mesa
);
AROS_LP3(void, glWindowPos3f,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 528, Mesa
);
AROS_LP1(void, glWindowPos3fv,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 529, Mesa
);
AROS_LP3(void, glWindowPos3i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 530, Mesa
);
AROS_LP1(void, glWindowPos3iv,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 531, Mesa
);
AROS_LP3(void, glWindowPos3s,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 532, Mesa
);
AROS_LP1(void, glWindowPos3sv,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 533, Mesa
);
AROS_LP2(void, glGenQueries,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 534, Mesa
);
AROS_LP2(void, glDeleteQueries,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 535, Mesa
);
AROS_LP1(GLboolean, glIsQuery,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 536, Mesa
);
AROS_LP2(void, glBeginQuery,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 537, Mesa
);
AROS_LP1(void, glEndQuery,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 538, Mesa
);
AROS_LP3(void, glGetQueryiv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 539, Mesa
);
AROS_LP3(void, glGetQueryObjectiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 540, Mesa
);
AROS_LP3(void, glGetQueryObjectuiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLuint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 541, Mesa
);
AROS_LP2(void, glBindBuffer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 542, Mesa
);
AROS_LP2(void, glDeleteBuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 543, Mesa
);
AROS_LP2(void, glGenBuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 544, Mesa
);
AROS_LP1(GLboolean, glIsBuffer,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 545, Mesa
);
AROS_LP4(void, glBufferData,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizeiptr, b, D1),
         AROS_LPA(const GLvoid *, c, A0),
         AROS_LPA(GLenum, d, D2),
         LIBBASETYPEPTR, MesaBase, 546, Mesa
);
AROS_LP4(void, glBufferSubData,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptr, b, D1),
         AROS_LPA(GLsizeiptr, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 547, Mesa
);
AROS_LP4(void, glGetBufferSubData,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptr, b, D1),
         AROS_LPA(GLsizeiptr, c, D2),
         AROS_LPA(GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 548, Mesa
);
AROS_LP2(GLvoid*, glMapBuffer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         LIBBASETYPEPTR, MesaBase, 549, Mesa
);
AROS_LP1(GLboolean, glUnmapBuffer,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 550, Mesa
);
AROS_LP3(void, glGetBufferParameteriv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 551, Mesa
);
AROS_LP3(void, glGetBufferPointerv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *  *, c, A0),
         LIBBASETYPEPTR, MesaBase, 552, Mesa
);
AROS_LP2(void, glBlendEquationSeparate,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         LIBBASETYPEPTR, MesaBase, 553, Mesa
);
AROS_LP2(void, glDrawBuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLenum *, b, A0),
         LIBBASETYPEPTR, MesaBase, 554, Mesa
);
AROS_LP4(void, glStencilOpSeparate,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         LIBBASETYPEPTR, MesaBase, 555, Mesa
);
AROS_LP4(void, glStencilFuncSeparate,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLuint, d, D3),
         LIBBASETYPEPTR, MesaBase, 556, Mesa
);
AROS_LP2(void, glStencilMaskSeparate,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 557, Mesa
);
AROS_LP2(void, glAttachShader,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 558, Mesa
);
AROS_LP3(void, glBindAttribLocation,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLchar *, c, A0),
         LIBBASETYPEPTR, MesaBase, 559, Mesa
);
AROS_LP1(void, glCompileShader,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 560, Mesa
);
AROS_LP0(GLuint, glCreateProgram,
         LIBBASETYPEPTR, MesaBase, 561, Mesa
);
AROS_LP1(GLuint, glCreateShader,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 562, Mesa
);
AROS_LP1(void, glDeleteProgram,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 563, Mesa
);
AROS_LP1(void, glDeleteShader,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 564, Mesa
);
AROS_LP2(void, glDetachShader,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 565, Mesa
);
AROS_LP1(void, glDisableVertexAttribArray,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 566, Mesa
);
AROS_LP1(void, glEnableVertexAttribArray,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 567, Mesa
);
AROS_LP7(void, glGetActiveAttrib,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei *, d, A0),
         AROS_LPA(GLint *, e, A1),
         AROS_LPA(GLenum *, f, A2),
         AROS_LPA(GLchar *, g, A3),
         LIBBASETYPEPTR, MesaBase, 568, Mesa
);
AROS_LP7(void, glGetActiveUniform,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei *, d, A0),
         AROS_LPA(GLint *, e, A1),
         AROS_LPA(GLenum *, f, A2),
         AROS_LPA(GLchar *, g, A3),
         LIBBASETYPEPTR, MesaBase, 569, Mesa
);
AROS_LP4(void, glGetAttachedShaders,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLuint *, d, A1),
         LIBBASETYPEPTR, MesaBase, 570, Mesa
);
AROS_LP2(GLint, glGetAttribLocation,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLchar *, b, A0),
         LIBBASETYPEPTR, MesaBase, 571, Mesa
);
AROS_LP3(void, glGetProgramiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 572, Mesa
);
AROS_LP4(void, glGetProgramInfoLog,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLchar *, d, A1),
         LIBBASETYPEPTR, MesaBase, 573, Mesa
);
AROS_LP3(void, glGetShaderiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 574, Mesa
);
AROS_LP4(void, glGetShaderInfoLog,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLchar *, d, A1),
         LIBBASETYPEPTR, MesaBase, 575, Mesa
);
AROS_LP4(void, glGetShaderSource,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLchar *, d, A1),
         LIBBASETYPEPTR, MesaBase, 576, Mesa
);
AROS_LP2(GLint, glGetUniformLocation,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLchar *, b, A0),
         LIBBASETYPEPTR, MesaBase, 577, Mesa
);
AROS_LP3(void, glGetUniformfv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 578, Mesa
);
AROS_LP3(void, glGetUniformiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 579, Mesa
);
AROS_LP3(void, glGetVertexAttribdv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 580, Mesa
);
AROS_LP3(void, glGetVertexAttribfv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 581, Mesa
);
AROS_LP3(void, glGetVertexAttribiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 582, Mesa
);
AROS_LP3(void, glGetVertexAttribPointerv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *  *, c, A0),
         LIBBASETYPEPTR, MesaBase, 583, Mesa
);
AROS_LP1(GLboolean, glIsProgram,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 584, Mesa
);
AROS_LP1(GLboolean, glIsShader,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 585, Mesa
);
AROS_LP1(void, glLinkProgram,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 586, Mesa
);
AROS_LP4(void, glShaderSource,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLchar *  *, c, A0),
         AROS_LPA(const GLint *, d, A1),
         LIBBASETYPEPTR, MesaBase, 587, Mesa
);
AROS_LP1(void, glUseProgram,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 588, Mesa
);
AROS_LP2(void, glUniform1f,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 589, Mesa
);
AROS_LP3(void, glUniform2f,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 590, Mesa
);
AROS_LP4(void, glUniform3f,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 591, Mesa
);
AROS_LP5(void, glUniform4f,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         LIBBASETYPEPTR, MesaBase, 592, Mesa
);
AROS_LP2(void, glUniform1i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 593, Mesa
);
AROS_LP3(void, glUniform2i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 594, Mesa
);
AROS_LP4(void, glUniform3i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         LIBBASETYPEPTR, MesaBase, 595, Mesa
);
AROS_LP5(void, glUniform4i,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 596, Mesa
);
AROS_LP3(void, glUniform1fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 597, Mesa
);
AROS_LP3(void, glUniform2fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 598, Mesa
);
AROS_LP3(void, glUniform3fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 599, Mesa
);
AROS_LP3(void, glUniform4fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 600, Mesa
);
AROS_LP3(void, glUniform1iv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 601, Mesa
);
AROS_LP3(void, glUniform2iv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 602, Mesa
);
AROS_LP3(void, glUniform3iv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 603, Mesa
);
AROS_LP3(void, glUniform4iv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 604, Mesa
);
AROS_LP4(void, glUniformMatrix2fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 605, Mesa
);
AROS_LP4(void, glUniformMatrix3fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 606, Mesa
);
AROS_LP4(void, glUniformMatrix4fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 607, Mesa
);
AROS_LP1(void, glValidateProgram,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 608, Mesa
);
AROS_LP2(void, glVertexAttrib1d,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 609, Mesa
);
AROS_LP2(void, glVertexAttrib1dv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 610, Mesa
);
AROS_LP2(void, glVertexAttrib1f,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 611, Mesa
);
AROS_LP2(void, glVertexAttrib1fv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 612, Mesa
);
AROS_LP2(void, glVertexAttrib1s,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 613, Mesa
);
AROS_LP2(void, glVertexAttrib1sv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 614, Mesa
);
AROS_LP3(void, glVertexAttrib2d,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 615, Mesa
);
AROS_LP2(void, glVertexAttrib2dv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 616, Mesa
);
AROS_LP3(void, glVertexAttrib2f,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 617, Mesa
);
AROS_LP2(void, glVertexAttrib2fv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 618, Mesa
);
AROS_LP3(void, glVertexAttrib2s,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 619, Mesa
);
AROS_LP2(void, glVertexAttrib2sv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 620, Mesa
);
AROS_LP4(void, glVertexAttrib3d,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         LIBBASETYPEPTR, MesaBase, 621, Mesa
);
AROS_LP2(void, glVertexAttrib3dv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 622, Mesa
);
AROS_LP4(void, glVertexAttrib3f,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 623, Mesa
);
AROS_LP2(void, glVertexAttrib3fv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 624, Mesa
);
AROS_LP4(void, glVertexAttrib3s,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         LIBBASETYPEPTR, MesaBase, 625, Mesa
);
AROS_LP2(void, glVertexAttrib3sv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 626, Mesa
);
AROS_LP2(void, glVertexAttrib4Nbv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLbyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 627, Mesa
);
AROS_LP2(void, glVertexAttrib4Niv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 628, Mesa
);
AROS_LP2(void, glVertexAttrib4Nsv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 629, Mesa
);
AROS_LP5(void, glVertexAttrib4Nub,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLubyte, b, D1),
         AROS_LPA(GLubyte, c, D2),
         AROS_LPA(GLubyte, d, D3),
         AROS_LPA(GLubyte, e, D4),
         LIBBASETYPEPTR, MesaBase, 630, Mesa
);
AROS_LP2(void, glVertexAttrib4Nubv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLubyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 631, Mesa
);
AROS_LP2(void, glVertexAttrib4Nuiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 632, Mesa
);
AROS_LP2(void, glVertexAttrib4Nusv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLushort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 633, Mesa
);
AROS_LP2(void, glVertexAttrib4bv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLbyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 634, Mesa
);
AROS_LP5(void, glVertexAttrib4d,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         LIBBASETYPEPTR, MesaBase, 635, Mesa
);
AROS_LP2(void, glVertexAttrib4dv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 636, Mesa
);
AROS_LP5(void, glVertexAttrib4f,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         LIBBASETYPEPTR, MesaBase, 637, Mesa
);
AROS_LP2(void, glVertexAttrib4fv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 638, Mesa
);
AROS_LP2(void, glVertexAttrib4iv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 639, Mesa
);
AROS_LP5(void, glVertexAttrib4s,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         AROS_LPA(GLshort, e, D4),
         LIBBASETYPEPTR, MesaBase, 640, Mesa
);
AROS_LP2(void, glVertexAttrib4sv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 641, Mesa
);
AROS_LP2(void, glVertexAttrib4ubv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLubyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 642, Mesa
);
AROS_LP2(void, glVertexAttrib4uiv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 643, Mesa
);
AROS_LP2(void, glVertexAttrib4usv,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLushort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 644, Mesa
);
AROS_LP6(void, glVertexAttribPointer,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLboolean, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(const GLvoid *, f, A0),
         LIBBASETYPEPTR, MesaBase, 645, Mesa
);
AROS_LP4(void, glUniformMatrix2x3fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 646, Mesa
);
AROS_LP4(void, glUniformMatrix3x2fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 647, Mesa
);
AROS_LP4(void, glUniformMatrix2x4fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 648, Mesa
);
AROS_LP4(void, glUniformMatrix4x2fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 649, Mesa
);
AROS_LP4(void, glUniformMatrix3x4fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 650, Mesa
);
AROS_LP4(void, glUniformMatrix4x3fv,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 651, Mesa
);
AROS_LP1(void, glLoadTransposeMatrixfARB,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 652, Mesa
);
AROS_LP1(void, glLoadTransposeMatrixdARB,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 653, Mesa
);
AROS_LP1(void, glMultTransposeMatrixfARB,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 654, Mesa
);
AROS_LP1(void, glMultTransposeMatrixdARB,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 655, Mesa
);
AROS_LP2(void, glSampleCoverageARB,
         AROS_LPA(GLclampf, a, D0),
         AROS_LPA(GLboolean, b, D1),
         LIBBASETYPEPTR, MesaBase, 656, Mesa
);
AROS_LP9(void, glCompressedTexImage3DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLint, g, D6),
         AROS_LPA(GLsizei, h, D7),
         AROS_LPA(const GLvoid *, i, A0),
         LIBBASETYPEPTR, MesaBase, 657, Mesa
);
AROS_LP8(void, glCompressedTexImage2DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(GLint, f, D5),
         AROS_LPA(GLsizei, g, D6),
         AROS_LPA(const GLvoid *, h, A0),
         LIBBASETYPEPTR, MesaBase, 658, Mesa
);
AROS_LP7(void, glCompressedTexImage1DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(const GLvoid *, g, A0),
         LIBBASETYPEPTR, MesaBase, 659, Mesa
);
AROS_LP11(void, glCompressedTexSubImage3DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLsizei, g, D6),
         AROS_LPA(GLsizei, h, D7),
         AROS_LPA(GLenum, i, A0),
         AROS_LPA(GLsizei, j, A1),
         AROS_LPA(const GLvoid *, k, A2),
         LIBBASETYPEPTR, MesaBase, 660, Mesa
);
AROS_LP9(void, glCompressedTexSubImage2DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLenum, g, D6),
         AROS_LPA(GLsizei, h, D7),
         AROS_LPA(const GLvoid *, i, A0),
         LIBBASETYPEPTR, MesaBase, 661, Mesa
);
AROS_LP7(void, glCompressedTexSubImage1DARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(const GLvoid *, g, A0),
         LIBBASETYPEPTR, MesaBase, 662, Mesa
);
AROS_LP3(void, glGetCompressedTexImageARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLvoid *, c, A0),
         LIBBASETYPEPTR, MesaBase, 663, Mesa
);
AROS_LP2(void, glPointParameterfARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 664, Mesa
);
AROS_LP2(void, glPointParameterfvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 665, Mesa
);
AROS_LP2(void, glWindowPos2dARB,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 666, Mesa
);
AROS_LP1(void, glWindowPos2dvARB,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 667, Mesa
);
AROS_LP2(void, glWindowPos2fARB,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 668, Mesa
);
AROS_LP1(void, glWindowPos2fvARB,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 669, Mesa
);
AROS_LP2(void, glWindowPos2iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 670, Mesa
);
AROS_LP1(void, glWindowPos2ivARB,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 671, Mesa
);
AROS_LP2(void, glWindowPos2sARB,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 672, Mesa
);
AROS_LP1(void, glWindowPos2svARB,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 673, Mesa
);
AROS_LP3(void, glWindowPos3dARB,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 674, Mesa
);
AROS_LP1(void, glWindowPos3dvARB,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 675, Mesa
);
AROS_LP3(void, glWindowPos3fARB,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 676, Mesa
);
AROS_LP1(void, glWindowPos3fvARB,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 677, Mesa
);
AROS_LP3(void, glWindowPos3iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 678, Mesa
);
AROS_LP1(void, glWindowPos3ivARB,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 679, Mesa
);
AROS_LP3(void, glWindowPos3sARB,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 680, Mesa
);
AROS_LP1(void, glWindowPos3svARB,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 681, Mesa
);
AROS_LP2(void, glVertexAttrib1dARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 682, Mesa
);
AROS_LP2(void, glVertexAttrib1dvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 683, Mesa
);
AROS_LP2(void, glVertexAttrib1fARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 684, Mesa
);
AROS_LP2(void, glVertexAttrib1fvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 685, Mesa
);
AROS_LP2(void, glVertexAttrib1sARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 686, Mesa
);
AROS_LP2(void, glVertexAttrib1svARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 687, Mesa
);
AROS_LP3(void, glVertexAttrib2dARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 688, Mesa
);
AROS_LP2(void, glVertexAttrib2dvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 689, Mesa
);
AROS_LP3(void, glVertexAttrib2fARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 690, Mesa
);
AROS_LP2(void, glVertexAttrib2fvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 691, Mesa
);
AROS_LP3(void, glVertexAttrib2sARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 692, Mesa
);
AROS_LP2(void, glVertexAttrib2svARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 693, Mesa
);
AROS_LP4(void, glVertexAttrib3dARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         LIBBASETYPEPTR, MesaBase, 694, Mesa
);
AROS_LP2(void, glVertexAttrib3dvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 695, Mesa
);
AROS_LP4(void, glVertexAttrib3fARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 696, Mesa
);
AROS_LP2(void, glVertexAttrib3fvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 697, Mesa
);
AROS_LP4(void, glVertexAttrib3sARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         LIBBASETYPEPTR, MesaBase, 698, Mesa
);
AROS_LP2(void, glVertexAttrib3svARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 699, Mesa
);
AROS_LP2(void, glVertexAttrib4NbvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLbyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 700, Mesa
);
AROS_LP2(void, glVertexAttrib4NivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 701, Mesa
);
AROS_LP2(void, glVertexAttrib4NsvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 702, Mesa
);
AROS_LP5(void, glVertexAttrib4NubARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLubyte, b, D1),
         AROS_LPA(GLubyte, c, D2),
         AROS_LPA(GLubyte, d, D3),
         AROS_LPA(GLubyte, e, D4),
         LIBBASETYPEPTR, MesaBase, 703, Mesa
);
AROS_LP2(void, glVertexAttrib4NubvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLubyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 704, Mesa
);
AROS_LP2(void, glVertexAttrib4NuivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 705, Mesa
);
AROS_LP2(void, glVertexAttrib4NusvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLushort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 706, Mesa
);
AROS_LP2(void, glVertexAttrib4bvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLbyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 707, Mesa
);
AROS_LP5(void, glVertexAttrib4dARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         LIBBASETYPEPTR, MesaBase, 708, Mesa
);
AROS_LP2(void, glVertexAttrib4dvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 709, Mesa
);
AROS_LP5(void, glVertexAttrib4fARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         LIBBASETYPEPTR, MesaBase, 710, Mesa
);
AROS_LP2(void, glVertexAttrib4fvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 711, Mesa
);
AROS_LP2(void, glVertexAttrib4ivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 712, Mesa
);
AROS_LP5(void, glVertexAttrib4sARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         AROS_LPA(GLshort, e, D4),
         LIBBASETYPEPTR, MesaBase, 713, Mesa
);
AROS_LP2(void, glVertexAttrib4svARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 714, Mesa
);
AROS_LP2(void, glVertexAttrib4ubvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLubyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 715, Mesa
);
AROS_LP2(void, glVertexAttrib4uivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 716, Mesa
);
AROS_LP2(void, glVertexAttrib4usvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLushort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 717, Mesa
);
AROS_LP6(void, glVertexAttribPointerARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLboolean, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(const GLvoid *, f, A0),
         LIBBASETYPEPTR, MesaBase, 718, Mesa
);
AROS_LP1(void, glEnableVertexAttribArrayARB,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 719, Mesa
);
AROS_LP1(void, glDisableVertexAttribArrayARB,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 720, Mesa
);
AROS_LP4(void, glProgramStringARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 721, Mesa
);
AROS_LP2(void, glBindProgramARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 722, Mesa
);
AROS_LP2(void, glDeleteProgramsARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 723, Mesa
);
AROS_LP2(void, glGenProgramsARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 724, Mesa
);
AROS_LP6(void, glProgramEnvParameter4dARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         AROS_LPA(GLdouble, f, D5),
         LIBBASETYPEPTR, MesaBase, 725, Mesa
);
AROS_LP3(void, glProgramEnvParameter4dvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 726, Mesa
);
AROS_LP6(void, glProgramEnvParameter4fARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         AROS_LPA(GLfloat, f, D5),
         LIBBASETYPEPTR, MesaBase, 727, Mesa
);
AROS_LP3(void, glProgramEnvParameter4fvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 728, Mesa
);
AROS_LP6(void, glProgramLocalParameter4dARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         AROS_LPA(GLdouble, f, D5),
         LIBBASETYPEPTR, MesaBase, 729, Mesa
);
AROS_LP3(void, glProgramLocalParameter4dvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 730, Mesa
);
AROS_LP6(void, glProgramLocalParameter4fARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         AROS_LPA(GLfloat, f, D5),
         LIBBASETYPEPTR, MesaBase, 731, Mesa
);
AROS_LP3(void, glProgramLocalParameter4fvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 732, Mesa
);
AROS_LP3(void, glGetProgramEnvParameterdvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 733, Mesa
);
AROS_LP3(void, glGetProgramEnvParameterfvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 734, Mesa
);
AROS_LP3(void, glGetProgramLocalParameterdvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 735, Mesa
);
AROS_LP3(void, glGetProgramLocalParameterfvARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 736, Mesa
);
AROS_LP3(void, glGetProgramivARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 737, Mesa
);
AROS_LP3(void, glGetProgramStringARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *, c, A0),
         LIBBASETYPEPTR, MesaBase, 738, Mesa
);
AROS_LP3(void, glGetVertexAttribdvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 739, Mesa
);
AROS_LP3(void, glGetVertexAttribfvARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 740, Mesa
);
AROS_LP3(void, glGetVertexAttribivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 741, Mesa
);
AROS_LP3(void, glGetVertexAttribPointervARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *  *, c, A0),
         LIBBASETYPEPTR, MesaBase, 742, Mesa
);
AROS_LP1(GLboolean, glIsProgramARB,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 743, Mesa
);
AROS_LP2(void, glBindBufferARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 744, Mesa
);
AROS_LP2(void, glDeleteBuffersARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 745, Mesa
);
AROS_LP2(void, glGenBuffersARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 746, Mesa
);
AROS_LP1(GLboolean, glIsBufferARB,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 747, Mesa
);
AROS_LP4(void, glBufferDataARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizeiptrARB, b, D1),
         AROS_LPA(const GLvoid *, c, A0),
         AROS_LPA(GLenum, d, D2),
         LIBBASETYPEPTR, MesaBase, 748, Mesa
);
AROS_LP4(void, glBufferSubDataARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptrARB, b, D1),
         AROS_LPA(GLsizeiptrARB, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 749, Mesa
);
AROS_LP4(void, glGetBufferSubDataARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptrARB, b, D1),
         AROS_LPA(GLsizeiptrARB, c, D2),
         AROS_LPA(GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 750, Mesa
);
AROS_LP2(GLvoid*, glMapBufferARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         LIBBASETYPEPTR, MesaBase, 751, Mesa
);
AROS_LP1(GLboolean, glUnmapBufferARB,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 752, Mesa
);
AROS_LP3(void, glGetBufferParameterivARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 753, Mesa
);
AROS_LP3(void, glGetBufferPointervARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *  *, c, A0),
         LIBBASETYPEPTR, MesaBase, 754, Mesa
);
AROS_LP2(void, glGenQueriesARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 755, Mesa
);
AROS_LP2(void, glDeleteQueriesARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 756, Mesa
);
AROS_LP1(GLboolean, glIsQueryARB,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 757, Mesa
);
AROS_LP2(void, glBeginQueryARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 758, Mesa
);
AROS_LP1(void, glEndQueryARB,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 759, Mesa
);
AROS_LP3(void, glGetQueryivARB,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 760, Mesa
);
AROS_LP3(void, glGetQueryObjectivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 761, Mesa
);
AROS_LP3(void, glGetQueryObjectuivARB,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLuint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 762, Mesa
);
AROS_LP1(void, glDeleteObjectARB,
         AROS_LPA(GLhandleARB, a, D0),
         LIBBASETYPEPTR, MesaBase, 763, Mesa
);
AROS_LP1(GLhandleARB, glGetHandleARB,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 764, Mesa
);
AROS_LP2(void, glDetachObjectARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLhandleARB, b, D1),
         LIBBASETYPEPTR, MesaBase, 765, Mesa
);
AROS_LP1(GLhandleARB, glCreateShaderObjectARB,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 766, Mesa
);
AROS_LP4(void, glShaderSourceARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLcharARB *  *, c, A0),
         AROS_LPA(const GLint *, d, A1),
         LIBBASETYPEPTR, MesaBase, 767, Mesa
);
AROS_LP1(void, glCompileShaderARB,
         AROS_LPA(GLhandleARB, a, D0),
         LIBBASETYPEPTR, MesaBase, 768, Mesa
);
AROS_LP0(GLhandleARB, glCreateProgramObjectARB,
         LIBBASETYPEPTR, MesaBase, 769, Mesa
);
AROS_LP2(void, glAttachObjectARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLhandleARB, b, D1),
         LIBBASETYPEPTR, MesaBase, 770, Mesa
);
AROS_LP1(void, glLinkProgramARB,
         AROS_LPA(GLhandleARB, a, D0),
         LIBBASETYPEPTR, MesaBase, 771, Mesa
);
AROS_LP1(void, glUseProgramObjectARB,
         AROS_LPA(GLhandleARB, a, D0),
         LIBBASETYPEPTR, MesaBase, 772, Mesa
);
AROS_LP1(void, glValidateProgramARB,
         AROS_LPA(GLhandleARB, a, D0),
         LIBBASETYPEPTR, MesaBase, 773, Mesa
);
AROS_LP2(void, glUniform1fARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 774, Mesa
);
AROS_LP3(void, glUniform2fARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 775, Mesa
);
AROS_LP4(void, glUniform3fARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 776, Mesa
);
AROS_LP5(void, glUniform4fARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         LIBBASETYPEPTR, MesaBase, 777, Mesa
);
AROS_LP2(void, glUniform1iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 778, Mesa
);
AROS_LP3(void, glUniform2iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 779, Mesa
);
AROS_LP4(void, glUniform3iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         LIBBASETYPEPTR, MesaBase, 780, Mesa
);
AROS_LP5(void, glUniform4iARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 781, Mesa
);
AROS_LP3(void, glUniform1fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 782, Mesa
);
AROS_LP3(void, glUniform2fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 783, Mesa
);
AROS_LP3(void, glUniform3fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 784, Mesa
);
AROS_LP3(void, glUniform4fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 785, Mesa
);
AROS_LP3(void, glUniform1ivARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 786, Mesa
);
AROS_LP3(void, glUniform2ivARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 787, Mesa
);
AROS_LP3(void, glUniform3ivARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 788, Mesa
);
AROS_LP3(void, glUniform4ivARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 789, Mesa
);
AROS_LP4(void, glUniformMatrix2fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 790, Mesa
);
AROS_LP4(void, glUniformMatrix3fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 791, Mesa
);
AROS_LP4(void, glUniformMatrix4fvARB,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 792, Mesa
);
AROS_LP3(void, glGetObjectParameterfvARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 793, Mesa
);
AROS_LP3(void, glGetObjectParameterivARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 794, Mesa
);
AROS_LP4(void, glGetInfoLogARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLcharARB *, d, A1),
         LIBBASETYPEPTR, MesaBase, 795, Mesa
);
AROS_LP4(void, glGetAttachedObjectsARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLhandleARB *, d, A1),
         LIBBASETYPEPTR, MesaBase, 796, Mesa
);
AROS_LP2(GLint, glGetUniformLocationARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(const GLcharARB *, b, A0),
         LIBBASETYPEPTR, MesaBase, 797, Mesa
);
AROS_LP7(void, glGetActiveUniformARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei *, d, A0),
         AROS_LPA(GLint *, e, A1),
         AROS_LPA(GLenum *, f, A2),
         AROS_LPA(GLcharARB *, g, A3),
         LIBBASETYPEPTR, MesaBase, 798, Mesa
);
AROS_LP3(void, glGetUniformfvARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 799, Mesa
);
AROS_LP3(void, glGetUniformivARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 800, Mesa
);
AROS_LP4(void, glGetShaderSourceARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei *, c, A0),
         AROS_LPA(GLcharARB *, d, A1),
         LIBBASETYPEPTR, MesaBase, 801, Mesa
);
AROS_LP3(void, glBindAttribLocationARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLcharARB *, c, A0),
         LIBBASETYPEPTR, MesaBase, 802, Mesa
);
AROS_LP7(void, glGetActiveAttribARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei *, d, A0),
         AROS_LPA(GLint *, e, A1),
         AROS_LPA(GLenum *, f, A2),
         AROS_LPA(GLcharARB *, g, A3),
         LIBBASETYPEPTR, MesaBase, 803, Mesa
);
AROS_LP2(GLint, glGetAttribLocationARB,
         AROS_LPA(GLhandleARB, a, D0),
         AROS_LPA(const GLcharARB *, b, A0),
         LIBBASETYPEPTR, MesaBase, 804, Mesa
);
AROS_LP2(void, glDrawBuffersARB,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLenum *, b, A0),
         LIBBASETYPEPTR, MesaBase, 805, Mesa
);
AROS_LP1(GLboolean, glIsRenderbuffer,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 806, Mesa
);
AROS_LP2(void, glBindRenderbuffer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 807, Mesa
);
AROS_LP2(void, glDeleteRenderbuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 808, Mesa
);
AROS_LP2(void, glGenRenderbuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 809, Mesa
);
AROS_LP4(void, glRenderbufferStorage,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei, d, D3),
         LIBBASETYPEPTR, MesaBase, 810, Mesa
);
AROS_LP3(void, glGetRenderbufferParameteriv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 811, Mesa
);
AROS_LP1(GLboolean, glIsFramebuffer,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 812, Mesa
);
AROS_LP2(void, glBindFramebuffer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 813, Mesa
);
AROS_LP2(void, glDeleteFramebuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 814, Mesa
);
AROS_LP2(void, glGenFramebuffers,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 815, Mesa
);
AROS_LP1(GLenum, glCheckFramebufferStatus,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 816, Mesa
);
AROS_LP5(void, glFramebufferTexture1D,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 817, Mesa
);
AROS_LP5(void, glFramebufferTexture2D,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 818, Mesa
);
AROS_LP6(void, glFramebufferTexture3D,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLint, f, D5),
         LIBBASETYPEPTR, MesaBase, 819, Mesa
);
AROS_LP4(void, glFramebufferRenderbuffer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         LIBBASETYPEPTR, MesaBase, 820, Mesa
);
AROS_LP4(void, glGetFramebufferAttachmentParameteriv,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint *, d, A0),
         LIBBASETYPEPTR, MesaBase, 821, Mesa
);
AROS_LP1(void, glGenerateMipmap,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 822, Mesa
);
AROS_LP10(void, glBlitFramebuffer,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLint, f, D5),
         AROS_LPA(GLint, g, D6),
         AROS_LPA(GLint, h, D7),
         AROS_LPA(GLbitfield, i, A0),
         AROS_LPA(GLenum, j, A1),
         LIBBASETYPEPTR, MesaBase, 823, Mesa
);
AROS_LP5(void, glRenderbufferStorageMultisample,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLsizei, e, D4),
         LIBBASETYPEPTR, MesaBase, 824, Mesa
);
AROS_LP5(void, glFramebufferTextureLayer,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 825, Mesa
);
AROS_LP4(void, glBlendColorEXT,
         AROS_LPA(GLclampf, a, D0),
         AROS_LPA(GLclampf, b, D1),
         AROS_LPA(GLclampf, c, D2),
         AROS_LPA(GLclampf, d, D3),
         LIBBASETYPEPTR, MesaBase, 826, Mesa
);
AROS_LP2(void, glPolygonOffsetEXT,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 827, Mesa
);
AROS_LP10(void, glTexImage3DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLint, g, D6),
         AROS_LPA(GLenum, h, D7),
         AROS_LPA(GLenum, i, A0),
         AROS_LPA(const GLvoid *, j, A1),
         LIBBASETYPEPTR, MesaBase, 828, Mesa
);
AROS_LP11(void, glTexSubImage3DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLsizei, g, D6),
         AROS_LPA(GLsizei, h, D7),
         AROS_LPA(GLenum, i, A0),
         AROS_LPA(GLenum, j, A1),
         AROS_LPA(const GLvoid *, k, A2),
         LIBBASETYPEPTR, MesaBase, 829, Mesa
);
AROS_LP7(void, glTexSubImage1DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(GLenum, f, D5),
         AROS_LPA(const GLvoid *, g, A0),
         LIBBASETYPEPTR, MesaBase, 830, Mesa
);
AROS_LP9(void, glTexSubImage2DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLsizei, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLenum, g, D6),
         AROS_LPA(GLenum, h, D7),
         AROS_LPA(const GLvoid *, i, A0),
         LIBBASETYPEPTR, MesaBase, 831, Mesa
);
AROS_LP7(void, glCopyTexImage1DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLint, g, D6),
         LIBBASETYPEPTR, MesaBase, 832, Mesa
);
AROS_LP8(void, glCopyTexImage2DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         AROS_LPA(GLsizei, g, D6),
         AROS_LPA(GLint, h, D7),
         LIBBASETYPEPTR, MesaBase, 833, Mesa
);
AROS_LP6(void, glCopyTexSubImage1DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLsizei, f, D5),
         LIBBASETYPEPTR, MesaBase, 834, Mesa
);
AROS_LP8(void, glCopyTexSubImage2DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLint, f, D5),
         AROS_LPA(GLsizei, g, D6),
         AROS_LPA(GLsizei, h, D7),
         LIBBASETYPEPTR, MesaBase, 835, Mesa
);
AROS_LP9(void, glCopyTexSubImage3DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLint, f, D5),
         AROS_LPA(GLint, g, D6),
         AROS_LPA(GLsizei, h, D7),
         AROS_LPA(GLsizei, i, A0),
         LIBBASETYPEPTR, MesaBase, 836, Mesa
);
AROS_LP3(GLboolean, glAreTexturesResidentEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         AROS_LPA(GLboolean *, c, A1),
         LIBBASETYPEPTR, MesaBase, 837, Mesa
);
AROS_LP2(void, glBindTextureEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 838, Mesa
);
AROS_LP2(void, glDeleteTexturesEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 839, Mesa
);
AROS_LP2(void, glGenTexturesEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 840, Mesa
);
AROS_LP1(GLboolean, glIsTextureEXT,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 841, Mesa
);
AROS_LP3(void, glPrioritizeTexturesEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         AROS_LPA(const GLclampf *, c, A1),
         LIBBASETYPEPTR, MesaBase, 842, Mesa
);
AROS_LP1(void, glArrayElementEXT,
         AROS_LPA(GLint, a, D0),
         LIBBASETYPEPTR, MesaBase, 843, Mesa
);
AROS_LP5(void, glColorPointerEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(const GLvoid *, e, A0),
         LIBBASETYPEPTR, MesaBase, 844, Mesa
);
AROS_LP3(void, glDrawArraysEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         LIBBASETYPEPTR, MesaBase, 845, Mesa
);
AROS_LP3(void, glEdgeFlagPointerEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLboolean *, c, A0),
         LIBBASETYPEPTR, MesaBase, 846, Mesa
);
AROS_LP2(void, glGetPointervEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLvoid *  *, b, A0),
         LIBBASETYPEPTR, MesaBase, 847, Mesa
);
AROS_LP4(void, glIndexPointerEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 848, Mesa
);
AROS_LP4(void, glNormalPointerEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 849, Mesa
);
AROS_LP5(void, glTexCoordPointerEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(const GLvoid *, e, A0),
         LIBBASETYPEPTR, MesaBase, 850, Mesa
);
AROS_LP5(void, glVertexPointerEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(const GLvoid *, e, A0),
         LIBBASETYPEPTR, MesaBase, 851, Mesa
);
AROS_LP1(void, glBlendEquationEXT,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 852, Mesa
);
AROS_LP2(void, glPointParameterfEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 853, Mesa
);
AROS_LP2(void, glPointParameterfvEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 854, Mesa
);
AROS_LP6(void, glColorTableEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLenum, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(const GLvoid *, f, A0),
         LIBBASETYPEPTR, MesaBase, 855, Mesa
);
AROS_LP4(void, glGetColorTableEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 856, Mesa
);
AROS_LP3(void, glGetColorTableParameterivEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 857, Mesa
);
AROS_LP3(void, glGetColorTableParameterfvEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 858, Mesa
);
AROS_LP2(void, glLockArraysEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         LIBBASETYPEPTR, MesaBase, 859, Mesa
);
AROS_LP0(void, glUnlockArraysEXT,
         LIBBASETYPEPTR, MesaBase, 860, Mesa
);
AROS_LP6(void, glDrawRangeElementsEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(const GLvoid *, f, A0),
         LIBBASETYPEPTR, MesaBase, 861, Mesa
);
AROS_LP3(void, glSecondaryColor3bEXT,
         AROS_LPA(GLbyte, a, D0),
         AROS_LPA(GLbyte, b, D1),
         AROS_LPA(GLbyte, c, D2),
         LIBBASETYPEPTR, MesaBase, 862, Mesa
);
AROS_LP1(void, glSecondaryColor3bvEXT,
         AROS_LPA(const GLbyte *, a, A0),
         LIBBASETYPEPTR, MesaBase, 863, Mesa
);
AROS_LP3(void, glSecondaryColor3dEXT,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 864, Mesa
);
AROS_LP1(void, glSecondaryColor3dvEXT,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 865, Mesa
);
AROS_LP3(void, glSecondaryColor3fEXT,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 866, Mesa
);
AROS_LP1(void, glSecondaryColor3fvEXT,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 867, Mesa
);
AROS_LP3(void, glSecondaryColor3iEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 868, Mesa
);
AROS_LP1(void, glSecondaryColor3ivEXT,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 869, Mesa
);
AROS_LP3(void, glSecondaryColor3sEXT,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 870, Mesa
);
AROS_LP1(void, glSecondaryColor3svEXT,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 871, Mesa
);
AROS_LP3(void, glSecondaryColor3ubEXT,
         AROS_LPA(GLubyte, a, D0),
         AROS_LPA(GLubyte, b, D1),
         AROS_LPA(GLubyte, c, D2),
         LIBBASETYPEPTR, MesaBase, 872, Mesa
);
AROS_LP1(void, glSecondaryColor3ubvEXT,
         AROS_LPA(const GLubyte *, a, A0),
         LIBBASETYPEPTR, MesaBase, 873, Mesa
);
AROS_LP3(void, glSecondaryColor3uiEXT,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         LIBBASETYPEPTR, MesaBase, 874, Mesa
);
AROS_LP1(void, glSecondaryColor3uivEXT,
         AROS_LPA(const GLuint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 875, Mesa
);
AROS_LP3(void, glSecondaryColor3usEXT,
         AROS_LPA(GLushort, a, D0),
         AROS_LPA(GLushort, b, D1),
         AROS_LPA(GLushort, c, D2),
         LIBBASETYPEPTR, MesaBase, 876, Mesa
);
AROS_LP1(void, glSecondaryColor3usvEXT,
         AROS_LPA(const GLushort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 877, Mesa
);
AROS_LP4(void, glSecondaryColorPointerEXT,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         LIBBASETYPEPTR, MesaBase, 878, Mesa
);
AROS_LP4(void, glMultiDrawArraysEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint *, b, A0),
         AROS_LPA(GLsizei *, c, A1),
         AROS_LPA(GLsizei, d, D1),
         LIBBASETYPEPTR, MesaBase, 879, Mesa
);
AROS_LP5(void, glMultiDrawElementsEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLsizei *, b, A0),
         AROS_LPA(GLenum, c, D1),
         AROS_LPA(const GLvoid *  *, d, A1),
         AROS_LPA(GLsizei, e, D2),
         LIBBASETYPEPTR, MesaBase, 880, Mesa
);
AROS_LP1(void, glFogCoordfEXT,
         AROS_LPA(GLfloat, a, D0),
         LIBBASETYPEPTR, MesaBase, 881, Mesa
);
AROS_LP1(void, glFogCoordfvEXT,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 882, Mesa
);
AROS_LP1(void, glFogCoorddEXT,
         AROS_LPA(GLdouble, a, D0),
         LIBBASETYPEPTR, MesaBase, 883, Mesa
);
AROS_LP1(void, glFogCoorddvEXT,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 884, Mesa
);
AROS_LP3(void, glFogCoordPointerEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLvoid *, c, A0),
         LIBBASETYPEPTR, MesaBase, 885, Mesa
);
AROS_LP4(void, glBlendFuncSeparateEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         LIBBASETYPEPTR, MesaBase, 886, Mesa
);
AROS_LP0(void, glFlushVertexArrayRangeNV,
         LIBBASETYPEPTR, MesaBase, 887, Mesa
);
AROS_LP2(void, glVertexArrayRangeNV,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLvoid *, b, A0),
         LIBBASETYPEPTR, MesaBase, 888, Mesa
);
AROS_LP2(void, glCombinerParameterfvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 889, Mesa
);
AROS_LP2(void, glCombinerParameterfNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 890, Mesa
);
AROS_LP2(void, glCombinerParameterivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 891, Mesa
);
AROS_LP2(void, glCombinerParameteriNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 892, Mesa
);
AROS_LP6(void, glCombinerInputNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(GLenum, f, D5),
         LIBBASETYPEPTR, MesaBase, 893, Mesa
);
AROS_LP10(void, glCombinerOutputNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(GLenum, f, D5),
         AROS_LPA(GLenum, g, D6),
         AROS_LPA(GLboolean, h, D7),
         AROS_LPA(GLboolean, i, A0),
         AROS_LPA(GLboolean, j, A1),
         LIBBASETYPEPTR, MesaBase, 894, Mesa
);
AROS_LP4(void, glFinalCombinerInputNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         LIBBASETYPEPTR, MesaBase, 895, Mesa
);
AROS_LP5(void, glGetCombinerInputParameterfvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         AROS_LPA(GLfloat *, e, A0),
         LIBBASETYPEPTR, MesaBase, 896, Mesa
);
AROS_LP5(void, glGetCombinerInputParameterivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         AROS_LPA(GLint *, e, A0),
         LIBBASETYPEPTR, MesaBase, 897, Mesa
);
AROS_LP4(void, glGetCombinerOutputParameterfvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 898, Mesa
);
AROS_LP4(void, glGetCombinerOutputParameterivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint *, d, A0),
         LIBBASETYPEPTR, MesaBase, 899, Mesa
);
AROS_LP3(void, glGetFinalCombinerInputParameterfvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 900, Mesa
);
AROS_LP3(void, glGetFinalCombinerInputParameterivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 901, Mesa
);
AROS_LP0(void, glResizeBuffersMESA,
         LIBBASETYPEPTR, MesaBase, 902, Mesa
);
AROS_LP2(void, glWindowPos2dMESA,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 903, Mesa
);
AROS_LP1(void, glWindowPos2dvMESA,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 904, Mesa
);
AROS_LP2(void, glWindowPos2fMESA,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 905, Mesa
);
AROS_LP1(void, glWindowPos2fvMESA,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 906, Mesa
);
AROS_LP2(void, glWindowPos2iMESA,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 907, Mesa
);
AROS_LP1(void, glWindowPos2ivMESA,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 908, Mesa
);
AROS_LP2(void, glWindowPos2sMESA,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 909, Mesa
);
AROS_LP1(void, glWindowPos2svMESA,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 910, Mesa
);
AROS_LP3(void, glWindowPos3dMESA,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 911, Mesa
);
AROS_LP1(void, glWindowPos3dvMESA,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 912, Mesa
);
AROS_LP3(void, glWindowPos3fMESA,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 913, Mesa
);
AROS_LP1(void, glWindowPos3fvMESA,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 914, Mesa
);
AROS_LP3(void, glWindowPos3iMESA,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         LIBBASETYPEPTR, MesaBase, 915, Mesa
);
AROS_LP1(void, glWindowPos3ivMESA,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 916, Mesa
);
AROS_LP3(void, glWindowPos3sMESA,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 917, Mesa
);
AROS_LP1(void, glWindowPos3svMESA,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 918, Mesa
);
AROS_LP4(void, glWindowPos4dMESA,
         AROS_LPA(GLdouble, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         LIBBASETYPEPTR, MesaBase, 919, Mesa
);
AROS_LP1(void, glWindowPos4dvMESA,
         AROS_LPA(const GLdouble *, a, A0),
         LIBBASETYPEPTR, MesaBase, 920, Mesa
);
AROS_LP4(void, glWindowPos4fMESA,
         AROS_LPA(GLfloat, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 921, Mesa
);
AROS_LP1(void, glWindowPos4fvMESA,
         AROS_LPA(const GLfloat *, a, A0),
         LIBBASETYPEPTR, MesaBase, 922, Mesa
);
AROS_LP4(void, glWindowPos4iMESA,
         AROS_LPA(GLint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLint, c, D2),
         AROS_LPA(GLint, d, D3),
         LIBBASETYPEPTR, MesaBase, 923, Mesa
);
AROS_LP1(void, glWindowPos4ivMESA,
         AROS_LPA(const GLint *, a, A0),
         LIBBASETYPEPTR, MesaBase, 924, Mesa
);
AROS_LP4(void, glWindowPos4sMESA,
         AROS_LPA(GLshort, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         LIBBASETYPEPTR, MesaBase, 925, Mesa
);
AROS_LP1(void, glWindowPos4svMESA,
         AROS_LPA(const GLshort *, a, A0),
         LIBBASETYPEPTR, MesaBase, 926, Mesa
);
AROS_LP3(GLboolean, glAreProgramsResidentNV,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         AROS_LPA(GLboolean *, c, A1),
         LIBBASETYPEPTR, MesaBase, 927, Mesa
);
AROS_LP2(void, glBindProgramNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 928, Mesa
);
AROS_LP2(void, glDeleteProgramsNV,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 929, Mesa
);
AROS_LP3(void, glExecuteProgramNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 930, Mesa
);
AROS_LP2(void, glGenProgramsNV,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 931, Mesa
);
AROS_LP4(void, glGetProgramParameterdvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLdouble *, d, A0),
         LIBBASETYPEPTR, MesaBase, 932, Mesa
);
AROS_LP4(void, glGetProgramParameterfvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 933, Mesa
);
AROS_LP3(void, glGetProgramivNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 934, Mesa
);
AROS_LP3(void, glGetProgramStringNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLubyte *, c, A0),
         LIBBASETYPEPTR, MesaBase, 935, Mesa
);
AROS_LP4(void, glGetTrackMatrixivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint *, d, A0),
         LIBBASETYPEPTR, MesaBase, 936, Mesa
);
AROS_LP3(void, glGetVertexAttribdvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 937, Mesa
);
AROS_LP3(void, glGetVertexAttribfvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 938, Mesa
);
AROS_LP3(void, glGetVertexAttribivNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 939, Mesa
);
AROS_LP3(void, glGetVertexAttribPointervNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLvoid *  *, c, A0),
         LIBBASETYPEPTR, MesaBase, 940, Mesa
);
AROS_LP1(GLboolean, glIsProgramNV,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 941, Mesa
);
AROS_LP4(void, glLoadProgramNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(const GLubyte *, d, A0),
         LIBBASETYPEPTR, MesaBase, 942, Mesa
);
AROS_LP6(void, glProgramParameter4dNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         AROS_LPA(GLdouble, f, D5),
         LIBBASETYPEPTR, MesaBase, 943, Mesa
);
AROS_LP3(void, glProgramParameter4dvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 944, Mesa
);
AROS_LP6(void, glProgramParameter4fNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         AROS_LPA(GLfloat, f, D5),
         LIBBASETYPEPTR, MesaBase, 945, Mesa
);
AROS_LP3(void, glProgramParameter4fvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 946, Mesa
);
AROS_LP4(void, glProgramParameters4dvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(const GLdouble *, d, A0),
         LIBBASETYPEPTR, MesaBase, 947, Mesa
);
AROS_LP4(void, glProgramParameters4fvNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(const GLfloat *, d, A0),
         LIBBASETYPEPTR, MesaBase, 948, Mesa
);
AROS_LP2(void, glRequestResidentProgramsNV,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 949, Mesa
);
AROS_LP4(void, glTrackMatrixNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLenum, d, D3),
         LIBBASETYPEPTR, MesaBase, 950, Mesa
);
AROS_LP5(void, glVertexAttribPointerNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(const GLvoid *, e, A0),
         LIBBASETYPEPTR, MesaBase, 951, Mesa
);
AROS_LP2(void, glVertexAttrib1dNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         LIBBASETYPEPTR, MesaBase, 952, Mesa
);
AROS_LP2(void, glVertexAttrib1dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 953, Mesa
);
AROS_LP2(void, glVertexAttrib1fNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         LIBBASETYPEPTR, MesaBase, 954, Mesa
);
AROS_LP2(void, glVertexAttrib1fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 955, Mesa
);
AROS_LP2(void, glVertexAttrib1sNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         LIBBASETYPEPTR, MesaBase, 956, Mesa
);
AROS_LP2(void, glVertexAttrib1svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 957, Mesa
);
AROS_LP3(void, glVertexAttrib2dNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         LIBBASETYPEPTR, MesaBase, 958, Mesa
);
AROS_LP2(void, glVertexAttrib2dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 959, Mesa
);
AROS_LP3(void, glVertexAttrib2fNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         LIBBASETYPEPTR, MesaBase, 960, Mesa
);
AROS_LP2(void, glVertexAttrib2fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 961, Mesa
);
AROS_LP3(void, glVertexAttrib2sNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         LIBBASETYPEPTR, MesaBase, 962, Mesa
);
AROS_LP2(void, glVertexAttrib2svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 963, Mesa
);
AROS_LP4(void, glVertexAttrib3dNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         LIBBASETYPEPTR, MesaBase, 964, Mesa
);
AROS_LP2(void, glVertexAttrib3dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 965, Mesa
);
AROS_LP4(void, glVertexAttrib3fNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         LIBBASETYPEPTR, MesaBase, 966, Mesa
);
AROS_LP2(void, glVertexAttrib3fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 967, Mesa
);
AROS_LP4(void, glVertexAttrib3sNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         LIBBASETYPEPTR, MesaBase, 968, Mesa
);
AROS_LP2(void, glVertexAttrib3svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 969, Mesa
);
AROS_LP5(void, glVertexAttrib4dNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLdouble, b, D1),
         AROS_LPA(GLdouble, c, D2),
         AROS_LPA(GLdouble, d, D3),
         AROS_LPA(GLdouble, e, D4),
         LIBBASETYPEPTR, MesaBase, 970, Mesa
);
AROS_LP2(void, glVertexAttrib4dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLdouble *, b, A0),
         LIBBASETYPEPTR, MesaBase, 971, Mesa
);
AROS_LP5(void, glVertexAttrib4fNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLfloat, b, D1),
         AROS_LPA(GLfloat, c, D2),
         AROS_LPA(GLfloat, d, D3),
         AROS_LPA(GLfloat, e, D4),
         LIBBASETYPEPTR, MesaBase, 972, Mesa
);
AROS_LP2(void, glVertexAttrib4fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 973, Mesa
);
AROS_LP5(void, glVertexAttrib4sNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLshort, b, D1),
         AROS_LPA(GLshort, c, D2),
         AROS_LPA(GLshort, d, D3),
         AROS_LPA(GLshort, e, D4),
         LIBBASETYPEPTR, MesaBase, 974, Mesa
);
AROS_LP2(void, glVertexAttrib4svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLshort *, b, A0),
         LIBBASETYPEPTR, MesaBase, 975, Mesa
);
AROS_LP5(void, glVertexAttrib4ubNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLubyte, b, D1),
         AROS_LPA(GLubyte, c, D2),
         AROS_LPA(GLubyte, d, D3),
         AROS_LPA(GLubyte, e, D4),
         LIBBASETYPEPTR, MesaBase, 976, Mesa
);
AROS_LP2(void, glVertexAttrib4ubvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLubyte *, b, A0),
         LIBBASETYPEPTR, MesaBase, 977, Mesa
);
AROS_LP3(void, glVertexAttribs1dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 978, Mesa
);
AROS_LP3(void, glVertexAttribs1fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 979, Mesa
);
AROS_LP3(void, glVertexAttribs1svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLshort *, c, A0),
         LIBBASETYPEPTR, MesaBase, 980, Mesa
);
AROS_LP3(void, glVertexAttribs2dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 981, Mesa
);
AROS_LP3(void, glVertexAttribs2fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 982, Mesa
);
AROS_LP3(void, glVertexAttribs2svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLshort *, c, A0),
         LIBBASETYPEPTR, MesaBase, 983, Mesa
);
AROS_LP3(void, glVertexAttribs3dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 984, Mesa
);
AROS_LP3(void, glVertexAttribs3fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 985, Mesa
);
AROS_LP3(void, glVertexAttribs3svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLshort *, c, A0),
         LIBBASETYPEPTR, MesaBase, 986, Mesa
);
AROS_LP3(void, glVertexAttribs4dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLdouble *, c, A0),
         LIBBASETYPEPTR, MesaBase, 987, Mesa
);
AROS_LP3(void, glVertexAttribs4fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLfloat *, c, A0),
         LIBBASETYPEPTR, MesaBase, 988, Mesa
);
AROS_LP3(void, glVertexAttribs4svNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLshort *, c, A0),
         LIBBASETYPEPTR, MesaBase, 989, Mesa
);
AROS_LP3(void, glVertexAttribs4ubvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         LIBBASETYPEPTR, MesaBase, 990, Mesa
);
AROS_LP2(void, glTexBumpParameterivATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 991, Mesa
);
AROS_LP2(void, glTexBumpParameterfvATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 992, Mesa
);
AROS_LP2(void, glGetTexBumpParameterivATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 993, Mesa
);
AROS_LP2(void, glGetTexBumpParameterfvATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 994, Mesa
);
AROS_LP1(GLuint, glGenFragmentShadersATI,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 995, Mesa
);
AROS_LP1(void, glBindFragmentShaderATI,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 996, Mesa
);
AROS_LP1(void, glDeleteFragmentShaderATI,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 997, Mesa
);
AROS_LP0(void, glBeginFragmentShaderATI,
         LIBBASETYPEPTR, MesaBase, 998, Mesa
);
AROS_LP0(void, glEndFragmentShaderATI,
         LIBBASETYPEPTR, MesaBase, 999, Mesa
);
AROS_LP3(void, glPassTexCoordATI,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         LIBBASETYPEPTR, MesaBase, 1000, Mesa
);
AROS_LP3(void, glSampleMapATI,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         LIBBASETYPEPTR, MesaBase, 1001, Mesa
);
AROS_LP7(void, glColorFragmentOp1ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         AROS_LPA(GLuint, g, D6),
         LIBBASETYPEPTR, MesaBase, 1002, Mesa
);
AROS_LP10(void, glColorFragmentOp2ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         AROS_LPA(GLuint, g, D6),
         AROS_LPA(GLuint, h, D7),
         AROS_LPA(GLuint, i, A0),
         AROS_LPA(GLuint, j, A1),
         LIBBASETYPEPTR, MesaBase, 1003, Mesa
);
AROS_LP13(void, glColorFragmentOp3ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         AROS_LPA(GLuint, g, D6),
         AROS_LPA(GLuint, h, D7),
         AROS_LPA(GLuint, i, A0),
         AROS_LPA(GLuint, j, A1),
         AROS_LPA(GLuint, k, A2),
         AROS_LPA(GLuint, l, A3),
         AROS_LPA(GLuint, m, A4),
         LIBBASETYPEPTR, MesaBase, 1004, Mesa
);
AROS_LP6(void, glAlphaFragmentOp1ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         LIBBASETYPEPTR, MesaBase, 1005, Mesa
);
AROS_LP9(void, glAlphaFragmentOp2ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         AROS_LPA(GLuint, g, D6),
         AROS_LPA(GLuint, h, D7),
         AROS_LPA(GLuint, i, A0),
         LIBBASETYPEPTR, MesaBase, 1006, Mesa
);
AROS_LP12(void, glAlphaFragmentOp3ATI,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLuint, e, D4),
         AROS_LPA(GLuint, f, D5),
         AROS_LPA(GLuint, g, D6),
         AROS_LPA(GLuint, h, D7),
         AROS_LPA(GLuint, i, A0),
         AROS_LPA(GLuint, j, A1),
         AROS_LPA(GLuint, k, A2),
         AROS_LPA(GLuint, l, A3),
         LIBBASETYPEPTR, MesaBase, 1007, Mesa
);
AROS_LP2(void, glSetFragmentShaderConstantATI,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(const GLfloat *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1008, Mesa
);
AROS_LP2(void, glPointParameteriNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1009, Mesa
);
AROS_LP2(void, glPointParameterivNV,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1010, Mesa
);
AROS_LP2(void, glDrawBuffersATI,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLenum *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1011, Mesa
);
AROS_LP7(void, glProgramNamedParameter4fNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(GLfloat, d, D2),
         AROS_LPA(GLfloat, e, D3),
         AROS_LPA(GLfloat, f, D4),
         AROS_LPA(GLfloat, g, D5),
         LIBBASETYPEPTR, MesaBase, 1012, Mesa
);
AROS_LP7(void, glProgramNamedParameter4dNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(GLdouble, d, D2),
         AROS_LPA(GLdouble, e, D3),
         AROS_LPA(GLdouble, f, D4),
         AROS_LPA(GLdouble, g, D5),
         LIBBASETYPEPTR, MesaBase, 1013, Mesa
);
AROS_LP4(void, glProgramNamedParameter4fvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(const GLfloat *, d, A1),
         LIBBASETYPEPTR, MesaBase, 1014, Mesa
);
AROS_LP4(void, glProgramNamedParameter4dvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(const GLdouble *, d, A1),
         LIBBASETYPEPTR, MesaBase, 1015, Mesa
);
AROS_LP4(void, glGetProgramNamedParameterfvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(GLfloat *, d, A1),
         LIBBASETYPEPTR, MesaBase, 1016, Mesa
);
AROS_LP4(void, glGetProgramNamedParameterdvNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(const GLubyte *, c, A0),
         AROS_LPA(GLdouble *, d, A1),
         LIBBASETYPEPTR, MesaBase, 1017, Mesa
);
AROS_LP1(GLboolean, glIsRenderbufferEXT,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 1018, Mesa
);
AROS_LP2(void, glBindRenderbufferEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1019, Mesa
);
AROS_LP2(void, glDeleteRenderbuffersEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1020, Mesa
);
AROS_LP2(void, glGenRenderbuffersEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1021, Mesa
);
AROS_LP4(void, glRenderbufferStorageEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei, d, D3),
         LIBBASETYPEPTR, MesaBase, 1022, Mesa
);
AROS_LP3(void, glGetRenderbufferParameterivEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 1023, Mesa
);
AROS_LP1(GLboolean, glIsFramebufferEXT,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 1024, Mesa
);
AROS_LP2(void, glBindFramebufferEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1025, Mesa
);
AROS_LP2(void, glDeleteFramebuffersEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1026, Mesa
);
AROS_LP2(void, glGenFramebuffersEXT,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1027, Mesa
);
AROS_LP1(GLenum, glCheckFramebufferStatusEXT,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 1028, Mesa
);
AROS_LP5(void, glFramebufferTexture1DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 1029, Mesa
);
AROS_LP5(void, glFramebufferTexture2DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 1030, Mesa
);
AROS_LP6(void, glFramebufferTexture3DEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         AROS_LPA(GLint, e, D4),
         AROS_LPA(GLint, f, D5),
         LIBBASETYPEPTR, MesaBase, 1031, Mesa
);
AROS_LP4(void, glFramebufferRenderbufferEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLuint, d, D3),
         LIBBASETYPEPTR, MesaBase, 1032, Mesa
);
AROS_LP4(void, glGetFramebufferAttachmentParameterivEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint *, d, A0),
         LIBBASETYPEPTR, MesaBase, 1033, Mesa
);
AROS_LP1(void, glGenerateMipmapEXT,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 1034, Mesa
);
AROS_LP5(void, glFramebufferTextureLayerEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLint, d, D3),
         AROS_LPA(GLint, e, D4),
         LIBBASETYPEPTR, MesaBase, 1035, Mesa
);
AROS_LP4(GLvoid*, glMapBufferRange,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptr, b, D1),
         AROS_LPA(GLsizeiptr, c, D2),
         AROS_LPA(GLbitfield, d, D3),
         LIBBASETYPEPTR, MesaBase, 1036, Mesa
);
AROS_LP3(void, glFlushMappedBufferRange,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLintptr, b, D1),
         AROS_LPA(GLsizeiptr, c, D2),
         LIBBASETYPEPTR, MesaBase, 1037, Mesa
);
AROS_LP1(void, glBindVertexArray,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 1038, Mesa
);
AROS_LP2(void, glDeleteVertexArrays,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(const GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1039, Mesa
);
AROS_LP2(void, glGenVertexArrays,
         AROS_LPA(GLsizei, a, D0),
         AROS_LPA(GLuint *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1040, Mesa
);
AROS_LP1(GLboolean, glIsVertexArray,
         AROS_LPA(GLuint, a, D0),
         LIBBASETYPEPTR, MesaBase, 1041, Mesa
);
AROS_LP5(void, glCopyBufferSubData,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLintptr, c, D2),
         AROS_LPA(GLintptr, d, D3),
         AROS_LPA(GLsizeiptr, e, D4),
         LIBBASETYPEPTR, MesaBase, 1042, Mesa
);
AROS_LP2(GLsync, glFenceSync,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLbitfield, b, D1),
         LIBBASETYPEPTR, MesaBase, 1043, Mesa
);
AROS_LP1(GLboolean, glIsSync,
         AROS_LPA(GLsync, a, D0),
         LIBBASETYPEPTR, MesaBase, 1044, Mesa
);
AROS_LP1(void, glDeleteSync,
         AROS_LPA(GLsync, a, D0),
         LIBBASETYPEPTR, MesaBase, 1045, Mesa
);
AROS_LP3(GLenum, glClientWaitSync,
         AROS_LPA(GLsync, a, D0),
         AROS_LPA(GLbitfield, b, D1),
         AROS_LPA(GLuint64, c, D2),
         LIBBASETYPEPTR, MesaBase, 1046, Mesa
);
AROS_LP3(void, glWaitSync,
         AROS_LPA(GLsync, a, D0),
         AROS_LPA(GLbitfield, b, D1),
         AROS_LPA(GLuint64, c, D2),
         LIBBASETYPEPTR, MesaBase, 1047, Mesa
);
AROS_LP2(void, glGetInteger64v,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLint64 *, b, A0),
         LIBBASETYPEPTR, MesaBase, 1048, Mesa
);
AROS_LP5(void, glGetSynciv,
         AROS_LPA(GLsync, a, D0),
         AROS_LPA(GLenum, b, D1),
         AROS_LPA(GLsizei, c, D2),
         AROS_LPA(GLsizei *, d, A0),
         AROS_LPA(GLint *, e, A1),
         LIBBASETYPEPTR, MesaBase, 1049, Mesa
);
AROS_LP1(void, glProvokingVertexEXT,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 1050, Mesa
);
AROS_LP5(void, glDrawElementsBaseVertex,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(const GLvoid *, d, A0),
         AROS_LPA(GLint, e, D3),
         LIBBASETYPEPTR, MesaBase, 1051, Mesa
);
AROS_LP7(void, glDrawRangeElementsBaseVertex,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLuint, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLenum, e, D4),
         AROS_LPA(const GLvoid *, f, A0),
         AROS_LPA(GLint, g, D5),
         LIBBASETYPEPTR, MesaBase, 1052, Mesa
);
AROS_LP6(void, glMultiDrawElementsBaseVertex,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(const GLsizei *, b, A0),
         AROS_LPA(GLenum, c, D1),
         AROS_LPA(const GLvoid *  *, d, A1),
         AROS_LPA(GLsizei, e, D2),
         AROS_LPA(const GLint *, f, A2),
         LIBBASETYPEPTR, MesaBase, 1053, Mesa
);
AROS_LP1(void, glProvokingVertex,
         AROS_LPA(GLenum, a, D0),
         LIBBASETYPEPTR, MesaBase, 1054, Mesa
);
AROS_LP5(void, glRenderbufferStorageMultisampleEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLsizei, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLsizei, d, D3),
         AROS_LPA(GLsizei, e, D4),
         LIBBASETYPEPTR, MesaBase, 1055, Mesa
);
AROS_LP5(void, glColorMaskIndexedEXT,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLboolean, b, D1),
         AROS_LPA(GLboolean, c, D2),
         AROS_LPA(GLboolean, d, D3),
         AROS_LPA(GLboolean, e, D4),
         LIBBASETYPEPTR, MesaBase, 1056, Mesa
);
AROS_LP3(void, glGetBooleanIndexedvEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLboolean *, c, A0),
         LIBBASETYPEPTR, MesaBase, 1057, Mesa
);
AROS_LP3(void, glGetIntegerIndexedvEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLint *, c, A0),
         LIBBASETYPEPTR, MesaBase, 1058, Mesa
);
AROS_LP2(void, glEnableIndexedEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1059, Mesa
);
AROS_LP2(void, glDisableIndexedEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1060, Mesa
);
AROS_LP2(GLboolean, glIsEnabledIndexedEXT,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         LIBBASETYPEPTR, MesaBase, 1061, Mesa
);
AROS_LP2(void, glBeginConditionalRenderNV,
         AROS_LPA(GLuint, a, D0),
         AROS_LPA(GLenum, b, D1),
         LIBBASETYPEPTR, MesaBase, 1062, Mesa
);
AROS_LP0(void, glEndConditionalRenderNV,
         LIBBASETYPEPTR, MesaBase, 1063, Mesa
);
AROS_LP3(GLenum, glObjectPurgeableAPPLE,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         LIBBASETYPEPTR, MesaBase, 1064, Mesa
);
AROS_LP3(GLenum, glObjectUnpurgeableAPPLE,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         LIBBASETYPEPTR, MesaBase, 1065, Mesa
);
AROS_LP4(void, glGetObjectParameterivAPPLE,
         AROS_LPA(GLenum, a, D0),
         AROS_LPA(GLuint, b, D1),
         AROS_LPA(GLenum, c, D2),
         AROS_LPA(GLint *, d, A0),
         LIBBASETYPEPTR, MesaBase, 1066, Mesa
);

__END_DECLS

#endif /* CLIB_MESA_PROTOS_H */
