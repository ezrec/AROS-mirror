#ifndef SCALOS_H
#define SCALOS_H

// :ts=4 (Tabsize)
// last used id: 171

/*
** Amiga Workbench® Replacement
**
** (C) Copyright 1999,2000 Aliendesign
** Stefan Sommerfeld, Jörg Rebenstorf
**
** Redistribution and use in source and binary forms are permitted provided that
** the above copyright notice and this paragraph are duplicated in all such
** forms and that any documentation, advertising materials, and other
** materials related to such distribution and use acknowledge that the
** software was developed by Aliendesign.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif /* UTILITY_TAGITEM_H */

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif /* INTUITION_INTUITION_H */

#ifndef INTUITION_INTUITION_H
#include <intuition/classusr.h>
#endif /* INTUITION_INTUITION_H */

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif /* DOS_DOS_H */

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif /* EXEC_LISTS_H */

/*----------------------------------------------------------------------*/

/* The tree of classes of Scalos is:
 * Root
 *   +--Graphic
 *   |  +--Bitmap
 *   |  +--Icon
 *   |     +--DOSIcon
 *   |     |  +--DOSFileIcon
 *   |     |  +--DOSDirIcon
 *   |     |     +--DOSDevcieIcon
 *   |     +--AppIconList
 *   |     +--AppIcon
 *   |     +--AmigaIcon
 *   |     +--DOSListIcon
 *   |
 *   +--PopupMenu
 *   +--MenuItem
 *   +--WindowTask
 *   +--Screen
 *   +--Area
 *      +--Panel
 *      |  +--IconPanel
 *      +--Chunky8Test
 *      +--PlanarTest
 *      +--Group
 *         +--PanelView
 *
 * ThreadRoot
 *   +--Window
 *   +--ThreadArea
 *      +--RectFill
 *
 */

#define SCC_DUMMY                   TAG_USER + 0x00600000

/* --------------------- Window --------------------------------------- */

#define SCC_WINDOW_NAME             "Window.scalos"

#define SCCA_Window_Left            SCC_DUMMY + 2       // ISG LONG
#define SCCA_Window_Top             SCC_DUMMY + 3       // ISG LONG
#define SCCA_Window_Width           SCC_DUMMY + 4       // ISG LONG
#define SCCA_Window_Height          SCC_DUMMY + 5       // ISG LONG
#define SCCA_Window_Title           SCC_DUMMY + 7       // ISG char *
#define SCCA_Window_SliderRight     SCC_DUMMY + 9       // I.G BOOL !private! struct Gadget *
#define SCCA_Window_SliderBottom    SCC_DUMMY + 10      // I.G BOOL !private! struct Gagdet *
#define SCCA_Window_ScreenObj       SCC_DUMMY + 11      // I.. Object
#define SCCA_Window_Root            SCC_DUMMY + 15      // I.. Object
#define SCCA_Window_CloseRequest    SCC_DUMMY + 27      // ..G BOOL
#define SCCA_Window_Window          SCC_DUMMY + 45      // ..G struct Window *

#define SCCM_Window_Open            SCC_DUMMY + 6       // return type: ULONG
struct SCCP_Window_Open
{
        ULONG   MethodID;
        ULONG   open;
};

#define SCCM_Window_Message         SCC_DUMMY + 17      // return type: void !!! Obsolete !!!
struct SCCP_Window_Message
{
        ULONG   MethodID;
        struct  IntuiMessage *imsg;
};

#define SCCM_Window_SetIDCMP        SCC_DUMMY + 51      // return type: void
                                                                                                                // PRIVATE! Use SCCM_Area_SetIDCMP
struct SCCP_Window_SetIDCMP
{
        ULONG   MethodID;
        ULONG   IDCMP;
        Object  *obj;
};

#define SCCM_Window_SetClipRegion   SCC_DUMMY + 56      // return type: void
struct SCCP_Window_SetClipRegion
{
        ULONG   MethodID;
        struct  Region *clipregion;
};

#define SCCM_Window_ClearClipRegion SCC_DUMMY + 57      // return type: void

#define SCCM_Window_BeginDraw       SCC_DUMMY + 58      // return type: ULONG
struct SCCP_Window_BeginDraw
{
        ULONG   MethodID;
        Object  *object;
};

#define SCCM_Window_EndDraw         SCC_DUMMY + 160     // return type: void
struct SCCP_Window_EndDraw
{
        ULONG   MethodID;
        Object  *object;
};

#define SCCM_Window_DelayedShowObject   SCC_DUMMY + 77      // return type: void
struct SCCP_Window_DelayedShowObject
{
        ULONG   MethodID;
        Object  *object;
};

/* ------------------ Area ----------------------------- */

#define SCC_AREA_NAME               "Area.scalos"
#define SCC_THREADAREA_NAME         "ThreadArea.scalos"

#define SCCA_Area_InnerWidth        SCC_DUMMY + 75      // ..G ULONG
#define SCCA_Area_InnerHeight       SCC_DUMMY + 76      // ..G ULONG

#define SCCM_Area_Draw              SCC_DUMMY + 18      // return type: void

#define SCCM_Area_Setup             SCC_DUMMY + 19      // return type: ULONG
struct SCCP_Area_Setup
{
        ULONG   MethodID;
        struct  SC_RenderInfo *rinfo;
};

#define SCCM_Area_Cleanup           SCC_DUMMY + 20      // return type: void
#define SCCM_Area_Show              SCC_DUMMY + 21      // return type: void
struct SCCP_Area_Show
{
        ULONG   MethodID;
        ULONG   action;                                     // see below
        struct  IBox *bounds;                               // pos/size of this object
};


#define SCCM_Area_Hide              SCC_DUMMY + 22      // return type: void
struct SCCP_Area_Hide
{
        ULONG   MethodID;
        ULONG   action;                                     // see below
};

#define SCCM_Area_HandleInput       SCC_DUMMY + 27      // return type: void
struct SCCP_Area_HandleInput
{
        ULONG   MethodID;
        struct  IntuiMessage    *imsg;
};

#define SCCM_Area_AskMinMax         SCC_DUMMY + 1       // return type: void
struct SCCP_Area_AskMinMax
{
        ULONG   MethodID;
        struct  SC_MinMax *sizes;
};

#define SCCM_Area_SetIDCMP          SCC_DUMMY + 8       // return type: void
struct SCCP_Area_SetIDCMP
{
        ULONG   MethodID;
        ULONG   IDCMP;
};

#define SCCM_Area_BeginDraw        SCC_DUMMY + 59      // return type: ULONG
struct SCCP_Area_BeginDraw
{
        ULONG   MethodID;
        Object  *object;
};

#define SCCM_Area_EndDraw          SCC_DUMMY + 161     // return type: void
struct SCCP_Area_EndDraw
{
        ULONG   MethodID;
        Object  *object;
};

#define SCCM_Area_DelayedShow       SCC_DUMMY + 78      // return type: void

// ULONG action is one of ...
#define SCCV_Area_Action_OpenClose      0               // window has been opened (SCCM_Show) / closed (SCCM_Area_Hide)
#define SCCV_Area_Action_Resize         1               // window has been resized

#define SCCV_Area_MaxSize               30000           // maxsize for SCCM_Area_AskMinMax

// ULONG drawtype is one of ...
#define SCCV_Area_Drawtype_Complete     0
#define SCCV_Area_Drawtype_Update       1

#define SCCA_Area_StaticContents    SCC_DUMMY + 80

/*------------------- Screen ---------------------------*/

#define SCC_SCREEN_NAME             "Screen.scalos"

#define SCCA_Screen_Screen          SCC_DUMMY + 12      // ..G struct Screen
#define SCCA_Screen_PubName         SCC_DUMMY + 13      // I.. char *
#define SCCA_Screen_DrawInfo        SCC_DUMMY + 14      // ..G struct DrawInfo
#define SCCA_Screen_PenShareMap     SCC_DUMMY + 55      // ..G APTR PenShareMap (GUIGFX)


/*--------------- WindowTask ---------------------------*/

#define SCC_WINDOWTASK_NAME         "WindowTask.scalos"

#define SCCA_WindowTask_Window      SCC_DUMMY + 23

#define SCCM_WindowTask_Input       SCC_DUMMY + 26      // return type: ULONG
struct SCCP_WindowTask_Input
{
        ULONG   MethodID;
        ULONG   *signal;        // signal bits to wait for
};


#define SCCM_WindowTask_Return      SCC_DUMMY + 43      // return type: void

/*------------------ Root ------------------------------*/

#define SCC_ROOT_NAME               "Root.scalos"
#define SCC_THREADROOT_NAME         "ThreadRoot.scalos"

#define SCCA_MsgPort                SCC_DUMMY + 52      // ..G struct MsgPort *

// These are replacements to OM_NEW and OM_DISPOSE

#define SCCM_Init                   SCC_DUMMY + 24      // return type: ULONG
// Don't use OM_NEW anymore!

struct SCCP_Init
{
        ULONG   MethodID;
        struct  TagItem *ops_AttrList;
};

#define SCCM_Exit                   SCC_DUMMY + 25      // return type: void
// Don't use OM_DISPOSE anymore!

#define SCCM_Notify                 SCC_DUMMY + 28      // return type: void
struct SCCP_Notify
{
        ULONG   MethodID;
        ULONG   TriggerAttr;
        ULONG   TriggerVal;
        APTR    DestObject;
        ULONG   numargs;
        /* args follow here */
};

#define SCCM_Set                    SCC_DUMMY + 29      // no method, only for notification
struct SCCP_Set
{
        ULONG   MethodID;
        ULONG   Attr;
        ULONG   Value;
};


#define SCCM_Input                  SCC_DUMMY + 48      // return type : ULONG

#define SCCM_MessageReceived        SCC_DUMMY + 49      // return type : void
struct SCCP_MessageReceived
{
        ULONG   MethodID;
        struct  Message *message;
};

#define SCCM_ReplyReceived          SCC_DUMMY + 50      // return type : void
struct SCCP_ReplyReceived
{
        ULONG   MethodID;
        struct  Message *message;
};

#define SCCM_LockObjectList         SCC_DUMMY + 53      // return type : struct MinList *
struct SCCP_LockObjectList
{
        ULONG   MethodID;
        ULONG   locktype;
};

#define SCCM_UnlockObjectList       SCC_DUMMY + 54      // return type : void

#define SCCM_Clone                  SCC_DUMMY + 159     // return type : object

#define SCCV_EveryTime              0x49893131
#define SCCV_TriggerValue           0x49893131
#define SCCV_LockExclusive          0
#define SCCV_LockShared             1
#define SCCV_LockAttempt            2
#define SCCV_LockAttemptShared      3

/*------------------ MethodSender ------------------------*/


#define SCC_METHODSENDER_NAME       "MethodSender.scalos"

#define SCCA_MethodSender_DestObject    SCC_DUMMY + 81  // I.. Object *

/*------------------ Graphic -----------------------------*/

#define SCC_GRAPHIC_NAME             "Graphic.scalos"

#define SCCM_Graphic_Draw           SCC_DUMMY + 26      // return type: void

struct SCCP_Graphic_Draw                                // parameters for SCCM_Bitmap_Draw method
{
        ULONG            MethodID;
        LONG             posX,posY;
        ULONG            drawflags;
        
};

#define SCCM_Graphic_PreThinkScreen SCC_DUMMY + 40      // return type: ULONG
struct SCCP_Graphic_PreThinkScreen
{
        ULONG            MethodID;
        Object          *Screenobj;
};

#define SCCM_Graphic_PostThinkScreen SCC_DUMMY + 41     // return type: void

#define SCCM_Graphic_PreThinkWindow  SCC_DUMMY + 30     // return type: ULONG
struct SCCP_Graphic_PreThinkWindow
{
        ULONG            MethodID;
        struct RastPort *rastport;
        ULONG            action;                            // refer to SCCP_Area_Show
};  


#define SCCM_Graphic_PostThinkWindow SCC_DUMMY + 42      // return type: void
struct SCCP_Graphic_PostThinkWindow
{
        ULONG            MethodID;
        ULONG            action;                            // refer to SCCP_Area_Show
};  

#define SCCM_Graphic_HitTest        SCC_DUMMY + 162     // return type: ULONG
struct SCCP_Graphic_HitTest
{
        ULONG   MethodID;
        LONG    X;
        LONG    Y;
};

#define SCCM_Graphic_Erase          SCC_DUMMY + 158      // return type: void

#define SCCA_Graphic_Left           SCC_DUMMY + 82      // ISG (ULONG)
#define SCCA_Graphic_Top            SCC_DUMMY + 83      // ISG (ULONG)

#define SCCA_Graphic_Width          SCC_DUMMY + 33      // ISG (ULONG)
#define SCCA_Graphic_Height         SCC_DUMMY + 32      // ISG (ULONG)

// Drawflags
#define SCCV_Graphic_DrawFlags_RelPos           0
#define SCCV_Graphic_DrawFlags_AbsPos           1
                                                                                                                
/*------------------ Bitmap -----------------------------*/

#define SCC_BITMAP_NAME             "Bitmap.scalos"

#define SCCM_Bitmap_Fill            SCC_DUMMY + 79      // return type: void
struct SCCP_Bitmap_Fill
{
        ULONG            MethodID;
        LONG             MinX;
        LONG             MinY;
        LONG             MaxX;
        LONG             MaxY;
        LONG             OffsetX;
        LONG             OffsetY;
        ULONG            BlitType;
};  


#define SCCA_Bitmap_Bitmap          SCC_DUMMY + 34      // I.G (struct BitMap *)
#define SCCA_Bitmap_RawData         SCC_DUMMY + 35      // I.G (APTR)
#define SCCA_Bitmap_RawType         SCC_DUMMY + 44      // I.G (ULONG) type of bitmap, see below
#define SCCA_Bitmap_CopyOriginal    SCC_DUMMY + 153     // I.. (BOOL)

/* SCCA_Bitmap_RawType is one of
 *
 * PIXFMT_CHUNKY_CLUT (rawdata 8bit chunky with/without palette)
 * PIXFMT_0RGB_32 (rawdata 24bit chunky)
 *
 * defined in <render/render.h>
 * refer to guigfx autodocs for more
 */

#define SCCA_Bitmap_Palette         SCC_DUMMY + 36      // I.. (APTR)
#define SCCA_Bitmap_NumColors       SCC_DUMMY + 37      // I.. (ULONG)
#define SCCA_Bitmap_PaletteFormat   SCC_DUMMY + 38      // I.. (ULONG)

/* SCCA_Bitmap_PaletteFormat is one of
 *
 * PALFMT_RGB8 ((ULONG) 0x00rrggbb)
 * PALFMT_RGB32 (ULONG red; ULONG green; ULONG blue;)
 *
 * defined in <render/render.h>
 * refer to guigfx autodocs for more
 */

#define SCCA_Bitmap_BltMask         SCC_DUMMY + 48      // I.. (PLANEPTR)

#define SCCV_Bitmap_Fill_BlitType_Clipped           0
#define SCCV_Bitmap_Fill_BlitType_IgnoreClipping    1
                                                                                                                
/*------------------ Group -----------------------------*/

#define SCC_GROUP_NAME              "Group.scalos"

#define SCCA_Group_Child            SCC_DUMMY + 31      // I.. (Object *)
#define SCCA_Group_Type             SCC_DUMMY + 46      // I.. (ULONG)

// SCCA_Group_Type is one of ...
#define SCCV_Group_HGroup           1                   // default
#define SCCV_Group_VGroup           2

/*------------------ Panel -----------------------------*/

#define SCC_PANEL_NAME              "Panel.scalos"

#define SCCA_Panel_Left             SCC_DUMMY + 60      // ISG (LONG)
#define SCCA_Panel_Top              SCC_DUMMY + 61      // ISG (LONG)

#define SCCA_Panel_MinX             SCC_DUMMY + 62      // ISG (LONG)
#define SCCA_Panel_MinY             SCC_DUMMY + 63      // ISG (LONG)
#define SCCA_Panel_MaxX             SCC_DUMMY + 73      // ISG (LONG)
#define SCCA_Panel_MaxY             SCC_DUMMY + 74      // ISG (LONG)

#define SCCA_Panel_Background       SCC_DUMMY + 64      // I.. (Object *)

#define SCCM_Panel_Redraw           SCC_DUMMY + 65      // return type: void
struct SCCP_Panel_Redraw
{
        ULONG   MethodID;
        ULONG   drawtype;                   // defined in SCCM_Area_Draw
};


/*------------------ PanelView -----------------------------*/

#define SCC_PANELVIEW_NAME          "PanelView.scalos"

#define SCCA_PanelView_Panel        SCC_DUMMY + 66      // I.. (Object *)

#define SCCA_PanelView_Left         SCC_DUMMY + 67      // ISG (LONG)
#define SCCA_PanelView_Top          SCC_DUMMY + 68      // ISG (LONG)

#define SCCA_PanelView_MinX         SCC_DUMMY + 69      // ISG (LONG)
#define SCCA_PanelView_MinY         SCC_DUMMY + 70      // ISG (LONG)
#define SCCA_PanelView_MaxX         SCC_DUMMY + 71      // ISG (LONG)
#define SCCA_PanelView_MaxY         SCC_DUMMY + 72      // ISG (LONG)


/*------------------- Text ---------------------------------*/

#define SCC_TEXT_NAME               "Text.scalos"

#define SCCA_Text_String            SCC_DUMMY + 87      // ISG (char *)
#define SCCA_Text_Type              SCC_DUMMY + 88      // IS. (ULONG)
#define SCCA_Text_Style             SCC_DUMMY + 89      // IS. (UBYTE)
#define SCCA_Text_TextPen           SCC_DUMMY + 90      // IS. (ULONG)
#define SCCA_Text_OutlinePen        SCC_DUMMY + 93      // IS. (ULONG)
#define SCCA_Text_BackPen           SCC_DUMMY + 91      // IS. (ULONG)
#define SCCA_Text_Font              SCC_DUMMY + 92      // IS. (struct TextAttr *)

#define SCCV_Text_Type_Normal           0
#define SCCV_Text_Type_Outline          1
#define SCCV_Text_Type_Shadow           2
#define SCCV_Text_BackPen_Transparent   -2

/*-------------------------- Icon --------------------------------------- */

#define SCC_ICON_NAME             "Icon.scalos"

#define SCCA_Icon_Selected          SCC_DUMMY + 84      // ISG (BOOL)
#define SCCA_Icon_GraphicNormal     SCC_DUMMY + 85      // IS. (Object *)
#define SCCA_Icon_GraphicSelected   SCC_DUMMY + 86      // IS. (Object *)
#define SCCA_Icon_IsList            SCC_DUMMY + 100     // ..G (BOOL)
#define SCCA_Icon_Name              SCC_DUMMY + 101     // I.G (char *)
#define SCCA_Icon_Table             SCC_DUMMY + 122     // ..G (Object *)

#define SCCA_Icon_NoPosition        SCC_DUMMY + 134     // ISG (BOOL)
#define SCCA_Icon_Type              SCC_DUMMY + 135     // ISG (UBYTE)
#define SCCA_Icon_DefaultTool       SCC_DUMMY + 136     // ISG (char *)
#define SCCA_Icon_StackSize         SCC_DUMMY + 137     // ISG (ULONG)
#define SCCA_Icon_WindowLeft        SCC_DUMMY + 138     // ISG (WORD)
#define SCCA_Icon_WindowTop         SCC_DUMMY + 139     // ISG (WORD)
#define SCCA_Icon_WindowWidth       SCC_DUMMY + 140     // ISG (UWORD)
#define SCCA_Icon_WindowHeight      SCC_DUMMY + 141     // ISG (UWORD)
#define SCCA_Icon_WindowOffsetX     SCC_DUMMY + 142     // ISG (LONG)
#define SCCA_Icon_WindowOffsetY     SCC_DUMMY + 143     // ISG (LONG)
#define SCCA_Icon_WindowViewMode    SCC_DUMMY + 144     // ISG (UWORD)
#define SCCA_Icon_WindowAllFiles    SCC_DUMMY + 145     // ISG (BOOL)
#define SCCA_Icon_RawIconType       SCC_DUMMY + 146     // ISG (ULONG)
#define SCCA_Icon_ToolTypes         SCC_DUMMY + 147     // ISG (Object *)
#define SCCA_Icon_Path              SCC_DUMMY + 163     // I.. (char *)

#define SCCM_Icon_Open              SCC_DUMMY + 99      // return type: ULONG
struct SCCP_Icon_Open
{
        ULONG MethodID;
        ULONG type;
};

#define SCCM_Icon_GetObject         SCC_DUMMY + 124     // return type: ULONG
struct SCCP_Icon_GetObject
{
        ULONG MethodID;
        struct TagItem *taglist;
};

#define SCCM_Icon_Entry             SCC_DUMMY + 123    // return type: ULONG
struct SCCP_Icon_Entry
{
        ULONG MethodID;
        ULONG Pos;
};

#define SCCM_Icon_Close             SCC_DUMMY + 94      // return type: void
struct SCCP_Icon_Close
{
        ULONG MethodID;
        ULONG  handle;
};

#define SCCM_Icon_Read              SCC_DUMMY + 95      // return type: ULONG
struct SCCP_Icon_Read
{
        ULONG MethodID;
        ULONG handle;
        APTR  memory;
        ULONG size;
};

#define SCCM_Icon_OpenIcon          SCC_DUMMY + 133     // return type: APTR
struct SCCP_Icon_OpenIcon
{
        ULONG MethodID;
        char *ext;
};

#define SCCM_Icon_ReadIcon          SCC_DUMMY + 96      // return type: ULONG
struct SCCP_Icon_ReadIcon
{
        ULONG MethodID;
        APTR handle;
        APTR mem;
        ULONG size;
};

#define SCCM_Icon_CloseIcon         SCC_DUMMY + 97      // return type: void
struct SCCP_Icon_CloseIcon
{
        ULONG MethodID;
        APTR  handle;
};

#define SCCM_Icon_Write             SCC_DUMMY + 98      // return type: ULONG
struct SCCP_Icon_Write
{
        ULONG MethodID;
        ULONG handle;
        APTR  memory;
        ULONG size;
};

#define SCCM_Icon_GetString         SCC_DUMMY + 130     // return type: ULONG
struct SCCP_Icon_GetString
{
        ULONG MethodID;
        ULONG Attr;
};

#define SCCM_Icon_GetIcon           SCC_DUMMY + 132     // return type: ULONG
struct SCCP_Icon_GetIcon
{
        ULONG MethodID;
        ULONG type;
};

#define SCCM_Icon_PopupMenu         SCC_DUMMY + 171     // return type: ULONG
struct SCCP_Icon_PopupMenu
{
        ULONG MethodID;
        ULONG type;
};

#define SCCV_Icon_Entry_First           1
#define SCCV_Icon_Entry_Last            2
#define SCCV_Icon_Entry_Next            3
#define SCCV_Icon_Entry_Previous        4
#define SCCV_Icon_Entry_None            0

#define SCCV_Icon_GetIcon_Disk          0
#define SCCV_Icon_GetIcon_Default       1
#define SCCV_Icon_GetIcon_DiskDefault   2

#define SCCV_Icon_PopupMenu_ClickMenu   0
#define SCCV_Icon_PopupMenu_DropMenu    1

/*-------------------------- DOSIcon ------------------------------------ */

#define SCC_DOSICON_NAME          "DOSIcon.scalos"

#define SCCA_DOSIcon_Lock           SCC_DUMMY + 102     // ISG (BPTR)
#define SCCA_DOSIcon_Size           SCC_DUMMY + 103     // I.G (ULONG)
#define SCCA_DOSIcon_Protection     SCC_DUMMY + 125     // I.G (ULONG)
#define SCCA_DOSIcon_Date           SCC_DUMMY + 126     // I.G (struct DateStamp *)
#define SCCA_DOSIcon_Comment        SCC_DUMMY + 127     // I.G (char *)
#define SCCA_DOSIcon_OwnerUID       SCC_DUMMY + 128     // I.G (WORD)
#define SCCA_DOSIcon_OwnerGID       SCC_DUMMY + 129     // I.G (WORD)
#define SCCA_DOSIcon_FileType       SCC_DUMMY + 131     // I.G (LONG)

#define SCCA_DOSIcon_FileType_Root          ST_ROOT
#define SCCA_DOSIcon_FileType_Dir           ST_USERDIR
#define SCCA_DOSIcon_FileType_SoftLinkDir   ST_SOFTLINK
#define SCCA_DOSIcon_FileType_LinkDir       ST_LINKDIR
#define SCCA_DOSIcon_FileType_File          ST_FILE
#define SCCA_DOSIcon_FileType_LinkFile      ST_LINKFILE
#define SCCA_DOSIcon_FileType_SoftLinkFile  -10             // not standard DOS

/*-------------------------- DOSListIcon -------------------------------- */

#define SCC_DOSLISTICON_NAME      "DOSListIcon.scalos"

/*-------------------------- AppIcon ------------------------------------ */

#define SCC_APPICON_NAME          "AppIcon.scalos"

#define SCCA_AppIcon_MsgPort        SCC_DUMMY + 104     // ISG (struct MsgPort *)
#define SCCA_AppIcon_ID             SCC_DUMMY + 105     // ISG (ULONG)
#define SCCA_AppIcon_UserData       SCC_DUMMY + 106     // ISG (ULONG)
#define SCCA_AppIcon_Lock           SCC_DUMMY + 107     // ISG (BPTR)

/*-------------------------- DOSDeviceIcon ------------------------------ */

#define SCC_DOSDEVICEICON_NAME    "DOSDeviceIcon.scalos"

#define SCCA_DOSDeviceIcon_DeviceName SCC_DUMMY + 108     // I.G (char *)
#define SCCA_DOSDeviceIcon_Handler    SCC_DUMMY + 120     // I.G (struct MsgPort *)
#define SCCA_DOSDeviceIcon_AssignType SCC_DUMMY + 109     // I.G (LONG)

/*--------------------------- DOSDirIcon -------------------------------- */

#define SCC_DOSDIRICON_NAME      "DOSDirIcon.scalos"

/*--------------------------- DOSFileIcon -------------------------------- */

#define SCC_DOSFILEICON_NAME      "DOSFileIcon.scalos"

/*-------------------------- AmigaIcon ---------------------------------- */

#define SCC_AMIGAICON_NAME        "AmigaIcon.scalos"

#define SCCM_AmigaIcon_Add          SCC_DUMMY + 112     // return type: BOOL
struct SCCP_AmigaIcon_Add
{
        ULONG   MethodID;
        struct  SC_Class *class;
        char    *classname;
        char    *name;
};

#define SCCM_AmigaIcon_Remove       SCC_DUMMY + 121     // return type: BOOL
struct SCCP_AmigaIcon_Remove
{
        ULONG   MethodID;
        struct  SC_Class *class;
        char    *classname;
};

/*---------------------------- Table ------------------------------------ */

#define SCC_TABLE_NAME            "Table.scalos"

#define SCCA_Table_Attribute        SCC_DUMMY + 110     // ..G (ULONG)
#define SCCA_Table_Name             SCC_DUMMY + 111     // ..G (char *)
#define SCCA_Table_Count            SCC_DUMMY + 113     // ..G (ULONG)
#define SCCA_Table_Type             SCC_DUMMY + 114     // ..G (ULONG)
#define SCCA_Table_Flags            SCC_DUMMY + 115     // ..G (ULONG)
#define SCCA_Table_Size             SCC_DUMMY + 116     // ..G (ULONG)

#define SCCV_Table_Type_String      0
#define SCCV_Table_Type_Long        1
#define SCCV_Table_Type_Double      2
#define SCCV_Table_Type_Date        3
#define SCCV_Table_Type_Raw         4
#define SCCV_Table_Type_FileType    5       // LONG

#define SCCM_Table_Add              SCC_DUMMY + 117     // return type: BOOL
struct SCCP_Table_Add
{
        ULONG   MethodID;
        ULONG   Attr;
        char    *Name;
        ULONG   Type;
        ULONG   Flags;
        ULONG   Size;
};

#define SCCM_Table_Remove           SCC_DUMMY + 118     // return type: void
struct SCCP_Table_Remove
{
        ULONG   MethodID;
        ULONG   Attr;
};

#define SCCM_Table_Entry            SCC_DUMMY + 119     // return type: ULONG
struct SCCP_Table_Entry
{
        ULONG   MethodID;
        ULONG   Pos;
};

#define SCCV_Table_Entry_First      0
#define SCCV_Table_Entry_Last       1
#define SCCV_Table_Entry_Next       2
#define SCCV_Table_Entry_Previous   3

/*---------------------------- NodeList ---------------------------------- */

#define SCC_NODELIST_NAME            "NodeList.scalos"

#define SCCA_NodeList_Argument      SCC_DUMMY + 148     // ..G (ULONG) (char *)
#define SCCA_NodeList_StringList    SCC_DUMMY + 149     // I.. (BOOL)

#define SCCM_NodeList_Insert        SCC_DUMMY + 150     // return type: BOOL
struct SCCP_NodeList_Insert
{
        ULONG   MethodID;
        ULONG   arg;
        ULONG   pos;
};

#define SCCM_NodeList_Remove        SCC_DUMMY + 151     // return type: void

#define SCCM_NodeList_Entry         SCC_DUMMY + 152     // return type: BOOL
struct SCCP_NodeList_Entry
{
        ULONG   MethodID;
        ULONG   Pos;
};

#define SCCV_NodeList_Entry_First      0
#define SCCV_NodeList_Entry_Last       1
#define SCCV_NodeList_Entry_Next       2
#define SCCV_NodeList_Entry_Previous   3

#define SCCV_NodeList_Insert_Before    1
#define SCCV_NodeList_Insert_After     0

/*---------------------------- IconPanel -------------------------------- */

#define SCC_ICONPANEL_NAME            "IconPanel.scalos"

#define SCCM_IconPanel_Add          SCC_DUMMY + 154
struct SCCP_IconPanel_Add
{
        ULONG   MethodID;
        Object  *iconobj;
};

#define SCCM_IconPanel_Remove       SCC_DUMMY + 155
struct SCCP_IconPanel_Remove
{
        ULONG   MethodID;
        ULONG   itemid;
};

#define SCCM_IconPanel_GetEntry     SCC_DUMMY + 156
struct SCCP_IconPanel_GetEntry
{
        ULONG   MethodID;
        ULONG   which;
        ULONG   itemid;
};

#define SCCM_IconPanel_GetEntryPos  SCC_DUMMY + 157
struct SCCP_IconPanel_GetEntryPos
{
        ULONG   MethodID;
        LONG    x;
        LONG    y;
};

#define SCCV_IconPanel_First            0
#define SCCV_IconPanel_FirstSelected    1
#define SCCV_IconPanel_Next             2
#define SCCV_IconPanel_NextSelected     3
#define SCCV_IconPanel_Previous         4
#define SCCV_IconPanel_PreviousSelected 5
#define SCCV_IconPanel_Last             6
#define SCCV_IconPanel_LastSelected     7
#define SCCV_IconPanel_Active           8

/*------------------ PopupMenu --------------------------*/

#define SCC_POPUPMENU_NAME          "PopupMenu.scalos"

#define SCCA_PopupMenu_Item         SCC_DUMMY + 164     // IS. (Object *)
#define SCCA_PopupMenu_Window       SCC_DUMMY + 165     // IS. (Object *)

#define SCCM_PopupMenu_Open         SCC_DUMMY + 166     // return type: ULONG

/*------------------ MenuItem -----------------------------*/

#define SCC_MENUITEM_NAME           "MenuItem.scalos"

#define SCCA_MenuItem_Name          SCC_DUMMY + 167     // I.. (char *)
#define SCCA_MenuItem_PopupItem     SCC_DUMMY + 168     // ..G (Object *)
#define SCCA_MenuItem_Menu          SCC_DUMMY + 169     // I.. (Object *)
#define SCCA_MenuItem_Activated     SCC_DUMMY + 170     // .SG (ULONG)

/* ---------------------  Messages -------------------------------------- */

struct SC_Message
{
        struct  Message execmsg;
        ULONG   scalosmid;              // SCALOS_MSGID
        UWORD   id;                     // MessageID
        UWORD   flags;                  // Flags (see below)
        ULONG   returnvalue;            // ReturnValue for replys
};

enum
{
        SCMSG_QUIT = 1,
        SCMSG_METHOD,
        SCMSG_STARTUP,
        SCMSG_SETIDCMP,
        SCMSG_NOTIFY,
};

#define SCMF_InterMsg       0x00000001


/*--------- objects derived from AreaClass are "ScalosObject"s ---------- */

struct SC_RenderInfo
{
        Object *screenobj;
        Object *windowobj;
        struct Window *window;
        struct RastPort *rport;
};


struct SC_AreaObject
{
        struct SC_RenderInfo *rinfo;
        struct IBox bounds;
};

struct SC_GraphicObject
{
        struct IBox       bounds;
        Object           *screenobj;
        struct RastPort  *rastport;
};

struct SC_MinMax
{
        UWORD   minwidth;
        UWORD   minheight;
        UWORD   maxwidth;
        UWORD   maxheight;
        UWORD   defwidth;
        UWORD   defheight;
};

struct SC_Class
{
        struct Hook     Dispatcher;
        struct SC_MethodData    *Methods;
        struct SC_Class *Super;
        char            *ClassName;
        ULONG           InstOffset;
        ULONG           InstSize;
        ULONG           UserData;
        ULONG           SubclassCount;
        ULONG           ObjectCount;
        ULONG           Flags;
};

#define SCCLF_PPC               0x00000001          /* PPC native class */

#define SCINST_DATA( cl, o ) ((VOID *) (((UBYTE *)o) + cl->InstOffset))

struct SC_MethodData
{
        ULONG   MethodID;
        ULONG   UserData;       /* can be used as function pointer */
        ULONG   Size;           /* size of all arguments */
        ULONG   Flags;
        struct  SC_MethodArg *Args;
};

#define SCMDF_DIRECTMETHOD      0x00000001  /* Method will always be called direct */
#define SCMDF_FULLFLUSH         0x00000002  /* Method is to complex to flush it only partialy */

#define SCMETHOD_DONE           -1          /* Used to make the end of the MethodData array */

struct SC_MethodArg
{
        struct  MinNode node;
        ULONG   ID;
        ULONG   Offset;
        ULONG   Size;
};


/* This structure is located before the real objectdata starts */

struct SC_Object
{
        struct MinNode  sco_Node;
        struct SC_Class *sco_Class;
};

/* convenient typecast  */
#define _SCOBJ( o ) ((struct SC_Object *)(o))

/* get "public" handle on baseclass instance from real beginning of obj data */
#define SCBASEOBJECT( _obj )    ( (Object *) (_SCOBJ(_obj)+1) )

/* get back to object data struct from public handle */
#define _SCOBJECT( o )      (_SCOBJ(o) - 1)

/* get class pointer from an object handle  */
#define SCOCLASS( o )   ( (_SCOBJECT(o))->sco_Class )

/*--------------- Macros -------------------------------*/

#define scRenderInfo(o) (((struct SC_AreaObject *) (o))->rinfo)

#define _scrport(o) (scRenderInfo(o)->rport)
#define _scwinobj(o) (scRenderInfo(o)->windowobj)
#define _scwindow(o) (scRenderInfo(o)->window)
#define _scleft(o) (((struct SC_AreaObject *) (o))->bounds.Left)
#define _sctop(o) (((struct SC_AreaObject *) (o))->bounds.Top)
#define _scwidth(o) (((struct SC_AreaObject *) (o))->bounds.Width)
#define _scheight(o) (((struct SC_AreaObject *) (o))->bounds.Height)
#define _scright(o) (_scleft(o) + _scwidth(o) - 1)
#define _scbottom(o) (_sctop(o) + _scheight(o) - 1)

/*------------------------------------------------------*/

#endif /* SCALOS_H */

// OM_GET method returns ULONG in a Scalos application !!
// OM_SET method returns void in a Scalos application !!

#define IsMinListEmpty(x) \
                ( ((x)->mlh_TailPred) == (struct MinNode *)(x) )

