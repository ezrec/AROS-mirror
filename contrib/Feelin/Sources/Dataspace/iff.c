#include "Private.h"

//#define DB_WRITEIFF

///DS_WriteIFF
F_METHODM(LONG,DS_WriteIFF,FS_Dataspace_WriteIFF)
{
	struct LocalObjectData *LOD   = F_LOD(Class,Obj);

	APTR next;
	struct FeelinChunkData *chunk;

	PushChunk(Msg -> IFF,Msg -> Type,Msg -> ID,IFFSIZE_UNKNOWN);

	next = LOD -> Chunks.Head;

///DB_WRITEIFF
#ifdef DB_WRITEIFF
	if (!next)
	{
		F_DebugOut("DS.WriteIFF() - Dataspace Empty\n");
	}
#endif
//+

	while ((chunk = F_NextNode(&next)) != NULL)
	{
///DB_WRITEIFF
#ifdef DB_WRITEIFF
		ULONG *array = (ULONG *)((ULONG)(chunk) + sizeof (struct FeelinChunkData));
		F_Log(0,"Chunk 0x%lx - ID %08.lx (%4.4s) - Data: %08lx%08lx%08lx%08lx",chunk,chunk -> ID,&chunk -> ID,array[0],array[1],array[2],array[3]);
#endif
//+
		WriteChunkBytes(Msg -> IFF,(APTR)((ULONG)(chunk) + sizeof (struct FeelinNode)),     4);
		WriteChunkBytes(Msg -> IFF,(APTR)((ULONG)(chunk) + sizeof (struct FeelinNode) + 4), 4);
		WriteChunkBytes(Msg -> IFF,(APTR)((ULONG)(chunk) + sizeof (struct FeelinNode) + 8), chunk -> Size);
	}
	return PopChunk(Msg -> IFF);
} 
//+
///DS_ReadIFF
F_METHODM(ULONG,DS_ReadIFF,FS_Dataspace_ReadIFF)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct ContextNode *cn;
	ULONG read = 0;

	if ((cn = CurrentChunk(Msg -> IFF)) != NULL)
	{
	   while (cn -> cn_Size != read)
	   {
	      struct FeelinChunkData *chunk;
			uint32 id,size;
 
			read += ReadChunkBytes(Msg -> IFF,(APTR)(&id),sizeof (uint32));
			read += ReadChunkBytes(Msg -> IFF,(APTR)(&size),sizeof (uint32));

			if ((chunk = F_NewP(LOD -> Pool,size + sizeof (struct FeelinChunkData))) != NULL)
	      {
				chunk -> ID = id;
				chunk -> Size = size;

	         read += ReadChunkBytes(Msg -> IFF,(APTR)((ULONG)(chunk) + sizeof (struct FeelinChunkData)),size);

				F_LinkTail(&LOD -> Chunks,(FNode *) chunk);
	      }
	   }
	}
	return read;
}
//+
