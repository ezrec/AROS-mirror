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
#include <clib/dtclass_protos.h>
#include <datatypes/pictureclass.h>
#include <devices/rawkeycodes.h>

#include <libraries/gadtools.h>

#include <emul/emulregs.h>
#include <emul/emulinterface.h>
////

#include <private/vapor/vapor.h>
#include "util.h"
#include "poppler.h"
#include "settings.h"
#include "system/functions.h"

#include "mcc/documentview_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"

struct Data
{
	ULONG dummy;
	int groupsmodified;
};

static const char *layouts[] = {"Single page", "Continuous", "Facing pages", "Continuous facing pages", NULL};
static const char *scalings[] = {"Fit to Window", "Fit to Window Width", "Original", NULL};

struct attribute
{
	Object *object;
	unsigned int attribute;
	int group;
	unsigned int value;
	char name[64];
};

struct attribute attributes[MUIA_VPDFSettings_Last - MUIA_VPDFSettings_First + 1] = {0};
struct attribute attributes_old[MUIA_VPDFSettings_Last - MUIA_VPDFSettings_First + 1] = {0};

#define ATTR(a) a, #a

static Object *register_attribute(int id, char *name, Object *obj, unsigned int attribute, int group)
{
	id = id - MUIA_VPDFSettings_First;

	attributes[id].object = obj;
	attributes[id].attribute = attribute;
	attributes[id].group = group;
	attributes[id].value = 0;
	stccpy(attributes[id].name, name, sizeof(attributes[id].name));
	if (strlen(name) >= sizeof(attributes[id].name))
		printf("************SETTINGS ERROR: Attribute name too long:%s****************\n", name);

	return obj;
}

/* save settings to disk */

static int setWrite(char *fname)
{
	FILE *f = fopen(fname, "w");

	if (f != NULL)
	{
    	int i;

		for(i=MUIA_VPDFSettings_First+1; i<MUIA_VPDFSettings_Last; i++)
		{
			int id = i - MUIA_VPDFSettings_First;
			if (attributes[id].object != NULL)
			{
				fprintf(f, "%s 0x%lx\n", attributes[id].name, attributes[id].value);
			}
		}
		fclose(f);
		return TRUE;
	}
	return FALSE;
}

static int setRead(char *fname)
{
	FILE *f = fopen(fname, "r");

	if (f != NULL)
	{
    	int i;
		char buffer[2048];

		while(fgets(buffer, sizeof(buffer), f))
		{
			char *delim = strchr(buffer, ' ');
			if (delim != NULL)
			{
				char *name = buffer;
				char *value = delim + 1;

				*delim = '\0';

				/* lookup attribute and set value */

				for(i=MUIA_VPDFSettings_First+1; i<MUIA_VPDFSettings_Last; i++)
				{
					int id = i - MUIA_VPDFSettings_First;

					if (attributes[id].object != NULL && 0 == strcmp(name, attributes[id].name))
					{
						unsigned int lvalue = strtol(value, NULL, 0); /* values are written as hex, but in case user modifies them... */
						attributes[id].value = lvalue;
					}
				}

			}
		}
		fclose(f);
		return TRUE;
	}
	return FALSE;
}


static int setSave(void)
{
	return setWrite("ENVARC:VPDF.config");
}

static int setUse(void)
{
	return setWrite("ENV:VPDF.config");
}


DEFNEW
{
	obj = DoSuperNew(cl, obj,
					MUIA_Group_Horiz, FALSE,
					Child, ColGroup(3),
						Child, LLabel1("Layout:"),
						Child, register_attribute(ATTR(MUIA_VPDFSettings_Layout),
							CycleObject, MUIA_Cycle_Entries, layouts, End,
							MUIA_Cycle_Active, VPDF_SGROUP_LAYOUT),
						Child, HSpace(0),
						Child, LLabel1("Scaling:"),
						Child, register_attribute(ATTR(MUIA_VPDFSettings_Scaling),
							CycleObject, MUIA_Cycle_Entries, scalings, End,
							MUIA_Cycle_Active, VPDF_SGROUP_SCALING),
						Child, HSpace(0),
						End,
					Child, HVSpace,
					TAG_MORE, INITTAGS);

	if (obj != NULL)
	{

		/* load from file */

		setRead("ENV:VPDF.config");

		/* move to objects and make a snapshot */

		DoMethod(obj, MUIM_VPDFSettings_ToObjects);
		memcpy(attributes_old, attributes, sizeof(attributes));

		setWrite("ENVARC:VPDF.config");
	}

	return (ULONG)obj;
}

DEFGET
{
	GETDATA;

	if (msg->opg_AttrID > MUIA_VPDFSettings_First && msg->opg_AttrID < MUIA_VPDFSettings_Last)
	{
		int id = msg->opg_AttrID - MUIA_VPDFSettings_First;
		*(ULONG*)msg->opg_Storage = (ULONG)attributes[id].value;
		return TRUE;
	}

	return DOSUPER;

}

DEFMMETHOD(VPDFSettings_FromObjects)
{
	GETDATA;
	int i;

	data->groupsmodified = 0;

	for(i=MUIA_VPDFSettings_First+1; i<MUIA_VPDFSettings_Last; i++)
	{
		int id = i - MUIA_VPDFSettings_First;
		if (attributes[id].object != NULL)
		{
			attributes[id].value = xget(attributes[id].object, attributes[id].attribute);
			if (attributes_old[id].value != attributes[id].value)
				data->groupsmodified |= attributes[id].group;
		}
	}

	return data->groupsmodified;
}

DEFMMETHOD(VPDFSettings_ToObjects)
{
	GETDATA;
	int i;
	int modified = 0;

	for(i=MUIA_VPDFSettings_First+1; i<MUIA_VPDFSettings_Last; i++)
	{
		int id = i - MUIA_VPDFSettings_First;
		if (attributes[id].object != NULL) /* always set. no matter what old state looked like. snapshot might be invalid */
			nnset(attributes[id].object, attributes[id].attribute, attributes[id].value);
		if (attributes[id].value != attributes_old[id].value)
			modified |= attributes[id].group;
	}

	data->groupsmodified = 0;

	return modified;
}

DEFMMETHOD(VPDFSettings_Use)
{
	unsigned int modified = DoMethod(obj, MUIM_VPDFSettings_FromObjects);
	setUse();
	memcpy(attributes_old, attributes, sizeof(attributes));
	return modified;
}

DEFMMETHOD(VPDFSettings_Save)
{
	unsigned int modified = DoMethod(obj, MUIM_VPDFSettings_FromObjects);
	setUse();
	setSave();
	memcpy(attributes_old, attributes, sizeof(attributes));
	return modified;
}

DEFMMETHOD(VPDFSettings_Cancel)
{
	unsigned int modified;
	memcpy(attributes, attributes_old, sizeof(attributes));
	return DoMethod(obj, MUIM_VPDFSettings_ToObjects);
}

BEGINMTABLE
	DECNEW
	DECGET
	DECMMETHOD(VPDFSettings_FromObjects)
	DECMMETHOD(VPDFSettings_ToObjects)
	DECMMETHOD(VPDFSettings_Use)
	DECMMETHOD(VPDFSettings_Save)
	DECMMETHOD(VPDFSettings_Cancel)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, VPDFSettingsClass)




