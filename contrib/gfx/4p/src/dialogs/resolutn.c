/*
** resolutn.c
**
**  2001 August 26
**
**  screen display resolution selector
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
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

#include "jsui.h"

int resd_videobpp   = 32;
int resd_videoflags = SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF; // | SDL_FULLSCREEN;
int resd_width      = 640;
int resd_height     = 480;


int resd_button_callback(JSUI_DIALOG * d, int object, int msg)
{
    if (!d) return( JSUI_R_O_K );

    if(msg != JSUI_MSG_LRELEASE) return( JSUI_R_O_K );

    switch(object)
    {
    case(3):  /* 320x200 */  resd_width = 320;   resd_height = 200;   break;
    case(4):  /* 320x240 */  resd_width = 320;   resd_height = 240;   break;
    case(5):  /* 360x240 */  resd_width = 360;   resd_height = 240;   break;
    case(6):  /* 640x400 */  resd_width = 640;   resd_height = 400;   break;
    case(7):  /* 640x480 */  resd_width = 640;   resd_height = 480;   break;
    case(8):  /* 720x480 */  resd_width = 720;   resd_height = 480;   break;
    case(9):  /* 800x600 */  resd_width = 800;   resd_height = 600;   break;
    case(10): /* 896x600 */  resd_width = 896;   resd_height = 600;   break;
    case(11): /* 1024x768 */ resd_width = 1024;  resd_height = 768;   break;
    case(12): /* 1152x768 */ resd_width = 1152;  resd_height = 768;   break;
    
    case(14): /* window */
        resd_videoflags = SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF;
        break;

    case(15): /* full screen */
        resd_videoflags = SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
        break;
        
    case(17): /* 8 bit */   /// resd_videobpp = 8;  break; //disabled
    case(18): /* 32 bit */  resd_videobpp = 32;  break;
        
    case(20):
	printf("CLOSE PLEASE\n");
	return( JSUI_R_CLOSE );
        break;
    }
    return JSUI_R_O_K;
}

JSUI_DIALOG resolution_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */
    {jsui_widget_frame, 0, 0, 355, 280, 0, 
            JSUI_FRAME_DRAGGABLE | JSUI_FRAME_TITLEBAR | JSUI_FRAME_CLOSE, 
	    JSUI_FRAME_CENTER,
            0, "Choose Screen Format", NULL, NULL  },

    {jsui_widget_text,  20, 26, 150, 0, 0, JSUI_F_NOFRAME, JSUI_T_CENTER, 0, "3:4", NULL, NULL},
    {jsui_widget_text, 185, 26, 150, 0, 0, JSUI_F_NOFRAME, JSUI_T_CENTER, 0, "16:9", NULL, NULL},
                
    /* 3 */
    {jsui_widget_button,  20,  40, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "320x200 2x", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  20,  60, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "320x240 2x", NULL, (void *)resd_button_callback},
    {jsui_widget_button, 185,  60, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "360x240 2x", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  20,  80, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "640x400", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  20, 100, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO|JSUI_B_ON, 2, "640x480", NULL, (void *)resd_button_callback},
    {jsui_widget_button, 185, 100, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "720x480", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  20, 120, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "800x600", NULL, (void *)resd_button_callback},
    {jsui_widget_button, 185, 120, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "896x600", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  20, 140, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "1024x768", NULL, (void *)resd_button_callback},
    {jsui_widget_button, 185, 140, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 2, "1152x768", NULL, (void *)resd_button_callback},

    {jsui_widget_3Dbox, 5, 10, 345, 160, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, JSUI_3_LOWERED, " Resolution ", NULL, NULL},

    /* 14 */
    {jsui_widget_button,  15, 195, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO|JSUI_B_ON, 3, "Window", NULL, (void *)resd_button_callback},
    {jsui_widget_button,  15, 215, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 3, "Full Screen", NULL, (void *)resd_button_callback},
    {jsui_widget_3Dbox, 5, 180, 170, 65, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, JSUI_3_LOWERED, " Display ", NULL, NULL},
    
    /* 17 */
    {jsui_widget_button, 190, 195, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO, 4, "8 bpp", NULL, (void *)resd_button_callback},
    {jsui_widget_button, 190, 215, 150, 18, 0, 0, JSUI_B_STICKY|JSUI_B_RADIO|JSUI_B_ON, 4, "32 bbp", NULL, (void *)resd_button_callback},
    {jsui_widget_3Dbox,  180, 180, 170, 65, 0, JSUI_F_NOFRAME, JSUI_T_LEFT, JSUI_3_LOWERED, " Depth ", NULL, NULL},

    /* 20 */
    {jsui_widget_button, 200, 255, 150, 18, 0, 0, 0, 4, "Ok", NULL, (void *)resd_button_callback},

    JSUI_DLG_END
};

int
run_resolution_selector(
        SDL_Surface * screen,
        Uint32 * new_width,
        Uint32 * new_height,
        Uint8  * new_videobpp,
        Uint32 * new_videoflags
)
{
    jsui_dialog_run_modal(resolution_dialog);
    
    while ( jsui_is_running(resolution_dialog) ) 
    {
	SDL_Delay(10);
	jsui_poll(screen);
    }
    
    *new_videobpp   = resd_videobpp;
    *new_videoflags = resd_videoflags;
    *new_width      = resd_width;
    *new_height     = resd_height;
    
    printf ("User selected %d x %d %s%dbpp %s\n", *new_width, *new_height, 
        (*new_width<=360)?"Doubled ":"",
        *new_videobpp,
        (*new_videoflags & SDL_FULLSCREEN)?"Full Screen":"Windowed"
    );

    return 1;
}
