/*	drawing stuff for rubik.c 	M.J.Round 3rd Feb 90	*/

#include "header.c"

extern struct GfxBase * GfxBase;
int twistlist[21];
int x1,y1,x2,y2,x3,y3;

char tr [120] =			/*	transformations for 90 degree side rotates	*/
	{
	0,2,8,6,
	1,5,7,3,
	18,45,29,42,
	19,48,28,39,
	20,51,27,36,
	
	9,11,17,15,
	10,14,16,12,
	24,47,35,44,
	25,50,34,41,
	26,53,33,38,
	
	18,20,26,24,
	19,23,25,21,
	0,45,11,38,
	1,46,10,37,
	2,47,9,36,
	
	27,29,35,33,
	28,32,34,30,
	6,51,17,44,
	7,52,16,43,
	8,53,15,42,
	
	36,38,44,42,
	37,41,43,39,
	18,9,33,6,
	21,12,30,3,
	24,15,27,0,
	
	45,47,53,51,
	46,50,52,48,
	20,11,35,8,
	23,14,32,5,
	26,17,29,2
	};

extern struct points point;	/*	holds posn.s of all points 	*/
extern struct points start;
extern struct one_square square[56];
extern struct Window *wnd1;
extern struct Window *wnd2;
extern struct Window *window;
extern struct Screen *scr1;
extern struct Screen *scr2;
extern struct Screen *screen;

extern int viewdist;
extern short xcent,ycent;
extern short angle(short, short, unsigned short);
extern unsigned short hypoten (short, short);
extern unsigned short scaling;

extern void roll (short);
extern void yaw (short);
extern void pitch (short);
extern void fb (short, short);
extern void bt (short, short);
extern void lr (short, short);

void twist (short ident, short amount)
		/*	ident = 0 to 5 for front,back,(bottom),(top),left,right	*/
		/*	amount = -1, +1 or +2	(90 degree twists)				*/
	{
	short c;
	unsigned short i,j,k;

	if (amount > 0)
		{
		for (i = 0; i<amount; i++)
			{
			for (j = ident * 20; j < ident * 20 + 20; j += 4)
				{
				c = (square[tr[j]]).colour;
				for (k = j; k < (j + 3); k++)
					(square[tr[k]]).colour = (square[tr[k+1]]).colour;
				(square[tr[j+3]]).colour = c;
				}
			}
		}
	else
		{
		for (i = 0; i < (-amount); i++)
			{
			for (j = ident * 20; j < ident * 20 + 20; j += 4)
				{
				c = (square[tr[j+3]]).colour;
				for (k = j + 3; k > j; k--)
					(square[tr[k]]).colour = (square[tr[k-1]]).colour;
				(square[tr[j]]).colour = c;
				}
			}
		}
	}
	
/*	return true if a square is clockwise else false */
int clockwise(register int i)
	{
	register int d;
	short x,y,ang1,ang2;
	unsigned short r;

	d=(viewdist+(square[i].corner[0]->z)) << scaling;
	x1 = xcent+(viewdist*square[i].corner[0]->x)/d;
	y1 = ycent+(viewdist*square[i].corner[0]->y)/d;

	d=(viewdist+(square[i].corner[1]->z)) << scaling;
	x2 = xcent+(viewdist*square[i].corner[1]->x)/d;
	y2 = ycent+(viewdist*square[i].corner[1]->y)/d;

	d=(viewdist+(square[i].corner[2]->z)) << scaling;
	x3 = xcent+(viewdist*square[i].corner[2]->x)/d;
	y3 = ycent+(viewdist*square[i].corner[2]->y)/d;

	x = x2-x1;
	y = y2-y1;
	
	ang1 = ang2 = 0;
	
	if (x || y)
		ang1 = angle(x, y, (r = hypoten (x, y)));
	
	x = x3-x2;
	y = y3-y2;

	if (x || y)
		ang2 = angle(x, y, (r = hypoten (x, y)));
	
	d = ang1-ang2;
	
	if (d > 180)
		d -= 360;
	
	if (d < -179)
		d += 360;

	return (d > 0);
	}

void draw_square(register int i)
	{
	register int d;
	SetAPen (window->RPort,square[i].colour);
	AreaMove (window->RPort,x1,y1);
	AreaDraw (window->RPort,x2,y2);
	AreaDraw (window->RPort,x3,y3);
	d=(viewdist+(square[i].corner[3]->z)) << scaling;
	AreaDraw
		(
		window->RPort,
		xcent+(viewdist*square[i].corner[3]->x)/d,
		ycent+(viewdist*square[i].corner[3]->y)/d
		);
	AreaEnd (window->RPort);
	}

/*	this routine renders the cube with roll, yaw and pitch alpha, beta, gamma
	if twists is non-zero, <layer> is rotated delta degrees at a time, through
	<twists> 90 degree twists.
*/
void showcube
			(
			short alpha, 
			short beta, 
			short gamma, 
			short layer,
			short twists,
			short delta
			)
	{
	int i,j;
	short theta = 0;
	
	do
		{
		point = start;	/*	this copies the 'zeroed' cube to the workspace */

		if (twists)
			{
			theta += (twists > 0) ? -delta : delta;

			if ((theta <= -90) || (theta >= 90))
				{
				theta = (twists > 0) ? 1 : -1;
				twist (layer,theta);
				twists -= theta;
				theta = 0;
				}
			else
				{
				switch (layer)
					{
					case 0:
					case 1:
						fb(layer,theta);
						break;
					case 2:
					case 3:
						bt((short)(layer-2),theta);
						break;
					default:
						lr((short)(layer-4),theta);
						break;
					}
				}
			}

		if (alpha)
			roll (alpha);
		if (beta)
			yaw (beta);
		if (gamma)
			pitch (gamma);
	
		SetAPen(window->RPort,7);
		RectFill(window->RPort,2,10,190,198);
		
		if (theta == 0)
			{
			for (i=0; i<54; i++)
				if (clockwise(i))
					draw_square(i);
				else if ((i % 3) == 0)
					i += 2;
			}
		else if (clockwise(54))
			{
			draw_square(54);

			for (i=0; i<21; i++)
				if (clockwise(twistlist[i]))
					draw_square(twistlist[i]);
				else if ((i % 3) == 0)
					i += 2;

			j = 0;
			
			for (i=0; i<54; i++)
				{
				while ((j < 21) && ((twistlist[j]) < i))
					j++;
				
				if ((j > 20) || (i != twistlist[j]))
					{
					if (clockwise(i))
						draw_square(i);
					else if ((i % 3) == 0)
						i += 2;
					}
				}
			}
		else
			{
			if (clockwise(55))		/*	need this to get corners	*/
				draw_square(55);
			
			j = 0;
			
			for (i=0; i<54; i++)
				{
				while ((j < 21) && ((twistlist[j]) < i))
					j++;
				
				if ((j > 20) || (i != twistlist[j]))
					{
					if (clockwise(i))
						draw_square(i);
					else if ((i % 3) == 0)
						i += 2;
					}
				}
			
			for (i=0; i<21; i++)
				if (clockwise(twistlist[i]))
					draw_square(twistlist[i]);
				else if ((i % 3) == 0)
					i += 2;
			}

		ScreenToFront(screen);
		
		if (screen == scr1)
			{
			screen = scr2;
			window = wnd2;
			}
		else
			{
			screen = scr1;
			window = wnd1;
			}
		} while (twists);
	}


int whatsquare(int x, int y)	/*	returns square number, or -1	*/
	{
	struct Window *viswindow;
	int colour,i;
	
	if (y<12 || x>190)
		return(-1);
	
	if (window == wnd1)
		viswindow = wnd2;
	else
		viswindow = wnd1;
	
	for (i = 0; i < 4 && (colour = ReadPixel(viswindow->RPort,x,y)) == 0; i++)
		{
		switch (i)
			{
			case 0:
				x += 2;
				break;
			case 1:
				y += 2;
				break;
			case 3:
				x -= 2;
				break;
			}
		}		/*	find a nearby square if they click on the lines	*/
	
	if (colour <= 0 || colour == 7)
		return (-1);
		
	ClipBlit
			(
			viswindow->RPort,2,10,				/*	source posn		*/
			window->RPort,2,10,			/*	dest posn		*/
			188,188,							/*	size			*/
			0xc0								/*	direct copy		*/
			);
	
	SetAPen(window->RPort,7);
	Flood (window->RPort,1,x,y);
	
	for (i=0; i<54; i++)
		if (clockwise(i))
			{
			draw_square(i);
			if (ReadPixel(window->RPort,x,y) != 7)
				return(i);
	
			}
	
	return (-1);
	}

int paints(int x, int y, int pen)	/*	returns square number, or -1	*/
	{
	struct Window *viswindow;
	int colour,i;
	
	if (y<12 || x>190)
		return(-1);

	if (window == wnd1)
		viswindow = wnd2;
	else
		viswindow = wnd1;

	for (i = 0; i < 4 && (colour = ReadPixel(viswindow->RPort,x,y)) == 0; i++)
		{
		switch (i)
			{
			case 0:
				x += 2;
				break;
			case 1:
				y += 2;
				break;
			case 3:
				x -= 2;
				break;
			}
		}		/*	find a nearby square if they click on the lines	*/
	
	if (colour <= 0 || colour == 7)
		return (-1);

	SetAPen(viswindow->RPort,pen);
	Flood (viswindow->RPort,1,x,y);
	
	return (whatsquare(x,y));
	}

void findangles (short *alpha, short *beta, short *gamma)
	{
	short x,y,z;
	
	y = ((point.xyz [0] [0] [5]).y + (point.xyz [5] [5] [5]).y)/2;
	z = ((point.xyz [0] [0] [5]).z + (point.xyz [5] [5] [5]).z)/2;

	pitch ((short) -(*gamma = -angle (z,y,hypoten (z,y))));
	
	x = ((point.xyz [0] [0] [5]).x + (point.xyz [5] [5] [5]).x)/2;
	z = ((point.xyz [0] [0] [5]).z + (point.xyz [5] [5] [5]).z)/2;
	
	yaw ((short) -(*beta = -angle (z,x,hypoten (z,x))));
	
	x = ((point.xyz [0] [5] [0]).x + (point.xyz [5] [5] [5]).x)/2;
	y = ((point.xyz [0] [5] [0]).y + (point.xyz [5] [5] [5]).y)/2;
	
	*alpha = -angle (y,x,hypoten (y,x));
	}
