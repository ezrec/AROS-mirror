/***************************************************************************
                          config.h  -  description
                             -------------------
    begin                : Tue Feb 13 2001
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

#ifndef __CONFIG_H
#define __CONFIG_H

/* configure struct */
enum { BALL_BELOW_BONUS = 0, BALL_ABOVE_BONUS };
enum { DEBRIS_BELOW_BALL = 0, DEBRIS_ABOVE_BALL };
typedef struct {
    /* directory to save config and saved games */
    char dir_name[512];
    /* levels */
    int levelset_id_local;
    int levelset_count_local; /* save number of local game levelsets 
                                 for safety (to reset id if count changed) */
    int levelset_id_home;
    int levelset_count_home; /* save number of levelsets for safety 
                                (to reset id if count changed) */
    /* players */
    int player_count;
    char player_names[4][32];
    /* game */
    int diff; /* diffculty */
    int startlevel;
    int rel_warp_limit; /* percentage of bricks required to be destroyed 
                           before player may proceed to next level */
    /* controls */
    int k_left;
    int k_right;
    int k_lfire;
    int k_rfire;
    int k_return; /* return ball on click on this key */
    int k_turbo; /* double paddle speed while this key is pressed */
    int k_warp; /* warp to next level */
    int rel_motion; /* use relative mouse motion; motion_mod and invert need this enabled */
    int grab; /* keep mouse in window */
    int motion_mod;
    int convex;
    int invert;
    float key_speed; /* move with key_speed pix per sec when keys are used */
    int linear_corner; /* assume a 45° line for corner? */
    /* sound */
    int sound;
    int volume; /* 1 - 8 */
    int speech; /* enable speech? */
    int audio_buffer_size;
    /* graphics */
    int anim;
    int fullscreen;
    int fade;
    int bonus_info;
    int fps; /* frames per second: 0 - no limit, 1 - 50, 2 - 100, 3 - 200 */
	int ball_level;
	int debris_level;
    int i_key_speed; /* integer value that is devided by 100 to get real key_speed */
    /* various */
    int use_hints;
    int return_on_click; /* autoreturn on click if true else automatically */
    int theme_id; /* 0 == default theme */
    int theme_count; /* to check and properly reset id if number of themes changed */
    /* multiplayer */
    char host[16];
    int  port; /* remote address of server */
    char username[16]; /* username at server */
    int  mp_diff;   /* difficulty */
    int  mp_rounds; /* rounds per level in a levelset */
    int  mp_frags; /* points a player needs to win a round */
    int  mp_balls; /* maximum number of balls a player may fire */
    int  client_game_port; /* port at which the client tries to run the direct connection */
} Config;

/* config directory name in home directory */
#ifdef _WIN32
#define CONFIG_DIR_NAME "lgames"
#else
#define CONFIG_DIR_NAME ".lgames"
#endif

/* set config to default */
void config_reset();

/* load config */
void config_load( );

/* save config */
void config_save( );

#endif
