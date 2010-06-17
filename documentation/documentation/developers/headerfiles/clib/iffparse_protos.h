#ifndef CLIB_IFFPARSE_PROTOS_H
#define CLIB_IFFPARSE_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/iffparse/iffparse.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <utility/utility.h>
#include <utility/hooks.h>
#include <libraries/iffparse.h>

__BEGIN_DECLS

AROS_LP0(struct IFFHandle *, AllocIFF,
         LIBBASETYPEPTR, IFFParseBase, 5, IFFParse
);
AROS_LP2(LONG, OpenIFF,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, rwMode, D0),
         LIBBASETYPEPTR, IFFParseBase, 6, IFFParse
);
AROS_LP2(LONG, ParseIFF,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, mode, D0),
         LIBBASETYPEPTR, IFFParseBase, 7, IFFParse
);
AROS_LP1(void, CloseIFF,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 8, IFFParse
);
AROS_LP1(void, FreeIFF,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 9, IFFParse
);
AROS_LP3(LONG, ReadChunkBytes,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(APTR, buf, A1),
         AROS_LPA(LONG, numBytes, D0),
         LIBBASETYPEPTR, IFFParseBase, 10, IFFParse
);
AROS_LP3(LONG, WriteChunkBytes,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(APTR, buf, A1),
         AROS_LPA(LONG, numBytes, D0),
         LIBBASETYPEPTR, IFFParseBase, 11, IFFParse
);
AROS_LP4(LONG, ReadChunkRecords,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(APTR, buf, A1),
         AROS_LPA(LONG, bytesPerRecord, D0),
         AROS_LPA(LONG, numRecords, D1),
         LIBBASETYPEPTR, IFFParseBase, 12, IFFParse
);
AROS_LP4(LONG, WriteChunkRecords,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(APTR, buf, A1),
         AROS_LPA(LONG, bytesPerRecord, D0),
         AROS_LPA(LONG, numRecords, D1),
         LIBBASETYPEPTR, IFFParseBase, 13, IFFParse
);
AROS_LP4(LONG, PushChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         AROS_LPA(LONG, size, D2),
         LIBBASETYPEPTR, IFFParseBase, 14, IFFParse
);
AROS_LP1(LONG, PopChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 15, IFFParse
);
AROS_LP6(LONG, EntryHandler,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         AROS_LPA(LONG, position, D2),
         AROS_LPA(struct Hook *, handler, A1),
         AROS_LPA(APTR, object, A2),
         LIBBASETYPEPTR, IFFParseBase, 17, IFFParse
);
AROS_LP6(LONG, ExitHandler,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         AROS_LPA(LONG, position, D2),
         AROS_LPA(struct Hook *, handler, A1),
         AROS_LPA(APTR, object, A2),
         LIBBASETYPEPTR, IFFParseBase, 18, IFFParse
);
AROS_LP3(LONG, PropChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 19, IFFParse
);
AROS_LP3(LONG, PropChunks,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG *, propArray, A1),
         AROS_LPA(LONG, numPairs, D0),
         LIBBASETYPEPTR, IFFParseBase, 20, IFFParse
);
AROS_LP3(LONG, StopChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 21, IFFParse
);
AROS_LP3(LONG, StopChunks,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG *, propArray, A1),
         AROS_LPA(LONG, numPairs, D0),
         LIBBASETYPEPTR, IFFParseBase, 22, IFFParse
);
AROS_LP3(LONG, CollectionChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 23, IFFParse
);
AROS_LP3(LONG, CollectionChunks,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG *, propArray, A1),
         AROS_LPA(LONG, numPairs, D0),
         LIBBASETYPEPTR, IFFParseBase, 24, IFFParse
);
AROS_LP3(LONG, StopOnExit,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 25, IFFParse
);
AROS_LP3(struct StoredProperty *, FindProp,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 26, IFFParse
);
AROS_LP3(struct CollectionItem *, FindCollection,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         LIBBASETYPEPTR, IFFParseBase, 27, IFFParse
);
AROS_LP1(struct ContextNode *, FindPropContext,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 28, IFFParse
);
AROS_LP1(struct ContextNode *, CurrentChunk,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 29, IFFParse
);
AROS_LP1(struct ContextNode *, ParentChunk,
         AROS_LPA(struct ContextNode *, contextNode, A0),
         LIBBASETYPEPTR, IFFParseBase, 30, IFFParse
);
AROS_LP4(struct LocalContextItem *, AllocLocalItem,
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         AROS_LPA(LONG, ident, D2),
         AROS_LPA(ULONG, dataSize, D3),
         LIBBASETYPEPTR, IFFParseBase, 31, IFFParse
);
AROS_LP1(APTR, LocalItemData,
         AROS_LPA(struct LocalContextItem *, localItem, A0),
         LIBBASETYPEPTR, IFFParseBase, 32, IFFParse
);
AROS_LP2(void, SetLocalItemPurge,
         AROS_LPA(struct LocalContextItem *, localItem, A0),
         AROS_LPA(struct Hook *, purgeHook, A1),
         LIBBASETYPEPTR, IFFParseBase, 33, IFFParse
);
AROS_LP1(void, FreeLocalItem,
         AROS_LPA(struct LocalContextItem *, localItem, A0),
         LIBBASETYPEPTR, IFFParseBase, 34, IFFParse
);
AROS_LP4(struct LocalContextItem *, FindLocalItem,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, type, D0),
         AROS_LPA(LONG, id, D1),
         AROS_LPA(LONG, ident, D2),
         LIBBASETYPEPTR, IFFParseBase, 35, IFFParse
);
AROS_LP3(LONG, StoreLocalItem,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(struct LocalContextItem *, localItem, A1),
         AROS_LPA(LONG, position, D0),
         LIBBASETYPEPTR, IFFParseBase, 36, IFFParse
);
AROS_LP3(void, StoreItemInContext,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(struct LocalContextItem *, localItem, A1),
         AROS_LPA(struct ContextNode *, contextNode, A2),
         LIBBASETYPEPTR, IFFParseBase, 37, IFFParse
);
AROS_LP3(void, InitIFF,
         AROS_LPA(struct IFFHandle *, iff, A0),
         AROS_LPA(LONG, flags, D0),
         AROS_LPA(struct Hook *, streamHook, A1),
         LIBBASETYPEPTR, IFFParseBase, 38, IFFParse
);
AROS_LP1(void, InitIFFasDOS,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 39, IFFParse
);
AROS_LP1(void, InitIFFasClip,
         AROS_LPA(struct IFFHandle *, iff, A0),
         LIBBASETYPEPTR, IFFParseBase, 40, IFFParse
);
AROS_LP1(struct ClipboardHandle *, OpenClipboard,
         AROS_LPA(LONG, unitNumber, D0),
         LIBBASETYPEPTR, IFFParseBase, 41, IFFParse
);
AROS_LP1(void, CloseClipboard,
         AROS_LPA(struct ClipboardHandle *, clipHandle, A0),
         LIBBASETYPEPTR, IFFParseBase, 42, IFFParse
);
AROS_LP1(LONG, GoodID,
         AROS_LPA(LONG, id, D0),
         LIBBASETYPEPTR, IFFParseBase, 43, IFFParse
);
AROS_LP1(LONG, GoodType,
         AROS_LPA(LONG, type, D0),
         LIBBASETYPEPTR, IFFParseBase, 44, IFFParse
);
AROS_LP2(STRPTR, IDtoStr,
         AROS_LPA(LONG, id, D0),
         AROS_LPA(STRPTR, buf, A0),
         LIBBASETYPEPTR, IFFParseBase, 45, IFFParse
);

__END_DECLS

#endif /* CLIB_IFFPARSE_PROTOS_H */
