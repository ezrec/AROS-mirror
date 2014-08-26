/// System includes
#define AROS_ALMOST_COMPATIBLE
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <libraries/asl.h>
#include <workbench/workbench.h>

#include <proto/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <exec/libraries.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <dos/dos.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>

#include <proto/alib.h>
#include <proto/utility.h>

#include <proto/datatypes.h>
#include <proto/dtclass.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#if defined(__MORPHOS__)
#include <emul/emulregs.h>
#include <emul/emulinterface.h>
#endif
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "system/functions.h"
#include "logger.h"

struct Data
{
	Object *list;
};

DEFNEW
{
	Object *list;

	obj = DoSuperNew(cl, obj,
						Child, list = ListObject,
							MUIA_Frame, MUIV_Frame_ReadList,
							MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
							MUIA_List_DestructHook, MUIV_List_DestructHook_String,
							End,
						TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		memset(data, 0, sizeof(struct Data));
		data->list = list;
	}

	return (ULONG)obj;
}

DEFMMETHOD(Logger_LogMessage)
{
	GETDATA;
	DoMethod(data->list, MUIM_List_InsertSingle, msg->message, MUIV_List_Insert_Bottom);
	return 0;
}

BEGINMTABLE
	DECNEW
	DECMMETHOD(Logger_LogMessage)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, LoggerClass)
