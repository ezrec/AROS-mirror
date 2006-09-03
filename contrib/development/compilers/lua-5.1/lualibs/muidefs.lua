--[[
/******************************************************************************
* $Id$
*
* Copyright (C) 2006 Matthias Rustler.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
]]

-- class identifiers to use with mui.make
mui.MUIO_Label= 1 		--  STRPTR label, ULONG flags
mui.MUIO_Button= 2 		--  STRPTR label
mui.MUIO_Checkmark= 3 		--  STRPTR label
mui.MUIO_Cycle= 4 		--  STRPTR label, STRPTR *entries
mui.MUIO_Radio= 5 		--  STRPTR label, STRPTR *entries
mui.MUIO_Slider= 6 		--  STRPTR label, LONG min, LONG max
mui.MUIO_String= 7 		--  STRPTR label, LONG maxlen
mui.MUIO_PopButton= 8 		--  STRPTR imagespec
mui.MUIO_HSpace= 9 		--  LONG space
mui.MUIO_VSpace= 10 		--  LONG space
mui.MUIO_HBar= 11 		--  LONG space
mui.MUIO_VBar= 12 		--  LONG space
mui.MUIO_MenustripNM= 13 		--  struct NewMenu *nm, ULONG flags
mui.MUIO_Menuitem= 14 		--  STRPTR label, STRPTR shortcut, ULONG flags, ULONG data
mui.MUIO_BarTitle= 15 		--  STRPTR label
mui.MUIO_NumericButton= 16 		--  STRPTR label, LONG min, LONG max, STRPTR format
mui.MUIO_CoolButton= 111 		--  STRPTR label, APTR CoolImage, ULONG flags
mui.MUIO_ImageButton= 112 		--  CONST_STRPTR label, CONST_STRPTR imagePath

mui.MUIO_Menuitem_CopyStrings= 1073741824
mui.MUIO_Label_SingleFrame= 256
mui.MUIO_Label_DoubleFrame= 512
mui.MUIO_Label_LeftAligned= 1024
mui.MUIO_Label_Centered= 2048
mui.MUIO_Label_FreeVert= 4096
mui.MUIO_MenustripNM_CommandKeyCheck= 1 		--  check for 'localized' menu items such as 'O
mui.MUIO_CoolButton_CoolImageID= 1

-- ==================================================================
-- BOOPSI methods
mui.OM_ADDMEMBER= 265 
mui.OM_REMMEMBER= 266 

-- ==================================================================
mui.MUIC_Aboutmui= "Aboutmui.mui"

mui.MUIA_Aboutmui_Application= -2143148765 		--  V11 i.. Object *

-- ==================================================================
mui.MUIC_Application= "Application.mui"

mui.MUIM_Application_AboutMUI= -2143104483 		--  MUI: V14
mui.MUIM_Application_AddInputHandler= -2143096679 		--  MUI: V11
mui.MUIM_Application_CheckRefresh= -2143138456 		--  MUI: V11
mui.MUIM_Application_GetMenuCheck= -2143108953 		--  MUI: V4
mui.MUIM_Application_GetMenuState= -2143115889 		--  MUI: V4
mui.MUIM_Application_Input= -2143104779 		--  MUI: V4
mui.MUIM_Application_InputBuffered= -2143125927 		--  MUI: V4
mui.MUIM_Application_Load= -2143094515 		--  MUI: V4
mui.MUIM_Application_NewInput= -2143143002 		--  MUI: V11
mui.MUIM_Application_OpenConfigWindow= -2143118918 		--  MUI: V11
mui.MUIM_Application_PushMethod= -2143117576 		--  MUI: V4
mui.MUIM_Application_RemInputHandler= -2143098961 		--  MUI: V11
mui.MUIM_Application_ReturnID= -2143127825 		--  MUI: V4
mui.MUIM_Application_Save= -2143148049 		--  MUI: V4
mui.MUIM_Application_SetConfigItem= -2143139200 		--  MUI: V11
mui.MUIM_Application_SetMenuCheck= -2143115513 		--  MUI: V4
mui.MUIM_Application_SetMenuState= -2143122449 		--  MUI: V4
mui.MUIM_Application_ShowHelp= -2143132551 		--  MUI: V4
mui.MUIM_Application_SetConfigdata= -1874722560 		--  Zune 20030407
mui.MUIM_Application_OpenWindows= -1874722559 		--  Zune 20030407
mui.MUIM_Application_Iconify= -1874722558 		--  Zune: V1
mui.MUIM_Application_Execute= -1874722557

mui.MUIA_Application_Active= -2143133525 		--  MUI: V4  isg BOOL
mui.MUIA_Application_Author= -2143139774 		--  MUI: V4  i.g STRPTR
mui.MUIA_Application_Base= -2143100806 		--  MUI: V4  i.g STRPTR
mui.MUIA_Application_Broker= -2143102002 		--  MUI: V4  ..g Broker *
mui.MUIA_Application_BrokerHook= -2143121589 		--  MUI: V4  isg struct Hook *
mui.MUIA_Application_BrokerPort= -2143100755 		--  MUI: V6  ..g struct MsgPort *
mui.MUIA_Application_BrokerPri= -2143106864 		--  MUI: V6  i.g LONG
mui.MUIA_Application_Commands= -2143123896 		--  MUI: V4  isg struct MUI_Command *
mui.MUIA_Application_Copyright= -2143097011 		--  MUI: V4  i.g STRPTR
mui.MUIA_Application_Description= -2143150138 		--  MUI: V4  i.g STRPTR
mui.MUIA_Application_DiskObject= -2143144501 		--  MUI: V4  isg struct DiskObject *
mui.MUIA_Application_DoubleStart= -2143142970 		--  MUI: V4  ..g BOOL
mui.MUIA_Application_DropObject= -2143153562 		--  MUI: V5  is. Object *
mui.MUIA_Application_ForceQuit= -2143135777 		--  MUI: V8  ..g BOOL
mui.MUIA_Application_HelpFile= -2143120396 		--  MUI: V8  isg STRPTR
mui.MUIA_Application_Iconified= -2143117185 		--  MUI: V4  .sg BOOL
mui.MUIA_Application_MenuAction= -2143123103 		--  MUI: V4  ..g ULONG
mui.MUIA_Application_MenuHelp= -2143136757 		--  MUI: V4  ..g ULONG
mui.MUIA_Application_Menustrip= -2143137063 		--  MUI: V8  i.. Object *
mui.MUIA_Application_RexxHook= -2143126462 		--  MUI: V7  isg struct Hook *
mui.MUIA_Application_RexxMsg= -2143093368 		--  MUI: V4  ..g struct RxMsg *
mui.MUIA_Application_RexxString= -2143103215 		--  MUI: V4  .s. STRPTR
mui.MUIA_Application_SingleTask= -2143116600 		--  MUI: V4  i.. BOOL
mui.MUIA_Application_Sleep= -2143135983 		--  MUI: V4  .s. BOOL
mui.MUIA_Application_Title= -2143125064 		--  MUI: V4  i.g STRPTR
mui.MUIA_Application_UseCommodities= -2143133979 		--  MUI: V10 i.. BOOL
mui.MUIA_Application_UsedClasses= -2143098457 		--  MUI undoc: V20 i.. STRPTR []
mui.MUIA_Application_UseRexx= -2143149177 		--  MUI: V10 i.. BOOL
mui.MUIA_Application_Version= -2143112385 		--  [I-G] CONST_STRPTR Example: $VER: Program 1.3 (14.11.03)
mui.MUIA_Application_Window= -2143109152 		--  MUI: V4  i.. Object *
mui.MUIA_Application_WindowList= -2143118658 		--  MUI: V13 ..g struct List *
mui.MUIA_Application_Configdata= -1874722560 		--  Zune 20030407 .s. Object *
mui.MUIA_Application_Version_Number= -1874722559 		-- [I-G] CONST_STRPTR Examples: 1.5, 2.37.4b
mui.MUIA_Application_Version_Date= -1874722558 		-- [I-G] CONST_STRPTR YYYY-MM-DD
mui.MUIA_Application_Version_Extra= -1874722557 		-- [I-G] CONST_STRPTR Example: nightly build

mui.MUIV_Application_Save_ENV= 0
mui.MUIV_Application_Save_ENVARC= -1
mui.MUIV_Application_Load_ENV= 0
mui.MUIV_Application_Load_ENVARC= -1
mui.MUIV_Application_ReturnID_Quit= -1

-- ==================================================================
mui.MUIC_Area= "Area.mui"

mui.MUIM_AskMinMax= -2143143820 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_Cleanup= -2143102587 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_ContextMenuBuild= -2143118034 		--  MUI: V11
mui.MUIM_ContextMenuChoice= -2143154418 		--  MUI: V11
mui.MUIM_CreateBubble= -2143151039 		--  MUI: V18
mui.MUIM_CreateDragImage= -2143098001 		--  MUI: V18 */ /* For Custom Classes only */ /* Undoc
mui.MUIM_CreateShortHelp= -2143121773 		--  MUI: V11
mui.MUIM_CustomBackfill= -2143122061 		--  Undoc
mui.MUIM_DeleteBubble= -2143153745 		--  MUI: V18
mui.MUIM_DeleteDragImage= -2143145929 		--  MUI: V18 */ /* For Custom Classes only */ /* Undoc
mui.MUIM_DeleteShortHelp= -2143104166 		--  MUI: V11
mui.MUIM_DoDrag= -2143152453 		--  MUI: V18 */ /* For Custom Classes only */ /* Undoc
mui.MUIM_DragBegin= -2143109062 		--  MUI: V11
mui.MUIM_DragDrop= -2143107755 		--  MUI: V11
mui.MUIM_DragFinish= -2143137296 		--  MUI: V11
mui.MUIM_DragQuery= -2143157663 		--  MUI: V11
mui.MUIM_DragReport= -2143097427 		--  MUI: V11
mui.MUIM_Draw= -2143129793 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_DrawBackground= -2143143734 		--  MUI: V11
mui.MUIM_GoActive= -2143139558 		--  Undoc
mui.MUIM_GoInactive= -2143146996 		--  Undoc
mui.MUIM_HandleEvent= -2143130266 		--  MUI: V16 */ /* For Custom Classes only
mui.MUIM_HandleInput= -2143147494 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_Hide= -2143096305 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_Setup= -2143124652 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_Show= -2143105916 		--  MUI: V4  */ /* For Custom Classes only
mui.MUIM_Layout= -1874722304
mui.MUIM_DrawParentBackground= -1874722303

mui.MUIA_Background= -2143136677 		--  MUI: V4  is. LONG
mui.MUIA_BottomEdge= -2143099566 		--  MUI: V4  ..g LONG
mui.MUIA_ContextMenu= -2143111420 		--  MUI: V11 isg Object *
mui.MUIA_ContextMenuTrigger= -2143116607 		--  MUI: V11 ..g Object *
mui.MUIA_ControlChar= -2143153653 		--  MUI: V4  isg char
mui.MUIA_CustomBackfill= -2143155613 		--  undoc    i..
mui.MUIA_CycleChain= -2143150873 		--  MUI: V11 isg LONG
mui.MUIA_Disabled= -2143144351 		--  MUI: V4  isg BOOL
mui.MUIA_Draggable= -2143155346 		--  MUI: V11 isg BOOL
mui.MUIA_Dropable= -2143093810 		--  MUI: V11 isg BOOL
mui.MUIA_FillArea= -2143120221 		--  MUI: V4  is. BOOL
mui.MUIA_FixHeight= -2143114965 		--  MUI: V4  i.. LONG
mui.MUIA_FixHeightTxt= -2143127822 		--  MUI: V4  i.. STRPTR
mui.MUIA_FixWidth= -2143116303 		--  MUI: V4  i.. LONG
mui.MUIA_FixWidthTxt= -2143104956 		--  MUI: V4  i.. STRPTR
mui.MUIA_Font= -2143109552 		--  MUI: V4  i.g struct TextFont *
mui.MUIA_Frame= -2143114140 		--  MUI: V4  i.. LONG
mui.MUIA_FramePhantomHoriz= -2143097482 		--  MUI: V4  i.. BOOL
mui.MUIA_FrameTitle= -2143104569 		--  MUI: V4  i.. STRPTR
mui.MUIA_Height= -2143145417 		--  MUI: V4  ..g LONG
mui.MUIA_HorizDisappear= -2143119851 		--  MUI: V11 isg LONG
mui.MUIA_HorizWeight= -2143130183 		--  MUI: V4  isg WORD
mui.MUIA_InnerBottom= -2143096128 		--  MUI: V4  i.g LONG
mui.MUIA_InnerLeft= -2143147784 		--  MUI: V4  i.g LONG
mui.MUIA_InnerRight= -2143119361 		--  MUI: V4  i.g LONG
mui.MUIA_InnerTop= -2143150410 		--  MUI: V4  i.g LONG
mui.MUIA_InputMode= -2143094012 		--  MUI: V4  i.. LONG
mui.MUIA_LeftEdge= -2143109434 		--  MUI: V4  ..g LONG
mui.MUIA_MaxHeight= -2143120412 		--  MUI: V11 i.. LONG
mui.MUIA_MaxWidth= -2143096558 		--  MUI: V11 i.. LONG
mui.MUIA_Pressed= -2143144651 		--  MUI: V4  ..g BOOL
mui.MUIA_RightEdge= -2143110526 		--  MUI: V4  ..g LONG
mui.MUIA_Selected= -2143132341 		--  MUI: V4  isg BOOL
mui.MUIA_ShortHelp= -2143121437 		--  MUI: V11 isg STRPTR
mui.MUIA_ShowMe= -2143118424 		--  MUI: V4  isg BOOL
mui.MUIA_ShowSelState= -2143106388 		--  MUI: V4  i.. BOOL
mui.MUIA_Timer= -2143132619 		--  MUI: V4  ..g LONG
mui.MUIA_TopEdge= -2143137637 		--  MUI: V4  ..g LONG
mui.MUIA_VertDisappear= -2143104721 		--  MUI: V11 isg LONG
mui.MUIA_VertWeight= -2143119152 		--  MUI: V4  isg WORD
mui.MUIA_Weight= -2143150817 		--  MUI: V4  i.. WORD
mui.MUIA_Width= -2143111780 		--  MUI: V4  ..g LONG
mui.MUIA_Window= -2143152751 		--  MUI: V4  ..g struct Window *
mui.MUIA_WindowObject= -2143132002 		--  MUI: V4  ..g Object *
mui.MUIA_NestedDisabled= -1874722304 		--  Zune 20030530  isg BOOL

mui.MUIV_Frame_None= 0
mui.MUIV_Frame_Button= 1
mui.MUIV_Frame_ImageButton= 2
mui.MUIV_Frame_Text= 3
mui.MUIV_Frame_String= 4
mui.MUIV_Frame_ReadList= 5
mui.MUIV_Frame_InputList= 6
mui.MUIV_Frame_Prop= 7
mui.MUIV_Frame_Gauge= 8
mui.MUIV_Frame_Group= 9
mui.MUIV_Frame_PopUp= 10
mui.MUIV_Frame_Virtual= 11
mui.MUIV_Frame_Slider= 12
mui.MUIV_Frame_Knob= 13
mui.MUIV_Frame_Drag= 14
mui.MUIV_Frame_Count= 15
mui.MUIV_InputMode_None= 0
mui.MUIV_InputMode_RelVerify= 1
mui.MUIV_InputMode_Immediate= 2
mui.MUIV_InputMode_Toggle= 3
mui.MUIV_DragQuery_Refuse= 0
mui.MUIV_DragQuery_Accept= 1
mui.MUIV_DragReport_Abort= 0
mui.MUIV_DragReport_Continue= 1
mui.MUIV_DragReport_Lock= 2
mui.MUIV_DragReport_Refresh= 3
mui.MUIV_CreateBubble_DontHidePointer= 1

-- ==================================================================
mui.MUIC_Balance= "Balance.mui"

mui.MUIA_Balance_Quiet= -1874722048 		--  (zune) V20 i   LONG

-- ==================================================================
mui.MUIC_Bitmap= "Bitmap.mui"

mui.MUIA_Bitmap_Bitmap= -2143127107 		--  MUI: V8  isg struct BitMap *
mui.MUIA_Bitmap_Height= -2143152800 		--  MUI: V8  isg LONG
mui.MUIA_Bitmap_MappingTable= -2143100355 		--  MUI: V8  isg UBYTE *
mui.MUIA_Bitmap_Precision= -2143155084 		--  MUI: V11 isg LONG
mui.MUIA_Bitmap_RemappedBitmap= -2143143353 		--  MUI: V11 ..g struct BitMap *
mui.MUIA_Bitmap_SourceColors= -2143136928 		--  MUI: V8  isg ULONG *
mui.MUIA_Bitmap_Transparent= -2143148027 		--  MUI: V8  isg LONG
mui.MUIA_Bitmap_UseFriend= -2143143464 		--  MUI: V11 i.. BOOL
mui.MUIA_Bitmap_Width= -2143098054 		--  MUI: V8  isg LONG

-- ==================================================================
mui.MUIC_Bodychunk= "Bodychunk.mui"

mui.MUIA_Bodychunk_Body= -2143106457 		--  V8  isg UBYTE *
mui.MUIA_Bodychunk_Compression= -2143101345 		--  V8  isg UBYTE
mui.MUIA_Bodychunk_Depth= -2143108206 		--  V8  isg LONG
mui.MUIA_Bodychunk_Masking= -2143143154 		--  V8  isg UBYTE

-- ==================================================================
mui.MUIC_Boopsi= "Boopsi.mui"

mui.MUIA_Boopsi_Class= -2143131239 		--  V4  isg struct IClass *
mui.MUIA_Boopsi_ClassID= -2143109213 		--  V4  isg char *
mui.MUIA_Boopsi_MaxHeight= -2143128193 		--  V4  isg ULONG
mui.MUIA_Boopsi_MaxWidth= -2143109967 		--  V4  isg ULONG
mui.MUIA_Boopsi_MinHeight= -2143146861 		--  V4  isg ULONG
mui.MUIA_Boopsi_MinWidth= -2143121486 		--  V4  isg ULONG
mui.MUIA_Boopsi_Object= -2143157896 		--  V4  ..g Object *
mui.MUIA_Boopsi_Remember= -2143095619 		--  V4  i.. ULONG
mui.MUIA_Boopsi_Smart= -2143110953 		--  V9  i.. BOOL
mui.MUIA_Boopsi_TagDrawInfo= -2143110425 		--  V4  isg ULONG
mui.MUIA_Boopsi_TagScreen= -2143110031 		--  V4  isg ULONG
mui.MUIA_Boopsi_TagWindow= -2143100643 		--  V4  isg ULONG

-- ==================================================================
mui.MUIC_ChunkyImage= "ChunkyImage.mui"

mui.MUIA_ChunkyImage_Pixels= -1874706432 		--  V8  isg UBYTE *
mui.MUIA_ChunkyImage_Palette= -1874706431 		--  V8  isg UBYTE *
mui.MUIA_ChunkyImage_NumColors= -1874706430 		--  V8  isg LONG
mui.MUIA_ChunkyImage_Modulo= -1874706429 		--  V8  isg LONG

-- ==================================================================
mui.MUIC_Coloradjust= "Coloradjust.mui"

mui.MUIA_Coloradjust_Red= -2143154518 		--  isg ULONG
mui.MUIA_Coloradjust_Green= -2143124053 		--  isg ULONG
mui.MUIA_Coloradjust_Blue= -2143111005 		--  isg ULONG
mui.MUIA_Coloradjust_RGB= -2143094631 		--  isg ULONG *
mui.MUIA_Coloradjust_ModeID= -2143097767 		--  isg ULONG

-- ==================================================================
mui.MUIC_Colorfield= "Colorfield.mui"

mui.MUIA_Colorfield_Pen= -2143129286 		--  ..g  ULONG
mui.MUIA_Colorfield_Red= -2143127050 		--  isg  ULONG
mui.MUIA_Colorfield_Green= -2143140762 		--  isg  ULONG
mui.MUIA_Colorfield_Blue= -2143104080 		--  isg  ULONG
mui.MUIA_Colorfield_RGB= -2143131782 		--  isg  ULONG *

-- ==================================================================
mui.MUIC_Crawling= "Crawling.mui"

-- ==================================================================
mui.MUIC_Cycle= "Cycle.mui"

mui.MUIA_Cycle_Active= -2143152248 		--  MUI:V4  isg LONG
mui.MUIA_Cycle_Entries= -2143156695 		--  MUI:V4  i.. STRPTR

mui.MUIV_Cycle_Active_Next= -1
mui.MUIV_Cycle_Active_Prev= -2

-- ==================================================================
mui.MUIC_Dataspace= "Dataspace.mui"

mui.MUIM_Dataspace_Add= -2143145114 		--  MUI: V11
mui.MUIM_Dataspace_Clear= -2143111479 		--  MUI: V11
mui.MUIM_Dataspace_Find= -2143124692 		--  MUI: V11
mui.MUIM_Dataspace_Merge= -2143142357 		--  MUI: V11
mui.MUIM_Dataspace_ReadIFF= -2143154693 		--  MUI: V11
mui.MUIM_Dataspace_Remove= -2143101727 		--  MUI: V11
mui.MUIM_Dataspace_WriteIFF= -2143134066 		--  MUI: V11

mui.MUIA_Dataspace_Pool= -2143138567 		--  MUI: V11 i.. APTR

-- ==================================================================
mui.MUIC_Dirlist= "Dirlist.mui"

mui.MUIM_Dirlist_ReRead= -2143146639 		--  MUI: V4

mui.MUIA_Dirlist_AcceptPattern= -2143128054 		--  MUI: V4  is. STRPTR
mui.MUIA_Dirlist_Directory= -2143098303 		--  MUI: V4  isg STRPTR
mui.MUIA_Dirlist_DrawersOnly= -2143112327 		--  MUI: V4  is. BOOL
mui.MUIA_Dirlist_FilesOnly= -2143123094 		--  MUI: V4  is. BOOL
mui.MUIA_Dirlist_FilterDrawers= -2143139118 		--  MUI: V4  is. BOOL
mui.MUIA_Dirlist_FilterHook= -2143113703 		--  MUI: V4  is. struct Hook *
mui.MUIA_Dirlist_MultiSelDirs= -2143123885 		--  MUI: V6  is. BOOL
mui.MUIA_Dirlist_NumBytes= -2143117786 		--  MUI: V4  ..g LONG
mui.MUIA_Dirlist_NumDrawers= -2143118152 		--  MUI: V4  ..g LONG
mui.MUIA_Dirlist_NumFiles= -2143115536 		--  MUI: V4  ..g LONG
mui.MUIA_Dirlist_Path= -2143133322 		--  MUI: V4  ..g STRPTR
mui.MUIA_Dirlist_RejectIcons= -2143139832 		--  MUI: V4  is. BOOL
mui.MUIA_Dirlist_RejectPattern= -2143135289 		--  MUI: V4  is. STRPTR
mui.MUIA_Dirlist_SortDirs= -2143110215 		--  MUI: V4  is. LONG
mui.MUIA_Dirlist_SortHighLow= -2143151978 		--  MUI: V4  is. BOOL
mui.MUIA_Dirlist_SortType= -2143147844 		--  MUI: V4  is. LONG
mui.MUIA_Dirlist_Status= -2143141666 		--  MUI: V4  ..g LONG

mui.MUIV_Dirlist_SortDirs_First= 0
mui.MUIV_Dirlist_SortDirs_Last= 1
mui.MUIV_Dirlist_SortDirs_Mix= 2
mui.MUIV_Dirlist_SortType_Name= 0
mui.MUIV_Dirlist_SortType_Date= 1
mui.MUIV_Dirlist_SortType_Size= 2
mui.MUIV_Dirlist_Status_Invalid= 0
mui.MUIV_Dirlist_Status_Reading= 1
mui.MUIV_Dirlist_Status_Valid= 2

-- ==================================================================
mui.MUIC_Dtpic= "Dtpic.mui"

mui.MUIA_Dtpic_Name= -2143142542 		--  i.. STRPTR

-- ==================================================================
mui.MUIC_Family= "Family.mui"

mui.MUIM_Family_AddHead= -2143100416 		--  MUI: V8
mui.MUIM_Family_AddTail= -2143103150 		--  MUI: V8
mui.MUIM_Family_Insert= -2143138508 		--  MUI: V8
mui.MUIM_Family_Remove= -2143094615 		--  MUI: V8
mui.MUIM_Family_Sort= -2143151031 		--  MUI: V8
mui.MUIM_Family_Transfer= -2143108790 		--  MUI: V8

mui.MUIA_Family_Child= -2143107434 		--  MUI: V8  i.. Object *
mui.MUIA_Family_List= -2143138914 		--  MUI: V8  ..g struct MinList *

-- ==================================================================
mui.MUIC_Floattext= "Floattext.mui"

mui.MUIA_Floattext_Justify= -2143101949 		--  MUI: V4  isg BOOL
mui.MUIA_Floattext_SkipChars= -2143134595 		--  MUI: V4  is. STRPTR
mui.MUIA_Floattext_TabSize= -2143126249 		--  MUI: V4  is. LONG
mui.MUIA_Floattext_Text= -2143104662 		--  MUI: V4  isg STRPTR

-- ==================================================================
mui.MUIC_Frameadjust= "Frameadjust.mui"

mui.MUIA_Frameadjust_Spec= -1874719488 		--  Zune 20030330 ig. CONST_STRPTR

-- ==================================================================
mui.MUIC_Framedisplay= "Framedisplay.mui"

mui.MUIA_Framedisplay_Spec= -2143152236 		--  MUI: V??  isg struct MUI_FrameSpec  *

-- ==================================================================
mui.MUIC_Gauge= "Gauge.mui"

mui.MUIA_Gauge_Current= -2143096611 		--  MUI: V4  isg LONG
mui.MUIA_Gauge_Divide= -2143102753 		--  MUI: V4  isg LONG
mui.MUIA_Gauge_Horiz= -2143145251 		--  MUI: V4  i.. BOOL
mui.MUIA_Gauge_InfoText= -2143109355 		--  MUI: V7  isg STRPTR
mui.MUIA_Gauge_Max= -2143109925 		--  MUI: V4  isg LONG
mui.MUIA_Gauge_DupInfoText= -1874718976 		--  ZUNE: V1  i.. BOOL - defaults to FALSE

-- ==================================================================
mui.MUIC_Group= "Group.mui"

mui.MUIM_Group_ExitChange= -2143104564 		--  MUI: V11
mui.MUIM_Group_InitChange= -2143156089 		--  MUI: V11
mui.MUIM_Group_Sort= -2143128553 		--  MUI: V4
mui.MUIM_Group_DoMethodNoForward= -1874718720

mui.MUIA_Group_ActivePage= -2143141479 		--  MUI: V5  isg LONG
mui.MUIA_Group_Child= -2143148314 		--  MUI: V4  i.. Object *
mui.MUIA_Group_ChildList= -2143140024 		--  MUI: V4  ..g struct List *
mui.MUIA_Group_Columns= -2143095786 		--  MUI: V4  is. LONG
mui.MUIA_Group_Forward= -2143153118 		--  MUI: V11 .s. BOOL
mui.MUIA_Group_Horiz= -2143136917 		--  MUI: V4  i.. BOOL
mui.MUIA_Group_HorizSpacing= -2143107503 		--  MUI: V4  isg LONG
mui.MUIA_Group_LayoutHook= -2143108174 		--  MUI: V11 i.. struct Hook *
mui.MUIA_Group_PageMode= -2143151521 		--  MUI: V5  i.. BOOL
mui.MUIA_Group_Rows= -2143111537 		--  MUI: V4  is. LONG
mui.MUIA_Group_SameHeight= -2143157378 		--  MUI: V4  i.. BOOL
mui.MUIA_Group_SameSize= -2143156128 		--  MUI: V4  i.. BOOL
mui.MUIA_Group_SameWidth= -2143112212 		--  MUI: V4  i.. BOOL
mui.MUIA_Group_Spacing= -2143123859 		--  MUI: V4  is. LONG
mui.MUIA_Group_VertSpacing= -2143100481 		--  MUI: V4  isg LONG
mui.MUIA_Group_Virtual= -1874718720 		--  Zune: V1 i.. BOOL

mui.MUIV_Group_ActivePage_First= 0
mui.MUIV_Group_ActivePage_Last= -1
mui.MUIV_Group_ActivePage_Prev= -2
mui.MUIV_Group_ActivePage_Next= -3
mui.MUIV_Group_ActivePage_Advance= -4

-- ==================================================================
mui.MUIC_IconDrawerList= "IconDrawerList.mui"

mui.MUIA_IconDrawerList_Drawer= -1874705664 		--  Zune: V1  isg LONG

-- ==================================================================
mui.MUIC_IconList= "IconList.mui"

mui.MUIM_IconList_Clear= -1874705920 		--  Zune: V1
mui.MUIM_IconList_Update= -1874705919 		--  Zune: V1
mui.MUIM_IconList_Add= -1874705918 		--  Zune: V1 returns BOOL
mui.MUIM_IconList_NextSelected= -1874705917 		--  Zune: V1
mui.MUIM_IconList_UnselectAll= -1874705916 		--  Zune: V1
mui.MUIM_IconList_Sort= -1874705915 		--  Zune: V1
mui.MUIM_IconList_GetSortBits= -1874705914 		--  Zune: V1
mui.MUIM_IconList_SetSortBits= -1874705913 		--  Zune: V1
mui.MUIM_IconList_PositionIcons= -1874705912 		--  Zune: V1

mui.MUIV_IconList_NextSelected_Start= 0
mui.MUIV_IconList_NextSelected_End= 0
mui.MUIA_IconList_DoubleClick= -1874705920 		--  Zune: V1 ..G BOOL
mui.MUIA_IconList_Left= -1874705919 		--  Zune: V1 ..G LONG
mui.MUIA_IconList_Top= -1874705918 		--  Zune: V1 ..G LONG
mui.MUIA_IconList_Width= -1874705917 		--  Zune: V1 ..G LONG
mui.MUIA_IconList_Height= -1874705916 		--  Zune: V1 ..G LONG
mui.MUIA_IconList_IconsDropped= -1874705915 		--  Zune: V1 ..G struct IconList_Entry *
mui.MUIA_IconList_Clicked= -1874705914 		--  Zune: V1 ..G struct IconList_Click *

mui.ICONLIST_SORT_DRAWERS_MIXED= 1 		-- mix folders and files when sorting
mui.ICONLIST_SORT_DRAWERS_LAST= 2 		-- ignored if mixed is set
mui.ICONLIST_SORT_REVERSE= 4 		-- reverse sort direction
mui.ICONLIST_SORT_BY_DATE= 8 		-- both date and size = sort by type
mui.ICONLIST_SORT_BY_SIZE= 16 		-- neither = sort by name
mui.ICONLIST_SHOW_HIDDEN= 32 		-- needs to be globally overridable
mui.ICONLIST_SHOW_INFO= 64 		-- show icon *.info files
mui.ICONLIST_DISP_VERTICAL= 128 		-- tile icons vertically
mui.ICONLIST_DISP_NOICONS= 256 		-- name only mode
mui.ICONLIST_DISP_DETAILS= 512 		-- name=details mode, icon=??

-- ==================================================================
mui.MUIC_IconListview= "IconListview.mui"

mui.MUIA_IconListview_IconList= -1874705152 		--  Zune: V1  i.g Object *
mui.MUIA_IconListview_UseWinBorder= -1874705151 		--  Zune: V1  i.. BOOL

-- ==================================================================
mui.MUIC_IconVolumeList= "IconVolumneList.mui"

-- ==================================================================
mui.MUIC_Image= "Image.mui"

mui.MUIA_Image_FontMatch= -2143125155 		--  MUI: V4  i.. BOOL
mui.MUIA_Image_FontMatchHeight= -2143117530 		--  MUI: V4  i.. BOOL
mui.MUIA_Image_FontMatchWidth= -2143143489 		--  MUI: V4  i.. BOOL
mui.MUIA_Image_FreeHoriz= -2143102332 		--  MUI: V4  i.. BOOL
mui.MUIA_Image_FreeVert= -2143098328 		--  MUI: V4  i.. BOOL
mui.MUIA_Image_OldImage= -2143137987 		--  MUI: V4  i.. struct Image *
mui.MUIA_Image_Spec= -2143145003 		--  MUI: V4  i.. char *
mui.MUIA_Image_State= -2143116371 		--  MUI: V4  is. LONG

-- ==================================================================
mui.MUIC_Imageadjust= "Imageadjust.mui"

mui.MUIA_Imageadjust_Type= -2143146197 		--  MUI: V11 i.. LONG
mui.MUIA_Imageadjust_Spec= -2143127071 		--  MUI: ??? .g. char *
mui.MUIA_Imageadjust_Originator= -1874718464 		--  Zune: i.. Object *

mui.MUIV_Imageadjust_Type_All= 0
mui.MUIV_Imageadjust_Type_Image= 1
mui.MUIV_Imageadjust_Type_Background= 2
mui.MUIV_Imageadjust_Type_Pen= 3

-- ==================================================================
mui.MUIC_Imagedisplay= "Imagedisplay.mui"

mui.MUIA_Imagedisplay_Spec= -2143115961 		--  MUI: V11 isg struct MUI_ImageSpec *
mui.MUIA_Imagedisplay_UseDefSize= -2143152019 		--  MUI: V11 i.. BOOL (undoc)
mui.MUIA_Imagedisplay_FreeHoriz= -1874718208 		--  Zune 20030323 i.. BOOL [TRUE]
mui.MUIA_Imagedisplay_FreeVert= -1874718207 		--  Zune 20030323 i.. BOOL [TRUE]

-- ==================================================================
mui.MUIC_Knob= "Knob.mui"

-- ==================================================================
mui.MUIC_Levelmeter= "Levelmeter.mui"

mui.MUIA_Levelmeter_Label= -2143154731 		--  V11 isg STRPTR

-- ==================================================================
mui.MUIC_Listview= "Listview.mui"

mui.MUIA_Listview_ClickColumn= -2143104589 		--  V7  ..g LONG
mui.MUIA_Listview_DefClickColumn= -2143112554 		--  V7  isg LONG
mui.MUIA_Listview_DoubleClick= -2143140299 		--  V4  i.g BOOL
mui.MUIA_Listview_DragType= -2143134509 		--  V11 isg LONG
mui.MUIA_Listview_Input= -2143131603 		--  V4  i.. BOOL
mui.MUIA_Listview_List= -2143109938 		--  V4  i.g Object
mui.MUIA_Listview_MultiSelect= -2143126008 		--  V7  i.. LONG
mui.MUIA_Listview_ScrollerPos= -2143112780 		--  V10 i.. BOOL
mui.MUIA_Listview_SelectChange= -2143152241 		--  V4  ..g BOOL

mui.MUIV_Listview_DragType_None= 0
mui.MUIV_Listview_DragType_Immediate= 1
mui.MUIV_Listview_MultiSelect_None= 0
mui.MUIV_Listview_MultiSelect_Default= 1
mui.MUIV_Listview_MultiSelect_Shifted= 2
mui.MUIV_Listview_MultiSelect_Always= 3
mui.MUIV_Listview_ScrollerPos_Default= 0
mui.MUIV_Listview_ScrollerPos_Left= 1
mui.MUIV_Listview_ScrollerPos_Right= 2
mui.MUIV_Listview_ScrollerPos_None= 3

-- ==================================================================
mui.MUIC_List= "List.mui"

mui.MUIM_List_Clear= -2143113847 		--  MUI: V4
mui.MUIM_List_CreateImage= -2143119356 		--  MUI: V11
mui.MUIM_List_DeleteImage= -2143154344 		--  MUI: V11
mui.MUIM_List_Exchange= -2143140212 		--  MUI: V4
mui.MUIM_List_GetEntry= -2143125268 		--  MUI: V4
mui.MUIM_List_Insert= -2143130489 		--  MUI: V4
mui.MUIM_List_InsertSingle= -2143136555 		--  MUI: V7
mui.MUIM_List_Jump= -2143110485 		--  MUI: V4
mui.MUIM_List_Move= -2143136830 		--  MUI: V9
mui.MUIM_List_NextSelected= -2143133929 		--  MUI: V6
mui.MUIM_List_Redraw= -2143127149 		--  MUI: V4
mui.MUIM_List_Remove= -2143132546 		--  MUI: V4
mui.MUIM_List_Select= -2143137064 		--  MUI: V4
mui.MUIM_List_Sort= -2143149451 		--  MUI: V4
mui.MUIM_List_TestPos= -2143133880 		--  MUI: V11
mui.MUIM_List_InsertSingleAsTree= -1874717691 		--  Zune: V1

mui.MUIA_List_Active= -2143143652 		--  MUI: V4  isg LONG
mui.MUIA_List_AdjustHeight= -2143124211 		--  MUI: V4  i.. BOOL
mui.MUIA_List_AdjustWidth= -2143144630 		--  MUI: V4  i.. BOOL
mui.MUIA_List_AutoVisible= -2143116219 		--  MUI: V11 isg BOOL
mui.MUIA_List_CompareHook= -2143134700 		--  MUI: V4  is. struct Hook *
mui.MUIA_List_ConstructHook= -2143123121 		--  MUI: V4  is. struct Hook *
mui.MUIA_List_DestructHook= -2143119410 		--  MUI: V4  is. struct Hook *
mui.MUIA_List_DisplayHook= -2143111979 		--  MUI: V4  is. struct Hook *
mui.MUIA_List_DragSortable= -2143133543 		--  MUI: V11 isg BOOL
mui.MUIA_List_DropMark= -2143114330 		--  MUI: V11 ..g LONG
mui.MUIA_List_Entries= -2143152556 		--  MUI: V4  ..g LONG
mui.MUIA_List_First= -2143143724 		--  MUI: V4  ..g LONG
mui.MUIA_List_Format= -2143142902 		--  MUI: V4  isg STRPTR
mui.MUIA_List_InsertPosition= -2143104819 		--  MUI: V9  ..g LONG
mui.MUIA_List_MinLineHeight= -2143104573 		--  MUI: V4  i.. LONG
mui.MUIA_List_MultiTestHook= -2143108410 		--  MUI: V4  is. struct Hook *
mui.MUIA_List_Pool= -2143144911 		--  MUI: V13 i.. APTR
mui.MUIA_List_PoolPuddleSize= -2143116053 		--  MUI: V13 i.. ULONG
mui.MUIA_List_PoolThreshSize= -2143107956 		--  MUI: V13 i.. ULONG
mui.MUIA_List_Quiet= -2143102777 		--  MUI: V4  .s. BOOL
mui.MUIA_List_ShowDropMarks= -2143107341 		--  MUI: V11 isg BOOL
mui.MUIA_List_SourceArray= -2143108960 		--  MUI: V4  i.. APTR
mui.MUIA_List_Title= -2143142298 		--  MUI: V6  isg char *
mui.MUIA_List_Visible= -2143151841 		--  MUI: V4  ..g LONG

mui.MUI_LPR_ABOVE= 1
mui.MUI_LPR_BELOW= 2
mui.MUI_LPR_LEFT= 4
mui.MUI_LPR_RIGHT= 8

mui.MUIV_List_Active_Off= -1
mui.MUIV_List_Active_Top= -2
mui.MUIV_List_Active_Bottom= -3
mui.MUIV_List_Active_Up= -4
mui.MUIV_List_Active_Down= -5
mui.MUIV_List_Active_PageUp= -6
mui.MUIV_List_Active_PageDown= -7
mui.MUIV_List_ConstructHook_String= -1
mui.MUIV_List_CopyHook_String= -1
mui.MUIV_List_CursorType_None= 0
mui.MUIV_List_CursorType_Bar= 1
mui.MUIV_List_CursorType_Rect= 2
mui.MUIV_List_DestructHook_String= -1
mui.MUIV_List_Insert_Top= 0
mui.MUIV_List_Insert_Active= -1
mui.MUIV_List_Insert_Sorted= -2
mui.MUIV_List_Insert_Bottom= -3
mui.MUIV_List_Remove_First= 0
mui.MUIV_List_Remove_Active= -1
mui.MUIV_List_Remove_Last= -2
mui.MUIV_List_Remove_Selected= -3
mui.MUIV_List_Select_Active= -1
mui.MUIV_List_Select_All= -2
mui.MUIV_List_Select_Off= 0
mui.MUIV_List_Select_On= 1
mui.MUIV_List_Select_Toggle= 2
mui.MUIV_List_Select_Ask= 3
mui.MUIV_List_GetEntry_Active= -1
mui.MUIV_List_Redraw_Active= -1
mui.MUIV_List_Redraw_All= -2
mui.MUIV_List_Move_Top= 0
mui.MUIV_List_Move_Active= -1
mui.MUIV_List_Move_Bottom= -2
mui.MUIV_List_Move_Next= -3 		--  for 2nd parameter only
mui.MUIV_List_Move_Previous= -4 		--  for 2nd parameter only
mui.MUIV_List_Exchange_Top= 0
mui.MUIV_List_Exchange_Active= -1
mui.MUIV_List_Exchange_Bottom= -2
mui.MUIV_List_Exchange_Next= -3 		--  for 2nd parameter only
mui.MUIV_List_Exchange_Previous= -4 		--  for 2nd parameter only
mui.MUIV_List_Jump_Top= 0
mui.MUIV_List_Jump_Active= -1
mui.MUIV_List_Jump_Bottom= -2
mui.MUIV_List_Jump_Down= -3
mui.MUIV_List_Jump_Up= -4
mui.MUIV_List_NextSelected_Start= -1
mui.MUIV_List_NextSelected_End= -1
mui.MUIV_NList_SelectChange_Flag_Multi= 1
mui.MUIV_List_InsertSingleAsTree_Root= -1
mui.MUIV_List_InsertSingleAsTree_Top= 0
mui.MUIV_List_InsertSingleAsTree_Active= -1
mui.MUIV_List_InsertSingleAsTree_Sorted= -2
mui.MUIV_List_InsertSingleAsTree_Bottom= -3
mui.MUIV_List_InsertSingleAsTree_List= 1
mui.MUIV_List_InsertSingleAsTree_Closed= 2

-- ==================================================================
mui.MUIC_Menustrip= "Menustrip.mui"

mui.MUIA_Menustrip_Enabled= -2143125157 		--  MUI: V8  isg BOOL

-- ==================================================================
mui.MUIC_Menu= "Menu.mui"

mui.MUIA_Menu_Enabled= -2143097528 		--  MUI: V8  isg BOOL
mui.MUIA_Menu_Title= -2143117085 		--  MUI: V8  isg STRPTR

-- ==================================================================
mui.MUIC_Menuitem= "Menuitem.mui"

mui.MUIA_Menuitem_Checked= -2143136214 		--  MUI: V8  isg BOOL
mui.MUIA_Menuitem_Checkit= -2143135026 		--  MUI: V8  isg BOOL
mui.MUIA_Menuitem_CommandString= -2143110708 		--  MUI: V16 isg BOOL
mui.MUIA_Menuitem_Enabled= -2143113713 		--  MUI: V8  isg BOOL
mui.MUIA_Menuitem_Exclude= -2143155258 		--  MUI: V8  isg LONG
mui.MUIA_Menuitem_Shortcut= -2143150032 		--  MUI: V8  isg STRPTR
mui.MUIA_Menuitem_Title= -2143151938 		--  MUI: V8  isg STRPTR
mui.MUIA_Menuitem_Toggle= -2143138468 		--  MUI: V8  isg BOOL
mui.MUIA_Menuitem_Trigger= -2143129806 		--  MUI: V8  ..g struct MenuItem *
mui.MUIA_Menuitem_NewMenu= -1874715648 		--  Zune: V1 ..g struct NewMenu *

mui.MUIV_Menuitem_Shortcut_Check= -1

-- ==================================================================
mui.MUIC_Notify= "Notify.mui"

mui.MUIM_CallHook= -2143110805 		--  MUI: V4
mui.MUIM_Export= -2143154404 		--  MUI: V12
mui.MUIM_FindUData= -2143108714 		--  MUI: V8
mui.MUIM_GetConfigItem= -2143142181 		--  MUI: V11
mui.MUIM_GetUData= -2143097588 		--  MUI: V8
mui.MUIM_Import= -2143105006 		--  MUI: V12
mui.MUIM_KillNotify= -2143104448 		--  MUI: V4
mui.MUIM_KillNotifyObj= -2143112891 		--  MUI: V16
mui.MUIM_MultiSet= -2143104170 		--  MUI: V7
mui.MUIM_NoNotifySet= -2143149713 		--  MUI: V9
mui.MUIM_Notify= -2143106613 		--  MUI: V4
mui.MUIM_Set= -2143136614 		--  MUI: V4
mui.MUIM_SetAsString= -2143148656 		--  MUI: V4
mui.MUIM_SetUData= -2143106784 		--  MUI: V8
mui.MUIM_SetUDataOnce= -2143106535 		--  MUI: V11
mui.MUIM_WriteLong= -2143122042 		--  MUI: V6
mui.MUIM_WriteString= -2143138828 		--  MUI: V6
mui.MUIM_ConnectParent= -1874715392 		--  Zune: V1
mui.MUIM_DisconnectParent= -1874715391 		--  Zune: V1

mui.MUIA_ApplicationObject= -2143104018 		--  MUI: V4  ..g Object *
mui.MUIA_AppMessage= -2143151787 		--  MUI: V5  ..g struct AppMessage *
mui.MUIA_HelpLine= -2143115227 		--  MUI: V4  isg LONG
mui.MUIA_HelpNode= -2143155323 		--  MUI: V4  isg STRPTR
mui.MUIA_NoNotify= -2143143943 		--  MUI: V7  .s. BOOL
mui.MUIA_ObjectID= -2143103122 		--  MUI: V11 isg ULONG
mui.MUIA_Parent= -2143100065 		--  MUI: V11 ..g Object *
mui.MUIA_Revision= -2143125846 		--  MUI: V4  ..g LONG
mui.MUIA_UserData= -2143157485 		--  MUI: V4  isg ULONG
mui.MUIA_Version= -2143149311 		--  MUI: V4  ..g LONG

mui.MUIV_TriggerValue= 1233727793
mui.MUIV_NotTriggerValue= 1233727795
mui.MUIV_EveryTime= 1233727793 		--  as TrigVal
mui.MUIV_Notify_Self= 1
mui.MUIV_Notify_Window= 2
mui.MUIV_Notify_Application= 3
mui.MUIV_Notify_Parent= 4

-- ==================================================================
mui.MUIC_Numeric= "Numeric.mui"

mui.MUIM_Numeric_Decrease= -2143140953 		--  MUI: V11
mui.MUIM_Numeric_Increase= -2143129907 		--  MUI: V11
mui.MUIM_Numeric_ScaleToValue= -2143157460 		--  MUI: V11
mui.MUIM_Numeric_SetDefault= -2143114486 		--  MUI: V11
mui.MUIM_Numeric_Stringify= -2143139695 		--  MUI: V11
mui.MUIM_Numeric_ValueToScale= -2143142321 		--  MUI: V11
mui.MUIM_Numeric_ValueToScaleExt= -1874715136 		--  ZUNE only

mui.MUIA_Numeric_CheckAllSizes= -2143152748 		--  MUI: V11 isg BOOL
mui.MUIA_Numeric_Default= -2143132696 		--  MUI: V11 isg LONG
mui.MUIA_Numeric_Format= -2143132695 		--  MUI: V11 isg STRPTR
mui.MUIA_Numeric_Max= -2143103094 		--  MUI: V11 isg LONG
mui.MUIA_Numeric_Min= -2143099900 		--  MUI: V11 isg LONG
mui.MUIA_Numeric_Reverse= -2143096160 		--  MUI: V11 isg BOOL
mui.MUIA_Numeric_RevLeftRight= -2143120217 		--  MUI: V11 isg BOOL
mui.MUIA_Numeric_RevUpDown= -2143137059 		--  MUI: V11 isg BOOL
mui.MUIA_Numeric_Value= -2143113670 		--  MUI: V11 isg LONG

-- ==================================================================
mui.MUIC_Numericbutton= "Numericbutton.mui"

-- ==================================================================
mui.MUIC_Palette= "Palette.mui"

mui.MUIA_Palette_Entries= -2143116328 		--  V6  i.g struct MUI_Palette_Entry *
mui.MUIA_Palette_Groupable= -2143142297 		--  V6  isg BOOL
mui.MUIA_Palette_Names= -2143108190 		--  V6  isg char **

mui.MUIV_Palette_Entry_End= -1

-- ==================================================================
mui.MUIC_Penadjust= "Penadjust.mui"

mui.MUIA_Penadjust_PSIMode= -2143150917 		--  MUI: V11  i.. BOOL

-- ==================================================================
mui.MUIC_Pendisplay= "Pendisplay.mui"

mui.MUIM_Pendisplay_SetColormap= -2143140953 		--  MUI: V13
mui.MUIM_Pendisplay_SetMUIPen= -2143129907 		--  MUI: V13
mui.MUIM_Pendisplay_SetRGB= -2143157460 		--  MUI: V13

mui.MUIA_Pendisplay_Pen= -2143115448 		--  MUI: V13  ..g Object *
mui.MUIA_Pendisplay_Reference= -2143101916 		--  MUI: V13  isg Object *
mui.MUIA_Pendisplay_RGBcolor= -2143116887 		--  MUI: V11  isg struct MUI_RGBcolor *
mui.MUIA_Pendisplay_Spec= -2143116796 		--  MUI: V11  isg struct MUI_PenSpec  *


-- ==================================================================
mui.MUIC_Popasl= "Popasl.mui"

mui.MUIA_Popasl_Active= -2143151305 		--  MUI: V7  ..g BOOL
mui.MUIA_Popasl_StartHook= -2143111421 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popasl_StopHook= -2143102766 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popasl_Type= -2143101123 		--  MUI: V7  i.g ULONG

-- ==================================================================
mui.MUIC_Popframe= "Popframe.mui"

-- ==================================================================
mui.MUIC_Popimage= "Popimage.mui"

-- ==================================================================
mui.MUIC_Poplist= "Poplist.mui"

mui.MUIA_Poplist_Array= -2143156148 		--  MUI: V8  i.. char **

-- ==================================================================
mui.MUIC_Popobject= "Popobject.mui"

mui.MUIA_Popobject_Follow= -2143138635 		--  MUI: V7  isg BOOL
mui.MUIA_Popobject_Light= -2143115869 		--  MUI: V7  isg BOOL
mui.MUIA_Popobject_Object= -2143120413 		--  MUI: V7  i.g Object *
mui.MUIA_Popobject_ObjStrHook= -2143102140 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popobject_StrObjHook= -2143093791 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popobject_Volatile= -2143137044 		--  MUI: V7  isg BOOL
mui.MUIA_Popobject_WindowHook= -2143096428 		--  MUI: V9  isg struct Hook *

-- ==================================================================
mui.MUIC_Poppen= "Poppen.mui"

-- ==================================================================
mui.MUIC_Popscreen= "Popscreen.mui"

-- ==================================================================
mui.MUIC_Popstring= "Popstring.mui"

mui.MUIM_Popstring_Close= -2143101870 		--  MUI: V7
mui.MUIM_Popstring_Open= -2143135558 		--  MUI: V7

mui.MUIA_Popstring_Button= -2143104839 		--  MUI: V7  i.g Object *
mui.MUIA_Popstring_CloseHook= -2143136065 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popstring_OpenHook= -2143118080 		--  MUI: V7  isg struct Hook *
mui.MUIA_Popstring_String= -2143143446 		--  MUI: V7  i.g Object *
mui.MUIA_Popstring_Toggle= -2143147142 		--  MUI: V7  isg BOOL

-- ==================================================================
mui.MUIC_Prop= "Prop.mui"

mui.MUIM_Prop_Decrease= -2143154735 		--  MUI: V16
mui.MUIM_Prop_Increase= -2143106368 		--  MUI: V16

mui.MUIA_Prop_Entries= -2143093797 		--  MUI: V4  isg LONG
mui.MUIA_Prop_First= -2143103822 		--  MUI: V4  isg LONG
mui.MUIA_Prop_Horiz= -2143095565 		--  MUI: V4  i.g BOOL
mui.MUIA_Prop_Slider= -2143118278 		--  MUI: V4  isg BOOL
mui.MUIA_Prop_UseWinBorder= -2143101202 		--  MUI: V13 i.. LONG
mui.MUIA_Prop_Visible= -2143093082 		--  MUI: V4  isg LONG
mui.MUIA_Prop_DeltaFactor= -2143126434 		--  MUI:    is. LONG
mui.MUIA_Prop_DoSmooth= -2143144242 		--  MUI: V4 i.. LONG

mui.MUIV_Prop_UseWinBorder_None= 0
mui.MUIV_Prop_UseWinBorder_Left= 1
mui.MUIV_Prop_UseWinBorder_Right= 2
mui.MUIV_Prop_UseWinBorder_Bottom= 3

-- ==================================================================
mui.MUIC_Rectangle= "Rectangle.mui"

mui.MUIA_Rectangle_BarTitle= -2143132023 		--  V11 i.g STRPTR
mui.MUIA_Rectangle_HBar= -2143106749 		--  V7  i.g BOOL
mui.MUIA_Rectangle_VBar= -2143149564 		--  V7  i.g BOOL

-- ==================================================================
mui.MUIC_Radio= "Radio.mui"

mui.MUIA_Radio_Active= -2143118527 		--  MUI:V4  isg LONG
mui.MUIA_Radio_Entries= -2143111519 		--  MUI:V4  i.. STRPTR *

-- ==================================================================
mui.MUIC_Register= "Register.mui"

mui.MUIA_Register_Frame= -2143144805 		--  V7  i.g BOOL
mui.MUIA_Register_Titles= -2143119380 		--  V7  i.g STRPTR *
mui.MUIA_Register_Columns= -1874711552 		--  Zune V1  i..

-- ==================================================================
mui.MUIC_Scale= "Scale.mui"

mui.MUIA_Scale_Horiz= -2143120998 		--  MUI: V4  isg BOOL

-- ==================================================================
mui.MUIC_Scrmodelist= "Scrmodelist.mui"

-- ==================================================================
mui.MUIC_Scrollbar= "Scrollbar.mui"

mui.MUIA_Scrollbar_Type= -2143093909 		--  V11 i.. LONG

mui.MUIV_Scrollbar_Type_Default= 0
mui.MUIV_Scrollbar_Type_Bottom= 1
mui.MUIV_Scrollbar_Type_Top= 2
mui.MUIV_Scrollbar_Type_Sym= 3

-- ==================================================================
mui.MUIC_Scrollbutton= "Scrollbutton.mui"

mui.MUIA_Scrollbutton_NewPosition= -1874706176 		--  --G  ULONG (2 x WORD)
mui.MUIA_Scrollbutton_Horiz= -1874706175 		--  -SG  WORD
mui.MUIA_Scrollbutton_Vert= -1874706174 		--  -SG  WORD
mui.MUIA_Scrollbutton_HorizProp= -1874706173 		--  --G  Object *
mui.MUIA_Scrollbutton_VertProp= -1874706172 		--  --G  Object *
-- ==================================================================
mui.MUIC_Scrollgroup= "Scrollgroup.mui"

mui.MUIA_Scrollgroup_Contents= -2143153567 		--  V4  i.g Object *
mui.MUIA_Scrollgroup_FreeHoriz= -2143120653 		--  V9  i.. BOOL
mui.MUIA_Scrollgroup_FreeVert= -2143148814 		--  V9  i.. BOOL
mui.MUIA_Scrollgroup_HorizBar= -2143111619 		--  V16 ..g Object *
mui.MUIA_Scrollgroup_UseWinBorder= -2143124287 		--  V13 i.. BOOL
mui.MUIA_Scrollgroup_VertBar= -2143105600 		--  V16 ..g Object *

-- ==================================================================
mui.MUIC_Semaphore= "Semaphore.mui"

mui.MUIM_Semaphore_Attempt= -2143130398 		--  MUI: V11
mui.MUIM_Semaphore_AttemptShared= -2143148719 		--  MUI: V11
mui.MUIM_Semaphore_Obtain= -2143127824 		--  MUI: V11
mui.MUIM_Semaphore_ObtainShared= -2143098366 		--  MUI: V11
mui.MUIM_Semaphore_Release= -2143150291 		--  MUI: V11

-- ==================================================================
mui.MUIC_Settings= "Settings.mui"

-- ==================================================================
mui.MUIC_Settingsgroup= "Settingsgroup.mui"

mui.MUIM_Settingsgroup_ConfigToGadgets= -2143129533 		--  MUI: V11
mui.MUIM_Settingsgroup_GadgetsToConfig= -2143137214 		--  MUI: V11

-- ==================================================================
mui.MUIC_Slider= "Slider.mui"

mui.MUIA_Slider_Horiz= -2143094063 		--  V11 isg BOOL
mui.MUIA_Slider_Quiet= -2143155418 		--  V6  i.. BOOL

-- ==================================================================
mui.MUIC_String= "String.mui"

mui.MUIM_String_ClearSelected= -1392504828
mui.MUIM_String_FileNameStart= -1392504826
mui.MUIM_String_Insert= -1392504830

mui.MUIA_String_Accept= -2143099935 		--  V4  isg STRPTR
mui.MUIA_String_Acknowledge= -2143157652 		--  V4  ..g STRPTR
mui.MUIA_String_AdvanceOnCR= -2143148322 		--  V11 isg BOOL
mui.MUIA_String_AttachedList= -2143154222 		--  V4  isg Object *
mui.MUIA_String_BufferPos= -2143122580 		--  V4  .sg LONG
mui.MUIA_String_Contents= -2143121411 		--  V4  isg STRPTR
mui.MUIA_String_DisplayPos= -2143105857 		--  V4  .sg LONG
mui.MUIA_String_EditHook= -2143138765 		--  V7  isg struct Hook *
mui.MUIA_String_Format= -2143128444 		--  V4  i.g LONG
mui.MUIA_String_Integer= -2143129974 		--  V4  isg ULONG
mui.MUIA_String_LonelyEditHook= -2143152791 		--  V11 isg BOOL
mui.MUIA_String_MaxLen= -2143139452 		--  V4  i.g LONG
mui.MUIA_String_Reject= -2143152228 		--  V4  isg STRPTR
mui.MUIA_String_Secret= -2143123607 		--  V4  i.g BOOL
mui.MUIA_String_Columns= -1392504827
mui.MUIA_String_NoInput= -1392504825
mui.MUIA_String_SelectSize= -1392504831
mui.MUIA_String_StayActive= -1392504829
mui.MUIA_String_KeyUpFocus= -1392504824
mui.MUIA_String_KeyDownFocus= -1392504823

mui.MUIV_String_Format_Left= 0
mui.MUIV_String_Format_Center= 1
mui.MUIV_String_Format_Right= 2
mui.MUIV_String_Insert_StartOfString= 0
mui.MUIV_String_Insert_EndOfString= -2
mui.MUIV_String_Insert_BufferPos= -1
mui.MUIV_String_BufferPos_End= -1

mui.MUIR_String_FileNameStart_Volume= -1

-- ==================================================================
mui.MUIC_Text= "Text.mui"

mui.MUIA_Text_Contents= -2143094564 		--  MUI: V4  isg STRPTR
mui.MUIA_Text_HiChar= -2143151873 		--  MUI: V4  i.. char
mui.MUIA_Text_HiCharIdx= -2143152907 		--           i.. char
mui.MUIA_Text_PreParse= -2143136147 		--  MUI: V4  isg STRPTR
mui.MUIA_Text_SetMax= -2143138550 		--  MUI: V4  i.. BOOL
mui.MUIA_Text_SetMin= -2143138288 		--  MUI: V4  i.. BOOL
mui.MUIA_Text_SetVMax= -2143154805 		--  MUI: V11 i.. BOOL

mui.MUIX_L= "\27l"		--  justify left
mui.MUIX_C= "\27c"		--  justify centered
mui.MUIX_R= "\27r"		--  justify right
mui.MUIX_N= "\27n"		--  normal style
mui.MUIX_B= "\27b"		--  bold style
mui.MUIX_I= "\27i"		--  italic style
mui.MUIX_U= "\27u"		--  underlined style
mui.MUIX_PT= "\27" .. "2"		--  use text pen
mui.MUIX_PH= "\27" .. "8"		--  use highlight text pen

-- ==================================================================
mui.MUIC_Virtgroup= "Virtgroup.mui"

mui.MUIA_Virtgroup_Height= -2143145928 		--  V6  ..g LONG
mui.MUIA_Virtgroup_Input= -2143125634 		--  V11 i.. BOOL
mui.MUIA_Virtgroup_Left= -2143120527 		--  V6  isg LONG
mui.MUIA_Virtgroup_Top= -2143137280 		--  V6  isg LONG
mui.MUIA_Virtgroup_Width= -2143126455 		--  V6  ..g LONG

-- ==================================================================
mui.MUIC_Volumelist= "Volumelist.mui"

-- ==================================================================
mui.MUIC_Window= "Window.mui"

mui.MUIM_Window_ActionIconify= -2143146816 		--  MUI: V18 undoc
mui.MUIM_Window_AddEventHandler= -2143157321 		--  MUI: V16
mui.MUIM_Window_Cleanup= -2143114458 		--  MUI: V18 undoc */ /* For custom classes only
mui.MUIM_Window_RemEventHandler= -2143131746 		--  MUI: V16
mui.MUIM_Window_ScreenToBack= -2143121091 		--  MUI: V4
mui.MUIM_Window_ScreenToFront= -2143148124 		--  MUI: V4
mui.MUIM_Window_Setup= -2143108276 		--  MUI: V18 undoc */ /* For custom Classes only
mui.MUIM_Window_Snapshot= -2143120290 		--  MUI: V11
mui.MUIM_Window_ToBack= -2143152850 		--  MUI: V4
mui.MUIM_Window_ToFront= -2143136433 		--  MUI: V4
mui.MUIM_Window_AllocGadgetID= -1874708991 		--  Zune: V1 - allocate a GadgetID for BOOPSI gadgets
mui.MUIM_Window_FreeGadgetID= -1874708988 		--  Zune: V1 - free the GadgetID for BOOPSI gadgets

mui.MUIA_Window_Activate= -2143122129 		--  MUI: V4  isg BOOL
mui.MUIA_Window_ActiveObject= -2143127259 		--  MUI: V4  .sg Object *
mui.MUIA_Window_AltHeight= -2143105821 		--  MUI: V4  i.g LONG
mui.MUIA_Window_AltLeftEdge= -2143146651 		--  MUI: V4  i.g LONG
mui.MUIA_Window_AltTopEdge= -2143098469 		--  MUI: V4  i.g LONG
mui.MUIA_Window_AltWidth= -2143133452 		--  MUI: V4  i.g LONG
mui.MUIA_Window_AppWindow= -2143125297 		--  MUI: V5  i.. BOOL
mui.MUIA_Window_Backdrop= -2143108933 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_Borderless= -2143118471 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_CloseGadget= -2143117040 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_CloseRequest= -2143098770 		--  MUI: V4  ..g BOOL
mui.MUIA_Window_DefaultObject= -2143120169 		--  MUI: V4  isg Object *
mui.MUIA_Window_DepthGadget= -2143151837 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_DisableKeys= -2143138762 		--  MUI: V15 isg ULONG               */ /* undoc
mui.MUIA_Window_DragBar= -2143157155 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_FancyDrawing= -2143109874 		--  MUI: V8  isg BOOL
mui.MUIA_Window_Height= -2143135674 		--  MUI: V4  i.g LONG
mui.MUIA_Window_ID= -2143157827 		--  MUI: V4  isg ULONG
mui.MUIA_Window_InputEvent= -2143139880 		--  MUI: V4  ..g struct InputEvent *
mui.MUIA_Window_IsSubWindow= -2143111766 		--  MUI: V4  isg BOOL
mui.MUIA_Window_LeftEdge= -2143130523 		--  MUI: V4  i.g LONG
mui.MUIA_Window_MenuAction= -2143128287 		--  MUI: V8  isg ULONG
mui.MUIA_Window_Menustrip= -2143124130 		--  MUI: V8  i.g Object *
mui.MUIA_Window_MouseObject= -2143109221 		--  MUI: V10 ..g Object *
mui.MUIA_Window_NeedsMouseObject= -2143144150 		--  MUI: V10 i.. BOOL
mui.MUIA_Window_NoMenus= -2143117835 		--  MUI: V4  is. BOOL
mui.MUIA_Window_Open= -2143122784 		--  MUI: V4  .sg BOOL
mui.MUIA_Window_PublicScreen= -2143127324 		--  MUI: V6  isg STRPTR
mui.MUIA_Window_RefWindow= -2143157772 		--  MUI: V4  is. Object *
mui.MUIA_Window_RootObject= -2143106139 		--  MUI: V4  isg Object *
mui.MUIA_Window_Screen= -2143101105 		--  MUI: V4  isg struct Screen *
mui.MUIA_Window_ScreenTitle= -2143144784 		--  MUI: V5  isg STRPTR
mui.MUIA_Window_SizeGadget= -2143100099 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_SizeRight= -2143139968 		--  MUI: V4  i.. BOOL
mui.MUIA_Window_Sleep= -2143098917 		--  MUI: V4  .sg BOOL
mui.MUIA_Window_Title= -2143113923 		--  MUI: V4  isg STRPTR
mui.MUIA_Window_TopEdge= -2143126426 		--  MUI: V4  i.g LONG
mui.MUIA_Window_UseBottomBorderScroller= -2143138183 		--  MUI: V13 isg BOOL
mui.MUIA_Window_UseLeftBorderScroller= -2143141058 		--  MUI: V13 isg BOOL
mui.MUIA_Window_UseRightBorderScroller= -2143109026 		--  MUI: V13 isg BOOL
mui.MUIA_Window_Width= -2143101778 		--  MUI: V4  i.g LONG
mui.MUIA_Window_Window= -2143131070 		--  MUI: V4  ..g struct Window *
mui.MUIA_Window_EraseArea= -1874708992 		--  Zune only i.. BOOL (default: TRUE)
mui.MUIA_Window_WandererBackdrop= -1874708991 		--  ZUNE .s. BOOL
mui.MUIA_Window_ZoomGadget= -1874708990

mui.MUIV_Window_ActiveObject_None= 0
mui.MUIV_Window_ActiveObject_Next= -1
mui.MUIV_Window_ActiveObject_Prev= -2
mui.MUIV_Window_AltHeight_Scaled= -1000
mui.MUIV_Window_AltLeftEdge_Centered= -1
mui.MUIV_Window_AltLeftEdge_Moused= -2
mui.MUIV_Window_AltLeftEdge_NoChange= -1000
mui.MUIV_Window_AltTopEdge_Centered= -1
mui.MUIV_Window_AltTopEdge_Moused= -2
mui.MUIV_Window_AltTopEdge_NoChange= -1000
mui.MUIV_Window_AltWidth_Scaled= -1000
mui.MUIV_Window_Height_Scaled= -1000
mui.MUIV_Window_Height_Default= -1001
mui.MUIV_Window_LeftEdge_Centered= -1
mui.MUIV_Window_LeftEdge_Moused= -2
mui.MUIV_Window_TopEdge_Centered= -1
mui.MUIV_Window_TopEdge_Moused= -2
mui.MUIV_Window_Width_Scaled= -1000
mui.MUIV_Window_Width_Default= -1001

-- ==================================================================
mui.MUII_WindowBack= 0
mui.MUII_RequesterBack= 1
mui.MUII_ButtonBack= 2
mui.MUII_ListBack= 3
mui.MUII_TextBack= 4
mui.MUII_PropBack= 5
mui.MUII_PopupBack= 6
mui.MUII_SelectedBack= 7
mui.MUII_ListCursor= 8
mui.MUII_ListSelect= 9
mui.MUII_ListSelCur= 10
mui.MUII_ArrowUp= 11
mui.MUII_ArrowDown= 12
mui.MUII_ArrowLeft= 13
mui.MUII_ArrowRight= 14
mui.MUII_CheckMark= 15
mui.MUII_RadioButton= 16
mui.MUII_Cycle= 17
mui.MUII_PopUp= 18
mui.MUII_PopFile= 19
mui.MUII_PopDrawer= 20
mui.MUII_PropKnob= 21
mui.MUII_Drawer= 22
mui.MUII_HardDisk= 23
mui.MUII_Disk= 24
mui.MUII_Chip= 25
mui.MUII_Volume= 26
mui.MUII_RegisterBack= 27
mui.MUII_Network= 28
mui.MUII_Assign= 29
mui.MUII_TapePlay= 30
mui.MUII_TapePlayBack= 31
mui.MUII_TapePause= 32
mui.MUII_TapeStop= 33
mui.MUII_TapeRecord= 34
mui.MUII_GroupBack= 35
mui.MUII_SliderBack= 36
mui.MUII_SliderKnob= 37
mui.MUII_TapeUp= 38
mui.MUII_TapeDown= 39
mui.MUII_PageBack= 40
mui.MUII_ReadListBack= 41
mui.MUII_Count= 42
mui.MUII_BACKGROUND= 128
mui.MUII_SHADOW= 129
mui.MUII_SHINE= 130
mui.MUII_FILL= 131
mui.MUII_SHADOWBACK= 132
mui.MUII_SHADOWFILL= 133
mui.MUII_SHADOWSHINE= 134
mui.MUII_FILLBACK= 135
mui.MUII_FILLSHINE= 136
mui.MUII_SHINEBACK= 137
mui.MUII_FILLBACK2= 138
mui.MUII_HSHINEBACK= 139
mui.MUII_HSHADOWBACK= 140
mui.MUII_HSHINESHINE= 141
mui.MUII_HSHADOWSHADOW= 142
mui.MUII_MARKSHINE= 143
mui.MUII_MARKHALFSHINE= 144
mui.MUII_MARKBACKGROUND= 145
mui.MUII_LASTPAT= 146

mui.MC_TEMPLATE_ID= -1
mui.MUI_RXERR_BADDEFINITION= -1
mui.MUI_RXERR_OUTOFMEMORY= -2
mui.MUI_RXERR_UNKNOWNCOMMAND= -3
mui.MUI_RXERR_BADSYNTAX= -4

mui.Child= -2143148314
mui.SubWindow= -2143109152
mui.WindowContents= -2143106139

mui.MUI_MAXMAX= 10000
mui.MPEN_COUNT= 8
mui.MUIPEN_MASK= 65535

mui.MUIV_Font_Inherit= 0
mui.MUIV_Font_Normal= -1
mui.MUIV_Font_List= -2
mui.MUIV_Font_Tiny= -3
mui.MUIV_Font_Fixed= -4
mui.MUIV_Font_Title= -5
mui.MUIV_Font_Big= -6
mui.MUIV_Font_Button= -7
mui.MUIV_Font_Knob= -8
mui.MUIV_Font_NegCount= -9

mui.MUIKEY_RELEASE= -2
mui.MUIKEY_NONE= -1
mui.MUIKEY_PRESS= 0
mui.MUIKEY_TOGGLE= 1
mui.MUIKEY_UP= 2
mui.MUIKEY_DOWN= 3
mui.MUIKEY_PAGEUP= 4
mui.MUIKEY_PAGEDOWN= 5
mui.MUIKEY_TOP= 6
mui.MUIKEY_BOTTOM= 7
mui.MUIKEY_LEFT= 8
mui.MUIKEY_RIGHT= 9
mui.MUIKEY_WORDLEFT= 10
mui.MUIKEY_WORDRIGHT= 11
mui.MUIKEY_LINESTART= 12
mui.MUIKEY_LINEEND= 13
mui.MUIKEY_GADGET_NEXT= 14
mui.MUIKEY_GADGET_PREV= 15
mui.MUIKEY_GADGET_OFF= 16
mui.MUIKEY_WINDOW_CLOSE= 17
mui.MUIKEY_WINDOW_NEXT= 18
mui.MUIKEY_WINDOW_PREV= 19
mui.MUIKEY_HELP= 20
mui.MUIKEY_POPUP= 21
mui.MUIKEY_COUNT= 22

mui.MUIKEYF_PRESS= 1
mui.MUIKEYF_TOGGLE= 2
mui.MUIKEYF_UP= 4
mui.MUIKEYF_DOWN= 8
mui.MUIKEYF_PAGEUP= 16
mui.MUIKEYF_PAGEDOWN= 32
mui.MUIKEYF_TOP= 64
mui.MUIKEYF_BOTTOM= 128
mui.MUIKEYF_LEFT= 256
mui.MUIKEYF_RIGHT= 512
mui.MUIKEYF_WORDLEFT= 1024
mui.MUIKEYF_WORDRIGHT= 2048
mui.MUIKEYF_LINESTART= 4096
mui.MUIKEYF_LINEEND= 8192
mui.MUIKEYF_GADGET_NEXT= 16384
mui.MUIKEYF_GADGET_PREV= 32768
mui.MUIKEYF_GADGET_OFF= 65536
mui.MUIKEYF_WINDOW_CLOSE= 131072
mui.MUIKEYF_WINDOW_NEXT= 262144
mui.MUIKEYF_WINDOW_PREV= 524288
mui.MUIKEYF_HELP= 1048576
mui.MUIKEYF_POPUP= 2097152

mui.MUI_EHF_ALWAYSKEYS= 1
mui.MUI_EHF_GUIMODE= 2 		--  handler will not be called if object is not visible or disabled
mui.MUI_EventHandlerRC_Eat= 1 		--  do not invoke more handlers ers
mui.MUILM_UNKNOWN= -1 		--  should be returned if the hook function doesn't understand lm_Type

