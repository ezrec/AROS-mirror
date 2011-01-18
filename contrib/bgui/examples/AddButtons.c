/*
 * @(#) $Header$
 *
 * AddButtons.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1994 Paul Weterings.
 * All Rights Reserved.
 *
 * Modified by Ian J. Einman, 4/26/96
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:19:17  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:07  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:29  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.3  1999/08/01 03:56:35  mlemos
 * Added missing disposal of removed buttons.
 *
 * Revision 1.1.2.2  1999/07/28 16:40:19  mlemos
 * Fixed the code that attempted to remove a button that failed to be inserted
 * by simply disposing the object.
 *
 * Revision 1.1.2.1  1998/02/28 17:44:45  mlemos
 * Ian sources
 *
 *
 */

/*
dcc AddButtons.c -mi -ms -mRR -proto -lbgui
quit
*/

#include "DemoCode.h"

/*
 *      Object ID's. Please note that the ID's are shared
 *      between the menus and the gadget objects.
 */
#define ID_ADD          21
#define ID_QUIT         22
#define ID_INS          23
#define ID_REM          24

/*
 *      Simple menu strip.
 */
struct NewMenu SimpleMenu[] = {
   Title( "Project" ),
      Item( "Add",        "A", ID_ADD ),
      Item( "Insert",     "I", ID_INS ),
      Item( "Remove all", "R", ID_REM ),
      ItemBar,
      Item( "Quit",       "Q", ID_QUIT ),
   End
};

/*
 *      Simple button creation macros.
 */
#define AddButton\
   ButtonObject,\
      LAB_Label,              "Added",\
      LAB_Style,              FSF_BOLD,\
      FuzzButtonFrame,\
   EndObject

#define InsButton\
   ButtonObject,\
      LAB_Label,              "Inserted",\
      LAB_Style,              FSF_BOLD,\
      FuzzButtonFrame,\
   EndObject


VOID StartDemo( void )
{
   struct Window           *window;
   Object                  *WO_Window, *GO_Add, *GO_Quit, *GO_Ins, *GO_Rem, *addobj[20], *base;
   ULONG                    signal = 0, rc;
   BOOL                     running = TRUE, ok = FALSE;
   int                      x = 0, xx;

   /*
    *      Create window object.
    */
   WO_Window = WindowObject,
      WINDOW_Title,           "Add/Insert Demo",
      WINDOW_MenuStrip,       SimpleMenu,
      WINDOW_LockHeight,      TRUE,
      WINDOW_AutoAspect,      TRUE,
      WINDOW_AutoKeyLabel,    TRUE,
      WINDOW_MasterGroup,
         base = HGroupObject,
            StartMember, GO_Add  = FuzzButton( "_Add",        ID_ADD  ), EndMember,
            StartMember, GO_Ins  = FuzzButton( "_Insert",     ID_INS  ), EndMember,
            StartMember, GO_Rem  = FuzzButton( "_Remove all", ID_REM  ), EndMember,
            StartMember, GO_Quit = FuzzButton( "_Quit",       ID_QUIT ), EndMember,
         EndObject,
      EndObject;

        /*
         *      OK?
         */
   if ( WO_Window )
   {
      /*
       *      Open window.
       */
      if ( window = WindowOpen( WO_Window ))
      {
         /*
          *      Get signal mask.
          */
         GetAttr( WINDOW_SigMask, WO_Window, &signal );
         do {
            /*
             *      Poll messages.
             */
            Wait( signal );
            while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE )
            {
               switch ( rc )
               {
               case WMHI_CLOSEWINDOW:
               case ID_QUIT:
                  /*
                   *      Bye now.
                   */
                  running = FALSE;
                  break;

               case ID_ADD:
                  if ( x == 19 )
                  {
                     Tell( "Max Nr. of gadgets\n" );
                     break;
                  }
                  x++;

                  addobj[x]  = AddButton;

                  ok = DoMethod( base, GRM_ADDMEMBER, addobj[ x ],
                     LGO_FixMinHeight, FALSE,
                     LGO_Weight,       DEFAULT_WEIGHT,
                     TAG_END );

                  if (!ok)
                  {
                     DisposeObject(addobj[ x ]);
                     x--;
                     Tell( "Last object did not fit!\n" );
                  }

                  if ( ! window )
                     goto error;
                  break;

               case ID_REM:
                  if ( x > 0 )
                  {
                     for ( xx = 1; xx <= x; xx++ )
                     {
                        DoMethod( base, GRM_REMMEMBER, addobj[ xx ] );
                        DisposeObject(addobj[ xx ]);
                     }
                     x = 0;
                  }
                  else
                     Tell("Were out of gadgets!\n");
                  break;

               case ID_INS:
                  if ( x == 19 )
                  {
                     Tell( "Max Nr. of gadgets\n" );
                     break;
                  }
                  x++;

                  addobj[x]  = InsButton;

                  ok = DoMethod( base, GRM_INSERTMEMBER, addobj[ x ], GO_Rem,
                     LGO_FixMinHeight, FALSE,
                     LGO_Weight,       DEFAULT_WEIGHT,
                     TAG_END );

                  if (!ok)
                  {
                     DisposeObject(addobj[ x ]);
                     x--;
                     Tell( "Last object did not fit!\n" );
                  }

                  if ( ! window )
                     goto error;
                  break;

               }
            }
         } while ( running );
      }
      else
         Tell( "Could not open the window\n" );
      error:
      DisposeObject( WO_Window );
   }
   else
      Tell( "Could not create the window object\n" );
}
