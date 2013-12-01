#ifndef PRAGMAS_SCALOS_PRAGMAS_H
#define PRAGMAS_SCALOS_PRAGMAS_H

#ifndef CLIB_SCALOS_PROTOS_H
#include <clib/scalos_protos.h>
#endif

#pragma  libcall ScalosBase SCA_WBStart            01e 09803
#pragma  libcall ScalosBase SCA_SortNodes          024 09803
#pragma  libcall ScalosBase SCA_NewAddAppIcon      02a a981005
#pragma  libcall ScalosBase SCA_RemoveAppObject    030 801
#pragma  libcall ScalosBase SCA_NewAddAppWindow    036 a981005
#pragma  libcall ScalosBase SCA_NewAddAppMenuItem  03c a981005
#pragma  libcall ScalosBase SCA_AllocStdNode       042 0802
#pragma  libcall ScalosBase SCA_AllocNode          048 0802
#pragma  libcall ScalosBase SCA_FreeNode           04e 9802
#pragma  libcall ScalosBase SCA_FreeAllNodes       054 801
#pragma  libcall ScalosBase SCA_MoveNode           05a 09803
#pragma  libcall ScalosBase SCA_SwapNodes          060 a9803
#pragma  libcall ScalosBase SCA_OpenIconWindow     066 801
#pragma  libcall ScalosBase SCA_LockWindowList     06c 001
#pragma  libcall ScalosBase SCA_UnLockWindowList   072 00
#pragma  libcall ScalosBase SCA_AllocMessage       078 1002
#pragma  libcall ScalosBase SCA_FreeMessage        07e 901
#pragma  libcall ScalosBase SCA_InitDrag           084 801
#pragma  libcall ScalosBase SCA_EndDrag            08a 801
#pragma  libcall ScalosBase SCA_AddBob             090 3210a9807
#pragma  libcall ScalosBase SCA_DrawDrag           096 210804
#pragma  libcall ScalosBase SCA_UpdateIcon         09c 18003
#pragma  libcall ScalosBase SCA_MakeWBArgs         0a2 09803
#pragma  libcall ScalosBase SCA_FreeWBArgs         0a8 10803
#pragma  libcall ScalosBase SCA_ScreenTitleMsg     0b4 9802
#pragma  libcall ScalosBase SCA_MakeScalosClass    0ba a09804
#pragma  libcall ScalosBase SCA_FreeScalosClass    0c0 801
#pragma  libcall ScalosBase SCA_NewScalosObject    0c6 9802
#pragma  libcall ScalosBase SCA_DisposeScalosObject 0cc 801
#pragma  libcall ScalosBase SCA_ScalosControlA     0d2 9802
#pragma  libcall ScalosBase SCA_GetDefIconObject   0d8 9802
#pragma  libcall ScalosBase SCA_OpenDrawerByName   0de 9802
#pragma  libcall ScalosBase SCA_CountWBArgs        0e4 801
#pragma  libcall ScalosBase SCA_GetDefIconObjectA  0ea a9803
#pragma  libcall ScalosBase SCA_LockDrag           0f0 801
#pragma  libcall ScalosBase SCA_UnlockDrag         0f6 801
#ifdef __SASC_60
#pragma  tagcall ScalosBase SCA_WBStartTags        01e 90803
#pragma  tagcall ScalosBase SCA_NewAddAppIconTags  02a a981005
#pragma  tagcall ScalosBase SCA_NewAddAppWindowTags 036 a981005
#pragma  tagcall ScalosBase SCA_NewAddAppMenuItemTags 03c a981005
#pragma  tagcall ScalosBase SCA_OpenIconWindowTags 066 801
#pragma  tagcall ScalosBase SCA_ScreenTitleMsgArgs 0b4 9802
#pragma  tagcall ScalosBase SCA_NewScalosObjectTags 0c6 9802
#pragma  tagcall ScalosBase SCA_ScalosControl      0d2 9802
#pragma  tagcall ScalosBase SCA_ScalosControlTags  0d2 9802
#pragma  tagcall ScalosBase SCA_OpenDrawerByNameTags 0de 9802
#pragma  tagcall ScalosBase SCA_GetDefIconObjectTags 0ea a9803
#endif

#endif	/*  PRAGMAS_SCALOS_PRAGMA_H  */
