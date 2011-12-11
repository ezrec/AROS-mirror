/*
**	TimeDate.c
**
**	Phone rate calculation support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
** :ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

VOID
SetActivePattern(struct List *Pattern)
{
	ObtainSemaphore(&PatternSemaphore);

	ActivePattern = Pattern;

	ReleaseSemaphore(&PatternSemaphore);
}

struct List *
GetActivePattern()
{
	struct List *Pattern;

	SafeObtainSemaphoreShared(&PatternSemaphore);

	Pattern = ActivePattern;

	ReleaseSemaphore(&PatternSemaphore);

	return(Pattern);
}

VOID
UnlockActivePattern()
{
	ReleaseSemaphore(&PatternSemaphore);
}

struct List *
LockActivePattern()
{
	SafeObtainSemaphoreShared(&PatternSemaphore);

	return(ActivePattern);
}

	/* FreeTimeDateNode(struct TimeDateNode *Node):
	 *
	 *	Free the memory allocated for a TimeDateNode and
	 *	the associated data table.
	 */

VOID
FreeTimeDateNode(struct TimeDateNode *Node)
{
	if(Node)
	{
		FreeVecPooled(Node->Table);
		FreeVecPooled(Node);
	}
}

	/* FreeTimeDateList(struct List *List):
	 *
	 *	Free a list of TimeDateNodes.
	 */

VOID
FreeTimeDateList(struct List *List)
{
	if(List)
	{
		struct Node *Node;

		while(Node = RemHead(List))
			FreeTimeDateNode((struct TimeDateNode *)Node);
	}
}

	/* CopyTimeDateList(struct List *From,struct List *To,BOOL SkipFirst):
	 *
	 *	Copies the TimeDateNode list from one location into
	 *	another.
	 */

BOOL
CopyTimeDateList(struct List *From,struct List *To,BOOL SkipFirst)
{
	if(!IsListEmpty(From))
	{
		struct TimeDateNode *FromNode,*ToNode;
		struct List OtherList;

		NewList(&OtherList);

		FromNode = (struct TimeDateNode *)From->lh_Head;

		if(SkipFirst)
			FromNode = (struct TimeDateNode *)FromNode->Node.ln_Succ;

		for( ; FromNode->Node.ln_Succ ; FromNode = (struct TimeDateNode *)FromNode->Node.ln_Succ)
		{
			if(ToNode = (struct TimeDateNode *)AllocVecPooled(sizeof(struct TimeDateNode),MEMF_ANY))
			{
				CopyMem(FromNode,ToNode,sizeof(struct TimeDateNode));

				ToNode->Node.ln_Name = ToNode->Buffer;

				if(ToNode->Table = (struct TimeDate *)AllocVecPooled(sizeof(struct TimeDate) * FromNode->Table[0].Count,MEMF_ANY))
				{
					CopyMem(FromNode->Table,ToNode->Table,sizeof(struct TimeDate) * FromNode->Table[0].Count);

					AddTail(&OtherList,&ToNode->Node);
				}
				else
				{
					FreeVecPooled(ToNode);

					FreeList(&OtherList);

					return(FALSE);
				}
			}
			else
			{
				FreeList(&OtherList);

				return(FALSE);
			}
		}

		MoveList(&OtherList,To);
	}

	return(TRUE);
}

	/* AdaptTimeDateNode(struct TimeDateNode *Node):
	 *
	 *	Adapt the title and comment of a TimeDateNode.
	 */

VOID
AdaptTimeDateNode(struct TimeDateNode *Node)
{
	CONST_STRPTR Comment = Node->Header.Comment[0] ? Node->Header.Comment : LocaleString(MSG_TERMPHONE_NO_COMMENT_TXT);

	if(Node->Header.Month == -1)
	{
		if(Node->Header.Day == -1)
			strcpy(Node->Buffer,LocaleString(MSG_TERMPHONE_STANDARD_SETTINGS_TXT));
		else
			LimitedSPrintf(sizeof(Node->Buffer),Node->Buffer,LocaleString(MSG_TERMPHONE_DAYS_TXT),Comment);
	}
	else
		LimitedSPrintf(sizeof(Node->Buffer),Node->Buffer,"%2ld %s » %s",Node->Header.Day,LocaleString(MSG_TERMPHONE_JAN_TXT + Node->Header.Month),Comment);
}

	/* TimeCompare(struct TimeDate *A,struct TimeDate *B):
	 *
	 *	Comparison routine required by SortTimeTable().
	 */

STATIC LONG
TimeCompare(struct TimeDate *A,struct TimeDate *B)
{
	return(((LONG)A->Time) - ((LONG)B->Time));
}

	/* SortTimeTable(struct TimeDateNode *Node):
	 *
	 *	Sort the time table associated with a
	 *	TimeDateNode in ascending order.
	 */

VOID
SortTimeTable(struct TimeDateNode *Node)
{
	qsort(Node->Table,Node->Table[0].Count,sizeof(struct TimeDate),(SORTFUNC)TimeCompare);
}

	/* BuildTimeList(struct TimeDateNode *Node):
	 *
	 *	Build a read-to-display time table list from a TimeDateNode.
	 */

struct List *
BuildTimeList(struct TimeDateNode *Node)
{
	struct List *List;

	if(List = (struct List *)AllocVecPooled(sizeof(struct List) + Node->Table[0].Count * sizeof(struct TimeNode),MEMF_ANY | MEMF_CLEAR))
	{
		struct TimeNode	*Time = (struct TimeNode *)(List + 1);
		UBYTE			 LocalBuffer[40];
		LONG			 i;

		NewList(List);

		for(i = 0 ; i < Node->Table[0].Count ; i++)
		{
			Time[i].Node.ln_Name	= Time[i].Name;
			Time[i].Time			= Node->Table[i].Time;

			FormatTime(LocalBuffer,sizeof(LocalBuffer),Time[i].Time / 6,(Time[i].Time % 6) * 10,-1);

			LocalBuffer[19] = 0;

			strcpy(Time[i].Name,LocalBuffer);

			AddTail(List,(struct Node *)&Time[i]);
		}
	}

	return(List);
}

	/* ResizeTimeDateNode(struct TimeDateNode *Node,LONG Count,UBYTE Time):
	 *
	 *	Resize the time table associated with a TimeDateNode.
	 */

BOOL
ResizeTimeDateNode(struct TimeDateNode *Node,LONG Count,UBYTE Time)
{
	if(Count != Node->Table[0].Count)
	{
		struct TimeDate *Table;

		if(Table = (struct TimeDate *)AllocVecPooled(sizeof(struct TimeDate) * Count,MEMF_ANY | MEMF_CLEAR))
		{
			LONG i;

			CopyMem(Node->Table,Table,sizeof(struct TimeDate) * Count);

			if(Count > Node->Table[0].Count)
			{
				for(i = Node->Table[0].Count ; i < Count ; i++)
				{
					CopyMem(&Node->Table[0],&Table[i],sizeof(struct TimeDate));

					Table[i].Time = Time;
				}
			}

			for(i = 0 ; i < Count ; i++)
				Table[i].Count = Count;

			FreeVecPooled(Node->Table);

			Node->Table = Table;

			return(TRUE);
		}
		else
			return(FALSE);
	}
	else
		return(TRUE);
}

	/* DeleteTimeDateNode(struct TimeDateNode *Node,LONG Index):
	 *
	 *	Delete a single timetable entry from a TimeDateNode.
	 */

BOOL
DeleteTimeDateNode(struct TimeDateNode *Node,LONG Index)
{
	struct TimeDate	*Table;
	LONG			 Count = Node->Table[0].Count - 1;

	if(Table = (struct TimeDate *)AllocVecPooled(sizeof(struct TimeDate) * Count,MEMF_ANY | MEMF_CLEAR))
	{
		LONG i,j;

		for(i = j = 0 ; i < Node->Table[0].Count ; i++)
		{
			if(i != Index)
			{
				CopyMem(&Node->Table[i],&Table[j],sizeof(struct TimeDate));

				Table[j++].Count = Count;
			}
		}

		FreeVecPooled(Node->Table);

		Node->Table = Table;

		return(TRUE);
	}
	else
		return(FALSE);
}

	/* CreateTimeDateNode(BYTE Month,BYTE Day,STRPTR Comment,LONG Count):
	 *
	 *	Create a new TimeDateNode including time table entries.
	 */

struct TimeDateNode *
CreateTimeDateNode(LONG Month,LONG Day,STRPTR Comment,LONG Count)
{
	struct TimeDateNode *Node;

	if(Node = (struct TimeDateNode *)AllocVecPooled(sizeof(struct TimeDateNode),MEMF_ANY | MEMF_CLEAR))
	{
		if(Node->Table = (struct TimeDate *)AllocVecPooled(sizeof(struct TimeDate) * Count,MEMF_ANY | MEMF_CLEAR))
		{
			Node->Node.ln_Name = Node->Buffer;

			Node->Header.Month	= Month;
			Node->Header.Day	= Day;

			Node->Table[0].PayPerUnit[DT_FIRST_UNIT]	= 0;
			Node->Table[0].SecPerUnit[DT_FIRST_UNIT]	= 0;
			Node->Table[0].PayPerUnit[DT_NEXT_UNIT]		= 0;
			Node->Table[0].SecPerUnit[DT_NEXT_UNIT]		= 0;
			Node->Table[0].Time							= DT_GET_TIME( 0,0);
			Node->Table[0].Count						= Count;

			if(Count > 1)
			{
				Node->Table[1].PayPerUnit[DT_FIRST_UNIT]	= 0;
				Node->Table[1].SecPerUnit[DT_FIRST_UNIT]	= 0;
				Node->Table[1].PayPerUnit[DT_NEXT_UNIT]		= 0;
				Node->Table[1].SecPerUnit[DT_NEXT_UNIT]		= 0;
				Node->Table[1].Time							= DT_GET_TIME(12,0);
				Node->Table[1].Count						= Count;
			}

			strcpy(Node->Header.Comment,Comment);

			AdaptTimeDateNode(Node);

			return(Node);
		}
		else
			FreeVecPooled(Node);
	}

	return(NULL);
}

/****************************************************************************/

	/* SelectTime():
	 *
	 *	Searches for the correct unit/pay conversion values.
	 */

VOID
SelectTime(struct PhoneEntry *SomeEntry,struct List *List,struct timeval *TimeVal)
{
	struct TimeDateNode	*TimeDateNode;
	struct ClockData	 ClockData;
	LONG				 Time;
	BOOL				 FoundOne = FALSE;
	LONG				 i,Last;

		/* If we get a specific time of day, use it. */

	if(TimeVal)
		Amiga2Date(TimeVal->tv_secs,&ClockData);
	else
	{
		struct timeval Now;

			/* Obtain current time and date. */

		GetSysTime(&Now);

			/* Convert into a more suitable form (note: seconds are
			 * required as an input value, ice is extra).
			 */

		Amiga2Date(Now.tv_secs,&ClockData);
	}

		/* Apparently, in the US of A the week starts with Sunday, we'll
		 * wrap the week around to let it start with Monday.
		 */

	if(ClockData.wday)
		ClockData.wday--;
	else
		ClockData.wday = 6;

		/* Change the month, too... */

	ClockData.month--;

	if(List)
	{
		if(IsListEmpty(List))
			List = NULL;
	}

		/* If no special list to search is given use the phonebook entry. */

	if(!List)
	{
		if(SomeEntry && !SomeEntry->Header->NoRates)
			List = (struct List *)&SomeEntry->TimeDateList;
		else
		{
			PayPerUnit[DT_FIRST_UNIT]	= 0;
			PayPerUnit[DT_NEXT_UNIT]	= 0;
			SecPerUnit[DT_FIRST_UNIT]	= 0;
			SecPerUnit[DT_NEXT_UNIT]	= 0;

			return;
		}
	}

	if(IsListEmpty(List))
	{
		PayPerUnit[DT_FIRST_UNIT]	= 0;
		PayPerUnit[DT_NEXT_UNIT]	= 0;
		SecPerUnit[DT_FIRST_UNIT]	= 0;
		SecPerUnit[DT_NEXT_UNIT]	= 0;

		return;
	}

		/* First step: search for current day. */

	TimeDateNode = (struct TimeDateNode *)List->lh_Head;

		/* Skip first entry. */

	TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ;

		/* First step: search for date settings. */

	for( ; TimeDateNode->Node.ln_Succ ; TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ)
	{
			/* Does it match a specific date? */

		if(TimeDateNode->Header.Month == ClockData.month && TimeDateNode->Header.Day == ClockData.mday)
		{
			FoundOne = TRUE;

			break;
		}
	}

		/* Second step: search for week day settings. */

	if(!FoundOne)
	{
		TimeDateNode = (struct TimeDateNode *)List->lh_Head;

			/* Skip first entry. */

		TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ;

		for( ; TimeDateNode->Node.ln_Succ ; TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ)
		{
				/* Does it match a specific day? */

			if(TimeDateNode->Header.Month == -1 && (TimeDateNode->Header.Day & (1L << ClockData.wday)))
			{
				FoundOne = TRUE;

				break;
			}
		}
	}

		/* Third step: use default settings. */

	if(!FoundOne)
		TimeDateNode = (struct TimeDateNode *)List->lh_Head;

		/* Convert current time to packed format. */

	Time = DT_GET_TIME(ClockData.hour,ClockData.min);

		/* Start with a blank. */

	Last = -1;

		/* Look for fitting time. */

	for(i = 0 ; i < TimeDateNode->Table[0].Count ; i++)
	{
			/* The time we are looking for must be >= the
			 * current time.
			 */

		if(TimeDateNode->Table[i].Time > Time)
		{
			if(i == 0)
				break;
			else
			{
				Last = i - 1;
				break;
			}
		}
		else
			continue;
	}

		/* If none is found, take the last one in the list.
		 * Note that this requires the list to be sorted in
		 * ascending order.
		 */

	if(Last == -1)
		Last = TimeDateNode->Table[0].Count - 1;

		/* Fill in the remaining data. */

	PayPerUnit[DT_FIRST_UNIT]	= TimeDateNode->Table[Last].PayPerUnit[DT_FIRST_UNIT];
	PayPerUnit[DT_NEXT_UNIT]	= TimeDateNode->Table[Last].PayPerUnit[DT_NEXT_UNIT];
	SecPerUnit[DT_FIRST_UNIT]	= TimeDateNode->Table[Last].SecPerUnit[DT_FIRST_UNIT];
	SecPerUnit[DT_NEXT_UNIT]	= TimeDateNode->Table[Last].SecPerUnit[DT_NEXT_UNIT];
}

struct List *
FindTimeDate(struct List *Patterns,STRPTR Number)
{
	struct PatternNode	*Node;
	UBYTE				 Dst[2 * 256 + 2];

	if(Kick30)
	{
		for(Node = (struct PatternNode *)Patterns->lh_Head ; Node->Node.ln_Succ ; Node = (struct PatternNode *)Node->Node.ln_Succ)
		{
			if(ParsePatternNoCase(Node->Pattern,Dst,sizeof(Dst)) != -1)
			{
				if(MatchPatternNoCase(Dst,Number))
					return((struct List *)&Node->List);
			}
		}
	}
	else
	{
		UBYTE	Src[256];
		STRPTR	Index;

		for(Node = (struct PatternNode *)Patterns->lh_Head ; Node->Node.ln_Succ ; Node = (struct PatternNode *)Node->Node.ln_Succ)
		{
			strcpy(Index = Src,Node->Pattern);

				/* Work around a bug in Kickstart 2.04 which involves character classes in
				 * wildcard patterns.
				 */

			while(*Index)
			{
				*Index = ToUpper(*Index);

				Index++;
			}

			if(ParsePatternNoCase(Src,Dst,sizeof(Dst)) != -1)
			{
				if(MatchPatternNoCase(Dst,Number))
					return((struct List *)&Node->List);
			}
		}
	}

	return(NULL);
}

VOID
DeletePatternNode(struct PatternNode *Pattern)
{
	if(Pattern)
	{
		struct TimeDateNode	*Node,
							*NextNode;

		for(Node = (struct TimeDateNode *)Pattern->List.mlh_Head ; NextNode = (struct TimeDateNode *)Node->Node.ln_Succ ; Node = NextNode)
			FreeTimeDateNode(Node);

		FreeVecPooled(Pattern);
	}
}

struct PatternNode *
CreatePatternNode(STRPTR Comment)
{
	struct PatternNode *Node;

	if(Node = (struct PatternNode *)AllocVecPooled(sizeof(struct PatternNode),MEMF_ANY | MEMF_CLEAR))
	{
		struct TimeDateNode *TimeDateNode;

		Node->Node.ln_Name = Node->Comment;

		strcpy(Node->Comment,Comment);

		NewList((struct List *)&Node->List);

		if(TimeDateNode = CreateTimeDateNode(-1,-1,"",2))
		{
			AddTail((struct List *)&Node->List,&TimeDateNode->Node);

			return(Node);
		}

		FreeVecPooled(Node);
	}

	return(NULL);
}

VOID
DeletePatternList(struct List *List)
{
	if(List)
	{
		FreePatternList(List);

		FreeVecPooled(List);
	}
}

VOID
FreePatternList(struct List *List)
{
	if(List)
	{
		struct PatternNode	*Pattern,
							*NextPattern;

		for(Pattern = (struct PatternNode *)List->lh_Head ; NextPattern = (struct PatternNode *)Pattern->Node.ln_Succ ; Pattern = NextPattern)
			DeletePatternNode(Pattern);

		NewList(List);
	}
}

VOID
ConvertTimeDate(struct TimeDateOld *Old,struct TimeDate *New)
{
	memset(New,0,sizeof(struct TimeDate));

	New->Count			= Old->Count;
	New->PayPerUnit[0]	= ((ULONG)Old->PayPerUnit[0]) * 10000;
	New->PayPerUnit[1]	= ((ULONG)Old->PayPerUnit[1]) * 10000;
	New->SecPerUnit[0]	= ((ULONG)Old->SecPerUnit[0]) * 10000;
	New->SecPerUnit[1]	= ((ULONG)Old->SecPerUnit[1]) * 10000;
	New->Time			= Old->Time;
	New->Mark			= FALSE;
}

struct List *
LoadTimeDateList(STRPTR Name,LONG *Error)
{
	struct IFFHandle	*Handle;
	struct List			*List = NULL;

	*Error = 0;

	if(Handle = OpenIFFStream(Name,MODE_OLDFILE))
	{
		STATIC LONG Stops[4 * 2] =
		{
			ID_TERM, ID_VERS,
			ID_TERM, ID_NAME,
			ID_TERM, ID_DAT2,
			ID_TERM, ID_DATE
		};

		struct ContextNode *Chunk;

		if(!(*Error = StopChunks(Handle,Stops,4)))
		{
			if(List = CreateList())
			{
				struct PatternNode *Pattern = NULL;

				while(!ParseIFF(Handle,IFFPARSE_SCAN))
				{
					Chunk = CurrentChunk(Handle);

					if(Chunk->cn_ID == ID_NAME)
					{
						if(Pattern = (struct PatternNode *)AllocVecPooled(sizeof(struct PatternNode),MEMF_ANY | MEMF_CLEAR))
						{
							if(ReadChunkBytes(Handle,Pattern->Pattern,Chunk->cn_Size) == Chunk->cn_Size)
							{
								NewList((struct List *)&Pattern->List);

								Pattern->Node.ln_Name = Pattern->Comment;

								AddTail(List,(struct Node *)Pattern);
							}
							else
							{
								*Error = IoErr();

								FreeVecPooled(Pattern);

								break;
							}
						}
						else
						{
							*Error = ERROR_NO_FREE_STORE;

							break;
						}
					}

					if(Chunk->cn_ID == ID_DATE)
					{
						struct TimeDateNode	*Node;
						LONG				 Count = (Chunk->cn_Size - sizeof(struct TimeDateHeader)) / sizeof(struct TimeDateOld);

						if(!Pattern)
						{
							*Error = IFFERR_MANGLED;

							break;
						}

						if(Node = CreateTimeDateNode(-1,-1,"",Count))
						{
							struct TimeDateOld *Old;

							if(Old = (struct TimeDateOld *)AllocVecPooled(sizeof(struct TimeDateOld) * Count,MEMF_ANY))
							{
								if(ReadChunkBytes(Handle,&Node->Header,sizeof(struct TimeDateHeader)) == sizeof(struct TimeDateHeader))
								{
									if(ReadChunkRecords(Handle,Old,sizeof(struct TimeDateOld),Count) == Count)
									{
										LONG i;

										for(i = 0 ; i < Count ; i++)
											ConvertTimeDate(&Old[i],&Node->Table[i]);

										AdaptTimeDateNode(Node);

										AddTail((struct List *)&Pattern->List,&Node->Node);

										FreeVecPooled(Old);
									}
									else
									{
										*Error = IoErr();

										FreeTimeDateNode(Node);
										FreeVecPooled(Old);

										break;
									}
								}
								else
								{
									*Error = IoErr();

									FreeTimeDateNode(Node);
									FreeVecPooled(Old);

									break;
								}
							}
						}
						else
						{
							*Error = ERROR_NO_FREE_STORE;
							break;
						}
					}

					if(Chunk->cn_ID == ID_DAT2)
					{
						struct TimeDateNode	*Node;
						LONG				 Count = (Chunk->cn_Size - sizeof(struct TimeDateHeader)) / sizeof(struct TimeDate);

						if(!Pattern)
						{
							*Error = IFFERR_MANGLED;

							break;
						}

						if(Node = CreateTimeDateNode(-1,-1,"",Count))
						{
							if(ReadChunkBytes(Handle,&Node->Header,sizeof(struct TimeDateHeader)) == sizeof(struct TimeDateHeader))
							{
								if(ReadChunkRecords(Handle,Node->Table,sizeof(struct TimeDate),Count) == Count)
								{
									AdaptTimeDateNode(Node);

									AddTail((struct List *)&Pattern->List,&Node->Node);
								}
								else
								{
									*Error = IoErr();

									FreeTimeDateNode(Node);

									break;
								}
							}
							else
							{
								*Error = IoErr();

								FreeTimeDateNode(Node);

								break;
							}
						}
						else
						{
							*Error = ERROR_NO_FREE_STORE;
							break;
						}
					}
				}
			}
		}

		CloseIFFStream(Handle);
	}
	else
		*Error = IoErr();

	if(*Error)
	{
		DeletePatternList(List);

		return(NULL);
	}
	else
		return(List);
}

BOOL
SaveTimeDateList(STRPTR Name,struct List *List,LONG *Error)
{
	*Error = 0;

	if(!IsListEmpty(List))
	{
		struct IFFHandle *Handle;

		if(Handle = OpenIFFStream(Name,MODE_NEWFILE))
		{
			if(!(*Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
			{
				if(!(*Error = PushChunk(Handle,0,ID_VERS,IFFSIZE_UNKNOWN)))
				{
					struct TermInfo TermInfo;

					TermInfo.Version	= CONFIG_FILE_VERSION;
					TermInfo.Revision	= CONFIG_FILE_REVISION;

					if(WriteChunkRecords(Handle,&TermInfo,sizeof(struct TermInfo),1) == 1)
					{
						if(!(*Error = PopChunk(Handle)))
						{
							struct PatternNode *Node;

							for(Node = (struct PatternNode *)List->lh_Head ; !(*Error) && Node->Node.ln_Succ ; Node = (struct PatternNode *)Node->Node.ln_Succ)
							{
								if(!(*Error = PushChunk(Handle,0,ID_NAME,296)))
								{
									if(WriteChunkBytes(Handle,Node->Pattern,296) == 296)
									{
										if(!(*Error = PopChunk(Handle)))
										{
											struct TimeDateNode *TimeDateNode;

											for(TimeDateNode = (struct TimeDateNode *)Node->List.mlh_Head ; TimeDateNode->Node.ln_Succ ; TimeDateNode = (struct TimeDateNode *)TimeDateNode->Node.ln_Succ)
											{
												if(!(*Error = PushChunk(Handle,0,ID_DAT2,sizeof(struct TimeDateHeader) + TimeDateNode->Table[0].Count * sizeof(struct TimeDate))))
												{
													if(WriteChunkBytes(Handle,&TimeDateNode->Header,sizeof(struct TimeDateHeader)) != sizeof(struct TimeDateHeader))
													{
														*Error = IoErr();

														break;
													}
													else
													{
														if(WriteChunkBytes(Handle,TimeDateNode->Table,TimeDateNode->Table[0].Count * sizeof(struct TimeDate)) != TimeDateNode->Table[0].Count * sizeof(struct TimeDate))
														{
															*Error = IoErr();

															break;
														}
														else
														{
															if(*Error = PopChunk(Handle))
																break;
														}
													}
												}
												else
													break;
											}
										}
									}
									else
										*Error = IoErr();
								}
							}
						}
					}
				}

				if(!(*Error))
					*Error = PopChunk(Handle);
			}

			if(*Error)
				DeleteFile(Name);
			else
				AddProtection(Name,FIBF_EXECUTE);

			CloseIFFStream(Handle);
		}
		else
			*Error = IoErr();
	}

	if(*Error)
		return(FALSE);
	else
		return(TRUE);
}
