/*
** uievents
**
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "SDL.h"
#include "jsui.h"
#include "active.h"

/* was q hit? */
int jsui_request_quit = 0;

int jsui_key_shift   = 0;
int jsui_key_control = 0;
int jsui_key_meta    = 0;

int buttonstates = 0;
int x = -1; // XXX Trying this out...
int y = -1;

filterEventsFallback _fallback = NULL;

int _jsui_reset_filter = 0;

////////////////////////////////////////////////////////////////////////////////
#define DO_FALLBACK(E,V,W,X) \
        if (_fallback)  _fallback((E),(V),(W),(X));

#define CHECKFORCLOSEANDRETURN(A) \
	if( ((A) == JSUI_R_CLOSE)  &&  jsr )\
	{\
	    jsui_dialog_close(jsr->dialog);\
	    jsr = NULL;\
	    return( JSUI_R_O_K );\
	} else {\
	    return( A );\
	}

int
jsui_filter_events(
	const SDL_Event *event
)
{
    static JSUI_DIALOG * dlg = NULL;
    static JSUI_DIALOG_RUNNING * jsr = NULL;
    static JSUI_DIALOG * olddlg = NULL;
    static int focus_obj = 0;
    static int old_mouse_x = -1;
    static int old_mouse_y = -1;
    int new_button = 0;
    int ret = JSUI_R_O_K;

    // this is a hack
    if (!event && _jsui_reset_filter == 1)
    {
        dlg = NULL;
        jsr = NULL;
        olddlg = NULL;
        focus_obj = 0;
        old_mouse_x = -1;
        old_mouse_y = -1;
        _jsui_reset_filter = 0;
        CHECKFORCLOSEANDRETURN (ret);
    }


    // this is also a hack, and i'm sorry about that, but this 
    // is the only way i can think of to do this right now.
    if (!event)
    {
	if (buttonstates)
	{
	    // make sure we don't have wandering boxes..
	    if (jsr && old_mouse_x >=0)
	    {
		jsr->mx = old_mouse_x - jsr->x - jsr->vbx;
		jsr->my = old_mouse_y - jsr->y - jsr->vby;
		jsr->mb = buttonstates;
	    }

	    if (buttonstates & 4)
	    {
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_LDRAG);
                else DO_FALLBACK(JSUI_MSG_LDRAG, x, y, buttonstates);
	    }
	    if (buttonstates & 2)
	    {
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_MDRAG); 
                else DO_FALLBACK(JSUI_MSG_MDRAG, x, y, buttonstates);
	    }
	    if (buttonstates & 1)
	    {
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_RDRAG); 
                else DO_FALLBACK(JSUI_MSG_RDRAG, x, y, buttonstates);
	    }
	} else {
	    // check for window if it was closed...
	    if (jsr)
		ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_IDLE); 
            else 
		DO_FALLBACK(JSUI_MSG_IDLE, x, y, 0);
	}

	// "We met before, but something tells me 
	//  this time you're going to remember me."

	CHECKFORCLOSEANDRETURN(ret);
    }

    switch (event->type)
    {
	// humans are sheep.
	// BAH!  BAH I TELL YOU!

    case (SDL_ACTIVEEVENT):
	// screen app events -- we should probably just ignore these...
#ifdef MAD_MONKEY_OUTPUT
        printf("%s ", event->active.gain ? "gained" : "lost");
	if ( event->active.state & SDL_APPACTIVE )
		printf("active");
	if ( event->active.state & SDL_APPMOUSEFOCUS )
		printf("mouse");
	if ( event->active.state & SDL_APPINPUTFOCUS )
		printf("input");
	printf(" focus\n");
#endif

	// See if we are iconified or restored
	if ( event->active.state & SDL_APPACTIVE ) {
#ifdef MAD_MONKEY_OUTPUT
		printf("App has been %s\n",
			event->active.gain ?  "restored" : "iconified");
#endif
	}
	CHECKFORCLOSEANDRETURN(0);

    case SDL_QUIT:
	// quit event
	//printf("QUIT\n");
	CHECKFORCLOSEANDRETURN(SDL_QUIT);


    case SDL_MOUSEBUTTONDOWN:
	// a button was just pressed down
        
        // if we're on a single-button system, like my shiny G4 laptop,
        // we will check to see if they've also got the apple key pressed
        // down.  if so, we convert the button to be a right-mouse button.
        if (jsui_key_meta)
            new_button = SDL_BUTTON_RIGHT;
        else
            new_button = event->button.button;
        
	if (new_button == SDL_BUTTON_LEFT)
	{
	    //printf("Left down\n");
	    buttonstates |= 4;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_LPRESS);
            else DO_FALLBACK(JSUI_MSG_LPRESS, x, y, buttonstates);
	} 
	if (new_button == SDL_BUTTON_MIDDLE) {
	    //printf("Middle down\n");
	    buttonstates |= 2;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_MPRESS); 
            else DO_FALLBACK(JSUI_MSG_MPRESS, x, y, buttonstates);
	} 
	if (new_button == SDL_BUTTON_RIGHT) {
	    //printf("Right down\n");
	    buttonstates |= 1;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_RPRESS); 
            else DO_FALLBACK(JSUI_MSG_RPRESS, x, y, buttonstates);
	}

	CHECKFORCLOSEANDRETURN(ret);

    case SDL_MOUSEBUTTONUP:
	// a button was just released
	if (event->button.button == SDL_BUTTON_LEFT)
	{
	    //printf("Left up\n");
	    buttonstates = 0; //&= ~4;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_LRELEASE);
            else DO_FALLBACK(JSUI_MSG_LRELEASE, x, y, buttonstates);
	} else
	if (event->button.button == SDL_BUTTON_MIDDLE) {
	    //printf("Middle up\n");
	    buttonstates = 0; //&= ~2;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_MRELEASE); 
            else DO_FALLBACK(JSUI_MSG_MRELEASE, x, y, buttonstates);
	} else
	if (event->button.button == SDL_BUTTON_RIGHT) {
	    //printf("Right up\n");
	    buttonstates = 0; //&= ~1;
	    if (jsr) jsr->mb = buttonstates;
	    if (jsr) ret = jsui_dialog_tell_message(
			jsr->dialog, jsr->focus_obj, JSUI_MSG_RRELEASE); 
            else DO_FALLBACK(JSUI_MSG_RRELEASE, x, y, buttonstates);
	}
	CHECKFORCLOSEANDRETURN(ret);


    case SDL_MOUSEMOTION:
	if (jsr)
	{
	    jsr->mx = event->motion.x - jsr->x - jsr->vbx;
	    jsr->my = event->motion.y - jsr->y - jsr->vby;
	    jsr->mb = buttonstates;
	}

	old_mouse_x = event->motion.x;
	old_mouse_y = event->motion.y;

	    x = event->motion.x;
	    y = event->motion.y;

	if (buttonstates)
	{
	    // just pass along mouse drag messages

	    // adjust for window placement
	    // send active widget the message
	    if (buttonstates & 4)
	    {
		//printf("left drag\n");
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_LDRAG); 
                else DO_FALLBACK(JSUI_MSG_LDRAG, x, y, buttonstates);
	    }
	    if (buttonstates & 2)
	    {
		//printf("middle drag\n");
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_MDRAG); 
                else DO_FALLBACK(JSUI_MSG_MDRAG, x, y, buttonstates);
	    }
	    if (buttonstates & 1)
	    {
		//printf("right drag\n");
		if (jsr) ret = jsui_dialog_tell_message(
		    jsr->dialog, jsr->focus_obj, JSUI_MSG_RDRAG); 
                else DO_FALLBACK(JSUI_MSG_RDRAG, x, y, buttonstates);
	    }
	} else {
	    // check for focus change
	    x = event->motion.x;
	    y = event->motion.y;

	    dlg = jsui_focus_find_active(x,y);
	    if (dlg != olddlg)
	    {
		if (olddlg)
		{
		    olddlg->flags &= ~JSUI_F_GOTFOCUS;
		    jsui_dialog_tell_message(olddlg, 0, JSUI_MSG_LOSTFOCUS);
		}

		if (dlg)
		{
		    dlg->flags |= JSUI_F_GOTFOCUS;
		    jsui_dialog_tell_message(dlg, 0, JSUI_MSG_GOTFOCUS);
		}
		olddlg = dlg;
	    }
	    jsr = jsui_dialog_find(dlg);

	    // check for dialog widget focus
	    if (jsr)
	    {
		jsr->mx = x - jsr->x - jsr->vbx;
		jsr->my = y - jsr->y - jsr->vby;
		jsr->mb = buttonstates;

		focus_obj = jsui_focus_find_widget(dlg, jsr->mx, jsr->my);

		if (focus_obj != jsr->focus_obj)
		{
		    if (jsr->focus_obj != 0)
		    {
			jsr->dialog[jsr->focus_obj].flags &= ~JSUI_F_GOTFOCUS;

			ret = jsui_dialog_tell_message(
				    jsr->dialog,
				    jsr->focus_obj,
				    JSUI_MSG_LOSTFOCUS); 
		    }

		    jsr->focus_obj = focus_obj;

		    if (jsr->focus_obj != 0)
		    {
			jsr->dialog[jsr->focus_obj].flags |= JSUI_F_GOTFOCUS;

			ret = jsui_dialog_tell_message(
				    jsr->dialog,
				    jsr->focus_obj,
				    JSUI_MSG_GOTFOCUS); 
		    }
		}
	    }
	}

    case SDL_KEYUP:
        if ( (event->key.keysym.sym == SDLK_LSHIFT) ||
                (event->key.keysym.sym == SDLK_RSHIFT) )
        {
            jsui_key_shift--;
            if (jsui_key_shift <0)  jsui_key_shift = 0;

        } else 

        if ( (event->key.keysym.sym == SDLK_LMETA) ||
                (event->key.keysym.sym == SDLK_RMETA) )
        {
            jsui_key_meta--;
        } else 

        if ( (event->key.keysym.sym == SDLK_LCTRL) ||
                (event->key.keysym.sym == SDLK_RCTRL) )
        {
            jsui_key_control--;
            if (jsui_key_control <0)  jsui_key_control = 0;
        } 
        
        CHECKFORCLOSEANDRETURN(ret);

    case SDL_KEYDOWN:
        if ( (event->key.keysym.sym == SDLK_LCTRL) ||
                (event->key.keysym.sym == SDLK_RCTRL) )
        {
            jsui_key_control++;
        } else 

        if ( (event->key.keysym.sym == SDLK_LSHIFT) ||
                (event->key.keysym.sym == SDLK_RSHIFT) )
        {
            jsui_key_shift++;
        } else 

        if ( (event->key.keysym.sym == SDLK_LMETA) ||
                (event->key.keysym.sym == SDLK_RMETA) )
        {
            jsui_key_meta++;
	} else {
            if (jsr) ret = jsui_dialog_tell_msginfo(
			    jsr->dialog,
			    jsr->focus_obj,  
			    JSUI_MSG_CHAR,
			    (0x7f & event->key.keysym.sym)
			);
	    else
	    switch (0x7f & event->key.keysym.sym)
	    {
	    case ('q'):
		jsui_request_quit++;
		break;
	    default:
		DO_FALLBACK(JSUI_MSG_CHAR, 0x7f & event->key.keysym.sym, 0, 0);
		break;
	    }
	}
        break;
	
    default:
	if (event->type >= JSUI_MSG_USER)
	{
	    DO_FALLBACK(event->type, 0, 0, 0);
	}
    
    }
        
//    if (event)
//    printf("Message not absorbed: %d\n", event->type);

    CHECKFORCLOSEANDRETURN(ret);
}
#undef DO_FALLBACK

