#ifndef BGUIBase_PRAGMA_H
#define BGUIBase_PRAGMA_H
/*
 * @(#) $Header$
 *
 * $VER: bgui_pragmas.h 41.10 (20.1.97)
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:13:59  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:16:07  mlemos
 * Ian sources
 *
 *
 */

#pragma libcall BGUIBase BGUI_GetClassPtr 1e 001
#pragma libcall BGUIBase BGUI_NewObjectA 24 8002
#pragma libcall BGUIBase BGUI_RequestA 2a a9803
#pragma libcall BGUIBase BGUI_Help 30 0a9804
#pragma libcall BGUIBase BGUI_LockWindow 36 801
#pragma libcall BGUIBase BGUI_UnlockWindow 3c 801
#pragma libcall BGUIBase BGUI_DoGadgetMethodA 42 ba9804
/*pragma libcall BGUIBase BGUI_Private0 48 0*/
/*pragma libcall BGUIBase BGUI_Private1 4e 0*/
#pragma libcall BGUIBase BGUI_AllocBitMap 54 8321005
#pragma libcall BGUIBase BGUI_FreeBitMap 5a 801
#pragma libcall BGUIBase BGUI_CreateRPortBitMap 60 210804
#pragma libcall BGUIBase BGUI_FreeRPortBitMap 66 801
#pragma libcall BGUIBase BGUI_InfoTextSize 6c ba9804
#pragma libcall BGUIBase BGUI_InfoText 72 ba9804
#pragma libcall BGUIBase BGUI_GetLocaleStr 78 0802
#pragma libcall BGUIBase BGUI_GetCatalogStr 7e 90803
#pragma libcall BGUIBase BGUI_FillRectPattern 84 32108906
#pragma libcall BGUIBase BGUI_PostRender 8a 9a803
#pragma libcall BGUIBase BGUI_MakeClassA 90 801
#pragma libcall BGUIBase BGUI_FreeClass 96 801
#pragma libcall BGUIBase BGUI_PackStructureTags 9c a9803
#pragma libcall BGUIBase BGUI_UnpackStructureTags a2 a9803
/*pragma libcall BGUIBase BGUI_Private2 a8 0*/
/*pragma libcall BGUIBase BGUI_Private3 ae 0*/
/*pragma libcall BGUIBase BGUI_Private4 b4 0*/

#ifdef __SASC
#pragma tagcall BGUIBase BGUI_NewObject 24 8002
#pragma tagcall BGUIBase BGUI_Request 2a a9803
#pragma tagcall BGUIBase BGUI_DoGadgetMethod 42 ba9804
#pragma tagcall BGUIBase BGUI_MakeClass 90 801
#endif

#endif
