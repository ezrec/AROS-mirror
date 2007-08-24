/* Automatically generated header! Do not edit! */

#ifndef _PPCINLINE_FEELIN_H
#define _PPCINLINE_FEELIN_H

#ifndef __PPCINLINE_MACROS_H
#include <ppcinline/macros.h>
#endif /* !__PPCINLINE_MACROS_H */

#ifndef FEELIN_BASE_NAME
#define FEELIN_BASE_NAME FeelinBase
#endif /* !FEELIN_BASE_NAME */

#undef F_StrNewA
#define F_StrNewA(__p0, __p1, __p2) \
    LP3(168, STRPTR , F_StrNewA, \
        uint32 *, __p0, a0, \
        STRPTR , __p1, a1, \
        int32 *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_NextNode
#define F_NextNode(__p0) \
    LP1(156, APTR , F_NextNode, \
        APTR *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicFTI
#define F_DynamicFTI(__p0, __p1) \
    LP2(294, struct TagItem *, F_DynamicFTI, \
        uint32 , __p0, d0, \
        struct TagItem *, __p1, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_OPool
#define F_OPool(__p0) \
    LP1NR(96, F_OPool, \
        APTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_ClassDoA
#define F_ClassDoA(__p0, __p1, __p2, __p3) \
    LP4(342, uint32 , F_ClassDoA, \
        FClass *, __p0, a2, \
        FObject , __p1, a0, \
        uint32 , __p2, d0, \
        APTR , __p3, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_New
#define F_New(__p0) \
    LP1(72, APTR , F_New, \
        uint32 , __p0, d0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_CloseClass
#define F_CloseClass(__p0) \
    LP1NR(318, F_CloseClass, \
        FClass *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_RPool
#define F_RPool(__p0) \
    LP1NR(108, F_RPool, \
        APTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicResolveTable
#define F_DynamicResolveTable(__p0) \
    LP1NR(270, F_DynamicResolveTable, \
        FDynamicEntry *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SPool
#define F_SPool(__p0) \
    LP1NR(102, F_SPool, \
        APTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Draw
#define F_Draw(__p0, __p1) \
    LP2NR(384, F_Draw, \
        FObject , __p0, a0, \
        uint32 , __p1, d0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicNTI
#define F_DynamicNTI(__p0, __p1, __p2) \
    LP3(288, struct TagItem *, F_DynamicNTI, \
        struct TagItem **, __p0, a0, \
        struct TagItem *, __p1, a1, \
        FClass *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicFindID
#define F_DynamicFindID(__p0) \
    LP1(264, uint32 , F_DynamicFindID, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DoA
#define F_DoA(__p0, __p1, __p2) \
    LP3(336, uint32 , F_DoA, \
        FObject , __p0, a0, \
        uint32 , __p1, d0, \
        APTR , __p2, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SharedFind
#define F_SharedFind(__p0) \
    LP1(402, FObject , F_SharedFind, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashCreate
#define F_HashCreate(__p0) \
    LP1(198, FHashTable *, F_HashCreate, \
        uint32 , __p0, d0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_MakeObjA
#define F_MakeObjA(__p0, __p1) \
    LP2(360, FObject , F_MakeObjA, \
        uint32 , __p0, d0, \
        uint32 *, __p1, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_StrFmtA
#define F_StrFmtA(__p0, __p1, __p2) \
    LP3(162, STRPTR , F_StrFmtA, \
        APTR , __p0, a0, \
        STRPTR , __p1, a1, \
        int32 *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkMember
#define F_LinkMember(__p0, __p1) \
    LP2(150, APTR , F_LinkMember, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_OpenClass
#define F_OpenClass(__p0) \
    LP1(312, FClass *, F_OpenClass, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashRem
#define F_HashRem(__p0, __p1, __p2) \
    LP3(234, int32 , F_HashRem, \
        FHashTable *, __p0, a0, \
        UBYTE *, __p1, a1, \
        uint32 , __p2, d0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SharedOpen
#define F_SharedOpen(__p0) \
    LP1(420, FObject , F_SharedOpen, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SharedCreate
#define F_SharedCreate(__p0, __p1) \
    LP2(408, FObject , F_SharedCreate, \
        FObject , __p0, a0, \
        STRPTR , __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashAdd
#define F_HashAdd(__p0, __p1, __p2, __p3) \
    LP4(228, FHashLink *, F_HashAdd, \
        FHashTable *, __p0, a0, \
        UBYTE *, __p1, a1, \
        uint32 , __p2, d0, \
        APTR , __p3, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashRemLink
#define F_HashRemLink(__p0, __p1) \
    LP2(222, int32 , F_HashRemLink, \
        FHashTable *, __p0, a0, \
        FHashLink *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_StrLen
#define F_StrLen(__p0) \
    LP1(174, uint32 , F_StrLen, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicCreate
#define F_DynamicCreate(__p0) \
    LP1(240, int32 , F_DynamicCreate, \
        FClass *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_NewP
#define F_NewP(__p0, __p1) \
    LP2(78, APTR , F_NewP, \
        APTR , __p0, a0, \
        uint32 , __p1, d0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashAddLink
#define F_HashAddLink(__p0, __p1) \
    LP2(216, FHashLink *, F_HashAddLink, \
        FHashTable *, __p0, a0, \
        FHashLink *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkRemove
#define F_LinkRemove(__p0, __p1) \
    LP2(144, APTR , F_LinkRemove, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkHead
#define F_LinkHead(__p0, __p1) \
    LP2(126, APTR , F_LinkHead, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Get
#define F_Get(__p0, __p1) \
    LP2(372, uint32 , F_Get, \
        FObject , __p0, a0, \
        uint32 , __p1, d1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicGTD
#define F_DynamicGTD(__p0, __p1, __p2) \
    LP3(300, uint32 , F_DynamicGTD, \
        uint32 , __p0, d0, \
        uint32 , __p1, d1, \
        struct TagItem *, __p2, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashFind
#define F_HashFind(__p0, __p1, __p2, __p3) \
    LP4(210, FHashLink *, F_HashFind, \
        FHashTable *, __p0, a0, \
        UBYTE *, __p1, a1, \
        uint32 , __p2, d1, \
        uint32 *, __p3, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_NewObjA
#define F_NewObjA(__p0, __p1) \
    LP2(354, FObject , F_NewObjA, \
        STRPTR , __p0, a0, \
        struct TagItem *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicFindMethod
#define F_DynamicFindMethod(__p0, __p1, __p2) \
    LP3(258, FClassMethod *, F_DynamicFindMethod, \
        STRPTR , __p0, a0, \
        FClass *, __p1, a1, \
        FClass **, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DeleteClass
#define F_DeleteClass(__p0) \
    LP1NR(330, F_DeleteClass, \
        FClass *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicRemAutoTable
#define F_DynamicRemAutoTable(__p0) \
    LP1NR(282, F_DynamicRemAutoTable, \
        FDynamicEntry *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_CreateClassA
#define F_CreateClassA(__p0, __p1) \
    LP2(324, FClass *, F_CreateClassA, \
        STRPTR , __p0, a0, \
        struct TagItem *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Set
#define F_Set(__p0, __p1, __p2) \
    LP3NR(378, F_Set, \
        FObject , __p0, a0, \
        uint32 , __p1, d1, \
        uint32 , __p2, d2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_CreatePoolA
#define F_CreatePoolA(__p0, __p1) \
    LP2(30, APTR , F_CreatePoolA, \
        uint32 , __p0, d0, \
        struct TagItem *, __p1, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Erase
#define F_Erase(__p0, __p1, __p2, __p3, __p4, __p5) \
    LP6NR(396, F_Erase, \
        FObject , __p0, a0, \
        WORD , __p1, d0, \
        WORD , __p2, d1, \
        WORD , __p3, d2, \
        WORD , __p4, d3, \
        uint32 , __p5, d4, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Dispose
#define F_Dispose(__p0) \
    LP1NR(84, F_Dispose, \
        APTR , __p0, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_HashDelete
#define F_HashDelete(__p0) \
    LP1NR(204, F_HashDelete, \
        FHashTable *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkTail
#define F_LinkTail(__p0, __p1) \
    LP2(120, APTR , F_LinkTail, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_AlertA
#define F_AlertA(__p0, __p1, __p2) \
    LP3NR(192, F_AlertA, \
        STRPTR , __p0, a0, \
        STRPTR , __p1, a1, \
        int32 *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_Layout
#define F_Layout(__p0, __p1, __p2, __p3, __p4, __p5) \
    LP6NR(390, F_Layout, \
        FObject , __p0, a0, \
        WORD , __p1, d0, \
        WORD , __p2, d1, \
        UWORD , __p3, d2, \
        UWORD , __p4, d3, \
        uint32 , __p5, d4, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LogA
#define F_LogA(__p0, __p1, __p2) \
    LP3NR(186, F_LogA, \
        uint32 , __p0, d0, \
        STRPTR , __p1, a0, \
        int32 *, __p2, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkInsert
#define F_LinkInsert(__p0, __p1, __p2) \
    LP3(132, APTR , F_LinkInsert, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        FNode *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SuperDoA
#define F_SuperDoA(__p0, __p1, __p2, __p3) \
    LP4(348, uint32 , F_SuperDoA, \
        FClass *, __p0, a2, \
        FObject , __p1, a0, \
        uint32 , __p2, d0, \
        APTR , __p3, a1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_LinkMove
#define F_LinkMove(__p0, __p1, __p2) \
    LP3(138, APTR , F_LinkMove, \
        FList *, __p0, a0, \
        FNode *, __p1, a1, \
        FNode *, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SharedDelete
#define F_SharedDelete(__p0) \
    LP1NR(414, F_SharedDelete, \
        FObject , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DeletePool
#define F_DeletePool(__p0) \
    LP1NR(36, F_DeletePool, \
        APTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_SharedClose
#define F_SharedClose(__p0) \
    LP1NR(426, F_SharedClose, \
        FObject , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicAddAutoTable
#define F_DynamicAddAutoTable(__p0) \
    LP1(276, int32 , F_DynamicAddAutoTable, \
        FDynamicEntry *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DisposeObj
#define F_DisposeObj(__p0) \
    LP1NR(366, F_DisposeObj, \
        FObject , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_FindClass
#define F_FindClass(__p0) \
    LP1(306, FClass *, F_FindClass, \
        STRPTR , __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicDelete
#define F_DynamicDelete(__p0) \
    LP1NR(246, F_DynamicDelete, \
        FClass *, __p0, a0, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_StrCmp
#define F_StrCmp(__p0, __p1, __p2) \
    LP3(180, int32 , F_StrCmp, \
        STRPTR , __p0, a0, \
        STRPTR , __p1, a1, \
        uint32 , __p2, d1, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#undef F_DynamicFindAttribute
#define F_DynamicFindAttribute(__p0, __p1, __p2) \
    LP3(252, FClassAttribute *, F_DynamicFindAttribute, \
        STRPTR , __p0, a0, \
        FClass *, __p1, a1, \
        FClass **, __p2, a2, \
        , FEELIN_BASE_NAME, 0, 0, 0, 0, 0, 0)

#if defined(USE_INLINE_STDARG) && !defined(__STRICT_ANSI__)

#include <stdarg.h>

#undef F_ClassDo
#define F_ClassDo(__p0, __p1, __p2, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_ClassDoA(__p0, __p1, __p2, (APTR )_tags);})

#undef F_MakeObj
#define F_MakeObj(__p0, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_MakeObjA(__p0, (uint32 *)_tags);})

#undef F_CreateClass
#define F_CreateClass(__p0, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_CreateClassA(__p0, (struct TagItem *)_tags);})

#undef F_CreatePool
#define F_CreatePool(__p0, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_CreatePoolA(__p0, (struct TagItem *)_tags);})

#undef F_StrFmt
#define F_StrFmt(__p0, __p1, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_StrFmtA(__p0, __p1, (int32 *)_tags);})

#undef F_SuperDo
#define F_SuperDo(__p0, __p1, __p2, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_SuperDoA(__p0, __p1, __p2, (APTR )_tags);})

#undef F_Log
#define F_Log(__p0, __p1, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_LogA(__p0, __p1, (int32 *)_tags);})

#undef F_StrNew
#define F_StrNew(__p0, __p1, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_StrNewA(__p0, __p1, (int32 *)_tags);})

#undef F_NewObj
#define F_NewObj(__p0, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_NewObjA(__p0, (struct TagItem *)_tags);})

#undef F_Do
#define F_Do(__p0, __p1, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_DoA(__p0, __p1, (APTR )_tags);})

#undef F_Alert
#define F_Alert(__p0, __p1, ...) \
    ({ULONG _tags[] = { __VA_ARGS__ }; \
    F_AlertA(__p0, __p1, (int32 *)_tags);})

#endif

#endif /* !_PPCINLINE_FEELIN_H */
