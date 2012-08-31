#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <string.h>

void *malloc(size_t size)
{
    return AllocVec(size, MEMF_ANY);
}

void free(void *mem)
{
    FreeVec(mem);
}

void *calloc(size_t nmemb, size_t size)
{
    return AllocVec(nmemb * size, MEMF_ANY | MEMF_CLEAR);
}

void *realloc(void *ptr, size_t size)
{
    void *nptr = NULL;

    if (size > 0) {
        nptr = malloc(size);
        if (nptr && ptr) {
            /* FIXME: If AllocVec uses IPTR instead of ULONG,
             *        this will need to be changed to support 64bit
             *        architectures.
             */
            ULONG oldsize = *((ULONG *)(ptr - AROS_ALIGN(sizeof(ULONG))));
            CopyMem(ptr, nptr, (oldsize < size) ? oldsize : size);
        }
    }
    free(ptr);

    return nptr;
}

char *strdup(const char *s)
{
    char *ns;
    size_t len = strlen(s) + 1;

    ns = malloc(len);
    if (ns) {
        CopyMem(s, ns, len);
    }

    return ns;
}
