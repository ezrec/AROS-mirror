/**
 *
 *   Include file for External Emulator Handling
 *
 **/

#ifndef _COMPILER_H
#define REG(x) register __ ## x
#define ASM __asm
#endif	/* _COMPILER_H */

/*
 *   The xem_option structure
 */

struct xem_option
{
	STRPTR	xemo_description;	/* description of the option                  */
	ULONG	xemo_type;		/* type of option                             */
	STRPTR	xemo_value;		/* pointer to a buffer with the current value */
	ULONG	xemo_length;		/* buffer size                                */
};

/*
 *   Valid values for xemo_type are:
 */

#define XEMO_BOOLEAN 1L         /* xemo_value is "yes", "no", "on" or "off"   */
#define XEMO_LONG    2L         /* xemo_value is string representing a number */
#define XEMO_STRING  3L         /* xemo_value is a string                     */
#define XEMO_HEADER  4L         /* xemo_value is ignored                      */
#define XEMO_COMMAND 5L         /* xemo_value is ignored                      */
#define XEMO_COMMPAR 6L         /* xemo_value contains command parameters     */

struct XEmulatorMacroKey
{
	struct MinNode	xmk_Node;
	UWORD		xmk_Qualifier;
	UBYTE		xmk_Type;
	UBYTE		xmk_Code;
	APTR		xmk_UserData;
};

/*----- Qualifiers for XMK_QUALIFIER -----*/

#define XMKQ_NONE      0
#define XMKQ_SHIFT     1
#define XMKQ_ALTERNATE 2
#define XMKQ_CONTROL   3

/*----- Types for XMK_TYPE -----*/

#define XMKT_IGNORE 1
#define XMKT_RAWKEY 2
#define XMKT_COOKED 3


/*----- Types for XEmulatorInfo -----*/

#define XEMI_CURSOR_POSITION	1	/* home = 1,1 */
#define XEMI_CONSOLE_DIMENSIONS	2

/*----- Macros for XEmulatorInfo -----*/

#define XEMI_EXTRACT_Y_POSITION(result)	(result >> 16)
#define XEMI_EXTRACT_X_POSITION(result)	(result & 0xffff)

#define XEMI_EXTRACT_COLUMNS(result)	(result >> 16)
#define XEMI_EXTRACT_LINES(result)	(result & 0xffff)

/*----- Modes for XEmulatorPreferences -----*/

#define XEM_PREFS_RESET	0	/* Reset to builtin defaults	*/
#define XEM_PREFS_LOAD	1	/* Load preferences from file	*/
#define XEM_PREFS_SAVE	2	/* Save preferences to file	*/


struct XEmulatorHostData
{
	STRPTR	Source;
	STRPTR	Destination;
	BOOL	InESC;		/* INTER-EMULATOR PRIVATE DATA..  DO NOT TOUCH 'EM..!! */
	BOOL	InCSI;		/* INTER-EMULATOR PRIVATE DATA..  DO NOT TOUCH 'EM..!! */
};

/*
 *   The structure
 */

struct XEM_IO
{
	struct Window	*xem_window;
	struct TextFont	*xem_font;
	APTR		 xem_console;
	STRPTR		 xem_string;
	ULONG		*xem_signal;
	UWORD		 xem_screendepth;
	UWORD		 xem_pad;

	LONG		(* ASM xem_sread)(REG(a0) APTR,REG(d0) LONG,REG(d1) ULONG);
	LONG		(* ASM xem_swrite)(REG(a0) STRPTR,REG(d0) LONG);
	LONG		(*     xem_sflush)(VOID);
	LONG		(*     xem_sbreak)(VOID);
	LONG		(*     xem_squery)(VOID);
	VOID		(*     xem_sstart)(VOID);
	LONG		(*     xem_sstop)(VOID);

	VOID		(* ASM xem_tbeep)(REG(d0) ULONG,REG(d1) ULONG);
	LONG		(* ASM xem_tgets)(REG(a0) STRPTR,REG(a1) STRPTR,REG(d0) ULONG);
	ULONG		(* ASM xem_toptions)(REG(d0) LONG,REG(a0) struct xem_option **);

	LONG		(* ASM xem_process_macrokeys)(REG(a0) struct XEmulatorMacroKey *);
};

#ifdef __AROS__
#include <proto/xem.h>
#else

BOOL	XEmulatorSetup(struct XEM_IO *io);
BOOL	XEmulatorOpenConsole(struct XEM_IO *io);
VOID	XEmulatorCloseConsole(struct XEM_IO *io);
VOID	XEmulatorCleanup(struct XEM_IO *io);
VOID	XEmulatorWrite(struct XEM_IO *io, STRPTR string, ULONG len);
BOOL	XEmulatorSignal(struct XEM_IO *io, ULONG sig);
ULONG	XEmulatorHostMon(struct XEM_IO *io, struct XEmulatorHostData *hd, ULONG actual);
ULONG	XEmulatorUserMon(struct XEM_IO *io, STRPTR retstr, ULONG maxlen, struct IntuiMessage *imsg);
BOOL	XEmulatorOptions(struct XEM_IO *io);
BOOL	XEmulatorClearConsole(struct XEM_IO *io);
BOOL	XEmulatorResetConsole(struct XEM_IO *io);
BOOL	XEmulatorResetTextStyles(struct XEM_IO *io);
BOOL	XEmulatorResetCharset(struct XEM_IO *io);
ULONG	XEmulatorGetFreeMacroKeys(struct XEM_IO *io, ULONG qualifier);
BOOL	XEmulatorMacroKeyFilter(struct XEM_IO *io, struct List *keys);
ULONG	XEmulatorInfo(struct XEM_IO *xem_io, ULONG type);
BOOL	XEmulatorPreferences(struct XEM_IO *xem_io, STRPTR fileName, ULONG mode);

#ifndef NO_PRAGMAS
#pragma libcall XEmulatorBase XEmulatorSetup 1E 801
#pragma libcall XEmulatorBase XEmulatorOpenConsole 24 801
#pragma libcall XEmulatorBase XEmulatorCloseConsole 2A 801
#pragma libcall XEmulatorBase XEmulatorCleanup 30 801
#pragma libcall XEmulatorBase XEmulatorWrite 36 9803
#pragma libcall XEmulatorBase XEmulatorSignal 3C 802
#pragma libcall XEmulatorBase XEmulatorHostMon 42 9803
#pragma libcall XEmulatorBase XEmulatorUserMon 48 A09804
#pragma libcall XEmulatorBase XEmulatorOptions 4E 801
#pragma libcall XEmulatorBase XEmulatorClearConsole 54 801
#pragma libcall XEmulatorBase XEmulatorResetConsole 5A 801
#pragma libcall XEmulatorBase XEmulatorResetTextStyles 60 801
#pragma libcall XEmulatorBase XEmulatorResetCharset 66 801
#pragma libcall XEmulatorBase XEmulatorGetFreeMacroKeys 6c 802
#pragma libcall XEmulatorBase XEmulatorMacroKeyFilter 72 9802
#pragma libcall XEmulatorBase XEmulatorInfo 78 802
#pragma libcall XEmulatorBase XEmulatorPreferences 7E 9803
#endif	/* NO_PRAGMAS */

#endif
