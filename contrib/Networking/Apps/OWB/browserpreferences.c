/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

/* Process this class after all the base
 * custom classes have been created
 */
#define ZUNE_CUSTOMCLASS_PASS   101

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <proto/intuition.h>
#include <proto/alib.h>
#include <proto/utility.h>
#include <aros/debug.h>
#include <zune/customclasses.h>

#include "WebPreferencesZune.h"

#include "browserpreferences_private.h"
#include "browserpreferences.h"

IPTR BrowserPreferences__OM_NEW(struct IClass *cl, Object *self, struct opSet *msg)
{
    STRPTR downloadDestination = StrDup("T:");
    if(!downloadDestination)
	return (IPTR) NULL;
    
    self = (Object *) DoSuperNewTags
    (
        cl, self, NULL,
        TAG_MORE, (IPTR) msg->ops_AttrList	
    );

    if(!self)
	return (IPTR) NULL;
    
    struct BrowserPreferences_DATA *data = (struct BrowserPreferences_DATA *) INST_DATA(cl, self);
    data->downloadDestination = downloadDestination;
    data->requestDownloadedFileName = FALSE;
    
    return (IPTR) self;
}

IPTR BrowserPreferences__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct BrowserPreferences_DATA *data = (struct BrowserPreferences_DATA *) INST_DATA(cl, obj);
    FreeVec(data->downloadDestination);
    return DoSuperMethodA(cl,obj,msg);
}

IPTR BrowserPreferences__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct BrowserPreferences_DATA *data = (struct BrowserPreferences_DATA *) INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;

    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
            case MUIA_BrowserPreferences_DownloadDestination:
        	if(data->downloadDestination)
        	    FreeVec(data->downloadDestination);
        	data->downloadDestination = StrDup((STRPTR) tag->ti_Data);
        	break;
            case MUIA_BrowserPreferences_RequestDownloadedFileName:
        	data->requestDownloadedFileName = (BOOL) tag->ti_Data;
        	break;
	} /* switch(tag->ti_Tag) */
	
    } /* while ((tag = NextTagItem(&tags)) != NULL) */
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR BrowserPreferences__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct BrowserPreferences_DATA *data = (struct BrowserPreferences_DATA *) INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
        case MUIA_BrowserPreferences_DownloadDestination:
            *(msg->opg_Storage) = (IPTR) data->downloadDestination;
            break;
        case MUIA_BrowserPreferences_RequestDownloadedFileName:
            *(BOOL*) msg->opg_Storage = data->requestDownloadedFileName;
            break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

ZUNE_CUSTOMCLASS_4(
    BrowserPreferences, NULL, NULL, WebPreferences_CLASS,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    OM_SET, struct opSet*,
    OM_GET, struct opGet*
)
