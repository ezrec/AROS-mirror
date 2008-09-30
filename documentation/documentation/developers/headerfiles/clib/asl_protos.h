#ifndef CLIB_ASL_PROTOS_H
#define CLIB_ASL_PROTOS_H

/*
    *** Automatically generated from 'asl.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <utility/tagitem.h>
#include <libraries/asl.h>

APTR AllocAslRequestTags(ULONG reqType, Tag tag1, ...);
BOOL AslRequestTags(APTR requester, Tag tag1, ...);
AROS_LP0(struct FileRequester *, AllocFileRequest,
         LIBBASETYPEPTR, AslBase, 5, Asl
);
AROS_LP1(void, FreeFileRequest,
         AROS_LPA(struct FileRequester *, fileReq, A0),
         LIBBASETYPEPTR, AslBase, 6, Asl
);
AROS_LP1(BOOL, RequestFile,
         AROS_LPA(struct FileRequester *, fileReq, A0),
         LIBBASETYPEPTR, AslBase, 7, Asl
);
AROS_LP2(APTR, AllocAslRequest,
         AROS_LPA(ULONG, reqType, D0),
         AROS_LPA(struct TagItem *, tagList, A0),
         LIBBASETYPEPTR, AslBase, 8, Asl
);
AROS_LP1(void, FreeAslRequest,
         AROS_LPA(APTR, requester, A0),
         LIBBASETYPEPTR, AslBase, 9, Asl
);
AROS_LP2(BOOL, AslRequest,
         AROS_LPA(APTR, requester, A0),
         AROS_LPA(struct TagItem *, tagList, A1),
         LIBBASETYPEPTR, AslBase, 10, Asl
);
AROS_LP1(void, AbortAslRequest,
         AROS_LPA(APTR, requester, A0),
         LIBBASETYPEPTR, AslBase, 13, Asl
);
AROS_LP1(void, ActivateAslRequest,
         AROS_LPA(APTR, requester, A0),
         LIBBASETYPEPTR, AslBase, 14, Asl
);

#endif /* CLIB_ASL_PROTOS_H */
