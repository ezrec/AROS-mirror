/* Automatically generated header! Do not edit! */

#ifndef _INLINE_FEELIN_H
#define _INLINE_FEELIN_H

#ifndef AROS_LIBCALL_H
#include <aros/libcall.h>
#endif /* !AROS_LIBCALL_H */

#ifndef FEELIN_BASE_NAME
#define FEELIN_BASE_NAME FeelinBase
#endif /* !FEELIN_BASE_NAME */

#define F_AlertA(___Title, ___Body, ___Params) \
	AROS_LC3(void, F_AlertA, \
	AROS_LCA(STRPTR, (___Title), A0), \
	AROS_LCA(STRPTR, (___Body), A1), \
	AROS_LCA(int32 *, (___Params), A2), \
	struct Library *, FEELIN_BASE_NAME, 32, /* s */)

#define F_ClassDoA(___feelin_class, ___Obj, ___Method, ___Msg) \
	AROS_LC4(uint32, F_ClassDoA, \
	AROS_LCA(FClass *, (___feelin_class), A2), \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Method), D0), \
	AROS_LCA(APTR, (___Msg), A1), \
	struct Library *, FEELIN_BASE_NAME, 57, /* s */)

#define F_CloseClass(___feelin_class) \
	AROS_LC1(void, F_CloseClass, \
	AROS_LCA(FClass *, (___feelin_class), A0), \
	struct Library *, FEELIN_BASE_NAME, 53, /* s */)

#define F_CreateClassA(___Name, ___Tagl) \
	AROS_LC2(FClass *, F_CreateClassA, \
	AROS_LCA(STRPTR, (___Name), A0), \
	AROS_LCA(struct TagItem *, (___Tagl), A1), \
	struct Library *, FEELIN_BASE_NAME, 54, /* s */)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)
#define F_CreateClass(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; F_CreateClassA((a0), (struct TagItem *)_tags);})
#endif

#define F_CreatePoolA(___ItemSize, ___Tagl) \
	AROS_LC2(APTR, F_CreatePoolA, \
	AROS_LCA(uint32, (___ItemSize), D0), \
	AROS_LCA(struct TagItem *, (___Tagl), A0), \
	struct Library *, FEELIN_BASE_NAME, 5, /* s */)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)
#define F_CreatePool(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; F_CreatePoolA((a0), (struct TagItem *)_tags);})
#endif

#define F_DeleteClass(___feelin_class) \
	AROS_LC1(void, F_DeleteClass, \
	AROS_LCA(FClass *, (___feelin_class), A0), \
	struct Library *, FEELIN_BASE_NAME, 55, /* s */)

#define F_DeletePool(___Pool) \
	AROS_LC1(void, F_DeletePool, \
	AROS_LCA(APTR, (___Pool), A0), \
	struct Library *, FEELIN_BASE_NAME, 6, /* s */)

#define F_Dispose(___Mem) \
	AROS_LC1(void, F_Dispose, \
	AROS_LCA(APTR, (___Mem), A1), \
	struct Library *, FEELIN_BASE_NAME, 14, /* s */)

#define F_DisposeObj(___Obj) \
	AROS_LC1(void, F_DisposeObj, \
	AROS_LCA(FObject, (___Obj), A0), \
	struct Library *, FEELIN_BASE_NAME, 61, /* s */)

#define F_DoA(___Obj, ___Method, ___Msg) \
	AROS_LC3(uint32, F_DoA, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Method), D0), \
	AROS_LCA(APTR, (___Msg), A1), \
	struct Library *, FEELIN_BASE_NAME, 56, /* s */)

#define F_Draw(___Obj, ___Flags) \
	AROS_LC2(void, F_Draw, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Flags), D0), \
	struct Library *, FEELIN_BASE_NAME, 64, /* s */)

#define F_DynamicAddAutoTable(___Table) \
	AROS_LC1(int32, F_DynamicAddAutoTable, \
	AROS_LCA(FDynamicEntry *, (___Table), A0), \
	struct Library *, FEELIN_BASE_NAME, 46, /* s */)

#define F_DynamicCreate(___feelin_class) \
	AROS_LC1(int32, F_DynamicCreate, \
	AROS_LCA(FClass *, (___feelin_class), A0), \
	struct Library *, FEELIN_BASE_NAME, 40, /* s */)

#define F_DynamicDelete(___feelin_class) \
	AROS_LC1(void, F_DynamicDelete, \
	AROS_LCA(FClass *, (___feelin_class), A0), \
	struct Library *, FEELIN_BASE_NAME, 41, /* s */)

#define F_DynamicFTI(___Attribute, ___Tagl) \
	AROS_LC2(struct TagItem *, F_DynamicFTI, \
	AROS_LCA(uint32, (___Attribute), D0), \
	AROS_LCA(struct TagItem *, (___Tagl), A0), \
	struct Library *, FEELIN_BASE_NAME, 49, /* s */)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)
#define F_DynamicFTITags(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; F_DynamicFTI((a0), (struct TagItem *)_tags);})
#endif

#define F_DynamicFindAttribute(___Name, ___FromClass, ___RealClassPtr) \
	AROS_LC3(FClassAttribute *, F_DynamicFindAttribute, \
	AROS_LCA(STRPTR, (___Name), A0), \
	AROS_LCA(FClass *, (___FromClass), A1), \
	AROS_LCA(FClass **, (___RealClassPtr), A2), \
	struct Library *, FEELIN_BASE_NAME, 42, /* s */)

#define F_DynamicFindID(___Name) \
	AROS_LC1(uint32, F_DynamicFindID, \
	AROS_LCA(STRPTR, (___Name), A0), \
	struct Library *, FEELIN_BASE_NAME, 44, /* s */)

#define F_DynamicFindMethod(___Name, ___FromClass, ___RealClassPtr) \
	AROS_LC3(FClassMethod *, F_DynamicFindMethod, \
	AROS_LCA(STRPTR, (___Name), A0), \
	AROS_LCA(FClass *, (___FromClass), A1), \
	AROS_LCA(FClass **, (___RealClassPtr), A2), \
	struct Library *, FEELIN_BASE_NAME, 43, /* s */)

#define F_DynamicGTD(___Attribute, ___DefaultValue, ___Tagl) \
	AROS_LC3(uint32, F_DynamicGTD, \
	AROS_LCA(uint32, (___Attribute), D0), \
	AROS_LCA(uint32, (___DefaultValue), D1), \
	AROS_LCA(struct TagItem *, (___Tagl), A0), \
	struct Library *, FEELIN_BASE_NAME, 50, /* s */)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)
#define F_DynamicGTDTags(a0, a1, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; F_DynamicGTD((a0), (a1), (struct TagItem *)_tags);})
#endif

#define F_DynamicNTI(___TagListptr, ___Item, ___feelin_class) \
	AROS_LC3(struct TagItem *, F_DynamicNTI, \
	AROS_LCA(struct TagItem **, (___TagListptr), A0), \
	AROS_LCA(struct TagItem *, (___Item), A1), \
	AROS_LCA(FClass *, (___feelin_class), A2), \
	struct Library *, FEELIN_BASE_NAME, 48, /* s */)

#define F_DynamicRemAutoTable(___Table) \
	AROS_LC1(void, F_DynamicRemAutoTable, \
	AROS_LCA(FDynamicEntry *, (___Table), A0), \
	struct Library *, FEELIN_BASE_NAME, 47, /* s */)

#define F_DynamicResolveTable(___Table) \
	AROS_LC1(void, F_DynamicResolveTable, \
	AROS_LCA(FDynamicEntry *, (___Table), A0), \
	struct Library *, FEELIN_BASE_NAME, 45, /* s */)

#define F_Erase(___Obj, ___X1, ___Y1, ___X2, ___Y2, ___Flags) \
	AROS_LC6(void, F_Erase, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(WORD, (___X1), D0), \
	AROS_LCA(WORD, (___Y1), D1), \
	AROS_LCA(WORD, (___X2), D2), \
	AROS_LCA(WORD, (___Y2), D3), \
	AROS_LCA(uint32, (___Flags), D4), \
	struct Library *, FEELIN_BASE_NAME, 66, /* s */)

#define F_FindClass(___Name) \
	AROS_LC1(FClass *, F_FindClass, \
	AROS_LCA(STRPTR, (___Name), A0), \
	struct Library *, FEELIN_BASE_NAME, 51, /* s */)

#define F_Get(___Obj, ___Attribute) \
	AROS_LC2(uint32, F_Get, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Attribute), D1), \
	struct Library *, FEELIN_BASE_NAME, 62, /* s */)

#define F_HashAdd(___Table, ___Key, ___KeyLength, ___Data) \
	AROS_LC4(FHashLink *, F_HashAdd, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	AROS_LCA(UBYTE *, (___Key), A1), \
	AROS_LCA(uint32, (___KeyLength), D0), \
	AROS_LCA(APTR, (___Data), A2), \
	struct Library *, FEELIN_BASE_NAME, 38, /* s */)

#define F_HashAddLink(___Table, ___Link) \
	AROS_LC2(FHashLink *, F_HashAddLink, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	AROS_LCA(FHashLink *, (___Link), A1), \
	struct Library *, FEELIN_BASE_NAME, 36, /* s */)

#define F_HashCreate(___Size) \
	AROS_LC1(FHashTable *, F_HashCreate, \
	AROS_LCA(uint32, (___Size), D0), \
	struct Library *, FEELIN_BASE_NAME, 33, /* s */)

#define F_HashDelete(___Table) \
	AROS_LC1(void, F_HashDelete, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	struct Library *, FEELIN_BASE_NAME, 34, /* s */)

#define F_HashFind(___Table, ___Key, ___KeyLength, ___HashValuePtr) \
	AROS_LC4(FHashLink *, F_HashFind, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	AROS_LCA(UBYTE *, (___Key), A1), \
	AROS_LCA(uint32, (___KeyLength), D1), \
	AROS_LCA(uint32 *, (___HashValuePtr), A2), \
	struct Library *, FEELIN_BASE_NAME, 35, /* s */)

#define F_HashRem(___Table, ___Key, ___KeyLength) \
	AROS_LC3(int32, F_HashRem, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	AROS_LCA(UBYTE *, (___Key), A1), \
	AROS_LCA(uint32, (___KeyLength), D0), \
	struct Library *, FEELIN_BASE_NAME, 39, /* s */)

#define F_HashRemLink(___Table, ___Link) \
	AROS_LC2(int32, F_HashRemLink, \
	AROS_LCA(FHashTable *, (___Table), A0), \
	AROS_LCA(FHashLink *, (___Link), A1), \
	struct Library *, FEELIN_BASE_NAME, 37, /* s */)

#define F_Layout(___Obj, ___X, ___Y, ___W, ___H, ___Flags) \
	AROS_LC6(void, F_Layout, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(WORD, (___X), D0), \
	AROS_LCA(WORD, (___Y), D1), \
	AROS_LCA(UWORD, (___W), D2), \
	AROS_LCA(UWORD, (___H), D3), \
	AROS_LCA(uint32, (___Flags), D4), \
	struct Library *, FEELIN_BASE_NAME, 65, /* s */)

#define F_LinkHead(___List, ___Node) \
	AROS_LC2(APTR, F_LinkHead, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	struct Library *, FEELIN_BASE_NAME, 21, /* s */)

#define F_LinkInsert(___List, ___Node, ___Prev) \
	AROS_LC3(APTR, F_LinkInsert, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	AROS_LCA(FNode *, (___Prev), A2), \
	struct Library *, FEELIN_BASE_NAME, 22, /* s */)

#define F_LinkMember(___List, ___Node) \
	AROS_LC2(APTR, F_LinkMember, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	struct Library *, FEELIN_BASE_NAME, 25, /* s */)

#define F_LinkMove(___List, ___Node, ___Prev) \
	AROS_LC3(APTR, F_LinkMove, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	AROS_LCA(FNode *, (___Prev), A2), \
	struct Library *, FEELIN_BASE_NAME, 23, /* s */)

#define F_LinkRemove(___List, ___Node) \
	AROS_LC2(APTR, F_LinkRemove, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	struct Library *, FEELIN_BASE_NAME, 24, /* s */)

#define F_LinkTail(___List, ___Node) \
	AROS_LC2(APTR, F_LinkTail, \
	AROS_LCA(FList *, (___List), A0), \
	AROS_LCA(FNode *, (___Node), A1), \
	struct Library *, FEELIN_BASE_NAME, 20, /* s */)

#define F_LogA(___Level, ___Fmt, ___Params) \
	AROS_LC3(void, F_LogA, \
	AROS_LCA(uint32, (___Level), D0), \
	AROS_LCA(STRPTR, (___Fmt), A0), \
	AROS_LCA(int32 *, (___Params), A1), \
	struct Library *, FEELIN_BASE_NAME, 31, /* s */)

#define F_MakeObjA(___Type, ___Params) \
	AROS_LC2(FObject, F_MakeObjA, \
	AROS_LCA(uint32, (___Type), D0), \
	AROS_LCA(uint32 *, (___Params), A0), \
	struct Library *, FEELIN_BASE_NAME, 60, /* s */)

#define F_New(___Size) \
	AROS_LC1(APTR, F_New, \
	AROS_LCA(uint32, (___Size), D0), \
	struct Library *, FEELIN_BASE_NAME, 12, /* s */)

#define F_NewObjA(___Name, ___Tagl) \
	AROS_LC2(FObject, F_NewObjA, \
	AROS_LCA(STRPTR, (___Name), A0), \
	AROS_LCA(struct TagItem *, (___Tagl), A1), \
	struct Library *, FEELIN_BASE_NAME, 59, /* s */)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)
#define F_NewObj(a0, ...) \
	({ULONG _tags[] = { __VA_ARGS__ }; F_NewObjA((a0), (struct TagItem *)_tags);})
#endif

#define F_NewP(___Pool, ___Size) \
	AROS_LC2(APTR, F_NewP, \
	AROS_LCA(APTR, (___Pool), A0), \
	AROS_LCA(uint32, (___Size), D0), \
	struct Library *, FEELIN_BASE_NAME, 13, /* s */)

#define F_NextNode(___Nodeptrptr) \
	AROS_LC1(APTR, F_NextNode, \
	AROS_LCA(APTR *, (___Nodeptrptr), A0), \
	struct Library *, FEELIN_BASE_NAME, 26, /* s */)

#define F_OPool(___Pool) \
	AROS_LC1(void, F_OPool, \
	AROS_LCA(APTR, (___Pool), A0), \
	struct Library *, FEELIN_BASE_NAME, 16, /* s */)

#define F_OpenClass(___Name) \
	AROS_LC1(FClass *, F_OpenClass, \
	AROS_LCA(STRPTR, (___Name), A0), \
	struct Library *, FEELIN_BASE_NAME, 52, /* s */)

#define F_RPool(___Pool) \
	AROS_LC1(void, F_RPool, \
	AROS_LCA(APTR, (___Pool), A0), \
	struct Library *, FEELIN_BASE_NAME, 18, /* s */)

#define F_SPool(___Pool) \
	AROS_LC1(void, F_SPool, \
	AROS_LCA(APTR, (___Pool), A0), \
	struct Library *, FEELIN_BASE_NAME, 17, /* s */)

#define F_Set(___Obj, ___Attribute, ___Value) \
	AROS_LC3(void, F_Set, \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Attribute), D1), \
	AROS_LCA(uint32, (___Value), D2), \
	struct Library *, FEELIN_BASE_NAME, 63, /* s */)

#define F_SharedClose(___Object) \
	AROS_LC1(void, F_SharedClose, \
	AROS_LCA(FObject, (___Object), A0), \
	struct Library *, FEELIN_BASE_NAME, 71, /* s */)

#define F_SharedCreate(___Object, ___Name) \
	AROS_LC2(FObject, F_SharedCreate, \
	AROS_LCA(FObject, (___Object), A0), \
	AROS_LCA(STRPTR, (___Name), A1), \
	struct Library *, FEELIN_BASE_NAME, 68, /* s */)

#define F_SharedDelete(___Object) \
	AROS_LC1(void, F_SharedDelete, \
	AROS_LCA(FObject, (___Object), A0), \
	struct Library *, FEELIN_BASE_NAME, 69, /* s */)

#define F_SharedFind(___Name) \
	AROS_LC1(FObject, F_SharedFind, \
	AROS_LCA(STRPTR, (___Name), A0), \
	struct Library *, FEELIN_BASE_NAME, 67, /* s */)

#define F_SharedOpen(___Name) \
	AROS_LC1(FObject, F_SharedOpen, \
	AROS_LCA(STRPTR, (___Name), A0), \
	struct Library *, FEELIN_BASE_NAME, 70, /* s */)

#define F_StrCmp(___Str1, ___Str2, ___Length) \
	AROS_LC3(int32, F_StrCmp, \
	AROS_LCA(STRPTR, (___Str1), A0), \
	AROS_LCA(STRPTR, (___Str2), A1), \
	AROS_LCA(uint32, (___Length), D1), \
	struct Library *, FEELIN_BASE_NAME, 30, /* s */)

#define F_StrFmtA(___Buffer, ___Fmt, ___Params) \
	AROS_LC3(STRPTR, F_StrFmtA, \
	AROS_LCA(APTR, (___Buffer), A0), \
	AROS_LCA(STRPTR, (___Fmt), A1), \
	AROS_LCA(int32 *, (___Params), A2), \
	struct Library *, FEELIN_BASE_NAME, 27, /* s */)

#define F_StrLen(___Str) \
	AROS_LC1(uint32, F_StrLen, \
	AROS_LCA(STRPTR, (___Str), A0), \
	struct Library *, FEELIN_BASE_NAME, 29, /* s */)

#define F_StrNewA(___Lengthptr, ___Fmt, ___Params) \
	AROS_LC3(STRPTR, F_StrNewA, \
	AROS_LCA(uint32 *, (___Lengthptr), A0), \
	AROS_LCA(STRPTR, (___Fmt), A1), \
	AROS_LCA(int32 *, (___Params), A2), \
	struct Library *, FEELIN_BASE_NAME, 28, /* s */)

#define F_SuperDoA(___feelin_class, ___Obj, ___Method, ___Msg) \
	AROS_LC4(uint32, F_SuperDoA, \
	AROS_LCA(FClass *, (___feelin_class), A2), \
	AROS_LCA(FObject, (___Obj), A0), \
	AROS_LCA(uint32, (___Method), D0), \
	AROS_LCA(APTR, (___Msg), A1), \
	struct Library *, FEELIN_BASE_NAME, 58, /* s */)

#endif /* !_INLINE_FEELIN_H */
