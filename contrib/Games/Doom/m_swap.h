// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Endianess handling, swapping 16bit and 32bit.
//
//-----------------------------------------------------------------------------


#ifndef __M_SWAP__
#define __M_SWAP__


#ifdef __GNUG__
#pragma interface
#endif

#ifdef AROS


#ifndef AROS_SYSTEM_H
#include <aros/system.h>
#endif

#if AROS_BIG_ENDIAN
#define __BIG_ENDIAN__
#endif

#endif

// Endianess handling.
// WAD files are stored little endian.
#ifdef __BIG_ENDIAN__
unsigned short	SwapSHORT(unsigned short);
unsigned long	SwapLONG(unsigned long);
#define SWAPSHORT(x)	((short)SwapSHORT((unsigned short) (x)))
#define SWAPLONG(x)     ((long)SwapLONG((unsigned long) (x)))
#else
#define SWAPSHORT(x)	(x)
#define SWAPLONG(x)     (x)
#endif




#endif
//-----------------------------------------------------------------------------
//
// $Log$
// Revision 1.2  2003/07/19 02:59:04  iaint
// Remove unnecessary inclusion of <aros/machine.h>. No file should really
// include this directly, except for one or two other headers.
//
// Revision 1.1  2000/02/29 18:21:06  stegerg
// Doom port based on ADoomPPC. Read README.AROS!
//
//
//-----------------------------------------------------------------------------
