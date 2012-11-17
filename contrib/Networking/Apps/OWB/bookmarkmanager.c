/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>
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
#include <sqlite3.h>

#include "bookmarkmanager.h"
#include "bookmarkmanager_private.h"

#include "locale.h"
#include "browserapp.h"

static IPTR BookmarkDisplayFunc(struct Hook *hook, char **columns, struct Bookmark *bookmark)
{
    if(bookmark == NULL)
    {
	columns[0] = (char*) _(MSG_BookmarkManager_Label);
	columns[1] = (char*) _(MSG_BookmarkManager_URL);
    }
    else
    {
        columns[0] = bookmark->label;
        columns[1] = bookmark->url;
    }
    D(bug("displaying %s | %s\n", columns[0], columns[1]));
    return 1;
}

static IPTR BookmarkDestroyFunc(struct Hook *hook, APTR pool, struct Bookmark *bookmark)
{
    if(!bookmark)
	return 0;

    FreeVec(bookmark->label);
    FreeVec(bookmark->url);
    FreeVec(bookmark);
    
    return 0;
}

static IPTR BookmarkCreateFunc(struct Hook *hook, APTR pool, struct Bookmark *bookmark)
{
    return (IPTR) bookmark;
}

/*** Methods ****************************************************************/
IPTR BookmarkManager__OM_NEW(Class *CLASS, Object *self, struct opSet *message)
{
    struct BookmarkManager_DATA tmpdata; 
    struct BookmarkManager_DATA *data = NULL; 
    struct TagItem *tag = NULL, *tstate = message->ops_AttrList;
    Object *insert_bt, *remove_bt, *change_bt, *up_bt, *down_bt;
    
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
   
    tmpdata.create_hook.h_Entry = HookEntry;
    tmpdata.create_hook.h_SubEntry = (HOOKFUNC) BookmarkCreateFunc;
    tmpdata.destroy_hook.h_Entry = HookEntry;
    tmpdata.destroy_hook.h_SubEntry = (HOOKFUNC) BookmarkDestroyFunc;
    tmpdata.display_hook.h_Entry = HookEntry;
    tmpdata.display_hook.h_SubEntry = (HOOKFUNC) BookmarkDisplayFunc;
    
    tmpdata.selectedURL = StrDup("");

    self = (Object *) DoSuperNewTags
    (
        CLASS, self, NULL,
        MUIA_Window_Open, FALSE,
        MUIA_Window_Title, (IPTR)_(MSG_BookmarkManager_Title),
        WindowContents, (IPTR)(VGroup,
            MUIA_InnerLeft, 5,
            MUIA_InnerRight, 5,
            MUIA_InnerTop, 5,
            MUIA_InnerBottom, 5,
	    Child, (IPTR)(VGroup,
	        Child, (IPTR)(ListviewObject,
	            MUIA_Listview_List, (IPTR)(tmpdata.bookmarks_list = ListObject,
			MUIA_CycleChain, 1,
                        MUIA_List_Format, (IPTR)"BAR,",
                        MUIA_List_Title, TRUE,
                        End),
                    End),
                End),
	    Child, (IPTR)(VGroup,
		Child, (IPTR)(ColGroup(2),
		    Child, (IPTR)(Label2(_(MSG_BookmarkManager_Label))),
		    Child, (IPTR)(tmpdata.label_string = StringObject,
			MUIA_Frame, MUIV_Frame_String,
			MUIA_CycleChain, 1,
			MUIA_String_MaxLen, 256,
			End),
		    Child, (IPTR)(Label2(_(MSG_BookmarkManager_URL))),
		    Child, (IPTR)(tmpdata.url_string = StringObject,
			MUIA_Frame, MUIV_Frame_String,
			MUIA_CycleChain, 1,
			MUIA_String_MaxLen, 2048,
			End),
		    End),
		End),
	    Child, (IPTR)(HGroup,
		Child, (IPTR)(insert_bt = SimpleButton(_(MSG_BookmarkManager_Insert))),
		Child, (IPTR)(change_bt = SimpleButton(_(MSG_BookmarkManager_Change))),
		Child, (IPTR)(remove_bt = SimpleButton(_(MSG_BookmarkManager_Remove))),
		Child, (IPTR)(up_bt = SimpleButton(_(MSG_BookmarkManager_MoveUp))),
		Child, (IPTR)(down_bt = SimpleButton(_(MSG_BookmarkManager_MoveDown))),
		End),
            End),
        TAG_MORE, (IPTR) message->ops_AttrList
    );
    
    if (self == NULL)
	return (IPTR) NULL;

    tmpdata.bookmarks_menu = MenuObject,
        MUIA_Menu_Title, (IPTR)_(MSG_BookmarkManager_MenuTitle),
        End;
    
    /* Click Close window with close gadget */
    DoMethod(self, MUIM_Notify, MUIA_Window_CloseRequest, (IPTR) TRUE,
        (IPTR) self,  (IPTR) 3,
        MUIM_Set, MUIA_Window_Open, (IPTR) FALSE);

    DoMethod(insert_bt, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 1,
	MUIM_BookmarkManager_Insert);

    DoMethod(change_bt, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 1,
	MUIM_BookmarkManager_Change);

    DoMethod(remove_bt, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 1,
	MUIM_BookmarkManager_Remove);

    DoMethod(up_bt, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 2,
	MUIM_BookmarkManager_Move, MUIV_BookmarkManager_MoveUp);

    DoMethod(down_bt, MUIM_Notify, MUIA_Pressed, FALSE,
	(IPTR) self, 2,
	MUIM_BookmarkManager_Move, MUIV_BookmarkManager_MoveDown);

    DoMethod(tmpdata.bookmarks_list, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
	self, 1,
	MUIM_BookmarkManager_Display);
    
    data = INST_DATA(CLASS, self);
    *data = tmpdata;
    
    set(data->bookmarks_list, MUIA_List_DisplayHook, &data->display_hook);
    set(data->bookmarks_list, MUIA_List_DestructHook, &data->destroy_hook);

    if(sqlite3_open("PROGDIR:bookmarks.db", &data->db) && sqlite3_open(":memory:", &data->db))
    {
	CoerceMethod(CLASS, self, OM_DISPOSE);
	return (IPTR) NULL;
    }
    else
    {
	sqlite3_exec(data->db, "CREATE TABLE IF NOT EXISTS bookmarks (id INTEGER PRIMARY KEY, parent INTEGER DEFAULT NULL, label VARCHAR NOT NULL, url VARCHAR, isCategory BOOL DEFAULT FALSE, place INTEGER NOT NULL)", NULL, NULL, NULL);
	sqlite3_stmt *stmt = NULL;
	sqlite3_prepare(data->db, "SELECT id, label, url, place FROM bookmarks ORDER BY place ASC", -1, &stmt, NULL);
	while(sqlite3_step(stmt) == SQLITE_ROW)
	{
	    struct Bookmark *bookmark = AllocVec(sizeof(struct Bookmark), MEMF_ANY | MEMF_CLEAR);
	    if(!bookmark)
		break;
	    
	    bookmark->id = sqlite3_column_int64(stmt, 0);
	    bookmark->label = StrDup((STRPTR) sqlite3_column_text(stmt, 1));
	    bookmark->url = StrDup((STRPTR) sqlite3_column_text(stmt, 2));
	    
	    DoMethod(data->bookmarks_list, MUIM_List_InsertSingle, bookmark, MUIV_List_Insert_Bottom);
	    bookmark->menuItem = MenuitemObject,
                MUIA_Menuitem_Title, (IPTR)bookmark->label,
                End;
	    DoMethod(data->bookmarks_menu, MUIM_Family_AddTail, bookmark->menuItem);

	    DoMethod(bookmark->menuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
		(IPTR) self, 3,
		MUIM_Set, MUIA_BookmarkManager_SelectedURL, bookmark->url);
	}
	
	sqlite3_finalize(stmt);
    }
    
    return (IPTR) self;
}

IPTR BookmarkManager__OM_DISPOSE(struct IClass *cl, Object *obj, Msg msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    
    sqlite3_close(data->db);
    
    return DoSuperMethodA(cl,obj,msg);
}

IPTR BookmarkManager__MUIM_BookmarkManager_Insert(struct IClass *cl, Object *obj, Msg msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    sqlite3_stmt *stmt = NULL;
    struct Bookmark *bookmark;
    
    bookmark = AllocVec(sizeof(struct Bookmark), MEMF_ANY | MEMF_CLEAR);
    if(!bookmark)
        goto error;
    
    bookmark->label = StrDup((STRPTR) XGET(data->label_string, MUIA_String_Contents));
    bookmark->url = StrDup((STRPTR) XGET(data->url_string, MUIA_String_Contents));
    
    D(bug("preparing insert statement\n"));
    if(sqlite3_prepare(data->db, "INSERT INTO bookmarks (id, label, url, place) VALUES (NULL, ?1, ?2, (SELECT ifnull(max(place)+1,0) FROM bookmarks))", -1, &stmt, NULL) != SQLITE_OK)
	goto database_error;
    
    D(bug("binding values %s %s\n", bookmark->label, bookmark->url));
    if(
	sqlite3_bind_text(stmt, 1, bookmark->label, -1, SQLITE_STATIC) != SQLITE_OK ||
	sqlite3_bind_text(stmt, 2, bookmark->url, -1, SQLITE_STATIC) != SQLITE_OK
    )
	goto database_error;
	    
    D(bug("executing statement\n"));
    if(sqlite3_step(stmt) != SQLITE_DONE)
	goto database_error;
    
    bookmark->id = sqlite3_last_insert_rowid(data->db);

    D(bug("finalizing statement\n"));
    sqlite3_finalize(stmt);
    stmt = NULL;
    
    D(bug("inserting entry %p into list\n", bookmark));
    
    DoMethod(data->bookmarks_list, MUIM_List_InsertSingle, bookmark, MUIV_List_Insert_Bottom);
    
    /* Create bookmark menu item */
    bookmark->menuItem = MenuitemObject,
	MUIA_Menuitem_Title, (IPTR)bookmark->label,
	End;
    DoMethod(data->bookmarks_menu, MUIM_Family_AddTail, bookmark->menuItem);

    DoMethod(bookmark->menuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	(IPTR) obj, 3,
	MUIM_Set, MUIA_BookmarkManager_SelectedURL, bookmark->url);
	
    set(data->label_string, MUIA_String_Contents, "");
    set(data->url_string, MUIA_String_Contents, "");

    DoMethod(_app(obj), MUIM_Application_UpdateMenus);

    return (IPTR) 0;

database_error:
    D(bug("Database error: %s\n", sqlite3_errmsg(data->db)));
error:
    if(bookmark)
    {
	if(bookmark->label)
	    FreeVec(bookmark->label);
	if(bookmark->url)
	    FreeVec(bookmark->url);
	FreeVec(bookmark);
    }
    if(stmt)
	sqlite3_finalize(stmt);
    
    return (IPTR) 1;
}

IPTR BookmarkManager__MUIM_BookmarkManager_Change(struct IClass *cl, Object *obj, Msg msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    sqlite3_stmt *stmt = NULL;

    int active = XGET(data->bookmarks_list, MUIA_List_Active);
    if(active == MUIV_List_Active_Off)
	return (IPTR) 1;
    
    struct Bookmark *bookmark = NULL;
    DoMethod(data->bookmarks_list, MUIM_List_GetEntry, active, &bookmark);

    FreeVec(bookmark->label);
    bookmark->label = StrDup((STRPTR) XGET(data->label_string, MUIA_String_Contents));
    FreeVec(bookmark->url);
    bookmark->url = StrDup((STRPTR) XGET(data->url_string, MUIA_String_Contents));

    D(bug("preparing update statement\n"));
    if(sqlite3_prepare(data->db, "UPDATE bookmarks SET label = ?1, url = ?2 WHERE id = ?5", -1, &stmt, NULL) != SQLITE_OK)
	goto database_error;
    
    D(bug("binding values %s %s, %ld\n", bookmark->label, bookmark->url, bookmark->id));
    if(
	sqlite3_bind_text(stmt, 1, bookmark->label, -1, SQLITE_STATIC) != SQLITE_OK ||
	sqlite3_bind_text(stmt, 2, bookmark->url, -1, SQLITE_STATIC) != SQLITE_OK ||
	sqlite3_bind_int64(stmt, 5, bookmark->id) != SQLITE_OK
    )
	goto database_error;
	    
    D(bug("executing statement\n"));
    if(sqlite3_step(stmt) != SQLITE_DONE)
	goto database_error;

    D(bug("finalizing statement\n"));
    sqlite3_finalize(stmt);

    DoMethod(data->bookmarks_list, MUIM_List_Redraw, active);
    
    /* Update menu item */
    set(bookmark->menuItem, MUIA_Menuitem_Title, bookmark->label);
    
    DoMethod(bookmark->menuItem, MUIM_KillNotify, MUIA_Menuitem_Trigger);
    DoMethod(bookmark->menuItem, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
	(IPTR) obj, 3,
	MUIM_Set, MUIA_BookmarkManager_SelectedURL, bookmark->url);

    DoMethod(_app(obj), MUIM_Application_UpdateMenus);

    return (IPTR) 0;
database_error:
    D(bug("Database error: %s\n", sqlite3_errmsg(data->db)));
    if(stmt)
	sqlite3_finalize(stmt);
    
    return (IPTR) 1;
}

IPTR BookmarkManager__MUIM_BookmarkManager_Remove(struct IClass *cl, Object *obj, Msg msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);

    sqlite3_stmt *stmt = NULL;

    int active = XGET(data->bookmarks_list, MUIA_List_Active);
    if(active == MUIV_List_Active_Off)
	return (IPTR) 1;
    
    struct Bookmark *bookmark = NULL;
    DoMethod(data->bookmarks_list, MUIM_List_GetEntry, active, &bookmark);

    D(bug("preparing update statement\n"));
    if(sqlite3_prepare(data->db, "UPDATE bookmarks SET place = place - 1 WHERE place > (SELECT place FROM bookmarks WHERE id = ?1)", -1, &stmt, NULL) != SQLITE_OK)
	goto database_error;

    D(bug("binding values %ld\n", bookmark->id));
    if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	goto database_error;

    D(bug("executing statement\n"));
    if(sqlite3_step(stmt) != SQLITE_DONE)
	goto database_error;

    D(bug("finalizing statement\n"));
    sqlite3_finalize(stmt);
    stmt = NULL;
	
    D(bug("preparing delete statement\n"));
    if(sqlite3_prepare(data->db, "DELETE FROM bookmarks WHERE id = ?1", -1, &stmt, NULL) != SQLITE_OK)
	goto database_error;
    
    D(bug("binding values %ld\n", bookmark->id));
    if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	goto database_error;
	    
    D(bug("executing statement\n"));
    if(sqlite3_step(stmt) != SQLITE_DONE)
	goto database_error;

    D(bug("finalizing statement\n"));
    sqlite3_finalize(stmt);

    DoMethod(data->bookmarks_menu, MUIM_Family_Remove, bookmark->menuItem);

    DoMethod(data->bookmarks_list, MUIM_List_Remove, active);

    DoMethod(_app(obj), MUIM_Application_UpdateMenus);

    return (IPTR) 0;

database_error:
    D(bug("Database error: %s\n", sqlite3_errmsg(data->db)));
    if(stmt)
	sqlite3_finalize(stmt);
    
    return (IPTR) 1;
}

IPTR BookmarkManager__MUIM_BookmarkManager_Display(struct IClass *cl, Object *obj, Msg msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);

    int active = XGET(data->bookmarks_list, MUIA_List_Active);
    if(active == MUIV_List_Active_Off)
	return (IPTR) 1;
    
    struct Bookmark *bookmark = NULL;
    DoMethod(data->bookmarks_list, MUIM_List_GetEntry, active, &bookmark);

    set(data->label_string, MUIA_String_Contents, bookmark->label);
    set(data->url_string, MUIA_String_Contents, bookmark->url);
    
    return (IPTR) 0;
}

IPTR BookmarkManager__OM_GET(Class *cl, Object *obj, struct opGet *msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    IPTR retval = TRUE;

    switch(msg->opg_AttrID)
    {
        case MUIA_BookmarkManager_BookmarkMenu:
            *(Object**)msg->opg_Storage = data->bookmarks_menu;
            break;
        case MUIA_BookmarkManager_SelectedURL:
            *(STRPTR*)msg->opg_Storage = data->selectedURL;
            break;
    	default:
	    retval = DoSuperMethodA(cl, obj, (Msg) msg);
	    break;
    }
    
    return retval;
}

IPTR BookmarkManager__OM_SET(Class *cl, Object *obj, struct opSet *msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    struct TagItem *tags  = msg->ops_AttrList;
    struct TagItem *tag;

    while ((tag = NextTagItem(&tags)) != NULL)
    {
    	switch(tag->ti_Tag)
	{
            case MUIA_BookmarkManager_SelectedURL:
        	if(data->selectedURL)
        	    FreeVec(data->selectedURL);
        	data->selectedURL = StrDup((STRPTR) tag->ti_Data);
        	break;
	} /* switch(tag->ti_Tag) */
	
    } /* while ((tag = NextTagItem(&tags)) != NULL) */
    
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR BookmarkManager__MUIM_BookmarkManager_Move(struct IClass *cl, Object *obj, struct MUIP_BookmarkManager_Move *msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);
    Object *cstate, *child, *prev = NULL, *prevprev = NULL, *next = NULL;
    struct MinList *ChildList = NULL;
    sqlite3_stmt *stmt = NULL;

    int active = XGET(data->bookmarks_list, MUIA_List_Active);
    D(bug("active: %d\n", active));
    if(active == MUIV_List_Active_Off)
	return (IPTR) 1;

    struct Bookmark *bookmark = NULL;
    DoMethod(data->bookmarks_list, MUIM_List_GetEntry, active, &bookmark);

    int length = XGET(data->bookmarks_list, MUIA_List_Entries);

    get(data->bookmarks_menu, MUIA_Family_List, &ChildList);
    cstate = (Object*) ChildList->mlh_Head;
    switch(msg->position)
    {
    case MUIV_BookmarkManager_MoveUp:
	D(bug("MoveUp\n"));
	if(active == 0)
	    goto error;

	D(bug("preparing update statement\n"));
	if(sqlite3_prepare(data->db, "UPDATE bookmarks SET place = place + 1 WHERE place = (SELECT place FROM bookmarks WHERE id = ?1) - 1", -1, &stmt, NULL) != SQLITE_OK)
	    goto database_error;
	D(bug("binding values %ld\n", bookmark->id));
	if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	    goto database_error;
	D(bug("executing statement\n"));
	if(sqlite3_step(stmt) != SQLITE_DONE)
	    goto database_error;
	D(bug("finalizing statement\n"));
	sqlite3_finalize(stmt);
	stmt = NULL;
	D(bug("preparing update statement\n"));
	if(sqlite3_prepare(data->db, "UPDATE bookmarks SET place = place - 1 WHERE id = ?1", -1, &stmt, NULL) != SQLITE_OK)
	    goto database_error;
	D(bug("binding values %ld\n", bookmark->id));
	if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	    goto database_error;
	D(bug("executing statement\n"));
	if(sqlite3_step(stmt) != SQLITE_DONE)
	    goto database_error;
	D(bug("finalizing statement\n"));
	sqlite3_finalize(stmt);
	stmt = NULL;
	
	DoMethod(data->bookmarks_list, MUIM_List_Move, active, MUIV_List_Move_Previous);
	set(data->bookmarks_list, MUIA_List_Active, MUIV_List_Active_Up);
	while ((child = NextObject(&cstate)))
	{
	    if (child == bookmark->menuItem)
		break;
	    prevprev = prev;
	    prev = child;
	}
	DoMethod(data->bookmarks_menu, MUIM_Family_Remove, bookmark->menuItem);
	if (prevprev)
	    DoMethod(data->bookmarks_menu, MUIM_Family_Insert, bookmark->menuItem, prevprev);
	else
	    DoMethod(data->bookmarks_menu, MUIM_Family_AddHead, bookmark->menuItem);
	break;
    case MUIV_BookmarkManager_MoveDown:
	D(bug("MoveDown\n"));
	if(active == length - 1)
	    goto error;

	D(bug("preparing update statement\n"));
	if(sqlite3_prepare(data->db, "UPDATE bookmarks SET place = place - 1 WHERE place = (SELECT place FROM bookmarks WHERE id = ?1) + 1", -1, &stmt, NULL) != SQLITE_OK)
	    goto database_error;
	D(bug("binding values %ld\n", bookmark->id));
	if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	    goto database_error;
	D(bug("executing statement\n"));
	if(sqlite3_step(stmt) != SQLITE_DONE)
	    goto database_error;
	D(bug("finalizing statement\n"));
	sqlite3_finalize(stmt);
	stmt = NULL;
	D(bug("preparing update statement\n"));
	if(sqlite3_prepare(data->db, "UPDATE bookmarks SET place = place + 1 WHERE id = ?1", -1, &stmt, NULL) != SQLITE_OK)
	    goto database_error;
	D(bug("binding values %ld\n", bookmark->id));
	if(sqlite3_bind_int64(stmt, 1, bookmark->id) != SQLITE_OK)
	    goto database_error;
	D(bug("executing statement\n"));
	if(sqlite3_step(stmt) != SQLITE_DONE)
	    goto database_error;
	D(bug("finalizing statement\n"));
	sqlite3_finalize(stmt);
	stmt = NULL;
	
	DoMethod(data->bookmarks_list, MUIM_List_Move, active, MUIV_List_Move_Next);
	set(data->bookmarks_list, MUIA_List_Active, MUIV_List_Active_Down);
	while ((child = NextObject(&cstate)))
	{
	    if (child == bookmark->menuItem)
	    {
		next = NextObject(&cstate);
		break;
	    }
	}
	DoMethod(data->bookmarks_menu, MUIM_Family_Remove, bookmark->menuItem);
	if (next)
	    DoMethod(data->bookmarks_menu, MUIM_Family_Insert, bookmark->menuItem, next);
	else
	    DoMethod(data->bookmarks_menu, MUIM_Family_AddTail, bookmark->menuItem);
	break;
    default:
	break;
    }
    
    DoMethod(_app(obj), MUIM_Application_UpdateMenus);
    
    return (IPTR) 0;

database_error:
    D(bug("Database error: %s\n", sqlite3_errmsg(data->db)));
error:
    if(stmt)
	sqlite3_finalize(stmt);
    return (IPTR) 1;
}

IPTR BookmarkManager__MUIM_BookmarkManager_Bookmark(struct IClass *cl, Object *obj, struct MUIP_BookmarkManager_Bookmark *msg)
{
    struct BookmarkManager_DATA *data = (struct BookmarkManager_DATA *) INST_DATA(cl, obj);

    set(data->label_string, MUIA_String_Contents, msg->label);
    set(data->url_string, MUIA_String_Contents, msg->url);
    set(obj, MUIA_Window_Open, (IPTR) TRUE);
    DoMethod(obj, MUIM_Window_ToFront);

    return TRUE;
}

/*** Setup ******************************************************************/
ZUNE_CUSTOMCLASS_10
(
    BookmarkManager, NULL, MUIC_Window, NULL,
    OM_NEW, struct opSet*,
    OM_DISPOSE, Msg,
    OM_SET, struct opSet*,
    OM_GET, struct opGet*,
    MUIM_BookmarkManager_Insert, Msg,
    MUIM_BookmarkManager_Change, Msg,
    MUIM_BookmarkManager_Remove, Msg,
    MUIM_BookmarkManager_Display, Msg,
    MUIM_BookmarkManager_Move, struct MUIP_BookmarkManager_Move*,
    MUIM_BookmarkManager_Bookmark, struct MUIP_BookmarkManager_Bookmark*
)
