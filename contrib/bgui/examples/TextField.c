/*
 * @(#) $Header$
 *
 * TextField.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:20:08  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:57  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:12  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/07/30 22:11:47  mlemos
 * Initial revision.
 *
 *
 *
 */

#include "DemoCode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <intuition/icclass.h>
#include <gadgets/textfield.h>
#include <proto/textfield.h>
#include <proto/iffparse.h>

/*
**      Library base pointers.
**/
struct Library *TextFieldBase = NULL;

/*
**      Object ID's
**/
#define ID_RIGHTPROP 1
#define ID_TEXTFIELD 2

/*
**      And were in...
**/
VOID StartDemo( void )
{
        Object                  *WN_Window, *GO_Textfield, *GO_Master, *GO_RightProp;
        struct Window           *win;
        ULONG                   winsig = 0L, sigrec, rc;
        BOOL                    running = TRUE;

        /*
        **      Open the libraries. Get display information etc...
        **/
        if ( TextFieldBase = OpenLibrary( TEXTFIELD_NAME, TEXTFIELD_VER )) {

                struct ClipboardHandle *clip_handle, *undo_handle;
                struct IntuiText text1_title;

                text1_title.FrontPen = 0;
                text1_title.BackPen = 1;  /* don't really need to set for JAM1 */
                text1_title.DrawMode = JAM1;
                text1_title.LeftEdge = 0;
                text1_title.TopEdge = 0;
                text1_title.ITextFont = NULL;
                text1_title.IText = NULL;
                text1_title.NextText = NULL;

                /* Open the clipboard; no need to verify */
                clip_handle = OpenClipboard(0);
                undo_handle = OpenClipboard(42);

                /*
                **      Create a small window.
                **/
                WN_Window = WindowObject,
                        WINDOW_Title,           "Text field",
                        WINDOW_RMBTrap,         TRUE,
                        WINDOW_ScaleWidth,      20,
                        WINDOW_ScaleHeight,     20,
                        WINDOW_AutoAspect,      TRUE,

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
                                     GA_ID,          ID_RIGHTPROP,
                                  EndObject,
                               EndMember,
                            EndObject,

                        WINDOW_MasterGroup,
                                GO_Master = VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                        StartMember,
                                                GO_Textfield = ExternalObject,
                                                        EXT_MinWidth,           30,
                                                        EXT_MinHeight,          30,
                                                        EXT_Class,              TEXTFIELD_GetClass(),
                                                        EXT_NoRebuild,          TRUE,

                                                        FRM_Type,               FRTYPE_RIDGE,

                                                        GA_FollowMouse,         TRUE,
                                                        GA_IntuiText,           &text1_title,
                                                        GA_ID,                  ID_TEXTFIELD,

                                                        TEXTFIELD_Text,         (ULONG)"Right Amiga C - Copy\nRight Amiga X - Cut\nRight Amiga V - Paste\n",
                                                        TEXTFIELD_UserAlign,    TRUE,
                                                        TEXTFIELD_BlinkRate,    500000,
                                                        TEXTFIELD_TabSpaces,    4,
                                                        TEXTFIELD_NonPrintChars,TRUE,
                                                        TEXTFIELD_ClipStream,   clip_handle,
                                                        TEXTFIELD_UndoStream,   undo_handle,

                                                EndObject,
                                        EndMember,
                                EndObject,
                EndObject;

                if ( WN_Window ) {

                        /*
                        **      Open up the window.
                        **/
                        if ( win = WindowOpen( WN_Window )) {
                                /*
                                **      Obtain window sigmask.
                                **/
                                GetAttr( WINDOW_SigMask, WN_Window, &winsig );
                                /*
                                **      Wait for messages.
                                **/
                                ActivateGadget((struct Gadget *)GO_Textfield,win,NULL);
                                do {
                                        sigrec = Wait( winsig );

                                        /*
                                        **      Window signal?
                                        **/
                                        if ( sigrec & winsig ) {
                                                while ( WN_Window && (( rc = HandleEvent( WN_Window )) != WMHI_NOMORE )) {
                                                        switch ( rc ) {

                                                                case    WMHI_CLOSEWINDOW:
                                                                        /*
                                                                        **      The end.
                                                                        **/
                                                                        running = FALSE;
                                                                        break;

                                                                case    ID_RIGHTPROP:
                                                                {
                                                                        Object *object;
                                                                        ULONG top,visible,total;

                                                                        GetAttr( EXT_Object, GO_Textfield, ( ULONG * )&object);
                                                                        GetAttr( TEXTFIELD_Top, object, &top);
                                                                        GetAttr( PGA_Visible, GO_RightProp, &visible);
                                                                        GetAttr( PGA_Total, GO_RightProp, &total);
                                                                        if(visible>total)
                                                                                visible=total;
                                                                        SetGadgetAttrs((struct Gadget *)object,win,NULL,
                                                                                TEXTFIELD_Top,top,
                                                                                TEXTFIELD_Visible,visible,
                                                                                TEXTFIELD_Lines,total,
                                                                                TAG_END);
                                                                }

                                                                case    ID_TEXTFIELD:
                                                                {
                                                                        Object *object;
                                                                        ULONG top,visible,total;

                                                                        GetAttr( EXT_Object, GO_Textfield, ( ULONG * )&object);
                                                                        GetAttr( TEXTFIELD_Top, object, &top);
                                                                        GetAttr( TEXTFIELD_Visible, object, &visible);
                                                                        GetAttr( TEXTFIELD_Lines, object, &total);
                                                                        if(visible>total)
                                                                                visible=total;
                                                                        SetGadgetAttrs((struct Gadget *)GO_RightProp,win,NULL,
                                                                                PGA_Top,top,
                                                                                PGA_Visible,visible,
                                                                                PGA_Total,total,
                                                                                TAG_END);
                                                                }
                                                        }
                                                }
                                        }
                                } while ( running );
                        }
                        /*
                        **      Kill the window.
                        **/
                        DisposeObject( WN_Window );
                }

                /* Close the clipboard */
                if (undo_handle) {
                	CloseClipboard(undo_handle);
                }
                if (clip_handle) {
                	CloseClipboard(clip_handle);
                }

                /*
                **      Close the textfield.gadget.
                **/
                CloseLibrary( TextFieldBase );
        }
        else
        {
                struct bguiRequest   req;

                memset(&req,'\0',sizeof(req));
                req.br_GadgetFormat  = "_Ok";
                req.br_TextFormat    = ISEQ_C "This program requires Mark Thomas' gadgets/textfield.gadget\ninstalled in your LIBS: directory.\n\n You may find it at: " ISEQ_U ISEQ_FILL "aminet://dev/gui/textfield.lha" ISEQ_N ".";
                req.br_Flags         = BREQF_AUTO_ASPECT|BREQF_FAST_KEYS;
                req.br_ReqPos        = POS_CENTERMOUSE;
                req.br_Underscore    = '_';

                BGUI_RequestA( NULL, &req, NULL);
        }
}
