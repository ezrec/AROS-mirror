
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>


LIBAPI void InsertAlphaChannelA(UBYTE *chunky, UWORD width, UWORD height, ULONG *rgb, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	LONG x;
	ULONG col;
	if (!chunky || !width || !height || !rgb) return;

	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			col = *rgb;
		#ifdef RENDER_LITTLE_ENDIAN
			col &= 0xffffff00;
			col |= *chunky++;
		#else
			col &= 0x00ffffff;
			col |= *chunky++ << 24;
		#endif
			*rgb++ = col;
		}
		rgb += tsw - width;
		chunky += tdw - width;
	}
}

LIBAPI void ExtractAlphaChannelA(ULONG *rgb, UWORD width, UWORD height, UBYTE *chunky, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	LONG x;
	if (!chunky || !width || !height || !rgb) return;

	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
		#ifdef RENDER_LITTLE_ENDIAN
			*chunky++ = (*rgb++) & 0xFF;
		#else
			*chunky++ = *rgb++ >> 24;
		#endif
		}
		rgb += tsw - width;
		chunky += tdw - width;
	}
}

LIBAPI void ApplyAlphaChannelA(ULONG *src, UWORD width, UWORD height, ULONG *dst, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	LONG taw = GetTagData(RND_AlphaWidth, width, tags);
	UBYTE *alpha = (UBYTE *) GetTagData(RND_AlphaChannel, (ULONG) src, tags);
	LONG alphaoffs = GetTagData(RND_AlphaModulo, 4, tags);
	LONG x, s, d, a;
	ULONG col1, col2, rgb;
	if (!src || !width || !height || !dst || !alpha) return;
	
	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			a = *alpha;
			col1 = *src++;
			col2 = *dst;

			s = (col1 & 0xff0000) >> 16;
			d = (col2 & 0xff0000) >> 16;
			s += (((d - s) * a) >> 8);
			rgb = s << 8;

			s = (col1 & 0x00ff00) >> 8;
			d = (col2 & 0x00ff00) >> 8;
			rgb |= s + (((d - s) * a) >> 8);
			rgb <<= 8;

		#ifdef RENDER_LITTLE_ENDIAN
			s = (col1 & 0xff000000) >> 24;
			d = (col2 & 0xff000000) >> 24;
			rgb |= (s + (((d - s) * a) >> 8)) << 24;
			
			rgb |= (col2 & 0xff);			/* keep alpha channel in dest */
		#else
			s = (col1 & 0x0000ff);
			d = (col2 & 0x0000ff);
			rgb |= s + (((d - s) * a) >> 8);
			
			rgb |= (col2 & 0xff000000);		/* keep alpha channel in dest */
		#endif			
			*dst++ = rgb;
			alpha += alphaoffs;
		}

		alpha += (taw - width) * alphaoffs;
		src += tsw - width;
		dst += tdw - width;
	}
}

LIBAPI void TintRGBArrayA(ULONG *src, UWORD width, UWORD height, ULONG rgb, UWORD ratio, ULONG *dst, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	LONG r = (rgb & 0xff0000) >> 16;
	LONG g = (rgb & 0x00ff00) >> 8;
	LONG b = (rgb & 0x0000ff);
	LONG s, x;
	ULONG col;

	if (!src || !width || !height || !dst) return;

	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			col = *src++;
		#ifdef RENDER_LITTLE_ENDIAN
			s = (col & 0xff0000) >> 16;
			s += (((g - s) * ratio) >> 8);
			rgb = s << 8;

			s = (col & 0x00ff00) >> 8;
			rgb |= s + (((r - s) * ratio) >> 8);
			rgb <<= 8;

			s = (col & 0xff000000) >> 24;
			rgb |= (s + (((b - s) * ratio) >> 8)) << 24;
			
			rgb |= (col & 0xff);			/* keep alpha channel in dest */
		#else
			s = (col & 0xff0000) >> 16;
			s += (((r - s) * ratio) >> 8);
			rgb = s << 8;

			s = (col & 0x00ff00) >> 8;
			rgb |= s + (((g - s) * ratio) >> 8);
			rgb <<= 8;

			s = (col & 0x0000ff);
			rgb |= s + (((b - s) * ratio) >> 8);
			
			rgb |= (col & 0xff000000);		/* keep alpha channel in dest */
		#endif			
			*dst++ = rgb;
		}
		src += tsw - width;
		dst += tdw - width;
	}
}

LIBAPI void MixRGBArrayA(ULONG *src, UWORD width, UWORD height, ULONG *dst, UWORD ratio, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	LONG x, s, d;
	ULONG col1, col2, rgb;
	if (!src || !width || !height || !dst) return;
	
	ratio = 255 - ratio;		/* ! */
	
	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			col1 = *src++;
			col2 = *dst;

			s = (col1 & 0xff0000) >> 16;
			d = (col2 & 0xff0000) >> 16;
			s += (((d - s) * ratio) >> 8);
			rgb = s << 8;

			s = (col1 & 0x00ff00) >> 8;
			d = (col2 & 0x00ff00) >> 8;
			rgb |= s + (((d - s) * ratio) >> 8);
			rgb <<= 8;

		#ifdef RENDER_LITTLE_ENDIAN
			s = (col1 & 0xff000000) >> 24;
			d = (col2 & 0xff000000) >> 24;
			rgb |= (s + (((d - s) * ratio) >> 8)) << 24;
			
			rgb |= (col2 & 0xff);			/* keep alpha channel in dest */
		#else
			s = (col1 & 0x0000ff);
			d = (col2 & 0x0000ff);
			rgb |= s + (((d - s) * ratio) >> 8);
			
			rgb |= (col2 & 0xff000000);		/* keep alpha channel in dest */
		#endif
			*dst++ = rgb;
		}
		src += tsw - width;
		dst += tdw - width;
	}
}

LIBAPI void MixAlphaChannelA(ULONG *src1, ULONG *src2, UWORD width, UWORD height, ULONG *dst, struct TagItem *tags)
{
	LONG tsw1 = GetTagData(RND_SourceWidth, width, tags);
	LONG tsw2 = GetTagData(RND_SourceWidth2, width, tags);
	LONG tdw = GetTagData(RND_DestWidth, width, tags);
	UBYTE *alpha1 = (UBYTE *) GetTagData(RND_AlphaChannel, (ULONG) src1, tags);
	UBYTE *alpha2 = (UBYTE *) GetTagData(RND_AlphaChannel2, (ULONG) src2, tags);
	LONG taw1 = GetTagData(RND_AlphaWidth, width, tags);
	LONG taw2 = GetTagData(RND_AlphaWidth2, width, tags);
	LONG aoffs1 = GetTagData(RND_AlphaModulo, 4, tags);
	LONG aoffs2 = GetTagData(RND_AlphaModulo2, 4, tags);
	LONG x, a, s, d;
	ULONG col1, col2, rgb;

	if (!src1 || !src2 || !width || !height || !dst) return;

	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			a = 0;

			if (alpha2)
			{
				a = 255 - *alpha2;
				alpha2 += aoffs2;
			}

			if (alpha1)
			{
				a += *alpha1;
				alpha1 += aoffs1;

				if (alpha2) a >>= 1;
			}
			
			col1 = *src1++;
			col2 = *src2++;
			
			s = (col1 & 0xff0000) >> 16;
			d = (col2 & 0xff0000) >> 16;
			d += (((s - d) * a) >> 8);
			rgb = d << 8;

			s = (col1 & 0x00ff00) >> 8;
			d = (col2 & 0x00ff00) >> 8;
			rgb |= d + (((s - d) * a) >> 8);
			rgb <<= 8;

		#ifdef RENDER_LITTLE_ENDIAN
			s = (col1 & 0xff000000) >> 24;
			d = (col2 & 0xff000000) >> 24;
			rgb |= (d + (((s - d) * a) >> 8)) << 24;
			
			rgb |= (*dst & 0xff);			/* keep alpha channel in dest */
		#else
			s = (col1 & 0x0000ff);
			d = (col2 & 0x0000ff);
			rgb |= d + (((s - d) * a) >> 8);
			
			rgb |= (*dst & 0xff000000);		/* keep alpha channel in dest */
		#endif
			*dst++ = rgb;
		}
		
		if (alpha1) alpha1 += (taw1 - width) * aoffs1;
		if (alpha2) alpha2 += (taw2 - width) * aoffs2;
		
		src1 += tsw1 - width;
		src2 += tsw2 - width;
		dst += tdw - width;
	}
}



#define ABS(x) ((x) >= 0 ? (x) : -(x))

LIBAPI void CreateAlphaArrayA(ULONG *rgb, UWORD width, UWORD height, struct TagItem *tags)
{
	LONG tsw = GetTagData(RND_SourceWidth, width, tags);
	LONG taw = GetTagData(RND_AlphaWidth, width, tags);
	UBYTE *dst = (UBYTE *) GetTagData(RND_AlphaChannel, (ULONG) rgb, tags);
	LONG aoffs = GetTagData(RND_AlphaModulo, 4, tags);
	ULONG maskrgb = GetTagData(RND_MaskRGB, 0x000000, tags);
	LONG x;
	ULONG col;
	LONG r, g, b;

	if (!width || !height || !rgb || !dst) return;

	while (height--)
	{
		for (x = 0; x < width; ++x)
		{
			col = *rgb++;
		#ifdef RENDER_LITTLE_ENDIAN
			r = (maskrgb & 0xff0000) >> 16;
			r -= (col & 0x00ff00) >> 8;
			g = (maskrgb & 0x00ff00) >> 8;
			g -= (col & 0xff0000) >> 16;
			b = (maskrgb & 0x0000ff);
			b -= (col & 0xff000000) >> 24;
		#else			
			r = (maskrgb & 0xff0000) >> 16;
			r -= (col & 0xff0000) >> 16;
			g = (maskrgb & 0x00ff00) >> 8;
			g -= (col & 0x00ff00) >> 8;
			b = (maskrgb & 0x0000ff);
			b -= (col & 0x0000ff);
		#endif
			*dst = 0.299f * ABS(r) + 0.587f * ABS(g) + 0.114f * ABS(b);
			dst += aoffs;
		}
		
		rgb += tsw - width;
		dst += (taw - width) * aoffs;
	}
}
