/*
**	PrintPanel.c
**
**	Control panel to set options for phonebook printing
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_FILE=1, GAD_PLAIN, GAD_USE, GAD_CANCEL, GAD_OPTION };

STATIC UBYTE	OutputFile[MAX_FILENAME_LENGTH] = "PRT:";
STATIC ULONG	Flags = PRINT_SERIAL | PRINT_MODEM | PRINT_SCREEN | PRINT_TERMINAL | PRINT_USERNAME | PRINT_COMMENT;
STATIC BOOL		Plain = FALSE;

VOID
PrintPanel(struct Window *Window,struct List *PhoneList,LONG Count)
{
	LayoutHandle *Handle;
	BOOL Result;

	Result = FALSE;

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		BYTE Storage[6];

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
				LA_LabelID,	MSG_V36_0152,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LONG i;

					for(i = 0 ; i < 6 ; i++)
					{
						if(i == 3)
						{
							LT_EndGroup(Handle);

							LT_New(Handle,
								LA_Type,VERTICAL_KIND,
							TAG_DONE);
						}

						if(Flags & (1L << i))
							Storage[i] = TRUE;
						else
							Storage[i] = FALSE;

						LT_New(Handle,
							LA_Type,	CHECKBOX_KIND,
							LA_LabelID,	MSG_PRINTPANEL_INCLUDE_SERIAL_GAD + i,
							LA_ID,		GAD_OPTION + i,
							LA_BYTE,	&Storage[i],
						TAG_DONE);
					}

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
				LA_LabelID,	MSG_V36_0153,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_PRINTPANEL_OUTPUT_FILE_OR_DEVICE_GAD,
					LA_ID,			GAD_FILE,
					LA_STRPTR,		OutputFile,
					LA_Chars,		30,
					LAST_Picker,	TRUE,
					GTST_MaxChars,	sizeof(OutputFile) - 1,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelID,		MSG_PRINTPANEL_PLAIN_TEXT_GAD,
					LA_ID,			GAD_PLAIN,
					LA_BYTE,		&Plain,
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
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_CANCEL,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_PRINTPANEL_OUTPUT_OPTIONS_TXT,
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

			GuideContext(CONTEXT_PRINT);

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

					if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadget->GadgetID == GAD_FILE)
					{
						UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
						struct FileRequester *FileRequest;

						strcpy(DummyBuffer,OutputFile);

						if(*FilePart(DummyBuffer) == 0)
							strcpy(DummyBuffer,LocaleString(MSG_PRINTPANEL_FILENAME_TXT));

						if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_PRINTPANEL_SELECT_OUTPUT_FILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
						{
							FreeAslRequest(FileRequest);

							LT_SetAttributes(Handle,GAD_FILE,
								GTST_String,DummyBuffer,
							TAG_DONE);
						}
					}

					if(MsgClass == IDCMP_GADGETUP)
					{
						LONG i;

						switch(MsgGadget->GadgetID)
						{
							case GAD_USE:

								LT_UpdateStrings(Handle);

								Flags = 0;

								for(i = 0 ; i < 6 ; i++)
								{
									if(Storage[i])
										Flags |= (1L << i);
								}

								Result = Done = TRUE;
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

	if(Result)
	{
		LONG Error = 0;
		BPTR File;

		if(File = Open(OutputFile,MODE_NEWFILE))
		{
			struct Window *ReqWindow;
			struct EasyStruct  Easy;

			Easy.es_StructSize		= sizeof(struct EasyStruct);
			Easy.es_Flags			= 0;
			Easy.es_Title			= (UBYTE *)LocaleString(MSG_TERMAUX_TERM_REQUEST_TXT);
			Easy.es_GadgetFormat	= (UBYTE *)LocaleString(MSG_PRINT_STOP_TXT);
			Easy.es_TextFormat		= (UBYTE *)LocaleString(MSG_PRINT_PRINTING_PHONEBOOK_TXT);

			if(ReqWindow = BuildEasyRequest(Window,&Easy,0))
			{
				struct PhoneNode *TempNode;
				BOOL Continue;

				Continue = TRUE;

				if(Count > 0)
				{
					for(TempNode = (struct PhoneNode *)PhoneList->lh_Head ; Continue && TempNode->Node.ln_Succ ; TempNode = (struct PhoneNode *)TempNode->Node.ln_Succ)
					{
						if(TempNode->Entry->Count >= 0)
							Continue = PrintEntry(File,ReqWindow,Plain,&Error,TempNode->Entry,Flags);
					}
				}
				else
				{
					for(TempNode = (struct PhoneNode *)PhoneList->lh_Head ; Continue && TempNode->Node.ln_Succ ; TempNode = (struct PhoneNode *)TempNode->Node.ln_Succ)
						Continue = PrintEntry(File,ReqWindow,Plain,&Error,TempNode->Entry,Flags);
				}

				FreeSysRequest(ReqWindow);
			}

			Close(File);

			if(PathPart(OutputFile) != OutputFile)
			{
				AddProtection(OutputFile,FIBF_EXECUTE);

				if(Config->MiscConfig->CreateIcons)
					AddIcon(OutputFile,FILETYPE_TEXT,TRUE);
			}
		}
		else
			Error = IoErr();

		if(Error)
		{
			UBYTE LocalBuffer[256];
			STRPTR ErrorString;

			if(Fault(Error,NULL,LocalBuffer,sizeof(LocalBuffer)))
				ErrorString = LocalBuffer;
			else
				ErrorString = "???";

			ShowRequest(Window,LocaleString(MSG_PRINT_ERROR_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),Error,ErrorString);
		}
	}
}
