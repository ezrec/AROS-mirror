#include <exec/memory.h>
#include <exec/semaphores.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <string.h>

#define PoolSize 131072
static void *MemPool = NULL;
static struct SignalSemaphore PoolSem;

ULONG InitSupport(void)
{
    if ((MemPool = CreatePool(MEMF_PUBLIC, PoolSize, PoolSize)))
    {
        InitSemaphore(&PoolSem);
        return 1;
    }

    return 0;
}

void RemoveSupport(void)
{
    if (MemPool)
    {
        DeletePool(MemPool);
        MemPool = NULL;
    }
}

void *malloc(unsigned long size)
{
    ULONG *MyMemory;

    if (size == 0) return NULL;
    size += 8;

    ObtainSemaphore(&PoolSem);
    MyMemory = (ULONG *)AllocPooled(MemPool, size);
    ReleaseSemaphore(&PoolSem);

    if (MyMemory)
    {
        *MyMemory++ = size;

        if ((ULONG)MyMemory & 7)
        {
            *MyMemory++ = 0;
        }
    }

    return((void *)MyMemory);
}

void *calloc(unsigned long nobj, unsigned long size)
{
    unsigned long siz = size * nobj;
    void *ptr;

    if ((ptr = malloc(siz)))
        bzero(ptr, siz);

    return ptr;
}

void free(void *ptr)
{
    ULONG *MemPtr;
    ULONG Size;

    if (ptr == NULL) return;

    MemPtr = ptr;
    Size = *--MemPtr;

    if (Size == 0)
    {
        Size = *--MemPtr;
    }

    ObtainSemaphore(&PoolSem);
    FreePooled(MemPool, MemPtr, Size);
    ReleaseSemaphore(&PoolSem);
}

