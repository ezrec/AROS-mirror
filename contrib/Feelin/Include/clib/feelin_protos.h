#ifndef CLIB_FEELIN_PROTOS_H
#define CLIB_FEELIN_PROTOS_H

/*
**    $VER: feelin_protos.h 8.0 (2004/12/18)
**
**    C prototypes definitions
**
**    © 2001-2004 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif

APTR                    F_CreatePoolA           (uint32 ItemSize,struct TagItem *Tagl);
void                    F_DeletePool            (APTR feelin_pool);
APTR                    F_New                   (uint32 Size);
APTR                    F_NewP                  (APTR feelin_pool,uint32 Size);
void                    F_Dispose               (APTR Mem);
void                    F_OPool                 (APTR feelin_pool);
void                    F_SPool                 (APTR feelin_pool);
void                    F_RPool                 (APTR feelin_pool);

APTR                    F_LinkTail              (FList *List,FNode *Node);
APTR                    F_LinkHead              (FList *List,FNode *Node);
APTR                    F_LinkInsert            (FList *List,FNode *Node, FNode *Prev);
APTR                    F_LinkMove              (FList *List,FNode *Node, FNode *Prev);
APTR                    F_LinkRemove            (FList *List,FNode *Node);
APTR                    F_LinkMember            (FList *List,FNode *Node);
APTR                    F_NextNode              (APTR *Nodeptrptr);

STRPTR                  F_StrNewA               (uint32 *Len,STRPTR Fmt,int32 *Params);
STRPTR                  F_StrFmtA               (APTR Buf,STRPTR Str,int32 *Params);
uint32                  F_StrLen                (STRPTR Str);
int32                   F_StrCmp                (STRPTR Str1,STRPTR Str2,uint32 Length);

void                    F_LogA                  (uint32 Level,STRPTR Fmt,int32 *Params);
void                    F_AlertA                (STRPTR Title,STRPTR Body,int32 *Params);

FHashTable *            F_HashCreate            (uint32 Size);
void                    F_HashDelete            (FHashTable *Table);
FHashLink *             F_HashFind              (FHashTable *Table,UBYTE *Key,uint32 KeyLenght,uint32 *HashValuePtr);
FHashLink *             F_HashAddLink           (FHashTable *Table,FHashLink *Link);
int32                   F_HashRemLink           (FHashTable *Table,FHashLink *Link);
FHashLink *             F_HashAdd               (FHashTable *Table,UBYTE *Key,uint32 KeyLength,APTR Data);
int32                   F_HashRem               (FHashTable *Table,UBYTE *Key,uint32 KeyLength);

int32                   F_DynamicCreate         (FClass *feelin_class);
void                    F_DynamicDelete         (FClass *feelin_class);
FClassAttribute *       F_DynamicFindAttribute  (STRPTR Name,FClass *FromClass,FClass **RealClassPtr);
FClassMethod *          F_DynamicFindMethod     (STRPTR Name,FClass *FromClass,FClass **RealClassPtr);
uint32                  F_DynamicFindID         (STRPTR Name);
void                    F_DynamicResolveTable   (FDynamicEntry *Entries);
int32                   F_DynamicAddAutoTable   (FDynamicEntry *Table);
void                    F_DynamicRemAutoTable   (FDynamicEntry *Table);
struct TagItem *        F_DynamicNTI            (struct TagItem **TagListptr,struct TagItem *Item,FClass *feelin_class);
struct TagItem *        F_DynamicFTI            (uint32 Tag,struct TagItem *Tagl);
uint32                  F_DynamicGTD            (uint32 Tag,uint32 Default,struct TagItem *Tagl);

FClass *                F_FindClass             (STRPTR Name);
FClass *                F_OpenClass             (STRPTR Name);
void                    F_CloseClass            (FClass *feelin_class);
FClass *                F_CreateClassA          (STRPTR Name,struct TagItem *Tagl);
void                    F_DeleteClass           (FClass *feelin_class);

uint32                  F_DoA                   (FObject Obj,uint32 Method,APTR Msg);
uint32                  F_ClassDoA              (FClass *feelin_class, FObject Obj, uint32 Method, APTR Msg);
uint32                  F_SuperDoA              (FClass *feelin_class, FObject Obj, uint32 Method, APTR Msg);

FObject                 F_NewObjA               (STRPTR Name, struct TagItem *Tagl);
FObject                 F_MakeObjA              (uint32 Type,uint32 *Params);
void                    F_DisposeObj            (FObject Obj);

uint32                  F_Get                   (FObject Obj,uint32 Attribute);
void                    F_Set                   (FObject Obj,uint32 Attribute,uint32 Value);

void                    F_Draw                  (FObject Obj,uint32 Flags);
void                    F_Layout                (FObject Obj,WORD X,WORD Y,UWORD W,UWORD H,uint32 Flags);
void                    F_Erase                 (FObject Obj,WORD X1,WORD Y1,WORD X2,WORD Y2,uint32 Flags);

FObject                 F_SharedFind            (STRPTR Name);
FObject                 F_SharedCreate          (FObject Object,STRPTR Name);
void                    F_SharedDelete          (FObject Shared);
FObject                 F_SharedOpen            (STRPTR Name);
void                    F_SharedClose           (FObject Shared);

#ifndef NO_TAG_CALL
APTR                    F_CreatePool            (uint32 ItemSize,...);
STRPTR                  F_StrNew                (uint32 *Len,STRPTR Fmt,...);
STRPTR                  F_StrFmt                (APTR Buf,STRPTR Str,...);
void                    F_Log                   (uint32 Level,STRPTR Fmt,...);
void                    F_Alert                 (STRPTR Title,STRPTR Body,...);
FClass *                F_CreateClass           (STRPTR Name,...);
uint32                  F_Do                    (FObject Obj,uint32 Method,...);
uint32                  F_ClassDo               (FClass *feelin_class,FObject Obj,uint32 Method,...);
uint32                  F_SuperDo               (FClass *feelin_class,FObject Obj,uint32 Method,...);
FObject                 F_NewObj                (STRPTR Name,...);
FObject                 F_MakeObj               (uint32 Type,...);
#endif

#endif  /* CLIB_FEELIN_PROTOS_H */
