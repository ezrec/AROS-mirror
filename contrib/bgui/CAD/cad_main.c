/*
 * @(#) $Header$
 *
 * Change AutoDoc program
 * cad_main.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/05/14 23:32:49  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:10:54  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:23:16  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:55:39  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.12  1999/08/29 20:33:28  mlemos
 * Made the program create a unique temporary file before calling the editor
 * to allow multiple instances of the program to edit autodoc nodes.
 *
 * Revision 1.1.2.11  1999/08/21 19:26:21  mlemos
 * Integrated Anton Rolls changes to fix the command line argument template
 * handling and XML format support.
 *
 * Revision 1.1.2.10  1999/05/31 02:52:49  mlemos
 * Integrated Anton Rolls changes to generate documentation files in
 * AmigaGuide and HTML formats.
 *
 * Revision 1.1.2.9  1998/12/26 21:45:18  nobody
 * Made the program window stay busy while an entry is being edited.
 *
 * Revision 1.1.2.8  1998/12/26 21:34:29  nobody
 * Added the ability to set the editor program in the user interface.
 *
 * Revision 1.1.2.7  1998/12/26 20:57:33  nobody
 * Added the ability to edit an entry by double clicking on the list.
 *
 * Revision 1.1.2.6  1998/11/29 22:59:06  mlemos
 * Made the CAD program return an error code when it can't open BGUI library.
 *
 * Revision 1.1.2.5  1998/11/28 14:57:43  mlemos
 * Added BGUI Development Team to the copyright notice.
 *
 * Revision 1.1.2.4  1998/11/28 14:47:19  mlemos
 * Undone the changes to set the list view to pre-clear the list entries as
 * that should have been the list view class default behaviour even for lists
 * with Display hooks.
 *
 * Revision 1.1.2.3  1998/11/25 15:34:52  mlemos
 * Ensured that the list requires pre-clearing because entries are defined by
 * a custom hook.
 * Fixed typo in error message when failing to open temporary file.
 *
 * Revision 1.1.2.2  1998/10/01 04:42:57  mlemos
 * Added support for the program take input and output file arguments to load
 * CAD files and generate Autodoc files respectively.
 *
 * Revision 1.1.2.1  1998/09/19 01:50:19  mlemos
 * Ian sources.
 *
 *
 *
 */

#include "cad.h"
#include <stdio.h> /* for printf() */
#include <stdlib.h>

#define EDITOR_NAME_ENVIRONMENT_VARIABLE "editor"
#define DEFAULT_EDITOR_NAME "ed -s"

/* see ReadArgs() in main()
	 These are the locations in the array which ReadArgs() fills with nice values for us.
 */
#define ARG_FROMLIST     0
#define ARG_INPUT        1
#define ARG_OUTPUT       2
#define ARG_AUTODOC      3
#define ARG_AMIGAGUIDE   4
#define ARG_HTML         5
#define ARG_XML          6
#define ARG_VERBOSE      7
#define MAX_ARG          8   /* the maximum number of args... keep me up to date! */

/* used in ReadArgs() in main() */
/* consider putting Output/M */

#define ARG_TEMPLATE "FromList/K,From=In=Input,To=Out=Output,Autodoc=Doc/S,Amigaguide=Guide/S,HTML/S,XML/S,Verbose/S"
#define EXTENDED_HELP "  usages:\n"\
	"(multiple files, multiple outputs)\n"\
	"  CAD FROMLIST listfile [DOC] [GUIDE] [HTML] [XML]\n\n"\
	"(single files)\n"\
	"  CAD [IN] file.cad [[TO] file.doc]\n"\
	"  CAD [IN] file.cad [[TO] file.guide]\n"\
	"  CAD [IN] file.cad [[TO] file.html]\n"\
	"  CAD [IN] file.cad [[TO] file.xml]\n"

#define BAD_ARG_MESSAGE "The supplied arguments do not match the template.\nType CAD ? for the template, then ? again for extra help.\n"

/*
 * Global data.
 */
UBYTE *VStr = VERSTAG;
UBYTE FileName[ 256 ], BaseName[ 64 ], GenName[ 256 ]={0}, GenDir[256]={0};
ULONG NumDesc = 0;

/*
 * Program flags.
 */
UBYTE        CadChanged = 0;

/*
 * BGUI objects.
 */
Object   *WD_Main    = NULL;
Object   *FR_File    = NULL;
Object   *LV_List    = NULL;
Object   *ST_Base    = NULL,
         *ST_Edit    = NULL,
         *ST_Editor  = NULL;
Object   *BT_Add     = NULL,
         *BT_Remove  = NULL,
         *BT_Edit    = NULL,
         *BT_Sort    = NULL,
         *BT_Quit    = NULL;

struct Library    *BGUIBase   = NULL;
struct Window     *MainWindow = NULL;

/*
 * Object ID's.
 */
#define ID_New       1
#define ID_Open      2
#define ID_Save      3
#define ID_Save_As   4
#define ID_Generate  5
#define ID_About     6
#define ID_Quit      7
#define ID_Edit      8
#define ID_List      9

/*
 * A simple menu.
 */
struct NewMenu     Menus[] = {
   Title( "Project" ),
      Item( "New",            "N",    ID_New          ),
      Item( "Open...",        "O",    ID_Open         ),
      ItemBar,
      Item( "Save",           "S",    ID_Save         ),
      Item( "Save As...",     "A",    ID_Save_As      ),
      ItemBar,
      Item( "Generate...",    "G",    ID_Generate     ),
      ItemBar,
      Item( "About...",       "?",    ID_About        ),
      ItemBar,
      Item( "Quit",           "Q",    ID_Quit         ),
   End
};

/*****************
 ** IDCMP hook. **
 *****************/

/*
 * A IDCMP hook for the window which allows us
 * to control the listview from the keyboard.
 */
//STATIC SAVEDS ASM VOID Win_IDCMP_Func( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct IntuiMessage *msg )
STATIC SAVEDS ASM REGFUNC3(VOID, Win_IDCMP_Func,
	REGPARAM(a0, struct Hook *, hook),
	REGPARAM(a2, Object *, obj),
	REGPARAM(a1, struct IntuiMessage *, msg))
{
   struct Window  *window;
   Object         *lv_obj = ( Object * )hook->h_Data;
   ULONG          set = 0;

   /*
    * Obtain window pointer.
    */
   GetAttr( WINDOW_Window,        obj,    ( ULONG * )&window );

   /*
    * What key is pressed?
    */
   switch ( msg->Code ) {

      case  0x4C:
         /*
          * UP    - Move entry up.
          * SHIFT + UP  - Move page up.
          * CTRL + UP   - Move to the top.
          */
         if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) set = LISTV_Select_Page_Up;
         else if ( msg->Qualifier & IEQUALIFIER_CONTROL )         set = LISTV_Select_First;
         else                          set = LISTV_Select_Previous;
         break;

      case  0x4D:
         /*
          * DOWN     - Move entry down.
          * SHIFT + DOWN   - Move page down.
          * CTRL + DOWN - Move to the end.
          */
         if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) set = LISTV_Select_Page_Down;
         else if ( msg->Qualifier & IEQUALIFIER_CONTROL )         set = LISTV_Select_Last;
         else                          set = LISTV_Select_Next;
         break;

      case  0x43:
      case  0x44:
         /*
          * RETURN or ENTER - Report the listview ID to the event handler.
          */
         DoMethod( obj, WM_REPORT_ID, (( struct Gadget * )lv_obj )->GadgetID, WMRIF_DOUBLE_CLICK );
         break;
   }

   /*
    * Position changed?
    */
   if ( set )
      SetGadgetAttrs(( struct Gadget * )lv_obj, window, NULL, LISTV_Select, set, TAG_END );
}

STATIC struct Hook Win_IDCMP = { NULL, NULL, (HOOKFUNC)Win_IDCMP_Func, NULL, NULL };

/*********************
 ** Listview hooks. **
 *********************/

//STATIC SAVEDS ASM APTR LV_List_Resource_Func( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvResource *lvr )
STATIC SAVEDS ASM REGFUNC3(APTR, LV_List_Resource_Func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvResource *, lvr))
{
   APTR        rc = NULL;

   switch ( lvr->lvr_Command ) {

      case  LVRC_MAKE:
         /*
          * We use what is passed in.
          */
         rc = lvr->lvr_Entry;
         break;

      case  LVRC_KILL:
         FreeVec( lvr->lvr_Entry );
         break;
   }

   return( rc );
}
//STATIC SAVEDS ASM UBYTE *LV_List_Display_Func( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvRender *lvr )
STATIC SAVEDS ASM REGFUNC3(UBYTE *, LV_List_Display_Func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender, *lvr))
{
   /*
    * Simply return a pointer to the node name
    * for the Listview class to render.
    */
   return( &(( CADNODE * )lvr->lvr_Entry )->cn_Name[ 0 ] );
}

//STATIC SAVEDS ASM LONG LV_List_Compare_Func( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct lvCompare *lvc )
STATIC SAVEDS ASM REGFUNC3(LONG, LV_List_Compare_Func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvCompare *, lvc))
{
   /*
    * Return the result of stricmp() on the two
    * node names.
    */
   return(( LONG )stricmp( &(( CADNODE * )lvc->lvc_EntryA )->cn_Name[ 0 ],
            &(( CADNODE * )lvc->lvc_EntryB )->cn_Name[ 0 ] ));
}

STATIC struct Hook LV_List_Resource = { NULL, NULL, (HOOKFUNC)LV_List_Resource_Func, NULL, NULL };
STATIC struct Hook LV_List_Display  = { NULL, NULL, (HOOKFUNC)LV_List_Display_Func,  NULL, NULL };
STATIC struct Hook LV_List_Compare  = { NULL, NULL, (HOOKFUNC)LV_List_Compare_Func,  NULL, NULL };

/*********************************
 ** Listview notification hook. **
 *********************************/
//STATIC SAVEDS ASM ULONG LV_List_Func( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC SAVEDS ASM REGFUNC3(ULONG, LV_List_Func,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate, *opu))
{
   struct TagItem        *tag;
   CADNODE                *cn = NULL;
   UBYTE           *contents;
   ULONG       disable;

   /*
    * Let's get the selected node.
    */
   if ( tag = FindTagItem( LISTV_Entry, opu->opu_AttrList ))
      cn = ( CADNODE * )tag->ti_Data;

   /*
    * A valid node?
    */
   if ( cn ) {
      contents = &cn->cn_Name[ 0 ];
      disable  = FALSE;
   } else {
      contents = NULL;
      disable  = TRUE;
   }

   /*
    * Update objects.
    */
   SetAttr( ST_Edit, opu->opu_GInfo, GA_Disabled, disable, STRINGA_TextVal, contents, TAG_END );
   MultiSetAttr( opu->opu_GInfo, GA_Disabled, disable, BT_Remove, BT_Edit, NULL );

   return( 1 );
}

STATIC struct Hook LV_List_Hook = { NULL, NULL, (HOOKFUNC)LV_List_Func, NULL, NULL };

/***********************************
 ** Add button notification hook. **
 ***********************************/

//STATIC SAVEDS ASM BT_Add_Func( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC SAVEDS ASM REGFUNC3(int, BT_Add_Func,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate, *opu))
{
   CADNODE                 *cn;

   /*
    * We only respond to final messages.
    */
   if ( ! ( opu->opu_Flags & OPUF_INTERIM )) {
      /*
       * First we allocate a new node.
       */
      if ( cn = ( CADNODE * )AllocVec( sizeof( CADNODE ), MEMF_CLEAR )) {
         /*
          * Copy the name "<new>" into it.
          */
         strcpy( &cn->cn_Name[ 0 ], "<new>" );

         /*
          * Add to the listview.
          */
         DoMethod( LV_List, LVM_ADDSINGLE, opu->opu_GInfo, cn, LVAP_TAIL, LVASF_SELECT );

         BGUI_DoGadgetMethod(ST_Edit, MainWindow, NULL, GM_GOACTIVE);
         ActivateGadget((struct Gadget *)ST_Edit, MainWindow, NULL);

         /*
          * One more entry.
          */
         NumDesc++;
         CadChanged = 1;

         return( 1 );
      }
   }
   return( 0 );
}

STATIC struct Hook BT_Add_Hook = { NULL, NULL, (HOOKFUNC)BT_Add_Func, NULL, NULL };

/**************************************
 ** Remove button notification hook. **
 **************************************/

//STATIC SAVEDS ASM BT_Remove_Func( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC SAVEDS ASM REGFUNC3(int, BT_Remove_Func,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
   /*
    * We only respond to final messages.
    */
   if (!(opu->opu_Flags & OPUF_INTERIM))
   {
      /*
       * Look up the selected node.
       */
      if (FirstSelected(LV_List)) {
         /*
          * Remove it from the listview.
          */
         DoMethod( LV_List, LVM_REMSELECTED, opu->opu_GInfo );

         /*
          * One less...
          */
         NumDesc--;
         CadChanged = 1;

         return 1;
      }
   }
   return 0;
}

STATIC struct Hook BT_Remove_Hook = { NULL, NULL, (HOOKFUNC)BT_Remove_Func, NULL, NULL };

/***********************************
 ** Edit string notification hook. **
 ***********************************/
//STATIC SAVEDS ASM ST_Edit_Func( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC SAVEDS ASM REGFUNC3(int, ST_Edit_Func,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
   UBYTE       *data;
   CADNODE                 *cn;

   /*
    * We only respond to final messages.
    */
   if ( ! ( opu->opu_Flags & OPUF_INTERIM )) {
      /*
       * Look up the selected node.
       */
      if ( cn = ( CADNODE * )FirstSelected( LV_List )) {
         /*
          * Obtain the new name.
          */
         GetAttr( STRINGA_TextVal, obj, ( ULONG * )&data );

         /*
          * Did it change?
          */
         if ( stricmp( data, &cn->cn_Name[ 0 ] )) {
            /*
             * Lock the list.
             */
            DoMethod( LV_List, LVM_LOCKLIST, NULL );

            /*
             * Copy the name.
             */
            strcpy( &cn->cn_Name[ 0 ], data );

            /*
             * Unlock the list.
             */
            DoMethod( LV_List, LVM_UNLOCKLIST, NULL );

            /*
             * Refresh.
             */
            DoMethod( LV_List, LVM_REDRAW, opu->opu_GInfo );
            CadChanged = 1;

            return( 1 );
         }
      }
   }
   return( 0 );
}

STATIC struct Hook ST_Edit_Hook = { NULL, NULL, (HOOKFUNC)ST_Edit_Func, NULL, NULL };

/**************************************
 ** Sort button notification hook.   **
 **************************************/

//STATIC SAVEDS ASM BT_Sort_Func(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu)
STATIC SAVEDS ASM REGFUNC3(int, BT_Sort_Func,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
   /*
    * We only respond to final messages.
    */
   if (!(opu->opu_Flags & OPUF_INTERIM))
   {
      /*
       * Sort the list.
       */
      DoMethod(LV_List, LVM_SORT, opu->opu_GInfo);

      return 1;
   }
   return 0;
}
STATIC struct Hook BT_Sort_Hook = { NULL, NULL, (HOOKFUNC)BT_Sort_Func, NULL, NULL };

/*****************************************
 ** Base name string notification hook. **
 *****************************************/
//STATIC SAVEDS ASM ST_Base_Func( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct opUpdate *opu )
STATIC SAVEDS ASM REGFUNC3(int, ST_Base_Func,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu))
{
   UBYTE       *data;

   /*
    * We only respond to final messages.
    */
   if ( ! ( opu->opu_Flags & OPUF_INTERIM )) {
      /*
       * Obtain the new name and copy
       * it into the node.
       */
      GetAttr( STRINGA_TextVal, obj, ( ULONG * )&data );
      strcpy( BaseName, data );

      CadChanged = 1;

      return( 1 );
   }
   return( 0 );
}

STATIC struct Hook ST_Base_Hook = { NULL, NULL, (HOOKFUNC)ST_Base_Func, NULL, NULL };

/*
 * Create the main window.
 */
BOOL CreateMainWindow( void )
{
   ULONG       tmp = 0;
   UBYTE editor_name[256];

   if(GetVar(EDITOR_NAME_ENVIRONMENT_VARIABLE,editor_name,sizeof(editor_name)-1,GVF_GLOBAL_ONLY|GVF_LOCAL_ONLY)<0)
      strcpy(editor_name,DEFAULT_EDITOR_NAME);
   WD_Main = WindowObject,
      WINDOW_Title,           NAME,
      WINDOW_MenuStrip,       Menus,
      WINDOW_AutoAspect,      TRUE,
      WINDOW_SmartRefresh,    TRUE,
      WINDOW_ScaleWidth,      25,
      WINDOW_ScaleHeight,     60,
      WINDOW_IDCMPHook,       &Win_IDCMP,
      WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
      WINDOW_AutoKeyLabel,    TRUE,
      WINDOW_MasterGroup,
         VGroupObject, NormalOffset, NormalSpacing,
            StartMember,
               VGroupObject,
                  StartMember,
                     VGroupObject, BOffset(4),
                        StartMember,
                           ST_Base = StringObject,
                              LAB_Label,        "_Base Name:",
                              STRINGA_MaxChars, 64,
                              STRINGA_TextVal,  BaseName,
                           EndObject,
                        EndMember,
                     EndObject, FixMinHeight,
                  EndMember,
                  StartMember,
                     LV_List = ListviewObject,
                        LISTV_ResourceHook,  &LV_List_Resource,
                        LISTV_DisplayHook,   &LV_List_Display,
                        LISTV_CompareHook,   &LV_List_Compare,
                        LISTV_ShowDropSpot,  TRUE,
                        PGA_NewLook,         TRUE,
                        PGA_Borderless,      TRUE,
                        BT_DragObject,       TRUE,
                        BT_DropObject,       TRUE,
                        GA_ID,               ID_List,
                     EndObject,
                  EndMember,
                  StartMember,
                     ST_Edit = StringObject,
                        STRINGA_MaxChars, 32,
                        GA_Disabled,      NumDesc ? FALSE : TRUE,
                     EndObject, FixMinHeight,
                  EndMember,
                  StartMember,
                     VGroupObject, TOffset(4),
                        StartMember,
                           ST_Editor = StringObject,
                              LAB_Label,        "_Text editor:",
                              STRINGA_MaxChars, 255,
                              STRINGA_TextVal, (ULONG)editor_name,
                           EndObject,
                        EndMember,
                     EndObject, FixMinHeight,
                  EndMember,
                  StartMember,
                     HGroupObject, Spacing(8), TOffset(4),
                        StartMember,
                           BT_Add = ButtonObject,
                              LAB_Label,     "_Add",
                           EndObject,
                        EndMember,
                        StartMember,
                           BT_Remove = ButtonObject,
                              LAB_Label,     "_Remove",
                              GA_Disabled,   NumDesc ? FALSE : TRUE,
                           EndObject,
                        EndMember,
                        StartMember,
                           BT_Edit = ButtonObject,
                              LAB_Label,     "_Edit",
                              GA_ID,         ID_Edit,
                              GA_Disabled,   NumDesc ? FALSE : TRUE,
                           EndObject,
                        EndMember,
                        StartMember,
                           BT_Sort = ButtonObject,
                              LAB_Label,     "_Sort",
                           EndObject,
                        EndMember,
                     EndObject, FixMinHeight,
                  EndMember,
               EndObject,
            EndMember,
         EndObject,
   EndObject;

   /*
    * Object tree OK?
    */
   if (WD_Main)
   {
      /*
       * Setip IDCMP hook.
       */
      Win_IDCMP.h_Data = (APTR)LV_List;

      /*
       * And notification.
       */
      tmp += AddHook( LV_List,   &LV_List_Hook   );
      tmp += AddHook( BT_Add,    &BT_Add_Hook    );
      tmp += AddHook( BT_Remove, &BT_Remove_Hook );
      tmp += AddHook( ST_Edit,   &ST_Edit_Hook   );
      tmp += AddHook( BT_Sort,   &BT_Sort_Hook   );
      tmp += AddHook( ST_Base,   &ST_Base_Hook   );

      if (tmp == 6)
         return TRUE;

      DisposeObject( WD_Main );
   }
   return FALSE;
}

/*
 * Run the editor to edit the
 * node data.
 */
VOID RunEditor( CADNODE *cn )
{
   BPTR        file;
   UBYTE       fname[ L_tmpnam ], rstr[ 256 ], *newdata,*editor_name;
   LONG        rc;

   /*
    * Build temporary file name.
    */
   tmpnam(fname);

   /*
    * Write old contents.
    */
   if ( file = Open( fname, MODE_NEWFILE )) {
      if ( cn->cn_DataLength ) {
         if ( Write( file, cn->cn_Data, cn->cn_DataLength ) != cn->cn_DataLength ) {
            DosRequest( "OK", ISEQ_C "Error writing temporary file!" );
            Close( file );
            return;
         }
      }
      Close( file );
   } else {
      DosRequest( "OK", ISEQ_C "Unable to open the temporary file!" );
      return;
   }

   /*
    * Build editor command string.
    */
   sprintf(rstr, "%s %s", GetAttr(STRINGA_TextVal,ST_Editor,(ULONG *)&editor_name) ? editor_name : (UBYTE *)"$editor", fname);

   WindowBusy(WD_Main);

   /*
    * Run the editor.
    */
   if ((rc = SystemTags( rstr, SYS_Input, Input(), SYS_Output, Output(), TAG_END )) == 0)
   {
      /*
       * Open the temporary file.
       */
      if ( file = Open( fname, MODE_OLDFILE )) {
         /*
          * Find out its length.
          */
              Seek( file, 0, OFFSET_END );
         rc = Seek( file, 0, OFFSET_BEGINNING );

         if ( rc > 0 ) {
            /*
             * Allocate memory for read.
             */
            if ( newdata = ( UBYTE * )AllocVec( rc, MEMF_ANY )) {
               /*
                * Load it.
                */
               if ( Read( file, newdata, rc ) == rc ) {
                  /*
                   * Setup new data and deallocate
                   * the old.
                   */
                  if ( cn->cn_Data ) FreeVec( cn->cn_Data );
                  cn->cn_Data   = newdata;
                  cn->cn_DataLength = rc;
               } else {
                  DosRequest( "OK", ISEQ_C "Error reading temporary file!" );
                  FreeVec( newdata );
               }
            } else
               MyRequest( "OK", ISEQ_C "Out of memory!" );
         } else if ( rc == -1 )
            DosRequest( "OK", ISEQ_C "Error seeking temporary file!" );
         else {
            if ( cn->cn_Data ) FreeVec( cn->cn_Data );
            cn->cn_Data   = NULL;
            cn->cn_DataLength = 0;
         }
         Close( file );
      } else
         DosRequest( "OK", ISEQ_C "Unable to open temporary file!" );
   } else if ( rc == -1 )
      DosRequest( "OK", ISEQ_C "Error running the editor!" );
   else
      DosRequest( "OK", ISEQ_C "Editor returned %ld!", rc );

   WindowReady(WD_Main);

   /* Finsihed editing */
   DeleteFile(fname); /* Delete temporary file. */
   ActivateWindow(MainWindow); /* ensure we get the focus back from the editor */
   ScreenToFront(MainWindow->WScreen); /* pop the screen where CAD has its window to the front */
}

/*
 * Pop filerequester.
 */
BOOL PopRequest( BOOL savemode, UBYTE *buffer )
{
   UBYTE        *p;
   BOOL     rc = FALSE;

   /*
    * Create requester if not done yet.
    */
   if (!FR_File)
   {
      FR_File = FileReqObject,
         ASLFR_Window,           MainWindow,
         ASLFR_SleepWindow,      TRUE,
         ASLFR_InitialPattern,   "#?.cad",
      EndObject;
   };

   /*
    * Requester available?
    */
   if (FR_File)
   {
      SetAttrs(FR_File, ASLFR_InitialFile,  FilePart(buffer),
                        ASLFR_InitialDrawer,  GenDir,
                        ASLFR_DoSaveMode,   savemode,
                        TAG_END);

      if (!DoRequest(FR_File))
      {
         GetAttr(FRQ_Path, FR_File, (ULONG *)&p);
         strcpy(buffer, p);
         rc = TRUE;
      }
   }

   return( rc );
}

/*
 * Handle events.
 */
VOID HandleEvents(void)
{
   ULONG    signal,last_entry_clicked=-1,last_click_seconds=0,last_click_micros=0;
   int      rc, running = TRUE;

   /*
    * Obtain window signal mask.
    */
   GetAttr(WINDOW_SigMask, WD_Main, &signal);

   while (running)
   {
      Wait(signal);

      while ((rc = HandleEvent(WD_Main)) != WMHI_NOMORE)
      {
         switch (rc)
         {
            case WMHI_CLOSEWINDOW:
            case ID_Quit:
               running = FALSE;
               break;

            case ID_Edit:
               RunEditor((CADNODE *)FirstSelected(LV_List));
               break;

            case ID_New:
               SetGadgetAttrs((struct Gadget *)ST_Base, MainWindow, NULL, STRINGA_TextVal, NULL, TAG_END);
               ClearList(MainWindow, LV_List);
               *FileName = *BaseName = *GenName = 0;
               NumDesc = 0;
               break;

            case ID_Open:
               if (PopRequest(FALSE, FileName))
                  LoadCadFile(1);
               break;

            case ID_Save:
               if (NumDesc)
               {
                  if (FileName[0])
                  {
                     SaveCadFile(1);
                     break;
                  };
               };
               /* fall through if no filename. */

            case ID_Save_As:
               if (NumDesc)
               {
                  sprintf(FileName, "%s.cad", BaseName);
                  if (PopRequest( TRUE, FileName ))
                     SaveCadFile(1);
               } else
                  MyRequest("OK", ISEQ_C "Nothing to save!");
               break;

            case ID_Generate:
               if (NumDesc)
               {
                  switch (MyRequest( "*_AutoDoc|_HTML|Amiga_Guide|_Cancel", "Select the output format:" ))
                  {
                    case 0: /* cancel - don't do nothin' */
                      break;

                    case 1: /* text */
                      sprintf(GenName, "%s.doc", BaseName);
                      if (PopRequest(TRUE, GenName))
                        Generate(1, MATCH_DOC);
                      break;         

                    case 2: /* html */
                      sprintf(GenName, "%s.html", BaseName);
                      if (PopRequest(TRUE, GenName)) 
                        Generate(1, MATCH_HTML);
                      break;

                    case 3: /* amigaguide */
                      sprintf(GenName, "%s.guide", BaseName);
                      if (PopRequest(TRUE, GenName))
                        Generate(1, MATCH_GUIDE);
                      break;
                  }               

               } else
                  MyRequest("OK", ISEQ_C "Nothing to generate!");
               break;

            case ID_About:
               MyRequest( "_Continue", ISEQ_C ISEQ_B NAME " " VERSION " " __AMIGADATE__ "\n\n"
                        ISEQ_N "Copyright © 1998 BGUI Development Team.\n"
                        ISEQ_N "Copyright © 1996 Ian J. Einman.\n"
                        ISEQ_N "Copyright © 1996 Jaba Development." );
               break;
            case ID_List:
            {
               ULONG last_clicked;

               if(GetAttr(LISTV_LastClickedNum, LV_List, &last_clicked))
               {
                  ULONG seconds,micros;

                  CurrentTime(&seconds,&micros);
                  if(last_entry_clicked==last_clicked
                  && DoubleClick(last_click_seconds,last_click_micros,seconds,micros))
                    RunEditor((CADNODE *)FirstSelected(LV_List));
                  last_click_seconds=seconds;
                  last_click_micros=micros;
                  last_entry_clicked=last_clicked;
               }
            }
         };
      };
   };
}

char *matchlist[] =
  { "", "fromlist", "to", "doc", "guide", "html", "xml" };

/* match a string in the list - return one of the defined MATCH_xxx, or 0 if not found
  MATCH_xxx defines are in cad.h
*/

ULONG Match(const char *string)
{
  ULONG i=0;
  while( i < MATCH_MAX )
  {
    if( stricmp(string, matchlist[i]) )
      i++;
    else
      break;
  };

  if( i < MATCH_MAX )
    return (i);
  else
    return (0);
}

/*

  These two variables are accessed by main() and ReadAListFile

 */

BOOL openMainWindow=TRUE; /* Will we open the main window? */
       /* This will be set to false when any arguments other than */
       /* INPUT or VERBOSE are specified, */
       /* or when there is an error opening a file. */
int rc=0;  /* main() return code */
LONG error=0;
BOOL generatedOK=TRUE;

/*
 * Main entry...
 */
int main(LONG argc, char *argv[])
{
  void ReadAListFile(LONG *arg);
  void SingleFile(LONG *arg);

  struct RDArgs *arguments; /* "handle" necessary for FreeArgs() */
  LONG arg[MAX_ARG]={0}; /* where the arguments will go finally. */
                                         
  struct RDArgs rdargs = {0}; /* see dos/rdargs.h */
  rdargs.RDA_ExtHelp = EXTENDED_HELP; /* set the extended help */

  arguments = ReadArgs(ARG_TEMPLATE, arg, &rdargs);

  if(!arguments)
  {
    printf(BAD_ARG_MESSAGE);
    exit(20);
  }

  /*
   * Open the bgui.library
   */
  if (BGUIBase = OpenLibrary("bgui.library", 40))
  {
    if (CreateMainWindow()) /* Create the main window object tree. */
    {
      /* See if user wants to open the window. */
      /* This becomes false when there are command line arguments other than */
      /* VERBOSE or INPUT are specified. */
      /* ie. The user tries to use the command line to generate a file directly, */
      /* without using the window, so we don't annoy him by popping it open on any */
      /* sort of error */

      if( arg[ARG_FROMLIST] ||
          arg[ARG_OUTPUT] ||
          arg[ARG_AUTODOC] ||
          arg[ARG_AMIGAGUIDE] ||
          arg[ARG_HTML] ||
          arg[ARG_XML]
        )
      {
        /* if(arg[ARG_VERBOSE])  printf("!openMainWindow\n"); */
        openMainWindow = FALSE;
      }

      /* Handle multiple file input - specified with FROMLIST */

      if( arg[ARG_FROMLIST] ) /* was FromList specified? */
      {
        /* read-a-list-file mode - multiple inputs, multiple outputs */
        ReadAListFile(arg);
      }

      /* Handle single file input - specified with IN and OUT */

      if( arg[ARG_INPUT] )  /* was an input file specified? */
      {
        SingleFile(arg); /* single file mode */
      }


      if(openMainWindow) /* ==TRUE if the user specifies incomplete or missing arguments */
      {
        if (MainWindow = WindowOpen(WD_Main)) /* Open the main window */
        {
          HandleEvents();
        }
      }
      /*
       * Dump the objects.
       */
      if (WD_Main) DisposeObject(WD_Main);
      if (FR_File) DisposeObject(FR_File);
    }
    /*
     * Close the bgui.library.
     */
    CloseLibrary(BGUIBase);
  }
  else /* couldn't open bgui.library v 40 */
  {
    rc=20;
  }

  if( !generatedOK )
  {
    rc=20;
    error=IoErr();
  }

  SetIoErr(error);
  exit(rc);

  return (0);
}

/*

  Do all the work that read-a-list-file mode entails:
  - open the file specified with FROMLIST
  - get filenames out of it one by one, and
  - generate the output files, depending on the switches: DOC, HTML, GUIDE etc.

  - main() and ReadAListFile both have access to the variables:
    openMainWindow, rc, error, and generatedOK
   - openMainWindow may be set to FALSE when there is an error opening a file.
   - rc may be set to 20 if there is an error opening a file, and
   - error records the result of IoErr() in such a case.
   - generatedOK is the BOOL result from Generate()
 */

void ReadAListFile(LONG *arg)
{
  void MakeSureFileNameIsOK(UBYTE *);

  BPTR listFile;
  UBYTE listFileName[256]={0};
  char *lastdot; /* to point to the last '.' in a filename */

  if( arg[ARG_VERBOSE] )  printf("CAD FROMLIST");

  /* try to open the list file and read the filenames to process */

  strcpy( listFileName, (char *)arg[ARG_FROMLIST]); /* get listFileName */
  if( arg[ARG_VERBOSE] )  printf(" %s", listFileName);
  if ( listFile = Open( listFileName, MODE_OLDFILE )) /* Open listFile */
  {
    if( arg[ARG_VERBOSE] )  printf(" TO");

    /* loop to process successive FileNames from the listFile */
    while( FGets(listFile, FileName, 256) ) /* read next line into FileName */
    {
      MakeSureFileNameIsOK( FileName);

      if( LoadCadFile(0) )   /* load the input file (opens FileName) */
      {
        strcpy( GenName, FileName ); /* copy the input file name to output file name */

        lastdot = strrchr(GenName, '.');  /* find the last dot in the filename */
        if( !lastdot)  /* no dot? */
        {
          strcat( GenName, "."); /* append a dot to the filename */
          lastdot = strrchr(GenName, '.');  /* find the last dot again */
        }

        /* good - now find out what types of output we want. */
        /* Generate each specified type, modifying GenName so */
        /* its extension matches its type. */

        if( arg[ARG_AUTODOC] )
        {
          strcpy(lastdot+1, "doc" );  /* append "doc" to filename */
          generatedOK = Generate(0, MATCH_DOC );
          if( arg[ARG_VERBOSE] )  printf("\n %s", GenName);
        }

        if( arg[ARG_AMIGAGUIDE] )
        {
          strcpy(lastdot+1, "guide" );  /* append "guide" to filename */
          generatedOK = Generate(0, MATCH_GUIDE );
          if( arg[ARG_VERBOSE] )  printf("\n %s", GenName);
        }

        if( arg[ARG_HTML] )
        {
          strcpy(lastdot+1, "html" );  /* append "html" to filename */
          generatedOK = Generate(0, MATCH_HTML );
          if( arg[ARG_VERBOSE] )  printf("\n %s", GenName);
        }

        if( arg[ARG_XML] )
        {
          strcpy(lastdot+1, "xml" );  /* append "xml" to filename */
          generatedOK = Generate(0, MATCH_XML );
          if( arg[ARG_VERBOSE] )  printf("\n %s", GenName);
        }
      }
      else /* couldn't open the input file - open should be FALSE */
      {
        if( arg[ARG_VERBOSE] )  printf(" [couldn't open %s]", FileName);
        openMainWindow = FALSE;
        rc=20;
        error=IoErr();
      }
    } /* end of the loop processing the listfile */

    Close(listFile);
  }
  else /* unable to open listfile */
  {
    if( arg[ARG_VERBOSE] )  printf(" [unable to open]");
    openMainWindow = FALSE;
    rc=20;
    error=IoErr();
  }
  if( arg[ARG_VERBOSE] )  printf("\n"); /* newline */
}

/*
  After the filename has been read from the FromList file,
  its last character could be null or newline (or something
  else). It should have a null in order to be a nice c string,
  so we make sure it has one.
*/

void MakeSureFileNameIsOK(UBYTE *filename)
{
  UBYTE *lastchar; /* the last character in the filename string */

  if(strlen(filename)>0)
  {
    lastchar = &filename[ strlen(filename)-1 ];

    switch( *(lastchar) )
    {
      case '\n':            /* if last char is a newline */
        *(lastchar) = '\0'; /* replace it with a terminating null */
        /* printf("\n %s [last char newline]", filename); */
        break;

      case '\0':            /* if last char is a null - good */
        /* printf("\n %s [last char null]", filename); */
        break;

      default:              /* something else ?! - weird */
        /* printf("\n %s [last char = %ld not newline or null ]", filename, (LONG) *(lastchar) ); */
        break;
    }
    /* printf("\n %s", filename); */
  }
}
     
void SingleFile(LONG *arg)
{
  /* single-file mode - single input */

  char *lastdot; /* to point to the last '.' in a filename */

  if( arg[ARG_VERBOSE] )  printf(" [single file mode]");

  strcpy( FileName, (char *)arg[ARG_INPUT] ); /* copy the input file name */
  if( LoadCadFile(0) )   /* load the input file */
  {
    /* input file opened - good */
    if( arg[ARG_VERBOSE] )  printf(" %s", FileName);

    /* now see if there is an output file */
    if( arg[ARG_OUTPUT] )
    {
      BOOL foundUsefulExtension = FALSE;
      /* take it as the output filename */
      /* see what kind of file to create by looking at filename extension */

      strcpy( GenName, (char *)arg[ARG_OUTPUT] );
      if( arg[ARG_VERBOSE] )  printf(" %s", GenName);

      if(lastdot = strrchr(GenName, '.') )  /* find the last dot */
      {
        if( strlen(GenName) > lastdot-GenName+1 )  /* chars after last dot ? */
        {
          if( Match(lastdot+1) )  /* see if we know the extension */
          {
            foundUsefulExtension = TRUE;
            generatedOK = Generate(0, Match(lastdot+1) );
          }
        }
      }

      if(!foundUsefulExtension) /* we did not find a recognized extension */
      {
        /* default to autodoc text file */
        generatedOK = Generate(0, MATCH_DOC); /* produce text file */
      }
    }
    else /* no output file specified */
    {
      /* Here we should check if DOC, GUIDE, HTML or XML tags were specified.
         If so, copy the input filename to the output filename, replace the
         extension with doc, guide etc, and Generate.
       */

      ULONG pathLength=0;

      if( arg[ARG_VERBOSE] )  printf(" [no output]");
      /* copy the Path part from the input file name to GenDir,
         so by default, the generate file requester will show the
         same path as specified for the input file. = more friendly
       */
      pathLength = (ULONG)( PathPart(FileName) - FileName );
      strncat(GenDir, FileName, pathLength);
    }
  }
  else /* couldn't open the input file */
  {
    if( arg[ARG_VERBOSE] )  printf(" [couldn't open %s]", FileName);
    openMainWindow = FALSE; /* Don't open the GUI */
    rc=20;
    error=IoErr();
  }
  if( arg[ARG_VERBOSE] )  printf("\n");
}
