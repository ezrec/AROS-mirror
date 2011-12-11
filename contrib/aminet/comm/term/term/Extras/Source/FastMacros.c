/*
**	FastMacros.c
**
**	Fast! macros support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* CreateFastMacroNode(STRPTR Macro):
	 *
	 *	Create a new fast! macro node.
	 */

struct MacroNode *
CreateFastMacroNode(CONST_STRPTR Macro)
{
	struct MacroNode *Node;

	if(Node = (struct MacroNode *)AllocVecPooled(sizeof(struct MacroNode),MEMF_ANY|MEMF_CLEAR))
	{
		Node->Node.ln_Name = Node->Macro;

		if(Macro)
			LimitedStrcpy(sizeof(Node->Macro),Node->Macro,Macro);
	}

	return(Node);
}

	/* SaveFastMacros(STRPTR Name):
	 *
	 *	Save the fast! macro list to a file.
	 */

BOOL
SaveFastMacros(STRPTR Name,struct List *FastMacroList)
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
					else
						Error = PopChunk(Handle);
				}

				if(!Error)
					Error = PopChunk(Handle);
			}

			if(!Error)
			{
				struct MacroNode *Node;

				for(Node = (struct MacroNode *)FastMacroList->lh_Head ; !Error && Node->Node.ln_Succ ; Node = (struct MacroNode *)Node->Node.ln_Succ)
				{
					if(!(Error = PushChunk(Handle,ID_TERM,ID_FORM,IFFSIZE_UNKNOWN)))
					{
						if(!(Error = PushChunk(Handle,0,ID_FAST,IFFSIZE_UNKNOWN)))
						{
							if(WriteChunkBytes(Handle,Node->Macro,20) != 20)
								Error = IoErr();
							else
							{
								if(WriteChunkBytes(Handle,Node->Code,256) != 256)
									Error = IoErr();
							}

							if(!Error)
								Error = PopChunk(Handle);
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

	/* LoadFastMacros(STRPTR Name):
	 *
	 *	Restore the fast! macro list from a file.
	 */

BOOL
LoadFastMacros(STRPTR Name,struct List *FastMacroList)
{
	STATIC ULONG Stops[2 * 2] =
	{
		ID_TERM,ID_VERS,
		ID_TERM,ID_FAST
	};

	struct ContextNode *Chunk;
	struct IFFHandle *Handle;
	struct List *LocalList;
	struct MacroNode *Node;
	LONG Error;

	if(!(LocalList = CreateList()))
		Error = IoErr();
	else
	{
		if(!(Handle = OpenIFFStream(Name,MODE_OLDFILE)))
			Error = IoErr();
		else
		{
			if(!(Error = StopChunks(Handle,(LONG *)Stops,2)))
			{
				while(!Error && !ParseIFF(Handle,IFFPARSE_SCAN))
				{
					Chunk = CurrentChunk(Handle);

					if(Chunk->cn_ID == ID_VERS)
					{
						struct TermInfo TermInfo;

						if(ReadChunkBytes(Handle,&TermInfo,sizeof(struct TermInfo)) != sizeof(struct TermInfo))
							Error = IoErr();
						else
						{
							if((TermInfo.Version > CONFIG_FILE_VERSION) || (TermInfo.Version == CONFIG_FILE_VERSION && TermInfo.Revision > CONFIG_FILE_REVISION) || (TermInfo.Version == 1 && TermInfo.Revision < 6))
								Error = ERR_OUTDATED;
						}
					}

					if(!Error && Chunk->cn_ID == ID_FAST)
					{
						if(!(Node = CreateFastMacroNode(NULL)))
							Error = ERROR_NO_FREE_STORE;
						else
						{
							if(ReadChunkBytes(Handle,Node->Macro,20) != 20)
								Error = IoErr();
							else
							{
								if(ReadChunkBytes(Handle,Node->Code,256) != 256)
									Error = IoErr();
								else
									AddTail(LocalList,(struct Node *)Node);
							}
						}
					}
				}

				if(!Error && !IsListEmpty(LocalList))
				{
					FreeList(FastMacroList);

					MoveList(LocalList,FastMacroList);

					FastMacroCount = GetListSize(FastMacroList);
				}
			}

			CloseIFFStream(Handle);
		}

		DeleteList(LocalList);
	}

	if(Error)
	{
		SetIoErr(Error);
		return(FALSE);
	}
	else
		return(TRUE);
}
