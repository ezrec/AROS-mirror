/*
**	HotkeyPanel.c
**
**	Editing panel for hotkey configuration
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_TERMTOFRONT=1,GAD_BUFFERTOFRONT,GAD_SKIPDIAL,GAD_ABORTAREXX,
			GAD_PRIORITY,GAD_HOTKEYS,GAD_USE,GAD_LOAD,GAD_SAVE,GAD_CANCEL,
			GAD_DISCARD
		};

struct Library *KeymapBase;

struct CodeName
{
	ULONG	Code;
	STRPTR	Name;
};

STATIC BOOL
GoodCode(STRPTR Code)
{
	IX Expression;

	return((BOOL)(ParseIX(Code,&Expression) == 0));
}

	/* EditRoutine():
	 *
	 *	A special string gadget editing routine for key
	 *	combination input.
	 */

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
EditRoutine(REG(a0) struct Hook *UnusedHook,REG(a2) struct SGWork *Work,REG(a1) Msg msg)
#else
AROS_UFH3(STATIC ULONG, EditRoutine,
 AROS_UFHA(struct Hook *      , UnusedHook, A0),
 AROS_UFHA(struct SGWork *    , Work      , A2),
 AROS_UFHA(Msg                , msg       , A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
		/* ANSI key codes. */

	STATIC struct CodeName KeyTable[] =
	{
		'\r',	"Return",
		'\b',	"Backspace",
		'\033',	"Escape",
		' ',	"Spacebar",
		',',	"Comma",
		'\177',	"Delete",
		'\t',	"Tab",

		0
	};

		/* Special raw key codes */

	STATIC struct CodeName RawTable[] =
	{
		CURSOR_UP_CODE,		"Cursor_Up",
		CURSOR_DOWN_CODE,	"Cursor_Down",
		CURSOR_RIGHT_CODE,	"Cursor_Right",
		CURSOR_LEFT_CODE,	"Cursor_Left",

		F01_CODE,			"F1",
		F02_CODE,			"F2",
		F03_CODE,			"F3",
		F04_CODE,			"F4",
		F05_CODE,			"F5",
		F06_CODE,			"F6",
		F07_CODE,			"F7",
		F08_CODE,			"F8",
		F09_CODE,			"F9",
		F10_CODE,			"F10",

		HELP_CODE,			"Help",

		0
	};

		/* Qualifier key codes. */

	STATIC struct CodeName QualifierTable[] =
	{
		SHIFT_KEY,				"Shift ",
		ALT_KEY,				"Alt ",
		CONTROL_KEY,			"Control ",
		IEQUALIFIER_LCOMMAND,	"LAmiga ",
		IEQUALIFIER_RCOMMAND,	"RAmiga ",
		IEQUALIFIER_LEFTBUTTON,	"Left_Button ",
		IEQUALIFIER_MIDBUTTON,	"Middle_Button ",
		IEQUALIFIER_RBUTTON,	"Right_Button ",
		IEQUALIFIER_NUMERICPAD,	"Numeric_Pad ",

		0
	};

	ULONG Qualifier;
	UBYTE Key[10];
	STRPTR KeyName;
	LONG KeyLen;
	BOOL GotIt;
	LONG i;

	switch(msg->MethodID)
	{
		case SGH_KEY:

				/* If the caps lock key is active, don't change anything */

			if(Work->IEvent->ie_Qualifier & IEQUALIFIER_CAPSLOCK)
				break;

				/* Check for Amiga+Q and Amiga+X. */

			if(Work->EditOp == EO_RESET || Work->EditOp == EO_CLEAR)
				break;

				/* Ditch the qualifier keys */

			if(Work->IEvent->ie_Code >= 96 && Work->IEvent->ie_Code <= 103)
			{
				Work->Actions &= ~(SGA_USE | SGA_BEEP);
				break;
			}

				/* Strip all the qualifiers we don't want */

			Qualifier = Work->IEvent->ie_Qualifier & ~(IEQUALIFIER_REPEAT | IEQUALIFIER_INTERRUPT | IEQUALIFIER_MULTIBROADCAST | IEQUALIFIER_RELATIVEMOUSE);

				/* Clear the translation buffer. */

			Key[0] = 0;

				/* Check for raw keys */

			GotIt = FALSE;

			for(i = 0 ; RawTable[i].Code ; i++)
			{
				if(Work->IEvent->ie_Code == RawTable[i].Code)
				{
					KeyName = RawTable[i].Name;
					GotIt = TRUE;

					break;
				}
			}

				/* Translate the code. */

			if(!GotIt)
			{
				struct InputEvent Event;

					/* Make a copy of the input event and */
					/* clear the qualifier bits */

				CopyMem(Work->IEvent,&Event,sizeof(struct InputEvent));

				Event.ie_Qualifier = NULL;

					/* Translate the event */

				if((KeyLen = MapRawKey(&Event,Key,10,NULL)) < 0)
					KeyLen = 10;

					/* Did we get anything? */

				if(KeyLen)
				{
						/* Carriage return was pressed */

					if(Key[0] == '\r')
					{
							/* This probably ends input */

						if(!Qualifier)
						{
							Work->Actions = (Work->Actions & ~SGA_BEEP) | SGA_USE | SGA_END;

							break;
						}
						else
						{
								/* Is this the enter key? */

							if(Qualifier & IEQUALIFIER_NUMERICPAD)
							{
								KeyName = "Enter";

								Qualifier &= ~IEQUALIFIER_NUMERICPAD;

								GotIt = TRUE;
							}
						}
					}

						/* If this is just the tab key, pass it through cleanly */

					if(Key[0] == '\t' && !(Qualifier & ~SHIFT_KEY))
						break;

						/* Now check for special characters; can't use ToUpper() */
						/* here since commodities.library will get it dead wrong. */

					if(!GotIt)
					{
						for(i = 0 ; KeyTable[i].Code ; i++)
						{
							if(KeyTable[i].Code == ToUpper(Key[0]))
							{
								KeyName = KeyTable[i].Name;

								GotIt = TRUE;

								break;
							}
						}
					}

						/* If no special character is involved, */
						/* use the vanilla character code */

					if(!GotIt)
					{
						if((Key[0] > ' ' && Key[0] < 127) || Key[0] >= 160)
						{
							Key[1] = 0;

							KeyName = Key;

							GotIt = TRUE;
						}
					}
				}

					/* Did we get anything useful? */

				if(!GotIt)
				{
					Work->Actions = (Work->Actions & ~SGA_USE) | SGA_BEEP;

					break;
				}
			}

				/* Take care of the qualifiers. Note that we do not distinguish */
				/* between the left and right shift/alt keys */

			if(Qualifier)
			{
				LONG MaxChars;

					/* How long the string can get. */

				MaxChars = Work->StringInfo->MaxChars;

					/* Ok, now start building the string */

				Work->WorkBuffer[0] = 0;

				for(i = 0 ; QualifierTable[i].Code ; i++)
				{
					if(Qualifier & QualifierTable[i].Code)
						LimitedStrcat(MaxChars,Work->WorkBuffer,QualifierTable[i].Name);
				}

					/* Add the key itself */

				LimitedStrcat(MaxChars,Work->WorkBuffer,KeyName);

					/* Update the work data */

				Work->NumChars	= strlen(Work->WorkBuffer);
				Work->BufferPos	= Work->NumChars;

					/* Finished... */

				Work->Actions = (Work->Actions & ~(SGA_BEEP | SGA_PREVACTIVE | SGA_NEXTACTIVE | SGA_END)) | SGA_REDISPLAY | SGA_USE;
			}
			else
				Work->Actions &= ~(SGA_USE | SGA_BEEP);

			break;

		case SGH_CLICK:

			break;

		default:

			return(FALSE);
	}

	return(TRUE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

BOOL
HotkeyPanelConfig(struct Configuration *LocalConfig,struct Hotkeys *Hotkeys,STRPTR LastKeys,BOOL *ChangePtr)
{
	STATIC struct Hook EditHook;

	struct Hotkeys *PrivateHotkeys;
	struct LayoutHandle	*Handle;
	BOOL Changed = FALSE;

	if(!(PrivateHotkeys = (struct Hotkeys *)AllocVecPooled(sizeof(struct Hotkeys),MEMF_ANY)))
	{
		DisplayBeep(NULL);
		return(FALSE);
	}

	CopyMem(Hotkeys,PrivateHotkeys,sizeof(struct Hotkeys));

	InitHook(&EditHook,(HOOKFUNC)EditRoutine,NULL);

	KeymapBase = OpenLibrary("keymap.library",0);

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
				LA_LabelID,	MSG_V36_0083,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_HOTKEYPANEL_TERM_SCREEN_TO_FRONT_GAD,
					LA_STRPTR,		PrivateHotkeys->termScreenToFront,
					LA_Chars,		30,
					GTST_MaxChars,	sizeof(PrivateHotkeys->termScreenToFront) - 1,
					LA_ID,			GAD_TERMTOFRONT,

					KeymapBase ? GTST_EditHook : TAG_IGNORE,&EditHook,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_HOTKEYPANEL_BUFFER_SCREEN_TO_FRONT_GAD,
					LA_STRPTR,		PrivateHotkeys->BufferScreenToFront,
					GTST_MaxChars,	sizeof(PrivateHotkeys->BufferScreenToFront) - 1,
					LA_ID,			GAD_BUFFERTOFRONT,

					KeymapBase ? GTST_EditHook : TAG_IGNORE,&EditHook,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_HOTKEYPANEL_SKIP_DIAL_ENTRY_GAD,
					LA_STRPTR,		PrivateHotkeys->SkipDialEntry,
					GTST_MaxChars,	sizeof(PrivateHotkeys->SkipDialEntry) - 1,
					LA_ID,			GAD_SKIPDIAL,

					KeymapBase ? GTST_EditHook : TAG_IGNORE,&EditHook,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_LabelID,		MSG_HOTKEYPANEL_ABORT_AREXX_GAD,
					LA_STRPTR,		PrivateHotkeys->AbortARexx,
					GTST_MaxChars,	sizeof(PrivateHotkeys->AbortARexx) - 1,
					LA_ID,			GAD_ABORTAREXX,

					KeymapBase ? GTST_EditHook : TAG_IGNORE,&EditHook,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelID,		MSG_V36_0084,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,			SliderType,
					LA_LabelID,			MSG_HOTKEYPANEL_COMMODITY_PRIORITY_GAD,
					LA_BYTE,			&PrivateHotkeys->CommodityPriority,
					GTSL_Min,			-128,
					GTSL_Max,			127,
					GTSL_LevelFormat,	"%4ld",
					LA_Chars,			16,
					LA_ID,				GAD_PRIORITY,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,	CHECKBOX_KIND,
					LA_LabelID,	MSG_HOTKEYPANEL_HOTKEYS_ENABLED_GAD,
					LA_BYTE,	&PrivateHotkeys->HotkeysEnabled,
					LA_ID,		GAD_HOTKEYS,
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
			LAWN_TitleID,		MSG_HOTKEYPANEL_HOTKEY_PREFERENCES_TXT,
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
			BOOL Done = FALSE;
			ULONG MsgClass;
			struct Gadget *MsgGadget;
			struct FileRequester *FileRequest;
			UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
			STRPTR String;

			GuideContext(CONTEXT_HOTKEYS);

			PushWindow(PanelWindow);

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
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP)
					{
						switch(MsgGadget->GadgetID)
						{
							case GAD_TERMTOFRONT:
							case GAD_BUFFERTOFRONT:
							case GAD_SKIPDIAL:
							case GAD_ABORTAREXX:

								String = LT_GetString(Handle,MsgGadget->GadgetID);

								if(!GoodCode(String))
								{
									DisplayBeep(PanelWindow->WScreen);

									LT_Activate(Handle,MsgGadget->GadgetID);
								}

								break;

							case GAD_USE:

								LT_UpdateStrings(Handle);

								CopyMem(PrivateHotkeys,Hotkeys,sizeof(struct Hotkeys));

								Changed = TRUE;

								if(ChangePtr)
									*ChangePtr = TRUE;

								Done = TRUE;
								break;

							case GAD_DISCARD:

								if(ChangePtr)
									*ChangePtr = FALSE;

								LastKeys[0] = 0;
								Changed = Done = TRUE;

								if(LocalConfig)
									strcpy(LocalConfig->HotkeyFileName,LastKeys);

								break;

							case GAD_CANCEL:

								Changed = FALSE;
								Done = TRUE;
								break;

							case GAD_LOAD:

								LT_LockWindow(PanelWindow);

								strcpy(DummyBuffer,LastKeys);

								if(FileRequest = OpenSingleFile(PanelWindow,LocaleString(MSG_HOTKEYPANEL_LOAD_HOTKEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!LoadHotkeys(DummyBuffer,PrivateHotkeys))
										ShowError(PanelWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastKeys,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->HotkeyFileName,LastKeys);

										LT_SetAttributes(Handle,GAD_TERMTOFRONT,
											GTST_String,PrivateHotkeys->termScreenToFront,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_BUFFERTOFRONT,
											GTST_String,PrivateHotkeys->BufferScreenToFront,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_SKIPDIAL,
											GTST_String,PrivateHotkeys->SkipDialEntry,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_ABORTAREXX,
											GTST_String,PrivateHotkeys->AbortARexx,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_PRIORITY,
											GTSL_Level,PrivateHotkeys->CommodityPriority,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_HOTKEYS,
											GTCB_Checked,PrivateHotkeys->HotkeysEnabled,
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

								LT_UpdateStrings(Handle);

								strcpy(DummyBuffer,LastKeys);

								if(FileRequest = SaveFile(PanelWindow,LocaleString(MSG_HOTKEYPANEL_SAVE_HOTKEYS_TXT),NULL,"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(!WriteIFFData(DummyBuffer,PrivateHotkeys,sizeof(struct Hotkeys),ID_HOTK))
										ShowError(PanelWindow,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									else
									{
										strcpy(LastKeys,DummyBuffer);

										if(LocalConfig)
											strcpy(LocalConfig->HotkeyFileName,LastKeys);

										if(ChangePtr)
											*ChangePtr = FALSE;
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

	FreeVecPooled(PrivateHotkeys);

	CloseLibrary(KeymapBase);

	return(Changed);
}
