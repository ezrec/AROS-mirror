/* intui.c:
 *
 * System requester stuff.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 18-Jul-94   fmu   - New CDDA icon (by Olivier Tonino)
 *                   - User-definable icon
 * 05-Jan-94   fmu   Retry displaying CD-DA icon if WB is not open.
 * 02-Jan-94   fmu   Location for CD-DA icon may be defined by user.
 * 11-Dec-93   fmu   Fixed AddAppIconA() call for DICE.
 * 28-Nov-93   fmu   Added custom CD-DA icon; removed GetDefDiskObject() call.
 * 09-Oct-93   fmu   SAS/C support added
 */

#include <stdarg.h>
#include <stdio.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/iobsolete.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/wb.h>

#include <aros/debug.h>

#include "intui.h"
#include "baseredef.h"

#define AROS_KERNEL 1

struct MsgPort *g_app_port;
struct AppIcon *g_app_icon;
ULONG g_app_sigbit;
int g_retry_show_cdda_icon = FALSE;

UWORD g_image_data[] = {
	/* Plane 0 */
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0001,0x1D5F,0x5000,
		0x0002,0x8000,0x0000,0x0002,0x8000,0x0000,0x001E,0xB9F1,0xF000,
		0x0030,0xEF0F,0x1800,0x002E,0xAAF2,0xE800,0x002E,0xEEF6,0xB800,
		0x002E,0xAB1A,0x8000,0x002E,0xA9EA,0xB800,0x002E,0xA9EA,0xE800,
		0x0030,0xAA1B,0x1800,0x001F,0x39F1,0xF000,0x0000,0x0000,0x0000,
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0003,0xFFAB,0x9110,
		0x000F,0x04D5,0x6930,0x4430,0xAAAA,0x8BA4,0x0040,0x54D4,0x5440,
		0x54A8,0x0AA8,0xA87C,0x0955,0x44D5,0x5448,0x52AA,0xA02A,0x8014,
		0x22AA,0xA7D4,0x0062,0x5001,0x4D6A,0xAAD4,0xAAAA,0x07DF,0x554A,
		0x5250,0x882F,0xFED4,0xB805,0x5717,0xFDFA,0x552A,0xAE0B,0xFBD4,
		0xEF95,0x5C45,0xE3EE,0x5D62,0xB80A,0x1F5C,0xFFDD,0x5555,0xFBFE,
		0x7777,0xF777,0xD776,0xFFFE,0xFFFE,0x7FFE,0xFFFF,0xFFFF,0xFFFE,
	/* Plane 1 */
		0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x140D,0x1000,
		0x5455,0x4000,0x0454,0x0001,0x0000,0x0000,0x5541,0x6C04,0x0554,
		0x888F,0x10F0,0xE088,0x555B,0x5505,0x1554,0xAA91,0x1171,0x02AA,
		0x5551,0x54E5,0x5554,0xEED1,0x5595,0x46EE,0x555B,0x5415,0x1554,
		0xFFCF,0x55E4,0xE7FE,0x5540,0x6C04,0x0554,0xFFFF,0xFFFF,0xFFFE,
		0xDDDD,0xDDDD,0xDDDC,0xFFFF,0xFFFF,0xFFFE,0xFFFC,0x55FE,0x6EEE,
		0xFFF6,0xAEFB,0xDEEE,0xBB9F,0xFFF7,0x755A,0xFFFF,0xBEEE,0xAFBE,
		0xAAFF,0xF7DD,0x5FC2,0xF7FF,0xFABA,0xFFB6,0xA9FF,0xFFF7,0xFFEA,
		0xDF55,0x583F,0xFFBC,0xAAAA,0xB6DF,0xFFAA,0x53F5,0xFBAA,0xAAD4,
		0xABEF,0x7FF4,0x054A,0x45FB,0xFDFA,0x02C4,0xABDF,0xF9FD,0x0D2A,
		0x10EF,0xF2BE,0xBA10,0xA2DD,0xE5F7,0xEAA2,0x0013,0xFAAB,0x4800,
		0x8887,0x088A,0x4888,0x0000,0xD00A,0x8000,0x0000,0x0000,0x0000,
	/* Plane 2 */
		0xFFFF,0xFFFF,0xFFFE,0xFFFF,0xFFFF,0xFFFE,0xFFFE,0xE2A0,0xAFFE,
		0xFFFC,0x7FFF,0xFFFE,0xFFFC,0x7FFF,0xFFFE,0xFFE0,0x460E,0x0FFE,
		0xFFD0,0x4505,0x17FE,0xFFC0,0x440C,0x07FE,0xFFC4,0x440C,0x47FE,
		0xFFC4,0x4514,0x7FFE,0xFFC4,0x4604,0x47FE,0xFFC0,0x4604,0x07FE,
		0xFFD0,0x4415,0x17FE,0xFFE0,0xC60E,0x0FFE,0xFFFF,0xFFFF,0xFFFE,
		0xFFFF,0xFFFF,0xFFFE,0xFFFF,0xFFFF,0xFFFE,0xFFFF,0xFFAB,0xFFFE,
		0xFFFF,0x51D5,0x7FFE,0xFFF0,0x00A8,0x8BBE,0xFFC0,0x41D1,0x547E,
		0xFFA8,0x08A2,0xA87E,0xFF55,0x45C5,0x547E,0xFEAA,0xA02A,0x803E,
		0xFEAA,0xA454,0x007E,0xFD55,0x4BAA,0xAAFE,0xFE0A,0x045F,0x553E,
		0xFE10,0xA82F,0xFEBE,0xFE04,0x0357,0xFD3E,0xFE20,0x06AB,0xFA7E,
		0xFF10,0x0D55,0xE4FE,0xFFA2,0x1AAA,0x11FE,0xFFED,0x5555,0x87FE,
		0xFFF8,0xFFFC,0x3FFE,0xFFFF,0x0001,0xFFFE,0xFFFF,0xFFFF,0xFFFE
};

struct Image g_disk_object_image = {
  0, 0,			/* top corner */
  47, 36, 3,		/* width, height, depth */
  g_image_data,
  0xff, 0x00,		/* planepick, planeonoff */
  NULL,			/* next image */
};

UBYTE *g_tool_types[] = {
  NULL
};

struct DiskObject *g_user_disk_object = NULL;

struct DiskObject g_disk_object = {
  WB_DISKMAGIC,
  WB_DISKVERSION,
  {
    NULL,
    0, 0,
    47, 37,
    GADGIMAGE | GADGHCOMP,
    RELVERIFY | GADGIMMEDIATE,
    BOOLGADGET,
    (APTR) &g_disk_object_image,
    NULL,
  },
  0,
  "",
  (char **) g_tool_types,
  NO_ICON_POSITION,
  NO_ICON_POSITION,
  NULL,
  NULL,
  0
};

long g_xpos = NO_ICON_POSITION;
long g_ypos = NO_ICON_POSITION;

char *g_iconname = "CD-DA";

void Init_Intui(struct ACDRBase *acdrbase) {

	IntuitionBase = 
		OpenLibrary ((UBYTE *) "intuition.library", 37);
	IconBase = OpenLibrary ((UBYTE *) "icon.library", 37);
	if (!IconBase)
		Display_Error (acdrbase, "cannot open icon.library");
	WorkbenchBase = OpenLibrary ((UBYTE *) "workbench.library", 37);
	if (!WorkbenchBase)
		Display_Error(acdrbase, "cannot open workbench.library");

#ifndef AROS_KERNEL
	g_user_disk_object = GetDiskObject ("env:cdda");
	if (!g_user_disk_object)
	{
#endif
		g_user_disk_object = &g_disk_object;
#ifndef AROS_KERNEL
	}
	else
	{
	char *name;
		name = FindToolType (g_user_disk_object->do_ToolTypes, "ICONNAME");
		if (name)
			g_iconname = name;
	}
#endif

	g_app_port = NULL;
	g_app_sigbit = 0;
	g_app_icon = NULL;
}

void Close_Intui(struct ACDRBase *acdrbase) {
	if (WorkbenchBase)
		CloseLibrary (WorkbenchBase);
	if (IconBase)
		CloseLibrary (IconBase);
	if (IntuitionBase)
		CloseLibrary ((struct Library *) IntuitionBase);
}

void Display_Error (struct ACDRBase *acdrbase, char *p_message, ...) {
va_list arg;

#ifndef AROS_KERNEL
        static struct EasyStruct req =
	{
		sizeof (struct EasyStruct),
		0,
		(UBYTE *) "CDROM Handler Error",
		NULL,
		(UBYTE *) "Abort"
	};
          
	va_start (arg, p_message);
	if (IntuitionBase)
	{
		req.es_TextFormat = (UBYTE *) p_message;
		EasyRequestArgs (NULL, &req, NULL, arg);
	}
	va_end (p_message);
#else        
        kprintf( "cdrom.handler error: %s\n", p_message );
#endif
}

void Show_CDDA_Icon (struct ACDRBase *acdrbase) {

	if (!IconBase || !WorkbenchBase)
		return;

	g_user_disk_object->do_CurrentX = g_xpos;
	g_user_disk_object->do_CurrentY = g_ypos;

	if (g_app_icon)
		Display_Error(acdrbase, "Show_CDDA_Icon called twice!");

	g_app_port = CreateMsgPort ();
	if (!g_app_port)
		return;

	g_app_sigbit = 1<<g_app_port->mp_SigBit;

	g_app_icon = AddAppIconA
		(
			0,
			0,
			(UBYTE *) g_iconname,
			g_app_port,
			NULL,
			g_user_disk_object,
			NULL
		);

	/*
		AddAppIconA may fail if the Workbench has not yet been loaded.
   */
	if (!g_app_icon)
	{
		DeleteMsgPort (g_app_port);
		g_app_port = NULL;
		g_retry_show_cdda_icon = TRUE;
		return;
	}
	g_retry_show_cdda_icon = FALSE;
}

void Hide_CDDA_Icon(struct ACDRBase *acdrbase) {
struct Message *msg;

	g_retry_show_cdda_icon = FALSE;

	if (!IconBase || !WorkbenchBase)
		return;

	if (g_app_icon)
		RemoveAppIcon (g_app_icon);
	if (g_app_port)
	{
		while (msg = GetMsg (g_app_port))
			ReplyMsg (msg);
		DeleteMsgPort (g_app_port);
	}

	g_app_port = NULL;
	g_app_sigbit = 0;
	g_app_icon = NULL;
}
