#ifndef _VBCCINLINE_FEELIN_H
#define _VBCCINLINE_FEELIN_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif

APTR __F_CreatePoolA(__reg("a6") struct Library *, __reg("d0") ULONG ItemSize, __reg("a0") struct TagItem * Tagl)="\tjsr\t-30(a6)";
#define F_CreatePoolA(ItemSize, Tagl) __F_CreatePoolA(FeelinBase, (ItemSize), (Tagl))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
APTR __F_CreatePool(__reg("a6") struct Library *, __reg("d0") ULONG ItemSize, ...)="\tmove.l\ta0,-(a7)\n\tlea\t4(a7),a0\n\tjsr\t-30(a6)\n\tmovea.l\t(a7)+,a0";
#define F_CreatePool(...) __F_CreatePool(FeelinBase, __VA_ARGS__)
#endif
*/
void __F_DeletePool(__reg("a6") struct Library *, __reg("a0") APTR Pool)="\tjsr\t-36(a6)";
#define F_DeletePool(Pool) __F_DeletePool(FeelinBase, (Pool))

APTR __F_New(__reg("a6") struct Library *, __reg("d0") ULONG Size)="\tjsr\t-72(a6)";
#define F_New(Size) __F_New(FeelinBase, (Size))

APTR __F_NewP(__reg("a6") struct Library *, __reg("a0") APTR Pool, __reg("d0") ULONG Size)="\tjsr\t-78(a6)";
#define F_NewP(Pool, Size) __F_NewP(FeelinBase, (Pool), (Size))

void __F_Dispose(__reg("a6") struct Library *, __reg("a1") APTR Mem)="\tjsr\t-84(a6)";
#define F_Dispose(Mem) __F_Dispose(FeelinBase, (Mem))

void __F_OPool(__reg("a6") struct Library *, __reg("a0") APTR Pool)="\tjsr\t-96(a6)";
#define F_OPool(Pool) __F_OPool(FeelinBase, (Pool))

void __F_SPool(__reg("a6") struct Library *, __reg("a0") APTR Pool)="\tjsr\t-102(a6)";
#define F_SPool(Pool) __F_SPool(FeelinBase, (Pool))

void __F_RPool(__reg("a6") struct Library *, __reg("a0") APTR Pool)="\tjsr\t-108(a6)";
#define F_RPool(Pool) __F_RPool(FeelinBase, (Pool))

APTR __F_LinkTail(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node)="\tjsr\t-120(a6)";
#define F_LinkTail(List, Node) __F_LinkTail(FeelinBase, (List), (Node))

APTR __F_LinkHead(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node)="\tjsr\t-126(a6)";
#define F_LinkHead(List, Node) __F_LinkHead(FeelinBase, (List), (Node))

APTR __F_LinkInsert(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node, __reg("a2") FNode * Prev)="\tjsr\t-132(a6)";
#define F_LinkInsert(List, Node, Prev) __F_LinkInsert(FeelinBase, (List), (Node), (Prev))

APTR __F_LinkMove(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node, __reg("a2") FNode * Prev)="\tjsr\t-138(a6)";
#define F_LinkMove(List, Node, Prev) __F_LinkMove(FeelinBase, (List), (Node), (Prev))

APTR __F_LinkRemove(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node)="\tjsr\t-144(a6)";
#define F_LinkRemove(List, Node) __F_LinkRemove(FeelinBase, (List), (Node))

APTR __F_LinkMember(__reg("a6") struct Library *, __reg("a0") FList * List, __reg("a1") FNode * Node)="\tjsr\t-150(a6)";
#define F_LinkMember(List, Node) __F_LinkMember(FeelinBase, (List), (Node))

APTR __F_NextNode(__reg("a6") struct Library *, __reg("a0") APTR * Nodeptrptr)="\tjsr\t-156(a6)";
#define F_NextNode(Nodeptrptr) __F_NextNode(FeelinBase, (Nodeptrptr))

STRPTR __F_StrFmtA(__reg("a6") struct Library *, __reg("a0") APTR Buffer, __reg("a1") STRPTR Fmt, __reg("a2") LONG * Params)="\tjsr\t-162(a6)";
#define F_StrFmtA(Buffer, Fmt, Params) __F_StrFmtA(FeelinBase, (Buffer), (Fmt), (Params))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
ULONG __F_StrFmt(__reg("a6") struct Library *, __reg("a0") APTR Buffer, __reg("a1") STRPTR Fmt, ...)="\tmove.l\ta2,-(a7)\n\tlea\t4(a7),a2\n\tjsr\t-162(a6)\n\tmovea.l\t(a7)+,a2";
#define F_StrFmt(Buffer, ...) __F_StrFmt(FeelinBase, (Buffer), __VA_ARGS__)
#endif
*/
STRPTR __F_StrNewA(__reg("a6") struct Library *, __reg("a0") ULONG * Lengthptr, __reg("a1") STRPTR Fmt, __reg("a2") LONG * Params)="\tjsr\t-168(a6)";
#define F_StrNewA(Lengthptr, Fmt, Params) __F_StrNewA(FeelinBase, (Lengthptr), (Fmt), (Params))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
STRPTR __F_StrNew(__reg("a6") struct Library *, __reg("a0") ULONG * Lengthptr, __reg("a1") STRPTR Fmt, ...)="\tmove.l\ta2,-(a7)\n\tlea\t4(a7),a2\n\tjsr\t-168(a6)\n\tmovea.l\t(a7)+,a2";
#define F_StrNew(Lengthptr, ...) __F_StrNew(FeelinBase, (Lengthptr), __VA_ARGS__)
#endif
*/
ULONG __F_StrLen(__reg("a6") struct Library *, __reg("a0") STRPTR Str)="\tjsr\t-174(a6)";
#define F_StrLen(Str) __F_StrLen(FeelinBase, (Str))

LONG __F_StrCmp(__reg("a6") struct Library *, __reg("a0") STRPTR Str1, __reg("a1") STRPTR Str2, __reg("d1") ULONG Length)="\tjsr\t-180(a6)";
#define F_StrCmp(Str1, Str2, Length) __F_StrCmp(FeelinBase, (Str1), (Str2), (Length))

void __F_LogA(__reg("a6") struct Library *, __reg("d0") ULONG Level, __reg("a0") STRPTR Fmt, __reg("a1") LONG * Params)="\tjsr\t-186(a6)";
#define F_LogA(Level, Fmt, Params) __F_LogA(FeelinBase, (Level), (Fmt), (Params))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
void __F_Log(__reg("a6") struct Library *, __reg("d0") ULONG Level, __reg("a0") STRPTR Fmt, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-186(a6)\n\tmovea.l\t(a7)+,a1";
#define F_Log(Level, ...) __F_Log(FeelinBase, (Level), __VA_ARGS__)
#endif
*/
void __F_AlertA(__reg("a6") struct Library *, __reg("a0") STRPTR Title, __reg("a1") STRPTR Body, __reg("a2") LONG * Params)="\tjsr\t-192(a6)";
#define F_AlertA(Title, Body, Params) __F_AlertA(FeelinBase, (Title), (Body), (Params))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
void __F_Alert(__reg("a6") struct Library *, __reg("a0") STRPTR Title, __reg("a1") STRPTR Body, ...)="\tmove.l\ta2,-(a7)\n\tlea\t4(a7),a2\n\tjsr\t-192(a6)\n\tmovea.l\t(a7)+,a2";
#define F_Alert(Title, ...) __F_Alert(FeelinBase, (Title), __VA_ARGS__)
#endif
*/
FHashTable * __F_HashCreate(__reg("a6") struct Library *, __reg("d0") ULONG Size)="\tjsr\t-198(a6)";
#define F_HashCreate(Size) __F_HashCreate(FeelinBase, (Size))

void __F_HashDelete(__reg("a6") struct Library *, __reg("a0") FHashTable * Table)="\tjsr\t-204(a6)";
#define F_HashDelete(Table) __F_HashDelete(FeelinBase, (Table))

FHashLink * __F_HashFind(__reg("a6") struct Library *, __reg("a0") FHashTable * Table, __reg("a1") UBYTE * Key, __reg("d1") ULONG KeyLength, __reg("a2") ULONG * HashValuePtr)="\tjsr\t-210(a6)";
#define F_HashFind(Table, Key, KeyLength, HashValuePtr) __F_HashFind(FeelinBase, (Table), (Key), (KeyLength), (HashValuePtr))

FHashLink * __F_HashAddLink(__reg("a6") struct Library *, __reg("a0") FHashTable * Table, __reg("a1") FHashLink * Link)="\tjsr\t-216(a6)";
#define F_HashAddLink(Table, Link) __F_HashAddLink(FeelinBase, (Table), (Link))

void __F_HashRemLink(__reg("a6") struct Library *, __reg("a0") FHashTable * Table, __reg("a1") FHashLink * Link)="\tjsr\t-222(a6)";
#define F_HashRemLink(Table, Link) __F_HashRemLink(FeelinBase, (Table), (Link))

FHashLink * __F_HashAdd(__reg("a6") struct Library *, __reg("a0") FHashTable * Table, __reg("a1") UBYTE * Key, __reg("d0") ULONG KeyLength, __reg("a2") APTR Data)="\tjsr\t-228(a6)";
#define F_HashAdd(Table, Key, KeyLength, Data) __F_HashAdd(FeelinBase, (Table), (Key), (KeyLength), (Data))

void __F_HashRem(__reg("a6") struct Library *, __reg("a0") FHashTable * Table, __reg("a1") UBYTE * Key, __reg("d0") ULONG KeyLength)="\tjsr\t-234(a6)";
#define F_HashRem(Table, Key, KeyLength) __F_HashRem(FeelinBase, (Table), (Key), (KeyLength))

LONG __F_DynamicCreate(__reg("a6") struct Library *, __reg("a0") FClass * feelin_class)="\tjsr\t-240(a6)";
#define F_DynamicCreate(feelin_class) __F_DynamicCreate(FeelinBase, (feelin_class))

void __F_DynamicDelete(__reg("a6") struct Library *, __reg("a0") FClass * feelin_class)="\tjsr\t-246(a6)";
#define F_DynamicDelete(feelin_class) __F_DynamicDelete(FeelinBase, (feelin_class))

FClassAttribute * __F_DynamicFindAttribute(__reg("a6") struct Library *, __reg("a0") STRPTR Name, __reg("a1") FClass * FromClass, __reg("a2") FClass ** RealClassPtr)="\tjsr\t-252(a6)";
#define F_DynamicFindAttribute(Name, FromClass, RealClassPtr) __F_DynamicFindAttribute(FeelinBase, (Name), (FromClass), (RealClassPtr))

FClassMethod * __F_DynamicFindMethod(__reg("a6") struct Library *, __reg("a0") STRPTR Name, __reg("a1") FClass * FromClass, __reg("a2") FClass ** RealClassPtr)="\tjsr\t-258(a6)";
#define F_DynamicFindMethod(Name, FromClass, RealClassPtr) __F_DynamicFindMethod(FeelinBase, (Name), (FromClass), (RealClassPtr))

ULONG __F_DynamicFindID(__reg("a6") struct Library *, __reg("a0") STRPTR Name)="\tjsr\t-264(a6)";
#define F_DynamicFindID(Name) __F_DynamicFindID(FeelinBase, (Name))

void __F_DynamicResolveTable(__reg("a6") struct Library *, __reg("a0") FDynamicEntry * Table)="\tjsr\t-270(a6)";
#define F_DynamicResolveTable(Table) __F_DynamicResolveTable(FeelinBase, (Table))

LONG __F_DynamicAddAutoTable(__reg("a6") struct Library *, __reg("a0") FDynamicEntry * Table)="\tjsr\t-276(a6)";
#define F_DynamicAddAutoTable(Table) __F_DynamicAddAutoTable(FeelinBase, (Table))

void __F_DynamicRemAutoTable(__reg("a6") struct Library *, __reg("a0") FDynamicEntry * Table)="\tjsr\t-282(a6)";
#define F_DynamicRemAutoTable(Table) __F_DynamicRemAutoTable(FeelinBase, (Table))

struct TagItem * __F_DynamicNTI(__reg("a6") struct Library *, __reg("a0") struct TagItem ** TagListptr, __reg("a1") struct TagItem * Item, __reg("a2") FClass * feelin_class)="\tjsr\t-288(a6)";
#define F_DynamicNTI(TagListptr, Item, feelin_class) __F_DynamicNTI(FeelinBase, (TagListptr), (Item), (feelin_class))

struct TagItem * __F_DynamicFTI(__reg("a6") struct Library *, __reg("d0") ULONG Attribute, __reg("a0") struct TagItem * Tagl)="\tjsr\t-294(a6)";
#define F_DynamicFTI(Attribute, Tagl) __F_DynamicFTI(FeelinBase, (Attribute), (Tagl))

ULONG __F_DynamicGTD(__reg("a6") struct Library *, __reg("d0") ULONG Attribute, __reg("d1") ULONG DefaultValue, __reg("a0") struct TagItem * Tagl)="\tjsr\t-300(a6)";
#define F_DynamicGTD(Attribute, DefaultValue, Tagl) __F_DynamicGTD(FeelinBase, (Attribute), (DefaultValue), (Tagl))

FClass * __F_FindClass(__reg("a6") struct Library *, __reg("a0") STRPTR Name)="\tjsr\t-306(a6)";
#define F_FindClass(Name) __F_FindClass(FeelinBase, (Name))

FClass * __F_OpenClass(__reg("a6") struct Library *, __reg("a0") STRPTR Name)="\tjsr\t-312(a6)";
#define F_OpenClass(Name) __F_OpenClass(FeelinBase, (Name))

void __F_CloseClass(__reg("a6") struct Library *, __reg("a0") FClass * feelin_class)="\tjsr\t-318(a6)";
#define F_CloseClass(feelin_class) __F_CloseClass(FeelinBase, (feelin_class))

FClass * __F_CreateClassA(__reg("a6") struct Library *, __reg("a0") STRPTR Name, __reg("a1") struct TagItem * Tagl)="\tjsr\t-324(a6)";
#define F_CreateClassA(Name, Tagl) __F_CreateClassA(FeelinBase, (Name), (Tagl))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
FClass * __F_CreateClass(__reg("a6") struct Library *, __reg("a0") STRPTR Name, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-324(a6)\n\tmovea.l\t(a7)+,a1";
#define F_CreateClass(...) __F_CreateClass(FeelinBase, __VA_ARGS__)
#endif
*/
void __F_DeleteClass(__reg("a6") struct Library *, __reg("a0") FClass * feelin_class)="\tjsr\t-330(a6)";
#define F_DeleteClass(feelin_class) __F_DeleteClass(FeelinBase, (feelin_class))

ULONG __F_DoA(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d0") ULONG Method, __reg("a1") APTR Msg)="\tjsr\t-336(a6)";
#define F_DoA(Obj, Method, Msg) __F_DoA(FeelinBase, (Obj), (Method), (Msg))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
ULONG __F_Do(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d0") ULONG Method, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-336(a6)\n\tmovea.l\t(a7)+,a1";
#define F_Do(Obj, ...) __F_Do(FeelinBase, (Obj), __VA_ARGS__)
#endif
*/
ULONG __F_ClassDoA(__reg("a6") struct Library *, __reg("a2") FClass * feelin_class, __reg("a0") FObject Obj, __reg("d0") ULONG Method, __reg("a1") APTR Msg)="\tjsr\t-342(a6)";
#define F_ClassDoA(feelin_class, Obj, Method, Msg) __F_ClassDoA(FeelinBase, (feelin_class), (Obj), (Method), (Msg))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
ULONG __F_ClassDo(__reg("a6") struct Library *, __reg("a2") FClass * feelin_class, __reg("a0") FObject Obj, __reg("d0") ULONG Method, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-342(a6)\n\tmovea.l\t(a7)+,a1";
#define F_ClassDo(feelin_class, Obj, ...) __F_ClassDo(FeelinBase, (feelin_class), (Obj), __VA_ARGS__)
#endif
*/
ULONG __F_SuperDoA(__reg("a6") struct Library *, __reg("a2") FClass * feelin_class, __reg("a0") FObject Obj, __reg("d0") ULONG Method, __reg("a1") APTR Msg)="\tjsr\t-348(a6)";
#define F_SuperDoA(feelin_class, Obj, Method, Msg) __F_SuperDoA(FeelinBase, (feelin_class), (Obj), (Method), (Msg))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
ULONG __F_SuperDo(__reg("a6") struct Library *, __reg("a2") FClass * feelin_class, __reg("a0") FObject Obj, __reg("d0") ULONG Method, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-348(a6)\n\tmovea.l\t(a7)+,a1";
#define F_SuperDo(feelin_class, Obj, ...) __F_SuperDo(FeelinBase, (feelin_class), (Obj), __VA_ARGS__)
#endif
*/
FObject __F_NewObjA(__reg("a6") struct Library *, __reg("a0") STRPTR Name, __reg("a1") struct TagItem * Tagl)="\tjsr\t-354(a6)";
#define F_NewObjA(Name, Tagl) __F_NewObjA(FeelinBase, (Name), (Tagl))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
FObject __F_NewObj(__reg("a6") struct Library *, __reg("a0") STRPTR Name, ...)="\tmove.l\ta1,-(a7)\n\tlea\t4(a7),a1\n\tjsr\t-354(a6)\n\tmovea.l\t(a7)+,a1";
#define F_NewObj(...) __F_NewObj(FeelinBase, __VA_ARGS__)
#endif
*/
FObject __F_MakeObjA(__reg("a6") struct Library *, __reg("d0") ULONG Type, __reg("a0") ULONG * Params)="\tjsr\t-360(a6)";
#define F_MakeObjA(Type, Params) __F_MakeObjA(FeelinBase, (Type), (Params))
/*
#if !defined(NO_INLINE_STDARG) && (__STDC__ == 1L) && (__STDC_VERSION__ >= 199901L)
FObject __F_MakeObj(__reg("a6") struct Library *, __reg("d0") ULONG Type, ...)="\tmove.l\ta0,-(a7)\n\tlea\t4(a7),a0\n\tjsr\t-360(a6)\n\tmovea.l\t(a7)+,a0";
#define F_MakeObj(...) __F_MakeObj(FeelinBase, __VA_ARGS__)
#endif
*/
void __F_DisposeObj(__reg("a6") struct Library *, __reg("a0") FObject Obj)="\tjsr\t-366(a6)";
#define F_DisposeObj(Obj) __F_DisposeObj(FeelinBase, (Obj))

ULONG __F_Get(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d1") ULONG Attribute)="\tjsr\t-372(a6)";
#define F_Get(Obj, Attribute) __F_Get(FeelinBase, (Obj), (Attribute))

void __F_Set(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d1") ULONG Attribute, __reg("d2") ULONG Value)="\tjsr\t-378(a6)";
#define F_Set(Obj, Attribute, Value) __F_Set(FeelinBase, (Obj), (Attribute), (Value))

void __F_Draw(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d0") ULONG Flags)="\tjsr\t-384(a6)";
#define F_Draw(Obj, Flags) __F_Draw(FeelinBase, (Obj), (Flags))

void __F_Layout(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d0") WORD X, __reg("d1") WORD Y, __reg("d2") UWORD W, __reg("d3") UWORD H, __reg("d4") ULONG Flags)="\tjsr\t-390(a6)";
#define F_Layout(Obj, X, Y, W, H, Flags) __F_Layout(FeelinBase, (Obj), (X), (Y), (W), (H), (Flags))

void __F_Erase(__reg("a6") struct Library *, __reg("a0") FObject Obj, __reg("d0") WORD X1, __reg("d1") WORD Y1, __reg("d2") WORD X2, __reg("d3") WORD Y2, __reg("d4") ULONG Flags)="\tjsr\t-396(a6)";
#define F_Erase(Obj, X1, Y1, X2, Y2, Flags) __F_Erase(FeelinBase, (Obj), (X1), (Y1), (X2), (Y2), (Flags))

FObject __F_SharedFind(__reg("a6") struct Library *, __reg("a0") STRPTR Name)="\tjsr\t-402(a6)";
#define F_SharedFind(Name) __F_SharedFind(FeelinBase, (Name))

FObject __F_SharedCreate(__reg("a6") struct Library *, __reg("a0") FObject Object, __reg("a1") STRPTR Name)="\tjsr\t-408(a6)";
#define F_SharedCreate(Object, Name) __F_SharedCreate(FeelinBase, (Object), (Name))

void __F_SharedDelete(__reg("a6") struct Library *, __reg("a0") FObject Object)="\tjsr\t-414(a6)";
#define F_SharedDelete(Object) __F_SharedDelete(FeelinBase, (Object))

FObject __F_SharedOpen(__reg("a6") struct Library *, __reg("a0") STRPTR Name)="\tjsr\t-420(a6)";
#define F_SharedOpen(Name) __F_SharedOpen(FeelinBase, (Name))

void __F_SharedClose(__reg("a6") struct Library *, __reg("a0") FObject Object)="\tjsr\t-426(a6)";
#define F_SharedClose(Object) __F_SharedClose(FeelinBase, (Object))

#endif /*  _VBCCINLINE_FEELIN_H  */
