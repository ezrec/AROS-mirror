
#include <exec/types.h>

/* Code to swap two memory areas */

void swapmem (UBYTE * ptr1, UBYTE * ptr2, int len)
{
    UBYTE tmp;
    
    if (ptr1 == ptr2)
	return;

    
    for ( ; len > 0; len--)
    {
	tmp = *ptr1;
	*ptr1 = *ptr2;
	*ptr2 = tmp;

	ptr1 ++;
	ptr2 ++;
    }
}
