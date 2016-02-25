/****************************************************************************
*                                                                           *
* gui.c -- Lunapaint,                                                       *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/

#include "gui.h"
#include "locale.h"

/*
    Program menu
*/
IPTR ProgramMenu;
Object *mainPulldownMenu;

/*
    - The MUI "Application" runs on *PaintApp
    - The canvases is managed in the canvases list
*/
Object *PaintApp;
struct WindowList *canvases;
Object *testWindow;
Object *AlertWindow;
Object *AlertText;
Object *AlertOk;
Object *LunaBackdrop;


void Init_Application ( )
{
   struct NewMenu MyMenu[] = {
        {NM_TITLE,    _(MSG_MENU_PROJECT)},
            {NM_ITEM, _(MSG_MENU_NEW),                "N",    2, 0L, (APTR)599 },
            {NM_ITEM, _(MSG_MENU_LOAD),               "O",    2, 0L, (APTR)597 },
            {NM_ITEM, _(MSG_MENU_LOAD_DT),            "L",    2, 0L, (APTR)594 },
            {NM_ITEM, _(MSG_MENU_SAVE),               "S",    2, 0L, (APTR)598 },
            {NM_ITEM, _(MSG_MENU_SAVE_AS),            "A",    2, 0L, (APTR)596 },
            {NM_ITEM, _(MSG_MENU_REVERT),             "R",    2, 0L, (APTR)595 },
            {NM_ITEM, _(MSG_MENU_IMPORT),             "I",    2, 0L, (APTR)600 },
            {NM_ITEM, _(MSG_MENU_EXPORT),             "E",    2, 0L, (APTR)601 },
            {NM_ITEM, _(MSG_MENU_INFO),               "I",    2, 0L, (APTR)604 },
            {NM_ITEM, NM_BARLABEL,                     0 ,    0, 0L, (APTR)0   },
            {NM_ITEM, _(MSG_MENU_ABOUT),             NULL,    2, 0L, (APTR)602 },
            {NM_ITEM, _(MSG_MENU_QUIT),               "Q",    2, 0L, (APTR)700 },
        {NM_TITLE,    _(MSG_MENU_EDIT)},
            {NM_ITEM, _(MSG_MENU_ZOOMIN),             "+",    2, 0L, (APTR)730 },
            {NM_ITEM, _(MSG_MENU_ZOOMOUT),            "-",    2, 0L, (APTR)731 },
            {NM_ITEM, _(MSG_MENU_SHOWALL),            "=",    2, 0L, (APTR)732 },
            {NM_ITEM, NM_BARLABEL,                     0 ,    0, 0L, (APTR)0   },
            {NM_ITEM, _(MSG_MENU_CP_SW),              "K",    2, 0L, (APTR)740 },
            {NM_ITEM, _(MSG_MENU_SWAP),               "J",    2, 0L, (APTR)741 },
            {NM_ITEM, _(MSG_MENU_FLIPV),             NULL,    2, 0L, (APTR)743 },
            {NM_ITEM, _(MSG_MENU_FLIPH),             NULL,    2, 0L, (APTR)744 },
            {NM_ITEM, _(MSG_MENU_CLEARL),             "Y",    2, 0L, (APTR)745 },
            {NM_ITEM, _(MSG_MENU_NXT_COLOR),          "X",    2, 0L, (APTR)746 },
            {NM_ITEM, _(MSG_MENU_PRV_COLOR),          "Z",    2, 0L, (APTR)747 },
            {NM_ITEM, _(MSG_MENU_PAL_ED),            NULL,    2, 0L, (APTR)750 },
            {NM_ITEM, NM_BARLABEL,                     0 ,    0, 0L, (APTR)0   },
            {NM_ITEM, _(MSG_MENU_LAYER_OFFSET),      NULL,    2, 0L, (APTR)742 },
            {NM_ITEM, _(MSG_MENU_RENDER_TEXT),       NULL,    2, 0L, (APTR)755 },
            {NM_ITEM, NM_BARLABEL,                     0 ,    0, 0L, (APTR)0   },
            {NM_ITEM, _(MSG_MENU_FLIPBV),            NULL,    2, 0L, (APTR)748 },
            {NM_ITEM, _(MSG_MENU_FLIPBH),            NULL,    2, 0L, (APTR)749 },
            {NM_ITEM, _(MSG_MENU_ROT_BRSH),          NULL,    2, 0L, (APTR)751 },
        {NM_TITLE,    _(MSG_MENU_TOOLS)},
            {NM_ITEM, _(MSG_MENU_DRAW),                       "D",    2, 0L, (APTR)850 },
            {NM_ITEM, _(MSG_MENU_LINE),                       "V",    2, 0L, (APTR)851 },
            {NM_ITEM, _(MSG_MENU_RECT),                       "G",    2, 0L, (APTR)853 },
            {NM_ITEM, _(MSG_MENU_CIRCLE),                     "C",    2, 0L, (APTR)855 },
            {NM_ITEM, _(MSG_MENU_GET_BRSH),                   "B",    2, 0L, (APTR)854 },
            {NM_ITEM, _(MSG_MENU_FILL),                       "F",    2, 0L, (APTR)852 },
            {NM_ITEM, _(MSG_MENU_COLOR_PICK),                 "H",    2, 0L, (APTR)856 },
        {NM_TITLE,    _(MSG_MENU_PAINTMODES)},
            {NM_ITEM, _(MSG_MENU_NORMAL),                     "1",    2, 0L, (APTR)800 },
            {NM_ITEM, _(MSG_MENU_COLOR),                      "2",    2, 0L, (APTR)808 },
            {NM_ITEM, _(MSG_MENU_COLORIZE),                   "3",    2, 0L, (APTR)801 },
            {NM_ITEM, _(MSG_MENU_LIGHTEN),                    "4",    2, 0L, (APTR)804 },
            {NM_ITEM, _(MSG_MENU_DARKEN),                     "5",    2, 0L, (APTR)805 },
            {NM_ITEM, _(MSG_MENU_BLUR),                       "6",    2, 0L, (APTR)806 },
            {NM_ITEM, _(MSG_MENU_SMUDGE),                     "7",    2, 0L, (APTR)803 },
            {NM_ITEM, _(MSG_MENU_ERASE),                      "8",    2, 0L, (APTR)802 },
            {NM_ITEM, _(MSG_MENU_UNERASE),                    "9",    2, 0L, (APTR)807 },
        {NM_TITLE,    _(MSG_MENU_ANIMATION)},
            {NM_ITEM, _(MSG_MENU_NXT_FR),                 ".",    2, 0L, (APTR)820 },
            {NM_ITEM, _(MSG_MENU_PRV_FR),                 ",",    2, 0L, (APTR)821 },
            {NM_ITEM, _(MSG_MENU_ONION),                  "'",    2, 0L, (APTR)825 },
            //{NM_ITEM, NM_BARLABEL,                         0 ,    0, 0L, (APTR)0   },
        {NM_TITLE,    _(MSG_MENU_WINDOWS)},
            {NM_ITEM, _(MSG_MENU_TOGGLE),                 NULL,   CHECKIT|MENUTOGGLE, 0L, (APTR)400 },
            {NM_ITEM, _(MSG_MENU_TG_LY),                  NULL,   CHECKIT|MENUTOGGLE, 0L, (APTR)401 },
            {NM_ITEM, _(MSG_MENU_TG_FULLSC),              NULL,   CHECKIT|MENUTOGGLE, 0L, (APTR)402 },
        {NM_TITLE,    _(MSG_MENU_FILTERS)},
            {NM_ITEM, _(MSG_MENU_SOON),                   NULL,   2, 0L, (APTR)999 },
        {NM_TITLE,    _(MSG_MENU_PREFS)},
            {NM_ITEM, _(MSG_MENU_PRG_PREF),                "P",   2, 0L, (APTR)900 },
        {NM_END}
    };
    mainPulldownMenu = MUI_MakeObject (
        MUIO_MenustripNM, ( IPTR )MyMenu, (IPTR)NULL
    );

    loadPreferences ( );

    InitLunaScreen ( );

    // Initialize the events listener
    InitEvents ( );

    // Let's initialize the used palette!
    initPalette ( );

    // Init toolbox and related windows
    Init_AboutWindow ( );
    Init_ToolboxWindow ( );
    Init_LayersWindow ( );
    Init_AnimationWindow ( );
    Init_PaletteWindow ( );
    Init_PrefsWindow ( );
    Init_AlertWindow ( );
    Init_TextToBrushWindow ( );
    Init_Backdrop ( );

    // Initialize the gui for making a new project
    nwNewWindow ( );
    // Initialize the export window
    makeExportWindow ( );
    // Initialize the import window
    makeImportWindow ( );

    PaintApp = ApplicationObject,
        MUIA_Application_Menustrip, (IPTR)mainPulldownMenu,
        MUIA_Application_Title, __(MSG_APPNAME),
        MUIA_Application_Base, (IPTR) "LUNAPAINT",
        MUIA_Application_Version, ( IPTR )VERSION,
        MUIA_Application_Author, ( IPTR )"Hogne Titlestad",
        MUIA_Application_Copyright, ( IPTR )"Hogne Titlestad",
        MUIA_Application_Description, __(MSG_APPDESC),
        MUIA_Application_SingleTask, TRUE,
        SubWindow, ( IPTR )toolbox,
        SubWindow, ( IPTR )WindowLayers,
        SubWindow, ( IPTR )WindowAnimation,
        SubWindow, ( IPTR )paletteWindow,
        SubWindow, ( IPTR )nwWindow,
        SubWindow, ( IPTR )aboutWindow,
        SubWindow, ( IPTR )offsetWindow,
        SubWindow, ( IPTR )exportWindow,
        SubWindow, ( IPTR )importWindow,
        SubWindow, ( IPTR )PrefsWindow,
        SubWindow, ( IPTR )AlertWindow,
        SubWindow, ( IPTR )textToBrushWindow,
        SubWindow, ( IPTR )LunaBackdrop,
    End;

    if ( PaintApp == NULL )
    {
        printf ( "Couldn't generate application object..\n" );
        Exit_Application ( );
        return;
    }

    Init_ToolboxMethods ( );
    Init_PaletteMethods ( );
    Init_NewProjectMethods ( );
    Init_TextToBrushMethods ( );

    // Add the first window so we have something to use
    canvases = NULL; // tell init code that canvases is uninitialized
    globalActiveCanvas = NULL;
    globalActiveWindow = NULL;
    globalWindowIncrement = 1;
    globalCurrentTool = LUNA_TOOL_BRUSH;
    globalColorMode = LUNA_COLORMODE_64BIT;
    brushTool.paintmode = LUNA_PAINTMODE_NORMAL;
    brushTool.RecordContour = FALSE;
    brushTool.ContourBuffer = NULL;
    brushTool.opacitymode = LUNA_OPACITYMODE_ADD;
    globalBrushMode = 0; // normal
    MouseHasMoved = FALSE;
    redrawTimes = 0;
    fullscreenEditing = FALSE;

    // Open the toolbox and declare the app running!
    if ( GlobalPrefs.ScreenmodeType != 0 )
        set ( LunaBackdrop, MUIA_Window_Open, TRUE );
    set ( toolbox, MUIA_Window_Open, TRUE );
    setToolbarActive ( );
    makeToolBrush ( );
    GetDrawFillState ( );
}

void Exit_Application ( )
{
    // Delete canvases (free allocated memory)
    deleteCanvaswindows ( canvases );

    if ( canvases != NULL )
        FreeVec ( canvases );

    // Free up layers window buffers etc
    Exit_LayersWindow ( );

    LockPubScreen ( ( STRPTR )"Lunapaint" );

    // Dispose of the paint app
    if ( PaintApp != NULL )
        MUI_DisposeObject ( PaintApp );

    UnlockPubScreen ( ( STRPTR )"Lunapaint", lunaPubScreen );

    if ( lunaPubScreen != NULL )
        CloseScreen ( lunaPubScreen );

    // Shutdown events
    ShutdownEvents ( );


    // Free all tool buffers
    if ( brushTool.buffer != NULL )
        FreeVec ( brushTool.buffer );
    if ( brushTool.ContourBuffer != NULL )
        freeValueList ( &brushTool.ContourBuffer );
    if ( lineTool.buffer != NULL )
        FreeVec ( lineTool.buffer );
    if ( circleTool.buffer != NULL )
        FreeVec ( circleTool.buffer );
    if ( rectangleTool.buffer != NULL )
        FreeVec ( rectangleTool.buffer );
    if ( clipbrushTool.buffer != NULL )
        FreeVec ( clipbrushTool.buffer );

    // Free up palette
    if ( globalPalette != NULL )
        FreeVec ( globalPalette );

    // Final cleanup with toolbox related things
    Exit_ToolboxWindow ( );

    // Clean up fonts etc
    Exit_TextToBrushWindow ( );
}

int checkSignalBreak ( ULONG *sigs )
{
    if ( *sigs )
    {
        *sigs = Wait ( *sigs | SIGBREAKF_CTRL_C );
        if ( *sigs & SIGBREAKF_CTRL_C )
            return 1;
    }
    return 0;
}

int getSignals ( ULONG *sigs )
{
    LONG result = DoMethod ( PaintApp, MUIM_Application_NewInput, ( IPTR )sigs );

    if ( result	!= MUIV_Application_ReturnID_Quit )
    {
        return 1;
    }
    return 0;
}

void InitLunaScreen ( )
{
    if ( lunaPubScreen != NULL )
    {
        UnlockPubScreen ( ( STRPTR )"Lunapaint", lunaPubScreen );
        CloseScreen ( lunaPubScreen );
    }

    // Open a clone screen
    if ( GlobalPrefs.ScreenmodeType == 1 )
    {
        lunaPubScreen = OpenScreenTags (
            lunaScreen,
            SA_Title, ( IPTR )VERSION,
            SA_PubName, ( IPTR )"Lunapaint",
            SA_ShowTitle, TRUE,
            SA_SysFont, 1,
            SA_SharePens, TRUE,
            SA_LikeWorkbench, TRUE,
            TAG_DONE
        );
        PubScreenStatus( lunaPubScreen, 0 );
    }
    // Open a specified screen
    else if ( 0 )
    {
        /*
        ULONG idok;
        idok = BestCModeIDTags (
            CYBRBIDTG_NominalWidth, scrWidth,
            CYBRBIDTG_NominalHeight, scrHeight,
            CYBRBIDTG_Depth, scrDepth,
            TAG_DONE
        );
        lunaPubScreen = OpenScreenTags (
            lunaScreen,
            SA_Width, scrWidth,
            SA_Height, scrHeight,
            SA_Depth, scrDepth,
            SA_DisplayID, idok,
            SA_Title, ( IPTR )LUNA_SCREEN_TITLE,
            SA_PubName, ( IPTR )"Lunapaint",
            SA_ShowTitle, TRUE,
            SA_SysFont, 1,
            SA_SharePens, TRUE,
            SA_LikeWorkbench, TRUE,
            TAG_DONE
        );
        PubScreenStatus( lunaPubScreen, 0 );
        */
    }
    // Open on Workbench/Wanderer/Ambient whatever
    else if ( GlobalPrefs.ScreenmodeType == 0 )
    {
        lunaPubScreen = LockPubScreen ( ( UBYTE *)"Workbench" );
    }

}

void HideOpenWindows ( )
{
    set ( toolbox, MUIA_Window_Open, FALSE );
    set ( WindowLayers, MUIA_Window_Open, FALSE );
    set ( WindowAnimation, MUIA_Window_Open, FALSE );
    set ( paletteWindow, MUIA_Window_Open, FALSE );
    set ( nwWindow, MUIA_Window_Open, FALSE );
    set ( aboutWindow, MUIA_Window_Open, FALSE );
    set ( offsetWindow, MUIA_Window_Open, FALSE );
    set ( exportWindow, MUIA_Window_Open, FALSE );
    set ( importWindow, MUIA_Window_Open, FALSE );
    set ( PrefsWindow, MUIA_Window_Open, FALSE );
    set ( AlertWindow, MUIA_Window_Open, FALSE );
    set ( LunaBackdrop, MUIA_Window_Open, FALSE );
    set ( textToBrushWindow, MUIA_Window_Open, FALSE );

    /*WindowList *lst = canvases;

    while ( lst != NULL )
    {
        set ( lst->win, MUIA_Window_Open, FALSE );
        lst = lst->nextwin;
    }*/

}

void ReopenWindows ( )
{
    set ( toolbox, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( WindowLayers, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( WindowAnimation, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( paletteWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( nwWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( aboutWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( offsetWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( exportWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( importWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( PrefsWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( AlertWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( LunaBackdrop, MUIA_Window_Screen, ( IPTR )lunaPubScreen );
    set ( textToBrushWindow, MUIA_Window_Screen, ( IPTR )lunaPubScreen );

    if ( GlobalPrefs.ScreenmodeType > 0 )
        set ( LunaBackdrop, MUIA_Window_Open, TRUE );
    set ( toolbox, MUIA_Window_Open, TRUE );
}

void Init_Backdrop ( )
{
    ULONG window_Height = lunaPubScreen->Height;
    ULONG window_Delta = lunaPubScreen->BarHeight;

    LunaBackdrop = WindowObject,
        MUIA_Window_Title, ( IPTR )NULL,
        MUIA_Window_ScreenTitle, ( IPTR )VERSION,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_Backdrop, TRUE,
        MUIA_Window_Borderless, TRUE,
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_SizeGadget, FALSE,
        MUIA_Window_DepthGadget, FALSE,
        MUIA_Window_DragBar, FALSE,
        MUIA_Window_Width, MUIV_Window_Width_Screen ( 100 ),
        MUIA_Window_Height, window_Height - window_Delta,
        MUIA_Window_TopEdge, window_Delta,
        MUIA_Window_LeftEdge, 0,
        MUIA_Window_UseBottomBorderScroller,        FALSE,
        MUIA_Window_UseRightBorderScroller,         FALSE,
        WindowContents, ( IPTR )VGroup,
            MUIA_Background, ( IPTR )"5:PROGDIR:data/backdrop.png",
            Child, RectangleObject, End,
        End,
    End;
}

void Init_AlertWindow ( )
{
    AlertWindow = WindowObject,
        MUIA_Window_Title, __(MSG_WIN_ALERT),
        MUIA_Window_ScreenTitle, __(MSG_SCR_ALERT),
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','W'),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )( AlertText = TextObject,
                MUIA_Text_Contents, ( IPTR )"",
            End ),
            Child, ( IPTR )( AlertOk = SimpleButton ( _(MSG_BUTTON_OK) ) ),
        End,
    End;
    DoMethod (
        AlertWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        AlertWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        AlertOk, MUIM_Notify, MUIA_Pressed, FALSE,
        AlertWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
}

void ShowAlert ( unsigned char *text )
{
    set ( AlertText, MUIA_Text_Contents, ( IPTR )text );
    set ( AlertWindow, MUIA_Window_Open, TRUE );
}
