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
#include <devices/input.h>

#include "SDL_config.h"

#include "SDL_mouse.h"
#include "../../events/SDL_events_c.h"
#include "../SDL_pixels_c.h"
#include "../SDL_cursor_c.h"
#include "SDL_cgxmouse_c.h"

struct WMcursor
{
	UWORD*	image;
	WORD	width, height;
	WORD	offx, offy;
};

void CGX_FreeWMCursor(_THIS, WMcursor *cursor)
{
	D(bug("[SDL] CGX_FreeWMCursor()\n"));

	if (cursor)
	{
		if (SDL_Window)
			ClearPointer(SDL_Window);

		if (cursor->image)
			free(cursor->image);

		free(cursor);
	}
}

WMcursor *CGX_CreateWMCursor(_THIS, Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
	struct WMcursor *cursor = NULL;

	if (w <= 16 && (cursor = malloc(sizeof(*cursor))))
	{
		cursor->width  = w;
		cursor->height = h;
		cursor->offx   = -hot_x;
		cursor->offy   = -hot_y;

		if ((cursor->image = malloc(w*h)))
		{
			ULONG y;
			UWORD *p;

			p = cursor->image + 2;

			for (y = 0; y < h; y++)
			{
				if (w <= 8)
				{
					*p++ = (*mask++) << 8;
					*p++ = (*data++) << 8;
				}
				else
				{
					*p++ = mask[0] << 8 | mask[1];
					*p++ = data[0] << 8 | data[1];
					data += 2;
					mask += 2;
				}
			}
		}
		else
		{
			free(cursor);
			cursor = NULL;
		}
	}

	D(bug("[SDL] CGX_CreateWMCursor() (size %ld/%ld) -> 0x%08.8lx\n", w, h, (IPTR)cursor));

	return cursor;
}

int CGX_ShowWMCursor(_THIS, WMcursor *cursor)
{
	/* Don't do anything if the display is gone */
	if ( SDL_Display == NULL)
	{
		return(0);
	}

	/* Set the cursor, or blank if cursor is NULL */
	if ( SDL_Window )
	{
		SDL_Lock_EventThread();
		if ( cursor == NULL )
		{
			if ( SDL_BlankCursor != NULL )
			{
				// Hide cursor HERE
				SetPointer(SDL_Window,(UWORD *)SDL_BlankCursor,1,1,0,0);
			}
		}
		else
		{
			// Show cursor
			SetPointer(SDL_Window, cursor->image, cursor->height, cursor->width, cursor->offx, cursor->offy);
		}
		SDL_Unlock_EventThread();
	}
	return(1);
}

void CGX_WarpWMCursor(_THIS, Uint16 x, Uint16 y)
{
	D(bug("[SDL] CGX_WarpWMCursor(%ld, %ld)\n", x, y));
	
	if (this->hidden->window_active)
	{
		/*
			Note: code below may cause unwanted side effects
		 */
		struct IOStdReq *req;
		struct MsgPort *port;

		SDL_Lock_EventThread();

		port = CreateMsgPort();
		req  = (struct IOStdReq *)CreateIORequest(port, sizeof(*req));

		if (req)
		{
			if (OpenDevice("input.device", 0, (struct IORequest *)req, 0) == 0)
			{
				struct InputEvent *ie;
				struct IEPointerPixel *newpos;

				if ((ie = malloc(sizeof(*ie) + sizeof(*newpos))))
				{
					newpos = (struct IEPointerPixel *)(ie + 1);

					newpos->iepp_Screen = SDL_Display;
					newpos->iepp_Position.X = x + SDL_Window->BorderLeft + SDL_Window->LeftEdge;
					newpos->iepp_Position.Y = y + SDL_Window->BorderTop + SDL_Window->TopEdge;

					ie->ie_EventAddress = newpos;
					ie->ie_NextEvent    = NULL;
					ie->ie_Class        = IECLASS_NEWPOINTERPOS;
					ie->ie_SubClass     = IESUBCLASS_PIXEL;
					ie->ie_Code         = IECODE_NOBUTTON;
					ie->ie_Qualifier    = 0;

					req->io_Data    = ie;
					req->io_Length  = sizeof(*ie);
					req->io_Command = IND_WRITEEVENT;

					DoIO((struct IORequest *)req);
					free(ie);
				}
				CloseDevice((struct IORequest *)req);
			}
		}
		DeleteIORequest((struct IORequest *)req);
		DeleteMsgPort(port);
		
		SDL_Unlock_EventThread();
	}
}


void CGX_CheckMouseMode(_THIS)
{
	/* Check to see if we need to enter or leave mouse relative mode */
	/* under AROS the mouse mode is always absolute => nothing to be done*/
}
