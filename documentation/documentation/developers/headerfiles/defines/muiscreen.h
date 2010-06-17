#ifndef DEFINES_MUISCREEN_H
#define DEFINES_MUISCREEN_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/muiscreen/muiscreen.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for muiscreen
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __MUIS_AllocPubScreenDesc_WB(__MUIScreenBase, __arg1) \
        AROS_LC1(struct MUI_PubScreenDesc*, MUIS_AllocPubScreenDesc, \
                  AROS_LCA(struct MUI_PubScreenDesc *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 5, MUIScreen)

#define MUIS_AllocPubScreenDesc(arg1) \
    __MUIS_AllocPubScreenDesc_WB(MUIScreenBase, (arg1))

#define __MUIS_FreePubScreenDesc_WB(__MUIScreenBase, __arg1) \
        AROS_LC1NR(VOID, MUIS_FreePubScreenDesc, \
                  AROS_LCA(struct MUI_PubScreenDesc *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 6, MUIScreen)

#define MUIS_FreePubScreenDesc(arg1) \
    __MUIS_FreePubScreenDesc_WB(MUIScreenBase, (arg1))

#define __MUIS_OpenPubScreen_WB(__MUIScreenBase, __arg1) \
        AROS_LC1(char*, MUIS_OpenPubScreen, \
                  AROS_LCA(struct MUI_PubScreenDesc *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 7, MUIScreen)

#define MUIS_OpenPubScreen(arg1) \
    __MUIS_OpenPubScreen_WB(MUIScreenBase, (arg1))

#define __MUIS_ClosePubScreen_WB(__MUIScreenBase, __arg1) \
        AROS_LC1(BOOL, MUIS_ClosePubScreen, \
                  AROS_LCA(char *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 8, MUIScreen)

#define MUIS_ClosePubScreen(arg1) \
    __MUIS_ClosePubScreen_WB(MUIScreenBase, (arg1))

#define __MUIS_OpenPubFile_WB(__MUIScreenBase, __arg1, __arg2) \
        AROS_LC2(APTR, MUIS_OpenPubFile, \
                  AROS_LCA(char *,(__arg1),A0), \
                  AROS_LCA(ULONG,(__arg2),D0), \
        struct Library *, (__MUIScreenBase), 9, MUIScreen)

#define MUIS_OpenPubFile(arg1, arg2) \
    __MUIS_OpenPubFile_WB(MUIScreenBase, (arg1), (arg2))

#define __MUIS_ClosePubFile_WB(__MUIScreenBase, __arg1) \
        AROS_LC1NR(void, MUIS_ClosePubFile, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 10, MUIScreen)

#define MUIS_ClosePubFile(arg1) \
    __MUIS_ClosePubFile_WB(MUIScreenBase, (arg1))

#define __MUIS_ReadPubFile_WB(__MUIScreenBase, __arg1) \
        AROS_LC1(struct MUI_PubScreenDesc*, MUIS_ReadPubFile, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 11, MUIScreen)

#define MUIS_ReadPubFile(arg1) \
    __MUIS_ReadPubFile_WB(MUIScreenBase, (arg1))

#define __MUIS_WritePubFile_WB(__MUIScreenBase, __arg1, __arg2) \
        AROS_LC2(BOOL, MUIS_WritePubFile, \
                  AROS_LCA(APTR,(__arg1),A0), \
                  AROS_LCA(struct MUI_PubScreenDesc *,(__arg2),A1), \
        struct Library *, (__MUIScreenBase), 12, MUIScreen)

#define MUIS_WritePubFile(arg1, arg2) \
    __MUIS_WritePubFile_WB(MUIScreenBase, (arg1), (arg2))

#define __MUIS_AddInfoClient_WB(__MUIScreenBase, __arg1) \
        AROS_LC1NR(void, MUIS_AddInfoClient, \
                  AROS_LCA(struct MUIS_InfoClient *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 13, MUIScreen)

#define MUIS_AddInfoClient(arg1) \
    __MUIS_AddInfoClient_WB(MUIScreenBase, (arg1))

#define __MUIS_RemInfoClient_WB(__MUIScreenBase, __arg1) \
        AROS_LC1NR(void, MUIS_RemInfoClient, \
                  AROS_LCA(struct MUIS_InfoClient *,(__arg1),A0), \
        struct Library *, (__MUIScreenBase), 14, MUIScreen)

#define MUIS_RemInfoClient(arg1) \
    __MUIS_RemInfoClient_WB(MUIScreenBase, (arg1))

__END_DECLS

#endif /* DEFINES_MUISCREEN_H*/
