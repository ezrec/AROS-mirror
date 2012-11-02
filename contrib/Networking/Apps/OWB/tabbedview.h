#ifndef _TABBEDVIEW_H
#define _TABBEDVIEW_H

/*
    Copyright © 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <utility/tagitem.h>
#include <libraries/mui.h>

/*** Identifier base ********************************************************/
#define MUIB_TabbedView                      (TAG_USER | 0x10000100)

/*** Public attributes ******************************************************/
#define MUIA_TabbedView_ActiveObject         (MUIB_TabbedView  | 0x00000000)

/*** Methods ****************************************************************/
#define MUIM_TabbedView_ForwardAttribute     (MUIB_TabbedView | 0x00000000)
struct  MUIP_TabbedView_ForwardAttribute     { STACKED ULONG MethodID; STACKED Object *sender; STACKED IPTR attribute; STACKED IPTR value; };
#define MUIM_TabbedView_TriggerNotifications (MUIB_TabbedView | 0x00000001)

/*** Variables **************************************************************/
extern struct MUI_CustomClass *TabbedView_CLASS;

/*** Macros *****************************************************************/
#define TabbedViewObject BOOPSIOBJMACRO_START(TabbedView_CLASS->mcc_Class)

#endif /* _TABBEDVIEW_H */
