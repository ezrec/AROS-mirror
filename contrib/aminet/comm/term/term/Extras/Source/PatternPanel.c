/*
**	PatternPanel.c
**
**	Editing panel for phone number patterns
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
** :ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST,GAD_COMMENT,GAD_PATTERN,
			GAD_TOP,GAD_UP,GAD_DOWN,GAD_END,
			GAD_NEW,GAD_REMOVE,GAD_CLEAR,
			GAD_EDIT,GAD_COPY,
			GAD_LOAD,GAD_SAVE,GAD_USE,GAD_CLOSE,GAD_DISCARD
		};

STATIC VOID
ChangeLocalState(LayoutHandle *Handle,struct List *List,struct PatternNode *Node)
{
	BOOL IsFirst,IsLast,IsEmpty,IsInvalid,IsLocked;
	Tag StringTag;

	if(IsEmpty = IsListEmpty(List))
		IsFirst = IsLast = IsInvalid = IsLocked = TRUE;
	else
	{
		if(Node)
		{
			IsFirst		= (BOOL)(Node == (struct PatternNode *)List->lh_Head);
			IsLast		= (BOOL)(Node == (struct PatternNode *)List->lh_TailPred);
			IsInvalid	= FALSE;
			IsLocked	= (BOOL)(&Node->List == (struct MinList *)GetActivePattern());
		}
		else
			IsFirst = IsLast = IsInvalid = IsLocked = TRUE;
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

	LT_SetAttributes(Handle,GAD_LOAD,
		GA_Disabled,		GetActivePattern() != NULL,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CLEAR,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_REMOVE,
		GA_Disabled,	IsLocked,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_EDIT,
		GA_Disabled,	IsLocked,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_COPY,
		GA_Disabled,	IsLocked,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_PATTERN,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_COMMENT,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);
}

BOOL
PatternPanelConfig(struct List *PatternList,STRPTR LastPattern,BOOL *ChangedPtr)
{
	STATIC WORD ButtonID[] =
	{
		MSG_FASTMACROPANEL_NEW_GAD,
		MSG_GLOBAL_REMOVE_GAD,
		MSG_UPLOADQUEUE_CLEAR_TXT,
		MSG_V36_0043,
		MSG_COPY_GAD
	};

	struct LayoutHandle *Handle;
	struct PatternNode	*Node;
	LONG				 ListCount;
	LONG				 i,Max,Len;
	LONG				 MaxWidth,MaxHeight;
	BOOL				 Changed = FALSE;

	ListCount = GetListSize(PatternList);

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window	*LocalWindow;
		struct IBox 	Bounds;

		for(i = Max = 0 ; i < NUM_ELEMENTS(ButtonID) ; i++)
		{
			if((Len = LT_LabelChars(Handle,LocaleString(ButtonID[i]))) > Max)
				Max = Len;
		}

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelID, 	MSG_AREACODE_GROUPS_TXT,
			TAG_DONE);
			{
				MaxWidth = MaxHeight = 0;

				if(PatternList)
				{
					struct Node *Node;
					LONG Len;

					for(Node = PatternList->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
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
					LA_LabelID, 	MSG_AREACODE_GROUPS_GAD,
					LA_LabelPlace,	PLACE_Left,
					LA_Chars,		30,
					LA_ID,			GAD_LIST,
					LALV_Lines,		10,
					LALV_CursorKey, TRUE,
					LALV_ResizeX,	TRUE,
					LALV_ResizeY,	TRUE,
					LALV_MaxGrowY,	MaxHeight,
					LALV_MaxGrowX,	MaxWidth,
					GTLV_Labels,	PatternList,

					Kick30 ? LALV_Link : TAG_IGNORE, NIL_LINK,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID, 	MSG_AREACODE_NAME_GAD,
					LA_ID,			GAD_COMMENT,
					GTST_MaxChars,	structsizeof(PatternNode,Comment) - 1,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID, 	MSG_AREACODE_PATTERN_GAD,
					LA_ID,			GAD_PATTERN,
					GTST_MaxChars,	structsizeof(PatternNode,Pattern) - 1,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LA_LabelID, 	MSG_V36_0038,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Previous,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_TOP,
					LA_NoKey,			TRUE,
					GA_Disabled,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Rewind,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_UP,
					LA_NoKey,			TRUE,
					GA_Disabled,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Play,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_DOWN,
					LA_NoKey,			TRUE,
					GA_Disabled,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			TAPEDECK_KIND,
					LATD_ButtonType,	TDBT_Next,
					LATD_Smaller,		TRUE,
					LA_ID,				GAD_END,
					LA_NoKey,			TRUE,
					GA_Disabled,		TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LA_LabelID, 	MSG_V36_0043,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_FASTMACROPANEL_NEW_GAD,
					LA_ID,			GAD_NEW,
					LA_Chars,		Max,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_GLOBAL_REMOVE_GAD,
					LA_ID,			GAD_REMOVE,
					GA_Disabled,	TRUE,
					LA_Chars,		Max,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_COPY_GAD,
					LA_ID,			GAD_COPY,
					GA_Disabled,	TRUE,
					LA_Chars,		Max,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_UPLOADQUEUE_CLEAR_TXT,
					LA_ID,			GAD_CLEAR,
					GA_Disabled,	IsListEmpty(PatternList),
					LA_Chars,		Max,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_V36_0043,
					LA_ID,			GAD_EDIT,
					GA_Disabled,	TRUE,
					LA_Chars,		Max,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,LA_Type,XBAR_KIND,LAXB_FullSize,TRUE,TAG_DONE);

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
					LA_LabelID, 	MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LABT_ReturnKey, TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_GLOBAL_LOAD_GAD,
					LA_ID,			GAD_LOAD,
					GA_Disabled,	GetActivePattern() != NULL,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_GLOBAL_SAVE_GAD,
					LA_ID,			GAD_SAVE,
					GA_Disabled,	IsListEmpty(PatternList),
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_DISCARD_GAD,
					LA_ID,			GAD_DISCARD,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID, 	MSG_GLOBAL_CLOSE_TXT,
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
			LAWN_TitleID,		MSG_AREACODE_GROUPS_TITLE,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget, 	TRUE,
			WA_DragBar, 		TRUE,
			WA_RMBTrap, 		TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,

			Screen ? LAWN_Bounds : TAG_IGNORE, &Bounds,
		TAG_DONE))
		{
			struct IntuiMessage 	*Message;
			BOOL	 				 Done = FALSE;
			ULONG					 MsgClass;
			UWORD					 MsgCode;
			struct Gadget			*MsgGadget;

			struct PatternNode		*Selected = NULL;
			LONG					 Offset = -1;

			UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester	*FileRequest;
			LONG					 Error;

			GuideContext(CONTEXT_AREA_CODES);

			PushWindow(LocalWindow);

			do
			{
				if(Wait(PORTMASK(LocalWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass		= Message->Class;
					MsgCode 		= Message->Code;
					MsgGadget		= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						if(MsgGadget->GadgetID == GAD_LIST)
						{
							if(Selected)
							{
								strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
								strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
							}

							if(!Selected)
							{
								if(Selected = (struct PatternNode *)GetListNode(MsgCode,(struct List *)PatternList))
								{
									Offset = MsgCode;

									LT_SetAttributes(Handle,GAD_COMMENT,
										GTST_String,	Selected->Comment,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_PATTERN,
										GTST_String,	Selected->Pattern,
									TAG_DONE);

									ChangeLocalState(Handle,PatternList,Selected);
								}
							}

							if(Selected && (&Selected->List != (struct MinList *)GetActivePattern()))
							{
								LT_PressButton(Handle,GAD_EDIT);

								LT_LockWindow(LocalWindow);

								if(RatePanel(LocalWindow,GlobalPhoneHandle,NULL,(struct List *)&Selected->List))
								{
									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								LT_UnlockWindow(LocalWindow);
							}
						}
					}

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_CLOSE:
							case GAD_USE:

								if(Selected)
								{
									strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
									strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
								}

								Done = TRUE;
								break;

							case GAD_TOP:

								MoveListViewNode(Handle,PatternList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_HEAD);

								ChangeLocalState(Handle,PatternList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_UP:

								MoveListViewNode(Handle,PatternList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_PRED);

								ChangeLocalState(Handle,PatternList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_DOWN:

								MoveListViewNode(Handle,PatternList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_SUCC);

								ChangeLocalState(Handle,PatternList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_END:

								MoveListViewNode(Handle,PatternList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_TAIL);

								ChangeLocalState(Handle,PatternList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_NEW:

								if(Selected)
								{
									strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
									strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
								}

								if(Node = CreatePatternNode(""))
								{
									if(Selected)
									{
										strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
										strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
									}

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									AddTail((struct List *)PatternList,(struct Node *)Node);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	PatternList,
										LALV_Selected,	ListCount++,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_PATTERN,
										GTST_String,	"",
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_COMMENT,
										GTST_String,	"",
									TAG_DONE);

									Selected = Node;
									Offset = ListCount - 1;

									ChangeLocalState(Handle,PatternList,Selected);

									LT_Activate(Handle,GAD_COMMENT);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}
								else
									DisplayBeep(LocalWindow->WScreen);

								break;

							case GAD_COPY:

								if(Node = CreatePatternNode(Selected->Comment))
								{
									struct TimeDateNode *OldNode,*NewNode;
									BOOL AllFine;

									FreeList((struct List *)&Node->List);

									AllFine = TRUE;

									for(OldNode = (struct TimeDateNode *)Selected->List.mlh_Head ; OldNode->Node.ln_Succ ; OldNode = (struct TimeDateNode *)OldNode->Node.ln_Succ)
									{
										if(NewNode = CreateTimeDateNode(0,1,"",OldNode->Table[0].Count))
										{
											strcpy(NewNode->Node.ln_Name = NewNode->Buffer,OldNode->Buffer);

											NewNode->Header = OldNode->Header;

											CopyMem(OldNode->Table,NewNode->Table,sizeof(struct TimeDate) * OldNode->Table[0].Count);

											AdaptTimeDateNode(NewNode);

											AddTail((struct List *)&Node->List,(struct Node *)NewNode);
										}
										else
										{
											AllFine = FALSE;
											break;
										}
									}

									if(!AllFine)
									{
										FreeList((struct List *)&Node->List);
										FreeVecPooled(Node);
										DisplayBeep(LocalWindow->WScreen);
									}
									else
									{
										if(Selected)
										{
											strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
											strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
										}

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	~0,
										TAG_DONE);

										AddTail((struct List *)PatternList,(struct Node *)Node);

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	PatternList,
											LALV_Selected,	ListCount++,
										TAG_DONE);

										strcpy(Node->Pattern,Selected->Pattern);
										strcpy(Node->Comment,Selected->Comment);

										Selected = Node;
										Offset = ListCount - 1;

										LT_SetAttributes(Handle,GAD_PATTERN,
											GTST_String,	Selected->Pattern,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_COMMENT,
											GTST_String,	Selected->Comment,
										TAG_DONE);

										ChangeLocalState(Handle,PatternList,Selected);

										LT_Activate(Handle,GAD_COMMENT);

										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = TRUE;
									}
								}
								else
									DisplayBeep(LocalWindow->WScreen);

								break;

							case GAD_DISCARD:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								FreePatternList(PatternList);

								Done = Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								LastPattern[0] = 0;
								strcpy(Config->AreaCodeFileName,LastPattern);

								break;

							case GAD_CLEAR:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								FreePatternList(PatternList);
								ListCount = 0;

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	PatternList,
									LALV_Selected,	~0,
								TAG_DONE);

								Selected = NULL;
								Offset = -1;

								ChangeLocalState(Handle,PatternList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								break;

							case GAD_EDIT:

								if(Selected)
								{
									LT_LockWindow(LocalWindow);

									if(RatePanel(LocalWindow,GlobalPhoneHandle,NULL,(struct List *)&Selected->List))
									{
										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = TRUE;
									}

									LT_UnlockWindow(LocalWindow);
								}

								break;

							case GAD_REMOVE:

								if(Selected)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Node = (struct PatternNode *)RemoveGetNext((struct Node *)Selected);
									DeletePatternNode(Selected);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	PatternList,
										LALV_Selected,	Offset = GetNodeOffset((struct Node *)Node,(struct List *)PatternList),
									TAG_DONE);

									Selected = Node;

									if(Selected)
									{
										LT_SetAttributes(Handle,GAD_PATTERN,
											GTST_String,	Selected->Pattern,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_COMMENT,
											GTST_String,	Selected->Comment,
										TAG_DONE);
									}

									ChangeLocalState(Handle,PatternList,Selected);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LIST:

								if(Selected)
								{
									strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
									strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
								}

								if(Selected = (struct PatternNode *)GetListNode(MsgCode,(struct List *)PatternList))
								{
									Offset = MsgCode;

									LT_SetAttributes(Handle,GAD_COMMENT,
										GTST_String,	Selected->Comment,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_PATTERN,
										GTST_String,	Selected->Pattern,
									TAG_DONE);

									ChangeLocalState(Handle,PatternList,Selected);
								}

								break;

							case GAD_COMMENT:

								if(Selected)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	PatternList,
										LALV_Selected,	GetNodeOffset((struct Node *)Selected,(struct List *)PatternList),
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_PATTERN,
										GTST_String,	Selected->Pattern,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_COMMENT,
										GTST_String,	Selected->Comment,
									TAG_DONE);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;

									LT_Activate(Handle,GAD_PATTERN);
								}

								break;

							case GAD_PATTERN:

								if(Selected)
								{
									strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LOAD:

								LT_LockWindow(LocalWindow);

								strcpy(DummyBuffer,LastPattern);

								if(FileRequest = OpenSingleFile(LocalWindow,LocaleString(MSG_AREACODE_LOAD_GROUP_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									struct List *SomeList;

									FreeAslRequest(FileRequest);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Selected = NULL;
									Offset = -1;

									if(!(SomeList = LoadTimeDateList(DummyBuffer,&Error)))
										ShowError(LocalWindow,ERR_LOAD_ERROR,Error,DummyBuffer);
									else
									{
										FreePatternList(PatternList);

										MoveList(SomeList,PatternList);
										FreeVecPooled(SomeList);

										strcpy(LastPattern,DummyBuffer);
										strcpy(Config->AreaCodeFileName,LastPattern);

										Changed = FALSE;

										if(ChangedPtr)
											*ChangedPtr = FALSE;

										ListCount = GetListSize(PatternList);
									}

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	PatternList,
										LALV_Selected,	~0,
									TAG_DONE);

									ChangeLocalState(Handle,PatternList,Selected);
								}

								LT_UnlockWindow(LocalWindow);

								break;

							case GAD_SAVE:

								if(Selected)
								{
									strcpy(Selected->Pattern,LT_GetString(Handle,GAD_PATTERN));
									strcpy(Selected->Comment,LT_GetString(Handle,GAD_COMMENT));
								}

								LT_LockWindow(LocalWindow);

								strcpy(DummyBuffer,LastPattern);

								if(FileRequest = SaveFile(LocalWindow,LocaleString(MSG_AREACODE_SAVE_GROUP_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!SaveTimeDateList(DummyBuffer,PatternList,&Error))
										ShowError(LocalWindow,ERR_SAVE_ERROR,Error,DummyBuffer);
									else
									{
										strcpy(LastPattern,DummyBuffer);

										strcpy(Config->AreaCodeFileName,LastPattern);

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}
								}

								LT_UnlockWindow(LocalWindow);

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

	return(Changed);
}
