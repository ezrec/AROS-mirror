/*
**	SoundPanel.c
**
**	Editing panel for sound configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_BELL=1,GAD_CONNECT,GAD_DISCONNECT,GAD_GOOD_TRANSFER,
			GAD_BAD_TRANSFER,GAD_RING,GAD_VOICE,GAD_ERROR,

			GAD_PLAY_BELL,GAD_PLAY_CONNECT,GAD_PLAY_DISCONNECT,GAD_PLAY_GOOD_TRANSFER,
			GAD_PLAY_BAD_TRANSFER,GAD_PLAY_RING,GAD_PLAY_VOICE,GAD_PLAY_ERROR,

			GAD_PRELOAD,GAD_VOLUME,
			GAD_USE,GAD_LOAD,GAD_SAVE,GAD_CANCEL,GAD_DISCARD
		};

BOOL
SoundPanelConfig(struct SoundConfig *SoundConfig,STRPTR LastSound,BOOL *ChangedPtr)
{
	struct SoundConfig *BackupConfig;

	struct LayoutHandle *Handle;
	BOOL Changed = FALSE;

	if(!(BackupConfig = (struct SoundConfig *)AllocVecPooled(sizeof(struct SoundConfig),MEMF_ANY)))
	{
		DisplayBeep(NULL);
		return(FALSE);
	}

	CopyMem(SoundConfig,BackupConfig,sizeof(struct SoundConfig));

	if(Handle = LT_CreateHandleTags(Window->WScreen,
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
				STATIC WORD LabelID[] =
				{
					MSG_SOUNDPANEL_BELL_GAD,
					MSG_SOUNDPANEL_CONNECT_GAD,
					MSG_SOUNDPANEL_DISCONNECT_GAD,
					MSG_SOUNDPANEL_GOOD_TRANSFER_GAD,
					MSG_SOUNDPANEL_BAD_TRANSFER_GAD,
					MSG_SOUNDPANEL_RING_GAD,
					MSG_SOUNDPANEL_VOICE_GAD,
					MSG_ERROR_NOTIFICATION_SOUND_TXT
				};

				LONG i,Max,Size;

				for(i = Max = 0 ; i < NUM_ELEMENTS(LabelID) ; i++)
				{
					Size = LT_LabelChars(Handle,LocaleString(LabelID[i]));

					if(Size > Max)
						Max = Size;
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_BELL_GAD,
						LA_STRPTR,		BackupConfig->BellFile,
						LA_Chars,		30,
						LA_ID,			GAD_BELL,
						LAST_Picker,	TRUE,
						GTST_MaxChars,	sizeof(BackupConfig->BellFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_BELL,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_CONNECT_GAD,
						LA_STRPTR,		BackupConfig->ConnectFile,
						LA_ID,			GAD_CONNECT,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->ConnectFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_CONNECT,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_DISCONNECT_GAD,
						LA_STRPTR,		BackupConfig->DisconnectFile,
						LA_ID,			GAD_DISCONNECT,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->DisconnectFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_DISCONNECT,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_GOOD_TRANSFER_GAD,
						LA_STRPTR,		BackupConfig->GoodTransferFile,
						LA_ID,			GAD_GOOD_TRANSFER,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->GoodTransferFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_GOOD_TRANSFER,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_BAD_TRANSFER_GAD,
						LA_STRPTR,		BackupConfig->BadTransferFile,
						LA_ID,			GAD_BAD_TRANSFER,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->BadTransferFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_BAD_TRANSFER,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_RING_GAD,
						LA_STRPTR,		BackupConfig->RingFile,
						LA_ID,			GAD_RING,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->RingFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_RING,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_SOUNDPANEL_VOICE_GAD,
						LA_STRPTR,		BackupConfig->VoiceFile,
						LA_ID,			GAD_VOICE,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->VoiceFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_VOICE,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelChars,	Max,
						LA_LabelID,		MSG_ERROR_NOTIFICATION_SOUND_TXT,
						LA_STRPTR,		BackupConfig->ErrorNotifyFile,
						LA_ID,			GAD_ERROR,
						LAST_Picker,	TRUE,
						LA_Chars,		30,
						GTST_MaxChars,	sizeof(BackupConfig->ErrorNotifyFile) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		TAPEDECK_KIND,
						LA_ID,			GAD_PLAY_ERROR,
						LATD_Smaller,	TRUE,
						LATD_ButtonType,TDBT_Play,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	XBAR_KIND,
			TAG_DONE);

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_SOUNDPANEL_VOLUME_GAD,
					LA_BYTE,			&BackupConfig->Volume,
					LA_ID,				GAD_VOLUME,
					LA_Chars,			10,
					GTSL_Min,			0,
					GTSL_Max,			100,
					GTSL_LevelFormat,	"%ld%%",
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			CHECKBOX_KIND,
					LA_LabelID,			MSG_SOUNDPANEL_PRELOAD_GAD,
					LA_BYTE,			&BackupConfig->Preload,
					LA_ID,				GAD_PRELOAD,
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
					LA_LabelID,		MSG_GLOBAL_LOAD_GAD,
					LA_ID,			GAD_LOAD,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_SAVE_GAD,
					LA_ID,			GAD_SAVE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_DISCARD_GAD,
					LA_ID,			GAD_DISCARD,
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
			LAWN_TitleID,		MSG_SOUNDPANEL_SOUND_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			struct IntuiMessage		*Message;
			BOOL					 Done = FALSE;
			ULONG					 MsgClass;
			struct Gadget			*MsgGadget;
			UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
			struct FileRequester	*FileRequest;

			GuideContext(CONTEXT_SOUND);

			PushWindow(PanelWindow);

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

								CopyMem(BackupConfig,SoundConfig,sizeof(struct SoundConfig));

								Changed = TRUE;

								if(ChangedPtr)
									*ChangedPtr = TRUE;

								Done = TRUE;
								break;

							case GAD_DISCARD:

								Changed = Done = TRUE;

								if(ChangedPtr)
									*ChangedPtr = FALSE;

								LastSound[0] = 0;

								strcpy(Config->SoundFileName,LastSound);

								break;

							case GAD_CANCEL:

								Changed = FALSE;
								Done = TRUE;
								break;

							case GAD_LOAD:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastSound);

								if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_SOUNDPANEL_LOAD_SOUNDS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!ReadIFFData(DummyBuffer,BackupConfig,sizeof(struct SoundConfig),ID_SOUN))
										ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									else
									{
										Changed = TRUE;

										if(ChangedPtr)
											*ChangedPtr = FALSE;

										strcpy(LastSound,DummyBuffer);

										strcpy(Config->SoundFileName,LastSound);

										LT_SetAttributes(Handle,GAD_BELL,
											GTST_String,BackupConfig->BellFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_CONNECT,
											GTST_String,BackupConfig->ConnectFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_DISCONNECT,
											GTST_String,BackupConfig->DisconnectFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_GOOD_TRANSFER,
											GTST_String,BackupConfig->GoodTransferFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_BAD_TRANSFER,
											GTST_String,BackupConfig->BadTransferFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_RING,
											GTST_String,BackupConfig->RingFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_VOICE,
											GTST_String,BackupConfig->VoiceFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_ERROR,
											GTST_String,BackupConfig->ErrorNotifyFile,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_PRELOAD,
											GTCB_Checked,BackupConfig->Preload,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_VOLUME,
											GTSL_Level,BackupConfig->Volume,
										TAG_DONE);
									}
								}

								LT_UnlockWindow(PanelWindow);

								break;

							case GAD_SAVE:

								LT_UpdateStrings(Handle);

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastSound);

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_SOUNDPANEL_SAVE_SOUNDS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!WriteIFFData(DummyBuffer,BackupConfig,sizeof(struct SoundConfig),ID_SOUN))
										ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastSound,DummyBuffer);

										strcpy(Config->SoundFileName,LastSound);

										if(ChangedPtr)
											*ChangedPtr = FALSE;
									}
								}

								LT_UnlockWindow(PanelWindow);

								break;

							default:

								if(MsgGadget->GadgetID >= GAD_PLAY_BELL && MsgGadget->GadgetID <= GAD_PLAY_ERROR)
								{
									STRPTR Name = LT_GetString(Handle,MsgGadget->GadgetID - GAD_PLAY_BELL + GAD_BELL);

									if(Name[0])
									{
										if(FindFile(PanelWindow,Name,NULL))
										{
											if(BackupConfig->Volume)
											{
												struct SoundInfo *SoundInfo;

												LT_LockWindow(PanelWindow);

												if(SoundInfo = LoadSound(Name,FALSE))
												{
													PlaySound(SoundInfo);

													FreeSound(SoundInfo);
												}

												LT_UnlockWindow(PanelWindow);
											}
										}
									}
								}

								break;
						}
					}

					if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadget->GadgetID >= GAD_BELL && MsgGadget->GadgetID <= GAD_ERROR)
					{
						strcpy(DummyBuffer,LT_GetString(Handle,MsgGadget->GadgetID));

						if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_SELECT_SOUND_FILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
						{
							FreeAslRequest(FileRequest);

							FindFile(PanelWindow,DummyBuffer,NULL);

							LT_SetAttributes(Handle,MsgGadget->GadgetID,
								GTST_String,DummyBuffer,
							TAG_DONE);
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	FreeVecPooled(BackupConfig);

	return(Changed);
}
