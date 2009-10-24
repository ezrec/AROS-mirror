#ifndef _BROWSERAPP_H
#define _BROWSERAPP_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

#include <WebViewZune.h>

/*** Identifier base ********************************************************/
#define MUIB_BrowserApp                               (MUIB_AROS | 0x00000f00)

/*** Public attributes ******************************************************/
#define MUIA_BrowserApp_Title                  (MUIB_BrowserApp  | 0x00000000)
#define MUIA_BrowserApp_BookmarkManagerOpened  (MUIB_BrowserApp  | 0x00000001)
#define MUIA_BrowserApp_DownloadManagerOpened  (MUIB_BrowserApp  | 0x00000002)
#define MUIA_BrowserApp_PreferencesOpened      (MUIB_BrowserApp  | 0x00000003)

/*** Public methods *********************************************************/
#define MUIM_BrowserApp_OpenNewWindow          (MUIB_BrowserApp  | 0x00000000)
struct  MUIP_BrowserApp_OpenNewWindow {STACKED ULONG MethodID; STACKED struct WindowSpecification *specification;};
#define MUIM_BrowserApp_CloseActiveWindow      (MUIB_BrowserApp  | 0x00000001)
#define MUIM_BrowserApp_CloseWindow            (MUIB_BrowserApp  | 0x00000002)
struct  MUIP_BrowserApp_CloseWindow {STACKED ULONG MethodID; STACKED Object *win;};
#define MUIM_BrowserApp_Bookmark               (MUIB_BrowserApp  | 0x00000003)
struct  MUIP_BrowserApp_Bookmark    {STACKED ULONG MethodID; STRPTR title; STRPTR url;};

/*** Variables **************************************************************/
extern struct MUI_CustomClass *BrowserApp_CLASS;

/*** Macros *****************************************************************/
#define BrowserAppObject BOOPSIOBJMACRO_START(BrowserApp_CLASS->mcc_Class)

#endif /* _BROWSERAPP_H */
