#include "arosdrm.h"
#include "nouveau_drm.h"

#define DEBUG 1
#include <aros/debug.h>

int 
drmCommandNone(int fd, unsigned long drmCommandIndex)
{
    D(bug("drmCommandNone - %d\n", drmCommandIndex));
    return 0;
}

int
drmCommandRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    D(bug("drmCommandRead - %d\n", drmCommandIndex));
    return 0;
}

int
drmCommandWrite(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    D(bug("drmCommandWrite - %d\n", drmCommandIndex));
    return 0;
}

int
drmCommandWriteRead(int fd, unsigned long drmCommandIndex, void *data, unsigned long size)
{
    D(bug("drmCommandWriteRead - %d\n", drmCommandIndex));
    return 0;
}
 
int           
drmMap(int fd, drm_handle_t handle, drmSize size, drmAddressPtr address)
{
    D(bug("drmMap\n"));
    return 0;
}

int
drmUnmap(drmAddress address, drmSize size)
{
    D(bug("drmUnmap\n"));
    return 0;
}

int
drmOpen(const char *name, const char *busid)
{
    D(bug("drmOpen %s, %s\n", name, busid));
    return 4242; /*FIXME: some id just for now */
}

int
drmClose(int fd)
{
    D(bug("drmClose\n"));
    return 0;
}

drmVersionPtr
drmGetVersion(int fd)
{
    static drmVersion ver;
    D(bug("drmGetVersion\n"));
    ver.version_patchlevel = NOUVEAU_DRM_HEADER_PATCHLEVEL;
    return &ver;
}

void
drmFreeVersion(drmVersionPtr ptr)
{
    D(bug("drmFreeVersion\n"));
}

int
drmCreateContext(int fd, drm_context_t * handle)
{
    D(bug("drmCreateContext\n"));
    return 0;
}

int
drmDestroyContext(int fd, drm_context_t handle)
{
    D(bug("drmDestroyContext\n"));
    return 0;
}

