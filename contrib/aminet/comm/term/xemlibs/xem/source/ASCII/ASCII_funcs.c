/* $Revision Header built by KCommodity by Kai Iske *** (do not edit) ************
**
** © Copyright by Ueli `Lonely Rider' Kaufmann
**
** File             : ASCII_funcs.c
** Created on       : Saturday 14-Nov-92 12:58:06
** Created by       : U. Kaufmann
** Current revision : V4.30
**
**
** Purpose
** -------
**     ASCII Emulator ala XEm
**
**
** Revision V4.10
** --------------
**   - optimized scrolling (scrolls only used bitplanes)
**
** Revision V4.20
** --------------
**   - custom-font support added
**
** Revision V4.30
** --------------
**   - workbench support added
**
*********************************************************************************/

#include "ownincs/ASCIIconsole.h"

STATIC VOID	ASCII_cursorFlip(struct ASCIIConsole *);
STATIC VOID	ASCII_textout(struct ASCIIConsole *);
STATIC VOID	ASCII_cursorup(struct ASCIIConsole *, UWORD);
STATIC VOID	ASCII_cursordown(struct ASCIIConsole *, UWORD);
STATIC VOID	ASCII_reset(struct ASCIIConsole *);
STATIC VOID	ASCII_internalsettings(struct ASCIIConsole *);
STATIC VOID	ASCII_newframe(struct ASCIIConsole *con, struct IBox *frame);
STATIC VOID ASCII_checkframe(struct ASCIIConsole *con);


/* utils.a */
VOID *GetSucc(VOID *);
VOID *GetHead(VOID *);
VOID *GetTail(VOID *);
VOID *GetPred(VOID *);
VOID BZero(VOID *, LONG);
VOID BSet(VOID *, LONG, LONG);



                           /* ASCII Zerhacker */


struct	ExecBase			*SysBase;
struct	DosLibrary		*DOSBase;
struct	GfxBase			*GfxBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*KeymapBase;
struct	Library			*UtilityBase;


VOID __saveds __asm __UserLibCleanup(register __a6 struct Library *libBase)
{
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



	return(0);
}


BOOL __saveds __asm XEmulatorSetup(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con;
	
	io->xem_console = NULL;

	if(con = AllocMem(sizeof(struct ASCIIConsole), MEMF_PUBLIC|MEMF_CLEAR))
	{
		con->io	= io;			/* for easier passing..(-; */
		con->rp	= io->xem_window->RPort;
		con->font = io->xem_font;
		io->xem_console = con;

		ASCII_internalsettings(con);		/* just a preset */

		*con->io->xem_signal = 0;

		return((BOOL)(con->font != NULL));
	}

	return(FALSE);
}


VOID __saveds __asm XEmulatorCleanup(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	if(con)
	{
		*con->io->xem_signal = 0;
		FreeMem(con, sizeof(struct ASCIIConsole));
		io->xem_console = NULL;
	}
}



LONG AtoL(UBYTE *str)
{
	LONG cnt=0, help;

	while(*str <= ' ')		/* skip white spaces.. */
	{
		if(*str == '\0')
			return(0);
		str++;
	}

	if(*str == '-')
	{
		help = -1;
		str++;
	}
	else
	{
		if(*str == '+')
			str++;
		help = 1;
	}
	
	for(;;)
	{
		if(*str >= '0' && *str <= '9')
		{
			cnt *= 10;
			cnt += *str - '0';
		}
		else
			break;

		if(!(*str))
			break;

		str++;
	}

	return(cnt * help);
}


BOOL __saveds __asm XEmulatorOpenConsole(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	if(con == NULL)
		return(FALSE);

	con->rp = io->xem_window->RPort;		/* DO NOT forget this..!! */
	con->font = io->xem_font;

	ASCII_newframe(con, NULL);

	ASCII_checkframe(con);

	con->isReady = TRUE;

	ASCII_reset(con);

	return((BOOL)(con->font != NULL));
}


VOID __saveds __asm XEmulatorCloseConsole(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	con->isReady = FALSE;
	return;
}


STATIC VOID ASCII_clearScreen(struct ASCIIConsole *con)
{
	con->row = 1;
	con->col = 1;
	con->ordc= 0;

	mySetRast(con);
	memset(&con->linAttr[1], con->background_pen, con->lines);
}



STATIC WORD ASCII_getmaxframeleft(struct ASCIIConsole *con)
{
	return(con->io->xem_window->BorderLeft);
}


STATIC WORD ASCII_getmaxframetop(struct ASCIIConsole *con)
{
	return(con->io->xem_window->BorderTop);
}


STATIC WORD ASCII_getmaxframewidth(struct ASCIIConsole *con)
{
	return((WORD)(con->io->xem_window->Width - con->io->xem_window->BorderLeft - con->io->xem_window->BorderRight));
}


STATIC WORD ASCII_getmaxframeheight(struct ASCIIConsole *con)
{
	return((WORD)(con->io->xem_window->Height - con->io->xem_window->BorderTop - con->io->xem_window->BorderBottom));
}


STATIC VOID ASCII_resetscreen(struct ASCIIConsole *con)
{
	con->lines	= con->border.Height	/ YSIZE;
	con->columns= con->border.Width	/ XSIZE;

	if(con->lines == 0  ||  con->columns == 0)	/* help prevent gurus..(-: */
	{
		con->displayInactive = TRUE;
		con->lines	 = 1;
		con->columns = 1;
	}
	else
		con->displayInactive = FALSE;

	con->row		= 1;
	con->col		= 1;
	con->ordc	= 0;
	con->ordcol = 0;
}


STATIC VOID ASCII_reset(struct ASCIIConsole *con)
{
	ASCII_resetscreen(con);

	/* don`t touch con->mode & con->foreground_pen */

	ASCII_clearScreen(con);

	SetDrMd(con->rp, JAM2);
	SetFont(con->rp, con->io->xem_font);
	SetAPen(con->rp, con->foreground_pen);
	SetBPen(con->rp, con->background_pen);
	SetSoftStyle(con->rp, 0, 0);

	ASCII_cursorFlip(con);
}


VOID __saveds __asm XEmulatorWrite(register __a0 struct XEM_IO *io, register __a1 UBYTE *buff, register __d0 ULONG buflen)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	ULONG len;
	BOOL ttyFlag=FALSE;
	UBYTE c, *scp;

	ASCII_cursorFlip(con);

	len = (((LONG)buflen) == -1) ? strlen(buff) : buflen;
	scp = buff;
	while(len--)
	{
		if(ttyFlag)
		{
			ttyFlag = FALSE;
			c = '[';
			goto PUTIT;
		}

		c = *scp++;

		if(con->ASCII != FALSE)	/* else TTY */
		{
			if(c == CAN || c == SUB)
			{
				con->inESC = FALSE;
				con->inCSI = FALSE;

				continue;
			}


			if(con->inESC != FALSE)
			{
				if(c == '[')	/* ist's ein verkappter CSI ? */
				{
					con->inESC = FALSE;
					con->inCSI = TRUE;
				}
				else
				{
					if(c >= '0')
						con->inESC = FALSE;
				}

				continue;
			}


			if(con->inCSI != FALSE)		/* CSI - Befehl ? */
			{
				if(c >= '@')
					con->inCSI = FALSE;

				continue;
			}
		}


		switch(c)
		{
			case BS:
				if(con->ordc)
					ASCII_textout(con);
				if(con->col > 1)
					con->col--;
			break;

			case LF:
				if(con->ordc)
					ASCII_textout(con);
				ASCII_cursordown(con, 1);
			break;

			case VT:
				if(con->ordc)
					ASCII_textout(con);
				ASCII_cursorup(con, 1);
			break;

			case FF:
				ASCII_clearScreen(con);
			break;

			case CR:
				if(con->ordc)
					ASCII_textout(con);
				con->col = 1;
			break;

			case ESC:
				if(con->ASCII != FALSE)
					con->inESC = TRUE;
				else
				{
					c = '^';
					goto PUTIT;
				}
			break;

			case CSI:
				if(con->ASCII != FALSE)
					con->inCSI = TRUE;
				else
				{
					ttyFlag = TRUE;
					len++;
					c = '^';
					goto PUTIT;
				}
			break;

			default:
			{
PUTIT:
				if((c >= 32 && c < 127)  ||  (c >= 160 && c < 255))
				{
					if(con->ordc == 0)
						con->ordcol = con->col;
					con->ordText[con->ordc++] = c;
					con->col++;

					if(con->col == con->columns)
					{
						ASCII_textout(con);
						ASCII_cursordown(con, 1);
						con->col = 1;
					}
				}
			}
		}
	}

	if(con->ordc)
		ASCII_textout(con);

	ASCII_cursorFlip(con);
}


BOOL __saveds __asm XEmulatorSignal(register __a0 struct XEM_IO *io, register __d0 ULONG sig)
{
	return(TRUE);
}


STATIC BOOL HandleMacroKeys(struct ASCIIConsole *con, struct IntuiMessage *imsg, UBYTE chr)
{
	if(con->macrokeys  &&  con->io->xem_process_macrokeys)
	{
		struct XEmulatorMacroKey *key;
		BOOL shift, alt, ctrl;
		UWORD qual;

		shift	= imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
		ctrl	= imsg->Qualifier &  IEQUALIFIER_CONTROL;
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

		if(key = GetHead(con->macrokeys))
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
			while(key = GetSucc(key));
		}
	}
	return(FALSE);
}


ULONG __saveds __asm XEmulatorUserMon(register __a0 struct XEM_IO *io, register __a1 UBYTE *retstr, register __d0 ULONG maxlen, register __a2 struct IntuiMessage *imsg)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;
	struct InputEvent ie;
	ULONG length=0;
	UWORD code, qual;

	if(imsg->Code & IECODE_UP_PREFIX)
		return(0);

	*retstr = '\0';

   ie.ie_Class			= IECLASS_RAWKEY;
   ie.ie_SubClass		= 0;
	code = ie.ie_Code			= imsg->Code;
	qual = ie.ie_Qualifier	= imsg->Qualifier;
	ie.ie_position.ie_addr	= *((APTR *)imsg->IAddress);

	if((length = MapRawKey(&ie, retstr, maxlen, NULL)) <= 0)
		return(0);

	retstr[length] = '\0';			/* Null terminate the value */

	if(HandleMacroKeys(con, imsg, retstr[0]))
		length = 0;

	return(length);
}


ULONG __saveds __asm XEmulatorHostMon(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorHostData *hostData, register __d0 ULONG actual)
{
	if(hostData->Destination != NULL  &&  actual != 0)
	{
		ULONG cnt;
		UBYTE *read, *write;
		REGISTER UBYTE c;

		read	= hostData->Source;
		write = hostData->Destination;
		for(cnt=0; cnt < actual; cnt++)
		{
			c = *read++;

			if(c == '\x18'  ||  c == '\x1A')	/* CAN  ||  SUB */
			{
				hostData->InESC = FALSE;
				hostData->InCSI = FALSE;
				continue;
			}

			if(hostData->InESC)		/* Escape - Befehl ? */
			{
				if(c == '[')	/* ist's ein verkappter CSI ? */
				{
					hostData->InCSI = TRUE;
					hostData->InESC = FALSE;
				}
				else
				{
					if(c >= '0')
						hostData->InESC = FALSE;
				}
				continue;
			}

			if(hostData->InCSI)		/* CSI - Command? */
			{
				if(c >= '@')
					hostData->InCSI = FALSE;
				continue;
			}


			if(c == ESC)
			{
				hostData->InESC = TRUE;
				hostData->InCSI = FALSE;
				continue;
			}

			if(c == CSI)
			{
				hostData->InESC = FALSE;
				hostData->InCSI = TRUE;
				continue;
			}

			*write++ = c;
		}

		actual = (ULONG)(write - hostData->Destination);
	}

	return(actual);
}


STATIC VOID ASCII_scrollLinAttrsDown(struct ASCIIConsole *con, UWORD lines)
{
	UBYTE *src, *dst, cnt;

	cnt = con->lines;
	if(lines > cnt)
		lines = cnt;

	cnt -= lines;
	dst = &con->linAttr[1];
	src = &con->linAttr[1] + lines;

	while(cnt--)
		*dst++ = *src++;
	while(lines--)
	/* ScrollRaster() uses BPen to fill empty lines.. */
		*dst++ = con->background_pen;
}


STATIC VOID ASCII_scrollLinAttrsUp(struct ASCIIConsole *con, UWORD lines)
{
	UBYTE *src, *dst, cnt;

	cnt = con->lines + 1;
	if(lines > cnt)
		lines = cnt;

	cnt -= lines;
	dst = &con->linAttr[1] + con->lines;
	src = &con->linAttr[1] + con->lines - lines;

	while(cnt--)
		*dst-- = *src--;
	while(lines--)
	/* ScrollRaster() uses BPen to fill empty lines.. */
		*dst-- = con->background_pen;
}


STATIC VOID ASCII_setTurboMask(struct ASCIIConsole *con)
{
	UBYTE *ptr, mask, i;

	ptr = con->linAttr + 1;
	for(i=0, mask=0; i<con->lines; i++)
		mask |= *(ptr + i);

	SetWrMsk(con->rp, mask);
}


STATIC VOID ASCII_setDefaultMask(struct ASCIIConsole *con)
{
	UBYTE mask;

	switch(con->io->xem_screendepth)
	{
		case 1:
			mask = 1;
		break;

		case 2:
			mask = 2+1;
		break;

		case 3:
			mask = 4+2+1;
		break;

		default:
		case 4:
			mask = 8+4+2+1;
		break;

	}

	SetWrMsk(con->rp, mask);
}


STATIC VOID ASCII_cursordown(struct ASCIIConsole *con, UWORD i)
{
	while(i--)
	{
		if(con->row == con->lines)
		{
			ASCII_setTurboMask(con);
			myScrollRaster(con, 0, +YSIZE, 0, 0,
								con->border.Width - 1, (con->lines * YSIZE) - 1);
			ASCII_setDefaultMask(con);
			ASCII_scrollLinAttrsDown(con, 1);
		}
		else
		{
			if(con->row < con->lines)
				con->row++;
		}
	}
}


STATIC VOID ASCII_cursorup(struct ASCIIConsole *con, UWORD i)
{
	while(i--)
	{
		if(con->row == 1)
		{
			ASCII_setTurboMask(con);
			myScrollRaster(con, 0, -YSIZE, 0, 0,
								con->border.Width - 1,(con->lines * YSIZE) - 1);
			ASCII_setDefaultMask(con);
			ASCII_scrollLinAttrsUp(con, 1);
		}
		else
		{
			if(con->row > 1)
				con->row--;
		}
	}
}


STATIC VOID ASCII_cursorFlip(struct ASCIIConsole *con)
{
	if(con->col > con->columns)
		con->col = con->columns;

	SetDrMd(con->rp, COMPLEMENT);
	myRectFill(con, (con->col - 1) * XSIZE, (con->row - 1) * YSIZE,
						 (con->col * XSIZE) - 1, (con->row * YSIZE) - 1, -1);
	SetDrMd(con->rp, JAM2);
}


STATIC VOID ASCII_textout(struct ASCIIConsole *con)
{
	con->linAttr[con->row] |= (con->foreground_pen | con->background_pen);

	myMove_Text(con, con->ordcol, con->row, con->ordText, con->ordc);
	con->ordc = 0;
}


BOOL __saveds __asm XEmulatorClearConsole(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	ASCII_clearScreen(con);
	ASCII_cursorFlip(con);

	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetConsole(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	ASCII_reset(con);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetTextStyles(register __a0 struct XEM_IO *io)
{
	return(FALSE);
}


BOOL __saveds __asm XEmulatorResetCharset(register __a0 struct XEM_IO *io)
{
	return(FALSE);
}


VOID ASCII_configopt(struct xem_option *opt, UBYTE *title, UBYTE *buff, UWORD length, BOOL type, BOOL value)
{
	opt->xemo_description= title;
	opt->xemo_value		= buff;
	opt->xemo_type			= type;
	opt->xemo_length		= length;

	if(type == XEMO_BOOLEAN)
	{
		if(value != FALSE)
			strcpy(opt->xemo_value, "on");
		else
			strcpy(opt->xemo_value, "off");

		opt->xemo_length = 4;
	}
}

STATIC BOOL ASCII_options(struct ASCIIConsole *con);

BOOL __saveds __asm XEmulatorOptions(register __a0 struct XEM_IO *io)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	return(ASCII_options(con));
}
STATIC BOOL ASCII_options(struct ASCIIConsole *con)
{
	enum	{ OPT_HEADER,OPT_ASCII,OPT_FRONTPEN,OPT_BACKPEN,
			  NUMOPTS };

	struct xem_option *opti[NUMOPTS];
	ULONG changed;
	UBYTE aPenbuff[5], bPenbuff[5], asciibuff[5];

	struct xem_option opt0;
	struct xem_option opt1;
	struct xem_option opt2;
	struct xem_option opt3;

	opti[OPT_HEADER]	= &opt0;
	opti[OPT_ASCII]	= &opt1;
	opti[OPT_FRONTPEN]= &opt2;
	opti[OPT_BACKPEN]	= &opt3;

	aPenbuff[0] = (con->foreground_pen / 10) + '0';
	aPenbuff[1] = (con->foreground_pen % 10) + '0';
	aPenbuff[2] = 0;

	bPenbuff[0] = (con->background_pen / 10) + '0';
	bPenbuff[1] = (con->background_pen % 10) + '0';
	bPenbuff[2] = 0;

	ASCII_configopt(opti[OPT_HEADER],	"General options:", NULL,	0,		XEMO_HEADER,	FALSE);
	ASCII_configopt(opti[OPT_ASCII],		"ASCII Mode.:", asciibuff,	4, XEMO_BOOLEAN, con->ASCII);
	ASCII_configopt(opti[OPT_FRONTPEN],	"Front Pen..:", aPenbuff,	4, XEMO_LONG, FALSE);
	ASCII_configopt(opti[OPT_BACKPEN],	"Back Pen...:", bPenbuff,	4, XEMO_LONG, FALSE);

	changed = con->io->xem_toptions(NUMOPTS, opti);

	if(changed & (1 << OPT_ASCII))
	{
		con->ASCII = (!stricmp(opti[OPT_ASCII]->xemo_value, "yes") || !stricmp(opti[OPT_ASCII]->xemo_value, "on"));
	}

	if(changed & (1 << OPT_FRONTPEN))
	{
		con->foreground_pen = AtoL(aPenbuff);
		SetAPen(con->rp, con->foreground_pen);
	}

	if(changed & (1 << OPT_BACKPEN))
	{
		con->background_pen = AtoL(bPenbuff);
		SetBPen(con->rp, con->background_pen);
	}

	return(TRUE);
}


ULONG __saveds __asm XEmulatorGetFreeMacroKeys(register __a0 struct XEM_IO *io, register __d0 ULONG qualifier)
{
	return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1);
}


BOOL __saveds __asm XEmulatorMacroKeyFilter(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorMacroKey *macrokeys)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	if(con != NULL  &&  con->io->xem_process_macrokeys)
	{
		con->macrokeys = macrokeys;
		return(TRUE);
	}

	return(FALSE);
}


LONG __saveds __asm XEmulatorInfo(register __a0 struct XEM_IO *io, register __d0 ULONG type)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;
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


STATIC VOID ASCII_internalsettings(struct ASCIIConsole *con)
{
	/* some good(?) settings */

	con->ASCII = TRUE;
	con->foreground_pen = 1;
	con->background_pen = 0;
}


UBYTE *PreferencesTemplate = "FGP=FOREGROUND_PEN/N,BGP=BACKGROUND_PEN/N,TTY=TTY_MODE";
enum { ARG_APEN=0,ARG_BPEN,ARG_TTY,ARGS };

STATIC BOOL ASCII_parseoptions(struct ASCIIConsole *con, STRPTR optionsBuffer)
{
	UBYTE **ArgArray;
	UBYTE *ArgBuffer;
	BOOL success=FALSE;

	if(SysBase->LibNode.lib_Version < 37)	/* sorry, wrong number..<-:< */
		return(FALSE);

	if(ArgBuffer = (UBYTE *)AllocVec(1024, MEMF_ANY | MEMF_CLEAR))
	{
		strcpy(ArgBuffer, optionsBuffer);
		strcat(ArgBuffer, "\n");

		if(ArgArray = (UBYTE **)AllocVec(sizeof(UBYTE *) * ARGS, MEMF_ANY | MEMF_CLEAR))
		{
			struct RDArgs *ArgsPtr;

			if(ArgsPtr = (struct RDArgs *)AllocDosObject(DOS_RDARGS, TAG_DONE))
			{
				ArgsPtr->RDA_Source.CS_Buffer = ArgBuffer;
				ArgsPtr->RDA_Source.CS_Length = strlen(ArgBuffer);
				ArgsPtr->RDA_Source.CS_CurChr = 0;
				ArgsPtr->RDA_DAList	= NULL;
				ArgsPtr->RDA_Buffer	= NULL;
				ArgsPtr->RDA_BufSiz	= 0;
				ArgsPtr->RDA_ExtHelp	= NULL;
				ArgsPtr->RDA_Flags	= RDAF_NOPROMPT;

				if(ReadArgs(PreferencesTemplate, (LONG *)ArgArray, ArgsPtr))
				{
/*					VOID KPrintF(UBYTE *, ...); */
					success = TRUE;

					if(ArgArray[ARG_APEN])
						con->foreground_pen = *((LONG *)ArgArray[ARG_APEN]);

					if(ArgArray[ARG_BPEN])
						con->background_pen = *((LONG *)ArgArray[ARG_BPEN]);

					if(ArgArray[ARG_TTY])
						con->ASCII = (!strnicmp(ArgArray[ARG_TTY], "NO", 2));

					FreeArgs(ArgsPtr);
				}
				FreeDosObject(DOS_RDARGS, ArgsPtr);
			}
			FreeVec(ArgArray);
		}
		FreeVec(ArgBuffer);
	}
	return(success);
}


BOOL __saveds __asm XEmulatorPreferences(register __a0 struct XEM_IO *io, register __a1 STRPTR filename, register __d0 ULONG mode)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;
	BOOL success=FALSE;

	if(con != NULL)
	{
		switch(mode)
		{
			case XEM_PREFS_RESET:
				success = TRUE;
				ASCII_internalsettings(con);
			break;

			case XEM_PREFS_LOAD:
			{
				BPTR fh;
				UBYTE buf[80];

				if(fh = Open(filename, MODE_OLDFILE))
				{
					if(Read(fh, buf, 80) > 0)
					{
						if(ASCII_parseoptions(con, buf) == FALSE)
							ASCII_internalsettings(con);
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
					UBYTE buf[80], *mode;

					success = TRUE;

					mode = (con->ASCII != FALSE) ? "NO" : "YES";

					sprintf(buf, "FOREGROUND_PEN=%ld BACKGROUND_PEN=%ld TTY_MODE=%s\n", con->foreground_pen, con->background_pen, mode);

					FPrintf(fh, buf);

					Close(fh);
				}
			}
			break;

		}
	}
	return(success);
}


STATIC VOID ASCII_newframe(struct ASCIIConsole *con, struct IBox *frame)
{
	if(frame != NULL)
	{
		con->border.Left	= frame->Left;
		con->border.Top	= frame->Top;
		con->border.Width	= frame->Width;
		con->border.Height= frame->Height;
	}
	else
	{
		con->border.Left	= ASCII_getmaxframeleft(con);		/* Pixels.. */
		con->border.Top	= ASCII_getmaxframetop(con);		/* Pixels.. */
		con->border.Width	= ASCII_getmaxframewidth(con);	/* Pixels.. */
		con->border.Height= ASCII_getmaxframeheight(con);	/* Pixels.. */
	}
}


STATIC VOID ASCII_checkframe(struct ASCIIConsole *con)
{
	UWORD lines, columns;

	lines	 = con->border.Height / YSIZE;
	columns= con->border.Width	 / XSIZE;

	if(con->columns != columns  ||  con->lines != lines)
		ASCII_resetscreen(con);
}


VOID __saveds __asm XEmulatorNewSize(register __a0 struct XEM_IO *io, register __a1 struct IBox *frame)
{
	struct ASCIIConsole *con = (struct ASCIIConsole *)io->xem_console;

	if(con != NULL)
	{
		ASCII_cursorFlip(con);

		ASCII_newframe(con, frame);
		ASCII_checkframe(con);

		ASCII_cursorFlip(con);
	}
}


/* end of source-code */
