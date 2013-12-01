// Delete.module.c
// $Date$
// $Revision$
// $Id$

//     ___       ___
//   _/  /_______\  \_     ___ ___ __ _                       _ __ ___ ___.
//__//  / _______ \  \\___/                                               \___
//_/ | '  \__ __/  ` | \_/        © Copyright 1999, Christopher Page       \__
// \ | |    | |__  | | / \   Released as Free Software under the GNU GPL   /.
//  >| .    |  _/  . |<   >--- --- -- -                       - -- --- ---<.
// / \  \   | |   /  / \ /   This file is part of the ScalosDelete code    \.
// \  \  \_/   \_/  /  / \  and it is released under the GNU GPL. Please   /.
//  \  \           /  /   \   read the "COPYING" file which should have   /.
// //\  \_________/  /\\ //\    been included in the distribution arc.   /.
//- --\   _______   /-- - --\      for full details of the license      /-----
//-----\_/       \_/---------\   ___________________________________   /------
//                            \_/                                   \_/.
//
// Description:
//
//  Interface setup and entrypoints for ScalosDelete
//
// Functions:
//
//  int  wbmain        (WBStartup   *);
//  int  main          (int , char **);
//  void ExitMUI       (APTR, STRPTR);
//  void InitMUI       (void);
//  void SetupInterface(void);
//
// Detail:
//
//  This file contains the setup/ shutdown, interface creation and entrypoint
//  functions for ScalosDelete. It is written for StormC so some modification
//  will be required to recompile under other systems. Note that StormC uses
//  wbmain() as the entrypoint for workbench launched programs, you can probably
//  remove this if you recompile under SAS/C or similar (DiceC uses the same
//  mechanism).
//
// Modification History:
//
//  [02 June 1999, Chris <chris@worldfoundry.demon.co.uk>]
//
//  Converted header notice to GNU GPL version, checked and recommented where
//  required.
//
// Fold Markers:
//
//  Start: /*GFS*/
//    End: /*GFE*/

#define Delete_CODE
#define Delete_ARRAY
#include"Delete.module.h"

//----------------------------------------------------------------------------

/*GFE*/
// Prototypes
int  wbmain          (struct WBStartup *);
int  main            (int, char **);
void ExitMUI         (APTR, STRPTR);
void InitMUI         (void);
void SetupInterface  (void);

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg);
static SAVEDS(void) INTERRUPT EnableTrashcanHookFunc(struct Hook *hook, Object *o, Msg msg);
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev);

//----------------------------------------------------------------------------

const char Version[]   = VERSTAG;

// Library bases and classes. Only hand-open the ones we really need a specific
// version of..
extern struct ExecBase *SysBase;
struct Library *MUIMasterBase = NULL;
struct Library *AslBase       = NULL;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase     = NULL;
#endif
struct ScalosBase *ScalosBase = NULL;
T_LOCALEBASE LocaleBase       = NULL;
struct Catalog *WordsCat      = NULL;
static struct Locale *DeleteLocale     = NULL;

#ifdef __amigaos4__
struct IntuitionBase *IntuitionBase = NULL;

struct MUIMasterIFace *IMUIMaster = NULL;
struct AslIFace *IAsl             = NULL;
struct ScalosIFace *IScalos       = NULL;
struct LocaleIFace *ILocale       = NULL;
struct IntuitionIFace *IIntuition = NULL;
#endif

// MUI globals
/*GFS*/
Object *MUI_App      = NULL;
Object *WI_Delete    = NULL;

Object *LV_Files     = NULL;
Object *BT_Details   = NULL;
Object *TX_ReadOut   = NULL;

Object *BT_Delete    = NULL;
Object *BT_About     = NULL;
Object *BT_Cancel    = NULL;

Object *GP_Progress  = NULL;
Object *TX_Progress  = NULL;
Object *GA_Progress  = NULL;
Object *BT_STOP      = NULL;

Object *ST_TrashDir  = NULL;
Object *CM_TrashCan  = NULL;
Object *CM_DirConf   = NULL;
Object *CM_FileConf  = NULL;
Object *CM_Quiet     = NULL;
Object *BT_Save      = NULL;
Object *BT_Use       = NULL;
Object *BT_Restore   = NULL;

CONST_STRPTR Pages[ 3] = { NULL };
CONST_STRPTR Keys [14] = { NULL };

static Object *LampUndoPossible;
static Object *TextUndoPossible;

Object *WI_About     = NULL;
Object *BT_About_MUI = NULL;
static Object *MenuAbout;
static Object *MenuAboutMUI;
static Object *MenuQuit;
static Object *WIN_AboutMUI;

static struct Hook AboutMUIHook = { { NULL, NULL }, HOOKFUNC_DEF(OpenAboutMUIHookFunc), NULL };
static struct Hook EnableTrashcanHook = { { NULL, NULL }, HOOKFUNC_DEF(EnableTrashcanHookFunc), NULL };

//----------------------------------------------------------------------------

/* void ExitMUI(APTR, STRPTR)                                                */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=                                                */
/* If an error occurs, or when the window is closed, this routine is called  */
/* to clean up all the resources, display an error message if required and   */
/* then exit().                                                              */
/*                                                                           */
/* Parameters:                                                               */
/*      errApp      Pointer to the MUI Application object to close.          */
/*      errString   String to display in an EasyRequest. If this is          */
/*                  NULL then nothing is displayed                           */

/*GFS*/  void ExitMUI(APTR errApp, STRPTR errString)
{
//	LONG EntryNum = 0;

	struct EasyStruct ErrorDisplay =
		{
	        sizeof(struct EasyStruct),
	        0,
	        "Delete.module Error",      // Not localised (may not have Locale y'see..)
	        "%s",
	        "Ok",
		};

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	// Show the error
	if(errString)
		EasyRequest(NULL, &ErrorDisplay, NULL, errString, 0);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if(errApp)
		MUI_DisposeObject(errApp);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	// Clean up
	if(FileBuffer)
		free(FileBuffer);
	if(PathBuffer)
		free(PathBuffer);
	if(DirBuffer)
		free(DirBuffer);

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if(WordsCat)
		CloseCatalog(WordsCat     );
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
	if (DeleteLocale)
		CloseLocale(DeleteLocale);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

#ifdef __amigaos4__
	if (IIntuition )
		DropInterface((struct Interface *)IIntuition  );
#endif
	if (IntuitionBase)
		CloseLibrary((struct Library *) IntuitionBase);
#ifdef __amigaos4__
	if(IMUIMaster   )
		DropInterface((struct Interface *)IMUIMaster  );
#endif
	if(MUIMasterBase)
		CloseLibrary(MUIMasterBase);
#ifdef __amigaos4__
	if(IAsl         )
		DropInterface((struct Interface *)IAsl        );
#endif
	if(AslBase      )
		CloseLibrary(AslBase      );
#ifndef __amigaos4__
	if(UtilityBase  )
		CloseLibrary((struct Library *) UtilityBase  );
#endif
#ifdef __amigaos4__
	if(IScalos      )
		DropInterface((struct Interface *)IScalos     );
#endif
	if(ScalosBase   )
		CloseLibrary(&ScalosBase->scb_LibNode);
	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
#ifdef __amigaos4__
	if(ILocale      )
		DropInterface((struct Interface *)ILocale     );
#endif
	if(LocaleBase   )
		CloseLibrary((struct Library *) LocaleBase   );

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));

	exit(0);
}/*GFE*/


/* void InitMUI(void)                                                        */
/* -=-=-=-=-=-=-=-=-=                                                        */
/* This routine opens the libraries and locale catalog. If an error occurs   */
/* (ie: library will not open, memory allocation fails etc) then ExitMUI()   */
/* is called with an error. Note that the error messages are not localised as*/
/* locale.library may not be available when the error is encountered..       */

/*GFS*/  void InitMUI(void)
{
	if(!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN)))
		{
	        ExitMUI(NULL, "Unable to open "MUIMASTER_NAME".");
		}
#ifdef __amigaos4__
	else
		{
		IMUIMaster = (struct MUIMasterIFace *)GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
		}
#endif

	if(!(AslBase       = OpenLibrary(AslName , 39)))
		{
	        ExitMUI(NULL, "Unable to open "AslName".");
		}
#ifdef __amigaos4__
	else
		{
		IAsl = (struct AslIFace *)GetInterface((struct Library *)AslBase, "main", 1, NULL);
		}
#endif
	if(!(IntuitionBase       = (struct IntuitionBase *) OpenLibrary(IntuitionName , 39)))
		{
		ExitMUI(NULL, "Unable to open " IntuitionName ".");
		}
#ifdef __amigaos4__
	else
		{
		IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
		}
#endif
#ifndef __amigaos4__
	if(!(UtilityBase   = (T_UTILITYBASE) OpenLibrary(UTILITYNAME, 39)))
		{
	        ExitMUI(NULL, "Unable to open "UTILITYNAME".");
		}
#endif

	if(!(ScalosBase   = (struct ScalosBase *) OpenLibrary("scalos.library", 39)))
		{
	        ExitMUI(NULL, "Unable to open scalos.library.");
		}
#ifdef __amigaos4__
	else
		{
		IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
		}
#endif

	if(!(LocaleBase    = (T_LOCALEBASE)OpenLibrary("locale.library", 37)))
		{
	        ExitMUI(NULL, "Unable to open locale.library v39+");
		}
#ifdef __amigaos4__
	else
		{
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
		}
#endif

	DeleteLocale = OpenLocale(NULL);
	if (DeleteLocale)
		WordsCat = OpenCatalogA(DeleteLocale, "Scalos/Delete.catalog", NULL);

        if(!(FileBuffer = malloc(MAX_FILENAME_LEN)))
		{
		ExitMUI(NULL, "Unable to allocate file buffer");
		}

        if(!(PathBuffer = malloc(MAX_FILENAME_LEN)))
		{
		ExitMUI(NULL, "Unable to allocate path buffer");
		}

        if(!(DirBuffer  = malloc(MAX_FILENAME_LEN)))
		{
		ExitMUI(NULL, "Unable to allocate directory buffer");
		}
}/*GFE*/

#if 0
/* int wbmain(WBStartup *)                                                   */
/* -=-=-=-=-=-=-=-=-=-=-=-                                                   */
/* Entry point for WB started programs. Required by StormC and DiceC but can */
/* probably be removed for other system. It just calls main() with argc set  */
/* to 0 and argv pointing to the WBStartup message.                          */

/*GFS*/  int wbmain(struct WBStartup *wb_Startup)
{
	return (main(0, (char **)wb_Startup));
}/*GFE*/
#endif

/* int main(int char **)                                                     */
/* -=-=-=-=-=-=-=-=-=-=-                                                     */
/* Eh? You want COMMENTS?? For *MAIN*??? Oh, all right then main routine..   */
/* there that's all your getting. Comments indeed..                          */

/*GFS*/  int main(int argc, char **argv)
{
	BOOL  Running = TRUE;
	ULONG  Sig     =    0;
	ULONG  Source  =    0;

	// Wake up MUI...
	InitMUI();

	if (!CheckMCCVersion(MUIC_NList, 20, 121) ||
		!CheckMCCVersion(MUIC_NListview, 19, 66) ||
		!CheckMCCVersion(MUIC_Lamp, 11, 1) )
		return 10;

	SetupInterface();

	// Bomb if the user has started the program from shell (daft user...) otherwise
	// read the filelist...
	if(argc != 0)
		{
		ExitMUI(MUI_App, "Delete.module may not be started from shell");
		}

	// Bomb out if something has gone clogs-skyward
	if(!MUI_App)
		{
		ExitMUI(MUI_App, "Unable to create application");
		}

	CallHookPkt(&EnableTrashcanHook, MUI_App,  NULL);

	// Some domethods to make processing easier.
	DoMethod(WI_Delete   , MUIM_Notify, MUIA_Window_CloseRequest, TRUE          , MUI_App    , 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	DoMethod(BT_Cancel   , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	DoMethod(BT_Delete   , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, DELETE_IT);
	DoMethod(BT_Details  , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, DETAIL_IT);
	DoMethod(BT_STOP     , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, STOP_IT  );

	DoMethod(BT_Save     , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, PREFSSAVE);
	DoMethod(BT_Use      , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, PREFSUSE );
	DoMethod(BT_Restore  , MUIM_Notify, MUIA_Pressed            , FALSE         , MUI_App    , 2, MUIM_Application_ReturnID, PREFSLOAD);

	DoMethod(CM_TrashCan , MUIM_Notify, MUIA_Selected           , MUIV_EveryTime,
		MUI_App, 2, MUIM_CallHook, &EnableTrashcanHook);

	// About handling...
	DoMethod(WI_About    , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , WI_About, 3, MUIM_Set, MUIA_Window_Open, FALSE   );
	DoMethod(BT_About    , MUIM_Notify, MUIA_Pressed            , FALSE, WI_About, 3, MUIM_Set, MUIA_Window_Open, TRUE    );
	DoMethod(BT_About_MUI, MUIM_Notify, MUIA_Pressed            , FALSE, MUI_App , 2, MUIM_Application_AboutMUI , WI_About);

	// Menu handling
	DoMethod(MenuAbout, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		WI_About, 3, MUIM_Set, MUIA_Window_Open, TRUE);
	DoMethod(MenuAboutMUI, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		MUI_App, 2, MUIM_CallHook, &AboutMUIHook);
	DoMethod(MenuQuit, MUIM_Notify,MUIA_Menuitem_Trigger,MUIV_EveryTime,
		MUI_App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

	// Read in the settings
	DoMethod(MUI_App, MUIM_Application_Load, MUIV_Application_Load_ENV);

	// Get the show on the road...
	set(WI_Delete, MUIA_Window_Open, TRUE);

	// +jmc+ : Activate delete button gadget
	set(WI_Delete, MUIA_Window_ActiveObject, BT_Delete);

	// Open the about window & GNU GPL details....
	get(CM_Quiet , MUIA_Selected   , &Source);
	if(Source == 0)
		set(WI_About, MUIA_Window_Open, TRUE);

	if(!ListFiles((struct WBStartup *)argv))
		ExitMUI(MUI_App, NULL);

	// Main loop: Block on the MUI Notify...
	while (Running)
	{
		Source = DoMethod(MUI_App, MUIM_Application_NewInput, &Sig);
		switch(Source)
			{
		case MUIV_Application_ReturnID_Quit:
		        Running = FALSE;
		        break;
		case PREFSSAVE:
		        DoMethod(MUI_App, MUIM_Application_Save, MUIV_Application_Save_ENVARC);
		case PREFSUSE :
		        DoMethod(MUI_App, MUIM_Application_Save, MUIV_Application_Save_ENV);
		        break;
		case PREFSLOAD:
		        DoMethod(MUI_App, MUIM_Application_Load, MUIV_Application_Load_ENV);
		        break;
		case DETAIL_IT:
		        GetDetails((struct WBStartup *)argv);
		        break;
		case DELETE_IT:
			set(BT_Details, MUIA_Disabled, TRUE);
			set(BT_Delete, MUIA_Disabled, TRUE);
			set(BT_About, MUIA_Disabled, TRUE);
			set(BT_Cancel, MUIA_Disabled, TRUE);
		        KillFiles((struct WBStartup *)argv);
			Running = FALSE;
		        break;
			}
		if(Running && Sig)
			Wait(Sig);
	}

	// Clean up and begone.
	ExitMUI(MUI_App, NULL);

	return 0;
}/*GFE*/


/* void SetupInterface(void)                                                 */
/* -=-=-=-=-=-=-=-=-=-=-=-=-                                                 */
/* Sets up the interface all in one compound command. Much easier to read    */
/* than lots of small functions adding bits here and there IMO...            */

/*GFS*/  void SetupInterface(void)
{
	char VersionBuffer[64];
	char DateBuffer   [64];

	// Set up the compiled and version info...
	sprintf(VersionBuffer, "%d.%d", VERSION, REVISION);
	sprintf(DateBuffer   , "%s %s"   , __TIME__, __DATE__);

	// Localise the page names
	Pages[0] = GetLocString(MSG_PAGES_FILES);
	Pages[1] = GetLocString(MSG_PAGES_PREFS);

	// and the key shortcuts. Bit of a hack this really....
	Keys[ 0]  = GetLocString(MSG_KEY_DETAILS);
	Keys[ 1]  = GetLocString(MSG_KEY_TRASH  );
	Keys[ 2]  = GetLocString(MSG_KEY_DIRS   );
	Keys[ 3]  = GetLocString(MSG_KEY_FILES  );
	Keys[ 4]  = GetLocString(MSG_KEY_QUIET  );
	Keys[ 5]  = GetLocString(MSG_KEY_TLOC   );
	Keys[ 6]  = GetLocString(MSG_KEY_SAVE   );
	Keys[ 7]  = GetLocString(MSG_KEY_USE    );
	Keys[ 8]  = GetLocString(MSG_KEY_REST   );
	Keys[ 9]  = GetLocString(MSG_KEY_DELETE );
	Keys[10]  = GetLocString(MSG_KEY_ABOUT  );
	Keys[11]  = GetLocString(MSG_KEY_CANCEL );
	Keys[12]  = GetLocString(MSG_KEY_STOP   );
	Keys[13]  = GetLocString(MSG_KEY_AMUI   );

	MUI_App = ApplicationObject,
		MUIA_Application_Title      , GetLocString(MSG_APP_TITLE),
                MUIA_Application_Version    , &Version[1],
                MUIA_Application_Copyright  , "(C)1999 Chris Page, The World Foundry",
                MUIA_Application_Author     , "Chris Page, The World Foundry",
		MUIA_Application_Description, GetLocString(MSG_APP_DESC),
                MUIA_Application_Base       , BASENAME,
                MUIA_Application_SingleTask , TRUE,
/*GFS*/         SubWindow, WI_Delete = WindowObject,
		        MUIA_Window_Title      , GetLocString(MSG_WIN_TITLE),
                        MUIA_Window_ID         , MAKE_ID('M','A','I','N'),
                        MUIA_Window_ScreenTitle, VSTR,
                        WindowContents, VGroup,
				Child, RegisterGroup(Pages),
					MUIA_CycleChain, TRUE,
					MUIA_Background, MUII_RegisterBack,
					Child, VGroup,
						Child, HGroup,
						        GroupFrame,
						        MUIA_Background, MUII_GroupBack,
							Child, LampUndoPossible = LampObject,
								MUIA_Lamp_Type, MUIV_Lamp_Type_Huge,
								MUIA_Lamp_Color, MUIV_Lamp_Color_Off,
								End, //LampObject
							Child, TextUndoPossible = TextObject,
								MUIA_Text_Contents, GetLocString(MSGID_UNDO_NOT_POSSIBLE),
								End, //TextObject
							End, //HGroup

						Child, VGroup,
							MUIA_Group_Spacing, 0,
							Child,  LV_Files = NListviewObject,
								MUIA_CycleChain, 1,
		                                                MUIA_NListview_Vert_ScrollBar, TRUE,
		                                                MUIA_NListview_NList         , NListObject,
			                                                MUIA_CycleChain, 1,
			                                                MUIA_NList_Input         , FALSE,
			                                                MUIA_NList_DragSortable  , FALSE,
			                                                MUIA_NList_Title         , TRUE,
			                                                MUIA_NList_Format        , "BAR,BAR,BAR,",
			                                                MUIA_NList_ConstructHook , &ConList,
			                                                MUIA_NList_DestructHook  , &DesList,
			                                                MUIA_NList_DisplayHook   , &DisList,
			                                                MUIA_NList_CompareHook   , &CmpList,
									End, //NListObject
								End, //NListviewObject
							Child, BT_Details = KeyButtonChain(GetLocString(MSG_BUTTON_FULL), Keys[0][0], 1),
							End, //VGroup

	                                        Child, TX_ReadOut =  TextObject,
		                                        TextFrame,
		                                        MUIA_Background, MUII_TextBack,
		                                        MUIA_Weight    , 800,
							MUIA_Text_PreParse, "\33c",
		                                        MUIA_FixHeightTxt , " ",
							End, //TextObject

						End, //VGroup

					Child, VGroup,
						Child, ColGroup(4),
						        GroupFrame,
						        MUIA_Background, MUII_GroupBack,

							Child, HVSpace,
						        Child, KeyLabel1(GetLocString(MSG_LABEL_TRASH), Keys[1][0]),
							Child, CM_TrashCan = KeyCheckMarkHelpID(TRUE, Keys[1][0], 1, MAKE_ID('C', 'M', 'T', 'C'), GetLocString(MSG_HELP_TRASH)),
						        Child, HVSpace,

						        Child, HVSpace,
						        Child, KeyLabel1(GetLocString(MSG_LABEL_DIRS)  , Keys[2][0]),
							Child, CM_DirConf  = KeyCheckMarkHelpID(FALSE, Keys[2][0], 1, MAKE_ID('C', 'M', 'D', 'C'), GetLocString(MSG_HELP_DIRS )),
						        Child, HVSpace,

						        Child, HVSpace,
						        Child, KeyLabel1(GetLocString(MSG_LABEL_FILES), Keys[3][0]),
							Child, CM_FileConf = KeyCheckMarkHelpID(FALSE, Keys[3][0], 1, MAKE_ID('C', 'M', 'F', 'C'), GetLocString(MSG_HELP_FILES)),
						        Child, HVSpace,

						        Child, HVSpace,
						        Child, KeyLabel1(GetLocString(MSG_LABEL_QUIET), Keys[4][0]),
							Child, CM_Quiet    = KeyCheckMarkHelpID(TRUE, Keys[4][0], 1, MAKE_ID('C', 'M', 'Q', 'I'), GetLocString(MSG_HELP_QUIET)),
						        Child, HVSpace,

							End, //ColGroup

						Child, HVSpace,

						Child, HGroup,
							GroupFrameT(GetLocString(MSG_LABEL_TLOC)),

		                                        Child, ST_TrashDir = PopaslObject,
								MUIA_Popstring_String, StringObject,
		                                                        StringFrame,
		                                                        MUIA_String_MaxLen  , 128,
		                                                        MUIA_CycleChain     ,   1,
		                                                        MUIA_ControlChar    , Keys[5][0],
								        MUIA_ShortHelp      , GetLocString(MSG_HELP_TLOC),
		                                                        MUIA_ExportID       , MAKE_ID('S', 'T', 'T', 'C'),
									End, //StringObject
	                                                        MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
	                                                        MUIA_CycleChain      , 1,
							        ASLFR_TitleText      , GetLocString(MSG_ASL_TITLE),
	                                                        ASLFR_RejectIcons    , TRUE,
	                                                        ASLFR_DrawersOnly    , TRUE,
								End, //PopaslObject
							End, //HGroup
	                                        Child, VSpace(0),
	                                        Child, HGroup,
		                                        MUIA_Group_SameSize, TRUE,
						        Child, BT_Save    = KeyButtonChain(GetLocString(MSG_BUTTON_SAVE), Keys[6][0], 1),
						        Child, BT_Use     = KeyButtonChain(GetLocString(MSG_BUTTON_USE ), Keys[7][0], 1),
						        Child, BT_Restore = KeyButtonChain(GetLocString(MSG_BUTTON_REST), Keys[8][0], 1),
							End, //HGroup
						End, //VGroup
					End, //RegisterGroup

                                Child, HGroup,
	                                MUIA_Group_SameSize, TRUE,
				        Child, BT_Delete = KeyButtonChain(GetLocString(MSG_BUTTON_DEL), Keys[ 9][0], 1),
				        Child, BT_About  = KeyButtonChain(GetLocString(MSG_BUTTON_ABT), Keys[10][0], 1),
				        Child, BT_Cancel = KeyButtonChain(GetLocString(MSG_BUTTON_CAN), Keys[11][0], 1),
					End, //HGroup

				Child, GP_Progress = VGroup,
					GroupFrame,
	                                MUIA_Background, MUII_GroupBack,
	                                MUIA_ShowMe, FALSE,
	                                Child, HGroup,
	                                        Child, TX_Progress = TextObject,
							MUIA_Background, MUII_TextBack,
		                                        TextFrame,
		                                        MUIA_Weight    , 800,
		                                        MUIA_Background, MUII_TextBack,
							MUIA_Text_PreParse, "\33c",
		                                        MUIA_FixHeightTxt , " ",
							End, //TextObject
						Child, BT_STOP = KeyButtonChain(GetLocString(MSG_BUTTON_STOP), Keys[12][0], 1),
						End, //HGroup
	                                Child, VGroup,
	                                        Child, GA_Progress = GaugeObject,
		                                        GaugeFrame,
		                                        MUIA_Gauge_Current ,    0,
		                                        MUIA_Gauge_Max     ,  100,
		                                        MUIA_Gauge_Horiz   , TRUE,
		                                        MUIA_FixHeightTxt  , " ",
						        MUIA_Gauge_InfoText, GetLocString(MSG_LABEL_INFO),
							End, //GaugeObject
						Child, ScaleObject,
							MUIA_Scale_Horiz  , TRUE,
							End, //ScaleObject
						End, //VGroup
					End, //VGroup
				End, //VGroup
/*GFE*/			End, //WindowObject

/*GFS*/         SubWindow, WI_About = WindowObject,
		        MUIA_Window_Title      , GetLocString(MSG_BUTTON_ABT),
                        MUIA_Window_ID         , MAKE_ID('A','B','O','T'),
                        MUIA_Window_ScreenTitle, VSTR,
                        WindowContents, VGroup,
                                Child, TextObject,
	                                TextFrame,
	                                MUIA_Background, MUII_TextBack,
				        MUIA_Text_Contents, GetLocString(MSG_ABOUT_HEAD),
					End, //TextObject
				Child, ColGroup(7),
	                                Child, HSpace(0),
	                                Child, TextObject,
						MUIA_Text_Contents, GetLocString(MSG_LABEL_VERS),
						MUIA_Text_PreParse, "\33r",
						End, //TextObject
	                                Child, TextObject,
						MUIA_Text_Contents, VersionBuffer,
						End, //TextObject
	                                Child, HSpace(0),
	                                Child, TextObject,
						MUIA_Text_Contents, GetLocString(MSG_LABEL_COMP),
						MUIA_Text_PreParse, "\33r",
						End, //TextObject
	                                Child, TextObject,
						MUIA_Text_Contents, DateBuffer,
						End, //TextObject
	                                Child, HSpace(0),
					End, //ColGroup
				Child, ScrollgroupObject,
					MUIA_Scrollgroup_Contents, VGroupV,
						VirtualFrame ,
						Child, TextObject,
							MUIA_Background, MUII_TextBack,
							MUIA_Text_Contents, GetLocString(MSG_ABOUT_BODY),
							End, //TextObject
						End, //VGroupV
					End, //ScrollgroupObject

				Child, BT_About_MUI  = KeyButtonChain(GetLocString(MSG_BUTTON_AMUI), Keys[13][0], 1),
				End, //VGroup
/*GFE*/                 End, //WindowObject

		MUIA_Application_Menustrip, MenustripObject,
			Child, MenuObjectT(GetLocString(MSGID_MENU_PROJECT)),

				Child, MenuAbout = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUT),
				End,
				Child, MenuAboutMUI = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_ABOUTMUI),
				End,
				Child, MenuitemObject,
					MUIA_Menuitem_Title, -1,
				End,
				Child, MenuQuit = MenuitemObject,
					MUIA_Menuitem_Title, GetLocString(MSGID_MENU_PROJECT_QUIT),
					MUIA_Menuitem_Shortcut, GetLocString(MSGID_MENU_PROJECT_QUIT_SHORT),
				End,

			End, //MenuObjectT
		End, //MenuStripObject
	End; //ApplicationObject
}/*GFE*/

//----------------------------------------------------------------------------

CONST_STRPTR GetLocString(ULONG MsgId)
{
	struct Delete_LocaleInfo li;

	li.li_Catalog = WordsCat;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

//	KPrintF(__FILE__ "%s/%ld: Catalog=%08lx  LocaleBase=%08lx\n",
//		__FUNC__, __LINE__, li.li_Catalog, li.li_LocaleBase);

	return (CONST_STRPTR) GetDeleteString(&li, MsgId);
}

//----------------------------------------------------------------------------

STRPTR safe_strcat(STRPTR dest, CONST_STRPTR src, size_t DestLen)
{
	STRPTR dst;
	size_t Len = strlen(dest);

	if (DestLen < Len)
		return dest;

	DestLen -= Len;
	dst = dest + Len;

	while (DestLen > 1 && *src)
		{
		*dst++ = *src++;
		DestLen--;
		}
	*dst = '\0';

	return dest;
}

//----------------------------------------------------------------------------

#if !defined(__SASC) &&!defined(__MORPHOS__)
// Replacement for SAS/C library functions

size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}

#endif /* __SASC */

//----------------------------------------------------------------------------

static SAVEDS(APTR) INTERRUPT OpenAboutMUIHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	if (NULL == WIN_AboutMUI)
		{
		WIN_AboutMUI = MUI_NewObject(MUIC_Aboutmui,
			MUIA_Window_RefWindow, WI_Delete,
			MUIA_Aboutmui_Application, MUI_App,
			End;
		}

	if (WIN_AboutMUI)
		set(WIN_AboutMUI, MUIA_Window_Open, TRUE);

	return 0;
}

//----------------------------------------------------------------------------

static SAVEDS(void) INTERRUPT EnableTrashcanHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	ULONG UseTrashcan = 0;

	get(CM_TrashCan, MUIA_Selected, &UseTrashcan);

	set(ST_TrashDir, MUIA_Disabled, UseTrashcan);
	set(LampUndoPossible, MUIA_Lamp_Color, UseTrashcan ? MUIV_Lamp_Color_Ok : MUIV_Lamp_Color_Error);
	set(TextUndoPossible, MUIA_Text_Contents, GetLocString(UseTrashcan ? MSGID_UNDO_OK : MSGID_UNDO_NOT_POSSIBLE));
}

//----------------------------------------------------------------------------

//  Checks if a certain version of a MCC is available
static BOOL CheckMCCVersion(CONST_STRPTR name, ULONG minver, ULONG minrev)
{
	BOOL flush = TRUE;

	d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: %s ", name, __LINE__);)

	while (1)
		{
		ULONG ver = 0;
		ULONG rev = 0;
		struct Library *base;
		char libname[256];

		// First we attempt to acquire the version and revision through MUI
		Object *obj = MUI_NewObject((STRPTR) name, TAG_DONE);
		if (obj)
			{
			get(obj, MUIA_Version, &ver);
			get(obj, MUIA_Revision, &rev);

			MUI_DisposeObject(obj);

			if(ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through MUIA_Version/Revision\n", __LINE__, ver, rev);)
				return TRUE;
				}
			}

		// If we did't get the version we wanted, let's try to open the
		// libraries ourselves and see what happens...
		stccpy(libname, "PROGDIR:mui", sizeof(libname));
		AddPart(libname, name, sizeof(libname));

		if ((base = OpenLibrary(&libname[8], 0)) || (base = OpenLibrary(&libname[0], 0)))
			{
			UWORD OpenCnt = base->lib_OpenCnt;

			ver = base->lib_Version;
			rev = base->lib_Revision;

			CloseLibrary(base);

			// we add some additional check here so that eventual broken .mcc also have
			// a chance to pass this test (i.e. Toolbar.mcc is broken)
			if (ver > minver || (ver == minver && rev >= minrev))
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: v%ld.%ld found through OpenLibrary()\n", __LINE__, ver, rev);)
				return TRUE;
				}

			if (OpenCnt > 1)
				{
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
					(STRPTR) GetLocString(MSGID_STARTUP_MCC_IN_USE),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}

			// Attempt to flush the library if open count is 0 or because the
			// user wants to retry (meaning there's a chance that it's 0 now)

			if (flush)
				{
				struct Library *result;

				Forbid();
				if ((result = (struct Library *) FindName(&((struct ExecBase *)SysBase)->LibList, name)))
					RemLibrary(result);
				Permit();
				flush = FALSE;
				}
			else
				{
				d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: couldn`t find minimum required version.\n", __LINE__);)

				// We're out of luck - open count is 0, we've tried to flush
				// and still haven't got the version we want
				if (MUI_Request(NULL, NULL, 0L,
					(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
					(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
					(STRPTR) GetLocString(MSGID_STARTUP_OLD_MCC),
					name, minver, minrev, ver, rev))
					{
					flush = TRUE;
					}
				else
					break;
				}
			}
		else
			{
			// No MCC at all - no need to attempt flush
			flush = FALSE;
			if (!MUI_Request(NULL, NULL, 0L,
				(STRPTR) GetLocString(MSGID_STARTUP_FAILURE),
				(STRPTR) GetLocString(MSGID_STARTUP_RETRY_QUIT_GAD),
				(STRPTR) GetLocString(MSGID_STARTUP_MCC_NOT_FOUND),
				name, minver, minrev))
				{
				break;
				}
			}
		}

	return FALSE;
}

//----------------------------------------------------------------------------

