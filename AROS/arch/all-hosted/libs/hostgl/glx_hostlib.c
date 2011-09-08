#include <aros/config.h>

#include <aros/symbolsets.h>

#include "glx_hostlib.h"

#include <proto/hostlib.h>

#include LC_LIBDEFS_FILE

#define DEBUG 0
#include <aros/debug.h>

void *glx_handle = NULL;

struct glx_func glx_func;

static const char *glx_func_names[] = {
    "glXCreateContext",
    "glXDestroyContext",
    "glXGetProcAddress"
};

#define GLX_NUM_FUNCS (3)

APTR HostLibBase;

void *hostlib_load_so(const char *sofile, const char **names, int nfuncs, void **funcptr) {
    void *handle;
    char *err;
    int i;

    D(bug("[glx] loading %d functions from %s\n", nfuncs, sofile));

    if ((handle = HostLib_Open(sofile, &err)) == NULL) {
        kprintf("[glx] couldn't open '%s': %s\n", sofile, err);
        return NULL;
    }

    for (i = 0; i < nfuncs; i++) {
        funcptr[i] = HostLib_GetPointer(handle, names[i], &err);
        D(bug("%s(%x)\n", names[i], funcptr[i]));
        if (err != NULL) {
            kprintf("[glx] couldn't get symbol '%s' from '%s': %s\n", names[i], sofile, err);
            HostLib_Close(handle, NULL);
            return NULL;
        }
    }

    D(bug("[glx] done\n"));

    return handle;
}

static int glx_hostlib_init(LIBBASETYPEPTR LIBBASE) {
    D(bug("[glx] hostlib init\n"));

    if ((HostLibBase = OpenResource("hostlib.resource")) == NULL) {
        kprintf("[glx] couldn't open hostlib.resource\n");
        return FALSE;
    }

    if ((glx_handle = hostlib_load_so(GLX_SOFILE, glx_func_names, GLX_NUM_FUNCS, (void **) &glx_func)) == NULL)
        return FALSE;

    return TRUE;
}

static int glx_hostlib_expunge(LIBBASETYPEPTR LIBBASE) {
    D(bug("[glx] hostlib expunge\n"));

    if (glx_handle != NULL)
        HostLib_Close(glx_handle, NULL);

    return TRUE;
}

ADD2INITLIB(glx_hostlib_init, 0)
ADD2EXPUNGELIB(glx_hostlib_expunge, 0)

