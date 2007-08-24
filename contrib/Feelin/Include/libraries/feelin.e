OPT MODULE
OPT EXPORT,PREPROCESS

/*
**    $VER: feelin.e 10.00 (2005/05/12)
**
**    feelin.library definitions
**
**    © 2000-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

MODULE 'exec/semaphores','exec/libraries','exec/ports',
      'graphics/rastport','graphics/text',
      'intuition/intuition'

CONST    FTRUE = 1
CONST    FV_FEELIN_VERSION = 9

->PROC Module information
/*
   All constants follow these rules :

   FM_<method>          - General method, understood by all classes [NUM]
   FM_<class>_<method>  - Class method                              [NUM,STR]
   FA_<attrib>          - General attribute, known by many classes. [NUM]
   FA_<class>_<attrib>  - Specific class attribute                  [NUM,STR]
   FV_<class>_<value>   - Special attribute value                   [NUM]
   FS_<name>            - Structure of arguments to a method

   FA_<attrib> definitions are followed by a comment [ISG] :
     I : it's possible to specify this attribute at object creation time.
     S : it's possible to change this attribute with F_Set().
     G : it's possible to get this attribute with F_Get().

   FR_<class> = Registered class id (unsigned 24 bits)
   FR_<class>_Mthd = First Method ID of <class>
   FR_<class>_Attr = First Attribute ID of <class>

   Ranges:

   $C0000000 - $CFFFFFFF is reserved for custom classes methods.
   $8C000000 - $8CFFFFFF is reserved for custom classes attributes.

   $D0000000 - DFFFFFFFF is reserved for Dynamic methods.
   $8D000000 - 8DFFFFFFF is reserved for Dynamic attributes.

   $F0000000 - $FFFFFFFF is reserved for Feelin methods.
   $8F000000 - $8FFFFFFF is reserved for Feelin attributes.

   Since v6 of feelin.library most classes use dynamic IDs instead of static
   ones.  Dynamic  IDs  are  generated on the fly at run time, thus they are
   different at each session and future proof. You should  have  a  look  at
   Feelin_DEV.guide to learn more about this.

*/

CONST   FCCM_BASE = $C0000000,
        FCCA_BASE = $8C000000,

        DYNA_MTHD = $D0000000,
        DYNA_ATTR = $8D000000,

        MTHD_BASE = $F0000000,
        ATTR_BASE = $8F000000
->ENDPROC

/*** Misc ******************************************************************/

->PROC Basic structures
OBJECT feelinNode
   next:PTR TO feelinNode
   prev:PTR TO feelinNode
ENDOBJECT

OBJECT feelinList
   head:PTR TO feelinNode
   tail:PTR TO feelinNode
ENDOBJECT

OBJECT feelinListSemaphored OF feelinList
   semaphore:ss
ENDOBJECT

OBJECT feelinNodeData OF feelinNode
   data
ENDOBJECT
->ENDPROC
->PROC SYSTEM
OBJECT feelinMethodEntry
   method
   name:PTR TO CHAR
   id:LONG
ENDOBJECT

OBJECT feelinDynamicEntry
   name:PTR TO CHAR
   id:LONG
ENDOBJECT

/* SYSTEM PUBLIC */

OBJECT feelinbase OF lib
   console

   sys:PTR TO lib
   dos:PTR TO lib
   graphics:PTR TO lib
   intuition:PTR TO lib
   utility:PTR TO lib
   layers:PTR TO lib
   locale:PTR TO lib
/* end of public data */
ENDOBJECT
->ENDPROC
->PROC Macros:
->PROC Macros / Drawing
#define _FPen(o,fp)        SetAPen(rp,_pen(o,fp))
#define _APen(p)           SetAPen(rp,p)
#define _BPen(p)           SetBPen(rp,p)
#define _DrMd(m)           SetDrMd(rp,m)
#define _Plot(x1,y1)       WritePixel(rp,x1,y1)
#define _Move(x,y)         Move(rp,x,y)
#define _Draw(x,y)         Draw(rp,x,y)
#define _Boxf(x1,y1,x2,y2) RectFill(rp,x1,y1,x2,y2)
#define _Text(s,c)         Text(rp,s,c)
->ENDPROC
->PROC Macros / E library patch (Fe_Xxx -> F_Xxx)
#define F_CreatePool    Fe_CreatePool
#define F_DeletePool    Fe_DeletePool
#define F_New           Fe_New
#define F_NewP          Fe_NewP
#define F_Dispose       Fe_Dispose
#define F_OPool         Fe_OPool
#define F_SPool         Fe_SPool
#define F_RPool         Fe_RPool

#define F_LinkTail      Fe_LinkTail
#define F_LinkHead      Fe_LinkHead
#define F_LinkInsert    Fe_LinkInsert
#define F_LinkRemove    Fe_LinkRemove
#define F_LinkMember    Fe_LinkMember
#define F_NextNode      Fe_NextNode
#define F_StrCmp        Fe_StrCmp

#define F_FindClass     Fe_FindClass
#define F_OpenClass     Fe_OpenClass
#define F_CloseClass    Fe_CloseClass
#define F_CreateClassA  Fe_CreateClassA
#define F_DeleteClass   Fe_DeleteClass

#define F_DoA           Fe_DoA
#define F_ClassDoA      Fe_ClassDoA
#define F_SuperDoA      Fe_SuperDoA

#define F_NewObjA       Fe_NewObjA
#define F_MakeObjA      Fe_MakeObjA
#define F_DisposeObj    Fe_DisposeObj

#define F_Set              Fe_Set
#define F_Get              Fe_Get
#define F_Draw             Fe_Draw
#define F_DrawBackground   Fe_DrawBackground
#define F_AskMinMax        Fe_AskMinMax
#define F_Layout           Fe_Layout
#define F_ModifyHandler    Fe_ModifyHandler

#define F_AlertA              Fe_AlertA
#define F_DebugOutA           Fe_DebugOutA

#define F_OpenServer          Fe_OpenServer
#define F_CloseServer         Fe_CloseServer
#define F_CreateServerA       Fe_CreateServerA
#define F_DeleteServer        Fe_DeleteServer
#define F_SendServerMessageA  Fe_SendServerMessageA

#define F_DynamicFindID          Fe_DynamicFindID
#define F_DynamicResolveTable    Fe_DynamicResolveTable
#define F_DynamicNTI             Fe_DynamicNTI
#define F_DynamicFTI             Fe_DynamicFTI
#define F_DynamicGTD             Fe_DynamicGTD
->ENDPROC
->PROC Macros / Object Generation
ENUM FV_MakeObj_Label = 1,                       -> label:PTR TO CHAR, flags:LONG
     FV_MakeObj_Button,                          -> label:PTR TO CHAR
     FV_MakeObj_BarTitle,                        -> label:PTR TO CHAR
     FV_MakeObj_Gauge,                           -> Horiz:BOOL,LONG Min, LONG Max, LONG Value
     FV_MakeObj_Slider,                          -> Horiz:BOOL,LONG Min, LONG Max, LONG Value
     FV_MakeObj_Prop,                            -> Horiz:BOOL,LONG entries, LONG visible, LONG first
     FV_MakeObj_String,                          -> label:PTR TO CHAR,maxlen:LONG
     FV_MakeObj_Checkbox                         -> selected:LONG

#define SimpleButton(l)       F_MakeObjA(FV_MakeObj_Button,[l,TAG_DONE])
#define BarTitle(t)           F_MakeObjA(FV_MakeObj_BarTitle,[t,TAG_DONE])
#define Gauge(h,mi,ma,v)      F_MakeObjA(FV_MakeObj_Gauge,[h,mi,ma,v,TAG_DONE])
#define Slider(h,mi,ma,v)     F_MakeObjA(FV_MakeObj_Slider,[h,mi,ma,v,TAG_DONE])
#define Prop(h,e,v,f)         F_MakeObjA(FV_MakeObj_Prop,[h,e,v,f,TAG_DONE])
#define String_(s,l)          F_MakeObjA(FV_MakeObj_String,[s,l,TAG_DONE])
#define Checkbox(sel)         F_MakeObjA(FV_MakeObj_Checkbox,[sel,TAG_DONE])
 
#define Child           FA_Child
#define End             NIL])
->ENDPROC
->PROC Macros / ASM
#define SaveSP(x) MOVE.L   x,-(A7)
#define LoadSP(x) MOVE.L   (A7)+,x
->ENDPROC
->ENDPROC :Macros

->PROC feelin.library / F_CreatePoolA()

ENUM  FA_Pool_Attributes = ATTR_BASE,
     FA_Pool_ItemSize,
     FA_Pool_Items,
     FA_Pool_Name,
     FA_Pool_Public

->ENDPROC

/*** Low-level Objects *****************************************************/

->PROC FC_Object                          $000000
#define FC_Object 'Object'
#define FObject                                 LONG

CONST FR_Object            = $000000
CONST FR_Object_MTHD       = MTHD_BASE OR FR_Object,
     FR_Object_ATTR       = ATTR_BASE OR FR_Object

ENUM  FM_New                                  = FR_Object_MTHD,
     FM_Dispose,
     FM_Get,
     FM_Set,
     FM_Notify,
     FM_UnNotify,
     FM_CallHook,
     FM_CallHookEntry,
     FM_WriteLong,
     FM_WriteString,
     FM_MultiSet,
     FM_SetAsString,
     FM_Export,
     FM_Import,
     FM_Connect,
     FM_Disconnect,
     FM_AddMember,
     FM_RemMember

ENUM  FA_Class                                  = FR_Object_ATTR,
     FA_Revision,
     FA_Version,
     FA_ID,
     FA_UserData,
     FA_NoNotify,
     FA_Parent,
     FA_Child,
     FA_ContextHelp,
     FA_ContextMenu

ENUM  FV_Notify_None,
     FV_Notify_Self,
     FV_Notify_Parent,
     FV_Notify_Window
CONST FV_Notify_Always                          = $49893131,
     FV_Notify_Toggle                          = $49893132,
     FV_Notify_Value                           = $49893131

ENUM  FV_AddMember_Head = -1,
     FV_AddMember_Tail,
     FV_AddMember_Insert

#define _class(o)                               (Long(o-4))
#define _classname(o)                           (_class(o)::feelinClass.name)
->ENDPROC
->PROC FC_Class                           $000040
#define FC_Class 'Class'

CONST FR_Class             = $000040
CONST FR_Class_MTHD        = MTHD_BASE OR FR_Class,
     FR_Class_ATTR        = ATTR_BASE OR FR_Class

ENUM  FA_Class_Name                           = FR_Class_ATTR,
     FA_Class_Super,
     FA_Class_LODSize,
     FA_Class_Dispatcher,
     FA_Class_UserData,
     FA_Class_Pool,
     FA_Class_Methods,
     FA_Class_Attributes,
     FA_Class_ResolveTable,
     FA_Class_AutoReolveTable

OBJECT feelinClass OF feelinNode
   super:PTR TO feelinClass
   name:PTR TO CHAR
   offset:INT
   lodsize:INT
   userdata
   usercount:LONG

   methods:PTR TO feelinDynamicEntry         -> see FA_Class_Methods
   attributes:PTR TO feelinDynamicEntry      -> see FA_Class_Attributes
   resolvedids:PTR TO feelinDynamicEntry     -> see FA_Class_ResolveTable
   autoresolvedids:PTR TO feelinDynamicEntry -> see FA_Class_AutioResolveTable
ENDOBJECT

#define F_QUERY                  PROC FCC_Query(which)
#define F_METHOD(name)           PROC name(class=A2:PTR TO feelinClass,obj=A0,method=D0,msg=A1)
#define F_METHODM(name,fs)       PROC name(class=A2:PTR TO feelinClass,obj=A0,method=D0,msg=A1:PTR TO fs)
#define F_HOOK(name)             PROC name(hook=A0:PTR TO hook,obj=A2,msg=A1)
#define F_HOOKM(name,fs)         PROC name(hook=A0:PTR TO hook,obj=A2,msg=A1:PTR TO fs)
#define F_SUPERDO                F_SuperDoA(class,obj,method,msg)
#define F_OBJDO(o)               F_DoA(o,method,msg)
#define F_STORE(val)             item.data = val
#define F_LOD(cl,o)              o + cl::feelinClass.offset
#define F_ID(tab,n)              (tab[n].id)
#define F_IDM(n)                 F_ID(class.methods,n)
#define F_IDA(n)                 F_ID(class.attributes,n)
#define F_IDR(n)                 F_ID(class.resolvedids,n)
#define F_IDO(n)                 F_ID(class.autoresolvedids,n)
->ENDPROC

/*** Support ***************************************************************/

->PROC FC_Render                          $900000

CONST FR_Render                                 = $900000
CONST FR_Render_MTHD                            = MTHD_BASE OR FR_Render,
     FR_Render_ATTR                            = ATTR_BASE OR FR_Render
#define FC_Render                               'Render'

CONST FA_Render_Application                     = FR_Render_ATTR +  0,
     FA_Render_Display                         = FR_Render_ATTR +  1,
     FA_Render_Window                          = FR_Render_ATTR +  2,
     FA_Render_RPort                           = FR_Render_ATTR +  3,
     FA_Render_Palette                         = FR_Render_ATTR +  4,
     FA_Render_Friend                          = FR_Render_ATTR +  5

CONST FM_Render_AddClip                         = FR_Render_MTHD +  0,
     FM_Render_RemClip                         = FR_Render_MTHD +  1,
     FM_Render_AddClipRegion                   = FR_Render_MTHD +  2

OBJECT feelinRender
   application:FObject                        -> Valid between FM_Setup / FM_Cleanup
   display:FObject                            -> Valid between FM_Setup / FM_Cleanup
   window:FObject                             -> Valid between FM_Setup / FM_Cleanup
   rport:PTR TO rastport                        -> Valid between FM_Show  / FM_Hide
   palette:PTR TO feelinPalette                 -> Set at will
   flags
ENDOBJECT

CONST FF_Render_Refreshing                      = 1 <<  0,
     FF_Render_Complex                         = 1 <<  1,
     FF_Render_TrueColors                      = 1 << 30,
     FF_Render_Forbid                          = 1 << 31
->ENDPROC
->PROC FC_Family                          $900040
CONST FR_Family                                 = $900040
CONST FR_Family_MTHD                            = MTHD_BASE OR FR_Family,
     FR_Family_ATTR                            = ATTR_BASE OR FR_Family

ENUM  FA_Family                                 = FR_Family_ATTR,
     FA_Family_Head,
     FA_Family_Tail,
     FA_Family_Owner

OBJECT feelinFamilyNode OF feelinNode
   object:FObject
ENDOBJECT

#define FC_Family         'Family'
#define FamilyObject F_NewObjA(FC_Family,[TAG_IGNORE,NIL
->ENDPROC
->PROC FC_ImageDisplay                    $9000C0
CONST FR_ImageDisplay                           = $9000C0
CONST FR_ImageDisplay_MTHD                      = MTHD_BASE OR FR_ImageDisplay,
     FR_ImageDisplay_ATTR                      = ATTR_BASE OR FR_ImageDisplay

ENUM  FM_ImageDisplay_Setup                     = FR_ImageDisplay_MTHD,
     FM_ImageDisplay_Cleanup,
     FM_ImageDisplay_Draw,
     FM_ImageDisplay_Size

ENUM  FA_ImageDisplay_Spec                      = FR_ImageDisplay_ATTR,
     FA_ImageDisplay_State,
     FA_ImageDisplay_Width,
     FA_ImageDisplay_Height

ENUM  FI_None,
     FI_Shine,               FI_HalfShine,              FI_Fill,             FI_HalfShadow,             FI_Shadow,           FI_HalfDark,            FI_Dark,             FI_Highlight,
     FI_Shine_HalfShine,     FI_HalfShine_Fill,         FI_Fill_HalfShadow,  FI_HalfShadow_Shadow,      FI_Shadow_HalfDark,  FI_HalfDark_Dark,       FI_Dark_Highlight,
     FI_Shine_Fill,          FI_HalfShine_HalfShadow,   FI_Fill_Shadow,      FI_HalfShadow_HalfDark,    FI_Shadow_Dark,      FI_HalfDark_Highlight,
     FI_Shine_HalfShadow,    FI_HalfShine_Shadow,       FI_Fill_HalfDark,    FI_HalfShadow_Dark,        FI_Shadow_Highlight,
     FI_Shine_Shadow,        FI_HalfShine_HalfDark,     FI_Fill_Dark,        FI_HalfShadow_Highlight,
     FI_Shine_HalfDark,      FI_HalfShine_Dark,         FI_Fill_Highlight,
     FI_Shine_Dark,          FI_HalfShine_Highlight,
     FI_Shine_Highlight

CONST FF_ImageDisplay_Region                    = 1 << 0

OBJECT fs_ImageDisplay_Setup                   ; render:PTR TO feelinRender ; ENDOBJECT
OBJECT fs_ImageDisplay_Cleanup                 ; render:PTR TO feelinRender ; ENDOBJECT
OBJECT fs_ImageDisplay_Draw                    ; render:PTR TO feelinRender ; rect:PTR TO feelinRect ; flags:LONG ; ENDOBJECT
OBJECT fs_ImageDisplay_Size                    ; width:LONG ; height:LONG ; flags:LONG ; ENDOBJECT
OBJECT fs_ImageDisplay_HookDraw                ; render:PTR TO feelinRender ; rect:PTR TO feelinRect ; flags:LONG ; region:PTR TO feelinRect ; ENDOBJECT

#define FS_ImageDisplay_HookDraw                 fs_ImageDisplay_HookDraw

#define FC_ImageDisplay             'ImageDisplay'
#define ImageDisplayObject          F_NewObjA(FC_ImageDisplay,[TAG_IGNORE,NIL
->ENDPROC
->PROC FC_TextDisplay                     $900100
CONST FR_TextDisplay                            = $900100
CONST FR_TextDisplay_MTHD                       = MTHD_BASE OR FR_TextDisplay,
      FR_TextDisplay_ATTR                       = ATTR_BASE OR FR_TextDisplay

#define FC_TextDisplay                          "TextDisplay"

ENUM  FM_TextDisplay_Setup                      = FR_TextDisplay_MTHD,
      FM_TextDisplay_Cleanup,
      FM_TextDisplay_Draw

ENUM  FA_TextDisplay_Contents                   = FR_TextDisplay_ATTR,
      FA_TextDisplay_PreParse,
      FA_TextDisplay_Font,
      FA_TextDisplay_Width,
      FA_TextDisplay_Height,
      FA_TextDisplay_Shortcut

->ENDPROC

/*** GUI System ************************************************************/

->PROC Application                        $004000
CONST FR_Application                            = $004000
CONST FR_Application_MTHD                       = MTHD_BASE OR FR_Application,
      FR_Application_ATTR                       = ATTR_BASE OR FR_Application

ENUM  FM_Application_Run                        = FR_Application_MTHD,
      FM_Application_Shutdown,
      FM_Application_Sleep,
      FM_Application_Awake,
      FM_Application_PushMethod,
      FM_Application_Setup,
      FM_Application_Cleanup,
      FM_Application_Load,
      FM_Application_Save,
      FM_Application_Resolve,
      FM_Application_ResolveInt,
      FM_Application_OpenFont,
      FM_Application_AddSignalHandler,
      FM_Application_RemSignalHandler,
      FM_Application_Update,
      FM_Application_ModifyEvents

ENUM  FA_Application                            = FR_Application_ATTR,
      FA_Application_Title,
      FA_Application_Version,
      FA_Application_Copyright,
      FA_Application_Author,
      FA_Application_Description,
      FA_Application_Base,
      FA_Application_Unique,
      FA_Application_Signal,
      FA_Application_UserSignals,
      FA_Application_WindowPort,
      FA_Application_BrokerPort,
      FA_Application_BrokerHook,
      FA_Application_BrokerPri,
      FA_Application_Broker,
      FA_Application_Display,
      FA_Application_Dataspace,
      FA_Application_Preference,
      FA_Application_Sleep,
      FA_Application_ResolveMapping

ENUM  FV_Application_ENV,
      FV_Application_ENVARC,
      FV_Application_BOTH

ENUM  FV_KEY_NONE,
      FV_KEY_PRESS,
      FV_KEY_RELEASE,
      FV_KEY_UP,
      FV_KEY_DOWN,
      FV_KEY_STEPUP,
      FV_KEY_STEPDOWN,
      FV_KEY_TOP,
      FV_KEY_BOTTOM,
      FV_KEY_LEFT,
      FV_KEY_RIGHT,
      FV_KEY_STEPLEFT,
      FV_KEY_STEPRIGHT,
      FV_KEY_FIRST,
      FV_KEY_LAST,
      FV_KEY_CHARBACK,
      FV_KEY_CHARDEL,
      FV_KEY_WORDBACK,
      FV_KEY_WORDDEL,
      FV_KEY_LINEBACK,
      FV_KEY_LINEDEL,
      FV_KEY_NEXTOBJ,
      FV_KEY_PREVOBJ,
      FV_KEY_NOOBJ,
      FV_KEY_CLOSEWINDOW,
      FV_KEY_COUNT

#define FC_Application                          'Application'
#define AppObject                               F_NewObjA(FC_Application,[TAG_IGNORE,NIL
#define FS_Resolve                              fs_Application_Resolve
#define FS_OpenFont                             fs_Application_OpenFont

/*** FeelinSignalHandler ***/

OBJECT feelinSignalHandler OF feelinNode
   flags
   object:FObject
   method

UNION [
   [signals,reserved],
   [secs, micros]
     ]
ENDOBJECT

CONST FF_SignalHandler_Timer = 1 << 0

/*** FeelinEvent ***********************************************************/

OBJECT feelinEvent
   imsg:PTR TO intuimessage
   flags:LONG
   class:LONG
   code:INT
   qualifier:INT
   key:CHAR
   decodedchar:CHAR
   reserved:INT
   mousex:INT
   mousey:INT
   seconds:LONG
   micros:LONG
   window
ENDOBJECT

-> classes

CONST FF_EVENT_TICK                 = %00000001,
      FF_EVENT_KEY                  = %00000010,
      FF_EVENT_BUTTON               = %00000100,
      FF_EVENT_MOTION               = %00001000,
      FF_EVENT_WINDOW               = %00010000,
      FF_EVENT_DISK                 = %00100000,
      FF_EVENT_WBDROP               = %01000000

-> codes

ENUM  FV_EVENT_BUTTON_SELECT,
      FV_EVENT_BUTTON_MENU,
      FV_EVENT_BUTTON_MIDDLE,
      FV_EVENT_BUTTON_WHEEL

ENUM  FV_EVENT_WINDOW_CLOSE,
      FV_EVENT_WINDOW_ACTIVE,
      FV_EVENT_WINDOW_INACTIVE,
      FV_EVENT_WINDOW_SIZE,
      FV_EVENT_WINDOW_DEPTH,
      FV_EVENT_WINDOW_CHANGE,
      FV_EVENT_WINDOW_REFRESH

ENUM  FV_EVENT_DISK_LOAD,
      FV_EVENT_DISK_EJECT

-> flags

CONST FF_EVENT_KEY_UP               = %00000001,
      FF_EVENT_KEY_REPEAT           = %00000010,
      FF_EVENT_BUTTON_DOWN          = %00000001,
      FF_EVENT_BUTTON_DOUBLE        = %00000010

/***************************************************************************/

OBJECT feelinAppResolveMapping
   name:PTR TO CHAR
   data
ENDOBJECT

/***************************************************************************/

OBJECT fs_Application_Resolve ; id,default ; ENDOBJECT
OBJECT fs_Application_OpenFont ; obj:FObject,spec:PTR TO CHAR ; ENDOBJECT
->ENDPROC
->PROC Window                             $004040
#define FC_Window          'Window'
#define WindowObject F_NewObjA(FC_Window,[TAG_IGNORE,NIL

CONST FR_Window      = $004040
CONST FR_Window_Mthd = MTHD_BASE OR FR_Window,
     FR_Window_Attr = ATTR_BASE OR FR_Window

ENUM  FM_Window_Setup = FR_Window_Mthd,
      FM_Window_Cleanup,
      FM_Window_Open,
      FM_Window_Close,
      FM_Window_ChainAdd,
      FM_Window_ChainRem,
      FM_Window_AddEventHandler,
      FM_Window_RemEventHandler,
      FM_Window_HandleEvent,
      FM_Window_Layout,
      FM_Window_Draw

ENUM  FA_Window               = FR_Window_Attr,
      FA_Window_Title,
      FA_Window_ScreenTitle,
      FA_Window_Open,
      FA_Window_CloseRequest,
      FA_Window_Active,
      FA_Window_ActiveObject,
      FA_Window_Backdrop,
      FA_Window_Borderless,
      FA_Window_Resizable,
      FA_Window_GadNone,
      FA_Window_GadDragbar,
      FA_Window_GadClose,
      FA_Window_GadDepth,
      FA_Window_GadIconify,
      FA_Window_Decorator

ENUM  FV_Window_ActiveObject_Prev = -2,
      FV_Window_ActiveObject_Next,
      FV_Window_ActiveObject_None

/*** EventHandler **********************************************************/

OBJECT feelinEventHandler OF feelinNode
   flags:INT
   priority:CHAR                 -> Linked by priority
   reserved:CHAR
   events                        -> One or more IDCMP flags this handler should react on
   object:FObject              -> Object which should receive FM_HandleEvent
   class:PTR TO feelinClass      -> If class <> NIL then class will be called instead of _class(object)
ENDOBJECT

CONST FF_HandleEvent_Eat = %0001 -> Returned by FM_HandleEvent to stop event propagation
->ENDPROC
->PROC Display                            $004080
CONST FR_Display      =                $004080
CONST FR_Display_MTHD = MTHD_BASE OR FR_Display,
      FR_Display_ATTR = ATTR_BASE OR FR_Display

ENUM  FM_AddColor                               = FR_Display_MTHD,
      FM_RemColor,
      FM_AddPalette,
      FM_RemPalette,
      FM_CreateColor,
      FM_CreateColorScheme

CONST FM_DeleteColor                            = FM_RemColor,
      FM_DeleteColorScheme                      = FM_RemPalette

OBJECT fs_AddColor                              ; argb:LONG ; ENDOBJECT
OBJECT fs_RemColor                              ; color:PTR TO feelinColor ; ENDOBJECT
OBJECT fs_RemPalette                            ; palette:PTR TO feelinPalette ; ENDOBJECT
OBJECT fs_CreateColor                           ; spec:PTR TO CHAR; reference:PTR TO feelinPalette ; ENDOBJECT
#define FS_DeleteColor                          fs_RemColor
OBJECT fs_CreateColorScheme                     ; spec:PTR TO CHAR; reference:PTR TO feelinPalette ; ENDOBJECT
#define FS_DeleteColorScheme                    fs_RemPalette

ENUM  FA_ColorScheme                            = FR_Display_ATTR

ENUM  FV_Pen_Text,
      FV_Pen_Shine,
      FV_Pen_HalfShine,
      FV_Pen_Fill,
      FV_Pen_HalfShadow,
      FV_Pen_Shadow,
      FV_Pen_HalfDark,
      FV_Pen_Dark,
      FV_Pen_Highlight,
      FV_PEN_COUNT

CONST FV_COLOR_SPACE                            = 12

/*** FeelinColor ***********************************************************/

OBJECT feelinColor
   _priv0:LONG
   _priv1:LONG
   pen:LONG
   argb:LONG
ENDOBJECT

/*** FeelinPalette *********************************************************/

OBJECT feelinPalette
   _priv0:LONG
   _priv1:LONG
   _priv2:LONG
   colorcount:LONG
   pens:PTR TO LONG
   argb:PTR TO LONG
   colors:PTR TO LONG
ENDOBJECT

#define FC_Display                               "Display"
->ENDPROC
/*** GUI Classes ***********************************************************/

->PROC Frame                              $001000
CONST FR_Frame                                  = $001000
CONST FR_Frame_MTHD                             = MTHD_BASE OR FR_Frame,
      FR_Frame_ATTR                             = ATTR_BASE OR FR_Frame

ENUM  FM_Frame_Setup                            = FR_Frame_MTHD,
      FM_Frame_Cleanup,
      FM_Frame_Draw

ENUM  FA_Frame                                  = FR_Frame_ATTR,
      FA_Frame_PublicData,
      FA_Frame_Font,
      FA_Frame_Title,
      FA_Frame_PreParse,
      FA_Frame_Position,
      FA_Frame_InnerLeft,
      FA_Frame_InnerTop,
      FA_Frame_InnerRight,
      FA_Frame_InnerBottom,
      FA_Frame_InnerWidth,
      FA_Frame_InnerHeight,
      FA_Back,
      FA_NoFill

CONST FV_Frame_None                 = 0,
      FV_Frame_LastBuiltin          = 49

ENUM  FV_Frame_UpLeft,              -> FA_Frame_Position
      FV_Frame_UpRight,
      FV_Frame_UpCenter,
      FV_Frame_DownLeft,
      FV_Frame_DownRight,
      FV_Frame_DownCenter

CONST FF_Frame_Draw_Select                      = %0010

OBJECT feelinInner
   l:CHAR
   t:CHAR
   r:CHAR
   b:CHAR
ENDOBJECT

OBJECT feelinFrameData
    border[2]:feelinInner
    padding[2]:feelinInner
    back
ENDOBJECT

#define NoFrame         FA_Frame,FV_Frame_None
#define ButtonFrame     FA_Frame,'$button-frame'
#define TextFrame       FA_Frame,'$text-frame'
#define StringFrame     FA_Frame,'$string-frame'
#define PropFrame       FA_Frame,'$prop-frame'
#define SliderFrame     FA_Frame,'$slider-frame'
#define GaugeFrame      FA_Frame,'$gauge-frame'
#define GroupFrame      FA_Frame,'$group-frame'
->ENDPROC
->PROC Area                               $001040
CONST FR_Area                                   = $001040
CONST FR_Area_MTHD                              = MTHD_BASE OR FR_Area,
      FR_Area_ATTR                              = ATTR_BASE OR FR_Area

ENUM  FM_Setup                                  = FR_Area_MTHD,
      FM_Cleanup,
      FM_Show,
      FM_Hide,
      FM_AskMinMax,
      FM_Layout,
      FM_Draw,
      FM_Erase,
      FM_HandleEvent,
      FM_ModifyHandler,
      FM_GoActive,
      FM_GoInactive,
      FM_GoDisabled,
      FM_GoEnabled,
      FM_DnDDo,
      FM_DnDQuery,
      FM_DnDBegin,
      FM_DnDFinish,
      FM_DnDReport,
      FM_DnDDrop,
      FM_RethinkLayout,
      FM_BuildContextMenu,
      FM_BuildContextHelp

#define FS_Draw               feelins_Draw
#define FS_Erase              feelins_Erase
#define FS_Setup              feelins_Setup
#define FS_HandleEvent        feelins_HandleEvent
#define FS_DragQuery          feelins_DragQuery
#define FS_BuildContextHelp   feelins_BuildContextHelp
#define FS_BuildContextMenu   feelins_BuildContextMenu

ENUM  FA_AreaData              = FR_Area_ATTR,
      FA_Left,
      FA_Top,
      FA_Width,
      FA_Height,
      FA_Right,
      FA_Bottom,
      FA_MinWidth,
      FA_MinHeight,
      FA_MaxWidth,
      FA_MaxHeight,
      FA_FixWidth,
      FA_FixHeight,
      FA_SetMin,
      FA_SetMax,
      FA_Weight,
      FA_Active,
      FA_Selected,
      FA_Pressed,
      FA_Hidden,
      FA_Disabled,
      FA_Draggable,
      FA_Dropable,
      FA_Horizontal,
      FA_Timer,
      FA_InputMode,
      FA_ControlChar,
      FA_ChainToCycle,
      FA_Font,
      FA_WindowObject

ENUM  FV_InputMode_None,
      FV_InputMode_Immediate,
      FV_InputMode_Release,
      FV_InputMode_Toggle

ENUM  FV_SetNone,
      FV_SetBoth,
      FV_SetWidth,
      FV_SetHeight

ENUM  FV_Font_Inherit = -1

CONST FF_Horizontal           = %00000000000000000000000000000001,
      FF_Area_SetMinW         = %00000000000000000000000000000010,
      FF_Area_SetMaxW         = %00000000000000000000000000000100,
      FF_Area_SetMinH         = %00000000000000000000000000001000,
      FF_Area_SetMaxH         = %00000000000000000000000000010000,
      FF_Area_Selected        = %00000000000000000000000000100000,
      FF_Area_Pressed         = %00000000000000000000000001000000,
      FF_Area_Active          = %00000000000000000000000010000000,
      FF_Area_Disabled        = %00000000000000000000000100000000,

      FF_Area_Group           = %00000000000000000001000000000000,
      FF_Area_CanDraw         = 1 << 13,
      FF_Area_CanShow         = 1 << 14,
      FF_Area_Setup           = 1 << 15

CONST FF_Draw_Object         = %00000000000000000000000000000001,
      FF_Draw_Update         = %00000000000000000000000000000010,
      FF_Draw_Custom_1       = %00000001000000000000000000000000,
      FF_Draw_Custom_2       = %00000010000000000000000000000000,
      FF_Draw_Custom_3       = %00000100000000000000000000000000,
      FF_Draw_Custom_4       = %00001000000000000000000000000000,
      FF_Draw_Custom_5       = %00010000000000000000000000000000,
      FF_Draw_Custom_6       = %00100000000000000000000000000000,
      FF_Draw_Custom_7       = %01000000000000000000000000000000,
      FF_Draw_Custom_8       = %10000000000000000000000000000000

CONST FF_Erase_Fill          = %00000000000000000000000000000001,
      FF_Erase_Region        = %00000000000000000000000000000010

CONST FV_MAXMAX = 10000

OBJECT feelinRect   ; x1:INT, y1:INT, x2:INT, y2:INT         ; ENDOBJECT
OBJECT feelinBox    ; x:INT,y:INT,w:INT,h:INT                ; ENDOBJECT
OBJECT feelinMinMax ; minw:INT, minh:INT, maxw:INT, maxh:INT ; ENDOBJECT

#define FAreaPublic                             feelinAreaPublic

OBJECT feelinAreaPublic
   parent:FObject

   render:PTR TO feelinRender

   flags:LONG

   box:feelinBox
   inner:feelinInner
   minmax:feelinMinMax

   pens:PTR TO LONG
   font:PTR TO textfont
   weight:INT
ENDOBJECT

OBJECT feelins_Setup                            ; render:PTR TO feelinRender ; ENDOBJECT
OBJECT feelins_Draw                             ; flags ; ENDOBJECT
OBJECT feelins_Erase                            ; rect:PTR TO feelinRect ; flags ; ENDOBJECT
OBJECT feelins_HandleEvent                      ; fev:PTR TO feelinEvent ; ENDOBJECT
OBJECT feelins_ModifyHandler                    ; add,sub ; ENDOBJECT
OBJECT feelins_DnDDo                            ; mousex,mousey ; ENDOBJECT
OBJECT feelins_DnDQuery                         ; mousex,mousey ; source:FObject ; ENDOBJECT
OBJECT feelins_DnDBegin                         ; source:FObject ; ENDOBJECT
OBJECT feelins_DnDFinish                        ; source:FObject ; ENDOBJECT
OBJECT feelins_DnDReport                        ; mousex,mousey ; source:FObject ; update ; dragbox:PTR TO feelinBox ; ENDOBJECT
OBJECT feelins_DnDDrop                          ; mousex,mousey ; source:FObject ; ENDOBJECT
OBJECT feelins_BuildContextHelp                 ; mousex,mousey,help:PTR TO CHAR ; ENDOBJECT
OBJECT FS_BuildContextMenu                      ; mousex,mousey,menu:FObject,contextowner:FObject,render:PTR TO feelinRender ; ENDOBJECT

/* the following macros assume  the  variable  'lod'  to  be  declared  and
holding  a  field  'areadata'  of type (FAreaData *). If it is not the case
undef '_areadata' and redefine it to your convenience */

#define _areadata                               (lod.areapublic::FAreaData)
#define _parent                                 (_areadata.parent)
#define _render                                 (_areadata.render)
#define _flags                                  (_areadata.flags)
#define _box                                    (_areadata.box)
#define _x                                      (_box.x)
#define _y                                      (_box.y)
#define _w                                      (_box.w)
#define _h                                      (_box.h)
#define _inner                                  (_areadata.inner)
#define _bl                                     (_inner.l)
#define _bt                                     (_inner.t)
#define _br                                     (_inner.r)
#define _bb                                     (_inner.b)
#define _minmax                                 (_areadata.minmax)
#define _minw                                   (_minmax.minw)
#define _minh                                   (_minmax.minh)
#define _maxw                                   (_minmax.maxw)
#define _maxh                                   (_minmax.maxh)
#define _pens                                   (_areadata.pens)
#define _font                                   (_areadata.font)
#define _weight                                 (_areadata.weight)

/* extanded coordinates macros */

#define _x2                                     (_x + _w - 1)
#define _y2                                     (_y + _h - 1)
#define _ix                                     (_x + _bl)
#define _iy                                     (_y + _bt)
#define _iw                                     (_w - _bl - _br)
#define _ih                                     (_h - _bt - _bb)
#define _ix2                                    (_x2 - _br)
#define _iy2                                    (_y2 - _bb)

/* macros to access  "_areadata.render".  !!WARNING!!  "_render"  *MUST*  be
checked before accessing flieds */

#define _display                                _render.display
#define _app                                    _render.application
#define _win                                    _render.window
#define _rp                                     _render.rport

/* macros that can be used while creating objects */

#define AreaObject            F_NewObjA(FC_Area,[TAG_IGNORE,NIL
#define InputImmediate        FA_InputMode,FV_InputMode_Immediate
#define InputRelease          FA_InputMode,FV_InputMode_Release
#define InputToggle           FA_InputMode,FV_InputMode_Toggle
#define DontChain             FA_ChainToCycle,FALSE
#define HorizLayout           FA_Horizontal,TRUE
->ENDPROC
->PROC Text                               $001080
CONST FR_Text                                   = $001080
CONST FR_Text_MTHD                              = MTHD_BASE OR FR_Text,
     FR_Text_ATTR                              = ATTR_BASE OR FR_Text

ENUM  FA_Text                                   = FR_Text_ATTR,
     FA_Text_PreParse,
     FA_Text_AltPreParse,
     FA_Text_HCenter,
     FA_Text_VCenter,
     FA_Text_Shortcut,
     FA_Text_Static

#define TextObject                  F_NewObjA(FC_Text,[TAG_IGNORE,NIL
#define TextBack                    FA_Back,'$text-back'
->ENDPROC
->PROC Group                              $0010C0
#define FC_Group     'Group'
#define GroupObject  F_NewObjA(FC_Group,[TAG_IGNORE,NIL
#define VGroup       F_NewObjA(FC_Group,[TAG_IGNORE,NIL
#define HGroup       F_NewObjA(FC_Group,[FA_Horizontal,TRUE
#define Page         F_NewObjA(FC_Group,[FA_Group_PageMode,TRUE

CONST FR_Group                                  = $0010C0
CONST FR_Group_MTHD                             = MTHD_BASE OR FR_Group,
     FR_Group_ATTR                             = ATTR_BASE OR FR_Group

ENUM  FM_Group_Forward                          = FR_Group_MTHD,
     FM_Group_InitChange,
     FM_Group_ExitChange

ENUM  FA_Group_HSpacing    = FR_Group_ATTR,
     FA_Group_VSpacing,
     FA_Group_SameSize,
     FA_Group_SameWidth,
     FA_Group_SameHeight,
     FA_Group_RelSizing,
     FA_Group_PageMode,
     FA_Group_PageFont,
     FA_Group_PageTitle,
     FA_Group_PageAPreParse,
     FA_Group_PageIPreParse,
     FA_Group_ActivePage,
     FA_Group_Rows,
     FA_Group_Columns,
     FA_Group_LayoutHook,
     FA_Group_MinMaxHook,
     FA_Group_Forward

ENUM  FV_Group_ActivePage_Prev = -4,
     FV_Group_ActivePage_Next,
     FV_Group_ActivePage_Last,
     FV_Group_ActivePage_First
->ENDPROC

/***************************************************************************/

#define FC_Area                     'Area'
#define FC_Balance                  'Balance'
#define FC_Bar                      'Bar'
#define FC_Cycle                    'Cycle'
#define FC_Dataspace                'Dataspace'
#define FC_Frame                    'Frame'
#define FC_Gauge                    'Gauge'
#define FC_Image                    'Image'
#define FC_Numeric                  'Numeric'
#define FC_Picture                  'Picture'
#define FC_PopHelp                  'PopHelp'
#define FC_PopColor                 'PopColor'
#define FC_Prop                     'Prop'
#define FC_Radio                    'Radio'
#define FC_Scale                    'Scale'
#define FC_Slider                   'Slider'
#define FC_String                   'String'
#define FC_Text                     'Text'

#define BalanceID(id)               F_NewObjA(FC_Balance,[FA_ID,id,TAG_DONE])
#define BalanceObject               F_NewObjA(FC_Balance,[TAG_IGNORE,NIL
#define BarObject                   F_NewObjA(FC_Bar,[TAG_IGNORE,NIL
#define CycleObject                 F_NewObjA(FC_Cycle,[TAG_IGNORE,NIL
#define DataspaceObject             F_NewObjA(FC_Dataspace,[TAG_IGNORE,NIL
#define GaugeObject                 F_NewObjA(FC_Gauge,[TAG_IGNORE,NIL
#define ImageObject                 F_NewObjA(FC_Image,[TAG_IGNORE,NIL
#define NumericObject               F_NewObjA(FC_Numeric,[TAG_IGNORE,NIL
#define PictureObject               F_NewObjA(FC_Picture,[TAG_IGNORE,NIL
#define PopColorObject              F_NewObjA(FC_PopColor,[TAG_IGNORE,NIL
#define PopHelpObject               F_NewObjA(FC_PopHelp,[TAG_IGNORE,NIL
#define PropObject                  F_NewObjA(FC_Prop,[TAG_IGNORE,NIL
#define RadioObject                 F_NewObjA(FC_Radio,[TAG_IGNORE,NIL
#define ScaleObject                 F_NewObjA(FC_Scale,[TAG_IGNORE,NIL
#define SliderObject                F_NewObjA(FC_Slider,[TAG_IGNORE,NIL
#define StringObject                F_NewObjA(FC_String,[TAG_IGNORE,NIL

/*** Values ****************************************************************/

ENUM  FV_Image_Normal,
      FV_Image_Selected,
      FV_Image_Enabled,
      FV_Image_Disabled

ENUM  FV_String_Left,
      FV_String_Center,
      FV_String_Right

ENUM    FV_Cycle_EntriesType_String,
        FV_Cycle_EntriesType_Array

/*** Macros ****************************************************************/

#define ButtonBack      FA_Back, '$button-back'
