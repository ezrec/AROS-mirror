#include "ownincs/VT340console.h"

       /* dies wird einmal ein GUTER (=schneller..??) Parser..(-: */

struct VT52_Parm {
	VOID (*Routine)(struct VT340Console *con, UBYTE *args, ULONG argc);
	BYTE	TransparentPrint;
	UBYTE MatchChar;
};

struct VT52_Parm VT52_cmds[] = {
	{ VT340_simplecursorup		, FALSE	,	'A' },
	{ VT340_simplecursordown	, FALSE	,	'B' },
	{ VT340_simplecursorright	, FALSE	,	'C' },
	{ VT340_simplecursorleft	, FALSE	,	'D' },
	{ VT52_EnterGrafixMode		, FALSE	,	'F' },
	{ VT52_ExitGrafixMode		, FALSE	,	'G' },
	{ VT340_cursorposition		, FALSE	,	'H' },
	{ VT340_cursorup				, FALSE	,	'I' },
	{ VT340_eraseindisplay		, FALSE	,	'J' },
	{ VT340_eraseinline			, FALSE	,	'K' },
/* direct cursor address is handled specially..<-: */
	{ VT52_Identify				, FALSE	,	'Z' },
	{ VT52_EnterAltKeyboardMode, FALSE	,	'=' },
	{ VT52_ExitAltKeyboardMode	, FALSE	,	'>' },
	{ VT52_EnterANSI				, FALSE	,	'<' },
	{ VT52_EnterAutoPrint		, FALSE	,	'^' },
	{ VT52_ExitAutoPrint			, FALSE	,	'_' },
	{ VT52_EnterControlPrint	, FALSE	,	'W' },
	{ VT52_ExitControlPrint		, TRUE	,	'X' },
	{ VT52_PrintScreen			, FALSE	,	']' },
	{ VT52_PrintRow				, FALSE	,	'V' },

	{ NULL, NUL },
};


STATIC VOID VT340_nul(struct VT340Console *con);
STATIC VOID VT340_enq(struct VT340Console *con);
STATIC VOID VT340_bel(struct VT340Console *con);
STATIC VOID VT340_bs(struct VT340Console *con);
STATIC VOID VT340_ht(struct VT340Console *con);
STATIC VOID VT340_lf(struct VT340Console *con);
STATIC VOID VT340_ff(struct VT340Console *con);
STATIC VOID VT340_cr(struct VT340Console *con);
STATIC VOID VT340_so(struct VT340Console *con);
STATIC VOID VT340_si(struct VT340Console *con);
STATIC VOID VT340_xon(struct VT340Console *con);
STATIC VOID VT340_xof(struct VT340Console *con);
STATIC VOID VT340_can(struct VT340Console *con);
STATIC VOID VT340_sub(struct VT340Console *con);
STATIC VOID VT340_esc(struct VT340Console *con);
STATIC VOID VT340_del(struct VT340Console *con);
STATIC VOID VT340_ind(struct VT340Console *con);
STATIC VOID VT340_nel(struct VT340Console *con);
STATIC VOID VT340_ssa(struct VT340Console *con);
STATIC VOID VT340_esa(struct VT340Console *con);
STATIC VOID VT340_hts(struct VT340Console *con);
STATIC VOID VT340_ri(struct VT340Console *con);
STATIC VOID VT340_ss2(struct VT340Console *con);
STATIC VOID VT340_ss3(struct VT340Console *con);
STATIC VOID VT340_dcs(struct VT340Console *con);
STATIC VOID VT340_sts(struct VT340Console *con);
STATIC VOID VT340_spa(struct VT340Console *con);
STATIC VOID VT340_epa(struct VT340Console *con);
STATIC VOID VT340_csi(struct VT340Console *con);
STATIC VOID VT340_st(struct VT340Console *con);
STATIC VOID VT340_osc(struct VT340Console *con);
STATIC VOID VT340_pm(struct VT340Console *con);
STATIC VOID VT340_apc(struct VT340Console *con);


UBYTE PrintableTab[256] =
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
#define IsPrintable(c)	(PrintableTab[(c)])

struct SpecialKey SpecialKeys[34] = {	/* nicht vergessen, in funcs. das */
	VT340_nul, NUL,							/* IMPORT nachzuführen */
	VT340_enq, ENQ,	
	VT340_bel, BEL,	
	VT340_bs	, BS,	
	VT340_ht	, HT,	
	VT340_lf	, LF,	
	VT340_lf	, VT,		/* `VT' is handled the same way as `LF' */
	VT340_ff	, FF,	
	VT340_cr	, CR,	
	VT340_so	, SO,	
	VT340_si	, SI,	
	VT340_xon, XON,	
	VT340_xof, XOF,	
	VT340_can, CAN,	
	VT340_sub, SUB,	
	VT340_esc, ESC,	
	VT340_del, DEL,	

	VT340_ind, IND,	
	VT340_nel, NEL,	
	VT340_ssa, SSA,	
	VT340_esa, ESA,	
	VT340_hts, HTS,	
	VT340_ri	, RI,	
	VT340_ss2, SS2,	
	VT340_ss3, SS3,	
	VT340_dcs, DCS,	
	VT340_sts, STS,	
	VT340_spa, SPA,	
	VT340_epa, EPA,	
	VT340_csi, CSI,	
	VT340_st	, ST,	
	VT340_osc, OSC,	
	VT340_pm	, PM,	
	VT340_apc, APC,	
};


struct VT340_Parm {
	VOID	(*Routine)(struct VT340Console *con, UBYTE *args, ULONG argc);
	BYTE	TransparentPrint;
	UBYTE	ServiceClass;
	UBYTE	Intermediate;	/* FIRST intermediate..!! */
	UBYTE	MatchChar;
};

enum { BOTH=1, VT340 };

struct VT340_Parm ESC_cmds[] = {								/* Mnemonic */
	{ VT340_invokeCSI				, TRUE,	BOTH	,	NUL,	'[' },	/* CSI */
	{ VT340_cursordown			, FALSE,	BOTH	,	NUL,	'D' },	/* IND */
	{ VT340_cursornextline		, FALSE,	BOTH	,	NUL,	'E' },	/* NEL */
	{ VT340_cursorup				, FALSE, BOTH	,	NUL,	'M' },	/* IND */
	{ VT340_setTab					, FALSE, BOTH	,	NUL,	'H' },	/* HTS */
	{ VT340_setconvert			, FALSE, VT340	,	' ',	'F' },	/* S7C1T */
	{ VT340_setconvert			, FALSE, VT340	,	' ',	'G' },	/* S8C1T */
	{ VT340_requestterminal		, FALSE, BOTH	,	NUL,	'Z' },	/* DECID */
	{ VT340_hardreset				, FALSE, BOTH	,	NUL,	'c' },	/* RIS */
	{ VT340_savecursor			, FALSE, BOTH	,	NUL,	'7' },	/* DECSC */
	{ VT340_restorecursor		, FALSE, BOTH	,	NUL,	'8' },	/* DECRC */
	{ VT340_setnumpad				, FALSE, BOTH	,	NUL,	'=' },	/* DECKPNM */
	{ VT340_setnumpad				, FALSE, BOTH	,	NUL,	'>' },	/* DECKPAM */
/* */		
	{ PageTextScaleMode			, FALSE, BOTH	,	'#',	'3' },	/* DECDHL top half */
	{ PageTextScaleMode			, FALSE, BOTH	,	'#',	'4' },	/* DECDHL bot half */
	{ PageTextScaleMode			, FALSE, BOTH	,	'#',	'5' },	/* DECSWL */
	{ PageTextScaleMode			, FALSE, BOTH	,	'#',	'6' },	/* DECDWL */
	{ PageTextScaleMode			, FALSE, BOTH	,	'#',	'8' },	/* DECALN */
/* */
	{ VT340_setG0					, FALSE, BOTH	,	'(',	'B' },	/* SCS G0 ASCII			*/
	{ VT340_setG0					, FALSE, BOTH	,	'(',	'0' },	/* SCS G0 GRAPHICS		*/
	{ VT340_setG0					, FALSE, BOTH	,	'(',	'A' },	/* SCS G0 ISO SUPPL.		*/
	{ VT340_setG0					, FALSE, VT340	,	'(',	'5' },	/* SCS G0 DEC SUPPL.		*/
	{ VT340_setG0					, FALSE, VT340	,	'(',	'<' },	/* SCS G0 PREF. SUPPL.	*/
	{ VT340_setG0					, FALSE, VT340	,	'(',	'>' },	/* SCS G0 TECHNICAL.		*/
/* */
	{ VT340_setG1					, FALSE, BOTH	,	')',	'B' },	/* SCS G1 ASCII			*/
	{ VT340_setG1					, FALSE, BOTH	,	')',	'0' },	/* SCS G1 GRAPHICS		*/
	{ VT340_setG1					, FALSE, BOTH	,	')',	'A' },	/* SCS G1 ISO SUPPL.		*/
	{ VT340_setG1					, FALSE, VT340	,	')',	'5' },	/* SCS G1 DEC SUPPL.		*/
	{ VT340_setG1					, FALSE, VT340	,	')',	'<' },	/* SCS G1 PREF. SUPPL.	*/
	{ VT340_setG1					, FALSE, VT340	,	')',	'>' },	/* SCS G1 TECHNICAL.		*/
/* */
	{ VT340_setG2					, FALSE, VT340	,	'*',	'A' },	/* SCS G2 ISO SUPPL.		*/
	{ VT340_setG2					, FALSE, VT340	,	'*',	'B' },	/* SCS G2 ASCII			*/
	{ VT340_setG2					, FALSE, VT340	,	'*',	'0' },	/* SCS G2 GRAPHICS		*/
	{ VT340_setG2					, FALSE, VT340	,	'*',	'5' },	/* SCS G2 DEC SUPPL.		*/
	{ VT340_setG2					, FALSE, VT340	,	'*',	'<' },	/* SCS G2 PREF. SUPPL.	*/
	{ VT340_setG2					, FALSE, VT340	,	'*',	'>' },	/* SCS G2 TECHNICAL.		*/
/* */
	{ VT340_setG3					, FALSE, VT340	,	'+',	'A' },	/* SCS G3 ISO SUPPL.		*/
	{ VT340_setG3					, FALSE, VT340	,	'+',	'B' },	/* SCS G3 ASCII			*/
	{ VT340_setG3					, FALSE, VT340	,	'+',	'0' },	/* SCS G3 GRAPHICS		*/
	{ VT340_setG3					, FALSE, VT340	,	'+',	'5' },	/* SCS G3 DEC SUPPL.		*/
	{ VT340_setG3					, FALSE, VT340	,	'+',	'<' },	/* SCS G3 PREF. SUPPL.	*/
	{ VT340_setG3					, FALSE, VT340	,	'+',	'>' },	/* SCS G3 TECHNICAL.		*/
/* */
	{ VT340_setG1_GR				, FALSE, VT340	,	NUL,	'~' },	/* LS1R */
/* */
	{ VT340_setG2_GL				, FALSE, VT340	,	NUL,	'n' },	/* LS2  */
	{ VT340_setG2_GR				, FALSE, VT340	,	NUL,	'}' },	/* LS2R */
/* */
	{ VT340_setG3_GL				, FALSE, VT340	,	NUL,	'o' },	/* LS3  */
	{ VT340_setG3_GR				, FALSE, VT340	,	NUL,	'|' },	/* LS3R */
/* */
	{ VT340_setG2_GL_s			, FALSE, VT340	,	NUL,	'N' },	/* SS2  */
	{ VT340_setG3_GL_s			, FALSE, VT340	,	NUL,	'O' },	/* SS3  */
/* */
	{ VT340_invokeDCS				, FALSE, BOTH	,	NUL,	'P' },	/* DCS  */
/* */
	{ VT340_invokePrivate		, FALSE, BOTH	,	NUL,	']' },	/* OSC  */
	{ VT340_invokePrivate		, FALSE, BOTH	,	NUL,	'^' },	/* PM   */
	{ VT340_invokePrivate		, FALSE, BOTH	,	NUL,	'_' },	/* APC  */
	{ VT340_invokePrivate		, FALSE, BOTH	,	NUL,	'/' },	/* ST  */

	{ NULL, FALSE, NUL, NUL },	/* der krönende Abschluss..(-: */
};

struct VT340_Parm CSI_cmds[] = {								/* Mnemonic */
	{ VT340_simplecursorup		, FALSE, BOTH	,	NUL,	'A' },	/* CUU */
	{ VT340_simplecursordown	, FALSE, BOTH	,	NUL,	'B' },	/* CUD */
	{ VT340_simplecursorright	, FALSE, BOTH	,	NUL,	'C' },	/* CUF */
	{ VT340_simplecursorleft	, FALSE, BOTH	,	NUL,	'D' },	/* CUB */
	{ VT340_cursorposition		, FALSE, BOTH	,	NUL,	'H' },	/* CUP */
	{ VT340_textmodes				, FALSE, BOTH	,	NUL,	'm' },	/* SGR */
	{ VT340_ANSIsetmodes			, FALSE, BOTH	,	NUL,	'h' },	/* SM ANSI */
	{ VT340_ANSIresetmodes		, FALSE, BOTH	,	NUL,	'l' },	/* RM ANSI */
	{ VT340_DECsetmodes			, FALSE, BOTH	,  '?',	'h' },	/* SM DEC */
	{ VT340_DECresetmodes		, FALSE, BOTH	,  '?',	'l' },	/* RM DEC */

	{ VT340_insertchars			, FALSE, VT340	,	NUL,	'@' },	/* ICH */
	{ VT340_cursornextline		, FALSE, BOTH	,	NUL,	'E' },	/* CNL */
	{ VT340_cursoraboveline		, FALSE, BOTH	,	NUL,	'F' },	/* CPL */
	{ VT340_setcol					, FALSE, BOTH	,	NUL,	'G' },	/* CHA */
	{ VT340_tabForward			, FALSE, BOTH	,	NUL,	'I' },	/* CHI */
	{ VT340_eraseindisplay		, FALSE, BOTH	,	NUL,	'J' },	/* ED  */
	{ VT340_eraseinline			, FALSE, BOTH	,	NUL,	'K' },	/* EL  */
	{ VT340_sel_eraseindisplay	, FALSE, VT340	,	'?',	'J' },	/* DECSED */
	{ VT340_sel_eraseinline		, FALSE, VT340	,	'?',	'K' },	/* DECSEL */
	{ VT340_insertlines			, FALSE, BOTH	,	NUL,	'L' },	/* IL  */
	{ VT340_deletelines			, FALSE, BOTH	,	NUL,	'M' },	/* DL  */
	{ VT340_deletechars			, FALSE, BOTH	,	NUL,	'P' },	/* DCH */
	{ VT340_scrollcon				, FALSE, BOTH	,	NUL,	'S' },	/* SU  */
	{ VT340_scrollcon				, FALSE, BOTH	,	NUL,	'T' },	/* SD  */
	{ VT340_tabControl			, FALSE, BOTH	,	NUL,	'W' },	/* CTC */
	{ VT340_erasechars			, FALSE, VT340	,	NUL,	'X' },	/* ECH */
	{ VT340_tabBackward			, FALSE, BOTH	,	NUL,	'Z' },	/* CBT */

	{ VT340_moveCursForward		, FALSE, BOTH	,	NUL,	'a' },	/* CUF */
	{ VT340_requestterminal		, FALSE, BOTH	,	NUL,	'c' },	/* DA primary   */
	{ VT340_requestterminal		, FALSE, BOTH	,	'>',	'c' },	/* DA secondary */
	{ VT340_setrow					, FALSE, BOTH	,	NUL,	'd' },	/* CVA */
	{ VT340_moveCursDown			, FALSE, BOTH	,	NUL,	'e' },	/* CUD */
	{ VT340_cursorposition		, FALSE, BOTH	,	NUL,	'f' },	/* HVP */
	{ VT340_tabClear				, FALSE, BOTH	,	NUL,	'g' },	/* TBC */
	{ VT340_printcontrolmode	, TRUE,	BOTH	,  NUL,	'i' },	/* ANSI MC */
	{ VT340_autoprintmode		, FALSE, BOTH	,  '?',	'i' },	/* DEC MC */
	{ VT340_devicestatreport	, FALSE, BOTH	,	NUL,	'n' },	/* DSR */
	{ VT340_devicestatreport	, FALSE, BOTH	,	'?',	'n' },	/* DSR */
	{ VT340_softreset				, FALSE, VT340	,	'!',	'p' },	/* DECSTR */
	{ VT340_setserviceclass		, FALSE, VT340	,	'"',	'p' },	/* DECSCL */
	{ VT340_reportANSIsettings	, FALSE, VT340	,	'$',	'p' },	/* DECRQM ANSI */
	{ VT340_reportDECsettings	, FALSE, VT340	,	'?',	'p' },	/* DECRQM DEC  */
	{ VT340_setselective			, FALSE, VT340	,	'"',	'q' },	/* DECSCA */
	{ VT340_setregion				, FALSE, BOTH	,	NUL,	'r' },	/* DECSTBM */
	{ VT340_reportUPSS			, FALSE, VT340	,	'&',	'u' },	/* DECRQUPSS */
	{ VT340_reportcursorstate	, FALSE, VT340	,	'$',	'u' },	/* DECCIR */
	{ VT340_reportprestate		, FALSE, VT340	,	'$',	'w' },	/* DECRQPSR */
	{ VT340_selectactivedisplay, FALSE, VT340	,	'$',	'}' },	/* DECSASD */
	{ VT340_setstatusline		, FALSE, VT340	,	'$',	'~' },	/* DECSSDT */

	/* Following commands aren't implemented at this time */
	{ VT340_ignore					, FALSE, BOTH	,	NUL,	'R' },	/* CPR */
	{ VT340_ignore					, FALSE, BOTH	,	' ',	'@' },	/*  ?  */
	{ VT340_ignore					, FALSE, BOTH	,	' ',	'A' },	/*  ?  */
	{ VT340_ignore					, FALSE, VT340	,	NUL,	'y' },	/* DECRPM ANSI */
	{ VT340_ignore					, FALSE, VT340	,	'?',	'y' },	/* DECRPM DEC  */


	{ NULL, NUL, NUL },		/* der krönende Abschluss..(-: */
};


IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;
IMPORT struct	Library			*DiskfontBase;
IMPORT struct	Library			*KeymapBase;


VOID VT340_writecon(struct VT340Console *con, UBYTE *buff, LONG buflen)
{
	UWORD i;
	UBYTE c;

	if(con->displayInactive != FALSE  ||  con->isReady == FALSE)
		return;

	PageCursorInvisible(con);

	if(buflen == -1)
		buflen = strlen(buff);

	while(buflen--)
	{
		c = *buff++;

		if(con->pstat & P_ANSI_MODE)	/* VT-102 or VT-340 */
		{
/*
**			if(con->inTEK != FALSE)
**			{
**				if(TEK_parse(con, c) != FALSE)
**					continue;
**
**				KPrintF("VT340: TEK offline\r\n");
**			}
*/

			if(con->inPrivate != FALSE)
			{
			/* 'OSC' 'PM' 'APC' is cancelled only by an 'ST' or 'ESC /' */

				if(c == ST  ||  (con->lastPRIVc == ESC && c == '/'))
					VT340_cancel(con);
				else
					con->lastPRIVc = c;

				continue;
			}


			if(con->inDCS != FALSE)
			{
			/* 'DCS' is cancelled by an CAN ; SUB ; ESC ; ST */

				if(c == CAN  ||  c == SUB  ||  c == ESC  ||  c == ST)
					VT340_cancel(con);

				continue;
			}

			if(con->inESC != FALSE  &&  (c >= 32  &&  c < 127))
			{
				if(c < '0')
				{
					if(con->icnt < MAXINTER)
					{
						con->inter[con->icnt] = c;		/* something like '#', etc. */
						con->icnt++;
						con->inter[con->icnt] = NUL;	/* clear next intermediate */
					}
					continue;
				}

				for(i=0; ESC_cmds[i].Routine; i++)
				{
					if(				c == ESC_cmds[i].MatchChar		&&
						con->inter[0] == ESC_cmds[i].Intermediate)
					{
						if(ESC_cmds[i].ServiceClass != BOTH)
						{
							if(!(con->pstat & P_VT340_MODE)  &&  ESC_cmds[i].ServiceClass == VT340)
								break;
						}

						if(con->ordC != 0)
							PageTextOut(con);

						if(!(con->gstat & G_PRINT_CONTROL)  ||
							ESC_cmds[i].TransparentPrint != FALSE)
						{
							con->inESC = ESC_cmds[i].MatchChar;
							ESC_cmds[i].Routine(con, con->args, con->argc);
						}
						break;
					}
				}
				con->inESC = FALSE;
				continue;
			}

			if(con->inCSI != FALSE  &&  (c >= 32  &&  c < 127))
			{
				if(c < '@')
				{
					if(c >= '0'  &&  c <= '9')	/* Parameter.. */
					{
						con->args[con->argc] = con->args[con->argc] * 10;
						con->args[con->argc] += c - '0';
						continue;
					}

					if(c == ';')					/* Parameter-TrennZeichen */
					{
						con->args[++con->argc] = 0;
						if(con->argc > MAXARGS)
							con->argc = MAXARGS;
						continue;
					}

					if(con->icnt < MAXINTER)
					{
						con->inter[con->icnt++] = c;	/* something like '?', etc. */
						con->inter[con->icnt] = NUL;	/* clear next intermediate */
					}
					continue;
				}

				for(i=0; CSI_cmds[i].Routine; i++)
				{
					if(				c == CSI_cmds[i].MatchChar		&&
						con->inter[0] == CSI_cmds[i].Intermediate)
					{
						if(CSI_cmds[i].ServiceClass != BOTH)
						{
							if(!(con->pstat & P_VT340_MODE)  &&  CSI_cmds[i].ServiceClass == VT340)
								break;
						}

						if(con->ordC != 0)
							PageTextOut(con);

						if(!(con->gstat & G_PRINT_CONTROL)  ||
							CSI_cmds[i].TransparentPrint != FALSE)
						{
							con->inCSI = CSI_cmds[i].MatchChar;
							CSI_cmds[i].Routine(con, con->args, con->argc);
						}
						break;
					}
				}
				con->inCSI = FALSE;
				continue;
			}
		}
		else	/* VT-52 */
		{
			c &= ~(0x80);	/* kill possible hi-bit */

			if(con->inESC != FALSE  &&  c >= 32)
			{
				if(!(con->gstat & G_PRINT_CONTROL)  &&
					(c == 'Y'  ||  con->VT52_dca))	/* Direct Cursor Address */
				{
					if(con->VT52_dca == FALSE)
						con->VT52_dca = TRUE;
					else
					{
						con->args[con->argc++] = c;
						con->args[con->argc] = NUL;
						if(con->argc == 2)
						{
							con->args[0]	-= 31;
							con->args[1]	-= 31;
							con->inESC		= 'Y';	/* compatibility rules.. */
							VT340_cursorposition(con, con->args, con->argc);
							VT340_cancel(con);
						}
					}
					continue;
				}
				else	/* regular command */
				{
					for(i=0; VT52_cmds[i].Routine; i++)
					{
						if(c == VT52_cmds[i].MatchChar)
						{
							if(con->ordC != 0)
								PageTextOut(con);

							if(!(con->gstat & G_PRINT_CONTROL)  ||
								VT52_cmds[i].TransparentPrint != FALSE)
							{
								con->inESC = c;
								VT52_cmds[i].Routine(con, con->args, con->argc);
							}
							break;
						}
					}

					con->inESC = FALSE;
				}

				continue;
			}
		}

		if(IsPrintable(c))
		{
			if(con->gstat & G_PRINT_CONTROL  &&  con->printerHandle != NULL)
			{
				if(con->ordC == con->columns)
					PageTextOut(con);

				con->ordText[con->ordC++] = c;
			}
			else
			{
				UWORD lim;

				lim = con->columns;

				if(PageGetLineMode(con) != SCALE_ATTR_NORMAL)
					lim >>= 1;

				if(con->ordC == 0)
					con->ordCol = con->col;
				con->ordText[con->ordC++] = c;
				con->col++;

				if(con->fstat & F_GXGL)
				{
					PageTextOut(con);
					VT340_setlset(con, con->slset);
					con->fstat &= ~F_GXGL;
				}

				if(con->col > lim)
				{
					if(con->ordC != 0)
						PageTextOut(con);

					if(con->tstat & T_WRAP)
						VT340_cursornextline(con, "\001", 0);
					else
						con->col = lim;
				}
			}
		}
		else	/* char is a control-char.. */
		{
			if(con->specialMap[c] != -1)
			{
				if(con->ordC != 0)
					PageTextOut(con);

				SpecialKeys[con->specialMap[c]].Routine(con);
			}
		}
	}


	if(con->ordC != 0)
		PageTextOut(con);

	PageCursorVisible(con);
}


STATIC BOOL VT340_printctrlchr(struct VT340Console *con, UBYTE c)
{
	if(con->gstat & G_PRINT_CONTROL  &&  con->printerHandle != NULL)
	{
		if(!FWrite(con->printerHandle, &c, 1, 1))
		{
			con->gstat &= ~G_PRINT_CONTROL;

			if(con->gstat & G_AUTO_PRINT_INACTIVE)
			{
				con->gstat &= ~G_AUTO_PRINT_INACTIVE;
				con->gstat |=  G_AUTO_PRINT;
			}

			CreateOwnStatusLine(con);
		}
		return(FALSE);
	}
	else
	{
		if(con->gstat & G_AUTO_PRINT  &&  con->printerHandle != NULL)
		{
			if(!FWrite(con->printerHandle, &c, 1, 1))
			{
				con->gstat &= ~(G_AUTO_PRINT | G_AUTO_PRINT_INACTIVE);
				CreateOwnStatusLine(con);
			}
		}
	}
	return(TRUE);
}


STATIC VOID VT340_nul(struct VT340Console *con)
{
	VT340_printctrlchr(con, NUL);
}

STATIC VOID VT340_enq(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, ENQ) == FALSE)
		return;

	if(con->answerBack[0] != '\0')
		con->io->xem_swrite(con->answerBack, -1);
}

STATIC VOID VT340_bel(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, BEL) == FALSE)
		return;

	con->io->xem_tbeep(1, 0);
}

STATIC VOID VT340_bs(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, BS) == FALSE)
		return;

	if(con->col > 1)
	{
		con->col--;
		if(con->pstat & P_DESTRUCTIVEBS)
			PageDeleteChars(con, con->col, 1);
	}
}

STATIC VOID VT340_ht(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, HT) == FALSE)
		return;

	VT340_tabForward(con, "\001", 1);
}

STATIC VOID VT340_lf(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, LF) == FALSE)
		return;

	if(con->ustat & U_NEWLINE)
		VT340_cursornextline(con, "\001", 0);
	else
		VT340_cursordown(con, "\001", 0);
}

STATIC VOID VT340_ff(struct VT340Console *con)
{
	if(con->pstat & P_DESTRUCTIVEFF)
	{
		if(VT340_printctrlchr(con, FF) == FALSE)
			return;

		PageEraseInDisplay(con, 2);
		con->row = con->firstLine;
		con->col = 1;
	}
	else
	{
		if(VT340_printctrlchr(con, LF) == FALSE)
			return;

		VT340_lf(con);
	}
}

STATIC VOID VT340_cr(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, CR) == FALSE)
		return;

	con->col = 1;
}

STATIC VOID VT340_so(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, SO) == FALSE)
		return;

	if(con->pstat & P_ANSI_MODE)
		VT340_setlset(con, 1);
}

STATIC VOID VT340_si(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, SI) == FALSE)
		return;

	if(con->pstat & P_ANSI_MODE)
		VT340_setlset(con, 0);
}

STATIC VOID VT340_xon(struct VT340Console *con)
{
	VT340_printctrlchr(con, XON);
}

STATIC VOID VT340_xof(struct VT340Console *con)
{
	VT340_printctrlchr(con, XOF);
}

STATIC VOID VT340_can(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, CAN) == FALSE)
		return;

	VT340_cancel(con);
}

STATIC VOID VT340_sub(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, SUB) == FALSE)
		return;

	VT340_cancel(con);
}

STATIC VOID VT340_esc(struct VT340Console *con)
{
	VT340_invokeESC(con, NULL, NUL);
}

STATIC VOID VT340_del(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, DEL) == FALSE)
		return;

	if(con->pstat & P_DESTRUCTIVEDEL)
		PageDeleteChars(con, con->col, 1);
}

STATIC VOID VT340_ind(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, IND) == FALSE)
		return;

	VT340_cursordown(con, "\001", 0);
}

STATIC VOID VT340_nel(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, NEL) == FALSE)
		return;

	VT340_cursornextline(con, "\001", 0);
}

STATIC VOID VT340_ssa(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_esa(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_hts(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, HTS) == FALSE)
		return;

	VT340_setTab(con, NULL, NUL);
}

STATIC VOID VT340_ri(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, RI) == FALSE)
		return;

	VT340_cursorup(con, "\001", 0);
}

STATIC VOID VT340_ss2(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, SS2) == FALSE)
		return;

	con->slset = con->lset;
	con->fstat |= F_GXGL;
	VT340_setlset(con, 2);
}

STATIC VOID VT340_ss3(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, SS3) == FALSE)
		return;

	con->slset = con->lset;
	con->fstat |= F_GXGL;
	VT340_setlset(con, 3);
}

STATIC VOID VT340_dcs(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, DCS) == FALSE)
		return;

	VT340_invokeDCS(con, NULL, NUL);
}

STATIC VOID VT340_sts(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_spa(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_epa(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_csi(struct VT340Console *con)
{
	VT340_invokeCSI(con, NULL, 0);
}

STATIC VOID VT340_st(struct VT340Console *con)
{
	;
}

STATIC VOID VT340_osc(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, OSC) == FALSE)
		return;

	VT340_invokePrivate(con, NULL, NUL);
}

STATIC VOID VT340_pm(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, PM) == FALSE)
		return;

	VT340_invokePrivate(con, NULL, NUL);
}

STATIC VOID VT340_apc(struct VT340Console *con)
{
	if(VT340_printctrlchr(con, APC) == FALSE)
		return;

	VT340_invokePrivate(con, NULL, NUL);
}


/* <EOB> */
