/*
 * @(#) $Header$
 *
 * MultiSelect.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:20:02  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:52  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:08  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.3  1998/12/07 00:14:09  mlemos
 * Made the listview be activated by the key indicated in the added label.
 *
 * Revision 1.1.2.2  1998/03/01 19:45:41  mlemos
 * Fixed the display of empty lines.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:38  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc MultiSelect.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"

/*
**      The entries shown in the list.
**/
UBYTE   *ListEntries[] = {
        ISEQ_C "This listview object has multi-",
        ISEQ_C "selection turned on. You can",
        ISEQ_C "multi-select the items by holding",
        ISEQ_C "down the SHIFT-key while clicking",
        ISEQ_C "on the different items or by clicking",
        ISEQ_C "on an entry and dragging the mouse",
        ISEQ_C "up or down.",
        "",
        ISEQ_C "If you check the \"No SHIFT\" checbox",
        ISEQ_C "you can multi-select the items without",
        ISEQ_C "using the SHIFT key",
        NULL
};

/*
**      Map-list.
**/
struct TagItem CheckToList[] = { GA_Selected, LISTV_MultiSelectNoShift, TAG_END };

/*
**      Object ID's.
**/
#define ID_SHOW                 1
#define ID_QUIT                 2
#define ID_ALL                  3
#define ID_NONE                 4

VOID StartDemo( void )
{
        struct Window           *window;
        Object                  *WO_Window, *GO_Quit, *GO_Show, *GO_List, *GO_Shift, *GO_All, *GO_None;
        ULONG                    signal, rc, tmp = 0;
        BOOL                     running = TRUE;

        /*
         *      Create the window object.
         */
        WO_Window = WindowObject,
                WINDOW_Title,           "Multi-Selection Demo",
                WINDOW_AutoAspect,      TRUE,
                WINDOW_SmartRefresh,    TRUE,
                WINDOW_RMBTrap,         TRUE,
                WINDOW_ScaleWidth,      30,
                WINDOW_ScaleHeight,     30,
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ), GROUP_BackFill, SHINE_RASTER,
                                StartMember,
                                        VGroupObject, HOffset( 4 ), VOffset( 4 ),
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_Recessed,           TRUE,
                                                StartMember,
                                                        GO_List = ListviewObject,
                                                                LISTV_EntryArray,               ListEntries,
                                                                LISTV_MultiSelect,              TRUE,
                                                                Label("Se_lect next entry"),
                                                                Place(PLACE_ABOVE),
                                                        EndObject,
                                                EndMember,
                                                StartMember,
                                                        HGroupObject,
                                                                StartMember, GO_All  = KeyButton( "_All",  ID_ALL  ), EndMember,
                                                                StartMember, GO_None = KeyButton( "N_one", ID_NONE ), EndMember,
                                                        EndObject, FixMinHeight,
                                                EndMember,
                                        EndObject,
                                EndMember,
                                StartMember,
                                        HGroupObject, HOffset( 4 ), VOffset( 4 ),
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_Recessed,           TRUE,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Shift = KeyCheckBox( "_No SHIFT:", FALSE, 0 ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                        EndObject, FixMinHeight,
                                EndMember,
                                StartMember,
                                        HGroupObject, Spacing( 4 ),
                                                StartMember, GO_Show = KeyButton( "_Show", ID_SHOW ), EndMember,
                                                VarSpace( DEFAULT_WEIGHT ),
                                                StartMember, GO_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
                                        EndObject, FixMinHeight,
                                EndMember,
                        EndObject,
        EndObject;

        /*
        **      Object created OK?
        **/
        if ( WO_Window ) {
                /*
                **      Assign the keys to the buttons.
                **/
                tmp += GadgetKey( WO_Window, GO_Quit,  "q" );
                tmp += GadgetKey( WO_Window, GO_Show,  "s" );
                tmp += GadgetKey( WO_Window, GO_Shift, "n" );
                tmp += GadgetKey( WO_Window, GO_All,   "a" );
                tmp += GadgetKey( WO_Window, GO_None,  "o" );
                tmp += GadgetKey( WO_Window, GO_List,  "l" );
                /*
                **      OK?
                **/
                if ( tmp == 6 ) {
                        /*
                        **      Add notification.
                        **/
                        if ( AddMap( GO_Shift, GO_List, CheckToList )) {
                                /*
                                **      try to open the window.
                                **/
                                if ( window = WindowOpen( WO_Window )) {
                                        /*
                                        **      Obtain it's wait mask.
                                        **/
                                        GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                        /*
                                        **      Event loop...
                                        **/
                                        do {
                                                Wait( signal );
                                                /*
                                                **      Handle events.
                                                **/
                                                while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                        /*
                                                        **      Evaluate return code.
                                                        **/
                                                        switch ( rc ) {

                                                                case    WMHI_CLOSEWINDOW:
                                                                case    ID_QUIT:
                                                                        running = FALSE;
                                                                        break;

                                                                case    ID_ALL:
                                                                        SetGadgetAttrs(( struct Gadget * )GO_List, window, NULL, LISTV_SelectMulti, LISTV_Select_All, TAG_END );
                                                                        break;

                                                                case    ID_NONE:
                                                                        SetGadgetAttrs(( struct Gadget * )GO_List, window, NULL, LISTV_DeSelect, ~0, TAG_END );
                                                                        break;

                                                                case    ID_SHOW:
                                                                {
                                                                        UBYTE           *str;

                                                                        /*
                                                                        **      Simply dump all selected entries
                                                                        **      to the console.
                                                                        **/
                                                                        if ( str = ( UBYTE * )FirstSelected( GO_List )) {
                                                                                do {
                                                                                        STRPTR text=str;

                                                                                        if(*text)
                                                                                          text++;
                                                                                        if(*text)
                                                                                          text++;
                                                                                        Tell( "%s\n", text);
                                                                                        str = ( UBYTE * )NextSelected( GO_List, str );
                                                                                } while ( str );
                                                                        } else
                                                                                /*
                                                                                **      Oops. There are no selected
                                                                                **      entries.
                                                                                **/
                                                                                Tell( "No selections made!\n" );
                                                                        break;
                                                                }
                                                        }
                                                }
                                        } while ( running );
                                } else
                                        Tell( "Could not open the window\n" );
                        } else
                                Tell( "Unable to add notification\n" );
                } else
                        Tell( "Could not assign gadget keys\n" );
                /*
                **      Disposing of the window object will
                **      also close the window if it is
                **      already opened and it will dispose of
                **      all objects attached to it.
                **/
                DisposeObject( WO_Window );
        } else
                Tell( "Could not create the window object\n" );
}
