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

#include <WebViewZune.h>

#include "searchbar.h"
#include "searchbar_private.h"

#include "browserwindow.h"

#include "locale.h"

/*** Methods ****************************************************************/
IPTR SearchBar__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct SearchBar_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    Object *located_string, *next_button, *previous_button, *case_checkmark;
    Object *bt_close;
    
    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Group_Horiz, TRUE,
        MUIA_ShowMe, FALSE,
        Child, (IPTR)(bt_close = HGroup,
	    MUIA_Frame, MUIV_Frame_ImageButton,
	    MUIA_InputMode, MUIV_InputMode_RelVerify,
	    Child, (IPTR)(ImageObject,
		MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/close.png",
		End),
	    End),
       	Child, (IPTR)(Label2(_(MSG_SearchWindow_Locate))),
        Child, (IPTR)(located_string = StringObject,
            MUIA_HorizWeight, 100,
            MUIA_Frame, MUIV_Frame_String,
            End),
       	Child, (IPTR)(Label1(_(MSG_SearchWindow_CaseSensitive))),
        Child, (IPTR)(case_checkmark = MUI_MakeObject(MUIO_Checkmark, FALSE)),
    	Child, (IPTR)(previous_button = SimpleButton(_(MSG_SearchWindow_FindPrevious))),
    	Child, (IPTR)(next_button = SimpleButton(_(MSG_SearchWindow_FindNext))),
    	Child, (IPTR)(HSpace(0)),
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    /* Click Close window with close gadget */
    DoMethod(bt_close, MUIM_Notify, MUIA_Pressed, (IPTR) FALSE,
        (IPTR) self,  (IPTR) 3,
        MUIM_Set, MUIA_ShowMe, (IPTR) FALSE);

    DoMethod(next_button, MUIM_Notify, MUIA_Pressed, (IPTR) FALSE,
	(IPTR) self, 2,
	MUIM_SearchBar_Find, (IPTR) 1);

    DoMethod(previous_button, MUIM_Notify, MUIA_Pressed, (IPTR) FALSE,
	(IPTR) self, 2,
	MUIM_SearchBar_Find, (IPTR) 0);

    DoMethod(located_string, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
	(IPTR) self, 2,
	MUIM_SearchBar_Find, (IPTR) 1);
    
    data = INST_DATA(CLASS, self);
    data->next_button = next_button;
    data->previous_button = previous_button;
    data->case_checkmark = case_checkmark;
    data->located_string = located_string;
    
    return (IPTR) self;
}

IPTR SearchBar__MUIM_SearchBar_Find(struct IClass *cl, Object *obj, struct MUIP_SearchBar_Find *msg)
{
    struct SearchBar_DATA *data = (struct SearchBar_DATA *) INST_DATA(cl, obj);
    struct OWB_SearchMsg searchMsg = {};
    searchMsg.forward = msg->forward;
    get(data->located_string, MUIA_String_Contents, &searchMsg.criteria);
    get(data->case_checkmark, MUIA_Pressed, &searchMsg.caseSensitive);
    
    D(bug("criteria: %s, direction: %d, case sensitive: %d\n", searchMsg.criteria, searchMsg.forward, searchMsg.caseSensitive));
    
    Object* webView = (Object *)XGET(_win(obj), MUIA_BrowserWindow_WebView);
    if(webView)
    {
	DoMethod(webView, MUIM_WebView_SearchFor, searchMsg.criteria, (IPTR) searchMsg.forward, (IPTR) searchMsg.caseSensitive);
    }
    
    return TRUE;
}

IPTR SearchBar__MUIM_Setup(struct IClass *cl, Object *obj, struct MUIP_Setup *msg)
{
    struct SearchBar_DATA *data = (struct SearchBar_DATA *) INST_DATA(cl, obj);
    
    if (!DoSuperMethodA(cl, obj, (Msg)msg))
	return FALSE;
    
    DoMethod(obj, MUIM_Notify, MUIA_ShowMe, (IPTR) TRUE,
	(IPTR) _win(obj), (IPTR) 3,
	MUIM_Set, MUIA_Window_ActiveObject, data->located_string);

    DoMethod(_win(obj), MUIM_Notify, MUIA_Window_InputEvent, "control f",
	    (IPTR) obj, (IPTR) 3,
	    MUIM_Set, MUIA_ShowMe, (IPTR) TRUE);

    return TRUE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_3
(
    SearchBar, NULL, MUIC_Group, NULL,
    OM_NEW, struct opSet*, 
    MUIM_SearchBar_Find, struct MUIP_SearchBar_Find *,
    MUIM_Setup, struct MUIP_Setup *
)
