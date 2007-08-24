#include "Private.h"

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Document_New
F_METHOD(FObject,Document_New)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    APTR source=NULL;
    uint32 type=0;
            
    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Document_Source:      source = (APTR)(item.ti_Data); break;
        case FA_Document_SourceType:  type = item.ti_Data; break;
    }

    if (F_SUPERDO())
    {
        if (source && type && LOD -> Pool)
        {
            return Obj;
        }
        else
        {
            return Obj;
        }
    }
    return NULL;
}
//+
///Document_Dispose
F_METHOD(void,Document_Dispose)
{
    F_Do(Obj,F_IDM(FM_Document_Clear));
    
    F_SUPERDO();
}
//+
///Document_Get
F_METHOD(void,Document_Get)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Document_Version:     F_STORE(LOD -> version); break;
        case FA_Document_Revision:    F_STORE(LOD -> revision); break;
        case FA_Document_Pool:        F_STORE(LOD -> Pool); break;
    }

    F_SUPERDO();
}
//+
///Document_Set
F_METHOD(void,Document_Set)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    struct TagItem *Tags = Msg,item;
    
    uint32 type=0;
    APTR source=NULL;

    while  (F_DynamicNTI(&Tags,&item,Class))
    switch (item.ti_Tag)
    {
        case FA_Document_Version:     LOD -> version = item.ti_Data; break;
        case FA_Document_Revision:    LOD -> revision = item.ti_Data; break;
        case FA_Document_Source:      source = (APTR)(item.ti_Data); break;
        case FA_Document_SourceType:  type = item.ti_Data; break;
    }

    F_SUPERDO();
    
    if (source && type)
    {
        F_Do(Obj,F_IDM(FM_Document_Clear));
            
        LOD -> Pool = F_CreatePool(4096,FA_Pool_Items,1,FA_Pool_Name,"Document.Data",TAG_DONE);
  
        if (LOD->Pool);
        {
            LOD -> HTable = F_NewP(LOD -> Pool,sizeof (FHashTable) + sizeof (APTR) * FV_HASH_NORMAL);
            
            if (LOD->HTable)
            {
                LOD -> HTable -> Size = FV_HASH_NORMAL;
                LOD -> HTable -> Entries = (APTR)((uint32)(LOD -> HTable) + sizeof (FHashTable));

                if ((source = (APTR) F_Do(Obj,F_IDM(FM_Document_Read),source,type,LOD -> Pool)) != NULL)
                {
                    BOOL ok = F_Do(Obj,F_IDM(FM_Document_Parse),source,LOD -> Pool);

                    F_Dispose(source);
                    
                    if (ok)
                    {
                        return;
                    }
                }
            }
            F_Do(Obj,F_IDM(FM_Document_Clear));
        }
    }
}
//+

///Document_Read
F_METHODM(STRPTR,Document_Read,FS_Document_Read)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    if (LOD -> Pool)
    {
        STRPTR buf=NULL;
     
        if (Msg -> Type == FV_Document_SourceType_File)
        {
            BPTR lock;

            LOD -> sourcename = F_StrNew(NULL,"%s",Msg -> Source);
            
            if ((lock = Lock(Msg->Source, ACCESS_READ)) != NULL)
            {
                BPTR file;
     
                if ((file = Open(Msg -> Source,MODE_OLDFILE)) != NULL)
                {
                    struct FileInfoBlock *fib;

                    if ((fib = AllocDosObject(DOS_FIB,TAG_DONE)) != NULL)
                    {
                        if (Examine(lock,fib))
                        {
                            if ((buf = F_NewP(LOD -> Pool,fib -> fib_Size + 1)) != NULL)
                            {
                                if (!Read(file,buf,fib -> fib_Size))
                                {
                                    buf = NULL;
                                }
                            }
                        }
                        FreeDosObject(DOS_FIB,fib);
                    }
                    else
                    {
                        F_Log(FV_LOG_CORE,"AllocDosObject() failed");
                    }
                    Close(file);
                }
                else
                {
                    F_Log(FV_LOG_USER,"Unable to open file \"%s\"",Msg -> Source);
                }
                UnLock(lock);
            }
            else
            {
                F_Log(FV_LOG_USER,"Unable to lock file \"%s\"",Msg->Source);
            }
        }
        else if (Msg -> Type == FV_Document_SourceType_Memory)
        {
            uint32 len = F_StrLen(Msg -> Source);
            
            if ((buf = F_NewP(LOD -> Pool,len + 1)) != NULL)
            {
                CopyMem(Msg -> Source,buf,len);
            }
        }
        else
        {
            F_Log(FV_LOG_DEV,"Unknown source type (%ld,0x%08lx)",Msg -> Type,Msg -> Source);
        }

        return buf;
    }
    return NULL;
}
//+
///Document_Parse
F_METHODM(int32,Document_Parse,FS_Document_Parse)
{
    return FALSE;
}
//+
///Document_AddIDs
F_METHODM(uint32,Document_AddIDs,FS_Document_AddIDs)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    uint32 n=0;
    
    if (LOD -> HTable && Msg -> Table)
    {
        FDOCID *id;
 
        for (id = Msg -> Table ; id -> Name ; id++)
        {
            FDOCName *name;
            
            if ((name = (FDOCName *) F_HashFind(LOD -> HTable,id -> Name,id -> Length,NULL)) != NULL)
            {
//            F_Log(0,"Name (%s) Resolved (%ld)",name -> Key,r -> ID);
 
                name -> ID = id -> Value; n++;
            }
        }
    }
    return n;
}
//+
///Document_Log
F_METHODM(void,Document_Log,FS_Document_Log)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);
    
    STRPTR fst=NULL;
    STRPTR str;

    if (LOD -> sourcename)
    {
        fst = F_StrNew(NULL,"Error in '%s' ",LOD -> sourcename);
    }

    str = F_StrNewA(NULL,Msg -> Fmt,(APTR)((uint32)(Msg) + sizeof (struct FS_Document_Log)));

    if (Msg -> Source)
    {
        F_Log(0,"\n\n%s%04ld: %s\n\n%32.32s\n",(fst) ? (fst) : (STRPTR)(""),Msg -> Line,str,Msg -> Source);
    }
    else
    {
        F_Log(0,"\n\n%s%04ld: %s\n",(fst) ? (fst) : (STRPTR)(""),Msg -> Line,str);
    }
    
    F_Dispose(fst);
    F_Dispose(str);
}
//+
///Document_Resolve
F_METHODM(uint32,Document_Resolve,FS_Document_Resolve)
{
    uint32 value=0;
    bits32 done=0;

//   F_Log(0,"ATTRIBUTE (%s)(0x%08lx)(%02ld) - VALUES (0x%08lx) - DATA (%s)(0x%08lx)",Attribute -> Name,Attribute,Attribute -> Type,Attribute -> Values,Data,Data);

    if (Msg -> Data)
    {

/*** check values **********************************************************/

        if (Msg -> Values)
        {
            FDOCValue *val;

            for (val = Msg -> Values ; val -> Name ; val++)
            {
                if (F_StrCmp(Msg -> Data,val -> Name,ALL) == 0) break;
            }

            if (val -> Name)
            {
                value = val -> Value; done = 0xFFFFFFFF;
            }
        }

/*** check boolean *********************************************************/

        if (!done && (FF_TYPE_BOOLEAN & Msg -> Type))
        {
            done = FV_TYPE_BOOLEAN;

            if (F_StrCmp("true",Msg -> Data,ALL) == 0)         value = TRUE;
            else if (F_StrCmp("yes",Msg -> Data,ALL) == 0)     value = TRUE;
            else if (F_StrCmp("false",Msg -> Data,ALL) == 0)   value = FALSE;
            else if (F_StrCmp("no",Msg -> Data,ALL) == 0)      value = FALSE;
            else
            {
                done = FV_TYPE_BOOLEAN;
            }
        }

/*** check integer *********************************************************/

        if (!done && (FF_TYPE_INTEGER & Msg -> Type))
        {
            if (*Msg -> Data == '#')
            {
                if (stch_l(Msg -> Data + 1,(int32 *)(&value)))
                {
                    done = FV_TYPE_HEXADECIMAL;
                }
            }
            else if (*Msg -> Data == '!')
            {
                if (F_StrLen(Msg -> Data + 1) == 4)
                {
                    value = *((uint32 *)(Msg -> Data + 1)); done = FV_TYPE_CONSTANT;
                }
            }
            else if (*Msg -> Data == '%')
            {
                if (F_StrLen(Msg -> Data + 1) < 32)
                {
                    STRPTR bit;

                    for (bit = Msg -> Data + 1; *bit ; bit++)
                    {
                        if (*bit == '1')
                        {
                            value = value << 1;
                            value |= 1;
                        }
                        else if (*bit == '0')
                        {
                            value = value << 1;
                        }
                        else
                        {
                            F_Do(Obj,F_IDM(FM_Document_Log),0,NULL,"Binary error (%s)",Msg -> Data);
                        }
                    }

                    if (!*bit)
                    {
                        done = FV_TYPE_BINARY;
                    }
                }
                else
                {
                    F_Do(Obj,F_IDM(FM_Document_Log),0,NULL,"Binary too long (%s)",Msg -> Data);
                }
            }
            else
            {
                uint32 len = stcd_l(Msg -> Data,(int32 *)(&value));
 
                if (len)
                {
                    if (Msg -> Data[len] == '%')
                    {
                        done = FV_TYPE_PERCENTAGE;
                    }
                    else if (Msg -> Data[len] == 'p' && Msg -> Data[len+1] == 'x')
                    {
                        done = FV_TYPE_PIXEL;
                    }
                    else
                    {
                        done = FV_TYPE_INTEGER;
                    }
                }
            }
        }

/*** check string **********************************************************/

        if (!done && (FV_TYPE_STRING & Msg -> Type))
        {
            done = FV_TYPE_STRING; value = (uint32)(Msg -> Data);
        }
    }

    if (Msg -> Done)
    {
        *Msg -> Done = done;
    }

    return value;
}
//+
///Document_FindName
F_METHODM(APTR,Document_FindName,FS_Document_FindName)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> HTable)
    {
        return F_HashFind(LOD -> HTable,Msg -> Key,Msg -> KeyLength,NULL);
    }
    return NULL;
}
//+
///Document_ObtainName
F_METHODM(FDOCName *,Document_ObtainName,FS_Document_ObtainName)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    if (LOD -> Pool)
    {
        uint32 hash=0;
        FDOCName *name;

        if ((name = (FDOCName *) F_HashFind(LOD -> HTable,Msg -> Key,Msg -> KeyLength,&hash)) != NULL)
        {
            return name;
        }
        else if ((name = F_NewP(LOD -> Pool,sizeof (FDOCName) + Msg -> KeyLength + 1)) != NULL)
        {
            name -> Key = (APTR)((uint32)(name) + sizeof (FDOCName));
            name -> KeyLength = Msg -> KeyLength;

            CopyMem(Msg -> Key,name -> Key,Msg -> KeyLength);

            name -> Next = (FDOCName *) LOD -> HTable -> Entries[hash];
            LOD -> HTable -> Entries[hash] = (FHashLink *) name;

    //      F_Log(0,"NAME (%s) - NEXT 0x%08lx - HASH 0x%08lx",name -> Key,name -> Next,hash);
        }
        return name;
    }
    return NULL;
}
//+
///Document_Clear
F_METHOD(void,Document_Clear)
{
    struct LocalObjectData *LOD = F_LOD(Class,Obj);

    LOD -> HTable = NULL;
    F_DeletePool(LOD -> Pool); LOD -> Pool = NULL;
    F_Dispose(LOD -> sourcename); LOD -> sourcename = NULL;
}
//+
