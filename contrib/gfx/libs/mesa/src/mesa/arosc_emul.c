/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <proto/dos.h>

#define DEBUG 0
#include <aros/debug.h>

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

/* 
    The purpose of this file is to
    a) block arosc.library from auto opening
    b) provide implementation for functions from arosc.library which are used by Mesa/Gallium

    Mesa/Gallium cannot use arosc.library because arosc.library context is bound with caller task
    and get's destroyed when the task exits. mesa.library at that point is still in RAM and
    crashes at expunge.
*/


/* Blocks arosc.library from beeing auto opened */
/* Every arosc.library function which is not implemented here will now segfault */
struct Library * aroscbase = NULL;


/* malloc/calloc/realloc/free */

/* This is a copy of implementation from arosc.library so that mesa.library uses its 
   own space for malloc/calloc/realloc calls */

#define MEMALIGN_MAGIC ((size_t) 0x57E2CB09)
APTR __mempool = NULL;

void * malloc (size_t size)
{
    UBYTE *mem = NULL;

    D(bug("arosc_emul_malloc\n"));
    /* Allocate the memory */
    mem = AllocPooled (__mempool, size + AROS_ALIGN(sizeof(size_t)));
    if (mem)
    {
        *((size_t *)mem) = size;
        mem += AROS_ALIGN(sizeof(size_t));
    }
    else
        errno = ENOMEM;

    return mem;
}

void free (void * memory)
{
    D(bug("arosc_emul_free\n"));
    if (memory)
    {
        unsigned char *mem;
        size_t         size;

        mem = ((UBYTE *)memory) - AROS_ALIGN(sizeof(size_t));

        size = *((size_t *) mem);
        if (size == MEMALIGN_MAGIC)
            free(((void **) mem)[-1]);
        else 
        {
            size += AROS_ALIGN(sizeof(size_t));
            FreePooled (__mempool, mem, size);
        }
    }
}

void * calloc (size_t count, size_t size)
{
    ULONG * mem;
    D(bug("arosc_emul_calloc\n"));
    size *= count;

    /* Allocate the memory */
    mem = malloc (size);

    if (mem)
        memset (mem, 0, size);

    return mem;
}

void * realloc (void * oldmem, size_t size)
{
    
    UBYTE * mem, * newmem;
    size_t oldsize;
    
    D(bug("arosc_emul_realloc\n"));
    
    if (!oldmem)
        return malloc (size);

    mem = (UBYTE *)oldmem - AROS_ALIGN(sizeof(size_t));
    oldsize = *((size_t *)mem);

    /* Reduce or enlarge the memory ? */
    if (size < oldsize)
    {
        /* Don't change anything for small changes */
        if ((oldsize - size) < 4096)
            newmem = oldmem;
        else
            goto copy;
    }
    else if (size == oldsize) /* Keep the size ? */
        newmem = oldmem;
    else
    {
copy:
        newmem = malloc (size);

        if (newmem)
        {
            CopyMem (oldmem, newmem, size);
            free (oldmem);
        }
    }

    return newmem;
}

char *getenv (const char *name)
{ 
    /* This function is not thread-safe */
    static TEXT buff[128] = {NULL};
    
    D(bug("arosc_emul_getenv: %s\n", name));

    if (GetVar(name, buff, 128, GVF_BINARY_VAR) == -1)
        return NULL;
    else
        return buff;
}

int fprintf (FILE * fh, const char * format, ...)
{
    bug("------IMPLEMENT(arosc_emul_fprintf)\n");
    
    return 0;
}


int __init_emul(void)
{
    /* malloc/calloc/realloc/free */
    __mempool = CreatePool(MEMF_ANY | MEMF_SEM_PROTECTED, 65536L, 4096L);

    if (!__mempool)
    {
        return 0;
    }

    return 1;
}


void __exit_emul(void)
{
    /* malloc/calloc/realloc/free */
    if (__mempool)
    {
        DeletePool(__mempool);
    }
}

ADD2INIT(__init_emul, 0);
ADD2EXIT(__exit_emul, 0);
