/*
**	SpeechPanel.c
**
**	Editing panel for speech configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_RATE=1,GAD_PITCH,GAD_FREQUENCY,GAD_VOLUME,GAD_SEX,GAD_ENABLED,GAD_SPEAK,
			GAD_USE,GAD_LOAD,GAD_SAVE,GAD_CANCEL,GAD_DISCARD
		};

STATIC LONG SAVE_DS STACKARGS
VolumeDisplay(struct Gadget *UnusedGadget,LONG Level)
{
	return((100 * (LONG)Level) / MAXVOL);
}

BOOL
SpeechPanelConfig(struct SpeechConfig *SpeechConfig,STRPTR LastSpeech,BOOL *ChangePtr)
{
	struct SpeechConfig PrivateConfig;
	LayoutHandle *Handle;
	STRPTR LevelFormat;
	BOOL Changed;

	Changed = FALSE;

	LevelFormat = LocaleBase ? "%lD" : "%ld";

	CopyMem(SpeechConfig,&PrivateConfig,sizeof(struct SpeechConfig));

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
				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_SPEECHPANEL_RATE_GAD,
					LA_WORD,			&PrivateConfig.Rate,
					LA_Chars,			20,
					LA_ID,				GAD_RATE,
					GTSL_Min,			MINRATE,
					GTSL_Max,			MAXRATE,
					GTSL_LevelFormat,	LevelFormat,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_SPEECHPANEL_PITCH_GAD,
					LA_WORD,			&PrivateConfig.Pitch,
					LA_ID,				GAD_PITCH,
					GTSL_Min,			MINPITCH,
					GTSL_Max,			MAXPITCH,
					GTSL_LevelFormat,	LevelFormat,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_SPEECHPANEL_FREQUENCY_GAD,
					LA_LONG,			&PrivateConfig.Frequency,
					LA_ID,				GAD_FREQUENCY,
					GTSL_Min,			MINFREQ,
					GTSL_Max,			MAXFREQ,
					GTSL_LevelFormat,	LevelFormat,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_SPEECHPANEL_VOLUME_GAD,
					LA_BYTE,			&PrivateConfig.Volume,
					LA_ID,				GAD_VOLUME,
					GTSL_Min,			MINVOL,
					GTSL_Max,			MAXVOL,
					GTSL_LevelFormat,	"%ld%%",
					GTSL_DispFunc,		VolumeDisplay,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			MX_KIND,
					LA_LabelID,			MSG_SPEECHPANEL_SEX_GAD,
					LA_BYTE,			&PrivateConfig.Sex,
					LA_ID,				GAD_SEX,
					LAMX_FirstLabel,	MSG_SPEECHPANEL_MALE_TXT,
					LAMX_LastLabel,		MSG_SPEECHPANEL_FEMALE_TXT,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			CHECKBOX_KIND,
					LA_LabelID,			MSG_SPEECHPANEL_SPEECH_ENABLED_GAD,
					LA_BYTE,			&PrivateConfig.Enabled,
					LA_ID,				GAD_ENABLED,
					LA_ExtraSpace,		TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,			XBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			BUTTON_KIND,
					LA_LabelID,			MSG_SPEECHPANEL_SPEAK_GAD,
					LA_ID,				GAD_SPEAK,
					LABT_ExtraFat,		TRUE,
					GA_Disabled,		!PrivateConfig.Enabled,
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
			LAWN_TitleID,		MSG_SPEECHPANEL_SPEECH_PREFERENCES_TXT,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			UBYTE DummyBuffer[MAX_FILENAME_LENGTH],*DummyChar;
			struct FileRequester *FileRequest;
			struct IntuiMessage *Message;
			struct Gadget *MsgGadget;
			BOOL OldEnglish;
			ULONG MsgClass;
			UWORD MsgCode;
			BOOL Done;

			OldEnglish = English;

			GuideContext(CONTEXT_SPEECH);

			PushWindow(PanelWindow);

			Done = FALSE;

			do
			{
				if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
				{
					Changed = FALSE;

					break;
				}

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
							case GAD_ENABLED:

								LT_SetAttributes(Handle,GAD_SPEAK,
									GA_Disabled,!MsgCode,
								TAG_DONE);

								break;

							case GAD_USE:

								Changed = Done = TRUE;

								if(ChangePtr)
									*ChangePtr = TRUE;

								CopyMem(&PrivateConfig,SpeechConfig,sizeof(struct SpeechConfig));

								break;

							case GAD_DISCARD:

								LastSpeech[0] = 0;

								if(ChangePtr)
									*ChangePtr = FALSE;

								strcpy(Config->SpeechFileName,LastSpeech);

								Done = Changed = TRUE;
								break;

							case GAD_CANCEL:

								Done = TRUE;
								Changed = FALSE;
								break;

							case GAD_LOAD:

								SplitFileName(LastSpeech,&DummyChar,DummyBuffer);

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastSpeech);

								if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_SPEECHPANEL_LOAD_SPEECH_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!ReadIFFData(DummyBuffer,&PrivateConfig,sizeof(struct SpeechConfig),ID_SPEK))
										ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastSpeech,DummyBuffer);

										strcpy(Config->SpeechFileName,LastSpeech);

										LT_SetAttributes(Handle,GAD_RATE,
											GTSL_Level,		PrivateConfig.Rate,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_PITCH,
											GTSL_Level,		PrivateConfig.Pitch,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_FREQUENCY,
											GTSL_Level,		PrivateConfig.Frequency,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_VOLUME,
											GTSL_Level,		PrivateConfig.Volume,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_SEX,
											GTCY_Active,	PrivateConfig.Sex,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_ENABLED,
											GTCB_Checked,	PrivateConfig.Enabled,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_SPEAK,
											GA_Disabled,	PrivateConfig.Enabled == FALSE,
										TAG_DONE);

										Changed = TRUE;

										if(ChangePtr)
											*ChangePtr = FALSE;
									}
								}

								LT_UnlockWindow(PanelWindow);

								break;

							case GAD_SAVE:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastSpeech);

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_SPEECHPANEL_SAVE_SPEECH_SETTINGS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!WriteIFFData(DummyBuffer,&PrivateConfig,sizeof(struct SpeechConfig),ID_SPEK))
										ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastSpeech,DummyBuffer);

										strcpy(Config->SpeechFileName,LastSpeech);

										if(ChangePtr)
											*ChangePtr = FALSE;
									}
								}

								LT_UnlockWindow(PanelWindow);

								break;

							case GAD_SPEAK:

								SwapMem(SpeechConfig,&PrivateConfig,sizeof(struct SpeechConfig));

								SpeechSetup();

								English = TRUE;

								Say(LocaleString(MSG_SPEECHPANEL_THIS_IS_TERM_SPEAKING_TXT));

								SwapMem(SpeechConfig,&PrivateConfig,sizeof(struct SpeechConfig));

								English = OldEnglish;

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

	SpeechSetup();

	return(Changed);
}
