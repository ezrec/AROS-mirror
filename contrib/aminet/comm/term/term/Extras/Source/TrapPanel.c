/*
**	TrapPanel.c
**
**	Editing panel for trap list
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST,GAD_SEQUENCE,GAD_COMMAND,
			GAD_TOP,GAD_UP,GAD_DOWN,GAD_END,
			GAD_NEW,GAD_REMOVE,GAD_CLEAR,
			GAD_LOAD,GAD_SAVE,GAD_USE,GAD_CLOSE,
			GAD_DISCARD
		};

STATIC VOID
ChangeLocalState(LayoutHandle *Handle,struct GenericList *List,struct TrapNode *Node)
{
	BOOL IsFirst,IsLast,IsEmpty,IsInvalid;
	Tag StringTag;

	if(IsEmpty = IsListEmpty((struct List *)List))
		IsFirst = IsLast = IsInvalid = TRUE;
	else
	{
		if(Node)
		{
			IsFirst		= (BOOL)(Node == (struct TrapNode *)((struct List *)List)->lh_Head);
			IsLast		= (BOOL)(Node == (struct TrapNode *)((struct List *)List)->lh_TailPred);
			IsInvalid	= FALSE;
		}
		else
			IsFirst = IsLast = IsInvalid = TRUE;
	}

	StringTag = IsInvalid ? GTST_String : TAG_IGNORE;

	LT_SetAttributes(Handle,GAD_TOP,
		GA_Disabled,	IsFirst,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_UP,
		GA_Disabled,	IsFirst,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_DOWN,
		GA_Disabled,	IsLast,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_END,
		GA_Disabled,	IsLast,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_SAVE,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CLEAR,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_REMOVE,
		GA_Disabled,	IsInvalid,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_SEQUENCE,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_COMMAND,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);
}

BOOL
TrapPanelConfig(struct Configuration *LocalConfig,BOOL *ChangedPtr)
{
	struct LayoutHandle	*Handle;
	struct GenericList	*TrapList;
	struct TrapNode		*Node,
						*Next;
	BOOL				 Changed;

	Changed = FALSE;

	TrapList = GenericListTable[GLIST_TRAP];

	LockGenericList(TrapList);

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window	*LocalWindow;
		struct IBox	 Bounds;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_TRAPPANEL_TRAP_CONTROLS_TXT,
			TAG_DONE);
			{
				LONG MaxWidth,MaxHeight;

				MaxWidth = MaxHeight = 0;

				if(TrapList)
				{
					struct Node *Node;
					LONG Len;

					for(Node = (struct Node *)TrapList->ListHeader.mlh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
					{
						Len = strlen(Node->ln_Name);

						if(Len > MaxWidth)
							MaxWidth = Len;

						MaxHeight++;
					}
				}

				if(MaxWidth < 30)
					MaxWidth = 30;

				if(MaxHeight < 10)
					MaxHeight = 10;
				else
				{
					if(MaxHeight > 20)
						MaxHeight = 20;
				}

				LT_New(Handle,
					LA_Type,		LISTVIEW_KIND,
					LA_LabelID,		MSG_TRAPPANEL_TRAP_LIST_TXT,
					LA_LabelPlace,	PLACE_Left,
					LA_Chars,		30,
					LA_ID,			GAD_LIST,
					LALV_Lines,		10,
					LALV_CursorKey,	TRUE,
					LALV_MaxGrowX,	MaxWidth,
					LALV_MaxGrowY,	MaxHeight,
					LALV_ResizeY,	TRUE,
					LALV_ResizeX,	TRUE,
					GTLV_Labels,	TrapList,

					Kick30 ? LALV_Link : TAG_IGNORE, NIL_LINK,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_TRAPPANEL_SEQUENCE_TXT,
					LA_ID,			GAD_SEQUENCE,
					GTST_MaxChars,	255,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_TRAPPANEL_COMMAND_TXT,
					LA_ID,			GAD_COMMAND,
					GTST_MaxChars,	255,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LA_LabelID,		MSG_V36_0038,
					LAGR_Spread,	TRUE,
					LAGR_SameSize,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LATD_ButtonType,TDBT_Previous,
						LATD_Smaller,	TRUE,
						LA_ID,			GAD_TOP,
						LA_NoKey,		TRUE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LATD_ButtonType,TDBT_Rewind,
						LATD_Smaller,	TRUE,
						LA_ID,			GAD_UP,
						LA_NoKey,		TRUE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LATD_ButtonType,TDBT_Play,
						LATD_Smaller,	TRUE,
						LA_ID,			GAD_DOWN,
						LA_NoKey,		TRUE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LATD_ButtonType,TDBT_Next,
						LATD_Smaller,	TRUE,
						LA_ID,			GAD_END,
						LA_NoKey,		TRUE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LA_LabelID,		MSG_V36_0043,
					LAGR_Spread,	TRUE,
					LAGR_SameSize,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_FASTMACROPANEL_NEW_GAD,
						LA_ID,			GAD_NEW,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_REMOVE_GAD,
						LA_ID,			GAD_REMOVE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_UPLOADQUEUE_CLEAR_TXT,
						LA_ID,			GAD_CLEAR,
						GA_Disabled,	IsListEmpty((struct List *)TrapList),
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,LA_Type,HORIZONTAL_KIND,
				LAGR_SameSize,	TRUE,
				LAGR_Spread,	TRUE,
				LA_ExtraSpace,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_LOAD_GAD,
					LA_ID,			GAD_LOAD,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_SAVE_GAD,
					LA_ID,			GAD_SAVE,
					GA_Disabled,	IsListEmpty((struct List *)TrapList),
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_DISCARD_GAD,
					LA_ID,			GAD_DISCARD,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CLOSE_TXT,
					LA_ID,			GAD_CLOSE,
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(Screen && Window)
		{
			Bounds.Left		= Window->LeftEdge;
			Bounds.Top		= Window->TopEdge;
			Bounds.Width	= Window->Width;
			Bounds.Height	= Window->Height;
		}

		if(LocalWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_TRAPPANEL_TRAP_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,

			Screen ? LAWN_Bounds : TAG_IGNORE, &Bounds,
		TAG_DONE))
		{
			struct IntuiMessage		*Message;
			BOOL					 Done = FALSE;
			ULONG					 MsgClass;
			UWORD					 MsgCode;
			struct Gadget			*MsgGadget;

			struct TrapNode			*Selected = NULL;
			LONG					 Offset = -1;

			UBYTE		 			 DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester	*FileRequest;

			GuideContext(CONTEXT_TRAPS);

			PushWindow(LocalWindow);

			do
			{
				if(Wait(PORTMASK(LocalWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgCode		= Message->Code;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_CLOSE:
							case GAD_USE:

								if(Selected)
								{
									strcpy(Selected->Command,LT_GetString(Handle,GAD_COMMAND));
									strcpy(Selected->Node.ln_Name,LT_GetString(Handle,GAD_SEQUENCE));
								}

								Done = TRUE;
								break;

							case GAD_TOP:

								MoveListViewNode(Handle,(struct List *)TrapList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_HEAD);

								ChangeLocalState(Handle,TrapList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_UP:

								MoveListViewNode(Handle,(struct List *)TrapList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_PRED);

								ChangeLocalState(Handle,TrapList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_DOWN:

								MoveListViewNode(Handle,(struct List *)TrapList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_SUCC);

								ChangeLocalState(Handle,TrapList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_END:

								MoveListViewNode(Handle,(struct List *)TrapList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_TAIL);

								ChangeLocalState(Handle,TrapList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_NEW:

								if(Node = CreateTrapNode("",""))
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									AddGenericListNode(TrapList,(struct Node *)Node,ADD_GLIST_BOTTOM,TRUE);

									Selected = Node;
									Offset = GenericListCount(TrapList) - 1;

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TrapList,
										LALV_Selected,	Offset,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_COMMAND,
										GTST_String,	"",
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_SEQUENCE,
										GTST_String,	"",
									TAG_DONE);

									ChangeLocalState(Handle,TrapList,Selected);

									LT_Activate(Handle,GAD_SEQUENCE);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}
								else
									DisplayBeep(LocalWindow->WScreen);

								break;

							case GAD_CLEAR:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								ClearGenericList(TrapList,FALSE);

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	TrapList,
									LALV_Selected,	~0,
								TAG_DONE);

								Selected = NULL;
								Offset = -1;

								ChangeLocalState(Handle,TrapList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								break;

							case GAD_REMOVE:

								if(Selected)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Node = (struct TrapNode *)DeleteGenericListNode(TrapList,(struct Node *)Selected,FALSE);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TrapList,
										LALV_Selected,	Offset = GetNodeOffset((struct Node *)Node,(struct List *)TrapList),
									TAG_DONE);

									Selected = Node;

									if(Selected)
									{
										LT_SetAttributes(Handle,GAD_COMMAND,
											GTST_String,	Selected->Command,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_SEQUENCE,
											GTST_String,	Selected->Node.ln_Name,
										TAG_DONE);
									}

									ChangeLocalState(Handle,TrapList,Selected);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LIST:

								if(Selected)
								{
									strcpy(Selected->Command,LT_GetString(Handle,GAD_COMMAND));
									strcpy(Selected->Node.ln_Name,LT_GetString(Handle,GAD_SEQUENCE));
								}

								if(Selected = (struct TrapNode *)GetListNode(MsgCode,(struct List *)TrapList))
								{
									Offset = MsgCode;

									LT_SetAttributes(Handle,GAD_SEQUENCE,
										GTST_String,	Selected->Node.ln_Name,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_COMMAND,
										GTST_String,	Selected->Command,
									TAG_DONE);

									ChangeLocalState(Handle,TrapList,Selected);
								}

								break;

							case GAD_SEQUENCE:

								if(Selected)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Selected = ChangeTrapNode(TrapList,Selected,LT_GetString(Handle,GAD_SEQUENCE),NULL);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TrapList,
										LALV_Selected,	Offset = GetNodeOffset((struct Node *)Selected,(struct List *)TrapList),
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_COMMAND,
										GTST_String,	Selected->Command,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_SEQUENCE,
										GTST_String,	Selected->Node.ln_Name,
									TAG_DONE);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;

									LT_Activate(Handle,GAD_COMMAND);
								}

								break;

							case GAD_COMMAND:

								if(Selected)
								{
									Selected = ChangeTrapNode(TrapList,Selected,NULL,LT_GetString(Handle,GAD_COMMAND));

									TrapsChanged = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LOAD:

								LT_LockWindow(LocalWindow);

								strcpy(DummyBuffer,LastTraps);

								if(FileRequest = OpenSingleFile(LocalWindow,LocaleString(MSG_TRAPPANEL_LOAD_TRAP_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Selected = NULL;
									Offset = -1;

									if(!LoadTraps(DummyBuffer,TrapList))
										ShowError(LocalWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastTraps,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->TrapFileName,LastTraps);

										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	TrapList,
										LALV_Selected,	~0,
									TAG_DONE);

									ChangeLocalState(Handle,TrapList,Selected);
								}

								LT_UnlockWindow(LocalWindow);

								break;

							case GAD_SAVE:

								if(Selected)
								{
									strcpy(Selected->Command,		LT_GetString(Handle,GAD_COMMAND));
									strcpy(Selected->Node.ln_Name,	LT_GetString(Handle,GAD_SEQUENCE));
								}

								LT_LockWindow(LocalWindow);

								strcpy(DummyBuffer,LastTraps);

								if(FileRequest = SaveFile(LocalWindow,LocaleString(MSG_TRAPPANEL_SAVE_TRAP_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!SaveTraps(DummyBuffer,(struct List *)TrapList))
										ShowError(LocalWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastTraps,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->TrapFileName,LastTraps);

										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}
								}

								LT_UnlockWindow(LocalWindow);

								break;

							case GAD_DISCARD:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								ClearGenericList(TrapList,FALSE);

								Done = Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								LastTraps[0] = 0;

								if(LocalConfig)
									strcpy(LocalConfig->TrapFileName,LastTraps);

								WatchTraps = FALSE;

								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	for(Node = (struct TrapNode *)TrapList->ListHeader.mlh_Head ; Next = (struct TrapNode *)Node->Node.ln_Succ ; Node = Next)
	{
		if(!Node->Node.ln_Name[0] || !Node->SequenceLen)
			DeleteGenericListNode(TrapList,(struct Node *)Node,FALSE);
	}

	UnlockGenericList(TrapList);

	return(Changed);
}
