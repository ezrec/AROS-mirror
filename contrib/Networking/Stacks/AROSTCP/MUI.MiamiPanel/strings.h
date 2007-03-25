/****************************************************************
   This file was created automatically by `FlexCat 2.4'
   from "MUI.MiamiPanel.cd".

   Do NOT edit by hand!
****************************************************************/

#ifndef MUI.MiamiPanel_H
#define MUI.MiamiPanel_H


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

/***************************************************************/


#ifdef CATCOMP_NUMBERS

#define MSG_Copyright 0
#define MSG_Description 1
#define MSG_Menu_Project 2
#define MSG_Menu_About 3
#define MSG_Menu_AboutMUI 4
#define MSG_Menu_Hide 5
#define MSG_Menu_Quit 6
#define MSG_Menu_Miami 7
#define MSG_Menu_Miami_Show 8
#define MSG_Menu_Miami_Hide 9
#define MSG_Menu_Miami_Quit 10
#define MSG_Menu_Prefs 11
#define MSG_Menu_Edit 12
#define MSG_Menu_Save 13
#define MSG_Menu_Use 14
#define MSG_Menu_Restore 15
#define MSG_Menu_LastSaved 16
#define MSG_Menu_MUI 17
#define MSG_Window_Title 18
#define MSG_Button_Show 19
#define MSG_Button_Show_Help 20
#define MSG_Button_Hide 21
#define MSG_Button_Hide_Help 22
#define MSG_Button_Quit 23
#define MSG_Button_Quit_Help 24
#define MSG_IFGroup_Help 25
#define MSG_IFGroup_CItem_Online 26
#define MSG_IFGroup_CItem_Offline 27
#define MSG_IFGroup_CItem_Scale 28
#define MSG_IFGroup_CItem_ScaleFmt 29
#define MSG_IF_Title_Name 30
#define MSG_IF_Title_State 31
#define MSG_IF_Title_OnTime 32
#define MSG_IF_Title_Rate 33
#define MSG_IF_Title_Speed 34
#define MSG_IF_Title_Traffic 35
#define MSG_IF_Button_Online 36
#define MSG_IF_Button_Offline 37
#define MSG_IF_Status_GoingOnline 38
#define MSG_IF_Status_GoingOffline 39
#define MSG_IF_Status_Suspending 40
#define MSG_IF_Status_Online 41
#define MSG_IF_Status_Offline 42
#define MSG_IF_Status_Suspended 43
#define MSG_Traffic_Byte 44
#define MSG_Traffic_Bytes 45
#define MSG_Traffic_KB 46
#define MSG_Traffic_MB 47
#define MSG_Rate_Bs 48
#define MSG_Rate_KBs 49
#define MSG_Rate_MBs 50
#define MSG_Prefs_WinTitle 51
#define MSG_Prefs_Interfaces 52
#define MSG_Prefs_MiamiCtrls 53
#define MSG_Prefs_Options 54
#define MSG_Prefs_IF_Traffic_Format 55
#define MSG_Prefs_IF_Traffic_Format_Help 56
#define MSG_Prefs_IF_Traffic_Format_Long 57
#define MSG_Prefs_IF_Traffic_Format_Short 58
#define MSG_Prefs_IF_Traffic_Grouping 59
#define MSG_Prefs_IF_Traffic_Grouping_Help 60
#define MSG_Prefs_IF_Rate_Format 61
#define MSG_Prefs_IF_Rate_Format_Help 62
#define MSG_Prefs_IF_Rate_Format_Long 63
#define MSG_Prefs_IF_Rate_Format_Short 64
#define MSG_Prefs_IF_Rate_Grouping 65
#define MSG_Prefs_IF_Rate_Grouping_Help 66
#define MSG_Prefs_Miami_Position 67
#define MSG_Prefs_Miami_Position_Help 68
#define MSG_Prefs_Miami_Position_Top 69
#define MSG_Prefs_Miami_Position_Bottom 70
#define MSG_Prefs_Miami_Position_Left 71
#define MSG_Prefs_Miami_Position_Right 72
#define MSG_Prefs_Miami_ViewMode 73
#define MSG_Prefs_Miami_ViewMode_Help 74
#define MSG_Prefs_Miami_ViewMode_TextGfx 75
#define MSG_Prefs_Miami_ViewMode_Gfx 76
#define MSG_Prefs_Miami_ViewMode_Text 77
#define MSG_Prefs_Miami_Borderless 78
#define MSG_Prefs_Miami_Borderless_Help 79
#define MSG_Prefs_Miami_Sunny 80
#define MSG_Prefs_Miami_Sunny_Help 81
#define MSG_Prefs_Miami_Raised 82
#define MSG_Prefs_Miami_Raised_Help 83
#define MSG_Prefs_Miami_Scaled 84
#define MSG_Prefs_Miami_Scaled_Help 85
#define MSG_Prefs_Miami_LabelPosition 86
#define MSG_Prefs_Miami_LabelPosition_Help 87
#define MSG_Prefs_Miami_LabelPosition_Bottom 88
#define MSG_Prefs_Miami_LabelPosition_Top 89
#define MSG_Prefs_Miami_LabelPosition_Right 90
#define MSG_Prefs_Miami_LabelPosition_Left 91
#define MSG_Prefs_Miami_BarLayout 92
#define MSG_Prefs_Miami_BarLayout_Help 93
#define MSG_Prefs_Miami_BarLayout_Left 94
#define MSG_Prefs_Miami_BarLayout_Center 95
#define MSG_Prefs_Miami_BarLayout_Right 96
#define MSG_Prefs_Miami_Underscore 97
#define MSG_Prefs_Miami_Underscore_Help 98
#define MSG_Prefs_OnIconify 99
#define MSG_Prefs_OnIconify_Help 100
#define MSG_Prefs_OI_Quit 101
#define MSG_Prefs_OI_Hide 102
#define MSG_Prefs_BWin 103
#define MSG_Prefs_BWin_Help 104
#define MSG_Prefs_BWinBorders 105
#define MSG_Prefs_BWinBorders_Help 106
#define MSG_Prefs_Save 107
#define MSG_Prefs_Save_Help 108
#define MSG_Prefs_Use 109
#define MSG_Prefs_Use_Help 110
#define MSG_Prefs_Apply 111
#define MSG_Prefs_Apply_Help 112
#define MSG_Prefs_Test 113
#define MSG_Prefs_Test_Help 114
#define MSG_Prefs_Cancel 115
#define MSG_Prefs_Cancel_Help 116
#define MSG_Prefs_IF_UseBusyBar 117
#define MSG_Prefs_IF_UseBusyBar_Help 118
#define MSG_Prefs_Miami_Frame 119
#define MSG_Prefs_Miami_Frame_Help 120
#define MSG_Prefs_Miami_DragBar 121
#define MSG_Prefs_Miami_DragBar_Help 122
#define MSG_Prefs_UseTransparency 123
#define MSG_Prefs_UseTransparency_Help 124
#define MSG_About_WinTitle 125
#define MSG_About_Information 126
#define MSG_About_Version 127
#define MSG_About_Author 128
#define MSG_About_Support 129
#define MSG_About_MiamiSite 130
#define MSG_About_ThirdParts 131
#define MSG_About_Lamp 132
#define MSG_About_Busy 133
#define MSG_About_OfCourse 134
#define MSG_About_Translation 135

#endif /* CATCOMP_NUMBERS */


/***************************************************************/

#ifdef CATCOMP_STRINGS

#define MSG_Copyright_STR "Copyright 2001-2005 by Alfonso Ranieri"
#define MSG_Description_STR "MUI MiamiDx control panel."
#define MSG_Menu_Project_STR "Project"
#define MSG_Menu_About_STR "?\000About..."
#define MSG_Menu_AboutMUI_STR "!\000About MUI..."
#define MSG_Menu_Hide_STR "H\000Hide"
#define MSG_Menu_Quit_STR "Q\000Quit"
#define MSG_Menu_Miami_STR "MiamiDx"
#define MSG_Menu_Miami_Show_STR "W\000Show..."
#define MSG_Menu_Miami_Hide_STR "D\000Hide"
#define MSG_Menu_Miami_Quit_STR "T\000Quit"
#define MSG_Menu_Prefs_STR "Preferences"
#define MSG_Menu_Edit_STR "E\000Edit..."
#define MSG_Menu_Save_STR "S\000Save"
#define MSG_Menu_Use_STR "U\000Use"
#define MSG_Menu_Restore_STR "R\000Restore"
#define MSG_Menu_LastSaved_STR "L\000Last saved"
#define MSG_Menu_MUI_STR "M\000MUI Settings..."
#define MSG_Window_Title_STR "MUI.MiamiPanel"
#define MSG_Button_Show_STR "Sho_w"
#define MSG_Button_Show_Help_STR "Show MiamiDx main window."
#define MSG_Button_Hide_STR "Hi_de"
#define MSG_Button_Hide_Help_STR "Hide MiamiDx main window."
#define MSG_Button_Quit_STR "Qui_t"
#define MSG_Button_Quit_Help_STR "Quit MiamiDx."
#define MSG_IFGroup_Help_STR "Interfaces list. If you chose to have\n"\
	"the On/Offline button, clicking it\n"\
	"puts the interface On/Offline."
#define MSG_IFGroup_CItem_Online_STR "Online"
#define MSG_IFGroup_CItem_Offline_STR "Offline"
#define MSG_IFGroup_CItem_Scale_STR "Scale"
#define MSG_IFGroup_CItem_ScaleFmt_STR "%ldKb"
#define MSG_IF_Title_Name_STR "Name:"
#define MSG_IF_Title_State_STR "State:"
#define MSG_IF_Title_OnTime_STR "Ontime:"
#define MSG_IF_Title_Rate_STR "Rate:"
#define MSG_IF_Title_Speed_STR "Speed:"
#define MSG_IF_Title_Traffic_STR "Traffic:"
#define MSG_IF_Button_Online_STR "Online"
#define MSG_IF_Button_Offline_STR "Offline"
#define MSG_IF_Status_GoingOnline_STR "Going online..."
#define MSG_IF_Status_GoingOffline_STR "Going offline..."
#define MSG_IF_Status_Suspending_STR "Suspending..."
#define MSG_IF_Status_Online_STR "Online"
#define MSG_IF_Status_Offline_STR "Offline"
#define MSG_IF_Status_Suspended_STR "Suspended"
#define MSG_Traffic_Byte_STR "byte"
#define MSG_Traffic_Bytes_STR "bytes"
#define MSG_Traffic_KB_STR "Kb"
#define MSG_Traffic_MB_STR "Mb"
#define MSG_Rate_Bs_STR "bps"
#define MSG_Rate_KBs_STR "Kb/s"
#define MSG_Rate_MBs_STR "Mb/s"
#define MSG_Prefs_WinTitle_STR "MUI.MiamiPanel Preferences"
#define MSG_Prefs_Interfaces_STR "Interfaces"
#define MSG_Prefs_MiamiCtrls_STR "Buttons"
#define MSG_Prefs_Options_STR "Options"
#define MSG_Prefs_IF_Traffic_Format_STR "T_raffic format"
#define MSG_Prefs_IF_Traffic_Format_Help_STR "Traffic format:\n"\
	"Long - traffic shown in bytes\n"\
	"Short - traffic shown in Kb/Mb"
#define MSG_Prefs_IF_Traffic_Format_Long_STR "Long"
#define MSG_Prefs_IF_Traffic_Format_Short_STR "Short"
#define MSG_Prefs_IF_Traffic_Grouping_STR "_Group"
#define MSG_Prefs_IF_Traffic_Grouping_Help_STR "If selected, traffic\n"\
	"digits are grouped."
#define MSG_Prefs_IF_Rate_Format_STR "Rat_e format"
#define MSG_Prefs_IF_Rate_Format_Help_STR "Rate format:\n"\
	"Long - rate shown in bytes\n"\
	"Short - rate shown in Kb/Mb"
#define MSG_Prefs_IF_Rate_Format_Long_STR "Long"
#define MSG_Prefs_IF_Rate_Format_Short_STR "Short"
#define MSG_Prefs_IF_Rate_Grouping_STR "Gr_oup"
#define MSG_Prefs_IF_Rate_Grouping_Help_STR "If selected, rate\n"\
	"digits are grouped."
#define MSG_Prefs_Miami_Position_STR "_Position"
#define MSG_Prefs_Miami_Position_Help_STR "Adjust the position\n"\
	"of the buttons bar."
#define MSG_Prefs_Miami_Position_Top_STR "Top"
#define MSG_Prefs_Miami_Position_Bottom_STR "Bottom"
#define MSG_Prefs_Miami_Position_Left_STR "Left"
#define MSG_Prefs_Miami_Position_Right_STR "Right"
#define MSG_Prefs_Miami_ViewMode_STR "Appea_rance"
#define MSG_Prefs_Miami_ViewMode_Help_STR "Adjust the appearance\n"\
	"of the buttons bar."
#define MSG_Prefs_Miami_ViewMode_TextGfx_STR "Icons and text"
#define MSG_Prefs_Miami_ViewMode_Gfx_STR "Icons only"
#define MSG_Prefs_Miami_ViewMode_Text_STR "Text only"
#define MSG_Prefs_Miami_Borderless_STR "_Borderless"
#define MSG_Prefs_Miami_Borderless_Help_STR "If selected, buttons\n"\
	"are borderless."
#define MSG_Prefs_Miami_Sunny_STR "_Highlight"
#define MSG_Prefs_Miami_Sunny_Help_STR "If selected, buttons,\n"\
	"rendered in black and\n"\
	"white, are colored when\n"\
	"the mouse is over them."
#define MSG_Prefs_Miami_Raised_STR "Rais_ed"
#define MSG_Prefs_Miami_Raised_Help_STR "If selected, a border is drawn\n"\
	"around borderless buttons,\n"\
	"when the mouse is over them."
#define MSG_Prefs_Miami_Scaled_STR "Scale_d"
#define MSG_Prefs_Miami_Scaled_Help_STR "If selected, buttons are\n"\
	"scaled, according to\n"\
	"MUI/TheBar prefereces."
#define MSG_Prefs_Miami_LabelPosition_STR "_Label position"
#define MSG_Prefs_Miami_LabelPosition_Help_STR "Adjust the position of the\n"\
	"text in \"Icons and text\"\n"\
	"buttons."
#define MSG_Prefs_Miami_LabelPosition_Bottom_STR "Bottom"
#define MSG_Prefs_Miami_LabelPosition_Top_STR "Top"
#define MSG_Prefs_Miami_LabelPosition_Right_STR "Right"
#define MSG_Prefs_Miami_LabelPosition_Left_STR "Left"
#define MSG_Prefs_Miami_BarLayout_STR "_Inner position"
#define MSG_Prefs_Miami_BarLayout_Help_STR "Adjust the position of the\n"\
	"buttons in the bar."
#define MSG_Prefs_Miami_BarLayout_Left_STR "Left/Up"
#define MSG_Prefs_Miami_BarLayout_Center_STR "Center"
#define MSG_Prefs_Miami_BarLayout_Right_STR "Right/Down"
#define MSG_Prefs_Miami_Underscore_STR "Use sh_ort cuts"
#define MSG_Prefs_Miami_Underscore_Help_STR "If selected, buttons text\n"\
	"shortcuts are used."
#define MSG_Prefs_OnIconify_STR "_On iconify"
#define MSG_Prefs_OnIconify_Help_STR "Action to take on iconify:\n"\
	"Quit - quit the panel\n"\
	"Hide - iconify the panel"
#define MSG_Prefs_OI_Quit_STR "Quit"
#define MSG_Prefs_OI_Hide_STR "Hide"
#define MSG_Prefs_BWin_STR "Us_e BWin"
#define MSG_Prefs_BWin_Help_STR "If selected, the panel uses\n"\
	"BWin.mcc for its main window."
#define MSG_Prefs_BWinBorders_STR "Use _BWin borders"
#define MSG_Prefs_BWinBorders_Help_STR "If selected, BWin.mcc\n"\
	"uses borders."
#define MSG_Prefs_Save_STR "_Save"
#define MSG_Prefs_Save_Help_STR "Save preferences and exit."
#define MSG_Prefs_Use_STR "_Use"
#define MSG_Prefs_Use_Help_STR "Use preferences and exit."
#define MSG_Prefs_Apply_STR "_Apply"
#define MSG_Prefs_Apply_Help_STR "Apply preferences and exit."
#define MSG_Prefs_Test_STR "_Test"
#define MSG_Prefs_Test_Help_STR "Test preferences without exit.\n"\
	"Preferences are restored\n"\
	"before exit."
#define MSG_Prefs_Cancel_STR "_Cancel"
#define MSG_Prefs_Cancel_Help_STR "Exit without save."
#define MSG_Prefs_IF_UseBusyBar_STR "Use _busy bar"
#define MSG_Prefs_IF_UseBusyBar_Help_STR "If selected, a busy bar appears\n"\
	"when an interface changes its\n"\
	"state."
#define MSG_Prefs_Miami_Frame_STR "_Frame"
#define MSG_Prefs_Miami_Frame_Help_STR "If selected, buttons\n"\
	"bar is framed."
#define MSG_Prefs_Miami_DragBar_STR "_Drag bar"
#define MSG_Prefs_Miami_DragBar_Help_STR "If selected, buttons\n"\
	"bar is draggable."
#define MSG_Prefs_UseTransparency_STR "Use t_ransparency"
#define MSG_Prefs_UseTransparency_Help_STR "If selected, the panel uses transparency."
#define MSG_About_WinTitle_STR "About MUI.MiamiPanel"
#define MSG_About_Information_STR "Information"
#define MSG_About_Version_STR "Version:"
#define MSG_About_Author_STR "Author:"
#define MSG_About_Support_STR "Support:"
#define MSG_About_MiamiSite_STR "MiamiDx site:"
#define MSG_About_ThirdParts_STR "Third parts"
#define MSG_About_Lamp_STR "Lamp.mcc by"
#define MSG_About_Busy_STR "Busy.mcc by"
#define MSG_About_OfCourse_STR ", of course!"
#define MSG_About_Translation_STR ""

#endif /* CATCOMP_STRINGS */


/***************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
  LONG   cca_ID;
  STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
  {MSG_Copyright,(STRPTR)MSG_Copyright_STR},
  {MSG_Description,(STRPTR)MSG_Description_STR},
  {MSG_Menu_Project,(STRPTR)MSG_Menu_Project_STR},
  {MSG_Menu_About,(STRPTR)MSG_Menu_About_STR},
  {MSG_Menu_AboutMUI,(STRPTR)MSG_Menu_AboutMUI_STR},
  {MSG_Menu_Hide,(STRPTR)MSG_Menu_Hide_STR},
  {MSG_Menu_Quit,(STRPTR)MSG_Menu_Quit_STR},
  {MSG_Menu_Miami,(STRPTR)MSG_Menu_Miami_STR},
  {MSG_Menu_Miami_Show,(STRPTR)MSG_Menu_Miami_Show_STR},
  {MSG_Menu_Miami_Hide,(STRPTR)MSG_Menu_Miami_Hide_STR},
  {MSG_Menu_Miami_Quit,(STRPTR)MSG_Menu_Miami_Quit_STR},
  {MSG_Menu_Prefs,(STRPTR)MSG_Menu_Prefs_STR},
  {MSG_Menu_Edit,(STRPTR)MSG_Menu_Edit_STR},
  {MSG_Menu_Save,(STRPTR)MSG_Menu_Save_STR},
  {MSG_Menu_Use,(STRPTR)MSG_Menu_Use_STR},
  {MSG_Menu_Restore,(STRPTR)MSG_Menu_Restore_STR},
  {MSG_Menu_LastSaved,(STRPTR)MSG_Menu_LastSaved_STR},
  {MSG_Menu_MUI,(STRPTR)MSG_Menu_MUI_STR},
  {MSG_Window_Title,(STRPTR)MSG_Window_Title_STR},
  {MSG_Button_Show,(STRPTR)MSG_Button_Show_STR},
  {MSG_Button_Show_Help,(STRPTR)MSG_Button_Show_Help_STR},
  {MSG_Button_Hide,(STRPTR)MSG_Button_Hide_STR},
  {MSG_Button_Hide_Help,(STRPTR)MSG_Button_Hide_Help_STR},
  {MSG_Button_Quit,(STRPTR)MSG_Button_Quit_STR},
  {MSG_Button_Quit_Help,(STRPTR)MSG_Button_Quit_Help_STR},
  {MSG_IFGroup_Help,(STRPTR)MSG_IFGroup_Help_STR},
  {MSG_IFGroup_CItem_Online,(STRPTR)MSG_IFGroup_CItem_Online_STR},
  {MSG_IFGroup_CItem_Offline,(STRPTR)MSG_IFGroup_CItem_Offline_STR},
  {MSG_IFGroup_CItem_Scale,(STRPTR)MSG_IFGroup_CItem_Scale_STR},
  {MSG_IFGroup_CItem_ScaleFmt,(STRPTR)MSG_IFGroup_CItem_ScaleFmt_STR},
  {MSG_IF_Title_Name,(STRPTR)MSG_IF_Title_Name_STR},
  {MSG_IF_Title_State,(STRPTR)MSG_IF_Title_State_STR},
  {MSG_IF_Title_OnTime,(STRPTR)MSG_IF_Title_OnTime_STR},
  {MSG_IF_Title_Rate,(STRPTR)MSG_IF_Title_Rate_STR},
  {MSG_IF_Title_Speed,(STRPTR)MSG_IF_Title_Speed_STR},
  {MSG_IF_Title_Traffic,(STRPTR)MSG_IF_Title_Traffic_STR},
  {MSG_IF_Button_Online,(STRPTR)MSG_IF_Button_Online_STR},
  {MSG_IF_Button_Offline,(STRPTR)MSG_IF_Button_Offline_STR},
  {MSG_IF_Status_GoingOnline,(STRPTR)MSG_IF_Status_GoingOnline_STR},
  {MSG_IF_Status_GoingOffline,(STRPTR)MSG_IF_Status_GoingOffline_STR},
  {MSG_IF_Status_Suspending,(STRPTR)MSG_IF_Status_Suspending_STR},
  {MSG_IF_Status_Online,(STRPTR)MSG_IF_Status_Online_STR},
  {MSG_IF_Status_Offline,(STRPTR)MSG_IF_Status_Offline_STR},
  {MSG_IF_Status_Suspended,(STRPTR)MSG_IF_Status_Suspended_STR},
  {MSG_Traffic_Byte,(STRPTR)MSG_Traffic_Byte_STR},
  {MSG_Traffic_Bytes,(STRPTR)MSG_Traffic_Bytes_STR},
  {MSG_Traffic_KB,(STRPTR)MSG_Traffic_KB_STR},
  {MSG_Traffic_MB,(STRPTR)MSG_Traffic_MB_STR},
  {MSG_Rate_Bs,(STRPTR)MSG_Rate_Bs_STR},
  {MSG_Rate_KBs,(STRPTR)MSG_Rate_KBs_STR},
  {MSG_Rate_MBs,(STRPTR)MSG_Rate_MBs_STR},
  {MSG_Prefs_WinTitle,(STRPTR)MSG_Prefs_WinTitle_STR},
  {MSG_Prefs_Interfaces,(STRPTR)MSG_Prefs_Interfaces_STR},
  {MSG_Prefs_MiamiCtrls,(STRPTR)MSG_Prefs_MiamiCtrls_STR},
  {MSG_Prefs_Options,(STRPTR)MSG_Prefs_Options_STR},
  {MSG_Prefs_IF_Traffic_Format,(STRPTR)MSG_Prefs_IF_Traffic_Format_STR},
  {MSG_Prefs_IF_Traffic_Format_Help,(STRPTR)MSG_Prefs_IF_Traffic_Format_Help_STR},
  {MSG_Prefs_IF_Traffic_Format_Long,(STRPTR)MSG_Prefs_IF_Traffic_Format_Long_STR},
  {MSG_Prefs_IF_Traffic_Format_Short,(STRPTR)MSG_Prefs_IF_Traffic_Format_Short_STR},
  {MSG_Prefs_IF_Traffic_Grouping,(STRPTR)MSG_Prefs_IF_Traffic_Grouping_STR},
  {MSG_Prefs_IF_Traffic_Grouping_Help,(STRPTR)MSG_Prefs_IF_Traffic_Grouping_Help_STR},
  {MSG_Prefs_IF_Rate_Format,(STRPTR)MSG_Prefs_IF_Rate_Format_STR},
  {MSG_Prefs_IF_Rate_Format_Help,(STRPTR)MSG_Prefs_IF_Rate_Format_Help_STR},
  {MSG_Prefs_IF_Rate_Format_Long,(STRPTR)MSG_Prefs_IF_Rate_Format_Long_STR},
  {MSG_Prefs_IF_Rate_Format_Short,(STRPTR)MSG_Prefs_IF_Rate_Format_Short_STR},
  {MSG_Prefs_IF_Rate_Grouping,(STRPTR)MSG_Prefs_IF_Rate_Grouping_STR},
  {MSG_Prefs_IF_Rate_Grouping_Help,(STRPTR)MSG_Prefs_IF_Rate_Grouping_Help_STR},
  {MSG_Prefs_Miami_Position,(STRPTR)MSG_Prefs_Miami_Position_STR},
  {MSG_Prefs_Miami_Position_Help,(STRPTR)MSG_Prefs_Miami_Position_Help_STR},
  {MSG_Prefs_Miami_Position_Top,(STRPTR)MSG_Prefs_Miami_Position_Top_STR},
  {MSG_Prefs_Miami_Position_Bottom,(STRPTR)MSG_Prefs_Miami_Position_Bottom_STR},
  {MSG_Prefs_Miami_Position_Left,(STRPTR)MSG_Prefs_Miami_Position_Left_STR},
  {MSG_Prefs_Miami_Position_Right,(STRPTR)MSG_Prefs_Miami_Position_Right_STR},
  {MSG_Prefs_Miami_ViewMode,(STRPTR)MSG_Prefs_Miami_ViewMode_STR},
  {MSG_Prefs_Miami_ViewMode_Help,(STRPTR)MSG_Prefs_Miami_ViewMode_Help_STR},
  {MSG_Prefs_Miami_ViewMode_TextGfx,(STRPTR)MSG_Prefs_Miami_ViewMode_TextGfx_STR},
  {MSG_Prefs_Miami_ViewMode_Gfx,(STRPTR)MSG_Prefs_Miami_ViewMode_Gfx_STR},
  {MSG_Prefs_Miami_ViewMode_Text,(STRPTR)MSG_Prefs_Miami_ViewMode_Text_STR},
  {MSG_Prefs_Miami_Borderless,(STRPTR)MSG_Prefs_Miami_Borderless_STR},
  {MSG_Prefs_Miami_Borderless_Help,(STRPTR)MSG_Prefs_Miami_Borderless_Help_STR},
  {MSG_Prefs_Miami_Sunny,(STRPTR)MSG_Prefs_Miami_Sunny_STR},
  {MSG_Prefs_Miami_Sunny_Help,(STRPTR)MSG_Prefs_Miami_Sunny_Help_STR},
  {MSG_Prefs_Miami_Raised,(STRPTR)MSG_Prefs_Miami_Raised_STR},
  {MSG_Prefs_Miami_Raised_Help,(STRPTR)MSG_Prefs_Miami_Raised_Help_STR},
  {MSG_Prefs_Miami_Scaled,(STRPTR)MSG_Prefs_Miami_Scaled_STR},
  {MSG_Prefs_Miami_Scaled_Help,(STRPTR)MSG_Prefs_Miami_Scaled_Help_STR},
  {MSG_Prefs_Miami_LabelPosition,(STRPTR)MSG_Prefs_Miami_LabelPosition_STR},
  {MSG_Prefs_Miami_LabelPosition_Help,(STRPTR)MSG_Prefs_Miami_LabelPosition_Help_STR},
  {MSG_Prefs_Miami_LabelPosition_Bottom,(STRPTR)MSG_Prefs_Miami_LabelPosition_Bottom_STR},
  {MSG_Prefs_Miami_LabelPosition_Top,(STRPTR)MSG_Prefs_Miami_LabelPosition_Top_STR},
  {MSG_Prefs_Miami_LabelPosition_Right,(STRPTR)MSG_Prefs_Miami_LabelPosition_Right_STR},
  {MSG_Prefs_Miami_LabelPosition_Left,(STRPTR)MSG_Prefs_Miami_LabelPosition_Left_STR},
  {MSG_Prefs_Miami_BarLayout,(STRPTR)MSG_Prefs_Miami_BarLayout_STR},
  {MSG_Prefs_Miami_BarLayout_Help,(STRPTR)MSG_Prefs_Miami_BarLayout_Help_STR},
  {MSG_Prefs_Miami_BarLayout_Left,(STRPTR)MSG_Prefs_Miami_BarLayout_Left_STR},
  {MSG_Prefs_Miami_BarLayout_Center,(STRPTR)MSG_Prefs_Miami_BarLayout_Center_STR},
  {MSG_Prefs_Miami_BarLayout_Right,(STRPTR)MSG_Prefs_Miami_BarLayout_Right_STR},
  {MSG_Prefs_Miami_Underscore,(STRPTR)MSG_Prefs_Miami_Underscore_STR},
  {MSG_Prefs_Miami_Underscore_Help,(STRPTR)MSG_Prefs_Miami_Underscore_Help_STR},
  {MSG_Prefs_OnIconify,(STRPTR)MSG_Prefs_OnIconify_STR},
  {MSG_Prefs_OnIconify_Help,(STRPTR)MSG_Prefs_OnIconify_Help_STR},
  {MSG_Prefs_OI_Quit,(STRPTR)MSG_Prefs_OI_Quit_STR},
  {MSG_Prefs_OI_Hide,(STRPTR)MSG_Prefs_OI_Hide_STR},
  {MSG_Prefs_BWin,(STRPTR)MSG_Prefs_BWin_STR},
  {MSG_Prefs_BWin_Help,(STRPTR)MSG_Prefs_BWin_Help_STR},
  {MSG_Prefs_BWinBorders,(STRPTR)MSG_Prefs_BWinBorders_STR},
  {MSG_Prefs_BWinBorders_Help,(STRPTR)MSG_Prefs_BWinBorders_Help_STR},
  {MSG_Prefs_Save,(STRPTR)MSG_Prefs_Save_STR},
  {MSG_Prefs_Save_Help,(STRPTR)MSG_Prefs_Save_Help_STR},
  {MSG_Prefs_Use,(STRPTR)MSG_Prefs_Use_STR},
  {MSG_Prefs_Use_Help,(STRPTR)MSG_Prefs_Use_Help_STR},
  {MSG_Prefs_Apply,(STRPTR)MSG_Prefs_Apply_STR},
  {MSG_Prefs_Apply_Help,(STRPTR)MSG_Prefs_Apply_Help_STR},
  {MSG_Prefs_Test,(STRPTR)MSG_Prefs_Test_STR},
  {MSG_Prefs_Test_Help,(STRPTR)MSG_Prefs_Test_Help_STR},
  {MSG_Prefs_Cancel,(STRPTR)MSG_Prefs_Cancel_STR},
  {MSG_Prefs_Cancel_Help,(STRPTR)MSG_Prefs_Cancel_Help_STR},
  {MSG_Prefs_IF_UseBusyBar,(STRPTR)MSG_Prefs_IF_UseBusyBar_STR},
  {MSG_Prefs_IF_UseBusyBar_Help,(STRPTR)MSG_Prefs_IF_UseBusyBar_Help_STR},
  {MSG_Prefs_Miami_Frame,(STRPTR)MSG_Prefs_Miami_Frame_STR},
  {MSG_Prefs_Miami_Frame_Help,(STRPTR)MSG_Prefs_Miami_Frame_Help_STR},
  {MSG_Prefs_Miami_DragBar,(STRPTR)MSG_Prefs_Miami_DragBar_STR},
  {MSG_Prefs_Miami_DragBar_Help,(STRPTR)MSG_Prefs_Miami_DragBar_Help_STR},
  {MSG_Prefs_UseTransparency,(STRPTR)MSG_Prefs_UseTransparency_STR},
  {MSG_Prefs_UseTransparency_Help,(STRPTR)MSG_Prefs_UseTransparency_Help_STR},
  {MSG_About_WinTitle,(STRPTR)MSG_About_WinTitle_STR},
  {MSG_About_Information,(STRPTR)MSG_About_Information_STR},
  {MSG_About_Version,(STRPTR)MSG_About_Version_STR},
  {MSG_About_Author,(STRPTR)MSG_About_Author_STR},
  {MSG_About_Support,(STRPTR)MSG_About_Support_STR},
  {MSG_About_MiamiSite,(STRPTR)MSG_About_MiamiSite_STR},
  {MSG_About_ThirdParts,(STRPTR)MSG_About_ThirdParts_STR},
  {MSG_About_Lamp,(STRPTR)MSG_About_Lamp_STR},
  {MSG_About_Busy,(STRPTR)MSG_About_Busy_STR},
  {MSG_About_OfCourse,(STRPTR)MSG_About_OfCourse_STR},
  {MSG_About_Translation,(STRPTR)MSG_About_Translation_STR},
  {0,NULL}
};

#endif /* CATCOMP_ARRAY */

/***************************************************************/


#ifdef CATCOMP_BLOCK

//static const chat CatCompBlock[] =
//{
//     
//};

#endif /* CATCOMP_BLOCK */

/***************************************************************/

struct LocaleInfo
{
  APTR li_LocaleBase;
  APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
  LONG *l;
  UWORD *w;
  STRPTR  builtIn;

  l = (LONG *)CatCompBlock;

  while (*l != stringNum)
  {
    w = (UWORD *)((ULONG)l + 4);
    l = (LONG *)((ULONG)l + (LONG)*w + 6);
  }
  builtIn = (STRPTR)((ULONG)l + 6);

#define XLocaleBase LocaleBase
#define LocaleBase li->li_LocaleBase

  if (LocaleBase)
    return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#define LocaleBase XLocaleBase
#undef XLocaleBase

  return (builtIn);
}  

#endif /* CATCOMP_CODE */

/***************************************************************/


#endif
