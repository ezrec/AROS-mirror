/*************************************************
* Function to do tek 4010 graphics and mode
* switching, the function returns TRUE if it
* uses the character else the
* character can be used for something else
*************************************************/

#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;

                /* TEK Funktionen zu VT340 Emulator.. */

/* macros.. */

#define COLOR(i, j, k, l) SetRGB4(&con->TEKscreen->ViewPort, i, j, k, l)
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<=(b)?(a):(b))
#endif

/* this macro puts the tek screen at the back
 * and sets things so that the user gets the
 * non-tek mode of operation, there are some problems here
 * as we do not use intuition to control window positions */

/* the screen size */
#define XMIN 0
#define YMIN 0 
#define XMAX 640		/* changed from old tek version		*/
#define YMAX 256
#define COLORSET 1024

STATIC VOID TEK_off(struct VT340Console *, BOOL);
STATIC VOID TEK_reset(struct VT340Console *);


BOOL TEK_parse(struct VT340Console *con, UBYTE c)
{
#define DX 8
#define DY 10

/*
**	if(con->inTEK == FALSE)
**	{
**		if(c == GS)
**		{
**			con->inTEK = TRUE;
**
**			if(con->TEKscreen != NULL)
**			{
**				ScreenToFront(con->TEKscreen);
**				con->TEKmainmode = MOVE;
**			}
**		}
**		return((BOOL)con->inTEK); /* i.e. if c == GS we used it and can leave */
**	}
*/

	if(con->TEKscreen == NULL)
	{
		TEK_open(con);

		if(con->TEKscreen != NULL)
		{
			ScreenToFront(con->TEKscreen);
			con->TEKmainmode = MOVE;
		}
	}



/* first handle case if graph is sent without having the tek screen
 * turned on; just eat the graphics commands until the graph is
 * finished - then turn back to text mode
 * also make this selanar compatible
 */

	if(con->TEKscreen == NULL)
	{
		if(c == CAN  ||  (con->TEKlastc == ESC  &&  c == '2'))
			con->inTEK = FALSE;	 /* turn tek mode off	*/
		con->TEKlastc = c;
		return(TRUE);
	}


	if(con->TEKescmode != FALSE)
	{
		if(con->TEKcolormode > 0)
		{
			c = c - '0';
			con->TEKcolormode++;

			switch(con->TEKcolormode)
			{
				case 2:
					con->TEKcolorindex = c;
				break;

				case 3:
					con->TEKred = c;
				break;

				case 4:
					con->TEKgreen = c;
				break;

				case 5:
					COLOR(con->TEKcolorindex, con->TEKred, con->TEKgreen, (ULONG)c);
					con->TEKcolormode = 0;
					con->TEKescmode = FALSE;
				break;

			}
			return((BOOL)con->inTEK);
		}

		switch(c)
		{
	    /* Selanar Compatable graphics terminator */
			case '2':
 				TEK_off(con, TRUE);
				con->TEKboxmode = FALSE;
			break;

		/* I do not know what the tek 4100 area fill commands are so I made-up
 			my own, this will not harm the line drawing mode. */
			case 'A':
				con->TEKboxmode = TRUE;
			break;

			case 'B':
				con->TEKboxmode = FALSE;
			break;

			case 'Q':
				con->TEKcolormode = 1;
			return((BOOL)con->inTEK);

		/* another one of my own commands */
			case 'R':
			/* reset to default then clear screen */
				TEK_reset(con);
				con->TEKapen = 1;

			case FF:
				/* clear page */
 				con->TEKx = XMIN;
				con->TEKy = YMIN + DY;
				con->TEKmainmode = ALPHA;
				con->TEK4100 = NUL;
				SetRast(con->TEKrp, 0);
			break;

		/* looks like a 4100 command */
			case 'M':
				con->TEK4100 = 'M';
			break;
		}
		con->TEKescmode = FALSE;
	}
	else
	{
		if(con->TEK4100 != NUL)
		{
			if(con->TEK4100 == COLORSET)
				con->TEKapen = c - '0';
			SetAPen(con->TEKrp, con->TEKapen);
	  
			if(con->TEK4100 == 'M'  &&  c == 'L')  
				con->TEK4100 = COLORSET;
			else
				con->TEK4100 = NUL;
		}
		else
		{
			if(isprint(c))
			{
				if(con->TEKmainmode == ALPHA)
				{
					if(con->TEKx > XMAX-DX)
						con->TEKx = XMAX-DX;

					if(con->TEKx < XMIN)
						con->TEKx = XMIN;

					if(con->TEKy < YMIN+DY)
						con->TEKy = YMIN+DY;

					if(con->TEKy > YMAX)
						con->TEKy = YMAX;

					SetAPen(con->TEKrp, 1);
					Move(con->TEKrp, con->TEKx, con->TEKy);
					Text(con->TEKrp, &c, 1);
					SetAPen(con->TEKrp, con->TEKapen);
					con->TEKx += DX;
					if(con->TEKx > XMAX)
						con->TEKx = XMAX;		
				}
				else
				{
				/* a note here about 4014 graphics, If your graphics software
					drives a Tek 4014 then this will work perfecly well, you
					just will not be able to use the 4096 pixel resolution
					that that big storage tube device offers */

					WORD x, y;
					UBYTE data, tag;

					tag = (c & 0x60) >> 5;
					data = c & 0x1F;

					switch(tag)
					{
						case 1:	/* Hi-Y or Hi-X */
						{
							if(con->TEKlastTag == 3)
								con->TEKhiX = data << 5;
							else
								con->TEKhiY = data << 5;
						}
						break;

						case 2:		/* Lo-X */
						{
						/* low x always sent so don't save it */
							x = con->TEKhiX + data;
							y = con->TEKhiY + con->TEKloY;

							if(con->TEKscale != FALSE)
							{
								x = (x * XMAX + 512) / 1024;
								y = (y * YMAX + 512) / 1024;
	       				}

							if(con->TEKlace == FALSE)
							{
		       				x >>= 1;
								y >>= 1;
							}


							if(x > XMAX)
								x = XMAX;
							if(x < XMIN)
								x = XMIN;

							if(y > YMAX)
								y = YMAX;
							if(y < YMIN)
								y = YMIN;

							switch(con->TEKmainmode)
	     					{
								case MOVE:
									con->TEKmainmode = LINE;
									Move(con->TEKrp, x, y);
/*									KPrintF("MOVE: `x=%ld' `y=%ld'\r\n", x, y);*/
								break;

								case LINE:
									if(con->TEKboxmode)
									{
										RectFill(con->TEKrp,
														min(con->TEKx, x), 
														min(con->TEKy, y),
														max(con->TEKx, x),
														max(con->TEKy, y));
/*										KPrintF("RECT: `x=%ld' `y=%ld'\r\n", x, y);*/
	        						}
	        						else
	        						{
										Draw(con->TEKrp, x, y);
/*										KPrintF("LINE: `x=%ld' `y=%ld'\r\n", x, y);*/
	        						}
	     							break;

								case POINT:
									WritePixel(con->TEKrp, x, y);
/*									KPrintF("POINT: `x=%ld' `y=%ld'\r\n", x, y);*/
	      					break;

							}
							con->TEKx = x;
							con->TEKy = y;
	  					}
	  					break;

						case 3:		/* Lo-Y */
							con->TEKloY = data;
						break;
					}
					con->TEKlastTag = tag; 
				}
			}
			else
			{
				switch(c)
				{
					case BEL:
						con->io->xem_tbeep(1, 0);
					break;

					case BS:
 						con->TEKx -= DX;
						if(con->TEKx < XMIN)
							con->TEKx = XMIN;
 					break;

					case HT:
						con->TEKx += DX;
						if(con->TEKx > XMAX)
							con->TEKx = XMAX;
					break;

					case LF:
						con->TEKy += DY;
						if(con->TEKy > YMAX)
							con->TEKy = YMAX;
					break;

					case VT:
						con->TEKy -= DY;
						if(con->TEKy < (YMIN + DY))
							con->TEKy = YMIN + DY;
					break;

					case CR:
						con->TEKx = XMIN;
						con->TEKmainmode = ALPHA;
					break;

					case CAN:
						TEK_off(con, TRUE);
						con->TEKboxmode = FALSE;
					break;

					case SUB: /* GIN mode */
						con->TEKmainmode = GIN;
					break;

					case ESC:
						con->TEKescmode = TRUE;
					break;

					case FS: /* (point-plot) CTRL 4 */
						con->TEKmainmode = POINT;
					break;

					case GS: /* vector       CTRL 5 */
						con->TEKmainmode = MOVE;
					break;

					case RS: /* increment    CTRL 6 */
						con->TEKmainmode = INCREMENT;
					break;

					case US: /* alpha mode   CTRL 7 */
						con->TEKmainmode = ALPHA;
					break;

					default:
						;
					break;

				}
			}
		}
	}
	return(TRUE);
}


STATIC VOID TEK_reset(struct VT340Console *con)
{
	/* mess up the colors */
	COLOR( 0,  0,  0,  0);
	COLOR( 1, 15, 15, 15);
	COLOR( 2, 15,  0,  0);
	COLOR( 3,  0, 15,  0);
	COLOR( 4,  0,  0, 15);
	COLOR( 5,  0, 15, 15);
	COLOR( 6, 15,  0, 15);
	COLOR( 7, 15, 15,  0);
	COLOR( 8, 15,  8,  0);
	COLOR( 9,  8, 15,  0);
	COLOR(10,  0, 15,  8);
	COLOR(11,  0,  8, 15);
	COLOR(12,  8,  0, 15);
	COLOR(13, 15,  0,  8);
	COLOR(14,  5,  5,  5);
	COLOR(15, 10, 10, 10);

	SetRast(con->TEKrp, 0);
	SetAPen(con->TEKrp, 1); 
	Move(con->TEKrp, 0, 0);
}


STATIC VOID TEK_off(struct VT340Console *con, BOOL force)
{
	con->inTEK = (force != FALSE) ? FALSE : TRUE;
	ScreenToBack(con->TEKscreen);
}


BOOL TEK_open(struct VT340Console *con)
{
	STATIC struct NewScreen TekNewScreen = { 
		XMIN, YMIN,
		XMAX, YMAX, 1,
		0, 1,
		HIRES | LACE,
		CUSTOMSCREEN,
		NULL, NULL, NULL, NULL };

	STATIC struct NewWindow TekNewWindow = {
		XMIN, YMIN,
		XMAX, YMAX,
		1, 0,
		NULL,
		BORDERLESS | SMART_REFRESH,
		NULL, NULL, NULL, NULL, NULL,
		0, 0, 0, 0,
		CUSTOMSCREEN };


	if(con->TEKscreen != NULL)
		return(TRUE);

	con->TEKlace = TRUE;
	con->TEKscale= TRUE;
	

	con->TEKscreen = (struct Screen *)OpenScreen(&TekNewScreen);
	if(con->TEKscreen == NULL)
		return(FALSE);

	TekNewWindow.Screen = con->TEKscreen;
	con->TEKwindow = (struct Window *)OpenWindow(&TekNewWindow);
	if(con->TEKwindow == NULL)
	{
		TEK_close(con);
		return(FALSE);
	}
	con->TEKrp = con->TEKwindow->RPort;

	con->TEKx = XMIN;
	con->TEKy = YMIN + DY;

	con->TEKwindow->UserPort = con->io->xem_window->UserPort;
	ModifyIDCMP(con->TEKwindow, con->io->xem_window->IDCMPFlags);

/* allow for area fill */
	InitArea(&con->TEKai, con->buffer, 100);
	con->TEKrp->AreaInfo = &con->TEKai;
	con->TEKfillras = AllocRaster(XMAX, YMAX);
	InitTmpRas(&con->TEKtr, con->TEKfillras, RASSIZE(XMAX, YMAX)); 

	TEK_reset(con);
	TEK_off(con, FALSE);

	return(TRUE);
}


VOID TEK_close(struct VT340Console *con)
{
	con->TEKmainmode = FALSE;

	if(con->TEKfillras != NULL)
	{
		FreeRaster(con->TEKfillras, XMAX, YMAX);
		con->TEKfillras = NULL;
	}

	if(con->TEKwindow != NULL)
	{
		con->TEKwindow->UserPort = NULL; /* I will not close the commprogis port */
		CloseWindow(con->TEKwindow);
		con->TEKwindow = NULL;
	}

	if(con->TEKscreen != NULL)
	{
		CloseScreen(con->TEKscreen);
		con->TEKscreen = NULL;
	}
}


