/* $Revision Header built by Ueli Kaufmann ********** (please edit) **********
*
*  Copyright by Ueli Kaufmann
*
* File             : xembbs.library
* Created on       : Sunday 13-Dec-92 10:47:21
* Created by       : Ueli Kaufmann
* Created with     ; SAS/C 6.0, Cygnus-Ed V2.12, Shell OS2.04
* Current revision : V4.4
*
*
* Purpose
* -------
*   font-sensitive replacement for xemibm.library
*     
* Known Bugs
* ----------
*   doesn't support DEC VT-xxx commands
*
*
* Revision V4.4 (Friday 11-Dec-92 20:13:48)
* -------------
* - converted xemibm.lib to xembbs.lib  (tribute to William Waring)
* - made the beast wbench compatible
* - xembbs will work fine with custom-fonts. But you should use
*   a font like IBM.font for 100% compatibilty with IBM-ANSI..
* - added a blinking/underscore cursor (:
*
*****************************************************************************/



/************************/

/*
**	#include	<dos.h>
**	#include	<string.h>
**	#include	<stdarg.h>
**	/*#include	<m68881.h>	gibt sonst troubles mit <clib/alib_protos.h> */
**	/*#include	<math.h>*/
**	
**	#include <exec/ports.h>
**	#include <exec/lists.h>
**	#include <exec/nodes.h>
**	#include <exec/memory.h>
**	#include <exec/execbase.h>
**	
**	#include <dos/dos.h>
**	#include <dos/dostags.h>
**	#include <dos/dosextens.h>
**	#include <dos/exall.h>
**	#include <dos/rdargs.h>
**	
**	#include	<devices/serial.h>
**	
**	#include <libraries/iffparse.h>
**	#include <libraries/gadtools.h>
**	#include	<libraries/filehandler.h>
**	#include	<libraries/dos.h>
**	#include <libraries/asl.h>
**	#include	<libraries/dosextens.h>
**	#include	<libraries/reqtools.h>
**	
**	#include <intuition/intuitionbase.h>
**	#include <intuition/intuition.h>
**	#include <intuition/screens.h>
**	#include <intuition/gadgetclass.h>
**	#include <intuition/icclass.h>
**	
**	#include <graphics/gfxbase.h>
**	#include <graphics/gfxmacros.h>
**	#include <graphics/displayinfo.h>
**	#include <graphics/view.h>
**	
**	#include <workbench/workbench.h>
**	#include <workbench/startup.h>
**	
**	
**	#include <xpr/xpr_proto.h>
**	#include <proto/reqtools.h>
**	#include <clib/dos_protos.h>
**	#include	<clib/alib_protos.h>
**	#include	<clib/alib_stdio_protos.h>
**	#include <proto/all.h>
**	#include <proto/nofrag.h>
**	#include <pragmas/nofrag_pragmas.h>
*/


#include "ownincs/BBSconsole.h"



/* allowed globals.. */

struct	ExecBase			*SysBase;
struct	DosLibrary		*DOSBase;
struct	GfxBase			*GfxBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*UtilityBase;
struct	Library			*KeymapBase;
struct	Library			*DiskfontBase;

/* utils.a */
VOID *GetSucc(VOID *);
VOID *GetHead(VOID *);
VOID *GetTail(VOID *);
VOID *GetPred(VOID *);
VOID BZero(VOID *, LONG);
VOID BSet(VOID *, LONG, LONG);



struct BBS_Parm {
	VOID (*Routine)(struct BBSConsole *con, UBYTE *args, ULONG argc);
	UBYTE intermediate;	/* FIRST intermediate..!! */
	UBYTE matchChar;
};



                      /* supported ANSI-commands */ 




struct BBS_Parm ESC_cmds[] = {							/* Mnemonic */
	{ BBS_invokeCSI			,	NUL,	'[' },			/* CSI */
	{ BBS_hardreset			,	NUL,	'c' },			/* RIS */

	{ NULL, NUL, NUL },		/* der krönende Abschluss..(-: */
};


struct BBS_Parm CSI_cmds[] = {							/* Mnemonic */
	{ BBS_insertchars			,	NUL,	'@' },			/* ICH */
	{ BBS_cursorup				,	NUL,	'A' },			/* CUU */
	{ BBS_cursordown			,	NUL,	'B' },			/* CUD */
	{ BBS_cursorright			,	NUL,	'C' },			/* CUF */
	{ BBS_cursorleft			,	NUL,	'D' },			/* CUB */
	{ BBS_cursornextline		,	NUL,	'E' },			/* CNL */
	{ BBS_cursoraboveline	,	NUL,	'F' },			/* CPL */
	{ BBS_cursorposition		,	NUL,	'H' },			/* CUP */
	{ BBS_eraseindisplay		,	NUL,	'J' },			/* ED  */
	{ BBS_eraseinline			,	NUL,	'K' },			/* EL  */
	{ BBS_insertlines			,	NUL,	'L' },			/* IL  */
	{ BBS_deletelines			,	NUL,	'M' },			/* DL  */
	{ BBS_deletechars			,	NUL,	'P' },			/* DCH */
	{ BBS_ignore				,	NUL,	'R' },			/* CPR */
	{ BBS_scrollup				,	NUL,	'S' },			/* SU  */
	{ BBS_scrolldown			,	NUL,	'T' },			/* SD  */

	{ BBS_cursorposition		,	NUL,	'f' },			/* HVP */
	{ BBS_ANSIsetmodes		,	NUL,	'h' },			/* SM */
	{ BBS_ANSIresetmodes		,	NUL,	'l' },			/* RM */
	{ BBS_ANSIsetmodes		,	'>',	'h' },			/* SM */
	{ BBS_ANSIresetmodes		,	'>',	'l' },			/* RM */
	{ BBS_ANSIsetmodes		,	'?',	'h' },			/* SM */
	{ BBS_ANSIresetmodes		,	'?',	'l' },			/* RM */
	{ BBS_textmodes			,	NUL,	'm' },			/* SGR */
	{ BBS_devicestatreport	,	NUL,	'n' },			/* DSR */
	{ BBS_savecursor			,	NUL,	's' },			/* */
	{ BBS_restorecursor		,	NUL,	'u' },			/* */

	{ NULL, NUL, NUL },		/* der krönende Abschluss..(-: */
};



VOID __saveds __asm __UserLibCleanup(register __a6 struct Library *libBase)
{
	if(DiskfontBase != NULL)
	{
		CloseLibrary(DiskfontBase);
		DiskfontBase = NULL;
	}

	if(KeymapBase != NULL)
	{
		CloseLibrary(KeymapBase);
		KeymapBase = NULL;
	}

	if(UtilityBase != NULL)
	{
		CloseLibrary(UtilityBase);
		UtilityBase = NULL;
	}

	if(IntuitionBase != NULL)
	{
		CloseLibrary((struct Library *)IntuitionBase);
		IntuitionBase = NULL;
	}

	if(GfxBase != NULL)
	{
		CloseLibrary((struct Library *)GfxBase);
		GfxBase = NULL;
	}

	if(DOSBase != NULL)
	{
		CloseLibrary((struct Library *)DOSBase);
		DOSBase = NULL;
	}
}


int __saveds __asm __UserLibInit(register __a6 struct Library *libBase)
{
	SysBase = *(struct ExecBase **)4;

	if((DOSBase			= (struct DosLibrary *)OpenLibrary("dos.library", 37)) == NULL)
		return(1);

	if((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 37)) == NULL)
		return(1);

	if((GfxBase			= (struct GfxBase *)OpenLibrary("graphics.library", 37)) == NULL)
		return(1);

	if((UtilityBase	= OpenLibrary("utility.library", 37)) == NULL)
		return(1);

	if((KeymapBase		= OpenLibrary("keymap.library", 37)) == NULL)
		return(1);

	if((DiskfontBase	= OpenLibrary("diskfont.library", 33)) == NULL)
		return(1);


	return(0);
}


BOOL __saveds __asm XEmulatorSetup(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con;
	
	io->xem_console = NULL;

	if(con = AllocMem(sizeof(struct BBSConsole), MEMF_PUBLIC | MEMF_CLEAR))
	{
		con->io = io;			/* for easier passing..(-; */
		con->rp = io->xem_window->RPort;
		con->font = io->xem_font;
		io->xem_console = con;
		*con->io->xem_signal = 0;

		BBS_internalsettings(con);		/* just to be on the save side.. */

		return((BOOL)(con->font != NULL));
	}

	return(FALSE);
}


VOID __saveds __asm XEmulatorCleanup(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
	{
		*con->io->xem_signal = 0;
		FreeMem(con, sizeof(struct BBSConsole));
	}
}


BOOL __saveds __asm XEmulatorOpenConsole(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	con->rp = io->xem_window->RPort;
	con->font = io->xem_font;

	switch(con->io->xem_screendepth)
	{
		case 0:
		case 1:		/* MONO */
			con->foregroundPen = 1;
			con->hiLite	= FALSE;
			con->mono	= TRUE;
		break;

		case 2:		/* MONO & BOLD */
			con->foregroundPen = 1;
			con->hiLite	= TRUE;
			con->mono	= TRUE;
		break;

		case 3:		/* COLOR */
			con->foregroundPen = 7;
			con->hiLite	= FALSE;
			con->mono	= FALSE;
		break;

		default:		/* COLOR & BOLD */
			con->foregroundPen = 7;
			con->hiLite	= TRUE;
			con->mono	= FALSE;
		break;
	}

	BBS_newframe(con, NULL);

	BBS_clearwholeframe(con);

	con->isReady = TRUE;

	BBS_reset(con);

	CreateCursorServer(con);
/*		*con->io->xem_signal is already updated.. */
	BBS_cursorVisible(con);

	return((BOOL)(con->font != NULL));
}


VOID __saveds __asm XEmulatorCloseConsole(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	DeleteCursorServer(con);
	*con->io->xem_signal = 0;

	con->isReady = FALSE;
}


VOID __saveds __asm XEmulatorWrite(register __a0 struct XEM_IO *io, register __a1 UBYTE *buff, register __d0 ULONG buflen)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
		BBS_writecon(con, buff, buflen);
}
VOID BBS_writecon(struct BBSConsole *con, UBYTE *buff, ULONG buflen)
{
	UBYTE c, *scp;

	BBS_cursorInvisible(con);

	if((LONG)buflen == -1)
		buflen = strlen(buff);

	scp = buff;
	while(buflen--)
	{
		c = *scp++;

		if(c == CAN  ||  c == SUB)
		{
			BBS_cancel(con);
			continue;
		}



		if(con->inESC != FALSE   &&  (c >= 32  &&  c < 127))
		{
			UWORD i;

			if(c < '0')
			{
				if(con->icnt < MAXINTER)
				{
					con->inter[con->icnt] = c;		/* something like '#', etc. */
					con->icnt++;
					con->inter[con->icnt] = NUL;	/* clear next intermediate  */
				}
				continue;
			}

			for(i=0; ESC_cmds[i].Routine; i++)
			{
				if(				c == ESC_cmds[i].matchChar		&&
					con->inter[0] == ESC_cmds[i].intermediate)
				{
					if(con->ordc > 0)
						BBS_textout(con);

					con->inESC = ESC_cmds[i].matchChar;
					ESC_cmds[i].Routine(con, con->args, con->argc);
					break;
				}
			}

/*
			if(c == '[')
				con->inCSI = TRUE;
*/

			con->inESC = FALSE;
			continue;
		}



		if(con->inCSI != FALSE   &&  (c >= 32  &&  c < 127))
		{
			UWORD i;

			if(c < '@')
			{
				if(c >= '0'  &&  c <= '9')		/* Parameter.. */
				{
					con->args[con->argc] = con->args[con->argc] * 10;
					con->args[con->argc] += c - '0';
					continue;
				}

				if(c == ';')					/* Parameter-Delimeter */
				{
					con->args[++con->argc] = 0;
					if(con->argc > MAXARGS)
						con->argc = MAXARGS;
					continue;
				}

				if(con->icnt < MAXINTER)
				{
					con->inter[con->icnt] = c;		/* something like '?', etc. */
					con->icnt++;
					con->inter[con->icnt] = NUL;	/* clear next intermediate  */
				}
				continue;
			}

			for(i=0; CSI_cmds[i].Routine != NULL; i++)
			{
				if(				c == CSI_cmds[i].matchChar		&&
					con->inter[0] == CSI_cmds[i].intermediate)
				{
					if(con->ordc > 0)
						BBS_textout(con);

					con->inCSI = c;
					CSI_cmds[i].Routine(con, con->args, con->argc);
					break;
				}
			}
			con->inCSI = FALSE;
			continue;
		}

		if(c < ' '  &&  con->ordc > 0)
			BBS_textout(con);

		switch(c)
		{
			case ESC:
				BBS_invokeESC(con, NULL, NUL);
			break;

			case CSI:
				BBS_invokeCSI(con, NULL, NUL);
			break;

			case BEL:
				con->io->xem_tbeep(1, 0);
			break;

			case BS:
				if(con->col > 1)
					con->col--;
			break;

			case HT:
				while(con->col < con->columns)
				{
					con->col++;
					if(con->tabs[con->col])
						break;
				}
			break;

			case LF:
				if(con->stat & NEWLINE)
					BBS_cursornextline(con, "\001", 0);
				else
					BBS_cursordown(con, "\001", 0);
			break;

			case VT:
				BBS_cursorup(con, "\001", 0);
			break;

			case FF:
				BBS_erasescreen(con);
			break;

			case CR:
				con->col = 1;
			break;

			case SO:
				con->shift = 128;
			break;

			case SI:
				con->shift = 0;
			break;

			default:
				if(con->ordc == 0)
					con->ordcol = con->col;

				con->ordtext[con->ordc] = c | con->shift;
				con->col++;
				con->ordc++;

				if(con->col > con->columns)
				{
					BBS_textout(con);

					if(con->stat & X_SCROLLCURSOR)
						BBS_cursornextline(con, "\001", 0);
					else
						con->col = con->columns;
				}
			break;
		}
	}

	if(con->ordc > 0)
		BBS_textout(con);

	BBS_cursorVisible(con);
}


BOOL __saveds __asm XEmulatorSignal(register __a0 struct XEM_IO *io, register __d0 ULONG signal)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con == NULL)
		return(FALSE);

	if(signal & 1<<(con->timerPort->mp_SigBit))
	{
		ULONG time;

		WaitIO((struct IORequest *)con->doTime);
		con->requestPending = FALSE;

		time = (con->cursorVisible != FALSE) ? CURSOR_OFF : CURSOR_ON;

		BBS_cursorFlip(con);
		ASyncTimer(con->doTime, time, &con->requestPending);
	}

	return(TRUE);
}


STATIC BOOL HandleMacroKeys(struct BBSConsole *con, struct IntuiMessage *imsg, UBYTE chr)
{
	if(con->macrokeys != NULL  &&  con->io->xem_process_macrokeys != NULL)
	{
		struct XEmulatorMacroKey *key;
		BOOL shift, alt, ctrl;
		UWORD qual;

		shift	= imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
		ctrl	= imsg->Qualifier & IEQUALIFIER_CONTROL;
		alt	= imsg->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);

		if(shift != FALSE)
			qual = XMKQ_SHIFT;
		else
		{
			if(alt != FALSE)
				qual = XMKQ_ALTERNATE;
			else
			{
				if(ctrl != FALSE)
					qual = XMKQ_CONTROL;
				else
					qual = XMKQ_NONE;
			}
		}

		if((key = GetHead(con->macrokeys)) != NULL)
		{
			do
			{
				if(key->xmk_Qualifier == qual)
				{
					BOOL match=FALSE;

					if(key->xmk_Type == XMKT_RAWKEY)
					{
						if(key->xmk_Code == imsg->Code)
							match = TRUE;
					}

					if(key->xmk_Type == XMKT_COOKED)
					{
						if(key->xmk_Code == chr)
							match = TRUE;
					}

					if(match != FALSE)
					{
						con->io->xem_process_macrokeys(key);
						return(TRUE);
					}
				}
			}
			while((key = GetSucc(key)) != NULL);
		}
	}
	return(FALSE);
}


ULONG __saveds __asm XEmulatorUserMon(register __a0 struct XEM_IO *io, register __a1 UBYTE *retstr, register __d0 ULONG maxlen, register __a2 struct IntuiMessage *imsg)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;
	struct InputEvent ie;
	ULONG length=0;
	UWORD code;

	code = imsg->Code;
	if(code & IECODE_UP_PREFIX)
		return(0);

	retstr[0] = '\0';

   ie.ie_Class			= IECLASS_RAWKEY;
   ie.ie_SubClass		= 0;
	ie.ie_Code			= code;
	ie.ie_Qualifier	= imsg->Qualifier;
	ie.ie_position.ie_addr = *((APTR *)imsg->IAddress);

	if((length = MapRawKey(&ie, retstr, 20, NULL)) <= 0)
		return(0);

	if(retstr[0] == CR  &&  con->stat & NEWLINE  &&
	  (code == 0x0043  ||  code == 0x0044))
	{
		retstr[1] = LF;
		retstr[2] = '\0';
		length = 2;
	}
	else
	{
		if(retstr[0] == CSI)	/* convert to older 7-bit standard.. */
		{
			auto UBYTE mybuf[40];

			if(length > (40-1))
				return(0);

			strcpy(&mybuf[1], retstr);
			mybuf[0] = ESC;
			mybuf[1] = '[';
			strcpy(retstr, mybuf);
			length++;
		}
	}

	retstr[length] = '\0';

	if(HandleMacroKeys(con, imsg, retstr[0]) != FALSE)
		length = 0;

	return(length);
}


ULONG __saveds __asm XEmulatorHostMon(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorHostData *hd, register __d0 ULONG actual)
{
	if(hd->Destination != NULL)
	{
		ULONG cnt;
		UBYTE *read, *write;
		REGISTER UBYTE c;

		read	= hd->Source;
		write = hd->Destination;
		for(cnt=0; cnt < actual; cnt++)
		{
			c = *read++;

			if(c == '\x18'  ||  c == '\x1A')	/* CAN  ||  SUB */
			{
				hd->InESC = FALSE;
				hd->InCSI = FALSE;
				continue;
			}

			if(hd->InESC != FALSE)		/* Escape - Command ? */
			{
				if(c == '[')	/* perhaps a hidden CSI ? */
				{
					hd->InCSI = TRUE;
					hd->InESC = FALSE;
				}
				else
				{
					if(c >= '0')
						hd->InESC = FALSE;
				}
				continue;
			}

			if(hd->InCSI != FALSE)		/* CSI - Command ? */
			{
				if(c >= '@')
					hd->InCSI = FALSE;
				continue;
			}


			if(c == ESC)
			{
				hd->InESC = TRUE;
				hd->InCSI = FALSE;
				continue;
			}

			if(c == CSI)
			{
				hd->InESC = FALSE;
				hd->InCSI = TRUE;
				continue;
			}

			*write++ = c;
		}

		return((ULONG)(write - hd->Destination));
	}
	else
		return(0);
}


BOOL __saveds __asm XEmulatorClearConsole(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
		BBS_writecon(con, "\014", 1);

	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetConsole(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
	{
		BBS_reset(con);				/* RIS */
		BBS_cursorVisible(con);
	}

	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetTextStyles(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
		BBS_writecon(con, "\033[m", -1);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetCharset(register __a0 struct XEM_IO *io)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
		BBS_writecon(con, "\017", 1);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorOptions(register __a0 struct XEM_IO *io)
{
#define BUFLEN 4

	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;
	UBYTE opt_header=0, opt_crlf, opt_horiz, opt_vert, opt_curb, opt_under;
	ULONG numopts;

	struct xem_option *opti[8];
	UBYTE buff[8][BUFLEN];

	struct xem_option opt0;
	struct xem_option opt1;
	struct xem_option opt2;
	struct xem_option opt3;
	struct xem_option opt4;
	struct xem_option opt5;
	struct xem_option opt6;
	struct xem_option opt7;

	opti[0] = &opt0;
	opti[1] = &opt1;
	opti[2] = &opt2;
	opti[3] = &opt3;
	opti[4] = &opt4;
	opti[5] = &opt5;
	opti[6] = &opt6;
	opti[7] = &opt7;


	BBS_cursorInvisible(con);

	numopts = 0;

	opti[opt_header]->xemo_description = "BBS - ANSI options:";
	opti[opt_header]->xemo_type	= XEMO_HEADER;
	opti[opt_header]->xemo_value	= NULL;
	opti[opt_header]->xemo_length	= NULL;

	opt_crlf = ++numopts;
	opti[opt_crlf]->xemo_value = buff[opt_crlf];
	if(con->stat & NEWLINE)
		strcpy(opti[opt_crlf]->xemo_value, "on");
	else
		strcpy(opti[opt_crlf]->xemo_value, "off");
	opti[opt_crlf]->xemo_description = "EOL-tx and LF-rx => CR+LF";
	opti[opt_crlf]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_crlf]->xemo_length	= BUFLEN;

	opt_horiz = ++numopts;
	opti[opt_horiz]->xemo_value = buff[opt_horiz];
	if(con->stat & X_SCROLLCURSOR)
		strcpy(opti[opt_horiz]->xemo_value, "on");
	else
		strcpy(opti[opt_horiz]->xemo_value, "off");
	opti[opt_horiz]->xemo_description = "Auto horizontal scrolling";
	opti[opt_horiz]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_horiz]->xemo_length	= BUFLEN;

	opt_vert = ++numopts;
	opti[opt_vert]->xemo_value = buff[opt_vert];
	if(con->stat & Y_SCROLLCURSOR)
		strcpy(opti[opt_vert]->xemo_value, "on");
	else
		strcpy(opti[opt_vert]->xemo_value, "off");
	opti[opt_vert]->xemo_description = "Auto vertical scrolling";
	opti[opt_vert]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_vert]->xemo_length	= BUFLEN;

	opt_under = ++numopts;
	opti[opt_under]->xemo_value = buff[opt_under];
	if(con->stat & CURSOR_UNDERSCORE)
		strcpy(opti[opt_under]->xemo_value, "on");
	else
		strcpy(opti[opt_under]->xemo_value, "off");
	opti[opt_under]->xemo_description = "Cursor underscore";
	opti[opt_under]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_under]->xemo_length	= BUFLEN;

	opt_curb = ++numopts;
	opti[opt_curb]->xemo_value = buff[opt_curb];
	if(con->stat & BLINK_CURSOR)
		strcpy(opti[opt_curb]->xemo_value, "on");
	else
		strcpy(opti[opt_curb]->xemo_value, "off");
	opti[opt_curb]->xemo_description = "Cursor blinking";
	opti[opt_curb]->xemo_type		= XEMO_BOOLEAN;
	opti[opt_curb]->xemo_length	= BUFLEN;


	con->io->xem_toptions(++numopts, opti); 


	if(!stricmp(opti[opt_crlf]->xemo_value, "yes") || !stricmp(opti[opt_crlf]->xemo_value, "on"))
		con->stat |= NEWLINE;
	else
		con->stat &= ~NEWLINE;

	if(!stricmp(opti[opt_horiz]->xemo_value, "yes") || !stricmp(opti[opt_horiz]->xemo_value, "on"))
		con->stat |= X_SCROLLCURSOR;
	else
		con->stat &= ~X_SCROLLCURSOR;

	if(!stricmp(opti[opt_vert]->xemo_value, "yes") || !stricmp(opti[opt_vert]->xemo_value, "on"))
		con->stat |= Y_SCROLLCURSOR;
	else
		con->stat &= ~Y_SCROLLCURSOR;

	if(!stricmp(opti[opt_curb]->xemo_value, "yes") || !stricmp(opti[opt_curb]->xemo_value, "on"))
		con->stat |= BLINK_CURSOR;
	else
		con->stat &= ~BLINK_CURSOR;

	if(!stricmp(opti[opt_under]->xemo_value, "yes") || !stricmp(opti[opt_under]->xemo_value, "on"))
		con->stat |= CURSOR_UNDERSCORE;
	else
		con->stat &= ~CURSOR_UNDERSCORE;


	if(!(con->stat & BLINK_CURSOR))
		DeleteCursorServer(con);
	else
		CreateCursorServer(con);

	BBS_cursorVisible(con);

	return(TRUE);
}


ULONG __saveds __asm XEmulatorGetFreeMacroKeys(register __a0 struct XEM_IO *io, register __d0 ULONG qualifier)
{
	return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1);
}


BOOL __saveds __asm XEmulatorMacroKeyFilter(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorMacroKey *macrokeys)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL  &&  con->io->xem_process_macrokeys)
	{
		con->macrokeys = macrokeys;
		return(TRUE);
	}

	return(FALSE);
}


LONG __saveds __asm XEmulatorInfo(register __a0 struct XEM_IO *io, register __d0 ULONG type)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;
	LONG result = -1;

	if(con != NULL)
	{
		switch(type)
		{
			case XEMI_CURSOR_POSITION:
				result = XEMI_CREATE_POSITION(con->row, con->col);
			break;

			case XEMI_CONSOLE_DIMENSIONS:
				result = XEMI_CREATE_DIMENSIONS(con->columns, con->lines);
			break;

		}
	}

	return(result);
}


BOOL __saveds __asm XEmulatorPreferences(register __a0 struct XEM_IO *io, register __a1 STRPTR filename, register __d0 ULONG mode)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;
	BOOL success=FALSE;

	if(con != NULL)
	{
		switch(mode)
		{
			case XEM_PREFS_RESET:
				success = TRUE;
				BBS_internalsettings(con);
			break;

			case XEM_PREFS_LOAD:
			{
				BPTR fh;
				UBYTE buf[80];

				if(fh = Open(filename, MODE_OLDFILE))
				{
					if(Read(fh, buf, 80) > 0)
					{
						if(BBS_parseoptions(con, buf) == FALSE)
							BBS_internalsettings(con);
						else
							success = TRUE;
					}
					Close(fh);
				}
			}
			break;

			case XEM_PREFS_SAVE:
			{
				BPTR fh;

				if(fh = Open(filename, MODE_NEWFILE))
				{
					UBYTE *mode;

					success = TRUE;

					mode = (con->stat & X_SCROLLCURSOR) ? "YES" : "NO";
					FPrintf(fh, "HSCROLL=%s", mode);

					mode = (con->stat & Y_SCROLLCURSOR) ? "YES" : "NO";
					FPrintf(fh, " VSCROLL=%s", mode);

					mode = (con->stat & NEWLINE) ? "YES" : "NO";
					FPrintf(fh, " NEWLINE=%s", mode);

					mode = (con->stat & BLINK_CURSOR) ? "YES" : "NO";
					FPrintf(fh, " BLINK=%s", mode);

					mode = (con->stat & CURSOR_UNDERSCORE) ? "YES" : "NO";
					FPrintf(fh, " UNDERSCORE=%s", mode);

					con->r_stat = con->stat;

					Close(fh);
				}
				else
					success = FALSE;
			}
			break;

		}
	}
	return(success);
}


VOID __saveds __asm XEmulatorNewSize(register __a0 struct XEM_IO *io, register __a1 struct IBox *frame)
{
	struct BBSConsole *con = (struct BBSConsole *)io->xem_console;

	if(con != NULL)
	{
		BBS_cursorInvisible(con);
		BBS_newframe(con, frame);
		BBS_checkframe(con);
		BBS_cursorVisible(con);
	}
}


/* end of source-code */
