****************************************************************************
**
** MUI - MagicUserInterface
** (c) 1993-95 by Stefan Stuntz
**
** Main Header File
**
*** Assembler modifications 26-Aug-93 by Henri Veisterä.
** 
*** Updated for MUI V2.2 25-Aug-94 by Paul Hickman.
**
**  Updated for MUI V3.1 and Assembler modifications 17-Dec-95 by
**  Stefan Sommerfeld
**
****************************************************************************
** General Header File Information
****************************************************************************
**
** All macro and structure definitions follow these rules:
**
** Name                       Meaning
**
** MUIC_<class>               Name of a class
** MUIM_<class>_<method>      Method
** MUIP_<class>_<method>      Methods parameter structure
** MUIV_<class>_<method>_<x>  Special method value
** MUIA_<class>_<attrib>      Attribute
** MUIV_<class>_<attrib>_<x>  Special attribute value
** MUIE_<error>               Error return code from MUI_Error()
** MUII_<name>                Standard MUI image
** MUIX_<code>                Control codes for text strings
** MUIO_<name>                Object type for MUI_MakeObject()
**
** MUIA_... attribute definitions are followed by a comment
** consisting of the three possible letters I, S and G.
** I: it's possible to specify this attribute at object creation time.
** S: it's possible to change this attribute with SetAttrs().
** G: it's possible to get this attribute with GetAttr().
**
** Items marked with "Custom Class" are for use in custom classes only!
*/



   IFND LIBRARIES_MUI_I
LIBRARIES_MUI_I SET 1

****************************************************************************
** Library specification
****************************************************************************

MUIMASTER_NAME MACRO
         dc.b     "muimaster.library",0
         even
         ENDM
MUIMASTER_VMIN EQU 13
CALLMUI  MACRO   ; Func
         move.l   _MUIMasterBase,a6
         jsr      _LVO\1(a6)
         ENDM
NULL     EQU      0
TRUE     EQU      1
FALSE    EQU      NULL


*************************************************************************
** Config items for MUIM_GetConfigItem
*************************************************************************


MUICFG_PublicScreen	= 36


*************************************************************************
** Black box specification structures for images, pens, frames
*************************************************************************


****************************************************************************
**
** For Boopsi Image Implementors Only:
**
** If MUI is using a boopsi image object, it will send a special method
** immediately after object creation. This method has a parameter structure
** where the boopsi can fill in its minimum and maximum size and learn if
** its used in a horizontal or vertical context.
**
** The boopsi image must use the method id (MUIM_BoopsiQuery) as return
** value. That's how MUI sees that the method is implemented.
**
** Note: MUI does not depend on this method. If the boopsi image doesn't
**       implement it, minimum size will be 0 and maximum size unlimited.
**
***************************************************************************/

MUIM_BoopsiQuery EQU      $80427157 ;* this is send to the boopsi and *
                                    ;* must be used as return value   *

*************************************************************************
** Structures and Macros for creating custom classes.
*************************************************************************


*
** GENERAL NOTES:
**
** - Everything described in this header file is only valid within
**   MUI classes. You may never use any of these things out of
**   a class, e.g. in a traditional MUI application.
**
** - Except when otherwise stated, all structures are strictly read only.
*


* Global information for every object *

MUIKEY_RELEASE 		EQU -2 * not a real key, faked when MUIKEY_PRESS is released *
MUIKEY_NONE    		EQU -1 
MUIKEY_PRESS		EQU  0
MUIKEY_TOGGLE		EQU  1
MUIKEY_UP		EQU  2		
MUIKEY_DOWN		EQU  3
MUIKEY_PAGEUP		EQU  4
MUIKEY_PAGEDOWN		EQU  5
MUIKEY_TOP		EQU  6
MUIKEY_BOTTOM		EQU  7
MUIKEY_LEFT		EQU  8
MUIKEY_RIGHT		EQU  9
MUIKEY_WORDLEFT		EQU 10
MUIKEY_WORDRIGHT	EQU 11
MUIKEY_LINESTART	EQU 12
MUIKEY_LINEEND		EQU 13
MUIKEY_GADGET_NEXT	EQU 14
MUIKEY_GADGET_PREV	EQU 15
MUIKEY_GADGET_OFF	EQU 16
MUIKEY_WINDOW_CLOSE	EQU 17
MUIKEY_WINDOW_NEXT	EQU 18
MUIKEY_WINDOW_PREV	EQU 19
MUIKEY_HELP		EQU 20
MUIKEY_POPUP		EQU 21
MUIKEY_COUNT 		EQU 22 * counter *


********************************************************************
* Some useful shortcuts. define MUI_NOSHORTCUTS to get rid of them *

* I Have left the original C macros here, so you can see what they are
* for, but I have not converted them to assembler.
*
*

;#define _app(obj)         (muiGlobalInfo(obj)->mgi_ApplicationObject)
;#define _win(obj)         (muiRenderInfo(obj)->mri_WindowObject)
;#define _dri(obj)         (muiRenderInfo(obj)->mri_DrawInfo)
;#define _window(obj)      (muiRenderInfo(obj)->mri_Window)
;#define _screen(obj)      (muiRenderInfo(obj)->mri_Screen)
;#define _rp(obj)          (muiRenderInfo(obj)->mri_RastPort)
;#define _left(obj)        (muiAreaData(obj)->mad_Box.Left)
;#define _top(obj)         (muiAreaData(obj)->mad_Box.Top)
;#define _width(obj)       (muiAreaData(obj)->mad_Box.Width)
;#define _height(obj)      (muiAreaData(obj)->mad_Box.Height)
;#define _right(obj)       (_left(obj)+_width(obj)-1)
;#define _bottom(obj)      (_top(obj)+_height(obj)-1)
;#define _addleft(obj)     (muiAreaData(obj)->mad_addleft  )
;#define _addtop(obj)      (muiAreaData(obj)->mad_addtop   )
;#define _subwidth(obj)    (muiAreaData(obj)->mad_subwidth )
;#define _subheight(obj)   (muiAreaData(obj)->mad_subheight)
;#define _mleft(obj)       (_left(obj)+_addleft(obj))
;#define _mtop(obj)        (_top(obj)+_addtop(obj))
;#define _mwidth(obj)      (_width(obj)-_subwidth(obj))
;#define _mheight(obj)     (_height(obj)-_subheight(obj))
;#define _mright(obj)      (_mleft(obj)+_mwidth(obj)-1)
;#define _mbottom(obj)     (_mtop(obj)+_mheight(obj)-1)
;#define _font(obj)        (muiAreaData(obj)->mad_Font)
;#define _flags(obj)       (muiAreaData(obj)->mad_Flags)




		;* ... private data follows ... *

*************************************************************************
** Public Screen Stuff
*************************************************************************

*
** NOTE: This stuff is only included to allow compilation of the supplied
**       public screen manager for educational purposes. Everything
**       here is subject to change without notice and I guarantee to
**       do that just for fun!
**       More info can be found in the screen manager source file.
*

* The folowing labels are strings. You have to copy they to your source
* if you want to use they.

;PSD_INITIAL_NAME	dc.b	"(unnamed)",0
;PSD_INITIAL_TITLE	dc.b	"MUI Public Screen",0
;PSD_ID_MPUB		dc.l	'MPUB'

;PSD_NAME_FRONTMOST	dc.b	"«Frontmost»",0

;PSD_FILENAME_SAVE	dc.b	"envarc:mui/PublicScreens.iff",0
;PSD_FILENAME_USE	dc.b	"env:mui/PublicScreens.iff",0

PSD_MAXLEN_NAME			= 32
PSD_MAXLEN_TITLE		= 128
PSD_MAXLEN_FONT			= 48
PSD_MAXLEN_BACKGROUND		= 256
PSD_MAXSYSPENS			= 20
PSD_NUMSYSPENS			= 12
PSD_MAXMUIPENS			= 10

***************************************************************************
** Object Types for MUI_MakeObject()
***************************************************************************

MUIO_Label        EQU	1   * STRPTR label, ULONG flags 
MUIO_Button       EQU	2   * STRPTR label 
MUIO_Checkmark    EQU	3   * STRPTR label
MUIO_Cycle        EQU	4   * STRPTR label, STRPTR *entries 
MUIO_Radio        EQU	5   * STRPTR label, STRPTR *entries 
MUIO_Slider       EQU	6   * STRPTR label, LONG min, LONG max 
MUIO_String       EQU	7   * STRPTR label, LONG maxlen 
MUIO_PopButton    EQU	8   * STRPTR imagespec 
MUIO_HSpace       EQU	9   * LONG space   
MUIO_VSpace       EQU	10   * LONG space   
MUIO_HBar         EQU	11   * LONG space   
MUIO_VBar         EQU	12   * LONG space   
MUIO_MenustripNM  EQU	13   * struct NewMenu .*ULONG flags 
MUIO_Menuitem     EQU	14   * STRPTR label, STRPTR shortcut, ULONG flags, ULONG data 
MUIO_BarTitle     EQU	15   * STRPTR label 
MUIO_NumericButton  EQU	16   * STRPTR label, LONG min, LONG max, STRPTR format

MUIO_Label_SingleFrame EQU	1<<8
MUIO_Label_DoubleFrame EQU	1<<9
MUIO_Label_LeftAligned EQU	1<<10
MUIO_Label_Centered    EQU	1<<11
MUIO_Label_FreeVert    EQU	1<<12

MUIO_MenustripNM_CommandKeyCheck EQU	1<<0 * check for "localized" menu items such as "O\0Open"


****************************************************************************
** ARexx Interface
****************************************************************************


MUI_RXERR_BADDEFINITION  EQU	-1
MUI_RXERR_OUTOFMEMORY    EQU	-2
MUI_RXERR_UNKNOWNCOMMAND EQU	-3
MUI_RXERR_BADSYNTAX      EQU	-4



****************************************************************************
** Return values for MUI_Error()
****************************************************************************

MUIE_OK                    EQU 0
MUIE_OutOfMemory           EQU 1
MUIE_OutOfGfxMemory        EQU 2
MUIE_InvalidWindowObject   EQU 3
MUIE_MissingLibrary        EQU 4
MUIE_NoARexx               EQU 5
MUIE_SingleTask            EQU 6



****************************************************************************
** Standard MUI Images
****************************************************************************

MUII_WindowBack		EQU 0
MUII_RequesterBack	EQU 1
MUII_ButtonBack		EQU 2
MUII_ListBack		EQU 3
MUII_TextBack		EQU 4
MUII_PropBack		EQU 5
MUII_PopupBack		EQU 6
MUII_SelectedBack	EQU 7
MUII_ListCursor		EQU 8
MUII_ListSelect		EQU 9
MUII_ListSelCur		EQU 10
MUII_ArrowUp		EQU 11
MUII_ArrowDown		EQU 12
MUII_ArrowLeft		EQU 13
MUII_ArrowRight		EQU 14
MUII_CheckMark		EQU 15
MUII_RadioButton	EQU 16
MUII_Cycle		EQU 17
MUII_PopUp		EQU 18
MUII_PopFile		EQU 19
MUII_PopDrawer		EQU 20
MUII_PropKnob		EQU 21
MUII_Drawer		EQU 22
MUII_HardDisk		EQU 23
MUII_Disk		EQU 24
MUII_Chip		EQU 25
MUII_Volume		EQU 26
MUII_PopUpBack		EQU 27
MUII_Network		EQU 28
MUII_Assign		EQU 29
MUII_TapePlay		EQU 30
MUII_TapePlayBack	EQU 31
MUII_TapePause		EQU 32
MUII_TapeStop		EQU 33
MUII_TapeRecord		EQU 34
MUII_GroupBack		EQU 35
MUII_SliderBack		EQU 36
MUII_SliderKnob		EQU 37
MUII_TapeUp		EQU 38
MUII_TapeDown		EQU 39
MUII_PageBack		EQU 40
MUII_ReadListBack	EQU 41
MUII_Count		EQU 42

MUII_BACKGROUND		EQU 128    * These are direct color
MUII_SHADOW		EQU 129    * combinations and are not
MUII_SHINE		EQU 130    * affected by users prefs.
MUII_FILL		EQU 131
MUII_SHADOWBACK		EQU 132    * Generally, you should
MUII_SHADOWFILL		EQU 133    * avoid using them. Better
MUII_SHADOWSHINE	EQU 134    * use one of the customized
MUII_FILLBACK		EQU 135    * images above.
MUII_FILLSHINE		EQU 136
MUII_SHINEBACK		EQU 137
MUII_FILLBACK2		EQU 138
MUII_HSHINEBACK		EQU 139
MUII_HSHADOWBACK	EQU 140
MUII_HSHINESHINE	EQU 141
MUII_HSHADOWSHADOW	EQU 142
MUII_MARKSHINE		EQU 143
MUII_MARKHALFSHINE	EQU 144
MUII_MARKBACKGROUND	EQU 145
MUII_LASTPAT		EQU 145


****************************************************************************
** Special values for some methods 
****************************************************************************

MUIV_TriggerValue       EQU $49893131
MUIV_NotTriggerValue    EQU $49893133
MUIV_EveryTime          EQU $49893131

MUIV_Notify_Self	EQU 1
MUIV_Notify_Window	EQU 2
MUIV_Notify_Application	EQU 3
MUIV_Notify_Parent	EQU 4

MUIV_Application_Save_ENV	EQU 0
MUIV_Application_Save_ENVARC	EQU ~0
MUIV_Application_Load_ENV	EQU 0
MUIV_Application_Load_ENVARC	EQU ~0

MUIV_Application_ReturnID_Quit	EQU -1

MUIV_List_Insert_Top		EQU 0
MUIV_List_Insert_Active		EQU -1
MUIV_List_Insert_Sorted		EQU -2
MUIV_List_Insert_Bottom		EQU -3

MUIV_List_Remove_First		EQU 0
MUIV_List_Remove_Active		EQU -1
MUIV_List_Remove_Last		EQU -2
MUIV_List_Remove_Selected	EQU -3

MUIV_List_Select_Off		EQU 0
MUIV_List_Select_On		EQU 1
MUIV_List_Select_Toggle		EQU 2
MUIV_List_Select_Ask		EQU 3

MUIV_List_GetEntry_Active	EQU -1
MUIV_List_Select_Active		EQU -1
MUIV_List_Select_All		EQU -2

MUIV_List_Redraw_Active		EQU -1
MUIV_List_Redraw_All		EQU -2

MUIV_List_Move_Top		EQU 0
MUIV_List_Move_Active		EQU -1
MUIV_List_Move_Bottom		EQU -2
MUIV_List_Move_Next		EQU -3 * only valid for second parameter 
MUIV_List_Move_Previous		EQU -4 * only valid for second parameter
 
MUIV_List_Exchange_Top		EQU  0
MUIV_List_Exchange_Active	EQU -1
MUIV_List_Exchange_Bottom	EQU -2
MUIV_List_Exchange_Next		EQU -3 * only valid for second parameter
MUIV_List_Exchange_Previous	EQU -4 * only valid for second parameter

MUIV_List_Jump_Top		EQU 0
MUIV_List_Jump_Active		EQU -1
MUIV_List_Jump_Bottom		EQU -2
MUIV_List_Jump_Up		EQU -4
MUIV_List_Jump_Down		EQU -3

MUIV_List_NextSelected_Start	EQU -1
MUIV_List_NextSelected_End	EQU -1

MUIV_DragQuery_Refuse		EQU 0
MUIV_DragQuery_Accept		EQU 1

MUIV_DragReport_Abort		EQU 0
MUIV_DragReport_Continue	EQU 1
MUIV_DragReport_Lock		EQU 2
MUIV_DragReport_Refresh		EQU 3

***************************************************************************
** Parameter structures for some classes
***************************************************************************/




;*********************************************
;** Begin of automatic header file creation **
;*********************************************




;****************************************************************************
;** Notify                                                                 **
;****************************************************************************

;** Methods **

MUIM_CallHook                       EQU $8042b96b ;** V4  **
MUIM_Export                         EQU $80420f1c ;** V12 **
MUIM_FindUData                      EQU $8042c196 ;** V8  **
MUIM_GetUData                       EQU $8042ed0c ;** V8  **
MUIM_Import                         EQU $8042d012 ;** V12 **
MUIM_KillNotify                     EQU $8042d240 ;** V4  **
MUIM_MultiSet                       EQU $8042d356 ;** V7  **
MUIM_NoNotifySet                    EQU $8042216f ;** V9  **
MUIM_Notify                         EQU $8042c9cb ;** V4  **
MUIM_Set                            EQU $8042549a ;** V4  **
MUIM_SetAsString                    EQU $80422590 ;** V4  **
MUIM_SetUData                       EQU $8042c920 ;** V8  **
MUIM_SetUDataOnce                   EQU $8042ca19 ;** V11 **
MUIM_WriteLong                      EQU $80428d86 ;** V6  **
MUIM_WriteString                    EQU $80424bf4 ;** V6  **

;** Attributes **

MUIA_ApplicationObject              EQU $8042d3ee ;** V4  ..g Object *          **
MUIA_AppMessage                     EQU $80421955 ;** V5  ..g struct AppMessage * **
MUIA_HelpLine                       EQU $8042a825 ;** V4  isg LONG              **
MUIA_HelpNode                       EQU $80420b85 ;** V4  isg STRPTR            **
MUIA_NoNotify                       EQU $804237f9 ;** V7  .s. BOOL              **
MUIA_Parent                         EQU $8042e35f ;** V11 ..g Object *          **
MUIA_Revision                       EQU $80427eaa ;** V4  ..g LONG              **
MUIA_UserData                       EQU $80420313 ;** V4  isg ULONG             **
MUIA_Version                        EQU $80422301 ;** V4  ..g LONG              **



;****************************************************************************
;** Family                                                                 **
;****************************************************************************

;** Methods **

MUIM_Family_AddHead                 EQU $8042e200 ;** V8  **
MUIM_Family_AddTail                 EQU $8042d752 ;** V8  **
MUIM_Family_Insert                  EQU $80424d34 ;** V8  **
MUIM_Family_Remove                  EQU $8042f8a9 ;** V8  **
MUIM_Family_Sort                    EQU $80421c49 ;** V8  **
MUIM_Family_Transfer                EQU $8042c14a ;** V8  **

;** Attributes **

MUIA_Family_Child                   EQU $8042c696 ;** V8  i.. Object *          **
MUIA_Family_List                    EQU $80424b9e ;** V8  ..g struct MinList *  **



;****************************************************************************
;** Menustrip                                                              **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Menustrip_Enabled              EQU $8042815b ;** V8  isg BOOL              **



;****************************************************************************
;** Menu                                                                   **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Menu_Enabled                   EQU $8042ed48 ;** V8  isg BOOL              **
MUIA_Menu_Title                     EQU $8042a0e3 ;** V8  isg STRPTR            **



;****************************************************************************
;** Menuitem                                                               **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Menuitem_Checked               EQU $8042562a ;** V8  isg BOOL              **
MUIA_Menuitem_Checkit               EQU $80425ace ;** V8  isg BOOL              **
MUIA_Menuitem_Enabled               EQU $8042ae0f ;** V8  isg BOOL              **
MUIA_Menuitem_Exclude               EQU $80420bc6 ;** V8  isg LONG              **
MUIA_Menuitem_Shortcut              EQU $80422030 ;** V8  isg STRPTR            **
MUIA_Menuitem_Title                 EQU $804218be ;** V8  isg STRPTR            **
MUIA_Menuitem_Toggle                EQU $80424d5c ;** V8  isg BOOL              **
MUIA_Menuitem_Trigger               EQU $80426f32 ;** V8  ..g struct MenuItem * **

MUIV_Menuitem_Shortcut_Check        EQU -1


;****************************************************************************
;** Application                                                            **
;****************************************************************************

;** Methods **

MUIM_Application_AboutMUI           EQU $8042d21d ;** V14 **
MUIM_Application_AddInputHandler    EQU $8042f099 ;** V11 **
MUIM_Application_CheckRefresh       EQU $80424d68 ;** V11 **
MUIM_Application_GetMenuCheck       EQU $8042c0a7 ;** V4  **
MUIM_Application_GetMenuState       EQU $8042a58f ;** V4  **
MUIM_Application_Input              EQU $8042d0f5 ;** V4  **
MUIM_Application_InputBuffered      EQU $80427e59 ;** V4  **
MUIM_Application_Load               EQU $8042f90d ;** V4  **
MUIM_Application_NewInput           EQU $80423ba6 ;** V11 **
MUIM_Application_OpenConfigWindow   EQU $804299ba ;** V11 **
MUIM_Application_PushMethod         EQU $80429ef8 ;** V4  **
MUIM_Application_RemInputHandler    EQU $8042e7af ;** V11 **
MUIM_Application_ReturnID           EQU $804276ef ;** V4  **
MUIM_Application_Save               EQU $804227ef ;** V4  **
MUIM_Application_SetConfigItem      EQU $80424a80 ;** V11 **
MUIM_Application_SetMenuCheck       EQU $8042a707 ;** V4  **
MUIM_Application_SetMenuState       EQU $80428bef ;** V4  **
MUIM_Application_ShowHelp           EQU $80426479 ;** V4  **

;** Attributes **

MUIA_Application_Active             EQU $804260ab ;** V4  isg BOOL              **
MUIA_Application_Author             EQU $80424842 ;** V4  i.g STRPTR            **
MUIA_Application_Base               EQU $8042e07a ;** V4  i.g STRPTR            **
MUIA_Application_Broker             EQU $8042dbce ;** V4  ..g Broker *          **
MUIA_Application_BrokerHook         EQU $80428f4b ;** V4  isg struct Hook *     **
MUIA_Application_BrokerPort         EQU $8042e0ad ;** V6  ..g struct MsgPort *  **
MUIA_Application_BrokerPri          EQU $8042c8d0 ;** V6  i.g LONG              **
MUIA_Application_Commands           EQU $80428648 ;** V4  isg struct MUI_Command * **
MUIA_Application_Copyright          EQU $8042ef4d ;** V4  i.g STRPTR            **
MUIA_Application_Description        EQU $80421fc6 ;** V4  i.g STRPTR            **
MUIA_Application_DiskObject         EQU $804235cb ;** V4  isg struct DiskObject * **
MUIA_Application_DoubleStart        EQU $80423bc6 ;** V4  ..g BOOL              **
MUIA_Application_DropObject         EQU $80421266 ;** V5  is. Object *          **
MUIA_Application_ForceQuit          EQU $804257df ;** V8  ..g BOOL              **
MUIA_Application_HelpFile           EQU $804293f4 ;** V8  isg STRPTR            **
MUIA_Application_Iconified          EQU $8042a07f ;** V4  .sg BOOL              **
MUIA_Application_Menu               EQU $80420e1f ;** V4  i.g struct NewMenu *  **
MUIA_Application_MenuAction         EQU $80428961 ;** V4  ..g ULONG             **
MUIA_Application_MenuHelp           EQU $8042540b ;** V4  ..g ULONG             **
MUIA_Application_Menustrip          EQU $804252d9 ;** V8  i.. Object *          **
MUIA_Application_RexxHook           EQU $80427c42 ;** V7  isg struct Hook *     **
MUIA_Application_RexxMsg            EQU $8042fd88 ;** V4  ..g struct RxMsg *    **
MUIA_Application_RexxString         EQU $8042d711 ;** V4  .s. STRPTR            **
MUIA_Application_SingleTask         EQU $8042a2c8 ;** V4  i.. BOOL              **
MUIA_Application_Sleep              EQU $80425711 ;** V4  .s. BOOL              **
MUIA_Application_Title              EQU $804281b8 ;** V4  i.g STRPTR            **
MUIA_Application_UseCommodities     EQU $80425ee5 ;** V10 i.. BOOL              **
MUIA_Application_UseRexx            EQU $80422387 ;** V10 i.. BOOL              **
MUIA_Application_Version            EQU $8042b33f ;** V4  i.g STRPTR            **
MUIA_Application_Window             EQU $8042bfe0 ;** V4  i.. Object *          **
MUIA_Application_WindowList         EQU $80429abe ;** V13 ..g struct List *     **



;****************************************************************************
;** Window                                                                 **
;****************************************************************************

;** Methods **

MUIM_Window_GetMenuCheck            EQU $80420414 ;** V4  **
MUIM_Window_GetMenuState            EQU $80420d2f ;** V4  **
MUIM_Window_ScreenToBack            EQU $8042913d ;** V4  **
MUIM_Window_ScreenToFront           EQU $804227a4 ;** V4  **
MUIM_Window_SetCycleChain           EQU $80426510 ;** V4  **
MUIM_Window_SetMenuCheck            EQU $80422243 ;** V4  **
MUIM_Window_SetMenuState            EQU $80422b5e ;** V4  **
MUIM_Window_ToBack                  EQU $8042152e ;** V4  **
MUIM_Window_ToFront                 EQU $8042554f ;** V4  **

;** Attributes **

MUIA_Window_Activate                EQU $80428d2f ;** V4  isg BOOL              **
MUIA_Window_ActiveObject            EQU $80427925 ;** V4  .sg Object *          **
MUIA_Window_AltHeight               EQU $8042cce3 ;** V4  i.g LONG              **
MUIA_Window_AltLeftEdge             EQU $80422d65 ;** V4  i.g LONG              **
MUIA_Window_AltTopEdge              EQU $8042e99b ;** V4  i.g LONG              **
MUIA_Window_AltWidth                EQU $804260f4 ;** V4  i.g LONG              **
MUIA_Window_AppWindow               EQU $804280cf ;** V5  i.. BOOL              **
MUIA_Window_Backdrop                EQU $8042c0bb ;** V4  i.. BOOL              **
MUIA_Window_Borderless              EQU $80429b79 ;** V4  i.. BOOL              **
MUIA_Window_CloseGadget             EQU $8042a110 ;** V4  i.. BOOL              **
MUIA_Window_CloseRequest            EQU $8042e86e ;** V4  ..g BOOL              **
MUIA_Window_DefaultObject           EQU $804294d7 ;** V4  isg Object *          **
MUIA_Window_DepthGadget             EQU $80421923 ;** V4  i.. BOOL              **
MUIA_Window_DragBar                 EQU $8042045d ;** V4  i.. BOOL              **
MUIA_Window_FancyDrawing            EQU $8042bd0e ;** V8  isg BOOL              **
MUIA_Window_Height                  EQU $80425846 ;** V4  i.g LONG              **
MUIA_Window_ID                      EQU $804201bd ;** V4  isg ULONG             **
MUIA_Window_InputEvent              EQU $804247d8 ;** V4  ..g struct InputEvent * **
MUIA_Window_IsSubWindow             EQU $8042b5aa ;** V4  isg BOOL              **
MUIA_Window_LeftEdge                EQU $80426c65 ;** V4  i.g LONG              **
MUIA_Window_Menu                    EQU $8042db94 ;** V4  i.. struct NewMenu *  **
MUIA_Window_MenuAction              EQU $80427521 ;** V8  isg ULONG             **
MUIA_Window_Menustrip               EQU $8042855e ;** V8  i.g Object *          **
MUIA_Window_MouseObject             EQU $8042bf9b ;** V10 ..g Object *          **
MUIA_Window_NeedsMouseObject        EQU $8042372a ;** V10 i.. BOOL              **
MUIA_Window_NoMenus                 EQU $80429df5 ;** V4  is. BOOL              **
MUIA_Window_Open                    EQU $80428aa0 ;** V4  .sg BOOL              **
MUIA_Window_PublicScreen            EQU $804278e4 ;** V6  isg STRPTR            **
MUIA_Window_RefWindow               EQU $804201f4 ;** V4  is. Object *          **
MUIA_Window_RootObject              EQU $8042cba5 ;** V4  isg Object *          **
MUIA_Window_Screen                  EQU $8042df4f ;** V4  isg struct Screen *   **
MUIA_Window_ScreenTitle             EQU $804234b0 ;** V5  isg STRPTR            **
MUIA_Window_SizeGadget              EQU $8042e33d ;** V4  i.. BOOL              **
MUIA_Window_SizeRight               EQU $80424780 ;** V4  i.. BOOL              **
MUIA_Window_Sleep                   EQU $8042e7db ;** V4  .sg BOOL              **
MUIA_Window_Title                   EQU $8042ad3d ;** V4  isg STRPTR            **
MUIA_Window_TopEdge                 EQU $80427c66 ;** V4  i.g LONG              **
MUIA_Window_UseBottomBorderScroller EQU $80424e79 ;** V13 is. BOOL              **
MUIA_Window_UseLeftBorderScroller   EQU $8042433e ;** V13 is. BOOL              **
MUIA_Window_UseRightBorderScroller  EQU $8042c05e ;** V13 is. BOOL              **
MUIA_Window_Width                   EQU $8042dcae ;** V4  i.g LONG              **
MUIA_Window_Window                  EQU $80426a42 ;** V4  ..g struct Window *   **

MUIV_Window_ActiveObject_None       EQU 0
MUIV_Window_ActiveObject_Next       EQU -1
MUIV_Window_ActiveObject_Prev       EQU -2
MUIV_Window_AltHeightMinMax         EQU 0
MUIV_Window_AltHeightVisible        EQU -100
MUIV_Window_AltHeightScreen         EQU -200
MUIV_Window_AltHeight_Scaled        EQU -1000
MUIV_Window_AltLeftEdge_Centered    EQU -1
MUIV_Window_AltLeftEdge_Moused      EQU -2
MUIV_Window_AltLeftEdge_NoChange    EQU -1000
MUIV_Window_AltTopEdge_Centered     EQU -1
MUIV_Window_AltTopEdge_Moused       EQU -2
MUIV_Window_AltTopEdgeDelta         EQU -3
MUIV_Window_AltTopEdge_NoChange     EQU -1000
MUIV_Window_AltWidthMinMax          EQU 0
MUIV_Window_AltWidthVisible         EQU -100
MUIV_Window_AltWidthScreen          EQU -200
MUIV_Window_AltWidth_Scaled         EQU -1000
MUIV_Window_HeightMinMax            EQU 0
MUIV_Window_HeightVisible           EQU -100
MUIV_Window_HeightScreen            EQU -200
MUIV_Window_Height_Scaled           EQU -1000
MUIV_Window_Height_Default          EQU -1001
MUIV_Window_LeftEdge_Centered       EQU -1
MUIV_Window_LeftEdge_Moused         EQU -2
MUIV_Window_Menu_NoMenu             EQU -1
MUIV_Window_TopEdge_Centered        EQU -1
MUIV_Window_TopEdge_Moused          EQU -2
MUIV_Window_TopEdgeDelta            EQU -3
MUIV_Window_WidthMinMax             EQU 0
MUIV_Window_WidthVisible            EQU -100
MUIV_Window_WidthScreen             EQU -200
MUIV_Window_Width_Scaled            EQU -1000
MUIV_Window_Width_Default           EQU -1001


;****************************************************************************
;** Aboutmui                                                               **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Aboutmui_Application           EQU $80422523 ;** V11 i.. Object *          **



;****************************************************************************
;** Area                                                                   **
;****************************************************************************

;** Methods **

MUIM_AskMinMax                      EQU $80423874 ;** V4  **
MUIM_Cleanup                        EQU $8042d985 ;** V4  **
MUIM_ContextMenuBuild               EQU $80429d2e ;** V11 **
MUIM_ContextMenuChoice              EQU $80420f0e ;** V11 **
MUIM_DragBegin                      EQU $8042c03a ;** V11 **
MUIM_DragDrop                       EQU $8042c555 ;** V11 **
MUIM_DragFinish                     EQU $804251f0 ;** V11 **
MUIM_DragQuery                      EQU $80420261 ;** V11 **
MUIM_DragReport                     EQU $8042edad ;** V11 **
MUIM_Draw                           EQU $80426f3f ;** V4  **
MUIM_HandleInput                    EQU $80422a1a ;** V4  **
MUIM_Hide                           EQU $8042f20f ;** V4  **
MUIM_Setup                          EQU $80428354 ;** V4  **
MUIM_Show                           EQU $8042cc84 ;** V4  **

;** Attributes **

MUIA_Background                     EQU $8042545b ;** V4  is. LONG              **
MUIA_BottomEdge                     EQU $8042e552 ;** V4  ..g LONG              **
MUIA_ContextMenu                    EQU $8042b704 ;** V11 isg Object *          **
MUIA_ContextMenuTrigger             EQU $8042a2c1 ;** V11 ..g Object *          **
MUIA_ControlChar                    EQU $8042120b ;** V4  isg char              **
MUIA_CycleChain                     EQU $80421ce7 ;** V11 isg LONG              **
MUIA_Disabled                       EQU $80423661 ;** V4  isg BOOL              **
MUIA_Draggable                      EQU $80420b6e ;** V11 isg BOOL              **
MUIA_Dropable                       EQU $8042fbce ;** V11 isg BOOL              **
MUIA_ExportID                       EQU $8042d76e ;** V4  isg ULONG             **
MUIA_FillArea                       EQU $804294a3 ;** V4  is. BOOL              **
MUIA_FixHeight                      EQU $8042a92b ;** V4  i.. LONG              **
MUIA_FixHeightTxt                   EQU $804276f2 ;** V4  i.. STRPTR            **
MUIA_FixWidth                       EQU $8042a3f1 ;** V4  i.. LONG              **
MUIA_FixWidthTxt                    EQU $8042d044 ;** V4  i.. STRPTR            **
MUIA_Font                           EQU $8042be50 ;** V4  i.g struct TextFont * **
MUIA_Frame                          EQU $8042ac64 ;** V4  i.. LONG              **
MUIA_FramePhantomHoriz              EQU $8042ed76 ;** V4  i.. BOOL              **
MUIA_FrameTitle                     EQU $8042d1c7 ;** V4  i.. STRPTR            **
MUIA_Height                         EQU $80423237 ;** V4  ..g LONG              **
MUIA_HorizDisappear                 EQU $80429615 ;** V11 isg LONG              **
MUIA_HorizWeight                    EQU $80426db9 ;** V4  isg WORD              **
MUIA_InnerBottom                    EQU $8042f2c0 ;** V4  i.g LONG              **
MUIA_InnerLeft                      EQU $804228f8 ;** V4  i.g LONG              **
MUIA_InnerRight                     EQU $804297ff ;** V4  i.g LONG              **
MUIA_InnerTop                       EQU $80421eb6 ;** V4  i.g LONG              **
MUIA_InputMode                      EQU $8042fb04 ;** V4  i.. LONG              **
MUIA_LeftEdge                       EQU $8042bec6 ;** V4  ..g LONG              **
MUIA_MaxHeight                      EQU $804293e4 ;** V11 i.. LONG              **
MUIA_MaxWidth                       EQU $8042f112 ;** V11 i.. LONG              **
MUIA_ObjectID                       EQU $8042d76e ;** V11 isg ULONG             **
MUIA_Pressed                        EQU $80423535 ;** V4  ..g BOOL              **
MUIA_RightEdge                      EQU $8042ba82 ;** V4  ..g LONG              **
MUIA_Selected                       EQU $8042654b ;** V4  isg BOOL              **
MUIA_ShortHelp                      EQU $80428fe3 ;** V11 isg STRPTR            **
MUIA_ShowMe                         EQU $80429ba8 ;** V4  isg BOOL              **
MUIA_ShowSelState                   EQU $8042caac ;** V4  i.. BOOL              **
MUIA_Timer                          EQU $80426435 ;** V4  ..g LONG              **
MUIA_TopEdge                        EQU $8042509b ;** V4  ..g LONG              **
MUIA_VertDisappear                  EQU $8042d12f ;** V11 isg LONG              **
MUIA_VertWeight                     EQU $804298d0 ;** V4  isg WORD              **
MUIA_Weight                         EQU $80421d1f ;** V4  i.. WORD              **
MUIA_Width                          EQU $8042b59c ;** V4  ..g LONG              **
MUIA_Window                         EQU $80421591 ;** V4  ..g struct Window *   **
MUIA_WindowObject                   EQU $8042669e ;** V4  ..g Object *          **

MUIV_Font_Inherit                   EQU 0
MUIV_Font_Normal                    EQU -1
MUIV_Font_List                      EQU -2
MUIV_Font_Tiny                      EQU -3
MUIV_Font_Fixed                     EQU -4
MUIV_Font_Title                     EQU -5
MUIV_Font_Big                       EQU -6
MUIV_Font_Button                    EQU -7
MUIV_Frame_None                     EQU 0
MUIV_Frame_Button                   EQU 1
MUIV_Frame_ImageButton              EQU 2
MUIV_Frame_Text                     EQU 3
MUIV_Frame_String                   EQU 4
MUIV_Frame_ReadList                 EQU 5
MUIV_Frame_InputList                EQU 6
MUIV_Frame_Prop                     EQU 7
MUIV_Frame_Gauge                    EQU 8
MUIV_Frame_Group                    EQU 9
MUIV_Frame_PopUp                    EQU 10
MUIV_Frame_Virtual                  EQU 11
MUIV_Frame_Slider                   EQU 12
MUIV_Frame_Count                    EQU 13
MUIV_InputMode_None                 EQU 0
MUIV_InputMode_RelVerify            EQU 1
MUIV_InputMode_Immediate            EQU 2
MUIV_InputMode_Toggle               EQU 3


;****************************************************************************
;** Rectangle                                                              **
;****************************************************************************

;** Attributes **

MUIA_Rectangle_BarTitle             EQU $80426689 ;** V11 i.g STRPTR            **
MUIA_Rectangle_HBar                 EQU $8042c943 ;** V7  i.g BOOL              **
MUIA_Rectangle_VBar                 EQU $80422204 ;** V7  i.g BOOL              **



;****************************************************************************
;** Balance                                                                **
;****************************************************************************


;****************************************************************************
;** Image                                                                  **
;****************************************************************************

;** Attributes **

MUIA_Image_FontMatch                EQU $8042815d ;** V4  i.. BOOL              **
MUIA_Image_FontMatchHeight          EQU $80429f26 ;** V4  i.. BOOL              **
MUIA_Image_FontMatchWidth           EQU $804239bf ;** V4  i.. BOOL              **
MUIA_Image_FreeHoriz                EQU $8042da84 ;** V4  i.. BOOL              **
MUIA_Image_FreeVert                 EQU $8042ea28 ;** V4  i.. BOOL              **
MUIA_Image_OldImage                 EQU $80424f3d ;** V4  i.. struct Image *    **
MUIA_Image_Spec                     EQU $804233d5 ;** V4  i.. char *            **
MUIA_Image_State                    EQU $8042a3ad ;** V4  is. LONG              **



;****************************************************************************
;** Bitmap                                                                 **
;****************************************************************************

;** Attributes **

MUIA_Bitmap_Bitmap                  EQU $804279bd ;** V8  isg struct BitMap *   **
MUIA_Bitmap_Height                  EQU $80421560 ;** V8  isg LONG              **
MUIA_Bitmap_MappingTable            EQU $8042e23d ;** V8  isg UBYTE *           **
MUIA_Bitmap_Precision               EQU $80420c74 ;** V11 isg LONG              **
MUIA_Bitmap_RemappedBitmap          EQU $80423a47 ;** V11 ..g struct BitMap *   **
MUIA_Bitmap_SourceColors            EQU $80425360 ;** V8  isg ULONG *           **
MUIA_Bitmap_Transparent             EQU $80422805 ;** V8  isg LONG              **
MUIA_Bitmap_UseFriend               EQU $804239d8 ;** V11 i.. BOOL              **
MUIA_Bitmap_Width                   EQU $8042eb3a ;** V8  isg LONG              **



;****************************************************************************
;** Bodychunk                                                              **
;****************************************************************************

;** Attributes **

MUIA_Bodychunk_Body                 EQU $8042ca67 ;** V8  isg UBYTE *           **
MUIA_Bodychunk_Compression          EQU $8042de5f ;** V8  isg UBYTE             **
MUIA_Bodychunk_Depth                EQU $8042c392 ;** V8  isg LONG              **
MUIA_Bodychunk_Masking              EQU $80423b0e ;** V8  isg UBYTE             **



;****************************************************************************
;** Text                                                                   **
;****************************************************************************

;** Attributes **

MUIA_Text_Contents                  EQU $8042f8dc ;** V4  isg STRPTR            **
MUIA_Text_HiChar                    EQU $804218ff ;** V4  i.. char              **
MUIA_Text_PreParse                  EQU $8042566d ;** V4  isg STRPTR            **
MUIA_Text_SetMax                    EQU $80424d0a ;** V4  i.. BOOL              **
MUIA_Text_SetMin                    EQU $80424e10 ;** V4  i.. BOOL              **
MUIA_Text_SetVMax                   EQU $80420d8b ;** V11 i.. BOOL              **



;****************************************************************************
;** Gadget                                                                 **
;****************************************************************************

;** Attributes **

MUIA_Gadget_Gadget                  EQU $8042ec1a ;** V11 ..g struct Gadget *   **



;****************************************************************************
;** String                                                                 **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_String_Accept                  EQU $8042e3e1 ;** V4  isg STRPTR            **
MUIA_String_Acknowledge             EQU $8042026c ;** V4  ..g STRPTR            **
MUIA_String_AdvanceOnCR             EQU $804226de ;** V11 isg BOOL              **
MUIA_String_AttachedList            EQU $80420fd2 ;** V4  i.. Object *          **
MUIA_String_BufferPos               EQU $80428b6c ;** V4  .sg LONG              **
MUIA_String_Contents                EQU $80428ffd ;** V4  isg STRPTR            **
MUIA_String_DisplayPos              EQU $8042ccbf ;** V4  .sg LONG              **
MUIA_String_EditHook                EQU $80424c33 ;** V7  isg struct Hook *     **
MUIA_String_Format                  EQU $80427484 ;** V4  i.g LONG              **
MUIA_String_Integer                 EQU $80426e8a ;** V4  isg ULONG             **
MUIA_String_LonelyEditHook          EQU $80421569 ;** V11 isg BOOL              **
MUIA_String_MaxLen                  EQU $80424984 ;** V4  i.g LONG              **
MUIA_String_Reject                  EQU $8042179c ;** V4  isg STRPTR            **
MUIA_String_Secret                  EQU $80428769 ;** V4  i.g BOOL              **

MUIV_String_Format_Left             EQU 0
MUIV_String_Format_Center           EQU 1
MUIV_String_Format_Right            EQU 2


;****************************************************************************
;** Boopsi                                                                 **
;****************************************************************************

;** Attributes **

MUIA_Boopsi_Class                   EQU $80426999 ;** V4  isg struct IClass *   **
MUIA_Boopsi_ClassID                 EQU $8042bfa3 ;** V4  isg char *            **
MUIA_Boopsi_MaxHeight               EQU $8042757f ;** V4  isg ULONG             **
MUIA_Boopsi_MaxWidth                EQU $8042bcb1 ;** V4  isg ULONG             **
MUIA_Boopsi_MinHeight               EQU $80422c93 ;** V4  isg ULONG             **
MUIA_Boopsi_MinWidth                EQU $80428fb2 ;** V4  isg ULONG             **
MUIA_Boopsi_Object                  EQU $80420178 ;** V4  ..g Object *          **
MUIA_Boopsi_Remember                EQU $8042f4bd ;** V4  i.. ULONG             **
MUIA_Boopsi_Smart                   EQU $8042b8d7 ;** V9  i.. BOOL              **
MUIA_Boopsi_TagDrawInfo             EQU $8042bae7 ;** V4  isg ULONG             **
MUIA_Boopsi_TagScreen               EQU $8042bc71 ;** V4  isg ULONG             **
MUIA_Boopsi_TagWindow               EQU $8042e11d ;** V4  isg ULONG             **



;****************************************************************************
;** Prop                                                                   **
;****************************************************************************

;** Attributes **

MUIA_Prop_Entries                   EQU $8042fbdb ;** V4  isg LONG              **
MUIA_Prop_First                     EQU $8042d4b2 ;** V4  isg LONG              **
MUIA_Prop_Horiz                     EQU $8042f4f3 ;** V4  i.g BOOL              **
MUIA_Prop_Slider                    EQU $80429c3a ;** V4  isg BOOL              **
MUIA_Prop_UseWinBorder              EQU $8042deee ;** V13 i.. LONG              **
MUIA_Prop_Visible                   EQU $8042fea6 ;** V4  isg LONG              **

MUIV_Prop_UseWinBorder_None         EQU 0
MUIV_Prop_UseWinBorder_Left         EQU 1
MUIV_Prop_UseWinBorder_Right        EQU 2
MUIV_Prop_UseWinBorder_Bottom       EQU 3


;****************************************************************************
;** Gauge                                                                  **
;****************************************************************************

;** Attributes **

MUIA_Gauge_Current                  EQU $8042f0dd ;** V4  isg LONG              **
MUIA_Gauge_Divide                   EQU $8042d8df ;** V4  isg BOOL              **
MUIA_Gauge_Horiz                    EQU $804232dd ;** V4  i.. BOOL              **
MUIA_Gauge_InfoText                 EQU $8042bf15 ;** V7  isg STRPTR            **
MUIA_Gauge_Max                      EQU $8042bcdb ;** V4  isg LONG              **



;****************************************************************************
;** Scale                                                                  **
;****************************************************************************

;** Attributes **

MUIA_Scale_Horiz                    EQU $8042919a ;** V4  isg BOOL              **



;****************************************************************************
;** Colorfield                                                             **
;****************************************************************************

;** Attributes **

MUIA_Colorfield_Blue                EQU $8042d3b0 ;** V4  isg ULONG             **
MUIA_Colorfield_Green               EQU $80424466 ;** V4  isg ULONG             **
MUIA_Colorfield_Pen                 EQU $8042713a ;** V4  ..g ULONG             **
MUIA_Colorfield_Red                 EQU $804279f6 ;** V4  isg ULONG             **
MUIA_Colorfield_RGB                 EQU $8042677a ;** V4  isg ULONG *           **



;****************************************************************************
;** List                                                                   **
;****************************************************************************

;** Methods **

MUIM_List_Clear                     EQU $8042ad89 ;** V4  **
MUIM_List_CreateImage               EQU $80429804 ;** V11 **
MUIM_List_DeleteImage               EQU $80420f58 ;** V11 **
MUIM_List_Exchange                  EQU $8042468c ;** V4  **
MUIM_List_GetEntry                  EQU $804280ec ;** V4  **
MUIM_List_Insert                    EQU $80426c87 ;** V4  **
MUIM_List_InsertSingle              EQU $804254d5 ;** V7  **
MUIM_List_Jump                      EQU $8042baab ;** V4  **
MUIM_List_Move                      EQU $804253c2 ;** V9  **
MUIM_List_NextSelected              EQU $80425f17 ;** V6  **
MUIM_List_Redraw                    EQU $80427993 ;** V4  **
MUIM_List_Remove                    EQU $8042647e ;** V4  **
MUIM_List_Select                    EQU $804252d8 ;** V4  **
MUIM_List_Sort                      EQU $80422275 ;** V4  **
MUIM_List_TestPos                   EQU $80425f48 ;** V11 **

;** Attributes **

MUIA_List_Active                    EQU $8042391c ;** V4  isg LONG              **
MUIA_List_AdjustHeight              EQU $8042850d ;** V4  i.. BOOL              **
MUIA_List_AdjustWidth               EQU $8042354a ;** V4  i.. BOOL              **
MUIA_List_AutoVisible               EQU $8042a445 ;** V11 isg BOOL              **
MUIA_List_CompareHook               EQU $80425c14 ;** V4  is. struct Hook *     **
MUIA_List_ConstructHook             EQU $8042894f ;** V4  is. struct Hook *     **
MUIA_List_DestructHook              EQU $804297ce ;** V4  is. struct Hook *     **
MUIA_List_DisplayHook               EQU $8042b4d5 ;** V4  is. struct Hook *     **
MUIA_List_DragSortable              EQU $80426099 ;** V11 isg BOOL              **
MUIA_List_DropMark                  EQU $8042aba6 ;** V11 ..g LONG              **
MUIA_List_Entries                   EQU $80421654 ;** V4  ..g LONG              **
MUIA_List_First                     EQU $804238d4 ;** V4  ..g LONG              **
MUIA_List_Format                    EQU $80423c0a ;** V4  isg STRPTR            **
MUIA_List_InsertPosition            EQU $8042d0cd ;** V9  ..g LONG              **
MUIA_List_MinLineHeight             EQU $8042d1c3 ;** V4  i.. LONG              **
MUIA_List_MultiTestHook             EQU $8042c2c6 ;** V4  is. struct Hook *     **
MUIA_List_Pool                      EQU $80423431 ;** V13 i.. APTR              **
MUIA_List_PoolPuddleSize            EQU $8042a4eb ;** V13 i.. ULONG             **
MUIA_List_PoolThreshSize            EQU $8042c48c ;** V13 i.. ULONG             **
MUIA_List_Quiet                     EQU $8042d8c7 ;** V4  .s. BOOL              **
MUIA_List_ShowDropMarks             EQU $8042c6f3 ;** V11 isg BOOL              **
MUIA_List_SourceArray               EQU $8042c0a0 ;** V4  i.. APTR              **
MUIA_List_Title                     EQU $80423e66 ;** V6  isg char *            **
MUIA_List_Visible                   EQU $8042191f ;** V4  ..g LONG              **

MUIV_List_Active_Off                EQU -1
MUIV_List_Active_Top                EQU -2
MUIV_List_Active_Bottom             EQU -3
MUIV_List_Active_Up                 EQU -4
MUIV_List_Active_Down               EQU -5
MUIV_List_Active_PageUp             EQU -6
MUIV_List_Active_PageDown           EQU -7
MUIV_List_ConstructHook_String      EQU -1
MUIV_List_DestructHook_String       EQU -1


;****************************************************************************
;** Floattext                                                              **
;****************************************************************************

;** Attributes **

MUIA_Floattext_Justify              EQU $8042dc03 ;** V4  isg BOOL              **
MUIA_Floattext_SkipChars            EQU $80425c7d ;** V4  is. STRPTR            **
MUIA_Floattext_TabSize              EQU $80427d17 ;** V4  is. LONG              **
MUIA_Floattext_Text                 EQU $8042d16a ;** V4  isg STRPTR            **



;****************************************************************************
;** Volumelist                                                             **
;****************************************************************************


;****************************************************************************
;** Scrmodelist                                                            **
;****************************************************************************

;** Attributes **




;****************************************************************************
;** Dirlist                                                                **
;****************************************************************************

;** Methods **

MUIM_Dirlist_ReRead                 EQU $80422d71 ;** V4  **

;** Attributes **

MUIA_Dirlist_AcceptPattern          EQU $8042760a ;** V4  is. STRPTR            **
MUIA_Dirlist_Directory              EQU $8042ea41 ;** V4  isg STRPTR            **
MUIA_Dirlist_DrawersOnly            EQU $8042b379 ;** V4  is. BOOL              **
MUIA_Dirlist_FilesOnly              EQU $8042896a ;** V4  is. BOOL              **
MUIA_Dirlist_FilterDrawers          EQU $80424ad2 ;** V4  is. BOOL              **
MUIA_Dirlist_FilterHook             EQU $8042ae19 ;** V4  is. struct Hook *     **
MUIA_Dirlist_MultiSelDirs           EQU $80428653 ;** V6  is. BOOL              **
MUIA_Dirlist_NumBytes               EQU $80429e26 ;** V4  ..g LONG              **
MUIA_Dirlist_NumDrawers             EQU $80429cb8 ;** V4  ..g LONG              **
MUIA_Dirlist_NumFiles               EQU $8042a6f0 ;** V4  ..g LONG              **
MUIA_Dirlist_Path                   EQU $80426176 ;** V4  ..g STRPTR            **
MUIA_Dirlist_RejectIcons            EQU $80424808 ;** V4  is. BOOL              **
MUIA_Dirlist_RejectPattern          EQU $804259c7 ;** V4  is. STRPTR            **
MUIA_Dirlist_SortDirs               EQU $8042bbb9 ;** V4  is. LONG              **
MUIA_Dirlist_SortHighLow            EQU $80421896 ;** V4  is. BOOL              **
MUIA_Dirlist_SortType               EQU $804228bc ;** V4  is. LONG              **
MUIA_Dirlist_Status                 EQU $804240de ;** V4  ..g LONG              **

MUIV_Dirlist_SortDirs_First         EQU 0
MUIV_Dirlist_SortDirs_Last          EQU 1
MUIV_Dirlist_SortDirs_Mix           EQU 2
MUIV_Dirlist_SortType_Name          EQU 0
MUIV_Dirlist_SortType_Date          EQU 1
MUIV_Dirlist_SortType_Size          EQU 2
MUIV_Dirlist_Status_Invalid         EQU 0
MUIV_Dirlist_Status_Reading         EQU 1
MUIV_Dirlist_Status_Valid           EQU 2


;****************************************************************************
;** Numeric                                                                **
;****************************************************************************

;** Methods **

MUIM_Numeric_Decrease               EQU $804243a7 ;** V11 **
MUIM_Numeric_Increase               EQU $80426ecd ;** V11 **
MUIM_Numeric_ScaleToValue           EQU $8042032c ;** V11 **
MUIM_Numeric_SetDefault             EQU $8042ab0a ;** V11 **
MUIM_Numeric_Stringify              EQU $80424891 ;** V11 **
MUIM_Numeric_ValueToScale           EQU $80423e4f ;** V11 **

;** Attributes **

MUIA_Numeric_Default                EQU $804263e8 ;** V11 isg LONG              **
MUIA_Numeric_Format                 EQU $804263e9 ;** V11 isg STRPTR            **
MUIA_Numeric_Max                    EQU $8042d78a ;** V11 isg LONG              **
MUIA_Numeric_Min                    EQU $8042e404 ;** V11 isg LONG              **
MUIA_Numeric_Reverse                EQU $8042f2a0 ;** V11 isg BOOL              **
MUIA_Numeric_RevLeftRight           EQU $804294a7 ;** V11 isg BOOL              **
MUIA_Numeric_RevUpDown              EQU $804252dd ;** V11 isg BOOL              **
MUIA_Numeric_Value                  EQU $8042ae3a ;** V11 isg LONG              **



;****************************************************************************
;** Framedisplay                                                           **
;****************************************************************************

;** Attributes **




;****************************************************************************
;** Popframe                                                               **
;****************************************************************************


;****************************************************************************
;** Imagedisplay                                                           **
;****************************************************************************

;** Attributes **




;****************************************************************************
;** Popimage                                                               **
;****************************************************************************


;****************************************************************************
;** Pendisplay                                                             **
;****************************************************************************

;** Methods **

MUIM_Pendisplay_SetColormap         EQU $80426c80 ;** V13 **
MUIM_Pendisplay_SetMUIPen           EQU $8042039d ;** V13 **
MUIM_Pendisplay_SetRGB              EQU $8042c131 ;** V13 **

;** Attributes **

MUIA_Pendisplay_Pen                 EQU $8042a748 ;** V13 ..g Object *          **
MUIA_Pendisplay_Reference           EQU $8042dc24 ;** V13 isg Object *          **
MUIA_Pendisplay_RGBcolor            EQU $8042a1a9 ;** V11 isg struct MUI_RBBcolor * **
MUIA_Pendisplay_Spec                EQU $8042a204 ;** V11 isg struct MUI_PenSpec  * **



;****************************************************************************
;** Poppen                                                                 **
;****************************************************************************


;****************************************************************************
;** Knob                                                                   **
;****************************************************************************


;****************************************************************************
;** Levelmeter                                                             **
;****************************************************************************

;** Attributes **

MUIA_Levelmeter_Label               EQU $80420dd5 ;** V11 isg STRPTR            **



;****************************************************************************
;** Numericbutton                                                          **
;****************************************************************************


;****************************************************************************
;** Slider                                                                 **
;****************************************************************************

;** Attributes **

MUIA_Slider_Horiz                   EQU $8042fad1 ;** V11 isg BOOL              **
MUIA_Slider_Level                   EQU $8042ae3a ;** V4  isg LONG              **
MUIA_Slider_Max                     EQU $8042d78a ;** V4  isg LONG              **
MUIA_Slider_Min                     EQU $8042e404 ;** V4  isg LONG              **
MUIA_Slider_Quiet                   EQU $80420b26 ;** V6  i.. BOOL              **
MUIA_Slider_Reverse                 EQU $8042f2a0 ;** V4  isg BOOL              **



;****************************************************************************
;** Group                                                                  **
;****************************************************************************

;** Methods **

MUIM_Group_ExitChange               EQU $8042d1cc ;** V11 **
MUIM_Group_InitChange               EQU $80420887 ;** V11 **

;** Attributes **

MUIA_Group_ActivePage               EQU $80424199 ;** V5  isg LONG              **
MUIA_Group_Child                    EQU $804226e6 ;** V4  i.. Object *          **
MUIA_Group_ChildList                EQU $80424748 ;** V4  ..g struct List *     **
MUIA_Group_Columns                  EQU $8042f416 ;** V4  is. LONG              **
MUIA_Group_Horiz                    EQU $8042536b ;** V4  i.. BOOL              **
MUIA_Group_HorizSpacing             EQU $8042c651 ;** V4  isg LONG              **
MUIA_Group_LayoutHook               EQU $8042c3b2 ;** V11 i.. struct Hook *     **
MUIA_Group_PageMode                 EQU $80421a5f ;** V5  i.. BOOL              **
MUIA_Group_Rows                     EQU $8042b68f ;** V4  is. LONG              **
MUIA_Group_SameHeight               EQU $8042037e ;** V4  i.. BOOL              **
MUIA_Group_SameSize                 EQU $80420860 ;** V4  i.. BOOL              **
MUIA_Group_SameWidth                EQU $8042b3ec ;** V4  i.. BOOL              **
MUIA_Group_Spacing                  EQU $8042866d ;** V4  is. LONG              **
MUIA_Group_VertSpacing              EQU $8042e1bf ;** V4  isg LONG              **

MUIV_Group_ActivePage_First         EQU 0
MUIV_Group_ActivePage_Last          EQU -1
MUIV_Group_ActivePage_Prev          EQU -2
MUIV_Group_ActivePage_Next          EQU -3
MUIV_Group_ActivePage_Advance       EQU -4


;****************************************************************************
;** Mccprefs                                                               **
;****************************************************************************


;****************************************************************************
;** Register                                                               **
;****************************************************************************

;** Attributes **

MUIA_Register_Frame                 EQU $8042349b ;** V7  i.g BOOL              **
MUIA_Register_Titles                EQU $804297ec ;** V7  i.g STRPTR *          **



;****************************************************************************
;** Settingsgroup                                                          **
;****************************************************************************

;** Methods **


;** Attributes **




;****************************************************************************
;** Settings                                                               **
;****************************************************************************

;** Methods **


;** Attributes **




;****************************************************************************
;** Frameadjust                                                            **
;****************************************************************************

;** Methods **


;** Attributes **




;****************************************************************************
;** Penadjust                                                              **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Penadjust_PSIMode              EQU $80421cbb ;** V11 i.. BOOL              **



;****************************************************************************
;** Imageadjust                                                            **
;****************************************************************************

;** Methods **


;** Attributes **




;****************************************************************************
;** Virtgroup                                                              **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Virtgroup_Height               EQU $80423038 ;** V6  ..g LONG              **
MUIA_Virtgroup_Input                EQU $80427f7e ;** V11 i.. BOOL              **
MUIA_Virtgroup_Left                 EQU $80429371 ;** V6  isg LONG              **
MUIA_Virtgroup_Top                  EQU $80425200 ;** V6  isg LONG              **
MUIA_Virtgroup_Width                EQU $80427c49 ;** V6  ..g LONG              **



;****************************************************************************
;** Scrollgroup                                                            **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Scrollgroup_Contents           EQU $80421261 ;** V4  i.. Object *          **
MUIA_Scrollgroup_FreeHoriz          EQU $804292f3 ;** V9  i.. BOOL              **
MUIA_Scrollgroup_FreeVert           EQU $804224f2 ;** V9  i.. BOOL              **
MUIA_Scrollgroup_UseWinBorder       EQU $804284c1 ;** V13 i.. BOOL              **



;****************************************************************************
;** Scrollbar                                                              **
;****************************************************************************

;** Attributes **

MUIA_Scrollbar_Type                 EQU $8042fb6b ;** V11 i.. LONG              **

MUIV_Scrollbar_Type_Default         EQU 0
MUIV_Scrollbar_Type_Bottom          EQU 1
MUIV_Scrollbar_Type_Top             EQU 2
MUIV_Scrollbar_Type_Sym             EQU 3


;****************************************************************************
;** Listview                                                               **
;****************************************************************************

;** Attributes **

MUIA_Listview_ClickColumn           EQU $8042d1b3 ;** V7  ..g LONG              **
MUIA_Listview_DefClickColumn        EQU $8042b296 ;** V7  isg LONG              **
MUIA_Listview_DoubleClick           EQU $80424635 ;** V4  i.g BOOL              **
MUIA_Listview_DragType              EQU $80425cd3 ;** V11 isg LONG              **
MUIA_Listview_Input                 EQU $8042682d ;** V4  i.. BOOL              **
MUIA_Listview_List                  EQU $8042bcce ;** V4  i.g Object *          **
MUIA_Listview_MultiSelect           EQU $80427e08 ;** V7  i.. LONG              **
MUIA_Listview_ScrollerPos           EQU $8042b1b4 ;** V10 i.. BOOL              **
MUIA_Listview_SelectChange          EQU $8042178f ;** V4  ..g BOOL              **

MUIV_Listview_DragType_None         EQU 0
MUIV_Listview_DragType_Immediate    EQU 1
MUIV_Listview_MultiSelect_None      EQU 0
MUIV_Listview_MultiSelect_Default   EQU 1
MUIV_Listview_MultiSelect_Shifted   EQU 2
MUIV_Listview_MultiSelect_Always    EQU 3
MUIV_Listview_ScrollerPos_Default   EQU 0
MUIV_Listview_ScrollerPos_Left      EQU 1
MUIV_Listview_ScrollerPos_Right     EQU 2
MUIV_Listview_ScrollerPos_None      EQU 3


;****************************************************************************
;** Radio                                                                  **
;****************************************************************************

;** Attributes **

MUIA_Radio_Active                   EQU $80429b41 ;** V4  isg LONG              **
MUIA_Radio_Entries                  EQU $8042b6a1 ;** V4  i.. STRPTR *          **



;****************************************************************************
;** Cycle                                                                  **
;****************************************************************************

;** Attributes **

MUIA_Cycle_Active                   EQU $80421788 ;** V4  isg LONG              **
MUIA_Cycle_Entries                  EQU $80420629 ;** V4  i.. STRPTR *          **

MUIV_Cycle_Active_Next              EQU -1
MUIV_Cycle_Active_Prev              EQU -2


;****************************************************************************
;** Coloradjust                                                            **
;****************************************************************************

;** Methods **


;** Attributes **

MUIA_Coloradjust_Blue               EQU $8042b8a3 ;** V4  isg ULONG             **
MUIA_Coloradjust_Green              EQU $804285ab ;** V4  isg ULONG             **
MUIA_Coloradjust_ModeID             EQU $8042ec59 ;** V4  isg ULONG             **
MUIA_Coloradjust_Red                EQU $80420eaa ;** V4  isg ULONG             **
MUIA_Coloradjust_RGB                EQU $8042f899 ;** V4  isg ULONG *           **



;****************************************************************************
;** Palette                                                                **
;****************************************************************************

;** Attributes **

MUIA_Palette_Entries                EQU $8042a3d8 ;** V6  i.g struct MUI_Palette_Entry * **
MUIA_Palette_Groupable              EQU $80423e67 ;** V6  isg BOOL              **
MUIA_Palette_Names                  EQU $8042c3a2 ;** V6  isg char **           **



;****************************************************************************
;** Popstring                                                              **
;****************************************************************************

;** Methods **

MUIM_Popstring_Close                EQU $8042dc52 ;** V7  **
MUIM_Popstring_Open                 EQU $804258ba ;** V7  **

;** Attributes **

MUIA_Popstring_Button               EQU $8042d0b9 ;** V7  i.g Object *          **
MUIA_Popstring_CloseHook            EQU $804256bf ;** V7  isg struct Hook *     **
MUIA_Popstring_OpenHook             EQU $80429d00 ;** V7  isg struct Hook *     **
MUIA_Popstring_String               EQU $804239ea ;** V7  i.g Object *          **
MUIA_Popstring_Toggle               EQU $80422b7a ;** V7  isg BOOL              **



;****************************************************************************
;** Popobject                                                              **
;****************************************************************************

;** Attributes **

MUIA_Popobject_Follow               EQU $80424cb5 ;** V7  isg BOOL              **
MUIA_Popobject_Light                EQU $8042a5a3 ;** V7  isg BOOL              **
MUIA_Popobject_Object               EQU $804293e3 ;** V7  i.g Object *          **
MUIA_Popobject_ObjStrHook           EQU $8042db44 ;** V7  isg struct Hook *     **
MUIA_Popobject_StrObjHook           EQU $8042fbe1 ;** V7  isg struct Hook *     **
MUIA_Popobject_Volatile             EQU $804252ec ;** V7  isg BOOL              **
MUIA_Popobject_WindowHook           EQU $8042f194 ;** V9  isg struct Hook *     **



;****************************************************************************
;** Poplist                                                                **
;****************************************************************************

;** Attributes **

MUIA_Poplist_Array                  EQU $8042084c ;** V8  i.. char **           **



;****************************************************************************
;** Popscreen                                                              **
;****************************************************************************

;** Attributes **




;****************************************************************************
;** Popasl                                                                 **
;****************************************************************************

;** Attributes **

MUIA_Popasl_Active                  EQU $80421b37 ;** V7  ..g BOOL              **
MUIA_Popasl_StartHook               EQU $8042b703 ;** V7  isg struct Hook *     **
MUIA_Popasl_StopHook                EQU $8042d8d2 ;** V7  isg struct Hook *     **
MUIA_Popasl_Type                    EQU $8042df3d ;** V7  i.g ULONG             **



;****************************************************************************
;** Semaphore                                                              **
;****************************************************************************

;** Methods **

MUIM_Semaphore_Attempt              EQU $80426ce2 ;** V11 **
MUIM_Semaphore_AttemptShared        EQU $80422551 ;** V11 **
MUIM_Semaphore_Obtain               EQU $804276f0 ;** V11 **
MUIM_Semaphore_ObtainShared         EQU $8042ea02 ;** V11 **
MUIM_Semaphore_Release              EQU $80421f2d ;** V11 **


;****************************************************************************
;** Applist                                                                **
;****************************************************************************

;** Methods **



;****************************************************************************
;** Dataspace                                                              **
;****************************************************************************

;** Methods **

MUIM_Dataspace_Add                  EQU $80423366 ;** V11 **
MUIM_Dataspace_Clear                EQU $8042b6c9 ;** V11 **
MUIM_Dataspace_Merge                EQU $80423e2b ;** V11 **
MUIM_Dataspace_ReadIFF              EQU $80420dfb ;** V11 **
MUIM_Dataspace_Remove               EQU $8042dce1 ;** V11 **
MUIM_Dataspace_WriteIFF             EQU $80425e8e ;** V11 **

;** Attributes **

MUIA_Dataspace_Pool                 EQU $80424cf9 ;** V11 i.. APTR              **



;****************************************************************************
;** Configdata                                                             **
;****************************************************************************

;** Methods **


;** Attributes **




;*******************************************
;** End of automatic header file creation **
;*******************************************




****************************************************************************
**
** Macro Section
** -------------
**
** To make GUI creation more easy and understandable, you can use the
** macros below. If you dont want, just define MUI_NOSHORTCUTS to disable
** them.
**
** These macros are available to C programmers only.
**
*** NOTE: This .i file contains the corresponding macros for
*** assembler programmers.  All assembler related comments are
*** marked with three *'s.  The original comments and examples for
*** C are still intact.
**
****************************************************************************

   IFND MUI_NOSHORTCUTS



****************************************************************************
**
** Object Generation
** -----------------
**
** The xxxObject (and xChilds) macros generate new instances of MUI classes.
** Every xxxObject can be followed by tagitems specifying initial create
** time attributes for the new object and must be terminated with the
** End macro:
**
** obj = StringObject,
**          MUIA_String_Contents, "foo",
**          MUIA_String_MaxLen  , 40,
**          End;
**
** With the Child, SubWindow and WindowContents shortcuts you can
** construct a complete GUI within one command:
**
** app = ApplicationObject,
**
**          ...
**
**          SubWindow, WindowObject,
**             WindowContents, VGroup,
**                Child, String("foo",40),
**                Child, String("bar",50),
**                Child, HGroup,
**                   Child, CheckMark(TRUE),
**                   Child, CheckMark(FALSE),
**                   End,
**                End,
**             End,
**
**          SubWindow, WindowObject,
**             WindowContents, HGroup,
**                Child, ...,
**                Child, ...,
**                End,
**             End,
**
**          ...
**
**          End;
**
****************************************************************************


****************************************************************************
***
*** These assembler macros behave somewhat in the same way as the C macros
*** but with some minor differences:
*** The macro names End, and SET are already in use in most assembler
*** compilers, so they are replaced with Endi and seti (for consistencys
*** sake get is also renamed to geti).
***
*** You must provide memory for all the taglists needed in the object
*** creation.  The maximum memory needed is passed to you in the 
*** TAG_SPACE variable.  This is not the mimimum memory needed in most
*** cases and is often a few kilos too large, but this is the best I
*** could come up with the assembler macro commands.
*** Note that you must store the value of TAG_SPACE only after all
*** the objects are created.  TAG_SPACE is incremented as object
*** creation macros are called and in the end holds the maximum
*** theoretical tagitem space usage in bytes.  You pass the pointer to
*** this memory (which you have yourself allocated) in the register MR.
*** You can EQUR MR to any of the registers a3, a4 or a5 (the macros
*** don't use these registers).
***
*** All calls to xxxObject and xxxGroup _must_ be finished with an Endi
*** call.  The Endi macro actually calls the MUI_NewObjectA function
*** and places the result object to the taglist.
***
*** The MUIT macro is just a handy way of moving mixed stuff to the
*** taglist.  Upto 9 items can be moved to the stack on one source line.
*** You can move _only constants_ with the MUIT macro, use the
*** MUIT2 macro to move more mixed stuff (pointers, registers).
*** Remember to use # to denote constants when using MUIT2.
*** The Endi macro is a special case for the MUIT and MUIT2 macros.
*** This is snooped out and every 'MUIT Endi' call is converted to
*** an Endi macro call.
***
*** Also the very common calls 'MUIT Child', 'MUIT SubWindow' and
*** 'MUIT WindowContents' have their own macros Childi, SubWindowi
*** and WindowContentsi.  Childi macro can take upto five arguments.
*** There are three versions of Childi: Childi, Child2 and Child3.
*** Templates for these macros are:
*** Childi [macro[,argument1[,argument2[,argument3[,argument4]]]]]
*** Child2 [macro[,macro[,macro[,macro[,macro]]]]]
*** Child3 [macro[,macro[,argument1[,argument2[,argument3]]]]]
*** Arguments for Childi are interpreted as the first one being a
*** full macro name and the last four arguments for this macro.
*** Arguments for Child2 are treated as macros placed on their
*** own separate lines.  These macros can't have any arguments.
*** Arguments for Child3 are treated as two macros placed on their
*** own separate lines.  The last macro can have three arguments.
***
*** The C example above with these assembler macros:
***
***   ApplicationObject
***   
***      ...
***
***      SubWindowi
***      WindowObject
***         WindowContentsi
***         VGroup
***            Childi String,foo,40
***            Childi String,bar,50
***            Childi HGroup
***               Childi Checkmark,TRUE
***               Childi Checkmark,FALSE
***               Endi
***            Endi
***         Endi
***
***      SubWindowi
***      WindowObject
***         WindowContentsi
***         HGroup
***            Childi
***            ...
***            Childi
***            ...
***            Endi
***         Endi
***
***      ...
***         
***      Endi
***      is app
***
***   app   dc.l  0
***   bar   dc.b  "bar",0
***   foo   dc.b  "foo",0
***
****************************************************************************

*** TAG_SPACE will hold the max taglist size in bytes

TAG_SPACE SET 0

*** Macros to move a tagitem to the taglist if it is given

cmv	MACRO
	IFNC	'\1',''
	IFC	'\1','Endi'
	Endi
	ELSE
	move.l	#\1,(MR)+
TAG_SPACE SET TAG_SPACE+4
	ENDC
	ENDC
	ENDM

cmv2	MACRO
	IFNC	'\1',''
	IFC	'\1','Endi'
	Endi
	ELSE
	move.l	\1,(MR)+
TAG_SPACE SET TAG_SPACE+4
	ENDC
	ENDC
	ENDM

*** Macro to move a tagitem to stack if it is given

cmv3	MACRO
	ifge	NARG-1
	move.l	\1,-(sp)
	ENDC
	ENDM

cmv4	MACRO
	ifge	NARG-1
	move.l	#\1,-(sp)
	ENDC
	ENDM

*** Macros to move max 9 tagitems to the taglist

MUIT	MACRO
	cmv	\1
	cmv	\2
	cmv	\3
	cmv	\4
	cmv	\5
	cmv	\6
	cmv	\7
	cmv	\8
	cmv	\9
	ENDM

MUIT2	MACRO
	cmv2	\1
	cmv2	\2
	cmv2	\3
	cmv2	\4
	cmv2	\5
	cmv2	\6
	cmv2	\7
	cmv2	\8
	cmv2	\9
	ENDM

MakeObj	MACRO
	movem.l	a0/a2,-(a7)
	move.l	a7,a2
	clr.l	-(a7)
	cmv4	\5
	cmv4	\4
	cmv4	\3
	cmv4	\2
	move.l	#\1,d0
	move.l	a7,a0
	CALLMUI	MUI_MakeObjectA
	move.l	a2,a7
	movem.l	(a7)+,a0/a2
	cmv2	d0
	ENDM

*** DoMethod macro for easier assembler DoMethod'ing, max 20 tagitems.
*** Note that _DoMethod is defined in amiga.lib, so you must link
*** your own object code with it.
*** Warning ASM-One V1.25 can only use 9 Args !!!

DoMethod    MACRO
	movem.l d1/a0-a1/a6,-(a7)
	move.l  a7,a6
	clr.l   -(a7)
	ifge	NARG-15
	move.l  \f,-(a7)
	endc
	ifge	NARG-14
	move.l  \e,-(a7)
	endc
	ifge	NARG-13
	move.l  \d,-(a7)
	endc
	ifge	NARG-12
	move.l  \c,-(a7)
	endc
	ifge	NARG-11
	move.l  \b,-(a7)
	endc
	ifge	NARG-10
	move.l  \a,-(a7)
	endc
	ifge	NARG-9
	move.l  \9,-(a7)
	endc
	ifge	NARG-8
	move.l  \8,-(a7)
	endc
	ifge	NARG-7
	move.l  \7,-(a7)
	endc
	ifge	NARG-6
	move.l  \6,-(a7)
	endc
	ifge	NARG-5
	move.l  \5,-(a7)
	endc
	ifge	NARG-4
	move.l  \4,-(a7)
	endc
	ifge	NARG-3
	move.l  \3,-(a7)
	endc
	ifge	NARG-2
	move.l  \2,-(a7)
	endc
	ifge	NARG-1
	move.l  \1,-(a7)
	endc
	move.l  a7,a1
	jsr _DoMethod
	move.l  a6,a7
	movem.l (a7)+,d1/a0-a1/a6
	ENDM


*** MUI_Request macro for easier assembler MUI_Request'ing, max
*** 20 tagitems.

MUI_Request MACRO
	movem.l	a0-a4,-(sp)
	move.l	sp,a4
	cmv3	\9
	cmv3	\8
	cmv3	\7
	move.l	a4,a3
	move.l	#\6,a2
	move.l	#\5,a1
	move.l	#\4,a0
	move.l	#\3,d2
	move.l	\2,d1
	move.l	\1,d0
	CALLMUI	MUI_RequestA
	move.l	a4,sp
	movem.l	(sp)+,a0-a4
	ENDM

*** Macro for getting a pointer to an object you just created.
*** This is valid only after an Endi macro.

is	MACRO
	move.l	d0,\1
	ENDM

Endi	MACRO
	clr.l	(MR)+
TAG_SPACE SET TAG_SPACE+4
	move.l	a2,a1
	CALLMUI	MUI_NewObjectA
	move.l	a2,MR
	movem.l	(sp)+,a0/a2
	cmv2	d0
	ENDM

MenustripObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Menustrip_f SET 1
	lea	MUIC_Menustrip,a0
	move.l	MR,a2
	ENDM

MenuObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Menu_f SET 1
	lea	MUIC_Menu,a0
	move.l	MR,a2
	ENDM

MenuObjectT MACRO
	movem.l	a0/a2,-(sp)
MUIC_Menu_f SET 1
	lea	MUIC_Menu,a0
	move.l	MR,a2
	MUIT	MUIA_Menu_Title,\1
	ENDM

MenuitemObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Menuitem_f SET 1
	lea	MUIC_Menuitem,a0
	move.l	MR,a2
	ENDM

WindowObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Window_f SET 1
	lea	MUIC_Window,a0
	move.l	MR,a2
	ENDM

ImageObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Image_f SET 1
	lea	MUIC_Image,a0
	move.l	MR,a2
	ENDM

BitmapObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Bitmap_f SET 1
	lea	MUIC_Bitmap,a0
	move.l	MR,a2
	ENDM

BodychunkObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Bodychunk_f SET 1
	lea	MUIC_Bodychunk,a0
	move.l	MR,a2
	ENDM

NotifyObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Notify_f SET 1
	lea	MUIC_Notify,a0
	move.l	MR,a2
	ENDM

ApplicationObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Application_f SET 1
	lea	MUIC_Application,a0
	move.l	MR,a2
	ENDM

TextObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Text_f SET 1
	lea	MUIC_Text,a0
	move.l	MR,a2
	ENDM

RectangleObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Rectangle_f SET 1
	lea	MUIC_Rectangle,a0
	move.l	MR,a2
	ENDM

ListObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_List_f SET 1
	lea	MUIC_List,a0
	move.l	MR,a2
	ENDM

PropObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Prop_f SET 1
	lea	MUIC_Prop,a0
	move.l	MR,a2
	ENDM

StringObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_String_f SET 1
	lea	MUIC_String,a0
	move.l	MR,a2
	ENDM

ScrollbarObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Scrollbar_f SET 1
	lea	MUIC_Scrollbar,a0
	move.l	MR,a2
	ENDM

ListviewObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Listview_f SET 1
	lea	MUIC_Listview,a0
	move.l	MR,a2
	ENDM

RadioObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Radio_f SET 1
	lea	MUIC_Radio,a0
	move.l	MR,a2
	ENDM

VolumelistObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Volumelist_f SET 1
	lea	MUIC_Volumelist,a0
	move.l	MR,a2
	ENDM

FloattextObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Floattext_f SET 1
	lea	MUIC_Floattext,a0
	move.l	MR,a2
	ENDM

DirlistObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Dirlist_f SET 1
	lea	MUIC_Dirlist,a0
	move.l	MR,a2
	ENDM

SliderObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Slider_f SET 1
	lea	MUIC_Slider,a0
	move.l	MR,a2
	ENDM

CycleObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Cycle_f SET 1
	lea	MUIC_Cycle,a0
	move.l	MR,a2
	ENDM

GaugeObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Gauge_f SET 1
	lea	MUIC_Gauge,a0
	move.l	MR,a2
	ENDM

ScaleObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Scale_f SET 1
	lea	MUIC_Scale,a0
	move.l	MR,a2
	ENDM

NumericObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Numeric_f SET 1
	lea	MUIC_Numeric,a0
	move.l	MR,a2
	ENDM

BoopsiObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Boopsi_f SET 1
	lea	MUIC_Boopsi,a0
	move.l	MR,a2
	ENDM

ColorfieldObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Colorfield_f SET 1
	lea	MUIC_Colorfield,a0
	move.l	MR,a2
	ENDM

ColorpanelObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Colorpanel_f SET 1
	lea	MUIC_Colorpanel,a0
	move.l	MR,a2
	ENDM

ColoradjustObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Coloradjust_f SET 1
	lea	MUIC_Coloradjust,a0
	move.l	MR,a2
	ENDM

PaletteObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Palette_f SET 1
	lea	MUIC_Palette,a0
	move.l	MR,a2
	ENDM

GroupObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	ENDM

RegisterObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Register_f SET 1
	lea	MUIC_Register,a0
	move.l	MR,a2
	ENDM

VirtgroupObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	ENDM

ScrollgroupObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Scrollgroup_f SET 1
	lea	MUIC_Scrollgroup,a0
	move.l	MR,a2
	ENDM

PopstringObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Popstring_f SET 1
	lea	MUIC_Popstring,a0
	move.l	MR,a2
	ENDM

PopobjectObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Popobject_f SET 1
	lea	MUIC_Popobject,a0
	move.l	MR,a2
	ENDM

PoplistObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Poplist_f SET 1
	lea	MUIC_Poplist,a0
	move.l	MR,a2
	ENDM

PopaslObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Popasl_f SET 1
	lea	MUIC_Popasl,a0
	move.l	MR,a2
	ENDM

ScrmodelistObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Scrmodelist_f SET 1
	lea	MUIC_Scrmodelist,a0
	move.l	MR,a2
	ENDM

NumericbuttonObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Numericbutton_f SET 1
	lea	MUIC_Numericbutton,a0
	move.l	MR,a2
	ENDM

KnobObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Knob_f SET 1
	lea	MUIC_Knob,a0
	move.l	MR,a2
	ENDM

LevelmeterObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Levelmeter_f SET 1
	lea	MUIC_Levelmeter,a0
	move.l	MR,a2
	ENDM

PenadjustObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Penabjust_f SET 1
	lea	MUIC_Penabjust,a0
	move.l	MR,a2
	ENDM

PoppenObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Poppen_f SET 1
	lea	MUIC_Poppen,a0
	move.l	MR,a2
	ENDM

AboutmuiObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Aboutmui_f SET 1
	lea	MUIC_Aboutmui,a0
	move.l	MR,a2
	ENDM

KeyentryObject MACRO
	movem.l	a0/a2,-(sp)
MUIC_Keyentry_f SET 1
	lea	MUIC_Keyentry,a0
	move.l	MR,a2
	ENDM

VGroup	MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	ENDM

HGroup	MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Horiz,TRUE
	ENDM

ColGroup MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Columns,\1
	ENDM

RowGroup MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Rows,\1
	ENDM

PageGroup MACRO
	movem.l	a0/a2,-(sp)
MUIC_Group_f SET 1
	lea	MUIC_Group,a0
	move.l	MR,a2
	MUIT	MUIA_Group_PageMode,TRUE
	ENDM

VGroupV	MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	ENDM

HGroupV	MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Horiz,TRUE
	ENDM

ColGroupV MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Columns,\1
	ENDM

RowGroupV MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	MUIT	MUIA_Group_Rows,\1
	ENDM

PageGroupV MACRO
	movem.l	a0/a2,-(sp)
MUIC_Virtgroup_f SET 1
	lea	MUIC_Virtgroup,a0
	move.l	MR,a2
	MUIT	MUIA_Group_PageMode,TRUE
	ENDM

RegisterGroup MACRO
	movem.l	a0/a2,-(sp)
MUIC_Register_f SET 1
	lea	MUIC_Register,a0
	move.l	MR,a2
	MUIT	MUIA_Register_Titles,\1
	ENDM

Childi	MACRO
	cmv	MUIA_Group_Child
	\1 \2,\3,\4,\5
	ENDM
Child2	MACRO
	cmv	MUIA_Group_Child
	\1
	\2
	\3
	\4
	\5
	ENDM
Child3	MACRO
	cmv	MUIA_Group_Child
	\1
	\2 \3,\4,\5
	ENDM
SubWindowi MACRO
	cmv	MUIA_Application_Window
	ENDM
WindowContentsi MACRO
	cmv	MUIA_Window_RootObject
	ENDM


Child		EQU MUIA_Group_Child
SubWindow	EQU MUIA_Application_Window
WindowContents	EQU MUIA_Window_RootObject



****************************************************************************
**
** Frame Types
** -----------
**
** These macros may be used to specify one of MUI's different frame types.
** Note that every macro consists of one or more { ti_Tag, ti_Data }
** pairs.
**
** GroupFrameT() is a special kind of frame that contains a centered
** title text.
**
** HGroup, GroupFrameT("Horiz Groups"),
**    Child, RectangleObject, TextFrame  , End,
**    Child, RectangleObject, StringFrame, End,
**    Child, RectangleObject, ButtonFrame, End,
**    Child, RectangleObject, ListFrame  , End,
**    End,
**
****************************************************************************

****************************************************************************
***
*** Assembler version of the above C example:
***
*** HGroup
***    GroupFrameT horizg
***    Child2 RectangleObject,TextFrame,Endi
***    Child2 RectangleObject,StringFrame,Endi
***    Child2 RectangleObject,ButtonFrame,Endi
***    Child2 RectangleObject,ListFrame,Endi
***    Endi
***
*** horizg  dc.b  "Horiz Groups",0
***	even
***
****************************************************************************

*** These macros call MUIT themselves, do not use eg. 'MUIT NoFrame'

NoFrame	MACRO
	MUIT	MUIA_Frame,MUIV_Frame_None
	ENDM
ButtonFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Button
	ENDM
ImageButtonFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_ImageButton
	ENDM
TextFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Text
	ENDM
StringFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_String
	ENDM
ReadListFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_ReadList
	ENDM
InputListFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_InputList
	ENDM
PropFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Prop
	ENDM
GaugeFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Gauge
	ENDM
GroupFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Group
	ENDM
GroupFrameT MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Group,MUIA_Frame_Title,\1
	ENDM
VirtualFrame MACRO
	MUIT	MUIA_Frame,MUIV_Frame_Virtual
	ENDM


****************************************************************************
**
** Spacing Macros
** --------------
**
****************************************************************************

*** For these macros tagitem space is allocated from the stack and is
*** fixed in size.  So, there is no need for a separate Endi call.

HVSpace	MACRO
	move.l	a0,-(sp)
MUIC_Rectangle_f SET 1
	lea	MUIC_Rectangle,a0
	clr.l	-(sp)
	move.l	sp,a1
	CALLMUI	MUI_NewObjectA
	addq.l	#4,sp
	move.l	(sp)+,a0
	MUIT2	d0
	ENDM
   
HSpace	MACRO
	MakeObj	MUIO_HSpace,\1
	ENDM

VSpace	MACRO
	MakeObj	MUIO_VSpace,\1
	ENDM

HCenter	MACRO
	HGroup
	GroupSpacing	0
	Childi	HSpace,0
	Childi	MUIT2,\1
	Childi	HSpace,0
	Endi
	ENDM

VCenter	MACRO
	VGroup
	GroupSpacing	0
	Childi	VSpace,0
	Childi	MUIT2,\1
	Childi	VSpace,0
	Endi
	ENDM

InnerSpacing MACRO
	MUIT	MUIA_InnerLeft,\1
	MUIT	MUIA_InnerRight,\1
	MUIT	MUIA_InnerTop,\2
	MUIT	MUIA_InnerBottom,\2
	ENDM

GroupSpacing MACRO
	MUIT	MUIA_Group_Spacing,\1
	ENDM



****************************************************************************
***
*** You use these assembler macros like this:
***
*** String mystr1,40
***
*** CheckMark TRUE
***
*** SimpleButton mysbut1
***
*** KeyButton mykbut1,"c"
***
*** Cycle myentr1
***
*** KeyCycle myentr1,"k"
***
*** Radio rname1,rbuts1
***
*** String mystr1,35
*** is strobj1
*** Popup ST_Font,strobj1,MyHook,MUII_Popup
***
***
*** MyHook  rts   ; dummy hook, does nothing
*** mysrt1  dc.b  "String contents",0
***	even
*** mysbut1 dc.b  "Button",0
***	even
*** mykbut1 dc.b  "Cancel",0
***	even
*** myentr1 dc.l  entrs1,entrs2,entrs3,NULL
*** entrs1  dc.b  "One",0
*** entrs2  dc.b  "Two",0
*** entrs3  dc.b  "Three",0
***	even
*** rname1  dc.b  "Radio Buttons:",0
***	even
*** rbuts1  dc.l  rbut1,rbut2,rbut3,NULL
*** rbut1   dc.b  "Button1",0
***	even
*** rbut2   dc.b  "Button2",0
***	even
*** rbut3   dc.b  "Button3",0
***	even
*** strobj  dc.l  0
*** ST_Font dc.l  0
***
***
****************************************************************************

****************************************************************************
**
** String-Object
** -------------
**
** The following macro creates a simple string gadget.
**
****************************************************************************

String	MACRO
	StringObject
	StringFrame
	MUIT	MUIA_String_MaxLen,\2
	MUIT	MUIA_String_Contents,\1
	Endi
	ENDM

KeyString MACRO
	StringObject
	StringFrame
	MUIT	MUIA_ControlChar,\3
	MUIT	MUIA_String_MaxLen,\2
	MUIT	MUIA_String_Contents,\1
	Endi
	ENDM

****************************************************************************
**
** CheckMark-Object
** ----------------
**
** The following macro creates a checkmark gadget.
**
****************************************************************************

CheckMark MACRO
	ImageObject
	ImageButtonFrame
	MUIT	MUIA_InputMode,MUIV_InputMode_Toggle
	MUIT	MUIA_Image_Spec,MUII_CheckMark
	MUIT	MUIA_Image_FreeVert,TRUE
	MUIT	MUIA_Selected,\1
	MUIT	MUIA_Background,MUII_ButtonBack
	MUIT	MUIA_ShowSelState,FALSE
	Endi
	ENDM

KeyCheckMark MACRO
	ImageObject
	ImageButtonFrame
	MUIT	MUIA_InputMode,MUIV_InputMode_Toggle
	MUIT	MUIA_Image_Spec,MUII_CheckMark
	MUIT	MUIA_Image_FreeVert,TRUE
	MUIT	MUIA_Selected,\1
	MUIT	MUIA_Background,MUII_ButtonBack
	MUIT	MUIA_ShowSelState,FALSE
	MUIT	MUIA_ControlChar,\2
	Endi
	ENDM


****************************************************************************
**
** Button-Objects
** --------------
**
** Note: Use small letters for KeyButtons, e.g.
**       KeyButton("Cancel",'c')  and not  KeyButton("Cancel",'C') !!
**
****************************************************************************

SimpleButton MACRO
	MakeObj	MUIO_Button,\1
	ENDM

KeyButton MACRO
	TextObject
	ButtonFrame
	MUIT	MUIA_Text_Contents,\1
	MUIT	MUIA_Text_PreParse,PreParse
	MUIT	MUIA_Text_SetMax,FALSE
	MUIT	MUIA_Text_HiChar,\2
	MUIT	MUIA_ControlChar,\2
	MUIT	MUIA_InputMode,MUIV_InputMode_RelVerify
	MUIT	MUIA_Background,MUII_ButtonBack
	Endi
	ENDM


****************************************************************************
**
** Cycle-Object
** ------------
**
****************************************************************************

Cycle	MACRO
	CycleObject
	MUIT	MUIA_Cycle_Entries,\1
	Endi
	ENDM

KeyCycle MACRO
	CycleObject
	MUIT	MUIA_Cycle_Entries,\1
	MUIT	MUIA_ControlChar,\2
	Endi
	ENDM



****************************************************************************
**
** Popup-Object
** ------------
**
** An often needed GUI element is a string gadget with a little button
** that opens up a (small) window with a list containing possible entries
** for this gadget. Together with the Popup and the String macro,
** such a thing would look like
**
** VGroup,
**    Child, Popup(ST_Font, String("helvetica/13",32), &Hook, MUII_Popup),
**    ...,
**
** ST_Font will hold a pointer to the embedded string gadget and can
** be used to set and get its contents as with every other string object.
**
** The hook will be called with the string gadget as object whenever
** the user releases the popup button and could look like this:
**
** ULONG __asm __saveds HookFunc(register __a2 APTR obj,MUII_File)
** {
**    ...
**
**    // put our application to sleep while displaying the requester
**    set(Application,MUIA_Application_Sleep,TRUE);
**
**    // get the calling objects window and position
**    get(obj,MUIA_Window  ,&window);
**    get(obj,MUIA_LeftEdge,&l);
**    get(obj,MUIA_TopEdge ,&t);
**    get(obj,MUIA_Width   ,&w);
**    get(obj,MUIA_Height  ,&h);
**
**    if (req=MUI_AllocAslRequestTags(ASL_FontRequest,TAG_DONE))
**    {
**       if (MUI_AslRequestTags(req,
**	 ASLFO_Window	,window,
**	 ASLFO_PrivateIDCMP   ,TRUE,
**	 ASLFO_TitleText      ,"Select Font",
**	 ASLFO_InitialLeftEdge,window->LeftEdge + l,
**	 ASLFO_InitialTopEdge ,window->TopEdge  + t+h,
**	 ASLFO_InitialWidth   ,w,
**	 ASLFO_InitialHeight  ,250,
**	 TAG_DONE))
**       {
**	 // set the new contents for our string gadget
**	 set(obj,MUIA_String_Contents,req->fo_Attr.ta_Name);
**       }
**       MUI_FreeAslRequest(req);
**    }
**
**    // wake up our application again
**    set(Application,MUIA_Application_Sleep,FALSE);
**
**    return(0);
** }
**
** Note: This macro needs a "APTR dummy;" declaration somewhere in your
**       code to work.
**
****************************************************************************

PopButton MACRO
	MakeObj	MUIO_PopButton,\1
	ENDM


****************************************************************************
**
** Labeling Objects
** ----------------
**
** Labeling objects, e.g. a group of string gadgets,
**
**   Small: |foo   |
**  Normal: |bar   |
**     Big: |foobar|
**    Huge: |barfoo|
**
** is done using a 2 column group:
**
** ColGroup(2),
**    Child, Label2("Small:" ),
**    Child, StringObject, End,
**    Child, Label2("Normal:"),
**    Child, StringObject, End,
**    Child, Label2("Big:"   ),
**    Child, StringObject, End,
**    Child, Label2("Huge:"  ),
**    Child, StringObject, End,
**    End,
**
** Note that we have three versions of the label macro, depending on
** the frame type of the right hand object:
**
** Label1(): For use with standard frames (e.g. checkmarks).
** Label2(): For use with double high frames (e.g. string gadgets).
** Label() : For use with objects without a frame.
**
** These macros ensure that your label will look fine even if the
** user of your application configured some strange spacing values.
** If you want to use your own labeling, you'll have to pay attention
** on this topic yourself.
**
****************************************************************************

****************************************************************************
***
*** And the above C example in assembler:
***
*** ColGroup 2
***   Childi Label2,small
***   Child2 StringObject,Endi
***   Childi Label2,normal
***   Child2 StringObject,Endi
***   Childi Label2,big
***   Child2 StringObject,Endi
***   Childi Label2,huge
***   Child2 StringObject,Endi
***   Endi
***
*** small   dc.b  "Small:",0
***	even
*** normal  dc.b  "Normal:",0
***	even
*** big     dc.b  "Big:",0
***	even
*** huge    dc.b  "Huge:",0
***	even
***
****************************************************************************



; +jl+ name changed because "Symbol was declared twice"
MUILabel	MACRO
	MakeObj	MUIO_Label,\1,0
	ENDM

Label1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_SingleFrame
	ENDM

Label2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_DoubleFrame
	ENDM

LLabel	MACRO
	MakeObj	MUIO_Label,\1,MUIO_LeftAligned
	ENDM

LLabel1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_LeftAligned|MUIO_Label_SingleFrame
	ENDM

LLabel2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_LeftAligned|MUIO_Label_DoubleFrame
	ENDM

CLabel	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Centered
	ENDM

CLabel1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Centered|MUIO_Label_SingleFrame
	ENDM

CLabel2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Centered|MUIO_Label_DoubleFrame
	ENDM

FreeLabel	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert
	ENDM

FreeLabel1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_Label_SingleFrame
	ENDM

FreeLabel2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_Label_DoubleFrame
	ENDM

FreeLLabel	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_LeftAligned
	ENDM

FreeLLabel1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_LeftAligned|MUIO_Label_SingleFrame
	ENDM

FreeLLabel2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_LeftAligned|MUIO_Label_DoubleFrame
	ENDM

FreeCLabel	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_Centered
	ENDM

FreeCLabel1	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_Centered|MUIO_Label_SingleFrame
	ENDM

FreeCLabel2	MACRO
	MakeObj	MUIO_Label,\1,MUIO_Label_FreeVert|MUIO_Centered|MUIO_Label_DoubleFrame
	ENDM

KeyLabel MACRO
	TextObject
	MUIT	MUIA_Text_PreParse,PreParse2
	MUIT	MUIA_Text_Contents,\1
	MUIT	MUIA_Weight,0
	MUIT	MUIA_InnerLeft,0
	MUIT	MUIA_InnerRight,0,
	MUIT	MUIA_Text_HiChar,\2
	Endi
	ENDM

KeyLabel1 MACRO
	TextObject
	ButtonFrame
	MUIT	MUIA_Text_PreParse,PreParse2
	MUIT	MUIA_Text_Contents,\1
	MUIT	MUIA_Weight,0
	MUIT	MUIA_InnerLeft,0
	MUIT	MUIA_InnerRight,0
	MUIT	MUIA_Text_HiChar,\2
	MUIT	MUIA_FramePhantomHoriz,TRUE
	Endi
	ENDM

KeyLabel2 MACRO
	TextObject
	StringFrame
	MUIT	MUIA_Text_PreParse,PreParse2
	MUIT	MUIA_Text_Contents,\1
	MUIT	MUIA_Weight,0
	MUIT	MUIA_InnerLeft,0
	MUIT	MUIA_InnerRight,0
	MUIT	MUIA_Text_HiChar,\2
	MUIT	MUIA_FramePhantomHoriz,TRUE
	Endi
	ENDM


***************************************************************************
**
** Radio-Object
** ------------
**
***************************************************************************

Radio	MACRO
	RadioObject
	GroupFrameT	\1
	MUIT	MUIA_Radio_Entries,\2
	MUIT	MUIA_Background,MUII_GroupBack
	Endi
	ENDM

KeyRadio MACRO
	RadioObject
	GroupFrameT	\1
	MUIT	MUIA_Radio_Entries,\2
	MUIT	MUIA_ControlChar,\3
	MUIT	MUIA_Background,MUII_GroupBack
	Endi
	ENDM    



***************************************************************************
**
** Slider-Object
** -------------
**
***************************************************************************


Slider	MACRO
	SliderObject
	MUIT	MUIA_Numeric_Min,\1
	MUIT	MUIA_Numeric_Max,\2
	MUIT	MUIA_Numeric_Value,\3
	Endi
	ENDM   

KeySlider MACRO
	SliderObject
	MUIT	MUIA_Numeric_Min, \1
	MUIT	MUIA_Numeric_Max, \2
	MUIT	MUIA_Numeric_Value, \3
	MUIT	MUIA_ControlChar, \4
	Endi
	ENDM





****************************************************************************
**
** Controlling Objects
** -------------------
**
** set() and get() are two short stubs for BOOPSI GetAttr() and SetAttrs()
** calls:
**
** {
**    char *x;
**
**    set(obj,MUIA_String_Contents,"foobar");
**    get(obj,MUIA_String_Contents,&x);
**
**    printf("gadget contains '%s'\n",x);
** }
**
****************************************************************************

****************************************************************************
***
*** And the above C example in assembler:
***
*** seti obj,#MUIA_String_Contents,#foobar
*** geti obj,#MUIA_String_Contents,#x
***   move.l   #myfmt,d1
***   move.l   #data,d2
***   CALLDOS VPrintf
***
*** foobar     dc.b  "foobar",0
***	   even
*** data       dc.l  x
*** x	 dcb.b 10
*** myfmt      dc.b  "gadget contains '%s'",10,0
***	   even
***
*** The names of the set and get macros have been changed to seti and geti
*** since most assemblers already have the pseudo op-code SET.
*** Note that seti is designed to take multiple tagitems (max 8).
***
****************************************************************************

geti	MACRO
	move.l	a6,-(a7)
	move.l	\2,d0
	move.l	\1,a0
	move.l	\3,a1
	move.l	_IntuitionBase,a6
	jsr	-654(a6)
	move.l	(a7)+,a6
	ENDM

seti	MACRO
	move.l	a6,-(sp)
	move.l	sp,a6
	clr.l	-(sp)
	cmv3	\3
	cmv3	\2
	move.l	\1,a0
	move.l	sp,a1
	move.l	a6,-(a7)
	move.l	_IntuitionBase,a6
	jsr	-648(a6)
	move.l	(a7)+,a6
	move.l	a6,sp
	move.l	(sp)+,a6
	ENDM

nnseti	MACRO
	movem.l	a2/a6,-(sp)
	move.l	sp,a2
	clr.l	-(sp)
	cmv3	\3
	cmv3	\2
	cmv3	#TRUE
	cmv3	#MUIA_NoNotify
	move.l	\1,a0
	move.l	sp,a1
	move.l	_IntuitionBase,a6
	jsr	-648(a6)
	move.l	a2,sp
	movem.l	(sp)+,a2/a6
	ENDM

setmutex MACRO
	seti	\1,#MUIA_Radio_Active,\2
	ENDM
setcycle MACRO
	seti	\1,#MUIA_Cycle_Active,\2
	ENDM
setstring MACRO
	seti	\1,#MUIA_String_Contents,\2
	ENDM
setcheckmark MACRO
	seti	\1,#MUIA_Selected,\2
	ENDM
setslider MACRO
	seti	\1,#MUIA_Numeric_Value,\2
	ENDM


   ENDC  ;MUI_NOSHORTCUTS


* End of Include file, for using MUI from an application program. The rest
* of this is for custom class implementors only
*
*****************************************************************************
*****************************************************************************
*****************************************************************************


   ENDC  ;LIBRARIES_MUI_I
