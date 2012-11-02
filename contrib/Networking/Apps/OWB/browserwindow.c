/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>
#include <utility/tagitem.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>
#include <proto/utility.h>
#include <aros/debug.h>
#include <libraries/codesets.h>
#include <proto/codesets.h>
#include <zune/customclasses.h>

#include <WebViewZune.h>

#include "locale.h"
#include "browsertab.h"
#include "downloadmanager.h"
#include "searchbar.h"
#include "preferencesmanager.h"
#include "browserpreferences.h"
#include "bookmarkmanager.h"
#include "tabs.h"
#include "tabbedview.h"

#include <curl/curl.h>

#include "browsertab.h"
#include "searchbar.h"
#include "tabs.h"
#include "tabbedview.h"
#include "browserapp.h"

#include "browserwindow.h"
#include "browserwindow_private.h"

struct MUI_CustomClass *BrowserWindowClass;

/* Evil global variables */
struct Hook openNewHook, alertHook, confirmHook, promptHook, policyHook, credentialsHook, closeHook;

static IPTR OpenNewFunc(struct Hook *hook, Object *sender, void *data)
{
    Object *newWebView = NULL;
    struct WindowSpecification *spec = *(struct WindowSpecification**) data;
    if(spec == NULL)
    {
	// data is null - open a new browser tab
	newWebView = (Object*) DoMethod(_win(sender), MUIM_BrowserWindow_OpenNewTab, MUIV_BrowserWindow_BackgroundTab);
    }
    else
    {
	// window specification is present
	newWebView = (Object*) DoMethod(_app(sender), MUIM_BrowserApp_OpenNewWindow, spec);
    }
    return (IPTR) newWebView;
}

static void GoFunc(struct Hook *hook, Object *urlString, void *data)
{
    UBYTE *buf = NULL;
    get(urlString, MUIA_String_Contents, &buf);
    set(urlString, MUIA_String_Acknowledge, buf);
}

static void WebSearchFunc(struct Hook *hook, Object *searchString, void *data)
{
    UBYTE *buf = NULL;
    get(searchString, MUIA_String_Contents, &buf);
    set(searchString, MUIA_String_Acknowledge, buf);    
}

static void WebSearchAcknowledgeFunc(struct Hook *hook, Object *searchString, char **search)
{
    struct codeset *utfCodeset = CodesetsFindA("UTF-8", NULL);
    if(!utfCodeset)
	return;
    
    char *searchConverted = CodesetsConvertStr(CSA_Source, (IPTR) *search, CSA_DestCodeset, (IPTR) utfCodeset, TAG_END);
    if(searchConverted)
    {
        char *escaped = curl_escape(searchConverted, 0);
        if(escaped)
        {
            char *template = "http://google.com/search?q=%s&ie=UTF-8&oe=UTF-8";
            int length = snprintf(NULL, 0, template, escaped) + 1;
            char *url = AllocVec(length, MEMF_ANY);
            if(url)
            {
                snprintf(url, length, template, escaped);
                DoMethod(_win(searchString), MUIM_BrowserWindow_OpenURLInActiveTab, url);
                FreeVec(url);
            }
            curl_free(escaped);
        }
        CodesetsFreeA(searchConverted, NULL);
    }
}

static IPTR AlertFunc(struct Hook *hook, Object *webView, STRPTR *message)
{
    return MUI_Request(_app(webView), _win(webView), 0, _(MSG_JavaScript_Alert), _(MSG_JavaScript_AlertOK), "%s", (char*) *message);
}

static IPTR ConfirmFunc(struct Hook *hook, Object *webView, STRPTR *message)
{
    return !MUI_Request(_app(webView), _win(webView), 0, _(MSG_JavaScript_Confirm), _(MSG_JavaScript_ConfirmOptions), "%s", (char*) *message);
}

static IPTR PromptFunc(struct Hook *hook, Object *webView, STRPTR **args)
{
    Object *req_wnd, *bt_cancel, *bt_ok, *str;
    STRPTR answer = NULL;
    
    req_wnd = (Object*) WindowObject,
        MUIA_Window_Title, (IPTR) _(MSG_JavaScript_Prompt),
        MUIA_Window_RefWindow, (IPTR) _win(webView),
        MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
        MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_SizeGadget, FALSE,
        WindowContents, (IPTR)(VGroup,
            MUIA_Background, MUII_RequesterBack,
            Child, (IPTR)(HGroup,
                Child, (IPTR)(TextObject,
                    TextFrame,
                    MUIA_InnerBottom, 8,
                    MUIA_InnerLeft, 8,
                    MUIA_InnerRight, 8,
                    MUIA_InnerTop, 8,
                    MUIA_Background, MUII_TextBack,
                    MUIA_Text_SetMax, TRUE,
                    MUIA_Text_Contents, (IPTR) (*args)[0],
                    End),
                End),
            Child, (IPTR)(VSpace(2)),
            Child, (IPTR)(str = (Object*) StringObject,
                MUIA_Frame, MUIV_Frame_String,
                MUIA_String_Contents, (IPTR) (*args)[1],
                End),
            Child, (IPTR)(VSpace(2)),
            Child, (IPTR)(HGroup, 
                Child, (IPTR)(bt_cancel = (Object*) SimpleButton(_(MSG_JavaScript_PromptCancel))),
                Child, (IPTR)(HSpace(0)),
                Child, (IPTR)(bt_ok = (Object*) SimpleButton(_(MSG_JavaScript_PromptOK))),
                End),
            End),
        End;
    
    if (!req_wnd)
        return (IPTR) NULL;

    DoMethod(_app(webView), OM_ADDMEMBER, (IPTR)req_wnd);

    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
    	(IPTR)_app(webView), 2, MUIM_Application_ReturnID, 1);

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE,
    	(IPTR)_app(webView), 2, MUIM_Application_ReturnID, 2);

    set(req_wnd, MUIA_Window_ActiveObject, bt_ok);
    set(req_wnd, MUIA_Window_Open, TRUE);

    LONG result = -1;

    if (XGET(req_wnd, MUIA_Window_Open))
    {
        ULONG sigs = 0;
        
        while (result == -1)
        {
            ULONG ret = DoMethod(_app(webView), MUIM_Application_NewInput, (IPTR)&sigs);
            if(ret == 1 || ret == 2)
            {
                result = ret;
                break;
            }
            
            if (sigs)
                sigs = Wait(sigs);
        }
    }

    if (result == 2)
    {
        STRPTR value = (STRPTR) XGET(str, MUIA_String_Contents);
        if(value)
            answer = StrDup(value);
    }

    set(req_wnd, MUIA_Window_Open, FALSE);
    DoMethod(_app(webView), OM_REMMEMBER, (IPTR)req_wnd);
    MUI_DisposeObject(req_wnd);
    
    return (IPTR) answer;
}

static IPTR PolicyFunc(struct Hook *hook, Object *webView, CONST_STRPTR **args)
{
    switch(MUI_Request(_app(webView), _win(webView), 0, _(MSG_RequestPolicy_Title), _(MSG_RequestPolicy_Options), _(MSG_RequestPolicy_Message), (IPTR) (*args)[0], (IPTR) (*args)[1]))
    {
    default:
    case 0:
	return MUIV_WebView_Policy_Ignore;
    case 1:
	return MUIV_WebView_Policy_Download;
    case 2:
	return MUIV_WebView_Policy_Use;
    }
}

static IPTR CredentialsFunc(struct Hook *hook, Object *webView, IPTR **args)
{
    Object *req_wnd, *bt_cancel, *bt_ok, *username_str, *password_str;
    CONST_STRPTR hostname = (CONST_STRPTR) (*args)[0];
    CONST_STRPTR realm = (CONST_STRPTR) (*args)[1];
    CONST_STRPTR *username = (CONST_STRPTR*) (*args)[2];
    CONST_STRPTR *password = (CONST_STRPTR*) (*args)[3];
    BOOL ret = FALSE;
    char msg[1024];
    
    snprintf(msg, sizeof(msg), _(MSG_RequestCredentials_Message), realm, hostname);
    
    req_wnd = (Object*) WindowObject,
        MUIA_Window_Title, (IPTR) _(MSG_RequestCredentials_Title),
        MUIA_Window_RefWindow, (IPTR) _win(webView),
        MUIA_Window_LeftEdge, MUIV_Window_LeftEdge_Centered,
        MUIA_Window_TopEdge, MUIV_Window_TopEdge_Centered,
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_SizeGadget, FALSE,
        WindowContents, (IPTR)(VGroup,
            MUIA_Background, MUII_RequesterBack,
            Child, (IPTR)(HGroup,
                Child, (IPTR)(TextObject,
                    TextFrame,
                    MUIA_InnerBottom, 8,
                    MUIA_InnerLeft, 8,
                    MUIA_InnerRight, 8,
                    MUIA_InnerTop, 8,
                    MUIA_Background, MUII_TextBack,
                    MUIA_Text_SetMax, TRUE,
                    MUIA_Text_Contents, (IPTR) msg,
                    End),
                End),
            Child, (IPTR)(VSpace(2)),
            Child, (IPTR)(ColGroup(2),
                Child, (IPTR)(Label1(_(MSG_RequestCredentials_Username))),
		Child, (IPTR)(username_str = (Object*) StringObject,
		    MUIA_Frame, MUIV_Frame_String,
		    MUIA_CycleChain, 1,
		    End),
                Child, (IPTR)(Label1(_(MSG_RequestCredentials_Password))),
		Child, (IPTR)(password_str = (Object*) StringObject,
		    MUIA_Frame, MUIV_Frame_String,
		    MUIA_String_Secret, TRUE,
		    MUIA_CycleChain, 1,
		    End),
		End),
            Child, (IPTR)(VSpace(2)),
            Child, (IPTR)(HGroup, 
                Child, (IPTR)(bt_cancel = (Object*) SimpleButton(_(MSG_RequestCredentials_Cancel))),
                Child, (IPTR)(HSpace(0)),
                Child, (IPTR)(bt_ok = (Object*) SimpleButton(_(MSG_RequestCredentials_OK))),
                End),
            End),
        End;
    
    if (!req_wnd)
        return (IPTR) NULL;

    DoMethod(_app(webView), OM_ADDMEMBER, (IPTR)req_wnd);

    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
    	(IPTR)_app(webView), 2, MUIM_Application_ReturnID, 1);

    DoMethod(bt_ok, MUIM_Notify, MUIA_Pressed, FALSE,
    	(IPTR)_app(webView), 2, MUIM_Application_ReturnID, 2);

    set(req_wnd, MUIA_Window_ActiveObject, bt_ok);
    set(req_wnd, MUIA_Window_Open, TRUE);

    LONG result = -1;

    if (XGET(req_wnd, MUIA_Window_Open))
    {
        ULONG sigs = 0;
        
        while (result == -1)
        {
            ULONG ret = DoMethod(_app(webView), MUIM_Application_NewInput, (IPTR)&sigs);
            if(ret == 1 || ret == 2)
            {
                result = ret;
                break;
            }
            
            if (sigs)
                sigs = Wait(sigs);
        }
    }

    if (result == 2)
    {
        *username = strdup((const char*) XGET(username_str, MUIA_String_Contents));
        *password = strdup((const char*) XGET(password_str, MUIA_String_Contents));
        ret = TRUE;
    }

    set(req_wnd, MUIA_Window_Open, FALSE);
    DoMethod(_app(webView), OM_REMMEMBER, (IPTR)req_wnd);
    MUI_DisposeObject(req_wnd);
    
    return (IPTR) ret;
}

static IPTR CloseFunc(struct Hook *hook, Object *webView, APTR *arg)
{
    return DoMethod(_app(webView), MUIM_Application_PushMethod, _app(webView), 3, MUIM_BrowserApp_CloseWindow, _win(webView));
}

/*** Methods ****************************************************************/
IPTR BrowserWindow__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct BrowserWindow_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
 
    Object *webView, *tab, *tabs, *tabbed;
    Object *toolTips, *progressBar, *urlString;
    Object *bt_back, *bt_forward, *bt_search, *bt_go, *bt_download, *bt_preferences, *bt_find, *bt_reload, *bt_stop, *bt_bookmarks;
    Object *searchCriteria, *searchBar;
    Object *bt_close, *menuclosetab, *menunewtab, *menubookmarktab, *tabContextMenu;
    BOOL showStatusBar = TRUE;
    BOOL showToolBar = TRUE;
    BOOL showLocationBar = TRUE;
    BOOL showScrollbars = TRUE;
    Object **webViewPointer = NULL;
    
    openNewHook.h_Entry = HookEntry;
    openNewHook.h_SubEntry = (HOOKFUNC) OpenNewFunc;
    alertHook.h_Entry = HookEntry;
    alertHook.h_SubEntry = (HOOKFUNC) AlertFunc;
    confirmHook.h_Entry = HookEntry;
    confirmHook.h_SubEntry = (HOOKFUNC) ConfirmFunc;
    promptHook.h_Entry = HookEntry;
    promptHook.h_SubEntry = (HOOKFUNC) PromptFunc;
    policyHook.h_Entry = HookEntry;
    policyHook.h_SubEntry = (HOOKFUNC) PolicyFunc;
    credentialsHook.h_Entry = HookEntry;
    credentialsHook.h_SubEntry = (HOOKFUNC) CredentialsFunc;
    closeHook.h_Entry = HookEntry;
    closeHook.h_SubEntry = (HOOKFUNC) CloseFunc;
    
    /* Parse initial attributes --------------------------------------------*/
    while ((tag = NextTagItem(&tstate)) != NULL)
    {
        switch (tag->ti_Tag)
        {
	    case MUIA_BrowserWindow_ShowStatusBar:
		showStatusBar = (BOOL) tag->ti_Data;
		break;
	    case MUIA_BrowserWindow_ShowToolBar:
		showToolBar = (BOOL) tag->ti_Data;
		break;
	    case MUIA_BrowserWindow_ShowLocationBar:
		showLocationBar = (BOOL) tag->ti_Data;
		break;
	    case MUIA_BrowserWindow_ShowScrollbars:
		showScrollbars = (BOOL) tag->ti_Data;
		break;
	    case MUIA_BrowserWindow_WebViewPointer:
		webViewPointer = (Object**) tag->ti_Data;
            default:
                continue; /* Don't supress non-processed tags */
        }
        
        tag->ti_Tag = TAG_IGNORE;
    }

    (void)showScrollbars; /* unused */
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
	MUIA_Window_Title, (IPTR)_(MSG_OWB),
	WindowContents, (IPTR)(VGroup,
	    Child, (IPTR)(HGroup,
		MUIA_ShowMe, showToolBar || showLocationBar,
	        Child, (IPTR)(HGroup,
		    MUIA_ShowMe, showToolBar,
		    MUIA_HorizWeight, 1,
		    MUIA_Group_Spacing, 2,
		    Child, (IPTR)(bt_back = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/back.png",
			    End),
			End),
		    Child, (IPTR)(bt_forward = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/forward.png",
			    End),
			End),
		    Child, (IPTR)(bt_stop = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/stop.png",
			    End),
			End),
		    Child, (IPTR)(bt_reload = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/reload.png",
			    End),
			End),
		    End),
		Child, (IPTR)(HGroup,
		    MUIA_ShowMe, showLocationBar,
		    MUIA_HorizWeight, 99,
		    MUIA_Group_Spacing, 2,
		    Child, (IPTR)(urlString = StringObject,
			MUIA_String_Contents, (IPTR)"",
			MUIA_CycleChain, 1,
			MUIA_Frame, MUIV_Frame_String,
			MUIA_String_MaxLen, 1024,
			End),
		    Child, (IPTR)(bt_go = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/go.png",
			    End),
			End),
		    End),
		Child, (IPTR)(HGroup,
		    MUIA_ShowMe, showToolBar,
		    MUIA_HorizWeight, 25,
		    MUIA_Group_Spacing, 2,
		    Child, (IPTR)(searchCriteria = StringObject,
			MUIA_CycleChain, 1,
			MUIA_String_MaxLen, 1024,
			MUIA_Frame, MUIV_Frame_String,
			End),
		    Child, (IPTR)(bt_search = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/search.png",
			    End),
			End),
		    End),
		End),
	    Child, (IPTR)(VGroup,
		MUIA_Group_Spacing, 0,
		Child, (IPTR)(tabs = NewObject(Tabs_CLASS->mcc_Class, NULL,
		    MUIA_ShowMe, showToolBar,
		    MUIA_Group_Spacing, 4,
		    MUIA_ContextMenu, (IPTR)(tabContextMenu = MenustripObject,
			MUIA_Family_Child, (IPTR)(MenuObject,
			    MUIA_Menu_Title, (IPTR)_(MSG_Tabs),
			    MUIA_Family_Child, (IPTR)(menuclosetab = MenuitemObject,
				MUIA_Menuitem_Title, (IPTR)_(MSG_Tabs_CloseActive),
				End),
			    MUIA_Family_Child, (IPTR)(menunewtab = MenuitemObject,
				MUIA_Menuitem_Title, (IPTR)_(MSG_Tabs_OpenNew),
				End),
			    MUIA_Family_Child, (IPTR)(menubookmarktab = MenuitemObject,
				MUIA_Menuitem_Title, (IPTR)_(MSG_Tabs_Bookmark),
				End),
			    End),
			End),
		    MUIA_Tabs_Location, MUIV_Tabs_Top,
		    MUIA_Tabs_CloseButton, (IPTR)(bt_close = HGroup,
			MUIA_Frame, MUIV_Frame_ImageButton,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/close.png",
			    End),
			End),
		    Child, (IPTR)(tab = NewObject(BrowserTab_CLASS->mcc_Class, NULL, TAG_DONE)),
		    TAG_END)),
		Child, (IPTR)(tabbed = NewObject(TabbedView_CLASS->mcc_Class, NULL,
		    MUIA_Group_Horiz, FALSE,
		    MUIA_VertWeight, 99,
		    MUIA_InnerTop, 4,
		    MUIA_InnerLeft, 5,
		    MUIA_InnerRight, 5,
		    MUIA_InnerBottom, 5,
		    Child, (IPTR)(webView = NewObject(WebView_CLASS->mcc_Class, NULL, 
			MUIA_WebView_CreateNewHook, (IPTR)&openNewHook,
			MUIA_WebView_AlertHook, (IPTR)&alertHook,
			MUIA_WebView_ConfirmHook, (IPTR)&confirmHook,
			MUIA_WebView_PromptHook, (IPTR)&promptHook,
			MUIA_WebView_PolicyHook, (IPTR)&policyHook,
			MUIA_WebView_CredentialsHook, (IPTR)&credentialsHook,
			MUIA_WebView_CloseHook, (IPTR)&closeHook,
			TAG_END)),
		    TAG_END)),
		End),
	    Child, (IPTR)(searchBar = NewObject(SearchBar_CLASS->mcc_Class, NULL, 
		TAG_END)),
	    Child, (IPTR)(HGroup,
		MUIA_ShowMe, showToolBar || showStatusBar,
		Child, (IPTR)(HGroup,
	            MUIA_ShowMe, showToolBar,
		    Child, (IPTR)(bt_download = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/download.png",
			    End),
			End),
		    Child, (IPTR)(bt_preferences = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/preferences.png",
			    End),
			End),
		    Child, (IPTR)(bt_find = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/find.png",
			    End),
			End),
		    Child, (IPTR)(bt_bookmarks = HGroup,
			MUIA_Frame, MUIV_Frame_None,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			Child, (IPTR)(ImageObject,
			    MUIA_Image_Spec, (IPTR)"3:PROGDIR:resources/bookmarks.png",
			    End),
			End),
		    End),
		Child, (IPTR)(HGroup,
		    MUIA_ShowMe, showStatusBar,
		    Child, (IPTR)(toolTips = TextObject,
			MUIA_Frame, MUIV_Frame_Text,
			MUIA_HorizWeight, 90,
			MUIA_Text_SetMin, FALSE,
			End),
		    Child, (IPTR)(progressBar = GaugeObject,
			MUIA_HorizWeight, 10,
			MUIA_Gauge_Horiz, TRUE,
			End),
		    End),
		End),
	   End),
        TAG_MORE, (IPTR) message->ops_AttrList	
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    if(webViewPointer)
	*webViewPointer = webView;
    
    data = INST_DATA(CLASS, self);
    data->tabs = tabs;
    data->tabbed = tabbed;
    data->urlString = urlString;
    data->tabContextMenu = tabContextMenu;
    data->progressBar = progressBar;
    data->searchBar = searchBar;
    data->toolTips = toolTips;
    data->goHook.h_Entry = HookEntry;
    data->goHook.h_SubEntry = (HOOKFUNC) GoFunc;
    data->webSearchHook.h_Entry = HookEntry;
    data->webSearchHook.h_SubEntry = (HOOKFUNC) WebSearchFunc;
    data->webSearchAcknowledgeHook.h_Entry = HookEntry;
    data->webSearchAcknowledgeHook.h_SubEntry = (HOOKFUNC) WebSearchAcknowledgeFunc;
    
    /* Click Close gadget or hit Escape to quit */
    DoMethod(self, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	    (IPTR) MUIV_Notify_Application, 2,
	    MUIM_BrowserApp_CloseWindow, self);

    /* Close active tab */
    DoMethod(menuclosetab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, 1,
	    MUIM_BrowserWindow_CloseActiveTab);

    DoMethod(self, MUIM_Notify, MUIA_Window_InputEvent, "control w",
	    (IPTR) self, 1,
	    MUIM_BrowserWindow_CloseActiveTab);

    /* Open new tab */
    DoMethod(menunewtab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, 2,
	    MUIM_BrowserWindow_OpenNewTab, MUIV_BrowserWindow_ForegroundTab);

    DoMethod(self, MUIM_Notify, MUIA_Window_InputEvent, "control t",
	    (IPTR) self, 2,
	    MUIM_BrowserWindow_OpenNewTab, MUIV_BrowserWindow_ForegroundTab);

    /* Bookmark tab */
    DoMethod(menubookmarktab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, 1,
	    MUIM_BrowserWindow_Bookmark);

    /* Switch active tab */
    DoMethod(tabs, MUIM_Notify, MUIA_Tabs_ActiveTab, MUIV_EveryTime,
	    (IPTR) tabbed, (IPTR) 3,
	    MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue);

    /* Close active tab */
    DoMethod(bt_close, MUIM_Notify, MUIA_Pressed, FALSE,
	    (IPTR) self, (IPTR) 3,
	    MUIM_BrowserWindow_CloseActiveTab);

    /* Go */
    DoMethod(bt_go, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) urlString, 3,
	     MUIM_CallHook, &data->goHook, NULL);

    /* Go acknowledged */
    DoMethod(urlString, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
	     (IPTR) self, 2,
	     MUIM_BrowserWindow_OpenURLInActiveTab, MUIV_TriggerValue);

    /* Web Search */
    DoMethod(bt_search, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) searchCriteria, 3,
	     MUIM_CallHook, &data->webSearchHook, NULL);

    /* Web Search acknowledged */
    DoMethod(searchCriteria, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
	     (IPTR) searchCriteria, 3,
	     MUIM_CallHook, &data->webSearchAcknowledgeHook, MUIV_TriggerValue);

    /* Download manager window */
    DoMethod(bt_download, MUIM_Notify, MUIA_Selected, FALSE,
	     (IPTR) MUIV_Notify_Application, 3,
	     MUIM_Set, MUIA_BrowserApp_DownloadManagerOpened, TRUE);

    /* Text Search window */
    DoMethod(bt_find, MUIM_Notify, MUIA_Selected, FALSE,
	     (IPTR) searchBar, 3,
	     MUIM_Set, MUIA_ShowMe, TRUE); 

    /* Preferences */
    DoMethod(bt_preferences, MUIM_Notify, MUIA_Selected, FALSE,
	     (IPTR) MUIV_Notify_Application, 3,
	     MUIM_Set, MUIA_BrowserApp_PreferencesOpened, TRUE);

    /* Bookmark manager */
    DoMethod(bt_bookmarks, MUIM_Notify, MUIA_Selected, FALSE,
	     (IPTR) MUIV_Notify_Application, 3,
	     MUIM_Set, MUIA_BrowserApp_BookmarkManagerOpened, TRUE);

    /* Connect initial webView title with the tab title */
    DoMethod(webView, MUIM_Notify, MUIA_WebView_Title, MUIV_EveryTime,
	     (IPTR) tab, 3,
	     MUIM_Set, MUIA_BrowserTab_Title, MUIV_TriggerValue);

    /* Go back */
    DoMethod(bt_back, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) tabbed, 1,
	     MUIM_WebView_GoBack);

    /* Go forward */
    DoMethod(bt_forward, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) tabbed, 1,
	     MUIM_WebView_GoForward);

    /* Stop loading */
    DoMethod(bt_stop, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) tabbed, 1,
	     MUIM_WebView_StopLoading);

    /* Reload page */
    DoMethod(bt_reload, MUIM_Notify, MUIA_Pressed, FALSE,
	     (IPTR) tabbed, 1,
	     MUIM_WebView_Reload);

    /* Set tooltips */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_ToolTip, MUIV_EveryTime,
	     (IPTR) self, 3,
	     MUIM_Set, MUIA_BrowserWindow_Status, MUIV_TriggerValue);

    /* Display state info */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_EveryTime,
	     (IPTR) self, 3,
	     MUIM_Set, MUIA_BrowserWindow_State, MUIV_TriggerValue);

    /* Disable stop button if ready */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_WebView_State_Ready,
	     (IPTR) bt_stop, 3,
	     MUIM_Set, MUIA_Disabled, TRUE);

    /* Disable stop button if error */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_WebView_State_Error,
	     (IPTR) bt_stop, 3,
	     MUIM_Set, MUIA_Disabled, TRUE);

    /* Enable stop button if connecting */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_WebView_State_Connecting,
	     (IPTR) bt_stop, 3,
	     MUIM_Set, MUIA_Disabled, FALSE);

    /* Enable stop button if loading */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_WebView_State_Loading,
	     (IPTR) bt_stop, 3,
	     MUIM_Set, MUIA_Disabled, FALSE);

    /* Set progress bar value to 0 when changing states */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_EveryTime,
	     (IPTR) progressBar, 3,
	     MUIM_Set, MUIA_Gauge_Current, 0);

    /* Update progress bar */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_EstimatedProgress, MUIV_EveryTime,
	     (IPTR) self, 3,
	     MUIM_Set, MUIA_BrowserWindow_LoadingProgress, MUIV_TriggerValue);

    /* Update URL */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_URL, MUIV_EveryTime,
	     (IPTR) urlString, 3,
	     MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);
    
    /* Update title */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_Title, MUIV_EveryTime,
	     (IPTR) self, 3,
	     MUIM_Set, MUIA_Window_Title, MUIV_TriggerValue);

    /* Update back button state */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_CanGoBack, MUIV_EveryTime,
	(IPTR) bt_back, 3,
	MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

    /* Update forward button state */
    DoMethod(tabbed, MUIM_Notify, MUIA_WebView_CanGoForward, MUIV_EveryTime,
	(IPTR) bt_forward, 3,
	MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

    /* Open new window */
    DoMethod(self, MUIM_Notify, MUIA_Window_InputEvent, "control n",
	    (IPTR) MUIV_Notify_Application, 2,
	    MUIM_BrowserApp_OpenNewWindow, NULL);
    
    /* Back and forward buttons are disabled by default */
    set(bt_back, MUIA_Disabled, TRUE);
    set(bt_forward, MUIA_Disabled, TRUE);
    set(bt_stop, MUIA_Disabled, TRUE);

    /* Display initial state information */
    set(self, MUIA_BrowserWindow_State, MUIV_WebView_State_Ready);

    return (IPTR) self;
}

IPTR BrowserWindow__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct BrowserWindow_DATA *data = INST_DATA(CLASS, self);
    IPTR ret;
    ret = DoSuperMethodA(CLASS, self, message);
    MUI_DisposeObject(data->tabContextMenu);

    return ret;
}

IPTR BrowserWindow__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct BrowserWindow_DATA *data = INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;
    
    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_BrowserWindow_LoadingProgress:
    	    {
		LONG value = tag->ti_Data;
		
		/* Display progress information only when loading */
		if(XGET(data->tabbed, MUIA_WebView_State) != MUIV_WebView_State_Loading)
		    value = 0;

		set(data->progressBar, MUIA_Gauge_Current, value);
    		break;
    	    }
    	    case MUIA_BrowserWindow_Status:
    	    {
		STRPTR value = (STRPTR) tag->ti_Data;
		/* Display state information only if there's no tooltip text to show */
		if(!value || value[0] == '\0')
		{
		    switch(XGET(data->tabbed, MUIA_WebView_State))
		    {
			case MUIV_WebView_State_Ready:
			    value = (STRPTR) _(MSG_Ready);
			    break;
			case MUIV_WebView_State_Connecting:
			    value = (STRPTR) _(MSG_Connecting);
			    break;
			case MUIV_WebView_State_Loading:
			    value = (STRPTR) _(MSG_Loading);
			    break;
			case MUIV_WebView_State_Error:
			    value = (STRPTR) _(MSG_Error);
			    break;
		    }
		}
		set(data->toolTips, MUIA_Text_Contents, value);
		break;
    	    }
    	    case MUIA_BrowserWindow_State:
    	    {
    		STRPTR status = (STRPTR) XGET(data->tabbed, MUIA_WebView_ToolTip);
    		/* Display state information only if there's no tooltip text to show */
    		if(!status || status[0] == '\0')
    		{
    		    switch(tag->ti_Data)
		    {
			case MUIV_WebView_State_Ready:
			    status = (STRPTR) _(MSG_Ready);
			    break;
			case MUIV_WebView_State_Connecting:
			    status = (STRPTR) _(MSG_Connecting);
			    break;
			case MUIV_WebView_State_Loading:
			    status = (STRPTR) _(MSG_Loading);
			    break;
			case MUIV_WebView_State_Error:
			    status = (STRPTR) _(MSG_Error);
			    break;
		    }
    		}
    		set(data->toolTips, MUIA_Text_Contents, status);
    		break;
    	    }
    	    default:
    		continue;
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR BrowserWindow__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct BrowserWindow_DATA *data = INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
        case MUIA_BrowserWindow_WebView:
            *(Object**)msg->opg_Storage = data->tabbed;
            break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

IPTR BrowserWindow__MUIM_BrowserWindow_OpenURLInActiveTab(Class *cl, Object *obj, struct MUIP_BrowserWindow_OpenURLInActiveTab *msg)
{
    struct BrowserWindow_DATA *data = INST_DATA(cl, obj);
    
    set(data->urlString, MUIA_String_Contents, msg->url);
    
    char *buf = msg->url;
    const char *httpPrefix = "http://";
    
    set(data->tabbed, MUIA_WebView_Active, TRUE);
    
    if(!strstr(buf, "://"))
    {
	/* No protocol is specified, assume it's http */
	char *url = AllocVec(strlen(buf) + strlen(httpPrefix) + 1, MEMF_ANY);
	if(url)
	{
	    strcpy(url, httpPrefix);
	    strcat(url, buf);
	    DoMethod(data->tabbed, MUIM_WebView_LoadURL, url);
	    FreeVec(url);
	}
    }
    else
        DoMethod(data->tabbed, MUIM_WebView_LoadURL, buf);

    return 0;
}

IPTR BrowserWindow__MUIM_BrowserWindow_OpenNewTab(Class *cl, Object *obj, struct MUIP_BrowserWindow_OpenNewTab *msg)
{
    struct BrowserWindow_DATA *data = INST_DATA(cl, obj);
    
    Object *newTab = NULL, *newWebView = NULL;

    DoMethod(data->tabs, MUIM_Group_InitChange);
    DoMethod(data->tabs, OM_ADDMEMBER, newTab = NewObject(BrowserTab_CLASS->mcc_Class, NULL, TAG_END));
    DoMethod(data->tabs, MUIM_Group_ExitChange);
    
    DoMethod(data->tabbed, MUIM_Group_InitChange);
    DoMethod(data->tabbed, OM_ADDMEMBER, newWebView = NewObject(
	    WebView_CLASS->mcc_Class, NULL, 
	    MUIA_WebView_CreateNewHook, &openNewHook, 
	    MUIA_WebView_AlertHook, &alertHook,
	    MUIA_WebView_ConfirmHook, &confirmHook,
	    MUIA_WebView_PromptHook, &promptHook,
	    MUIA_WebView_PolicyHook, &policyHook,
	    MUIA_WebView_CredentialsHook, &credentialsHook,
	    MUIA_WebView_CloseHook, &closeHook,
	    TAG_END));
    DoMethod(data->tabbed, MUIM_Group_ExitChange);

    DoMethod(newWebView, MUIM_Notify, MUIA_WebView_Title, MUIV_EveryTime,
	     (IPTR) newTab, 3,
	     MUIM_Set, MUIA_BrowserTab_Title, MUIV_TriggerValue);
    
    if(msg->mode == MUIV_BrowserWindow_ForegroundTab)
    {
	struct List *children = (struct List*) XGET(data->tabbed, MUIA_Group_ChildList);
	ULONG childNum;
	ListLength(children,childNum);
	set(data->tabs, MUIA_Tabs_ActiveTab, childNum - 1);
	DoMethod(_app(obj), MUIM_Application_PushMethod, obj, 3, MUIM_Set, MUIA_Window_ActiveObject, data->urlString);
    }
    
    return (IPTR) newWebView;
}

IPTR BrowserWindow__MUIM_BrowserWindow_CloseActiveTab(Class *cl, Object *obj, Msg msg)
{
    struct BrowserWindow_DATA *data = INST_DATA(cl, obj);
        
    Object *deletedWebView = (Object*) XGET(data->tabbed, MUIA_TabbedView_ActiveObject);

    DoMethod(deletedWebView, MUIM_KillNotify, MUIA_WebView_Title);    

    DoMethod(data->tabbed, MUIM_Group_InitChange);
    DoMethod(data->tabbed, OM_REMMEMBER, (IPTR) deletedWebView);
    DoMethod(data->tabbed, MUIM_Group_ExitChange);

    Object *deletedTab = (Object*) XGET(data->tabs, MUIA_Tabs_ActiveTabObject);
    DoMethod(data->tabs, MUIM_Group_InitChange);
    LONG removed = DoMethod(data->tabs, OM_REMMEMBER, (IPTR) deletedTab);
    DoMethod(data->tabs, MUIM_Group_ExitChange);

    if(!removed)
    {
	/* Whoops, it's the last tab, get ready for exit */
        DoMethod(data->tabbed, MUIM_Group_InitChange);
        DoMethod(data->tabbed, OM_ADDMEMBER, (IPTR) deletedWebView);
        DoMethod(data->tabbed, MUIM_Group_ExitChange);
        
	DoMethod(_app(obj), MUIM_Application_PushMethod, _app(obj), 2, MUIM_BrowserApp_CloseWindow, obj);
	return 0;
    }

    /* Destroy the closed tab */
    MUI_DisposeObject(deletedTab);
    MUI_DisposeObject(deletedWebView);
    
    return 0;
}

IPTR BrowserWindow__MUIM_BrowserWindow_Bookmark(struct IClass *cl, Object *obj, Msg msg)
{
    struct BrowserWindow_DATA *data = (struct BrowserWindow_DATA *) INST_DATA(cl, obj);
    
    Object *webView = (Object*) XGET(data->tabbed, MUIA_TabbedView_ActiveObject);
    Object *tab = (Object*) XGET(data->tabs, MUIA_Tabs_ActiveTabObject);
    STRPTR title = (STRPTR) XGET(tab, MUIA_BrowserTab_Title);
    STRPTR url = (STRPTR) XGET(webView, MUIA_WebView_URL);
    return DoMethod(_app(obj), MUIM_BrowserApp_Bookmark, title, url);
}

IPTR BrowserWindow__MUIM_BrowserWindow_Find(struct IClass *cl, Object *obj, Msg msg)
{
    struct BrowserWindow_DATA *data = (struct BrowserWindow_DATA *) INST_DATA(cl, obj);
    set(data->searchBar, MUIA_ShowMe, TRUE);
    return TRUE;
}

IPTR BrowserWindow__MUIM_BrowserWindow_FindNext(struct IClass *cl, Object *obj, Msg msg)
{
    struct BrowserWindow_DATA *data = (struct BrowserWindow_DATA *) INST_DATA(cl, obj);
    DoMethod(data->searchBar, MUIM_SearchBar_Find, (IPTR) TRUE);
    return TRUE;
}

IPTR BrowserWindow__MUIM_BrowserWindow_Zoom(struct IClass *cl, Object *obj, struct MUIP_BrowserWindow_Zoom *msg)
{
    struct BrowserWindow_DATA *data = (struct BrowserWindow_DATA *) INST_DATA(cl, obj);
    Object *webView = (Object*) XGET(data->tabbed, MUIA_TabbedView_ActiveObject);
    switch(msg->mode)
    {
	case MUIV_BrowserWindow_ZoomIn:
	    return DoMethod(webView, MUIM_WebView_Zoom, MUIV_WebView_ZoomIn);
	    break;
	case MUIV_BrowserWindow_ZoomOut:
	    return DoMethod(webView, MUIM_WebView_Zoom, MUIV_WebView_ZoomOut);
	    break;
	case MUIV_BrowserWindow_ZoomReset:
	    return DoMethod(webView, MUIM_WebView_Zoom, MUIV_WebView_ZoomReset);
	    break;
    }

    return FALSE;
}

IPTR BrowserWindow__MUIM_BrowserWindow_ShowSource(struct IClass *cl, Object *obj, Msg msg)
{
    struct BrowserWindow_DATA *data = (struct BrowserWindow_DATA *) INST_DATA(cl, obj);

    struct WindowSpecification spec = {
	MUIV_Window_LeftEdge_Centered,
	MUIV_Window_TopEdge_Centered,
	MUIV_Window_Width_Default,
	MUIV_Window_Height_Default,
	TRUE,
	TRUE,
	FALSE,
	TRUE,
	TRUE,
	FALSE
    };

    Object *webView = (Object*) XGET(data->tabbed, MUIA_TabbedView_ActiveObject);
    STRPTR url = (STRPTR) XGET(webView, MUIA_WebView_URL);
    STRPTR htmlData = (STRPTR) XGET(webView, MUIA_WebView_Data);
    Object* newWebView = (Object*) DoMethod(_app(obj), MUIM_BrowserApp_OpenNewWindow, &spec);
    set(newWebView, MUIA_WebView_SourceMode, TRUE);
    DoMethod(newWebView, MUIM_WebView_LoadHTMLString, url, htmlData, -1, "text/html", "UTF-8");
    FreeVec(htmlData);

    return TRUE;
}

/*** Setup ******************************************************************/
__ZUNE_CUSTOMCLASS_START(BrowserWindow)
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__OM_NEW, OM_NEW, struct opSet*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__OM_DISPOSE, OM_DISPOSE, Msg);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__OM_SET, OM_SET, struct opSet*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__OM_GET, OM_GET, struct opGet*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_OpenURLInActiveTab, MUIM_BrowserWindow_OpenURLInActiveTab, struct MUIP_BrowserWindow_OpenURLInActiveTab*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_OpenNewTab, MUIM_BrowserWindow_OpenNewTab, struct MUIP_BrowserWindow_OpenNewTab*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_CloseActiveTab, MUIM_BrowserWindow_CloseActiveTab, Msg);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_Bookmark, MUIM_BrowserWindow_Bookmark, Msg);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_Find, MUIM_BrowserWindow_Find, Msg);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_FindNext, MUIM_BrowserWindow_FindNext, Msg);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_Zoom, MUIM_BrowserWindow_Zoom, struct MUIP_BrowserWindow_Zoom*);
__ZUNE_CUSTOMCLASS_METHOD(BrowserWindow__MUIM_BrowserWindow_ShowSource, MUIM_BrowserWindow_ShowSource, Msg);
__ZUNE_CUSTOMCLASS_END(BrowserWindow, NULL, MUIC_Window, NULL)
