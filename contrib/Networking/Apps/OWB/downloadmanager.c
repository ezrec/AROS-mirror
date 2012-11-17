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
#include <proto/asl.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <proto/intuition.h>
#include <proto/alib.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <string.h>
#include <stdio.h>
#include <aros/debug.h>
#include <aros/symbolsets.h>
#include <zune/customclasses.h>

#include "DownloadDelegateZune.h"
#include "browserpreferences.h"

#include "downloadmanager_private.h"
#include "downloadmanager.h"

#include "locale.h"

enum { DOWNLOAD_CREATED, DOWNLOAD_ACTIVE, DOWNLOAD_ERROR, DOWNLOAD_FINISHED, DOWNLOAD_DESTROYED };

static IPTR ManagerDisplayFunc(struct Hook *hook, char **columns, struct Download *download)
{
    if(download == NULL)
    {
	columns[0] = (char*) _(MSG_DownloadFileName);
	columns[1] = (char*) _(MSG_DownloadFileSize);
	columns[2] = (char*) _(MSG_DownloadProgress);
	columns[3] = (char*) _(MSG_DownloadSpeed);
	columns[4] = (char*) _(MSG_DownloadState);
    }
    else
    {
        columns[0] = (download->filename ? download->filename : (char*) _(MSG_UnknownFileName));
        columns[1] = (download->filesize ? download->filesize : (char*) _(MSG_UnknownFileSize));
        columns[2] = download->progress;
        columns[3] = (download->speed ? download->speed : (char*) _(MSG_UnknownDownloadSpeed));
        switch(download->state)
        {
        case DOWNLOAD_CREATED:
            columns[4] = (char*) _(MSG_DownloadState_Starting);
            break;
        case DOWNLOAD_ACTIVE:
            columns[4] = (char*) _(MSG_DownloadState_Active);
            break;
        case DOWNLOAD_ERROR:
            columns[4] = (char*) _(MSG_DownloadState_Failed);
            break;
        case DOWNLOAD_FINISHED:
            columns[4] = (char*) _(MSG_DownloadState_Finished);
            break;
        default:
            columns[4] = "";
            break;
        }
    }
    D(bug("displaying %s | %s | %s\n", columns[0], columns[1], columns[2]));
    return 1;
}

static IPTR ManagerDestroyFunc(struct Hook *hook, APTR pool, struct Download *download)
{
    if(!download)
	return 0;

    download->state = DOWNLOAD_DESTROYED;
    FreeVec(download->filename);
    FreeVec(download->filesize);
    FreeVec(download->speed);
    Remove((struct Node*) download);
    FreeVec(download);
    
    return 0;
}

static IPTR CancelFunc(struct Hook *hook, Object *list, void *data)
{
    LONG active = 0;
    get(list, MUIA_List_Active, &active);
    
    if(active == MUIV_List_Active_Off)
	return 0;
    
    struct Download *download = NULL;
    DoMethod(list, MUIM_List_GetEntry, active, &download);
    
    if(download)
    {
	Object *manager = download->manager;
	DoMethod(manager, MUIM_DownloadDelegate_CancelDownload, (IPTR) download->identifier);
	DoMethod(list, MUIM_List_Remove, active);
	DoMethod(manager, MUIM_DownloadManager_UpdateInterface);
    }
    
    return 0;
}

static IPTR ClearFinishedFunc(struct Hook *hook, Object *list, void *data)
{
    int i;
    for(i = XGET(list, MUIA_List_Entries) - 1; i>= 0; i--)
    {
	struct Download *download = NULL;
	DoMethod(list, MUIM_List_GetEntry, i, &download);
	if(download)
	{
	    if(download->state == DOWNLOAD_FINISHED)
		DoMethod(list, MUIM_List_Remove, i);
	}
    }
    return 0;
}

static struct Download *GetDownload(struct DownloadManager_DATA *manager, void *downid)
{
    struct Download *download;
    ForeachNode(&manager->downloads, download) 
    {
	if(download->identifier == downid)
	    return download;
    }
    return NULL;
}

IPTR DownloadManager__OM_NEW(struct IClass *cl, Object *self, struct opSet *msg)
{
    Object *list = NULL, *bt_cancel, *bt_clearFinished;
    Object *preferences = NULL;
    struct TagItem *tag, *tags;
    
    for (tags = msg->ops_AttrList; (tag = NextTagItem(&tags)); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_DownloadManager_Preferences:
        	preferences = (Object*) tag->ti_Data;
        	break;
            default:
                continue; /* Don't supress non-processed tags */
        }
        tag->ti_Tag = TAG_IGNORE;
    }
    
    if(!preferences) {
        D(bug("DownloadManager: No preferences given\n"));
        return (IPTR) NULL;
    }

    self = (Object *) DoSuperNewTags
    (
        cl, self, NULL,
    
        MUIA_Window_Title, _(MSG_DownloadManager_Title),
    	WindowContents, (IPTR)(VGroup,
    	    MUIA_InnerLeft, 5,
    	    MUIA_InnerRight, 5,
    	    MUIA_InnerTop, 5,
    	    MUIA_InnerBottom, 5,
    	    Child, (IPTR)(ListviewObject,
                MUIA_Listview_List, (IPTR)(list = ListObject,
            	    MUIA_List_Format, (IPTR)"BAR,BAR,BAR,BAR,",
            	    MUIA_List_Title, TRUE,
		    End),
	        End),
	    Child, (IPTR)(HGroup,
	        Child, (IPTR)(bt_cancel = SimpleButton(_(MSG_DownloadManager_Cancel))),
	        Child, (IPTR)(bt_clearFinished = SimpleButton(_(MSG_DownloadManager_ClearFinished))),
	        End),
	    End),
        TAG_MORE, (IPTR) msg->ops_AttrList	
    );
    
    if(!self) {
	D(bug("DownloadManager: Could not create self\n"));
	return (IPTR) NULL;
    }
    
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, self);
    
    NewList(&manager->downloads);
    manager->displayHook.h_Entry = HookEntry;
    manager->displayHook.h_SubEntry = (HOOKFUNC) ManagerDisplayFunc;
    manager->destroyHook.h_Entry = HookEntry;
    manager->destroyHook.h_SubEntry = (HOOKFUNC) ManagerDestroyFunc;
    manager->cancelHook.h_Entry = HookEntry;
    manager->cancelHook.h_SubEntry = (HOOKFUNC) CancelFunc;
    manager->clearFinishedHook.h_Entry = HookEntry;
    manager->clearFinishedHook.h_SubEntry = (HOOKFUNC) ClearFinishedFunc;
    manager->list = list;
    manager->preferences = preferences;
    manager->bt_cancel = bt_cancel;

    set(bt_cancel, MUIA_Disabled, TRUE);
    set(list, MUIA_List_DisplayHook, &manager->displayHook);
    set(list, MUIA_List_DestructHook, &manager->destroyHook);
    
    /* Close window with close gadget */
    DoMethod(self, MUIM_Notify, MUIA_Window_CloseRequest, (IPTR) TRUE,
        (IPTR) self, (IPTR) 3,
        MUIM_Set, MUIA_Window_Open, (IPTR) FALSE);

    DoMethod(bt_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) list, (IPTR) 3,
	MUIM_CallHook, &manager->cancelHook, NULL);

    DoMethod(bt_clearFinished, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) list, (IPTR) 3,
	MUIM_CallHook, &manager->clearFinishedHook, NULL);

    DoMethod(list, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
	(IPTR) self, (IPTR) 1,
	MUIM_DownloadManager_UpdateInterface);

    return (IPTR) self;
}

IPTR DownloadManager__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);
    
    /* Destroy all downloads, so they won't send notifications anymore */
    DoMethod(manager->list, MUIM_List_Clear);
    
    return DoSuperMethodA(cl,obj,msg);
}

IPTR DownloadManager__MUIM_DownloadDelegate_DidBeginDownload(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DidBeginDownload* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);

    struct Download *download = (struct Download*) AllocVec(sizeof(struct Download), MEMF_ANY | MEMF_CLEAR);
    download->identifier = message->download;
    download->state = DOWNLOAD_CREATED;
    download->sizeTotal = -1;
    download->sizeDownloaded = 0;
    download->manager = obj;
    download->filename = NULL;
    download->filesize = NULL;
    download->speed = NULL;
    download->speedMeasureTime = time(NULL);
    download->listRedrawTime = time(NULL);
    download->speedSizeDownloaded = 0;
    sprintf(download->progress, "%ld %%", (long)0);

    if(!download)
	return FALSE;

    AddHead(&manager->downloads, (struct Node*) download);

    DoMethod(manager->list, MUIM_List_InsertSingle, download, 0);
    SetAttrs(obj, MUIA_Window_Open, TRUE);
    DoMethod(obj, MUIM_DownloadManager_UpdateInterface);

    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadDelegate_DidFinishDownload(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DidFinishDownload* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);

    struct Download *download = GetDownload(manager, message->download);
    if(!download)
	return FALSE;
    
    download->state = DOWNLOAD_FINISHED;

    sprintf(download->progress, "%ld %%", (long) (download->sizeTotal > 0 ? 100 * download->sizeDownloaded / download->sizeTotal : 0));

    /* Activate finished entry */
    int i;
    for (i=0;;i++)
    {
	struct Download *down;
        DoMethod(manager->list, MUIM_List_GetEntry, i, &down);
        if(!down)
            break;
        
        if(down == download)
        {
            set(manager->list, MUIA_List_Active, i);
            break;
        }
    }

    
    SetAttrs(obj, MUIA_Window_Open, TRUE);
    DoMethod(manager->list, MUIM_List_Redraw, MUIV_List_Redraw_All);
    DoMethod(obj, MUIM_DownloadManager_UpdateInterface);
    
    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadDelegate_DidFailWithError(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DidFailWithError* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);

    struct Download *download = GetDownload(manager, message->download);
    if(!download)
	return FALSE;
    
    download->state = DOWNLOAD_ERROR;
    
    /* Activate failed entry */
    int i;
    for (i=0;;i++)
    {
	struct Download *down;
        DoMethod(manager->list, MUIM_List_GetEntry, i, &down);
        if(!down)
            break;
        
        if(down == download)
        {
            set(manager->list, MUIA_List_Active, i);
            break;
        }
    }

    SetAttrs(obj, MUIA_Window_Open, TRUE);
    DoMethod(manager->list, MUIM_List_Redraw, MUIV_List_Redraw_All);
    DoMethod(obj, MUIM_DownloadManager_UpdateInterface);
    
    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadDelegate_DecideDestinationWithSuggestedFilename(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DecideDestinationWithSuggestedFilename* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);
    char *destination = NULL;

    struct Download *download = GetDownload(manager, message->download);
    if(!download)
	return FALSE;
    
    if(download->filename)
	FreeVec(download->filename);

    char *filename = message->filename;
    
    const char *prefix = (const char*) XGET(manager->preferences, MUIA_BrowserPreferences_DownloadDestination);
    if(strlen(filename) == 0)
	filename = strdup(_(MSG_UnknownFileName));
    
    BOOL requestFileName = XGET(manager->preferences, MUIA_BrowserPreferences_RequestDownloadedFileName);
    if(requestFileName)
    {
	struct FileRequester *fr;

	struct TagItem frtags[] =
	{
	    { ASLFR_TitleText,     (IPTR)_(MSG_DownloadManager_RequestDownloadedFileTitle) },
	    { ASLFR_InitialDrawer, (IPTR)prefix                   },
	    { ASLFR_InitialFile,   (IPTR)filename                 },
	    { ASLFR_DoSaveMode,    (BOOL)TRUE                     },
	    { TAG_END                                             }
	};

	if((fr = (struct FileRequester *) AllocAslRequest(ASL_FileRequest, frtags)))
	{
	    if(AslRequest(fr, NULL))
	    {
		download->filename = StrDup(fr->fr_File);
		destination = AllocVec(strlen(fr->fr_Drawer) + strlen(fr->fr_File) + 2, MEMF_ANY);
		strcpy(destination, fr->fr_Drawer);
		AddPart(destination, fr->fr_File, strlen(fr->fr_Drawer) + strlen(fr->fr_File) + 2);
	    }
	    else
	    {
		destination = NULL;
	    }
	    FreeAslRequest(fr);
	}
	else
	{
	    destination = NULL;
	}
    }
    else
    {
	download->filename = StrDup(filename);
	destination = AllocVec(strlen(filename) + strlen(prefix) + 2, MEMF_ANY);
	strcpy(destination, prefix);
	AddPart(destination, filename, strlen(filename) + strlen(prefix) + 2);
    }

    D(bug("setting destination %s\n", destination));
    DoMethod(obj, MUIM_DownloadDelegate_SetDestination, download->identifier, destination);
    FreeVec(destination);

    if(destination == NULL)
    {
	// download is cancelled now, remove it
	int i;
	for(i = XGET(manager->list, MUIA_List_Entries) - 1; i>= 0; i--)
	{
	    struct Download *download_entry = NULL;
	    DoMethod(manager->list, MUIM_List_GetEntry, i, &download_entry);
	    if(download == download_entry)
	    {
		set(manager->list, MUIA_List_Active, i);
		DoMethod(_app(obj), MUIM_Application_PushMethod, manager->list, 3, MUIM_CallHook, &manager->cancelHook, NULL);
		break;
	    }
	}
    }

    DoMethod(manager->list, MUIM_List_Redraw, MUIV_List_Redraw_All);
    
    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadDelegate_DidReceiveExpectedContentLength(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DidReceiveExpectedContentLength* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);

    struct Download *download = GetDownload(manager, message->download);
    if(!download)
	return FALSE;
    
    download->sizeTotal = message->length;

    if(download->filesize)
	FreeVec(download->filesize);

    char *template;
    double scaledsize;
    
    if(download->sizeTotal < 1024)
    {
	template = "%.0lf b";
	scaledsize = download->sizeTotal;
    }
    else if(download->sizeTotal < 1024 * 1024)
    {
	template = "%.2lf kB";
	scaledsize = 1.0 * download->sizeTotal / 1024; 
    }
    else
    {
	template = "%.2lf MB";
	scaledsize = 1.0 * download->sizeTotal / (1024 * 1024);
    }
    
    char c;
    int length = snprintf(&c, 0, template, scaledsize) + 1;
    char *filesize = AllocVec(length, MEMF_ANY);
    if(filesize)
    {
	snprintf(filesize, length, template, scaledsize);
	download->filesize = filesize;

	DoMethod(manager->list, MUIM_List_Redraw, MUIV_List_Redraw_All);
    }
    
    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadDelegate_DidReceiveDataOfLength(Class *cl, Object *obj, struct  MUIP_DownloadDelegate_DidReceiveDataOfLength* message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);

    D(bug("DownloadProgressed(%d)\n", message->length));
    struct Download *download = GetDownload(manager, message->download);
    if(!download)
	return FALSE;

    if(download->state == DOWNLOAD_CREATED)
	download->state = DOWNLOAD_ACTIVE;

    download->sizeDownloaded += message->length;

    time_t currentTime = time(NULL);
    if(currentTime > download->speedMeasureTime)
    {
	long long downloaded = download->sizeDownloaded - download->speedSizeDownloaded;
	download->speedSizeDownloaded = download->sizeDownloaded;
	int elapsed = currentTime - download->speedMeasureTime;
	download->speedMeasureTime = currentTime;
	
	if(download->speed)
	    FreeVec(download->speed);
    
	char *template;
	double scaledspeed;
    
	if(downloaded < 1024)
	{
	    template = "%.0lf b/s";
	    scaledspeed = downloaded / elapsed;
	}
	else if(downloaded < 1024 * 1024)
	{
	    template = "%.2lf kB/s";
	    scaledspeed = 1.0 * downloaded / (1024 * elapsed); 
	}
	else
	{
	    template = "%.2lf MB/s";
	    scaledspeed = 1.0 * downloaded / (1024 * 1024 * elapsed);
	}
	
	char c;
	int length = snprintf(&c, 0, template, scaledspeed) + 1;
	char *transferspeed = AllocVec(length, MEMF_ANY);
	if(transferspeed)
	{
	    snprintf(transferspeed, length, template, scaledspeed);
	    download->speed = transferspeed;
	}
    }

    if(currentTime > download->listRedrawTime)
    {
	download->listRedrawTime = currentTime;
	sprintf(download->progress, "%ld %%", (long) (download->sizeTotal > 0 ? 100 * download->sizeDownloaded / download->sizeTotal : 0));
	DoMethod(manager->list, MUIM_List_Redraw, MUIV_List_Redraw_All);
    }
    
    return TRUE;
}

IPTR DownloadManager__MUIM_DownloadManager_UpdateInterface(Class *cl, Object *obj, Msg message)
{
    struct DownloadManager_DATA *manager = (struct DownloadManager_DATA *) INST_DATA(cl, obj);
    LONG active = XGET(manager->list, MUIA_List_Active);
    struct Download *download = NULL;
    
    if(active == MUIV_List_Active_Off)
    {
	set(manager->bt_cancel, MUIA_Disabled, TRUE);
	return TRUE;
    }
    
    DoMethod(manager->list, MUIM_List_GetEntry, active, &download);
    
    if(download && download->state == DOWNLOAD_ACTIVE)
	set(manager->bt_cancel, MUIA_Disabled, FALSE);
    else
	set(manager->bt_cancel, MUIA_Disabled, TRUE);
    
    return TRUE;
}

ZUNE_CUSTOMCLASS_9(
    DownloadManager, NULL, NULL, DownloadDelegate_CLASS,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    MUIM_DownloadDelegate_DidBeginDownload, struct  MUIP_DownloadDelegate_DidBeginDownload*,
    MUIM_DownloadDelegate_DidFinishDownload, struct  MUIP_DownloadDelegate_DidFinishDownload*,
    MUIM_DownloadDelegate_DidFailWithError, struct  MUIP_DownloadDelegate_DidFailWithError*,
    MUIM_DownloadDelegate_DidReceiveExpectedContentLength, struct  MUIP_DownloadDelegate_DidReceiveExpectedContentLength*,
    MUIM_DownloadDelegate_DidReceiveDataOfLength, struct  MUIP_DownloadDelegate_DidReceiveDataOfLength*,
    MUIM_DownloadDelegate_DecideDestinationWithSuggestedFilename, struct  MUIP_DownloadDelegate_DecideDestinationWithSuggestedFilename*,
    MUIM_DownloadManager_UpdateInterface, Msg
)
