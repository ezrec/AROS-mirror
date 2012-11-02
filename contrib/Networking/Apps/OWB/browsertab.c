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
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <zune/customclasses.h>

#include "browsertab.h"
#include "browsertab_private.h"

#include "locale.h"

struct MUI_CustomClass *BrowserTabClass;


/*** Methods ****************************************************************/
IPTR BrowserTab__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct BrowserTab_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    Object *titleObject;   
    STRPTR title = NULL;

    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_BrowserTab_Title:
        	title = StrDup((STRPTR) tag->ti_Data);
        	if(!title)
        	    return (IPTR) NULL;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }
    
    if(title == NULL)
    {
	title = StrDup(_(MSG_Untitled));
	if(!title)
	    return (IPTR) NULL;
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Frame, MUIV_Frame_None,
	MUIA_InnerLeft, 0,
	MUIA_InnerRight, 0,
	MUIA_InnerTop, 0,
	MUIA_InnerBottom, 0,
	MUIA_Group_Horiz, TRUE,
	MUIA_Group_HorizSpacing, 0,
	MUIA_ShowSelState, FALSE,
	Child, (IPTR)(titleObject = TextObject,
            MUIA_Frame, MUIV_Frame_None,
      	    MUIA_Text_SetMin, FALSE,
            MUIA_Text_Contents, (IPTR)title,
            End),
        TAG_MORE, (IPTR) message->ops_AttrList	
    );
    
    if (self == NULL)
	return (IPTR) NULL;
    
    data = INST_DATA(CLASS, self);
    data->title = title;
    data->titleObject = titleObject;

    return (IPTR) self;
}

IPTR BrowserTab__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct BrowserTab_DATA *data = INST_DATA(CLASS, self);
    IPTR ret;
    ret = DoSuperMethodA(CLASS, self, message);
    FreeVec(data->title);
    return ret;
}

IPTR BrowserTab__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct BrowserTab_DATA *data = INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;
    
    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_BrowserTab_Title:
    	    {
    		if(tag->ti_Data == (IPTR) NULL || ((char*) tag->ti_Data)[0] == 0)
    		    tag->ti_Data = (IPTR) _(MSG_Untitled);
    		STRPTR newtitle = StrDup((STRPTR) tag->ti_Data);
    		if(!newtitle)
    		    return FALSE;
    		FreeVec(data->title);
    		data->title = newtitle;
    		set(data->titleObject, MUIA_Text_Contents, data->title);
    		break;
    	    }
    	    default:
    		continue;
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR BrowserTab__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct BrowserTab_DATA *data = INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
    	case MUIA_BrowserTab_Title:
    	    *(STRPTR*)msg->opg_Storage = data->title;
    	    break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_4
(
    BrowserTab, NULL, MUIC_Group, NULL,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    OM_SET, struct opSet*,
    OM_GET, struct opGet*
)
