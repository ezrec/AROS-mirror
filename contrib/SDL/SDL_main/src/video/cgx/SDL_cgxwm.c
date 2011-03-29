/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_version.h"
#include "SDL_video.h"
#include "SDL_syswm.h"
#include "SDL_mouse.h"
#include "../../events/SDL_events_c.h"
#include "SDL_cgxmodes_c.h"
#include "SDL_cgxwm_c.h"
#include "SDL_cgxmouse_c.h"

void CGX_SetIcon(_THIS, SDL_Surface *icon, Uint8 *mask)
{
	/* Not yet implemented */
}

int CGX_IconifyWindow(_THIS)
{
	/* Not yet implemented */
	return 0;
}

void CGX_SetCaption(_THIS, const char *title, const char *icon)
{
	if(SDL_Window) SetWindowTitles(SDL_Window,(char *)title,NULL);
}

int CGX_GetWMInfo(_THIS, SDL_SysWMinfo *info)
{
	if ( info->version.major <= SDL_MAJOR_VERSION )
	{
		return(1);
	}
	else
	{
		SDL_SetError("Application not compiled with SDL %d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
		return(-1);
	}
}


SDL_GrabMode CGX_GrabWMInput(_THIS, SDL_GrabMode mode)
{
	D(bug("CGX_GrabWMInput()\n"));

	SDL_Lock_EventThread();
	
	if ( this->screen == NULL )	return(SDL_GRAB_OFF);
	if ( ! SDL_Window ) return(mode);	/* Will be set later on mode switch */

	switch(mode)
	{
		case SDL_GRAB_OFF:
			this->hidden->GrabMouse = 0;
			break;
		case SDL_GRAB_QUERY:
			mode = this->hidden->GrabMouse?SDL_GRAB_ON:SDL_GRAB_OFF;
			break;
		case SDL_GRAB_ON:
			if (!(this->screen->flags & SDL_FULLSCREEN))
				this->hidden->GrabMouse = 1;
			break;
		default:
			break;
	}

	SDL_Unlock_EventThread();
	return(mode);
}

