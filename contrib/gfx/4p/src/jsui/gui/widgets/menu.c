/*
 * menu widget
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


/*
    NOTE:  This is pretty much just a quick hack to get menus working
	    for the paint program.  I will be redoing them correctly
	    later on.  I'm not happy with this design yet.
 */

/* 

on mouse enter top bar, switch display to menu headings

on mouse drag  or click over menu item
    if popup:
	if root menu, display beneath item
	if submenu, display to the right of itemo

if mouse drag outside of topmost menu
    remove child node

on mouse release
    select entry under mouse (if any)
    remove children
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsui.h"

#define MENUPADDING (4)

int menupoll = 0;

int
menu_stub (
    int msg,
    JSUI_MENU * menu,
    int c
)
{   
    return JSUI_R_O_K;
}


typedef struct MENU_ACTIVE {
    int nelements;
    IPAGE * backbuffer;
    JSUI_MENU * menu;
    int x;
    int y;
    struct MENU_ACTIVE * next;
} MENU_ACTIVE;

MENU_ACTIVE * 
menu_add_child(
	MENU_ACTIVE * list,
	JSUI_MENU * newmenu,
	int x,
	int y
)
{
    MENU_ACTIVE * ma;
    JSUI_MENU * tm = newmenu;
    PEN * newpenBG = NULL;
    PEN * newpenFG = NULL;
    int maxw = 0;
    int yoffs = 0;

    if (!newmenu) return( list );

printf(" add child \n");
menupoll=1;

    // create and initialize the node
    ma = (MENU_ACTIVE *) malloc (sizeof(MENU_ACTIVE));
    ma->menu = newmenu;
    ma->x = x;
    ma->y = y;

    // initialize the nelements entry
    ma->nelements = 0;
    while(tm->text != NULL)
    {
	maxw = MAX(maxw, strlen(tm->text));
	ma->nelements++;
	tm++;
    }

printf(" %d elements %d %d \n", ma->nelements, x, y);

    // initalize the backup buffer
    ma->backbuffer = Page_Create(
		maxw*jsui_current_font->w + MENUPADDING*2 + 1, 
		ma->nelements * (jsui_current_font->h + MENUPADDING)
				+ MENUPADDING + 1,
		32);

    Page_Draw_Image( ma->backbuffer, 0, 0, jsui_screen, x,y, 
			x+ma->backbuffer->w, y+ma->backbuffer->h,
			DRAW_STYLE_MATTE);

    // draw the new menu to the screen
    primitive_3d_rect_down(jsui_screen, x, y, 
		x+ma->backbuffer->w, y+ma->backbuffer->h, JSUI_C_BG);

    newpenBG = Pen_Create();
    newpenBG->Icolor = JSUI_C_BG;
    newpenBG->text_type = TEXT_TYPE_JAM2;

    newpenFG = Pen_Create();
    newpenFG->Icolor = JSUI_C_FG;
    newpenFG->text_type = TEXT_TYPE_JAM2;


    yoffs = 1 + MENUPADDING;
    tm = newmenu;
    while(tm->text != NULL)
    {
	font_render_text( jsui_screen, newpenFG, newpenBG,
	    jsui_current_font, x+1+MENUPADDING, y+yoffs,
	    tm->text
	    );

	yoffs += jsui_current_font->h + MENUPADDING;
	tm++;
    }
    
    Pen_Destroy( newpenBG );
    Pen_Destroy( newpenFG );

    // add it to the list and return the list
    ma->next = list;
    return( ma );
}


MENU_ACTIVE *
menu_remove_child(
	MENU_ACTIVE * list
)
{
    MENU_ACTIVE * ma = list;

printf("remove child\n");
    if (!ma) return( NULL );

printf("blit back\n");
    // blit the buffer back out to the screen
    if (ma->backbuffer)
	Page_Draw_Image( jsui_screen, ma->x, ma->y,
			ma->backbuffer, 0 , 0,
			ma->backbuffer->w, ma->backbuffer->h,
			DRAW_STYLE_MATTE);

    // remove the entry off the list
    list = list->next;

    Page_Destroy( ma->backbuffer );
    free( ma );

    
    return( list );
}

void
menu_remove_children(
	MENU_ACTIVE * list
)
{
printf(" remove children \n");
    while (list)
    {
	list = menu_remove_child( list );
    }
    menupoll=0;
}


int jsui_widget_menu
	(int msg, void * vjdr, JSUI_DIALOG * d, int c)
{
    MENU_ACTIVE * am = NULL;
    PEN * newpenBG = NULL;
    PEN * newpenFG = NULL;
    JSUI_DIALOG_RUNNING * jdr = (JSUI_DIALOG_RUNNING *)vjdr;

    switch(msg)
    {
	case (JSUI_MSG_START):
	    if (jsui_screen && jdr)
	    {
		//setup the dimensions and rebuild the buffer
		if (jdr->buffer)
		    Page_Destroy( jdr->buffer );
		d->w = jsui_screen->w;
		if (jsui_current_font)
		    d->h = jsui_current_font->h + (MENUPADDING*2);
		else 
		    d->h = 12;

		jdr->x = 0;
		jdr->y = 0;
		jdr->vbx = 0;
		jdr->vby = 0;
		jdr->buffer = Page_Create( d->w, d->h, 32 );
	    }
	    //printf("MENU START\n");
	    break;

	case (JSUI_MSG_LPRESS):
	case (JSUI_MSG_RPRESS):
	    am = (MENU_ACTIVE *) d->dp2;
	    printf("MENU LPRESS\n");
	    break;

	case (JSUI_MSG_LDRAG):
	case (JSUI_MSG_RDRAG):
	    am = (MENU_ACTIVE *) d->dp2;
	    printf("MENU LDRAG\n");
	    break;

	case (JSUI_MSG_LRELEASE):
	case (JSUI_MSG_RRELEASE):
	    am = (MENU_ACTIVE *) d->dp2;
	    printf("MENU LRELEASE\n");
	    menu_remove_children(am);
	    d->dp2 = NULL;

	    if (jdr->my > d->h)
		jsui_dialog_send_message(d, JSUI_MSG_LOSTFOCUS, 0);
	    break;

	case (JSUI_MSG_GOTFOCUS):
	    am = (MENU_ACTIVE *) d->dp2;
	    printf("MENU GOTFOCUS\n");
	    d->flags |= JSUI_F_GOTFOCUS;

	    //d->dp2 = menu_add_child( am, (JSUI_MENU *) d->dp, 200, 200);

	    jsui_dialog_send_message(d, JSUI_MSG_DRAW, 0);
	    break;

	case (JSUI_MSG_LOSTFOCUS):
	    am = (MENU_ACTIVE *) d->dp2;
	    printf("MENU LOSTFOCUS\n");
	    d->flags &= ~JSUI_F_GOTFOCUS;
	    menu_remove_children(am);
	    d->dp2 = NULL;
	    jsui_dialog_send_message(d, JSUI_MSG_DRAW, 0);
	    break;

	case (JSUI_MSG_DRAW):
	    printf("MENU DRAW\n");
	    if (!jdr || !jdr->buffer)  break;
	    newpenBG = Pen_Create();
	    newpenBG->Icolor = JSUI_C_BG;
	    newpenBG->text_type = TEXT_TYPE_JAM2;

	    newpenFG = Pen_Create();
	    newpenFG->Icolor = JSUI_C_FG;
	    newpenFG->text_type = TEXT_TYPE_JAM2;

	    primitive_3d_rect_up( jdr->buffer, 0, 0, d->w-1, d->h-1, JSUI_C_BG );

	    if (d->flags & JSUI_F_GOTFOCUS)
	    {
		if (d->dp)
		{
		    int xpos = 1+MENUPADDING;
		    JSUI_MENU * jm = (JSUI_MENU *) d->dp;

		    while (jm->text != NULL)
		    {
			font_render_text( jdr->buffer, newpenFG, newpenBG,
			    jsui_current_font, xpos, MENUPADDING, 
			    jm->text
			    );

			xpos = xpos
				+ (strlen(jm->text) * jsui_current_font->w)
				+ MENUPADDING * 3;

			jm++;
		    }
		} else {
		    font_render_text( jdr->buffer, newpenFG, newpenBG,
			    jsui_current_font, MENUPADDING, MENUPADDING, 
			    "no menus defined"
			    );
		}
	    } else {
		font_render_text( jdr->buffer, newpenFG, newpenBG,
			jsui_current_font, 1+ MENUPADDING, MENUPADDING, 
			"UberPaint 4p"
			);

		font_render_text_center( jdr->buffer, newpenFG, newpenBG,
			jsui_current_font, 
			jdr->buffer->w/2, MENUPADDING, 
			"Color"
			);

		font_render_text_right( jdr->buffer, newpenFG, newpenBG,
			jsui_current_font, 
			jdr->buffer->w-MENUPADDING+1, MENUPADDING, 
			"jsl@umlautllama.com"
			);
	    }

	    Pen_Destroy(newpenFG);
	    Pen_Destroy(newpenBG);
	    break;
    }

    return JSUI_R_O_K;
}
