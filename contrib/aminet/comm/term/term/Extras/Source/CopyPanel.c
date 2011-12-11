/*
**	CopyPanel.c
**
**	Editing panel for configuration copying
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_SERIAL=1,GAD_MODEM,GAD_SCREEN,GAD_TERMINAL,GAD_EMULATION,
			GAD_CLIPBOARD,GAD_CAPTURE,GAD_COMMANDS,GAD_MISC,GAD_PATHS,GAD_TRANSFER,
			GAD_TRANSLATION,GAD_KEYMACROS,GAD_CURSORKEYS,GAD_FASTMACROS,GAD_COPYTOALL,
			GAD_DEFAULT,GAD_ALL,GAD_CLEAR,GAD_USE,GAD_CANCEL
		};

enum	{	COPY_MAIN_CONFIG,COPY_DEFAULTS };

STATIC ULONG	Bits = 0;

STATIC BYTE		CopyMode = COPY_MAIN_CONFIG;
STATIC BOOL		AllFileSettings = FALSE;

STATIC VOID
CopyConfig(struct Configuration *LocalConfig,LONG WhichPart)
{
	STATIC LONG TypeMappings[][2] =
	{
	    {
		GAD_SERIAL,			PREF_SERIAL,
	    }, {
		GAD_MODEM,			PREF_MODEM,
	    }, {
		GAD_SCREEN,			PREF_SCREEN,
	    }, {
		GAD_TERMINAL,		PREF_TERMINAL,
	    }, {
		GAD_EMULATION,		PREF_EMULATION,
	    }, {
		GAD_CLIPBOARD,		PREF_CLIP,
	    }, {
		GAD_CAPTURE,		PREF_CAPTURE,
	    }, {
		GAD_COMMANDS,		PREF_COMMAND,
	    }, {
		GAD_MISC,			PREF_MISC,
	    }, {
		GAD_PATHS,			PREF_PATH,
	    }, {
		GAD_TRANSFER,		PREF_TRANSFER,
	    }, {
		GAD_TRANSLATION,	PREF_TRANSLATIONFILENAME,
	    }, {
		GAD_KEYMACROS,		PREF_MACROFILENAME,
	    }, {
		GAD_CURSORKEYS,		PREF_CURSORFILENAME,
	    }, {
		GAD_FASTMACROS,		PREF_FASTMACROFILENAME,
	    }, {
		-1
	    }
	};

	LONG i;

	for(i = 0 ; TypeMappings[i][0] != -1 ; i++)
	{
		if(WhichPart == TypeMappings[i][0])
		{
			LONG Type;
			APTR Data;

			Type = TypeMappings[i][1];
			Data = GetConfigEntry(LocalConfig,Type);

			if(CopyMode == COPY_MAIN_CONFIG)
			{
				if(CreateConfigEntry(LocalConfig,Type))
				{
					if(Data)
						PutConfigEntry(LocalConfig,Data,Type);
					else
						ResetConfigEntry(LocalConfig,Type);
				}
			}
			else
			{
				if(Data && (Type != PREF_FILE || AllFileSettings))
					DeleteConfigEntry(LocalConfig,Type);
			}

			break;
		}
	}
}

BOOL
CopyPanel(struct Window *Parent,struct Configuration *LocalConfig,BOOL Selective)
{
	LayoutHandle *Handle;
	BOOL MadeChanges;

	MadeChanges = FALSE;

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
				LA_LabelID,	MSG_V36_0028,
			TAG_DONE);
			{
				LONG i;

				for(i = 0 ; i <= GAD_FASTMACROS - GAD_SERIAL ; i++)
				{
					if(!(i % 5))
					{
						LT_New(Handle,
							LA_Type,	VERTICAL_KIND,
						TAG_DONE);
					}

					LT_New(Handle,
						LA_Type,		CHECKBOX_KIND,
						LA_LabelID,		MSG_COPYPANEL_SERIAL_GAD + i,
						LA_ID,			GAD_SERIAL + i,
						GTCB_Checked,	Bits & (1L << i),
					TAG_DONE);

					if(!((i + 1) % 5))
						LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	XBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		CHECKBOX_KIND,
					LA_LabelID,		MSG_COPYPANEL_TO_ALL_ENTRIES_GAD,
					LA_ID,			GAD_COPYTOALL,
					GTCB_Checked,	!LocalConfig,
					GA_Disabled,	!LocalConfig,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		MX_KIND,
					LA_LabelID,		MSG_COPYPANEL_COPY_TYPE_GAD,
					LA_ID,			GAD_DEFAULT,
					LA_BYTE,		&CopyMode,
					LAMX_FirstLabel,MSG_COPYPANEL_COPY_GLOBAL_CONFIG_MSG,
					LAMX_LastLabel,	MSG_COPYPANEL_COPY_GLOBAL_DEFAULTS_MSG,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	XBAR_KIND,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	BUTTON_KIND,
					LA_LabelID,	MSG_COPYPANEL_ALL_GAD,
					LA_ID,		GAD_ALL,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	BUTTON_KIND,
					LA_LabelID,	MSG_COPYPANEL_CLEAR_GAD,
					LA_ID,		GAD_CLEAR,
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
			LAWN_TitleID,		MSG_COPYPANEL_COPY_CONFIG_TXT,
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

			Done = FALSE;

			GuideContext(CONTEXT_COPY);

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
						LONG i;

						switch(MsgGadget->GadgetID)
						{
							case GAD_ALL:

								for(i = GAD_SERIAL ; i <= GAD_FASTMACROS ; i++)
									LT_SetAttributes(Handle,i,GTCB_Checked,TRUE,TAG_DONE);

								break;

							case GAD_CLEAR:

								for(i = GAD_SERIAL ; i <= GAD_FASTMACROS ; i++)
									LT_SetAttributes(Handle,i,GTCB_Checked,FALSE,TAG_DONE);

								break;

							case GAD_USE:

								Bits = 0;

								if(LT_GetAttributes(Handle,GAD_COPYTOALL,TAG_DONE))
								{
									LONG j;

									AllFileSettings = TRUE;

									for(i = GAD_TRANSLATION ; i <= GAD_FASTMACROS ; i++)
										AllFileSettings &= LT_GetAttributes(Handle,i,TAG_DONE);

									if(Selective)
									{
										for(i = 0 ; i <= GAD_FASTMACROS - GAD_SERIAL ; i++)
										{
											if(LT_GetAttributes(Handle,GAD_SERIAL + i,TAG_DONE))
											{
												for(j = 0 ; j < GlobalPhoneHandle->NumPhoneEntries ; j++)
												{
													if(GlobalPhoneHandle->Phonebook[j]->Count != -1)
														CopyConfig(GlobalPhoneHandle->Phonebook[j]->Config,GAD_SERIAL + i);
												}

												Bits |= (1L << i);

												MadeChanges = TRUE;
											}
										}
									}
									else
									{
										for(i = 0 ; i <= GAD_FASTMACROS - GAD_SERIAL ; i++)
										{
											if(LT_GetAttributes(Handle,GAD_SERIAL + i,TAG_DONE))
											{
												for(j = 0 ; j < GlobalPhoneHandle->NumPhoneEntries ; j++)
													CopyConfig(GlobalPhoneHandle->Phonebook[j]->Config,GAD_SERIAL + i);

												Bits |= (1L << i);

												MadeChanges = TRUE;
											}
										}
									}
								}
								else
								{
									if(LocalConfig)
									{
										for(i = 0 ; i <= GAD_FASTMACROS - GAD_SERIAL ; i++)
										{
											if(LT_GetAttributes(Handle,GAD_SERIAL + i,TAG_DONE))
											{
												CopyConfig(LocalConfig,GAD_SERIAL + i);

												Bits |= (1L << i);

												MadeChanges = TRUE;
											}
										}
									}
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

	return(MadeChanges);
}
