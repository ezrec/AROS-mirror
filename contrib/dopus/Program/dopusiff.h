/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

/* IFF definitions for DOpus */

#ifndef DOPUS_IFF
#define DOPUS_IFF

/* Form types and other defines */

#define ID_8SVX MAKE_ID('8','S','V','X')
#define ID_ANHD MAKE_ID('A','N','H','D')
#define ID_ANIM MAKE_ID('A','N','I','M')
#define ID_BMHD MAKE_ID('B','M','H','D')
#define ID_BODY MAKE_ID('B','O','D','Y')
#define ID_CAMG MAKE_ID('C','A','M','G')
#define ID_CHAN MAKE_ID('C','H','A','N')
#define ID_CMAP MAKE_ID('C','M','A','P')
#define ID_CRNG MAKE_ID('C','R','N','G')
#define ID_CTBL MAKE_ID('C','T','B','L')
#define ID_DLTA MAKE_ID('D','L','T','A')
#define ID_DPAN MAKE_ID('D','P','A','N')
#define ID_DYCP MAKE_ID('D','Y','C','P')
#define ID_GRAB MAKE_ID('G','R','A','B')
#define ID_ILBM MAKE_ID('I','L','B','M')
#define ID_SHAM MAKE_ID('S','H','A','M')
#define ID_VHDR MAKE_ID('V','H','D','R')

#define CRNG_NORATE 36
#define CRNG_ACTIVE 1<<0
#define CRNG_REVERSE 1<<1
#define CY_CYCL 0
#define CY_WAIT 1

/* IFF Chunk structures */

typedef struct {
	ULONG ckID;
	ULONG ckSize;
} ChunkHeader;

typedef struct {
	ULONG ckID;
	ULONG ckSize;
	UBYTE ckData[1];
} Chunk;

/* IFF Animation structures */

typedef struct {
	UWORD version;
	UWORD nframes;
	unsigned char framespersecond;
	char pad;
	UWORD flags;
} DPAnimChunk;

typedef struct {
	UBYTE operation;
	UBYTE mask;
	UWORD w,h;
	WORD x,y;
	ULONG abstime;
	ULONG reltime;
	UBYTE interleave;
	UBYTE pad0;
	ULONG bits;
	UBYTE pad[16];
} AnimHdr;

/* IFF ILBM structures */

typedef struct BitHeader
{
	UWORD w,h;
	UWORD x,y;
	UBYTE nPlanes;
	UBYTE masking;
	UBYTE compression;
	UBYTE flags;
	UWORD transparentColor;
	UBYTE xAspect, yAspect;
	WORD pageWidth, pageHeight;
} BitMapHeader;

#define BMHF_CMAPOK 1<<7

typedef struct C_Range
{
	WORD pad1;
	WORD rate;
	WORD active;
	UBYTE low,high;
} CRange;


/* IFF 8SVX structures */

typedef struct VHeader {
	ULONG oneShotHiSamples,repeatHiSamples,samplesPerHiCycle;
	UWORD samplesPerSec;
	UBYTE ctOctave,sCompression;
	long volume;
} Voice8Header;

#include "pchg.h"

#endif
