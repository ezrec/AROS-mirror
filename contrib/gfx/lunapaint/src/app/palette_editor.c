/****************************************************************************
*                                                                           *
* palette_editor.c -- Lunapaint,                                            *
*    http://developer.berlios.de/projects/lunapaintami/                     *
* Copyright (C) 2006, 2007, Hogne Titlestad <hogga@sub-ether.org>           *
* Copyright (C) 2009-2011 LunaPaint Development Team                        *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with this program; if not, write to the Free Software Foundation,   *
* Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.            *
*                                                                           *
****************************************************************************/
#include <SDI_hook.h>
#include "palette_editor.h"

Object *paletteWindow;
Object *paletteRect;
Object *palSlideR;
Object *palSlideG;
Object *palSlideB;
Object *palColRect;
Object *palBtnUse;
Object *palBtnSave;
Object *palBtnLoad;
Object *palStrFileSave;

// Action buttons
Object *palBtnCopy;
Object *palBtnSwap;
Object *palBtnSpread;
Object *palBtnReverse;
Object *palBtnClear;
Object *palBtnPaste;

unsigned int tempCopiedColor;
BOOL tempCopiedColorExists;

struct Hook rgbslider_hook;
struct Hook paletteSave_hook;
struct Hook paletteLoad_hook;
struct Hook paletteClose_hook;


/*
    Call the function that opens a palette file requestor and
    save on the resulting file...
*/

HOOKPROTONHNO(paletteSave_func, ULONG, int *param)
{
    savePalette ( );
    return ( IPTR )NULL;
}

/*
    We do this when we close the palette window
*/

HOOKPROTONHNO(paletteClose_func, ULONG, int *param)
{
    set ( paletteWindow, MUIA_Window_Open, FALSE );
    DoMethod ( tbxAreaPalette, MUIM_Draw );
    return ( IPTR )NULL;
}

/*
    Call the function that opens a palette file requestor and
    load the resulting file...
*/

HOOKPROTONHNO (paletteLoad_func, ULONG, int *param)
{
    loadPalette ( );
    tbxPaletteRedraw ( );

    return ( IPTR )NULL;
}
/*
    To this every time a slider RGB moves...
*/
HOOKPROTONHNO(rgbslider_func, ULONG, int *param)
{

    // Set current selected color

    int R = XGET ( palSlideR, MUIA_Numeric_Value );
    int G = XGET ( palSlideG, MUIA_Numeric_Value );
    int B = XGET ( palSlideB, MUIA_Numeric_Value );

    if ( currColor >= 0 && currColor < 256 )
        globalPalette[ currColor ] = RGBtoPaletteColor ( R, G, B );

    updateColorPreview ( ); PaletteRedraw ( currColor );

    return 0;
}


/*
    Dispatcher for our palette area
*/

DISPATCHERPROTO(PaletteArea)
{
    switch ( msg->MethodID )
    {
        case MUIM_Draw:
            return ( IPTR )PaletteRedraw ( -1 );

        case MUIM_HandleInput:
            return ( IPTR )PaletteEvents ( cl, obj, ( struct MUIP_HandleInput* )msg );

        case MUIM_Setup:

            if ( 1 )
            {
                // Update slider positions when the window is initialized
                struct rgbData colors = paletteColorToRGB ( globalPalette[ currColor ] );
                set ( palSlideR, MUIA_Numeric_Value, colors.r );
                set ( palSlideG, MUIA_Numeric_Value, colors.g );
                set ( palSlideB, MUIA_Numeric_Value, colors.b );
            }
            MUI_RequestIDCMP( obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY );
            return DoSuperMethodA ( cl, obj, msg );

        case MUIM_Luna_Palette_Copy:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Copy );

        case MUIM_Luna_Palette_Paste:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Paste );

        case MUIM_Luna_Palette_Swap:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Swap );

        case MUIM_Luna_Palette_Reverse:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Reverse );

        case MUIM_Luna_Palette_Spread:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Spread );

        case MUIM_Luna_Palette_Clear:
            return ( IPTR )PaletteActions ( MUIM_Luna_Palette_Clear );

        /* Tool stuff (should be in a dispatcher in the toolbox! */

        case MUIM_Luna_SetTool_Draw:
            if ( globalCurrentTool != LUNA_TOOL_CLIPBRUSH )
            {
                globalBrushMode = 0; // normal drawing mode
                DoMethod ( tbxAreaPalette, MUIM_Luna_Canvas_AlterBrushShape );
            }
            globalCurrentTool = LUNA_TOOL_BRUSH;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_Fill:

            // Make a small brush now
            globalCurrentTool = LUNA_TOOL_FILL;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_Line:
            globalCurrentTool = LUNA_TOOL_LINE;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_Rectangle:
            globalCurrentTool = LUNA_TOOL_RECTANGLE;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_Circle:
            globalCurrentTool = LUNA_TOOL_CIRCLE;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_ClipBrush:
            globalCurrentTool = LUNA_TOOL_CLIPBRUSH;
            globalBrushMode = 1; // clipbrush mode
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_SetTool_Colorpicker:
            globalCurrentTool = LUNA_TOOL_COLORPICKER;
            setToolbarActive ( );
            return 0;

        case MUIM_Luna_ExecuteRevert:
            checkMenuEvents ( 595 );
            return 0;

        /* End tool stuff */


        default:
            return DoSuperMethodA ( cl, obj, msg );
    }
    return ( IPTR )0;
}

IPTR PaletteRedraw ( int colorIndex )
{
    BOOL WeAreOpen = FALSE; get ( paletteWindow, MUIA_Window_Open, &WeAreOpen );
    if ( !WeAreOpen ) return ( IPTR )NULL;

    int Y = XGET ( paletteRect, MUIA_TopEdge );
    int X = XGET ( paletteRect, MUIA_LeftEdge );
    int W = XGET ( paletteRect, MUIA_Width );
    int H = XGET ( paletteRect, MUIA_Height );

    ULONG *tempBuf = AllocVec ( W * H * 4, MEMF_ANY );

    double xslice = ( ( double )W / 16 );
    double yslice = ( ( double )H / 16 );
    double targetx = xslice * ( currColor % 16 );
    double targety = yslice * ( currColor / 16 );

    int y = 0; for ( ; y < H; y++ )
    {
        int x = 0; for ( ; x < W; x++ )
        {
            int inX = ( double )x / W * 16;
            int inY = ( double )y / H * 16;
            int index = inY * 16 + inX;
            char check = ( y + x ) % 2;

            if (
                index == currColor &&
                (
                    x == ( int )targetx || x == ( int )( targetx + xslice - 1 ) ||
                    y == ( int )targety || y == ( int )( targety + yslice - 1 )
                ) &&
                check == 0
            )
            {
                tempBuf[ y * W + x ] = 0xFFFFFF;
            }
            else
                tempBuf[ y * W + x ] = globalPalette[ index ];
        }
    }

    if ( globalBrushMode == 0 ) makeToolBrush ( );

    // Plot all colors on screen
    if ( colorIndex == -1 )
    {
        WritePixelArray (
            tempBuf, 0, 0, 4 * W, _rp ( paletteRect ), X, Y, W, H, RECTFMT_RGBA
        );
    }
    else
    // Write only the selected color on screen
    // and the previous selected color
    {
        // Draw over previous color
        int targetx = ( prevColor % 16 ) * xslice;
        int targety = ( prevColor / 16 ) * yslice;
        WritePixelArray (
            tempBuf, targetx, targety, 4 * W,
            _rp ( paletteRect ), targetx + X, targety + Y, ( int )xslice, ( int )yslice,
            RECTFMT_RGBA
        );
        // Current color
        targetx = ( currColor % 16 ) * xslice;
        targety = ( currColor / 16 ) * yslice;
        WritePixelArray (
            tempBuf, targetx, targety, 4 * W,
            _rp ( paletteRect ), targetx + X, targety + Y, ( int )xslice, ( int )yslice,
            RECTFMT_RGBA
        );
    }

    FreeVec ( tempBuf );

    updateColorPreview ( );

    return ( IPTR )NULL;
}

IPTR PaletteEvents ( Class *CLASS, Object *self, struct MUIP_HandleInput *msg )
{
    if ( msg->imsg )
    {
        switch ( msg->imsg->Class )
        {
            case IDCMP_MOUSEBUTTONS:
                if ( msg->imsg->Code == SELECTDOWN )
                {
                    // Set a color based on click in palette
                    int mousex, mousey, topedge=0, leftedge=0, wi=0, he=0;
                    double slicex, slicey;
                    get ( paletteRect, MUIA_TopEdge, &topedge );
                    get ( paletteRect, MUIA_LeftEdge, &leftedge );
                    get ( paletteRect, MUIA_Width, &wi );
                    get ( paletteRect, MUIA_Height, &he );
                    slicex = ( ( double )wi / 16 );
                    slicey = ( ( double )he / 16 );
                    mousex = ( int )msg->imsg->MouseX - leftedge;
                    mousey = ( int )msg->imsg->MouseY - topedge;

                    // Only regard things happening in the palette area
                    // widget itself
                    if ( mousex >= 0 && mousey >= 0 && mousex < wi && mousey < he )
                    {
                        int indexX = ( int )( ( double )mousex / slicex );
                        int indexY = ( int )( ( double )mousey / slicey );
                        int index = ( unsigned int )( ( indexY * 16 ) + indexX );
                        if ( index >= 0 && index < 256 )
                        {
                            prevColor = currColor;
                            currColor = ( unsigned int )( ( indexY * 16 ) + indexX );
                            globalColor = globalPalette[ currColor ];
                            DoMethod ( tbxAreaPalette, MUIM_Draw );
                        }

                        // Update slider positions
                        struct rgbData colors = paletteColorToRGB ( globalPalette[ currColor ] );
                        set ( palSlideR, MUIA_Numeric_Value, colors.r );
                        set ( palSlideG, MUIA_Numeric_Value, colors.g );
                        set ( palSlideB, MUIA_Numeric_Value, colors.b );
                    }
                }
                break;
            default:
                break;
        }
    }
    return ( IPTR )NULL;
}

IPTR PaletteActions ( ULONG action )
{
    switch ( action )
    {
        case MUIM_Luna_Palette_Copy:
            tempCopiedColor = *( globalPalette + currColor );
            tempCopiedColorExists = TRUE;
            break;

        case MUIM_Luna_Palette_Paste:

            if ( tempCopiedColorExists )
            {
                // Paste copied color
                *( globalPalette + currColor ) = tempCopiedColor;
                // Update slider positions
                struct rgbData colors = paletteColorToRGB ( *( globalPalette + currColor ) );
                set ( palSlideR, MUIA_Numeric_Value, colors.r );
                set ( palSlideG, MUIA_Numeric_Value, colors.g );
                set ( palSlideB, MUIA_Numeric_Value, colors.b );
            }

            break;

        case MUIM_Luna_Palette_Swap:

            // Swap two colors
            if ( prevColor != currColor )
            {
                unsigned int tempcol = *( globalPalette + prevColor );
                *( globalPalette + prevColor ) = *( globalPalette + currColor );
                *( globalPalette + currColor ) = tempcol;
                // Update slider positions
                struct rgbData colors = paletteColorToRGB ( *( globalPalette + currColor ) );
                set ( palSlideR, MUIA_Numeric_Value, colors.r );
                set ( palSlideG, MUIA_Numeric_Value, colors.g );
                set ( palSlideB, MUIA_Numeric_Value, colors.b );
                // Redraw everything
                PaletteRedraw ( -1 );
            }

            break;

        case MUIM_Luna_Palette_Reverse:

            if ( currColor != prevColor )
            {
                unsigned int firstCol = ( currColor > prevColor ) ? prevColor : currColor;
                unsigned int lastCol = ( currColor > prevColor ) ? currColor : prevColor;
                int span = ( int )lastCol - ( int )firstCol + 1;
                unsigned int *temp = AllocVec ( 4 * span, MEMF_ANY );
                // Copy the present order
                unsigned int i = 0; for ( ; i < span; i++ )
                {
                    temp[ i ] = globalPalette[ firstCol + i ];
                }
                // Place the reverse order
                for ( i = 0; i < span; i++ )
                {
                    globalPalette[ lastCol - i ] = temp[ i ];
                }

                FreeVec ( temp );

                // Update slider positions
                struct rgbData colors = paletteColorToRGB ( *( globalPalette + currColor ) );
                set ( palSlideR, MUIA_Numeric_Value, colors.r );
                set ( palSlideG, MUIA_Numeric_Value, colors.g );
                set ( palSlideB, MUIA_Numeric_Value, colors.b );
                // Redraw everything
                PaletteRedraw ( -1 );
            }

            break;

        case MUIM_Luna_Palette_Spread:
            if ( currColor != prevColor )
            {
                unsigned int firstCol = ( currColor > prevColor ) ? prevColor : currColor;
                unsigned int lastCol = ( currColor > prevColor ) ? currColor : prevColor;
                struct rgbData source = paletteColorToRGB ( *( globalPalette + firstCol ) );
                struct rgbData dest = paletteColorToRGB ( *( globalPalette + lastCol ) );
                int span = lastCol - firstCol;
                unsigned int i; for ( i = 1; i < span; i++ )
                {
                    struct rgbData result;
                    int dr = ( int )dest.r;
                    int dg = ( int )dest.g;
                    int db = ( int )dest.b;
                    int sr = ( int )source.r;
                    int sg = ( int )source.g;
                    int sb = ( int )source.b;
                    result.r = ( unsigned int )( sr - ( ( double )( sr - dr ) * ( ( double )i / span ) ) );
                    result.g = ( unsigned int )( sg - ( ( double )( sg - dg ) * ( ( double )i / span ) ) );
                    result.b = ( unsigned int )( sb - ( ( double )( sb - db ) * ( ( double )i / span ) ) );
                    *( globalPalette + firstCol + i ) = RGBtoPaletteColor ( result.r, result.g, result.b );
                }
                // Redraw everything
                PaletteRedraw ( -1 );
            }
            break;

        case MUIM_Luna_Palette_Clear:
            if ( 1 )
            {
                // Clear all colors in palette with current color
                unsigned int tempColor = *( globalPalette + currColor );
                unsigned int a = 0; for ( a = 0; a < 256; a++ )
                    *( globalPalette + a ) = tempColor;
                // Update slider positions
                struct rgbData colors = paletteColorToRGB ( *( globalPalette + currColor ) );
                set ( palSlideR, MUIA_Numeric_Value, colors.r );
                set ( palSlideG, MUIA_Numeric_Value, colors.g );
                set ( palSlideB, MUIA_Numeric_Value, colors.b );
                // Redraw everything
                PaletteRedraw ( -1 );
            }
            break;

        default:
            break;
    }
    return ( IPTR )NULL;
}

void updateColorPreview ( )
{
    // Update only color preview if we have an open window!

    BOOL state = FALSE;

    get ( paletteWindow, MUIA_Window_Open, &state );

    if ( state == TRUE )
    {
        int R=0, G=0, B=0, W=0, H=0, TE=0, LE=0;

        get ( palSlideR, MUIA_Numeric_Value, &R );
        get ( palSlideG, MUIA_Numeric_Value, &G );
        get ( palSlideB, MUIA_Numeric_Value, &B );
        get ( palColRect, MUIA_TopEdge, &TE );
        get ( palColRect, MUIA_LeftEdge, &LE );
        get ( palColRect, MUIA_Width, &W );
        get ( palColRect, MUIA_Height, &H );

        ULONG *tempBuf = AllocVec ( 4 * ( W * H ), MEMF_ANY );
        unsigned int tempCol = RGBtoPaletteColor ( R, G, B );

        int Y = 0; for ( ; Y < H; Y++ )
        {
            int X = 0; for ( ; X < W; X++ )
                tempBuf[ ( W * Y ) + X ] = tempCol;
        }
        WritePixelArray (
            tempBuf, 0, 0, W * 4, _rp ( palColRect ),
            LE, TE, W, H, RECTFMT_RGBA
        );
        FreeVec ( tempBuf );
    }
}

void Init_PaletteWindow ( )
{
    struct MUI_CustomClass *mcc =
        MUI_CreateCustomClass (
            NULL, MUIC_Rectangle, NULL,
            0,
            &PaletteArea
        );

    paletteWindow = WindowObject,
        MUIA_Window_Title, __(MSG_PALETTE_WIN),
        MUIA_Window_SizeGadget, FALSE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','P','W'),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, ( IPTR )HGroup,
                    Child, ( IPTR )VGroup,
                        Child, ( IPTR )HGroup,
                            Child, ( IPTR )VGroup,
                                Child, ( IPTR )( palBtnCopy = SimpleButton ( __(MSG_PALETTE_COPY) )),
                                Child, ( IPTR )( palBtnPaste = SimpleButton ( __(MSG_PALETTE_PASTE) )),
                                Child, ( IPTR )( palBtnClear = SimpleButton ( __(MSG_PALETTE_CLEAR) )),
                            End,
                            Child, ( IPTR )VGroup,
                                Child, ( IPTR )( palBtnReverse = SimpleButton ( __(MSG_PALETTE_REVERSE) )),
                                Child, ( IPTR )( palBtnSwap = SimpleButton ( __(MSG_PALETTE_SWAP) )),
                                Child, ( IPTR )( palBtnSpread = SimpleButton ( __(MSG_PALETTE_SPREAD) )),
                            End,
                        End,
                        Child, ( IPTR )VGroup,
                            Child, ( IPTR )RectangleObject,
                                MUIA_Frame, MUIV_Frame_None,
                                MUIA_FixHeight, 10,
                                MUIA_Rectangle_HBar, TRUE,
                            End,
                            Child, ( IPTR )StringObject,
                                MUIA_String_MaxLen, 8,
                                MUIA_String_Format, MUIV_String_Format_Center,
                                MUIA_Frame, MUIV_Frame_String,
                            End,
                            Child, ( IPTR )SimpleButton ( __(MSG_PALETTE_WEB) ),
                        End,
                    End,
                    Child, ( IPTR )VGroup,
                        Child, ( IPTR )HGroup,
                            Child, ( IPTR )GroupObject,
                                MUIA_Frame, MUIV_Frame_Group,
                                Child, ( IPTR )( palColRect = RectangleObject,
                                    MUIA_FixHeight, 15,
                                End ),
                            End,
                        End,
                        Child, ( IPTR )HGroup,
                            Child, ( IPTR )( palSlideR = SliderObject,
                                MUIA_Slider_Horiz, FALSE,
                                MUIA_Numeric_Min, 0,
                                MUIA_Numeric_Max, 255,
                                MUIA_Numeric_Value, 0,
                            End ),
                            Child, ( IPTR )( palSlideG = SliderObject,
                                MUIA_Slider_Horiz, FALSE,
                                MUIA_Numeric_Min, 0,
                                MUIA_Numeric_Max, 255,
                                MUIA_Numeric_Value, 0,
                            End ),
                            Child, ( IPTR )( palSlideB = SliderObject,
                                MUIA_Slider_Horiz, FALSE,
                                MUIA_Numeric_Min, 0,
                                MUIA_Numeric_Max, 255,
                                MUIA_Numeric_Value, 0,
                            End ),
                        End,
                    End,
                End,
                Child, ( IPTR )VGroup,
                    Child, ( IPTR )RectangleObject,
                        MUIA_Frame, MUIV_Frame_None,
                        MUIA_FixHeight, 10,
                        MUIA_Rectangle_HBar, TRUE,
                    End,
                    Child, ( IPTR )GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        Child, ( IPTR )( paletteRect = NewObject (
                        mcc->mcc_Class, NULL,
                        MUIA_FixWidth, 256,
                        MUIA_FixHeight, 128,
                        TAG_DONE
                        ) ),
                    End,
                End,
                Child, HGroup,
                    Child, ( IPTR )( palBtnLoad = SimpleButton ( __(MSG_PALETTE_LOAD) ) ),
                    Child, ( IPTR )( palBtnSave = SimpleButton ( __(MSG_PALETTE_SAVE) ) ),
                    Child, ( IPTR )( palBtnUse = SimpleButton (  __(MSG_PALETTE__USE)  )),
                End,
            End,
        End,
    End;
}

void Init_PaletteMethods ( )
{
    // Setup hooks
    rgbslider_hook.h_Entry = ( HOOKFUNC )&rgbslider_func;
    paletteLoad_hook.h_Entry = ( HOOKFUNC )&paletteLoad_func;
    paletteSave_hook.h_Entry = ( HOOKFUNC )&paletteSave_func;
    paletteClose_hook.h_Entry = ( HOOKFUNC )&paletteClose_func;

    // Set initial slider positions
    struct rgbData colors = paletteColorToRGB ( globalPalette[ currColor ] );
    set ( palSlideR, MUIA_Numeric_Value, colors.r );
    set ( palSlideG, MUIA_Numeric_Value, colors.g );
    set ( palSlideB, MUIA_Numeric_Value, colors.b );

    DoMethod (
        palSlideR, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
        palSlideR, 3, MUIM_CallHook, &rgbslider_hook, MUIV_TriggerValue
    );
    DoMethod (
        palSlideG, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
        palSlideG, 3, MUIM_CallHook, &rgbslider_hook, MUIV_TriggerValue
    );
    DoMethod (
        palSlideB, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
        palSlideB, 3, MUIM_CallHook, &rgbslider_hook, MUIV_TriggerValue
    );
    DoMethod (
        palBtnUse, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteWindow, 3, MUIM_CallHook, &paletteClose_hook
    );
    DoMethod (
        paletteWindow, MUIM_Notify,
        MUIA_Window_CloseRequest, TRUE,
        paletteWindow, 3, MUIM_CallHook, &paletteClose_hook
    );

    // Action buttons
    DoMethod (
        palBtnCopy, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Copy
    );
    DoMethod (
        palBtnPaste, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Paste
    );
    DoMethod (
        palBtnSpread, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Spread
    );
    DoMethod (
        palBtnSwap, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Swap
    );
    DoMethod (
        palBtnReverse, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Reverse
    );
    DoMethod (
        palBtnClear, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_Palette_Clear
    );

    // Load/Save
    DoMethod (
        palBtnSave, MUIM_Notify, MUIA_Pressed, FALSE,
        palBtnSave, 3, MUIM_CallHook, &paletteSave_hook, MUIV_TriggerValue
    );
    DoMethod (
        palBtnLoad, MUIM_Notify, MUIA_Pressed, FALSE,
        palBtnLoad, 3, MUIM_CallHook, &paletteLoad_hook, MUIV_TriggerValue
    );
}

ULONG savePalette ( )
{
    // We have a filename!!
    char *filename = getFilename ( );
    if ( filename != NULL )
    {
        // The aros way!
        BPTR myfile;
        if ( ( myfile = Open ( filename, MODE_NEWFILE ) ) != NULL )
        {
            Write ( myfile, globalPalette, 4 * 256 );
            Close ( myfile );
        }
        else printf ( "Failed to write palette..\n" );
        FreeVec ( filename );
    }
    return 0;
}

ULONG loadPalette ( )
{
    // We have a filename!!
    char *filename = getFilename ( );
    if ( filename != NULL )
    {
        // The aros way!
        BPTR myfile;
        if ( ( myfile = Open ( filename, MODE_OLDFILE ) ) != NULL )
        {
            Read ( myfile, globalPalette, 4 * 256 );
            Close ( myfile );
        }
        else printf ( "Failed to load palette..\n" );
        FreeVec ( filename );
    }
    return 0;
}


