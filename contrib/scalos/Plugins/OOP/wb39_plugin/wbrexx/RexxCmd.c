// RexxCmd.c
// $Date$
// $Revision$


#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>

#include <graphics/text.h>
#include <workbench/workbench.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <libraries/amigaguide.h>
#include <libraries/locale.h>

#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <clib/alib_protos.h>

#include <proto/rexxsyslib.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/wb.h>
#include <proto/icon.h>
#include <proto/amigaguide.h>
#include <proto/locale.h>

#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>


//#include "wb39.h"
#include "wbrexx.h"
#include "Scalos_Helper.h"
//#include "wb39proto.h"


static LONG OpenWBHelpFile(void);
static SAVEDS(void) WBHelpProcess(void);

#if !defined(__SASC) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
static size_t stccpy(char *dest, const char *src, size_t MaxLen);
#endif /* !defined(__SASC) && !defined(__MORPHOS__)  */

// aus wb39.c
extern struct ScaRootList *rList;

extern T_UTILITYBASE UtilityBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *LayersBase;
extern struct Library *WorkbenchBase;
extern struct Library *IconBase;


#if 0
------------------------------------------------------------------------------
ACTIVATEWINDOW command

Purpose:

This command will attempt to make a window the active one.

Format:

ACTIVATEWINDOW [WINDOW] <ROOT|Drawer name>

Template:

ACTIVATEWINDOW WINDOW

Parameters:

WINDOW

Either ROOT to activate the Workbench root window (where volume icons and
AppIcons live) or the fully qualified name of a drawer window to activate.
Note that the drawer window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

Errors:

10 - If the named window cannot be activated. The error code will be placed
in the WORKBENCH.LASTERROR variable.

Result:

-

Notes:

If you choose to have a window activated that is not the root window you must
make sure that the window name is given as a fully qualified path name. For
example Work: is a fully qualified name, and so is SYS:Utilities.
Devs/Printers would not be a fully qualified name. A fully qualified name
always contains the name of an assignment, a volume or a device.

Example:

/* Activate the root window. */ ADDRESS workbench
ACTIVATEWINDOW root

/* Activate the Work: partition`s window. */
ACTIVATEWINDOW ,Work:`
------------------------------------------------------------------------------
#endif
LONG Cmd_ActivateWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	ActivateWindow(swi->ws_Window);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}



#if 0
------------------------------------------------------------------------------
CHANGEWINDOW command

Purpose:

This command will attempt to change the size and the position of a window.

Format:

CHANGEWINDOW [WINDOW] <ROOT|ACTIVE|Drawer name> [[LEFTEDGE] <new left edge
position>][[TOPEDGE] <new top edge position>][[WIDTH] <new window
width>][[HEIGHT] <new window height>]

Template:

CHANGEWINDOW WINDOW,LEFTEDGE/N,TOPEDGE/N,WIDTH/N,HEIGHT/N

Parameter:

WINDOW

Either ROOT to resize/move the Workbench root window (where volume icons and
AppIcons live), ACTIVE to change the currently active Workbench window or
the fully qualified name of a drawer window to change. Note that the drawer
window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

LEFTEDGE

New left edge window position.

TOPEDGE

New top edge window position.

WIDTH

New window width.

HEIGHT

New window height.

Errors:

10 - If the named window cannot be changed; this can also happen if you
specified ACTIVE as the window name and none of the Workbench windows is
currently active. The error code will be placed in the WORBENCH.LASTERROR
variable.

Result:

-

Notes:

If you choose to have a window changed that is neither the root nor the
active window you must make sure that the window name is given as a fully
qualified path name. For example Work: is a fully qualified name, and so is
SYS:Utilities. Devs/Printers would not be a fully qualified name. A fully
qualified name always contains the name of an assignment, a volume or a
device.

Example:

/* Change the root window; move it to position 10,30. * and change its size
to 200100 pixels. */
ADDRESS workbench
CHANGEWINDOW root LEFTEDGE 10 TOPEDGE 30 WIDTH 200 HEIGHT 100

/* Change the currently active window. */
CHANGEWINDOW active 20 40 200 100


------------------------------------------------------------------------------
#endif
LONG Cmd_ChangeWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_LEFTEDGE, ARG_TOPEDGE, ARG_WIDTH, ARG_HEIGHT };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;
	WORD Left, Top, Width, Height;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (Args[ARG_LEFTEDGE])
		Left = *((LONG *) Args[ARG_LEFTEDGE]);
	else
		Left = swi->ws_Window->LeftEdge;
	if (Args[ARG_TOPEDGE])
		Top = *((LONG *) Args[ARG_TOPEDGE]);
	else
		Top = swi->ws_Window->TopEdge;
	if (Args[ARG_WIDTH])
		Width = *((LONG *) Args[ARG_WIDTH]);
	else
		Width = swi->ws_Window->Width;
	if (Args[ARG_HEIGHT])
		Height = *((LONG *) Args[ARG_HEIGHT]);
	else
		Height = swi->ws_Window->Height;

	ChangeWindowBox(swi->ws_Window, Left, Top, Width,Height);

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
DELETE command

Purpose:

This command is for deleting files and drawers (and their contents).

Format:

DELETE [NAME] <File or drawer name> [ALL]

Template:

DELETE NAME/A,ALL/S

Parameters:

NAME

Name of the file or drawer or volume to delete.

ALL

If the object in question is a drawer, attempt to delete the contents of the
drawer as well as the drawer itself. If this option is not specified, the
DELETE command will only attempt to delete the drawer itself, which may fail
if the drawer is not yet empty.

Errors:

10 - If the named file, drawer or volume could not be found or could not be
deleted.

Result:

-

Notes:

The file name given must be an absolute path, such as in RAM:Empty. A
relative path, such as /fred/barney will not work.

Example:

/* Delete the contents of the drawer RAM:Empty. */

ADDRESS workbench
DELETE ,RAM:Empty` ALL


------------------------------------------------------------------------------
#endif
LONG Cmd_Delete(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_NAME, ARG_ALL };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Args[ARG_NAME]);)

	if (Args[ARG_ALL])
		{
		if (DeleteDirectory(Args[ARG_NAME]))
			{
			// Object deleted, now delete icon
			if (!DeleteDiskObject(Args[ARG_NAME]))
				{
				Result = IoErr();

				// ignore errors from missing icons
				if (ERROR_OBJECT_NOT_FOUND == Result)
					Result = RETURN_OK;
				}
			}
		else
			Result = IoErr();
		}
	else
		{
		if (DeleteFile(Args[ARG_NAME]))
			{
			// Object deleted, now delete icon
			if (!DeleteDiskObject(Args[ARG_NAME]))
				{
				Result = IoErr();

				// ignore errors from missing icons
				if (ERROR_OBJECT_NOT_FOUND == Result)
					Result = RETURN_OK;
				}
			}
		else
			Result = IoErr();
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
FAULT command

Purpose:

This command will return a human readable explanation corresponding to an
error code.

Format:

FAULT [CODE] <Error code>

Template:

FAULT CODE/A/N

Parameters:

CODE

Error code to return a human readable explanation for.

Errors:

-

Result:

-

Example:

/* Query the error message corresponding to error code #205. */
ADDRESS workbench
OPTIONS RESULTS
FAULT 205
SAY result


------------------------------------------------------------------------------
#endif
LONG Cmd_Fault(APTR UserData,struct RexxMsg *Message,STRPTR *Args)
{
	enum	{ ARG_NUMBER };

	UBYTE ErrorText[100];
	LONG Number;

	Number = *(LONG *)Args[ARG_NUMBER];

	Printf("FAULT %d",Number);
	Printf("\n");

		/* Obtain the AmigaDOS error text corresponding
		 * to the error code. In your own applications
		 * you would want to supply descriptive texts
		 * for each error code your program supports
		 * in addition to the standard AmigaDOS set.
		 */

	if(Fault(Number,"",(STRPTR)ErrorText,99) > 0)
	{
		ReturnRexxMsg(Message,(STRPTR)&ErrorText[2]);

		return(0);
	}
	else
		return(ERROR_OBJECT_NOT_FOUND);
}


#if 0
------------------------------------------------------------------------------
HELP command

Purpose:

This command can be used to open the online help and to obtain information on
the supported menus, commands and command parameters.

Format:

HELP [COMMAND <Command name>] [MENUS] [PROMPT]

Template:

HELP COMMAND/K,MENUS/S,PROMPT/S

Parameters:

COMMAND

Name of the command whose command template should be retrieved.

MENUS

Specify this parameter to retrieve a list of menu items currently available.

PROMPT

Specify this parameter to invoke the online help system.

If no parameter is provided, a list of supported commands will be returned.

Errors:

10 - If the named command is not supported by the ARexx interface. The error
code will be placed in the WORKBENCH.LASTERROR variable.

Result:

RESULT

The command template, list of menu items or commands, as specified in the
command parameters.

Example:

/* Retrieve the list of supported commands. */
ADDRESS workbench
OPTIONS results

HELP
SAY result

/* Retrieve the command template of the `GETATTR` command. */
HELP COMMAND getattr
SAY result

/* Retrieve the list of available menu items. */
HELP MENUS
SAY result


------------------------------------------------------------------------------
#endif
LONG Cmd_Help(APTR UserData,struct RexxMsg *Message,STRPTR *Args)
{
	enum	{ ARG_COMMAND,ARG_MENUS,ARG_PROMPT };

	struct LocalData *LocalData;
	struct CmdFunc *Table;
	STRPTR String;
	STRPTR Command;
	LONG Len;
	LONG i;

	if (Args[ARG_PROMPT])
		{
		return OpenWBHelpFile();
		}
	else
		{
		LocalData = (struct LocalData *)UserData;

		Table = LocalData->CommandTable;

		/* If no command is specified then we should
		 * return a list of all the commands this host
		 * supports.
		 */

		if(Args[ARG_COMMAND] == NULL)
		{
			/* Count the number of bytes to allocate
			 * for a list of all commands.
			 */
			for(i = 0, Len = 1 ; Table[i].CommandName ; i++)
				Len += strlen(Table[i].CommandName) + 1;

			/* Make room for the command list. */
			String = AllocVec(Len,MEMF_ANY);

			if(String == NULL)
				return(ERROR_NO_FREE_STORE);
			else
			{
				/* Start with an empty string. */
				String[0] = 0;

				/* Add all the commands to the list. */
				for(i = 0 ; Table[i].CommandName ; i++)
				{
					strcat(String,Table[i].CommandName);
					strcat(String," ");
				}

				/* Terminate the string. */
				String[Len - 1] = 0;

				/* Reply the RexxMsg and return the
				 * command list as the command result.
				 */
				ReturnRexxMsg(Message,String);

				/* Dispose of the temporary storage. */
				FreeVec(String);

				return(0);
			}
		}
		else
			{
			/* So we should return the argument template
			 * of a specific command.
			 */
			Command = Args[ARG_COMMAND];

			/* Try to find the command in the table. */
			for(i = 0 ; Table[i].CommandName ; i++)
				{
				/* Is this the command we are
				 * looking for?
				 */
				if(Stricmp(Table[i].CommandName,Command) == 0)
					{
					/* Get the argument template. */
					String = Table[i].Template;

					/* Is the template an empty
					 * string?
					 */
					if(String[0] == '\0')
						{
						/* Provide an empty template. */
						String = ",";
						}

					/* Reply the RexxMsg and return
					 * the template as the command
					 * result.
					 */
					ReturnRexxMsg(Message,String);

					return(0);
					}
				}

			/* No matching command was found. */
			return(ERROR_OBJECT_NOT_FOUND);
			}
		}
}


#if 0
------------------------------------------------------------------------------
INFO command

Purpose:

This command is for opening the Workbench icon information requester.

Format:

INFO [NAME] <File, drawer or volume name>

Template:

INFO NAME/A

Parameters :

NAME

Name of the file, drawer or volume to open the information window for.

Errors:

10 - If the named file, drawer or volume could not be found. The error code
will be placed in the WORKBENCH.LASTERROR variable.

Result:

-

Example:

/* Open the information window for SYS:. */
ADDRESS workbench

INFO NAME ,SYS:`


------------------------------------------------------------------------------
#endif
LONG Cmd_Info(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_NAME };
	LONG Result = RETURN_OK;
	BPTR fLock;
	char *ResultString = NULL;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Args[ARG_NAME]);)

	fLock = Lock(Args[ARG_NAME], ACCESS_READ);
	if (fLock)
		{
		WBInfo((BPTR)NULL, Args[ARG_NAME], rList->rl_internInfos->ii_Screen);
		UnLock(fLock);
		}
	else
		Result = ERROR_OBJECT_NOT_FOUND;

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
KEYBOARD command

Purpose:

This command can be used to bind ARexx commands to key combinations.

Format:

KEYBOARD [NAME] <Name of key combination> ADD|REMOVE [KEY <Key combination>]
[CMD <ARexx command>]

Template:

KEYBOARD NAME/A,ADD/S,REMOVE/S,KEY,CMD/F

Parameters:

NAME

Name of the key combination to add or remove. Each key combination must have
a name with which it is associated. The name must be unique.

ADD

This tells the KEYBOARD command to add a new keyboard combination. You will
also need to specify the KEY and CMD parameters.

REMOVE

This tells the KEYBOARD command to remove an existing keyboard combination.

KEY

The keyboard combination to add; this must be in the same format as used by
the Commodities programs.

CMD

This is the ARexx command to bind to the keyboard combination. The command
can either be the name of an ARexx script to execute or a short ARexx program
in a single line.

Errors:

10 - The command will fail if you tried to add a duplicate of an existing key
combination or if the key combination to remove does not exist. The error
code will be placed in the WORKBENCH.LASTERROR variable.

Result:

-

Example:

/* Bind an ARexx script to the [Control]+A key combination. * When pressed,
this will cause the ARexx script by the name * test.wb to be executed. ARexx
will search for that program * in the REXX: directory. If no test.wb file
can * be found, ARexx will attempt to execute a script * by the name of
test.rexx. */

ADDRESS workbench

KEYBOARD ADD NAME test1 KEY ,ctrl a, CMD ,test`

/* Bind an ARexx script to the [Alt]+[F1] key combination. * When pressed,
this will cause a short inline program to be * executed. */
KEYBOARD ADD NAME test2 KEY ,alt f1, CMD `say 42

/* Bind an ARexx script to the [Shift]+[Help] key combination. * When
pressed, this will cause the Workbench About menu item * to be invoked. */
KEYBOARD ADD NAME test3 KEY ,shift help, CMD `MENU INVOKE WORKBENCH.ABOUT

/* Remove the first key combination we added above. */
KEYBOARD REMOVE NAME test1


------------------------------------------------------------------------------
#endif
LONG Cmd_Keyboard(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_NAME, ARG_ADD, ARG_REMOVE, ARG_KEY, ARG_CMD };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;

	d1(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Args[ARG_NAME]);)

	if (NULL == Args[ARG_ADD] && NULL == Args[ARG_REMOVE])
		return ERROR_REQUIRED_ARG_MISSING;

	if (Args[ARG_ADD])
		{
		d1(kprintf(__FUNC__ "/%ld: ADD\n", __LINE__);)

		if (NULL == Args[ARG_NAME])
			return ERROR_REQUIRED_ARG_MISSING;
		if (NULL == Args[ARG_KEY])
			return ERROR_REQUIRED_ARG_MISSING;
		if (NULL == Args[ARG_CMD])
			return ERROR_REQUIRED_ARG_MISSING;

		Result = AddKeyboardCommand(Args[ARG_NAME], Args[ARG_KEY], Args[ARG_CMD]);
		}
	else if (Args[ARG_REMOVE])
		{
		d1(kprintf(__FUNC__ "/%ld: REMOVE\n", __LINE__);)

		if (NULL == Args[ARG_NAME])
			return ERROR_REQUIRED_ARG_MISSING;

		Result = RemoveKeyboardCommand(Args[ARG_NAME]);
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
LOCKGUI command

Purpose:

This command will block access to all Workbench drawer windows.

Format:

LOCKGUI

Template:

LOCKGUI

Parameters:

-

Errors:

-

Result:

-

Notes:

It takes as many UNLOCKGUI commands as there were LOCKGUI commands to make
the Workbench drawer windows usable again. In other words, the LOCKGUI
command nests.

Example:

/* Block access to all Workbench drawer windows. */
ADDRESS workbench

LOCKGUI


------------------------------------------------------------------------------
#endif
LONG Cmd_LockGUI(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	LONG Result = RETURN_OK;
	char *ResultString = NULL;

	/* !!! */

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
UNZOOMWINDOW command

Purpose:

This command will attempt return a window to its original position and
dimensions.

Format:

UNZOOMWINDOW [WINDOW] <ROOT|ACTIVE|Drawer name>

Template:

UNZOOMWINDOW WINDOW

Parameters:

WINDOW

Name of the window to operate on. ROOT will use the Workbench root window
(where volume icons and AppIcons live), ACTIVE will use the currently active
Workbench window. Any other fully qualified path name will use the drawer
window corresponding to the path. If no WINDOW parameter is specified, this
command will try to operate on the currently active Workbench window.

Errors:

10 - If the named window cannot be found. The error code will be placed in
the WORKBENCH.LASTERROR variable.

Result:

-

Example:

/* Change the root window. */
ADDRESS workbench

UNZOOMWINDOW root


------------------------------------------------------------------------------
#endif
LONG Cmd_UnzoomWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (swi->ws_Window->Flags & WFLG_HASZOOM)
		{
		if (swi->ws_Window->Flags & WFLG_ZOOMED)
			ZipWindow(swi->ws_Window);
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
MENU command

Purpose:

This command is for invoking items of the Workbench menu, as if the user had
selected them with the mouse and for adding/removing user menus.

Format:

MENU [WINDOW <Window name>] [INVOKE] <Menu name> [NAME <Menu name>] [TITLE
<Menu title>] [SHORTCUT <Menu shortcut>] [ADD|REMOVE] [CMD <ARexx command>]

Template:

MENU WINDOW/K,INVOKE,NAME/K,TITLE/K,SHORTCUT/K,ADD/S,REMOVE/S,CMD/K/F

Parameters:

The following set of parameters can be used solely for invoking menu items.

WINDOW

Name of the window whose menu should be invoked. This can be ROOT to work on
the Workbench root window (where volume icons and AppIcons live), ACTIVE to
work on the currently active Workbench window or the fully qualified name of
a drawer window. Note that the drawer window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

INVOKE

Name of the menu to invoke. See below for a list of available menu items.

The following set of parameters are for adding and removing menu items.

NAME

Name of the menu item to add or remove. Each menu item must have a name with
which it is associated. The name must be unique and has nothing to do with
the title of the item, as shown in the Tools menu.

TITLE

This is the text that will be used as the menu item title, as it will appear
in the Tools menu. This parameter is required if you ADD a new menu item.

SHORTCUT

When adding a new menu item, this will be the menu shortcut associated with
the item. Please note that the shortcut cannot be longer than a single
character and that it will be ignored if there already is an item in any of
the menus which uses this shortcut. This parameter is optional.

ADD

This tells the MENU command to add a new item to the Tools menu. When adding
a menu item you will also need to specify the NAME, TITLE and CMD parameters.

REMOVE

This tells the MENU command to remove a menu item previously added via the
ARexx interface. When removing a menu item you will also need to specify the
NAME parameter.

CMD

This is the ARexx command to bind to the new menu item. The command can
either be the name of an ARexx script to execute or a short ARexx program in
a single line.

Menu items:

WORKBENCH.BACKDROP

Toggles the Workbench Backdrop window switch.

WORKBENCH.EXECUTE

Invokes the Workbench Execute Command requester. The user will be prompted
to enter the command to be executed.

WORKBENCH.REDRAWALL

Invokes the Workbench Redraw All function.

WORKBENCH.UPDATEALL

Invokes the Workbench Update All function.

WORKBENCH.LASTMESSAGE

Redisplays the last Workbench error message.

WORKBENCH.ABOUT

Displays the Workbench About... requester.

WORKBENCH.QUIT

Attempts to close Workbench; this may bring up a requester the user will have
to answer.

WINDOW.NEWDRAWER

Prompts the user to enter the name of a new drawer to be created.

WINDOW.OPENPARENT

If possible, this will open the parent directory of the drawer the command
operates on.

WINDOW.CLOSE

If possible, this will close the drawer the command operates on.

WINDOW.UPDATE

This will update the drawer the command operates on, i.e. the contents will
be reread.

WINDOW.SELECTCONTENTS

This will select the contents of the drawer the command operates on.

WINDOW.CLEARSELECTION

This unselects all icons selected in the drawer the command operates on.

WINDOW.CLEANUPBY.COLUMN

This will sort the contents of the drawer and place the icons in columns.

WINDOW.CLEANUPBY.NAME

This will sort the contents of the drawer by name and place the icons in
rows.

WINDOW.CLEANUPBY.DATE

This will sort the contents of the drawer by date and place the icons in
rows.

WINDOW.CLEANUPBY.SIZE

This will sort the contents of the drawer by size and place the icons in
rows.

WINDOW.CLEANUPBY.TYPE

This will sort the contents of the drawer by type and place the icons in
rows.

WINDOW.RESIZETOFIT

This will resize the drawer window, trying to make it just as large as to
allow all its icons to fit.

WINDOW.SNAPSHOT.WINDOW

This will snapshot the drawer window, but none of its contents.

WINDOW.SNAPSHOT.ALL

This will snapshot the drawer window and its contents.

WINDOW.SHOW.ONLYICONS

This will change the display mode of the drawer to show only files and
drawers which have icons attached.

WINDOW.SHOW.ALLFILES

This will change the display mode of the drawer to show all files and
drawers, regardless of whether they have icons attached or not.

WINDOW.VIEWBY.ICON

This will change the display mode of the drawer to show its contents as
icons.

WINDOW.VIEWBY.NAME

This will change the display mode of the drawer to show its contents in
textual format, sorted by name.

WINDOW.VIEWBY.DATE

This will change the display mode of the drawer to show its contents in
textual format, sorted by date.

WINDOW.VIEWBY.SIZE

This will change the display mode of the drawer to show its contents in
textual format, sorted by size.

WINDOW.VIEWBY.TYPE

This will change the display mode of the drawer to show its contents in
textual format, sorted by type.

ICONS.OPEN 

This will open the currently selected icons. Workbench may bring up a requester in case project icons are found which lack a default tool.

ICONS.COPY 

This will duplicate the currently selected icons.

ICONS.RENAME 

This will prompt the user to choose a new name for each currently selected icon.

ICONS.INFORMATION 

This will open the information window for every currently selected icon.

ICONS.SNAPSHOT 

This will lock the position of every currently selected icon.

ICONS.UNSNAPSHOT 

This will unlock the position of every currently selected icon.

ICONS.LEAVEOUT 

This will permanently put all currently selected icons on the Workbench root window.

ICONS.PUTAWAY 

This will move all currently selected icons out of the root window and put them back into the drawers they belong.

ICONS.DELETE 

This will cause all currently selected files to be deleted, provided the user confirms this action first.

ICONS.FORMATDISK 

This will invoke the Format command on every currently selected disk icon. This will not format the disks immediately. The user will have to confirm this action first.

ICONS.EMPTYTRASH 

With a trashcan icon selected, this will empty it.

TOOLS.RESETWB 

This will close and reopen all Workbench windows.

------------------------------------------------------------------------------
#endif

static const struct InvokeMenu WBMenus[] =
	{
	{ "WORKBENCH.BACKDROP",		FALSE,	"backdrop",		NULL },
	{ "WORKBENCH.EXECUTE",		FALSE,	"executecommand",	NULL },
	{ "WORKBENCH.REDRAWALL",	FALSE,	"redrawall",		NULL },
	{ "WORKBENCH.UPDATEALL",	FALSE,	"updateall",		NULL },
	{ "WORKBENCH.LASTMESSAGE",	FALSE,	"lastmsg",		NULL },
	{ "WORKBENCH.ABOUT",		FALSE,	"about",		NULL },
	{ "WORKBENCH.QUIT",		FALSE,	"quit",			NULL },

	{ "WINDOW.NEWDRAWER",		TRUE,	"makedir",		NULL },
	{ "WINDOW.OPENPARENT",		TRUE,	NULL,			MenuOpenParentWindow },
	{ "WINDOW.CLOSE",		TRUE,	NULL,			MenuCloseScalosWindow },
	{ "WINDOW.UPDATE",		TRUE,	NULL,			MenuUpdateScalosWindow },
	{ "WINDOW.SELECTCONTENTS",	TRUE,	"selectall",		NULL },
	{ "WINDOW.CLEARSELECTION", 	TRUE,	NULL,			MenuUnselectAll },
	{ "WINDOW.CLEANUPBY.COLUMN",	TRUE,	"cleanup",		NULL },
	{ "WINDOW.CLEANUPBY.NAME",	TRUE,	"cleanupbyname",	NULL },
	{ "WINDOW.CLEANUPBY.DATE",	TRUE,	"cleanupbydate",	NULL },
	{ "WINDOW.CLEANUPBY.SIZE",	TRUE,	"cleanupbysize",	NULL },
	{ "WINDOW.CLEANUPBY.TYPE",	TRUE,	"cleanupbytype",	NULL },
	{ "WINDOW.RESIZETOFIT",		TRUE,	"sizetofit",		NULL },
	{ "WINDOW.SNAPSHOT.WINDOW",	TRUE,	"snapshotwindow",	NULL },
	{ "WINDOW.SNAPSHOT.ALL",	TRUE,	"snapshotall",		NULL },
	{ "WINDOW.SHOW.ONLYICONS",	TRUE,	"showonlyicons",	NULL },
	{ "WINDOW.SHOW.ALLFILES",	TRUE,	NULL,			MenuShowAllFiles },
	{ "WINDOW.VIEWBY.ICON",		TRUE,	NULL,			MenuSetViewModeByIcon },
	{ "WINDOW.VIEWBY.NAME",		TRUE,	NULL,			MenuSetViewModeByName },
	{ "WINDOW.VIEWBY.DATE",		TRUE,	NULL,			MenuSetViewModeByDate },
	{ "WINDOW.VIEWBY.SIZE",		TRUE,	NULL,			MenuSetViewModeBySize },
	{ "WINDOW.VIEWBY.TYPE",		TRUE,	NULL,			MenuSetViewModeByType },
	{ "WINDOW.FIND",		TRUE,	NULL,			NULL }, /* !!! */

	{ "ICONS.OPEN",			TRUE,	"open",			NULL },
	{ "ICONS.COPY",			TRUE,	"clone",		NULL },
	{ "ICONS.RENAME",		TRUE,	"rename",		NULL },
	{ "ICONS.INFORMATION",		TRUE,	"iconinfo",		NULL },
	{ "ICONS.SNAPSHOT",		TRUE,	"snapshot",		NULL },
	{ "ICONS.UNSNAPSHOT",		TRUE,	"unsnapshot",		NULL },
	{ "ICONS.LEAVEOUT",		TRUE,	"leaveout",		NULL },
	{ "ICONS.PUTAWAY",		TRUE,	"putaway",		NULL },
	{ "ICONS.DELETE",		TRUE,	"reset",		NULL },
	{ "ICONS.FORMATDISK",		TRUE,	"formatdisk",		NULL },
	{ "ICONS.EMPTYTRASH",		TRUE,	"emptytrashcan",	NULL },

	{ "TOOLS.RESETWB",		FALSE,	NULL,			NULL }, /* !!! */

	{ "DEBUG.SAD",			FALSE,	NULL,			NULL }, /* !!! */
	{ "DEBUG.FLUSHLIBS",		FALSE,	NULL,			NULL }, /* !!! */

	{ NULL,				FALSE,	NULL,			NULL }
	};

LONG Cmd_Menu(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_INVOKE, ARG_NAME, ARG_TITLE, 
		ARG_SHORTCUT, ARG_ADD, ARG_REMOVE, ARG_CMD };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d1(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (Args[ARG_ADD])
		{
		d1(kprintf(__FUNC__ "/%ld: ADD\n", __LINE__);)

		if (NULL == Args[ARG_NAME])
			return ERROR_REQUIRED_ARG_MISSING;
		if (NULL == Args[ARG_TITLE])
			return ERROR_REQUIRED_ARG_MISSING;
		if (NULL == Args[ARG_CMD])
			return ERROR_REQUIRED_ARG_MISSING;

		Result = AddMenuItem(Args[ARG_NAME], Args[ARG_TITLE], Args[ARG_CMD], Args[ARG_SHORTCUT]);
		}
	else if (Args[ARG_REMOVE])
		{
		d1(kprintf(__FUNC__ "/%ld: REMOVE\n", __LINE__);)

		if (NULL == Args[ARG_NAME])
			return ERROR_REQUIRED_ARG_MISSING;

		Result = RemoveMenuItem(Args[ARG_NAME]);
		}
	else		// Args[ARG_INVOKE] is optional
		{
		const struct InvokeMenu *menu = WBMenus;
		BOOL Found = FALSE;

		d1(kprintf(__FUNC__ "/%ld: INVOKE <%s>\n", __LINE__, Args[ARG_INVOKE]);)

		while (menu->ivm_WBmenuName && !Found && RETURN_OK == Result)
			{
			if (0 == Stricmp((STRPTR) menu->ivm_WBmenuName, Args[ARG_INVOKE]))
				{
				if (!menu->ivm_WindowRequired && (NULL == swi))
					swi = FindWindowByName("root");

				if (NULL == swi)
					return(ERROR_OBJECT_NOT_FOUND);

				if (menu->ivm_ScalosMenuName)
					{
					DoMethod(swi->ws_WindowTask->mt_MainObject,
						SCCM_IconWin_MenuCommand, menu->ivm_ScalosMenuName,
						NULL, 0);
					}
				else if (menu->ivm_MenuFunc)
					{
					Result = (*menu->ivm_MenuFunc)(swi);
					}
				Found = TRUE;
				}
			menu++;
			}
		if (!Found)
			Result = ERROR_OBJECT_NOT_FOUND;
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
MOVEWINDOW command

Purpose:

This command will attempt to change the position of a window.

Format:

MOVEWINDOW [WINDOW] <ROOT|ACTIVE|Drawer name> [[LEFTEDGE] <new left edge
position>] [[TOPEDGE] <new top edge position>]

Template:

MOVEWINDOW WINDOW,LEFTEDGE/N,TOPEDGE/N

Parameters:

WINDOW

Either ROOT to move the Workbench root window (where volume icons and
AppIcons live), ACTIVE to move the currently active Workbench window or the
fully qualified name of a drawer window to change. Note that the drawer
window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

LEFTEDGE

New left edge window position.

TOPEDGE

New top edge window position.

Errors:

10 - If the named window cannot be moved; this can also happen if you
specified ACTIVE as the window name and none of the Workbench windows is
currently active. The error code will be placed in the WORKBENCH.LASTERROR
variable.

Result:

-

Notes:

If you choose to have a window changed that is neither the root nor the
active window you must make sure that the window name is given as a fully
qualified path name. For example Work: is a fully qualified name, and so is
SYS:Utilities. Devs/Printers would not be a fully qualified name. A fully
qualified name always contains the name of an assignment, a volume or a
device.

Example:

/* Move the root window to position 10,30. */
ADDRESS workbench

MOVEWINDOW root LEFTEDGE 10 TOPEDGE 30

/* Move the currently active window. */
MOVEWINDOW active 20 40


------------------------------------------------------------------------------
#endif
LONG Cmd_MoveWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_LEFTEDGE, ARG_TOPEDGE };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;
	WORD Left, Top;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (Args[ARG_LEFTEDGE])
		Left = *((LONG *) Args[ARG_LEFTEDGE]);
	else
		Left = swi->ws_Window->LeftEdge;
	if (Args[ARG_TOPEDGE])
		Top = *((LONG *) Args[ARG_TOPEDGE]);
	else
		Top = swi->ws_Window->TopEdge;

	ChangeWindowBox(swi->ws_Window, Left, Top, swi->ws_Window->Width, swi->ws_Window->Height);

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
NEWDRAWER command

Purpose:

This command is for creating new drawers.

Format:

NEWDRAWER [NAME] <Name of drawer to create>

Template:

NEWDRAWER NAME/A

Parameters:

NAME

Name of the drawer to be created.

Errors:

10 - If the named drawer could not be created.

Result:

-

Notes:

The drawer name given must be an absolute path, such as in RAM:Empty. A
relative path, such as /fred/barney will not work.

Example :

/* Create a drawer by the name of Empty in the RAM disk. */
ADDRESS workbench

NEWDRAWER ,RAM:Empty`


------------------------------------------------------------------------------
#endif
LONG Cmd_NewDrawer(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_NAME };
	LONG Result;
	char *ResultString = NULL;

	d(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, Args[ARG_NAME]);)

	Result = MenuNewDrawer(Args[ARG_NAME]);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
RENAME command

Purpose:

This command is for renaming files, drawers and volumes.

Format:

RENAME [OLDNAME] <Name of file/drawer/volume to rename> [NEWNAME] <New name
of the file/drawer/volume>

Template:

RENAME OLDNAME/A,NEWNAME/A

Parameters:

OLDNAME

Name of the file/drawer/volume to be renamed. This must be an absolute path,
such as in RAM:Empty. A relative path, such as /fred/barney, will not work.

NEWNAME

The new name to assign to the file/drawer/volume. This must not be an
absolute or relative path. For example, wilma is valid new name, /wilma or
wilma: would be invalid names.

Errors:

10 - If the object cannot be renamed.

Result:

-

Notes:

The RENAME command does not work like for example the AmigaDOS Rename
command. For example, RENAME ,ram:empty` ,newname` will rename the file
,RAM:empty` to ,RAM:newname`.

Example:

/* Rename a drawer by the name of Old in the RAM disk to New. */
ADDRESS workbench

RENAME ,RAM:Old` ,New`


------------------------------------------------------------------------------
#endif
LONG Cmd_Rename(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_OLDNAME, ARG_NEWNAME };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;

	d(kprintf(__FUNC__ "/%ld: Old=<%s>  New=<%s>\n", __LINE__, Args[ARG_OLDNAME], Args[ARG_NEWNAME]);)

	// rename object "xyzzy"
	if (Rename(Args[ARG_OLDNAME], Args[ARG_NEWNAME]))
		{
		char oldNameIcon[256], newNameIcon[256];

		stccpy(oldNameIcon, Args[ARG_OLDNAME], sizeof(oldNameIcon)-6);
		strcat(oldNameIcon, ".info");

		stccpy(newNameIcon, Args[ARG_NEWNAME], sizeof(newNameIcon)-6);
		strcat(newNameIcon, ".info");

		// Rename icon "xyzzy.info"
		if (!Rename(oldNameIcon, newNameIcon))
			Result = IoErr();
		}
	else
		Result = IoErr();

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
RX command

Purpose:

This command is for executing ARexx scripts and commands.

Format:

RX [CONSOLE] [ASYNC] [CMD] <Command to execute>

Template:

RX CONSOLE/S,ASYNC/S,CMD/A/F

Parameters:

CONSOLE

This switch indicates that a console (for default I/O) is needed.

ASYNC

This switch indicates that the command should be run asynchronously, i.e. the
RX command will return as soon as ARexx has been instructed to run the
command you specified. Otherwise, the RX command will wait for the specified
ARexx command to complete execution.

COMMAND

This is the name of the ARexx program to execute.

Errors:

10 - If the given ARexx program could not be executed.

Result:

-

Example:

/* Execute an ARexx program by the name of ,test.wb`; * its output should be
sent to a console window. */
ADDRESS workbench

RX CONSOLE CMD ,test.wb`


------------------------------------------------------------------------------
#endif
LONG Cmd_Rx(APTR UserData,struct RexxMsg *Message,STRPTR *Args)
{
	enum	{ ARG_CONSOLE,ARG_ASYNC,ARG_COMMAND };

	struct LocalData *LocalData;
	struct ChildMessage *ChildMessage;
	BPTR Stream;
	struct Process *Child;
	struct FileHandle *Handle;
	LONG Error;

	Printf("RX");

	if(Args[ARG_CONSOLE] != NULL)
		Printf(" CONSOLE");

	if(Args[ARG_ASYNC] != NULL)
		Printf(" ASYNC");

	if(Args[ARG_COMMAND] != NULL)
		Printf(" COMMAND=\"%s\"", (ULONG) Args[ARG_COMMAND]);

	Printf("\n");

		/* If no command was invoked, return without
		 * doing any real work.
		 */

	if(Args[ARG_COMMAND] == NULL)
	{
		ReturnRexxMsg(Message,0);
		return(0);
	}

	/* Should we open a console output window? */

	if(Args[ARG_CONSOLE])
	{
		LocalData = (struct LocalData *)UserData;

			/* Create the process startup message. */

		ChildMessage = CreateChildMessage(LocalData->GlobalPort,Args[ARG_COMMAND],Message);

		if(ChildMessage != NULL)
		{
				/* Fill in the name of the Rexx host. */

			ChildMessage->PortName = LocalData->PortName;

				/* If the command should execute synchronously,
				 * store the RexxMsg pointer in the startup
				 * message. The background process will then
				 * return the message after executing the
				 * command.
				 */

			if(Args[ARG_ASYNC] == NULL)
				ChildMessage->RexxMsg = Message;

				/* Open the output console; you may want to substitute
				 * the title "Host" in your own programs.
				 */

			if(Stream = Open("CON:0/25/640/150/Host/AUTO/WAIT",MODE_NEWFILE))
			{
				/* Turn the BPTR to the file handle into a
				 * pointer to the file handle.
				 */
				STATIC_PATCHFUNC(RxEntry);
				Handle = (struct FileHandle *)BADDR(Stream);

					/* Create the background process. */

				Child = CreateNewProcTags(
					NP_Input,	Stream,
					NP_CloseInput,	FALSE,
					NP_Output,	NULL,
					NP_ConsoleTask,	(ULONG) Handle->fh_Type,
					NP_WindowPtr,	-1,
					NP_Entry,	(ULONG) PATCH_NEWFUNC(RxEntry),
				TAG_DONE);

				if(Child != NULL)
				{
						/* Send the startup message. */

					PutMsg(&Child->pr_MsgPort, (struct Message *) ChildMessage);

						/* Increment the number of outstanding
						 * startup messages.
						 */

					LocalData->Usage++;

						/* If the command was to be executed
						 * asynchronously, return the message
						 * now so the calling program can
						 * continue running while the process
						 * is dealing with the command.
						 */

					if(Args[ARG_ASYNC] != NULL)
						ReturnRexxMsg(Message,0);

					return(0);
				}
				else
					Error = IoErr();

				Close(Stream);
			}
			else
				Error = IoErr();

			DeleteChildMessage(ChildMessage);
		}
		else
			Error = IoErr();
	}
	else
	{
			/* If the command should execute asynchronously,
			 * reply the RexxMsg now.
			 */

		if(Args[ARG_ASYNC] != NULL)
			ReturnRexxMsg(Message,0);

			/* Execute the command. */

		Error = DoRexxCommand(Args[ARG_COMMAND]);

			/* If the RexxMsg was already replied,
			 * return now.
			 */

		if(Args[ARG_ASYNC] != NULL)
			return(0);

			/* If no error occured, return the RexxMsg. */

		if(Error == 0)
		{
			ReturnRexxMsg(Message,0);
			return(0);
		}
	}

	return(Error);
}


#if 0
------------------------------------------------------------------------------
SIZEWINDOW command

Purpose:

This command will attempt to change the size of a window.

Format:

SIZEWINDOW [WINDOW] <ROOT|ACTIVE|Drawer name> [[WIDTH] <new window width>]
[[HEIGHT] <new window height>]

Template:

SIZEWINDOW WINDOW,WIDTH/N,HEIGHT/N

Parameters:

WINDOW

Either ROOT to resize the Workbench root window (where volume icons and
AppIcons live), ACTIVE to resize the currently active Workbench window or
the fully qualified name of a drawer window to change. Note that the drawer
window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

WIDTH

New window width.

HEIGHT

New window height.

Errors:

10 - If the named window cannot be resized; this can also happen if you
specified ACTIVE as the window name and none of the Workbench windows is
currently active. The error code will be placed in the WORKBENCH.LASTERROR
variable.

Result:

-

Notes:

If you choose to have a window resized that is neither the root nor the
active window you must make sure that the window name is given as a fully
qualified path name. For example Work: is a fully qualified name, and so is
SYS:Utilities. Devs/Printers would not be a fully qualified name. A fully
qualified name always contains the name of an assignment, a volume or a
device.

Example:

/* Change the root window size to 200100 pixels. */
ADDRESS workbench

SIZEWINDOW root 30 WIDTH 200 HEIGHT 100

/* Resize the currently active window. */
SIZEWINDOW active 200 100


------------------------------------------------------------------------------
#endif
LONG Cmd_SizeWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_WIDTH, ARG_HEIGHT };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;
	WORD Width, Height;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (Args[ARG_WIDTH])
		Width = *((LONG *) Args[ARG_WIDTH]);
	else
		Width = swi->ws_Window->Width;
	if (Args[ARG_HEIGHT])
		Height = *((LONG *) Args[ARG_HEIGHT]);
	else
		Height = swi->ws_Window->Height;

	ChangeWindowBox(swi->ws_Window, swi->ws_Window->LeftEdge, swi->ws_Window->TopEdge, Width,Height);

	DoMethod(swi->ws_WindowTask->mt_MainObject,
		SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
UNLOCKGUI command

Purpose:

This command will allow access to all Workbench drawer windows locked with
the LOCKGUI command.

Format:

UNLOCKGUI

Template:

UNLOCKGUI

Parameters:

-

Errors:

-

Result:

-

Notes:

It takes as many UNLOCKGUI commands as there were LOCKGUI commands to make
the Workbench drawer windows usable again. In other words, the LOCKGUI
command nests.

Example:

/* Reallow access to all Workbench drawer windows. */
ADDRESS workbench

UNLOCKGUI


------------------------------------------------------------------------------
#endif
LONG Cmd_UnlockGUI(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	LONG Result = RETURN_OK;
	char *ResultString = NULL;

	/* !!! */

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
VIEW command

Purpose:

This command will change the position of the viewable display area of a
window.

Format:

VIEW [WINDOW] <ROOT|ACTIVE|Drawer name> [PAGE|PIXEL] [UP|DOWN|LEFT|RIGHT]

Template:

VIEW WINDOW,PAGE/S,PIXEL/S,UP/S,DOWN/S,LEFT/S,RIGHT/S

Parameters:

WINDOW

Either ROOT to change the Workbench root window view (where volume icons and
AppIcons live), ACTIVE to change the currently active Workbench window view
or the fully qualified name of a drawer window to change. Note that the
drawer window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

UP

Move the view up by about 1/8 of the window height. If PAGE is specified,
moves the view up by a whole page. If PIXEL is specified, moves the view up
by a single pixel.

DOWN

Move the view down by about 1/8 of the window height. If PAGE is specified,
moves the view down by a whole page. If PIXEL is specified, moves the view
down by a single pixel.

LEFT

Move the view left by about 1/8 of the window height. If PAGE is specified,
moves the view left by a whole page. If PIXEL is specified, moves the view
left by a single pixel.

RIGHT

Move the view right by about 1/8 of the window height. If PAGE is specified,
moves the view right by a whole page. If PIXEL is specified, moves the view
right by a single pixel.

Errors:

10 - If the named window view cannot be changed; this can also happen if you
specified ACTIVE as the window name and none of the Workbench windows is
currently active. The error code will be placed in the WORKBENCH.LASTERROR
variable.

Result:

-

Notes:

If you choose to have a window view changed that is neither the root nor the
active window you must make sure that the window name is given as a fully
qualified path name. For example Work: is a fully qualified name, and so is
SYS:Utilities. Devs/Printers would not be a fully qualified name. A fully
qualified name always contains the name of an assignment, a volume or a
device.

To find out about a window`s current view position, use the GETATTR command
and query the window`s WINDOW.VIEW.LEFT and WINDOW.VIEW.TOP attributes.

Example:

/* Change the root window view; move it up by a whole page. */
ADDRESS workbench

VIEW root PAGE UP

------------------------------------------------------------------------------
#endif
LONG Cmd_View(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_PAGE, ARG_PIXEL, 
		ARG_UP, ARG_DOWN, ARG_LEFT, ARG_RIGHT };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;
	WORD Width, Height;
	LONG DeltaX, DeltaY;
	LONG deltaX = 0, deltaY = 0;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	Width = swi->ws_Window->Width - swi->ws_Window->BorderLeft - swi->ws_Window->BorderRight;
	Height = swi->ws_Window->Height - swi->ws_Window->BorderTop - swi->ws_Window->BorderBottom;

	if (Args[ARG_PAGE])
		{
		DeltaX = Width;
		DeltaY = Height;
		}
	else if (Args[ARG_PIXEL])
		{
		DeltaX = DeltaY = 1;
		}
	else
		{
		DeltaX = Width / 8;
		DeltaY = Height / 8;
		}

	if (Args[ARG_LEFT])
		{
		deltaX = -DeltaX;
		}
	else if (Args[ARG_RIGHT])
		{
		deltaX = DeltaX;
		}

	if (Args[ARG_UP])
		{
		deltaY = DeltaY;
		}
	else if (Args[ARG_DOWN])
		{
		deltaY = -DeltaY;
		}

	if (deltaX != 0 || deltaY != 0)
		{
		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_DeltaMove, deltaX, deltaY);

		DoMethod(swi->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_SetVirtSize, SETVIRTF_AdjustRightSlider | SETVIRTF_AdjustBottomSlider);
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
WINDOW command

Purpose:

This command will change, open, close or snapshot windows.

Format:

WINDOW [WINDOWS] <Window name> .. <Window name> [OPEN|CLOSE] [SNAPSHOT]
[ACTIVATE] [MIN|MAX] [FRONT|BACK] [CYCLE PREVIOUS|NEXT]

Template:

WINDOW WINDOWS/M/A,OPEN/S,CLOSE/S,SNAPSHOT/S,ACTIVATE/S,MIN/S,MAX/S,
FRONT/S,BACK/S,CYCLE/K

Parameters:

WINDOWS

Names of the windows to operate on. This can be ROOT to for the Workbench
root window (where volume icons and AppIcons live), ACTIVE for the currently
active Workbench window or the fully qualified name of a drawer window.

OPEN

Attempt to open the specified windows.

CLOSE

Close the specified windows. Note that if a window is closed no further
operations (such as SNAPSHOT, ACTIVATE, etc.) can be performed on it.

SNAPSHOT

Snapshot the sizes and positions of the specified windows.

ACTIVATE

Activate the specified windows. With multiple windows to activate, only one
window will wind up as the active one. Commonly, this will be the last window
in the list.

MIN

Resize the windows to their minimum dimensions.

MAX

Resize the windows to their maximum dimensions.

FRONT

Move the windows into the foreground.

BACK

Move the windows into the background.

CYCLE

This command operates on the currently active drawer window. You can specify
either PREVIOUS, to activate the previous drawer window in the list, or
NEXT, to activate the next following drawer window in the list.

Errors:

10 - If the named windows cannot be opened or operated on; this can also
happen if you specified ACTIVE as a window name and none of the Workbench
windows is currently active. The error code will be placed in the
WORKBENCH.LASTERROR variable.

Result:

-

Notes:

If you choose to have a window operated on that is neither the root nor the
active window you must make sure that the window name is given as a fully
qualified path name. For example Work: is a fully qualified name, and so is
SYS:Utilities. Devs/Printers would not be a fully qualified name. A fully
qualified name always contains the name of an assignment, a volume or a
device.

Example:

/* Open the Work: drawer. */
ADDRESS workbench

WINDOW ,Work:` OPEN

/* Activate the root window. */
WINDOW root ACTIVATE


------------------------------------------------------------------------------
#endif
LONG Cmd_Window(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOWS, ARG_OPEN, ARG_CLOSE, ARG_SNAPSHOT, ARG_ACTIVATE,
		ARG_MIN, ARG_MAX, ARG_FRONT, ARG_BACK, ARG_CYCLE };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;
	STRPTR *NamesPtr = (STRPTR *) Args[ARG_WINDOWS];

	d(kprintf(__FUNC__ "/%ld: \n", __LINE__);)

	while (*NamesPtr && RETURN_OK == Result)
		{
		d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, *NamesPtr);)

		swi = FindWindowByName(*NamesPtr);

		d(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi);)

		if (Args[ARG_OPEN])
			{
			d(kprintf(__FUNC__ "/%ld: OPEN\n", __LINE__);)

			WaitOpen(SCA_OpenDrawerByName(*NamesPtr, NULL));
			}
		if (Args[ARG_CLOSE])
			{
			d(kprintf(__FUNC__ "/%ld: CLOSE\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			SCA_LockWindowList(SCA_LockWindowList_Shared);

			// CloseScalosWindow() expects windowlist to be locked
			CloseScalosWindow(swi);
			swi = NULL;
			}
		if (Args[ARG_SNAPSHOT])
			{
			d(kprintf(__FUNC__ "/%ld: SNAPSHOT\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			DoMethod(swi->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_MenuCommand, "snapshotwindow",
				NULL, 0);
			}
		if (Args[ARG_ACTIVATE])
			{
			d(kprintf(__FUNC__ "/%ld: ACTIVATE\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			ActivateWindow(swi->ws_Window);
			}
		if (Args[ARG_MIN])
			{
			d(kprintf(__FUNC__ "/%ld: MIN\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			ChangeWindowBox(swi->ws_Window,
				swi->ws_Window->LeftEdge, swi->ws_Window->TopEdge,
				swi->ws_Window->MinWidth, swi->ws_Window->MinHeight);
			}
		if (Args[ARG_MAX])
			{
			d(kprintf(__FUNC__ "/%ld: MAX\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			ChangeWindowBox(swi->ws_Window,
				swi->ws_Window->LeftEdge, swi->ws_Window->TopEdge,
				swi->ws_Window->MaxWidth, swi->ws_Window->MaxHeight);
			}
		if (Args[ARG_FRONT])
			{
			d(kprintf(__FUNC__ "/%ld: FRONT\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			WindowToFront(swi->ws_Window);
			}
		if (Args[ARG_BACK])
			{
			d(kprintf(__FUNC__ "/%ld: BACK\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			WindowToBack(swi->ws_Window);
			}
		if (Args[ARG_CYCLE])
			{
			d(kprintf(__FUNC__ "/%ld: CYCLE\n", __LINE__);)

			if (NULL == swi)
				return ERROR_OBJECT_NOT_FOUND;

			if (0 == Stricmp(Args[ARG_CYCLE], "PREVIOUS"))
				{
				d(kprintf(__FUNC__ "/%ld: CYCLE PREVIOUS\n", __LINE__);)

				swi = GetPrevWindow(swi);
				if (swi)
					ActivateWindow(swi->ws_Window);
				else
					Result = ERROR_OBJECT_NOT_FOUND;
				}
			else if (0 == Stricmp(Args[ARG_CYCLE], "NEXT"))
				{
				d(kprintf(__FUNC__ "/%ld: CYCLE NEXT\n", __LINE__);)

				swi = GetNextWindow(swi);
				if (swi)
					ActivateWindow(swi->ws_Window);
				else
					Result = ERROR_OBJECT_NOT_FOUND;
				}
			}

		NamesPtr++;
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
WINDOWTOBACK command

Purpose:

This command will push a window into the background.

Format:

WINDOWTOBACK [WINDOW] <ROOT|ACTIVE|Drawer name>

Template:

WINDOWTOBACK WINDOW

Parameters:

WINDOW

ROOT to push the the Workbench root window (where volume icons and AppIcons
live) into to the background, ACTIVE to push the currently active Workbench
window into the background or the fully qualified name of a drawer window.
Note that the drawer window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

Errors:

10 - If the named window cannot be found. The error code will be placed in
the WORKBENCH.LASTERROR variable.

Result:

-

Notes:

If you choose to have a window pushed into the background that is not the
root window or the currently active window you must make sure that the window
name is given as a fully qualified path name. For example Work: is a fully
qualified name, and so is SYS:Utilities. Devs/Printers would not be a fully
qualified name. A fully qualified name always contains the name of an
assignment, a volume or a device.

Example:

/* Push the root window into the background. */
ADDRESS workbench

WINDOWTOBACK root


------------------------------------------------------------------------------
#endif
LONG Cmd_WindowToBack(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	WindowToBack(swi->ws_Window);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
WINDOWTOFRONT command

Purpose:

This command will bring a window to the foreground.

Format:

WINDOWTOFRONT [WINDOW] <ROOT|ACTIVE|Drawer name>

Template:

WINDOWTOFRONT WINDOW

Parameters:

WINDOW

ROOT to bring the the Workbench root window (where volume icons and AppIcons
live) to the foreground, ACTIVE to bring the currently active Workbench
window to the foreground or the fully qualified name of a drawer window. Note
that the drawer window must already be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

Errors:

10 - If the named window cannot be found. The error code will be placed in
the WORKBENCH.LASTERROR variable.

Result:

-

Notes:

If you choose to have a window brought to the foreground that is not the root
window or the currently active window you must make sure that the window name
is given as a fully qualified path name. For example Work: is a fully
qualified name, and so is SYS:Utilities. Devs/Printers would not be a fully
qualified name. A fully qualified name always contains the name of an
assignment, a volume or a device.

Example:

/* Bring the root window to the foreground. */ ADDRESS workbench

WINDOWTOFRONT root


------------------------------------------------------------------------------
#endif
LONG Cmd_WindowToFront(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	WindowToFront(swi->ws_Window);

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


#if 0
------------------------------------------------------------------------------
ZOOMWINDOW command

Purpose:

This command will change a window to alternate position and dimensions.

Format:

ZOOMWINDOW [WINDOW] <ROOT|ACTIVE|Drawer name>

Template:

ZOOMWINDOW WINDOW

Parameters:

WINDOW

Name of the window to operate on. ROOT will use the Workbench root window
(where volume icons and AppIcons live), ACTIVE will use the currently active
Workbench window. Any other fully qualified path name will use the drawer
window corresponding to the path. If no WINDOW parameter is specified, this
command will try to operate on the currently active Workbench window.

Errors:

10 - If the named window cannot be found. The error code will be placed in
the WORKBENCH.LASTERROR variable.

Result:

-

Example:

/* Change the root window. */
ADDRESS workbench

ZOOMWINDOW root


------------------------------------------------------------------------------
#endif
LONG Cmd_ZoomWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (swi->ws_Window->Flags & WFLG_HASZOOM)
		{
		if (!(swi->ws_Window->Flags & WFLG_ZOOMED))
			ZipWindow(swi->ws_Window);
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


static LONG OpenWBHelpFile(void)
{
	struct Process *WBHelpProc;
	STATIC_PATCHFUNC(WBHelpProcess);

	WBHelpProc = CreateNewProcTags(NP_Name, (ULONG) "WB39 WBHelp Handler",
			NP_Priority, 0,
			NP_Entry, (ULONG) PATCH_NEWFUNC(WBHelpProcess),
			NP_StackSize, 16384,
			TAG_END);
	if (WBHelpProc == NULL)
		{
		return IoErr();
		}

	return RETURN_OK;
}


static SAVEDS(void) WBHelpProcess(void)
{
//	LONG Result;
	BPTR lock = 0;
	struct Locale *locale;
	AMIGAGUIDECONTEXT handle = NULL;

	d1(kprintf("%s/%s/%ld Start WBHelp Process\n", __FILE__, __FUNC__, __LINE__);)

	do	{
		struct NewAmigaGuide nag;
		char LanguageName[40];
		char HelpDir[256];
		CONST_STRPTR HelpFile = "workbench.guide";
		size_t len;
	        STRPTR context[] =
			{
			NULL
			};

		memset(&nag, 0, sizeof(nag));

		locale = OpenLocale(NULL);
		if (NULL == locale)
			{
			//Result = IoErr();
			break;
			}

		d1(kprintf( "%s/%s/%ld: loc_LocaleName=<%s>  loc_LanguageName=<%s>\n", __FILE__, __FUNC__, __LINE__, locale->loc_LocaleName, locale->loc_LanguageName);)

		// first try to find help file in native language
		// use name from locale without the trailing ".language"
		len = strlen(locale->loc_LanguageName);
		if (len > strlen(".language"))
			len -= strlen(".language");
		len++;		// include room for trailing "\0"
		if (len > sizeof(LanguageName))
			len = sizeof(LanguageName);
		stccpy(LanguageName, locale->loc_LanguageName, len);
		snprintf(HelpDir, sizeof(HelpDir), "HELP:%s/sys/%s", LanguageName, HelpFile);

		lock = Lock(HelpDir, ACCESS_READ);
		d1(kprintf( "%s/%s/%ld: HelpDir=<%s>  lock=%08lx\n", __FILE__, __FUNC__, __LINE__, HelpDir, lock);)

		if (lock)
			{
			snprintf(HelpDir, sizeof(HelpDir), "HELP:%s/sys", locale->loc_LanguageName);
			UnLock(lock);
			}
		else
			{
			// if native language not found, try "english"
			snprintf(HelpDir, sizeof(HelpDir), "HELP:english/sys");
			}

		nag.nag_Name = (STRPTR) HelpFile;
		nag.nag_Flags = 0;
		nag.nag_Context = context;
		nag.nag_HostPort = "";
		nag.nag_ClientPort = "WBHelp";
		nag.nag_Node = "main";

		nag.nag_Lock = lock = Lock(HelpDir, ACCESS_READ);

		d1(kprintf( "%s/%s/%ld: nag_Lock=%08lx\n", __FILE__, __FUNC__, __LINE__, nag.nag_Lock);)
		if (0 == nag.nag_Lock)
			{
			//Result = IoErr();
			break;
			}

		d1(kprintf("%s/%s/%ld: before OpenAmigaGuide\n", __FILE__, __FUNC__, __LINE__);)

		// OpenAmigaGuideA
		handle = OpenAmigaGuide(&nag,
			TAG_END);

		d1(kprintf("%s/%s/%ld: handle=%08lx\n", __FILE__, __FUNC__, __LINE__, handle);)
		if (NULL == handle)
			{
			//Result = IoErr();
			break;
			}

		} while (0);

	if (locale)
		CloseLocale(locale);
	if (handle)
		CloseAmigaGuide(handle);

	if (lock)
		UnLock(lock);

	d1(kprintf("%s/%s/%ld END WBHelp Process\n", __FILE__, __FUNC__, __LINE__);)
}


#if !defined(__SASC) &&!defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
// Replacement for SAS/C library functions

static size_t stccpy(char *dest, const char *src, size_t MaxLen)
{
	size_t Count = 0;

	while (*src && MaxLen > 1)
		{
		*dest++ = *src++;
		MaxLen--;
		Count++;
		}
	*dest = '\0';
	Count++;

	return Count;
}
#endif /* !defined(__SASC) &&!defined(__MORPHOS__)  */
