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

#include "WebViewZune.h"
#include "tabbedgroup.h"
#include "tabbedview.h"
#include "tabbedview_private.h"

IPTR forwardedAttributes[] = 
{ 
    MUIA_WebView_ToolTip, 
    MUIA_WebView_EstimatedProgress,
    MUIA_WebView_IsLoading,
    MUIA_WebView_URL,
    MUIA_WebView_Title,
    MUIA_WebView_CanGoBack,
    MUIA_WebView_CanGoForward,
}; 

/*** Methods ****************************************************************/
IPTR TabbedView__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct TabbedView_DATA *data = NULL;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    data = INST_DATA(CLASS, self);

    /* Send notifications after changing active tab */
    DoMethod(self, MUIM_Notify, MUIA_Group_ActivePage, MUIV_EveryTime,
         (IPTR) self, 1,
         MUIM_TabbedView_TriggerNotifications);
    
    return (IPTR) self;
}

IPTR TabbedView__OM_ADDMEMBER(Class *cl, Object *obj, struct opMember *msg)
{
    struct TabbedView_DATA *data = (struct TabbedView_DATA *) INST_DATA(cl, obj);
    Object *webView = msg->opam_Object;
    int i;

    for(i = 0; i < sizeof(forwardedAttributes) / sizeof(IPTR); i++)
    {
        DoMethod(webView, MUIM_Notify, forwardedAttributes[i], MUIV_EveryTime,
            (IPTR) obj, (IPTR) 4,
            MUIM_TabbedView_ForwardAttribute, webView, forwardedAttributes[i], MUIV_TriggerValue);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR TabbedView__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct TabbedView_DATA *data = (struct TabbedView_DATA *) INST_DATA(cl, obj);
    IPTR retval = TRUE;

    switch(msg->opg_AttrID)
    {
        case MUIA_TabbedView_ActiveObject:
        {
            int currentPage = XGET(obj, MUIA_Group_ActivePage);
            struct List *children = (struct List *) XGET(obj, MUIA_Group_ChildList);
            Object *activeView;
        
            APTR state = children->lh_Head;
            do
            {
        	activeView = NextObject(&state);
            }
            while(currentPage--);
            
            *(Object**)msg->opg_Storage = activeView;
            break;
        }
    	default:
    	{
    	    int i;
            for(i = 0; i < sizeof(forwardedAttributes) / sizeof(IPTR); i++)
            {
        	if(forwardedAttributes[i] == msg->opg_AttrID)
            	{
            	    Object *activeView = (Object*) XGET(obj, MUIA_TabbedView_ActiveObject);
            	    retval = DoMethodA(activeView, (Msg) msg);
            	    return retval;
            	}
            }

	    retval = DoSuperMethodA(cl, obj, (Msg) msg);
	    break;
    	}
    }
    
    return retval;
}

IPTR TabbedView__MUIM_TabbedView_ForwardMethod(Class *cl, Object *obj, Msg message)
{
    Object *activeView = (Object*) XGET(obj, MUIA_TabbedView_ActiveObject);
    return DoMethodA(activeView, message);
}

static IPTR TabbedView__MUIM_TabbedView_ForwardAttribute(Class *cl, Object *obj, struct MUIP_TabbedView_ForwardAttribute *msg)
{
    Object *activeView = (Object*) XGET(obj, MUIA_TabbedView_ActiveObject);
    if(activeView == msg->sender)
    {
	int i;
	for(i = 0; i < sizeof(forwardedAttributes) / sizeof(IPTR); i++)
	{
	    if(forwardedAttributes[i] == msg->attribute)
	    {
		SetAttrs(obj, MUIA_Group_Forward, FALSE, msg->attribute, msg->value, TAG_END);
		break;
	    }
	}
    }
    return TRUE;
}

static IPTR TabbedView__MUIM_TabbedView_TriggerNotifications(Class *cl, Object *obj, Msg msg)
{
    Object *activeView = (Object*) XGET(obj, MUIA_TabbedView_ActiveObject);
    
    int i;
    for(i = 0; i < sizeof(forwardedAttributes) / sizeof(IPTR); i++)
    {
        SetAttrs(obj, MUIA_Group_Forward, FALSE, forwardedAttributes[i], XGET(activeView, forwardedAttributes[i]), TAG_END);
    }
    
    return TRUE;
}

__ZUNE_CUSTOMCLASS_START(TabbedView)
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__OM_NEW, OM_NEW, struct opSet*);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__OM_GET, OM_GET, struct opGet*);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__OM_ADDMEMBER, OM_ADDMEMBER, struct opMember*);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_LoadURL, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_GoBack, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_GoForward, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_StopLoading, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_Reload, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardMethod, MUIM_WebView_SearchFor, Msg);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_ForwardAttribute, MUIM_TabbedView_ForwardAttribute, struct MUIP_TabbedView_ForwardAttribute*);
__ZUNE_CUSTOMCLASS_METHOD(TabbedView__MUIM_TabbedView_TriggerNotifications, MUIM_TabbedView_TriggerNotifications, Msg);
__ZUNE_CUSTOMCLASS_END(TabbedView, NULL, NULL, TabbedGroup_CLASS)
