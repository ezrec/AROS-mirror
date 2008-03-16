/*
** This file was created automatically by flexcat.
** It is written to be compatible with catcomp,
** so you choose, but... Do NOT edit by hand!
**/

#ifndef _LOC_H
#define _LOC_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
    #undef CATCOMP_NUMBERS
    #define CATCOMP_NUMBERS
    #undef CATCOMP_STRINGS
    #define CATCOMP_STRINGS
#endif

/****************************************************************************/

#ifdef CATCOMP_NUMBERS

#define Msg_Reg_Colors 1L
#define Msg_Reg_Appearance 2L
#define Msg_Reg_Spacing 3L
#define Msg_Reg_Options 4L
#define Msg_Title_Backgrounds 100L
#define Msg_GroupBackground 101L
#define Msg_GroupBackground_Help 102L
#define Msg_GroupBackground_Title 103L
#define Msg_UseGroupBackground 104L
#define Msg_UseGroupBackground_Help 105L
#define Msg_ButtonBackground 106L
#define Msg_ButtonBackground_Help 107L
#define Msg_ButtonBackground_Title 108L
#define Msg_UseButtonBackground 109L
#define Msg_UseButtonBackground_Help 110L
#define Msg_Title_Frame 111L
#define Msg_FrameShinePen 112L
#define Msg_FrameShinePen_Help 113L
#define Msg_FrameShinePen_Title 114L
#define Msg_FrameShadowPen 115L
#define Msg_FrameShadowPen_Help 116L
#define Msg_FrameShadowPen_Title 117L
#define Msg_FrameStyle_Recessed 118L
#define Msg_FrameStyle_Normal 119L
#define Msg_FrameStyle 120L
#define Msg_FrameStyle_Help 121L
#define Msg_Title_Disabled 122L
#define Msg_DisabledBody 123L
#define Msg_DisabledBody_Help 124L
#define Msg_DisabledBody_Title 125L
#define Msg_DisabledShadow 126L
#define Msg_DisabledShadow_Help 127L
#define Msg_DisabledShadow_Title 128L
#define Msg_Title_BarSpacer 129L
#define Msg_BarSpacerShinePen 130L
#define Msg_BarSpacerShinePen_Help 131L
#define Msg_BarSpacerShinePen_Title 132L
#define Msg_BarSpacerShadowPen 133L
#define Msg_BarSpacerShadowPen_Help 134L
#define Msg_BarSpacerShadowPen_Title 135L
#define Msg_Title_BarFrame 136L
#define Msg_BarFrameShine 137L
#define Msg_BarFrameShine_Help 138L
#define Msg_BarFrameShine_Title 139L
#define Msg_BarFrameShadow 140L
#define Msg_BarFrameShadow_Help 141L
#define Msg_BarFrameShadow_Title 142L
#define Msg_Title_DragBar 143L
#define Msg_DragBarShine 144L
#define Msg_DragBarShine_Help 145L
#define Msg_DragBarShine_Title 146L
#define Msg_DragBarShadow 147L
#define Msg_DragBarShadow_Help 148L
#define Msg_DragBarShadow_Title 149L
#define Msg_DragBarFill 150L
#define Msg_DragBarFill_Help 151L
#define Msg_DragBarFill_Title 152L
#define Msg_UseDragBarFill 153L
#define Msg_UseDragBarFill_Help 154L
#define Msg_Frame 155L
#define Msg_Frame_Help 156L
#define Msg_Frame_Title 157L
#define Msg_TextFont 200L
#define Msg_TextFont_Help 201L
#define Msg_TextGfxFont 202L
#define Msg_TextGfxFont_Help 203L
#define Msg_ViewMode 204L
#define Msg_ViewMode_Help 205L
#define Msg_Appearance_Text 206L
#define Msg_TextGfx 207L
#define Msg_Gfx 208L
#define Msg_Text 209L
#define Msg_Borderless 210L
#define Msg_Borderless_Help 211L
#define Msg_Sunny 212L
#define Msg_Sunny_Help 213L
#define Msg_Raised 214L
#define Msg_Raised_Help 215L
#define Msg_Scaled 216L
#define Msg_Scaled_Help 217L
#define Msg_BarSpacer 218L
#define Msg_BarSpacer_Help 219L
#define Msg_EnableKeys 220L
#define Msg_EnableKeys_Help 221L
#define Msg_LabelPos 222L
#define Msg_LabelPos_Help 223L
#define Msg_LabelPos_Bottom 224L
#define Msg_LabelPos_Top 225L
#define Msg_LabelPos_Right 226L
#define Msg_LabelPos_Left 227L
#define Msg_Title_GroupSpacing 300L
#define Msg_HorizGroupSpacing 301L
#define Msg_HorizGroupSpacing_Help 302L
#define Msg_VertGroupSpacing 303L
#define Msg_VertGroupSpacing_Help 304L
#define Msg_BarSpacerSpacing 305L
#define Msg_BarSpacerSpacing_Help 306L
#define Msg_Title_InnerSpacing 307L
#define Msg_HorizInnerSpacing 308L
#define Msg_HorizInnerSpacing_Help 309L
#define Msg_TopInnerSpacing 310L
#define Msg_TopInnerSpacing_Help 311L
#define Msg_BottomInnerSpacing 312L
#define Msg_BottomInnerSpacing_Help 313L
#define Msg_Title_BarFrameSpacing 314L
#define Msg_LeftBarFrameSpacing 315L
#define Msg_LeftBarFrameSpacing_Help 316L
#define Msg_RightBarFrameSpacing 317L
#define Msg_RightBarFrameSpacing_Help 318L
#define Msg_TopBarFrameSpacing 319L
#define Msg_TopBarFrameSpacing_Help 320L
#define Msg_BottomBarFrameSpacing 321L
#define Msg_BottomBarFrameSpacing_Help 322L
#define Msg_Title_TextGfxSpacing 323L
#define Msg_HorizTextGfxSpacing 324L
#define Msg_HorizTextGfxSpacing_Help 325L
#define Msg_VertTextGfxSpacing 326L
#define Msg_VertTextGfxSpacing_Help 327L
#define Msg_Precision_Gui 400L
#define Msg_Precision_Icon 401L
#define Msg_Precision_Image 402L
#define Msg_Precision_Exact 403L
#define Msg_Precision 404L
#define Msg_Precision_Help 405L
#define Msg_Event_IntuiTicks 406L
#define Msg_Event_MouseMove 407L
#define Msg_Event 408L
#define Msg_Event_Help 409L
#define Msg_Scale 410L
#define Msg_Scale_Help 411L
#define Msg_SpecialSelect 412L
#define Msg_SpecialSelect_Help 413L
#define Msg_TextOverUseShine 414L
#define Msg_TextOverUseShine_Help 415L
#define Msg_IgnoreSelImages 416L
#define Msg_IgnoreSelImages_Help 417L
#define Msg_IgnoreDisImages 418L
#define Msg_IgnoreDisImages_Help 419L
#define Msg_DisMode_Shape 420L
#define Msg_DisMode_Grid 421L
#define Msg_DisMode_FullGrid 422L
#define Msg_DisMode 423L
#define Msg_DisMode_Help 424L
#define Msg_DontMove 425L
#define Msg_DontMove_Help 426L
#define Msg_DisMode_Sunny 427L
#define Msg_NtRaiseActive 428L
#define Msg_NtRaiseActive_Help 429L
#define Msg_SpacersSize 430L
#define Msg_SpacersSize_Help 431L
#define Msg_SpacersSize_Quarter 432L
#define Msg_SpacersSize_Half 433L
#define Msg_SpacersSize_One 434L
#define Msg_Event_MouseObject 435L
#define Msg_DisMode_Blend 436L
#define Msg_DisMode_BlendGrey 437L
#define Msg_Item_Defaults 500L
#define Msg_NoMUI20Event_Help 502L
#define Msg_SpacersSize_None 503L
#define Msg_SpacersSize_OnePoint 504L
#define Msg_SpacersSize_TwoPoint 505L
#define Msg_Info_First 600L
#define Msg_Info_Reserved 601L
#define Msg_Info_Rest 602L
#define Msg_Info_Translator 603L
#define Msg_Item_Globals 604L
#define Msg_Item_LastSaved 605L
#define Msg_Item_Restore 606L
#define Msg_Pop_OK 700L
#define Msg_Pop_OK_Help 701L
#define Msg_Pop_Cancel 702L
#define Msg_Pop_Cancel_Help 703L
#define Msg_Popback_Pattern 710L
#define Msg_Popback_Color 711L
#define Msg_Popback_Bitmap 712L
#define Msg_Popback_Gradient 713L
#define Msg_Popback_HorizGradient 714L
#define Msg_Popback_HorizGradientHelp 715L
#define Msg_Popback_VertGradient 716L
#define Msg_Popback_VertGradientHelp 717L
#define Msg_Popback_From 718L
#define Msg_Popback_To 719L
#define Msg_Popback_Swap 720L
#define Msg_Popback_SwapHelp 721L
#define Msg_Poppen_MUI 800L
#define Msg_Poppen_Colormap 801L
#define Msg_Poppen_RGB 802L
#define Msg_Poppen_Shine 803L
#define Msg_Poppen_Halfshine 804L
#define Msg_Poppen_Background 805L
#define Msg_Poppen_Halfshadow 806L
#define Msg_Poppen_Shadow 807L
#define Msg_Poppen_Text 808L
#define Msg_Poppen_Fill 809L
#define Msg_Poppen_Mark 810L
#define Msg_Poppen_ColorMapHelp 811L
#define Msg_ColoradjustRed 900L
#define Msg_Coloradjust_RedHelp 901L
#define Msg_ColoradjustGreen 902L
#define Msg_Coloradjust_GreenHelp 903L
#define Msg_ColoradjustBlue 904L
#define Msg_Coloradjust_BlueHelp 905L
#define Msg_Coloradjust_WheelAbbr 906L
#define Msg_Coloradjust_WheelHelp 907L
#define Msg_Coloradjust_GradientHelp 908L
#define Msg_PrefsShortHelp 1000L

#endif /* CATCOMP_NUMBERS */

/****************************************************************************/

#ifdef CATCOMP_STRINGS

const unsigned char Msg_Reg_Colors_STR[] = "Colors";
const unsigned char Msg_Reg_Appearance_STR[] = "Appearance";
const unsigned char Msg_Reg_Spacing_STR[] = "Spacing";
const unsigned char Msg_Reg_Options_STR[] = "Options";
const unsigned char Msg_Title_Backgrounds_STR[] = "Backgrounds";
const unsigned char Msg_GroupBackground_STR[] = "Bar";
const unsigned char Msg_GroupBackground_Help_STR[] = "Bar background.";
const unsigned char Msg_GroupBackground_Title_STR[] = "Bar background";
const unsigned char Msg_UseGroupBackground_STR[] = "Active";
const unsigned char Msg_UseGroupBackground_Help_STR[] = "If selected, the bar gets\n"\
    "your preferred background,\n"\
    "otherwise it gets its\n"\
    "parent background.";
const unsigned char Msg_ButtonBackground_STR[] = "Buttons";
const unsigned char Msg_ButtonBackground_Help_STR[] = "Active buttons background.";
const unsigned char Msg_ButtonBackground_Title_STR[] = "Active buttons background";
const unsigned char Msg_UseButtonBackground_STR[] = "Active";
const unsigned char Msg_UseButtonBackground_Help_STR[] = "If selected, the buttons get\n"\
    "your preferred background,\n"\
    "otherwise they get their\n"\
    "parent background.";
const unsigned char Msg_Title_Frame_STR[] = "Buttons frame";
const unsigned char Msg_FrameShinePen_STR[] = "Shine";
const unsigned char Msg_FrameShinePen_Help_STR[] = "Active buttons frame shine pen.";
const unsigned char Msg_FrameShinePen_Title_STR[] = "Frame shine pen";
const unsigned char Msg_FrameShadowPen_STR[] = "Shadow";
const unsigned char Msg_FrameShadowPen_Help_STR[] = "Active buttons frame shadow pen.";
const unsigned char Msg_FrameShadowPen_Title_STR[] = "Frame shadow pen";
const unsigned char Msg_FrameStyle_Recessed_STR[] = "Recessed";
const unsigned char Msg_FrameStyle_Normal_STR[] = "Normal";
const unsigned char Msg_FrameStyle_STR[] = "Style";
const unsigned char Msg_FrameStyle_Help_STR[] = "Active buttons frame style.";
const unsigned char Msg_Title_Disabled_STR[] = "Disabled buttons";
const unsigned char Msg_DisabledBody_STR[] = "Body";
const unsigned char Msg_DisabledBody_Help_STR[] = "Disabled buttons body pen. It is used as\n"\
    "main pen for disabled buttons in Shape\n"\
    "mode and as grid pen for disabled\n"\
    "buttons in Grid modes.";
const unsigned char Msg_DisabledBody_Title_STR[] = "Body pen";
const unsigned char Msg_DisabledShadow_STR[] = "Shadow";
const unsigned char Msg_DisabledShadow_Help_STR[] = "Disabled buttons shadow pen. It is used as\n"\
    "secondary pen for disabled buttons in Shape\n"\
    "mode and as text pen for disabled buttons\n"\
    "in Grid modes.";
const unsigned char Msg_DisabledShadow_Title_STR[] = "Shadow pen";
const unsigned char Msg_Title_BarSpacer_STR[] = "Bar spacer";
const unsigned char Msg_BarSpacerShinePen_STR[] = "Shine";
const unsigned char Msg_BarSpacerShinePen_Help_STR[] = "Bar spacer shine pen.";
const unsigned char Msg_BarSpacerShinePen_Title_STR[] = "Spacer shine pen";
const unsigned char Msg_BarSpacerShadowPen_STR[] = "Shadow";
const unsigned char Msg_BarSpacerShadowPen_Help_STR[] = "Bar spacer shadow pen.";
const unsigned char Msg_BarSpacerShadowPen_Title_STR[] = "Spacer shadow pen";
const unsigned char Msg_Title_BarFrame_STR[] = "Bar frame";
const unsigned char Msg_BarFrameShine_STR[] = "Shine";
const unsigned char Msg_BarFrameShine_Help_STR[] = "Bar frame shine pen.";
const unsigned char Msg_BarFrameShine_Title_STR[] = "Frame shine pen";
const unsigned char Msg_BarFrameShadow_STR[] = "Shadow";
const unsigned char Msg_BarFrameShadow_Help_STR[] = "Bar frame shadow pen.";
const unsigned char Msg_BarFrameShadow_Title_STR[] = "Frame shadow pen";
const unsigned char Msg_Title_DragBar_STR[] = "DragBar";
const unsigned char Msg_DragBarShine_STR[] = "Shine";
const unsigned char Msg_DragBarShine_Help_STR[] = "DragBar shine pen.";
const unsigned char Msg_DragBarShine_Title_STR[] = "DragBar shine pen";
const unsigned char Msg_DragBarShadow_STR[] = "Shadow";
const unsigned char Msg_DragBarShadow_Help_STR[] = "DragBar shadow pen.";
const unsigned char Msg_DragBarShadow_Title_STR[] = "DragBar shadow pen";
const unsigned char Msg_DragBarFill_STR[] = "Fill";
const unsigned char Msg_DragBarFill_Help_STR[] = "DragBar fill pen.";
const unsigned char Msg_DragBarFill_Title_STR[] = "DragBar fill pen";
const unsigned char Msg_UseDragBarFill_STR[] = "Use";
const unsigned char Msg_UseDragBarFill_Help_STR[] = "If selected, the dragbar fill\n"\
    "pen is used.";
const unsigned char Msg_Frame_STR[] = "Frame";
const unsigned char Msg_Frame_Help_STR[] = "Bar frame";
const unsigned char Msg_Frame_Title_STR[] = "Bar frame";
const unsigned char Msg_TextFont_STR[] = "Text";
const unsigned char Msg_TextFont_Help_STR[] = "Text only buttons font.";
const unsigned char Msg_TextGfxFont_STR[] = "TextGfx";
const unsigned char Msg_TextGfxFont_Help_STR[] = "Text and image buttons font.";
const unsigned char Msg_ViewMode_STR[] = "View mode";
const unsigned char Msg_ViewMode_Help_STR[] = "Adjust the appearance of the buttons.";
const unsigned char Msg_Appearance_Text_STR[] = "The options below are used by TheBar if and only if the\n"\
    "object requested for it at creation time. It means that\n"\
    "if the programmer didn't decide otherwise, they are simply\n"\
    "ignored.";
const unsigned char Msg_TextGfx_STR[] = "Icons and text";
const unsigned char Msg_Gfx_STR[] = "Icons only";
const unsigned char Msg_Text_STR[] = "Text only";
const unsigned char Msg_Borderless_STR[] = "Borderless";
const unsigned char Msg_Borderless_Help_STR[] = "If selected, buttons are borderless.";
const unsigned char Msg_Sunny_STR[] = "Highlight";
const unsigned char Msg_Sunny_Help_STR[] = "If selected, buttons, normally in black\n"\
    "and white, are colored when the\n"\
    "mouse is over them.";
const unsigned char Msg_Raised_STR[] = "Raised";
const unsigned char Msg_Raised_Help_STR[] = "If selected, buttons, normally frameless,\n"\
    "are framed when the mouse is over them.";
const unsigned char Msg_Scaled_STR[] = "Scaled";
const unsigned char Msg_Scaled_Help_STR[] = "If selected, buttons are scaled.";
const unsigned char Msg_BarSpacer_STR[] = "Bar spacer";
const unsigned char Msg_BarSpacer_Help_STR[] = "If selected, a bar spacer is used\n"\
    "as buttons separator.";
const unsigned char Msg_EnableKeys_STR[] = "Enable keys";
const unsigned char Msg_EnableKeys_Help_STR[] = "If selected, buttons short cuts\n"\
    "are active.";
const unsigned char Msg_LabelPos_STR[] = "Labels at";
const unsigned char Msg_LabelPos_Help_STR[] = "Adjust the position of text in\n"\
    "Icons and text buttons.";
const unsigned char Msg_LabelPos_Bottom_STR[] = "Bottom";
const unsigned char Msg_LabelPos_Top_STR[] = "Top";
const unsigned char Msg_LabelPos_Right_STR[] = "Right";
const unsigned char Msg_LabelPos_Left_STR[] = "Left";
const unsigned char Msg_Title_GroupSpacing_STR[] = "Bar";
const unsigned char Msg_HorizGroupSpacing_STR[] = "Horiz";
const unsigned char Msg_HorizGroupSpacing_Help_STR[] = "Pixels between two buttons in a bar row.";
const unsigned char Msg_VertGroupSpacing_STR[] = "Vert";
const unsigned char Msg_VertGroupSpacing_Help_STR[] = "Pixels between two bar rows.";
const unsigned char Msg_BarSpacerSpacing_STR[] = "Bar";
const unsigned char Msg_BarSpacerSpacing_Help_STR[] = "Pixels between a Bar spacer\n"\
    "and the previous/next object.";
const unsigned char Msg_Title_InnerSpacing_STR[] = "Buttons";
const unsigned char Msg_HorizInnerSpacing_STR[] = "Horiz";
const unsigned char Msg_HorizInnerSpacing_Help_STR[] = "Pixels between the left/right frame\n"\
    "border and the button contents.";
const unsigned char Msg_TopInnerSpacing_STR[] = "Top";
const unsigned char Msg_TopInnerSpacing_Help_STR[] = "Pixels between the top frame border\n"\
    "and the button contents.";
const unsigned char Msg_BottomInnerSpacing_STR[] = "Bottom";
const unsigned char Msg_BottomInnerSpacing_Help_STR[] = "Pixels between the bottom frame border\n"\
    "and the button contents.";
const unsigned char Msg_Title_BarFrameSpacing_STR[] = "Bar frame";
const unsigned char Msg_LeftBarFrameSpacing_STR[] = "Left";
const unsigned char Msg_LeftBarFrameSpacing_Help_STR[] = "Pixels between the left frame border\n"\
    "and the bar contents.";
const unsigned char Msg_RightBarFrameSpacing_STR[] = "Right";
const unsigned char Msg_RightBarFrameSpacing_Help_STR[] = "Pixels between the right frame border\n"\
    "and the bar contents.";
const unsigned char Msg_TopBarFrameSpacing_STR[] = "Top";
const unsigned char Msg_TopBarFrameSpacing_Help_STR[] = "Pixels between the top frame border\n"\
    "and the bar contents.";
const unsigned char Msg_BottomBarFrameSpacing_STR[] = "Bottom";
const unsigned char Msg_BottomBarFrameSpacing_Help_STR[] = "Pixels between the bottom frame border\n"\
    "and the bar contents.";
const unsigned char Msg_Title_TextGfxSpacing_STR[] = "Text/Gfx";
const unsigned char Msg_HorizTextGfxSpacing_STR[] = "Horiz";
const unsigned char Msg_HorizTextGfxSpacing_Help_STR[] = "Pixels between an image and\n"\
    "its left/right placed label.";
const unsigned char Msg_VertTextGfxSpacing_STR[] = "Vert";
const unsigned char Msg_VertTextGfxSpacing_Help_STR[] = "Pixels between an image and\n"\
    "its bottom/top placed label.";
const unsigned char Msg_Precision_Gui_STR[] = "Bad";
const unsigned char Msg_Precision_Icon_STR[] = "Poor";
const unsigned char Msg_Precision_Image_STR[] = "Good";
const unsigned char Msg_Precision_Exact_STR[] = "Best";
const unsigned char Msg_Precision_STR[] = "Quality";
const unsigned char Msg_Precision_Help_STR[] = "Precision to use when obtaining pens.";
const unsigned char Msg_Event_IntuiTicks_STR[] = "IntuiTicks";
const unsigned char Msg_Event_MouseMove_STR[] = "MouseMove";
const unsigned char Msg_Event_STR[] = "Event";
const unsigned char Msg_Event_Help_STR[] = "How to verify if the mouse is over a button:\n"\
    "- IntuiTicks: it is verified periodically\n"\
    "- MouseMove: it is verified at every mouse move\n"\
    "- MouseObject, it is verified at when the mouse\n"\
    "over a new object";
const unsigned char Msg_Scale_STR[] = "Scale";
const unsigned char Msg_Scale_Help_STR[] = "Scale factor for scaled images.";
const unsigned char Msg_SpecialSelect_STR[] = "Special select";
const unsigned char Msg_SpecialSelect_Help_STR[] = "If selected, buttons contents is moved\n"\
    "when they are selected or active.";
const unsigned char Msg_TextOverUseShine_STR[] = "Special text";
const unsigned char Msg_TextOverUseShine_Help_STR[] = "If selected, active buttons labels\n"\
    "are drawn with the shine pen.";
const unsigned char Msg_IgnoreSelImages_STR[] = "Ignore selected images";
const unsigned char Msg_IgnoreSelImages_Help_STR[] = "If selected, selected images\n"\
    "are ignored.";
const unsigned char Msg_IgnoreDisImages_STR[] = "Ignore disabled images";
const unsigned char Msg_IgnoreDisImages_Help_STR[] = "If selected, disabled images\n"\
    "are ignored.";
const unsigned char Msg_DisMode_Shape_STR[] = "Shape";
const unsigned char Msg_DisMode_Grid_STR[] = "Grid";
const unsigned char Msg_DisMode_FullGrid_STR[] = "Full grid";
const unsigned char Msg_DisMode_STR[] = "Disable";
const unsigned char Msg_DisMode_Help_STR[] = "How to render disabled buttons.";
const unsigned char Msg_DontMove_STR[] = "Don't move";
const unsigned char Msg_DontMove_Help_STR[] = "If selected, the contents of the buttons\n"\
    "is not moved when they are active.";
const unsigned char Msg_DisMode_Sunny_STR[] = "Grey";
const unsigned char Msg_NtRaiseActive_STR[] = "Don't raise active buttons";
const unsigned char Msg_NtRaiseActive_Help_STR[] = "If selected, active buttons are not raised.";
const unsigned char Msg_SpacersSize_STR[] = "Spacers size";
const unsigned char Msg_SpacersSize_Help_STR[] = "Size of spacers.";
const unsigned char Msg_SpacersSize_Quarter_STR[] = "Quarter of a button";
const unsigned char Msg_SpacersSize_Half_STR[] = "Half of a button";
const unsigned char Msg_SpacersSize_One_STR[] = "As a button";
const unsigned char Msg_Event_MouseObject_STR[] = "MouseObject";
const unsigned char Msg_DisMode_Blend_STR[] = "Blend";
const unsigned char Msg_DisMode_BlendGrey_STR[] = "Blend grey";
const unsigned char Msg_Item_Defaults_STR[] = "Reset to defaults";
const unsigned char Msg_NoMUI20Event_Help_STR[] = "How to verify if the mouse is over a button:\n"\
    "- IntuiTicks: it is verified periodically\n"\
    "- MouseMove: it is verified at every mouse move";
const unsigned char Msg_SpacersSize_None_STR[] = "None";
const unsigned char Msg_SpacersSize_OnePoint_STR[] = "One point";
const unsigned char Msg_SpacersSize_TwoPoint_STR[] = "Two points";
const unsigned char Msg_Info_First_STR[] = "%s\n"\
    "Copyright (c) 2003-2008 Alfonso Ranieri\n"\
    "and TheBar Open Source Team";
const unsigned char Msg_Info_Reserved_STR[] = "Distributed under the terms of the LGPL2.";
const unsigned char Msg_Info_Rest_STR[] = "For recent versions and updates visit:\n"\
    "http://www.sourceforge.net/projects/thebar";
const unsigned char Msg_Info_Translator_STR[] = "";
const unsigned char Msg_Item_Globals_STR[] = "Reset to global";
const unsigned char Msg_Item_LastSaved_STR[] = "Last saved";
const unsigned char Msg_Item_Restore_STR[] = "Restore";
const unsigned char Msg_Pop_OK_STR[] = "OK";
const unsigned char Msg_Pop_OK_Help_STR[] = "Accept changes.";
const unsigned char Msg_Pop_Cancel_STR[] = "Cancel";
const unsigned char Msg_Pop_Cancel_Help_STR[] = "Exit without changing.";
const unsigned char Msg_Popback_Pattern_STR[] = "Pattern";
const unsigned char Msg_Popback_Color_STR[] = "Color";
const unsigned char Msg_Popback_Bitmap_STR[] = "Bitmap";
const unsigned char Msg_Popback_Gradient_STR[] = "Gradient";
const unsigned char Msg_Popback_HorizGradient_STR[] = "Horizontal";
const unsigned char Msg_Popback_HorizGradientHelp_STR[] = "If selected, you get a\n"\
    "horizontal gradient.";
const unsigned char Msg_Popback_VertGradient_STR[] = "Vertical";
const unsigned char Msg_Popback_VertGradientHelp_STR[] = "If selected, you get a\n"\
    "vertical gradient.";
const unsigned char Msg_Popback_From_STR[] = "From color";
const unsigned char Msg_Popback_To_STR[] = "To color";
const unsigned char Msg_Popback_Swap_STR[] = "Swap";
const unsigned char Msg_Popback_SwapHelp_STR[] = "Swaps from/to colors.";
const unsigned char Msg_Poppen_MUI_STR[] = "MUI";
const unsigned char Msg_Poppen_Colormap_STR[] = "Colormap";
const unsigned char Msg_Poppen_RGB_STR[] = "RGB";
const unsigned char Msg_Poppen_Shine_STR[] = "Shine";
const unsigned char Msg_Poppen_Halfshine_STR[] = "Halfshine";
const unsigned char Msg_Poppen_Background_STR[] = "Background";
const unsigned char Msg_Poppen_Halfshadow_STR[] = "Halfshadow";
const unsigned char Msg_Poppen_Shadow_STR[] = "Shadow";
const unsigned char Msg_Poppen_Text_STR[] = "Text";
const unsigned char Msg_Poppen_Fill_STR[] = "Fill";
const unsigned char Msg_Poppen_Mark_STR[] = "Mark";
const unsigned char Msg_Poppen_ColorMapHelp_STR[] = "Select a screen color entry.";
const unsigned char Msg_ColoradjustRed_STR[] = "Red";
const unsigned char Msg_Coloradjust_RedHelp_STR[] = "The red component of the color.";
const unsigned char Msg_ColoradjustGreen_STR[] = "Green";
const unsigned char Msg_Coloradjust_GreenHelp_STR[] = "The green component of the color.";
const unsigned char Msg_ColoradjustBlue_STR[] = "Blue";
const unsigned char Msg_Coloradjust_BlueHelp_STR[] = "The blue component of the color.";
const unsigned char Msg_Coloradjust_WheelAbbr_STR[] = "GCBMRY";
const unsigned char Msg_Coloradjust_WheelHelp_STR[] = "Move the mouse to select a color.";
const unsigned char Msg_Coloradjust_GradientHelp_STR[] = "The brightness of the color.";
const unsigned char Msg_PrefsShortHelp_STR[] = "Next generation tool bar class.";

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
    {Msg_Reg_Colors,(STRPTR)(Msg_Reg_Colors_STR)},
    {Msg_Reg_Appearance,(STRPTR)(Msg_Reg_Appearance_STR)},
    {Msg_Reg_Spacing,(STRPTR)(Msg_Reg_Spacing_STR)},
    {Msg_Reg_Options,(STRPTR)(Msg_Reg_Options_STR)},
    {Msg_Title_Backgrounds,(STRPTR)(Msg_Title_Backgrounds_STR)},
    {Msg_GroupBackground,(STRPTR)(Msg_GroupBackground_STR)},
    {Msg_GroupBackground_Help,(STRPTR)(Msg_GroupBackground_Help_STR)},
    {Msg_GroupBackground_Title,(STRPTR)(Msg_GroupBackground_Title_STR)},
    {Msg_UseGroupBackground,(STRPTR)(Msg_UseGroupBackground_STR)},
    {Msg_UseGroupBackground_Help,(STRPTR)(Msg_UseGroupBackground_Help_STR)},
    {Msg_ButtonBackground,(STRPTR)(Msg_ButtonBackground_STR)},
    {Msg_ButtonBackground_Help,(STRPTR)(Msg_ButtonBackground_Help_STR)},
    {Msg_ButtonBackground_Title,(STRPTR)(Msg_ButtonBackground_Title_STR)},
    {Msg_UseButtonBackground,(STRPTR)(Msg_UseButtonBackground_STR)},
    {Msg_UseButtonBackground_Help,(STRPTR)(Msg_UseButtonBackground_Help_STR)},
    {Msg_Title_Frame,(STRPTR)(Msg_Title_Frame_STR)},
    {Msg_FrameShinePen,(STRPTR)(Msg_FrameShinePen_STR)},
    {Msg_FrameShinePen_Help,(STRPTR)(Msg_FrameShinePen_Help_STR)},
    {Msg_FrameShinePen_Title,(STRPTR)(Msg_FrameShinePen_Title_STR)},
    {Msg_FrameShadowPen,(STRPTR)(Msg_FrameShadowPen_STR)},
    {Msg_FrameShadowPen_Help,(STRPTR)(Msg_FrameShadowPen_Help_STR)},
    {Msg_FrameShadowPen_Title,(STRPTR)(Msg_FrameShadowPen_Title_STR)},
    {Msg_FrameStyle_Recessed,(STRPTR)(Msg_FrameStyle_Recessed_STR)},
    {Msg_FrameStyle_Normal,(STRPTR)(Msg_FrameStyle_Normal_STR)},
    {Msg_FrameStyle,(STRPTR)(Msg_FrameStyle_STR)},
    {Msg_FrameStyle_Help,(STRPTR)(Msg_FrameStyle_Help_STR)},
    {Msg_Title_Disabled,(STRPTR)(Msg_Title_Disabled_STR)},
    {Msg_DisabledBody,(STRPTR)(Msg_DisabledBody_STR)},
    {Msg_DisabledBody_Help,(STRPTR)(Msg_DisabledBody_Help_STR)},
    {Msg_DisabledBody_Title,(STRPTR)(Msg_DisabledBody_Title_STR)},
    {Msg_DisabledShadow,(STRPTR)(Msg_DisabledShadow_STR)},
    {Msg_DisabledShadow_Help,(STRPTR)(Msg_DisabledShadow_Help_STR)},
    {Msg_DisabledShadow_Title,(STRPTR)(Msg_DisabledShadow_Title_STR)},
    {Msg_Title_BarSpacer,(STRPTR)(Msg_Title_BarSpacer_STR)},
    {Msg_BarSpacerShinePen,(STRPTR)(Msg_BarSpacerShinePen_STR)},
    {Msg_BarSpacerShinePen_Help,(STRPTR)(Msg_BarSpacerShinePen_Help_STR)},
    {Msg_BarSpacerShinePen_Title,(STRPTR)(Msg_BarSpacerShinePen_Title_STR)},
    {Msg_BarSpacerShadowPen,(STRPTR)(Msg_BarSpacerShadowPen_STR)},
    {Msg_BarSpacerShadowPen_Help,(STRPTR)(Msg_BarSpacerShadowPen_Help_STR)},
    {Msg_BarSpacerShadowPen_Title,(STRPTR)(Msg_BarSpacerShadowPen_Title_STR)},
    {Msg_Title_BarFrame,(STRPTR)(Msg_Title_BarFrame_STR)},
    {Msg_BarFrameShine,(STRPTR)(Msg_BarFrameShine_STR)},
    {Msg_BarFrameShine_Help,(STRPTR)(Msg_BarFrameShine_Help_STR)},
    {Msg_BarFrameShine_Title,(STRPTR)(Msg_BarFrameShine_Title_STR)},
    {Msg_BarFrameShadow,(STRPTR)(Msg_BarFrameShadow_STR)},
    {Msg_BarFrameShadow_Help,(STRPTR)(Msg_BarFrameShadow_Help_STR)},
    {Msg_BarFrameShadow_Title,(STRPTR)(Msg_BarFrameShadow_Title_STR)},
    {Msg_Title_DragBar,(STRPTR)(Msg_Title_DragBar_STR)},
    {Msg_DragBarShine,(STRPTR)(Msg_DragBarShine_STR)},
    {Msg_DragBarShine_Help,(STRPTR)(Msg_DragBarShine_Help_STR)},
    {Msg_DragBarShine_Title,(STRPTR)(Msg_DragBarShine_Title_STR)},
    {Msg_DragBarShadow,(STRPTR)(Msg_DragBarShadow_STR)},
    {Msg_DragBarShadow_Help,(STRPTR)(Msg_DragBarShadow_Help_STR)},
    {Msg_DragBarShadow_Title,(STRPTR)(Msg_DragBarShadow_Title_STR)},
    {Msg_DragBarFill,(STRPTR)(Msg_DragBarFill_STR)},
    {Msg_DragBarFill_Help,(STRPTR)(Msg_DragBarFill_Help_STR)},
    {Msg_DragBarFill_Title,(STRPTR)(Msg_DragBarFill_Title_STR)},
    {Msg_UseDragBarFill,(STRPTR)(Msg_UseDragBarFill_STR)},
    {Msg_UseDragBarFill_Help,(STRPTR)(Msg_UseDragBarFill_Help_STR)},
    {Msg_Frame,(STRPTR)(Msg_Frame_STR)},
    {Msg_Frame_Help,(STRPTR)(Msg_Frame_Help_STR)},
    {Msg_Frame_Title,(STRPTR)(Msg_Frame_Title_STR)},
    {Msg_TextFont,(STRPTR)(Msg_TextFont_STR)},
    {Msg_TextFont_Help,(STRPTR)(Msg_TextFont_Help_STR)},
    {Msg_TextGfxFont,(STRPTR)(Msg_TextGfxFont_STR)},
    {Msg_TextGfxFont_Help,(STRPTR)(Msg_TextGfxFont_Help_STR)},
    {Msg_ViewMode,(STRPTR)(Msg_ViewMode_STR)},
    {Msg_ViewMode_Help,(STRPTR)(Msg_ViewMode_Help_STR)},
    {Msg_Appearance_Text,(STRPTR)(Msg_Appearance_Text_STR)},
    {Msg_TextGfx,(STRPTR)(Msg_TextGfx_STR)},
    {Msg_Gfx,(STRPTR)(Msg_Gfx_STR)},
    {Msg_Text,(STRPTR)(Msg_Text_STR)},
    {Msg_Borderless,(STRPTR)(Msg_Borderless_STR)},
    {Msg_Borderless_Help,(STRPTR)(Msg_Borderless_Help_STR)},
    {Msg_Sunny,(STRPTR)(Msg_Sunny_STR)},
    {Msg_Sunny_Help,(STRPTR)(Msg_Sunny_Help_STR)},
    {Msg_Raised,(STRPTR)(Msg_Raised_STR)},
    {Msg_Raised_Help,(STRPTR)(Msg_Raised_Help_STR)},
    {Msg_Scaled,(STRPTR)(Msg_Scaled_STR)},
    {Msg_Scaled_Help,(STRPTR)(Msg_Scaled_Help_STR)},
    {Msg_BarSpacer,(STRPTR)(Msg_BarSpacer_STR)},
    {Msg_BarSpacer_Help,(STRPTR)(Msg_BarSpacer_Help_STR)},
    {Msg_EnableKeys,(STRPTR)(Msg_EnableKeys_STR)},
    {Msg_EnableKeys_Help,(STRPTR)(Msg_EnableKeys_Help_STR)},
    {Msg_LabelPos,(STRPTR)(Msg_LabelPos_STR)},
    {Msg_LabelPos_Help,(STRPTR)(Msg_LabelPos_Help_STR)},
    {Msg_LabelPos_Bottom,(STRPTR)(Msg_LabelPos_Bottom_STR)},
    {Msg_LabelPos_Top,(STRPTR)(Msg_LabelPos_Top_STR)},
    {Msg_LabelPos_Right,(STRPTR)(Msg_LabelPos_Right_STR)},
    {Msg_LabelPos_Left,(STRPTR)(Msg_LabelPos_Left_STR)},
    {Msg_Title_GroupSpacing,(STRPTR)(Msg_Title_GroupSpacing_STR)},
    {Msg_HorizGroupSpacing,(STRPTR)(Msg_HorizGroupSpacing_STR)},
    {Msg_HorizGroupSpacing_Help,(STRPTR)(Msg_HorizGroupSpacing_Help_STR)},
    {Msg_VertGroupSpacing,(STRPTR)(Msg_VertGroupSpacing_STR)},
    {Msg_VertGroupSpacing_Help,(STRPTR)(Msg_VertGroupSpacing_Help_STR)},
    {Msg_BarSpacerSpacing,(STRPTR)(Msg_BarSpacerSpacing_STR)},
    {Msg_BarSpacerSpacing_Help,(STRPTR)(Msg_BarSpacerSpacing_Help_STR)},
    {Msg_Title_InnerSpacing,(STRPTR)(Msg_Title_InnerSpacing_STR)},
    {Msg_HorizInnerSpacing,(STRPTR)(Msg_HorizInnerSpacing_STR)},
    {Msg_HorizInnerSpacing_Help,(STRPTR)(Msg_HorizInnerSpacing_Help_STR)},
    {Msg_TopInnerSpacing,(STRPTR)(Msg_TopInnerSpacing_STR)},
    {Msg_TopInnerSpacing_Help,(STRPTR)(Msg_TopInnerSpacing_Help_STR)},
    {Msg_BottomInnerSpacing,(STRPTR)(Msg_BottomInnerSpacing_STR)},
    {Msg_BottomInnerSpacing_Help,(STRPTR)(Msg_BottomInnerSpacing_Help_STR)},
    {Msg_Title_BarFrameSpacing,(STRPTR)(Msg_Title_BarFrameSpacing_STR)},
    {Msg_LeftBarFrameSpacing,(STRPTR)(Msg_LeftBarFrameSpacing_STR)},
    {Msg_LeftBarFrameSpacing_Help,(STRPTR)(Msg_LeftBarFrameSpacing_Help_STR)},
    {Msg_RightBarFrameSpacing,(STRPTR)(Msg_RightBarFrameSpacing_STR)},
    {Msg_RightBarFrameSpacing_Help,(STRPTR)(Msg_RightBarFrameSpacing_Help_STR)},
    {Msg_TopBarFrameSpacing,(STRPTR)(Msg_TopBarFrameSpacing_STR)},
    {Msg_TopBarFrameSpacing_Help,(STRPTR)(Msg_TopBarFrameSpacing_Help_STR)},
    {Msg_BottomBarFrameSpacing,(STRPTR)(Msg_BottomBarFrameSpacing_STR)},
    {Msg_BottomBarFrameSpacing_Help,(STRPTR)(Msg_BottomBarFrameSpacing_Help_STR)},
    {Msg_Title_TextGfxSpacing,(STRPTR)(Msg_Title_TextGfxSpacing_STR)},
    {Msg_HorizTextGfxSpacing,(STRPTR)(Msg_HorizTextGfxSpacing_STR)},
    {Msg_HorizTextGfxSpacing_Help,(STRPTR)(Msg_HorizTextGfxSpacing_Help_STR)},
    {Msg_VertTextGfxSpacing,(STRPTR)(Msg_VertTextGfxSpacing_STR)},
    {Msg_VertTextGfxSpacing_Help,(STRPTR)(Msg_VertTextGfxSpacing_Help_STR)},
    {Msg_Precision_Gui,(STRPTR)(Msg_Precision_Gui_STR)},
    {Msg_Precision_Icon,(STRPTR)(Msg_Precision_Icon_STR)},
    {Msg_Precision_Image,(STRPTR)(Msg_Precision_Image_STR)},
    {Msg_Precision_Exact,(STRPTR)(Msg_Precision_Exact_STR)},
    {Msg_Precision,(STRPTR)(Msg_Precision_STR)},
    {Msg_Precision_Help,(STRPTR)(Msg_Precision_Help_STR)},
    {Msg_Event_IntuiTicks,(STRPTR)(Msg_Event_IntuiTicks_STR)},
    {Msg_Event_MouseMove,(STRPTR)(Msg_Event_MouseMove_STR)},
    {Msg_Event,(STRPTR)(Msg_Event_STR)},
    {Msg_Event_Help,(STRPTR)(Msg_Event_Help_STR)},
    {Msg_Scale,(STRPTR)(Msg_Scale_STR)},
    {Msg_Scale_Help,(STRPTR)(Msg_Scale_Help_STR)},
    {Msg_SpecialSelect,(STRPTR)(Msg_SpecialSelect_STR)},
    {Msg_SpecialSelect_Help,(STRPTR)(Msg_SpecialSelect_Help_STR)},
    {Msg_TextOverUseShine,(STRPTR)(Msg_TextOverUseShine_STR)},
    {Msg_TextOverUseShine_Help,(STRPTR)(Msg_TextOverUseShine_Help_STR)},
    {Msg_IgnoreSelImages,(STRPTR)(Msg_IgnoreSelImages_STR)},
    {Msg_IgnoreSelImages_Help,(STRPTR)(Msg_IgnoreSelImages_Help_STR)},
    {Msg_IgnoreDisImages,(STRPTR)(Msg_IgnoreDisImages_STR)},
    {Msg_IgnoreDisImages_Help,(STRPTR)(Msg_IgnoreDisImages_Help_STR)},
    {Msg_DisMode_Shape,(STRPTR)(Msg_DisMode_Shape_STR)},
    {Msg_DisMode_Grid,(STRPTR)(Msg_DisMode_Grid_STR)},
    {Msg_DisMode_FullGrid,(STRPTR)(Msg_DisMode_FullGrid_STR)},
    {Msg_DisMode,(STRPTR)(Msg_DisMode_STR)},
    {Msg_DisMode_Help,(STRPTR)(Msg_DisMode_Help_STR)},
    {Msg_DontMove,(STRPTR)(Msg_DontMove_STR)},
    {Msg_DontMove_Help,(STRPTR)(Msg_DontMove_Help_STR)},
    {Msg_DisMode_Sunny,(STRPTR)(Msg_DisMode_Sunny_STR)},
    {Msg_NtRaiseActive,(STRPTR)(Msg_NtRaiseActive_STR)},
    {Msg_NtRaiseActive_Help,(STRPTR)(Msg_NtRaiseActive_Help_STR)},
    {Msg_SpacersSize,(STRPTR)(Msg_SpacersSize_STR)},
    {Msg_SpacersSize_Help,(STRPTR)(Msg_SpacersSize_Help_STR)},
    {Msg_SpacersSize_Quarter,(STRPTR)(Msg_SpacersSize_Quarter_STR)},
    {Msg_SpacersSize_Half,(STRPTR)(Msg_SpacersSize_Half_STR)},
    {Msg_SpacersSize_One,(STRPTR)(Msg_SpacersSize_One_STR)},
    {Msg_Event_MouseObject,(STRPTR)(Msg_Event_MouseObject_STR)},
    {Msg_DisMode_Blend,(STRPTR)(Msg_DisMode_Blend_STR)},
    {Msg_DisMode_BlendGrey,(STRPTR)(Msg_DisMode_BlendGrey_STR)},
    {Msg_Item_Defaults,(STRPTR)(Msg_Item_Defaults_STR)},
    {Msg_NoMUI20Event_Help,(STRPTR)(Msg_NoMUI20Event_Help_STR)},
    {Msg_SpacersSize_None,(STRPTR)(Msg_SpacersSize_None_STR)},
    {Msg_SpacersSize_OnePoint,(STRPTR)(Msg_SpacersSize_OnePoint_STR)},
    {Msg_SpacersSize_TwoPoint,(STRPTR)(Msg_SpacersSize_TwoPoint_STR)},
    {Msg_Info_First,(STRPTR)(Msg_Info_First_STR)},
    {Msg_Info_Reserved,(STRPTR)(Msg_Info_Reserved_STR)},
    {Msg_Info_Rest,(STRPTR)(Msg_Info_Rest_STR)},
    {Msg_Info_Translator,(STRPTR)(Msg_Info_Translator_STR)},
    {Msg_Item_Globals,(STRPTR)(Msg_Item_Globals_STR)},
    {Msg_Item_LastSaved,(STRPTR)(Msg_Item_LastSaved_STR)},
    {Msg_Item_Restore,(STRPTR)(Msg_Item_Restore_STR)},
    {Msg_Pop_OK,(STRPTR)(Msg_Pop_OK_STR)},
    {Msg_Pop_OK_Help,(STRPTR)(Msg_Pop_OK_Help_STR)},
    {Msg_Pop_Cancel,(STRPTR)(Msg_Pop_Cancel_STR)},
    {Msg_Pop_Cancel_Help,(STRPTR)(Msg_Pop_Cancel_Help_STR)},
    {Msg_Popback_Pattern,(STRPTR)(Msg_Popback_Pattern_STR)},
    {Msg_Popback_Color,(STRPTR)(Msg_Popback_Color_STR)},
    {Msg_Popback_Bitmap,(STRPTR)(Msg_Popback_Bitmap_STR)},
    {Msg_Popback_Gradient,(STRPTR)(Msg_Popback_Gradient_STR)},
    {Msg_Popback_HorizGradient,(STRPTR)(Msg_Popback_HorizGradient_STR)},
    {Msg_Popback_HorizGradientHelp,(STRPTR)(Msg_Popback_HorizGradientHelp_STR)},
    {Msg_Popback_VertGradient,(STRPTR)(Msg_Popback_VertGradient_STR)},
    {Msg_Popback_VertGradientHelp,(STRPTR)(Msg_Popback_VertGradientHelp_STR)},
    {Msg_Popback_From,(STRPTR)(Msg_Popback_From_STR)},
    {Msg_Popback_To,(STRPTR)(Msg_Popback_To_STR)},
    {Msg_Popback_Swap,(STRPTR)(Msg_Popback_Swap_STR)},
    {Msg_Popback_SwapHelp,(STRPTR)(Msg_Popback_SwapHelp_STR)},
    {Msg_Poppen_MUI,(STRPTR)(Msg_Poppen_MUI_STR)},
    {Msg_Poppen_Colormap,(STRPTR)(Msg_Poppen_Colormap_STR)},
    {Msg_Poppen_RGB,(STRPTR)(Msg_Poppen_RGB_STR)},
    {Msg_Poppen_Shine,(STRPTR)(Msg_Poppen_Shine_STR)},
    {Msg_Poppen_Halfshine,(STRPTR)(Msg_Poppen_Halfshine_STR)},
    {Msg_Poppen_Background,(STRPTR)(Msg_Poppen_Background_STR)},
    {Msg_Poppen_Halfshadow,(STRPTR)(Msg_Poppen_Halfshadow_STR)},
    {Msg_Poppen_Shadow,(STRPTR)(Msg_Poppen_Shadow_STR)},
    {Msg_Poppen_Text,(STRPTR)(Msg_Poppen_Text_STR)},
    {Msg_Poppen_Fill,(STRPTR)(Msg_Poppen_Fill_STR)},
    {Msg_Poppen_Mark,(STRPTR)(Msg_Poppen_Mark_STR)},
    {Msg_Poppen_ColorMapHelp,(STRPTR)(Msg_Poppen_ColorMapHelp_STR)},
    {Msg_ColoradjustRed,(STRPTR)(Msg_ColoradjustRed_STR)},
    {Msg_Coloradjust_RedHelp,(STRPTR)(Msg_Coloradjust_RedHelp_STR)},
    {Msg_ColoradjustGreen,(STRPTR)(Msg_ColoradjustGreen_STR)},
    {Msg_Coloradjust_GreenHelp,(STRPTR)(Msg_Coloradjust_GreenHelp_STR)},
    {Msg_ColoradjustBlue,(STRPTR)(Msg_ColoradjustBlue_STR)},
    {Msg_Coloradjust_BlueHelp,(STRPTR)(Msg_Coloradjust_BlueHelp_STR)},
    {Msg_Coloradjust_WheelAbbr,(STRPTR)(Msg_Coloradjust_WheelAbbr_STR)},
    {Msg_Coloradjust_WheelHelp,(STRPTR)(Msg_Coloradjust_WheelHelp_STR)},
    {Msg_Coloradjust_GradientHelp,(STRPTR)(Msg_Coloradjust_GradientHelp_STR)},
    {Msg_PrefsShortHelp,(STRPTR)(Msg_PrefsShortHelp_STR)},
    {0,NULL} /* Yes, I know about %(,); I just like it this way. */
};

#endif /* CATCOMP_ARRAY */

/****************************************************************************/

#endif /* _LOC_H */
