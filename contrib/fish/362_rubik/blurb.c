/*	blurb for rubik	*/

#include <aros/oldprograms.h>

extern struct Window *window,*wnd1,*wnd2;
extern struct GfxBase * GfxBase;
struct Window *w;

void clean()
	{
	if (window == wnd1)
		w = wnd2;
	else
		w = wnd1;
	
	SetAPen (w->RPort,0);
	SetDrMd (w->RPort,JAM1);
	RectFill(w->RPort,2,10,190,198);
	SetAPen (w->RPort,7);
	}

void blurb (short line, char *p)
	{
	Move (w->RPort,2,20+10*line);
	Text (w->RPort,p,strlen(p));
	}

void helpblurb()
	{
	clean();
	blurb(1,"Use SETUP mode to paint");
	blurb(2,"the cube till it looks");
	blurb(3,"like yours.  Then click");
	blurb(4,"SOLVE.");
	blurb(5,"In NORMAL mode, you can");
	blurb(6,"twist a face to the");
	blurb(7,"left or right, by");
	blurb(8,"pointing at it, and");
	blurb(9,"clicking the left or");
	blurb(10,"right mouse button.");
	blurb(12,"The gadgets at the top");
	blurb(13,"right of the screen");
	blurb(14,"rotate the whole cube.");
	blurb(16,"CLICK TO CONTINUE....");
	}

void aboutblurb()
	{
	clean();
	blurb(1,"Written by Martin Round");
	blurb(2,"Halesowen England.");
	blurb(4,"V 1.0  Public Domain.");
	blurb(6,"Contact me on these BBs");
	blurb(7,"MABBS:    021-444-8972");
	blurb(8,"Plug Ole: 021-472-0256");
	blurb(9,"AMLINK:   021-778-5868");
	blurb(11,"Does anyone have some");
	blurb(12,"code to solve cubes in");
	blurb(13,"sixty moves or less?");
	blurb(16,"CLICK TO CONTINUE....");
	}


