/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
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
#include <aros/debug.h>

#include "tab.h"
#include "tabs.h"
#include "tabs_private.h"

#define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#define _isinobject(x,y,obj) (_between(_left(obj),(x),_right (obj)) \
                          && _between(_top(obj) ,(y),_bottom(obj)))

ULONG Tabs_Layout_Function(struct Hook *hook, Object *obj, struct MUI_LayoutMsg *lm)
{
    struct Tabs_DATA *data = (struct Tabs_DATA *) hook->h_Data;
    switch (lm->lm_Type)
    {
        case MUILM_MINMAX:
        {
	    Object *cstate = (Object *)lm->lm_Children->mlh_Head;
    	    Object *child;
    
    	    WORD maxminwidth  = 0;
    	    WORD maxminheight = 0;
    	    WORD mintotalwidth = 0;
    	    WORD mintotalheight = 0;
    	    LONG number_of_children = 0;
    
    	    /* find out biggest widths & heights of our children */
    
    	    while((child = NextObject(&cstate)))
    	    {
    		if(maxminwidth < MUI_MAXMAX && _minwidth(child) > maxminwidth)
    		    maxminwidth  = _minwidth(child);

    		if(maxminheight < MUI_MAXMAX && _minheight(child) > maxminheight)
    		    maxminheight = _minheight(child);

    		mintotalheight += _minheight(child);
    		
    		number_of_children++;
    	    }
    
    	    if(data->location == MUIV_Tabs_Top)
    	    {
    		mintotalwidth = number_of_children * maxminwidth + (number_of_children - 1) * XGET(obj, MUIA_Group_HorizSpacing);
    		lm->lm_MinMax.MinWidth = lm->lm_MinMax.DefWidth = mintotalwidth;
    		lm->lm_MinMax.MinHeight = lm->lm_MinMax.DefHeight = maxminheight + data->protrusion;
    		lm->lm_MinMax.MaxWidth  = MUI_MAXMAX;
    	    	lm->lm_MinMax.MaxHeight = lm->lm_MinMax.DefHeight = maxminheight + data->protrusion;    		
    	    }
    	    else if(data->location == MUIV_Tabs_Left)
    	    {
    		mintotalheight += (number_of_children - 1) * XGET(obj, MUIA_Group_VertSpacing);
    		lm->lm_MinMax.MinWidth = lm->lm_MinMax.DefWidth = maxminwidth + data->protrusion;
    		lm->lm_MinMax.MinHeight = lm->lm_MinMax.DefHeight = mintotalheight + data->protrusion;
    		lm->lm_MinMax.MaxWidth  = lm->lm_MinMax.DefWidth = maxminwidth + data->protrusion;
    	    	lm->lm_MinMax.MaxHeight = MUI_MAXMAX;
    	    }

    	    return 0;
        }
        case MUILM_LAYOUT:
        {
            APTR cstate;
            Object *child;
    	    LONG number_of_children = 0;

    	    cstate = lm->lm_Children->mlh_Head;
            while((child = NextObject(&cstate)))
            {
        	number_of_children++;
            }

            if(data->location == MUIV_Tabs_Top)
            {
                WORD horiz_spacing = XGET(obj, MUIA_Group_HorizSpacing);
                WORD childwidth = (lm->lm_Layout.Width - (number_of_children - 1) * horiz_spacing) / number_of_children;
                WORD leftovers = lm->lm_Layout.Width - (number_of_children - 1) * horiz_spacing - number_of_children * childwidth;
                WORD left = 0;
                cstate = lm->lm_Children->mlh_Head;
                while((child = NextObject(&cstate)))
                {
                    WORD cwidth = childwidth;
                    WORD cheight = _height(obj);
                    if(leftovers-- > 0)
                	cwidth++;
                    if(!XGET(child, MUIA_Tab_Active))
                	cheight -= data->protrusion;
                    if(!MUI_Layout(child, left, lm->lm_Layout.Height - cheight, cwidth, cheight, 0))
                        return(FALSE);
                    
                    left += cwidth + horiz_spacing;
                }
            }
            else if(data->location == MUIV_Tabs_Left)
            {
                WORD vert_spacing = XGET(obj, MUIA_Group_VertSpacing);
                WORD top = 0;
                cstate = lm->lm_Children->mlh_Head;
                while((child = NextObject(&cstate)))
                {
                    WORD cheight = _minheight(child);
                    if(XGET(child, MUIA_Tab_Active))
                	cheight += data->protrusion;
                    
                    if(!MUI_Layout(child, 0, top, lm->lm_Layout.Width, cheight, 0))
                        return(FALSE);
                    
                    top += cheight + vert_spacing;
                }
            }
            else
        	return FALSE;
            
            return TRUE;
        }
    }
    return TRUE;
}

IPTR Tabs__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct Tabs_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    struct Hook *layout_hook;
    Object *firstTab = NULL;
    Object *closeButton = NULL;

    /* Lay tabs horizontally by default */
    LONG location = MUIV_Tabs_Top;
    
    layout_hook = AllocVec(sizeof(struct Hook), MEMF_ANY | MEMF_CLEAR);
    if (!layout_hook) return (IPTR) NULL;

    layout_hook->h_Entry = HookEntry;
    layout_hook->h_SubEntry = (HOOKFUNC)Tabs_Layout_Function;

    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem( &tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Tabs_Location:
        	location = tag->ti_Data;
        	break;
            case MUIA_Tabs_CloseButton:
        	closeButton = (Object*) tag->ti_Data;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Group_LayoutHook, (IPTR) layout_hook,
	TAG_MORE, (IPTR) message->ops_AttrList	
    );
    
    if (self == NULL)
    {
	FreeVec(layout_hook);
	CoerceMethod(CLASS, self, OM_DISPOSE);
	return (IPTR) NULL;
    }

    data = INST_DATA(CLASS, self);
    layout_hook->h_Data = data;
    data->layout_hook = layout_hook;
    data->location = location;
    data->protrusion = 4;
    data->closeButton = closeButton;

    /* We need tab events to be processed after all objects contained in tabs
       like for example close button, hence the low priority value */
    data->ehn.ehn_Events = IDCMP_MOUSEBUTTONS;
    data->ehn.ehn_Priority = -7;
    data->ehn.ehn_Flags    = 0;
    data->ehn.ehn_Object   = self;
    data->ehn.ehn_Class    = CLASS;

    struct List *children = (struct List*) XGET(self, MUIA_Group_ChildList);
    APTR cstate = children->lh_Head;
    firstTab = NextObject(&cstate);
    data->activeObject = firstTab;
    set(firstTab, MUIA_Tab_Active, TRUE);
    
    if(closeButton)
    {
        DoMethod(firstTab, MUIM_Group_InitChange);
        DoMethod(firstTab, OM_ADDMEMBER, closeButton);
        DoMethod(firstTab, MUIM_Group_ExitChange);
    }

    return (IPTR) self;
}

IPTR Tabs__OM_DISPOSE
(
    Class *CLASS, Object *self, Msg message 
)
{
    struct Tabs_DATA *data   = INST_DATA(CLASS, self);
    FreeVec(data->layout_hook);
    return DoSuperMethodA(CLASS, self, message);
}

IPTR Tabs__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct Tabs_DATA *data = INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;
    
    while ((tag = NextTagItem( &tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_Tabs_ActiveTab:
    	    {
        	struct List *children = (struct List*) XGET(obj, MUIA_Group_ChildList);
        	APTR cstate = children->lh_Head;
        	ULONG active = (ULONG) tag->ti_Data;
        	Object *child;
                do
                {
                    child = NextObject(&cstate);
                }
                while(child && active--);
                if(child)
                {
                    /* Change the active tab, reconnect close button if it's present */
    		    DoMethod(obj, MUIM_Group_InitChange);
                    set(data->activeObject, MUIA_Tab_Active, FALSE);
                    if(data->closeButton)
                	DoMethod(data->activeObject, OM_REMMEMBER, data->closeButton);
                    data->activeObject = child;
    		    set(data->activeObject, MUIA_Tab_Active, TRUE);
    		    if(data->closeButton)
    			DoMethod(data->activeObject, OM_ADDMEMBER, data->closeButton);
    		    DoMethod(obj, MUIM_Group_ExitChange);
                }
                break;
    	    }          
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR Tabs__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct Tabs_DATA *data = INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
        case MUIA_Tabs_Location:
            *(ULONG*)msg->opg_Storage = data->location;
            break;
        case MUIA_Tabs_ActiveTab:
        {
	    struct List *children;
	    Object *child;
	    APTR cstate;
	    ULONG active = 0;
	    get(obj, MUIA_Group_ChildList, &children);
            cstate = children->lh_Head;
            while((child = NextObject(&cstate)))
            {
        	if(child == data->activeObject)
        	{
                    *(ULONG*)msg->opg_Storage = active;
        	    break;
        	}
        	active++;
            }
            break;
        }
        case MUIA_Tabs_ActiveTabObject:
        {
	    struct List *children;
	    APTR cstate;
	    get(data->activeObject, MUIA_Group_ChildList, &children);
            cstate = children->lh_Head;
            *(Object**)msg->opg_Storage = NextObject(&cstate);
            break;
        }
        case MUIA_Tabs_CloseButton:
            *(Object**)msg->opg_Storage = data->closeButton;
            break;
     	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

IPTR Tabs__MUIM_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
    struct Tabs_DATA *data = INST_DATA(cl, obj);
    struct List *children;
    APTR cstate;
    Object *child;
    WORD horiz_spacing = XGET(obj, MUIA_Group_HorizSpacing);
    WORD vert_spacing = XGET(obj, MUIA_Group_VertSpacing);
    
    /* Draw all the children */
    DoSuperMethodA(cl,obj,(Msg)msg);

    if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE)))
	return(0);
    
    /* Now draw missing TabbedGroup border between the spaces */
    get(obj, MUIA_Group_ChildList, &children);
    cstate = children->lh_Head;
    
    child = NextObject(&cstate);
    SetAPen(_rp(obj), _pens(obj)[MPEN_SHINE]);
    if(data->location == MUIV_Tabs_Top)
    {
        while(child && (child = NextObject(&cstate)))
        {
    	    RectFill(_rp(obj), _left(child) - horiz_spacing, _bottom(child), _left(child) - 1, _bottom(child));
        }
    }
    else if(data->location == MUIV_Tabs_Left)
    {
	WORD lasty;
        while(child && (child = NextObject(&cstate)))
        {
    	    RectFill(_rp(obj), _right(child), _top(child) - vert_spacing, _right(child), _top(child) - 1);
    	    lasty = _bottom(child);
        }
        RectFill(_rp(obj), _right(obj), lasty + 1, _right(obj), _bottom(obj));
    }
    else
	return FALSE;

    SetAPen(_rp(obj), _pens(obj)[MPEN_SHADOW]);
    WritePixel(_rp(obj), _right(obj), _bottom(obj));

    return TRUE;
}

IPTR Tabs__OM_ADDMEMBER(struct IClass *cl, Object *obj, struct opMember *msg)
{
    DoSuperMethod(cl, obj, OM_ADDMEMBER, (IPTR) NewObject(Tab_CLASS->mcc_Class, NULL, Child, msg->opam_Object, TAG_END));
    return TRUE;
}

IPTR Tabs__OM_REMMEMBER(struct IClass *cl, Object *obj, struct opMember *msg)
{
    struct List *children = (struct List*) XGET(obj, MUIA_Group_ChildList);
    APTR cstate = children->lh_Head;
    Object *child, *prev = NULL, *next = NULL;
    int tabn;
 
    /* Find previous and next tab */
    for(tabn = 0; (child = NextObject(&cstate)); tabn++)
    {
	struct List *tabChildren = (struct List*) XGET(child, MUIA_Group_ChildList);
	APTR tabcstate = tabChildren->lh_Head;
	if(NextObject(&tabcstate) == msg->opam_Object)
	    break;
	prev = child;
    }
    if(!child)
	return FALSE;

    next = NextObject(&cstate);
    
    /* Last tab can't be closed */
    if(!prev && !next)
	return FALSE;

    DoSuperMethod(cl, obj, OM_REMMEMBER, child);

    /* Activate next tab if it's present, otherwise previous */
    if(next)
	set(obj, MUIA_Tabs_ActiveTab, tabn);
    else
	set(obj, MUIA_Tabs_ActiveTab, tabn - 1);

    /* Dispose of Tab object, user is responsible for deleting the tab contents */
    DoMethod(child, MUIM_Group_InitChange);
    DoMethod(child, OM_REMMEMBER, msg->opam_Object);
    DoMethod(child, MUIM_Group_ExitChange);
    MUI_DisposeObject(child);

    return TRUE;
}

IPTR Tabs__MUIM_Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct Tabs_DATA *data = INST_DATA(cl, obj);
    
    if(!DoSuperMethodA(cl, obj, (Msg) msg))
	return FALSE;

    DoMethod(_win(obj), MUIM_Window_AddEventHandler, (IPTR)&data->ehn);
    return TRUE;
}

IPTR Tabs__MUIM_Cleanup(struct IClass *cl, Object *obj, struct MUIP_Cleanup *msg)
{
    struct Tabs_DATA *data = INST_DATA(cl, obj);

    DoMethod(_win(obj), MUIM_Window_RemEventHandler, (IPTR)&data->ehn);

    return DoSuperMethodA(cl, obj, (Msg) msg);
}

IPTR Tabs__MUIM_HandleEvent(struct IClass *cl, Object *obj, struct MUIP_HandleEvent *msg)
{
    if (!msg->imsg)
        return 0;

    switch (msg->imsg->Class)
    {
        case IDCMP_MOUSEBUTTONS:
            if (msg->imsg->Code == SELECTDOWN && _isinobject(msg->imsg->MouseX, msg->imsg->MouseY,obj))
            {
                struct List *children = (struct List*) XGET(obj, MUIA_Group_ChildList);
                APTR cstate = children->lh_Head;
                Object *child;
                int i;
             
                /* Find previous and next tab */
                for(i = 0; (child = NextObject(&cstate)); i++)
                {
                    if(_isinobject(msg->imsg->MouseX, msg->imsg->MouseY,child))
                    {
                	/* Activate this tab */
                	set(obj, MUIA_Tabs_ActiveTab, i);
                    }
                }
            }
            break;
    }
    return 0;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_10
(
    Tabs, NULL, MUIC_Group, NULL,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    OM_GET, struct opGet*,
    OM_SET, struct opSet*,
    MUIM_Draw, struct MUIP_Draw*,
    OM_ADDMEMBER, struct opMember*,
    OM_REMMEMBER, struct opMember*,
    MUIM_Setup, struct MUIP_Setup*,
    MUIM_Cleanup, struct MUIP_Cleanup*,
    MUIM_HandleEvent, struct MUIP_HandleEvent*
)

