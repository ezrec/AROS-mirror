/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* 
 *
 * ilbm.h:	Definitions for IFFParse ILBM reader.
 *
 * 5/92
 */

#ifndef IFFP_ILBM_H
#define IFFP_ILBM_H

#ifndef IFFP_IFF_H
#include "iff.h"
#endif

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif
#ifndef GRAPHICS_VIDEOCONTROL_H
#include <graphics/videocontrol.h>
#endif

#define	ID_ILBM		MAKE_ID('I','L','B','M')
#define	ID_BMHD		MAKE_ID('B','M','H','D')
#define	ID_CMAP		MAKE_ID('C','M','A','P')
#define	ID_CRNG		MAKE_ID('C','R','N','G')
#define	ID_CCRT		MAKE_ID('C','C','R','T')
#define	ID_GRAB		MAKE_ID('G','R','A','B')
#define	ID_SPRT		MAKE_ID('S','P','R','T')
#define	ID_DEST		MAKE_ID('D','E','S','T')
#define	ID_CAMG		MAKE_ID('C','A','M','G')

#define sizeofColorRegister  3

typedef WORD Color4;   /* Amiga RAM version of a color-register,
          * with 4 bits each RGB in low 12 bits.*/

#define MAXAMDEPTH 8

#define MAXSAVEDEPTH 24

#define	BytesPerRow(w)	((w) + 15 >> 4 << 1)
#define BitsPerRow(w)	((w) + 15 >> 4 << 4)

#define BADFLAGS  (SPRITES|VP_HIDE|GENLOCK_AUDIO|GENLOCK_VIDEO)
#define OLDCAMGMASK  (~BADFLAGS)


#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3

#define	cmpNone			0
#define	cmpByteRun1		1

#define RowBytes(w)	((((w) + 15) >> 4) << 1)

struct FMBitMapHeader {
	UWORD	w, h;		/* Width, height in pixels */
	WORD	x, y;		/* x, y position for this bitmap  */
	UBYTE	nPlanes;	/* # of planes (not including mask) */
	UBYTE	masking;	/* a masking technique listed above */
	UBYTE	compression;	/* cmpNone or cmpByteRun1 */
	UBYTE	Flags;		/* must be zero for now */
	UWORD	transparentColor;
	UBYTE	xAspect, yAspect;
	WORD	pageWidth, pageHeight;
};

#define BMHDB_CMAPOK    7
#define BMHDF_CMAPOK    (1 << BMHDB_CMAPOK)

typedef struct {
    UBYTE red, green, blue;   /* MUST be UBYTEs so ">> 4" won't sign extend.*/
    } ColorRegister;

typedef struct {
    WORD x, y;      /* coordinates (pixels) */
    } Point2D;

typedef struct {
    UBYTE depth;   /* # bitplanes in the original source */
    UBYTE pad1;      /* UNUSED; for consistency store 0 here */
    UWORD planePick;   /* how to scatter source bitplanes into destination */
    UWORD planeOnOff;   /* default bitplane data for planePick */
    UWORD planeMask;   /* selects which bitplanes to store into */
    } DestMerge;

typedef UWORD SpritePrecedence;
typedef struct {
   ULONG ViewModes;
   } CamgChunk;

#define RNG_NORATE  36   /* Dpaint uses this rate to mean non-active */
typedef struct {
    WORD  pad1;              /* reserved for future use; store 0 here */
    WORD  rate;      /* 60/sec=16384, 30/sec=8192, 1/sec=16384/60=273 */
    WORD  active;     /* bit0 set = active, bit 1 set = reverse */
    UBYTE low, high;   /* lower and upper color registers selected */
    } CRange;

typedef struct {
   WORD  direction;  /* 0=don't cycle, 1=forward, -1=backwards */
   UBYTE start;      /* range lower */
   UBYTE end;        /* range upper */
   LONG  seconds;    /* seconds between cycling */
   LONG  microseconds; /* msecs between cycling */
   WORD  pad;        /* future exp - store 0 here */
   } CcrtChunk;

#endif /* IFFP_ILBM_H */

struct ShowScreen {
	WORD system;			/* 00 */
	struct Screen *screen;		/* 02 */
	struct Window *window;		/* 06 */
	struct BitMap *bitmap;		/* 10 */
	UWORD *mouse;			/* 14 */
	struct Task *keytask;		/* 18 */
	struct GfxBase *gfxbase;	/* 22 */
	struct BitMap *tempbm;		/* 26 */
	struct RastPort temprp;
};

WORD openshowscreen(struct FMList*,WORD,WORD,WORD,ULONG,WORD,WORD,WORD,struct ShowScreen*);
void closeshowscreen(struct ShowScreen*);
WORD testbutton(struct Window*);
WORD waitbutton(struct Window*);
