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

#include "iconinfo.h"

char *version="$VER: DOpus_Icon 1.7 (7 Mar 93) Copyright © 1993 Jonathan Potter";

struct DOpusBase *DOpusBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *LayersBase;
struct Library *IconBase;

char protect_flags[6]={          /* The protection bits we understand */
	FIBF_SCRIPT,
	FIBF_ARCHIVE,
	FIBF_READ,
	FIBF_WRITE,
	FIBF_EXECUTE,
	FIBF_DELETE};

char
	*specific_gadtext[5][6],       /* Icon-specific gadget labels */
	*icon_type_names[8],           /* Names of different icon types */

	protect_keys[6]={                       /* Key equivalents for protection bits */
		0x21,                                 /* Script */
		0x20,                                 /* Archived */
		0x13,                                 /* Readable */
		0x11,                                 /* Writable */
		0x12,                                 /* Executable */
		0x22};                                /* Deletable */
