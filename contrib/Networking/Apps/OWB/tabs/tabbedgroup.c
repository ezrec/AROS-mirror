/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <dos/dos.h>
#include <libraries/mui.h>
#include <zune/customclasses.h>

#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <proto/alib.h>

#include "tabbedgroup.h"
#include "tabbedgroup_private.h"

/*** Methods ****************************************************************/
IPTR TabbedGroup__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct TabbedGroup_DATA *data = NULL; 
    struct TagItem *tag = NULL;
    struct TagItem *tstate = message->ops_AttrList;
    /* Lay tabs horizontally by default */
    BYTE direction = HORIZONTAL_TABBED;

    /* Parse initial attributes */
    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Group_Horiz:
        	if(tag->ti_Data)
        	    direction = HORIZONTAL_TABBED;
        	else
        	    direction = VERTICAL_TABBED;  
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL, MUIA_Group_PageMode, TRUE,
	TAG_MORE, (IPTR) message->ops_AttrList	
    );
    
    if (self == NULL)
	return (IPTR) NULL;
    
    data = INST_DATA(CLASS, self);
    data->direction = direction;
        
    return (IPTR) self;
}

IPTR TabbedGroup__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct TabbedGroup_DATA *data = INST_DATA(cl, obj);
    
    DoSuperMethodA(cl,obj,(Msg)msg);

    if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE)))
	return(0);
    
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);

    RectFill(_rp(obj), _right(obj),
    	_top(obj),
	_right(obj),
	_bottom(obj));
	     
    RectFill(_rp(obj), _left(obj),
    	_bottom(obj),
	_right(obj),
	_bottom(obj));

    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);

    if(data->direction == VERTICAL_TABBED)
    {
	RectFill(_rp(obj), _left(obj),
	    _top(obj),
	    _left(obj),
	    _bottom(obj));
    }

    if(data->direction == HORIZONTAL_TABBED)
    {
        RectFill(_rp(obj), _left(obj),
            _top(obj),
    	    _right(obj),
    	    _top(obj));
    }

    return TRUE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_2
(
    TabbedGroup, NULL, MUIC_Group, NULL,
    OM_NEW, struct opSet*,
    MUIM_Draw, struct MUIP_Draw*
)
