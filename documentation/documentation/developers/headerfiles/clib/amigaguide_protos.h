#ifndef CLIB_AMIGAGUIDE_PROTOS_H
#define CLIB_AMIGAGUIDE_PROTOS_H

/*
    *** Automatically generated from 'amigaguide.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#ifndef LIBRARIES_AMIGAGUIDE_H
#   include <libraries/amigaguide.h>
#endif

/* Prototypes for stubs in amiga.lib */
AMIGAGUIDEHOST AddAmigaGuideHost (struct Hook * hook, STRPTR name, Tag tag1, ...);
AMIGAGUIDECONTEXT OpenAmigaGuide (struct NewAmigaGuide * nag, Tag tag1, ...);
AMIGAGUIDECONTEXT OpenAmigaGuideAsync (struct NewAmigaGuide * nag, Tag tag1, ...);
LONG RemoveAmigaGuideHost (AMIGAGUIDEHOST key, Tag tag1, ...);
BOOL SendAmigaGuideCmd (AMIGAGUIDECONTEXT handle, STRPTR cmd, Tag tag1, ...);
BOOL SendAmigaGuideContext (AMIGAGUIDECONTEXT handle, Tag tag1, ...);
LONG SetAmigaGuideAttrs (AMIGAGUIDECONTEXT handle, Tag tag1, ...);
BOOL SetAmigaGuideContext (AMIGAGUIDECONTEXT handle, ULONG context, Tag tag1, ...);

AROS_LP1(LONG, LockAmigaGuideBase,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         LIBBASETYPEPTR, AmigaGuideBase, 6, AmigaGuide
);
AROS_LP1(void, UnlockAmigaGuideBase,
         AROS_LPA(LONG, key, D0),
         LIBBASETYPEPTR, AmigaGuideBase, 7, AmigaGuide
);
AROS_LP2(AMIGAGUIDECONTEXT, OpenAmigaGuideA,
         AROS_LPA(struct NewAmigaGuide *, nag, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 9, AmigaGuide
);
AROS_LP2(AMIGAGUIDECONTEXT, OpenAmigaGuideAsyncA,
         AROS_LPA(struct NewAmigaGuide *, nag, A0),
         AROS_LPA(struct TagItem *, attrs, D0),
         LIBBASETYPEPTR, AmigaGuideBase, 10, AmigaGuide
);
AROS_LP1(void, CloseAmigaGuide,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         LIBBASETYPEPTR, AmigaGuideBase, 11, AmigaGuide
);
AROS_LP1(ULONG, AmigaGuideSignal,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         LIBBASETYPEPTR, AmigaGuideBase, 12, AmigaGuide
);
AROS_LP1(struct AmigaGuideMsg *, GetAmigaGuideMsg,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         LIBBASETYPEPTR, AmigaGuideBase, 13, AmigaGuide
);
AROS_LP1(void, ReplyAmigaGuideMsg,
         AROS_LPA(struct AmigaGuideMsg *, msg, A0),
         LIBBASETYPEPTR, AmigaGuideBase, 14, AmigaGuide
);
AROS_LP3(BOOL, SetAmigaGuideContextA,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         AROS_LPA(ULONG, context, D0),
         AROS_LPA(struct TagItem *, attrs, D1),
         LIBBASETYPEPTR, AmigaGuideBase, 15, AmigaGuide
);
AROS_LP2(BOOL, SendAmigaGuideContextA,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         AROS_LPA(struct TagItem *, attrs, D0),
         LIBBASETYPEPTR, AmigaGuideBase, 16, AmigaGuide
);
AROS_LP3(BOOL, SendAmigaGuideCmdA,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         AROS_LPA(STRPTR, cmd, D0),
         AROS_LPA(struct TagItem *, attrs, D1),
         LIBBASETYPEPTR, AmigaGuideBase, 17, AmigaGuide
);
AROS_LP2(LONG, SetAmigaGuideAttrsA,
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 18, AmigaGuide
);
AROS_LP3(LONG, GetAmigaGuideAttr,
         AROS_LPA(Tag, tag, D0),
         AROS_LPA(AMIGAGUIDECONTEXT, handle, A0),
         AROS_LPA(ULONG *, storage, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 19, AmigaGuide
);
AROS_LP2(LONG, LoadXRef,
         AROS_LPA(BPTR, lock, A0),
         AROS_LPA(STRPTR, name, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 21, AmigaGuide
);
AROS_LP0(void, ExpungeXRef,
         LIBBASETYPEPTR, AmigaGuideBase, 22, AmigaGuide
);
AROS_LP3(AMIGAGUIDEHOST, AddAmigaGuideHostA,
         AROS_LPA(struct Hook *, hook, A0),
         AROS_LPA(STRPTR, name, D0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 23, AmigaGuide
);
AROS_LP2(LONG, RemoveAmigaGuideHostA,
         AROS_LPA(AMIGAGUIDEHOST, key, A0),
         AROS_LPA(struct TagItem *, attrs, A1),
         LIBBASETYPEPTR, AmigaGuideBase, 24, AmigaGuide
);
AROS_LP1(STRPTR, GetAmigaGuideString,
         AROS_LPA(ULONG, id, D0),
         LIBBASETYPEPTR, AmigaGuideBase, 35, AmigaGuide
);

#endif /* CLIB_AMIGAGUIDE_PROTOS_H */
