
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include "rnd_palette.h"

/* 
**	the implementation of mapping-engines is a dummy for now.
*/

LIBAPI RNDMAP *CreateMapEngineA(RNDPAL *palette, struct TagItem *tags)
{
	return (RNDMAP *) palette;
}

LIBAPI void DeleteMapEngine(RNDMAP *mapengine)
{
}

LIBAPI ULONG MapRGBArrayA(RNDPAL *pal, ULONG *src, UWORD width, UWORD height, UBYTE *dst, struct TagItem *tags)
{
	ULONG result = CONV_NO_DATA;
	if (pal && src && width && height && dst)
	{
		ObtainSemaphore(&pal->lock);
		
		result = CONV_NOT_ENOUGH_MEMORY;
		if (GetP2Table(pal))
		{
			LONG tsw = GetTagData(RND_SourceWidth, width, tags);
			LONG tdw = GetTagData(RND_DestWidth, width, tags);
			UBYTE *pentab = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);
			LONG y, x;

			if (pentab)
			{
				for (y = 0; y < height; ++y)
				{
					for (x = 0; x < width; ++x)
					{
					#ifdef RENDER_LITTLE_ENDIAN
					    	ULONG pix = *src++;
						*dst++ = pentab[P2Lookup(pal, AROS_BE2LONG(pix))];
					#else
						*dst++ = pentab[P2Lookup(pal, *src++)];
					#endif
					}
					src += tsw - width;
					dst += tdw - width;
				}
			}
			else
			{
				for (y = 0; y < height; ++y)
				{
					for (x = 0; x < width; ++x)
					{
					#ifdef RENDER_LITTLE_ENDIAN
					    	ULONG pix = *src++;
						*dst++ = P2Lookup(pal, AROS_BE2LONG(pix));
					#else
						*dst++ = P2Lookup(pal, *src++);
					#endif
					}
					src += tsw - width;
					dst += tdw - width;
				}
			}
			result = CONV_SUCCESS;
		}

		ReleaseSemaphore(&pal->lock);
	}
	
	return result;
}

LIBAPI ULONG MapChunkyArrayA(RNDMAP *pal, UBYTE *src, RNDPAL *srcpal, UWORD width, UWORD height, UBYTE *dst, struct TagItem *tags)
{
	ULONG result = CONV_NO_DATA;
	if (pal && src && srcpal && width && height && dst)
	{	
		ObtainSemaphore(&pal->lock);
		ObtainSemaphoreShared(&srcpal->lock);
		
		result = CONV_NOT_ENOUGH_MEMORY;
		if (GetP2Table(pal))
		{
			LONG tsw = GetTagData(RND_SourceWidth, width, tags);
			LONG tdw = GetTagData(RND_DestWidth, width, tags);
			UBYTE *pentab = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);
			LONG y, x;

			if (pentab)
			{
				for (y = 0; y < height; ++y)
				{
					for (x = 0; x < width; ++x)
					{
						*dst++ = pentab[P2Lookup(pal, srcpal->table[*src++])];
					}
					src += tsw - width;
					dst += tdw - width;
				}
			}
			else
			{
				for (y = 0; y < height; ++y)
				{
					for (x = 0; x < width; ++x)
					{
						*dst++ = P2Lookup(pal, srcpal->table[*src++]);
					}
					src += tsw - width;
					dst += tdw - width;
				}
			}
			result = CONV_SUCCESS;
		}

		ReleaseSemaphore(&srcpal->lock);
		ReleaseSemaphore(&pal->lock);
	}
	return result;
}
