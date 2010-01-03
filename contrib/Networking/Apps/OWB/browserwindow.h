#ifndef _BROWSERWINDOW_H
#define _BROWSERWINDOW_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_BrowserWindow                 (MUIB_AROS | 0x00000f00)

/*** Public attributes ******************************************************/
#define MUIA_BrowserWindow_WebView         (MUIB_BrowserWindow  | 0x00000000)
#define MUIA_BrowserWindow_LoadingProgress (MUIB_BrowserWindow  | 0x00000001)
#define MUIA_BrowserWindow_Status          (MUIB_BrowserWindow  | 0x00000002)
#define MUIA_BrowserWindow_State           (MUIB_BrowserWindow  | 0x00000003)
#define MUIA_BrowserWindow_ShowStatusBar   (MUIB_BrowserWindow  | 0x00000004)
#define MUIA_BrowserWindow_ShowToolBar     (MUIB_BrowserWindow  | 0x00000005)
#define MUIA_BrowserWindow_ShowLocationBar (MUIB_BrowserWindow  | 0x00000006)
#define MUIA_BrowserWindow_ShowScrollbars  (MUIB_BrowserWindow  | 0x00000007)
#define MUIA_BrowserWindow_WebViewPointer  (MUIB_BrowserWindow  | 0x00000008)

/*** Public methods *********************************************************/
#define MUIM_BrowserWindow_OpenURLInActiveTab (MUIB_BrowserWindow  | 0x00000000)
struct  MUIP_BrowserWindow_OpenURLInActiveTab {STACKED ULONG MethodID; STACKED STRPTR url;};
#define MUIM_BrowserWindow_OpenNewTab          (MUIB_BrowserWindow  | 0x00000001)
struct  MUIP_BrowserWindow_OpenNewTab {STACKED ULONG MethodID; STACKED ULONG mode;};
#define MUIM_BrowserWindow_CloseActiveTab      (MUIB_BrowserWindow  | 0x00000002)
#define MUIM_BrowserWindow_Bookmark            (MUIB_BrowserWindow  | 0x00000003)
#define MUIM_BrowserWindow_Find                (MUIB_BrowserWindow  | 0x00000004)
#define MUIM_BrowserWindow_FindNext            (MUIB_BrowserWindow  | 0x00000005)
#define MUIM_BrowserWindow_Zoom                (MUIB_BrowserWindow  | 0x00000006)
struct  MUIP_BrowserWindow_Zoom    {STACKED ULONG MethodID; STACKED ULONG mode;};
#define MUIM_BrowserWindow_ShowSource          (MUIB_BrowserWindow  | 0x00000007)

/*** Constants **************************************************************/
#define MUIV_BrowserWindow_BackgroundTab       (MUIB_BrowserWindow  | 0x00000000)
#define MUIV_BrowserWindow_ForegroundTab       (MUIB_BrowserWindow  | 0x00000001)
#define MUIV_BrowserWindow_ZoomIn              (MUIB_BrowserWindow  | 0x00000002)
#define MUIV_BrowserWindow_ZoomOut             (MUIB_BrowserWindow  | 0x00000003)
#define MUIV_BrowserWindow_ZoomReset           (MUIB_BrowserWindow  | 0x00000004)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *BrowserWindow_CLASS;

/*** Macros *****************************************************************/
#define BrowserWindowObject BOOPSIOBJMACRO_START(BrowserWindow_CLASS->mcc_Class)

#endif /* _BROWSERWINDOW_H */
