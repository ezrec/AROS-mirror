/*
**	Buffer.c
**
**	Auxilary routines for text buffer/capture management.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=3
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Search string window gadgets. */

enum	{	GAD_STRING=1000,GAD_OK,GAD_CANCEL };

	/* Maximum size of an allocated line string. */

#define STRING_SIZE	(1 + 255 + 1)

	/* How many strings to include in a single puddle. */

#define STRING_COUNT	10

	/* The number of lines the buffer will grow. */

#define BUFFER_GROW	100

	/* Memory pool header. */

STATIC APTR BufferPoolHeader;

	/* Word separator characters. */

STATIC BYTE WordSeparators[256] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0
};

	/* AllocString(STRPTR String,LONG Len):
	 *
	 *	Allocate space for a string, new pooled version.
	 */

STATIC STRPTR
AllocString(STRPTR String,LONG Len)
{
	STRPTR Mem;

	if(Len > 255)
		Len = 255;

	if(Mem = (STRPTR)AsmAllocPooled(BufferPoolHeader,1 + Len + 1,SysBase))
	{
		*Mem++ = Len;

		if(Len)
			CopyMem(String,Mem,Len);

		Mem[Len] = 0;
	}

	return(Mem);
}

	/* FreeString(STRPTR String):
	 *
	 *	Free the space occupied by a string, new pooled version.
	 */

STATIC VOID
FreeString(STRPTR String)
{
	AsmFreePooled(BufferPoolHeader,&String[-1],1 + String[-1] + 1,SysBase);
}

	/* AddLine(STRPTR Line,LONG Size):
	 *
	 *	Add a line to the display buffer.
	 */

VOID
AddLine(STRPTR Line,LONG Size)
{
		/* Are we still to update the buffer contents? */

	if(!BufferClosed)
	{
		ULONG Signals;
		BOOL Notify;

		Signals = NULL;

		ObtainSemaphore(&BufferSemaphore);

			/* Is the buffer array initialized? */

		if(BufferLines)
		{
				/* Check for limit. */

			if(Config->CaptureConfig->MaxBufferSize && BufferSpace >= Config->CaptureConfig->MaxBufferSize)
			{
				LONG i;

				BufferSpace -= BufferLines[0][-1];

				FreeString(BufferLines[0]);

				for(i = 1 ; i < MaxLines ; i++)
					BufferLines[i - 1] = BufferLines[i];

				Lines--;

					/* Tell the buffer task to
					 * refresh the display.
					 */

				Signals = SIG_MOVEUP;
			}
			else
			{
					/* We've reached the last line in the buffer. */

				if(Lines == MaxLines || ((Lines & 3) == 3 && AvailMem(MEMF_LARGEST) <= Config->CaptureConfig->BufferSafetyMemory))
				{
					STRPTR *MoreBuffer;

						/* Allocate space for some more lines. */

					if(MoreBuffer = (STRPTR *)AllocVecPooled((MaxLines + BUFFER_GROW) * sizeof(STRPTR),MEMF_ANY | MEMF_CLEAR))
					{
						BufferChanged = TRUE;

							/* Copy the old lines to the new
							 * buffer.
							 */

						CopyMem(BufferLines,MoreBuffer,Lines * sizeof(STRPTR));

							/* Free the old lines. */

						FreeVecPooled(BufferLines);

							/* Set the new buffer. */

						MaxLines += BUFFER_GROW;

						BufferLines = MoreBuffer;
					}
					else
					{
						BufferChanged = TRUE;

							/* We couldn't get enough memory
							 * to extend the number of lines
							 * in the buffer, so we'll have
							 * to wrap the contents of the
							 * buffer around.
							 */

						if(Lines)
						{
							LONG i;

							BufferSpace -= BufferLines[0][-1];

							FreeString(BufferLines[0]);

							for(i = 1 ; i < MaxLines ; i++)
								BufferLines[i - 1] = BufferLines[i];

							Lines--;

								/* Tell the buffer task to
								 * refresh the display.
								 */

							Signals = SIG_MOVEUP;
						}
					}
				}
			}

				/* Allocate a new line and copy the buffer contents
				 * into it.
				 */

			if(BufferLines[Lines] = AllocString(Line,Size))
			{
				BufferChanged = TRUE;

				Lines++;

				BufferSpace += Size;
			}

			Notify = TRUE;
		}
		else
			Notify = FALSE;

		ReleaseSemaphore(&BufferSemaphore);

		if(Notify)
		{
				/* Tell the buffer tasks to update the displays. */

			if(!Signals)
				Signals = SIG_UPDATE;

			NotifyBuffer(&BufferTaskSemaphore,&BufferInfoData,Signals);
			NotifyBuffer(&ReviewTaskSemaphore,&ReviewInfoData,Signals);
		}
	}
}

	/* DeleteBuffer():
	 *
	 *	Delete buffer resources.
	 */

VOID
DeleteBuffer()
{
	FreeVecPooled(BufferLines);
	BufferLines = NULL;

	if(BufferPoolHeader)
	{
		AsmDeletePool(BufferPoolHeader,SysBase);
		BufferPoolHeader = NULL;
	}
}

	/* CreateBuffer():
	 *
	 *	Allocate buffer resources.
	 */

BOOL
CreateBuffer()
{
	if(BufferLines = (STRPTR *)AllocVecPooled(MaxLines * sizeof(STRPTR),MEMF_ANY | MEMF_CLEAR))
	{
			/* Create a memory pool header if possible. */

		if(BufferPoolHeader = AsmCreatePool(MEMF_ANY | MEMF_PUBLIC,STRING_SIZE * STRING_COUNT,STRING_SIZE * STRING_COUNT,SysBase))
			return(TRUE);
		else
		{
			FreeVecPooled(BufferLines);
			BufferLines = NULL;
		}
	}

	return(FALSE);
}

	/* FreeBuffer():
	 *
	 *	Release the contents of the text buffer.
	 */

VOID
FreeBuffer()
{
	BOOL Notify;

	ObtainSemaphore(&BufferSemaphore);

		/* Free the contents of the display buffer. */

	if(BufferLines)
	{
		APTR NewPoolHeader;

			/* If a new pool header is available, free the old
			 * pool and replace it with the new pool.
			 */

		if(NewPoolHeader = AsmCreatePool(MEMF_ANY | MEMF_PUBLIC,STRING_SIZE * STRING_COUNT,STRING_SIZE * STRING_COUNT,SysBase))
		{
			AsmDeletePool(BufferPoolHeader,SysBase);

			BufferPoolHeader = NewPoolHeader;
		}
		else
		{
			LONG i;

			for(i = 0 ; i < Lines ; i++)
			{
				if(BufferLines[i])
					FreeString(BufferLines[i]);
			}
		}

		FreeVecPooled(BufferLines);

		Lines = 0;

		MaxLines = BUFFER_GROW;

		BufferLines = (STRPTR *)AllocVecPooled(MaxLines * sizeof(STRPTR),MEMF_ANY | MEMF_CLEAR);

		Notify = TRUE;
	}
	else
		Notify = FALSE;

	BufferSpace = 0;

	BufferChanged = FALSE;

	ReleaseSemaphore(&BufferSemaphore);

	if(Notify)
	{
		NotifyBuffer(&BufferTaskSemaphore,&BufferInfoData,SIG_UPDATE);
		NotifyBuffer(&ReviewTaskSemaphore,&ReviewInfoData,SIG_UPDATE);
	}
}

	/* DeleteSearchInfo(struct SearchInfo *Info):
	 *
	 *	Free buffer allocated by CreateSearchInfo().
	 */

VOID
DeleteSearchInfo(struct SearchInfo *Info)
{
	FreeVecPooled(Info);
}

	/* CreateSearchInfo(STRPTR Pattern):
	 *
	 *	Create auxilary data required by SearchTextBuffer().
	 */

struct SearchInfo *
CreateSearchInfo(STRPTR Pattern,BOOL Forward,BOOL IgnoreCase,BOOL WholeWords)
{
	struct SearchInfo *Info;

		/* Allocate the buffer. */

	if(Info = (struct SearchInfo *)AllocVecPooled(sizeof(struct SearchInfo),MEMF_ANY|MEMF_CLEAR))
	{
		LONG i;

			/* Determine pattern width. */

		Info->PatternWidth	= strlen(Pattern);
		Info->IgnoreCase	= IgnoreCase;

		if(Info->PatternWidth == 1)
		{
			if(WordSeparators[Pattern[0]])
				WholeWords = FALSE;
		}

		Info->WholeWords = WholeWords;

			/* Turn the pattern into upper case characters. */

		if(IgnoreCase)
		{
			for(i = 0 ; i <= Info->PatternWidth ; i++)
				Info->Pattern[i] = ToUpper(Pattern[i]);
		}
		else
		{
			for(i = 0 ; i <= Info->PatternWidth ; i++)
				Info->Pattern[i] = Pattern[i];
		}

			/* Fill the entire range with the maximum pattern width. */

		for(i = 0 ; i < 256 ; i++)
			Info->Distance[i] = Info->PatternWidth;

			/* Fill in the matching distances. */

		if(Forward)
		{
			for(i = 0 ; i < Info->PatternWidth - 1 ; i++)
				Info->Distance[Info->Pattern[i]] = Info->PatternWidth - 1 - i;
		}
		else
		{
			for(i = Info->PatternWidth - 1 ; i > 0 ; i--)
				Info->Distance[Info->Pattern[i]] = i;
		}

			/* Restart from scratch. */

		Info->FoundY	= -1;
		Info->Forward	= Forward;
	}

	return(Info);
}

	/* SearchTextBuffer():
	 *
	 *	String search function, based on the Boyer-Moore search
	 *	algorithm.
	 */

LONG
SearchTextBuffer(struct SearchInfo *Info)
{
	if(BufferLines)
	{
		UBYTE	*Distance,*Pattern;
		LONG LineWidth,PatternWidth;
		STRPTR Line;
		LONG SearchPosition,PatternIndex,LineIndex,LastSearchPosition;
		LONG i;

			/* Extract the relevant data. */

		Distance			= Info->Distance;
		Pattern			= Info->Pattern;
		PatternWidth	= Info->PatternWidth;

		if(Info->WholeWords)
		{
				/* Which direction are we to search? */

			if(Info->IgnoreCase)
			{
				if(Info->Forward)
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= 0;
						LastSearchPosition	= 0;
					}
					else
					{
							/* Proceed to the next line. */

						if(!(LastSearchPosition = Info->Index))
							Info->FoundY = (Info->FoundY + 1) % Lines;
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i < Lines ; i++)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Where are we to start searching? */

							if(LastSearchPosition)
								SearchPosition = LastSearchPosition;
							else
								SearchPosition = PatternWidth;

							do
							{
									/* How many line characters
									 * match the pattern?
									 */

								PatternIndex	= PatternWidth - 1;
								LineIndex		= SearchPosition - 1;

								while(PatternIndex >= 0 && Pattern[PatternIndex] == ToUpper(Line[LineIndex]))
								{
									LineIndex--;
									PatternIndex--;
								}

									/* Update the line search index
									 * for subsequent searches.
									 */

								SearchPosition += Distance[ToUpper(Line[SearchPosition - 1])];

									/* Found the pattern? */

								if(PatternIndex < 0)
								{
									LONG X = LineIndex + 1;

									if(X)
									{
										if(!WordSeparators[Line[X - 1]])
											continue;
										else
										{
											if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
												continue;
										}
									}
									else
									{
										if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
											continue;
									}

										/* Store the position. */

									Info->FoundX	= LineIndex + 1;
									Info->FoundY	= i;

										/* Remember column to start
										 * next search attempt at.
										 */

									if(SearchPosition <= LineWidth)
										Info->Index = SearchPosition;
									else
										Info->Index = 0;

									return(i);
								}
							}
							while(SearchPosition <= LineWidth);
						}

							/* Reset search column. */

						LastSearchPosition = 0;
					}
				}
				else
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= Lines - 1;
						LastSearchPosition	= 0;
					}
					else
					{
						if((LastSearchPosition = Info->Index) < 1)
						{
							if(Info->FoundY)
								Info->FoundY--;
							else
								Info->FoundY = Lines - 1;
						}
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i >= 0 ; i--)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Cast the magic spell of Boyer-Moore... */

							if(LastSearchPosition < 1)
								SearchPosition = LineWidth - (PatternWidth - 1);
							else
								SearchPosition = LastSearchPosition;

							do
							{
								PatternIndex = 0;
								LineIndex	 = SearchPosition - 1;

								while(PatternIndex < PatternWidth && Pattern[PatternIndex] == ToUpper(Line[LineIndex]))
								{
									LineIndex++;
									PatternIndex++;
								}

								SearchPosition -= Distance[ToUpper(Line[SearchPosition - 1])];

								if(PatternIndex == PatternWidth)
								{
									LONG X = LineIndex - PatternWidth;

									if(X)
									{
										if(!WordSeparators[Line[X - 1]])
											continue;
										else
										{
											if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
												continue;
										}
									}
									else
									{
										if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
											continue;
									}

									Info->FoundX	= LineIndex - PatternWidth;
									Info->FoundY	= i;
									Info->Index		= SearchPosition;

									return(i);
								}
							}
							while(SearchPosition > 0);
						}

						LastSearchPosition = 0;
					}
				}
			}
			else
			{
				if(Info->Forward)
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= 0;
						LastSearchPosition	= 0;
					}
					else
					{
							/* Proceed to the next line. */

						if(!(LastSearchPosition = Info->Index))
							Info->FoundY = (Info->FoundY + 1) % Lines;
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i < Lines ; i++)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Where are we to start searching? */

							if(LastSearchPosition)
								SearchPosition = LastSearchPosition;
							else
								SearchPosition = PatternWidth;

							do
							{
									/* How many line characters
									 * match the pattern?
									 */

								PatternIndex	= PatternWidth - 1;
								LineIndex		= SearchPosition - 1;

								while(PatternIndex >= 0 && Pattern[PatternIndex] == Line[LineIndex])
								{
									LineIndex--;
									PatternIndex--;
								}

									/* Update the line search index
									 * for subsequent searches.
									 */

								SearchPosition += Distance[Line[SearchPosition - 1]];

									/* Found the pattern? */

								if(PatternIndex < 0)
								{
									LONG X = LineIndex + 1;

									if(X)
									{
										if(!WordSeparators[Line[X - 1]])
											continue;
										else
										{
											if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
												continue;
										}
									}
									else
									{
										if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
											continue;
									}

										/* Store the position. */

									Info->FoundX	= LineIndex + 1;
									Info->FoundY	= i;

										/* Remember column to start
										 * next search attempt at.
										 */

									if(SearchPosition <= LineWidth)
										Info->Index = SearchPosition;
									else
										Info->Index = 0;

									return(i);
								}
							}
							while(SearchPosition <= LineWidth);
						}

							/* Reset search column. */

						LastSearchPosition = 0;
					}
				}
				else
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= Lines - 1;
						LastSearchPosition	= 0;
					}
					else
					{
						if((LastSearchPosition = Info->Index) < 1)
						{
							if(Info->FoundY)
								Info->FoundY--;
							else
								Info->FoundY = Lines - 1;
						}
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i >= 0 ; i--)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Cast the magic spell of Boyer-Moore... */

							if(LastSearchPosition < 1)
								SearchPosition = LineWidth - (PatternWidth - 1);
							else
								SearchPosition = LastSearchPosition;

							do
							{
								PatternIndex = 0;
								LineIndex = SearchPosition - 1;

								while(PatternIndex < PatternWidth && Pattern[PatternIndex] == Line[LineIndex])
								{
									LineIndex++;
									PatternIndex++;
								}

								SearchPosition -= Distance[Line[SearchPosition - 1]];

								if(PatternIndex == PatternWidth)
								{
									LONG X = LineIndex - PatternWidth;

									if(X)
									{
										if(!WordSeparators[Line[X - 1]])
											continue;
										else
										{
											if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
												continue;
										}
									}
									else
									{
										if(X + PatternWidth < LineWidth && !WordSeparators[Line[X + PatternWidth]])
											continue;
									}

									Info->FoundX	= LineIndex - PatternWidth;
									Info->FoundY	= i;
									Info->Index	= SearchPosition;

									return(i);
								}
							}
							while(SearchPosition > 0);
						}

						LastSearchPosition = 0;
					}
				}
			}
		}
		else
		{
				/* Which direction are we to search? */

			if(Info->IgnoreCase)
			{
				if(Info->Forward)
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= 0;
						LastSearchPosition	= 0;
					}
					else
					{
							/* Proceed to the next line. */

						if(!(LastSearchPosition = Info->Index))
							Info->FoundY = (Info->FoundY + 1) % Lines;
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i < Lines ; i++)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Where are we to start searching? */

							if(LastSearchPosition)
								SearchPosition = LastSearchPosition;
							else
								SearchPosition = PatternWidth;

							do
							{
									/* How many line characters
									 * match the pattern?
									 */

								PatternIndex	= PatternWidth - 1;
								LineIndex		= SearchPosition - 1;

								while(PatternIndex >= 0 && Pattern[PatternIndex] == ToUpper(Line[LineIndex]))
								{
									LineIndex--;
									PatternIndex--;
								}

									/* Update the line search index
									 * for subsequent searches.
									 */

								SearchPosition += Distance[ToUpper(Line[SearchPosition - 1])];

									/* Found the pattern? */

								if(PatternIndex < 0)
								{
										/* Store the position. */

									Info->FoundX	= LineIndex + 1;
									Info->FoundY	= i;

										/* Remember column to start
										 * next search attempt at.
										 */

									if(SearchPosition <= LineWidth)
										Info->Index = SearchPosition;
									else
										Info->Index = 0;

									return(i);
								}
							}
							while(SearchPosition <= LineWidth);
						}

							/* Reset search column. */

						LastSearchPosition = 0;
					}
				}
				else
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= Lines - 1;
						LastSearchPosition	= 0;
					}
					else
					{
						if((LastSearchPosition = Info->Index) < 1)
						{
							if(Info->FoundY)
								Info->FoundY--;
							else
								Info->FoundY = Lines - 1;
						}
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i >= 0 ; i--)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Cast the magic spell of Boyer-Moore... */

							if(LastSearchPosition < 1)
								SearchPosition = LineWidth - (PatternWidth - 1);
							else
								SearchPosition = LastSearchPosition;

							do
							{
								PatternIndex = 0;
								LineIndex = SearchPosition - 1;

								while(PatternIndex < PatternWidth && Pattern[PatternIndex] == ToUpper(Line[LineIndex]))
								{
									LineIndex++;
									PatternIndex++;
								}

								SearchPosition -= Distance[ToUpper(Line[SearchPosition - 1])];

								if(PatternIndex == PatternWidth)
								{
									Info->FoundX	= LineIndex - PatternWidth;
									Info->FoundY	= i;
									Info->Index		= SearchPosition;

									return(i);
								}
							}
							while(SearchPosition > 0);
						}

						LastSearchPosition = 0;
					}
				}
			}
			else
			{
				if(Info->Forward)
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= 0;
						LastSearchPosition	= 0;
					}
					else
					{
							/* Proceed to the next line. */

						if(!(LastSearchPosition = Info->Index))
							Info->FoundY = (Info->FoundY + 1) % Lines;
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i < Lines ; i++)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Where are we to start searching? */

							if(LastSearchPosition)
								SearchPosition = LastSearchPosition;
							else
								SearchPosition = PatternWidth;

							do
							{
									/* How many line characters
									 * match the pattern?
									 */

								PatternIndex	= PatternWidth - 1;
								LineIndex		= SearchPosition - 1;

								while(PatternIndex >= 0 && Pattern[PatternIndex] == Line[LineIndex])
								{
									LineIndex--;
									PatternIndex--;
								}

									/* Update the line search index
									 * for subsequent searches.
									 */

								SearchPosition += Distance[Line[SearchPosition - 1]];

									/* Found the pattern? */

								if(PatternIndex < 0)
								{
										/* Store the position. */

									Info->FoundX	= LineIndex + 1;
									Info->FoundY	= i;

										/* Remember column to start
										 * next search attempt at.
										 */

									if(SearchPosition <= LineWidth)
										Info->Index = SearchPosition;
									else
										Info->Index = 0;

									return(i);
								}
							}
							while(SearchPosition <= LineWidth);
						}

							/* Reset search column. */

						LastSearchPosition = 0;
					}
				}
				else
				{
						/* Update the search positions. */

					if(Info->FoundY == -1)
					{
						Info->FoundX			= 0;
						Info->FoundY			= Lines - 1;
						LastSearchPosition	= 0;
					}
					else
					{
						if((LastSearchPosition = Info->Index) < 1)
						{
							if(Info->FoundY)
								Info->FoundY--;
							else
								Info->FoundY = Lines - 1;
						}
					}

						/* Run down the buffer. */

					for(i = Info->FoundY ; i >= 0 ; i--)
					{
						Line = BufferLines[i];

							/* Is there anything to search for? */

						if((LineWidth = Line[-1]) >= PatternWidth)
						{
								/* Cast the magic spell of Boyer-Moore... */

							if(LastSearchPosition < 1)
								SearchPosition = LineWidth - (PatternWidth - 1);
							else
								SearchPosition = LastSearchPosition;

							do
							{
								PatternIndex = 0;
								LineIndex = SearchPosition - 1;

								while(PatternIndex < PatternWidth && Pattern[PatternIndex] == Line[LineIndex])
								{
									LineIndex++;
									PatternIndex++;
								}

								SearchPosition -= Distance[Line[SearchPosition - 1]];

								if(PatternIndex == PatternWidth)
								{
									Info->FoundX	= LineIndex - PatternWidth;
									Info->FoundY	= i;
									Info->Index	= SearchPosition;

									return(i);
								}
							}
							while(SearchPosition > 0);
						}

						LastSearchPosition = 0;
					}
				}
			}
		}
	}

	return(-1);
}

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
HistoryFunc(REG(a0) struct Hook *Hook,REG(a2) APTR Unused,REG(a1) STRPTR NewString)
#else
AROS_UFH3(STATIC ULONG, HistoryFunc,
 AROS_UFHA(struct Hook *  , Hook , A0),
 AROS_UFHA(APTR           , Unused, A2),
 AROS_UFHA(STRPTR         , NewString, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
	struct List *List = (struct List *)Hook->h_Data;

	if(NewString)
	{
		struct Node *Node = CreateNode(NewString);

		if(Node)
			AddTail(List,Node);
		else
			return(FALSE);
	}
	else
	{
		struct Node *Node = RemHead(List);

		FreeVecPooled(Node);
	}

	return(TRUE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

BOOL
HandleSearchMessage(struct SearchContext *Context,struct IntuiMessage **MessagePtr)
{
	struct IntuiMessage *Message;
	BOOL Done;

	Done = FALSE;

	if(Message = GT_FilterIMsg(*MessagePtr))
	{
		ULONG MsgClass,MsgQualifier;
		struct Gadget *MsgGadget;
		UWORD MsgCode;

		MsgClass			= Message->Class;
		MsgQualifier	= Message->Qualifier;
		MsgGadget		= (struct Gadget *)Message->IAddress;
		MsgCode			= Message->Code;

		LT_HandleInput(Context->SearchHandle,MsgQualifier,&MsgClass,&MsgCode,&MsgGadget);

		if(MsgClass == IDCMP_CLOSEWINDOW)
			Done = TRUE;

		if(MsgClass == IDCMP_GADGETUP)
		{
			switch(MsgGadget->GadgetID)
			{
				case GAD_STRING:

					if(MsgCode == '\r')
					{
						if(Context->LocalBuffer[0])
						{
							strcpy(Context->Buffer,Context->LocalBuffer);

							Context->Ok = TRUE;

							LT_PressButton(Context->SearchHandle,GAD_OK);
						}
						else
							LT_PressButton(Context->SearchHandle,GAD_CANCEL);

						Done = TRUE;
					}

					break;

				case GAD_OK:

					LT_UpdateStrings(Context->SearchHandle);

					if(Context->LocalBuffer[0])
					{
						strcpy(Context->Buffer,Context->LocalBuffer);

						Context->Ok = TRUE;
					}

					Done = TRUE;

					break;

				case GAD_CANCEL:

					Done = TRUE;

					break;
			}
		}

		GT_PostFilterIMsg(Message);
	}

	ReplyMsg((struct Message *)*MessagePtr);

	*MessagePtr = NULL;

	return(Done);
}

VOID
DeleteSearchContext(struct SearchContext *Context)
{
	if(Context)
	{
		LT_DeleteHandle(Context->SearchHandle);

		FreeVecPooled(Context);
	}
}

struct SearchContext *
CreateSearchContext(struct Window *ParentWindow,STRPTR Buffer,struct Hook *HistoryHook,struct List *HistoryHookList,BOOL *Forward,BOOL *IgnoreCase,BOOL *WholeWords)
{
	struct SearchContext *Context;

	if(Context = (struct SearchContext *)AllocVecPooled(sizeof(struct SearchContext),MEMF_ANY | MEMF_CLEAR))
	{
		struct LayoutHandle *Handle;

		Context->Buffer = Buffer;

		InitHook(HistoryHook,(HOOKFUNC)HistoryFunc,HistoryHookList);

		if(Handle = LT_CreateHandleTags(ParentWindow->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			struct Window *Window;

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,					STRING_KIND,
							LA_LabelID,				MSG_V36_0788,
							LA_STRPTR,				Context->LocalBuffer,
							LA_Chars,				30,
							LA_ID,					GAD_STRING,
							LAST_HistoryLines,	MAX(Config->CaptureConfig->SearchHistory,1),
							LAST_HistoryHook,		HistoryHook,
							GTST_MaxChars,			sizeof(Context->LocalBuffer) - 1,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,YBAR_KIND,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,	VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,	MSG_TERMBUFFER_SEARCH_FORWARD_TXT,
							LA_BYTE,		Forward,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,	MSG_TEXTBUFFER_IGNORE_CASE_GAD,
							LA_BYTE,		IgnoreCase,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,		CHECKBOX_KIND,
							LA_LabelID,	MSG_SEARCH_ONLY_WHOLE_WORDS_TXT,
							LA_BYTE,		WholeWords,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,			XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,LA_Type,HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,				BUTTON_KIND,
						LA_LabelID,			MSG_TERMXPR_OKAY_GAD,
						LA_ID,				GAD_OK,
						LABT_ReturnKey,	TRUE,
						LABT_ExtraFat,		TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,				BUTTON_KIND,
						LA_LabelID,			MSG_GLOBAL_CANCEL_GAD,
						LA_ID,				GAD_CANCEL,
						LABT_EscKey,		TRUE,
						LABT_ExtraFat,		TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			if(Window = LT_Build(Handle,
				LAWN_TitleID,		MSG_TERMBUFFER_ENTER_SEARCH_STRING_TXT,
				LAWN_IDCMP,			IDCMP_CLOSEWINDOW,
				LAWN_HelpHook,		&GuideHook,
				LAWN_Parent,		ParentWindow,
				LAWN_UserPort,		ParentWindow->UserPort,
				WA_DepthGadget,	TRUE,
				WA_CloseGadget,	TRUE,
				WA_DragBar,			TRUE,
				WA_RMBTrap,			TRUE,
				WA_Activate,		TRUE,
				WA_SimpleRefresh,	TRUE,
			TAG_DONE))
			{
				LT_Activate(Handle,GAD_STRING);

				Context->SearchHandle = Handle;
				Context->SearchWindow = Window;

				return(Context);
			}
		}

		FreeVecPooled(Context);
	}

	return(NULL);
}
