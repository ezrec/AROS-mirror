/*********************************************************************
----------------------------------------------------------------------

	guigfx_convolve

----------------------------------------------------------------------
*********************************************************************/

#include <render/render.h>
#include <render/renderhooks.h>
#include <utility/tagitem.h>
#include <graphics/gfx.h>
#include <graphics/view.h>
#include <exec/memory.h>

#include <proto/render.h>
#include <proto/graphics.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/cybergraphics.h>
#include <libraries/cybergraphics.h>

#include <guigfx/guigfx.h>

#include "guigfx_global.h"
#include "guigfx_convolve.h"
#include "guigfx_picturemethod.h"

#define DEFAULT_SPATSIZEX	1
#define DEFAULT_SPATSIZEY	1


int defaultspatkernel_blur[(DEFAULT_SPATSIZEX * 2 + 1) * (DEFAULT_SPATSIZEY * 2 + 1)] =
{
	0.0*255,	1.0*255,	0.0*255,
	1.0*255,	2.0*255,	1.0*255,
	0.0*255,	1.0*255,	0.0*255
	// div: 6
	// inc: 0
};

int defaultspatkernel_emboss[(DEFAULT_SPATSIZEX * 2 + 1) * (DEFAULT_SPATSIZEY * 2 + 1)] =
{
	0,0,0,
	0,99,0,
	0,0,-99
	// div: 1
	// inc: 204
};

int defaultspatkernel_rise[(DEFAULT_SPATSIZEX * 2 + 1) * (DEFAULT_SPATSIZEY * 2 + 1)] =
{
	0.0*255,	0.0*255,	0.0*255,
	0.0*255,	2.0*255,	0.0*255,
	0.0*255,	0.0*255,	-1.0*255
	// div: 1
	// inc: 0
};

int defaultspatkernel_sharpen[(DEFAULT_SPATSIZEX * 2 + 1) * (DEFAULT_SPATSIZEY * 2 + 1)] =
{
	00,	-1,	00,
	-1,	10,	-1,
	00,	-1,	00
	// div: 6
	// inc: 0
};

KERNEL defaultkernel =
{
	{1.0,0.0,0.0,
	 0.0,1.0,0.0,
	 0.0,0.0,1.0,
	 0.0,0.0,0.0},
//	defaultspatkernel_sharpen,
	defaultspatkernel_emboss,
	DEFAULT_SPATSIZEX, DEFAULT_SPATSIZEY,
	BORDERMODE_CONTINOUS,
	0x0,
	1, 0
};

#define	GETRGB(b,x,y)	((b) + ((((x) + destwidth) % destwidth) + (((y) + destheight) % destheight) * destwidth))
#define	GETSPAT(b,x,y) ((b)->spatkernel + ((x) + (b)->spatwidth) + ((y) + (b)->spatheight) * ((b)->spatwidth * 2 + 1))


ULONG PIC_Convolve(PIC *pic, KERNEL *kernel, TAGLIST tags)
{
	BOOL success = FALSE;
	UWORD destwidth, destheight, destx, desty;

	if (pic)
	{
		if (!kernel) kernel = &defaultkernel;

		ObtainSemaphore(&pic->semaphore);

		destwidth = GetTagData(GGFX_DestWidth, pic->width, tags);
		destheight = GetTagData(GGFX_DestHeight, pic->height, tags);
		destx = GetTagData(GGFX_DestX, 0, tags);
		desty = GetTagData(GGFX_DestY, 0, tags);


		if (ExtractAlphaArray(pic))
		{
			if (PIC_Render(pic, PIXFMT_0RGB_32, NULL))
			{
				ULONG *dest;
				ULONG *source = ((ULONG *) pic->array) + destx + desty * pic->width;
				
				if ((dest = AllocRenderVec(MemHandler, destwidth * destheight * 4)))
				{
					int x,y,c,d;
					ULONG *sbufptr = source;
					ULONG *dbufptr = dest;
					int SUMR, SUMG, SUMB;
					int k, div, inc;
					ULONG rgb;
					
					div = kernel->divisor;
					inc = kernel->increment;
					
					for (y = 0; y < destheight; y++) 
					{
						for (x = 0; x < destwidth; x++) 
						{
							SUMR = 0;
							SUMG = 0;
							SUMB = 0;
							
							for (d = -kernel->spatheight; d < kernel->spatheight + 1; d++)
							{
								for (c = -kernel->spatwidth; c < kernel->spatwidth + 1; c++)
								{
									k = *GETSPAT(kernel, c, d);
									rgb = *GETRGB(sbufptr, x + c, y + d);
									
								#ifdef GUIGFX_LITTLE_ENDIAN
									SUMR += ((rgb & 0x0000ff00) >> 8) * k;
									SUMG += ((rgb & 0x00ff0000) >> 16) * k;
									SUMB += ((rgb & 0xff000000) >> 24) * k;
								#else
									SUMR += ((rgb & 0xff0000) >> 16) * k;
									SUMG += ((rgb & 0x00ff00) >> 8) * k;
									SUMB += (rgb & 0x0000ff) * k;
								#endif
								}
							}
							
						#ifdef GUIGFX_LITTLE_ENDIAN
							*dbufptr++ =	((RNG(0, (SUMR / div), 255)) << 8) +
									((RNG(0, (SUMG / div), 255)) << 16) +
									((RNG(0, (SUMB / div), 255)) << 24);
						#else
							*dbufptr++ =	((RNG(0, (SUMR / div), 255)) << 16) +
														((RNG(0, (SUMG / div), 255)) << 8) +
														 RNG(0, (SUMB / div), 255);
						#endif
						}
					}
				
					dbufptr = dest;	
					for (y = 0; y < destheight; ++y)
					{
						for (x = 0; x < destwidth; ++x)
						{
							*source++ = *dbufptr++;
						}
						dbufptr += destwidth - pic->width;
					}
	
					FreeRenderVec(dest);
				}
	
				success = TRUE;
			}
		}
	}

	ReleaseSemaphore(&pic->semaphore);

	return (ULONG) success;
}
