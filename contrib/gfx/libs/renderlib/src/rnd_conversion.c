
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <graphics/gfx.h>

/************************************************************************** 
**
**	remaparray
*/

LIBAPI void RemapArrayA(UBYTE *src, UWORD width, UWORD height, 
	UBYTE *dst, UBYTE *pentab, struct TagItem *tags)
{
	if (src && width && height && dst && pentab)
	{
		LONG tsw = GetTagData(RND_SourceWidth, width, tags);
		LONG tdw = GetTagData(RND_DestWidth, width, tags);
		LONG y, x;
		
		for (y = 0; y < height; ++y)
		{
			for (x = 0; x < width; ++x)
			{
				*dst++ = pentab[*src++];
			}
			src += tsw - width;
			dst += tdw - width;
		}
	}
}

/************************************************************************** 
**
**	createpentable
*/

LIBAPI void CreatePenTableA(UBYTE *src, RNDPAL *srcpal, UWORD width, UWORD height, 
	RNDPAL *dstpal, UBYTE *pentab, struct TagItem *tags)
{
	if (src && srcpal && width && height && dstpal && pentab)
	{
		LONG y, x;
		LONG i;
		WORD table[256];
		LONG tsw = GetTagData(RND_SourceWidth, width, tags);
		UBYTE *secondary = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);

		memfill32((ULONG *) table, 256 * sizeof(WORD), 0xffffffff);
		
		for (y = 0; y < height; ++y)
		{
			for (x = 0; x < width; ++x)
			{
				i = *src++;
				table[i] = P2Lookup(dstpal, srcpal->table[i]);
			}
			src += tsw - width;
		}
		
		if (secondary)
		{
			for (i = 0; i < 256; ++i)
			{
				*pentab++ = secondary[table[i]];
			}
		}
		else
		{
			for (i = 0; i < 256; ++i)
			{
				*pentab++ = table[i];
			}
		}
	}
}

/************************************************************************** 
**
**	chunky2rgb
*/

LIBAPI ULONG Chunky2RGBA(UBYTE *src, UWORD width, UWORD height, 
	ULONG *dst, RNDPAL *pal, struct TagItem *tags)
{
	ULONG result = CONV_NO_DATA;
	if (src && width && height && dst && pal)
	{
		struct RND_ProgressMessage progmsg;
		struct RND_LineMessage linemsg;

		struct Hook *proghook = (struct Hook *) GetTagData(RND_ProgressHook, NULL, tags);
		struct Hook *linehook = (struct Hook *) GetTagData(RND_LineHook, NULL, tags);
		LONG tsw = GetTagData(RND_SourceWidth, width, tags);
		LONG tdw = GetTagData(RND_DestWidth, width, tags);
		ULONG colormode = GetTagData(RND_ColorMode, COLORMODE_CLUT, tags);
		LONG left = GetTagData(RND_LeftEdge, 0, tags);
		UBYTE *pentab = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);
		LONG y, x;

		progmsg.RND_PMsg_type = PMSGTYPE_LINES_CONVERTED;
		progmsg.RND_PMsg_total = height;

		result = CONV_CALLBACK_ABORTED;
		
		for (y = 0; y < height; ++y)
		{
			if (linehook)
			{
				linemsg.RND_LMsg_type = LMSGTYPE_LINE_FETCH;
				linemsg.RND_LMsg_row = y;
				if (!CallHookPkt(linehook, src, &linemsg)) goto abort;
			}		
		
			switch (colormode)
			{
				case COLORMODE_HAM6:
				{
					UBYTE p = 0, nib;
					ULONG col;
					if (pentab) p = pentab[0];
					col = pal->table[p] & 0xf0f0f0;
					col |= col >> 4;
					for (x = 0; x < width + left; ++x)
					{
						p = *src++;
						if (pentab) p = pentab[p];
						nib = p & 15;
						switch ((p >> 4) & 3)
						{
							case 0:
								col = pal->table[nib] & 0xf0f0f0;
								col |= col >> 4;
								break;
							case 2:
								col &= 0x00ffff;
								col |= nib << 20;
								col |= nib << 16;
								break;
							case 3:
								col &= 0xff00ff;
								col |= nib << 12;
								col |= nib << 8;
								break;
							case 1:
								col &= 0xffff00;
								col |= nib << 4;
								col |= nib;
								break;
						}
					#ifdef RENDER_LITTLE_ENDIAN
					    	if (x >= left) *dst++ = AROS_LONG2BE(col);
					#else
						if (x >= left) *dst++ = col;
					#endif
					}
					break;
				}
				
				case COLORMODE_HAM8:
				{
					UBYTE p = 0, nib;
					ULONG col;
					if (pentab) p = pentab[0];
					col = pal->table[p];
					for (x = 0; x < width + left; ++x)
					{
						p = *src++;
						if (pentab) p = pentab[p];
						nib = p & 63;
						switch ((p >> 6) & 3)
						{
							case 0:
								col = pal->table[nib];
								break;
							case 2:
								col &= 0x03ffff;
								col |= nib << 18;
								break;
							case 3:
								col &= 0xff03ff;
								col |= nib << 10;
								break;
							case 1:
								col &= 0xffff03;
								col |= nib << 2;
								break;
						}
					#ifdef RENDER_LITTLE_ENDIAN
					    	if (x >= left) *dst++ = AROS_LONG2BE(col);
					#else
						if (x >= left) *dst++ = col;
					#endif
					}
					break;
				}
	
				default:
				case COLORMODE_CLUT:
					src += left;
					if (pentab)
					{
						for (x = 0; x < width; ++x)
						{
						#ifdef RENDER_LITTLE_ENDIAN
						    	ULONG pix = pal->table[pentab[*src++]];
							*dst++ = AROS_LONG2BE(pix);
						#else
							*dst++ = pal->table[pentab[*src++]];
						#endif
						}
					}
					else
					{
						for (x = 0; x < width; ++x)
						{
						#ifdef RENDER_LITTLE_ENDIAN
						    	ULONG pix = pal->table[*src++];
							*dst++ = AROS_LONG2BE(pix);
						#else
							*dst++ = pal->table[*src++];
						#endif
						}
					}
					break;
			}

			if (linehook)
			{
				linemsg.RND_LMsg_type = LMSGTYPE_LINE_RENDERED;
				linemsg.RND_LMsg_row = y;
				if (!CallHookPkt(linehook, dst - width, &linemsg)) goto abort;
			}		

			if (proghook)
			{
				progmsg.RND_PMsg_count = y;
				if (!CallHookPkt(proghook, NULL, &progmsg)) goto abort;
			}

			src += tsw - width;
			dst += tdw - width;
		}

		result = CONV_SUCCESS;
	}

abort:		
	return result;
}

/************************************************************************** 
**
**	planar2chunky
*/

LIBAPI void Planar2ChunkyA(UWORD **planetab, UWORD bytewidth, UWORD rows, UWORD depth, 
	UWORD bytesperrow, UBYTE *dst, struct TagItem *tags)
{
	LONG y, w, x, offs;
	LONG destwidth = GetTagData(RND_DestWidth, bytewidth * 8, tags);
	UWORD mask;
	UBYTE p;
	
	if (!planetab || !bytewidth || !rows || !depth || !dst) return;
	

#ifdef	RENDER_LITTLE_ENDIAN
#warning "Endianess fixes in Planar2ChunkyA() not yet done!"
#endif
	
	for (y = 0; y < rows; ++y)
	{
		for (w = 0; w < bytewidth/2; ++w)
		{
			offs = y * (bytesperrow>>1) + w;
			mask = 0x8000;
			for (x = 0; x < 16; ++x)
			{
				p = 0;
				switch (depth)
				{
					default:
					case 8:          if (*(planetab[7] + offs) & mask) p |= 1;
					case 7: p <<= 1; if (*(planetab[6] + offs) & mask) p |= 1;
					case 6: p <<= 1; if (*(planetab[5] + offs) & mask) p |= 1;
					case 5: p <<= 1; if (*(planetab[4] + offs) & mask) p |= 1;
					case 4: p <<= 1; if (*(planetab[3] + offs) & mask) p |= 1;
					case 3: p <<= 1; if (*(planetab[2] + offs) & mask) p |= 1;
					case 2: p <<= 1; if (*(planetab[1] + offs) & mask) p |= 1;
					case 1: p <<= 1; if (*(planetab[0] + offs) & mask) p |= 1;
				}

				dst[w*16 + x] = p;
				mask >>= 1;
			}
		}
		dst += destwidth;
	}
}

/************************************************************************** 
**
**	chunky2bitmap
*/

LIBAPI void Chunky2BitMapA(UBYTE *src, UWORD sx, UWORD sy, UWORD width, UWORD height, 
	struct BitMap *bm, UWORD dx, UWORD dy, struct TagItem *tags)
{
	LONG tsw;
	UBYTE *pentab;
	LONG y, x;
	UWORD words[8];
	if (!src || !bm || !width || !height) return;
	tsw = GetTagData(RND_SourceWidth, width, tags);
	pentab = (UBYTE *) GetTagData(RND_PenTable, NULL, tags);
	
	src += sx + sy * tsw;
	
	for (y = dy; y < dy + height; ++y)
	{
		for (x = dx; x < dx + width; ++x)
		{
			UBYTE p = *src++;
			if (pentab) p = pentab[p];
			switch (bm->Depth)
			{
				default:
				case 8: words[7] <<= 1; words[7] |= (p & 128) >> 7;
				case 7: words[6] <<= 1; words[6] |= (p & 64) >> 6;
				case 6: words[5] <<= 1; words[5] |= (p & 32) >> 5;
				case 5: words[4] <<= 1; words[4] |= (p & 16) >> 4;
				case 4: words[3] <<= 1; words[3] |= (p & 8) >> 3;
				case 3: words[2] <<= 1; words[2] |= (p & 4) >> 2;
				case 2: words[1] <<= 1; words[1] |= (p & 2) >> 1;
				case 1: words[0] <<= 1; words[0] |= (p & 1);
			}
			
			if (((x + 1) & 15) == 0)
			{
				LONG offs = (y * bm->BytesPerRow + (x >> 3)) >> 1;
				
				if (x - dx < 15)	/* first word not at 16bit boundary, must be merged */
				{
					UWORD mask = 0xffff << (x - dx + 1);
				
					switch (bm->Depth)
					{
						default:
						case 8:	words[7] = (words[7] & ~mask) | (((UWORD *) bm->Planes[7])[offs] & mask);
						case 7:	words[6] = (words[6] & ~mask) | (((UWORD *) bm->Planes[6])[offs] & mask);
						case 6:	words[5] = (words[5] & ~mask) | (((UWORD *) bm->Planes[5])[offs] & mask);
						case 5:	words[4] = (words[4] & ~mask) | (((UWORD *) bm->Planes[4])[offs] & mask);
						case 4:	words[3] = (words[3] & ~mask) | (((UWORD *) bm->Planes[3])[offs] & mask);
						case 3:	words[2] = (words[2] & ~mask) | (((UWORD *) bm->Planes[2])[offs] & mask);
						case 2:	words[1] = (words[1] & ~mask) | (((UWORD *) bm->Planes[1])[offs] & mask);
						case 1:	words[0] = (words[0] & ~mask) | (((UWORD *) bm->Planes[0])[offs] & mask);
					}
				}
				
				switch (bm->Depth)
				{
					default:
					case 8:	((UWORD *) bm->Planes[7])[offs] = words[7];
					case 7:	((UWORD *) bm->Planes[6])[offs] = words[6];
					case 6:	((UWORD *) bm->Planes[5])[offs] = words[5];
					case 5:	((UWORD *) bm->Planes[4])[offs] = words[4];
					case 4:	((UWORD *) bm->Planes[3])[offs] = words[3];
					case 3:	((UWORD *) bm->Planes[2])[offs] = words[2];
					case 2:	((UWORD *) bm->Planes[1])[offs] = words[1];
					case 1:	((UWORD *) bm->Planes[0])[offs] = words[0];
				}
			}
		}
		
		if (x & 15)		/* remainder at the right, must be merged into planes */
		{
			LONG offs = (y * bm->BytesPerRow + (x >> 3)) >> 1;
			UWORD mask;
			mask = 0xffff >> x;
			x = 16 - (x & 15);

			switch (bm->Depth)
			{
				default:
				case 8:	((UWORD *) bm->Planes[7])[offs] = ((UWORD *) bm->Planes[7])[offs] & mask | (words[7] << x);
				case 7:	((UWORD *) bm->Planes[6])[offs] = ((UWORD *) bm->Planes[6])[offs] & mask | (words[6] << x);
				case 6:	((UWORD *) bm->Planes[5])[offs] = ((UWORD *) bm->Planes[5])[offs] & mask | (words[5] << x);
				case 5:	((UWORD *) bm->Planes[4])[offs] = ((UWORD *) bm->Planes[4])[offs] & mask | (words[4] << x);
				case 4:	((UWORD *) bm->Planes[3])[offs] = ((UWORD *) bm->Planes[3])[offs] & mask | (words[3] << x);
				case 3:	((UWORD *) bm->Planes[2])[offs] = ((UWORD *) bm->Planes[2])[offs] & mask | (words[2] << x);
				case 2:	((UWORD *) bm->Planes[1])[offs] = ((UWORD *) bm->Planes[1])[offs] & mask | (words[1] << x);
				case 1:	((UWORD *) bm->Planes[0])[offs] = ((UWORD *) bm->Planes[0])[offs] & mask | (words[0] << x);
			}
		}
		

		src += tsw - width;
	}
}

