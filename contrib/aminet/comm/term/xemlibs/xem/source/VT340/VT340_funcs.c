/* $Revision Header built by Ueli Kaufmann ********** (please edit) **********
*
*  Copyright by Ueli Kaufmann
*
* File             : xemvt340.library
* Created on       : Montag 12-Apr-93  15:03:44
* Created by       : Ueli Kaufmann
* Created with     ; SAS/C 6.0, Cygnus-Ed V2.12, Shell OS2.04
* Current revision : V4.4
*
*
* Purpose
* -------
*   100% compatibilty with DEC VT300 terminals including page and tektronix.
*     
* Known Bugs
* ----------
*  - page commands not yet implemented.
*  - tektronix 4010/14 not yet implemented  (means, it works. But I've
*    no access for testing/debugging..) 
*  - 132 chars per line are not supported.
*  - downline-loadable-fonts are not supported.
*
*
* Revision V4.4
* -------------
* - made the beast wbench compatible
* - added a blinking/underscore cursor (:
* - removed some incompatibilities and added some new bugs (:
* - " <CSI>24H<CSI>J" caused xemvt340 to crash ):
*****************************************************************************/

#include "ownincs/VT340console.h"

                        /* DEC VT-340 Zerhacker */



/* allowed globals.. */

struct	ExecBase			*SysBase;
struct	DosLibrary		*DOSBase;
struct	GfxBase			*GfxBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*UtilityBase;
struct	Library			*DiskfontBase;
struct	Library			*KeymapBase;


IMPORT struct SpecialKey SpecialKeys[34];


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
	struct VT340Console *con;
	UWORD i;
	
	io->xem_console = NULL;

	if(con = AllocVec(sizeof(struct VT340Console), MEMF_PUBLIC|MEMF_CLEAR))
	{
		con->io = io;			/* for easier passing..(-; */
		con->rp = io->xem_window->RPort;
		io->xem_console = con;

		con->isReady = FALSE;

		*con->io->xem_signal = 0;

		strcpy(con->screenName, DEFAULT_SCREENNAME);
		strcpy(con->printerName, DEFAULT_PRINTERNAME);

		BSet(con->specialMap, 256, -1);

		for(i=0; i < sizeof(SpecialKeys) / sizeof(struct SpecialKey); i++)
			con->specialMap[SpecialKeys[i].Key] = i;

		if(!VT340_openfonts(con))
			return(FALSE);

		if(!(con->DCS_buffer = AllocVec(DCS_BUFLEN, MEMF_ANY | MEMF_CLEAR)))
			return(FALSE);

		VT340_ParseInit(con, NULL, XEM_PREFS_RESET);	/* to be on the save side..)-: */

		return(TRUE);
	}

	return(FALSE);
}


VOID __saveds __asm XEmulatorCleanup(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		con->isReady = FALSE;

		TEK_close(con);

		VT340_closefonts(con);
		SetFont(con->rp, con->io->xem_font);

		if(con->DCS_buffer != NULL)
		{
			FreeVec(con->DCS_buffer);
			con->DCS_buffer = NULL;
		}

		VT340_closescreenFH(con);

		VT340_closeprinterFH(con);

		*con->io->xem_signal = 0;

		FreeVec(con);
		io->xem_console = NULL;
	}
}


BOOL __saveds __asm XEmulatorOpenConsole(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con == NULL)
		return(FALSE);

	con->isReady = FALSE;

	con->rp = io->xem_window->RPort;
	con->writeMask = con->rp->Mask;


/*	VT340_colorModes(con);	in resetcon() */

	if(CreatePage(con) == FALSE)
		return(FALSE);

	if(CreateScale(con) == FALSE)
		return(FALSE);
	con->ScaleCache = -1;

	VT340_newframe(con, NULL);
/*	VT340_checkframe(con);	/* not needed in this case..!! */ */

	con->isReady = TRUE;		/* Ev`rything`s at its place now, I hope..(-: */

	VT340_resetcon(con, TRUE);

	CreateCursorServer(con);
/*	*con->io->xem_signal is already updated.. */
	PageCursorVisible(con);

	return(TRUE);
}


VOID __saveds __asm XEmulatorCloseConsole(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

/*	FPrintf(Output(), "XEmulatorCloseConsole Start\n");*/

	if(con == NULL)
		return;

	DeleteCursorServer(con);
	*con->io->xem_signal = 0;

	SetFont(con->rp, con->io->xem_font);

	DeleteLockedWindow(con);

	DeletePage(con);

	DeleteScale(con);

/*	FPrintf(Output(), "XEmulatorCloseConsole Ende\n");*/

	con->isReady = FALSE;
}


VOID __saveds __asm XEmulatorWrite(register __a0 struct XEM_IO *io, register __a1 UBYTE *buff, register __d0 LONG buflen)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
		VT340_writecon(con, buff, buflen);
}


BOOL __saveds __asm XEmulatorSignal(register __a0 struct XEM_IO *io, register __d0 ULONG signal)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con == NULL)
		return(FALSE);

	if(signal & 1<<(con->timerPort->mp_SigBit))
	{
		ULONG time;

		WaitIO((struct IORequest *)con->doTime);
		con->requestPending = FALSE;

		time = (con->cursorVisible != FALSE) ? CURSOR_OFF : CURSOR_ON;

		PageCursorFlip(con);
		ASyncTimer(con->doTime, time, &con->requestPending);
	}

	return(TRUE);
}


ULONG __saveds __asm XEmulatorHostMon(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorHostData *hostData, register __d0 ULONG actual)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con == NULL)
		return(0);

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

			if(hostData->InESC != FALSE)		/* Escape - Befehl ? */
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

			if(hostData->InCSI != FALSE)		/* CSI - Befehl ? */
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


STATIC BOOL HandleMacroKeys(struct VT340Console *con, struct IntuiMessage *imsg, UBYTE chr)
{
	if(con->macrokeys  &&  con->io->xem_process_macrokeys)
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
	struct VT340Console *con = (struct VT340Console *)io->xem_console;
	struct InputEvent ie;
	ULONG length = 0;
	BOOL none, shift, npad, alt, ctrl;
	UBYTE *p;

/* VT-340 function key defaults */
	STATIC UBYTE *fkeys[] = {
	/* F1 - F5 are local functions and don't send codes.. */
		"17~",	/* F6 */
		"18~",	/* F7 */
		"19~",	/* F8 */
		"20~",	/* F9 */
		"21~",	/* F10 */
		"23~",	/* F11 */
		"24~",	/* F12 */
		"25~",	/* F13 */
		"26~",	/* F14 */
		"28~",	/* F15 */
		"29~",	/* F16 */
		"31~",	/* F17 */
		"32~",	/* F18 */
		"33~",	/* F19 */
		"34~" };	/* F20 */


	STATIC UBYTE nkeys[] = {
		'4',		/* SELECT      */
		'5',		/* NEXT SCREEN */
		'6',		/* PREV SCREEN */
		'1',		/* FIND        */
		'2',		/* INSERT HERE */
		'3',		/* REMOVE      */
		'\0' };


/* Numeric keypad return values in application mode */
	STATIC UBYTE akeys[] = {'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', '\0'};


/* Numeric keypad for A2000 and A500 have keys that the A1000 doesn't have.
** Use them for the VT340 PF-Keys. */
	STATIC UBYTE pfkeys[] = { 'P', 'Q', 'R', 'S' };


/* end of data, uff..(-: */


	if(con == NULL  ||  imsg->Code & IECODE_UP_PREFIX)
		return(0);

	p	= retstr;
	*p	= '\0';

	npad	= imsg->Qualifier & IEQUALIFIER_NUMERICPAD;
	shift	= imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
	ctrl	= imsg->Qualifier & IEQUALIFIER_CONTROL;
	alt	= imsg->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);
	none	= (shift == FALSE  &&  ctrl == FALSE  &&  alt == FALSE);

	ie.ie_Class				= IECLASS_RAWKEY;
	ie.ie_SubClass			= 0;
	ie.ie_Code				= imsg->Code;
	ie.ie_Qualifier		= imsg->Qualifier;
	ie.ie_position.ie_addr = *((APTR *)imsg->IAddress);

	if((length = MapRawKey(&ie, retstr, maxlen, NULL)) <= 0)
		return(0);

	*(p+length) = '\0';			/* Null terminate the value */


	if(alt  &&  length == 1  &&  p[0] >= IND  &&  p[0] <= APC  &&  con->pstat & P_CONVERT)
	{
		switch(p[0])
		{
			case IND:
				p[1] = 'D';
			break;

			case NEL:
				p[1] = 'E';
			break;

			case HTS:
				p[1] = 'H';
			break;

			case RI:
				p[1] = 'M';
			break;

			case SS2:
				p[1] = 'N';
			break;

			case SS3:
				p[1] = 'O';
			break;

			case DCS:
				p[1] = 'P';
			break;

			case CSI:
				p[1] = '[';
			break;

			case ST:
				p[1] = '/';
			break;

			case OSC:
				p[1] = ']';
			break;

			case PM:
				p[1] = '^';
			break;

			case APC:
				p[1] = '_';
			break;
		}

		if(p[1])
		{
			p[0] = ESC;
			p[2] = '\0';

			return(2);
		}
	}

	if(p[0] == CSI)	/* 8-bit Control Sequence Introducer.. */
	{

	/* First, we try to match the cursor keys */

		if(p[1] >= 'A'  &&  p[1] <= 'D')
		{
			if(con->pstat & P_ANSI_MODE)	/* VT-102 or VT-340 */
			{
				if(con->pstat & P_CONVERT)
				{
					p[0] = ESC;
					p[2] = p[1];
					p[1] = (con->ustat & U_CURAPP) ? 'O' : '[';
					p[3] = '\0';
					return(3);
				}

				if(con->ustat & U_CURAPP)
					p[0] = SS3;
			}
			else
				p[0] = ESC;		/* VT-52 */

			p[2] = '\0';
			return(2);
		}

	/* Second, I remember, there were some funny keys on my hack-board..(-: */
		
		if(p[1] >= '0'  &&  p[1] <= '9')		/* fun keys */
		{
			UBYTE cnt = AtoL(&p[1]);

			if(cnt <= 5)	/* local functions */
			{
				switch(cnt)
				{
					case 0:	/* HOLD SCREEN */
						p[0] = (con->holdScreen == FALSE) ? XOF : XON;
						con->holdScreen = (p[0] == XOF);
						p[1] = '\0';
					return(1);

					case 1:	/* PRINT SCREEN */
					{
						if(alt != FALSE)	/* PRINT CONTROLLED */
						{
							if(con->gstat & G_PRINT_CONTROL)
							{
								VT340_printterminator(con, con->printerHandle);
								con->gstat &= ~G_PRINT_CONTROL;

								if(con->gstat & G_AUTO_PRINT_INACTIVE)
								{
									con->gstat &= ~G_AUTO_PRINT_INACTIVE;
									con->gstat |=  G_AUTO_PRINT;
								}
							}
							else
							{
								if(VT340_openprinterFH(con) != FALSE)
								{
									con->gstat |= G_PRINT_CONTROL;

									if(con->gstat & G_AUTO_PRINT)
									{
										con->gstat |=  G_AUTO_PRINT_INACTIVE;
										con->gstat &= ~G_AUTO_PRINT;
									}
								}
							}

							CreateOwnStatusLine(con);
							return(0);
						}


						if(ctrl != FALSE)		/* AUTO PRINT */
						{
							if(con->gstat & G_AUTO_PRINT  ||
								con->gstat & G_AUTO_PRINT_INACTIVE)
							{
								if(con->gstat & G_AUTO_PRINT)
									VT340_printterminator(con, con->printerHandle);

								con->gstat &= ~(G_AUTO_PRINT | G_AUTO_PRINT_INACTIVE);
							}

							else
							{
								if(VT340_openprinterFH(con) != FALSE)
								{
									if(con->gstat & G_PRINT_CONTROL)
										con->gstat |=  G_AUTO_PRINT_INACTIVE;
									else
										con->gstat |= G_AUTO_PRINT;
								}
							}

							CreateOwnStatusLine(con);
							return(0);
						}


						if(VT340_openscreenFH(con) != FALSE)	/* ASCII SCREEN HARDCOPY */
						{
							VT340_printinfo(con, con->screenHandle);
							PagePrintDisplay(con, con->screenHandle, TRUE);
							VT340_printterminator(con, con->screenHandle);
							VT340_closescreenFH(con);
						}
					}
					return(0);

					case 2:		/* SET UP */
						VT340_options((APTR)con);
					return(0);

					case 3:		/* DATA / TALK */
						VT340_transfer(con);
					return(0);

					case 4:		/* VA */
						if(ctrl != FALSE)
						{
							if(con->answerBack[0] != '\0')
								con->io->xem_swrite(con->answerBack, -1);
						}
						else
						{
							if(alt == FALSE)
								con->io->xem_sbreak();
						}

					return(0);

				}
			}

			if(cnt == 14  &&  !ctrl  &&  !alt)	/* Shifted-VA => HANG UP */
			{
				UWORD i;

				Delay(50);
				for(i=0; i<3; i++)
				{
					con->io->xem_swrite("+", 1);
					Delay(20);
				}
				Delay(50);
				con->io->xem_swrite("ATH\r", 4);

				return(0);
			}


			if(con->pstat & P_VT340_MODE)
			{
				if(con->pstat & P_CONVERT)
				{
					p[0] = ESC;
					p[1] = '[';
			   	strcpy(&p[2], fkeys[cnt - 5]);
				}
				else
			   	strcpy(&p[1], fkeys[cnt - 5]);
			}
			else	/* VT-52 or VT-102 */
			{
				switch(cnt + 1)
				{
					case 11:
						p[0] = ESC;
					break;

					case 12:
						p[0] = BS;
					break;

					case 13:
						p[0] = LF;
					break;

					default:
						p[0] = '\0';
					break;

				}

				if(p[0] != '\0')
				{
					p[1] = '\0';
					return(1);
				}
			}


			if(con->macrokeys)	/* are there any user-defined macrokeys..?? */
			{
				if(HandleMacroKeys(con, imsg, retstr[0]))
					return(0);
			}

			return((ULONG)strlen(p));
		}

	/* At last, it can only be the HELP key..)-: */

		if(p[1] == '?')
		{
			if(con->pstat & P_ANSI_MODE  &&  con->pstat & P_VT340_MODE)
			{
				UBYTE cnt=0;

				if(con->pstat & P_CONVERT)
				{
					p[cnt++] = ESC;
					p[cnt++] = '[';
				}
				else
					cnt++;	/* skip CSI */

				p[cnt++] = '2';				/* transform into HELP or DO key */
				p[cnt++] = (shift) ? '9' : '8';
				p[cnt++] = '~';
				p[cnt] = '\0';

				return((ULONG)cnt);
			}
			else
			{
				p[0] = '\0';	/* VT-52 or VT-102 */
				return(0);
			}
		}
	}


	if(p[0] == CSI  &&  con->pstat & P_CONVERT  &&  con->pstat & P_ANSI_MODE)
	{
		UBYTE buf[24];

	/* Some none-VT Sequences like Scroll-Up/Down or Tab-Left.. */
		sprintf(buf, "\033[%s", &p[1]);
		strcpy(p, buf);

		return((ULONG)strlen(p));
	}


	if(npad != FALSE)
	{
		UBYTE c=p[0];

		if(c >= '0'  &&  c <= '9')
		{
			if(shift  &&  con->pstat & P_ANSI_MODE  &&  con->pstat & P_VT340_MODE)
			{
				if(c >= '4')	/* FIND, INSERT, REMOVE, etc. */
				{
					UBYTE cnt=0;

					if(con->pstat & P_CONVERT)
					{
						p[cnt++] = ESC;
						p[cnt++] = '[';
					}
					else
						p[cnt++] = CSI;
					p[cnt++] = nkeys[c - '4'];
					p[cnt++] = '~';
					p[cnt] = '\0';

					return((ULONG)cnt);
				}
				else
				{
					p[0] = '\0';
					return(0);	/* Shift1 - Shift3 are trapped.. */
				}
			}
			else
			{
				if(con->ustat & U_KEYAPP)
				{
					UBYTE cnt=0;

					if(con->pstat & P_ANSI_MODE)
					{
						if(con->pstat & P_CONVERT)
						{
							p[cnt++] = ESC;
							p[cnt++] = 'O';
						}
						else
							p[cnt++] = SS3;
					}
					else
					{
						p[cnt++] = ESC;
						p[cnt++] = '?';
					}
					p[cnt++] = akeys[c - '0'];
					p[cnt] = '\0';

					return((ULONG)cnt);
				}
				else
					return(1);	/* ok */
			}
		}

		c = '\0';	/* from now on, we have to work with raw-key codes..)-: */


		if(imsg->Code >= 0x005A  &&  imsg->Code <= 0x005D)	/* PF1 - PF4 */
		{
			UBYTE cnt=0;

			if(shift  ||  ctrl  ||  alt)
				return(length);

			if(con->pstat & P_ANSI_MODE)
			{
				if(con->pstat & P_CONVERT)
				{
					p[cnt++] = ESC;
					p[cnt++] = 'O';
				}
				else
					p[cnt++] = SS3;
			}
			else
				p[cnt++] = ESC;
			p[cnt++] = pfkeys[imsg->Code - 0x005A];
			p[cnt] = '\0';

			return((ULONG)cnt);
		}


		if(con->ustat & U_KEYAPP)
		{
			UBYTE cnt=0;

			switch(imsg->Code)
			{
/* - */		case 0x004A:
					c = 'm';
				goto matched;

/* , */		case 0x005E:
					c = 'l';
				goto matched;

/* . */		case 0x003C:
					c = 'n';
				goto matched;

/* ENTER */	case 0x0043:
					c = 'M';
matched:
					if(con->pstat & P_ANSI_MODE)
					{
						if(con->pstat & P_CONVERT)
						{
							p[cnt++] = ESC;
							p[cnt++] = 'O';
						}
						else
							p[cnt++] = SS3;
					}
					else
					{
						p[cnt++] = ESC;
						p[cnt++] = '?';
					}
					p[cnt++] = c;
					p[cnt] = '\0';

				return((ULONG)cnt);

			}
		}
	}


	if(imsg->Code == 0x0041)	/* BS */
	{
		p[0] = (con->ustat & U_BACKSPACE_BS) ? BS : DEL;
		p[1] = '\0';

		return(2);
	}


	if(length == 1)
	{
		switch(p[0])
		{
			case CR:
				if(con->ustat & U_NEWLINE  &&  (imsg->Code == 0x0043  ||  imsg->Code == 0x0044))
				{
					;
					p[1] = LF;
					p[2] = '\0';
					length = 2;
				}
			break;

			case '3':	/* ESC */
			case '4':	/* FS  */
			case '5':	/* GS  */
			case '6':	/* RS  */
			case '7':	/* US  */
				if(ctrl != FALSE)
					p[0] -= ('3' - ESC);
			break;

		}
	}

	return(length);
}


BOOL __saveds __asm XEmulatorOptions(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
		return(VT340_options(con));
	else
		return(FALSE);
}
BOOL VT340_options(struct VT340Console *con)
{
#define BUFLEN 4

enum	{	CMD_HEADER=0, CMD_CLASSES, CMD_IDENTIFY,
			CMD_STDOPTS, CMD_OPTIONS, CMD_ALIGN,
			CMD_RELEASE, CMD_HARDRESET, CMD_SOFTRESET,
			NUMCMDS };

enum	{	CLA_HEADER=0, CLA_VT52, CLA_VT102, CLA_VT340_7, CLA_VT340_8,
			NUMCLAS };

enum	{	ID_HEADER=0, ID_VT100, ID_VT101, ID_VT102, ID_VT220, ID_VT340,
			NUMIDS };

enum	{	OPT_HEADER=0, OPT_NEWLINE, OPT_INSERT, OPT_WRAP,
			OPT_CURSOR, OPT_SCROLL, OPT_QUICK, OPT_LINES, OPT_ADJUST,
			OPT_BS, OPT_FF, OPT_DEL,
			OPT_BSDEL, OPT_ISO, OPT_COLOR,
			OPT_CURBLI, OPT_CURUND,
			OPT_STSLINE, OPT_SCREEN, OPT_PRINTER, OPT_ANSWER,
			NUMOPTS };


	STATIC UBYTE *ident[] = { 
	/*  01234567890123456 */
		"  VT-100  ",
		"  VT-101  ",
		"  VT-102  ",
		"  VT-220  ",
		"  VT-340  " };

	STATIC UBYTE *sclasses[] = { 
	/*  01234567890123456 */
		"  VT-52  7Bits  ",
		"  VT-102 7Bits  ",
		"  VT-340 7Bits  ",
		"  VT-340 8Bits  " };

	UBYTE *mark;

	struct xem_option *opti[NUMOPTS];
	UBYTE buff[17][BUFLEN];
	UWORD i;

	struct xem_option opt0;
	struct xem_option opt1;
	struct xem_option opt2;
	struct xem_option opt3;
	struct xem_option opt4;
	struct xem_option opt5;
	struct xem_option opt6;
	struct xem_option opt7;
	struct xem_option opt8;
	struct xem_option opt9;
	struct xem_option opt10;
	struct xem_option opt11;
	struct xem_option opt12;
	struct xem_option opt13;
	struct xem_option opt14;
	struct xem_option opt15;
	struct xem_option opt16;
	struct xem_option opt17;
	struct xem_option opt18;
	struct xem_option opt19;
	struct xem_option opt20;


	PageCursorInvisible(con);

AGAIN:

	opti[CMD_HEADER]	= &opt0;
	opti[CMD_CLASSES]	= &opt1;
	opti[CMD_IDENTIFY]= &opt2;
	opti[CMD_STDOPTS]	= &opt3;
	opti[CMD_OPTIONS]	= &opt4;
	opti[CMD_ALIGN]	= &opt5;
	opti[CMD_RELEASE]	= &opt6;
	opti[CMD_HARDRESET]= &opt7;
	opti[CMD_SOFTRESET]= &opt8;


/****/
	VT340_configopt(opti[CMD_HEADER],	"MAIN MENU",			NULL,	0,	XEMO_HEADER, FALSE);
/****/
	VT340_configopt(opti[CMD_CLASSES],	"Service Classes",	NULL,	0,	XEMO_COMMAND, FALSE);
/****/
	if(con->pstat & P_ANSI_MODE)
		VT340_configopt(opti[CMD_IDENTIFY],	"Identify Terminal",	NULL,	0,	XEMO_COMMAND, FALSE);
	else
		VT340_configopt(opti[CMD_IDENTIFY], "",					NULL,	0,	NULL,			  FALSE);
/****/
	VT340_configopt(opti[CMD_STDOPTS],	"DEC®-VT Settings",		NULL,	0,	XEMO_COMMAND, FALSE);
/****/
	VT340_configopt(opti[CMD_OPTIONS],	"Change Settings",	NULL,	0,	XEMO_COMMAND, FALSE);
/****/
	VT340_configopt(opti[CMD_ALIGN],		"Screen Alignment",	NULL,	0,	XEMO_COMMAND, FALSE);
/****/
	VT340_configopt(opti[CMD_RELEASE],	"Unlock Keyboard",	NULL,	0,	XEMO_COMMAND, FALSE);
/****/
		VT340_configopt(opti[CMD_HARDRESET], "Hard Reset (RIS)",	NULL,	0,	XEMO_COMMAND, FALSE);
/****/
	if(con->pstat & P_ANSI_MODE  &&  con->pstat & P_VT340_MODE)
		VT340_configopt(opti[CMD_SOFTRESET], "Soft Reset (DECSTR)",	NULL,	0,	XEMO_COMMAND, FALSE);
	else
		VT340_configopt(opti[CMD_SOFTRESET], "",					NULL,	0,	NULL,			  FALSE);
/****/


	switch(con->io->xem_toptions(NUMCMDS, opti))
	{
		case (1 << CMD_CLASSES):
		{
			opti[CLA_HEADER]	= &opt0;
			opti[CLA_VT52]		= &opt1;
			opti[CLA_VT102]	= &opt2;
			opti[CLA_VT340_7]	= &opt3;
			opti[CLA_VT340_8]	= &opt4;

			VT340_configopt(opti[CLA_HEADER],	"SERVICE CLASSES:", NULL,	0,	XEMO_HEADER, FALSE);
			VT340_configopt(opti[CLA_VT52],		sclasses[0],	NULL,	0,	XEMO_COMMAND, FALSE);
			VT340_configopt(opti[CLA_VT102],		sclasses[1],	NULL,	0,	XEMO_COMMAND, FALSE);
			VT340_configopt(opti[CLA_VT340_7],	sclasses[2],	NULL,	0,	XEMO_COMMAND, FALSE);
			VT340_configopt(opti[CLA_VT340_8],	sclasses[3],	NULL,	0,	XEMO_COMMAND, FALSE);

			if(con->pstat & P_ANSI_MODE)
			{
				if(con->pstat & P_VT340_MODE)
				{
					if(con->pstat & P_CONVERT)
						mark = sclasses[2];		/* VT-340 7 bit */
					else
						mark = sclasses[3];		/* VT-340 8 bit */
				}
				else
					mark = sclasses[1];			/* VT-102 */
			}
			else
				mark = sclasses[0];				/* VT-52 */

			for(i=0; i<4; i++)
			{
				if(sclasses[i] == mark)
				{
					sclasses[i][0]		= '»';
					sclasses[i][15]	= '«';
				}
				else
				{
					sclasses[i][0]		= ' ';
					sclasses[i][15]	= ' ';
				}
			}

			switch(con->io->xem_toptions(NUMCLAS, opti))
			{
				case (1 << CLA_VT52):
					VT340_invokeVT52(con);
				break;

				case (1 << CLA_VT102):
					VT340_resetcon(con, FALSE);
					con->pstat |= P_ANSI_MODE;
					con->pstat &= ~P_VT340_MODE;
					con->pstat |= P_CONVERT;
				break;

				case (1 << CLA_VT340_7):
					VT340_resetcon(con, FALSE);
					con->pstat |= P_ANSI_MODE;
					con->pstat |= P_VT340_MODE;
					con->pstat |= P_CONVERT;
				break;

				case (1 << CLA_VT340_8):
					VT340_resetcon(con, FALSE);
					con->pstat |= P_ANSI_MODE;
					con->pstat |= P_VT340_MODE;
					con->pstat &= ~P_CONVERT;
				break;

			}

			CreateOwnStatusLine(con);
		}
		goto AGAIN;

		case (1 << CMD_IDENTIFY):
		{
			if((con->pstat & P_ANSI_MODE) == FALSE)
				break;

			mark = NULL;

			switch(con->identify)
			{
				default:
				case IDENT_VT100:
					mark = ident[0];
				break;

				case IDENT_VT101:
					mark = ident[1];
				break;

				case IDENT_VT102:
					mark = ident[2];
				break;

				case IDENT_VT220:
					mark = ident[3];
				break;

				case IDENT_VT340:
					mark = ident[4];
				break;

			}

			for(i=0; i<5; i++)
			{
				if(ident[i] == mark)
				{
					ident[i][0]	= '»';
					ident[i][9]	= '«';
				}
				else
				{
					ident[i][0]	= ' ';
					ident[i][9]	= ' ';
				}
			}

			opti[ID_HEADER]= &opt0;
			opti[ID_VT100]	= &opt1;
			opti[ID_VT101]	= &opt2;
			opti[ID_VT102]	= &opt3;
			opti[ID_VT220]	= &opt4;
			opti[ID_VT340]	= &opt5;

			VT340_configopt(opti[ID_HEADER],	"IDENTIFY TERMINAL:", NULL,	0,	XEMO_HEADER, FALSE);
			VT340_configopt(opti[ID_VT100], ident[0], NULL, 0, XEMO_COMMAND, FALSE);
			VT340_configopt(opti[ID_VT101], ident[1], NULL, 0, XEMO_COMMAND, FALSE);
			VT340_configopt(opti[ID_VT102], ident[2], NULL, 0, XEMO_COMMAND, FALSE);
			VT340_configopt(opti[ID_VT220], ident[3], NULL, 0, XEMO_COMMAND, FALSE);
			VT340_configopt(opti[ID_VT340], ident[4], NULL, 0, XEMO_COMMAND, FALSE);


			switch(con->io->xem_toptions(NUMIDS, opti))
			{
				case (1 << ID_VT100):
					con->identify = IDENT_VT100;
				break;

				case (1 << ID_VT101):
					con->identify = IDENT_VT101;
				break;

				case (1 << ID_VT102):
					con->identify = IDENT_VT102;
				break;

				case (1 << ID_VT220):
					con->identify = IDENT_VT220;
				break;

				case (1 << ID_VT340):
					con->identify = IDENT_VT340;
				break;

			}
		}
		goto AGAIN;

		case (1 << CMD_STDOPTS):
		{
			ULONG r_ustat, r_tstat, r_pstat, r_identify;

			r_identify	= con->r_identify;
			r_tstat		= con->r_tstat;
			r_ustat		= con->r_ustat;
			r_pstat		= con->r_pstat;


			con->r_identify = IDENT_VT340;
/***/
			con->r_tstat &= ~(T_WRAP);
/***/
			con->r_ustat &= ~(U_NEWLINE);
/***/
			con->r_pstat |=  (P_ANSI_MODE | P_VT340_MODE);
			con->r_pstat |=  (P_24LINES | P_CONVERT);
			con->r_pstat &= ~(P_DESTRUCTIVEBS | P_DESTRUCTIVEFF | P_DESTRUCTIVEDEL | P_OWN_STATUSLINE);
			con->r_pstat &= ~(P_ADJUST_LINES | P_SMOOTH_SCROLL | P_ISO_LATIN1 | P_QUICK_REND);
			con->r_pstat &= ~(P_CURSOR_BLINKING | P_CURSOR_UNDERSCORE);


			VT340_resetcon(con, TRUE);

			con->r_ustat	= r_ustat;
			con->r_tstat	= r_tstat;
			con->r_pstat	= r_pstat;
			con->r_identify= r_identify;
		}
		/* fall thru */

		case (1 << CMD_OPTIONS):
		{
/*			VOID KPrintF(UBYTE *, ...);*/

			ULONG changed;
			BOOL val1, val2;

			opti[OPT_HEADER]	= &opt0;
			opti[OPT_NEWLINE]	= &opt1;
			opti[OPT_INSERT]	= &opt2;
			opti[OPT_WRAP]		= &opt3;
			opti[OPT_CURSOR]	= &opt4;
			opti[OPT_SCROLL]	= &opt5;
			opti[OPT_QUICK]	= &opt6;
			opti[OPT_LINES]	= &opt7;
			opti[OPT_BS]		= &opt8;
			opti[OPT_FF]		= &opt9;
			opti[OPT_DEL]		= &opt10;
			opti[OPT_ADJUST]	= &opt11;
			opti[OPT_BSDEL]	= &opt12;
			opti[OPT_ISO]		= &opt13;
			opti[OPT_COLOR]	= &opt14;
			opti[OPT_CURBLI]	= &opt15;
			opti[OPT_CURUND]	= &opt16;
			opti[OPT_STSLINE]	= &opt17;
			opti[OPT_SCREEN]	= &opt18;
			opti[OPT_PRINTER]	= &opt19;
			opti[OPT_ANSWER]	= &opt20;

			VT340_configopt(opti[OPT_HEADER],	"GENERAL SETTINGS..:", NULL,	0,	XEMO_HEADER,	FALSE);
			VT340_configopt(opti[OPT_NEWLINE],	"Newline mode......:", buff[0],	BUFLEN,	XEMO_BOOLEAN,	con->ustat & U_NEWLINE);
			VT340_configopt(opti[OPT_INSERT],	"Insert mode.......:", buff[1],	BUFLEN,	XEMO_BOOLEAN,	con->ustat & U_INSERT);
			VT340_configopt(opti[OPT_WRAP],		"Wrap mode.........:", buff[2],	BUFLEN,	XEMO_BOOLEAN,	con->tstat & T_WRAP);
			VT340_configopt(opti[OPT_CURSOR],	"Cursor visible....:", buff[3],	BUFLEN,	XEMO_BOOLEAN,	con->ustat & U_CURSOR_VISIBLE);
			VT340_configopt(opti[OPT_SCROLL],	"Smooth scroll.....:", buff[4],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_SMOOTH_SCROLL);
			VT340_configopt(opti[OPT_QUICK],		"Quick rend........:", buff[5],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_QUICK_REND);
			VT340_configopt(opti[OPT_LINES],		"Use 24 lines......:", buff[6],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_24LINES);
			VT340_configopt(opti[OPT_ADJUST],	"Adjust lines......:", buff[7],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_ADJUST_LINES);
			VT340_configopt(opti[OPT_STSLINE],	"Own status-line...:", buff[8],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_OWN_STATUSLINE);
			VT340_configopt(opti[OPT_BS],			"Destructive BSpace:", buff[9],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_DESTRUCTIVEBS);
			VT340_configopt(opti[OPT_FF],			"Destructive FFeed.:", buff[10],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_DESTRUCTIVEFF);
			VT340_configopt(opti[OPT_DEL],		"Destructive Delete:", buff[11],	BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_DESTRUCTIVEDEL);
			VT340_configopt(opti[OPT_BSDEL],		"BackspaceKey = BS.:", buff[12], BUFLEN,	XEMO_BOOLEAN,	con->ustat & U_BACKSPACE_BS);
			VT340_configopt(opti[OPT_ISO],		"Use ISO-Latin1....:", buff[13], BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_ISO_LATIN1);
			VT340_configopt(opti[OPT_CURBLI],	"Cursor blinking...:", buff[14], BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_CURSOR_BLINKING);
			VT340_configopt(opti[OPT_CURUND],	"Cursor underscore.:", buff[15], BUFLEN,	XEMO_BOOLEAN,	con->pstat & P_CURSOR_UNDERSCORE);
			VT340_configopt(opti[OPT_COLOR],		"Use ANSI colors...:", buff[16], BUFLEN,	(con->mono) ? NULL : XEMO_BOOLEAN, con->pstat & P_ANSI_COLORS);
			VT340_configopt(opti[OPT_SCREEN],	"Save screen to....:", con->screenName,	SCREEN_BUF, XEMO_STRING, FALSE);
			VT340_configopt(opti[OPT_PRINTER],	"Printer name......:", con->printerName,	SCREEN_BUF, XEMO_STRING, FALSE);
			VT340_configopt(opti[OPT_ANSWER],	"Answer-back msg...:", con->answerBack, ANSWER_BACK, XEMO_STRING, FALSE);


			changed = con->io->xem_toptions(NUMOPTS, opti);

			DeleteStatusLine(con, TRUE);

			VT340_setopt(opti[OPT_WRAP],	 &con->tstat, T_WRAP,				changed, OPT_WRAP);
			VT340_setopt(opti[OPT_INSERT], &con->ustat, U_INSERT,				changed, OPT_INSERT);
			VT340_setopt(opti[OPT_NEWLINE],&con->ustat, U_NEWLINE,			changed, OPT_NEWLINE);
			VT340_setopt(opti[OPT_BSDEL],	 &con->ustat, U_BACKSPACE_BS,		changed, OPT_BSDEL);
			VT340_setopt(opti[OPT_CURSOR], &con->ustat, U_CURSOR_VISIBLE,	changed, OPT_CURSOR);
			VT340_setopt(opti[OPT_CURBLI], &con->pstat, P_CURSOR_BLINKING,	changed, OPT_CURBLI);
			VT340_setopt(opti[OPT_CURUND], &con->pstat, P_CURSOR_UNDERSCORE,changed,OPT_CURUND);
			VT340_setopt(opti[OPT_SCROLL], &con->pstat, P_SMOOTH_SCROLL,	changed, OPT_SCROLL);
			VT340_setopt(opti[OPT_QUICK],	 &con->pstat, P_QUICK_REND,		changed, OPT_QUICK);
			VT340_setopt(opti[OPT_BS],		 &con->pstat, P_DESTRUCTIVEBS,	changed, OPT_BS);
			VT340_setopt(opti[OPT_FF],		 &con->pstat, P_DESTRUCTIVEFF,	changed, OPT_FF);
			VT340_setopt(opti[OPT_DEL],	 &con->pstat, P_DESTRUCTIVEDEL,	changed, OPT_DEL);
			VT340_setopt(opti[OPT_STSLINE],&con->pstat, P_OWN_STATUSLINE,	changed, OPT_STSLINE);
			if(VT340_setopt(opti[OPT_COLOR],&con->pstat,P_ANSI_COLORS,		changed, OPT_COLOR))
			{
				VT340_colorModes(con);
				VT340_resetstyles(con, TRUE);	/* reset colors only */
			}

			val1 = VT340_setopt(opti[OPT_LINES],	&con->pstat, P_24LINES,			changed, OPT_LINES);
			val2 = VT340_setopt(opti[OPT_ADJUST],	&con->pstat, P_ADJUST_LINES,	changed, OPT_ADJUST);
			if(val1  ||  val2)
				VT340_resetscreen(con, TRUE);


			val1 = VT340_setopt(opti[OPT_ISO], &con->pstat, P_ISO_LATIN1, changed, OPT_ISO);
			if(val1)
			{
				UWORD old_upss, i;

				old_upss = con->upss;
				con->upss = (con->upss == ISO_SUPPLEMENTAL) ? DEC_SUPPLEMENTAL : ISO_SUPPLEMENTAL;

				for(i=0; i<SETS; i++)
				{
					if(con->gset[i] == old_upss)
						VT340_setset(con, con->upss, i);
				}
			}

			if(changed & (1 << OPT_SCREEN))
				VT340_closescreenFH(con);

			if(changed & (1 << OPT_PRINTER))
				VT340_closeprinterFH(con);

			if(con->pstat & P_CURSOR_BLINKING)
				CreateCursorServer(con);
			else
				DeleteCursorServer(con);

			PageCursorVisible(con);

			CreateOwnStatusLine(con);
		}
		goto AGAIN;

		case (1 << CMD_ALIGN):
			VT340_screenaligntest(con);
		goto AGAIN;

		case (1 << CMD_RELEASE):
			VT340_cancel(con);
		goto AGAIN;

		case (1 << CMD_HARDRESET):
			VT340_resetcon(con, TRUE);		/* Reset To Initial State */
		goto AGAIN;

		case (1 << CMD_SOFTRESET):
			if(con->pstat & P_ANSI_MODE  &&  con->pstat & P_VT340_MODE)
				VT340_resetcon(con, FALSE);
		goto AGAIN;

	}

	PageCursorVisible(con);

	return(TRUE);
}


VOID __saveds __asm XEmulatorClearConsole(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		VT340_resetscreen(con, TRUE);
		PageCursorVisible(con);
	}
}


VOID __saveds __asm XEmulatorResetConsole(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		PageCursorInvisible(con);

		if(con->pstat & P_ANSI_MODE)
		{
			if(con->pstat & P_VT340_MODE)
				VT340_resetcon(con, FALSE);	/* VT-340  Soft-Reset */
			else
				VT340_resetcon(con, TRUE);		/* VT-102 */
		}
		else
			VT340_invokeVT52(con);				/* VT-52 */

		PageCursorVisible(con);
	}
}


VOID __saveds __asm XEmulatorResetTextStyles(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		VT340_resetstyles(con, FALSE);
	}
}


VOID __saveds __asm XEmulatorResetCharset(register __a0 struct XEM_IO *io)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		VT340_resetfonts(con);
	}
}


ULONG __saveds __asm XEmulatorGetFreeMacroKeys(register __a0 struct XEM_IO *io, register __d0 ULONG qualifier)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	switch(qualifier)
	{
		case 0:	/* NONE */
			return(512 + 256 + 128 + 64 + 32);
		break;

		case 1:	/* SHIFT */
			if(con->pstat & P_VT340_MODE)
				return(512 + 256 + 128 + 64 + 32 + 8 + 4 + 2 + 1);
			else
				return(512 + 256 + 128 + 64 + 32 + 8);
		break;

		case 2:	/* ALTERNATE */
			return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 1);
		break;

		case 3:	/* CONTROL */
			return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 1);
		break;

	}
}


BOOL __saveds __asm XEmulatorMacroKeyFilter(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorMacroKey *macrokeys)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL  &&  con->io->xem_process_macrokeys)
	{
		con->macrokeys = macrokeys;
		return(TRUE);
	}

	return(FALSE);
}


LONG __saveds __asm XEmulatorInfo(register __a0 struct XEM_IO *io, register __d0 ULONG type)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;
	LONG result = -1;

	if(con != NULL)
	{
		switch(type)
		{
			case XEMI_CURSOR_POSITION:
				result = XEMI_CREATE_POSITION(con->row, con->col);
			break;

			case XEMI_CONSOLE_DIMENSIONS:
				result = XEMI_CREATE_DIMENSIONS(con->columns, con->lastLine);
			break;

		}
	}

	return(result);
}


BOOL __saveds __asm XEmulatorPreferences(register __a0 struct XEM_IO *io, register __a1 STRPTR prefsFile, register __d0 ULONG mode)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;
	BOOL success=FALSE;

	if(con != NULL)
		success = VT340_ParseInit(con, prefsFile, mode);

	return(success);
}


STATIC VOID VT340_checkframe(struct VT340Console *con)
{
	UWORD lines, columns;

	lines	 = con->border.Height >> 3;
	columns= con->border.Width  >> 3;

	if(con->columns != columns  ||  con->statusLine != lines)
	{
		VT340_resetscreen(con, FALSE);
		RefreshPage(con);
	}
}


VOID __saveds __asm XEmulatorNewSize(register __a0 struct XEM_IO *io, register __a1 struct IBox *frame)
{
	struct VT340Console *con = (struct VT340Console *)io->xem_console;

	if(con != NULL)
	{
		PageCursorInvisible(con);

		VT340_newframe(con, frame);
		VT340_checkframe(con);

		DeleteStatusLine(con, FALSE);
		CreateOwnStatusLine(con);

		PageCursorVisible(con);
	}
}



/* end of source-code */
