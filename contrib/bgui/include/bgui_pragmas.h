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
 * Revision 42.0  2000/05/09 22:23:23  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:57  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.2  1999/02/19 05:00:59  mlemos
 * Added support for Storm C.
 *
 * Revision 41.10.2.1  1998/03/01 02:25:30  mlemos
 * Added new memory allocation debugging functions to the library
 *
 * Revision 41.10  1998/02/25 21:13:59  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:16:07  mlemos
 * Ian sources
 *
 *
 */

#ifdef __STORM__
#ifndef _INCLUDE_PRAGMA_BGUI_LIB_H
#include <pragma/bgui_lib.h>
#endif
#else
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
/*pragma libcall BGUIBase BGUI_Private5 ba 0*/
/*pragma libcall BGUIBase BGUI_Private6 c0 0*/

#ifdef __SASC
#pragma tagcall BGUIBase BGUI_NewObject 24 8002
#pragma tagcall BGUIBase BGUI_Request 2a a9803
#pragma tagcall BGUIBase BGUI_DoGadgetMethod 42 ba9804
#pragma tagcall BGUIBase BGUI_MakeClass 90 801
#endif

#endif

#endif
