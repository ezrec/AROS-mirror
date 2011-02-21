/*
 * @(#) $Header$
 *
 * Images.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.4  2003/01/18 19:10:12  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.3  2000/08/08 19:29:54  chodorowski
 * Minor changes.
 *
 * Revision 42.2  2000/07/13 19:13:23  stegerg
 * changed image data array from UWORD[] to UBYTE[] so that it is
 * in the correct format on both big and little endian machines.
 *
 * Revision 42.1  2000/07/04 05:02:22  bergers
 * Made examples compilable.
 *
 * Revision 42.0  2000/05/09 22:19:51  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:41  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:58  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:23  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc Images.c -proto -mi -ms -mRR -lbgui
quit
*/

#include "DemoCode.h"


/*
**      Generated with IconEdit.
**/
#ifdef __AROS__
UBYTE HelpI1Data[] =
#else
CHIP UBYTE HelpI1Data[] =
#endif
{
/* Plane 0 */
    0x00,0x00,0x00,0x20,0x00,0x00,0x00,0xA0,0x00,0x20,0x00,0x00,0x01,0x43,0xE0,0x20,
    0x00,0x00,0x0A,0x8F,0xF8,0x20,0x00,0x00,0x05,0x1C,0x7C,0x3F,0xE0,0x00,0x00,0x38,
    0x3E,0x00,0x00,0x00,0x00,0x38,0x3E,0x00,0x08,0x00,0x00,0xFF,0xBF,0xFF,0x08,0x00,
    0x00,0x3C,0x3E,0x00,0x08,0x00,0x07,0xFF,0xFF,0x3F,0x08,0x00,0x00,0x00,0x7C,0x00,
    0x08,0x00,0x06,0xFF,0x7F,0xFD,0x08,0x00,0x00,0x00,0x70,0x00,0x08,0x00,0x07,0xF7,
    0xFA,0xE7,0x08,0x00,0x00,0x00,0xE0,0x00,0x08,0x00,0x07,0xDE,0xFB,0xFD,0x08,0x00,
    0x00,0x00,0xC0,0x00,0x08,0x00,0x06,0xFD,0xF7,0xEF,0x08,0x00,0x00,0x01,0xC0,0x00,
    0x08,0x00,0x00,0x03,0xE0,0x00,0x08,0x00,0x00,0x03,0xE1,0xB7,0x08,0x00,0x00,0x03,
    0xE0,0x00,0x08,0x00,0x00,0x01,0xC0,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
    0x00,0x00,0x00,0x00,0x08,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0x00,
/* Plane 1 */
    0xFF,0xFF,0xFF,0xD0,0x00,0x00,0xFE,0xAF,0xFF,0xDC,0x00,0x00,0xFD,0x5C,0x1F,0xDF,
    0x00,0x00,0xE0,0xB0,0x27,0xDF,0xC0,0x00,0xF0,0x6B,0x9B,0xC0,0x00,0x00,0xFF,0xD7,
    0xDD,0xFF,0xF0,0x00,0xFF,0xD7,0xDD,0xFF,0xF0,0x00,0xFF,0x18,0x5C,0x00,0xF0,0x00,
    0xFF,0xDB,0xDD,0xFF,0xF0,0x00,0xF8,0x00,0x18,0xC0,0xF0,0x00,0xFF,0xFF,0xBB,0xFF,
    0xF0,0x00,0xF9,0x00,0xB0,0x02,0xF0,0x00,0xFF,0xFF,0xAF,0xFF,0xF0,0x00,0xF8,0x08,
    0x25,0x18,0xF0,0x00,0xFF,0xFF,0x5F,0xFF,0xF0,0x00,0xF8,0x21,0x44,0x02,0xF0,0x00,
    0xFF,0xFF,0x3F,0xFF,0xF0,0x00,0xF9,0x02,0x88,0x10,0xF0,0x00,0xFF,0xFE,0xBF,0xFF,
    0xF0,0x00,0xFF,0xFD,0xDF,0xFF,0xF0,0x00,0xFF,0xFD,0xDE,0x48,0xF0,0x00,0xFF,0xFD,
    0xDF,0xFF,0xF0,0x00,0xFF,0xFE,0x3F,0xFF,0xF0,0x00,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,
    0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

struct Image HelpI1 =
{
    0, 0,                       /* Upper left corner */
    37, 26, 2,                  /* Width, Height, Depth */
    (WORD *) HelpI1Data,        /* Image data */
    0x0003, 0x0000,             /* PlanePick, PlaneOnOff */
    NULL                        /* Next image */
};

#ifdef __AROS__
UBYTE HelpI2Data[] =
#else
CHIP UBYTE HelpI2Data[] =
#endif
{
/* Plane 0 */
    0x00,0x00,0x00,0x20,0x00,0x00,0x00,0xA0,0x00,0x20,0x00,0x00,0x01,0x40,0xE0,0x20,
    0x00,0x00,0x0A,0x81,0xF0,0x20,0x00,0x00,0x05,0x01,0xF0,0x3F,0xE0,0x00,0x00,0x01,
    0xF0,0x00,0x00,0x00,0x00,0x01,0xF0,0x00,0x08,0x00,0x00,0xFF,0xF7,0xFF,0x08,0x00,
    0x00,0x00,0xE0,0x00,0x08,0x00,0x07,0xFB,0xFF,0x3F,0x08,0x00,0x00,0x00,0xE0,0x00,
    0x08,0x00,0x06,0xFF,0xFF,0xFD,0x08,0x00,0x00,0x00,0xE0,0x00,0x08,0x00,0x07,0xF7,
    0xFA,0xE7,0x08,0x00,0x00,0x00,0x40,0x00,0x08,0x00,0x07,0xDE,0xFB,0xFD,0x08,0x00,
    0x00,0x00,0x40,0x00,0x08,0x00,0x06,0xFD,0xF7,0xEF,0x08,0x00,0x00,0x00,0xE0,0x00,
    0x08,0x00,0x00,0x01,0xF0,0x00,0x08,0x00,0x00,0x01,0xF1,0xB7,0x08,0x00,0x00,0x01,
    0xF0,0x00,0x08,0x00,0x00,0x00,0xE0,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x08,0x00,
    0x00,0x00,0x00,0x00,0x08,0x00,0x7F,0xFF,0xFF,0xFF,0xF8,0x00,
/* Plane 1 */
    0xFF,0xFF,0xFF,0xD0,0x00,0x00,0xFE,0xAF,0xFF,0xDC,0x00,0x00,0xFD,0x5F,0x1F,0xDF,
    0x00,0x00,0xE0,0xBE,0xEF,0xDF,0xC0,0x00,0xF0,0x7E,0xEF,0xC0,0x00,0x00,0xFF,0xFE,
    0xEF,0xFF,0xF0,0x00,0xFF,0xFE,0xEF,0xFF,0xF0,0x00,0xFF,0x00,0x48,0x00,0xF0,0x00,
    0xFF,0xFF,0x5F,0xFF,0xF0,0x00,0xF8,0x04,0x40,0xC0,0xF0,0x00,0xFF,0xFF,0x5F,0xFF,
    0xF0,0x00,0xF9,0x00,0x40,0x02,0xF0,0x00,0xFF,0xFF,0x5F,0xFF,0xF0,0x00,0xF8,0x08,
    0x05,0x18,0xF0,0x00,0xFF,0xFF,0xBF,0xFF,0xF0,0x00,0xF8,0x21,0x04,0x02,0xF0,0x00,
    0xFF,0xFF,0xBF,0xFF,0xF0,0x00,0xF9,0x02,0x08,0x10,0xF0,0x00,0xFF,0xFF,0x5F,0xFF,
    0xF0,0x00,0xFF,0xFE,0xEF,0xFF,0xF0,0x00,0xFF,0xFE,0xEE,0x48,0xF0,0x00,0xFF,0xFE,
    0xEF,0xFF,0xF0,0x00,0xFF,0xFF,0x1F,0xFF,0xF0,0x00,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,
    0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

struct Image HelpI2 =
{
    0, 0,                       /* Upper left corner */
    37, 26, 2,                  /* Width, Height, Depth */
    (WORD *) HelpI2Data,        /* Image data */
    0x0003, 0x0000,             /* PlanePick, PlaneOnOff */
    NULL                        /* Next image */
};

/*
**      Put up a simple requester.
**/
STATIC ULONG ReqA( struct Window *win, UBYTE *gadgets, UBYTE *body, IPTR *args )
{
        struct bguiRequest      req = { };

        req.br_GadgetFormat     = gadgets;
        req.br_TextFormat       = body;
        req.br_Flags            = BREQF_CENTERWINDOW | BREQF_LOCKWINDOW;
        req.br_Underscore       = '_';

        return( BGUI_RequestA( win, &req, args));
}

#define Req(win, gadgets, body, ...) \
	({ IPTR __args[] = { AROS_PP_VARIADIC_CAST2IPTR(__VA_ARGS__) }; \
	    ReqA(win, gadgets, body, __args); })


/*
**      Object ID's.
**/
#define ID_QUIT                 1
#define ID_HELP                 2

VOID StartDemo( void )
{
        struct Window           *window;
        Object                  *WO_Window, *GO_Quit, *GO_Help;
        IPTR                     signal = 0;
        ULONG                    rc, tmp = 0;
        BOOL                     running = TRUE;

        /*
        **      Create the window object.
        **/
        WO_Window = WindowObject,
                WINDOW_Title,           "Image Demo",
                WINDOW_AutoAspect,      TRUE,
                WINDOW_MasterGroup,
                        VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                StartMember,
                                        GO_Help = ButtonObject,
                                                BUTTON_Image,           &HelpI1,
                                                BUTTON_SelectedImage,   &HelpI2,
                                                LAB_Label,              "_Help",
                                                LAB_Underscore,         '_',
                                                LAB_Place,              PLACE_LEFT,
                                                FRM_Type,               FRTYPE_BUTTON,
                                                FRM_EdgesOnly,          TRUE,
                                                GA_ID,                  ID_HELP,
                                        EndObject,
                                EndMember,
                                StartMember,
                                        HGroupObject,
                                                VarSpace( 50 ),
                                                StartMember, GO_Quit  = KeyButton( "_Quit",  ID_QUIT  ), EndMember,
                                                VarSpace( 50 ),
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
                tmp  = GadgetKey( WO_Window, GO_Quit,  "q" );
                tmp += GadgetKey( WO_Window, GO_Help,  "h" );
                /*
                **      OK?
                **/
                if ( tmp == 2 ) {
                        /*
                        **      try to open the window.
                        **/
                        if (( window = WindowOpen( WO_Window ))) {
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

                                                        case    ID_HELP:
                                                                Req( window, "_Continue", ISEQ_C "This small demo shows you how to use\n"
                                                                                          "standard intuition images in button objects." );
                                                                break;

                                                }
                                        }
                                } while ( running );
                        } else
                                Tell( "Could not open the window\n" );
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
