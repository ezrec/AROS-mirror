#ifndef GLX_HOSTLIB_H
#define GLX_HOSTLIB_H

#include <aros/config.h>

#include <GL/glx.h>

struct glx_func {
    GLXContext (*glXCreateContext) (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
    void (*glXDestroyContext) (Display *dpy, GLXContext ctx);
    /* glXGetProcAddress is only to be used internally */
    void * (*glXGetProcAddress)(const GLubyte *procname);
};

extern void *glx_handle;
extern struct glx_func glx_func;

#ifdef HOST_OS_linux
#define GLX_SOFILE    "libGL.so"
#endif

#ifdef HOST_OS_darwin
#endif

#ifndef GLX_SOFILE
#define GLX_SOFILE    "libGL.so"
#endif

#define GLXCALL(func,...) (glx_func.func(__VA_ARGS__))

/* GL functions are retrieved via glXGetProcAddress */
#define GLCALL(func,...) (gl_func.func(__VA_ARGS__))

struct gl_func {
};

#endif
