/****************************************************************************
*                                                                           *
* color.c -- Lunapaint,                                                     *
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

#include "color.h"


char filledDrawing;
unsigned int *globalPalette;
unsigned int globalColor;
unsigned int currColor;
unsigned int prevColor;


struct rgbData paletteColorToRGB ( unsigned int color )
{
    unsigned int r, g, b;
    r = color << 24;    r = r >> 24;
    g = color << 16;    g = g >> 24;
    b = color << 8;     b = b >> 24;

    struct rgbData mydata = { r, g, b };
    return mydata;
}

struct rgbaDataL canvasColorToRGBA_ull ( unsigned long long int color )
{
    unsigned long long int r, g, b, a;
    r = color >> 48;
    g = color << 16; g = g >> 48;
    b = color << 32; b = b >> 48;
    a = color << 48; a = a >> 48;

    struct rgbaDataL mydata = { r, g, b, a };
    return mydata;
}

struct rgbaData canvasColorToRGBA ( unsigned long long int color )
{
    struct rgbaDataL rgbal = canvasColorToRGBA_ull ( color );
    unsigned int r, g, b, a;
    r = rgbal.r / 256;
    g = rgbal.g / 256;
    b = rgbal.b / 256;
    a = rgbal.a / 256;
    struct rgbaData rgba = { r, g, b, a };
    return rgba;
}

unsigned int RGBtoPaletteColor ( unsigned int r, unsigned int g, unsigned int b )
{
    unsigned int rgb = ( r ) | ( g << 8 ) | ( b << 16 );
    return rgb;
}

unsigned int bufferToScreenColor ( unsigned long long int col )
{
    struct rgbaDataL rgbal = canvasColorToRGBA_ull ( col );
    unsigned int r, g, b, a;
    r = rgbal.r / 256;
    g = rgbal.g / 256;
    b = rgbal.b / 256;
    a = rgbal.a / 256;
    return ( unsigned int )( r << 24 | g << 16 | b << 8 | a );
}

struct rgbaData bufferToRGBA ( unsigned int color )
{
    unsigned int r, g, b, a;
    r = color >> 24;
    g = color << 8;     g = g >> 24;
    b = color << 16;    b = b >> 24;
    a = color << 24;    a = a >> 24;
    struct rgbaData mydata = { r, g, b, a };
    return mydata;
}

unsigned long long int PaletteToBuffercolor ( unsigned int rgb )
{
    unsigned long long int r, g, b, rgba;
    r = ( rgb << 24 ) >> 24; r = ( int )( r / 256.0 * MAXCOLOR );
    g = ( rgb << 16 ) >> 24; g = ( int )( g / 256.0 * MAXCOLOR );
    b = ( rgb << 8  ) >> 24; b = ( int )( b / 256.0 * MAXCOLOR );
    rgba = r << 48 | g << 32 | b << 16 | MAXCOLOR;
    return rgba;
}

struct rgba64 PaletteToRgba64 ( unsigned int rgb )
{
    unsigned long long int r, g, b;
    r = ( rgb << 24 ) >> 24; r = ( int )( r / 256.0 * MAXCOLOR );
    g = ( rgb << 16 ) >> 24; g = ( int )( g / 256.0 * MAXCOLOR );
    b = ( rgb << 8  ) >> 24; b = ( int )( b / 256.0 * MAXCOLOR );
    return ( struct rgba64 ){ r, g, b, MAXCOLOR };
}

unsigned int canvasToARGBPixel ( unsigned long long int source )
{
    // Fetch RGBA from 64-bit source;
    unsigned long long int R, G, B, A;
    R = source;         R = R >> 48;
    G = source << 16;   G = G >> 48;
    B = source << 32;   B = B >> 48;
    A = source << 48;   A = A >> 48;

    // Convert to 8-bit integer
    int r, g, b, a;
    r = ( int )( R / 256 );
    g = ( int )( G / 256 );
    b = ( int )( B / 256 );
    a = ( int )( A / 256 );

    unsigned int pixel = ( r << 8 ) | ( g << 16 ) | ( b << 24 ) | 0;
    return pixel;
}

unsigned int canvasToWindowPixel ( unsigned long long int source )
{
    // Fetch RGBA from 64-bit source;
    unsigned long long int R, G, B, A;
    R = source;         R = R >> 48;
    G = source << 16;   G = G >> 48;
    B = source << 32;   B = B >> 48;
    A = source << 48;   A = A >> 48;

    // Convert to 8-bit integer
    int r, g, b, a;
    r = ( int )( R / 256 );
    g = ( int )( G / 256 );
    b = ( int )( B / 256 );
    a = ( int )( A / 256 );

    unsigned int pixel = ( r ) | ( g << 8 ) | ( b << 16 ) | 0;
    return pixel;
}
