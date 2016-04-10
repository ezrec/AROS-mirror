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

#ifndef APP_H
#define APP_H 1

/*------------------------------------------------------------------------*/

#define WIN_MAX 	3
#define OBJ_MAX 	9

/*------------------------------------------------------------------------*/

enum windows
{
	MAIN = 0,
	SEARCH,
	ABOUT,
};

enum objects_main
{
	M_Path = 0,
	M_Files,
	M_Docs,
	M_Text,
	M_OpenAll,
	M_CloseAll,
	M_Filter,
	M_FilterClear,
	M_Mode,
};

enum objects_search
{
	S_List = 0,
	S_String,
	S_Done,
};

enum objects_about
{
	A_Text = 0,
	A_Ok,
};

enum menu_ids
{
	MENU_PROJECT = 1,
		MENU_About,
		MENU_AboutMUI,
		MENU_Iconify,
		MENU_Quit,
	MENU_TOOLS = 5,
		MENU_Search,
		MENU_MakeGuide,
};

/*------------------------------------------------------------------------*/

extern Object *MUI_App;
extern Object *MUI_Win[WIN_MAX];
extern Object *MUI_Obj[WIN_MAX][OBJ_MAX];

/*------------------------------------------------------------------------*/

BOOL InitApplication(void);
void ExitApplication(void);

void mProject_About(void);
void mProject_AboutMUI(void);
void mProject_Iconify(void);
void mTools_Search(void);

/*------------------------------------------------------------------------*/

#endif /* APP_H */
