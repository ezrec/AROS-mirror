/*	bits and pieces for rubik.c		M.J.R.	4th Feb 90	*/

#include "header.c"

#include <intuition/intuition.h>

extern struct Window * wnd1,* wnd2;
extern struct Screen * scr1,* scr2;
extern struct Gadget redplus;
extern struct Gadget greenplus;
extern struct Gadget blueplus;
extern struct Gadget redminus;
extern struct Gadget greenminus;
extern struct Gadget blueminus;
extern struct Gadget colour1;
extern struct Gadget colour2;
extern struct Gadget colour3;
extern struct Gadget colour4;
extern struct Gadget colour5;
extern struct Gadget colour6;
extern struct Gadget redplusa;
extern struct Gadget greenplusa;
extern struct Gadget blueplusa;
extern struct Gadget redminusa;
extern struct Gadget greenminusa;
extern struct Gadget blueminusa;
extern struct Gadget colour1a;
extern struct Gadget colour2a;
extern struct Gadget colour3a;
extern struct Gadget colour4a;
extern struct Gadget colour5a;
extern struct Gadget colour6a;

extern struct GfxBase * GfxBase;

void getcolours(int pen, int *red, int *green, int *blue)
	{
	struct ViewPort *vp;
	UWORD colour;
	
	vp = ViewPortAddress(wnd1);
	
	colour = GetRGB4(vp->ColorMap,pen);
	
	*blue = colour & 0x00f;
	*green = (colour & 0x0f0) >> 4;
	*red = (colour & 0xf00) >> 8;
	}
	
void showcolours (int red, int green, int blue)
	{
	char buffer[7];
	
	sprintf(buffer,"%02d%02d%02d",red,green,blue);

	SetAPen(wnd1->RPort,7);
	SetBPen(wnd1->RPort,0);
	SetDrMd(wnd1->RPort,JAM2);

	Move(wnd1->RPort,249,184);
	Text(wnd1->RPort,buffer,2);
	Move(wnd1->RPort,273,184);
	Text(wnd1->RPort,&(buffer[2]),2);
	Move(wnd1->RPort,297,184);
	Text(wnd1->RPort,&(buffer[4]),2);

	ClipBlit
		(
		wnd1->RPort,243,157,
		wnd2->RPort,243,157,
		75,41,
		0xc0
		);
	}

void modcolours (int pen, int colour, int change)
	{
	int primary[3];
	
	getcolours(pen,&(primary[0]),&(primary[1]),&(primary[2]));
	
	if ((((primary[colour]) += change) > 15) || ((primary[colour]) < 0))
		(primary[colour]) -= change;
	
	SetRGB4(&(scr1->ViewPort),pen,primary[0],primary[1],primary[2]);
	SetRGB4(&(scr2->ViewPort),pen,primary[0],primary[1],primary[2]);
	
	showcolours(primary[0],primary[1],primary[2]);
	}

void selectpen (int pen)
	{
	int i,j,red,green,blue;
	
	SetAPen(wnd1->RPort,0);
	SetDrMd(wnd1->RPort,JAM1);
	
	for (i=0; i<4; i++)
		{
		Move(wnd1->RPort,202+10*i,165);
		Draw(wnd1->RPort,202+10*i,185);
		}

	for (i=0; i<3; i++)
		{
		Move(wnd1->RPort,202,165+10*i);
		Draw(wnd1->RPort,232,165+10*i);
		}

	SetAPen(wnd1->RPort,7);

	
	if (pen > 3)
		{
		i = 162+10*pen;
		j = 175;
		}
	else
		{
		i = 192+10*pen;
		j = 165;
		}
	
	Move (wnd1->RPort,i,j);
	Draw (wnd1->RPort,i+10,j);
	Draw (wnd1->RPort,i+10,j+10);
	Draw (wnd1->RPort,i,j+10);
	Draw (wnd1->RPort,i,j);
	
	ClipBlit
		(
		wnd1->RPort,201,164,
		wnd2->RPort,201,164,
		32,22,
		0xc0
		);
	
	getcolours(pen,&red,&green,&blue);
	showcolours(red,green,blue);
	}

void addpaints()
	{
	int i;
	
	AddGadget(wnd1,&redplus,-1);
	AddGadget(wnd1,&greenplus,-1);
	AddGadget(wnd1,&blueplus,-1);
	AddGadget(wnd1,&redminus,-1);
	AddGadget(wnd1,&greenminus,-1);
	AddGadget(wnd1,&blueminus,-1);
	AddGadget(wnd1,&colour1,-1);
	AddGadget(wnd1,&colour2,-1);
	AddGadget(wnd1,&colour3,-1);
	AddGadget(wnd1,&colour4,-1);
	AddGadget(wnd1,&colour5,-1);
	AddGadget(wnd1,&colour6,-1);

	AddGadget(wnd2,&redplusa,-1);
	AddGadget(wnd2,&greenplusa,-1);
	AddGadget(wnd2,&blueplusa,-1);
	AddGadget(wnd2,&redminusa,-1);
	AddGadget(wnd2,&greenminusa,-1);
	AddGadget(wnd2,&blueminusa,-1);
	AddGadget(wnd2,&colour1a,-1);
	AddGadget(wnd2,&colour2a,-1);
	AddGadget(wnd2,&colour3a,-1);
	AddGadget(wnd2,&colour4a,-1);
	AddGadget(wnd2,&colour5a,-1);
	AddGadget(wnd2,&colour6a,-1);
	
	SetAPen(wnd1->RPort,7);
	SetDrMd(wnd1->RPort,JAM1);
	
	for (i=0; i<3; i++)
		{
		Move(wnd1->RPort,253+i*24,168);
		Draw(wnd1->RPort,260+i*24,168);
		Move(wnd1->RPort,253+i*24,186);
		Draw(wnd1->RPort,260+i*24,186);
		}
	
	Move(wnd1->RPort,200,163);
	Draw(wnd1->RPort,234,163);
	Draw(wnd1->RPort,234,187);
	Draw(wnd1->RPort,200,187);
	Draw(wnd1->RPort,200,163);

	for (i=0; i<4; i++)
		{
		Move(wnd1->RPort,244+24*i,158);
		Draw(wnd1->RPort,244+24*i,195);
		Draw(wnd1->RPort,245+24*i,195);
		Draw(wnd1->RPort,245+24*i,158);
		}

	for (i=0; i<2; i++)
		{
		Move(wnd1->RPort,244,157+39*i);
		Draw(wnd1->RPort,317,157+39*i);
		Draw(wnd1->RPort,317,158+39*i);
		Draw(wnd1->RPort,244,158+39*i);
		}
	
	Move(wnd1->RPort,253,166);
	Text(wnd1->RPort,"R  G  B",7);
	
	ClipBlit
		(
		wnd1->RPort,190,150,
		wnd2->RPort,190,150,
		128,48,
		0xc0
		);
				
	RefreshGadgets(&redplus,wnd1,NULL);
	RefreshGadgets(&redplusa,wnd2,NULL);
	
	selectpen(1);
	}


void removepaints()
	{
	RemoveGadget(wnd1,&redplus);
	RemoveGadget(wnd1,&greenplus);
	RemoveGadget(wnd1,&blueplus);
	RemoveGadget(wnd1,&redminus);
	RemoveGadget(wnd1,&greenminus);
	RemoveGadget(wnd1,&blueminus);
	RemoveGadget(wnd1,&colour1);
	RemoveGadget(wnd1,&colour2);
	RemoveGadget(wnd1,&colour3);
	RemoveGadget(wnd1,&colour4);
	RemoveGadget(wnd1,&colour5);
	RemoveGadget(wnd1,&colour6);

	RemoveGadget(wnd2,&redplusa);
	RemoveGadget(wnd2,&greenplusa);
	RemoveGadget(wnd2,&blueplusa);
	RemoveGadget(wnd2,&redminusa);
	RemoveGadget(wnd2,&greenminusa);
	RemoveGadget(wnd2,&blueminusa);
	RemoveGadget(wnd2,&colour1a);
	RemoveGadget(wnd2,&colour2a);
	RemoveGadget(wnd2,&colour3a);
	RemoveGadget(wnd2,&colour4a);
	RemoveGadget(wnd2,&colour5a);
	RemoveGadget(wnd2,&colour6a);

	SetDrMd(wnd1->RPort,JAM1);
	SetAPen(wnd1->RPort,0);
	RectFill(wnd1->RPort,190,150,317,197);
	
	ClipBlit
		(
		wnd1->RPort,190,150,
		wnd2->RPort,190,150,
		128,48,
		0xc0
		);
	}
