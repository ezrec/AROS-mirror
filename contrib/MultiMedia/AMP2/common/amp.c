/*
 *
 * amp.c
 *
 */

#include <stdio.h>

#include <exec/memory.h>

#if !defined(__AROS__)
#include <powerpc/powerpc_protos.h>
#else

#include "aros-inc.h"

#endif
#include "amp.h"

/* Allocate and free aligned memory */
void * mallocalign (size_t size)
{
    return AllocVecPPC(size, MEMF_PUBLIC|MEMF_CLEAR, 32);
}

void freealign (void * buffer)
{
    FreeVecPPC(buffer);
}
