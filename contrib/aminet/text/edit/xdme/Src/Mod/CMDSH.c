
#define CMDSH_C 1

#define ACTION_FORCE 2001

/******************************************************************************

    MODULE
	CMDSH.c

    DESCRIPTION
	COMmand SHell for XDME

    NOTES

    BUGS
	the "AbortPkt()" call does obviously not work, so the
	Command Window should better be closed with a request out
	of itself;
	we are currently using ACTION_FORCE to satify our own
	ACTION_READ-request, but that is a hack, since that action
	is not documented in includes/dos/#?.h

    TODO
	*fix bugs - which ?
	*use win/format_string to generate Prompt - but that function is currently static in win.c =8-((((
	*we should better use ACTION_WAIT_FOR_CHAR instead of the
	ACTION_READ so we can control timeouts in order to update
	the Prompt and to have a sure close-possibility.

    EXAMPLES

    SEE ALSO

    INDEX

    HISTORY
	13-09-94 b_noll created
	14-09-94 b_noll command documentation, added Prompt
	21-09-94 b_noll added DEFCMD/DEFHELP-support

******************************************************************************/


/**************************************
	      Includes
**************************************/
#include <proto/exec.h>
#include <proto/dos.h>
/*
#include <exec/ports.h>
#include <exec/memory.h>
#include <utility/tagitem.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
*/

#if 0
#include <exec/initializers.h>
#undef	offsetof
#define offsetof(x,y) OFFSET(x,y)
#else
#include <stddef.h>
#endif

#include "defs.h"

/**************************************
	    Global Variables
**************************************/

Prototype ULONG CMDSH_SigMask;
Prototype UBYTE*CMDSH_FileName;
Prototype UBYTE*CMDSH_Prompt;
//Prototype UBYTE CMDSH_Active;
//Prototype UBYTE CMDSH_ErrorsOut;
//Prototype UBYTE CMDSH_WarningsOut;

/**************************************
      Internal Defines & Structures
**************************************/

#define CMDSH_OK	  0
#define CMDSH_NOFILE	 -1
#define CMDSH_NOTFOUND	 -2
#define CMDSH_FILEOPEN	 -3
#define CMDSH_NOPORT	 -4
#define CMDSH_NOTINTER	 -5
/* #define CMDSH_ */

#define CMDSH_BufferSize  MAXLINELEN
#define CMDSH_CloseMsg	  "\nCommandShell is Closing!\n\f"

#define DEF_FILENAME "CON:0/11/640/60/XDME Command Shell/Close"
#define DEF_PROMPT   "`XDME> "

/**************************************
	   Internal Variables
**************************************/


/* #undef  Local */
/* #define Local */

Local struct MsgPort	*CMDSH_Port	    = NULL;	/* Our ReplyMessagePort */
Local UBYTE		*CMDSH_Buffer	    = 0;	/* Needed for the ASynch IO */
Local struct DosPacket	*CMDSH_Packet	    = NULL;	/* Needed for the ASynch IO */
Local BPTR		 CMDSH_File	    = 0L;	/* The file created by _Open() */
Local UBYTE		 CMDSH_Pending	    = 0;	/*	    Info about A Message is sent   */
//	UBYTE		   CMDSH_Active       = 0;	  /* External info about A CmdShell is open  */
//	UBYTE		   CMDSH_ErrorsOut    = 0;	  /* External_only! control if errors	shall be displayed on cmdshell */
//	UBYTE		   CMDSH_WarningsOut  = 0;	  /* External_only! control if warnings shall be displayed on cmdshell */
//    UBYTE		 CMDSH_LoopUpdate   = 0;	/* Request to update the Prompt  */
      ULONG		 CMDSH_SigMask	    = 0;	/* External info about 1<<_Port->mp_SigBit */
      UBYTE		*CMDSH_FileName     = NULL;	/*	    Info about which file to use   */
      UBYTE		*CMDSH_Prompt	    = NULL;	/*	    Info about the File's prompt   */


Local struct Message	*CMDSH_Message	   = NULL; /* Shortcut to do2msg(CMDSH_Packet) */
Local struct FileHandle *CMDSH_FileHandle  = NULL; /* Shortcut to (CMDSH_File<<2) */
Local struct MsgPort	*CMDSH_FilePort    = NULL; /* Shortcut to (CMDSH_Filehandle->fh_Type) */

/**************************************
	   Internal Prototypes
**************************************/

void CMDSH_Update(void);

/**************************************
		 Macros
**************************************/

#define do2msg(pkt) ((struct Message *)(((ULONG)(pkt)) - offsetof (struct StandardPacket, sp_Pkt)))

/**************************************
	     Implementation
**************************************/





int CMDSH_Send (void) {

    /* ---- safety check */
    if (!CMDSH_File)
	return CMDSH_NOFILE;

    /* ---- Show the prompt */
    CMDSH_Update();
    // FPuts(CMDSH_File, CMDSH_Prompt);
    // Flush(CMDSH_File);

    /* ---- [re]init the packet */
    CMDSH_Packet->dp_Type = ACTION_READ;
    CMDSH_Packet->dp_Arg1 = CMDSH_FileHandle->fh_Arg1;
    CMDSH_Packet->dp_Arg2 = (LONG)CMDSH_Buffer;
    CMDSH_Packet->dp_Arg3 = CMDSH_BufferSize;

    /* ---- send the packet */
    /* CMDSH_Message->mn_ReplyPort = CMDSH_Packet->dp_Port = CMDSH_Port; */
    /* PutMsg (CMDSH_FilePort, CMDSH_Message); */
    SendPkt(CMDSH_Packet, CMDSH_FilePort, CMDSH_Port);

    CMDSH_Pending = 1;

    return 0;
} /* CMDSH_Send */

int CMDSH_CloseShell (void) {

    /* ---- safety check */
    if (!CMDSH_File)
	return CMDSH_NOFILE;

    /* ---- abort the current packet */
    if (CMDSH_Pending) {
	ULONG signals, oldsigs;
	struct DosPacket *dp2;
	oldsigs = SetSignal(0L, SIGF_DOS);

	/* ---- satisfy the READ-Packet */
	dp2 = AllocDosObject (DOS_STDPKT, NULL);
	if (dp2) {
	    dp2->dp_Type = ACTION_FORCE; /* ****That action is not noted in the includes**** */
	    dp2->dp_Arg1 = CMDSH_FileHandle->fh_Arg1;
	    dp2->dp_Arg2 = (LONG)  CMDSH_CloseMsg;
	    dp2->dp_Arg3 = sizeof (CMDSH_CloseMsg);

	    SendPkt(dp2, CMDSH_FilePort, CMDSH_Port);
	    Remove(&WaitPort(CMDSH_Port)->mn_Node);
	    //GetMsg(CMDSH_Port);
	} /* if */

	/* ---- Break the read Packet */
	/*	the AbortPkt() is probably not neccessary since we are now */
	/*	satisfying our own READ-request w/ FORCE (s.a.) */
	// AbortPkt(CMDSH_FileHandle->fh_Type, CMDSH_Packet);


	/* ---- wait for the packet to be replied */
#if 0
	signals = Wait(SIGF_DOS| (1 << CMDSH_Port->mp_SigBit));
	if (signals & SIGF_DOS)
	    GetMsg  (CMDSH_Port);
	else {
	    struct Process *pr;
	    pr = (void *)FindTask(NULL);
	    GetMsg (&pr->pr_MsgPort);
	} /* if */
#else
	Remove(&WaitPort(CMDSH_Port)->mn_Node);
	//GetMsg(CMDSH_Port);
#endif
	if (dp2)
	    FreeDosObject(DOS_STDPKT, dp2);

	oldsigs = SetSignal(oldsigs, SIGF_DOS);
	CMDSH_Pending = 0;
    } /* if */

    /* ---- Close the file */
    if (CMDSH_File)
	Close (CMDSH_File);
    CMDSH_File	     = 0;

    /* ---- clear all file data */
    CMDSH_Active     = 0;
    CMDSH_FileHandle = NULL;
    CMDSH_FilePort   = NULL;

    return CMDSH_OK;
} /* CMDSH_CloseShell */

int CMDSH_OpenShell (const UBYTE *filename) {

    /* ---- safety check */
    if (!CMDSH_Port)
	return CMDSH_NOPORT;

    if (CMDSH_File)
	return CMDSH_FILEOPEN;

    /* ---- Open the file */
    CMDSH_File = Open(filename, MODE_OLDFILE);
    if (!CMDSH_File)
	return CMDSH_NOTFOUND;

    /* ---- safety check */
    if (!IsInteractive(CMDSH_File)) {
	Close (CMDSH_File);
	CMDSH_File = NULL;
	return CMDSH_NOTINTER;
    } /* if */

    /* ---- [re]init all file data */
    CMDSH_Active     = 1;
    CMDSH_FileHandle = BADDR(CMDSH_File);
#ifndef __AROS__
    CMDSH_FilePort   = CMDSH_FileHandle->fh_Type;
#else
    CMDSH_FilePort   = NULL;
#endif

    /* ---- prompt for input */
    CMDSH_Send();

    return CMDSH_OK;
} /* CMDSH_OpenShell */


/*************************************************
	Entry & Exit Code
*************************************************/

Prototype void	CMDSH_Terminate  (void);
void CMDSH_Terminate (void) {

    /* ---- close the file */
    CMDSH_CloseShell();

    /* ---- free all permanent data */
    if (CMDSH_Packet)
	FreeDosObject(DOS_STDPKT, CMDSH_Packet);
    CMDSH_Packet = NULL;

    if (CMDSH_Buffer)
	FreeMem(CMDSH_Buffer, CMDSH_BufferSize);
    CMDSH_Buffer = NULL;

    if (CMDSH_Port)
	DeleteMsgPort(CMDSH_Port);
    CMDSH_Port = NULL;

    if (CMDSH_FileName)
	free(CMDSH_FileName);
    CMDSH_FileName = NULL;

    if (CMDSH_Prompt)
	free(CMDSH_Prompt);
    CMDSH_Prompt = NULL;

} /* CMDSH_Terminate */

Prototype int	CMDSH_Initialize (void);
int CMDSH_Initialize (void) {
    /* ---- allocate all permanent data */
    CMDSH_Packet   = AllocDosObject(DOS_STDPKT, NULL);
    CMDSH_Port	   = CreateMsgPort();
    CMDSH_Buffer   = AllocMem(CMDSH_BufferSize, MEMF_PUBLIC); /* _MUST_ be public Memory! */
    CMDSH_FileName = strdup(DEF_FILENAME);
    CMDSH_Prompt   = strdup(DEF_PROMPT);

    /* ---- safety check */
    if (!CMDSH_Packet || !CMDSH_Port || !CMDSH_Buffer || !CMDSH_FileName || ! CMDSH_Prompt) {
	CMDSH_Terminate();
	return 0;
    } /* if */

    /* ---- init the known shortcuts */
    CMDSH_Message = do2msg(CMDSH_Packet);
    CMDSH_SigMask = 1 << CMDSH_Port->mp_SigBit;
    return 1;
} /* CMDSH_Initialize */

/*************************************************
	Application Interface
*************************************************/

/*
** Explain an CMDSH related error
*/
int CMDSH_Error (int number, const UBYTE *command, ...) {
    switch (number) {
    case CMDSH_OK:
	return RET_SUCC;
    case CMDSH_NOTFOUND:
	error ("%s:\nFile `%s' not found!", command, CMDSH_FileName);
	break;
    case CMDSH_NOFILE:
	error ("%s:\nCommand Shell not open!", command);
	break;
    case CMDSH_NOPORT:
	error ("%s:\nCommand Shell Module not active!", command);
	break;
    case CMDSH_FILEOPEN:
	error ("%s:\nCommand Shell is already open!", command);
	break;
    case CMDSH_NOTINTER:
	error ("%s:\nFile `%s' is not interactive!", command, CMDSH_FileName);
	break;
    default:
	error ("%s:\nCommand Shell Internal Error!", command);
    } /* switch */
    return RET_FAIL;
} /* CMDSH_Error */

/*
**  Get a packet from the Commandshell
*/
Prototype void	CMDSH_Control	 (void);
void CMDSH_Control (void) {
    struct Message *msg;
    msg = GetMsg(CMDSH_Port);
    if (msg != (void *)CMDSH_Message) {
	/* ---- not our packet? */
	if (msg)
	    ReplyMsg(msg);
    } else {
	LONG len, err;
	CMDSH_Pending	 = 0;
	len		 = CMDSH_Packet->dp_Res1;
//	err		 = CMDSH_Packet->dp_Res2;
	CMDSH_Buffer[len] = 0;
	if (!len) {
	    /* ---- user selected Close in the shell? */
	    CMDSH_CloseShell();
	} else {

//	      UBYTE inter = (CMDSH_ErrorsOut?1:0) | (CMDSH_WarningsOut?2:0);
//	      CMDSH_ErrorsOut = CMDSH_WarningsOut = 1;

	    /* ---- strip ending NewLine */
	    while ((--len >= 0) && (CMDSH_Buffer[len] == '\n'))
		CMDSH_Buffer[len] = 0;

	    /* ---- execute the command */
	    do_command(CMDSH_Buffer);

//	      CMDSH_ErrorsOut	= 1 && (inter & 1);
//	      CMDSH_WarningsOut = 1 && (inter & 2);

	    /* ---- prompt for the next input */
	    if (CMDSH_File) {
		CMDSH_Send();
	    } /* if !terminated */
	} /* if !eof */
    } /* if is_cmdmsg */
} /* CMDSH_Control */

/*
**  Redisplay the prompt on the Commandshell
*/
void CMDSH_Update (void) {
    char *inter1, *inter2, *inter3, *inter4, quoted;

    /* SHORTER/BETTER: */
    /* FLEXPRINTF(CMDSH_File, FPuts, CMDSH_Prompt, NULL, mygetvar); */
    /* OR: */
    /* format_string (tmp_buffer, CMDSH_Prompt); */
    /* FPuts   (CMDSH_File, tmp_buffer); */
    /* Flush   (CMDSH_File); */


    inter1 = inter3 = strdup (CMDSH_Prompt);
    if (!inter1) {
	FPuts	(CMDSH_File, CMDSH_Prompt);
	Flush	(CMDSH_File);
	nomemory();
	return;
    } /* if */

    inter2 = breakout(&inter1, &quoted, &inter4);
    if (*inter1) {/* Not enclosed w/ brackets */

	FPuts	(CMDSH_File, CMDSH_Prompt);
	Flush	(CMDSH_File);
    } else {
	FPuts	(CMDSH_File, inter2);
	Flush	(CMDSH_File);
    } /* if */
    if (inter4)
	free(inter4);
    free(inter2);
} /* CMDSH_Update */

/*
**  Make some Output on the Cmdshell
**  after that redisplay the prompt
*/
Prototype int	CMDSH_Print	 (const char *, ULONG *);
int CMDSH_Print (const char *fmt, ULONG *va) {
    if (!CMDSH_File)
	return CMDSH_NOFILE;

    if (CMDSH_Pending)
	FPuts	(CMDSH_File, "\n");
    VFPrintf (CMDSH_File, fmt, va);

    CMDSH_Update();

    return CMDSH_OK;
} /* CMDSH_Print */


/*************************************************
	Fragments of other modules
*************************************************/

/* the line comments are just hints, where to
** look, they cannot be considered staying fixed */

#ifdef MAIN_C

/* line 261/main */
    CMDSH_Initialize();
    Mask |= CMDSH_SigMask;

/* line 485/main */
	if (wait_ret & CMDSH_SigMask) {
	   text_cursor (0);
	   show_cursor ++;

	    CMDSH_Control();
	} /* if is_cmdshell */

/* line 1090/main */
    CMDSH_Terminate();

#endif

#ifdef WIN_C

/* line 1417/error */
    if (CMDSH_Active && CMDSH_ErrorsOut) {
	va_start (va, fmt);
	CMDSH_Print(fmt, (ULONG *)va);
	va_end (va);
    } /* if */

/* line 1440/warn */
    if (CMDSH_Active && CMDSH_WarningsOut) {
	va_start (va, fmt);
	CMDSH_Print(fmt, (ULONG *)va);
	va_end (va);
    } /* if */

#endif


/*************************************************
	    XDME Command Interface
*************************************************/

/*
*!
*! >CLOSECMDSHELL
*!
*!  Close the Commandshell Window.
*!
*  Please note, that currently it is _not_ _safe_
*  to call that commando from elsewhere than the CommandShell
*  itself.
*
*/
/*DEFHELP #commandshell CLOSECMDSHELL - close the command shell */

DEFUSERCMD("closecmdshell", 0, CF_COK|CF_ICO|CF_VWM, int, do_closecmdshell, (void),)
{
//    if (!CMDSH_Pending)
      return CMDSH_Error( CMDSH_CloseShell(), av[0]);
//    error ("%s:\n`%s' currently can be used only\nfrom _within_ the Commandshell!\n", av[0], av[0]);
//    return RET_FAIL;
} /* do_closecmdshell */

/*
*!
*! >OPENCMDSHELL
*!
*!  Open the Commandshell
*!
*!  Please Note, that only one Commandshell can be open
*!  at a time.
*!
*/
/*DEFHELP #commandshell OPENCMDSHELL - open the command shell with the filename in $CMDSHELLNAME */

DEFUSERCMD("opencmdshell", 0, CF_COK|CF_ICO|CF_VWM, int, do_opencmdshell, (void),)
{
    return CMDSH_Error( CMDSH_OpenShell(CMDSH_FileName), av[0], CMDSH_FileName);
} /* do_opencmdshell */


/*
*!
*! >CMDSHELLOUT text
*!
*!  output to the Commandshell
*!
*/
/*DEFHELP #commandshell CMDSHELLOUT txt - output a string to the command shell */

DEFUSERCMD("cmdshellout", 1, CF_COK|CF_ICO|CF_VWM, int, do_cmdshelloutput, (void),)
{
    return CMDSH_Error(CMDSH_Print("%s\n", (ULONG *)&av[1]), av[0]);
} /* do_cmdshelloutput */




/*DEFLONG #long OPENCMDSHELL,CLOSECMDSHELL,CMDSHELLOUT

That Package allows use of an ansynchroneous Commandshell,
a simple Console window to type commands into.

Most aspects of that package can be controlled via variables,
but there are also 3 commands:

    OPENCMDSHELL - makes the commandshell appear;

    CLOSECMDSHELL - makes the commandshell dissappear;

    CMDSHELLOUT text - write some to the commandshell
	please note, that there is a "bug" - we are using
	a printf function to diplay the string, but there
	is no possibility to define any argument for the
	printf template; so all "%" should be duplicated
	or omitted.

the following special variables complete the package:

    $cmdshell - (BOOL) the status of the commandshell;
	 setting it to "1" makes the Commandshell open,
	 setting it to "0" makes the Commandshell close.

    $cmdshellfile - (FILE) the file to be used for the commandshell;
	that file _must_ be interactive.
	plase note, that modification of that variable currently
	only has effect after the NEXT open of the commandshell,
	the active commandshell is not changed.
	Defaults to "CON:0/11/640/60/XDME Command Shell/Close".

    $cmdshellprompt - (STRING) the string to displayed to signal
	the user, that he can enter a command;
	plase note, that modification of that variable currently
	only has effect after the NEXT update of the prompt,
	i.e. after the next time, something was written to the
	commandshell, or the user pressed return in the cmdshell;
	the active prompt is not changed.
	The prompt is expanded (via variable-expansion) each time,
	it is displayed
	Defaults to "`XDME> "

    $errorsoncmdshell - (BOOL) as long as that flag is set, and
	the commandshell is open, all errors are dispayed on the
	commandshell.

    $warningsoncmdshell - (BOOL) as long as that flag is set, and
	the commandshell is open, all warnings are dispayed on the
	commandshell.

*/

#undef STATIC_COM
/***********************************************************
    The STATIC_COM section has currently _2_ purposes:
      - first of all it is designed to allow modules
	that need no references from the main application
	and so can be used just by adding them to the
	source tree of a tool
      - to achieve that goal we have 2 possibilities:
	- we can use the "COMMAND(...)" lines to generate
	  a List of all commands of a certain application,
	  (static solution)
	- we can add AUTOINIT and AUTOEXIT functions
	  which add those COMMAND lines to a global
	  database themselfes (dynamic solution)
	  that way is very interesting in case of using
	  a split application, cluttered in many small
	  modules, which can be activated or removed
	  by user request

    Since Dynamic method means too much overhead to be of
    any use for the XDME, we will probably probably never
    use the DYNAMIC solution, and for that reason it might
    be of more use to put those command lines to the
    functions, which implement those commands.
***********************************************************/
#ifdef STATIC_COM

    COMMAND("closecmdshell",  0, CF_COK|CF_ICO|CF_VWM, (FPTR)do_closecmdshell )
    COMMAND("opencmdshell",   0, CF_COK|CF_ICO|CF_VWM, (FPTR)do_opencmdshell  )
    COMMAND("cmdshellout",    1, CF_COK|CF_ICO|CF_VWM, (FPTR)do_cmdshelloutput)

    DEFFLAG( 94-09-21, CMDSH_Active, 0 )
    DEFFLAG( 94-09-21, CMDSH_WarningsOut, 0 )
    DEFFLAG( 94-09-21, CMDSH_ErrorsOut, 0 )

#endif

#undef SPC_VAR
/***********************************************************
    The SPC_VAR section has only one purpose:
      - it contains descriptions for all special
	variables of the module, which are of interest
	for the user - to change them or to ask their
	values via variable interface.
    that section shall _never_ be considered C-Source
    (for that reason the "#undef" is put just before)
    but it is scanned by an external parser in order
    to produce a list of all Special variables and
    their access functions in a seperate File.
***********************************************************/
#ifdef SPC_VAR

str cmdshellname = %{
    reference = CMDSH_FileName;
    get       = value = $(REFERENCE);
    set       = $(REFERENCE) = strrep ($(REFERENCE), value);
    help      = %{ The filename used for the CommandShell in the next opening; the used file must be interactive, and it must not be a "AUTO" Console window %};
%};

bit cmdshell  = %{
    reference = CMDSH_Active;
    set       = %{ { char inter; inter = test_arg(value, $(REFERENCE)); if (inter != $(REFERENCE)) if (inter) do_opencmdshell(); else do_closecmdshell(); } %};
    help      = %{ the IsOpen status of the CommandShell; %};
%};

str cmdshellprompt = %{
    reference = CMDSH_Prompt;
    get       = value = $(REFERENCE);
    set       = $(REFERENCE) = strrep ($(REFERENCE), value);
    help      = %{ the prompt string of the Command Shell %};
%};

bit errorsoncmdshell  = %{
    reference = CMDSH_ErrorsOut;
    help      = %{ Decide if errors shall be displayed on the commandshell; %};
%};

bit warningsoncmdshell	= %{
    reference = CMDSH_WarningsOut;
    help      = %{ Decide if warnings shall be displayed on the commandshell; %};
%};



#endif


/******************************************************************************
*****  END CMDSH.c
******************************************************************************/


/*

ACTION_WAIT_FOR_CHAR:
    arg1 = TimeOut -> Res1 = Success, Res2 = NumChars

ACTION_FORCE == 2001:
    Arg1 = fh->Arg1? Arg2 = buffer, Arg3 = NumChars -> Res = Success, Res2 = IoErr()
*/

