
#include <devices/rawkeycodes.h>
#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/muimaster.h>
#include <private/vapor/vapor.h>
#include <proto/utility.h>

#if defined(__AROS__)
#include <clib/arossupport_protos.h>
#endif

#include "annotation_class.h"
#include "pageview_class.h"
#include "util.h"
#include "window.h"

struct Data
{
	struct MUI_EventHandlerNode ehnode;
	int x, y;
	char *contents;
	Object *refobj;
	int visible;
};

DEFNEW
{
	char *contents = GetTagData(MUIA_Annotation_Contents, "", INITTAGS);
	
	obj = DoSuperNew(cl, obj,
				MUIA_Group_Horiz, FALSE,
				MUIA_Alpha, 50,
#if defined(MUIA_Floating)
// FIXME: AROS
				MUIA_Floating, TRUE,
#endif
				MUIA_ShowMe, FALSE,
				Child, StringObject,
					MUIA_Frame, MUIV_Frame_String,
#if defined(MUIA_Textinput_Multiline)
// FIXME: AROS
					MUIA_Textinput_Multiline, 5,
#endif
					MUIA_String_Contents, contents,
					End,
				TAG_MORE, INITTAGS);
				
	if (obj != NULL)
	{
		GETDATA;
		
		data->x = GetTagData(MUIA_Annotation_PosX, 0, INITTAGS);
		data->y = GetTagData(MUIA_Annotation_PosY, 0, INITTAGS);
		data->refobj = GetTagData(MUIA_Annotation_RefObject, NULL, INITTAGS);
		data->visible = 0;
		data->contents = contents;
	}
				
	return obj;
}

DEFDISP
{
	return DOSUPER;
}


DEFMMETHOD(Layout)
{
	GETDATA;
	int width = xget(_parent(obj), MUIA_Width);
	int height = xget(_parent(obj), MUIA_Height);
	
	int	x0, y0;
	int	prvwidth, prvheight;
	int imgwidth, imgheight;

	x0 = _mleft(data->refobj);
	y0 = _mtop(data->refobj);
	prvwidth = xget(data->refobj, MUIA_PageView_RenderWidth);
	prvheight = xget(data->refobj, MUIA_PageView_RenderHeight);

	imgwidth = xget(data->refobj, MUIA_PageView_Width);
	imgheight = xget(data->refobj, MUIA_PageView_Height);

	/* calculate placement */

	if (imgheight < prvheight)
		y0 += (prvheight - imgheight) / 2;
	
	if (imgwidth < prvwidth)
		x0 += (prvwidth - imgwidth) / 2;
		
#if defined(__AROS__)
    kprintf("[Annotation::Layout] not implemented\n");
    // FIXME: AROS
#else
	msg->left = data->x + x0;
	msg->top = data->y + y0;
	
	if (msg->height > 50)
		msg->height = 50;

	if (msg->width > 200)
		msg->width = 200;
#endif
	
	return DOSUPER;
}

DEFSET
{
	GETDATA;
	
	FORTAG(INITTAGS)
	{
		case MUIA_Annotation_PosX:
			data->x = tag->ti_Data;
			break;
			
		case MUIA_Annotation_PosY:
			data->y = tag->ti_Data;
			break;
	}
	NEXTTAG

	return DOSUPER;
}

DEFMMETHOD(Annotation_Toggle)
{
	GETDATA;
	
	data->visible = 1 - data->visible;
	set(obj, MUIA_ShowMe, data->visible);
	
	return TRUE;
}

BEGINMTABLE
	DECNEW
	DECDISP
	DECSET
	DECMMETHOD(Layout)
	DECMMETHOD(Annotation_Toggle)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, AnnotationClass)
