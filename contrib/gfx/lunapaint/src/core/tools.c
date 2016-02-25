/****************************************************************************
*                                                                           *
* tools.c -- Lunapaint,                                                     *
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

#include "tools.h"
#define MODE_FILL 1
#define MODE_SEEK 0

BOOL toolBrush ( )
{
    if ( !brushTool.initialized )
    {
        makeToolBrush ( );
        brushTool.initialized = TRUE;
    }

    if ( MouseButtonL || MouseButtonR )
    {
        struct WindowList *window = globalActiveWindow;

        // Remember old values
        pMouseX = dMouseX;
        pMouseY = dMouseY;
        dMouseX = cMouseX;
        dMouseY = cMouseY;

        // Draw from - to
        double x1 = pMouseX;
        double y1 = pMouseY;
        double x2 = dMouseX;
        double y2 = dMouseY;

        struct Affrect rect = { 0, 0, 0, 0 };

        // Setup or empty the contour buffer for filled drawing
        if ( brushTool.RecordContour && brushTool.ContourBuffer == NULL && filledDrawing != DRAW_DRAW )
        {
            brushTool.ContourMinX = x2;
            brushTool.ContourMinY = y2;
            brushTool.ContourMaxX = x2;
            brushTool.ContourMaxY = y2;
        }

        // Plot
        if ( y2 == y1 && x2 == x1 )
        {
            // plot brush
            rect = plotBrush (
                x1, y1,
                globalActiveCanvas->width,
                globalActiveCanvas->height,
                globalActiveCanvas->activebuffer
            );
            // Add coordinates to contour buffer if filling
            if ( brushTool.RecordContour && filledDrawing != DRAW_DRAW )
            {
                addListValue ( x1, y1, &brushTool.ContourBuffer );
                if ( x1 >= brushTool.ContourMaxX ) brushTool.ContourMaxX = x1;
                if ( x1 <= brushTool.ContourMinX ) brushTool.ContourMinX = x1;
                if ( y1 >= brushTool.ContourMaxY ) brushTool.ContourMaxY = y1;
                if ( y1 <= brushTool.ContourMinY ) brushTool.ContourMinY = y1;
            }
        }
        // Draw a line and get back
        // absolute buffer coorinates
        else
        {
            rect = drawLine (
                x1, y1, x2, y2,
                globalActiveCanvas->width,
                globalActiveCanvas->height,
                globalActiveCanvas->activebuffer,
                TRUE
            );
            // Add coordinates to contour buffer if filling
            if ( brushTool.RecordContour && filledDrawing != DRAW_DRAW )
            {
                addListValue ( x1, y1, &brushTool.ContourBuffer );
                addListValue ( x2, y2, &brushTool.ContourBuffer );
                if ( x1 >= brushTool.ContourMaxX ) brushTool.ContourMaxX = x1;
                if ( x1 <= brushTool.ContourMinX ) brushTool.ContourMinX = x1;
                if ( y1 >= brushTool.ContourMaxY ) brushTool.ContourMaxY = y1;
                if ( y1 <= brushTool.ContourMinY ) brushTool.ContourMinY = y1;
                if ( x2 >= brushTool.ContourMaxX ) brushTool.ContourMaxX = x2;
                if ( x2 <= brushTool.ContourMinX ) brushTool.ContourMinX = x2;
                if ( y2 >= brushTool.ContourMaxY ) brushTool.ContourMaxY = y2;
                if ( y2 <= brushTool.ContourMinY ) brushTool.ContourMinY = y2;
            }
        }

        // Update affected screen
        // blitAreaRect only takes absolute buffer coordinates
        blitAreaRect (
            rect.x, rect.y,
            rect.w, rect.h,
            globalActiveCanvas,
            _rp ( window->area )
        );
        return TRUE;
    }
    else
    {
        if ( !brushTool.RecordContour && brushTool.ContourBuffer != NULL && filledDrawing != DRAW_DRAW )
        {
            toolBrushFill ( );
            freeValueList ( &brushTool.ContourBuffer );
            globalActiveCanvas->winHasChanged = TRUE;
            DoMethod ( globalActiveWindow->area, MUIM_Draw );
        }
    }
    return FALSE;
}

void toolBrushFill ( )
{
    // Make a buffer with brushsize margin
    int margin = (
        (
            brushTool.width > brushTool.height ?
                brushTool.width :
                brushTool.height
        ) / 2.0
    ) + 2;

    // Double mardin.. optimization
    int margin2 = margin * 2;
    int MinX = brushTool.ContourMinX;
    int MinY = brushTool.ContourMinY;
    int MaxX = brushTool.ContourMaxX;
    int MaxY = brushTool.ContourMaxY;

    int bufwidth = ( MaxX - MinX ) + margin2;
    int bufheight = ( MaxY - MinY ) + margin2;
    int size = bufheight * bufwidth;
    unsigned char *pixelBuffer = AllocVec ( size, MEMF_ANY );

    // Clear buffer with 1
    int i = 0; for ( ; i < size; i++ ) pixelBuffer[ i ] = 1;

    // Draw shape
    struct ValueList *ptr = brushTool.ContourBuffer;
    double px = ( ptr->x - MinX ) + margin, py = ( ptr->y - MinY ) + margin, dx = 0, dy = 0;
    while ( ptr != NULL )
    {
        dx = ( ptr->x - MinX ) + margin; // draw x - margin offset
        dy = ( ptr->y - MinY ) + margin; //
        drawLineCharbuf ( px, py, dx, dy, bufwidth, bufheight, pixelBuffer, 2 );
        ptr = ptr->Next;
        px = dx; // previous x
        py = dy; // p y
    }

    // Connect shape, head to tail
    ptr = brushTool.ContourBuffer;
    dx = ( ptr->x - MinX ) + margin;
    dy = ( ptr->y - MinY ) + margin;
    drawLineCharbuf ( px, py, dx, dy, bufwidth, bufheight, pixelBuffer, 2 );

    // Fill outside area
    fillCharbuf ( 0, 0, bufwidth, bufheight, pixelBuffer, 2 );

    // Put pixels on canvas
    for ( i = 0; i < size; i++ )
    {
        // Coords and reset marker
        if ( pixelBuffer[ i ] == 1 )
        {
            int x = ( i % bufwidth ) + MinX - margin;
            int y = ( i / bufwidth ) + MinY - margin;
            plotBrush (
                x, y,
                globalActiveCanvas->width,
                globalActiveCanvas->height,
                globalActiveCanvas->activebuffer
            );
        }
    }

    // Remove outline and redraw outline
    if ( brushTool.antialias )
    {
        double x[ 4 ] = { -1, 1,  0,  0 };
        double y[ 4 ] = {  0, 0, -1, 1 };
        int z = 0; for ( ; z < 4; z++ )
        {
            ptr = brushTool.ContourBuffer; i = 0;
            px = ptr->x + x[ 0 ]; py = ptr->y + y[ 0 ];
            while ( ptr != NULL )
            {
                dx = ptr->x + x[ z ]; // draw x - with 1px offset
                dy = ptr->y + y[ z ]; //
                drawLine ( px, py, dx, dy, globalActiveCanvas->width, globalActiveCanvas->width, globalActiveCanvas->activebuffer, 0 );
                ptr = ptr->Next;
                px = dx; // previous x
                py = dy; // p y
                i++;
            }
            ptr = brushTool.ContourBuffer;
            dx = ptr->x + x[ z ]; dy = ptr->y + y[ z ];
            drawLine ( px, py, dx, dy, globalActiveCanvas->width, globalActiveCanvas->width, globalActiveCanvas->activebuffer, 0 );
        }
    }

    // Clean up
    FreeVec ( pixelBuffer );

}


void makeToolBrush ( )
{
    if ( brushTool.buffer != NULL )
        FreeVec ( brushTool.buffer );

    int buflen = brushTool.width * brushTool.height;
    brushTool.buffer = AllocVec ( buflen * 8, MEMF_ANY );

    // Only one radius is used when generating circular brushes
    double hRad = brushTool.width / 2.0;

    struct rgbaDataL rgba = canvasColorToRGBA_ull (
        PaletteToBuffercolor ( globalColor )
    );

    unsigned long long int a = 0;
    unsigned long long int alpha = MAXCOLOR;

    // Normally for odd brushes
    int i = 0; for ( ; i < buflen; i++ )
    {
        int x = i % brushTool.width;
        int y = i / brushTool.width;
        double dist = getDistance ( hRad, hRad, x, y );
        if ( dist > hRad )
            a = 0;
        else
        {
            if ( brushTool.feather ) a = alpha - ( dist / hRad * alpha );
            else a = alpha;
        }
        brushTool.buffer[ i ] = ( rgba.r << 48 ) | ( rgba.g << 32 ) | ( rgba.b << 16 ) | a;
    }
    // Refresh brush preview
    DoMethod ( tbxAreaPreview, MUIM_Draw );
}


BOOL toolFill ( )
{
    if ( MouseButtonL && !isFilling )
    {
        isFilling = TRUE;
        int x = cMouseX;
        int y = cMouseY;

        unsigned long long int ccolor =
            PaletteToBuffercolor ( globalColor );
        unsigned long long int dcolor =
            globalActiveCanvas->activebuffer[ ( y * globalActiveCanvas->width ) + x ];

        // Only fill if dest color is not current color!
        // No filling on color which is being filled with!
        // That would produce ill results!
        if ( ccolor != dcolor )
        {
            floodFill (
                x, y,
                globalActiveCanvas->width, globalActiveCanvas->height,
                ccolor, dcolor,
                globalActiveCanvas->activebuffer, 255
            );
        }
        globalActiveCanvas->winHasChanged = TRUE;
        DoMethod ( globalActiveWindow->area, MUIM_Draw, FALSE );
        isFilling = FALSE;
        return TRUE;
    }
    return FALSE;
}


BOOL toolLine ( )
{
    if ( lineTool.initialized != 1 )
    {
        lineTool.mode = 0;
        lineTool.x = 0;
        lineTool.y = 0;
        lineTool.h = 0;
        lineTool.w = 0;
        lineTool.initialized = 1;
    }

    // Pressing the mouse button
    if ( MouseButtonL || MouseButtonR )
    {
        if ( lineTool.mode == 0 )
        {
            lineTool.x = ( int )cMouseX;
            lineTool.y = ( int )cMouseY;
            lineTool.mode = 1;
        }
        else
        {
            double curX = ( int )cMouseX;
            double curY = ( int )cMouseY;

            if ( lineTool.w )
                FreeVec ( lineTool.buffer );

            double ox = 0, oy = 0, w = 0, h = 0;

            if ( lineTool.x > curX )
            {
                ox = curX - 1;
                w = lineTool.x - curX + 2;
            }
            else
            {
                ox = lineTool.x - 1;
                w = curX - lineTool.x + 2;
            }
            if ( lineTool.y > curY )
            {
                oy = curY - 1;
                h = lineTool.y - curY + 2;
            }
            else
            {
                oy = lineTool.y - 1;
                h = curY - lineTool.y + 2;
            }

            // Include the brushsize in the calculation
            int datawidth = ( int )( w + brushTool.width );
            int dataheight = ( int )( h + brushTool.height );
            int datasize = datawidth * dataheight * 8;
            lineTool.buffer = AllocVec ( datasize, MEMF_ANY|MEMF_CLEAR );

            // Draw line on tool buffer without feather or antialias

            int xoff = brushTool.width / 2.0;
            int yoff = brushTool.height / 2.0;

            lineTool.ox = ox - xoff;
            lineTool.oy = oy - yoff;
            lineTool.w = datawidth;
            lineTool.h = dataheight;
            lineTool.dx = curX;
            lineTool.dy = curY;

            drawLine (
                lineTool.x - ox + xoff,
                lineTool.y - oy + yoff,
                curX - ox + xoff,
                curY - oy + yoff,
                lineTool.w,
                lineTool.h,
                lineTool.buffer,
                FALSE
            );

            return TRUE;
        }
    }
    // On dropping the mouse button after press!
    // Should init the line drawing and fill the buffer!
    else
    {
        if ( lineTool.mode == 1 )
        {
            struct Affrect rect = drawLine (
                lineTool.x,
                lineTool.y,
                lineTool.dx,
                lineTool.dy,
                globalActiveCanvas->width,
                globalActiveCanvas->height,
                globalActiveCanvas->activebuffer,
                FALSE
            );
            blitAreaRect (
                rect.x, rect.y,
                rect.w, rect.h,
                globalActiveCanvas,
                _rp ( globalActiveWindow->area )
            );
            lineTool.mode = 0;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL toolRectangle ( )
{
    if ( rectangleTool.initialized != 1 )
    {
        rectangleTool.mode = 0;
        rectangleTool.x = 0;
        rectangleTool.y = 0;
        rectangleTool.h = 0;
        rectangleTool.w = 0;
        rectangleTool.initialized = 1;
    }

    // Pressing the mouse button
    if ( MouseButtonL || MouseButtonR )
    {
        if ( rectangleTool.mode == 0 )
        {
            rectangleTool.x = ( int )cMouseX;
            rectangleTool.y = ( int )cMouseY;
            rectangleTool.mode = 1;
        }
        else
        {
            double curX = ( int )cMouseX;
            double curY = ( int )cMouseY;

            if ( rectangleTool.w )
                FreeVec ( rectangleTool.buffer );

            double ox = 0, oy = 0, w = 0, h = 0;

            if ( rectangleTool.x > curX )
            {
                ox = curX - 1;
                w = rectangleTool.x - curX + 2;
            }
            else
            {
                ox = rectangleTool.x - 1;
                w = curX - rectangleTool.x + 2;
            }
            if ( rectangleTool.y > curY )
            {
                oy = curY - 1;
                h = rectangleTool.y - curY + 2;
            }
            else
            {
                oy = rectangleTool.y - 1;
                h = curY - rectangleTool.y + 2;
            }

            // Include the brushsize in the calculation
            int datawidth = ( int )( w + brushTool.width );
            int dataheight = ( int )( h + brushTool.height );
            int datasize = datawidth * dataheight * 8;
            rectangleTool.buffer = AllocVec ( datasize, MEMF_ANY|MEMF_CLEAR );

            // Draw line on tool buffer without feather or antialias

            int xoff = brushTool.width / 2.0;
            int yoff = brushTool.height / 2.0;

            rectangleTool.ox = ox - xoff;
            rectangleTool.oy = oy - yoff;
            rectangleTool.w = datawidth;
            rectangleTool.h = dataheight;
            rectangleTool.dx = curX;
            rectangleTool.dy = curY;

            double x1 = rectangleTool.x - ox + xoff;
            double y1 = rectangleTool.y - oy + yoff;
            double x2 = curX - ox + xoff;
            double y2 = curY - oy + yoff;

            // Make visible
            int oldMode = brushTool.paintmode;
            brushTool.paintmode = LUNA_PAINTMODE_NORMAL;

            if ( filledDrawing != DRAW_DRAW )
            {
                int oldw = brushTool.width, oldh = brushTool.height;
                brushTool.width = 1;
                brushTool.height = 1;
                BOOL ba = brushTool.antialias;
                brushTool.antialias = FALSE;
                makeToolBrush ( );

                int starty = y1 > y2 ? y2 : y1;
                int endy = y1 > y2 ? y1 : y2;

                int y; for ( y = starty; y <= endy; y++ )
                {
                    drawLine (
                        x1, y, x2, y,
                        rectangleTool.w, rectangleTool.h, rectangleTool.buffer, FALSE
                    );
                }
                brushTool.width = oldw;
                brushTool.height = oldh;
                brushTool.antialias = ba;
                makeToolBrush ( );
            }

            if ( filledDrawing == DRAW_DRAW || filledDrawing == DRAW_DRAWFILLED )
            {
                drawLine (
                    x1, y1, x2, y1,
                    rectangleTool.w, rectangleTool.h,
                    rectangleTool.buffer, TRUE
                );
                drawLine (
                    x2, y1, x2, y2,
                    rectangleTool.w, rectangleTool.h,
                    rectangleTool.buffer, TRUE
                );
                drawLine (
                    x2, y2, x1, y2,
                    rectangleTool.w, rectangleTool.h,
                    rectangleTool.buffer, TRUE
                );
                drawLine (
                    x1, y2, x1, y1,
                    rectangleTool.w, rectangleTool.h,
                    rectangleTool.buffer, TRUE
                );
            }

            brushTool.paintmode = oldMode;

            return TRUE;
        }
    }
    // On dropping the mouse button after press!
    // Should init the line drawing and fill the buffer!
    else
    {
        if ( rectangleTool.mode == 1 )
        {
            double x1 = rectangleTool.x;
            double y1 = rectangleTool.y;
            double x2 = rectangleTool.dx;
            double y2 = rectangleTool.dy;

            if ( filledDrawing != DRAW_DRAW )
            {
                int oldw = brushTool.width, oldh = brushTool.height;
                brushTool.width = 1;
                brushTool.height = 1;
                BOOL ba = brushTool.antialias;
                brushTool.antialias = FALSE;
                makeToolBrush ( );

                int starty = y1 > y2 ? y2 : y1;
                int endy = y1 > y2 ? y1 : y2;

                int y; for ( y = starty; y <= endy; y++ )
                {
                    drawLine (
                        x1, y, x2, y,
                        globalActiveCanvas->width, globalActiveCanvas->height,
                        globalActiveCanvas->activebuffer, FALSE
                    );
                }
                brushTool.width = oldw;
                brushTool.height = oldh;
                brushTool.antialias = ba;
                makeToolBrush ( );
            }

            if ( filledDrawing == DRAW_DRAW || filledDrawing == DRAW_DRAWFILLED )
            {
                drawLine (
                    x1, y1, x2, y1,
                    globalActiveCanvas->width, globalActiveCanvas->height,
                    globalActiveCanvas->activebuffer, TRUE
                );
                drawLine (
                    x2, y1, x2, y2,
                    globalActiveCanvas->width, globalActiveCanvas->height,
                    globalActiveCanvas->activebuffer, TRUE
                );
                drawLine (
                    x2, y2, x1, y2,
                    globalActiveCanvas->width, globalActiveCanvas->height,
                    globalActiveCanvas->activebuffer, TRUE
                );
                drawLine (
                    x1, y2, x1, y1,
                    globalActiveCanvas->width, globalActiveCanvas->height,
                    globalActiveCanvas->activebuffer, TRUE
                );
            }

            blitAreaRect (
                rectangleTool.ox, rectangleTool.oy,
                rectangleTool.w, rectangleTool.h,
                globalActiveCanvas,
                _rp ( globalActiveWindow->area )
            );

            rectangleTool.mode = 0;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL toolCircle ( )
{
    if ( circleTool.initialized != 1 )
    {
        circleTool.mode = 0;
        circleTool.x = 0;
        circleTool.y = 0;
        circleTool.h = 0;
        circleTool.w = 0;
        circleTool.bufwidth = 0;
        circleTool.bufheight = 0;
        circleTool.initialized = 1;
    }

    // Pressing the mouse button
    if ( MouseButtonL || MouseButtonR )
    {
        if ( circleTool.mode == 0 )
        {
            circleTool.x = ( int )cMouseX;
            circleTool.y = ( int )cMouseY;
            circleTool.mode = 1;
        }
        else
        {
            if ( circleTool.buffer != NULL )
                FreeVec ( circleTool.buffer );

            int cirWidth = abs ( circleTool.x - ( int )cMouseX );
            int cirHeight = abs ( circleTool.y - ( int )cMouseY );

            // Include the brushsize in the calculation
            int datawidth = ( int )( cirWidth * 2 ) + brushTool.width + 1;
            int dataheight = ( int )( cirHeight * 2 ) + brushTool.height + 1;
            int datasize = datawidth * dataheight * 8;

            circleTool.buffer = AllocVec ( datasize, MEMF_ANY|MEMF_CLEAR );

            double offx = brushTool.width / 2.0;
            double offy = brushTool.height / 2.0;

            circleTool.ox = circleTool.x - cirWidth - offx;
            circleTool.oy = circleTool.y - cirHeight - offy;
            circleTool.w = cirWidth;
            circleTool.h = cirHeight;
            circleTool.bufwidth = datawidth;
            circleTool.bufheight = dataheight;

            // Make sure preview is visible
            int oldMode = brushTool.paintmode;
            brushTool.paintmode = LUNA_PAINTMODE_NORMAL;
            drawCircle (
                cirWidth + offx,
                cirHeight + offy,
                cirWidth, cirHeight,
                datawidth, dataheight,
                circleTool.buffer
            );
            brushTool.paintmode = oldMode;
            return TRUE;
        }
    }
    // On dropping the mouse button after press!
    // Should init the line drawing and fill the buffer!
    else
    {
        if ( circleTool.mode == 1 )
        {
            struct Affrect rect = drawCircle (
                circleTool.ox + circleTool.w + ( brushTool.width / 2.0 ),
                circleTool.oy + circleTool.h + ( brushTool.height / 2.0 ),
                circleTool.w,  circleTool.h,
                globalActiveCanvas->width, globalActiveCanvas->height,
                globalActiveCanvas->activebuffer
            );

            blitAreaRect (
                rect.x, rect.y,
                rect.w, rect.h,
                globalActiveCanvas,
                _rp ( globalActiveWindow->area )
            );
            circleTool.mode = 0;
            return TRUE;
        }
    }
    return FALSE;
}

BOOL toolClipBrush ( )
{
    if ( clipbrushTool.initialized != 1 )
    {
        clipbrushTool.mode = 0;
        clipbrushTool.x = 0;
        clipbrushTool.y = 0;
        clipbrushTool.h = 0;
        clipbrushTool.w = 0;
        clipbrushTool.initialized = 1;
    }

    // Pressing the mouse button
    if ( MouseButtonL || MouseButtonR )
    {
        if ( clipbrushTool.mode == 0 )
        {
            // Yes why -1, it's how it works when getting the coord
            // has something to do with the line tool?
            clipbrushTool.x = ( int )cMouseX;
            clipbrushTool.y = ( int )cMouseY;

            // snap to bounds
            if ( clipbrushTool.x < 0 ) clipbrushTool.x = 0;
            if ( clipbrushTool.x >= globalActiveCanvas->width )
                clipbrushTool.x = globalActiveCanvas->width - 1;
            if ( clipbrushTool.y < 0 ) clipbrushTool.y = 0;
            if ( clipbrushTool.y >= globalActiveCanvas->height )
                clipbrushTool.y = globalActiveCanvas->height - 1;

            // Coords measured, go on
            clipbrushTool.mode = 1;
        }
        else
        {
            double curX = ( int )cMouseX;
            double curY = ( int )cMouseY;

            // snap to bounds
            if ( curX < 0 ) curX = 0;
            if ( curX > globalActiveCanvas->width )
                curX = globalActiveCanvas->width;
            if ( curY < 0 ) curY = 0;
            if ( curY > globalActiveCanvas->height )
                curY = globalActiveCanvas->height;

            if ( clipbrushTool.buffer != NULL )
                FreeVec ( clipbrushTool.buffer );

            double ox = 0, oy = 0, w = 0, h = 0;

            if ( clipbrushTool.x > curX )
            {
                ox = curX;
                w = clipbrushTool.x - curX;
            }
            else
            {
                ox = clipbrushTool.x;
                w = curX - clipbrushTool.x;
            }
            if ( clipbrushTool.y > curY )
            {
                oy = curY;
                h = clipbrushTool.y - curY;
            }
            else
            {
                oy = clipbrushTool.y;
                h = curY - clipbrushTool.y;
            }

            // Make sure we have w / h
            if ( !w ) w = 1;
            if ( !h ) h = 1;

            int datawidth = ( int )w;
            int dataheight = ( int )h;
            int datasize = datawidth * dataheight;

            clipbrushTool.buffer = AllocVec ( datasize * 8, MEMF_ANY|MEMF_CLEAR );

            // Draw line on tool buffer without feather or antialias
            clipbrushTool.ox = ( int )ox;
            clipbrushTool.oy = ( int )oy;
            clipbrushTool.w = ( int )w;
            clipbrushTool.h = ( int )h;
            clipbrushTool.dx = curX;
            clipbrushTool.dy = curY;

            double x1 = 0;
            double y1 = 0;
            double x2 = w - 1;
            double y2 = h - 1;


            int bw = brushTool.width;
            int bh = brushTool.height;
            int bo = brushTool.opacity;
            int bp = brushTool.power;
            int bs = brushTool.step;
            int ct = brushTool.paintmode;
            BOOL ba = brushTool.antialias;
            BOOL bf = brushTool.feather;

            // Configure brush tool to make
            // the rect drawing
            brushTool.width = 1;
            brushTool.height = 1;
            brushTool.step = 1;
            brushTool.opacity = 255;
            brushTool.power = 100;
            brushTool.antialias = FALSE;
            brushTool.feather = FALSE;
            brushTool.paintmode = LUNA_PAINTMODE_NORMAL;
            makeToolBrush ( );

            // Draw rect
            drawLine (
                x1, y1, x2, y1,
                clipbrushTool.w, clipbrushTool.h,
                clipbrushTool.buffer, TRUE
            );
            drawLine (
                x2, y1, x2, y2,
                clipbrushTool.w, clipbrushTool.h,
                clipbrushTool.buffer, TRUE
            );
            drawLine (
                x2, y2, x1, y2,
                clipbrushTool.w, clipbrushTool.h,
                clipbrushTool.buffer, TRUE
            );
            drawLine (
                x1, y2, x1, y1,
                clipbrushTool.w, clipbrushTool.h,
                clipbrushTool.buffer, TRUE
            );

            // Restore old brush settings
            brushTool.width = bw;
            brushTool.height = bh;
            brushTool.step = bs;
            brushTool.opacity = bo;
            brushTool.power = bp;
            brushTool.antialias = ba;
            brushTool.feather = bf;
            brushTool.paintmode = ct;
            makeToolBrush ( );
            return TRUE;
        }
    }
    // On dropping the mouse button after press!
    // This copies an area to the brush tool!
    else
    {
        if ( clipbrushTool.mode == 1 )
        {
            if ( clipbrushTool.w > 0 && clipbrushTool.h > 0 )
            {
                // Free brush tool buffer if not freed
                if ( brushTool.buffer != NULL )
                    FreeVec ( brushTool.buffer );

                // Allocate new space
                brushTool.buffer = AllocVec ( clipbrushTool.w * clipbrushTool.h * 8, MEMF_ANY );
                brushTool.width = clipbrushTool.w;
                brushTool.height = clipbrushTool.h;

                // Copy from current active layer to brushtool
                int y = 0; for ( ; y < brushTool.height; y++ )
                {
                    int ybmul = brushTool.width * y;
                    int yamul = globalActiveCanvas->width * ( y + ( int )clipbrushTool.oy );
                    int x = 0; for ( ; x < brushTool.width; x++ )
                    {
                        int offset = yamul + ( ( int )clipbrushTool.ox + x );
                        brushTool.buffer[ ybmul + x ] =
                            globalActiveCanvas->activebuffer[ offset ];
                    }
                }

                blitAreaRect (
                    clipbrushTool.ox, clipbrushTool.oy,
                    clipbrushTool.w, clipbrushTool.h,
                    globalActiveCanvas,
                    _rp ( globalActiveWindow->area )
                );

            }

            clipbrushTool.mode = 0;

            // Set draw tool now
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Draw, FALSE );
            DoMethod ( tbxAreaPreview, MUIM_Draw );

            return TRUE;
        }
    }
    return FALSE;
}

BOOL toolColorPicker ( )
{
    if ( MouseButtonL || MouseButtonR )
    {
        unsigned long long int color = globalActiveCanvas->activebuffer[
            ( int )( cMouseY ) * globalActiveCanvas->width + ( int )( cMouseX )
        ];

        struct rgba64 result = *( struct rgba64 *)&( color );

        if ( globalColorMode == LUNA_COLORMODE_SNAP )
            result = snapToPalette ( result, TRUE );

        globalColor = canvasToWindowPixel ( *( unsigned long long int *)&result );

        makeToolBrush ( );
    }
    return FALSE;
}

void flipBrushVert ( )
{
    unsigned long long int *tmpBuf = AllocVec ( brushTool.width * brushTool.height * 8, MEMF_ANY );
    int y = 0; for ( ; y < brushTool.height; y++ )
    {
        int x = 0; for ( ; x < brushTool.width; x++ )
        {
            int spos = y * brushTool.width + x;
            int dpos = ( brushTool.height - y - 1 ) * brushTool.width + x;
            tmpBuf[ dpos ] = brushTool.buffer[ spos ];
        }
    }
    FreeVec ( brushTool.buffer );
    brushTool.buffer = tmpBuf;
}

void flipBrushHoriz ( )
{
    unsigned long long int *tmpBuf = AllocVec ( brushTool.width * brushTool.height * 8, MEMF_ANY );
    int y = 0; for ( ; y < brushTool.height; y++ )
    {
        int x = 0; for ( ; x < brushTool.width; x++ )
        {
            int spos = y * brushTool.width + x;
            int dpos = y * brushTool.width + ( brushTool.width - x - 1 );
            tmpBuf[ dpos ] = brushTool.buffer[ spos ];
        }
    }
    FreeVec ( brushTool.buffer );
    brushTool.buffer = tmpBuf;
}

void rotateBrush90 ( )
{
    int size = brushTool.height * brushTool.width;
    unsigned long long int *tmpBuf = AllocVec ( size * 8, MEMF_ANY );
    int i = 0, x = 0, y = 0; for ( ; i < size; i++ )
    {
        x = i % brushTool.width;
        y = i / brushTool.width;
        tmpBuf[ x * brushTool.height + ( brushTool.height - 1 - y ) ] = brushTool.buffer[ i ];
    }
    FreeVec ( brushTool.buffer );
    brushTool.buffer = tmpBuf;
    int height = brushTool.height;
    brushTool.height = brushTool.width;
    brushTool.width = height;
}
