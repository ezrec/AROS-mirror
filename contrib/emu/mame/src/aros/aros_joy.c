#include "driver.h"

#include <exec/types.h>

static struct JoystickInfo joylist[] =
{
	{ 0, 0, 0 }	/* end of table */
};

const struct JoystickInfo *osd_get_joy_list(void)
{
	return joylist;
}

int osd_is_joy_pressed(int joycode)
{
	return 0;
}

void osd_poll_joysticks(void)
{
}

/* return a value in the range -128 .. 128 (yes, 128, not 127) */
void osd_analogjoy_read(int player,int *analog_x, int *analog_y)
{
	*analog_x = *analog_y = 0;
}

int osd_joystick_needs_calibration (void)
{
	return 0;
}


void osd_joystick_start_calibration (void)
{
}

char *osd_joystick_calibrate_next (void)
{
	return NULL;
}

void osd_joystick_calibrate (void)
{
}

void osd_joystick_end_calibration (void)
{
}

void osd_trak_read(int player,int *deltax,int *deltay)
{
	*deltax = *deltay = 0;
}
