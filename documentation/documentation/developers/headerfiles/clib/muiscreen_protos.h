#ifndef CLIB_MUISCREEN_PROTOS_H
#define CLIB_MUISCREEN_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muiscreen/muiscreen.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>

#include <libraries/muiscreen.h>

__BEGIN_DECLS

AROS_LP1(struct MUI_PubScreenDesc*, MUIS_AllocPubScreenDesc,
         AROS_LPA(struct MUI_PubScreenDesc *, src, A0),
         LIBBASETYPEPTR, MUIScreenBase, 5, MUIScreen
);
AROS_LP1(VOID, MUIS_FreePubScreenDesc,
         AROS_LPA(struct MUI_PubScreenDesc *, psd, A0),
         LIBBASETYPEPTR, MUIScreenBase, 6, MUIScreen
);
AROS_LP1(char*, MUIS_OpenPubScreen,
         AROS_LPA(struct MUI_PubScreenDesc *, desc, A0),
         LIBBASETYPEPTR, MUIScreenBase, 7, MUIScreen
);
AROS_LP1(BOOL, MUIS_ClosePubScreen,
         AROS_LPA(char *, name, A0),
         LIBBASETYPEPTR, MUIScreenBase, 8, MUIScreen
);
AROS_LP2(APTR, MUIS_OpenPubFile,
         AROS_LPA(char *, name, A0),
         AROS_LPA(ULONG, mode, D0),
         LIBBASETYPEPTR, MUIScreenBase, 9, MUIScreen
);
AROS_LP1(void, MUIS_ClosePubFile,
         AROS_LPA(APTR, pf, A0),
         LIBBASETYPEPTR, MUIScreenBase, 10, MUIScreen
);
AROS_LP1(struct MUI_PubScreenDesc*, MUIS_ReadPubFile,
         AROS_LPA(APTR, pf, A0),
         LIBBASETYPEPTR, MUIScreenBase, 11, MUIScreen
);
AROS_LP2(BOOL, MUIS_WritePubFile,
         AROS_LPA(APTR, pf, A0),
         AROS_LPA(struct MUI_PubScreenDesc *, desc, A1),
         LIBBASETYPEPTR, MUIScreenBase, 12, MUIScreen
);
AROS_LP1(void, MUIS_AddInfoClient,
         AROS_LPA(struct MUIS_InfoClient *, sic, A0),
         LIBBASETYPEPTR, MUIScreenBase, 13, MUIScreen
);
AROS_LP1(void, MUIS_RemInfoClient,
         AROS_LPA(struct MUIS_InfoClient *, sic, A0),
         LIBBASETYPEPTR, MUIScreenBase, 14, MUIScreen
);

__END_DECLS

#endif /* CLIB_MUISCREEN_PROTOS_H */
