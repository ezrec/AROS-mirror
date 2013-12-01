#ifndef  CLIB_PM_PROTOS_H
#define  CLIB_PM_PROTOS_H

/*
**	$Id$
**
**	C prototypes. For use with 32 bit integers only.
**
**	Copyright © 2001 Amiga, Inc.
**	    All Rights Reserved
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef  LIBRARIES_PM_H
#include <libraries/pm.h>
#endif
struct PopupMenu *PM_MakeMenuA( struct TagItem *tags );
struct PopupMenu *PM_MakeMenu( ULONG tags, ... );
struct PopupMenu *PM_MakeItemA( struct TagItem *tags );
struct PopupMenu *PM_MakeItem( ULONG tags, ... );
VOID PM_FreePopupMenu( struct PopupMenu *menu );
ULONG PM_OpenPopupMenuA( struct Window *wnd, struct TagItem *tags );
ULONG PM_OpenPopupMenu( struct Window *wnd, ULONG tags, ... );
struct PM_IDLst *PM_MakeIDListA( struct TagItem *tags );
struct PM_IDLst *PM_MakeIDList( ULONG tags, ... );
BOOL PM_ItemChecked( struct PopupMenu *pm, ULONG id );
LONG PM_GetItemAttrsA( struct PopupMenu *item, struct TagItem *tags );
LONG PM_GetItemAttrs( struct PopupMenu *item, ULONG tags, ... );
LONG PM_SetItemAttrsA( struct PopupMenu *item, struct TagItem *tags );
LONG PM_SetItemAttrs( struct PopupMenu *item, ULONG tags, ... );
struct PopupMenu *PM_FindItem( struct PopupMenu *menu, ULONG id );
VOID PM_AlterState( struct PopupMenu *menu, struct PM_IDLst *idlst, ULONG action );
struct PM_IDLst *PM_ExLstA( ULONG *id );
struct PM_IDLst *PM_ExLst( ULONG id, ... );
APTR PM_FilterIMsgA( struct Window *window, struct PopupMenu *menu, struct IntuiMessage *imsg, struct TagItem *tags );
APTR PM_FilterIMsg( struct Window *window, struct PopupMenu *menu, struct IntuiMessage *imsg, ULONG tags, ... );
LONG PM_InsertMenuItemA( struct PopupMenu *menu, struct TagItem *tags );
LONG PM_InsertMenuItem( struct PopupMenu *menu, ULONG tags, ... );
struct PopupMenu *PM_RemoveMenuItem( struct PopupMenu *menu, struct PopupMenu *item );
BOOL PM_AbortHook( APTR handle );
STRPTR PM_GetVersion( VOID );
VOID PM_ReloadPrefs( VOID );
LONG PM_LayoutMenuA( struct Window *window, struct PopupMenu *menu, struct TagItem *tags );
LONG PM_LayoutMenu( struct Window *window, struct PopupMenu *menu, ULONG tags, ... );
VOID PM_FreeIDList( struct PM_IDLst *list );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* CLIB_PM_PROTOS_H */
