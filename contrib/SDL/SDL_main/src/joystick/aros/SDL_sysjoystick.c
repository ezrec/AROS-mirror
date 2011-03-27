/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sam Lantinga
    slouken@libsdl.org

		20040518	SDL_SYS_JoystickOpen() function fixed
				SetJoyPortAttrs() function added to initialize port

		20040515	SDL_SYS_JoystickInit() function fixed
				SDL_SYS_JoystickName() function fixed
				SDL_SYS_JoystickQuit() function fixed
				joystick max number (2) removed
				PrivateJoyAxis 2nd parameters inverted!

		20040505	"inline" include replaced with "proto"

		20040501	LowlevelBase static declaration removed
				Debug lines added
*/


/* This is the system specific header for the SDL joystick API */

#include <stdio.h>		/* For the definition of NULL */

#include <libraries/lowlevel.h>
//#define NO_LOWLEVEL_EXT
#ifndef NO_LOWLEVEL_EXT
#include <libraries/lowlevel_ext.h>
#endif
#include <proto/exec.h>
#include <proto/lowlevel.h>
#include <proto/graphics.h>
#include "../../main/aros/mydebug.h"

/*
extern struct ExecBase *SysBase;
extern struct GfxBase *GfxBase;
*/

#include <stdlib.h>

#include "SDL_events.h"
#include "SDL_error.h"
#include "SDL_joystick.h"
#include "../SDL_sysjoystick.h"
#include "../SDL_joystick_c.h"

/* The maximum number of joysticks we'll detect */
#define MAX_JOYSTICKS 4 /* lowlevel.library is limited to 4 ports */

/* Directions/Axis differences */
#define MOS_PLUS  32767 /* was 127, changed by Henes (20040801) */
#define MOS_MINUS -32768 /* was -127 */

#ifndef JP_TYPE_ANALOGUE
#define JP_TYPE_ANALOGUE  (14<<28)	  /* port has analogue joystick  */
#define JP_XAXIS_MASK	(255<<0)	/* horizontal position */
#define JP_YAXIS_MASK	(255<<8)	/* vertical position */
#define JP_ANALOGUE_PORT_MAGIC (1<<16) /* port offset to force analogue readout */
#endif

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */

struct Library *LowLevelBase = NULL;

unsigned long joybut[] =
{
	JPF_BUTTON_RED,
	JPF_BUTTON_BLUE,
	JPF_BUTTON_YELLOW,
	JPF_BUTTON_GREEN,
	JPF_BUTTON_PLAY,
	JPF_BUTTON_FORWARD,
	JPF_BUTTON_REVERSE
/*
	JPF_JOY_UP,
	JPF_JOY_DOWN,
	JPF_JOY_LEFT,
	JPF_JOY_RIGHT
*/
};

struct joystick_hwdata
{
	ULONG joystate;
#ifndef NO_LOWLEVEL_EXT
	ULONG joystate_ext;
	ULONG supports_analog;
#endif
};

/* lowlevel portNumber 0 <-> 1 */
static int PortIndex(int index)
{
	switch(index)
	{
		case 0:
			return 1;
			break;

		case 1:
			return 0;
			break;

		default:
			break;
	}

	return index;
}

int SDL_SYS_JoystickInit(void)
{
	int numjoysticks = 0;
	unsigned long joyflag = 0L;

	D(bug("SDL_SYS_JoystickInit()\n"));

	if(!LowLevelBase)
	{
		if((LowLevelBase = OpenLibrary("lowlevel.library",37)))
		{
			numjoysticks = 0;
			while(numjoysticks < MAX_JOYSTICKS)
			{
				joyflag = ReadJoyPort(PortIndex(numjoysticks));

				if((joyflag&JP_TYPE_MASK) == JP_TYPE_NOTAVAIL)
				{
					break;
				}

				numjoysticks++;
			}			

			return numjoysticks;
		}
		else
		{
			/* failed to open lowlevel.library! */
			SDL_SetError("Unable to open lowlevel.library");
			return 0;
		}
	}
	else
	{
		return SDL_numjoysticks;
	}
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index)
{
	static char	name[64];
	unsigned long joyflag = 0L;

	D(bug("SDL_SYS_JoystickName()\n"));

	if(LowLevelBase)
	{
		memset(name, 0, sizeof(name));

		index = PortIndex(index);

		joyflag = ReadJoyPort(index);

		if((joyflag&JP_TYPE_MASK) == JP_TYPE_NOTAVAIL)
		{
			SDL_SetError("No joystick available with that index");

			return NULL;
		}

		if((joyflag&JP_TYPE_MASK) == JP_TYPE_GAMECTLR) sprintf(name, "Port %d is a Game Controller", index);
		if((joyflag&JP_TYPE_MASK) == JP_TYPE_MOUSE) sprintf(name, "Port %d is a Mouse", index);
		if((joyflag&JP_TYPE_MASK) == JP_TYPE_JOYSTK) sprintf(name, "Port %d is a Joystick", index);
		if((joyflag&JP_TYPE_MASK) == JP_TYPE_UNKNOWN) sprintf(name, "Port %d is an unknown device", index);
		
		return name;
	}
	else
	{
		SDL_SetError("No joystick available with that index");

		return NULL;
	}
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */

int SDL_SYS_JoystickOpen(SDL_Joystick *joystick)
{
	unsigned long temp = 0L;

	D(bug("SDL_SYS_JoystickOpen()"));
	D(bug("Opening joystick %ld\n", joystick->index));

	if(!LowLevelBase)
	{
		if(SDL_SYS_JoystickInit() < 1)
		{
			SDL_SetError("Initialize Joysticks first!");
			return -1;
		}
	}

	/* if(!(joystick->hwdata=malloc(sizeof(struct joystick_hwdata)))) */
	joystick->hwdata = (struct joystick_hwdata *) malloc(sizeof(*joystick->hwdata));
	if ( joystick->hwdata == NULL )
	{
		SDL_OutOfMemory();

		return -1;
	}
	memset(joystick->hwdata, 0, sizeof(*joystick->hwdata));

	SetJoyPortAttrs(PortIndex(joystick->index), SJA_Type, SJA_TYPE_GAMECTLR, TAG_END);

	temp = ReadJoyPort(PortIndex(joystick->index));

	if((temp & JP_TYPE_MASK)==JP_TYPE_GAMECTLR)
	{
		joystick->nbuttons = 7;
		joystick->nhats = 1;
	}
	else if((temp & JP_TYPE_MASK) == JP_TYPE_JOYSTK)
	{
		joystick->nbuttons = 3;
		joystick->nhats = 1;
	}
	else if((temp & JP_TYPE_MASK) == JP_TYPE_MOUSE)
	{
		joystick->nbuttons = 3;
		joystick->nhats = 0;
	}
	else if((temp & JP_TYPE_MASK) == JP_TYPE_UNKNOWN)
	{
		joystick->nbuttons = 3;
		joystick->nhats = 1;
	}
	else if((temp & JP_TYPE_MASK) == JP_TYPE_NOTAVAIL)
	{
		joystick->nbuttons = 0;
		joystick->nhats = 0;
	}

	joystick->nballs = 0;
	joystick->naxes = 2; /* FIXME: even for JP_TYPE_NOTAVAIL ? */
	joystick->hwdata->joystate = 0L;
#ifndef NO_LOWLEVEL_EXT
	joystick->hwdata->joystate_ext = 0L;

	if (LowLevelBase->lib_Version > 50 || (LowLevelBase->lib_Version >= 50 && LowLevelBase->lib_Revision >= 17))
		joystick->hwdata->supports_analog = 1;
#endif

	return 0;
}

/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */
void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
	ULONG data;
#ifndef NO_LOWLEVEL_EXT
	ULONG data_ext;
#endif
	int	i;
	
	D(bug("SDL_SYS_JoystickUpdate()\n"));

	if(!LowLevelBase)
	{
		if(SDL_SYS_JoystickInit() < 1)
		{
			SDL_SetError("Initialize Joysticks first!");

			return;
		}
	}

	data = ReadJoyPort(PortIndex(joystick->index));
#ifndef NO_LOWLEVEL_EXT
	if (joystick->hwdata->supports_analog)
		data_ext = ReadJoyPort(PortIndex(joystick->index) + JP_ANALOGUE_PORT_MAGIC);
#endif

	/* only send an event when something changed */

	/* hats */
	if((joystick->hwdata->joystate & JP_DIRECTION_MASK) != (data & JP_DIRECTION_MASK))
	{
		if(joystick->nhats)
		{
			Uint8 value_hat = SDL_HAT_CENTERED;

			if(data & JPF_JOY_DOWN)
			{
				value_hat |= SDL_HAT_DOWN;
			}
			else if(data & JPF_JOY_UP)
			{
				value_hat |= SDL_HAT_UP;
			}

			if(data & JPF_JOY_LEFT)
			{
				value_hat |= SDL_HAT_LEFT;
			}
			else if(data & JPF_JOY_RIGHT)
			{
				value_hat |= SDL_HAT_RIGHT;
			}

			SDL_PrivateJoystickHat(joystick, 0, value_hat);
		}
	}

	/* axes */
#ifndef NO_LOWLEVEL_EXT
	if (joystick->hwdata->supports_analog && data_ext & JP_TYPE_ANALOGUE)
	{
		if((joystick->hwdata->joystate_ext & JP_XAXIS_MASK) != (data_ext & JP_XAXIS_MASK))
		{
			Sint16 value;

			value = (data_ext & JP_XAXIS_MASK) * 2*32767 / 255 - 32767;
			SDL_PrivateJoystickAxis(joystick, 0, value);
		}

		if((joystick->hwdata->joystate_ext & JP_YAXIS_MASK) != (data_ext & JP_YAXIS_MASK))
		{
			Sint16 value;

			value = ((data_ext & JP_YAXIS_MASK)>>8) * 2*32767 / 255 - 32767;
			SDL_PrivateJoystickAxis(joystick, 1, value);
		}
	}
	else
#endif
	{
		if((joystick->hwdata->joystate & (JPF_JOY_DOWN|JPF_JOY_UP)) != (data & (JPF_JOY_DOWN|JPF_JOY_UP)))
		{
			Sint16 value;

			/* UP and DOWN direction */
			if(data & JPF_JOY_DOWN)
			{
				value = MOS_PLUS;
			}
			else if(data & JPF_JOY_UP)
			{
				value = MOS_MINUS;
			}
			else
			{
				value = 0;
			}

			SDL_PrivateJoystickAxis(joystick, 1, value);
		}

		if((joystick->hwdata->joystate & (JPF_JOY_LEFT|JPF_JOY_RIGHT)) != (data & (JPF_JOY_LEFT|JPF_JOY_RIGHT)))
		{
			Sint16 value;

			/* LEFT and RIGHT direction */
			if(data & JPF_JOY_LEFT)
			{
				value = MOS_MINUS;
			}
			else if(data & JPF_JOY_RIGHT)
			{
				value = MOS_PLUS;
			}
			else
			{
				value = 0;
			}

			SDL_PrivateJoystickAxis(joystick, 0, value);
		}
	}

	/* Joy buttons */
	for(i = 0; i < joystick->nbuttons; i++)
	{
		if( (data & joybut[i]) )
		{
			if(i == 1)
			{
#warning WTF?
				data &= ~(joybut[2]);
			}

			if(!(joystick->hwdata->joystate & joybut[i]))
			{
				SDL_PrivateJoystickButton(joystick, i, SDL_PRESSED);
			}
		}
		else if(joystick->hwdata->joystate & joybut[i])
		{
			SDL_PrivateJoystickButton(joystick, i, SDL_RELEASED);
		}
	}

	joystick->hwdata->joystate = data;
#ifndef NO_LOWLEVEL_EXT
	joystick->hwdata->joystate_ext = data_ext;
#endif
}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick)
{
	D(bug("SDL_SYS_JoystickClose()\n"));

	if(LowLevelBase)	/* ne to reinitialize */
	{
		SetJoyPortAttrs(PortIndex(joystick->index), SJA_Type, SJA_TYPE_AUTOSENSE, TAG_END);
	}

	if(joystick->hwdata)
	{
		free(joystick->hwdata);
	}

	return;
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
	D(bug("SDL_SYS_JoystickQuit()\n"));

	if(LowLevelBase)
	{
		CloseLibrary(LowLevelBase);
		LowLevelBase = NULL;
		SDL_numjoysticks = 0;
	}

	return;
}

