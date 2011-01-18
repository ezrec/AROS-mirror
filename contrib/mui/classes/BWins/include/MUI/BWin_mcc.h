#ifndef MUI_BWIN_MCC_H
#define MUI_BWIN_MCC_H

/*
**  $VER: BWin_mcc.h 22.0 (20.2.2005)
**  Includes Release 22.0
**
**  BWin.mcc
**  Borderless windows class
**
**  (C) Copyright 2002 Alfonso [alfie] Ranieri <alforan@tin.it>
**      Destributed under the terms of the GNU Public Licence 2
*/

#ifndef LIBRARIES_MUI_H
#include <libraries/mui.h>
#endif

#ifdef __AROS__
#define STACKED
#endif

/***********************************************************************/

#define MUIC_BWin  "BWin.mcc"
#define BWinObject MUI_NewObject(MUIC_BWin

/***********************************************************************/
/*
** Attributes - Scheme is [ISGN]
*/

/* Base */
#define MUIA_BWin_Base        0xFEC9012C

#define MUIA_BWin_NoDragBar   0xFEC90135  /* [I.G.] BOOL  */
#define MUIA_BWin_NoSize      0xFEC90136  /* [I.G.] BOOL  */
#define MUIA_BWin_NoClose     0xFEC90137  /* [I.G.] BOOL  */
#define MUIA_BWin_ShowDragBar 0xFEC90138  /* [ISG.] BOOL  */
#define MUIA_BWin_ShowSize    0xFEC90139  /* [ISG.] BOOL  */
#define MUIA_BWin_ShowClose   0xFEC9013A  /* [ISG.] BOOL  */
#define MUIA_BWin_AutoShow    0xFEC9013B  /* [ISG.] BOOL  */
#define MUIA_BWin_Save        0xFEC9013C  /* [ISG.] ULONG */
#define MUIA_BWin_NoCloseItem 0xFEC9013D  /* [ISG.] BOOL  */
#define MUIA_BWin_NoHideItem  0xFEC9013E  /* [ISG.] BOOL  */
#define MUIA_BWin_DragBarTop  0xFEC9013F  /* [ISG.] BOOL  */
#define MUIA_BWin_Background  0xFEC90140  /* [ISGN] ULONG */
#define MUIA_BWin_Borders     0xFEC90141  /* [I.G.] ULONG */
#define MUIA_BWin_UserItemID  0xFEC90142  /* [..GN] ULONG */
#define MUIA_BWin_InnerLeft   0xFEC90143  /* [ISGN] ULONG */
#define MUIA_BWin_InnerTop    0xFEC90144  /* [ISGN] ULONG */
#define MUIA_BWin_InnerRight  0xFEC90145  /* [ISGN] ULONG */
#define MUIA_BWin_InnerBottom 0xFEC90146  /* [ISGN] ULONG */

/***********************************************************************/
/*
** Special values
*/

/* MUIA_BWin_Save values */
enum
{
    MUIV_BWin_Save_DragBarTop  = 1<<0,
    MUIV_BWin_Save_AutoShow    = 1<<1,
    MUIV_BWin_Save_ShowDragBar = 1<<2,
    MUIV_BWin_Save_ShowSize    = 1<<3,
    MUIV_BWin_Save_ShowClose   = 1<<4,

    MUIV_BWin_Save_All         = MUIV_BWin_Save_DragBarTop|MUIV_BWin_Save_AutoShow|MUIV_BWin_Save_ShowDragBar|MUIV_BWin_Save_ShowSize|MUIV_BWin_Save_ShowClose,
};

/***********************************************************************/
/*
** Methods
*/

#define MUIM_BWin_AddUserItem    0xFEC9012D /* struct MUIP_BWin_AddUserItem    */
#define MUIM_BWin_RemoveUserItem 0xFEC9012E /* struct MUIP_BWin_RemoveUserItem */

/***********************************************************************/
/*
** Methods structures
*/

struct MUIP_BWin_AddUserItem
{
    STACKED ULONG  MethodID;
    STACKED STRPTR title;
};

#define MUIV_BWin_AddUserItem_Bar ((STRPTR)-1)

struct MUIP_BWin_RemoveUserItem
{
    STACKED ULONG MethodID;
    STACKED ULONG id;
};

/***********************************************************************/

#endif /* MUI_BWIN_MCC_H */
