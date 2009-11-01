/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#define GL_GLEXT_PROTOTYPES
#undef USE_MGL_NAMESPACE
#include <GL/arosmesa.h>
#include "glapi.h"
#include "glapioffsets.h"
#define NEED_FUNCTION_POINTER
#include "glprocs.h"
#include <string.h>

/**
 * Search the table of static entrypoint functions for the named function
 * and return the corresponding glprocs_table_t entry.
 */
static const glprocs_table_t *
find_entry( const char * n )
{
   GLuint i;
   for (i = 0; static_functions[i].Name_offset >= 0; i++) {
      const char *testName = gl_string_table + static_functions[i].Name_offset;
#ifdef MANGLE
      /* skip the "m" prefix on the name */
      if (strcmp(testName, n + 1) == 0)
#else
      if (strcmp(testName, n) == 0)
#endif
      {
         return &static_functions[i];
      }
   }
   return NULL;
}

#ifdef USE_X86_ASM

#if defined( GLX_USE_TLS )
extern       GLubyte gl_dispatch_functions_start[];
extern       GLubyte gl_dispatch_functions_end[];
#else
extern const GLubyte gl_dispatch_functions_start[];
#endif

#endif /* USE_X86_ASM */

/**
 * Return dispatch function address for the named static (built-in) function.
 * Return NULL if function not found.
 */
static _glapi_proc
get_static_proc_address(const char *funcName)
{
   const glprocs_table_t * const f = find_entry( funcName );
   if (f) {
#if defined(DISPATCH_FUNCTION_SIZE) && defined(GLX_INDIRECT_RENDERING)
      return (f->Address == NULL)
     ? (_glapi_proc) (gl_dispatch_functions_start
              + (DISPATCH_FUNCTION_SIZE * f->Offset))
         : f->Address;
#elif defined(DISPATCH_FUNCTION_SIZE)
      return (_glapi_proc) (gl_dispatch_functions_start 
                            + (DISPATCH_FUNCTION_SIZE * f->Offset));
#else
      return f->Address;
#endif
   }
   else {
      return NULL;
   }
}

AROSMesaProc AROSMesaGetProcAddress(const GLubyte * procname)
{
    return get_static_proc_address(procname);
}

#if defined(AROS_MESA_SHARED)
/* Implementation of no-op functions to remove linker problems */
void GLAPIENTRY gl_dispatch_stub_563(GLenum pname, GLfloat * params) {};
void GLAPIENTRY gl_dispatch_stub_564(GLenum pname, GLint * params) {};
void GLAPIENTRY gl_dispatch_stub_565(GLenum pname, GLfloat param) {};
void GLAPIENTRY gl_dispatch_stub_566(GLenum pname, const GLfloat * params) {};
void GLAPIENTRY gl_dispatch_stub_567(GLenum pname, GLint param) {};
void GLAPIENTRY gl_dispatch_stub_568(GLenum pname, const GLint * params) {};
void GLAPIENTRY gl_dispatch_stub_569(GLclampf value, GLboolean invert) {};
void GLAPIENTRY gl_dispatch_stub_570(GLenum pattern) {};
void GLAPIENTRY gl_dispatch_stub_607(GLenum mode) {};
void GLAPIENTRY gl_dispatch_stub_649(const GLenum * mode, const GLint * first, const GLsizei * count, GLsizei primcount, GLint modestride) {};
void GLAPIENTRY gl_dispatch_stub_650(const GLenum * mode, const GLsizei * count, GLenum type, const GLvoid * const * indices, GLsizei primcount, GLint modestride) {};
void GLAPIENTRY gl_dispatch_stub_651(GLsizei n, const GLuint * fences) {};
void GLAPIENTRY gl_dispatch_stub_652(GLuint fence) {};
void GLAPIENTRY gl_dispatch_stub_653(GLsizei n, GLuint * fences) {};
void GLAPIENTRY gl_dispatch_stub_654(GLuint fence, GLenum pname, GLint * params) {};
GLboolean GLAPIENTRY gl_dispatch_stub_655(GLuint fence) { return GL_FALSE; };
void GLAPIENTRY gl_dispatch_stub_656(GLuint fence, GLenum condition) {};
GLboolean GLAPIENTRY gl_dispatch_stub_657(GLuint fence) { return GL_FALSE; };
void GLAPIENTRY gl_dispatch_stub_738(GLenum face) {};
void GLAPIENTRY gl_dispatch_stub_739(GLuint array) {};
void GLAPIENTRY gl_dispatch_stub_740(GLsizei n, const GLuint * arrays) {};
void GLAPIENTRY gl_dispatch_stub_741(GLsizei n, GLuint * arrays) {};
GLboolean GLAPIENTRY gl_dispatch_stub_742(GLuint array) { return GL_FALSE; };
void GLAPIENTRY gl_dispatch_stub_749(GLclampd zmin, GLclampd zmax) {};
void GLAPIENTRY gl_dispatch_stub_768(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {};
void GLAPIENTRY gl_dispatch_stub_770(GLenum frontfunc, GLenum backfunc, GLint ref, GLuint mask) {};
void GLAPIENTRY gl_dispatch_stub_771(GLenum target, GLuint index, GLsizei count, const GLfloat * params) {};
void GLAPIENTRY gl_dispatch_stub_772(GLenum target, GLuint index, GLsizei count, const GLfloat * params) {};
void GLAPIENTRY gl_dispatch_stub_773(GLuint id, GLenum pname, GLint64EXT * params) {};
void GLAPIENTRY gl_dispatch_stub_774(GLuint id, GLenum pname, GLuint64EXT * params) {};
#endif
