#if 0

/*
  NListview.mcc (c) Copyright 1996 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d510020 to 0x9d51002F
  *** use only YOUR OWN Serial Number for your public custom class ***
  NListview_priv_mcc.h
*/

#ifndef MUI_NListview_priv_MCC_H
#define MUI_NListview_priv_MCC_H

#ifndef MUI_MUI_H
#include "mui.h"
#endif

#include <MUI/NListview_mcc.h>

#include <mcc_common.h>
#include <mcc_debug.h>


struct NLVData
{
  Object *LI_NList;
  Object *PR_Vert;
  Object *PR_Horiz;
  Object *Group;
  LONG   Vert_Attached;
  LONG   Horiz_Attached;
  LONG   VertSB;
  LONG   HorizSB;
  LONG   Vert_ScrollBar;
  LONG   Horiz_ScrollBar;
  BOOL   sem;
  BOOL   SETUP;
};

#endif /* MUI_NListview_priv_MCC_H */

#endif
