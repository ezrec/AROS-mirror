#include <exec/types.h>

void Planar2Chunky(APTR source,APTR dest,WORD breite,WORD planes)
{
    LONG x, p, bpr;
    
    bpr = ((breite + 15) / 16) * 2;
    
    for(x = 0; x < breite; x++)
    {
	LONG mask   = 1 << (7 - (x & 7));
	LONG offset = x / 8;
	unsigned char chunkypix = 0;

	for(p = 0; p < planes; p++)
	{
	    if (((UBYTE *)source)[p * bpr + offset] & mask) chunkypix |= (1 << p);
	}
	((UBYTE *)dest)[x] = chunkypix;
    }
}


