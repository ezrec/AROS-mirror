/*
 * gfx_prim
 * 
 *   Basic graphics primitives.
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <stdio.h>
#include "jsui.h"


void
__primitive_pixel(
    IPAGE * page,
    PEN * pen,
    int x, 
    int y
)
{
    if (page->ncolors <= 0 && !page->timage) return;
    if (page->ncolors > 0  && !page->pimage) return;

    if (y >= page->h || x >= page->w) return;
    if (y < 0 || x < 0) return;

    if (page->ncolors > 0)
    {
	page->pimage[ (y*page->w)+x ] = pen->Icolor;
    } else {
	page->timage[ (y*page->w)+x ].r = pen->RGBcolor.r;
	page->timage[ (y*page->w)+x ].g = pen->RGBcolor.g;
	page->timage[ (y*page->w)+x ].b = pen->RGBcolor.b;
    }
}


void
primitive_pixel(
    IPAGE * page,
    PEN * pen,
    int x, 
    int y
)
{
//    printf("pixel at %d %d\n");
    __primitive_pixel(page,pen,x,y);

    Page_DR_Dirtify(page, x, y, x, y);
}

void
primitive_pixel_dotted(
    IPAGE * page,
    PEN * pen,
    int x, 
    int y
)
{
    if ((x&2) == (y&2))
    {
	__primitive_pixel(page,pen,x,y);

	Page_DR_Dirtify(page, x, y, x, y);
    }
}


int
primitive_pixel_get(
    IPAGE * page,
    int x, 
    int y
)
{
    if (!page) return 0;
    if (y >= page->h || x >= page->w) return 0;

    if (page->ncolors > 0)
    {
        return (page->pimage[ (y*page->w)+x ]);
    } else {
	return (
	    (page->timage[ (y*page->w)+x ].r <<16)
		&
	    (page->timage[ (y*page->w)+x ].g <<8)
		&
	    page->timage[ (y*page->w)+x ].b   );
    }

}


////////////////////////////////////////////////////////////////////////////////


void
__primitive_line_horizontal(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int x2,
    int y
)
{
    if (!page || !func) return;
    for (x1 = x1 ; x1 <= x2 ; x1++)
    {
	func(page, pen, x1, y);
    }
}


void
primitive_line_horizontal(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int x2,
    int y
)
{
    __primitive_line_horizontal(page,pen,func,x1,x2,y);
    Page_DR_Dirtify(page, x1, y, x2, y);
}



void
__primitive_line_vertical(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x,
    int y1,
    int y2
)
{
    if (!page || !func) return;
    for (y1 = y1 ; y1 <= y2 ; y1++)
    {
	func(page, pen, x, y1);
    }
}


void
primitive_line_vertical(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x,
    int y1,
    int y2
)
{
    __primitive_line_vertical(page,pen,func,x,y1,y2);
    Page_DR_Dirtify(page, x, y1, x, y2);
}




//  draw a diagonal line that's more horizontal than vertical
void
primitive_hdiagline(
	IPAGE * page, 
	PEN * pen,
	primitive_callback func,
	int x1, int y1, int x2, int y2,
	int dx, int dy
)
{
    int avdy = ABS(dy);
    int d = (avdy<<1) - dx;
    int dy_x_2 = avdy<<1;
    int dy_m_dx_x_2= (avdy-dx)<<1;

    if (dy > 0){  // this creates more code but is faster
        while (x1 <= x2){
            func(page, pen, x1, y1);
            func(page, pen, x2, y2);
            if (d < 0){
                d += dy_x_2;
            } else {
                d += dy_m_dx_x_2;
                y1++; y2--;
            }
            x1++; x2--;
        }
    } else {
        while (x1 <= x2){
            func(page, pen, x1, y1);
            func(page, pen, x2, y2);
            if (d < 0){
                d += dy_x_2;
            } else {
                d += dy_m_dx_x_2;
                y1--; y2++;
            }
            x1++; x2--;
        }
    }
    Page_DR_Dirtify(page, x1, y1, x2, y2);
}

//  draw a diagonal line that's more vertical than horizontal
void 
primitive_vdiagline(
	IPAGE * page, 
	PEN * pen,
	primitive_callback func,
        int x1, int y1, int x2, int y2,
        int dx, int dy
)
{
    int avdx = ABS(dx);
    int d = (avdx<<1) - dy;
    int dx_x_2 = avdx<<1;
    int dx_m_dy_x_2= (avdx-dy)<<1;

    if (dx > 0){  // this creates more code but is faster
        while (y1 <= y2){
            func(page, pen, x1, y1);
            func(page, pen, x2, y2);
            if (d < 0){
                d += dx_x_2;
            } else {
                d += dx_m_dy_x_2;
                x1++; x2--;
            }
            y1++; y2--;
        }
    } else {
        while (y1 <= y2){
            func(page, pen, x1, y1);
            func(page, pen, x2, y2);
            if (d < 0){
                d += dx_x_2;
            } else {
                d += dx_m_dy_x_2;
                x1--; x2++;
            }
            y1++; y2--;
        }
    }
    Page_DR_Dirtify(page, x1, y1, x2, y2);
}



void
primitive_line(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int y1,
    int x2,
    int y2
)
{
    int dx;
    int dy;

    if (!page || !pen) return;
    if (!func) return;
    if (page->ncolors <= 0 && !page->timage) return;
    if (page->ncolors > 0  && !page->pimage) return;

    if (x1 == x2){
        // a vertical line
	primitive_line_vertical(page, pen, func, x1, MIN(y1,y2), MAX(y1,y2));
        return;
    }

    if (y1 == y2){
        // a horizontal line
	primitive_line_horizontal(page, pen, func, MIN(x1,x2), MAX(x1,x2), y1);
        return;
    }

/*
**  1) find out which sets of angles to use
**     (set 1 or set 2)
**     compare the absolute values of the slopes
**
**  2) figure out which quadrant it's in
**     (Quadrant A, B, C, or D)
**     compare dx and dy with 0 for this result
**
**    \        |        /
**      \  2B  |  2A  /
**        \    |    /                       set 1:  |dx| > |dy|
**          \  |  /   1A
**      1B    \|/               dy < 0      set 2:  |dx| < |dy|
**    ---------*---------       ------
**      1C    /|\     1D        dy > 0
**          /  |  \
**        /    |    \
**      /  2C  | 2D   \
**    /        |        \
**
**     dx < 0  |  dx > 0
*/
    dx = x2-x1;
    dy = y2-y1;



    if ( ABS(dx) > ABS(dy) ){
        // it's an angle (315,45) or (135,225)   slow angle (Set 1)
        if (dx > 0){ // 1A or 1D
            primitive_hdiagline(page, pen, func, x1, y1,  x2, y2,  dx, dy);
            return;
        } else { // 1B or 1C
            //dx *= -1; dy *= -1;
            primitive_hdiagline(page, pen, func, x2, y2,  x1, y1,  -dx, -dy);
            return;
        }
    }
    else
    {
        // it's an angle (45,135) or (225,315)   steep angle (set 2)
        if (dy > 0){ // 2A or 2B (2B or not 2B)
            primitive_vdiagline(page, pen, func, x1, y1,  x2, y2,  dx, dy);
            return;
        } else { // 2C or 2D
            //dx *= -1; dy *= -1;
            primitive_vdiagline(page, pen, func, x2, y2,  x1, y1,  -dx, -dy);
            return;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////

void
primitive_rectangle_hollow(
    IPAGE * page,
    PEN * pen,
    primitive_callback func,
    int x1,
    int y1,
    int x2,
    int y2
)
{
    if (!page || !pen || !func) return;

    __primitive_line_horizontal(page, pen, func, MIN(x1,x2), MAX(x1,x2), y1);
    __primitive_line_vertical  (page, pen, func, x2, MIN(y1,y2), MAX(y1,y2));
    __primitive_line_horizontal(page, pen, func, MIN(x1,x2), MAX(x1,x2), y2);
    __primitive_line_vertical  (page, pen, func, x1, MIN(y1,y2), MAX(y1,y2));
    Page_DR_Dirtify(page, x1, y1, x2, y2);
}


void
primitive_rectangle_filled(
    IPAGE * page,
    PEN * pen,
    int x1,
    int y1,
    int x2,
    int y2
)
{
    int yl, yg;
    int xl, xg;
    if (!page || !pen) return;

    yl = MIN(y1, y2);
    yg = MAX(y1, y2);

    xl = MIN(x1, x2);
    xg = MAX(x1, x2);

    for (yl = yl ; yl <= yg ; yl++)
    {
	__primitive_line_horizontal(page, pen, __primitive_pixel, xl, xg, yl);
    }
    Page_DR_Dirtify(page, x1, y1, x2, y2);
    
}


////////////////////////////////////////////////////////////////////////////////


void
primitive_rectangle_fourcolor(
        IPAGE * dest,
        primitive_callback func,
        int x1, int y1, int x2, int y2,
        int fillcolor,
        int c1, int c2, int c3, int c4
)
{
    PEN * newpen = Pen_Create();
    if (!dest) return;
    if (!newpen) return;

    if (fillcolor >= 0)
    {
        newpen->Icolor = fillcolor;
        primitive_rectangle_filled(dest, newpen, x1, y1, x2, y2);
    }

    newpen->Icolor = c1;
    primitive_line_horizontal(dest, newpen, func, x1, x2, y1);

    newpen->Icolor = c2;
    primitive_line_vertical(dest, newpen, func, x2, y1, y2);

    newpen->Icolor = c3;
    primitive_line_horizontal(dest, newpen, func, x1, x2, y2);

    newpen->Icolor = c4;
    primitive_line_vertical(dest, newpen, func, x1, y1, y2);

    Pen_Destroy(newpen);
}

////////////////////////////////////////////////////////////////////////////////

void
primitive_rectangle_checkerboard(
        IPAGE * dest,
        int x1, int y1, int x2, int y2,
        int fillcolor
)
{
    int x,y;
    PEN * newpen = Pen_Create();
    if (!dest) return;
    if (!newpen) return;

    newpen->Icolor = fillcolor;

    for (x = x1 ; x < x2 ; x++)
	for (y = y1 ; y < y2 ; y++)
	{
	    if ( (x + y) & 1)
		__primitive_pixel(dest,newpen,x,y);
	}

    Pen_Destroy(newpen);
}





////////////////////////////////////////////////////////////////////////////////


void 
__circlepoints(
	IPAGE * page,
	PEN * pen,
	primitive_callback func,
	int x,
	int y,
	int xoffs,
	int yoffs
)
{
    func(page, pen, xoffs+x, yoffs+y);
    func(page, pen, xoffs+y, yoffs+x);
    func(page, pen, xoffs+y, yoffs-x);
    func(page, pen, xoffs+x, yoffs-y);

    func(page, pen, xoffs-x, yoffs-y);
    func(page, pen, xoffs-y, yoffs-x);
    func(page, pen, xoffs-y, yoffs+x);
    func(page, pen, xoffs-x, yoffs+y);
}


void
primitive_circle_hollow(
	IPAGE * page,
	PEN * pen,
	primitive_callback func,
	int xoffs,
	int yoffs,
	double radius
)
{
    int x,y;
    double d;

    if (!page || !pen || !func) return;

    x=0;
    y=(int)radius;
    d=5/4-radius;

    __circlepoints(page, pen, func, x, y, xoffs, yoffs);

    while (y>x)
    {
	if (d<0)
	{
	    d = d + 2 * x + 3;
	    x++;
	} else {
	    d = d + 2 * (x - y)+5;
	    x++;
	    y--;
	}
	__circlepoints(page, pen, func, x, y, xoffs, yoffs);
    }
}


#ifdef WE_WANT_IT_TO_CRASH_LOTS
////////////////////////////////////////////////////////////////////////////////
// flood fill



// fvdfh version
// for those of you who aren't me,  "fvdfh" stands for:
//   Foley, Van Dam, Feiner, and Hughes.  The authors of;
//    "Computer Graphics: Practice and Principles" 
//    AKA, the graphics programming bible.
void
__floodfill4(
        IPAGE * page,
        PEN * pen,
        int x,
        int y,
        int oldvalue
)
{
    if ( (x >= page->w) || (y >= page->h) ) return;
    if ( (x < 0) || (y < 0) ) return;

    if (primitive_pixel_get(page, x,y) == oldvalue)
    {
        __primitive_pixel(page, pen, x, y);
        __floodfill4(page, pen, x, y-1, oldvalue);
        __floodfill4(page, pen, x, y+1, oldvalue);
        __floodfill4(page, pen, x-1, y, oldvalue);
        __floodfill4(page, pen, x+1, y, oldvalue);
    }
}


void
primitive_floodfill(
	IPAGE * page,
	PEN * pen,
	int x,
	int y
)
{
    int current_pixel;

    if (!page || !pen) return;

    if (x > page->w || x < 0) return;
    if (y > page->h || y < 0) return;

    current_pixel = primitive_pixel_get(page, x, y);

    __floodfill4( page, pen, x, y, current_pixel );
    Page_DR_Dirtify(page, 0, 0, page->w-1, page->h-1);
}
#endif
