/* C version of remap.asm */

#include <exec/types.h>

void RemapBytes(APTR mem,APTR remaptable,LONG size)
{
#if 1
    while(size > 0)
    {
    	ULONG src = *((ULONG *)mem);
	ULONG dst;
	
	dst =   ((UWORD *)remaptable)[src & 0xFF] & 0xFF;

	src >>= 8;	
	dst |=  (((UWORD *)remaptable)[src & 0xFF] & 0xFF) << 8;

	src >>= 8;
	dst |=  (((UWORD *)remaptable)[src & 0xFF] & 0xFF) << 16;

	src >>= 8;
	dst |=  (((UWORD *)remaptable)[src & 0xFF] & 0xFF) << 24;
	
	*((ULONG *)mem)++ = dst;
	
    	size -= 4;
    }
#endif

}

