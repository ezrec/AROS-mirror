/*
**	PhonePanel.c
**
**	The phonebook and support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The gadget IDs are in there */

#ifndef _PHONEPANEL_H
#include "PhonePanel.h"
#endif	/* _PHONEPANEL_H */

	/* What action is to follow clicking into the listview. */

enum	{	CLICK_Select,CLICK_Toggle };

	/* What to do with an entry. */

enum	{	CHANGE_Set,CHANGE_Clear,CHANGE_Toggle };

	/* Local prototypes. */

STATIC VOID CloseSomeWindow(PhoneListContext *Context, struct Window *Window);
STATIC VOID LockInterface(PhoneListContext *Context);
STATIC VOID UnlockInterface(PhoneListContext *Context);
STATIC VOID HighlightActiveEntry(PhoneListContext *Context, BOOL Activate);
STATIC VOID UpdateWindowTitle(PhoneListContext *Context);
STATIC VOID UpdateEditorData(PhoneListContext *Context);
STATIC PhoneGroupNode *GetActiveGroup(PhoneListContext *Context);
STATIC VOID DeletePhoneListContext(PhoneListContext *Context);
STATIC PhoneListContext *CreatePhoneListContext(PhonebookHandle *PhoneHandle);
STATIC VOID SelectActiveEntry(PhoneListContext *Context, LONG Index, BOOL ShowIt);
STATIC VOID DetachList(PhoneListContext *Context);
STATIC VOID AttachList(PhoneListContext *Context);
STATIC VOID UpdateActiveEntry(PhoneListContext *Context);
STATIC VOID ChangeActiveEntry(PhoneListContext *Context, LONG How);
STATIC VOID ToggleAllEntries(PhoneListContext *Context);
STATIC VOID SelectAllEntries(PhoneListContext *Context);
STATIC VOID ClearAllEntries(PhoneListContext *Context);
STATIC VOID SelectIfMarked(PhoneListContext *Context);
STATIC VOID DeleteActiveEntry(PhoneListContext *Context);
STATIC VOID CopyActiveEntry(PhoneListContext *Context);
STATIC VOID LoadNewPhonebook(PhoneListContext *Context);
STATIC VOID MergeNewPhonebook(PhoneListContext *Context);
STATIC VOID ChangeGroup(PhoneListContext *Context);
STATIC VOID SelectGroup(PhoneListContext *Context, LONG Index);
STATIC VOID SplitGroup(PhoneListContext *Context);
STATIC VOID SortThePhonebook(PhoneListContext *Context, LONG How, BOOL ReverseOrder);
STATIC VOID SelectNewTaggedEntry(PhoneListContext *Context, LONG How);
STATIC VOID MoveActiveEntry(PhoneListContext *Context, LONG How);
STATIC VOID AddNewEntry(PhoneListContext *Context);

/******************************************************************************/

STATIC VOID
CloseSomeWindow(PhoneListContext *Context,struct Window *Window)
{
	struct Window **WindowPtr;
	LayoutHandle **HandlePtr;

	WindowPtr = NULL;
	HandlePtr = NULL;

	if(Window == Context->Window)
	{
		WindowPtr = &Context->Window;
		HandlePtr = &Context->Manager;
	}

	if(Window == Context->EditWindow)
	{
		WindowPtr = &Context->EditWindow;
		HandlePtr = &Context->Editor;
	}

	if(Window == Context->PatternWindow)
	{
		WindowPtr = &Context->PatternWindow;
		HandlePtr = &Context->Selector;
	}

	if(Window == Context->GroupWindow)
	{
		WindowPtr = &Context->GroupWindow;
		HandlePtr = &Context->Grouping;
	}

	if(Window == Context->SortWindow)
	{
		WindowPtr = &Context->SortWindow;
		HandlePtr = &Context->Sorting;
	}

	if(Window == Context->PasswordWindow)
	{
		WindowPtr = &Context->PasswordWindow;
		HandlePtr = &Context->PasswordHandle;
	}

	if(WindowPtr)
	{
		LT_DeleteHandle(*HandlePtr);

		if(Window != Context->Window)
			LT_ShowWindow(Context->Manager,TRUE);

		*HandlePtr = NULL;
		*WindowPtr = NULL;
	}
}

STATIC VOID
LockInterface(PhoneListContext *Context)
{
	LT_LockWindow(Context->Window);
	LT_LockWindow(Context->PasswordWindow);
	LT_LockWindow(Context->EditWindow);
	LT_LockWindow(Context->PatternWindow);
	LT_LockWindow(Context->GroupWindow);
	LT_LockWindow(Context->SortWindow);
}

STATIC VOID
UnlockInterface(PhoneListContext *Context)
{
	LT_UnlockWindow(Context->Window);
	LT_UnlockWindow(Context->PasswordWindow);
	LT_UnlockWindow(Context->EditWindow);
	LT_UnlockWindow(Context->PatternWindow);
	LT_UnlockWindow(Context->GroupWindow);
	LT_UnlockWindow(Context->SortWindow);
}

STATIC VOID
HighlightActiveEntry(PhoneListContext *Context,BOOL Activate)
{
	BOOL IsFirst,IsLast,Disabled;
	LayoutHandle *Manager;
	LayoutHandle *Editor;
	struct List *PhoneList;
	PhoneNode *Node;
	ULONG Count;
	ULONG Grouper;
	BOOL Hide;

	Manager = Context->Manager;
	Editor = Context->Editor;
	PhoneList = Context->CurrentList;
	Count = Context->PhoneHandle->DialMarker;
	Grouper = Context->PhoneHandle->DefaultGroup;
	Hide = Context->Hide;
	Node = Context->SelectedNode;

	if(Node)
	{
		IsFirst		= (Node == (struct PhoneNode *)PhoneList->lh_Head);
		IsLast		= (Node == (struct PhoneNode *)PhoneList->lh_TailPred);
		Disabled	= FALSE;
	}
	else
	{
		IsFirst		= TRUE;
		IsLast		= TRUE;
		Disabled	= TRUE;
	}

	LT_SetAttributes(Manager,GAD_COMMENTLINE,
		LABX_Text,		Node ? Node->Entry->Header->Comment : (STRPTR)"",
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MOVE_FIRST,
		GA_Disabled,	IsFirst || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MOVE_UP,
		GA_Disabled,	IsFirst || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MOVE_DOWN,
		GA_Disabled,	IsLast || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MOVE_LAST,
		GA_Disabled,	IsLast || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_EDIT,
		GA_Disabled,	!Node || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_COPY,
		GA_Disabled,	!Node || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_DELETE,
		GA_Disabled,	!Node || Disabled || Node->Entry == GetActiveEntry(Context->PhoneHandle),
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_USE,
		GA_Disabled,	!Node || Disabled,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MAKE_GROUP,
		GA_Disabled,	!Node && !Count,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SPLIT_GROUP,
		GA_Disabled,	Grouper == 0,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SELECT,
		GA_Disabled,	Disabled || (Node && Node->Entry->Count != -1),
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_CLEAR,
		GA_Disabled,	Disabled || (Node && Node->Entry->Count == -1),
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_TOGGLE_ALL,
		GA_Disabled,	Context->NumEntries == 0,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SELECT_ALL,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries || Count == Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_CLEAR_ALL,
		GA_Disabled,	!Count,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_PATTERN,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_DIAL,
		GA_Disabled,	(Online && !Config->MiscConfig->ProtectiveMode) || (!Node && Context->PhoneHandle->DialMarker == 0) || (Node && Context->PhoneHandle->DialMarker == 0 && !(Node->Entry->Header->Number[0] || Node->Entry->Header->Name[0])),
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_LOAD,
		GA_Disabled,	GetActiveEntry(Context->PhoneHandle) != NULL,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_MERGE,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SAVE,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_PRINT,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SORT,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_COPY_CONFIG,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_SORT_ORDER,
		GA_Disabled,	!Context->PhoneHandle->NumPhoneEntries,
	TAG_DONE);

	LT_SetAttributes(Manager,GAD_GROUPID,
		GTCY_Active,	Grouper,
	TAG_DONE);

	if(Editor)
	{
		struct Configuration *LocalConfig;
		BOOL RatesValid;

		if(Node)
		{
			IsFirst			= (Node->Entry->Count == -1) || (Node->Entry->Count == 0);
			IsLast			= (Node->Entry->Count == -1) || (Node->Entry->Count == Count - 1);
			Disabled		= FALSE;
			LocalConfig		= Node->Entry->Config;
			RatesValid		= (!IsListEmpty(&Node->Entry->TimeDateList) && !Node->Entry->Header->NoRates);
		}
		else
		{
			IsFirst			= TRUE;
			IsLast			= TRUE;
			Disabled		= TRUE;
			LocalConfig		= NULL;
			RatesValid		= FALSE;
		}

		LT_SetAttributes(Editor,GAD_SYSTEM_NAME,
			GTST_String,	Node ? Node->Entry->Header->Name : (STRPTR)"",
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_COMMENT,
			GTST_String,	Node ? Node->Entry->Header->Comment : (STRPTR)"",
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_NUMBERS,
			GTST_String,	Node ? Node->Entry->Header->Number : (STRPTR)"",
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_QUICK_MENU,
			GTCB_Checked,	Node ? Node->Entry->Header->QuickMenu : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_AUTODIAL,
			GTCB_Checked,	Node ? Node->Entry->Header->AutoDial : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_USER_NAME,
			GTST_String,	(Node && !Hide) ? Node->Entry->Header->UserName : (STRPTR)"",
			GA_Disabled,	Hide || Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_USER_PASSWORD,
			GTST_String,	(Node && !Hide) ? Node->Entry->Header->Password : (STRPTR)"",
			GA_Disabled,	Hide || Disabled,
		TAG_DONE);


		LT_SetAttributes(Editor,GAD_SERIAL_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->SerialConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_SERIAL_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MODEM_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->ModemConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MODEM_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_SCREEN_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->ScreenConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_SCREEN_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TERMINAL_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->TerminalConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TERMINAL_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_EMULATION_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->EmulationConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_EMULATION_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CLIPBOARD_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->ClipConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CLIPBOARD_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CAPTURE_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->CaptureConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CAPTURE_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_COMMAND_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->CommandConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_COMMAND_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MISC_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->MiscConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MISC_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_PATH_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->PathConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_PATH_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TRANSFER_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->TransferConfig != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TRANSFER_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TRANSLATION_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->TranslationFileName != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_TRANSLATION_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MACRO_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->MacroFileName != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_MACRO_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CURSOR_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->CursorFileName != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_CURSOR_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_FASTMACRO_STATE,
			GTCB_Checked,	LocalConfig ? (LocalConfig->FastMacroFileName != NULL) : FALSE,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_FASTMACRO_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_RATE_STATE,
			GTCB_Checked,	RatesValid,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_RATE_EDIT,
			GA_Disabled,	Disabled,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_FIRST_ENTRY,
			GA_Disabled,	Disabled || IsFirst,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_PREVIOUS_ENTRY,
			GA_Disabled,	Disabled || IsFirst,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_NEXT_ENTRY,
			GA_Disabled,	Disabled || IsLast,
		TAG_DONE);

		LT_SetAttributes(Editor,GAD_LAST_ENTRY,
			GA_Disabled,	Disabled || IsLast,
		TAG_DONE);

		if(Activate)
		{
			LT_ShowWindow(Editor,TRUE);

			LT_Activate(Editor,GAD_SYSTEM_NAME);
		}
	}
}

STATIC VOID
UpdateWindowTitle(PhoneListContext *Context)
{
	CONST_STRPTR Title;

	if(Context->PhoneHandle->DialMarker > 0) {
		STRPTR Temp;
		LimitedSPrintf(sizeof(Context->WindowTitle),Temp = Context->WindowTitle,LocaleString(MSG_PHONEPANEL_SELECTED_OUT_OF_TXT),LocaleString(MSG_PHONEPANEL_PHONEBOOK_TXT),Context->PhoneHandle->DialMarker,Context->PhoneHandle->NumPhoneEntries);
		Title = Temp;
	}
	else
		Title = LocaleString(MSG_PHONEPANEL_PHONEBOOK_TXT);

	SetWindowTitles(Context->Window,Title,(STRPTR)~0);
}

STATIC VOID
UpdateEditorData(PhoneListContext *Context)
{
	if(Context->Editor && Context->SelectedNode)
	{
		PhoneHeader		*Header	= Context->SelectedNode->Entry->Header;
		LayoutHandle	*Editor	= Context->Editor;

		strcpy(Header->Name,	LT_GetString(Editor,GAD_SYSTEM_NAME));
		strcpy(Header->Comment,	LT_GetString(Editor,GAD_COMMENT));
		strcpy(Header->Number,	LT_GetString(Editor,GAD_NUMBERS));

		if(!Context->Hide)
		{
			strcpy(Header->UserName,LT_GetString(Editor,GAD_USER_NAME));
			strcpy(Header->Password,LT_GetString(Editor,GAD_USER_PASSWORD));
		}

		UpdatePhoneNode(Context->SelectedNode);
		UpdateActiveEntry(Context);
	}
}

STATIC PhoneGroupNode *
GetActiveGroup(PhoneListContext *Context)
{
	if(Context->PhoneHandle->DefaultGroup > 0)
	{
		CONST_STRPTR Name;

		Name = Context->GroupLabels[Context->PhoneHandle->DefaultGroup];

		return((PhoneGroupNode *)((IPTR)Name - offsetof(PhoneGroupNode,LocalName)));
	}
	else
		return(NULL);
}

STATIC VOID
DeletePhoneListContext(PhoneListContext *Context)
{
	DeletePhoneList(Context->PhoneHandle);
	FreeVecPooled(Context->GroupLabels);
	DeleteMsgPort(Context->WindowPort);
	FreeVecPooled(Context);
}

STATIC PhoneListContext *
CreatePhoneListContext(PhonebookHandle *PhoneHandle)
{
	PhoneListContext *Context;

	if(Context = (PhoneListContext *)AllocVecPooled(sizeof(*Context),MEMF_ANY|MEMF_CLEAR))
	{
			/* Create the message port */

		if(Context->WindowPort = CreateMsgPort())
		{
				/* Create the group labels */

			if(Context->GroupLabels = BuildLabels(PhoneHandle))
			{
				LONG i;

					/* Count the number of labels */

				for(i = 0 ; Context->GroupLabels[i] != NULL ; i++);

					/* If the group index is larger than the group label table */
					/* permits, truncate it. */

				if(PhoneHandle->DefaultGroup >= i)
					PhoneHandle->DefaultGroup = i - 1;

				Context->PhoneHandle	= PhoneHandle;
				Context->SelectedIndex	= -1;
				Context->Hide			= TRUE;

					/* Preset the window title */

				strcpy(Context->WindowTitle,LocaleString(MSG_PHONEPANEL_PHONEBOOK_TXT));

				if(InitPhoneList(PhoneHandle))
				{
						/* Update the window title */

					if(Context->PhoneHandle->DialMarker > 0)
						LimitedSPrintf(sizeof(Context->WindowTitle),Context->WindowTitle,LocaleString(MSG_PHONEPANEL_SELECTED_OUT_OF_TXT),LocaleString(MSG_PHONEPANEL_PHONEBOOK_TXT),Context->PhoneHandle->DialMarker,Context->PhoneHandle->NumPhoneEntries);

						/* Are we to display a group list? */

					if(PhoneHandle->DefaultGroup > 0)
					{
						PhoneGroupNode *GroupNode;

						GroupNode = GetActiveGroup(Context);

						Context->CurrentList	= (struct List *)&GroupNode->GroupList;
						Context->NumEntries		= GetListSize(Context->CurrentList);
					}
					else
					{
						Context->CurrentList	= PhoneHandle->PhoneList;
						Context->NumEntries		= PhoneHandle->NumPhoneEntries;
					}

					return(Context);
				}

				FreeVecPooled(Context->GroupLabels);
			}

			DeleteMsgPort(Context->WindowPort);
		}

		FreeVecPooled(Context);
	}

	return(NULL);
}

STATIC VOID
SelectActiveEntry(PhoneListContext *Context,LONG Index,BOOL ShowIt)
{
	PhoneNode *Node;

	if(Context->PhoneHandle->DefaultGroup > 0)
		Node = (PhoneNode *)GetListNode(Index,Context->CurrentList);
	else
	{
		if(Index >= 0 && Index < Context->PhoneHandle->NumPhoneEntries)
			Node = Context->PhoneHandle->Phonebook[Index]->Node;
		else
			Node = NULL;
	}

	if(Node)
	{
		Context->SelectedNode	= Node;
		Context->SelectedIndex	= Index;

		if(ShowIt)
		{
			LT_SetAttributes(Context->Manager,GAD_NAMELIST,
				LALV_Selected,Context->SelectedIndex,
			TAG_DONE);
		}

		HighlightActiveEntry(Context,FALSE);
	}
}

STATIC VOID
DetachList(PhoneListContext *Context)
{
	LT_SetAttributes(Context->Manager,GAD_NAMELIST,
		GTLV_Labels,~0,
	TAG_DONE);
}

STATIC VOID
AttachList(PhoneListContext *Context)
{
	LT_SetAttributes(Context->Manager,GAD_NAMELIST,
		GTLV_Labels,	Context->CurrentList,
		GTLV_Selected,	Context->SelectedIndex,
		GA_Disabled,	IsListEmpty(Context->CurrentList),
	TAG_DONE);
}

STATIC VOID
UpdateActiveEntry(PhoneListContext *Context)
{
	if(Context->SelectedNode)
	{
		if(!Context->SelectedNode->Entry->Header->Name[0] || !Context->SelectedNode->Entry->Header->Number[0])
			UnmarkDialEntry(Context->PhoneHandle,Context->SelectedNode->Entry);

		AttachList(Context);

		HighlightActiveEntry(Context,FALSE);
	}
}

STATIC VOID
ChangeActiveEntry(PhoneListContext *Context,LONG How)
{
	if(Context->SelectedNode)
	{
		VOID (*Work)(PhonebookHandle *,PhoneEntry *);

		DetachList(Context);

		switch(How)
		{
			case CHANGE_Set:

				Work = MarkDialEntry;
				break;

			case CHANGE_Clear:

				Work = UnmarkDialEntry;
				break;

			case CHANGE_Toggle:

				Work = ToggleDialEntry;
				break;

			default:
				Work = NULL;
				break;
		}

		if (Work)
			(*Work)(Context->PhoneHandle,Context->SelectedNode->Entry);

		UpdateWindowTitle(Context);

		AttachList(Context);

		HighlightActiveEntry(Context,FALSE);
	}
}

STATIC VOID
ToggleAllEntries(PhoneListContext *Context)
{
	PhoneNode *Node;

	DetachList(Context);

	for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
		ToggleDialEntry(Context->PhoneHandle,Node->Entry);

	UpdateWindowTitle(Context);

	AttachList(Context);

	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SelectAllEntries(PhoneListContext *Context)
{
	PhoneNode *Node;

	DetachList(Context);

	for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
		MarkDialEntry(Context->PhoneHandle,Node->Entry);

	UpdateWindowTitle(Context);

	AttachList(Context);

	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
ClearAllEntries(PhoneListContext *Context)
{
	PhoneNode *Node;

	DetachList(Context);

	for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
		UnmarkDialEntry(Context->PhoneHandle,Node->Entry);

	UpdateWindowTitle(Context);

	AttachList(Context);

	Context->SelectedIndex	= -1;
	Context->SelectedNode	= NULL;

	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SelectIfMarked(PhoneListContext *Context)
{
	LONG HowManyMarked;
	PhoneNode *Node;

	HowManyMarked = 0;

	for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
	{
		if(Node->Entry->Header->Marked > 0)
			HowManyMarked++;
	}

	if(HowManyMarked > 0)
	{
		PhoneNode *Which;
		LONG i,Counter;

		DetachList(Context);

		for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
			UnmarkDialEntry(Context->PhoneHandle,Node->Entry);

		for(i = 0 ; i < HowManyMarked ; i++)
		{
			Which	= NULL;
			Counter	= -1;

			for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
			{
				if(Node->Entry->Header->Marked > 0)
				{
					if(Counter == -1 || Node->Entry->Header->Marked <= Counter)
					{
						Counter	= Node->Entry->Header->Marked;
						Which	= Node;
					}
				}
			}

			if(Which)
			{
				MarkDialEntry(Context->PhoneHandle,Which->Entry);
				Which->Entry->Header->Marked = 0;
			}
		}

		UpdateWindowTitle(Context);

		AttachList(Context);

		HighlightActiveEntry(Context,FALSE);
	}
}

STATIC VOID
DeleteActiveEntry(PhoneListContext *Context)
{
	if(Context->SelectedNode)
	{
		if(Context->SelectedNode->Entry == GetActiveEntry(Context->PhoneHandle))
			ShowRequest(Context->Window,LocaleString(MSG_PHONEPANEL_PHONE_ENTRY_IN_USE_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
		else
		{
			PhoneNode *NextNode;
			LONG NextIndex;

			if(Context->NumEntries == 1)
			{
				NextNode	= NULL;
				NextIndex	= -1;
			}
			else
			{
				if(Context->SelectedIndex < Context->NumEntries - 1)
				{
					NextNode	= (PhoneNode *)Context->SelectedNode->Node.ln_Succ;
					NextIndex	= Context->SelectedIndex;
				}
				else
				{
					NextNode	= (PhoneNode *)Context->SelectedNode->Node.ln_Pred;
					NextIndex	= Context->SelectedIndex - 1;
				}
			}

			DetachList(Context);

				/* This is for keeping the dial list halfway intact. */

			if(Context->SelectedNode->Entry->DialNode)
				RemoveAndDeleteRelatedDialNodes(Context->PhoneHandle,Context->SelectedNode->Entry->DialNode);

			if(Context->PhoneHandle->DefaultGroup > 0)
			{
				RemoveGroupEntry(Context->SelectedNode->Entry);
				DeleteGroupEntry(Context->SelectedNode->Entry);
			}
			else
			{
				RemovePhoneEntry(Context->PhoneHandle,Context->SelectedNode->Entry);
				DeletePhoneEntry(Context->SelectedNode->Entry);
			}

			Context->NumEntries--;

			Context->SelectedNode	= NextNode;
			Context->SelectedIndex	= NextIndex;

			AttachList(Context);

			HighlightActiveEntry(Context,FALSE);

			if(!Context->NumEntries)
				CloseSomeWindow(Context,Context->GroupWindow);

			UpdateWindowTitle(Context);
		}
	}
}

STATIC VOID
CopyActiveEntry(PhoneListContext *Context)
{
	if(Context->SelectedNode)
	{
		PhoneEntry *Entry;

		UpdateEditorData(Context);

		if(Entry = ClonePhoneEntry(Context->PhoneHandle,Context->SelectedNode->Entry))
		{
			DetachList(Context);

				/* Add the entry to the list */

			if(AddPhoneEntry(Context->PhoneHandle,Entry))
			{
					/* Are we displaying a group? */

				if(Context->PhoneHandle->DefaultGroup)
				{
						/* Link this entry into the group */

					if(!AddGroupEntry(GetActiveGroup(Context),Entry))
					{
						RemovePhoneEntry(Context->PhoneHandle,Entry);
						DeletePhoneEntry(Entry);

						Entry = NULL;
					}
				}
			}

			if(Entry)
			{
					/* Grab the new node */

				Context->SelectedNode	= Entry->Node;
				Context->SelectedIndex	= Context->NumEntries++;

				AttachList(Context);

					/* Open the editor window so the user will be able to edit the new entry */

				if(!Context->Editor)
				{
					if(Context->Editor = CreateEditorHandle(Context,TRUE))
						Context->EditWindow = Context->Editor->Window;
				}

				HighlightActiveEntry(Context,TRUE);

				UpdateWindowTitle(Context);

				PhonebookChanged = TRUE;
				RebuildMenu = TRUE;
				ActivateJob(MainJobQueue,RebuildMenuJob);
			}
			else
				AttachList(Context);
		}
	}
}

STATIC VOID
LoadNewPhonebook(PhoneListContext *Context)
{
		/* If an entry is still in use, don't let the user replace */
		/* the phonebook */

	if(GetActiveEntry(Context->PhoneHandle))
		ShowRequest(Context->Window,LocaleString(MSG_PHONEPANEL_PHONE_ENTRY_IN_USE_LOAD_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
	else
	{
		UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
		struct FileRequester *FileRequest;
		PhonebookHandle *LocalPhoneHandle;

		strcpy(DummyBuffer,LastPhone);

		if(FileRequest = OpenSingleFile(Context->Window,LocaleString(MSG_PHONEPANEL_LOAD_PHONEBOOK_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
		{
			FreeAslRequest(FileRequest);

			if(!(LocalPhoneHandle = LoadPhonebook(DummyBuffer)))
				ShowError(Context->Window,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
		}
		else
			LocalPhoneHandle = NULL;

		if(LocalPhoneHandle)
		{
			CONST_STRPTR *OtherLabels;

				/* The group labels need to be rebuilt. */

			if(OtherLabels = BuildLabels(LocalPhoneHandle))
			{
				if(InitPhoneList(LocalPhoneHandle))
				{
					DetachList(Context);

					CloseSomeWindow(Context,Context->GroupWindow);

					LT_SetAttributes(Context->Manager,GAD_GROUPID,
						GTCY_Active,	LocalPhoneHandle->DefaultGroup,
						GTCY_Labels,	OtherLabels,
					TAG_DONE);

					FreeVecPooled(Context->GroupLabels);
					Context->GroupLabels = OtherLabels;

					if(GlobalPhoneHandle == Context->PhoneHandle)
						GlobalPhoneHandle = LocalPhoneHandle;

					DeletePhonebook(Context->PhoneHandle);

					Context->PhoneHandle = LocalPhoneHandle;
					Context->GroupLabels = OtherLabels;

					if(Context->PhoneHandle->DefaultGroup > 0)
					{
						PhoneGroupNode *Group;

							/* Grab the list we wanted */

						Group = GetActiveGroup(Context);

						Context->CurrentList = (struct List *)&Group->GroupList;
						Context->NumEntries = GetListSize(Context->CurrentList);
					}
					else
					{
						Context->CurrentList = LocalPhoneHandle->PhoneList;
						Context->NumEntries = LocalPhoneHandle->NumPhoneEntries;
					}

					Context->SelectedIndex	= -1;
					Context->SelectedNode	= NULL;

					LT_SetAttributes(Context->Manager,GAD_MAIN_AUTODIAL,
						GTCB_Checked,	Context->PhoneHandle->AutoDial,
					TAG_DONE);

					LT_SetAttributes(Context->Manager,GAD_MAIN_EXIT_WHEN_FINISHED,
						GA_Disabled,	!Context->PhoneHandle->AutoDial,
						GTCB_Checked,	Context->PhoneHandle->AutoExit,
					TAG_DONE);

					AttachList(Context);

					UpdateWindowTitle(Context);

					strcpy(LastPhone,DummyBuffer);
					strcpy(Config->PhonebookFileName,LastPhone);

					PhonebookChanged = FALSE;
					RebuildMenu = TRUE;
					ActivateJob(MainJobQueue,RebuildMenuJob);

					HighlightActiveEntry(Context,FALSE);

					SelectIfMarked(Context);

					OtherLabels = NULL;
					LocalPhoneHandle = NULL;
				}

				FreeVecPooled(OtherLabels);
			}

			DeletePhonebook(LocalPhoneHandle);
		}
	}
}

STATIC VOID
MergeNewPhonebook(PhoneListContext *Context)
{
	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester *FileRequest;
	PhonebookHandle *LocalPhoneHandle;

	strcpy(DummyBuffer,LastPhone);

	if(FileRequest = OpenSingleFile(Context->Window,LocaleString(MSG_PHONEPANEL_LOAD_PHONEBOOK_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
	{
		FreeAslRequest(FileRequest);

		if(!(LocalPhoneHandle = LoadPhonebook(DummyBuffer)))
			ShowError(Context->Window,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
	}
	else
		LocalPhoneHandle = NULL;

	if(LocalPhoneHandle)
	{
		if(InitPhoneList(LocalPhoneHandle))
		{
			DetachList(Context);

			MergePhonebooks(LocalPhoneHandle,Context->PhoneHandle);

			AttachList(Context);

			if(Context->PhoneHandle->DefaultGroup == 0)
				Context->NumEntries = GetListSize(Context->CurrentList);

			UpdateWindowTitle(Context);

			PhonebookChanged = TRUE;
			RebuildMenu = TRUE;
			ActivateJob(MainJobQueue,RebuildMenuJob);
		}

		DeletePhonebook(LocalPhoneHandle);
	}
}

STATIC VOID
ChangeGroup(PhoneListContext *Context)
{
	STRPTR Name;
	LONG Count;

	Count = Context->PhoneHandle->DialMarker;

		/* Which group should the tagged entries go into? */

	Name = LT_GetString(Context->Grouping,GAD_GROUP_NAME);

	DetachList(Context);

		/* If it's "All", then the entries will be removed from the */
		/* subgroups and end up in the main group */

	if(!Stricmp(Name,Context->GroupLabels[0]))
	{
		PhoneNode *Node,*Next;

		for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Next = (PhoneNode *)Node->Node.ln_Succ ; Node = Next)
		{
			if(Node->Entry->ThisGroup && Node->Entry->Count >= 0)
			{
				RemoveGroupEntry(Node->Entry);
				DeleteGroupEntry(Node->Entry);
			}
		}
	}
	else
	{
		PhoneGroupNode *WhichGroup,*GroupNode;
		BOOL RenameGroup,Success;

		WhichGroup	= NULL;
		RenameGroup	= TRUE,
		Success		= TRUE;

			/* Check if the requested group already exists. In this */
			/* case, the user wants to move entries from one group */
			/* to another. */

		for(GroupNode = (PhoneGroupNode *)Context->PhoneHandle->PhoneGroupList.mlh_Head ; GroupNode->Node.ln_Succ ; GroupNode = (PhoneGroupNode *)GroupNode->Node.ln_Succ)
		{
			if(!Stricmp(GroupNode->Node.ln_Name,Name))
			{
				RenameGroup = FALSE;
				break;
			}
		}

			/* Are we displaying a group list at all? */

		if(Context->PhoneHandle->DefaultGroup > 0)
		{
			struct PhoneNode *SomeNode;

			for(SomeNode = (struct PhoneNode *)Context->CurrentList->lh_Head ; SomeNode->Node.ln_Succ ; SomeNode = (struct PhoneNode *)SomeNode->Node.ln_Succ)
			{
				if(SomeNode->Entry->Count >= 0)
				{
					WhichGroup = SomeNode->Entry->ThisGroup;
					break;
				}
			}

				/* If there is no group to play with, don't rename it */

			if(!WhichGroup)
				RenameGroup = FALSE;
		}
		else
			RenameGroup = FALSE;

			/* Check if the marked entries are already */
			/* members of the same group */

		if(RenameGroup)
		{
			PhoneEntry **Phonebook = Context->PhoneHandle->Phonebook;
			LONG i,NotGrouped = 0,InGroup = 0;

			for(i = 0 ; RenameGroup && i < Context->PhoneHandle->NumPhoneEntries ; i++)
			{
				if(Phonebook[i]->Count != -1)
				{
					if(Phonebook[i]->ThisGroup)
					{
						if(NotGrouped)
							RenameGroup = FALSE;
						else
						{
							if(Phonebook[i]->ThisGroup == WhichGroup)
								InGroup++;
							else
								RenameGroup = FALSE;
						}
					}
					else
					{
						if(WhichGroup)
							RenameGroup = FALSE;
						else
							NotGrouped++;
					}
				}
				else
				{
					if(Phonebook[i]->ThisGroup == WhichGroup)
						InGroup++;
				}
			}

				/* If we should continue, check if there are */
				/* any group members at all and if all members */
				/* of the group are selected. */

			if(NotGrouped || InGroup != Count)
				RenameGroup = FALSE;
		}

		if(RenameGroup)
		{
			strcpy(WhichGroup->LocalName,Name);

			GroupNode = WhichGroup;
		}
		else
		{
				/* Either find an existing group of the given name */
				/* or make a new group. */

			if(GroupNode = CreatePhoneGroup(Context->PhoneHandle,Name))
			{
				PhoneNode	*Node,*Next;
				LONG		 Total = 0;

					/* Add the tagged entries to the group */

				for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Next = (PhoneNode *)Node->Node.ln_Succ ; Node = Next)
				{
					if(Node->Entry->Count != -1)
					{
						AddGroupEntry(GroupNode,Node->Entry);

						Total++;
					}
				}

				if(!Total && Context->SelectedNode)
				{
					if(!AddGroupEntry(GroupNode,Context->SelectedNode->Entry))
						Success = FALSE;
				}
			}
			else
				Success = FALSE;
		}

		if(Success)
		{
			CONST_STRPTR *OtherLabels;

				/* Now update the labels */

			if(OtherLabels = BuildLabels(Context->PhoneHandle))
			{
				PhoneGroupNode *This;
				LONG GroupCounter;

				LT_SetAttributes(Context->Manager,GAD_GROUPID,
					GTCY_Labels,OtherLabels,
				TAG_DONE);

				FreeVecPooled(Context->GroupLabels);
				Context->GroupLabels = OtherLabels;

				Context->CurrentList = (struct List *)&GroupNode->GroupList;

				for(This = (PhoneGroupNode *)Context->PhoneHandle->PhoneGroupList.mlh_Head, GroupCounter = 1 ; This->Node.ln_Succ ; This = (PhoneGroupNode *)This->Node.ln_Succ, GroupCounter++)
				{
					if(This == GroupNode)
						break;
				}

				Context->PhoneHandle->DefaultGroup = GroupCounter;

				if(Context->GroupWindow)
				{
					LT_SetAttributes(Context->Grouping,GAD_GROUP_LIST,
						GTCY_Labels,	Context->GroupLabels,
						GTCY_Active,	Context->PhoneHandle->DefaultGroup,
					TAG_DONE);
				}

				LT_SetAttributes(Context->Manager,GAD_GROUPID,
					GTCY_Labels,	Context->GroupLabels,
					GTCY_Active,	Context->PhoneHandle->DefaultGroup,
				TAG_DONE);

				PhonebookChanged = TRUE;
				RebuildMenu = TRUE;
				ActivateJob(MainJobQueue,RebuildMenuJob);
			}
		}
	}

	Context->SelectedIndex	= -1;
	Context->SelectedNode	= NULL;

	AttachList(Context);
	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SelectGroup(PhoneListContext *Context,LONG Index)
{
	Context->SelectedIndex	= -1;
	Context->SelectedNode	= NULL;

	DetachList(Context);

	Context->PhoneHandle->DefaultGroup = Index;

		/* Special group? */

	if(Context->PhoneHandle->DefaultGroup > 0)
	{
		PhoneGroupNode *Group;

			/* Grab the list we wanted */

		Group = GetActiveGroup(Context);

		Context->CurrentList	= (struct List *)&Group->GroupList;
		Context->NumEntries		= GetListSize(Context->CurrentList);
	}
	else
	{
		if(Context->PhoneHandle->PhoneList)
		{
			Context->CurrentList	= Context->PhoneHandle->PhoneList;
			Context->NumEntries		= Context->PhoneHandle->NumPhoneEntries;
		}
		else
		{
			STATIC struct List EmptyList;

			NewList(&EmptyList);

			Context->CurrentList	= &EmptyList;
			Context->NumEntries		= 0;
		}
	}

	AttachList(Context);
	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SplitGroup(PhoneListContext *Context)
{
	PhoneGroupNode *PhoneGroup;
	CONST_STRPTR *OtherLabels;

	DetachList(Context);

	PhoneGroup = GetActiveGroup(Context);

	Remove((struct Node *)PhoneGroup);

	if(OtherLabels = BuildLabels(Context->PhoneHandle))
	{
		CloseSomeWindow(Context,Context->GroupWindow);

		DeletePhoneGroupNode(PhoneGroup);

		Context->PhoneHandle->DefaultGroup = 0;

		Context->CurrentList	= Context->PhoneHandle->PhoneList;
		Context->NumEntries		= Context->PhoneHandle->NumPhoneEntries;

		Context->SelectedNode	= NULL;
		Context->SelectedIndex	= -1;

		LT_SetAttributes(Context->Manager,GAD_GROUPID,
			GTCY_Labels,OtherLabels,
			GTCY_Active,0,
		TAG_DONE);

		FreeVecPooled(Context->GroupLabels);
		Context->GroupLabels = OtherLabels;

		PhonebookChanged = TRUE;
	}
	else
		InsertSorted((struct List *)&Context->PhoneHandle->PhoneGroupList,(struct Node *)PhoneGroup);

	AttachList(Context);
	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SortThePhonebook(PhoneListContext *Context,LONG How,BOOL ReverseOrder)
{
	DetachList(Context);

	if(Context->PhoneHandle->DefaultGroup > 0)
		SortGroupList(Context->CurrentList,How,ReverseOrder);
	else
	{
		SortPhoneList(Context->PhoneHandle,How,ReverseOrder);
		UpdatePhoneList(Context->PhoneHandle);
	}

	Context->SelectedNode	= NULL;
	Context->SelectedIndex	= -1;

	PhonebookChanged = TRUE;
	RebuildMenu = TRUE;
	ActivateJob(MainJobQueue,RebuildMenuJob);

	AttachList(Context);
	HighlightActiveEntry(Context,FALSE);
}

STATIC VOID
SelectNewTaggedEntry(PhoneListContext *Context,LONG How)
{
	if(Context->SelectedNode)
	{
		PhoneNode *SelectedNode = Context->SelectedNode;

		if(SelectedNode->Entry->Count >= 0)
		{
			LONG i,Which = 0;
			BOOL ListIsReady;

			DetachList(Context);
			ListIsReady = FALSE;

			switch(How)
			{
				case MOVE_HEAD:

					Which = 0;
					break;

				case MOVE_PRED:

					Which = SelectedNode->Entry->Count - 1;
					break;

				case MOVE_SUCC:

					Which = SelectedNode->Entry->Count + 1;
					break;

				case MOVE_TAIL:

					Which = Context->PhoneHandle->DialMarker - 1;
					break;
			}

			if(Which >= 0 && Which < Context->PhoneHandle->DialMarker)
			{
				PhoneEntry **Phonebook = Context->PhoneHandle->Phonebook;

				for(i = 0 ; i < Context->PhoneHandle->NumPhoneEntries ; i++)
				{
					if(Phonebook[i]->Node->Entry->Count == Which)
					{
						LONG NodeIndex;

						UpdateEditorData(Context);

						SelectedNode = Phonebook[i]->Node;

						if(SelectedNode->Entry->GroupNode)
						{
							PhoneGroupNode *GroupNode;
							LONG GroupIndex;

							SelectedNode = SelectedNode->Entry->GroupNode;

							FindGroup((struct List *)&Context->PhoneHandle->PhoneGroupList,&GroupIndex,&NodeIndex,SelectedNode);

							Context->PhoneHandle->DefaultGroup = GroupIndex;

							GroupNode = GetActiveGroup(Context);

							Context->CurrentList	= (struct List *)&GroupNode->GroupList;
							Context->NumEntries		= GetListSize(Context->CurrentList);
						}
						else
						{
							Context->PhoneHandle->DefaultGroup = 0;

							Context->CurrentList	= Context->PhoneHandle->PhoneList;
							Context->NumEntries		= Context->PhoneHandle->NumPhoneEntries;

							NodeIndex = i;
						}

						Context->SelectedNode	= SelectedNode;
						Context->SelectedIndex	= NodeIndex;

						AttachList(Context);
						ListIsReady = TRUE;

						HighlightActiveEntry(Context,FALSE);

						break;
					}
				}
			}

			if(!ListIsReady)
				AttachList(Context);
		}
	}
}

STATIC VOID
MoveActiveEntry(PhoneListContext *Context,LONG How)
{
	if(Context->SelectedNode != NULL)
	{
		DetachList(Context);

		if(Context->PhoneHandle->DefaultGroup == 0)
			Context->SelectedIndex = MovePhoneEntry(Context->PhoneHandle,Context->SelectedIndex,How);
		else
		{
			LONG NewPosition = 0;

			switch(How)
			{
				case MOVE_HEAD:

					NewPosition = 0;
					break;

				case MOVE_PRED:

					NewPosition = Context->SelectedIndex - 1;
					break;

				case MOVE_SUCC:

					NewPosition = Context->SelectedIndex + 1;
					break;

				case MOVE_TAIL:

					NewPosition = Context->NumEntries - 1;
					break;
			}

			if(NewPosition >= 0 && NewPosition < Context->NumEntries)
			{
				MoveNode(Context->CurrentList,(struct Node *)Context->SelectedNode,How);

				Context->SelectedIndex = NewPosition;
			}
		}

		AttachList(Context);

		HighlightActiveEntry(Context,FALSE);
	}
}

STATIC VOID
AddNewEntry(PhoneListContext *Context)
{
	PhoneEntry *Entry;

	UpdateEditorData(Context);

	DetachList(Context);

		/* Create the new entry */

	if(Entry = CreatePhoneEntry(Context->PhoneHandle))
	{
			/* Add the entry to the list */

		if(AddPhoneEntry(Context->PhoneHandle,Entry))
		{
				/* Are we displaying a group? */

			if(Context->PhoneHandle->DefaultGroup)
			{
				PhoneGroupNode *Group;

					/* Link this entry into the group */

				Group = GetActiveGroup(Context);

				if(!AddGroupEntry(Group,Entry))
				{
					RemovePhoneEntry(Context->PhoneHandle,Entry);
					DeletePhoneEntry(Entry);

					Entry = NULL;
				}
			}
		}
		else
		{
			DeletePhoneEntry(Entry);

			Entry = NULL;
		}
	}

	if(Entry)
	{
			/* Grab the new node */

		Context->SelectedNode	= Entry->Node;
		Context->SelectedIndex	= Context->NumEntries++;

		AttachList(Context);

			/* Open the editor window so the user can edit the new entry */

		if(!Context->Editor)
		{
			if(Context->Editor = CreateEditorHandle(Context,TRUE))
				Context->EditWindow = Context->Editor->Window;
		}

		HighlightActiveEntry(Context,TRUE);
		UpdateWindowTitle(Context);

		PhonebookChanged = TRUE;
		RebuildMenu = TRUE;
		ActivateJob(MainJobQueue,RebuildMenuJob);
	}
	else
		AttachList(Context);
}

BOOL
PhonePanel(ULONG InitialQualifier)
{
	STATIC UBYTE	NamePattern[64]		= "#?",
					NumberPattern[64]	= "#?",
					CommentPattern[64]	= "#?";
	STATIC BYTE		Mode = 1;

	STATIC WORD		Criteria;
	STATIC BOOL		ReverseOrder;

	/************************************************************************/

	UBYTE				 ParsedNamePattern[2 * sizeof(NamePattern) + 2],
						 ParsedNumberPattern[2 * sizeof(NumberPattern) + 2],
						 ParsedCommentPattern[2 * sizeof(CommentPattern) + 2];
	PhoneListContext	*Context;
	UBYTE				 NewPassword[MAX_PASSWORD_LENGTH+1];
	PhoneNode			*Node;
	BOOL				 Changes;
	BOOL				 DialTheList = FALSE;

		/* Set up the list context */

	if(!(Context = CreatePhoneListContext(GlobalPhoneHandle)))
	{
		DisplayBeep(Window->WScreen);
		return(FALSE);
	}

		/* If the user holds down any shift key, the editor window will open in */
		/* addition to the manager window */

	if(InitialQualifier & SHIFT_KEY)
	{
		if(Context->Editor = CreateEditorHandle(Context,FALSE))
			Context->EditWindow = Context->Editor->Window;
	}

		/* Don't go any further if the manager window is not open */

	if(Context->Manager = CreateManagerHandle(Context,Context->Editor ? Context->EditWindow : Window))
	{
		struct IntuiMessage		*Message;
		ULONG					 MsgClass,
								 MsgQualifier;
		UWORD					 MsgCode;
		LONG					 KeyCode;
		struct Gadget			*MsgGadget;
		LONG					 MsgGadgetID;
		struct Window			*MsgWindow;
		ULONG					 CompleteQualifier;
		LONG					 ClickAction;
		LayoutHandle			*Handle = NULL;
		BOOL					 Done = FALSE;

			/* We will need these pointers later */

		Context->Window = Context->Manager->Window;

		GuideContext(CONTEXT_PHONEBOOK);

		PushWindow(Context->Window);

			/* Select the entries that need to be selected. */

		SelectIfMarked(Context);

		do
		{
			if(Wait(PORTMASK(Context->WindowPort) | SIG_BREAK) & SIG_BREAK)
				break;

			while(Message = (struct IntuiMessage *)GT_GetIMsg(Context->WindowPort))
			{
				MsgClass		= Message->Class;
				MsgQualifier	= Message->Qualifier;
				MsgCode			= Message->Code;
				MsgGadget		= (struct Gadget *)Message->IAddress;
				MsgWindow		= Message->IDCMPWindow;

				GT_ReplyIMsg(Message);

					/* Jump to the handle the message originated from */

				if(MsgWindow == Context->Window)
					Handle = Context->Manager;

				if(MsgWindow == Context->EditWindow)
					Handle = Context->Editor;

				if(MsgWindow == Context->PatternWindow)
					Handle = Context->Selector;

				if(MsgWindow == Context->GroupWindow)
					Handle = Context->Grouping;

				if(MsgWindow == Context->SortWindow)
					Handle = Context->Sorting;

				if(MsgWindow == Context->PasswordWindow)
					Handle = Context->PasswordHandle;

					/* Save the original event qualifier */

				CompleteQualifier = MsgQualifier;

					/* Now, this is not nice, but needs to be done in order */
					/* to avoid having keyboard movements mess up the list of */
					/* tagged entries. */

				if(MsgClass == IDCMP_RAWKEY && (MsgCode >= CURSORUP && MsgCode <= CURSORDOWN))
					MsgQualifier &= ~SHIFT_KEY;

					/* We'll save the key code for the editing window */

				if(MsgClass == IDCMP_RAWKEY)
					KeyCode = MsgCode;
				else
					KeyCode = 0;

					/* Let gtlayout take its share */

				LT_HandleInput(Handle,CompleteQualifier,&MsgClass,&MsgCode,&MsgGadget);

					/* Extract the gadget ID */

				if(MsgClass == IDCMP_GADGETUP || MsgClass == IDCMP_IDCMPUPDATE)
					MsgGadgetID = MsgGadget->GadgetID;
				else
					MsgGadgetID = -1;

					/* Now convert the raw key code for special commands */

				switch(LT_GetCode(CompleteQualifier,MsgClass,MsgCode,MsgGadget))
				{
						/* Toggle the current entry */

					case ' ':

						ChangeActiveEntry(Context,CHANGE_Toggle);
						break;

						/* Untag this or all entries */

					case 127:

						if(CompleteQualifier & SHIFT_KEY)
						{
							if(Context->PhoneHandle->DialMarker > 0)
							{
								MsgClass	= IDCMP_GADGETUP;
								MsgGadgetID	= GAD_CLEAR_ALL;

								LT_PressButton(Handle,MsgGadgetID);
							}
						}
						else
						{
							if(Context->SelectedNode != NULL && Context->SelectedNode->Entry->Count != -1)
							{
								MsgClass	= IDCMP_GADGETUP;
								MsgGadgetID	= GAD_CLEAR;

								LT_PressButton(Handle,MsgGadgetID);
							}
						}

						break;
				}

					/* Moving around through the editing window */

				if(KeyCode != 0 && MsgWindow == Context->EditWindow)
				{
					switch(KeyCode)
					{
						case CURSORLEFT:

							MsgGadgetID = -1;

							if(Context->SelectedNode && Context->SelectedNode->Entry->Count > 0)
							{
								MsgClass = IDCMP_GADGETUP;

								if(CompleteQualifier & SHIFT_KEY)
									MsgGadgetID	= GAD_FIRST_ENTRY;
								else
									MsgGadgetID	= GAD_PREVIOUS_ENTRY;

								LT_PressButton(Handle,MsgGadgetID);
							}

							break;

						case CURSORRIGHT:

							MsgGadgetID = -1;

							if(Context->SelectedNode && Context->SelectedNode->Entry->Count < Context->PhoneHandle->DialMarker - 1)
							{
								MsgClass = IDCMP_GADGETUP;

								if(CompleteQualifier & SHIFT_KEY)
									MsgGadgetID	= GAD_LAST_ENTRY;
								else
									MsgGadgetID	= GAD_NEXT_ENTRY;

								LT_PressButton(Handle,MsgGadgetID);
							}

							break;
					}
				}

					/* Close a window */

				if(MsgClass == IDCMP_CLOSEWINDOW)
				{
					if(MsgWindow == Context->Window)
						Done = TRUE;
					else
						CloseSomeWindow(Context,MsgWindow);
				}

					/* Hit a gadget */

				if(MsgClass == IDCMP_GADGETUP)
				{
					switch(MsgGadgetID)
					{
							/* Toggle all entries */

						case GAD_TOGGLE_ALL:

							ToggleAllEntries(Context);
							break;

							/* Select all entries */

						case GAD_SELECT_ALL:

							SelectAllEntries(Context);
							break;

							/* Clear all selections in this group */

						case GAD_CLEAR_ALL:

							ClearAllEntries(Context);
							break;

							/* Picked a list entry */

						case GAD_NAMELIST:

							UpdateEditorData(Context);

							if(MsgQualifier & SHIFT_KEY)
								ClickAction = CLICK_Toggle;
							else
								ClickAction = CLICK_Select;

							if(MsgCode == Context->SelectedIndex)
							{
								if(ClickAction == CLICK_Toggle)
									ChangeActiveEntry(Context,CHANGE_Toggle);
							}
							else
							{
								SelectActiveEntry(Context,MsgCode,FALSE);

								if(ClickAction == CLICK_Toggle)
									ChangeActiveEntry(Context,CHANGE_Toggle);
							}

							break;

							/* Print the current group */

						case GAD_PRINT:

							LockInterface(Context);

							PrintPanel(Context->Window,Context->CurrentList,Context->PhoneHandle->DialMarker);

							UnlockInterface(Context);

							LT_ShowWindow(Handle,TRUE);

							break;

							/* Copy the setting stored with the current entry */
							/* into the global settings */

						case GAD_USE:

							if(Context->SelectedNode)
							{
								PhoneEntry *Entry = Context->SelectedNode->Entry;

								UpdateEditorData(Context);

								SaveConfig(Config,PrivateConfig);
								SaveConfig(Entry->Config,Config);

								strcpy(Password,Entry->Header->Password);
								strcpy(UserName,Entry->Header->UserName);

								ConfigSetup();

								Done = TRUE;

								ConfigChanged = FALSE;
							}

							break;

							/* Add a new phonebook entry to the group */

						case GAD_ADD:

							AddNewEntry(Context);
							break;

							/* Edit the current entry */

						case GAD_EDIT:

							UpdateEditorData(Context);

							if(Context->SelectedNode)
							{
								if(!Context->Editor)
								{
									if(Context->Editor = CreateEditorHandle(Context,TRUE))
										Context->EditWindow = Context->Editor->Window;
								}
								else
									HighlightActiveEntry(Context,TRUE);
							}

							break;

							/* Hide user name and password? */

						case GAD_HIDE_USERNAME_PASSWORD:

							if(Context->SelectedNode && !Context->Hide)
							{
								PhoneEntry *Entry = Context->SelectedNode->Entry;

								strcpy(Entry->Header->UserName,LT_GetString(Context->Editor,GAD_USER_NAME));
								strcpy(Entry->Header->Password,LT_GetString(Context->Editor,GAD_USER_PASSWORD));
							}

							Context->Hide = MsgCode;

							UpdateActiveEntry(Context);

							break;

							/* Change the system name; this is what shows up in the list */

						case GAD_SYSTEM_NAME:

							if(Context->SelectedNode)
							{
								DetachList(Context);

								strcpy(Context->SelectedNode->Entry->Header->Name,LT_GetString(Context->Editor,GAD_SYSTEM_NAME));
								UpdatePhoneNode(Context->SelectedNode);

								UpdateActiveEntry(Context);

								PhonebookChanged = TRUE;
								RebuildMenu = TRUE;
								ActivateJob(MainJobQueue,RebuildMenuJob);

								if(MsgCode == '\r')
									LT_Activate(Context->Editor,GAD_COMMENT);
							}

							break;

							/* Change the comment; this is what shows up directly below the list */

						case GAD_COMMENT:

							if(Context->SelectedNode)
							{
								PhoneEntry *Entry = Context->SelectedNode->Entry;

								strcpy(Entry->Header->Comment,LT_GetString(Context->Editor,GAD_COMMENT));

								PhonebookChanged = TRUE;

								if(MsgCode == '\r')
									LT_Activate(Handle,GAD_NUMBERS);

								LT_SetAttributes(Context->Manager,GAD_COMMENTLINE,
									GTTX_Text,	Entry->Header->Comment,
								TAG_DONE);
							}

							break;

							/* Put this entry into the quick dial menu or don't */

						case GAD_QUICK_MENU:

							if(Context->SelectedNode)
							{
								PhoneHeader *Header = Context->SelectedNode->Entry->Header;

								if(Header->QuickMenu != MsgCode)
								{
									Header->QuickMenu = MsgCode;

									PhonebookChanged = TRUE;
									RebuildMenu = TRUE;
									ActivateJob(MainJobQueue,RebuildMenuJob);
								}
							}

							break;

							/* Do the same for the autodial feature */

						case GAD_AUTODIAL:

							if(Context->SelectedNode)
								Context->SelectedNode->Entry->Header->AutoDial = MsgCode;

							break;

							/* Change the phone numbers for this entry */

						case GAD_NUMBERS:

							if(Context->SelectedNode)
							{
								PhoneEntry *Entry = Context->SelectedNode->Entry;

								strcpy(Entry->Header->Number,LT_GetString(Context->Editor,GAD_NUMBERS));

								PhonebookChanged = TRUE;

								UpdateActiveEntry(Context);
							}

							break;

							/* Change the user name */

						case GAD_USER_NAME:

							if(Context->SelectedNode)
							{
								strcpy(Context->SelectedNode->Entry->Header->UserName,LT_GetString(Context->Editor,GAD_USER_NAME));

								PhonebookChanged = TRUE;
							}

							break;

							/* Change the login password */

						case GAD_USER_PASSWORD:

							if(Context->SelectedNode)
							{
								strcpy(Context->SelectedNode->Entry->Header->Password,LT_GetString(Context->Editor,GAD_USER_PASSWORD));

								PhonebookChanged = TRUE;
							}

							break;

							/* Join the currently tagged entries in a new group */

						case GAD_MAKE_GROUP:

							if(Context->GroupWindow)
							{
								LT_ShowWindow(Context->Grouping,TRUE);
								LT_Activate(Context->Grouping,GAD_GROUP_NAME);
							}
							else
							{
								if(Context->Grouping = CreateGroupHandle(Context->Window,Context->GroupLabels,Context->PhoneHandle->DefaultGroup))
									Context->GroupWindow = Context->Grouping->Window;
							}

							break;

							/* Close the grouping window without doing anything */

						case GAD_GROUP_CANCEL:

							CloseSomeWindow(Context,Context->GroupWindow);
							break;

							/* Open the sort window */

						case GAD_SORT:

							if(Context->Sorting)
								LT_ShowWindow(Context->Sorting,TRUE);
							else
							{
								if(Context->Sorting = CreateSortHandle(Context->Window,&Criteria,&ReverseOrder))
									Context->SortWindow = Context->Sorting->Window;
								else
									DisplayBeep(Context->Window->WScreen);
							}

							break;

							/* Copy cofiguration data around */

						case GAD_COPY_CONFIG:

							LockInterface(Context);

							if(!Context->SelectedNode)
								Changes = CopyPanel(Context->Window,NULL,Context->PhoneHandle->DialMarker > 0);
							else
								Changes = CopyPanel(Context->Window,Context->SelectedNode->Entry->Config,Context->PhoneHandle->DialMarker > 0);

							if(Changes)
							{
								HighlightActiveEntry(Context,FALSE);

								PhonebookChanged = TRUE;
							}

							UnlockInterface(Context);

							LT_ShowWindow(Context->Manager,TRUE);

							break;

							/* Change or clear the password */

						case GAD_PASSWORD:

							if(Context->PasswordWindow)
								LT_ShowWindow(Context->PasswordHandle,TRUE);
							else
							{
								if(Context->PasswordHandle = CreatePasswordHandle(Context->Window,Context->PhoneHandle->PhonePasswordUsed))
									Context->PasswordWindow = Context->PasswordHandle->Window;
								else
									DisplayBeep(Context->Window->WScreen);
							}

							break;

							/* Changed the new password */

						case GAD_NEW_PASSWORD:
						case GAD_REPEAT_PASSWORD:

							UpdatePasswordWindow(Context->PasswordHandle);
							break;

						case GAD_CLEAR_PASSWORD:

							Context->PhoneHandle->PhonePasswordUsed = FALSE;

							PhonebookChanged = TRUE;

							CloseSomeWindow(Context,Context->PasswordWindow);

							break;

						case GAD_CANCEL_PASSWORD:

							CloseSomeWindow(Context,Context->PasswordWindow);
							break;

						case GAD_USE_NEW_PASSWORD:

							memset(NewPassword,0,sizeof(NewPassword));

							if(GetWindowPassword(Handle,NewPassword))
							{
								PhonebookChanged = TRUE;
								Context->PhoneHandle->PhonePasswordUsed = TRUE;

								Encrypt(NewPassword,MAX_PASSWORD_LENGTH,Context->PhoneHandle->PhonePassword,NewPassword,strlen(NewPassword));

								CloseSomeWindow(Context,Context->PasswordWindow);
							}

							break;

							/* Close the editor window */

						case GAD_CLOSE:

							DetachList(Context);
							UpdateEditorData(Context);
							HighlightActiveEntry(Context,TRUE);

							CloseSomeWindow(Context,Context->EditWindow);
							break;

							/* Hit the autodial button */

						case GAD_MAIN_AUTODIAL:

							LT_SetAttributes(Context->Manager,GAD_MAIN_EXIT_WHEN_FINISHED,
								GA_Disabled,	!Context->PhoneHandle->AutoDial,
							TAG_DONE);

							break;

							/* Make a copy of the current entry */

						case GAD_COPY:

							CopyActiveEntry(Context);
							break;

							/* Delete the current entry */

						case GAD_DELETE:

							DeleteActiveEntry(Context);
							break;

							/* Load a new phonebook */

						case GAD_LOAD:

							LockInterface(Context);

							LoadNewPhonebook(Context);

							UnlockInterface(Context);

							break;

							/* Merge existing phonebook with a new phonebook loaded from disk */

						case GAD_MERGE:

							UpdateEditorData(Context);

							LockInterface(Context);

							MergeNewPhonebook(Context);

							UnlockInterface(Context);

							break;

							/* Copy the selected group name into the text entry field */

						case GAD_GROUP_LIST:

							LT_SetAttributes(Context->Grouping,GAD_GROUP_NAME,
								GTST_String,Context->GroupLabels[MsgCode],
							TAG_DONE);

							break;

							/* Do something to the selected entries. */

						case GAD_GROUP_OK:

							LockInterface(Context);

							ChangeGroup(Context);

							UnlockInterface(Context);

							CloseSomeWindow(Context,Context->GroupWindow);

							break;

							/* Display a different group */

						case GAD_GROUPID:

							SelectGroup(Context,MsgCode);
							break;

							/* Get rid of this group */

						case GAD_SPLIT_GROUP:

							SplitGroup(Context);
							break;

							/* Sort the phonelist */

						case GAD_SORT_AND_CLOSE:
						case GAD_SORT_IT:

							UpdateEditorData(Context);

							if(Context->NumEntries > 1)
							{
								LockInterface(Context);

								SortThePhonebook(Context,Criteria,ReverseOrder);

								UnlockInterface(Context);
							}

							if(MsgGadgetID != GAD_SORT_IT)
								CloseSomeWindow(Context,Context->SortWindow);

							break;

						case GAD_SORT_JUST_CLOSE:

							CloseSomeWindow(Context,Context->SortWindow);
							break;

							/* Save the phonebook to disk */

						case GAD_SAVE:

							UpdateEditorData(Context);

							if(Context->PhoneHandle->NumPhoneEntries > 0)
							{
								UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
								struct FileRequester *FileRequest;

								LockInterface(Context);

								strcpy(DummyBuffer,LastPhone);

								if(FileRequest = SaveFile(Context->Window,LocaleString(MSG_PHONEPANEL_SAVE_PHONEBOOK_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!SavePhonebook(DummyBuffer,Context->PhoneHandle))
										ShowError(Context->Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastPhone,DummyBuffer);
										strcpy(Config->PhonebookFileName,LastPhone);

										PhonebookChanged = FALSE;
									}
								}

								UnlockInterface(Context);
							}

							break;

							/* Move to the first tagged entry */

						case GAD_FIRST_ENTRY:

							SelectNewTaggedEntry(Context,MOVE_HEAD);
							break;

							/* Move to the previous tagged entry */

						case GAD_PREVIOUS_ENTRY:

							SelectNewTaggedEntry(Context,MOVE_PRED);
							break;

							/* Move to the next tagged entry */

						case GAD_NEXT_ENTRY:

							SelectNewTaggedEntry(Context,MOVE_SUCC);
							break;

							/* Move to the next tagged entry */

						case GAD_LAST_ENTRY:

							SelectNewTaggedEntry(Context,MOVE_TAIL);
							break;

							/* Move entry to beginning of list */

						case GAD_MOVE_FIRST:

							MoveActiveEntry(Context,MOVE_HEAD);
							break;

							/* Move entry up in the list */

						case GAD_MOVE_UP:

							MoveActiveEntry(Context,MOVE_PRED);
							break;

							/* Move entry down in the list */

						case GAD_MOVE_DOWN:

							MoveActiveEntry(Context,MOVE_SUCC);
							break;

							/* Move entry to end of list */

						case GAD_MOVE_LAST:

							MoveActiveEntry(Context,MOVE_TAIL);
							break;

							/* Fiddle with the editor checkboxes */

						case GAD_SERIAL_STATE:
						case GAD_MODEM_STATE:
						case GAD_SCREEN_STATE:
						case GAD_TERMINAL_STATE:
						case GAD_EMULATION_STATE:
						case GAD_CLIPBOARD_STATE:
						case GAD_CAPTURE_STATE:
						case GAD_COMMAND_STATE:
						case GAD_MISC_STATE:
						case GAD_PATH_STATE:
						case GAD_TRANSFER_STATE:
						case GAD_TRANSLATION_STATE:
						case GAD_MACRO_STATE:
						case GAD_CURSOR_STATE:
						case GAD_FASTMACRO_STATE:
						case GAD_RATE_STATE:

							if(Context->SelectedNode != NULL)
							{
								if(ChangeState(MsgGadgetID,MsgCode,Context->SelectedNode))
								{
									HighlightActiveEntry(Context,FALSE);

									PhonebookChanged = TRUE;
								}
							}

							break;

							/* Edit the settings of the current entry */

						case GAD_SERIAL_EDIT:
						case GAD_MODEM_EDIT:
						case GAD_SCREEN_EDIT:
						case GAD_TERMINAL_EDIT:
						case GAD_EMULATION_EDIT:
						case GAD_CLIPBOARD_EDIT:
						case GAD_CAPTURE_EDIT:
						case GAD_COMMAND_EDIT:
						case GAD_MISC_EDIT:
						case GAD_PATH_EDIT:
						case GAD_TRANSFER_EDIT:
						case GAD_TRANSLATION_EDIT:
						case GAD_MACRO_EDIT:
						case GAD_CURSOR_EDIT:
						case GAD_FASTMACRO_EDIT:
						case GAD_RATE_EDIT:

							if(Context->SelectedNode)
							{
								BOOL Result = FALSE;

								LockInterface(Context);

								if(MsgGadgetID == GAD_RATE_EDIT)
								{
									if(Context->SelectedNode->Entry == GetActiveEntry(Context->PhoneHandle))
										ShowRequest(Context->Window,LocaleString(MSG_PHONEPANEL_PHONE_ENTRY_IN_USE_EDIT_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
									else
									{
										BOOL HadToFill = FALSE;

										if(IsListEmpty(&Context->SelectedNode->Entry->TimeDateList))
										{
											struct TimeDateNode *TimeDateNode;

												/* Provide defaults */

											if(TimeDateNode = CreateTimeDateNode(-1,-1,"",2))
											{
												AddTail((struct List *)&Context->SelectedNode->Entry->TimeDateList,&TimeDateNode->Node);

												HadToFill = TRUE;

												Context->SelectedNode->Entry->Header->NoRates = FALSE;
											}
											else
												Context->SelectedNode->Entry->Header->NoRates = TRUE;	/* Sorry, guv'nor */
										}

										if(!IsListEmpty(&Context->SelectedNode->Entry->TimeDateList))
											Result = RatePanel(Context->EditWindow,Context->PhoneHandle,Context->SelectedNode->Entry,NULL);
										else
											Result = FALSE;

										if(!Result && HadToFill)
										{
												/* Discard the time/date list */

											FreeTimeDateList((struct List *)&Context->SelectedNode->Entry->TimeDateList);

												/* No rates here */

											Context->SelectedNode->Entry->Header->NoRates = TRUE;
										}
									}
								}
								else
									Result = EditConfig(Context->SelectedNode->Entry->Config,MsgGadgetID,MsgQualifier,Context->EditWindow);

								if(Result)
								{
									HighlightActiveEntry(Context,FALSE);

									PhonebookChanged = TRUE;
								}

								UnlockInterface(Context);

								LT_ShowWindow(Context->Editor,TRUE);
							}

							break;

							/* Open the pattern selection window */

						case GAD_PATTERN:

							if(Context->PatternWindow)
							{
								LT_ShowWindow(Context->Selector,TRUE);
								LT_Activate(Context->Selector,GAD_SELECT_NAME);
							}
							else
							{
								if(Context->Selector = CreateSelectorHandle(Context->Window,NamePattern,NumberPattern,CommentPattern,&Mode))
									Context->PatternWindow = Context->Selector->Window;
								else
									DisplayBeep(Context->Window->WScreen);
							}

							break;

							/* Apply the selection pattern to the phonebook */

						case GAD_SELECT_APPLY:
						case GAD_SELECT_APPLY_AND_CLOSE:

							UpdateEditorData(Context);

							LT_UpdateStrings(Context->Selector);

							ParsePatternNoCase(NamePattern,		ParsedNamePattern,		130);
							ParsePatternNoCase(NumberPattern,	ParsedNumberPattern,	130);
							ParsePatternNoCase(CommentPattern,	ParsedCommentPattern,	130);

							DetachList(Context);

							for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
							{
								if((Node->Entry->Count < 0 && Mode != 0) || (Node->Entry->Count >= 0 && Mode == 0))
								{
									if(MatchPatternNoCase(ParsedNamePattern,Node->Entry->Header->Name) & MatchPatternNoCase(ParsedNumberPattern,Node->Entry->Header->Number) & MatchPatternNoCase(ParsedCommentPattern,Node->Entry->Header->Comment))
									{
										if(Mode)
											MarkDialEntry(Context->PhoneHandle,Node->Entry);
										else
											UnmarkDialEntry(Context->PhoneHandle,Node->Entry);
									}
								}
							}

							AttachList(Context);

							UpdateWindowTitle(Context);

							if(!Context->SelectedNode)
							{
								LONG Index;
								BOOL GotIt;

								GotIt = FALSE;

								for(Index = 0, Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Index++, Node = (PhoneNode *)Node->Node.ln_Succ)
								{
									if(MatchPatternNoCase(ParsedNamePattern,Node->Entry->Header->Name) & MatchPatternNoCase(ParsedNumberPattern,Node->Entry->Header->Number) & MatchPatternNoCase(ParsedCommentPattern,Node->Entry->Header->Comment))
									{
										GotIt = TRUE;
										break;
									}
								}

								if(GotIt)
									SelectActiveEntry(Context,Index,TRUE);
							}

							HighlightActiveEntry(Context,FALSE);

							if(MsgGadgetID != GAD_SELECT_APPLY)
								CloseSomeWindow(Context,Context->PatternWindow);

							break;

							/* Close the pattern window */

						case GAD_SELECT_CLOSE:

							CloseSomeWindow(Context,Context->PatternWindow);
							break;

							/* Dial the selected list */

						case GAD_DIAL:

							UpdateEditorData(Context);

								/* If no entry is tagged for dialing and we have */
								/* an active entry, tag it now. */

							if(Context->PhoneHandle->DialMarker == 0 && Context->SelectedNode)
								MarkDialEntry(Context->PhoneHandle,Context->SelectedNode->Entry);

								/* Can we go and dial? */

							if(Context->PhoneHandle->DialMarker > 0)
							{
								DialTheList	= TRUE;
								Done		= TRUE;
							}

							break;
					}
				}

					/* Double-clicked on an entry? */

				if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadgetID == GAD_NAMELIST)
				{
					if(!(MsgQualifier & SHIFT_KEY) && !Online && Context->PhoneHandle->Phonebook[MsgCode]->Header->Number[0])
					{
						PhoneEntry *Entry;
						PhoneNode *Node;

						UpdateEditorData(Context);

						SelectActiveEntry(Context,MsgCode,FALSE);

						DetachList(Context);

						Entry = Context->SelectedNode->Entry;

						for(Node = (PhoneNode *)Context->CurrentList->lh_Head ; Node->Node.ln_Succ ; Node = (PhoneNode *)Node->Node.ln_Succ)
						{
							if(Node->Entry == Entry)
								MarkDialEntry(Context->PhoneHandle,Node->Entry);
							else
								UnmarkDialEntry(Context->PhoneHandle,Node->Entry);
						}

						UpdateWindowTitle(Context);

						AttachList(Context);

						HighlightActiveEntry(Context,FALSE);

						if(Context->PhoneHandle->DialMarker > 0)
						{
							DialTheList	= TRUE;
							Done		= TRUE;
						}

						LT_PressButton(Handle,GAD_DIAL);
					}
				}
			}
		}
		while(!Done);

		PopWindow();
	}

		/* Close the windows we have open */

	CloseSomeWindow(Context,Context->Window);
	CloseSomeWindow(Context,Context->EditWindow);
	CloseSomeWindow(Context,Context->PatternWindow);
	CloseSomeWindow(Context,Context->GroupWindow);
	CloseSomeWindow(Context,Context->SortWindow);
	CloseSomeWindow(Context,Context->PasswordWindow);

		/* Should we really go and dial the tagged entries? */

	if(DialTheList)
	{
			/* Remove all entries from the list. */

		DeleteDialList(Context->PhoneHandle);

			/* Add all the tagged phonebook entries to the dial list */

		if(!AddAllDialEntries(Context->PhoneHandle))
			DialTheList = FALSE;
	}
	else
	{
			/* Remove all entries from the list that got there during
			 * the last dial attempt but are no longer tagged for
			 * dialing.
			 */

		CleanseDialList(Context->PhoneHandle);
	}

		/* If the phonebook has changed so will the dial menu */

	if(PhonebookChanged)
	{
		RebuildMenu = TRUE;
		ActivateJob(MainJobQueue,RebuildMenuJob);
	}

		/* If the phonebook needs rebuilding, start by
		 * making the quick dial menu unavailable.
		 */

	if(RebuildMenu)
		DisconnectDialMenu();

		/* Delete the display list */

	DeletePhoneList(Context->PhoneHandle);

		/* And delete the temporary data allocated */

	DeletePhoneListContext(Context);

	return(DialTheList);
}
