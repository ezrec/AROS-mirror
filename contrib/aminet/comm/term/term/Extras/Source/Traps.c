/*
**	Traps.c
**
**	Trap routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

struct TrapNode *
ChangeTrapNode(struct GenericList *TrapList,struct TrapNode *Current,STRPTR Sequence,STRPTR Command)
{
	struct TrapNode *New;

	if(!Sequence)
		Sequence = Current->Node.ln_Name;

	if(!Command)
		Command = Current->Command;

	if(New = CreateTrapNode(Sequence,Command))
	{
		Insert((struct List *)TrapList,(struct Node *)New,(struct Node *)Current);

		Remove((struct Node *)Current);

		FreeVecPooled(Current);

		return(New);
	}
	else
		return(Current);
}

struct TrapNode *
CreateTrapNode(STRPTR SequenceBuffer,STRPTR CommandBuffer)
{
	UBYTE LocalBuffer[256];
	struct TrapNode	*Node;
	LONG Len;

	Len = TranslateString(SequenceBuffer,LocalBuffer);

	if(Node = (struct TrapNode *)AllocVecPooled(sizeof(struct TrapNode) + strlen(SequenceBuffer) + 1 + Len + strlen(CommandBuffer) + 1,MEMF_ANY))
	{
		STRPTR String;

		String = Node->Node.ln_Name = (STRPTR)(Node + 1);

		strcpy(String,SequenceBuffer);

		String += strlen(String) + 1;

		Node->Sequence = String;

		CopyMem(LocalBuffer,String,Len);

		String += Len;

		Node->Command = String;

		Node->SequenceLen = Len;
		Node->Count = 0;

		strcpy(String,CommandBuffer);
	}

	return(Node);
}

	/* SaveTraps(STRPTR Name):
	 *
	 *	Save the trap list to a file.
	 */

BOOL
SaveTraps(STRPTR Name,struct List *TrapList)
{
	struct IFFHandle *Handle;
	LONG Error;

	if(!(Handle = OpenIFFStream(Name,MODE_NEWFILE)))
		Error = IoErr();
	else
	{
		if(!(Error = PushChunk(Handle,ID_TERM,ID_CAT,IFFSIZE_UNKNOWN)))
		{
			if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
			{
				if(!(Error = PushChunk(Handle,0,ID_VERS,IFFSIZE_UNKNOWN)))
				{
					struct TermInfo TermInfo;

					TermInfo.Version	= CONFIG_FILE_VERSION;
					TermInfo.Revision	= CONFIG_FILE_REVISION;

					if(WriteChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) != sizeof(struct TermInfo))
						Error = IoErr();

					if(!Error)
						Error = PopChunk(Handle);
				}

				if(!Error)
				{
					if(!(Error = PushChunk(Handle,0,ID_TRST,sizeof(struct TrapSettings))))
					{
						struct TrapSettings TrapSettings;

						memset(&TrapSettings,0,sizeof(TrapSettings));

						TrapSettings.Enabled = WatchTraps;

						if(WriteChunkBytes(Handle,&TrapSettings,sizeof(struct TrapSettings)) != sizeof(struct TrapSettings))
							Error = IoErr();

						if(!Error)
							Error = PopChunk(Handle);
					}
				}

				if(!Error)
					Error = PopChunk(Handle);
			}

			if(!Error)
			{
				struct TrapNode *Node;

				for(Node = (struct TrapNode *)TrapList->lh_Head ; !Error && Node->Node.ln_Succ ; Node = (struct TrapNode *)Node->Node.ln_Succ)
				{
					if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
					{
						if(!(Error = PushChunk(Handle,0,ID_SEQN,strlen(Node->Node.ln_Name))))
						{
							if(WriteChunkBytes(Handle,Node->Node.ln_Name,strlen(Node->Node.ln_Name)) != strlen(Node->Node.ln_Name))
								Error = IoErr();

							if(!Error)
								Error = PopChunk(Handle);
						}

						if(!Error)
						{
							if(!(Error = PushChunk(Handle,0,ID_TRAP,strlen(Node->Command))))
							{
								if(WriteChunkBytes(Handle,Node->Command,strlen(Node->Command)) != strlen(Node->Command))
									Error = IoErr();

								if(!Error)
									Error = PopChunk(Handle);
							}
						}

						if(!Error)
							Error = PopChunk(Handle);
					}
				}
			}

			if(!Error)
				Error = PopChunk(Handle);
		}

		CloseIFFStream(Handle);
	}

	if(Error)
	{
		DeleteFile(Name);
		SetIoErr(Error);

		return(FALSE);
	}
	else
	{
		AddProtection(Name,FIBF_EXECUTE);

		return(TRUE);
	}
}

	/* LoadTraps(STRPTR Name):
	 *
	 *	Restore the trap list from a file.
	 */

BOOL
LoadTraps(STRPTR Name,struct GenericList *TrapList)
{
	STATIC ULONG Stops[8] =
	{
		ID_TERM,ID_VERS,
		ID_TERM,ID_SEQN,
		ID_TERM,ID_TRAP,
		ID_TERM,ID_TRST
	};

	UBYTE SequenceBuffer[256],CommandBuffer[256];
	struct TrapSettings	TrapSettings;
	struct ContextNode *Chunk;
	struct IFFHandle *Handle;
	BOOL TrapSettingsFound;
	LONG Error;

	TrapSettingsFound = FALSE;

	if(!(Handle = OpenIFFStream(Name,MODE_OLDFILE)))
		Error = IoErr();
	else
	{
		if(!(Error = StopChunks(Handle,(LONG *)Stops,4)))
		{
			SequenceBuffer[0] = CommandBuffer[0] = 0;

			while(!ParseIFF(Handle,IFFPARSE_SCAN))
			{
				Chunk = CurrentChunk(Handle);

				if(Chunk->cn_ID == ID_VERS)
				{
					struct TermInfo TermInfo;

					if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) == sizeof(struct TermInfo))
					{
						if((TermInfo.Version > CONFIG_FILE_VERSION) || (TermInfo.Version == CONFIG_FILE_VERSION && TermInfo.Revision > CONFIG_FILE_REVISION) || (TermInfo.Version == 1 && TermInfo.Revision < 6))
						{
							Error = ERR_OUTDATED;

							break;
						}
					}
					else
					{
						Error = IoErr();

						break;
					}
				}

				if(Chunk->cn_ID == ID_TRST)
				{
					if(ReadChunkBytes(Handle,&TrapSettings,sizeof(struct TrapSettings)) == sizeof(struct TrapSettings))
						TrapSettingsFound = TRUE;
					else
					{
						Error = IoErr();

						break;
					}
				}

				if(Chunk->cn_ID == ID_SEQN)
				{
					if(ReadChunkBytes(Handle,SequenceBuffer,Chunk->cn_Size) == Chunk->cn_Size)
						SequenceBuffer[Chunk->cn_Size] = 0;
					else
					{
						Error = IoErr();

						break;
					}
				}

				if(Chunk->cn_ID == ID_TRAP)
				{
					if(ReadChunkBytes(Handle,CommandBuffer,Chunk->cn_Size) == Chunk->cn_Size)
						CommandBuffer[Chunk->cn_Size] = 0;
					else
					{
						Error = IoErr();

						break;
					}
				}

				if(!Error && SequenceBuffer[0] && CommandBuffer[0])
				{
					struct TrapNode *Node;

					if(Node = CreateTrapNode(SequenceBuffer,CommandBuffer))
					{
						AddGenericListNode(TrapList,(struct Node *)Node,ADD_GLIST_BOTTOM,FALSE);

						SequenceBuffer[0] = CommandBuffer[0] = 0;
					}
					else
					{
						Error = ERROR_NO_FREE_STORE;

						break;
					}
				}
			}
		}

		CloseIFFStream(Handle);
	}

	if(Error)
	{
		SetIoErr(Error);

		return(FALSE);
	}
	else
	{
		TrapsChanged = FALSE;

		if(TrapSettingsFound)
			WatchTraps = TrapSettings.Enabled;

		if(!TrapList->ListCount && WatchTraps)
			WatchTraps = FALSE;

		return(TRUE);
	}
}

	/* TrapFilter(STRPTR Data,LONG Size):
	 *
	 *	Handle the trap list, similar to FlowFilter().
	 */

VOID
TrapFilter(STRPTR Data,LONG Size)
{
	if(Size)
	{
		STATIC LONG WaitCount = 0;

		struct List	*List = (struct List *)&GenericListTable[GLIST_TRAP]->ListHeader;
		struct TrapNode	*Node;

		LONG c,Mask;

		LockGenericList(GenericListTable[GLIST_TRAP]);

		if(Config->SerialConfig->StripBit8)
			Mask = 0x7F;
		else
			Mask = 0xFF;

		do
		{
			BOOL MatchMade;

			c = (*Data++) & Mask;

			do
			{
				MatchMade = FALSE;

				for(Node = (struct TrapNode *)List->lh_Head ; Node->Node.ln_Succ ; Node = (struct TrapNode *)Node->Node.ln_Succ)
				{
					if(Node->Count == WaitCount)
					{
						if(c == (Node->Sequence[WaitCount] & Mask))
						{
							MatchMade = TRUE;

							if(++Node->Count == Node->SequenceLen)
							{
								struct DataMsg *Msg;

								Node->Count = 0;

								if(Msg = (struct DataMsg *)CreateMsgItem(sizeof(struct DataMsg) + strlen(Node->Command) + 1))
								{
									Msg->Type = DATAMSGTYPE_SERIALCOMMAND;
									Msg->Data = (STRPTR)(Msg + 1);

									strcpy(Msg->Data,Node->Command);

									PutMsgItem(SpecialQueue,(struct MsgItem *)Msg);
								}
							}
						}
					}
				}

				if(MatchMade)
					WaitCount++;
				else
				{
					if(WaitCount)
					{
						WaitCount = 0;

						for(Node = (struct TrapNode *)List->lh_Head ; Node->Node.ln_Succ ; Node = (struct TrapNode *)Node->Node.ln_Succ)
							Node->Count = 0;
					}
					else
						break;
				}
			}
			while(!WaitCount);
		}
		while(--Size);

		UnlockGenericList(GenericListTable[GLIST_TRAP]);
	}
}
