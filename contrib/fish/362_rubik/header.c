/*	header.c  ...for rubik.c	*/

/*	started 23-Dec-89	*/

struct coord 
	{
	short x;
	short y;
	short z;
	};

struct points
	{
	struct coord xyz [6] [6] [6];
	};

struct one_square
	{
	struct coord *corner[4];
	short colour;
	};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  need undef to stop compiler giving snotty warning
	messages when NULL is redefined in exec/types.h
*/

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/io.h>
#include <exec/exec.h>
#include <exec/memory.h>
#include <exec/lists.h>
#include <exec/interrupts.h>
#include <exec/ports.h>
#include <exec/libraries.h>
#include <exec/tasks.h>
#include <exec/execbase.h>
#include <exec/devices.h>

#include <devices/input.h>
#include <devices/inputevent.h>

#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>

/*
#include <hardware/dmabits.h>
#include <hardware/custom.h>
#include <hardware/blit.h>
*/

#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/view.h>
#include <graphics/gels.h>
#include <graphics/regions.h>
#include <graphics/clip.h>
#include <graphics/text.h>
#include <graphics/gfxbase.h>

#include <devices/keymap.h>

#include <libraries/dos.h>
#include <libraries/diskfont.h>

#include <workbench/startup.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include <aros/oldprograms.h>