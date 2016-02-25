/****************************************************************************
*                                                                           *
* drawtools.c -- Lunapaint,                                                 *
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

#include "drawtools.h"


int fStackPointer;
BOOL isFilling;


BOOL fPop ( int *x, int *y, int pitch, int *fStack )
{
    if ( fStackPointer > 0 )
    {
        int p = fStack[ fStackPointer ];
        *x = p % pitch;
        *y = p / pitch;
        fStackPointer--;
        return 1;
    }
    return 0;
}

void fPush ( int x, int y, int limit, int pitch, int *fStack )
{
    if ( fStackPointer + 1 < limit )
    {
        fStackPointer++;
        fStack[ fStackPointer ] =  pitch * y + x;
    }
}

struct Affrect drawLine (
    double x1, double y1, double x2, double y2,
    int bufferwidth, int bufferheight,
    unsigned long long int *buffer, BOOL subtract_endpoint
)
{
    // Find the area drawn on and paint

    double X = 0, Y = 0, W = 0, H = 0;

    if ( x2 > x1 )
    {
        X = x1; W = x2 - x1;
    }
    else
    {
        X = x2; W = x1 - x2;
    }
    if ( y2 > y1 )
    {
        Y = y1; H = y2 - y1;
    }
    else
    {
        Y = y2; H = y1 - y2;
    }

    double dist;

    float slopex = 0.0, slopey = 0.0;

    if ( W > H )
    {
        slopex = 1.0;
        slopey =  H / W;
        dist = W;
    }
    else if ( H > W )
    {
        slopex = W / H;
        slopey = 1.0;
        dist = H;
    }
    else
    {
        slopex = 1.0;
        slopey = 1.0;
        dist = W;
    }

    // If we are to subtract endpoint
    dist -= ( subtract_endpoint ? ( 1.0 / globalActiveCanvas->zoom ) : 0.0 );

    double coordx = x1, coordy = y1;

    if ( x1 > x2 ) slopex = -slopex;
    if ( y1 > y2 ) slopey = -slopey;

    int i = 0; for ( ; i <= dist; i++ )
    {
        if ( i % brushTool.step == 0 )
            plotBrush ( coordx, coordy, bufferwidth, bufferheight, buffer );
        coordx += slopex; coordy += slopey;
    }

    // Return affected coorinates
    int offx = brushTool.width / 2.0;
    int offy = brushTool.height / 2.0;
    struct Affrect rect = {
        ( int )( X - 1 - offx ), ( int )( Y - 1 - offy ),
        ( int )( W + brushTool.width + 2 ), ( int )( H + brushTool.height + 2 )
    };
    return rect;
}

void drawLineCharbuf (
    double x1, double y1, double x2, double y2,
    int bufferwidth, int bufferheight,
    unsigned char *buffer, unsigned char value
)
{
    // Find the area drawn on and paint

    double X = 0.0, Y = 0.0, W = 0.0, H = 0.0;

    if ( x2 > x1 )
    {
        X = x1; W = x2 - x1;
    }
    else
    {
        X = x2; W = x1 - x2;
    }
    if ( y2 > y1 )
    {
        Y = y1; H = y2 - y1;
    }
    else
    {
        Y = y2; H = y1 - y2;
    }

    double dist;

    float slopex, slopey;

    if ( W > H )
    {
        slopex = 1.0; slopey = H / W; dist = W;
    }
    else if ( H > W )
    {
        slopex = W / H; slopey = 1.0; dist = H;
    }
    else
    {
        slopex = 1.0; slopey = 1.0; dist = W;
    }

    double coordx = x1, coordy = y1;

    if ( x1 > x2 ) slopex = -slopex;
    if ( y1 > y2 ) slopey = -slopey;

    int i = 0; for ( ; i <= dist; i++ )
    {
        if (
            ( int )coordy >= 0 && ( int )coordy < bufferheight &&
            ( int )coordx >= 0 && ( int )coordx < bufferwidth
        )
        {
            int coord = ( int )coordy * bufferwidth + ( int )coordx;
            buffer[ coord ] = value;
        }
        coordx += slopex; coordy += slopey;
    }
}

struct Affrect plotBrush (
    double x, double y,
    int bufferwidth, int bufferheight,
    unsigned long long int *buffer
)
{
    // Offset of brush top/left drawing in relation to x/y
    double offx = 0.0, offy = 0.0;

    // Special case for 1x1 brushes
    if ( brushTool.width == 1 && brushTool.height == 1 )
    {
        struct rgba64 sourcecol = *( struct rgba64 *)brushTool.buffer;
        if ( x < 0 || x >= bufferwidth || y < 0 || y >= bufferheight )
            return ( struct Affrect ){ 0, 0, 0, 0 };
        if ( brushTool.antialias )
            pixelAntialias ( x, y, sourcecol, bufferwidth, bufferheight, buffer );
        else
            pixelPlain ( ( int )x, ( int )y, sourcecol, bufferwidth, bufferheight, buffer );
    }
    else
    {
        // Precision to center the brush on x,y coordinate
        offx = brushTool.width / 2.0;
        offy = brushTool.height / 2.0;
        double offxx = x - offx;
        double offyy = y - offy;

        if ( brushTool.antialias )
        {
            int by = 0; for ( ; by < brushTool.height; by++ )
            {
                int ymul = by * brushTool.width;
                double py = by + offyy;
                int bx = 0; for ( ; bx < brushTool.width; bx++ )
                {
                    int pos = ymul + bx;
                    double px = bx + offxx;
                    struct rgba64 col2 = *( struct rgba64 *)&brushTool.buffer[ pos ];
                    pixelAntialias ( px, py, col2, bufferwidth, bufferheight, buffer );

                }
            }
        }
        else
        {
            int by = 0; for ( ; by < brushTool.height; by++ )
            {
                int ymul = by * brushTool.width;
                double py = by + offyy;
                int bx = 0; for ( ; bx < brushTool.width; bx++ )
                {
                    int pos = ymul + bx;
                    double px = bx + offxx;
                    struct rgba64 col2 = *( struct rgba64 *)&brushTool.buffer[ pos ];
                    pixelPlain ( ( int )px, ( int )py, col2, bufferwidth, bufferheight, buffer );
                }
            }
        }
    }

    // Return the affected area (x,y,w,h)
    struct Affrect rect = { ( int )( x - offx - 1 ), ( int )( y - offy - 1 ), brushTool.width + 2, brushTool.height + 2 };
    return rect;
}

void pixelAntialias (
    double x, double y, struct rgba64 paintcol,
    int bw, int bh, unsigned long long int *buf
)
{
    int ox = ( int )x;
    int oy = ( int )y;

    int any = 0; for ( ;any < 2; any++ )
    {
        int datay = oy + any;
        if ( datay < 0 || datay >= bh ) continue;
        int yml = datay * bw;
        int anx = 0; for ( ;anx < 2; anx++ )
        {
            int datax = ox + anx;
            if ( datax < 0 || datax >= bw ) continue;
            int dataoffset = yml + datax;
            struct rgba64 sourcecol = *( struct rgba64 *)&buf[ dataoffset ];
            buf[ dataoffset ] = processPixel (
                sourcecol, paintcol, datax, datay, x - datax, y - datay
            );
        }
    }
}

void pixelPlain ( int x, int y, struct rgba64 paintcol, int bw, int bh, unsigned long long int *buf )
{
    if ( y < 0 || y >= bh || x < 0 || x >= bw ) return;
    int dataoffset = ( y * bw ) + x;
    struct rgba64 sourcecol = *( struct rgba64 *)&buf[ dataoffset ];
    buf[ dataoffset ] = processPixel ( sourcecol, paintcol, x, y, x, y );
}

unsigned long long int processPixel (
    struct rgba64 origCol, struct rgba64 paintCol,
    double x, double y, double diffx, double diffy
)
{
    // Align colors correctly ( SHOULD BE FIXED IN THE FUTURE! )
    int r1 = origCol.a, 	g1 = origCol.b, 	b1 = origCol.g, 	a1 = origCol.r,
        r2 = paintCol.a, 	g2 = paintCol.b, 	b2 = paintCol.g, 	a2 = paintCol.r;

    // Take into account the brush tool opacity and power
    a2 = ( a2 / 255.0 * brushTool.opacity ) / 100.0 * brushTool.power;

    // Calculate alpha
    double alpha = a2 / MAXCOLOR_DBL;

    // Decrease alpha with antialiasing calc.
    if ( brushTool.antialias )
    {
        double dist = sqrt ( ( diffx * diffx ) + ( diffy * diffy ) );
        alpha -= dist * alpha;
    }
    if ( alpha <= 0 )
        return *( unsigned long long int *)&origCol;
    else if ( alpha > 1 ) alpha = 1;

    switch ( brushTool.paintmode )
    {
        case LUNA_PAINTMODE_NORMAL:

            // Wonderful blending exception
            if ( a1 <= 0 )
            {
                r1 = r2;
                g1 = g2;
                b1 = b2;
                if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                    a1 = a2 * alpha;
            }
            else
            {
                r1 -= ( r1 - r2 ) * alpha;
                g1 -= ( g1 - g2 ) * alpha;
                b1 -= ( b1 - b2 ) * alpha;

                if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                {
                    a1 += a2 * alpha;
                    if ( a1 > MAXCOLOR ) a1 = MAXCOLOR;
                }
            }
            origCol.r = r1; origCol.g = g1;
            origCol.b = b1; origCol.a = a1;

            break;

        case LUNA_PAINTMODE_COLOR:

            {
                struct rgba64 cols = PaletteToRgba64 ( globalPalette[ currColor ] );
                if ( a1 <= 0 )
                {
                    r1 = cols.r;
                    g1 = cols.g;
                    b1 = cols.b;
                    if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                        a1 = a2 * alpha;
                }
                else
                {
                    r1 -= ( r1 - cols.r ) * alpha;
                    g1 -= ( g1 - cols.g ) * alpha;
                    b1 -= ( b1 - cols.b ) * alpha;
                    if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                    {
                        a1 += a2 * alpha;
                        if ( a1 > MAXCOLOR ) a1 = MAXCOLOR;
                    }
                }
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }

            break;

        case LUNA_PAINTMODE_LIGHTEN:

            if ( a1 >= 0 )
            {
                r1 += ( r1 + 255 ) * alpha;
                g1 += ( g1 + 255 ) * alpha;
                b1 += ( b1 + 255 ) * alpha;
                if ( r1 > MAXCOLOR ) r1 = MAXCOLOR;
                if ( g1 > MAXCOLOR ) g1 = MAXCOLOR;
                if ( b1 > MAXCOLOR ) b1 = MAXCOLOR;
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }

            break;

        case LUNA_PAINTMODE_DARKEN:

            if ( a1 >= 0 )
            {
                r1 -= ( 255 + r1 ) * alpha;
                g1 -= ( 255 + g1 ) * alpha;
                b1 -= ( 255 + b1 ) * alpha;
                if ( r1 < 0 ) r1 = 0;
                if ( g1 < 0 ) g1 = 0;
                if ( b1 < 0 ) b1 = 0;
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }

            break;

        case LUNA_PAINTMODE_COLORIZE:

            if ( a1 >= 0 )
            {
                // Different product of each color
                int r3 = 0, g3 = 0, b3 = 0;

                double toneO = ( ( r1 + g1 + b1 ) / 3 );
                double toneC = ( ( r2 + g2 + b2 ) / 3 );


                r3 = ( r2 * toneO / toneC ); if ( r3 > MAXCOLOR ) r3 = MAXCOLOR; if ( r3 < 0 ) r3 = 0;
                g3 = ( g2 * toneO / toneC ); if ( g3 > MAXCOLOR ) g3 = MAXCOLOR; if ( g3 < 0 ) g3 = 0;
                b3 = ( b2 * toneO / toneC ); if ( b3 > MAXCOLOR ) b3 = MAXCOLOR; if ( b3 < 0 ) b3 = 0;

                r1 -= ( r1 - r3 ) * alpha;
                g1 -= ( g1 - g3 ) * alpha;
                b1 -= ( b1 - b3 ) * alpha;

                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }

            break;

        case LUNA_PAINTMODE_BLUR:

            {
                // TODO: Make power ajustable?
                int power = 1;

                int r = 0, g = 0, b = 0, a = 0;
                double samples = 0.0;

                int sy3 = 0; for ( sy3 = -power; sy3 <= power; sy3++ )
                {
                    int ysy3 = y + sy3;
                    int yoffset = ysy3 * globalActiveCanvas->width;
                    int sx3 = 0; for ( sx3 = -power; sx3 <= power; sx3++ )
                    {
                        int xsx3 = x + sx3;

                        if (
                            xsx3 >= 0 && xsx3 < globalActiveCanvas->width &&
                            ysy3 >= 0 && ysy3 < globalActiveCanvas->height )
                        {
                            int offset = yoffset + xsx3;
                            struct rgba64 tmp = *( struct rgba64 *)&globalActiveCanvas->activebuffer[ offset ];
                            r += tmp.a;
                            g += tmp.b;
                            b += tmp.g;
                            a += tmp.r;
                            samples++;
                        }
                    }
                }

                if ( samples > 0 )
                {
                    r = r / samples;
                    g = g / samples;
                    b = b / samples;

                    r1 -= ( ( r1 - r ) * alpha );
                    g1 -= ( ( g1 - g ) * alpha );
                    b1 -= ( ( b1 - b ) * alpha );

                    if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                    {
                        a = ( a / samples );
                        a1 -= ( ( a1 - a ) * alpha );
                    }
                }

                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }
            break;

        case LUNA_PAINTMODE_SMUDGE:

            {
                // TODO: pick the color that is in front of the mouse movement
                int diffx = pMouseX - dMouseX;
                int diffy = pMouseY - dMouseY;


                // TODO: Make power ajustable?
                int power = 1;

                int r = 0, g = 0, b = 0, a = 0;
                double samples = 0;

                int sy3; for ( sy3 = -power + diffy; sy3 <= power + diffy; sy3++ )
                {
                    int ysy3 = y + sy3;
                    int yoffset =  ysy3 * globalActiveCanvas->width;
                    int sx3; for ( sx3 = -power + diffx; sx3 <= power + diffx; sx3++ )
                    {
                        int xsx3 = x + sx3;
                        if (
                            xsx3 >= 0 && xsx3 < globalActiveCanvas->width &&
                            ysy3 >= 0 && ysy3 < globalActiveCanvas->height )
                        {
                            int offset = yoffset + xsx3;
                            struct rgba64 tmp = *( struct rgba64 *)&globalActiveCanvas->activebuffer[ offset ];
                            r += tmp.a;
                            g += tmp.b;
                            b += tmp.g;
                            a += tmp.r;
                            samples++;
                        }
                    }
                }
                if ( samples > 0 )
                {
                    r = ( r / samples );
                    g = ( g / samples );
                    b = ( b / samples );

                    r1 -= ( ( r1 - r ) * alpha );
                    g1 -= ( ( g1 - g ) * alpha );
                    b1 -= ( ( b1 - b ) * alpha );

                    if ( brushTool.opacitymode == LUNA_OPACITYMODE_ADD )
                    {
                        a = ( a / samples );
                        a1 -= ( ( a1 - a ) * alpha );
                    }
                }
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }
            break;

        case LUNA_PAINTMODE_ERASE:

            if ( a1 >= 0 )
            {
                a1 -= a2 * alpha; if ( a1 < 0 ) a1 = 0;
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }
            break;

        case LUNA_PAINTMODE_UNERASE:

            if ( a1 >= 0 )
            {
                a1 += a2 * alpha; if ( a1 > MAXCOLOR ) a1 = MAXCOLOR;
                origCol.r = r1; origCol.g = g1;
                origCol.b = b1; origCol.a = a1;
            }
            break;

    }

    // Resulting 64bit pixel
    struct rgba64 result = { origCol.a, origCol.b, origCol.g, origCol.r };

    // Snap to colors
    if ( globalColorMode == LUNA_COLORMODE_SNAP && brushTool.paintmode != LUNA_PAINTMODE_ERASE )
        result = snapToPalette ( result, FALSE );

    return *( unsigned long long int * )&result;
}

struct Affrect floodFill (
    int x, int y,
    int bufferwidth, int bufferheight,
    unsigned long long int color,
    unsigned long long int clickColor,
    unsigned long long int *buffer,
    unsigned int threshold
)
{
    // Exit if out of bounds for filling or otherwise we're not allowed
    if ( x < 0 || y < 0 || x >= bufferwidth || y >= bufferheight || clickColor == color )
        return ( struct Affrect ){ 0, 0, 0, 0 };

    // redraw all rect =)
    struct Affrect rect = { x, y, 1, 1 };

    unsigned int click8 = bufferToScreenColor ( clickColor );
    unsigned int clicka = ( click8 << 24 ) >> 24; // a part of rgba

    // Max stacksize
    int stacksize = bufferwidth * bufferheight;
    int *fStack = AllocVec ( 4 * stacksize, MEMF_ANY|MEMF_CLEAR );
    fStackPointer = 0;
    int fx = x, fy = y;

    // Add first pixel where the mouse clicked
    fPush ( fx, fy, stacksize, bufferwidth, fStack );

    // Do - until there's no more pixels to fill in the stack
    while ( fPop ( &fx, &fy, bufferwidth, fStack ) )
    {
        // fill at pixelpos
        int boffset = ( fy * bufferwidth );
        buffer[ boffset + fx ] = color;

        // Make sure we have the correct rect to update
        if ( fx < rect.x ) rect.x = fx;
        if ( fx > rect.x + rect.w ) rect.w = fx - rect.x + 1;
        if ( fy < rect.y ) rect.y = fx;
        if ( fy > rect.y + rect.h ) rect.h = fy - rect.y + 1;

        // Watch for buffer overflow
        if ( fStackPointer >= stacksize - 1 )
        break;

        // The colors to the n,e,s,w directions
        unsigned long long int topcol = 0ULL, botcol = 0ULL, lefcol = 0ULL, rigcol = 0ULL;
        unsigned int rigcol8 = 0U, lefcol8 = 0U, botcol8 = 0U, topcol8 = 0U;
        unsigned int riga = 0U, lefa = 0U, bota = 0U, topa = 0U; // <- a part of rgba

        // Make sure the coords bound to box
        BOOL tz = TRUE, bz = TRUE, lz = TRUE, rz = TRUE;
        if ( fx + 1 < bufferwidth )
        {
            rigcol = buffer[ boffset + fx + 1 ];
            rigcol8 = bufferToScreenColor ( rigcol );
            riga = ( rigcol8 << 24 ) >> 24;
        }
        else rz = FALSE;
        if ( fx - 1 >= 0 )
        {
            lefcol = buffer[ boffset + fx - 1 ];
            lefcol8 = bufferToScreenColor ( lefcol );
            lefa = ( lefcol8 << 24 ) >> 24;
        }
        else lz = FALSE;
        if ( fy + 1 < bufferheight )
        {
            botcol = buffer[ ( ( fy + 1 ) * bufferwidth ) + fx ];
            botcol8 = bufferToScreenColor ( botcol );
            bota = ( botcol8 << 24 ) >> 24;
        }
        else bz = FALSE;
        if ( fy - 1 >= 0 )
        {
            topcol = buffer[ ( ( fy - 1 ) * bufferwidth ) + fx ];
            topcol8 = bufferToScreenColor ( topcol );
            topa = ( topcol8 << 24 ) >> 24;
        }
        else tz = FALSE;

        // Add pixels within bounds 24-bit
        // Fill if clickcolor or that clickcolor alpha = 0 and destcolor alpha = 0
        if ( rigcol != color && rz && ( click8 == rigcol8 || riga + clicka == 0 ) )
            fPush ( fx + 1, fy, stacksize, bufferwidth, fStack );
        if ( lefcol != color && lz && ( click8 == lefcol8 || lefa + clicka == 0 ) )
            fPush ( fx - 1, fy, stacksize, bufferwidth, fStack );
        if ( botcol != color && bz && ( click8 == botcol8 || bota + clicka == 0 ) )
            fPush ( fx, fy + 1, stacksize, bufferwidth, fStack );
        if ( topcol != color && tz && ( click8 == topcol8 || topa + clicka == 0 ) )
            fPush ( fx, fy - 1, stacksize, bufferwidth, fStack );
    }
    FreeVec ( fStack );
    return rect;
}

void fillCharbuf (
    int x, int y, int bufferwidth, int bufferheight,
    unsigned char *buffer, unsigned char value
)
{

    // Max stacksize
    int stacksize = bufferwidth * bufferheight;
    int *fStack = AllocVec ( stacksize * 4, MEMF_ANY|MEMF_CLEAR );
    fStackPointer = 0;
    int fx = x, fy = y;
    unsigned char clickColor = buffer[ y * bufferwidth + x ];
    if ( clickColor == value ) return;

    // Exit if out of bounds for filling!
    if ( x < 0 || y < 0 || x >= bufferwidth || y >= bufferheight )
    {
        FreeVec ( fStack ); return;
    }

    // Add first pixel where the mouse clicked
    fPush ( fx, fy, stacksize, bufferwidth, fStack );

    // Do - until there's no more pixels to fill in the stack
    while ( fPop ( &fx, &fy, bufferwidth, fStack ) )
    {
        // Watch for buffer overflow
        if ( fStackPointer >= stacksize - 1 )
            break;

        // rowpos here
        int boffset = ( fy * bufferwidth );

        // Fill
        buffer[ boffset + fx ] = value;

        // The colors to the n,e,s,w directions
        unsigned char topcol = 0, botcol = 0, lefcol = 0, rigcol = 0;

        // Make sure the coords bound to box
        BOOL tz = TRUE, bz = TRUE, lz = TRUE, rz = TRUE;
        if ( fx + 1 < bufferwidth )
            rigcol = buffer[ boffset + fx + 1 ];
        else rz = FALSE;
        if ( fx - 1 >= 0 )
            lefcol = buffer[ boffset + fx - 1 ];
        else lz = FALSE;
        if ( fy + 1 < bufferheight )
            botcol = buffer[ ( ( fy + 1 ) * bufferwidth ) + fx ];
        else bz = FALSE;
        if ( fy - 1 >= 0 )
            topcol = buffer[ ( ( fy - 1 ) * bufferwidth ) + fx ];
        else tz = FALSE;

        // Add pixels within bounds
        if ( rz && clickColor == rigcol && value != rigcol )
            fPush ( fx + 1, fy, stacksize, bufferwidth, fStack );
        if ( lz && clickColor == lefcol && value != lefcol )
            fPush ( fx - 1, fy, stacksize, bufferwidth, fStack );
        if ( bz && clickColor == botcol && value != botcol )
            fPush ( fx, fy + 1, stacksize, bufferwidth, fStack );
        if ( tz && clickColor == topcol && value != topcol )
            fPush ( fx, fy - 1, stacksize, bufferwidth, fStack );
    }
    FreeVec ( fStack );
}


struct Affrect drawCircle (
    double x, double y, double w, double h,
    unsigned int bufferwidth, unsigned int bufferheight,
    unsigned long long int *buffer
)
{
    double prevx = cos ( RAD ) * w + x;
    double prevy = sin ( RAD ) * h + y;

    if ( filledDrawing != DRAW_DRAW )
    {
        // Setup data
        int size = h > w ? h : w;
        int size2 = size * 2 + 1;
        int datalen = size2 * size2;
        double fillradius = size;
        unsigned long long int *tmpBuf = AllocVec ( datalen * 8, MEMF_ANY );

        // Make a uniform circular shape
        int yc = 0; for ( ; yc < size2; yc++ )
        {
            int tmpoff = yc * size2;
            int xc = 0; for ( ; xc < size2; xc++ )
            {
                int tmpoffx = tmpoff + xc;
                if ( getDistance ( xc, yc, size, size ) < fillradius )
                    tmpBuf[ tmpoffx ] = 0xffffffffffffffffULL;
                else
                    tmpBuf[ tmpoffx ] = 0x0000000000000000ULL;
            }
        }

        // Copy the tmpbuf data scaled to fit the wanted shape
        int sx = x - w;
        int dx = x + w;
        int sy = y - h;
        int dy = y + h;
        int w2 = w * 2;
        int h2 = h * 2;
        double xw = x - w;
        double yh = y - h;

        BOOL ba = brushTool.antialias;
        brushTool.antialias = FALSE;

        yc = sy; for ( ; yc < dy; yc++ )
        {
            int pry = ( yc - yh ) / h2 * size2;
            int xc = sx; for ( ; xc < dx; xc++ )
            {
                int prx = ( xc - xw ) / w2 * size2;
                int offset = pry * size2 + prx;

                if ( offset < 0 || offset >= datalen ) continue;
                if ( tmpBuf[ offset ] != 0x0000000000000000ULL )
                    plotBrush ( xc, yc, bufferwidth, bufferheight, buffer );
            }
        }
        FreeVec ( tmpBuf );
        brushTool.antialias = ba;
    }

    if ( filledDrawing == DRAW_DRAW || filledDrawing == DRAW_DRAWFILLED )
    {
        int a = 0; for ( ; a <= 361; a++ )
        {
            double phase = a * RAD;
            double curx = cos ( phase ) * w + x;
            double cury = sin ( phase ) * h + y;

            if ( floor ( prevx ) != floor ( curx ) || floor ( prevy ) != floor ( cury ) )
            {
                drawLine (
                    prevx, prevy,
                    curx, cury,
                    bufferwidth, bufferheight,
                    buffer, FALSE
                );
                prevx = curx;
                prevy = cury;
            }
        }
    }

    int offx = brushTool.width / 2.0;
    int offy = brushTool.height / 2.0;
    struct Affrect rect = {
        x - w - offx - 2,
        y - h - offy - 2,
        ( w * 2 ) + ( ( offx + 2 ) * 2 ),
        ( h * 2 ) + ( ( offy + 2 ) * 2 )
    };
    return rect;
}

struct rgba64 snapToPalette ( struct rgba64 color, BOOL selectIndex )
{
    int rating = 1024; // high number
    int index = 0;
    struct rgba32 testcol = { color.r / 256, color.g / 256, color.b / 256, color.a / 256 };

    // find the colors that are the most alike
    // giving the best diff rating (the least different)
    int c = 0; for ( ; c < 256; c++ )
    {
        struct rgba32 curcol = *( struct rgba32 *)&globalPalette[ c ];
        int test =
            abs( curcol.r - testcol.a ) +
            abs( curcol.g - testcol.b ) +
            abs( curcol.b - testcol.g );
        if ( test < rating )
        {
            index = c;
            rating = test;
        }
    }
    // Return the result
    struct rgba32 rescol = *( struct rgba32 *)&globalPalette[ index ];

    if ( selectIndex )
    {
        currColor = index;
        DoMethod ( tbxAreaPalette, MUIM_Draw );
    }

    struct rgba64 res = {
        MAXCOLOR,
        ( unsigned long long int )( rescol.b / 255.0 * MAXCOLOR ),
        ( unsigned long long int )( rescol.g / 255.0 * MAXCOLOR ),
        ( unsigned long long int )( rescol.r / 255.0 * MAXCOLOR )
    };
    return res;
}
