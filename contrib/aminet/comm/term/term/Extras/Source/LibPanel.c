/*
**	LibPanel.c
**
**	Editing panel for transfer library configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Length of a signature string after translating it back. */

#define SIGNATURE_LENGTH 40

enum	{
			GAD_DEFAULT_TYPE=1000,
			GAD_DEFAULT_NAME,
			GAD_DEFAULT_SENDSIGNATURE,
			GAD_DEFAULT_PREFS,

			GAD_ASCII_UPLOAD_TYPE,
			GAD_ASCII_UPLOAD_NAME,
			GAD_ASCII_UPLOAD_SIGNATURE,
			GAD_ASCII_UPLOAD_PREFS,
			GAD_ASCII_DOWNLOAD_TYPE,
			GAD_ASCII_DOWNLOAD_NAME,
			GAD_ASCII_DOWNLOAD_SIGNATURE,
			GAD_ASCII_DOWNLOAD_PREFS,

			GAD_TEXT_UPLOAD_TYPE,
			GAD_TEXT_UPLOAD_NAME,
			GAD_TEXT_UPLOAD_SIGNATURE,
			GAD_TEXT_UPLOAD_PREFS,
			GAD_TEXT_DOWNLOAD_TYPE,
			GAD_TEXT_DOWNLOAD_NAME,
			GAD_TEXT_DOWNLOAD_SIGNATURE,
			GAD_TEXT_DOWNLOAD_PREFS,

			GAD_BINARY_UPLOAD_TYPE,
			GAD_BINARY_UPLOAD_NAME,
			GAD_BINARY_UPLOAD_SIGNATURE,
			GAD_BINARY_UPLOAD_PREFS,
			GAD_BINARY_DOWNLOAD_TYPE,
			GAD_BINARY_DOWNLOAD_NAME,
			GAD_BINARY_DOWNLOAD_SIGNATURE,
			GAD_BINARY_DOWNLOAD_PREFS,

			GAD_DEFAULT_RECEIVESIGNATURE,

			GAD_MANGLE,GAD_PAGE,GAD_PAGEGROUP,
			GAD_NOTIFY_AFTER_N_ERRORS,GAD_NOTIFY_WHEN,
			GAD_IDENTIFY_COMMAND,
			GAD_USE,GAD_CANCEL,

			GAD_PROTOCOLTYPE,GAD_PAGER
	};

BOOL
SignaturePanel(struct Window *Parent,STRPTR String,LONG ReceiveMode)
{
	STATIC STRPTR SendProtocols[] =
	{
		"Z-Modem",
		NULL
	};

	STATIC STRPTR SendSignatureTypes[] =
	{
		"*^XB01",
		NULL
	};

	STATIC STRPTR ReceiveProtocols[] =
	{
		"Z-Modem",
		"Hydra",
		"QuickB",
		NULL
	};

	STATIC STRPTR ReceiveSignatureTypes[] =
	{
		"*^XB00",
		"^XcA\\\\f5\\\\a3^Xa",
		"\\*ENQ",
		NULL
	};

	STRPTR *SignatureTypes,*Protocols;
	struct LayoutHandle	*Handle;
	BOOL Ok;

	Ok = FALSE;

	if(ReceiveMode)
	{
		SignatureTypes	= ReceiveSignatureTypes;
		Protocols		= ReceiveProtocols;
	}
	else
	{
		SignatureTypes	= SendSignatureTypes;
		Protocols		= SendProtocols;
	}

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		BYTE Index;

		Index = 0;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		LISTVIEW_KIND,
					LA_LabelID,		MSG_XFER_SELECT_SIGNATURE_TXT,
					LALV_Labels,	Protocols,
					LALV_Link,		NIL_LINK,
					LALV_CursorKey,	TRUE,
					LALV_MaxGrowY,	5,
					LA_BYTE,		&Index,
					LA_ID,			GAD_PROTOCOLTYPE,
					LA_Chars,		30,
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
			LAWN_TitleID,		MSG_XFER_SIGNATURE_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
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

			GuideContext(CONTEXT_SIGNATURE);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

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
							case GAD_USE:

								strcpy(String,SignatureTypes[Index]);

								Ok = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						strcpy(String,SignatureTypes[Index]);

						Ok = Done = TRUE;

						LT_PressButton(Handle,GAD_USE);
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	return(Ok);
}

BOOL
LibPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[6] =
	{
		MSG_XFER_GENERAL_OPTIONS_TXT,
		MSG_XFER_PAGE1_TXT,
		MSG_XFER_PAGE4_TXT,
		MSG_XFER_PAGE3_TXT,
		MSG_XFER_PAGE2_TXT,
		-1
	};

	STATIC BYTE InitialPage = 0;

	STRPTR Signatures[TRANSFERSIG_BINARYDOWNLOAD + 1];
	struct FileRequester *FileRequester;
	LayoutHandle *Handle;
	STRPTR String;
	BOOL UseIt;
	LONG i;

	if(!(Signatures[0] = AllocVecPooled(SIGNATURE_LENGTH * (TRANSFERSIG_BINARYDOWNLOAD + 1),MEMF_ANY)))
		return(FALSE);

	SaveConfig(LocalConfig,PrivateConfig);

	for(i = 1 ; i <= TRANSFERSIG_BINARYDOWNLOAD ; i++)
		Signatures[i] = Signatures[i - 1] + SIGNATURE_LENGTH;

	for(i = 0 ; i <= TRANSFERSIG_BINARYDOWNLOAD ; i++)
	{
		if(PrivateConfig->TransferConfig->Signatures[i] . Length)
			TranslateBack(PrivateConfig->TransferConfig->Signatures[i] . Signature,PrivateConfig->TransferConfig->Signatures[i] . Length,Signatures[i],SIGNATURE_LENGTH);
		else
			Signatures[i][0] = 0;
	}

	UseIt = FALSE;

	if(Handle = LT_CreateHandleTags(Parent->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
		LAHN_ExitFlush,	FALSE,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			TAB_KIND,
					LA_LabelID,			MSG_XFER_PAGE_TXT,
					LACY_LabelTable,	PageTitleID,
					LACY_AutoPageID,	GAD_PAGEGROUP,
					LACY_TabKey,		TRUE,
					LA_BYTE,			&InitialPage,
					LATB_FullWidth,		TRUE,
					LA_ID,				GAD_PAGER,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_ID,			GAD_PAGEGROUP,
				LAGR_ActivePage,InitialPage,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	HORIZONTAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_MISCPANEL_OVERRIDE_TRANSFER_PATH_GAD,
								LA_BYTE,	&PrivateConfig->TransferConfig->OverridePath,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_MISCPANEL_SET_ARCHIVED_BIT_GAD,
								LA_BYTE,	&PrivateConfig->TransferConfig->SetArchivedBit,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_MISCPANEL_TRANSFER_ICONS_GAD,
								LA_BYTE,	&PrivateConfig->TransferConfig->TransferIcons,
							TAG_DONE);

							LT_EndGroup(Handle);
						}

						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
						{
							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_MANGLE_UPLOAD_FILENAMES_TXT,
								LA_BYTE,	&PrivateConfig->TransferConfig->MangleFileNames,
								LA_ID,		GAD_MANGLE,
							TAG_DONE);

							LT_New(Handle,
								LA_Type,	CHECKBOX_KIND,
								LA_LabelID,	MSG_MISCPANEL_HIDE_UPLOAD_ICON_GAD,
								LA_BYTE,	&PrivateConfig->TransferConfig->HideUploadIcon,
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
							LA_Type,				XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,				INTEGER_KIND,
							LA_LabelID,				MSG_NOTIFY_USER_AFTER_N_ERRORS_HAVE_OCCURED_TXT,
							LA_UWORD,				&PrivateConfig->TransferConfig->ErrorNotification,
							LA_ID,					GAD_NOTIFY_AFTER_N_ERRORS,
							LA_Chars,				8,
							LAIN_Min,				0,
							LAIN_Max,				65535,
							LAIN_UseIncrementers,	TRUE,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			CYCLE_KIND,
							LA_LabelID,			MSG_NOTIFY_USER_TXT,
							LA_BYTE,			&PrivateConfig->TransferConfig->TransferNotification,
							LA_ID,				GAD_NOTIFY_WHEN,
							LACY_FirstLabel,	MSG_NOTIFY_USER_MODE1_TXT,
							LACY_LastLabel,		MSG_NOTIFY_USER_MODE4_TXT,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			CYCLE_KIND,
							LA_LabelID,			MSG_MISCPANEL_IDENTIFY_FILES_GAD,
							LACY_FirstLabel,	MSG_MISCPANEL_IDENTIFY_IGNORE_TXT,
							LACY_LastLabel,		MSG_MISCPANEL_IDENTIFY_SOURCE_TXT,
							LA_BYTE,			&PrivateConfig->TransferConfig->IdentifyFiles,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			XBAR_KIND,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			STRING_KIND,
							LA_LabelID,			MSG_IDENTIFY_COMMAND_GAD,
							LA_UWORD,			PrivateConfig->TransferConfig->IdentifyCommand,
							LA_ID,				GAD_IDENTIFY_COMMAND,
							GTST_MaxChars,		sizeof(PrivateConfig->TransferConfig->IdentifyCommand),
							LAST_UsePicker,		TRUE,
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
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_DEFAULT_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->DefaultType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE2_TXT,
						LA_Chars,		SIGNATURE_LENGTH - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_DEFAULT_NAME,
						LA_LabelID,		MSG_XFER_NAME_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->DefaultLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->DefaultLibrary) - 1,
						LAST_Picker,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_DEFAULT_SENDSIGNATURE,
						LA_LabelID,		MSG_XFER_SEND_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_DEFAULTUPLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_DEFAULT_RECEIVESIGNATURE,
						LA_LabelID,		MSG_XFER_RECEIVE_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_DEFAULTDOWNLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_DEFAULT_PREFS,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_BINARY_UPLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->BinaryUploadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE3_TXT,
						LA_Chars,		SIGNATURE_LENGTH - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_BINARY_UPLOAD_NAME,
						LA_LabelID,		MSG_XFER_SEND_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->BinaryUploadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->BinaryUploadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryUploadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_BINARY_UPLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_BINARYUPLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryUploadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_BINARY_UPLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryUploadType == XFER_DEFAULT || PrivateConfig->TransferConfig->BinaryUploadType == XFER_EXTERNALPROGRAM,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_BINARY_DOWNLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->BinaryDownloadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE3_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_BINARY_DOWNLOAD_NAME,
						LA_LabelID,		MSG_XFER_RECEIVE_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->BinaryDownloadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->BinaryDownloadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryDownloadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_BINARY_DOWNLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_BINARYDOWNLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryDownloadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_BINARY_DOWNLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->BinaryDownloadType == XFER_DEFAULT || PrivateConfig->TransferConfig->BinaryDownloadType == XFER_EXTERNALPROGRAM,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_TEXT_UPLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->TextUploadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE3_TXT,
						LA_Chars,		SIGNATURE_LENGTH - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_TEXT_UPLOAD_NAME,
						LA_LabelID,		MSG_XFER_SEND_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->TextUploadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->TextUploadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->TextUploadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_TEXT_UPLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_TEXTUPLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->TextUploadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_TEXT_UPLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->TextUploadType == XFER_DEFAULT || PrivateConfig->TransferConfig->TextUploadType == XFER_EXTERNALPROGRAM,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_TEXT_DOWNLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->TextDownloadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE3_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_TEXT_DOWNLOAD_NAME,
						LA_LabelID,		MSG_XFER_RECEIVE_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->TextDownloadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->TextDownloadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->TextDownloadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_TEXT_DOWNLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_TEXTDOWNLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->TextDownloadType == XFER_DEFAULT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_TEXT_DOWNLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->TextDownloadType == XFER_DEFAULT || PrivateConfig->TransferConfig->TextDownloadType == XFER_EXTERNALPROGRAM,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_ASCII_UPLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->ASCIIUploadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE4_TXT,
						LA_Chars,		SIGNATURE_LENGTH - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_ASCII_UPLOAD_NAME,
						LA_LabelID,		MSG_XFER_SEND_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->ASCIIUploadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->ASCIIUploadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIUploadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIUploadType == XFER_INTERNAL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_ASCII_UPLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_ASCIIUPLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIUploadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIUploadType == XFER_INTERNAL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_ASCII_UPLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIUploadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIUploadType == XFER_EXTERNALPROGRAM,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_ID,			GAD_ASCII_DOWNLOAD_TYPE,
						LA_BYTE,		&PrivateConfig->TransferConfig->ASCIIDownloadType,
						LA_LabelID,		MSG_XFER_TYPE_TXT,
						LACY_FirstLabel,MSG_XFER_TYPE1_TXT,
						LACY_LastLabel,	MSG_XFER_TYPE4_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_ASCII_DOWNLOAD_NAME,
						LA_LabelID,		MSG_XFER_RECEIVE_TXT,
						LA_STRPTR,		PrivateConfig->TransferConfig->ASCIIDownloadLibrary,
						GTST_MaxChars,	sizeof(PrivateConfig->TransferConfig->ASCIIDownloadLibrary) - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_INTERNAL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_ASCII_DOWNLOAD_SIGNATURE,
						LA_LabelID,		MSG_XFER_SIGNATURE_TXT,
						LA_STRPTR,		Signatures[TRANSFERSIG_ASCIIDOWNLOAD],
						GTST_MaxChars,	SIGNATURE_LENGTH - 1,
						LAST_Picker,	TRUE,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_INTERNAL,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_XFER_SETTINGS_TXT,
						LA_ID,			GAD_ASCII_DOWNLOAD_PREFS,
						GA_Disabled,	PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_DEFAULT || PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_EXTERNALPROGRAM,
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
			LAWN_TitleID,		MSG_LIBPANEL_LIBRARY_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Parent,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			struct IntuiMessage	*Message;
			struct Gadget *MsgGadget;
			STRPTR Contents;
			ULONG MsgClass;
			UWORD MsgCode;
			LONG Type;
			BOOL Done;

			GuideContext(CONTEXT_LIBS);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
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
							case GAD_USE:

								LT_UpdateStrings(Handle);

								for(i = 0 ; i <= TRANSFERSIG_BINARYDOWNLOAD ; i++)
									PrivateConfig->TransferConfig->Signatures[i] . Length = TranslateString(Signatures[i],PrivateConfig->TransferConfig->Signatures[i] . Signature);

								UseIt = Done = TRUE;

								break;

							case GAD_CANCEL:

								Done = TRUE;

								break;

							case GAD_DEFAULT_NAME:
							case GAD_ASCII_UPLOAD_NAME:
							case GAD_ASCII_DOWNLOAD_NAME:
							case GAD_TEXT_UPLOAD_NAME:
							case GAD_TEXT_DOWNLOAD_NAME:
							case GAD_BINARY_UPLOAD_NAME:
							case GAD_BINARY_DOWNLOAD_NAME:

								switch(LT_GetAttributes(Handle,MsgGadget->GadgetID - 1,TAG_DONE))
								{
									case XFER_XPR:

										FindLibDev(PanelWindow,LT_GetString(Handle,MsgGadget->GadgetID),NT_LIBRARY,NULL);

										break;

									case XFER_EXTERNALPROGRAM:

										FindProgram(PanelWindow,LT_GetString(Handle,MsgGadget->GadgetID),NULL);

										break;
								}

								break;

							case GAD_DEFAULT_TYPE:
							case GAD_ASCII_UPLOAD_TYPE:
							case GAD_ASCII_DOWNLOAD_TYPE:
							case GAD_TEXT_UPLOAD_TYPE:
							case GAD_TEXT_DOWNLOAD_TYPE:
							case GAD_BINARY_UPLOAD_TYPE:
							case GAD_BINARY_DOWNLOAD_TYPE:

								LT_SetAttributes(Handle,MsgGadget->GadgetID + 1,
									GA_Disabled,	MsgCode == XFER_DEFAULT || MsgCode == XFER_INTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,MsgGadget->GadgetID + 2,
									GA_Disabled,	MsgCode == XFER_DEFAULT || MsgCode == XFER_INTERNAL,
								TAG_DONE);

								LT_SetAttributes(Handle,MsgGadget->GadgetID + 3,
									GA_Disabled,	MsgCode == XFER_DEFAULT || MsgCode == XFER_EXTERNALPROGRAM,
								TAG_DONE);

								break;

							case GAD_DEFAULT_PREFS:
							case GAD_ASCII_UPLOAD_PREFS:
							case GAD_ASCII_DOWNLOAD_PREFS:
							case GAD_TEXT_UPLOAD_PREFS:
							case GAD_TEXT_DOWNLOAD_PREFS:
							case GAD_BINARY_UPLOAD_PREFS:
							case GAD_BINARY_DOWNLOAD_PREFS:

								Type		= LT_GetAttributes(Handle,MsgGadget->GadgetID - 3,TAG_DONE);
								Contents	= LT_GetString(Handle,MsgGadget->GadgetID - 2);

								if(Type == XFER_XPR)
								{
									if(Contents[0])
									{
										LT_LockWindow(PanelWindow);

										if(ChangeProtocol(Contents,Type))
										{
											if(XProtocolBase)
											{
												XprIO->xpr_filename = NULL;

												XPRCommandSelected = FALSE;

												ClearSerial();

												NewOptions = FALSE;

												TransferBits = XProtocolSetup(XprIO);

												RestartSerial();

												DeleteTransferPanel(TRUE);

													/* Successful? */

												if(!(TransferBits & XPRS_SUCCESS))
												{
													ShowRequest(PanelWindow,LocaleString(MSG_GLOBAL_FAILED_TO_SET_UP_PROTOCOL_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT),LastXprLibrary);

													CloseLibrary(XProtocolBase);

													XProtocolBase = NULL;

													LastXprLibrary[0] = 0;

													TransferBits = 0;

													SetTransferMenu(TRUE);
												}
												else
													SaveProtocolOpts();
											}
										}

										ResetProtocol();

										LT_UnlockWindow(PanelWindow);
									}
								}

								if(Type == XFER_INTERNAL)
								{
									LT_LockWindow(PanelWindow);

									ASCIITransferPanel(PanelWindow,PrivateConfig);

									LT_UnlockWindow(PanelWindow);

									LT_ShowWindow(Handle,TRUE);
								}

								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_DEFAULT_SENDSIGNATURE:
							case GAD_ASCII_UPLOAD_SIGNATURE:
							case GAD_TEXT_UPLOAD_SIGNATURE:
							case GAD_BINARY_UPLOAD_SIGNATURE:

								LT_LockWindow(PanelWindow);

								if(SignaturePanel(PanelWindow,LocalBuffer,FALSE))
									LT_SetAttributes(Handle,MsgGadget->GadgetID,GTST_String,LocalBuffer,TAG_DONE);

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);
								break;

							case GAD_DEFAULT_RECEIVESIGNATURE:
							case GAD_ASCII_DOWNLOAD_SIGNATURE:
							case GAD_TEXT_DOWNLOAD_SIGNATURE:
							case GAD_BINARY_DOWNLOAD_SIGNATURE:

								LT_LockWindow(PanelWindow);

								if(SignaturePanel(PanelWindow,LocalBuffer,TRUE))
									LT_SetAttributes(Handle,MsgGadget->GadgetID,GTST_String,LocalBuffer,TAG_DONE);

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);
								break;

							case GAD_IDENTIFY_COMMAND:

								LT_LockWindow(PanelWindow);

								String = LT_GetString(Handle,MsgGadget->GadgetID);

								while(*String == ' ')
									String++;

								strcpy(LocalBuffer,String);

								for(i = 0 ; i < strlen(LocalBuffer) ; i++, String++)
								{
									if(LocalBuffer[i] == ' ' || LocalBuffer[i] == '\t')
									{
										LocalBuffer[i] = 0;

										break;
									}
								}

								if(i == strlen(LocalBuffer))
									String = NULL;

								if(FileRequester = OpenSingleFile(PanelWindow,NULL,NULL,NULL,LocalBuffer,sizeof(LocalBuffer)))
								{
									FreeAslRequest(FileRequester);

									FindProgram(PanelWindow,LocalBuffer,NULL);

									if(String)
										LimitedStrcat(sizeof(LocalBuffer),LocalBuffer,String);

									LT_SetAttributes(Handle,MsgGadget->GadgetID,GTST_String,LocalBuffer,TAG_DONE);
								}

								LT_UnlockWindow(PanelWindow);

								LT_ShowWindow(Handle,TRUE);
								break;

							default:

								Type = LT_GetAttributes(Handle,MsgGadget->GadgetID - 1,TAG_DONE);

								if(Type == XFER_XPR)
								{
									LT_LockWindow(PanelWindow);

									strcpy(LocalBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

									if(PickFile(PanelWindow,"Libs:","xpr#?.library",LocaleString(MSG_LIBPANEL_SELECT_DEFAULT_TRANSFER_LIBRARY_TXT + (MsgGadget->GadgetID - GAD_DEFAULT_TYPE) / 4),LocalBuffer,NT_LIBRARY))
									{
										FindLibDev(PanelWindow,LocalBuffer,NT_LIBRARY,NULL);

										LT_SetAttributes(Handle,MsgGadget->GadgetID,GTST_String,LocalBuffer,TAG_DONE);
									}

									LT_UnlockWindow(PanelWindow);

									LT_ShowWindow(Handle,TRUE);
								}

								if(Type == XFER_EXTERNALPROGRAM)
								{
									LT_LockWindow(PanelWindow);

									Contents = LT_GetString(Handle,MsgGadget->GadgetID);

									if(ExternalCommandPanel(PanelWindow,Contents))
									{
										FindProgram(PanelWindow,Contents,NULL);

										LT_SetAttributes(Handle,MsgGadget->GadgetID,GTST_String,Contents,TAG_DONE);
									}

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

	FreeVecPooled(Signatures[0]);

	if(UseIt)
	{
		if(Config->MiscConfig->ProtectiveMode && !PrivateConfig->TransferConfig->OverridePath)
		{
			STRPTR Library,Path;

			Library	= NULL;
			Path	= NULL;

			if(PrivateConfig->TransferConfig->DefaultType == XFER_XPR && PrivateConfig->TransferConfig->DefaultLibrary[0])
			{
				Library	= PrivateConfig->TransferConfig->DefaultLibrary;
				Path	= PrivateConfig->PathConfig->BinaryDownloadPath;
			}

			if(PrivateConfig->TransferConfig->ASCIIDownloadType == XFER_XPR && PrivateConfig->TransferConfig->ASCIIDownloadLibrary[0])
			{
				Library	= PrivateConfig->TransferConfig->ASCIIDownloadLibrary;
				Path	= PrivateConfig->PathConfig->ASCIIDownloadPath;
			}

			if(PrivateConfig->TransferConfig->TextDownloadType == XFER_XPR && PrivateConfig->TransferConfig->TextDownloadLibrary[0])
			{
				Library	= PrivateConfig->TransferConfig->TextDownloadLibrary;
				Path	= PrivateConfig->PathConfig->TextDownloadPath;
			}

			if(PrivateConfig->TransferConfig->BinaryDownloadType == XFER_XPR && PrivateConfig->TransferConfig->BinaryDownloadLibrary[0])
			{
				Library	= PrivateConfig->TransferConfig->BinaryDownloadLibrary;
				Path	= PrivateConfig->PathConfig->BinaryDownloadPath;
			}

			if(Library && Path)
			{
				UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

				if(*Path == 0)
				{
					if(LocalGetCurrentDirName(LocalBuffer,sizeof(LocalBuffer)))
						Path = LocalBuffer;
				}

				if(ShowRequest(Window,LocaleString(MSG_XPR_PATH_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Library,Path))
					PrivateConfig->TransferConfig->OverridePath = TRUE;
			}
		}
	}

	if(UseIt)
		SwapConfig(LocalConfig,PrivateConfig);
	else
		SaveConfig(LocalConfig,PrivateConfig);

	return(UseIt);
}
