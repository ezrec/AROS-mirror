/****************************************************************************
*                                                                           *
* preferences.c -- Lunapaint,                                               *
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

#include  <SDI_hook.h>
#include "preferences.h"

struct Hook PrefsHandler_hook;
struct LunapaintPrefs GlobalPrefs;

Object *PrefsWindow;
Object *PrefsScrnMdType;
Object *PrefsLayBackMode;
Object *PrefsBtnUse;
Object *PrefsBtnSave;
Object *PrefsBtnCancel;
Object *PrefsCycScrType;

static STRPTR titles[] = { NULL, NULL, NULL };
static STRPTR scrntypes[] = { NULL, NULL, NULL };
static STRPTR layerbacktypes[] = { NULL, NULL, NULL, NULL, NULL };


HOOKPROTONHNO(PrefsHandler_func, void, int *param)
{
    set ( PrefsWindow, MUIA_Window_Open, FALSE );

    BOOL save = *( BOOL *)param;

    int screenchoice = XGET ( PrefsCycScrType, MUIA_Cycle_Active );
    if ( screenchoice != GlobalPrefs.ScreenmodeType )
    {
        GlobalPrefs.ScreenmodeType = screenchoice;
        set ( PrefsCycScrType, MUIA_Cycle_Active, ( IPTR )GlobalPrefs.ScreenmodeType );
    }
    // Open the screen again and move windows
    if ( canvases != NULL )
    {
        ShowAlert ( _(MSG_PREFS_ALERT) );
    }
    else
    {
        HideOpenWindows ( );
        InitLunaScreen ( );
        ReopenWindows ( );
    }

    // View
    GlobalPrefs.LayerBackgroundMode = XGET ( PrefsLayBackMode, MUIA_Cycle_Active );

    if ( save == 1 )
        savePreferences ( );
}


void Init_PrefsWindow ( )
{
    titles[0] = _(MSG_PREFS_GENERAL);
    titles[1] = _(MSG_PREFS_VIEW);
    scrntypes[0] = _(MSG_PREFS_WAND_USE);
    scrntypes[1] = _(MSG_PREFS_WAND_CLONE);
    layerbacktypes[0] = _(MSG_PREFS_CHECKERS);
    layerbacktypes[1] = _(MSG_PREFS_BLACK);
    layerbacktypes[2] = _(MSG_PREFS_GRAY);
    layerbacktypes[3] = _(MSG_PREFS_WHITE);
    // Init the window itself
    PrefsWindow = WindowObject,
        MUIA_Window_Title, __(MSG_PREFS_WIN),
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('L','P','P','R'),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )RegisterObject,
                MUIA_Register_Titles, titles,
                Child, ( IPTR )GroupObject,
                    MUIA_FrameTitle, __(MSG_PREFS_WIN_SCR),
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_CycleChain, 1,
                    Child, ( IPTR )( PrefsCycScrType = CycleObject,
                        MUIA_Cycle_Entries, scrntypes,
                        MUIA_Cycle_Active, 0,
                        MUIA_CycleChain, 1,
                    End ),
                End,
                Child, ( IPTR )GroupObject,
                    MUIA_FrameTitle, __(MSG_PREFS_LAYERS),
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_CycleChain, 1,
                    Child, ( IPTR )( PrefsLayBackMode = CycleObject,
                        MUIA_Cycle_Entries, layerbacktypes,
                        MUIA_Cycle_Active, 0,
                        MUIA_CycleChain, 1,
                    End ),
                End,
            End,
            Child, ( IPTR )HGroup,
                Child, ( IPTR )( PrefsBtnSave = SimpleButton ( _(MSG_PREFS_SAVE) ) ),
                Child, ( IPTR )( PrefsBtnUse = SimpleButton ( _(MSG_PREFS_USE) ) ),
                Child, ( IPTR )( PrefsBtnCancel = SimpleButton ( _(MSG_PREFS_CANCEL) ) ),
            End,
        End,
    End;

    MakeStaticHook(PrefsHandler_hook, &PrefsHandler_func);

    // Setting the prefs visible in the gui
    set ( PrefsCycScrType, MUIA_Cycle_Active, ( IPTR )GlobalPrefs.ScreenmodeType );
    set ( PrefsLayBackMode, MUIA_Cycle_Active, ( IPTR )GlobalPrefs.LayerBackgroundMode );

    DoMethod (
        PrefsWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        PrefsWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        PrefsBtnCancel, MUIM_Notify, MUIA_Pressed, FALSE,
        PrefsWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        PrefsBtnUse, MUIM_Notify, MUIA_Pressed, FALSE,
        PrefsBtnUse, 3, MUIM_CallHook, &PrefsHandler_hook, FALSE
    );
    DoMethod (
        PrefsBtnSave, MUIM_Notify, MUIA_Pressed, FALSE,
        PrefsBtnSave, 3, MUIM_CallHook, &PrefsHandler_hook, TRUE
    );

}

BOOL savePreferences ( )
{
    struct PrefHeader head = { 0 };
    struct IFFHandle *handle;
    BOOL result = FALSE;
    BPTR filehandle = NULL;

    if ( ( filehandle = Open ( "PROGDIR:lunapaint.prefs", MODE_NEWFILE ) ) == NULL )
        return FALSE;

    if ( !( handle = AllocIFF() ) )
    {
        goto prefs_save_ending;
    }

    handle->iff_Stream = ( IPTR )filehandle;

    InitIFFasDOS( handle );

    if ( OpenIFF( handle, IFFF_WRITE ) )
    {
        goto prefs_save_ending;
    }

    if ( PushChunk( handle, ID_PREF, ID_FORM, IFFSIZE_UNKNOWN ) )
    {
        goto prefs_save_ending;
    }

    head.ph_Version = ( IPTR )"0.1";
    head.ph_Type = 0;

    if ( PushChunk( handle, ID_PREF, ID_PRHD, IFFSIZE_UNKNOWN ) )
    {
        goto prefs_save_ending;
    }

    if ( !WriteChunkBytes( handle, &head, sizeof( struct PrefHeader ) ) )
    {
        goto prefs_save_ending;
    }

    if ( PopChunk( handle ) )
    {
        goto prefs_save_ending;
    }

    if ( PushChunk( handle, ID_PREF, ID_LUNAPAINT, sizeof( struct LunapaintPrefs ) ) )
    {
        goto prefs_save_ending;
    }

    // Save prefs struct
    if ( !WriteChunkBytes( handle, &GlobalPrefs, sizeof( struct LunapaintPrefs ) ) )
    {
        goto prefs_save_ending;
    }
    if ( PopChunk( handle ) )
    {
        goto prefs_save_ending;
    }

    // End saving
    if ( PopChunk(handle) )
    {
        goto prefs_save_ending;
    }

    result = TRUE;

    prefs_save_ending:

    if ( handle != NULL ) CloseIFF( handle );
    if ( filehandle != NULL ) Close ( filehandle );
    if ( handle != NULL ) FreeIFF( handle );

    return result;
}

BOOL loadPreferences ( )
{
    struct ContextNode *context;
    struct IFFHandle *handle;
    BOOL result = FALSE;
    BPTR filehandle = NULL;


    if ( !( handle = AllocIFF( ) ) )
        return FALSE;

    if ( ( filehandle = Open ( "PROGDIR:lunapaint.prefs", MODE_OLDFILE ) ) == NULL )
        goto prefs_load_ending;

    handle->iff_Stream = ( IPTR )filehandle;
    InitIFFasDOS( handle );

    if ( OpenIFF( handle, IFFF_READ ) )
        goto prefs_load_ending;

    if ( StopChunk( handle, ID_PREF, ID_LUNAPAINT ) )
        goto prefs_load_ending;

    if ( ParseIFF( handle, IFFPARSE_SCAN ) )
        goto prefs_load_ending;

    context = CurrentChunk( handle );

    if ( ReadChunkBytes ( handle, &GlobalPrefs, sizeof( struct LunapaintPrefs ) ) )
        goto prefs_load_ending;

    result = TRUE;

    prefs_load_ending:

    if ( handle != NULL ) CloseIFF(handle);
    if ( filehandle != NULL ) Close (filehandle);
    if ( handle != NULL ) FreeIFF(handle);

    return result;
}
