/*********************************************************************
----------------------------------------------------------------------

	MysticView
	settings

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif
#include <ctype.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <libraries/mysticview.h>
#include <intuition/intuition.h>
#include <exec/memory.h>

#include <clib/macros.h>

#ifdef __MORPHOS__
#include <public/proto/libamiga/amiga_protos.h>
#else
#include <clib/alib_protos.h>
#endif

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

#include "Mystic_InitExit.h"
#include "Mystic_Screen.h"
#include "Mystic_Window.h"
#include "Mystic_Tools.h"
#include "Mystic_Settings.h"

//!#include "Mystic_PicList.h"
#include "Mystic_HandlerWrapper.h"


#include <assert.h>


#if defined(__STORM__) || defined(__GNUC__)


int stcl_h(char *out, long v)
{
	ULONG mask = 0xf0000000;
	int shift = 28;
	long t;
	int len = 1;

	while (shift >= 0)
	{
		t = v & mask;
		t >>= shift;
		*out++ = t < 10 ? '0' + t : 'a' + t - 10;
		len++;
		shift -= 4;
		mask >>= 4;
	}

	*out = 0;

	return len;
}

int stch_l(const char *in, long *v)
{
	int l = 0;
	char c;
	int n;
	*v = 0;

	while (l < 9)
	{
		c = *in++;
		if (c >= '0' && c <= '9')
		{
			n = c - '0';
		}
		else if (c >= 'a' && c <= 'f')
		{
			n = c - 'a' + 10;
		}
		else if (c >= 'A' && c <= 'F')
		{
			n = c - 'A' + 10;
		}
		else
		{
			break;
		}

		l++;
		*v = (*v << 4) + n;
	}

	return l;
}


#endif



/*********************************************************************

	string = GetKeyWord(value, defaultvalue,
			string, value,
			string, value,
			string, value,
			NULL)

	assign a string to a value from a list of choices

*********************************************************************/

char * STDARGS GetKeyWord(int value, char *def, ...)
{
	char *s = def;
	va_list va;
	ULONG *args;
	char *string;
	int keyvalue;

	va_start(va, def);
#ifdef __MORPHOS__
	args = (ULONG *) va->overflow_arg_area;
#else
	args = (ULONG *) va;
#endif

	while ((string = *((char **) args)))
	{
		args++;
		keyvalue = *((int *) args);
		args++;

		if (keyvalue == value)
		{
			s = string;
			break;
		}
	}

	return s;
}


/*********************************************************************

	node = FindPrefsNode(liste, name)

	find the named preference node in a list

*********************************************************************/

struct Node *FindPrefsNode(struct List *list, char *name)
{
	struct Node *node = NULL;

	if (list && name)
	{
		if (!IsListEmpty(list))
		{
			struct Node *currentnode;
			struct Node *nextnode;
			char *text;
			int searchlen = strlen(name);

			currentnode = list->lh_Head;
			while ((nextnode = currentnode->ln_Succ))
			{
				if ((text = currentnode->ln_Name))
				{
					if (!Strnicmp(name, text, searchlen))
					{
						node = currentnode;
						break;
					}
					else if (text[0] == '(')
					{
						if (!Strnicmp(name, &text[1], searchlen))
						{
							node = currentnode;
							break;
						}
					}
				}
				currentnode = nextnode;
			}
		}
	}

	return node;
}


/*********************************************************************

	success = SetPrefsNode(liste, setting, value, enable)

	set the named prefs node to the specified value

*********************************************************************/

BOOL SetPrefsNode(struct List *list, char *setting, char *value, BOOL enabled)
{
	struct Node *listnode, *newnode;
	BOOL success = FALSE;

	if (list && setting)
	{
		if ((newnode = Malloclear(sizeof(struct Node))))
		{
			char *text;
			int len = 10;

			if (setting) len += strlen(setting);
			if (value) len += strlen(value);

			if ((text = Malloc(len)))
			{
				if (value && enabled)
				{
					sprintf(text, "%s=%s", setting, value);
				}
				else if (value && !enabled)
				{
					sprintf(text, "(%s=%s)", setting, value);
				}
				else if (!value && enabled)
				{
					sprintf(text, "%s", setting);
				}
				else if (!value && !enabled)
				{
					sprintf(text, "(%s)", setting);
				}

				newnode->ln_Name = text;
				success = TRUE;
			}
		}

		if (success)
		{
			listnode = FindPrefsNode(list, setting);

			Insert(list, newnode, listnode);

			if (listnode)
			{
				Remove(listnode);
				Free(listnode->ln_Name);
				Free(listnode);
			}
		}
		else
		{
			Free(newnode);
		}

	}

	return success;
}


/*********************************************************************

	success = SetPrefsNodeBool(list, setting, enable)

*********************************************************************/

BOOL SetPrefsNodeBool(struct List *list, char *setting, BOOL enabled)
{
	if (enabled)
	{
		return SetPrefsNode(list, setting, "YES", TRUE);
	}
	else
	{
		return SetPrefsNode(list, setting, "NO", TRUE);
	}
}


/*********************************************************************

	success = SetPrefsNodeNum(list, setting, value, enable)

*********************************************************************/

BOOL SetPrefsNodeNum(struct List *list, char *setting, int num, BOOL enabled)
{
	if (enabled)
	{
		char text[100];
	#ifdef __AROS__
		sprintf(text, "%d", num);
	#else
		sprintf(text, "%ld", num);
	#endif
		return SetPrefsNode(list, setting, text, TRUE);
	}
	else
	{
		return SetPrefsNode(list, setting, NULL, FALSE);
	}
}


/*********************************************************************

	success = SetPrefsNodeHex(list, setting, value, enable)

*********************************************************************/

BOOL SetPrefsNodeHex(struct List *list, char *setting, int num, BOOL enabled)
{
	if (enabled)
	{
		char text[100];
		stcl_h(text, num);
		return SetPrefsNode(list, setting, text, TRUE);
	}
	else
	{
		return SetPrefsNode(list, setting, NULL, FALSE);
	}
}


/*********************************************************************

	success = SetPrefsNodeFloat(list, setting, value, enable)

*********************************************************************/

BOOL SetPrefsNodeFloat(struct List *list, char *setting, double num, BOOL enabled)
{
	if (enabled)
	{
		char text[20];
	//	gcvt(num, 3, text);
		sprintf(text, "%.2f", num);
		return SetPrefsNode(list, setting, text, TRUE);
	}
	else
	{
		return SetPrefsNode(list, setting, NULL, FALSE);
	}
}



/*********************************************************************

	stringlist = CreateMainSettingsArray(mainsettings, ttypes)

*********************************************************************/

char **CreateMainSettingsArray(struct mainsettings *settings, char **ttypes)
{
	char **settingsarray = NULL;
	struct List *toollist;

	if ((toollist = CreateListFromArray(ttypes)))
	{
		int error = 0;


		//	boolean settings:

//		error |= !SetPrefsNodeBool(toollist, "AUTOSAVESETTINGS", settings->autosavesettings);
		error |= !SetPrefsNodeBool(toollist, "BACKDROP", settings->borderless);
		error |= !SetPrefsNodeBool(toollist, "HAM", settings->hamscreen);
		error |= !SetPrefsNodeBool(toollist, "PICINFO", settings->picinfo);
		error |= !SetPrefsNodeBool(toollist, "STATICPALETTE", settings->staticpalette);
		error |= !SetPrefsNodeBool(toollist, "REQUESTFILE", settings->requestfile);
		error |= !SetPrefsNodeBool(toollist, "SLIDESHOW", settings->slideshow);
		error |= !SetPrefsNodeBool(toollist, "RECURSE", settings->recursive);
		error |= !SetPrefsNodeBool(toollist, "LOOP", settings->loop);
		error |= !SetPrefsNodeBool(toollist, "SHOWCURSOR", settings->markcenter);
		error |= !SetPrefsNodeBool(toollist, "AUTOQUIT", settings->autoquit);
		error |= !SetPrefsNodeBool(toollist, "AUTOHIDE", settings->autohide);
		error |= !SetPrefsNodeBool(toollist, "AUTOSTOP", settings->autostop);
		error |= !SetPrefsNodeBool(toollist, "AUTOCLEAR", settings->autoclear);
		error |= !SetPrefsNodeBool(toollist, "SHOWARROWS", settings->showarrows);
		error |= !SetPrefsNodeBool(toollist, "SHOWPIP", settings->showpip);
		error |= !SetPrefsNodeBool(toollist, "SHOWBUTTONS", settings->showbuttons);
		error |= !SetPrefsNodeBool(toollist, "RESETDISPLAY", settings->resetdisplaysettings);
		error |= !SetPrefsNodeBool(toollist, "CX_POPUP", settings->cx_popup);
		error |= !SetPrefsNodeBool(toollist, "AUTODISPLAYMODE", settings->autodisplaymode);
		error |= !SetPrefsNodeBool(toollist, "AUTOCROP", settings->autocrop);
		error |= !SetPrefsNodeBool(toollist, "REVERSE", settings->reverse);
		error |= !SetPrefsNodeBool(toollist, "CREATETHUMBNAILS", settings->createthumbnails);
		error |= !SetPrefsNodeBool(toollist, "ASYNCSCANNING", settings->asyncscanning);
		error |= !SetPrefsNodeBool(toollist, "FILTERPICTURES", settings->filterpictures);
		error |= !SetPrefsNodeBool(toollist, "APPENDICONS", settings->appendicons);
		error |= !SetPrefsNodeBool(toollist, "HIDEONCLOSE", settings->hideonclose);
		error |= !SetPrefsNodeBool(toollist, "HIDEONESCAPE", settings->hideonescape);
		error |= !SetPrefsNodeBool(toollist, "SMOOTHDISPLAY", settings->smoothdisplay);
		error |= !SetPrefsNodeBool(toollist, "SIMPLESCANNING", settings->simplescanning);

		//	numeric settings:

		error |= !SetPrefsNodeNum(toollist, "BUFFERPERCENT", settings->bufferpercent, TRUE);
		error |= !SetPrefsNodeNum(toollist, "DEPTH", settings->depth, settings->depth > 0);
		error |= !SetPrefsNodeNum(toollist, "SCREENASPECTX", settings->screenaspectx, settings->screenaspectx);
		error |= !SetPrefsNodeNum(toollist, "SCREENASPECTY", settings->screenaspecty, settings->screenaspecty);
		error |= !SetPrefsNodeNum(toollist, "SCRWIDTH", settings->scrwidth, settings->scrwidth > 0);
		error |= !SetPrefsNodeNum(toollist, "SCRHEIGHT", settings->scrheight, settings->scrheight > 0);
		error |= !SetPrefsNodeNum(toollist, "SLIDEDELAY", settings->slidedelay, TRUE);
		error |= !SetPrefsNodeNum(toollist, "WINLEFT", settings->winleft, settings->winleft > -1);
		error |= !SetPrefsNodeNum(toollist, "WINTOP", settings->wintop, settings->wintop > -1);
		error |= !SetPrefsNodeNum(toollist, "WINWIDTH", settings->winwidth, settings->winwidth > 0);
		error |= !SetPrefsNodeNum(toollist, "WINHEIGHT", settings->winheight, settings->winheight > 0);
		error |= !SetPrefsNodeNum(toollist, "CX_PRIORITY", settings->cx_priority, TRUE);
		error |= !SetPrefsNodeNum(toollist, "ROTATESTEP", settings->rotatestep, TRUE);
		error |= !SetPrefsNodeNum(toollist, "THUMBSIZE", settings->thumbsize, TRUE);
		error |= !SetPrefsNodeNum(toollist, "THUMBCOLORS", settings->thumbnumcolors, TRUE);


		//	floating number settings:

		error |= !SetPrefsNodeFloat(toollist, "ZOOMSTEP", settings->zoomstep, TRUE);


		//	hexadecimal settings:

		error |= !SetPrefsNodeHex(toollist, "BGCOLOR", settings->bgcolor, TRUE);
		error |= !SetPrefsNodeHex(toollist, "TEXTCOLOR", settings->textcolor, TRUE);
		error |= !SetPrefsNodeHex(toollist, "MARKCOLOR", settings->markcolor, TRUE);
		error |= !SetPrefsNodeHex(toollist, "MODEID", settings->modeID, settings->modeID != INVALID_ID);


		//	string settings:

		error |= !SetPrefsNode(toollist, "PUBSCREEN", settings->pubscreen, !!settings->pubscreen);
		error |= !SetPrefsNode(toollist, "STARTPIC", settings->startpic, !!settings->startpic);
		error |= !SetPrefsNode(toollist, "BUTTONPIC", settings->buttonpic, !!settings->buttonpic);
		error |= !SetPrefsNode(toollist, "ANIMPIC", settings->animpic, !!settings->animpic);
		error |= !SetPrefsNode(toollist, "CX_POPKEY", settings->hotkey, !!settings->hotkey);
		error |= !SetPrefsNode(toollist, "REJECTPATTERN", settings->rejectpattern, !!settings->rejectpattern);
		error |= !SetPrefsNode(toollist, "FONTSPEC", settings->fontspec, !!settings->fontspec);
		error |= !SetPrefsNode(toollist, "DEFAULTTOOL", settings->defaulttool, !!settings->defaulttool);
		error |= !SetPrefsNode(toollist, "PICFORMAT", settings->picformat, !!settings->picformat);



		//	keyword settings:

#if 0
		error |= !SetPrefsNode(toollist, "AUTOTHUMBNAIL",
					GetKeyWord(settings->thumbnailmode,
							"OFF",
							"UPDATE", THUMBNAIL_UPDATE,
							"NOUPDATE", THUMBNAIL_NOUPDATE, NULL),
					TRUE);
#endif

		error |= !SetPrefsNode(toollist, "CONFIRM",
					GetKeyWord(settings->confirmlevel,
							"ALWAYS",
							"NEVER", CONFIRM_NEVER, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "DISPLAYMODE",
					GetKeyWord(settings->displaymode,
							"KEEPASPECT",
							"KEEPASPECT2", MVDISPMODE_KEEPASPECT_MAX,
							"ONEPIXEL", MVDISPMODE_ONEPIXEL,
							"IGNOREASPECT", MVDISPMODE_IGNOREASPECT,
							"FIT", MVDISPMODE_FIT, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "DITHERING",
					GetKeyWord(settings->dither,
							"OFF",
							"AUTO", MVDITHERMODE_AUTO,
							"ON", MVDITHERMODE_ON, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "DITHERMODE",
					GetKeyWord(settings->dithermode,
							"EDD",
							"FS", DITHERMODE_FS, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "ICONIFY",
					GetKeyWord(settings->iconifymode,
							"APPICON",
							"APPITEM", ICONIFY_APPITEM,
							"INVISIBLE", ICONIFY_INVISIBLE, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "PREVIEWMODE",
					GetKeyWord(settings->previewmode,
							"NONE",
							"GRID", MVPREVMODE_GRID,
							"OPAQUE", MVPREVMODE_OPAQUE, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "REFRESHMODE",
					GetKeyWord(settings->refreshmode,
							"SMART",
							"SIMPLE", WFLG_SIMPLE_REFRESH, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "RENDERQUALITY",
					GetKeyWord(settings->hstype,
							"LOW",
							"HIGH", HSTYPE_15BIT_TURBO, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "SCROPENMODE",
					GetKeyWord(settings->scropenmode,
							"DEFAULT",
							"CUSTOM", SCROPENMODE_CUSTOM,
							"PUBLIC", SCROPENMODE_PUBLIC, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "SORTMODE",
					GetKeyWord(settings->sortmode,
							"NONE",
							"FILENAME", SORTMODE_ALPHA_FILE,
							"FULLNAME", SORTMODE_ALPHA_FULL,
							"SIZE", SORTMODE_FILESIZE,
							"DATE", SORTMODE_DATE,
							"RANDOM", SORTMODE_RANDOM, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "PRECISION",
					GetKeyWord(settings->precision,
							"IMAGE",
							"EXACT", PRECISION_EXACT,
							"ICON", PRECISION_ICON,
							"GUI", PRECISION_GUI, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "WINOPENMODE",
					GetKeyWord(settings->winopenmode,
							"DEFAULT",
							"CENTER", WOPENMODE_CENTER,
							"MOUSE", WOPENMODE_MOUSE, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "WINSIZEMODE",
					GetKeyWord(settings->winsizemode,
							"DEFAULT",
							"HALF", WSIZEMODE_HALF,
							"VISIBLE", WSIZEMODE_VISIBLE,
							"SMALL", WSIZEMODE_33PERCENT,
							"LARGE", WSIZEMODE_66PERCENT,
							"FULL", WSIZEMODE_FULL, NULL),
					TRUE);

		error |= !SetPrefsNode(toollist, "LMBACTION",
					GetKeyWord(settings->leftmouseaction,
							"DRAG",
							"NEXT", MOUSEACTION_NEXT, NULL),
					TRUE);


		settingsarray = CreateArrayFromList(toollist);

		DeleteList(toollist);
	}

	return settingsarray;
}




/*********************************************************************

	pathsettings = CreatePathSettings(ttypes, oldsettings, tags)

	tooltypes override tags override (oldsettings or defaults)

*********************************************************************/

struct pathsettings * STDARGS CreatePathSettings(char **ttypes, struct pathsettings *oldsettings, ...)
{
	struct pathsettings *mvs;
//	va_list va;
//	struct TagItem *tags;
	int i;

/*
	va_start(va, oldsettings);
#ifdef __MORPHOS__
	taglist = (struct TagItem *) va->overflow_arg_area;
#else
	taglist = (struct TagItem *) va;
#endif
*/

	if ((mvs = Malloclear(sizeof(struct pathsettings))))
	{
		char *s;
		char temps[50];

		if (oldsettings)
		{
			LOCK(oldsettings);
			CopyMem(oldsettings, mvs, sizeof(struct pathsettings));

			if (mvs->startpath)		mvs->startpath = _StrDup(mvs->startpath);
			if (mvs->listpath)		mvs->listpath = _StrDup(mvs->listpath);
			if (mvs->presetpath)	mvs->presetpath = _StrDup(mvs->presetpath);
			if (mvs->savepath)		mvs->savepath = _StrDup(mvs->savepath);
			if (mvs->copypath)		mvs->copypath = _StrDup(mvs->copypath);



			for (i = 0; i < 10; ++i)
			{
				if (mvs->destpath[i])
				{
					mvs->destpath[i] = _StrDup(mvs->destpath[i]);
				}
			}

			for (i = 0; i < 10; ++i)
			{
				if (mvs->preset[i])
				{
					mvs->preset[i] = _StrDup(mvs->preset[i]);
				}
			}

			UNLOCK(oldsettings);
		}
		else
		{
			mvs->presetpath =		_StrDup(DEFAULT_PRESETPATH);
			mvs->listpath =			_StrDup(DEFAULT_LISTPATH);
			mvs->copypath =			_StrDup(DEFAULT_COPYPATH);
			mvs->autosavesettings = DEFAULT_AUTOSAVESETTINGS;
		}

		InitSemaphore(&mvs->semaphore);



		//	Tooltypes

		if ((s = ArgString((UBYTE **) ttypes, "STARTPATH", NULL)))
		{
			Free(mvs->startpath);
			mvs->startpath = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "LISTPATH", NULL)))
		{
			Free(mvs->listpath);
			mvs->listpath = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "SAVEPATH", NULL)))
		{
			Free(mvs->savepath);
			mvs->savepath = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "PRESETPATH", NULL)))
		{
			Free(mvs->presetpath);
			mvs->presetpath = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "COPYPATH", NULL)))
		{
			Free(mvs->copypath);
			mvs->copypath = _StrDup(s);
		}

		for (i = 0; i < 10; ++i)
		{
		#ifdef __AROS__
			sprintf(temps, "DESTPATH%d", i);
		#else
			sprintf(temps, "DESTPATH%ld", i);
		#endif
			if ((s = ArgString((UBYTE **) ttypes, temps, NULL)))
			{
				Free(mvs->destpath[i]);
				mvs->destpath[i] = _StrDup(s);
			}
		}

		for (i = 0; i < 10; ++i)
		{
		#ifdef __AROS__
			sprintf(temps, "PRESET%d", i);
		#else
			sprintf(temps, "PRESET%ld", i);
		#endif
			if ((s = ArgString((UBYTE **) ttypes, temps, NULL)))
			{
				Free(mvs->preset[i]);
				mvs->preset[i] = _StrDup(s);
			}
		}

		mvs->autosavesettings = GetBooleanSetting(ttypes, "AUTOSAVESETTINGS", mvs->autosavesettings);

	}

	return mvs;
}





/*********************************************************************

	stringlist = CreatePathSettingsArray(pathsettings, ttypes)

*********************************************************************/

char **CreatePathSettingsArray(struct pathsettings *settings, char **ttypes)
{
	char **settingsarray = NULL;
	struct List *toollist;

	if ((toollist = CreateListFromArray(ttypes)))
	{
		int error = 0;
		int i;
		char temps[50];

		for (i = 0; i < 10; ++i)
		{
			sprintf(temps, "DESTPATH%d", i);
			error |= !SetPrefsNode(toollist, temps, settings->destpath[i], !!settings->destpath[i]);
		}

		for (i = 0; i < 10; ++i)
		{
			sprintf(temps, "PRESET%d", i);
			error |= !SetPrefsNode(toollist, temps, settings->preset[i], !!settings->preset[i]);
		}

		error |= !SetPrefsNode(toollist, "COPYPATH", settings->copypath, !!settings->copypath);
		error |= !SetPrefsNode(toollist, "LISTPATH", settings->listpath, !!settings->listpath);
		error |= !SetPrefsNode(toollist, "PRESETPATH", settings->presetpath, !!settings->presetpath);
		error |= !SetPrefsNode(toollist, "STARTPATH", settings->startpath, !!settings->startpath);
		error |= !SetPrefsNode(toollist, "SAVEPATH", settings->savepath, !!settings->savepath);
		error |= !SetPrefsNodeBool(toollist, "AUTOSAVESETTINGS", settings->autosavesettings);

		settingsarray = CreateArrayFromList(toollist);

		DeleteList(toollist);
	}

	return settingsarray;
}





/*********************************************************************

	success = SaveDefaultSettings(mainsettings, filename)

	save default settings to the disk icon

*********************************************************************/

BOOL SaveDefaultSettings(struct mainsettings *settings, char *filename)
{
	struct DiskObject *dob;
	BOOL success = FALSE;

	if (!(dob = GetDiskObject(filename)))
	{
		dob = GetDefDiskObject(WBTOOL);
	}

	if (dob)
	{
		char **newttarray;

		if ((newttarray = CreateMainSettingsArray(settings, (char **)dob->do_ToolTypes)))
		{
			dob->do_ToolTypes = (STRPTR *)newttarray;
			success = PutDiskObject(filename, dob);
			DeleteStringList(newttarray);
		}

		FreeDiskObject(dob);
	}

	return success;
}



/*********************************************************************

	mainsettings = LoadDefaultSettings(filename)

	load default settings from the disk icon

*********************************************************************/

struct mainsettings *LoadDefaultSettings(char *filename)
{
	struct mainsettings *settings = NULL;
	struct DiskObject *dob;

	if ((dob = GetDiskObject(filename)))
	{
		settings = CreateMainSettings((char **)dob->do_ToolTypes, NULL, NULL);
		FreeDiskObject(dob);
	}

	return settings;
}




/*********************************************************************

	success = SavePreset(mview, mainsettings, filename)

	save the main settings to a preset file

*********************************************************************/

BOOL SavePreset(struct mview *mv, struct mainsettings *settings, char *filename)
{
	BOOL success = FALSE;
	char **settingsarray;

	BOOL icon = TRUE;

	if (filename)
	{
		if ((settingsarray = CreateMainSettingsArray(settings, NULL)))
		{
			FILE *fp;
			char **t = settingsarray;

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

			DeleteStringList(settingsarray);

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

							tt[0] = _StrDup("MYSTICVIEW_PRESET=YES");

							backdeftool = mv->diskobject->do_DefaultTool;
							backtt = (char **)mv->diskobject->do_ToolTypes;
							backtype = mv->diskobject->do_Type;

							mv->diskobject->do_Type = WBPROJECT;
							mv->diskobject->do_ToolTypes = (STRPTR *)tt;
						//	mv->diskobject->do_DefaultTool = "mysticview";
							mv->diskobject->do_DefaultTool = settings->defaulttool;

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
	}

	return success;
}



/*********************************************************************

	mainsettings = LoadPreset(filename, oldsettings)

	load the main settings from a preset file

*********************************************************************/

struct mainsettings *LoadPreset(char *filename, struct mainsettings *oldsettings)
{
	struct mainsettings *settings = NULL;
	char **settingsarray;

	if ((settingsarray = LoadStringList(filename)))
	{
		settings = CreateMainSettings(settingsarray, oldsettings, NULL);

		DeleteStringList(settingsarray);
	}

	return settings;
}







/*********************************************************************

	success = GetBooleanSetting(ttypes, name, default)

*********************************************************************/

BOOL GetBooleanSetting(char **ttypes, char *name, BOOL def)
{
	char *s;
	BOOL r = def;

	if ((s = ArgString((UBYTE **) ttypes, name, NULL)))
	{
		if (!Stricmp(s, "YES"))
		{
			r = TRUE;
		}
		else
		{
			r = FALSE;
		}
	}

	return r;
}





/*********************************************************************

	DeleteMainSettings(mainsettings)

*********************************************************************/

void DeleteMainSettings(struct mainsettings *settings)
{
	if (settings)
	{
		Free(settings->hotkey);
		Free(settings->buttonpic);
		Free(settings->animpic);
		Free(settings->startpic);
		Free(settings->pubscreen);
		Free(settings->rejectpattern);
		Free(settings->fontspec);
		Free(settings->defaulttool);
		Free(settings->picformat);

		Free(settings);
	}
}


/*********************************************************************

	DeletePathSettings(pathsettings)

*********************************************************************/

void DeletePathSettings(struct pathsettings *settings)
{
	if (settings)
	{
		int i;

		Free(settings->savepath);
		Free(settings->startpath);
		Free(settings->listpath);
		Free(settings->presetpath);
		Free(settings->copypath);

		for (i = 0; i < 10; ++i)
		{
			Free(settings->destpath[i]);
		}

		for (i = 0; i < 10; ++i)
		{
			Free(settings->preset[i]);
		}

		Free(settings);
	}
}




/*********************************************************************

	mainsettings = CreateMainSettings(ttypes, oldsettings, tags)

	tooltypes override tags override (oldsettings or defaults)

*********************************************************************/

struct mainsettings * STDARGS CreateMainSettings(char **ttypes, struct mainsettings *oldsettings, ...)
{
	struct mainsettings *mvs;
//	va_list va;
//	struct TagItem *tags;

/*
	va_start(va, oldsettings);
#ifdef __MORPHOS__
	tags = (struct TagItem *) va->overflow_arg_area;
#else
	tags = (struct TagItem *) va;
#endif
*/

	if ((mvs = Malloclear(sizeof(struct mainsettings))))
	{
		char *s;
		LONG temp;

		if (oldsettings)
		{
			LOCK(oldsettings);
			CopyMem(oldsettings, mvs, sizeof(struct mainsettings));

			if (mvs->startpic)		mvs->startpic = _StrDup(mvs->startpic);
			if (mvs->buttonpic)		mvs->buttonpic = _StrDup(mvs->buttonpic);
			if (mvs->animpic)		mvs->animpic = _StrDup(mvs->animpic);
			if (mvs->hotkey)		mvs->hotkey = _StrDup(mvs->hotkey);
			if (mvs->pubscreen)		mvs->pubscreen = _StrDup(mvs->pubscreen);
			if (mvs->rejectpattern)	mvs->rejectpattern = _StrDup(mvs->rejectpattern);
			if (mvs->fontspec)		mvs->fontspec = _StrDup(mvs->fontspec);
			if (mvs->defaulttool)	mvs->defaulttool = _StrDup(mvs->defaulttool);
			if (mvs->picformat)		mvs->picformat = _StrDup(mvs->picformat);

			UNLOCK(oldsettings);
		}
		else
		{
			mvs->winleft =			DEFAULT_WINLEFT;
			mvs->wintop =			DEFAULT_WINTOP;
			mvs->winwidth =			DEFAULT_WINWIDTH;
			mvs->winheight =		DEFAULT_WINHEIGHT;
			mvs->winopenmode =		DEFAULT_WINOPENMODE;
			mvs->winsizemode =		DEFAULT_WINSIZEMODE;
			mvs->refreshmode =		DEFAULT_REFRESHMODE;
			mvs->displaymode =		DEFAULT_DISPLAYMODE;
			mvs->borderless =		DEFAULT_BORDERLESS;
			mvs->previewmode =		DEFAULT_PREVIEWMODE;

			mvs->bufferpercent =	DEFAULT_BUFFERPERCENT;
			mvs->depth =			DEFAULT_DEPTH;
			mvs->precision =		DEFAULT_PRECISION;
			mvs->hamscreen =		DEFAULT_HAMSCREEN;
			mvs->screenaspectx =	DEFAULT_SCREENASPECTX;
			mvs->screenaspecty =	DEFAULT_SCREENASPECTY;
			mvs->requestfile =		DEFAULT_REQUESTFILE;
			mvs->bgcolor =			DEFAULT_BGCOLOR;
			mvs->textcolor =		DEFAULT_TEXTCOLOR;
			mvs->markcolor =		DEFAULT_MARKCOLOR;
			mvs->hstype	 = 			DEFAULT_HSTYPE;
			mvs->recursive =		DEFAULT_RECURSIVE;
			mvs->dither =			DEFAULT_DITHER;
			mvs->staticpalette =	DEFAULT_STATICPALETTE;
			mvs->slidedelay =		DEFAULT_SLIDEDELAY;
			mvs->smoothdisplay =	DEFAULT_SMOOTHDISPLAY;
			mvs->simplescanning =	DEFAULT_SIMPLESCANNING;
			mvs->slideshow =		DEFAULT_SLIDESHOW;
			mvs->scrwidth =			DEFAULT_SCRWIDTH;
			mvs->scrheight =		DEFAULT_SCRHEIGHT;
			mvs->modeID =			DEFAULT_MODEID;
			mvs->picinfo =			DEFAULT_PICINFO;
			mvs->dithermode =		DEFAULT_DITHERMODE;
			mvs->startpic =			_StrDup(DEFAULT_STARTPIC);
			mvs->buttonpic =		_StrDup(DEFAULT_BUTTONPIC);
			mvs->animpic =			_StrDup(DEFAULT_ANIMPIC);
			mvs->loop =				DEFAULT_LOOP;
			mvs->autoquit =			DEFAULT_AUTOQUIT;
			mvs->sortmode =			DEFAULT_SORTMODE;
			mvs->reverse =			DEFAULT_REVERSE;
			mvs->zoomstep = 		DEFAULT_ZOOMSTEP;
			mvs->markcenter = 		DEFAULT_MARKCENTER;
			mvs->scropenmode = 		DEFAULT_SCROPENMODE;
			mvs->cx_priority = 		DEFAULT_CX_PRIORITY;
			mvs->hotkey =			_StrDup(DEFAULT_HOTKEY);
			mvs->defaulttool =		_StrDup(DEFAULT_DEFAULTTOOL);
			mvs->picformat =		_StrDup(DEFAULT_PICFORMAT);
			mvs->cx_popup =			DEFAULT_CX_POPUP;
			mvs->autohide =			DEFAULT_AUTOHIDE;
			mvs->rotatestep = 		DEFAULT_ROTATESTEP;
			mvs->confirmlevel = 	DEFAULT_CONFIRMLEVEL;
			mvs->iconifymode = 		DEFAULT_ICONIFYMODE;
			mvs->thumbsize =		DEFAULT_THUMBSIZE;
			mvs->thumbnumcolors =	DEFAULT_THUMBNUMCOLORS;
			mvs->rejectpattern =	_StrDup(DEFAULT_REJECTPATTERN);
			mvs->resetdisplaysettings = 	DEFAULT_RESETDISPLAYSETTINGS;
			mvs->createthumbnails = 	DEFAULT_CREATETHUMBNAILS;
			mvs->asyncscanning = 	DEFAULT_ASYNCSCANNING;
			mvs->filterpictures = 	DEFAULT_FILTERPICTURES;
			mvs->appendicons = 	DEFAULT_APPENDICONS;
			mvs->hideonclose = 		DEFAULT_HIDEONCLOSE;
			mvs->hideonescape = 	DEFAULT_HIDEONESCAPE;
			mvs->leftmouseaction = 	DEFAULT_LMBACTION;
			mvs->autodisplaymode =	DEFAULT_AUTODISPLAYMODE;
			mvs->autocrop =			DEFAULT_AUTOCROP;
			mvs->autostop =			DEFAULT_AUTOSTOP;
			mvs->autoclear =		DEFAULT_AUTOCLEAR;
			mvs->showarrows =		DEFAULT_SHOWARROWS;
			mvs->showpip =			DEFAULT_SHOWPIP;
			mvs->showbuttons =		DEFAULT_SHOWBUTTONS;
			
			mvs->browsemode = 		DEFAULT_BROWSEMODE;
		}

		InitSemaphore(&mvs->semaphore);



		//	Tooltypes

		if ((s = ArgString((UBYTE **) ttypes, "PUBSCREEN", NULL)))
		{
			Free(mvs->pubscreen);
			mvs->pubscreen = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "FONTSPEC", NULL)))
		{
			Free(mvs->fontspec);
			mvs->fontspec = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "DEFAULTTOOL", NULL)))
		{
			Free(mvs->defaulttool);
			mvs->defaulttool = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "PICFORMAT", NULL)))
		{
			Free(mvs->picformat);
			mvs->picformat = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "CX_POPKEY", NULL)))
		{
			Free(mvs->hotkey);
			mvs->hotkey = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "STARTPIC", NULL)))
		{
			Free(mvs->startpic);
			mvs->startpic = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "BUTTONPIC", NULL)))
		{
			Free(mvs->buttonpic);
			mvs->buttonpic = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "ANIMPIC", NULL)))
		{
			Free(mvs->animpic);
			mvs->animpic = _StrDup(s);
		}

		if ((s = ArgString((UBYTE **) ttypes, "REJECTPATTERN", NULL)))
		{
			Free(mvs->rejectpattern);
			mvs->rejectpattern = _StrDup(s);
		}


		mvs->cx_priority = 	ArgInt((UBYTE **) ttypes, "CX_PRIORITY", mvs->cx_priority);

		mvs->winleft =		ArgInt((UBYTE **) ttypes, "WINLEFT", mvs->winleft);
		mvs->wintop =		ArgInt((UBYTE **) ttypes, "WINTOP", mvs->wintop);
		mvs->winwidth =		ArgInt((UBYTE **) ttypes, "WINWIDTH", mvs->winwidth);
		mvs->winheight =	ArgInt((UBYTE **) ttypes, "WINHEIGHT", mvs->winheight);

		mvs->depth =		ArgInt((UBYTE **) ttypes, "DEPTH", mvs->depth);

		mvs->bufferpercent = ArgInt((UBYTE **) ttypes, "BUFFERPERCENT", mvs->bufferpercent);
		mvs->bufferpercent = RNG(1, mvs->bufferpercent, 90);

		mvs->screenaspectx = ArgInt((UBYTE **) ttypes, "SCREENASPECTX", mvs->screenaspectx);
		mvs->screenaspecty = ArgInt((UBYTE **) ttypes, "SCREENASPECTY", mvs->screenaspecty);

		mvs->slidedelay =	ArgInt((UBYTE **) ttypes, "SLIDEDELAY", mvs->slidedelay);
		mvs->slidedelay =	RNG(0, mvs->slidedelay, 100000);

		mvs->scrwidth 	= 	ArgInt((UBYTE **) ttypes, "SCRWIDTH", mvs->scrwidth);
		mvs->scrheight = 	ArgInt((UBYTE **) ttypes, "SCRHEIGHT", mvs->scrheight);

		mvs->rotatestep =	ArgInt((UBYTE **) ttypes, "ROTATESTEP", mvs->rotatestep);
		mvs->rotatestep =	RNG(1, mvs->rotatestep, 180);

		mvs->thumbsize =	ArgInt((UBYTE **) ttypes, "THUMBSIZE", mvs->thumbsize);
		mvs->thumbsize = 	RNG(10, mvs->thumbsize, 93);
		mvs->thumbnumcolors =	ArgInt((UBYTE **) ttypes, "THUMBCOLORS", mvs->thumbnumcolors);
		mvs->thumbnumcolors	=	RNG(4, mvs->thumbnumcolors, 256);


		s = ArgString((UBYTE **) ttypes, "ICONIFY", NULL);
		if (s)
		{
			mvs->iconifymode = Stricmp(s, "APPICON") ? mvs->iconifymode : ICONIFY_APPICON;
			mvs->iconifymode = Stricmp(s, "APPITEM") ? mvs->iconifymode : ICONIFY_APPITEM;
			mvs->iconifymode = Stricmp(s, "INVISIBLE") ? mvs->iconifymode : ICONIFY_INVISIBLE;
		}

		s = ArgString((UBYTE **) ttypes, "CONFIRM", NULL);
		if (s)
		{
			mvs->confirmlevel = Stricmp(s, "ALWAYS") ? mvs->confirmlevel : CONFIRM_ALWAYS;
			mvs->confirmlevel = Stricmp(s, "NEVER") ? mvs->confirmlevel : CONFIRM_NEVER;
		}

		s = ArgString((UBYTE **) ttypes, "DITHERMODE", NULL);
		if (s)
		{
			mvs->dithermode = Stricmp(s, "FS") ? mvs->dithermode : DITHERMODE_FS;
			mvs->dithermode = Stricmp(s, "EDD") ? mvs->dithermode : DITHERMODE_EDD;
		}

		s = ArgString((UBYTE **) ttypes, "PREVIEWMODE", NULL);
		if (s)
		{
			mvs->previewmode = Stricmp(s, "NONE") ? mvs->previewmode : MVPREVMODE_NONE;
			mvs->previewmode = Stricmp(s, "OPAQUE") ? mvs->previewmode : MVPREVMODE_OPAQUE;
			mvs->previewmode = Stricmp(s, "GRID") ? mvs->previewmode : MVPREVMODE_GRID;
		}

		s = ArgString((UBYTE **) ttypes, "WINOPENMODE", NULL);
		if (s)
		{
			mvs->winopenmode = Stricmp(s, "DEFAULT") ? mvs->winopenmode : WOPENMODE_NONE;
			mvs->winopenmode = Stricmp(s, "CENTER") ? mvs->winopenmode : WOPENMODE_CENTER;
			mvs->winopenmode = Stricmp(s, "MOUSE") ? mvs->winopenmode : WOPENMODE_MOUSE;
		}

		if ((s = ArgString((UBYTE **) ttypes, "WINSIZEMODE", NULL)))
		{
			mvs->winsizemode = Stricmp(s, "DEFAULT") ? mvs->winsizemode : WSIZEMODE_NONE;
			mvs->winsizemode = Stricmp(s, "VISIBLE") ? mvs->winsizemode : WSIZEMODE_VISIBLE;
			mvs->winsizemode = Stricmp(s, "HALF") ? mvs->winsizemode : WSIZEMODE_HALF;
			mvs->winsizemode = Stricmp(s, "LARGE") ? mvs->winsizemode : WSIZEMODE_66PERCENT;
			mvs->winsizemode = Stricmp(s, "SMALL") ? mvs->winsizemode : WSIZEMODE_33PERCENT;
			mvs->winsizemode = Stricmp(s, "FULL") ? mvs->winsizemode : WSIZEMODE_FULL;
		}

		if ((s = ArgString((UBYTE **) ttypes, "REFRESHMODE", NULL)))
		{
			mvs->refreshmode = Stricmp(s, "SIMPLE") ? mvs->refreshmode : WFLG_SIMPLE_REFRESH;
			mvs->refreshmode = Stricmp(s, "SMART") ? mvs->refreshmode : WFLG_SMART_REFRESH;
		}

		if ((s = ArgString((UBYTE **) ttypes, "DISPLAYMODE", NULL)))
		{
			mvs->displaymode = Stricmp(s, "FIT") ? mvs->displaymode : MVDISPMODE_FIT;
			mvs->displaymode = Stricmp(s, "ONEPIXEL") ? mvs->displaymode : MVDISPMODE_ONEPIXEL;
			mvs->displaymode = Stricmp(s, "KEEPASPECT") ? mvs->displaymode : MVDISPMODE_KEEPASPECT_MIN;
			mvs->displaymode = Stricmp(s, "KEEPASPECT2") ? mvs->displaymode : MVDISPMODE_KEEPASPECT_MAX;
			mvs->displaymode = Stricmp(s, "IGNOREASPECT") ? mvs->displaymode : MVDISPMODE_IGNOREASPECT;
		}

		if ((s = ArgString((UBYTE **) ttypes, "PRECISION", NULL)))
		{
			mvs->precision = Stricmp(s, "EXACT") ? mvs->precision : PRECISION_EXACT;
			mvs->precision = Stricmp(s, "IMAGE") ? mvs->precision : PRECISION_IMAGE;
			mvs->precision = Stricmp(s, "ICON") ? mvs->precision : PRECISION_ICON;
		}

		if ((s = ArgString((UBYTE **) ttypes, "RENDERQUALITY", NULL)))
		{
			mvs->hstype = Stricmp(s, "HIGH") ? mvs->hstype : HSTYPE_15BIT_TURBO;
			mvs->hstype = Stricmp(s, "LOW") ? mvs->hstype : HSTYPE_12BIT_TURBO;
		}

		if ((s = ArgString((UBYTE **) ttypes, "SORTMODE", NULL)))
		{
			mvs->sortmode = Stricmp(s, "NONE") ? mvs->sortmode : SORTMODE_NONE;
			mvs->sortmode = Stricmp(s, "FILENAME") ? mvs->sortmode : SORTMODE_ALPHA_FILE;
			mvs->sortmode = Stricmp(s, "FULLNAME") ? mvs->sortmode : SORTMODE_ALPHA_FULL;
			mvs->sortmode = Stricmp(s, "SIZE") ? mvs->sortmode : SORTMODE_FILESIZE;
			mvs->sortmode = Stricmp(s, "DATE") ? mvs->sortmode : SORTMODE_DATE;
			mvs->sortmode = Stricmp(s, "RANDOM") ? mvs->sortmode : SORTMODE_RANDOM;
		}

		s = ArgString((UBYTE **) ttypes, "SCROPENMODE", NULL);
		if (s)
		{
			mvs->scropenmode = Stricmp(s, "DEFAULT") ? mvs->scropenmode : SCROPENMODE_NONE;
			mvs->scropenmode = Stricmp(s, "CUSTOM") ? mvs->scropenmode : SCROPENMODE_CUSTOM;
			mvs->scropenmode = Stricmp(s, "PUBLIC") ? mvs->scropenmode : SCROPENMODE_PUBLIC;
		}

		s = ArgString((UBYTE **) ttypes, "DITHERING", NULL);
		if (s)
		{
			mvs->dither = Stricmp(s, "OFF") ? mvs->dither : MVDITHERMODE_OFF;
			mvs->dither = Stricmp(s, "ON") ? mvs->dither : MVDITHERMODE_ON;
			mvs->dither = Stricmp(s, "AUTO") ? mvs->dither : MVDITHERMODE_AUTO;
		}

		if ((s = ArgString((UBYTE **) ttypes, "LMBACTION", NULL)))
		{
			mvs->leftmouseaction = Stricmp(s, "DRAG") ? mvs->leftmouseaction : MOUSEACTION_DRAG;
			mvs->leftmouseaction = Stricmp(s, "NEXT") ? mvs->leftmouseaction : MOUSEACTION_NEXT;
		}



		s = ArgString((UBYTE **) ttypes, "BGCOLOR", NULL);
		if (s)
		{
			if (stch_l(s, &temp) > 0)
			{
				mvs->bgcolor = temp;
			}
		}
		s = ArgString((UBYTE **) ttypes, "TEXTCOLOR", NULL);
		if (s)
		{
			if (stch_l(s, &temp) > 0)
			{
				mvs->textcolor = temp;
			}
		}
		s = ArgString((UBYTE **) ttypes, "MARKCOLOR", NULL);
		if (s)
		{
			if (stch_l(s, &temp) > 0)
			{
				mvs->markcolor = temp;
			}
		}

		s = ArgString((UBYTE **) ttypes, "MODEID", NULL);
		if (s)
		{
			if (stch_l(s, &temp) > 0)
			{
				mvs->modeID = temp;
			}
		}

		if ((s = ArgString((UBYTE **) ttypes, "ZOOMSTEP", NULL)))
		{
			mvs->zoomstep = strtod(s, NULL);
			mvs->zoomstep = RNG(0.01, mvs->zoomstep, 2);
		}


		mvs->borderless = GetBooleanSetting(ttypes, "BACKDROP", mvs->borderless);
		mvs->hamscreen = GetBooleanSetting(ttypes, "HAM", mvs->hamscreen);
		mvs->requestfile = GetBooleanSetting(ttypes, "REQUESTFILE", mvs->requestfile);
		mvs->recursive = GetBooleanSetting(ttypes, "RECURSE", mvs->recursive);
		mvs->staticpalette = GetBooleanSetting(ttypes, "STATICPALETTE", mvs->staticpalette);
		mvs->slideshow = GetBooleanSetting(ttypes, "SLIDESHOW", mvs->slideshow);
	//	mvs->autosavesettings = GetBooleanSetting(ttypes, "AUTOSAVESETTINGS", mvs->autosavesettings);
		mvs->picinfo = GetBooleanSetting(ttypes, "PICINFO", mvs->picinfo);
		mvs->loop = GetBooleanSetting(ttypes, "LOOP", mvs->loop);
		mvs->markcenter = GetBooleanSetting(ttypes, "SHOWCURSOR", mvs->markcenter);
		mvs->autoquit = GetBooleanSetting(ttypes, "AUTOQUIT", mvs->autoquit);
		mvs->autohide = GetBooleanSetting(ttypes, "AUTOHIDE", mvs->autohide);
		mvs->autostop = GetBooleanSetting(ttypes, "AUTOSTOP", mvs->autostop);
		mvs->autoclear = GetBooleanSetting(ttypes, "AUTOCLEAR", mvs->autoclear);
		mvs->showarrows = GetBooleanSetting(ttypes, "SHOWARROWS", mvs->showarrows);
		mvs->showpip = GetBooleanSetting(ttypes, "SHOWPIP", mvs->showpip);
		mvs->showbuttons = GetBooleanSetting(ttypes, "SHOWBUTTONS", mvs->showbuttons);
		mvs->resetdisplaysettings = GetBooleanSetting(ttypes, "RESETDISPLAY", mvs->resetdisplaysettings);
		mvs->cx_popup = GetBooleanSetting(ttypes, "CX_POPUP", mvs->cx_popup);
		mvs->autodisplaymode = GetBooleanSetting(ttypes, "AUTODISPLAYMODE", mvs->autodisplaymode);
		mvs->autocrop = GetBooleanSetting(ttypes, "AUTOCROP", mvs->autocrop);
		mvs->reverse = GetBooleanSetting(ttypes, "REVERSE", mvs->reverse);
		mvs->createthumbnails = GetBooleanSetting(ttypes, "CREATETHUMBNAILS", mvs->createthumbnails);
		mvs->asyncscanning = GetBooleanSetting(ttypes, "ASYNCSCANNING", mvs->asyncscanning);
		mvs->filterpictures = GetBooleanSetting(ttypes, "FILTERPICTURES", mvs->filterpictures);
		mvs->appendicons = GetBooleanSetting(ttypes, "APPENDICONS", mvs->appendicons);
		mvs->hideonclose = GetBooleanSetting(ttypes, "HIDEONCLOSE", mvs->hideonclose);
		mvs->hideonescape = GetBooleanSetting(ttypes, "HIDEONESCAPE", mvs->hideonescape);
		mvs->smoothdisplay = GetBooleanSetting(ttypes, "SMOOTHDISPLAY", mvs->smoothdisplay);
		mvs->simplescanning = GetBooleanSetting(ttypes, "SIMPLESCANNING", mvs->simplescanning);
	}

	return mvs;
}









/*********************************************************************

	stringlist = GetToolTypes(filename)

*********************************************************************/

char **GetToolTypes(char *filename)
{
	char **tt = NULL;
	struct DiskObject *dob;

	if ((dob = GetDiskObject(filename)))
	{
		tt = DupStringList((char **)dob->do_ToolTypes);
		FreeDiskObject(dob);
	}

	return tt;
}


/*********************************************************************

	success = PutToolTypes(filename, stringlist)

*********************************************************************/

BOOL PutToolTypes(char *filename, char **tt)
{
	struct DiskObject *dob;
	BOOL success = FALSE;

	if ((dob = GetDiskObject(filename)))
	{
		char **backup;

		backup = (char **)dob->do_ToolTypes;

		dob->do_ToolTypes = (STRPTR *)tt;

		success = PutDiskObject(filename, dob);

		dob->do_ToolTypes = (STRPTR *)backup;

		FreeDiskObject(dob);
	}

	return success;
}



/*********************************************************************

	newtooltypes = SetToolType(tooltypes, entry, value)

*********************************************************************/

char **SetToolType(char **ttypes, char *entry, char *value)
{
	char **newttypes = NULL;
	struct List *tlist;

	if ((tlist = CreateListFromArray(ttypes)))
	{
		if (SetPrefsNode(tlist, entry, value, TRUE))
		{
			newttypes = CreateArrayFromList(tlist);
		}

		DeleteList(tlist);
	}

	return newttypes;
}




/*********************************************************************

	success = SaveArray(stringlist, pathname, filename)

*********************************************************************/

BOOL SaveArray(char **array, char *pathname, char *filename)
{
	BOOL success = FALSE;

	if (pathname && filename && array)
	{
		FILE *fp;
		char *fullname;

		success = TRUE;

		if ((fullname = FullName(pathname, filename)))
		{
			if (!Exists(fullname))
			{
				if (!Exists(pathname))
				{
					char command[200];
					sprintf(command, "makedir %c%s%c\n", 34, pathname, 34);
					success = !System(command, NULL);
				}
			}

			if (success)
			{
				success = FALSE;

				if ((fp = fopen(fullname, "wb")))
				{
					char **t = array;
					success = TRUE;

					while (*t && success)
					{
						success = (fprintf(fp, "%s%c", *t, 10) >= 0);
						t++;
					}

					fclose(fp);
				}
			}

			Free(fullname);
		}
	}

	return success;
}





/*********************************************************************

	success = SavePathSettings(pathsettings, pathname)

*********************************************************************/

BOOL SavePathSettings(struct pathsettings *pathsettings, char *pathname)
{
	BOOL success = FALSE;

	if (pathsettings)
	{
		char **array;

		if ((array = CreatePathSettingsArray(pathsettings, NULL)))
		{
			success = SaveArray(array, pathname, "Paths");
			DeleteStringList(array);
		}
	}

	return success;
}



/*********************************************************************

	pathsettings = LoadPathSettings(pathname, oldpathsettings)

*********************************************************************/

struct pathsettings *LoadPathSettings(char *pathname, struct pathsettings *oldsettings)
{
	struct pathsettings *currentsettings;

	if ((currentsettings = CreatePathSettings(NULL, oldsettings, NULL)))
	{
		struct pathsettings *tempsettings;

		if (pathname)
		{
			char *fullname;
			char **array;

			if ((fullname = FullName(pathname, "Paths")))
			{
				if ((array = LoadStringList(fullname)))
				{
					if ((tempsettings = CreatePathSettings(array, currentsettings, NULL)))
					{
						DeletePathSettings(currentsettings);
						currentsettings = tempsettings;
					}
					DeleteStringList(array);
				}
				Free(fullname);
			}
		}
	}

	return currentsettings;
}
