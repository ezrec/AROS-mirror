#include "tblib.h"

int cmp64 ( struct Integer64 *dst, struct Integer64 *src )
{
    unsigned long long *dst2 = (unsigned long long *)dst;
    unsigned long long *src2 = (unsigned long long *)src;

    if(dst2 < src2) return -1;
    if(dst2 > src2) return 1;

    return 0;
}

