// Dither.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/iconobject.h>
#include <libraries/mcpgfx.h>
#include <scalos/scalosgfx.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <proto/datatypes.h>
#include <proto/intuition.h>
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>

#include "scalosgfx.h"

//-----------------------------------------------------------------------

#define HASH_SIZE 20023

#define MAXCOLORS 32767

/* #define LARGE_NORM */
#define LARGE_LUM

/* #define REP_CENTER_BOX */
/* #define REP_AVERAGE_COLORS */
#define REP_AVERAGE_PIXELS

//-----------------------------------------------------------------------

#define PPM_MAXMAXVAL PGM_MAXMAXVAL

#define PPM_GETR(p) ((p).Red)
#define PPM_GETG(p) ((p).Green)
#define PPM_GETB(p) ((p).Blue)

/************* added definitions *****************/

#define PPM_EQUAL(p,q) ( (p).Red == (q).Red \
	&& (p).Green == (q).Green \
	&& (p).Blue == (q).Blue )

/* Color scaling macro -- to make writing ppmtowhatever easier. */

#define PPM_DEPTH(newp, p, oldmaxval, newmaxval) \
	(newp).Red   = ( (int) (p).Red   * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval); \
	(newp).Green = ( (int) (p).Green * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval); \
	(newp).Blue  = ( (int) (p).Blue  * (newmaxval) + (oldmaxval) / 2 ) / (oldmaxval)


/* Luminance macro. */

#define PPM_LUMIN(p) ( 0.299 * (p).Red + 0.587 * (p).Green + 0.114 * (p).Blue )

struct box
	{
	int ind;
	int colors;
	int sum;
	};

/* Color histogram stuff. */

struct colorhist_item
	{
	struct ARGB color;
	int value;
	};

struct colorhist_list_item
	{
	struct colorhist_item ch;
	struct colorhist_list_item * next;
	};

struct longRGB
	{
	LONG Red;
	LONG Green;
	LONG Blue;
	};

//-----------------------------------------------------------------------

static struct colorhist_item *mediancut( struct colorhist_item * chv, int colors,
	int sum, ULONG maxval, int newcolors, struct ScalosGfxBase *ScalosGfxBase);
static int redcompare(const void *c1, const void *c2);
static int greencompare(const void *c1, const void *c2);
static int bluecompare(const void *c1, const void *c2);
static int sumcompare(const void *c1, const void *c2);
static long ppm_hashpixel(const struct ARGB *p);
static struct colorhist_item * ppm_computecolorhist(const struct ARGBHeader *argbh,
	int maxcolors, int *colorsP, struct ScalosGfxBase *ScalosGfxBase);
static struct colorhist_list_item **ppm_computecolorhash(const struct ARGBHeader *argbh,
	int maxcolors, int *colorsP, struct ScalosGfxBase *ScalosGfxBase);
static struct colorhist_list_item ** ppm_alloccolorhash(struct ScalosGfxBase *ScalosGfxBase);
static int ppm_addtocolorhash(struct colorhist_list_item **cht, const struct ARGB *colorP, int value, struct ScalosGfxBase *ScalosGfxBase);
static struct colorhist_item * ppm_colorhashtocolorhist(struct colorhist_list_item ** cht, int maxcolors, struct ScalosGfxBase *ScalosGfxBase);
static int ppm_lookupcolor(struct colorhist_list_item ** cht, const struct ARGB *colorP);
static void ppm_freecolorhist(struct colorhist_item * chv, struct ScalosGfxBase *ScalosGfxBase);
static void ppm_freecolorhash(struct colorhist_list_item ** cht, struct ScalosGfxBase *ScalosGfxBase);
static void *pm_allocrow(int cols, int size, struct ScalosGfxBase *ScalosGfxBase);
static void pm_freerow(void *itrow, struct ScalosGfxBase *ScalosGfxBase);

//-----------------------------------------------------------------------

/*
** Copyright (C) 1989, 1991 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/


struct ScalosBitMapAndColor *MedianCut(struct ARGBHeader *argbh,
	ULONG Depth, ULONG newcolors,
        BOOL floyd, struct BitMap *FriendBM,
        struct ScalosGfxBase *ScalosGfxBase)
{
	struct ScalosBitMapAndColor *sac = NULL;
	struct ARGBHeader mappixels;
	struct ARGB *pixels;
	UBYTE *AllocPixelLine = NULL;
	struct BitMap *TempBM = NULL;
	int row;
        int col, limitcol;
	ULONG maxval, newmaxval;
	int colors = 0;
        int ind;
	struct colorhist_item *chv = NULL;
	struct colorhist_item *colormap = NULL;
	struct colorhist_list_item **cht = NULL;
	BOOL usehash;
	struct longRGB *thiserr = NULL;
	struct longRGB *nexterr = NULL;
#define FS_SCALE 1024
	BOOL fs_direction = FALSE;
	BOOL Success = FALSE;

	d1(KPrintF(__FILE__ "/" "%s/%ld: START  w=%ld  h=%ld  floyd=%ld\n", \
		__LINE__, argbh->argb_Width, argbh->argb_Height, floyd));

	d1(KPrintF(__FILE__ "/" "%s/%ld: argb_ImageData:  A=%ld  R=%ld  G=%ld  B=%ld\n", \
		__LINE__, argbh->argb_ImageData->Alpha, \
		argbh->argb_ImageData->Red, argbh->argb_ImageData->Green, \
		argbh->argb_ImageData->Blue));

	do	{
		ULONG *pColorTable;
		struct RastPort rp;
		struct RastPort TempRp;
		struct longRGB s = { 0, 0, 0 };

		InitRastPort(&rp);
		InitRastPort(&TempRp);

		memset(&mappixels, 0, sizeof(mappixels));

		if (newcolors < 2)
			{
			d1(KPrintF(__FILE__ "/" "%s/%ld:number of colors must be > 2\n", __FUNC__, __LINE__));
			break;
			}
		if (newcolors > 256)
			{
			d1(KPrintF(__FILE__ "/" "%s/%ld:number of colors must be < 256\n", __FUNC__, __LINE__));
			break;
			}

		maxval = 255;

		sac = AllocSAC(argbh->argb_Width, argbh->argb_Height, Depth, FriendBM, NULL, ScalosGfxBase);
		if (NULL == sac)
			break;

		sac->sac_TransparentColor = SAC_TRANSPARENT_NONE;

		rp.BitMap = sac->sac_BitMap;
		d1(KPrintF(__FILE__ "/" "%s/%ld: rp.BitMap=%08lx\n", __FUNC__, __LINE__, rp.BitMap));

		// setup temp. RastPort for use by WritePixelLine8()
		TempRp.Layer = NULL;
		TempRp.BitMap = TempBM = AllocBitMap(TEMPRP_WIDTH(argbh->argb_Width), 1, 8, 0, NULL);
		if (NULL == TempBM)
			break;

		// allocate pens for 1 line (1 byte per pixel)
		AllocPixelLine = ScalosGfxAllocVecPooled(ScalosGfxBase, ((argbh->argb_Width + 15) >> 4) << 4);
		if (NULL == AllocPixelLine)
			break;

		/*
		** Step 2: attempt to make a histogram of the colors, unclustered.
		** If at first we don't succeed, lower maxval to increase color
		** coherence and try again.  This will eventually terminate, with
		** maxval at worst 15, since 32^3 is approximately MAXCOLORS.
		*/
		for ( ; ; )
		        {
			d1(KPrintF(__FILE__ "/" "%s/%ld: making histogram...\n", __FUNC__, __LINE__));
			chv = ppm_computecolorhist(argbh, MAXCOLORS, &colors, ScalosGfxBase);
			if ( chv != (struct colorhist_item *) NULL )
				break;

			d1(KPrintF(__FILE__ "/" "%s/%ld: too many colors!\n", __FUNC__, __LINE__));

		        newmaxval = maxval / 2;
			d1(KPrintF(__FILE__ "/" "%s/%ld: scaling colors from maxval=%d to maxval=%d to improve clustering...\n",
				__LINE__, maxval, newmaxval));
			for ( row = 0, pixels = argbh->argb_ImageData;
					row < argbh->argb_Height;
					++row, pixels += argbh->argb_Width)
				{
				struct ARGB *pP = pixels;
				for ( col = 0; col < argbh->argb_Width; ++col, ++pP )
					{
					PPM_DEPTH(*pP, *pP, maxval, newmaxval);
					}
				}
		        maxval = newmaxval;
			}
		d1(KPrintF(__FILE__ "/" "%s/%ld: %d colors found\n", __FUNC__, __LINE__, colors));

		/*
		** Step 3: apply median-cut to histogram, making the new colormap.
		*/
		d1(KPrintF(__FILE__ "/" "%s/%ld: choosing %d colors...\n", __FUNC__, __LINE__, newcolors));
		colormap = mediancut( chv, colors, argbh->argb_Height * argbh->argb_Width, maxval, newcolors, ScalosGfxBase );
		if (NULL == colormap)
			break;

		d1(KPrintF(__FILE__ "/" "%s/%ld: colormap: %08lx %08lx %08lx %08lx\n", \
			__LINE__, *((ULONG *) &colormap[0].color), \
			*((ULONG *) &colormap[1].color), \
			*((ULONG *) &colormap[2].color), \
			*((ULONG *) &colormap[3].color)));

	        /*
	        ** Step 4: map the colors in the image to their closest match in the
	        ** new colormap, and write 'em out.
	        */
		d1(KPrintF(__FILE__ "/" "%s/%ld: mapping image to new colors...\n", __FUNC__, __LINE__));
		cht = ppm_alloccolorhash(ScalosGfxBase);
		d1(KPrintF(__FILE__ "/" "%s/%ld: cht=%08lx\n", __FUNC__, __LINE__, cht));
		if (NULL == cht)
			break;

		usehash = TRUE;
//+++		ppm_writeppminit( stdout, argbh->argb_Width, argbh->argb_Height, maxval, 0 );
	        if ( floyd )
			{
			struct DateStamp ds;

			/* Initialize Floyd-Steinberg error vectors. */
			thiserr = (struct longRGB *) pm_allocrow( argbh->argb_Width + 2, sizeof(struct longRGB), ScalosGfxBase);
			if (NULL == thiserr)
				break;
			nexterr = (struct longRGB *) pm_allocrow( argbh->argb_Width + 2, sizeof(struct longRGB), ScalosGfxBase );
			if (NULL == nexterr)
				break;

			DateStamp(&ds);
			srand(ds.ds_Tick);

			d1(KPrintF(__FILE__ "/" "%s/%ld: thiserr=%08lx  nexterr=%08lx\n", \
				__LINE__, thiserr, nexterr));

			for ( col = 0; col < argbh->argb_Width + 2; ++col )
			        {
				thiserr[col].Red   = rand() % ( FS_SCALE * 2 ) - FS_SCALE;
				thiserr[col].Green = rand() % ( FS_SCALE * 2 ) - FS_SCALE;
				thiserr[col].Blue  = rand() % ( FS_SCALE * 2 ) - FS_SCALE;
			        /* (random errors in [-1 .. 1]) */
				d1(KPrintF(__FILE__ "/" "%s/%ld: F/S error[%ld]: R=%08lx G=%08lx B=%08lx\n", \
					__LINE__, col, thiserr[col].Red, thiserr[col].Green, thiserr[col].Blue));
			        }

			fs_direction = TRUE;
			}

		d1(KPrintF(__FILE__ "/" "%s/%ld: \n", __FUNC__, __LINE__));

		for ( row = 0, pixels = argbh->argb_ImageData;
			row < argbh->argb_Height;
			++row, pixels += argbh->argb_Width)
			{
			UBYTE *pPixelLine;
			struct ARGB *pP;

			d1(KPrintF(__FILE__ "/" "%s/%ld: row=%ld  pixels=%08lx\n", __FUNC__, __LINE__, row, pixels));

			if ( floyd )
				{
				for ( col = 0; col < argbh->argb_Width + 2; ++col )
					nexterr[col].Red = nexterr[col].Green = nexterr[col].Blue = 0;
				}
			if ( ( ! floyd ) || fs_direction )
			        {
			        col = 0;
				limitcol = argbh->argb_Width;
				pP = pixels;
				pPixelLine = AllocPixelLine;
			        }
			else
			        {
				col = argbh->argb_Width - 1;
			        limitcol = -1;
				pP = pixels + col;
				pPixelLine = AllocPixelLine + col;
			        }

			d1(KPrintF(__FILE__ "/" "%s/%ld: pP=%08lx\n", __FUNC__, __LINE__, pP));

			do      {
				if ( floyd )
					{
					/* Use Floyd-Steinberg errors to adjust actual color. */
					d1(KPrintF(__FILE__ "/" "%s/%ld:   thiserr[%ld]: R=%4ld G=%4ld B=%4ld\n", \
						__LINE__, col + 1, thiserr[col + 1].Red, thiserr[col + 1].Green, thiserr[col + 1].Blue));

					s.Red   = pP->Red   + thiserr[col + 1].Red   / FS_SCALE;
					s.Green = pP->Green + thiserr[col + 1].Green / FS_SCALE;
					s.Blue  = pP->Blue  + thiserr[col + 1].Blue  / FS_SCALE;

					if ( s.Red < 0 )
						s.Red = 0;
					else if ( s.Red > maxval )
						s.Red = maxval;
					if ( s.Green < 0 )
						s.Green = 0;
					else if ( s.Green > maxval )
						s.Green = maxval;
					if ( s.Blue < 0 )
						s.Blue = 0;
					else if ( s.Blue > maxval )
						s.Blue = maxval;

					d1(KPrintF(__FILE__ "/" "%s/%ld:   Red=%3d  s.Red=%ld\n", __FUNC__, __LINE__, pP->Red, s.Red));
					d1(KPrintF(__FILE__ "/" "%s/%ld: Green=%3d  s.Green=%ld\n", __FUNC__, __LINE__, pP->Green, s.Green));
					d1(KPrintF(__FILE__ "/" "%s/%ld:  Blue=%3d  s.Blue=%ld\n", __FUNC__, __LINE__, pP->Blue, s.Blue));

					pP->Red = s.Red;
					pP->Green = s.Green;
					pP->Blue = s.Blue;
					}

			        /* Check hash table to see if we have already matched this color. */
			        ind = ppm_lookupcolor( cht, pP );
				d1(KPrintF(__FILE__ "/" "%s/%ld: ind=%ld\n", __FUNC__, __LINE__, ind));
			        if ( ind == -1 )
					{
					/* No; search colormap for closest match. */
					int i, r1, g1, b1;
					long dist;

					r1 = pP->Red;
					g1 = pP->Green;
					b1 = pP->Blue;
					dist = LONG_MAX;
					for ( i = 0; i < newcolors; ++i )
					        {
						int r2, g2, b2;
						long newdist;

						r2 = colormap[i].color.Red;
						g2 = colormap[i].color.Green;
						b2 = colormap[i].color.Blue;

					        newdist = ( r1 - r2 ) * ( r1 - r2 ) +
						          ( g1 - g2 ) * ( g1 - g2 ) +
						          ( b1 - b2 ) * ( b1 - b2 );
					        if ( newdist < dist )
							{
							ind = i;
							dist = newdist;
							}
					        }
					if ( usehash )
					        {
						if ( ppm_addtocolorhash( cht, pP, ind, ScalosGfxBase) < 0 )
							{
							d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory adding to hash table, proceeding without it\n", __FUNC__, __LINE__));
							usehash = FALSE;
							}
					        }
					}

				if ( floyd )
					{
					/* Propagate Floyd-Steinberg error terms. */
					long err;

					if ( fs_direction )
					        {
						err = (s.Red - (long) colormap[ind].color.Red) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Red   err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col + 2].Red += ( err * 7 ) / 16;
						nexterr[col    ].Red += ( err * 3 ) / 16;
						nexterr[col + 1].Red += ( err * 5 ) / 16;
						nexterr[col + 2].Red += ( err     ) / 16;
						err = (s.Green - (long) colormap[ind].color.Green) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Green err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col + 2].Green += ( err * 7 ) / 16;
						nexterr[col    ].Green += ( err * 3 ) / 16;
						nexterr[col + 1].Green += ( err * 5 ) / 16;
						nexterr[col + 2].Green += ( err     ) / 16;
						err = (s.Blue - (long) colormap[ind].color.Blue) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Blue  err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col + 2].Blue += ( err * 7 ) / 16;
						nexterr[col    ].Blue += ( err * 3 ) / 16;
						nexterr[col + 1].Blue += ( err * 5 ) / 16;
						nexterr[col + 2].Blue += ( err     ) / 16;
					        }
					else
					        {
						err = (s.Red - (long) colormap[ind].color.Red) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Red   err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col    ].Red += ( err * 7 ) / 16;
						nexterr[col + 2].Red += ( err * 3 ) / 16;
						nexterr[col + 1].Red += ( err * 5 ) / 16;
						nexterr[col    ].Red += ( err     ) / 16;
						err = (s.Green - (long) colormap[ind].color.Green) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Green err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col    ].Green += ( err * 7 ) / 16;
						nexterr[col + 2].Green += ( err * 3 ) / 16;
						nexterr[col + 1].Green += ( err * 5 ) / 16;
						nexterr[col    ].Green += ( err     ) / 16;
						err = (s.Blue - (long) colormap[ind].color.Blue) * FS_SCALE;
						d1(KPrintF(__FILE__ "/" "%s/%ld: Blue  err=%ld\n", __FUNC__, __LINE__, err));
						thiserr[col    ].Blue += ( err * 7 ) / 16;
						nexterr[col + 2].Blue += ( err * 3 ) / 16;
						nexterr[col + 1].Blue += ( err * 5 ) / 16;
						nexterr[col    ].Blue += ( err     ) / 16;
					        }
					}

				pP->Red   = colormap[ind].color.Red;
				pP->Green = colormap[ind].color.Green;
				pP->Blue  = colormap[ind].color.Blue;

				// Check transparency, and set color index to 0 for transparent pixels
				if (pP->Alpha >= 128)
					*pPixelLine = 1 + ind;
				else
					{
					// mark this pixel as transparent
					*pPixelLine = sac->sac_TransparentColor = 0;
					}

				d1(KPrintF(__FILE__ "/" "%s/%ld: row=%ld  col=%ld  ind=%ld  color=%08lx\n", \
					__LINE__, row, col, ind, *((ULONG *) pP)));

				if ( ( ! floyd ) || fs_direction )
					{
					++col;
					++pP;
					++pPixelLine;
					}
				else
					{
					--col;
					--pP;
					--pPixelLine;
					}
				} while ( col != limitcol );

			if ( floyd )
			        {
				struct longRGB *temperr;

				temperr = thiserr;
				thiserr = nexterr;
				nexterr = temperr;
				fs_direction = !fs_direction;
			        }

			d1(KPrintF(__FILE__ "/" "%s/%ld: row=%ld\n", __FUNC__, __LINE__, row));

			WritePixelLine8(&rp, 0, row,
				argbh->argb_Width, AllocPixelLine, &TempRp);
			}

		d1(KPrintF(__FILE__ "/" "%s/%ld: \n", __FUNC__, __LINE__));

		// fill sac_ColorTable
		// leave first entry in sac_ColorTable free since
		// it is used as transparency indicator
		for (ind = 0, pColorTable = sac->sac_ColorTable + 3;
			ind < sac->sac_NumColors - 1; ind++)
			{
			*pColorTable++ = colormap[ind].color.Red << 24;
			*pColorTable++ = colormap[ind].color.Green << 24;
			*pColorTable++ = colormap[ind].color.Blue << 24;
			}
		Success = TRUE;
		} while (0);

	d1(KPrintF(__FILE__ "/" "%s/%ld: \n", __FUNC__, __LINE__));

	if (!Success)
		FreeSAC(sac, ScalosGfxBase);
	if (colormap)
		ScalosGfxFreeVecPooled(ScalosGfxBase, colormap);
	if (cht)
		ppm_freecolorhash(cht, ScalosGfxBase);
	if (chv)
		ppm_freecolorhist( chv, ScalosGfxBase);
	if (thiserr)
		pm_freerow(thiserr, ScalosGfxBase);
	if (nexterr)
		pm_freerow(nexterr, ScalosGfxBase);
	if (AllocPixelLine)
		ScalosGfxFreeVecPooled(ScalosGfxBase, AllocPixelLine);
	if (TempBM)
		FreeBitMap(TempBM);

	d1(KPrintF(__FILE__ "/" "%s/%ld: END Success=%ld\n", __FUNC__, __LINE__, Success));

	return sac;
}


/*
** Here is the fun part, the median-cut colormap generator.  This is based
** on Paul Heckbert's paper "Color Image Quantization for Frame Buffer
** Display", SIGGRAPH '82 Proceedings, page 297.
*/
static struct colorhist_item *mediancut( struct colorhist_item * chv,
	int colors, int sum, ULONG maxval, int newcolors,
        struct ScalosGfxBase *ScalosGfxBase )
{
        struct colorhist_item * colormap;
        struct box * bv;
        int bi, i;
        int boxes;

	bv = (struct box *) ScalosGfxAllocVecPooled(ScalosGfxBase, sizeof(struct box) * newcolors );
	colormap = (struct colorhist_item *) ScalosGfxAllocVecPooled(ScalosGfxBase, sizeof(struct colorhist_item) * newcolors );

	if (bv == NULL || colormap == NULL)
		{
		d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory\n", __FUNC__, __LINE__));
		return NULL;
		}

	for ( i = 0; i < newcolors; ++i )
		{
		colormap[i].color.Alpha = 0xff;
		colormap[i].color.Red = 0;
		colormap[i].color.Green = 0;
		colormap[i].color.Blue = 0;
		}

        /*
        ** Set up the initial box.
        */
        bv[0].ind = 0;
        bv[0].colors = colors;
        bv[0].sum = sum;
        boxes = 1;

        /*
        ** Main loop: split boxes until we have enough.
        */
        while ( boxes < newcolors )
		{
		int indx, clrs;
		int sm;
		int minr, maxr, ming, maxg, minb, maxb, v;
		int halfsum, lowersum;

		/*
		** Find the first splittable box.
		*/
		for ( bi = 0; bi < boxes; ++bi )
			{
			if ( bv[bi].colors >= 2 )
				break;
			}
		if ( bi == boxes )
			break;	    /* ran out of colors! */

		indx = bv[bi].ind;
		clrs = bv[bi].colors;
		sm = bv[bi].sum;

		/*
		** Go through the box finding the minimum and maximum of each
		** component - the boundaries of the box.
		*/
		minr = maxr = chv[indx].color.Red;
		ming = maxg = chv[indx].color.Green;
		minb = maxb = chv[indx].color.Blue;

		for ( i = 1; i < clrs; ++i )
		        {
			v = chv[indx + i].color.Red;
			if ( v < minr )
				minr = v;
			if ( v > maxr )
				maxr = v;
			v = chv[indx + i].color.Green;
			if ( v < ming )
				ming = v;
			if ( v > maxg )
				maxg = v;
			v = chv[indx + i].color.Blue;
			if ( v < minb )
				minb = v;
			if ( v > maxb )
				maxb = v;
		        }

		/*
		** Find the largest dimension, and sort by that component.  I have
		** included two methods for determining the "largest" dimension;
		** first by simply comparing the range in RGB space, and second
		** by transforming into luminosities before the comparison.  You
		** can switch which method is used by switching the commenting on
		** the LARGE_ defines at the beginning of this source file.
		*/
#ifdef LARGE_NORM
		if ( maxr - minr >= maxg - ming && maxr - minr >= maxb - minb )
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), redcompare );
		else if ( maxg - ming >= maxb - minb )
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), greencompare );
		else
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), bluecompare );
#endif /*LARGE_NORM*/

#ifdef LARGE_LUM
		{
		struct ARGB p;
		float rl, gl, bl;

		p.Red = maxr - minr;
		p.Green = 0;
		p.Blue = 0;

		rl = PPM_LUMIN(p);

		p.Red = 0;
		p.Green = maxg - ming;
		p.Blue = 0;
		gl = PPM_LUMIN(p);

		p.Red = 0;
		p.Green = 0;
		p.Blue = maxb - minb;
		bl = PPM_LUMIN(p);

		if ( rl >= gl && rl >= bl )
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), redcompare );
		else if ( gl >= bl )
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), greencompare );
		else
			qsort((char*) &(chv[indx]), clrs, sizeof(struct colorhist_item), bluecompare );
		}
#endif /*LARGE_LUM*/

		/*
		** Now find the median based on the counts, so that about half the
		** pixels (not colors, pixels) are in each subdivision.
		*/
		lowersum = chv[indx].value;
		halfsum = sm / 2;
		for ( i = 1; i < clrs - 1; ++i )
		        {
		        if ( lowersum >= halfsum )
				break;
		        lowersum += chv[indx + i].value;
		        }

		/*
		** Split the box, and sort to bring the biggest boxes to the top.
		*/
		bv[bi].colors = i;
		bv[bi].sum = lowersum;
		bv[boxes].ind = indx + i;
		bv[boxes].colors = clrs - i;
		bv[boxes].sum = sm - lowersum;
		++boxes;

		qsort( (char*) bv, boxes, sizeof(struct box), sumcompare );
		}

	/*
	** Ok, we've got enough boxes.  Now choose a representative color for
	** each box.  There are a number of possible ways to make this choice.
	** One would be to choose the center of the box; this ignores any structure
	** within the boxes.  Another method would be to average all the colors in
	** the box - this is the method specified in Heckbert's paper.  A third
	** method is to average all the pixels in the box.  You can switch which
	** method is used by switching the commenting on the REP_ defines at
	** the beginning of this source file.
	*/
	for ( bi = 0; bi < boxes; ++bi )
		{
#ifdef REP_CENTER_BOX
		int indx = bv[bi].ind;
		int clrs = bv[bi].colors;
		int minr, maxr, ming, maxg, minb, maxb, v;

		minr = maxr = chv[indx].color.Red;
		ming = maxg = chv[indx].color.Green;
		minb = maxb = chv[indx].color.Blue;
		for ( i = 1; i < clrs; ++i )
		        {
			v = chv[indx + i].color.Red;
		        minr = min( minr, v );
		        maxr = max( maxr, v );
			v = chv[indx + i].color.Green;
		        ming = min( ming, v );
		        maxg = max( maxg, v );
			v = chv[indx + i].color.Blue;
		        minb = min( minb, v );
		        maxb = max( maxb, v );
		        }

		colormap[bi].color.Red   = (minr + maxr) / 2;
		colormap[bi].color.Green = (ming + maxg) / 2;
		colormap[bi].color.Blue  = (minb + maxb) / 2;

#endif /*REP_CENTER_BOX*/
#ifdef REP_AVERAGE_COLORS
		int indx = bv[bi].ind;
		int clrs = bv[bi].colors;
		long r = 0, g = 0, b = 0;

		for ( i = 0; i < clrs; ++i )
		        {
			r += chv[indx + i].color.Red;
			g += chv[indx + i].color.Green;
			b += chv[indx + i].color.Blue;
		        }
		colormap[bi].color.Red   = r / clrs;
		colormap[bi].color.Green = g / clrs;
		colormap[bi].color.Blue  = b / clrs;
#endif /*REP_AVERAGE_COLORS*/

#ifdef REP_AVERAGE_PIXELS
		int indx = bv[bi].ind;
		int clrs = bv[bi].colors;
		long r = 0, g = 0, b = 0, sum = 0;

		for ( i = 0; i < clrs; ++i )
		        {
			r += chv[indx + i].color.Red   * chv[indx + i].value;
			g += chv[indx + i].color.Green * chv[indx + i].value;
			b += chv[indx + i].color.Blue  * chv[indx + i].value;
		        sum += chv[indx + i].value;
		        }
		r = r / sum;
		if ( r > maxval )
			r = maxval;   /* avoid math errors */
		g = g / sum;
		if ( g > maxval )
			g = maxval;
		b = b / sum;
		if ( b > maxval )
			b = maxval;

		colormap[bi].color.Red = r;
		colormap[bi].color.Green = g;
		colormap[bi].color.Blue = b;
#endif /*REP_AVERAGE_PIXELS*/
		}

        /*
        ** All done.
        */

	ScalosGfxFreeVecPooled(ScalosGfxBase, (char*) bv);     //+++

        return colormap;
}

static int redcompare(const void *c1, const void *c2)
{
	const struct colorhist_item * ch1 = c1;
	const struct colorhist_item * ch2 = c2;

	return (int) (ch1->color.Red - ch2->color.Red);
}

static int greencompare(const void *c1, const void *c2)
{
	const struct colorhist_item * ch1 = c1;
	const struct colorhist_item * ch2 = c2;

	return (int) (ch1->color.Green - ch2->color.Green);
}

static int bluecompare(const void *c1, const void *c2)
{
	const struct colorhist_item * ch1 = c1;
	const struct colorhist_item * ch2 = c2;

	return (int) (ch1->color.Blue - ch2->color.Blue);
}

static int sumcompare(const void *c1, const void *c2)
{
	const struct box *b1 = c1;
	const struct box *b2 = c2;

	return b2->sum - b1->sum;
}


static long ppm_hashpixel(const struct ARGB *p)
{
	return (long) ((p->Red * 33023 + p->Green * 30013 + p->Blue * 27011) & 0x7fffffff ) % HASH_SIZE;
}


static struct colorhist_item * ppm_computecolorhist(const struct ARGBHeader *argbh,
	int maxcolors, int *colorsP, struct ScalosGfxBase *ScalosGfxBase)
{
        struct colorhist_list_item ** cht;
        struct colorhist_item * chv;

	cht = ppm_computecolorhash(argbh, maxcolors, colorsP, ScalosGfxBase);
	if ( cht == (struct colorhist_list_item **) 0 )
		return (struct colorhist_item *) 0;

	chv = ppm_colorhashtocolorhist( cht, maxcolors, ScalosGfxBase);
	ppm_freecolorhash( cht, ScalosGfxBase );

	return chv;
}


static struct colorhist_list_item ** ppm_computecolorhash(const struct ARGBHeader *argbh,
	int maxcolors, int *colorsP, struct ScalosGfxBase *ScalosGfxBase)
{
        struct colorhist_list_item ** cht;
        struct colorhist_list_item * chl;
	int row;
	const struct ARGB *argb = argbh->argb_ImageData;

	cht = ppm_alloccolorhash(ScalosGfxBase);
        *colorsP = 0;

        /* Go through the entire image, building a hash table of colors. */
	for (row = 0; row < argbh->argb_Height; ++row)
		{
		int col;
		const struct ARGB *pP = argb;

		argb += argbh->argb_Width;

		for (col = 0; col < argbh->argb_Width; ++col, ++pP)
		        {
			int hash;

			hash = ppm_hashpixel(pP);

			for ( chl = cht[hash]; chl != NULL; chl = chl->next )
				{
				if ( PPM_EQUAL( chl->ch.color, *pP ) )
					break;
				}

			if ( chl != NULL)
				++(chl->ch.value);
		        else
				{
				if ( ++(*colorsP) > maxcolors )
					{
					ppm_freecolorhash( cht, ScalosGfxBase );
					return NULL;
					}
				chl = (struct colorhist_list_item *) ScalosGfxAllocVecPooled(ScalosGfxBase, sizeof(struct colorhist_list_item) );
				if ( chl == 0 )
					{
					d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory computing hash table\n", __FUNC__, __LINE__));
					return NULL;
					}
				chl->ch.color = *pP;
				chl->ch.value = 1;
				chl->next = cht[hash];
				cht[hash] = chl;
				}
		        }
		}

	return cht;
}


static struct colorhist_list_item ** ppm_alloccolorhash(struct ScalosGfxBase *ScalosGfxBase)
{
	struct colorhist_list_item ** cht;
	int i;

	cht = (struct colorhist_list_item **) ScalosGfxAllocVecPooled(ScalosGfxBase, HASH_SIZE * sizeof(struct colorhist_list_item *) );
	if (NULL == cht)
		{
		d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory allocating hash table\n", __FUNC__, __LINE__));
		return NULL;
		}

	for ( i = 0; i < HASH_SIZE; ++i )
		cht[i] = NULL;

	return cht;
}


static int ppm_addtocolorhash(struct colorhist_list_item **cht, const struct ARGB *colorP, int value, struct ScalosGfxBase *ScalosGfxBase)
{
	int hash;
	struct colorhist_list_item * chl;

	chl = (struct colorhist_list_item *) ScalosGfxAllocVecPooled(ScalosGfxBase, sizeof(struct colorhist_list_item) );
	if (NULL == chl)
	    return -1;
	hash = ppm_hashpixel(colorP);
	chl->ch.color = *colorP;
	chl->ch.value = value;
	chl->next = cht[hash];
	cht[hash] = chl;

	return 0;
}


static struct colorhist_item * ppm_colorhashtocolorhist(struct colorhist_list_item ** cht, int maxcolors, struct ScalosGfxBase *ScalosGfxBase)
{
	struct colorhist_item * chv;
	struct colorhist_list_item * chl;
	int i, j;

	/* Now collate the hash table into a simple colorhist array. */
	chv = (struct colorhist_item *) ScalosGfxAllocVecPooled(ScalosGfxBase, maxcolors * sizeof(struct colorhist_item) );
	/* (Leave room for expansion by caller.) */
	if (NULL ==  chv)
		{
		d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory generating histogram\n", __FUNC__, __LINE__));
		return NULL;
		}

	/* Loop through the hash table. */
	j = 0;
	for ( i = 0; i < HASH_SIZE; ++i )
		{
		for ( chl = cht[i]; chl != (struct colorhist_list_item *) 0; chl = chl->next )
			{
			/* Add the new entry. */
			chv[j] = chl->ch;
			++j;
			}
		}

	/* All done. */
	return chv;
}


static int ppm_lookupcolor(struct colorhist_list_item ** cht, const struct ARGB *colorP)
{
        int hash;
        struct colorhist_list_item * chl;

	hash = ppm_hashpixel(colorP);

        for ( chl = cht[hash]; chl != (struct colorhist_list_item *) 0; chl = chl->next )
		{
		if ( PPM_EQUAL( chl->ch.color, *colorP ) )
			return chl->ch.value;
		}

        return -1;
}


static void ppm_freecolorhist(struct colorhist_item *chv, struct ScalosGfxBase *ScalosGfxBase)
{
	ScalosGfxFreeVecPooled(ScalosGfxBase, (char*) chv );
}


static void ppm_freecolorhash(struct colorhist_list_item **cht, struct ScalosGfxBase *ScalosGfxBase)
{
        int i;
	struct colorhist_list_item *chl, *chlnext;

        for ( i = 0; i < HASH_SIZE; ++i )
		{
		for ( chl = cht[i]; chl != NULL; chl = chlnext )
		        {
		        chlnext = chl->next;
			ScalosGfxFreeVecPooled(ScalosGfxBase, (char*) chl );
		        }
		}
	ScalosGfxFreeVecPooled(ScalosGfxBase, (char*) cht );
}


static void *pm_allocrow(int cols, int size, struct ScalosGfxBase *ScalosGfxBase)
{
	void *itrow;

	itrow = ScalosGfxAllocVecPooled(ScalosGfxBase, cols * size );
	if (NULL ==  itrow)
		d1(KPrintF(__FILE__ "/" "%s/%ld: out of memory allocating a row\n", __FUNC__, __LINE__));

	return itrow;
}


static void pm_freerow(void *itrow, struct ScalosGfxBase *ScalosGfxBase)
{
	ScalosGfxFreeVecPooled(ScalosGfxBase, itrow );
}

