/*
**	CaptureParser.c
**
**	Capture filter parser
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif


/**********************************************************************/


typedef BOOL (* SPECIAL_JUMP)(ParseContext *Context);
typedef BOOL (* ABORT_JUMP)(ParseContext *Context,LONG Char);

STATIC SPECIAL_JUMP	*LocalSpecialTable;
STATIC ABORT_JUMP	*LocalAbortTable;


/**********************************************************************/


STATIC BOOL
AbortCancel(ParseContext *Context,LONG UnusedChar)
{
	Context->pc_InSequence		= FALSE;
	Context->pc_CharsInBuffer	= 0;
	Context->pc_ScanStep		= 0;

	return(FALSE);
}

STATIC BOOL
AbortEsc(ParseContext *Context,LONG c)
{
	AbortCancel(Context,c);

	Context->pc_InSequence = TRUE;

	return(TRUE);
}

STATIC BOOL
LocalParse(ParseContext *Context,LONG c)
{
	if(!Context->pc_ScanStep)
	{
		LONG i;

		for(i = 0 ; i < NumCodes ; i++)
		{
			if(ANSICode[i].FirstChar == c)
			{
				if(ANSICode[i].ExactSize == 1)
				{
					Context->pc_CharsInBuffer = Context->pc_ScanStep = 0;

					return(FALSE);
				}
				else
				{
					Context->pc_ScanStep = i;

					Context->pc_SaveBuffer[Context->pc_CharsInBuffer++] = c;

					Context->pc_Arnie = ANSICode[i].Terminator;

					return(TRUE);
				}
			}
		}
	}
	else
	{
		if(Context->pc_CharsInBuffer < MAX_SCAN_SIZE)
		{
			if(Context->pc_Arnie)
			{
				LONG i;

				for(i = Context->pc_ScanStep ; i < NumCodes ; i++)
				{
					if(ANSICode[i].FirstChar == Context->pc_SaveBuffer[0])
					{
						if(Context->pc_Arnie[c])
						{
							Context->pc_CharsInBuffer = Context->pc_ScanStep = 0;

							Context->pc_Arnie = NULL;

							return(FALSE);
						}
						else
						{
							if(ANSICode[i].Match[c])
							{
								Context->pc_ScanStep = i;

								Context->pc_SaveBuffer[Context->pc_CharsInBuffer++] = c;

								return(TRUE);
							}
						}
					}
				}
			}
			else
			{
				LONG i;

				for(i = Context->pc_ScanStep ; i < NumCodes ; i++)
				{
					if(ANSICode[i].FirstChar == Context->pc_SaveBuffer[0])
					{
						if(ANSICode[i].LastChar == c || (!ANSICode[i].LastChar && Context->pc_CharsInBuffer == 2 && ANSICode[i].ExactSize == 3))	/* Special case for VT52 */
						{
							Context->pc_CharsInBuffer = Context->pc_ScanStep = 0;

							return(FALSE);
						}
						else
						{
							if(ANSICode[i].Match[c])
							{
								Context->pc_ScanStep = i;

								Context->pc_SaveBuffer[Context->pc_CharsInBuffer++] = c;

								return(TRUE);
							}
						}
					}
				}
			}
		}
	}

	Context->pc_CharsInBuffer = Context->pc_ScanStep = 0;

	Context->pc_Arnie = NULL;

	return(FALSE);
}

STATIC BOOL
AbortCSI(ParseContext *Context,LONG c)
{
	AbortCancel(Context,c);

	Context->pc_InSequence = TRUE;

	return(LocalParse(Context,'['));
}


/**********************************************************************/


STATIC BOOL
SpecialBackspace(ParseContext *Context)
{
	if(Context->pc_LineIndex)
		Context->pc_LineIndex--;

	return(FALSE);
}

STATIC BOOL
SpecialReturn(ParseContext *Context)
{
	Context->pc_LineIndex = 0;

	return(FALSE);
}

STATIC BOOL
SpecialTab(ParseContext *Context)
{
	LONG Index = (Context->pc_LineIndex + 7) & ~8;

	if(Index == Context->pc_LineIndex)
		Index += 8;

	if(Index > Context->pc_LineLen)
		memset(&Context->pc_LineBuffer[Context->pc_LineLen],' ',Index - Context->pc_LineLen);

	Context->pc_LineIndex = Index;

	if(Index > Context->pc_LineLen)
		Context->pc_LineLen = Index;

	return(FALSE);
}

STATIC BOOL
SpecialEsc(ParseContext *UnusedContext)
{
	return(TRUE);
}

STATIC BOOL
SpecialCSI(ParseContext *Context)
{
	return(LocalParse(Context,'['));
}


/**********************************************************************/


VOID
CaptureParserExit()
{
	FreeVecPooled(LocalSpecialTable);
	FreeVecPooled(LocalAbortTable);

	LocalSpecialTable = NULL;
	LocalAbortTable = NULL;
}

BOOL
CaptureParserInit()
{
	if(LocalSpecialTable = (SPECIAL_JUMP *)AllocVecPooled(256 * sizeof(SPECIAL_JUMP),MEMF_ANY | MEMF_CLEAR))
	{
		if(LocalAbortTable = (ABORT_JUMP *)AllocVecPooled(256 * sizeof(ABORT_JUMP),MEMF_ANY | MEMF_CLEAR))
		{
			STATIC struct { UBYTE Key; SPECIAL_JUMP Routine; } LocalSpecialKeys[] =
			{
			    {
				'\b',	SpecialBackspace,
			    }, {
				'\r',	SpecialReturn,
			    }, {
				'\t',	SpecialTab,
			    }, {
				27,		SpecialEsc,
			    }, {
				155,	SpecialCSI
			    }
			};

			LONG i;

			for(i = 0 ; i < NUM_ELEMENTS(LocalSpecialKeys) ; i++)
				LocalSpecialTable[LocalSpecialKeys[i].Key] = LocalSpecialKeys[i].Routine;

			for(i = 0 ; i < 256 ; i++)
			{
				switch(AbortMap[i])
				{
					case 0:

						LocalAbortTable[i] = LocalParse;
						break;

					case 1:

						LocalAbortTable[i] = AbortCancel;
						break;

					case 2:

						LocalAbortTable[i] = AbortEsc;
						break;

					case 3:

						LocalAbortTable[i] = AbortCSI;
						break;
				}
			}

			return(TRUE);
		}
	}

	return(FALSE);
}

ParseContext *
CreateParseContext()
{
	return((ParseContext *)AllocVecPooled(sizeof(ParseContext),MEMF_ANY | MEMF_CLEAR));
}


/**********************************************************************/


	/* CaptureParser(ParseContext *Context,STRPTR Buffer,LONG Size,COPTR OutputRoutine):
	 *
	 *	This is very similar to what happens in Emulation.c, in fact we're using the
	 *	same data. The basic difference is that no emulation routines are invoked.
	 *	Every single one is skipped, only the special control codes like form feed,
	 *	tab or carriage return survive. Eventually, we end up building single text
	 *	lines that are handed over to the supplied output callback routine. Are we
	 *	having fun?
	 */

VOID
CaptureParser(ParseContext *Context,STRPTR Buffer,LONG Size,COPTR OutputRoutine)
{
	if(Size > 0)
	{
		LONG c,Mask;

		if(Config->SerialConfig->StripBit8)
			Mask = 0x7F;
		else
			Mask = 0xFF;

		if(Context->pc_InSequence)
		{
			BOOL Result;

			do
			{
				c = *Buffer++ & Mask;

				Result = (*LocalAbortTable[c])(Context,c);
			}
			while(--Size > 0 && Result);

			Context->pc_InSequence = Result;
		}

		if(Size > 0)
		{
			LONG BufferWidth = Config->CaptureConfig->BufferWidth - 1;

			if(Config->TerminalConfig->FontMode == FONT_STANDARD)
			{
				do
				{
					c = (*Buffer++) & Mask;

					if(Context->pc_InSequence)
						Context->pc_InSequence = (*LocalAbortTable[c])(Context,c);
					else
					{
						if(LocalSpecialTable[c])
							Context->pc_InSequence = (*LocalSpecialTable[c])(Context);
						else
						{
							if(c == '\n' || c == '\f' || c == '\v')
							{
								(*OutputRoutine)(Context->pc_LineBuffer,Context->pc_LineLen);

								Context->pc_LineIndex = Context->pc_LineLen = 0;
							}
							else
							{
								if(IsGlyph[c])
								{
									Context->pc_LineBuffer[Context->pc_LineIndex++] = c;

									if(Context->pc_LineIndex > Context->pc_LineLen)
										Context->pc_LineLen = Context->pc_LineIndex;

									if(Context->pc_LineLen > BufferWidth)
									{
										(*OutputRoutine)(Context->pc_LineBuffer,Context->pc_LineLen);

										Context->pc_LineIndex = Context->pc_LineLen = 0;
									}
								}
							}
						}
					}
				}
				while(--Size > 0);
			}
			else
			{
				do
				{
					c = (*Buffer++) & Mask;

					if(Context->pc_InSequence)
						Context->pc_InSequence = (*LocalAbortTable[c])(Context,c);
					else
					{
						if(LocalSpecialTable[c])
							Context->pc_InSequence = (*LocalSpecialTable[c])(Context);
						else
						{
							if(c == '\n' || c == '\f' || c == '\v')
							{
								(*OutputRoutine)(Context->pc_LineBuffer,Context->pc_LineLen);

								Context->pc_LineIndex = Context->pc_LineLen = 0;
							}
							else
							{
								if(c)
								{
									Context->pc_LineBuffer[Context->pc_LineIndex++] = c;

									if(Context->pc_LineIndex > Context->pc_LineLen)
										Context->pc_LineLen = Context->pc_LineIndex;

									if(Context->pc_LineLen > BufferWidth)
									{
										(*OutputRoutine)(Context->pc_LineBuffer,Context->pc_LineLen);

										Context->pc_LineIndex = Context->pc_LineLen = 0;
									}
								}
							}
						}
					}
				}
				while(--Size > 0);
			}
		}
	}
}
