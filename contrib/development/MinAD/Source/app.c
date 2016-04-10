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

/* MUI objects */
Object *MUI_App;
Object *MUI_Win[WIN_MAX];
Object *MUI_Obj[WIN_MAX][OBJ_MAX];
Object *MUI_Menu;
Object *MUI_MenuObj[10];

/*------------------------------------------------------------------------*/

struct DiskObject *diskobj = NULL;

/*------------------------------------------------------------------------*/

/* ASL CloseHook */
#if defined(__AROS__)
AROS_UFH3(void, ASLCloseF,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(struct FileRequester *, msg, A1))
{
	AROS_USERFUNC_INIT
#else
static void ASLCloseF(void)
{
	A1(struct FileRequester *, msg);
#endif

	if (msg && msg->fr_Drawer)
	{
		UBYTE realpath[1024];

		if (AssignAvail(msg->fr_Drawer, realpath))
			_strcpy(autodocs.ad_Path, realpath);
		else
			_strcpy(autodocs.ad_Path, msg->fr_Drawer);

		set(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, autodocs.ad_Path);
		UpdateListtree();
	}
#if defined(__AROS__)
	AROS_USERFUNC_EXIT
}
static struct Hook ASLCloseH = {{NULL,NULL}, (HOOKFUNC)ASLCloseF, NULL, NULL};
#else
}

MAKEHOOK(ASLCloseH, ASLCloseF); //MakeHook from "SDI_hook.h"
#endif

/*------------------------------------------------------------------------*/

/* NListtree DiplayHook */
#if defined(__AROS__)
AROS_UFH3(void, LTreeDspF,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(struct MUIP_NListtree_DisplayMessage *, msg, A1))
{
	AROS_USERFUNC_INIT
#else
static void LTreeDspF(void)
{
	A1(struct MUIP_NListtree_DisplayMessage *, msg);
#endif

	if (msg->TreeNode)
		*msg->Array++ = msg->TreeNode->tn_Name;
	else
		*msg->Array++ = "Index";
#if defined(__AROS__)
	AROS_USERFUNC_EXIT
}
static struct Hook LTreeDspH = {{NULL,NULL}, (HOOKFUNC)LTreeDspF, NULL, NULL};
#else
}

MAKEHOOK(LTreeDspH, LTreeDspF); //MakeHook from "SDI_hook.h"
#endif

/*------------------------------------------------------------------------*/

/* NList DiplayHook */
#if defined(__AROS__)
AROS_UFH3(void, NListDspF,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(Object *, object, A2),
AROS_UFHA(struct NList_DisplayMessage *, msg, A1))
{
	AROS_USERFUNC_INIT
#else
static void NListDspF(void)
{
	A1(struct NList_DisplayMessage *, msg);
#endif
	struct AutoDocMessage *adm = (struct AutoDocMessage *)msg->entry;

	if (adm) {
		if (adm->adm_File->adf_Group == GRP_MCC)
			msg->strings[0]	= "MCC";
		else if (adm->adm_File->adf_Group == GRP_MUI)
			msg->strings[0]	= "MUI";
		else if (adm->adm_File->adf_Group == GRP_NDK)
			msg->strings[0]	= "NDK";
		else
			msg->strings[0]	= "None";

		msg->strings[1]	= (char *)adm->adm_File->adf_FileNameShort;
		msg->strings[2]	= (char *)adm->adm_Func->adf_FuncNameShort;
		msg->preparses[0]	= "\033c";
		msg->preparses[1]	= "\033l";
		msg->preparses[2]	= "\033l";
	} else {
		msg->strings[0]	= "Group";
		msg->strings[1]	= "File";
		msg->strings[2]	= "Function";
		msg->preparses[0]	= "\033l";
		msg->preparses[1]	= "\033l";
		msg->preparses[2]	= "\033l";
	}
#if defined(__AROS__)
	AROS_USERFUNC_EXIT
}
static struct Hook NListDspH = {{NULL,NULL}, (HOOKFUNC)NListDspF, NULL, NULL};
#else
}

MAKEHOOK(NListDspH, NListDspF);
#endif

/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

BOOL InitApplication(void)
{
	static const UBYTE *modes[] =
	{
		"Autogroup",
		"Normal",
		NULL
	};

	diskobj = GetDiskObject(APP_DISKOBJECT);

	MUI_App = ApplicationObject,
		MUIA_Application_Title, APP_PROGNAME,
		MUIA_Application_Version, APP_VERSTAG,
		MUIA_Application_Copyright, APP_COPYRIGHT,
		MUIA_Application_Author, APP_AUTHOR,
		MUIA_Application_Description,	APP_DESCRIPTION,
		MUIA_Application_Base, APP_PROGNAME,
		MUIA_Application_DiskObject, diskobj,
		MUIA_Application_Menustrip, MUI_Menu = MenustripObject,
			Child, MenuObject,
				MUIA_Menu_Title, "Project",
				Child, MUI_MenuObj[MENU_About]	 = MenuitemObject, MUIA_Menuitem_Title, "About...",		  MUIA_Menuitem_Shortcut, "?", End,
				Child, MUI_MenuObj[MENU_AboutMUI] = MenuitemObject, MUIA_Menuitem_Title, "About MUI...",	 End,
				Child,										MenuitemObject, MUIA_Menuitem_Title, NM_BARLABEL,		 End,
				Child, MUI_MenuObj[MENU_Iconify]  = MenuitemObject, MUIA_Menuitem_Title, "Iconify",			MUIA_Menuitem_Shortcut, "I", End,
				Child, MUI_MenuObj[MENU_Quit]	  = MenuitemObject, MUIA_Menuitem_Title, "Quit",				MUIA_Menuitem_Shortcut, "Q", End,
			End,
			Child, MenuObject,
				MUIA_Menu_Title, "Tools",
				Child, MUI_MenuObj[MENU_Search]	= MenuitemObject, MUIA_Menuitem_Title, "Search function", MUIA_Menuitem_Shortcut, "S", End,
			End,
		End,

		SubWindow, MUI_Win[MAIN] = WindowObject,
			MUIA_Window_ID, MAKE_ID('M','A','I','N'),
			MUIA_Window_Title, APP_PROGNAME" · Main",
			WindowContents, VGroup,
				Child, HGroup,
					Child, Label("Path"),
					Child, PopaslObject,
						MUIA_CycleChain, 1,
						MUIA_Popasl_StopHook, &ASLCloseH,
						MUIA_Popstring_String, MUI_Obj[MAIN][M_Path] = String(NULL, SIZE_PATH),
						MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
						ASLFR_TitleText, "Select your AutoDocs path",
						ASLFR_DoPatterns, TRUE,
						ASLFR_DrawersOnly, TRUE,
					End,
				End,
				Child, HGroup,
					Child, VGroup,
						MUIA_Weight, 40,
						Child, NListviewObject,
							MUIA_Weight, 30,
							MUIA_CycleChain, 1,
							MUIA_NListview_NList, MUI_Obj[MAIN][M_Files] = NListtreeObject,
								MUIA_NListtree_MultiSelect, MUIV_NListtree_MultiSelect_None,
								MUIA_NListtree_DoubleClick, MUIV_NListtree_DoubleClick_Tree,
								MUIA_NListtree_DisplayHook, &LTreeDspH,
								MUIA_NListtree_EmptyNodes, FALSE,
								MUIA_NListtree_DragDropSort, FALSE,
								MUIA_NListtree_TreeColumn, 0,
							End,
						End,
						Child, BalanceObject, End,
						Child, HGroup,
							Child, Label("Mode"),
							Child, MUI_Obj[MAIN][M_Mode] = CycleObject,
								MUIA_CycleChain, 1,
								MUIA_Cycle_Entries, modes,
								MUIA_Cycle_Active, 0,
							End,
						End,
						Child, NListviewObject,
							MUIA_CycleChain, 1,
							MUIA_NListview_NList, MUI_Obj[MAIN][M_Docs] = NListtreeObject,
								MUIA_NListtree_MultiSelect, MUIV_NListtree_MultiSelect_None,
								MUIA_NListtree_DoubleClick, MUIV_NListtree_DoubleClick_Tree,
								MUIA_NListtree_DisplayHook, &LTreeDspH,
								MUIA_NListtree_EmptyNodes, FALSE,
								MUIA_NListtree_DragDropSort, FALSE,
								MUIA_NListtree_TreeColumn, 0,
							End,
						End,
						Child, HGroup,
							Child, Label("Filter"),
							Child, MUI_Obj[MAIN][M_Filter] =  StringObject,
								MUIA_CycleChain, 1,
								StringFrame,
								MUIA_String_MaxLen, SIZE_NAME,
								MUIA_String_Contents, NULL,
							End,
							Child, MUI_Obj[MAIN][M_FilterClear] =  TextObject,
								MUIA_Weight, 0,
								MUIA_CycleChain, 1,
								ButtonFrame,
								MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_Text_Contents, "C",
								MUIA_Text_PreParse, "\33c",
								MUIA_Text_HiChar, 'c',
								MUIA_ControlChar, 'c',
								MUIA_InputMode, MUIV_InputMode_RelVerify,
							End,
						End,
						Child, HGroup,
							Child, MUI_Obj[MAIN][M_OpenAll] = TextObject,
								MUIA_CycleChain, 1,
								ButtonFrame, MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_Text_Contents, "Open All",
								MUIA_Text_PreParse, "\33c",
								MUIA_Text_HiChar, 'o',
								MUIA_ControlChar, 'o',
								MUIA_InputMode, MUIV_InputMode_RelVerify,
							End,
							Child, MUI_Obj[MAIN][M_CloseAll] = TextObject,
								MUIA_CycleChain, 1,
								ButtonFrame, MUIA_Background, MUII_ButtonBack,
								MUIA_Font, MUIV_Font_Button,
								MUIA_Text_Contents, "Close All",
								MUIA_Text_PreParse, "\33c",
								MUIA_Text_HiChar, 'c',
								MUIA_ControlChar, 'c',
								MUIA_InputMode, MUIV_InputMode_RelVerify,
							End,
						End,
					End,
					Child, BalanceObject, End,
					//Child, ScrollgroupObject,
						//MUIA_CycleChain, TRUE,
						//MUIA_Scrollgroup_UseWinBorder, TRUE,
						//MUIA_Scrollgroup_Contents, VGroupV,
							Child, MUI_Obj[MAIN][M_Text] = NListviewObject,
								MUIA_CycleChain, 1,
								MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_None,
								MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
								MUIA_NListview_NList, NFloattextObject,
									MUIA_Font, MUIV_Font_Fixed,
									//MUIA_NList_DefaultObjectOnClick, TRUE,
									MUIA_NFloattext_Text, NULL,
									MUIA_NFloattext_TabSize, 4,
									MUIA_NFloattext_Justify, FALSE, //TRUE,
								End,
							End,
						//End,
					//End,
				End,
			End,
		End,

		SubWindow, MUI_Win[ABOUT] = WindowObject,
			MUIA_Window_Title, APP_PROGNAME" · About",
			MUIA_Window_SizeGadget, FALSE,
			WindowContents, VGroup,
				MUIA_Background, MUII_RequesterBack,
				Child, VGroup, VirtualFrame,
					InnerSpacing(10, 10),
					Child, DTPicObject,
						MUIA_DTPic_Name, APP_LOGOFILE,
					End,
					Child, MUI_Obj[ABOUT][A_Text] = TextObject, TextFrame,
						MUIA_FramePhantomHoriz, TRUE,
						MUIA_Weight, 0,
						MUIA_InnerLeft, 0,
						MUIA_InnerRight, 0,
						MUIA_Text_PreParse, "\033c",
						MUIA_Text_Contents, "",
					End,
				End,
				Child, VSpace(2),
				Child, HGroup,
					Child, HVSpace,
					Child, MUI_Obj[ABOUT][A_Ok] = TextObject, ButtonFrame,
						MUIA_CycleChain, 1,
						MUIA_Background, MUII_ButtonBack,
						MUIA_Font, MUIV_Font_Button,
						MUIA_Text_Contents, "Ok",
						MUIA_Text_PreParse, "\33c",
						MUIA_Text_HiChar, 'o',
						MUIA_ControlChar, 'o',
						MUIA_InputMode, MUIV_InputMode_RelVerify,
					End,
					Child, HVSpace,
				End,
			End,
		End,

		SubWindow, MUI_Win[SEARCH] = WindowObject,
			MUIA_Window_ID, MAKE_ID('S','E','A','R'),
			MUIA_Window_Title, APP_PROGNAME" · Search",
			WindowContents, VGroup,
				Child, NListviewObject,
					MUIA_CycleChain, 1,
					MUIA_NListview_NList, MUI_Obj[SEARCH][S_List] = NListObject,
						InputListFrame,
						MUIA_NList_DefaultObjectOnClick, TRUE,
						MUIA_NList_MultiSelect, MUIV_NList_MultiSelect_None,
						MUIA_NList_DisplayHook2, &NListDspH,
						MUIA_NList_Format, "BAR,BAR,BAR",
						MUIA_NList_SourceArray, NULL,
						MUIA_NList_AutoVisible, TRUE,
						MUIA_NList_TitleSeparator, TRUE,
						MUIA_NList_Title, TRUE,
						MUIA_NList_EntryValueDependent, TRUE,
						MUIA_NList_MinColSortable,	0,
						MUIA_NList_Imports, MUIV_NList_Imports_All,
						MUIA_NList_Exports, MUIV_NList_Exports_All,
					End,
				End,
				Child, HGroup,
					Child, Label("Pattern"),
					Child, MUI_Obj[SEARCH][S_String] = String(NULL, SIZE_NAME),
				End,
			End,
		End,

	End;

	if (!MUI_App) {
		ErrorMsg("Failed to create Application!"); return FALSE;
	}
	//menu
	DoMethod(MUI_MenuObj[MENU_About],	 MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_mProjct_About);
	DoMethod(MUI_MenuObj[MENU_AboutMUI], MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_mProjct_AboutMUI);
	DoMethod(MUI_MenuObj[MENU_Iconify],  MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_mProjct_Iconify);
	DoMethod(MUI_MenuObj[MENU_Quit],	  MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(MUI_MenuObj[MENU_Search],	MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_mTools_Search);

	//window close request
	DoMethod(MUI_Win[MAIN],	MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(MUI_Win[ABOUT],  MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(MUI_Win[SEARCH], MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUI_Win[SEARCH], 3, MUIM_Set, MUIA_Window_Open, FALSE);

	//other
	DoMethod(MUI_Obj[MAIN][M_Files], MUIM_Notify, MUIA_NListtree_Active,  MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_DirRide);
	DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_Notify, MUIA_NListtree_Active,  MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_ShowFunc);
	DoMethod(MUI_Obj[MAIN][M_Path], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_UpdateListtreeHitEnter);
	DoMethod(MUI_Obj[MAIN][M_OpenAll], MUIM_Notify, MUIA_Pressed, FALSE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_OpenAll);
	DoMethod(MUI_Obj[MAIN][M_CloseAll], MUIM_Notify, MUIA_Pressed, FALSE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_CloseAll);
	//DoMethod(MUI_Obj[MAIN][M_Filter], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Filter);
	DoMethod(MUI_Obj[MAIN][M_Filter], MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Filter);

	DoMethod(MUI_Obj[MAIN][M_FilterClear], MUIM_Notify, MUIA_Pressed, FALSE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_FilterClear);
	DoMethod(MUI_Obj[MAIN][M_Mode], MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_UpdateListtreeMode);

	DoMethod(MUI_Obj[SEARCH][S_List], MUIM_Notify, MUIA_NList_Active,  MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_SelectFromSearch);
	DoMethod(MUI_Obj[SEARCH][S_String], MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Search);

	DoMethod(MUI_Obj[ABOUT][A_Ok], MUIM_Notify, MUIA_Pressed, FALSE, MUI_App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	set(MUI_Win[MAIN], MUIA_Window_ActiveObject, MUI_Obj[MAIN][M_Text]);
	set(MUI_Win[ABOUT], MUIA_Window_ActiveObject, MUI_Obj[ABOUT][A_Ok]);

	return TRUE;
}

void ExitApplication(void)
{
	if (diskobj)
		FreeDiskObject(diskobj);

	MUI_DisposeObject(MUI_App);
}

/*------------------------------------------------------------------------*/

void mProject_About(void)
{
	BOOL done = FALSE;

	set(MUI_Obj[ABOUT][A_Text], MUIA_Text_Contents,
		"\033c\033b"APP_PROGNAME"\033n\n"
		""APP_DESCRIPTION"\n"
		"\n"
		"\033bVersion\033n\n"
		APP_VERREV" ["APP_BUILD"] ("APP_DATE")\n"
		"\n"
		"\033bAuthor\033n\n"
		APP_COPYRIGHT"\n"
		APP_HOMEPAGE"\n"
		APP_RIGHTS"\n"
		"\n"
		"\033bLicence\033n\n"
		APP_LICENCE"\n"
		"\n"
		"\n"
		"NList custom classes\n"
		"Copyright ©2001-"__YEAR__" NList Open Source Team\n"
		"http://www.sourceforge.net/projects/nlist-classes"
	);
	set(MUI_App, MUIA_Application_Sleep, TRUE);
	set(MUI_Win[ABOUT], MUIA_Window_Open, TRUE);

	while (!done)
	{
		ULONG signals;

		switch (DoMethod(MUI_App, MUIM_Application_NewInput, &signals))
		{
			case MUIV_Application_ReturnID_Quit:
				done = TRUE;
				break;
			default:
				break;
		}
		if (!done && signals)
			Wait(signals);
	}

	set(MUI_Win[ABOUT], MUIA_Window_Open, FALSE);
	set(MUI_App, MUIA_Application_Sleep, FALSE);
}

/*------------------------------------------------------------------------*/

void mProject_AboutMUI(void)
{
	Object *win;

	if ((win = AboutmuiObject, MUIA_Aboutmui_Application, MUI_App, End))
		set(win, MUIA_Window_Open, TRUE);
}

/*------------------------------------------------------------------------*/

void mProject_Iconify(void)
{
	set(MUI_App, MUIA_Application_Iconified, TRUE);
}

/*------------------------------------------------------------------------*/

void mTools_Search(void)
{
	set(MUI_Win[SEARCH], MUIA_Window_ActiveObject, MUI_Obj[SEARCH][S_String]);
	set(MUI_Win[SEARCH], MUIA_Window_Open, TRUE);
}

/*------------------------------------------------------------------------*/

