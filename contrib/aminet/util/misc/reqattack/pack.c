/* C version of pack.asm */

#include <exec/types.h>

void UnpackBytes(APTR source,APTR dest,LONG unpackedsize,APTR *nextdata)
{
    UBYTE r;
    BYTE c;
    
    for(;;)
    {
	c = *((BYTE *)source)++;
	if (c >= 0)
	{
    	    while(c-- >= 0)
	    {
		*((UBYTE *)dest)++ = *((UBYTE *)source)++;
		if (--unpackedsize <= 0)
		{
		    *nextdata = source;
		    return;
		}
	    }
	}
	else if (c != -128)
	{
    	    c = -c;
	    r = *((UBYTE *)source)++;

	    while(c-- >= 0)
	    {
		*((UBYTE *)dest)++ = r;
		if (--unpackedsize <= 0)
		{
		    *nextdata = source;
		    return;
		}
	    }
	}
    }

    *nextdata = source;
    return;
}



