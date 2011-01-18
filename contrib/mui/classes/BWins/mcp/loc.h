#ifndef LOC_H
#define LOC_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MCPMSG_Title_Window 100
#define MCPMSG_WinShinePen 101
#define MCPMSG_WinShinePen_Title 102
#define MCPMSG_WinShinePen_Help 103
#define MCPMSG_WinShadowPen 104
#define MCPMSG_WinShadowPen_Title 105
#define MCPMSG_WinShadowPen_Help 106
#define MCPMSG_WinForegroundPen 107
#define MCPMSG_WinForegroundPen_Title 108
#define MCPMSG_WinForegroundPen_Help 109
#define MCPMSG_Title_DragBar 110
#define MCPMSG_DragBarShinePen 111
#define MCPMSG_DragBarShinePen_Title 112
#define MCPMSG_DragBarShinePen_Help 113
#define MCPMSG_DragBarShadowPen 114
#define MCPMSG_DragBarShadowPen_Title 115
#define MCPMSG_DragBarShadowPen_Help 116
#define MCPMSG_DragBarBack 117
#define MCPMSG_DragBarBack_Title 118
#define MCPMSG_DragBarBack_Help 119
#define MCPMSG_UseDragBarBack 120
#define MCPMSG_UseDragBarBack_Help 121
#define MCPMSG_DragBarType 122
#define MCPMSG_DragBarType_Help 123
#define MCPMSG_Title_Size 124
#define MCPMSG_SizeShinePen 125
#define MCPMSG_SizeShinePen_Title 126
#define MCPMSG_SizeShinePen_Help 127
#define MCPMSG_SizeShadowPen 128
#define MCPMSG_SizeShadowPen_Title 129
#define MCPMSG_SizeShadowPen_Help 130
#define MCPMSG_SizeForegroundPen 131
#define MCPMSG_SizeForegroundPen_Title 132
#define MCPMSG_SizeForegroundPen_Help 133
#define MCPMSG_UseSizeForegroundPen 134
#define MCPMSG_UseSizeForegroundPen_Help 135
#define MCPMSG_SizeType 136
#define MCPMSG_SizeType_Help 137
#define MCPMSG_Title_Close 138
#define MCPMSG_CloseShinePen 139
#define MCPMSG_CloseShinePen_Title 140
#define MCPMSG_CloseShinePen_Help 141
#define MCPMSG_CloseShadowPen 142
#define MCPMSG_CloseShadowPen_Title 143
#define MCPMSG_CloseShadowPen_Help 144
#define MCPMSG_CloseBack 145
#define MCPMSG_CloseBack_Title 146
#define MCPMSG_CloseBack_Help 147
#define MCPMSG_UseCloseBack 148
#define MCPMSG_UseCloseBack_Help 149
#define MCPMSG_CloseType 150
#define MCPMSG_CloseType_Help 151
#define MCPMSG_Title_Spacing 152
#define MCPMSG_LeftSpacing 153
#define MCPMSG_LeftSpacing_Help 154
#define MCPMSG_TopSpacing 155
#define MCPMSG_TopSpacing_Help 156
#define MCPMSG_RightSpacing 157
#define MCPMSG_RightSpacing_Help 158
#define MCPMSG_BottomSpacing 159
#define MCPMSG_BottomSpacing_Help 160
#define MCPMSG_Shape_None 200
#define MCPMSG_Shape_Original 201
#define MCPMSG_Shape_Line 202
#define MCPMSG_Shape_Solid 203
#define MCPMSG_MenuTitle_BWin 250
#define MCPMSG_Item_Defaults 251
#define MCPMSG_Item_Alfie_Defaults 252
#define MCPMSG_Info_First 400
#define MCPMSG_Info_Reserved 401
#define MCPMSG_Info_Rest 402
#define MCPMSG_Info_Translator 403
#define MCPMSG_DragBarActive 404
#define MCPMSG_DragBarNonActive 405
#define MCPMSG_DragBarOver 406
#define MCPMSG_DragBarOver_Help 407

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MCPMSG_Title_Window_STR "Window"
#define MCPMSG_WinShinePen_STR "Shine"
#define MCPMSG_WinShinePen_Title_STR "Shine pen"
#define MCPMSG_WinShinePen_Help_STR "Adjust the window\nframe shine pen."
#define MCPMSG_WinShadowPen_STR "Shadow"
#define MCPMSG_WinShadowPen_Title_STR "Shadow pen"
#define MCPMSG_WinShadowPen_Help_STR "Adjust the window\nframe shadow pen."
#define MCPMSG_WinForegroundPen_STR "Foreground"
#define MCPMSG_WinForegroundPen_Title_STR "Foreground pen"
#define MCPMSG_WinForegroundPen_Help_STR "Adjust the window frame\nforeground pen."
#define MCPMSG_Title_DragBar_STR "DragBar"
#define MCPMSG_DragBarShinePen_STR "Shine"
#define MCPMSG_DragBarShinePen_Title_STR "Shine pen"
#define MCPMSG_DragBarShinePen_Help_STR "Adjust the DragBar\ngadget shine pen."
#define MCPMSG_DragBarShadowPen_STR "Shadow"
#define MCPMSG_DragBarShadowPen_Title_STR "Shadow pen"
#define MCPMSG_DragBarShadowPen_Help_STR "Adjust the DragBar\ngadget shadow pen."
#define MCPMSG_DragBarBack_STR "Background"
#define MCPMSG_DragBarBack_Title_STR "DragBar background"
#define MCPMSG_DragBarBack_Help_STR "Adjust the DragBar\ngadget background."
#define MCPMSG_UseDragBarBack_STR "Use"
#define MCPMSG_UseDragBarBack_Help_STR "Choose if to use\nthe background."
#define MCPMSG_DragBarType_STR "Type"
#define MCPMSG_DragBarType_Help_STR "Select how to render\nthe DragBar gadget."
#define MCPMSG_Title_Size_STR "Size"
#define MCPMSG_SizeShinePen_STR "Shine"
#define MCPMSG_SizeShinePen_Title_STR "Shine pen"
#define MCPMSG_SizeShinePen_Help_STR "Adjust the Size\ngadget shine pen."
#define MCPMSG_SizeShadowPen_STR "Shadow"
#define MCPMSG_SizeShadowPen_Title_STR "Shadow pen"
#define MCPMSG_SizeShadowPen_Help_STR "Adjust the Size\ngadget shadow pen."
#define MCPMSG_SizeForegroundPen_STR "Foreground"
#define MCPMSG_SizeForegroundPen_Title_STR "Foreground pen"
#define MCPMSG_SizeForegroundPen_Help_STR "Adjust the Size gadget\nforeground pen."
#define MCPMSG_UseSizeForegroundPen_STR "Use"
#define MCPMSG_UseSizeForegroundPen_Help_STR "Choose if to use the\nforeground pen."
#define MCPMSG_SizeType_STR "Type"
#define MCPMSG_SizeType_Help_STR "Select how to render\nthe Size gadget."
#define MCPMSG_Title_Close_STR "Close"
#define MCPMSG_CloseShinePen_STR "Shine"
#define MCPMSG_CloseShinePen_Title_STR "Shine pen"
#define MCPMSG_CloseShinePen_Help_STR "Adjust the Close\ngadget shine pen."
#define MCPMSG_CloseShadowPen_STR "Shadow"
#define MCPMSG_CloseShadowPen_Title_STR "Shadow pen"
#define MCPMSG_CloseShadowPen_Help_STR "Adjust the Close\ngadget shadow pen."
#define MCPMSG_CloseBack_STR "Background"
#define MCPMSG_CloseBack_Title_STR "Close background"
#define MCPMSG_CloseBack_Help_STR "Adjust the Close\ngadget background."
#define MCPMSG_UseCloseBack_STR "Use"
#define MCPMSG_UseCloseBack_Help_STR "Choose if to use\nthe background."
#define MCPMSG_CloseType_STR "Type"
#define MCPMSG_CloseType_Help_STR "Select how to render\nthe Close gadget."
#define MCPMSG_Title_Spacing_STR "Spacing"
#define MCPMSG_LeftSpacing_STR "Left"
#define MCPMSG_LeftSpacing_Help_STR "Adjust the window left spacing."
#define MCPMSG_TopSpacing_STR "Top"
#define MCPMSG_TopSpacing_Help_STR "Adjust the window top spacing."
#define MCPMSG_RightSpacing_STR "Right"
#define MCPMSG_RightSpacing_Help_STR "Adjust the window right spacing."
#define MCPMSG_BottomSpacing_STR "Bottom"
#define MCPMSG_BottomSpacing_Help_STR "Adjust the window spacing."
#define MCPMSG_Shape_None_STR "None"
#define MCPMSG_Shape_Original_STR "Original"
#define MCPMSG_Shape_Line_STR "Line"
#define MCPMSG_Shape_Solid_STR "Solid"
#define MCPMSG_MenuTitle_BWin_STR "BWin"
#define MCPMSG_Item_Defaults_STR "Reset to defaults"
#define MCPMSG_Item_Alfie_Defaults_STR "alfie's defaults"
#define MCPMSG_Info_First_STR "%s\nCopyright 2002-2003 by Alfonso Ranieri."
#define MCPMSG_Info_Reserved_STR "All rights reserved."
#define MCPMSG_Info_Rest_STR "Please write to <alforan@tin.it> for\ndevelopers docs and licensing terms."
#define MCPMSG_Info_Translator_STR ""
#define MCPMSG_DragBarActive_STR "Active"
#define MCPMSG_DragBarNonActive_STR "Non active"
#define MCPMSG_DragBarOver_STR "DragBar over"
#define MCPMSG_DragBarOver_Help_STR "Choose if to draw the DragBar\nover the window edges."

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MCPMSG_Title_Window,(STRPTR)MCPMSG_Title_Window_STR},
    {MCPMSG_WinShinePen,(STRPTR)MCPMSG_WinShinePen_STR},
    {MCPMSG_WinShinePen_Title,(STRPTR)MCPMSG_WinShinePen_Title_STR},
    {MCPMSG_WinShinePen_Help,(STRPTR)MCPMSG_WinShinePen_Help_STR},
    {MCPMSG_WinShadowPen,(STRPTR)MCPMSG_WinShadowPen_STR},
    {MCPMSG_WinShadowPen_Title,(STRPTR)MCPMSG_WinShadowPen_Title_STR},
    {MCPMSG_WinShadowPen_Help,(STRPTR)MCPMSG_WinShadowPen_Help_STR},
    {MCPMSG_WinForegroundPen,(STRPTR)MCPMSG_WinForegroundPen_STR},
    {MCPMSG_WinForegroundPen_Title,(STRPTR)MCPMSG_WinForegroundPen_Title_STR},
    {MCPMSG_WinForegroundPen_Help,(STRPTR)MCPMSG_WinForegroundPen_Help_STR},
    {MCPMSG_Title_DragBar,(STRPTR)MCPMSG_Title_DragBar_STR},
    {MCPMSG_DragBarShinePen,(STRPTR)MCPMSG_DragBarShinePen_STR},
    {MCPMSG_DragBarShinePen_Title,(STRPTR)MCPMSG_DragBarShinePen_Title_STR},
    {MCPMSG_DragBarShinePen_Help,(STRPTR)MCPMSG_DragBarShinePen_Help_STR},
    {MCPMSG_DragBarShadowPen,(STRPTR)MCPMSG_DragBarShadowPen_STR},
    {MCPMSG_DragBarShadowPen_Title,(STRPTR)MCPMSG_DragBarShadowPen_Title_STR},
    {MCPMSG_DragBarShadowPen_Help,(STRPTR)MCPMSG_DragBarShadowPen_Help_STR},
    {MCPMSG_DragBarBack,(STRPTR)MCPMSG_DragBarBack_STR},
    {MCPMSG_DragBarBack_Title,(STRPTR)MCPMSG_DragBarBack_Title_STR},
    {MCPMSG_DragBarBack_Help,(STRPTR)MCPMSG_DragBarBack_Help_STR},
    {MCPMSG_UseDragBarBack,(STRPTR)MCPMSG_UseDragBarBack_STR},
    {MCPMSG_UseDragBarBack_Help,(STRPTR)MCPMSG_UseDragBarBack_Help_STR},
    {MCPMSG_DragBarType,(STRPTR)MCPMSG_DragBarType_STR},
    {MCPMSG_DragBarType_Help,(STRPTR)MCPMSG_DragBarType_Help_STR},
    {MCPMSG_Title_Size,(STRPTR)MCPMSG_Title_Size_STR},
    {MCPMSG_SizeShinePen,(STRPTR)MCPMSG_SizeShinePen_STR},
    {MCPMSG_SizeShinePen_Title,(STRPTR)MCPMSG_SizeShinePen_Title_STR},
    {MCPMSG_SizeShinePen_Help,(STRPTR)MCPMSG_SizeShinePen_Help_STR},
    {MCPMSG_SizeShadowPen,(STRPTR)MCPMSG_SizeShadowPen_STR},
    {MCPMSG_SizeShadowPen_Title,(STRPTR)MCPMSG_SizeShadowPen_Title_STR},
    {MCPMSG_SizeShadowPen_Help,(STRPTR)MCPMSG_SizeShadowPen_Help_STR},
    {MCPMSG_SizeForegroundPen,(STRPTR)MCPMSG_SizeForegroundPen_STR},
    {MCPMSG_SizeForegroundPen_Title,(STRPTR)MCPMSG_SizeForegroundPen_Title_STR},
    {MCPMSG_SizeForegroundPen_Help,(STRPTR)MCPMSG_SizeForegroundPen_Help_STR},
    {MCPMSG_UseSizeForegroundPen,(STRPTR)MCPMSG_UseSizeForegroundPen_STR},
    {MCPMSG_UseSizeForegroundPen_Help,(STRPTR)MCPMSG_UseSizeForegroundPen_Help_STR},
    {MCPMSG_SizeType,(STRPTR)MCPMSG_SizeType_STR},
    {MCPMSG_SizeType_Help,(STRPTR)MCPMSG_SizeType_Help_STR},
    {MCPMSG_Title_Close,(STRPTR)MCPMSG_Title_Close_STR},
    {MCPMSG_CloseShinePen,(STRPTR)MCPMSG_CloseShinePen_STR},
    {MCPMSG_CloseShinePen_Title,(STRPTR)MCPMSG_CloseShinePen_Title_STR},
    {MCPMSG_CloseShinePen_Help,(STRPTR)MCPMSG_CloseShinePen_Help_STR},
    {MCPMSG_CloseShadowPen,(STRPTR)MCPMSG_CloseShadowPen_STR},
    {MCPMSG_CloseShadowPen_Title,(STRPTR)MCPMSG_CloseShadowPen_Title_STR},
    {MCPMSG_CloseShadowPen_Help,(STRPTR)MCPMSG_CloseShadowPen_Help_STR},
    {MCPMSG_CloseBack,(STRPTR)MCPMSG_CloseBack_STR},
    {MCPMSG_CloseBack_Title,(STRPTR)MCPMSG_CloseBack_Title_STR},
    {MCPMSG_CloseBack_Help,(STRPTR)MCPMSG_CloseBack_Help_STR},
    {MCPMSG_UseCloseBack,(STRPTR)MCPMSG_UseCloseBack_STR},
    {MCPMSG_UseCloseBack_Help,(STRPTR)MCPMSG_UseCloseBack_Help_STR},
    {MCPMSG_CloseType,(STRPTR)MCPMSG_CloseType_STR},
    {MCPMSG_CloseType_Help,(STRPTR)MCPMSG_CloseType_Help_STR},
    {MCPMSG_Title_Spacing,(STRPTR)MCPMSG_Title_Spacing_STR},
    {MCPMSG_LeftSpacing,(STRPTR)MCPMSG_LeftSpacing_STR},
    {MCPMSG_LeftSpacing_Help,(STRPTR)MCPMSG_LeftSpacing_Help_STR},
    {MCPMSG_TopSpacing,(STRPTR)MCPMSG_TopSpacing_STR},
    {MCPMSG_TopSpacing_Help,(STRPTR)MCPMSG_TopSpacing_Help_STR},
    {MCPMSG_RightSpacing,(STRPTR)MCPMSG_RightSpacing_STR},
    {MCPMSG_RightSpacing_Help,(STRPTR)MCPMSG_RightSpacing_Help_STR},
    {MCPMSG_BottomSpacing,(STRPTR)MCPMSG_BottomSpacing_STR},
    {MCPMSG_BottomSpacing_Help,(STRPTR)MCPMSG_BottomSpacing_Help_STR},
    {MCPMSG_Shape_None,(STRPTR)MCPMSG_Shape_None_STR},
    {MCPMSG_Shape_Original,(STRPTR)MCPMSG_Shape_Original_STR},
    {MCPMSG_Shape_Line,(STRPTR)MCPMSG_Shape_Line_STR},
    {MCPMSG_Shape_Solid,(STRPTR)MCPMSG_Shape_Solid_STR},
    {MCPMSG_MenuTitle_BWin,(STRPTR)MCPMSG_MenuTitle_BWin_STR},
    {MCPMSG_Item_Defaults,(STRPTR)MCPMSG_Item_Defaults_STR},
    {MCPMSG_Item_Alfie_Defaults,(STRPTR)MCPMSG_Item_Alfie_Defaults_STR},
    {MCPMSG_Info_First,(STRPTR)MCPMSG_Info_First_STR},
    {MCPMSG_Info_Reserved,(STRPTR)MCPMSG_Info_Reserved_STR},
    {MCPMSG_Info_Rest,(STRPTR)MCPMSG_Info_Rest_STR},
    {MCPMSG_Info_Translator,(STRPTR)MCPMSG_Info_Translator_STR},
    {MCPMSG_DragBarActive,(STRPTR)MCPMSG_DragBarActive_STR},
    {MCPMSG_DragBarNonActive,(STRPTR)MCPMSG_DragBarNonActive_STR},
    {MCPMSG_DragBarOver,(STRPTR)MCPMSG_DragBarOver_STR},
    {MCPMSG_DragBarOver_Help,(STRPTR)MCPMSG_DragBarOver_Help_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x64\x00\x08"
    MCPMSG_Title_Window_STR "\x00\x00"
    "\x00\x00\x00\x65\x00\x06"
    MCPMSG_WinShinePen_STR "\x00"
    "\x00\x00\x00\x66\x00\x0A"
    MCPMSG_WinShinePen_Title_STR "\x00"
    "\x00\x00\x00\x67\x00\x24"
    MCPMSG_WinShinePen_Help_STR "\x00\x00"
    "\x00\x00\x00\x68\x00\x08"
    MCPMSG_WinShadowPen_STR "\x00\x00"
    "\x00\x00\x00\x69\x00\x0C"
    MCPMSG_WinShadowPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x6A\x00\x24"
    MCPMSG_WinShadowPen_Help_STR "\x00"
    "\x00\x00\x00\x6B\x00\x0C"
    MCPMSG_WinForegroundPen_STR "\x00\x00"
    "\x00\x00\x00\x6C\x00\x10"
    MCPMSG_WinForegroundPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x6D\x00\x28"
    MCPMSG_WinForegroundPen_Help_STR "\x00"
    "\x00\x00\x00\x6E\x00\x08"
    MCPMSG_Title_DragBar_STR "\x00"
    "\x00\x00\x00\x6F\x00\x06"
    MCPMSG_DragBarShinePen_STR "\x00"
    "\x00\x00\x00\x70\x00\x0A"
    MCPMSG_DragBarShinePen_Title_STR "\x00"
    "\x00\x00\x00\x71\x00\x26"
    MCPMSG_DragBarShinePen_Help_STR "\x00\x00"
    "\x00\x00\x00\x72\x00\x08"
    MCPMSG_DragBarShadowPen_STR "\x00\x00"
    "\x00\x00\x00\x73\x00\x0C"
    MCPMSG_DragBarShadowPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x74\x00\x26"
    MCPMSG_DragBarShadowPen_Help_STR "\x00"
    "\x00\x00\x00\x75\x00\x0C"
    MCPMSG_DragBarBack_STR "\x00\x00"
    "\x00\x00\x00\x76\x00\x14"
    MCPMSG_DragBarBack_Title_STR "\x00\x00"
    "\x00\x00\x00\x77\x00\x26"
    MCPMSG_DragBarBack_Help_STR "\x00"
    "\x00\x00\x00\x78\x00\x04"
    MCPMSG_UseDragBarBack_STR "\x00"
    "\x00\x00\x00\x79\x00\x22"
    MCPMSG_UseDragBarBack_Help_STR "\x00\x00"
    "\x00\x00\x00\x7A\x00\x06"
    MCPMSG_DragBarType_STR "\x00\x00"
    "\x00\x00\x00\x7B\x00\x2A"
    MCPMSG_DragBarType_Help_STR "\x00\x00"
    "\x00\x00\x00\x7C\x00\x06"
    MCPMSG_Title_Size_STR "\x00\x00"
    "\x00\x00\x00\x7D\x00\x06"
    MCPMSG_SizeShinePen_STR "\x00"
    "\x00\x00\x00\x7E\x00\x0A"
    MCPMSG_SizeShinePen_Title_STR "\x00"
    "\x00\x00\x00\x7F\x00\x22"
    MCPMSG_SizeShinePen_Help_STR "\x00"
    "\x00\x00\x00\x80\x00\x08"
    MCPMSG_SizeShadowPen_STR "\x00\x00"
    "\x00\x00\x00\x81\x00\x0C"
    MCPMSG_SizeShadowPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x82\x00\x24"
    MCPMSG_SizeShadowPen_Help_STR "\x00\x00"
    "\x00\x00\x00\x83\x00\x0C"
    MCPMSG_SizeForegroundPen_STR "\x00\x00"
    "\x00\x00\x00\x84\x00\x10"
    MCPMSG_SizeForegroundPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x85\x00\x28"
    MCPMSG_SizeForegroundPen_Help_STR "\x00\x00"
    "\x00\x00\x00\x86\x00\x04"
    MCPMSG_UseSizeForegroundPen_STR "\x00"
    "\x00\x00\x00\x87\x00\x26"
    MCPMSG_UseSizeForegroundPen_Help_STR "\x00\x00"
    "\x00\x00\x00\x88\x00\x06"
    MCPMSG_SizeType_STR "\x00\x00"
    "\x00\x00\x00\x89\x00\x26"
    MCPMSG_SizeType_Help_STR "\x00"
    "\x00\x00\x00\x8A\x00\x06"
    MCPMSG_Title_Close_STR "\x00"
    "\x00\x00\x00\x8B\x00\x06"
    MCPMSG_CloseShinePen_STR "\x00"
    "\x00\x00\x00\x8C\x00\x0A"
    MCPMSG_CloseShinePen_Title_STR "\x00"
    "\x00\x00\x00\x8D\x00\x24"
    MCPMSG_CloseShinePen_Help_STR "\x00\x00"
    "\x00\x00\x00\x8E\x00\x08"
    MCPMSG_CloseShadowPen_STR "\x00\x00"
    "\x00\x00\x00\x8F\x00\x0C"
    MCPMSG_CloseShadowPen_Title_STR "\x00\x00"
    "\x00\x00\x00\x90\x00\x24"
    MCPMSG_CloseShadowPen_Help_STR "\x00"
    "\x00\x00\x00\x91\x00\x0C"
    MCPMSG_CloseBack_STR "\x00\x00"
    "\x00\x00\x00\x92\x00\x12"
    MCPMSG_CloseBack_Title_STR "\x00\x00"
    "\x00\x00\x00\x93\x00\x24"
    MCPMSG_CloseBack_Help_STR "\x00"
    "\x00\x00\x00\x94\x00\x04"
    MCPMSG_UseCloseBack_STR "\x00"
    "\x00\x00\x00\x95\x00\x22"
    MCPMSG_UseCloseBack_Help_STR "\x00\x00"
    "\x00\x00\x00\x96\x00\x06"
    MCPMSG_CloseType_STR "\x00\x00"
    "\x00\x00\x00\x97\x00\x28"
    MCPMSG_CloseType_Help_STR "\x00\x00"
    "\x00\x00\x00\x98\x00\x08"
    MCPMSG_Title_Spacing_STR "\x00"
    "\x00\x00\x00\x99\x00\x06"
    MCPMSG_LeftSpacing_STR "\x00\x00"
    "\x00\x00\x00\x9A\x00\x20"
    MCPMSG_LeftSpacing_Help_STR "\x00"
    "\x00\x00\x00\x9B\x00\x04"
    MCPMSG_TopSpacing_STR "\x00"
    "\x00\x00\x00\x9C\x00\x20"
    MCPMSG_TopSpacing_Help_STR "\x00\x00"
    "\x00\x00\x00\x9D\x00\x06"
    MCPMSG_RightSpacing_STR "\x00"
    "\x00\x00\x00\x9E\x00\x22"
    MCPMSG_RightSpacing_Help_STR "\x00\x00"
    "\x00\x00\x00\x9F\x00\x08"
    MCPMSG_BottomSpacing_STR "\x00\x00"
    "\x00\x00\x00\xA0\x00\x1C"
    MCPMSG_BottomSpacing_Help_STR "\x00\x00"
    "\x00\x00\x00\xC8\x00\x06"
    MCPMSG_Shape_None_STR "\x00\x00"
    "\x00\x00\x00\xC9\x00\x0A"
    MCPMSG_Shape_Original_STR "\x00\x00"
    "\x00\x00\x00\xCA\x00\x06"
    MCPMSG_Shape_Line_STR "\x00\x00"
    "\x00\x00\x00\xCB\x00\x06"
    MCPMSG_Shape_Solid_STR "\x00"
    "\x00\x00\x00\xFA\x00\x06"
    MCPMSG_MenuTitle_BWin_STR "\x00\x00"
    "\x00\x00\x00\xFB\x00\x12"
    MCPMSG_Item_Defaults_STR "\x00"
    "\x00\x00\x00\xFC\x00\x12"
    MCPMSG_Item_Alfie_Defaults_STR "\x00\x00"
    "\x00\x00\x01\x90\x00\x2C"
    MCPMSG_Info_First_STR "\x00\x00"
    "\x00\x00\x01\x91\x00\x16"
    MCPMSG_Info_Reserved_STR "\x00\x00"
    "\x00\x00\x01\x92\x00\x4A"
    MCPMSG_Info_Rest_STR "\x00"
    "\x00\x00\x01\x93\x00\x02"
    MCPMSG_Info_Translator_STR "\x00\x00"
    "\x00\x00\x01\x94\x00\x08"
    MCPMSG_DragBarActive_STR "\x00\x00"
    "\x00\x00\x01\x95\x00\x0C"
    MCPMSG_DragBarNonActive_STR "\x00\x00"
    "\x00\x00\x01\x96\x00\x0E"
    MCPMSG_DragBarOver_STR "\x00\x00"
    "\x00\x00\x01\x97\x00\x36"
    MCPMSG_DragBarOver_Help_STR "\x00\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* LOC_H */
