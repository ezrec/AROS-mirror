#include <exec/types.h>
#include <libraries/mui.h>
#include <dos/var.h>
#include <utility/tagitem.h>
#include <devices/rawkeycodes.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/alib.h>
#include <proto/asl.h>
#include <aros/debug.h>
#include <libraries/codesets.h>
#include <libraries/asl.h>
#include <proto/utility.h>
#include <proto/codesets.h>
#include <zune/customclasses.h>
#include <devices/input.h>

#include <WebViewZune.h>

#include "locale.h"
#include "downloadmanager.h"
#include "preferencesmanager.h"
#include "browserpreferences.h"
#include "bookmarkmanager.h"
#include "browserwindow.h"

#include "browserapp.h"
#include "browserapp_private.h"

#include "version.h"

static Object* getActiveWindow(Object* app)
{
    struct List *wlist;
    APTR         wstate;
    Object      *curwin;

    get(app, MUIA_Application_WindowList, &wlist);

    if (wlist)
    {
	wstate = wlist->lh_Head;
	while ((curwin = NextObject(&wstate)))
	{
	    if(XGET(curwin, MUIA_Window_Activate))
	    {
		return curwin;
	    }
	}
    }
    
    return NULL;
}

static IPTR ForwardCallToActiveWindowFunc(struct Hook *hook, Object *app, IPTR *msg)
{   
    struct List *wlist;
    APTR         wstate;
    Object      *curwin;

    get(app, MUIA_Application_WindowList, &wlist);

    if (wlist)
    {
	wstate = wlist->lh_Head;
	while ((curwin = NextObject(&wstate)))
	{
	    if(XGET(curwin, MUIA_Window_Activate))
	    {
		return DoMethodA(curwin, (Msg) msg);
	    }
	}
    }
    return TRUE;
}

static IPTR RequestFileFunc(struct Hook *hook, Object *app, struct Hook **forwardCallHook)
{
    static char dirpart[500]="SYS:",filepart[500],fullname[1000];
    struct FileRequester *fr;

    struct TagItem frtags[] =
    {
	{ ASLFR_TitleText,     (IPTR)_(MSG_MainMenu_OpenFile) },
	{ ASLFR_InitialDrawer, (IPTR)dirpart                  },
	{ ASLFR_InitialFile,   (IPTR)filepart                 },
	{ TAG_END                                             }
    }; 

    if((fr = (struct FileRequester *) AllocAslRequest(ASL_FileRequest, frtags)))
    {
	if(AslRequest(fr, NULL))
	{
	    strncpy(dirpart,fr->fr_Drawer,498);
	    strncpy(filepart,fr->fr_File,498);
	    strcpy(fullname, "file:///");
	    strncat(fullname, fr->fr_Drawer, 490);
	    AddPart(fullname, fr->fr_File, 499);
	    
	    DoMethod(app, MUIM_CallHook, *forwardCallHook, MUIM_BrowserWindow_OpenURLInActiveTab, fullname);
	}
	FreeAslRequest(fr);
    }
    else
    {
	return FALSE;
    }
    
    return TRUE;
}

static IPTR CloseWindowFunc(struct Hook *hook, Object *app, Object **win)
{
    DoMethod(*win, MUIA_Window_Open, FALSE);
    DoMethod(app, OM_REMMEMBER, *win);
    MUI_DisposeObject(*win);
    return (IPTR)NULL;
}

static IPTR SynthetizeKeyboardEventFunc(struct Hook *hook, Object *app, IPTR *data)
{
    BYTE code = data[0];
    BYTE qualifier = data[1];
    struct IOStdReq *inputIO;
    struct MsgPort *inputMP;
    struct InputEvent *fakeEvent;

    if ((inputMP = CreateMsgPort()))
    {
        if ((fakeEvent = AllocMem(sizeof(struct InputEvent), MEMF_PUBLIC | MEMF_CLEAR)))
        {
            if ((inputIO = CreateIORequest(inputMP, sizeof(struct IOStdReq))))
            {
                if (!OpenDevice("input.device", 0, (struct IORequest *) inputIO, 0))
                {
		    /* Set up InputEvent fields */
		    fakeEvent->ie_Class = IECLASS_RAWKEY;
		    fakeEvent->ie_Code = code;
		    fakeEvent->ie_Qualifier = qualifier;

		    /* InputEvent */
		    inputIO->io_Data = (APTR)fakeEvent;
		    inputIO->io_Length = sizeof(struct InputEvent);
		    inputIO->io_Command = IND_WRITEEVENT;

		    DoIO((struct IORequest *)inputIO);

		    CloseDevice((struct IORequest *) inputIO);
		}
		DeleteIORequest(inputIO);
            }
            FreeMem(fakeEvent,sizeof(struct InputEvent));
        }
        DeleteMsgPort(inputMP);
    }

    return 0;
}
/*** Methods ****************************************************************/
IPTR BrowserApp__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct BrowserApp_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    
    Object *downloadManager, *preferencesManager, *bookmarkManager;
    Object *preferences, *fileMenu, *mainMenustrip;
    Object *quitMenuItem, *openTabMenuItem, *openWindowMenuItem, *closeTabMenuItem, *closeWindowMenuItem;
    Object *findMenuItem, *findNextMenuItem, *bookmarkManagerMenuItem, *downloadManagerMenuItem;
    Object *cutMenuItem, *copyMenuItem, *pasteMenuItem;
    Object *zoomInMenuItem, *zoomOutMenuItem, *zoomResetMenuItem, *viewSourceMenuItem;
    Object *preferencesManagerMenuItem, *openFileMenuItem;

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

    preferences = NewObject(BrowserPreferences_CLASS->mcc_Class, NULL, TAG_END);
    if(!preferences)
	return (IPTR) NULL;
    
    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Application_Title, (IPTR)"Origyn Web Browser",
        MUIA_Application_Version, (IPTR)"$VER: OWB " OWB_VERSION " (" OWB_RELEASE_DATE ")",
        MUIA_Application_Author, (IPTR)"Stanislaw Szymczyk",
        MUIA_Application_Copyright, (IPTR)"Copyright © 2009, The AROS Development Team. All rights reserved.",
        MUIA_Application_Description, (IPTR)"Port of Origyn Web Browser to AROS",
        MUIA_Application_Base, (IPTR)"OWB",
        MUIA_Application_Menustrip, (IPTR)(mainMenustrip = MenustripObject,
	    MUIA_Family_Child, (IPTR)(fileMenu = MenuObject,
		MUIA_Menu_Title, (IPTR)_(MSG_MainMenu_File),
		MUIA_Family_Child, (IPTR)(openTabMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_OpenNewTab),
		    End),
		MUIA_Family_Child, (IPTR)(openWindowMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_OpenNewWindow),
		    End),
		MUIA_Family_Child, (IPTR)(openFileMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_OpenFile),
		    End),
		MUIA_Family_Child, (IPTR)(closeTabMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_CloseCurrentTab),
		    End),
		MUIA_Family_Child, (IPTR)(closeWindowMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_CloseWindow),
		    End),
		MUIA_Family_Child, (IPTR)(MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)NM_BARLABEL,
		    End),
		MUIA_Family_Child, (IPTR)(quitMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Quit),
		    End),
		End),
	    MUIA_Family_Child, (IPTR)(MenuObject,
		MUIA_Menu_Title, (IPTR)_(MSG_MainMenu_Edit),
		MUIA_Family_Child, (IPTR)(findMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Find),
		    End),
		MUIA_Family_Child, (IPTR)(findNextMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_FindNext),
		    End),
		MUIA_Family_Child, (IPTR)(MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)NM_BARLABEL,
		    End),
		MUIA_Family_Child, (IPTR)(cutMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Cut),
		    End),
		MUIA_Family_Child, (IPTR)(copyMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Copy),
		    End),
		MUIA_Family_Child, (IPTR)(pasteMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Paste),
		    End),
		MUIA_Family_Child, (IPTR)(MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)NM_BARLABEL,
		    End),
	        MUIA_Family_Child, (IPTR)(preferencesManagerMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_Preferences),
		    End),
		End),
	    MUIA_Family_Child, (IPTR)(MenuObject,
		MUIA_Menu_Title, (IPTR)_(MSG_MainMenu_View),
		MUIA_Family_Child, (IPTR)(zoomInMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_ZoomIn),
		    End),
		MUIA_Family_Child, (IPTR)(zoomOutMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_ZoomOut),
		    End),
		MUIA_Family_Child, (IPTR)(zoomResetMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_ZoomReset),
		    End),
		MUIA_Family_Child, (IPTR)(MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)NM_BARLABEL,
		    End),
		MUIA_Family_Child, (IPTR)(viewSourceMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_ViewSource),
		    End),
		End),
	    MUIA_Family_Child, (IPTR)(MenuObject,
		MUIA_Menu_Title, (IPTR)_(MSG_MainMenu_Tools),
		MUIA_Family_Child, (IPTR)(bookmarkManagerMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_BookmarkManager),
		    End),
		MUIA_Family_Child, (IPTR)(downloadManagerMenuItem = MenuitemObject,
		    MUIA_Menuitem_Title, (IPTR)_(MSG_MainMenu_DownloadManager),
		    End),
		End),
            End),
        SubWindow, (IPTR)(downloadManager = NewObject(DownloadManager_CLASS->mcc_Class, NULL, 
	    MUIA_Window_Width, MUIV_Window_AltWidth_Screen(50),
	    MUIA_Window_Height, MUIV_Window_AltHeight_Screen(50),
	    MUIA_DownloadManager_Preferences, (IPTR)preferences,
            TAG_END)),
        SubWindow, (IPTR)(preferencesManager = NewObject(PreferencesManager_CLASS->mcc_Class, NULL, 
            MUIA_PreferencesManager_Preferences, (IPTR)preferences,
            TAG_END)),
        SubWindow, (IPTR)(bookmarkManager = NewObject(BookmarkManager_CLASS->mcc_Class, NULL, 
	    MUIA_Window_Width, MUIV_Window_AltWidth_Screen(50),
	    MUIA_Window_Height, MUIV_Window_AltHeight_Screen(50),
       	    TAG_END)),
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    data = INST_DATA(CLASS, self);
    data->preferencesManager = preferencesManager;
    data->bookmarkManager = bookmarkManager;
    data->downloadManager = downloadManager;
    data->forwardCallToActiveWindowHook.h_Entry = HookEntry;
    data->forwardCallToActiveWindowHook.h_SubEntry = (HOOKFUNC) ForwardCallToActiveWindowFunc;
    data->requestFileHook.h_Entry = HookEntry;
    data->requestFileHook.h_SubEntry = (HOOKFUNC) RequestFileFunc;
    data->closeWindowHook.h_Entry = HookEntry;
    data->closeWindowHook.h_SubEntry = (HOOKFUNC) CloseWindowFunc;
    data->synthetizeKeyboardEventHook.h_Entry = HookEntry;
    data->synthetizeKeyboardEventHook.h_SubEntry = (HOOKFUNC) SynthetizeKeyboardEventFunc;
    data->preferences = preferences;

    DoMethod(data->preferencesManager, MUIM_PreferencesManager_Load);

    DoMethod(mainMenustrip,MUIM_Family_AddTail,XGET(bookmarkManager, MUIA_BookmarkManager_BookmarkMenu));

    /* Open tab menu item */
    DoMethod(openTabMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_OpenNewTab, MUIV_BrowserWindow_ForegroundTab);

    /* Open window menu item */
    DoMethod(openWindowMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 2,
	    MUIM_BrowserApp_OpenNewWindow, NULL);

    /* Close window menu item */
    DoMethod(closeWindowMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 1,
	    MUIM_BrowserApp_CloseActiveWindow);

    /* Close tab menu item */
    DoMethod(closeTabMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 3,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_CloseActiveTab);

    /* Find menu item */
    DoMethod(findMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 3,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_Find);

    /* Find next menu item */
    DoMethod(findNextMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 3,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_FindNext);

    /* Bookmark menu items */
    DoMethod(bookmarkManager, MUIM_Notify, MUIA_BookmarkManager_SelectedURL, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_OpenURLInActiveTab, MUIV_TriggerValue);

    /* Quit menu item */
    DoMethod(quitMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 2,
	    MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    /* Bookmark manager menu item */
    DoMethod(bookmarkManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    bookmarkManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Open, (IPTR) TRUE);
    DoMethod(bookmarkManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    bookmarkManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Activate, (IPTR) TRUE);
    
    /* Download manager menu item */
    DoMethod(downloadManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    downloadManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Open, (IPTR) TRUE);
    DoMethod(downloadManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    downloadManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Activate, (IPTR) TRUE);

    /* Clipboard menu items */
    DoMethod(cutMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->synthetizeKeyboardEventHook, RAWKEY_X, IEQUALIFIER_RCOMMAND);
    DoMethod(copyMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->synthetizeKeyboardEventHook, RAWKEY_C, IEQUALIFIER_RCOMMAND);
    DoMethod(pasteMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->synthetizeKeyboardEventHook, RAWKEY_V, IEQUALIFIER_RCOMMAND);

    /* Preferences manager menu item */
    DoMethod(preferencesManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    preferencesManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Open, (IPTR) TRUE);
    DoMethod(preferencesManagerMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    preferencesManager, (IPTR) 3,
	    MUIM_Set, MUIA_Window_Activate, (IPTR) TRUE);

    /* View menu items */
    DoMethod(zoomInMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_Zoom, MUIV_BrowserWindow_ZoomIn);
    DoMethod(zoomOutMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_Zoom, MUIV_BrowserWindow_ZoomOut);
    DoMethod(zoomResetMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 4,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_Zoom, MUIV_BrowserWindow_ZoomReset);
    DoMethod(viewSourceMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 3,
	    MUIM_CallHook, &data->forwardCallToActiveWindowHook, MUIM_BrowserWindow_ShowSource);

    /* Find next menu item */
    DoMethod(openFileMenuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	    (IPTR) self, (IPTR) 3,
	    MUIM_CallHook, &data->requestFileHook, &data->forwardCallToActiveWindowHook);

    return (IPTR) self;
}

IPTR BrowserApp__OM_DISPOSE(Class *CLASS, Object *self, Msg message)
{
    struct BrowserApp_DATA *data = INST_DATA(CLASS, self);

    IPTR ret = DoSuperMethodA(CLASS, self, message);

    MUI_DisposeObject(data->preferences);

    return ret;
}

IPTR BrowserApp__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct BrowserApp_DATA *data = INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;
    
    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
    	    case MUIA_BrowserApp_BookmarkManagerOpened:
    	    {
    		BOOL opened = tag->ti_Data;
    		if(opened)
    		{
    		    set(data->bookmarkManager, MUIA_Window_Open, TRUE);
    		    set(data->bookmarkManager, MUIA_Window_Activate, TRUE);
    		}
    		else
    		{
    		    set(data->bookmarkManager, MUIA_Window_Open, FALSE);
    		}
    		break;
    	    }
    	    case MUIA_BrowserApp_DownloadManagerOpened:
    	    {
    		BOOL opened = tag->ti_Data;
    		if(opened)
    		{
    		    set(data->downloadManager, MUIA_Window_Open, TRUE);
    		    set(data->downloadManager, MUIA_Window_Activate, TRUE);
    		}
    		else
    		{
    		    set(data->downloadManager, MUIA_Window_Open, FALSE);
    		}
    		break;
    	    }
    	    case MUIA_BrowserApp_PreferencesOpened:
    	    {
    		BOOL opened = tag->ti_Data;
    		if(opened)
    		{
    		    set(data->preferencesManager, MUIA_Window_Open, TRUE);
    		    set(data->preferencesManager, MUIA_Window_Activate, TRUE);
    		}
    		else
    		{
    		    set(data->preferencesManager, MUIA_Window_Open, FALSE);
    		}
    		break;
    	    }
    	    default:
    		continue;
	}
    }
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR BrowserApp__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct BrowserApp_DATA *data = INST_DATA(cl, obj);
    IPTR retval = TRUE;
    
    switch(msg->opg_AttrID)
    {
	case MUIA_BrowserApp_BookmarkManagerOpened:
	    *(BOOL*)msg->opg_Storage = XGET(data->bookmarkManager, MUIA_Window_Open);
	    break;
	case MUIA_BrowserApp_DownloadManagerOpened:
	    *(BOOL*)msg->opg_Storage = XGET(data->downloadManager, MUIA_Window_Open);
	    break;
	case MUIA_BrowserApp_PreferencesOpened:
	    *(BOOL*)msg->opg_Storage = XGET(data->preferencesManager, MUIA_Window_Open);
	    break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg)msg);
	    break;
    }
    
    return retval;
}

IPTR BrowserApp__MUIM_BrowserApp_OpenNewWindow(Class *cl, Object *app, struct MUIP_BrowserApp_OpenNewWindow *msg)
{
    Object* webView = NULL;
    Object* wnd = NULL;
    
    if(msg->specification)
    {
	wnd = NewObject(BrowserWindow_CLASS->mcc_Class, NULL,
	    MUIA_Window_Width, (IPTR) msg->specification->width,
	    MUIA_Window_Height, (IPTR) msg->specification->height,
	    MUIA_Window_SizeGadget, (IPTR) msg->specification->resizable,
	    MUIA_Window_AltWidth, MUIV_Window_AltWidth_Screen(75),
	    MUIA_Window_AltHeight, MUIV_Window_AltHeight_Screen(75),
	    MUIA_Window_NoMenus, (IPTR) !msg->specification->menubar,
	    MUIA_BrowserWindow_ShowStatusBar, (IPTR) msg->specification->statusbar,
	    MUIA_BrowserWindow_ShowToolBar, (IPTR) msg->specification->toolbar,
	    MUIA_BrowserWindow_ShowLocationBar, (IPTR) msg->specification->locationbar,
	    MUIA_BrowserWindow_ShowScrollbars, (IPTR) msg->specification->scrollbars,
	    MUIA_BrowserWindow_WebViewPointer, &webView,
	    TAG_END);
    }
    else
    {
	wnd = NewObject(BrowserWindow_CLASS->mcc_Class, NULL,
	    MUIA_Window_Width, MUIV_Window_AltWidth_Screen(100),
	    MUIA_Window_Height, MUIV_Window_AltHeight_Screen(100),
	    MUIA_Window_AltWidth, MUIV_Window_AltWidth_Screen(75),
	    MUIA_Window_AltHeight, MUIV_Window_AltHeight_Screen(75),
	    MUIA_BrowserWindow_WebViewPointer, &webView,
	    TAG_END);	
    }
    
    DoMethod(app, OM_ADDMEMBER, wnd);

    set(wnd, MUIA_Window_Open, TRUE);
    set(wnd, MUIA_Window_Activate, TRUE);
	
    return (IPTR) webView;
}

IPTR BrowserApp__MUIM_BrowserApp_CloseActiveWindow(Class *cl, Object *obj, Msg msg)
{
    Object *win = getActiveWindow(obj);
    if(win)
    {
	DoMethod(obj, MUIM_BrowserApp_CloseWindow, win);
    }
    return TRUE;
}

IPTR BrowserApp__MUIM_BrowserApp_CloseWindow(Class *cl, Object *obj, struct MUIP_BrowserApp_CloseWindow *msg)
{
    struct BrowserApp_DATA *data = INST_DATA(cl, obj);

    /* count the number of browser windows */
    int windowsCount = 0;
    struct List *wlist;
    APTR         wstate;
    Object      *curwin;

    get(obj, MUIA_Application_WindowList, &wlist);

    if (wlist)
    {
	wstate = wlist->lh_Head;
	while ((curwin = NextObject(&wstate)))
	{
	    if(OCLASS(curwin) == BrowserWindow_CLASS->mcc_Class)
	    {
		windowsCount++;
	    }
	}
    }
    
    /* if it's the last one, exit the application */
    if(windowsCount == 1)
    {
	DoMethod(obj, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    }
    else
    {
	DoMethod(_app(msg->win), MUIM_Application_PushMethod, obj, 3, MUIM_CallHook, &data->closeWindowHook, msg->win);
    }
    
    return TRUE;
}

IPTR BrowserApp__MUIM_BrowserApp_Bookmark(struct IClass *cl, Object *obj, struct MUIP_BrowserApp_Bookmark *msg)
{
    struct BrowserApp_DATA *data = (struct BrowserApp_DATA *) INST_DATA(cl, obj);
    
    return DoMethod(data->bookmarkManager, MUIM_BookmarkManager_Bookmark, msg->title, msg->url);
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_8
(
    BrowserApp, NULL, MUIC_Application, NULL,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    OM_SET, struct opSet*,
    OM_GET, struct opGet*,
    MUIM_BrowserApp_OpenNewWindow, struct MUIP_BrowserApp_OpenNewWindow*,
    MUIM_BrowserApp_CloseActiveWindow, Msg,
    MUIM_BrowserApp_CloseWindow, struct MUIP_BrowserApp_CloseWindow*,
    MUIM_BrowserApp_Bookmark, struct MUIP_BrowserApp_Bookmark*
)
