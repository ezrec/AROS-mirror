
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <graphics/gfx.h>

#define NUMRAND	1237		/* prime number */

static LONG getrand(LONG seed)
{
	ULONG lo, hi;

	lo = 16807 * (LONG) (seed & 0xffff);
	hi = 16807 * (LONG) ((ULONG) seed >> 16);
	lo += (hi & 0x7fff) << 16;
	if (lo > 2147483647)
	{
		lo &= 2147483647;
		++lo;
	}
	lo += hi >> 15;
	if (lo > 2147483647)
	{
		lo &= 2147483647;
		++lo;
	}

	return (LONG) lo;
}

/************************************************************************** 
**
**	callbacks for rendering
**
**************************************************************************/

/* 
**	floyd:			P	A=7
**				B=3	C=5	D=1
*/

static __inline UBYTE dither_floyd(struct RenderData *rnd, WORD *dithbuf, WORD r, WORD g, WORD b)
{
	ULONG rgb;
	UBYTE pen;
	WORD A, B;

	r += rnd->Ar;
	g += rnd->Ag;
	b += rnd->Ab;

	if (r < 0) r = 0; else if (r > 255) r = 255;
	if (g < 0) g = 0; else if (g > 255) g = 255;
	if (b < 0) b = 0; else if (b > 255) b = 255;

	pen = P2Lookup2(rnd->dstpal, r, g, b);
	rgb = rnd->dstpal->table[pen];					

	b -= rgb & 0xff;							
	rgb >>= 8;									
	g -= rgb & 0xff;							
	rgb >>= 8;									
	r -= rgb & 0xff;

	B = r + r;
	A = (B + r) >> 4;						
	dithbuf[0] = rnd->Br + A;
	B = (B + B + r) >> 4;
	A += B;
	rnd->Br = rnd->Cr + B;
	B = r >> 4;
	rnd->Cr = B;
	rnd->Ar = r - A - B;

	B = g + g;
	A = (B + g) >> 4;
	dithbuf[1] = rnd->Bg + A;
	B = (B + B + g) >> 4;
	A += B;
	rnd->Bg = rnd->Cg + B;
	B = g >> 4;
	rnd->Cg = B;
	rnd->Ag = g - A - B;

	B = b + b;
	A = (B + b) >> 4;
	dithbuf[2] = rnd->Bb + A;
	B = (B + B + b) >> 4;
	A += B;
	rnd->Bb = rnd->Cb + B;
	B = b >> 4;
	rnd->Cb = B;
	rnd->Ab = b - A - B;

	return pen;
}

/************************************************************************** 
**	renderx: rgb dither floyd
*/

static void renderx_rgb_dither_floyd(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	WORD *dithbuf = rnd->ditherbuf + 3;
	ULONG rgb;
	WORD r, g, b;
	
	rnd->Br = 0;
	rnd->Cr = 0;
	rnd->Ar = 0;
	rnd->Bg = 0;
	rnd->Cg = 0;
	rnd->Ag = 0;
	rnd->Bb = 0;
	rnd->Cb = 0;
	rnd->Ab = 0;

	if (rnd->flipflop ^= 1)
	{
		while (width--)
		{
			rgb = *src++;
		#ifdef RENDER_LITTLE_ENDIAN
			r = *dithbuf++ + ((rgb & 0x0000ff00) >> 8);
			g = *dithbuf++ + ((rgb & 0x00ff0000) >> 16);
			b = *dithbuf++ + ((rgb & 0xff000000) >> 24);
		#else
			r = *dithbuf++ + ((rgb & 0xff0000) >> 16);
			g = *dithbuf++ + ((rgb & 0x00ff00) >> 8);
			b = *dithbuf++ + (rgb & 0x0000ff);
		#endif
			*dst++ = rnd->pentab[dither_floyd(rnd, dithbuf - 3, r, g, b)];
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			rgb = *--src;
		#ifdef RENDER_LITTLE_ENDIAN
			b = *--dithbuf + ((rgb & 0xff000000) >> 24);
			g = *--dithbuf + ((rgb & 0x00ff0000) >> 16);
			r = *--dithbuf + ((rgb & 0x0000ff00) >> 8);
		#else
			b = *--dithbuf + (rgb & 0x0000ff);
			g = *--dithbuf + ((rgb & 0x00ff00) >> 8);
			r = *--dithbuf + ((rgb & 0xff0000) >> 16);
		#endif
			*--dst = rnd->pentab[dither_floyd(rnd, dithbuf + 3, r, g, b)];
		}
	}
}


/************************************************************************** 
**	renderx: chunky dither floyd
*/

static void renderx_chunky_dither_floyd(struct RenderData *rnd, UBYTE *src, UBYTE *dst, LONG width)
{
	RNDPAL *srcpal = rnd->srcpal;
	WORD *dithbuf = rnd->ditherbuf + 3;
	ULONG rgb;
	WORD r, g, b;
	
	rnd->Br = 0;
	rnd->Cr = 0;
	rnd->Ar = 0;
	rnd->Bg = 0;
	rnd->Cg = 0;
	rnd->Ag = 0;
	rnd->Bb = 0;
	rnd->Cb = 0;
	rnd->Ab = 0;

	if (rnd->flipflop ^= 1)
	{
		while (width--)
		{
			rgb = srcpal->table[*src++];
			r = *dithbuf++ + ((rgb & 0xff0000) >> 16);
			g = *dithbuf++ + ((rgb & 0x00ff00) >> 8);
			b = *dithbuf++ + (rgb & 0x0000ff);
			*dst++ = rnd->pentab[dither_floyd(rnd, dithbuf - 3, r, g, b)];
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			rgb = srcpal->table[*--src];
			b = *--dithbuf + (rgb & 0x0000ff);
			g = *--dithbuf + ((rgb & 0x00ff00) >> 8);
			r = *--dithbuf + ((rgb & 0xff0000) >> 16);
			*--dst = rnd->pentab[dither_floyd(rnd, dithbuf + 3, r, g, b)];
		}
	}
}


/* 
**	edd:			P	A=1
**					B=1
*/

#define dither_edd(rgb)											\
	r += dithbuf[0];											\
	g += dithbuf[1];											\
	b += dithbuf[2];											\
	b += rgb & 0xff;											\
	rgb >>= 8;													\
	g += rgb & 0xff;											\
	rgb >>= 8;													\
	r += rgb & 0xff;											\
	if (r < 0) r = 0; else if (r > 255) r = 255;				\
	if (g < 0) g = 0; else if (g > 255) g = 255;				\
	if (b < 0) b = 0; else if (b > 255) b = 255;				\
	rgb = r;													\
	rgb <<= 8;													\
	rgb |= g;													\
	rgb <<= 8;													\
	rgb |= b;													\
	pen = P2Lookup(dstpal, rgb);								\
	rgb = dstpal->table[pen];									\
	r -= (rgb & 0xff0000) >> 16;								\
	g -= (rgb & 0x00ff00) >> 8;									\
	b -= (rgb & 0x0000ff);										\
	r -= dithbuf[0] = r >> 1;									\
	g -= dithbuf[1] = g >> 1;									\
	b -= dithbuf[2] = b >> 1;


/************************************************************************** 
**	renderx: rgb dither EDD
*/

static void renderx_rgb_dither_edd(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *dstpal = rnd->dstpal;
	WORD *dithbuf = rnd->ditherbuf;
	ULONG rgb;
	WORD r = 0, g = 0, b = 0;
	UBYTE pen;

	if (rnd->flipflop ^= 1)
	{	
		while (width--)
		{
		#ifdef RENDER_LITTLE_ENDIAN
		    	ULONG pix = *src++;
			rgb = AROS_BE2LONG(pix);
		#else
			rgb = *src++;
		#endif
			dither_edd(rgb);
			*dst++ = pentab[pen];
			dithbuf += 3;
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			dithbuf -= 3;
			
		#ifdef RENDER_LITTLE_ENDIAN
		    	{
			    	ULONG pix = *--src;
			    	rgb = AROS_BE2LONG(pix);
			}
		#else
			rgb = *--src;
		#endif
			dither_edd(rgb);
			*--dst = pentab[pen];
		}
	}
}


/************************************************************************** 
**	renderx: chunky dither EDD
*/

static void renderx_chunky_dither_edd(struct RenderData *rnd, UBYTE *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;
	WORD *dithbuf = rnd->ditherbuf;
	ULONG rgb;
	WORD r = 0, g = 0, b = 0;
	UBYTE pen;

	if (rnd->flipflop ^= 1)
	{	
		while (width--)
		{
			rgb = srcpal->table[*src++];
			dither_edd(rgb);
			*dst++ = pentab[pen];
			dithbuf += 3;
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			dithbuf -= 3;
			rgb = srcpal->table[*--src];
			dither_edd(rgb);
			*--dst = pentab[pen];
		}
	}
}



/* 
**	random dither
*/

#define dither_random(rgb)										\
	b += rgb & 0xff;											\
	rgb >>= 8;													\
	g += rgb & 0xff;											\
	rgb >>= 8;													\
	r += rgb & 0xff;											\
	if (r < 0) r = 0; else if (r > 255) r = 255;				\
	if (g < 0) g = 0; else if (g > 255) g = 255;				\
	if (b < 0) b = 0; else if (b > 255) b = 255;				\
	rgb = r;													\
	rgb <<= 8;													\
	rgb |= g;													\
	rgb <<= 8;													\
	rgb |= b;													\
	pen = P2Lookup(dstpal, rgb);								\
	rgb = dstpal->table[pen];									\
	b -= rgb & 0xff;											\
	rgb >>= 8;													\
	g -= rgb & 0xff;											\
	rgb >>= 8;													\
	r -= rgb & 0xff;											\
	if (--randi < 0) randi = NUMRAND - 1;						\
	r += rnd->randtab[randi];									\
	if (--randi < 0) randi = NUMRAND - 1;						\
	g += rnd->randtab[randi];									\
	if (--randi < 0) randi = NUMRAND - 1;						\
	b += rnd->randtab[randi];


/************************************************************************** 
**	renderx: rgb dither random
*/

static void renderx_rgb_dither_rand(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *dstpal = rnd->dstpal;
	WORD *dithbuf = rnd->ditherbuf;
	ULONG rgb;
	WORD r = 0, g = 0, b = 0;
	UBYTE pen;
	WORD randi = rnd->randi;

	if (rnd->flipflop ^= 1)
	{	
		while (width--)
		{
		#ifdef RENDER_LITTLE_ENDIAN
		    	ULONG pix = *src++;
			rgb = AROS_BE2LONG(pix);
		#else
			rgb = *src++;
		#endif
			dither_random(rgb);
			*dst++ = pentab[pen];
			dithbuf += 3;
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			dithbuf -= 3;
		#ifdef RENDER_LITTLE_ENDIAN
		    	{
			    	ULONG pix = *--src;
			    	rgb = AROS_BE2LONG(pix);
			}
		#else
			rgb = *--src;
		#endif
			dither_random(rgb);
			*--dst = pentab[pen];
		}
	}
	
	rnd->randi = randi;
}


/************************************************************************** 
**	renderx: chunky dither random
*/

static void renderx_chunky_dither_rand(struct RenderData *rnd, UBYTE *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;
	WORD *dithbuf = rnd->ditherbuf;
	ULONG rgb;
	WORD r = 0, g = 0, b = 0;
	UBYTE pen;
	WORD randi = rnd->randi;

	if (rnd->flipflop ^= 1)
	{	
		while (width--)
		{
			rgb = srcpal->table[*src++];
			dither_random(rgb);
			*dst++ = pentab[pen];
			dithbuf += 3;
		}
	}
	else
	{
		src += width;
		dst += width;
		dithbuf += width * 3;
		while (width--)
		{
			dithbuf -= 3;
			rgb = srcpal->table[*--src];
			dither_random(rgb);
			*--dst = pentab[pen];
		}
	}

	rnd->randi = randi;
}




/************************************************************************** 
**	renderx: rgb plain
*/

static void renderx_rgb_plain(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	RNDPAL *dstpal = rnd->dstpal;
	while (width--)
	{
	#ifdef RENDER_LITTLE_ENDIAN
	    	ULONG pix = *src++;
		*dst++ = P2Lookup(dstpal, AROS_BE2LONG(pix));
	#else
		*dst++ = P2Lookup(dstpal, *src++);
	#endif
	}
}

/************************************************************************** 
**	renderx: rgb with pentab
*/

static void renderx_rgb_pentab(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *dstpal = rnd->dstpal;
	while (width--)
	{
	#ifdef RENDER_LITTLE_ENDIAN
	    	ULONG pix = *src++;
		*dst++ = pentab[P2Lookup(dstpal, AROS_BE2LONG(pix))];
	#else
		*dst++ = pentab[P2Lookup(dstpal, *src++)];
    	#endif
	}
}



#define lookupham6(rgb,r,g,b)							\
{	LONG nr, ng, nb, d, bestd; ULONG rgb2; UBYTE p2;	\
	nr = (rgb & 0xf00000) >> 20;						\
	ng = (rgb & 0x00f000) >> 12;					\
	nb = (rgb & 0x0000f0) >> 4;						\
	bestd = (ng-g)*(ng-g) + (nb-b)*(nb-b);			\
	pen = 0x20 | nr;								\
	rgb2 = (nr << 8) | (g << 4) | b;				\
	d = (nr-r)*(nr-r) + (nb-b)*(nb-b);				\
	if (d < bestd)									\
	{												\
		bestd = d;									\
		pen = 0x30 | ng;							\
		rgb2 = (r << 8) | (ng << 4) | b;			\
	}												\
	d = (nr-r)*(nr-r) + (ng-g)*(ng-g);				\
	if (d < bestd)									\
	{												\
		bestd = d;									\
		pen = 0x10 | nb;							\
		rgb2 = (r << 8) | (g << 4) | nb;			\
	}												\
	p2 = P2Lookup(dstpal, rgb);						\
	rgb = dstpal->table[p2];						\
	r = (rgb & 0xf00000) >> 20;						\
	g = (rgb & 0x00f000) >> 12;						\
	b = (rgb & 0x0000f0) >> 4;						\
	nr -= r;										\
	ng -= g;										\
	nb -= b;										\
	if (nr*nr + ng*ng + nb*nb < bestd)				\
	{												\
		pen = pentab[p2];							\
	}												\
	else											\
	{												\
		r = (rgb2 & 0xf00) >> 8;					\
		g = (rgb2 & 0x0f0) >> 4;					\
		b = (rgb2 & 0x00f);							\
	}												\
}

/************************************************************************** 
**	renderx: rgb to ham6
*/

static void renderx_rgb_ham6(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *dstpal = rnd->dstpal;

	LONG hr = (dstpal->table[0] & 0xf00000) >> 20;
	LONG hg = (dstpal->table[0] & 0x00f000) >> 12;
	LONG hb = (dstpal->table[0] & 0x0000f0) >> 4;
	ULONG rgb;
	UBYTE pen;
	
	while (width--)
	{
		rgb = *src++;
	#ifdef RENDER_LITTLE_ENDIAN
	    	rgb = AROS_BE2LONG(rgb);
	#endif
		lookupham6(rgb, hr,hg,hb);
		*dst++ = pen;
	}
}

/************************************************************************** 
**	renderx: chunky to ham6
*/

static void renderx_chunky_ham6(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;

	LONG hr = (dstpal->table[0] & 0xf00000) >> 20;
	LONG hg = (dstpal->table[0] & 0x00f000) >> 12;
	LONG hb = (dstpal->table[0] & 0x0000f0) >> 4;
	ULONG rgb;
	UBYTE pen;
	
	while (width--)
	{
	#ifdef RENDER_LITTLE_ENDIAN
	    	ULONG pix = *src++;
		rgb = srcpal->table[AROS_BE2LONG(pix)];
	#else
		rgb = srcpal->table[*src++];
	#endif
		lookupham6(rgb, hr,hg,hb);
		*dst++ = pen;
	}
}




#define lookupham8(rgb,r,g,b)							\
{	LONG nr, ng, nb, d, bestd; ULONG rgb2; UBYTE p2;	\
	nr = (r & 3) | ((rgb & 0xfc0000) >> 16);			\
	ng = (g & 3) | ((rgb & 0x00fc00) >> 8);			\
	nb = (b & 3) | (rgb & 0x0000fc);				\
	bestd = (ng-g)*(ng-g) + (nb-b)*(nb-b);			\
	pen = 0x80 | (nr>>2);							\
	rgb2 = (nr << 16) | (g << 8) | b;				\
	d = (nr-r)*(nr-r) + (nb-b)*(nb-b);				\
	if (d < bestd)									\
	{												\
		bestd = d;									\
		pen = 0xc0 | (ng>>2);						\
		rgb2 = (r << 16) | (ng << 8) | b;			\
	}												\
	d = (nr-r)*(nr-r) + (ng-g)*(ng-g);				\
	if (d < bestd)									\
	{												\
		bestd = d;									\
		pen = 0x40 | (nb>>2);						\
		rgb2 = (r << 16) | (g << 8) | nb;			\
	}												\
	p2 = P2Lookup(dstpal, rgb);						\
	rgb = dstpal->table[p2];						\
	r = (rgb & 0xff0000) >> 16;						\
	g = (rgb & 0x00ff00) >> 8;						\
	b = (rgb & 0x0000ff);							\
	nr -= r;										\
	ng -= g;										\
	nb -= b;										\
	if (nr*nr + ng*ng + nb*nb < bestd)				\
	{												\
		pen = pentab[p2];							\
	}												\
	else											\
	{												\
		r = (rgb2 & 0xff0000) >> 16;				\
		g = (rgb2 & 0x00ff00) >> 8;					\
		b = (rgb2 & 0x0000ff);						\
	}												\
}

/************************************************************************** 
**	renderx: rgb to ham8
*/

static void renderx_rgb_ham8(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *dstpal = rnd->dstpal;

	LONG hr = (dstpal->table[0] & 0xff0000) >> 16;
	LONG hg = (dstpal->table[0] & 0x00ff00) >> 8;
	LONG hb = (dstpal->table[0] & 0x0000ff);
	ULONG rgb;
	UBYTE pen;
	
	while (width--)
	{
		rgb = *src++;
	#ifdef RENDER_LITTLE_ENDIAN
	    	rgb = AROS_BE2LONG(rgb);
	#endif
		lookupham8(rgb, hr,hg,hb);
		*dst++ = pen;
	}
}

/************************************************************************** 
**	renderx: chunky to ham8
*/

static void renderx_chunky_ham8(struct RenderData *rnd, ULONG *src, UBYTE *dst, LONG width)
{
	UBYTE *pentab = rnd->pentab;
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;

	LONG hr = (dstpal->table[0] & 0xff0000) >> 16;
	LONG hg = (dstpal->table[0] & 0x00ff00) >> 8;
	LONG hb = (dstpal->table[0] & 0x0000ff);
	ULONG rgb;
	UBYTE pen;
	
	while (width--)
	{
	#ifdef RENDER_LITTLE_ENDIAN
	    	ULONG pix = *src++;
		rgb = srcpal->table[AROS_BE2LONG(pix)];
	#else
		rgb = srcpal->table[*src++];
	#endif
		lookupham8(rgb, hr,hg,hb);
		*dst++ = pen;
	}
}




/************************************************************************** 
**	renderx: chunky plain
*/

static void renderx_chunky_plain(struct RenderData *rnd, UBYTE *src, UBYTE *dst, LONG width)
{
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;
	while (width--)
	{
		*dst++ = P2Lookup(dstpal, srcpal->table[*src++]);
	}
}

/************************************************************************** 
**	renderx: chunky with pentab
*/

static void renderx_chunky_pentab(struct RenderData *rnd, UBYTE *src, UBYTE *dst, LONG width)
{
	RNDPAL *srcpal = rnd->srcpal;
	RNDPAL *dstpal = rnd->dstpal;
	UBYTE *pentab = rnd->pentab;
	while (width--)
	{
		*dst++ = pentab[P2Lookup(dstpal, srcpal->table[*src++])];
	}
}




/************************************************************************** 
**	rendery: plain with hooks
*/

static ULONG rendery_plain_hooks(struct RenderData *rnd)
{
	ULONG result = REND_CALLBACK_ABORTED;
	UBYTE *src = rnd->src;
	UBYTE *dst = rnd->dst;
	LONG y;

	for (y = 0; y < rnd->height; ++y)
	{
		if (rnd->linehook)
		{
			rnd->fetchmsg.RND_LMsg_row = y;
			if (!CallHookPkt(rnd->linehook, src, &rnd->fetchmsg)) goto abort;
		}

		(*rnd->renderxfunc)(rnd, src, dst, rnd->width);

		if (rnd->linehook)
		{
			rnd->rendermsg.RND_LMsg_row = y;
			if (!CallHookPkt(rnd->linehook, dst, &rnd->rendermsg)) goto abort;
		}

		if (rnd->proghook)
		{
			rnd->progmsg.RND_PMsg_count = y;
			if (!CallHookPkt(rnd->proghook, rnd->dstpal, &rnd->progmsg)) goto abort;
		}

		src += rnd->tsw * rnd->srcbpp;
		dst += rnd->tdw * rnd->dstbpp;
	}

	result = REND_SUCCESS;

abort:
	return result;
}


/************************************************************************** 
**	rendery: with scale-engine and hooks
*/

static ULONG rendery_scale_hooks(struct RenderData *rnd)
{
	ULONG result = REND_NOT_ENOUGH_MEMORY;
	APTR linebuf;

	linebuf = AllocRenderMem(rnd->rmh, rnd->scaleengine->dw * rnd->srcbpp);
	if (linebuf)
	{
		if ((*rnd->scaleengine->initfunc)(rnd->scaleengine, &rnd->scdata))
		{
			UBYTE *dst = rnd->dst;
			LONG y;

			result = REND_CALLBACK_ABORTED;

			for (y = 0; y < rnd->scaleengine->dh; ++y)
			{
				UBYTE *src = rnd->src;

				if (rnd->bgcolor != 0xffffffff)
				{
					memfill32(linebuf, rnd->scaleengine->dw * rnd->srcbpp, rnd->bgcolor);
				}
				else if (rnd->bgpen != 0xffffffff)
				{
					memfill8(linebuf, rnd->scaleengine->dw * rnd->srcbpp, rnd->bgpen);
				}
				
				(*rnd->scaleengine->scalefunc)(rnd->scaleengine, &rnd->scdata, src, linebuf, y, rnd->tsw);
				(*rnd->renderxfunc)(rnd, linebuf, dst, rnd->scaleengine->dw);

				if (rnd->linehook)
				{
					rnd->rendermsg.RND_LMsg_row = y;
					if (!CallHookPkt(rnd->linehook, dst, &rnd->rendermsg)) goto abort;
				}
				
				if (rnd->proghook)
				{
					rnd->progmsg.RND_PMsg_count = y;
					if (!CallHookPkt(rnd->proghook, rnd->dstpal, &rnd->progmsg)) goto abort;
				}

				dst += rnd->tdw * rnd->dstbpp;
			}

			result = REND_SUCCESS;
abort:
			(*rnd->scaleengine->exitfunc)(rnd->scaleengine, &rnd->scdata);
		}
		FreeRenderMem(rnd->rmh, linebuf, rnd->scaleengine->dw * rnd->srcbpp);
	}
	
	return result;
}



/************************************************************************** 
**	init render data.
**	dstpal, width, height, src, dst, srcpal, srcbpp
**	must be initialized by the caller.
*/

static ULONG initrender(struct RenderData *rnd, struct TagItem *tags)
{
	LONG colormode = GetTagData(RND_ColorMode, COLORMODE_CLUT, tags);

	if (!rnd->dstpal) return REND_NO_DATA;
	if (!GetP2Table(rnd->dstpal)) return REND_NOT_ENOUGH_MEMORY;

	rnd->rmh = rnd->dstpal->rmh;
	rnd->dstbpp = 1;

	rnd->tsw = GetTagData(RND_SourceWidth, rnd->width, tags);
	rnd->tdw = GetTagData(RND_DestWidth, rnd->width, tags);

	rnd->pentab = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);
	if (!rnd->pentab)
	{
		LONG oz = GetTagData(RND_OffsetColorZero, 0, tags);

		LONG i;
		for (i = 0; i < 256; ++i)
		{
			rnd->pens[i] = i + oz;
		}

		if (oz)
		{
			rnd->pentab = rnd->pens;
		}
	}

	rnd->proghook = (struct Hook *) GetTagData(RND_ProgressHook, NULL, tags);
	rnd->linehook = (struct Hook *) GetTagData(RND_LineHook, NULL, tags);
	
	rnd->fetchmsg.RND_LMsg_type = LMSGTYPE_LINE_FETCH;
	rnd->rendermsg.RND_LMsg_type = LMSGTYPE_LINE_RENDERED;
	rnd->progmsg.RND_PMsg_type = PMSGTYPE_LINES_RENDERED;
	rnd->progmsg.RND_PMsg_total = rnd->height;

	rnd->scaleengine = (struct ScaleEngine *) GetTagData(RND_ScaleEngine, NULL, tags);
	if (rnd->scaleengine)
	{
		/* override */
		rnd->width = rnd->scaleengine->dw;
		rnd->height = rnd->scaleengine->dh;
		
		rnd->bgcolor = GetTagData(RND_BGColor, 0xffffffff, tags);
		rnd->bgpen = GetTagData(RND_BGPen, 0xffffffff, tags);
	}


	rnd->dithermode = GetTagData(RND_DitherMode, DITHERMODE_NONE, tags);
	if (rnd->dithermode != DITHERMODE_NONE)
	{
		rnd->ditherbuf = AllocRenderVecClear(rnd->rmh, sizeof(UWORD) * 3 * (rnd->width + 2));
		if (!rnd->ditherbuf) return REND_NOT_ENOUGH_MEMORY;
		if (rnd->dithermode == DITHERMODE_RANDOM)
		{
			LONG i, seed;
			LONG amount = GetTagData(RND_DitherAmount, 128, tags);
			rnd->randtab = AllocRenderMem(rnd->rmh, NUMRAND);
			if (!rnd->randtab) return REND_NOT_ENOUGH_MEMORY;
			for (i = 0; i < NUMRAND; ++i)
			{
				rnd->randtab[i] = (seed = getrand(seed)) % amount - amount/2;
			}
		}
	}
	

	/* determine renderx function */
	if (rnd->srcbpp == 1)
	{
		switch (colormode)
		{
			case COLORMODE_CLUT:
				switch (rnd->dithermode)
				{
					case DITHERMODE_EDD:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_dither_edd;
						break;
					case DITHERMODE_FS:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_dither_floyd;
						break;
					case DITHERMODE_RANDOM:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_dither_rand;
						break;

					default:
					case DITHERMODE_NONE:
						if (rnd->pentab)
						{
							rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_pentab;
						}
						else
						{
							rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_plain;
						}
						break;
				}
				break;
			
			case COLORMODE_HAM6:
				rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_ham6;
				break;

			case COLORMODE_HAM8:
				rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_chunky_ham8;
				break;
		}
	}
	else if (rnd->srcbpp == 4)
	{
		switch (colormode)
		{
			case COLORMODE_CLUT:
				switch (rnd->dithermode)
				{
					case DITHERMODE_EDD:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_dither_edd;
						break;
					case DITHERMODE_FS:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_dither_floyd;
						break;
					case DITHERMODE_RANDOM:
						rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_dither_rand;
						break;

					default:
					case DITHERMODE_NONE:
						if (rnd->pentab)
						{
							rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_pentab;
						}
						else
						{
							rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_plain;
						}
						break;
				}
				break;
			
			case COLORMODE_HAM6:
				rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_ham6;
				break;
			
			case COLORMODE_HAM8:
				rnd->renderxfunc = (void (*)(struct RenderData *, APTR, APTR, LONG)) renderx_rgb_ham8;
				break;
		}
	}
	
	/* determine rendery function */
	if (rnd->scaleengine)
	{
		rnd->renderyfunc = (ULONG (*)(struct RenderData *data)) rendery_scale_hooks;
	}
	else
	{
		rnd->renderyfunc = (ULONG (*)(struct RenderData *data)) rendery_plain_hooks;
	}


	if (!rnd->pentab) rnd->pentab = rnd->pens;

	return REND_SUCCESS;
}


static void exitrender(struct RenderData *data)
{
	if (data->randtab) FreeRenderMem(data->rmh, data->randtab, NUMRAND);
	FreeRenderVec((ULONG *) data->ditherbuf);
}


/************************************************************************** 
**
**	render
*/

LIBAPI ULONG RenderA(ULONG *src, UWORD w, UWORD h, UBYTE *dst, RNDPAL *dstpal, struct TagItem *tags)
{
	ULONG result = REND_NO_DATA;

	if (src && dst && w && h && dstpal)
	{
		struct RenderData data;
		memfill8((UBYTE *) &data, sizeof(struct RenderData), 0);
		
		ObtainSemaphore(&dstpal->lock);

		data.src = src;
		data.srcbpp = 4;
		data.dst = dst;
		data.width = w;		
		data.height = h;
		data.dstpal = dstpal;
		result = initrender(&data, tags);
		if (result == REND_SUCCESS)
		{
			result = (*data.renderyfunc)(&data);
		}
		exitrender(&data);

		ReleaseSemaphore(&dstpal->lock);
	}
	
	return result;
}


/************************************************************************** 
**
**	convertchunky
*/

LIBAPI ULONG ConvertChunkyA(UBYTE *src, RNDPAL *srcpal, UWORD w, UWORD h, UBYTE *dst, RNDPAL *dstpal, struct TagItem *tags)
{
	ULONG result = REND_NO_DATA;

	if (src && dst && w && h && dstpal)
	{
		struct RenderData data;
		memfill8((UBYTE *) &data, sizeof(struct RenderData), 0);

		ObtainSemaphore(&dstpal->lock);
		ObtainSemaphoreShared(&srcpal->lock);

		data.src = src;
		data.srcpal = srcpal;
		data.srcbpp = 1;
		data.dst = dst;
		data.dstpal = dstpal;
		data.width = w;		
		data.height = h;
		result = initrender(&data, tags);
		if (result == REND_SUCCESS)
		{
			result = (*data.renderyfunc)(&data);
		}
		exitrender(&data);

		ReleaseSemaphore(&srcpal->lock);
		ReleaseSemaphore(&dstpal->lock);
	}
	
	return result;
}
