#ifndef _INCLUDE_PRAGMA_SCALOS_LIB_H
#define _INCLUDE_PRAGMA_SCALOS_LIB_H

#ifndef CLIB_SCALOS_PROTOS_H
#include <clib/scalos_protos.h>
#endif

#pragma amicall(ScalosBase,0x01E,SCA_WBStart(a0,a1,d0))
#pragma amicall(ScalosBase,0x024,SCA_SortNodes(a0,a1,d0))
#pragma amicall(ScalosBase,0x02A,SCA_NewAddAppIcon(d0,d1,a0,a1,a2))
#pragma amicall(ScalosBase,0x030,SCA_RemoveAppObject(a0))
#pragma amicall(ScalosBase,0x036,SCA_NewAddAppWindow(d0,d1,a0,a1,a2))
#pragma amicall(ScalosBase,0x03C,SCA_NewAddAppMenuItem(d0,d1,a0,a1,a2))
#pragma amicall(ScalosBase,0x042,SCA_AllocStdNode(a0,d0))
#pragma amicall(ScalosBase,0x048,SCA_AllocNode(a0,d0))
#pragma amicall(ScalosBase,0x04E,SCA_FreeNode(a0,a1))
#pragma amicall(ScalosBase,0x054,SCA_FreeAllNodes(a0))
#pragma amicall(ScalosBase,0x05A,SCA_MoveNode(a0,a1,d0))
#pragma amicall(ScalosBase,0x060,SCA_SwapNodes(a0,a1,a2))
#pragma amicall(ScalosBase,0x066,SCA_OpenIconWindow(a0))
#pragma amicall(ScalosBase,0x06C,SCA_LockWindowList(d0))
#pragma amicall(ScalosBase,0x072,SCA_UnLockWindowList())
#pragma amicall(ScalosBase,0x078,SCA_AllocMessage(d0,d1))
#pragma amicall(ScalosBase,0x07E,SCA_FreeMessage(a1))
#pragma amicall(ScalosBase,0x084,SCA_InitDrag(a0))
#pragma amicall(ScalosBase,0x08A,SCA_EndDrag(a0))
#pragma amicall(ScalosBase,0x090,SCA_AddBob(a0,a1,a2,d0,d1,d2,d3))
#pragma amicall(ScalosBase,0x096,SCA_DrawDrag(a0,d0,d1,d2))
#pragma amicall(ScalosBase,0x09C,SCA_UpdateIcon(d0,a0,d1))
#pragma amicall(ScalosBase,0x0A2,SCA_MakeWBArgs(a0,a1,d0))
#pragma amicall(ScalosBase,0x0A8,SCA_FreeWBArgs(a0,d0,d1))
#pragma amicall(ScalosBase,0x0AE,SCA_RemapBitmap(a0,a1,a2))
#pragma amicall(ScalosBase,0x0B4,SCA_ScreenTitleMsg(a0,a1))
#pragma amicall(ScalosBase,0x0BA,SCA_MakeScalosClass(a0,a1,a2,d0))
#pragma amicall(ScalosBase,0x0C0,SCA_FreeScalosClass(a0))
#pragma amicall(ScalosBase,0x0C6,SCA_NewScalosObject(a0,a1))
//#pragma amicall(ScalosBase,0x0CC,SCA_DisposeObject(a0))
//#pragma tagcall(ScalosBase,0x02A,SCA_NewAddAppIconTags(d0,d1,a0,a1,a2))
//#pragma tagcall(ScalosBase,0x036,SCA_NewAddAppWindowTags(d0,d1,a0,a1,a2))
//#pragma tagcall(ScalosBase,0x03C,SCA_NewAddAppMenuItemTags(d0,d1,a0,a1,a2))
//#pragma tagcall(ScalosBase,0x066,SCA_OpenIconWindowTags(a0))
//#pragma tagcall(ScalosBase,0x0A2,SCA_MakeWB(a0,a1,d0))
//#pragma tagcall(ScalosBase,0x0A8,SCA_FreeWB(a0,d0,d1))
//#pragma tagcall(ScalosBase,0x0B4,SCA_ScreenTitleMsgArgs(a0,a1))
//#pragma tagcall(ScalosBase,0x0C6,SCA_NewScalosObjectTags(a0,a1))

#endif	/*  _INCLUDE_PRAGMA_SCALOS_LIB_H  */
