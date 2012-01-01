/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#include <dos/dos.h>	/* For AmigaDOS error definitions */

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#ifdef DO_MENUS	/* Support code */

/****** gtlayout.library/LT_NewMenuTagList ******************************************
*
*   NAME
*	LT_NewMenuTagList -- Allocate and layout menu items (V11)
*
*   SYNOPSIS
*	Menu = LT_NewMenuTagList(Tags)
*	 D0                       A0
*
*	struct Menu *LT_NewMenuTagList(struct TagItem *);
*
*	struct Menu *LT_NewMenuTags(...);
*
*   FUNCTION
*	Allocates Menus and MenuItems similar to LT_LayoutMenus().
*
*	As of v18 this routine will validate menu mutual exclusion
*	information.
*
*   INPUTS
*	Tags - Pointer to a list of tagitem values, as found
*	    in gtlayout.h
*
*
*	Valid tags include:
*
*	LAMN_Screen (struct Screen *) - Pointer to the Screen
*	    the menu is to appear upon. This tag is mandatory,
*	    unless the LAMN_LayoutHandle tag is used.
*
*	LAMN_TextAttr (struct TextAttr *) - Pointer to the
*	    TextAttr to use for the menu layout. If this tag
*	    is omitted the Screen->Font will be used.
*
*	LAMN_Error (LONG *) - Pointer to variable to receive
*	    an error in case of failure.
*
*	LAMN_AmigaGlyph (struct Image *) - Pointer to Image to
*	    use as the Amiga glyph in menus. Will be ignored if
*	    NULL.
*
*	    NOTE: Ignored by intuition.library v37 and below.
*
*	LAMN_CheckmarkGlyph (struct Image *) - Pointer to Image to
*	    use as the checkmark glyph in menus. Will be ignored
*	    if NULL.
*
*	LAMN_LayoutHandle (LayoutHandle *) - Pointer to a valid
*	    LayoutHandle as created by LT_CreateHandle. This tag
*	    provides all the information the single tags
*	    LAMN_Screen..LAMN_CheckmarkGlyph would otherwise
*	    need to provide.
*
*	LAMN_TitleText (STRPTR) - Name of new menu to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_TitleID (LONG) - Locale ID corresponding to the
*	    name of the new menu to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_ItemText (STRPTR) - Name of new menu item to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_ItemID (LONG) - Locale ID corresponding to the
*	    name of the new menu item to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_SubText (STRPTR) - Name of new submenu item to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_SubID (LONG) - Locale ID corresponding to the
*	    name of the new submenu item to create.
*	    You may precede the name with the keyboard shortcut
*	    to assign to this menu item as follows:
*
*	       A\0Save as...
*
*	    This will create a menu item using the shortcut "A"
*	    and the title "Save as...".
*
*	LAMN_KeyText (STRPTR) - Pointer to the string whose first
*	    character will be used as the keyboard shortcut for
*	    this menu/submenu item.
*
*	LAMN_KeyID (LONG) - Locale ID corresponding to the string whose
*	    first character will be used as the keyboard shortcut for
*	    this menu/submenu item.
*
*	LAMN_CommandText (STRPTR) - Pointer to the string which
*	    will be used as the keyboard shortcut for this
*	    menu/submenu item.
*
*	LAMN_CommandID (LONG) - Locale ID corresponding to the string
*	    which will be used as the keyboard shortcut for
*	    this menu/submenu item.
*
*	LAMN_MutualExclude (ULONG) - Mutual exclusion information for
*	    this menu/submenu item.
*
*	LAMN_UserData (APTR) - User data information for this
*	    menu/menu item/submenu item.
*
*	LAMN_Disabled (BOOL) - Controls whether this
*	    menu/menu item/submenu item should be disabled.
*
*	LAMN_CheckIt (BOOL) - Controls whether this menu/submenu item
*	    should be prepared to hold a checkmark.
*
*	        NOTE: This does not set the checkmark, use LAMN_Checked
*	            for this purpose.
*
*	LAMN_Checked (BOOL) - Controls whether this menu/submenu item
*	    should be marked with a checkmark. This tag implies
*	    "LAMN_CheckIt,TRUE".
*
*	LAMN_Toggle (BOOL) - Controls whether this menu/submenu item
*	    should be prepared to hold a checkmark the user is to
*	    toggle on demand. This tag implies "LAMN_CheckIt,TRUE".
*
*	        NOTE: this does not set the checkmark, use LAMN_Checked
*	            for this purpose.
*
*	LAMN_Code (UWORD) - Raw key code to associate with this
*	    menu/submenu item. To find out if a rawkey event
*	    corresponds to this menu item use LT_FindMenuCommand.
*
*	LAMN_Qualifier (ULONG) - Key qualifier to associate
*	    with this menu/submenu item.
*
*	        NOTE: the comparison does not distinguish between
*	            the left and right shift/caps/alt qualifier keys.
*
*	LAMN_ID (ULONG) - Unique ID to associate with this
*	    menu/menu item/submenu item. You can use this
*	    later to look up data using LT_GetMenuItem, etc.
*
*	LAMN_ExtraSpace (UWORD) - Number of pixels to put between
*	    neighbouring menu titles. (V18)
*	    Default: 0 pixels.
*
*   RESULT
*	Menu - Pointer to Menu structure, ready to pass to
*	       SetMenuStrip(), NULL on failure.
*
*   EXAMPLE
*	The following tagitem list:
*
*	    LAMN_TitleText,     "Project"
*	     LAMN_ItemText,     "New",
*	      LAMN_KeyText,     "N",
*	     LAMN_ItemText,     "Open...",
*	      LAMN_KeyText,     "O",
*	     LAMN_ItemText,     NM_BARLABEL,
*	     LAMN_ItemText,     "Save",
*	      LAMN_KeyText,     "S",
*	     LAMN_ItemText,     "A\0Save As...",
*	     LAMN_ItemText,     NM_BARLABEL,
*	     LAMN_ItemText,     "Print...",
*	      LAMN_KeyText,     "P",
*	     LAMN_ItemText,     NM_BARLABEL,
*	     LAMN_ItemText,     "Help...",
*	      LAMN_CommandText, "[Help]",
*	     LAMN_ItemText,     NM_BARLABEL,
*	     LAMN_ItemText,     "Quit...",
*	      LAMN_KeyText,     "Q",
*	    TAG_DONE
*
*	Will create the following menu:
*
*	+-------+
*	|Project|
*	+-------+------+
*	|New         aN|
*	|Open...     aO|
*	|~~~~~~~~~~~~~~|
*	|Save        aS|
*	|Save As...  aA|
*	|~~~~~~~~~~~~~~|
*	|Print...    aP|
*	|~~~~~~~~~~~~~~|
*	|Help... [Help]|
*	|~~~~~~~~~~~~~~|
*	|Quit...     aQ|
*	+--------------+
*
*   NOTES
*	You may freely add, remove, spindle & mutilate the contents of the
*	menu strip created, just don't trash or disconnect the base menu
*	entry this routine creates as all menu memory tracking data is
*	connected to it.
*
*   SEE ALSO
*	gtlayout.library/LT_DisposeMenu
*	gtlayout.library/LT_FindCommandItem
*	gtlayout.library/LT_GetMenuItem
*	gtlayout.library/LT_LayoutMenuA
*	gtlayout.library/LT_MenuControlTagList
*	gtlayout.library/LT_NewMenuTemplate
*	intuition.library/SetMenuStrip
*
******************************************************************************
*
*/

struct Menu * LIBENT
LT_NewMenuTagList(REG(a0) struct TagItem *TagList)
{
	RootMenu		*Root;
	struct Image	*AmigaGlyph	= NULL,
					*CheckGlyph = NULL;
	LONG			*ErrorPtr = NULL,
					 Error;
	struct Screen	*Screen = NULL;
	struct TextAttr	*TextAttr = NULL;
	struct TagItem	*List;
	struct TagItem	*Entry;
	LayoutHandle	*Handle = NULL;
	struct Hook		*LocaleHook = NULL;
	LONG			 ExtraSpace = 0;

	List = TagList;

	while(Entry = NextTagItem(&List))
	{
		switch(Entry->ti_Tag)
		{
			case LAMN_AmigaGlyph:

				AmigaGlyph = (struct Image *)Entry->ti_Data;
				break;

			case LAMN_CheckmarkGlyph:

				CheckGlyph = (struct Image *)Entry->ti_Data;
				break;

			case LAMN_Error:

				ErrorPtr = (LONG *)Entry->ti_Data;
				break;

			case LAMN_Screen:

				Screen = (struct Screen *)Entry->ti_Data;
				break;

			case LAMN_TextAttr:

				TextAttr = (struct TextAttr *)Entry->ti_Data;
				break;

			case LAMN_Handle:

				Handle		= (LayoutHandle *)Entry->ti_Data;
				Screen		= Handle->Screen;
				TextAttr	= Handle->InitialTextAttr;
				AmigaGlyph	= Handle->AmigaGlyph;
				CheckGlyph	= Handle->CheckGlyph;
				break;

			case LAMN_ExtraSpace:

				ExtraSpace = Entry->ti_Data;
				break;

			case LH_LocaleHook:

				LocaleHook = (struct Hook *)Entry->ti_Data;
				break;
		}
	}

	if(Handle)
		LocaleHook = Handle->LocaleHook;

	if(ErrorPtr)
		*ErrorPtr = 0;

	if(Root = LTP_NewMenu(Screen,TextAttr,AmigaGlyph,CheckGlyph,&Error))
	{
		Root->Handle		= Handle;
		Root->LocaleHook	= LocaleHook;

			// Create the menu

		if(LTP_CreateMenuTagList(Root,&Error,TagList))
		{
				// Do the layout

			if(LTP_LayoutMenu(Root,0,ExtraSpace))
				return(&Root->Menu);
			else
				Error = ERROR_DISK_FULL;
		}

		LT_DisposeMenu(&Root->Menu);
	}

	if(ErrorPtr)
		*ErrorPtr = Error;

	return(NULL);
}


/*****************************************************************************/


#ifndef __AROS__
struct Menu *
LT_NewMenuTags(Tag FirstTag,...)
{
	return(LT_NewMenuTagList((struct TagItem *)&FirstTag));
}
#endif

#endif	/* DO_MENUS */
