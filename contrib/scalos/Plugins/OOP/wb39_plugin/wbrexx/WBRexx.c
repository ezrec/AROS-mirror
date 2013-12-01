// WBRexx.c
// $Date$
// $Revision$

#define	__USE_OLD_TIMEVAL__	1

#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>

#include <libraries/commodities.h>
#include <workbench/workbench.h>

#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <clib/alib_protos.h>

#include <proto/rexxsyslib.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/scalos.h>
#include <proto/commodities.h>

#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>

#include <defs.h>

#include "wbrexx.h"

#ifdef __amigaos4__
#define SetRexxVar(msg, var, val, len) SetRexxVarFromMsg((var), (val), (msg))
#endif

/****************************************************************************/

static struct RexxHost *CreateRexxHost(CONST_STRPTR, ULONG, const struct CmdFunc *, const APTR);
static VOID DeleteRexxHost(struct RexxHost * Host);
static struct RexxParseData *CreateRexxParseData(VOID);
static VOID DeleteRexxParseData(struct RexxParseData **Data);
static VOID ReturnErrorMsg(struct RexxMsg *, CONST_STRPTR, LONG);
static VOID HandleRexxMsg(const struct RexxHost *, struct RexxParseData *, struct RexxMsg *);

static SAVEDS(void) WBRexxProcess(void);
static int WBRexxProcessInit(struct LocalData *LocalData);
static void DeleteToolMenuItem(struct ToolMenuItem *oldItem);
static STRPTR StringDup(CONST_STRPTR str);
static void DeleteKbdCommand(struct KeyboardCommand *kbc);
CONST_STRPTR CopyMenuTitle(STRPTR Dest, CONST_STRPTR Src, size_t MaxLen);
static struct ToolMenuItem *FindParentMenuItem(struct ToolMenuItem *tmi);

/****************************************************************************/

// from WBRexxMain.c
extern struct DosLibrary *DOSBase;
extern T_UTILITYBASE UtilityBase;
extern struct ScalosBase *ScalosBase;
extern struct Library *CxBase;


// To be initialized by this module
T_REXXSYSBASE RexxSysBase;

#ifdef __amigaos4__
struct RexxSysIFace *IRexxSys;
#endif

LONG LastError = RETURN_OK;		// will be returned by APPLICATION.LASTERROR

struct List MenuItemList;		// list of added tool menu items

static struct SignalSemaphore MenuItemListSemaphore;

static struct MsgPort *ToolMenuMsgPort;	// MsgPort for tool menu APPMENU messages

struct List KbdCommandList;		// list of keyboard commands

static struct SignalSemaphore KbdCommandListSemaphore;

static struct MsgPort *globalReplyPort;	// ReplyPort for DoRexxCommand


CONST_STRPTR DefIconsPath = "ENV:sys/def_";


/****************************************************************************/

/****** Host/CreateRexxHost **************************************************
*
*   NAME
*       CreateRexxHost -- Create a public message port for use as an ARexx
*           host address.
*
*   SYNOPSIS
*       Host = CreateRexxHost(BaseName,Index,CommandTable,UserData)
*
*       struct RexxHost *CreateRexxHost(const STRPTR BaseName,ULONG Index,
*           const struct CmdFunc *CommandTable,const APTR UserData);
*
*   FUNCTION
*       You supply a name and a project index, this routine will set up
*       a public message port for use as an ARexx host address for you
*       with the name you provided. The name will be checked for validity
*       (it may not hold blank spaces or tabs) and converted to all
*       upper case letters. If a non-zero project index number is provided,
*       a numeric extension will be added to the message port name.
*       This routine will try to make sure that the message port will bear
*       a unique name. For your convenience, you can have the host data
*       structure with a user data pointer.
*
*       Please keep the length of the port name down to up to 20 characters,
*       many users prefer short host names. This routine will reject names
*       longer than 80 characters.
*
*       The name is converted to all upper case characters to make it easier
*       to access the host name in ARexx scripts. Lower case names require
*       quoting whereas upper case names do not.
*
*   INPUTS
*       BaseName - Name of message port to create; the name may not
*           contain blank spaces or tabs and will be converted to
*           all upper case letters. The name you provide may be up to
*           80 characters long.
*
*       Index - Project index number; if nonzero will cause a numeric
*           extension to be added to the port name, i.e. for an index
*           number of 1 the name "BASENAME" will become "BASENAME.1".
*
*       CommandTable - Points to the first element of the array in
*           which command names, templates and functions are stored.
*
*       UserData - User data pointer which will be copied into the
*           RexxHost structure.
*
*   RESULT
*       Host - Pointer to RexxHost created or NULL if creation failed.
*           If NULL is returned you can retrieve an error code through
*           dos.library/IoErr.
*
*   SEE ALSO
*       exec.library/AddPort
*       exec.library/CreateMsgPort
*       dos.library/IoErr
*
******************************************************************************
*
*/

static struct RexxHost *
CreateRexxHost(
	CONST_STRPTR		BaseName,
	ULONG			Index,
	const struct CmdFunc *	CommandTable,
	const APTR		UserData)
{
	struct RexxHost * Host;
	UBYTE PortName[90];
	LONG Error;
	LONG Size;
	LONG Len = 0;
	LONG i;

	d1(kprintf(__FUNC__ "/%ld BaseName=<%s>  Index=%ld\n", __LINE__, BaseName, Index);)

	/* No host created yet. */

	Host = NULL;

	/* No error so far. */

	Error = RETURN_OK;

	/* Check for NULL name. */

	if(BaseName == NULL)
		Error = ERROR_REQUIRED_ARG_MISSING;

	/* Check for illegal characters. */

	if (RETURN_OK == Error)
		{
		Len = strlen(BaseName);

		for(i = 0 ; (Error == RETURN_OK) && (i < Len) ; i++)
			{
			if(BaseName[i] == ' ' || BaseName[i] == '\t')
				Error = ERROR_INVALID_COMPONENT_NAME;
			}
		}

		/* Check the name length. */

	if (Error == RETURN_OK)
		{
		if(Len > 80)
			Error = ERROR_OBJECT_TOO_LARGE;
		}

		/* Create the RexxHost data. */

	if (Error == RETURN_OK)
		{
		/* Add the index number if provided. */

		if(Index != 0)
			sprintf((char *)PortName,"%s.%ld",BaseName,(long)Index);
		else
			strcpy((char *)PortName,BaseName);

			/* Convert the port name to all upper case
			 * characters.
			 */

		for(i = 0 ; i < Len ; i++)
			PortName[i] = ToUpper(PortName[i]);

		/* Allocate space for the RexxHost. The name
		 * requires a terminating 0-byte which is included
		 * in the RexxHost structure.
		 */

		Size = sizeof(struct RexxHost) + strlen((char *)PortName);

		Host = (struct RexxHost *)AllocVec(Size,
			MEMF_ANY|MEMF_CLEAR|MEMF_PUBLIC);

			/* Did we get the memory we wanted? */

		if(Host == NULL)
			Error = ERROR_NO_FREE_STORE;
		else
			{
			/* Set up the message port. */

			Host->GlobalPort = CreateMsgPort();

				/* Did we get the port? If the creation
				 * failed we either ran out of free
				 * signal bits or memory.
				 */

			if(Host->GlobalPort == NULL)
				Error = ERROR_NO_FREE_STORE;
			else
				{
				/* Fill in the host data. Since the
				 * port will be searched for often
				 * by the ARexx resident process we
				 * assign it a priority of 1.
				 */

				strcpy((char *)Host->PortName,(char *)PortName);

				Host->GlobalPort->mp_Node.ln_Name =
					(char *)Host->PortName;

				Host->GlobalPort->mp_Node.ln_Pri = 1;

				Host->CommandTable = CommandTable;
				Host->UserData = UserData;
				}
			}
		}

	/* Try to add the message port. */

	if (Error == RETURN_OK)
		{
		/* We need to turn off multitasking for a moment
		 * while we access the global message port list.
		 */

		Forbid();

		/* Check if there already is a message port by
		 * the name we just used.
		 */

		if(FindPort((CONST_STRPTR)Host->PortName))
			Error = ERROR_OBJECT_EXISTS;

		/* If there is no such port yet, add the new
		 * message port to the public list.
		 */

		if (Error == RETURN_OK)
			AddPort(Host->GlobalPort);

		/* Enable multitasking again. */

		Permit();
		}

	d1(kprintf(__FUNC__ "/%ld Host=%08lx\n", __LINE__, Host);)

	/* Did we succeed? */

	if (Error == RETURN_OK)
		return(Host);
	else
		{
		/* Clean up the host data. */

		d1(kprintf(__FUNC__ "/%ld Host=%08lx\n", __LINE__, Host);)

		if (Host != NULL)
			{
			/* Delete the message port. This call
			 * will succeed even if the pointer is
			 * NULL.
			 */

			DeleteMsgPort(Host->GlobalPort);

			/* Free the memory allocated for the
			 * host data.
			 */

			FreeVec(Host);
			}

		d1(kprintf(__FUNC__ "/%ld Host=%08lx\n", __LINE__, Host);)

		/* Return failure. */

		return(NULL);
		}
}

/****** Host/DeleteRexxHost **************************************************
*
*   NAME
*       DeleteRexxHost -- Remove a rexx host port previously created by
*           CreateRexxHost.
*
*   SYNOPSIS
*       DeleteRexxHost(Host)
*
*       VOID DeleteRexxHost(struct RexxHost *);
*
*   FUNCTION
*       Before your application exits, call this routine on every rexx host
*       environment you created with CreateRexxHost(). If you forget to do so
*       your application will leave unused memory and, in case it was
*       run from Shell, signal bits allocated which can never be reclaimed.
*
*   INPUTS
*       Host - Result of CreateRexxHost() call. May be NULL in which case
*           this routine will do nothing.
*
*   RESULT
*       None
*
*   SEE ALSO
*       exec.library/DeleteMsgPort
*       exec.library/RemPort
*       CreateRexxHost
*
******************************************************************************
*
*/

static VOID DeleteRexxHost(struct RexxHost * Host)
{
	/* Did we get a valid host pointer? */

	d1(kprintf(__FUNC__ "/%ld Host=%08lx\n", __LINE__, Host);)

	if (Host != NULL)
		{
		d1(kprintf(__FUNC__ "/%ld GlobalPort=%08lx\n", __LINE__, Host->GlobalPort);)

		if (Host->GlobalPort)
			{
			struct RexxMsg * Message;

			/* Turn off multitasking while we remove the
			 * public message port.
			 */

			Forbid();

			/* Remove the message port. */

			RemPort(Host->GlobalPort);

			/* Now enable multitasking again. */

			Permit();

			/* Remove all pending rexx messages from the
			 * port and reply them to their owners.
			 */

			while((Message = (struct RexxMsg *)GetMsg(Host->GlobalPort))
				!= NULL)
				{
				/* Reject all messages. */

				Message->rm_Result1 = RC_FATAL;

				ReplyMsg((struct Message *)Message);
				}

			/* Delete the message port. */

			DeleteMsgPort(Host->GlobalPort);
			}

		/* Free the memory allocated. */

		FreeVec(Host);
		}
}

/****************************************************************************/

/****** Host/CreateRexxParseData *********************************************
*
*   NAME
*       CreateRexxParseData -- Create working data for command parsing.
*
*   SYNOPSIS
*       Data = CreateRexxParseData()
*
*       struct RexxParseData *CreateRexxParseData(VOID);
*
*   FUNCTION
*       The commands the host receives from the Rexx resident process will
*       need further processing. This is what the command parser will do,
*       but it needs environment data to work with. This routine allocates
*       the work space it needs.
*
*   INPUTS
*       None
*
*   RESULT
*       Data - Pointer to RexxParseData structure or NULL if it could not
*           be allocated. If NULL is returned the error code can be
*           obtained through IoErr().
*
*   SEE ALSO
*       dos.library/AllocDosObject
*       dos.library/IoErr
*
******************************************************************************
*
*/

static struct RexxParseData *CreateRexxParseData(VOID)
{
	struct Process * ThisProcess;
	struct RexxParseData * Data;
	LONG Error;

	/* No error has occured yet. */
	Error = RETURN_OK;

		/* Allocate the basic parser data. */

	Data = AllocVec(sizeof(struct RexxParseData),MEMF_ANY|MEMF_CLEAR);

		/* Did we get the memory? */

	if(Data == NULL)
		Error = ERROR_NO_FREE_STORE;

		/* Allocate the ReadArgs data. */

	if(Error == RETURN_OK)
		{
		/* AllocDosObjectTagList is a dos.library call which
		 * can be made by a Task.
		 */

		Data->ReadArgs = AllocDosObjectTagList(DOS_RDARGS,NULL);

		/* Did we get the data? */

		if(Data->ReadArgs == NULL)
			Error = ERROR_NO_FREE_STORE;
		}

		/* Did we succeed? */

	if(Error == RETURN_OK)
		return(Data);
	else
		{
			/* Clean up the parser data; in this case failure
			 * means either the ReadArgs data or the data
			 * structure itself could not be allocated. Thus,
			 * we only need to take care of the data, a single
			 * call to FreeVec() is sufficient. Since FreeVec()
			 * can be called with a NULL parameter we do not
			 * need to check if the data pointer is valid.
			 */

		FreeVec(Data);

			/* Obtain the control structure for this Process
			 * or Task. We need to set an error code, but the
			 * system call we will make can only be made by
			 * a Process.
			 */

		ThisProcess = (struct Process *)FindTask(NULL);

			/* If a process is making the call, set the
			 * error code so it can later be retrieved
			 * through IoErr().
			 */

		if(ThisProcess->pr_Task.tc_Node.ln_Type == NT_PROCESS)
			SetIoErr(Error);

			/* Return failure. */

		return(NULL);
		}
}

/****** Host/DeleteRexxParseData *********************************************
*
*   NAME
*       DeleteRexxParseData -- Free data allocated by CreateRexxParseData.
*
*   SYNOPSIS
*       DeleteRexxParseData(Data)
*
*       VOID DeleteRexxParseData(struct RexxParseData **);
*
*   FUNCTION
*       Before your program exits it should call this routine to release
*       all the memory allocated for command parsing. Otherwise it will lose
*       memory that can never be reclaimed.
*
*   INPUTS
*       Data - Pointer to RexxParseData structure created by
*           CreateRexxParseData. May be NULL in which case this routine
*           does nothing.
*
*   RESULT
*       None
*
*   SEE ALSO
*       dos.library/FreeDosObject
*
******************************************************************************
*
*/

static VOID DeleteRexxParseData(struct RexxParseData **Data)
{
	/* Did we get a valid data pointer? */

	d1(kprintf(__FUNC__ "/%ld *Data=%08lx\n", __LINE__, *Data);)

	if (*Data != NULL)
		{
		/* Clean up the parser data. */

		d1(kprintf(__FUNC__ "/%ld ReadArgs=%08lx\n", __LINE__, (*Data)->ReadArgs);)

		if ((*Data)->ReadArgs != NULL)
			{
			FreeDosObject(DOS_RDARGS, (*Data)->ReadArgs);
			(*Data)->ReadArgs = NULL;
			}

		/* Free the memory. */

		FreeVec(*Data);
		*Data = NULL;
		}
}

/****** Host/ReturnRexxMsg ***************************************************
*
*   NAME
*       ReturnRexxMsg -- Return a Rexx command message to the sender,
*           including a result code.
*
*   SYNOPSIS
*       ErrorCode = ReturnRexxMsg(Message,Result)
*
*       LONG ReturnRexxMsg(struct RexxMsg *Message,CONST_STRPTR Result);
*
*   FUNCTION
*       Clients addressing the host send special messages carrying the
*       commands and command arguments to the host message port. After
*       processing the commands the host needs to return these messages.
*
*       This routine cannot be used to return error or failure messages
*       to the client. For this purpose use ReturnErrorMsg() instead.
*
*   INPUTS
*       Message - Pointer to RexxMsg received from client.
*
*       Result - Pointer to string that should be transmitted to
*           the client as the command result. This may be NULL in
*           which case no result string will be transmitted.
*
*   RESULT
*       Error - 0 for success, any other value indicates failure.
*
*   SEE ALSO
*       exec.library/ReplyMsg
*       rexxsyslib.library/CreateArgstring
*       ReturnErrorMsg
*
******************************************************************************
*
*/

LONG ReturnRexxMsg(struct RexxMsg * Message,CONST_STRPTR Result)
{
	STRPTR ResultString;
	LONG ErrorCode;

	/* No error has occured yet. */

	ErrorCode = 0;

	/* Set up the RexxMsg to return no error. */

	Message->rm_Result1 = RC_OK;
	Message->rm_Result2 = 0;

	/* Check if the command should return a result. */

	if((Message->rm_Action & RXFF_RESULT) && Result != NULL)
		{
		/* To return the result string we need to make
		 * a copy for ARexx to use.
		 */

		if(ResultString = CreateArgstring((STRPTR) Result,strlen(Result)))
			{
			/* Put the string into the secondary
			 * result field.
			 */

			Message->rm_Result2 = (LONG)ResultString;
			}
		else
			{
				/* No memory available. */

			ErrorCode = ERR10_003;
			}
		}

	/* Reply the message, regardless of the error code. */

	ReplyMsg((struct Message *)Message);

	return(ErrorCode);
}

/****** Host/ReturnErrorMsg **************************************************
*
*   NAME
*       ReturnErrorMsg -- Return a Rexx command message to the sender and
*           indicate that the command failed.
*
*   SYNOPSIS
*       ReturnErrorMsg(Message,PortName,Error)
*
*       VOID ReturnErrorMsg(struct RexxMsg *Message,CONST_STRPTR PortName,
*           LONG Error)
*
*   FUNCTION
*       This routine permits to return a command message sent by a client
*       and set an error code the client to send the message can query.
*       The error code is stored in a variable whose name is made up from
*       the name of the host name and the prefix ".LASTERROR". Thus,
*       for the host name "HOST" the variable becomes "HOST.LASTERROR".
*
*   INPUTS
*       Message - Pointer to RexxMsg received from client.
*
*       PortName - Name of the host the message was sent to.
*
*       Error - Error code to return.
*
*   RESULT
*       None
*
*   SEE ALSO
*       exec.library/ReplyMsg
*       amiga.lib/SetRexxVar
*
******************************************************************************
*
*/

static VOID ReturnErrorMsg(
	struct RexxMsg *	Message,
	CONST_STRPTR		PortName,
	LONG			Error)
{
	BYTE Value[12];

		/* To signal an error the rc_Result1
		 * entry of the RexxMsg needs to be set to
		 * RC_ERROR. Unfortunately, we cannot convey
		 * the more meaningful error code through
		 * this interface which is why we set a
		 * Rexx variable to the error number. The
		 * Rexx script can then take a look at this
		 * variable and decide which further steps
		 * it should take.
		 */

	Message->rm_Result1 = RC_ERROR;

		/* Turn the error number into a string as
		 * ARexx only deals with strings.
		 */

	sprintf((char *)Value,"%ld",(long)Error);

	/* Now set the variable to the error code.
	 * We use the RVI (Rexx Variables Interface)
	 * routine for this purpose.
	 */

	SetRexxVar(Message, "WORKBENCH.LASTERROR", (STRPTR)Value, strlen(Value));

	ReplyMsg((struct Message *)Message);
}

/****** Host/HandleRexxMsg ***************************************************
*
*   NAME
*       HandleRexxMsg -- Process incoming rexx message and invoke the
*           apropriate function handler.
*
*   SYNOPSIS
*       HandleRexxMsg(Host,Data,Message)
*
*       VOID HandleRexxMsg(const struct RexxHost *Host,
*           struct RexxParseData *Data,struct RexxMsg *Message);
*
*   FUNCTION
*       This routine prepares the ground for the final command execution.
*       It tries to find the command the Rexx message should invoke and
*       parses the command arguments. If a matching command could be found
*       and the arguments are correct the corresponding function is invoked.
*       When control has passed through this routine the Rexx message will
*       have been replied.
*
*   INPUTS
*       Host - ARexx Host environment as created by CreateRexxHost
*
*       Data - Parser data as created by CreateParseData
*
*       Message - Pointer to RexxMsg received from client.
*
*   NOTES
*       This routine should only be called by a Process.
*
*   RESULT
*       None
*
*   SEE ALSO
*       dos.library/ReadArgs
*       CreateParseData
*       CreateRexxHost
*
******************************************************************************
*
*/

static VOID HandleRexxMsg(
	const struct RexxHost *	Host,
	struct RexxParseData *	Data,
	struct RexxMsg *	Message)
{
	const struct CmdFunc * Table;
	const struct CmdFunc * CmdFunc;
	STRPTR Command;
	STRPTR Arguments;
	LONG Error;
	LONG Len;
	LONG i;

		/* This is where the entire command string is stored,
		 * including following arguments.
		 */

	Command = (STRPTR)Message->rm_Args[0];

		/* Skip leading blanks. */

	while(*Command == ' ' || *Command == '\t')
		Command++;

		/* Now determine the length of the command. */

	Len = 0;

	while (Command[Len])
		{
		if(Command[Len] == '\t' || Command[Len] == ' ')
			break;
		else
			Len++;
		}

		/* Now we need to check if the command we received
		 * can be handled by one of the routines in the
		 * command table.
		 */

	Table = Host->CommandTable;
	CmdFunc = NULL;

	d1(kprintf(__FUNC__ "/%ld: cmd=<%s>\n", __LINE__, Command);)

	for(i = 0 ; Table[i].CommandName != NULL ; i++)
		{
		if((strlen(Table[i].CommandName) == Len) && (Strnicmp(Command,Table[i].CommandName,Len) == 0))
			{
			CmdFunc = &Table[i];
			break;
			}
		}

	d1(kprintf(__FUNC__ "/%ld: CmdFunc=%08lx\n", __LINE__, CmdFunc);)

		/* If no matching command could be found, return
		 * an error.
		 */

	if(CmdFunc == NULL)
		LastError = Error = ERROR_OBJECT_NOT_FOUND;
	else
		{
			/* Reset the argument pointers. */

		memset(Data->Args,0,sizeof(Data->Args));

			/* Separate the arguments from the command;
			 * first skip the leading blanks.
			 */

		Arguments = &Command[Len];

		while(*Arguments == '\t' || *Arguments == ' ')
			Arguments++;

			/* Remove trailing blanks. */

		Len = strlen(Arguments);

		while((Len > 0) &&
			(Arguments[Len - 1] == '\t' || Arguments[Len - 1] == ' '))
				Len--;

			/* We only allow for 510 characters of argument data. */

		if(Len > 510)
			Len = 510;

		/* Put the arguments into the parser buffer. */

		if(Len > 0)
			CopyMem(Arguments,Data->Line,Len);

		/* Add the line feed and the null-termination. */

		Data->Line[Len] = '\n';
		Data->Line[Len+1] = 0;

		/* Set up the parser to read from the argument buffer. */

		Data->ReadArgs->RDA_Source.CS_Buffer = (STRPTR)Data->Line;
		Data->ReadArgs->RDA_Source.CS_Length = Len + 1;
		Data->ReadArgs->RDA_Source.CS_CurChr = 0;
		Data->ReadArgs->RDA_Flags |= RDAF_NOPROMPT;

			/* Run the arguments through the parser. This will
			 * set up the Data->Args array.
			 */

		if(ReadArgs(CmdFunc->Template,(IPTR *)Data->Args,Data->ReadArgs))
			{
			/* Invoke the command. */

			Error = (*CmdFunc->Function)(Host->UserData,Message,Data->Args);
			if (RETURN_OK != Error)
				LastError = Error;

			d1(kprintf(__FUNC__ "/%ld: LastError=%ld  Error=%ld\n", __LINE__, LastError, Error);)

			/* Free the parser data. */

			FreeArgs(Data->ReadArgs);
			}
		else
			LastError = Error = IoErr();
		}

		/* If an error occured, return the message now.
		 * In any other case the command handler will have
		 * taken care of it already.
		 */

	if (Error)
		ReturnErrorMsg(Message, (STRPTR) Host->PortName,Error);
}

/****************************************************************************/

	/* The following routines implement the 15 mandatory ARexx commands
	 * every ARexx host should support, as per the "Amiga User Interface
	 * Style Guide". With the exception of the FAULT, HELP, QUIT and RX
	 * commands none of the example commands do any real work. They just
	 * print their arguments and return an error code.
	 *
	 * Every routine is invoked with three parameters:
	 *
	 * UserData -- The user data pointer passed to CreateRexxHost.
	 *
	 * Message -- Pointer to the RexxMsg received from the client.
	 *
	 * Args -- Pointer to an array of 32 string pointers, as set up
	 *         by the dos.library/ReadArgs parser.
	 *
	 * Each routine has to return an error code. If the code returned
	 * is nonzero the command dispatcher in HandleRexxMsg() will
	 * reply the RexxMsg. If the code is zero, the routine is
	 * responsible for replying the RexxMsg.
	 */

/****** Host/DoRexxCommand ***************************************************
*
*   NAME
*       DoRexxCommand -- Send a command to the Rexx resident process.
*
*   SYNOPSIS
*       Error = DoRexxCommand(Command)
*
*       LONG DoRexxCommand(CONST_STRPTR Command);
*
*   FUNCTION
*       A command is sent to the Rexx resident process. In this context
*       "command" either means the name of an ARexx script the resident
*       process is to load and execute or a small one-line program.
*
*   INPUTS
*       Command - Name of command to execute.
*
*   RESULT
*       Error - 0 for success, otherwise an error code.
*
*   SEE ALSO
*       rexxsyslib.library/CreateArgstring
*       rexxsyslib.library/DeleteArgstring
*       rexxsyslib.library/CreateRexxMsg
*       rexxsyslib.library/DeleteRexxMsg
*       exec.library/CreateMsgPort
*       exec.library/DeleteMsgPort
*       exec.library/FindPort
*       exec.library/PutMsg
*       exec.library/WaitPort
*
******************************************************************************
*
*/

LONG DoRexxCommand(CONST_STRPTR Command)
{
	struct RexxMsg *RexxMsg;
	LONG Error;

	do	{
		struct MsgPort *RexxPort;
		static CONST_STRPTR ConsoleName = "CON:0/11//189/WBRexx Output Window/AUTO/WAIT";

		/* Build a RexxMsg we can use. */
		RexxMsg = CreateRexxMsg(globalReplyPort,"WB","WORKBENCH");
		if (NULL == RexxMsg)
			{
			Error = ERROR_NO_FREE_STORE;
			break;
			}

		/* The command goes into the first
		 * message argument.
		 */
		RexxMsg->rm_Args[0] = CreateArgstring((STRPTR) Command,strlen(Command));
		if (0 == RexxMsg->rm_Args[0])
			{
			Error = ERROR_NO_FREE_STORE;
			break;
			}

		/* Mark the message as a command. */
		RexxMsg->rm_Action = RXCOMM;

		RexxMsg->rm_Stdin = Open("NIL:", MODE_OLDFILE);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: stdin=%08lx\n", __LINE__, RexxMsg->rm_Stdin));
		if ((BPTR)NULL == RexxMsg->rm_Stdin)
			{
			Error = IoErr();
			break;
			}

		RexxMsg->rm_Stdout = Open((STRPTR) ConsoleName, MODE_NEWFILE);
		d1(kprintf(__FILE__ "/" __FUNC__ "/%ld: stdout=%08lx\n", __LINE__, RexxMsg->rm_Stdout));
		if ((BPTR)NULL == RexxMsg->rm_Stdout)
			{
			Error = IoErr();
			break;
			}

		/* Now try to find the resident process
		 * communications port. We need to
		 * turn off multitasking while we look
		 * for it and send the message.
		 */

		Forbid();

		if (RexxPort = FindPort(RXSDIR))
			{
			PutMsg(RexxPort, &RexxMsg->rm_Node);

			Error = RETURN_OK;
			RexxMsg = NULL;
			}
		else
			Error = ERROR_OBJECT_NOT_FOUND;

		Permit();
		} while (0);

	if (RexxMsg)
		{
		if (RexxMsg->rm_Stdin)
			Close(RexxMsg->rm_Stdin);
		if (RexxMsg->rm_Stdout)
			Close(RexxMsg->rm_Stdout);

		ClearRexxMsg(RexxMsg, 16);

		DeleteRexxMsg(RexxMsg);
		}

	return(Error);
}

	/* RxEntry(VOID):
	 *
	 *	This is the entry point of a process that will get launched
	 *	if the RX command is invoked with the "CONSOLE" parameter.
	 */

SAVEDS(VOID) RxEntry(VOID)
{
	struct Process *ThisProcess;
	BPTR Stream;
	struct ChildMessage *ChildMessage;
	struct RexxMsg *SyncMsg;
	LONG Error;

		/* Obtain current process identifier. */

	ThisProcess = (struct Process *)FindTask(NULL);

		/* Pick up the startup message. */

	WaitPort(&ThisProcess->pr_MsgPort);
	ChildMessage = (struct ChildMessage *)GetMsg(&ThisProcess->pr_MsgPort);

		/* Remember the RexxMsg if any. If nonzero, the RX command
		 * was invoked without the ASYNC parameter and the message
		 * still need to be replied.
		 */

	SyncMsg = ChildMessage->RexxMsg;

	/* Redirect process output to the console window. */

	if(Stream = Open("CONSOLE:",MODE_NEWFILE))
	{
		SelectOutput(Stream);

			/* Send the command to the Rexx process. */

		Error = DoRexxCommand((CONST_STRPTR)ChildMessage->Command);

			/* Close the output stream. */

		SelectOutput((BPTR)NULL);
		Close(Stream);
	}
	else
		Error = IoErr();

		/* Do we need to reply the RexxMsg? */

	if(SyncMsg)
	{
		if(Error)
			ReturnErrorMsg(SyncMsg,ChildMessage->PortName,Error);
		else
			ReturnRexxMsg(SyncMsg,0);
	}

	ReplyMsg((struct Message *) ChildMessage);
}


/* DeleteChildMessage(struct ChildMessage *ChildMessage):
 *
 *	Dispose of a message created by CreateChildMessage().
 */
VOID DeleteChildMessage(struct ChildMessage *ChildMessage)
{
	FreeMem(ChildMessage,ChildMessage->Message.mn_Length);
}


/* CreateChildMessage():
 *
 *	This routine sets up a custom message to send to a
 *	child process which gets launched if the RX command
 *	is invoked with the CONSOLE keyword.
 */
struct ChildMessage *CreateChildMessage(struct MsgPort *ReplyPort,CONST_STRPTR Command,struct RexxMsg *RexxMessage)
{
	UWORD Length;
	struct ChildMessage *ChildMessage;

		/* Make room for the message plus the command string. */

	Length = sizeof(struct ChildMessage) + strlen(Command);

	ChildMessage = (struct ChildMessage *)AllocMem(Length,MEMF_CLEAR|MEMF_PUBLIC);

	if(ChildMessage != NULL)
	{
			/* Initialize the message. */

		ChildMessage->Message.mn_Length = Length;
		ChildMessage->Message.mn_ReplyPort = ReplyPort;

		strcpy((char *)ChildMessage->Command,Command);
	}

	return(ChildMessage);
}

/****************************************************************************/

	/* This is the table which lists the single commands this ARexx
	 * host implements. Each entry lists the name of the command to
	 * invoke, the argument template and the pointer to the routine
	 * which implements the command. The table is terminated by a
	 * NULL command name.
	 */

struct CmdFunc CommandTable[] =
{
	{ "ACTIVATEWINDOW",	  "WINDOW",						Cmd_ActivateWindow },
	{ "CHANGEWINDOW",	  "WINDOW,LEFTEDGE/N,TOPEDGE/N,WIDTH/N,HEIGHT/N",	Cmd_ChangeWindow },
	{ "DELETE",		  "NAME/A,ALL/S",					Cmd_Delete },
	{ "FAULT",		  "CODE/A/N",						Cmd_Fault },
	{ "GETATTR",		  "OBJECT/A,NAME/K,STEM/K,VAR/K",			Cmd_GetAttr },
	{ "HELP",		  "COMMAND/K,MENUS/S,PROMPT/S",				Cmd_Help },
	{ "ICON",		  "WINDOW,NAMES/M,OPEN/S,MAKEVISIBLE/S,"
				"SELECT/S,UNSELECT/S,UP/N,DOWN/N,LEFT/N,"
				"RIGHT/N,X/N,Y/N,ACTIVATE/K,CYCLE/K,MOVE/K",		Cmd_Icon },
	{ "INFO",		  "NAME/A",						Cmd_Info },
	{ "KEYBOARD",		  "NAME/A,ADD/S,REMOVE/S,KEY,CMD/F",			Cmd_Keyboard },
	{ "LOCKGUI",		  "",							Cmd_LockGUI },
	{ "MENU",		  "WINDOW/K,INVOKE,NAME/K,TITLE/K,"
				"SHORTCUT/K,ADD/S,REMOVE/S,CMD/K/F",			Cmd_Menu },
	{ "MOVEWINDOW",		  "WINDOW,LEFTEDGE/N,TOPEDGE/N",			Cmd_MoveWindow },
	{ "NEWDRAWER",		  "NAME/A",						Cmd_NewDrawer },
	{ "RENAME",		  "OLDNAME/A,NEWNAME/A",				Cmd_Rename },
	{ "RX",			  "CONSOLE/S,ASYNC/S,CMD/A/F",				Cmd_Rx },
	{ "SIZEWINDOW",		  "WINDOW,WIDTH/N,HEIGHT/N",				Cmd_SizeWindow },
	{ "UNLOCKGUI",		  "",							Cmd_UnlockGUI },
	{ "UNZOOMWINDOW",	  "WINDOW",						Cmd_UnzoomWindow },
	{ "VIEW",		  "WINDOW,PAGE/S,PIXEL/S,UP/S,DOWN/S,"
				"LEFT/S,RIGHT/S",					Cmd_View },
	{ "WINDOW",		  "WINDOWS/A/M,OPEN/S,CLOSE/S,SNAPSHOT/S,"
				"ACTIVATE/S,MIN/S,MAX/S,FRONT/S,"
				"BACK/S,CYCLE/K",					Cmd_Window },
	{ "WINDOWTOBACK",	  "WINDOW",						Cmd_WindowToBack },
	{ "WINDOWTOFRONT",	  "WINDOW",						Cmd_WindowToFront },
	{ "ZOOMWINDOW",		  "WINDOW",						Cmd_ZoomWindow },

	{ NULL,			NULL,							NULL }
};



/* ====================================================== */

static struct Task *HandshakeTask = NULL;
static ULONG HandshakeSignal = -1;
static struct Process *WBRexxProc = NULL;	// the Rexx Handler Process
struct MsgPort *WBRexxPort = NULL;
static BOOL WBRexxProcessRunning = FALSE;


BOOL StartWBRexxProcess(void)
{
	ULONG EmulationMode = TRUE;

	NewList(&MenuItemList);
	NewList(&KbdCommandList);

	InitSemaphore(&MenuItemListSemaphore);
	InitSemaphore(&KbdCommandListSemaphore);

	if (ScalosBase->scb_LibNode.lib_Version >= 40)
		{
		SCA_ScalosControl(NULL,
			SCALOSCTRLA_GetEmulationMode, (ULONG) &EmulationMode,
			TAG_END);
		}

	if (EmulationMode)
		{
		STATIC_PATCHFUNC(WBRexxProcess);

		HandshakeTask = FindTask(NULL);

		HandshakeSignal = AllocSignal(-1);
		if (-1 == HandshakeSignal)
			{
//			alarm("AllocSignal() failed");
			return FALSE;
			}

		WBRexxProc = CreateNewProcTags(NP_Name, (ULONG) "WB39 Rexx Handler",
				NP_Priority, 0,
				NP_Entry, (ULONG) PATCH_NEWFUNC(WBRexxProcess),
				NP_StackSize, 16384,
				TAG_END);
		if (WBRexxProc == NULL)
			{
//			alarm("TNC_INit: CreateProc() failed");
			return FALSE;
			}

		Wait(1 << HandshakeSignal);		/* Warten auf Ergebnis */

		FreeSignal(HandshakeSignal);
		HandshakeSignal = -1;

		if (!WBRexxProcessRunning)
			WBRexxProc = NULL;

		return WBRexxProcessRunning;
		}

	return TRUE;
}


void ShutdownWBRexxProcess(void)
{
	if (NULL == WBRexxProc)
		return;

	HandshakeTask = FindTask(NULL);
	HandshakeSignal = AllocSignal(-1);
	if (-1 == HandshakeSignal)
		{
//		alarm("AllocSignal() failed");
		return;
		}

	WBRexxProcessRunning = FALSE;

	Signal(&WBRexxProc->pr_Task, SIGBREAKF_CTRL_C);	// Signal WBRexxProc to terminate

	Wait(1 << HandshakeSignal);		/* Warten auf Ergebnis */

	FreeSignal(HandshakeSignal);
	HandshakeSignal = -1;

	WBRexxProc = NULL;
}


/* ====================================================== */

static SAVEDS(void) WBRexxProcess(void)
{
	struct LocalData LocalData;

	d1(kprintf(__FUNC__ "/%ld Start WBRexx Process\n", __LINE__);)

	memset(&LocalData, 0, sizeof(LocalData));

	if (RETURN_OK == WBRexxProcessInit(&LocalData))
		{
		ULONG GlobalPortMask = 1L << LocalData.Host->GlobalPort->mp_SigBit;
		ULONG ToolMenuPortMask = 1L << ToolMenuMsgPort->mp_SigBit;
		ULONG ReplyPortMask = 1L << globalReplyPort->mp_SigBit;

		WBRexxProcessRunning = TRUE;

		if (HandshakeSignal != -1)
			Signal(HandshakeTask, 1 << HandshakeSignal);

		/* Set up the local data. This must be done
		 * before the first Rexx command is processed.
		 */

		LocalData.CommandTable	= CommandTable;
		LocalData.Usage		= 0;
		LocalData.GlobalPort	= LocalData.Host->GlobalPort;
		LocalData.PortName	= (STRPTR)LocalData.Host->PortName;
		LocalData.Terminate	= FALSE;

		d1(kprintf(__FUNC__ "/%ld Ok  Hostname=<%s>\n", __LINE__, LocalData.Host->PortName);)
//		Printf("Host name is \"%s\", waiting for commands.\n", LocalData.Host->PortName);

		/* Loop until either the QUIT command is executed or
		 * Ctrl-C is pressed.
		 */

		do
			{
			ULONG Signals;
			/* Wait for Ctrl-C or an incoming Message. */

			Signals = Wait(SIGBREAKF_CTRL_C | GlobalPortMask | ToolMenuPortMask | ReplyPortMask);

			/* Ctrl-C signal received? */
			if(Signals & SIGBREAKF_CTRL_C)
				LocalData.Terminate = TRUE;

			/* Message received? */
			if(Signals & GlobalPortMask)
				{
				struct RexxMsg * RexxMessage;
					/* Process all incoming messages. */

				while((RexxMessage = (struct RexxMsg *)GetMsg(LocalData.Host->GlobalPort)) != NULL)
					{
					/* If the incoming message is indeed
					 * a RexxMsg, process the command
					 */

					if(IsRexxMsg(RexxMessage))
						HandleRexxMsg(LocalData.Host, LocalData.ParseData, RexxMessage);
					else
						{
						/* Otherwise it is a startup message
						 * sent to a background process. We
						 * need to delete this message and
						 * decrement the number of outstanding
						 * startup messages.
						 */

						DeleteChildMessage((struct ChildMessage *)RexxMessage);

						LocalData.Usage--;
						}
					}
				}
			if (Signals & ToolMenuPortMask)
				{
				struct AppMessage *aMsg;

				while (aMsg = (struct AppMessage *)GetMsg(ToolMenuMsgPort))
					{
					struct ToolMenuItem *tmItem;

					d1(kprintf(__FUNC__ "/%ld Msg=%08lx  Type=%ld  ID=%08lx  UserData=%08lx\n", \
						__LINE__, aMsg, aMsg->am_Type, aMsg->am_ID, aMsg->am_UserData);)

					tmItem = (struct ToolMenuItem *) aMsg->am_UserData;

					d1(kprintf(__FUNC__ "/%ld Cmd=<%s>\n", __LINE__, tmItem->tmi_Cmd);)

					// execute command string
					if (tmItem->tmi_Cmd)
						DoRexxCommand(tmItem->tmi_Cmd);

					ReplyMsg(&aMsg->am_Message);
					}
				}
			if (Signals & ReplyPortMask)
				{
				struct RexxMsg *rMsg;

				while (rMsg = (struct RexxMsg *)GetMsg(globalReplyPort))
					{
					ClearRexxMsg(rMsg, 1);

					if (rMsg->rm_Stdin)
						Close(rMsg->rm_Stdin);

					if (rMsg->rm_Stdout && RETURN_OK == rMsg->rm_Result1)
						{
						// only close Stdout if no error !
						Close(rMsg->rm_Stdout);
						}

					DeleteRexxMsg(rMsg);
					}
				}

			/* The loop will be terminated when the "Terminate" flag changes
			 * to TRUE and no further startup messages are outstanding.
			 */
			}

		while(LocalData.Usage > 0 || LocalData.Terminate == FALSE);
		}

	d1(kprintf(__FUNC__ "/%ld \n", __LINE__);)

	DeleteRexxHost(LocalData.Host);

	d1(kprintf(__FUNC__ "/%ld \n", __LINE__);)

	DeleteRexxParseData(&LocalData.ParseData);

#ifdef __amigaos4__
	if (IRexxSys)
		{
		DropInterface((struct Interface *)IRexxSys);
		IRexxSys = NULL;
		}
#endif
	if (RexxSysBase)
		{
		CloseLibrary((struct Library *) RexxSysBase);
		RexxSysBase = NULL;
		}

	if (ToolMenuMsgPort)
		{
		DeleteMsgPort(ToolMenuMsgPort);
		ToolMenuMsgPort = NULL;
		}
	if (globalReplyPort)
		{
		DeleteMsgPort(globalReplyPort);
		globalReplyPort = NULL;
		}

	d1(kprintf(__FUNC__ "/%ld \n", __LINE__);)

	WBRexxProcessRunning = FALSE;

	Forbid();
	if (HandshakeSignal != -1)
		Signal(HandshakeTask, 1 << HandshakeSignal);
}


static int WBRexxProcessInit(struct LocalData *LocalData)
{
	struct Process *myProc = (struct Process *) FindTask(NULL);

	/* Open the rexx system library. */
	RexxSysBase = (T_REXXSYSBASE) OpenLibrary(RXSNAME,0);

	myProc->pr_WindowPtr = NULL;

	ToolMenuMsgPort = CreateMsgPort();
	if (NULL == ToolMenuMsgPort)
		{
//		Printf("Could not create tool menu msgport\n");

		return(RETURN_FAIL);
		}

	globalReplyPort = CreateMsgPort();
	if (NULL == globalReplyPort)
		{
//		Printf("Could not create global reply msgport\n");

		return(RETURN_FAIL);
		}

	d1(kprintf(__FUNC__ "/%ld RexxSysBase=%08lx\n", __LINE__, RexxSysBase);)
	if (RexxSysBase == NULL)
		{
//		Printf("Could not open %s.\n",RXSNAME);

		return(RETURN_FAIL);
		}
#ifdef __amigaos4__
	IRexxSys = (struct RexxSysIFace *)GetInterface((struct Library *)RexxSysBase, "main", 1, NULL);
	if (NULL == IRexxSys)
		{
//		Printf("Could not get RexxSys' OS4 Interface");

		return(RETURN_FAIL);
		}
#endif

		/* Create the command parser data. */

	LocalData->ParseData = CreateRexxParseData();

	d1(kprintf(__FUNC__ "/%ld ParseData=%08lx\n", __LINE__, LocalData->ParseData);)
	if (LocalData->ParseData == NULL)
		{
//		Printf("Error creating rexx parse data.\n");

#ifdef __amigaos4__
		DropInterface((struct Interface *)IRexxSys);
		IRexxSys = NULL;
#endif
		CloseLibrary((struct Library *) RexxSysBase);
		RexxSysBase = NULL;

		return(RETURN_FAIL);
		}

	/* Finally create the Rexx host. */

	LocalData->Host = CreateRexxHost("WORKBENCH", 0, CommandTable,LocalData);

	d1(kprintf(__FUNC__ "/%ld Host=%08lx\n", __LINE__, LocalData->Host);)
	if (LocalData->Host == NULL)
		{
//		Printf("Error creating rexx host.\n");

		DeleteRexxParseData(&LocalData->ParseData);

#ifdef __amigaos4__
		DropInterface((struct Interface *)IRexxSys);
		IRexxSys = NULL;
#endif
		CloseLibrary((struct Library *) RexxSysBase);
		RexxSysBase = NULL;

		return(RETURN_FAIL);
		}

	return RETURN_OK;
}


// Split Argument into "."-delimited parts
CONST_STRPTR GetNextArgPart(CONST_STRPTR ArgSrc, STRPTR Buffer, size_t MaxLen)
{
	if ('.' == *ArgSrc)
		ArgSrc++;

	while (MaxLen > 1 && *ArgSrc && '.' != *ArgSrc)
		{
		*Buffer++ = *ArgSrc++;
		MaxLen--;
		}
	*Buffer = '\0';

	return ArgSrc;
}


ULONG AddMenuItem(CONST_STRPTR Name, CONST_STRPTR Title, CONST_STRPTR Cmd, CONST_STRPTR Shortcut)
{
	ULONG Result = RETURN_OK;
	struct ToolMenuItem *tmi;
	size_t len;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s> Title=<%s> Cmd=<%s>  Shortcut=<%s>\n", \
		__LINE__, Name, Title, Cmd, Shortcut);)

	ObtainSemaphore(&MenuItemListSemaphore);

	do	{
		for (tmi = (struct ToolMenuItem *) MenuItemList.lh_Head;
			tmi != (struct ToolMenuItem *) &MenuItemList.lh_Tail;
			tmi = (struct ToolMenuItem *) tmi->tmi_Node.ln_Succ)
			{
			if (0 == Stricmp((STRPTR) Name, tmi->tmi_Name))
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_OBJECT_EXISTS\n", __LINE__);)
				Result = ERROR_OBJECT_EXISTS;
				tmi = NULL;
				break;
				}
			}
		if (RETURN_OK != Result)
			break;

		tmi = AllocVec(sizeof(struct ToolMenuItem), MEMF_PUBLIC | MEMF_CLEAR);
		if (NULL == tmi)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		tmi->tmi_wbKey = NULL;
		tmi->tmi_wbTitleKey = NULL;

		tmi->tmi_UseCount = 1;

		tmi->tmi_Name = StringDup(Name);
		tmi->tmi_Cmd = StringDup(Cmd);
		tmi->tmi_Shortcut = StringDup(Shortcut);

		if ('\\' == *Title && '\\' != Title[1])
			{
			Title++;	// Skip "\"

			len = 1 + strlen(Title);
			tmi->tmi_MenuTitle = AllocVec(len, MEMF_PUBLIC);
			d1(kprintf(__FUNC__ "/%ld: tmi_MenuTitle=%08lx\n", __LINE__, tmi->tmi_MenuTitle);)
			if (NULL == tmi->tmi_MenuTitle)
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
		 		Result = ERROR_NO_FREE_STORE;
		 		break;
				}

			Title = CopyMenuTitle(tmi->tmi_MenuTitle, Title, len);

			d1(kprintf(__FUNC__ "/%ld: tmi_MenuTitle=<%s>  Title=<%s>\n", __LINE__, tmi->tmi_MenuTitle, Title);)
			}
		else
			{
			tmi->tmi_MenuTitle = NULL;
			}


		len = 1 + strlen(Title);
		tmi->tmi_Title = AllocVec(len, MEMF_PUBLIC);
		if (NULL == tmi->tmi_Title)
			{
			d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
	 		Result = ERROR_NO_FREE_STORE;
	 		break;
			}

		Title = CopyMenuTitle(tmi->tmi_Title, Title, len);

		d1(kprintf(__FUNC__ "/%ld: tmi_Title=<%s>  Title=<%s>\n", __LINE__, tmi->tmi_Title, Title);)

		if (*Title)
			{
			len = 1 + strlen(Title);
			tmi->tmi_SubItem = AllocVec(len, MEMF_PUBLIC);
			if (NULL == tmi->tmi_SubItem)
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
		 		Result = ERROR_NO_FREE_STORE;
		 		break;
				}

			CopyMenuTitle(tmi->tmi_SubItem, Title, len);

			d1(kprintf(__FUNC__ "/%ld: tmi_SubItem=<%s>\n", __LINE__, tmi->tmi_SubItem);)
			}
		else
			tmi->tmi_SubItem = NULL;

		if (NULL == tmi->tmi_Name || NULL == tmi->tmi_Title 
			|| NULL == tmi->tmi_Cmd || NULL == tmi->tmi_Shortcut)
			{
			d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		tmi->tmi_Parent = FindParentMenuItem(tmi);

		if (tmi->tmi_MenuTitle && NULL == tmi->tmi_wbTitleKey)
			{
			tmi->tmi_MenuAppObject = SCA_NewAddAppMenuItemTags(0, (ULONG) tmi,
				tmi->tmi_MenuTitle, ToolMenuMsgPort,
				WBAPPMENUA_GetTitleKey, (ULONG) &tmi->tmi_wbTitleKey,
				TAG_END);

			if (NULL == tmi->tmi_MenuAppObject)
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
				Result = ERROR_NO_FREE_STORE;
				break;
				}

			d1(kprintf(__FUNC__ "/%ld: tmi_wbTitleKey=%08lx\n", __LINE__, tmi->tmi_wbTitleKey);)
			}

		if (NULL == tmi->tmi_SubItem || NULL == tmi->tmi_wbKey)
			{
			tmi->tmi_ItemAppObject = SCA_NewAddAppMenuItemTags(0, (ULONG) tmi,
				tmi->tmi_Title, ToolMenuMsgPort,
				tmi->tmi_SubItem ? TAG_IGNORE : WBAPPMENUA_CommandKeyString, (ULONG) Shortcut,
				WBAPPMENUA_UseKey, (ULONG) tmi->tmi_wbTitleKey,
				tmi->tmi_SubItem ? WBAPPMENUA_GetKey : TAG_IGNORE, (ULONG) &tmi->tmi_wbKey,
				TAG_END);

			if (NULL == tmi->tmi_ItemAppObject)
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
				Result = ERROR_NO_FREE_STORE;
				break;
				}
			}

		d1(kprintf(__FUNC__ "/%ld: tmi_wbKey=%08lx\n", __LINE__, tmi->tmi_wbKey);)

		if (tmi->tmi_SubItem)
			{
			tmi->tmi_SubAppObject = SCA_NewAddAppMenuItemTags(0, (ULONG) tmi,
					tmi->tmi_SubItem, ToolMenuMsgPort,
					WBAPPMENUA_CommandKeyString, (ULONG) Shortcut,
					WBAPPMENUA_UseKey, (ULONG) tmi->tmi_wbKey,
					TAG_END);

			if (NULL == tmi->tmi_SubAppObject)
				{
				d1(kprintf(__FUNC__ "/%ld: ERROR_NO_FREE_STORE\n", __LINE__);)
				Result = ERROR_NO_FREE_STORE;
				break;
				}
			}

		AddTail(&MenuItemList, &tmi->tmi_Node);
		} while (0);

	if (RETURN_OK != Result && tmi)
		DeleteToolMenuItem(tmi);

	ReleaseSemaphore(&MenuItemListSemaphore);

	return Result;
}


ULONG RemoveMenuItem(CONST_STRPTR Name)
{
	struct ToolMenuItem *tmi;
	ULONG Result =  ERROR_OBJECT_NOT_FOUND;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Name);)

	ObtainSemaphore(&MenuItemListSemaphore);

	for (tmi = (struct ToolMenuItem *) MenuItemList.lh_Head;
		tmi != (struct ToolMenuItem *) &MenuItemList.lh_Tail;
		tmi = (struct ToolMenuItem *) tmi->tmi_Node.ln_Succ)
		{
		if (0 == Stricmp((STRPTR) Name, tmi->tmi_Name))
			{
			d1(kprintf(__FUNC__ "/%ld: FOUND tmi=%08lx\n", __LINE__, tmi);)

			if (tmi->tmi_Parent)
				{
				tmi->tmi_Parent->tmi_UseCount--;

				if (0 == tmi->tmi_Parent->tmi_UseCount)
					{
					Remove(&tmi->tmi_Parent->tmi_Node);
					DeleteToolMenuItem(tmi->tmi_Parent);
					}
				}

			tmi->tmi_UseCount--;

			if (0 == tmi->tmi_UseCount)
				{
				Remove(&tmi->tmi_Node);
				DeleteToolMenuItem(tmi);
				}

			Result = RETURN_OK;
			break;
			}
		}

	ReleaseSemaphore(&MenuItemListSemaphore);

	return Result;
}


static void DeleteToolMenuItem(struct ToolMenuItem *oldItem)
{
	if (oldItem)
		{
		if (oldItem->tmi_SubAppObject)
			SCA_RemoveAppObject(oldItem->tmi_SubAppObject);
		if (oldItem->tmi_ItemAppObject)
			SCA_RemoveAppObject(oldItem->tmi_ItemAppObject);
		if (oldItem->tmi_MenuAppObject)
			SCA_RemoveAppObject(oldItem->tmi_MenuAppObject);

		if (oldItem->tmi_Name)
			FreeVec(oldItem->tmi_Name);
		if (oldItem->tmi_Title)
			FreeVec(oldItem->tmi_Title);
		if (oldItem->tmi_Cmd)
			FreeVec(oldItem->tmi_Cmd);
		if (oldItem->tmi_Shortcut)
			FreeVec(oldItem->tmi_Shortcut);
		if (oldItem->tmi_MenuTitle)
			FreeVec(oldItem->tmi_MenuTitle);
		if (oldItem->tmi_SubItem)
			FreeVec(oldItem->tmi_SubItem);

		FreeVec(oldItem);
		}
}


static STRPTR StringDup(CONST_STRPTR str)
{
	STRPTR copy;

	if (str)
		{
		copy = AllocVec(1 + strlen(str), MEMF_PUBLIC);
		if (copy)
			strcpy(copy, str);
		}
	else
		copy = AllocVec(1, MEMF_PUBLIC | MEMF_CLEAR);

	return copy;
}


ULONG AddKeyboardCommand(CONST_STRPTR Name, CONST_STRPTR Key, CONST_STRPTR Cmd)
{
	struct KeyboardCommand *kbc;
	ULONG Result = RETURN_OK;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>  Cmd=<%s>  Key=<%s>\n", __LINE__, Name, Cmd, Key);)

	ObtainSemaphore(&KbdCommandListSemaphore);

	do	{
		for (kbc = (struct KeyboardCommand *) KbdCommandList.lh_Head;
			kbc != (struct KeyboardCommand *) &KbdCommandList.lh_Tail;
			kbc = (struct KeyboardCommand *) kbc->kbc_Node.ln_Succ)
			{
			d1(kprintf(__FUNC__ "/%ld: Name=<%s>  kbc=%08lx  kbc_Name=<%s> Next=%08lx\n", \
				__LINE__, Name, kbc, kbc->kbc_Name, kbc->kbc_Node.ln_Succ);)

			if (0 == Stricmp((STRPTR) Name, kbc->kbc_Name))
				{
				Result = ERROR_OBJECT_EXISTS;
				kbc = NULL;
				break;
				}
			}

		d1(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

		if (RETURN_OK != Result)
			break;

		kbc = AllocVec(sizeof(struct KeyboardCommand), MEMF_PUBLIC | MEMF_CLEAR);
		d1(kprintf(__FUNC__ "/%ld: kbc=%08lx\n", __LINE__, kbc);)
		if (NULL == kbc)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		kbc->kbc_IX.ix_Version = IX_VERSION;
		kbc->kbc_Name = StringDup(Name);
		kbc->kbc_Cmd = StringDup(Cmd);
		kbc->kbc_Key = StringDup(Key);

		if (NULL == kbc->kbc_Name || NULL == kbc->kbc_Cmd || NULL == kbc->kbc_Key)
			{
			Result = ERROR_NO_FREE_STORE;
			break;
			}

		d1(kprintf(__FUNC__ "/%ld: \n", __LINE__);)

		if (0 != ParseIX((STRPTR) Key, &kbc->kbc_IX))
			{
			d1(kprintf(__FUNC__ "/%ld: ParseIX() failed.\n", __LINE__);)
			Result = ERROR_BAD_TEMPLATE;
			break;
			}

		d1(kprintf(__FUNC__ "/%ld: \n", __LINE__);)

		AddTail(&KbdCommandList, &kbc->kbc_Node);
		} while (0);

	d1(kprintf(__FUNC__ "/%ld: \n", __LINE__);)

	if (RETURN_OK != Result && kbc)
		DeleteKbdCommand(kbc);

	ReleaseSemaphore(&KbdCommandListSemaphore);

	return RETURN_OK;
}


ULONG RemoveKeyboardCommand(CONST_STRPTR Name)
{
	ULONG Result = ERROR_OBJECT_NOT_FOUND;
	struct KeyboardCommand *kbc;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Name);)

	ObtainSemaphore(&KbdCommandListSemaphore);

	for (kbc = (struct KeyboardCommand *) KbdCommandList.lh_Head;
		kbc != (struct KeyboardCommand *) &KbdCommandList.lh_Tail;
		kbc = (struct KeyboardCommand *) kbc->kbc_Node.ln_Succ)
		{
		if (0 == Stricmp((STRPTR) Name, kbc->kbc_Name))
			{
			d1(kprintf(__FUNC__ "/%ld: Found  kbc=%08lx\n", __LINE__, kbc);)

			Remove(&kbc->kbc_Node);

			DeleteKbdCommand(kbc);

			Result = RETURN_OK;
			break;
			}
		}

	ReleaseSemaphore(&KbdCommandListSemaphore);

	return Result;
}


BOOL DoKeyboardCommand(struct IntuiMessage *iMsg)
{
	struct InputEvent ie;
	struct KeyboardCommand *kbc;
	BOOL Success = FALSE;

	memset(&ie, 0, sizeof(ie));

	ie.ie_Class = IECLASS_RAWKEY;
	ie.ie_SubClass = IESUBCLASS_COMPATIBLE;
	ie.ie_Code = iMsg->Code;
	ie.ie_Qualifier = iMsg->Qualifier;
	ie.ie_TimeStamp.tv_secs = iMsg->Seconds;
	ie.ie_TimeStamp.tv_micro = iMsg->Micros;

	d1(kprintf(__FUNC__ "/%ld: Code=%08lx  Qualifier=%08lx\n", __LINE__, ie.ie_Code, ie.ie_Qualifier);)

	ObtainSemaphoreShared(&KbdCommandListSemaphore);

	for (kbc = (struct KeyboardCommand *) KbdCommandList.lh_Head;
		kbc != (struct KeyboardCommand *) &KbdCommandList.lh_Tail;
		kbc = (struct KeyboardCommand *) kbc->kbc_Node.ln_Succ)
		{
		if (MatchIX(&ie, &kbc->kbc_IX))
			{
			d1(kprintf(__FUNC__ "/%ld: FOUND Name=<%s>  Cmd=<%s>\n", __LINE__, kbc->kbc_Name, kbc->kbc_Cmd);)
			DoRexxCommand(kbc->kbc_Cmd);

			Success = TRUE;
			break;
			}
		}

	ReleaseSemaphore(&KbdCommandListSemaphore);

	return Success;
}


static void DeleteKbdCommand(struct KeyboardCommand *kbc)
{
	if (kbc)
		{
		if (kbc->kbc_Name)
			FreeVec(kbc->kbc_Name);
		if (kbc->kbc_Cmd)
			FreeVec(kbc->kbc_Cmd);
		if (kbc->kbc_Key)
			FreeVec(kbc->kbc_Key);

		FreeVec(kbc);
		}
}


CONST_STRPTR CopyMenuTitle(STRPTR Dest, CONST_STRPTR Src, size_t MaxLen)
{
	while (*Src && MaxLen > 1)
		{
		if ('\\' == *Src)
			{
			Src++;
			if ('\\' == *Src)
				*Dest++ = *Src++;	// copy "\\" as "\"
			else
				{
				break;
				}
			}
		else
			*Dest++ = *Src++;

		MaxLen--;
		}

	*Dest = '\0';

	if ('\\' == *Src)
		Src++;	// skip "\" between menu Src and item Src

	return Src;
}


static struct ToolMenuItem *FindParentMenuItem(struct ToolMenuItem *tmi)
{
	struct ToolMenuItem *tmiParent;

	for (tmiParent = (struct ToolMenuItem *) MenuItemList.lh_Head;
		tmiParent != (struct ToolMenuItem *) &MenuItemList.lh_Tail;
		tmiParent = (struct ToolMenuItem *) tmiParent->tmi_Node.ln_Succ)
		{
		if (tmi->tmi_MenuTitle)
			{
			if (tmiParent->tmi_MenuTitle && 0 == Stricmp(tmiParent->tmi_MenuTitle, tmi->tmi_MenuTitle))
				{
				tmiParent->tmi_UseCount++;
				tmi->tmi_wbTitleKey = tmiParent->tmi_wbTitleKey;
				tmi->tmi_wbKey = tmiParent->tmi_wbKey;

				return tmiParent;
				}
			}
		else
			{
			if (tmi->tmi_SubItem && tmiParent->tmi_SubItem
				&& 0 == Stricmp(tmiParent->tmi_Title, tmi->tmi_Title))
				{
				tmiParent->tmi_UseCount++;
				tmi->tmi_wbTitleKey = tmiParent->tmi_wbTitleKey;
				tmi->tmi_wbKey = tmiParent->tmi_wbKey;

				return tmiParent;
				}
			}
		}

	return NULL;
}

