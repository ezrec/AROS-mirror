/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <dos/dos.h>
#include <libraries/mui.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <zune/customclasses.h>

#include "tab.h"
#include "tabs.h"
#include "tab_private.h"

/*** Methods ****************************************************************/
IPTR Tab__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct Tab_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    
    int frameWidth = 5;
    int frameHeight = 5;
    BYTE active = 0;

    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Tab_Active:
        	active = tag->ti_Data;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Frame, MUIV_Frame_None,
	MUIA_InnerLeft, (IPTR) frameWidth,
	MUIA_InnerRight, (IPTR) frameWidth,
	MUIA_InnerTop, (IPTR) frameHeight,
	MUIA_InnerBottom, (IPTR) frameHeight - 1,
	MUIA_Group_Horiz, TRUE,
	MUIA_Group_HorizSpacing, 0,
	MUIA_ShowSelState, FALSE,
        TAG_MORE, (IPTR) message->ops_AttrList	
    );
    
    if (self == NULL)
	return (IPTR) NULL;
    
    data = INST_DATA(CLASS, self);
    data->frameWidth = frameWidth;
    data->frameHeight = frameHeight;
    data->active = active;
    data->nodraw = 0;

    return (IPTR) self;
}

IPTR Tab__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct Tab_DATA *data = INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;
    
    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_Tab_Active:
                data->active = (BYTE) tag->ti_Data;
    		break;
    	    default:
    		continue;
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR Tab__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct Tab_DATA *data = INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
    	case MUIA_Tab_Active:
    	    *(BYTE*)msg->opg_Storage = data->active;
    	    break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

/* Similar like in Register class */
static void DrawTopTab(Object *obj, struct Tab_DATA *data, WORD x1, WORD y1, WORD x2, WORD y2)
{
    if (!data->active)
    {
	/* Clear the rounded edges of an unactive tab with default background */
	nnset(obj, MUIA_Background, data->background);
	
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y1,
		 (IPTR) 1, (IPTR) 3,
		 (IPTR) x1, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1 + 1, (IPTR) y1,
		 (IPTR) 1, (IPTR) 2,
		 (IPTR) x1 + 1, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1 + 2, (IPTR) y1,
		 (IPTR) 1, (IPTR) 1,
		 (IPTR) x1 + 2, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x2 - 3, (IPTR) y1,
		 (IPTR) 3, (IPTR) 1,
		 (IPTR) x2 - 3, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x2 - 1, (IPTR) y1,
		 (IPTR) 1, (IPTR) 2,
		 (IPTR) x2 - 1, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x2, (IPTR) y1,
		 (IPTR) 1, (IPTR) 3,
		 (IPTR) x2, (IPTR) y1, (IPTR) 0);

	nnset(obj, MUIA_Background, MUII_BACKGROUND);	
    }

    /* top horiz bar */
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
    RectFill(_rp(obj), x1 + 4, y1, x2 - 4, y1);
    /* left vert bar */
    RectFill(_rp(obj), x1, y1 + 4, x1, y2 - (data->active ? 2 : 1));
    WritePixel(_rp(obj), x1 + 1, y1 + 3);
    WritePixel(_rp(obj), x1 + 1, y1 + 2);
    WritePixel(_rp(obj), x1 + 2, y1 + 1);
    WritePixel(_rp(obj), x1 + 3, y1 + 1);
    SetAPen(_rp(obj), _pens(obj)[MPEN_HALFSHINE]);
    WritePixel(_rp(obj), x1 + 3, y1);
    WritePixel(_rp(obj), x1 + 4, y1 + 1);
    WritePixel(_rp(obj), x1 + 2, y1 + 2);
    WritePixel(_rp(obj), x1 + 3, y1 + 2);
    WritePixel(_rp(obj), x1 + 2, y1 + 3);
    WritePixel(_rp(obj), x1, y1 + 3);
    WritePixel(_rp(obj), x1 + 1, y1 + 4);

    if (data->active)
    {
	/* bottom horiz bar */
	SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
	WritePixel(_rp(obj), x1, y2 - 1);	    
	WritePixel(_rp(obj), x1, y2);

	SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
	WritePixel(_rp(obj), x2, y2 - 1);	    
	WritePixel(_rp(obj), x2, y2);
    }
    else
    {
	SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
	RectFill(_rp(obj), x1, y2, x2, y2);	
    }

    /* right vert bar */
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
    WritePixel(_rp(obj), x2 - 1, y1 + 2);
    RectFill(_rp(obj), x2, y1 + 4, x2, y2 - (data->active ? 2 : 1));
    SetAPen(_rp(obj), _pens(obj)[MPEN_HALFSHADOW]);
    WritePixel(_rp(obj), x2 - 2, y1 + 1);
    WritePixel(_rp(obj), x2 - 1, y1 + 3);
    WritePixel(_rp(obj), x2, y1 + 3);
    SetAPen(_rp(obj), _pens(obj)[MPEN_BACKGROUND]);
    WritePixel(_rp(obj), x2 - 3, y1 + 1);
}

/* Like above, just symetrically flipped */
static void DrawLeftTab(Object *obj, struct Tab_DATA *data, WORD x1, WORD y1, WORD x2, WORD y2)
{
    if (!data->active)
    {
	/* Clear the rounded edges of an unactive tab with default background */
	nnset(obj, MUIA_Background, data->background);
	
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y1,
		 (IPTR) 3, (IPTR) 1,
		 (IPTR) x1, (IPTR) y1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y1 + 1,
		 (IPTR) 2, (IPTR) 1,
		 (IPTR) x1, (IPTR) y1 + 1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y1 + 2,
		 (IPTR) 1, (IPTR) 1,
		 (IPTR) x1, (IPTR) y1 + 2, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y2 - 2,
		 (IPTR) 1, (IPTR) 1,
		 (IPTR) x1, (IPTR) y2 - 2, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y2 - 1,
		 (IPTR) 2, (IPTR) 1,
		 (IPTR) x1, (IPTR) y2 - 1, (IPTR) 0);
	DoMethod(obj,MUIM_DrawBackground, (IPTR) x1, (IPTR) y2,
		 (IPTR) 3, (IPTR) 1,
		 (IPTR) x1, (IPTR) y2, (IPTR) 0);

	nnset(obj, MUIA_Background, MUII_BACKGROUND);	
    }

    /* left vert bar */
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
    RectFill(_rp(obj), x1, y1 + 4, x1, y2 - 4);
    /* top horiz bar */
    RectFill(_rp(obj), x1 + 4, y1, x2 - (data->active ? 2 : 1), y1);
    WritePixel(_rp(obj), x1 + 1, y1 + 3);
    WritePixel(_rp(obj), x1 + 1, y1 + 2);
    WritePixel(_rp(obj), x1 + 2, y1 + 1);
    WritePixel(_rp(obj), x1 + 3, y1 + 1);
    SetAPen(_rp(obj), _pens(obj)[MPEN_HALFSHINE]);
    WritePixel(_rp(obj), x1 + 3, y1);
    WritePixel(_rp(obj), x1 + 4, y1 + 1);
    WritePixel(_rp(obj), x1 + 2, y1 + 2);
    WritePixel(_rp(obj), x1 + 3, y1 + 2);
    WritePixel(_rp(obj), x1 + 2, y1 + 3);
    WritePixel(_rp(obj), x1, y1 + 3);
    WritePixel(_rp(obj), x1 + 1, y1 + 4);

    if (data->active)
    {
	/* bottom horiz bar */
	SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
	WritePixel(_rp(obj), x2 - 1, y1);	    
	WritePixel(_rp(obj), x2, y1); 

	SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
	WritePixel(_rp(obj), x2 - 1, y2);	    
	WritePixel(_rp(obj), x2, y2);
    }
    else
    {
	SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
	RectFill(_rp(obj), x2, y1, x2, y2);	
    }

    /* bottom horiz bar */
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
    WritePixel(_rp(obj), x1 + 2, y2 - 1);
    RectFill(_rp(obj), x1 + 4, y2, x2 - (data->active ? 2 : 1), y2);
    SetAPen(_rp(obj), _pens(obj)[MPEN_HALFSHADOW]);
    WritePixel(_rp(obj), x1 + 1, y2 - 2);
    WritePixel(_rp(obj), x1 + 3, y2 - 1);
    WritePixel(_rp(obj), x1 + 3, y2);
    SetAPen(_rp(obj), _pens(obj)[MPEN_BACKGROUND]);
    WritePixel(_rp(obj), x1 + 1, y2 - 3);
}

/* Drawing code */
IPTR Tab__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct Tab_DATA *data = INST_DATA(cl, obj);
    Object *parent = _parent(obj);
    WORD x1 = _left(obj) ;
    WORD y1 = _top(obj);
    WORD x2 = _right(obj);
    WORD y2 = _bottom(obj);
    ULONG location = XGET(parent, MUIA_Tabs_Location);

    if(data->nodraw)
	return 1;

    /* Zune does MUI_Redraw(obj, MADF_DRAWOBJECT) when setting new background,
       we don't need it */
    data->nodraw = 1;

    if (data->active)
    {
	nnset(obj, MUIA_Background, data->background);
    }
    else
    {
	nnset(obj, MUIA_Background, MUII_BACKGROUND);	
    }

    /* Draw all child objects */
    DoSuperMethodA(cl, obj, (Msg) msg);

    /* Draw tab frame */
    if(location == MUIV_Tabs_Top)
	DrawTopTab(obj, data, x1, y1, x2, y2);
    else if(location == MUIV_Tabs_Left)
	DrawLeftTab(obj, data, x1, y1, x2, y2);

    data->nodraw = 0;
    
    return TRUE;
}

IPTR Tab__MUIM_Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct Tab_DATA *data = INST_DATA(cl, obj);
    
    if(!DoSuperMethodA(cl, obj, (Msg) msg))
	return FALSE;

    get(obj, MUIA_Background, &data->background);

    return TRUE;
}

ZUNE_CUSTOMCLASS_5(
    Tab, NULL, MUIC_Group, NULL,
    OM_NEW, struct opSet*,
    OM_SET, struct opSet*,
    OM_GET, struct opGet*,
    MUIM_Setup, struct MUIP_Setup*,
    MUIM_Draw, struct MUIP_Draw *
)
