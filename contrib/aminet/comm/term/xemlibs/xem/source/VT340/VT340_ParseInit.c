#include "ownincs/VT340console.h"


/* allowed globals.. */
IMPORT struct	ExecBase			*SysBase;
IMPORT struct	DosLibrary		*DOSBase;
IMPORT struct	GfxBase			*GfxBase;
IMPORT struct	IntuitionBase	*IntuitionBase;

/* #define SETTINGS "ENV:vt340/vt340.setup" */




/**********************************************************************/


STATIC VOID VT340_load_setclass(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL)	/* VT340 - 7Bit */
	{
		con->r_pstat |= (P_ANSI_MODE | P_VT340_MODE | P_CONVERT);
		return;
	}


	if(!strnicmp(p, "VT52", 4))
	{
		con->r_pstat |= (P_VT340_MODE | P_CONVERT);
		con->r_pstat &= ~P_ANSI_MODE;
	}
	else
	{
		if(!strnicmp(p, "VT102", 5))
		{
			con->r_pstat |=  (P_ANSI_MODE | P_CONVERT);
			con->r_pstat &= ~P_VT340_MODE;
		}
		else
		{
			if(!strnicmp(p, "VT340_7", 7))
				con->r_pstat |=  (P_ANSI_MODE | P_VT340_MODE | P_CONVERT);
			else
			{
				con->r_pstat |=  (P_ANSI_MODE | P_VT340_MODE);
				con->r_pstat &= ~P_CONVERT;
			}
		}
	}
}


STATIC VOID VT340_load_setid(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL)
	{
		con->r_identify = IDENT_VT340;
		return;
	}


	if(!strnicmp(p, "VT100", 5))
		con->r_identify = IDENT_VT100;
	else
	{
		if(!strnicmp(p, "VT101", 5))
			con->r_identify = IDENT_VT101;
		else
		{
			if(!strnicmp(p, "VT102", 5))
				con->r_identify = IDENT_VT102;
			else
			{
				if(!strnicmp(p, "VT220", 5))
					con->r_identify = IDENT_VT220;
				else
					con->r_identify = IDENT_VT340;
			}
		}
	}
}


STATIC VOID VT340_load_newlinemode(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_ustat |= U_NEWLINE;
	else
		con->r_ustat &= ~U_NEWLINE;
}


STATIC VOID VT340_load_insertmode(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_ustat |= U_INSERT;
	else
		con->r_ustat &= ~U_INSERT;
}


STATIC VOID VT340_load_wrapmode(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_tstat |= T_WRAP;
	else
		con->r_tstat &= ~T_WRAP;
}


/*VOID KPrintF(UBYTE *, ...);*/


STATIC VOID VT340_load_cursorvisible(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "OFF"))
		con->r_ustat &= ~U_CURSOR_VISIBLE;
	else
		con->r_ustat |= U_CURSOR_VISIBLE;
}


STATIC VOID VT340_load_cursorblinking(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_CURSOR_BLINKING;
	else
		con->r_pstat &= ~P_CURSOR_BLINKING;
}


STATIC VOID VT340_load_cursorunderscore(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_CURSOR_UNDERSCORE;
	else
		con->r_pstat &= ~P_CURSOR_UNDERSCORE;
}


STATIC VOID VT340_load_smoothscroll(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_SMOOTH_SCROLL;
	else
		con->r_pstat &= ~P_SMOOTH_SCROLL;
}


STATIC VOID VT340_load_quickrend(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_QUICK_REND;
	else
		con->r_pstat &= ~P_QUICK_REND;
}


STATIC VOID VT340_load_use24lines(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "OFF"))
		con->r_pstat &= ~P_24LINES;
	else
		con->r_pstat |= P_24LINES;
}


STATIC VOID VT340_load_adjustlines(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_ADJUST_LINES;
	else
		con->r_pstat &= ~P_ADJUST_LINES;
}


STATIC VOID VT340_load_destructivebs(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEBS;
	else
		con->r_pstat &= ~P_DESTRUCTIVEBS;
}


STATIC VOID VT340_load_destructiveff(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEFF;
	else
		con->r_pstat &= ~P_DESTRUCTIVEFF;
}


STATIC VOID VT340_load_destructivedel(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_DESTRUCTIVEDEL;
	else
		con->r_pstat &= ~P_DESTRUCTIVEDEL;
}


STATIC VOID VT340_load_swapbsdel(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "OFF"))
		con->r_ustat &= ~U_BACKSPACE_BS;
	else
		con->r_ustat |= U_BACKSPACE_BS;
}


STATIC VOID VT340_load_useisolatin1(struct VT340Console *con, UBYTE *p)
{
	if(p != NULL  &&  !stricmp(p, "ON"))
		con->r_pstat |= P_ISO_LATIN1;
	else
		con->r_pstat &= ~P_ISO_LATIN1;
}


STATIC VOID VT340_load_ansicolors(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL  ||  !stricmp(p, "OFF"))
		con->r_pstat &= ~P_ANSI_COLORS;
	else
		con->r_pstat |= P_ANSI_COLORS;
}


STATIC VOID VT340_load_savescreento(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL)
		p = DEFAULT_SCREENNAME;

	strcpy(con->screenName, p);
}


STATIC VOID VT340_load_printername(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL)
		p = DEFAULT_PRINTERNAME;

	strcpy(con->printerName, p);
}


STATIC VOID VT340_load_answerbackmsg(struct VT340Console *con, UBYTE *p)
{
	if(p == NULL)
		p = "\0";

	strcpy(con->answerBack, p);
}


STATIC VOID VT340_load_titlebar(struct VT340Console *con, UBYTE *p)
{
	if((con->ustat & U_HOST_STATUSLINE) == FALSE)
	{
		if(p != NULL  &&  !stricmp(p, "ON"))
			con->r_pstat |= P_OWN_STATUSLINE;
		else
			con->r_pstat &= ~P_OWN_STATUSLINE;
	}
}


/**********************************************************************/


STATIC STRPTR VT340_save_setclass(struct VT340Console *con)
{
	if(!(con->pstat & P_ANSI_MODE))
		return("VT52");


	if(!(con->pstat & P_VT340_MODE))
		return("VT102");


	if(!(con->pstat & P_CONVERT))
		return("VT340_8");


	return("VT340_7");
}


STATIC STRPTR VT340_save_setid(struct VT340Console *con)
{
	if(con->identify == IDENT_VT100)
		return("VT100");


	if(con->identify == IDENT_VT101)
		return("VT101");


	if(con->identify == IDENT_VT102)
		return("VT102");


	if(con->identify == IDENT_VT220)
		return("VT220");


	return("VT340");
}


STATIC STRPTR VT340_save_newlinemode(struct VT340Console *con)
{
	return((con->ustat & U_NEWLINE) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_insertmode(struct VT340Console *con)
{
	return((con->ustat & U_INSERT) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_wrapmode(struct VT340Console *con)
{
	return((con->tstat & T_WRAP) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_cursorvisible(struct VT340Console *con)
{
	return((con->ustat & U_CURSOR_VISIBLE) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_cursorblinking(struct VT340Console *con)
{
	return((con->pstat & P_CURSOR_BLINKING) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_cursorunderscore(struct VT340Console *con)
{
	return((con->pstat & P_CURSOR_UNDERSCORE) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_smoothscroll(struct VT340Console *con)
{
	return((con->pstat & P_SMOOTH_SCROLL) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_quickrend(struct VT340Console *con)
{
	return((con->pstat & P_QUICK_REND) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_use24lines(struct VT340Console *con)
{
	return((con->pstat & P_24LINES) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_adjustlines(struct VT340Console *con)
{
	return((con->pstat & P_ADJUST_LINES) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_destructivebs(struct VT340Console *con)
{
	return((con->pstat & P_DESTRUCTIVEBS) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_destructiveff(struct VT340Console *con)
{
	return((con->pstat & P_DESTRUCTIVEFF) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_destructivedel(struct VT340Console *con)
{
	return((con->pstat & P_DESTRUCTIVEDEL) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_swapbsdel(struct VT340Console *con)
{
	return((con->ustat & U_BACKSPACE_BS) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_useisolatin1(struct VT340Console *con)
{
	return((con->pstat & P_ISO_LATIN1) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_ansicolors(struct VT340Console *con)
{
	return((con->pstat & P_ANSI_COLORS) ? "ON" : "OFF");
}


STATIC STRPTR VT340_save_savescreento(struct VT340Console *con)
{
	return(con->screenName);
}


STATIC STRPTR VT340_save_printername(struct VT340Console *con)
{
	return(con->printerName);
}


STATIC STRPTR VT340_save_answerbackmsg(struct VT340Console *con)
{
	return(con->answerBack);
}


STATIC STRPTR VT340_save_titlebar(struct VT340Console *con)
{
	if((con->ustat & U_HOST_STATUSLINE) == FALSE)
		return((con->pstat & P_OWN_STATUSLINE) ? "ON" : "OFF");
	else
		return("OFF");
}


/**********************************************************************/


STATIC LONG GetFileSize(UBYTE *file)
{
	struct FileInfoBlock	*info;
	BPTR lock;
	LONG size=0;

	if(info = (struct FileInfoBlock *)AllocMem(sizeof(struct FileInfoBlock),MEMF_PUBLIC))
	{
		if(lock = Lock(file, ACCESS_READ))
		{
			if(Examine(lock, info))
				size = info->fib_Size;

			UnLock(lock);
		}
		FreeMem(info, sizeof(struct FileInfoBlock));
	}
	return(size);
}


STATIC VOID PresetCommand(struct VT340Console *con, VOID (*func)(struct VT340Console *, UBYTE *))
{
	func(con, NULL);
}


STATIC VOID LoadCommand(struct VT340Console *con, UBYTE *buffer, UBYTE *command, VOID (*func)(struct VT340Console *, UBYTE *))
{
	UBYTE *ptr;

	if(ptr = strstr(buffer, command))
	{
		if(ptr = strchr(ptr, 39))
		{
			UBYTE *ptr1;

			ptr++;
			if(ptr1 = strchr(ptr, 39))
			{
				*ptr1 =  0;
				func(con, ptr);
				*ptr1 = 39;
			}
		}
	}
}


STATIC VOID SaveCommand(struct VT340Console *con, BPTR fh, UBYTE *command, STRPTR (*func)(struct VT340Console *))
{
	FPrintf(fh, "%s\t'%s'\n", command, func(con));
}


BOOL VT340_ParseInit(struct VT340Console *con, STRPTR prefsFile, ULONG mode)
{
	if(prefsFile == NULL  ||  prefsFile[0] == '\0'  ||  mode == XEM_PREFS_RESET)
	{
		PresetCommand(con, VT340_load_setclass);			/* preset.. */
		PresetCommand(con, VT340_load_setid);
		PresetCommand(con, VT340_load_newlinemode);
		PresetCommand(con, VT340_load_insertmode);
		PresetCommand(con, VT340_load_wrapmode);
		PresetCommand(con, VT340_load_cursorvisible);
		PresetCommand(con, VT340_load_smoothscroll);
		PresetCommand(con, VT340_load_quickrend);
		PresetCommand(con, VT340_load_use24lines);
		PresetCommand(con, VT340_load_adjustlines);
		PresetCommand(con, VT340_load_destructivebs);
		PresetCommand(con, VT340_load_destructiveff);
		PresetCommand(con, VT340_load_destructivedel);
		PresetCommand(con, VT340_load_swapbsdel);
		PresetCommand(con, VT340_load_useisolatin1);
		PresetCommand(con, VT340_load_titlebar);
		PresetCommand(con, VT340_load_ansicolors);
		PresetCommand(con, VT340_load_cursorblinking);
		PresetCommand(con, VT340_load_cursorunderscore);
		PresetCommand(con, VT340_load_savescreento);
		PresetCommand(con, VT340_load_printername);
		PresetCommand(con, VT340_load_answerbackmsg);

		return(TRUE);
	}


	if(mode == XEM_PREFS_LOAD)	/* load prefs.. */
	{
		LONG size;
		BOOL success=TRUE;


		if((size = GetFileSize(prefsFile)) > 0)
		{
			BPTR fh;
			UBYTE *file;

			if(file = AllocVec(size, MEMF_ANY))
			{
				if(fh = Open(prefsFile, MODE_OLDFILE))
				{
					if(Read(fh, file, size) > 0)
					{
						LoadCommand(con, file, "SetClass",			VT340_load_setclass);
						LoadCommand(con, file, "SetID",				VT340_load_setid);
						LoadCommand(con, file, "NewlineMode",		VT340_load_newlinemode);
						LoadCommand(con, file, "InsertMode",		VT340_load_insertmode);
						LoadCommand(con, file, "WrapMode",			VT340_load_wrapmode);
						LoadCommand(con, file, "CursorVisible",	VT340_load_cursorvisible);
						LoadCommand(con, file, "SmoothScroll", 	VT340_load_smoothscroll);
						LoadCommand(con, file, "QuickRend", 		VT340_load_quickrend);
						LoadCommand(con, file, "Use24lines",		VT340_load_use24lines);
						LoadCommand(con, file, "AdjustLines",		VT340_load_adjustlines);
						LoadCommand(con, file, "DestructiveBS",	VT340_load_destructivebs);
						LoadCommand(con, file, "DestructiveFF",	VT340_load_destructiveff);
						LoadCommand(con, file, "DestructiveDEL",	VT340_load_destructivedel);
						LoadCommand(con, file, "BackspaceBS",		VT340_load_swapbsdel);
						LoadCommand(con, file, "UseISOLatin1",		VT340_load_useisolatin1);
						LoadCommand(con, file, "TitleBar",			VT340_load_titlebar);
						LoadCommand(con, file, "AnsiColors",		VT340_load_ansicolors);
						LoadCommand(con, file, "CursorBlinking",	VT340_load_cursorblinking);
						LoadCommand(con, file, "CursorUnderscore",VT340_load_cursorunderscore);
						LoadCommand(con, file, "SaveScreenTo",		VT340_load_savescreento);
						LoadCommand(con, file, "PrinterName",		VT340_load_printername);
						LoadCommand(con, file, "AnswerBackMsg",	VT340_load_answerbackmsg);
					}
					Close(fh);
				}
				else
					success = FALSE;

				FreeVec(file);
			}
		}
		return(success);
	}


	if(mode == XEM_PREFS_SAVE)	/* save prefs */
	{
		BPTR fh;
		BOOL success=TRUE;

		if(fh = Open(prefsFile, MODE_NEWFILE))
		{
			FPuts(fh, ".ts=8\n/* Prefs for external vt340 */\n");
			FPuts(fh, "/* Command-words are case sensitive..!! */\n\n");

			SaveCommand(con, fh, "SetClass",			VT340_save_setclass);
			SaveCommand(con, fh, "SetIdent",			VT340_save_setid);
			SaveCommand(con, fh, "NewlineMode",		VT340_save_newlinemode);
			SaveCommand(con, fh, "InsertMode",		VT340_save_insertmode);
			SaveCommand(con, fh, "WrapMode",			VT340_save_wrapmode);
			SaveCommand(con, fh, "CursorVisible",	VT340_save_cursorvisible);
			SaveCommand(con, fh, "SmoothScroll", 	VT340_save_smoothscroll);
			SaveCommand(con, fh, "QuickRend", 		VT340_save_quickrend);
			SaveCommand(con, fh, "Use24lines",		VT340_save_use24lines);
			SaveCommand(con, fh, "AdjustLines",		VT340_save_adjustlines);
			SaveCommand(con, fh, "DestructiveBS",	VT340_save_destructivebs);
			SaveCommand(con, fh, "DestructiveFF",	VT340_save_destructiveff);
			SaveCommand(con, fh, "DestructiveDEL",	VT340_save_destructivedel);
			SaveCommand(con, fh, "BackspaceBS",		VT340_save_swapbsdel);
			SaveCommand(con, fh, "UseISOLatin1",	VT340_save_useisolatin1);
			SaveCommand(con, fh, "TitleBar",			VT340_save_titlebar);
			SaveCommand(con, fh, "AnsiColors",		VT340_save_ansicolors);
			SaveCommand(con, fh, "CursorBlinking",	VT340_save_cursorblinking);
			SaveCommand(con, fh, "CursorUnderscore",VT340_save_cursorunderscore);
			SaveCommand(con, fh, "SaveScreenTo",	VT340_save_savescreento);
			SaveCommand(con, fh, "PrinterName",		VT340_save_printername);
			SaveCommand(con, fh, "AnswerBackMsg",	VT340_save_answerbackmsg);

			con->r_pstat		= con->pstat;

			con->r_identify	= con->identify;

			if(con->tstat & T_WRAP)
				con->r_tstat |= T_WRAP;
			else
				con->r_tstat &= ~T_WRAP;

			if(con->ustat & U_CURSOR_VISIBLE)
				con->r_ustat |= U_CURSOR_VISIBLE;
			else
				con->r_ustat &= ~U_CURSOR_VISIBLE;

			if(con->ustat & U_NEWLINE)
				con->r_ustat |= U_NEWLINE;
			else
				con->r_ustat &= ~U_NEWLINE;

			if(con->ustat & U_INSERT)
				con->r_ustat |= U_INSERT;
			else
				con->r_ustat &= ~U_INSERT;


			Close(fh);
		}
		else
			success = FALSE;

		return(success);
	}
}


/* <EOB> */
