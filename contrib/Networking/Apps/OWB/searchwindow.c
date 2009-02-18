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

#include "searchwindow.h"
#include "searchwindow_private.h"

#include "locale.h"

/*** Methods ****************************************************************/
IPTR SearchWindow__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct SearchWindow_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    Object *located_string, *next_button, *previous_button, *case_checkmark;
    struct Hook *search_hook;
    
    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem((const struct TagItem **)&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            case MUIA_SearchWindow_SearchHook:
        	search_hook = (Object*) tag->ti_Data;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Window_Open, FALSE,
        MUIA_Window_Title, _(MSG_SearchWindow_Title),
        WindowContents, VGroup,
            MUIA_InnerLeft, 5,
            MUIA_InnerRight, 5,
            MUIA_InnerTop, 5,
            MUIA_InnerBottom, 5,
            Child, HGroup,
            	Child, Label2(_(MSG_SearchWindow_Locate)),
            	Child, located_string = StringObject,
            	    MUIA_HorizWeight, 60,
            	    MUIA_Frame, MUIV_Frame_String,
            	    End,
            	End,
            Child, HGroup,
            	Child, Label1(_(MSG_SearchWindow_CaseSensitive)),
            	Child, case_checkmark = MUI_MakeObject(MUIO_Checkmark, FALSE),
            	End,
            Child, HGroup,
    	        Child, next_button = SimpleButton(_(MSG_SearchWindow_FindNext)),
    	        Child, previous_button = SimpleButton(_(MSG_SearchWindow_FindPrevious)),
    	        End,
            End,
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    /* Click Close window with close gadget */
    DoMethod(self, MUIM_Notify, MUIA_Window_CloseRequest, (IPTR) TRUE,
        (IPTR) self,  (IPTR) 3,
        MUIM_Set, MUIA_Window_Open, (IPTR) FALSE);

    DoMethod(next_button, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 2,
	MUIM_SearchWindow_Find, 1);

    DoMethod(previous_button, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 2,
	MUIM_SearchWindow_Find, 0);

    data = INST_DATA(CLASS, self);
    data->next_button = next_button;
    data->previous_button = previous_button;
    data->case_checkmark = case_checkmark;
    data->located_string = located_string;
    data->search_hook = search_hook;
    
    return (IPTR) self;
}

IPTR SearchWindow__MUIM_SearchWindow_Find(struct IClass *cl, Object *obj, struct MUIP_SearchWindow_Find *msg)
{
    struct SearchWindow_DATA *data = (struct SearchWindow_DATA *) INST_DATA(cl, obj);
    struct OWB_SearchMsg searchMsg;
    searchMsg.forward = msg->forward;
    get(data->located_string, MUIA_String_Contents, &searchMsg.criteria);
    get(data->case_checkmark, MUIA_Pressed, &searchMsg.caseSensitive);
    
    D(bug("criteria: %s, direction: %d, case sensitive: %d\n", searchMsg.criteria, searchMsg.forward, searchMsg.caseSensitive));
    
    if(data->search_hook)
	DoMethod(obj, MUIM_CallHook, data->search_hook, &searchMsg);
    
    return TRUE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_2
(
    SearchWindow, NULL, MUIC_Window, NULL,
    OM_NEW, struct opSet*, 
    MUIM_SearchWindow_Find, struct MUIP_SearchWindow_Find *
)
