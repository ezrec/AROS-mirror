#include "driver.h"

#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/intuition.h>

#include "aros_input.h"
#include "aros_video.h"
#include "aros_main.h"

/************************************************************************************/

int use_mouse, joystick, use_hotrod;

BOOL keypressed[256];

/************************************************************************************/

static struct KeyboardInfo keylist[] =
{
	{ "A",			0x20,				KEYCODE_A },
	{ "B",			0x35,				KEYCODE_B },
	{ "C",			0x33,				KEYCODE_C },
	{ "D",			0x22,				KEYCODE_D },
	{ "E",			0x12,				KEYCODE_E },
	{ "F",			0x23,				KEYCODE_F },
	{ "G",			0x24,				KEYCODE_G },
	{ "H",			0x25,				KEYCODE_H },
	{ "I",			0x17,				KEYCODE_I },
	{ "J",			0x26,				KEYCODE_J },
	{ "K",			0x27,				KEYCODE_K },
	{ "L",			0x28,				KEYCODE_L },
	{ "M",			0x37,				KEYCODE_M },
	{ "N",			0x36,				KEYCODE_N },
	{ "O",			0x18,				KEYCODE_O },
	{ "P",			0x19,				KEYCODE_P },
	{ "Q",			0x10,				KEYCODE_Q },
	{ "R",			0x13,				KEYCODE_R },
	{ "S",			0x21,				KEYCODE_S },
	{ "T",			0x14,				KEYCODE_T },
	{ "U",			0x16,				KEYCODE_U },
	{ "V",			0x34,				KEYCODE_V },
	{ "W",			0x11,				KEYCODE_W },
	{ "X",			0x32,				KEYCODE_X },
	{ "Y",			0x31,				KEYCODE_Y },
	{ "Z",			0x15,				KEYCODE_Z },
	{ "0",			0x0A,				KEYCODE_0 },
	{ "1",			0x01,				KEYCODE_1 },
	{ "2",			0x02,				KEYCODE_2 },
	{ "3",			0x03,				KEYCODE_3 },
	{ "4",			0x04,				KEYCODE_4 },
	{ "5",			0x05,				KEYCODE_5 },
	{ "6",			0x06,				KEYCODE_6 },
	{ "7",			0x07,				KEYCODE_7 },
	{ "8",			0x08,				KEYCODE_8 },
	{ "9",			0x09,				KEYCODE_9 },
	{ "0 PAD",		0x0F,				KEYCODE_0_PAD },
	{ "1 PAD",		0x1D,				KEYCODE_1_PAD },
	{ "2 PAD",		0x1E,				KEYCODE_2_PAD },
	{ "3 PAD",		0x1F,				KEYCODE_3_PAD },
	{ "4 PAD",		0x2D,				KEYCODE_4_PAD },
	{ "5 PAD",		0x2E,				KEYCODE_5_PAD },
	{ "6 PAD",		0x2F,				KEYCODE_6_PAD },
	{ "7 PAD",		0x3D,				KEYCODE_7_PAD },
	{ "8 PAD",		0x3E,				KEYCODE_8_PAD },
	{ "9 PAD",		0x3F,				KEYCODE_9_PAD },
	{ "F1",			0x50,				KEYCODE_F1 },
	{ "F2",			0x51,				KEYCODE_F2 },
	{ "F3",			0x52,				KEYCODE_F3 },
	{ "F4",			0x53,				KEYCODE_F4 },
	{ "F5",			0x54,				KEYCODE_F5 },
	{ "F6",			0x55,				KEYCODE_F6 },
	{ "F7",			0x56,				KEYCODE_F7 },
	{ "F8",			0x57,				KEYCODE_F8 },
	{ "F9",			0x58,				KEYCODE_F9 },
	{ "F10",		0x59,				KEYCODE_F10 },
	{ "F11",		0x4B,				KEYCODE_F11 },
	{ "F12",		0x6F,				KEYCODE_F12 },
	{ "ESC",		0x45,				KEYCODE_ESC },
	{ "~",			0x00,				KEYCODE_TILDE },
	{ "-",          	0x0B,         		 	KEYCODE_MINUS },
	{ "=",          	0x0A,         			KEYCODE_EQUALS },
	{ "BKSPACE",		0x41,				KEYCODE_BACKSPACE },
	{ "TAB",		0x42,				KEYCODE_TAB },
	{ "[",          	0x1A,      			KEYCODE_OPENBRACE },
	{ "]",          	0x1B,     			KEYCODE_CLOSEBRACE },
	{ "ENTER",		0x44,				KEYCODE_ENTER },
	{ ";",         		0x29,          			KEYCODE_COLON },
	{ ":",          	0x2A,         			KEYCODE_QUOTE },
	{ "\\",         	0x0D,      			KEYCODE_BACKSLASH },
	{ "<",          	0x30,     			KEYCODE_BACKSLASH2 },
	{ ",",          	0x38,          			KEYCODE_COMMA },
	{ ".",          	0x39,          			KEYCODE_STOP },
	{ "/",          	0x3A,          			KEYCODE_SLASH },
	{ "SPACE",		0x40,				KEYCODE_SPACE },
	{ "INS",		0x47,				KEYCODE_INSERT },
	{ "DEL",		0x46,				KEYCODE_DEL },
	{ "HOME",		0x70,				KEYCODE_HOME },
	{ "END",		0x71,				KEYCODE_END },
	{ "PGUP",		0x48,				KEYCODE_PGUP },
	{ "PGDN",		0x49,				KEYCODE_PGDN },
	{ "LEFT",		0x4F,				KEYCODE_LEFT },
	{ "RIGHT",		0x4E,				KEYCODE_RIGHT },
	{ "UP",			0x4C,				KEYCODE_UP },
	{ "DOWN",		0x4D,				KEYCODE_DOWN },
	{ "/ PAD",      	0x5C,      			KEYCODE_SLASH_PAD },
	{ "* PAD",      	0x5D,       			KEYCODE_ASTERISK },
	{ "- PAD",     	 	0x4A,      			KEYCODE_MINUS_PAD },
	{ "+ PAD",     		0x5E,       			KEYCODE_PLUS_PAD },
	{ ". PAD",      	0x3C,        			KEYCODE_DEL_PAD },
	{ "ENTER PAD",  	0x43,      			KEYCODE_ENTER_PAD },
/*	{ "PRTSCR",     	0xFE,        			KEYCODE_PRTSCR },  AROSFIXME? */
	{ "PAUSE",     		0x6E,          			KEYCODE_PAUSE },
	{ "LSHIFT",		0x60,				KEYCODE_LSHIFT },
	{ "RSHIFT",		0x61,				KEYCODE_RSHIFT },
	{ "LCTRL",		0x63,				KEYCODE_LCONTROL },
	{ "RCTRL",		0x63,				KEYCODE_RCONTROL },
	{ "ALT",		0x64,				KEYCODE_LALT },
	{ "ALTGR",		0x65,				KEYCODE_RALT },
	{ "LWIN",		0x66,				KEYCODE_OTHER },
	{ "RWIN",		0x67,				KEYCODE_OTHER },
/*	{ "MENU",		KEY_MENU,			KEYCODE_OTHER },
	{ "SCRLOCK",    	KEY_SCRLOCK,        		KEYCODE_SCRLOCK },
	{ "NUMLOCK",   		KEY_NUMLOCK,        		KEYCODE_NUMLOCK },*/
	{ "CAPSLOCK",   	0x62,		       		KEYCODE_CAPSLOCK },
	{ 0, 0, 0 }	/* end of table */
};


/************************************************************************************/

/* return a list of all available keys */
const struct KeyboardInfo *osd_get_key_list(void)
{
	return keylist;
}


/************************************************************************************/

int osd_wait_keypress(void)
{
    struct IntuiMessage *msg;
    BOOL done = FALSE;

    if (win)
    {
        while(!done)
	{
            WaitPort(win->UserPort);
	    while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
	    {
	        if (msg->Class == IDCMP_RAWKEY) done = TRUE;
	        ReplyMsg((struct Message *)msg);
	    }
	}
    }
}

/************************************************************************************/

int osd_is_key_pressed(int keycode)
{
    return keypressed[keycode];
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

/************************************************************************************/

void osd_customize_inputport_defaults(struct ipd *defaults)
{
}

/************************************************************************************/

void AROS_Input_Update(void)
{
    struct IntuiMessage *msg;
    
    while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
        if (msg->Class == IDCMP_RAWKEY)
	{
	    WORD code = msg->Code & ~IECODE_UP_PREFIX;
	    
	    if (code == msg->Code)
	    {
	        keypressed[code] = TRUE;
	    } else {
	        keypressed[code] = FALSE;
	    }
	}
	
        ReplyMsg((struct Message *)msg);
    }
}

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

