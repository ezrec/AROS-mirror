/*********************************************************************
----------------------------------------------------------------------

	MysticView
	initexit

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
#include <libraries/commodities.h>
#include <libraries/asl.h>
#include <libraries/mysticview.h>

#include <exec/memory.h>

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
#include <proto/commodities.h>
#ifndef NO_NEWICONS
#include <proto/newicon.h>
#endif

#include <clib/alib_protos.h>

#include "Mystic_InitExit.h"
#include "Mystic_Screen.h"
#include "Mystic_Window.h"
#include "Mystic_Settings.h"
#include "Mystic_Tools.h"
#include "Mystic_Subtask.h"
#include "Mystic_Functions.h"
#include "Mystic_Icons.h"

#include <assert.h>

/*********************************************************************
----------------------------------------------------------------------

	DeleteMView(mv)

----------------------------------------------------------------------
*********************************************************************/

void DeleteMView(struct mview *mv)
{
	if (mv)
	{
		CloseSubTask(mv->abouttask);

/*
		if (mv->appiconthumbnail)
		{
			struct NewDiskObject *dob = mv->appiconthumbnail->dob;
			DeleteIconThumbnail(mv->appiconthumbnail);
			if (dob)
			{
				FreeNewDiskObject(dob);
			}
		}
*/

		if (mv->appicon)
		{
			RemoveAppIcon(mv->appicon);
		}
		if (mv->appitem)
		{
			RemoveAppMenuItem(mv->appitem);
		}

		if (mv->diskobject)
		{
			FreeDiskObject(mv->diskobject);
		}

		{
			//!!!	DeleteStringList(mv->pubscreenlist);

			char **p = mv->pubscreenlist;
			if (p)
			{
				while (*p)
				{
					if ((STRPTR) *p != NM_BARLABEL)
					{
						Free(*p);
					}
					p++;
				}
				Free(mv->pubscreenlist);
			}
		}
	
		if (mv->newbroker)
		{
			Free(mv->newbroker->nb_Name);
			Free(mv->newbroker);
		}

		ActivateCxObj(mv->broker, FALSE);
		DeleteCxObjAll(mv->broker);
		DeletePort(mv->brokermsgport);

		FreeAslRequest(mv->renamefreq);
		FreeAslRequest(mv->copyfreq);
		FreeAslRequest(mv->savefreq);
		FreeAslRequest(mv->startpicfreq);
		FreeAslRequest(mv->presetfreq);
		FreeAslRequest(mv->destfreq);
		FreeAslRequest(mv->listfreq);
		FreeAslRequest(mv->freq);
		FreeAslRequest(mv->modereq);
		DeletePicture(mv->logopic);
		DeletePicture(mv->buttonpic);
		DeletePicture(mv->animpic);

		DeleteMVWindow(mv, mv->window);
		DeleteMVScreen(mv->screen);

		DeleteMsgPort(mv->appmsgport);

		Free(mv);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	mv = CreateMView (startsettings, char *progfilename)

----------------------------------------------------------------------
*********************************************************************/

struct mview *CreateMView(struct mainsettings *mvs, char *progfilename)
{
	BOOL success = FALSE;
	
	struct mview *mv;

	if (mv = Malloclear(sizeof(struct mview)))
	{
		InitSemaphore(&mv->semaphore);
		mv->progfilename = progfilename;

		if (mv->appmsgport = CreateMsgPort())
		{

			mv->appSignal = 1L << mv->appmsgport->mp_SigBit;

			success = TRUE;
			
			if (success)
			{
				if (!(mv->renamefreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->freq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->destfreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->listfreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->presetfreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->startpicfreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->modereq = AllocAslRequestTags(ASL_ScreenModeRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->savefreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}

			if (success)
			{
				if (!(mv->copyfreq = AllocAslRequestTags(ASL_FileRequest, NULL))) success = FALSE;
			}


			if (success)
			{
				if (_Stricmp(mvs->startpic, "none") != 0)
				{
					if (mvs->startpic)
					{
						mv->logopic = LoadPicture(mvs->startpic, NULL);
					}
					
					if (!mv->logopic)
					{							
						MysticLogoPalette[0] = mvs->bgcolor;
						
						mv->logopic = MakePicture(MysticLogo, LOGOWIDTH, LOGOHEIGHT,
							GGFX_PixelFormat, PIXFMT_CHUNKY_CLUT, 
							GGFX_Palette, MysticLogoPalette,
							GGFX_PaletteFormat, PALFMT_RGB8,
							GGFX_Independent, TRUE,
							GGFX_NumColors, LOGONUMCOLORS, TAG_DONE);
					}
				}
				
				if (mvs->buttonpic)
				{
					mv->buttonpic = LoadPicture(mvs->buttonpic, NULL);
				}
				
				if (!mv->buttonpic)
				{
					mv->buttonpic = MakePicture(MysticButtons, BUTTONSWIDTH, BUTTONSHEIGHT,
						GGFX_PixelFormat, PIXFMT_CHUNKY_CLUT, 
						GGFX_Palette, MysticButtonsPalette,
						GGFX_PaletteFormat, PALFMT_RGB8,
						GGFX_Independent, TRUE,
						GGFX_NumColors, BUTTONSNUMCOLORS, TAG_DONE);
				}


				if (mvs->animpic)
				{
					mv->animpic = LoadPicture(mvs->animpic, NULL);
				}
				
				if (!mv->animpic)
				{
					mv->animpic = MakePicture(MysticAnim, ANIMWIDTH, ANIMHEIGHT*ANIMFRAMES,
						GGFX_PixelFormat, PIXFMT_CHUNKY_CLUT,
						GGFX_Palette, MysticAnimPalette,
						GGFX_PaletteFormat, PALFMT_RGB8,
						GGFX_Independent, TRUE,
						GGFX_NumColors, ANIMNUMCOLORS, TAG_DONE);
				}
				
				if (!mv->buttonpic || !mv->animpic)
				{
					success = FALSE;
				}
			}
		}
	}


	if (success)
	{
		success = FALSE;

		if (mv->brokermsgport = CreateMsgPort())
		{
			if (mv->newbroker = Malloclear(sizeof(struct NewBroker)))
			{
				char tempname[100];
				int count = 1;

				mv->newbroker->nb_Version = NB_VERSION;
				mv->newbroker->nb_Title = PROGNAME PROGVERSION "";
				mv->newbroker->nb_Descr = "picture viewer";
				mv->newbroker->nb_Unique = NBU_UNIQUE;
				mv->newbroker->nb_Flags = COF_SHOW_HIDE;
				mv->newbroker->nb_Port = mv->brokermsgport;
				mv->newbroker->nb_Pri = mvs->cx_priority;
				
				strcpy(tempname, PROGNAME);
				
				while (count < 64)		// better we limit the show to something reasonable...
				{
					Free(mv->newbroker->nb_Name);
					if (mv->newbroker->nb_Name = _StrDup(tempname))
					{
						if (mv->broker = CxBroker(mv->newbroker, NULL))
						{
							CxObj *filter, *sender, *translate;
						
							if (filter = CxFilter(mvs->hotkey))
							{
								AttachCxObj(mv->broker, filter);
								if (sender = CxSender(mv->brokermsgport, EVT_HOTKEY))
								{
									AttachCxObj(filter, sender);
									if (translate = (CxTranslate(NULL)))
									{
										AttachCxObj(filter, translate);
										if (! CxObjError(filter))
										{
											ActivateCxObj(mv->broker, TRUE);
											success = TRUE;

											mv->diskobject = GetDiskObject(mv->progfilename);
											
											if (mvs->appiconname)
											{
												mv->iconifyicon = GetDiskObject(mvs->appiconname);
											}
											
											MVFunction_SetHideMode(mv, mvs, NULL);
										}
									}
								}
							}
							break;
						}
						else
						{
							sprintf(tempname, PROGNAME ".%d", ++count);
						}
					}
					else
					{
						break;
					}
				}
				
			}
		}
	}


	if (!success)
	{
		DeleteMView(mv);
		mv = NULL;
	}

	return mv;
}

