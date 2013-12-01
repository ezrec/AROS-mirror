//
// pmfind.h
//
// PopupMenu Library - Menu item searching
//
// Copyright (C)2000 Henrik Isaksson <henrik@boing.nu>
// All Rights Reserved.
//

#ifndef PM_FIND_H
#define PM_FIND_H

struct PM_Window;

#ifndef LIBRARIES_POPUPMENU_H
struct PopupMenu;
#endif /* LIBRARIES_POPUPMENU_H */

// Find a selectable item in a menu, no submenu recursion, but group recursion.
struct PopupMenu *PM_FindNextSelectable(struct PM_Window *a, struct PopupMenu *pm, BOOL *found);
struct PopupMenu *PM_FindFirstSelectable(struct PopupMenu *pm);
struct PopupMenu *PM_FindPrevSelectable(struct PM_Window *a, struct PopupMenu *pm, BOOL *found);
struct PopupMenu *PM_FindLastSelectable(struct PopupMenu *pm);

// Find item 'ID' in menu 'base'. Recursive.
struct PopupMenu *PM_FindID(struct PopupMenu *base, ULONG ID);

// Find last item in a menu. Recursive.
struct PopupMenu *PM_FindLast(struct PopupMenu *base);

// Find item with command key 'key' in menu 'base'. Recursive.
struct PopupMenu *PM_FindItemCommKey(struct PopupMenu *base, UBYTE key);

// Find item before item 'ID' in menu 'base'. Recursive.
struct PopupMenu *PM_FindBeforeID(struct PopupMenu *base, ULONG ID);

// Find item before item 'item' :) in menu 'base'. Recursive.
struct PopupMenu *PM_FindBefore(struct PopupMenu *base, struct PopupMenu *item);

LIBFUNC_P3_PROTO(BOOL, LIBPM_ItemChecked,
	A1, struct PopupMenu *, pm,
	D1, ULONG, ID,
	A6, struct PopupMenuBase *, l);

// Exported PM_FindID
LIBFUNC_P3_PROTO(struct PopupMenu *, LIBPM_FindItem,
	A1, struct PopupMenu *, menu,
	D1, ULONG, ID,
	A6, struct PopupMenuBase *, l);
    
struct PopupMenu *PM_FindSortedInsertPoint(struct PopupMenu *pm, struct PopupMenu *fm);

#endif /* PM_FIND_H */
