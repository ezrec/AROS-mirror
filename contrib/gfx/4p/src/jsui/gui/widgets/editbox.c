/*
 * text edit box widget
 * 
 */

/*
 * JSUI - Jerry's SDL User Interface
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

#include <string.h>
#include <stdlib.h>
#if !defined (__APPLE__) && !defined (__MACH__)
//#include <malloc.h>
#endif

#include <ctype.h>
#include "jsui.h"


int jsui_widget_edit		
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    PEN * newpen;
    int nvis;
    int x,y,w,h;
    int xx;
    char *a,*b;
    char * dbuff;
    int * hd = NULL;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    // dp char buffer
    // dp2 pointer to int array:
    //	   [0] position of current starting position
    // d1 max characters
    // d2 cursor position  in the string

    switch(msg)
    {
    case(JSUI_MSG_START):
	// alloc extras
	if (!d->dp2)
	{
	    hd = (int *) malloc( sizeof(int) * 1 );
	    hd[0] = 0;
	    d->dp2 = (void *)hd;
	}

	d->d2 = 0;
	    
	break;

    case(JSUI_MSG_END):
	// free extras
	if (d->dp2)  free( d->dp2 );
	d->dp2 = NULL;
	break;

    case(JSUI_MSG_LPRESS):
    case(JSUI_MSG_LDRAG):
	// move cursor
	hd = (int *) d->dp2;
	
	xx = (jdr->mx - d->x - 2) / jsui_current_font->w ;
	d->d2 = xx + hd[0];

	if (d->d2 > strlen(d->dp)) d->d2 = strlen(d->dp);

	jsui_widget_edit(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_LRELEASE):
	// do nothing
	break;

    case(JSUI_MSG_GOTFOCUS):
	// show cursor
	// SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	//SDL_EnableKeyRepeat(10, 20);
	jsui_widget_edit(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_LOSTFOCUS):
	// SDL_EnableKeyRepeat(0, 0);
	// show open cursor
	jsui_widget_edit(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_CHAR):
	// accept the key
	// return does nothing
	// delete removes character under cursor
	// backspace moved cursor back 1, deletes
	// arrow keys move it around
	if (jsui_key_shift)
	{
	    if(isalpha(c))  c = toupper(c); else 
	    if(isdigit(c))  c = ")!@#$%^&*("[c - '0']; else
	    if(c == '`')    c = '~'; else
	    if(c == '-')    c = '-'; else
	    if(c == '=')    c = '+'; else
	    if(c == '[')    c = '{'; else
	    if(c == ']')    c = '}'; else
	    if(c == '\\')   c = '|'; else
	    if(c == ';')    c = ':'; else
	    if(c == '\'')   c = '\"'; else
	    if(c == ',')    c = '<'; else
	    if(c == '.')    c = '>'; else
	    if(c == '/')    c = '?';
	}

	switch( c )
	{
	    case( SDLK_BACKSPACE ):
		if (d->d2 == 0) break;
		// move the cursor back one space
		// then delete a character.
		d->d2--;

	    case( SDLK_DELETE ):
		if (d->d2 >= strlen(d->dp))  break;
		// remove the character under the cursor
		// shift the string to the left.

		a = (char *)d->dp;
		b = (char *)d->dp;

		a += d->d2;
		b += (d->d2+1);

		while (*a != '\0')
		{
		    *a = *b;
		    a++; b++;
		}
		break;

	    case( SDLK_HOME ):
	    case( 0x16 ):
		// move the cursor to the beginning of the string
		d->d2 = 0;
		break;

	    case( SDLK_END ):
	    case( 0x17 ):
		// move the cursor to the end of the string
		d->d2 = strlen(d->dp);
		break;

	    case( SDLK_LEFT ):
	    case( 0x14 ):
		// move the cursor left one character, if possible
		d->d2--;
		if (d->d2 < 0)  d->d2 = 0;
		break;

	    case( SDLK_RIGHT ):
	    case( 0x13 ):
		// move the cursor right one character, if possible
		d->d2++;
		if (d->d2 > strlen(d->dp))  d->d2 = strlen(d->dp);
		break;

	    // all sorts of keys that we will ignore...
		// f-keys, return, pageup, pagedown, 
	    case(0x1a): case(0x1b): case(0x1c): case(0x1d): case(0x1e):
	    case(0x1f): case('\t'): case('\n'):
	    case(0x11): case(0x12): case(0x0d): case(0x18): case(0x19):
		break;

	    default:
		// editable characters...
		if (strlen(d->dp) >= (d->d1)-2)  break;

		// shift the string to the right for insertion
		a = (char *)d->dp;
		b = (char *)d->dp;

		a = a + strlen(d->dp);
		b = b + strlen(d->dp) + 1;

		while (a != ((char*)d->dp + d->d2 - 1))
		{
		    *b = *a;
		    a--; b--;
		} 

		// insert the new character
		a = (char *) d->dp;
		a += d->d2;
		*a = c;
		d->d2++;

		break;
	}
	jsui_widget_edit(JSUI_MSG_DRAW, vjdr, d, c);
	break;

    case(JSUI_MSG_DRAW):
	if (!d->dp) return JSUI_R_O_K;
	w = strlen(d->dp) * jsui_current_font->w;
	h = jsui_current_font->h;

	newpen = Pen_Create();

	// setup some helpers...
	y = jdr->vby + d->y + (d->h/2) - h/2;
	x = jdr->vbx + d->x + 2;

	hd = (int *) d->dp2;

	nvis = ( (d->w - 4) / jsui_current_font->w);
	dbuff = (char *) malloc( sizeof(char) * nvis +1 );

	// cursor past right
	while (d->d2 >= hd[0] + nvis)
	{
	    hd[0]++;
	}

	// cursor past left
	while (d->d2 < hd[0])
	{
	    hd[0]--;
	}

	xx = d->d2 - hd[0];

	// generate the buffer we'll use
	a = (char *)d->dp;
	a += hd[0];

	strncpy(dbuff, a, nvis);
	dbuff[nvis-1] = '\0';

	// actually draw some stuff
	primitive_3d_rect_down(jdr->buffer, 
	    jdr->vbx + d->x, jdr->vby + d->y, 
	    jdr->vbx + d->x + d->w-1,
	    jdr->vby + d->y + d->h-1,
	    JSUI_C_BG);

	newpen->Icolor = JSUI_C_CURSOR;
	if (d->flags & JSUI_F_GOTFOCUS)
	{
	    primitive_rectangle_filled(
		jdr->buffer, newpen, 
		x + 1 + (xx * jsui_current_font->w),
		y-2,
		x + 1 + ((1+xx) * jsui_current_font->w),
		y + 1 + jsui_current_font->h
	    );
	} else {
	    primitive_rectangle_hollow(
		jdr->buffer, newpen, primitive_pixel,
		x + 1 + (xx * jsui_current_font->w),
		y-2,
		x + 1 + ((1+xx) * jsui_current_font->w),
		y + 1 + jsui_current_font->h
	    );
	}

	newpen->Icolor = JSUI_C_FG;

	// need to adjust the sting to fit the window.
	font_render_text(jdr->buffer, newpen, newpen, jsui_current_font,
			x+2, y, dbuff);

	free( dbuff );
	Pen_Destroy(newpen);

	jdr->flags |= JSUI_F_DIRTY;
    }
    return JSUI_R_O_K;
}
