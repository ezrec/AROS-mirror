/*	rubik.c	*/

/*	started 23-Dec-89	*/

/*	Declarations for CBACK	*/

#include "header.c"

extern BPTR _Backstdout;		/*	std output when run in background	*/
long _BackGroundIO = 1;			/*	Flag to tell it we want to do I/O	*/
long _stack = 4000;				/*	Amount of stack space task needs	*/
char *_procname = "Rubik";		/*	The name of the task to create		*/
long _priority = 0;				/*	The priority to run us at			*/

struct points point;	/*	holds posn.s of all points 	*/
struct points start;
struct one_square square[56];		/*	54 on cube plus 2 inter-planes	*/
struct coord *temp;
struct Window *wnd1 = NULL;
struct Window *wnd2 = NULL;
struct Window *window;
struct Screen *scr1 = NULL;
struct Screen *scr2 = NULL;
struct Screen *screen;
struct IntuiMessage *msg = NULL;
extern struct IntuiMessage *solve();
extern struct Gadget fastergadget,slowergadget;
extern struct Gadget normalgadget,solvegadget,setupgadget;

extern struct GfxBase * GfxBase;

/*	size is half the side length of a SMALL cube...1/6 of whole cube	*/
unsigned short size;
short mode;
int viewdist;
short xcent,ycent;
short alpha,beta,gamma,delta;
unsigned short scaling;

extern int clockwise (int);
extern int whatsquare (int, int);
extern int paints (int, int, int);
extern short waiting_output;

extern void init_squares();
extern void init_points();
extern void init_trig();
extern void cleanup();
extern void getwindow();
extern void roll (short);
extern void yaw (short);
extern void pitch (short);
extern void twist (short, short);
extern void draw_square(int);
extern void showcube (short, short, short, short, short, short);
extern void findangles (short *, short *, short *);
extern void addpaints();
extern void removepaints();
extern void selectpen(int);
extern void modcolours(int, int, int);
extern void helpblurb();
extern void aboutblurb();

void swap (int i)
	{
	temp = square[i].corner[1];
	square[i].corner[1] = square[i].corner[3];
	square[i].corner[3] = temp;
	}

void showit (short *alpha, short *beta, short *gamma)
	{
	ReplyMsg((struct Message *) msg);
	findangles(alpha,beta,gamma);
	showcube (*alpha,*beta,*gamma,0,0,0);
	}

void select (short mode)	/*	pseudo mutual exclude	*/
	{
	int i;
	
	for (i=0; i<3; i++)
		{
		SetDrMd (wnd1->RPort,JAM1);
		SetDrMd (wnd2->RPort,JAM1);

		if (i == mode)
			{
			SetAPen(wnd1->RPort,7);
			SetAPen(wnd2->RPort,7);
			}
		else
			{
			SetAPen(wnd1->RPort,0);
			SetAPen(wnd2->RPort,0);
			}

		RectFill (wnd1->RPort,198,114+12*i,252,124+12*i);
		RectFill (wnd2->RPort,198,114+12*i,252,124+12*i);
		SetDrMd (wnd1->RPort,COMPLEMENT);
		SetDrMd (wnd2->RPort,COMPLEMENT);
		switch (i)
			{
			case 0:
				Move(wnd1->RPort,202,122);
				Text(wnd1->RPort,"NORMAL",6);
				Move(wnd2->RPort,202,122);
				Text(wnd2->RPort,"NORMAL",6);
				break;

			case 1:
				Move(wnd1->RPort,206,134);
				Text(wnd1->RPort,"SOLVE",5);
				Move(wnd2->RPort,206,134);
				Text(wnd2->RPort,"SOLVE",5);
				break;

			case 2:
				Move(wnd1->RPort,206,146);
				Text(wnd1->RPort,"SETUP",5);
				Move(wnd2->RPort,206,146);
				Text(wnd2->RPort,"SETUP",5);
				break;
			}
		}
	SetDrMd (wnd1->RPort,JAM1);
	SetDrMd (wnd2->RPort,JAM1);

	}

struct IntuiMessage *getmsg()
	{
	if
		(
		(msg = (struct IntuiMessage *) GetMsg(wnd1->UserPort)) ||
		(msg = (struct IntuiMessage *) GetMsg(wnd2->UserPort))
		)
		;
	return (msg);
	}
	
void showtwist (short i, short j)
	{
	showcube (alpha,beta,gamma,i,j,delta);
	}

void main (char *cmd)
	{
	int i,j;
	int pen;
		
	size = 2048;
	scaling = 7;
	viewdist = (2 << (scaling-3)) * size;
		
	init_trig();
	
	getwindow();
	init_squares();
	init_points();

	SetOutlinePen(wnd1->RPort,0);
	SetOutlinePen(wnd2->RPort,0);

	window = wnd1;
	screen = scr1;

	xcent = 96;
	ycent = 104;
	delta = 15;					/*	degrees of twist per frame	*/

	alpha = 0;
	beta = 45;
	gamma = 30;

	/*	make those squares that we can initialy see clockwise	*/
	point = start;
	
	for (i=0; i<54; i++)
		{
		if (clockwise(i))
			{
			if (i > 8)
				swap (i);
			}
		else	/*	anticlockwise	*/
			{
			if (i < 9)
				swap (i);
			}
		}

	showcube (alpha,beta,gamma,0,0,0);
	select (mode = 0);
	waiting_output = 0;

	for (;;)	/*	forever	*/
		{
		if (mode == 1)
			{
			msg = solve();
			if (msg == NULL)
				select (mode = 0);
			}
		else
			msg = getmsg();
		
		if (msg)
			{
			switch (msg->Class)
				{
				case CLOSEWINDOW:
	 				ReplyMsg((struct Message *) msg);
	 				cleanup();
					Close (_Backstdout);
	 				exit(0L);
	 				break;
					
				case GADGETUP:
					switch (((struct Gadget *) msg->IAddress)->GadgetID)
						{
						case 1:
							roll (delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 2:
							roll ((short) -delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 3:
							yaw (delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 4:
							yaw ((short) -delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 5:
							pitch (delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 6:
							pitch ((short) -delta);
							showit (&alpha,&beta,&gamma);
							break;

						case 7:			/*	front	*/
							alpha = 0;
							beta = 45;
							gamma = 30;
							ReplyMsg((struct Message *) msg);
							showcube (alpha,beta,gamma,0,0,0);
							break;

						case 8:			/*	back	*/
							alpha = 0;
							beta = -135;
							gamma = -30;
							ReplyMsg((struct Message *) msg);
							showcube (alpha,beta,gamma,0,0,0);
							break;
						
						case 9:			/*	faster	*/
							ReplyMsg((struct Message *) msg);
							if (delta == 5)
								delta = 15;
							else if (delta == 15)
								delta = 30;
							else
								{
								delta = 45;
								OffGadget(&fastergadget,wnd1,NULL);
								OffGadget(&fastergadget,wnd2,NULL);
								}
							OnGadget(&slowergadget,wnd1,NULL);
							OnGadget(&slowergadget,wnd2,NULL);

							break;

						case 10:		/*	slower	*/
							ReplyMsg((struct Message *) msg);
							if (delta == 45)
								delta = 30;
							else if (delta == 30)
								delta = 15;
							else
								{
								delta = 5;
								OffGadget(&slowergadget,wnd1,NULL);
								OffGadget(&slowergadget,wnd2,NULL);
								}
							OnGadget(&fastergadget,wnd1,NULL);
							OnGadget(&fastergadget,wnd2,NULL);
							break;
						
						case 11:		/*	help	*/
							ReplyMsg((struct Message *) msg);
							helpblurb();
							
							while ((msg = getmsg()) == NULL)
								Delay (1);
								
							ReplyMsg((struct Message *) msg);
							showcube (alpha,beta,gamma,0,0,0);
							break;

						case 12:		/*	about	*/
							ReplyMsg((struct Message *) msg);
							aboutblurb();
							
							while ((msg = getmsg()) == NULL)
								Delay (1);
								
							ReplyMsg((struct Message *) msg);
							showcube (alpha,beta,gamma,0,0,0);
							break;

						case 13:		/*	normal	*/
							ReplyMsg((struct Message *) msg);
							if (mode == 2)
								removepaints();
							select(mode = 0);
							
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
								
							break;

						case 14:		/*	solve	*/
							ReplyMsg((struct Message *) msg);
							if (mode == 2)
								removepaints();
							select(mode = 1);
							break;

						case 15:		/*	setup	*/
							if (mode != 2)
								{
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

								addpaints();
								pen = 1;
								}
							ReplyMsg((struct Message *) msg);
							select(mode = 2);
							break;
						
						case 16:		/*	redplus		*/
						case 17:		/*	greenplus	*/
						case 18:		/*	blueplus	*/
						case 19:		/*	redminus	*/
						case 20:		/*	greenminus	*/
						case 21:		/*	blueminus	*/
							i =
								(
								((struct Gadget *)msg->IAddress)->GadgetID
								)
								-16;

							ReplyMsg((struct Message *) msg);
						
							if (mode == 2)
								{
								if (i > 2)
									modcolours(pen, i-3, -1);
								else
									modcolours(pen, i, +1);
								}
							
							break;
						
						case 22:
						case 23:
						case 24:
						case 25:
						case 26:
						case 27:			/*	choosing pen colour	*/
							if (mode == 2)
								pen =
										(
										((struct Gadget *)msg->IAddress)
										->GadgetID
										)
										-21;

							ReplyMsg((struct Message *) msg);

							if (mode == 2)
								selectpen(pen);

							break;
							
						default:
							ReplyMsg((struct Message *) msg);
							break;
						}
					break;

				case MOUSEBUTTONS:
					if (msg->Code == SELECTDOWN || msg->Code == MENUDOWN)
						{
						if (mode == 0)
							{
							if	((i = whatsquare(msg->MouseX,msg->MouseY)) >=0)
								{
								/*	make i = layer to twist, j = direction */
							
								j = msg->Code == SELECTDOWN ? 1: -1;
							
								if ((i /= 9) == 1 || i == 2 || i == 4)
									j = -j;
								
								showtwist ((short) i,(short) j);
								waiting_output = 0;
								}
							}
						else if (mode == 2)
							{
							if	((i = paints(msg->MouseX,msg->MouseY,pen)) >=0)
								{
								(square[i]).colour = pen;
								waiting_output = 0;
								}
							}
						}
					ReplyMsg((struct Message *) msg);
					break;

				default:
					ReplyMsg((struct Message *) msg);
	 				break;
				}
			}
		else
			Delay (1);	/*	give other tasks a chance	*/
		}
	}

