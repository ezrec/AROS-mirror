/*
**	Global.h
**
**	Global data structure definitions
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H 1

#ifndef _HEADERS_H
#include "Headers.h"
#endif	/* _HEADERS_H */

	/* Special character codes. */

#define XOF ('S' & 0x1F)
#define XON ('Q' & 0x1F)

#define BEL '\a'
#define VTB '\v'
#define TAB '\t'
#define BKS '\b'
#define ENT '\n'
#define RET '\r'
#define ESC '\033'
#define DEL '\177'
#define FFD '\f'
#define CAN '\030'
#define SUB '\032'
#define ENQ '\005'
#define SI  '\016'
#define SO  '\017'

	/* Control keys we assign special codes to. */

#define CUP 150
#define CDN 151
#define CFW 152
#define CBK 153

#define FN1 128
#define FN2 129
#define FN3 130
#define FN4 131
#define FN5 132
#define FN6 133
#define FN7 134
#define FN8 135
#define FN9 136
#define F10 137

#define HLP 149

	/* Control sequence introducer. */

#define CSI 155

	/* Standard IFF chunk types. */

#ifndef ID_8SVX
#define ID_8SVX	MAKE_ID('8','S','V','X')	/* Eight bit sampled voice. */
#endif	/* ID_8SVX */

#ifndef ID_VHDR
#define ID_VHDR	MAKE_ID('V','H','D','R')	/* Voice header. */
#endif	/* ID_VHDR */

#ifndef ID_BODY
#define ID_BODY	MAKE_ID('B','O','D','Y')	/* Generic data body chunk. */
#endif	/* ID_BODY */

#ifndef ID_ANNO
#define ID_ANNO MAKE_ID('A','N','N','O')	/* Annotation chunk. */
#endif	/* ID_ANNO */

#define ID_CHAN MAKE_ID('C','H','A','N')	/* Sound channel information. */
#define ID_CHRS	MAKE_ID('C','H','R','S')	/* Character data. */
#define ID_FTXT	MAKE_ID('F','T','X','T')	/* Formatted text. */

	/* "term" private chunk types. */

#define ID_DATE	MAKE_ID('D','A','T','E')	/* Date and time information. */
#define ID_DAT2	MAKE_ID('D','A','T','2')	/* Date and time information, second try. */

#ifndef ID_NAME
#define ID_NAME	MAKE_ID('N','A','M','E')	/* Name header */
#endif	/* ID_NAME */

#ifndef ID_PREF
#define ID_PREF	MAKE_ID('P','R','E','F')	/* Preferences data chunk. */
#endif	/* ID_PREF */

#define ID_DIAL	MAKE_ID('D','I','A','L')	/* Number of phonebook entries. */
#define ID_FAST	MAKE_ID('F','A','S','T')	/* Fast! macro settings. */
#define ID_TRAP	MAKE_ID('T','R','A','P')	/* Trap command. */
#define ID_SEQN	MAKE_ID('S','E','Q','N')	/* Trap sequence. */
#define ID_TRST MAKE_ID('T','R','S','T')	/* Trap settings. */
#define ID_HOTK	MAKE_ID('H','O','T','K')	/* Hotkey settings. */
#define ID_KEYS	MAKE_ID('K','E','Y','S')	/* Function key settings. */
#define ID_PHON	MAKE_ID('P','H','O','N')	/* Phonebook data. */
#define ID_PSWD	MAKE_ID('P','S','W','D')	/* Password entry. */
#define ID_RECV	MAKE_ID('R','E','C','V')	/* Translation table data (receiver side). */
#define ID_SEND	MAKE_ID('S','E','N','D')	/* Translation table data (sender side). */
#define ID_TRNS	MAKE_ID('T','R','N','S')	/* Translation table data (master table). */
#define ID_SPEK	MAKE_ID('S','P','E','K')	/* Speech settings. */
#define ID_TERM	MAKE_ID('T','E','R','M')	/* "term" data chunk. */
#define ID_VERS	MAKE_ID('V','E','R','S')	/* Version identification. */
#define ID_WIND	MAKE_ID('W','I','N','D')	/* Fast! macro window size and position. */
#define ID_SOUN	MAKE_ID('S','O','U','N')	/* Sound settings. */
#define ID_WINF	MAKE_ID('W','I','N','F')	/* Window position/size information. */
#define ID_GRUP MAKE_ID('G','R','U','P')	/* Grouping information. */

	/* "term" configuration and phonebook file chunk types. */

#ifndef ID_SERL
#define ID_SERL	MAKE_ID('S','E','R','L')	/* Serial settings. */
#endif	/* ID_SERL */

#define ID_MODM	MAKE_ID('M','O','D','M')	/* Modem settings. */
#define ID_COMD	MAKE_ID('C','O','M','D')	/* Command settings. */
#define ID_SCRN	MAKE_ID('S','C','R','N')	/* Screen settings. */
#define ID_TRML	MAKE_ID('T','R','M','L')	/* Terminal setting. */
#define ID_PATH	MAKE_ID('P','A','T','H')	/* Path settings. */
#define ID_MISC	MAKE_ID('M','I','S','C')	/* Misc settings. */
#define ID_CLIP	MAKE_ID('C','L','I','P')	/* Clip settings. */
#define ID_CPTR	MAKE_ID('C','P','T','R')	/* Capture settings. */
#define ID_FILE	MAKE_ID('F','I','L','E')	/* File settings. */
#define ID_EMLN	MAKE_ID('E','M','L','N')	/* Emulation settings. */
#define ID_XFER	MAKE_ID('X','F','E','R')	/* Transfer settings. */
#define ID_XLNM	MAKE_ID('X','L','N','M')	/* Translation file name. */
#define ID_MFNM	MAKE_ID('M','F','N','M')	/* Macro file name. */
#define ID_CRNM	MAKE_ID('C','R','N','M')	/* Cursor file name. */
#define ID_FMNM	MAKE_ID('F','M','N','M')	/* Fast macro file name. */
#define ID_SPNM	MAKE_ID('S','P','N','M')	/* Speech file name. */
#define ID_SONM	MAKE_ID('S','O','N','M')	/* Sound file name. */
#define ID_ACNM	MAKE_ID('A','C','N','M')	/* Area code file name. */
#define ID_PBNM	MAKE_ID('P','B','N','M')	/* Phonebook file name. */
#define ID_HKNM	MAKE_ID('H','K','N','M')	/* Hotkey file name. */
#define ID_TRNM	MAKE_ID('T','R','N','M')	/* Trap file name. */

	/* This still appears to be missing. */

#ifndef FORMAT_DEF
#define FORMAT_DEF 4
#endif	/* FORMAT_DEF */

	/* Single precision boolean value. */

typedef char BOOLEAN;

	/* Structure forward declarations for the GNU `C' compiler. */

#ifdef __GNUC__
struct MsgItem;
struct GenericList;
#endif	/* __GNUC__ */

	/* The configuration file format version. This will from now
	 * on hopefully stay the same.
	 */

#define CONFIG_FILE_VERSION	4
#define CONFIG_FILE_REVISION	3

	/* The name of the global `term' message port. */

#define TERMPORTNAME "term Port"

	/* IDCMP flags for the main window */

#define DEFAULT_IDCMP		(IDCMP_RAWKEY | IDCMP_INACTIVEWINDOW | IDCMP_ACTIVEWINDOW | IDCMP_MOUSEMOVE | IDCMP_GADGETUP | IDCMP_GADGETDOWN | IDCMP_MENUPICK | IDCMP_MOUSEMOVE | IDCMP_MOUSEBUTTONS | IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_IDCMPUPDATE | IDCMP_MENUHELP)

	/* The interval in which routine checks are executed (in seconds). */

#define ROUTINE_CHECK_INTERVAL	2

	/* Jump table entry. */

typedef BOOL (* JUMP)(LONG Char);

	/* Capture output routine. */

typedef VOID (* COPTR)(APTR Data,LONG Size);

	/* ConTransfer callback routine. */

typedef VOID (* CONTRANSFER)(STRPTR,LONG);

	/* Simple text transfer callback routine. */

typedef BOOL (* SENDLINE)(STRPTR,LONG);

	/* PutChar routine for RawDoFmt(). */

typedef VOID (* PUTCHAR)(UBYTE Char,APTR PutChData);

	/* Mode filter function callback. */

typedef ULONG (* MODEFILTER)(ULONG,APTR);

	/* Stackcall routine type. */

typedef LONG (* STACKARGS STACKCALL)(ULONG Arg1,...);

	/* The routine to do the comparison for QuickSort. */

typedef int (* SORTFUNC)(const void * , const void *);

	/* CR/LF receiver translation */

typedef LONG (* TRANSLATEFUNC)(STRPTR Data,LONG Size);

	/* Raster save callback routine. */

typedef VOID (* SAVERASTER)(LONG First,LONG Last);

	/* Bypass for serial output. */

typedef BOOL (* BYPASS)(STRPTR Buffer,LONG Size);

	/* Normal serial output. */

typedef VOID (* SERWRITE)(STRPTR Buffer,LONG Size);

	/* This defines the destructor data type */

typedef VOID (* DESTRUCTOR)(struct MsgItem *);

	/* The generic list notify function. */

typedef VOID (* GENERICNOTIFY)(struct GenericList *List,WORD Login);

	/* A task or process entry function. */

typedef VOID (* STACKARGS TASKENTRY)(LONG Arg,...);

	/* Menu item codes. */

enum	{	MEN_SAVE_AS_PICTURE=1,MEN_SAVE_AS_TEXT,MEN_PRINT_SCREEN,MEN_PRINT_CLIP,MEN_CAPTURE_TO_FILE,
		MEN_CAPTURE_TO_PRINTER,MEN_ICONIFY,MEN_ABOUT,MEN_QUIT,

		MEN_COPY,MEN_PASTE,MEN_CLEAR,

		MEN_EXECUTE_DOS_COMMAND,MEN_EXECUTE_REXX_COMMAND,MEN_RECORD,MEN_RECORD_LINE,
		MEN_EDIT_TRAPS,MEN_DISABLE_TRAPS,

		MEN_PHONEBOOK,MEN_REDIAL,MEN_DIAL_NUMBER,MEN_SEND_BREAK,
		MEN_HANG_UP,MEN_WAIT,MEN_FLUSH_BUFFER,MEN_RELEASE_DEVICE,

		MEN_UPLOAD_ASCII,MEN_DOWNLOAD_ASCII,MEN_UPLOAD_TEXT,MEN_DOWNLOAD_TEXT,MEN_EDIT_AND_UPLOAD_TEXT,
		MEN_UPLOAD_BINARY,MEN_DOWNLOAD_BINARY,

		MEN_CLEAR_BUFFER,MEN_DISPLAY_BUFFER,MEN_CLOSE_BUFFER,MEN_FREEZE_BUFFER,MEN_OPEN_BUFFER,
		MEN_SAVE_BUFFER_AS,

		MEN_CLEAR_SCREEN,MEN_RESET_FONT,MEN_RESET_STYLES,MEN_RESET_TERMINAL,

		MEN_SERIAL,MEN_MODEM,MEN_SCREEN,MEN_TERMINAL,MEN_SET_EMULATION,MEN_CLIPBOARD,MEN_CAPTURE,MEN_COMMANDS,
		MEN_MISC,MEN_PATH,MEN_TRANSFER_PROTOCOL,MEN_TRANSFER,MEN_TRANSLATION,MEN_MACROS,
		MEN_CURSORKEYS,MEN_FAST_MACROS,MEN_HOTKEYS,MEN_SPEECH,MEN_SOUND,MEN_RATES,MEN_SET_CONSOLE,MEN_OPEN_SETTINGS,MEN_SAVE_SETTINGS,
		MEN_SAVE_SETTINGS_AS,

		MEN_STATUS_WINDOW,MEN_REVIEW_WINDOW,MEN_PACKET_WINDOW,MEN_CHAT_LINE,MEN_FAST_MACROS_WINDOW,
		MEN_UPLOAD_QUEUE_WINDOW,

		/* Avoid recompilation */

		MEN_PRINT_SCREEN_AS_GFX,
		MEN_EXTRA_DIAL,
		MEN_MATRIX_WINDOW,
		MEN_CAPTURE_TO_RAW_FILE,
		MEN_SELECT_ALL
	};

	/* Menu item code limit, required by the quick dial menu. */

#define DIAL_MENU_LIMIT 1000

	/* How many phone book entries will be stored in
	 * the quick dial menu.
	 */

#define DIAL_MENU_MAX	50

	/* The size of the attention scanner buffers. */

#define ATTENTION_BUFFER_SIZE 260

	/* The maximum length of a password. */

#define MAX_PASSWORD_LENGTH 20

	/* Undefine the following symbols, the preferences header file
	 * will use the same names and the same values.
	 */

#undef PARITY_NONE
#undef PARITY_EVEN
#undef PARITY_ODD
#undef PARITY_MARK
#undef PARITY_SPACE

	/* Serial settings. */

enum	{	PARITY_NONE,PARITY_EVEN,PARITY_ODD,PARITY_MARK,PARITY_SPACE };

enum	{	HANDSHAKING_NONE,HANDSHAKING_RTSCTS,HANDSHAKING_RTSCTS_DSR };
enum	{	DUPLEX_FULL,DUPLEX_HALF };
enum	{	FLOW_NONE,FLOW_XON_XOFF };

	/* Terminal settings. */

enum	{	EOL_IGNORE,EOL_CR,EOL_LF,EOL_CRLF,EOL_LFCR };
enum	{	EMULATION_ANSIVT100,EMULATION_ATOMIC,EMULATION_TTY,EMULATION_EXTERNAL,EMULATION_HEX };
enum	{	COLOUR_AMIGA,COLOUR_EIGHT,COLOUR_SIXTEEN,COLOUR_MONO };

	/* Protocol types. */

enum	{	PROTOCOL_XMODEM,PROTOCOL_XMODEMCRC,PROTOCOL_YMODEM,PROTOCOL_ZMODEM };

	/* Font types. */

enum	{	FONT_STANDARD,FONT_IBM,FONT_IBM_RAW };

	/* Bell modes. */

enum	{	BELL_NONE, BELL_VISIBLE, BELL_AUDIBLE, BELL_BOTH, BELL_SYSTEM };

	/* Data flow scanner indices. */

enum	{	SCAN_NOCARRIER, SCAN_CONNECT, SCAN_VOICE, SCAN_RING, SCAN_BUSY, SCAN_NODIALTONE,
		SCAN_OK, SCAN_ERROR, SCAN_SIGDEFAULTUPLOAD, SIG_DEFAULTUPLOAD,
		SCAN_SIGASCIIUPLOAD,SCAN_SIGASCIIDOWNLOAD, SCAN_SIGTEXTUPLOAD,
		SCAN_SIGTEXTDOWNLOAD, SCAN_SIGBINARYUPLOAD,
		SCAN_SIGBINARYDOWNLOAD, SCAN_COUNT
	};

	/* Status types. */

enum	{	STATUS_READY,STATUS_HOLDING,STATUS_DIALING,STATUS_UPLOAD,
		STATUS_DOWNLOAD,STATUS_BREAKING,STATUS_HANGUP,
		STATUS_RECORDING,STATUS_RECORDING_LINE,STATUS_AREXX
	};

	/* Sound types. */

enum	{	SOUND_BELL,SOUND_CONNECT,SOUND_DISCONNECT,SOUND_GOODTRANSFER,
		SOUND_BADTRANSFER,SOUND_RING,SOUND_VOICE,SOUND_ERROR,
		SOUND_COUNT
	};

	/* Settings types. */

enum	{	PREF_ALL,PREF_SERIAL,PREF_MODEM,PREF_COMMAND,PREF_SCREEN,
		PREF_TERMINAL,PREF_PATH,PREF_MISC,PREF_CLIP,PREF_CAPTURE,
		PREF_FILE,PREF_EMULATION,PREF_TRANSFER,PREF_TRANSLATIONFILENAME,
		PREF_MACROFILENAME,PREF_CURSORFILENAME,PREF_FASTMACROFILENAME,
		PREF_SPEECHFILENAME,PREF_SOUNDFILENAME,PREF_AREACODEFILENAME,
		PREF_PHONEBOOKFILENAME,PREF_HOTKEYFILENAME,PREF_TRAPFILENAME,
		PREF_RATES	/* Note: PREF_RATES must always be last */
	};

	/* Gadgets attached to the main window. */

enum	{	CHAT_GadgetID=400 };

	/* Status line modes. */

enum	{	STATUSLINE_DISABLED,STATUSLINE_STANDARD,STATUSLINE_COMPRESSED };

	/* Capture filter modes. */

enum	{	FILTER_NONE,FILTER_ESCAPE,FILTER_CONTROL,FILTER_BOTH };

	/* Scrolling modes. */

enum	{	SCROLL_JUMP,SCROLL_SMOOTH };

	/* Auto-capture file creation. */

enum	{	AUTOCAPTURE_DATE_NAME,AUTOCAPTURE_DATE_INCLUDE };

	/* Cursor and numeric keypad modes. */

enum	{	KEYMODE_STANDARD,KEYMODE_APPLICATION };

	/* Font scales. */

enum	{	SCALE_NORMAL,SCALE_HALF };
enum	{	SCALE_ATTR_NORMAL,SCALE_ATTR_TOP2X,SCALE_ATTR_BOT2X,SCALE_ATTR_2X };

	/* Character tables. */

enum	{	TABLE_ASCII,TABLE_GFX };

	/* Transfer types. */

enum	{	TRANSFER_BINARY,TRANSFER_TEXT,TRANSFER_ASCII };

	/* Translation table entry types. */

enum	{	TRANSLATE_SINGLE,TRANSLATE_STRING };

	/* Upload panel actions. */

enum	{	UPLOAD_TEXT=1,UPLOAD_BINARY,UPLOAD_IGNORE,UPLOAD_ABORT,
		UPLOAD_TEXT_FROM_LIST,UPLOAD_BINARY_FROM_LIST,UPLOAD_DUMMY
	};

	/* Alert types. */

enum	{	ALERT_NONE,ALERT_BEEP,ALERT_SCREEN,ALERT_BEEP_SCREEN };

	/* Online time display modes. */

enum	{	ONLINETIME_TIME,ONLINETIME_COST,ONLINETIME_BOTH };

	/* What to print. */

enum	{	PRINT_SCREEN,PRINT_CLIP };

	/* Dial command types. */

enum	{	DIAL_IGNORE,DIAL_LIST,DIAL_REDIAL,DIAL_MENU };

	/* Errors returned by ReconfigureSerial(). */

enum	{	RECONFIGURE_FAILURE,RECONFIGURE_NOCHANGE,RECONFIGURE_SUCCESS };

	/* Operation codes for SetItem(). */

enum	{	SETITEM_SETCHECK,SETITEM_CLRCHECK,SETITEM_ON,SETITEM_OFF };

	/* File identification actions. */

enum	{	IDENTIFY_IGNORE,IDENTIFY_FILETYPE,IDENTIFY_SOURCE };

	/* Generic list types. */

enum	{	GLIST_UPLOAD,GLIST_DOWNLOAD,GLIST_DIAL,GLIST_WAIT,GLIST_TRAP,GLIST_COUNT };

	/* Generic list add operations. */

enum	{	ADD_GLIST_BOTTOM,ADD_GLIST_TOP,ADD_GLIST_BEHIND,ADD_GLIST_BEFORE };

	/* Dialing modes. */

enum	{	DIALMODE_PULSE, DIALMODE_TONE, DIALMODE_MODEM, DIALMODE_ISDN };

	/* Transfer window gadgets. */

enum	{	GAD_TRANSFER_INFORMATION_LIST=1,
		GAD_TRANSFER_PERCENT,GAD_TRANSFER_TIME,
		GAD_TRANSFER_ABORT,GAD_TRANSFER_SKIP,
		GAD_TRANSFER_PROTOCOL,GAD_TRANSFER_FILE,
		GAD_TRANSFER_SIZE,GAD_TRANSFER_SECONDS,
		GAD_TRANSFER_TOTALTIME,GAD_TRANSFER_ERRORS,
		GAD_TRANSFER_ABORT_FILE
	};

	/* How to satisfy OwnDevUnit.library requests. */

enum	{	ODU_RELEASE,ODU_WAIT,ODU_KEEP };

	/* When to notify the user during a file transfer. */

enum	{	XFERNOTIFY_NEVER,XFERNOTIFY_ALWAYS,XFERNOTIFY_START,XFERNOTIFY_END };

	/* XPR options setting. */

enum	{	GAD_XPROPTIONS_USE=42,GAD_XPROPTIONS_CANCEL };

	/* Terminal types */

enum	{	TERMINAL_VT200,TERMINAL_VT102,TERMINAL_VT101,TERMINAL_VT100 };

	/* Call log file formats */

enum	{	LOGFILEFORMAT_CallInfo,LOGFILEFORMAT_PhoneLog };

	/* AmigaGuide help context. */

enum	{	CONTEXT_MAIN,CONTEXT_SERIAL,CONTEXT_MODEM,CONTEXT_SCREEN,
		CONTEXT_TERMINAL,CONTEXT_CLIP,CONTEXT_CAPTURE,CONTEXT_COMMAND,
		CONTEXT_MISC,CONTEXT_PATHS,CONTEXT_TRANSLATION,CONTEXT_MACROS,
		CONTEXT_CURSOR,CONTEXT_FASTMACROS,CONTEXT_HOTKEYS,CONTEXT_SPEECH,
		CONTEXT_EMULATION,CONTEXT_TRANSFER,CONTEXT_PHONEBOOK,CONTEXT_DATE,
		CONTEXT_DAY,CONTEXT_IMPORT,CONTEXT_RATES,CONTEXT_TIME,
		CONTEXT_COPY,CONTEXT_PASSWORDUSER,CONTEXT_DIAL,CONTEXT_PRINT,
		CONTEXT_TEXTBUFFER,CONTEXT_PACKETWINDOW,CONTEXT_SOUND,
		CONTEXT_LIBS,CONTEXT_ASCII,CONTEXT_PENS,CONTEXT_UPLOAD_QUEUE,
		CONTEXT_TRAPS,CONTEXT_AREA_CODES,CONTEXT_MAIN_MENU,
		CONTEXT_BUFFER_MENU,CONTEXT_PACKET_MENU,CONTEXT_PROJECT_MEN,
		CONTEXT_EDIT_MEN,CONTEXT_CMDS_MEN,CONTEXT_PHONE_MEN,
		CONTEXT_TRANSF_MEN,CONTEXT_BUFFER_MEN,CONTEXT_TERMINAL_MEN,
		CONTEXT_SETTINGS_MEN,CONTEXT_WINDOWS_MEN,CONTEXT_DIAL_MEN,
		CONTEXT_TEXTPEN_PANEL,CONTEXT_ASCII_TRANSFER_SETTINGS,
		CONTEXT_PARAMETERS,CONTEXT_SIGNATURE

	};

	/* Character types. */

enum	{	CHAR_VANILLA,CHAR_CURSOR,CHAR_FUNCTION,CHAR_HELP,
		CHAR_XON,CHAR_XOFF,CHAR_ENTER,CHAR_RETURN };

	/* Review window positioning codes. */

enum	{	REVIEW_MOVE_TOP,REVIEW_MOVE_BOTTOM,REVIEW_MOVE_UP,REVIEW_MOVE_DOWN };

	/* File type classes. */

enum	{	FILETYPE_NONE,FILETYPE_DIR,FILETYPE_FILE,FILETYPE_TEXT,FILETYPE_SOUND,
		FILETYPE_PICTURE,FILETYPE_PREFS,FILETYPE_ARCHIVE,
		FILETYPE_PROGRAM,

		FILETYPE_COUNT };

	/* Text pacing modes. */

enum	{	PACE_DIRECT,PACE_ECHO,PACE_ANYECHO,PACE_PROMPT,PACE_DELAY,PACE_KEYBOARD };

	/* Where to open the buffer text window. */

enum	{	BUFFER_TOP,BUFFER_END };

	/* Where to place the buffer screen horizontally. */

enum	{	SCREEN_LEFT,SCREEN_CENTRE,SCREEN_RIGHT };

	/* File transfer mode. */

enum	{	XFER_XPR,XFER_EXTERNALPROGRAM,XFER_DEFAULT,XFER_INTERNAL };

	/* File transfer protocol signatures. */

enum	{	TRANSFERSIG_DEFAULTUPLOAD,TRANSFERSIG_DEFAULTDOWNLOAD,
		TRANSFERSIG_ASCIIUPLOAD,TRANSFERSIG_ASCIIDOWNLOAD,
		TRANSFERSIG_TEXTUPLOAD,TRANSFERSIG_TEXTDOWNLOAD,
		TRANSFERSIG_BINARYUPLOAD,TRANSFERSIG_BINARYDOWNLOAD
	};

	/* Requester modes. */

enum	{	REQUESTERMODE_CENTRE,REQUESTERMODE_PREFS,
		REQUESTERMODE_IGNORE
	};

	/* Buffer capture modes. */

enum	{	BUFFERMODE_FLOW, BUFFERMODE_REVIEW };

	/* How to sort the phonebook. */

enum	{	SORT_NAME,SORT_NUMBER,SORT_COMMENT,SORT_SELECTION };

	/* How to move list nodes around. */

enum	{	MOVE_HEAD,MOVE_PRED,MOVE_SUCC,MOVE_TAIL };

	/* Some raw key codes. */

#define HELP_CODE		95
#define DEL_CODE		70
#define BACKSPACE_CODE		65
#define RAMIGA_CODE		103
#define TAB_CODE		66

#define CURSOR_UP_CODE		76
#define CURSOR_DOWN_CODE	77
#define CURSOR_RIGHT_CODE	78
#define CURSOR_LEFT_CODE	79

#define F01_CODE		80
#define F02_CODE		81
#define F03_CODE		82
#define F04_CODE		83
#define F05_CODE		84
#define F06_CODE		85
#define F07_CODE		86
#define F08_CODE		87
#define F09_CODE		88
#define F10_CODE		89

#define ALT_KEY			(IEQUALIFIER_LALT|IEQUALIFIER_RALT)
#define SHIFT_KEY		(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)
#define CONTROL_KEY		IEQUALIFIER_CONTROL

	/* A couple of useful minterms. */

#define MINTERM_ZERO		0
#define MINTERM_ONE		ABC | ABNC | ANBC | ANBNC | NABC | NABNC | NANBC | NANBNC
#define MINTERM_COPY		ABC | ABNC | NABC | NABNC
#define MINTERM_NOT_C		ABNC | ANBNC | NABNC | NANBNC
#define MINTERM_B_AND_C		ABC | NABC
#define MINTERM_NOT_B_AND_C	ANBC | NANBC
#define MINTERM_B_OR_C		ABC | ABNC | NABC | NABNC | ANBC | NANBC

	/* Printer control bits. */

#define PRINT_SERIAL	(1L << 0)
#define PRINT_MODEM	(1L << 1)
#define PRINT_SCREEN	(1L << 2)
#define PRINT_TERMINAL	(1L << 3)
#define PRINT_USERNAME	(1L << 4)
#define PRINT_COMMENT	(1L << 5)
#define PRINT_SIZE	(1L << 6)
#define PRINT_DATE	(1L << 7)
#define PRINT_BITS	(1L << 8)

	/* Day of week flags. */

#define DAY_MONDAY	(1L << 0)
#define DAY_TUESDAY	(1L << 1)
#define DAY_WEDNESDAY	(1L << 2)
#define DAY_THURSDAY	(1L << 3)
#define DAY_FRIDAY	(1L << 4)
#define DAY_SATURDAY	(1L << 5)
#define DAY_SUNDAY	(1L << 6)

	/* These signal masks will make life a bit easier. */

#define SIG_WINDOW	(1L << Window -> UserPort -> mp_SigBit)
#define SIG_SERIAL	(ReadPort ? 1L << ReadPort -> mp_SigBit : 0)
#define SIG_TIMER	(1L << TimePort -> mp_SigBit)
#define SIG_QUEUE	(SpecialQueue -> SigMask)
#define SIG_CHECK	(1L << CheckBit)
#define SIG_REXX	(1L << TermRexxPort -> mp_SigBit)
#define SIG_OWNDEVUNIT	(1L << OwnDevBit)

	/* Task termination and handshake signal.
	 * Note: don't try this at home kids.
	 */

#define SIG_KILL	SIGBREAKF_CTRL_C
#define SIG_HANDSHAKE	SIGF_SINGLE

	/* Double-buffered file server command. */

#define	SIG_COMMAND	SIGBREAKF_CTRL_D

	/* ARexx break signal. */

#define SIG_BREAK	SIGBREAKF_CTRL_D

	/* Hotkey reset command. */

#define SIG_RESET	SIGBREAKF_CTRL_D

	/* Skip dial entry. */

#define SIG_SKIP	SIGBREAKF_CTRL_D

	/* Status commands. */

#define SIG_BELL	SIGBREAKF_CTRL_D
#define SIG_RESETTIME	SIGBREAKF_CTRL_E

	/* Buffer special signals. */

#define SIG_TOFRONT	SIGBREAKF_CTRL_D
#define SIG_UPDATE	SIGBREAKF_CTRL_E
#define SIG_MOVEUP	SIGBREAKF_CTRL_F

	/* Status display task. */

#define SIG_CLOSEWINDOW	SIGBREAKF_CTRL_F

	/* Upload queue. */

#define SIG_SHOW	SIGBREAKF_CTRL_D
#define SIG_HIDE	SIGBREAKF_CTRL_E
#define SIG_GOAWAY	SIGBREAKF_CTRL_F

	/* A port signal mask. */

#define PORTMASK(P)	(1L << (((struct MsgPort *)(P)) -> mp_SigBit))

	/* Replacements for CheckItem() and OnMenu()/OffMenu(). */

#define CheckItem(ID,Mode)	SetItem(ID,Mode ? SETITEM_SETCHECK : SETITEM_CLRCHECK)
#define OnItem(ID)		SetItem(ID,SETITEM_ON)
#define OffItem(ID)		SetItem(ID,SETITEM_OFF)

	/* A handy signal macro. */

#define ClrSignal(Signals)	SetSignal(0,Signals)

	/* Another neat macro. */

#define NUM_ELEMENTS(s)		(sizeof(s) / sizeof((s)[0]))

	/* Definitions to access the line signal setting functions supported by
	 * some serial IO boards (namely the ASDG board).
	 */

#define SIOCMD_SETCTRLLINES	(CMD_NONSTD + 7)
#define SIOB_RTSB		0
#define SIOB_DTRB		1
#define SIOB_RTSF		(1L << SIOB_RTSB)
#define SIOB_DTRF		(1L << SIOB_DTRB)

	/* Turn time of day into `compressed' format. */

#define DT_GET_TIME(Hour,Minute) ((Minute) / 10 + (Hour) * 6)

	/* Vector offsets for the TimeDate structure. */

enum	{ DT_FIRST_UNIT,DT_NEXT_UNIT };

	/* Multiply lines and columns with the current font size. */

#define MUL_X(x)		OffsetXTable[x]
#define MUL_Y(y)		OffsetYTable[y]

	/* This macro lets us long-align structures on the stack */

#define D_S(type,name)	char a_##name[sizeof(type)+3]; \
			type *name = (type *)((LONG)(a_##name+3) & ~3);

	/* Like sizeof(..), but for a structure entry. */

#define structsizeof(s,e)	sizeof(((struct s *)0)->e)

	/* A handy and short alias. */

#define TICK			(CHECKIT | MENUTOGGLE)

	/* For now the maximum length of a path/file name. */

#define MAX_FILENAME_LENGTH	256

	/* The maximum width of a line to be stored in the text buffer. */

#define BUFFER_LINE_MAX		255

	/* User interface definitions. */

#define SZ_AutoWidth	TAG_USER+2	/* Use default width? */
#define SZ_NewColumn	TAG_USER+4	/* Start new column */
#define SZ_Lines	TAG_USER+5	/* Number of lines in object */

	/* Box information types. */

enum	{	BOX_LEFT,BOX_TOP,BOX_WIDTH,BOX_HEIGHT };

	/* Status gadget properties. */

#define SGA_FullWidth	(TAG_USER+42)
#define SGA_Mode	(TAG_USER+43)

	/* Sizing gadget sizes. */

#define SIZE_GADGET_WIDTH_HIGH	18
#define SIZE_GADGET_WIDTH_LOW	13

	/* Some windows to be opened on the main screen. */

enum	{	WINDOW_PACKET=100,WINDOW_STATUS,
		WINDOW_REVIEW,WINDOW_FAST,WINDOW_MAIN,
		WINDOW_CHARTAB,WINDOW_QUEUE,WINDOW_FILETRANSFER
	};

	/* Window alignment information. */

#define WC_ALIGNLEFT	(1<<0)
#define WC_ALIGNRIGHT	(1<<1)
#define WC_ALIGNTOP	(1<<2)
#define WC_ALIGNBOTTOM	(1<<3)
#define WC_EXPANDWIDTH	(1<<4)
#define WC_EXPANDHEIGHT	(1<<5)
#define WC_ALIGNSIDE	(1<<6)
#define WC_ALIGNBELOW	(1<<7)

	/* To turn printable characters into control characters. */

#define CONTROL_(x)	(x & 0x1F)

	/* Screen mode list entry. */

struct ModeNode
{
	struct Node	Node;
	ULONG		DisplayID;
};

	/* Trap management. */

struct TrapNode
{
	struct Node	Node;
	LONG		Count,
			SequenceLen;
	STRPTR		Sequence,
			Command;
};

	/* Trap preferences, not much. */

struct TrapSettings
{
	BOOLEAN		Enabled;
	BOOLEAN		Filter;
};

	/* Window alignment data. */

struct WindowInfo
{
	WORD	ID;
	UWORD	WindowFlags;

	LONG	Left,
		Top,
		Width,
		Height;
};

	/* Special character treatment. */

struct SpecialKey
{
	UBYTE	Key;
	JUMP	Routine;
};

	/* Cursor backup data. */

struct CursorData
{
	struct TextFont	*CurrentFont;
	WORD		 CursorX,
			 CursorY;
	UBYTE		 Charset;
	UBYTE		 CharMode[2];
	UBYTE		 Attributes;
	UBYTE		 FgPen,
			 BgPen;
	UBYTE		 Style;
};

	/* File transfer information. */

struct FileTransferNode
{
	struct MinNode	 MinNode;
	ULONG		 Size;
	STRPTR		 Name;
};

struct FileTransferInfo
{
	struct MinList		 FileList;
	ULONG			 TotalSize;
	LONG			 TotalFiles;

	ULONG			 DoneSize;
	LONG			 DoneFiles;

	struct FileTransferNode	*CurrentFile;
	ULONG			 CurrentSize;
};

	/* Global data link. */

struct TermPort
{
	struct MsgPort		 MsgPort;

	struct Window		*TopWindow;

	struct SignalSemaphore	 OpenSemaphore;

	LONG			 ID,OpenCount,HoldIt;
	LONG			 Reserved[5];
};

	/* Buffer search support. */

struct SearchInfo
{
	UBYTE	Distance[256],
		Pattern[256];
	LONG	FoundX,
		FoundY;
	WORD	PatternWidth;
	WORD	Index;
	BOOLEAN	Forward,
		IgnoreCase,
		WholeWords;
};

struct SearchContext
{
	struct Window		*SearchWindow;
	struct LayoutHandle	*SearchHandle;

	UBYTE			 LocalBuffer[256];
	STRPTR			 Buffer;

	BOOLEAN			 Ok;
};

	/* Support for optimized scrolling routines. */

struct ScrollLineInfo
{
	LONG	Left,Right,Width,
		ColourMask;
};

	/* A Fast! macro list entry. */

struct MacroNode
{
	struct Node	Node;
	UBYTE		Macro[30];	/* Macro title */
	UBYTE		Code[256];	/* The execution code. */
};

	/* Speech preferences. */

struct SpeechConfig
{
	WORD	Rate,
		Pitch;
	LONG	Frequency;
	BOOLEAN	Sex,
		Volume,
		Enabled;
	BYTE	Pad;
};

	/* Sound preferences */

struct SoundConfig
{
	UBYTE	BellFile[MAX_FILENAME_LENGTH],

		ConnectFile[MAX_FILENAME_LENGTH],
		DisconnectFile[MAX_FILENAME_LENGTH],

		GoodTransferFile[MAX_FILENAME_LENGTH],
		BadTransferFile[MAX_FILENAME_LENGTH],

		RingFile[MAX_FILENAME_LENGTH],
		VoiceFile[MAX_FILENAME_LENGTH];

	BOOLEAN	Preload;
	BYTE	Pad;
	BOOLEAN	Volume;
	BYTE	Pad2;

	UBYTE	ErrorNotifyFile[MAX_FILENAME_LENGTH];
};

	/* Macro Preferences. */

struct MacroKeys
{
	UBYTE	Keys[4][10][256];
};

	/* Cursor key preferences. */

struct CursorKeys
{
	UBYTE	Keys[4][4][256];
};

	/* Translation table entry. */

struct TranslationEntry
{
	STRPTR	String;		/* Corresponding code string. */
	UBYTE	Type;		/* Table entry type. */
	UBYTE	Len;		/* String length if any. */
	UBYTE	Extra;		/* Saves space, don't need to allocate memory for this one. */
	UBYTE	Null;		/* Terminating null. */
};

struct TranslationHeader
{
	UBYTE	Type;		/* Table entry type. */
	UBYTE	Code;		/* Table offset. */
	UBYTE	Len;		/* Entry length. */
	BYTE	Pad;
};

	/* Buffer translation handle. */

struct TranslationHandle
{
	STRPTR			LocalBuffer;
	LONG			LocalLen;

	STRPTR			SourceBuffer;
	LONG			SourceLen;

	STRPTR			DestinationBuffer;
	LONG			DestinationLen;

	struct TranslationEntry	**Table;
};

	/* term hotkey configuration. */

struct HotkeysOld
{
	UBYTE	termScreenToFront[256];
	UBYTE	BufferScreenToFront[256];
	UBYTE	SkipDialEntry[256];
	BOOLEAN	CommodityPriority;
	BOOLEAN	HotkeysEnabled;

	ULONG	Reserved[25];
};

struct Hotkeys
{
	UBYTE	termScreenToFront[256];
	UBYTE	BufferScreenToFront[256];
	UBYTE	SkipDialEntry[256];
	BOOLEAN	CommodityPriority;
	BOOLEAN	HotkeysEnabled;

	ULONG	Reserved[25];

	UBYTE	AbortARexx[256];
};

	/* Time/date structure. */

struct TimeDate
{
	LONG	Count;		/* Table size. */

	ULONG	PayPerUnit[2];	/* Pay for each unit; stored as ten thousands */
	ULONG	SecPerUnit[2];	/* Number of seconds each unit lasts; stored as ten thousands */

	UBYTE	Time;		/* The time associated with this entry. */
	BOOLEAN	Mark;		/* Used by the rates editor to keep track of item being edited. */
};

struct TimeDateOld
{
	LONG	Count;		/* Table size. */
	WORD	PayPerUnit[2],	/* Pay for each unit. */
		SecPerUnit[2];	/* Number of seconds each unit lasts. */
	UBYTE	Time;		/* The time associated with this entry. */
	BYTE	Pad;
};

	/* Time date header information. */

struct TimeDateHeader
{
	UBYTE	Comment[22];	/* Comment for this entry. */

	BOOLEAN	Month,		/* Month of year or -1 -> Day = bitmapped days of week. */
		Day;		/* Day of month or -1 -> standard settings. */
};

struct TimeDateNode
{
	struct Node		 Node;		/* Default node entry. */
	UBYTE			 Buffer[30];	/* Name buffer. */

	struct TimeDate		*Table;		/* Table of time/date entries. */

	struct TimeDateHeader	 Header;	/* Header information. */
};

struct TimeNode
{
	struct Node	Node;		/* Default node entry. */
	UBYTE		Name[20];	/* Name string. */
	UBYTE		Time;		/* The time for this entry. */
};

	/* This node type links phone number patterns
	 * and date/time lists.
	 */

struct PatternNode
{
	struct Node	Node;
	UBYTE		Pattern[256],
			Comment[40];
	struct MinList	List;
};

	/* Serial settings. */

struct SerialSettings
{
	ULONG	BaudRate;		/* Baud rate. */
	ULONG	BreakLength;		/* Length of break signal in microseconds. */
	ULONG	SerialBufferSize;	/* Size of serial read/write buffer. */

	UBYTE	SerialDevice[MAX_FILENAME_LENGTH];
					/* Serial device name. */
	LONG	UnitNumber;		/* Serial device unit number. */

	BYTE	BitsPerChar;		/* Bits per character. */
	BYTE	Parity;			/* Parity check mode. */
	BYTE	StopBits;		/* Number of stop bits. */
	BYTE	HandshakingProtocol;	/* Handshaking protocol (RTS/CTS, etc.). */
	BOOLEAN	Duplex;			/* Full- or half-duplex? */

	BOOLEAN	xONxOFF;		/* xON/xOFF handshaking enabled? */
	BOOLEAN	HighSpeed;		/* Radboogie? */
	BOOLEAN	Shared;			/* Shared device access? */
	BOOLEAN	StripBit8;		/* Strip high-order bit? */
	BOOLEAN	CheckCarrier;		/* Track the carrier signal? */
	BOOLEAN	PassThrough;		/* Pass xON/xOFF characters through to modem? */
	BOOLEAN	UseOwnDevUnit;		/* Enable OwnDevUnit.library? */

	ULONG	Quantum;		/* Buffer read quantum. */

	BOOLEAN	IgnoreCarrier;		/* Ignore carrier signal. */

	BOOLEAN	UseNet;			/* Use DNet-ID. */
	UWORD	NetID;			/* The DNet-ID to use. */

	BOOLEAN	SatisfyODURequests;	/* How to satisfy ODU requests. */
	BOOLEAN	DirectConnection;	/* Directly connected to host? */

	UWORD	LastVersionSaved,	/* The last version the config file was saved with. */
		LastRevisionSaved;

	BOOLEAN	ReleaseODUWhenOnline;	/* Release the device even when online? */
	BOOLEAN	ReleaseODUWhenDialing;	/* Release the device even when dialing? */
	BOOLEAN	NoODUIfShared;		/* Don't use ODU when device is opened in shared mode? */
	BYTE	Pad;
};

	/* Modem settings. */

struct ModemSettings
{
	UBYTE	ModemInit[80];		/* Modem init string. */
	UBYTE 	ModemExit[80];		/* Modem exit string. */
	UBYTE 	ModemHangup[80];	/* Modem hangup string. */
	UBYTE	DialPrefix[80];		/* Dial command prefix. */
	UBYTE	DialSuffix[80];		/* Dial command suffix. */

	UBYTE	NoCarrier[16];		/* `No carrier' string. */
	UBYTE	NoDialTone[16];		/* `No dialtone' string. */
	UBYTE	Connect[16];		/* `Connect' string. */
	UBYTE	Voice[16];		/* `Voice' string. */
	UBYTE	Ring[16];		/* `Ring' string. */
	UBYTE	Busy[16];		/* `Busy' string. */

	LONG	RedialDelay;		/* Length of redial delay. */
	LONG	DialRetries;		/* Number of dial retries. */
	LONG	DialTimeout;		/* Length of dial timeout. */
	BOOLEAN	ConnectAutoBaud;	/* Use baud rate returned by modem? */
	BOOLEAN	DropDTR;		/* Drop the DTR signal on hangup? */
	BOOLEAN	RedialAfterHangup;	/* Redial list after hangup? */
	BOOLEAN	VerboseDialing;		/* Output modem responses while dialing? */

	UBYTE	Ok[16];			/* `Ok' string. */
	UBYTE	Error[16];		/* `Error' string. */

	BOOLEAN	NoCarrierIsBusy;	/* Treat `NO CARRIER' as `BUSY' signal? */
	BOOLEAN	AbortHangsUp;		/* Dialer `Abort' sends hangup string? */

	WORD	ConnectLimit;				/* Connection limit. */
	UBYTE	ConnectLimitMacro[MAX_FILENAME_LENGTH];	/* Macro to be executed when limit is reached. */

	LONG	TimeToConnect;		/* Number of seconds to pass between modem picking up
					 * the line and issuing the `CONNECT' message.
					 */

	BOOLEAN	DialMode;		/* Use tone or pulse dialing? */
	BOOLEAN	DoNotSendMainModemCommands;	/* Do not send modem init/exit commands when dialing and using the main configuration? */

	LONG	InterDialDelay;		/* Seconds to wait between two successive dial attempts. */

	LONG	CharSendDelay;		/* Microseconds to wait between chars to send. */

	UBYTE	PBX_Prefix[80];		/* Additional commands to insert between dial prefix and phone number. */
	BOOLEAN	PBX_Mode;		/* Send the prefix above. */
	BYTE	Pad;
};

	/* Command settings. */

struct CommandSettings
{
	UBYTE	StartupMacro[256];	/* Startup macro. */
	UBYTE	LogoffMacro[256];	/* Macro executed after carrier has dropped or hung up. */
	UBYTE	UploadMacro[256];	/* Macro to execute after a successful upload. */
	UBYTE	DownloadMacro[256];	/* Macro to execute after a successful download. */

	UBYTE	LoginMacro[256];	/* Login macro, complements the logoff macro. */
};

	/* Screen settings. */

struct ScreenSettings
{
	ULONG	DisplayMode;			/* Screen display mode. */
	WORD	ColourMode;			/* The terminal colour mode. */
	UWORD	Colours[16];			/* Colour palette. */
	UBYTE	FontName[MAX_FILENAME_LENGTH];	/* Default user interface font. */
	WORD	FontHeight;			/* Default user interface font height. */

	BOOLEAN	MakeScreenPublic;		/* Are we to make our screen public? */
	BOOLEAN	ShanghaiWindows;		/* Are we to `shanghai' windows? */

	BOOLEAN	Blinking;			/* Screen blinking enabled? */
	BOOLEAN	FasterLayout;			/* Faster screen layout? */
	BOOLEAN	TitleBar;			/* Is the screen title bar enabled? */
	BOOLEAN	StatusLine;			/* Which mode is the status line in? */

	BOOLEAN	UseWorkbench;			/* Use the Workbench screen for the terminal window? */

	UBYTE	PubScreenName[MAXPUBSCREENNAME + 1];	/* Name of public screen to open window upon. */

	BOOLEAN	TimeMode;			/* Online time/fee display. */
	BOOLEAN	Depth;				/* Screen depth if any, 0 = don't bother. */

	BOOLEAN	UsePens;			/* Use special screen pens? */
	BYTE	PenColourMode;			/* For which colour mode was the pen array built? */

	UWORD	PenArray[16];			/* Pen index array. */

	ULONG	DisplayWidth,			/* Screen width. */
		DisplayHeight;			/* Screen height. */
	UWORD	OverscanType;			/* Screen overscan type. */

	BOOLEAN	ShareScreen;			/* Don't open a backdrop window, share pens. */
	BOOLEAN	SplitStatus;			/* Split the status line from the main window. */

	BYTE	Pad;
	BOOLEAN	UseColours96;			/* Use the 96 bit colours? */
	ULONG	Colours96[16 * 3];		/* The 96 bit colour table. */
};

	/* Terminal settings. */

struct TerminalSettings
{
	BOOLEAN	BellMode;				/* The bell mode. */
	BOOLEAN	AlertMode;				/* The user alert mode. */
	BOOLEAN	EmulationMode;				/* The terminal emulation mode. */
	BOOLEAN	FontMode;				/* The font mode. */

	BOOLEAN	SendCR;					/* Standard translations. */
	BOOLEAN	SendLF;
	BOOLEAN	ReceiveCR;
	BOOLEAN	ReceiveLF;

	UWORD	NumColumns,				/* Size of the terminal window. */
		NumLines;

	UBYTE	KeyMapFileName[MAX_FILENAME_LENGTH];	/* Name of custom keymap file. */
	UBYTE	EmulationFileName[MAX_FILENAME_LENGTH];	/* External emulation file name. */
	UBYTE	BeepFileName[MAX_FILENAME_LENGTH];	/* Name of the custom beep sound file. */

	UBYTE	TextFontName[MAX_FILENAME_LENGTH];	/* Terminal text font name. */
	WORD	TextFontHeight;				/* Terminal text font height. */

	BOOLEAN	UseTerminalTask;			/* Enable the terminal emulation process. */
	BOOLEAN	AutoSize;				/* Terminal size should be auto adapted. */

	UBYTE	IBMFontName[MAX_FILENAME_LENGTH];	/* Terminal text font name. */
	WORD	IBMFontHeight;				/* Terminal text font height. */
};

	/* Path settings. */

struct PathSettings
{
	UBYTE	ASCIIUploadPath[MAX_FILENAME_LENGTH];		/* Default ASCII upload file path. */
	UBYTE	ASCIIDownloadPath[MAX_FILENAME_LENGTH];		/* Default ASCII download file path. */

	UBYTE	TextUploadPath[MAX_FILENAME_LENGTH];		/* Default text upload file path. */
	UBYTE	TextDownloadPath[MAX_FILENAME_LENGTH];		/* Default text download file path. */

	UBYTE	BinaryUploadPath[MAX_FILENAME_LENGTH];		/* Default binary data upload file path. */
	UBYTE	BinaryDownloadPath[MAX_FILENAME_LENGTH];	/* Default binary data download file path. */

	UBYTE	DefaultStorage[MAX_FILENAME_LENGTH];		/* Default configuration storage path. */

	UBYTE	Editor[MAX_FILENAME_LENGTH];			/* Text file editor to use. */

	UBYTE	HelpFile[MAX_FILENAME_LENGTH];			/* Help text file. */
};

	/* Miscellaneous settings. */

struct MiscSettings
{
	BOOLEAN	Priority;		/* Program priority. */
	BOOLEAN	BackupConfig;		/* Save the configuration between calls? */

	BOOLEAN	OpenFastMacroPanel;	/* Open the fast! macro panel? */
	BOOLEAN	ReleaseDevice;		/* Release serial device when iconified? */

	BOOLEAN	TransferServer;		/* Enable data transfer server? */
	BOOLEAN	EmulationServer;	/* Enable terminal emulation server? */

	BOOLEAN	OverridePath;		/* Override the protocol transfer path? */
	BOOLEAN	AutoUpload;		/* Enable the auto-upload panel? */
	BOOLEAN	SetArchivedBit;		/* Set the archived bit for files sent? */
	BOOLEAN	IdentifyFiles;		/* Try to identify files after download? */

	BOOLEAN	TransferIcons;		/* Transfer files along with their icons? */
	BOOLEAN	CreateIcons;		/* Create icons for files saved? */

	BOOLEAN	SimpleIO;		/* Use simple file I/O? */
	BOOLEAN	PerfMeter;		/* Transfer performance meter enabled? */

	LONG	IOBufferSize;		/* Size of the asynchronous I/O buffer. */

	BOOLEAN	HideUploadIcon;		/* Hide the upload queue icon? */
	BOOLEAN	ProtectiveMode;		/* Give hints and ask for confirmation? */

	BOOLEAN	AlertMode;		/* The user alert mode. */

	BOOLEAN	RequesterMode;		/* Centre, ignore, use preference? */

	UWORD	RequesterWidth,		/* Width of asl requesters. */
		RequesterHeight;	/* Height of asl requesters. */
	UWORD	RequesterLeft,		/* Left edge of asl requesters. */
		RequesterTop;		/* Top edge of asl requesters. */

	UBYTE	WaitString[256];	/* String to send repeatedly while the "Wait" code is running. */
	LONG	WaitDelay;		/* Time to wait between single commands. */

	UBYTE	WindowName[256];	/* The console window specifier. */
	BOOLEAN	SuppressOutput;		/* Suppress console output? */
	BYTE	Pad;
};

	/* Clipboard settings. */

struct ClipSettings
{
	WORD	ClipboardUnit;		/* Clipboard unit to use. */

	WORD	LineDelay,		/* Insert line delay. */
		CharDelay;		/* Insert character delay. */

	UBYTE	InsertPrefix[80];	/* Line insertion prefix. */
	UBYTE	InsertSuffix[80];	/* Line insertion suffix. */

	UBYTE	LinePrompt[256];	/* Line wait prompt. */
	WORD	SendTimeout;		/* Line send timeout. */
	UBYTE	PacingMode;		/* Text pacing mode. */
	BOOLEAN	ConvertLF;		/* Convert <LF> characters to <CR> */
};

	/* Capture and logfile settings. */

struct CaptureSettings
{
	BOOLEAN	LogActions;				/* Create logfile? */
	BOOLEAN	LogCall;				/* Create callinfo-compatible logfile? */
	UBYTE	LogFileName[MAX_FILENAME_LENGTH];	/* Default logfile name. */

	LONG	MaxBufferSize;				/* Maximum text buffer size. */
	BOOLEAN	BufferEnabled;				/* Is the text buffer enabled? */

	BOOLEAN	ConnectAutoCapture;			/* Open capture file on logon? */
	BOOLEAN	CaptureFilterMode;			/* The capture filter mode. */
	UBYTE	CapturePath[MAX_FILENAME_LENGTH];	/* The default path for capture files. */

	UBYTE	CallLogFileName[MAX_FILENAME_LENGTH];	/* Name of the call log file. */
	UBYTE	BufferPath[MAX_FILENAME_LENGTH];	/* Text buffer save path. */

	BOOLEAN	AutoCaptureDate;			/* Where to include the creation date? */
	UBYTE	SearchHistory;

	BOOLEAN	OpenBufferWindow;			/* Where to open the buffer window. */
	BOOLEAN	OpenBufferScreen;			/* Where to open the buffer screen. */

	BOOLEAN	BufferScreenPosition;			/* Where to place the buffer screen (horizontal) */
	UBYTE	BufferWidth;				/* How long each line in the buffer should be. */

	BOOLEAN	RememberBufferWindow;			/* Remember buffer window context? */
	BOOLEAN	RememberBufferScreen;			/* Remember buffer screen context? */

	ULONG	BufferScreenMode;			/* Screen display mode for buffer screen. */

	BOOLEAN	ConvertChars;				/* Convert IBM characters? */
	BOOLEAN	BufferMode;				/* How the capture buffer should receive data. */

	BYTE	LogFileFormat;				/* Format of the log files. */
	BYTE	Pad;

	ULONG	BufferSafetyMemory;			/* Don't allocate any further memory if
							 * this or less memory is still available.
							 */
};

	/* File settings. */

struct FileSettings
{
	UBYTE	ProtocolFileName[MAX_FILENAME_LENGTH];		/* Transfer protocol file name. */
	UBYTE	TranslationFileName[MAX_FILENAME_LENGTH];	/* Character translation file name. */
	UBYTE	MacroFileName[MAX_FILENAME_LENGTH];		/* Keyboard macro file name. */
	UBYTE	CursorFileName[MAX_FILENAME_LENGTH];		/* Cursor key file name. */
	UBYTE	FastMacroFileName[MAX_FILENAME_LENGTH];		/* Fast macro file name. */
};

	/* Emulation settings. */

struct EmulationSettings
{
	BOOLEAN	CursorMode;		/* Are the cursor keys switched to application mode? */
	BOOLEAN	NumericMode;		/* Is the numeric keypad switched to application mode? */

	BOOLEAN	CursorWrap;		/* Is cursor position wrapping enabled? */
	BOOLEAN	LineWrap;		/* Is character line wrapping enabled? */

	BOOLEAN	InsertMode;		/* Is the character insertion mode enabled? */
	BOOLEAN	NewLineMode;		/* Is the newline mode enabled? */

	BOOLEAN	FontScale;		/* Which font scale is enabled? */
	BOOLEAN	ScrollMode;		/* Is smooth scrolling enabled? */
	BOOLEAN	DestructiveBackspace;	/* Backspace erases characters? */
	BOOLEAN	SwapBSDelete;		/* DEL and BS keys are swapped? */
	BOOLEAN	PrinterEnabled;		/* Printer commands enabled? */
	BOOLEAN	CLSResetsCursor;	/* Clear screen command resets cursor position? */

	UBYTE	AnswerBack[80];		/* Answer-back message. */

	BOOLEAN	KeysLocked;		/* Numeric keypad mode locked? */
	UBYTE	MaxScroll;		/* Maximum number of lines to prescroll. */
	UBYTE	MaxJump;		/* Maximum number of lines to scroll in one jump. */
	BOOLEAN	CursorLocked;		/* Cursor keys locked? */
	BOOLEAN	FontLocked;		/* Font size locked? */
	BOOLEAN	LockWrapping;		/* Line wrapping locked? */
	BOOLEAN	LockStyle;		/* Text style locked? */
	BOOLEAN	LockColour;		/* Text colour locked? */

	BOOLEAN	UseStandardPens;	/* Use the standard pens/attributes? */
	BYTE	TerminalType;		/* How to identify ourselves (VT100 or VT200) */

	UBYTE	Attributes[4];		/* Attribute mappings. */
	UWORD	Pens[16];		/* Rendering pens. */
};

struct TransferSignature
{
	UBYTE	Signature[40];
	WORD	Length;
};

	/* File transfer library settings. */

struct TransferSettings
{
	UBYTE	DefaultLibrary[MAX_FILENAME_LENGTH],
		ASCIIUploadLibrary[MAX_FILENAME_LENGTH],
		ASCIIDownloadLibrary[MAX_FILENAME_LENGTH],
		TextUploadLibrary[MAX_FILENAME_LENGTH],
		TextDownloadLibrary[MAX_FILENAME_LENGTH],
		BinaryUploadLibrary[MAX_FILENAME_LENGTH],
		BinaryDownloadLibrary[MAX_FILENAME_LENGTH];
	BOOLEAN	InternalASCIIUpload,
		InternalASCIIDownload,
		QuietTransfer,
		MangleFileNames;

	WORD	LineDelay,		/* Insert line delay. */
		CharDelay;		/* Insert character delay. */

	UBYTE	LinePrompt[256];	/* Line wait prompt. */
	WORD	SendTimeout;		/* Line send timeout. */
	UBYTE	PacingMode;		/* Text pacing mode. */
	BOOLEAN	StripBit8;		/* Strip high order bit. */
	BOOLEAN	IgnoreDataPastArnold;	/* Ignore data past terminator. */
	UBYTE	TerminatorChar;		/* Terminator character. */
	BYTE	SendCR,
		SendLF,
		ReceiveCR,
		ReceiveLF;

	UWORD	ErrorNotification;	/* Notify user after <n> errors have occured. */
	BYTE	TransferNotification;	/* Notify the user when a transfer has started/finished? */
	BYTE	DefaultType,
		ASCIIDownloadType,
		ASCIIUploadType,
		TextDownloadType,
		TextUploadType,
		BinaryUploadType,
		BinaryDownloadType;

	struct TransferSignature Signatures[TRANSFERSIG_BINARYDOWNLOAD + 1];

	BOOLEAN	OverridePath;		/* Override the protocol transfer path? */
	BOOLEAN	SetArchivedBit;		/* Set the archived bit for files sent? */
	BOOLEAN	IdentifyFiles;		/* Try to identify files after download? */
	BOOLEAN	TransferIcons;		/* Transfer files along with their icons? */
	BOOLEAN	PerfMeter;		/* Transfer performance meter enabled? */
	BOOLEAN	HideUploadIcon;		/* Hide the upload queue icon? */

	UBYTE	IdentifyCommand[256];	/* Command used to identify downloaded files. */

	BOOLEAN	ExpandBlankLines;	/* Insert spaces when transferring blank text lines? */
	BYTE	Pad;
};

	/* The new configuration settings. */

struct Configuration
{
	struct SerialSettings		*SerialConfig;
	struct ModemSettings		*ModemConfig;
	struct ScreenSettings		*ScreenConfig;
	struct TerminalSettings		*TerminalConfig;
	struct EmulationSettings	*EmulationConfig;
	struct ClipSettings		*ClipConfig;
	struct CaptureSettings		*CaptureConfig;
	struct CommandSettings		*CommandConfig;
	struct MiscSettings		*MiscConfig;
	struct PathSettings		*PathConfig;
	struct FileSettings		*FileConfig;
	struct TransferSettings		*TransferConfig;

	STRPTR				TranslationFileName;
	STRPTR				MacroFileName;
	STRPTR				CursorFileName;
	STRPTR				FastMacroFileName;

	STRPTR				SpeechFileName;
	STRPTR				SoundFileName;
	STRPTR				AreaCodeFileName;
	STRPTR				PhonebookFileName;
	STRPTR				HotkeyFileName;
	STRPTR				TrapFileName;
};

	/* A text box for several lines of text. */

struct TextBox
{
	struct TextBox		*NextBox;		/* Next box in chain. */

	LONG			 Left,Top,		/* Position and size. */
				 Width,Height;

	LONG			 LineWidth,		/* Line width in pixels. */
				 LineHeight;		/* Line height in pixels. */

	LONG			 NumChars,		/* Number of chars per line. */
				 NumLines;		/* Number of lines. */

	STRPTR			*Title,			/* Line titles. */
				*Text;			/* Line texts. */

	LONG			 TitleFgPen,
				 TitleBgPen,
				 TextPen;
};

	/* Block marker structure. */

struct BlockMarker
{
		/* The object to manipulate. */

	struct RastPort *	RPort;

		/* Origin anchor point. */

	LONG			OriginX,
				OriginY;

		/* First and last selected line. */

	LONG			FirstLine,
				LastLine;

		/* First and last selected column. */

	LONG			FirstColumn,
				LastColumn;

		/* Top of display window, lines in the buffer and size of
		 * the display window.
		 */

	LONG			Top,
				Lines,
				Width,
				Height;

		/* Canvas left and top edge. */

	LONG			LeftEdge,
				TopEdge;

		/* Last mouse position. */

	LONG			LastX,
				LastY;

		/* Single character dimensions. */

	LONG			TextFontWidth,
				TextFontHeight;

		/* Plane write mask. */

	UBYTE			WriteMask;
};

	/* Clipboard marker callback routine. */

typedef VOID (* MARKER)(struct BlockMarker *,LONG,LONG,LONG,LONG);

	/* Global data flow <-> term interface. */

struct FlowInfo
{
	BOOLEAN	Changed;

	BOOLEAN	NoCarrier;

	BOOLEAN	Connect,
		Voice,
		Ring,
		Busy,
		NoDialTone,
		Ok,
		Error;

	BOOLEAN	Signature;
};

	/* A scan sequence, somewhat similar to the FlowInfo structure. */

struct WaitNode
{
	struct Node	Node;
	LONG		Count;		/* Number of characters matched. */
	STRPTR		Response;	/* Immediate response. */
	LONG		ResponseLen;	/* Length of immediate response string. */
};

	/* A rudimentary dial node, as employed by the ARexx interface */

struct GenericDialNode
{
	struct Node	Node;		/* The node name */
	STRPTR		Number;		/* The number to dial if any */
	LONG		Index;		/* The phonebook entry to dial */
};

	/* A list as employed by the ARexx interface. */

struct GenericList
{
	struct MinList		 ListHeader;
	struct Node		*ListNode;
	LONG			 ListOffset;
	LONG			 ListCount;
	struct SignalSemaphore	 ListSemaphore;
	APTR			 UserData;
	GENERICNOTIFY		 Notify;
};

	/* Number of buffers to be used for buffered I/O. */

#define BUFFER_NUMBER		2

	/* Auxiliary structure for buffered file I/O. */

struct Buffer
{
	struct Message		 Message;			/* Vanilla message header. */

	struct MsgPort		*InfoPort;			/* The filing system the destination file handle is bound to. */
	struct InfoData		 InfoData;

	BOOLEAN			 SimpleIO;			/* Real simple I/O? */
	BYTE			 Pad;

	BOOLEAN			 Read;				/* Last access has read data. */
	BOOLEAN			 Written;			/* Last access has written data. */

	LONG			 Action;			/* Action to perform. */
	LONG			 ActionData[2];			/* Seek and the like. */
	LONG			 Result;			/* Return value. */

	BPTR			 FileHandle;			/* Dos filehandle. */

	UBYTE			*Data;				/* Data buffer. */
	LONG			 BufLength;			/* Length of data buffer. */

	LONG			 BufPosition;			/* Read/write pointer into buffer. */

	LONG			 ReadBufFull;			/* How many bytes are still to be read from the buffer? */
	LONG			 WriteBufFull;			/* How many bytes are still to be written to the buffer?*/

	LONG			 RealPosition;			/* Real position in file. */
	LONG			 Cached;			/* How many bytes in pre-load cache. */

	UBYTE			*DataBuffer[BUFFER_NUMBER];	/* The data buffers. */
	LONG			 DataLength[BUFFER_NUMBER];	/* The lengths of the data buffers. */
	WORD			 DataCount;			/* Which buffer to use. */
	BOOLEAN			 WriteAccess;			/* TRUE if a write file handle. */
	BOOLEAN			 LastActionFailed;		/* TRUE if last action failed -> abort actions. */

	struct Process		*Child;
	struct Process		*Caller;			/* Synchronization. */

	BOOLEAN			 Fresh;				/* TRUE if no read/write access has taken place yet. */
	BOOLEAN			 Used;				/* Did any access take place at all? */

	struct DateStamp	 OpenDate;			/* Date and time when file was opened. */
};

	/* xpr serial bits. */

#define ST_PARTYON	(1L << 0)
#define ST_PARTYODD	(1L << 1)
#define ST_7WIRE	(1L << 2)
#define ST_QBREAK	(1L << 3)
#define ST_RADBOOGIE	(1L << 4)
#define ST_SHARED	(1L << 5)
#define ST_EOFMODE	(1L << 6)
#define ST_XDISABLED	(1L << 7)
#define ST_PARTYMARKON	(1L << 8)
#define ST_PARTYMARK	(1L << 9)
#define ST_2BITS	(1L << 10)
#define ST_READ7	(1L << 11)
#define ST_WRITE7	(1L << 12)

	/* Miscellaneous definitions. */

#define MILLION		1000000

	/* Raster text line attributes. */

#define	ATTR_UNDERLINE	1
#define	ATTR_HIGHLIGHT	2
#define ATTR_BLINK	4
#define	ATTR_INVERSE	8

enum	{	TEXTATTR_UNDERLINE,TEXTATTR_HIGHLIGHT,
		TEXTATTR_BLINK,TEXTATTR_INVERSE,
		TEXTATTR_NONE
	};

	/* Audio channels. */

#define LEFT0F  1
#define RIGHT0F  2
#define RIGHT1F  4
#define LEFT1F  8

	/* Program revision and the appropriate info structure. */

struct TermInfo
{
	UWORD	Version;
	UWORD	Revision;
};

/**********************************************************************/


	/* A message queue handle */

struct MsgQueue
{
	struct SignalSemaphore	 Access;	/* Access semaphore */
	struct MinList		 MsgList;	/* This is where the queue items go */

	LONG			 QueueSize,	/* The length of the queue */
				 MaxSize;	/* The maximum size of the queue, may be 0 */

	struct MinList		 WaitList;	/* The list of tasks to wait for a wakeup signal */

	struct Task		*SigTask;	/* The owner of this handle */
	ULONG			 SigMask;	/* The signal mask to wake up the owner */
	WORD			 SigBit;	/* The bit allocated for the mask, may be -1 */

	BOOLEAN			 Discard;	/* Discard or queue new items? */
};

	/* A message item as used by the queue manager */

struct MsgItem
{
	struct MinNode		 MinNode;	/* List node link */
	DESTRUCTOR		 Destructor;	/* Destructor routine if any */
};

	/* Your simple message item destructor setup */

#define InitMsgItem(Item,Dest)	(((struct MsgItem *)Item) -> Destructor = (Dest))


/**********************************************************************/


enum	{	DATAMSGTYPE_WRITE,DATAMSGTYPE_WRITECLIP,
		DATAMSGTYPE_UPDATEREVIEW,DATAMSGTYPE_MOVEREVIEW,
		DATAMSGTYPE_SERIALCOMMAND,DATAMSGTYPE_UPLOAD,
		DATAMSGTYPE_COMMANDDONE,DATAMSGTYPE_MENU,
		DATAMSGTYPE_RENDEZVOUS,DATAMSGTYPE_SERIALCOMMANDNOECHO,
		DATAMSGTYPE_CLEARBUFFER,DATAMSGTYPE_REDIAL,
		DATAMSGTYPE_HANGUP,DATAMSGTYPE_GOONLINE,
		DATAMSGTYPE_DIAL,DATAMSGTYPE_WRITESTRING,
		DATAMSGTYPE_WRITESTRING_NOECHO
	};

	/* An extension of the MsgItem data type */

struct DataMsg
{
	struct MsgItem	Item;	/* The message item link */

	LONG		Type;	/* Message type */
	UBYTE *		Data;	/* Pointer to data area */
	LONG		Size;	/* Size of data area */

	struct Task *	Client;	/* Client task to signal */
	ULONG		Mask;	/* Client signal mask */
};

	/* An extension of the DataMsg data type */

struct DataReplyMsg
{
	struct DataMsg	Data;		/* DataMsg link */

	struct Task *	Creator;	/* Creator to notify */
	ULONG		CreatorMask;	/* Creator notification signal */
};

	/* Another extension of the DataMsg data type */

struct DataDialMsg
{
	struct DataMsg	Data;		/* DataMsg link */

	struct RexxMsg *DialMsg;	/* RexxMsg to reply when done */
	struct List *	DialList;	/* List of entries to dial */
};


/**********************************************************************/


	/* Serial device attributes */

#define SERA_Baud		TAG_USER+1	/* Baud rate */
#define SERA_BreakTime		TAG_USER+2	/* Break time */
#define SERA_BitsPerChar	TAG_USER+3	/* Bits per character */
#define SERA_StopBits		TAG_USER+4	/* Number of stop bits */
#define SERA_BufferSize		TAG_USER+5	/* Read/Write buffer size */
#define SERA_Parity		TAG_USER+6	/* Parity */
#define SERA_Handshaking	TAG_USER+7	/* Handshaking mode */
#define SERA_HighSpeed		TAG_USER+8	/* Highspeed mode */
#define SERA_Shared		TAG_USER+9	/* Shared access */


/**********************************************************************/


	/* One of the emulation callback routines. */

typedef VOID (* EPTR)(STRPTR Buffer);

	/* This structure describes an ANSI control sequence. */

struct ControlCode
{
	UBYTE	FirstChar;
	STRPTR	Match,
		Terminator;
	UBYTE	LastChar;

	BYTE	ExactSize;
	EPTR	Func;
};


/**********************************************************************/


	/* A 96 bit colour entry. */

typedef struct ColourEntry
{
	WORD	One,
		Which;

	ULONG	Red,
		Green,
		Blue;
} ColourEntry;

	/* A table of colour entries, ready for LoadRGB32(). */

typedef struct ColourTable
{
	WORD			NumColours;

	struct ColourEntry	Entry[0];

	WORD			Terminator;
} ColourTable;

	/* Spread a byte across a long word. */

#define SPREAD(v)	((ULONG)(v) << 24 | (ULONG)(v) << 16 | (ULONG)(v) << 8 | (v))


/**********************************************************************/


	/* AmigaUW terminal window resize support. */

#define UWCMD_TTYRESIZE (CMD_NONSTD+20)


/**********************************************************************/


	/* A phone list entry. */

typedef struct PhoneNode
{
	struct Node		 Node;		/* A standard node header. */
	UBYTE			 LocalName[60];	/* Name of this entry; this includes
						 * a prefix assigned if this entry is
						 * marked to go into the dial list.
						 */
	struct PhoneEntry	*Entry;		/* Points to the curresponding phonebook entry. */
} PhoneNode;

	/* A dial list entry. */

struct DialNode
{
	struct Node		Node;		/* List link */
	struct PhoneEntry *	Entry;		/* Points to the configuration data */
	STRPTR			Number;		/* Holds the number to dial if any */
	BOOL			FromPhonebook;	/* TRUE if this entry was put here because */
						/* it was tagged in the phonebook */
	STRPTR			InitCommand;	/* Modem setup command, if any. */
	STRPTR			ExitCommand;	/* Modem cleanup command, if any. */
	STRPTR			DialPrefix;	/* Modem dial prefix, if any. */
	STRPTR			DialSuffix;	/* Modem dial suffix, if any. */
};

	/* Phonebook entry header. */

typedef struct PhoneHeader
{
	UBYTE		Name[40],		/* BBS name. */
			Number[100],		/* Phone number. */
			Comment[100];		/* Comment. */

	UBYTE		UserName[100],		/* User name for this BSS. */
			Password[100];		/* Password for user name. */

	BOOLEAN		QuickMenu;		/* This entry appears in the quick dial menu? */
	BOOLEAN		NoRates;		/* Do not use rate information. */

	ULONG		ID;			/* Unique ID of this entry. */

	struct timeval	Creation;		/* Creation time. */

	BOOLEAN		AutoDial;		/* Put this entry on the autodial list. */
	BYTE		Pad;

	LONG		Marked;			/* > 0 if marked for dialing. */
} PhoneHeader;

	/* A phonebook entry. */

typedef struct PhoneEntry
{
	struct PhoneNode	*Node;		/* Points back to attached list node if any;
						 * this node would be on display in the
						 * phonebook list view.
						 */
	struct DialNode		*DialNode;	/* Points back to attached list node if any;
						 * this node would be on the dial list.
						 */

	struct MinList		 TimeDateList;	/* Rates and costs. */

	LONG			 Count;		/* Denotes the order in which this entry is to
						 * go into the dial list. A value < 0 indicates
						 * that this entry is not going to go into the
						 * dial list.
						 */

	struct PhoneGroupNode	*ThisGroup;	/* The group in which this entry is found. A
						 * value of NULL indicates that this entry is
						 * not a memer of a group.
						 */
	struct PhoneNode	*GroupNode;	/* Points to the group node attached to this
						 * entry. A value of NULL indicates that this
						 * entry is not a member of a group.
						 */

	struct PhoneHeader	*Header;	/* Points to phone book header. */
	struct Configuration	*Config;	/* Points to configuration data. */
} PhoneEntry;

typedef struct PhonebookHandle
{
	struct PhoneEntry	**Phonebook;		/* Table of phonebook entries. */
	LONG			 PhoneSize;		/* Total size of the phonebook table */
	LONG			 NumPhoneEntries;	/* Number of table entries actually used. */
	struct MinList		 PhoneGroupList;	/* The list of groups used by this phonebook. */
	ULONG			 PhonebookID;		/* A counter every phonebook entry receives
							 * its unique ID from.
							 */
	ULONG			 DefaultGroup;		/* Default group to show after loading. */
	UBYTE			 PhonePassword[30];	/* The access password assigned to this phonebook. */
	BOOLEAN			 PhonePasswordUsed;	/* Indicates whether the password is active or not. */
	BOOLEAN			 AutoDial;		/* Indicates whether this phonebook has the
							 * automatic startup dial feature enabled.
							 */
	BOOLEAN			 AutoExit;		/* Indicates whether this phonebook has the
							 * automatic startup dial & exit feature enabled.
							 */
	LONG			 DialMarker;		/* Number of entries marked for dialing. */

	struct List		*DialList;		/* The dial list attached to this phonebook. */

	struct List		*PhoneList;		/* List of phonebook entries, ready to display
							 * in a list view.
							 */
	struct SignalSemaphore	 ActiveSemaphore;	/* Must be locked before messing with the active entry. */
	PhoneEntry		*ActiveEntry;		/* Currently active phonebook entry. */
} PhonebookHandle;

typedef struct PhoneGroupNode
{
	struct Node		 Node;			/* A standard node header. */
	UBYTE			 LocalName[40];		/* The name of the group. */
	struct MinList		 GroupList;		/* The phonebook entries assigned to this group. */
} PhoneGroupNode;

typedef struct PhonebookGlobals
{
	ULONG			 Count;			/* Number of entries in this phonebook */
	ULONG			 ID;			/* ID counter for this phonebook */
	ULONG			 DefaultGroup;		/* Number of the group the phonebook displays when first opened */
	BOOLEAN			 AutoDial;		/* Start dialing after program is started */
	BOOLEAN			 AutoExit;		/* Exit program after list is dialed */
} PhonebookGlobals;

typedef struct PhoneGroupHeader
{
	UBYTE			 FullName[40];		/* Name of this very group */
	LONG			 Count;			/* Number of entries to follow */
	BYTE			 Pad[8];		/* For future extension, if necessary */
} PhoneGroupHeader;

typedef struct PhoneListContext
{
	PhonebookHandle		*PhoneHandle;		/* The phonebook currently in use */
	struct List		*CurrentList;		/* List currently being displayed */
	LONG			 NumEntries;		/* Number of entries in that list */
	STRPTR			*GroupLabels;		/* The labels to go into the group display */
	struct MsgPort		*WindowPort;		/* The MsgPort to use for the windows attached */
										/* to this control panel */
	PhoneNode		*SelectedNode;		/* Currently active entry (from the list) */
	LONG			 SelectedIndex;		/* Currently active entry (from the table) */

	UBYTE			 WindowTitle[80];	/* The main window title */

	LayoutHandle		*Manager;		/* The main window layout handle */
	LayoutHandle		*Editor;		/* The editor window layout handle */
	LayoutHandle		*Selector;
	LayoutHandle		*Grouping;
	LayoutHandle		*Sorting;
	LayoutHandle		*PasswordHandle;

	struct Window		*Window;		/* The main window */
	struct Window		*EditWindow;
	struct Window		*PatternWindow;
	struct Window		*GroupWindow;
	struct Window		*SortWindow;
	struct Window		*PasswordWindow;

	BOOL			 Hide;			/* Whether or not password and user name should be hidden */
} PhoneListContext;


/**********************************************************************/


	/* Window backfill support */

typedef struct LayerMsg
{
	ULONG			Layer;
	struct Rectangle	Bounds;
} LayerMsg;


/**********************************************************************/


	/* Information table entry types. */

enum	{	INFO_STEM,INFO_TEXT,INFO_NUMERIC,INFO_BOOLEAN,INFO_MAPPED };

	/* Data types to save or open. */

enum	{	DATATYPE_TRANSLATIONS,DATATYPE_FUNCTIONKEYS,DATATYPE_CURSORKEYS,
		DATATYPE_FASTMACROS,DATATYPE_HOTKEYS,DATATYPE_SPEECH,DATATYPE_SOUND,DATATYPE_BUFFER,
		DATATYPE_CONFIGURATION,DATATYPE_PHONEBOOK,DATATYPE_SCREENTEXT,
		DATATYPE_SCREENIMAGE,DATATYPE_COUNT };

	/* Requester types. */

enum	{	REQUESTER_SERIAL,REQUESTER_MODEM,REQUESTER_SCREEN,
		REQUESTER_TERMINAL,REQUESTER_EMULATION,REQUESTER_CLIPBOARD,
		REQUESTER_CAPTURE,REQUESTER_COMMANDS,REQUESTER_MISC,
		REQUESTER_PATH,REQUESTER_TRANSFER,REQUESTER_TRANSLATIONS,
		REQUESTER_FUNCTIONKEYS,REQUESTER_CURSORKEYS,REQUESTER_FASTMACROS,
		REQUESTER_HOTKEYS,REQUESTER_SPEECH,REQUESTER_SOUND,REQUESTER_PHONE,
		REQUESTER_COUNT };

	/* Window types. */

enum	{	WINDOWID_BUFFER,WINDOWID_REVIEW,WINDOWID_PACKET,WINDOWID_FASTMACROS,
		WINDOWID_STATUS,WINDOWID_MAIN,WINDOWID_UPLOAD_QUEUE,WINDOWID_SINGLE_CHAR_ENTRY,
		WINDOWID_COUNT
	};

	/* The maximum length of an ARexx result variable. */

#define MAX_RESULT_LEN	65536

	/* ARexx interface error codes. */

#define TERMERROR_NO_DATA_TO_PROCESS			1000
#define TERMERROR_INDEX_OUT_OF_RANGE			1001
#define TERMERROR_UNKNOWN_OBJECT			1002
#define TERMERROR_RESULT_VARIABLE_REQUIRED		1003
#define TERMERROR_UNIT_NOT_AVAILABLE			1004
#define TERMERROR_DEVICE_DRIVER_STILL_OPEN		1005
#define TERMERROR_CLIPBOARD_ERROR			1006
#define TERMERROR_INFORMATION_IS_READ_ONLY		1007
#define TERMERROR_DATA_TYPES_INCOMPATIBLE		1008
#define TERMERROR_LIST_IS_ALREADY_EMPTY			1009
#define TERMERROR_UNKNOWN_LIST				1010
#define TERMERROR_WRONG_LIST				1011
#define TERMERROR_UNKNOWN_COMMAND			1012

	/* An information table entry node. */

struct AttributeEntry
{
	WORD	 Level;		/* Node level */
	STRPTR	 Name;		/* Name or NULL for numeric value */
	UBYTE	 Type;		/* Node type (INFO_TEXT through INFO_MAPPED) */
	BOOLEAN	 ReadOnly;	/* If information happens to be read-only */
	STRPTR	*Mappings;	/* Type mappings if applicable */
	WORD	 NodeID;	/* Node ID code */
};

	/* Argument mutual exclusion information. */

struct ExclusionInfo
{
	BYTE	A,B;
};

	/* A special message and argument packet. */

struct RexxPkt
{
	struct Message		 Message;

	struct RexxMsg		*RexxMsg;
	STRPTR			*Array;
	struct RDArgs		*Args;
	struct CommandInfo	*CommandInfo;
	LONG			 Results[2];
};

	/* A special function key pointer. */

typedef STRPTR (* COMMAND)(struct RexxPkt *);

	/* Argument inclusion bits. */

#define INCLUDE_00 (1 <<  0)
#define INCLUDE_01 (1 <<  1)
#define INCLUDE_02 (1 <<  2)
#define INCLUDE_03 (1 <<  3)
#define INCLUDE_04 (1 <<  4)
#define INCLUDE_05 (1 <<  5)
#define INCLUDE_06 (1 <<  6)
#define INCLUDE_07 (1 <<  7)
#define INCLUDE_08 (1 <<  8)
#define INCLUDE_09 (1 <<  9)
#define INCLUDE_10 (1 << 10)
#define INCLUDE_11 (1 << 11)

	/* Rexx command information. */

struct CommandInfo
{
	BOOLEAN	Async;
	BOOLEAN	AutoCleanup;
	COMMAND	Routine;
	STRPTR	Name,
		Arguments;
};


/**********************************************************************/


enum	{	ATTR_APPLICATION,
		ATTR_APPLICATION_VERSION,
		ATTR_APPLICATION_SCREEN,
		ATTR_APPLICATION_SESSION,
		ATTR_APPLICATION_SESSION_ONLINE,
		ATTR_APPLICATION_SESSION_SESSIONSTART,
		ATTR_APPLICATION_SESSION_BYTESSENT,
		ATTR_APPLICATION_SESSION_BYTESRECEIVED,
		ATTR_APPLICATION_SESSION_CONNECTMESSAGE,
		ATTR_APPLICATION_SESSION_BBSNAME,
		ATTR_APPLICATION_SESSION_BBSNUMBER,
		ATTR_APPLICATION_SESSION_BBSCOMMENT,
		ATTR_APPLICATION_SESSION_USERNAME,
		ATTR_APPLICATION_SESSION_ONLINEMINUTES,
		ATTR_APPLICATION_SESSION_ONLINECOST,
		ATTR_APPLICATION_AREXX,
		ATTR_APPLICATION_LASTERROR,
		ATTR_APPLICATION_TERMINAL,
		ATTR_APPLICATION_TERMINAL_ROWS,
		ATTR_APPLICATION_TERMINAL_COLUMNS,
		ATTR_APPLICATION_BUFFER,
		ATTR_APPLICATION_BUFFER_SIZE,
		ATTR_PHONEBOOK,
		ATTR_PHONEBOOK_COUNT,
		ATTR_PHONEBOOK_X,
		ATTR_PHONEBOOK_X_NAME,
		ATTR_PHONEBOOK_X_NUMBER,
		ATTR_PHONEBOOK_X_COMMENTTEXT,
		ATTR_PHONEBOOK_X_USERNAME,
		ATTR_PHONEBOOK_X_PASSWORDTEXT,
		ATTR_PREFS_SERIAL,
		ATTR_PREFS_SERIAL_BAUDRATE,
		ATTR_PREFS_SERIAL_BREAKLENGTH,
		ATTR_PREFS_SERIAL_BUFFERSIZE,
		ATTR_PREFS_SERIAL_DEVICENAME,
		ATTR_PREFS_SERIAL_UNIT,
		ATTR_PREFS_SERIAL_BITSPERCHAR,
		ATTR_PREFS_SERIAL_PARITYMODE,
		ATTR_PREFS_SERIAL_STOPBITS,
		ATTR_PREFS_SERIAL_HANDSHAKINGMODE,
		ATTR_PREFS_SERIAL_DUPLEXMODE,
		ATTR_PREFS_SERIAL_XONXOFF,
		ATTR_PREFS_SERIAL_HIGHSPEED,
		ATTR_PREFS_SERIAL_SHARED,
		ATTR_PREFS_SERIAL_STRIPBIT8,
		ATTR_PREFS_SERIAL_CARRIERCHECK,
		ATTR_PREFS_SERIAL_PASSXONXOFFTHROUGH,
		ATTR_PREFS_SERIAL_QUANTUM,
		ATTR_PREFS_SERIAL_USE_OWNDEVUNIT,
		ATTR_PREFS_SERIAL_OWNDEVUNIT_REQUEST,
		ATTR_PREFS_SERIAL_DIRECT_CONNECTION,
		ATTR_PREFS_SERIAL_RELEASE_WHEN_ONLINE,
		ATTR_PREFS_SERIAL_RELEASE_WHEN_DIALING,
		ATTR_PREFS_SERIAL_NO_ODU_IF_SHARED,
		ATTR_PREFS_SERIAL_LOCAL_ECHO,
		ATTR_PREFS_MODEM,
		ATTR_PREFS_MODEM_MODEMINITTEXT,
		ATTR_PREFS_MODEM_MODEMEXITTEXT,
		ATTR_PREFS_MODEM_MODEMHANGUPTEXT,
		ATTR_PREFS_MODEM_DIALPREFIXTEXT,
		ATTR_PREFS_MODEM_DIALSUFFIXTEXT,
		ATTR_PREFS_MODEM_NOCARRIERTEXT,
		ATTR_PREFS_MODEM_NODIALTONETEXT,
		ATTR_PREFS_MODEM_CONNECTTEXT,
		ATTR_PREFS_MODEM_VOICETEXT,
		ATTR_PREFS_MODEM_RINGTEXT,
		ATTR_PREFS_MODEM_BUSYTEXT,
		ATTR_PREFS_MODEM_OKTEXT,
		ATTR_PREFS_MODEM_ERRORTEXT,
		ATTR_PREFS_MODEM_REDIALDELAY,
		ATTR_PREFS_MODEM_DIALRETRIES,
		ATTR_PREFS_MODEM_DIALTIMEOUT,
		ATTR_PREFS_MODEM_CONNECTAUTOBAUD,
		ATTR_PREFS_MODEM_HANGUPDROPSDTR,
		ATTR_PREFS_MODEM_REDIALAFTERHANGUP,
		ATTR_PREFS_MODEM_VERBOSEDIALING,
		ATTR_PREFS_MODEM_NOCARRIERISBUSY,
		ATTR_PREFS_MODEM_CONNECTLIMIT,
		ATTR_PREFS_MODEM_CONNECTLIMITMACRO,
		ATTR_PREFS_MODEM_TIME_TO_CONNECT,
		ATTR_PREFS_MODEM_DIAL_MODE,
		ATTR_PREFS_MODEM_INTER_DIAL_DELAY,
		ATTR_PREFS_MODEM_CHAR_SEND_DELAY,
		ATTR_PREFS_MODEM_DO_NOT_SEND_COMMANDS,
		ATTR_PREFS_MODEM_PBX_PREFIX,
		ATTR_PREFS_MODEM_PBX_MODE,
		ATTR_PREFS_COMMANDS,
		ATTR_PREFS_COMMANDS_STARTUPMACROTEXT,
		ATTR_PREFS_COMMANDS_LOGOFFMACROTEXT,
		ATTR_PREFS_COMMANDS_UPLOADMACROTEXT,
		ATTR_PREFS_COMMANDS_DOWNLOADMACROTEXT,
		ATTR_PREFS_COMMANDS_LOGINMACROTEXT,
		ATTR_PREFS_SCREEN,
		ATTR_PREFS_SCREEN_WINDOW_BORDER,
		ATTR_PREFS_SCREEN_SPLIT_STATUS,
		ATTR_PREFS_SCREEN_COLOURMODE,
		ATTR_PREFS_SCREEN_FONTNAME,
		ATTR_PREFS_SCREEN_FONTSIZE,
		ATTR_PREFS_SCREEN_MAKESCREENPUBLIC,
		ATTR_PREFS_SCREEN_SHANGHAIWINDOWS,
		ATTR_PREFS_SCREEN_BLINKING,
		ATTR_PREFS_SCREEN_FASTERLAYOUT,
		ATTR_PREFS_SCREEN_TITLEBAR,
		ATTR_PREFS_SCREEN_STATUSLINEMODE,
		ATTR_PREFS_SCREEN_USEWORKBENCH,
		ATTR_PREFS_SCREEN_PUBSCREENNAME,
		ATTR_PREFS_SCREEN_ONLINEDISPLAY,
		ATTR_PREFS_SCREEN_USEPENS,
		ATTR_PREFS_TERMINAL,
		ATTR_PREFS_TERMINAL_BELLMODE,
		ATTR_PREFS_TERMINAL_EMULATIONMODE,
		ATTR_PREFS_TERMINAL_FONTMODE,
		ATTR_PREFS_TERMINAL_SENDCRMODE,
		ATTR_PREFS_TERMINAL_SENDLFMODE,
		ATTR_PREFS_TERMINAL_RECEIVECRMODE,
		ATTR_PREFS_TERMINAL_RECEIVELFMODE,
		ATTR_PREFS_TERMINAL_NUMCOLUMNS,
		ATTR_PREFS_TERMINAL_NUMLINES,
		ATTR_PREFS_TERMINAL_KEYMAPNAME,
		ATTR_PREFS_TERMINAL_EMULATIONNAME,
		ATTR_PREFS_TERMINAL_FONTNAME,
		ATTR_PREFS_TERMINAL_FONTSIZE,
		ATTR_PREFS_TERMINAL_USETERMINALPROCESS,
		ATTR_PREFS_TERMINAL_AUTOSIZE,
		ATTR_PREFS_PATHS,
		ATTR_PREFS_PATHS_ASCIIUPLOADPATH,
		ATTR_PREFS_PATHS_ASCIIDOWNLOADPATH,
		ATTR_PREFS_PATHS_TEXTUPLOADPATH,
		ATTR_PREFS_PATHS_TEXTDOWNLOADPATH,
		ATTR_PREFS_PATHS_BINARYUPLOADPATH,
		ATTR_PREFS_PATHS_BINARYDOWNLOADPATH,
		ATTR_PREFS_PATHS_CONFIGPATH,
		ATTR_PREFS_PATHS_EDITORNAME,
		ATTR_PREFS_PATHS_HELPFILENAME,
		ATTR_PREFS_MISC,
		ATTR_PREFS_MISC_PRIORITY,
		ATTR_PREFS_MISC_BACKUPCONFIG,
		ATTR_PREFS_MISC_OPENFASTMACROPANEL,
		ATTR_PREFS_MISC_RELEASEDEVICE,
		ATTR_PREFS_MISC_OVERRIDEPATH,
		ATTR_PREFS_MISC_AUTOUPLOAD,
		ATTR_PREFS_MISC_SETARCHIVEDBIT,
		ATTR_PREFS_MISC_COMMENTMODE,
		ATTR_PREFS_MISC_TRANSFERICONS,
		ATTR_PREFS_MISC_CREATEICONS,
		ATTR_PREFS_MISC_SIMPLEIO,
		ATTR_PREFS_MISC_TRANSFER_PERFMETER,
		ATTR_PREFS_MISC_IOBUFFERSIZE,
		ATTR_PREFS_MISC_HIDE_UPLOAD_ICON,
		ATTR_PREFS_MISC_OVERWRITE_WARNING,
		ATTR_PREFS_MISC_ALERTMODE,
		ATTR_PREFS_MISC_REQUESTERMODE,
		ATTR_PREFS_MISC_REQUESTERWIDTH,
		ATTR_PREFS_MISC_REQUESTERHEIGHT,
		ATTR_PREFS_MISC_WAITSTRING,
		ATTR_PREFS_MISC_WAITDELAY,
		ATTR_PREFS_MISC_CONSOLEWINDOW,
		ATTR_PREFS_MISC_SUPPRESSOUTPUT,
		ATTR_PREFS_CLIPBOARD,
		ATTR_PREFS_CLIPBOARD_UNIT,
		ATTR_PREFS_CLIPBOARD_LINEDELAY,
		ATTR_PREFS_CLIPBOARD_CHARDELAY,
		ATTR_PREFS_CLIPBOARD_INSERTPREFIXTEXT,
		ATTR_PREFS_CLIPBOARD_INSERTSUFFIXTEXT,
		ATTR_PREFS_CLIPBOARD_TEXTPACING,
		ATTR_PREFS_CLIPBOARD_LINEPROMPT,
		ATTR_PREFS_CLIPBOARD_SENDTIMEOUT,
		ATTR_PREFS_CLIPBOARD_CONVERTLF,
		ATTR_PREFS_CAPTURE,
		ATTR_PREFS_CAPTURE_LOGACTIONS,
		ATTR_PREFS_CAPTURE_LOGCALLS,
		ATTR_PREFS_CAPTURE_LOGFILENAME,
		ATTR_PREFS_CAPTURE_MAXBUFFERSIZE,
		ATTR_PREFS_CAPTURE_BUFFER,
		ATTR_PREFS_CAPTURE_CONNECTAUTOCAPTURE,
		ATTR_PREFS_CAPTURE_AUTOCAPTUREDATE,
		ATTR_PREFS_CAPTURE_CAPTUREFILTER,
		ATTR_PREFS_CAPTURE_CAPTUREPATH,
		ATTR_PREFS_CAPTURE_CALLLOGFILENAME,
		ATTR_PREFS_CAPTURE_BUFFERSAVEPATH,
		ATTR_PREFS_CAPTURE_OPENBUFFERWINDOW,
		ATTR_PREFS_CAPTURE_OPENBUFFERSCREEN,
		ATTR_PREFS_CAPTURE_BUFFERSCREENPOSITION,
		ATTR_PREFS_CAPTURE_BUFFERWIDTH,
		ATTR_PREFS_CAPTURE_REMEMBER_BUFFERWINDOW,
		ATTR_PREFS_CAPTURE_REMEMBER_BUFFERSCREEN,
		ATTR_PREFS_CAPTURE_SEARCH_HISTORY,
		ATTR_PREFS_CAPTURE_CONVERTCHARACTERS,
		ATTR_PREFS_CAPTURE_BUFFERMODE,
		ATTR_PREFS_CAPTURE_BUFFERSAFETYMEMORY,
		ATTR_PREFS_FILE,
		ATTR_PREFS_FILE_TRANSFERPROTOCOLNAME,
		ATTR_PREFS_FILE_TRANSLATIONFILENAME,
		ATTR_PREFS_FILE_MACROFILENAME,
		ATTR_PREFS_FILE_CURSORFILENAME,
		ATTR_PREFS_FILE_FASTMACROFILENAME,
		ATTR_PREFS_EMULATION,
		ATTR_PREFS_EMULATION_IDENTIFICATION,
		ATTR_PREFS_EMULATION_CURSORMODE,
		ATTR_PREFS_EMULATION_NUMERICMODE,
		ATTR_PREFS_EMULATION_CURSORWRAP,
		ATTR_PREFS_EMULATION_LINEWRAP,
		ATTR_PREFS_EMULATION_INSERTMODE,
		ATTR_PREFS_EMULATION_NEWLINEMODE,
		ATTR_PREFS_EMULATION_FONTSCALEMODE,
		ATTR_PREFS_EMULATION_SCROLLMODE,
		ATTR_PREFS_EMULATION_DESTRUCTIVEBACKSPACE,
		ATTR_PREFS_EMULATION_SWAPBSDELETE,
		ATTR_PREFS_EMULATION_CURSORLOCKED,
		ATTR_PREFS_EMULATION_FONTLOCKED,
		ATTR_PREFS_EMULATION_NUMPADLOCKED,
		ATTR_PREFS_EMULATION_PRINTERENABLED,
		ATTR_PREFS_EMULATION_ANSWERBACKTEXT,
		ATTR_PREFS_EMULATION_CLS_RESETS_CURSOR,
		ATTR_PREFS_EMULATION_MAXSCROLL,
		ATTR_PREFS_EMULATION_MAXJUMP,
		ATTR_PREFS_EMULATION_WRAPLOCKED,
		ATTR_PREFS_EMULATION_STYLELOCKED,
		ATTR_PREFS_EMULATION_COLOURLOCKED,
		ATTR_PREFS_EMULATION_USEPENS,
		ATTR_PREFS_TRANSFER,
		ATTR_PREFS_TRANSFER_DEFAULT_LIBRARY,
		ATTR_PREFS_TRANSFER_ASCII_UPLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_INTERNAL_ASCII_UPLOAD,
		ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_INTERNAL_ASCII_DOWNLOAD,
		ATTR_PREFS_TRANSFER_QUIET_TRANSFER,
		ATTR_PREFS_TRANSFER_TEXT_UPLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_BINARY_UPLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_LIBRARY,
		ATTR_PREFS_TRANSFER_MANGLE_FILE_NAMES,
		ATTR_PREFS_TRANSFER_LINEDELAY,
		ATTR_PREFS_TRANSFER_CHARDELAY,
		ATTR_PREFS_TRANSFER_TEXTPACING,
		ATTR_PREFS_TRANSFER_LINEPROMPT,
		ATTR_PREFS_TRANSFER_SENDTIMEOUT,
		ATTR_PREFS_TRANSFER_STRIP_BIT_8,
		ATTR_PREFS_TRANSFER_IGNORE_DATA_PAST_ARNOLD,
		ATTR_PREFS_TRANSFER_TERMINATOR_CHAR,
		ATTR_PREFS_TRANSFER_ERROR_NOTIFY_COUNT,
		ATTR_PREFS_TRANSFER_ERROR_NOTIFY_WHEN,
		ATTR_PREFS_TRANSFER_OVERRIDEPATH,
		ATTR_PREFS_TRANSFER_SETARCHIVEDBIT,
		ATTR_PREFS_TRANSFER_COMMENTMODE,
		ATTR_PREFS_TRANSFER_TRANSFERICONS,
		ATTR_PREFS_TRANSFER_TRANSFER_PERFMETER,
		ATTR_PREFS_TRANSFER_HIDE_UPLOAD_ICON,
		ATTR_PREFS_TRANSFER_DEFAULT_TYPE,
		ATTR_PREFS_TRANSFER_DEFAULT_SEND_SIGNATURE,
		ATTR_PREFS_TRANSFER_DEFAULT_RECEIVE_SIGNATURE,
		ATTR_PREFS_TRANSFER_ASCII_UPLOAD_TYPE,
		ATTR_PREFS_TRANSFER_ASCII_UPLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_TYPE,
		ATTR_PREFS_TRANSFER_ASCII_DOWNLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_TEXT_UPLOAD_TYPE,
		ATTR_PREFS_TRANSFER_TEXT_UPLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_TYPE,
		ATTR_PREFS_TRANSFER_TEXT_DOWNLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_BINARY_UPLOAD_TYPE,
		ATTR_PREFS_TRANSFER_BINARY_UPLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_TYPE,
		ATTR_PREFS_TRANSFER_BINARY_DOWNLOAD_SIGNATURE,
		ATTR_PREFS_TRANSFER_IDENTIFY_COMMAND,
		ATTR_PREFS_TRANSFER_EXPAND_BLANK_LINES,
		ATTR_PREFS_SPEECH,
		ATTR_PREFS_SPEECH_RATE,
		ATTR_PREFS_SPEECH_PITCH,
		ATTR_PREFS_SPEECH_FREQUENCY,
		ATTR_PREFS_SPEECH_SEXMODE,
		ATTR_PREFS_SPEECH_VOLUME,
		ATTR_PREFS_SPEECH_SPEECH,
		ATTR_PREFS_TRANSLATIONS,
		ATTR_PREFS_TRANSLATIONS_X,
		ATTR_PREFS_TRANSLATIONS_X_SEND,
		ATTR_PREFS_TRANSLATIONS_X_RECEIVE,
		ATTR_PREFS_FASTMACROS,
		ATTR_PREFS_FASTMACROS_COUNT,
		ATTR_PREFS_FASTMACROS_X,
		ATTR_PREFS_FASTMACROS_X_NAME,
		ATTR_PREFS_FASTMACROS_X_CODE,
		ATTR_PREFS_HOTKEYS,
		ATTR_PREFS_HOTKEYS_TERMSCREENTOFRONTTEXT,
		ATTR_PREFS_HOTKEYS_BUFFERSCREENTOFRONTTEXT,
		ATTR_PREFS_HOTKEYS_SKIPDIALENTRYTEXT,
		ATTR_PREFS_HOTKEYS_ABORTAREXX,
		ATTR_PREFS_HOTKEYS_COMMODITYPRIORITY,
		ATTR_PREFS_HOTKEYS_HOTKEYSENABLED,
		ATTR_PREFS_CURSORKEYS,
		ATTR_PREFS_CURSORKEYS_UPTEXT,
		ATTR_PREFS_CURSORKEYS_RIGHTTEXT,
		ATTR_PREFS_CURSORKEYS_DOWNTEXT,
		ATTR_PREFS_CURSORKEYS_LEFTTEXT,
		ATTR_PREFS_CURSORKEYS_SHIFT,
		ATTR_PREFS_CURSORKEYS_SHIFT_UPTEXT,
		ATTR_PREFS_CURSORKEYS_SHIFT_RIGHTTEXT,
		ATTR_PREFS_CURSORKEYS_SHIFT_DOWNTEXT,
		ATTR_PREFS_CURSORKEYS_SHIFT_LEFTTEXT,
		ATTR_PREFS_CURSORKEYS_ALT,
		ATTR_PREFS_CURSORKEYS_ALT_UPTEXT,
		ATTR_PREFS_CURSORKEYS_ALT_RIGHTTEXT,
		ATTR_PREFS_CURSORKEYS_ALT_DOWNTEXT,
		ATTR_PREFS_CURSORKEYS_ALT_LEFTTEXT,
		ATTR_PREFS_CURSORKEYS_CONTROL,
		ATTR_PREFS_CURSORKEYS_CONTROL_UPTEXT,
		ATTR_PREFS_CURSORKEYS_CONTROL_RIGHTTEXT,
		ATTR_PREFS_CURSORKEYS_CONTROL_DOWNTEXT,
		ATTR_PREFS_CURSORKEYS_CONTROL_LEFTTEXT,
		ATTR_PREFS_FUNCTIONKEYS,
		ATTR_PREFS_FUNCTIONKEYS_X,
		ATTR_PREFS_FUNCTIONKEYS_SHIFT,
		ATTR_PREFS_FUNCTIONKEYS_SHIFT_X,
		ATTR_PREFS_FUNCTIONKEYS_ALT,
		ATTR_PREFS_FUNCTIONKEYS_ALT_X,
		ATTR_PREFS_FUNCTIONKEYS_CONTROL,
		ATTR_PREFS_FUNCTIONKEYS_CONTROL_X,
		ATTR_PREFS_PROTOCOL,
		ATTR_PREFS_CONSOLE,
		ATTR_PREFS_SOUND,
		ATTR_PREFS_SOUND_BELLNAME,
		ATTR_PREFS_SOUND_CONNECTNAME,
		ATTR_PREFS_SOUND_DISCONNECTNAME,
		ATTR_PREFS_SOUND_GOODTRANSFERNAME,
		ATTR_PREFS_SOUND_BADTRANSFERNAME,
		ATTR_PREFS_SOUND_RINGNAME,
		ATTR_PREFS_SOUND_VOICENAME,
		ATTR_PREFS_SOUND_PRELOAD,
		ATTR_PREFS_SOUND_ERROR,
		ATTR_PREFS_SOUND_VOLUME
	};


/**********************************************************************/


	/* How many characters we will keep in the scan buffer. */

#define MAX_SCAN_SIZE	255

typedef struct ParseContext
{
	STRPTR	pc_Arnie;
	WORD	pc_CharsInBuffer,
		pc_ScanStep;
	UBYTE	pc_SaveBuffer[MAX_SCAN_SIZE + 1];

	UBYTE	pc_LineBuffer[BUFFER_LINE_MAX + 1];
	WORD	pc_LineLen,
		pc_LineIndex;

	BOOLEAN	pc_InSequence;
} ParseContext;


/**********************************************************************/


	/* Sound replay information. */

struct SoundInfo
{
	UBYTE		 Name[MAX_FILENAME_LENGTH];

	ULONG		 Rate,
			 Length,
			 Volume;

	APTR		 SoundData;

	APTR		 LeftData,
			 RightData;

	Object		*SoundObject;
	struct timeval	 SoundTime;
};


/**********************************************************************/


	/* Text buffer private data (render info & window). */

typedef struct TextBufferInfo
{
	UWORD			 Left,Top,
				 Width,Height;

	struct Process		*Buddy;

	struct Window		*Window;
	struct Screen		*Screen;
	struct Menu		*BufferMenuStrip;

	BYTE			 BufferSignal;
	BOOLEAN			 BufferTerminated;

	struct RastPort		*RPort;

	LONG			 NumBufferLines,
				 NumBufferColumns,
				 LastTopLine;

	struct DrawInfo		*BufferDrawInfo;
	struct Image		*BufferAmigaGlyph,
				*BufferCheckGlyph;

	struct TextFont		*LocalFont;

	WORD			 LocalTextFontWidth,
				 LocalTextFontHeight;

	struct TTextAttr	 LocalTextFont;
	UBYTE			 LocalTextFontName[MAX_FILENAME_LENGTH];

	struct TTextAttr	 LocalUserFont;
	UBYTE			 LocalUserFontName[MAX_FILENAME_LENGTH];

	LONG			 TopLine,
				 DisplayedLines;

	BOOL			 SearchForward,
				 IgnoreCase,
				 WholeWords,
				 NeedClipConversion;

	Object			*Scroller,
				*UpArrow,
				*DownArrow;

	Object			*UpImage,
				*DownImage;

	LONG			 ArrowWidth;

	UWORD			*BufferLineOffsets,
				*BufferColumnOffsets;

	UWORD			 TitleFrontPen,
				 TitleBackPen,
				 TextFrontPen,
				 TextBackPen;

	WORD			 OldColumn,
				 OldLine,
				 OldLength;

	LONG			 TitleOffset;
	LONG			 MaxPen;
	WORD			 RightBorderWidth;

	struct Screen		*Parent;

	UBYTE			 TitleBuffer[100];

	struct MsgQueue		*Queue;
	ULONG			 QueueMask;

	struct MarkerContext	*Marker;
	BOOLEAN			 Interrupted;
	BYTE			 Pad[3];

	BOOL			*pSearchForward;
	BOOL			*pIgnoreCase;
	BOOL			*pWholeWords;
	LONG			*pTopLine;
} TextBufferInfo;


/**********************************************************************/


	/* The catalog data is stored in the following format. */

struct CatCompArrayType
{
	ULONG	cca_ID;
	STRPTR	cca_Str;
};


/**********************************************************************/


#ifdef __GNUC__
struct LaunchMsg;
#endif	/* __GNUC__ */

	/* This is what we can do. */

enum	{ LAUNCH_Program,LAUNCH_RexxCmd,LAUNCH_RexxString };

	/* A callback routine to handle the cleanup work. */

typedef VOID (*LAUNCHCLEANUP)(struct LaunchMsg *Message);

typedef struct LaunchMsg
{
	struct Message	 Message;	/* Standard message header. */

	WORD		 Type;		/* What type of command this message implements. */

	struct RexxPkt	*RexxPkt;	/* An ARexx interface command packet. */

	LONG		 Result;	/* Primary result code. */
	LONG		 Result2;	/* Secondary result code. */

	LAUNCHCLEANUP	 Cleanup;	/* A cleanup routine that gets call when all the work is done. */

	UBYTE		 Command[1];	/* The command to execute, if any. */
} LaunchMsg;


/**********************************************************************/


#ifdef __GNUC__
struct JobNode;
struct JobQueue;
#endif	/* __GNUC__ */

typedef BOOL (*JOBFUNCTION)(struct JobNode *);

typedef ULONG (*JOBQUEUEWAIT)(struct JobQueue *);

	/* The job types supported. */

enum { JOBTYPE_Once,JOBTYPE_Always,JOBTYPE_Disposable,JOBTYPE_Wait };

typedef struct JobNode
{
	struct Node		 Node;		/* Link and name. */
	WORD			 Type;		/* Job type. */
	BOOL			 Active;	/* Whether or not this job is currently active. */

	struct JobQueue		*HomeQueue;	/* In which queue to find this job. */

	JOBFUNCTION		 Function;	/* The function to invoke. */
	JOBFUNCTION		 SubFunction;	/* If necessary, the function invoked by
						 * the default entry point.
						 */
	ULONG			 Mask;		/* Signal to wait for. */
} JobNode;

typedef struct JobQueue
{
	struct MinList		 ReadyList;	/* List of jobs ready for execution. */

	ULONG			 ReadyMask;	/* Signal mask to wait for events. */
	LONG			 OnceCounter;	/* Number of jobs to call once only. */

	JobNode			*NextJob;	/* The next job to execute. */

	JOBQUEUEWAIT		 QueueWait;	/* The routine to wait for events to happen. */

	struct SignalSemaphore	 AccessLock;	/* Access lock for this data structure. */
} JobQueue;

	/* To initialize the jobs from scratch. */

struct JobInitStruct
{
	STRPTR		Name;		/* Name of this job, if any. */
	JobNode **	Node;		/* The node to initialize. */
	JOBFUNCTION	Function;	/* The function to invoke. */
	WORD		Type;		/* The job type. */
};

/**********************************************************************/

	/* For LimitedVSprintf() and friends. */

struct FormatContext
{
	STRPTR	Index;
	LONG	Size;
};

/**********************************************************************/

enum
{
	MARKERASK_Clipped,
	MARKERASK_Scroll
};

	/* Ask for the mouse position to be converted into a line and column index. */

typedef VOID (*MARKER_AskPosition)(struct MarkerContext *Context,LONG *Column,LONG *Line,WORD Mode);

	/* Scroll the visible region. */

typedef VOID (*MARKER_Scroll)(struct MarkerContext *Context,LONG DeltaX,LONG DeltaY);

	/* Highlight a line of text between the "Left" and "Right" column (inclusive).
	 * If both parameters are < 0 the entire line should be highlighted.
	 */

typedef VOID (*MARKER_Highlight)(struct MarkerContext *Context,LONG Line,LONG Left,LONG Right);

	/* Transfer the buffer contents line by line. */

typedef APTR (*MARKER_TransferStartStop)(struct MarkerContext *Context,APTR UserData,ULONG Qualifier);
typedef BOOL (*MARKER_Put)(APTR UserData,STRPTR Buffer,LONG Length);
typedef BOOL (*MARKER_Transfer)(struct MarkerContext *Context,LONG Line,LONG Left,LONG Right,MARKER_Put Put,APTR UserData);
typedef BOOL (*MARKER_PickWord)(struct MarkerContext *Context,LONG Left,LONG Top,LONG *WordLeft,LONG *WordRight);

typedef struct MarkerContext
{
	LONG	FirstVisibleLine,	/* The first line visible on the screen. */
		NumVisibleLines;	/* The number of lines visible on the screen. */
	LONG	NumLines;		/* The number of text lines in the buffer. */

	LONG	FirstVisibleColumn,	/* The first column visible on the screen. */
		NumVisibleColumns;	/* The number of columns visible on the screen. */
	LONG	NumColumns;		/* The number of text columns used by the buffer. */

	LONG	AnchorColumn,		/* The current marker anchor position on the anchor line. */
		AnchorLine;		/* The line on which the marker is anchored. */

	LONG	CurrentColumn,		/* This is where the mouse pointer currently is. */
		CurrentLine;

	LONG	FirstFullLine,		/* The first and the last line that is completely highlighted. */
		LastFullLine;

	LONG	FirstPartialLine,	/* The first line that has text marked in it. */
		FirstPartialLeft,	/* The first column in which text is marked (inclusive). */
		FirstPartialRight;	/* The last column in which text is marked (inclusive). */

	LONG	LastPartialLine,	/* The last line that has text marked in it. */
		LastPartialLeft,	/* The first column in which text is marked (inclusive). */
		LastPartialRight;	/* The last column in which text is marked (inclusive). */

	LONG	Direction;		/* If -1, user is dragging the mouse towards the top left corner,
					 * otherwise user is dragging the mouse towards the bottom right corner.
					 */

		/* Ask for the mouse position to be converted into a line and column index. */

	MARKER_AskPosition AskPosition;

		/* Scroll the visible region. */

	MARKER_Scroll Scroll;

		/* Highlight a line of text between the "Left" and "Right" column (inclusive).
		 */

	MARKER_Highlight Highlight;

		/* Unhighlight a line of text between the "Left" and "Right" column (inclusive).
		 */

	MARKER_Highlight Unhighlight;

		/* Prepare for data transfer or clean up after the transfer. */

	MARKER_TransferStartStop TransferStartStop;

		/* Transfer the contents of a buffer line. */

	MARKER_Transfer Transfer;

		/* Called by the Transfer function. */

	MARKER_Put Put;
	MARKER_Put PutLine;

		/* Pick the word at the given position. */

	MARKER_PickWord PickWord;

		/* User defined data. */

	APTR UserData;
} MarkerContext;

/**********************************************************************/

	/* And now for something completely different... */

#ifndef _PROTOS_H
#include "Protos.h"
#endif	/* _PROTOS_H */

#ifndef _DATA_H
#include "Data.h"
#endif	/* _DATA_H */

#ifndef _ERRORS_H
#include "Errors.h"
#endif	/* _ERRORS_H */

#endif	/* _GLOBAL_H */
