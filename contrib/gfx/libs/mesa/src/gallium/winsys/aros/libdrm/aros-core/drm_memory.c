#include "drmP.h"

#include <proto/exec.h>

void *drm_calloc(size_t nmemb, size_t size, int area)
{
    return AllocVec(nmemb * size, MEMF_PUBLIC | MEMF_CLEAR);
}

void *drm_alloc(size_t size, int area)
{
    return AllocVec(size, MEMF_PUBLIC);
}

void drm_free(void *pt, size_t size, int area)
{
    /* FIXME: Why size is given ? */
    FreeVec(pt);
}