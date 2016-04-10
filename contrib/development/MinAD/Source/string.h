/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef _STRING_H
#define _STRING_H 1

/*------------------------------------------------------------------------*/

LONG VSCountf(const UBYTE *format, va_list args);
LONG  SCountf(const UBYTE *format, ...);

LONG VSPrintf(UBYTE *str, const UBYTE *format, va_list args);
LONG  SPrintf(UBYTE *str, const UBYTE *format, ...);

LONG VSNPrintf(UBYTE *str, ULONG n, const UBYTE *format, va_list args);
LONG  SNPrintf(UBYTE *str, ULONG n, const UBYTE *format, ...);

/*------------------------------------------------------------------------*/

#endif

