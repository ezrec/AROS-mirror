/*********************************************************************
----------------------------------------------------------------------

	MysticView
	window

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>

#include <clib/macros.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>

#include "Mystic_Window.h"
#include "Mystic_InitExit.h"
#include "Mystic_Settings.h"
#include "Mystic_Debug.h"




/*********************************************************************
----------------------------------------------------------------------

	WindowBusy(mvwindow)
	WindowFree(mvwindow)

----------------------------------------------------------------------
*********************************************************************/

void WindowBusy(struct mvwindow *win)
{
	if (++win->windowbusycount == 1)
	{
		SetWindowPointer(win->window, WA_BusyPointer, TRUE, TAG_DONE);
	}
}

void WindowFree(struct mvwindow *win)
{
	if (--win->windowbusycount == 0)
	{
		SetWindowPointer(win->window, TAG_DONE);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	void UpdateWindowParameters(mvwindow)

----------------------------------------------------------------------
*********************************************************************/

void UpdateWindowParameters(struct mvwindow *win)
{
	if (win)
	{
		//LOCK(win);
/**	
		win->winleft = win->window->LeftEdge;
		win->wintop = win->window->TopEdge;
		win->winwidth = win->window->Width;
		win->winheight = win->window->Height;
		win->innerleft = win->window->BorderLeft;
		win->innertop = win->window->BorderTop;
		win->innerwidth = win->winwidth - win->innerleft - win->window->BorderRight;
		win->innerheight = win->winheight - win->innertop - win->window->BorderBottom;
**/
		win->winleft = win->window->LeftEdge;
		win->wintop = win->window->TopEdge;
		win->winwidth = win->window->Width;
		win->winheight = win->window->Height;
		win->innerleft = 0;	//win->window->BorderLeft;
		win->innertop = 0;	//win->window->BorderTop;
		win->innerwidth = win->winwidth - win->window->BorderLeft - win->window->BorderRight;
		win->innerheight = win->winheight - win->window->BorderTop - win->window->BorderBottom;
	
		//UNLOCK(win);
	}
}


/*--------------------------------------------------------------------

	RemoveAppWindowSafely(appwindow, appmsgport)

--------------------------------------------------------------------*/

void RemoveAppWindowSafely(struct AppWindow *appwin, struct MsgPort *appmsgport)
{
	if (appwin && appmsgport)
	{
		struct AppMessage *appmsg;
	
		Forbid();
	
		while (appmsg = (struct AppMessage *) GetMsg(appmsgport))
		{
			ReplyMsg((struct Message *) appmsg);
		}
	
		RemoveAppWindow(appwin);
	
		Permit();
	}
}


/*********************************************************************
----------------------------------------------------------------------

	DeleteMVWindow (mvwindow)

----------------------------------------------------------------------
*********************************************************************/

void DeleteMVWindow (struct mview *viewer, struct mvwindow *win)
{
	if (win)
	{
		WindowBusy(win);

		LOCK(win);

		//DeletePicture(win->winbackpic);

		Forbid();
		RemoveAppWindowSafely(win->appwindow, viewer->appmsgport);
	//	DeleteMsgPort(win->appmsgport);
		Permit();

		if (win->window)
		{
			ClearMenuStrip(win->window);
			ReleasePen(win->window->WScreen->ViewPort.ColorMap, win->backpen);
			CloseWindow(win->window);
		}

		if (win->menu)
		{
			FreeMenus(win->menu);
		}

		UNLOCK(win);
		
		Free(win);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	mvwindow = CreateMVWindow (mview, mvscreen, mainsettings)

----------------------------------------------------------------------
*********************************************************************/

struct mvwindow *CreateMVWindow (struct mview *mv, struct mvscreen *scr, struct mainsettings *mvs)
{
	struct mvwindow *win = NULL;

	if (scr && mvs)
	{	
		if (win = Malloclear(sizeof(struct mvwindow)))
		{
			BOOL success = FALSE;
			struct TagItem *taglist;
			
			win->backpen = -1;

	
			InitSemaphore(&win->semaphore);
	
	//		if (win->lastgadget = CreateContext(&win->gadgetlist))
	//		{
			//	if (win->appmsgport = CreateMsgPort())
			//	{
			//		win->appSignal = 1L << mv->appmsgport->mp_SigBit;
		
					if (taglist = AllocateTagItems(20))
					{
						UWORD visibleWidth, visibleHeight, visibleMidX, visibleMidY;
						UWORD visibleLeft, visibleTop;
						WORD winwidth, winheight, wintop, winleft;
						struct TagItem *tp = taglist;
						ULONG modeID;
						WORD winsizemode, winopenmode;
			
						winwidth = mvs->winwidth;
						winheight = mvs->winheight;
						winleft = mvs->winleft;
						wintop = mvs->wintop;
						winsizemode = mvs->winsizemode;
						winopenmode = mvs->winopenmode;
				
						visibleWidth = scr->screen->Width;
						visibleHeight = scr->screen->Height;
				
						if ((modeID = GetVPModeID(&scr->screen->ViewPort)) != INVALID_ID)
						{
							DisplayInfoHandle dih;
					
							if(dih = FindDisplayInfo(modeID))
							{
								struct DimensionInfo di;	
					
								if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, modeID))
								{
									visibleWidth = di.TxtOScan.MaxX - di.TxtOScan.MinX + 1;
									visibleHeight = di.TxtOScan.MaxY - di.TxtOScan.MinY + 1;
								}
							}
						}
		
						visibleLeft = -scr->screen->ViewPort.DxOffset;
						visibleTop = -scr->screen->ViewPort.DyOffset;
		
						visibleMidX = visibleWidth/2 - scr->screen->ViewPort.DxOffset;
						visibleMidY = visibleHeight/2 - scr->screen->ViewPort.DyOffset;
		
		
						if (visibleTop < scr->screen->BarHeight + 1)
						{
							visibleTop += scr->screen->BarHeight + 1;
							visibleHeight -= scr->screen->BarHeight + 1;
							visibleMidY += (scr->screen->BarHeight + 1)/2;
						}
		
						switch (winsizemode)
						{				
							case WSIZEMODE_HALF:
								winwidth = visibleWidth / 2;
								winheight = visibleHeight / 2;
								inserttag(tp, WA_Width, winwidth);
								inserttag(tp, WA_Height, winheight);
								break;
					
							case WSIZEMODE_66PERCENT:
								winwidth = visibleWidth * 2 / 3;
								winheight = visibleHeight * 2 / 3;
								inserttag(tp, WA_Width, winwidth);
								inserttag(tp, WA_Height, winheight);
								break;
			
							case WSIZEMODE_33PERCENT:
								winwidth = visibleWidth / 3;
								winheight = visibleHeight / 3;
								inserttag(tp, WA_Width, winwidth);
								inserttag(tp, WA_Height, winheight);
								break;
					
							case WSIZEMODE_VISIBLE:
								winwidth = visibleWidth;
								winheight = visibleHeight;
								inserttag(tp, WA_Width, winwidth);
								inserttag(tp, WA_Height, winheight);
								break;
					
							case WSIZEMODE_FULL:
								winwidth = scr->screen->Width;
								winheight = scr->screen->Height;
								inserttag(tp, WA_Width, winwidth);
								inserttag(tp, WA_Height, winheight);
								break;
								
							default:
								if(winwidth > 0) inserttag(tp, WA_Width, winwidth);
								if(winheight > 0) inserttag(tp, WA_Height, winheight);
								break;
						}
		
						switch (winopenmode)
						{
							case WOPENMODE_CENTER:
								winleft = visibleMidX - winwidth/2;
								wintop = visibleMidY - winheight/2;
								inserttag(tp, WA_Left, winleft);
								inserttag(tp, WA_Top, wintop);
								break;
					
							case WOPENMODE_MOUSE:
								winleft = scr->screen->MouseX - winwidth/2;
								wintop = scr->screen->MouseY - winheight/2;
								inserttag(tp, WA_Left, winleft);
								inserttag(tp, WA_Top, wintop);
								break;
					
							default:
								if(wintop > -1) inserttag(tp, WA_Top, wintop);
								if(winleft > -1) inserttag(tp, WA_Left, winleft);
								break;
						}
		
						win->otherwinpos[0] = visibleLeft;
						win->otherwinpos[1] = visibleTop;
						win->otherwinpos[2] = visibleWidth;
						win->otherwinpos[3] = visibleHeight;
		
						inserttag(tp, WA_Zoom, &win->otherwinpos);
		
		
						inserttag(tp, WA_PubScreen, scr->screen);
				
						if(!mvs->borderless)
						{
							inserttag(tp, WA_Title, DEFAULT_WINTITLE);
						}
				
						inserttag(tp, WA_NewLookMenus, TRUE);
				
				
						inserttag(tp, WA_Flags, 
									(mvs->borderless ? WFLG_BORDERLESS | WFLG_BACKDROP :
									WFLG_SIZEBBOTTOM | WFLG_DRAGBAR | WFLG_GIMMEZEROZERO |
									WFLG_SIZEGADGET | WFLG_DEPTHGADGET | WFLG_ACTIVATE |
									WFLG_CLOSEGADGET) | mvs->refreshmode | WFLG_REPORTMOUSE);
									
	
						win->idcmpmask = IDCMP_CLOSEWINDOW | IDCMP_MENUPICK | /** IDCMP_MENUVERIFY | **/
							IDCMP_NEWSIZE | IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_REFRESHWINDOW |
							IDCMP_VANILLAKEY | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS;
				
						inserttag(tp, WA_IDCMP, win->idcmpmask);

				//		inserttag(tp, WA_RMBTrap, TRUE);
				
						inserttag(tp, WA_MinWidth, DEFAULT_MINWIDTH);
						inserttag(tp, WA_MinHeight, DEFAULT_MINHEIGHT);
						inserttag(tp, WA_MaxWidth, DEFAULT_MAXWIDTH);
				 		inserttag(tp, WA_MaxHeight, DEFAULT_MAXHEIGHT);
		
						inserttag(tp, TAG_DONE, 0);
				
						if (win->menu = CreateMenus(mainmenu, NULL))
						{
							if(LayoutMenus(win->menu, scr->visualinfo, GTMN_NewLookMenus, TRUE, TAG_DONE))
							{
								if(win->window = OpenWindowTagList(NULL, taglist))
								{
									if (win->backpen = ObtainBestPen(scr->screen->ViewPort.ColorMap,
										RED_RGB32(mvs->bgcolor),
										GREEN_RGB32(mvs->bgcolor),
										BLUE_RGB32(mvs->bgcolor),
										OBP_Precision, PRECISION_ICON, TAG_DONE))
									{
									//	ULONG realrgb[3];
									//	GetRGB32(scr->screen->ViewPort.ColorMap, win->backpen, 1, realrgb);
									//	mvs->realbgcolor = 
									//		((realrgb[0] & 0xff000000) >> 8) +
									//		((realrgb[1] & 0xff000000) >> 16) +
									//		((realrgb[2] & 0xff000000) >> 24);
									
										if (scr->screenfont)
										{
											SetFont(win->window->RPort, scr->screenfont);
										}
									
										win->idcmpSignal = 1L << win->window->UserPort->mp_SigBit;
										UpdateWindowParameters(win);
				
										win->appwindow = AddAppWindow(0, 0, win->window, mv->appmsgport, NULL);
	
										
										SetMenuStrip(win->window, win->menu);
										
										ActivateWindow(win->window);
		
										success = TRUE;
									}
								}
							}
						}
				
						FreeTagItems(taglist);
					}
			//	}
	//		}
			
			if (!success)
			{
				DeleteMVWindow(mv, win);
				win = NULL;
			}
			
		}
	}

	return win;
}
