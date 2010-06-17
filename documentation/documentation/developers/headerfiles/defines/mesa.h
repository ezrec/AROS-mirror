#ifndef DEFINES_MESA_H
#define DEFINES_MESA_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/mesa/src/mesa/arosmesa.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for mesa
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS

#define AROSMesaCreateContextTags ((AROSMesaContext (*)(long Tag1, ...))__AROS_GETVECADDR(MesaBase,5))
#define AROSMesaCreateContext ((AROSMesaContext (*)(struct TagItem *tagList))__AROS_GETVECADDR(MesaBase,6))

#define __AROSMesaDestroyContext_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, AROSMesaDestroyContext, \
                  AROS_LCA(AROSMesaContext,(__arg1),A0), \
        struct Library *, (__MesaBase), 7, Mesa)

#define AROSMesaDestroyContext(arg1) \
    __AROSMesaDestroyContext_WB(MesaBase, (arg1))

#define __AROSMesaMakeCurrent_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, AROSMesaMakeCurrent, \
                  AROS_LCA(AROSMesaContext,(__arg1),A0), \
        struct Library *, (__MesaBase), 8, Mesa)

#define AROSMesaMakeCurrent(arg1) \
    __AROSMesaMakeCurrent_WB(MesaBase, (arg1))

#define __AROSMesaSwapBuffers_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, AROSMesaSwapBuffers, \
                  AROS_LCA(AROSMesaContext,(__arg1),A0), \
        struct Library *, (__MesaBase), 9, Mesa)

#define AROSMesaSwapBuffers(arg1) \
    __AROSMesaSwapBuffers_WB(MesaBase, (arg1))

#define __AROSMesaGetCurrentContext_WB(__MesaBase) \
        AROS_LC0(AROSMesaContext, AROSMesaGetCurrentContext, \
        struct Library *, (__MesaBase), 10, Mesa)

#define AROSMesaGetCurrentContext() \
    __AROSMesaGetCurrentContext_WB(MesaBase)

#define __glClearIndex_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClearIndex, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 35, Mesa)

#define glClearIndex(arg1) \
    __glClearIndex_WB(MesaBase, (arg1))

#define __glClearColor_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glClearColor, \
                  AROS_LCA(GLclampf,(__arg1),D0), \
                  AROS_LCA(GLclampf,(__arg2),D1), \
                  AROS_LCA(GLclampf,(__arg3),D2), \
                  AROS_LCA(GLclampf,(__arg4),D3), \
        struct Library *, (__MesaBase), 36, Mesa)

#define glClearColor(arg1, arg2, arg3, arg4) \
    __glClearColor_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glClear_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClear, \
                  AROS_LCA(GLbitfield,(__arg1),D0), \
        struct Library *, (__MesaBase), 37, Mesa)

#define glClear(arg1) \
    __glClear_WB(MesaBase, (arg1))

#define __glIndexMask_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexMask, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 38, Mesa)

#define glIndexMask(arg1) \
    __glIndexMask_WB(MesaBase, (arg1))

#define __glColorMask_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColorMask, \
                  AROS_LCA(GLboolean,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(GLboolean,(__arg4),D3), \
        struct Library *, (__MesaBase), 39, Mesa)

#define glColorMask(arg1, arg2, arg3, arg4) \
    __glColorMask_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glAlphaFunc_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glAlphaFunc, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLclampf,(__arg2),D1), \
        struct Library *, (__MesaBase), 40, Mesa)

#define glAlphaFunc(arg1, arg2) \
    __glAlphaFunc_WB(MesaBase, (arg1), (arg2))

#define __glBlendFunc_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBlendFunc, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 41, Mesa)

#define glBlendFunc(arg1, arg2) \
    __glBlendFunc_WB(MesaBase, (arg1), (arg2))

#define __glLogicOp_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLogicOp, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 42, Mesa)

#define glLogicOp(arg1) \
    __glLogicOp_WB(MesaBase, (arg1))

#define __glCullFace_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glCullFace, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 43, Mesa)

#define glCullFace(arg1) \
    __glCullFace_WB(MesaBase, (arg1))

#define __glFrontFace_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFrontFace, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 44, Mesa)

#define glFrontFace(arg1) \
    __glFrontFace_WB(MesaBase, (arg1))

#define __glPointSize_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPointSize, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 45, Mesa)

#define glPointSize(arg1) \
    __glPointSize_WB(MesaBase, (arg1))

#define __glLineWidth_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLineWidth, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 46, Mesa)

#define glLineWidth(arg1) \
    __glLineWidth_WB(MesaBase, (arg1))

#define __glLineStipple_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLineStipple, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLushort,(__arg2),D1), \
        struct Library *, (__MesaBase), 47, Mesa)

#define glLineStipple(arg1, arg2) \
    __glLineStipple_WB(MesaBase, (arg1), (arg2))

#define __glPolygonMode_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPolygonMode, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 48, Mesa)

#define glPolygonMode(arg1, arg2) \
    __glPolygonMode_WB(MesaBase, (arg1), (arg2))

#define __glPolygonOffset_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPolygonOffset, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 49, Mesa)

#define glPolygonOffset(arg1, arg2) \
    __glPolygonOffset_WB(MesaBase, (arg1), (arg2))

#define __glPolygonStipple_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPolygonStipple, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 50, Mesa)

#define glPolygonStipple(arg1) \
    __glPolygonStipple_WB(MesaBase, (arg1))

#define __glGetPolygonStipple_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glGetPolygonStipple, \
                  AROS_LCA(GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 51, Mesa)

#define glGetPolygonStipple(arg1) \
    __glGetPolygonStipple_WB(MesaBase, (arg1))

#define __glEdgeFlag_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEdgeFlag, \
                  AROS_LCA(GLboolean,(__arg1),D0), \
        struct Library *, (__MesaBase), 52, Mesa)

#define glEdgeFlag(arg1) \
    __glEdgeFlag_WB(MesaBase, (arg1))

#define __glEdgeFlagv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEdgeFlagv, \
                  AROS_LCA(const GLboolean *,(__arg1),A0), \
        struct Library *, (__MesaBase), 53, Mesa)

#define glEdgeFlagv(arg1) \
    __glEdgeFlagv_WB(MesaBase, (arg1))

#define __glScissor_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glScissor, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
        struct Library *, (__MesaBase), 54, Mesa)

#define glScissor(arg1, arg2, arg3, arg4) \
    __glScissor_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glClipPlane_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glClipPlane, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 55, Mesa)

#define glClipPlane(arg1, arg2) \
    __glClipPlane_WB(MesaBase, (arg1), (arg2))

#define __glGetClipPlane_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetClipPlane, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 56, Mesa)

#define glGetClipPlane(arg1, arg2) \
    __glGetClipPlane_WB(MesaBase, (arg1), (arg2))

#define __glDrawBuffer_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDrawBuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 57, Mesa)

#define glDrawBuffer(arg1) \
    __glDrawBuffer_WB(MesaBase, (arg1))

#define __glReadBuffer_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glReadBuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 58, Mesa)

#define glReadBuffer(arg1) \
    __glReadBuffer_WB(MesaBase, (arg1))

#define __glEnable_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEnable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 59, Mesa)

#define glEnable(arg1) \
    __glEnable_WB(MesaBase, (arg1))

#define __glDisable_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDisable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 60, Mesa)

#define glDisable(arg1) \
    __glDisable_WB(MesaBase, (arg1))

#define __glIsEnabled_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsEnabled, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 61, Mesa)

#define glIsEnabled(arg1) \
    __glIsEnabled_WB(MesaBase, (arg1))

#define __glEnableClientState_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEnableClientState, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 62, Mesa)

#define glEnableClientState(arg1) \
    __glEnableClientState_WB(MesaBase, (arg1))

#define __glDisableClientState_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDisableClientState, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 63, Mesa)

#define glDisableClientState(arg1) \
    __glDisableClientState_WB(MesaBase, (arg1))

#define __glGetBooleanv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetBooleanv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLboolean *,(__arg2),A0), \
        struct Library *, (__MesaBase), 64, Mesa)

#define glGetBooleanv(arg1, arg2) \
    __glGetBooleanv_WB(MesaBase, (arg1), (arg2))

#define __glGetDoublev_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetDoublev, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 65, Mesa)

#define glGetDoublev(arg1, arg2) \
    __glGetDoublev_WB(MesaBase, (arg1), (arg2))

#define __glGetFloatv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetFloatv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 66, Mesa)

#define glGetFloatv(arg1, arg2) \
    __glGetFloatv_WB(MesaBase, (arg1), (arg2))

#define __glGetIntegerv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetIntegerv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 67, Mesa)

#define glGetIntegerv(arg1, arg2) \
    __glGetIntegerv_WB(MesaBase, (arg1), (arg2))

#define __glPushAttrib_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPushAttrib, \
                  AROS_LCA(GLbitfield,(__arg1),D0), \
        struct Library *, (__MesaBase), 68, Mesa)

#define glPushAttrib(arg1) \
    __glPushAttrib_WB(MesaBase, (arg1))

#define __glPopAttrib_WB(__MesaBase) \
        AROS_LC0NR(void, glPopAttrib, \
        struct Library *, (__MesaBase), 69, Mesa)

#define glPopAttrib() \
    __glPopAttrib_WB(MesaBase)

#define __glPushClientAttrib_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPushClientAttrib, \
                  AROS_LCA(GLbitfield,(__arg1),D0), \
        struct Library *, (__MesaBase), 70, Mesa)

#define glPushClientAttrib(arg1) \
    __glPushClientAttrib_WB(MesaBase, (arg1))

#define __glPopClientAttrib_WB(__MesaBase) \
        AROS_LC0NR(void, glPopClientAttrib, \
        struct Library *, (__MesaBase), 71, Mesa)

#define glPopClientAttrib() \
    __glPopClientAttrib_WB(MesaBase)

#define __glRenderMode_WB(__MesaBase, __arg1) \
        AROS_LC1(GLint, glRenderMode, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 72, Mesa)

#define glRenderMode(arg1) \
    __glRenderMode_WB(MesaBase, (arg1))

#define __glGetError_WB(__MesaBase) \
        AROS_LC0(GLenum, glGetError, \
        struct Library *, (__MesaBase), 73, Mesa)

#define glGetError() \
    __glGetError_WB(MesaBase)

#define __glGetString_WB(__MesaBase, __arg1) \
        AROS_LC1(const GLubyte *, glGetString, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 74, Mesa)

#define glGetString(arg1) \
    __glGetString_WB(MesaBase, (arg1))

#define __glFinish_WB(__MesaBase) \
        AROS_LC0NR(void, glFinish, \
        struct Library *, (__MesaBase), 75, Mesa)

#define glFinish() \
    __glFinish_WB(MesaBase)

#define __glFlush_WB(__MesaBase) \
        AROS_LC0NR(void, glFlush, \
        struct Library *, (__MesaBase), 76, Mesa)

#define glFlush() \
    __glFlush_WB(MesaBase)

#define __glHint_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glHint, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 77, Mesa)

#define glHint(arg1, arg2) \
    __glHint_WB(MesaBase, (arg1), (arg2))

#define __glClearDepth_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClearDepth, \
                  AROS_LCA(GLclampd,(__arg1),D0), \
        struct Library *, (__MesaBase), 78, Mesa)

#define glClearDepth(arg1) \
    __glClearDepth_WB(MesaBase, (arg1))

#define __glDepthFunc_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDepthFunc, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 79, Mesa)

#define glDepthFunc(arg1) \
    __glDepthFunc_WB(MesaBase, (arg1))

#define __glDepthMask_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDepthMask, \
                  AROS_LCA(GLboolean,(__arg1),D0), \
        struct Library *, (__MesaBase), 80, Mesa)

#define glDepthMask(arg1) \
    __glDepthMask_WB(MesaBase, (arg1))

#define __glDepthRange_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDepthRange, \
                  AROS_LCA(GLclampd,(__arg1),D0), \
                  AROS_LCA(GLclampd,(__arg2),D1), \
        struct Library *, (__MesaBase), 81, Mesa)

#define glDepthRange(arg1, arg2) \
    __glDepthRange_WB(MesaBase, (arg1), (arg2))

#define __glClearAccum_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glClearAccum, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 82, Mesa)

#define glClearAccum(arg1, arg2, arg3, arg4) \
    __glClearAccum_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glAccum_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glAccum, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 83, Mesa)

#define glAccum(arg1, arg2) \
    __glAccum_WB(MesaBase, (arg1), (arg2))

#define __glMatrixMode_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMatrixMode, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 84, Mesa)

#define glMatrixMode(arg1) \
    __glMatrixMode_WB(MesaBase, (arg1))

#define __glOrtho_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glOrtho, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 85, Mesa)

#define glOrtho(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glOrtho_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glFrustum_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glFrustum, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 86, Mesa)

#define glFrustum(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glFrustum_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glViewport_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glViewport, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
        struct Library *, (__MesaBase), 87, Mesa)

#define glViewport(arg1, arg2, arg3, arg4) \
    __glViewport_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glPushMatrix_WB(__MesaBase) \
        AROS_LC0NR(void, glPushMatrix, \
        struct Library *, (__MesaBase), 88, Mesa)

#define glPushMatrix() \
    __glPushMatrix_WB(MesaBase)

#define __glPopMatrix_WB(__MesaBase) \
        AROS_LC0NR(void, glPopMatrix, \
        struct Library *, (__MesaBase), 89, Mesa)

#define glPopMatrix() \
    __glPopMatrix_WB(MesaBase)

#define __glLoadIdentity_WB(__MesaBase) \
        AROS_LC0NR(void, glLoadIdentity, \
        struct Library *, (__MesaBase), 90, Mesa)

#define glLoadIdentity() \
    __glLoadIdentity_WB(MesaBase)

#define __glLoadMatrixd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadMatrixd, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 91, Mesa)

#define glLoadMatrixd(arg1) \
    __glLoadMatrixd_WB(MesaBase, (arg1))

#define __glLoadMatrixf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadMatrixf, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 92, Mesa)

#define glLoadMatrixf(arg1) \
    __glLoadMatrixf_WB(MesaBase, (arg1))

#define __glMultMatrixd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultMatrixd, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 93, Mesa)

#define glMultMatrixd(arg1) \
    __glMultMatrixd_WB(MesaBase, (arg1))

#define __glMultMatrixf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultMatrixf, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 94, Mesa)

#define glMultMatrixf(arg1) \
    __glMultMatrixf_WB(MesaBase, (arg1))

#define __glRotated_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRotated, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 95, Mesa)

#define glRotated(arg1, arg2, arg3, arg4) \
    __glRotated_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRotatef_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRotatef, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 96, Mesa)

#define glRotatef(arg1, arg2, arg3, arg4) \
    __glRotatef_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glScaled_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glScaled, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 97, Mesa)

#define glScaled(arg1, arg2, arg3) \
    __glScaled_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glScalef_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glScalef, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 98, Mesa)

#define glScalef(arg1, arg2, arg3) \
    __glScalef_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTranslated_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTranslated, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 99, Mesa)

#define glTranslated(arg1, arg2, arg3) \
    __glTranslated_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTranslatef_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTranslatef, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 100, Mesa)

#define glTranslatef(arg1, arg2, arg3) \
    __glTranslatef_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsList_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsList, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 101, Mesa)

#define glIsList(arg1) \
    __glIsList_WB(MesaBase, (arg1))

#define __glDeleteLists_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteLists, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
        struct Library *, (__MesaBase), 102, Mesa)

#define glDeleteLists(arg1, arg2) \
    __glDeleteLists_WB(MesaBase, (arg1), (arg2))

#define __glGenLists_WB(__MesaBase, __arg1) \
        AROS_LC1(GLuint, glGenLists, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
        struct Library *, (__MesaBase), 103, Mesa)

#define glGenLists(arg1) \
    __glGenLists_WB(MesaBase, (arg1))

#define __glNewList_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glNewList, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 104, Mesa)

#define glNewList(arg1, arg2) \
    __glNewList_WB(MesaBase, (arg1), (arg2))

#define __glEndList_WB(__MesaBase) \
        AROS_LC0NR(void, glEndList, \
        struct Library *, (__MesaBase), 105, Mesa)

#define glEndList() \
    __glEndList_WB(MesaBase)

#define __glCallList_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glCallList, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 106, Mesa)

#define glCallList(arg1) \
    __glCallList_WB(MesaBase, (arg1))

#define __glCallLists_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glCallLists, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 107, Mesa)

#define glCallLists(arg1, arg2, arg3) \
    __glCallLists_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glListBase_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glListBase, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 108, Mesa)

#define glListBase(arg1) \
    __glListBase_WB(MesaBase, (arg1))

#define __glBegin_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glBegin, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 109, Mesa)

#define glBegin(arg1) \
    __glBegin_WB(MesaBase, (arg1))

#define __glEnd_WB(__MesaBase) \
        AROS_LC0NR(void, glEnd, \
        struct Library *, (__MesaBase), 110, Mesa)

#define glEnd() \
    __glEnd_WB(MesaBase)

#define __glVertex2d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertex2d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 111, Mesa)

#define glVertex2d(arg1, arg2) \
    __glVertex2d_WB(MesaBase, (arg1), (arg2))

#define __glVertex2f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertex2f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 112, Mesa)

#define glVertex2f(arg1, arg2) \
    __glVertex2f_WB(MesaBase, (arg1), (arg2))

#define __glVertex2i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertex2i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 113, Mesa)

#define glVertex2i(arg1, arg2) \
    __glVertex2i_WB(MesaBase, (arg1), (arg2))

#define __glVertex2s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertex2s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 114, Mesa)

#define glVertex2s(arg1, arg2) \
    __glVertex2s_WB(MesaBase, (arg1), (arg2))

#define __glVertex3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertex3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 115, Mesa)

#define glVertex3d(arg1, arg2, arg3) \
    __glVertex3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertex3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertex3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 116, Mesa)

#define glVertex3f(arg1, arg2, arg3) \
    __glVertex3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertex3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertex3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 117, Mesa)

#define glVertex3i(arg1, arg2, arg3) \
    __glVertex3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertex3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertex3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 118, Mesa)

#define glVertex3s(arg1, arg2, arg3) \
    __glVertex3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertex4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertex4d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 119, Mesa)

#define glVertex4d(arg1, arg2, arg3, arg4) \
    __glVertex4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertex4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertex4f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 120, Mesa)

#define glVertex4f(arg1, arg2, arg3, arg4) \
    __glVertex4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertex4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertex4i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 121, Mesa)

#define glVertex4i(arg1, arg2, arg3, arg4) \
    __glVertex4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertex4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertex4s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 122, Mesa)

#define glVertex4s(arg1, arg2, arg3, arg4) \
    __glVertex4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertex2dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex2dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 123, Mesa)

#define glVertex2dv(arg1) \
    __glVertex2dv_WB(MesaBase, (arg1))

#define __glVertex2fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex2fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 124, Mesa)

#define glVertex2fv(arg1) \
    __glVertex2fv_WB(MesaBase, (arg1))

#define __glVertex2iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex2iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 125, Mesa)

#define glVertex2iv(arg1) \
    __glVertex2iv_WB(MesaBase, (arg1))

#define __glVertex2sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex2sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 126, Mesa)

#define glVertex2sv(arg1) \
    __glVertex2sv_WB(MesaBase, (arg1))

#define __glVertex3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 127, Mesa)

#define glVertex3dv(arg1) \
    __glVertex3dv_WB(MesaBase, (arg1))

#define __glVertex3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 128, Mesa)

#define glVertex3fv(arg1) \
    __glVertex3fv_WB(MesaBase, (arg1))

#define __glVertex3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 129, Mesa)

#define glVertex3iv(arg1) \
    __glVertex3iv_WB(MesaBase, (arg1))

#define __glVertex3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 130, Mesa)

#define glVertex3sv(arg1) \
    __glVertex3sv_WB(MesaBase, (arg1))

#define __glVertex4dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex4dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 131, Mesa)

#define glVertex4dv(arg1) \
    __glVertex4dv_WB(MesaBase, (arg1))

#define __glVertex4fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex4fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 132, Mesa)

#define glVertex4fv(arg1) \
    __glVertex4fv_WB(MesaBase, (arg1))

#define __glVertex4iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex4iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 133, Mesa)

#define glVertex4iv(arg1) \
    __glVertex4iv_WB(MesaBase, (arg1))

#define __glVertex4sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glVertex4sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 134, Mesa)

#define glVertex4sv(arg1) \
    __glVertex4sv_WB(MesaBase, (arg1))

#define __glNormal3b_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormal3b, \
                  AROS_LCA(GLbyte,(__arg1),D0), \
                  AROS_LCA(GLbyte,(__arg2),D1), \
                  AROS_LCA(GLbyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 135, Mesa)

#define glNormal3b(arg1, arg2, arg3) \
    __glNormal3b_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glNormal3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormal3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 136, Mesa)

#define glNormal3d(arg1, arg2, arg3) \
    __glNormal3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glNormal3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormal3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 137, Mesa)

#define glNormal3f(arg1, arg2, arg3) \
    __glNormal3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glNormal3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormal3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 138, Mesa)

#define glNormal3i(arg1, arg2, arg3) \
    __glNormal3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glNormal3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormal3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 139, Mesa)

#define glNormal3s(arg1, arg2, arg3) \
    __glNormal3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glNormal3bv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glNormal3bv, \
                  AROS_LCA(const GLbyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 140, Mesa)

#define glNormal3bv(arg1) \
    __glNormal3bv_WB(MesaBase, (arg1))

#define __glNormal3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glNormal3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 141, Mesa)

#define glNormal3dv(arg1) \
    __glNormal3dv_WB(MesaBase, (arg1))

#define __glNormal3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glNormal3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 142, Mesa)

#define glNormal3fv(arg1) \
    __glNormal3fv_WB(MesaBase, (arg1))

#define __glNormal3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glNormal3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 143, Mesa)

#define glNormal3iv(arg1) \
    __glNormal3iv_WB(MesaBase, (arg1))

#define __glNormal3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glNormal3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 144, Mesa)

#define glNormal3sv(arg1) \
    __glNormal3sv_WB(MesaBase, (arg1))

#define __glIndexd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexd, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
        struct Library *, (__MesaBase), 145, Mesa)

#define glIndexd(arg1) \
    __glIndexd_WB(MesaBase, (arg1))

#define __glIndexf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexf, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 146, Mesa)

#define glIndexf(arg1) \
    __glIndexf_WB(MesaBase, (arg1))

#define __glIndexi_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexi, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 147, Mesa)

#define glIndexi(arg1) \
    __glIndexi_WB(MesaBase, (arg1))

#define __glIndexs_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexs, \
                  AROS_LCA(GLshort,(__arg1),D0), \
        struct Library *, (__MesaBase), 148, Mesa)

#define glIndexs(arg1) \
    __glIndexs_WB(MesaBase, (arg1))

#define __glIndexub_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexub, \
                  AROS_LCA(GLubyte,(__arg1),D0), \
        struct Library *, (__MesaBase), 149, Mesa)

#define glIndexub(arg1) \
    __glIndexub_WB(MesaBase, (arg1))

#define __glIndexdv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexdv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 150, Mesa)

#define glIndexdv(arg1) \
    __glIndexdv_WB(MesaBase, (arg1))

#define __glIndexfv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexfv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 151, Mesa)

#define glIndexfv(arg1) \
    __glIndexfv_WB(MesaBase, (arg1))

#define __glIndexiv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexiv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 152, Mesa)

#define glIndexiv(arg1) \
    __glIndexiv_WB(MesaBase, (arg1))

#define __glIndexsv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexsv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 153, Mesa)

#define glIndexsv(arg1) \
    __glIndexsv_WB(MesaBase, (arg1))

#define __glIndexubv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glIndexubv, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 154, Mesa)

#define glIndexubv(arg1) \
    __glIndexubv_WB(MesaBase, (arg1))

#define __glColor3b_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3b, \
                  AROS_LCA(GLbyte,(__arg1),D0), \
                  AROS_LCA(GLbyte,(__arg2),D1), \
                  AROS_LCA(GLbyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 155, Mesa)

#define glColor3b(arg1, arg2, arg3) \
    __glColor3b_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 156, Mesa)

#define glColor3d(arg1, arg2, arg3) \
    __glColor3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 157, Mesa)

#define glColor3f(arg1, arg2, arg3) \
    __glColor3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 158, Mesa)

#define glColor3i(arg1, arg2, arg3) \
    __glColor3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 159, Mesa)

#define glColor3s(arg1, arg2, arg3) \
    __glColor3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3ub_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3ub, \
                  AROS_LCA(GLubyte,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 160, Mesa)

#define glColor3ub(arg1, arg2, arg3) \
    __glColor3ub_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3ui_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3ui, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
        struct Library *, (__MesaBase), 161, Mesa)

#define glColor3ui(arg1, arg2, arg3) \
    __glColor3ui_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor3us_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColor3us, \
                  AROS_LCA(GLushort,(__arg1),D0), \
                  AROS_LCA(GLushort,(__arg2),D1), \
                  AROS_LCA(GLushort,(__arg3),D2), \
        struct Library *, (__MesaBase), 162, Mesa)

#define glColor3us(arg1, arg2, arg3) \
    __glColor3us_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColor4b_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4b, \
                  AROS_LCA(GLbyte,(__arg1),D0), \
                  AROS_LCA(GLbyte,(__arg2),D1), \
                  AROS_LCA(GLbyte,(__arg3),D2), \
                  AROS_LCA(GLbyte,(__arg4),D3), \
        struct Library *, (__MesaBase), 163, Mesa)

#define glColor4b(arg1, arg2, arg3, arg4) \
    __glColor4b_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 164, Mesa)

#define glColor4d(arg1, arg2, arg3, arg4) \
    __glColor4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 165, Mesa)

#define glColor4f(arg1, arg2, arg3, arg4) \
    __glColor4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 166, Mesa)

#define glColor4i(arg1, arg2, arg3, arg4) \
    __glColor4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 167, Mesa)

#define glColor4s(arg1, arg2, arg3, arg4) \
    __glColor4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4ub_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4ub, \
                  AROS_LCA(GLubyte,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
                  AROS_LCA(GLubyte,(__arg4),D3), \
        struct Library *, (__MesaBase), 168, Mesa)

#define glColor4ub(arg1, arg2, arg3, arg4) \
    __glColor4ub_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4ui_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4ui, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
        struct Library *, (__MesaBase), 169, Mesa)

#define glColor4ui(arg1, arg2, arg3, arg4) \
    __glColor4ui_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor4us_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColor4us, \
                  AROS_LCA(GLushort,(__arg1),D0), \
                  AROS_LCA(GLushort,(__arg2),D1), \
                  AROS_LCA(GLushort,(__arg3),D2), \
                  AROS_LCA(GLushort,(__arg4),D3), \
        struct Library *, (__MesaBase), 170, Mesa)

#define glColor4us(arg1, arg2, arg3, arg4) \
    __glColor4us_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glColor3bv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3bv, \
                  AROS_LCA(const GLbyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 171, Mesa)

#define glColor3bv(arg1) \
    __glColor3bv_WB(MesaBase, (arg1))

#define __glColor3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 172, Mesa)

#define glColor3dv(arg1) \
    __glColor3dv_WB(MesaBase, (arg1))

#define __glColor3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 173, Mesa)

#define glColor3fv(arg1) \
    __glColor3fv_WB(MesaBase, (arg1))

#define __glColor3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 174, Mesa)

#define glColor3iv(arg1) \
    __glColor3iv_WB(MesaBase, (arg1))

#define __glColor3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 175, Mesa)

#define glColor3sv(arg1) \
    __glColor3sv_WB(MesaBase, (arg1))

#define __glColor3ubv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3ubv, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 176, Mesa)

#define glColor3ubv(arg1) \
    __glColor3ubv_WB(MesaBase, (arg1))

#define __glColor3uiv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3uiv, \
                  AROS_LCA(const GLuint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 177, Mesa)

#define glColor3uiv(arg1) \
    __glColor3uiv_WB(MesaBase, (arg1))

#define __glColor3usv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor3usv, \
                  AROS_LCA(const GLushort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 178, Mesa)

#define glColor3usv(arg1) \
    __glColor3usv_WB(MesaBase, (arg1))

#define __glColor4bv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4bv, \
                  AROS_LCA(const GLbyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 179, Mesa)

#define glColor4bv(arg1) \
    __glColor4bv_WB(MesaBase, (arg1))

#define __glColor4dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 180, Mesa)

#define glColor4dv(arg1) \
    __glColor4dv_WB(MesaBase, (arg1))

#define __glColor4fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 181, Mesa)

#define glColor4fv(arg1) \
    __glColor4fv_WB(MesaBase, (arg1))

#define __glColor4iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 182, Mesa)

#define glColor4iv(arg1) \
    __glColor4iv_WB(MesaBase, (arg1))

#define __glColor4sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 183, Mesa)

#define glColor4sv(arg1) \
    __glColor4sv_WB(MesaBase, (arg1))

#define __glColor4ubv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4ubv, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 184, Mesa)

#define glColor4ubv(arg1) \
    __glColor4ubv_WB(MesaBase, (arg1))

#define __glColor4uiv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4uiv, \
                  AROS_LCA(const GLuint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 185, Mesa)

#define glColor4uiv(arg1) \
    __glColor4uiv_WB(MesaBase, (arg1))

#define __glColor4usv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glColor4usv, \
                  AROS_LCA(const GLushort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 186, Mesa)

#define glColor4usv(arg1) \
    __glColor4usv_WB(MesaBase, (arg1))

#define __glTexCoord1d_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
        struct Library *, (__MesaBase), 187, Mesa)

#define glTexCoord1d(arg1) \
    __glTexCoord1d_WB(MesaBase, (arg1))

#define __glTexCoord1f_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 188, Mesa)

#define glTexCoord1f(arg1) \
    __glTexCoord1f_WB(MesaBase, (arg1))

#define __glTexCoord1i_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1i, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 189, Mesa)

#define glTexCoord1i(arg1) \
    __glTexCoord1i_WB(MesaBase, (arg1))

#define __glTexCoord1s_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
        struct Library *, (__MesaBase), 190, Mesa)

#define glTexCoord1s(arg1) \
    __glTexCoord1s_WB(MesaBase, (arg1))

#define __glTexCoord2d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexCoord2d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 191, Mesa)

#define glTexCoord2d(arg1, arg2) \
    __glTexCoord2d_WB(MesaBase, (arg1), (arg2))

#define __glTexCoord2f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexCoord2f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 192, Mesa)

#define glTexCoord2f(arg1, arg2) \
    __glTexCoord2f_WB(MesaBase, (arg1), (arg2))

#define __glTexCoord2i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexCoord2i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 193, Mesa)

#define glTexCoord2i(arg1, arg2) \
    __glTexCoord2i_WB(MesaBase, (arg1), (arg2))

#define __glTexCoord2s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexCoord2s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 194, Mesa)

#define glTexCoord2s(arg1, arg2) \
    __glTexCoord2s_WB(MesaBase, (arg1), (arg2))

#define __glTexCoord3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexCoord3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 195, Mesa)

#define glTexCoord3d(arg1, arg2, arg3) \
    __glTexCoord3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexCoord3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexCoord3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 196, Mesa)

#define glTexCoord3f(arg1, arg2, arg3) \
    __glTexCoord3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexCoord3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexCoord3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 197, Mesa)

#define glTexCoord3i(arg1, arg2, arg3) \
    __glTexCoord3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexCoord3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexCoord3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 198, Mesa)

#define glTexCoord3s(arg1, arg2, arg3) \
    __glTexCoord3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexCoord4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTexCoord4d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 199, Mesa)

#define glTexCoord4d(arg1, arg2, arg3, arg4) \
    __glTexCoord4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexCoord4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTexCoord4f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 200, Mesa)

#define glTexCoord4f(arg1, arg2, arg3, arg4) \
    __glTexCoord4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexCoord4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTexCoord4i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 201, Mesa)

#define glTexCoord4i(arg1, arg2, arg3, arg4) \
    __glTexCoord4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexCoord4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTexCoord4s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 202, Mesa)

#define glTexCoord4s(arg1, arg2, arg3, arg4) \
    __glTexCoord4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexCoord1dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 203, Mesa)

#define glTexCoord1dv(arg1) \
    __glTexCoord1dv_WB(MesaBase, (arg1))

#define __glTexCoord1fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 204, Mesa)

#define glTexCoord1fv(arg1) \
    __glTexCoord1fv_WB(MesaBase, (arg1))

#define __glTexCoord1iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 205, Mesa)

#define glTexCoord1iv(arg1) \
    __glTexCoord1iv_WB(MesaBase, (arg1))

#define __glTexCoord1sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord1sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 206, Mesa)

#define glTexCoord1sv(arg1) \
    __glTexCoord1sv_WB(MesaBase, (arg1))

#define __glTexCoord2dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord2dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 207, Mesa)

#define glTexCoord2dv(arg1) \
    __glTexCoord2dv_WB(MesaBase, (arg1))

#define __glTexCoord2fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord2fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 208, Mesa)

#define glTexCoord2fv(arg1) \
    __glTexCoord2fv_WB(MesaBase, (arg1))

#define __glTexCoord2iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord2iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 209, Mesa)

#define glTexCoord2iv(arg1) \
    __glTexCoord2iv_WB(MesaBase, (arg1))

#define __glTexCoord2sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord2sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 210, Mesa)

#define glTexCoord2sv(arg1) \
    __glTexCoord2sv_WB(MesaBase, (arg1))

#define __glTexCoord3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 211, Mesa)

#define glTexCoord3dv(arg1) \
    __glTexCoord3dv_WB(MesaBase, (arg1))

#define __glTexCoord3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 212, Mesa)

#define glTexCoord3fv(arg1) \
    __glTexCoord3fv_WB(MesaBase, (arg1))

#define __glTexCoord3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 213, Mesa)

#define glTexCoord3iv(arg1) \
    __glTexCoord3iv_WB(MesaBase, (arg1))

#define __glTexCoord3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 214, Mesa)

#define glTexCoord3sv(arg1) \
    __glTexCoord3sv_WB(MesaBase, (arg1))

#define __glTexCoord4dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord4dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 215, Mesa)

#define glTexCoord4dv(arg1) \
    __glTexCoord4dv_WB(MesaBase, (arg1))

#define __glTexCoord4fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord4fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 216, Mesa)

#define glTexCoord4fv(arg1) \
    __glTexCoord4fv_WB(MesaBase, (arg1))

#define __glTexCoord4iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord4iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 217, Mesa)

#define glTexCoord4iv(arg1) \
    __glTexCoord4iv_WB(MesaBase, (arg1))

#define __glTexCoord4sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glTexCoord4sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 218, Mesa)

#define glTexCoord4sv(arg1) \
    __glTexCoord4sv_WB(MesaBase, (arg1))

#define __glRasterPos2d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRasterPos2d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 219, Mesa)

#define glRasterPos2d(arg1, arg2) \
    __glRasterPos2d_WB(MesaBase, (arg1), (arg2))

#define __glRasterPos2f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRasterPos2f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 220, Mesa)

#define glRasterPos2f(arg1, arg2) \
    __glRasterPos2f_WB(MesaBase, (arg1), (arg2))

#define __glRasterPos2i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRasterPos2i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 221, Mesa)

#define glRasterPos2i(arg1, arg2) \
    __glRasterPos2i_WB(MesaBase, (arg1), (arg2))

#define __glRasterPos2s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRasterPos2s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 222, Mesa)

#define glRasterPos2s(arg1, arg2) \
    __glRasterPos2s_WB(MesaBase, (arg1), (arg2))

#define __glRasterPos3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glRasterPos3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 223, Mesa)

#define glRasterPos3d(arg1, arg2, arg3) \
    __glRasterPos3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glRasterPos3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glRasterPos3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 224, Mesa)

#define glRasterPos3f(arg1, arg2, arg3) \
    __glRasterPos3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glRasterPos3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glRasterPos3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 225, Mesa)

#define glRasterPos3i(arg1, arg2, arg3) \
    __glRasterPos3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glRasterPos3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glRasterPos3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 226, Mesa)

#define glRasterPos3s(arg1, arg2, arg3) \
    __glRasterPos3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glRasterPos4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRasterPos4d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 227, Mesa)

#define glRasterPos4d(arg1, arg2, arg3, arg4) \
    __glRasterPos4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRasterPos4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRasterPos4f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 228, Mesa)

#define glRasterPos4f(arg1, arg2, arg3, arg4) \
    __glRasterPos4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRasterPos4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRasterPos4i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 229, Mesa)

#define glRasterPos4i(arg1, arg2, arg3, arg4) \
    __glRasterPos4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRasterPos4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRasterPos4s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 230, Mesa)

#define glRasterPos4s(arg1, arg2, arg3, arg4) \
    __glRasterPos4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRasterPos2dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos2dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 231, Mesa)

#define glRasterPos2dv(arg1) \
    __glRasterPos2dv_WB(MesaBase, (arg1))

#define __glRasterPos2fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos2fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 232, Mesa)

#define glRasterPos2fv(arg1) \
    __glRasterPos2fv_WB(MesaBase, (arg1))

#define __glRasterPos2iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos2iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 233, Mesa)

#define glRasterPos2iv(arg1) \
    __glRasterPos2iv_WB(MesaBase, (arg1))

#define __glRasterPos2sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos2sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 234, Mesa)

#define glRasterPos2sv(arg1) \
    __glRasterPos2sv_WB(MesaBase, (arg1))

#define __glRasterPos3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 235, Mesa)

#define glRasterPos3dv(arg1) \
    __glRasterPos3dv_WB(MesaBase, (arg1))

#define __glRasterPos3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 236, Mesa)

#define glRasterPos3fv(arg1) \
    __glRasterPos3fv_WB(MesaBase, (arg1))

#define __glRasterPos3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 237, Mesa)

#define glRasterPos3iv(arg1) \
    __glRasterPos3iv_WB(MesaBase, (arg1))

#define __glRasterPos3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 238, Mesa)

#define glRasterPos3sv(arg1) \
    __glRasterPos3sv_WB(MesaBase, (arg1))

#define __glRasterPos4dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos4dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 239, Mesa)

#define glRasterPos4dv(arg1) \
    __glRasterPos4dv_WB(MesaBase, (arg1))

#define __glRasterPos4fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos4fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 240, Mesa)

#define glRasterPos4fv(arg1) \
    __glRasterPos4fv_WB(MesaBase, (arg1))

#define __glRasterPos4iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos4iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 241, Mesa)

#define glRasterPos4iv(arg1) \
    __glRasterPos4iv_WB(MesaBase, (arg1))

#define __glRasterPos4sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glRasterPos4sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 242, Mesa)

#define glRasterPos4sv(arg1) \
    __glRasterPos4sv_WB(MesaBase, (arg1))

#define __glRectd_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRectd, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 243, Mesa)

#define glRectd(arg1, arg2, arg3, arg4) \
    __glRectd_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRectf_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRectf, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 244, Mesa)

#define glRectf(arg1, arg2, arg3, arg4) \
    __glRectf_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRecti_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRecti, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 245, Mesa)

#define glRecti(arg1, arg2, arg3, arg4) \
    __glRecti_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRects_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRects, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 246, Mesa)

#define glRects(arg1, arg2, arg3, arg4) \
    __glRects_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRectdv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRectdv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
                  AROS_LCA(const GLdouble *,(__arg2),A1), \
        struct Library *, (__MesaBase), 247, Mesa)

#define glRectdv(arg1, arg2) \
    __glRectdv_WB(MesaBase, (arg1), (arg2))

#define __glRectfv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRectfv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
                  AROS_LCA(const GLfloat *,(__arg2),A1), \
        struct Library *, (__MesaBase), 248, Mesa)

#define glRectfv(arg1, arg2) \
    __glRectfv_WB(MesaBase, (arg1), (arg2))

#define __glRectiv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRectiv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
                  AROS_LCA(const GLint *,(__arg2),A1), \
        struct Library *, (__MesaBase), 249, Mesa)

#define glRectiv(arg1, arg2) \
    __glRectiv_WB(MesaBase, (arg1), (arg2))

#define __glRectsv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRectsv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
                  AROS_LCA(const GLshort *,(__arg2),A1), \
        struct Library *, (__MesaBase), 250, Mesa)

#define glRectsv(arg1, arg2) \
    __glRectsv_WB(MesaBase, (arg1), (arg2))

#define __glVertexPointer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexPointer, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 251, Mesa)

#define glVertexPointer(arg1, arg2, arg3, arg4) \
    __glVertexPointer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glNormalPointer_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glNormalPointer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 252, Mesa)

#define glNormalPointer(arg1, arg2, arg3) \
    __glNormalPointer_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColorPointer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glColorPointer, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 253, Mesa)

#define glColorPointer(arg1, arg2, arg3, arg4) \
    __glColorPointer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glIndexPointer_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glIndexPointer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 254, Mesa)

#define glIndexPointer(arg1, arg2, arg3) \
    __glIndexPointer_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexCoordPointer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTexCoordPointer, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 255, Mesa)

#define glTexCoordPointer(arg1, arg2, arg3, arg4) \
    __glTexCoordPointer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glEdgeFlagPointer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glEdgeFlagPointer, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLvoid *,(__arg2),A0), \
        struct Library *, (__MesaBase), 256, Mesa)

#define glEdgeFlagPointer(arg1, arg2) \
    __glEdgeFlagPointer_WB(MesaBase, (arg1), (arg2))

#define __glGetPointerv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetPointerv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLvoid * *,(__arg2),A0), \
        struct Library *, (__MesaBase), 257, Mesa)

#define glGetPointerv(arg1, arg2) \
    __glGetPointerv_WB(MesaBase, (arg1), (arg2))

#define __glArrayElement_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glArrayElement, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 258, Mesa)

#define glArrayElement(arg1) \
    __glArrayElement_WB(MesaBase, (arg1))

#define __glDrawArrays_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glDrawArrays, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
        struct Library *, (__MesaBase), 259, Mesa)

#define glDrawArrays(arg1, arg2, arg3) \
    __glDrawArrays_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glDrawElements_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glDrawElements, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 260, Mesa)

#define glDrawElements(arg1, arg2, arg3, arg4) \
    __glDrawElements_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glInterleavedArrays_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glInterleavedArrays, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 261, Mesa)

#define glInterleavedArrays(arg1, arg2, arg3) \
    __glInterleavedArrays_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glShadeModel_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glShadeModel, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 262, Mesa)

#define glShadeModel(arg1) \
    __glShadeModel_WB(MesaBase, (arg1))

#define __glLightf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glLightf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 263, Mesa)

#define glLightf(arg1, arg2, arg3) \
    __glLightf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glLighti_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glLighti, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 264, Mesa)

#define glLighti(arg1, arg2, arg3) \
    __glLighti_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glLightfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glLightfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 265, Mesa)

#define glLightfv(arg1, arg2, arg3) \
    __glLightfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glLightiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glLightiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 266, Mesa)

#define glLightiv(arg1, arg2, arg3) \
    __glLightiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetLightfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetLightfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 267, Mesa)

#define glGetLightfv(arg1, arg2, arg3) \
    __glGetLightfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetLightiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetLightiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 268, Mesa)

#define glGetLightiv(arg1, arg2, arg3) \
    __glGetLightiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glLightModelf_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLightModelf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 269, Mesa)

#define glLightModelf(arg1, arg2) \
    __glLightModelf_WB(MesaBase, (arg1), (arg2))

#define __glLightModeli_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLightModeli, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 270, Mesa)

#define glLightModeli(arg1, arg2) \
    __glLightModeli_WB(MesaBase, (arg1), (arg2))

#define __glLightModelfv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLightModelfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 271, Mesa)

#define glLightModelfv(arg1, arg2) \
    __glLightModelfv_WB(MesaBase, (arg1), (arg2))

#define __glLightModeliv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLightModeliv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 272, Mesa)

#define glLightModeliv(arg1, arg2) \
    __glLightModeliv_WB(MesaBase, (arg1), (arg2))

#define __glMaterialf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMaterialf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 273, Mesa)

#define glMaterialf(arg1, arg2, arg3) \
    __glMaterialf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMateriali_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMateriali, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 274, Mesa)

#define glMateriali(arg1, arg2, arg3) \
    __glMateriali_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMaterialfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMaterialfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 275, Mesa)

#define glMaterialfv(arg1, arg2, arg3) \
    __glMaterialfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMaterialiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMaterialiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 276, Mesa)

#define glMaterialiv(arg1, arg2, arg3) \
    __glMaterialiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetMaterialfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMaterialfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 277, Mesa)

#define glGetMaterialfv(arg1, arg2, arg3) \
    __glGetMaterialfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetMaterialiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMaterialiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 278, Mesa)

#define glGetMaterialiv(arg1, arg2, arg3) \
    __glGetMaterialiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColorMaterial_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glColorMaterial, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 279, Mesa)

#define glColorMaterial(arg1, arg2) \
    __glColorMaterial_WB(MesaBase, (arg1), (arg2))

#define __glPixelZoom_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPixelZoom, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 280, Mesa)

#define glPixelZoom(arg1, arg2) \
    __glPixelZoom_WB(MesaBase, (arg1), (arg2))

#define __glPixelStoref_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPixelStoref, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 281, Mesa)

#define glPixelStoref(arg1, arg2) \
    __glPixelStoref_WB(MesaBase, (arg1), (arg2))

#define __glPixelStorei_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPixelStorei, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 282, Mesa)

#define glPixelStorei(arg1, arg2) \
    __glPixelStorei_WB(MesaBase, (arg1), (arg2))

#define __glPixelTransferf_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPixelTransferf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 283, Mesa)

#define glPixelTransferf(arg1, arg2) \
    __glPixelTransferf_WB(MesaBase, (arg1), (arg2))

#define __glPixelTransferi_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPixelTransferi, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 284, Mesa)

#define glPixelTransferi(arg1, arg2) \
    __glPixelTransferi_WB(MesaBase, (arg1), (arg2))

#define __glPixelMapfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPixelMapfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 285, Mesa)

#define glPixelMapfv(arg1, arg2, arg3) \
    __glPixelMapfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glPixelMapuiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPixelMapuiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLuint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 286, Mesa)

#define glPixelMapuiv(arg1, arg2, arg3) \
    __glPixelMapuiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glPixelMapusv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPixelMapusv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLushort *,(__arg3),A0), \
        struct Library *, (__MesaBase), 287, Mesa)

#define glPixelMapusv(arg1, arg2, arg3) \
    __glPixelMapusv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetPixelMapfv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetPixelMapfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 288, Mesa)

#define glGetPixelMapfv(arg1, arg2) \
    __glGetPixelMapfv_WB(MesaBase, (arg1), (arg2))

#define __glGetPixelMapuiv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetPixelMapuiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 289, Mesa)

#define glGetPixelMapuiv(arg1, arg2) \
    __glGetPixelMapuiv_WB(MesaBase, (arg1), (arg2))

#define __glGetPixelMapusv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetPixelMapusv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLushort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 290, Mesa)

#define glGetPixelMapusv(arg1, arg2) \
    __glGetPixelMapusv_WB(MesaBase, (arg1), (arg2))

#define __glBitmap_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glBitmap, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
                  AROS_LCA(const GLubyte *,(__arg7),A0), \
        struct Library *, (__MesaBase), 291, Mesa)

#define glBitmap(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glBitmap_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glReadPixels_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glReadPixels, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 292, Mesa)

#define glReadPixels(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glReadPixels_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glDrawPixels_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glDrawPixels, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(const GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 293, Mesa)

#define glDrawPixels(arg1, arg2, arg3, arg4, arg5) \
    __glDrawPixels_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glCopyPixels_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glCopyPixels, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
        struct Library *, (__MesaBase), 294, Mesa)

#define glCopyPixels(arg1, arg2, arg3, arg4, arg5) \
    __glCopyPixels_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glStencilFunc_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glStencilFunc, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
        struct Library *, (__MesaBase), 295, Mesa)

#define glStencilFunc(arg1, arg2, arg3) \
    __glStencilFunc_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glStencilMask_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glStencilMask, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 296, Mesa)

#define glStencilMask(arg1) \
    __glStencilMask_WB(MesaBase, (arg1))

#define __glStencilOp_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glStencilOp, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
        struct Library *, (__MesaBase), 297, Mesa)

#define glStencilOp(arg1, arg2, arg3) \
    __glStencilOp_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glClearStencil_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClearStencil, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 298, Mesa)

#define glClearStencil(arg1) \
    __glClearStencil_WB(MesaBase, (arg1))

#define __glTexGend_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGend, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 299, Mesa)

#define glTexGend(arg1, arg2, arg3) \
    __glTexGend_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexGenf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGenf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 300, Mesa)

#define glTexGenf(arg1, arg2, arg3) \
    __glTexGenf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexGeni_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGeni, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 301, Mesa)

#define glTexGeni(arg1, arg2, arg3) \
    __glTexGeni_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexGendv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGendv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 302, Mesa)

#define glTexGendv(arg1, arg2, arg3) \
    __glTexGendv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexGenfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGenfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 303, Mesa)

#define glTexGenfv(arg1, arg2, arg3) \
    __glTexGenfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexGeniv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexGeniv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 304, Mesa)

#define glTexGeniv(arg1, arg2, arg3) \
    __glTexGeniv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexGendv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexGendv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 305, Mesa)

#define glGetTexGendv(arg1, arg2, arg3) \
    __glGetTexGendv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexGenfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexGenfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 306, Mesa)

#define glGetTexGenfv(arg1, arg2, arg3) \
    __glGetTexGenfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexGeniv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexGeniv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 307, Mesa)

#define glGetTexGeniv(arg1, arg2, arg3) \
    __glGetTexGeniv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexEnvf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexEnvf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 308, Mesa)

#define glTexEnvf(arg1, arg2, arg3) \
    __glTexEnvf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexEnvi_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexEnvi, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 309, Mesa)

#define glTexEnvi(arg1, arg2, arg3) \
    __glTexEnvi_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexEnvfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexEnvfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 310, Mesa)

#define glTexEnvfv(arg1, arg2, arg3) \
    __glTexEnvfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexEnviv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexEnviv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 311, Mesa)

#define glTexEnviv(arg1, arg2, arg3) \
    __glTexEnviv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexEnvfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexEnvfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 312, Mesa)

#define glGetTexEnvfv(arg1, arg2, arg3) \
    __glGetTexEnvfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexEnviv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexEnviv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 313, Mesa)

#define glGetTexEnviv(arg1, arg2, arg3) \
    __glGetTexEnviv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexParameterf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexParameterf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 314, Mesa)

#define glTexParameterf(arg1, arg2, arg3) \
    __glTexParameterf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexParameteri_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexParameteri, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 315, Mesa)

#define glTexParameteri(arg1, arg2, arg3) \
    __glTexParameteri_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 316, Mesa)

#define glTexParameterfv(arg1, arg2, arg3) \
    __glTexParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glTexParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 317, Mesa)

#define glTexParameteriv(arg1, arg2, arg3) \
    __glTexParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 318, Mesa)

#define glGetTexParameterfv(arg1, arg2, arg3) \
    __glGetTexParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetTexParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 319, Mesa)

#define glGetTexParameteriv(arg1, arg2, arg3) \
    __glGetTexParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTexLevelParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetTexLevelParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 320, Mesa)

#define glGetTexLevelParameterfv(arg1, arg2, arg3, arg4) \
    __glGetTexLevelParameterfv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetTexLevelParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetTexLevelParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 321, Mesa)

#define glGetTexLevelParameteriv(arg1, arg2, arg3, arg4) \
    __glGetTexLevelParameteriv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glTexImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(const GLvoid *,(__arg8),A0), \
        struct Library *, (__MesaBase), 322, Mesa)

#define glTexImage1D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glTexImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glTexImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glTexImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLenum,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 323, Mesa)

#define glTexImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glTexImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glGetTexImage_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetTexImage, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 324, Mesa)

#define glGetTexImage(arg1, arg2, arg3, arg4, arg5) \
    __glGetTexImage_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGenTextures_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenTextures, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 325, Mesa)

#define glGenTextures(arg1, arg2) \
    __glGenTextures_WB(MesaBase, (arg1), (arg2))

#define __glDeleteTextures_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteTextures, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 326, Mesa)

#define glDeleteTextures(arg1, arg2) \
    __glDeleteTextures_WB(MesaBase, (arg1), (arg2))

#define __glBindTexture_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindTexture, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 327, Mesa)

#define glBindTexture(arg1, arg2) \
    __glBindTexture_WB(MesaBase, (arg1), (arg2))

#define __glPrioritizeTextures_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPrioritizeTextures, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
                  AROS_LCA(const GLclampf *,(__arg3),A1), \
        struct Library *, (__MesaBase), 328, Mesa)

#define glPrioritizeTextures(arg1, arg2, arg3) \
    __glPrioritizeTextures_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glAreTexturesResident_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLboolean, glAreTexturesResident, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
                  AROS_LCA(GLboolean *,(__arg3),A1), \
        struct Library *, (__MesaBase), 329, Mesa)

#define glAreTexturesResident(arg1, arg2, arg3) \
    __glAreTexturesResident_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsTexture_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsTexture, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 330, Mesa)

#define glIsTexture(arg1) \
    __glIsTexture_WB(MesaBase, (arg1))

#define __glTexSubImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glTexSubImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 331, Mesa)

#define glTexSubImage1D(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glTexSubImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glTexSubImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glTexSubImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLenum,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 332, Mesa)

#define glTexSubImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glTexSubImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCopyTexImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCopyTexImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
        struct Library *, (__MesaBase), 333, Mesa)

#define glCopyTexImage1D(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCopyTexImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glCopyTexImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCopyTexImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLint,(__arg8),D7), \
        struct Library *, (__MesaBase), 334, Mesa)

#define glCopyTexImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCopyTexImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glCopyTexSubImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glCopyTexSubImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
        struct Library *, (__MesaBase), 335, Mesa)

#define glCopyTexSubImage1D(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glCopyTexSubImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glCopyTexSubImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCopyTexSubImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
        struct Library *, (__MesaBase), 336, Mesa)

#define glCopyTexSubImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCopyTexSubImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glMap1d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glMap1d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(const GLdouble *,(__arg6),A0), \
        struct Library *, (__MesaBase), 337, Mesa)

#define glMap1d(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glMap1d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glMap1f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glMap1f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(const GLfloat *,(__arg6),A0), \
        struct Library *, (__MesaBase), 338, Mesa)

#define glMap1f(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glMap1f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glMap2d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glMap2d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
                  AROS_LCA(GLdouble,(__arg7),D6), \
                  AROS_LCA(GLint,(__arg8),D7), \
                  AROS_LCA(GLint,(__arg9),A0), \
                  AROS_LCA(const GLdouble *,(__arg10),A1), \
        struct Library *, (__MesaBase), 339, Mesa)

#define glMap2d(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glMap2d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glMap2f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glMap2f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
                  AROS_LCA(GLfloat,(__arg7),D6), \
                  AROS_LCA(GLint,(__arg8),D7), \
                  AROS_LCA(GLint,(__arg9),A0), \
                  AROS_LCA(const GLfloat *,(__arg10),A1), \
        struct Library *, (__MesaBase), 340, Mesa)

#define glMap2f(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glMap2f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glGetMapdv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMapdv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 341, Mesa)

#define glGetMapdv(arg1, arg2, arg3) \
    __glGetMapdv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetMapfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMapfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 342, Mesa)

#define glGetMapfv(arg1, arg2, arg3) \
    __glGetMapfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetMapiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMapiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 343, Mesa)

#define glGetMapiv(arg1, arg2, arg3) \
    __glGetMapiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glEvalCoord1d_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord1d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
        struct Library *, (__MesaBase), 344, Mesa)

#define glEvalCoord1d(arg1) \
    __glEvalCoord1d_WB(MesaBase, (arg1))

#define __glEvalCoord1f_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord1f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 345, Mesa)

#define glEvalCoord1f(arg1) \
    __glEvalCoord1f_WB(MesaBase, (arg1))

#define __glEvalCoord1dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord1dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 346, Mesa)

#define glEvalCoord1dv(arg1) \
    __glEvalCoord1dv_WB(MesaBase, (arg1))

#define __glEvalCoord1fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord1fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 347, Mesa)

#define glEvalCoord1fv(arg1) \
    __glEvalCoord1fv_WB(MesaBase, (arg1))

#define __glEvalCoord2d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glEvalCoord2d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 348, Mesa)

#define glEvalCoord2d(arg1, arg2) \
    __glEvalCoord2d_WB(MesaBase, (arg1), (arg2))

#define __glEvalCoord2f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glEvalCoord2f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 349, Mesa)

#define glEvalCoord2f(arg1, arg2) \
    __glEvalCoord2f_WB(MesaBase, (arg1), (arg2))

#define __glEvalCoord2dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord2dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 350, Mesa)

#define glEvalCoord2dv(arg1) \
    __glEvalCoord2dv_WB(MesaBase, (arg1))

#define __glEvalCoord2fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalCoord2fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 351, Mesa)

#define glEvalCoord2fv(arg1) \
    __glEvalCoord2fv_WB(MesaBase, (arg1))

#define __glMapGrid1d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMapGrid1d, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 352, Mesa)

#define glMapGrid1d(arg1, arg2, arg3) \
    __glMapGrid1d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMapGrid1f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMapGrid1f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 353, Mesa)

#define glMapGrid1f(arg1, arg2, arg3) \
    __glMapGrid1f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMapGrid2d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glMapGrid2d, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 354, Mesa)

#define glMapGrid2d(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glMapGrid2d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glMapGrid2f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glMapGrid2f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
        struct Library *, (__MesaBase), 355, Mesa)

#define glMapGrid2f(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glMapGrid2f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glEvalPoint1_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEvalPoint1, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 356, Mesa)

#define glEvalPoint1(arg1) \
    __glEvalPoint1_WB(MesaBase, (arg1))

#define __glEvalPoint2_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glEvalPoint2, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 357, Mesa)

#define glEvalPoint2(arg1, arg2) \
    __glEvalPoint2_WB(MesaBase, (arg1), (arg2))

#define __glEvalMesh1_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glEvalMesh1, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 358, Mesa)

#define glEvalMesh1(arg1, arg2, arg3) \
    __glEvalMesh1_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glEvalMesh2_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glEvalMesh2, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 359, Mesa)

#define glEvalMesh2(arg1, arg2, arg3, arg4, arg5) \
    __glEvalMesh2_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFogf_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glFogf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 360, Mesa)

#define glFogf(arg1, arg2) \
    __glFogf_WB(MesaBase, (arg1), (arg2))

#define __glFogi_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glFogi, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 361, Mesa)

#define glFogi(arg1, arg2) \
    __glFogi_WB(MesaBase, (arg1), (arg2))

#define __glFogfv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glFogfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 362, Mesa)

#define glFogfv(arg1, arg2) \
    __glFogfv_WB(MesaBase, (arg1), (arg2))

#define __glFogiv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glFogiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 363, Mesa)

#define glFogiv(arg1, arg2) \
    __glFogiv_WB(MesaBase, (arg1), (arg2))

#define __glFeedbackBuffer_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glFeedbackBuffer, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 364, Mesa)

#define glFeedbackBuffer(arg1, arg2, arg3) \
    __glFeedbackBuffer_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glPassThrough_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPassThrough, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 365, Mesa)

#define glPassThrough(arg1) \
    __glPassThrough_WB(MesaBase, (arg1))

#define __glSelectBuffer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glSelectBuffer, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 366, Mesa)

#define glSelectBuffer(arg1, arg2) \
    __glSelectBuffer_WB(MesaBase, (arg1), (arg2))

#define __glInitNames_WB(__MesaBase) \
        AROS_LC0NR(void, glInitNames, \
        struct Library *, (__MesaBase), 367, Mesa)

#define glInitNames() \
    __glInitNames_WB(MesaBase)

#define __glLoadName_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadName, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 368, Mesa)

#define glLoadName(arg1) \
    __glLoadName_WB(MesaBase, (arg1))

#define __glPushName_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glPushName, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 369, Mesa)

#define glPushName(arg1) \
    __glPushName_WB(MesaBase, (arg1))

#define __glPopName_WB(__MesaBase) \
        AROS_LC0NR(void, glPopName, \
        struct Library *, (__MesaBase), 370, Mesa)

#define glPopName() \
    __glPopName_WB(MesaBase)

#define __glDrawRangeElements_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glDrawRangeElements, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 371, Mesa)

#define glDrawRangeElements(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glDrawRangeElements_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glTexImage3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glTexImage3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLenum,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(const GLvoid *,(__arg10),A1), \
        struct Library *, (__MesaBase), 372, Mesa)

#define glTexImage3D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glTexImage3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glTexSubImage3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11) \
        AROS_LC11NR(void, glTexSubImage3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(GLenum,(__arg10),A1), \
                  AROS_LCA(const GLvoid *,(__arg11),A2), \
        struct Library *, (__MesaBase), 373, Mesa)

#define glTexSubImage3D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
    __glTexSubImage3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11))

#define __glCopyTexSubImage3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCopyTexSubImage3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLsizei,(__arg9),A0), \
        struct Library *, (__MesaBase), 374, Mesa)

#define glCopyTexSubImage3D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCopyTexSubImage3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glColorTable_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glColorTable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 375, Mesa)

#define glColorTable(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glColorTable_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glColorSubTable_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glColorSubTable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 376, Mesa)

#define glColorSubTable(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glColorSubTable_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glColorTableParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColorTableParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 377, Mesa)

#define glColorTableParameteriv(arg1, arg2, arg3) \
    __glColorTableParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColorTableParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glColorTableParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 378, Mesa)

#define glColorTableParameterfv(arg1, arg2, arg3) \
    __glColorTableParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glCopyColorSubTable_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glCopyColorSubTable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
        struct Library *, (__MesaBase), 379, Mesa)

#define glCopyColorSubTable(arg1, arg2, arg3, arg4, arg5) \
    __glCopyColorSubTable_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glCopyColorTable_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glCopyColorTable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
        struct Library *, (__MesaBase), 380, Mesa)

#define glCopyColorTable(arg1, arg2, arg3, arg4, arg5) \
    __glCopyColorTable_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetColorTable_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetColorTable, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 381, Mesa)

#define glGetColorTable(arg1, arg2, arg3, arg4) \
    __glGetColorTable_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetColorTableParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetColorTableParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 382, Mesa)

#define glGetColorTableParameterfv(arg1, arg2, arg3) \
    __glGetColorTableParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetColorTableParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetColorTableParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 383, Mesa)

#define glGetColorTableParameteriv(arg1, arg2, arg3) \
    __glGetColorTableParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBlendEquation_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glBlendEquation, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 384, Mesa)

#define glBlendEquation(arg1) \
    __glBlendEquation_WB(MesaBase, (arg1))

#define __glBlendColor_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBlendColor, \
                  AROS_LCA(GLclampf,(__arg1),D0), \
                  AROS_LCA(GLclampf,(__arg2),D1), \
                  AROS_LCA(GLclampf,(__arg3),D2), \
                  AROS_LCA(GLclampf,(__arg4),D3), \
        struct Library *, (__MesaBase), 385, Mesa)

#define glBlendColor(arg1, arg2, arg3, arg4) \
    __glBlendColor_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glHistogram_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glHistogram, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLboolean,(__arg4),D3), \
        struct Library *, (__MesaBase), 386, Mesa)

#define glHistogram(arg1, arg2, arg3, arg4) \
    __glHistogram_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glResetHistogram_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glResetHistogram, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 387, Mesa)

#define glResetHistogram(arg1) \
    __glResetHistogram_WB(MesaBase, (arg1))

#define __glGetHistogram_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetHistogram, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 388, Mesa)

#define glGetHistogram(arg1, arg2, arg3, arg4, arg5) \
    __glGetHistogram_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetHistogramParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetHistogramParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 389, Mesa)

#define glGetHistogramParameterfv(arg1, arg2, arg3) \
    __glGetHistogramParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetHistogramParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetHistogramParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 390, Mesa)

#define glGetHistogramParameteriv(arg1, arg2, arg3) \
    __glGetHistogramParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMinmax_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMinmax, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
        struct Library *, (__MesaBase), 391, Mesa)

#define glMinmax(arg1, arg2, arg3) \
    __glMinmax_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glResetMinmax_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glResetMinmax, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 392, Mesa)

#define glResetMinmax(arg1) \
    __glResetMinmax_WB(MesaBase, (arg1))

#define __glGetMinmax_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetMinmax, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 393, Mesa)

#define glGetMinmax(arg1, arg2, arg3, arg4, arg5) \
    __glGetMinmax_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetMinmaxParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMinmaxParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 394, Mesa)

#define glGetMinmaxParameterfv(arg1, arg2, arg3) \
    __glGetMinmaxParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetMinmaxParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetMinmaxParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 395, Mesa)

#define glGetMinmaxParameteriv(arg1, arg2, arg3) \
    __glGetMinmaxParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glConvolutionFilter1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glConvolutionFilter1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 396, Mesa)

#define glConvolutionFilter1D(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glConvolutionFilter1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glConvolutionFilter2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glConvolutionFilter2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 397, Mesa)

#define glConvolutionFilter2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glConvolutionFilter2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glConvolutionParameterf_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glConvolutionParameterf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 398, Mesa)

#define glConvolutionParameterf(arg1, arg2, arg3) \
    __glConvolutionParameterf_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glConvolutionParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glConvolutionParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 399, Mesa)

#define glConvolutionParameterfv(arg1, arg2, arg3) \
    __glConvolutionParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glConvolutionParameteri_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glConvolutionParameteri, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 400, Mesa)

#define glConvolutionParameteri(arg1, arg2, arg3) \
    __glConvolutionParameteri_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glConvolutionParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glConvolutionParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 401, Mesa)

#define glConvolutionParameteriv(arg1, arg2, arg3) \
    __glConvolutionParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glCopyConvolutionFilter1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glCopyConvolutionFilter1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
        struct Library *, (__MesaBase), 402, Mesa)

#define glCopyConvolutionFilter1D(arg1, arg2, arg3, arg4, arg5) \
    __glCopyConvolutionFilter1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glCopyConvolutionFilter2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glCopyConvolutionFilter2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
        struct Library *, (__MesaBase), 403, Mesa)

#define glCopyConvolutionFilter2D(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glCopyConvolutionFilter2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glGetConvolutionFilter_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetConvolutionFilter, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 404, Mesa)

#define glGetConvolutionFilter(arg1, arg2, arg3, arg4) \
    __glGetConvolutionFilter_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetConvolutionParameterfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetConvolutionParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 405, Mesa)

#define glGetConvolutionParameterfv(arg1, arg2, arg3) \
    __glGetConvolutionParameterfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetConvolutionParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetConvolutionParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 406, Mesa)

#define glGetConvolutionParameteriv(arg1, arg2, arg3) \
    __glGetConvolutionParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSeparableFilter2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glSeparableFilter2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
                  AROS_LCA(const GLvoid *,(__arg8),A1), \
        struct Library *, (__MesaBase), 407, Mesa)

#define glSeparableFilter2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glSeparableFilter2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glGetSeparableFilter_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glGetSeparableFilter, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
                  AROS_LCA(GLvoid *,(__arg5),A1), \
                  AROS_LCA(GLvoid *,(__arg6),A2), \
        struct Library *, (__MesaBase), 408, Mesa)

#define glGetSeparableFilter(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glGetSeparableFilter_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glActiveTexture_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glActiveTexture, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 409, Mesa)

#define glActiveTexture(arg1) \
    __glActiveTexture_WB(MesaBase, (arg1))

#define __glClientActiveTexture_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClientActiveTexture, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 410, Mesa)

#define glClientActiveTexture(arg1) \
    __glClientActiveTexture_WB(MesaBase, (arg1))

#define __glCompressedTexImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCompressedTexImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 411, Mesa)

#define glCompressedTexImage1D(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCompressedTexImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glCompressedTexImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCompressedTexImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(const GLvoid *,(__arg8),A0), \
        struct Library *, (__MesaBase), 412, Mesa)

#define glCompressedTexImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCompressedTexImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glCompressedTexImage3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCompressedTexImage3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 413, Mesa)

#define glCompressedTexImage3D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCompressedTexImage3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCompressedTexSubImage1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCompressedTexSubImage1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 414, Mesa)

#define glCompressedTexSubImage1D(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCompressedTexSubImage1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glCompressedTexSubImage2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCompressedTexSubImage2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 415, Mesa)

#define glCompressedTexSubImage2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCompressedTexSubImage2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCompressedTexSubImage3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11) \
        AROS_LC11NR(void, glCompressedTexSubImage3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(GLsizei,(__arg10),A1), \
                  AROS_LCA(const GLvoid *,(__arg11),A2), \
        struct Library *, (__MesaBase), 416, Mesa)

#define glCompressedTexSubImage3D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
    __glCompressedTexSubImage3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11))

#define __glGetCompressedTexImage_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetCompressedTexImage, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 417, Mesa)

#define glGetCompressedTexImage(arg1, arg2, arg3) \
    __glGetCompressedTexImage_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord1d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 418, Mesa)

#define glMultiTexCoord1d(arg1, arg2) \
    __glMultiTexCoord1d_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1dv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 419, Mesa)

#define glMultiTexCoord1dv(arg1, arg2) \
    __glMultiTexCoord1dv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 420, Mesa)

#define glMultiTexCoord1f(arg1, arg2) \
    __glMultiTexCoord1f_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1fv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 421, Mesa)

#define glMultiTexCoord1fv(arg1, arg2) \
    __glMultiTexCoord1fv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1i, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 422, Mesa)

#define glMultiTexCoord1i(arg1, arg2) \
    __glMultiTexCoord1i_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1iv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1iv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 423, Mesa)

#define glMultiTexCoord1iv(arg1, arg2) \
    __glMultiTexCoord1iv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1s, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 424, Mesa)

#define glMultiTexCoord1s(arg1, arg2) \
    __glMultiTexCoord1s_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1sv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 425, Mesa)

#define glMultiTexCoord1sv(arg1, arg2) \
    __glMultiTexCoord1sv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 426, Mesa)

#define glMultiTexCoord2d(arg1, arg2, arg3) \
    __glMultiTexCoord2d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2dv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 427, Mesa)

#define glMultiTexCoord2dv(arg1, arg2) \
    __glMultiTexCoord2dv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 428, Mesa)

#define glMultiTexCoord2f(arg1, arg2, arg3) \
    __glMultiTexCoord2f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2fv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 429, Mesa)

#define glMultiTexCoord2fv(arg1, arg2) \
    __glMultiTexCoord2fv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2i, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 430, Mesa)

#define glMultiTexCoord2i(arg1, arg2, arg3) \
    __glMultiTexCoord2i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2iv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2iv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 431, Mesa)

#define glMultiTexCoord2iv(arg1, arg2) \
    __glMultiTexCoord2iv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2s, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 432, Mesa)

#define glMultiTexCoord2s(arg1, arg2, arg3) \
    __glMultiTexCoord2s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2sv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 433, Mesa)

#define glMultiTexCoord2sv(arg1, arg2) \
    __glMultiTexCoord2sv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 434, Mesa)

#define glMultiTexCoord3d(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3dv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 435, Mesa)

#define glMultiTexCoord3dv(arg1, arg2) \
    __glMultiTexCoord3dv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 436, Mesa)

#define glMultiTexCoord3f(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3fv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 437, Mesa)

#define glMultiTexCoord3fv(arg1, arg2) \
    __glMultiTexCoord3fv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3i, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 438, Mesa)

#define glMultiTexCoord3i(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3iv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3iv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 439, Mesa)

#define glMultiTexCoord3iv(arg1, arg2) \
    __glMultiTexCoord3iv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3s, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 440, Mesa)

#define glMultiTexCoord3s(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3sv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 441, Mesa)

#define glMultiTexCoord3sv(arg1, arg2) \
    __glMultiTexCoord3sv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4d, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
        struct Library *, (__MesaBase), 442, Mesa)

#define glMultiTexCoord4d(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4dv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 443, Mesa)

#define glMultiTexCoord4dv(arg1, arg2) \
    __glMultiTexCoord4dv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4f, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 444, Mesa)

#define glMultiTexCoord4f(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4fv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 445, Mesa)

#define glMultiTexCoord4fv(arg1, arg2) \
    __glMultiTexCoord4fv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4i, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 446, Mesa)

#define glMultiTexCoord4i(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4iv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4iv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 447, Mesa)

#define glMultiTexCoord4iv(arg1, arg2) \
    __glMultiTexCoord4iv_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4s, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
                  AROS_LCA(GLshort,(__arg5),D4), \
        struct Library *, (__MesaBase), 448, Mesa)

#define glMultiTexCoord4s(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4sv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 449, Mesa)

#define glMultiTexCoord4sv(arg1, arg2) \
    __glMultiTexCoord4sv_WB(MesaBase, (arg1), (arg2))

#define __glLoadTransposeMatrixd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadTransposeMatrixd, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 450, Mesa)

#define glLoadTransposeMatrixd(arg1) \
    __glLoadTransposeMatrixd_WB(MesaBase, (arg1))

#define __glLoadTransposeMatrixf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadTransposeMatrixf, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 451, Mesa)

#define glLoadTransposeMatrixf(arg1) \
    __glLoadTransposeMatrixf_WB(MesaBase, (arg1))

#define __glMultTransposeMatrixd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultTransposeMatrixd, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 452, Mesa)

#define glMultTransposeMatrixd(arg1) \
    __glMultTransposeMatrixd_WB(MesaBase, (arg1))

#define __glMultTransposeMatrixf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultTransposeMatrixf, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 453, Mesa)

#define glMultTransposeMatrixf(arg1) \
    __glMultTransposeMatrixf_WB(MesaBase, (arg1))

#define __glSampleCoverage_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glSampleCoverage, \
                  AROS_LCA(GLclampf,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
        struct Library *, (__MesaBase), 454, Mesa)

#define glSampleCoverage(arg1, arg2) \
    __glSampleCoverage_WB(MesaBase, (arg1), (arg2))

#define __glActiveTextureARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glActiveTextureARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 455, Mesa)

#define glActiveTextureARB(arg1) \
    __glActiveTextureARB_WB(MesaBase, (arg1))

#define __glClientActiveTextureARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glClientActiveTextureARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 456, Mesa)

#define glClientActiveTextureARB(arg1) \
    __glClientActiveTextureARB_WB(MesaBase, (arg1))

#define __glMultiTexCoord1dARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 457, Mesa)

#define glMultiTexCoord1dARB(arg1, arg2) \
    __glMultiTexCoord1dARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 458, Mesa)

#define glMultiTexCoord1dvARB(arg1, arg2) \
    __glMultiTexCoord1dvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1fARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 459, Mesa)

#define glMultiTexCoord1fARB(arg1, arg2) \
    __glMultiTexCoord1fARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 460, Mesa)

#define glMultiTexCoord1fvARB(arg1, arg2) \
    __glMultiTexCoord1fvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1iARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1iARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 461, Mesa)

#define glMultiTexCoord1iARB(arg1, arg2) \
    __glMultiTexCoord1iARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1ivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1ivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 462, Mesa)

#define glMultiTexCoord1ivARB(arg1, arg2) \
    __glMultiTexCoord1ivARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1sARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1sARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 463, Mesa)

#define glMultiTexCoord1sARB(arg1, arg2) \
    __glMultiTexCoord1sARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord1svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord1svARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 464, Mesa)

#define glMultiTexCoord1svARB(arg1, arg2) \
    __glMultiTexCoord1svARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2dARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 465, Mesa)

#define glMultiTexCoord2dARB(arg1, arg2, arg3) \
    __glMultiTexCoord2dARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 466, Mesa)

#define glMultiTexCoord2dvARB(arg1, arg2) \
    __glMultiTexCoord2dvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2fARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 467, Mesa)

#define glMultiTexCoord2fARB(arg1, arg2, arg3) \
    __glMultiTexCoord2fARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 468, Mesa)

#define glMultiTexCoord2fvARB(arg1, arg2) \
    __glMultiTexCoord2fvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2iARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2iARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 469, Mesa)

#define glMultiTexCoord2iARB(arg1, arg2, arg3) \
    __glMultiTexCoord2iARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2ivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2ivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 470, Mesa)

#define glMultiTexCoord2ivARB(arg1, arg2) \
    __glMultiTexCoord2ivARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord2sARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glMultiTexCoord2sARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 471, Mesa)

#define glMultiTexCoord2sARB(arg1, arg2, arg3) \
    __glMultiTexCoord2sARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiTexCoord2svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord2svARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 472, Mesa)

#define glMultiTexCoord2svARB(arg1, arg2) \
    __glMultiTexCoord2svARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 473, Mesa)

#define glMultiTexCoord3dARB(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 474, Mesa)

#define glMultiTexCoord3dvARB(arg1, arg2) \
    __glMultiTexCoord3dvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 475, Mesa)

#define glMultiTexCoord3fARB(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 476, Mesa)

#define glMultiTexCoord3fvARB(arg1, arg2) \
    __glMultiTexCoord3fvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3iARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3iARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 477, Mesa)

#define glMultiTexCoord3iARB(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3iARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3ivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3ivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 478, Mesa)

#define glMultiTexCoord3ivARB(arg1, arg2) \
    __glMultiTexCoord3ivARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord3sARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiTexCoord3sARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 479, Mesa)

#define glMultiTexCoord3sARB(arg1, arg2, arg3, arg4) \
    __glMultiTexCoord3sARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiTexCoord3svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord3svARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 480, Mesa)

#define glMultiTexCoord3svARB(arg1, arg2) \
    __glMultiTexCoord3svARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
        struct Library *, (__MesaBase), 481, Mesa)

#define glMultiTexCoord4dARB(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 482, Mesa)

#define glMultiTexCoord4dvARB(arg1, arg2) \
    __glMultiTexCoord4dvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 483, Mesa)

#define glMultiTexCoord4fARB(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 484, Mesa)

#define glMultiTexCoord4fvARB(arg1, arg2) \
    __glMultiTexCoord4fvARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4iARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4iARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 485, Mesa)

#define glMultiTexCoord4iARB(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4iARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4ivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4ivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 486, Mesa)

#define glMultiTexCoord4ivARB(arg1, arg2) \
    __glMultiTexCoord4ivARB_WB(MesaBase, (arg1), (arg2))

#define __glMultiTexCoord4sARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiTexCoord4sARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
                  AROS_LCA(GLshort,(__arg5),D4), \
        struct Library *, (__MesaBase), 487, Mesa)

#define glMultiTexCoord4sARB(arg1, arg2, arg3, arg4, arg5) \
    __glMultiTexCoord4sARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMultiTexCoord4svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glMultiTexCoord4svARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 488, Mesa)

#define glMultiTexCoord4svARB(arg1, arg2) \
    __glMultiTexCoord4svARB_WB(MesaBase, (arg1), (arg2))

#define __glBlendFuncSeparate_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBlendFuncSeparate, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
        struct Library *, (__MesaBase), 489, Mesa)

#define glBlendFuncSeparate(arg1, arg2, arg3, arg4) \
    __glBlendFuncSeparate_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glFogCoordf_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoordf, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 490, Mesa)

#define glFogCoordf(arg1) \
    __glFogCoordf_WB(MesaBase, (arg1))

#define __glFogCoordfv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoordfv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 491, Mesa)

#define glFogCoordfv(arg1) \
    __glFogCoordfv_WB(MesaBase, (arg1))

#define __glFogCoordd_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoordd, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
        struct Library *, (__MesaBase), 492, Mesa)

#define glFogCoordd(arg1) \
    __glFogCoordd_WB(MesaBase, (arg1))

#define __glFogCoorddv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoorddv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 493, Mesa)

#define glFogCoorddv(arg1) \
    __glFogCoorddv_WB(MesaBase, (arg1))

#define __glFogCoordPointer_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glFogCoordPointer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 494, Mesa)

#define glFogCoordPointer(arg1, arg2, arg3) \
    __glFogCoordPointer_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glMultiDrawArrays_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiDrawArrays, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint *,(__arg2),A0), \
                  AROS_LCA(GLsizei *,(__arg3),A1), \
                  AROS_LCA(GLsizei,(__arg4),D1), \
        struct Library *, (__MesaBase), 495, Mesa)

#define glMultiDrawArrays(arg1, arg2, arg3, arg4) \
    __glMultiDrawArrays_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiDrawElements_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiDrawElements, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLsizei *,(__arg2),A0), \
                  AROS_LCA(GLenum,(__arg3),D1), \
                  AROS_LCA(const GLvoid *  *,(__arg4),A1), \
                  AROS_LCA(GLsizei,(__arg5),D2), \
        struct Library *, (__MesaBase), 496, Mesa)

#define glMultiDrawElements(arg1, arg2, arg3, arg4, arg5) \
    __glMultiDrawElements_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glPointParameterf_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterf, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 497, Mesa)

#define glPointParameterf(arg1, arg2) \
    __glPointParameterf_WB(MesaBase, (arg1), (arg2))

#define __glPointParameterfv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterfv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 498, Mesa)

#define glPointParameterfv(arg1, arg2) \
    __glPointParameterfv_WB(MesaBase, (arg1), (arg2))

#define __glPointParameteri_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameteri, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 499, Mesa)

#define glPointParameteri(arg1, arg2) \
    __glPointParameteri_WB(MesaBase, (arg1), (arg2))

#define __glPointParameteriv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 500, Mesa)

#define glPointParameteriv(arg1, arg2) \
    __glPointParameteriv_WB(MesaBase, (arg1), (arg2))

#define __glSecondaryColor3b_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3b, \
                  AROS_LCA(GLbyte,(__arg1),D0), \
                  AROS_LCA(GLbyte,(__arg2),D1), \
                  AROS_LCA(GLbyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 501, Mesa)

#define glSecondaryColor3b(arg1, arg2, arg3) \
    __glSecondaryColor3b_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3bv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3bv, \
                  AROS_LCA(const GLbyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 502, Mesa)

#define glSecondaryColor3bv(arg1) \
    __glSecondaryColor3bv_WB(MesaBase, (arg1))

#define __glSecondaryColor3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 503, Mesa)

#define glSecondaryColor3d(arg1, arg2, arg3) \
    __glSecondaryColor3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 504, Mesa)

#define glSecondaryColor3dv(arg1) \
    __glSecondaryColor3dv_WB(MesaBase, (arg1))

#define __glSecondaryColor3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 505, Mesa)

#define glSecondaryColor3f(arg1, arg2, arg3) \
    __glSecondaryColor3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 506, Mesa)

#define glSecondaryColor3fv(arg1) \
    __glSecondaryColor3fv_WB(MesaBase, (arg1))

#define __glSecondaryColor3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 507, Mesa)

#define glSecondaryColor3i(arg1, arg2, arg3) \
    __glSecondaryColor3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 508, Mesa)

#define glSecondaryColor3iv(arg1) \
    __glSecondaryColor3iv_WB(MesaBase, (arg1))

#define __glSecondaryColor3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 509, Mesa)

#define glSecondaryColor3s(arg1, arg2, arg3) \
    __glSecondaryColor3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 510, Mesa)

#define glSecondaryColor3sv(arg1) \
    __glSecondaryColor3sv_WB(MesaBase, (arg1))

#define __glSecondaryColor3ub_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3ub, \
                  AROS_LCA(GLubyte,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 511, Mesa)

#define glSecondaryColor3ub(arg1, arg2, arg3) \
    __glSecondaryColor3ub_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3ubv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3ubv, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 512, Mesa)

#define glSecondaryColor3ubv(arg1) \
    __glSecondaryColor3ubv_WB(MesaBase, (arg1))

#define __glSecondaryColor3ui_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3ui, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
        struct Library *, (__MesaBase), 513, Mesa)

#define glSecondaryColor3ui(arg1, arg2, arg3) \
    __glSecondaryColor3ui_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3uiv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3uiv, \
                  AROS_LCA(const GLuint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 514, Mesa)

#define glSecondaryColor3uiv(arg1) \
    __glSecondaryColor3uiv_WB(MesaBase, (arg1))

#define __glSecondaryColor3us_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3us, \
                  AROS_LCA(GLushort,(__arg1),D0), \
                  AROS_LCA(GLushort,(__arg2),D1), \
                  AROS_LCA(GLushort,(__arg3),D2), \
        struct Library *, (__MesaBase), 515, Mesa)

#define glSecondaryColor3us(arg1, arg2, arg3) \
    __glSecondaryColor3us_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3usv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3usv, \
                  AROS_LCA(const GLushort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 516, Mesa)

#define glSecondaryColor3usv(arg1) \
    __glSecondaryColor3usv_WB(MesaBase, (arg1))

#define __glSecondaryColorPointer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glSecondaryColorPointer, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 517, Mesa)

#define glSecondaryColorPointer(arg1, arg2, arg3, arg4) \
    __glSecondaryColorPointer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glWindowPos2d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 518, Mesa)

#define glWindowPos2d(arg1, arg2) \
    __glWindowPos2d_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 519, Mesa)

#define glWindowPos2dv(arg1) \
    __glWindowPos2dv_WB(MesaBase, (arg1))

#define __glWindowPos2f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 520, Mesa)

#define glWindowPos2f(arg1, arg2) \
    __glWindowPos2f_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 521, Mesa)

#define glWindowPos2fv(arg1) \
    __glWindowPos2fv_WB(MesaBase, (arg1))

#define __glWindowPos2i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 522, Mesa)

#define glWindowPos2i(arg1, arg2) \
    __glWindowPos2i_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 523, Mesa)

#define glWindowPos2iv(arg1) \
    __glWindowPos2iv_WB(MesaBase, (arg1))

#define __glWindowPos2s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 524, Mesa)

#define glWindowPos2s(arg1, arg2) \
    __glWindowPos2s_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 525, Mesa)

#define glWindowPos2sv(arg1) \
    __glWindowPos2sv_WB(MesaBase, (arg1))

#define __glWindowPos3d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3d, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 526, Mesa)

#define glWindowPos3d(arg1, arg2, arg3) \
    __glWindowPos3d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3dv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3dv, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 527, Mesa)

#define glWindowPos3dv(arg1) \
    __glWindowPos3dv_WB(MesaBase, (arg1))

#define __glWindowPos3f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3f, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 528, Mesa)

#define glWindowPos3f(arg1, arg2, arg3) \
    __glWindowPos3f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3fv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3fv, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 529, Mesa)

#define glWindowPos3fv(arg1) \
    __glWindowPos3fv_WB(MesaBase, (arg1))

#define __glWindowPos3i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 530, Mesa)

#define glWindowPos3i(arg1, arg2, arg3) \
    __glWindowPos3i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3iv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3iv, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 531, Mesa)

#define glWindowPos3iv(arg1) \
    __glWindowPos3iv_WB(MesaBase, (arg1))

#define __glWindowPos3s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3s, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 532, Mesa)

#define glWindowPos3s(arg1, arg2, arg3) \
    __glWindowPos3s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3sv_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3sv, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 533, Mesa)

#define glWindowPos3sv(arg1) \
    __glWindowPos3sv_WB(MesaBase, (arg1))

#define __glGenQueries_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenQueries, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 534, Mesa)

#define glGenQueries(arg1, arg2) \
    __glGenQueries_WB(MesaBase, (arg1), (arg2))

#define __glDeleteQueries_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteQueries, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 535, Mesa)

#define glDeleteQueries(arg1, arg2) \
    __glDeleteQueries_WB(MesaBase, (arg1), (arg2))

#define __glIsQuery_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsQuery, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 536, Mesa)

#define glIsQuery(arg1) \
    __glIsQuery_WB(MesaBase, (arg1))

#define __glBeginQuery_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBeginQuery, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 537, Mesa)

#define glBeginQuery(arg1, arg2) \
    __glBeginQuery_WB(MesaBase, (arg1), (arg2))

#define __glEndQuery_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEndQuery, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 538, Mesa)

#define glEndQuery(arg1) \
    __glEndQuery_WB(MesaBase, (arg1))

#define __glGetQueryiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryiv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 539, Mesa)

#define glGetQueryiv(arg1, arg2, arg3) \
    __glGetQueryiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetQueryObjectiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryObjectiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 540, Mesa)

#define glGetQueryObjectiv(arg1, arg2, arg3) \
    __glGetQueryObjectiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetQueryObjectuiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryObjectuiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLuint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 541, Mesa)

#define glGetQueryObjectuiv(arg1, arg2, arg3) \
    __glGetQueryObjectuiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBindBuffer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindBuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 542, Mesa)

#define glBindBuffer(arg1, arg2) \
    __glBindBuffer_WB(MesaBase, (arg1), (arg2))

#define __glDeleteBuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteBuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 543, Mesa)

#define glDeleteBuffers(arg1, arg2) \
    __glDeleteBuffers_WB(MesaBase, (arg1), (arg2))

#define __glGenBuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenBuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 544, Mesa)

#define glGenBuffers(arg1, arg2) \
    __glGenBuffers_WB(MesaBase, (arg1), (arg2))

#define __glIsBuffer_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsBuffer, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 545, Mesa)

#define glIsBuffer(arg1) \
    __glIsBuffer_WB(MesaBase, (arg1))

#define __glBufferData_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBufferData, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizeiptr,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
                  AROS_LCA(GLenum,(__arg4),D2), \
        struct Library *, (__MesaBase), 546, Mesa)

#define glBufferData(arg1, arg2, arg3, arg4) \
    __glBufferData_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glBufferSubData_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBufferSubData, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptr,(__arg2),D1), \
                  AROS_LCA(GLsizeiptr,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 547, Mesa)

#define glBufferSubData(arg1, arg2, arg3, arg4) \
    __glBufferSubData_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetBufferSubData_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetBufferSubData, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptr,(__arg2),D1), \
                  AROS_LCA(GLsizeiptr,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 548, Mesa)

#define glGetBufferSubData(arg1, arg2, arg3, arg4) \
    __glGetBufferSubData_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMapBuffer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLvoid*, glMapBuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 549, Mesa)

#define glMapBuffer(arg1, arg2) \
    __glMapBuffer_WB(MesaBase, (arg1), (arg2))

#define __glUnmapBuffer_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glUnmapBuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 550, Mesa)

#define glUnmapBuffer(arg1) \
    __glUnmapBuffer_WB(MesaBase, (arg1))

#define __glGetBufferParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetBufferParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 551, Mesa)

#define glGetBufferParameteriv(arg1, arg2, arg3) \
    __glGetBufferParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetBufferPointerv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetBufferPointerv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *  *,(__arg3),A0), \
        struct Library *, (__MesaBase), 552, Mesa)

#define glGetBufferPointerv(arg1, arg2, arg3) \
    __glGetBufferPointerv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBlendEquationSeparate_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBlendEquationSeparate, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 553, Mesa)

#define glBlendEquationSeparate(arg1, arg2) \
    __glBlendEquationSeparate_WB(MesaBase, (arg1), (arg2))

#define __glDrawBuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDrawBuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLenum *,(__arg2),A0), \
        struct Library *, (__MesaBase), 554, Mesa)

#define glDrawBuffers(arg1, arg2) \
    __glDrawBuffers_WB(MesaBase, (arg1), (arg2))

#define __glStencilOpSeparate_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glStencilOpSeparate, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
        struct Library *, (__MesaBase), 555, Mesa)

#define glStencilOpSeparate(arg1, arg2, arg3, arg4) \
    __glStencilOpSeparate_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glStencilFuncSeparate_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glStencilFuncSeparate, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
        struct Library *, (__MesaBase), 556, Mesa)

#define glStencilFuncSeparate(arg1, arg2, arg3, arg4) \
    __glStencilFuncSeparate_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glStencilMaskSeparate_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glStencilMaskSeparate, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 557, Mesa)

#define glStencilMaskSeparate(arg1, arg2) \
    __glStencilMaskSeparate_WB(MesaBase, (arg1), (arg2))

#define __glAttachShader_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glAttachShader, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 558, Mesa)

#define glAttachShader(arg1, arg2) \
    __glAttachShader_WB(MesaBase, (arg1), (arg2))

#define __glBindAttribLocation_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glBindAttribLocation, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLchar *,(__arg3),A0), \
        struct Library *, (__MesaBase), 559, Mesa)

#define glBindAttribLocation(arg1, arg2, arg3) \
    __glBindAttribLocation_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glCompileShader_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glCompileShader, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 560, Mesa)

#define glCompileShader(arg1) \
    __glCompileShader_WB(MesaBase, (arg1))

#define __glCreateProgram_WB(__MesaBase) \
        AROS_LC0(GLuint, glCreateProgram, \
        struct Library *, (__MesaBase), 561, Mesa)

#define glCreateProgram() \
    __glCreateProgram_WB(MesaBase)

#define __glCreateShader_WB(__MesaBase, __arg1) \
        AROS_LC1(GLuint, glCreateShader, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 562, Mesa)

#define glCreateShader(arg1) \
    __glCreateShader_WB(MesaBase, (arg1))

#define __glDeleteProgram_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDeleteProgram, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 563, Mesa)

#define glDeleteProgram(arg1) \
    __glDeleteProgram_WB(MesaBase, (arg1))

#define __glDeleteShader_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDeleteShader, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 564, Mesa)

#define glDeleteShader(arg1) \
    __glDeleteShader_WB(MesaBase, (arg1))

#define __glDetachShader_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDetachShader, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 565, Mesa)

#define glDetachShader(arg1, arg2) \
    __glDetachShader_WB(MesaBase, (arg1), (arg2))

#define __glDisableVertexAttribArray_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDisableVertexAttribArray, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 566, Mesa)

#define glDisableVertexAttribArray(arg1) \
    __glDisableVertexAttribArray_WB(MesaBase, (arg1))

#define __glEnableVertexAttribArray_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEnableVertexAttribArray, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 567, Mesa)

#define glEnableVertexAttribArray(arg1) \
    __glEnableVertexAttribArray_WB(MesaBase, (arg1))

#define __glGetActiveAttrib_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glGetActiveAttrib, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei *,(__arg4),A0), \
                  AROS_LCA(GLint *,(__arg5),A1), \
                  AROS_LCA(GLenum *,(__arg6),A2), \
                  AROS_LCA(GLchar *,(__arg7),A3), \
        struct Library *, (__MesaBase), 568, Mesa)

#define glGetActiveAttrib(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glGetActiveAttrib_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glGetActiveUniform_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glGetActiveUniform, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei *,(__arg4),A0), \
                  AROS_LCA(GLint *,(__arg5),A1), \
                  AROS_LCA(GLenum *,(__arg6),A2), \
                  AROS_LCA(GLchar *,(__arg7),A3), \
        struct Library *, (__MesaBase), 569, Mesa)

#define glGetActiveUniform(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glGetActiveUniform_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glGetAttachedShaders_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetAttachedShaders, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLuint *,(__arg4),A1), \
        struct Library *, (__MesaBase), 570, Mesa)

#define glGetAttachedShaders(arg1, arg2, arg3, arg4) \
    __glGetAttachedShaders_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetAttribLocation_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLint, glGetAttribLocation, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLchar *,(__arg2),A0), \
        struct Library *, (__MesaBase), 571, Mesa)

#define glGetAttribLocation(arg1, arg2) \
    __glGetAttribLocation_WB(MesaBase, (arg1), (arg2))

#define __glGetProgramiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 572, Mesa)

#define glGetProgramiv(arg1, arg2, arg3) \
    __glGetProgramiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramInfoLog_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetProgramInfoLog, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLchar *,(__arg4),A1), \
        struct Library *, (__MesaBase), 573, Mesa)

#define glGetProgramInfoLog(arg1, arg2, arg3, arg4) \
    __glGetProgramInfoLog_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetShaderiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetShaderiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 574, Mesa)

#define glGetShaderiv(arg1, arg2, arg3) \
    __glGetShaderiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetShaderInfoLog_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetShaderInfoLog, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLchar *,(__arg4),A1), \
        struct Library *, (__MesaBase), 575, Mesa)

#define glGetShaderInfoLog(arg1, arg2, arg3, arg4) \
    __glGetShaderInfoLog_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetShaderSource_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetShaderSource, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLchar *,(__arg4),A1), \
        struct Library *, (__MesaBase), 576, Mesa)

#define glGetShaderSource(arg1, arg2, arg3, arg4) \
    __glGetShaderSource_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetUniformLocation_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLint, glGetUniformLocation, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLchar *,(__arg2),A0), \
        struct Library *, (__MesaBase), 577, Mesa)

#define glGetUniformLocation(arg1, arg2) \
    __glGetUniformLocation_WB(MesaBase, (arg1), (arg2))

#define __glGetUniformfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetUniformfv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 578, Mesa)

#define glGetUniformfv(arg1, arg2, arg3) \
    __glGetUniformfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetUniformiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetUniformiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 579, Mesa)

#define glGetUniformiv(arg1, arg2, arg3) \
    __glGetUniformiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribdv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribdv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 580, Mesa)

#define glGetVertexAttribdv(arg1, arg2, arg3) \
    __glGetVertexAttribdv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribfv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribfv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 581, Mesa)

#define glGetVertexAttribfv(arg1, arg2, arg3) \
    __glGetVertexAttribfv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribiv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 582, Mesa)

#define glGetVertexAttribiv(arg1, arg2, arg3) \
    __glGetVertexAttribiv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribPointerv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribPointerv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *  *,(__arg3),A0), \
        struct Library *, (__MesaBase), 583, Mesa)

#define glGetVertexAttribPointerv(arg1, arg2, arg3) \
    __glGetVertexAttribPointerv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsProgram_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsProgram, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 584, Mesa)

#define glIsProgram(arg1) \
    __glIsProgram_WB(MesaBase, (arg1))

#define __glIsShader_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsShader, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 585, Mesa)

#define glIsShader(arg1) \
    __glIsShader_WB(MesaBase, (arg1))

#define __glLinkProgram_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLinkProgram, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 586, Mesa)

#define glLinkProgram(arg1) \
    __glLinkProgram_WB(MesaBase, (arg1))

#define __glShaderSource_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glShaderSource, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLchar *  *,(__arg3),A0), \
                  AROS_LCA(const GLint *,(__arg4),A1), \
        struct Library *, (__MesaBase), 587, Mesa)

#define glShaderSource(arg1, arg2, arg3, arg4) \
    __glShaderSource_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUseProgram_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glUseProgram, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 588, Mesa)

#define glUseProgram(arg1) \
    __glUseProgram_WB(MesaBase, (arg1))

#define __glUniform1f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glUniform1f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 589, Mesa)

#define glUniform1f(arg1, arg2) \
    __glUniform1f_WB(MesaBase, (arg1), (arg2))

#define __glUniform2f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 590, Mesa)

#define glUniform2f(arg1, arg2, arg3) \
    __glUniform2f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniform3f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 591, Mesa)

#define glUniform3f(arg1, arg2, arg3, arg4) \
    __glUniform3f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniform4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glUniform4f, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 592, Mesa)

#define glUniform4f(arg1, arg2, arg3, arg4, arg5) \
    __glUniform4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glUniform1i_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glUniform1i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 593, Mesa)

#define glUniform1i(arg1, arg2) \
    __glUniform1i_WB(MesaBase, (arg1), (arg2))

#define __glUniform2i_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 594, Mesa)

#define glUniform2i(arg1, arg2, arg3) \
    __glUniform2i_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniform3i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 595, Mesa)

#define glUniform3i(arg1, arg2, arg3, arg4) \
    __glUniform3i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniform4i_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glUniform4i, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 596, Mesa)

#define glUniform4i(arg1, arg2, arg3, arg4, arg5) \
    __glUniform4i_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glUniform1fv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform1fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 597, Mesa)

#define glUniform1fv(arg1, arg2, arg3) \
    __glUniform1fv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform2fv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 598, Mesa)

#define glUniform2fv(arg1, arg2, arg3) \
    __glUniform2fv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3fv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform3fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 599, Mesa)

#define glUniform3fv(arg1, arg2, arg3) \
    __glUniform3fv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform4fv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform4fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 600, Mesa)

#define glUniform4fv(arg1, arg2, arg3) \
    __glUniform4fv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform1iv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform1iv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 601, Mesa)

#define glUniform1iv(arg1, arg2, arg3) \
    __glUniform1iv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform2iv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2iv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 602, Mesa)

#define glUniform2iv(arg1, arg2, arg3) \
    __glUniform2iv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3iv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform3iv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 603, Mesa)

#define glUniform3iv(arg1, arg2, arg3) \
    __glUniform3iv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform4iv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform4iv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 604, Mesa)

#define glUniform4iv(arg1, arg2, arg3) \
    __glUniform4iv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniformMatrix2fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix2fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 605, Mesa)

#define glUniformMatrix2fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix2fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix3fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix3fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 606, Mesa)

#define glUniformMatrix3fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix3fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix4fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix4fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 607, Mesa)

#define glUniformMatrix4fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix4fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glValidateProgram_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glValidateProgram, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 608, Mesa)

#define glValidateProgram(arg1) \
    __glValidateProgram_WB(MesaBase, (arg1))

#define __glVertexAttrib1d_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1d, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 609, Mesa)

#define glVertexAttrib1d(arg1, arg2) \
    __glVertexAttrib1d_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1dv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 610, Mesa)

#define glVertexAttrib1dv(arg1, arg2) \
    __glVertexAttrib1dv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1f_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1f, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 611, Mesa)

#define glVertexAttrib1f(arg1, arg2) \
    __glVertexAttrib1f_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1fv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 612, Mesa)

#define glVertexAttrib1fv(arg1, arg2) \
    __glVertexAttrib1fv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1s_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1s, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 613, Mesa)

#define glVertexAttrib1s(arg1, arg2) \
    __glVertexAttrib1s_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1sv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 614, Mesa)

#define glVertexAttrib1sv(arg1, arg2) \
    __glVertexAttrib1sv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2d_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2d, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 615, Mesa)

#define glVertexAttrib2d(arg1, arg2, arg3) \
    __glVertexAttrib2d_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2dv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 616, Mesa)

#define glVertexAttrib2dv(arg1, arg2) \
    __glVertexAttrib2dv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2f_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2f, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 617, Mesa)

#define glVertexAttrib2f(arg1, arg2, arg3) \
    __glVertexAttrib2f_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2fv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 618, Mesa)

#define glVertexAttrib2fv(arg1, arg2) \
    __glVertexAttrib2fv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2s_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2s, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 619, Mesa)

#define glVertexAttrib2s(arg1, arg2, arg3) \
    __glVertexAttrib2s_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2sv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 620, Mesa)

#define glVertexAttrib2sv(arg1, arg2) \
    __glVertexAttrib2sv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3d, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 621, Mesa)

#define glVertexAttrib3d(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3dv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 622, Mesa)

#define glVertexAttrib3dv(arg1, arg2) \
    __glVertexAttrib3dv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3f, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 623, Mesa)

#define glVertexAttrib3f(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3fv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 624, Mesa)

#define glVertexAttrib3fv(arg1, arg2) \
    __glVertexAttrib3fv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3s, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 625, Mesa)

#define glVertexAttrib3s(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3sv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 626, Mesa)

#define glVertexAttrib3sv(arg1, arg2) \
    __glVertexAttrib3sv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Nbv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Nbv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLbyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 627, Mesa)

#define glVertexAttrib4Nbv(arg1, arg2) \
    __glVertexAttrib4Nbv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Niv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Niv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 628, Mesa)

#define glVertexAttrib4Niv(arg1, arg2) \
    __glVertexAttrib4Niv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Nsv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Nsv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 629, Mesa)

#define glVertexAttrib4Nsv(arg1, arg2) \
    __glVertexAttrib4Nsv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Nub_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4Nub, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
                  AROS_LCA(GLubyte,(__arg4),D3), \
                  AROS_LCA(GLubyte,(__arg5),D4), \
        struct Library *, (__MesaBase), 630, Mesa)

#define glVertexAttrib4Nub(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4Nub_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4Nubv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Nubv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLubyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 631, Mesa)

#define glVertexAttrib4Nubv(arg1, arg2) \
    __glVertexAttrib4Nubv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Nuiv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Nuiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 632, Mesa)

#define glVertexAttrib4Nuiv(arg1, arg2) \
    __glVertexAttrib4Nuiv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4Nusv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4Nusv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLushort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 633, Mesa)

#define glVertexAttrib4Nusv(arg1, arg2) \
    __glVertexAttrib4Nusv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4bv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4bv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLbyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 634, Mesa)

#define glVertexAttrib4bv(arg1, arg2) \
    __glVertexAttrib4bv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4d_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4d, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
        struct Library *, (__MesaBase), 635, Mesa)

#define glVertexAttrib4d(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4d_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4dv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4dv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 636, Mesa)

#define glVertexAttrib4dv(arg1, arg2) \
    __glVertexAttrib4dv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4f_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4f, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 637, Mesa)

#define glVertexAttrib4f(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4f_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4fv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4fv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 638, Mesa)

#define glVertexAttrib4fv(arg1, arg2) \
    __glVertexAttrib4fv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4iv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4iv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 639, Mesa)

#define glVertexAttrib4iv(arg1, arg2) \
    __glVertexAttrib4iv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4s_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4s, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
                  AROS_LCA(GLshort,(__arg5),D4), \
        struct Library *, (__MesaBase), 640, Mesa)

#define glVertexAttrib4s(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4s_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4sv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4sv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 641, Mesa)

#define glVertexAttrib4sv(arg1, arg2) \
    __glVertexAttrib4sv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4ubv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4ubv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLubyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 642, Mesa)

#define glVertexAttrib4ubv(arg1, arg2) \
    __glVertexAttrib4ubv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4uiv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4uiv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 643, Mesa)

#define glVertexAttrib4uiv(arg1, arg2) \
    __glVertexAttrib4uiv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4usv_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4usv, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLushort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 644, Mesa)

#define glVertexAttrib4usv(arg1, arg2) \
    __glVertexAttrib4usv_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttribPointer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glVertexAttribPointer, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLboolean,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 645, Mesa)

#define glVertexAttribPointer(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glVertexAttribPointer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glUniformMatrix2x3fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix2x3fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 646, Mesa)

#define glUniformMatrix2x3fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix2x3fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix3x2fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix3x2fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 647, Mesa)

#define glUniformMatrix3x2fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix3x2fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix2x4fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix2x4fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 648, Mesa)

#define glUniformMatrix2x4fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix2x4fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix4x2fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix4x2fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 649, Mesa)

#define glUniformMatrix4x2fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix4x2fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix3x4fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix3x4fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 650, Mesa)

#define glUniformMatrix3x4fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix3x4fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix4x3fv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix4x3fv, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 651, Mesa)

#define glUniformMatrix4x3fv(arg1, arg2, arg3, arg4) \
    __glUniformMatrix4x3fv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glLoadTransposeMatrixfARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadTransposeMatrixfARB, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 652, Mesa)

#define glLoadTransposeMatrixfARB(arg1) \
    __glLoadTransposeMatrixfARB_WB(MesaBase, (arg1))

#define __glLoadTransposeMatrixdARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLoadTransposeMatrixdARB, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 653, Mesa)

#define glLoadTransposeMatrixdARB(arg1) \
    __glLoadTransposeMatrixdARB_WB(MesaBase, (arg1))

#define __glMultTransposeMatrixfARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultTransposeMatrixfARB, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 654, Mesa)

#define glMultTransposeMatrixfARB(arg1) \
    __glMultTransposeMatrixfARB_WB(MesaBase, (arg1))

#define __glMultTransposeMatrixdARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glMultTransposeMatrixdARB, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 655, Mesa)

#define glMultTransposeMatrixdARB(arg1) \
    __glMultTransposeMatrixdARB_WB(MesaBase, (arg1))

#define __glSampleCoverageARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glSampleCoverageARB, \
                  AROS_LCA(GLclampf,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
        struct Library *, (__MesaBase), 656, Mesa)

#define glSampleCoverageARB(arg1, arg2) \
    __glSampleCoverageARB_WB(MesaBase, (arg1), (arg2))

#define __glCompressedTexImage3DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCompressedTexImage3DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 657, Mesa)

#define glCompressedTexImage3DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCompressedTexImage3DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCompressedTexImage2DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCompressedTexImage2DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(const GLvoid *,(__arg8),A0), \
        struct Library *, (__MesaBase), 658, Mesa)

#define glCompressedTexImage2DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCompressedTexImage2DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glCompressedTexImage1DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCompressedTexImage1DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 659, Mesa)

#define glCompressedTexImage1DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCompressedTexImage1DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glCompressedTexSubImage3DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11) \
        AROS_LC11NR(void, glCompressedTexSubImage3DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(GLsizei,(__arg10),A1), \
                  AROS_LCA(const GLvoid *,(__arg11),A2), \
        struct Library *, (__MesaBase), 660, Mesa)

#define glCompressedTexSubImage3DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
    __glCompressedTexSubImage3DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11))

#define __glCompressedTexSubImage2DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCompressedTexSubImage2DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 661, Mesa)

#define glCompressedTexSubImage2DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCompressedTexSubImage2DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCompressedTexSubImage1DARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCompressedTexSubImage1DARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 662, Mesa)

#define glCompressedTexSubImage1DARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCompressedTexSubImage1DARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glGetCompressedTexImageARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetCompressedTexImageARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 663, Mesa)

#define glGetCompressedTexImageARB(arg1, arg2, arg3) \
    __glGetCompressedTexImageARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glPointParameterfARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterfARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 664, Mesa)

#define glPointParameterfARB(arg1, arg2) \
    __glPointParameterfARB_WB(MesaBase, (arg1), (arg2))

#define __glPointParameterfvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterfvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 665, Mesa)

#define glPointParameterfvARB(arg1, arg2) \
    __glPointParameterfvARB_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2dARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2dARB, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 666, Mesa)

#define glWindowPos2dARB(arg1, arg2) \
    __glWindowPos2dARB_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2dvARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2dvARB, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 667, Mesa)

#define glWindowPos2dvARB(arg1) \
    __glWindowPos2dvARB_WB(MesaBase, (arg1))

#define __glWindowPos2fARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2fARB, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 668, Mesa)

#define glWindowPos2fARB(arg1, arg2) \
    __glWindowPos2fARB_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2fvARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2fvARB, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 669, Mesa)

#define glWindowPos2fvARB(arg1) \
    __glWindowPos2fvARB_WB(MesaBase, (arg1))

#define __glWindowPos2iARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 670, Mesa)

#define glWindowPos2iARB(arg1, arg2) \
    __glWindowPos2iARB_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2ivARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2ivARB, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 671, Mesa)

#define glWindowPos2ivARB(arg1) \
    __glWindowPos2ivARB_WB(MesaBase, (arg1))

#define __glWindowPos2sARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2sARB, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 672, Mesa)

#define glWindowPos2sARB(arg1, arg2) \
    __glWindowPos2sARB_WB(MesaBase, (arg1), (arg2))

#define __glWindowPos2svARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2svARB, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 673, Mesa)

#define glWindowPos2svARB(arg1) \
    __glWindowPos2svARB_WB(MesaBase, (arg1))

#define __glWindowPos3dARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3dARB, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 674, Mesa)

#define glWindowPos3dARB(arg1, arg2, arg3) \
    __glWindowPos3dARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3dvARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3dvARB, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 675, Mesa)

#define glWindowPos3dvARB(arg1) \
    __glWindowPos3dvARB_WB(MesaBase, (arg1))

#define __glWindowPos3fARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3fARB, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 676, Mesa)

#define glWindowPos3fARB(arg1, arg2, arg3) \
    __glWindowPos3fARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3fvARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3fvARB, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 677, Mesa)

#define glWindowPos3fvARB(arg1) \
    __glWindowPos3fvARB_WB(MesaBase, (arg1))

#define __glWindowPos3iARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 678, Mesa)

#define glWindowPos3iARB(arg1, arg2, arg3) \
    __glWindowPos3iARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3ivARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3ivARB, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 679, Mesa)

#define glWindowPos3ivARB(arg1) \
    __glWindowPos3ivARB_WB(MesaBase, (arg1))

#define __glWindowPos3sARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3sARB, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 680, Mesa)

#define glWindowPos3sARB(arg1, arg2, arg3) \
    __glWindowPos3sARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWindowPos3svARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3svARB, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 681, Mesa)

#define glWindowPos3svARB(arg1) \
    __glWindowPos3svARB_WB(MesaBase, (arg1))

#define __glVertexAttrib1dARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1dARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 682, Mesa)

#define glVertexAttrib1dARB(arg1, arg2) \
    __glVertexAttrib1dARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1dvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 683, Mesa)

#define glVertexAttrib1dvARB(arg1, arg2) \
    __glVertexAttrib1dvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1fARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1fARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 684, Mesa)

#define glVertexAttrib1fARB(arg1, arg2) \
    __glVertexAttrib1fARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1fvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 685, Mesa)

#define glVertexAttrib1fvARB(arg1, arg2) \
    __glVertexAttrib1fvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1sARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1sARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 686, Mesa)

#define glVertexAttrib1sARB(arg1, arg2) \
    __glVertexAttrib1sARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1svARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 687, Mesa)

#define glVertexAttrib1svARB(arg1, arg2) \
    __glVertexAttrib1svARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2dARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2dARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 688, Mesa)

#define glVertexAttrib2dARB(arg1, arg2, arg3) \
    __glVertexAttrib2dARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2dvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 689, Mesa)

#define glVertexAttrib2dvARB(arg1, arg2) \
    __glVertexAttrib2dvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2fARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2fARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 690, Mesa)

#define glVertexAttrib2fARB(arg1, arg2, arg3) \
    __glVertexAttrib2fARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2fvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 691, Mesa)

#define glVertexAttrib2fvARB(arg1, arg2) \
    __glVertexAttrib2fvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2sARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2sARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 692, Mesa)

#define glVertexAttrib2sARB(arg1, arg2, arg3) \
    __glVertexAttrib2sARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2svARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 693, Mesa)

#define glVertexAttrib2svARB(arg1, arg2) \
    __glVertexAttrib2svARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3dARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 694, Mesa)

#define glVertexAttrib3dARB(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3dvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 695, Mesa)

#define glVertexAttrib3dvARB(arg1, arg2) \
    __glVertexAttrib3dvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3fARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 696, Mesa)

#define glVertexAttrib3fARB(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3fvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 697, Mesa)

#define glVertexAttrib3fvARB(arg1, arg2) \
    __glVertexAttrib3fvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3sARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3sARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 698, Mesa)

#define glVertexAttrib3sARB(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3sARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3svARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 699, Mesa)

#define glVertexAttrib3svARB(arg1, arg2) \
    __glVertexAttrib3svARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NbvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NbvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLbyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 700, Mesa)

#define glVertexAttrib4NbvARB(arg1, arg2) \
    __glVertexAttrib4NbvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 701, Mesa)

#define glVertexAttrib4NivARB(arg1, arg2) \
    __glVertexAttrib4NivARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NsvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NsvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 702, Mesa)

#define glVertexAttrib4NsvARB(arg1, arg2) \
    __glVertexAttrib4NsvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NubARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4NubARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
                  AROS_LCA(GLubyte,(__arg4),D3), \
                  AROS_LCA(GLubyte,(__arg5),D4), \
        struct Library *, (__MesaBase), 703, Mesa)

#define glVertexAttrib4NubARB(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4NubARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4NubvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NubvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLubyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 704, Mesa)

#define glVertexAttrib4NubvARB(arg1, arg2) \
    __glVertexAttrib4NubvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NuivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NuivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 705, Mesa)

#define glVertexAttrib4NuivARB(arg1, arg2) \
    __glVertexAttrib4NuivARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4NusvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4NusvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLushort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 706, Mesa)

#define glVertexAttrib4NusvARB(arg1, arg2) \
    __glVertexAttrib4NusvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4bvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4bvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLbyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 707, Mesa)

#define glVertexAttrib4bvARB(arg1, arg2) \
    __glVertexAttrib4bvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4dARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
        struct Library *, (__MesaBase), 708, Mesa)

#define glVertexAttrib4dARB(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4dvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4dvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 709, Mesa)

#define glVertexAttrib4dvARB(arg1, arg2) \
    __glVertexAttrib4dvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4fARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 710, Mesa)

#define glVertexAttrib4fARB(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4fvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4fvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 711, Mesa)

#define glVertexAttrib4fvARB(arg1, arg2) \
    __glVertexAttrib4fvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4ivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4ivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 712, Mesa)

#define glVertexAttrib4ivARB(arg1, arg2) \
    __glVertexAttrib4ivARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4sARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4sARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
                  AROS_LCA(GLshort,(__arg5),D4), \
        struct Library *, (__MesaBase), 713, Mesa)

#define glVertexAttrib4sARB(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4sARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4svARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4svARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 714, Mesa)

#define glVertexAttrib4svARB(arg1, arg2) \
    __glVertexAttrib4svARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4ubvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4ubvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLubyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 715, Mesa)

#define glVertexAttrib4ubvARB(arg1, arg2) \
    __glVertexAttrib4ubvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4uivARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4uivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 716, Mesa)

#define glVertexAttrib4uivARB(arg1, arg2) \
    __glVertexAttrib4uivARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4usvARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4usvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLushort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 717, Mesa)

#define glVertexAttrib4usvARB(arg1, arg2) \
    __glVertexAttrib4usvARB_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttribPointerARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glVertexAttribPointerARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLboolean,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 718, Mesa)

#define glVertexAttribPointerARB(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glVertexAttribPointerARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glEnableVertexAttribArrayARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEnableVertexAttribArrayARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 719, Mesa)

#define glEnableVertexAttribArrayARB(arg1) \
    __glEnableVertexAttribArrayARB_WB(MesaBase, (arg1))

#define __glDisableVertexAttribArrayARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDisableVertexAttribArrayARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 720, Mesa)

#define glDisableVertexAttribArrayARB(arg1) \
    __glDisableVertexAttribArrayARB_WB(MesaBase, (arg1))

#define __glProgramStringARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glProgramStringARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 721, Mesa)

#define glProgramStringARB(arg1, arg2, arg3, arg4) \
    __glProgramStringARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glBindProgramARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindProgramARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 722, Mesa)

#define glBindProgramARB(arg1, arg2) \
    __glBindProgramARB_WB(MesaBase, (arg1), (arg2))

#define __glDeleteProgramsARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteProgramsARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 723, Mesa)

#define glDeleteProgramsARB(arg1, arg2) \
    __glDeleteProgramsARB_WB(MesaBase, (arg1), (arg2))

#define __glGenProgramsARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenProgramsARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 724, Mesa)

#define glGenProgramsARB(arg1, arg2) \
    __glGenProgramsARB_WB(MesaBase, (arg1), (arg2))

#define __glProgramEnvParameter4dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramEnvParameter4dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 725, Mesa)

#define glProgramEnvParameter4dARB(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramEnvParameter4dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramEnvParameter4dvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramEnvParameter4dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 726, Mesa)

#define glProgramEnvParameter4dvARB(arg1, arg2, arg3) \
    __glProgramEnvParameter4dvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glProgramEnvParameter4fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramEnvParameter4fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
        struct Library *, (__MesaBase), 727, Mesa)

#define glProgramEnvParameter4fARB(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramEnvParameter4fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramEnvParameter4fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramEnvParameter4fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 728, Mesa)

#define glProgramEnvParameter4fvARB(arg1, arg2, arg3) \
    __glProgramEnvParameter4fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glProgramLocalParameter4dARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramLocalParameter4dARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 729, Mesa)

#define glProgramLocalParameter4dARB(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramLocalParameter4dARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramLocalParameter4dvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramLocalParameter4dvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 730, Mesa)

#define glProgramLocalParameter4dvARB(arg1, arg2, arg3) \
    __glProgramLocalParameter4dvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glProgramLocalParameter4fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramLocalParameter4fARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
        struct Library *, (__MesaBase), 731, Mesa)

#define glProgramLocalParameter4fARB(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramLocalParameter4fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramLocalParameter4fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramLocalParameter4fvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 732, Mesa)

#define glProgramLocalParameter4fvARB(arg1, arg2, arg3) \
    __glProgramLocalParameter4fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramEnvParameterdvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramEnvParameterdvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 733, Mesa)

#define glGetProgramEnvParameterdvARB(arg1, arg2, arg3) \
    __glGetProgramEnvParameterdvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramEnvParameterfvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramEnvParameterfvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 734, Mesa)

#define glGetProgramEnvParameterfvARB(arg1, arg2, arg3) \
    __glGetProgramEnvParameterfvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramLocalParameterdvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramLocalParameterdvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 735, Mesa)

#define glGetProgramLocalParameterdvARB(arg1, arg2, arg3) \
    __glGetProgramLocalParameterdvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramLocalParameterfvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramLocalParameterfvARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 736, Mesa)

#define glGetProgramLocalParameterfvARB(arg1, arg2, arg3) \
    __glGetProgramLocalParameterfvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 737, Mesa)

#define glGetProgramivARB(arg1, arg2, arg3) \
    __glGetProgramivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramStringARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramStringARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 738, Mesa)

#define glGetProgramStringARB(arg1, arg2, arg3) \
    __glGetProgramStringARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribdvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribdvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 739, Mesa)

#define glGetVertexAttribdvARB(arg1, arg2, arg3) \
    __glGetVertexAttribdvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribfvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribfvARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 740, Mesa)

#define glGetVertexAttribfvARB(arg1, arg2, arg3) \
    __glGetVertexAttribfvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 741, Mesa)

#define glGetVertexAttribivARB(arg1, arg2, arg3) \
    __glGetVertexAttribivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribPointervARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribPointervARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *  *,(__arg3),A0), \
        struct Library *, (__MesaBase), 742, Mesa)

#define glGetVertexAttribPointervARB(arg1, arg2, arg3) \
    __glGetVertexAttribPointervARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsProgramARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsProgramARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 743, Mesa)

#define glIsProgramARB(arg1) \
    __glIsProgramARB_WB(MesaBase, (arg1))

#define __glBindBufferARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindBufferARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 744, Mesa)

#define glBindBufferARB(arg1, arg2) \
    __glBindBufferARB_WB(MesaBase, (arg1), (arg2))

#define __glDeleteBuffersARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteBuffersARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 745, Mesa)

#define glDeleteBuffersARB(arg1, arg2) \
    __glDeleteBuffersARB_WB(MesaBase, (arg1), (arg2))

#define __glGenBuffersARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenBuffersARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 746, Mesa)

#define glGenBuffersARB(arg1, arg2) \
    __glGenBuffersARB_WB(MesaBase, (arg1), (arg2))

#define __glIsBufferARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsBufferARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 747, Mesa)

#define glIsBufferARB(arg1) \
    __glIsBufferARB_WB(MesaBase, (arg1))

#define __glBufferDataARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBufferDataARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizeiptrARB,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
                  AROS_LCA(GLenum,(__arg4),D2), \
        struct Library *, (__MesaBase), 748, Mesa)

#define glBufferDataARB(arg1, arg2, arg3, arg4) \
    __glBufferDataARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glBufferSubDataARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBufferSubDataARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptrARB,(__arg2),D1), \
                  AROS_LCA(GLsizeiptrARB,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 749, Mesa)

#define glBufferSubDataARB(arg1, arg2, arg3, arg4) \
    __glBufferSubDataARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetBufferSubDataARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetBufferSubDataARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptrARB,(__arg2),D1), \
                  AROS_LCA(GLsizeiptrARB,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 750, Mesa)

#define glGetBufferSubDataARB(arg1, arg2, arg3, arg4) \
    __glGetBufferSubDataARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMapBufferARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLvoid*, glMapBufferARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 751, Mesa)

#define glMapBufferARB(arg1, arg2) \
    __glMapBufferARB_WB(MesaBase, (arg1), (arg2))

#define __glUnmapBufferARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glUnmapBufferARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 752, Mesa)

#define glUnmapBufferARB(arg1) \
    __glUnmapBufferARB_WB(MesaBase, (arg1))

#define __glGetBufferParameterivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetBufferParameterivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 753, Mesa)

#define glGetBufferParameterivARB(arg1, arg2, arg3) \
    __glGetBufferParameterivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetBufferPointervARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetBufferPointervARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *  *,(__arg3),A0), \
        struct Library *, (__MesaBase), 754, Mesa)

#define glGetBufferPointervARB(arg1, arg2, arg3) \
    __glGetBufferPointervARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGenQueriesARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenQueriesARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 755, Mesa)

#define glGenQueriesARB(arg1, arg2) \
    __glGenQueriesARB_WB(MesaBase, (arg1), (arg2))

#define __glDeleteQueriesARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteQueriesARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 756, Mesa)

#define glDeleteQueriesARB(arg1, arg2) \
    __glDeleteQueriesARB_WB(MesaBase, (arg1), (arg2))

#define __glIsQueryARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsQueryARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 757, Mesa)

#define glIsQueryARB(arg1) \
    __glIsQueryARB_WB(MesaBase, (arg1))

#define __glBeginQueryARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBeginQueryARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 758, Mesa)

#define glBeginQueryARB(arg1, arg2) \
    __glBeginQueryARB_WB(MesaBase, (arg1), (arg2))

#define __glEndQueryARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glEndQueryARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 759, Mesa)

#define glEndQueryARB(arg1) \
    __glEndQueryARB_WB(MesaBase, (arg1))

#define __glGetQueryivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryivARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 760, Mesa)

#define glGetQueryivARB(arg1, arg2, arg3) \
    __glGetQueryivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetQueryObjectivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryObjectivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 761, Mesa)

#define glGetQueryObjectivARB(arg1, arg2, arg3) \
    __glGetQueryObjectivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetQueryObjectuivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetQueryObjectuivARB, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLuint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 762, Mesa)

#define glGetQueryObjectuivARB(arg1, arg2, arg3) \
    __glGetQueryObjectuivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glDeleteObjectARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDeleteObjectARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
        struct Library *, (__MesaBase), 763, Mesa)

#define glDeleteObjectARB(arg1) \
    __glDeleteObjectARB_WB(MesaBase, (arg1))

#define __glGetHandleARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLhandleARB, glGetHandleARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 764, Mesa)

#define glGetHandleARB(arg1) \
    __glGetHandleARB_WB(MesaBase, (arg1))

#define __glDetachObjectARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDetachObjectARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLhandleARB,(__arg2),D1), \
        struct Library *, (__MesaBase), 765, Mesa)

#define glDetachObjectARB(arg1, arg2) \
    __glDetachObjectARB_WB(MesaBase, (arg1), (arg2))

#define __glCreateShaderObjectARB_WB(__MesaBase, __arg1) \
        AROS_LC1(GLhandleARB, glCreateShaderObjectARB, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 766, Mesa)

#define glCreateShaderObjectARB(arg1) \
    __glCreateShaderObjectARB_WB(MesaBase, (arg1))

#define __glShaderSourceARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glShaderSourceARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLcharARB *  *,(__arg3),A0), \
                  AROS_LCA(const GLint *,(__arg4),A1), \
        struct Library *, (__MesaBase), 767, Mesa)

#define glShaderSourceARB(arg1, arg2, arg3, arg4) \
    __glShaderSourceARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glCompileShaderARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glCompileShaderARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
        struct Library *, (__MesaBase), 768, Mesa)

#define glCompileShaderARB(arg1) \
    __glCompileShaderARB_WB(MesaBase, (arg1))

#define __glCreateProgramObjectARB_WB(__MesaBase) \
        AROS_LC0(GLhandleARB, glCreateProgramObjectARB, \
        struct Library *, (__MesaBase), 769, Mesa)

#define glCreateProgramObjectARB() \
    __glCreateProgramObjectARB_WB(MesaBase)

#define __glAttachObjectARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glAttachObjectARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLhandleARB,(__arg2),D1), \
        struct Library *, (__MesaBase), 770, Mesa)

#define glAttachObjectARB(arg1, arg2) \
    __glAttachObjectARB_WB(MesaBase, (arg1), (arg2))

#define __glLinkProgramARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glLinkProgramARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
        struct Library *, (__MesaBase), 771, Mesa)

#define glLinkProgramARB(arg1) \
    __glLinkProgramARB_WB(MesaBase, (arg1))

#define __glUseProgramObjectARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glUseProgramObjectARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
        struct Library *, (__MesaBase), 772, Mesa)

#define glUseProgramObjectARB(arg1) \
    __glUseProgramObjectARB_WB(MesaBase, (arg1))

#define __glValidateProgramARB_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glValidateProgramARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
        struct Library *, (__MesaBase), 773, Mesa)

#define glValidateProgramARB(arg1) \
    __glValidateProgramARB_WB(MesaBase, (arg1))

#define __glUniform1fARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glUniform1fARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 774, Mesa)

#define glUniform1fARB(arg1, arg2) \
    __glUniform1fARB_WB(MesaBase, (arg1), (arg2))

#define __glUniform2fARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2fARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 775, Mesa)

#define glUniform2fARB(arg1, arg2, arg3) \
    __glUniform2fARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniform3fARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 776, Mesa)

#define glUniform3fARB(arg1, arg2, arg3, arg4) \
    __glUniform3fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniform4fARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glUniform4fARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 777, Mesa)

#define glUniform4fARB(arg1, arg2, arg3, arg4, arg5) \
    __glUniform4fARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glUniform1iARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glUniform1iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 778, Mesa)

#define glUniform1iARB(arg1, arg2) \
    __glUniform1iARB_WB(MesaBase, (arg1), (arg2))

#define __glUniform2iARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 779, Mesa)

#define glUniform2iARB(arg1, arg2, arg3) \
    __glUniform2iARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3iARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniform3iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 780, Mesa)

#define glUniform3iARB(arg1, arg2, arg3, arg4) \
    __glUniform3iARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniform4iARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glUniform4iARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 781, Mesa)

#define glUniform4iARB(arg1, arg2, arg3, arg4, arg5) \
    __glUniform4iARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glUniform1fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform1fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 782, Mesa)

#define glUniform1fvARB(arg1, arg2, arg3) \
    __glUniform1fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform2fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 783, Mesa)

#define glUniform2fvARB(arg1, arg2, arg3) \
    __glUniform2fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform3fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 784, Mesa)

#define glUniform3fvARB(arg1, arg2, arg3) \
    __glUniform3fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform4fvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform4fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 785, Mesa)

#define glUniform4fvARB(arg1, arg2, arg3) \
    __glUniform4fvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform1ivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform1ivARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 786, Mesa)

#define glUniform1ivARB(arg1, arg2, arg3) \
    __glUniform1ivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform2ivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform2ivARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 787, Mesa)

#define glUniform2ivARB(arg1, arg2, arg3) \
    __glUniform2ivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform3ivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform3ivARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 788, Mesa)

#define glUniform3ivARB(arg1, arg2, arg3) \
    __glUniform3ivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniform4ivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glUniform4ivARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 789, Mesa)

#define glUniform4ivARB(arg1, arg2, arg3) \
    __glUniform4ivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glUniformMatrix2fvARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix2fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 790, Mesa)

#define glUniformMatrix2fvARB(arg1, arg2, arg3, arg4) \
    __glUniformMatrix2fvARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix3fvARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix3fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 791, Mesa)

#define glUniformMatrix3fvARB(arg1, arg2, arg3, arg4) \
    __glUniformMatrix3fvARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glUniformMatrix4fvARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glUniformMatrix4fvARB, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 792, Mesa)

#define glUniformMatrix4fvARB(arg1, arg2, arg3, arg4) \
    __glUniformMatrix4fvARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetObjectParameterfvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetObjectParameterfvARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 793, Mesa)

#define glGetObjectParameterfvARB(arg1, arg2, arg3) \
    __glGetObjectParameterfvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetObjectParameterivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetObjectParameterivARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 794, Mesa)

#define glGetObjectParameterivARB(arg1, arg2, arg3) \
    __glGetObjectParameterivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetInfoLogARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetInfoLogARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLcharARB *,(__arg4),A1), \
        struct Library *, (__MesaBase), 795, Mesa)

#define glGetInfoLogARB(arg1, arg2, arg3, arg4) \
    __glGetInfoLogARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetAttachedObjectsARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetAttachedObjectsARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLhandleARB *,(__arg4),A1), \
        struct Library *, (__MesaBase), 796, Mesa)

#define glGetAttachedObjectsARB(arg1, arg2, arg3, arg4) \
    __glGetAttachedObjectsARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetUniformLocationARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLint, glGetUniformLocationARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(const GLcharARB *,(__arg2),A0), \
        struct Library *, (__MesaBase), 797, Mesa)

#define glGetUniformLocationARB(arg1, arg2) \
    __glGetUniformLocationARB_WB(MesaBase, (arg1), (arg2))

#define __glGetActiveUniformARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glGetActiveUniformARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei *,(__arg4),A0), \
                  AROS_LCA(GLint *,(__arg5),A1), \
                  AROS_LCA(GLenum *,(__arg6),A2), \
                  AROS_LCA(GLcharARB *,(__arg7),A3), \
        struct Library *, (__MesaBase), 798, Mesa)

#define glGetActiveUniformARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glGetActiveUniformARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glGetUniformfvARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetUniformfvARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 799, Mesa)

#define glGetUniformfvARB(arg1, arg2, arg3) \
    __glGetUniformfvARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetUniformivARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetUniformivARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 800, Mesa)

#define glGetUniformivARB(arg1, arg2, arg3) \
    __glGetUniformivARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetShaderSourceARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetShaderSourceARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei *,(__arg3),A0), \
                  AROS_LCA(GLcharARB *,(__arg4),A1), \
        struct Library *, (__MesaBase), 801, Mesa)

#define glGetShaderSourceARB(arg1, arg2, arg3, arg4) \
    __glGetShaderSourceARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glBindAttribLocationARB_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glBindAttribLocationARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLcharARB *,(__arg3),A0), \
        struct Library *, (__MesaBase), 802, Mesa)

#define glBindAttribLocationARB(arg1, arg2, arg3) \
    __glBindAttribLocationARB_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetActiveAttribARB_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glGetActiveAttribARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei *,(__arg4),A0), \
                  AROS_LCA(GLint *,(__arg5),A1), \
                  AROS_LCA(GLenum *,(__arg6),A2), \
                  AROS_LCA(GLcharARB *,(__arg7),A3), \
        struct Library *, (__MesaBase), 803, Mesa)

#define glGetActiveAttribARB(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glGetActiveAttribARB_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glGetAttribLocationARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLint, glGetAttribLocationARB, \
                  AROS_LCA(GLhandleARB,(__arg1),D0), \
                  AROS_LCA(const GLcharARB *,(__arg2),A0), \
        struct Library *, (__MesaBase), 804, Mesa)

#define glGetAttribLocationARB(arg1, arg2) \
    __glGetAttribLocationARB_WB(MesaBase, (arg1), (arg2))

#define __glDrawBuffersARB_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDrawBuffersARB, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLenum *,(__arg2),A0), \
        struct Library *, (__MesaBase), 805, Mesa)

#define glDrawBuffersARB(arg1, arg2) \
    __glDrawBuffersARB_WB(MesaBase, (arg1), (arg2))

#define __glIsRenderbuffer_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsRenderbuffer, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 806, Mesa)

#define glIsRenderbuffer(arg1) \
    __glIsRenderbuffer_WB(MesaBase, (arg1))

#define __glBindRenderbuffer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindRenderbuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 807, Mesa)

#define glBindRenderbuffer(arg1, arg2) \
    __glBindRenderbuffer_WB(MesaBase, (arg1), (arg2))

#define __glDeleteRenderbuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteRenderbuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 808, Mesa)

#define glDeleteRenderbuffers(arg1, arg2) \
    __glDeleteRenderbuffers_WB(MesaBase, (arg1), (arg2))

#define __glGenRenderbuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenRenderbuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 809, Mesa)

#define glGenRenderbuffers(arg1, arg2) \
    __glGenRenderbuffers_WB(MesaBase, (arg1), (arg2))

#define __glRenderbufferStorage_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRenderbufferStorage, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
        struct Library *, (__MesaBase), 810, Mesa)

#define glRenderbufferStorage(arg1, arg2, arg3, arg4) \
    __glRenderbufferStorage_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetRenderbufferParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetRenderbufferParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 811, Mesa)

#define glGetRenderbufferParameteriv(arg1, arg2, arg3) \
    __glGetRenderbufferParameteriv_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsFramebuffer_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsFramebuffer, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 812, Mesa)

#define glIsFramebuffer(arg1) \
    __glIsFramebuffer_WB(MesaBase, (arg1))

#define __glBindFramebuffer_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindFramebuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 813, Mesa)

#define glBindFramebuffer(arg1, arg2) \
    __glBindFramebuffer_WB(MesaBase, (arg1), (arg2))

#define __glDeleteFramebuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteFramebuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 814, Mesa)

#define glDeleteFramebuffers(arg1, arg2) \
    __glDeleteFramebuffers_WB(MesaBase, (arg1), (arg2))

#define __glGenFramebuffers_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenFramebuffers, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 815, Mesa)

#define glGenFramebuffers(arg1, arg2) \
    __glGenFramebuffers_WB(MesaBase, (arg1), (arg2))

#define __glCheckFramebufferStatus_WB(__MesaBase, __arg1) \
        AROS_LC1(GLenum, glCheckFramebufferStatus, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 816, Mesa)

#define glCheckFramebufferStatus(arg1) \
    __glCheckFramebufferStatus_WB(MesaBase, (arg1))

#define __glFramebufferTexture1D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTexture1D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 817, Mesa)

#define glFramebufferTexture1D(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTexture1D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFramebufferTexture2D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTexture2D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 818, Mesa)

#define glFramebufferTexture2D(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTexture2D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFramebufferTexture3D_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glFramebufferTexture3D, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
        struct Library *, (__MesaBase), 819, Mesa)

#define glFramebufferTexture3D(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glFramebufferTexture3D_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glFramebufferRenderbuffer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glFramebufferRenderbuffer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
        struct Library *, (__MesaBase), 820, Mesa)

#define glFramebufferRenderbuffer(arg1, arg2, arg3, arg4) \
    __glFramebufferRenderbuffer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetFramebufferAttachmentParameteriv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetFramebufferAttachmentParameteriv, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 821, Mesa)

#define glGetFramebufferAttachmentParameteriv(arg1, arg2, arg3, arg4) \
    __glGetFramebufferAttachmentParameteriv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGenerateMipmap_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glGenerateMipmap, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 822, Mesa)

#define glGenerateMipmap(arg1) \
    __glGenerateMipmap_WB(MesaBase, (arg1))

#define __glBlitFramebuffer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glBlitFramebuffer, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLint,(__arg8),D7), \
                  AROS_LCA(GLbitfield,(__arg9),A0), \
                  AROS_LCA(GLenum,(__arg10),A1), \
        struct Library *, (__MesaBase), 823, Mesa)

#define glBlitFramebuffer(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glBlitFramebuffer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glRenderbufferStorageMultisample_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glRenderbufferStorageMultisample, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
        struct Library *, (__MesaBase), 824, Mesa)

#define glRenderbufferStorageMultisample(arg1, arg2, arg3, arg4, arg5) \
    __glRenderbufferStorageMultisample_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFramebufferTextureLayer_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTextureLayer, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 825, Mesa)

#define glFramebufferTextureLayer(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTextureLayer_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glBlendColorEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBlendColorEXT, \
                  AROS_LCA(GLclampf,(__arg1),D0), \
                  AROS_LCA(GLclampf,(__arg2),D1), \
                  AROS_LCA(GLclampf,(__arg3),D2), \
                  AROS_LCA(GLclampf,(__arg4),D3), \
        struct Library *, (__MesaBase), 826, Mesa)

#define glBlendColorEXT(arg1, arg2, arg3, arg4) \
    __glBlendColorEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glPolygonOffsetEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPolygonOffsetEXT, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 827, Mesa)

#define glPolygonOffsetEXT(arg1, arg2) \
    __glPolygonOffsetEXT_WB(MesaBase, (arg1), (arg2))

#define __glTexImage3DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glTexImage3DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLenum,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(const GLvoid *,(__arg10),A1), \
        struct Library *, (__MesaBase), 828, Mesa)

#define glTexImage3DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glTexImage3DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glTexSubImage3DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11) \
        AROS_LC11NR(void, glTexSubImage3DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLenum,(__arg9),A0), \
                  AROS_LCA(GLenum,(__arg10),A1), \
                  AROS_LCA(const GLvoid *,(__arg11),A2), \
        struct Library *, (__MesaBase), 829, Mesa)

#define glTexSubImage3DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11) \
    __glTexSubImage3DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11))

#define __glTexSubImage1DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glTexSubImage1DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(const GLvoid *,(__arg7),A0), \
        struct Library *, (__MesaBase), 830, Mesa)

#define glTexSubImage1DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glTexSubImage1DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glTexSubImage2DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glTexSubImage2DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLenum,(__arg8),D7), \
                  AROS_LCA(const GLvoid *,(__arg9),A0), \
        struct Library *, (__MesaBase), 831, Mesa)

#define glTexSubImage2DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glTexSubImage2DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glCopyTexImage1DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glCopyTexImage1DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
        struct Library *, (__MesaBase), 832, Mesa)

#define glCopyTexImage1DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glCopyTexImage1DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glCopyTexImage2DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCopyTexImage2DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLint,(__arg8),D7), \
        struct Library *, (__MesaBase), 833, Mesa)

#define glCopyTexImage2DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCopyTexImage2DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glCopyTexSubImage1DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glCopyTexSubImage1DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLsizei,(__arg6),D5), \
        struct Library *, (__MesaBase), 834, Mesa)

#define glCopyTexSubImage1DEXT(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glCopyTexSubImage1DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glCopyTexSubImage2DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8) \
        AROS_LC8NR(void, glCopyTexSubImage2DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLsizei,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
        struct Library *, (__MesaBase), 835, Mesa)

#define glCopyTexSubImage2DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __glCopyTexSubImage2DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8))

#define __glCopyTexSubImage3DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glCopyTexSubImage3DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
                  AROS_LCA(GLint,(__arg7),D6), \
                  AROS_LCA(GLsizei,(__arg8),D7), \
                  AROS_LCA(GLsizei,(__arg9),A0), \
        struct Library *, (__MesaBase), 836, Mesa)

#define glCopyTexSubImage3DEXT(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glCopyTexSubImage3DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glAreTexturesResidentEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLboolean, glAreTexturesResidentEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
                  AROS_LCA(GLboolean *,(__arg3),A1), \
        struct Library *, (__MesaBase), 837, Mesa)

#define glAreTexturesResidentEXT(arg1, arg2, arg3) \
    __glAreTexturesResidentEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBindTextureEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindTextureEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 838, Mesa)

#define glBindTextureEXT(arg1, arg2) \
    __glBindTextureEXT_WB(MesaBase, (arg1), (arg2))

#define __glDeleteTexturesEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteTexturesEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 839, Mesa)

#define glDeleteTexturesEXT(arg1, arg2) \
    __glDeleteTexturesEXT_WB(MesaBase, (arg1), (arg2))

#define __glGenTexturesEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenTexturesEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 840, Mesa)

#define glGenTexturesEXT(arg1, arg2) \
    __glGenTexturesEXT_WB(MesaBase, (arg1), (arg2))

#define __glIsTextureEXT_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsTextureEXT, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 841, Mesa)

#define glIsTextureEXT(arg1) \
    __glIsTextureEXT_WB(MesaBase, (arg1))

#define __glPrioritizeTexturesEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPrioritizeTexturesEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
                  AROS_LCA(const GLclampf *,(__arg3),A1), \
        struct Library *, (__MesaBase), 842, Mesa)

#define glPrioritizeTexturesEXT(arg1, arg2, arg3) \
    __glPrioritizeTexturesEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glArrayElementEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glArrayElementEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
        struct Library *, (__MesaBase), 843, Mesa)

#define glArrayElementEXT(arg1) \
    __glArrayElementEXT_WB(MesaBase, (arg1))

#define __glColorPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glColorPointerEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(const GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 844, Mesa)

#define glColorPointerEXT(arg1, arg2, arg3, arg4, arg5) \
    __glColorPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glDrawArraysEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glDrawArraysEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
        struct Library *, (__MesaBase), 845, Mesa)

#define glDrawArraysEXT(arg1, arg2, arg3) \
    __glDrawArraysEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glEdgeFlagPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glEdgeFlagPointerEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLboolean *,(__arg3),A0), \
        struct Library *, (__MesaBase), 846, Mesa)

#define glEdgeFlagPointerEXT(arg1, arg2, arg3) \
    __glEdgeFlagPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetPointervEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetPointervEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLvoid *  *,(__arg2),A0), \
        struct Library *, (__MesaBase), 847, Mesa)

#define glGetPointervEXT(arg1, arg2) \
    __glGetPointervEXT_WB(MesaBase, (arg1), (arg2))

#define __glIndexPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glIndexPointerEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 848, Mesa)

#define glIndexPointerEXT(arg1, arg2, arg3, arg4) \
    __glIndexPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glNormalPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glNormalPointerEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 849, Mesa)

#define glNormalPointerEXT(arg1, arg2, arg3, arg4) \
    __glNormalPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glTexCoordPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glTexCoordPointerEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(const GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 850, Mesa)

#define glTexCoordPointerEXT(arg1, arg2, arg3, arg4, arg5) \
    __glTexCoordPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexPointerEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(const GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 851, Mesa)

#define glVertexPointerEXT(arg1, arg2, arg3, arg4, arg5) \
    __glVertexPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glBlendEquationEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glBlendEquationEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 852, Mesa)

#define glBlendEquationEXT(arg1) \
    __glBlendEquationEXT_WB(MesaBase, (arg1))

#define __glPointParameterfEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterfEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 853, Mesa)

#define glPointParameterfEXT(arg1, arg2) \
    __glPointParameterfEXT_WB(MesaBase, (arg1), (arg2))

#define __glPointParameterfvEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterfvEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 854, Mesa)

#define glPointParameterfvEXT(arg1, arg2) \
    __glPointParameterfvEXT_WB(MesaBase, (arg1), (arg2))

#define __glColorTableEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glColorTableEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 855, Mesa)

#define glColorTableEXT(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glColorTableEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glGetColorTableEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetColorTableEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 856, Mesa)

#define glGetColorTableEXT(arg1, arg2, arg3, arg4) \
    __glGetColorTableEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetColorTableParameterivEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetColorTableParameterivEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 857, Mesa)

#define glGetColorTableParameterivEXT(arg1, arg2, arg3) \
    __glGetColorTableParameterivEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetColorTableParameterfvEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetColorTableParameterfvEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 858, Mesa)

#define glGetColorTableParameterfvEXT(arg1, arg2, arg3) \
    __glGetColorTableParameterfvEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glLockArraysEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glLockArraysEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
        struct Library *, (__MesaBase), 859, Mesa)

#define glLockArraysEXT(arg1, arg2) \
    __glLockArraysEXT_WB(MesaBase, (arg1), (arg2))

#define __glUnlockArraysEXT_WB(__MesaBase) \
        AROS_LC0NR(void, glUnlockArraysEXT, \
        struct Library *, (__MesaBase), 860, Mesa)

#define glUnlockArraysEXT() \
    __glUnlockArraysEXT_WB(MesaBase)

#define __glDrawRangeElementsEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glDrawRangeElementsEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
        struct Library *, (__MesaBase), 861, Mesa)

#define glDrawRangeElementsEXT(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glDrawRangeElementsEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glSecondaryColor3bEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3bEXT, \
                  AROS_LCA(GLbyte,(__arg1),D0), \
                  AROS_LCA(GLbyte,(__arg2),D1), \
                  AROS_LCA(GLbyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 862, Mesa)

#define glSecondaryColor3bEXT(arg1, arg2, arg3) \
    __glSecondaryColor3bEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3bvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3bvEXT, \
                  AROS_LCA(const GLbyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 863, Mesa)

#define glSecondaryColor3bvEXT(arg1) \
    __glSecondaryColor3bvEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3dEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3dEXT, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 864, Mesa)

#define glSecondaryColor3dEXT(arg1, arg2, arg3) \
    __glSecondaryColor3dEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3dvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3dvEXT, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 865, Mesa)

#define glSecondaryColor3dvEXT(arg1) \
    __glSecondaryColor3dvEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3fEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3fEXT, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 866, Mesa)

#define glSecondaryColor3fEXT(arg1, arg2, arg3) \
    __glSecondaryColor3fEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3fvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3fvEXT, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 867, Mesa)

#define glSecondaryColor3fvEXT(arg1) \
    __glSecondaryColor3fvEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3iEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3iEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 868, Mesa)

#define glSecondaryColor3iEXT(arg1, arg2, arg3) \
    __glSecondaryColor3iEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3ivEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3ivEXT, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 869, Mesa)

#define glSecondaryColor3ivEXT(arg1) \
    __glSecondaryColor3ivEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3sEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3sEXT, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 870, Mesa)

#define glSecondaryColor3sEXT(arg1, arg2, arg3) \
    __glSecondaryColor3sEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3svEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3svEXT, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 871, Mesa)

#define glSecondaryColor3svEXT(arg1) \
    __glSecondaryColor3svEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3ubEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3ubEXT, \
                  AROS_LCA(GLubyte,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
        struct Library *, (__MesaBase), 872, Mesa)

#define glSecondaryColor3ubEXT(arg1, arg2, arg3) \
    __glSecondaryColor3ubEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3ubvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3ubvEXT, \
                  AROS_LCA(const GLubyte *,(__arg1),A0), \
        struct Library *, (__MesaBase), 873, Mesa)

#define glSecondaryColor3ubvEXT(arg1) \
    __glSecondaryColor3ubvEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3uiEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3uiEXT, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
        struct Library *, (__MesaBase), 874, Mesa)

#define glSecondaryColor3uiEXT(arg1, arg2, arg3) \
    __glSecondaryColor3uiEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3uivEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3uivEXT, \
                  AROS_LCA(const GLuint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 875, Mesa)

#define glSecondaryColor3uivEXT(arg1) \
    __glSecondaryColor3uivEXT_WB(MesaBase, (arg1))

#define __glSecondaryColor3usEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSecondaryColor3usEXT, \
                  AROS_LCA(GLushort,(__arg1),D0), \
                  AROS_LCA(GLushort,(__arg2),D1), \
                  AROS_LCA(GLushort,(__arg3),D2), \
        struct Library *, (__MesaBase), 876, Mesa)

#define glSecondaryColor3usEXT(arg1, arg2, arg3) \
    __glSecondaryColor3usEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSecondaryColor3usvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glSecondaryColor3usvEXT, \
                  AROS_LCA(const GLushort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 877, Mesa)

#define glSecondaryColor3usvEXT(arg1) \
    __glSecondaryColor3usvEXT_WB(MesaBase, (arg1))

#define __glSecondaryColorPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glSecondaryColorPointerEXT, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
        struct Library *, (__MesaBase), 878, Mesa)

#define glSecondaryColorPointerEXT(arg1, arg2, arg3, arg4) \
    __glSecondaryColorPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiDrawArraysEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glMultiDrawArraysEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint *,(__arg2),A0), \
                  AROS_LCA(GLsizei *,(__arg3),A1), \
                  AROS_LCA(GLsizei,(__arg4),D1), \
        struct Library *, (__MesaBase), 879, Mesa)

#define glMultiDrawArraysEXT(arg1, arg2, arg3, arg4) \
    __glMultiDrawArraysEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glMultiDrawElementsEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glMultiDrawElementsEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLsizei *,(__arg2),A0), \
                  AROS_LCA(GLenum,(__arg3),D1), \
                  AROS_LCA(const GLvoid *  *,(__arg4),A1), \
                  AROS_LCA(GLsizei,(__arg5),D2), \
        struct Library *, (__MesaBase), 880, Mesa)

#define glMultiDrawElementsEXT(arg1, arg2, arg3, arg4, arg5) \
    __glMultiDrawElementsEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFogCoordfEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoordfEXT, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
        struct Library *, (__MesaBase), 881, Mesa)

#define glFogCoordfEXT(arg1) \
    __glFogCoordfEXT_WB(MesaBase, (arg1))

#define __glFogCoordfvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoordfvEXT, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 882, Mesa)

#define glFogCoordfvEXT(arg1) \
    __glFogCoordfvEXT_WB(MesaBase, (arg1))

#define __glFogCoorddEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoorddEXT, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
        struct Library *, (__MesaBase), 883, Mesa)

#define glFogCoorddEXT(arg1) \
    __glFogCoorddEXT_WB(MesaBase, (arg1))

#define __glFogCoorddvEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glFogCoorddvEXT, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 884, Mesa)

#define glFogCoorddvEXT(arg1) \
    __glFogCoorddvEXT_WB(MesaBase, (arg1))

#define __glFogCoordPointerEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glFogCoordPointerEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLvoid *,(__arg3),A0), \
        struct Library *, (__MesaBase), 885, Mesa)

#define glFogCoordPointerEXT(arg1, arg2, arg3) \
    __glFogCoordPointerEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBlendFuncSeparateEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glBlendFuncSeparateEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
        struct Library *, (__MesaBase), 886, Mesa)

#define glBlendFuncSeparateEXT(arg1, arg2, arg3, arg4) \
    __glBlendFuncSeparateEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glFlushVertexArrayRangeNV_WB(__MesaBase) \
        AROS_LC0NR(void, glFlushVertexArrayRangeNV, \
        struct Library *, (__MesaBase), 887, Mesa)

#define glFlushVertexArrayRangeNV() \
    __glFlushVertexArrayRangeNV_WB(MesaBase)

#define __glVertexArrayRangeNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexArrayRangeNV, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLvoid *,(__arg2),A0), \
        struct Library *, (__MesaBase), 888, Mesa)

#define glVertexArrayRangeNV(arg1, arg2) \
    __glVertexArrayRangeNV_WB(MesaBase, (arg1), (arg2))

#define __glCombinerParameterfvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glCombinerParameterfvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 889, Mesa)

#define glCombinerParameterfvNV(arg1, arg2) \
    __glCombinerParameterfvNV_WB(MesaBase, (arg1), (arg2))

#define __glCombinerParameterfNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glCombinerParameterfNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 890, Mesa)

#define glCombinerParameterfNV(arg1, arg2) \
    __glCombinerParameterfNV_WB(MesaBase, (arg1), (arg2))

#define __glCombinerParameterivNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glCombinerParameterivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 891, Mesa)

#define glCombinerParameterivNV(arg1, arg2) \
    __glCombinerParameterivNV_WB(MesaBase, (arg1), (arg2))

#define __glCombinerParameteriNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glCombinerParameteriNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 892, Mesa)

#define glCombinerParameteriNV(arg1, arg2) \
    __glCombinerParameteriNV_WB(MesaBase, (arg1), (arg2))

#define __glCombinerInputNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glCombinerInputNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
        struct Library *, (__MesaBase), 893, Mesa)

#define glCombinerInputNV(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glCombinerInputNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glCombinerOutputNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glCombinerOutputNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(GLenum,(__arg6),D5), \
                  AROS_LCA(GLenum,(__arg7),D6), \
                  AROS_LCA(GLboolean,(__arg8),D7), \
                  AROS_LCA(GLboolean,(__arg9),A0), \
                  AROS_LCA(GLboolean,(__arg10),A1), \
        struct Library *, (__MesaBase), 894, Mesa)

#define glCombinerOutputNV(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glCombinerOutputNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glFinalCombinerInputNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glFinalCombinerInputNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
        struct Library *, (__MesaBase), 895, Mesa)

#define glFinalCombinerInputNV(arg1, arg2, arg3, arg4) \
    __glFinalCombinerInputNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetCombinerInputParameterfvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetCombinerInputParameterfvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLfloat *,(__arg5),A0), \
        struct Library *, (__MesaBase), 896, Mesa)

#define glGetCombinerInputParameterfvNV(arg1, arg2, arg3, arg4, arg5) \
    __glGetCombinerInputParameterfvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetCombinerInputParameterivNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetCombinerInputParameterivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
                  AROS_LCA(GLint *,(__arg5),A0), \
        struct Library *, (__MesaBase), 897, Mesa)

#define glGetCombinerInputParameterivNV(arg1, arg2, arg3, arg4, arg5) \
    __glGetCombinerInputParameterivNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetCombinerOutputParameterfvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetCombinerOutputParameterfvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 898, Mesa)

#define glGetCombinerOutputParameterfvNV(arg1, arg2, arg3, arg4) \
    __glGetCombinerOutputParameterfvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetCombinerOutputParameterivNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetCombinerOutputParameterivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 899, Mesa)

#define glGetCombinerOutputParameterivNV(arg1, arg2, arg3, arg4) \
    __glGetCombinerOutputParameterivNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetFinalCombinerInputParameterfvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetFinalCombinerInputParameterfvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 900, Mesa)

#define glGetFinalCombinerInputParameterfvNV(arg1, arg2, arg3) \
    __glGetFinalCombinerInputParameterfvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetFinalCombinerInputParameterivNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetFinalCombinerInputParameterivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 901, Mesa)

#define glGetFinalCombinerInputParameterivNV(arg1, arg2, arg3) \
    __glGetFinalCombinerInputParameterivNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glResizeBuffersMESA_WB(__MesaBase) \
        AROS_LC0NR(void, glResizeBuffersMESA, \
        struct Library *, (__MesaBase), 902, Mesa)

#define glResizeBuffersMESA() \
    __glResizeBuffersMESA_WB(MesaBase)

#define __glWindowPos2dMESA_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2dMESA, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 903, Mesa)

#define glWindowPos2dMESA(arg1, arg2) \
    __glWindowPos2dMESA_WB(MesaBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2dMES(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2dMESA((arg1), (GLdouble)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2dvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2dvMESA, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 904, Mesa)

#define glWindowPos2dvMESA(arg1) \
    __glWindowPos2dvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2dvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2dvMESA((const GLdouble *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2fMESA_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2fMESA, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 905, Mesa)

#define glWindowPos2fMESA(arg1, arg2) \
    __glWindowPos2fMESA_WB(MesaBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2fMES(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2fMESA((arg1), (GLfloat)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2fvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2fvMESA, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 906, Mesa)

#define glWindowPos2fvMESA(arg1) \
    __glWindowPos2fvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2fvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2fvMESA((const GLfloat *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2iMESA_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2iMESA, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 907, Mesa)

#define glWindowPos2iMESA(arg1, arg2) \
    __glWindowPos2iMESA_WB(MesaBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2iMES(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2iMESA((arg1), (GLint)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2ivMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2ivMESA, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 908, Mesa)

#define glWindowPos2ivMESA(arg1) \
    __glWindowPos2ivMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2ivMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2ivMESA((const GLint *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2sMESA_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glWindowPos2sMESA, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 909, Mesa)

#define glWindowPos2sMESA(arg1, arg2) \
    __glWindowPos2sMESA_WB(MesaBase, (arg1), (arg2))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2sMES(arg1, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2sMESA((arg1), (GLshort)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos2svMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos2svMESA, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 910, Mesa)

#define glWindowPos2svMESA(arg1) \
    __glWindowPos2svMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos2svMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos2svMESA((const GLshort *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3dMESA_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3dMESA, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 911, Mesa)

#define glWindowPos3dMESA(arg1, arg2, arg3) \
    __glWindowPos3dMESA_WB(MesaBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3dMES(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3dMESA((arg1), (arg2), (GLdouble)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3dvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3dvMESA, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 912, Mesa)

#define glWindowPos3dvMESA(arg1) \
    __glWindowPos3dvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3dvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3dvMESA((const GLdouble *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3fMESA_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3fMESA, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 913, Mesa)

#define glWindowPos3fMESA(arg1, arg2, arg3) \
    __glWindowPos3fMESA_WB(MesaBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3fMES(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3fMESA((arg1), (arg2), (GLfloat)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3fvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3fvMESA, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 914, Mesa)

#define glWindowPos3fvMESA(arg1) \
    __glWindowPos3fvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3fvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3fvMESA((const GLfloat *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3iMESA_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3iMESA, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
        struct Library *, (__MesaBase), 915, Mesa)

#define glWindowPos3iMESA(arg1, arg2, arg3) \
    __glWindowPos3iMESA_WB(MesaBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3iMES(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3iMESA((arg1), (arg2), (GLint)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3ivMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3ivMESA, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 916, Mesa)

#define glWindowPos3ivMESA(arg1) \
    __glWindowPos3ivMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3ivMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3ivMESA((const GLint *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3sMESA_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWindowPos3sMESA, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 917, Mesa)

#define glWindowPos3sMESA(arg1, arg2, arg3) \
    __glWindowPos3sMESA_WB(MesaBase, (arg1), (arg2), (arg3))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3sMES(arg1, arg2, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3sMESA((arg1), (arg2), (GLshort)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos3svMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos3svMESA, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 918, Mesa)

#define glWindowPos3svMESA(arg1) \
    __glWindowPos3svMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos3svMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos3svMESA((const GLshort *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4dMESA_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glWindowPos4dMESA, \
                  AROS_LCA(GLdouble,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 919, Mesa)

#define glWindowPos4dMESA(arg1, arg2, arg3, arg4) \
    __glWindowPos4dMESA_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4dMES(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4dMESA((arg1), (arg2), (arg3), (GLdouble)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4dvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos4dvMESA, \
                  AROS_LCA(const GLdouble *,(__arg1),A0), \
        struct Library *, (__MesaBase), 920, Mesa)

#define glWindowPos4dvMESA(arg1) \
    __glWindowPos4dvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4dvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4dvMESA((const GLdouble *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4fMESA_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glWindowPos4fMESA, \
                  AROS_LCA(GLfloat,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 921, Mesa)

#define glWindowPos4fMESA(arg1, arg2, arg3, arg4) \
    __glWindowPos4fMESA_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4fMES(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4fMESA((arg1), (arg2), (arg3), (GLfloat)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4fvMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos4fvMESA, \
                  AROS_LCA(const GLfloat *,(__arg1),A0), \
        struct Library *, (__MesaBase), 922, Mesa)

#define glWindowPos4fvMESA(arg1) \
    __glWindowPos4fvMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4fvMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4fvMESA((const GLfloat *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4iMESA_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glWindowPos4iMESA, \
                  AROS_LCA(GLint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
        struct Library *, (__MesaBase), 923, Mesa)

#define glWindowPos4iMESA(arg1, arg2, arg3, arg4) \
    __glWindowPos4iMESA_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4iMES(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4iMESA((arg1), (arg2), (arg3), (GLint)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4ivMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos4ivMESA, \
                  AROS_LCA(const GLint *,(__arg1),A0), \
        struct Library *, (__MesaBase), 924, Mesa)

#define glWindowPos4ivMESA(arg1) \
    __glWindowPos4ivMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4ivMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4ivMESA((const GLint *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4sMESA_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glWindowPos4sMESA, \
                  AROS_LCA(GLshort,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 925, Mesa)

#define glWindowPos4sMESA(arg1, arg2, arg3, arg4) \
    __glWindowPos4sMESA_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4sMES(arg1, arg2, arg3, ...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4sMESA((arg1), (arg2), (arg3), (GLshort)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glWindowPos4svMESA_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glWindowPos4svMESA, \
                  AROS_LCA(const GLshort *,(__arg1),A0), \
        struct Library *, (__MesaBase), 926, Mesa)

#define glWindowPos4svMESA(arg1) \
    __glWindowPos4svMESA_WB(MesaBase, (arg1))

#if !defined(NO_INLINE_STDARG) && !defined(MESA_NO_INLINE_STDARG)
#define glWindowPos4svMES(...) \
({ \
    IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
    glWindowPos4svMESA((const GLshort *)__args); \
})
#endif /* !NO_INLINE_STDARG */

#define __glAreProgramsResidentNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLboolean, glAreProgramsResidentNV, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
                  AROS_LCA(GLboolean *,(__arg3),A1), \
        struct Library *, (__MesaBase), 927, Mesa)

#define glAreProgramsResidentNV(arg1, arg2, arg3) \
    __glAreProgramsResidentNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBindProgramNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindProgramNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 928, Mesa)

#define glBindProgramNV(arg1, arg2) \
    __glBindProgramNV_WB(MesaBase, (arg1), (arg2))

#define __glDeleteProgramsNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteProgramsNV, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 929, Mesa)

#define glDeleteProgramsNV(arg1, arg2) \
    __glDeleteProgramsNV_WB(MesaBase, (arg1), (arg2))

#define __glExecuteProgramNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glExecuteProgramNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 930, Mesa)

#define glExecuteProgramNV(arg1, arg2, arg3) \
    __glExecuteProgramNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGenProgramsNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenProgramsNV, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 931, Mesa)

#define glGenProgramsNV(arg1, arg2) \
    __glGenProgramsNV_WB(MesaBase, (arg1), (arg2))

#define __glGetProgramParameterdvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetProgramParameterdvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLdouble *,(__arg4),A0), \
        struct Library *, (__MesaBase), 932, Mesa)

#define glGetProgramParameterdvNV(arg1, arg2, arg3, arg4) \
    __glGetProgramParameterdvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetProgramParameterfvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetProgramParameterfvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 933, Mesa)

#define glGetProgramParameterfvNV(arg1, arg2, arg3, arg4) \
    __glGetProgramParameterfvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetProgramivNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramivNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 934, Mesa)

#define glGetProgramivNV(arg1, arg2, arg3) \
    __glGetProgramivNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetProgramStringNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetProgramStringNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLubyte *,(__arg3),A0), \
        struct Library *, (__MesaBase), 935, Mesa)

#define glGetProgramStringNV(arg1, arg2, arg3) \
    __glGetProgramStringNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetTrackMatrixivNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetTrackMatrixivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 936, Mesa)

#define glGetTrackMatrixivNV(arg1, arg2, arg3, arg4) \
    __glGetTrackMatrixivNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetVertexAttribdvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribdvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 937, Mesa)

#define glGetVertexAttribdvNV(arg1, arg2, arg3) \
    __glGetVertexAttribdvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribfvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribfvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 938, Mesa)

#define glGetVertexAttribfvNV(arg1, arg2, arg3) \
    __glGetVertexAttribfvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribivNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribivNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 939, Mesa)

#define glGetVertexAttribivNV(arg1, arg2, arg3) \
    __glGetVertexAttribivNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetVertexAttribPointervNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetVertexAttribPointervNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLvoid *  *,(__arg3),A0), \
        struct Library *, (__MesaBase), 940, Mesa)

#define glGetVertexAttribPointervNV(arg1, arg2, arg3) \
    __glGetVertexAttribPointervNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsProgramNV_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsProgramNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 941, Mesa)

#define glIsProgramNV(arg1) \
    __glIsProgramNV_WB(MesaBase, (arg1))

#define __glLoadProgramNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glLoadProgramNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(const GLubyte *,(__arg4),A0), \
        struct Library *, (__MesaBase), 942, Mesa)

#define glLoadProgramNV(arg1, arg2, arg3, arg4) \
    __glLoadProgramNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glProgramParameter4dNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramParameter4dNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
                  AROS_LCA(GLdouble,(__arg6),D5), \
        struct Library *, (__MesaBase), 943, Mesa)

#define glProgramParameter4dNV(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramParameter4dNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramParameter4dvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramParameter4dvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 944, Mesa)

#define glProgramParameter4dvNV(arg1, arg2, arg3) \
    __glProgramParameter4dvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glProgramParameter4fNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glProgramParameter4fNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
                  AROS_LCA(GLfloat,(__arg6),D5), \
        struct Library *, (__MesaBase), 945, Mesa)

#define glProgramParameter4fNV(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glProgramParameter4fNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProgramParameter4fvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glProgramParameter4fvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 946, Mesa)

#define glProgramParameter4fvNV(arg1, arg2, arg3) \
    __glProgramParameter4fvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glProgramParameters4dvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glProgramParameters4dvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(const GLdouble *,(__arg4),A0), \
        struct Library *, (__MesaBase), 947, Mesa)

#define glProgramParameters4dvNV(arg1, arg2, arg3, arg4) \
    __glProgramParameters4dvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glProgramParameters4fvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glProgramParameters4fvNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(const GLfloat *,(__arg4),A0), \
        struct Library *, (__MesaBase), 948, Mesa)

#define glProgramParameters4fvNV(arg1, arg2, arg3, arg4) \
    __glProgramParameters4fvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glRequestResidentProgramsNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glRequestResidentProgramsNV, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 949, Mesa)

#define glRequestResidentProgramsNV(arg1, arg2) \
    __glRequestResidentProgramsNV_WB(MesaBase, (arg1), (arg2))

#define __glTrackMatrixNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glTrackMatrixNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLenum,(__arg4),D3), \
        struct Library *, (__MesaBase), 950, Mesa)

#define glTrackMatrixNV(arg1, arg2, arg3, arg4) \
    __glTrackMatrixNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttribPointerNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttribPointerNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(const GLvoid *,(__arg5),A0), \
        struct Library *, (__MesaBase), 951, Mesa)

#define glVertexAttribPointerNV(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttribPointerNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib1dNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1dNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
        struct Library *, (__MesaBase), 952, Mesa)

#define glVertexAttrib1dNV(arg1, arg2) \
    __glVertexAttrib1dNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1dvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 953, Mesa)

#define glVertexAttrib1dvNV(arg1, arg2) \
    __glVertexAttrib1dvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1fNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1fNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
        struct Library *, (__MesaBase), 954, Mesa)

#define glVertexAttrib1fNV(arg1, arg2) \
    __glVertexAttrib1fNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1fvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 955, Mesa)

#define glVertexAttrib1fvNV(arg1, arg2) \
    __glVertexAttrib1fvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1sNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1sNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
        struct Library *, (__MesaBase), 956, Mesa)

#define glVertexAttrib1sNV(arg1, arg2) \
    __glVertexAttrib1sNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib1svNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib1svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 957, Mesa)

#define glVertexAttrib1svNV(arg1, arg2) \
    __glVertexAttrib1svNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2dNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2dNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
        struct Library *, (__MesaBase), 958, Mesa)

#define glVertexAttrib2dNV(arg1, arg2, arg3) \
    __glVertexAttrib2dNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2dvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 959, Mesa)

#define glVertexAttrib2dvNV(arg1, arg2) \
    __glVertexAttrib2dvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2fNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2fNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
        struct Library *, (__MesaBase), 960, Mesa)

#define glVertexAttrib2fNV(arg1, arg2, arg3) \
    __glVertexAttrib2fNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2fvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 961, Mesa)

#define glVertexAttrib2fvNV(arg1, arg2) \
    __glVertexAttrib2fvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib2sNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttrib2sNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
        struct Library *, (__MesaBase), 962, Mesa)

#define glVertexAttrib2sNV(arg1, arg2, arg3) \
    __glVertexAttrib2sNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttrib2svNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib2svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 963, Mesa)

#define glVertexAttrib2svNV(arg1, arg2) \
    __glVertexAttrib2svNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3dNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3dNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
        struct Library *, (__MesaBase), 964, Mesa)

#define glVertexAttrib3dNV(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3dNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3dvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 965, Mesa)

#define glVertexAttrib3dvNV(arg1, arg2) \
    __glVertexAttrib3dvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3fNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3fNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
        struct Library *, (__MesaBase), 966, Mesa)

#define glVertexAttrib3fNV(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3fNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3fvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 967, Mesa)

#define glVertexAttrib3fvNV(arg1, arg2) \
    __glVertexAttrib3fvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib3sNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glVertexAttrib3sNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
        struct Library *, (__MesaBase), 968, Mesa)

#define glVertexAttrib3sNV(arg1, arg2, arg3, arg4) \
    __glVertexAttrib3sNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glVertexAttrib3svNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib3svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 969, Mesa)

#define glVertexAttrib3svNV(arg1, arg2) \
    __glVertexAttrib3svNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4dNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4dNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLdouble,(__arg2),D1), \
                  AROS_LCA(GLdouble,(__arg3),D2), \
                  AROS_LCA(GLdouble,(__arg4),D3), \
                  AROS_LCA(GLdouble,(__arg5),D4), \
        struct Library *, (__MesaBase), 970, Mesa)

#define glVertexAttrib4dNV(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4dNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4dvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLdouble *,(__arg2),A0), \
        struct Library *, (__MesaBase), 971, Mesa)

#define glVertexAttrib4dvNV(arg1, arg2) \
    __glVertexAttrib4dvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4fNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4fNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLfloat,(__arg2),D1), \
                  AROS_LCA(GLfloat,(__arg3),D2), \
                  AROS_LCA(GLfloat,(__arg4),D3), \
                  AROS_LCA(GLfloat,(__arg5),D4), \
        struct Library *, (__MesaBase), 972, Mesa)

#define glVertexAttrib4fNV(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4fNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4fvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 973, Mesa)

#define glVertexAttrib4fvNV(arg1, arg2) \
    __glVertexAttrib4fvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4sNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4sNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLshort,(__arg2),D1), \
                  AROS_LCA(GLshort,(__arg3),D2), \
                  AROS_LCA(GLshort,(__arg4),D3), \
                  AROS_LCA(GLshort,(__arg5),D4), \
        struct Library *, (__MesaBase), 974, Mesa)

#define glVertexAttrib4sNV(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4sNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4svNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLshort *,(__arg2),A0), \
        struct Library *, (__MesaBase), 975, Mesa)

#define glVertexAttrib4svNV(arg1, arg2) \
    __glVertexAttrib4svNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttrib4ubNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glVertexAttrib4ubNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLubyte,(__arg2),D1), \
                  AROS_LCA(GLubyte,(__arg3),D2), \
                  AROS_LCA(GLubyte,(__arg4),D3), \
                  AROS_LCA(GLubyte,(__arg5),D4), \
        struct Library *, (__MesaBase), 976, Mesa)

#define glVertexAttrib4ubNV(arg1, arg2, arg3, arg4, arg5) \
    __glVertexAttrib4ubNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glVertexAttrib4ubvNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glVertexAttrib4ubvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLubyte *,(__arg2),A0), \
        struct Library *, (__MesaBase), 977, Mesa)

#define glVertexAttrib4ubvNV(arg1, arg2) \
    __glVertexAttrib4ubvNV_WB(MesaBase, (arg1), (arg2))

#define __glVertexAttribs1dvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs1dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 978, Mesa)

#define glVertexAttribs1dvNV(arg1, arg2, arg3) \
    __glVertexAttribs1dvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs1fvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs1fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 979, Mesa)

#define glVertexAttribs1fvNV(arg1, arg2, arg3) \
    __glVertexAttribs1fvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs1svNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs1svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLshort *,(__arg3),A0), \
        struct Library *, (__MesaBase), 980, Mesa)

#define glVertexAttribs1svNV(arg1, arg2, arg3) \
    __glVertexAttribs1svNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs2dvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs2dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 981, Mesa)

#define glVertexAttribs2dvNV(arg1, arg2, arg3) \
    __glVertexAttribs2dvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs2fvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs2fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 982, Mesa)

#define glVertexAttribs2fvNV(arg1, arg2, arg3) \
    __glVertexAttribs2fvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs2svNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs2svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLshort *,(__arg3),A0), \
        struct Library *, (__MesaBase), 983, Mesa)

#define glVertexAttribs2svNV(arg1, arg2, arg3) \
    __glVertexAttribs2svNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs3dvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs3dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 984, Mesa)

#define glVertexAttribs3dvNV(arg1, arg2, arg3) \
    __glVertexAttribs3dvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs3fvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs3fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 985, Mesa)

#define glVertexAttribs3fvNV(arg1, arg2, arg3) \
    __glVertexAttribs3fvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs3svNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs3svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLshort *,(__arg3),A0), \
        struct Library *, (__MesaBase), 986, Mesa)

#define glVertexAttribs3svNV(arg1, arg2, arg3) \
    __glVertexAttribs3svNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs4dvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs4dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLdouble *,(__arg3),A0), \
        struct Library *, (__MesaBase), 987, Mesa)

#define glVertexAttribs4dvNV(arg1, arg2, arg3) \
    __glVertexAttribs4dvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs4fvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs4fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLfloat *,(__arg3),A0), \
        struct Library *, (__MesaBase), 988, Mesa)

#define glVertexAttribs4fvNV(arg1, arg2, arg3) \
    __glVertexAttribs4fvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs4svNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs4svNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLshort *,(__arg3),A0), \
        struct Library *, (__MesaBase), 989, Mesa)

#define glVertexAttribs4svNV(arg1, arg2, arg3) \
    __glVertexAttribs4svNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glVertexAttribs4ubvNV_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glVertexAttribs4ubvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
        struct Library *, (__MesaBase), 990, Mesa)

#define glVertexAttribs4ubvNV(arg1, arg2, arg3) \
    __glVertexAttribs4ubvNV_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glTexBumpParameterivATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexBumpParameterivATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 991, Mesa)

#define glTexBumpParameterivATI(arg1, arg2) \
    __glTexBumpParameterivATI_WB(MesaBase, (arg1), (arg2))

#define __glTexBumpParameterfvATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glTexBumpParameterfvATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 992, Mesa)

#define glTexBumpParameterfvATI(arg1, arg2) \
    __glTexBumpParameterfvATI_WB(MesaBase, (arg1), (arg2))

#define __glGetTexBumpParameterivATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetTexBumpParameterivATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 993, Mesa)

#define glGetTexBumpParameterivATI(arg1, arg2) \
    __glGetTexBumpParameterivATI_WB(MesaBase, (arg1), (arg2))

#define __glGetTexBumpParameterfvATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetTexBumpParameterfvATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 994, Mesa)

#define glGetTexBumpParameterfvATI(arg1, arg2) \
    __glGetTexBumpParameterfvATI_WB(MesaBase, (arg1), (arg2))

#define __glGenFragmentShadersATI_WB(__MesaBase, __arg1) \
        AROS_LC1(GLuint, glGenFragmentShadersATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 995, Mesa)

#define glGenFragmentShadersATI(arg1) \
    __glGenFragmentShadersATI_WB(MesaBase, (arg1))

#define __glBindFragmentShaderATI_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glBindFragmentShaderATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 996, Mesa)

#define glBindFragmentShaderATI(arg1) \
    __glBindFragmentShaderATI_WB(MesaBase, (arg1))

#define __glDeleteFragmentShaderATI_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDeleteFragmentShaderATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 997, Mesa)

#define glDeleteFragmentShaderATI(arg1) \
    __glDeleteFragmentShaderATI_WB(MesaBase, (arg1))

#define __glBeginFragmentShaderATI_WB(__MesaBase) \
        AROS_LC0NR(void, glBeginFragmentShaderATI, \
        struct Library *, (__MesaBase), 998, Mesa)

#define glBeginFragmentShaderATI() \
    __glBeginFragmentShaderATI_WB(MesaBase)

#define __glEndFragmentShaderATI_WB(__MesaBase) \
        AROS_LC0NR(void, glEndFragmentShaderATI, \
        struct Library *, (__MesaBase), 999, Mesa)

#define glEndFragmentShaderATI() \
    __glEndFragmentShaderATI_WB(MesaBase)

#define __glPassTexCoordATI_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glPassTexCoordATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
        struct Library *, (__MesaBase), 1000, Mesa)

#define glPassTexCoordATI(arg1, arg2, arg3) \
    __glPassTexCoordATI_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glSampleMapATI_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glSampleMapATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
        struct Library *, (__MesaBase), 1001, Mesa)

#define glSampleMapATI(arg1, arg2, arg3) \
    __glSampleMapATI_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glColorFragmentOp1ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glColorFragmentOp1ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
                  AROS_LCA(GLuint,(__arg7),D6), \
        struct Library *, (__MesaBase), 1002, Mesa)

#define glColorFragmentOp1ATI(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glColorFragmentOp1ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glColorFragmentOp2ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10) \
        AROS_LC10NR(void, glColorFragmentOp2ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
                  AROS_LCA(GLuint,(__arg7),D6), \
                  AROS_LCA(GLuint,(__arg8),D7), \
                  AROS_LCA(GLuint,(__arg9),A0), \
                  AROS_LCA(GLuint,(__arg10),A1), \
        struct Library *, (__MesaBase), 1003, Mesa)

#define glColorFragmentOp2ATI(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10) \
    __glColorFragmentOp2ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10))

#define __glColorFragmentOp3ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11, __arg12, __arg13) \
        AROS_LC13NR(void, glColorFragmentOp3ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
                  AROS_LCA(GLuint,(__arg7),D6), \
                  AROS_LCA(GLuint,(__arg8),D7), \
                  AROS_LCA(GLuint,(__arg9),A0), \
                  AROS_LCA(GLuint,(__arg10),A1), \
                  AROS_LCA(GLuint,(__arg11),A2), \
                  AROS_LCA(GLuint,(__arg12),A3), \
                  AROS_LCA(GLuint,(__arg13),A4), \
        struct Library *, (__MesaBase), 1004, Mesa)

#define glColorFragmentOp3ATI(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13) \
    __glColorFragmentOp3ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11), (arg12), (arg13))

#define __glAlphaFragmentOp1ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glAlphaFragmentOp1ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
        struct Library *, (__MesaBase), 1005, Mesa)

#define glAlphaFragmentOp1ATI(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glAlphaFragmentOp1ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glAlphaFragmentOp2ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9) \
        AROS_LC9NR(void, glAlphaFragmentOp2ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
                  AROS_LCA(GLuint,(__arg7),D6), \
                  AROS_LCA(GLuint,(__arg8),D7), \
                  AROS_LCA(GLuint,(__arg9),A0), \
        struct Library *, (__MesaBase), 1006, Mesa)

#define glAlphaFragmentOp2ATI(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __glAlphaFragmentOp2ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9))

#define __glAlphaFragmentOp3ATI_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7, __arg8, __arg9, __arg10, __arg11, __arg12) \
        AROS_LC12NR(void, glAlphaFragmentOp3ATI, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLuint,(__arg5),D4), \
                  AROS_LCA(GLuint,(__arg6),D5), \
                  AROS_LCA(GLuint,(__arg7),D6), \
                  AROS_LCA(GLuint,(__arg8),D7), \
                  AROS_LCA(GLuint,(__arg9),A0), \
                  AROS_LCA(GLuint,(__arg10),A1), \
                  AROS_LCA(GLuint,(__arg11),A2), \
                  AROS_LCA(GLuint,(__arg12),A3), \
        struct Library *, (__MesaBase), 1007, Mesa)

#define glAlphaFragmentOp3ATI(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12) \
    __glAlphaFragmentOp3ATI_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), (arg10), (arg11), (arg12))

#define __glSetFragmentShaderConstantATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glSetFragmentShaderConstantATI, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(const GLfloat *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1008, Mesa)

#define glSetFragmentShaderConstantATI(arg1, arg2) \
    __glSetFragmentShaderConstantATI_WB(MesaBase, (arg1), (arg2))

#define __glPointParameteriNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameteriNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1009, Mesa)

#define glPointParameteriNV(arg1, arg2) \
    __glPointParameteriNV_WB(MesaBase, (arg1), (arg2))

#define __glPointParameterivNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glPointParameterivNV, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1010, Mesa)

#define glPointParameterivNV(arg1, arg2) \
    __glPointParameterivNV_WB(MesaBase, (arg1), (arg2))

#define __glDrawBuffersATI_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDrawBuffersATI, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLenum *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1011, Mesa)

#define glDrawBuffersATI(arg1, arg2) \
    __glDrawBuffersATI_WB(MesaBase, (arg1), (arg2))

#define __glProgramNamedParameter4fNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glProgramNamedParameter4fNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(GLfloat,(__arg4),D2), \
                  AROS_LCA(GLfloat,(__arg5),D3), \
                  AROS_LCA(GLfloat,(__arg6),D4), \
                  AROS_LCA(GLfloat,(__arg7),D5), \
        struct Library *, (__MesaBase), 1012, Mesa)

#define glProgramNamedParameter4fNV(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glProgramNamedParameter4fNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glProgramNamedParameter4dNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glProgramNamedParameter4dNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(GLdouble,(__arg4),D2), \
                  AROS_LCA(GLdouble,(__arg5),D3), \
                  AROS_LCA(GLdouble,(__arg6),D4), \
                  AROS_LCA(GLdouble,(__arg7),D5), \
        struct Library *, (__MesaBase), 1013, Mesa)

#define glProgramNamedParameter4dNV(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glProgramNamedParameter4dNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glProgramNamedParameter4fvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glProgramNamedParameter4fvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(const GLfloat *,(__arg4),A1), \
        struct Library *, (__MesaBase), 1014, Mesa)

#define glProgramNamedParameter4fvNV(arg1, arg2, arg3, arg4) \
    __glProgramNamedParameter4fvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glProgramNamedParameter4dvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glProgramNamedParameter4dvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(const GLdouble *,(__arg4),A1), \
        struct Library *, (__MesaBase), 1015, Mesa)

#define glProgramNamedParameter4dvNV(arg1, arg2, arg3, arg4) \
    __glProgramNamedParameter4dvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetProgramNamedParameterfvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetProgramNamedParameterfvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(GLfloat *,(__arg4),A1), \
        struct Library *, (__MesaBase), 1016, Mesa)

#define glGetProgramNamedParameterfvNV(arg1, arg2, arg3, arg4) \
    __glGetProgramNamedParameterfvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetProgramNamedParameterdvNV_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetProgramNamedParameterdvNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(const GLubyte *,(__arg3),A0), \
                  AROS_LCA(GLdouble *,(__arg4),A1), \
        struct Library *, (__MesaBase), 1017, Mesa)

#define glGetProgramNamedParameterdvNV(arg1, arg2, arg3, arg4) \
    __glGetProgramNamedParameterdvNV_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glIsRenderbufferEXT_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsRenderbufferEXT, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 1018, Mesa)

#define glIsRenderbufferEXT(arg1) \
    __glIsRenderbufferEXT_WB(MesaBase, (arg1))

#define __glBindRenderbufferEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindRenderbufferEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1019, Mesa)

#define glBindRenderbufferEXT(arg1, arg2) \
    __glBindRenderbufferEXT_WB(MesaBase, (arg1), (arg2))

#define __glDeleteRenderbuffersEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteRenderbuffersEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1020, Mesa)

#define glDeleteRenderbuffersEXT(arg1, arg2) \
    __glDeleteRenderbuffersEXT_WB(MesaBase, (arg1), (arg2))

#define __glGenRenderbuffersEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenRenderbuffersEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1021, Mesa)

#define glGenRenderbuffersEXT(arg1, arg2) \
    __glGenRenderbuffersEXT_WB(MesaBase, (arg1), (arg2))

#define __glRenderbufferStorageEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glRenderbufferStorageEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
        struct Library *, (__MesaBase), 1022, Mesa)

#define glRenderbufferStorageEXT(arg1, arg2, arg3, arg4) \
    __glRenderbufferStorageEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetRenderbufferParameterivEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetRenderbufferParameterivEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 1023, Mesa)

#define glGetRenderbufferParameterivEXT(arg1, arg2, arg3) \
    __glGetRenderbufferParameterivEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glIsFramebufferEXT_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsFramebufferEXT, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 1024, Mesa)

#define glIsFramebufferEXT(arg1) \
    __glIsFramebufferEXT_WB(MesaBase, (arg1))

#define __glBindFramebufferEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBindFramebufferEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1025, Mesa)

#define glBindFramebufferEXT(arg1, arg2) \
    __glBindFramebufferEXT_WB(MesaBase, (arg1), (arg2))

#define __glDeleteFramebuffersEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteFramebuffersEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1026, Mesa)

#define glDeleteFramebuffersEXT(arg1, arg2) \
    __glDeleteFramebuffersEXT_WB(MesaBase, (arg1), (arg2))

#define __glGenFramebuffersEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenFramebuffersEXT, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1027, Mesa)

#define glGenFramebuffersEXT(arg1, arg2) \
    __glGenFramebuffersEXT_WB(MesaBase, (arg1), (arg2))

#define __glCheckFramebufferStatusEXT_WB(__MesaBase, __arg1) \
        AROS_LC1(GLenum, glCheckFramebufferStatusEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 1028, Mesa)

#define glCheckFramebufferStatusEXT(arg1) \
    __glCheckFramebufferStatusEXT_WB(MesaBase, (arg1))

#define __glFramebufferTexture1DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTexture1DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 1029, Mesa)

#define glFramebufferTexture1DEXT(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTexture1DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFramebufferTexture2DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTexture2DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 1030, Mesa)

#define glFramebufferTexture2DEXT(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTexture2DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFramebufferTexture3DEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glFramebufferTexture3DEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
                  AROS_LCA(GLint,(__arg6),D5), \
        struct Library *, (__MesaBase), 1031, Mesa)

#define glFramebufferTexture3DEXT(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glFramebufferTexture3DEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glFramebufferRenderbufferEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glFramebufferRenderbufferEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLuint,(__arg4),D3), \
        struct Library *, (__MesaBase), 1032, Mesa)

#define glFramebufferRenderbufferEXT(arg1, arg2, arg3, arg4) \
    __glFramebufferRenderbufferEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGetFramebufferAttachmentParameterivEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetFramebufferAttachmentParameterivEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 1033, Mesa)

#define glGetFramebufferAttachmentParameterivEXT(arg1, arg2, arg3, arg4) \
    __glGetFramebufferAttachmentParameterivEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glGenerateMipmapEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glGenerateMipmapEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 1034, Mesa)

#define glGenerateMipmapEXT(arg1) \
    __glGenerateMipmapEXT_WB(MesaBase, (arg1))

#define __glFramebufferTextureLayerEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glFramebufferTextureLayerEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLint,(__arg4),D3), \
                  AROS_LCA(GLint,(__arg5),D4), \
        struct Library *, (__MesaBase), 1035, Mesa)

#define glFramebufferTextureLayerEXT(arg1, arg2, arg3, arg4, arg5) \
    __glFramebufferTextureLayerEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glMapBufferRange_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(GLvoid*, glMapBufferRange, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptr,(__arg2),D1), \
                  AROS_LCA(GLsizeiptr,(__arg3),D2), \
                  AROS_LCA(GLbitfield,(__arg4),D3), \
        struct Library *, (__MesaBase), 1036, Mesa)

#define glMapBufferRange(arg1, arg2, arg3, arg4) \
    __glMapBufferRange_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

#define __glFlushMappedBufferRange_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glFlushMappedBufferRange, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLintptr,(__arg2),D1), \
                  AROS_LCA(GLsizeiptr,(__arg3),D2), \
        struct Library *, (__MesaBase), 1037, Mesa)

#define glFlushMappedBufferRange(arg1, arg2, arg3) \
    __glFlushMappedBufferRange_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glBindVertexArray_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glBindVertexArray, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 1038, Mesa)

#define glBindVertexArray(arg1) \
    __glBindVertexArray_WB(MesaBase, (arg1))

#define __glDeleteVertexArrays_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDeleteVertexArrays, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(const GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1039, Mesa)

#define glDeleteVertexArrays(arg1, arg2) \
    __glDeleteVertexArrays_WB(MesaBase, (arg1), (arg2))

#define __glGenVertexArrays_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGenVertexArrays, \
                  AROS_LCA(GLsizei,(__arg1),D0), \
                  AROS_LCA(GLuint *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1040, Mesa)

#define glGenVertexArrays(arg1, arg2) \
    __glGenVertexArrays_WB(MesaBase, (arg1), (arg2))

#define __glIsVertexArray_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsVertexArray, \
                  AROS_LCA(GLuint,(__arg1),D0), \
        struct Library *, (__MesaBase), 1041, Mesa)

#define glIsVertexArray(arg1) \
    __glIsVertexArray_WB(MesaBase, (arg1))

#define __glCopyBufferSubData_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glCopyBufferSubData, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLintptr,(__arg3),D2), \
                  AROS_LCA(GLintptr,(__arg4),D3), \
                  AROS_LCA(GLsizeiptr,(__arg5),D4), \
        struct Library *, (__MesaBase), 1042, Mesa)

#define glCopyBufferSubData(arg1, arg2, arg3, arg4, arg5) \
    __glCopyBufferSubData_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glFenceSync_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLsync, glFenceSync, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLbitfield,(__arg2),D1), \
        struct Library *, (__MesaBase), 1043, Mesa)

#define glFenceSync(arg1, arg2) \
    __glFenceSync_WB(MesaBase, (arg1), (arg2))

#define __glIsSync_WB(__MesaBase, __arg1) \
        AROS_LC1(GLboolean, glIsSync, \
                  AROS_LCA(GLsync,(__arg1),D0), \
        struct Library *, (__MesaBase), 1044, Mesa)

#define glIsSync(arg1) \
    __glIsSync_WB(MesaBase, (arg1))

#define __glDeleteSync_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glDeleteSync, \
                  AROS_LCA(GLsync,(__arg1),D0), \
        struct Library *, (__MesaBase), 1045, Mesa)

#define glDeleteSync(arg1) \
    __glDeleteSync_WB(MesaBase, (arg1))

#define __glClientWaitSync_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLenum, glClientWaitSync, \
                  AROS_LCA(GLsync,(__arg1),D0), \
                  AROS_LCA(GLbitfield,(__arg2),D1), \
                  AROS_LCA(GLuint64,(__arg3),D2), \
        struct Library *, (__MesaBase), 1046, Mesa)

#define glClientWaitSync(arg1, arg2, arg3) \
    __glClientWaitSync_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glWaitSync_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glWaitSync, \
                  AROS_LCA(GLsync,(__arg1),D0), \
                  AROS_LCA(GLbitfield,(__arg2),D1), \
                  AROS_LCA(GLuint64,(__arg3),D2), \
        struct Library *, (__MesaBase), 1047, Mesa)

#define glWaitSync(arg1, arg2, arg3) \
    __glWaitSync_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetInteger64v_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glGetInteger64v, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLint64 *,(__arg2),A0), \
        struct Library *, (__MesaBase), 1048, Mesa)

#define glGetInteger64v(arg1, arg2) \
    __glGetInteger64v_WB(MesaBase, (arg1), (arg2))

#define __glGetSynciv_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glGetSynciv, \
                  AROS_LCA(GLsync,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
                  AROS_LCA(GLsizei,(__arg3),D2), \
                  AROS_LCA(GLsizei *,(__arg4),A0), \
                  AROS_LCA(GLint *,(__arg5),A1), \
        struct Library *, (__MesaBase), 1049, Mesa)

#define glGetSynciv(arg1, arg2, arg3, arg4, arg5) \
    __glGetSynciv_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glProvokingVertexEXT_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glProvokingVertexEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 1050, Mesa)

#define glProvokingVertexEXT(arg1) \
    __glProvokingVertexEXT_WB(MesaBase, (arg1))

#define __glDrawElementsBaseVertex_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glDrawElementsBaseVertex, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(const GLvoid *,(__arg4),A0), \
                  AROS_LCA(GLint,(__arg5),D3), \
        struct Library *, (__MesaBase), 1051, Mesa)

#define glDrawElementsBaseVertex(arg1, arg2, arg3, arg4, arg5) \
    __glDrawElementsBaseVertex_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glDrawRangeElementsBaseVertex_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6, __arg7) \
        AROS_LC7NR(void, glDrawRangeElementsBaseVertex, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLuint,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLenum,(__arg5),D4), \
                  AROS_LCA(const GLvoid *,(__arg6),A0), \
                  AROS_LCA(GLint,(__arg7),D5), \
        struct Library *, (__MesaBase), 1052, Mesa)

#define glDrawRangeElementsBaseVertex(arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
    __glDrawRangeElementsBaseVertex_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7))

#define __glMultiDrawElementsBaseVertex_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6NR(void, glMultiDrawElementsBaseVertex, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(const GLsizei *,(__arg2),A0), \
                  AROS_LCA(GLenum,(__arg3),D1), \
                  AROS_LCA(const GLvoid *  *,(__arg4),A1), \
                  AROS_LCA(GLsizei,(__arg5),D2), \
                  AROS_LCA(const GLint *,(__arg6),A2), \
        struct Library *, (__MesaBase), 1053, Mesa)

#define glMultiDrawElementsBaseVertex(arg1, arg2, arg3, arg4, arg5, arg6) \
    __glMultiDrawElementsBaseVertex_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __glProvokingVertex_WB(__MesaBase, __arg1) \
        AROS_LC1NR(void, glProvokingVertex, \
                  AROS_LCA(GLenum,(__arg1),D0), \
        struct Library *, (__MesaBase), 1054, Mesa)

#define glProvokingVertex(arg1) \
    __glProvokingVertex_WB(MesaBase, (arg1))

#define __glRenderbufferStorageMultisampleEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glRenderbufferStorageMultisampleEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLsizei,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLsizei,(__arg4),D3), \
                  AROS_LCA(GLsizei,(__arg5),D4), \
        struct Library *, (__MesaBase), 1055, Mesa)

#define glRenderbufferStorageMultisampleEXT(arg1, arg2, arg3, arg4, arg5) \
    __glRenderbufferStorageMultisampleEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glColorMaskIndexedEXT_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5NR(void, glColorMaskIndexedEXT, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLboolean,(__arg2),D1), \
                  AROS_LCA(GLboolean,(__arg3),D2), \
                  AROS_LCA(GLboolean,(__arg4),D3), \
                  AROS_LCA(GLboolean,(__arg5),D4), \
        struct Library *, (__MesaBase), 1056, Mesa)

#define glColorMaskIndexedEXT(arg1, arg2, arg3, arg4, arg5) \
    __glColorMaskIndexedEXT_WB(MesaBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __glGetBooleanIndexedvEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetBooleanIndexedvEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLboolean *,(__arg3),A0), \
        struct Library *, (__MesaBase), 1057, Mesa)

#define glGetBooleanIndexedvEXT(arg1, arg2, arg3) \
    __glGetBooleanIndexedvEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetIntegerIndexedvEXT_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, glGetIntegerIndexedvEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLint *,(__arg3),A0), \
        struct Library *, (__MesaBase), 1058, Mesa)

#define glGetIntegerIndexedvEXT(arg1, arg2, arg3) \
    __glGetIntegerIndexedvEXT_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glEnableIndexedEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glEnableIndexedEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1059, Mesa)

#define glEnableIndexedEXT(arg1, arg2) \
    __glEnableIndexedEXT_WB(MesaBase, (arg1), (arg2))

#define __glDisableIndexedEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glDisableIndexedEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1060, Mesa)

#define glDisableIndexedEXT(arg1, arg2) \
    __glDisableIndexedEXT_WB(MesaBase, (arg1), (arg2))

#define __glIsEnabledIndexedEXT_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2(GLboolean, glIsEnabledIndexedEXT, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
        struct Library *, (__MesaBase), 1061, Mesa)

#define glIsEnabledIndexedEXT(arg1, arg2) \
    __glIsEnabledIndexedEXT_WB(MesaBase, (arg1), (arg2))

#define __glBeginConditionalRenderNV_WB(__MesaBase, __arg1, __arg2) \
        AROS_LC2NR(void, glBeginConditionalRenderNV, \
                  AROS_LCA(GLuint,(__arg1),D0), \
                  AROS_LCA(GLenum,(__arg2),D1), \
        struct Library *, (__MesaBase), 1062, Mesa)

#define glBeginConditionalRenderNV(arg1, arg2) \
    __glBeginConditionalRenderNV_WB(MesaBase, (arg1), (arg2))

#define __glEndConditionalRenderNV_WB(__MesaBase) \
        AROS_LC0NR(void, glEndConditionalRenderNV, \
        struct Library *, (__MesaBase), 1063, Mesa)

#define glEndConditionalRenderNV() \
    __glEndConditionalRenderNV_WB(MesaBase)

#define __glObjectPurgeableAPPLE_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLenum, glObjectPurgeableAPPLE, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
        struct Library *, (__MesaBase), 1064, Mesa)

#define glObjectPurgeableAPPLE(arg1, arg2, arg3) \
    __glObjectPurgeableAPPLE_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glObjectUnpurgeableAPPLE_WB(__MesaBase, __arg1, __arg2, __arg3) \
        AROS_LC3(GLenum, glObjectUnpurgeableAPPLE, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
        struct Library *, (__MesaBase), 1065, Mesa)

#define glObjectUnpurgeableAPPLE(arg1, arg2, arg3) \
    __glObjectUnpurgeableAPPLE_WB(MesaBase, (arg1), (arg2), (arg3))

#define __glGetObjectParameterivAPPLE_WB(__MesaBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4NR(void, glGetObjectParameterivAPPLE, \
                  AROS_LCA(GLenum,(__arg1),D0), \
                  AROS_LCA(GLuint,(__arg2),D1), \
                  AROS_LCA(GLenum,(__arg3),D2), \
                  AROS_LCA(GLint *,(__arg4),A0), \
        struct Library *, (__MesaBase), 1066, Mesa)

#define glGetObjectParameterivAPPLE(arg1, arg2, arg3, arg4) \
    __glGetObjectParameterivAPPLE_WB(MesaBase, (arg1), (arg2), (arg3), (arg4))

__END_DECLS

#endif /* DEFINES_MESA_H*/
