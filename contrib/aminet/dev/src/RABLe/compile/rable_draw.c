#include <stdlib.h>  /* exit() function defined here */
#include <stdio.h>   /* puts(), sprintf(), printf(), etc. */
#include <math.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>

#include <aros/oldprograms.h>
#include <exec/types.h>

#include "rable_draw.h"
#include "rable.h"
#include "rable_gadgets.h"

/* Globals for drawing module */

extern int curr_pen;
SHORT polygon[] = { 30,30, 150,30, 150,150, 30,150, 30,30 };
BOOL	draw_flag = FALSE;
WORD	xstart, ystart, xlast, ylast;
	

/* All of the drawing routines will go here */

void draw_circle(struct Window *window1, int type, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;

WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				DrawEllipse(rp, xstart, ystart, abs(xstart - xlast), abs(xstart - xlast));
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				if (type == filled)
				{
					AreaEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(xstart - xcurrent));
					AreaEnd(rp);
				}
				else
					DrawEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(xstart - xcurrent));
                                RefreshWindowFrame(window1);				
				draw_flag = FALSE;

				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
				ystart = ycurrent;
         	SetDrMd(rp, COMPLEMENT);
				WritePixel(rp, xstart, ystart);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				DrawEllipse(rp, xstart, ystart, abs(xstart - xlast), abs(xstart - xlast));
				DrawEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(xstart - xcurrent));
				xlast = xcurrent;
			}
		break;

}
}

void draw_oval(struct Window *window1, int type, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				DrawEllipse(rp, xstart, ystart, abs(xstart - xlast), abs(ystart - ylast));
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				if(type == filled)
				{
					AreaEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(ystart - ycurrent));
					AreaEnd(rp);
				}						
				else
					DrawEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(ystart - ycurrent));
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
				ylast = ystart = ycurrent;
         	SetDrMd(rp, COMPLEMENT);
         	WritePixel(rp, xstart, ystart);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				DrawEllipse(rp, xstart, ystart, abs(xstart - xlast), abs(ystart - ylast));
				DrawEllipse(rp, xstart, ystart, abs(xstart - xcurrent), abs(ystart - ycurrent));
				xlast = xcurrent;
				ylast = ycurrent;
			}
		break;

}
}


void draw_line(struct Window *window1, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				Move(rp, xstart, ystart);
				Draw(rp, xlast, ylast);
				SetDrMd(rp, JAM2);
				Move(rp, xstart, ystart);
				SetAPen(window1->RPort, (curr_pen-1));
				Draw(rp, xcurrent, ycurrent);
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
         	ylast = ystart = ycurrent;
				SetDrMd(rp, COMPLEMENT);
				WritePixel(rp, xstart,ystart);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				Move(rp, xstart, ystart);
				Draw(rp, xlast, ylast);
				Move(rp, xstart, ystart);
				Draw(rp, xcurrent, ycurrent);
				xlast = xcurrent;
				ylast = ycurrent;
			}
		break;

}
}


void draw_free(struct Window *window1, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				WritePixel(rp, xcurrent, ycurrent);
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xcurrent;
         	ylast = ycurrent;
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				WritePixel(rp, xlast,ylast);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				Move(rp, xlast, ylast);
				Draw(rp, xcurrent, ycurrent);
				xlast = xcurrent;
				ylast = ycurrent;
				//WritePixel(rp, xcurrent, ycurrent);
				
			}
		break;

}
}


void draw_square(struct Window *window1, int type, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				make_square(rp, xlast, ylast);
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				
				if (type == normal)
					make_square(rp, xcurrent, ycurrent);
				else
					make_fsquare(rp, xcurrent, ycurrent);
					
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
         	ylast = ystart = ycurrent;
				SetDrMd(rp, COMPLEMENT);
				make_square(rp, xcurrent, ycurrent);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				make_square(rp, xlast, ylast);
				make_square(rp, xcurrent, ycurrent);
				xlast = xcurrent;
				ylast = ycurrent;
			}
		break;

}
}


void draw_triangle(struct Window *window1, int type, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				make_triangle(rp, xlast, ylast);
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				
				if (type == normal)
					make_triangle(rp, xcurrent, ycurrent);
				else
					make_ftriangle(rp, xcurrent, ycurrent);
					
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
         	ylast = ystart = ycurrent;
				SetDrMd(rp, COMPLEMENT);
				make_triangle(rp, xcurrent, ycurrent);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				make_triangle(rp, xlast, ylast);
				make_triangle(rp, xcurrent, ycurrent);
				xlast = xcurrent;
				ylast = ycurrent;
			}
		break;

}
}


void draw_rectangle(struct Window *window1, int type, struct IntuiMessage *message)
{
struct RastPort *rp = window1->RPort;
WORD xcurrent = message->MouseX, ycurrent = message->MouseY;

switch(message->Class)
{
	case IDCMP_MOUSEBUTTONS:
		switch(message->Code)
		{
			case SELECTUP:
				make_rectangle(rp, xlast, ylast);
				SetDrMd(rp, JAM2);
				SetAPen(window1->RPort, (curr_pen-1));
				
				if (type == normal)
					make_rectangle(rp, xcurrent, ycurrent);
				else
					make_frectangle(rp, xcurrent, ycurrent);
					
				draw_flag = FALSE;
                                RefreshWindowFrame(window1);
				break;
				
			case SELECTDOWN:
				xlast = xstart = xcurrent;
         	ylast = ystart = ycurrent;
				SetDrMd(rp, COMPLEMENT);
				make_rectangle(rp, xcurrent, ycurrent);
				draw_flag = TRUE;
				break;
		}

	case IDCMP_MOUSEMOVE:
			if(draw_flag == TRUE)
			{
				make_rectangle(rp, xlast, ylast);
				make_rectangle(rp, xcurrent, ycurrent);
				xlast = xcurrent;
				ylast = ycurrent;
			}
		break;

}
}



void make_triangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{

double x0, y0, x1, y1, x2, y2, theta0, theta, r0;	// These are used for polar calculations
WORD  X1, Y1, X2, Y2;	// These will be used for the actual rectangular coordinates
x0 = (double)xcurrent - (double)xstart, y0 = (double)ystart - (double)ycurrent;

theta = 2.09439;  // Angle in radians for adjustment = 2pi/3

r0 = sqrt((x0 * x0 + y0 * y0));	// Calculate initial vector
theta0 = atan((y0 / x0));

x1 = r0 * cos((theta0 + theta));	// Calculate second vector
y1 = r0 * sin((theta0 + theta));

x2 = r0 * cos((theta0 - theta));	// Calculate third vector
y2 = r0 * sin((theta0 - theta));


if (x0 >= 0)
{
	
	X1 = xstart + (WORD)x1;
	X2 = xstart + (WORD)x2;
	
	Y1 = ystart - (WORD)y1;
	Y2 = ystart - (WORD)y2;

}
else
{
		
	X1 = xstart - (WORD)x1;
	X2 = xstart - (WORD)x2;
	
	Y1 = ystart + (WORD)y1;
	Y2 = ystart + (WORD)y2;

}

Move(rp, xcurrent, ycurrent);
Draw(rp, X1, Y1);
Draw(rp, X2, Y2);
Draw(rp, xcurrent, ycurrent);

}

void make_ftriangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{

double x0, y0, x1, y1, x2, y2, theta0, theta, r0;	// These are used for polar calculations
WORD  X1, Y1, X2, Y2;	// These will be used for the actual rectangular coordinates
x0 = (double)xcurrent - (double)xstart, y0 = (double)ystart - (double)ycurrent;

theta = 2.09439;  // Angle in radians for adjustment = 2pi/3

r0 = sqrt((x0 * x0 + y0 * y0));	// Calculate initial vector
theta0 = atan((y0 / x0));

x1 = r0 * cos((theta0 + theta));	// Calculate second vector
y1 = r0 * sin((theta0 + theta));

x2 = r0 * cos((theta0 - theta));	// Calculate third vector
y2 = r0 * sin((theta0 - theta));


if (x0 >= 0)
{
	
	X1 = xstart + (WORD)x1;
	X2 = xstart + (WORD)x2;
	
	Y1 = ystart - (WORD)y1;
	Y2 = ystart - (WORD)y2;

}
else
{
		
	X1 = xstart - (WORD)x1;
	X2 = xstart - (WORD)x2;
	
	Y1 = ystart + (WORD)y1;
	Y2 = ystart + (WORD)y2;

}

AreaMove(rp, xcurrent, ycurrent);
AreaDraw(rp, X1, Y1);
AreaDraw(rp, X2, Y2);
AreaDraw(rp, xcurrent, ycurrent);
AreaEnd(rp);
}

void make_rectangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{

Move(rp, xstart, ystart);
Draw(rp, xcurrent, ystart);
Draw(rp, xcurrent, ycurrent);
Draw(rp, xstart, ycurrent); 
Draw(rp, xstart, ystart);

}

void make_frectangle(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{
AreaMove(rp, xstart, ystart);
AreaDraw(rp, xcurrent, ystart);
AreaDraw(rp, xcurrent, ycurrent);
AreaDraw(rp, xstart, ycurrent); 
AreaDraw(rp, xstart, ystart);
AreaEnd(rp);	
}

void make_square(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{
WORD xdelta = xcurrent - xstart, ydelta = ycurrent - ystart;

Move(rp, xcurrent, ycurrent);

if (ydelta == 0)
{
	Draw(rp, xstart, (ystart - xdelta));
	Draw(rp, (xstart - xdelta), ystart);
	Draw(rp, xstart, (ystart + xdelta));
}
else
{
	Move(rp, xcurrent, ycurrent);
	Draw(rp, (xstart + ydelta), (ystart - xdelta));
	Draw(rp, (xstart - xdelta), (ystart - ydelta));
	Draw(rp, (xstart - ydelta), (ystart + xdelta));
}

Draw(rp, xcurrent, ycurrent);

}

void make_fsquare(struct RastPort *rp, WORD xcurrent, WORD ycurrent)
{
WORD xdelta = xcurrent - xstart, ydelta = ycurrent - ystart;

AreaMove(rp, xcurrent, ycurrent);

if (ydelta == 0)
{
	
	AreaDraw(rp, xstart, (ystart - xdelta));
	AreaDraw(rp, (xstart - xdelta), ystart);
	AreaDraw(rp, xstart, (ystart + xdelta));
}
else
{
	AreaDraw(rp, (xstart + ydelta), (ystart - xdelta));
	AreaDraw(rp, (xstart - xdelta), (ystart - ydelta));
	AreaDraw(rp, (xstart - ydelta), (ystart + xdelta));
}

AreaDraw(rp, xcurrent, ycurrent);

/* Draw the shape */
AreaEnd(rp);

}

