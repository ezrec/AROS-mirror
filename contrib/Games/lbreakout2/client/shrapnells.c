/***************************************************************************
                          shrapnells.c  -  description
                             -------------------
    begin                : Sat Sep 8 2001
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

#include "lbreakout.h"
#include "config.h"
#include "shrapnells.h"

extern Config config;
extern SDL_Surface *stk_display;
extern SDL_Surface *offscreen;
List *shrapnells = 0;
float shr_alpha_change = 0.25;
float shr_start_alpha = 255;

/*
====================================================================
Local
====================================================================
*/

/*
====================================================================
Create a single shrapnell and add to list.
====================================================================
*/
void shrapnell_create( int x, int y, int w, int h, float vx, float vy )
{
    Shrapnell *shr = calloc( 1, sizeof( Shrapnell ) );
    shr->pic = stk_surface_create( SDL_SWSURFACE, w,h );
    SDL_SetColorKey(shr->pic, 0, 0);
    stk_surface_blit( offscreen, x, y,w,h, shr->pic, 0,0 );
    shr->x = x;
    shr->y = y;
    shr->v.x = vx;
    shr->v.y = vy;
    shr->alpha = shr_start_alpha;
    list_add( shrapnells, shr);
}
/*
====================================================================
Delete a shrapnell
====================================================================
*/
void shrapnell_delete( void *ptr )
{
    Shrapnell *shr = (Shrapnell*)ptr;
    SDL_FreeSurface( shr->pic );
    free( shr );
}

/*
====================================================================
Public
====================================================================
*/

/*
====================================================================
Initate or delete shrapnell list
====================================================================
*/
void shrapnells_init()
{
    shrapnells = list_create( LIST_AUTO_DELETE, shrapnell_delete );
}
void shrapnells_delete()
{
    list_delete( shrapnells );
}
/*
====================================================================
Create shrapnells from position in offscreen. The vector imp is the 
impuls and type the type of what caused the destruction.
====================================================================
*/
void shrapnells_create( int x, int y, int w, int h, int type, Vector imp ) 
{
    int i, j;
    int shr_w, shr_h;
    Vector v;
    int dx, dy;
    /* no animations? */
    if ( !config.anim ) 
        return;
    /*
     * 1 - low
     * 2 - medium
     * 3 - high
     */
    /* according to type create shrapnells */
    switch ( type ) {
        case SHR_BY_NORMAL_BALL:
            vector_norm( &imp );
            imp.x *= 0.13; imp.y *= 0.13;
            shrapnell_create( x, y, w, h, imp.x, imp.y );
            break;
        case SHR_BY_ENERGY_BALL:
            if ( config.anim == 1 ) {
                /* low */
                shrapnell_create( x, y, w, h, 0, 0 );
                break;
            }
            shr_w = w / 2;
            shr_h = h / 2;
            for ( i = 0; i < w; i += shr_w )
                for ( j = 0; j < h; j += shr_h ) {
                    v.x = ( x + ( w >> 1 ) ) - ( x + i + ( shr_w >> 1 ) );
                    v.y = ( y + ( h >> 1 ) ) - ( y + j + ( shr_h >> 1 ) );
                    vector_norm( &v );
                    v.x *= 0.01; v.y *= 0.01;
                    shrapnell_create( x + i, y + j, shr_w, shr_h, v.x, v.y );
                }
            break;
        case SHR_BY_SHOT:
            if ( config.anim == 1 ) {
                /* low */
                shrapnell_create( x, y, w, h, 0, imp.y * 0.02 );
                break;
            }
            shr_w = w / 8;
            shr_h = h;
            for ( i = 0; i < ( w >> 1 ); i += shr_w ) {
                shrapnell_create( x + i, y, shr_w, shr_h, 0, imp.y * 0.002 * ( i + 1 ) );
                shrapnell_create( x + w - shr_w - i, y, shr_w, shr_h, 0, imp.y * 0.002 * ( i + 1 ) );
            }
            break;
        case SHR_BY_EXPL:
            shr_w = config.anim == 3 ? 5 : 10;
            shr_h = config.anim == 3 ? 5 : 10;
            for ( i = 0; i < w; i += shr_w )
                for ( j = 0; j < h; j += shr_h ) {
                    dx = rand() % 2 == 0 ? 1 : -1;
                    dy = rand() % 2 == 0 ? 1 : -1;
                    shrapnell_create( x + i, y + j, shr_w, shr_h, 
                                      (float)( ( rand() % 6 ) + 5) / 100 * dx, (float)( ( rand() % 6 ) + 5) / 100 * dy );
                }
            break;
    }
}
#ifdef _1
/*
====================================================================
Create shrapnells from position in offscreen
====================================================================
*/
void shrapnells_create( int x, int y, int w, int h, int res )
{
    int mod = config.anim;
    int i, j;
    int r;
    int shr_w, shr_h, dx, dy;

    /* no animations? */
    if (!config.anim) return;

    /* high/low resolution shrapnells? */
    if ( res == SHR_LOW_RES )
        r = rand() % 3;
    else {
        r = rand() % 5;
        if (r == 4) r = 3;
    }

    /* check result */
    switch (r) {
        case 0:
            shr_w = w / (8 * mod);
            shr_h = h;
            for (i = 0; i < (4 * mod); i++)
                shrapnell_create(x + i * shr_w, y, shr_w, shr_h, -0.05 - i * 0.01, 0);
            for (i = (4 * mod) - 1; i >= 0; i--)
                shrapnell_create(x + w - (i + 1) * shr_w, y, shr_w, shr_h, 0.05 + i * 0.01, 0);
            break;
        case 1:
            shr_w = w;
            shr_h = h / (4 * mod);
            for (i = 0; i < (2 * mod); i++)
                shrapnell_create(x, y + i * shr_h, shr_w, shr_h, 0, -0.05 - i * 0.01);
            for (i = (2 * mod) - 1; i >= 0; i--)
                shrapnell_create(x, y + h - (i + 1) * shr_h, shr_w, shr_h, 0, 0.05 + i * 0.01);
            break;
        case 2:
            shr_w = w / (8 * mod);
            shr_h = h;
            for (i = 0; i < (8 * mod); i++)
                shrapnell_create(x + i * shr_w, y, shr_w, shr_h, 0, (1 - 2 * (i & 1)) * 0.1);
            break;
        case 3:
            shr_w = config.anim == 1 ? 8 : 4;
            shr_h = config.anim == 1 ? 8 : 4;
            for (i = 0; i < w / shr_w; i++)
                for (j = 0; j < h / shr_h; j++) {
                    dx = rand() % 2 == 0 ? 1 : -1;
                    dy = rand() % 2 == 0 ? 1 : -1;
                    shrapnell_create(x + i * shr_w, y + j * shr_h, shr_w, shr_h, (float)(rand()%6+5) / 100 * dx, (float)(rand()%6+5) / 100 * dy);
                 }
            break;
    }
}
#endif
/*
====================================================================
Remove all shrapnells
====================================================================
*/
void shrapnells_reset()
{
    list_clear( shrapnells );
}
/*
====================================================================
Show, hide all shrapnells.
====================================================================
*/
void shrapnells_hide()
{
    ListEntry  *entry = shrapnells->head->next;
    Shrapnell   *shr;
    int         x, y;
    while ( entry != shrapnells->tail ) {
        shr = entry->item;
        entry = entry->next;
        x = (int)shr->x;
        y = (int)shr->y;
        stk_surface_blit( offscreen, x, y,
            shr->pic->w, shr->pic->h,
            stk_display, x, y );
        stk_display_store_drect();
    }
}
void shrapnells_show()
{
    ListEntry  *entry = shrapnells->head->next;
    Shrapnell   *shr;
    while ( entry != shrapnells->tail ) {
        shr = entry->item;
        entry = entry->next;
        stk_surface_blit( shr->pic, 
            0,0,shr->pic->w,shr->pic->h,
            stk_display, (int)shr->x, (int)shr->y );
        stk_display_store_drect();
    }
}
/*
====================================================================
Update and remove expired shrapnells
====================================================================
*/
void shrapnells_update( int ms )
{
    ListEntry  *entry = shrapnells->head->next;
    Shrapnell   *shr;
    while ( entry != shrapnells->tail ) {
        shr = entry->item;
        entry = entry->next;
        shr->x += shr->v.x * ms;
        shr->y += shr->v.y * ms;
        if (shr->alpha > 0 ) {
            shr->alpha -= shr_alpha_change * ms;
            SDL_SetAlpha( shr->pic, SDL_SRCALPHA, (int)shr->alpha );
        }
        if (shr->alpha <= 0 || 
            shr->x + shr->pic->w < 0 || 
            shr->y + shr->pic->h < 0 || 
            shr->x > stk_display->w || 
            shr->y > stk_display->h )
            list_delete_entry( shrapnells, entry->prev );
    }
}

/*
====================================================================
Brick explosion animations.
====================================================================
*/
#define exp_change 0.015
#define exp_alpha_change 0.10
List *exps;
SDL_Surface *current_exp_pic = 0; /* points either to exp_pic or exp_dark_pic */
extern SDL_Surface *exp_pic;
extern SDL_Surface *exp_dark_pic;
int   exp_w, exp_h; /* size */
int   exp_frame_count; /* frame count */
int   exp_count; /* number of various explosions */
typedef struct {
    float alpha;
    float cur; /* current frame */
    int x, y; /* position in screen */
    int y_off; /* y offset in explosion picture */
} Exp;
void exp_load()
{
    exps = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
    exp_w = exp_h = 60; /* hardcoded as we have various explosions in one file now */
    exp_count = exp_pic->h / exp_h;
    exp_frame_count = exp_pic->w / exp_w;
}
void exps_clear()
{
    list_clear( exps );
}
void exp_delete()
{
    if ( exps ) list_delete( exps ); exps = 0;
}
void exp_create( int x, int y )
{
    Exp *exp = calloc( 1, sizeof( Exp ) );
    exp->x = x - ( exp_w >> 1 );
    exp->y = y - ( exp_h >> 1 );
    exp->alpha = 255;
    /* red/yellow get 4 parts, gree&yellow&bluw get 1 part each */ 
    switch ( rand() % ( 4 + 4 + 1 + 1 + 1 ) ) {
        case 0: case 1: case 2: case 3:
            exp->y_off = 0; break;
        case 4: case 5: case 6: case 7:
            exp->y_off = exp_h * 4; break;
        case 8:
            exp->y_off = exp_h * 1; break;
        case 9: 
            exp->y_off = exp_h * 2; break;
        case 10:
            exp->y_off = exp_h * 3; break;
    }
    list_add( exps, exp );
}
void exps_hide()
{
    Exp *exp;
    list_reset( exps );
    while ( ( exp = list_next( exps ) ) ) {
        stk_surface_blit( offscreen, exp->x, exp->y,
            exp_w, exp_h, stk_display, exp->x, exp->y );
        stk_display_store_drect();
    }
}
void exps_show()
{
    Exp *exp;
    list_reset( exps );
    while ( ( exp = list_next( exps ) ) ) {
        stk_surface_alpha_blit( current_exp_pic, 
            (int)exp->cur * exp_w, exp->y_off,
            exp_w, exp_h,
            stk_display, exp->x, exp->y, exp->alpha );
    }
}
void exps_update( int ms )
{
    Exp *exp;
    list_reset( exps );
    while ( ( exp = list_next( exps ) ) ) {
        if ( exp->alpha > 0 ) {
            exp->alpha -= exp_alpha_change * ms;
            if ( exp->alpha < 0 ) exp->alpha = 0;
        }
        exp->cur += exp_change * ms;
        if ( exp->cur >= exp_frame_count )
            list_delete_current( exps );
    }    
}
void exps_set_dark( int dark )
{
    if ( dark )
        current_exp_pic = exp_dark_pic;
    else
        current_exp_pic = exp_pic;
}

