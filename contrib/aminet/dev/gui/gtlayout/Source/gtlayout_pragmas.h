/*
**	$VER: gtlayout_pragmas.h 39.1 (24.11.1997)
**
**	SAS/C format pragma files.
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

/* "gtlayout.library" */
#pragma libcall GTLayoutBase LT_LevelWidth 1e 2DB10A9808
#pragma libcall GTLayoutBase LT_DeleteHandle 24 801
#pragma libcall GTLayoutBase LT_CreateHandle 2a 9802
#pragma libcall GTLayoutBase LT_CreateHandleTagList 30 9802
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_CreateHandleTags 30 9802
#endif
#pragma libcall GTLayoutBase LT_Rebuild 36 10A9805
#pragma libcall GTLayoutBase LT_HandleInput 3c BA90805
#pragma libcall GTLayoutBase LT_BeginRefresh 42 801
#pragma libcall GTLayoutBase LT_EndRefresh 48 0802
#pragma libcall GTLayoutBase LT_GetAttributesA 4e 90803
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_GetAttributes 4e 90803
#endif
#pragma libcall GTLayoutBase LT_SetAttributesA 54 90803
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_SetAttributes 54 90803
#endif
#pragma libcall GTLayoutBase LT_AddA 5a 9210805
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_Add 5a 9210805
#endif
#pragma libcall GTLayoutBase LT_NewA 60 9802
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_New 60 9802
#endif
#pragma libcall GTLayoutBase LT_EndGroup 66 801
#pragma libcall GTLayoutBase LT_LayoutA 6c B3210A9808
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_Layout 6c B3210A9808
#endif
#pragma libcall GTLayoutBase LT_LayoutMenusA 72 A9803
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_LayoutMenus 72 A9803
#endif
/*--- (3 function slots reserved here) ---*/
#pragma libcall GTLayoutBase LT_LabelWidth 8a 9802
#pragma libcall GTLayoutBase LT_LabelChars 90 9802
#pragma libcall GTLayoutBase LT_LockWindow 96 801
#pragma libcall GTLayoutBase LT_UnlockWindow 9c 801
#pragma libcall GTLayoutBase LT_DeleteWindowLock a2 801
#pragma libcall GTLayoutBase LT_ShowWindow a8 9802
#pragma libcall GTLayoutBase LT_Activate ae 0802
#pragma libcall GTLayoutBase LT_PressButton b4 0802
#pragma libcall GTLayoutBase LT_GetCode ba 821004
/*--- Added in v1.78 --------------------------------------------------*/
#pragma libcall GTLayoutBase LT_GetIMsg c0 801
#pragma libcall GTLayoutBase LT_ReplyIMsg c6 801
/*--- Added in v3.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_BuildA cc 9802
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_Build cc 9802
#endif
#pragma libcall GTLayoutBase LT_RebuildTagList d2 90803
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_RebuildTags d2 90803
#endif
/*--- Added in v9.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_UpdateStrings d8 801
/*--- Added in v11.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_DisposeMenu de 801
#pragma libcall GTLayoutBase LT_NewMenuTemplate e4 10BA9806
#pragma libcall GTLayoutBase LT_NewMenuTagList ea 801
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_NewMenuTags ea 801
#endif
#pragma libcall GTLayoutBase LT_MenuControlTagList f0 A9803
#ifdef __SASC_60
#pragma tagcall GTLayoutBase LT_MenuControlTags f0 A9803
#endif
#pragma libcall GTLayoutBase LT_GetMenuItem f6 0802
#pragma libcall GTLayoutBase LT_FindMenuCommand fc 910804
/*--- Added in v14.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_NewLevelWidth 102 23B10A9808
/*--- Added in v31.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_Refresh 108 801
/*--- Added in v34.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_CatchUpRefresh 10e 801
/*--- Added in v39.0 ---------------------------------------------------*/
#pragma libcall GTLayoutBase LT_GetWindowUserData 114 9802
