#include "ownincs/BBSconsole.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;


              /* Support functions for the BBS emulator.. */

STATIC WORD BBS_getmaxframeleft(struct BBSConsole *con);
STATIC WORD BBS_getmaxframetop(struct BBSConsole *con);
STATIC WORD BBS_getmaxframewidth(struct BBSConsole *con);
STATIC WORD BBS_getmaxframeheight(struct BBSConsole *con);


VOID BBS_reset(struct BBSConsole *con)
{
	UWORD i;
/*	WORD x1,y1,x2,y2;*/

	BBS_cancel(con);

	BBS_erasescreen(con);

/*
	x1 = con->border.Left;
	y1 = con->border.Top;
	x2 = con->border.Left + con->border.Width;
	y2 = con->border.Top + con->border.Height;
	Move(con->io->xem_window->RPort, x1, y1);
	Draw(con->io->xem_window->RPort, x1, y2);
	Draw(con->io->xem_window->RPort, x2, y2);
	Draw(con->io->xem_window->RPort, x2, y1);
	Draw(con->io->xem_window->RPort, x1, y1);
*/


	BBS_checkframe(con);

	con->cursorVisible = FALSE;

	con->row = 1;
	con->col = 1;
	BBS_resetstyles(con, FALSE);

/* Text-Puffer Argumente resetten.. */
	con->ordcol = 1;
	con->ordc	= 0;
	con->shift	= 0;
	con->s_shift= 0;


	con->stat = con->r_stat;	/* restore user settings */


/* Tabulatoren setzen.. */
	setmem(con->tabs, con->columns, 0);
	for(i=9; i<con->columns; i+=8)
		con->tabs[i] = 1;


	SetDrMd(con->rp, JAM2);
	SetFont(con->rp, con->font);
	SetAPen(con->rp, con->apen);
	SetBPen(con->rp, con->bpen);
	SetSoftStyle(con->rp, FS_NORMAL, SYSTEM_TEXTSTYLES);

	BBS_savecursor(con, NULL, NUL);
}


VOID BBS_resetstyles(struct BBSConsole *con, BOOL colorsOnly)
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


VOID BBS_newframe(struct BBSConsole *con, struct IBox *frame)
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
		con->border.Left	= BBS_getmaxframeleft(con);	/* Pixels.. */
		con->border.Top	= BBS_getmaxframetop(con);		/* Pixels.. */
		con->border.Width	= BBS_getmaxframewidth(con);	/* Pixels.. */
		con->border.Height= BBS_getmaxframeheight(con);	/* Pixels.. */
	}
}


VOID BBS_checkframe(struct BBSConsole *con)
{
	UWORD lines, columns;

	lines	 = con->border.Height / YSIZE;
	columns= con->border.Width  / XSIZE;

	con->displayInactive = (lines == 0  ||  columns == 0);

	if(con->columns != columns  ||  con->lines != lines)
	{
		con->columns= columns;
		con->lines	= lines;

		if(con->row > lines  ||  con->col > columns)
		{
			con->row = 1;
			con->col = 1;
			BBS_clearwholeframe(con);
		}
	}
}


VOID BBS_internalsettings(struct BBSConsole *con)
{
	/* some good(?) settings */

	con->r_stat &= ~NEWLINE;
	con->r_stat |= Y_SCROLLCURSOR;
	con->r_stat |= X_SCROLLCURSOR;
	con->r_stat |= BLINK_CURSOR;
	con->r_stat &= ~CURSOR_UNDERSCORE;
}


enum { ARG_HS=0,ARG_VS,ARG_NL,ARG_BLINK,ARG_UNDER,ARGS };

BOOL BBS_parseoptions(struct BBSConsole *con, STRPTR optionsBuffer)
{
	STATIC UBYTE *PreferencesTemplate = "HSCROLL,VSCROLL,NEWLINE,BLINK,UNDERSCORE";
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

					if(ArgArray[ARG_HS] != NULL)
					{
						if(!strnicmp(ArgArray[ARG_HS], "NO", 2))
							con->r_stat &= ~X_SCROLLCURSOR;
						else
							con->r_stat |= X_SCROLLCURSOR;
					}

					if(ArgArray[ARG_VS] != NULL)
					{
						if(!strnicmp(ArgArray[ARG_VS], "NO", 2))
							con->r_stat &= ~Y_SCROLLCURSOR;
						else
							con->r_stat |= Y_SCROLLCURSOR;
					}

					if(ArgArray[ARG_NL] != NULL)
					{
						if(!strnicmp(ArgArray[ARG_NL], "NO", 2))
							con->r_stat &= ~NEWLINE;
						else
							con->r_stat |= NEWLINE;
					}

					if(ArgArray[ARG_BLINK] != NULL)
					{
						if(!strnicmp(ArgArray[ARG_BLINK], "NO", 2))
							con->r_stat &= ~BLINK_CURSOR;
						else
							con->r_stat |= BLINK_CURSOR;
					}

					if(ArgArray[ARG_UNDER] != NULL)
					{
						if(!strnicmp(ArgArray[ARG_UNDER], "NO", 2))
							con->r_stat &= ~CURSOR_UNDERSCORE;
						else
							con->r_stat |= CURSOR_UNDERSCORE;
					}

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


/* cancels a ANSI sequence.. */
VOID BBS_cancel(struct BBSConsole *con)
{
	con->inESC = FALSE;
	con->inCSI = FALSE;

	con->argc		= 0;
	con->args[0]	= NUL;
	con->args[1]	= NUL;
	con->icnt		= 0;
	con->inter[0]	= NUL;
}


VOID __saveds BBS_cursorVisible(struct BBSConsole *con)
{
	if(con->cursorVisible == FALSE)
		BBS_cursorFlip(con);
}


VOID __saveds BBS_cursorInvisible(struct BBSConsole *con)
{
	if(con->cursorVisible != FALSE)
		BBS_cursorFlip(con);
}


VOID __saveds BBS_cursorFlip(struct BBSConsole *con)
{
	UBYTE offset;

	offset = (con->stat & CURSOR_UNDERSCORE) ? BOTLINE : 0;

	con->cursorVisible = (con->cursorVisible != FALSE) ? FALSE : TRUE;

	if(con->col > con->columns)
		con->col = con->columns;

	SetDrMd(con->rp, COMPLEMENT);
	myRectFill(con, (con->col - 1) * XSIZE, ((con->row - 1) * YSIZE) + offset, (con->col * XSIZE) - 1, (con->row * YSIZE) - 1, -1);
	SetDrMd(con->rp, JAM2);
}


VOID BBS_textout(struct BBSConsole *con)
{
	myMove_Text(con, con->ordcol, con->row, con->ordtext, con->ordc);
	con->ordc = 0;
}


VOID BBS_erasechars(struct BBSConsole *con, UWORD i)
{
	WORD col;

	col = con->col + i - 1;
	if(col > con->columns)
		col = con->columns;

	myRectFill(con, (con->col - 1) * XSIZE, (con->row - 1) * YSIZE, (col * XSIZE)-1, (con->row * YSIZE)-1, BACKGROUND_PEN);
}


VOID BBS_eraselines(struct BBSConsole *con, UWORD i)
{
	WORD row;

	row = con->row + i;
	if(row > con->lines)
		row = con->lines;

	myRectFill(con, 0, (con->row - 1) * YSIZE, con->border.Width-1, (row * YSIZE)-1, BACKGROUND_PEN);
}


VOID BBS_eraselinebeg(struct BBSConsole *con)
{
	if(con->col > 1)
		myRectFill(con, 0, (con->row - 1) * YSIZE, (con->col * XSIZE)-1, (con->row * YSIZE)-1, BACKGROUND_PEN);
}


VOID BBS_erasescrbeg(struct BBSConsole *con)
{
	UWORD cnt = con->row - 1;

	if(con->row > 1)
		myRectFill(con, 0, 0, con->border.Width-1, (cnt * YSIZE)-1, BACKGROUND_PEN);
	BBS_eraselinebeg(con);
}


/* clears only the active frame */
VOID BBS_erasescreen(struct BBSConsole *con)
{
	myRectFill(con, 0, 0, con->border.Width-1, con->border.Height-1, BACKGROUND_PEN);
	con->row = 1;
	con->col = 1;
}


/* clears whole the frame */
VOID BBS_clearwholeframe(struct BBSConsole *con)
{
	mySetRast(con);
}


STATIC WORD BBS_getmaxframeleft(struct BBSConsole *con)
{
	return(con->io->xem_window->BorderLeft);
}


STATIC WORD BBS_getmaxframetop(struct BBSConsole *con)
{
	return(con->io->xem_window->BorderTop);
}


STATIC WORD BBS_getmaxframewidth(struct BBSConsole *con)
{
	return((WORD)(con->io->xem_window->Width - con->io->xem_window->BorderLeft - con->io->xem_window->BorderRight));
}


STATIC WORD BBS_getmaxframeheight(struct BBSConsole *con)
{
	return((WORD)(con->io->xem_window->Height - con->io->xem_window->BorderTop - con->io->xem_window->BorderBottom));
}








STATIC VOID DeleteTimer(struct MsgPort **timerPort, struct timerequest **doTime)
{
	if(*doTime != NULL  &&  (*doTime)->tr_node.io_Device != NULL)
	{
		CloseDevice((struct IORequest *)*doTime);
		*doTime = NULL;
	}

	if(*timerPort != NULL)
	{
		DeletePort(*timerPort);
		*timerPort = NULL;
	}

	if(*doTime != NULL)
	{
		DeleteIORequest((struct IORequest *)*doTime);
		*doTime = NULL;
	}
}


STATIC BOOL CreateTimer(struct MsgPort **timerPort, struct timerequest **doTime)
{
	if(*timerPort = CreatePort(NULL, 0))
	{
		if(*doTime = (struct timerequest *)CreateIORequest(*timerPort, sizeof(struct timerequest)))
		{
			if(!OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)*doTime, 0))
			{
				(*doTime)->tr_node.io_Command = TR_ADDREQUEST;
				(*doTime)->tr_node.io_Flags	= 0;
				(*doTime)->tr_node.io_Error	= 0;

				return(TRUE);
			}
		}
	}

	DeleteTimer(timerPort, doTime);

	return(FALSE);
}


VOID ASyncTimer(struct timerequest *doTime, ULONG ticks, BOOL *requestPending)
{
	doTime->tr_time.tv_secs		= ticks / 50;
	doTime->tr_time.tv_micro	= (ticks % 50) * 20000;
	doTime->tr_node.io_Command	= TR_ADDREQUEST;

	SendIO((struct IORequest *)doTime);
	*requestPending = TRUE;
}


BOOL CreateCursorServer(struct BBSConsole *con)
{
	if(con == NULL  ||  !(con->stat & BLINK_CURSOR)  ||  con->isReady == FALSE)
		return(FALSE);

	if(con->timerPort == NULL  &&  con->doTime == NULL)
	{
		if(CreateTimer(&(con->timerPort), &(con->doTime)) != FALSE)
		{
			*con->io->xem_signal |= (1 << con->timerPort->mp_SigBit);

			con->cursorVisible = FALSE;
			con->requestPending= FALSE;

			ASyncTimer(con->doTime, CURSOR_OFF, &con->requestPending);
		}
	}

	return(FALSE);
}


BOOL DeleteCursorServer(struct BBSConsole *con)
{
	if(con == NULL)
		return(FALSE);

	if(con->requestPending != FALSE)
	{
		AbortIO((struct IORequest *)con->doTime);
		WaitIO((struct IORequest *)con->doTime);
		con->requestPending = FALSE;
	}

	DeleteTimer(&(con->timerPort), &(con->doTime));

	return(TRUE);
}




/*
**#include <hardware/intbits.h>
**VOID __interrupt __saveds __asm CursorServer(register __a1 APTR data)
**{
**	struct BBSConsole *con = (struct BBSConsole *)data;
**
**	con->blinkCounter++;
**
**	if(con->blinkCounter == con->cursorOn)
**	{
**		if(con->cursorBlocked == FALSE)
**			Signal(con->libTask, 1 << con->libSignal);	/* flip on */
**	}
**
**	if(con->blinkCounter == (con->cursorOn + con->cursorOff))
**	{
**		if(con->cursorBlocked == FALSE)
**		{
**			Signal(con->libTask, 1 << con->libSignal);	/* flip off */
**			con->blinkCounter = 0;	/* reset counter */
**		}
**	}
**}
**
**
**BOOL BBS_CreateCursorServer(struct BBSConsole *con)
**{
**	if(con == NULL  ||  !(con->stat & BLINK_CURSOR)  ||  con->isReady == FALSE)
**		return(FALSE);
**
**	con->blinkCounter = 0;
**
**	if(con->interruptStarted == FALSE)
**	{
**		if((con->libSignal = AllocSignal(-1)) > 0)
**		{
**			con->libTask = FindTask(NULL);
**			*con->io->xem_signal |= (1 << con->libSignal);
**
**			con->cursorHandler.is_Node.ln_Name = "Blinki";
**			con->cursorHandler.is_Node.ln_Type = NT_INTERRUPT;
**			con->cursorHandler.is_Code	= CursorServer;
**			con->cursorHandler.is_Data	= (APTR)con;
**			AddIntServer(INTB_VERTB, &con->cursorHandler);
**			con->interruptStarted = TRUE;
**		}
**	}
**
**	return(con->interruptStarted);
**}
**
**
**BOOL BBS_DeleteCursorServer(struct BBSConsole *con)
**{
**	if(con == NULL)
**		return(FALSE);
**
**	if(con->interruptStarted != FALSE)
**	{
**		RemIntServer(INTB_VERTB, &con->cursorHandler);
**		con->interruptStarted = FALSE;
**
**		*con->io->xem_signal &= ~(1 << con->libSignal);
**		SetSignal(0, (1 << con->libSignal));
**
**		if(con->libSignal > 0)
**			FreeSignal(con->libSignal);
**
**		con->libSignal	= 0;
**		con->libTask	= NULL;
**
**		return(TRUE);
**	}
**	return(FALSE);
**}
*/


/* end of source-code */
