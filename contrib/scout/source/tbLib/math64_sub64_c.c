#include "tblib.h"

void sub64 ( struct Integer64 *dst, struct Integer64 *src )
{
    unsigned long long *dst2 = (unsigned long long *)dst;
    unsigned long long *src2 = (unsigned long long *)src;

    *dst2 -= *src2;
}

