#ifndef _INCLUDE_PRAGMA_BGUI_LIB_H
#define _INCLUDE_PRAGMA_BGUI_LIB_H
/*
 * @(#) $Header$
 *
 * $VER: bgui_lib.h (07/01/99)
 *
 * Storm-C pragma file
 *
 * (C) Copyright 1999 Giampiero Gabbiani.
 * (C) Copyright 1999 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.11  2000/05/09 20:37:32  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:02:21  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/02/19 04:58:59  mlemos
 * Initial revision.
 *
 *
 *
 */

#ifndef CLIB_BGUI_PROTOS_H
#include <clib/bgui_protos.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma amicall(BGUIBase, 0x1E, BGUI_GetClassPtr(D0))
#pragma amicall(BGUIBase, 0x24, BGUI_NewObjectA(D0,A0))
#pragma tagcall(BGUIBase, 0x24, BGUI_NewObject(D0,A0))
#pragma amicall(BGUIBase, 0x2A, BGUI_RequestA(A0,A1,A2))
#pragma tagcall(BGUIBase, 0x2A, BGUI_Request(A0,A1,A2))
#pragma amicall(BGUIBase, 0x30, BGUI_Help(A0,A1,A2,D0))
#pragma amicall(BGUIBase, 0x36, BGUI_LockWindow(A0))
#pragma amicall(BGUIBase, 0x3C, BGUI_UnlockWindow(A0))
#pragma amicall(BGUIBase, 0x42, BGUI_DoGadgetMethodA(A0,A1,A2,A3))
#pragma tagcall(BGUIBase, 0x42, BGUI_DoGadgetMethod(A0,A1,A2,A3))
/**** private ****
#pragma amicall(BGUIBase, 0x48, BGUI_AllocPoolMem(D0))
#pragma amicall(BGUIBase, 0x4E, BGUI_FreePoolMem(A0))
*/
#pragma amicall(BGUIBase, 0x54, BGUI_AllocBitMap(D0,D1,D2,D3,A0))
#pragma amicall(BGUIBase, 0x5A, BGUI_FreeBitMap(A0))
#pragma amicall(BGUIBase, 0x60, BGUI_CreateRPortBitMap(A0,D0,D1,D2))
#pragma amicall(BGUIBase, 0x66, BGUI_FreeRPortBitMap(A0))
#pragma amicall(BGUIBase, 0x6C, BGUI_InfoTextSize(A0,A1,A2,A3))
#pragma amicall(BGUIBase, 0x72, BGUI_InfoText(A0,A1,A2,A3))
#pragma amicall(BGUIBase, 0x78, BGUI_GetLocaleStr(A0,D0))
#pragma amicall(BGUIBase, 0x7E, BGUI_GetCatalogStr(A0,D0,A1))
#pragma amicall(BGUIBase, 0x84, BGUI_FillRectPattern(A1,A0,D0,D1,D2,D3))
#pragma amicall(BGUIBase, 0x8A, BGUI_PostRender(A0,A2,A1))
#pragma amicall(BGUIBase, 0x90, BGUI_MakeClassA(A0))
#pragma tagcall(BGUIBase, 0x90, BGUI_MakeClass(A0))
#pragma amicall(BGUIBase, 0x96, BGUI_FreeClass(A0))
#pragma amicall(BGUIBase, 0x9C, BGUI_PackStructureTags(A0,A1,A2))
#pragma amicall(BGUIBase, 0xA2, BGUI_UnpackStructureTags(A0,A1,A2))
/**** private ****
#pragma amicall(BGUIBase, 0xA8, BGUI_GetDefaultTags(d0))
#pragma amicall(BGUIBase, 0xAE, BGUI_DefaultPrefs())
#pragma amicall(BGUIBase, 0xB4, BGUI_LoadPrefs(A0))
#pragma amicall(BGUIBase, 0xBA, BGUI_AllocPoolMemDebug(D0,A0,D1))
#pragma amicall(BGUIBase, 0xC0, BGUI_FreePoolMemDebug(A0,A1,D0))
*/
#ifdef __cplusplus
}
#endif

#endif
