
#include "lib_init.h"
#include "lib_debug.h"
#include <render/render.h>
#include <proto/utility.h>
#include <proto/exec.h>

/************************************************************************** 
**
**	createpalette
*/

LIBAPI RNDPAL *CreatePaletteA(struct TagItem *tags)
{
	APTR rmh;
	RNDPAL *p;
	UWORD type;
	
	type = (UWORD) GetTagData(RND_HSType, HSTYPE_15BIT, tags);

	type &= HSTYPE_MASK;
	switch (type)
	{
		case HSTYPE_12BIT:
		case HSTYPE_15BIT:
		case HSTYPE_18BIT:
			break;

		default:
			return NULL;
	}

	rmh = (APTR) GetTagData(RND_RMHandler, NULL, tags);
	p = AllocRenderVecClear(rmh, sizeof(RNDPAL));
	if (p)
	{
		InitSemaphore(&p->lock);
		p->type = type;
		p->rmh = rmh;
		p->numcolors = 0;
		p->p2mask = 0xff >> (8 - type);				/* ...11111 */
		p->p2mask2 = p->p2mask << (8 - type);		/* 11111... */
	}
	return p;
}

/************************************************************************** 
**
**	deletepalette
*/

LIBAPI void DeletePalette(RNDPAL *palette)
{
	FlushPalette(palette);
	FreeRenderVec((ULONG *) palette);
}

/************************************************************************** 
**
**	importpalette
*/

LIBAPI void ImportPaletteA(RNDPAL *palette, APTR coltab, UWORD numcol, struct TagItem *tags)
{
	BOOL newpal = GetTagData(RND_NewPalette, TRUE, tags);
	BOOL ehbpal = GetTagData(RND_EHBPalette, FALSE, tags);
	ULONG palfmt = GetTagData(RND_PaletteFormat, PALFMT_RGB8, tags);
	ULONG firstcol = GetTagData(RND_FirstColor, 0, tags);
	ULONG *pptr, *sptr;
	ULONG i, rgb;
	
	switch (palfmt)
	{
		case PALFMT_RGB32:
		case PALFMT_RGB8:
		case PALFMT_RGB4:
		case PALFMT_PALETTE:
			if (palette && numcol && coltab) break;
		default:
			return;
	}

	ObtainSemaphore(&palette->lock);
	pptr = palette->table;
	
	if (newpal)
	{
		palette->numcolors = 0;
		memfill32(pptr, 256*4, 0);
	}
	
	if (palfmt == PALFMT_PALETTE)
	{
		ObtainSemaphoreShared(&((RNDPAL *) coltab)->lock);
		sptr = ((RNDPAL *) coltab)->table;
	}
	else
	{
		sptr = coltab;
	}

	for (i = firstcol; i < firstcol + numcol; ++i)
	{
		switch (palfmt)
		{
			case PALFMT_RGB32:
				rgb = (*sptr++ & 0xff000000) >> 8;
				rgb |= (*sptr++ & 0xff000000) >> 16;
				rgb |= (*sptr++ & 0xff000000) >> 24;
				pptr[i] = rgb;
				break;
				
			case PALFMT_PALETTE:
			case PALFMT_RGB8:
				pptr[i] = *sptr++;
				break;
			
			case PALFMT_RGB4:
			{
				UWORD *sp = (UWORD *) sptr;
				rgb = (ULONG) *sp++;
				pptr[i] = ((rgb & 0xf00) << 12) |
							((rgb & 0xff0) << 8) |
							((rgb & 0x0ff) << 4) |
							(rgb & 0x00f);
				sptr = (ULONG *) sp;
				break;
			}
		}
		
		if (ehbpal && i >= 0 && i < 32)
		{
			pptr[i + 32] = (pptr[i] >> 1) & 0x7f7f7f;
		}
	}

	if (palfmt == PALFMT_PALETTE)
	{
		ReleaseSemaphore(&((RNDPAL *) coltab)->lock);
	}
	
	if (firstcol + numcol > palette->numcolors)
	{
		palette->numcolors = firstcol + numcol;
	}
	
	FlushPalette(palette);
	
	ReleaseSemaphore(&palette->lock);
}


/************************************************************************** 
**
**	exportpalette
*/

LIBAPI void ExportPaletteA(RNDPAL *palette, ULONG *coltab, struct TagItem *tags)
{
	ULONG palfmt = GetTagData(RND_PaletteFormat, PALFMT_RGB8, tags);
	ULONG firstcol = GetTagData(RND_FirstColor, 0, tags);
	ULONG numcol, i, rgb;

	switch (palfmt)
	{
		case PALFMT_RGB32:
		case PALFMT_RGB8:
		case PALFMT_RGB4:
			if (palette && coltab) break;
		default:
			return;
	}

	numcol = GetTagData(RND_NumColors, palette->numcolors, tags);
	if (!numcol) return;
	
	ObtainSemaphoreShared(&palette->lock);
	
	for (i = firstcol; i < firstcol + numcol; ++i)
	{
		rgb = palette->table[i];
		switch (palfmt)
		{
			case PALFMT_RGB32:
				*coltab++ = ((rgb & 0xff0000) << 8) | (rgb & 0xff0000) |
					((rgb & 0xff0000) >> 8) | ((rgb & 0xff0000) >> 16);
				*coltab++ = ((rgb & 0x00ff00) << 16) | ((rgb & 0x00ff00) << 8) |
					(rgb & 0x00ff00) | ((rgb & 0x00ff00) >> 8);
				*coltab++ = ((rgb & 0x0000ff) << 24) | ((rgb & 0x0000ff) << 16) |
					((rgb & 0x0000ff) << 8) | (rgb & 0x0000ff);
				break;
				
			case PALFMT_RGB8:
				*coltab++ = rgb;
				break;
				
			case PALFMT_RGB4:
			{
				UWORD *dp = (UWORD *) coltab;
				*dp++ = ((rgb & 0xf00000) >> 12) | ((rgb & 0xf000) >> 8) | ((rgb & 0xf0) >> 4);
				coltab = (ULONG *) dp;
			}
		}
	}

	ReleaseSemaphore(&palette->lock);
}


/************************************************************************** 
**
**	flushpalette
*/

LIBAPI void FlushPalette(RNDPAL *palette)
{
	if (palette)
	{
		ObtainSemaphore(&palette->lock);
		FreeRenderVec((ULONG *) palette->p2table);
		palette->p2table = NULL;
		ReleaseSemaphore(&palette->lock);
	}
}


/************************************************************************** 
**
**	calcpen
*/

static WORD calcpen(RNDPAL *p, ULONG rgb)
{
	LONG i, d;
	WORD besti;
	LONG bestd = 196000;
	WORD r, g, b;
	WORD dr, dg, db;
	
	r = (rgb & 0xff0000) >> 16;
	g = (rgb & 0x00ff00) >> 8;
	b = (rgb & 0x0000ff);

	for (i = 0; i < p->numcolors; ++i)
	{
		rgb = p->table[i];
		dr = r - ((rgb & 0xff0000) >> 16);
		dg = g - ((rgb & 0x00ff00) >> 8);
		db = b - (rgb & 0x0000ff);
		d = dr*dr + dg*dg + db*db;
		if (d < bestd)
		{
			besti = i;
			bestd = d;
			if (bestd == 0) break;
		}
	}
	return besti;
}

static WORD calcpen2(RNDPAL *p, WORD r, WORD g, WORD b)
{
	LONG i, d;
	WORD besti;
	LONG bestd = 196000;
	WORD dr, dg, db;
	ULONG rgb;
	
	for (i = 0; i < p->numcolors; ++i)
	{
		rgb = p->table[i];
		dr = r - ((rgb & 0xff0000) >> 16);
		dg = g - ((rgb & 0x00ff00) >> 8);
		db = b - (rgb & 0x0000ff);
		d = dr*dr + dg*dg + db*db;
		if (d < bestd)
		{
			besti = i;
			bestd = d;
			if (bestd == 0) break;
		}
	}
	return besti;
}


/************************************************************************** 
**
**	p2table
*/

LIBAPI WORD *GetP2Table(RNDPAL *p)
{
	if (!p->p2table)
	{
		ULONG size = (1L << (3 * p->type)) * sizeof(UWORD);
		p->p2table = AllocRenderVec(p->rmh, size);
		if (p->p2table)
		{
			memfill32((ULONG *) p->p2table, size, 0xffffffff);
		}
	}
	return p->p2table;
}

static __inline ULONG P2Index(RNDPAL *pal, ULONG rgb)
{
	ULONG foo;
	rgb >>= 8 - pal->type;
	foo = (rgb & pal->p2mask);
	foo <<= pal->type;
	rgb >>= 8;
	foo |= (rgb & pal->p2mask);
	foo <<= pal->type;
	rgb >>= 8;
	foo |= rgb;
	return foo;
}

LIBAPI __inline WORD P2Lookup(RNDPAL *pal, ULONG rgb)
{
	WORD *p2tab = pal->p2table + P2Index(pal, rgb);
	WORD result = *p2tab;
	if (result < 0)
	{
		result = calcpen(pal, rgb);
		*p2tab = result;
	}
	return result;
}


static __inline ULONG P2Index2(RNDPAL *pal, WORD r, WORD g, WORD b)
{
	ULONG foo;
	foo = b & pal->p2mask2;
	foo <<= pal->type;
	foo |= g & pal->p2mask2;
	foo <<= pal->type;
	foo |= r & pal->p2mask2;
	foo >>= 8 - pal->type;
	return foo;
}

LIBAPI __inline WORD P2Lookup2(RNDPAL *pal, WORD r, WORD g, WORD b)
{
	WORD *p2tab = pal->p2table + P2Index2(pal, r, g, b);
	WORD result = *p2tab;
	if (result < 0)
	{
		result = calcpen2(pal, r, g, b);
		*p2tab = result;
	}
	return result;
}


/************************************************************************** 
**
**	bestpen
*/

LIBAPI LONG BestPen(RNDPAL *p, ULONG rgb)
{
	LONG result = -1;
	if (p)
	{
		ObtainSemaphore(&p->lock);
		if (GetP2Table(p))
		{
			result = P2Lookup(p, rgb);
		}
		else
		{
			result = calcpen(p, rgb);
		}
		ReleaseSemaphore(&p->lock);
	}
	return result;
}


/************************************************************************** 
**
**	getpaletteattrs
*/

LIBAPI ULONG GetPaletteAttrs(RNDPAL *p, ULONG args)
{
	return p->numcolors;
}



/**************************************************************************
**
**	success = heapsort(data, refarray, length, cmpfunc)
**		LONG (*cmpfunc)(APTR data, APTR ref1, APTR ref2)
*/

typedef LONG (*CMPFUNC)(APTR userdata, APTR ref1, APTR ref2);

static BOOL heapsort(APTR data, APTR *refarray, ULONG length, CMPFUNC cmpfunc)
{
	ULONG indx, k, j, half, limit;
	APTR temp;
	
	if (refarray && cmpfunc && length > 1)
	{
		indx = (length >> 1) - 1;
		do
		{
			k = indx;
			temp = refarray[k];
			limit = length - 1;
			half = length >> 1;
			while (k < half)
			{
				j = k + k + 1;
				if ((j < limit) && ((*cmpfunc)(data, refarray[j + 1], refarray[j]) > 0))
				{
					++j;
				}
				if ((*cmpfunc)(data, temp, refarray[j]) >= 0)
				{
					break;
				}
				refarray[k] = refarray[j];
				k = j;
			}
			refarray[k] = temp;
		} while (indx-- != 0);
	
		while (--length > 0)
		{
			temp = refarray[0];
			refarray[0] = refarray[length];
			refarray[length] = temp;
			k = 0;
			temp = refarray[k];
			limit = length - 1;
			half = length >> 1;
			while (k < half)
			{
				j = k + k + 1;
				if ((j < limit) && ((*cmpfunc)(data, refarray[j + 1], refarray[j]) > 0))
				{
					++j;
				}
				if ((*cmpfunc)(data, temp, refarray[j]) >= 0)
				{
					break;
				}
				refarray[k] = refarray[j];
				k = j;
			}
			refarray[k] = temp;
		}
		return TRUE;
	}
	return FALSE;
}


/************************************************************************** 
**
**	sortpalette
*/

static LONG cmplum(ULONG rgb1, ULONG rgb2)
{
	ULONG Y1 = 	0.299 * ((rgb1 & 0xff0000) >> 16) +
				0.587 * ((rgb1 & 0x00ff00) >> 8) +
				0.114 * (rgb1 & 0x0000ff);
	ULONG Y2 = 	0.299 * ((rgb2 & 0xff0000) >> 16) +
				0.587 * ((rgb2 & 0x00ff00) >> 8) +
				0.114 * (rgb2 & 0x0000ff);

	if (Y1 > Y2) return 1;
	if (Y1 == Y2) return 0;
	return -1;
}

static LONG cmpfunc_lum_desc(APTR data, APTR rgb1, APTR rgb2)
{
	return -cmplum((ULONG) rgb1, (ULONG) rgb2);
}
static LONG cmpfunc_lum_asc(APTR data, APTR rgb1, APTR rgb2)
{
	return cmplum((ULONG) rgb1, (ULONG) rgb2);
}


static LONG cmpsat(ULONG rgb1, ULONG rgb2)
{
	ULONG S1, S2;
	LONG r = ((rgb1 & 0xff0000) >> 16);
	LONG g = ((rgb1 & 0x00ff00) >> 8);
	LONG b = (rgb1 & 0x0000ff);
	S1 = (r-g)*(r-g);
	S1 += (g-b)*(g-b);
	S1 += (b-r)*(b-r);
	r = ((rgb2 & 0xff0000) >> 16);
	g = ((rgb2 & 0x00ff00) >> 8);
	b = (rgb2 & 0x0000ff);
	S2 = (r-g)*(r-g);
	S2 += (g-b)*(g-b);
	S2 += (b-r)*(b-r);
	if (S1 > S2) return 1;
	if (S1 == S2) return 0;
	return -1;
}

static LONG cmpfunc_sat_desc(APTR data, APTR rgb1, APTR rgb2)
{
	return -cmpsat((ULONG) rgb1, (ULONG) rgb2);
}
static LONG cmpfunc_sat_asc(APTR data, APTR rgb1, APTR rgb2)
{
	return cmpsat((ULONG) rgb1, (ULONG) rgb2);
}



struct SortHistoEntry
{
	FLOAT measure;
	ULONG rgb;
};

static LONG cmpfunc_histo_desc(APTR data, struct SortHistoEntry *ref1, struct SortHistoEntry *ref2)
{
	if (ref1->measure > ref2->measure) return -1;
	if (ref1->measure == ref2->measure) return 0;
	return 1;
}

static LONG cmpfunc_histo_asc(APTR data, struct SortHistoEntry *ref1, struct SortHistoEntry *ref2)
{
	if (ref1->measure > ref2->measure) return 1;
	if (ref1->measure == ref2->measure) return 0;
	return -1;
}


/* 
**	adapthistogram(h, palette)
**	adapt histogram to palette, calculate entries in
**	sorthisto table
*/

static void adapthistogram(struct RNDTreeNode *node, RNDPAL *p, struct SortHistoEntry *table, ULONG sortmode)
{
	UBYTE pen;
	if (node->left)
	{
		adapthistogram(node->left, p, table, sortmode);
	}

	if (node->right)
	{
		adapthistogram(node->right, p, table, sortmode);
	}
	
	pen = P2Lookup(p, node->entry.rgb);
	switch (sortmode)
	{
		case PALMODE_SIGNIFICANCE:
		case PALMODE_POPULARITY:
			table[pen].measure += node->entry.count;
			break;
		case PALMODE_REPRESENTATION:
			table[pen].measure++;
			break;
	}
}


LIBAPI ULONG SortPaletteA(RNDPAL *p, ULONG mode, struct TagItem *tags)
{
	ULONG result = SORTP_NO_DATA;
	if (p)
	{
		APTR sortdata = p->table;
		struct SortHistoEntry *table = NULL;
		struct SortHistoEntry **reftable = NULL;
		LONG (*cmpfunc)(APTR, APTR, APTR) = NULL;
		RNDHISTO *h = (RNDHISTO *) GetTagData(RND_Histogram, NULL, tags);
		LONG i;

		ObtainSemaphore(&p->lock);
		
		switch (mode)
		{
			case PALMODE_BRIGHTNESS:
				cmpfunc = cmpfunc_lum_desc;
				break;
			case PALMODE_BRIGHTNESS | PALMODE_ASCENDING:
				cmpfunc = cmpfunc_lum_asc;
				break;
			case PALMODE_SATURATION:
				cmpfunc = cmpfunc_sat_desc;
				break;
			case PALMODE_SATURATION | PALMODE_ASCENDING:
				cmpfunc = cmpfunc_sat_asc;
				break;

			case PALMODE_SIGNIFICANCE | PALMODE_ASCENDING:
			case PALMODE_REPRESENTATION | PALMODE_ASCENDING:
			case PALMODE_POPULARITY | PALMODE_ASCENDING:
				cmpfunc = (CMPFUNC) cmpfunc_histo_asc;

			case PALMODE_SIGNIFICANCE:
			case PALMODE_REPRESENTATION:
			case PALMODE_POPULARITY:
				if (!cmpfunc) cmpfunc = (CMPFUNC) cmpfunc_histo_desc;
				if (!h) goto fail;
				result = SORTP_NOT_ENOUGH_MEMORY;
				if (!GetP2Table(p)) goto fail;
				table = AllocRenderVec(p->rmh, sizeof(struct SortHistoEntry) * p->numcolors);
				reftable = AllocRenderVec(p->rmh, sizeof(ULONG) * p->numcolors);
				if (!table || !reftable) goto fail;
				
				for (i = 0; i < p->numcolors; ++i)
				{
					table[i].measure = 0;
					table[i].rgb = p->table[i];
					reftable[i] = &table[i];
				}

				sortdata = reftable;

				ObtainSemaphore(&h->lock);
				adapthistogram(h->root, p, table, mode & 7);
				ReleaseSemaphore(&h->lock);

				if ((mode & 7) == PALMODE_SIGNIFICANCE)
				{
					LONG r, g, b, S;
					for (i = 0; i < p->numcolors; ++i)
					{
						r = (table[i].rgb & 0xff0000) >> 16;
						g = (table[i].rgb & 0x00ff00) >> 8;
						b = (table[i].rgb & 0x0000ff);
						S = (r-g)*(r-g);
						S += (g-b)*(g-b);
						S += (b-r)*(b-r);
						table[i].measure *= S;
					}
				}
				break;

			default:
				result = SORTP_NOT_IMPLEMENTED;
				goto fail;
		}
		
		heapsort(NULL, sortdata, p->numcolors, cmpfunc);

		if (table)
		{
			for (i = 0; i < p->numcolors; ++i)
			{
				p->table[i] = reftable[i]->rgb;
			}
		}

		FlushPalette(p);

		result = SORTP_SUCCESS;

fail:	if (table) FreeRenderVec((ULONG *) table);
		if (reftable) FreeRenderVec((ULONG *) reftable);

		ReleaseSemaphore(&p->lock);
	}
	
	return result;
}

