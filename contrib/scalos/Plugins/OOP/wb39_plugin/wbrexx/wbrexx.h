// WBRexx.h
// $Date$
// $Revision$


#ifndef WBREXX_H_INCLUDED
#define	WBREXX_H_INCLUDED

#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

#include <libraries/commodities.h>
#include <scalos/scalos.h>

#include <defs.h>
#include <stdlib.h>

struct InvokeMenu
	{
	const char *ivm_WBmenuName;
	BOOL ivm_WindowRequired;
	const char *ivm_ScalosMenuName;
	ULONG (*ivm_MenuFunc)(struct ScaWindowStruct *swi);
	};

enum UDataType
	{
	udaType_Nothing = 0,
	udaType_ScaWindowStruct,
	udaType_ScaIconNode,
	udaType_TextFont,
	udaType_Screen,
	udaType_KeyCommand,
	udaType_MenuCommand,
	};

struct UData
	{
	APTR uda_Data;
	enum UDataType uda_Type;
	};

struct LeafData
	{
	long lres_Result;

	const char *lres_ResultString;	// nur wenn lres_Result == RETURN_OK;

	const char *lres_ArgPtr;	// Pointer auf nächsten Teil des Object-Namens

	STRPTR *lres_Args;		// die originalen Argumente

	struct RexxMsg *lres_Message;

	ULONG lres_Flags;

	char lres_ResultBuffer[128];
	};

#define	LRESF_ORIGINALNAME	0x00000001


struct LeafCmd
	{
	const char *lc_Name;

	const struct LeafCmd *lc_Next;		// next Leaf on this Level or NULL

	const struct LeafCmd *lc_Sub;		// Sub-Tree or NULL;

	struct UData (*lc_Function)(struct UData, struct LeafData *, const char *StemLeaf, ULONG Index);

	ULONG (*lc_GetCount)(struct UData, struct LeafData *);		// only if ENUM == lc_Name
	};

struct RexxHost
{
	APTR			UserData;
	const struct CmdFunc *	CommandTable;

	struct MsgPort *	GlobalPort;
	UBYTE			PortName[1];
};

	/* This is for parsing commands the ARexx port receives. We employ
	 * the dos.library/ReadArgs parser for this purpose.
	 */

struct RexxParseData
{
	struct RDArgs *	ReadArgs;
	UBYTE		Line[512];	/* We allow for commands to be up to
					 * 511 characters long. This is far
					 * below the maximum string length
					 * limit ARexx imposed (32768
					 * characters) but should be more
					 * than sufficient for normal
					 * applications.
					 */
	STRPTR		Args[32];	/* Up to 32 command arguments are
					 * allowed.
					 */
};

	/* A table to map command names to functions which
	 * will handle them.
	 */

struct CmdFunc
{
	STRPTR CommandName;	/* Name of the command. */
	STRPTR Template;	/* The command template in standard
				 * AmigaDOS syntax.
				 */

		/* Pointer to the function which implements this command. */

	LONG (*Function)(APTR,struct RexxMsg *,STRPTR *);
};

/* The local data used by the functions the host supports. */

struct LocalData
{
	struct RexxParseData *	ParseData;
	struct RexxHost *	Host;
	struct CmdFunc *	CommandTable;	/* Pointer to table of commands. */
	struct MsgPort *	GlobalPort;	/* Copied from RexxHost. */
	STRPTR			PortName;	/* Copied from RexxHost. */
	ULONG			Usage;		/* Usage count for Rx command. */
	BOOL			Terminate;	/* Set to TRUE if host should exit. */
};

/* This is a custom message the Rx command will need if the
 * "CONSOLE" keyword is used.
 */

struct ChildMessage
{
	struct Message		Message;	/* Common message header. */

	struct RexxMsg *	RexxMsg;	/* The client message. */
	STRPTR			PortName;	/* Copied from LocalData. */
	UBYTE			Command[1];	/* Command to execute. */
};


enum IconState	{ icst_All, icst_Selected, icst_Unselected };


struct ToolMenuItem
	{
	struct Node tmi_Node;
	STRPTR tmi_Name;	// this is the "handle" to refer to this menu item
	STRPTR tmi_Title;	// the item title
	STRPTR tmi_Cmd;		// the command string
	STRPTR tmi_Shortcut;

	STRPTR tmi_MenuTitle;	// menu title for menus NULL for common tool menu items
	STRPTR tmi_SubItem;	// sub item name or NULL for common tool menu items

	APTR tmi_wbKey;		// "key" retrieved by WBAPPMENUA_GetKey
	APTR tmi_wbTitleKey;	// "key" retrieved by WBAPPMENUA_GetTitleKey

	struct AppObject *tmi_MenuAppObject;
	struct AppObject *tmi_ItemAppObject;
	struct AppObject *tmi_SubAppObject;

	ULONG tmi_UseCount;
	struct ToolMenuItem *tmi_Parent;
	};

struct KeyboardCommand
	{
	struct Node kbc_Node;
	STRPTR kbc_Name;
	STRPTR kbc_Cmd;
	STRPTR kbc_Key;
	IX kbc_IX;
	};

//----------------------------------------------------------------------------

#if defined(__SASC)
#include <stdarg.h>
int snprintf(char *, size_t, const char *, /*args*/ ...);
int vsnprintf(char *, size_t, const char *, va_list ap);
#endif /* __SASC */

//----------------------------------------------------------------------------

// WBRexx.c
LONG DoRexxCommand(CONST_STRPTR Command);
LONG ReturnRexxMsg(struct RexxMsg * Message,CONST_STRPTR Result);
SAVEDS(VOID) RxEntry(VOID);
VOID DeleteChildMessage(struct ChildMessage *ChildMessage);
struct ChildMessage *CreateChildMessage(struct MsgPort *ReplyPort,
	CONST_STRPTR Command,struct RexxMsg *RexxMessage);
void ShutdownWBRexxProcess(void);
BOOL StartWBRexxProcess(void);
CONST_STRPTR GetNextArgPart(CONST_STRPTR ArgSrc, STRPTR Buffer, size_t MaxLen);
ULONG AddMenuItem(CONST_STRPTR Name, CONST_STRPTR Title, CONST_STRPTR Cmd, CONST_STRPTR Shortcut);
ULONG RemoveMenuItem(CONST_STRPTR Name);
ULONG AddKeyboardCommand(CONST_STRPTR Name, CONST_STRPTR Key, CONST_STRPTR Cmd);
ULONG RemoveKeyboardCommand(CONST_STRPTR Name);
BOOL DoKeyboardCommand(struct IntuiMessage *iMsg);


// RexxGetAttrs.c
LONG Cmd_GetAttr(APTR UserData,struct RexxMsg *Message,STRPTR *Args);


// RexxIcon.c
LONG Cmd_Icon(APTR UserData, struct RexxMsg *Message, STRPTR *Args);


// RexxCmd.c
LONG Cmd_ActivateWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_ChangeWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Delete(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Fault(APTR UserData,struct RexxMsg *Message,STRPTR *Args);
LONG Cmd_Help(APTR UserData,struct RexxMsg *Message,STRPTR *Args);
LONG Cmd_Info(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Keyboard(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_LockGUI(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Menu(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_MoveWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_NewDrawer(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Rename(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Rx(APTR UserData,struct RexxMsg *Message,STRPTR *Args);
LONG Cmd_SizeWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_UnlockGUI(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_UnzoomWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_View(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_Window(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_WindowToBack(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_WindowToFront(APTR UserData, struct RexxMsg *Message, STRPTR *Args);
LONG Cmd_ZoomWindow(APTR UserData, struct RexxMsg *Message, STRPTR *Args);


//----------------------------------------------------------------------------

#endif
