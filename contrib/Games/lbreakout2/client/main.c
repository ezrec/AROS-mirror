/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Don Sep  6 12:02:57 CEST 2001
    copyright            : (C) 2001 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#ifdef _WIN32
#include <fcntl.h>
#endif

#include "lbreakout.h"
#include "file.h"
#include "chart.h"
#include "config.h"
#include "shrapnells.h"
#include "event.h"
#include "levels.h"
#include "game.h"
#include "manager.h"
#include "editor.h"
#include "help.h"
#include "theme.h"
#include "comm.h"
#include "client.h"
#include "../gui/gui.h"

extern int stk_quit_request;
extern SDL_Surface *stk_display;
extern Config config;
extern char **levelset_names_home;
extern char **levelset_names_local;
extern StkFont *font;
extern char **theme_names;
extern int theme_count;
extern Net_Socket *client;
extern SDL_Surface *mbkgnd, *cr_bkgnd;

int main(int argc, char *argv[])
{
    int result = ACTION_NONE;
    int leave = 0;
    char *editor_file = 0;
    char path[512];
    
#ifdef _WIN32
    /* Get Windows to open files in binary mode instead of default text mode */
    _fmode = _O_BINARY;
#endif    
    
    /* lbreakout info */
    printf( "LBreakout2 %s\nCopyright 2001,2002 Michael Speck\nPublished under GNU GPL\n---\n", VERSION );
    printf( "Looking up data in: %s\n", SRC_DIR );
    printf( "Looking up own levels in: %s/%s/lbreakout2-levels\n", (getenv( "HOME" )?getenv( "HOME" ):"."), CONFIG_DIR_NAME );
#ifndef AUDIO_ENABLED
    printf( "Compiled without sound and music\n" );
#endif

    set_random_seed(); /* set random seed */

    config_load();

    stk_init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK );
    if ( config.fullscreen )
        stk_display_open( SDL_SWSURFACE | SDL_FULLSCREEN, 640, 480, 16 );
    else
        stk_display_open( SDL_SWSURFACE, 640, 480, 16 );
    stk_audio_open( 0,0,0,config.audio_buffer_size );
    SDL_WM_SetCaption( "LBreakout2", 0 );
    SDL_SetEventFilter( event_filter );
    stk_audio_enable_sound( config.sound );
    stk_audio_set_sound_volume( config.volume * 16 );
    
    /* load the GUI graphics from SRC_DIR/gui_theme */
    stk_surface_set_path( SRC_DIR );
    stk_audio_set_path( SRC_DIR );
    gui_init( "gui_theme" );

    stk_surface_set_path( SRC_DIR "/gfx" );
    stk_audio_set_path( SRC_DIR "/sounds" );
    
    /* load resources */
    /* for simplicity all functions are kept but anything
     * that is now themeable is loaded in
     * theme_load instead of the original function
     * (deleting resources works analouge)
     */
    theme_get_list();
    if ( config.theme_count != theme_count ) {
        if ( config.theme_id >= theme_count )
            config.theme_id = 0;
        config.theme_count = theme_count;
    }
    theme_load( theme_names[config.theme_id] );
    /* old functions still with initialzations of
     * lists or variables 
     */
    game_create();
    hint_load_res();
    chart_load();
    manager_create();
    client_create();
    exp_load();
    editor_create();
	help_create();
    /* run game */
    manager_fade( STK_FADE_IN );
    while( !leave && !stk_quit_request ) {
        result = manager_run();
        switch( result ) {
            case ACTION_QUIT: leave = 1; break;
            case ACTION_PLAY:
                manager_fade( STK_FADE_OUT );
                if ( game_init( levelset_names_local[config.levelset_id_local], config.diff, 
                                0, 0, 0, 0, 0, 0 ) ) 
                    game_run();
                game_clear();
                manager_fade( STK_FADE_IN );
                break;
            case ACTION_EDIT:
                /* new set? */
                if ( strequal( "<CREATE SET>", levelset_names_home[config.levelset_id_home] ) ) {
                    editor_file = calloc( 16, sizeof( char ) );
                    snprintf( path, sizeof(path)-1, "%s/%s/lbreakout2-levels", getenv( "HOME" )? getenv("HOME"):".", CONFIG_DIR_NAME );
                    if ( !enter_string( font, "Set Name:", editor_file, 12 ) || !file_check( path, editor_file, "w" ) ) {
                        free( editor_file );
                        break;
                    }
                    else
                        manager_update_set_list();
                }
                else
                    editor_file = strdup( levelset_names_home[config.levelset_id_home] );
                if ( editor_init( editor_file ) ) {
                    manager_fade( STK_FADE_OUT );
                    editor_run();
                    editor_clear();
                    manager_fade( STK_FADE_IN );
                }
                free( editor_file ); editor_file = 0;
                break;
			case ACTION_QUICK_HELP:
				help_run();
				break;
            case ACTION_CLIENT:
                manager_fade( STK_FADE_OUT );
                client_run();
                manager_fade( STK_FADE_IN );
                break;
            default: break;
        }
    }
    manager_fade( STK_FADE_OUT );
    /* delete stuff */
    help_delete();
	manager_delete();
	chart_save();
    chart_delete();
    editor_delete();
    exp_delete();
    game_delete();
    hint_delete_res();
    theme_delete();
    theme_delete_list();
    
    config_save();

    return EXIT_SUCCESS;
}
