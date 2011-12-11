/*
**	Launch.c
**
**	Routines to launch programs, ARexx scripts, etc.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Local data. */

STATIC struct SignalSemaphore	LaunchSemaphore;
STATIC LONG						LaunchCounter;

STATIC BOOL
OpenStreams(STRPTR StreamName,BPTR *InputStream,BPTR *OutputStream)
{
	BPTR Stream;

		/* Open the output stream. */

	if(Stream = Open(StreamName,MODE_NEWFILE))
	{
			/* Check if the stream is not "NIL:" and if it is interactive. */

		if(GoodStream(Stream))
		{
			*InputStream	= Stream;
			*OutputStream	= BNULL;
		}
		else
		{
			*InputStream	= Open("NIL:",MODE_NEWFILE);
			*OutputStream	= Stream;
		}

		if(*InputStream)
			return(TRUE);
		else
		{
			LONG Error = IoErr();

			Close(Stream);

			SetIoErr(Error);
		}
	}

	return(FALSE);
}

	/* LaunchEntry(VOID):
	 *
	 *	Entry point of the routine that handles the workload.
	 */

STATIC VOID SAVE_DS
LaunchEntry(VOID)
{
	LaunchMsg		*Startup;
	struct MsgPort	*CurrentPort;
	BPTR			 InputStream,OutputStream;
	struct MsgPort	*GlobalRexxPort;

		/* Grab the startup packet. */

	CurrentPort = &((struct Process *)FindTask(NULL))->pr_MsgPort;
	WaitPort(CurrentPort);
	Startup = (LaunchMsg *)GetMsg(CurrentPort);

		/* Increment the launch counter. */

	ObtainSemaphore(&LaunchSemaphore);
	LaunchCounter++;
	ReleaseSemaphore(&LaunchSemaphore);

		/* Open the streams. */

	InputStream = OutputStream = BNULL;

	if(!OpenStreams(Startup->Message.mn_Node.ln_Name,&InputStream,&OutputStream))
		Startup->Result2 = IoErr();
	else
	{
		if(!OutputStream)
		{
			SelectInput(InputStream);
			SetConsoleTask(((struct FileHandle *)BADDR(InputStream))->fh_Type);

			if(!(OutputStream = Open("CONSOLE:",MODE_NEWFILE)))
				Startup->Result2 = IoErr();
		}
	}

	if(InputStream && OutputStream)
	{
		SelectOutput(OutputStream);

		Forbid();

			/* Try to find the global Rexx port. */

		if(!(GlobalRexxPort = FindPort(RXSDIR)))
			Startup->Result2 = ERROR_OBJECT_NOT_FOUND;
		else
		{
			struct MsgPort	 SinglePort;
			struct RexxMsg	*RexxMsg;

				/* Set up our local msgport. */

			InitSinglePort(&SinglePort);

				/* Create a message for Rexx. */

			if(RexxMsg = CreateRexxMsg(&SinglePort,"term",RexxPortName))
			{
					/* And put in the command to execute. */

				if(RexxMsg->rm_Args[0] = (IPTR)CreateArgstring(Startup->Command,strlen(Startup->Command)))
				{
						/* Flag the message as a command to be executed. */

					RexxMsg->rm_Action = RXCOMM;

						/* In case it's just a single string, flag it as being such. */

					if(Startup->Type == LAUNCH_RexxString)
						RexxMsg->rm_Action |= RXFF_STRING;

						/* Clear the port mask. */

					SetSignal(0,SIGF_SINGLE);

						/* Send the message and wait for it to return.
						 * Notice that after the WaitPort() call we
						 * won't call GetMsg() since we don't need the
						 * msgport any more and don't expect more than
						 * one message to end up here.
						 */

					PutMsg(GlobalRexxPort,(struct Message *)RexxMsg);
					WaitPort(&SinglePort);

						/* Store the command result. */

					Startup->Result		= RexxMsg->rm_Result1;
					Startup->Result2	= RexxMsg->rm_Result2;

						/* And clean up the rest. */

					DeleteArgstring((STRPTR)RexxMsg->rm_Args[0]);
				}
				else
					Startup->Result2 = ERROR_NO_FREE_STORE;

				DeleteRexxMsg(RexxMsg);
			}
			else
				Startup->Result2 = ERROR_NO_FREE_STORE;
		}

		Permit();
	}

		/* Call the cleanup routine if any. */

	if(Startup->Cleanup)
		(*Startup->Cleanup)(Startup);

		/* Close the streams. */

	if(InputStream)
		Close(InputStream);

	if(OutputStream)
		Close(OutputStream);

		/* Decrement the launch counter and disable
		 * multitasking for a short while.
		 */

	ObtainSemaphore(&LaunchSemaphore);

	LaunchCounter--;

	Forbid();

		/* Check if we should wake up the process that
		 * created us. If not, dispose of the startup
		 * message.
		 */

	if(Startup->Message.mn_ReplyPort != NULL)
		ReplyMsg((struct Message *)Startup);
	else
		DeleteLaunchMsg(Startup);

		/* Release the launch counter semaphore and exit. */

	ReleaseSemaphore(&LaunchSemaphore);
}

	/* CreateLaunchMsg(WORD Type,STRPTR Command,struct RexxPkt *RexxPkt,LAUNCHCLEANUP Cleanup):
	 *
	 *	This sets up a launch message with given parameters.
	 */

STATIC LaunchMsg *
CreateLaunchMsg(WORD Type,CONST_STRPTR Command,struct RexxPkt *RexxPkt,LAUNCHCLEANUP Cleanup)
{
	LaunchMsg *Startup;

		/* Allocate memory for the data structure and initialize it. */

	if(Startup = (LaunchMsg *)AllocVecPooled(sizeof(LaunchMsg) + strlen(Command) + 1,MEMF_ANY|MEMF_CLEAR))
	{
		Startup->Message.mn_Length	= sizeof(LaunchMsg) + strlen(Command);
		Startup->Type				= Type;
		Startup->RexxPkt			= RexxPkt;
		Startup->Cleanup			= Cleanup;

		strcpy(Startup->Command,Command);
	}

	return(Startup);
}

	/* DeleteLaunchMsg(LaunchMsg *Startup):
	 *
	 *	Releases all the memory allocated for the launch message.
	 */

VOID
DeleteLaunchMsg(LaunchMsg *Startup)
{
	if(Startup->Message.mn_Node.ln_Name)
		FreeVecPooled(Startup->Message.mn_Node.ln_Name);

	FreeVecPooled(Startup);
}

	/* CreateRexxCmdLaunchMsg():
	 *
	 *	Creates a special type of launch message for use with Rexx commands.
	 */

LaunchMsg *
CreateRexxCmdLaunchMsg(CONST_STRPTR RexxCmd,struct RexxPkt *RexxPkt,LAUNCHCLEANUP Cleanup)
{
	return(CreateLaunchMsg(LAUNCH_RexxCmd,RexxCmd,RexxPkt,Cleanup));
}

	/* CreateProgramLaunchMsg():
	 *
	 *	Creates a special type of launch message for use with AmigaDOS commands.
	 */

LaunchMsg *
CreateProgramLaunchMsg(CONST_STRPTR Program,LAUNCHCLEANUP Cleanup)
{
	return(CreateLaunchMsg(LAUNCH_Program,Program,NULL,Cleanup));
}

	/* LaunchSomething(STRPTR OriginalStreamName,BOOL Synchronous,LaunchMsg *Startup):
	 *
	 *	Launch a program either in synchronous or asynchronous fashion, using
	 *	a given output stream. If no stream name is given, "NIL:" will be used.
	 */

LONG
LaunchSomething(STRPTR OriginalStreamName,BOOL Synchronous,LaunchMsg *Startup)
{
	BPTR InputStream,OutputStream;
	BOOL InsertScreenName;
	LONG Result,Error;
	STRPTR StreamName;
	LONG i,Head;

		/* Did we get a name? If not, use "NIL:". */

	if(!OriginalStreamName)
		OriginalStreamName = "NIL:";
	else
	{
			/* Is the string empty? If so, use "NIL:". */

		if(!OriginalStreamName[0])
			OriginalStreamName = "NIL:";
	}

		/* Scan the stream name for "%s" which will be replaced by the
		 * name of the public screen `term' is using.
		 */

	for(i = 0, InsertScreenName = FALSE, Head = 0 ; i < strlen(OriginalStreamName) - 1 ; i++)
	{
		if(OriginalStreamName[i] == '%' && OriginalStreamName[i+1] == 's')
		{
			Head = i;

			InsertScreenName = TRUE;
			break;
		}
	}

		/* Should we insert a public screen name somewhere? */

	if(InsertScreenName)
	{
		UBYTE ScreenName[MAXPUBSCREENNAME + 1];

			/* Get the name of the current default public screen. */

		GetDefaultPubScreen(ScreenName);

			/* If the screen `term' has its main window on is on a
			 * public screen, get the name of this screen. Otherwise,
			 * don't touch the buffer contents.
			 */

		if(Window)
			GetPubScreenName(Window->WScreen,ScreenName);

			/* Make room for the screen name. */

		if(!(StreamName = (STRPTR)AllocVecPooled(strlen(ScreenName) + strlen(OriginalStreamName) - 1,MEMF_ANY)))
		{
			SetIoErr(ERROR_NO_FREE_STORE);

			return(FALSE);
		}

			/* Insert the screen name into the stream name. */

		if(Head > 0)
			strncpy(StreamName,OriginalStreamName,Head);

		StreamName[Head] = 0;

		strcat(StreamName,ScreenName);
		strcat(StreamName,&OriginalStreamName[Head + 2]);
	}
	else
		StreamName = OriginalStreamName;

		/* Reset these. */

	Result = Error = 0;

	InputStream = OutputStream = BNULL;

	if(Startup->Type == LAUNCH_Program)
	{
		if(!OpenStreams(StreamName,&InputStream,&OutputStream))
			Error = IoErr();
		else
		{
			if(Result = SystemTags(Startup->Command,
				SYS_Input,		InputStream,
				SYS_Output,		OutputStream,
				SYS_Asynch,		Synchronous == FALSE,
				SYS_UserShell,	TRUE,
				NP_StackSize,	8000,
			TAG_DONE))
				Error = IoErr();

				/* These will be closed when the program
				 * launched will exit.
				 */

			if(Synchronous == FALSE)
				InputStream = OutputStream = BNULL;
		}

		Startup->Result		= Result;
		Startup->Result2	= Error;
	}
	else
	{
		if(!(Startup->Message.mn_Node.ln_Name = (char *)AllocVecPooled(strlen(StreamName) + 1,MEMF_ANY)))
			Error = IoErr();
		else
		{
				/* The default process launch tags. */

			STATIC struct TagItem DefaultTags[] =
			{
				{ NP_Entry,		(IPTR)LaunchEntry, },
				{ NP_StackSize,	8000, },
				{ NP_Name,		(IPTR)"term Child Process", },
				{ NP_Cli,			TRUE, },
				{ NP_Input,		(IPTR)NULL, },
				{ NP_Output,		(IPTR)NULL, },
				{ NP_CloseInput,	FALSE, },
				{ NP_CloseOutput,	FALSE, },

				{ TAG_DONE }
			};

			struct Process *Child;

				/* Keep the stream name. */

			strcpy(Startup->Message.mn_Node.ln_Name,StreamName);

				/* Launch the child process. */

			if(!(Child = CreateNewProc(DefaultTags)))
			{
				Result	= RETURN_FAIL;
				Error	= IoErr();
			}
			else
			{
					/* If the command is to be executed in asynchronous
					 * fashion, just send the message. The child process
					 * will clean up all on its own.
					 */

				if(Synchronous == FALSE)
					PutMsg(&Child->pr_MsgPort,(struct Message *)Startup);
				else
				{
					struct MsgPort SinglePort;

						/* Set up the local msgport. */

					InitSinglePort(&SinglePort);

						/* Clear the port signal. */

					SetSignal(0,SIGF_SINGLE);

						/* Fill in the reply port. */

					Startup->Message.mn_ReplyPort = &SinglePort;

						/* Send the startup message and wait for the
						 * reply. Note that we don't GetMsg() the
						 * message we sent since the local msgport
						 * will be discarded anyway.
						 */

					PutMsg(&Child->pr_MsgPort,(struct Message *)Startup);
					WaitPort(&SinglePort);

						/* Copy the result data. */

					Result	= Startup->Result;
					Error	= Startup->Result2;
				}
			}
		}
	}

		/* Do the cleanup. */

	if(Synchronous)
	{
		Startup->Result		= Result;
		Startup->Result2	= Error;
	}

	if(Startup->Cleanup)
		(*Startup->Cleanup)(Startup);

	if(Synchronous)
		DeleteLaunchMsg(Startup);

		/* Close the streams still open. */

	if(InputStream)
		Close(InputStream);

	if(OutputStream)
		Close(OutputStream);

		/* If we had allocated a new stream name, release the memory. */

	if(StreamName != OriginalStreamName)
		FreeVecPooled(StreamName);

	SetIoErr(Error);

	return(Result);
}

	/* LaunchSetup():
	 *
	 *	Does the setup operation for the launch procedure.
	 */

VOID
LaunchSetup()
{
	InitSemaphore(&LaunchSemaphore);
}

	/* GetLaunchCounter():
	 *
	 *	Returns the number of processes launched that haven't
	 *	returned already.
	 */

LONG
GetLaunchCounter()
{
	LONG Counter;

	SafeObtainSemaphoreShared(&LaunchSemaphore);

	Counter = LaunchCounter;

	ReleaseSemaphore(&LaunchSemaphore);

	return(Counter);
}
