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

#ifndef DOPUS_CHIPIMAGE
#define DOPUS_CHIPIMAGE

#include "ushort.h"

#ifdef __SASC_60

extern /* __chip */ USHORT
	pageflip_data1[5],
	pageflip_data2[3],
	null_pointer[6];

extern /* __chip */ char beepwave[16];

#else

extern USHORT
	/* __chip */ pageflip_data1[5],
	/* __chip */ pageflip_data2[3],
	/* __chip */ null_pointer[6];

extern char /* __chip */ beepwave[16];

#endif

extern struct Image appicon_image;

#endif
