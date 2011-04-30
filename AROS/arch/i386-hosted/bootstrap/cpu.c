#include <stdio.h>

static char libstack[1024];

void setup_cpu(void)
{
    fprintf(stderr, "[Bootstrap] Setting up for i386\n");

    /* relbase stack setup */
    __asm__ __volatile__("movl %0, %%ebx" : : "r" (libstack));
}
