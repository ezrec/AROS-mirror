/*
**	CursorPanel.c
**
**	Editing panel for cursor key configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_QUALIFIER=1,GAD_UP,GAD_DOWN,GAD_RIGHT,GAD_LEFT,
			GAD_USE,GAD_LOAD,GAD_SAVE,GAD_CANCEL,GAD_DEFAULT,
			GAD_DISCARD
		};

BOOL
CursorPanelConfig(struct Configuration *LocalConfig,struct CursorKeys *CursorKeys,STRPTR LastCursorKeys,struct Window *Parent,BOOL *ChangedPtr)
{
	struct CursorKeys *Keys;
	BOOL Changed;

	Changed = FALSE;

	if(Keys = (struct CursorKeys *)AllocVecPooled(sizeof(struct CursorKeys),MEMF_ANY))
	{
		LayoutHandle *Handle;

		CopyMem(CursorKeys,Keys,sizeof(struct CursorKeys));

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
					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_CURSORPANEL_CURSOR_UP_GAD,
						LA_Chars,		30,
						LA_ID,			GAD_UP,
						GTST_MaxChars,	structsizeof(CursorKeys,Keys[0][0]) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_CURSORPANEL_CURSOR_DOWN_GAD,
						LA_Chars,		30,
						LA_ID,			GAD_DOWN,
						GTST_MaxChars,	structsizeof(CursorKeys,Keys[0][0]) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_CURSORPANEL_CURSOR_RIGHT_GAD,
						LA_Chars,		30,
						LA_ID,			GAD_RIGHT,
						GTST_MaxChars,	structsizeof(CursorKeys,Keys[0][0]) - 1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		STRING_KIND,
						LA_LabelID,		MSG_CURSORPANEL_CURSOR_LEFT_GAD,
						LA_Chars,		30,
						LA_ID,			GAD_LEFT,
						GTST_MaxChars,	structsizeof(CursorKeys,Keys[0][0]) - 1,
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
				LAWN_TitleID,		MSG_CURSORPANEL_CURSOR_PREFERENCES_TXT,
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

				Modifier = 0;

				GuideContext(CONTEXT_CURSOR);

				PushWindow(PanelWindow);

				LT_ShowWindow(Handle,TRUE);

				for(i = 0 ; i < 4 ; i++)
				{
					LT_SetAttributes(Handle,GAD_UP + i,
						GTST_String,Keys->Keys[Modifier][i],
					TAG_DONE);
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
								case GAD_DEFAULT:

									ResetCursorKeys(Keys);

									for(i = 0 ; i < 4 ; i++)
									{
										LT_SetAttributes(Handle,GAD_UP + i,
											GTST_String,Keys->Keys[Modifier][i],
										TAG_DONE);
									}

									break;

								case GAD_QUALIFIER:

									for(i = 0 ; i < 4 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_UP + i));

									Modifier = MsgCode;

									for(i = 0 ; i < 4 ; i++)
									{
										LT_SetAttributes(Handle,GAD_UP + i,
											GTST_String,Keys->Keys[Modifier][i],
										TAG_DONE);
									}

									break;

								case GAD_USE:

									LT_LockWindow(PanelWindow);

									for(i = 0 ; i < 4 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_UP + i));

									CopyMem(Keys,CursorKeys,sizeof(struct CursorKeys));

									Changed = TRUE;

									if(ChangedPtr)
										*ChangedPtr = TRUE;

									Done = TRUE;
									break;

								case GAD_DISCARD:

									if(ChangedPtr)
										*ChangedPtr = FALSE;

									Done = TRUE;
									Changed = TRUE;
									break;

								case GAD_CANCEL:

									Changed = FALSE;
									Done = TRUE;
									break;

								case GAD_LOAD:

									LT_LockWindow(PanelWindow);

									strcpy(DummyBuffer,LastCursorKeys);

									if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_CURSORPANEL_LOAD_CURSOR_KEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
									{
										FreeAslRequest(FileRequest);

										if(!ReadIFFData(DummyBuffer,Keys,sizeof(struct CursorKeys),ID_KEYS))
											ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
										else
										{
											Changed = TRUE;

											if(ChangedPtr)
												*ChangedPtr = FALSE;

											strcpy(LastCursorKeys,DummyBuffer);

											if(LocalConfig)
												strcpy(LocalConfig->CursorFileName,LastCursorKeys);

											for(i = 0 ; i < 4 ; i++)
											{
												LT_SetAttributes(Handle,GAD_UP + i,
													GTST_String,Keys->Keys[Modifier][i],
												TAG_DONE);
											}
										}
									}

									LT_UnlockWindow(PanelWindow);

									break;

								case GAD_SAVE:

									LT_LockWindow(PanelWindow);

									for(i = 0 ; i < 4 ; i++)
										strcpy(Keys->Keys[Modifier][i],LT_GetString(Handle,GAD_UP + i));

									strcpy(DummyBuffer,LastCursorKeys);

									if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_CURSORPANEL_SAVE_CURSOR_KEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
									{
										FreeAslRequest(FileRequest);

										if(!WriteIFFData(DummyBuffer,Keys,sizeof(struct CursorKeys),ID_KEYS))
											ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
										else
										{
											strcpy(LastCursorKeys,DummyBuffer);

											if(LocalConfig)
												strcpy(LocalConfig->CursorFileName,LastCursorKeys);

											if(ChangedPtr)
												*ChangedPtr = FALSE;
										}
									}

									LT_UnlockWindow(PanelWindow);

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
