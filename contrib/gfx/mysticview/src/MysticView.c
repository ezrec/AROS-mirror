/*********************************************************************
----------------------------------------------------------------------

	MysticView

	todo:
		- dateikommentar?!
 		- overwrite requester! -> save list, save preset
		- "Drag'n'Drop Append"
		- viewheight <= 0 abfangen!
		- newpichandler(): pichandler zurücksetzen, nicht dauernd
		  neu erzeugen
		- picture statistiks

	not yet:
		- auf arrows klicken -> scroll
		- doppelklick zoom
		- im pip scrollen
		- cursor greifen

	ideen:
		- markieren
		- clearbutton
		- das neuronale netz über eine public semaphore allen
		  instanzen gleichzeitig zur verfügung stellen
		- "private" screen: scrollvport refresh usw. !
      - picture info konfigurierbar
		- database features (double, similar)
		- recent pfade
		- directory browse mode
		- shellkommando auf tasten
		- DEFAULTTOOL option

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <assert.h>

#include <dos/rdargs.h>
#include <exec/memory.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <utility/hooks.h>
#include <utility/tagitem.h>
#include <guigfx/guigfx.h>
#include <clib/macros.h>
#include <libraries/gadtools.h>
#ifndef NO_SCREENNOTIFY
#include <libraries/screennotify.h>
#endif
#include <libraries/commodities.h>
#include <libraries/mysticview.h>

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/render.h>
#include <proto/icon.h>
#include <proto/gadtools.h>
#ifndef NO_NEWICONS
#include <proto/newicon.h>
#endif
#include <proto/commodities.h>
#include <proto/wb.h>
#include <proto/mysticview.h>
#include <proto/graphics.h>
#ifndef NO_POPUPMENU
#include <proto/pm.h>
#endif
#include <proto/layers.h>

#ifndef NO_POPUPMENU
#include <libraries/pm.h>
#endif

#ifdef __MORPHOS__
#include <public/proto/libamiga/amiga_protos.h>
#else
#include <clib/alib_protos.h>
#endif

#include "Mystic_Global.h"
#include "Mystic_InitExit.h"
#include "Mystic_Subtask.h"
#include "Mystic_Tools.h"
#include "Mystic_Timing.h"
#include "Mystic_Window.h"
#include "Mystic_Screen.h"
#include "Mystic_Settings.h"
#include "Mystic_FileHandling.h"

#include "Mystic_HandlerWrapper.h"

#include "Mystic_Keyfile.h"
#include "Mystic_Icons.h"
#include "Mystic_Functions.h"

#include "Mystic_Texts.h"
#include "Mystic_Neuromancer.h"
#include "Mystic_Gadgets.h"

#include "MysticView.h"

#include "Mystic_Debug.h"

long __stack = MAINSTACK;



/*******************************************************************
--------------------------------------------------------------------

	various helper functions

--------------------------------------------------------------------
*******************************************************************/


/*------------------------------------------------------------------

	success = FormatInfoText(deststring, formatstring, args)

	formatstring:		args[0]	string	%p	pathname
						args[1] string	%f	filename
						args[2] int		%w	width
						args[3] int		%h	height
						args[4] int		%d	depth
						args[5] string	%n	formatname

------------------------------------------------------------------*/

BOOL FormatInfoText(char *deststring, char *formatstring, APTR *args)
{
	APTR argarray[10];
	char *fstring;
	BOOL error = TRUE;
	
	if ((fstring = _StrDup(formatstring)))
	{
		char *p = fstring, c, d;
		int argcount = 0;

		error = FALSE;
		
		while ((c = *p))
		{
			if (c == '%' && (d = p[1]))
			{
				switch (d)
				{
					case 'p':
						error = args[0] == NULL;
						argarray[argcount++] = args[0];
						p[1] = 's';
						break;
					case 'f':
						error = args[1] == NULL;
						argarray[argcount++] = args[1];
						p[1] = 's';
						break;
					case 'w':
						error = args[2] == 0;
						argarray[argcount++] = args[2];
						p[1] = 'd';
						break;
					case 'h':
						error = args[3] == 0;
						argarray[argcount++] = args[3];
						p[1] = 'd';
						break;
					case 'd':
						error = args[4] == 0;
						argarray[argcount++] = args[4];
						p[1] = 'd';
						break;
					case 'n':
						error = args[5] == NULL;
						argarray[argcount++] = args[5];
						p[1] = 's';
						break;
					default:
						break;
				}
			}
			p++;
		}

		if (deststring && !error)
		{		
			if (argcount <= 10)
			{
				sprintf(deststring, fstring, 
					argarray[0], argarray[1], argarray[2], argarray[3], argarray[4],
					argarray[5], argarray[6], argarray[7], argarray[8], argarray[9]);
			}
			else
			{
				*deststring = 0;
			}
		}

		Free(fstring);
	}
	
	return (BOOL)(!error);
}







/*------------------------------------------------------------------

	changes = FilterFilePatternList(&fplist, &settings)

	parse a filepattern-list for listfiles and presetfiles.

	- the first listfile encountered determines a new list
	  (if it can be loaded successfully).
	  return value will be |= 1

	- the first preset-file encountered determines a new
	  set of settings.
	  return value will be |= 2

------------------------------------------------------------------*/

ULONG FilterFilePatternList(char ***array, struct mainsettings **settings)
{
	ULONG ret = 0;

	if (*array)
	{
		struct List *list;

		if ((list = CreateListFromArray(*array)))
		{
			struct Node *node = list->lh_Head, *nextnode;
			BOOL checksettings = (settings != NULL);
			BOOL checklistfile = TRUE;
			char **newfilepatternlist = NULL;

			while ((nextnode = node->ln_Succ) && (checklistfile || checksettings))
			{
				char **tt;
				if ((tt = GetToolTypes(node->ln_Name)))
				{
					if (checksettings)
					{
						if (GetBooleanSetting(tt, "MYSTICVIEW_PRESET", FALSE))
						{
							struct mainsettings *newsettings;
							if ((newsettings = LoadPreset(node->ln_Name, *settings)))
							{
								DeleteMainSettings(*settings);
								*settings = newsettings;
								ret |= 2;
								Remove(node);
								DeleteNode(node);
								checksettings = FALSE;
							}
						}
					}
					if (checklistfile)
					{
						if (GetBooleanSetting(tt, "MYSTICVIEW_LISTFILE", FALSE))
						{
							if ((newfilepatternlist = LoadStringList(node->ln_Name)))
							{
								Remove(node);
								DeleteNode(node);
								checklistfile = FALSE;
							}
						}
					}
					DeleteStringList(tt);
				}
				node = nextnode;
			}

			if (newfilepatternlist)
			{
				DeleteStringList(*array);
				*array = newfilepatternlist;
				ret |= 1;
			}
			else
			{
				if (CountListEntries(list) > 0)
				{
					char **newarray;
					if ((newarray = CreateArrayFromList(list)))
					{
						DeleteStringList(*array);
						*array = newarray;
					}
				}
				else
				{
					DeleteStringList(*array);
					*array = NULL;
				}
			}

			DeleteList(list);
		}
	}

	return ret;
}



/*------------------------------------------------------------------

	success = SaveListFile(mview, list, filename, defaulttool)

------------------------------------------------------------------*/

BOOL SaveListFile(struct mview *mv, char **list, char *filename, char *deftool)
{
	BOOL success = FALSE;
	BOOL icon = TRUE;

	if (filename && list)
	{
		FILE *fp;
		char **t = list;

		if ((fp = fopen(filename, "wb")))
		{
			success = TRUE;

			while (*t && success)
			{
				success = (fprintf(fp, "%s%c", *t, 10) >= 0);
				t++;
			}

			fclose(fp);
		}

		if (success)
		{
			if (icon)
			{
				//	put icon.

				if (mv->diskobject)
				{
					char **tt;

					if ((tt = CreateStringList(1)))
					{
						char *backdeftool;
						char **backtt;
						UBYTE backtype;

						tt[0] = _StrDup("MYSTICVIEW_LISTFILE=YES");

						backdeftool = mv->diskobject->do_DefaultTool;
						backtt = (char **)mv->diskobject->do_ToolTypes;
						backtype = mv->diskobject->do_Type;

						mv->diskobject->do_Type = WBPROJECT;
						mv->diskobject->do_ToolTypes = (STRPTR *)tt;
						mv->diskobject->do_DefaultTool = deftool;

						PutDiskObject(filename, mv->diskobject);

						mv->diskobject->do_Type = backtype;
						mv->diskobject->do_ToolTypes = (STRPTR *)backtt;
						mv->diskobject->do_DefaultTool = backdeftool;

						DeleteStringList(tt);
					}
				}
			}
		}
	}

	return success;
}


/*--------------------------------------------------------------------

	GetErrorText(errorcode)

--------------------------------------------------------------------*/

char *GetErrorText(LONG error)
{
	char *errortext = NULL;

	switch (error)
	{
		case 	0:
			break;

		case	ERROR_NO_FREE_STORE:
			errortext = "Not enough memory";
			break;

		case 	ERROR_OBJECT_TOO_LARGE:
			errortext = "Object too large";
			break;

		case	ERROR_OBJECT_NOT_FOUND:
			errortext = "Object not found";
			break;

		case	ERROR_DELETE_PROTECTED:
			errortext = "Object is delete protected";
			break;

		case	ERROR_WRITE_PROTECTED:
			errortext = "Object is write protected";
			break;

		case	ERROR_READ_PROTECTED:
			errortext = "Object is read protected";
			break;

		case	ERROR_NOT_A_DOS_DISK:
			errortext = "Not a DOS disk";
			break;

		case	ERROR_NO_DISK:
			errortext = "No disk";
			break;

		case	ERROR_OBJECT_IN_USE:
			errortext = "Object in use";
			break;

		case	ERROR_DISK_NOT_VALIDATED:
			errortext = "Disk not validated";
			break;

		case	ERROR_DISK_WRITE_PROTECTED:
			errortext = "Disk write protected";
			break;

		case	ERROR_OBJECT_WRONG_TYPE:
			errortext = "Object is not recognized";
			break;

		default:
			errortext = "Unknown error";
			break;
	}

	return errortext;
}



/*--------------------------------------------------------------------

	Train(mv, settings, viewdata)
		teach the neural net the current displaymode

	displaymode = Recall(mv, settings, viewdata)
		let the neural net choose a displaymode

--------------------------------------------------------------------*/

void Train(struct mview *mv, struct mainsettings *settings, struct viewdata *viewdata)
{
	if (settings && viewdata)
	{
		if (settings->autodisplaymode)
		{
			MVNeuro_Train(mv->screen, mv->window, settings,
				viewdata, viewdata->showpic);
		}
	}
}

int Recall(struct mview *mv, struct mainsettings *settings, struct viewdata *viewdata)
{
	int displaymode = settings->displaymode;

	if (settings && viewdata)
	{
		if (settings->autodisplaymode)
		{
			int dispmode;
			if (MVNeuro_RecallDisplayMode(&dispmode, mv->screen, mv->window, settings,
				viewdata, viewdata->showpic))
			{
				displaymode = dispmode;
			}
		}
	}

	return displaymode;
}




/*******************************************************************
--------------------------------------------------------------------

	buttons

--------------------------------------------------------------------
*******************************************************************/


/*--------------------------------------------------------------------

	ReleaseButtons(buttons)

--------------------------------------------------------------------*/

void ReleaseButtons(struct MVButtons *buttons)
{
	if (buttons)
	{
		FreeBitMap(buttons->animbitmap);
		buttons->animbitmap = NULL;
		FreeBitMap(buttons->bitmap);
		buttons->bitmap = NULL;
		ReleaseDrawHandle(buttons->drawhandle);
		buttons->drawhandle = NULL;
	}
}


/*--------------------------------------------------------------------

	AnimateButtons(viewdata, buttons, reset)

--------------------------------------------------------------------*/

void AnimateButtons(struct viewdata *viewdata, struct MVButtons *buttons, BOOL reset)
{
	if (buttons)
	{
		int newanimframe;

		if (reset)
		{
			newanimframe = 0;
		}
		else
		{
			newanimframe = (buttons->animframe + 1) % ANIMFRAMES;
		}

		if (newanimframe != buttons->animframe)
		{
			buttons->animframe = newanimframe;

			if (buttons->bitmap && buttons->animbitmap)
			{
				BltBitMap(buttons->animbitmap, 0, ANIMHEIGHT * buttons->animframe,
					buttons->bitmap,
					(buttons->gadgetwidth - ANIMWIDTH) / 2,
					(buttons->gadgetheight - ANIMHEIGHT) / 2,
					ANIMWIDTH, ANIMHEIGHT, 0xc0, 0xff, NULL);

				BltBitMap(buttons->animbitmap, 0, ANIMHEIGHT * buttons->animframe,
					buttons->bitmap,
					(buttons->gadgetwidth - ANIMWIDTH) / 2 + 1,
					buttons->gadgetheight + (buttons->gadgetheight - ANIMHEIGHT) / 2 + 1,
					ANIMWIDTH, ANIMHEIGHT, 0xc0, 0xff, NULL);
			}
		}

		ObtainSemaphore(&viewdata->rastlock);

		DrawFakeGadget(buttons->gadgetlist->window, buttons->gadgets[GAD_OPEN],
			buttons->gadgetlist->x, buttons->gadgetlist->y, buttons->bitmap);

		ReleaseSemaphore(&viewdata->rastlock);
	}
}


/*--------------------------------------------------------------------

	success = ObtainButtons(viewdata, buttons, window)

--------------------------------------------------------------------*/

BOOL ObtainButtons(struct viewdata *viewdata, struct MVButtons *buttons, struct Window *win)
{
	BOOL success = FALSE;

	if (buttons && win)
	{
		ReleaseButtons(buttons);

		ObtainSemaphore(&viewdata->rastlock);

		buttons->drawhandle = ObtainDrawHandle(buttons->psm,
			win->RPort, win->WScreen->ViewPort.ColorMap,
			GGFX_AutoDither, FALSE,
			GGFX_DitherMode, DITHERMODE_NONE,
			OBP_Precision, PRECISION_GUI, TAG_DONE);

		ReleaseSemaphore(&viewdata->rastlock);

		if (buttons->drawhandle)
		{
			buttons->animbitmap = CreatePictureBitMap(buttons->drawhandle,
				buttons->animpic, NULL);

			buttons->animframe = -1;

			buttons->bitmap = CreatePictureBitMap(buttons->drawhandle,
				buttons->pic, NULL);

			if (buttons->bitmap && buttons->animbitmap)
			{
				AnimateButtons(viewdata, buttons, TRUE);
				success = TRUE;
			}
		}
	}

	if (!success)
	{
		ReleaseButtons(buttons);
	}

	return success;
}


/*--------------------------------------------------------------------

	DeleteButtons(buttons)

--------------------------------------------------------------------*/

void DeleteButtons(struct MVButtons *buttons)
{
	if (buttons)
	{
		DeleteFakeGadgetList(buttons->gadgetlist);
		buttons->gadgetlist = NULL;
		ReleaseButtons(buttons);
		DeletePenShareMap(buttons->psm);
		Free(buttons);
	}
}


/*--------------------------------------------------------------------

	buttons = CreateButtons(buttonpic, animpic, mvwindow)

--------------------------------------------------------------------*/

struct MVButtons *CreateButtons(PICTURE *pic, PICTURE *animpic, struct mvwindow *win)
{
	struct MVButtons *buttons;
	BOOL success = FALSE;

	if ((buttons = Malloclear(sizeof(struct MVButtons))))
	{
		if ((buttons->pic = pic))
		{
			buttons->animpic = animpic;

			GetPictureAttrs(buttons->pic, PICATTR_Width, (IPTR)&buttons->width,
				PICATTR_Height, (IPTR)&buttons->height, TAG_DONE);

			buttons->gadgetwidth = buttons->width / 8;
			buttons->gadgetheight = buttons->height / 4;

			if ((buttons->psm = CreatePenShareMap(GGFX_HSType, HSTYPE_15BIT_TURBO, TAG_DONE)))
			{
				AddPicture(buttons->psm, buttons->pic, NULL);
				AddPicture(buttons->psm, buttons->animpic, NULL);
			}

			if ((buttons->gadgetlist = CreateFakeGadgetList(win->window, NULL)))
			{
				success = TRUE;

				if (success)
				{
					if ((buttons->gadgets[GAD_OPEN] = CreateFakeGadget(
						GAD_OPEN, FGADTYPE_PUSH,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 0,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 0,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 0,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_OPEN]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_RESTART] = CreateFakeGadget(
						GAD_RESTART, FGADTYPE_PUSH,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 1,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 1,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 1,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_RESTART]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_PREV] = CreateFakeGadget(
						GAD_PREV, FGADTYPE_PUSH,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 2,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 2,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 2,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_PREV]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_NEXT] = CreateFakeGadget(
						GAD_NEXT, FGADTYPE_PUSH,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 3,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 3,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 3,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_NEXT]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_SLIDE] = CreateFakeGadget(
						GAD_SLIDE, FGADTYPE_TOGGLE,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 4,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 4,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 4,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						FGDT_PICX_ACTIVE, buttons->gadgetwidth * 4,
						FGDT_PICY_ACTIVE, buttons->gadgetheight * 2,
						FGDT_PICX_ACTIVEPRESSED, buttons->gadgetwidth * 4,
						FGDT_PICY_ACTIVEPRESSED, buttons->gadgetheight * 3,
						FGDT_PICX_ACTIVEROLLOVER, buttons->gadgetwidth * 4,
						FGDT_PICY_ACTIVEROLLOVER, buttons->gadgetheight * 2,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_SLIDE]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_LOOP] = CreateFakeGadget(
						GAD_LOOP, FGADTYPE_TOGGLE,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 5,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 5,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 5,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						FGDT_PICX_ACTIVE, buttons->gadgetwidth * 5,
						FGDT_PICY_ACTIVE, buttons->gadgetheight * 2,
						FGDT_PICX_ACTIVEPRESSED, buttons->gadgetwidth * 5,
						FGDT_PICY_ACTIVEPRESSED, buttons->gadgetheight * 3,
						FGDT_PICX_ACTIVEROLLOVER, buttons->gadgetwidth * 5,
						FGDT_PICY_ACTIVEROLLOVER, buttons->gadgetheight * 2,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_LOOP]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_DISPLAYMODE] = CreateFakeGadget(
						GAD_DISPLAYMODE, FGADTYPE_TOGGLE,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 6,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 6,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 6,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						FGDT_PICX_ACTIVE, buttons->gadgetwidth * 6,
						FGDT_PICY_ACTIVE, buttons->gadgetheight * 2,
						FGDT_PICX_ACTIVEPRESSED, buttons->gadgetwidth * 6,
						FGDT_PICY_ACTIVEPRESSED, buttons->gadgetheight * 3,
						FGDT_PICX_ACTIVEROLLOVER, buttons->gadgetwidth * 6,
						FGDT_PICY_ACTIVEROLLOVER, buttons->gadgetheight * 2,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_DISPLAYMODE]);
						success = TRUE;
					}
				}
				if (success)
				{
					if ((buttons->gadgets[GAD_RESET] = CreateFakeGadget(
						GAD_RESET, FGADTYPE_PUSH,
						FGDT_WIDTH, buttons->gadgetwidth,
						FGDT_HEIGHT, buttons->gadgetheight,
						FGDT_BITMAP, (IPTR)buttons->bitmap,
						FGDT_PICX_INACTIVE, buttons->gadgetwidth * 7,
						FGDT_PICY_INACTIVE, buttons->gadgetheight * 0,
						FGDT_PICX_PRESSED, buttons->gadgetwidth * 7,
						FGDT_PICY_PRESSED, buttons->gadgetheight * 1,
						FGDT_PICX_ROLLOVER, buttons->gadgetwidth * 7,
						FGDT_PICY_ROLLOVER, buttons->gadgetheight * 0,
						TAG_DONE)))
					{
						AddFakeGadget(buttons->gadgetlist, buttons->gadgets[GAD_RESET]);
						success = TRUE;
					}
				}
			}
		}
	}

	if (!success)
	{
		DeleteButtons(buttons);
		buttons = NULL;
	}

	return buttons;
}


/*--------------------------------------------------------------------

	DrawButtons(viewdata, win, buttons)

--------------------------------------------------------------------*/

void DrawButtons(struct viewdata *viewdata, struct mvwindow *win, struct MVButtons *buttons)
{
	if (buttons && win)
	{
		int height;

		if (!buttons->bitmap)
		{
			ObtainButtons(viewdata, buttons, win->window);
		}

		ObtainSemaphore(&viewdata->rastlock);

		UpdateWindowParameters(win);

		height = MAX(buttons->gadgetheight + BUTTON_VSPACE * 2, ANIMHEIGHT);

		SetAPen(win->window->RPort, win->backpen);
		RectFill(win->window->RPort, win->innerleft, win->innertop,
			win->innerleft + win->innerwidth - 1,
			win->innertop + height - 1);

		if (buttons->bitmap)
		{
			int x;
			float width;

			width = buttons->gadgetwidth * GAD_NUM;
			width = MAX((float) win->innerwidth * 0.45, width);
			width = MIN(width, win->innerwidth - BUTTON_HSPACE * 2);

			x = (win->innerwidth - (int)width) / 2;

			LayoutFakeGadgetList(buttons->gadgetlist, ORIENTATION_RIGHT,
					(int) width, height - 2 * BUTTON_VSPACE,
					x,
					win->innertop + (height - buttons->gadgetheight) / 2);

		//	AnimateButtons(viewdata, buttons, TRUE);

			DrawFakeGadgetList(buttons->gadgetlist, buttons->bitmap);
		}

		ReleaseSemaphore(&viewdata->rastlock);
	}
}


/*--------------------------------------------------------------------

	SetButtonSettings(viewdata, window, buttons, settings, always)

--------------------------------------------------------------------*/

void SetButtonSettings(struct viewdata *viewdata, struct mvwindow *win, struct MVButtons *buttons, struct mainsettings *settings, BOOL always)
{
	if (buttons)
	{
		int change = 0;

		if (settings->slideshow != buttons->gadgets[GAD_SLIDE]->togglestatus)
		{
			buttons->gadgets[GAD_SLIDE]->togglestatus = settings->slideshow;
			change++;
		}

		if (settings->loop != buttons->gadgets[GAD_LOOP]->togglestatus)
		{
			buttons->gadgets[GAD_LOOP]->togglestatus = settings->loop;
			change++;
		}

		if (settings->displaymode !=
				(buttons->gadgets[GAD_DISPLAYMODE]->togglestatus ?
				MVDISPMODE_KEEPASPECT_MIN : MVDISPMODE_IGNOREASPECT))
		{
			buttons->gadgets[GAD_DISPLAYMODE]->togglestatus =
				settings->displaymode == MVDISPMODE_KEEPASPECT_MIN ? TRUE : FALSE;
			change++;
		}

		if ((change || always) && settings->showbuttons)
		{
			DrawButtons(viewdata, win, buttons);
		}
	}
}


/*******************************************************************
--------------------------------------------------------------------

	VIEWDATA

--------------------------------------------------------------------
*******************************************************************/

/*--------------------------------------------------------------------

	DeleteViewData(viewdata)

--------------------------------------------------------------------*/

void DeleteViewData(struct viewdata *viewdata)
{
	if (viewdata)
	{
		CloseTrigger(viewdata->trigger);
		FreeSignal(viewdata->picturevisiblesignal);
		DeletePicHandler(viewdata->pichandler);
		Free(viewdata->pathname);
		Free(viewdata->filename);
		Free(viewdata->formatname);
		Free(viewdata);
	}
}


/*--------------------------------------------------------------------

	viewdata = CreateViewData(startfilepatternlist)

--------------------------------------------------------------------*/

struct viewdata *CreateViewData(char **startfilepatternlist)
{
	struct viewdata *viewdata;

	if ((viewdata = Malloclear(sizeof(struct viewdata))))
	{
		viewdata->zoom = 1;
		viewdata->destzoom = 1;
		viewdata->xpos = .5;
		viewdata->destxpos = .5;
		viewdata->ypos = .5;
		viewdata->destypos = .5;
		viewdata->angle = 0;
		viewdata->oldzoom = viewdata->zoom;
		viewdata->oldxpos = viewdata->xpos;
		viewdata->oldypos = viewdata->ypos;
		viewdata->startfilepatternlist = startfilepatternlist;

		viewdata->trigger = CreateTrigger();
		viewdata->picturevisiblesignal = AllocSignal(-1);

		InitSemaphore(&viewdata->rastlock);

		if (!viewdata->trigger || viewdata->picturevisiblesignal == (UBYTE)-1)
		{
			DeleteViewData(viewdata);
			viewdata = NULL;
		}
	}

	return viewdata;
}


#define	SMOOTHCUTOFF	(0.0005)

/*--------------------------------------------------------------------

	ApplyViewData(struct viewdata *viewdata, settings, BOOL check)

--------------------------------------------------------------------*/

void ApplyViewData(struct viewdata *viewdata, struct mainsettings *settings, BOOL check)
{
	BOOL delay_smooth = FALSE;
	float deltazoom, deltaxpos, deltaypos;
	float xpos, ypos, zoom;
	int angle;
	ULONG *tagptr = viewdata->tagbuffer;
	int changecount = 0;

	assert(viewdata);
	assert(viewdata->view);

	viewdata->destzoom = RNG(0.1f, viewdata->destzoom, 10.0f);
	viewdata->destxpos = RNG(0.0f, viewdata->destxpos, 1.0f);
	viewdata->destypos = RNG(0.0f, viewdata->destypos, 1.0f);

	if (settings->smoothdisplay)
	{
		deltazoom = viewdata->destzoom - viewdata->zoom;
		if (ABS(deltazoom) > SMOOTHCUTOFF)
		{
			deltazoom /= 2;
			viewdata->zoom += deltazoom;
			delay_smooth = TRUE;
		}
		else
		{
			viewdata->zoom = viewdata->destzoom;
		}

		deltaxpos = viewdata->destxpos - viewdata->xpos;
		if (ABS(deltaxpos) > SMOOTHCUTOFF)
		{
			deltaxpos /= 2;
			viewdata->xpos += deltaxpos;
			delay_smooth = TRUE;
		}
		else
		{
			viewdata->xpos = viewdata->destxpos;
		}

		deltaypos = viewdata->destypos - viewdata->ypos;
		if (ABS(deltaypos) > SMOOTHCUTOFF)
		{
			deltaypos /= 2;
			viewdata->ypos += deltaypos;
			delay_smooth = TRUE;
		}
		else
		{
			viewdata->ypos = viewdata->destypos;
		}
	}
	else
	{
		viewdata->zoom = viewdata->destzoom;
		viewdata->xpos = viewdata->destxpos;
		viewdata->ypos = viewdata->destypos;
	}

	xpos = RNG(0.0, viewdata->xpos, 1.0);
	ypos = RNG(0.0, viewdata->ypos, 1.0);
	zoom = RNG(0.1, viewdata->zoom, 10);
	angle = viewdata->angle;

	if (!check || ABS(xpos - viewdata->oldxpos) > SMOOTHCUTOFF)
	{
		*tagptr++ = MVIEW_XPos;
		*tagptr++ = (ULONG) (xpos * 65536);
		changecount++;
	}
	viewdata->oldxpos = xpos;

	if (!check || ABS(ypos - viewdata->oldypos) > SMOOTHCUTOFF)
	{
		*tagptr++ = MVIEW_YPos;
		*tagptr++ = (ULONG) (ypos * 65536);
		changecount++;
	}
	viewdata->oldypos = ypos;

	if (!check || ABS(zoom - viewdata->oldzoom) > SMOOTHCUTOFF)
	{
		*tagptr++ = MVIEW_Zoom;
		*tagptr++ = (ULONG) (zoom * 65536);
		changecount++;
	}
	viewdata->oldzoom = zoom;

	if (!check || (angle - viewdata->oldangle))
	{
		*tagptr++ = MVIEW_Rotation;
		*tagptr++ = angle;
		viewdata->oldangle = angle;
		changecount++;
	}

	if (changecount)
	{
		*tagptr = TAG_DONE;

		MV_SetAttrsA(viewdata->view, (struct TagItem *) viewdata->tagbuffer);

		if (delay_smooth)
		{
			Delay(1);
		}
	}
}


/*--------------------------------------------------------------------

	char *SetPicInfoText(viewdata, pic, settings)

--------------------------------------------------------------------*/

char *SetPicInfoText(struct viewdata *viewdata, PICTURE *pic, struct mainsettings *settings)
{
	char *pitext = NULL;

	if (viewdata && settings)
	{
		if (settings->picinfo && viewdata->pichandler)
		{
			int maxpic, currentpic, status;
			BOOL infotext = FALSE;

			viewdata->statustext[0] = 0;
			viewdata->picturetext[0] = 0;

			if (pic)
			{
				APTR args[6];
				ULONG w = 0, h = 0, d = 0;
				ULONG p;
				GetPictureAttrs(pic,
					PICATTR_Width, (IPTR)&w, PICATTR_Height, (IPTR)&h, PICATTR_PixelFormat, (IPTR)&p,
					TAG_DONE);
				d = p == PIXFMT_CHUNKY_CLUT ? 8 : 24;

				args[0] = viewdata->pathname;
				args[1] = viewdata->filename;
				args[2] = (APTR) w;
				args[3] = (APTR) h;
				args[4] = (APTR) d;
				args[5] = viewdata->formatname;

				infotext = FormatInfoText(viewdata->picturetext, settings->picformat, args);
			}


			maxpic = GetTotalPicNumber(viewdata->pichandler);
			currentpic = GetCurrentPicNumber(viewdata->pichandler);
			status = GetPicHandlerStatus(viewdata->pichandler);


			if (maxpic == 0)
			{
				sprintf(viewdata->statustext, MVTEXT_STAT_NOPIX);
			}
			else if (viewdata->pichandler->errortext)
			{
				char *errortext = GetErrorText(viewdata->pichandler->error);

				if (errortext)
				{
					sprintf(viewdata->statustext, "%s - %s", viewdata->pichandler->errortext, errortext);
				}
				else
				{
					sprintf(viewdata->statustext, "%s", viewdata->pichandler->errortext);
				}
			}
			else if (infotext)
			{
				if (maxpic > 1)
				{
					if (settings->slideshow)
					{
						if (status & PICHSTATUS_LOADING)
						{
							char *text = viewdata->isdirectory ?
								MVTEXT_STAT_SLIDESHOWLOADING_DIR :
								MVTEXT_STAT_SLIDESHOWLOADING;

							sprintf(viewdata->statustext, text, currentpic + 1, maxpic, viewdata->picturetext);
						}
						else
						{
							char *text = viewdata->isdirectory ?
								MVTEXT_STAT_SLIDESHOW_DIR :
								MVTEXT_STAT_SLIDESHOW;

							sprintf(viewdata->statustext, text, currentpic + 1, maxpic, viewdata->picturetext);
						}
					}
					else
					{
						if (status & PICHSTATUS_LOADING)
						{
							char *text = viewdata->isdirectory ?
								MVTEXT_STAT_MULTILOADING_DIR :
								MVTEXT_STAT_MULTILOADING;

							sprintf(viewdata->statustext, text, currentpic + 1, maxpic, viewdata->picturetext);
						}
						else
						{
							char *text = viewdata->isdirectory ?
								MVTEXT_STAT_MULTI_DIR :
								MVTEXT_STAT_MULTI;

							sprintf(viewdata->statustext, text, currentpic + 1, maxpic, viewdata->picturetext);
						}
					}
				}
				else
				{
					if (status & PICHSTATUS_LOADING)
					{
						char *text = viewdata->isdirectory ?
							MVTEXT_STAT_NORMALLOADING_DIR :
							MVTEXT_STAT_NORMALLOADING;

						sprintf(viewdata->statustext, text, viewdata->picturetext);
					}
					else
					{
						char *text = viewdata->isdirectory ?
							MVTEXT_STAT_NORMAL_DIR :
							MVTEXT_STAT_NORMAL;

						sprintf(viewdata->statustext, text, viewdata->picturetext);
					}
				}
			}
			else
			{
				if (maxpic > 1)
				{
					if (status & PICHSTATUS_LOADING)
					{
						sprintf(viewdata->statustext, MVTEXT_STAT_SCANNINGLOADING, maxpic);
					}
					else
					{
						sprintf(viewdata->statustext, MVTEXT_STAT_SCANNING, maxpic);
					}
				}
				else
				{
					if (status & PICHSTATUS_LOADING)
					{
						sprintf(viewdata->statustext, MVTEXT_STAT_LOADING_ONE);
					}
				}
			}


			if (strlen(viewdata->statustext))
			{
				pitext = viewdata->statustext;
			}
		}
		else
		{
			pitext = NULL; 	//"text disabled / no pichandler";
		}
	}

	return pitext;
}


/*--------------------------------------------------------------------

	DeleteViewPic(viewdata, defaultpic, settings)

--------------------------------------------------------------------*/

void DeleteViewPic(struct viewdata *viewdata, PICTURE *defaultpic, struct mainsettings *settings)
{
	if (viewdata)
	{
		MV_SetAttrs(viewdata->view,
			MVIEW_Text, (IPTR)SetPicInfoText(viewdata, defaultpic, settings),
			MVIEW_Picture, (IPTR)defaultpic, TAG_DONE);

		viewdata->showpic = NULL;
		ReleasePic(viewdata->pichandler);
	}
}



/*--------------------------------------------------------------------

	newpicsignal = NewPicHandler(viewdata, filepatternlist, settings)

--------------------------------------------------------------------*/

ULONG NewPicHandler(struct viewdata *viewdata, char **filepatternlist, struct mainsettings *settings, PICTURE *defaultpic)
{
	if (viewdata)
	{
		DeletePicHandler(viewdata->pichandler);
		viewdata->pichandler = NULL;

		if (filepatternlist)
		{
			DeleteViewPic(viewdata, NULL, settings);
		}
		else
		{
			DeleteViewPic(viewdata, defaultpic, settings);
		}

		viewdata->pichandler = CreatePicHandler(filepatternlist,
			settings->asyncscanning,
			PICH_SimpleScanning, settings->simplescanning,
			PICH_Reject, settings->rejectpattern,
			PICH_SortMode, settings->sortmode,
			PICH_SortReverse, settings->reverse,
			PICH_BufferPercent, settings->bufferpercent,
			PICH_AutoCrop, settings->autocrop,
			PICH_IncludeDirs, settings->browsemode,
			PICH_Recursive, settings->browsemode ? FALSE : settings->recursive,
			TAG_DONE);

		return NewPicSignal(viewdata->pichandler);
	}
	else
	{
		return NULL;
	}
}



/*--------------------------------------------------------------------

	SetViewText(viewdata, text)

--------------------------------------------------------------------*/

void SetViewText(struct viewdata *viewdata, char *text)
{
	MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)text, TAG_DONE);
}



/*--------------------------------------------------------------------

	ResetDisplaySettings(viewdata, yesorno)

--------------------------------------------------------------------*/

void ResetDisplaySettings(struct viewdata *viewdata, BOOL reset)
{
	if (reset)
	{
		viewdata->destzoom = 1;
		viewdata->destxpos = .5;
		viewdata->destypos = .5;
		viewdata->angle = 0;
	}
}




/*******************************************************************
--------------------------------------------------------------------

	menu functions

--------------------------------------------------------------------
*******************************************************************/

/*--------------------------------------------------------------------

	newmenu = CreateNewMenuItems(num, commandkeys, strings, emptystring, idlist)

	DeleteNewMenuItems(newmenuitems)

--------------------------------------------------------------------*/

void DeleteNewMenuItems(struct NewMenu *newmenu)
{
	struct NewMenu *n;

	if ((n = newmenu))
	{
		do
		{
			if (newmenu->nm_Label != NM_BARLABEL)
			{
				Free(newmenu->nm_Label);
			}
			Free(newmenu->nm_CommKey);

		} while (newmenu++->nm_Type != NM_END);

		Free(n);
	}
}


struct NewMenu *CreateNewMenuItems(int num, char **commkeys,
	char **strings, char *nostring, APTR *idlist)
{
	int i;
	struct NewMenu *newitems, *nm;
	BOOL error = TRUE;

	if ((newitems = Malloclear(sizeof(struct NewMenu) * (num + 2))))
	{
		error = FALSE;
		nm = newitems;

		for (i = 0; i < (num + 1) && !error; ++i)
		{
			if (i == num)		/* last entry */
			{
				nm->nm_Type = NM_END;
			}
			else
			{
				error = TRUE;

				nm->nm_Type = NM_SUB;
				nm->nm_MutualExclude = 0;
				nm->nm_UserData = *idlist++;

				if (!(*strings))
				{
					nm->nm_Label = _StrDup(nostring);
				}
				else if ((STRPTR)(*strings) == NM_BARLABEL)
				{
					nm->nm_Label = NM_BARLABEL;
				}
				else
				{
					nm->nm_Label = _StrDup(*strings);
				}

				strings++;

				nm->nm_Flags = NM_COMMANDSTRING;
				if (commkeys)
				{
					char *c = *commkeys;
					if (c[0] == '_')
					{
						c++;
						nm->nm_Flags = NULL;
					}
					nm->nm_CommKey = _StrDup(c);
					commkeys++;
				}
				else
				{
					nm->nm_CommKey = _StrDup("");
				}

				if (nm->nm_Label && nm->nm_CommKey)
				{
					error = FALSE;
				}
			}
			nm++;
		}
	}

	if (!error)
	{
		return newitems;
	}
	else
	{
		DeleteNewMenuItems(newitems);
		return NULL;
	}
}


/*--------------------------------------------------------------------

	SetupPathMenus(mv, settings);
	FreePathMenus(mv);

	create path menus

--------------------------------------------------------------------*/

void FreePathMenus(struct mview *mv)
{
	if (mv)
	{
		struct MenuItem *it;

		if ((it = FindMenuItem(mv->window->menu, MITEM_COPYTOPATH)))	it->SubItem = NULL;
		if ((it = FindMenuItem(mv->window->menu, MITEM_MOVETOPATH)))	it->SubItem = NULL;

		if (mv->copypathmenuitems)
			FreeMenus((struct Menu *) mv->copypathmenuitems);
		mv->copypathmenuitems = NULL;

		if (mv->movepathmenuitems)
			FreeMenus((struct Menu *) mv->movepathmenuitems);
		mv->movepathmenuitems = NULL;

		DeleteNewMenuItems(mv->copynewmenuarray);	mv->copynewmenuarray = NULL;
		DeleteNewMenuItems(mv->movenewmenuarray);	mv->movenewmenuarray = NULL;
	}
}

void SetupPathMenus(struct mview *mv, struct pathsettings *settings)
{
	BOOL success = FALSE;

	if (mv && settings)
	{
		if (mv->window)
		{
			if (mv->window->menu)
			{
				char *menuarray[12];
				menuarray[0] = settings->copypath;
				menuarray[1] = NM_BARLABEL;
				menuarray[2] = settings->destpath[0];
				menuarray[3] = settings->destpath[1];
				menuarray[4] = settings->destpath[2];
				menuarray[5] = settings->destpath[3];
				menuarray[6] = settings->destpath[4];
				menuarray[7] = settings->destpath[5];
				menuarray[8] = settings->destpath[6];
				menuarray[9] = settings->destpath[7];
				menuarray[10] = settings->destpath[8];
				menuarray[11] = settings->destpath[9];


				ClearMenuStrip(mv->window->window);

				FreePathMenus(mv);

				{
					ULONG ids[12] = {MITEM_COPYTO, MITEM_DUMMY, MITEM_COPY1, MITEM_COPY1 + 1, MITEM_COPY1 + 2, MITEM_COPY1 + 3, MITEM_COPY1 + 4,
						MITEM_COPY1 + 5, MITEM_COPY1 + 6, MITEM_COPY1 + 7, MITEM_COPY1 + 8, MITEM_COPY1 + 9};
					char *commkeys[12] = {"_C", "", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10"};
					if ((mv->copynewmenuarray = CreateNewMenuItems(12, commkeys, menuarray, "(...)", (APTR *) ids)))
					{
						if ((mv->copypathmenuitems = (struct MenuItem *) CreateMenus(mv->copynewmenuarray, NULL)))
						{
							if (LayoutMenuItems(mv->copypathmenuitems, mv->screen->visualinfo,
								GTMN_NewLookMenus, TRUE, TAG_DONE))
							{
								success = TRUE;
							}
						}
					}
				}

				if (success)
				{
					ULONG ids[12] = {MITEM_MOVETO, MITEM_DUMMY, MITEM_MOVE1, MITEM_MOVE1 + 1, MITEM_MOVE1 + 2, MITEM_MOVE1 + 3, MITEM_MOVE1 + 4,
						MITEM_MOVE1 + 5, MITEM_MOVE1 + 6, MITEM_MOVE1 + 7, MITEM_MOVE1 + 8, MITEM_MOVE1 + 9};
					char *commkeys[12] = {"_M", "", "SHIFT F1", "SHIFT F2", "SHIFT F3", "SHIFT F4", "SHIFT F5", "SHIFT F6", "SHIFT F7", "SHIFT F8", "SHIFT F9", "SHIFT F10"};
					success = FALSE;
					if ((mv->movenewmenuarray = CreateNewMenuItems(12, commkeys, menuarray, "(...)", (APTR *) ids)))
					{
						if ((mv->movepathmenuitems = (struct MenuItem *) CreateMenus(mv->movenewmenuarray, NULL)))
						{
							if (LayoutMenuItems(mv->movepathmenuitems, mv->screen->visualinfo,
								GTMN_NewLookMenus, TRUE, TAG_DONE))
							{
								success = TRUE;
							}
						}
					}
				}

				if (success)
				{
					struct MenuItem *it;
					success = FALSE;

					if ((it = FindMenuItem(mv->window->menu, MITEM_COPYTOPATH)))
					{
						it->SubItem = mv->copypathmenuitems;
						success = TRUE;
					}

					if (success)
					{
						success = FALSE;
						if ((it = FindMenuItem(mv->window->menu, MITEM_MOVETOPATH)))
						{
							it->SubItem = mv->movepathmenuitems;
							success = TRUE;
						}
					}

					if (success)
					{
						success = LayoutMenus(mv->window->menu, mv->screen->visualinfo,
							GTMN_NewLookMenus, TRUE, TAG_DONE);
					}

					if (!success)
					{
						FreePathMenus(mv);
					}
				}

				SetMenuStrip(mv->window->window, mv->window->menu);
			}
		}
	}
}


/*--------------------------------------------------------------------

	SetupPresetMenu(mv, settings);
	FreePresetMenu(mv);

	create preset menu

--------------------------------------------------------------------*/

void FreePresetMenu(struct mview *mv)
{
	if (mv)
	{
		struct MenuItem *it;

		if ((it = FindMenuItem(mv->window->menu, MITEM_USEPRESET)))
		{
			it->SubItem = NULL;
		}

		if (mv->presetmenuitems)
		{
			FreeMenus((struct Menu *) mv->presetmenuitems);
		}

		mv->presetmenuitems = NULL;

		DeleteNewMenuItems(mv->presetmenuarray);
		mv->presetmenuarray = NULL;
	}
}

void SetupPresetMenu(struct mview *mv, struct pathsettings *settings)
{
	BOOL success = FALSE;

	if (mv && settings)
	{
		if (mv->window)
		{
			if (mv->window->menu)
			{
				int i;
				char *filenames[10];
				char *commkeys[10] = {"CTRL F1", "CTRL F2", "CTRL F3", "CTRL F4", "CTRL F5", "CTRL F6", "CTRL F7", "CTRL F8", "CTRL F9", "CTRL F10"};
				ULONG ids[10] =
					{MITEM_PRESET1, MITEM_PRESET1 + 1, MITEM_PRESET1 + 2, MITEM_PRESET1 + 3, MITEM_PRESET1 + 4,
					MITEM_PRESET1 + 5, MITEM_PRESET1 + 6, MITEM_PRESET1 + 7, MITEM_PRESET1 + 8, MITEM_PRESET1 + 9};

				for (i = 0; i < 10; ++i)
				{
					filenames[i] = settings->preset[i] ? _StrDup(FilePart(settings->preset[i])) : NULL;
				}

				ClearMenuStrip(mv->window->window);

				FreePresetMenu(mv);

				if ((mv->presetmenuarray = CreateNewMenuItems(10, commkeys, filenames, "(...)", (APTR *) ids)))
				{
					if ((mv->presetmenuitems = (struct MenuItem *) CreateMenus(mv->presetmenuarray, NULL)))
					{
						if (LayoutMenuItems(mv->presetmenuitems, mv->screen->visualinfo,
							GTMN_NewLookMenus, TRUE, TAG_DONE))
						{
							success = TRUE;
						}
					}
				}

				for (i = 0; i < 10; ++i)
				{
					Free(filenames[i]);
				}

				if (success)
				{
					struct MenuItem *it;
					success = FALSE;

					if ((it = FindMenuItem(mv->window->menu, MITEM_USEPRESET)))
					{
						it->SubItem = mv->presetmenuitems;
						success = TRUE;
					}

					if (success)
					{
						success = LayoutMenus(mv->window->menu, mv->screen->visualinfo,
							GTMN_NewLookMenus, TRUE, TAG_DONE);
					}

					if (!success)
					{
						FreePresetMenu(mv);
					}
				}

				SetMenuStrip(mv->window->window, mv->window->menu);
			}
		}
	}
}


/*--------------------------------------------------------------------

	SetupPubscrenMenu(mv, settings);
	FreePubscreenMenu(mv);

	create pubscreen menu

--------------------------------------------------------------------*/

void FreePubscreenMenu(struct mview *mv)
{
	if (mv)
	{
		struct MenuItem *it;

		if ((it = FindMenuItem(mv->window->menu, MITEM_SELECTPUBSCREEN)))
		{
			it->SubItem = NULL;
		}

		if (mv->pubscreenmenuitems)
		{
			FreeMenus((struct Menu *) mv->pubscreenmenuitems);
		}

		mv->pubscreenmenuitems = NULL;

		DeleteNewMenuItems(mv->pubscreenmenuarray);
		mv->pubscreenmenuarray = NULL;
	}
}

void SetupPubscreenMenu(struct mview *mv, struct mainsettings *settings)
{
	BOOL success = FALSE;

	if (mv && settings)
	{
		if (mv->window)
		{
			if (mv->window->menu)
			{
				int numscreens = 0;
				struct List *pubscreenlist;

				if ((pubscreenlist = LockPubScreenList()))
				{
					struct Node *node;
					struct Node *nextnode;

					node = pubscreenlist->lh_Head;
					while ((nextnode = node->ln_Succ))
					{
						numscreens++;
						node = nextnode;
					}

					if (numscreens)
					{
						int count = 0;

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

						if ((mv->pubscreenlist = CreateStringList(numscreens + 3)))
						{
							char *name;

						   node = pubscreenlist->lh_Head;
						   while ((nextnode = node->ln_Succ))
						   {
								BOOL getit = TRUE;

								name = ((struct PubScreenNode *) node)->psn_Node.ln_Name;

								if (getit)
								{
									if ((mv->pubscreenlist[count + 3] = _StrDup(name)))
									{
										count++;
									}
								}
								node = nextnode;
							}
						}
						numscreens = count;
					}
				}
				UnlockPubScreenList();

				if (numscreens && mv->pubscreenlist)
				{
					ULONG *idlist;

					if ((idlist = Malloc(sizeof(APTR) * (numscreens + 3))))
					{
						int i;

						mv->pubscreenlist[0] = _StrDup("Select Mode...");
						mv->pubscreenlist[1] = NM_BARLABEL;
						mv->pubscreenlist[2] = _StrDup("Default Screen");

						idlist[0] = MITEM_CUSTOMSCREEN;
						idlist[1] = MITEM_DUMMY;
						idlist[2] = MITEM_DEFAULTSCREEN;
						for (i = 0; i < numscreens; ++i)
						{
							idlist[i + 3] = MITEM_PUBSCREEN1 + i;
						}

						ClearMenuStrip(mv->window->window);

						FreePubscreenMenu(mv);

						if ((mv->pubscreenmenuarray = CreateNewMenuItems(numscreens + 3, NULL, mv->pubscreenlist, "", (APTR *) idlist)))
						{
							if ((mv->pubscreenmenuitems = (struct MenuItem *) CreateMenus(mv->pubscreenmenuarray, NULL)))
							{
								if (LayoutMenuItems(mv->pubscreenmenuitems, mv->screen->visualinfo,
									GTMN_NewLookMenus, TRUE, TAG_DONE))
								{
									success = TRUE;
								}
							}
						}

						if (success)
						{
							struct MenuItem *it;
							success = FALSE;

							if ((it = FindMenuItem(mv->window->menu, MITEM_SELECTPUBSCREEN)))
							{
								it->SubItem = mv->pubscreenmenuitems;
								success = TRUE;
							}

							if (success)
							{
								success = LayoutMenus(mv->window->menu, mv->screen->visualinfo,
									GTMN_NewLookMenus, TRUE, TAG_DONE);
							}

							if (!success)
							{
								FreePubscreenMenu(mv);
							}
						}

						SetMenuStrip(mv->window->window, mv->window->menu);

						Free(idlist);
					}
				}
			}
		}
	}
}


/*--------------------------------------------------------------------

	SetMenuSettings (mview, settings, viewdata)
	put the current settings into the menus.

--------------------------------------------------------------------*/

void SetMenuSettings(struct mview *mv, struct mainsettings *settings)
{
	if (mv)
	{
		if (mv->window)
		{
			struct Menu *menu = mv->window->menu;

			ClearMenuStrip(mv->window->window);

			CLEARMENUITEM(menu, MITEM_DITHERON);
			CLEARMENUITEM(menu, MITEM_DITHERAUTO);
			switch (settings->dither)
			{
				case MVDITHERMODE_ON:
					SETMENUITEM(menu, MITEM_DITHERON);
					break;
				case MVDITHERMODE_AUTO:
					SETMENUITEM(menu, MITEM_DITHERAUTO);
					break;
			}

			CLEARMENUITEM(menu, MITEM_KEEPASPECT1);
			CLEARMENUITEM(menu, MITEM_IGNOREASPECT);
			switch (settings->displaymode)
			{
				case MVDISPMODE_KEEPASPECT_MIN:
					SETMENUITEM(menu, MITEM_KEEPASPECT1);
					break;
				case MVDISPMODE_IGNOREASPECT:
					SETMENUITEM(menu, MITEM_IGNOREASPECT);
					break;
			}

			CLEARMENUITEM(menu, MITEM_STATIC);
			if (settings->staticpalette) SETMENUITEM(menu, MITEM_STATIC);

			CLEARMENUITEM(menu, MITEM_BACKDROP);
			if (settings->borderless) SETMENUITEM(menu, MITEM_BACKDROP);

			CLEARMENUITEM(menu, MITEM_SMALLWINDOW);
			CLEARMENUITEM(menu, MITEM_HALFWINDOW);
			CLEARMENUITEM(menu, MITEM_LARGEWINDOW);
			CLEARMENUITEM(menu, MITEM_VISIBLEWINDOW);
			CLEARMENUITEM(menu, MITEM_FULLWINDOW);
			CLEARMENUITEM(menu, MITEM_FIXEDWINDOWSIZE);
			switch (settings->winsizemode)
			{
				case WSIZEMODE_33PERCENT:
					SETMENUITEM(menu, MITEM_SMALLWINDOW);
					break;
				case WSIZEMODE_HALF:
					SETMENUITEM(menu, MITEM_HALFWINDOW);
					break;
				case WSIZEMODE_66PERCENT:
					SETMENUITEM(menu, MITEM_LARGEWINDOW);
					break;
				case WSIZEMODE_VISIBLE:
					SETMENUITEM(menu, MITEM_VISIBLEWINDOW);
					break;
				case WSIZEMODE_FULL:
					SETMENUITEM(menu, MITEM_FULLWINDOW);
					break;
				case WSIZEMODE_NONE:
					SETMENUITEM(menu, MITEM_FIXEDWINDOWSIZE);
					break;
			}

			CLEARMENUITEM(menu, MITEM_CENTERWINDOW);
			CLEARMENUITEM(menu, MITEM_MOUSEWINDOW);
			CLEARMENUITEM(menu, MITEM_FIXEDWINDOWPOS);
			switch (settings->winopenmode)
			{
				case WOPENMODE_CENTER:
					SETMENUITEM(menu, MITEM_CENTERWINDOW);
					break;
				case WOPENMODE_MOUSE:
					SETMENUITEM(menu, MITEM_MOUSEWINDOW);
					break;
				case WOPENMODE_NONE:
					SETMENUITEM(menu, MITEM_FIXEDWINDOWPOS);
					break;
			}

			CLEARMENUITEM(menu, MITEM_REQUESTFILE);
			if (settings->requestfile) SETMENUITEM(menu, MITEM_REQUESTFILE);

			CLEARMENUITEM(menu, MITEM_OPAQUE);
			CLEARMENUITEM(menu, MITEM_GRID);
			switch (settings->previewmode)
			{
				case MVPREVMODE_OPAQUE:
					SETMENUITEM(menu, MITEM_OPAQUE);
					break;
				case MVPREVMODE_GRID:
					SETMENUITEM(menu, MITEM_GRID);
					break;
			}

			CLEARMENUITEM(menu, MITEM_SLIDE);
			if (settings->slideshow) SETMENUITEM(menu, MITEM_SLIDE);

			CLEARMENUITEM(menu, MITEM_PICINFO);
			if (settings->picinfo) SETMENUITEM(menu, MITEM_PICINFO);

			CLEARMENUITEM(menu, MITEM_CACHE_OFF);
			CLEARMENUITEM(menu, MITEM_CACHE_SMALL);
			CLEARMENUITEM(menu, MITEM_CACHE_MEDIUM);
			CLEARMENUITEM(menu, MITEM_CACHE_LARGE);
			CLEARMENUITEM(menu, MITEM_CACHE_HUGE);
			switch (settings->bufferpercent)
			{
				case 1:
					SETMENUITEM(menu, MITEM_CACHE_OFF);
					break;
				case 15:
					SETMENUITEM(menu, MITEM_CACHE_SMALL);
					break;
				case 25:
					SETMENUITEM(menu, MITEM_CACHE_MEDIUM);
					break;
				case 35:
					SETMENUITEM(menu, MITEM_CACHE_LARGE);
					break;
				case 50:
					SETMENUITEM(menu, MITEM_CACHE_HUGE);
					break;
				default:
					break;
			}

			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_0);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_04);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_1);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_2);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_5);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_10);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_30);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_60);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_120);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_300);
			CLEARMENUITEM(menu, MITEM_SLIDEDELAY_600);
			switch (settings->slidedelay)
			{
				case 0:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_0);
					break;
				case 4:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_04);
					break;
				case 10:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_1);
					break;
				case 20:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_2);
					break;
				case 50:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_5);
					break;
				case 100:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_10);
					break;
				case 300:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_30);
					break;
				case 600:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_60);
					break;
				case 1200:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_120);
					break;
				case 3000:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_300);
					break;
				case 6000:
					SETMENUITEM(menu, MITEM_SLIDEDELAY_600);
					break;
				default:
					break;
			}

			CLEARMENUITEM(menu, MITEM_AUTOQUIT);
			if (settings->autoquit) SETMENUITEM(menu, MITEM_AUTOQUIT);

			CLEARMENUITEM(menu, MITEM_AUTOSTOP);
			if (settings->autostop) SETMENUITEM(menu, MITEM_AUTOSTOP);

			CLEARMENUITEM(menu, MITEM_AUTOCLEAR);
			if (settings->autoclear) SETMENUITEM(menu, MITEM_AUTOCLEAR);

			CLEARMENUITEM(menu, MITEM_AUTOHIDE);
			if (settings->autohide) SETMENUITEM(menu, MITEM_AUTOHIDE);

			CLEARMENUITEM(menu, MITEM_LOOP);
			if (settings->loop) SETMENUITEM(menu, MITEM_LOOP);

			CLEARMENUITEM(menu, MITEM_SORT_NONE);
			CLEARMENUITEM(menu, MITEM_SORT_ALPHA_FILE);
			CLEARMENUITEM(menu, MITEM_SORT_ALPHA_FULL);
			CLEARMENUITEM(menu, MITEM_SORT_RANDOM);
			CLEARMENUITEM(menu, MITEM_SORT_FILESIZE);
			CLEARMENUITEM(menu, MITEM_SORT_DATE);
			switch (settings->sortmode)
			{
				case SORTMODE_NONE:
					SETMENUITEM(menu, MITEM_SORT_NONE);
					break;
				case SORTMODE_ALPHA_FILE:
					SETMENUITEM(menu, MITEM_SORT_ALPHA_FILE);
					break;
				case SORTMODE_RANDOM:
					SETMENUITEM(menu, MITEM_SORT_RANDOM);
					break;
				case SORTMODE_FILESIZE:
					SETMENUITEM(menu, MITEM_SORT_FILESIZE);
					break;
				case SORTMODE_ALPHA_FULL:
					SETMENUITEM(menu, MITEM_SORT_ALPHA_FULL);
					break;
				case SORTMODE_DATE:
					SETMENUITEM(menu, MITEM_SORT_DATE);
					break;
			}

			CLEARMENUITEM(menu, MITEM_SORT_REVERSE);
			if (settings->reverse) SETMENUITEM(menu, MITEM_SORT_REVERSE);

			CLEARMENUITEM(menu, MITEM_MARKCENTER);
			if (settings->markcenter) SETMENUITEM(menu, MITEM_MARKCENTER);

			CLEARMENUITEM(menu, MITEM_SHOWARROWS);
			if (settings->showarrows) SETMENUITEM(menu, MITEM_SHOWARROWS);

			CLEARMENUITEM(menu, MITEM_SHOWPIP);
			if (settings->showpip) SETMENUITEM(menu, MITEM_SHOWPIP);

			CLEARMENUITEM(menu, MITEM_SHOWBUTTONS);
			if (settings->showbuttons) SETMENUITEM(menu, MITEM_SHOWBUTTONS);

			CLEARMENUITEM(menu, MITEM_RESETDISPLAYSETTINGS);
			if (settings->resetdisplaysettings) SETMENUITEM(menu, MITEM_RESETDISPLAYSETTINGS);

			CLEARMENUITEM(menu, MITEM_CREATETHUMBNAILS);
			if (settings->createthumbnails) SETMENUITEM(menu, MITEM_CREATETHUMBNAILS);

			CLEARMENUITEM(menu, MITEM_ASYNCSCANNING);
			if (settings->asyncscanning) SETMENUITEM(menu, MITEM_ASYNCSCANNING);

			CLEARMENUITEM(menu, MITEM_FILTERPICTURES);
			if (settings->filterpictures) SETMENUITEM(menu, MITEM_FILTERPICTURES);

			CLEARMENUITEM(menu, MITEM_APPENDICONS);
			if (settings->appendicons) SETMENUITEM(menu, MITEM_APPENDICONS);

			CLEARMENUITEM(menu, MITEM_LMB_DRAG);
			if (settings->leftmouseaction == MOUSEACTION_DRAG) SETMENUITEM(menu, MITEM_LMB_DRAG);

			CLEARMENUITEM(menu, MITEM_AUTODISPLAYMODE);
			if (settings->autodisplaymode) SETMENUITEM(menu, MITEM_AUTODISPLAYMODE);

			CLEARMENUITEM(menu, MITEM_AUTOCROP);
			if (settings->autocrop) SETMENUITEM(menu, MITEM_AUTOCROP);

			ResetMenuStrip(mv->window->window, mv->window->menu);
		}
	}

}


void SetMenuPathSettings(struct mview *mv, struct pathsettings *settings)
{
	if (mv)
	{
		if (mv->window)
		{
			struct Menu *menu = mv->window->menu;

			ClearMenuStrip(mv->window->window);

			CLEARMENUITEM(menu, MITEM_AUTOSAVESETTINGS);
			if (settings->autosavesettings) SETMENUITEM(menu, MITEM_AUTOSAVESETTINGS);

			ResetMenuStrip(mv->window->window, mv->window->menu);
		}
	}
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/*********************************************************************
----------------------------------------------------------------------

	waslastpic = GetNextPicture(mview, buttons, viewdata, settings, no-more-pic)

----------------------------------------------------------------------
*********************************************************************/

BOOL GetNextPicture(struct mview *mv, struct MVButtons *buttons, struct viewdata *viewdata,
	struct mainsettings *settings, PICTURE *defaultpic, ULONG *newpicsignal)
{
	BOOL lastpic = FALSE;

	if (viewdata->pichandler)
	{
		BOOL getit = TRUE;

		if (GetNextPicNumber(viewdata->pichandler) == 0)
		{
			if (GetCurrentPicNumber(viewdata->pichandler) != -1)
			{
				if (GetTotalPicNumber(viewdata->pichandler) == 1)
				{
					getit = FALSE;
					lastpic = FALSE;
				}
				else
				{
					if (!settings->loop)
					{
						lastpic = TRUE;
						getit = FALSE;

						DeleteViewPic(viewdata, defaultpic, settings);
				//		Free(viewdata->picinfotext);
				//		viewdata->picinfotext = NULL;
						SetViewText(viewdata, MVTEXT_SELECTNEXTPICTORESUME);
						ResetPicHandler(viewdata->pichandler);
					}
				}
			}
		}

		if (getit)
		{
			NextPic(viewdata->pichandler);			// request next picture
//			SetViewText(viewdata, SetPicInfoText(viewdata, defaultpic, settings));
		}

		SetTrigger(viewdata->trigger, 0);			// suspend trigger
	}

	if (lastpic)
	{
		ResetDisplaySettings(viewdata, settings->resetdisplaysettings);
		if (settings->autostop) settings->slideshow = FALSE;
		SetMenuSettings(mv, settings);
		SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);

		if (settings->autoclear)
		{
			//signals &= ~newpicsignal;
			*newpicsignal = NewPicHandler(viewdata, NULL, settings, mv->logopic);
		}

	}

	return lastpic;
}


/*********************************************************************
----------------------------------------------------------------------

	GetCurrentPicture(mview, buttons, viewdata, settings, no-more-pic)

----------------------------------------------------------------------
*********************************************************************/

void GetCurrentPicture(struct mview *mv, struct MVButtons *buttons, struct viewdata *viewdata,
	struct mainsettings *settings, PICTURE *defaultpic)
{
	if (viewdata->pichandler)
	{
		BOOL getit = TRUE;

		if (GetTotalPicNumber(viewdata->pichandler) == 0)
		{
			DeleteViewPic(viewdata, defaultpic, settings);
			getit = FALSE;
		}
		else
		{
			if (GetNextPicNumber(viewdata->pichandler) == 0)
			{
				getit = TRUE;
			}
		}

		if (getit)
		{
			CurrentPic(viewdata->pichandler);			// request
		}

//!!!!!!!!!		SetViewText(viewdata, SetPicInfoText(viewdata, defaultpic, settings));

		SetTrigger(viewdata->trigger, 0);			// suspend trigger
	}
}


/*********************************************************************
----------------------------------------------------------------------

	status = HandleMView (mview, **settings, **pathsettings, oldsettings, viewdata, reopen, char **filepatternlist)

----------------------------------------------------------------------
*********************************************************************/

int HandleMView (struct mview *mv,
	struct mainsettings **initsettings,
	struct pathsettings **initpathsettings,
	struct mainsettings *oldsettings, struct viewdata *viewdata, BOOL reopen)
{
	#define REQUEST_NONE		0
	#define REQUEST_OPENFILES	1
	#define REQUEST_APPENDFILES	2

	int status = STATUS_WORKING;
	int requestfiles = REQUEST_NONE;

	BOOL mousebutton = FALSE;
	BOOL checkinvisiblegadgets = FALSE;

	BOOL slidesuspended = FALSE;

	struct Window *win;
	struct Menu *menu;
	struct IntuiMessage *imsg;
	struct AppMessage *appmsg;

	ULONG signals;
	ULONG loadsignal = 0;
	ULONG idcmpsignal;
	ULONG appsignal;
	ULONG triggersignal;
	ULONG newpicsignal = 0;

	struct MVButtons *buttons;

	ULONG cxsignal = 1 << (mv->brokermsgport->mp_SigBit);
	CxMsg *cxmsg;

	struct mainsettings *settings = *initsettings;
	struct pathsettings *pathsettings = *initpathsettings;

	BOOL setviewrelative = FALSE;
	
	ULONG currentidcmp = mv->window->idcmpmask;
	int holdmx = -1, holdmy = -1;
	



	/*
	**		init menus
	*/

	SetupPathMenus(mv, pathsettings);
	SetupPresetMenu(mv, pathsettings);
	SetupPubscreenMenu(mv, settings);
	SetMenuSettings(mv, settings);
	SetMenuPathSettings(mv, pathsettings);



	/*
	**		misc initializations
	*/

	if (pathsettings->startpath && !viewdata->pathname)
	{
		viewdata->pathname = _StrDup(pathsettings->startpath);
	}

	triggersignal = SetTrigger(viewdata->trigger, 0);		// trigger suspended



	/*
	**		buttons
	*/

	buttons = CreateButtons(mv->buttonpic, mv->animpic, mv->window);
	if (!buttons)
	{
		EasyRequest(mv->window->window, &nobuttonsreq, NULL, NULL);
	}

	if (settings->showbuttons)
	{
		SetButtonSettings(viewdata, mv->window, buttons, settings, TRUE);
	}


	/*
	**		create viewer
	*/

	viewdata->view = MV_Create(mv->screen->screen, mv->window->window->RPort,
		MVIEW_BackColor, settings->bgcolor,
		MVIEW_TextColor, settings->textcolor,
		MVIEW_MarkColor, settings->markcolor,
		MVIEW_DisplayMode, settings->displaymode,
		MVIEW_DestWidth, mv->window->innerwidth,
		MVIEW_DestHeight, mv->window->innerheight - (settings->showbuttons && buttons ? buttons->gadgetheight + BUTTON_VSPACE * 2 : 0),
		MVIEW_DestX, mv->window->innerleft,
		MVIEW_DestY, mv->window->innertop + (settings->showbuttons && buttons ? buttons->gadgetheight + BUTTON_VSPACE * 2 : 0),
		MVIEW_Precision, settings->precision,
		MVIEW_PreviewMode, settings->previewmode,
		MVIEW_StaticPalette, settings->staticpalette,
		MVIEW_Dither, settings->dither,
		MVIEW_DitherMode, settings->dithermode,
		MVIEW_HSType, settings->hstype,
		MVIEW_ScreenAspectX, settings->screenaspectx,
		MVIEW_ScreenAspectY, settings->screenaspecty,
		MVIEW_ShowCursor, settings->markcenter,
		MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings),
		MVIEW_Picture, viewdata->showpic ? (IPTR)viewdata->showpic : (IPTR)mv->logopic,
		MVIEW_ReadySignal, viewdata->picturevisiblesignal,
		MVIEW_DrawArrows, settings->showarrows,
		MVIEW_ShowPip, settings->showpip,
		MVIEW_RPSemaphore, (IPTR)&viewdata->rastlock,
		TAG_DONE);

	if (!viewdata->view)
	{
		EasyRequest(mv->window->window, &noviewreq, NULL, NULL);
	}

	ApplyViewData(viewdata, settings, FALSE);

	MV_DrawOn(viewdata->view);



	/*
	**		create picture handler
	*/

	if (!viewdata->pichandler)
	{
		NewPicHandler(viewdata, viewdata->startfilepatternlist, settings, mv->logopic);
	}
	if (!viewdata->pichandler)
	{
		EasyRequest(mv->window->window, &nopichandlerreq, NULL, NULL);
	}
	else
	{
		newpicsignal = NewPicSignal(viewdata->pichandler);
		SetSortMode(viewdata->pichandler, settings->sortmode, settings->reverse);
	}


	/*
	**		open filerequester?
	*/

	if (!reopen && !viewdata->startfilepatternlist)
	{
		requestfiles = settings->requestfile ? REQUEST_OPENFILES : REQUEST_NONE;
	}




	/*
	**		continue slideshow / reload / display resume message
	*/

	if (GetTotalPicNumber(viewdata->pichandler) > 0)
	{
		if (GetCurrentPicNumber(viewdata->pichandler) == -1)
		{
			SetViewText(viewdata, MVTEXT_SELECTNEXTPICTORESUME);
			ResetPicHandler(viewdata->pichandler);
		}
		else
		{
			if (settings->slideshow)
			{
				if (GetTotalPicNumber(viewdata->pichandler) > 1)
				{
					SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
				}
			}
		}
	}




	/*
	**		main loop
	*/

	do
	{
		BOOL do_smoothing =
			(ABS(viewdata->destzoom - viewdata->zoom) > SMOOTHCUTOFF) |
			(ABS(viewdata->destxpos - viewdata->xpos) > SMOOTHCUTOFF) |
			(ABS(viewdata->destypos - viewdata->ypos) > SMOOTHCUTOFF);

		if (mv->window)
		{
			win = mv->window->window;
			menu = mv->window->menu;
			idcmpsignal = mv->window->idcmpSignal;
			appsignal = mv->appSignal;
		}
		else
		{
			win = NULL;
			menu = NULL;
			idcmpsignal = 0;
			appsignal = 0;
		}


		if (settings->showbuttons && !do_smoothing)
		{
			int status;
			while ((status = GetPicHandlerStatus(viewdata->pichandler))
					& (PICHSTATUS_LOADING | PICHSTATUS_SCANNING))
			{
				AnimateButtons(viewdata, buttons, FALSE);
				if ((signals = SetSignal(0, 0)) & (idcmpsignal | appsignal | triggersignal |
					newpicsignal | (1 << viewdata->picturevisiblesignal) |
					loadsignal | cxsignal | SIGBREAKF_CTRL_C))
				{
					break;
				}
				Delay(1);
			}

			if (status == PICHSTATUS_IDLE) AnimateButtons(viewdata, buttons, TRUE);
		}

		if (requestfiles == REQUEST_NONE && !do_smoothing)
		{
			signals = Wait(idcmpsignal | appsignal | triggersignal |
				newpicsignal | (1 << viewdata->picturevisiblesignal) |
				loadsignal | cxsignal | SIGBREAKF_CTRL_C);
		}
		else
		{
			signals = SetSignal(0, idcmpsignal | appsignal | triggersignal |
				newpicsignal | (1 << viewdata->picturevisiblesignal) |
				loadsignal | cxsignal | SIGBREAKF_CTRL_C);
		}



		if (win)
		{
			BOOL hardrefresh = FALSE;

			if (signals & idcmpsignal)
			{
				static LONG lastclick_seconds = 0, lastclick_micros = 0;
				struct IntuiMessage myImsg;
			//	BOOL setviewrelative = FALSE;
				BOOL relayoutmenus = FALSE;
				BOOL softrefresh = FALSE;
				BOOL setmenusettings = FALSE;

				while ((imsg = GT_GetIMsg(win->UserPort)))
				{
					memcpy(&myImsg, imsg, sizeof(struct IntuiMessage));

					GT_ReplyIMsg(imsg);

					myImsg.MouseX -= win->BorderLeft;
					myImsg.MouseY -= win->BorderTop;

					if (buttons && settings->showbuttons && myImsg.Class == IDCMP_MOUSEBUTTONS)
					{
						ULONG id;

						ObtainSemaphore(&viewdata->rastlock);
						id = HandleGadgetListEvent(buttons->gadgetlist, &myImsg, buttons->bitmap);
						ReleaseSemaphore(&viewdata->rastlock);

						if (id !=INVALID_FAKEGADGET)
						{
							switch (id)
							{
								case GAD_OPEN:
									requestfiles = REQUEST_OPENFILES;
									break;

								case GAD_RESTART:
									if (viewdata->pichandler)
									{
										ResetPicHandler(viewdata->pichandler);
										GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal);

										SetTrigger(viewdata->trigger, 0);
										settings->slideshow = FALSE;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									}
									break;

								case GAD_NEXT:
									if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
									{
										if (settings->autoquit) status = STATUS_QUIT;
										else if (settings->autohide) status = STATUS_SLEEP;
										else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
									}
									break;

								case GAD_PREV:
									if (PrevPic(viewdata->pichandler))
									{
										SetTrigger(viewdata->trigger, 0);
										settings->slideshow = FALSE;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									}
									break;

								case GAD_SLIDE:
									if ((settings->slideshow = buttons->gadgets[GAD_SLIDE]->togglestatus))
									{
										SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
									}
									else
									{
										SetTrigger(viewdata->trigger, 0);
									}
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									break;

								case GAD_LOOP:
									if ((settings->loop = buttons->gadgets[GAD_LOOP]->togglestatus))
									{
										settings->autoquit = FALSE;
										settings->autohide = FALSE;
								//		SetMenuSettings(mv, settings);
								//		setmenusettings = TRUE;
									}
									//SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									break;

								case GAD_DISPLAYMODE:
								{
									int newdisplaymode =
										buttons->gadgets[GAD_DISPLAYMODE]->togglestatus ?
										MVDISPMODE_KEEPASPECT_MIN : MVDISPMODE_IGNOREASPECT;

									if (newdisplaymode != settings->displaymode)
									{
										settings->displaymode = newdisplaymode;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										Train(mv, settings, viewdata);
										MV_SetAttrs(viewdata->view, MVIEW_DisplayMode, settings->displaymode, TAG_DONE);
									}
									break;
								}

								case GAD_RESET:
								{
									ResetDisplaySettings(viewdata, TRUE);
									break;
								}

								default:
									break;
							}
						}
					}


					switch (myImsg.Class)
					{
						case IDCMP_ACTIVEWINDOW:
							SetupPubscreenMenu(mv, settings);
							mousebutton = FALSE;
							break;

						case IDCMP_INACTIVEWINDOW:
							mousebutton = FALSE;
							break;

						case IDCMP_MOUSEMOVE:
							if (mousebutton)
							{
								if (!setviewrelative)
								{
									setviewrelative = TRUE;
								}
							}
							break;

						case IDCMP_MOUSEBUTTONS:
						{
							ULONG newidcmp = currentidcmp;
							int x, y, w, h;

							MV_GetAttrs(viewdata->view,
								MVIEW_PictureX, (IPTR)&x, MVIEW_PictureY, (IPTR)&y,
								MVIEW_PictureWidth, (IPTR)&w, MVIEW_PictureHeight, (IPTR)&h, TAG_DONE);

							switch (myImsg.Code)
							{
								case SELECTUP:
								{
									if (checkinvisiblegadgets)
									{
										if (myImsg.MouseY == 0)
										{
											if (myImsg.MouseX == 0)
											{
												status = settings->hideonclose ? STATUS_SLEEP : STATUS_QUIT;
											}
											else if (myImsg.MouseX == mv->screen->screen->Width - 1)
											{
												ScreenToBack(mv->screen->screen);
											}
										}
									}
									
									checkinvisiblegadgets = FALSE;
									setviewrelative = FALSE;
									mousebutton = FALSE;
									newidcmp &= ~(IDCMP_MOUSEMOVE | IDCMP_INTUITICKS);

									if (settings->leftmouseaction == MOUSEACTION_NEXT)
									{
										if (myImsg.MouseX >= x && myImsg.MouseX < x+w && myImsg.MouseY >= y && myImsg.MouseY < y+h)
										{
											if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
											{
												if (settings->autoquit) status = STATUS_QUIT;
												else if (settings->autohide) status = STATUS_SLEEP;
												else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
											}
										}
									}
									
									break;
								}
								
								case SELECTDOWN:
								{

									mousebutton = TRUE;

									if (myImsg.MouseY == 0 && win->TopEdge == 0)
									{
										if (myImsg.MouseX == 0 && win->LeftEdge == 0)
										{
											checkinvisiblegadgets = TRUE;
										}
										else if (myImsg.MouseX == win->Width - 1 && myImsg.MouseX == mv->screen->screen->Width - 1)
										{
											checkinvisiblegadgets = TRUE;
										}
									}
									
									if (!checkinvisiblegadgets)
									{
										if (myImsg.MouseX >= x && myImsg.MouseX < x+w && myImsg.MouseY >= y && myImsg.MouseY < y+h)
										{
											newidcmp |= IDCMP_INTUITICKS;
											holdmx = myImsg.MouseX;
											holdmy = myImsg.MouseY;
											if (settings->leftmouseaction == MOUSEACTION_DRAG)
											{
												newidcmp |= IDCMP_MOUSEMOVE;
												MV_SetViewStart(viewdata->view, myImsg.MouseX, myImsg.MouseY);
											}
										}
									}
									lastclick_seconds = myImsg.Seconds;
									lastclick_micros = myImsg.Micros;
									break;
								}
							}
							
							if (newidcmp != currentidcmp)
							{
								ModifyIDCMP(win, newidcmp);
								currentidcmp = newidcmp;
							}
	
							break;
						}


				#if 0

							switch (settings->leftmouseaction)
							{
								case MOUSEACTION_DRAG:
									switch (myImsg.Code)
									{
										case SELECTUP:
											if (checkinvisiblegadgets)
											{
												if (myImsg.MouseY == 0)
												{
													if (myImsg.MouseX == 0)
													{
														status = STATUS_QUIT;
													}
													else if (myImsg.MouseX == mv->screen->screen->Width - 1)
													{
														ScreenToBack(mv->screen->screen);
													}
												}
											}
											else
											{
												ModifyIDCMP(win, mv->window->idcmpmask);
												mousebutton = FALSE;
											}
											checkinvisiblegadgets = FALSE;
											
											if (setviewrelative)
											{
												setviewrelative = FALSE;
											}
											break;

										case SELECTDOWN:
										{
											int x, y, w, h;
											MV_GetAttrs(viewdata->view,
												MVIEW_PictureX, &x, MVIEW_PictureY, &y,
												MVIEW_PictureWidth, &w, MVIEW_PictureHeight, &h, TAG_DONE);

											if (myImsg.MouseY == 0 && win->TopEdge == 0)
											{
												if (myImsg.MouseX == 0 && win->LeftEdge == 0)
												{
													checkinvisiblegadgets = TRUE;
												}
												else if (myImsg.MouseX == win->Width - 1 && myImsg.MouseX == mv->screen->screen->Width - 1)
												{
													checkinvisiblegadgets = TRUE;
												}
											}

											if (myImsg.MouseX >= x && myImsg.MouseX < x+w && myImsg.MouseY >= y && myImsg.MouseY < y+h)
											{
												//		ModifyIDCMP(win, mv->window->idcmpmask | IDCMP_MOUSEMOVE);
												MV_SetViewStart(viewdata->view, myImsg.MouseX, myImsg.MouseY);
												//		mousebutton = TRUE;
											}
											

									ModifyIDCMP(win, mv->window->idcmpmask | IDCMP_MOUSEMOVE | IDCMP_INTUITICKS);
									mousebutton = TRUE;

											lastclick_seconds = myImsg.Seconds;
											lastclick_micros = myImsg.Micros;

											break;
										}
									}
									break;

								case MOUSEACTION_NEXT:
									if (myImsg.Code == SELECTDOWN)
									{
										int x, y, w, h;
										MV_GetAttrs(viewdata->view,
											MVIEW_PictureX, &x, MVIEW_PictureY, &y,
											MVIEW_PictureWidth, &w, MVIEW_PictureHeight, &h, TAG_DONE);

										if (myImsg.MouseX >= x && myImsg.MouseX < x+w && myImsg.MouseY >= y && myImsg.MouseY < y+h)
										{
											if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
											{
												if (settings->autoquit) status = STATUS_QUIT;
												else if (settings->autohide) status = STATUS_SLEEP;
												else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
											}
										}
									}
									break;
							}
							break;
						}
			#endif
						case IDCMP_CLOSEWINDOW:
							status = settings->hideonclose ? STATUS_SLEEP : STATUS_QUIT;
							break;

						case IDCMP_NEWSIZE:
							softrefresh = TRUE;
							break;

						case IDCMP_REFRESHWINDOW:
							BeginRefresh(win);
							EndRefresh(win, TRUE);
							softrefresh = TRUE;
							break;

						case IDCMP_RAWKEY:
						{
							double inkrement = .01;

							if (myImsg.Qualifier & IEQUALIFIER_CONTROL)
								inkrement=1.0;

							if (myImsg.Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
								inkrement=.1;

							if (myImsg.Code >= 80 && myImsg.Code <= 89)
							{
								int slot = myImsg.Code - 80;
								if (myImsg.Qualifier & IEQUALIFIER_CONTROL)
								{
									if (MVFunction_UsePreset(mv, &settings, pathsettings, viewdata, slot, TRUE))
									{
										status = STATUS_REOPENALL;
									}
								}
								else //if (myImsg.Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT) || myImsg.Qualifier == 0)
								{
									int action = (myImsg.Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) ? FILEACTION_MOVE : FILEACTION_COPY;
									MVFunction_CopyMoveToPath(mv, settings, pathsettings, viewdata, slot, action, TRUE, buttons);
									SetupPathMenus(mv, pathsettings);
								}
							}
							else
							{
								switch (myImsg.Code)
								{
									case 76:
										viewdata->destypos -= inkrement;
										break;
									case 77:
										viewdata->destypos += inkrement;
										break;
									case 78:
										viewdata->destxpos += inkrement;
										break;
									case 79:
										viewdata->destxpos -= inkrement;
										break;
									
									case 95:	/* help */
									{
										ResetDisplaySettings(viewdata, TRUE);
										break;
									}
								}
							}

							break;
						}

						case IDCMP_VANILLAKEY:
						{
							double inkrement = .01;
						
							if (myImsg.Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
								inkrement = 1;

							if (myImsg.Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
								inkrement = .1;

							switch (myImsg.Code)
							{
								case 127:
									if (RemovePicture(viewdata->pichandler))
									{
									 	DeleteViewPic(viewdata, NULL, settings);
										GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
									}
									else
									{
										EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
									}
									break;

								case '8':
									viewdata->destypos -= inkrement;
									break;
								case '2':
									viewdata->destypos += inkrement;
									break;
								case '6':
									viewdata->destxpos += inkrement;
									break;
								case '4':
									viewdata->destxpos -= inkrement;
									break;

								case 8:
									if (viewdata->pichandler)
									{
										ResetPicHandler(viewdata->pichandler);
										GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal);

										SetTrigger(viewdata->trigger, 0);
										settings->slideshow = FALSE;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									}

								case '1':
									if (settings->displaymode != MVDISPMODE_KEEPASPECT_MIN)
									{
										settings->displaymode = MVDISPMODE_KEEPASPECT_MIN;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										Train(mv, settings, viewdata);
										MV_SetAttrs(viewdata->view, MVIEW_DisplayMode, settings->displaymode, TAG_DONE);
									}
									break;

								case '3':
									if (settings->displaymode != MVDISPMODE_IGNOREASPECT)
									{
										settings->displaymode = MVDISPMODE_IGNOREASPECT;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										Train(mv, settings, viewdata);
										MV_SetAttrs(viewdata->view, MVIEW_DisplayMode, settings->displaymode, TAG_DONE);
									}
									break;

								case 'i':	case 'I':	case '0':
									settings->picinfo = !settings->picinfo;
									SetViewText(viewdata, SetPicInfoText(viewdata, viewdata->showpic, settings));
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									break;

								case 'o':	case 'O':  case '7':
									requestfiles = REQUEST_OPENFILES;
									break;

								case '+':
									viewdata->destzoom += settings->zoomstep;
									break;

								case '-':
									viewdata->destzoom -= settings->zoomstep;
									break;

								case '[': case '(': case '{':
								{
									viewdata->angle -= 65536 * settings->rotatestep / 360;
									break;
								}

								case ']': case ')': case '}':
									viewdata->angle += 65536 * settings->rotatestep / 360;
									break;

								case '/':
									viewdata->angle = 0;
									break;

								case 'x': case 'X':
								{
									PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
									if (DoPictureMethod(pic, PICMTHD_FLIPX, NULL))
									{
										hardrefresh = TRUE;
									}
									break;
								}

					//			case ',':
					//			{
					//				PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
					//				if (DoPictureMethod(pic, PICMTHD_CONVOLVE, NULL))
					//				{
					//					hardrefresh = TRUE;
					//				}
					//				break;
					//			}

								case 'y': case 'Y':
								{
									PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
									if (DoPictureMethod(pic, PICMTHD_FLIPY, NULL))
									{
										hardrefresh = TRUE;
									}
									break;
								}

								case '~':
								{
									PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
									if (DoPictureMethod(pic, PICMTHD_NEGATIVE, NULL))
									{
										hardrefresh = TRUE;
									}
									break;
								}

								case 'a': case 'A':
									settings->showarrows = !settings->showarrows;
									MV_SetAttrs(viewdata->view, MVIEW_DrawArrows, settings->showarrows, TAG_DONE);
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									break;

								case 'b': case 'B':
									settings->showbuttons = !settings->showbuttons;
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									softrefresh = TRUE;
									break;

								case 'p': case 'P':
									settings->showpip = !settings->showpip;
									MV_SetAttrs(viewdata->view, MVIEW_ShowPip, settings->showpip, TAG_DONE);
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									break;

								case 'c': case 'C':
									settings->markcenter = !settings->markcenter;
									MV_SetAttrs(viewdata->view, MVIEW_ShowCursor, settings->markcenter, TAG_DONE);
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									break;

								case 32:
									if (settings->slideshow)
									{
										settings->slideshow = FALSE;
										SetTrigger(viewdata->trigger, 0);
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
									}
									else
									{
										settings->slideshow = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
										if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
										{
											if (settings->autoquit) status = STATUS_QUIT;
											else if (settings->autohide) status = STATUS_SLEEP;
											else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
										}
									}
									break;

								case 13:	
								
									/*
									**	dive into directory
									*/
								
									if (settings->browsemode)
									{
										if (viewdata->isdirectory)
										{
											char *newpath = FullName(viewdata->pathname, viewdata->filename);
											if (newpath)
											{
												char *newpath2 = _StrDupCat(NULL, newpath, "/#?", NULL);
												if (newpath2)
												{
													char *filepatternlist[2];
													filepatternlist[0] = newpath2;
													filepatternlist[1] = NULL;								
													newpicsignal = NewPicHandler(viewdata, filepatternlist, settings, mv->logopic);
													Free(newpath2);
												}
												Free(newpath);
											}
										}
									}
								
								case 'n': case 'N':
									if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
									{
										if (settings->autoquit) status = STATUS_QUIT;
										else if (settings->autohide) status = STATUS_SLEEP;
										else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
									}
									break;

								case '.':
									if (PrevPic(viewdata->pichandler))
									{
										SetTrigger(viewdata->trigger, 0);
										settings->slideshow = FALSE;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									}
									break;
								case 9:
									ZipWindow(win);
									break;

								case 27:
									status = settings->hideonescape ? STATUS_SLEEP : STATUS_QUIT;
									break;

								case '#':
								case '5':
									viewdata->destxpos = .5;
									viewdata->destypos = .5;
									break;

								case '*':
									viewdata->destzoom = 1;
									break;

								case 's':	case 'S':	case '9':
									if ((settings->slideshow = !settings->slideshow))
									{
										SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
									}
									else
									{
										SetTrigger(viewdata->trigger, 0);
									}
								//	SetMenuSettings(mv, settings);
									setmenusettings = TRUE;
									SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
									MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
									break;

								default:
									break;
							}
							break;
						}


						case IDCMP_MENUPICK:
						{
							while((myImsg.Code != MENUNULL) && (status == STATUS_WORKING))
							{
								struct MenuItem *item = ItemAddress(menu, myImsg.Code);
								ULONG code = (ULONG) GTMENUITEM_USERDATA(item);

								switch (code)
								{
									case MITEM_RESETALL:
									{
										ResetDisplaySettings(viewdata, TRUE);
										break;
									}
									case MITEM_CLEAR:
										signals &= ~newpicsignal;
										newpicsignal = NewPicHandler(viewdata, NULL, settings, mv->logopic);
										ResetDisplaySettings(viewdata, settings->resetdisplaysettings);
										break;

									case MITEM_AUTOCROP:
										settings->autocrop = GetItemFlag(item, TRUE, FALSE);
										SetPicHandlerAutoCrop(viewdata->pichandler, settings->autocrop);
										break;

									case MITEM_AUTODISPLAYMODE:
										if ((settings->autodisplaymode = GetItemFlag(item, TRUE, FALSE)))
										{
											if (!NeuralBase)
											{
												EasyRequest(win, &noneuralreq, NULL, NULL);
												settings->autodisplaymode = FALSE;
											//	SetMenuSettings(mv, settings);
												setmenusettings = TRUE;
											}
											else
											{
												int newdispmode;
												if ((newdispmode = Recall(mv, settings, viewdata)) != settings->displaymode)
												{
													settings->displaymode = newdispmode;
													MV_SetAttrs(viewdata->view, MVIEW_DisplayMode, settings->displaymode, TAG_DONE);
												//	SetMenuSettings(mv, settings);
													setmenusettings = TRUE;
													SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
												}
											}
										}
										break;

									case MITEM_LMB_DRAG:
										settings->leftmouseaction =
											GetItemFlag(item, MOUSEACTION_DRAG, MOUSEACTION_NEXT);
										break;

									case MITEM_ASYNCSCANNING:
										settings->asyncscanning = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_FILTERPICTURES:
										settings->filterpictures = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_APPENDICONS:
										settings->appendicons = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_CREATETHUMBNAILS:
										settings->createthumbnails = GetItemFlag(item, TRUE, FALSE);
										if (settings->createthumbnails && !NewIconBase)
										{
											EasyRequest(win, &nonewiconreq, NULL, NULL);
										}
										break;

									case MITEM_RESTART:
										if (viewdata->pichandler)
										{
											ResetPicHandler(viewdata->pichandler);
											GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal);

											SetTrigger(viewdata->trigger, 0);
											settings->slideshow = FALSE;
										//	SetMenuSettings(mv, settings);
											setmenusettings = TRUE;
											SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
											MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
										}
										break;

									case MITEM_SAVELIST:
									{
										if (viewdata->pichandler)
										{
											char **list;

											if ((list = PicHandlerCreateFileList(viewdata->pichandler)))
											{
												char *pathname, *filename, *fullname;
												if (SaveRequest(mv->listfreq, win, MVTEXT_SAVELISTAS, pathsettings->listpath, NULL, &pathname, &filename))
												{
													if (pathname)
													{
														Free(pathsettings->listpath);
														pathsettings->listpath = _StrDup(pathname);
													}

													if ((fullname = FullName(pathname, filename)))
													{
														WindowBusy(mv->window);
														if	(!SaveListFile(mv, list, fullname, settings->defaulttool))
														{
															EasyRequest(mv->window->window, &errorsavinglistreq, NULL, NULL);
														}
														WindowFree(mv->window);
														Free(fullname);
													}
													Free(pathname);
													Free(filename);
												}

												DeleteStringList(list);
											}
										}
										else
										{
											EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
										}

										break;
									}

									case MITEM_OPENLIST:
									{
										char *pathname, *filename, *fullname;
										if (FileRequest(mv->listfreq, win, MVTEXT_OPENLISTFILE, pathsettings->listpath, NULL, &pathname, &filename))
										{
											if (pathname)
											{
												Free(pathsettings->listpath);
												pathsettings->listpath = _StrDup(pathname);
											}

											if ((fullname = FullName(pathname, filename)))
											{
												char **piclist;
												if ((piclist = LoadStringList(fullname)))
												{
													newpicsignal = NewPicHandler(viewdata, piclist, settings, mv->logopic);
													DeleteFilePatternList(piclist);
												}
												Free(fullname);
											}
											Free(pathname);
											Free(filename);
										}
										break;
									}

									case MITEM_APPENDLIST:
									{
										char *pathname, *filename, *fullname;
										if (FileRequest(mv->listfreq, win, MVTEXT_APPENDLISTFILE, pathsettings->listpath, NULL, &pathname, &filename))
										{
											if (pathname)
											{
												Free(pathsettings->listpath);
												pathsettings->listpath = _StrDup(pathname);
											}

											if ((fullname = FullName(pathname, filename)))
											{
												char **piclist;
												if ((piclist = LoadStringList(fullname)))
												{
													AppendPicHandler(viewdata->pichandler, piclist);
													DeleteFilePatternList(piclist);
												}
												Free(fullname);
											}
											Free(pathname);
											Free(filename);
										}
										break;
									}

									case MITEM_PICINFO:
										settings->picinfo = GetItemFlag(item, TRUE, FALSE);
										SetViewText(viewdata, SetPicInfoText(viewdata, viewdata->showpic, settings));
										break;

									case MITEM_SHOWARROWS:
										settings->showarrows = GetItemFlag(item, TRUE, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_DrawArrows, settings->showarrows, TAG_DONE);
										break;

									case MITEM_SHOWPIP:
										settings->showpip = GetItemFlag(item, TRUE, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_ShowPip, settings->showpip, TAG_DONE);
										break;

									case MITEM_SHOWBUTTONS:
									{
										int newshowbuttons;
										newshowbuttons = GetItemFlag(item, TRUE, FALSE);
										if (newshowbuttons != settings->showbuttons)
										{
											settings->showbuttons = newshowbuttons;
											softrefresh = TRUE;
										}
										break;
									}

									case MITEM_MARKCENTER:
										settings->markcenter = GetItemFlag(item, TRUE, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_ShowCursor, settings->markcenter, TAG_DONE);
										break;

									case MITEM_OPEN:
										requestfiles = REQUEST_OPENFILES;
										break;

									case MITEM_APPEND:
										requestfiles = REQUEST_APPENDFILES;
										break;

									case MITEM_NEXT:
										if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
										{
											if (settings->autoquit) status = STATUS_QUIT;
											else if (settings->autohide) status = STATUS_SLEEP;
											else if (settings->requestfile) requestfiles = REQUEST_OPENFILES;
										}
										break;

									case MITEM_PREV:
										if (PrevPic(viewdata->pichandler))
										{
											SetTrigger(viewdata->trigger, 0);
											settings->slideshow = FALSE;
										//	SetMenuSettings(mv, settings);
											setmenusettings = TRUE;
											SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
											MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
										}
										break;

									case MITEM_ABOUT:
										MVFunction_About(mv);
										break;

									case MITEM_SLIDE:
										if ((settings->slideshow = GetItemFlag(item, TRUE, FALSE)))
										{
											SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
										}
										else
										{
											SetTrigger(viewdata->trigger, 0);
										}
										MV_SetAttrs(viewdata->view, MVIEW_Text, (IPTR)SetPicInfoText(viewdata, viewdata->showpic, settings), TAG_DONE);
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										break;

									case MITEM_QUIT:
										status = STATUS_QUIT;
										break;

									case MITEM_HIDE:
										status = STATUS_SLEEP;
										break;

									case MITEM_ZIPWINDOW:
										ZipWindow(win);
										break;

									case MITEM_ZOOMIN:
										viewdata->destzoom += settings->zoomstep;
										break;

									case MITEM_ZOOMOUT:
										viewdata->destzoom -= settings->zoomstep;
										break;

									case MITEM_RESETZOOM:
										viewdata->destzoom = 1;
										break;

									case MITEM_ROTATELEFT:
										viewdata->angle -= 65536 * settings->rotatestep / 360;
										break;

									case MITEM_ROTATERIGHT:
										viewdata->angle += 65536 * settings->rotatestep / 360;
										break;

									case MITEM_RESETROT:
										viewdata->angle = 0;
										break;

									case MITEM_RESETPOS:
										viewdata->destxpos = .5;
										viewdata->destypos = .5;
										break;

									case MITEM_DITHERON:
									case MITEM_DITHERAUTO:
										settings->dither = GetMenuFlag(menu, MITEM_DITHERON, MVDITHERMODE_ON, MVDITHERMODE_OFF);
										settings->dither = GetMenuFlag(menu, MITEM_DITHERAUTO, MVDITHERMODE_AUTO, settings->dither);
										MV_SetAttrs(viewdata->view, MVIEW_Dither, settings->dither, TAG_DONE);
										break;

									case MITEM_AUTOQUIT:
										if ((settings->autoquit = GetItemFlag(item, TRUE, FALSE)))
										{
											settings->autohide = FALSE;
											settings->loop = FALSE;
										//	SetMenuSettings(mv, settings);
											setmenusettings = TRUE;
										}
										break;

									case MITEM_AUTOHIDE:
										if ((settings->autohide = GetItemFlag(item, TRUE, FALSE)))
										{
											settings->autoquit = FALSE;
											settings->loop = FALSE;
										//	SetMenuSettings(mv, settings);
											setmenusettings = TRUE;
										}
										break;

									case MITEM_AUTOSTOP:
										settings->autostop = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_AUTOCLEAR:
										settings->autoclear = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_LOOP:
										if ((settings->loop = GetItemFlag(item, TRUE, FALSE)))
										{
											settings->autoquit = FALSE;
											settings->autohide = FALSE;
										//	SetMenuSettings(mv, settings);
											setmenusettings = TRUE;
										}
										SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										break;

									case MITEM_RESETDISPLAYSETTINGS:
										settings->resetdisplaysettings = GetItemFlag(item, TRUE, FALSE);
										ResetDisplaySettings(viewdata, settings->resetdisplaysettings);
										break;

									case MITEM_OPAQUE:
									case MITEM_GRID:
										settings->previewmode =
											GetMenuFlag(menu, MITEM_OPAQUE, MVPREVMODE_OPAQUE, 0) +
											GetMenuFlag(menu, MITEM_GRID, MVPREVMODE_GRID, 0);
										MV_SetAttrs(viewdata->view, MVIEW_PreviewMode, settings->previewmode, TAG_DONE);
										break;

									case MITEM_KEEPASPECT1:
									case MITEM_IGNOREASPECT:
									{
										int newdisplaymode =
											GetMenuFlag(menu, MITEM_KEEPASPECT1, MVDISPMODE_KEEPASPECT_MIN, 0) +
											GetMenuFlag(menu, MITEM_IGNOREASPECT, MVDISPMODE_IGNOREASPECT, 0);

										if (newdisplaymode != settings->displaymode)
										{
											settings->displaymode = newdisplaymode;
											MV_SetAttrs(viewdata->view, MVIEW_DisplayMode, settings->displaymode, TAG_DONE);
											Train(mv, settings, viewdata);
											SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
										}
										break;
									}

									case MITEM_STATIC:
										settings->staticpalette = GetItemFlag(item, TRUE, FALSE);
										MV_SetAttrs(viewdata->view, MVIEW_StaticPalette, settings->staticpalette, TAG_DONE);
										break;

									case MITEM_BACKDROP:
										settings->borderless = GetItemFlag(item, TRUE, FALSE);
										status = STATUS_REOPENWINDOW;
										break;

									case MITEM_AUTOSAVESETTINGS:
										pathsettings->autosavesettings = GetItemFlag(item, TRUE, FALSE);
										break;

									case MITEM_REQUESTFILE:
										settings->requestfile = GetItemFlag(item, TRUE, FALSE) ?
											REQUEST_OPENFILES : REQUEST_NONE;
										break;

									case MITEM_CUSTOMSCREEN:
										if (ModeRequest(mv->modereq, win, settings, MVTEXT_SELECTSCREENMODE))
										{
											settings->scropenmode = SCROPENMODE_CUSTOM;
											status = STATUS_REOPENALL;
										}
										break;

									case MITEM_PUBLICSCREEN:
									case MITEM_DEFAULTSCREEN:
										settings->scropenmode =
											GetMenuFlag(menu, MITEM_CUSTOMSCREEN, SCROPENMODE_CUSTOM, 0) +
											GetMenuFlag(menu, MITEM_PUBLICSCREEN, SCROPENMODE_PUBLIC, 0) +
											GetMenuFlag(menu, MITEM_DEFAULTSCREEN, SCROPENMODE_NONE, 0);
										status = STATUS_REOPENALL;
										break;

									case MITEM_SMALLWINDOW:
									case MITEM_LARGEWINDOW:
									case MITEM_HALFWINDOW:
									case MITEM_FULLWINDOW:
									case MITEM_VISIBLEWINDOW:
									case MITEM_FIXEDWINDOWSIZE:
										settings->winsizemode =
											GetMenuFlag(menu, MITEM_SMALLWINDOW, WSIZEMODE_33PERCENT, 0) +
											GetMenuFlag(menu, MITEM_HALFWINDOW, WSIZEMODE_HALF, 0) +
											GetMenuFlag(menu, MITEM_LARGEWINDOW, WSIZEMODE_66PERCENT, 0) +
											GetMenuFlag(menu, MITEM_VISIBLEWINDOW, WSIZEMODE_VISIBLE, 0) +
											GetMenuFlag(menu, MITEM_FULLWINDOW, WSIZEMODE_FULL, 0) +
											GetMenuFlag(menu, MITEM_FIXEDWINDOWSIZE, WSIZEMODE_NONE, 0);

										if (settings->winsizemode == WSIZEMODE_NONE)
										{
											UpdateWindowParameters(mv->window);
											settings->winwidth = mv->window->winwidth;
											settings->winheight = mv->window->winheight;
										}
										else
										{
											status = STATUS_REOPENWINDOW;
										}
										break;

									case MITEM_SORT_NONE:
									case MITEM_SORT_ALPHA_FILE:
									case MITEM_SORT_ALPHA_FULL:
									case MITEM_SORT_DATE:
									case MITEM_SORT_RANDOM:
									case MITEM_SORT_FILESIZE:
									case MITEM_SORT_REVERSE:
										settings->sortmode =
											GetMenuFlag(menu, MITEM_SORT_NONE, SORTMODE_NONE, 0) +
											GetMenuFlag(menu, MITEM_SORT_ALPHA_FILE, SORTMODE_ALPHA_FILE, 0) +
											GetMenuFlag(menu, MITEM_SORT_RANDOM, SORTMODE_RANDOM, 0) +
											GetMenuFlag(menu, MITEM_SORT_FILESIZE, SORTMODE_FILESIZE, 0) +
											GetMenuFlag(menu, MITEM_SORT_DATE, SORTMODE_DATE, 0) +
											GetMenuFlag(menu, MITEM_SORT_ALPHA_FULL, SORTMODE_ALPHA_FULL, 0);
										settings->reverse = GetMenuFlag(menu, MITEM_SORT_REVERSE, TRUE, FALSE);
										if (viewdata->pichandler)
										{
											WindowBusy(mv->window);
											SetSortMode(viewdata->pichandler, settings->sortmode, settings->reverse);
											WindowFree(mv->window);
										}
										SetViewText(viewdata, SetPicInfoText(viewdata, mv->logopic, settings));
										break;

									case MITEM_CENTERWINDOW:
									case MITEM_MOUSEWINDOW:
									case MITEM_FIXEDWINDOWPOS:
										settings->winopenmode =
											GetMenuFlag(menu, MITEM_CENTERWINDOW, WOPENMODE_CENTER, 0) +
											GetMenuFlag(menu, MITEM_MOUSEWINDOW, WOPENMODE_MOUSE, 0) +
											GetMenuFlag(menu, MITEM_FIXEDWINDOWPOS, WOPENMODE_NONE, 0);
										if (settings->winopenmode == WOPENMODE_NONE)
										{
											UpdateWindowParameters(mv->window);
											settings->winleft = mv->window->winleft;
											settings->wintop = mv->window->wintop;
										}
										else
										{
											status = STATUS_REOPENWINDOW;
										}
										break;

									case MITEM_SNAPWINDOW:
										UpdateWindowParameters(mv->window);
										settings->winwidth = mv->window->winwidth;
										settings->winheight = mv->window->winheight;
										settings->winleft = mv->window->winleft;
										settings->wintop = mv->window->wintop;
										settings->winsizemode = WSIZEMODE_NONE;
										settings->winopenmode = WOPENMODE_NONE;
									//	SetMenuSettings(mv, settings);
										setmenusettings = TRUE;
										break;

									case MITEM_SAVESETTINGS:
										WindowBusy(mv->window);
										SaveDefaultSettings(settings, mv->progfilename);
										WindowFree(mv->window);
										break;

									case MITEM_RESETSETTINGS:
									{
										struct mainsettings *new;
										if ((new = CreateMainSettings(NULL, NULL, NULL)))
										{
											DeleteMainSettings(settings);
											settings = new;
											status = STATUS_REOPENALL;
										}
										break;
									}

									case MITEM_RESTORESETTINGS:
									{
										struct mainsettings *new;
										if ((new = CreateMainSettings(NULL, oldsettings, NULL)))
										{
											DeleteMainSettings(settings);
											settings = new;
											status = STATUS_REOPENALL;
										}
										break;
									}

									case MITEM_LASTSETTINGS:
									{
										struct mainsettings *new;
										WindowBusy(mv->window);
										if ((new = LoadDefaultSettings(mv->progfilename)))
										{
											DeleteMainSettings(settings);
											settings = new;
											status = STATUS_REOPENALL;
										}
										WindowFree(mv->window);
										break;
									}

									case MITEM_FLIPX:
									{
										PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
										WindowBusy(mv->window);
										if (DoPictureMethod(pic, PICMTHD_FLIPX, NULL))
										{
											hardrefresh = TRUE;
										}
										WindowFree(mv->window);
										break;
									}

									case MITEM_FLIPY:
									{
										PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
										WindowBusy(mv->window);
										if (DoPictureMethod(pic, PICMTHD_FLIPY, NULL))
										{
											hardrefresh = TRUE;
										}
										WindowFree(mv->window);
										break;
									}

									case MITEM_NEGATIVE:
									{
										PICTURE *pic = viewdata->showpic ? viewdata->showpic : mv->logopic;
										WindowBusy(mv->window);
										if (DoPictureMethod(pic, PICMTHD_NEGATIVE, NULL))
										{
											hardrefresh = TRUE;
										}
										WindowFree(mv->window);
										break;
									}

									case MITEM_PRESET1:
									case MITEM_PRESET1+1:
									case MITEM_PRESET1+2:
									case MITEM_PRESET1+3:
									case MITEM_PRESET1+4:
									case MITEM_PRESET1+5:
									case MITEM_PRESET1+6:
									case MITEM_PRESET1+7:
									case MITEM_PRESET1+8:
									case MITEM_PRESET1+9:
									{
										int slot = code - MITEM_PRESET1;
										if (MVFunction_UsePreset(mv, &settings, pathsettings, viewdata, slot, FALSE))
										{
											status = STATUS_REOPENALL;
										}
										break;
									}

									case MITEM_COPYTO:
									{
										MVFunction_CopyMoveTo(mv, settings, pathsettings, viewdata, FILEACTION_COPY, FALSE, buttons);
										relayoutmenus = TRUE;
										break;
									}
									case MITEM_MOVETO:
									{
										MVFunction_CopyMoveTo(mv, settings, pathsettings, viewdata, FILEACTION_MOVE, FALSE, buttons);
										relayoutmenus = TRUE;
										break;
									}

									case MITEM_COPY1:
									case MITEM_COPY1+1:
									case MITEM_COPY1+2:
									case MITEM_COPY1+3:
									case MITEM_COPY1+4:
									case MITEM_COPY1+5:
									case MITEM_COPY1+6:
									case MITEM_COPY1+7:
									case MITEM_COPY1+8:
									case MITEM_COPY1+9:
									case MITEM_MOVE1:
									case MITEM_MOVE1+1:
									case MITEM_MOVE1+2:
									case MITEM_MOVE1+3:
									case MITEM_MOVE1+4:
									case MITEM_MOVE1+5:
									case MITEM_MOVE1+6:
									case MITEM_MOVE1+7:
									case MITEM_MOVE1+8:
									case MITEM_MOVE1+9:
									{
										BOOL action = (code >= MITEM_MOVE1 && code <= MITEM_MOVE1+9) ? FILEACTION_MOVE : FILEACTION_COPY;
										int slot = code - (action == FILEACTION_MOVE ? MITEM_MOVE1 : MITEM_COPY1);
										if (slot >= 0 && slot < 10)
										{
											MVFunction_CopyMoveToPath(mv, settings, pathsettings, viewdata, slot, action, FALSE, buttons);
											relayoutmenus = TRUE;
										}
										break;
									}

									case MITEM_DELETE:
									{
										char *fullsourcename;

										if ((fullsourcename = GetPicFile(viewdata->pichandler)))
										{
											if (PerformFileAction(mv, fullsourcename, fullsourcename, FILEACTION_DELETE, settings->confirmlevel))
											{
												if (RemovePicture(viewdata->pichandler))
												{
												 	DeleteViewPic(viewdata, NULL, settings);
													GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
												}
											//	else
											//	{
											//		printf("ALERT: Delete Picture - something went wrong\n");
											//	}
											}
											Free(fullsourcename);
										}
										else
										{
											EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
										}
										break;
									}

									case MITEM_REMOVE:
									{
										if (RemovePicture(viewdata->pichandler))
										{
										 	DeleteViewPic(viewdata, NULL, settings);
											GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
										}
										else
										{
											EasyRequest(mv->window->window, &nopicreq, NULL, NULL);
										}
										break;
									}

									case MITEM_CACHE_OFF:
										settings->bufferpercent = 1;
										SetPicHandlerBuffer(viewdata->pichandler, settings->bufferpercent);
										break;
									case MITEM_CACHE_SMALL:
										settings->bufferpercent = 15;
										SetPicHandlerBuffer(viewdata->pichandler, settings->bufferpercent);
										break;
									case MITEM_CACHE_MEDIUM:
										settings->bufferpercent = 25;
										SetPicHandlerBuffer(viewdata->pichandler, settings->bufferpercent);
										break;
									case MITEM_CACHE_LARGE:
										settings->bufferpercent = 35;
										SetPicHandlerBuffer(viewdata->pichandler, settings->bufferpercent);
										break;
									case MITEM_CACHE_HUGE:
										settings->bufferpercent = 50;
										SetPicHandlerBuffer(viewdata->pichandler, settings->bufferpercent);
										break;

									case MITEM_SLIDEDELAY_0:
										settings->slidedelay = 0;
										break;
									case MITEM_SLIDEDELAY_04:
										settings->slidedelay = 4;
										break;
									case MITEM_SLIDEDELAY_1:
										settings->slidedelay = 10;
										break;
									case MITEM_SLIDEDELAY_2:
										settings->slidedelay = 20;
										break;
									case MITEM_SLIDEDELAY_5:
										settings->slidedelay = 50;
										break;
									case MITEM_SLIDEDELAY_10:
										settings->slidedelay = 100;
										break;
									case MITEM_SLIDEDELAY_30:
										settings->slidedelay = 300;
										break;
									case MITEM_SLIDEDELAY_60:
										settings->slidedelay = 600;
										break;
									case MITEM_SLIDEDELAY_120:
										settings->slidedelay = 1200;
										break;
									case MITEM_SLIDEDELAY_300:
										settings->slidedelay = 3000;
										break;
									case MITEM_SLIDEDELAY_600:
										settings->slidedelay = 6000;
										break;

									case MITEM_SAVEPRESET:
									{
										char *pathname, *filename, *fullname;
										if (SaveRequest(mv->presetfreq, win, MVTEXT_SAVEPRESETAS, pathsettings->presetpath, NULL, &pathname, &filename))
										{
											if (pathname)
											{
												Free(pathsettings->presetpath);
												pathsettings->presetpath = _StrDup(pathname);
											}

											if ((fullname = FullName(pathname, filename)))
											{
												WindowBusy(mv->window);
												SavePreset(mv, settings, fullname);
												WindowFree(mv->window);
												Free(fullname);
											}
											Free(pathname);
											Free(filename);
										}
										break;
									}

									case MITEM_LOADPRESET:
									{
										char *pathname, *filename, *fullname;
										if (FileRequest(mv->presetfreq, win, MVTEXT_LOADPRESET, pathsettings->presetpath, NULL, &pathname, &filename))
										{
											if (pathname)
											{
												Free(pathsettings->presetpath);
												pathsettings->presetpath = _StrDup(pathname);
											}

											if ((fullname = FullName(pathname, filename)))
											{
												struct mainsettings *new;
												WindowBusy(mv->window);
												if ((new = LoadPreset(fullname, settings)))
												{
													DeleteMainSettings(settings);
													settings = new;
													status = STATUS_REOPENALL;
												}
												WindowFree(mv->window);
												Free(fullname);
											}
											Free(pathname);
											Free(filename);
										}
										break;
									}

									default:
										break;
								}

								if (code >= MITEM_PUBSCREEN1)
								{
									int slot = code - MITEM_PUBSCREEN1;
									Free(settings->pubscreen);
									settings->pubscreen = _StrDup(mv->pubscreenlist[slot + 3]);
									settings->scropenmode = SCROPENMODE_PUBLIC;
									status = STATUS_REOPENALL;
									break;
								}

								myImsg.Code = item->NextSelect;
							}

							break;
						}

						default:
							break;
					}
				}


				if (relayoutmenus)
				{
					SetupPathMenus(mv, pathsettings);
					SetupPresetMenu(mv, pathsettings);
					SetupPubscreenMenu(mv, settings);
				}

				if (softrefresh)
				{
					int newdispmode;

					if ((newdispmode = Recall(mv, settings, viewdata)) != settings->displaymode)
					{
						settings->displaymode = newdispmode;
					//	SetMenuSettings(mv, settings);
						setmenusettings = TRUE;
					}

					SetButtonSettings(viewdata, mv->window, buttons, settings, TRUE);

					UpdateWindowParameters(mv->window);
					
					MV_SetAttrs(viewdata->view,
						MVIEW_DestWidth, mv->window->innerwidth,
						MVIEW_DestHeight, mv->window->innerheight - (settings->showbuttons && buttons ? buttons->gadgetheight + BUTTON_VSPACE * 2 : 0),
						MVIEW_DestX, mv->window->innerleft,
						MVIEW_DestY, mv->window->innertop + (settings->showbuttons && buttons ? buttons->gadgetheight + BUTTON_VSPACE * 2 : 0),
						MVIEW_DisplayMode, settings->displaymode,
						TAG_DONE);
				}


			//	MV_SetPIPRelative(viewdata->view, myImsg.MouseX, myImsg.MouseY, mousebutton);
	
				if (setviewrelative)
				{
					IPTR newxpos, newypos;
					MV_SetViewRelative(viewdata->view, myImsg.MouseX, myImsg.MouseY);
					MV_GetAttrs(viewdata->view, MVIEW_XPos, (IPTR)&newxpos, MVIEW_YPos, (IPTR)&newypos, TAG_DONE);
					viewdata->xpos = viewdata->destxpos = (double) newxpos / 65536;
					viewdata->ypos = viewdata->destypos = (double) newypos / 65536;
				}


				if (setmenusettings)
				{
					SetMenuSettings(mv, settings);
				}
			}


			ApplyViewData(viewdata, settings, TRUE);


			if (hardrefresh)
			{
				MV_Refresh(viewdata->view);
			}


			/*
			**	request files
			*/

			if (requestfiles != REQUEST_NONE)
			{
				char **filepatternlist;
				char *newpath, *newfile;

				if ((filepatternlist = MultiFileRequest(mv->freq, win,
					requestfiles == REQUEST_OPENFILES ? MVTEXT_SELECTPICTURES : MVTEXT_ADDPICTURES,
					viewdata->pathname, NULL /*viewdata->filename*/, &newpath, &newfile, settings->filterpictures, settings->rejectpattern)))
				{
					if (requestfiles == REQUEST_APPENDFILES && viewdata->pichandler)
					{
						AppendPicHandler(viewdata->pichandler, filepatternlist);
					}
					else
					{
						newpicsignal = NewPicHandler(viewdata, filepatternlist, settings, mv->logopic);
					}

					if (newpath)
					{
						Free(pathsettings->startpath);
						pathsettings->startpath = _StrDup(newpath);
					}

					Free(newpath);
					Free(newfile);
					DeleteFilePatternList(filepatternlist);
				}
				requestfiles = REQUEST_NONE;
			}


			/*
			**	appmessage loop
			*/

			if (signals & appsignal)
			{
				char **newfilepatternlist = NULL;
				char **filepatternlist = NULL;

				ScreenToFront(mv->screen->screen);
				WindowToFront(mv->window->window);

				while ((appmsg = (struct AppMessage *) GetMsg(mv->appmsgport)))
				{
					DeleteFilePatternList(newfilepatternlist);
					if ((newfilepatternlist = CreateFilePatternList(appmsg->am_ArgList, appmsg->am_NumArgs)))
					{
						DeleteFilePatternList(filepatternlist);
						filepatternlist = newfilepatternlist;
						newfilepatternlist = NULL;
					}
					ReplyMsg((struct Message *) appmsg);
				}

				if (filepatternlist)
				{
					if (FilterFilePatternList(&filepatternlist, &settings) & 2)
					{
						status = STATUS_REOPENALL;
					}
				}

				if (filepatternlist)
				{
					if (settings->appendicons && viewdata->pichandler)
					{
						AppendPicHandler(viewdata->pichandler, filepatternlist);
					}
					else
					{
						newpicsignal = NewPicHandler(viewdata, filepatternlist, settings, mv->logopic);
					}
				}

				DeleteFilePatternList(filepatternlist);
			}
		}


		/*
		**	commodity broker
		*/

		if (signals & cxsignal)
		{
			ULONG msgid, msgtype;
			while ((cxmsg = (CxMsg *) GetMsg(mv->brokermsgport)))
			{
				msgid = CxMsgID(cxmsg);
				msgtype = CxMsgType(cxmsg);
				ReplyMsg((struct Message *) cxmsg);

				if (msgtype == CXM_COMMAND)
				{
					switch (msgid)
					{
						case CXCMD_ENABLE:
						case CXCMD_DISABLE:
							break;

						case CXCMD_DISAPPEAR:
							status = STATUS_SLEEP;
							break;

						case CXCMD_KILL:
							status = STATUS_QUIT;
							break;

						default:
							break;
					}
				}
			}
		}


		/*
		**		trigger signal arrived
		*/

		if (signals & triggersignal)
		{
			if (GetNextPicture(mv, buttons, viewdata, settings, mv->logopic, &newpicsignal))
			{
				if (settings->autoquit)	status = STATUS_QUIT;
				else if (settings->autohide)	status = STATUS_SLEEP;
				else if (settings->requestfile) requestfiles = TRUE;
			}
		}


		/*
		**		new picture arrived
		*/

		if (signals & newpicsignal)
		{
			if (viewdata->pichandler)
			{
				LONG error;

				if (NewPicAvailable(viewdata->pichandler, &error))
				{
					PICTURE *newpic;
					char *fullname;

					Free(viewdata->filename); viewdata->filename = NULL;
					Free(viewdata->pathname); viewdata->pathname = NULL;
					Free(viewdata->formatname); viewdata->formatname = NULL;

					if ((fullname = GetPicFile(viewdata->pichandler)))
					{
						viewdata->formatname = GetPicFormat(viewdata->pichandler);
						viewdata->isdirectory = GetPicFileType(viewdata->pichandler);
						viewdata->filename = _StrDup(FilePart(fullname));
						*PathPart(fullname) = (char) 0;
						viewdata->pathname = _StrDup(fullname);
						Free(fullname);
					}

					ResetDisplaySettings(viewdata, settings->resetdisplaysettings);

					if ((newpic = GetPic(viewdata->pichandler)))
					{
						int newdispmode;

						viewdata->showpic = newpic;

						if ((newdispmode = Recall(mv, settings, viewdata)) != settings->displaymode)
						{
							settings->displaymode = newdispmode;
							SetMenuSettings(mv, settings);
							SetButtonSettings(viewdata, mv->window, buttons, settings, FALSE);
						}

						MV_SetAttrs(viewdata->view,
							MVIEW_Picture, viewdata->showpic ? (IPTR)viewdata->showpic : (IPTR)mv->logopic,
							MVIEW_DisplayMode, settings->displaymode,
							MVIEW_Text, (IPTR)SetPicInfoText(viewdata, newpic, settings), TAG_DONE);

						MVFunction_CreateAutoThumbnail(mv->window, settings, viewdata);

						if (settings->slideshow)
						{
							slidesuspended = TRUE;		// wait for picture to be fully visible!
						}
					}
				}
				else
				{
				
					if (error == ERROR_OBJECT_WRONG_TYPE && settings->simplescanning)
					{
						/*
						**	unrecognized file and simplescanning mode on:
						**	silently remove the non-picture from the list
						*/
					
						if (PicHandler_Remove(viewdata->pichandler->newPicHandler,
							viewdata->pichandler->requestedID))
						{
							viewdata->pichandler->error = 0;
							Free(viewdata->pichandler->errortext);
							viewdata->pichandler->errortext = NULL;
						 	DeleteViewPic(viewdata, NULL, settings);
							GetCurrentPicture(mv, buttons, viewdata, settings, mv->logopic);
						}
					}
					else
					{
						char *text;
						text = SetPicInfoText(viewdata, viewdata->showpic, settings);

						if (error)
						{
							viewdata->destzoom = 1;
							viewdata->destxpos = 0.5;
							viewdata->destypos = 0.5;
		
							DeleteViewPic(viewdata, mv->logopic, settings);
							SetViewText(viewdata, text);
							ApplyViewData(viewdata, settings, TRUE);
						}
						else
						{
							SetViewText(viewdata, text);
						}
					}
				}
			}
		}


		if (slidesuspended)		//wait for picture to be fully visible
		{
			if (settings->slideshow)
			{
				if (signals & (1 << viewdata->picturevisiblesignal))
				{
					slidesuspended = FALSE;		// fully visible picture breaks suspended state
					SetTrigger(viewdata->trigger, settings->slidedelay ? settings->slidedelay * 5 : MINDELAY);
				}
				else
				{
					SetTrigger(viewdata->trigger, 0);
				}
			}
			else
			{
				slidesuspended = FALSE;
			}
		}


		/*
		**	break
		*/

		if (signals & SIGBREAKF_CTRL_C)
		{
			status = STATUS_QUIT;
		}


	} while (status == STATUS_WORKING);




	WindowBusy(mv->window);

	DeleteButtons(buttons);

	MV_Delete(viewdata->view);
	viewdata->view = NULL;

	FreePubscreenMenu(mv);
	FreePathMenus(mv);
	FreePresetMenu(mv);

	*initsettings = settings;
	*initpathsettings = pathsettings;

	WindowFree(mv->window);

	return status;
}


/*********************************************************************
----------------------------------------------------------------------

	main

----------------------------------------------------------------------
*********************************************************************/

int mymain(int argc, char **argv);

#ifdef __MORPHOS__
extern struct WBStartup *_WBenchMsg;
#endif

#ifdef __STORM__

struct WBStartup *_WBenchMsg;
void wbmain(struct WBStartup *argmsg)
{
	struct WBArg *wbarg;

	_WBenchMsg = argmsg;

	wbarg = argmsg->sm_ArgList;
	if (wbarg->wa_Lock)
	{
		BPTR olddir;
		olddir = CurrentDir(wbarg->wa_Lock);
		mymain(0, (char **) argmsg);
		CurrentDir(olddir);
	}
	else
	{
		mymain(0, (char **) argmsg);
	}
}

#endif


int main(int argc, char **argv)
{
	return mymain(argc, argv);
}


int mymain(int argc, char **argv)
{
	char **startfilepatternlist;
	char **wbargarray = NULL;
	char *progfilename = NULL;
	char **ttypes;

	char **piclist = NULL;


	struct mview *viewer;
	struct mainsettings *initsettings = NULL;
	struct pathsettings *initpathsettings;

	int result;

	struct RDArgs *rdargs = NULL;
	long opts[RDNUMARGS];

	memset((void *) opts, 0, (size_t) (sizeof(long) * RDNUMARGS));

	if (InitGlobal())
	{
		struct keyfile *key = InstallKey();

		ttypes = (char **) ArgArrayInit((LONG) argc, (UBYTE **) argv);

		MVNeuro_CreateNet();

		initpathsettings = LoadPathSettings("ENV:MysticView", NULL);

    	#ifdef __AROS__
	#warning "Fix AROS WBenchMsg/_WBenchMsg/not_in_any_header stuff"
	#define _WBenchMsg WBenchMsg
	extern struct WBStartup *WBenchMsg;
	#endif
		if(_WBenchMsg)
		{
			char fullname[MAXFULLNAMELEN];
			struct mainsettings *newsettings;

			if (NameFromLock(GetProgramDir(), fullname, MAXPATHNAMELEN))
			{
				if (AddPart(fullname, _WBenchMsg->sm_ArgList[0].wa_Name, MAXFULLNAMELEN))
				{
					progfilename = _StrDup(fullname);
				}
			}

			initsettings = LoadDefaultSettings(progfilename);

			if ((newsettings = CreateMainSettings(ttypes, initsettings, NULL)))
			{
				DeleteMainSettings(initsettings);
				initsettings = newsettings;
			}

			wbargarray = CreateFilePatternList(&_WBenchMsg->sm_ArgList[1], _WBenchMsg->sm_NumArgs - 1);

			/*
			**	check for preset and listfiles
			*/

			if (wbargarray)
			{
				FilterFilePatternList(&wbargarray, &initsettings);
			}

			startfilepatternlist = wbargarray;

		}
		else
		{
			char fullname[MAXFULLNAMELEN];
			struct mainsettings *newsettings;

			rdargs = ReadArgs(RDARGTEMPLATE, opts, NULL);
			startfilepatternlist = (char **) opts[0];


			if (NameFromLock(GetProgramDir(), fullname, MAXPATHNAMELEN))
			{
				if (AddPart(fullname, argv[0], MAXFULLNAMELEN))
				{
					progfilename = _StrDup(fullname);
				}
			}


			if (!opts[ARG_DEFAULT])			// do not read config when DEFAULT is specified
			{
				struct DiskObject *dob;

				if ((dob = GetDiskObject(progfilename)))
				{
					initsettings = CreateMainSettings((char **)dob->do_ToolTypes, NULL, NULL);
					FreeDiskObject(dob);
				}
			}


			//	use list file?

			if (opts[ARG_LISTFILE])
			{
				if ((piclist = LoadStringList((char *) opts[ARG_LISTFILE])))
				{
					startfilepatternlist = piclist;
				}
				else
				{
					char *fullname = FullName(initpathsettings->listpath, FilePart((char *)opts[ARG_LISTFILE]));
					if ((piclist = LoadStringList(fullname)))
					{
						startfilepatternlist = piclist;
					}
					Free(fullname);
				}
			}

			// use preset?

			if (opts[ARG_PRESETFILE])
			{
				struct mainsettings *new;


				if ((new = LoadPreset((char *) opts[ARG_PRESETFILE], initsettings)))
				{
					DeleteMainSettings(initsettings);
					initsettings = new;
				}
				else
				{
					char *fullname = FullName(initpathsettings->presetpath, FilePart((char *) opts[ARG_PRESETFILE]));
					if ((new = LoadPreset(fullname, initsettings)))
					{
						DeleteMainSettings(initsettings);
						initsettings = new;
					}
					Free(fullname);
				}
			}
			if ((newsettings = CreateMainSettings(ttypes, initsettings, NULL)))
			{
				DeleteMainSettings(initsettings);
				initsettings = newsettings;
			}

			if (opts[ARG_APPEND] && startfilepatternlist)
			{
				if (PicHandler_AppendScanList(startfilepatternlist,
					PICH_Reject, initsettings ? (ULONG) initsettings->rejectpattern : (ULONG) TRUE,
					PICH_Recursive, initsettings ? (ULONG) initsettings->recursive : (ULONG) TRUE,
					TAG_DONE))
				{
					goto appended_raus;
				}
			}

		}


		globalpriority = SetTaskPri(FindTask(NULL), MAXPRIORITY);
		SetTaskPri(FindTask(NULL), globalpriority);


		if (!(SetSignal(0,0) & SIGBREAKF_CTRL_C))
		{
			struct mainsettings *settings = NULL;
			struct mainsettings *oldsettings;
			struct pathsettings *pathsettings = NULL;

			if ((settings = CreateMainSettings(NULL, initsettings, NULL)))
			{
			//	if (pathsettings = LoadPathSettings("ENV:MysticView", initpathsettings))
				if ((pathsettings = CreatePathSettings(NULL, initpathsettings, NULL)))
				{
					if ((oldsettings = CreateMainSettings(NULL, settings, NULL)))
					{
						struct viewdata *viewdata;

						if ((viewdata = CreateViewData(startfilepatternlist)))
						{
							BOOL reopen = FALSE;
							int status = settings->cx_popup ? STATUS_WORKING : STATUS_SLEEP;

							if ((viewer = CreateMView(settings, progfilename)))
							{

								do
								{

									switch (status)
									{
										case STATUS_WORKING:
											if (!viewer->screen) viewer->screen = CreateMVScreen(settings);
											if (!viewer->window) viewer->window = CreateMVWindow(viewer, viewer->screen, settings);

											if (viewer->screen && viewer->window)
											{
												status = HandleMView(viewer, &settings, &pathsettings, oldsettings, viewdata, reopen);
											}
											else
											{
												printf("*** could not open window - screen: %s - window %s.\n", viewer->screen?"okay":"failed", viewer->window?"okay":"failed");
												status = STATUS_QUIT;
											}

											break;

										case STATUS_REOPENALL:
											DeleteMVWindow(viewer, viewer->window);
											viewer->window = NULL;
											DeleteMVScreen(viewer->screen);
											viewer->screen = NULL;
											status = STATUS_WORKING;
											reopen = TRUE;
											break;

										case STATUS_REOPENWINDOW:
											DeleteMVWindow(viewer, viewer->window);
											viewer->window = NULL;
											status = STATUS_WORKING;
											reopen = TRUE;
											break;

										case STATUS_SLEEP:
										{
											ULONG cxsignal = 1 << (viewer->brokermsgport->mp_SigBit);
											struct AppMessage *appmsg;
											CxMsg *cxmsg;
											ULONG signals;

											DeleteMVWindow(viewer, viewer->window);
											viewer->window = NULL;

											DeleteMVScreen(viewer->screen);
											viewer->screen = NULL;

											do
											{
												signals = Wait(cxsignal | viewer->appSignal | SIGBREAKF_CTRL_C);

												if (signals & viewer->appSignal)
												{
													char **newfilepatternlist = NULL;
													char **filepatternlist = NULL;

													while ((appmsg = (struct AppMessage *) GetMsg(viewer->appmsgport)))
													{
														DeleteFilePatternList(newfilepatternlist);
														if ((newfilepatternlist = CreateFilePatternList(appmsg->am_ArgList, appmsg->am_NumArgs)))
														{
															DeleteFilePatternList(filepatternlist);
															filepatternlist = newfilepatternlist;
															newfilepatternlist = NULL;
														}
														ReplyMsg((struct Message *) appmsg);
													}

													if (filepatternlist)
													{
														FilterFilePatternList(&filepatternlist, &settings);
													}
													if (filepatternlist)
													{
														NewPicHandler(viewdata, filepatternlist, settings, NULL);
														DeleteFilePatternList(filepatternlist);
													}
													status = STATUS_WORKING;
												}

												if (signals & cxsignal)
												{
													ULONG msgid, msgtype;
													while ((cxmsg = (CxMsg *) GetMsg(viewer->brokermsgport)))
													{
														msgid = CxMsgID(cxmsg);
														msgtype = CxMsgType(cxmsg);
														ReplyMsg((struct Message *) cxmsg);

														switch (msgtype)
														{
															case CXM_COMMAND:
																switch (msgid)
																{
																	case CXCMD_APPEAR:
																		status = STATUS_WORKING;
																		break;

																	case CXCMD_KILL:
																		status = STATUS_QUIT;
																		break;

																	default:
																		break;
																}

															case CXM_IEVENT:
																if (msgid == EVT_HOTKEY)
																{
																	status = STATUS_WORKING;
																	break;
																}
														}
													}
												}
												if (signals & SIGBREAKF_CTRL_C)
												{
													status = STATUS_QUIT;
												}
											} while (status == STATUS_SLEEP);

											break;
										}

										default:
										case STATUS_QUIT:
											DeleteMView(viewer);
											viewer = NULL;
											break;
									}

								} while (status != STATUS_QUIT);

								DeleteMView(viewer);

							}
							DeleteViewData(viewdata);
						}

						if (pathsettings->autosavesettings)
						{
							SaveDefaultSettings(settings, progfilename);
						}

						SavePathSettings(pathsettings, "ENVARC:MysticView");
						SavePathSettings(pathsettings, "ENV:MysticView");

						DeleteMainSettings(oldsettings);
					}

					DeletePathSettings(pathsettings);
				}
				DeleteMainSettings(settings);
			}
		}

appended_raus:


		DeleteFilePatternList(wbargarray);
		FreeArgs(rdargs);


		DeleteStringList(piclist);

		Free(progfilename);

		DeleteMainSettings(initsettings);

		DeletePathSettings(initpathsettings);

		ArgArrayDone();

		MVNeuro_DeleteNet();

		RemoveKey(key);

		CloseGlobal();
		result = RETURN_OK;
	}
	else
	{
		printf("*** abnormal program termination.\n");
		result = RETURN_FAIL;
	}

	return result;
}


