
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>

/************************************************************************** 
**
**	rgbarraydiversity
*/

LIBAPI LONG RGBArrayDiversityA(ULONG *src, UWORD width, UWORD height, struct TagItem *tags)
{
	LONG D = -1;
	RNDPAL *dstpal = (RNDPAL *) GetTagData(RND_MapEngine, NULL, tags);
	dstpal = (RNDPAL *) GetTagData(RND_Palette, (ULONG) dstpal, tags);
	
	if (src && dstpal && width && height)
	{
		LONG tsw = GetTagData(RND_SourceWidth, width, tags);
		LONG xstep = GetTagData(RND_Interleave, 0, tags) + 1;
		LONG y, x;
		FLOAT DF = 0;
		ULONG rgb1, rgb2;
		LONG dr, dg, db;
		LONG num = 0;

		ObtainSemaphore(&dstpal->lock);
		
		if (GetP2Table(dstpal))
		{
			for (y = 0; y < height; ++y)
			{
				for (x = 0; x < width; x += xstep)
				{
				#ifdef RENDER_LITTLE_ENDIAN
					rgb1 = AROS_BE2LONG(src[x]);
				#else
					rgb1 = src[x];
				#endif
					rgb2 = dstpal->table[P2Lookup(dstpal, rgb1)];

					dr = ((rgb1 & 0xff0000) >> 16);
					dr -= (LONG) ((rgb2 & 0xff0000) >> 16);
					dg = ((rgb1 & 0x00ff00) >> 8);
					dg -= (LONG) ((rgb2 & 0x00ff00) >> 8);
					db = (rgb1 & 0x0000ff);
					db -= (LONG) (rgb2 & 0x0000ff);
					DF += dr*dr + dg*dg + db*db;
					num++;
				}
				src += tsw;
			}
		}

		ReleaseSemaphore(&dstpal->lock);
		
		D = DF / num;
	}

	return D;
}

/************************************************************************** 
**
**	chunkyarraydiversity
*/

LIBAPI LONG ChunkyArrayDiversityA(UBYTE *src, RNDPAL *srcpal, UWORD width, UWORD height, struct TagItem *tags)
{
	LONG D = -1;
	RNDPAL *dstpal = (RNDPAL *) GetTagData(RND_MapEngine, NULL, tags);
	dstpal = (RNDPAL *) GetTagData(RND_Palette, (ULONG) dstpal, tags);
	
	if (src && srcpal && dstpal && width && height)
	{
		LONG tsw = GetTagData(RND_SourceWidth, width, tags);
		LONG xstep = GetTagData(RND_Interleave, 0, tags) + 1;
		LONG y, x;
		FLOAT DF = 0;
		ULONG rgb1, rgb2;
		LONG dr, dg, db;
		LONG num = 0;

		ObtainSemaphore(&srcpal->lock);
		ObtainSemaphore(&dstpal->lock);
		
		if (GetP2Table(dstpal))
		{
			for (y = 0; y < height; ++y)
			{
				for (x = 0; x < width; x += xstep)
				{
					rgb1 = srcpal->table[src[x]];
					rgb2 = dstpal->table[P2Lookup(dstpal, rgb1)];
					dr = ((rgb1 & 0xff0000) >> 16);
					dr -= (LONG) ((rgb2 & 0xff0000) >> 16);
					dg = ((rgb1 & 0x00ff00) >> 8);
					dg -= (LONG) ((rgb2 & 0x00ff00) >> 8);
					db = (rgb1 & 0x0000ff);
					db -= (LONG) (rgb2 & 0x0000ff);
					DF += dr*dr + dg*dg + db*db;
					num++;
				}
				src += tsw;
			}
		}

		ReleaseSemaphore(&dstpal->lock);
		ReleaseSemaphore(&srcpal->lock);
		
		D = DF / num;
	}

	return D;
}

