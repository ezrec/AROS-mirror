/*
** main.c
**
**  1999 Dec 29 - 2000 Jan ??
**
**  code skeleton borrowed from the SDL demo code
**  bits of this will be parted out as needed.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsui.h"
#include "version.h"
#include "resolutn.h"
#include "menus.h"
#include "paint.h"
#include "splash.h"
#include "dialogs.h"


//#define SKIP_REQUEST_RESOLUTION_SPLASH (1)

SDL_Surface * screen = NULL;

void
my_exit(
	void
) 
{
    //jsui_upset();
    //SDL_Quit();

    //printf("\n%s    v%s (%s)  \"%s\"\n", "jsui", J_VERSION, J_REVISION, J_TPOTC);

}



void
request_resolution_screen(
        Uint32 * video_width,
        Uint32 * video_height,
        Uint8  * video_bpp,
        Uint32 * video_flags
)
{
    IPAGE * backing_page;
    
    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0 ) 
    {
	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }
//    atexit(my_exit);

    SDL_WM_SetCaption("UberPaint - 4P", "UberPaint");

 /* Initialize the display */
/*
    screen = SDL_SetVideoMode(640, 480, 32,
                SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF);
*/
screen = SDL_SetVideoMode(640, 480, 32,
                SDL_SWSURFACE);

    if ( screen == NULL )
    {
        fprintf(stderr,
                "Couldn't get a simple video screen: %s\n", 
SDL_GetError());
        exit(1);
    }

    if (jsui_setup(screen, NULL) < 0)
    {
	printf ("ERROR: Could not allocate a screen\n");
    }

    backing_page = generate_splash();
    
    jsui_active_background_page = backing_page;

    run_resolution_selector(screen, video_width, video_height, 
		video_bpp, video_flags);
    
    Page_Destroy(backing_page);
    
    jsui_upset();
    SDL_Quit();
}


/* this is to check to see if we're running under OSX libSDL. */
#if defined (__APPLE__) && defined (__MACH__)
int SDL_main(
#else
int main(
#endif
	int argc,
	char *argv[]
)
{
#ifdef SKIP_REQUEST_RESOLUTION_SPLASH
    printf("WARNING: SPLASH SCREEN IS DISABLED!\n");
    image_width = 320;
    image_height = 200;
    video_bpp = 32;
    video_flags = SDL_SWSURFACE;
 //   video_flags = SDL_SWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF;
#else
    request_resolution_screen(&image_width, &image_height, 
		&video_bpp, &video_flags);
#endif

    // auto scale the screen
    if (image_width < 640)
	video_width = image_width * 2;
    else
	video_width = image_width;

    if (image_height < 400)
	video_height = image_height * 2;
    else
	video_height = image_height;

    if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0 ) 
    {
	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }
//    atexit(my_exit);

    SDL_WM_SetCaption("4P", "4P");
    
    /*  now start up the user selected screen.... */
    
    screen = SDL_SetVideoMode(video_width, video_height, 
		    video_bpp, video_flags);
    if ( screen == NULL ) 
    {
	fprintf(stderr, "Couldn't set video mode (%dx%dx%d: %s\n", 
                    video_width, video_height, video_bpp, SDL_GetError());
	exit(1);
    }

    if (jsui_setup(screen, PaintFilterEvents) < 0)
    {
	printf ("ERROR: Could not allocate a screen\n");
    }

    paint_init();
    paint_update();

    (void)do_acceptor(screen, "Uberpaint: 4P", 
		"This is beta software.", 
		"It will probably crash on you.");

/*  ***JJJJJJJ XXXXXXXXX
  // test out the palette editor dialog
    jsui_dialog_run_modal( palette_edit_dialog );
*/

    /* Loop, waiting for QUIT */
    while ( jsui_request_quit < 2 )
    {
	SDL_Delay(50);
        paint_update();
	jsui_poll(screen);
    }

    jsui_upset();
    paint_deinit();
    SDL_Quit();

    exit(0);
}

