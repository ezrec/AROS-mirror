/*
**	EndOfLineTranslate.c
**
**	End-of-line character translation routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**:	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC STRPTR	CR_Translation,
				LF_Translation;
STATIC WORD		CR_Trans_Len,
				LF_Trans_Len;

STATIC LONG
Translate_CR_LF_1(STRPTR Data,LONG Size)
{
	STRPTR	String	= Data;
	LONG	Count	= 0;
	LONG	c;

	while(Size--)
	{
		switch(c = *Data++)
		{
			case '\r':

				if(CR_Trans_Len)
				{
					*String++ = *CR_Translation;

					Count++;
				}

				break;

			case '\n':

				if(LF_Trans_Len)
				{
					*String++ = *LF_Translation;

					Count++;
				}

				break;

			default:

				*String++ = c;

				Count++;

				break;
		}
	}

	return(Count);
}

STATIC LONG
Translate_CR_LF_2x2(STRPTR Data,LONG Size)
{
	STRPTR	String	= Data;
	LONG	Count	= 0;
	LONG	c;

	/* ALWAYS */
	{
		BOOL GotIt = FALSE;
		LONG i;

		for(i = 0 ; i < Size ; i++)
		{
			if(String[i] == '\r' || String[i] == '\n')
			{
				GotIt = TRUE;

				break;
			}
		}

		if(GotIt)
		{
			CopyMem(Data,StripBuffer,Size);

			String	= Data;
			Data	= StripBuffer;
		}
		else
			return(Size);
	}

	while(Size--)
	{
		switch(c = *Data++)
		{
			case '\r':

				if(CR_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < CR_Trans_Len ; i++)
					{
						*String++ = CR_Translation[i];

						Count++;
					}
				}

				break;

			case '\n':

				if(LF_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < LF_Trans_Len ; i++)
					{
						*String++ = LF_Translation[i];

						Count++;
					}
				}

				break;

			default:

				*String++ = c;

				Count++;

				break;
		}
	}

	return(Count);
}

STATIC LONG
Translate_CR_LF_2LF(STRPTR Data,LONG Size)
{
	STRPTR	String	= Data;
	LONG	Count	= 0;
	LONG	c;

	/* ALWAYS */
	{
		BOOL GotIt = FALSE;
		LONG i;

		for(i = 0 ; i < Size ; i++)
		{
			if(String[i] == '\n')
			{
				GotIt = TRUE;

				break;
			}
		}

		if(GotIt)
		{
			CopyMem(Data,StripBuffer,Size);

			String	= Data;
			Data	= StripBuffer;
		}
		else
			return(Size);
	}

	while(Size--)
	{
		switch(c = *Data++)
		{
			case '\r':

				if(CR_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < CR_Trans_Len ; i++)
					{
						*String++ = CR_Translation[i];

						Count++;
					}
				}

				break;

			case '\n':

				if(LF_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < LF_Trans_Len ; i++)
					{
						*String++ = LF_Translation[i];

						Count++;
					}
				}

				break;

			default:

				*String++ = c;

				Count++;

				break;
		}
	}

	return(Count);
}

STATIC LONG
Translate_CR_LF_2CR(STRPTR Data,LONG Size)
{
	STRPTR	String	= Data;
	LONG	Count	= 0;
	LONG	c;

	/* ALWAYS */
	{
		BOOL GotIt = FALSE;
		LONG i;

		for(i = 0 ; i < Size ; i++)
		{
			if(String[i] == '\r')
			{
				GotIt = TRUE;

				break;
			}
		}

		if(GotIt)
		{
			CopyMem(Data,StripBuffer,Size);

			String	= Data;
			Data	= StripBuffer;
		}
		else
			return(Size);
	}

	while(Size--)
	{
		switch(c = *Data++)
		{
			case '\r':

				if(CR_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < CR_Trans_Len ; i++)
					{
						*String++ = CR_Translation[i];

						Count++;
					}
				}

				break;

			case '\n':

				if(LF_Trans_Len)
				{
					LONG i;

					for(i = 0 ; i < LF_Trans_Len ; i++)
					{
						*String++ = LF_Translation[i];

						Count++;
					}
				}

				break;

			default:

				*String++ = c;

				Count++;

				break;
		}
	}

	return(Count);
}

VOID
Update_CR_LF_Translation()
{
	Forbid();

	if(Config->TerminalConfig->ReceiveCR == EOL_CRLF || Config->TerminalConfig->ReceiveCR == EOL_LFCR || Config->TerminalConfig->ReceiveLF == EOL_LFCR || Config->TerminalConfig->ReceiveLF == EOL_CRLF)
		SerialBufferSize = Config->SerialConfig->SerialBufferSize / 2;
	else
		SerialBufferSize = Config->SerialConfig->SerialBufferSize;

	switch(Config->TerminalConfig->ReceiveCR)
	{
		case EOL_IGNORE:

			CR_Trans_Len = 0;
			break;

		case EOL_CR:

			CR_Trans_Len	= 1;
			CR_Translation	= "\r";
			break;

		case EOL_LF:

			CR_Trans_Len	= 1;
			CR_Translation	= "\n";
			break;

		case EOL_CRLF:

			CR_Trans_Len	= 2;
			CR_Translation	= "\r\n";
			break;

		case EOL_LFCR:

			CR_Trans_Len	= 2;
			CR_Translation	= "\n\r";
			break;
	}

	switch(Config->TerminalConfig->ReceiveLF)
	{
		case EOL_IGNORE:

			LF_Trans_Len = 0;
			break;

		case EOL_LF:

			LF_Trans_Len	= 1;
			LF_Translation	= "\n";
			break;

		case EOL_CR:

			LF_Trans_Len	= 1;
			LF_Translation	= "\r";
			break;

		case EOL_LFCR:

			LF_Trans_Len	= 2;
			LF_Translation	= "\n\r";
			break;

		case EOL_CRLF:

			LF_Trans_Len	= 2;
			LF_Translation	= "\r\n";
			break;
	}

	if(Config->TerminalConfig->ReceiveCR == EOL_CR && Config->TerminalConfig->ReceiveLF == EOL_LF)
		Translate_CR_LF = NULL;
	else
	{
		if(LF_Trans_Len <= 1 && CR_Trans_Len <= 1)
			Translate_CR_LF = Translate_CR_LF_1;
		else
		{
			if(LF_Trans_Len == 2 && CR_Trans_Len == 2)
				Translate_CR_LF = Translate_CR_LF_2x2;
			else
			{
				if(LF_Trans_Len == 2)
					Translate_CR_LF = Translate_CR_LF_2LF;
				else
				{
					if(CR_Trans_Len == 2)
						Translate_CR_LF = Translate_CR_LF_2CR;
					else
						Translate_CR_LF = Translate_CR_LF_1;
				}
			}
		}
	}

	ConTransferUpdate();

	Permit();
}
