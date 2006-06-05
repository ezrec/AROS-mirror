/*
 *		SETTINGS.C											vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *
 *		This module handles all the command parsing for tooltypes, command
 *		line options, config files, AREXX, etc.
 */		

#include "system.h"
#include "snoopdos.h"

extern char Version[];			/* From Snoopdos.c */
extern char CommodityTitle[];	/* From Snoopdos.c */

char ConfigID[] = "<SnoopDos Settings>";

ULONG			UpdateFlags;		/* Shows what settings were changed		*/
struct MsgPort *RemoteReplyPort;	/* Used when talking to b/g SnoopDos	*/

/*
 *		Now the result codes returned by our simple command interpreter
 */
typedef enum {
	EXEC_FAIL,			/* The command failed for some reason	*/
	EXEC_OKAY,			/* The command succeeded				*/
	EXEC_UNKNOWN,		/* The command wasn't recognised		*/
	EXEC_NOPARAM		/* The command was missing a parameter	*/
} ExecEnum;

/*
 *		Now let's define the commands we understand.
 */
#define CMD_UNKNOWN		-1
#define CMD_NOPARAM		-2

typedef enum {
	CMD_END,

	/*
	 *		Options within function requester. Note that these MUST correspond
	 *		exactly with the first set of enums in SNOOPDOS.H for GID_#? !
	 */
	CMD_FINDPORT,
	CMD_FINDRESIDENT,
	CMD_FINDSEMAPHORE,
	CMD_FINDTASK,
	CMD_LOCKSCREEN,
	CMD_OPENDEVICE,
	CMD_OPENFONT,
	CMD_OPENLIBRARY,
	CMD_OPENRESOURCE,
	CMD_READTOOLTYPES,
	CMD_SENDREXX,

	CMD_CHANGEDIR,
	CMD_DELETE,
	CMD_EXECUTE,
	CMD_GETVAR,
	CMD_LOADSEG,
	CMD_LOCK,
	CMD_MAKEDIR,
	CMD_MAKELINK,
	CMD_OPEN,
  	CMD_RENAME,
  	CMD_RUNCOMMAND,
  	CMD_SETVAR,
	CMD_DOSSYSTEM,		/* CMD_SYSTEM is defined in dos/dosextens.h	*/

	CMD_DUMMY1,
	CMD_DUMMY2,
	CMD_DUMMY3,
	CMD_DUMMY4,
	CMD_DUMMY5,

	CMD_ONLYSHOWFAILS,
	CMD_SHOWCLI,
	CMD_SHOWFULLPATH,
	CMD_USEDEVICENAME,
	CMD_MONITORPACKETS,
	CMD_MONALLPACKETS,
	CMD_MONROMCALLS,
	CMD_IGNORESHELL,

#define MAX_BOOL_CMD			CMD_IGNORESHELL

	CMD_MATCHNAME,

	/*
	 *		General commands
	 */
	CMD_LOADSETTINGS,
	CMD_SAVESETTINGS,
	CMD_LOADDEFSETTINGS,
	CMD_SAVEDEFSETTINGS,
	CMD_SETTINGS,
	CMD_FUNCTIONS,
	CMD_LANGUAGE,
	CMD_CLEARBUFFER,
	CMD_WINDOWWIDTH,
	CMD_HELP,
	CMD_COPYWINDOW,
	CMD_COPYBUFFER,
	CMD_SAVEWINDOW,
	CMD_SAVEBUFFER,
	CMD_SHOW,
	CMD_HIDE,
	CMD_OPENFORMAT,
	CMD_OPENFUNCTION,
	CMD_OPENSETUP,
	CMD_CLOSEFORMAT,
	CMD_CLOSEFUNCTION,
	CMD_CLOSESETUP,
	CMD_QUIT,
	CMD_PAUSE,
	CMD_UNPAUSE,
	CMD_DISABLE,
	CMD_ENABLE,
	CMD_SINGLESTEP,
	CMD_OPENLOG,
	CMD_APPENDLOG,
	CMD_OPENSERIALLOG,
	CMD_CLOSELOG,
	CMD_ADDLOG,
	CMD_FLUSHLOG,
	CMD_SCROLLUP,
	CMD_SCROLLDOWN,
	CMD_GOTO,

	/*
	 *		General settings
	 */
	CMD_PATCHRAMLIB,
	CMD_STACKLIMIT,
	CMD_ICONPOS,
	CMD_SHOWGADGETS,
	CMD_HIDEGADGETS,
	CMD_AUTOOPEN,
	CMD_DISABLEWHENHIDDEN,
	CMD_SHOWSTATUS,
	CMD_HIDESTATUS,
	CMD_CREATEICONS,
	CMD_TEXTSPACING,
	CMD_SIMPLEREFRESH,
	CMD_SMARTREFRESH,
	CMD_LEFTALIGNED,
	CMD_RIGHTALIGNED,
	CMD_ROWQUALIFIER,
	CMD_MAINPOS,
	CMD_MAINSIZE,
	CMD_FUNCPOS,
	CMD_FORMPOS,
	CMD_SETPOS,
	CMD_TASKPRI,
	CMD_CXPRI,

	/*
	 *		Options within setup requester
	 */
	CMD_HIDEMETHOD,
	CMD_SCREENTYPE,
	CMD_BUFFERSIZE,
	CMD_LOGMODE,
	CMD_FILEIOTYPE,
	CMD_FORMAT,
	CMD_LOGFORMAT,
	CMD_HOTKEY,
	CMD_SCREENNAME,
	CMD_LOGNAME,
	CMD_WINDOWFONT,
	CMD_BUFFERFONT,

	NUMCOMMANDS

} COMMAND_ID;


/*
 *		Now associate a command string with each of these identifiers
 */
struct Command {
	short	cmdid;			/* Command ID							*/
	short	changemask;		/* Which group of settings will change	*/
	short	numparms;		/* Min number of parameters (0 or more)	*/
	char	*name;			/* Command name							*/
} CommandTable[] = {
	/*
	 *		In alphabetical order, for quick reference
	 */
	{ CMD_ADDLOG,			SET_NONE,	1, "AddLog" },
	{ CMD_APPENDLOG,		SET_NONE,	1, "AppendLog" },
	{ CMD_AUTOOPEN,			SET_MAIN,	0, "AutoOpen" },
	{ CMD_BUFFERFONT, 		SET_SETUP, 	1, "BufferFont" },
	{ CMD_BUFFERSIZE, 		SET_SETUP, 	1, "BufferSize" },
	{ CMD_CHANGEDIR,		SET_FUNC,	0, "ChangeDir" },
	{ CMD_CLEARBUFFER,		SET_FUNC,	0, "ClearBuffer" },
	{ CMD_CLOSEFORMAT,		SET_NONE,	0, "CloseFormat" },
	{ CMD_CLOSEFUNCTION,		SET_NONE,	0, "CloseFunction" },
	{ CMD_CLOSELOG,			SET_NONE,	0, "CloseLog" },
	{ CMD_CLOSESETUP, 		SET_NONE,	0, "CloseSetup" },
	{ CMD_COPYBUFFER, 		SET_NONE,	0, "CopyBuffer" },
	{ CMD_COPYWINDOW, 		SET_NONE,	0, "CopyWindow" },
	{ CMD_CREATEICONS,		SET_MAIN,	0, "CreateIcons" },
	{ CMD_HOTKEY,			SET_SETUP,	1, "CX_PopKey" },
	{ CMD_SHOW,			SET_NONE,	0, "CX_Popup" },
	{ CMD_CXPRI,			SET_NONE,	1, "CX_Priority" },
	{ CMD_DELETE, 			SET_FUNC,	0, "Delete" },
	{ CMD_DISABLE,			SET_NONE,	0, "Disable" },
	{ CMD_DISABLEWHENHIDDEN,	SET_MAIN,	0, "DisableWhenHidden" },
	{ CMD_ENABLE, 			SET_NONE,	0, "Enable" },
	{ CMD_EXECUTE,			SET_FUNC,	0, "Execute" },
	{ CMD_FILEIOTYPE, 		SET_SETUP,	1, "FileIOType" },
	{ CMD_FINDPORT,			SET_FUNC,	0, "FindPort" },
	{ CMD_FINDRESIDENT,		SET_FUNC,	0, "FindResident" },
	{ CMD_FINDSEMAPHORE,		SET_FUNC,	0, "FindSemaphore" },
	{ CMD_FINDTASK,			SET_FUNC,	0, "FindTask" },
	{ CMD_FLUSHLOG,			SET_NONE,	0, "FlushLog" },
	{ CMD_FORMAT, 			SET_SETUP,	1, "Format" },
	{ CMD_FORMPOS,			SET_MAIN,	1, "FormatWindowPos" },
	{ CMD_FUNCTIONS,		SET_FUNC,	1, "Functions" },
	{ CMD_FUNCPOS,			SET_MAIN,	1, "FunctionWindowPos" },
	{ CMD_GETVAR, 			SET_FUNC,	0, "GetVar" },
	{ CMD_GOTO,			SET_NONE,	1, "GotoLine" },
	{ CMD_HELP,			SET_NONE,	0, "Help" },
	{ CMD_HIDE,			SET_NONE,	0, "Hide" },
	{ CMD_HIDEGADGETS,		SET_MAIN,	0, "HideGadgets" },
	{ CMD_HIDEMETHOD, 		SET_SETUP,	1, "HideMethod" },
	{ CMD_HIDESTATUS, 		SET_MAIN,	0, "HideStatus" },
	{ CMD_HOTKEY, 			SET_SETUP,	1, "HotKey" },
	{ CMD_ICONPOS,			SET_MAIN,	1, "IconPos" },
	{ CMD_IGNORESHELL,		SET_FUNC,	0, "IgnoreShell" },
	{ CMD_LANGUAGE,			SET_NONE,	1, "Language" },
	{ CMD_LEFTALIGNED,		SET_MAIN,	0, "LeftAligned" },
	{ CMD_LOADDEFSETTINGS,		SET_NONE,	0, "LoadDefSettings" },
	{ CMD_LOADSEG,			SET_FUNC,	0, "LoadSeg" },
	{ CMD_LOADSETTINGS, 		SET_NONE,	1, "LoadSettings" },
	{ CMD_LOCK,			SET_FUNC,	0, "Lock" },
	{ CMD_LOCKSCREEN, 		SET_FUNC,	0, "LockScreen" },
	{ CMD_LOGFORMAT,		SET_SETUP,	1, "LogFormat" },
	{ CMD_LOGMODE,			SET_SETUP,	1, "LogMode" },
	{ CMD_LOGNAME,			SET_SETUP,	1, "LogName" },
	{ CMD_MAINPOS,			SET_MAIN,	1, "MainWindowPos" },
	{ CMD_MAINSIZE,			SET_MAIN,	1, "MainWindowSize" },
	{ CMD_MAKEDIR,			SET_FUNC,	0, "MakeDir" },
	{ CMD_MAKELINK,			SET_FUNC,	0, "MakeLink" },
	{ CMD_MATCHNAME,		SET_FUNC,	1, "MatchName" },
	{ CMD_MONITORPACKETS, 		SET_FUNC,	0, "MonitorPackets" },
	{ CMD_MONROMCALLS,		SET_FUNC,	0, "MonitorROMCalls" },
	{ CMD_ONLYSHOWFAILS,		SET_FUNC,	0, "OnlyShowFails" },
	{ CMD_OPEN,			SET_FUNC,	0, "Open" },
	{ CMD_OPENDEVICE, 		SET_FUNC,	0, "OpenDevice" },
	{ CMD_OPENFONT,			SET_FUNC,	0, "OpenFont" },
	{ CMD_OPENFORMAT, 		SET_NONE,	0, "OpenFormat" },
	{ CMD_OPENFUNCTION,		SET_NONE,	0, "OpenFunction" },
	{ CMD_OPENLIBRARY,		SET_FUNC,	0, "OpenLibrary" },
	{ CMD_OPENLOG,			SET_NONE,	1, "OpenLog" },
	{ CMD_OPENRESOURCE,		SET_FUNC,	0, "OpenResource" },
	{ CMD_OPENSERIALLOG,		SET_NONE,	0, "OpenSerialLog" },
	{ CMD_OPENSETUP,		SET_NONE,	0, "OpenSetup" },
	{ CMD_MONALLPACKETS,		SET_FUNC,	0, "PacketDebugger" },
	{ CMD_PATCHRAMLIB,		SET_NONE,	0, "PatchRamLib" },
	{ CMD_PAUSE,			SET_NONE,	0, "Pause" },
	{ CMD_QUIT,			SET_NONE,	0, "Quit" },
	{ CMD_READTOOLTYPES,		SET_FUNC,	0, "ReadToolTypes" },
	{ CMD_RENAME, 			SET_FUNC,	0, "Rename" },
	{ CMD_RIGHTALIGNED,		SET_MAIN,	0, "RightAligned" },
	{ CMD_ROWQUALIFIER,		SET_MAIN,	1, "RowQualifier" },
	{ CMD_RUNCOMMAND, 		SET_FUNC,	0, "RunCommand" },
	{ CMD_SAVEBUFFER, 		SET_NONE,	1, "SaveBuffer" },
	{ CMD_SAVEDEFSETTINGS,  	SET_NONE,	0, "SaveDefSettings" },
	{ CMD_SAVESETTINGS, 		SET_NONE,	1, "SaveSettings" },
	{ CMD_SAVEWINDOW, 		SET_NONE,	1, "SaveWindow" },
	{ CMD_SCREENNAME, 		SET_SETUP,	1, "ScreenName" },
	{ CMD_SCREENTYPE, 		SET_SETUP,	1, "ScreenType" },
	{ CMD_SCROLLDOWN,		SET_NONE,	1, "ScrollDown" },
	{ CMD_SCROLLUP,			SET_NONE,	1, "ScrollUp" },
	{ CMD_SENDREXX,			SET_FUNC,	0, "SendRexx" },
	{ CMD_SETTINGS,			SET_NONE,	1, "Settings" },
	{ CMD_SETPOS, 			SET_MAIN,	1, "SetupWindowPos" },
	{ CMD_SETVAR, 			SET_FUNC,	0, "SetVar" },
	{ CMD_SHOW,			SET_NONE,	0, "Show" },
	{ CMD_SHOWCLI,			SET_FUNC,	0, "ShowCLI" },
	{ CMD_SHOWFULLPATH,		SET_FUNC,	0, "ShowFullPath" },
	{ CMD_SHOWGADGETS,		SET_MAIN,	0, "ShowGadgets" },
	{ CMD_SHOWSTATUS, 		SET_MAIN,	0, "ShowStatus" },
	{ CMD_SIMPLEREFRESH,		SET_MAIN,	0, "SimpleRefresh" },
	{ CMD_SINGLESTEP,		SET_NONE,	0, "SingleStep" },
	{ CMD_SMARTREFRESH,		SET_MAIN,	0, "SmartRefresh" },
	{ CMD_STACKLIMIT,		SET_MAIN,	1, "StackLimit" },
	{ CMD_DOSSYSTEM,		SET_FUNC,	0, "System" },
	{ CMD_TASKPRI,			SET_NONE,	1, "TaskPri" },
	{ CMD_TEXTSPACING,		SET_MAIN,	1, "TextSpacing" },
	{ CMD_UNPAUSE,			SET_NONE,	0, "Unpause" },
	{ CMD_USEDEVICENAME,		SET_FUNC,	0, "UseDeviceNames" },
	{ CMD_WINDOWFONT, 		SET_SETUP,	1, "WindowFont" },
	{ CMD_WINDOWWIDTH,		SET_NONE,	1, "WindowWidth" },
	{ CMD_END,			0,		0, NULL }
};

#define NUM_CMDNAMES	(sizeof(CommandTable) / sizeof(CommandTable[0]))

char *CmdNames[NUMCOMMANDS];	/* Map command IDs back to name strings	*/

char *Names_HideMethod[] = { "Invisible", "Iconify",	"ToolsMenu",
							 "None",	   NULL };
char *Names_ScreenType[] = { "Default",   "Front",		"Named",		NULL };
char *Names_LogMode[]	 = { "Prompt",	  "Append",		"Overwrite",
						     "SerialPort", NULL };
char *Names_FileIOType[] = { "Automatic", "Immediate",	"Buffered",		NULL };

char *Names_Functions[]  = { "All", "None", "AllDos", "NoDos",
							 "AllSystem", "NoSystem", NULL };

char *Names_RowQualifier[] = { "Ignore", "None", "Shift", "Alt", "Ctrl",
							   "All",     NULL };

typedef enum {
	FUNC_ALL, FUNC_NONE, FUNC_ALLDOS, FUNC_NODOS,
	FUNC_ALLSYSTEM, FUNC_NOSYSTEM
} FuncTypes;

/*
 *		InitSettings()
 *
 *		Initialises variables associated with settings. For now, this
 *		just means the mapping array that maps command IDs to command
 *		names. This makes it easier to write out the configuration file.
 */
void InitSettings(void)
{
	struct Command *cmd;
	int i;

	for (i = 0; i < NUMCOMMANDS; i++)
		CmdNames[i] = "";	/* Make sure non-cmds initialised to safe value */

	for (cmd = CommandTable; cmd->cmdid != CMD_END; cmd++)
		CmdNames[cmd->cmdid] = cmd->name; 
}

/*
 *		MatchParam(&var, param, keywords)
 *
 *		Searches the keyword list for a match with param and stores
 *		the result in the given variable. If no match is found, var
 *		is not altered and returns EXEC_FAIL, else returns EXEC_OKAY.
 */
int MatchParam(unsigned char *var, char *param, char **keywords)
{
	int i;

	for (i = 0; keywords[i]; i++) {
		if (stricmp(param, keywords[i]) == 0) {
			*var = i;
			return (EXEC_OKAY);
		}
	}
	return (EXEC_FAIL);
}

/*
 *		ParseFontName(fontspec, fontname, &fontsize)
 *
 *		Parses the font spec in fontspec, which is of the form
 *		"fontname size" or "fontname.font size" or "fontname.size",
 *		into two parts.
 *
 *		The bulk of the name (plus a .font suffix) is copied into fontname.
 *		The size of the font is copied into fontsize. In the event of a parse
 *		error, EXEC_FAIL is returned and no alteration is made to fontname
 *		or fontsize. For success, EXEC_OKAY is returned.
 *
 *		Note that the contents of the string pointed to by fontspec may be
 *		altered. If fontspec is "default" then a fontsize of 0 is returned
 *		to indicate that SnoopDos should use the system default fonts.
 */
int ParseFontName(char *fontspec, char *fontname, UWORD *fontsize)
{
	char *p;
	int size;

	/*
	 *		First, see if we can find a dot specifier. Anything before
	 *		the dot is a font name.
	 */
	if (stricmp(fontspec, "default") == 0) {
		strcpy(fontname, fontspec);
		*fontsize = 0;
		return (EXEC_OKAY);
	}
	p = strchr(fontspec, '.');
	if (p) {
		*p++ = '\0';
		if (strnicmp(p, "font", 4) == 0)
			p += 4;
	} else {
		p = strchr(fontspec, ' ');
		if (!p)
			return (EXEC_FAIL);
		*p++ = '\0';
	}
	while (*p == ' ' || *p == '.')
		p++;
	size = atoi(p);
	if (size == 0)
		return (EXEC_FAIL);
	
	strcpy(fontname, fontspec);
	strcat(fontname, ".font");
	*fontsize = size;
	return (EXEC_OKAY);
}

/*
 *		ParseTwoNums(param, &num1, &num2)
 *
 *		Parses the given param string, assumed to be in the form "x1,x2"
 *		and stores the two numbers in the given variables. If either number
 *		is invalid, then no change is made to either number and EXEC_FAIL
 *		is returned, else EXEC_OKAY is returned.
 *
 *		The string pointed to by param may be altered.
 */
int ParseTwoNums(char *param, WORD *num1, WORD *num2)
{
	char *p;

	p = strchr(param, ',');
	if (!p)
		return (EXEC_FAIL);
	*p++ = '\0';

	while (*p == ' ')
		p++;
	
	if (*param < '-' || *param > '9' || *p < '-' || *p > '9')
		return (EXEC_FAIL);
	
	*num1 = atoi(param);
	*num2 = atoi(p);
	return (EXEC_OKAY);
}

/*
 *		cid = ParseCommand(cmdline, cmdname, param, &boolvalue, &cmd)
 *
 *		Copies the cmdline into two separate strings, one for the
 *		command name and one for the parameters. Bool is initalised
 *		to 0 or 1, according to the default value of the parameter
 *		(1 if Yes,True,On, 0 if No, False, Off, or if the command
 *		is prefixed by the word No).
 *
 *		cmd points to the command structure for this command.
 *
 *		cid is the command ID that matches the command, CMD_END for
 *		a null command, CMD_UNKNOWN for an unrecognised command,
 *		and CMD_NOPARAM for a known command with too few parameters.
 *		If CMD_NOPARAM is returned, cmdname and &cmd will be initialised
 *		to the command that was matched.
 *
 */
int ParseCommand(char *cmdline, char *cmdname, char *param, int *boolvalue,
				 struct Command **pcmd)
{
	struct Command *cmd;
	int cid;
	char *p;
	char *q;

	if (!cmdline)
		return (CMD_END);

	/*
	 *		First, copy just the command header
	 */
	for (p = cmdline; *p == ' ' || *p == '\t'; p++)
		;
	q = cmdname;
	while (*p        && *p != ';'  && *p != '='  &&
		   *p != ' ' && *p != '\t' && *p != '\n')
		*q++ = *p++;
	*q = '\0';

	if (!*cmdname)
		return (CMD_END);	/* Skip over blank lines and comments */

	/*
	 *		Now skip over any whitespace until we find the next parameter
	 */
	while (*p == ' ' || *p == '=' || *p == '\t' || *p == '\n')
		p++;
	q = param;
	if (*p == '\"') {
		/*
		 *		Got a quoted string .. copy everything inside the quotes
		 */
		p++;
		while (*p && *p != '\n' && *p != '\"')
			*q++ = *p++;
		*q = '\0';
	} else {
		/*
		 *		Not a quoted string .. copy everything to end of line
		 *		or semicolon, excluding any trailing spaces.
		 */
		while (*p && *p != '\n' && *p != ';')
			*q++ = *p++;
		while (--q >= param && (*q == ' ' || *q == '\n' || *q == '\t'))
			;
		*++q = '\0';
		if (q == param)
			q = NULL;
	}

	/*
	 *		Finally, if we got no specific parameter, check if the
	 *		command beings with "No" -- if so, then assume the
	 *		parameter is boolean instead and set accordingly
	 */
	*boolvalue = 1;
	if (!q) {
		if (strnicmp(cmdname, "no", 2) == 0) {
			*boolvalue = 0;
			cmdname   += 2;
		}
	} else {
		if (	stricmp(param, "off") == 0	||
				stricmp(param, "no")  == 0	||
				stricmp(param, "0")   == 0)
		{
			*boolvalue = 0;
		}
	}

	/*
	 *		Now locate identifier which matches command
	 */
	for (cmd = CommandTable; cmd->cmdid != CMD_END; cmd++)
		if (stricmp(cmd->name, cmdname) == 0)
			break;
	
	cid = cmd->cmdid;
	if (cid == CMD_END)
		return (CMD_UNKNOWN);

	*pcmd = cmd;
	if (cmd->numparms > 0 && !q)
		return (CMD_NOPARAM);
	
	return (cid);
}

/*
 *		ExecCommand(cmd, mode, set)
 *
 *		Interprets the given command and executes it. cmd points to the
 *		command string which is one of the following forms:
 *
 *			command
 *			command=value
 *			setting
 *			nosetting
 *			setting=on
 *			setting=off
 *	
 *		The "setting" commands are used to toggle the various boolean
 *		settings that can be accessed through the requesters.
 *
 *		mode is the place where the setting occurs. This will be one of
 *		MODE_REXX, MODE_SETTINGS, MODE_CMDLINE or MODE_TOOLTYPE, depending
 *		on where the command being executed originated from. In some cases,
 *		this affects how the command is executed.
 *
 *		set is a pointer to a copy of the current settings (NOT the actual
 *		settings!) This copy is updated to reflect the changes. The intention
 *		is that you can call ExecCommand() multiple times with the same
 *		copy, and then finally call InstallSettings() at the end to make
 *		the new settings take effect. This way, only a single update
 *		needs to be done for an entire configuration file.
 *
 *		The global UpdateFlags should be initialised to 0 before the first
 *		call, and then passed to InstallSettings() at the end -- it will
 *		contain the appropriate combination of SET_??? flags to indicate
 *		what settings actually changed.
 *
 *		Returns EXEC_OKAY for success, EXEC_FAIL for failure, or EXEC_UNKNOWN
 *		if the command couldn't be understood.
 */
int ExecCommand(char *cmdline, int mode, Settings *set)
{
	APTR oldwinptr = *TaskWindowPtr;
	struct Command *cmd;		/* Pointer to command						*/
	char cmdname[100];			/* Storage area for command 				*/
	char parambuf[100];			/* Storage for parameters					*/
	char *param;				/* Pointer to parameters					*/
	int	 boolvalue;				/* for settings: either 1 or 0				*/
	int  success = EXEC_OKAY;	/* Default return value						*/
	int  cid;
	int  val;

	cid = ParseCommand(cmdline, cmdname, parambuf, &boolvalue, &cmd);
	switch (cid) {
		case CMD_END:		return (EXEC_OKAY);
		case CMD_UNKNOWN:	return (EXEC_UNKNOWN);
		case CMD_NOPARAM:	return (EXEC_NOPARAM);
	}

	UpdateFlags |= cmd->changemask;
	param = parambuf;
	
	/*
	 *		Okay, got ourselves a valid command. Now interpret it.
	 */
	if (cid <= MAX_BOOL_CMD) {
		/*
		 *		Got a boolean option to update
		 */
		set->Func.Opts[cid] = boolvalue;
		return (EXEC_OKAY);
	}

	switch (cid) {
		case CMD_LANGUAGE:
			strcpy(Language, param);
			break;

		case CMD_SETTINGS:
			strcpy(DefaultConfigName, param);
			strcpy(ConfigFileName, param);
			break;

		case CMD_LOADDEFSETTINGS:
			success = LoadConfig(DefaultConfigName, mode, set);
			break;

		case CMD_SAVEDEFSETTINGS:
			success = SaveConfig(DefaultConfigName, SAVE_NOICON);
			break;

		case CMD_LOADSETTINGS:
			success = LoadConfig(param, mode, set);
			break;

		case CMD_SAVESETTINGS:
			success = SaveConfig(param, SAVE_ICON);
			break;

		case CMD_HELP:
		{
			char helpmsg[100];

			/*
			 *		We install the current settings first, to ensure
			 *		that things like the default public screen are
			 *		correctly set up before we open help.
			 */
			if (UpdateFlags) {
				InstallSettings(set, UpdateFlags);
				UpdateFlags = 0;
				*set = CurSettings;
			}
			if (*param)
				mysprintf(helpmsg, "LINK %s\n", param);
			else
				strcpy(helpmsg, MSG(MSG_LINK_MAIN));
			ShowAGuide(helpmsg);
			break;
		}

		case CMD_SHOWGADGETS:	set->ShowGadgets   =  boolvalue;	break;
		case CMD_HIDEGADGETS:	set->ShowGadgets   = !boolvalue;	break;
		case CMD_SHOWSTATUS:	set->ShowStatus    =  boolvalue;	break;
		case CMD_HIDESTATUS:	set->ShowStatus	   = !boolvalue;	break;
		case CMD_AUTOOPEN:		set->AutoOpenMain  =  boolvalue;	break;
		case CMD_DISABLEWHENHIDDEN: set->DisableWhenHidden = boolvalue; break;
		case CMD_CREATEICONS:	set->MakeIcons     =  boolvalue;	break;
		case CMD_SIMPLEREFRESH:	set->SimpleRefresh =  boolvalue;	break;
		case CMD_SMARTREFRESH:	set->SimpleRefresh = !boolvalue;	break;
		case CMD_LEFTALIGNED:	set->RightAlign    = !boolvalue;	break;
		case CMD_RIGHTALIGNED:	set->RightAlign    =  boolvalue;	break;
		case CMD_TEXTSPACING:	
			val = *param - '0';
			if (val < 0 || val > 2)
				return (EXEC_FAIL);
			set->TextSpacing = val;
			break;

		case CMD_ROWQUALIFIER:
			return MatchParam(&set->RowQualifier, param, Names_RowQualifier);

		case CMD_FUNCTIONS:
			/*
			 *		Set or clear the function table according to the
			 *		parameters
			 */
		{
			unsigned char functype;
			int low  = FIRST_SYS_GADGET;
			int high = LAST_DOS_GADGET;
			int bool = 1;
			int i;

			if (MatchParam(&functype, param, Names_Functions) == EXEC_FAIL)
				return (EXEC_FAIL);

			switch (functype) {
				case FUNC_ALL:		 break;				/* Use defaults */
				case FUNC_NONE:		 bool = 0; break;
				case FUNC_ALLDOS:	 low  = FIRST_DOS_GADGET;			break;
				case FUNC_NODOS:	 low  = FIRST_DOS_GADGET; bool = 0; break;
				case FUNC_ALLSYSTEM: high = LAST_SYS_GADGET;			break;
				case FUNC_NOSYSTEM:	 high = LAST_SYS_GADGET;  bool = 0; break;
			}
			for (i = low; i <= high; i++)
				set->Func.Opts[i] = bool;
			break;
		}

		case CMD_COPYWINDOW:
			DisableAllWindows();
			success = SaveBuffer(SAVEBUF_WINDOW, SAVEBUF_CLIPBOARD,
								 SAVEBUF_OVERWRITE);
			EnableAllWindows();
			break;
								/* Fallthrough */
		case CMD_COPYBUFFER:
			DisableAllWindows();
			success = SaveBuffer(SAVEBUF_ALL, SAVEBUF_CLIPBOARD,
								 SAVEBUF_OVERWRITE);
			EnableAllWindows();
			break;

		case CMD_SAVEWINDOW:
			DisableAllWindows();
			success = SaveBuffer(SAVEBUF_WINDOW, param, SAVEBUF_OVERWRITE);
			EnableAllWindows();
			break;

		case CMD_SAVEBUFFER:
			DisableAllWindows();
			success = SaveBuffer(SAVEBUF_ALL, param, SAVEBUF_OVERWRITE);
			EnableAllWindows();
			break;
			
		case CMD_CXPRI:
			CommodityPriority = atoi(param);
			if (HotKeyActive) {
				/*
				 *		Reinstall commodity to take advantage of the
				 *		updated priority
				 */
				InstallHotKey(CurSettings.Setup.HotKey);
			}
			break;

		case CMD_ICONPOS:
			success = ParseTwoNums(param, &set->IconPosLeft, &set->IconPosTop);
			break;

		case CMD_HIDE:
		case CMD_SHOW:
			/*
			 *		Install all settings modified so far
			 */
			if (UpdateFlags) {
				InstallSettings(set, UpdateFlags);
				UpdateFlags = 0;
				*set = CurSettings;
			}
			/*
			 *		We allow both of these so that NOSHOW and NOHIDE
			 *		will also work, as well as SHOW=YES and HIDE=NO
			 *		In particular, CX_Popup=Yes and CX_Popup=No will
			 *		work as expected.
			 */
			if ((boolvalue && cid == CMD_SHOW) ||
										(!boolvalue && cid == CMD_HIDE)) {
				/*
				 *		Showing SnoopDos. Install all settings changed
				 *		so far, so that they'll be in effect when the window
				 *		is open (especially font changes)
				 */
				success = ShowSnoopDos();
				HideOnStartup = 0;
			} else {
				HideSnoopDos();
				HideOnStartup = 1;
			}
			break;

		case CMD_OPENFORMAT:
		case CMD_OPENFUNCTION:
		case CMD_OPENSETUP:
				/*
				 *		Install the current settings, so that they take
				 *		effect before the window is opened (font changes
				 *		and the like).
				 */
				if (UpdateFlags) {
					InstallSettings(set, UpdateFlags);
					UpdateFlags = 0;
					*set = CurSettings;
				}
				switch (cid) {
					case CMD_OPENFORMAT:	success = OpenFormatWindow();
											break;
					case CMD_OPENFUNCTION:	success = OpenFunctionWindow();
											break;
					case CMD_OPENSETUP:		success = OpenSettingsWindow();
											break;
				}
				break;

		case CMD_CLOSEFORMAT:		CloseFormatWindow();			break;
		case CMD_CLOSEFUNCTION:		CloseFunctionWindow();			break;
		case CMD_CLOSESETUP:		CloseSettingsWindow();			break;

		case CMD_QUIT:
			if (mode != MODE_SETTINGS)
				QuitFlag = 1;
			else
				success = EXEC_FAIL;
			break;
			
		case CMD_SINGLESTEP:		SingleStep();						break;

		case CMD_PAUSE:
		case CMD_DISABLE:
		case CMD_UNPAUSE:
		case CMD_ENABLE:
			/*
			 *		We treat these four together so we can account for
			 *		all the different boolean verisons (Disable=Yes,
			 *		Enable=No, UnPause=Yes, Pause=No, etc.)
			 */
			if ((cid == CMD_DISABLE && boolvalue) ||
				(cid == CMD_ENABLE && !boolvalue))
			{
				SetMonitorMode(MONITOR_DISABLED);
			} else if ((cid == CMD_PAUSE && boolvalue) ||
					   (cid == CMD_UNPAUSE && !boolvalue))
			{
				SetMonitorMode(MONITOR_PAUSED);
			} else
				SetMonitorMode(MONITOR_NORMAL);
			break;

		case CMD_OPENLOG:
		case CMD_APPENDLOG:
			/*
			 *		For these two, we make any current settings take effect,
			 *		so that things like default file buffering mode will be
			 *		activated.
			 */
			if (UpdateFlags) {
				InstallSettings(set, UpdateFlags);
				UpdateFlags = 0;
				*set = CurSettings;
			}
			if (cid == CMD_OPENLOG)
				success = OpenLog(LOGMODE_OVERWRITE, param);
			else
				success = OpenLog(LOGMODE_APPEND, param);
			break;

		case CMD_OPENSERIALLOG:
			success = OpenLog(LOGMODE_SERIALPORT, param);
			break;

		case CMD_CLOSELOG:
			CloseLog();
			break;

		case CMD_ADDLOG:
			/*
			 *		Output a user comment to the specified logfile
			 */
			strcat(param, "\n");
			WriteLog(param);
			break;

		case CMD_FLUSHLOG:
			/*
			 *		Flush the current contents of the log
			 */
			WriteLog(NULL);
			break;

		case CMD_HIDEMETHOD: return MatchParam(&set->Setup.HideMethod,	param,
											   Names_HideMethod);
		case CMD_SCREENTYPE: return MatchParam(&set->Setup.ScreenType,	param,
											   Names_ScreenType);
		case CMD_LOGMODE:	 return MatchParam(&set->Setup.LogMode,		param,
											   Names_LogMode);
		case CMD_FILEIOTYPE: return MatchParam(&set->Setup.FileIOType,	param,
											   Names_FileIOType);

		case CMD_BUFFERSIZE:
			val = atoi(param);
			if (val > 1)
				set->Setup.BufferSize = val;
			break;

		case CMD_WINDOWWIDTH:
			SetMainWindowWidth(atoi(param));	/* 0 is supported */
			break;

		case CMD_STACKLIMIT:
			if (*param < '0' || *param > '9')
				return (EXEC_FAIL);
			set->StackLimit = atoi(param);
			break;

		case CMD_PATCHRAMLIB:	break;	/* Actually handled in ParseStartup */

		case CMD_FORMAT:		strcpy(set->Setup.BufferFormat,  param); break;
		case CMD_LOGFORMAT:
			if (stricmp(param, "none") == 0)
				*param = '\0';
			strcpy(set->Setup.LogfileFormat, param); break;
			break;

		case CMD_HOTKEY:		strcpy(set->Setup.HotKey,	 	 param); break;
		case CMD_SCREENNAME:	strcpy(set->Setup.ScreenName,	 param); break;
		case CMD_LOGNAME:		strcpy(set->Setup.LogFile,		 param);
								*TaskWindowPtr = (APTR)-1;
								if (IsFileSystem(set->Setup.LogFile))
									strcpy(ChosenLogName, param);
								*TaskWindowPtr = oldwinptr;
								break;

		case CMD_MATCHNAME:  	strcpy(set->Func.Pattern,		 param); break;
		case CMD_WINDOWFONT:
			return ParseFontName(param, set->Setup.WindowFont,
										&set->Setup.WinFontSize);
		case CMD_BUFFERFONT:
			return ParseFontName(param, set->Setup.BufferFont,
										&set->Setup.BufFontSize);

		case CMD_MAINSIZE:
			success = ParseTwoNums(param, &set->MainWinWidth,
									      &set->MainWinHeight);
			if (success && mode != MODE_SETTINGS && MainWindow)
				SizeWindow(MainWindow, set->MainWinWidth - MainWindow->Width,
									   set->MainWinHeight- MainWindow->Height);
			break;

		case CMD_MAINPOS:
			success = ParseTwoNums(param, &set->MainWinLeft, &set->MainWinTop);
			if (success && mode != MODE_SETTINGS && MainWindow)
				MoveWindow(MainWindow, set->MainWinLeft - MainWindow->LeftEdge,
									   set->MainWinTop  - MainWindow->TopEdge);
			break;

		case CMD_FORMPOS:
			success = ParseTwoNums(param, &set->FormWinLeft, &set->FormWinTop);
			if (success && mode != MODE_SETTINGS && FormWindow)
				MoveWindow(FormWindow, set->FormWinLeft - FormWindow->LeftEdge,
									   set->FormWinTop  - FormWindow->TopEdge);
			break;

		case CMD_FUNCPOS:
			success = ParseTwoNums(param, &set->FuncWinLeft, &set->FuncWinTop);
			if (success && mode != MODE_SETTINGS && FuncWindow)
				MoveWindow(FuncWindow, set->FuncWinLeft - FuncWindow->LeftEdge,
									   set->FuncWinTop  - FuncWindow->TopEdge);
			break;

		case CMD_SETPOS:
			success = ParseTwoNums(param, &set->SetupWinLeft,
										  &set->SetupWinTop);
			if (success && mode != MODE_SETTINGS && SetWindow)
				MoveWindow(SetWindow, set->SetupWinLeft - SetWindow->LeftEdge,
									  set->SetupWinTop  - SetWindow->TopEdge);
			break;

		case CMD_TASKPRI:
			val = atoi(param);
			if ((val == 0 && *param != '0') || val < -20 || val > 20)
				return (EXEC_FAIL);
			SetTaskPri(SysBase->ThisTask, val);
			if (MainWindow)
				SetMenuOptions();
			break;

		case CMD_CLEARBUFFER:
			ClearWindowBuffer();
			break;

		case CMD_GOTO:
			val = atoi(param);
			if (val != 0) {
				val += BaseSeq;		/* Get real start position */
				if (val > TopSeq)
					DoArrowScrolling(GID_DOWNARROW, val - TopSeq);
				else
					DoArrowScrolling(GID_UPARROW, TopSeq - val);
			} else
				success = EXEC_FAIL;
			break;

		case CMD_SCROLLUP:
		case CMD_SCROLLDOWN:
			val = atoi(param);
			if (!val)
				val = 1;
			if (cid == CMD_SCROLLUP)
				DoArrowScrolling(GID_UPARROW, val);
			else
				DoArrowScrolling(GID_DOWNARROW, val);
			break;
	}
	return (success ? EXEC_OKAY : EXEC_FAIL);
}

/*
 *		GetFontDesc(fontdesc, fontname, size)
 *
 *		Builds a a string from the supplied fontname and size and stores
 *		it in fontdesc. Returns a pointer to the new string.
 *
 *		For example, name="courier.font" and size=13 will return the
 *		string "courier 13";
 *
 *		In the event that the fontname is invalid, the string "Default"
 *		is returned instead.
 */
char *GetFontDesc(char *fontdesc, char *fontname, int size)
{
	char *p;

	if (!fontname || !*fontname)
		return ("Default");

	strcpy(fontdesc, fontname);
	p = strchr(fontdesc, '.');
	if (!p)
		p = fontdesc + strlen(fontdesc);
	
	mysprintf(p, " %ld", size);
	return (fontdesc);
}

/*
 *		SaveConfig(filename, saveicon)
 *
 *		Writes the current configuration to the named disk file. Returns
 *		1 for success, 0 for failure.
 *
 *		saveicon is SAVE_ICON if an icon should be saved and CreateIcos
 *		is true, or SAVE_NOICON if an icon should never be saved.
 */
int SaveConfig(char *filename, int saveicon)
{
	char tempdesc[MAX_SHORT_LEN];
	SetupSettings *setup = &CurSettings.Setup;
	BPTR file;
	int i;

	mysprintf(StatusLineText, MSG(MSG_STATUS_SAVESET), filename);
	ShowStatus(StatusLineText);

	DisableAllWindows();
	RecordWindowSizes();
	file = Open(filename, MODE_NEWFILE);
	if (!file) {
		EnableAllWindows();
		UpdateStatus();
		return (0);
	}
	FPrintf(file, "%s\n;\n;    %s\n;\n;    Settings file\n;\n",
			ConfigID, Version);

#define NM(x)			CmdNames[CMD_##x]
#define PF1(s,p)		FPrintf(file, s, p)
#define PF2(s,p1,p2)	FPrintf(file, s, p1, p2)

	/*
	 *		First of all, write out the function settings
	 */
	for (i = 1; i <= MAX_BOOL_CMD; i++) {
		if (*CmdNames[i]) {
			if (CurSettings.Func.Opts[i])
				PF1("%s\n",	CmdNames[i]);
			else
				PF1("No%s\n", CmdNames[i]);
		}
	}
	/*
	 *		Now write out all the variable settings
	 */
	PF2(";\n%s=\"%s\"\n",NM(MATCHNAME),	CurSettings.Func.Pattern);
	PF2("%s=%s\n",	 	NM(HIDEMETHOD), Names_HideMethod[setup->HideMethod]);
	PF2("%s=%s\n",	 	NM(SCREENTYPE), Names_ScreenType[setup->ScreenType]);
	PF2("%s=%s\n",	 	NM(LOGMODE),  	Names_LogMode[setup->LogMode]);
	PF2("%s=%s\n",	 	NM(FILEIOTYPE), Names_FileIOType[setup->FileIOType]);
	PF2("%s=%ld\n",	 	NM(BUFFERSIZE), setup->BufferSize);
	PF2("%s=\"%s\"\n",	NM(HOTKEY),	  	setup->HotKey);
	PF2("%s=\"%s\"\n",	NM(SCREENNAME), setup->ScreenName);
	PF2("%s=\"%s\"\n",	NM(LOGNAME),	setup->LogFile);
	PF2("%s=\"%s\"\n",	NM(WINDOWFONT), GetFontDesc(tempdesc,
													setup->WindowFont,
												 	setup->WinFontSize));
	PF2("%s=\"%s\"\n",	NM(BUFFERFONT), GetFontDesc(tempdesc,
													setup->BufferFont,
												 	setup->BufFontSize));
	PF2("%s=\"%s\"\n",	NM(FORMAT),     setup->BufferFormat);
	PF2("%s=\"%s\"\n",	NM(LOGFORMAT),  setup->LogfileFormat);

	PF2("%s=%ld\n",		NM(TEXTSPACING), CurSettings.TextSpacing);

	PF1("%s\n",		 	CurSettings.SimpleRefresh ? NM(SIMPLEREFRESH) :
												    NM(SMARTREFRESH));
	PF1("%s\n",		 	CurSettings.RightAlign    ? NM(RIGHTALIGNED) :
												    NM(LEFTALIGNED));
	PF2("%s=%s\n",      NM(ROWQUALIFIER), Names_RowQualifier[RowQual]);
	PF1("%s\n",		 	CurSettings.ShowStatus    ? NM(SHOWSTATUS)	  :
												    NM(HIDESTATUS));
	PF1("%s\n",		 	CurSettings.ShowGadgets   ? NM(SHOWGADGETS)   :
												    NM(HIDEGADGETS));
	PF2("%s%s\n",		(AutoOpen ? "" : "No"), NM(AUTOOPEN));
	PF2("%s%s\n",       (DisableOnHide ? "" : "No"), NM(DISABLEWHENHIDDEN));
	PF2("%s%s\n",		(CurSettings.MakeIcons ? "" : "No"), NM(CREATEICONS));
	
	FPrintf(file, "%s=%ld,%ld\n", NM(MAINPOS),	CurSettings.MainWinLeft,
											  	CurSettings.MainWinTop);
	FPrintf(file, "%s=%ld,%ld\n", NM(MAINSIZE),	CurSettings.MainWinWidth,
											  	CurSettings.MainWinHeight);
	FPrintf(file, "%s=%ld,%ld\n", NM(FORMPOS),	CurSettings.FormWinLeft,
											  	CurSettings.FormWinTop);
	FPrintf(file, "%s=%ld,%ld\n", NM(FUNCPOS),	CurSettings.FuncWinLeft,
											  	CurSettings.FuncWinTop);
	FPrintf(file, "%s=%ld,%ld\n", NM(SETPOS),	CurSettings.SetupWinLeft,
											  	CurSettings.SetupWinTop);
	FPrintf(file, "%s=%ld,%ld\n", NM(ICONPOS),  CurSettings.IconPosLeft,
												CurSettings.IconPosTop);
	FPrintf(file, "%s=%ld\n",     NM(STACKLIMIT), CurSettings.StackLimit);
	FPrintf(file, "%s=%ld\n",     NM(TASKPRI),
								  SysBase->ThisTask->tc_Node.ln_Pri);
	Close(file);
	if (CreateIcons && saveicon != SAVE_NOICON)
		WriteIcon(filename);
	EnableAllWindows();
	UpdateStatus();
	GotLastSaved = 1;		/* Indicate "Last Saved" can now be performed */
	return (1);
}

/*
 *		LoadConfig(filename, mode, set)
 *
 *		Attempts to load the specified configuration file into memory.
 *		Returns 1 for success (having executed all the commands, and updated
 *		windows etc. as appropriate) or 0 for failure (couldn't open file).
 *
 *		To prevent infinite loops, we only allow three levels of nested
 *		configuration files (i.e. a maximum of three recursive calls to
 *		LoadConfig)
 *
 *		Normally, set is NULL. If non-NULL, then it represents a set
 *		of settings currently being updated. In this case, LoadConfig
 *		doesn't actually install the new settings when it's finished
 *		reading them, it merely updates the values in set with the
 *		new values and relies on the caller to set them later on.
 *
 *		The mode parameter indicates how we are being called -- from
 *		the CLI, Workbench, ARexx or internally. This is used to determine
 *		how error messages should be displayed (currently, messages will
 *		only be displayed in a CLI window).
 *
 *		New: we now support reading from an interactive file (CON:....)
 *		This allows the user to open a command window by trying to load
 *		a suitable CON: specification. Commands will be executed as soon
 *		as they arrive, and settings will be updated immediately.
 */
int LoadConfig(char *filename, int mode, Settings *set)
{
	static int nestcount;

	Settings newsettings;
	Settings *myset;
	int  interactive;
	int  linenum   = 0;
	int  retvalue  = EXEC_FAIL;
	BPTR errorfile = NULL;
	BPTR file;
	char linebuf[200];

	if (set == NULL) {
		myset  		= &newsettings;
		*myset		= CurSettings;
		UpdateFlags = 0;
	} else {
		myset = set;
	}
	if (nestcount >= MAX_LOAD_NESTING)
		return (EXEC_FAIL);
	nestcount++;
	
	DisableAllWindows();
	mysprintf(StatusLineText, MSG(MSG_STATUS_LOADSET), filename);
	ShowStatus(StatusLineText);

	file = Open(filename, MODE_OLDFILE);
	if (!file)
		goto abort_load;

	/*
	 *		If we're executing this command from a CLI, then display
	 *		error messages in the CLI window, else don't display them
	 *		at all.
	 */
	if (mode == MODE_CMDLINE)
		errorfile = Output();

	interactive = IsInteractive(file);
	if (interactive) {
		errorfile = file;	/* Send errors to the command window */
	} else {
		/*
		 *		If we're not loading settings from an interactive
		 *		window, then check that the first line of the file
		 *		contains the settings file identifier
		 */
		if (!FGets(file, linebuf, 199)							||
			strnicmp(linebuf, ConfigID, strlen(ConfigID)) != 0)
		{
			goto abort_load;
		}
		linenum++;
	}

	/*
	 *		Now read in lines from the command file and execute them.
	 */
	if (interactive) {
		FPrintf(errorfile, "%s%s", MSG(MSG_CMD_HEADER), MSG(MSG_CMD_PROMPT));
		Flush(errorfile);
	}
	while (FGets(file, linebuf, 199) != NULL) {
		char *cmdline = linebuf;
		char *q;
		int result;

		linenum++;
		for (q = cmdline; *q && *q != '\n'; q++)
			;
		if (*q)
			*q++ = '\0';
	
		while (*cmdline == ' ' || *cmdline == '\t')
			cmdline++;

		if (!*cmdline || *cmdline == ';')
			continue;	/* Skip over lines with no text on them */

		if (stricmp(cmdline, "exit") == 0)	/* Skip rest of file */
			break;

		if (interactive && *cmdline == '?') {
			ShowCommands(errorfile);
			goto show_prompt;
		}
			
		/*
		 *		Now execute the command ... interactive commands are
		 *		executed internally, to allow QUIT to be executed.
		 */
		result = ExecCommand(cmdline,
							 (interactive ? MODE_INTERNAL : MODE_SETTINGS),
							 myset);
		if (QuitFlag)
			break;

		if (result == EXEC_OKAY) {
			if (interactive) {
				/*
				 *		For interactive commands, we update the
				 *		settings at each step along the way so
				 *		the user can see them taking effect.
				 */
				InstallSettings(myset, UpdateFlags);
				UpdateFlags = 0;
				*myset = CurSettings;
			}
		} else if (errorfile) {
			int msgid = 0;

			switch (result) {
				case EXEC_UNKNOWN:	msgid = MSG_CMD_UNKNOWN;	break;
				case EXEC_FAIL:		msgid = MSG_CMD_FAIL;		break;
				case EXEC_NOPARAM:	msgid = MSG_CMD_NOPARAM;	break;
			}
			if (!interactive)
				FPrintf(errorfile, "%s, line %ld: ", filename, linenum);
			FPrintf(errorfile, MSG(msgid), cmdline);
		}

show_prompt:
		if (interactive) {
			FPrintf(errorfile, MSG(MSG_CMD_PROMPT));
			Flush(errorfile);
		}
	}
	retvalue = EXEC_OKAY;

	if (interactive) {
		/*
		 *		For interactive use, we clear any CTRL-C typed by
		 *		the user during usage; this avoids confusion since
		 *		the CTRL-C wouldn't have taken effect immediately
		 *		anyway.
		 */
		CheckSignal(SIGBREAKF_CTRL_C);
	}
	/*
	 *		If we just successfully loaded a file, it was probably the
	 *		defaults file, so enable the "Last Saved" menu option (if it's
	 *		not enabled, then it doesn't try to load a file since there was
	 *		no file ever saved).
	 */
	GotLastSaved = 1;

abort_load:
	if (file)
		Close(file);
	nestcount--;
	if (set == NULL)
		InstallSettings(myset, UpdateFlags);

	UpdateStatus();
	EnableAllWindows();
	return (retvalue);
}

/*
 *		InitRexxPort
 *
 *		Attempts to create SnoopDos's public port and initialise SnoopPort
 *		and RexxPortMask accordingly. If the port already exists, still
 *		returns TRUE but SnoopPort will be NULL.
 *
 *		If we can't create the port for some reason, returns FALSE.
 */		
int InitRexxPort(void)
{
	struct MsgPort *port;

	Forbid();
	port = FindPort(PORT_NAME);
	if (!port) {
		SnoopPort = CreateMsgPort();
		if (!SnoopPort) {
			Permit();
			return (FALSE);
		}
		SnoopPort->mp_Node.ln_Name = PORT_NAME;
		SnoopPort->mp_Node.ln_Pri  = 1;	/* Speed up searches */
		AddPort(SnoopPort);
		RexxPortMask = 1 << SnoopPort->mp_SigBit;
	}
	Permit();
	return (TRUE);
}

/*
 *		CleanupRexxPort()
 *
 *		Cleans up our SnoopDos port -- removes it from the public port
 *		list, and then replies to any messages it has outstanding. Call
 *		before exiting.
 */
void CleanupRexxPort(void)
{
	if (SnoopPort) {
		struct RexxMsg *msg;

		Forbid();
		RemPort(SnoopPort);
		while ((msg = (struct RexxMsg *)GetMsg(SnoopPort)) != NULL) {
			msg->rm_Result1 = RC_FATAL;
			msg->rm_Result2 = 0;
			ReplyMsg((struct Message *)msg);
		}
		Permit();
		DeleteMsgPort(SnoopPort);
		SnoopPort = NULL;
	}
	if (RemoteReplyPort) {
		DeleteMsgPort(RemoteReplyPort);
		RemoteReplyPort = NULL;
	}
}

/*
 *		HandleRexxMsgs()
 *
 *		Handles any new messages at our Rexx port
 */
void HandleRexxMsgs(void)
{
	Settings newsettings = CurSettings;
	struct RexxMsg *msg;

	UpdateFlags = 0;
	while ((msg = (struct RexxMsg *)GetMsg(SnoopPort)) != NULL) {
		msg->rm_Result1 = RC_OK;
		msg->rm_Result2 = 0;
		if ((msg->rm_Action & RXCODEMASK) == RXCOMM) {
			/*
			 *		Got a valid ARexx message, now process it
			 */
			switch (ExecCommand(msg->rm_Args[0], MODE_REXX, &newsettings)) {
				/*
				 *		See include:rexx/errors.h for the meanings of
				 *		these Rexx return codes
				 */
				case EXEC_NOPARAM:	msg->rm_Result1 = 10; break;
				case EXEC_FAIL:		msg->rm_Result1 = 20; break;
				case EXEC_UNKNOWN:	msg->rm_Result1 = 30; break;
			}
		}
		ReplyMsg((struct Message *)msg);
	}
	InstallSettings(&newsettings, UpdateFlags);
}

/*
 *		SendRemote(cmdline, mode)
 *
 *		Sends a command to a remote copy of SnoopDos running in the
 *		background. The mode is MODE_CMDLINE or MODE_TOOLTYPE. The
 *		difference is that for MODE_TOOLYPE, HIDE commands are ignored.
 *
 *		In both cases, the command is first checked against the command
 *		table to make sure it's valid. If it's invalid, then if the mode
 *		is MODE_CMDLINE, an appropriate error message is printed.
 */
void SendRemote(char *cmdline, int mode)
{
	struct Command *cmd;
	struct MsgPort *ourport;
	char cmdname[100];
	char param[100];
	int boolvalue;
	int cid;

	cid = ParseCommand(cmdline, cmdname, param, &boolvalue, &cmd);
	switch (cid) {
		case CMD_UNKNOWN:
			if (mode == MODE_CMDLINE)
				Printf(MSG(MSG_ERROR_CLI_UNKNOWN), cmdline);
			return;

		case CMD_NOPARAM:
			if (mode == MODE_CMDLINE)
				Printf(MSG(MSG_ERROR_CLI_NOPARAM), cmdline);
			return;

		case CMD_HIDE:
			/*
			 *		Don't send a HIDE command to the current SnoopDos if
			 *		it was in an icon's tooltypes
			 */
			if (mode == MODE_TOOLTYPE)
				return;
			break;

		case CMD_END:
			return;
	}
	
	/*
	 *		Got a command that's okay to send. Now locate SnoopDos port
	 *		and send it. We have to locate the port each time, in case
	 *		the user quits the background copy suddenly.
	 */
	if (!RemoteReplyPort) {
		RemoteReplyPort = CreateMsgPort();
		if (!RemoteReplyPort)
			return;
	}
	Forbid();
	ourport = FindPort(PORT_NAME);
	if (ourport) {
		struct RexxMsg msg;

		msg.rm_Action  			 = RXCOMM;
		msg.rm_Args[0]			 = cmdline;
		msg.rm_Node.mn_ReplyPort = RemoteReplyPort;

		PutMsg(ourport, (struct Message *)&msg);
		WaitPort(RemoteReplyPort);
		GetMsg(RemoteReplyPort);
		Permit();
		if (mode == MODE_CMDLINE && msg.rm_Result1 >= RC_ERROR)
			Printf(MSG(MSG_ERROR_CLI_FAILED), cmdline);
	} else
		Permit();
}

/*
 *		ShowCommands(file)
 *
 *		Prints a neatly formatted list of recognised commands to
 *		the specified output file.
 */
void ShowCommands(BPTR file)
{
	int i;
	int numrows = (NUM_CMDNAMES + 3) / 4;

	FPrintf(file, MSG(MSG_CLI_HELPBANNER));
	for (i = 0; i < numrows; i++) {
		int j;

		if (CheckSignal(SIGBREAKF_CTRL_C)) {
			FPrintf(file, "^C\n");
			break;
		}
		for (j = i; j < NUM_CMDNAMES; j += numrows) {
			char *pmsg = (CommandTable[j].numparms > 0) ? "*" : " ";
			char *cmsg = CommandTable[j].name;

			if (j < (NUM_CMDNAMES - numrows))
				FPrintf(file, "%s%-18s", pmsg, cmsg);
			else
				FPrintf(file, "%s%s", pmsg, cmsg);
		}
		FPrintf(file, "\n");
	}
}

/*
 *		ParseStartupOpts()
 *
 *		Handles the startup options, either on the command line or via
 *		the tooltypes, and sets the default options accordingly.
 *
 *		We handle a couple of options specially (LOCALE and SETTINGS) since
 *		these must be set before doing anything else at all (we can't even
 *		print a meaningful error message without LOCALE, for example.)
 *
 *		After those, we read in our configuration file from disk, and then
 *		parse the remaining items which can thus override any of the options
 *		specified in the config file. This applies even to tooltypes, so if
 *		an icon has a certain option hardcoded into it, this will always
 *		override a config file with the same option set.
 *
 *		Normally, CLI and Tooltype options are only used to control things
 *		like opening a default logfile, set the Commodity priority or config
 *		file name, making SnoopDos start in the background, or automatically
 *		opening one or more windows.
 *
 *		As a final step, if we spot that we are not the main instance of
 *		SnoopDos, we send all the new commands to the background copy instead.
 *		 We also signal the background copy to come to the foreground by
 *		default (Workbench) or if no other options are specified (CLI).
 *
 *		Returns TRUE for success, FALSE for failure.
 *
 */
int ParseStartupOpts(int argc, char **argv)
{
	struct Settings newset = DefaultSettings;
	struct DiskObject *IconCache[20];
	struct DiskObject *dobj = NULL;
	struct WBArg *wbarg;
	char msg[200];
	int success = 1;
	int maxargs;
	int i;
	int gotsettings = 0;		/* If true, settings keyword specified */

	/*
	 *		We start off in a special disabled state (not 0, not 1)
	 *		to ensure no events get monitored during our initialisation.
	 *		We use a special state so that we can detect if the user
	 *		specifies Disabled = Yes/No during startup.
	 */
	Disabled = 2;		/* Start up in disabled state */

	if (WBenchMsg) {
		/*
		 *		Starting up from Workbench.
		 */
		if (!IconBase)
			return (FALSE);

		maxargs = min(WBenchMsg->sm_NumArgs, 20);

		for (i = 0, wbarg = WBenchMsg->sm_ArgList; i < maxargs; i++, wbarg++) {
			BPTR olddir;

			IconCache[i] = NULL;
			if (wbarg->wa_Lock && *wbarg->wa_Name) {
				olddir = CurrentDir(wbarg->wa_Lock);
				dobj   = GetDiskObject(wbarg->wa_Name);
				if (dobj) {
					char *param;
					
					param = FindToolType(dobj->do_ToolTypes,
										 CmdNames[CMD_SETTINGS]);
					if (param && *param) {
						strcpy(DefaultConfigName, param);
						gotsettings = 1;
					}
					param = FindToolType(dobj->do_ToolTypes,
										 CmdNames[CMD_LANGUAGE]);
					if (param && *param)
						strcpy(Language, param);

					param = FindToolType(dobj->do_ToolTypes,
										 CmdNames[CMD_PATCHRAMLIB]);
					if (param && stricmp(param, "no") == 0)
						NoPatchRamLib = 1;

					IconCache[i] = dobj;
				}
				CurrentDir(olddir);
			}
		}
	} else {
		/*
		 *		Starting up from the CLI so scan the command line using
		 *		the passed-in arguments.
		 */
		if (argc > 1) {
			if (argv[1][0] == '-' || argv[1][0] == '?') {
				Printf(MSG(MSG_CLI_USAGE), CommodityTitle, argv[0]);
				return (FALSE);
			}
		}

		/*
		 *		Do a prescan of the command line looking
		 *		for SETTINGS, LANGUAGE or PATCHRAMLIB, keywords.
		 */
		for (i = 1; i < argc; i++) {
			char cmdname[50];
			char param[150];
			char newcmd[200];
			int boolvalue;
			struct Command *cmd;
			int cid;

			cid = ParseCommand(argv[i], cmdname, param, &boolvalue, &cmd);
			if (cid == CMD_NOPARAM && ((i + 1) < argc)) {
				/*
				 *		Indicates we didn't get enough parameters for our
				 *		command. Let's grab the next option off the command
				 *		line instead and see if that makes sense.
				 */
				mysprintf(newcmd, "%s %s", argv[i], argv[i+1]);
				cid = ParseCommand(newcmd, cmdname, param, &boolvalue, &cmd);
				i++;	/* Skip over next parameter */
			}
			switch (cid) {
				case CMD_SETTINGS:
					strcpy(DefaultConfigName, param);
					gotsettings = 1;
					break;

				case CMD_LANGUAGE:
					strcpy(Language, param);
					break;

				case CMD_PATCHRAMLIB:
					NoPatchRamLib = !boolvalue;
					break;
			}
		}
	}

	/*
	 *		Now initialise locale according to the language chosen,
	 *		and read in our default configuration file.
	 */
	InitLocale(Language);
	InitMenus();
	/*
	 *		Note that LoadConfig() and further ExecCommand() calls may
	 *		cause the settings to be installed at any time, and UpdateFlags
	 *		to be reset to null. Thus, it is important to ensure that
	 *		we initialise them correctly in the first place.
	 */
	UpdateFlags = SET_ALL;
	if (SnoopPort) {
		/*
		 *		We only initialise our patches if we're the only task
		 *		running at the moment. We leave the initialisation
		 *		until now so that we can control whether or not our
		 *		ramlib patch gets installed.
		 */
		if (!InitPatches()) {
			if (WBenchMsg)
				ShowError(MSG(MSG_ERROR_INITPATCHES));
			else
				Printf("%s\n", MSG(MSG_ERROR_INITPATCHES));
			Cleanup(40);
		}
		if (!gotsettings) {
			/*
			 *		If we haven't yet got a settings file, then try a number of
			 *		possible locations (in order of priority). If we find a
			 *		match, we overwrite the default config name with the new
			 *		name, for future use by LoadConfig and SaveConfig.
			 */
			static char *defconfig[] = {
				"PROGDIR:" SETTINGS_BASENAME,
				"S:"       SETTINGS_BASENAME,
				"ENVARC:"  SETTINGS_BASENAME,
				NULL
			};
			APTR oldwinptr = *TaskWindowPtr;
			char **pdefname;

			*TaskWindowPtr = (APTR)-1;
			for (pdefname = defconfig; *pdefname; pdefname++) {
				BPTR lk = Lock(*pdefname, ACCESS_READ);

				if (lk) {
					UnLock(lk);
					break;
				}
			}
			if (!*pdefname) {
				/*
				 *		Choose a good default name. We try for ENVARC:
				 *		initially, but if that fails, fall back on S:
				 */
				if (IsFileSystem(defconfig[2]))
					pdefname = &defconfig[2];
				else
					pdefname = &defconfig[1];
			}
			strcpy(DefaultConfigName, *pdefname);
			strcpy(ConfigFileName,	  *pdefname);
			*TaskWindowPtr = oldwinptr;
		} else {
			/*
			 *		Do a quick check to see if the filename specified
			 *		using the Settings keyword exists -- if it doesn't,
			 *		then don't bother to try loading it (this avoids
			 *		a requester being shown unnecessarily).
			 */
			BPTR lk = Lock(DefaultConfigName, ACCESS_READ);

			if (lk)
				UnLock(lk);
			else
				gotsettings = 0;
		}

		/*
		 *		Now, one way or another, we have the default config filename
		 *		set up so try and load it.
		 */
		if (!LoadConfig(DefaultConfigName,
						(WBenchMsg ? MODE_TOOLTYPE : MODE_CMDLINE), &newset)
			&& gotsettings)
		{
			ShowError(MSG(MSG_ERROR_LOADING_SETTINGS), DefaultConfigName);
		}
	}

	/*
	 *		Finally, lets parse the remaining startup options and/or icons
	 */
	if (WBenchMsg) {
		/*
		 *		Scan all the config files (if any) that were passed as
		 *		parameters via icons.
		 */
		wbarg = WBenchMsg->sm_ArgList + 1;
		for (i = 1; i < WBenchMsg->sm_NumArgs; i++, wbarg++) {
			char *name = wbarg->wa_Name;

			if (SnoopPort) {
				BPTR olddir = CurrentDir(wbarg->wa_Lock);

				LoadConfig(name, MODE_INTERNAL, &newset);
				CurrentDir(olddir);
			} else {
				/*
				 *		There's a background copy running so we need
				 *		to tell it to load the config file instead.
				 */
				char *p;

				mysprintf(msg, "%s ", CmdNames[CMD_LOADSETTINGS]);
				p = msg + strlen(msg);
				NameFromLock(wbarg->wa_Lock, p, 150);
				AddPart(p, wbarg->wa_Name, 150);
				SendRemote(msg, MODE_TOOLTYPE);
			}
		}

		/*
		 *		Rescan the icons, this time processing all the tooltypes that
		 *		we recognise.  Any we don't recognise are ignored. SETTINGS
		 *		command may get parsed a second time, but that's okay.
		 */
		for (i = 0; i < maxargs; i++) {
			dobj = IconCache[i];
			if (dobj) {
				char **tooltypes;

				for (tooltypes = (char **)dobj->do_ToolTypes; *tooltypes; tooltypes++) {
					if (SnoopPort)
						ExecCommand(*tooltypes, MODE_TOOLTYPE, &newset);
					else
						SendRemote(*tooltypes, MODE_TOOLTYPE);
				}
				FreeDiskObject(dobj);
			}
		}
		if (!SnoopPort)
			SendRemote(CmdNames[CMD_SHOW], MODE_CMDLINE);
	} else {
		/*
		 *		Do a proper scan of the CLI arguments.
		 */
		for (i = 1; i < argc; i++) {
			char newcmd[200];
			char *thiscmd = argv[i];
			struct Command *cmd;
			int boolvalue;
			int cid;

			/*
			 *		Check if the command we're checking needs more than
			 *		one parameter. If so, and if it wasn't supplied, we
			 *		assume that the parameter after it on the command
			 *		line is to be used. This means the user doesn't have
			 *		to specifically type the '='.
			 *
			 *		HELP is a special case, since HELP will work with zero
			 *		parameters too. We only try and handle help if there
			 *		is at least one more parameter after it on the command
			 *		line. If there isn't, then we print a list of supported
			 *		commands instead.
			 */
			cid = ParseCommand(thiscmd, newcmd, newcmd+50, &boolvalue, &cmd);
			if ( (i+1) < argc && (cid == CMD_HELP ||
								  (cid == CMD_NOPARAM && cmd->numparms > 0)) )
			{
				/*
				 *		We didn't have a parameter for this command, so
				 *		assume the next parameter on the command line is
				 *		the parameter instead.
				 */
				mysprintf(newcmd, "%s %s", argv[i], argv[i+1]);
				thiscmd = newcmd;
				i++;
			} else if (cid == CMD_HELP) {
				/*
				 *		Didn't have anything following the HELP so
				 *		print CLI help instead and exit immediately.
				 */
				ShowCommands(Output());
				return (0);
			}
			if (SnoopPort) {
				switch (ExecCommand(thiscmd, MODE_CMDLINE, &newset)) {
					case EXEC_FAIL:
						Printf(MSG(MSG_ERROR_CLI_FAILED), thiscmd);
						success = 0;
						break;

					case EXEC_NOPARAM:
						Printf(MSG(MSG_ERROR_CLI_NOPARAM), thiscmd);
						success = 0;
						break;

					case EXEC_UNKNOWN:
						Printf(MSG(MSG_ERROR_CLI_UNKNOWN), thiscmd);
						success = 0;
						break;

					// case EXEC_OKAY:
				}
			} else {
				SendRemote(thiscmd, MODE_CMDLINE);
			}
		}
		if (!SnoopPort && argc == 1)
			SendRemote(CmdNames[CMD_SHOW], MODE_CMDLINE);
	}
	if (!SnoopPort)
		success = 0;

	/*
	 *		Now check if the user specified Disable=YES (or NO) during
	 *		startup. If they didn't, then reset the disable flag.
	 *		Regardless, we want to update the function flags to reflect
	 *		the current disable state.
	 */
	if (Disabled == 2)
		SetMonitorMode(MONITOR_NORMAL);

	if (success)
		InstallSettings(&newset, UpdateFlags | SET_FUNC);

	return (success);
}
