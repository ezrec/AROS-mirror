#define __A0
#define __D0
#define __D1
#define __D2
#define __chip
#define clrmem(addr,size) memset(addr, 0x0, size)
#include <aros/oldprograms.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "gfx.h"


struct Player
    {
	SHORT x, y;
	SHORT xwin, ywin;
	SHORT Moves;
	SHORT Chests;
	SHORT xoff;
	UBYTE mask[100][100];
    };

/**************/
/* prototypes */
/**************/

void PrintAt(__D0 SHORT, __D1 SHORT, __A0 char *);
void PutField(__A0 struct Player *, __D0 SHORT, __D1 SHORT);

SHORT SelectOption();
BOOL About();
BOOL Game();
void ChangePos(__A0 struct Player *);
BOOL DoPlayer(__A0 struct Player *, __D0 UBYTE);
void CreateWorld();
int main();

/***********/
/* globals */
/***********/

/* font def */

struct TextAttr fontdef =
    {
	"topaz.font",
	8,
	FS_NORMAL,
	FPF_ROMFONT
    };

/* Option Gadget Render1 */

SHORT OGadget_Render1XYa[] =
    {
	0, 0,
	0, 49,
	1, 48,
	1, 0,
	238, 0
    };

SHORT OGadget_Render1XYb[] =
    {
	1, 49,
	239, 49,
	239, 0,
	238, 1,
	238, 49
    };

struct Border OGadget_Render1Ub =
    {
	0, 0,
	2, 0, JAM1,
	5,
	OGadget_Render1XYb,
	NULL
    };

struct Border OGadget_Render1Ua =
    {
	0, 0,
	1, 0, JAM1,
	5,
	OGadget_Render1XYa,
	&OGadget_Render1Ub
    };

struct Border OGadget_Render1Sb =
    {
	0, 0,
	1, 0, JAM1,
	5,
	OGadget_Render1XYb,
	NULL
    };

struct Border OGadget_Render1Sa =
    {
	0, 0,
	2, 0, JAM1,
	5,
	OGadget_Render1XYa,
	&OGadget_Render1Sb
    };

/* Option Gadget Render2 */

SHORT OGadget_Render2XYa[] =
    {
	0, 0,
	0, 10,
	1, 10,
	1, 0,
	24, 0
    };

SHORT OGadget_Render2XYb[] =
    {
	1, 10,
	25, 10,
	25, 0,
	24, 1,
	24, 10
    };

struct Border OGadget_Render2Ub =
    {
	0, 0,
	2, 0, JAM1,
	5,
	OGadget_Render2XYb,
	NULL
    };

struct Border OGadget_Render2Ua =
    {
	0, 0,
	1, 0, JAM1,
	5,
	OGadget_Render2XYa,
	&OGadget_Render2Ub
    };

struct Border OGadget_Render2Sb =
    {
	0, 0,
	1, 0, JAM1,
	5,
	OGadget_Render2XYb,
	NULL
    };

struct Border OGadget_Render2Sa =
    {
	0, 0,
	2, 0, JAM1,
	5,
	OGadget_Render2XYa,
	&OGadget_Render2Sb
    };

/* Option Gadget 7 */

struct IntuiText OGadget7_Text =
    {
	1, 0, JAM1,
	5, 2,
	&fontdef,
	"<-",
	NULL
    };

struct Gadget OGadget7 =
    {
	NULL,
	376, 92, 26, 11,
	GADGHIMAGE,
	GADGIMMEDIATE | RELVERIFY,
	BOOLGADGET,
	&OGadget_Render2Ua,
	&OGadget_Render2Sa,
	&OGadget7_Text,
	0,
	0,
	6,
	NULL
    };

/* Option Gadget 6 */

struct IntuiText OGadget6_Text =
    {
	1, 0, JAM1,
	5, 2,
	&fontdef,
	"->",
	NULL
    };

struct Gadget OGadget6 =
    {
	&OGadget7,
	158, 114, 26, 11,
	GADGHIMAGE,
	GADGIMMEDIATE | RELVERIFY,
	BOOLGADGET,
	&OGadget_Render2Ua,
	&OGadget_Render2Sa,
	&OGadget6_Text,
	0,
	0,
	5,
	NULL
    };

/* Option Gadget 5 */

struct IntuiText OGadget5_Text =
    {
	1, 0, JAM1,
	5, 2,
	&fontdef,
	"<-",
	NULL
    };

struct Gadget OGadget5 =
    {
	&OGadget6,
	130, 114, 26, 11,
	GADGHIMAGE,
	GADGIMMEDIATE | RELVERIFY,
	BOOLGADGET,
	&OGadget_Render2Ua,
	&OGadget_Render2Sa,
	&OGadget5_Text,
	0,
	0,
	4,
	NULL
    };

/* Option Gadget 4 */

struct IntuiText OGadget4_Text =
    {
	1, 0, JAM1,
	5, 2,
	&fontdef,
	"->",
	NULL
    };

struct Gadget OGadget4 =
    {
	&OGadget5,
	158, 92, 26, 11,
	GADGHIMAGE,
	GADGIMMEDIATE | RELVERIFY,
	BOOLGADGET,
	&OGadget_Render2Ua,
	&OGadget_Render2Sa,
	&OGadget4_Text,
	0,
	0,
	3,
	NULL
    };

/* Option Gadget 3 */

struct IntuiText OGadget3_Text =
    {
	1, 0, JAM1,
	5, 2,
	&fontdef,
	"<-",
	NULL
    };

struct Gadget OGadget3 =
    {
	&OGadget4,
	130, 92, 26, 11,
	GADGHIMAGE,
	GADGIMMEDIATE | RELVERIFY,
	BOOLGADGET,
	&OGadget_Render2Ua,
	&OGadget_Render2Sa,
	&OGadget3_Text,
	0,
	0,
	2,
	NULL
    };

/* Option Gadget 2 */

struct IntuiText OGadget2_Text =
    {
	1, 0, JAM1,
	100, 21,
	&fontdef,
	"Start",
	NULL
    };

struct Gadget OGadget2 =
    {
	&OGadget3,
	280, 20, 240, 50,
	GADGHIMAGE,
	RELVERIFY,
	BOOLGADGET,
	&OGadget_Render1Ua,
	&OGadget_Render1Sa,
	&OGadget2_Text,
	0,
	0,
	1,
	NULL
    };

/* Option Gadget 1 */

struct IntuiText OGadget1_Text =
    {
	1, 0, JAM1,
	100, 21,
	&fontdef,
	"About",
	NULL
    };

struct Gadget OGadget1 =
    {
	&OGadget2,
	20, 20, 240, 50,
	GADGHIMAGE,
	RELVERIFY,
	BOOLGADGET,
	&OGadget_Render1Ua,
	&OGadget_Render1Sa,
	&OGadget1_Text,
	0,
	0,
	0,
	NULL
    };

/* window def */

struct NewWindow nw =
    {
	50, 25, 640-100, 200-50,
	0, 1,
	GADGETUP | GADGETDOWN | RAWKEY | CLOSEWINDOW | MOUSEBUTTONS,
	WINDOWCLOSE | WINDOWDRAG | WINDOWDEPTH | SMART_REFRESH |
	REPORTMOUSE | NOCAREREFRESH | RMBTRAP | ACTIVATE,
	NULL,
	NULL,
	"Ancient Egypt V1.0    © 1992 by Paradise Soft   FREEWARE",
	NULL,
	NULL,
	0, 0, 0, 0,
	WBENCHSCREEN
    };

struct Window *window;
struct RastPort *rp;

/* game data */


struct Player Player1;
struct Player Player2;

UBYTE world[100][100];
SHORT worldx = 50;
SHORT worldy = 50;
WORD fontheight = 8;
WORD wintop = 11;

BOOL GameMode = TRUE;

/********************/
/* main 	    */
/********************/

int wbmain()
    {
	return(main());
    }

struct IntuitionBase * IntuitionBase;
struct GfxBase * GfxBase;

BOOL openlibs(void)
{
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
  
  if (!GfxBase || !IntuitionBase)
    return FALSE;
  
  {
    struct Screen *scr = LockPubScreen(NULL);
    if (scr)
    {
    	fontheight = GfxBase->DefaultFont->tf_YSize;
	wintop = scr->WBorTop + scr->Font->ta_YSize + 1;
	
	OGadget1.TopEdge += wintop - 11;
	OGadget2.TopEdge += wintop - 11;
	OGadget3.TopEdge += wintop - 11;
	OGadget4.TopEdge += wintop - 11;
	OGadget5.TopEdge += wintop - 11;
	OGadget6.TopEdge += wintop - 11;
	OGadget7.TopEdge += wintop - 11;
	
    	UnlockPubScreen(NULL, scr);
    }
  }
  
  return TRUE;
}

void closelibs(void)
{
  if (GfxBase) CloseLibrary((struct Library *)GfxBase);
  if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
}

int main()
    {
      srand(time(NULL));

      if (openlibs())
      {
    	nw.Height += wintop - 11;
	
	window = (struct Window *)OpenWindow(&nw);

	if ( window )
	    {
		BOOL loop = TRUE;
		rp = window->RPort;

		while ( loop )
		    {
			switch ( SelectOption() )
			    {
				case -1 :
				    loop = FALSE;
				    break;
				case 0 :
				    loop = About();
				    break;
				case 1 :
				    loop = Game();
				    break;
			    }
		    }

		CloseWindow(window);
	    }
	else
	    {
		SHORT i;

		for ( i = 10 ; i ; i-- )
		    {
			DisplayBeep(NULL);
			Delay(2);
		    }
	    }
      }
      closelibs();
    }

/********************/
/* SelectOption()   */
/********************/

SHORT SelectOption()
    {
	char Buffer[40];

	SetAPen(rp, 0);
	RectFill(rp, 5, wintop, 535, 134 + wintop);

	SetAPen(rp, 1);
	PrintAt(20, 69 + wintop, "World Size :");
	sprintf(Buffer, "Width  : %3u", worldx);
	PrintAt(28, 89 + wintop, Buffer);
	sprintf(Buffer, "Height : %3u", worldy);
	PrintAt(28, 111 + wintop, Buffer);

	PrintAt(280, 69 + wintop, "Game mode :");
	if ( GameMode )
	    PrintAt(288, 89 + wintop, "One Player");
	else
	    PrintAt(288, 89 + wintop, "Two Player");

	OGadget7.NextGadget = window->FirstGadget;
	window->FirstGadget = &OGadget1;
	RefreshGadgets(&OGadget1, window, NULL);

	while ( TRUE )
	    {
		struct IntuiMessage *msg;

		Wait( 1 << window->UserPort->mp_SigBit );

		while ( msg = (struct IntuiMessage *)GetMsg(window->UserPort ))
		    {
			ULONG class = msg->Class;
			struct Gadget *iaddress = msg->IAddress;

			ReplyMsg((struct Message *)msg);

			switch ( class )
			    {
				case CLOSEWINDOW :
				    return(-1);
				case GADGETUP :
				    switch ( iaddress->GadgetID )
					{
					    case 0 :
						return(0);
					    case 1 :
						return(1);
					}
				    break;
				case GADGETDOWN :
				    switch ( iaddress->GadgetID )
					{
					    case 2 :
						while ( (worldx > 20) && (OGadget3.Flags & SELECTED) )
						    {
							worldx--;
							SetAPen(rp, 1);
							sprintf(Buffer, "Width  : %3u", worldx);
							PrintAt(28, 89 + wintop, Buffer);
							Delay(2);
						    }
						break;
					    case 3 :
						while ( (worldx < 100) && (OGadget4.Flags & SELECTED) )
						    {
							worldx++;
							SetAPen(rp, 1);
							sprintf(Buffer, "Width  : %3u", worldx);
							PrintAt(28, 89 + wintop, Buffer);
							Delay(2);
						    }
						break;
					    case 4 :
						while ( (worldy > 20) && (OGadget5.Flags & SELECTED) )
						    {
							worldy--;
							SetAPen(rp, 1);
							sprintf(Buffer, "Height : %3u", worldy);
							PrintAt(28, 111 + wintop, Buffer);
							Delay(2);
						    }
						break;
					    case 5 :
						while ( (worldy < 100) && (OGadget6.Flags & SELECTED) )
						    {
							worldy++;
							SetAPen(rp, 1);
							sprintf(Buffer, "Height : %3u", worldy);
							PrintAt(28, 111 + wintop, Buffer);
							Delay(2);
						    }
						break;
					    case 6 :
						GameMode = !GameMode;
						SetAPen(rp, 1);
						if ( GameMode )
						    PrintAt(288, 89 + wintop, "One Player");
						else
						    PrintAt(288, 89 + wintop, "Two Player");
						break;
					}
				    break;
			    }
		    }
	    }
    }

/********************/
/* About()          */
/********************/

BOOL About()
    {
	struct IntuiMessage *msg;
	BOOL loop = TRUE;

	window->FirstGadget = OGadget7.NextGadget;

	SetAPen(rp, 0);
	RectFill(rp, 5, wintop, 535, 134 + wintop);

	DrawImage(rp, &AboutImage, 197, wintop + 11);

	SetAPen(rp, 1);
	PrintAt(170, wintop + 69, "Coding & Graphics by Pink");
	PrintAt(210, wintop + 79, "Idea by Romulus");

	SetAPen(rp, 3);
	PrintAt(154, wintop + 99, "Read .doc for more information");

	while ( loop )
	    {
		Wait( 1 << window->UserPort->mp_SigBit );

		while ( msg = (struct IntuiMessage *)GetMsg(window->UserPort ))
		    {
			if ( (msg->Class == MOUSEBUTTONS) && (msg->Code == SELECTUP) )
			    loop = FALSE;

			ReplyMsg((struct Message *)msg);
		    }
	    }

	return(TRUE);
    }

/********************/
/* Game()           */
/********************/

BOOL Game()
    {
	BOOL loop = TRUE;

	window->FirstGadget = OGadget7.NextGadget;

	CreateWorld();

	/* init player */
	    {
		Player1.x = (rand()%(worldx-1)) + 1;
		Player1.y = (rand()%(worldy-1)) + 1;

		while ( world[Player1.x][Player1.y] != 0 )
		    {
			Player1.x++;
			if ( Player1.x >= worldx )
			    {
				Player1.x = 1;
				Player1.y++;
				if ( Player1.y >= worldy )
				    {
					Player1.x = 1;
					Player1.y = 1;
				    }
			    }
		    }

		Player1.xwin = Player1.x - 5;
		if ( Player1.xwin < 0 )
		    Player1.xwin = 0;
		else if ( Player1.xwin > worldx-12 )
		    Player1.xwin = worldx-12;

		Player1.ywin = Player1.y - 5;
		if ( Player1.ywin < 0 )
		    Player1.ywin = 0;
		else if ( Player1.ywin > worldy-11 )
		    Player1.ywin = worldy-11;

		Player1.Moves = 0;
		Player1.Chests = 0;
		Player1.xoff = 20;

		clrmem(Player1.mask, 100*100);

		Player2.x = (rand()%(worldx-1)) + 1;
		Player2.y = (rand()%(worldy-1)) + 1;

		while ( world[Player2.x][Player2.y] != 0 )
		    {
			Player2.x++;
			if ( Player2.x >= worldx )
			    {
				Player2.x = 1;
				Player2.y++;
				if ( Player2.y >= worldy )
				    {
					Player2.x = 1;
					Player2.y = 1;
				    }
			    }
		    }


		Player2.xwin = Player2.x - 5;
		if ( Player2.xwin < 0 )
		    Player2.xwin = 0;
		else if ( Player2.xwin > worldx-12 )
		    Player2.xwin = worldx-12;

		Player2.ywin = Player2.y - 5;
		if ( Player2.ywin < 0 )
		    Player2.ywin = 0;
		else if ( Player2.ywin > worldy-11 )
		    Player2.ywin = worldy-11;

		Player2.Moves = 0;
		Player2.Chests = 0;
		Player2.xoff = 280;

		clrmem(Player2.mask, 100*100);
	    }

	/* init screen */
	    {
		SHORT x, y;

		SetAPen(rp, 0);
		RectFill(rp, 5, wintop, 535, 134 + wintop);

		SetAPen(rp, 3);
		RectFill(rp, 20, 19 + wintop, 259, 128 + wintop);

		for ( x = 0 ; x < 12 ; x++ )
		    for ( y = 0 ; y < 11 ; y++ )
			PutField(&Player1, x+Player1.xwin, y+Player1.ywin);

		SetAPen(rp, 3);
		RectFill(rp, 280, 19 + wintop, 499, 128 + wintop);

		for ( x = 0 ; x < 12 ; x++ )
		    for ( y = 0 ; y < 11 ; y++ )
			PutField(&Player2, x+Player2.xwin, y+Player2.ywin);

		SetAPen(rp, 1);
		PrintAt(20, wintop + 1 + rp->TxBaseline, "Moves :   0  Chests : 0");
		PrintAt(280, wintop + 1 + rp->TxBaseline, "Moves :   0  Chests : 0");

		ChangePos(&Player1);
		ChangePos(&Player2);
	    }

	while ( loop )
	    {
		struct IntuiMessage *msg;

		Wait(1 << window->UserPort->mp_SigBit);

		while ( msg = (struct IntuiMessage *)GetMsg(window->UserPort) )
		    {
			ULONG class = msg->Class;
			USHORT code = msg->Code;
			struct Gadget *iaddress = msg->IAddress;
			SHORT mousex = msg->MouseX;
			SHORT mousey = msg->MouseY;

			ReplyMsg((struct Message *)msg);

			switch ( class )
			    {
				case CLOSEWINDOW :
				    loop = FALSE;
				case RAWKEY :
				    switch ( code )
					{
					    case 0x11 :
						loop = DoPlayer(&Player1, 0);
						break;
					    case 0x31 :
						loop = DoPlayer(&Player1, 1);
						break;
					    case 0x20 :
						loop = DoPlayer(&Player1, 2);
						break;
					    case 0x21 :
						loop = DoPlayer(&Player1, 3);
						break;
					    case 0x3E :
						if ( !GameMode )
						    loop = DoPlayer(&Player2, 0);
						break;
					    case 0x1E :
						if ( !GameMode )
						    loop = DoPlayer(&Player2, 1);
						break;
					    case 0x2D :
						if ( !GameMode )
						    loop = DoPlayer(&Player2, 2);
						break;
					    case 0x2F :
						if ( !GameMode )
						    loop = DoPlayer(&Player2, 3);
						break;
					}

				    break;
			    }
		    }
	    }

	/* end message */
	    {
		struct IntuiMessage *msg;
		BOOL loop = TRUE;

		SetAPen(rp, 0);
		RectFill(rp, 5, wintop, 535, 134 + wintop);

		if ( Player1.Chests == 3 )
		    {
			SetAPen(rp, 1);
			PrintAt(218, 69 + wintop, "Player 1 wins");
		    }
		else if ( Player2.Chests == 3 )
		    {
			SetAPen(rp, 1);
			PrintAt(218, 68 + wintop, "Player 2 wins");
		    }
		else
		    {
			SetAPen(rp, 1);
			PrintAt(214, 69 + wintop, "No Player wins");
		    }


		while ( loop )
		    {
			Wait( 1 << window->UserPort->mp_SigBit );

			while ( msg = (struct IntuiMessage *)GetMsg(window->UserPort ))
			    {
				if ( (msg->Class == MOUSEBUTTONS) && (msg->Code == SELECTUP) )
				    loop = FALSE;

				ReplyMsg((struct Message *)msg);
			    }
		    }
	    }

	return(TRUE);
    }

/********************/
/* DoPlayer()       */
/********************/

void ChangePos(__A0 struct Player *p)
    {
	SHORT xs=p->x-p->xwin, ys=p->y-p->ywin;
	SHORT x=p->x, y=p->y;

	p->mask[x+1][y-1] = TRUE; PutField(p, x+1, y-1);
	p->mask[x]  [y-1] = TRUE; PutField(p, x,   y-1);
	p->mask[x-1][y-1] = TRUE; PutField(p, x-1, y-1);
	p->mask[x+1][y]   = TRUE; PutField(p, x+1, y);
	p->mask[x]  [y]   = TRUE; DrawImage(rp, &ManImage, xs*20+p->xoff, ys*10+19+wintop);
	p->mask[x-1][y]   = TRUE; PutField(p, x-1, y);
	p->mask[x+1][y+1] = TRUE; PutField(p, x+1, y+1);
	p->mask[x]  [y+1] = TRUE; PutField(p, x,   y+1);
	p->mask[x-1][y+1] = TRUE; PutField(p, x-1, y+1);
    }

BOOL DoPlayer(__A0 struct Player *p, __D0 UBYTE dir)
    {
	char Buffer[40];

	SetAPen(rp, 1);
	sprintf(Buffer, "Moves : %3u  Chests : %u", p->Moves, p->Chests);
	PrintAt(p->xoff, wintop + 1 + rp->TxBaseline, Buffer);

	ChangePos(p);

	switch ( dir )
	    {
		case 0 :
		    switch ( world[p->x][p->y-1] )
			{
			    case 3 :
				p->Chests++;
				if ( p->Chests == 3 )
				    return(FALSE);
				world[p->x][p->y-1] = 0;
			    case 0 :
			    case 2 :
				p->y--;
				p->Moves++;

				if ( (p->ywin > 1) && (p->y < p->ywin + 3) )
				    {
					SHORT i;

					ScrollRaster(rp, 0, -10, p->xoff, 19 + wintop, p->xoff+239, 128 + wintop);

					p->ywin--;
					for ( i = 0 ; i < 12 ; i++ )
					    PutField(p, i+p->xwin, p->ywin);
				    }
				ChangePos(p);
				break;
			    case 1 :
				DisplayBeep(NULL);
				break;
			}
		    break;
		case 1 :
		    switch ( world[p->x][p->y+1] )
			{
			    case 3 :
				p->Chests++;
				if ( p->Chests == 3 )
				    return(FALSE);
				world[p->x][p->y+1] = 0;
			    case 0 :
			    case 2 :
				p->y++;
				p->Moves++;

				if ( (p->ywin < worldy-11) && (p->y > p->ywin + 7) )
				    {
					SHORT i;

					ScrollRaster(rp, 0, 10, p->xoff, 19 + wintop, p->xoff+239, 128 + wintop);

					p->ywin++;
					for ( i = 0 ; i < 12 ; i++ )
					    PutField(p, p->xwin+i, p->ywin+10);
				    }
				ChangePos(p);
				break;
			    case 1 :
				DisplayBeep(NULL);
				break;
			}
		    break;
		case 2 :
		    switch ( world[p->x-1][p->y] )
			{
			    case 3 :
				p->Chests++;
				if ( p->Chests == 3 )
				    return(FALSE);
				world[p->x-1][p->y] = 0;
			    case 0 :
			    case 2 :
				p->x--;
				p->Moves++;

				if ( (p->xwin > 1) && (p->x < p->xwin + 3) )
				    {
					SHORT i;

					ScrollRaster(rp, -20, 0, p->xoff, 19 + wintop, p->xoff+239, 128 + wintop);

					p->xwin--;
					for ( i = 0 ; i < 11 ; i++ )
					    PutField(p, p->xwin, p->ywin+i);
				    }
				ChangePos(p);
				break;
			    case 1 :
				DisplayBeep(NULL);
				break;
			}
		    break;
		case 3 :
		    switch ( world[p->x+1][p->y] )
			{
			    case 3 :
				p->Chests++;
				if ( p->Chests == 3 )
				    return(FALSE);
				world[p->x+1][p->y] = 0;
			    case 0 :
			    case 2 :
				p->x++;
				p->Moves++;

				if ( (p->xwin < worldx-12) && (p->x > p->xwin + 8) )
				    {
					SHORT i;

					ScrollRaster(rp, 20, 0, p->xoff, 19 + wintop, p->xoff+239, 128 + wintop);

					p->xwin++;
					for ( i = 0 ; i < 11 ; i++ )
					    PutField(p, p->xwin+11, p->ywin+i);
				    }
				ChangePos(p);
				break;
			    case 1 :
				DisplayBeep(NULL);
				break;
			}
		    break;
	    }

	return(TRUE);
    }

/********************/
/* CreateWorld()    */
/********************/

void Set(__D0 SHORT x, __D1 SHORT y, __D2 SHORT c)
    {
	world[x][y] = c;
    }

void CreateWay(__D0 SHORT x, __D1 SHORT y, __D2 SHORT d)
    {
	BOOL c = TRUE;

	while ( c )
	    {
		switch ( d )
		    {
			case 0 :
			    switch ( world[x][y-1] )
				{
				    case 0 :
				    case 1 :
					y--;
					Set(x, y, 0);
					break;
				    case 2 :
				    case 3 :
				    case 4 :
					c = FALSE;
					break;
				}
			    break;

			case 1 :
			    switch ( world[x][y+1] )
				{
				    case 0 :
				    case 1 :
					y++;
					Set(x, y, 0);
					break;
				    case 2 :
				    case 3 :
				    case 4 :
					c = FALSE;
					break;
				}
			    break;

			case 2 :
			    switch ( world[x-1][y] )
				{
				    case 0 :
				    case 1 :
					x--;
					Set(x, y, 0);
					break;
				    case 2 :
				    case 3 :
				    case 4 :
					c = FALSE;
					break;
				}
			    break;

			case 3 :
			    switch ( world[x+1][y] )
				{
				    case 0 :
				    case 1 :
					x++;
					Set(x, y, 0);
					break;
				    case 2 :
				    case 3 :
				    case 4 :
					c = FALSE;
					break;
				}
			    break;
		    } /* switch */

		if ( (rand() & 15) == 0 )
		    {
			if ( (rand() & 1) == 0 )
			    d += 2;
			else
			    d -= 2;

			if ( d < 0 )
			    d += 3;
			else if ( d > 3 )
			    d -= 3;
		    }
	    } /* for */
    }

void CreateWorld()
    {
	SHORT x, y;
	SHORT i;

	SetAPen(rp, 0);
	RectFill(rp, 5, wintop, 535, 134 + wintop);

	SetAPen(rp, 1);
	PrintAt(198, 69 + wintop, "Creating Map...");

	for ( x = 0 ; x <= worldx ; x++ )
	    {
		Set(x, 0, 4);
		Set(x, worldy, 4);
	    }

	for ( y = 1 ; y < worldy ; y++ )
	    {
		Set(0, y, 4);
		Set(worldx, y, 4);
		for ( x = 1 ; x < worldx ; x++ )
		    Set(x, y, 1);
	    }

	for ( i = worldx*worldy/400 ; i ; i-- )
	    {
		SHORT xd = (rand() % (worldx-9)) + 3;
		SHORT yd = (rand() % (worldy-9)) + 3;
		SHORT xs = (rand() & 3) + 3 + xd;
		SHORT ys = (rand() & 3) + 3 + yd;

		if ( xs > worldx-2 )
		    xs = worldx-2;

		if ( ys > worldy-2 )
		    ys = worldy-2;

		for ( x = xd ; x < xs ; x++ )
		    for ( y = yd ; y < ys ; y++ )
			Set(x, y, 3);

		x = xd + ( rand() % (xs-xd) );
		if ( rand() & 1 )
		    {
			if ( world[x][yd - 1] == 1 )
			    {
				Set(x, yd - 1, 2);
				CreateWay(x, yd - 1, 0);
			    }
		    }
		else
		    {
			if ( world[x][ys] == 1 )
			    {
				Set(x, ys, 2);
				CreateWay(x, ys, 1);
			    }
		    }

		y = yd + ( rand() % (ys-yd) );
		if ( rand() & 1 )
		    {
			if ( world[xd - 1][y] == 1 )
			    {
				Set(xd - 1, y, 2);
				CreateWay(xd - 1, y, 2);
			    }
		    }
		else
		    {
			if ( world[xs][y] == 1 )
			    {
				Set(xs, y, 2);
				CreateWay(xs, y, 3);
			    }
		    }
	    }

	for ( x = 1 ; x < worldx ; x++ )
	    for ( y = 1 ; y < worldy ; y++ )
		{
		    if ( world[x][y] == 3 )
			Set(x, y, 0);
		    else if ( world[x][y] == 4 )
			Set(x, y, 1);
		}

	for ( i = 0 ; i < 5 ; i++ )
	    {
		x = (rand() % (worldx-1)) + 1;
		y = (rand() % (worldy-1)) + 1;

		while ( world[x][y] != 0 )
		    {
			x++;
			if ( x >= worldx )
			    {
				x = 1;
				y++;
				if ( y >= worldy )
				    {
					x = 1;
					y = 1;
				    }
			    }
		    }

		Set(x, y, 3);
	    }
    }

/********************/
/* PutField()       */
/********************/

void PutField(__A0 struct Player *p, __D0 SHORT x, __D1 SHORT y)
    {
	SHORT xs = (x - p->xwin) * 20 + p->xoff;
	SHORT ys = (y - p->ywin) * 10 + 19 + wintop;

	if ( p->mask[x][y] )
	    if ( ((Player1.x == x) && (Player1.y == y)) || ((Player2.x == x) && (Player2.y == y)) )
		{
		    DrawImage(rp, &ManImage, xs, ys);
		}
	    else
		switch ( world[x][y] )
		    {
			case 0 :
			    DrawImage(rp, &FloorImage, xs, ys);
			    break;
			case 1 :
			    DrawImage(rp, &WallImage, xs, ys);
			    break;
			case 2 :
			    DrawImage(rp, &DoorImage, xs, ys);
			    break;
			case 3 :
			    DrawImage(rp, &ChestImage, xs, ys);
			    break;
		    }
	else
	    DrawImage(rp, &UnknownImage, xs, ys);
    }

/********************/
/* PrintAt()        */
/********************/

void PrintAt(__D0 SHORT x, __D1 SHORT y, __A0 char *t)
    {
	Move(rp, x, y);
	Text(rp, t, strlen(t));
    }

