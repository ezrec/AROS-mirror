/***************************************************************************
                          misc.c  -  description
                             -------------------
    begin                : Thu Sep 6 2001
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

#include <time.h>
#include "lbreakout.h"
#include "config.h"
#include "event.h"
#include "misc.h"

extern SDL_Surface *stk_display;
extern SDL_Surface *offscreen;
extern int stk_quit_request;
int shadow_size = 8;
#ifdef AUDIO_ENABLED
extern StkSound *wav_click;
#endif
extern int motion_button;
extern Config config;
extern int bkgnd_count;
extern SDL_Surface **bkgnds;

char circle_msg[256];

/*
====================================================================
Load background according to id and draw background to offscreen.
Return Value: loaded background surface
====================================================================
*/
SDL_Surface* bkgnd_draw( int id )
{
    SDL_Surface *bkgnd = 0;
    SDL_Surface *pic = 0;
    int i, j;

	if ( id >= bkgnd_count || id == -1 )
        id = rand() % bkgnd_count;
    
    /* background is always the size of screen */
    bkgnd = stk_surface_create( SDL_SWSURFACE, 
        stk_display->w, stk_display->h );
    SDL_SetColorKey( bkgnd, 0, 0 );
    stk_surface_fill( bkgnd, 0,0,-1,-1, 0x0 );

    /* load background */
    pic = bkgnds[id];
    for ( i = 0; i < bkgnd->w; i += pic->w ) {
        for ( j = 0; j < bkgnd->h; j += pic->h ) {
            stk_surface_blit( pic, 0,0,-1,-1,
                bkgnd, i, j);
        }
    }

    /* draw to offscreen */
    stk_surface_blit( bkgnd, 0,0,-1,-1, offscreen, 0,0 );

    return bkgnd;
}
/*
====================================================================
Confirm request. Darkens screen a bit and display text.
Return Value: True if successful
====================================================================
*/
void draw_confirm_screen( StkFont *font, SDL_Surface *buffer, char *str )
{
    int i, y, x;
    Text *text = create_text( str, 60 );
    stk_surface_fill( stk_display, 0,0,-1,-1, 0x0 );
    stk_surface_alpha_blit( buffer, 0,0,-1,-1, stk_display, 0,0, 128 );
    font->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_TOP;
    y = (stk_display->h - text->count * font->height) / 2;
    x = stk_display->w / 2;
    for ( i = 0; i < text->count; i++ ) {
        stk_font_write(font, stk_display, x, y, STK_OPAQUE, text->lines[i]);
        y += font->height;
    }
    delete_text( text );
}
int confirm( StkFont *font, char *str, int type )
{
    SDL_Event event;
    int go_on = 1;
    int ret = 0;
    SDL_Surface *buffer = 
        stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    SDL_SetColorKey(buffer, 0, 0);

#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif

    event_block_motion( 1 );
    event_clear_sdl_queue();

    stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
    if ( type == CONFIRM_PAUSE )
        stk_surface_gray( stk_display, 0,0,-1,-1,0 );
    else
        draw_confirm_screen( font, buffer, str );
    stk_display_update( STK_UPDATE_ALL );

    while (go_on && !stk_quit_request) {
        SDL_WaitEvent(&event);
        /* TEST */
        switch ( event.type ) {
            case SDL_QUIT: stk_quit_request = 1; break;
            case SDL_MOUSEBUTTONUP:
                if ( type == CONFIRM_ANY_KEY ) {
                    ret = 1; go_on = 0;
                }
/*                else
                if ( type == CONFIRM_YES_NO ) {
                    if ( event.button.button == LEFT_BUTTON )
                        ret = 1;
                    else
                        ret = 0;
                    go_on = 0;
                }*/
                break;
            case SDL_KEYDOWN:
                if ( type == CONFIRM_ANY_KEY ) {
                    ret = 1; go_on = 0;
                    break;
                }
                else
                if ( type == CONFIRM_PAUSE ) {
                    if ( event.key.keysym.sym == SDLK_p ) {
                        ret = 1; go_on = 0;
                        break;
                    }
					else
					if ( event.key.keysym.sym == SDLK_f ) {
						config.fullscreen = !config.fullscreen;
                        stk_display_apply_fullscreen( config.fullscreen );
						draw_confirm_screen( font, buffer, str );
						stk_display_update( STK_UPDATE_ALL );
					}
                }
                else
                switch (event.key.keysym.sym) {
                    case SDLK_y:
                        go_on = 0;
                        ret = 1;
                        break;
                    case SDLK_ESCAPE:
                    case SDLK_n:
                        go_on = 0;
                        ret = 0;
                    default:
                        break;
                }
                break;
        }
    }
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif
    stk_surface_blit( buffer, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
    SDL_FreeSurface(buffer);

    event_block_motion( 0 );

    return ret;
}
#ifdef NETWORK_ENABLED
/*
====================================================================
Display a info message (gray screen a bit and display text), 
send a MSG_READY when player has clicked and wait for a remote answer
(timeout 10 secs). Waiting may be cancelled by pressing ESCAPE which
results in sending a MSG_GAME_EXITED.
Return Value: True if both peers clicked to continue, False if the
connection was cancelled for some reason.
====================================================================
*/
int display_info( StkFont *font, char *str, Net_Socket *peer )
{
    char error[128];
    Net_Msg msg;
    SDL_Event event;
    int ret = 0, leave = 0;
    SDL_Surface *buffer = 
        stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    SDL_SetColorKey(buffer, 0, 0);

#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif

    event_block_motion( 1 );
    event_clear_sdl_queue();

    stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
	draw_confirm_screen( font, buffer, str );
    stk_display_update( STK_UPDATE_ALL );
    stk_wait_for_input();
    net_write_empty_msg( peer, MSG_READY ); 
	draw_confirm_screen( font, buffer, 
                         "Waiting for remote answer..." );
    stk_display_update( STK_UPDATE_ALL );
    event_clear_sdl_queue();
    while ( !leave ) {
        if ( SDL_PollEvent( &event ) )
            if ( (event.type == SDL_KEYDOWN && 
                 event.key.keysym.sym == SDLK_ESCAPE) ||
                 event.type == SDL_QUIT ) {
                    net_write_empty_msg( peer, MSG_GAME_EXITED );
                    leave = 1;
                    break;
                }
        if ( net_read_msg( peer, &msg, 0 ) )
            switch ( msg.type ) {
                case MSG_READY:
                    ret = 1; leave = 1;
                    break;
                case MSG_GAME_EXITED:
                    ret = 0; leave = 1;
                    sprintf( error, "remote player cancelled the game\n" );
                    confirm( font, error, CONFIRM_ANY_KEY );
                    break;
            }
        SDL_Delay( 10 );
    }
    
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif
    
    stk_surface_blit( buffer, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
    SDL_FreeSurface(buffer);
    
    event_block_motion( 0 );
    return ret;
}
#endif
/*
====================================================================
Create shadow surface for specified region in surface.
Return Value: Shadow surface
====================================================================
*/
SDL_Surface* create_shadow( SDL_Surface *surf, int x, int y, int w, int h )
{
    SDL_Surface *shadow = 0;
    int i, j;
    Uint32 white = SDL_MapRGB( stk_display->format, 0xff, 0xff, 0xff );
    Uint32 black = SDL_MapRGB( stk_display->format, 0, 0, 0 );
    shadow = stk_surface_create( SDL_SWSURFACE, w, h );
    SDL_SetColorKey( shadow, SDL_SRCCOLORKEY, white );
    for ( i = 0; i < w; i++ )
        for ( j = 0; j < h; j++ ) {
            if ( surf->flags & SDL_SRCCOLORKEY &&
                 stk_surface_get_pixel( surf, i, j ) == surf->format->colorkey )
                stk_surface_set_pixel( shadow, i, j, white );
            else
                stk_surface_set_pixel( shadow, i, j, black );
        }
    return shadow;
}

/*
====================================================================
Return vector struct with the specified coordinates.
====================================================================
*/
Vector vector_get( float x, float y )
{
    Vector v = { x, y };
    return v;
}
/*
====================================================================
Give vector the normed length of 1.
====================================================================
*/
void vector_norm( Vector *v )
{
    float length;
    if ( v->x == 0 && v->y == 0 ) return; /* NULL vector may not be normed */
    length = sqrt( v->x * v->x + v->y * v->y );
    v->x /= length;
    v->y /= length;
}
/*
====================================================================
Return monotony of vector. If vertical return 0
====================================================================
*/
float vector_monotony( Vector v )
{
    if ( v.x == 0 ) return 0;
    return v.y / v.x;
}
/*
====================================================================
Set length of a vector.
====================================================================
*/
void vector_set_length( Vector *v, float length )
{
    vector_norm( v );
    v->x *= length; v->y *= length;
}

/*
====================================================================
Initiate a line struct.
====================================================================
*/
void line_set( Line *line, float x, float y, float m )
{
    line->vertical = 0;
    line->m = m;
    line->n = y - m*x;
}
void line_set_vert( Line *line, float x )
{
    line->vertical = 1;
    line->x = x;
}
void line_set_hori( Line *line, float y )
{
    line->vertical = 0;
    line->m = 0;
    line->n = y;
}
/*
====================================================================
Intersect lines and set 'pos' to intersecting point.
Return Value: True if lines intersect.
====================================================================
*/
int line_intersect( Line *line, Line *target, Coord *pos )
{
    /* reset pos */
    pos->x = pos->y = 0;
    /* if lines are parallel return False */
    if ( line->vertical && target->vertical ) return 0; /* vertical parallels */ 
    if ( !line->vertical &&  !target->vertical && line->m == target->m ) return 0; /* non-vertical parallels */
    /* right now only one thing is supported: line horizontal */
    if ( line->m == 0 && line->vertical == 0 ) {
        pos->y = line->n;
        if ( target->vertical )
            pos->x = target->x;
        else
            pos->x = ( pos->y - target->n ) / target->m;
        return 1;
    }
    if ( line->vertical ) {
        if ( target->vertical ) return 0;
        pos->x = line->x;
        pos->y = target->m * pos->x + target->n;
        return 1;
    }
    if ( target->vertical ) {
        printf( "line_intersect: line non-vertical and target vertical not supported yet\n" );
        return 1;
    }
    /* compute if both lines are neither vertical nor horizontal */
    pos->x = ( line->n - target->n ) / ( target->m - line->m );
    pos->y = line->m * pos->x + line->n;
    return 1;
}

/*
====================================================================
Initiate a line struct.
====================================================================
*/
void iline_set( ILine *line, int x, int y, int m_4096 )
{
    line->vertical = 0;
    line->m_4096 = m_4096;
    line->n = y - ((m_4096*x)>>12);
}
void iline_set_vert( ILine *line, int x )
{
    line->vertical = 1;
    line->x = x;
}
void iline_set_hori( ILine *line, int y )
{
    line->vertical = 0;
    line->m_4096 = 0;
    line->n = y;
}
/*
====================================================================
Intersect lines and set 'pos' to intersecting point.
Return Value: True if lines intersect.
====================================================================
*/
int iline_intersect( ILine *line, ILine *target, ICoord *pos )
{
    /* reset pos */
    pos->x = pos->y = 0;
    /* if lines are parallel return False */
    if ( line->vertical && target->vertical ) return 0; /* vertical parallels */ 
    if ( !line->vertical &&  !target->vertical )
    if ( line->m_4096 == target->m_4096 ) return 0; /* non-vertical parallels */
    /* right now only one thing is supported: line horizontal */
    if ( line->m_4096 == 0 && line->vertical == 0 ) {
        pos->y = line->n;
        if ( target->vertical )
            pos->x = target->x;
        else
            pos->x = (( pos->y - target->n )<<12) / target->m_4096;
        return 1;
    }
    if ( line->vertical ) {
        if ( target->vertical ) return 0;
        pos->x = line->x;
        pos->y = ((target->m_4096 * pos->x)>>12) + target->n;
        return 1;
    }
    if ( target->vertical ) {
        printf( "line_intersect: line non-vertical and target vertical not supported yet\n" );
        return 1;
    }
    /* compute if both lines are neither vertical nor horizontal */
    pos->x = (( line->n - target->n )<<12) / ( target->m_4096 - line->m_4096 );
    pos->y = ((line->m_4096 * pos->x)>>12) + line->n;
    return 1;
}

/*
====================================================================
Intersect line pos+t*v with circle (x+m)²=r²
Important length of v MUST be 1.
Return Value: True if intersecting, Intersecting points
====================================================================
*/
int circle_intersect( Vector m, int r, Vector pos, Vector v, Vector *t1, Vector *t2 )
{
    Vector delta = { pos.x - m.x, pos.y - m.y };
    float  delta_v = delta.x * v.x + delta.y * v.y;
    float dis = delta_v * delta_v + r * r - ( delta.x * delta.x + delta.y * delta.y );
    float t;

    if ( dis < 0 ) {
#ifdef WITH_BUG_REPORT
		sprintf( circle_msg, "Diskriminante < 0" );
#endif		
		return 0; 
	}
	dis = sqrt( dis );

    t = -delta_v + dis;
    t1->x = pos.x + t * v.x; t1->y = pos.y + t * v.y;
    t = -delta_v - dis;
    t2->x = pos.x + t * v.x; t2->y = pos.y + t * v.y;
#ifdef WITH_BUG_REPORT
	sprintf( circle_msg, "Intersection points: (%4.2f,%4.2f), (%4.2f,%4.2f)", t1->x, t1->y, t2->x, t2->y );
#endif
    return 1;
}

/*
====================================================================
Enter a string and return True if ENTER received and False
if ESCAPE received.
====================================================================
*/
int enter_string( StkFont *font, char *caption, char *edit, int limit )
{
    SDL_Event event;
    int go_on = 1;
    int ret = 0;
    SDL_Surface *buffer = 
        stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    int length = strlen( edit );

    SDL_SetColorKey(buffer, 0, 0);

    event_block_motion( 1 );

    stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
    font->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_CENTER_Y;

    while ( go_on && !stk_quit_request ) {

        stk_surface_fill( stk_display, 0,0,-1,-1, 0x0 );
        stk_surface_alpha_blit( buffer, 0,0,-1,-1, stk_display, 0,0, 128 );
        stk_font_write(font, stk_display, stk_display->w / 2, stk_display->h / 2, STK_OPAQUE, caption);
        write_text_with_cursor(font, stk_display, stk_display->w / 2, stk_display->h / 2 + font->height, edit, STK_OPAQUE);
        stk_display_update( STK_UPDATE_ALL );
        event.type = SDL_NOEVENT;
        SDL_PollEvent(&event);
        /* TEST */
        switch ( event.type ) {
            case SDL_QUIT: stk_quit_request = 1; break;
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        ret = 0;
                        go_on = 0;
                        break;
                    case SDLK_RETURN:
                        ret = 1;
                        go_on = 0;
                        break;
                    case SDLK_BACKSPACE:
                        if ( length > 0 ) edit[--length] = 0;
                        break;
                    default:
                        if ( event.key.keysym.sym >= 32 && event.key.keysym.sym < 128 && length < limit ) {
                            edit[length++] = event.key.keysym.unicode;
                            edit[length] = 0;
                        }
                        break;
                }
                break;
        }
    }
    stk_surface_blit( buffer, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
    SDL_FreeSurface(buffer);

    event_block_motion( 0 );

    return ret;
}

/*
====================================================================
Display text blinking.
====================================================================
*/
void  write_text_with_cursor( StkFont *fnt, SDL_Surface *dest,
    int x, int y, char *str, int alpha)
{
    static int cursor_on = 0;
    static Uint32 last_tick = 0;
    // create temporary space for cursor and text
    char *text_with_cursor = calloc(strlen(str) + 2, sizeof(char));
    if (text_with_cursor) {
       strcpy(text_with_cursor, str);

       // Time to blink cursor on/off?
       if (SDL_GetTicks() - last_tick > 500) {
            last_tick = SDL_GetTicks();
	    cursor_on = ! cursor_on;
       }
       // Tack on cursor to end of text
       if (cursor_on) {
	    strcat(text_with_cursor, "_");
       }
       else {
	    strcat(text_with_cursor, " ");
       }
    	    
       stk_font_write(fnt,dest,x,y,alpha,text_with_cursor);
    
       free(text_with_cursor);
    }
}
