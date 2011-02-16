/*
**	AmigaGuide.c
**
**	AmigaGuide support routines
**
**	Copyright © 1990-1993 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Local data. */

STATIC STRPTR				 ContextList[55];
STATIC AMIGAGUIDECONTEXT	 Context;
STATIC struct NewAmigaGuide	 NewGuide;
STATIC struct Process		*GuideProcess;
STATIC LONG					 GuideContextID = CONTEXT_MAIN;

	/* GuideServer(VOID):
	 *
	 *	AmigaGuide server, handles all the signal processing.
	 */

STATIC VOID SAVE_DS
GuideServer(VOID)
{
		/* Open the help text file asynchronously... */

	if(Context = OpenAmigaGuideAsync(&NewGuide,TAG_DONE))
	{
		ULONG GuideMask,Signals;
		BOOL Done = FALSE;

			/* Pick up the signal notification mask. */

		GuideMask = AmigaGuideSignal(Context);

			/* Clear the context stack. */

		SetAmigaGuideContext(Context,0,TAG_DONE);

			/* Flag the main process to continue. */

		Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

			/* Go into input loop. */

		do
		{
				/* Wait for a signal... */

			Signals = Wait(SIG_KILL | GuideMask);

				/* Are we to quit? */

			if(Signals & SIG_KILL)
				Done = TRUE;

				/* Process icoming AmigaGuide messages. */

			if(Signals & GuideMask)
			{
				struct AmigaGuideMsg *GuideMessage;

				while(GuideMessage = GetAmigaGuideMsg(Context))
					ReplyAmigaGuideMsg(GuideMessage);
			}
		}
		while(!Done);

			/* Close the help text file. */

		CloseAmigaGuide(Context);
	}

		/* Lock & quit. */

	Forbid();

	GuideProcess = NULL;

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

	/* GuideLaunch(LONG ContextID):
	 *
	 *	Launch the AmigaGuide help file server.
	 */

STATIC BOOL
GuideLaunch(LONG ContextID)
{
		/* Is the main program running and is this the
		 * main program to make the call?
		 */

	if(Window && FindTask(NULL) == (struct Task *)ThisProcess)
	{
			/* Is the help file server already running? */

		if(!GuideProcess)
		{
			if(!AmigaGuideBase)
				AmigaGuideBase = OpenLibrary("amigaguide.library",0);

			if(AmigaGuideBase)
			{
				BOOL IsValidFile = TRUE;

					/* Do we have a valid AmigaGuide file name? */

				if(Config->PathConfig->HelpFile[0])
				{
					if(!GetFileSize(Config->PathConfig->HelpFile))
						IsValidFile = FALSE;
				}
				else
					IsValidFile = FALSE;

					/* Do we have a valid AmigaGuide file name? */

				if(!IsValidFile)
				{
						/* Don't pop up the file requester if any
						 * time-critical services are currently running!
						 */

					if(ContextID == CONTEXT_TRANSFER || ContextID == CONTEXT_DIAL)
					{
						DisplayBeep(Window->WScreen);

						return(FALSE);
					}
					else
					{
						UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
						struct FileRequester *FileRequest;

							/* Provide a default name if necessary. */

						if(!Config->PathConfig->HelpFile[0])
							strcpy(Config->PathConfig->HelpFile,"PROGDIR:term.guide");

							/* Block the windows. */

						BlockWindows();

							/* Get file and path name. */

						LimitedStrcpy(sizeof(DummyBuffer),DummyBuffer,Config->PathConfig->HelpFile);

							/* Get the help text file name. */

						if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_PATHPANEL_SELECT_HELP_FILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
						{
							FreeAslRequest(FileRequest);

							if(GetFileSize(DummyBuffer))
							{
								LimitedStrcpy(sizeof(Config->PathConfig->HelpFile),Config->PathConfig->HelpFile,DummyBuffer);
								IsValidFile = TRUE;
							}
						}

							/* Release the windows... */

						ReleaseWindows();
					}
				}

					/* Do we finally have a valid file name? */

				if(IsValidFile)
				{
					STATIC LONG NodeTable[] =
					{
						MSG_TERMAMIGAGUIDE_NODE_00_TXT,
						MSG_TERMAMIGAGUIDE_NODE_01_TXT,
						MSG_TERMAMIGAGUIDE_NODE_02_TXT,
						MSG_TERMAMIGAGUIDE_NODE_03_TXT,
						MSG_TERMAMIGAGUIDE_NODE_04_TXT,
						MSG_TERMAMIGAGUIDE_NODE_05_TXT,
						MSG_TERMAMIGAGUIDE_NODE_06_TXT,
						MSG_TERMAMIGAGUIDE_NODE_07_TXT,
						MSG_TERMAMIGAGUIDE_NODE_08_TXT,
						MSG_TERMAMIGAGUIDE_NODE_09_TXT,
						MSG_TERMAMIGAGUIDE_NODE_10_TXT,
						MSG_TERMAMIGAGUIDE_NODE_11_TXT,
						MSG_TERMAMIGAGUIDE_NODE_12_TXT,
						MSG_TERMAMIGAGUIDE_NODE_13_TXT,
						MSG_TERMAMIGAGUIDE_NODE_14_TXT,
						MSG_TERMAMIGAGUIDE_NODE_15_TXT,
						MSG_TERMAMIGAGUIDE_NODE_16_TXT,
						MSG_TERMAMIGAGUIDE_NODE_17_TXT,
						MSG_TERMAMIGAGUIDE_NODE_18_TXT,
						MSG_TERMAMIGAGUIDE_NODE_19_TXT,
						MSG_TERMAMIGAGUIDE_NODE_20_TXT,
						MSG_TERMAMIGAGUIDE_NODE_21_TXT,
						MSG_TERMAMIGAGUIDE_NODE_22_TXT,
						MSG_TERMAMIGAGUIDE_NODE_23_TXT,
						MSG_TERMAMIGAGUIDE_NODE_24_TXT,
						MSG_TERMAMIGAGUIDE_NODE_25_TXT,
						MSG_TERMAMIGAGUIDE_NODE_26_TXT,
						MSG_TERMAMIGAGUIDE_NODE_27_TXT,
						MSG_TERMAMIGAGUIDE_NODE_28_TXT,
						MSG_TERMAMIGAGUIDE_NODE_29_TXT,
						MSG_TERMAMIGAGUIDE_NODE_30_TXT,
						MSG_TERMAMIGAGUIDE_NODE_31_TXT,
						MSG_TERMAMIGAGUIDE_NODE_32_TXT,
						MSG_TERMAMIGAGUIDE_NODE_33_TXT,
						MSG_TERMAMIGAGUIDE_NODE_34_TXT,
						MSG_TERMAMIGAGUIDE_NODE_35_TXT,
						MSG_TERMAMIGAGUIDE_NODE_36_TXT,
						MSG_TERMAMIGAGUIDE_NODE_37_TXT,
						MSG_TERMAMIGAGUIDE_NODE_38_TXT,
						MSG_TERMAMIGAGUIDE_NODE_39_TXT,
						MSG_TERMAMIGAGUIDE_NODE_40_TXT,
						MSG_TERMAMIGAGUIDE_NODE_41_TXT,
						MSG_TERMAMIGAGUIDE_NODE_42_TXT,
						MSG_TERMAMIGAGUIDE_NODE_43_TXT,
						MSG_TERMAMIGAGUIDE_NODE_44_TXT,
						MSG_TERMAMIGAGUIDE_NODE_45_TXT,
						MSG_TERMAMIGAGUIDE_NODE_46_TXT,
						MSG_TERMAMIGAGUIDE_NODE_47_TXT,
						MSG_TERMAMIGAGUIDE_NODE_48_TXT,
						MSG_TERMAMIGAGUIDE_NODE_49_TXT,
						MSG_TERMAMIGAGUIDE_NODE_50_TXT,
						MSG_TERMAMIGAGUIDE_NODE_51_TXT,
						MSG_TERMAMIGAGUIDE_NODE_52_TXT,
						MSG_TERMAMIGAGUIDE_NODE_53_TXT,

						-1
					};

						/* Provide the context node names (note: language
						 * specific!).
						 */

					LocalizeStringTable(ContextList,NodeTable);

						/* Clear the instance. */

					memset(&NewGuide,0,sizeof(struct NewAmigaGuide));

						/* Fill in the structure. */

					NewGuide.nag_BaseName	= "termHelp";
					NewGuide.nag_Name		= Config->PathConfig->HelpFile;
					NewGuide.nag_ClientPort	= "TERM_HELP";
					NewGuide.nag_Context	= ContextList;
					NewGuide.nag_Screen		= Window->WScreen;

						/* Launch the server process. */

					GuideProcess = StartProcessWaitForHandshake("term AmigaGuide Process",(TASKENTRY)GuideServer,TAG_DONE);
				}
			}
		}

			/* Pop the main screen to the front if necessary. */

		if(GuideProcess)
		{
			ScreenToFront(Window->WScreen);

			return(TRUE);
		}
		else
		{
			if(AmigaGuideBase)
			{
				CloseLibrary(AmigaGuideBase);

				AmigaGuideBase = NULL;
			}

			DisplayBeep(Window->WScreen);

			return(FALSE);
		}
	}
	else
	{
		if(GuideProcess && Window)
		{
			ScreenToFront(Window->WScreen);

			return(TRUE);
		}
		else
			return(FALSE);
	}
}

	/* GuideCleanup():
	 *
	 *	Terminate the AmigaGuide server and free the
	 *	associated resources.
	 */

VOID
GuideCleanup()
{
	ShakeHands((struct Task *)GuideProcess,SIG_KILL);

	if(AmigaGuideBase)
	{
		CloseLibrary(AmigaGuideBase);

		AmigaGuideBase = NULL;
	}
}

	/* GuideContext(LONG NewContextID):
	 *
	 *	Set the global AmigaGuide context.
	 */

VOID
GuideContext(LONG NewContextID)
{
	GuideContextID = NewContextID;
}

#ifdef __AROS__
AROS_UFH3(ULONG, GuideSetupHook,
	AROS_UFHA(struct Hook *, UnusedHook, A0),
	AROS_UFHA(HelpMsg *, UnusedHelpMessage, A2),
	AROS_UFHA(struct IBox *, UnusedBounds, A1))
#else
ULONG SAVE_DS ASM
GuideSetupHook(REG(a0) struct Hook *UnusedHook,REG(a2) HelpMsg *UnusedHelpMessage,REG(a1) struct IBox *UnusedBounds)
#endif
{
#ifdef __AROS__
	AROS_USERFUNC_INIT
#endif
	if(GuideLaunch(GuideContextID))
	{
		SetAmigaGuideContext(Context,GuideContextID,TAG_DONE);
		SendAmigaGuideContext(Context,TAG_DONE);

		return(TRUE);
	}
	else
		return(FALSE);
#ifdef __AROS__
	AROS_USERFUNC_EXIT
#endif
}

	/* GuideSetup():
	 *
	 *	Try to display the currently selected AmigaGuide
	 *	help text.
	 */

VOID
GuideSetup()
{
	if(GuideLaunch(GuideContextID))
	{
		SetAmigaGuideContext(Context,GuideContextID,TAG_DONE);
		SendAmigaGuideContext(Context,TAG_DONE);
	}
}

	/* GuideDisplay(LONG ContextID):
	 *
	 *	Try to display an AmigaGuide help text.
	 */

VOID
GuideDisplay(LONG ContextID)
{
	if(GuideLaunch(ContextID))
	{
		SetAmigaGuideContext(Context,ContextID,TAG_DONE);
		SendAmigaGuideContext(Context,TAG_DONE);
	}
}
