/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>
#include <aros/debug.h>
#include <libraries/codesets.h>
#include <proto/codesets.h>

#include <WebViewZune.h>

#include "locale.h"
#include "browsertab.h"
#include "downloadmanager.h"
#include "searchwindow.h"
#include "preferencesmanager.h"
#include "browserpreferences.h"
#include "bookmarkmanager.h"
#include "tabs.h"
#include "tabbedview.h"

#include <curl/curl.h>

/* Evil global variables */
Object *tabs, *tabbed;
struct Hook openTabHook, alertHook, confirmHook, promptHook, policyHook, credentialsHook;

static IPTR NewTabFunc(struct Hook *hook, Object *sender, void *data)
{
    Object *newTab = NULL;
    Object *newWebView = NULL;

    DoMethod(tabs, MUIM_Group_InitChange);
    DoMethod(tabs, OM_ADDMEMBER, newTab = NewObject(BrowserTab_CLASS->mcc_Class, NULL, TAG_END));
    DoMethod(tabs, MUIM_Group_ExitChange);
    
    DoMethod(tabbed, MUIM_Group_InitChange);
    DoMethod(tabbed, OM_ADDMEMBER, newWebView = NewObject(
	    WebView_CLASS->mcc_Class, NULL, 
	    MUIA_WebView_CreateNewHook, &openTabHook, 
	    MUIA_WebView_AlertHook, &alertHook,
	    MUIA_WebView_ConfirmHook, &confirmHook,
	    MUIA_WebView_PromptHook, &promptHook,
	    MUIA_WebView_PolicyHook, &policyHook,
	    MUIA_WebView_CredentialsHook, &credentialsHook,
	    TAG_END));
    DoMethod(tabbed, MUIM_Group_ExitChange);

    DoMethod(newWebView, MUIM_Notify, MUIA_WebView_Title, MUIV_EveryTime,
             (IPTR) newTab, 3,
             MUIM_Set, MUIA_BrowserTab_Title, MUIV_TriggerValue);

    return (IPTR) newWebView;
}

static void CloseTabFunc(struct Hook *hook, Object *obj, void *data)
{
    Object *deletedWebView = (Object*) XGET(tabbed, MUIA_TabbedView_ActiveObject);

    DoMethod(deletedWebView, MUIM_KillNotify, MUIA_WebView_Title);    

    DoMethod(tabbed, MUIM_Group_InitChange);
    DoMethod(tabbed, OM_REMMEMBER, (IPTR) deletedWebView);
    DoMethod(tabbed, MUIM_Group_ExitChange);

    Object *deletedTab = (Object*) XGET(tabs, MUIA_Tabs_ActiveTabObject);
    DoMethod(tabs, MUIM_Group_InitChange);
    LONG removed = DoMethod(tabs, OM_REMMEMBER, (IPTR) deletedTab);
    DoMethod(tabs, MUIM_Group_ExitChange);

    if(!removed)
    {
	/* Whoops, it's the last tab, get ready for exit */
        DoMethod(tabbed, MUIM_Group_InitChange);
        DoMethod(tabbed, OM_ADDMEMBER, (IPTR) deletedWebView);
        DoMethod(tabbed, MUIM_Group_ExitChange);
	DoMethod(_app(obj), MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
	return;
    }

    /* Destroy the closed tab */
    MUI_DisposeObject(deletedTab);
    MUI_DisposeObject(deletedWebView);
}

static IPTR BookmarkTabFunc(struct Hook *hook, Object *obj, Object **bookmarkManager)
{
    Object *webView = (Object*) XGET(tabbed, MUIA_TabbedView_ActiveObject);
    Object *tab = (Object*) XGET(tabs, MUIA_Tabs_ActiveTabObject);
    STRPTR title = XGET(tab, MUIA_BrowserTab_Title);
    STRPTR url = XGET(webView, MUIA_WebView_URL);
    return DoMethod(*bookmarkManager, MUIM_BookmarkManager_Bookmark, title, url);
}

static void GoFunc(struct Hook *hook, Object *urlString, void *data)
{
    UBYTE *buf = NULL;
    get(urlString, MUIA_String_Contents, &buf);
    set(urlString, MUIA_String_Acknowledge, buf);
}

static void GoAcknowledgeFunc(struct Hook *hook, Object *urlString, STRPTR *url)
{   
    char *buf = *url;
    const char *httpPrefix = "http://";
    if(!strstr(buf, "://"))
    {
	/* No protocol is specified, assume it's http */
	char *url = AllocVec(strlen(buf) + strlen(httpPrefix) + 1, MEMF_ANY);
	if(url)
	{
	    strcpy(url, httpPrefix);
	    strcat(url, buf);
	    DoMethod(tabbed, MUIM_WebView_LoadURL, url);
	    FreeVec(url);
	}
    }
    else
        DoMethod(tabbed, MUIM_WebView_LoadURL, buf);
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
    
    char *searchConverted = CodesetsConvertStr(CSA_Source, *search, CSA_DestCodeset, utfCodeset, TAG_END);
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
                DoMethod(tabbed, MUIM_WebView_LoadURL, url);
                FreeVec(url);
            }
            curl_free(escaped);
        }
        CodesetsFreeA(searchConverted, NULL);
    }
}

static void TextSearchFunc(struct Hook *hook, Object *searchWindow, struct OWB_SearchMsg **msg)
{
    DoMethod(tabbed, MUIM_WebView_SearchFor, (*msg)->criteria, (*msg)->forward, (*msg)->caseSensitive);
}

static void UpdateURLFunc(struct Hook *hook, Object *urlString, STRPTR *url)
{
    set(urlString, MUIA_String_Contents, *url);
}

static void UpdateStatusBarFunc(struct Hook *hook, Object *toolTips, STRPTR *tooltip)
{
    STRPTR value = *tooltip;
    
    /* Display state information only if there's no tooltip text to show */
    if(!value || value[0] == '\0')
    {
        switch(XGET(tabbed, MUIA_WebView_State))
        {
            case MUIV_WebView_State_Ready:
        	value = _(MSG_Ready);
        	break;
            case MUIV_WebView_State_Connecting:
        	value = _(MSG_Connecting);
        	break;
            case MUIV_WebView_State_Loading:
        	value = _(MSG_Loading);
        	break;
            case MUIV_WebView_State_Error:
        	value = _(MSG_Error);
        	break;
        }
    }
    set(toolTips, MUIA_Text_Contents, value);
}

static void UpdateProgressBarFunc(struct Hook *hook, Object *gauge, LONG *current)
{
    LONG value = *current;
    
    /* Display progress information only when loading */
    switch(XGET(tabbed, MUIA_WebView_State))
    {
        case MUIV_WebView_State_Loading:
            break;
        default:
            value = 0;
            break;
    }
    set(gauge, MUIA_Gauge_Current, value);
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
        WindowContents, VGroup,
            MUIA_Background, MUII_RequesterBack,
            Child, HGroup,
                Child, TextObject,
                    TextFrame,
                    MUIA_InnerBottom, 8,
                    MUIA_InnerLeft, 8,
                    MUIA_InnerRight, 8,
                    MUIA_InnerTop, 8,
                    MUIA_Background, MUII_TextBack,
                    MUIA_Text_SetMax, TRUE,
                    MUIA_Text_Contents, (IPTR) (*args)[0],
                    End,
                End,
            Child, (IPTR) VSpace(2),
            Child, (IPTR) (str = (Object*) StringObject,
                MUIA_Frame, MUIV_Frame_String,
                MUIA_String_Contents, (IPTR) (*args)[1],
                End),
            Child, (IPTR) VSpace(2),
            Child, (IPTR) HGroup, 
                Child, (IPTR) (bt_cancel = (Object*) SimpleButton(_(MSG_JavaScript_PromptCancel))),
                Child, (IPTR) HSpace(0),
                Child, (IPTR) (bt_ok = (Object*) SimpleButton(_(MSG_JavaScript_PromptOK))),
                End,
            End,
        End;
    
    if (!req_wnd)
        return NULL;

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
        STRPTR value = XGET(str, MUIA_String_Contents);
        if(value)
            answer = StrDup(value);
    }

    set(req_wnd, MUIA_Window_Open, FALSE);
    DoMethod(_app(webView), OM_REMMEMBER, (IPTR)req_wnd);
    MUI_DisposeObject(req_wnd);
    
    return answer;
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
    CONST_STRPTR hostname = (*args)[0];
    CONST_STRPTR realm = (*args)[1];
    CONST_STRPTR *username = (*args)[2];
    CONST_STRPTR *password = (*args)[3];
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
        WindowContents, VGroup,
            MUIA_Background, MUII_RequesterBack,
            Child, HGroup,
                Child, TextObject,
                    TextFrame,
                    MUIA_InnerBottom, 8,
                    MUIA_InnerLeft, 8,
                    MUIA_InnerRight, 8,
                    MUIA_InnerTop, 8,
                    MUIA_Background, MUII_TextBack,
                    MUIA_Text_SetMax, TRUE,
                    MUIA_Text_Contents, (IPTR) msg,
                    End,
                End,
            Child, (IPTR) VSpace(2),
            Child, (IPTR) ColGroup(2),
                Child, (IPTR) Label1(_(MSG_RequestCredentials_Username)),
		Child, (IPTR) (username_str = (Object*) StringObject,
		    MUIA_Frame, MUIV_Frame_String,
		    MUIA_CycleChain, 1,
		    End),
                Child, (IPTR) Label1(_(MSG_RequestCredentials_Password)),
		Child, (IPTR) (password_str = (Object*) StringObject,
		    MUIA_Frame, MUIV_Frame_String,
		    MUIA_String_Secret, TRUE,
		    MUIA_CycleChain, 1,
		    End),
		End,
            Child, (IPTR) VSpace(2),
            Child, (IPTR) HGroup, 
                Child, (IPTR) (bt_cancel = (Object*) SimpleButton(_(MSG_RequestCredentials_Cancel))),
                Child, (IPTR) HSpace(0),
                Child, (IPTR) (bt_ok = (Object*) SimpleButton(_(MSG_RequestCredentials_OK))),
                End,
            End,
        End;
    
    if (!req_wnd)
        return NULL;

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
        *username = strdup(XGET(username_str, MUIA_String_Contents));
        *password = strdup(XGET(password_str, MUIA_String_Contents));
        ret = TRUE;
    }

    set(req_wnd, MUIA_Window_Open, FALSE);
    DoMethod(_app(webView), OM_REMMEMBER, (IPTR)req_wnd);
    MUI_DisposeObject(req_wnd);
    
    return (IPTR) ret;
}

int main(void)
{
    Object *bt_close, *menuclosetab, *menunewtab, *menubookmarktab, *tabContextMenu;
    Object *searchCriteria, *searchWindow, *downloadManager, *preferencesManager, *bookmarkManager;
    Object *app, *preferences, *wnd;
    Object *webView, *tab;
    Object *toolTips, *progressBar, *urlString;
    Object *bt_back, *bt_forward, *bt_search, *bt_go, *bt_download, *bt_preferences, *bt_find, *bt_reload, *bt_stop, *bt_bookmarks;
    struct Hook goHook, goAcknowledgeHook, webSearchHook, webSearchAcknowledgeHook, closeTabHook, textSearchHook, updateURLHook, updateStatusBarHook;
    struct Hook bookmarkTabHook, updateProgressBarHook;
    IPTR argArray[] = { 0 };
    struct RDArgs *args = NULL;
    const char *url = NULL;

    if((args = ReadArgs("URL", argArray, NULL)) != NULL)
    {
	if(argArray[0])
	{
	    url = StrDup((const char*) argArray[0]);
	    if(!url)
	    {
		FreeArgs(args);
		return 1;
	    }
	}
	FreeArgs(args);
    }

    Locale_Initialize();
    
    SetVar("FONTCONFIG_PATH", "PROGDIR:fonts/config", -1, LV_VAR | GVF_LOCAL_ONLY);
    SetVar("DISABLE_NI_WARNING", "1", -1, LV_VAR | GVF_LOCAL_ONLY);
    
    closeTabHook.h_Entry = HookEntry;
    closeTabHook.h_SubEntry = (HOOKFUNC) CloseTabFunc;
    openTabHook.h_Entry = HookEntry;
    openTabHook.h_SubEntry = (HOOKFUNC) NewTabFunc;
    bookmarkTabHook.h_Entry = HookEntry;
    bookmarkTabHook.h_SubEntry = (HOOKFUNC) BookmarkTabFunc;
    goHook.h_Entry = HookEntry;
    goHook.h_SubEntry = (HOOKFUNC) GoFunc;
    goAcknowledgeHook.h_Entry = HookEntry;
    goAcknowledgeHook.h_SubEntry = (HOOKFUNC) GoAcknowledgeFunc;
    webSearchHook.h_Entry = HookEntry;
    webSearchHook.h_SubEntry = (HOOKFUNC) WebSearchFunc;
    webSearchAcknowledgeHook.h_Entry = HookEntry;
    webSearchAcknowledgeHook.h_SubEntry = (HOOKFUNC) WebSearchAcknowledgeFunc;
    textSearchHook.h_Entry = HookEntry;
    textSearchHook.h_SubEntry = (HOOKFUNC) TextSearchFunc;
    updateURLHook.h_Entry = HookEntry;
    updateURLHook.h_SubEntry = (HOOKFUNC) UpdateURLFunc;
    updateStatusBarHook.h_Entry = HookEntry;
    updateStatusBarHook.h_SubEntry = (HOOKFUNC) UpdateStatusBarFunc;
    updateProgressBarHook.h_Entry = HookEntry;
    updateProgressBarHook.h_SubEntry = (HOOKFUNC) UpdateProgressBarFunc;
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

    preferences = NewObject(BrowserPreferences_CLASS->mcc_Class, NULL, TAG_END);
  
    // GUI creation
    app = ApplicationObject,
        MUIA_Application_Title, "Origyn Web Browser",
        MUIA_Application_Version, "$VER: OWB 0.97 (21.06.2009)",
        MUIA_Application_Author, "Stanislaw Szymczyk",
        MUIA_Application_Copyright, "Copyright © 2009, The AROS Development Team. All rights reserved.",
        MUIA_Application_Description, "Port of Origyn Web Browser to AROS",
        MUIA_Application_Base, "OWB",
        SubWindow, wnd = WindowObject,
            MUIA_Window_Title, _(MSG_OWB),
            MUIA_Window_NoMenus, TRUE,
            MUIA_Window_Width, MUIV_Window_AltWidth_Screen(100),
            MUIA_Window_Height, MUIV_Window_AltHeight_Screen(100),
            MUIA_Window_AltWidth, MUIV_Window_AltWidth_Screen(75),
            MUIA_Window_AltHeight, MUIV_Window_AltHeight_Screen(75),
            WindowContents, VGroup,
                Child, HGroup,
                    Child, HGroup,
                        MUIA_HorizWeight, 1,
                        MUIA_Group_Spacing, 2,
                        Child, bt_back = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/back.png",
            	                End,
            	            End,
                        Child, bt_forward = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/forward.png",
            	                End,
            	            End,
                        Child, bt_stop = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/stop.png",
            	                End,
            	            End,
                        Child, bt_reload = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/reload.png",
            	                End,
            	            End,
                        End,
                    Child, HGroup,
                        MUIA_HorizWeight, 99,
                        MUIA_Group_Spacing, 2,
                        Child, (IPTR) (urlString = StringObject,
                            MUIA_String_Contents, (IPTR)"",
                            MUIA_CycleChain, 1,
                            MUIA_Frame, MUIV_Frame_String,
                            MUIA_String_MaxLen, 1024,
                            End),
                        Child, bt_go = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/go.png",
            	                End,
            	            End,
            	        End,
                    Child, HGroup,
                        MUIA_HorizWeight, 25,
                        MUIA_Group_Spacing, 2,
                        Child, (IPTR) (searchCriteria = StringObject,
                            MUIA_CycleChain, 1,
                            MUIA_String_MaxLen, 1024,
                            MUIA_Frame, MUIV_Frame_String,
                            End),
                        Child, bt_search = HGroup,
            	            MUIA_Frame, MUIV_Frame_None,
            	            MUIA_InputMode, MUIV_InputMode_RelVerify,
            	            Child, ImageObject,
            	            	MUIA_Image_Spec, "3:PROGDIR:resources/search.png",
            	                End,
            	            End,
            	        End,
                    End,
                Child, VGroup,
                    MUIA_Group_Spacing, 0,
        	    Child, tabs = NewObject(Tabs_CLASS->mcc_Class, NULL,
        	        MUIA_Group_Spacing, 4,
                        MUIA_ContextMenu, tabContextMenu = MenustripObject,
                            MUIA_Family_Child, MenuObject,
                                MUIA_Menu_Title, _(MSG_Tabs),
                                MUIA_Family_Child, menuclosetab = MenuitemObject,
                                    MUIA_Menuitem_Title, _(MSG_Tabs_CloseActive),
                                    End,
                                MUIA_Family_Child, menunewtab = MenuitemObject,
                                    MUIA_Menuitem_Title, _(MSG_Tabs_OpenNew),
                                    End,
                                MUIA_Family_Child, menubookmarktab = MenuitemObject,
                                    MUIA_Menuitem_Title, _(MSG_Tabs_Bookmark),
                                    End,
                                End,
                            End,
        	    	MUIA_Tabs_Location, MUIV_Tabs_Top,
        	        MUIA_Tabs_CloseButton, bt_close = HGroup,
        	            MUIA_Frame, MUIV_Frame_ImageButton,
        	            MUIA_InputMode, MUIV_InputMode_RelVerify,
        	            Child, ImageObject,
        	            	MUIA_Image_Spec, "3:PROGDIR:resources/close.png",
        	                End,
        	            End,
        	        Child, tab = NewObject(BrowserTab_CLASS->mcc_Class, NULL, TAG_DONE),
        	        TAG_END),
        	    Child, tabbed = NewObject(TabbedView_CLASS->mcc_Class, NULL,
        	        MUIA_Group_Horiz, FALSE,
                        MUIA_VertWeight, 99,
        	        MUIA_InnerTop, 4,
        	        MUIA_InnerLeft, 5,
        	        MUIA_InnerRight, 5,
        	        MUIA_InnerBottom, 5,
                        Child, webView = NewObject(WebView_CLASS->mcc_Class, NULL, 
                            MUIA_WebView_CreateNewHook, &openTabHook,
                            MUIA_WebView_AlertHook, &alertHook,
                            MUIA_WebView_ConfirmHook, &confirmHook,
                            MUIA_WebView_PromptHook, &promptHook,
                            MUIA_WebView_PolicyHook, &policyHook,
                            MUIA_WebView_CredentialsHook, &credentialsHook,
                            TAG_END),
                        TAG_END),
                    End,
                Child, HGroup,
                    Child, HGroup,
                        Child, bt_download = HGroup,
                	    MUIA_Frame, MUIV_Frame_None,
                	    MUIA_InputMode, MUIV_InputMode_Toggle,
                	    Child, ImageObject,
                	        MUIA_Image_Spec, "3:PROGDIR:resources/download.png",
                	        End,
                	    End,
                        Child, bt_preferences = HGroup,
                	    MUIA_Frame, MUIV_Frame_None,
                	    MUIA_InputMode, MUIV_InputMode_Toggle,
                	    Child, ImageObject,
                	        MUIA_Image_Spec, "3:PROGDIR:resources/preferences.png",
                	        End,
                	    End,
                        Child, bt_find = HGroup,
                	    MUIA_Frame, MUIV_Frame_None,
                	    MUIA_InputMode, MUIV_InputMode_Toggle,
                	    Child, ImageObject,
                	        MUIA_Image_Spec, "3:PROGDIR:resources/find.png",
                	        End,
                	    End,
                        Child, bt_bookmarks = HGroup,
                	    MUIA_Frame, MUIV_Frame_None,
                	    MUIA_InputMode, MUIV_InputMode_Toggle,
                	    Child, ImageObject,
                	        MUIA_Image_Spec, "3:PROGDIR:resources/bookmarks.png",
                	        End,
                	    End,
                    	End,
                    Child, toolTips = TextObject,
                        MUIA_Frame, MUIV_Frame_Text,
                    	MUIA_HorizWeight, 90,
                    	End,
                    Child, progressBar = GaugeObject,
                        MUIA_HorizWeight, 10,
                    	MUIA_Gauge_Horiz, TRUE,
                    	End,
                    End,
               End,
            End,
        SubWindow, searchWindow = NewObject(SearchWindow_CLASS->mcc_Class, NULL, MUIA_SearchWindow_SearchHook, &textSearchHook, TAG_END),
        SubWindow, downloadManager = NewObject(DownloadManager_CLASS->mcc_Class, NULL, 
            MUIA_DownloadManager_Preferences, preferences,
            TAG_END),
        SubWindow, preferencesManager = NewObject(PreferencesManager_CLASS->mcc_Class, NULL, 
            MUIA_PreferencesManager_Preferences, preferences,
            TAG_END),
        SubWindow, bookmarkManager = NewObject(BookmarkManager_CLASS->mcc_Class, NULL, 
            TAG_END),
        End;

    if (app != NULL)
    {
        ULONG sigs = 0;

        set(urlString, MUIA_ContextMenu, XGET(bookmarkManager, MUIA_BookmarkManager_BookmarkMenu));
        
        DoMethod(preferencesManager, MUIM_PreferencesManager_Load);
        
        /* Click Close gadget or hit Escape to quit */
        DoMethod(wnd, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        	(IPTR)app, 2,
        	MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

        /* Close active tab */
        DoMethod(menuclosetab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        	(IPTR) tabs, 3,
        	MUIM_CallHook, &closeTabHook, NULL);

        DoMethod(wnd, MUIM_Notify, MUIA_Window_InputEvent, "control w",
        	(IPTR) tabs, 3,
        	MUIM_CallHook, &closeTabHook, NULL);

        /* Open new tab */
        DoMethod(menunewtab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        	(IPTR) tabs, 3,
        	MUIM_CallHook, &openTabHook, NULL);

        DoMethod(wnd, MUIM_Notify, MUIA_Window_InputEvent, "control t",
        	(IPTR) tabs, 3,
        	MUIM_CallHook, &openTabHook, NULL);

        /* Bookmark tab */
        DoMethod(menubookmarktab, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
        	(IPTR) tabs, 3,
        	MUIM_CallHook, &bookmarkTabHook, bookmarkManager);

        /* Switch active tab */
	DoMethod(tabs, MUIM_Notify, MUIA_Tabs_ActiveTab, MUIV_EveryTime,
		(IPTR) tabbed, (IPTR) 3,
		MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue);

	/* Close active tab */
	DoMethod(bt_close, MUIM_Notify, MUIA_Pressed, FALSE,
		(IPTR) tabs, (IPTR) 3,
		MUIM_CallHook, &closeTabHook, NULL);

        /* Go */
        DoMethod(bt_go, MUIM_Notify, MUIA_Pressed, FALSE,
                 (IPTR) urlString, 3,
                 MUIM_CallHook, &goHook, NULL);

        /* Go acknowledged */
        DoMethod(urlString, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
        	 (IPTR) urlString, 3,
        	 MUIM_CallHook, &goAcknowledgeHook, MUIV_TriggerValue);

        /* Bookmark chosen */
        DoMethod(bookmarkManager, MUIM_Notify, MUIA_BookmarkManager_SelectedURL, MUIV_EveryTime,
        	 (IPTR) urlString, 3,
        	 MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);
        
        DoMethod(bookmarkManager, MUIM_Notify, MUIA_BookmarkManager_SelectedURL, MUIV_EveryTime,
        	 (IPTR) urlString, 3,
        	 MUIM_CallHook, &goAcknowledgeHook, MUIV_TriggerValue);

        /* Web Search */
        DoMethod(bt_search, MUIM_Notify, MUIA_Pressed, FALSE,
                 (IPTR) searchCriteria, 3,
                 MUIM_CallHook, &webSearchHook, NULL);

        /* Web Search acknowledged */
        DoMethod(searchCriteria, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
        	 (IPTR) searchCriteria, 3,
        	 MUIM_CallHook, &webSearchAcknowledgeHook, MUIV_TriggerValue);

        /* Download manager window */
        DoMethod(bt_download, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
                 (IPTR) downloadManager, 3,
                 MUIM_Set, MUIA_Window_Open, MUIV_TriggerValue);

        DoMethod(downloadManager, MUIM_Notify, MUIA_Window_Open, MUIV_EveryTime,
                 (IPTR) bt_download, 3,
                 MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

        /* Text Search window */
        DoMethod(bt_find, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
                 (IPTR) searchWindow, 3,
                 MUIM_Set, MUIA_Window_Open, MUIV_TriggerValue); 

        DoMethod(searchWindow, MUIM_Notify, MUIA_Window_Open, MUIV_EveryTime,
                 (IPTR) bt_find, 3,
                 MUIM_Set, MUIA_Selected, MUIV_TriggerValue); 

        /* Preferences */
        DoMethod(bt_preferences, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
                 (IPTR) preferencesManager, 3,
                 MUIM_Set, MUIA_Window_Open, MUIV_TriggerValue);

        DoMethod(preferencesManager, MUIM_Notify, MUIA_Window_Open, MUIV_EveryTime,
                 (IPTR) bt_preferences, 3,
                 MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

        /* Bookmark manager */
        DoMethod(bt_bookmarks, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
                 (IPTR) bookmarkManager, 3,
                 MUIM_Set, MUIA_Window_Open, MUIV_TriggerValue);

        DoMethod(bookmarkManager, MUIM_Notify, MUIA_Window_Open, MUIV_EveryTime,
                 (IPTR) bt_bookmarks, 3,
                 MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
        
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
        	 (IPTR) toolTips, 3,
        	 MUIM_CallHook, &updateStatusBarHook, MUIV_TriggerValue);

        /* Display state info */
        DoMethod(tabbed, MUIM_Notify, MUIA_WebView_State, MUIV_EveryTime,
                 (IPTR) toolTips, 3,
                 MUIM_CallHook, &updateStatusBarHook, "");

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
                 (IPTR) progressBar, 3,
                 MUIM_CallHook, &updateProgressBarHook, MUIV_TriggerValue);
    
        /* Update URL */
        DoMethod(tabbed, MUIM_Notify, MUIA_WebView_URL, MUIV_EveryTime,
                 (IPTR) urlString, 3,
                 MUIM_CallHook, &updateURLHook, MUIV_TriggerValue);
        
        /* Update title */
        DoMethod(tabbed, MUIM_Notify, MUIA_WebView_Title, MUIV_EveryTime,
                 (IPTR) wnd, 3,
                 MUIM_Set, MUIA_Window_Title, MUIV_TriggerValue);
    
        /* Update back button state */
        DoMethod(tabbed, MUIM_Notify, MUIA_WebView_CanGoBack, MUIV_EveryTime,
    	    (IPTR) bt_back, 3,
    	    MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
    
        /* Update forward button state */
        DoMethod(tabbed, MUIM_Notify, MUIA_WebView_CanGoForward, MUIV_EveryTime,
    	    (IPTR) bt_forward, 3,
    	    MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

        /* Back and forward buttons are disabled by default */
       	set(bt_back, MUIA_Disabled, TRUE);
       	set(bt_forward, MUIA_Disabled, TRUE);
       	set(bt_stop, MUIA_Disabled, TRUE);

        /* Open the window */
        set(wnd, MUIA_Window_Open, TRUE);

        /* Check that the window opened */
        if (XGET(wnd, MUIA_Window_Open))
        {
            /* Display initial state information */
            CallHook(&updateStatusBarHook, toolTips, "");
            
            if(url)
        	CallHook(&goAcknowledgeHook, urlString, url);
            
            /* Main loop */
            while((LONG)DoMethod(app, MUIM_Application_NewInput, (IPTR)&sigs)
                  != MUIV_Application_ReturnID_Quit)
            {
                if (sigs)
                {
                    sigs = Wait(sigs | SIGBREAKF_CTRL_C);
                    if (sigs & SIGBREAKF_CTRL_C)
                	break;
                }
            }
        }
        /* Destroy our application and all its objects */
        MUI_DisposeObject(app);
        MUI_DisposeObject(preferences);
        MUI_DisposeObject(tabContextMenu);
    }

    Locale_Deinitialize();
    
    if(url)
	FreeVec(url);
    return 0;
}
