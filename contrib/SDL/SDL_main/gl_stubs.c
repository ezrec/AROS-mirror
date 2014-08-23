/*
    Copyright © 1995-2014, The AROS Development Team. All rights reserved.
*/
#undef  NOLIBINLINE
#undef  NOLIBDEFINES
#define NOLIBINLINE
#define NOLIBDEFINES
/* Be sure that the libbases are included in the stubs file */
#undef __NOLIBBASE__
#undef __GL_NOLIBBASE__
#include <proto/gl.h>
#include <stddef.h>

#include <aros/cpu.h>
#include <aros/genmodule.h>
#include <aros/libcall.h>
#include <aros/symbolsets.h>

void __glA_GLBase_libreq(){ AROS_LIBREQ(GLBase,20); }
AROS_GM_LIBFUNCSTUB(glACreateContext, GLBase, 5)
AROS_GM_LIBFUNCSTUB(glADestroyContext, GLBase, 6)
AROS_GM_LIBFUNCSTUB(glAMakeCurrent, GLBase, 7)
AROS_GM_LIBFUNCSTUB(glASwapBuffers, GLBase, 8)
AROS_GM_LIBFUNCSTUB(glASetRast, GLBase, 11)
AROS_GM_LIBFUNCSTUB(glAGetConfig, GLBase, 12)
AROS_GM_LIBFUNCSTUB(glAGetProcAddress, GLBase, 13)
