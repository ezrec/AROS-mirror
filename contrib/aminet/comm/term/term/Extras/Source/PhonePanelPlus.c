/*
**	PhonePanelPlus.c
**
**	The phonebook and support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The gadget IDs are in there */

#ifndef _PHONEPANEL_H
#include "PhonePanel.h"
#endif	/* _PHONEPANEL_H */

BOOL
SaveChanges(struct Window *Parent)
{
	return((BOOL)ShowRequest(Parent,LocaleString(MSG_SAVE_CHANGES),LocaleString(MSG_GLOBAL_YES_NO_TXT)));
}

BOOL
EditConfig(struct Configuration *Config,LONG Type,ULONG Qualifier,struct Window *Window)
{
	STATIC LONG TypeMappings[15][2] =
	{
		{ GAD_SERIAL_EDIT,		PREF_SERIAL, },
		{ GAD_MODEM_EDIT,			PREF_MODEM, },
		{ GAD_SCREEN_EDIT,		PREF_SCREEN, },
		{ GAD_TERMINAL_EDIT,		PREF_TERMINAL, },
		{ GAD_EMULATION_EDIT,		PREF_EMULATION, },
		{ GAD_CLIPBOARD_EDIT,		PREF_CLIP, },
		{ GAD_CAPTURE_EDIT,		PREF_CAPTURE, },
		{ GAD_COMMAND_EDIT,		PREF_COMMAND, },
		{ GAD_MISC_EDIT,			PREF_MISC, },
		{ GAD_PATH_EDIT,			PREF_PATH, },
		{ GAD_TRANSFER_EDIT,		PREF_TRANSFER, },
		{ GAD_TRANSLATION_EDIT,	PREF_TRANSLATIONFILENAME, },
		{ GAD_MACRO_EDIT,			PREF_MACROFILENAME, },
		{ GAD_CURSOR_EDIT,		PREF_CURSORFILENAME, },
		{ GAD_FASTMACRO_EDIT,		PREF_FASTMACROFILENAME },
	};

	struct FileRequester	*FileRequest;
	UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
	BOOL					 Changed	= FALSE,
							 FallBack	= TRUE;
	LONG					 PrefType = 0;
	LONG					 i;
	APTR					 Data;

		/* Find the prefs fragment that should be worked upon */

	for(i = 0 ; i < sizeof(TypeMappings) / (sizeof(LONG) * 2) ; i++)
	{
		if(TypeMappings[i][0] == Type)
		{
			PrefType = TypeMappings[i][1];

			break;
		}
	}

	Data = GetConfigEntry(Config,PrefType);

	if(CreateConfigEntry(Config,PrefType))
	{
		switch(Type)
		{
			case GAD_SERIAL_EDIT:

				Changed = SerialPanel(Window,Config);
				break;

			case GAD_MODEM_EDIT:

				Changed = ModemPanel(Window,Config);
				break;

			case GAD_SCREEN_EDIT:

				Changed = ScreenPanel(Window,Config);
				break;

			case GAD_TERMINAL_EDIT:

				Changed = TerminalPanel(Window,Config);
				break;

			case GAD_EMULATION_EDIT:

				Changed = EmulationPanel(Window,Config);
				break;

			case GAD_CLIPBOARD_EDIT:

				Changed = ClipPanel(Window,Config);
				break;

			case GAD_CAPTURE_EDIT:

				Changed = CapturePanel(Window,Config);
				break;

			case GAD_COMMAND_EDIT:

				Changed = CommandPanel(Window,Config);
				break;

			case GAD_MISC_EDIT:

				Changed = MiscPanel(Window,Config);
				break;

			case GAD_PATH_EDIT:

				Changed = PathPanel(Window,Config);
				break;

			case GAD_TRANSFER_EDIT:

				Changed = LibPanel(Window,Config);
				break;

			case GAD_TRANSLATION_EDIT:

				if(!(Qualifier & SHIFT_KEY))
				{
					struct TranslationEntry **Send,**Receive = NULL;
					BOOL			Success = FALSE;

					strcpy(DummyBuffer,Config->TranslationFileName);

					if(Send = AllocTranslationTable())
					{
						if(Receive = AllocTranslationTable())
						{
							Success = TRUE;

							if(DummyBuffer[0])
							{
								if(!LoadTranslationTables(DummyBuffer,Send,Receive))
								{
									LONG Error = IoErr();

									if(Error != ERROR_OBJECT_NOT_FOUND)
										ShowError(Window,ERR_LOAD_ERROR,Error,DummyBuffer);

									FillTranslationTable(Send);
									FillTranslationTable(Receive);
								}
							}
							else
							{
								FillTranslationTable(Send);
								FillTranslationTable(Receive);
							}
						}
					}

					if(Success)
					{
						if(TranslationPanelConfig(NULL,&Send,&Receive,DummyBuffer,Window,NULL))
						{
							if(Send || Receive)
							{
								if(IsStandardTable(Send) && IsStandardTable(Receive))
									DummyBuffer[0] = 0;

								if(DummyBuffer[0])
								{
									if(SaveChanges(Window))
									{
										if(!SaveTranslationTables(DummyBuffer,Send,Receive))
											ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
									}
								}
							}
							else
								DummyBuffer[0] = 0;
						}

						if(strcmp(Config->TranslationFileName,DummyBuffer))
						{
							strcpy(Config->TranslationFileName,DummyBuffer);

							Changed = TRUE;
						}

						FallBack = FALSE;
					}

					if(Send)
						FreeTranslationTable(Send);

					if(Receive)
						FreeTranslationTable(Receive);
				}

				if(FallBack)
				{
					strcpy(DummyBuffer,Config->TranslationFileName);

					if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_PHONEPANEL_SELECT_TRANSLATION_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
					{
						FreeAslRequest(FileRequest);

						strcpy(Config->TranslationFileName,DummyBuffer);

						Changed = TRUE;
					}
				}

				break;

			case GAD_MACRO_EDIT:

				if(!(Qualifier & SHIFT_KEY))
				{
					struct MacroKeys *Keys;

					if(Keys = (struct MacroKeys *)AllocVecPooled(sizeof(struct MacroKeys),MEMF_ANY | MEMF_CLEAR))
					{
						strcpy(DummyBuffer,Config->MacroFileName);

						if(DummyBuffer[0])
						{
							if(!LoadMacros(DummyBuffer,Keys))
							{
								LONG Error = IoErr();

								if(Error != ERROR_OBJECT_NOT_FOUND)
									ShowError(Window,ERR_LOAD_ERROR,Error,DummyBuffer);
							}
						}

						if(MacroPanelConfig(NULL,Keys,DummyBuffer,Window,NULL))
						{
							if(DummyBuffer[0] && SaveChanges(Window))
							{
								if(!WriteIFFData(DummyBuffer,Keys,sizeof(struct MacroKeys),ID_KEYS))
									ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
							}
						}

						if(strcmp(Config->MacroFileName,DummyBuffer))
						{
							strcpy(Config->MacroFileName,DummyBuffer);

							Changed = TRUE;
						}

						FallBack = FALSE;

						FreeVecPooled(Keys);
					}
				}

				if(FallBack)
				{
					strcpy(DummyBuffer,Config->MacroFileName);

					if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_PHONEPANEL_SELECT_KEYBOARD_MACROS_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
					{
						FreeAslRequest(FileRequest);

						strcpy(Config->MacroFileName,DummyBuffer);

						Changed = TRUE;
					}
				}

				break;

			case GAD_CURSOR_EDIT:

				if(!(Qualifier & SHIFT_KEY))
				{
					struct CursorKeys *Keys;

					if(Keys = (struct CursorKeys *)AllocVecPooled(sizeof(struct CursorKeys),MEMF_ANY | MEMF_CLEAR))
					{
						strcpy(DummyBuffer,Config->CursorFileName);

						if(DummyBuffer[0])
						{
							if(!ReadIFFData(DummyBuffer,Keys,sizeof(struct CursorKeys),ID_KEYS))
							{
								LONG Error = IoErr();

								ResetCursorKeys(Keys);

								if(Error != ERROR_OBJECT_NOT_FOUND)
									ShowError(Window,ERR_LOAD_ERROR,Error,DummyBuffer);
							}
						}
						else
							ResetCursorKeys(Keys);

						if(CursorPanelConfig(NULL,Keys,DummyBuffer,Window,NULL))
						{
							if(DummyBuffer[0] && SaveChanges(Window))
							{
								if(!WriteIFFData(DummyBuffer,Keys,sizeof(struct CursorKeys),ID_KEYS))
									ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
							}
						}

						if(strcmp(Config->CursorFileName,DummyBuffer))
						{
							strcpy(Config->CursorFileName,DummyBuffer);

							Changed = TRUE;
						}

						FallBack = FALSE;

						FreeVecPooled(Keys);
					}
				}

				if(FallBack)
				{
					strcpy(DummyBuffer,Config->CursorFileName);

					if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_PHONEPANEL_SELECT_CURSOR_KEYS_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
					{
						FreeAslRequest(FileRequest);

						strcpy(Config->CursorFileName,DummyBuffer);

						Changed = TRUE;
					}
				}

				break;

			case GAD_FASTMACRO_EDIT:

				if(!(Qualifier & SHIFT_KEY))
				{
					struct List *List;

					if(List = CreateList())
					{
						strcpy(DummyBuffer,Config->FastMacroFileName);

						if(DummyBuffer[0])
						{
							if(!LoadFastMacros(DummyBuffer,List))
							{
								LONG Error = IoErr();

								if(Error != ERROR_OBJECT_NOT_FOUND)
									ShowError(Window,ERR_LOAD_ERROR,Error,DummyBuffer);
							}
						}

						if(FastMacroPanelConfig(NULL,List,DummyBuffer,Window,NULL))
						{
							if(DummyBuffer[0] && SaveChanges(Window))
							{
								if(!SaveFastMacros(DummyBuffer,List))
									ShowError(Window,ERR_SAVE_ERROR,IoErr(),DummyBuffer);
							}
						}

						if(strcmp(Config->FastMacroFileName,DummyBuffer))
						{
							strcpy(Config->FastMacroFileName,DummyBuffer);

							Changed = TRUE;
						}

						FallBack = FALSE;

						DeleteList(List);
					}
				}

				if(FallBack)
				{
					strcpy(DummyBuffer,Config->FastMacroFileName);

					if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_PHONEPANEL_SELECT_FAST_MACROS_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),"#?.prefs",DummyBuffer,sizeof(DummyBuffer)))
					{
						FreeAslRequest(FileRequest);

						strcpy(Config->FastMacroFileName,DummyBuffer);

						Changed = TRUE;
					}
				}

				break;
		}

		if(!Changed && !Data)
			DeleteConfigEntry(Config,PrefType);
	}
	else
		DisplayBeep(Window->WScreen);

	return(Changed);
}

BOOL
ChangeState(LONG Type,LONG Default,struct PhoneNode *Node)
{
	STATIC LONG TypeMappings[][2] =
	{
		{ GAD_SERIAL_STATE,		PREF_SERIAL, },
		{ GAD_MODEM_STATE,		PREF_MODEM, },
		{ GAD_SCREEN_STATE,		PREF_SCREEN, },
		{ GAD_TERMINAL_STATE,		PREF_TERMINAL, },
		{ GAD_EMULATION_STATE,	PREF_EMULATION, },
		{ GAD_CLIPBOARD_STATE,	PREF_CLIP, },
		{ GAD_CAPTURE_STATE,		PREF_CAPTURE, },
		{ GAD_COMMAND_STATE,		PREF_COMMAND, },
		{ GAD_MISC_STATE,			PREF_MISC, },
		{ GAD_PATH_STATE,			PREF_PATH, },
		{ GAD_TRANSFER_STATE,		PREF_TRANSFER, },
		{ GAD_TRANSLATION_STATE,	PREF_TRANSLATIONFILENAME, },
		{ GAD_MACRO_STATE,		PREF_MACROFILENAME, },
		{ GAD_CURSOR_STATE,		PREF_CURSORFILENAME, },
		{ GAD_FASTMACRO_STATE,	PREF_FASTMACROFILENAME, },
		{ GAD_RATE_STATE,			PREF_RATES },
	};

	struct Configuration	*LocalConfig;
	LONG					 PrefType = 0;
	LONG					 i;
	APTR					 Data;

		/* Find the prefs fragment that should be worked upon */

	for(i = 0 ; i < sizeof(TypeMappings) / (sizeof(LONG) * 2) ; i++)
	{
		if(TypeMappings[i][0] == Type)
		{
			PrefType = TypeMappings[i][1];

			break;
		}
	}

		/* This is the one we're about to put through the wringer */

	LocalConfig = Node->Entry->Config;

		/* A special case, the phone rates */

	if(PrefType == PREF_RATES)
	{
		if(Default)
		{
				/* There will be some rates... */

			Node->Entry->Header->NoRates = FALSE;

				/* Nothing in this list? */

			if(IsListEmpty(&Node->Entry->TimeDateList))
			{
				struct TimeDateNode *TimeDateNode;

					/* Provide defaults */

				if(TimeDateNode = CreateTimeDateNode(-1,-1,"",2))
					AddTail((struct List *)&Node->Entry->TimeDateList,&TimeDateNode->Node);
				else
				{
					Node->Entry->Header->NoRates = TRUE;	/* Sorry, guv'nor */

					return(FALSE);
				}
			}
		}
		else
		{
				/* Discard the time/date list */

			FreeTimeDateList((struct List *)&Node->Entry->TimeDateList);

				/* No rates here */

			Node->Entry->Header->NoRates = TRUE;
		}

		return(TRUE);
	}

		/* Get a pointer to it */

	Data = GetConfigEntry(LocalConfig,PrefType);

		/* Default == TRUE means: replace fragment with a copy of the */
		/*                        current global configuration which the */
		/*                        user can edit later. Making a connection */
		/*                        through the phonebook will cause the */
		/*                        global configuration to be replaced */
		/*                        with this copy. */

	if(Default)
	{
		BOOL Result;

			/* Get rid of the old stuff */

		if(Data)
		{
			DeleteConfigEntry(LocalConfig,PrefType);

			Result = TRUE;
		}
		else
			Result = FALSE;

			/* Create a new fragment with default values */

		if(!CreateConfigEntry(LocalConfig,PrefType))
			return(Result);
	}
	else
	{
			/* Default == FALSE means: discard this fragment; making a */
			/*                         connection through the phonebook */
			/*                         will then leave these main config */
			/*                         fragments unmodified. */

		if(Data)
			DeleteConfigEntry(LocalConfig,PrefType);
		else
			return(FALSE);
	}

	return(TRUE);
}

CONST_STRPTR *
BuildLabels(PhonebookHandle *PhoneHandle)
{
	CONST_STRPTR *Labels;

	if(Labels = (CONST_STRPTR *)AllocVecPooled(sizeof(CONST_STRPTR) * (GetListSize((struct List *)&PhoneHandle->PhoneGroupList) + 2),MEMF_ANY))
	{
		struct Node *Node;
		CONST_STRPTR *Index;

		Index = Labels;

		*Index++ = LocaleString(MSG_PHONEBOOK_ALL_GROUP_TXT);

		for(Node = (struct Node *)PhoneHandle->PhoneGroupList.mlh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
			*Index++ = Node->ln_Name;

		*Index = NULL;
	}

	return(Labels);
}

VOID
FindGroup(struct List *List,LONG *GroupIndex,LONG *NodeIndex,PhoneNode *Wanted)
{
	PhoneGroupNode	*GroupNode;
	PhoneNode		*Node;
	LONG			 Index;
	ULONG			 Group;

	for(Group = 0, GroupNode = (PhoneGroupNode *)List->lh_Head ; GroupNode->Node.ln_Succ ; GroupNode = (PhoneGroupNode *)GroupNode->Node.ln_Succ, Group++)
	{
		for(Index = 0, Node = (struct PhoneNode *)GroupNode->GroupList.mlh_Head ; Node->Node.ln_Succ ; Node = (struct PhoneNode *)Node->Node.ln_Succ, Index++)
		{
			if(Node == Wanted)
			{
				*GroupIndex	= Group + 1;
				*NodeIndex	= Index;

				break;
			}
		}
	}
}
