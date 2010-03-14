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

#include <datatypes/animationclass.h>
#include <datatypes/pictureclass.h>
#include <datatypes/soundclass.h>

/* Form types and other defines */

#define ID_CTBL MAKE_ID('C','T','B','L')
#define ID_DPAN MAKE_ID('D','P','A','N')
#define ID_DYCP MAKE_ID('D','Y','C','P')
#define ID_SHAM MAKE_ID('S','H','A','M')
#define ID_PAN  MAKE_ID('P','A','N',' ')

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

/* IFF ILBM structures */

#define BMHF_CMAPOK 1<<7

typedef struct C_Range
{
	WORD pad1;
	WORD rate;
	WORD active;
	UBYTE low,high;
} CRange;


//#include "pchg.h"

#endif
