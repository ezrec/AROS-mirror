/* C version of p2c.asm */

#include <exec/types.h>
#include <string.h>

void Planar2Chunky(APTR source,APTR dest,WORD width,WORD planes)
{
    WORD width16 = (width + 15) & ~15;
    WORD x, p;
    UBYTE pmask = 1;
    
    memset(dest, 0, width16);
    
    for(p = 0; p < planes; p++, pmask <<= 1)
    {
        UBYTE *srcp = (UBYTE *)source + (width16 / 8) * p;
    	UBYTE src = *srcp++;
	UBYTE srcmask = 0x80;
    	UBYTE *dst = (UBYTE *)dest;
			
    	for(x = 0; x < width16; x++, dst++)
    	{
    	    if (src & srcmask) *dst |= pmask;
	    srcmask >>= 1;
	    if (!srcmask)
	    {
	    	srcmask = 0x80;
		src = *srcp++;
	    }
    	}
	
    }
    
}
