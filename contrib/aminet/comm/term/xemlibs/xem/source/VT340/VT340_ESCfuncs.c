#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


                /* ESC Funktionen zu VT340 Emulator.. */


/*
 ESC	1/11	1bh   ^[	Escape, start escape seq, cancel any others
*/
VOID VT340_invokeESC(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_cancel(con);
	con->inESC = TRUE;
}


/*
 ESC [		CSI		Control Sequence Introducer, see list below
*/
VOID VT340_invokeCSI(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_cancel(con);
	con->inCSI = TRUE;
}


/*
 ESC P		DCS
*/
VOID VT340_invokeDCS(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_cancel(con);
	con->inDCS = TRUE;
	VT340_locked(con, TRUE);
}


/*
 ESC ]		OSC
 ESC ^		PM
 ESC _		APC
*/
VOID VT340_invokePrivate(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_cancel(con);
	con->inPrivate = TRUE;
	VT340_locked(con, TRUE);
}


/*
 ESC /		ST
*/
VOID VT340_stringterm(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_cancel(con);
}


/*
 ESC ( <ident>	SCS		Designates 94 byte character set <ident> to G0
 ESC ) <ident>	SCS		Designates 94 byte character set <ident> to G1
 ESC * <ident>	SCS		Designates 94 byte character set <ident> to G2
 ESC + <ident>	SCS		Designates 94 byte character set <ident> to G3
 ESC - <ident>	SCS		Designates 96 byte character set <ident> to G1
 ESC . <ident>	SCS		Designates 96 byte character set <ident> to G2
 ESC / <ident>	SCS		Designates 96 byte character set <ident> to G3
	     <ident>    size	character set
		A	 94	UK-ASCII (ASCII, sharp sign 2/3 replaced by
				 Sterling sign), available only when a VT102
		A	 96	ISO Latin-1 (default in G2, G3)
		B	 94	ASCII (default in G0, G1)
		0	 94	DEC Special Graphics (line drawing)
		1	 94/96	Kermit, ALT-ROM
		2	 94	DEC Special Graphics (line drawing)
		%5	 94	DEC Supplemental Graphics
		<	 94/96	User Preferred Supplemental Set*
		>	 94	DEC Technical set (from VT340's)
 * VT300 terminals give choice of ISO Latin-1 (96) or DEC Supplemental
 Graphics (94) determined by a Setup menu selection or host command. MS Kermit
 has DEC Supplemental Graphics as the startup UPSS character set.
 
 If the size of the character set does not match the <ident> nothing happens.
 Startup defaults are ASCII in G0 and G1, ISO Latin-1 in G2 and G3, GL points
 to G0, GL points to G2. Activating DEC National Replacement Charactes maps
 the NRC set selected by SET TERMINAL CHARACTER-SET <country> into G0..G3.
 Single and Locking shifts, SS2, SS3, LS0 (SI), LS1 (SO), LS1R, LS2, LS2R,
 LS3, LS3R determine which set is mapped to the GLeft or GRight area.
*/
VOID VT340_setG0(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->inESC == 'A'  &&  !(con->pstat & P_VT340_MODE))
		con->inESC = 'B';
	VT340_setset(con, VT340_cset(con, con->inESC), 0);
}
VOID VT340_setG1(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->inESC == 'A'  &&  !(con->pstat & P_VT340_MODE))
		con->inESC = 'B';
	VT340_setset(con, VT340_cset(con, con->inESC), 1);
}
VOID VT340_setG2(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setset(con, VT340_cset(con, con->inESC), 2);
}
VOID VT340_setG3(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setset(con, VT340_cset(con, con->inESC), 3);
}


/*
 SI/LS0	0/15	0fh   ^O	Map character set in G0 to GL, locking shift
*/
VOID VT340_setG0_GL(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setlset(con, 0);
}


/*
 SO/LS1	0/14	0eh   ^N	Map character set in G1 to GL, locking shift
*/
VOID VT340_setG1_GL(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setlset(con, 1);
}


/*
 ESC ~		LS1R		Map character set in G1 to GR, locking shift
*/
VOID VT340_setG1_GR(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setrset(con, 1);
}


/*
 ESC n		LS2		Map character set in G2 to GL, locking shift
*/
VOID VT340_setG2_GL(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setlset(con, 2);
}


/*
 ESC }		LS2R		Map character set in G2 to GR, locking shift
*/
VOID VT340_setG2_GR(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setrset(con, 2);
}


/*
 ESC o		LS3		Map character set in G3 to GL, locking shift
*/
VOID VT340_setG3_GL(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setlset(con, 3);
}


/*
 ESC |		LS3R		Map character set in G3 to GR, locking shift
*/
VOID VT340_setG3_GR(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_setrset(con, 3);
}


/*
 ESC N		SS2		Single Shift 2, map G2 to GL for next char only
*/
VOID VT340_setG2_GL_s(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->slset = con->lset;
	con->fstat |= F_GXGL;
	VT340_setlset(con, 2);
}


/*
 ESC O		SS3		Single Shift 3, map G3 to GL for next char only
*/
VOID VT340_setG3_GL_s(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	con->slset = con->lset;
	con->fstat |= F_GXGL;
	VT340_setlset(con, 3);
}


VOID VT340_screenaligntest(struct VT340Console *con)
{
	UWORD i, j;
	WORD top, bot;

	PageCursorInvisible(con);

	VT340_writecon(con, "›H›J", -1);

	VT340_getTopBot(con, &top, &bot);
	for(i=top; i<bot; i++)
	{
		for(j=0; j<con->columns; j++)
			VT340_writecon(con, "E", 1);

		VT340_writecon(con, "\r\n", 2);
	}

	for(j=0; j<con->columns; j++)
		VT340_writecon(con, "E", 1);

	PageCursorVisible(con);

	VT340_cursorhome(con);
}


/*
 ESC H          HTS		Set one horizontal tab at current position
*/
VOID VT340_setTab(struct VT340Console *con, UBYTE *args, ULONG argc)
{
/*	args && argc sind NULL..!! */

	VT340_setActualTabPos(con);
}


/*
 ESC 7		DECSC		Save cursor position, graphic attributes, contents of
				char sets used as GL and GR, origin mode (DECOM), SS2/SS3 shifts.
*/
VOID VT340_savecursor(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD cnt;

/*	args && argc sind NULL..!! */

	con->s_row		= con->row;
	con->s_col		= con->col;
	con->s_attr		= con->attr;
	con->s_apen		= con->apen;
	con->s_bpen		= con->bpen;
	con->s_tstat	= con->tstat;

	for(cnt=0; cnt<SETS; cnt++)
		con->s_gset[cnt] = con->gset[cnt];
	con->s_lset = con->lset;
	con->s_rset = con->rset;
}


/*
 ESC 8		DECRC		Restore cursor from previously saved position
				and information listed for DECSC
*/
VOID VT340_restorecursor(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UWORD cnt;

	con->row	 	= con->s_row;
	con->col		= con->s_col;
	con->attr	= con->s_attr;
	con->apen	= con->s_apen;
	con->bpen	= con->s_bpen;

	for(cnt=0; cnt<SETS; cnt++)
		con->gset[cnt] = con->s_gset[cnt];
	VT340_setlset(con,con->s_lset);
	VT340_setrset(con,con->s_rset);

	con->tstat = con->s_tstat;

	SetAPen(con->rp, con->apen);
	SetBPen(con->rp, con->bpen);

	SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);
}


VOID VT340_hardreset(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	VT340_resetcon(con, TRUE);
}


/*
 ESC Z		DECID		Identify terminal. Responses are
				VT340: CSI ? 63; 1; 2; 8; 9 c
				VT102: CSI ? 6 c
				VT52:  ESC / Z
				Heath-19: ESC / K

 VT340 Report requests and responses
 
 Request CSI c			primary device attributes (terminal ident)
 Responses
	CSI ? 63; 1; 2; 8; 9 c		VT340 (132 col, printer, UDK, NRC)
	CSI ? 6 c			VT102
	ESC / Z				VT52, submode of VT340/VT102
	ESC / K				VT52, when in Heath-19 mode
 
 Request  CSI > c		secondary device attributes
 Response CSI > 24; 0; 0; 0 c		VT340, firmware version 0.0 (Kermit)
*/
VOID VT340_requestterminal(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	UBYTE *answer;

	if(con->inter[0] == '>')
		answer = "\033[>1;10;0c";				/* secondary attributes */
	else
	{
		switch(con->identify)
		{
			case IDENT_VT100:
				answer = "\033[?1;2c";
			break;

			case IDENT_VT101:
				answer = "\033[?1;0c";
			break;

			case IDENT_VT102:
				answer = "\033[?6c";
			break;

			case IDENT_VT220:
				answer = "\033[?62;1;2;6;7;8;9c";
			break;

			case IDENT_VT340:
				answer = "\033[?63;1;2;8;9c";
			break;

		}
	}

	con->io->xem_swrite(answer, -1);
}


/*
 ESC =		DECKPAM		Enter numeric keypad application mode
 ESC >		DECKNPNM		Enter numeric keypad numeric mode
*/
VOID VT340_setnumpad(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->inESC == '=')
		con->ustat |= U_KEYAPP; 
	else
		con->ustat &= ~U_KEYAPP; 
}


/*
 ESC sp F	S7C1T		Disable output of 8-bit controls, use 7-bit
 ESC sp G	S8C1T		Enable output of 8-bit control codes
*/
VOID VT340_setconvert(struct VT340Console *con, UBYTE *args, ULONG argc)
{
	if(con->pstat & P_VT340_MODE)
	{
		if(con->inESC == 'F')
			con->pstat |= P_CONVERT;
		else
			con->pstat &= ~P_CONVERT;

		CreateOwnStatusLine(con);
	}
}





/* end of source-code */
