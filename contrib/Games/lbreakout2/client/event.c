/***************************************************************************
                          event.c  -  description
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
#include "event.h"

extern SDL_Surface *stk_display;
extern SDL_Cursor *stk_empty_cursor;
extern SDL_Cursor *stk_standard_cursor;
extern Config config;
int keystate[SDLK_LAST];
int buttonstate[STK_BUTTON_COUNT];
int rel_motion = 0; /* relative mouse motion? */
int motion_x = 0, motion_y = 0; /* current position of mouse */
int motion_rel_x = 0; /* position of mouse relative to old position */
int motion = 0; /* motion occured? */
int motion_button = 0; /* button pressed while moving */
float motion_mod;
/* interna */
int intern_motion = 0; /* event_filter noted a motion so event_poll will set motion next time */
int intern_motion_rel_x = 0;
int intern_motion_x = 0, intern_motion_y = 0;
int intern_motion_last_x, intern_motion_last_y = 0;
int intern_motion_button = 0;
int intern_block_motion = 0;

/*
====================================================================
Event filter that blocks all events. Used to clear the SDL
event queue.
====================================================================
*/
int all_filter( const SDL_Event *event )
{
    return 0;
}

/*
====================================================================
Event filter used to get motion x.
====================================================================
*/
int event_filter( const SDL_Event *event )
{
    /* if this is a mousemotion event get the latest mouse position
    and set motion and motion_x and drop this event */
    /* event_poll does not return any motion events instead motion_x
    is used directly */
    if ( event->type == SDL_MOUSEMOTION ) {
        if ( intern_block_motion ) return 0;
        /* mouse warp to keep in window */
        if ( !intern_motion ) {
            intern_motion_last_x = intern_motion_x;
            intern_motion_last_y = intern_motion_y;
            intern_motion = 1;
            intern_motion_rel_x = event->motion.xrel;
        }
        else
            intern_motion_rel_x += event->motion.xrel;
        intern_motion_x = event->motion.x;
        intern_motion_y = event->motion.y;
        intern_motion_button = event->motion.state;
        return 0;
    }
    return 1;
}

/*
====================================================================
Reset event states
====================================================================
*/
void event_reset()
{
    memset( &keystate, 0, sizeof( int ) * SDLK_LAST );
    memset( &buttonstate, 0, sizeof( int ) * STK_BUTTON_COUNT );
    motion_mod = (float)(config.motion_mod) / 100;
    intern_motion = motion = 0;
}
/*
====================================================================
Grab or release input. Hide cursor if events are kept in window.
Use relative mouse motion and grab if config tells so.
====================================================================
*/
void event_grab_input()
{
    /* if rel_motion is set the global rel_motion_x is updated by event_poll()
    (internal value is always updated by event_filter()) */
    rel_motion = config.rel_motion;
    /* use empty cursor if fullscreen or grab is set */
    if ( config.grab || stk_display->flags & SDL_FULLSCREEN ) SDL_SetCursor( stk_empty_cursor );
    /* grab input if grab activated or rel_motion is set as we have to grab the input
    and hide the cursor if so */
    if ( config.grab || rel_motion ) SDL_WM_GrabInput( SDL_GRAB_ON );
    /* if rel_motion is set hide the cursor to get relative motion events even when
    cursor hits the border (due to grab) */
    if ( rel_motion ) SDL_ShowCursor( 0 );
}
void event_ungrab_input()
{
    /* show cursor, grab nothing */
    SDL_WM_GrabInput( SDL_GRAB_OFF );
    rel_motion = 0;
    SDL_ShowCursor( 1 ); SDL_SetCursor( stk_standard_cursor );
}
/*
====================================================================
Poll next event and set key and mousestate.
Return Value: True if event occured
====================================================================
*/
int event_poll( SDL_Event *event )
{
    motion = 0;
    /* check if motion occured: does not cause an event */
    if ( intern_motion ) {
        /* gather all motion events to one absolute/relative position until
        event_poll is called next time */
        intern_motion = 0;
        /* if either absolute position is used or relative motion occured set motion flag
        so paddle may move */
        if ( !rel_motion || intern_motion_rel_x ) motion = 1;
        /* absolute position */
        motion_x = intern_motion_x;
        motion_y = intern_motion_y;
        /* button */
        motion_button = intern_motion_button;
        /* use motion modifier and invert if wanted */
        if ( rel_motion && motion ) {
            motion_rel_x = (int)( motion_mod * intern_motion_rel_x );
            if ( config.invert ) motion_rel_x = -motion_rel_x;
#ifdef MOUSE_WARP
            /* reset mouse pointer as current solution does not work for
            BeOS */
            SDL_EventState( SDL_MOUSEMOTION, SDL_IGNORE );
            SDL_WarpMouse(stk_display->w >> 1, stk_display->h >> 1);
            SDL_EventState( SDL_MOUSEMOTION, SDL_ENABLE );
#endif
        }
    }
    /* check queued events */
    if ( SDL_PollEvent( event ) ) {
        switch ( event->type ) {
            case SDL_QUIT: return 1;
            case SDL_MOUSEBUTTONUP: buttonstate[event->button.button] = 0; return 1;
            case SDL_MOUSEBUTTONDOWN: buttonstate[event->button.button] = 1; return 1;
            case SDL_KEYUP: keystate[event->key.keysym.sym] = 0; return 1;
            case SDL_KEYDOWN: keystate[event->key.keysym.sym] = 1; return 1;
            default: return 1;
        }
    }
    else
        if ( motion ) {
            event->type = SDL_NOEVENT;
            return 1;
        }
    return 0;
}
/*
====================================================================
Block/unblock motion event
====================================================================
*/
void event_block_motion( int block )
{
    intern_block_motion = block;
}
/*
====================================================================
Clear the SDL event key (keydown events)
====================================================================
*/
void event_clear_sdl_queue()
{
    SDL_EventFilter old_filter;
    SDL_Event event;
    old_filter = SDL_GetEventFilter();
    SDL_SetEventFilter( all_filter );
    while ( SDL_PollEvent( &event ) );
    SDL_SetEventFilter( old_filter );
}
