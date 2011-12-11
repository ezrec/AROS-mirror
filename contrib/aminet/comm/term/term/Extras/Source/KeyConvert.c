/*
**	KeyConvert.c
**
**	Key code conversion routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* ConvertKey():
	 *
	 *	The actual key conversion routine.
	 */

STATIC LONG
ConvertKey(ULONG Qualifier,UWORD Code,ULONG Prev,STRPTR Buffer,LONG Len)
{
	LONG Actual;

		/* Fill in the defaults. */

	FakeInputEvent->ie_Code				= Code;
	FakeInputEvent->ie_Qualifier		= Qualifier;
	FakeInputEvent->ie_position.ie_addr	= (APTR)(IPTR)Prev;

		/* Clear the buffer (sortof). */

	Buffer[0] = 0;

		/* Convert the codes. */

	if((Actual = RawKeyConvert(FakeInputEvent,Buffer,Len,KeyMap)) > 0)
		return(Actual);
	else
		return(0);
}

	/* ConvertTheKey(STRPTR Buffer,LONG *Len,UWORD Code,UWORD Qualifier,ULONG Prev):
	 *
	 *	Much more simplified raw key conversion routine.
	 */

LONG
ConvertTheKey(STRPTR Buffer,LONG *Len,UWORD Code,UWORD Qualifier,ULONG Prev)
{
	if(Buffer)
		Buffer[0] = 0;

	if(Len)
		*Len = 0;

		/* Key was pressed, not released. */

	if(!(Code & IECODE_UP_PREFIX))
	{
			/* These are the sequences mapped to special
			 * control keys (cursor keys, function keys,
			 * the help key).
			 */

		STATIC struct { STRPTR RawCode; UBYTE Result; } ConversionTable[15] =
		{
			{ (STRPTR)"A",	CUP, },
			{ (STRPTR)"B",	CDN, },
			{ (STRPTR)"C",	CFW, },
			{ (STRPTR)"D",	CBK, },

			{ (STRPTR)"?~",	HLP, },

			{ (STRPTR)"0~",	FN1, },
			{ (STRPTR)"1~",	FN2, },
			{ (STRPTR)"2~",	FN3, },
			{ (STRPTR)"3~",	FN4, },
			{ (STRPTR)"4~",	FN5, },
			{ (STRPTR)"5~",	FN6, },
			{ (STRPTR)"6~",	FN7, },
			{ (STRPTR)"7~",	FN8, },
			{ (STRPTR)"8~",	FN9, },
			{ (STRPTR)"9~",	F10 },
		};

		STATIC UBYTE SeqLens[15] = { 1,1,1,1,2,2,2,2,2,2,2,2,2,2,2 };

		UBYTE	ConvertBuffer[256];
		LONG	Actual;

			/* If it's a function or cursor key, clear the qualifier. */

		if(Code >= CURSOR_UP_CODE && Code <= F10_CODE)
			Qualifier = 0;
		else
		{
				/* Does it have a numeric keypad qualifier set? */

			if((Qualifier & (IEQUALIFIER_NUMERICPAD | IEQUALIFIER_CONTROL)) == (IEQUALIFIER_NUMERICPAD | IEQUALIFIER_CONTROL))
			{
				STRPTR String = NULL;

					/* Look at the vanilla result. */

				if(ConvertKey(Qualifier & ~(CONTROL_KEY | SHIFT_KEY | ALT_KEY),Code,Prev,ConvertBuffer,1))
				{
						/* Take a look at the results. */

					switch(ConvertBuffer[0])
					{
						case '(':
						case '[':
						case '{':

							String = "\033OP";
							break;

						case ')':
						case ']':
						case '}':

							String = "\033OQ";
							break;

						case '/':

							String = "\033OR";
							break;

						case '*':

							String = "\033OS";
							break;

						default:

							String = NULL;
							break;
					}
				}

				if(!String)
				{
					switch(Code)
					{
						case 0x5A:

							String = "\033OP";
							break;

						case 0x5B:

							String = "\033OQ";
							break;

						case 0x5C:

							String = "\033OR";
							break;

						case 0x5D:

							String = "\033OS";
							break;
					}
				}

					/* Return the PFx key code. */

				if(String)
				{
					if(Buffer)
						CopyMem(String,Buffer,3);

					if(Len)
						*Len = 3;

					return('\033');
				}
			}

				/* Does it have a shift qualifier set? */

			if(Qualifier & SHIFT_KEY)
			{
					/* Do the conversion... */

				if(ConvertKey(Qualifier & ~SHIFT_KEY,Code,Prev,ConvertBuffer,1))
				{
						/* Did it produce a tab key? If so, transfer
						 * Esc tab instead.
						 */

					if(ConvertBuffer[0] == '\t')
					{
						if(Len)
							*Len = 2;

						if(Buffer)
							CopyMem("\033\t",Buffer,2);

						return('\033');
					}
				}
			}

				/* Does it have the control qualifier set? */

			if(Qualifier & CONTROL_KEY)
			{
					/* Do the conversion... */

				if(ConvertKey(Qualifier & ~CONTROL_KEY,Code,Prev,ConvertBuffer,1))
				{
						/* Did it produce a space or an `at' sign? */

					switch(ConvertBuffer[0])
					{
							/* NUL */

						case ' ':
						case '@':
						case '2':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0;

							return(0);

							/* Escape */

						case '3':
						case '[':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0x1B;

							return(0x1B);

							/* FS */

						case '4':
						case '/':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0x1C;

							return(0x1C);

							/* GS */

						case '5':
						case ']':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0x1D;

							return(0x1D);

							/* RS */

						case '6':
						case '~':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0x1E;

							return(0x1E);


							/* US */

						case '7':
						case '?':

							if(Len)
								*Len = 1;

							if(Buffer)
								Buffer[0] = 0x1F;

							return(0x1F);
					}
				}
			}
		}

			/* Do the final conversion... */

		if(Actual = ConvertKey(Qualifier,Code,Prev,ConvertBuffer,sizeof(ConvertBuffer) - 1))
		{
				/* Are we to swap the backspace and
				 * delete keys?
				 */

			if(Config->EmulationConfig->SwapBSDelete)
			{
				LONG i;

				for(i = 0 ; i < Actual ; i++)
				{
					if(ConvertBuffer[i] == BKS)
						ConvertBuffer[i] = DEL;
					else
					{
						if(ConvertBuffer[i] == DEL)
							ConvertBuffer[i] = BKS;
					}
				}
			}

				/* Translated sequence starts
				 * with a CSI, let's have a look
				 * at the associated control
				 * key.
				 */

			if(ConvertBuffer[0] == CSI)
			{
				LONG i;

				for(i = 0 ; i < sizeof(SeqLens) ; i++)
				{
						/* Does it match? */

					if(!memcmp(&ConvertBuffer[1],ConversionTable[i].RawCode,SeqLens[i]))
					{
							/* Store the length. */

						if(Len)
							*Len = 1;

							/* Store the result. */

						if(Buffer)
						{
							Buffer[0] = ConversionTable[i].Result;
							Buffer[1] = 0;
						}

						return(ConversionTable[i].Result);
					}
				}
			}

				/* Store the number of characters converted. */

			if(Len)
				*Len = Actual;

				/* Store the converted characters. */

			if(Buffer)
				CopyMem(ConvertBuffer,Buffer,Actual);

			return(ConvertBuffer[0]);
		}
	}

	return(0);
}

	/* KeyConvert(struct IntuiMessage *Message,STRPTR Buffer):
	 *
	 *	Convert a raw key information according to the
	 *	current keymap settings.
	 */

LONG
KeyConvert(struct IntuiMessage *Message,STRPTR Buffer,LONG *Len)
{
		/* Is this really a keyboard event? */

	if(Message->Class == IDCMP_RAWKEY)
		return(ConvertTheKey(Buffer,Len,Message->Code,Message->Qualifier,*(ULONG *)Message->IAddress));
	else
	{
		if(Buffer)
			Buffer[0] = 0;

		if(Len)
			*Len = 0;

		return(0);
	}
}
