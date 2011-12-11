/*
**	Menus.c
**
**	Routines that deal with pull-down-menu management
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* PrepareLocalizedMenu(struct NewMenu **NewMenuPtr,WORD *NumMenuEntryPtr):
	 *
	 *	Set up the main menu.
	 */

VOID
PrepareLocalizedMenu(struct NewMenu **NewMenuPtr,WORD *NumMenuEntryPtr)
{
	struct LocaleNewMenu
	{
		UBYTE	lnm_Type;			/* Menu template type. */
		LONG	lnm_LabelID;		/* Menu's label */
		STRPTR	lnm_CommKey;		/* MenuItem Command Key Equiv */
		UWORD	lnm_Flags;			/* Menu or MenuItem flags (see note) */
		LONG	lnm_MutualExclude;	/* MenuItem MutualExclude word */
		ULONG	lnm_UserData;		/* For your own use, see note */
	};

	#define LNM_BARLABEL -1

	STATIC struct LocaleNewMenu LocalNewMenu[] =
	{
		{ NM_TITLE, MSG_TERMDATA_PROJECT_MEN },
		{  NM_ITEM, MSG_TERMDATA_SAVE_SCREEN_AS_MEN },
		{   NM_SUB, MSG_TERMDATA_SAVE_AS_PICTURE_MEN,		NULL,	0,	0,	MEN_SAVE_AS_PICTURE },
		{   NM_SUB, MSG_TERMDATA_SAVE_AS_TEXT_MEN,			NULL,	0,	0,	MEN_SAVE_AS_TEXT },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_PRINT_MEN },
		{   NM_SUB, MSG_TERMDATA_PRINT_SCREEN_MEN,			NULL,	0,	0,	MEN_PRINT_SCREEN },
		{   NM_SUB, MSG_TERMDATA_PRINT_SCREEN_GFX_MEN,		NULL,	0,	0,	MEN_PRINT_SCREEN_AS_GFX },
		{   NM_SUB, MSG_TERMDATA_PRINT_CLIP_MEN,			NULL,	0,	0,	MEN_PRINT_CLIP },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_CAPTURE_MEN },
		{   NM_SUB, MSG_TERMDATA_CAPTURE_TO_FILE_MEN,		NULL,	TICK,	0,	MEN_CAPTURE_TO_FILE },
		{   NM_SUB, MSG_TERMDATA_CAPTURE_TO_RAW_FILE_MEN,	NULL,	TICK,	0,	MEN_CAPTURE_TO_RAW_FILE },
		{   NM_SUB, MSG_TERMDATA_CAPTURE_TO_PRINTER_MEN,	NULL,	TICK,	0,	MEN_CAPTURE_TO_PRINTER },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_ICONIFY_MEN,				NULL,	0,	0,	MEN_ICONIFY },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_ABOUT_MEN,					NULL,	0,	0,	MEN_ABOUT },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_QUIT_MEN,					NULL,	0,	0,	MEN_QUIT },

		{ NM_TITLE, MSG_TERMDATA_EDIT_MEN },
		{  NM_ITEM, MSG_TERMDATA_COPY_MEN,					NULL,	0,	0,	MEN_COPY },
		{  NM_ITEM, MSG_TERMDATA_PASTE_MEN,					NULL,	0,	0,	MEN_PASTE },
		{  NM_ITEM, MSG_TERMDATA_CLEAR_MEN,					NULL,	0,	0,	MEN_CLEAR },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_SELECT_ALL_MEN,						NULL,	0,	0,	MEN_SELECT_ALL },

		{ NM_TITLE, MSG_TERMDATA_COMMANDS_MEN },
		{  NM_ITEM, MSG_TERMDATA_EXECUTE_DOS_COMMAND_MEN,	NULL,	0,	0,	MEN_EXECUTE_DOS_COMMAND },
		{  NM_ITEM, MSG_TERMDATA_EXECUTE_REXX_COMMAND_MEN,	NULL,	0,	0,	MEN_EXECUTE_REXX_COMMAND },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_RECORD_MEN,				NULL,	TICK,	0,	MEN_RECORD },
		{  NM_ITEM, MSG_TERMDATA_RECORD_LINE_MEN,			NULL,	TICK,	0,	MEN_RECORD_LINE },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_EDIT_TRAPS_MEN,			NULL,	0,	0,	MEN_EDIT_TRAPS },
		{  NM_ITEM, MSG_TERMDATA_DISABLE_TRAPS_MEN,			NULL,	TICK,	0,	MEN_DISABLE_TRAPS },

		{ NM_TITLE, MSG_TERMDATA_PHONE_MEN },
		{  NM_ITEM, MSG_TERMDATA_PHONEBOOK_MEN,				NULL,	0,	0,	MEN_PHONEBOOK },
		{  NM_ITEM, MSG_TERMDATA_REDIAL_MEN,				NULL,	0,	0,	MEN_REDIAL },
		{  NM_ITEM, MSG_TERMDATA_DIAL_NUMBER_MEN,			NULL,	0,	0,	MEN_DIAL_NUMBER },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_SEND_BREAK_MEN,			NULL,	0,	0,	MEN_SEND_BREAK },
		{  NM_ITEM, MSG_TERMDATA_HANG_UP_MEN,				NULL,	0,	0,	MEN_HANG_UP },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_WAIT_MEN,					NULL,	0,	0,	MEN_WAIT },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_FLUSH_BUFFER_MEN,			NULL,	0,	0,	MEN_FLUSH_BUFFER },
		{  NM_ITEM, MSG_TERMDATA_RELEASE_DEVICE_MEN,		NULL,	0,	0,	MEN_RELEASE_DEVICE },

		{ NM_TITLE, MSG_TERMDATA_XFER_MEN },
		{  NM_ITEM, MSG_TERMDATA_UPLOAD_ASCII_MEN,			NULL,	0,	0,	MEN_UPLOAD_ASCII },
		{  NM_ITEM, MSG_TERMDATA_DOWNLOAD_ASCII_MEN,		NULL,	0,	0,	MEN_DOWNLOAD_ASCII },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_UPLOAD_TEXT_MEN,			NULL,	0,	0,	MEN_UPLOAD_TEXT },
		{  NM_ITEM, MSG_TERMDATA_DOWNLOAD_TEXT_MEN,			NULL,	0,	0,	MEN_DOWNLOAD_TEXT },
		{  NM_ITEM, MSG_TERMDATA_EDIT_AND_UPLOAD_TEXT_MEN,	NULL,	0,	0,	MEN_EDIT_AND_UPLOAD_TEXT },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_UPLOAD_BINARY_MEN,			NULL,	0,	0,	MEN_UPLOAD_BINARY },
		{  NM_ITEM, MSG_TERMDATA_DOWNLOAD_BINARY_MEN,		NULL,	0,	0,	MEN_DOWNLOAD_BINARY },

		{ NM_TITLE, MSG_TERMDATA_BUFFER_MEN },
		{  NM_ITEM, MSG_TERMDATA_CLEAR_BUFFER_MEN,			NULL,	0,	0,	MEN_CLEAR_BUFFER },
		{  NM_ITEM, MSG_TERMDATA_DISPLAY_BUFFER_MEN,		NULL,	0,	0,	MEN_DISPLAY_BUFFER },
		{  NM_ITEM, MSG_TERMDATA_CLOSE_BUFFER_MEN,			NULL,	0,	0,	MEN_CLOSE_BUFFER },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_FREEZE_BUFFER_MEN,			NULL,	TICK,	0,	MEN_FREEZE_BUFFER },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_OPEN_BUFFER_MEN,			NULL,	0,	0,	MEN_OPEN_BUFFER },
		{  NM_ITEM, MSG_TERMDATA_SAVE_BUFFER_AS_MEN,		NULL,	0,	0,	MEN_SAVE_BUFFER_AS },

		{ NM_TITLE, MSG_TERMDATA_SCREEN_MEN },
		{  NM_ITEM, MSG_TERMDATA_CLEAR_SCREEN_MEN,			NULL,	0,	0,	MEN_CLEAR_SCREEN },
		{  NM_ITEM, MSG_TERMDATA_RESET_FONT_MEN,			NULL,	0,	0,	MEN_RESET_FONT },
		{  NM_ITEM, MSG_TERMDATA_RESET_STYLES_MEN,			NULL,	0,	0,	MEN_RESET_STYLES },
		{  NM_ITEM, MSG_TERMDATA_RESET_TERMINAL_MEN,		NULL,	0,	0,	MEN_RESET_TERMINAL },

		{ NM_TITLE, MSG_TERMDATA_SETTINGS_MEN },
		{  NM_ITEM, MSG_TERMDATA_SERIAL_MEN,				NULL,	0,	0,	MEN_SERIAL },
		{  NM_ITEM, MSG_TERMDATA_MODEM_MEN,					NULL,	0,	0,	MEN_MODEM },
		{  NM_ITEM, MSG_TERMDATA_SCREEN_PREFS_MEN,			NULL,	0,	0,	MEN_SCREEN },
		{  NM_ITEM, MSG_TERMDATA_TERMINAL_MEN,				NULL,	0,	0,	MEN_TERMINAL },
		{  NM_ITEM, MSG_TERMDATA_SET_EMULATION_MEN,			NULL,	0,	0,	MEN_SET_EMULATION },
		{  NM_ITEM, MSG_TERMDATA_CLIPBOARD_PREFS_MEN,		NULL,	0,	0,	MEN_CLIPBOARD },
		{  NM_ITEM, MSG_TERMDATA_CAPTURE_PREFS_MEN,			NULL,	0,	0,	MEN_CAPTURE },
		{  NM_ITEM, MSG_TERMDATA_COMMANDS_PREFS_MEN,		NULL,	0,	0,	MEN_COMMANDS },
		{  NM_ITEM, MSG_TERMDATA_MISC_MEN,					NULL,	0,	0,	MEN_MISC },
		{  NM_ITEM, MSG_TERMDATA_PATH_MEN,					NULL,	0,	0,	MEN_PATH },
		{  NM_ITEM, MSG_TERMDATA_TRANSFER_PROTOCOL_MEN,		NULL,	0,	0,	MEN_TRANSFER_PROTOCOL },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_TRANSFER_MEN,				NULL,	0,	0,	MEN_TRANSFER },
		{  NM_ITEM, MSG_TERMDATA_TRANSLATION_MEN,			NULL,	0,	0,	MEN_TRANSLATION },
		{  NM_ITEM, MSG_TERMDATA_MACROS_MEN,				NULL,	0,	0,	MEN_MACROS },
		{  NM_ITEM, MSG_TERMDATA_CURSORKEYS_MEN,			NULL,	0,	0,	MEN_CURSORKEYS },
		{  NM_ITEM, MSG_TERMDATA_FAST_MACROS_MEN,			NULL,	0,	0,	MEN_FAST_MACROS },
		{  NM_ITEM, MSG_TERMDATA_HOTKEYS_MEN,				NULL,	0,	0,	MEN_HOTKEYS },
		{  NM_ITEM, MSG_TERMDATA_SPEECH_MEN,				NULL,	0,	0,	MEN_SPEECH },
		{  NM_ITEM, MSG_TERMDATA_SOUND_MEN,					NULL,	0,	0,	MEN_SOUND },
		{  NM_ITEM, MSG_AREACODES_MEN,						NULL,	0,	0,	MEN_RATES },
		{  NM_ITEM, LNM_BARLABEL },
		{  NM_ITEM, MSG_TERMDATA_OPEN_SETTINGS_MEN,			NULL,	0,	0,	MEN_OPEN_SETTINGS },
		{  NM_ITEM, MSG_TERMDATA_SAVE_SETTINGS_MEN,			NULL,	0,	0,	MEN_SAVE_SETTINGS },
		{  NM_ITEM, MSG_TERMDATA_SAVE_SETTINGS_AS_MEN,		NULL,	0,	0,	MEN_SAVE_SETTINGS_AS },

		{ NM_TITLE, MSG_TERMDATA_WINDOWS_MEN },
		{  NM_ITEM, MSG_TERMDATA_STATUS_WINDOW_MEN,			NULL,	TICK,	0,	MEN_STATUS_WINDOW },
		{  NM_ITEM, MSG_TERMDATA_REVIEW_WINDOW_MEN,			NULL,	TICK,	0,	MEN_REVIEW_WINDOW },
		{  NM_ITEM, MSG_TERMDATA_PACKET_WINDOW_MEN,			NULL,	CHECKIT,	0,	MEN_PACKET_WINDOW },
		{  NM_ITEM, MSG_TERMDATA_CHAT_LINE_MEN,				NULL,	TICK,	0,	MEN_CHAT_LINE },
		{  NM_ITEM, MSG_TERMDATA_FAST_MACROS_WINDOW_MEN,	NULL,	TICK,	0,	MEN_FAST_MACROS_WINDOW },
		{  NM_ITEM, MSG_TERMDATA_MATRIX_WINDOW_MEN,			NULL,	TICK,	0,	MEN_MATRIX_WINDOW },
		{  NM_ITEM, MSG_TERMDATA_UPLOAD_QUEUE_WINDOW_MEN,	NULL,	0,	0,	MEN_UPLOAD_QUEUE_WINDOW },

		{ NM_TITLE, MSG_TERMDATA_DIALING_MEN,				NULL,	0,	0,	MEN_EXTRA_DIAL },
		{   NM_END }
	};

	CONST_STRPTR String;
	LONG i;

	for(i = 0 ; LocalNewMenu[i].lnm_Type != NM_END ; i++)
	{
		if(LocalNewMenu[i].lnm_LabelID != LNM_BARLABEL)
		{
			String = LocaleString(LocalNewMenu[i].lnm_LabelID);

			if(String[1] == 0)
			{
				LocalNewMenu[i].lnm_CommKey = (APTR)String;

				String += 2;
			}

			LocalNewMenu[i].lnm_LabelID = (IPTR)String;
		}
	}

	*NewMenuPtr			= (struct NewMenu *)&LocalNewMenu[0];
	*NumMenuEntryPtr	= i + 1;
}

	/* CopyItemFlags(struct MenuItem *Src,struct MenuItem *Dst):
	 *
	 *	Copy single menu item flags from one menu strip
	 *	to another.
	 */

STATIC VOID
CopyItemFlags(struct MenuItem *Src,struct MenuItem *Dst)
{
	while(Src && Dst)
	{
		if(Src->SubItem)
			CopyItemFlags(Src->SubItem,Dst->SubItem);

		Dst->Flags = Src->Flags;

		Src = Src->NextItem;
		Dst = Dst->NextItem;
	}
}

	/* CopyMenuFlags(struct Menu *Src,struct Menu *Dst):
	 *
	 *	Copy menu flags from one menu strip to
	 *	another.
	 */

STATIC VOID
CopyMenuFlags(struct Menu *Src,struct Menu *Dst)
{
	struct MenuItem *SrcItem,*DstItem;

	while(Src && Dst)
	{
			/* Don't touch the quick dial menu. If CopyMenuFlags() */
			/* is called its contents are likely to have changed */

		if((ULONG)(IPTR)GTMENU_USERDATA(Src) == DIAL_MENU_LIMIT)
			break;
		else
		{
			SrcItem = Src->FirstItem;
			DstItem = Dst->FirstItem;

			while(SrcItem && DstItem)
			{
				CopyItemFlags(SrcItem,DstItem);

				SrcItem = SrcItem->NextItem;
				DstItem = DstItem->NextItem;
			}

			Src = Src->NextMenu;
			Dst = Dst->NextMenu;
		}
	}
}

	/* SetMenuStrips(struct Menu *Menu):
	 *
	 *	Set a menu for all windows that support it.
	 */

VOID
SetMenuStrips(struct Menu *Menu)
{
	ObtainSemaphore(&MenuSemaphore);

	if(Window)
		SetMenuStrip(Window,Menu);

	if(StatusWindow)
		SetMenuStrip(StatusWindow,Menu);

	if(FastWindow)
		SetMenuStrip(FastWindow,Menu);

	ReleaseSemaphore(&MenuSemaphore);
}

	/* ResetMenuStrips(struct Menu *Menu):
	 *
	 *	Reset a menu for all windows that support it.
	 */

VOID
ResetMenuStrips(struct Menu *Menu)
{
	ObtainSemaphore(&MenuSemaphore);

	if(Window)
		ResetMenuStrip(Window,Menu);

	if(StatusWindow)
		ResetMenuStrip(StatusWindow,Menu);

	if(FastWindow)
		ResetMenuStrip(FastWindow,Menu);

	ReleaseSemaphore(&MenuSemaphore);
}

	/* ClearMenuStrips():
	 *
	 *	Clear a menu of all windows that support it.
	 */

VOID
ClearMenuStrips()
{
	ObtainSemaphore(&MenuSemaphore);

	if(Window)
		ClearMenuStrip(Window);

	if(StatusWindow)
		ClearMenuStrip(StatusWindow);

	if(FastWindow)
		ClearMenuStrip(FastWindow);

	ReleaseSemaphore(&MenuSemaphore);
}

	/* AttachMenu():
	 *
	 *	Rebuild the main menu (if necessary) and attach it to the other windows.
	 */

BOOL
AttachMenu(struct Menu *ThisMenu)
{
	BOOL Result;

	ObtainSemaphore(&MenuSemaphore);

	if(!ThisMenu)
		ThisMenu = BuildMenu();

	if(ThisMenu)
	{
		if(Menu)
		{
			CopyMenuFlags(Menu,ThisMenu);

			ClearMenuStrips();

			LT_DisposeMenu(Menu);
		}

		Menu = ThisMenu;

		SetMenuStrips(Menu);

		Result = TRUE;
	}
	else
		Result = FALSE;

	ReleaseSemaphore(&MenuSemaphore);

	return(Result);
}

	/* DisconnectDialMenu():
	 *
	 *	Disconnect the quick dial menu as its contents will
	 *	be invalid. This may happen if `term' fails to rebuild
	 *	the main menu due to memory shortage. Please note that
	 *	disconnecting the menu this way is legal only for menus
	 *	created by gtlayout.library.
	 */

VOID
DisconnectDialMenu()
{
	ObtainSemaphore(&MenuSemaphore);

	if(Menu)
	{
		struct Menu *ThisMenu,*LastMenu = NULL;

		ClearMenuStrips();

		for(ThisMenu = Menu ; ThisMenu ; LastMenu = ThisMenu, ThisMenu = ThisMenu->NextMenu)
		{
			if(!ThisMenu->NextMenu && (ULONG)(IPTR)GTMENU_USERDATA(ThisMenu) == DIAL_MENU_LIMIT)
			{
				LastMenu->NextMenu = NULL;
				break;
			}
		}

		SetMenuStrips(Menu);
	}

	ReleaseSemaphore(&MenuSemaphore);
}

	/* BuildMenu():
	 *
	 *	Create the menu strip, including the quick dial menu.
	 */

struct Menu *
BuildMenu()
{
	LONG PhoneCount,Grouped,NotGrouped,Groups,Separator,i,j;
	PhoneEntry **Phonebook = GlobalPhoneHandle->Phonebook;
	BOOL SpeechAvailable;

		/* Check if the speech synthesizer is available. */

	SpeechAvailable = SpeechSynthesizerAvailable();

		/* Reset the variables. */

	Grouped = NotGrouped = Groups = PhoneCount = Separator = 0;

		/* Add the quick dial entries. This is not your basic */
		/* O(N) algorithm :-/ */

	if(GlobalPhoneHandle->Phonebook && GlobalPhoneHandle->NumPhoneEntries > 0)
	{
		for(i = 0 ; PhoneCount < DIAL_MENU_MAX && i < GlobalPhoneHandle->NumPhoneEntries ; i++)
		{
			if(Phonebook[i]->Header->QuickMenu)
			{
				PhoneCount++;

					/* Check if this entry is in a group */

				if(Phonebook[i]->ThisGroup)
				{
					BOOL FirstOne = TRUE;

					Grouped++;

						/* Are there other entries in this group? */

					for(j = 0 ; j < i ; j++)
					{
						if(Phonebook[j]->Header->QuickMenu && Phonebook[j]->ThisGroup == Phonebook[i]->ThisGroup)
						{
							FirstOne = FALSE;

							break;
						}
					}

						/* Is this the first one? */

					if(FirstOne)
						Groups++;
				}
				else
					NotGrouped++;
			}
		}

			/* Will we need to mix grouped and ungrouped entries? */

		if(Grouped && NotGrouped)
			Separator = 1;
		else
			Separator = 0;
	}

		/* Don't do the work if no quick dial menu needs to be built */

	if(PhoneCount)
	{
		struct NewMenu *NewMenu;

			/* Allocate new menu prototypes */

		if(NewMenu = (struct NewMenu *)AllocVecPooled((NumMenuEntries + PhoneCount + Groups + Separator) * sizeof(struct NewMenu),MEMF_ANY | MEMF_CLEAR))
		{
			struct Menu	*ThisMenu;
			LONG		 Count;

				/* Reset the menu type */

			TermMenu[NumMenuEntries - 2].nm_Type		= NM_TITLE;
			TermMenu[NumMenuEntries - 2].nm_UserData	= (APTR)DIAL_MENU_LIMIT;

			CopyMem(TermMenu,NewMenu,NumMenuEntries * sizeof(struct NewMenu));

			FirstDialMenu = -1;

			Count = NumMenuEntries - 1;

				/* Are we to pull entries just from one single group? */

			if(Grouped == 1 && !NotGrouped)
			{
				for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
				{
					if(Phonebook[i]->Header->QuickMenu)
					{
						NewMenu[Count].nm_Type		= NM_ITEM;
						NewMenu[Count].nm_Label		= Phonebook[i]->Header->Name;
						NewMenu[Count].nm_Flags		= CHECKIT|MENUTOGGLE;
						NewMenu[Count].nm_UserData	= (APTR)(IPTR)(DIAL_MENU_LIMIT + i);
						NewMenu[Count].nm_CommKey	= NULL;

						Count++;

						if(FirstDialMenu == -1)
							FirstDialMenu = DIAL_MENU_LIMIT + i;
					}
				}
			}
			else
			{
					/* First step: collect the entries that don't belong into groups */

				for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
				{
					if(Phonebook[i]->Header->QuickMenu && !Phonebook[i]->ThisGroup)
					{
						NewMenu[Count].nm_Type		= NM_ITEM;
						NewMenu[Count].nm_Label		= Phonebook[i]->Header->Name;
						NewMenu[Count].nm_Flags		= CHECKIT|MENUTOGGLE;
						NewMenu[Count].nm_UserData	= (APTR)(IPTR)(DIAL_MENU_LIMIT + i);
						NewMenu[Count].nm_CommKey	= NULL;

						Count++;

						if(FirstDialMenu == -1)
							FirstDialMenu = DIAL_MENU_LIMIT + i;
					}
				}

					/* If there needs to be a separator, add it */

				if(Separator)
				{
					NewMenu[Count].nm_Type		= NM_ITEM;
					NewMenu[Count].nm_Label		= NM_BARLABEL;
					NewMenu[Count].nm_Flags		= 0;
					NewMenu[Count].nm_UserData	= NULL;
					NewMenu[Count].nm_CommKey	= NULL;

					Count++;
				}

					/* Second step: collect the remaining entries that belong into groups */

				for(i = 0 ; i < GlobalPhoneHandle->NumPhoneEntries ; i++)
				{
					if(Phonebook[i]->Header->QuickMenu && Phonebook[i]->ThisGroup)
					{
						BOOL FirstOne = TRUE;

						for(j = 0 ; j < i ; j++)
						{
							if(Phonebook[j]->Header->QuickMenu && Phonebook[j]->ThisGroup && Phonebook[j]->ThisGroup == Phonebook[i]->ThisGroup)
							{
								FirstOne = FALSE;
								break;
							}
						}

						if(FirstOne)
						{
							LONG k;

							NewMenu[Count].nm_Type		= NM_ITEM;
							NewMenu[Count].nm_Label		= Phonebook[i]->ThisGroup->LocalName;
							NewMenu[Count].nm_Flags		= 0;
							NewMenu[Count].nm_UserData	= NULL;
							NewMenu[Count].nm_CommKey	= NULL;

							Count++;

							for(k = i ; k < GlobalPhoneHandle->NumPhoneEntries ; k++)
							{
								if(Phonebook[k]->Header->QuickMenu && Phonebook[k]->ThisGroup == Phonebook[i]->ThisGroup)
								{
									NewMenu[Count].nm_Type	= NM_SUB;
									NewMenu[Count].nm_Label	= Phonebook[k]->Header->Name;
									NewMenu[Count].nm_Flags	= CHECKIT|MENUTOGGLE;
									NewMenu[Count].nm_UserData	= (APTR)(IPTR)(DIAL_MENU_LIMIT + k);
									NewMenu[Count].nm_CommKey	= NULL;

									Count++;

									if(FirstDialMenu == -1)
										FirstDialMenu = DIAL_MENU_LIMIT + k;
								}
							}
						}
					}
				}

			}

			NewMenu[Count].nm_Type = NM_END;

				/* If the speech synthesizer is not available, disconnect */
				/* the speech settings menu item */

			for(i = Count - 1 ; i >= 0 ; i--)
			{
				if(TermMenu[i].nm_UserData == (APTR)MEN_SPEECH)
				{
					if(SpeechAvailable)
						TermMenu[i].nm_Flags &= ~NM_ITEMDISABLED;
					else
						TermMenu[i].nm_Flags |= NM_ITEMDISABLED;
				}

				if(TermMenu[i].nm_UserData == (APTR)MEN_EXECUTE_REXX_COMMAND)
				{
					if(!RexxSysBase)
						TermMenu[i].nm_Type |= NM_ITEMDISABLED;
				}
			}

				/* Now layout the menu */

			ThisMenu = LT_NewMenuTemplate(Window->WScreen,NULL,AmigaGlyph,CheckGlyph,NULL,NewMenu);

				/* We don't need this any more */

			FreeVecPooled(NewMenu);

				/* Successful? */

			if(ThisMenu)
				return(ThisMenu);
		}
	}

		/* If the speech synthesizer is not available, disconnect */
		/* the speech settings menu item */

	for(i = NumMenuEntries - 1 ; i >= 0 ; i--)
	{
		if(TermMenu[i].nm_UserData == (APTR)MEN_SPEECH)
		{
			if(SpeechAvailable)
				TermMenu[i].nm_Flags &= ~NM_ITEMDISABLED;
			else
				TermMenu[i].nm_Flags |= NM_ITEMDISABLED;
		}

		if(TermMenu[i].nm_UserData == (APTR)MEN_EXECUTE_REXX_COMMAND)
		{
			if(!RexxSysBase)
				TermMenu[i].nm_Flags |= NM_ITEMDISABLED;
			else
				TermMenu[i].nm_Flags &= ~NM_ITEMDISABLED;
		}
	}

		/* Disconnect the quick dial menu */

	TermMenu[NumMenuEntries - 2].nm_Type = NM_END;

		/* Create the default menu strip */

	return(LT_NewMenuTemplate(Window->WScreen,NULL,AmigaGlyph,CheckGlyph,NULL,TermMenu));
}

	/* SetClipMenu(BYTE Mode):
	 *
	 *	Enable/disable the copy/clear selection menu items.
	 */

VOID
SetClipMenu(BOOL Mode)
{
	if(Mode && RasterEnabled)
	{
		OnItem(MEN_COPY);
		OnItem(MEN_CLEAR);
	}
	else
	{
		OffItem(MEN_COPY);
		OffItem(MEN_CLEAR);
	}
}

	/* SetRedialMenu():
	 *
	 *	Make the `redial' menu item available or make it
	 *	unavailable.
	 */

VOID
SetRedialMenu()
{
	BOOL NoDialList;

	if(GlobalPhoneHandle->DialList)
		NoDialList = IsListEmpty(GlobalPhoneHandle->DialList);
	else
		NoDialList = TRUE;

	if(!NoDialList && (DialItemsAvailable || Config->MiscConfig->ProtectiveMode))
		OnItem(MEN_REDIAL);
	else
		OffItem(MEN_REDIAL);
}

	/* SetDialMenu(BOOL Mode):
	 *
	 *	Block or enable the dialing menu.
	 */

VOID
SetDialMenu(BOOL Mode)
{
	if(Window && Menu)
	{
		if(Mode || Config->MiscConfig->ProtectiveMode)
		{
			BOOL NoDialList;

			if(GlobalPhoneHandle->DialList)
				NoDialList = IsListEmpty(GlobalPhoneHandle->DialList);
			else
				NoDialList = TRUE;

			if(NoDialList)
				OffItem(MEN_REDIAL);
			else
				OnItem(MEN_REDIAL);

			OnItem(MEN_DIAL_NUMBER);

			if(FirstDialMenu != -1)
				OnItem(MEN_EXTRA_DIAL);
		}
		else
		{
			OffItem(MEN_REDIAL);
			OffItem(MEN_DIAL_NUMBER);

			if(FirstDialMenu != -1)
				OffItem(MEN_EXTRA_DIAL);
		}

		DialItemsAvailable = Mode;
	}
}

	/* SetTransferMenu(BOOL Mode):
	 *
	 *	Block or enable the transfer menu.
	 */

VOID
SetTransferMenu(BOOL Mode)
{
	if(Window && Menu)
	{
		BOOL	ValidDefault = FALSE,
				ValidASCIIDownload = FALSE,
				ValidASCIIUpload = FALSE,
				ValidTextDownload = FALSE,
				ValidTextUpload = FALSE,
				ValidBinaryDownload = FALSE,
				ValidBinaryUpload = FALSE;

		if(!Config->TransferConfig->DefaultLibrary[0] || !Mode || (!XProtocolBase && Config->TransferConfig->DefaultType == XFER_XPR))
			ValidDefault = FALSE;
		else
			ValidDefault = TRUE;

		switch(Config->TransferConfig->ASCIIUploadType)
		{
			case XFER_INTERNAL:

				ValidASCIIUpload = TRUE;
				break;

			case XFER_DEFAULT:

				ValidASCIIUpload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidASCIIUpload = Config->TransferConfig->ASCIIUploadLibrary[0];
				break;
		}

		switch(Config->TransferConfig->ASCIIDownloadType)
		{
			case XFER_INTERNAL:

				ValidASCIIDownload = TRUE;
				break;

			case XFER_DEFAULT:

				ValidASCIIDownload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidASCIIDownload = Config->TransferConfig->ASCIIDownloadLibrary[0];
				break;
		}


		switch(Config->TransferConfig->TextUploadType)
		{
			case XFER_DEFAULT:

				ValidTextUpload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidTextUpload = Config->TransferConfig->TextUploadLibrary[0];
				break;
		}

		switch(Config->TransferConfig->TextDownloadType)
		{
			case XFER_DEFAULT:

				ValidTextDownload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidTextDownload = Config->TransferConfig->TextDownloadLibrary[0];
				break;
		}

		switch(Config->TransferConfig->BinaryUploadType)
		{
			case XFER_DEFAULT:

				ValidBinaryUpload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidBinaryUpload = Config->TransferConfig->BinaryUploadLibrary[0];
				break;
		}

		switch(Config->TransferConfig->BinaryDownloadType)
		{
			case XFER_DEFAULT:

				ValidBinaryDownload = ValidDefault;
				break;

			case XFER_XPR:
			case XFER_EXTERNALPROGRAM:

				ValidBinaryDownload = Config->TransferConfig->BinaryDownloadLibrary[0];
				break;
		}

#ifdef BUILTIN_ZMODEM
		if(UseInternalZModem)
			ValidDefault = ValidBinaryDownload = ValidBinaryUpload = ValidTextDownload = TRUE;
#endif	/* BUILTIN_ZMODEM */

		if(ValidASCIIUpload)
			OnItem(MEN_UPLOAD_ASCII);
		else
			OffItem(MEN_UPLOAD_ASCII);

		if(ValidASCIIDownload)
			OnItem(MEN_DOWNLOAD_ASCII);
		else
			OffItem(MEN_DOWNLOAD_ASCII);

		if(ValidTextUpload)
		{
			OnItem(MEN_UPLOAD_TEXT);
			OnItem(MEN_EDIT_AND_UPLOAD_TEXT);
		}
		else
		{
			OffItem(MEN_UPLOAD_TEXT);
			OffItem(MEN_EDIT_AND_UPLOAD_TEXT);
		}

#ifdef BUILTIN_ZMODEM
		if(UseInternalZModem)
			OnItem(MEN_UPLOAD_TEXT);
#endif	/* BUILTIN_ZMODEM */

		if(ValidTextDownload)
			OnItem(MEN_DOWNLOAD_TEXT);
		else
			OffItem(MEN_DOWNLOAD_TEXT);

		if(ValidBinaryUpload)
			OnItem(MEN_UPLOAD_BINARY);
		else
			OffItem(MEN_UPLOAD_BINARY);

		if(ValidBinaryDownload)
			OnItem(MEN_DOWNLOAD_BINARY);
		else
			OffItem(MEN_DOWNLOAD_BINARY);

		if(ValidDefault)
			OnItem(MEN_TRANSFER);
		else
			OffItem(MEN_TRANSFER);
	}
}

	/* SetRasterMenu(BOOL Mode):
	 *
	 *	Block or enable the menu entries associated with
	 *	functions to access the screen raster.
	 */

VOID
SetRasterMenu(BOOL Mode)
{
	if(Window && Menu)
	{
		if(Mode)
		{
			OnItem(MEN_SAVE_AS_TEXT);
			OnItem(MEN_PRINT_SCREEN);
		}
		else
		{
			OffItem(MEN_SAVE_AS_TEXT);
			OffItem(MEN_PRINT_SCREEN);
		}
	}
}

	/* LookForIt(struct MenuItem *Item,ULONG ID):
	 *
	 *	Auxilary routine for FindThisItem(), scans
	 *	menu item and sub item lists.
	 */

STATIC struct MenuItem *
LookForIt(struct MenuItem *Item,ULONG ID)
{
	while(Item)
	{
		if((ULONG)(IPTR)GTMENUITEM_USERDATA(Item) == ID)
			return(Item);
		else
		{
			if(Item->SubItem)
			{
				struct MenuItem *TheItem;

				if(TheItem = LookForIt(Item->SubItem,ID))
					return(TheItem);
			}

			Item = Item->NextItem;
		}
	}

	return(NULL);
}

	/* FindThisItem(struct Menu *FirstMenu,ULONG MenuID):
	 *
	 *	Scan a menu for a menuitem associated with a
	 *	menu ID.
	 */

STATIC struct MenuItem *
FindThisItem(struct Menu *FirstMenu,ULONG MenuID)
{
	struct MenuItem *FoundItem = NULL;

	SafeObtainSemaphoreShared(&MenuSemaphore);

	if(TypeOfMem(FirstMenu))
	{
		struct MenuItem *Item;

		while(FirstMenu)
		{
			if(Item = LookForIt(FirstMenu->FirstItem,MenuID))
			{
				FoundItem = Item;
				break;
			}
			else
				FirstMenu = FirstMenu->NextMenu;
		}
	}

	ReleaseSemaphore(&MenuSemaphore);

	return(FoundItem);
}

STATIC struct Menu *
FindThisMenu(struct Menu *FirstMenu,ULONG MenuID)
{
	struct Menu *FoundMenu = NULL;

	SafeObtainSemaphoreShared(&MenuSemaphore);

	if(TypeOfMem(FirstMenu))
	{
		while(FirstMenu)
		{
			if(GTMENU_USERDATA(FirstMenu) == (APTR)(IPTR)MenuID)
			{
				FoundMenu = FirstMenu;
				break;
			}
			else
				FirstMenu = FirstMenu->NextMenu;
		}
	}

	ReleaseSemaphore(&MenuSemaphore);

	return(FoundMenu);
}

	/* GetItem(ULONG MenuID):
	 *
	 *	Get the checkmark state of a menu item.
	 */

BOOL
GetItem(ULONG MenuID)
{
	BOOL Result = FALSE;

	SafeObtainSemaphoreShared(&MenuSemaphore);

	if(Menu)
	{
		struct MenuItem *Item;

		if(Item = FindThisItem(Menu,MenuID))
		{
			if(Item->Flags & CHECKED)
				Result = TRUE;
		}
	}

	ReleaseSemaphore(&MenuSemaphore);

	return(Result);
}

	/* SetItem(ULONG MenuID,BOOL Mode):
	 *
	 *	Clear or set the checkmark or state of a menu item.
	 */

VOID
SetItem(ULONG MenuID,BOOL Mode)
{
	ObtainSemaphore(&MenuSemaphore);

		/* Is the global pull-down menu available? */

	if(Menu)
	{
		struct MenuItem	*Item;
		struct Menu	*ThisMenu;

		if(ThisMenu = FindThisMenu(Menu,MenuID))
		{
				/* Remove the menu from the windows. */

			ClearMenuStrips();

			switch(Mode)
			{
				case SETITEM_ON:

					ThisMenu->Flags |= MENUENABLED;
					break;

				case SETITEM_OFF:

					ThisMenu->Flags &= ~MENUENABLED;
					break;
			}

				/* Reattach the menu to the windows. */

			ResetMenuStrips(Menu);
		}
		else
		{
				/* Try to find the menu item and change
				 * the state of the checkmark.
				 */

			if(Item = FindThisItem(Menu,MenuID))
			{
					/* Remove the menu from the windows. */

				ClearMenuStrips();

				switch(Mode)
				{
					case SETITEM_SETCHECK:

						Item->Flags |= CHECKED;
						break;

					case SETITEM_CLRCHECK:

						Item->Flags &= ~CHECKED;
						break;

					case SETITEM_ON:

						Item->Flags |= ITEMENABLED;
						break;

					case SETITEM_OFF:

						Item->Flags &= ~ITEMENABLED;
						break;
				}

					/* Reattach the menu to the windows. */

				ResetMenuStrips(Menu);
			}
		}
	}

	ReleaseSemaphore(&MenuSemaphore);
}

	/* CreateMenuGlyphs():
	 *
	 *	Create scaled glyphs for pull-down menus.
	 */

BOOL
CreateMenuGlyphs(struct Screen *Screen,struct DrawInfo *DrawInfo,struct Image **AmigaPtr,struct Image **CheckPtr)
{
	struct Image *AmigaGlyph,*CheckGlyph;
	LONG AspectX,AspectY,Size,FontHeight;

	FontHeight = DrawInfo->dri_Font->tf_Baseline + 2;

	if(Screen->Flags & SCREENHIRES)
		Size = SYSISIZE_MEDRES;
	else
		Size = SYSISIZE_LOWRES;

	AspectX = DrawInfo->dri_Resolution.X;
	AspectY = DrawInfo->dri_Resolution.Y;

	AmigaGlyph = NewObject(NULL,SYSICLASS,
		SYSIA_DrawInfo,	DrawInfo,
		SYSIA_Size,		Size,
		SYSIA_Which,	AMIGAKEY,
		IA_Left,		0,
		LA_Top,			0,
		IA_Width,		(FontHeight * 3 * AspectY) / (2 * AspectX),
		IA_Height,		FontHeight,
	TAG_DONE);

	CheckGlyph = NewObject(NULL,SYSICLASS,
		SYSIA_DrawInfo,	DrawInfo,
		SYSIA_Size,		Size,
		SYSIA_Which,	MENUCHECK,
		IA_Left,		0,
		LA_Top,			0,
		IA_Width,		(FontHeight * AspectY) / AspectX,
		IA_Height,		FontHeight,
	TAG_DONE);

	if(AmigaGlyph && CheckGlyph)
	{
		*AmigaPtr = AmigaGlyph;
		*CheckPtr = CheckGlyph;

		return(TRUE);
	}
	else
	{
		DisposeObject(AmigaGlyph);
		DisposeObject(CheckGlyph);

		return(FALSE);
	}
}
