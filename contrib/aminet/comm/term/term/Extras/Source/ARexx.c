/*
**	ARexx.c
**
**	ARexx interface general support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* IsNumeric(STRPTR String):
	 *
	 *	Is the string really a number?
	 */

BOOL
IsNumeric(STRPTR String)
{
	while(*String == ' ' || *String == '\t')
		String++;

	while(*String)
	{
		if(*String < '0' || *String > '9')
			return(FALSE);
		else
			String++;
	}

	return(TRUE);
}

	/* CreateResult(CONST_STRPTR ResultString,LONG *Results):
	 *
	 *	Create a proper Rexx result string.
	 */

STRPTR
CreateResult(CONST_STRPTR ResultString,LONG *Results)
{
	STRPTR Result;

	if(!(Result = CreateArgstring(ResultString,strlen(ResultString))))
	{
		Results[0] = RC_ERROR;
		Results[1] = ERROR_NO_FREE_STORE;
	}

	return(Result);
}

	/* CreateResultLen(CONST_STRPTR ResultString,LONG *Results):
	 *
	 *	Create a proper Rexx result string given the
	 *	length of the source string.
	 */

STRPTR
CreateResultLen(CONST_STRPTR ResultString,LONG *Results,LONG Len)
{
	STRPTR Result;

	if(!(Result = CreateArgstring(ResultString,Len)))
	{
		Results[0] = RC_ERROR;
		Results[1] = ERROR_NO_FREE_STORE;
	}

	return(Result);
}

	/* CreateVarArgs(STRPTR Value,struct RexxPkt *Packet,STRPTR Stem,...):
	 *
	 *	Set a Rexx variable, special treatment.
	 */

BOOL
CreateVarArgs(CONST_STRPTR Value,struct RexxPkt *Packet,STRPTR Stem,...)
{
	UBYTE Name[256];
	va_list VarArgs;
	LONG Result;

	va_start(VarArgs,Stem);
	LimitedVSPrintf(sizeof(Name),Name,Stem,VarArgs);
	va_end(VarArgs);

	if((Result = SetRexxVar(Packet->RexxMsg,Name,(char *)Value,strlen(Value))))
	{
		Packet->Results[0] = RC_ERROR;
		Packet->Results[1] = Result;

		return(FALSE);
	}
	else
		return(TRUE);
}

	/* CreateVar(STRPTR Value,struct RexxPkt *Packet,STRPTR Name):
	 *
	 *	Set a Rexx variable, simple version.
	 */

STRPTR
CreateVar(CONST_STRPTR Value,struct RexxPkt *Packet,STRPTR Name)
{
	LONG Result;

	if(Result = SetRexxVar(Packet->RexxMsg,Name,(char *)Value,strlen(Value)))
	{
		Packet->Results[0] = RC_ERROR;
		Packet->Results[1] = Result;
	}

	return(NULL);
}

	/* CreateMatchBuffer(STRPTR Pattern):
	 *
	 *	Create a pattern buffer suitable for pattern matching.
	 */

STRPTR
CreateMatchBuffer(STRPTR Pattern)
{
	STRPTR Buffer;
	LONG Len;

	Len = strlen(Pattern) + 1;

	if(Buffer = (STRPTR)AllocVecPooled(2 * Len,MEMF_ANY))
	{
		if(ParsePatternNoCase(Pattern,Buffer,2 * Len) != -1)
			return(Buffer);
		else
			FreeVecPooled(Buffer);
	}

	return(NULL);
}

	/* MatchBuffer(STRPTR Buffer,STRPTR Name):
	 *
	 *	Match a pattern against a string.
	 */

BOOL
MatchBuffer(STRPTR Buffer,STRPTR Name)
{
	return((BOOL)(MatchPatternNoCase(Buffer,Name) != FALSE));
}

	/* DeleteMatchBuffer(STRPTR Buffer):
	 *
	 *	Free a pattern matching buffer.
	 */

VOID
DeleteMatchBuffer(STRPTR Buffer)
{
	FreeVecPooled(Buffer);
}

	/* ToMode(STRPTR Name):
	 *
	 *	Turn a transfer mode name into a key.
	 */

LONG
ToMode(STRPTR Name)
{
	STATIC STRPTR TransferModes[] =
	{
		"BINARY",
		"TEXT",
		"ASCII",
		NULL
	};

	LONG i;

	for(i = 0 ; TransferModes[i] ; i++)
	{
		if(!Stricmp(Name,TransferModes[i]))
			return(i);
	}

	return(-1);
}

	/* ToList(STRPTR Name):
	 *
	 *	Turn a list name into a key.
	 */

LONG
ToList(STRPTR Name)
{
	STATIC STRPTR ListTypes[GLIST_COUNT] =
	{
		"UPLOAD",
		"DOWNLOAD",
		"DIAL",
		"WAIT",
		"TRAP"
	};

	LONG i;

	for(i = 0 ; i < GLIST_COUNT ; i++)
	{
		if(!Stricmp(Name,ListTypes[i]))
			return(i);
	}

	return(-1);
}

	/* ToConfig(STRPTR Name):
	 *
	 *	Turn a configuration name into a key.
	 */

LONG
ToConfig(STRPTR Name)
{
	STATIC STRPTR DataTypes[DATATYPE_COUNT] =
	{
		"TRANSLATIONS",
		"FUNCTIONKEYS",
		"CURSORKEYS",
		"FASTMACROS",
		"HOTKEYS",
		"SPEECH",
		"SOUND",
		"BUFFER",
		"CONFIGURATION",
		"PHONE",
		"SCREENTEXT",
		"SCREENIMAGE"
	};

	LONG i;

	for(i = 0 ; i < DATATYPE_COUNT ; i++)
	{
		if(!Stricmp(Name,DataTypes[i]))
			return(i);
	}

	return(-1);
}

	/* ToRequester(STRPTR Name):
	 *
	 *	Turn a requester name into a key.
	 */

LONG
ToRequester(STRPTR Name)
{
	STRPTR RequesterTypes[REQUESTER_COUNT] =
	{
		"SERIAL",
		"MODEM",
		"SCREEN",
		"TERMINAL",
		"EMULATION",
		"CLIPBOARD",
		"CAPTURE",
		"COMMANDS",
		"MISC",
		"PATH",
		"TRANSFER",
		"TRANSLATIONS",
		"FUNCTIONKEYS",
		"CURSORKEYS",
		"FASTMACROS",
		"HOTKEYS",
		"SPEECH",
		"SOUND",
		"PHONE"
	};

	LONG i;

	for(i = 0 ; i < REQUESTER_COUNT ; i++)
	{
		if(!Stricmp(Name,RequesterTypes[i]))
			return(i);
	}

	return(-1);
}

	/* ToWindow(STRPTR Name):
	 *
	 *	Turn a window name into a key.
	 */

LONG
ToWindow(STRPTR Name)
{
	STATIC STRPTR WindowTypes[WINDOWID_COUNT] =
	{
		"BUFFER",
		"REVIEW",
		"PACKET",
		"FASTMACROS",
		"STATUS",
		"MAIN",
		"UPLOADQUEUE",
		"SINGLECHARENTRY"
	};

	LONG i;

	for(i = 0 ; i < WINDOWID_COUNT ; i++)
	{
		if(!Stricmp(WindowTypes[i],Name))
			return(i);
	}

	return(-1);
}

	/* ReplyRexxCommand():
	 *
	 *	Reply a command request the rexx server - or someone else -
	 *	has passed to us.
	 */

STATIC VOID
ReplyRexxCommand(struct RexxMsg *RexxMessage,LONG Primary,LONG Secondary)
{
	if(RexxMessage)
	{
		RexxMessage->rm_Result1 = Primary;
		RexxMessage->rm_Result2 = Secondary;

		ReplyMsg((struct Message *)RexxMessage);
	}
}

	/* InvokeRexxCommand(struct RexxPkt *Packet):
	 *
	 *	Invoke an ARexx command.
	 */

STATIC BOOL
InvokeRexxCommand(struct RexxPkt *Packet)
{
		/* Asynchronous command? */

	if(Packet->CommandInfo->Async)
	{
			/* Execute the command on the schedule of the rexx server process. */

		STRPTR Result = (*Packet->CommandInfo->Routine)(Packet);

			/* Will the command clean up all by itself? */

		if(!Packet->CommandInfo->AutoCleanup)
			RexxPktCleanup(Packet,Result);
	}
	else
	{
		struct RexxPkt *NewPacket;

			/* Create message packet. */

		if(NewPacket = (struct RexxPkt *)AllocVecPooled(sizeof(struct RexxPkt),MEMF_ANY))
		{
				/* Set up message packet. */

			CopyMem(Packet,NewPacket,sizeof(struct RexxPkt));

			NewPacket->Message.mn_ReplyPort	= RexxPort;
			NewPacket->Message.mn_Length	= sizeof(struct RexxPkt);

				/* Post it. */

			PutMsg(TermRexxPort,(struct Message *)NewPacket);
		}
		else
			ReplyRexxCommand(Packet->RexxMsg,-1,0);
	}

	return(TRUE);
}

	/* ParseRexxCommand(struct RexxMsg *RexxMsg):
	 *
	 *	Handles the synchronous Rexx commands and returns the
	 *	message if no matching command is found.
	 */

STATIC BOOL
ParseRexxCommand(struct RexxMsg *RexxMsg)
{
	UBYTE CommandBuffer[MAX_FILENAME_LENGTH];
	STRPTR Command,CommandArgs;
	LONG Len;

		/* Clear the local variables. */

	CommandBuffer[0]	= 0;
	Command				= (STRPTR)RexxMsg->rm_Args[0];
	CommandArgs			= NULL;
	Len					= 0;

		/* Skip leading blank spaces. */

	while(*Command == ' ' || *Command == '\t')
		Command++;

		/* Extract the command name. */

	do
	{
			/* Found the end of the string? */

		if(!Command[Len])
		{
				/* Copy the command name. */

			LimitedStrcpy(sizeof(CommandBuffer),CommandBuffer,Command);

				/* No arguments are provided. */

			CommandArgs = NULL;

			break;
		}

			/* Found a blank space? */

		if(Command[Len] == ' ' || Command[Len] == '\t')
		{
				/* Copy the command name. */

			CopyMem(Command,CommandBuffer,Len);

			CommandBuffer[Len] = 0;

				/* Look for any arguments. */

			CommandArgs = &Command[Len + 1];

				/* Skip blank spaces. */

			while(*CommandArgs == ' ' || *CommandArgs == '\t')
				CommandArgs++;

			break;
		}
	}
	while(++Len < 30);

		/* Did we find a command name? */

	if(CommandBuffer[0])
	{
		struct CommandInfo *CommandInfo;
		LONG CommandIndex;

			/* Which command is it? */

		for(CommandIndex = 0, CommandInfo = NULL ; CommandIndex < CommandTableSize ; CommandIndex++)
		{
			if(!Stricmp(CommandBuffer,CommandTable[CommandIndex].Name))
			{
				CommandInfo = &CommandTable[CommandIndex];

				break;
			}
		}

			/* Did we find the command? */

		if(CommandInfo)
		{
			struct RexxPkt Packet;
			BOOL Processed;

			Processed = FALSE;

				/* Set the result codes to defaults. */

			Packet.Results[0]	= RC_OK;
			Packet.Results[1]	= 0;

				/* Fill in the rest. */

			Packet.CommandInfo	= CommandInfo;
			Packet.RexxMsg		= RexxMsg;

				/* Does this command accept any arguments? */

			if(CommandInfo->Arguments)
			{
				IPTR *Array;

					/* Determine length of argument string. */

				if(CommandArgs)
					Len = strlen(CommandArgs);
				else
					Len = 0;

					/* Allocate temporary buffer, we will need to
					 * attach a line-feed character to the argument
					 * string.
					 */

				if(Array = (IPTR *)AllocVecPooled(12 * sizeof(IPTR) + Len + 2,MEMF_ANY | MEMF_CLEAR))
				{
					struct RDArgs *Args;
					STRPTR Buffer;

						/* Get the argument buffer. */

					Buffer = (STRPTR)&Array[12];

						/* Copy the argument string. */

					if(CommandArgs && Len)
						CopyMem(CommandArgs,Buffer,Len);

						/* Attach the line-feed character. */

					Buffer[Len] = '\n';

						/* Allocate argument parser data. */

					if(Args = (struct RDArgs *)AllocDosObjectTags(DOS_RDARGS,TAG_DONE))
					{
						Packet.Array	= (STRPTR *)Array;
						Packet.Args		= Args;

							/* Don't prompt for input! */

						Args->RDA_Flags |= RDAF_NOPROMPT;

							/* Set up parser data. */

						Args->RDA_Source.CS_Buffer	= Buffer;
						Args->RDA_Source.CS_Length	= Len + 1;
						Args->RDA_Source.CS_CurChr	= 0;

							/* Parse the arguments. */

						if(ReadArgs(CommandInfo->Arguments,Array,Args))
						{
							BOOL ArgsRequired;
							UWORD Inclusion;
							LONG i,Counted;

							Inclusion		= InclusionTable[CommandIndex];
							ArgsRequired	= FALSE;

								/* Look for required arguments. */

							for(i = Counted = 0 ; i < 12 ; i++)
							{
								if(Inclusion & (1L << i))
								{
									ArgsRequired = TRUE;

									if(Array[i])
										Counted++;
								}
							}

								/* Are any arguments required
								 * but not provided?
								 */

							if(ArgsRequired && !Counted)
							{
								Packet.Results[0] = RC_ERROR;
								Packet.Results[1] = ERROR_REQUIRED_ARG_MISSING;
							}
							else
							{
								struct ExclusionInfo *Exclusion = ExclusionTable[CommandIndex];

									/* Any mutually-exclusive arguments? */

								if(Exclusion)
								{
									BOOL ArgsOkay = TRUE;

									i = 0;

										/* Look for arguments to
										 * exclude each other.
										 */

									while(ArgsOkay && Exclusion[i].A != -1)
									{
										if(Array[Exclusion[i].A] && Array[Exclusion[i].B])
											ArgsOkay = FALSE;
										else
											i++;
									}

										/* All arguments correct? */

									if(ArgsOkay)
										Processed = InvokeRexxCommand(&Packet);
									else
									{
										Packet.Results[0] = RC_ERROR;
										Packet.Results[1] = ERROR_TOO_MANY_ARGS;
									}
								}
								else
									Processed = InvokeRexxCommand(&Packet);
							}

								/* Free allocated parser data. */

							if(!Processed)
								FreeArgs(Args);
						}
						else
						{
							LONG Error = IoErr();

							SetIoErr(Error);

							Packet.Results[0] = RC_ERROR;
							Packet.Results[1] = IoErr();
						}

							/* Free parser data. */

						if(!Processed)
							FreeDosObject(DOS_RDARGS,Args);
					}
					else
					{
						Packet.Results[0] = RC_ERROR;
						Packet.Results[1] = ERROR_NO_FREE_STORE;
					}

						/* Free temporary buffer. */

					if(!Processed)
						FreeVecPooled(Array);
				}
				else
				{
					Packet.Results[0] = RC_ERROR;
					Packet.Results[1] = ERROR_NO_FREE_STORE;
				}
			}
			else
			{
				Packet.Array	= NULL;
				Packet.Args		= NULL;

				Processed = InvokeRexxCommand(&Packet);
			}

			if(!Processed)
			{
				if(Packet.Results[0] && Packet.Results[1])
					LastRexxError = Packet.Results[1];

				ReplyRexxCommand(RexxMsg,Packet.Results[0],Packet.Results[1]);
			}

			return(TRUE);
		}
	}

	return(FALSE);
}

	/* RexxPktCleanup(struct RexxPkt *Packet,STRPTR Result):
	 *
	 *	Free the memory allocated for a message packet.
	 */

VOID
RexxPktCleanup(struct RexxPkt *Packet,STRPTR Result)
{
	if(Packet->Args)
	{
		FreeArgs(Packet->Args);

		FreeDosObject(DOS_RDARGS,Packet->Args);
	}

	FreeVecPooled(Packet->Array);

	if(Packet->Results[0])
	{
			/* Store error code. */

		if(Packet->Results[1])
		{
			UBYTE Buffer[10];

			LimitedSPrintf(sizeof(Buffer),Buffer,"%ld",LastRexxError = Packet->Results[1]);

			if(Packet->RexxMsg)
				SetRexxVar(Packet->RexxMsg,"TERM.LASTERROR",Buffer,strlen(Buffer));
		}

		ReplyRexxCommand(Packet->RexxMsg,Packet->Results[0],Packet->Results[1]);

		if(Result)
			DeleteArgstring(Result);
	}
	else
	{
		if(Result)
		{
			if(Packet->RexxMsg)
			{
				if(Packet->RexxMsg->rm_Action & RXFF_RESULT)
					ReplyRexxCommand(Packet->RexxMsg,0,(IPTR)Result);
				else
				{
					DeleteArgstring(Result);

					ReplyRexxCommand(Packet->RexxMsg,0,0);
				}
			}
			else
				DeleteArgstring(Result);
		}
		else
			ReplyRexxCommand(Packet->RexxMsg,0,0);
	}
}

	/* RexxServer(VOID):
	 *
	 *	Asynchronous ARexx host server.
	 */

VOID SAVE_DS
RexxServer(VOID)
{
		/* Create the public host port. */

	if(RexxPort = CreateMsgPort())
	{
		struct RexxMsg *RexxMsg;
		BOOL Done = FALSE;
		ULONG SignalSet;

		RexxPort->mp_Node.ln_Name	= RexxPortName;
		RexxPort->mp_Node.ln_Pri	= 1;

			/* Make it a public port. */

		AddPort(RexxPort);

			/* Signal our father that we're running. */

		Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

			/* Go into loop and wait for input. */

		do
		{
			SignalSet = Wait(SIG_KILL | PORTMASK(RexxPort));

				/* Are we to quit? */

			if(SignalSet & SIG_KILL)
				Done = TRUE;

				/* This is probably a Rexx command. */

			if(SignalSet & PORTMASK(RexxPort))
			{
					/* Pick up all the messages. */

				while(RexxMsg = (struct RexxMsg *)GetMsg(RexxPort))
				{
						/* This is probably the reply to some
						 * synchronous function invocation.
						 */

					if(RexxMsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
						FreeVecPooled(RexxMsg);
					else
					{
							/* At first try to run the
							 * command asynchronously.
							 * If this turns out to be
							 * somewhat `impossible' pass
							 * it to the `term' main process
							 * or - if in batch mode - try
							 * to deal with the message
							 * on our own.
							 */

						if(!ParseRexxCommand(RexxMsg))
						{
							UBYTE Buffer[10];

							LimitedSPrintf(sizeof(Buffer),Buffer,"%ld",LastRexxError = TERMERROR_UNKNOWN_COMMAND);

							SetRexxVar(RexxMsg,"TERM.LASTERROR",Buffer,strlen(Buffer));

							ReplyRexxCommand(RexxMsg,RC_ERROR,TERMERROR_UNKNOWN_COMMAND);
						}
					}
				}
			}
		}
		while(!Done);

			/* Process remaining messages. */

		while(RexxMsg = (struct RexxMsg *)GetMsg(RexxPort))
		{
			if(RexxMsg->rm_Node.mn_Node.ln_Type == NT_REPLYMSG)
				FreeVecPooled(RexxMsg);
			else
				ReplyRexxCommand(RexxMsg,-1,0);
		}

		DeleteMsgPort(RexxPort);

		RexxPort = NULL;
	}

	Forbid();

	RexxProcess = NULL;

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

	/* HandleRexxJob(JobNode *UnusedJob):
	 *
	 *	Tiny & simple subroutine to read and examine all
	 *	messages coming in to be processed synchronously
	 *	by the `term' main process.
	 */

BOOL
HandleRexxJob(JobNode *UnusedJob)
{
	struct RexxPkt *Packet;

		/* Obtain the message packet. */

	if(Packet = (struct RexxPkt *)GetMsg(TermRexxPort))
	{
		STRPTR Result;

		InRexx = TRUE;

		UpdateRequired = TransferUpdateRequired = FALSE;

			/* Execute the command. */

		PushStatus(STATUS_AREXX);
		Result = (*Packet->CommandInfo->Routine)(Packet);
		PopStatus();

			/* Free the packet data. */

		RexxPktCleanup(Packet,Result);

			/* Update the configuration if necessary. */

		if(UpdateRequired)
			ConfigSetup();

			/* Update the XPR options if necessary. */

		if(TransferUpdateRequired)
		{
			if(ProtocolSetup(TRUE))
				SaveProtocolOpts();
		}

			/* Return the message packet. */

		ReplyMsg((struct Message *)Packet);

		InRexx = FALSE;

		return(TRUE);
	}
	else
		return(FALSE);
}
