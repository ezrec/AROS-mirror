/*
**	MacroPanel.c
**
**	Editing panel for key macro configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_QUALIFIER=1,GAD_F1,GAD_F2,GAD_F3,GAD_F4,GAD_F5,GAD_F6,GAD_F7,
			GAD_F8,GAD_F9,GAD_F10,GAD_USE,GAD_LOAD,GAD_SAVE,GAD_CANCEL,
			GAD_DISCARD
		};

BOOL
MacroPanelConfig(struct Configuration *LocalConfig,struct MacroKeys *MacroKeys,STRPTR LastMacros,struct Window *Parent,BOOL *ChangePtr)
{
	struct MacroKeys *Keys;
	BOOL Changed;

	Changed = FALSE;

	if(Keys = (struct MacroKeys *)AllocVecPooled(sizeof(struct MacroKeys),MEMF_ANY))
	{
		struct LayoutHandle *Handle;

		CopyMem(MacroKeys,Keys,sizeof(struct MacroKeys));

		if(Handle = LT_CreateHandleTags(Parent->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			struct Window *PanelWindow;

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		CYCLE_KIND,
						LA_LabelID,		MSG_MACROPANEL_KEY_MODIFIER_GAD,
						LA_ID,			GAD_QUALIFIER,
						LACY_TabKey,	TRUE,
						LACY_FirstLabel,MSG_MACROPANEL_NONE_TXT,
						LACY_LastLabel,	MSG_MACROPANEL_CONTROL_TXT,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		XBAR_KIND,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					STATIC STRPTR Labels[10] =
					{
						"F _1",
						"F _2",
						"F _3",
						"F _4",
						"F _5",
						"F _6",
						"F _7",
						"F _8",
						"F _9",
						"F1_0"
					};

					LONG i;

					for(i = 0 ; i < 10 ; i++)
					{
						LT_New(Handle,
							LA_Type,		STRING_KIND,
							LA_LabelText,	Labels[i],
							LA_Chars,		39,
							LA_ID,			GAD_F1 + i,
							GTST_MaxChars,	structsizeof(MacroKeys,Keys[0][0]) - 1,
						TAG_DONE);
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
				LAWN_TitleID,		MSG_MACROPANEL_MACRO_PREFERENCES_TXT,
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
				struct Gadget *MsgGadget;
				LONG Modifier,i;
				ULONG MsgClass;
				UWORD MsgCode;
				BOOL Done;

				GuideContext(CONTEXT_MACROS);

				PushWindow(PanelWindow);

				LT_ShowWindow(Handle,TRUE);

				Modifier = 0;

				if(XEmulatorBase && LocalConfig && LocalConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
				{
					ULONG KeyMask = XEmulatorGetFreeMacroKeys(XEM_IO,Modifier);

					for(i = 0 ; i < 10 ; i++)
					{
						LT_SetAttributes(Handle,GAD_F1 + i,
							GTST_String,	Keys->Keys[Modifier][i],
							GA_Disabled,	!(KeyMask & (1L << i)),
						TAG_DONE);
					}
				}
				else
				{
					for(i = 0 ; i < 10 ; i++)
					{
						LT_SetAttributes(Handle,GAD_F1 + i,
							GTST_String,Keys->Keys[Modifier][i],
						TAG_DONE);
					}
				}

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
								case GAD_QUALIFIER:

									for(i = 0 ; i < 10 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_F1 + i));

									Modifier = MsgCode;

									if(XEmulatorBase && LocalConfig && LocalConfig->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
									{
										ULONG KeyMask = XEmulatorGetFreeMacroKeys(XEM_IO,Modifier);

										for(i = 0 ; i < 10 ; i++)
										{
											LT_SetAttributes(Handle,GAD_F1 + i,
												GTST_String,	Keys->Keys[Modifier][i],
												GA_Disabled,	!(KeyMask & (1L << i)),
											TAG_DONE);
										}
									}
									else
									{
										for(i = 0 ; i < 10 ; i++)
										{
											LT_SetAttributes(Handle,GAD_F1 + i,
												GTST_String,Keys->Keys[Modifier][i],
											TAG_DONE);
										}
									}

									break;

								case GAD_USE:

									LT_LockWindow(PanelWindow);

									for(i = 0 ; i < 10 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_F1 + i));

									CopyMem(Keys,MacroKeys,sizeof(struct MacroKeys));

									Changed = TRUE;

									if(ChangePtr)
										*ChangePtr |= TRUE;

									Done = TRUE;
									break;

								case GAD_LOAD:

									strcpy(DummyBuffer,LastMacros);

									LT_LockWindow(PanelWindow);

									if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_MACROPANEL_LOAD_MACRO_KEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
									{
										FreeAslRequest(FileRequest);

										if(!LoadMacros(DummyBuffer,Keys))
											ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
										else
										{
											Changed = TRUE;

											if(ChangePtr)
												*ChangePtr = FALSE;

											strcpy(LastMacros,DummyBuffer);

											if(LocalConfig)
												strcpy(LocalConfig->MacroFileName,LastMacros);

											for(i = 0 ; i < 10 ; i++)
											{
												LT_SetAttributes(Handle,GAD_F1 + i,
													GTST_String,Keys->Keys[Modifier][i],
												TAG_DONE);
											}
										}
									}

									LT_UnlockWindow(PanelWindow);

									break;

								case GAD_SAVE:

									strcpy(DummyBuffer,LastMacros);

									LT_LockWindow(PanelWindow);

									for(i = 0 ; i < 10 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_F1 + i));

									if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_MACROPANEL_SAVE_MACRO_KEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
									{
										FreeAslRequest(FileRequest);

										if(!WriteIFFData(DummyBuffer,Keys,sizeof(struct MacroKeys),ID_KEYS))
											ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
										else
										{
											strcpy(LastMacros,DummyBuffer);

											if(LocalConfig)
												strcpy(LocalConfig->MacroFileName,LastMacros);

											if(ChangePtr)
												*ChangePtr = FALSE;
										}
									}

									LT_UnlockWindow(PanelWindow);

									break;

								case GAD_DISCARD:

									Done = Changed = TRUE;

									if(ChangePtr)
										*ChangePtr = FALSE;

									LastMacros[0] = 0;

									if(LocalConfig)
										strcpy(LocalConfig->MacroFileName,LastMacros);

									break;

								case GAD_CANCEL:

									Done = TRUE;
									Changed = FALSE;
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

		FreeVecPooled(Keys);
	}

	return(Changed);
}
