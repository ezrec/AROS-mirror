/* AROS: This file replaces src/input.c which uses CLOCKS_PER_SEC and
   clock() which is not (yet) defined/working under AROS. */

/******************************************************************************

  input.c

  Handle input from the user - keyboard, joystick, etc.

******************************************************************************/

#include "driver.h"

#ifndef AROS

#include <time.h>

#else

#include <exec/types.h>
#include "aros_misc.h"

#define CLOCKS_PER_SEC TICKS_PER_SEC
#define clock() AROS_Ticker()
#define clock_t TICKER
#endif

/***************************************************************************/
/* Codes */

/* Subtype of codes */
#define CODE_TYPE_NONE 0U /* code not assigned */
#define CODE_TYPE_KEYBOARD_OS 1U /* os depend code */
#define CODE_TYPE_KEYBOARD_STANDARD 2U /* standard code */
#define CODE_TYPE_JOYSTICK_OS 3U /* os depend code */
#define CODE_TYPE_JOYSTICK_STANDARD 4U /* standard code */

/* Informations for every input code */
struct code_info {
	int memory; /* boolean memory */
	unsigned oscode; /* osdepend code */
	unsigned type; /* subtype */
};

/* Main code table, generic KEYCODE_*, JOYCODE_* are index in this table */
static struct code_info* code_map;

/* Element in the table */
static unsigned code_mac;

/* Create the code table */
int code_init(void)
{
	unsigned i;

	/* allocate */
	code_map = (struct code_info*)malloc( __code_max * sizeof(struct code_info) );
	if (!code_map)
		return -1;

	code_mac = 0;

	/* insert all known codes */
	for(i=0;i<__code_max;++i)
	{
		code_map[code_mac].memory = 0;
		code_map[code_mac].oscode = 0; /* not used */

		if (__code_key_first <= i && i <= __code_key_last)
			code_map[code_mac].type = CODE_TYPE_KEYBOARD_STANDARD;
		else if (__code_joy_first <= i && i <= __code_joy_last)
			code_map[code_mac].type = CODE_TYPE_JOYSTICK_STANDARD;
		else
			code_map[code_mac].type = CODE_TYPE_NONE; /* never happen */
		++code_mac;
	}

	return 0;
}

/* Delete the code table */
void code_close(void)
{
	code_mac = 0;
	free(code_map);
}

/* Find the OSD record of a specific standard oscode */
INLINE const struct KeyboardInfo* internal_code_find_keyboard_standard_os(unsigned oscode)
{
	const struct KeyboardInfo *keyinfo;
	keyinfo = osd_get_key_list();
	while (keyinfo->name)
	{
		if (keyinfo->code == oscode && keyinfo->standardcode != CODE_OTHER)
			return keyinfo;
		++keyinfo;
	}
	return 0;
}

INLINE const struct JoystickInfo* internal_code_find_joystick_standard_os(unsigned oscode)
{
	const struct JoystickInfo *joyinfo;
	joyinfo = osd_get_joy_list();
	while (joyinfo->name)
	{
		if (joyinfo->code == oscode && joyinfo->standardcode != CODE_OTHER)
			return joyinfo;
		++joyinfo;
	}
	return 0;
}

/* Find a osdepend code in the table */
static int code_find_os(unsigned oscode, unsigned type)
{
	unsigned i;
	const struct KeyboardInfo *keyinfo;
	const struct JoystickInfo *joyinfo;

	/* Search on the main table */
	for(i=__code_max;i<code_mac;++i)
		if (code_map[i].type == type && code_map[i].oscode == oscode)
			return i;

	/* Search in the OSD tables for a standard code */
	switch (type)
	{
		case CODE_TYPE_KEYBOARD_OS :
			keyinfo = internal_code_find_keyboard_standard_os(oscode);
			if (keyinfo)
				return keyinfo->standardcode;
			break;
		case CODE_TYPE_JOYSTICK_OS :
			joyinfo = internal_code_find_joystick_standard_os(oscode);
			if (joyinfo)
				return joyinfo->standardcode;
			break;
	}

	/* os code not found */
	return CODE_NONE;
}

/* Add a new osdepend code in the table */
static void code_add_os(unsigned oscode, unsigned type)
{
	struct code_info* new_code_map;
	new_code_map = realloc( code_map, (code_mac+1) * sizeof(struct code_info) );
	if (new_code_map)
	{
		code_map = new_code_map;
		code_map[code_mac].memory = 0;
		code_map[code_mac].oscode = oscode;
		code_map[code_mac].type = type;
		++code_mac;
	}
}

/* Find the record of a specific code type */

INLINE const struct KeyboardInfo* internal_code_find_keyboard_standard(unsigned code)
{
	const struct KeyboardInfo *keyinfo;
	keyinfo = osd_get_key_list();
	while (keyinfo->name)
	{
		if (keyinfo->standardcode == code)
			return keyinfo;
		++keyinfo;
	}
	return 0;
}

INLINE const struct KeyboardInfo* internal_code_find_keyboard_os(unsigned code)
{
	const struct KeyboardInfo *keyinfo;
	keyinfo = osd_get_key_list();
	while (keyinfo->name)
	{
		if (keyinfo->standardcode == CODE_OTHER && keyinfo->code == code_map[code].oscode)
			return keyinfo;
      		++keyinfo;
	}
	return 0;
}

INLINE const struct JoystickInfo* internal_code_find_joystick_standard(unsigned code)
{
	const struct JoystickInfo *joyinfo;
	joyinfo = osd_get_joy_list();
	while (joyinfo->name)
	{
		if (joyinfo->standardcode == code)
			return joyinfo;
		++joyinfo;
	}
	return 0;
}

INLINE const struct JoystickInfo* internal_code_find_joystick_os(unsigned code)
{
	const struct JoystickInfo *joyinfo;
	joyinfo = osd_get_joy_list();
	while (joyinfo->name)
	{
		if (joyinfo->standardcode == CODE_OTHER && joyinfo->code == code_map[code].oscode)
			return joyinfo;
		++joyinfo;
	}
	return 0;
}

/* Check if a code is pressed */
static int internal_code_pressed(unsigned code)
{
	const struct KeyboardInfo *keyinfo;
	const struct JoystickInfo *joyinfo;
	switch (code_map[code].type)
	{
		case CODE_TYPE_KEYBOARD_STANDARD :
			keyinfo = internal_code_find_keyboard_standard(code);
			if (keyinfo)
				return osd_is_key_pressed(keyinfo->code);
			break;
		case CODE_TYPE_KEYBOARD_OS :
			keyinfo = internal_code_find_keyboard_os(code);
			if (keyinfo)
				return osd_is_key_pressed(keyinfo->code);
			break;
		case CODE_TYPE_JOYSTICK_STANDARD :
			joyinfo = internal_code_find_joystick_standard(code);
			if (joyinfo)
				return osd_is_joy_pressed(joyinfo->code);
			break;
		case CODE_TYPE_JOYSTICK_OS :
			joyinfo = internal_code_find_joystick_os(code);
			if (joyinfo)
				return osd_is_joy_pressed(joyinfo->code);
			break;
	}
	return 0;
}

/* Return the name of the code */
INLINE const char* internal_code_name(unsigned code)
{
	const struct KeyboardInfo *keyinfo;
	const struct JoystickInfo *joyinfo;
	switch (code_map[code].type)
	{
		case CODE_TYPE_KEYBOARD_STANDARD :
			keyinfo = internal_code_find_keyboard_standard(code);
			if (keyinfo)
				return keyinfo->name;
			break;
		case CODE_TYPE_KEYBOARD_OS :
			keyinfo = internal_code_find_keyboard_os(code);
			if (keyinfo)
				return keyinfo->name;
			break;
		case CODE_TYPE_JOYSTICK_STANDARD :
			joyinfo = internal_code_find_joystick_standard(code);
			if (joyinfo)
				return joyinfo->name;
			break;
		case CODE_TYPE_JOYSTICK_OS :
			joyinfo = internal_code_find_joystick_os(code);
			if (joyinfo)
				return joyinfo->name;
			break;
	}
	return "n/a";
}

/* Update the code table */
static void internal_code_update(void)
{
	const struct KeyboardInfo *keyinfo;
	const struct JoystickInfo *joyinfo;

	/* add only osdepend code because all standard codes are already present */

	keyinfo = osd_get_key_list();
	while (keyinfo->name)
	{
		if (keyinfo->standardcode == CODE_OTHER)
			if (code_find_os(keyinfo->code,CODE_TYPE_KEYBOARD_OS) == CODE_NONE)
				code_add_os(keyinfo->code,CODE_TYPE_KEYBOARD_OS);
		++keyinfo;
	}

	joyinfo = osd_get_joy_list();
	while (joyinfo->name)
	{
		if (joyinfo->standardcode == CODE_OTHER)
                        if (code_find_os(joyinfo->code,CODE_TYPE_JOYSTICK_OS)==CODE_NONE)
				code_add_os(joyinfo->code,CODE_TYPE_JOYSTICK_OS);
		++joyinfo;
	}
}

/***************************************************************************/
/* Save support */

/* Flags used in saving codes to file */
#define SAVECODE_FLAGS_TYPE_NONE        0x00000000
#define SAVECODE_FLAGS_TYPE_STANDARD    0x10000000 /* standard code */
#define SAVECODE_FLAGS_TYPE_KEYBOARD_OS 0x20000000 /* keyboard os depend code */
#define SAVECODE_FLAGS_TYPE_JOYSTICK_OS 0x30000000 /* joystick os depend code */
#define SAVECODE_FLAGS_TYPE_MASK        0xF0000000

/* Convert one key osdepend code to one standard code */
InputCode keyoscode_to_code(unsigned oscode)
{
	InputCode code;

	if (oscode == OSD_KEY_NONE)
		return CODE_NONE;

	code = code_find_os(oscode,CODE_TYPE_KEYBOARD_OS);

	/* insert if missing */
	if (code == CODE_NONE)
	{
		code_add_os(oscode,CODE_TYPE_KEYBOARD_OS);
		/* this fail only if the realloc call in code_add_os fail */
		code = code_find_os(oscode,CODE_TYPE_KEYBOARD_OS);
	}

	return code;
}

/* Convert one joystick osdepend code to one code */
InputCode joyoscode_to_code(unsigned oscode)
{
	InputCode code = code_find_os(oscode,CODE_TYPE_JOYSTICK_OS);

	/* insert if missing */
	if (code == CODE_NONE)
	{
		code_add_os(oscode,CODE_TYPE_JOYSTICK_OS);
		/* this fail only if the realloc call in code_add_os fail */
		code = code_find_os(oscode,CODE_TYPE_JOYSTICK_OS);
	}

	return code;
}

/* Convert one saved code to one code */
InputCode savecode_to_code(unsigned savecode)
{
	unsigned type = savecode & SAVECODE_FLAGS_TYPE_MASK;
	unsigned code = savecode & ~SAVECODE_FLAGS_TYPE_MASK;

	switch (type)
	{
		case SAVECODE_FLAGS_TYPE_STANDARD :
			return code;
		case SAVECODE_FLAGS_TYPE_KEYBOARD_OS :
			return keyoscode_to_code(code);
		case SAVECODE_FLAGS_TYPE_JOYSTICK_OS :
			return joyoscode_to_code(code);
	}

	/* never happen */

	return CODE_NONE;
}

/* Convert one code to one saved code */
unsigned code_to_savecode(InputCode code)
{
	if (code < __code_max || code >= code_mac)
               	/* if greather than code_mac is a special CODE like CODE_OR */
		return code | SAVECODE_FLAGS_TYPE_STANDARD;

	switch (code_map[code].type)
	{
		case CODE_TYPE_KEYBOARD_OS : return code_map[code].oscode | SAVECODE_FLAGS_TYPE_KEYBOARD_OS;
		case CODE_TYPE_JOYSTICK_OS : return code_map[code].oscode | SAVECODE_FLAGS_TYPE_JOYSTICK_OS;
	}

	/* never happen */

	return 0;
}

/***************************************************************************/
/* Interface */

const char *code_name(InputCode code)
{
	if (code < code_mac)
		return internal_code_name(code);

	switch (code)
	{
		case CODE_NONE : return "None";
		case CODE_NOT : return "not";
		case CODE_OR : return "or";
	}

	return "n/a";
}

int code_pressed(InputCode code)
{
	int pressed;

	profiler_mark(PROFILER_INPUT);

	pressed = internal_code_pressed(code);

	profiler_mark(PROFILER_END);

	return pressed;
}

int code_pressed_memory(InputCode code)
{
	int pressed;

	profiler_mark(PROFILER_INPUT);

	pressed = internal_code_pressed(code);

	if (pressed)
	{
		if (code_map[code].memory == 0)
		{
			code_map[code].memory = 1;
		} else
			pressed = 0;
	} else
		code_map[code].memory = 0;

	profiler_mark(PROFILER_END);

	return pressed;
}

int code_pressed_memory_repeat(InputCode code, int speed)
{
	static int counter;
	static int keydelay;
	int pressed;

	profiler_mark(PROFILER_INPUT);

	pressed = internal_code_pressed(code);

	if (pressed)
	{
		if (code_map[code].memory == 0)
		{
			code_map[code].memory = 1;
			keydelay = 3;
			counter = 0;
		}
		else if (++counter > keydelay * speed * Machine->drv->frames_per_second / 60)
		{
			keydelay = 1;
			counter = 0;
		} else
			pressed = 0;
	} else
		code_map[code].memory = 0;

	profiler_mark(PROFILER_END);

	return pressed;
}

InputCode code_read_async(void)
{
	unsigned i;

	profiler_mark(PROFILER_INPUT);

	/* Update the table */
	internal_code_update();

	for(i=0;i<code_mac;++i)
		if (code_pressed_memory(i))
			return i;

	profiler_mark(PROFILER_END);

	return CODE_NONE;
}

InputCode code_read_sync(void)
{
	InputCode code;
	unsigned oscode;

	/* now let the OS process it */
	oscode = osd_wait_keypress();

	/* convert the code */
	code = keyoscode_to_code(oscode);

	while (code == CODE_NONE)
		code = code_read_async();

	return code;
}

/***************************************************************************/
/* Sequences */

void seq_set_0(InputSeq* a)
{
	int j;
	for(j=0;j<SEQ_MAX;++j)
		(*a)[j] = CODE_NONE;
}

void seq_set_1(InputSeq* a, InputCode code)
{
	int j;
	(*a)[0] = code;
	for(j=1;j<SEQ_MAX;++j)
		(*a)[j] = CODE_NONE;
}

void seq_set_2(InputSeq* a, InputCode code1, InputCode code2)
{
	int j;
	(*a)[0] = code1;
	(*a)[1] = code2;
	for(j=2;j<SEQ_MAX;++j)
		(*a)[j] = CODE_NONE;
}

void seq_set_3(InputSeq* a, InputCode code1, InputCode code2, InputCode code3)
{
	int j;
	(*a)[0] = code1;
	(*a)[1] = code2;
	(*a)[2] = code3;
	for(j=3;j<SEQ_MAX;++j)
		(*a)[j] = CODE_NONE;
}

void seq_copy(InputSeq* a, InputSeq* b)
{
	int j;
	for(j=0;j<SEQ_MAX;++j)
		(*a)[j] = (*b)[j];
}

int seq_cmp(InputSeq* a, InputSeq* b)
{
	int j;
	for(j=0;j<SEQ_MAX;++j)
		if ((*a)[j] != (*b)[j])
			return -1;
	return 0;
}

void seq_name(InputSeq* code, char* buffer, unsigned max)
{
	int j;
	char* dest = buffer;
	for(j=0;j<SEQ_MAX;++j)
	{
		const char* name;

		if ((*code)[j]==CODE_NONE)
			break;

		if (j && 1 + 1 <= max)
		{
			*dest = ' ';
			dest += 1;
			max -= 1;
		}

		name = code_name((*code)[j]);
		if (!name)
			break;

		if (strlen(name) + 1 <= max)
		{
			strcpy(dest,name);
			dest += strlen(name);
			max -= strlen(name);
		}
	}

	if (dest == buffer && 4 + 1 <= max)
		strcpy(dest,"None");
	else
		*dest = 0;
}

int seq_pressed(InputSeq* code)
{
	int j;
	int res = 1;
	int invert = 0;
	int count = 0;

	for(j=0;j<SEQ_MAX;++j)
	{
		switch ((*code)[j])
		{
			case CODE_NONE :
				return res && count;
			case CODE_OR :
				if (res && count)
					return 1;
				res = 1;
				count = 0;
				break;
			case CODE_NOT :
				invert = !invert;
				break;
			default:
				if (res && (code_pressed((*code)[j]) != 0) == invert)
					res = 0;
				invert = 0;
				++count;
		}
	}
	return res && count;
}

/* Static informations used in key/joy recording */
static InputCode record_seq[SEQ_MAX]; /* buffer for key recording */
static int record_count; /* number of key/joy press recorded */
static clock_t record_last; /* time of last key/joy press */

#define RECORD_TIME (CLOCKS_PER_SEC*2/3) /* max time between key press */

/* Start a sequence recording */
void seq_read_async_start(void)
{
	unsigned i;

	record_count = 0;
	record_last = clock();

	/* reset code memory, otherwise this memory may interferes with the input memory */
	for(i=0;i<code_mac;++i)
		code_map[i].memory = 1;
}

/* Check that almost one key/joy must be pressed */
static int seq_valid(InputSeq* seq)
{
	int j;
	int positive = 0;
	int pred_not = 0;
	int operand = 0;
	for(j=0;j<SEQ_MAX;++j)
	{
		switch ((*seq)[j])
		{
			case CODE_NONE :
				break;
			case CODE_OR :
				if (!operand || !positive)
					return 0;
				pred_not = 0;
				positive = 0;
				operand = 0;
				break;
			case CODE_NOT :
				if (pred_not)
					return 0;
				pred_not = !pred_not;
				operand = 0;
				break;
			default:
				if (!pred_not)
					positive = 1;
				pred_not = 0;
				operand = 1;
				break;
		}
	}
	return positive && operand;
}

/* Record a key/joy sequence
	return <0 if more input is needed
	return ==0 if sequence succesfully recorded
	return >0 if aborted
*/
int seq_read_async(InputSeq* seq, int first)
{
	InputCode newkey;

	if (input_ui_pressed(IPT_UI_CANCEL))
		return 1;

	if (record_count == SEQ_MAX
		|| (record_count > 0 && clock() > record_last + RECORD_TIME))	{
		int k = 0;
		if (!first)
		{
			/* search the first space free */
			while (k < SEQ_MAX && (*seq)[k] != CODE_NONE)
				++k;
		}

		/* if no space restart */
		if (k + record_count + (k!=0) > SEQ_MAX)
			k = 0;

		/* insert */
		if (k + record_count + (k!=0) <= SEQ_MAX)
		{
			int j;
			if (k!=0)
				(*seq)[k++] = CODE_OR;
			for(j=0;j<record_count;++j,++k)
				(*seq)[k] = record_seq[j];
		}
		/* fill to end */
		while (k < SEQ_MAX)
		{
			(*seq)[k] = CODE_NONE;
			++k;
		}

		if (!seq_valid(seq))
			seq_set_1(seq,CODE_NONE);

		return 0;
	}

	newkey = code_read_async();

	if (newkey != CODE_NONE)
	{
		/* if code is duplicate negate the code */
		if (record_count && newkey == record_seq[record_count-1])
			record_seq[record_count-1] = CODE_NOT;

		record_seq[record_count++] = newkey;
		record_last = clock();
	}

	return -1;
}

/***************************************************************************/
/* input ui */

/* Static buffer for memory input */
struct ui_info {
	int memory;
};

static struct ui_info ui_map[__ipt_max];

int input_ui_pressed(int code)
{
	int pressed;

	profiler_mark(PROFILER_INPUT);

	pressed = seq_pressed(input_port_type_seq(code));

	if (pressed)
	{
		if (ui_map[code].memory == 0)
		{
                        ui_map[code].memory = 1;
		} else
			pressed = 0;
	} else
		ui_map[code].memory = 0;

	profiler_mark(PROFILER_END);

	return pressed;
}

int input_ui_pressed_repeat(int code,int speed)
{
	static int counter,inputdelay;
	int pressed;

	profiler_mark(PROFILER_INPUT);

	pressed = seq_pressed(input_port_type_seq(code));

	if (pressed)
	{
		if (ui_map[code].memory == 0)
		{
			ui_map[code].memory = 1;
			inputdelay = 3;
			counter = 0;
		}
		else if (++counter > inputdelay * speed * Machine->drv->frames_per_second / 60)
		{
			inputdelay = 1;
			counter = 0;
		} else
			pressed = 0;
	} else
		ui_map[code].memory = 0;

	profiler_mark(PROFILER_END);

	return pressed;
}

