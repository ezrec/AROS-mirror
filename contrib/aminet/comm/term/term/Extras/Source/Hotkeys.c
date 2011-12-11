/*
**	Hotkeys.c
**
**	Hotkey support routines.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	CX_TERMSCREENTOFRONT,CX_BUFFERSCREENTOFRONT,CX_SKIPDIALENTRY,CX_ABORTAREXX };

	/* Asynchronous hotkey process. */

STATIC struct Process *CxProcess;

STATIC struct NewBroker TermBroker =
{
	NB_VERSION,
	"term",
	"term © 1990-1996 by Olaf Barthel",
	"Terminal program"
};

	/* Hotkey(STRPTR Code,struct MsgPort *Port,LONG ID):
	 *
	 *	A custom version of the amiga.lib supplied code.
	 */

STATIC CxObj *
CustomHotKey(STRPTR Code,struct MsgPort *Port,LONG ID)
{
	CxObj *Filter;

	if(Filter = CxFilter(Code))
	{
		CxObj *Sender;

		if(Sender = CxSender(Port,ID))
		{
			CxObj *Translator;

			AttachCxObj(Filter,Sender);

			if(Translator = CxTranslate(NULL))
			{
				AttachCxObj(Filter,Translator);

				if(!CxObjError(Filter))
					return(Filter);
			}
		}

		DeleteCxObjAll(Filter);
	}

	return(NULL);
}

	/* CreateBroker(struct MsgPort *CxPort):
	 *
	 *	Set up a CxObj commodity broker.
	 */

STATIC CxObj *
CreateBroker(struct MsgPort *CxPort)
{
	CxObj *Broker;

		/* Set the commodity priority. */

	TermBroker.nb_Pri = Hotkeys.CommodityPriority;

		/* Create the broker. */

	if(Broker = CxBroker(&TermBroker,NULL))
	{
			/* Add the hotkeys. */

		AttachCxObj(Broker,CustomHotKey(Hotkeys.termScreenToFront,		CxPort,CX_TERMSCREENTOFRONT));
		AttachCxObj(Broker,CustomHotKey(Hotkeys.BufferScreenToFront,	CxPort,CX_BUFFERSCREENTOFRONT));
		AttachCxObj(Broker,CustomHotKey(Hotkeys.SkipDialEntry,			CxPort,CX_SKIPDIALENTRY));
		AttachCxObj(Broker,CustomHotKey(Hotkeys.AbortARexx,				CxPort,CX_ABORTAREXX));

			/* Did an error show up? */

		if(!CxObjError(Broker))
		{
				/* Broker has been added, now activate it. */

			ActivateCxObj(Broker,Hotkeys.HotkeysEnabled);

			return(Broker);
		}

		DeleteCxObjAll(Broker);
	}

	return(NULL);
}

	/* TermCxServer():
	 *
	 *	Asynchronous hotkey server.
	 */

STATIC VOID SAVE_DS
TermCxServer(VOID)
{
	struct MsgPort *CxPort;
	CxMsg *Message;
	CxObj *Broker;

		/* Create a reply port. */

	if(CxPort = CreateMsgPort())
	{
			/* Add the port to the public list. */

		CxPort->mp_Node.ln_Name = (STRPTR)TermBroker.nb_Name;

		AddPort(CxPort);

			/* Install the port. */

		TermBroker.nb_Port = CxPort;

			/* Create the broker. */

		if(Broker = CreateBroker(CxPort))
		{
			ULONG SignalSet;
			BOOL Terminated;

				/* Signal father task that we're done. */

			Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

				/* Loop and loop... */

			Terminated = FALSE;

			while(!Terminated)
			{
					/* Wait for some signal. */

				SignalSet = Wait(SIG_KILL | SIG_RESET | PORTMASK(CxPort));

					/* ^C aborts. */

				if(SignalSet & SIG_KILL)
					Terminated = TRUE;

					/* ^D removes the broker and
					 * creates a new one.
					 */

				if(SignalSet & SIG_RESET)
				{
					DeleteCxObjAll(Broker);

					Broker = CreateBroker(CxPort);
				}

					/* A commodity message. */

				if(SignalSet & PORTMASK(CxPort))
				{
					ULONG MessageType,MessageID;

						/* Remove all messages. */

					while(Message = (CxMsg *)GetMsg(CxPort))
					{
							/* Extract type and ID. */

						MessageType	= CxMsgID(Message);
						MessageID	= CxMsgType(Message);

						ReplyMsg((struct Message *)Message);

							/* Take a look at the type... */

						switch(MessageID)
						{
								/* A hotkey was pressed. */

							case CXM_IEVENT:

								switch(MessageType)
								{
									case CX_TERMSCREENTOFRONT:

										Forbid();

										if(Window)
											BumpWindow(TopWindow);
										else
											Signal((struct Task *)ThisProcess,SIGBREAKF_CTRL_F);

										Permit();

										break;

									case CX_BUFFERSCREENTOFRONT:

										LaunchBuffer();
										break;

									case CX_SKIPDIALENTRY:

										Signal((struct Task *)ThisProcess,SIG_SKIP);
										break;

									case CX_ABORTAREXX:

										if(InRexx)
											Signal((struct Task *)ThisProcess,SIG_BREAK);

										break;
								}

								break;

								/* An internal commodity command. */

							case CXM_COMMAND:

								switch(MessageType)
								{
									case CXCMD_DISABLE:

										ActivateCxObj(Broker,Hotkeys.HotkeysEnabled = FALSE);
										break;

									case CXCMD_ENABLE:

										ActivateCxObj(Broker,Hotkeys.HotkeysEnabled = TRUE);
										break;
								}

								break;
						}
					}
				}
			}

				/* Remove the broker. */

			DeleteCxObjAll(Broker);
		}

			/* Remove the port from the public list. */

		RemPort(CxPort);

			/* Remove all pendig messages. */

		while(Message = (CxMsg *)GetMsg(CxPort))
			ReplyMsg((struct Message *)Message);

			/* Delete the reply port. */

		DeleteMsgPort(CxPort);
	}

	Forbid();

		/* Clear the task ID. */

	CxProcess = NULL;

		/* Signal father process that we're done. */

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

	/* ShutdownCx():
	 *
	 *	Remove the hotkey task.
	 */

VOID
ShutdownCx()
{
	ShakeHands((struct Task *)CxProcess,SIG_KILL);
}

	/* SetupCx():
	 *
	 *	Create the hotkey task.
	 */

BOOL
SetupCx()
{
		/* If the task is already running, tell it to
		 * update the hotkey settings.
		 */

	if(CxProcess)
	{
		Signal((struct Task *)CxProcess,SIG_RESET);

		return(TRUE);
	}
	else
	{
		if(CxProcess = StartProcessWaitForHandshake("term Hotkey Process",(TASKENTRY)TermCxServer,
			NP_Priority,	0,
			NP_WindowPtr,	-1,
		TAG_DONE))
			return(TRUE);
	}

	return(FALSE);
}

	/* LoadHotkeys(STRPTR Name,struct Hotkeys *Keys):
	 *
	 *	Load the hotkey settings from a file.
	 */

BOOL
LoadHotkeys(STRPTR Name,struct Hotkeys *Keys)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(!(Handle = OpenIFFStream(Name,MODE_OLDFILE)))
		Error = IoErr();
	else
	{
		/* Collect version number ID if
		 * available.
		 */

		if(!(Error = PropChunks(Handle,(LONG *)VersionProps,1)))
		{
			/* The following line tells iffparse to stop at the
			 * very beginning of a `Type' chunk contained in a
			 * `TERM' FORM chunk.
			 */

			if(!(Error = StopChunk(Handle,ID_TERM,ID_HOTK)))
			{
					/* Parse the file... */

				if(!ParseIFF(Handle,IFFPARSE_SCAN))
				{
					struct StoredProperty *Prop;

						/* Did we get a version ID? */

					if(!(Prop = FindProp(Handle,ID_TERM,ID_VERS)))
						Error = ERROR_OBJECT_WRONG_TYPE;
					else
					{
						struct TermInfo *TermInfo;

						TermInfo = (struct TermInfo *)Prop->sp_Data;

						if((TermInfo->Version < CONFIG_FILE_VERSION) || (TermInfo->Version == CONFIG_FILE_VERSION && TermInfo->Revision < CONFIG_FILE_REVISION))
						{
							if(ReadChunkBytes(Handle,Keys,sizeof(struct HotkeysOld)) != sizeof(struct HotkeysOld))
								Error = IoErr();
							else
								strcpy(Keys->AbortARexx,"lshift rshift escape");
						}
						else
						{
							if(ReadChunkBytes(Handle,Keys,sizeof(struct Hotkeys)) != sizeof(struct Hotkeys))
								Error = IoErr();
						}
					}
				}
			}
		}

		CloseIFFStream(Handle);
	}

	if(Error)
	{
		SetIoErr(Error);
		return(FALSE);
	}
	else
		return(TRUE);
}
