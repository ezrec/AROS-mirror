/*
*/

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <exec/libraries.h>
#include <graphics/gfx.h>
#include <proto/alib.h>
#include <proto/graphics.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <constructor.h>
#include <intuition/pointerclass.h>

#define	 SYSTEM_PRIVATE
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#undef	 SYSTEM_PRIVATE

#include <private/vapor/vapor.h>
#include <mui/Rawimage_mcc.h>

#include "pageview_class.h"
#include "toolbar_class.h"
#include "documentlayout_class.h"
#include "documentview_class.h"
#include "toolbar_images.h"

#include "util.h"
#include "poppler.h"
#include "../locale.h"


struct Data
{
	Object *btnZoomIn;
	Object *btnZoomOut;
	Object *strZoom;
	Object *btnZoomPopup;
	Object *btnRotateRight;
	Object *btnRotateLeft;
	Object *btnLayouts[4];
	Object *btnOutline;
	Object *btnDragScroll;
	Object *btnDragMark;

	Object *document;
	int zoomid;
};

static float presets[] = {
	8.33f, 12.5f, 25.0f, 33.33f, 50.0f, 66.67f, 75.0f, 100.0f, 125.0f, 150.0f, 200.0f, 300.0f, 400.0f, 600.0f, 800.0f, 1000.0f};

enum
{
	MEN_ZOOM = 1,
	MEN_ZOOM_FITWIDTH, MEN_ZOOM_FITPAGE,
	MEN_ZOOM_PRESET0,
};

struct layoutmode
{
	char name[128];
	int mode;
};

static struct layoutmode layoutmodes[4];


/*
 * MUI Standard methods
 */

///	dispose

DEFDISP
{
	GETDATA;

	/* dispose parent class */

	return(DOSUPER);
}

////

///	new

static Object *layoutbutton(char *label, UBYTE *image)
{
	return RawimageObject,
		MUIA_Rawimage_Data, image,
		MUIA_ShortHelp, label,
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_Background, MUII_ButtonBack,
		MUIA_InputMode, MUIV_InputMode_Immediate,
		End;
}

DEFNEW
{
	Object *btnZoomIn, *btnZoomOut, *btnLayouts[4], *strZoom, *btnRotateLeft, *btnRotateRight, *btnOutline;
	Object *btnZoomPopup, *btnDragMark, *btnDragScroll;

	layoutmodes[0].mode = MUIV_DocumentView_Layout_Single;
	layoutmodes[1].mode = MUIV_DocumentView_Layout_ContinuousSingle;
	layoutmodes[2].mode = MUIV_DocumentView_Layout_Facing;
	layoutmodes[3].mode = MUIV_DocumentView_Layout_ContinuousFacing;
	strncpy(layoutmodes[0].name,  LOCSTR( MSG_TOOLBAR_SINGLE ),128);
	strncpy(layoutmodes[1].name,  LOCSTR( MSG_TOOLBAR_CONTINUOUS  ),128);
	strncpy(layoutmodes[2].name,  LOCSTR( MSG_TOOLBAR_FACING  ),128);
	strncpy(layoutmodes[3].name,  LOCSTR( MSG_TOOLBAR_CONT_FACING  ),128);
	
	obj = DoSuperNew(cl, obj,
			MUIA_Group_Horiz, TRUE,
			Child, strZoom = TextObject,
				MUIA_Text_PreParse, "\033c",
				MUIA_Weight, 50,
				//MUIA_Frame, MUIV_Frame_Text,
				End,
			Child, btnZoomPopup = ImageObject,
									ButtonFrame,
									MUIA_Image_Spec           , MUII_PopUp,
									MUIA_Image_FontMatchString, "M",
									MUIA_Image_FontMatchWidth , TRUE,
									MUIA_Image_FreeVert       , TRUE,
									MUIA_InputMode            , MUIV_InputMode_RelVerify,
									MUIA_Background           , MUII_ButtonBack,
									End,

			Child, HGroup,
				MUIA_Weight, 1,
				Child, btnZoomIn = RawimageObject, 
										MUIA_Rawimage_Data, button_zoomin,
										MUIA_ShortHelp, LOCSTR( MSG_TOOLBAR_ZOOMIN  ),
										MUIA_Frame, MUIV_Frame_Button,
										MUIA_InputMode, MUIV_InputMode_RelVerify,
										MUIA_Background, MUII_ButtonBack,
										End,
				Child, btnZoomOut = RawimageObject, 
										MUIA_Rawimage_Data, button_zoomout,
										MUIA_ShortHelp, LOCSTR( MSG_TOOLBAR_ZOOMOUT  ),
										MUIA_Frame, MUIV_Frame_Button,
										MUIA_InputMode, MUIV_InputMode_RelVerify,
										MUIA_Background, MUII_ButtonBack,
										End,
				End,
				
			Child, HSpace(0),
						
			Child, btnRotateLeft = RawimageObject, 
										MUIA_Rawimage_Data, button_rotatecounterclockwise,
										MUIA_ShortHelp, LOCSTR( MSG_TOOLBAR_CLOCKWISE ), 
										MUIA_Frame, MUIV_Frame_Button,
										MUIA_InputMode, MUIV_InputMode_RelVerify,
										MUIA_Background, MUII_ButtonBack,
										End,
			Child, btnRotateRight = RawimageObject,
										MUIA_Rawimage_Data, button_rotateclockwise,
										MUIA_ShortHelp, LOCSTR( MSG_TOOLBAR_COUNTERCLOCKWISE), 
										MUIA_Frame, MUIV_Frame_Button,
										MUIA_InputMode, MUIV_InputMode_RelVerify,
										MUIA_Background, MUII_ButtonBack,
										End,
			Child, HSpace(0),

			Child, btnLayouts[0] = layoutbutton(layoutmodes[0].name, button_singlepage),
			Child, btnLayouts[1] = layoutbutton(layoutmodes[1].name, button_singlepagecont),
			Child, btnLayouts[2] = layoutbutton(layoutmodes[2].name, button_doublepage),
			Child, btnLayouts[3] = layoutbutton(layoutmodes[3].name, button_doublepagecont),
			
			Child, HSpace(0),
			
			Child, HGroup,
				MUIA_Weight, 1,
				Child, btnDragMark = TextObject,
										MUIA_Frame                , MUIV_Frame_Button,      
										MUIA_Text_Contents        , LOCSTR( MSG_TOOLBAR_MARK), 
										MUIA_InputMode            , MUIV_InputMode_Toggle,
										MUIA_Background           , MUII_ButtonBack,
										End,
				Child, btnDragScroll = TextObject,
										MUIA_Frame                , MUIV_Frame_Button,      
										MUIA_Text_Contents        , LOCSTR( MSG_TOOLBAR_SCROLL), 
										MUIA_InputMode            , MUIV_InputMode_Toggle,
										MUIA_Background           , MUII_ButtonBack,
										MUIA_Selected             , TRUE,
										End,
				End,
				
			Child, HSpace(0),
			
			Child, btnOutline = TextObject,
									MUIA_Frame                , MUIV_Frame_Button,      
									MUIA_Text_Contents        , LOCSTR( MSG_TOOLBAR_OUTLINE), 
									MUIA_InputMode            , MUIV_InputMode_Toggle,
									MUIA_Background           , MUII_ButtonBack,
									End,
		TAG_MORE, INITTAGS);

	if (obj != NULL)
	{
		GETDATA;
		int i;

		memset(data, 0, sizeof(struct Data));
		data->strZoom = strZoom;
		data->btnZoomPopup = btnZoomPopup;
		data->btnDragMark = btnDragMark;
		data->btnDragScroll = btnDragScroll;
		memcpy(data->btnLayouts, btnLayouts, sizeof(btnLayouts));

		FORTAG(INITTAGS)
		{
			case MUIA_Toolbar_DocumentView:
				data->document = (Object*)tag->ti_Data;
				break;
				
			case MUIA_DocumentView_Outline:
				nnset(btnOutline, MUIA_Selected, tag->ti_Data);
				break;
		}
		NEXTTAG

		DoMethod(btnZoomIn, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Toolbar_Zoom, 10);
		DoMethod(btnZoomOut, MUIM_Notify, MUIA_Pressed, FALSE, obj, 2, MUIM_Toolbar_Zoom, -10);
		DoMethod(btnZoomPopup, MUIM_Notify, MUIA_Selected, FALSE, obj, 1, MUIM_Toolbar_ZoomPopup);
		
		DoMethod(btnRotateLeft, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_DocumentView_RotateLeft);
		DoMethod(btnRotateRight, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_DocumentView_RotateRight);

		DoMethod(btnOutline, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, obj, 2, MUIM_Toolbar_ToggleOutline, MUIV_TriggerValue);

		DoMethod(btnDragMark, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, obj, 2, MUIM_Toolbar_DragAction, MUIV_DocumentView_DragAction_Mark);
		DoMethod(btnDragScroll, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, obj, 2, MUIM_Toolbar_DragAction, MUIV_DocumentView_DragAction_Scroll);

		for(i=0; i<4; i++)
			DoMethod(btnLayouts[i], MUIM_Notify, MUIA_Selected, TRUE, obj, 2, MUIM_Toolbar_Layout, i);

	}

	return (ULONG)obj;

}

////

///	setup

DEFMMETHOD(Setup)
{
	return DOSUPER;
}

////

///	cleanup

DEFMMETHOD(Cleanup)
{
	GETDATA;
	return(DOSUPER);
}

////

///	zoom - format string

static char *zoomFormat(int zoomid, char *buffer, int buffersize)
{
	if (zoomid == MEN_ZOOM_FITPAGE)
		snprintf(buffer, buffersize,  LOCSTR( MSG_TOOLBAR_FITPAGE));
	else if (zoomid == MEN_ZOOM_FITWIDTH)
		snprintf(buffer, buffersize,   LOCSTR( MSG_TOOLBAR_FITWIDTH));
	else
	{
		float zoom = presets[zoomid - MEN_ZOOM_PRESET0] / 100.0f;
		snprintf(buffer, buffersize, "%.2f%%", zoom * 100.0f);
	}

	return buffer;
}

////

///	set

DEFSET
{
	GETDATA;
	int i;

	FORTAG(INITTAGS)
	{
		case MUIA_Toolbar_DocumentView:
			data->document = (Object*)tag->ti_Data;

			/* update state depending on assigned document view. for now only layout mode + outlines */

			for(i=0; i<4; i++)
				nnset(data->btnLayouts[i], MUIA_Selected, layoutmodes[i].mode == xget(data->document, MUIA_DocumentView_Layout));

			/* setup zoom mode and factor (externalize if needed in other situations) */
			
			{
				char buff[32];
				int scaling = xget(data->document, MUIA_DocumentLayout_Scaling);
				float zoom = xget(data->document, MUIA_DocumentLayout_Zoom) / 65536.0f;
				int presnum;

				for(presnum = 0; presets[presnum] < zoom * 100.0f && presnum < (sizeof(presets) / sizeof(presets[0])); presnum++) {};

				if (presnum >= (sizeof(presets) / sizeof(presets[0])))
					break;

				if (scaling == MUIV_DocumentLayout_Scaling_FitPage)
					data->zoomid = MEN_ZOOM_FITPAGE;
				else if (scaling == MUIV_DocumentLayout_Scaling_FitWidth)
					data->zoomid = MEN_ZOOM_FITWIDTH;
				else		
					data->zoomid = MEN_ZOOM_PRESET0 + presnum;
					
				zoom = presets[presnum] / 100.0f;

				zoomFormat(data->zoomid, buff, sizeof(buff));
				set(data->strZoom, MUIA_Text_Contents, buff);
			}
			

			break;
	}
	NEXTTAG;

	return(DOSUPER);
}

////

///	get

DEFGET
{
	GETDATA;

	switch (msg->opg_AttrID)
	{
		case MUIA_PageView_NeedRefresh:
			return TRUE;
	}

	return(DOSUPER);
}

////

///	layout


DEFMMETHOD(Toolbar_Layout)
{
	GETDATA;
	int i;

	for(i=0; i<4; i++)
	{
		if (i != msg->layout)
			nnset(data->btnLayouts[i], MUIA_Selected, FALSE);
	}

	return DoMethod(data->document, MUIM_DocumentView_Layout, layoutmodes[msg->layout].mode);
}

////

///	zoom

DEFMMETHOD(Toolbar_Zoom)
{
	GETDATA;

	char buff[32];
	float zoom = xget(data->document, MUIA_DocumentLayout_Zoom) / 65536.0f;
	int presnum;

	for(presnum = 0; presets[presnum] < zoom * 100.0f && presnum < (sizeof(presets) / sizeof(presets[0])); presnum++) {};

	if (data->zoomid >= MEN_ZOOM_PRESET0)
	{
		if (msg->change > 0)
			presnum++;
		else if (msg->change < 0 && presnum > 0)
			presnum--;
	}
	else	// move to nearest preset
	{
		if (msg->change > 0)
		{
		}
		else if (msg->change < 0 && presnum > 0)
			presnum--;
	}

	if (presnum >= (sizeof(presets) / sizeof(presets[0])))
		return 0;

	data->zoomid = MEN_ZOOM_PRESET0 + presnum;
	zoom = presets[presnum] / 100.0f;

	zoomFormat(data->zoomid, buff, sizeof(buff));
    SetAttrs(data->document,
		MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_Zoom,
		MUIA_DocumentLayout_Zoom, (int)(zoom * 65536),
		TAG_DONE);
	//nnset(data->strZoom, MUIA_String_Contents, buff);
	set(data->strZoom, MUIA_Text_Contents, buff);

	return 0;
}

////

///	zoom popup

DEFMMETHOD(Toolbar_ZoomPopup)
{
	GETDATA;

	char buff[32];
	float zoom = xget(data->document, MUIA_DocumentLayout_Zoom) / 65536.0f;
	int presnum, id;

	Object *menZoom;

	{
		int i;
		char *extra[] = {"Fit page", "Fit width"};
		int extraid[] = {MEN_ZOOM_FITPAGE, MEN_ZOOM_FITWIDTH};

		menZoom = MenuObject,
			MUIA_Menu_Title, "Zoom",
			End;

		for(i=0; i<sizeof(presets) / sizeof(presets[0]); i++)
		{
			Object *menPreset;
			char pval[16];
			snprintf(pval, sizeof(pval), "%.2f%%", presets[i]);

			menPreset = MenuitemObject,
				MUIA_Menuitem_Title, pval,
				MUIA_Menuitem_CopyStrings, TRUE,
				MUIA_UserData, MEN_ZOOM_PRESET0 + i,
				End;

			DoMethod(menZoom, MUIM_Family_AddTail, menPreset);
		}

		for(i=0; i<sizeof(extra) / sizeof(extra[0]); i++)
		{
			Object *menPreset;

			menPreset = MenuitemObject,
				MUIA_Menuitem_Title, extra[i],
				MUIA_UserData, extraid[i],
				End;

			DoMethod(menZoom, MUIM_Family_AddTail, menPreset);
		}
	}

	menZoom = MenustripObject, Child, menZoom, End;

#if defined(MUIM_Menustrip_Popup)
    // FIXME: AROS doesn't have MUIM_Menustrip_Popup
	id = DoMethod(menZoom, MUIM_Menustrip_Popup, data->btnZoomPopup, 0 , _left(data->btnZoomPopup),_bottom(data->btnZoomPopup)+1);
#endif

	if (id != 0)
	{
		char buff[32];

		data->zoomid = id;
		zoomFormat(data->zoomid, buff, sizeof(buff));
		set(data->strZoom, MUIA_Text_Contents, buff);

		if (id == MEN_ZOOM_FITWIDTH)
			set(data->document, MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_FitWidth);
		else if (id == MEN_ZOOM_FITPAGE)
			set(data->document, MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_FitPage);
		else
		{
			float zoom = presets[id - MEN_ZOOM_PRESET0] / 100.0f;

			SetAttrs(data->document,
				MUIA_DocumentLayout_Scaling, MUIV_DocumentLayout_Scaling_Zoom,
				MUIA_DocumentLayout_Zoom, (int)(zoom * 65536),
				TAG_DONE);
		}


	}

	MUI_DisposeObject(menZoom);

	return 0;
}

////

DEFMMETHOD(DocumentView_RotateLeft)
{
	GETDATA;
	return DoMethodA(data->document, msg);
}

DEFMMETHOD(DocumentView_RotateRight)
{
	GETDATA;
	return DoMethodA(data->document, msg);
}

DEFMMETHOD(Toolbar_ToggleOutline)
{
	GETDATA;
	set(data->document, MUIA_DocumentView_Outline, msg->visible);
	return TRUE;
}

DEFMMETHOD(Toolbar_DragAction)
{
	GETDATA;
	
	if (msg->dragaction == MUIV_DocumentView_DragAction_Mark)
		nnset(data->btnDragScroll, MUIA_Selected, FALSE);
	else
		nnset(data->btnDragMark, MUIA_Selected, FALSE);
	
	set(data->document, MUIA_DocumentView_DragAction, msg->dragaction);
	return TRUE;
}

BEGINMTABLE
	DECMMETHOD(Setup)
	DECMMETHOD(Cleanup)
	DECSET
	DECGET
	DECNEW
	DECDISP
	DECMMETHOD(Toolbar_Zoom)
	DECMMETHOD(Toolbar_Layout)
	DECMMETHOD(Toolbar_ZoomPopup)
	DECMMETHOD(DocumentView_RotateLeft)
	DECMMETHOD(DocumentView_RotateRight)
	DECMMETHOD(Toolbar_ToggleOutline)
	DECMMETHOD(Toolbar_DragAction)
ENDMTABLE

DECSUBCLASS_NC(MUIC_Group, ToolbarClass)

