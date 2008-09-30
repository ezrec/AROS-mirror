#ifndef CLIB_CODESETS_PROTOS_H
#define CLIB_CODESETS_PROTOS_H

/*
    *** Automatically generated from 'codesets.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <libraries/codesets.h>

struct codeset * CodesetsSetDefault(STRPTR name, Tag attrs, ...);
void CodesetsFree(APTR obj, Tag attrs, ...);
STRPTR * CodesetsSupported(Tag attrs, ...);
struct codeset * CodesetsFind(STRPTR name, Tag attrs, ...);
struct codeset * CodesetsFindBest(Tag attrs, ...);
STRPTR CodesetsUTF8ToStr(Tag attrs, ...);
UTF8 * CodesetsUTF8Create(Tag attrs, ...);
ULONG CodesetsEncodeB64(Tag attrs, ...);
ULONG CodesetsDecodeB64(Tag attrs, ...);
ULONG CodesetsStrLen(STRPTR str, Tag attrs, ...);
void CodesetsFreeVecPooled(APTR pool, APTR mem, Tag attrs, ...);
STRPTR CodesetsConvertStr(Tag attrs, ...);
struct codesetList * CodesetsListCreate(Tag attrs, ...);
BOOL CodesetsListDelete(Tag tag1, ...);
BOOL CodesetsListAdd(struct codesetList *codesetsList, Tag attrs, ...);
BOOL CodesetsListRemove(Tag tag1, ...);
AROS_LP5(ULONG, CodesetsConvertUTF32toUTF16,
         AROS_LPA(const UTF32 **, sourceStart, A0),
         AROS_LPA(const UTF32 *, sourceEnd, A1),
         AROS_LPA(UTF16 **, targetStart, A2),
         AROS_LPA(UTF16 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 5, Codesets
);
AROS_LP5(ULONG, CodesetsConvertUTF16toUTF32,
         AROS_LPA(const  UTF16 **, sourceStart, A0),
         AROS_LPA(const UTF16 *, sourceEnd, A1),
         AROS_LPA(UTF32 **, targetStart, A2),
         AROS_LPA(UTF32 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 6, Codesets
);
AROS_LP5(ULONG, CodesetsConvertUTF16toUTF8,
         AROS_LPA(const UTF16 **, sourceStart, A0),
         AROS_LPA(const UTF16 *, sourceEnd, A1),
         AROS_LPA(UTF8 **, targetStart, A2),
         AROS_LPA(UTF8 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 7, Codesets
);
AROS_LP2(BOOL, CodesetsIsLegalUTF8,
         AROS_LPA(const UTF8 *, source, A0),
         AROS_LPA(ULONG, length, D0),
         LIBBASETYPEPTR, CodesetsBase, 8, Codesets
);
AROS_LP2(BOOL, CodesetsIsLegalUTF8Sequence,
         AROS_LPA(const UTF8 *, source, A0),
         AROS_LPA(const UTF8 *, sourceEnd, D1),
         LIBBASETYPEPTR, CodesetsBase, 9, Codesets
);
AROS_LP5(ULONG, CodesetsConvertUTF8toUTF16,
         AROS_LPA(const UTF8 **, sourceStart, A0),
         AROS_LPA(const UTF8 *, sourceEnd, A1),
         AROS_LPA(UTF16 **, targetStart, A2),
         AROS_LPA(UTF16 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 10, Codesets
);
AROS_LP5(ULONG, CodesetsConvertUTF32toUTF8,
         AROS_LPA(const UTF32 **, sourceStart, A0),
         AROS_LPA(const UTF32 *, sourceEnd, A1),
         AROS_LPA(UTF8 **, targetStart, A2),
         AROS_LPA(UTF8 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 11, Codesets
);
AROS_LP5(ULONG, CodesetsConvertUTF8toUTF32,
         AROS_LPA(const UTF8 **, sourceStart, A0),
         AROS_LPA(const UTF8 *, sourceEnd, A1),
         AROS_LPA(UTF32 **, targetStart, A2),
         AROS_LPA(UTF32 *, targetEnd, A3),
         AROS_LPA(ULONG, flags, D0),
         LIBBASETYPEPTR, CodesetsBase, 12, Codesets
);
AROS_LP2(struct codeset *, CodesetsSetDefaultA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, CodesetsBase, 13, Codesets
);
AROS_LP2(void, CodesetsFreeA,
         AROS_LPA(APTR, obj, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, CodesetsBase, 14, Codesets
);
AROS_LP1(STRPTR *, CodesetsSupportedA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 15, Codesets
);
AROS_LP2(struct codeset *, CodesetsFindA,
         AROS_LPA(STRPTR, name, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, CodesetsBase, 16, Codesets
);
AROS_LP1(struct codeset *, CodesetsFindBestA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 17, Codesets
);
AROS_LP1(ULONG, CodesetsUTF8Len,
         AROS_LPA(const UTF8 *, str, A0),
         LIBBASETYPEPTR, CodesetsBase, 18, Codesets
);
AROS_LP1(STRPTR, CodesetsUTF8ToStrA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 19, Codesets
);
AROS_LP1(UTF8 *, CodesetsUTF8CreateA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 20, Codesets
);
AROS_LP1(ULONG, CodesetsEncodeB64A,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 21, Codesets
);
AROS_LP1(ULONG, CodesetsDecodeB64A,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 22, Codesets
);
AROS_LP2(ULONG, CodesetsStrLenA,
         AROS_LPA(STRPTR, str, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, CodesetsBase, 23, Codesets
);
AROS_LP1(BOOL, CodesetsIsValidUTF8,
         AROS_LPA(STRPTR, str, A0),
         LIBBASETYPEPTR, CodesetsBase, 24, Codesets
);
AROS_LP3(void, CodesetsFreeVecPooledA,
         AROS_LPA(APTR, pool, A0),
         AROS_LPA(APTR, mem, A1),
         AROS_LPA(struct TagItem *, attrs, A2),
         LIBBASETYPEPTR, CodesetsBase, 25, Codesets
);
AROS_LP1(STRPTR, CodesetsConvertStrA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 26, Codesets
);
AROS_LP1(struct codesetList *, CodesetsListCreateA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 27, Codesets
);
AROS_LP1(BOOL, CodesetsListDeleteA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 28, Codesets
);
AROS_LP2(BOOL, CodesetsListAddA,
         AROS_LPA(struct codesetList *, codesetsList, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, CodesetsBase, 29, Codesets
);
AROS_LP1(BOOL, CodesetsListRemoveA,
         AROS_LPA(struct TagItem *, attrs, A0),
         LIBBASETYPEPTR, CodesetsBase, 30, Codesets
);

#endif /* CLIB_CODESETS_PROTOS_H */
