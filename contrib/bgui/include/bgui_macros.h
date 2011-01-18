#ifndef BGUI_BGUI_MACROS_H
#define BGUI_BGUI_MACROS_H
/*
 * @(#) $Header$
 *
 * $VER: libraries/bgui_macros.h 41.10 (19.1.97)
 * bgui.library macros.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2003/01/18 19:10:21  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.5  2000/08/08 20:57:28  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.4  2000/08/08 14:03:07  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros (not needed here).
 *
 * Revision 42.3  2000/08/07 21:51:05  stegerg
 * fixed/activated REGFUNC/REGPARAM macros.
 *
 * Revision 42.2  2000/05/29 00:40:25  bergers
 * Update to compile with AROS now. Should also still compile with SASC etc since I only made changes that test the define __AROS__. The compilation is still very noisy but it does the trick for the main directory. Maybe members of the BGUI team should also have a look at the compiler warnings because some could also cause problems on other systems... (Comparison always TRUE due to datatype (or something like that)). And please compile it on an Amiga to see whether it still works... Thanks.
 *
 * Revision 42.1  2000/05/15 19:28:20  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:23:17  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:01:52  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  1999/08/13 04:36:26  mlemos
 * Changed the pre-processor define to denote that this file was included.
 *
 * Revision 41.10.2.2  1999/02/19 05:00:57  mlemos
 * Added support for Storm C.
 *
 * Revision 41.10.2.1  1998/10/12 01:42:27  mlemos
 * Made the definition of compiler specific macros conditionally dependeing on
 * wether the macros are already defined.
 *
 * Revision 41.10  1998/02/25 21:13:54  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:15:59  mlemos
 * Ian sources
 *
 *
 */

#ifndef LIBRARIES_BGUI_H
#include <libraries/bgui.h>
#endif /* LIBRARIES_BGUI_H */

#ifndef CLIB_ALIB_PROTOS_H
#ifdef __AROS__
#include <proto/alib.h>
#else
#include <clib/alib_protos.h>
#endif
#endif /* CLIB_ALIB_PROTOS_H */


/*****************************************************************************
 *
 *      General object creation macros.
 */
#define LabelObject     BGUI_NewObject(BGUI_LABEL_IMAGE
#define FrameObject     BGUI_NewObject(BGUI_FRAME_IMAGE
#define VectorObject    BGUI_NewObject(BGUI_VECTOR_IMAGE

#define HGroupObject    BGUI_NewObject(BGUI_GROUP_GADGET
#define VGroupObject    BGUI_NewObject(BGUI_GROUP_GADGET, GROUP_Style, GRSTYLE_VERTICAL
#define ButtonObject    BGUI_NewObject(BGUI_BUTTON_GADGET
#define ToggleObject    BGUI_NewObject(BGUI_BUTTON_GADGET, GA_ToggleSelect, TRUE
#define CycleObject     BGUI_NewObject(BGUI_CYCLE_GADGET
#define CheckBoxObject  BGUI_NewObject(BGUI_CHECKBOX_GADGET
#define InfoObject      BGUI_NewObject(BGUI_INFO_GADGET
#define StringObject    BGUI_NewObject(BGUI_STRING_GADGET
#define PropObject      BGUI_NewObject(BGUI_PROP_GADGET
#define IndicatorObject BGUI_NewObject(BGUI_INDICATOR_GADGET
#define ProgressObject  BGUI_NewObject(BGUI_PROGRESS_GADGET
#define SliderObject    BGUI_NewObject(BGUI_SLIDER_GADGET
#define PageObject      BGUI_NewObject(BGUI_PAGE_GADGET
#define MxObject        BGUI_NewObject(BGUI_MX_GADGET
#define ListviewObject  BGUI_NewObject(BGUI_LISTVIEW_GADGET
#define ExternalObject  BGUI_NewObject(BGUI_EXTERNAL_GADGET, GA_Left, 0, GA_Top, 0, GA_Width, 0, GA_Height, 0
#define SeparatorObject BGUI_NewObject(BGUI_SEPARATOR_GADGET
#define AreaObject      BGUI_NewObject(BGUI_AREA_GADGET
#define ViewObject      BGUI_NewObject(BGUI_VIEW_GADGET
#define PaletteObject   BGUI_NewObject(BGUI_PALETTE_GADGET
#define PopButtonObject BGUI_NewObject(BGUI_POPBUTTON_GADGET

#define WindowObject    BGUI_NewObject(BGUI_WINDOW_OBJECT
#define FileReqObject   BGUI_NewObject(BGUI_FILEREQ_OBJECT
#define FontReqObject   BGUI_NewObject(BGUI_FONTREQ_OBJECT
#define ScreenReqObject BGUI_NewObject(BGUI_SCREENREQ_OBJECT
#define CommodityObject BGUI_NewObject(BGUI_COMMODITY_OBJECT
#define ARexxObject     BGUI_NewObject(BGUI_AREXX_OBJECT

#define EndObject       TAG_END)

/* Typo */
#define SeperatorObject         SeparatorObject

/*****************************************************************************
 *
 *      Label creation.
 */
#define Label(l)                LAB_Label, l
#define UScoreLabel(l,u)        LAB_Label, l, LAB_Underscore, u
#define Style(s)                LAB_Style, s
#define Place(p)                LAB_Place, p

/*****************************************************************************
 *
 *      Frames.
 */
#define ButtonFrame             FRM_Type, FRTYPE_BUTTON
#define RidgeFrame              FRM_Type, FRTYPE_RIDGE
#define DropBoxFrame            FRM_Type, FRTYPE_DROPBOX
#define NeXTFrame               FRM_Type, FRTYPE_NEXT
#define RadioFrame              FRM_Type, FRTYPE_RADIOBUTTON
#define XenFrame                FRM_Type, FRTYPE_XEN_BUTTON
#define TabAboveFrame           FRM_Type, FRTYPE_TAB_ABOVE
#define TabBelowFrame           FRM_Type, FRTYPE_TAB_BELOW
#define BorderFrame             FRM_Type, FRTYPE_BORDER
#define FuzzButtonFrame         FRM_Type, FRTYPE_FUZZ_BUTTON
#define FuzzRidgeFrame          FRM_Type, FRTYPE_FUZZ_RIDGE
#define NoFrame                 FRM_Type, FRTYPE_NONE
#define DefaultFrame            FRM_Type, FRTYPE_DEFAULT

/* For clarity. */
#define StringFrame             RidgeFrame
#define MxFrame                 RadioFrame

#define FrameTitle(t)           FRM_Title, t

/* Built-in back fills */
#define ShineRaster             FRM_BackFill, SHINE_RASTER
#define ShadowRaster            FRM_BackFill, SHADOW_RASTER
#define ShineShadowRaster       FRM_BackFill, SHINE_SHADOW_RASTER
#define FillRaster              FRM_BackFill, FILL_RASTER
#define ShineFillRaster         FRM_BackFill, SHINE_FILL_RASTER
#define ShadowFillRaster        FRM_BackFill, SHADOW_FILL_RASTER
#define ShineBlock              FRM_BackFill, SHINE_BLOCK
#define ShadowBlock             FRM_BackFill, SHADOW_BLOCK

/*****************************************************************************
 *
 *      Vector images.
 */
#define GetPath                 VIT_BuiltIn, BUILTIN_GETPATH
#define GetFile                 VIT_BuiltIn, BUILTIN_GETFILE
#define CheckMark               VIT_BuiltIn, BUILTIN_CHECKMARK
#define PopUp                   VIT_BuiltIn, BUILTIN_POPUP
#define ArrowUp                 VIT_BuiltIn, BUILTIN_ARROW_UP
#define ArrowDown               VIT_BuiltIn, BUILTIN_ARROW_DOWN
#define ArrowLeft               VIT_BuiltIn, BUILTIN_ARROW_LEFT
#define ArrowRight              VIT_BuiltIn, BUILTIN_ARROW_RIGHT

/*****************************************************************************
 *
 *      Group class macros.
 */
#define StartMember             GROUP_Member
#define EndMember               TAG_END, 0
#define Spacing(p)              GROUP_Spacing, p
#define Offset(p)               GROUP_Offset, p
#define HOffset(p)              GROUP_HorizOffset, p
#define VOffset(p)              GROUP_VertOffset, p
#define LOffset(p)              GROUP_LeftOffset, p
#define ROffset(p)              GROUP_RightOffset, p
#define TOffset(p)              GROUP_TopOffset, p
#define BOffset(p)              GROUP_BottomOffset, p
#define VarSpace(w)             GROUP_SpaceObject, w
#define EqualWidth              GROUP_EqualWidth, TRUE
#define EqualHeight             GROUP_EqualHeight, TRUE
#define Rows(n)                 GROUP_Rows, n
#define Columns(n)              GROUP_Columns, n

#define NormalSpacing           GROUP_Spacing, GRSPACE_NORMAL
#define NormalHOffset           GROUP_HorizOffset, GRSPACE_NORMAL
#define NormalVOffset           GROUP_VertOffset, GRSPACE_NORMAL
#define NarrowSpacing           GROUP_Spacing, GRSPACE_NARROW
#define NarrowHOffset           GROUP_HorizOffset, GRSPACE_NARROW
#define NarrowVOffset           GROUP_VertOffset, GRSPACE_NARROW
#define WideSpacing             GROUP_Spacing, GRSPACE_WIDE
#define WideHOffset             GROUP_HorizOffset, GRSPACE_WIDE
#define WideVOffset             GROUP_VertOffset, GRSPACE_WIDE
#define NormalOffset            NormalHOffset, NormalVOffset

/*****************************************************************************
 *
 *      Layout macros.
 */
#define FixMinWidth             LGO_FixMinWidth, TRUE
#define FixMinHeight            LGO_FixMinHeight, TRUE
#define Weight(w)               LGO_Weight, w
#define FixWidth(w)             LGO_FixWidth, w
#define FixHeight(h)            LGO_FixHeight, h
#define Align                   LGO_Align, TRUE
#define AlignTop                LGO_VAlign, GRALIGN_TOP
#define AlignBottom             LGO_VAlign, GRALIGN_BOTTOM
#define AlignCenterV            LGO_VAlign, GRALIGN_CENTER
#define AlignLeft               LGO_HAlign, GRALIGN_LEFT
#define AlignRight              LGO_HAlign, GRALIGN_RIGHT
#define AlignCenterH            LGO_HAlign, GRALIGN_CENTER
#define FixMinSize              FixMinWidth, FixMinHeight
#define FixSize(w,h)            FixWidth(w), FixHeight(h)
#define NoAlign                 LGO_NoAlign, TRUE
#define FixAspect(x,y)          LGO_FixAspect, ((x) << 16) | (y)

/*****************************************************************************
 *
 *      Page class macros.
 */
#define PageMember              PAGE_Member

/*****************************************************************************
 *
 *      "Quick" button creation macros.
 */
#define PrefButton(label,id)\
        ButtonObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
        EndObject

#define Button(label,id)\
        ButtonObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                ButtonFrame,\
        EndObject


#define PrefToggle(label,state,id)\
        ToggleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                GA_Selected,            state,\
        EndObject

#define Toggle(label,state,id)\
        ToggleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                GA_Selected,            state,\
                ButtonFrame,\
        EndObject

#define XenButton(label,id)\
        ButtonObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                XenFrame,\
        EndObject

#define XenToggle(label,state,id)\
        ToggleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                GA_Selected,            state,\
                XenFrame,\
        EndObject

#define KeyButton      Button
#define KeyToggle      Toggle
#define XenKeyButton   XenButton
#define XenKeyToggle   XenToggle

/*****************************************************************************
 *
 *      "Quick" cycle creation macros.
 */
#define PrefCycle(label,labels,active,id)\
        CycleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                CYC_Labels,             labels,\
                CYC_Active,             active,\
        EndObject

#define Cycle(label,labels,active,id)\
        CycleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                ButtonFrame,\
                CYC_Labels,             labels,\
                CYC_Active,             active,\
                CYC_Popup,              FALSE,\
        EndObject

#define XenCycle(label,labels,active,id)\
        CycleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                XenFrame,\
                CYC_Labels,             labels,\
                CYC_Active,             active,\
                CYC_Popup,              FALSE,\
        EndObject

#define PopCycle(label,labels,active,id)\
        CycleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                ButtonFrame,\
                CYC_Labels,             labels,\
                CYC_Active,             active,\
                CYC_Popup,              TRUE,\
        EndObject

#define XenPopCycle(label,labels,active,id)\
        CycleObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                XenFrame,\
                CYC_Labels,             labels,\
                CYC_Active,             active,\
                CYC_Popup,              TRUE,\
        EndObject

#define KeyCycle       Cycle
#define XenKeyCycle    XenCycle
#define KeyPopCycle    PopCycle
#define XenKeyPopCycle XenPopCycle

/*****************************************************************************
 *
 *      "Quick" checkbox creation macros.
 */
#define PrefCheckBoxNF(label,state,id)\
        CheckBoxObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                GA_Selected,            state,\
        EndObject

#define CheckBoxNF(label,state,id)\
        CheckBoxObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                ButtonFrame,\
                GA_Selected,            state,\
        EndObject

#define XenCheckBoxNF(label,state,id)\
        CheckBoxObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                XenFrame,\
                GA_Selected,            state,\
        EndObject

#define PrefCheckBox(label,state,id) PrefCheckBoxNF(label,state,id), FixMinSize
#define CheckBox(label,state,id)     CheckBoxNF(label,state,id), FixMinSize
#define XenCheckBox(label,state,id)  XenCheckBoxNF(label,state,id), FixMinSize

#define KeyCheckBoxNF    CheckBoxNF
#define XenKeyCheckBoxNF XenCheckBoxNF
#define KeyCheckBox      CheckBox
#define XenKeyCheckBox   XenCheckBox

/*****************************************************************************
 *
 *      "Quick" info object creation macros.
 */
#define PrefInfoFixed(label,text,args,numlines)\
        InfoObject,\
                LAB_Label,              label,\
                DefaultFrame,\
                INFO_TextFormat,        text,\
                INFO_Args,              args,\
                INFO_MinLines,          numlines,\
                INFO_FixTextWidth,      TRUE,\
        EndObject

#define PrefInfo(label,text,args,numlines)\
        InfoObject,\
                LAB_Label,              label,\
                DefaultFrame,\
                INFO_TextFormat,        text,\
                INFO_Args,              args,\
                INFO_MinLines,          numlines,\
        EndObject

#define InfoFixed(label,text,args,numlines)\
        InfoObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                INFO_TextFormat,        text,\
                INFO_Args,              args,\
                INFO_MinLines,          numlines,\
                INFO_FixTextWidth,      TRUE,\
        EndObject

#define InfoObj(label,text,args,numlines)\
        InfoObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                INFO_TextFormat,        text,\
                INFO_Args,              args,\
                INFO_MinLines,          numlines,\
        EndObject

/*****************************************************************************
 *
 *      "Quick" string/integer creation macros.
 */
#define PrefString(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                STRINGA_TextVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
                GA_TabCycle,            TRUE,\
        EndObject

#define String(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                RidgeFrame,\
                STRINGA_TextVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
        EndObject

#define TabString(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                RidgeFrame,\
                STRINGA_TextVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
                GA_TabCycle,            TRUE,\
        EndObject

#define PrefInteger(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                STRINGA_LongVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
                GA_TabCycle,            TRUE,\
        EndObject

#define Integer(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                RidgeFrame,\
                STRINGA_LongVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
        EndObject

#define TabInteger(label,contents,maxchars,id)\
        StringObject,\
                LAB_Label,              label,\
                RidgeFrame,\
                STRINGA_LongVal,        contents,\
                STRINGA_MaxChars,       maxchars,\
                GA_ID,                  id,\
                GA_TabCycle,            TRUE,\
        EndObject

#define KeyString     String
#define TabKeyString  TabString
#define KeyInteger    Integer
#define TabKeyInteger TabInteger

/*
 *      STRINGA_Pens & STRINGA_ActivePens pen-pack macro.
 */
#define PACKPENS(a,b) (((b<<8)&0xFF00)|((a)&0x00FF))

/*****************************************************************************
 *
 *      "Quick" scroller creation macros.
 */
#define HorizScroller(label,top,total,visible,id)\
        PropObject,\
                LAB_Label,              label,\
                PGA_Top,                top,\
                PGA_Total,              total,\
                PGA_Visible,            visible,\
                PGA_Freedom,            FREEHORIZ,\
                GA_ID,                  id,\
                PGA_Arrows,             TRUE,\
        EndObject

#define VertScroller(label,top,total,visible,id)\
        PropObject,\
                LAB_Label,              label,\
                PGA_Top,                top,\
                PGA_Total,              total,\
                PGA_Visible,            visible,\
                GA_ID,                  id,\
                PGA_Arrows,             TRUE,\
        EndObject

#define KeyHorizScroller HorizScroller
#define KeyVertScroller  VertScroller

/*****************************************************************************
 *
 *      "Quick" indicator creation macros.
 */
#define Indicator(min,max,level,just)\
        IndicatorObject,\
                INDIC_Min,              min,\
                INDIC_Max,              max,\
                INDIC_Level,            level,\
                INDIC_Justification,    just,\
        EndObject

#define IndicatorFormat(min,max,level,just,format)\
        IndicatorObject,\
                INDIC_Min,              min,\
                INDIC_Max,              max,\
                INDIC_Level,            level,\
                INDIC_Justification,    just,\
                INDIC_FormatString,     format,\
        EndObject

/*****************************************************************************
 *
 *      "Quick" progress creation macros.
 */
#define HorizProgress(label,min,max,done)\
        ProgressObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                PROGRESS_Min,           min,\
                PROGRESS_Max,           max,\
                PROGRESS_Done,          done,\
        EndObject

#define VertProgress(label,min,max,done)\
        ProgressObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                PROGRESS_Min,           min,\
                PROGRESS_Max,           max,\
                PROGRESS_Done,          done,\
                PROGRESS_Vertical,      TRUE,\
        EndObject

#define HorizProgressFS(label,min,max,done,fstr)\
        ProgressObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                PROGRESS_Min,           min,\
                PROGRESS_Max,           max,\
                PROGRESS_Done,          done,\
                PROGRESS_FormatString,  fstr,\
        EndObject

#define VertProgressFS(label,min,max,done,fstr)\
        ProgressObject,\
                LAB_Label,              label,\
                ButtonFrame,\
                FRM_Flags,              FRF_RECESSED,\
                PROGRESS_Min,           min,\
                PROGRESS_Max,           max,\
                PROGRESS_Done,          done,\
                PROGRESS_Vertical,      TRUE,\
                PROGRESS_FormatString,  fstr,\
        EndObject

/*****************************************************************************
 *
 *      "Quick" slider creation macros.
 */
#define HorizSlider(label,min,max,level,id)\
        SliderObject,\
                LAB_Label,              label,\
                SLIDER_Min,             min,\
                SLIDER_Max,             max,\
                SLIDER_Level,           level,\
                GA_ID,                  id,\
        EndObject

#define VertSlider(label,min,max,level,id)\
        SliderObject,\
                LAB_Label,              label,\
                SLIDER_Min,             min,\
                SLIDER_Max,             max,\
                SLIDER_Level,           level,\
                PGA_Freedom,            FREEVERT,\
                GA_ID,                  id,\
        EndObject

#define KeyHorizSlider HorizSlider
#define KeyVertSlider  VertSlider

/*****************************************************************************
 *
 *      "Quick" mx creation macros.
 */
#define PrefMx(label,labels,active,id)\
        MxObject,\
                GROUP_Style,            GRSTYLE_VERTICAL,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                GA_ID,                  id,\
        EndObject, FixMinSize

#define RightMx(label,labels,active,id)\
        MxObject,\
                GROUP_Style,            GRSTYLE_VERTICAL,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                MX_LabelPlace,          PLACE_RIGHT,\
                GA_ID,                  id,\
        EndObject, FixMinSize

#define LeftMx(label,labels,active,id)\
        MxObject,\
                GROUP_Style,            GRSTYLE_VERTICAL,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                MX_LabelPlace,          PLACE_LEFT,\
                GA_ID,                  id,\
        EndObject, FixMinSize

#define Tabs(label,labels,active,id)\
        MxObject,\
                MX_TabsObject,          TRUE,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                GA_ID,                  id,\
        EndObject, FixMinHeight


#define TabsEqual(label,labels,active,id)\
        MxObject,\
                GROUP_EqualWidth,       TRUE,\
                MX_TabsObject,          TRUE,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                GA_ID,                  id,\
        EndObject, FixMinHeight

#define USDTabs(label,labels,active,id)\
        MxObject,\
                MX_TabsObject,          TRUE,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                MX_TabsUpsideDown,      TRUE,\
                GA_ID,                  id,\
        EndObject, FixMinHeight

#define USDTabsEqual(label,labels,active,id)\
        MxObject,\
                GROUP_EqualWidth,       TRUE,\
                MX_TabsObject,          TRUE,\
                LAB_Label,              label,\
                MX_Labels,              labels,\
                MX_Active,              active,\
                MX_TabsUpsideDown,      TRUE,\
                GA_ID,                  id,\
        EndObject, FixMinHeight

#define RightMxKey      RightMx
#define LeftMxKey       LeftMx
#define TabsKey         Tabs
#define TabsEqualKey    TabsEqual
#define USDTabsKey      USDTabs
#define USDTabsEqualKey USDTabsEqual

/*****************************************************************************
 *
 *      "Quick" listview creation macros.
 */
#define StrListview(label,strings,id)\
        ListviewObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                LISTV_EntryArray,       strings,\
        EndObject

#define StrListviewSorted(label,strings,id)\
        ListviewObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                LISTV_EntryArray,       strings,\
                LISTV_SortEntryArray,   TRUE,\
        EndObject

#define ReadStrListview(label,strings)\
        ListviewObject,\
                LAB_Label,              label,\
                LISTV_EntryArray,       strings,\
                LISTV_ReadOnly,         TRUE,\
        EndObject

#define ReadStrListviewSorted(label,strings)\
        ListviewObject,\
                LAB_Label,              label,\
                LISTV_EntryArray,       strings,\
                LISTV_SortEntryArray,   TRUE,\
                LISTV_ReadOnly,         TRUE,\
        EndObject

#define MultiStrListview(label,strings,id)\
        ListviewObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                LISTV_EntryArray,       strings,\
                LISTV_MultiSelect,      TRUE,\
        EndObject

#define MultiStrListviewSorted(label,strings,id)\
        ListviewObject,\
                LAB_Label,              label,\
                GA_ID,                  id,\
                LISTV_EntryArray,       strings,\
                LISTV_SortEntryArray,   TRUE,\
                LISTV_MultiSelect,      TRUE,\
        EndObject

/*****************************************************************************
 *
 *      "Quick" separator bar creation macros.
 */
#define VertSeparator\
        SeperatorObject,\
        EndObject, FixMinWidth

#define VertThinSeparator\
        SeperatorObject,\
                SEP_Thin,               TRUE,\
        EndObject, FixMinWidth

#define HorizSeparator\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
        EndObject, FixMinHeight

#define TitleSeparator(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
        EndObject, FixMinHeight

#define HTitleSeparator(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_Highlight,          TRUE,\
        EndObject, FixMinHeight

#define CTitleSeparator(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_CenterTitle,        TRUE,\
        EndObject, FixMinHeight

#define CHTitleSeparator(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_Highlight,          TRUE,\
                SEP_CenterTitle,        TRUE,\
        EndObject, FixMinHeight

#define TitleSeparatorLeft(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_TitleLeft,          TRUE,\
        EndObject, FixMinHeight

#define HTitleSeparatorLeft(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_Highlight,          TRUE,\
                SEP_TitleLeft,          TRUE,\
        EndObject, FixMinHeight

#define CTitleSeparatorLeft(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_CenterTitle,        TRUE,\
                SEP_TitleLeft,          TRUE,\
        EndObject, FixMinHeight

#define CHTitleSeparatorLeft(t)\
        SeperatorObject,\
                SEP_Horiz,              TRUE,\
                SEP_Title,              t,\
                SEP_Highlight,          TRUE,\
                SEP_CenterTitle,        TRUE,\
                SEP_TitleLeft,          TRUE,\
        EndObject, FixMinHeight

/* Typos */
#define VertSeperator           VertSeparator
#define VertThinSeperator       VertThinSeparator
#define HorizSeperator          HorizSeparator
#define TitleSeperator          TitleSeparator
#define HTitleSeperator         HTitleSeparator
#define CTitleSeperator         CTitleSeparator
#define CHTitleSeperator        CHTitleSeparator

/*****************************************************************************
 *
 *      Some simple menu macros.
 */
#define Title(t)\
        { NM_TITLE, t, NULL, 0, 0, NULL }
#define Item(t,s,i)\
        { NM_ITEM, t, s, 0, 0, (APTR)i }
#define ItemBar\
        { NM_ITEM, NM_BARLABEL, NULL, 0, 0, NULL }
#define SubItem(t,s,i)\
        { NM_SUB, t, s, 0, 0, (APTR)i }
#define SubBar\
        { NM_SUB, NM_BARLABEL, NULL, 0, 0, NULL }
#define End\
        { NM_END, NULL, NULL, 0, 0, NULL }

/*****************************************************************************
 *
 *      Base class method macros.
 */
#define AddMap(object,target,map)\
        DoMethod( object, BASE_ADDMAP, target, map )

#define AddCondit(object,target,ttag,tdat,ftag,fdat,stag,sdat)\
        DoMethod( object, BASE_ADDCONDITIONAL, target,\
                  ttag, tdat,\
                  ftag, fdat,\
                  stag, sdat )

#define AddHook(object,hook)\
        DoMethod( object, BASE_ADDHOOK, hook )

#define RemMap(object,target)\
        DoMethod( object, BASE_REMMAP, target )

#define RemCondit(object,target)\
        DoMethod( object, BASE_REMCONDITIONAL, target )

#define RemHook( object,hook)\
        DoMethod( object, BASE_REMHOOK, hook )

/*****************************************************************************
 *
 *      Listview class method macros.
 */
#define AddEntry(window,object,entry,how)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_ADDSINGLE,\
                             NULL, entry, how, 0L )

#define AddEntryVisible(window,object,entry,how)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_ADDSINGLE,\
                             NULL, entry, how, LVASF_MAKEVISIBLE )

#define AddEntrySelect(window,object,entry,how)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_ADDSINGLE,\
                             NULL, entry, how, LVASF_SELECT )

#define InsertEntry(window,object,entry,where)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_INSERTSINGLE,\
                             NULL, where, entry, 0L )

#define InsertEntryVisible(window,object,entry,where)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_INSERTSINGLE,\
                             NULL, where, entry, LVASF_MAKEVISIBLE )

#define InsertEntrySelect(window,object,entry,where)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_INSERTSINGLE,\
                             NULL, where, entry, LVASF_SELECT )

#define ClearList(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_CLEAR, NULL )

#define FirstEntry(object)\
        DoMethod( object, LVM_FIRSTENTRY, NULL, 0L )

#define FirstSelected(object)\
        DoMethod( object, LVM_FIRSTENTRY, NULL, LVGEF_SELECTED )

#define LastEntry(object)\
        DoMethod( object, LVM_LASTENTRY, NULL, 0L )

#define LastSelected(object)\
        DoMethod( object, LVM_LASTENTRY, NULL, LVGEF_SELECTED )

#define NextEntry(object,last)\
        DoMethod( object, LVM_NEXTENTRY, last, 0L )

#define NextSelected(object,last)\
        DoMethod( object, LVM_NEXTENTRY, last, LVGEF_SELECTED )

#define PrevEntry(object,last)\
        DoMethod( object, LVM_PREVENTRY, last, 0L )

#define PrevSelected(object,last)\
        DoMethod( object, LVM_PREVENTRY, last, LVGEF_SELECTED )

#define RemoveEntry(object,entry)\
        DoMethod( object, LVM_REMENTRY, NULL, entry )

#define RemoveEntryVisible(window,object,entry)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_REMENTRY, NULL, entry )

#define RefreshList(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_REFRESH, NULL )

#define RedrawList(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_REDRAW, NULL )

#define SortList(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_SORT, NULL )

#define LockList(object)\
        DoMethod( object, LVM_LOCKLIST, NULL )

#define UnlockList(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_UNLOCKLIST, NULL )

#define MoveEntry(window,object,entry,dir)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_MOVE, NULL,\
                             entry, dir )

#define MoveSelectedEntry(window,object,dir)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_MOVE, NULL,\
                             NULL, dir )

#define ReplaceEntry(window,object,old,new)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_REPLACE, NULL,\
                             old, new )

#define RemoveSelected(window,object)\
        BGUI_DoGadgetMethod( object, window, NULL, LVM_REMSELECTED, NULL )

/*****************************************************************************
 *
 *      Window class method macros.
 */
#define GadgetKey(wobj,gobj,key)\
        DoMethod( wobj, WM_GADGETKEY, NULL, gobj, key )

#define WindowOpen(wobj)\
        ( struct Window * )DoMethod( wobj, WM_OPEN )

#define WindowClose(wobj)\
        DoMethod( wobj, WM_CLOSE )

#define WindowBusy(wobj)\
        DoMethod( wobj, WM_SLEEP )

#define WindowReady(wobj)\
        DoMethod( wobj, WM_WAKEUP )

#define HandleEvent(wobj)\
        DoMethod( wobj, WM_HANDLEIDCMP )

#define DisableMenu(wobj,id,set)\
        DoMethod( wobj, WM_DISABLEMENU, id, set )

#define CheckItem(wobj,id,set)\
        DoMethod( wobj, WM_CHECKITEM, id, set )

#define MenuDisabled(wobj,id)\
        DoMethod( wobj, WM_MENUDISABLED, id )

#define ItemChecked(wobj,id)\
        DoMethod( wobj, WM_ITEMCHECKED, id )

#define GetAppMsg(wobj)\
        ( struct AppMessage * )DoMethod( wobj, WM_GETAPPMSG )

#define AddUpdate(wobj,id,target,map)\
        DoMethod( wobj, WM_ADDUPDATE, id, target, map )

#define GetSignalWindow(wobj)\
        ( struct Window * )DoMethod( wobj, WM_GET_SIGNAL_WINDOW )

/*****************************************************************************
 *
 *      Commodity class method macros.
 */
#define AddHotkey(broker,desc,id,flags)\
        DoMethod( broker, CM_ADDHOTKEY, desc, id, flags )

#define RemHotkey(broker,id)\
        DoMethod( broker, CM_REMHOTKEY, id )

#define DisableHotkey(broker,id)\
        DoMethod( broker, CM_DISABLEHOTKEY, id )

#define EnableHotKey(broker,id)\
        DoMethod( broker, CM_ENABLEHOTKEY, id )

#define EnableBroker(broker)\
        DoMethod( broker, CM_ENABLEBROKER )

#define DisableBroker(broker)\
        DoMethod( broker, CM_DISABLEBROKER )

#define MsgInfo(broker,type,id,data)\
        DoMethod( broker, CM_MSGINFO,\
                  ( ULONG * )type,\
                  ( ULONG * )id,\
                  ( ULONG * )data )

/*****************************************************************************
 *
 *      AslReq class method macros.
 */
#define DoRequest(object)\
        DoMethod( object, ASLM_DOREQUEST )

#endif /* BGUI_BGUI_MACROS_H */
