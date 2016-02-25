/****************************************************************************
*                                                                           *
* color.h -- Lunapaint,                                                     *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#ifndef _colorconversion_h_
#define _colorconversion_h_

#define MAXCOLOR 65535
#define MAXCOLOR_DBL 65535.0
#define TRANSCOLOR 0x4444444444440000ULL
#define TRANSVALUE 0x0000000000004444ULL


/*
    Some color structs
*/

struct rgbData
{
    unsigned int r, g, b;
};

struct rgbaDataL
{
    unsigned long long int r, g, b, a;
};

struct rgbaData
{
    unsigned int r, g, b, a;
};

struct rgba32
{
    unsigned char r, g, b, a;
};

struct rgb24
{
    unsigned char r, g, b;
};

struct rgba64
{
    unsigned short r, g, b, a;
};

struct bgra32
{
    unsigned char b, g, r, a;
};

struct abgr32
{
    unsigned char a, b, g, r;
};

/*
    Take an unsigned int color in ABGR format and convert to a
    struct containing the R, G, B channels separately
*/
struct rgbData paletteColorToRGB ( unsigned int color );

/*
    Palettecolor to rgba64
*/
struct rgba64 PaletteToRgba64 ( unsigned int rgb );

/*
    Take unsigned long long int and extract rgba data
    in rgbaData
*/
struct rgbaData canvasColorToRGBA ( unsigned long long int color );

/*
    Take unsigned long long int and extract rgba and return
    in a struct typedef rgbaDataL
*/
struct rgbaDataL canvasColorToRGBA_ull ( unsigned long long int color );

/*
    Take unsigned int and return rgba
*/
struct rgbaData bufferToRGBA ( unsigned int color );

/*
    Take r, g, b separately and join them to a palette color of type
    unsigned int...
*/
unsigned int RGBtoPaletteColor ( unsigned int r, unsigned int g, unsigned int b );

/*
    Take unsigned long long int and return unsigned int
*/
unsigned int bufferToScreenColor ( unsigned long long int col );

/*
    Take a palette color and convert it to a buffer color
*/
unsigned long long int PaletteToBuffercolor ( unsigned int rgb );

/*
    Convert a 16-bit pr color to a 8-bit pr color pixel
*/
unsigned int canvasToWindowPixel ( unsigned long long int source );

/*
    Convert to an ARGB format for export etc
*/
unsigned int canvasToARGBPixel ( unsigned long long int source );

#endif
