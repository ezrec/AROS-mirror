/***************************************************************************
                          manager.c  -  description
                             -------------------
    begin                : Thu Sep 20 2001
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

#include "manager.h"
#include "chart.h"
#include "event.h"
#include "config.h"
#include "../gui/stk.h"
#include "levels.h"
#include "theme.h"

extern SDL_Surface *stk_display;
extern Config config;
extern List *menus; /* list of menus */
int menu_x = 54, menu_y = 220, menu_w = 256, menu_h = 256, menu_border = 16; /* default region where menu is displayed centered */
int cx = 384, cy = 220, cw = 256, ch = 200; /* center charts here */
Menu *cur_menu; /* current menu */
extern SDL_Surface *mbkgnd; /* menu background */
extern StkFont *mfont, *mhfont, *mcfont; /* font, highlight font, caption font */
int gap_height = 2;
extern char **levelset_names_home; /* home directory sets */
extern int levelset_count_home;
extern char **levelset_names_local; /* non-network levelsets */
extern int levelset_count_local;
extern int stk_quit_request;
#ifdef AUDIO_ENABLED
extern StkSound *wav_menu_click;
extern StkSound *wav_menu_motion;
#endif
/* some items we need to know to assign name lists later */
Item *item_levelset, *item_set, *item_theme;
extern List *charts;
int chart_id = 0; /* current chart displayed */
/* theme name list */
extern char **theme_names;
extern int theme_count;
extern char tournament_name[];

/* hints */
#define HINT_ 0
#define HINT_QUIT "There's nasty work out there waiting for you... better stay here!"
#define HINT_ANIM "If you turn animations low there'll be less details which will speed up the game a great deal."
#define HINT_TRP "Enable/disable transparency of bonuses and animations."
#define HINT_SHADOW "If you disable shadows it will speed up the game but won't look that good. ;-)"
#define HINT_BONUS_INFO "If this is enabled you'll see the time left for all bonuses and maluses at the right side of the screen."
#define HINT_DISPLAY "You may run LBreakout2 either fullscreen or windowed. This switch may be done anywhere by pressing 'f'."
#define HINT_FPS "You you don't want LBreakout2 to consume all of your CPU (best performance) limit the frame rate."
#define HINT_CONVEX "As long as this option is enabled the paddle is treated as convex allowing easy aiming. If you "\
                    "disable this option the real physics will show up and you'll have to influence the ball by paddle movement "\
                    "and the hemispherical ends. Just for cracks."
#define HINT_BALLLEVEL "If you can't see the ball because of thousands of bonuses coming down set this option to 'Above Bonuses'"
#define HINT_DEBRISLEVEL "If you can't see the ball because of all the debris coming down set this option to 'Below Balls'"
#define HINT_USEHINTS "Enable/disable these quick hints. See the manual for more information about how to use LBreakout2!"
#define HINT_KEYSPEED "If you use the keyboard as input you may here adjust the paddle speed to your needs."
#define HINT_PLAYERS "These are the number of players and their names. If one player looses a ball or restarts the level it's the next player's turn."
#define HINT_DIFF "EASY:#Lives: 6 (max. 10)#Paddle Size: 72 (max. 180)#Ball Speed: 240-400 pixels/sec#Bonus: -20%##"\
                  "MEDIUM:#Lives: 5 (max. 7)#Paddle Size: 72 (max. 144)#Ball Speed: 270-430 pixels/sec##"\
                  "HARD:#Lives: 4 (max. 5)#Paddle Size: 54 (max. 108)#Ball Speed: 300-450 pixels/sec#Bonus: +30%"
#define HINT_STARTGAME "Let's get it on!"
#define HINT_EDITOR_SET "This is the list of the levelsets found in ~/.lgames/lbreakout2-levels which you may edit. If you "\
                        "choose <CREATE SET> and click on 'Edit Set' below you'll be asked to enter a levelsetname and the new levelset " \
                        "will be saved in a file by this name."
#define HINT_EDITOR_EDIT "Click here to edit the levelset chosen above."
#define HINT_EDITOR_DELETE "Click here to delete the chosen levelset (after confirmation)."
#define HINT_KEYS "If you use the keyboard for input these are the keys for moving the paddle (Turbo will double the speed while pressed.), firing balls/shots and returning the ball to paddle after a timeout of ten seconds.#If you press the WARP key you'll proceed to the next level (after confirmation) but only after you cleared the required percentage of bricks. This percentage can be modified in 'Advanced Options'."
#define HINT_GRAB "Keeps the mouse in window. This option is only needed when running in windowed mode with relative mouse motion disabled."
#define HINT_RELMOTION "Turning this on allows you to invert the mouse motion, modify the mouse speed and use keyboard for fine-tuning."
#define HINT_MOD "This is the percentual motion modifier. The higher the faster the mouse moves."
#define HINT_INVERT "If for some reason you need an inversion. You may turn this on here."
#define HINT_RETURN "If a ball does not damage a brick within ten seconds it is allowed to return to the paddle. If this option is set "\
                    "to 'Auto' it will automatically return and if this option is set to 'By Click' you'll have to press the middle mouse"\
                    "button or the 'back'-key (default is Backspace)."
#define HINT_APPLY "Click here to apply the theme you selected above. Please check the manual at www.lgames.org for information on how to " \
                   "create a theme."
#define HINT_HOST "IP address of the game server as xxx.xxx.xxx.xxx.#DO NOT use 'localhost' if you run the server on your computer. You won't be able to challenge others. Use your IP queried by /sbin/ifconfig instead."
#define HINT_PORT "Port the game server is bound to."
#define HINT_USERNAME "If you're successfully connected to the game server this is your nickname there."
#define HINT_CONNECT "Connect to game server."
#define HINT_LOCAL "A local game with up to 4 alternating players."
#define HINT_NETWORK "A two-player or deathmatch game via Internet or LAN."

char hint_levelset[400]; /* information about version and author of selected levelset */
#define HINT_LEVELSET_APPENDIX "This is the levelset you actually want to play. Note, that each levelset got it's own highscore chart (left/right-click on the chart on the right)." \
                      " A levelset is either load from ~/.lgames/lbreakout2-levels or the installation directory."
char hint_theme[512]; /* theme information */
#define HINT_THEME_APPENDIX "This is the list of available themes. Choose any theme and click 'Apply' to select it"
#define HINT_WARP_LIMIT "After you destroyed this percentage of bricks you MAY warp to the next level by pressing 'w'. That is indicated by a small icon at the bottom right-hand side of the screen.#If you set this option to 0 you may instantly warp whereas 100 means you have to destroy all bricks."
#define HINT_THEME "In this menu you can change the appearance of LBreakout. Included to the release are the three winning themes from the Linux Game Tome contest 'absoluteB','Oz' and 'Moiree' along with the old graphics as theme 'Classic'.#For more themes check out http://lgames.sf.net."
#define HINT_CORNER "Linear:#++ There is no chance for balls to get infinitely stuck in a brick bounce combo.#"\
    "-- It is quite hard for balls to enter narrow passages.##Spherical:#"\
    "++ Feels better and balls can easily pass through small gaps.#"\
    "-- There is a chance for balls to get infinitely stuck within a series of indestructible "\
    "brick reflections if the level is too wrinkled. However, with the auto return option this "\
    "is no problem so 'Spherical' is the default."

/*
====================================================================
Callbacks of menu items.
====================================================================
*/
/* Disable/enable sound */
void cb_sound() {
#ifdef AUDIO_ENABLED
    stk_audio_enable_sound( config.sound );
#endif
}
/* set volume */
void cb_volume() {
#ifdef AUDIO_ENABLED
    stk_audio_set_sound_volume( config.volume * 16 );
#endif
}
/* toggle fullscreen */
void cb_fullscreen() {
    manager_show();
}
/* delete set */
void cb_delete_set()
{
    char fname[512];
    /* do not delete <CREATE SET> file */
    if ( strequal( "<CREATE SET>", levelset_names_home[config.levelset_id_home] ) ) {
        printf( "You cannot delete '<CREATE SET>'!\n" );
        return;
    }
    /* get file name + path */
    snprintf( fname, sizeof(fname)-1,"%s/%s/lbreakout2-levels/%s", getenv( "HOME" ), CONFIG_DIR_NAME, levelset_names_home[config.levelset_id_home] );
    remove( fname );
    levelsets_load_names(); /* reinit name lists and configs indices */
    /* reassign these name lists as position in memory has changed */
    value_set_new_names( item_levelset->value, levelset_names_local, levelset_count_local );
    value_set_new_names( item_set->value, levelset_names_home, levelset_count_home );
}
/* adjust set list */
void cb_adjust_set_list()
{
    /* reinit name lists and configs indices */
    levelsets_load_names();
    /* reassign these name lists as position in memory has changed */
    value_set_new_names( item_levelset->value, levelset_names_local, levelset_count_local );
    value_set_new_names( item_set->value, levelset_names_home, levelset_count_home );
}
/* set key speed from i_key_speed */
void cb_set_keyspeed()
{
    config.key_speed = (float)config.i_key_speed / 100;
}
/* if hints where disabled hide actual hint */
void cb_hints()
{
    if ( !config.use_hints )
        hint_set( 0 );
}
/* change theme */
void cb_change_theme()
{
    Menu *menu;
    theme_load( theme_names[config.theme_id] );
    hint_set_bkgnd( mbkgnd );
    /* apply the new background to all items */
    list_reset( menus );
    while ( ( menu = list_next( menus ) ) ) {
        menu_set_bkgnd( menu, mbkgnd );
        menu_set_fonts( menu, mcfont, mfont, mhfont );
    }
    stk_surface_blit( mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
}
/* update hint about selected levelset */
void cb_update_levelset_hint()
{
    Set_Chart *chart;
    FILE *file;
    int version, update, i;
    char author[32];
    if ( STRCMP( levelset_names_local[config.levelset_id_local], tournament_name ) ) {
        sprintf( hint_levelset, "Play *ALL* available levels in a random order." );
        hint_set_contents( item_levelset->hint, hint_levelset );
        hint_set( item_levelset->hint ); /* redraw the hint */
    }
    else
        if ( ( file = levelset_open( levelset_names_local[config.levelset_id_local], "rb" ) ) ) {
            levelset_get_version( file, &version, &update );
            levelset_get_first_author( file, author );
            sprintf( hint_levelset, "%s v%i.%02i#Author: %s##%s", 
                     levelset_names_local[config.levelset_id_local],
                     version, update, author, HINT_LEVELSET_APPENDIX );
            hint_set_contents( item_levelset->hint, hint_levelset );
            hint_set( item_levelset->hint ); /* redraw the hint */
            fclose( file );
        }
    /* select chart */
    for ( i = 0; i < charts->count; i++ ) {
        chart = chart_set_query_id( i );
        if ( STRCMP( chart->name, levelset_names_local[config.levelset_id_local] ) ) {
            chart_id = i;
            stk_surface_blit( 
                mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
            chart_show( chart_set_query_id( chart_id ),
                cx, cy, cw, ch );
        }
    }
}
/* update hint of theme by feeding it with the ABOUT file */
void cb_update_theme_hint()
{
    theme_get_info( theme_names[config.theme_id], hint_theme, 255 );
    strcat( hint_theme, "##" ); strcat( hint_theme, HINT_THEME_APPENDIX );
    hint_set_contents( item_theme->hint, hint_theme );
    hint_set( item_theme->hint ); /* redraw the hint */
} 

/*
====================================================================
Load/delete background and create and link all menus
====================================================================
*/
void manager_create()
{
    int i, j;
    Item *keys[7];
    Item *item;
    int filter[SDLK_LAST]; /* key filter */
    /* constant contence of switches */
    char *str_fps[] = { "No Limit", "100 FPS" };
    char *str_anim[] = { "Off", "Low", "Medium", "High" };
    char *str_diff[] = { "Easy", "Medium", "Hard" };
    /*
    main:
        new game:
            start game
			quick help
            ---
            levelset
            difficulty
            ---
            active players
            ---
            player1
            player2
            player3
            player4
        controls
        graphics
            animations
            transparency
            shadow
            ---
            display
            constant frame rate
        audio:
            sound
            volume
        advanced options:
            convex paddle
			ball level
            key speed
        editor:
            set: (list)
            edit
            delete
                yes
                no
            ---
            create empty set
                set name: (string)
                create set
                    yes
                    no
        ---
        quit
    */
    Menu *_main = 0;
#ifdef _1
    Menu *options = 0;
#endif
#ifdef AUDIO_ENABLED
    Menu *audio = 0;
#endif
    Menu *gfx = 0;
    Menu *game = 0;
    Menu *cont = 0;
    Menu *adv = 0;
    Menu *editor = 0;
    Menu *confirm_delete = 0;
    Menu *theme = 0;

    /* hints will be displayed on menu background */
    hint_set_bkgnd( mbkgnd );
    
    /* setup filter */
    filter_clear( filter );
    filter_set( filter, SDLK_a, SDLK_z, 1 );
    filter[SDLK_BACKSPACE] = 1;
    filter[SDLK_SPACE] = 1;
    filter[SDLK_RSHIFT] = 1;
    filter[SDLK_LSHIFT] = 1;
    filter[SDLK_LALT] = 1;
    filter[SDLK_RALT] = 1;
    filter[SDLK_LCTRL] = 1;
    filter[SDLK_RCTRL] = 1;
    filter[SDLK_UP] = 1;
    filter[SDLK_DOWN] = 1;
    filter[SDLK_LEFT] = 1;
    filter[SDLK_RIGHT] = 1;
    filter[SDLK_q] = 0;
    filter[SDLK_p] = 0;
    filter[SDLK_f] = 0;
    filter[SDLK_s] = 0;
    filter[SDLK_a] = 0;
    filter[SDLK_r] = 0;
    filter[SDLK_h] = 0;
    filter[SDLK_c] = 0;

    /* menus are added to this list for deleting later */
    menus = list_create( LIST_AUTO_DELETE, menu_delete );
    /* create menus */
    _main   = menu_create( "Menu", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
#ifdef _1
    options = menu_create( "Options", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
#endif
    gfx     = menu_create( "Graphics", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    game    = menu_create( "Local Game", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    cont    = menu_create( "Controls", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    adv     = menu_create( "Advanced Options", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
#ifdef AUDIO_ENABLED
    audio = menu_create( "Audio", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
#endif
    editor  = menu_create( "Editor", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    confirm_delete = menu_create( "Delete Set", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    theme = menu_create( "Select Theme", MENU_LAYOUT_CENTERED, menu_x, menu_y, menu_w, menu_h, menu_border, 1 );
    /* create items */
    /* main menu */
    menu_add( _main, item_create_link    ( "Local Game", HINT_LOCAL, game ) );
#ifdef NETWORK_ENABLED
    menu_add( _main, item_create_action  ( "Network Game", HINT_NETWORK, ACTION_CLIENT ) );
#endif
    menu_add( _main, item_create_action  ( "Quick Help", HINT_, ACTION_QUICK_HELP ) );
//    menu_add( _main, item_create_link    ( "Select Theme", HINT_THEME, theme ) );
    menu_add( _main, item_create_separator  ( "" ) );
#ifdef _1
    menu_add( _main, item_create_link       ( "Options", options ) );
#endif
    menu_add( _main, item_create_link       ( "Controls", HINT_, cont ) );
    menu_add( _main, item_create_link       ( "Graphics", HINT_, gfx ) );
#ifdef AUDIO_ENABLED
    menu_add( _main, item_create_link       ( "Audio",  HINT_,audio ) );
#else
    menu_add( _main, item_create_separator  ( "Audio" ) );
#endif
    menu_add( _main, item_create_link       ( "Advanced Options", HINT_, adv ) );
    menu_add( _main, item_create_separator  ( "" ) );
    menu_add( _main, item_create_link       ( "Editor", HINT_, editor ) );
    menu_add( _main, item_create_separator  ( "" ) );
    menu_add( _main, item_create_action     ( "Quit", HINT_QUIT, ACTION_QUIT ) );
#ifdef _1
    /* options */
    menu_add( options, item_create_link( "Controls", HINT_, cont ) );
    menu_add( options, item_create_link( "Graphics", HINT_, gfx ) );
    menu_add( options, item_create_link( "Audio", HINT_, audio ) );
    menu_add( options, item_create_separator( "Audio" ) );
    menu_add( options, item_create_separator( "" ) );
    menu_add( options, item_create_link( "Back", _main ) );
#endif
    /* audio */
#ifdef AUDIO_ENABLED
    item = item_create_switch( "Sound:", HINT_, &config.sound, "Off", "On" );
    item->callback = cb_sound;
    menu_add( audio, item );
    menu_add( audio, item_create_switch( "Speech:", HINT_, &config.speech, "Off", "On" ) );
    item = item_create_range( "Volume:", HINT_, &config.volume, 1, 8, 1 );
    item->callback = cb_volume;
    menu_add( audio, item );
    menu_add( audio, item_create_separator( "" ) );
    menu_add( audio, item_create_link( "Back", HINT_, _main ) );
#endif
    /* gfx */
    menu_add( gfx, item_create_switch_x( "Animations:", HINT_ANIM, &config.anim, str_anim, 4 ) );
    menu_add( gfx, item_create_switch( "Bonus Info:", HINT_BONUS_INFO, &config.bonus_info, "Off", "On" ) );
    menu_add( gfx, item_create_separator( "" ) );
    item = item_create_switch( "Display:", HINT_DISPLAY, &config.fullscreen, "Window", "Fullscreen" );
    item->callback = cb_fullscreen;
    menu_add( gfx, item );
    menu_add( gfx, item_create_switch_x( "Frame Rate:", HINT_FPS, &config.fps, str_fps, 2 ) );
    menu_add( gfx, item_create_separator( "" ) );
    menu_add( gfx, item_create_link( "Select Theme", HINT_THEME, theme ) );
    menu_add( gfx, item_create_separator( "" ) );
    menu_add( gfx, item_create_link( "Back", HINT_, _main ) );
    /* game */
    menu_add( game, item_create_action( "Start Game", HINT_STARTGAME, ACTION_PLAY ) );
    menu_add( game, item_create_separator( "" ) );
    item_levelset = item_create_switch_x( "Levelset:", "", &config.levelset_id_local, levelset_names_local, levelset_count_local );
    item_levelset->callback = cb_update_levelset_hint;
    cb_update_levelset_hint(); /* initiate first hint */
    menu_add( game, item_levelset );
    menu_add( game, item_create_switch_x( "Difficulty:", HINT_DIFF, &config.diff, str_diff, 3 ) );
    menu_add( game, item_create_separator( "" ) );
    menu_add( game, item_create_range( "Players:", HINT_PLAYERS, &config.player_count, 1, 4, 1 ) );
    menu_add( game, item_create_edit( "1st:", HINT_PLAYERS, config.player_names[0], 12 ) );
    menu_add( game, item_create_edit( "2nd:", HINT_PLAYERS, config.player_names[1], 12 ) );
    menu_add( game, item_create_edit( "3rd:", HINT_PLAYERS, config.player_names[2], 12 ) );
    menu_add( game, item_create_edit( "4th:", HINT_PLAYERS, config.player_names[3], 12 ) );
    menu_add( game, item_create_separator( "" ) );
    menu_add( game, item_create_link( "Back", HINT_, _main ) );
    /* controls */
    keys[0] = item_create_key( "Left:", HINT_KEYS, &config.k_left, filter );
    keys[1] = item_create_key( "Right:", HINT_KEYS, &config.k_right, filter );
    keys[2] = item_create_key( "Left Fire:", HINT_KEYS, &config.k_lfire, filter );
    keys[3] = item_create_key( "Right Fire:", HINT_KEYS, &config.k_rfire, filter );
    keys[4] = item_create_key( "Turbo:", HINT_KEYS, &config.k_turbo, filter );
    keys[5] = item_create_key( "Return:", HINT_KEYS, &config.k_return, filter );
//    keys[6] = item_create_key( "Warp:", HINT_KEYS, &config.k_warp, filter );
    for ( i = 0; i < 6; i++ )
        menu_add( cont, keys[i] );
    /* dynamic restriction */
    for ( i = 0; i < 6; i++ )
        for ( j = 0; j < 6; j++ )
            if ( j != i )
                value_add_other_key( keys[i]->value, keys[j]->value );
    /* other control stuff */
    menu_add( cont, item_create_separator( "" ) );
    menu_add( cont, item_create_switch( "Relative Motion:", HINT_RELMOTION, &config.rel_motion, "Off", "On" ) );
    menu_add( cont, item_create_range( "Motion Modifier:", HINT_MOD, &config.motion_mod, 40, 160, 10 ) );
    menu_add( cont, item_create_switch( "Invert Motion:", HINT_INVERT, &config.invert, "Off", "On" ) );
    menu_add( cont, item_create_switch( "Grab Mouse:", HINT_GRAB, &config.grab, "Off", "On" ) );
    menu_add( cont, item_create_separator( "" ) );
    menu_add( cont, item_create_link( "Back", HINT_, _main ) );
    /* advanced options */
    menu_add( adv, item_create_switch( "Convex Paddle:", HINT_CONVEX, &config.convex, "Off", "On" ) );
    //menu_add( adv, item_create_switch( "Corner:", HINT_CORNER, &config.linear_corner, "Spherical", "Linear" ) );
    menu_add( adv, item_create_switch( "Balls:", HINT_BALLLEVEL, &config.ball_level, "Below Bonuses", "Above Bonuses" ) );
    menu_add( adv, item_create_switch( "Debris:", HINT_DEBRISLEVEL, &config.debris_level, "Below Balls", "Above Balls" ) );
    item = item_create_range( "Key Speed:", HINT_KEYSPEED, &config.i_key_speed, 40, 100, 5 );
    item->callback = cb_set_keyspeed;
    menu_add( adv, item );
    menu_add( adv, item_create_switch( "Return Balls:", HINT_RETURN, &config.return_on_click, "Auto", "By Click" ) );
    item = item_create_switch( "Use Hints:", HINT_USEHINTS, &config.use_hints, "Off", "On" );
    item->callback = cb_hints;
    menu_add( adv, item );
    menu_add( adv, item_create_range( "Warp Limit:", HINT_WARP_LIMIT, &config.rel_warp_limit, 0, 100, 10 ) );
    //menu_add( adv, item_create_separator( "" ) );
//    menu_add( adv, item_create_link( "Select Theme", HINT_, theme ) );
    menu_add( adv, item_create_separator( "" ) );
    menu_add( adv, item_create_link( "Back", HINT_, _main ) );
    /* editor */
    item_set = item_create_switch_x( "Set:", HINT_EDITOR_SET, &config.levelset_id_home, levelset_names_home, levelset_count_home );
    menu_add( editor, item_set );
    menu_add( editor, item_create_action( "Edit Set", HINT_EDITOR_EDIT, ACTION_EDIT ) );
    menu_add( editor, item_create_link( "Delete Set", HINT_EDITOR_DELETE, confirm_delete ) );
    menu_add( editor, item_create_separator( "" ) );
    menu_add( editor, item_create_link( "Back", HINT_, _main ) );
    /* confirm_delete set */
    item = item_create_link( "Yes", HINT_ ,editor );
    item->callback = cb_delete_set;
    menu_add( confirm_delete, item );
    menu_add( confirm_delete, item_create_link( "No", HINT_, editor ) );
    /* theme */
    item_theme = item_create_switch_x( "Theme:", "", &config.theme_id, theme_names, theme_count );
    menu_add( theme, item_theme );
    item_theme->callback = cb_update_theme_hint;
    cb_update_theme_hint();
    menu_add( theme, item_create_separator( "" ) );
    item = item_create_link( "Apply", HINT_APPLY, gfx );
    item->callback = cb_change_theme;
    menu_add( theme, item );
    menu_add( theme, item_create_link( "Cancel", HINT_, gfx ) );

    /* adjust all menus */
    menu_adjust( _main );
#ifdef _1
    menu_adjust( options );
#endif
#ifdef AUDIO_ENABLED
    menu_adjust( audio );
#endif
    menu_adjust( gfx );
    menu_adjust( game );
    menu_adjust( cont );
    menu_adjust( adv );
    menu_adjust( editor );
    menu_adjust( confirm_delete );
    menu_adjust( theme );
    /* set main menu as current */
    menu_select( _main );
}
void manager_delete()
{
    list_delete( menus );
}
/*
====================================================================
Run menu until request sent
====================================================================
*/
int manager_run()
{
    SDL_Event event;
    int event_polled = 0; /* event occured? */
    int result = ACTION_NONE;
    int ms;
    /* draw highscores */
    chart_show( chart_set_query_id( chart_id ), cx, cy, cw, ch );
    /* loop */
    stk_timer_reset();
    while ( result == ACTION_NONE && !stk_quit_request ) {
        menu_hide( cur_menu );
        hint_hide();
        if ( event_poll( &event ) ) event_polled = 1;
        if ( event_polled && event.type == SDL_QUIT ) {
            result = ACTION_QUIT;
            stk_quit_request = 1;
        }
		/* fullscreen if no item selected */
		if ( event_polled ) {
			if ( cur_menu->cur_item == 0 || ( cur_menu->cur_item->type != ITEM_EDIT && cur_menu->cur_item->type != ITEM_KEY ) )
				if ( event.type == SDL_KEYDOWN )
					if ( event.key.keysym.sym == SDLK_f ) {
						config.fullscreen = !config.fullscreen;
						stk_display_apply_fullscreen( config.fullscreen );
                        stk_surface_blit( 
                            mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
                        stk_display_update( STK_UPDATE_ALL );
					}
			/* check if clicked on highscore */
			if ( event.type == SDL_MOUSEBUTTONUP ) 
				if ( event.button.x >= cx && event.button.y >= cy )
					if ( event.button.x < cx + cw && event.button.y < cy + ch ) {
#ifdef AUDIO_ENABLED
						stk_sound_play( wav_menu_click );
#endif
						/* set chart id */
						if ( event.button.button == STK_BUTTON_LEFT ) {
							chart_id++;
							if ( chart_id == charts->count ) chart_id = 0;
						}
						else {
							chart_id--;
							if ( chart_id == -1 ) chart_id = charts->count - 1;
						}
						/* redraw */
                        stk_surface_blit( 
                            mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
					    chart_show( chart_set_query_id( chart_id ), cx, cy, cw, ch );
					}
            result = menu_handle_event( cur_menu, &event );
		}
        ms = stk_timer_get_time();
        menu_update( cur_menu, ms );
        hint_update( ms );
        menu_show( cur_menu );
        chart_show( chart_set_query_id( chart_id ), cx, cy, cw, ch );
        hint_show();
        stk_display_update( STK_UPDATE_RECTS );
        SDL_Delay( 5 );
        event_polled = 0;
    }
    return result;
}
/*
====================================================================
Fade in/out background of menu
====================================================================
*/
void manager_fade( int type )
{
    if ( type == STK_FADE_IN )
        stk_surface_blit( mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_fade( type, STK_FADE_DEFAULT_TIME );
}
/*
====================================================================
Update screen without menu itself as this is shown next frame.
====================================================================
*/
void manager_show()
{
    stk_display_apply_fullscreen( config.fullscreen );
    stk_surface_blit( mbkgnd, 0,0,-1,-1, stk_display, 0,0 );
    chart_show( chart_set_query_id( chart_id ), cx, cy, cw, ch );
    stk_display_update( STK_UPDATE_ALL );
}

/*
====================================================================
Update set list when creating a new file for editor.
====================================================================
*/
void manager_update_set_list()
{
    cb_adjust_set_list(); /* hacky but shiiiit how cares? */
}
