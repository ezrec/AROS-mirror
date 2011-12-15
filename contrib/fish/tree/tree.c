/************************************************************************
*  Version 1.00       TREE.C - Draw a Recursive Tree         08-May-86  *
*  Commodore Amiga              Only Module                     TREE.C  *
*************************************************************************
*                  Copyright (c) 1986, Robert S. French                 *
* --------------------------------------------------------------------- *
*  This program has been placed in the public domain.  A limited        *
*  license is hereby granted for the unlimited use and distribution of  *
*  this program, provided it is not used for commercial or profit-      *
*  making purposes.  Thank you.                                         *
*************************************************************************
*  Author information:              |           Disclaimer:             *
*                                   |                                   *
*  Name:   Robert S. French         |  The author takes no responsibil- *
*  USnail: 2740 Frankfort Avenue    |  ity for damages incurred during  *
*          Louisville, KY  40206    |  the use of this program.         *
*  Phone:  (502) 897-5096           \-----------------------------------*
*  ARPA:   French#Robert%d@LLL-MFE     UUCP: ihnp4!ptsfa!well!french    *
*************************************************************************
*  Please send any comments, suggestions, or bugs to one of the above   *
*  addresses.                                                           *
*************************************************************************
*                           Acknowledgements                            *
*                           ================                            *
*                                                                       *
*  Original version presented for IBM BASIC by William K. Balthrop in   *
*  Home Computer Magazine, Vol. 5, No. 6.                               *
*                                                                       *
*  Random number generator from the Encyclopedia of Computer Science    *
*  and Engineering, second edition.                                     *
************************************************************************/

/* Necessary includes */

#include <aros/oldprograms.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <devices/keymap.h>
#include <graphics/copper.h>
/*
  #include <graphics/display.h>
*/
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/view.h>
#include <graphics/gels.h>
#include <graphics/regions.h>
#include <hardware/blit.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>

#define abort myabort

void exit(int);

/* Function Prototypes */
void chk_done();
void draw_ground();
long my_rand();
void init_rand();
void drawbranch(int x,int y,int ex,int ey,int d,int l);
void branch(int xs,int ys,int len,int dir,int rl);
void abort(char *s);

/* Library Pointers */

struct IntuitionBase *IntuitionBase = 0;
struct GfxBase *GfxBase = 0;

/* Initial Graphics Conditions */

struct NewScreen newscr = {
	0,		/* Left */
	0,		/* Top */
	640,	/* Width */
	200,	/* Height */
	4,		/* Depth */
	0,		/* DPen */
	1,		/* BPen */
	HIRES,		/* View Modes */
	CUSTOMSCREEN,	/* Type */
	0,		/* Font */
	"Recursive Tree (1.00) by Robert French - Right mouse button to exit",	/* Title */
	0,		/* Gadgets */
	0		/* Bitmap */
};

struct NewWindow newwin = {
	0,		/* Left */
	0,		/* Top */
	640,	/* Width */
	200,	/* Height */
	-1,	/* DPen */
	-1,	/* BPen */
	MOUSEBUTTONS,	/* IDCMP */
	BACKDROP | BORDERLESS | ACTIVATE | RMBTRAP,	/* Flags */
	0,		/* Gadgets */
	0,		/* Check */
	0,		/* Title */
	0,		/* Screen */
	0,		/* Bitmap */
	0,		/* MinWidth */
	0,		/* MinHeight */
	0,		/* MaxWidth */
	0,		/* MaxHeight */
	CUSTOMSCREEN	/* Screen type */
};

UWORD colors[16] = {
	0x000, 0xfff, 0xf00, 0xff0, 0x0d0, 0x0b0, 0x090, 0xa84,
	0x973, 0x970, 0x960, 0x860, 0x850, 0x750, 0x740, 0x640
};

struct Screen *scr = 0;
struct Window *win = 0;
struct RastPort *rp;
struct ViewPort *vp;

/* Random Number Definitions */

long seeds[17];
int seedp1,seedp2,seedp3;

/* Direction Definitions */

struct s_dir {
	int	x;
	int	y;
} offset[8] = {
	{  0, -1 },
	{  2, -1 },
	{  2,  0 },
	{  2,  1 },
	{  0,  1 },
	{ -2,  1 },
	{ -2,  0 },
	{ -2, -1 }
}, boff[4] = {
	{ -1,  0 },
	{ -1, -1 },
	{  0, -1 },
	{  1, -1 }
};

int main()
{
	int i;

	if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0)) == 0)
		abort("Can't open intuition.library");

	if ((GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0)) == 0)
		abort("Can't open graphics.library");

	if ((scr = (struct Screen *)OpenScreen(&newscr)) == 0)
		abort("Can't open screen");

	newwin.Screen = scr;

	if ((win = (struct Window *)OpenWindow(&newwin)) == 0)
		abort("Can't open window");

	rp = win->RPort;
	vp = &scr->ViewPort;

	LoadRGB4(vp,colors,16);
	SetDrMd(rp,JAM1);
	init_rand();

	for (;;) {
		SetAPen(rp,0);
		RectFill(rp,0,10,639,199);

		draw_ground();

		branch(320,120,14,0,1);		/* Draw first right branch */
		branch(320,120,14,0,-1);	/* Draw first left branch */

		for (i=0;i<10;i++) {
			Delay(60);
			chk_done();
		}
	}
	abort("");
}

void abort(s)
char *s;
{
	if (win)
		CloseWindow(win);
	if (scr)
		CloseScreen(scr);
	if (IntuitionBase)
		CloseLibrary((struct Library *)IntuitionBase);
	if (GfxBase)
		CloseLibrary((struct Library *)GfxBase);
	printf("%s\n",s);
	exit(0);
}

void branch(xs,ys,len,dir,rl)
int xs,ys,len,dir,rl;
{
	int r,xe,ye;

	chk_done();

	if (len > 11)
		len--;
	else {
		r = my_rand();
		switch (r) {
			case 12:
				len /= 2;
				break;
			case 1:
				len *= 7;
				len /= 10;
				break;
			case 2:
			case 3:
				len *= 8;
				len /= 10;
				break;
			case 4:
				len *= 9;
				len /= 10;
				break;
			default:
				len--;
				break;
		}
	}

	dir = (dir+rl) & 7;
	xe = xs+len*offset[dir].x;
	ye = ys+len*offset[dir].y;
	r = my_rand();
	switch (r) {
		case 0:
			xe--;
			break;
		case 1:
			xe++;
			break;
		case 2:
			ye--;
			break;
		case 3:
			ye++;
			break;
	}
	drawbranch(xs,ys,xe,ye,dir,len);
	if (len < 1)
		return;
	branch(xe,ye,len,dir,1);	/* Draw right branch */
	branch(xe,ye,len,dir,-1);	/* Draw left branch */
}

void drawbranch(x,y,ex,ey,d,l)
int x,y,ex,ey,d,l;
{
	SetAPen(rp,l+2);

	if (l > 9) {
		Move(rp,x+boff[d&3].x,y+boff[d&3].y);
		Draw(rp,ex+boff[d&3].x,ey+boff[d&3].y);
	}
	if (l > 6) {
		Move(rp,x-boff[d&3].x,y-boff[d&3].y);
		Draw(rp,ex-boff[d&3].x,ey-boff[d&3].y);
	}
	Move(rp,x,y);
	Draw(rp,ex,ey);
}

void init_rand()
{
	LONG sec,mic,x;
	int i;

	CurrentTime(&sec,&mic);

	x = sec*(mic | 1);

	for (i=0;i<17;i++) {
		seeds[i] = x;
		x = x*3+mic;
	}

	seedp1 = 4;
	seedp2 = 16;
	seedp3 = 0;
}

long my_rand()
{
	long result;

	result = seeds[seedp1]+seeds[seedp2];
	seeds[seedp3] = result;
	if (++seedp1 > 16)
		seedp1 = 0;
	if (++seedp2 > 16)
		seedp2 = 0;
	if (++seedp3 > 16)
		seedp3 = 0;

	return (result & 31);
}

void draw_ground()
{
	int i,j,x,y,c;

	SetAPen(rp,5);
	RectFill(rp,0,194,639,199);
	SetAPen(rp,14);
	x = 28;
	y = 199;
	for (c=0;c<13;c++) {
		for (j=0;j<2;j++) {
			for (i=0;i<c;i++,y--) {
				if (y < 118)
					return;
				Move(rp,318-x,y);
				Draw(rp,322+x,y);
			}
			x--;
		}
	}
}

void chk_done()
{
	struct IntuiMessage *message;
	ULONG class;
	USHORT code;

	while ((message = (struct IntuiMessage *)GetMsg(win->UserPort))) {
		class = message->Class;
		code = message->Code;
		ReplyMsg((struct Message *)message);
		if (class == MOUSEBUTTONS && code == MENUDOWN)
			abort("");
	}
}
