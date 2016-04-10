/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#include "include.h"

/*------------------------------------------------------------------------*/

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *IconBase = NULL;
struct Library *AslBase = NULL;
struct Library *MUIMasterBase = NULL;
#ifdef __MORPHOS__
struct Library *UtilityBase = NULL;
#else
struct UtilityBase *UtilityBase = NULL;
#endif

APTR pool = NULL;

/*------------------------------------------------------------------------*/

#ifdef __MORPHOS__
	#pragma pack(2)
#endif

static struct {
	APTR 	*base;
	UBYTE *name;
	ULONG  minver;
} libs[] = {
	{ (APTR *)&GfxBase, "graphics.library", MINSYS },
	{ (APTR *)&IntuitionBase, "intuition.library", MINSYS },
	{ (APTR *)&AslBase, "asl.library", MINSYS },
	{ (APTR *)&IconBase, "icon.library", MINSYS },
	{ (APTR *)&UtilityBase, "utility.library", MINSYS },
	{ (APTR *)&MUIMasterBase, "muimaster.library", MINMUI },
	{ NULL, NULL }
};

#ifdef __MORPHOS__
	#pragma pack()
#endif

static void Exit_Libs(void);
static BOOL Init_Libs(void)
{
	UBYTE i;

	for (i = 0; libs[i].base; i++) {
		if (!(*(libs[i].base) = (APTR)OpenLibrary(libs[i].name, libs[i].minver))) {
			ErrorMsg("Can't open %s v%lu", libs[i].name, libs[i].minver);
			Exit_Libs();
			return FALSE;
		}
	}
	return TRUE;
}

static void Exit_Libs(void)
{
	UBYTE i;

	for (i = 0; libs[i].base; i++) {
		if (*libs[i].base)
			CloseLibrary(*libs[i].base);
	}
}

/*------------------------------------------------------------------------*/

static int RunApplication(void)
{
	BOOL done = FALSE;

	set(MUI_Win[MAIN], MUIA_Window_Open, TRUE);
	while (!done)
	{
		ULONG signal;

		switch (DoMethod(MUI_App, MUIM_Application_Input, &signal))
		{
			case MUIV_Application_ReturnID_Quit:
				done = TRUE;
				break;
			case MUIV_mProjct_About:
				mProject_About();
				break;
			case MUIV_mProjct_AboutMUI:
				mProject_AboutMUI();
				break;
			case MUIV_mProjct_Iconify:
				mProject_Iconify();
				break;
			case MUIV_mTools_Search:
				mTools_Search();
				break;
			case MUIV_mTools_MakeGuide:
				mTools_MakeGuide();
				break;

			case MUIV_ShowFunc:
				ShowFunc();
				break;
			case MUIV_DirRide:
				DirRide();
				break;
			case MUIV_UpdateListtree:
				UpdateListtree();
				break;
			case MUIV_OpenAll:
				OpenCloseAll(TRUE);
				break;
			case MUIV_CloseAll:
				OpenCloseAll(FALSE);
				break;
			case MUIV_UpdateListtreeHitEnter:
			{
				UBYTE *path, realpath[1024];

				get(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, &path);
				if (AssignAvail(path, realpath))
					_strcpy(autodocs.ad_Path, realpath);
				else
					_strcpy(autodocs.ad_Path, path);

				set(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, autodocs.ad_Path);
				UpdateListtree();
				break;
			}
			case MUIV_UpdateListtreeMode:
			{
				if (autodocs.ad_Mode == MODE_NORMAL)
					autodocs.ad_Mode = MODE_AUTOGROUP;
				else
					autodocs.ad_Mode = MODE_NORMAL;

				ShowListtree();
				PrintfText(NULL);
				break;
			}
			case MUIV_Filter:
				Filter();
				OpenCloseAll(TRUE);
				break;
			case MUIV_FilterClear:
				FilterClear();
				//OpenCloseAll(FALSE);
				break;

			case MUIV_Search:
				Search();
				break;
			case MUIV_SelectFromSearch:
				SelectFromSearch();
				break;

			default:
				break;
		}
		if (!done && signal)
			Wait(signal);
	}
	set(MUI_Win[MAIN], MUIA_Window_Open, FALSE);
	return RETURN_OK;
}

/*------------------------------------------------------------------------*/

int main2(void)
{
	int rc = RETURN_FAIL;

	if (Init_Libs())
	{
		if ((pool = CreatePool(MEMF_PUBLIC, 32768, 32768)))
		{
			if (InitApplication())
			{
				InitListtree();
				rc = RunApplication();
				ExitListtree();
				ExitApplication();
			}
			DeletePool(pool);
		}
		Exit_Libs();
	}
		
	return rc;
}

/*------------------------------------------------------------------------*/

