/*
**	PathPanel.c
**
**	Editing panel for path configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_AUPLOAD=1000,GAD_ADOWNLOAD,GAD_TUPLOAD,GAD_TDOWNLOAD,
			GAD_BUPLOAD,GAD_BDOWNLOAD,GAD_CONFIGSTORE,GAD_EDITOR,GAD_HELPFILE,

			GAD_USE,GAD_CANCEL,
			GAD_PAGER,GAD_PAGEGROUP
		};

BOOL
PathPanel(struct Window *Parent,struct Configuration *LocalConfig)
{
	STATIC LONG PageTitleID[5] =
	{
		MSG_V36_0123,
		MSG_V36_0122,
		MSG_V36_0121,
		MSG_V36_0124,
		-1
	};

	STATIC BYTE InitialPage = 0;

	struct LayoutHandle	*Handle;
	BOOL UseIt;

	UseIt = FALSE;

	SaveConfig(LocalConfig,PrivateConfig);

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
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		TAB_KIND,
					LA_ID,			GAD_PAGER,
					LA_LabelID,		MSG_XFER_PAGE_TXT,
					LACY_LabelTable,PageTitleID,
					LACY_AutoPageID,GAD_PAGEGROUP,
					LACY_TabKey,	TRUE,
					LA_BYTE,		&InitialPage,
					LATB_FullWidth,	TRUE,
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
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_BINARY_UPLOAD_PATH_GAD,
						LA_ID,			GAD_BUPLOAD,
						LA_Chars,		20,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->BinaryUploadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->BinaryUploadPath,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_BDOWNLOAD,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_BINARY_DOWNLOAD_PATH_GAD,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->BinaryDownloadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->BinaryDownloadPath,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_TEXT_UPLOAD_PATH_GAD,
						LA_ID,			GAD_TUPLOAD,
						LA_Chars,		20,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->TextUploadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->TextUploadPath,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_TDOWNLOAD,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_TEXT_DOWNLOAD_PATH_GAD,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->TextDownloadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->TextDownloadPath,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_ASCII_UPLOAD_PATH_GAD,
						LA_ID,			GAD_AUPLOAD,
						LA_Chars,		20,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->ASCIIUploadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->ASCIIUploadPath,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_ADOWNLOAD,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_ASCII_DOWNLOAD_PATH_GAD,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->ASCIIDownloadPath) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->ASCIIDownloadPath,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_PATHPANEL_CONFIGURATION_STORAGE_DIR_GAD,
						LA_ID,			GAD_CONFIGSTORE,
						LA_Chars,		20,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->DefaultStorage) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->DefaultStorage,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_EDITOR,
						LA_LabelID,		MSG_PATHPANEL_DEFAULT_TEXT_EDITOR_GAD,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->Editor) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->Editor,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_ID,			GAD_HELPFILE,
						LA_LabelID,		MSG_PATHPANEL_HELP_TEXT_FILE_GAD,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(PrivateConfig->PathConfig->HelpFile) - 1,
						LA_STRPTR,		PrivateConfig->PathConfig->HelpFile,
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
			LAWN_TitleID,		MSG_PATHPANEL_PATH_PREFERENCES_TXT,
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
			BOOL				 Done = FALSE;
			ULONG				 MsgClass;
			struct Gadget		*MsgGadget;

			GuideContext(CONTEXT_PATHS);

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

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

								LT_UpdateStrings(Handle);

								UseIt = Done = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								break;

							default:

								if(MsgGadget->GadgetID >= GAD_AUPLOAD && MsgGadget->GadgetID <= GAD_HELPFILE)
								{
									if(TypeOfMem((APTR)LT_GetAttributes(Handle,MsgGadget->GadgetID,TAG_DONE)))
									{
										UBYTE DummyBuffer[MAX_FILENAME_LENGTH];

										strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

										if(MsgGadget->GadgetID < GAD_EDITOR)
											FindDrawer(PanelWindow,DummyBuffer,TRUE,NULL);
										else
										{
											if(DummyBuffer[0])
											{
												if(MsgGadget->GadgetID == GAD_EDITOR)
													FindProgram(PanelWindow,DummyBuffer,NULL);
												else
													FindFile(PanelWindow,DummyBuffer,NULL);
											}
										}
									}
								}

								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE)
					{
						LT_LockWindow(PanelWindow);

						if(MsgGadget->GadgetID >= GAD_AUPLOAD && MsgGadget->GadgetID <= GAD_HELPFILE)
						{
							LONG	ID		= MsgGadget->GadgetID;
							STRPTR	Name	= LT_GetString(Handle,ID);

							if(Name)
							{
								UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
								struct FileRequester *FileRequest;
								BOOL Saver,Drawer;

								strcpy(DummyBuffer,Name);

								Saver = FALSE;

								if(ID < GAD_EDITOR)
								{
									Drawer = TRUE;

									switch(ID)
									{
										case GAD_ADOWNLOAD:
										case GAD_TDOWNLOAD:
										case GAD_BDOWNLOAD:

											Saver = TRUE;
											break;
									}
								}
								else
									Drawer = FALSE;

								if(Drawer)
								{
									if(Saver)
										FileRequest = SaveDrawer(PanelWindow,LocaleString(ID - GAD_AUPLOAD + MSG_PATHPANEL_SELECT_DEFAULT_ASCII_UPLOAD_PATH_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),DummyBuffer,sizeof(DummyBuffer));
									else
										FileRequest = OpenDrawer(PanelWindow,LocaleString(ID - GAD_AUPLOAD + MSG_PATHPANEL_SELECT_DEFAULT_ASCII_UPLOAD_PATH_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),DummyBuffer,sizeof(DummyBuffer));
								}
								else
									FileRequest = OpenSingleFile(PanelWindow,LocaleString(ID - GAD_AUPLOAD + MSG_PATHPANEL_SELECT_DEFAULT_ASCII_UPLOAD_PATH_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer));

								if(FileRequest)
								{
									FreeAslRequest(FileRequest);

									if(Drawer)
										FindDrawer(PanelWindow,DummyBuffer,TRUE,NULL);
									else
									{
										if(ID == GAD_EDITOR)
											FindProgram(PanelWindow,DummyBuffer,NULL);
										else
											FindFile(PanelWindow,DummyBuffer,NULL);
									}

									LT_SetAttributes(Handle,ID,
										GTST_String,DummyBuffer,
									TAG_DONE);
								}
							}
						}

						LT_UnlockWindow(PanelWindow);
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	if(UseIt)
	{
		if(Config->MiscConfig->ProtectiveMode && !PrivateConfig->TransferConfig->OverridePath)
		{
			STRPTR	Library	= NULL,
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
