/*
    Copyright © 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <proto/exec.h>

#define MAX_COPY_SIZE (1024 * 1024)

int main()
{
    struct timeval  tv_start, tv_end;
    ULONG count = 20000000;
    double elapsed = 0.0;
    ULONG i, size;
    APTR memory, memory2;

    memory = AllocVec(MAX_COPY_SIZE, MEMF_ANY);
    memory2 = AllocVec(MAX_COPY_SIZE, MEMF_ANY);

    if (memory != NULL && memory2 != NULL)
    {
        for (size = 16; size <= MAX_COPY_SIZE; size *= 4, count /= 4)
        {
            gettimeofday(&tv_start, NULL);

            for(i = 0; i < count; i++)
            {
                CopyMem(memory + (size * i) % MAX_COPY_SIZE,
                    memory2 + (size * i) % MAX_COPY_SIZE, size);
            }

            gettimeofday(&tv_end, NULL);

            elapsed = ((double)(((tv_end.tv_sec * 1000000) + tv_end.tv_usec)
                - ((tv_start.tv_sec * 1000000) + tv_start.tv_usec)))
                / 1000000.0;

            printf("Block size:        %d bytes\n"
                "Elapsed time:      %f seconds\n"
                "Number of copies:  %d\n"
                "Copies per second: %f\n"
                "MBytes per second: %f\n\n",
                size, elapsed, count, (double) count / elapsed,
                (double) count * size / elapsed / (1024 * 1024));
        }
    }

    FreeVec(memory);
    FreeVec(memory2);

    return 0;
}
