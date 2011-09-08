#include <aros/config.h>

#include <aros/symbolsets.h>

#include "x11_hostlib.h"

#include <proto/hostlib.h>

#include LC_LIBDEFS_FILE

#define DEBUG 0
#include <aros/debug.h>

void *x11_handle = NULL;

struct x11_func x11_func;

static const char *x11_func_names[] = {
    "XOpenDisplay",
    "XCreateColormap",
    "XCreateWindow",
    "XCloseDisplay",
    "XDestroyWindow",
    "XFlush",
    "XFree",
    "XMapWindow"
};

#define X11_NUM_FUNCS (8)

APTR HostLibBase;

void *x11_hostlib_load_so(const char *sofile, const char **names, int nfuncs, void **funcptr) {
    void *handle;
    char *err;
    int i;

    D(bug("[x11] loading %d functions from %s\n", nfuncs, sofile));

    if ((handle = HostLib_Open(sofile, &err)) == NULL) {
        kprintf("[x11] couldn't open '%s': %s\n", sofile, err);
        return NULL;
    }

    for (i = 0; i < nfuncs; i++) {
        funcptr[i] = HostLib_GetPointer(handle, names[i], &err);
        D(bug("%s(%x)\n", names[i], funcptr[i]));
        if (err != NULL) {
            kprintf("[x11] couldn't get symbol '%s' from '%s': %s\n", names[i], sofile, err);
            HostLib_Close(handle, NULL);
            return NULL;
        }
    }

    D(bug("[x11] done\n"));

    return handle;
}

static int x11_hostlib_init(LIBBASETYPEPTR LIBBASE) {
    D(bug("[x11] hostlib init\n"));

    if ((HostLibBase = OpenResource("hostlib.resource")) == NULL) {
        kprintf("[x11] couldn't open hostlib.resource\n");
        return FALSE;
    }

    if ((x11_handle = x11_hostlib_load_so(X11_SOFILE, x11_func_names, X11_NUM_FUNCS, (void **) &x11_func)) == NULL) {
        HostLib_Close(x11_handle, NULL);
        return FALSE;
    }

    return TRUE;
}

static int x11_hostlib_expunge(LIBBASETYPEPTR LIBBASE) {
    D(bug("[x11] hostlib expunge\n"));

    if (x11_handle != NULL)
        HostLib_Close(x11_handle, NULL);

    return TRUE;
}

ADD2INITLIB(x11_hostlib_init, 0)
ADD2EXPUNGELIB(x11_hostlib_expunge, 0)

