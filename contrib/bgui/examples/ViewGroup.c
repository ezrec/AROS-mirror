/*
 * @(#) $Header$
 *
 * ViewGroup.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1994-1995 Jan van den Baard.
 * (C) Copyright 1994-1995 Jaba Development.
 * All Rights Reserved.
 *
 * Description: Test program for the clipped view groups.
 *
 * $Log$
 * Revision 42.4  2004/06/17 07:38:47  chodorowski
 * Added missing REGFUNC_END.
 *
 * Revision 42.3  2000/07/06 22:42:59  bergers
 * Bugfix.
 *
 * Revision 42.2  2000/07/04 05:02:22  bergers
 * Made examples compilable.
 *
 * Revision 42.1  2000/05/15 19:29:50  stegerg
 * replacements for REG macro.
 *
 * Revision 42.0  2000/05/09 22:20:13  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:34:02  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:17  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.4  1999/08/01 22:50:34  mlemos
 * Made the assignment of the mani view scroll bars be done after creating the
 * view.
 *
 * Revision 1.1.2.3  1998/12/08 03:53:32  mlemos
 * Added a few more entries to the listview in the nested clipped view object.
 *
 * Revision 1.1.2.2  1998/11/20 23:31:47  mlemos
 * Added a page in the main View object with another view nested inside of it.
 *
 * Revision 1.1.2.1  1998/10/18 17:51:41  mlemos
 * Ian sources.
 *
 *
 *
 */

/* Just execute to compile
dcc test.c -l bgui -proto -ms -mi -s -v
quit
*/

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>
#include <clib/alib_protos.h>
#include <proto/bgui.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#include <dos/dos.h>
#include <dos/dostags.h>

#include <stdio.h>
#include <stdlib.h>

#define SYSBASE()   (*((ULONG *)4))

Object *TestWindow, *text, *GO_BottomProp, *GO_RightProp;
struct Window *win;
struct Library *BGUIBase;
struct IntuitionBase * IntuitionBase;

/*
** Cycle and Mx labels.
**/
UBYTE *PageLab[] = { "Information", "Nested View", "Buttons", "Strings", "CheckBoxes", "Radio-Buttons", NULL };
UBYTE *MxLab[]  = { "MX #1",   "MX #2",   "MX #3",      "MX #4",         NULL };
UBYTE *ListViewLab[]  = { "Entry 1",   "Entry 2",   "Entry 3",  "Entry 4",  "Entry 5",  "Entry 6",  NULL };

/*
** Cycle to Page map-list.
**/
ULONG Cyc2Page[] = { MX_Active, PAGE_Active, TAG_END };

/*
** Tabs-key control of the tabs gadget.
**/
//SAVEDS ASM VOID TabHookFunc( REG(a0) struct Hook *hook, REG(a2) Object *obj, REG(a1) struct IntuiMessage *msg )
SAVEDS ASM REGFUNC3(VOID, TabHookFunc,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct IntuiMessage *, msg))
{
   struct Window        *window;
   Object            *mx_obj = ( Object * )hook->h_Data;
   ULONG           pos;

   /*
   ** Obtain window pointer and
   ** current tab position.
   **/
   GetAttr( WINDOW_Window, obj,  ( ULONG * )&window );
   GetAttr( MX_Active,  mx_obj, &pos );

   /*
   ** What key is pressed?
   **/
   if ( msg->Code == 0x42 ) {
      if ( msg->Qualifier & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) pos--;
      else                          pos++;
      SetGadgetAttrs(( struct Gadget * )mx_obj, window, NULL, MX_Active, pos, TAG_END );
   }
}
REGFUNC_END

struct Hook TabHook = { NULL, NULL, ( HOOKFUNC )TabHookFunc, NULL, NULL };


int main(void)
{
   Object            *c, *p, *m, *v, *s1, *s2, *s3;

   IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

   BGUIBase = OpenLibrary("bgui.library",37);

   if(!BGUIBase || !IntuitionBase) {
       exit(20);
   }

   /*
   ** Create tabs-object.
   **/
   c = Tabs( NULL, PageLab, 0, 0 );

   /*
   ** Put it in the hook data.
   **/
   TabHook.h_Data = (APTR)c;

   TestWindow = WindowObject,
      WINDOW_Title,           "Border gadgets",
      WINDOW_AutoKeyLabel,    TRUE,
      WINDOW_AutoAspect,      TRUE,
      WINDOW_IDCMPHookBits,   IDCMP_RAWKEY,
      WINDOW_IDCMPHook,       &TabHook,

      WINDOW_TBorderGroup,
          HGroupObject,
             StartMember, ButtonObject, SYSIA_Which, DEPTHIMAGE, GA_ID, 30, EndObject, EndMember,
             StartMember,
                ButtonObject,
                   FRM_Type,            FRTYPE_BORDER,
                   LAB_Label,           "Help...",
                   BUTTON_EncloseImage, TRUE,
                   FRM_InnerOffsetTop,  -1,
                   GA_ID,               40,
                EndObject,
             EndMember,
          EndObject,
      WINDOW_BBorderGroup,
          HGroupObject,
             StartMember,
                GO_BottomProp = PropObject,
                   PGA_Top,        0,
                   PGA_Total,      1,
                   PGA_Visible,    1,
                   PGA_Arrows,     TRUE,
                   PGA_Freedom,    FREEHORIZ,
                   PGA_NewLook,    TRUE,
                   GA_ID,          101,
                EndObject,
             EndMember,
          EndObject,
      WINDOW_RBorderGroup,
          VGroupObject,
             StartMember,
                GO_RightProp = PropObject,
                   PGA_Top,        0,
                   PGA_Total,      1,
                   PGA_Visible,    1,
                   PGA_Arrows,     TRUE,
                   PGA_Freedom,    FREEVERT,
                   PGA_NewLook,    TRUE,
                   GA_ID,          102,
                EndObject,
             EndMember,
          EndObject,

      WINDOW_MasterGroup,
         VGroupObject,
            StartMember,
         v = ViewObject, VIEW_ScaleMinWidth, 25, VIEW_ScaleMinHeight, 25, FRM_Type, FRTYPE_NONE, VIEW_HScroller, NULL, VIEW_VScroller, NULL, VIEW_Object,
         VGroupObject, NormalOffset, NormalSpacing,
            StartMember, c, FixMinHeight, EndMember,
            StartMember,
               p = PageObject,
                  /*
                  ** Info page.
                  **/
                  PageMember,
                     VGroupObject, NormalSpacing,
                        StartMember,
                           text = InfoObject, ButtonFrame, FRM_Flags, FRF_RECESSED,
                              INFO_TextFormat,   "This is also an updated example of how to manage border gadgets\n"
                                                 "with BGUI.\n\n"
                                                 "1. It is not necessary (or recommended) to add GA_#?Border\n"
                                                 "tags anymore, BGUI now handles this automatically.\n\n"
                                                 "2. You may use FRTYPE_BORDER for gadgets to get the proper\n"
                                                 "fill colors.\n\n"
                                                 "3. You do not need to specify any frames for prop gadgets,\n"
                                                 "these are done automatically.\n\n"
                                                 "4. Prop gadgets that are in borders now use sysiclass images\n"
                                                 "for their arrows for a more conforming look.\n",
                              INFO_FixTextWidth, TRUE,
                              INFO_MinLines,     14,
                              GA_ID,             50,
                           EndObject,
                        EndMember,
                        StartMember,
                        HGroupObject,
                           VarSpace(50),
                              StartMember,
                                 ButtonObject, GA_ID, 1001,
                                    Label( "_Quit" ),
                                 EndObject,
                              EndMember,
                              VarSpace(50),
                           EndObject, FixMinHeight,
                        EndMember,
                     EndObject,

                  /*
                  ** Nested view page.
                  **/
                  PageMember,
                     VGroupObject,
                        VarSpace(DEFAULT_WEIGHT),
                        StartMember,
                           HGroupObject,
                              VarSpace( DEFAULT_WEIGHT ),
                              StartMember,
                                 ViewObject, VIEW_ScaleMinWidth, 50, VIEW_ScaleMinHeight, 50, FRM_Type, FRTYPE_NONE, VIEW_Object,
                                    VGroupObject, NormalHOffset, NormalSpacing,
                                       StartMember,
                                          InfoFixed(NULL, ISEQ_C "ListView Object\nwithin a view object", NULL, 2 ), FixMinHeight,
                                       EndMember,
                                       StartMember,
                                          ListviewObject,
                                             LISTV_EntryArray,		ListViewLab,
                                          EndObject,
                                       EndMember,
                                    EndObject,
                                 EndObject,
                              EndMember,
                              VarSpace( DEFAULT_WEIGHT ),
                           EndObject, FixMinHeight,
                        EndMember,
                        VarSpace(DEFAULT_WEIGHT),
                     EndObject,

                  /*
                  ** Button page.
                  **/
                  PageMember,
                     VGroupObject, NormalSpacing,
                        VarSpace(DEFAULT_WEIGHT),
                        StartMember, PrefButton("Button #_1", 21), FixMinHeight, EndMember,
                        StartMember, PrefButton("Button #_2", 22), FixMinHeight, EndMember,
                        StartMember, PrefButton("Button #_3", 23), FixMinHeight, EndMember,
                        VarSpace(DEFAULT_WEIGHT),
                     EndObject,
                  /*
                  ** String page.
                  **/
                  PageMember,
                     VGroupObject, NormalSpacing,
                        VarSpace(DEFAULT_WEIGHT),
                        StartMember, s1 = PrefString("String #_1", "", 256, 31), FixMinHeight, EndMember,
                        StartMember, s2 = PrefString("String #_2", "", 256, 32), FixMinHeight, EndMember,
                        StartMember, s3 = PrefString("String #_3", "", 256, 33), FixMinHeight, EndMember,
                        VarSpace(DEFAULT_WEIGHT),
                     EndObject,
                  /*
                  ** CheckBox page.
                  **/
                  PageMember,
                     VGroupObject, NormalSpacing,
                        VarSpace(DEFAULT_WEIGHT),
                        StartMember,
                           HGroupObject,
                              VarSpace(DEFAULT_WEIGHT),
                              StartMember,
                                 VGroupObject, NormalSpacing,
                                    StartMember, PrefCheckBox("CheckBox #_1", FALSE, 41), EndMember,
                                    StartMember, PrefCheckBox("CheckBox #_2", FALSE, 42), EndMember,
                                    StartMember, PrefCheckBox("CheckBox #_3", FALSE, 43), EndMember,
                                 EndObject, FixMinWidth,
                              EndMember,
                              VarSpace(DEFAULT_WEIGHT),
                           EndObject,
                        EndMember,
                        VarSpace(DEFAULT_WEIGHT),
                     EndObject,
                  /*
                  ** Mx page.
                  **/
                  PageMember,
                     VGroupObject,
                        VarSpace(DEFAULT_WEIGHT),
                        StartMember,
                           HGroupObject,
                              VarSpace( DEFAULT_WEIGHT ),
                              StartMember, m = PrefMx("_Mx Object", MxLab, 0, 51), EndMember,
                              VarSpace( DEFAULT_WEIGHT ),
                           EndObject, FixMinHeight,
                        EndMember,
                        VarSpace(DEFAULT_WEIGHT),
                     EndObject,
               EndObject,
            EndMember,
         EndObject,
         EndObject,
         EndMember, EndObject,
   EndObject;

   if(!TestWindow) {
       exit(5);
   }

   /*
   ** Connect the cycle to the page.
   **/
   AddMap( c, p, Cyc2Page );
   /*
   ** Set tab-cycling order.
   **/
   DoMethod( TestWindow, WM_TABCYCLE_ORDER, s1, s2, s3, NULL );

   /*
    * We use these to supply our own scrollers.
    * If we wanted to use default scrollers, we would not use these SetAttrs.
    */

   win = WindowOpen( TestWindow );

    if(win) {
        BOOL quit = FALSE;
        ULONG rc, sigmask, sig;

        SetGadgetAttrs((struct Gadget *)v, win, NULL, VIEW_HScroller, GO_BottomProp, VIEW_VScroller, GO_RightProp,  TAG_DONE);

        GetAttr( WINDOW_SigMask, TestWindow, &sigmask );

        while(!quit) {
            sig = Wait(sigmask | SIGBREAKF_CTRL_C);

            if( sig & SIGBREAKF_CTRL_C )
                quit = TRUE;

            if( sig & sigmask ) {

                while( (rc = HandleEvent( TestWindow )) != WMHI_NOMORE)
                {
                    switch(rc)
                    {
                    case WMHI_CLOSEWINDOW:
                    case 1001: /* Quit gadget. I am just lazy. */
                        //BGUI_Request( NULL, &req, TAG_DONE );
                        quit = TRUE;
                        break;
                    }
                }
            }
        }
    }

    DisposeObject( TestWindow );

    CloseLibrary(BGUIBase);
    return 0;
}


