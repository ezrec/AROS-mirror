#include "Private.h"

//#define DB_ADD

///DS_Add
F_METHODM(APTR,DS_Add,FS_Dataspace_Add)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    F_Do(Obj,F_IDM(FM_Dataspace_Remove),Msg -> ID);

    if (Msg -> Size && Msg -> Data)
    {
        struct FeelinChunkData *chunk;

        if ((chunk = F_NewP(LOD -> Pool,Msg -> Size + sizeof (struct FeelinChunkData))) != NULL)
        {
///DB_ADD
#ifdef DB_ADD
            ULONG *data = (ULONG *)(((ULONG)(chunk)) + sizeof (struct FeelinChunkData));
#endif
//+
            chunk -> ID    = Msg -> ID;
            chunk -> Size  = Msg -> Size;

            CopyMem(Msg -> Data,(APTR)((ULONG)(chunk) + sizeof (struct FeelinChunkData)),Msg -> Size);
///DB_ADD
#ifdef DB_ADD
            F_DebugOut("Dataspace.Add - ID 0x%08lx (%4.4s) - Data: %08lx%08lx%08lx%08lx\n",Msg -> ID,&Msg -> ID,data[0],data[1],data[2],data[3]);
#endif
//+
            F_LinkTail(&LOD -> Chunks,(FNode *) chunk);

            return chunk;
        }
    }
    return NULL;
}
//+
///DS_Remove
F_METHODM(LONG,DS_Remove,FS_Dataspace_Remove)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinChunkData *chunk;

    if ((chunk = (APTR)(F_DoA(Obj,F_IDM(FM_Dataspace_Find),Msg))) != NULL)
    {
        chunk = (APTR)((ULONG)(chunk) - sizeof (struct FeelinChunkData));

        F_LinkRemove(&LOD -> Chunks,(FNode *) chunk);

        F_Dispose(chunk);

        return TRUE;
    }
    return FALSE;
}
//+
///DS_Find
F_METHODM(APTR,DS_Find,FS_Dataspace_Find)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinChunkData *chunk;
    APTR next = LOD -> Chunks.Head;
    
    while ((chunk = F_NextNode(&next)) != NULL)
    {
        if (chunk -> ID == Msg -> ID)
        {
            return (APTR)((ULONG)(chunk) + sizeof (struct FeelinChunkData));
        }
    }
    return NULL;
}
//+
///DS_Clear
F_METHOD(void,DS_Clear)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct FeelinChunkData *chunk;

    while ((chunk = (APTR)(LOD -> Chunks.Head)) != NULL)
    {
        F_LinkRemove(&LOD -> Chunks,(FNode *) chunk);
        F_Dispose(chunk);
    }
}
//+
