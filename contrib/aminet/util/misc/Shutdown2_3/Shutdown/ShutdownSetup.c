#include "ShutdownSetup.h"

#include "shutdownbase.h"

#include "LocaleSupport.h"
#include "shutdown.h"

enum	{	ARG_POPKEY,ARG_PRIORITY,ARG_NOCX };

#define SIG_KILL	SIGBREAKF_CTRL_C
#define SIG_HANDSHAKE	SIGBREAKF_CTRL_C

#define PROMPT		"K=CX_PopKey/K,P=CX_Priority/K/N,N=NoCx/S"

struct ShutdownBase
{
	struct Library		 LibNode;

	struct SignalSemaphore	 BlockLock;
	struct SignalSemaphore	 DevBlockLock;
	struct SignalSemaphore	 AccessLock;
	struct SignalSemaphore	 ShutdownLock;

	BYTE			 Shutdown;
	BYTE			 DevShutdown;
	BYTE			 Running;
	BYTE			 Closing;

	struct MinList		 AccessList;
	struct MinList		 ShutdownList;

	VOID			(*Main)();

	struct Process		*Father,
				*Child,
				*CxProcess;

	LONG			 OpenCount;

	LONG			 ErrorCode;
};

STATIC STRPTR		 VersTag = VERSTAG;

struct ExecBase		*SysBase;
struct DosLibrary	*DOSBase;

struct Library		*CxBase;
struct ShutdownBase	*ShutdownBase;

struct NewBroker NewBroker =
{
	NB_VERSION,
	NULL,
	NULL,
	NULL,
	NBU_UNIQUE,
	NULL,
	0,NULL,0
};

CxObj			*Broker;
struct MsgPort		*CxPort;

UBYTE			 CxPopKey[256];
LONG			 CxPriority	= 0,
			 CxError	= 0;

LONG __saveds		Main(VOID);
VOID __saveds __asm	ProcessCleanup(register __d1 BPTR SegList);
struct Process *	Detach(STRPTR Name,BYTE Pri,LONG StackSize,APTR Function);
CxObj * __regargs	CustomHotKey(STRPTR Code,struct MsgPort *Port,LONG ID);
VOID			ShutdownCx(VOID);
BYTE			SetupCx(VOID);
VOID __saveds		CxServer(VOID);

LONG __saveds
Main()
{
	struct Process		*ThisProcess;
	LONG			 Result = RETURN_FAIL;

	SysBase = *(struct ExecBase **)4;

	ThisProcess = (struct Process *)SysBase -> ThisTask;

	if(ThisProcess -> pr_CLI)
	{
		APTR OldPtr = ThisProcess -> pr_WindowPtr;

		if(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37))
		{
			struct Process	*Father;
			BPTR		 StdOut;
			struct RDArgs	*ArgsPtr;
			STRPTR		 Args[3] = { NULL, NULL, NULL };
			BYTE             UseCx = TRUE;

			LocaleOpen("shutdown.catalog","english");

			if(ArgsPtr = ReadArgs(PROMPT,(LONG *)Args,NULL))
			{
				if(Args[ARG_NOCX])
					UseCx = FALSE;

				if(Args[ARG_POPKEY])
					strcpy(CxPopKey,Args[ARG_POPKEY]);
				else
					strcpy(CxPopKey,"lcommand rcommand tab");

				if(Args[ARG_PRIORITY])
					CxPriority = *((LONG *)Args[ARG_PRIORITY]);

				FreeArgs(ArgsPtr);
			}
			else
			{
				PrintFault(IoErr(),"ShutdownSetup");

				CloseLibrary(DOSBase);

				return(RETURN_ERROR);
			}

			Printf(GetString(MSG_HEADER_TXT),VERS);

			Father = (struct Process *)SysBase -> ThisTask;

			StdOut = Father -> pr_COS;

			if(ShutdownBase = (struct ShutdownBase *)OpenLibrary("shutdown.library",2))
			{
				if(ShutdownBase -> Child)
				{
					Printf(GetString(MSG_REMOVING_TXT));

					Flush(StdOut);

					ShutdownBase -> Father = Father;

					Signal(ShutdownBase -> Child,SIG_KILL);

					Wait(SIG_HANDSHAKE);

					if(ShutdownBase -> CxProcess)
					{
						ShutdownBase -> Father = Father;

						Signal(ShutdownBase -> CxProcess,SIG_KILL);

						Wait(SIG_HANDSHAKE);
					}

					Printf(GetString(MSG_DONE_TXT));

					Result = RETURN_OK;
				}
				else
				{
					Printf(GetString(MSG_INSTALLING_TXT));

					Flush(StdOut);

					ShutdownBase -> Father = Father;

					if(CreateNewProcTags(
						NP_Entry,	ShutdownBase -> Main,
						NP_StackSize,	4000,
						NP_Name,	GetString(MSG_TASKNAME_NAME),
						NP_Priority,	5,
						NP_WindowPtr,	-1,
					TAG_DONE))
					{
						Wait(SIG_HANDSHAKE);

						if(!ShutdownBase -> Child)
							Printf(GetString(MSG_PROCESS_CREATION_FAILED_TXT));
						else
						{
							if(UseCx)
							{
								ShutdownBase -> Father = Father;

								Detach(GetString(MSG_BROKERNAME_TXT),80,4000,CxServer);

								Wait(SIG_HANDSHAKE);

								if(ShutdownBase -> CxProcess)
								{
									Printf(GetString(MSG_DONE_TXT));

									Result = RETURN_OK;
								}
								else
								{
									ShutdownBase -> Father = Father;

									Signal(ShutdownBase -> Child,SIG_KILL);

									Wait(SIG_HANDSHAKE);

									Printf(GetString(MSG_HOTKEYS_FAILED_TXT),CxError);
								}
                                                        }
                                                        else
                                                        {
								Printf(GetString(MSG_DONE_TXT));

								Result = RETURN_OK;
                                                        }
						}
					}
					else
						Printf(GetString(MSG_PROCESS_CREATION_FAILED_TXT));
				}
			}
			else
				Printf(GetString(MSG_OPENFAIL_TXT),"shutdown.library");

			LocaleClose();

			if(ShutdownBase)
			{
				if(!ShutdownBase -> CxProcess)
				{
					CloseLibrary(DOSBase);

					CloseLibrary(ShutdownBase);
				}
			}
			else
				CloseLibrary(DOSBase);
		}

		ThisProcess -> pr_WindowPtr = OldPtr;
	}
	else
	{
		WaitPort(&ThisProcess -> pr_MsgPort);

		Forbid();

		ReplyMsg(GetMsg(&ThisProcess -> pr_MsgPort));
	}

	return(Result);
}

VOID __saveds __asm
ProcessCleanup(register __d1 BPTR SegList)
{
	Forbid();

	UnLoadSeg(SegList);

	CloseLibrary(ShutdownBase);

	CloseLibrary(DOSBase);
}

struct Process *
Detach(STRPTR Name,BYTE Pri,LONG StackSize,APTR Function)
{
	struct Process			*Child;
	struct CommandLineInterface	*CLI;

	CLI = (struct CommandLineInterface *)BADDR(((struct Process *)SysBase -> ThisTask) -> pr_CLI);

	if(Child = CreateNewProcTags(
		NP_Name,	Name,
		NP_CommandName,	Name,
		NP_Priority,	Pri,
		NP_StackSize,	StackSize,
		NP_Entry,	Function,
		NP_Cli,		TRUE,
		NP_ExitCode,	ProcessCleanup,
		NP_ExitData,	CLI -> cli_Module,
	TAG_DONE))
	{
		CLI -> cli_Module = NULL;

		return(Child);
	}
	else
		return(NULL);
}

CxObj * __regargs
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

				if(!(CxError = CxObjError(Filter)))
					return(Filter);
			}
		}

		DeleteCxObjAll(Filter);
	}

	return(NULL);
}

VOID
ShutdownCx()
{
	if(CxPort)
	{
		struct Message *Message;

		if(Broker)
			DeleteCxObjAll(Broker);

		RemPort(CxPort);

		while(Message = GetMsg(CxPort))
			ReplyMsg(Message);

		DeleteMsgPort(CxPort);

		CxPort = NULL;
		Broker = NULL;
	}
}

BYTE
SetupCx()
{
	ShutdownCx();

	if(CxPort = CreateMsgPort())
	{
		NewBroker . nb_Name	= GetString(MSG_BROKERTITLE_TXT);
		NewBroker . nb_Title	= GetString(MSG_BROKERTITLE_TXT);
		NewBroker . nb_Descr	= GetString(MSG_BROKERDESCRIPTION_TXT);

		CxPort -> mp_Node . ln_Name = NewBroker . nb_Name;

		AddPort(CxPort);

		NewBroker . nb_Port = CxPort;
		NewBroker . nb_Pri  = CxPriority;

		if(Broker = CxBroker(&NewBroker,NULL))
		{
			AttachCxObj(Broker,CustomHotKey(CxPopKey,CxPort,0));

			if(!CxObjError(Broker))
			{
				ActivateCxObj(Broker,TRUE);

				return(TRUE);
			}
		}
	}

	ShutdownCx();

	return(FALSE);
}

VOID __saveds
CxServer()
{
	if(CxBase = OpenLibrary("commodities.library",37))
	{
		if(SetupCx())
		{
			ULONG	SignalSet;
			BYTE	Terminated = FALSE;

			ShutdownBase -> CxProcess = (struct Process *)SysBase -> ThisTask;

			Signal(ShutdownBase -> Father,SIG_HANDSHAKE);

			ShutdownBase -> Father = NULL;

			while(!Terminated)
			{
				SignalSet = Wait(SIG_KILL | (1 << CxPort -> mp_SigBit));

				if(SignalSet & SIG_KILL)
					Terminated = TRUE;

				if(SignalSet & (1 << CxPort -> mp_SigBit))
				{
					ULONG	 MessageType,
						 MessageID;
					CxMsg	*Message;

					while(Message = (CxMsg *)GetMsg(CxPort))
					{
						MessageType	= CxMsgID(Message);
						MessageID	= CxMsgType(Message);

						ReplyMsg((struct Message *)Message);

						switch(MessageID)
						{
							case CXM_IEVENT:	Shutdown(SHUTDOWN_FAST);

							case CXM_COMMAND:	switch(MessageType)
										{
											case CXCMD_DISABLE:	ActivateCxObj(Broker,FALSE);
														break;

											case CXCMD_ENABLE:	ActivateCxObj(Broker,TRUE);
														break;

											case CXCMD_KILL:	Terminated = TRUE;
														break;

											default:		break;
										}

										break;
						}
					}
				}
			}

			ShutdownCx();
		}

		CloseLibrary(CxBase);
	}

	Forbid();

	ShutdownBase -> CxProcess = NULL;

	if(ShutdownBase -> Father)
		Signal(ShutdownBase -> Father,SIG_HANDSHAKE);
}
