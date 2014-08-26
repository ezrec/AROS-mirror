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
#include "poppler.h"
#include "settings.h"
#include "locale.h"
#include "system/functions.h"

#include "mcc/documentview_class.h"
#include "mcc/documentlayout_class.h"
#include "mcc/toolbar_class.h"
#include "mcc/renderer_class.h"
#include "mcc/title_class.h"

#define ATTR(a) a, #a

struct Data
{
	ULONG dummy;
	int groupsmodified;
};


static const char *layouts[] = {"Single page", "Continuous", "Facing pages", "Continuous facing pages", NULL};
static const char *scalings[] = {"Fit to Window", "Fit to Window Width", "Original", NULL};
static char *ps_levels[] = {"Level 2", "Level 3", NULL};

struct attribute
{
	Object *object;
	unsigned int attribute;
	int group;
	unsigned int value;
	char svalue[512];        // TODO: group them in an union? fix length limit? -- kiero 
	int etype;
	char name[64];
};

struct attribute attributes[MUIA_VPDFSettings_Last - MUIA_VPDFSettings_First + 1] = {0};
struct attribute attributes_old[MUIA_VPDFSettings_Last - MUIA_VPDFSettings_First + 1] = {0};



Object *register_attribute(int id, char *name, Object *obj, unsigned int attribute, int group, int etype, unsigned int initial)
{
	id = id - MUIA_VPDFSettings_First;

	attributes[id].object = obj;
	attributes[id].attribute = attribute;
	attributes[id].group = group;
	attributes[id].value = 0;
	attributes[id].svalue[0] = 0;
	attributes[id].etype = etype;
	stccpy(attributes[id].name, name, sizeof(attributes[id].name));
	if (strlen(name) >= sizeof(attributes[id].name))
		printf("************SETTINGS ERROR: Attribute name too long:%s****************\n", name);

	switch(etype)
	{
		case ETYPE_NUMERIC:
			attributes[id].value = initial;
			break;
		case ETYPE_STRING:
			stccpy(attributes[id].svalue, (char*)initial, sizeof(attributes[id].svalue));
			break;
	}

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
				switch (attributes[id].etype)
				{
				
					case ETYPE_NUMERIC:
						fprintf(f, "%s 0x%lx\n", attributes[id].name, attributes[id].value);
						break;
					case ETYPE_STRING:
					{
                        char *enter;
					    if ((enter = strrchr(attributes[id].svalue, '\n')))
                            *enter = '\0';
						fprintf(f, "%s %s\n", attributes[id].name, attributes[id].svalue);
						break;
					}
				}
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
						switch (attributes[id].etype)
						{
							case ETYPE_NUMERIC:
							{
								unsigned int lvalue = strtol(value, NULL, 0); /* values are written as hex, but in case user modifies them... */
								attributes[id].value = lvalue;
								break;
							}
							case ETYPE_STRING:
							{
								stccpy(attributes[id].svalue, value, sizeof(attributes[id].svalue));
								break;
							}
						}
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

static Object *buildGroupSeparator(char *title)
{
		Object *p1;
		Object *grp = HGroup,
			MUIA_InnerBottom, 0,
			MUIA_InnerTop, 0,
			MUIA_Group_VertSpacing, 0,
			Child, p1 = MUI_MakeObject(MUIO_HBar,1),
			Child, LLabel1(title),
			Child, MUI_MakeObject(MUIO_HBar,1),
		End;

		set(p1, MUIA_Weight, 10);
		return grp;

}

DEFNEW
{
	obj = DoSuperNew(cl, obj,
                Child,  PageGroup,
               // MUIA_Group_ActivePage, 1,
					Child, VGroup,
							MUIA_Background, MUII_GroupBack,
                            Child, buildGroupSeparator(LOCSTR(MSG_SETTINGS_DOCUMENT)),
                            Child, ColGroup(3),
                                Child, LLabel1(LOCSTR(MSG_SETTINGS_LAYOUT)),
                                Child, register_attribute(ATTR(MUIA_VPDFSettings_Layout),
                                    CycleObject, MUIA_Cycle_Entries, layouts, End,
                                    MUIA_Cycle_Active, VPDF_SGROUP_LAYOUT, ETYPE_NUMERIC, MUIV_DocumentView_Layout_Single),
                                Child, HSpace(0),
                                Child, LLabel1(LOCSTR(MSG_SETTINGS_SCALING)),
                                Child, register_attribute(ATTR(MUIA_VPDFSettings_Scaling),
                                    CycleObject, MUIA_Cycle_Entries, scalings, End,
                                    MUIA_Cycle_Active, VPDF_SGROUP_SCALING, ETYPE_NUMERIC, MUIV_DocumentLayout_Scaling_FitPage),
                                Child, HSpace(0),
                                End,

                            Child, buildGroupSeparator(LOCSTR(MSG_WINDOWS)),

                            Child, ColGroup(3),

                                Child, HGroup,
                                    Child, register_attribute(ATTR(MUIA_VPDFSettings_Outline),
                                        CheckMark(TRUE),
                                        MUIA_Selected, VPDF_SGROUP_GUI, ETYPE_NUMERIC, TRUE),
                                    End,
                                    Child, LLabel1(LOCSTR(MSG_SETTINGS_OUTLINE)),

                                    Child, HSpace(0),

                                    Child, HGroup,
                                    Child, register_attribute(ATTR(MUIA_VPDFSettings_OpenLog),
                                        CheckMark(FALSE),
                                        MUIA_Selected, VPDF_SGROUP_GUI, ETYPE_NUMERIC, FALSE),
                                    End,
                                    Child, LLabel1(LOCSTR(MSG_SETTINGS_LOGWINDOW)),
                                    Child, HSpace(0),
                                 
                             
                                End,
                                 
                            Child, HVSpace,
                         End,    
                    Child, VGroup,
                       Child, register_attribute(ATTR(MUIA_VPDFSettings_PSName),
                                StringObject, 	
                                    MUIA_Frame, MUIV_Frame_String,
                                    MUIA_String_Contents, "PS:",
                                    MUIA_String_MaxLen, 512,       
                                    End,
								MUIA_String_Contents, VPDF_SGROUP_PRINTER, ETYPE_STRING, "PS:"),
						
						Child, 	register_attribute(ATTR(MUIA_VPDFSettings_PrintingMode),
                                CheckMark(TRUE),
                                MUIA_Selected, VPDF_SGROUP_PRINTER, ETYPE_NUMERIC, 1),
                                
                        Child, register_attribute(ATTR(MUIA_VPDFSettings_PSMode),  
                                CycleObject, MUIA_Cycle_Entries, ps_levels, End,  
                                MUIA_Cycle_Active,  VPDF_SGROUP_PRINTER, ETYPE_NUMERIC, 0), 
                    End,
                    
                 End,
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
	if (msg->opg_AttrID > MUIA_VPDFSettings_First && msg->opg_AttrID < MUIA_VPDFSettings_Last)
	{
		int id = msg->opg_AttrID - MUIA_VPDFSettings_First;
		switch (attributes[id].etype)
		{
			case ETYPE_NUMERIC:
				*(ULONG*)msg->opg_Storage = (ULONG)attributes[id].value;
				break;
			case ETYPE_STRING:
				*(ULONG*)msg->opg_Storage = (ULONG)attributes[id].svalue;
				break;
		}	
		return TRUE;
	}

	return DOSUPER;

}

DEFSET
{
	//GETDATA;

	struct TagItem *tag, *_tags = INITTAGS;
	while ((tag = NextTagItem(&_tags)))
	{
		if (tag->ti_Tag > MUIA_VPDFSettings_First && tag->ti_Tag < MUIA_VPDFSettings_Last)
		{
			int id = tag->ti_Tag - MUIA_VPDFSettings_First;
			
			switch (attributes[id].etype)
			{
				case ETYPE_NUMERIC:
					 attributes[id].value = tag->ti_Data;
					break;
				case ETYPE_STRING:
					stccpy(attributes[id].svalue, (char*)tag->ti_Data, sizeof(attributes[id].svalue));
					break;
			}	
			
			if (attributes[id].object != NULL)
				nnset(attributes[id].object, attributes[id].attribute, tag->ti_Data);
		}
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
			switch(attributes[id].etype)
			{
				case ETYPE_NUMERIC:
					attributes[id].value = xget(attributes[id].object, attributes[id].attribute);
					if (attributes_old[id].value != attributes[id].value)
						data->groupsmodified |= attributes[id].group;
					break;
				case ETYPE_STRING:
					stccpy(attributes[id].svalue, (char*)xget(attributes[id].object, attributes[id].attribute), sizeof(attributes[id].svalue));
					if (strcmp(attributes_old[id].svalue, attributes[id].svalue))
						data->groupsmodified |= attributes[id].group;
					break;
			}
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
		{
			switch(attributes[id].etype)
			{
				case ETYPE_NUMERIC:
					nnset(attributes[id].object, attributes[id].attribute, attributes[id].value);
					break;
				case ETYPE_STRING:
					nnset(attributes[id].object, attributes[id].attribute, attributes[id].svalue);
					break;
			}
		}
		
		switch(attributes[id].etype)
		{
			case ETYPE_NUMERIC:
				if (attributes[id].value != attributes_old[id].value)
					modified |= attributes[id].group;
				break;
			case ETYPE_STRING:
				if (strcmp(attributes[id].svalue, attributes_old[id].svalue))
					modified |= attributes[id].group;
				break;
		}
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
	DECSET
	DECMMETHOD(VPDFSettings_FromObjects)
	DECMMETHOD(VPDFSettings_ToObjects)
	DECMMETHOD(VPDFSettings_Use)
	DECMMETHOD(VPDFSettings_Save)
	DECMMETHOD(VPDFSettings_Cancel)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, VPDFSettingsClass)




