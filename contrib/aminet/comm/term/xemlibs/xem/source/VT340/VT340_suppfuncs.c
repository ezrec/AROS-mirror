#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;
IMPORT struct	Library			*DiskfontBase;



              /* Support Funktionen zu VT340 Emulator.. */

STATIC WORD VT340_getmaxframeleft(struct VT340Console *con);
STATIC WORD VT340_getmaxframetop(struct VT340Console *con);
STATIC WORD VT340_getmaxframewidth(struct VT340Console *con);
STATIC WORD VT340_getmaxframeheight(struct VT340Console *con);
/* STATIC VOID SaveFont(struct VT340Console *con); */

VOID VT340_resetcon(struct VT340Console *con, BOOL ris)
{
	VT340_colorModes(con);

	con->ustat |=  (U_CURSOR_VISIBLE);
	con->ustat &= ~(U_INSERT | U_KEYAPP | U_CURAPP);

	con->ustat &= ~(U_HOST_STATUSLINE | U_HOST_STATUSLINE_ACTIVE);
/*	DeleteStatusLine(con, FALSE);*/

	con->tstat &= ~(T_WRAP);

	con->ordC	= 0;
	con->ordCol = 1;
	con->DECSCA	= ERASEABLE;


	con->cursorVisible = FALSE;

	VT340_cancel(con);

	if(ris != FALSE)	/* Reset to Initial State */
	{
		con->identify = con->r_identify;

/**********/

		if(con->r_tstat & T_WRAP)
			con->tstat |= T_WRAP;
		else
			con->tstat &= ~T_WRAP;

/**********/

		if(con->r_ustat & U_CURSOR_VISIBLE)
			con->ustat |= U_CURSOR_VISIBLE;
		else
			con->ustat &= ~U_CURSOR_VISIBLE;

		if(con->r_ustat & U_NEWLINE)
			con->ustat |= U_NEWLINE;
		else
			con->ustat &= ~U_NEWLINE;

		if(con->r_ustat & U_INSERT)
			con->ustat |= U_INSERT;
		else
			con->ustat &= ~U_INSERT;

		if(con->r_ustat & U_BACKSPACE_BS)
			con->ustat |= U_BACKSPACE_BS;
		else
			con->ustat &= ~U_BACKSPACE_BS;

/**********/

		con->pstat = con->r_pstat;		/* private settings */

/**********/

		con->gstat &= ~(G_FULLSCREEN | G_FORMFEED);

/**********/
		VT340_resetstyles(con, FALSE);
		VT340_resetfonts(con);

/*		VT340_checkframe(con);	/* nicht nötig */ */

		VT340_resetscreen(con, TRUE);

		VT340_standardTabs(con);
	}
	else
	{
		con->tstat &= ~(T_USE_REGION);
		con->top = con->firstLine;
		con->bot = con->lastLine;

		VT340_resetstyles(con, FALSE);
		VT340_resetfonts(con);
	}

	con->m_row = con->row;	/* MAIN ACTIVE DISPLAY */
	con->m_col = con->col;

	VT340_savecursor(con, NULL, 0);

	CreateOwnStatusLine(con);
}


VOID VT340_resetstyles(struct VT340Console *con, BOOL colorsOnly)
{
	if(colorsOnly != FALSE  &&  con->attr & ATTR_INVERSE)
	{
		con->bpen = con->foregroundPen;
		con->apen = BACKGROUND_PEN;
	}
	else
	{
		con->apen = con->foregroundPen;
		con->bpen = BACKGROUND_PEN;
	}

	SetAPen(con->rp, con->apen);
	SetBPen(con->rp, con->bpen);


	if(colorsOnly == FALSE)
	{
		con->attr = ATTR_NORMAL;
		SetDrMd(con->rp, JAM2);
		SetSoftStyle(con->rp, FS_NORMAL, SYSTEM_TEXTSTYLES);
	}
}


VOID VT340_resetfonts(struct VT340Console *con)
{
	SetFont(con->rp, con->dcFont);

	con->upss	 = (con->pstat & P_ISO_LATIN1) ? ISO_SUPPLEMENTAL : DEC_SUPPLEMENTAL;
	con->gset[0] = ASCII;
	con->gset[1] = ASCII;
	con->gset[2] = con->upss;
	con->gset[3] = con->upss;
	VT340_setlset(con, 0);	/* ASCII */
	VT340_setrset(con, 2);	/* USER-PREFERRED SUPPLEMENTAL SET */
	con->slset	= con->s_lset = con->lset;
	con->s_rset	= con->rset;
}


VOID CreateLockedWindow(struct VT340Console *con)
{
	if(con->lockedWindow == NULL)
	{
		STATIC struct IntuiText iText = { 1, 0, JAM2, 0, 0, NULL, "xemVT340: keyboard locked..", NULL };
		LONG width;

		width = IntuiTextLength(&iText);
		width += 30;

		if(con->lockedWindow = OpenWindowTags(NULL,
			WA_Width,			width,
			WA_Height,			35,
			WA_Left,				(con->io->xem_window->Width - width) >> 1,
			WA_Top,				(con->io->xem_window->Height- 36) >> 1,
			WA_Activate,		TRUE,
			WA_DragBar,			TRUE,
			WA_DepthGadget,	TRUE,
			WA_CloseGadget,	FALSE,
			WA_RMBTrap,			TRUE,
			WA_CustomScreen,	con->io->xem_window->WScreen,	/* Is this legal? I dunno.. */
			WA_IDCMP,			0,
			WA_Title, 			"",
			TAG_DONE))
		{
			PrintIText(con->lockedWindow->RPort, &iText, 15, 16);
		}
	}
}


VOID DeleteLockedWindow(struct VT340Console *con)
{
	if(con->lockedWindow != NULL)
	{
		CloseWindow(con->lockedWindow);
		con->lockedWindow = NULL;
	}
}

VOID VT340_locked(struct VT340Console *con, BOOL locked)
{
#define LEFT 19

	if(locked != FALSE)
		CreateLockedWindow(con);
	else
		DeleteLockedWindow(con);


	if(con->pstat & P_OWN_STATUSLINE)
	{
		STRPTR text;
		UWORD len;

		if(LEFT > con->columns)
			return;

		SetFont(con->rp, con->topazFont);
/*		SetDrMd(con->rp, JAM2|INVERSVID);*/
		SetSoftStyle(con->rp, FS_NORMAL, SYSTEM_TEXTSTYLES);
		SetAPen(con->rp, BACKGROUND_PEN);
		SetBPen(con->rp, con->foregroundPen);

		text = (locked != FALSE) ? "LOCKED" : "READY ";
		len = strlen(text);

		if(LEFT + len > con->columns)
			len = con->columns - LEFT;

		myMove_Text(con, LEFT, con->statusLine, text, len);

		SetAPen(con->rp, con->apen);
		SetBPen(con->rp, con->bpen);
		SetSoftStyle(con->rp, GetTextStyle(con), SYSTEM_TEXTSTYLES);
/*		SetDrMd(con->rp, JAM2);*/
		SetFont(con->rp, con->dcFont);
	}
}


/* cancels any sequence.. */
VOID VT340_cancel(struct VT340Console *con)
{
	if(con->inDCS  ||  con->inPrivate)
		VT340_locked(con, FALSE);

	con->inESC		= FALSE;
	con->inCSI		= FALSE;
/*	con->inTEK		= FALSE;*/
	con->inDCS		= FALSE;
	con->inPrivate	= FALSE;

	con->argc		= 0;
	con->args[0]	= NUL;
	con->args[1]	= NUL;
	con->icnt		= 0;
	con->inter[0]	= NUL;
	con->lastPRIVc	= NUL;

	con->VT52_dca = FALSE;	/* VT-52 Direct Cursor Address */
}


STATIC WORD VT340_getmaxframeleft(struct VT340Console *con)
{
	return(con->io->xem_window->BorderLeft);
}


STATIC WORD VT340_getmaxframetop(struct VT340Console *con)
{
	return(con->io->xem_window->BorderTop);
}


STATIC WORD VT340_getmaxframewidth(struct VT340Console *con)
{
	return((WORD)(con->io->xem_window->Width - con->io->xem_window->BorderLeft - con->io->xem_window->BorderRight));
}


STATIC WORD VT340_getmaxframeheight(struct VT340Console *con)
{
	return((WORD)(con->io->xem_window->Height - con->io->xem_window->BorderTop - con->io->xem_window->BorderBottom));
}


#define FIRSTLCHAR          32
#define FIRSTRCHAR          (FIRSTLCHAR + 128)
#define CHARCOUNT           (128 - FIRSTLCHAR)



STATIC VOID SaveFont(struct VT340Console *con)
{
/*
	BPTR fh;

	if(fh = Open("ram:file", MODE_NEWFILE))
	{
	struct TextAttr topazAttr = { "topaz.font", 11, FS_NORMAL, FPF_DISKFONT };
		UBYTE *dcp, val=255, cnt1, cnt2, cnt3, buf[12];
		struct TextFont *df;
		UWORD dmod, *loc;


		if((df = OpenFont(&topazAttr)) == NULL)
		{
			if((df = OpenFont(&topazAttr)) == NULL)
				return;
		}
		dmod	= df->tf_Modulo;

		FPrintf(fh, "\n	SECTION 'FontData',data\n	XDEF	_ISOsuppl_Font\n_ISOsuppl_Font:\n\n");


		cnt1 = CHARCOUNT;
		while(cnt1--)
		{
			dcp = (UBYTE *)df->tf_CharData;
			loc = (UWORD *)df->tf_CharLoc;
			loc += (cnt1 + FIRSTRCHAR - 32) << 1;
			dcp += *loc >> 3;

			FPrintf(fh, "; %ld  (%lc)\n", val, val);
			val--;

			for(cnt2=0; cnt2<df->tf_YSize; cnt2++)
			{
				UBYTE cnt4;

				for(cnt3=0, cnt4=df->tf_XSize-1; cnt3<df->tf_XSize; cnt3++, cnt4--)
					buf[cnt4] = (*dcp & (1<<cnt3)) ? '1' : '0';

				FPrintf(fh, "dc.b	%%%s\n", buf);
				
				dcp += dmod;
			}
		}

		if(df != NULL)
			CloseFont(df);

		FPrintf(fh, "\nEND\n");
		Close(fh);
	}
*/
}


	/* G_reg =>> G0 || G1 || G2 || G3 */

VOID VT340_setlset(struct VT340Console *con, UWORD G_reg)
{
	struct TextFont *df;
	UWORD dmod, *loc;
	UBYTE *scp, *dcp, cnt1, cnt2;

	con->lset = G_reg;
	cnt2 = con->gset[G_reg];

	scp	= con->fonts[cnt2];
	df		= con->dcFont;
	dmod	= df->tf_Modulo;

	cnt1 = CHARCOUNT;
	while(cnt1--)
	{
		dcp = (UBYTE *)df->tf_CharData;
		loc = (UWORD *)df->tf_CharLoc;
		loc += (cnt1 + FIRSTLCHAR) << 1;
		dcp += *loc >> 3;

		for(cnt2=0; cnt2<FONT_YSIZE; cnt2++)
		{
			*dcp = *scp;
			scp++;
			dcp += dmod;
		}
	}
}


	/* G_reg =>> G0 || G1 || G2 || G3 */

VOID VT340_setrset(struct VT340Console *con, UWORD G_reg)
{
	struct TextFont *df;
	UWORD dmod, *loc;
	UBYTE *scp, *dcp, cnt1, cnt2;

	con->rset = G_reg;
	cnt2 = con->gset[G_reg];

	scp	= con->fonts[cnt2];
	df		= con->dcFont;
	dmod	= df->tf_Modulo;

	cnt1 = CHARCOUNT;
	while(cnt1--)
	{
		dcp = (UBYTE *)df->tf_CharData;
		loc = (UWORD *)df->tf_CharLoc;
		loc += (cnt1 + FIRSTRCHAR) << 1;
		dcp += *loc >> 3;

		for(cnt2=0; cnt2<FONT_YSIZE; cnt2++)
		{
			*dcp = *scp;
			scp++;
			dcp += dmod;
		}
	}
}


VOID VT340_setset(struct VT340Console *con, UWORD charset, UWORD G_reg)
{
	con->gset[G_reg] = charset;

	if(con->lset == G_reg)
		VT340_setlset(con, G_reg);

	if(con->rset == G_reg)
		VT340_setrset(con, G_reg);
}


UWORD VT340_cset(struct VT340Console *con, UBYTE c)
{
	switch(c)
	{
		case '5':
			return(DEC_SUPPLEMENTAL);

		case '<':
			return(con->upss);

		case '>':
			return(TECHNICAL);

		case '0':
			return(GRAPHICS);

		case 'A':
			return(ISO_SUPPLEMENTAL);

		case 'B':
		default:
			return(ASCII);
	}
}


VOID VT340_getRowsCols(struct VT340Console *con, struct IBox *border, WORD *rows, WORD *cols)
{
	*rows = border->Height >> 3;
	*cols = border->Width >> 3;
}


VOID VT340_resetscreen(struct VT340Console *con, BOOL clearScreen)
{
	WORD max;
	BOOL displayWasInactive;

	VT340_getRowsCols(con, &con->border, &max, &con->columns);

	if(con->isReady != FALSE)
		displayWasInactive = con->displayInactive;
	else
		displayWasInactive = FALSE;

	con->displayInactive = (max < 2  ||  con->columns == 0);

	if(max > MAXROWS)
		max = MAXROWS;

	con->statusLine = max;
	max--;

	if(con->displayInactive != FALSE)	/* help prevent GURU's */
	{
		con->statusLine= 2;
		con->firstLine = 1;
		con->lastLine	= 1;
		con->row			= 1;
		con->col			= 1;
		con->ySize		= FONT_YSIZE;
	}
	else
	{
		if(con->ustat & U_HOST_STATUSLINE_ACTIVE)
		{
			con->firstLine = con->statusLine;
			con->lastLine	= con->statusLine;
			con->row			= con->statusLine;
			con->col			= 1;
			con->ySize		= FONT_YSIZE;
		}
		else		/* main screen is active */
		{
			con->firstLine	= 1;
			if(max < 24)
			{
				con->lastLine	= max;
				con->ySize		= FONT_YSIZE;
			}
			else
			{
				con->lastLine = (con->pstat & P_24LINES) ? 24 : max;
				con->ySize = (con->pstat & P_24LINES  &&  con->pstat & P_ADJUST_LINES)
													? ((max<<3) / con->lastLine) : FONT_YSIZE;
			}
		}
	}

	con->top = con->firstLine;
	con->bot = con->lastLine;

	if(clearScreen != FALSE)
	{
		VT340_cursorhome(con);
		PageEraseInDisplay(con, 2);
	}
	else
	{
		VT340_checkdims(con);
	}

/*
	if(1)
	{
		UWORD x1, y1, x2, y2;

		x1 = con->border.Left;
		y1 = con->border.Top;
		x2 = con->border.Left + con->border.Width;
		y2 = con->border.Top + con->border.Height;
		x1--;
		y1--;
		x2++;
		y2++;
		Move(con->io->xem_window->RPort, x1, y1);
		Draw(con->io->xem_window->RPort, x1, y2);
		Draw(con->io->xem_window->RPort, x2, y2);
		Draw(con->io->xem_window->RPort, x2, y1);
		Draw(con->io->xem_window->RPort, x1, y1);
	}
*/

}


LONG AtoL(STRPTR string)
{
	LONG value=0;

	return((StrToLong(string, &value) > 0) ? value : 0);
}


#define CHARS 256
#define MODULO CHARS

BOOL VT340_createfont(struct VT340Console *con)
{
	IMPORT UWORD CharLoc[];
	struct TextFont *dcFont;
	UBYTE *charData;

	con->dcFont = NULL;
	con->fontRemember = NULL;

	dcFont = AllocRemember(&con->fontRemember, sizeof(struct TextFont), MEMF_CLEAR);
	charData = AllocRemember(&con->fontRemember, CHARS * FONT_YSIZE, MEMF_CLEAR | MEMF_CHIP);

	if(dcFont != NULL  &&  charData != NULL)
	{
		dcFont->tf_Message.mn_Node.ln_Succ	= NULL;
		dcFont->tf_Message.mn_Node.ln_Pred	= NULL;
		dcFont->tf_Message.mn_Node.ln_Type	= NT_FONT;
 		dcFont->tf_Message.mn_Node.ln_Pri	= 0;
		dcFont->tf_Message.mn_Node.ln_Name	= NULL;
		dcFont->tf_Message.mn_ReplyPort		= NULL;
		dcFont->tf_Message.mn_Length			= sizeof(struct TextFont);

		dcFont->tf_YSize		= 8;
		dcFont->tf_XSize		= 8;
		dcFont->tf_Style		= FS_NORMAL;
		dcFont->tf_Flags		= FPF_DESIGNED | FPF_DISKFONT;
		dcFont->tf_Baseline	= 6;
		dcFont->tf_BoldSmear	= 1;
		dcFont->tf_Accessors	= 1;
		dcFont->tf_LoChar		= 0;
		dcFont->tf_HiChar		= 255;
		dcFont->tf_CharData	= (APTR)charData;
		dcFont->tf_Modulo		= MODULO;
		dcFont->tf_CharLoc	= (APTR)CharLoc;
		dcFont->tf_CharSpace	= NULL;
		dcFont->tf_CharKern	= NULL;

		con->dcFont = dcFont;
	}
	else
		FreeRemember(&con->fontRemember, TRUE);

	return((BOOL)(con->dcFont != NULL));
}


BOOL VT340_openfonts(struct VT340Console *con)
{
	STATIC BOOL VT340_createfont(struct VT340Console *con);

	struct TextAttr topazAttr = { "topaz.font", 8, FS_NORMAL, FPF_DESIGNED | FPF_ROMFONT };

	IMPORT UBYTE Graphics_Font[];
	IMPORT UBYTE Technical_Font[];
	IMPORT UBYTE ASCII_Font[];
	IMPORT UBYTE DECsuppl_Font[];
	IMPORT UBYTE ISOsuppl_Font[];

	if(con->topazFont = OpenFont(&topazAttr))
	{
		con->fonts[ASCII]					= ASCII_Font;
		con->fonts[DEC_SUPPLEMENTAL]	= DECsuppl_Font;
		con->fonts[ISO_SUPPLEMENTAL]	= ISOsuppl_Font;
		con->fonts[GRAPHICS]				= Graphics_Font;
		con->fonts[TECHNICAL]			= Technical_Font;

/*		SaveFont(con);*/

		return(VT340_createfont(con));
	}
	else
		return(FALSE);
}


VOID VT340_closefonts(struct VT340Console *con)
{
	if(con->dcFont != NULL)
	{
		FreeRemember(&con->fontRemember, TRUE);
		con->dcFont = NULL;
		con->fontRemember = NULL;
	}

	if(con->topazFont != NULL)
	{
		CloseFont(con->topazFont);
		con->topazFont = NULL;
	}
}


VOID VT340_printinfo(struct VT340Console *con, BPTR handle)
{
	if(handle == NULL)
		return;

	;
}


VOID VT340_printterminator(struct VT340Console *con, BPTR handle)
{
	if(handle == NULL)
	{
		UBYTE c;

		c = (con->gstat & G_FORMFEED) ? FF : LF;
		FWrite(handle, &c, 1, 1);
	}
}


BOOL VT340_openprinterFH(struct VT340Console *con)
{
	if(con->printerHandle == NULL)
	{
		if(con->printerName[0] != '\0')
			con->printerHandle = Open(con->printerName, MODE_NEWFILE);
	}

	return((BOOL)(con->printerHandle != NULL));
}


BOOL VT340_openscreenFH(struct VT340Console *con)
{
	if(con->screenHandle == NULL)
	{
		if(con->screenName[0] != '\0')
		{
			BPTR lock;

			if(lock = Lock(con->screenName, SHARED_LOCK))
			{
				UnLock(lock);
				if(con->screenHandle = Open(con->screenName, MODE_READWRITE))
					Seek(con->screenHandle, 0, OFFSET_END);
			}
			else
				con->screenHandle = Open(con->screenName, MODE_NEWFILE);
		}
	}

	return((BOOL)(con->screenHandle != NULL));
}


VOID VT340_configopt(struct xem_option *opt, UBYTE *title, UBYTE *buff, UWORD length, BOOL type, BOOL value)
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


BOOL VT340_setopt(struct xem_option *opt, UWORD *stat, UWORD val, ULONG changed, ULONG flag)
{
	BOOL chd = FALSE;

	if(changed & (1 << flag))
	{
		chd = TRUE;
		if(!stricmp(opt->xemo_value, "yes") || !stricmp(opt->xemo_value, "on"))
			*stat |= val;
		else
			*stat &= ~val;
	}

	return(chd);
}


VOID VT340_getTopBot(struct VT340Console *con, WORD *top, WORD *bot)
{
	if(con->ustat & U_HOST_STATUSLINE_ACTIVE)
	{
		if(top != NULL)
			*top = con->statusLine;
		if(bot != NULL)
			*bot = con->statusLine;
	}
	else
	{
		if(top != NULL)
			*top = (con->tstat & T_USE_REGION) ? con->top : con->firstLine;
		if(bot != NULL)
			*bot = (con->tstat & T_USE_REGION) ? con->bot : con->lastLine;
	}
}

VOID VT340_checkdims(struct VT340Console *con)
{
	WORD top, bot;

	VT340_getTopBot(con, &top, &bot);

	if(con->row < top)
		con->row = top;

	if(con->row > bot)
		con->row = bot;

/*******/

	if(con->col < 1)
		con->col = 1;

	if(con->col > con->columns)
		con->col = con->columns;
}


VOID VT340_cursorhome(struct VT340Console *con)
{
	con->row = 0;
	con->col = 0;
	VT340_checkdims(con);
}


VOID VT340_standardTabs(struct VT340Console *con)
{
	WORD oldCol;

	oldCol = con->col;

	VT340_clearAllTabs(con);
	for(con->col=9; con->col<con->columns; con->col+=8)
		VT340_setActualTabPos(con);

	con->col = oldCol;
}


BOOL VT340_isTab(struct VT340Console *con)
{
	return((BOOL)(con->tabs[con->col] == 'T'));
}


VOID VT340_setActualTabPos(struct VT340Console *con)
{
	con->tabs[con->col] = 'T';
}


VOID VT340_clearActualTabPos(struct VT340Console *con)
{
	con->tabs[con->col] = 0;
}


VOID VT340_clearAllTabs(struct VT340Console *con)
{
	setmem(con->tabs, con->columns, 0);
}


VOID VT340_colorModes(struct VT340Console *con)
{
	switch(con->io->xem_screendepth)
	{
		case 2:	/* MONO & BOLD */
			con->foregroundPen = 1;
			con->hiLite = TRUE;
			con->mono = TRUE;
		break;

		case 3:	/* COLOR */
			con->hiLite = FALSE;
			con->mono = FALSE;
			con->foregroundPen = (con->pstat & P_ANSI_COLORS) ? 7 : 1;
		break;

		case 4:	/* COLOR & BOLD */
			con->mono = FALSE;
			if(con->pstat & P_ANSI_COLORS)
			{
				con->foregroundPen = 7;
				con->hiLite = TRUE;
			}
			else
			{
				con->foregroundPen = 1;
				con->hiLite = FALSE;
			}
		break;

		default:
/*		case 1:	/* MONO */ */
			con->foregroundPen = 1;
			con->hiLite = FALSE;
			con->mono = TRUE;
		break;

	}
}


VOID VT340_closeprinterFH(struct VT340Console *con)
{
	if(con->printerHandle != NULL)
	{
		Close(con->printerHandle);
		con->printerHandle = NULL;
	}
}

VOID VT340_closescreenFH(struct VT340Console *con)
{
	if(con->screenHandle != NULL)
	{
		Close(con->screenHandle);
		con->screenHandle = NULL;

		if(con->bytesWritten != FALSE)
		{
			SetProtection(con->screenName, FIBF_EXECUTE);
			con->bytesWritten = FALSE;
		}
	}
}



VOID VT340_newframe(struct VT340Console *con, struct IBox *frame)
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
		con->border.Left	= VT340_getmaxframeleft(con);		/* Pixels.. */
		con->border.Top	= VT340_getmaxframetop(con);		/* Pixels.. */
		con->border.Width	= VT340_getmaxframewidth(con);	/* Pixels.. */
		con->border.Height= VT340_getmaxframeheight(con);	/* Pixels.. */
	}
}


ULONG GetTextStyle(struct VT340Console *con)
{
	ULONG textStyles;

	textStyles = FS_NORMAL;

	if(con->attr & ATTR_FAT)
		textStyles |= FSF_BOLD;

	if(con->attr & ATTR_UNDERLINE)
		textStyles |= FSF_UNDERLINED;

	return(textStyles);
}



/* end of source-code */
