#include "ownincs/AmigaConsole.h"


                    /* AmigaANSI X3.64 / ISO 6429.2 */



/* allowed globals.. */
struct	ExecBase			*SysBase;
struct	DosLibrary		*DOSBase;
struct	GfxBase			*GfxBase;
struct	IntuitionBase	*IntuitionBase;
struct	Library			*KeymapBase;
struct	Library			*UtilityBase;


/* utils.a */
VOID *GetSucc(VOID *);
VOID *GetHead(VOID *);
VOID *GetTail(VOID *);
VOID *GetPred(VOID *);
VOID BZero(VOID *, LONG);
VOID BSet(VOID *, LONG, LONG);

STATIC VOID Amiga_internalsettings(struct AmigaConsole *con);
STATIC BOOL Amiga_parseoptions(struct AmigaConsole *con, STRPTR optionsBuffer);


VOID __saveds XEmulatorExpu(VOID)
{
	if(KeymapBase)
	{
		CloseLibrary(KeymapBase);
		KeymapBase = NULL;
	}
}


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



BOOL __saveds XEmulatorInit(VOID)
{
	KeymapBase = OpenLibrary("keymap.library", LIBRARY_MINIMUM);

	if(KeymapBase == NULL)
	{
		XEmulatorExpu();
		return(FALSE);
	}

	return(TRUE);
}


BOOL __saveds __asm XEmulatorSetup(register __a0 struct XEM_IO *io)
{
	struct AmigaConsole *con;
	
	io->xem_console = NULL;

	if(con = AllocMem(sizeof(struct AmigaConsole), MEMF_PUBLIC | MEMF_CLEAR))
	{
		con->io = io;			/* for easier passing..(-; */
		io->xem_console = con;

		if(con->wconport = CreatePort(NULL, 0))
		{
			if(con->wcon = CreateStdIO(con->wconport))
			{
				return(TRUE);
			}
		}
	}

	return(FALSE);
}


VOID __saveds __asm XEmulatorCleanup(register __a0 struct XEM_IO *io)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;

	if(con)
	{
		*con->io->xem_signal = 0;

		if(con->wcon->io_Device)
			CloseDevice((struct IORequest *)con->wcon);

		if(con->wcon)
			DeleteStdIO(con->wcon);

		if(con->wconport)
			DeletePort(con->wconport);

		FreeMem(con, sizeof(struct AmigaConsole));
	}
}


BOOL __saveds __asm XEmulatorOpenConsole(register __a0 struct XEM_IO *io)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;

	if(con)
	{
		con->wcon->io_Data	= (APTR)con->io->xem_window;
		con->wcon->io_Length	= sizeof(struct Window);
		if(!OpenDevice("console.device", 0, (struct IORequest *)con->wcon, 0))
		{
			if(con->preStr[0])
			{
				con->wcon->io_Length	= strlen(con->preStr);
				con->wcon->io_Data	= (APTR)con->preStr;
				con->wcon->io_Command= CMD_WRITE;
				DoIO((struct IORequest *)con->wcon);
			}

			return(TRUE);	
		}
		else
			con->wcon->io_Device = NULL;
	}

	return(FALSE);	
}


VOID __saveds __asm XEmulatorCloseConsole(register __a0 struct XEM_IO *io)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;

	if(con)
	{
		if(con->wcon->io_Device)
		{
			CloseDevice((struct IORequest *)con->wcon);
			con->wcon->io_Device = NULL;
		}
	}
}


VOID __saveds __asm XEmulatorWrite(register __a0 struct XEM_IO *io, register __a1 UBYTE *buff, register __d0 ULONG buflen)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;

	con->wcon->io_Length	= buflen;
	con->wcon->io_Data	= (APTR)buff;
	con->wcon->io_Command= CMD_WRITE;
	DoIO((struct IORequest *)con->wcon);
}

BOOL __saveds __asm XEmulatorSignal(register __a0 struct XEM_IO *io, register __d0 ULONG sig)
{
	return(TRUE);
}


STATIC BOOL HandleMacroKeys(struct AmigaConsole *con, struct IntuiMessage *imsg, UBYTE chr)
{
	if(con->macrokeys  &&  con->io->xem_process_macrokeys)
	{
		struct XEmulatorMacroKey *key;
		BOOL shift, alt, ctrl;
		UWORD qual;

		shift	= imsg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT);
		ctrl	= imsg->Qualifier &  IEQUALIFIER_CONTROL;
		alt	= imsg->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT);

		if(shift)
			qual = XMKQ_SHIFT;
		else
		{
			if(alt)
				qual = XMKQ_ALTERNATE;
			else
			{
				if(ctrl)
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
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;
	struct InputEvent ie;
	ULONG length=0;
	UBYTE *p;


	if(imsg->Code & IECODE_UP_PREFIX)
		return(0);

	p = retstr;
	*p = '\0';

   ie.ie_Class			= IECLASS_RAWKEY;
   ie.ie_SubClass		= 0;
	ie.ie_Code			= imsg->Code;
	ie.ie_Qualifier	= imsg->Qualifier;
	ie.ie_position.ie_addr = *((APTR *)imsg->IAddress);

	if((length = MapRawKey(&ie, retstr, maxlen, NULL)) <= 0)
		return(0);

	*(p+length) = '\0';			/* Null terminate the value */

	if(HandleMacroKeys(con, imsg, p[0]))
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

			if(hd->InESC)		/* Escape - Befehl ? */
			{
				if(c == '[')	/* ist's ein verkappter CSI ? */
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

			if(hd->InCSI)		/* CSI - Befehl ? */
			{
				if(c >= '@')
					hd->InCSI = FALSE;
				continue;
			}


			if(c == '\033')
			{
				hd->InESC = TRUE;
				hd->InCSI = FALSE;
				continue;
			}

			if(c == '\233')
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
	XEmulatorWrite(io, "\014", 1);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetConsole(register __a0 struct XEM_IO *io)
{
	XEmulatorWrite(io, "\033c", 2);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetTextStyles(register __a0 struct XEM_IO *io)
{
	XEmulatorWrite(io, "\033[m", -1);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorResetCharset(register __a0 struct XEM_IO *io)
{
	XEmulatorWrite(io, "\017", 1);
	return(TRUE);
}


BOOL __saveds __asm XEmulatorOptions(register __a0 struct XEM_IO *io)
{
	return(TRUE);
}


ULONG __saveds __asm XEmulatorGetFreeMacroKeys(register __a0 struct XEM_IO *io)
{
	return(512 + 256 + 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1);
}


BOOL __saveds __asm XEmulatorMacroKeyFilter(register __a0 struct XEM_IO *io, register __a1 struct XEmulatorMacroKey *macrokeys)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;

	if(con != NULL  &&  con->io->xem_process_macrokeys)
	{
		con->macrokeys = macrokeys;
		return(TRUE);
	}

	return(FALSE);
}


#include <devices/conunit.h>

LONG __saveds __asm XEmulatorInfo(register __a0 struct XEM_IO *io, register __d0 ULONG type)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;
	LONG result = -1;

	if(con != NULL)
	{
		struct ConUnit	*unit = (struct ConUnit *)con->wcon->io_Unit;

		switch(type)
		{
			case XEMI_CURSOR_POSITION:
				result = XEMI_CREATE_POSITION(unit->cu_YCCP+1, unit->cu_XCCP+1);
			break;

			case XEMI_CONSOLE_DIMENSIONS:
				result = XEMI_CREATE_DIMENSIONS(unit->cu_XMax+1, unit->cu_YMax+1);
			break;

		}
	}

	return(result);
}


BOOL __saveds __asm XEmulatorPreferences(register __a0 struct XEM_IO *io, register __a1 STRPTR filename, register __d0 ULONG mode)
{
	struct AmigaConsole *con = (struct AmigaConsole *)io->xem_console;
	BOOL success=FALSE;

	if(con != NULL)
	{
		switch(mode)
		{
			case XEM_PREFS_RESET:
				success = TRUE;
				Amiga_internalsettings(con);
			break;

			case XEM_PREFS_LOAD:
			{
				BPTR fh;
				UBYTE buf[80];

				if(fh = Open(filename, MODE_OLDFILE))
				{
					if(Read(fh, buf, 80) > 0)
					{
						if(Amiga_parseoptions(con, buf) == FALSE)
							Amiga_internalsettings(con);
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
					success = TRUE;

					FPrintf(fh, "CURSOR=YES");

					FPrintf(fh, " HSCROLL=YES");

					FPrintf(fh, " VSCROLL=YES");

					FPrintf(fh, " NEWLINE=NO");

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


STATIC VOID Amiga_internalsettings(struct AmigaConsole *con)
{
	/* some good(?) settings */

	strcpy(con->preStr, "› p");		/* cursor visible */
	strcat(con->preStr, "›>1h");		/* auto. horiz. Scrolling */
	strcat(con->preStr, "›?7h");		/* auto.  vert. Scrolling */
	strcat(con->preStr, "›20l");		/* LF => LF */
}


enum { ARG_CUR=0,ARG_HS,ARG_VS,ARG_NL,ARGS };

STATIC BOOL Amiga_parseoptions(struct AmigaConsole *con, STRPTR optionsBuffer)
{
	STATIC UBYTE *PreferencesTemplate = "CSR=CURSOR/K,HS=HSCROLL/K,VS=VSCROLL/K,NL=NEWLINE/K";
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

					if(ArgArray[ARG_CUR])
					{
						if(!strnicmp(ArgArray[ARG_CUR], "NO", 2))
							strcpy(con->preStr, "›0 p");
						else
							strcpy(con->preStr, "› p");
					}

					if(ArgArray[ARG_HS])
					{
						if(!strnicmp(ArgArray[ARG_HS], "NO", 2))
							strcat(con->preStr, "›>1l");
						else
							strcat(con->preStr, "›>1h");
					}

					if(ArgArray[ARG_VS])
					{
						if(!strnicmp(ArgArray[ARG_VS], "NO", 2))
							strcat(con->preStr, "›?7l");
						else
							strcat(con->preStr, "›?7h");
					}

					if(ArgArray[ARG_NL])
					{
						if(!strnicmp(ArgArray[ARG_NL], "NO", 2))
							strcat(con->preStr, "›20l");
						else
							strcat(con->preStr, "›20h");
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


VOID __saveds __asm XEmulatorNewSize(register __a0 struct XEM_IO *io, register __a1 struct IBox *frame)
{
}


/* end of source-code */
