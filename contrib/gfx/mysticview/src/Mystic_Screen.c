/*********************************************************************
----------------------------------------------------------------------

	MysticView
	screen

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
#include <proto/diskfont.h>

#include "Mystic_Screen.h"
#include "Mystic_Settings.h"


/*--------------------------------------------------------------------

	GetScreenAspect(screen, *aspectx, *aspecty)

--------------------------------------------------------------------*/

void GetScreenAspect(struct Screen *screen, int *aspectx, int *aspecty)
{
	ULONG modeID;
	*aspectx = 1;
	*aspecty = 1;

	if ((modeID = GetVPModeID(&screen->ViewPort)) != INVALID_ID)
	{
		DisplayInfoHandle dih;

		if((dih = FindDisplayInfo(modeID)))
		{
			struct DisplayInfo di;	

			if(GetDisplayInfoData(dih, (UBYTE*)&di, sizeof(struct DisplayInfo), DTAG_DISP, modeID))
			{
				*aspectx = (int) di.Resolution.x;
				*aspecty = (int) di.Resolution.y;
			}
		}
	}
}

/*********************************************************************
----------------------------------------------------------------------

	void DeleteMVScreen(mvscreen)

----------------------------------------------------------------------
*********************************************************************/

void DeleteMVScreen(struct mvscreen *scr)
{
	if (scr)
	{
		if (scr->screenfont)
		{
			CloseFont(scr->screenfont);
		}

		DeletePenShareMap(scr->psm);

		FreeVisualInfo(scr->visualinfo);

		if (scr->screen)
		{
			if (scr->screentype == CUSTOMSCREEN)
			{
				while ( (PubScreenStatus(scr->screen, PSNF_PRIVATE) & 1) == 0)
				{
					Delay(10);
				}
				CloseScreen(scr->screen);
			}
			else
			{			
				UnlockPubScreen(NULL, scr->screen);
			}
		}			
		Free(scr);
	}
}




/*********************************************************************
----------------------------------------------------------------------

	GetVisibleSize(mvscreen, &width, &height)

----------------------------------------------------------------------
*********************************************************************/

void GetVisibleSize(struct mvscreen *scr, int *width, int *height)
{
	if (scr)
	{
		DisplayInfoHandle dih;
		struct DimensionInfo di;	

		LOCK(scr);

		*width = scr->screen->Width;
		*height = scr->screen->Height;
		
		if((dih = FindDisplayInfo(scr->modeID)))
		{
			if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, scr->modeID))
			{
				*width = di.TxtOScan.MaxX - di.TxtOScan.MinX + 1;
				*height = di.TxtOScan.MaxY - di.TxtOScan.MinY + 1;
			}
		}

		UNLOCK(scr);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	struct mvscreen *CreateMVScreen(mvs)

----------------------------------------------------------------------
*********************************************************************/

struct mvscreen *CreateMVScreen(struct mainsettings *mvs)
{
	struct mvscreen *scr;

	if ((scr = Malloclear(sizeof(struct mvscreen))))
	{
		BOOL success = FALSE;

		InitSemaphore(&scr->semaphore);

		if (mvs->scropenmode == SCROPENMODE_CUSTOM)
		{
			ULONG id = INVALID_ID;
			UWORD width, height, depth;
			struct Screen *defscreen = NULL;
			struct ViewPort *vp = NULL;
		
			width = MINSCREENWIDTH;
			height = MINSCREENHEIGHT;
			depth = mvs->depth ? mvs->depth : MINSCREENDEPTH;


			if (mvs->modeID == INVALID_ID)
			{
				if ((defscreen = LockPubScreen(NULL)))
				{
					ULONG modeID;
					vp = &defscreen->ViewPort;
	
					width = defscreen->Width;
					height = defscreen->Height;
	
					if ((modeID = GetVPModeID(vp)) != INVALID_ID)
					{
						DisplayInfoHandle dih;
				
						if((dih = FindDisplayInfo(modeID)))
						{
							struct DimensionInfo di;	
				
							if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, modeID))
							{
								width = di.StdOScan.MaxX - di.StdOScan.MinX + 1;
								height = di.StdOScan.MaxY - di.StdOScan.MinY + 1;
								if (!mvs->depth)
								{
									depth = di.MaxDepth;
								}
							}
						}
					}
				}
			}
			else
			{
				DisplayInfoHandle dih;

				id = mvs->modeID;
				
				if((dih = FindDisplayInfo(id)))
				{
					struct DimensionInfo di;	
				
					if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, id))
					{
						width = di.StdOScan.MaxX - di.StdOScan.MinX + 1;
						height = di.StdOScan.MaxY - di.StdOScan.MinY + 1;
						if (!mvs->depth)
						{
							depth = di.MaxDepth;
						}
					}
				}
			}


			if (mvs->scrwidth > 0)
			{
				width = mvs->scrwidth;
			}
			
			if (mvs->scrheight > 0)
			{
				height = mvs->scrheight;
			}


			if (mvs->modeID == INVALID_ID)
			{
				if (CyberGfxBase && !mvs->hamscreen)
				{
					struct TagItem *taglist;
					if ((taglist = AllocateTagItems(10)))
					{
						struct TagItem *tp = taglist;
			
						inserttag(tp, CYBRBIDTG_NominalWidth, width);
						inserttag(tp, CYBRBIDTG_NominalHeight, height);
	
						if (mvs->depth)
						{
							inserttag(tp, CYBRBIDTG_Depth, RNG(MINSCREENDEPTH, mvs->depth, 32));
						}
						inserttag(tp, TAG_DONE, 0);
					
						id = BestCModeIDTagList(taglist);
			
						FreeTagItems(taglist);
					}
				}

				if (id == INVALID_ID)
				{
					struct TagItem *taglist;
					if ((taglist = AllocateTagItems(10)))
					{
						struct TagItem *tp = taglist;
			
						inserttag(tp, BIDTAG_NominalWidth, width);
						inserttag(tp, BIDTAG_NominalHeight, height);
	
						if (mvs->hamscreen)
						{
							inserttag(tp, BIDTAG_DIPFMustHave, DIPF_IS_HAM);
							depth = MIN(depth, 8);
						}
						else
						{
							inserttag(tp, BIDTAG_ViewPort, vp);
							if (mvs->depth)
							{
								inserttag(tp, BIDTAG_Depth, depth);
							}
						}
	
						inserttag(tp, TAG_DONE, 0);
			
						id = BestModeIDA(taglist);
						FreeTagItems(taglist);
					}
				}
			}


			if ((scr->screen = LockPubScreen(PROGNAME)))
			{
				scr->screentype = PUBLICSCREEN;
				success = TRUE;
				scr->modeID = GetVPModeID(&scr->screen->ViewPort);
			}
			else
			{
				if (id != INVALID_ID)
				{
					UWORD empty = 0xffff;
					
					if ((scr->screen = OpenScreenTags(NULL,
						SA_Width, width,
						SA_Height, height,
						SA_Depth, depth,
						SA_DisplayID, id, 
						SA_Type, PUBLICSCREEN,
						SA_AutoScroll, TRUE,
						SA_Title, (IPTR)PROGNAME,
						SA_PubName, (IPTR)PROGNAME,
						SA_ShowTitle, FALSE,
						SA_LikeWorkbench, TRUE,
						SA_FullPalette, TRUE,
						SA_SharePens, TRUE,
						SA_Pens, (IPTR)&empty,
						TAG_DONE)))
					{
						PubScreenStatus(scr->screen, 0);
						scr->screentype = CUSTOMSCREEN;
						scr->modeID = id;
						success = TRUE;
					}
				}
			}

			if (defscreen)
			{
				UnlockPubScreen(NULL, defscreen);
			}
		}

		//else
		
		if (!success)
		{

			if (!success)
			{
				if (mvs->scropenmode == SCROPENMODE_PUBLIC)
				{
					if ((scr->screen = LockPubScreen(mvs->pubscreen)))
					{
						ScreenToFront(scr->screen);
						success = TRUE;
					}
				}
			}
			
			if (!success)
			{
				success = !!(scr->screen = LockPubScreen(NULL));		
			}

			if (success)
			{
				scr->screentype = PUBLICSCREEN;
				scr->modeID = GetVPModeID(&scr->screen->ViewPort);
			}
		}

		if (success)
		{
			success = FALSE;
			if ((scr->visualinfo = GetVisualInfo(scr->screen, TAG_DONE)))
			{
				if ((scr->psm = CreatePenShareMap(GGFX_HSType, mvs->hstype, TAG_DONE)))
				{
					if (mvs->screenaspectx > 0 && mvs->screenaspecty > 0)
					{
						scr->aspectx = mvs->screenaspectx;
						scr->aspecty = mvs->screenaspecty;
					}
					else
					{
						GetScreenAspect(scr->screen, &scr->aspectx, &scr->aspecty);
					}
					success = TRUE;
				}
			}
		}

		if (success)
		{
			struct TextAttr fattr;
			fattr.ta_Name = (STRPTR) scr->screen->RastPort.Font->tf_Message.mn_Node.ln_Name;
			fattr.ta_YSize = scr->screen->RastPort.Font->tf_YSize;
			fattr.ta_Style = FS_NORMAL;
			fattr.ta_Flags = FPF_DESIGNED;

			if (mvs->fontspec)
			{
				char fontname[102];
				int fontysize;
				char fontstring[110];
			
			#ifdef __AROS__
				switch (sscanf(mvs->fontspec, "%d,%100s", &fontysize, fontname))
			#else
				switch (sscanf(mvs->fontspec, "%lu,%100s", &fontysize, fontname))
			#endif
				{
					case 2:
						sprintf(fontstring, "%s.font", fontname);
						fattr.ta_Name = fontstring;
						fattr.ta_YSize = fontysize;
						break;
					case 1:
						fattr.ta_YSize = fontysize;
						break;
					default:
						break;
				}


				if (DiskFontBase)
				{
					scr->screenfont = OpenDiskFont(&fattr);
				}
				else
				{
					scr->screenfont = OpenFont(&fattr);
				}
			}

			if (!scr->screenfont)
			{
				fattr.ta_Name = (STRPTR) scr->screen->RastPort.Font->tf_Message.mn_Node.ln_Name;
				fattr.ta_YSize = scr->screen->RastPort.Font->tf_YSize;
				fattr.ta_Style = FS_NORMAL;
				fattr.ta_Flags = 0;
				scr->screenfont = OpenFont(&fattr);	
			}
		}
		else
		{
			DeleteMVScreen(scr);
			scr = NULL;
		}
	}

	return scr;
}
