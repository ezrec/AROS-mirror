/****************************************************************************
*                                                                           *
* effects.c -- Lunapaint,                                                   *
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

#include "effects.h"

void effectOffset ( int x, int y, struct oCanvas *canvas )
{
    unsigned long long int *buffer = AllocVec ( canvas->width * canvas->height * 8, MEMF_ANY );

    unsigned long long int *buf = canvas->activebuffer;

    int cy, cx, offy, offx;

    // Y pass

    for ( cy = 0; cy < canvas->height; cy++ )
    {
        offy = ( cy + y ) % canvas->height;
        memcpy ( &buffer[ offy * canvas->width ], &buf[ cy * canvas->width ], canvas->width * 8 );
    }

    // Copy back

    memcpy ( buf, buffer, canvas->width * canvas->height * 8 );

    // X pass

    for ( cy = 0; cy < canvas->height; cy++ )
    {
        int dataoffy = cy * canvas->width;

        for ( cx = 0; cx < canvas->width; cx++ )
        {
            offx = ( cx + x ) % canvas->width;
            buffer[ dataoffy + offx ] = buf[ dataoffy + cx ];
        }
    }

    // Copy back

    memcpy ( buf, buffer, canvas->width * canvas->height * 8 );

    FreeVec ( buffer );
}

void effectFlipVert ( struct oCanvas *canvas )
{
    unsigned long long int *buf = canvas->activebuffer;
    int cx; for ( cx = 0; cx < canvas->width; cx++ )
    {
        int range = floor ( canvas->height / 2.0 );
        int cy; for ( cy = 0; cy < range; cy++ )
        {
            int yoff1 = cy * canvas->width + cx;
            int yoff2 = ( canvas->height - 1 - cy ) * canvas->width + cx;
            unsigned long long int col = buf[ yoff2 ];
            buf[ yoff2 ] = buf[ yoff1 ];
            buf[ yoff1 ] = col;
        }
    }
}

void effectFlipHoriz ( struct oCanvas *canvas )
{
    unsigned long long int *buf = canvas->activebuffer;
    int cy; for ( cy = 0; cy < canvas->height; cy++ )
    {
        int yoff = cy * canvas->width;
        int range = floor ( canvas->width / 2.0 );
        int cx; for ( cx = 0; cx < range; cx++ )
        {
            unsigned long long int col = buf[ yoff + cx ];
            buf[ yoff + cx ] = buf[ yoff + ( canvas->width - 1 - cx ) ];
            buf[ yoff + ( canvas->width - 1 - cx ) ] = col;
        }
    }
}
