#include <exec/types.h>

void RemapBytes(APTR mem, APTR remaptable, LONG size)
{
    while(size--)
    {
    	UBYTE c = *((UBYTE *)mem);
	
	c = ((UBYTE *)remaptable)[c];
	
    	*((UBYTE *)mem)++ = c;
    }
}
