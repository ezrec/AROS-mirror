/*
    Copyright � 1995-2003, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>
#ifndef __AROS__
#include <intuition/pointerclass.h>
#endif /* __AROS__ */

#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>

#include "compilerspecific.h"
#include "debug.h"

#ifdef MYDEBUG
const IPTR KnownMethods[] =
{
 OM_Dummy,
 OM_NEW,
 OM_DISPOSE,
 OM_SET,
 OM_GET,
 OM_ADDTAIL,
 OM_REMOVE,
 OM_NOTIFY,
 OM_UPDATE,
 OM_ADDMEMBER,
 OM_REMMEMBER,
 GM_HITTEST,
 GM_RENDER,
 GM_GOACTIVE,
 GM_HANDLEINPUT,
 GM_GOINACTIVE,
 GM_HELPTEST,
 GM_LAYOUT,
 DTM_Dummy,
 DTM_FRAMEBOX,
 DTM_PROCLAYOUT,
 DTM_ASYNCLAYOUT,
 DTM_REMOVEDTOBJECT,
 DTM_SELECT,
 DTM_CLEARSELECTED,
 DTM_COPY,
 DTM_PRINT,
 DTM_ABORTPRINT,
 DTM_NEWMEMBER,
 DTM_DISPOSEMEMBER,
 DTM_GOTO,
 DTM_TRIGGER,
 DTM_OBTAINDRAWINFO,
 DTM_DRAW,
 DTM_RELEASEDRAWINFO,
 DTM_WRITE,
 ICM_Dummy,
 ICM_SETLOOP,
 ICM_CLEARLOOP,
 ICM_CHECKLOOP,
 IM_DRAW,
 IM_HITTEST,
 IM_ERASE,
 IM_MOVE,
 IM_DRAWFRAME,
 IM_FRAMEBOX,
 IM_HITFRAME,
 IM_ERASEFRAME
};

const char *MethodNames[] =
{
 "OM_Dummy",
 "OM_NEW",
 "OM_DISPOSE",
 "OM_SET",
 "OM_GET",
 "OM_ADDTAIL",
 "OM_REMOVE",
 "OM_NOTIFY",
 "OM_UPDATE",
 "OM_ADDMEMBER",
 "OM_REMMEMBER",
 "GM_HITTEST",
 "GM_RENDER",
 "GM_GOACTIVE",
 "GM_HANDLEINPUT",
 "GM_GOINACTIVE",
 "GM_HELPTEST",
 "GM_LAYOUT",
 "DTM_Dummy",
 "DTM_FRAMEBOX",
 "DTM_PROCLAYOUT",
 "DTM_ASYNCLAYOUT",
 "DTM_REMOVEDTOBJECT",
 "DTM_SELECT",
 "DTM_CLEARSELECTED",
 "DTM_COPY",
 "DTM_PRINT",
 "DTM_ABORTPRINT",
 "DTM_NEWMEMBER",
 "DTM_DISPOSEMEMBER",
 "DTM_GOTO",
 "DTM_TRIGGER",
 "DTM_OBTAINDRAWINFO",
 "DTM_DRAW",
 "DTM_RELEASEDRAWINFO",
 "DTM_WRITE",
 "ICM_Dummy",
 "ICM_SETLOOP",
 "ICM_CLEARLOOP",
 "ICM_CHECKLOOP",
 "IM_DRAW",
 "IM_HITTEST",
 "IM_ERASE",
 "IM_MOVE",
 "IM_DRAWFRAME",
 "IM_FRAMEBOX",
 "IM_HITFRAME",
 "IM_ERASEFRAME"
};

const int NumMethods = 48;

/*---*/

const IPTR KnownAttribs[] =
{
 DTA_Dummy,
 DTA_TextAttr,
 DTA_TopVert,
 DTA_VisibleVert,
 DTA_TotalVert,
 DTA_VertUnit,
 DTA_TopHoriz,
 DTA_VisibleHoriz,
 DTA_TotalHoriz,
 DTA_HorizUnit,
 DTA_NodeName,
 DTA_Title,
 DTA_TriggerMethods,
 DTA_Data,
 DTA_TextFont,
 DTA_Methods,
 DTA_PrinterStatus,
 DTA_PrinterProc,
 DTA_LayoutProc,
 DTA_Busy,
 DTA_Sync,
 DTA_BaseName,
 DTA_GroupID,
 DTA_ErrorLevel,
 DTA_ErrorNumber,
 DTA_ErrorString,
 DTA_Conductor,
 DTA_ControlPanel,
 DTA_Immediate,
 DTA_Repeat,
 DTA_Name,
 DTA_SourceType,
 DTA_Handle,
 DTA_DataType,
 DTA_Domain,
#ifndef __AROS__
 DTA_Left,
 DTA_Top,
 DTA_Width,
 DTA_Height,
#endif /* __AROS__ */
 DTA_ObjName,
 DTA_ObjAuthor,
 DTA_ObjAnnotation,
 DTA_ObjCopyright,
 DTA_ObjVersion,
 DTA_ObjectID,
 DTA_UserData,
 DTA_FrameInfo,
#ifndef __AROS__
 DTA_RelRight,
 DTA_RelBottom,
 DTA_RelWidth,
 DTA_RelHeight,
#endif /* __AROS__ */
 DTA_SelectDomain,
 DTA_TotalPVert,
 DTA_TotalPHoriz,
 DTA_NominalVert,
 DTA_NominalHoriz,
 DTA_DestCols,
 DTA_DestRows,
 DTA_Special,
 DTA_RastPort,
 DTA_ARexxPortName,
 PDTA_ModeID,
 PDTA_BitMapHeader,
 PDTA_BitMap,
 PDTA_ColorRegisters,
 PDTA_CRegs,
 PDTA_GRegs,
 PDTA_ColorTable,
 PDTA_ColorTable2,
 PDTA_Allocated,
 PDTA_NumColors,
 PDTA_NumAlloc,
 PDTA_Remap,
 PDTA_Screen,
 PDTA_FreeSourceBitMap,
 PDTA_Grab,
 PDTA_DestBitMap,
 PDTA_ClassBitMap,
 PDTA_NumSparse,
 PDTA_SparseTable,
 GA_Dummy,
 GA_Left,
 GA_RelRight,
 GA_Top,
 GA_RelBottom,
 GA_Width,
 GA_RelWidth,
 GA_Height,
 GA_RelHeight,
 GA_Text,
 GA_Image,
 GA_Border,
 GA_SelectRender,
 GA_Highlight,
 GA_Disabled,
 GA_GZZGadget,
 GA_ID,
 GA_UserData,
 GA_SpecialInfo,
 GA_Selected,
 GA_EndGadget,
 GA_Immediate,
 GA_RelVerify,
 GA_FollowMouse,
 GA_RightBorder,
 GA_LeftBorder,
 GA_TopBorder,
 GA_BottomBorder,
 GA_ToggleSelect,
 GA_SysGadget,
 GA_SysGType,
 GA_Previous,
 GA_Next,
 GA_DrawInfo,
 GA_IntuiText,
 GA_LabelImage,
 GA_TabCycle,
 GA_GadgetHelp,
 GA_Bounds,
 GA_RelSpecial,
 PGA_Dummy,
 PGA_Freedom,
 PGA_Borderless,
 PGA_HorizPot,
 PGA_HorizBody,
 PGA_VertPot,
 PGA_VertBody,
 PGA_Total,
 PGA_Visible,
 PGA_Top,
 PGA_NewLook,
 STRINGA_Dummy,
 STRINGA_MaxChars,
 STRINGA_Buffer,
 STRINGA_UndoBuffer,
 STRINGA_WorkBuffer,
 STRINGA_BufferPos,
 STRINGA_DispPos,
 STRINGA_AltKeyMap,
 STRINGA_Font,
 STRINGA_Pens,
 STRINGA_ActivePens,
 STRINGA_EditHook,
 STRINGA_EditModes,
 STRINGA_ReplaceMode,
 STRINGA_FixedFieldMode,
 STRINGA_NoFilterMode,
 STRINGA_Justification,
 STRINGA_LongVal,
 STRINGA_TextVal,
 STRINGA_ExitHelp,
 LAYOUTA_Dummy,
 LAYOUTA_LayoutObj,
 LAYOUTA_Spacing,
 LAYOUTA_Orientation,
 ICA_Dummy,
 ICA_TARGET,
 ICA_MAP,
 ICSPECIAL_CODE,
 ICTARGET_IDCMP,
 IA_Dummy,
 IA_Left,
 IA_Top,
 IA_Width,
 IA_Height,
 IA_FGPen,
 IA_BGPen,
 IA_Data,
 IA_LineWidth,
 IA_Pens,
 IA_Resolution,
 IA_APattern,
 IA_APatSize,
 IA_Mode,
 IA_Font,
 IA_Outline,
 IA_Recessed,
 IA_DoubleEmboss,
 IA_EdgesOnly,
 SYSIA_Size,
 SYSIA_Depth,
 SYSIA_Which,
 SYSIA_DrawInfo,
#ifndef __AROS__
 SYSIA_Pens,
 IA_ShadowPen,
 IA_HighlightPen,
#endif /* __AROS__ */
 SYSIA_ReferenceFont,
 IA_SupportsDisable,
 IA_FrameType,
#ifndef __AROS__
 POINTERA_Dummy,
 POINTERA_BitMap,
 POINTERA_XOffset,
 POINTERA_YOffset,
 POINTERA_WordWidth,
 POINTERA_XResolution,
 POINTERA_YResolution
#endif /* __AROS__ */
};

const char *AttribNames[] =
{
 "DTA_Dummy",
 "DTA_TextAttr",
 "DTA_TopVert",
 "DTA_VisibleVert",
 "DTA_TotalVert",
 "DTA_VertUnit",
 "DTA_TopHoriz",
 "DTA_VisibleHoriz",
 "DTA_TotalHoriz",
 "DTA_HorizUnit",
 "DTA_NodeName",
 "DTA_Title",
 "DTA_TriggerMethods",
 "DTA_Data",
 "DTA_TextFont",
 "DTA_Methods",
 "DTA_PrinterStatus",
 "DTA_PrinterProc",
 "DTA_LayoutProc",
 "DTA_Busy",
 "DTA_Sync",
 "DTA_BaseName",
 "DTA_GroupID",
 "DTA_ErrorLevel",
 "DTA_ErrorNumber",
 "DTA_ErrorString",
 "DTA_Conductor",
 "DTA_ControlPanel",
 "DTA_Immediate",
 "DTA_Repeat",
 "DTA_Name",
 "DTA_SourceType",
 "DTA_Handle",
 "DTA_DataType",
 "DTA_Domain",
#ifndef __AROS__
 "DTA_Left",
 "DTA_Top",
 "DTA_Width",
 "DTA_Height",
#endif /* __AROS__ */
 "DTA_ObjName",
 "DTA_ObjAuthor",
 "DTA_ObjAnnotation",
 "DTA_ObjCopyright",
 "DTA_ObjVersion",
 "DTA_ObjectID",
 "DTA_UserData",
 "DTA_FrameInfo",
#ifndef __AROS__
 "DTA_RelRight",
 "DTA_RelBottom",
 "DTA_RelWidth",
 "DTA_RelHeight",
#endif /* __AROS__ */
 "DTA_SelectDomain",
 "DTA_TotalPVert",
 "DTA_TotalPHoriz",
 "DTA_NominalVert",
 "DTA_NominalHoriz",
 "DTA_DestCols",
 "DTA_DestRows",
 "DTA_Special",
 "DTA_RastPort",
 "DTA_ARexxPortName",
 "PDTA_ModeID",
 "PDTA_BitMapHeader",
 "PDTA_BitMap",
 "PDTA_ColorRegisters",
 "PDTA_CRegs",
 "PDTA_GRegs",
 "PDTA_ColorTable",
 "PDTA_ColorTable2",
 "PDTA_Allocated",
 "PDTA_NumColors",
 "PDTA_NumAlloc",
 "PDTA_Remap",
 "PDTA_Screen",
 "PDTA_FreeSourceBitMap",
 "PDTA_Grab",
 "PDTA_DestBitMap",
 "PDTA_ClassBitMap",
 "PDTA_NumSparse",
 "PDTA_SparseTable",
 "GA_Dummy",
 "GA_Left",
 "GA_RelRight",
 "GA_Top",
 "GA_RelBottom",
 "GA_Width",
 "GA_RelWidth",
 "GA_Height",
 "GA_RelHeight",
 "GA_Text",
 "GA_Image",
 "GA_Border",
 "GA_SelectRender",
 "GA_Highlight",
 "GA_Disabled",
 "GA_GZZGadget",
 "GA_ID",
 "GA_UserData",
 "GA_SpecialInfo",
 "GA_Selected",
 "GA_EndGadget",
 "GA_Immediate",
 "GA_RelVerify",
 "GA_FollowMouse",
 "GA_RightBorder",
 "GA_LeftBorder",
 "GA_TopBorder",
 "GA_BottomBorder",
 "GA_ToggleSelect",
 "GA_SysGadget",
 "GA_SysGType",
 "GA_Previous",
 "GA_Next",
 "GA_DrawInfo",
 "GA_IntuiText",
 "GA_LabelImage",
 "GA_TabCycle",
 "GA_GadgetHelp",
 "GA_Bounds",
 "GA_RelSpecial",
 "PGA_Dummy",
 "PGA_Freedom",
 "PGA_Borderless",
 "PGA_HorizPot",
 "PGA_HorizBody",
 "PGA_VertPot",
 "PGA_VertBody",
 "PGA_Total",
 "PGA_Visible",
 "PGA_Top",
 "PGA_NewLook",
 "STRINGA_Dummy",
 "STRINGA_MaxChars",
 "STRINGA_Buffer",
 "STRINGA_UndoBuffer",
 "STRINGA_WorkBuffer",
 "STRINGA_BufferPos",
 "STRINGA_DispPos",
 "STRINGA_AltKeyMap",
 "STRINGA_Font",
 "STRINGA_Pens",
 "STRINGA_ActivePens",
 "STRINGA_EditHook",
 "STRINGA_EditModes",
 "STRINGA_ReplaceMode",
 "STRINGA_FixedFieldMode",
 "STRINGA_NoFilterMode",
 "STRINGA_Justification",
 "STRINGA_LongVal",
 "STRINGA_TextVal",
 "STRINGA_ExitHelp",
 "LAYOUTA_Dummy",
 "LAYOUTA_LayoutObj",
 "LAYOUTA_Spacing",
 "LAYOUTA_Orientation",
 "ICA_Dummy",
 "ICA_TARGET",
 "ICA_MAP",
 "ICSPECIAL_CODE",
 "ICTARGET_IDCMP",
 "IA_Dummy",
 "IA_Left",
 "IA_Top",
 "IA_Width",
 "IA_Height",
 "IA_FGPen",
 "IA_BGPen",
 "IA_Data",
 "IA_LineWidth",
 "IA_Pens",
 "IA_Resolution",
 "IA_APattern",
 "IA_APatSize",
 "IA_Mode",
 "IA_Font",
 "IA_Outline",
 "IA_Recessed",
 "IA_DoubleEmboss",
 "IA_EdgesOnly",
 "SYSIA_Size",
 "SYSIA_Depth",
 "SYSIA_Which",
 "SYSIA_DrawInfo",
#ifndef __AROS__
 "SYSIA_Pens",
 "IA_ShadowPen",
 "IA_HighlightPen",
#endif /* __AROS__ */
 "SYSIA_ReferenceFont",
 "IA_SupportsDisable",
 "IA_FrameType",
#ifndef __AROS__
 "POINTERA_Dummy",
 "POINTERA_BitMap",
 "POINTERA_XOffset",
 "POINTERA_YOffset",
 "POINTERA_WordWidth",
 "POINTERA_XResolution",
 "POINTERA_YResolution"
#endif /* __AROS__ */
};

#ifdef __AROS__
const int NumAttribs = 178;
#else /* __AROS__ */
const int NumAttribs = 196;
#endif /* __AROS__ */

#endif /* MYDEBUG */
