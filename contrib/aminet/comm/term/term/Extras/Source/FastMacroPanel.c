/*
**	FastMacroPanel.c
**
**	Editing panel for fast! macros
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST,GAD_MACRO,GAD_CODE,
			GAD_TOP,GAD_UP,GAD_DOWN,GAD_END,
			GAD_NEW,GAD_REMOVE,GAD_CLEAR,
			GAD_LOAD,GAD_SAVE,GAD_USE,GAD_CLOSE,
			GAD_DISCARD
		};

STATIC VOID
ChangeLocalState(LayoutHandle *Handle,struct List *List,struct MacroNode *Node)
{
	BOOL IsFirst,IsLast,IsEmpty,IsInvalid;
	Tag StringTag;

	if(IsEmpty = IsListEmpty(List))
		IsFirst = IsLast = IsInvalid = TRUE;
	else
	{
		if(Node)
		{
			IsFirst		= (BOOL)(Node == (struct MacroNode *)List->lh_Head);
			IsLast		= (BOOL)(Node == (struct MacroNode *)List->lh_TailPred);
			IsInvalid	= FALSE;
		}
		else
			IsFirst = IsLast = IsInvalid = TRUE;
	}

	StringTag = IsInvalid ? GTST_String : TAG_IGNORE;

	LT_SetAttributes(Handle,GAD_SAVE,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CLEAR,
		GA_Disabled,	IsEmpty,
	TAG_DONE);

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

	LT_SetAttributes(Handle,GAD_MACRO,
		GA_Disabled,	IsInvalid,
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_CODE,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);

	LT_SetAttributes(Handle,GAD_REMOVE,
		GA_Disabled,	IsInvalid,
		StringTag,		"",
	TAG_DONE);
}

BOOL
FastMacroPanelConfig(struct Configuration *LocalConfig,struct List *FastMacroList,STRPTR LastFastMacros,struct Window *Parent,BOOL *ChangedPtr)
{
	struct LayoutHandle	*Handle;
	BOOL				 Changed = FALSE;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window	*FastWindow;
		struct IBox		 Bounds;
		LONG			 FastMacroCount;

		FastMacroCount = GetListSize(FastMacroList);

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_V36_0037,
			TAG_DONE);
			{
				LONG MaxWidth,MaxHeight;

				MaxWidth = MaxHeight = 0;

				if(FastMacroList)
				{
					struct Node *Node;
					LONG Len;

					for(Node = FastMacroList->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
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
					LA_LabelID,		MSG_FASTMACROPANEL_MACRO_LIST_GAD,
					LA_LabelPlace,	PLACE_Left,
					LA_Chars,		30,
					LALV_Lines,		10,
					LA_ID,			GAD_LIST,
					LALV_CursorKey,	TRUE,
					LALV_MaxGrowY,	MaxHeight,
					LALV_MaxGrowX,	MaxWidth,
					LALV_ResizeX,	TRUE,
					LALV_ResizeY,	TRUE,
					GTLV_Labels,	FastMacroList,

					Kick30 ? LALV_Link : TAG_IGNORE, NIL_LINK,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_FASTMACROPANEL_MACRO_GAD,
					LA_ID,			GAD_MACRO,
					GTST_MaxChars,	structsizeof(MacroNode,Macro) - 1,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_FASTMACROPANEL_MACRO_TEXT_GAD,
					LA_ID,			GAD_CODE,
					GTST_MaxChars,	structsizeof(MacroNode,Code) - 1,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,HORIZONTAL_KIND,
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
						GA_Disabled,	IsListEmpty(FastMacroList),
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
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
					GA_Disabled,	IsListEmpty(FastMacroList),
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

		if(FastWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_FASTMACROPANEL_FAST_MACRO_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,

			Screen ? LAWN_Bounds : TAG_IGNORE,&Bounds,
		TAG_DONE))
		{
			struct IntuiMessage		*Message;
			BOOL					 Done = FALSE;
			ULONG					 MsgClass;
			UWORD					 MsgCode;
			struct Gadget			*MsgGadget;

			struct MacroNode		*Node;
			struct MacroNode		*Selected;
			LONG					 Offset;

			UBYTE		 			 DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester	*FileRequest;

			GuideContext(CONTEXT_FASTMACROS);

			PushWindow(FastWindow);

			LT_ShowWindow(Handle,TRUE);

			Selected	= NULL;
			Offset		= -1;

			do
			{
				if(Wait(PORTMASK(FastWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
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
									strcpy(Selected->Macro,	LT_GetString(Handle,GAD_MACRO));
									strcpy(Selected->Code,	LT_GetString(Handle,GAD_CODE));
								}

								Done = TRUE;
								break;

							case GAD_TOP:

								MoveListViewNode(Handle,FastMacroList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_HEAD);

								ChangeLocalState(Handle,FastMacroList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_UP:

								MoveListViewNode(Handle,FastMacroList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_PRED);

								ChangeLocalState(Handle,FastMacroList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_DOWN:

								MoveListViewNode(Handle,FastMacroList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_SUCC);

								ChangeLocalState(Handle,FastMacroList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_END:

								MoveListViewNode(Handle,FastMacroList,GAD_LIST,(struct Node *)Selected,&Offset,MOVE_TAIL);

								ChangeLocalState(Handle,FastMacroList,Selected);

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								break;

							case GAD_NEW:

								if(Selected)
								{
									strcpy(Selected->Macro,	LT_GetString(Handle,GAD_MACRO));
									strcpy(Selected->Code,	LT_GetString(Handle,GAD_CODE));
								}

								if(Node = CreateFastMacroNode(LocaleString(MSG_FASTMACROPANEL_UNNAMED_TXT)))
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									AddTail(FastMacroList,(struct Node *)Node);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	FastMacroList,
										LALV_Selected,	FastMacroCount++,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_CODE,
										GTST_String,	"",
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_MACRO,
										GTST_String,	Node->Macro,
									TAG_DONE);

									Selected = Node;
									Offset = FastMacroCount - 1;

									ChangeLocalState(Handle,FastMacroList,Selected);

									LT_Activate(Handle,GAD_MACRO);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}
								else
									DisplayBeep(FastWindow->WScreen);

								break;

							case GAD_DISCARD:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								FreeList(FastMacroList);

								Done = Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								LastFastMacros[0] = 0;

								if(LocalConfig)
									strcpy(LocalConfig->FastMacroFileName,LastFastMacros);

								break;

							case GAD_CLEAR:

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	~0,
								TAG_DONE);

								FreeList(FastMacroList);
								FastMacroCount = 0;

								LT_SetAttributes(Handle,GAD_LIST,
									GTLV_Labels,	FastMacroList,
									LALV_Selected,	~0,
								TAG_DONE);

								Selected = NULL;
								Offset = -1;

								ChangeLocalState(Handle,FastMacroList,Selected);

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

									Node = (struct MacroNode *)RemoveGetNext((struct Node *)Selected);
									FreeVecPooled((struct Node *)Selected);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	FastMacroList,
										LALV_Selected,	Offset = GetNodeOffset((struct Node *)Node,(struct List *)FastMacroList),
									TAG_DONE);

									if(Selected = Node)
									{
										LT_SetAttributes(Handle,GAD_CODE,
											GTST_String,	Selected->Code,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_MACRO,
											GTST_String,	Selected->Macro,
										TAG_DONE);
									}

									ChangeLocalState(Handle,FastMacroList,Selected);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LIST:

								if(Selected)
								{
									strcpy(Selected->Macro,	LT_GetString(Handle,GAD_MACRO));
									strcpy(Selected->Code,	LT_GetString(Handle,GAD_CODE));
								}

								if(Selected = (struct MacroNode *)GetListNode(MsgCode,FastMacroList))
								{
									Offset = MsgCode;

									LT_SetAttributes(Handle,GAD_MACRO,
										GTST_String,	Selected->Macro,
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_CODE,
										GTST_String,	Selected->Code,
									TAG_DONE);

									ChangeLocalState(Handle,FastMacroList,Selected);
								}

								break;

							case GAD_MACRO:

								if(Selected)
								{
									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									strcpy(Selected->Macro,LT_GetString(Handle,GAD_MACRO));

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	FastMacroList,
										LALV_Selected,	GetNodeOffset((struct Node *)Selected,FastMacroList),
									TAG_DONE);

									LT_SetAttributes(Handle,GAD_CODE,
										GTST_String,	Selected->Code,
									TAG_DONE);

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;

									LT_Activate(Handle,GAD_CODE);
								}

								break;

							case GAD_CODE:

								if(Selected)
								{
									strcpy(Selected->Code,LT_GetString(Handle,GAD_CODE));

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;
								}

								break;

							case GAD_LOAD:

								LT_LockWindow(FastWindow);

								strcpy(DummyBuffer,LastFastMacros);

								if(FileRequest = OpenSingleFile(FastWindow,LocaleString(MSG_FASTMACROPANEL_LOAD_FAST_MACRO_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	~0,
									TAG_DONE);

									Selected = NULL;
									Offset = -1;

									if(!LoadFastMacros(DummyBuffer,FastMacroList))
										ShowError(FastWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									else
									{
										if(LocalConfig)
											strcpy(LocalConfig->FastMacroFileName,DummyBuffer);

										strcpy(LastFastMacros,DummyBuffer);

										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}

									LT_SetAttributes(Handle,GAD_LIST,
										GTLV_Labels,	FastMacroList,
										LALV_Selected,	~0,
									TAG_DONE);

									ChangeLocalState(Handle,FastMacroList,Selected);
								}

								LT_UnlockWindow(FastWindow);

								break;

							case GAD_SAVE:

								if(Selected)
								{
									strcpy(Selected->Macro,LT_GetString(Handle,GAD_MACRO));
									strcpy(Selected->Code,LT_GetString(Handle,GAD_CODE));
								}

								LT_LockWindow(FastWindow);

								strcpy(DummyBuffer,LastFastMacros);

								if(FileRequest = SaveFile(FastWindow,LocaleString(MSG_FASTMACROPANEL_SAVE_FAST_MACRO_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!SaveFastMacros(DummyBuffer,FastMacroList))
										ShowError(FastWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastFastMacros,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->FastMacroFileName,LastFastMacros);

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}
								}

								LT_UnlockWindow(FastWindow);

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
