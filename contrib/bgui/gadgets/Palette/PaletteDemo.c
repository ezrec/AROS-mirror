/*
 * @(#) $Header$
 *
 * PaletteDemo.c
 *
 * (C) Copyright 1999 BGUI Developement team.
 * (C) Copyright 1995-1996 Jaba Development.
 * (C) Copyright 1995-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/20 12:24:32  verhaegs
 * Use REGFUNC macro's in BGUI source code, not AROS_UFH
 *
 * Revision 42.5  2003/01/18 19:10:19  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.4  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.3  2000/08/08 13:51:35  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:07  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:17  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:04  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:05  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/05/31 01:42:28  mlemos
 * Ian sources.
 *
 *
 *
 */

#include <exec/types.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/bgui.h>

#ifndef BGUI_COMPILERSPECIFIC_H
#include <bgui/bgui_compilerspecific.h>
#endif

/*
 *      Object ID's.
 */
#define ID_QUIT                 1
#define ID_FRAME                2
#define ID_SFRAME               3
#define ID_LABEL                4
#define ID_SLABEL               5

/*
 *      Map-lists.
 */
ULONG p2f[]  = { PALETTE_CurrentColor, FRM_BackPen,         TAG_END };
ULONG p2fs[] = { PALETTE_CurrentColor, FRM_SelectedBackPen, TAG_END };
ULONG p2l[]  = { PALETTE_CurrentColor, LAB_Pen,             TAG_END };
ULONG p2ls[] = { PALETTE_CurrentColor, LAB_SelectedPen,     TAG_END };

/*
 *      Library base and class base.
 */
struct Library *BGUIBase;

struct IntuitionBase * IntuitionBase;
Class          *myButtonClass;

/*
 *      Info gadget text.
 */
UBYTE          *InfoTxt = ISEQ_C "As you can see the colors of the below button\n"
                          "are normal but when you change the colors with\n"
                          "the palette objects the colors of the button change.\n\n"
                          "You can also pickup the color and drop it onto the\n"
                          "button. " ISEQ_B "DragNDrop" ISEQ_N " in action.\n\n"
                          ISEQ_B "Run the program again an notice that you can\n" ISEQ_N 
                          ISEQ_B "drag and drop colors of one program\n" ISEQ_N
                          ISEQ_B "over the button of other program." ISEQ_N ;

/*
 *      Some casting macros.
 */
#define GAD(x) (( struct Gadget * )x )
#define BDQ(x) (( struct bmDragPoint * )x )
#define BDM(x) (( struct bmDragMsg * )x )

/*
 *      The button we use is a very simple subclass from the
 *      BGUI buttonclass to accept only drops from the four
 *      paletteclass objects in this demo or from other palette
 *      class objects from another task or window when they have
 *      the same ID as we use here.
 *
 *      SAS users remember! NOSTACKCHECK or __interrupt for class
 *      dispatchers, hook routines or anything else which may get
 *      called by a task other than your own.
 */
//SAVEDS ASM ULONG myButtonDispatch( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) Msg msg )
ASM REGFUNC3(ULONG, myButtonDispatch,
	     REGPARAM(A0, Class *, cl),
	     REGPARAM(A2, Object *, obj),
	     REGPARAM(A1, Msg, msg)
)
{
        ULONG                   rc, pen, tag;

        switch ( msg->MethodID ) {

                case    BASE_DRAGQUERY:
                        /*
                         *      We only accept drops from our paletteclass objects.
                         *      The test here is a bit simple but this way it does
                         *      allow for drops from another task. Just run this demo
                         *      twice and DragNDrop from one window to another.
                         */
                        if (( GAD( BDQ( msg )->bmdp_Source )->GadgetID >= ID_FRAME ) &&
                            ( GAD( BDQ( msg )->bmdp_Source )->GadgetID <= ID_SLABEL ))
                                rc = BQR_ACCEPT;
                        else
                                rc = BQR_REJECT;
                        break;

                case    BASE_DROPPED:
                        /*
                         *      Get the pen from the object.
                         */
                        GetAttr( PALETTE_CurrentColor, BDM( msg )->bmdm_Source, &pen );

                        /*
                         *      Let's see what has been dropped...
                         */
                        switch ( GAD( BDM( msg )->bmdm_Source )->GadgetID ) {

                                case    ID_FRAME:
                                        tag = FRM_BackPen;
                                        break;

                                case    ID_SFRAME:
                                        tag = FRM_SelectedBackPen;
                                        break;

                                case    ID_LABEL:
                                        tag = LAB_Pen;
                                        break;

                                case    ID_SLABEL:
                                        tag = LAB_SelectedPen;
                                        break;

                                default:
                                        tag = NULL;
                                        break;
                        }

                        /*
                         *      Set the pen. The superclass will force
                         *      a refresh on the object when the drop has
                         *      been made.
                         */
                        if(tag)
                                SetAttrs( obj, tag, pen, TAG_END );
                        rc = 1L;
                        break;

                default:
                        rc = DoSuperMethodA( cl, obj, msg );
                        break;
        }
        return( rc );
}
REGFUNC_END

/*
 *      Setup our button class.
 */
Class *MakeMyButtonClass( void )
{
        Class                   *cl = NULL, *super;

        /*
         *      Get a pointer to our superclass.
         */
        if ( super = BGUI_GetClassPtr( BGUI_BUTTON_GADGET )) {
                /*
                 *      Make our class.
                 */
                if ( cl = MakeClass( NULL, NULL, super, 0L, 0L ))
                        /*
                         *      Setup our dispatcher.
                         */
                        cl->cl_Dispatcher.h_Entry = ( HOOKFUNC )myButtonDispatch;
        }
        return( cl );
}

/*
 *      Here we go...
 */
int main( int argc, char **argv )
{
        struct Window           *window;
        Object                  *WO_Window, *GO_Quit, *GO_B, *GO_Pal[ 4 ]={0,0,0,0};
        ULONG                    signal, rc, tmp = 0, a;
        UWORD                    defpens[ 4 ] = { 0, 3, 1, 1 };
        BOOL                     running = TRUE;

        IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

        /*
         *      Open BGUI.
         */
        if ( (NULL != IntuitionBase) && 
             NULL != (BGUIBase = OpenLibrary( BGUINAME, BGUIVERSION ))) {
                        /*
                         *      And our drop-button class.
                         */
                        if ( myButtonClass = MakeMyButtonClass()) {
                                /*
                                 *      I assume a depth of three
                                 *      (8 colors) here for simplicity.
                                 */
                                for ( a = 0; a < 4; a++ )
                                        GO_Pal[ a ] = BGUI_NewObject(BGUI_PALETTE_GADGET,
                                                                                     FRM_Type,                          FRTYPE_BUTTON,
                                                                                     FRM_Recessed,                      TRUE,
                                                                                     PALETTE_Depth,                     3,
                                                                                     PALETTE_CurrentColor,              defpens[ a ],
                                                                                     GA_ID,                             a + 2,
                                                                                     BT_DragObject,                     TRUE,
                                                                                     TAG_END );
                                /*
                                 *      Create the window object.
                                 */
                                WO_Window = WindowObject,
                                        WINDOW_Title,           "PaletteClass Demo",
                                        WINDOW_AutoAspect,      TRUE,
                                        WINDOW_SmartRefresh,    TRUE,
                                        WINDOW_RMBTrap,         TRUE,
                                        WINDOW_IDCMP,           IDCMP_MOUSEMOVE,
                                        WINDOW_MasterGroup,
                                                VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                                        GROUP_BackFill,         SHINE_RASTER,
                                                        StartMember,
                                                                InfoFixed( NULL, InfoTxt, NULL, 10 ),
                                                        EndMember,
                                                        StartMember,
                                                                HGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                                                        FRM_Type,               FRTYPE_BUTTON,
                                                                        FRM_Recessed,           TRUE,
                                                                        StartMember, GO_B = NewObject( myButtonClass, NULL, FRM_Type, FRTYPE_BUTTON, LAB_Label, "Palette Demo", BT_DropObject, TRUE, TAG_END ), EndMember,
                                                                EndObject, FixMinHeight,
                                                        EndMember,
                                                        StartMember,
                                                                HGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                                                        FRM_Type,               FRTYPE_BUTTON,
                                                                        FRM_Recessed,           TRUE,
                                                                        StartMember,
                                                                                VGroupObject, Spacing( 4 ),
                                                                                        LAB_Label,      "Background:",
                                                                                        LAB_Place,      PLACE_ABOVE,
                                                                                        StartMember, GO_Pal[ 0 ], EndMember,
                                                                                        StartMember, GO_Pal[ 1 ], EndMember,
                                                                                EndObject,
                                                                        EndMember,
                                                                        StartMember,
                                                                                VGroupObject, Spacing( 4 ),
                                                                                        LAB_Label,      "Label:",
                                                                                        LAB_Place,      PLACE_ABOVE,
                                                                                        StartMember, GO_Pal[ 2 ], EndMember,
                                                                                        StartMember, GO_Pal[ 3 ], EndMember,
                                                                                EndObject,
                                                                        EndMember,
                                                                EndObject,
                                                        EndMember,
                                                        StartMember,
                                                                HGroupObject,
                                                                        VarSpace( DEFAULT_WEIGHT ),
                                                                        StartMember, GO_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
                                                                        VarSpace( DEFAULT_WEIGHT ),
                                                                EndObject, FixMinHeight,
                                                        EndMember,
                                                EndObject,
                                EndObject;

                                /*
                                 *      Object created OK?
                                 */
                                if ( WO_Window ) {
                                        tmp += GadgetKey( WO_Window, GO_Quit,  "q" );
                                        tmp += AddMap( GO_Pal[ 0 ], GO_B, p2f  );
                                        tmp += AddMap( GO_Pal[ 1 ], GO_B, p2fs );
                                        tmp += AddMap( GO_Pal[ 2 ], GO_B, p2l  );
                                        tmp += AddMap( GO_Pal[ 3 ], GO_B, p2ls );
                                        if ( tmp == 5 ) {
                                                if ( window = WindowOpen( WO_Window )) {
                                                        GetAttr( WINDOW_SigMask, WO_Window, &signal );
                                                        do {
                                                                Wait( signal );
                                                                while (( rc = HandleEvent( WO_Window )) != WMHI_NOMORE ) {
                                                                        switch ( rc ) {

                                                                                case    WMHI_CLOSEWINDOW:
                                                                                case    ID_QUIT:
                                                                                        running = FALSE;
                                                                                        break;
                                                                        }
                                                                }
                                                        } while ( running );
                                                }
                                        }
                                        DisposeObject( WO_Window );
                                }
                                FreeClass( myButtonClass );
                        }
                CloseLibrary( BGUIBase );
        }
        if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
}
#ifdef _DCC
int wbmain( struct WBStartup *wbs )
{
        return( main( 0, wbs ));
}
#endif
