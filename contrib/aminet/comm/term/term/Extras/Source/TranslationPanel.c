/*
**	TranslationPanel.c
**
**	Character code translation editing panel
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_PAGE=1,GAD_USE,GAD_DEFAULT,GAD_LOAD,GAD_SAVE,GAD_CANCEL,GAD_CLOSE,
			GAD_DISCARD,GAD_SEND_CODE_AS,GAD_RECEIVE_CODE_AS,
			GAD_CHAR=700
		};

STATIC BOOL
CodePanel(struct Window *Window,UBYTE CharCode,struct TranslationEntry **SendTable,struct TranslationEntry **ReceiveTable)
{
	LayoutHandle *Handle;
	BOOL Result;

	Result = FALSE;

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		UBYTE SendBuffer[256],ReceiveBuffer[256];
		UBYTE LocalBuffer[256];

		if(SendTable[CharCode])
			TranslateBack(SendTable[CharCode]->String,SendTable[CharCode]->Len,SendBuffer,sizeof(SendBuffer));
		else
			SendBuffer[0] = 0;

		if(ReceiveTable[CharCode])
			TranslateBack(ReceiveTable[CharCode]->String,ReceiveTable[CharCode]->Len,ReceiveBuffer,sizeof(ReceiveBuffer));
		else
			ReceiveBuffer[0] = 0;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			UBYTE OctalBuffer[6],HexBuffer[6],BinBuffer[10];
			LONG Code,i;

			OctalBuffer[0] = '0';

			for(Code = CharCode, i = 0 ; i < 3 ; i++)
			{
				OctalBuffer[2 - i + 1] = '0' + (Code & 7);

				Code = Code >> 3;
			}

			OctalBuffer[4] = 0;

			LimitedSPrintf(sizeof(HexBuffer),HexBuffer,"$%02lx",CharCode);

			BinBuffer[0] = '%';

			for(Code = CharCode, i = 0 ; i < 8 ; i++)
			{
				BinBuffer[7 - i + 1] = '0' + (Code & 1);

				Code = Code >> 1;
			}

			BinBuffer[9] = 0;

			if(CharCodes[CharCode][0] == '1' && CharCodes[CharCode][1])
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,LocaleString(MSG_V36_1772),CharCodes[CharCode]);
			else
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,LocaleString(MSG_V36_1773),CharCode,CharCodes[CharCode]);

			LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer + strlen(LocalBuffer)," · %s · %s · %s",HexBuffer,OctalBuffer,BinBuffer);

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelText,	LocalBuffer,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,				STRING_KIND,
					LA_LabelID,				MSG_TRANSLATIONPANEL_SEND_CODE_AS_GAD,
					LA_STRPTR,				SendBuffer,
					LA_Chars,				30,
					LA_ID,					GAD_SEND_CODE_AS,
					STRINGA_Justification,	GACT_STRINGCENTER,
					GTST_MaxChars,			sizeof(SendBuffer) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,				STRING_KIND,
					LA_LabelID,				MSG_TRANSLATIONPANEL_RECEIVE_CODE_AS_GAD,
					LA_STRPTR,				ReceiveBuffer,
					LA_ID,					GAD_RECEIVE_CODE_AS,
					STRINGA_Justification,	GACT_STRINGCENTER,
					GTST_MaxChars,			sizeof(ReceiveBuffer) - 1,
				TAG_DONE);

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
					LA_LabelID,		MSG_GLOBAL_DEFAULT_GAD,
					LA_ID,			GAD_DEFAULT,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL,
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_TRANSLATIONPANEL_CHARACTER_CODE_TRANSLATION_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			ULONG MsgClass;
			BOOL Done;

			LT_ShowWindow(Handle,TRUE);

			GuideContext(CONTEXT_TRANSLATION);

			PushWindow(PanelWindow);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_DEFAULT:

								LocalBuffer[0] = CharCode;

								TranslateBack(LocalBuffer,1,SendBuffer,sizeof(SendBuffer));
								TranslateBack(LocalBuffer,1,ReceiveBuffer,sizeof(ReceiveBuffer));

								LT_SetAttributes(Handle,GAD_SEND_CODE_AS,
									GTST_String,SendBuffer,
								TAG_DONE);

								LT_SetAttributes(Handle,GAD_RECEIVE_CODE_AS,
									GTST_String,ReceiveBuffer,
								TAG_DONE);

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								if(SendBuffer[0])
								{
									struct TranslationEntry *Entry = AllocTranslationEntry(SendBuffer);

									if(Entry)
									{
										if(SendTable[CharCode])
											FreeTranslationEntry(SendTable[CharCode]);

										SendTable[CharCode] = Entry;

										Result = TRUE;
									}
								}
								else
								{
									if(SendTable[CharCode])
										FreeTranslationEntry(SendTable[CharCode]);

									SendTable[CharCode] = NULL;

									Result = TRUE;
								}

								if(ReceiveBuffer[0])
								{
									struct TranslationEntry *Entry = AllocTranslationEntry(ReceiveBuffer);

									if(Entry)
									{
										if(ReceiveTable[CharCode])
											FreeTranslationEntry(ReceiveTable[CharCode]);

										ReceiveTable[CharCode] = Entry;

										Result = TRUE;
									}
								}
								else
								{
									if(ReceiveTable[CharCode])
										FreeTranslationEntry(ReceiveTable[CharCode]);

									ReceiveTable[CharCode] = NULL;

									Result = TRUE;
								}

								Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
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

	return(Result);
}

BOOL
TranslationPanelConfig(struct Configuration *LocalConfig,struct TranslationEntry ***SendTablePtr,struct TranslationEntry ***ReceiveTablePtr,STRPTR LastTranslation,struct Window *Parent,BOOL *ChangePtr)
{
	struct TranslationEntry	**SendTable,**ReceiveTable;
	BOOL MadeChanges,Created;
	LayoutHandle *Handle;

	MadeChanges		= FALSE;
	Created			= FALSE;

	SendTable		= *SendTablePtr;
	ReceiveTable	= *ReceiveTablePtr;

	if(!SendTable)
	{
		if(!(SendTable = AllocTranslationTable()))
		{
			DisplayBeep(Window->WScreen);

			return(FALSE);
		}

		if(!(ReceiveTable = AllocTranslationTable()))
		{
			DisplayBeep(Window->WScreen);

			FreeTranslationTable(SendTable);

			return(FALSE);
		}

		if(!FillTranslationTable(SendTable))
		{
			DisplayBeep(Window->WScreen);

			FreeTranslationTable(SendTable);
			FreeTranslationTable(ReceiveTable);

			return(FALSE);
		}

		if(!FillTranslationTable(ReceiveTable))
		{
			DisplayBeep(Window->WScreen);

			FreeTranslationTable(SendTable);
			FreeTranslationTable(ReceiveTable);

			return(FALSE);
		}

		Created = TRUE;
	}

	*SendTablePtr		= SendTable;
	*ReceiveTablePtr	= ReceiveTable;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		LONG Width,Max,i;

		Max = 0;

		for(i = 0 ; i < 256 ; i++)
		{
			if((Width = LT_LabelChars(Handle,CharCodes[i])) > Max)
				Max = Width;
		}

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				STATIC STRPTR Halfs[] =
				{
					"NUL-DEL",
					"128-\"ÿ\"",
					NULL
				};

				LT_New(Handle,
					LA_Type,		TAB_KIND,
					LA_LabelID,		MSG_TRANSLATIONPANEL_CHARACTER_PAGE_GAD,
					LA_ID,			GAD_PAGE,
					GTCY_Labels,	Halfs,
					LA_NoKey,		TRUE,
					LATB_FullWidth,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LONG j;

				for(i = 0 ; i < 8 ; i++)
				{
					LT_New(Handle,
						LA_Type,		HORIZONTAL_KIND,
						LAGR_SameSize,	TRUE,
					TAG_DONE);
					{
						for(j = 0 ; j < 16 ; j++)
						{
							LT_New(Handle,
								LA_Type,		BUTTON_KIND,
								LA_LabelText,	CharCodes[i * 16 + j],
								LA_ID,			GAD_CHAR + i * 16 + j,
								LA_Chars,		Max,
								LA_NoKey,		TRUE,
							TAG_DONE);
						}

						LT_EndGroup(Handle);
					}
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
				LAGR_Spread,TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_USE_GAD,
					LA_ID,			GAD_USE,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_DEFAULT_GAD,
					LA_ID,			GAD_DEFAULT,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_LOAD_GAD,
					LA_ID,			GAD_LOAD,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_SAVE_GAD,
					LA_ID,			GAD_SAVE,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_DISCARD_GAD,
					LA_ID,			GAD_DISCARD,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CLOSE_TXT,
					LA_ID,			GAD_CANCEL,
					LA_NoKey,		TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_TRANSLATIONPANEL_CHARACTER_TABLE_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester *FileRequest;
			struct IntuiMessage *Message;
			ULONG MsgClass,MsgQualifier;
			struct Gadget *MsgGadget;
			UWORD MsgCode;
			LONG CharBase;
			LONG KeyCode;
			BOOL Done;

			CharBase = 0;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
					break;

				while(Message = (struct IntuiMessage *)GT_GetIMsg(PanelWindow->UserPort))
				{
					MsgClass		= Message->Class;
					MsgQualifier	= Message->Qualifier;
					MsgCode			= Message->Code;
					MsgGadget		= (struct Gadget *)Message->IAddress;

					if(MsgClass == IDCMP_RAWKEY)
						KeyCode = LT_GetCode(MsgQualifier,MsgClass,MsgCode,MsgGadget);
					else
						KeyCode = -1;

					GT_ReplyIMsg(Message);

					LT_HandleInput(Handle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

					if(KeyCode != -1)
					{
						if(KeyCode >= CharBase && KeyCode < CharBase + 128)
							LT_PressButton(Handle,GAD_CHAR + KeyCode - CharBase);

						LT_LockWindow(PanelWindow);

						MadeChanges |= CodePanel(PanelWindow,KeyCode,SendTable,ReceiveTable);

						if(ChangePtr)
							*ChangePtr |= MadeChanges;

						LT_UnlockWindow(PanelWindow);

						LT_ShowWindow(Handle,TRUE);
					}

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_PAGE:

								LT_LockWindow(PanelWindow);

								if(MsgCode)
								{
									for(i = 0 ; i < 128 ; i++)
										LT_SetAttributes(Handle,GAD_CHAR + i,LA_LabelText,CharCodes[128 + i],TAG_DONE);

									CharBase = 128;
								}
								else
								{
									for(i = 0 ; i < 128 ; i++)
										LT_SetAttributes(Handle,GAD_CHAR + i,LA_LabelText,CharCodes[0 + i],TAG_DONE);

									CharBase = 0;
								}

								if(!LT_Rebuild(Handle,NULL,0,0,FALSE))
								{
									LT_DeleteHandle(Handle);

									Handle = NULL;

									Done = TRUE;
								}
								else
									LT_UnlockWindow(PanelWindow);

								break;

							case GAD_DISCARD:

								MadeChanges = Done = TRUE;
								LastTranslation[0] = 0;

								if(LocalConfig)
									strcpy(LocalConfig->TranslationFileName,LastTranslation);

							case GAD_DEFAULT:

								if(SendTable)
								{
									struct TranslationEntry **Table;

									if(Table = AllocTranslationTable())
									{
										if(FillTranslationTable(Table))
										{
											FreeTranslationTable(SendTable);

											SendTable = Table;
										}
									}
								}

								if(ReceiveTable)
								{
									struct TranslationEntry **Table;

									if(Table = AllocTranslationTable())
									{
										if(FillTranslationTable(Table))
										{
											FreeTranslationTable(ReceiveTable);

											ReceiveTable = Table;
										}
									}
								}

								break;

							case GAD_CLOSE:
							case GAD_USE:

								Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							case GAD_LOAD:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastTranslation);

								if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_TRANSLATIONPANEL_LOAD_TRANSLATION_TABLES_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									struct TranslationEntry **Send,**Receive;
									BOOL Success;

									Success = FALSE;
									Receive = NULL;

									FreeAslRequest(FileRequest);

									if(Send = AllocTranslationTable())
									{
										if(Receive = AllocTranslationTable())
											Success = LoadTranslationTables(DummyBuffer,Send,Receive);
									}

									if(!Success)
									{
										ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);

										if(Send)
											FreeTranslationTable(Send);

										if(Receive)
											FreeTranslationTable(Receive);
									}
									else
									{
										if(LocalConfig)
											strcpy(LocalConfig->TranslationFileName,DummyBuffer);

										strcpy(LastTranslation,DummyBuffer);

										MadeChanges	= TRUE;
										Created		= FALSE;

										FreeTranslationTable(SendTable);
										FreeTranslationTable(ReceiveTable);

										SendTable		= Send;
										ReceiveTable	= Receive;

										if(ChangePtr)
											*ChangePtr = FALSE;
									}
								}

								LT_UnlockWindow(PanelWindow);
								break;

							case GAD_SAVE:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastTranslation);

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_TRANSLATIONPANEL_SAVE_TRANSLATION_TABLES_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!SaveTranslationTables(DummyBuffer,SendTable,ReceiveTable))
										ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastTranslation,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->TranslationFileName,LastTranslation);

										Created = FALSE;

										if(ChangePtr)
											*ChangePtr = FALSE;
									}
								}

								LT_UnlockWindow(PanelWindow);
								break;

							default:

								if(MsgGadget->GadgetID >= GAD_CHAR && MsgGadget->GadgetID < GAD_CHAR + 128)
								{
									LT_LockWindow(PanelWindow);

									MadeChanges |= CodePanel(PanelWindow,CharBase + MsgGadget->GadgetID - GAD_CHAR,SendTable,ReceiveTable);

									if(ChangePtr)
										*ChangePtr |= MadeChanges;

									LT_UnlockWindow(PanelWindow);

									LT_ShowWindow(Handle,TRUE);
								}

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

	if((!MadeChanges && Created) || (IsStandardTable(SendTable) && IsStandardTable(ReceiveTable)))
	{
		FreeTranslationTable(SendTable);
		SendTable = NULL;

		FreeTranslationTable(ReceiveTable);
		ReceiveTable = NULL;
	}

	*SendTablePtr		= SendTable;
	*ReceiveTablePtr	= ReceiveTable;

	return(MadeChanges);
}
