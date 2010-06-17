#ifndef DEFINES_IFFPARSE_H
#define DEFINES_IFFPARSE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/iffparse/iffparse.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for iffparse
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __AllocIFF_WB(__IFFParseBase) \
        AROS_LC0(struct IFFHandle *, AllocIFF, \
        struct Library *, (__IFFParseBase), 5, IFFParse)

#define AllocIFF() \
    __AllocIFF_WB(IFFParseBase)

#define __OpenIFF_WB(__IFFParseBase, __arg1, __arg2) \
        AROS_LC2(LONG, OpenIFF, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__IFFParseBase), 6, IFFParse)

#define OpenIFF(arg1, arg2) \
    __OpenIFF_WB(IFFParseBase, (arg1), (arg2))

#define __ParseIFF_WB(__IFFParseBase, __arg1, __arg2) \
        AROS_LC2(LONG, ParseIFF, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
        struct Library *, (__IFFParseBase), 7, IFFParse)

#define ParseIFF(arg1, arg2) \
    __ParseIFF_WB(IFFParseBase, (arg1), (arg2))

#define __CloseIFF_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, CloseIFF, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 8, IFFParse)

#define CloseIFF(arg1) \
    __CloseIFF_WB(IFFParseBase, (arg1))

#define __FreeIFF_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, FreeIFF, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 9, IFFParse)

#define FreeIFF(arg1) \
    __FreeIFF_WB(IFFParseBase, (arg1))

#define __ReadChunkBytes_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, ReadChunkBytes, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 10, IFFParse)

#define ReadChunkBytes(arg1, arg2, arg3) \
    __ReadChunkBytes_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __WriteChunkBytes_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, WriteChunkBytes, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 11, IFFParse)

#define WriteChunkBytes(arg1, arg2, arg3) \
    __WriteChunkBytes_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __ReadChunkRecords_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, ReadChunkRecords, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct Library *, (__IFFParseBase), 12, IFFParse)

#define ReadChunkRecords(arg1, arg2, arg3, arg4) \
    __ReadChunkRecords_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4))

#define __WriteChunkRecords_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, WriteChunkRecords, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(APTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
                  AROS_LCA(LONG,(__arg4),D1), \
        struct Library *, (__IFFParseBase), 13, IFFParse)

#define WriteChunkRecords(arg1, arg2, arg3, arg4) \
    __WriteChunkRecords_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4))

#define __PushChunk_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, PushChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
        struct Library *, (__IFFParseBase), 14, IFFParse)

#define PushChunk(arg1, arg2, arg3, arg4) \
    __PushChunk_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4))

#define __PopChunk_WB(__IFFParseBase, __arg1) \
        AROS_LC1(LONG, PopChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 15, IFFParse)

#define PopChunk(arg1) \
    __PopChunk_WB(IFFParseBase, (arg1))

#define __EntryHandler_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6(LONG, EntryHandler, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
                  AROS_LCA(struct Hook *,(__arg5),A1), \
                  AROS_LCA(APTR,(__arg6),A2), \
        struct Library *, (__IFFParseBase), 17, IFFParse)

#define EntryHandler(arg1, arg2, arg3, arg4, arg5, arg6) \
    __EntryHandler_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __ExitHandler_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6) \
        AROS_LC6(LONG, ExitHandler, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
                  AROS_LCA(struct Hook *,(__arg5),A1), \
                  AROS_LCA(APTR,(__arg6),A2), \
        struct Library *, (__IFFParseBase), 18, IFFParse)

#define ExitHandler(arg1, arg2, arg3, arg4, arg5, arg6) \
    __ExitHandler_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4), (arg5), (arg6))

#define __PropChunk_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, PropChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 19, IFFParse)

#define PropChunk(arg1, arg2, arg3) \
    __PropChunk_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __PropChunks_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, PropChunks, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 20, IFFParse)

#define PropChunks(arg1, arg2, arg3) \
    __PropChunks_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __StopChunk_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, StopChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 21, IFFParse)

#define StopChunk(arg1, arg2, arg3) \
    __StopChunk_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __StopChunks_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, StopChunks, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 22, IFFParse)

#define StopChunks(arg1, arg2, arg3) \
    __StopChunks_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __CollectionChunk_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, CollectionChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 23, IFFParse)

#define CollectionChunk(arg1, arg2, arg3) \
    __CollectionChunk_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __CollectionChunks_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, CollectionChunks, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 24, IFFParse)

#define CollectionChunks(arg1, arg2, arg3) \
    __CollectionChunks_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __StopOnExit_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, StopOnExit, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 25, IFFParse)

#define StopOnExit(arg1, arg2, arg3) \
    __StopOnExit_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __FindProp_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct StoredProperty *, FindProp, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 26, IFFParse)

#define FindProp(arg1, arg2, arg3) \
    __FindProp_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __FindCollection_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(struct CollectionItem *, FindCollection, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
        struct Library *, (__IFFParseBase), 27, IFFParse)

#define FindCollection(arg1, arg2, arg3) \
    __FindCollection_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __FindPropContext_WB(__IFFParseBase, __arg1) \
        AROS_LC1(struct ContextNode *, FindPropContext, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 28, IFFParse)

#define FindPropContext(arg1) \
    __FindPropContext_WB(IFFParseBase, (arg1))

#define __CurrentChunk_WB(__IFFParseBase, __arg1) \
        AROS_LC1(struct ContextNode *, CurrentChunk, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 29, IFFParse)

#define CurrentChunk(arg1) \
    __CurrentChunk_WB(IFFParseBase, (arg1))

#define __ParentChunk_WB(__IFFParseBase, __arg1) \
        AROS_LC1(struct ContextNode *, ParentChunk, \
                  AROS_LCA(struct ContextNode *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 30, IFFParse)

#define ParentChunk(arg1) \
    __ParentChunk_WB(IFFParseBase, (arg1))

#define __AllocLocalItem_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(struct LocalContextItem *, AllocLocalItem, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(LONG,(__arg2),D1), \
                  AROS_LCA(LONG,(__arg3),D2), \
                  AROS_LCA(ULONG,(__arg4),D3), \
        struct Library *, (__IFFParseBase), 31, IFFParse)

#define AllocLocalItem(arg1, arg2, arg3, arg4) \
    __AllocLocalItem_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4))

#define __LocalItemData_WB(__IFFParseBase, __arg1) \
        AROS_LC1(APTR, LocalItemData, \
                  AROS_LCA(struct LocalContextItem *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 32, IFFParse)

#define LocalItemData(arg1) \
    __LocalItemData_WB(IFFParseBase, (arg1))

#define __SetLocalItemPurge_WB(__IFFParseBase, __arg1, __arg2) \
        AROS_LC2NR(void, SetLocalItemPurge, \
                  AROS_LCA(struct LocalContextItem *,(__arg1),A0), \
                  AROS_LCA(struct Hook *,(__arg2),A1), \
        struct Library *, (__IFFParseBase), 33, IFFParse)

#define SetLocalItemPurge(arg1, arg2) \
    __SetLocalItemPurge_WB(IFFParseBase, (arg1), (arg2))

#define __FreeLocalItem_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, FreeLocalItem, \
                  AROS_LCA(struct LocalContextItem *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 34, IFFParse)

#define FreeLocalItem(arg1) \
    __FreeLocalItem_WB(IFFParseBase, (arg1))

#define __FindLocalItem_WB(__IFFParseBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(struct LocalContextItem *, FindLocalItem, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(LONG,(__arg3),D1), \
                  AROS_LCA(LONG,(__arg4),D2), \
        struct Library *, (__IFFParseBase), 35, IFFParse)

#define FindLocalItem(arg1, arg2, arg3, arg4) \
    __FindLocalItem_WB(IFFParseBase, (arg1), (arg2), (arg3), (arg4))

#define __StoreLocalItem_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3(LONG, StoreLocalItem, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(struct LocalContextItem *,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__IFFParseBase), 36, IFFParse)

#define StoreLocalItem(arg1, arg2, arg3) \
    __StoreLocalItem_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __StoreItemInContext_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, StoreItemInContext, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(struct LocalContextItem *,(__arg2),A1), \
                  AROS_LCA(struct ContextNode *,(__arg3),A2), \
        struct Library *, (__IFFParseBase), 37, IFFParse)

#define StoreItemInContext(arg1, arg2, arg3) \
    __StoreItemInContext_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __InitIFF_WB(__IFFParseBase, __arg1, __arg2, __arg3) \
        AROS_LC3NR(void, InitIFF, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
                  AROS_LCA(LONG,(__arg2),D0), \
                  AROS_LCA(struct Hook *,(__arg3),A1), \
        struct Library *, (__IFFParseBase), 38, IFFParse)

#define InitIFF(arg1, arg2, arg3) \
    __InitIFF_WB(IFFParseBase, (arg1), (arg2), (arg3))

#define __InitIFFasDOS_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, InitIFFasDOS, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 39, IFFParse)

#define InitIFFasDOS(arg1) \
    __InitIFFasDOS_WB(IFFParseBase, (arg1))

#define __InitIFFasClip_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, InitIFFasClip, \
                  AROS_LCA(struct IFFHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 40, IFFParse)

#define InitIFFasClip(arg1) \
    __InitIFFasClip_WB(IFFParseBase, (arg1))

#define __OpenClipboard_WB(__IFFParseBase, __arg1) \
        AROS_LC1(struct ClipboardHandle *, OpenClipboard, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__IFFParseBase), 41, IFFParse)

#define OpenClipboard(arg1) \
    __OpenClipboard_WB(IFFParseBase, (arg1))

#define __CloseClipboard_WB(__IFFParseBase, __arg1) \
        AROS_LC1NR(void, CloseClipboard, \
                  AROS_LCA(struct ClipboardHandle *,(__arg1),A0), \
        struct Library *, (__IFFParseBase), 42, IFFParse)

#define CloseClipboard(arg1) \
    __CloseClipboard_WB(IFFParseBase, (arg1))

#define __GoodID_WB(__IFFParseBase, __arg1) \
        AROS_LC1(LONG, GoodID, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__IFFParseBase), 43, IFFParse)

#define GoodID(arg1) \
    __GoodID_WB(IFFParseBase, (arg1))

#define __GoodType_WB(__IFFParseBase, __arg1) \
        AROS_LC1(LONG, GoodType, \
                  AROS_LCA(LONG,(__arg1),D0), \
        struct Library *, (__IFFParseBase), 44, IFFParse)

#define GoodType(arg1) \
    __GoodType_WB(IFFParseBase, (arg1))

#define __IDtoStr_WB(__IFFParseBase, __arg1, __arg2) \
        AROS_LC2(STRPTR, IDtoStr, \
                  AROS_LCA(LONG,(__arg1),D0), \
                  AROS_LCA(STRPTR,(__arg2),A0), \
        struct Library *, (__IFFParseBase), 45, IFFParse)

#define IDtoStr(arg1, arg2) \
    __IDtoStr_WB(IFFParseBase, (arg1), (arg2))

__END_DECLS

#endif /* DEFINES_IFFPARSE_H*/
