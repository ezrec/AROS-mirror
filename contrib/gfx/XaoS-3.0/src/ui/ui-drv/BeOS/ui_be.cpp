/*
 *     XaoS, a fast portable realtime fractal zoomer 
 *                  Copyright © 1996,1997 by
 *
 *      Jan Hubicka          (hubicka@paru.cas.cz)
 *      Thomas Marsh         (tmarsh@austin.ibm.com)
 *
 *	ui_be.cpp	BeOS user interface code, Jens Kilian (jjk@acm.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <cstdio>
#include <cstring>

#include <Application.h>
#include <Autolock.h>
#include <Bitmap.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <Point.h>
#include <Rect.h>
#include <Screen.h>

#include "XaoSWindow.h"
#include "XaoSView.h"
#include "XaoSEvent.h"

extern "C" {
#	include "fconfig.h"
#	include "ui.h"
}

static XaoSWindow *gpWindow;		// our BWindow
static XaoSView *gpView;			// our BView

const int NCOLORS = 256;			// # of colors in 8-bit mode
static color_space gColorSpace;	// color space for buffers
static BBitmap *gpBuffers[2] =	// fractal buffers
{
	0, 0									
};
static int gCurrentBuffer;

struct ui_driver be_driver;
static int color8bit = 0;			// parameters settable from command line
static int truecolor = 0;
static char *window_size = 0;
static int window_width = XSIZE;
static int window_height = YSIZE;

// Return display buffer size.
static void be_get_size(int *x, int *y)
{
	*x = window_width;
	*y = window_height;
}

// XaoS has a somewhat different notion of buttons than BeOS ...
static int map_be_buttons(uint32 beButtons, uint32 /*modifiers*/)
{
	const uint32 bothButtons = B_PRIMARY_MOUSE_BUTTON|B_SECONDARY_MOUSE_BUTTON;
	if ((beButtons & bothButtons) == bothButtons) {
		// Left + right button -> middle, for serial mice (still buggy in DR9).
		return BUTTON2;
	}
	
	return ((beButtons & B_PRIMARY_MOUSE_BUTTON) ? BUTTON1 : 0)
		  | ((beButtons & B_SECONDARY_MOUSE_BUTTON) ? BUTTON3 : 0)
		  | ((beButtons & B_TERTIARY_MOUSE_BUTTON) ? BUTTON2 : 0);
}

// Get next event from keyboard, mouse, etc.
static void be_processevents(int wait, int *x, int *y, int *buttons, int *k)
{
	static int currX = 0, currY = 0;
	static int currButtons = 0;
	static int cursorMask = 0;

	if (wait || port_count(gpView->EventPort()) > 0) {
		do {
			// Read events from our message port.
			int32 eventCode;
			XaoSEvent event;
			if (read_port(gpView->EventPort(), &eventCode, &event, sizeof(XaoSEvent))
				 < B_NO_ERROR) {
				 break;
			}
			
			// Handle event.
			switch (eventCode) {

			case XaoSEvent::KeyDown:
				switch (event.keyEvent.bytes[0]) {

				case B_LEFT_ARROW:
					cursorMask |= 1;
					break;
				
				case B_RIGHT_ARROW:
					cursorMask |= 2;
					break;
				
				case B_UP_ARROW:
					cursorMask |= 4;
					break;
				
				case B_DOWN_ARROW:
					cursorMask |= 8;
					break;

				default:
					if (event.keyEvent.numBytes == 1) {
						if (event.keyEvent.bytes[0] == B_F1_KEY) {
							event.keyEvent.bytes[0] = 'h';
						} else if (event.keyEvent.bytes[0] == B_ESCAPE) {
							event.keyEvent.bytes[0] = 'q';
						}
						if (ui_key(event.keyEvent.bytes[0]) == 2) {
							return;
						}
					}
					break;
				}
				break;

			case XaoSEvent::KeyUp:
				switch (event.keyEvent.bytes[0]) {

				case B_LEFT_ARROW:
					cursorMask &= ~1;
					break;
				
				case B_RIGHT_ARROW:
					cursorMask &= ~2;
					break;
				
				case B_UP_ARROW:
					cursorMask &= ~4;
					break;
				
				case B_DOWN_ARROW:
					cursorMask &= ~8;
					break;

				default:
					break;
				}
				break;

			case XaoSEvent::Mouse:
				currX = event.mouseEvent.x;
				currY = event.mouseEvent.y;
				currButtons = map_be_buttons(event.mouseEvent.buttons,
													   event.mouseEvent.modifiers);
				break;

			case XaoSEvent::Resize:
				{	BAutolock locker(gpWindow);
					BRect bounds = gpView->Bounds();
					window_width = bounds.Width() + 1;
					window_height = bounds.Height() + 1;
				}
				ui_resize();
				break;
			}
			
		} while (port_count(gpView->EventPort()) > 0);
	}
	
	*x = currX;
	*y = currY;
	*buttons = currButtons;
	*k = cursorMask;
}

// Read the mouse.
static void be_getmouse(int *x, int *y, int *buttons)
{
	BPoint mouseLocation;
	uint32 mouseButtons;
	{	BAutolock locker(gpWindow);
		gpView->GetMouse(&mouseLocation, &mouseButtons);
	}
	*x = (int)mouseLocation.x;
	*y = (int)mouseLocation.y;
	*buttons = map_be_buttons(mouseButtons, modifiers());
}

// Clear the window.
static void be_clrscr(void)
{
	BAutolock locker(gpWindow);
	gpView->Clear();	
}

// Set the cursor shape.
static void be_mousetype(int type)
{
      switch (type) {
      
      case NORMALMOUSE:
              be_app->SetCursor(B_HAND_CURSOR);
              break;
      
      case WAITMOUSE:
              {
                      static const char cursor[] = {  // Clock face.
                              16, 1, 8, 8,
                              0x07, 0xe0, 0x1f, 0xf8, 0x38, 0x1c, 0x71, 0x8e,
                              0x61, 0x86, 0xc1, 0x83, 0xc1, 0x83, 0xc1, 0x83,
                              0xc1, 0xc3, 0xc0, 0xe3, 0xc0, 0x63, 0x60, 0x06,
                              0x70, 0x0e, 0x38, 0x1c, 0x1f, 0xf8, 0x07, 0xe0,
                              0x07, 0xe0, 0x1f, 0xf8, 0x3f, 0xfc, 0x7f, 0xfe,
                              0x7f, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                              0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0xfe,
                              0x7f, 0xfe, 0x3f, 0xfc, 0x1f, 0xf8, 0x07, 0xe0
                      };
                      be_app->SetCursor(cursor);
              }
              break;
      
      case REPLAYMOUSE:
              {
                      static const char cursor[] = {  // Stylized tape deck.
                              16, 1, 8, 8,
                              0x38, 0x38, 0x44, 0x44, 0x82, 0x82, 0x82, 0x82,
                              0x82, 0x82, 0x44, 0x44, 0x3f, 0xf8, 0x00, 0x00,
                              0x01, 0x00, 0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x38, 0x38, 0x7c, 0x7c, 0xfe, 0xfe, 0xfe, 0xfe,
                              0xfe, 0xfe, 0x7c, 0x7c, 0x3f, 0xf8, 0x00, 0x00,
                              0x01, 0x00, 0x03, 0x80, 0x07, 0xc0, 0x07, 0xc0,
                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                      };
                      be_app->SetCursor(cursor);
              }
              break;
      }
}

// Display c-string <text> at x,y
static void be_print(int x, int y, char *text)
{
	BAutolock locker(gpWindow);
	gpView->DrawText(BPoint(x, y), text, strlen(text));
}

// Display pixel buffer from fractal engine to window.
static void be_display(void)
{
	BAutolock locker(gpWindow);
 	gpView->Draw(gpView->Bounds());
}

// Allocate fractal engine pixel buffers.
static int be_alloc_buffers(char **buffer1, char **buffer2)
{
	// We let the fractal engine draw directly into one of
	// two BBitmaps, and flip between them.
	for (int i = 0; i < 2; ++i) {
		gpBuffers[i] =
			new BBitmap(BRect(B_ORIGIN, BPoint(window_width-1, window_height-1)),
							gColorSpace);
	}
	gCurrentBuffer = 0;
	
	*buffer1 = (char *)(gpBuffers[0]->Bits());
	*buffer2 = (char *)(gpBuffers[1]->Bits());
	
	BAutolock locker(gpWindow);
	gpView->SetBuffer(gpBuffers[gCurrentBuffer]);
		
	return gpBuffers[0]->BytesPerRow();
}

// Deallocate fractal engine pixel buffers.
static void be_free_buffers(char * /*buffer1*/, char * /*buffer2*/)
{
	BAutolock locker(gpWindow);
	gpView->SetBuffer(0);

	for (int i = 0; i < 2; ++i) {
		delete gpBuffers[i];
		gpBuffers[i] = 0;
	}
}

// Switch current pixel buffer to copy from on next display update.
static void be_flip_buffers(void)
{
	gCurrentBuffer ^= 1;

	BAutolock locker(gpWindow);
	gpView->SetBuffer(gpBuffers[gCurrentBuffer]);
}

static void be_set_range(ui_palette palette, int start, int end)
{
	// Tell XaoS about our fixed color map.
	BScreen screen;
	const color_map *pColorMap = screen.ColorMap();
	
	for (int i = start; i < end; ++i) {
		palette[i-start][0] = pColorMap->color_list[i].red;
		palette[i-start][1] = pColorMap->color_list[i].green;
		palette[i-start][2] = pColorMap->color_list[i].blue;
	}
}

// Set up the driver.
static int be_init(void)
{
	// Evaluate parameters.
	if (window_size) {
		int width, height;
		sscanf(window_size, "%dx%d", &width, &height);
		if (width > 0) {
			window_width = width;
		}
		if (height > 0) {
			window_height = height;
		}
	}

	if ((gpView = new XaoSView(window_width, window_height)) != 0) {
		if ((gpWindow = new XaoSWindow(gpView)) != 0) {

			{	// Select the display mode, trying to optimize performance.
				BScreen screen;
				const color_space cs = screen.ColorSpace();

				const union {
					char c[4];
					int32 i;
				} test = { { 'B', 'e', 'O', 'S' } };
				const bool bigEndian = test.i == 'BeOS';

				if (!color8bit && !truecolor) {
					// User didn't specify; select bit depth based on current
					// workspace.
					if (cs == B_RGB_32_BIT || cs == B_BIG_RGB_32_BIT) {
						truecolor = 1;
					} else {
						color8bit = 1;
					}
				}
				
				// Determine display mode.
				if (color8bit) {
					gColorSpace = B_COLOR_8_BIT;
					be_driver.imagetype = UI_FIXEDCOLOR;

				} else if (cs == B_BIG_RGB_32_BIT) {
					gColorSpace = B_BIG_RGB_32_BIT;
					be_driver.imagetype =
						bigEndian ? UI_TRUECOLOR : UI_TRUECOLORMI;

				} else {
					gColorSpace = B_RGB_32_BIT;
					be_driver.imagetype =
						bigEndian ? UI_TRUECOLORMI : UI_TRUECOLOR;
				}
			}
			
			// Get text dimensions.
			float textwidth, textheight;
			gpView->GetTextSize(textwidth, textheight);
			be_driver.textwidth = textwidth;
			be_driver.textheight = textheight;
			
			gpWindow->Show();
			return 1;
		}
		delete gpView;
	}

	return 0;
}

// Deallocate any stuff we allocated at startup.
static void be_uninitialise()
{
	// Tell the window that it may quit, and to do so.
	gpWindow->AllowQuit();
	gpWindow->PostMessage(B_QUIT_REQUESTED);
}

static char *helptext[] =
{
    "BeOS DRIVER VERSION 1.7 for DR9 PR2    ",
    "=======================================",
    "Written by Jens Kilian <jjk@acm.org>   ",
    "                                       ",
    "Features specific to the BeOS driver:  ",
    "                                       ",
    "- Two display modes:                   ",
    "  o 8-bit, fixed palette (-8bit)       ",
    "  o 32-bit true color (-truecolor)     ",
    "                                       ",
    "  In 32-bit workspaces, -truecolor is  ",
    "  selected by default; otherwise, 8-bit",
    "  mode is used.                        ",
    "                                       ",
    "- Use double-click to drag the current ",
    "  image to another application (e.g.,  ",
    "  Rraster).                            ",
    "                                       ",
    "Restrictions:                          ",
    "                                       ",
    "- If you have a serial mouse, you must ",
    "  click the left + right mouse buttons ",
    "  together for panning (to emulate the ",
    "  broken middle one).                  ",
    "  This is due to a BeOS bug which has  ",
    "  STILL not been fixed in DR9.         ",
    "                                       ",
    "- Dragging an image to a Tracker window",
    "  to save the image doesn't work (yet).",
    "                                       ",
    "- Screen size computations assume a 17\"",
    "  monitor at resolution 1024x768.      ",
    "  Use command-line options for others. ",
    "                                       ",
    "- The icon still sucks. (Hint, hint.)  ",
    "                                       ",
    "  Remember - Don't Dream It, Be It.    ",
    "                                       "
};

#define UGLYTEXTSIZE (sizeof(helptext)/sizeof(char *))

static struct params params[] =
{
    { "-8bit", P_SWITCH, &color8bit, "8-bit palette mode" },
    { "-truecolor", P_SWITCH, &truecolor, "32-bit true color mode" },
    { "-size", P_STRING, &window_size, "set window size (WIDTHxHEIGHT)" },
    { NULL, 0, NULL, NULL }
};

struct ui_driver be_driver =
{
    "BeOS",
    be_init,
    be_get_size,
    be_processevents,
    be_getmouse,
    be_uninitialise,
    NULL,						// set_color()
    be_set_range,
    be_print,
    be_display,
    be_alloc_buffers,
    be_free_buffers,
    be_flip_buffers,
    be_clrscr,
    be_mousetype,
    NULL,						// be_driveropt, ### NOT IMPLEMENTED YET
    NULL,						// flush()
    0,							// text width (see be_init)
    0,							// text height (see be_init)
    helptext,					// help text, size
    UGLYTEXTSIZE,
    params,						// command-line parameters
    UPDATE_AFTER_PALETTE|PIXELSIZE|RESOLUTION,
    0.02979, 0.02930,		// this is for my monitor in 1024x768
    1024, 768,
    0,							// image type (see be_init)
    0, NCOLORS-1, NCOLORS
};
