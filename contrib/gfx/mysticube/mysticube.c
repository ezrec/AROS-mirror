/*********************************************************************
----------------------------------------------------------------------

	MystiCube 1.0
	© 1997 captain bifat / TEK neoscientists

	drag'n'drop picture viewer
	a simple demonstration for guigfx.library

	many parameters are fixed, modify them
	to suit your personal taste

	contact me at
	timm.mueller@darmstadt.netsurf.de
	
	visit my support site at
	tek.flynet.de/binarydistillery/index.html

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <guigfx/guigfx.h>
#include <devices/timer.h>

#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/guigfx.h>

#include "mysticube.h"

#include "timer.h"

/*------------------------------------------------------------------*/

long __stack = 20000;

char versionstring[] = "$VER: " PROGNAME VERSION "";


/*********************************************************************
----------------------------------------------------------------------

	global

----------------------------------------------------------------------
*********************************************************************/

LONG timersec, timermicro;

void *timerstart()
{
    CurrentTime(&timersec, &timermicro);    
    return (void *)1;
}

unsigned long timerstop(void *handle)
{
    LONG sec, micro;
    long ret;
    
    CurrentTime(&sec, &micro);
    
    ret = (sec - timersec) * 1000 + (micro - timermicro) / 1000;

    return ret;
}

void CloseGlobal(void)
{
	CloseLibrary(GuiGFXBase);
	GuiGFXBase = NULL;
}

BOOL InitGlobal(void)
{
	GuiGFXBase = OpenLibrary("guigfx.library", GUIGFX_VERSION);

	if (!GuiGFXBase)
	{
		GuiGFXBase = OpenLibrary("libs/guigfx.library", GUIGFX_VERSION);
	}

	if (GuiGFXBase)
	{
		return TRUE;
	}

	CloseGlobal();
	
	return FALSE;
}


/*********************************************************************
----------------------------------------------------------------------

	void UpdateWindowParameters(mvwindow)

	get current window parameters

----------------------------------------------------------------------
*********************************************************************/

void UpdateWindowParameters(struct mvwindow *win)
{
	win->winleft = win->window->LeftEdge;
	win->wintop = win->window->TopEdge;
	win->winwidth = win->window->Width;
	win->winheight = win->window->Height;
	win->innerleft = win->window->BorderLeft;
	win->innertop = win->window->BorderTop;
	win->innerwidth = win->winwidth - win->innerleft - win->window->BorderRight;
	win->innerheight = win->winheight - win->innertop - win->window->BorderBottom;
}


/*********************************************************************
----------------------------------------------------------------------

	DeleteMVWindow (mvwindow)

----------------------------------------------------------------------
*********************************************************************/

void DeleteMVWindow (struct mvwindow *win)
{
	if (win)
	{
		if (win->window)
		{
			Forbid();

			if (win->appwindow && win->appmsgport)
			{
				struct AppMessage *appmsg;
			
				while ((appmsg = (struct AppMessage *) GetMsg(win->appmsgport)))
				{
					ReplyMsg((struct Message *) appmsg);
				}
			
				RemoveAppWindow(win->appwindow);
			}
	
			DeleteMsgPort(win->appmsgport);

			Permit();

			CloseWindow(win->window);
		}
		
		free(win);
	}
}



/*********************************************************************
----------------------------------------------------------------------

	mvwindow = CreateMVWindow (screen)

----------------------------------------------------------------------
*********************************************************************/

#define inserttag(x,t,d) {(x)->ti_Tag=(t);((x)++)->ti_Data=(ULONG)(d);}

struct mvwindow *CreateMVWindow (struct Screen *scr)
{
	struct mvwindow *win;
	
	if ((win = malloc(sizeof(struct mvwindow))))
	{
		BOOL success = FALSE;
		struct TagItem *taglist;

		memset(win, 0, sizeof(struct mvwindow));

		win->screen = scr;

		if((win->appmsgport = CreateMsgPort()))
		{
			win->appSignal = 1L << win->appmsgport->mp_SigBit;

			if((taglist = AllocateTagItems(20)))
			{
				UWORD visibleWidth, visibleHeight, visibleMidX, visibleMidY;
				UWORD visibleLeft, visibleTop;
				WORD winwidth, winheight, wintop, winleft;
				struct TagItem *tp = taglist;
				ULONG modeID;
		
				visibleWidth = scr->Width;
				visibleHeight = scr->Height;
		
				if ((modeID = GetVPModeID(&scr->ViewPort)) != INVALID_ID)
				{
					DisplayInfoHandle dih;
			
					if((dih = FindDisplayInfo(modeID)))
					{
						struct DimensionInfo di;	
			
						if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, modeID))
						{
							visibleWidth = di.TxtOScan.MaxX - di.TxtOScan.MinX;
							visibleHeight = di.TxtOScan.MaxY - di.TxtOScan.MinY;
						}
					}
				}
		
				visibleLeft = -scr->ViewPort.DxOffset;
				visibleTop = -scr->ViewPort.DyOffset;

				visibleMidX = visibleWidth/2 - scr->ViewPort.DxOffset;
				visibleMidY = visibleHeight/2 - scr->ViewPort.DyOffset;
		
				winwidth = visibleWidth / 2;
				winheight = visibleHeight / 2;
				inserttag(tp, WA_Width, winwidth);
				inserttag(tp, WA_Height, winheight);
		
				winleft = visibleMidX - winwidth/2;
				wintop = visibleMidY - winheight/2;
				inserttag(tp, WA_Left, winleft);
				inserttag(tp, WA_Top, wintop);
	
				win->otherwinpos[0] = visibleLeft;
				win->otherwinpos[1] = visibleTop;
				win->otherwinpos[2] = visibleWidth;
				win->otherwinpos[3] = visibleHeight;

				inserttag(tp, WA_Zoom, &win->otherwinpos);

				inserttag(tp, WA_PubScreen, scr);
		
				inserttag(tp, WA_Title, DEFAULT_WINTITLE);
		
				inserttag(tp, WA_NewLookMenus, TRUE);
		
		
				inserttag(tp, WA_Flags, 
							WFLG_SIZEBBOTTOM | WFLG_DRAGBAR | 
							WFLG_SIZEGADGET | WFLG_DEPTHGADGET | WFLG_ACTIVATE |
							WFLG_CLOSEGADGET | WFLG_SIMPLE_REFRESH);

				inserttag(tp, WA_IDCMP, 
							IDCMP_CLOSEWINDOW | IDCMP_MENUPICK | 
							IDCMP_NEWSIZE | IDCMP_ACTIVEWINDOW |
							IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS);
			
				inserttag(tp, WA_MinWidth, DEFAULT_MINWIDTH);
				inserttag(tp, WA_MinHeight, DEFAULT_MINHEIGHT);
				inserttag(tp, WA_MaxWidth, DEFAULT_MAXWIDTH);
		 		inserttag(tp, WA_MaxHeight, DEFAULT_MAXHEIGHT);

				inserttag(tp, TAG_DONE, 0);
		
				if((win->window = OpenWindowTagList(NULL, taglist)))
				{
					win->idcmpSignal = 1L << win->window->UserPort->mp_SigBit;
					UpdateWindowParameters(win);

					win->appwindow = AddAppWindow(0, 0, win->window, win->appmsgport, NULL);

					success = TRUE;
				}
		
				FreeTagItems(taglist);
			}
		}
		
		if (!success)
		{
			DeleteMVWindow(win);
			win = NULL;
		}
	}

	return win;
}

#undef inserttag


/*********************************************************************
----------------------------------------------------------------------

	docube(cube, bufferpic, width, height)

	calculate coordinates, draw faces

----------------------------------------------------------------------
*********************************************************************/

void docube(struct cube *cube, PICTURE *bufferpic, int width, int height)
{
	struct face *face;

	double xcosinus, xsinus;
	double ycosinus, ysinus;
	double zcosinus, zsinus;
	double x, y, z, t;
	int i;
	

	if (bufferpic)
	{
		//	calculate 2d coordinates
	
		xcosinus = cos(cube->xrot);
		xsinus = sin(cube->xrot);
		ycosinus = cos(cube->yrot);
		ysinus = sin(cube->yrot);
		zcosinus = cos(cube->zrot);
		zsinus = sin(cube->zrot);

		for (i = 0; i < 8; ++i)
		{
			x = cube->coords3d[i].x;
			y = cube->coords3d[i].y;
			z = cube->coords3d[i].z;
			
			t = y;
			y = y * xcosinus - z * xsinus;
			z = t * xsinus + z * xcosinus;
	
			t = x;
			x = x * ycosinus + z * ysinus;
			z = z * ycosinus - t * ysinus;
		
			t = x;
			x = x * zcosinus - y * zsinus;
			y = t * zsinus + y * zcosinus;
		
			z = (z + DISTANCE) / ZOOM;
		
			x /= z;
			y /= z;
		
			cube->coords2d[i].x = x * width/2 + width/2;
			cube->coords2d[i].y = y * width/2 + height/2;
		}


		//	clear background
		
		DoPictureMethod(bufferpic, PICMTHD_SET, cube->bgcolor, TAG_DONE);


		//	draw faces

		for (i = 0; i < 6; ++i)
		{
			face = &cube->faces[i];
	
			face->edges[0] = cube->coords2d[face->a]; 
			face->edges[1] = cube->coords2d[face->b]; 
			face->edges[2] = cube->coords2d[face->c]; 
			face->edges[3] = cube->coords2d[face->d]; 

			//	check visibility

			if 	((	(face->edges[1].x - face->edges[0].x) *
					(face->edges[2].y - face->edges[0].y) -
					(face->edges[2].x - face->edges[0].x) *
					(face->edges[1].y - face->edges[0].y)) > 0)
			{

				face->visible = TRUE;

				//	draw texture-mapped face
			
				if (face->mappedtexturepic)
				{
					DoPictureMethod(bufferpic, PICMTHD_TEXTURE,
						(IPTR)face->mappedtexturepic, (IPTR)face->edges, NULL);
				}
				else
				{
					if (cube->mappeddefaultpic)
					{
						DoPictureMethod(bufferpic, PICMTHD_TEXTURE,
							(IPTR)cube->mappeddefaultpic, (IPTR)face->edges, NULL);
					}
				}
			}
			else
			{
				face->visible = FALSE;
			}
		}
	}
}



/*********************************************************************
----------------------------------------------------------------------

	rotatecube(cube, timescale)

----------------------------------------------------------------------
*********************************************************************/

void rotatecube(struct cube *cube, double timescale)
{
	static double pi2 = 6.283056;

	if (cube)
	{
		cube->xrot += XROT * timescale;
		while (cube->xrot > pi2 || cube->xrot < 0)
		{
			cube->xrot -= pi2;
		}
	
		cube->yrot += YROT * timescale;
		while (cube->yrot > pi2 || cube->yrot < 0)
		{
			cube->yrot -= pi2;
		}
	
		cube->zrot += ZROT * timescale;
		while (cube->zrot > pi2 || cube->zrot < 0)
		{
			cube->zrot -= pi2;
		}
	}
}


/*********************************************************************
----------------------------------------------------------------------

	is = pointinpoly(x,y, numedges, edges)

	check if a point is inside a polygon

	by Ken McElvain (slightly modified)

----------------------------------------------------------------------
*********************************************************************/

int __inline whichquad(point2d pt, point2d orig)
{
	if(pt.x < orig.x)
	{
		if (pt.y < orig.y)
			return 2;
		else
			return 1;
	}
	else
	{
		if(pt.y < orig.y)
			return 3;
		else
			return 0;
	}
}

BOOL pointinpoly(int x, int y, int cnt, point2d *polypts)
{
	int	oldquad, newquad;
	point2d	pt, thispt, lastpt;
	int	a, b, i;
	int	wind = 0;       // current winding number

	pt.x = x;
	pt.y = y;

	lastpt = polypts[cnt-1];
	oldquad = whichquad(lastpt,pt); /* get starting angle */
	for(i=0;i<cnt;i++) { /* for each point in the polygon */
	        thispt = polypts[i];
	        newquad = whichquad(thispt,pt);
	        if(oldquad!=newquad) { /* adjust wind */
	                /*
	                 * use mod 4 comparsions to see if we have
	                 * advanced or backed up one quadrant 
	                 */
	                if (((oldquad+1)&3)==newquad) wind++;
	                else if (((newquad+1)&3)==oldquad) wind--;
	                else {
	                        /*
	                         * upper left to lower right, or
	                         * upper right to lower left. Determine
	                         * direction of winding  by intersection
	                         *  with x==0.
	                         */
	                        a = lastpt.y - thispt.y;
	                        a *= (pt.x - lastpt.x);
	                        b = lastpt.x - thispt.x;
	                        a += lastpt.y * b;
	                        b *= pt.y;
	
	                        if(a > b) wind += 2;
	                        else wind -= 2;
	                }
	        }
	        lastpt = thispt;
	oldquad = newquad;
	}

	return (BOOL) (wind != 0);

}


/*********************************************************************
----------------------------------------------------------------------

	nr = findface(cube, x, y)

	find the face that is hit throughout (x|y)

----------------------------------------------------------------------
*********************************************************************/

int findface(struct cube *cube, int x, int y)
{
	int facenr = -1;
	struct face *face;
	int i;

	if (cube)
	{
		for (i = 0; i < 6; ++i)
		{
			face = &cube->faces[i];

			if (face->visible)
			{
				if (pointinpoly(x,y, 4, face->edges))
				{
					facenr = i;
					break;
				}
			}
		}
	}

	return facenr;
}


/*********************************************************************
----------------------------------------------------------------------

	releasecube(cube)

	release the cube's drawhandle
	and free mapped pictures

----------------------------------------------------------------------
*********************************************************************/

void releasecube(struct cube *cube)
{
	if (cube)
	{
		int i;
	
		ReleaseDrawHandle(cube->drawhandle);
		cube->drawhandle = NULL;
		
		for (i = 0; i < 6; ++i)
		{
			DeletePicture(cube->faces[i].mappedtexturepic);
			cube->faces[i].mappedtexturepic = NULL;
		}

		DeletePicture(cube->mappeddefaultpic);
		cube->mappeddefaultpic = NULL;
	}
}



/*********************************************************************
----------------------------------------------------------------------

	drawhandle = obtaincube(cube, screen, window)
	
	compose and obtain a drawhandle for the cube

----------------------------------------------------------------------
*********************************************************************/

APTR obtaincube(struct cube *cube, struct Screen *scr, struct Window *win)
{
	APTR drawhandle = NULL;

	if (cube)
	{
		int i;

		//	release old drawhandle and delete mapped pictures

		releasecube(cube);

		//	obtain new drawhandle
		
		if ((drawhandle = cube->drawhandle = ObtainDrawHandle(cube->psm, win->RPort,
			scr->ViewPort.ColorMap, NULL)))
		{
			//	map pictures to the new drawhandle
		
			for (i = 0; i < 6; ++i)
			{
				if ((cube->faces[i].mappedtexturepic = ClonePicture(cube->faces[i].texturepic, NULL)))
				{
					DoPictureMethod(cube->faces[i].mappedtexturepic,
						PICMTHD_MAPDRAWHANDLE, (IPTR)drawhandle, NULL);
				}
			}

			//	map the default picture to the new drawhandle

			cube->mappeddefaultpic = ClonePicture(cube->defaultpic, NULL);
			DoPictureMethod(cube->mappeddefaultpic, PICMTHD_MAPDRAWHANDLE, (IPTR)drawhandle, NULL);
		}

	}

	return drawhandle;
}


/*********************************************************************
----------------------------------------------------------------------

	deletecube(cube)

----------------------------------------------------------------------
*********************************************************************/

void deletecube(struct cube *cube)
{
	if (cube)
	{
		releasecube(cube);
		DeletePenShareMap(cube->psm);
		free(cube);	
	}
}


/*********************************************************************
----------------------------------------------------------------------

	createcube(defaulttexture, bgcolor)

----------------------------------------------------------------------
*********************************************************************/

struct cube *createcube(PICTURE *defaultpic, ULONG bgcolor)
{
	BOOL success = FALSE;

	static point3d coords[8] =
	{
		{-1,-1,1},	{1,-1,1},	{1,1,1},	{-1,1,1},
		{-1,-1,-1}, {1,-1,-1},	{1,1,-1},	{-1,1,-1}
	};

	static int faceedges[] =
	{
		0,3,2,1,
		1,2,6,5,
		0,1,5,4,
		7,6,2,3,
		4,5,6,7,
		0,4,7,3
	};

	struct cube *cube;
	
	if ((cube = malloc(sizeof(struct cube))))
	{
		int i;

		memset(cube, 0, sizeof(struct cube));


		//	init 3d coordinates
		
		for (i = 0; i < 8; ++i)
		{
			cube->coords3d[i] = coords[i];
		}

		//	init edge indices
		
		for (i = 0; i < 6; ++i)
		{
			cube->faces[i].a = faceedges[i*4];
			cube->faces[i].b = faceedges[i*4+1];
			cube->faces[i].c = faceedges[i*4+2];
			cube->faces[i].d = faceedges[i*4+3];
		}

		cube->xrot = 0;
		cube->yrot = 0;
		cube->zrot = 0;

		cube->bgcolor = bgcolor;
		cube->defaultpic = defaultpic;

		//	create pen-sharemap
		
		if ((cube->psm = CreatePenShareMap(GGFX_HSType, HISTOGRAMTYPE, TAG_DONE)))
		{
			//	add the default picture to the pen-sharemap
		
			if (AddPicture(cube->psm, cube->defaultpic, NULL))
			{
				//	add the background color to the pen-sharemap
			
				if (AddPalette(cube->psm, &cube->bgcolor,
					GGFX_NumColors, 1, TAG_DONE))
				{
					success = TRUE;
				}
			}
		}
	}

	if (!success)
	{
		deletecube(cube);
		cube = NULL;
	}

	return cube;
}



/*********************************************************************
----------------------------------------------------------------------

	setcubetexture(cube, facenr, picture)

	set a new picture for the specified face

----------------------------------------------------------------------
*********************************************************************/

void setcubetexture(struct cube *cube, int nr, PICTURE *pic)
{
	if (cube && (nr >= 0 && nr < 6))
	{
		//	remove the old picture's colorhandle from the cube's pen-sharemap	

		RemColorHandle(cube->faces[nr].colorhandle);
		cube->faces[nr].colorhandle = NULL;

		//	delete the mapped picture

		DeletePicture(cube->faces[nr].mappedtexturepic);
		cube->faces[nr].mappedtexturepic = NULL;

		if (pic)
		{
			cube->faces[nr].texturepic = pic;

			//	add the new picture to the cube's pen-sharemap
		
			cube->faces[nr].colorhandle = AddPicture(cube->psm, pic, NULL);
		}
	}
}


/*********************************************************************
----------------------------------------------------------------------

	mysticube
	mainloop

----------------------------------------------------------------------
*********************************************************************/

void mysticube(struct mvwindow *win, struct cube *cube)
{
	APTR timehandle = NULL;
	int time;
	char text[30];
	BOOL timer = FALSE;
	
	PICTURE *bufferpic = NULL;
	APTR drawhandle;
	struct AppMessage *appmsg;
	BOOL finish = FALSE;
	ULONG signals;
	struct IntuiMessage *imsg;
	int i;

	PICTURE *picture[6] = {NULL, NULL, NULL, NULL, NULL, NULL};


	SetABPenDrMd(win->window->RPort, 1,2, JAM2);


	drawhandle = obtaincube(cube, win->screen, win->window);


	do
	{
		//	allocate draw buffer
	
		if (!bufferpic && drawhandle)
		{
			UpdateWindowParameters(win);
			if ((bufferpic = MakePicture(NULL, win->innerwidth, win->innerheight, NULL)))
			{
				//	map the draw buffer to the current drawhandle
			
				DoPictureMethod(bufferpic, PICMTHD_MAPDRAWHANDLE, (IPTR)drawhandle, NULL);
			}

			RefreshWindowFrame(win->window);
		}


		//	get signals
	
		signals = SetSignal(0, win->idcmpSignal | win->appSignal);


		//	IDCMP message loop

		if (signals & win->idcmpSignal)
		{
			while ((imsg = (struct IntuiMessage *) GetMsg(win->window->UserPort)))
			{
				ULONG iclass = imsg->Class;
				ULONG icode = imsg->Code;
		
				ReplyMsg((struct Message *) imsg);
		
				switch (iclass)
				{
					case IDCMP_CLOSEWINDOW:
						finish = TRUE;
						break;
		
					case IDCMP_NEWSIZE:
						if ((win->window->Width != win->winwidth) ||
							(win->window->Height != win->winheight))
						{
							DeletePicture(bufferpic);
							bufferpic = NULL;
						}
						break;

					case IDCMP_VANILLAKEY:
						switch (icode)
						{
							case 27:
								finish = TRUE;
								break;
							case 't':
								timer = timer ? FALSE : TRUE;
								break;
						}
				}
		
			}
		}


		//	check for pictures being dropped onto the main window

		if (signals & win->appSignal)
		{
			int x = -1, y = -1;

			while ((appmsg = (struct AppMessage *) GetMsg(win->appmsgport)))
			{
				struct WBArg *argptr = appmsg->am_ArgList;
				BOOL foundpic = FALSE;
				char pathname[MAXPATHNAMELEN];
				char filename[MAXFILENAMELEN];
		
				for (i = 0; i < appmsg->am_NumArgs && foundpic == FALSE; i++)
				{
					if (NameFromLock(argptr->wa_Lock, pathname, MAXPATHNAMELEN))
					{
						if(strlen(argptr->wa_Name))
						{
							strncpy(filename, argptr->wa_Name, MAXFILENAMELEN);
							foundpic = TRUE;
							UpdateWindowParameters(win);
							x = appmsg->am_MouseX - win->innerleft;
							y = appmsg->am_MouseY - win->innertop;
						}
					}
					argptr++;		// next argument
				}
				ReplyMsg((struct Message *) appmsg);
	
				if (foundpic)
				{
					//	get the surface number that has been hit
				
					int nr = findface(cube, x, y);
				
					if (nr >= 0)
					{
						char fullname[MAXFILENAMELEN+MAXPATHNAMELEN];
						PICTURE *newpic;
		
						strcpy(fullname, pathname);
						AddPart(fullname, filename, MAXFILENAMELEN+MAXPATHNAMELEN);

						//	load and set new picture
		
						if ((newpic = LoadPicture(fullname, NULL)))
						{
							DeletePicture(picture[nr]);
							picture[nr] = newpic;
							DoPictureMethod(picture[nr], PICMTHD_SCALE, 128, 128, NULL);
							setcubetexture(cube, nr, picture[nr]);
							RectFill(win->window->RPort, win->innerleft, win->innertop, win->innerleft + win->innerwidth - 1, win->innertop + win->innerheight - 1);
							drawhandle = obtaincube(cube, win->screen, win->window);
							DeletePicture(bufferpic);
							bufferpic = NULL;
							if (timehandle) timerstop(timehandle);
							timehandle = NULL;
						}
					}
				}
			}
		}

		//	get 1/1000 seconds since last frame

		if (timehandle)
		{
			time = timerstop(timehandle);
		}
		else
		{
			time = 0;
		}


		//	rotate cube
		
		rotatecube(cube, (double) time / 20);


		//	start timer

		timehandle = timerstart();


		//	draw cube

		if (bufferpic)
		{
			docube(cube, bufferpic, win->innerwidth, win->innerheight);
			DrawPicture(drawhandle, bufferpic, win->innerleft, win->innertop, NULL);

			if (timer)
			{
				//	display frames per second
	
				Move(win->window->RPort, win->innerleft, 
					win->innertop + win->innerheight - win->window->RPort->TxHeight + win->window->RPort->TxBaseline);
				sprintf(text, "fps: %ld", (ULONG) ((double) 1000 / (double) time));
				Text(win->window->RPort, text, strlen(text));
			}
		}


	} while (finish == FALSE);


	//	free pictures

	for (i = 0; i < 6; ++i)
	{
		DeletePicture(picture[i]);
	}

	//	free cube

	DeletePicture(bufferpic);
	releasecube(cube);
}



/*********************************************************************
----------------------------------------------------------------------

	main

----------------------------------------------------------------------
*********************************************************************/

int main (int argc, char **argv)
{
	ULONG result;

	if(InitGlobal())
	{
		struct mvwindow *window;
		struct Screen *defscreen;
		struct cube *cube;
		PICTURE *logopic;

		if ((defscreen = LockPubScreen(NULL)))
		{
			if ((window = CreateMVWindow(defscreen)))
			{
				if ((logopic = MakePicture(MysticLogo, LOGOWIDTH, LOGOHEIGHT,
					GGFX_Palette, (IPTR)MysticLogoPalette,
					GGFX_NumColors, LOGONUMCOLORS, TAG_DONE)))
				{
					if ((cube = createcube(logopic, BGCOLOR)))
					{
						SetTaskPri(FindTask(NULL), TASKPRIO);
						mysticube(window, cube);
						deletecube(cube);
						result = 0;
					}
					else
					{
						printf("*** out of memory\n");
						result = 20;
					}
					
					DeletePicture(logopic);
				}
				else
				{
					printf("*** out of memory\n");
					result = 20;
				}

				DeleteMVWindow(window);
			}
			else
			{
				printf("*** window could not be opened\n");
				result = 20;
			}

			UnlockPubScreen(NULL, defscreen);
		}
		else
		{
			printf("*** pubscreen could not be locked\n");
			result = 20;
		}
		
		CloseGlobal();
	}
	else
	{
		printf("*** global initialization failed\n");
		result = 20;
	}
	
	return result;
}
