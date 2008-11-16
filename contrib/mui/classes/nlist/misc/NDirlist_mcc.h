/*
  NDirlist.mcc (c) Copyright 1996 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d5100b0 to 0x9d5100cF
  *** use only YOUR OWN Serial Number for your public custom class ***
  NDirlist_mcc.h
*/

#ifndef MUI_NDirlist_MCC_H
#define MUI_NDirlist_MCC_H

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifndef MUI_NListview_MCC_H
#include <MUI/NListview_mcc.h>
#endif

#define MUIC_NDirlist "NDirlist.mcc"
#define NDirlistObject MUI_NewObject(MUIC_NDirlist


/* Attributes */

#define MUIA_NDirlist_AcceptPattern         0x9d5100b0 /* GM  is. STRPTR            */
#define MUIA_NDirlist_Directory             0x9d5100b0 /* GM  isg STRPTR            */
#define MUIA_NDirlist_DrawersOnly           0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_FilesOnly             0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_FilterDrawers         0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_FilterHook            0x9d5100b0 /* GM  is. struct Hook *     */
#define MUIA_NDirlist_MultiSelDirs          0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_NumBytes              0x9d5100b0 /* GM  ..g LONG              */
#define MUIA_NDirlist_NumDrawers            0x9d5100b0 /* GM  ..g LONG              */
#define MUIA_NDirlist_NumFiles              0x9d5100b0 /* GM  ..g LONG              */
#define MUIA_NDirlist_Path                  0x9d5100b0 /* GM  ..g STRPTR            */
#define MUIA_NDirlist_RejectIcons           0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_RejectPattern         0x9d5100b0 /* GM  is. STRPTR            */
#define MUIA_NDirlist_SortDirs              0x9d5100b0 /* GM  is. LONG              */
#define MUIA_NDirlist_SortHighLow           0x9d5100b0 /* GM  is. BOOL              */
#define MUIA_NDirlist_SortType              0x9d5100b0 /* GM  is. LONG              */
#define MUIA_NDirlist_Status                0x9d5100b0 /* GM  ..g LONG              */

#define MUIV_NDirlist_SortDirs_First        0
#define MUIV_NDirlist_SortDirs_Last         1
#define MUIV_NDirlist_SortDirs_Mix          2
#define MUIV_NDirlist_SortType_Name         0
#define MUIV_NDirlist_SortType_Date         1
#define MUIV_NDirlist_SortType_Size         2
#define MUIV_NDirlist_Status_Invalid        0
#define MUIV_NDirlist_Status_Reading        1
#define MUIV_NDirlist_Status_Valid          2


/* Methods */

#define MUIM_NDirlist_ReRead                0x9d5100cf /* GM  */
struct  MUIP_NDirlist_ReRead                { ULONG MethodID; };



#endif /* MUI_NDirlist_MCC_H */
