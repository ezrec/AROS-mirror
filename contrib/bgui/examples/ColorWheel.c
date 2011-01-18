/*
 * @(#) $Header$
 *
 * ColorWheel.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1995 Jaba Development.
 * (C) Copyright 1995 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2000/08/08 20:57:28  chodorowski
 * Minor fixes to build on Amiga.
 *
 * Revision 42.0  2000/05/09 22:19:32  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:33:22  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:58:42  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1998/02/28 17:45:05  mlemos
 * Ian sources
 *
 *
 */

/* Execute me to compile with DICE V3.0
dcc ColorWheel.c -proto -mi -ms -mRR -3.0 -lbgui
quit
*/

/*
 *      This example is based on the original ColorWheel.c
 *      code by Commodore.
 */

#include "DemoCode.h"

#include <dos/dos.h>
#include <gadgets/colorwheel.h>
#include <gadgets/gradientslider.h>
#include <graphics/displayinfo.h>

#include <clib/colorwheel_protos.h>

#include <stdio.h>
#include <stdlib.h>

/*
**      Library base pointers.
**/
struct Library *ColorWheelBase = NULL, *GradientSliderBase = NULL;

/*
**      Object ID's
**/
#define ID_QUIT                 1
#define ID_WHEEL                2

/*
**      Structure for LoadRGB32().
**/
struct load32 {
        UWORD           l32_len;
        UWORD           l32_pen;
        ULONG           l32_red;
        ULONG           l32_grn;
        ULONG           l32_blu;
};

/*
**      Ensure correct operation on ECS.
**/
#define GRADCOLORS 4

/*
**      The program even does version checking... sjeez...
**/
extern struct ExecBase *SysBase;

/*
**      And were in...
**/
VOID StartDemo( void )
{
        Object                  *WN_Window, *GA_Quit, *GA_Wheel, *GA_GrSlider, *GA_Master, *RealSlider = NULL;
        struct Window           *win;
        struct Screen           *scr;
        ULONG                   winsig = 0L, sigrec, rc;
        BOOL                    running = TRUE;
        ULONG                   colortable[ 96 ];
        struct load32           color_list[ GRADCOLORS + 1 ];
        WORD                    penns[ GRADCOLORS + 1 ];
        struct ColorWheelRGB    rgb;
        struct ColorWheelHSB    hsb;
        ULONG                   modeID = HIRES_KEY;
        UWORD                   numPens, i;
        DisplayInfoHandle       displayhandle;
        struct DimensionInfo    dimensioninfo;

        /*
        **      Need OS 3.0.
        **/
        if ( SysBase->LibNode.lib_Version < 39 ) {
                Tell( "OS 3.0 required!" );
                exit( 0 );
        }

        /*
        **      Open the libraries. Get display information etc...
        **/
        if ( ColorWheelBase = OpenLibrary( "gadgets/colorwheel.gadget", 39L )) {
                if ( GradientSliderBase = OpenLibrary( "gadgets/gradientslider.gadget", 39L )) {
                        if ( displayhandle = FindDisplayInfo( modeID )) {
                                if ( GetDisplayInfoData( displayhandle, ( UBYTE * )&dimensioninfo, sizeof( struct DimensionInfo ), DTAG_DIMS, NULL )) {
                                        /*
                                        **      Open up a screen with as many
                                        **      colors as possible. Also make it
                                        **      like your workbench screen.
                                        **/
                                        if ( scr = OpenScreenTags( NULL, SA_Depth,              dimensioninfo.MaxDepth,
                                                                         SA_SharePens,          TRUE,
                                                                         SA_LikeWorkbench,      TRUE,
                                                                         SA_Interleaved,        TRUE,
                                                                         SA_Title,              "ColorWheel Screen",
                                                                         TAG_END )) {
                                                /*
                                                **      Get the colors.
                                                **/
                                                GetRGB32( scr->ViewPort.ColorMap, 0L, 32L, colortable );

                                                /*
                                                **      Setup gradient slider
                                                **      as color 0.
                                                **/
                                                rgb.cw_Red   = colortable[ 0 ];
                                                rgb.cw_Green = colortable[ 1 ];
                                                rgb.cw_Blue  = colortable[ 2 ];

                                                /*
                                                **      Convert the RGB values to
                                                **      HSB values.
                                                **/
                                                ConvertRGBToHSB( &rgb, &hsb );

                                                /*
                                                **      Maximum brightness.
                                                **/
                                                hsb.cw_Brightness = 0xffffffff;

                                                numPens = 0;

                                                /*
                                                **      Build the pen array for the
                                                **      gradient slider (4 pens).
                                                **/
                                                while ( numPens < GRADCOLORS ) {
                                                        /*
                                                        **      Compute dim level of this pen.
                                                        **/
                                                        hsb.cw_Brightness = ( ULONG )0xffffffff - ((( ULONG )0xffffffff / GRADCOLORS ) * numPens );
                                                        /*
                                                        **      Convert this dim level
                                                        **      to RGB values.
                                                        **/
                                                        ConvertHSBToRGB( &hsb, &rgb );
                                                        /*
                                                        **      Allocate a pen.
                                                        **/
                                                        if (( penns[ numPens ] = ObtainPen( scr->ViewPort.ColorMap, -1, rgb.cw_Red, rgb.cw_Green, rgb.cw_Blue, PEN_EXCLUSIVE )) == -1 )
                                                                break;
                                                        /*
                                                        **      Stuff it in the color list.
                                                        **/
                                                        color_list[ numPens ].l32_len = 1;
                                                        color_list[ numPens ].l32_pen = penns[ numPens ];
                                                        /*
                                                        **      Next...
                                                        **/
                                                        numPens++;
                                                }

                                                /*
                                                **      Terminate pen array and
                                                **      color list.
                                                **/
                                                penns[ numPens ]              = ~0;
                                                color_list[ numPens ].l32_len = 0;

                                                /*
                                                **      Create the gradient slider here
                                                **      _with_ the EXT_NoRebuild tag set
                                                **      to TRUE. This _must_ be set to
                                                **      TRUE otherwise the external class
                                                **      will rebuild the object after each
                                                **      re-size resulting in colourful
                                                **      crashes when it gets attached to
                                                **      the colorwheel :)
                                                **/
                                                GA_GrSlider = ExternalObject,
                                                        EXT_MinWidth,           10,
                                                        EXT_MinHeight,          10,
                                                        EXT_ClassID,            "gradientslider.gadget",
                                                        EXT_NoRebuild,          TRUE,
                                                        GRAD_PenArray,          penns,
                                                        PGA_Freedom,            LORIENT_VERT,
                                                        TAG_END );

                                                /*
                                                **      Pick up a pointer to the
                                                **      "real" gradient slider object.
                                                **      This will be the pointer we
                                                **      pass to the colorwheel.
                                                **/
                                                if ( GA_GrSlider )
                                                        GetAttr( EXT_Object, GA_GrSlider, ( ULONG * )&RealSlider );

                                                /*
                                                **      Create a small window.
                                                **/
                                                WN_Window = WindowObject,
                                                        WINDOW_Title,           "ColorWheel",
                                                        WINDOW_RMBTrap,         TRUE,
                                                        WINDOW_Screen,          scr,
                                                        WINDOW_ScaleWidth,      20,
                                                        WINDOW_ScaleHeight,     20,
                                                        WINDOW_AutoAspect,      TRUE,
                                                        /*
                                                        **      The colorwheel is really slow :(
                                                        **/
                                                        WINDOW_NoBufferRP,      TRUE,
                                                        WINDOW_MasterGroup,
                                                                GA_Master = VGroupObject, HOffset( 4 ), VOffset( 4 ), Spacing( 4 ),
                                                                        StartMember, TitleSeperator( "Wheel & Slider" ), EndMember,
                                                                        StartMember,
                                                                                HGroupObject, Spacing( 4 ),
                                                                                        StartMember,
                                                                                                /*
                                                                                                **      The EXT_NoRebuild tag may _not_ be set to
                                                                                                **      TRUE for a colorwheel object. This is due
                                                                                                **      to the fact that colorwheels cannot change
                                                                                                **      size. This is also the reason why we need
                                                                                                **      to track the attributes of the colorwheel.
                                                                                                **/
                                                                                                GA_Wheel = ExternalObject,
                                                                                                        EXT_MinWidth,           30,
                                                                                                        EXT_MinHeight,          30,
                                                                                                        EXT_ClassID,            "colorwheel.gadget",
                                                                                                        WHEEL_Screen,           scr,
                                                                                                        /*
                                                                                                        **      Pass a pointer to the "real" gradient slider
                                                                                                        **      here.
                                                                                                        **/
                                                                                                        WHEEL_GradientSlider,   RealSlider,
                                                                                                        WHEEL_Red,              colortable[ 0 ],
                                                                                                        WHEEL_Green,            colortable[ 1 ],
                                                                                                        WHEEL_Blue,             colortable[ 2 ],
                                                                                                        GA_FollowMouse,         TRUE,
                                                                                                        GA_ID,                  ID_WHEEL,
                                                                                                        /*
                                                                                                        **      These attributes of the colorwheel are
                                                                                                        **      tracked and reset to the object after
                                                                                                        **      it has been rebuild. This way the current
                                                                                                        **      colorwheel internals will not be lost
                                                                                                        **      after the object is re-build.
                                                                                                        **/
                                                                                                        EXT_TrackAttr,          WHEEL_Red,
                                                                                                        EXT_TrackAttr,          WHEEL_Green,
                                                                                                        EXT_TrackAttr,          WHEEL_Blue,
                                                                                                        EXT_TrackAttr,          WHEEL_Hue,
                                                                                                        EXT_TrackAttr,          WHEEL_Saturation,
                                                                                                        EXT_TrackAttr,          WHEEL_Brightness,
                                                                                                EndObject,
                                                                                        EndMember,
                                                                                        /*
                                                                                        **      Add the externalclass object of the
                                                                                        **      gradient slider here. Right next to
                                                                                        **      the colorwheel :)
                                                                                        **/
                                                                                        StartMember,
                                                                                                GA_GrSlider, FixWidth( 20 ),
                                                                                        EndMember,
                                                                                EndObject,
                                                                        EndMember,
                                                                        StartMember, HorizSeperator, EndMember,
                                                                        StartMember,
                                                                                HGroupObject,
                                                                                        VarSpace( DEFAULT_WEIGHT ),
                                                                                        StartMember, GA_Quit = KeyButton( "_Quit", ID_QUIT ), EndMember,
                                                                                        VarSpace( DEFAULT_WEIGHT ),
                                                                                EndObject, FixMinHeight,
                                                                        EndMember,
                                                                EndObject,
                                                EndObject;

                                                if ( WN_Window ) {
                                                        /*
                                                        **      Make button selectable by the keyboard.
                                                        **/
                                                        GadgetKey( WN_Window, GA_Quit, "q" );
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
                                                                do {
                                                                        sigrec = Wait( winsig );

                                                                        /*
                                                                        **      Window signal?
                                                                        **/
                                                                        if ( sigrec & winsig ) {
                                                                                while ( WN_Window && (( rc = HandleEvent( WN_Window )) != WMHI_NOMORE )) {
                                                                                        switch ( rc ) {

                                                                                                case    WMHI_CLOSEWINDOW:
                                                                                                case    ID_QUIT:
                                                                                                        /*
                                                                                                        **      The end.
                                                                                                        **/
                                                                                                        running = FALSE;
                                                                                                        break;

                                                                                                case    ID_WHEEL:
                                                                                                        /*
                                                                                                        **      Obtain the wheel it's HSB settings.
                                                                                                        **/
                                                                                                        GetAttr( WHEEL_HSB, GA_Wheel, ( ULONG * )&hsb );

                                                                                                        i = 0;

                                                                                                        /*
                                                                                                        **      Recompute the gradient slider
                                                                                                        **      dim colors for the current
                                                                                                        **      colorwheel setting.
                                                                                                        **/
                                                                                                        while ( i < numPens ) {
                                                                                                                /*
                                                                                                                **      Compute dim level of this pen.
                                                                                                                **/
                                                                                                                hsb.cw_Brightness = ( ULONG )0xffffffff - ((( ULONG )0xffffffff / numPens ) * i );
                                                                                                                /*
                                                                                                                **      Convert to RGB values.
                                                                                                                **/
                                                                                                                ConvertHSBToRGB(&hsb,&rgb);
                                                                                                                /*
                                                                                                                **      Setup the pens in the color list.
                                                                                                                **/
                                                                                                                color_list[ i ].l32_red = rgb.cw_Red;
                                                                                                                color_list[ i ].l32_grn = rgb.cw_Green;
                                                                                                                color_list[ i ].l32_blu = rgb.cw_Blue;
                                                                                                                /*
                                                                                                                **      Next...
                                                                                                                **/
                                                                                                                i++;
                                                                                                        }
                                                                                                        /*
                                                                                                        **      Show the changes in the gradient slider.
                                                                                                        **/
                                                                                                        LoadRGB32( &scr->ViewPort, ( ULONG * )color_list );
                                                                                                        break;
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
                                                /*
                                                **      Release the allocated pens.
                                                **/
                                                while ( numPens > 0 ) {
                                                        numPens--;
                                                        ReleasePen( scr->ViewPort.ColorMap, penns[ numPens ] );
                                                }
                                                /*
                                                **      Close the screen.
                                                **/
                                                CloseScreen( scr );
                                        }
                                }
                        }
                        /*
                        **      Close the gradient.gadget.
                        **/
                        CloseLibrary( GradientSliderBase );
                }
                /*
                **      Close the colorwheel.gadget.
                **/
                CloseLibrary( ColorWheelBase );
        }
}
