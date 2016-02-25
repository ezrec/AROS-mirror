/****************************************************************************
*                                                                           *
* toolbox.c -- Lunapaint,                                                   *
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
#include "toolbox.h"

struct Hook getMenu_hook;
struct Hook brushOptions_hook;
struct Hook getFill_hook;
struct Hook getOpacMode_hook;

int ToolboxRunning;
Object *toolbox;
Object *toolboxTopGroup;
Object *tbxAreaPalette;
Object *tbxAreaPreview;
Object *tbxBtn_draw;
Object *tbxBtn_line;
Object *tbxBtn_rectangle;
Object *tbxBtn_polygon;
Object *tbxBtn_circle;
Object *tbxBtn_fill;
Object *tbxBtn_getbrush;
Object *tbxBtn_pickcolor;

char tbxPaletteClickMode;

Object *antiImage, *antiOffImage, *solidImage, *featherImage;
Object *brushOpGroup;

Object *tbxCyc_GridSize;
Object *tbxCycColorCtrl;

Object *tbxSlider_Brushdiameter;
Object *tbxSlider_Brushopacity;
Object *tbxSlider_Brushincrement;
Object *tbxSlider_Brushstep;
Object *tbxCycGrid;
Object *tbxCycFillmode;

Object *tbxCyc_PaletteSnap;

Object *tbxCycPaintMode;

Object *offsetWindow, *offsetWindowOk, *offsetWindowCancel, *offsetWindowX, *offsetWindowY;

unsigned int *PreviewRectData;

static STRPTR arr_drawmodes[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static STRPTR arr_fillmode[]  = { NULL, NULL, NULL, NULL};
static STRPTR arr_colorctrl[] = { NULL, NULL, NULL};
static STRPTR arr_grid[]      = { NULL, NULL, NULL};
static STRPTR arr_grdz[]      = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static STRPTR arr_pltsnap[]   = { NULL, NULL, NULL};

HOOKPROTONHNO(getMenu_func, void, int *param)
{
   checkMenuEvents (*param);
}

/* Get the opacity mode (add/keep) */
HOOKPROTONHNO(getOpacMode_func, void, int *param)
{
   brushTool.opacitymode = XGET( tbxCycColorCtrl, MUIA_Cycle_Active );
}

HOOKPROTONHNO(getFill_func, void, int *param)
{
	GetDrawFillState ( );
}


HOOKPROTONHNO(brushOptions_func, void, int *param)
{

    //int val = *( int *)param;
    int val = (*param);

    switch ( val )
    {
        case SET_ANTIALIAS:
            set ( antiImage, MUIA_Background, MUII_FILL );
            set ( antiOffImage, MUIA_Background, MUII_ButtonBack );
            brushTool.antialias = TRUE;
            break;
        case SET_ANTIALIASOFF:
            set ( antiImage, MUIA_Background, MUII_ButtonBack );
            set ( antiOffImage, MUIA_Background, MUII_FILL );
            brushTool.antialias = FALSE;
            break;
        case SET_FEATHERED:
            set ( featherImage, MUIA_Background, MUII_FILL );
            set ( solidImage, MUIA_Background, MUII_ButtonBack );
            brushTool.feather = TRUE;
            break;
        case SET_FEATHEREDOFF:
            set ( featherImage, MUIA_Background, MUII_ButtonBack );
            set ( solidImage, MUIA_Background, MUII_FILL );
            brushTool.feather = FALSE;
            break;
        default: break;
    }
    DoMethod ( tbxAreaPalette, MUIM_Luna_Canvas_AlterBrushShape );
}

DISPATCHERPROTO(tbxPreview)
{ 

  switch ( msg->MethodID )
    {
        case MUIM_Draw:
            return ( IPTR )tbxPaintPreview();
        default:
            return DoSuperMethodA ( cl, obj, (struct MUIP_Draw *)msg );
    }
    return ( IPTR )NULL;
}


void prevPaletteColor ( )
{
    currColor = ( currColor - 1 ) % 256;
    DoMethod ( tbxAreaPalette, MUIM_Draw );
    if ( globalBrushMode == 0 )
        makeToolBrush ( );
}
void nextPaletteColor ( )
{
    currColor = ( currColor + 1 ) % 256;
    DoMethod ( tbxAreaPalette, MUIM_Draw );
    if ( globalBrushMode == 0 )
        makeToolBrush ( );
}

IPTR tbxPaintPreview ( )
{
    if ( !XGET( toolbox, MUIA_Window_Open ) ) return 0;

    unsigned int w = XGET ( tbxAreaPreview, MUIA_Width );
    unsigned int h = XGET ( tbxAreaPreview, MUIA_Height );
    unsigned int t = XGET ( tbxAreaPreview, MUIA_TopEdge );
    unsigned int l = XGET ( tbxAreaPreview, MUIA_LeftEdge );
    unsigned int rectsize = w * h;

    unsigned int bpw = h;

    if ( PreviewRectData != NULL )
        FreeVec ( PreviewRectData );
    PreviewRectData = AllocVec ( rectsize * 4, MEMF_ANY );
    int i = 0; for ( ; i < rectsize; i++ )
    {
        int x = i % w;
        int y = i / w;

        // Draw brush
        if ( x >= 0 && x < bpw && y >= 0 && y < h && brushTool.buffer != NULL )
        {
            int offx = ( ( double )x / bpw ) * brushTool.width;
            int offy = ( ( double )y / h ) * brushTool.height;

            struct rgbaData rgba = canvasColorToRGBA (
                brushTool.buffer[ offy * brushTool.width + offx ]
            );

            if ( rgba.a == 0 )
            {
                PreviewRectData[ i ] = 0x000000;
            }
            else
            {
                // Blend towards gray
                rgba.r -= ( int )( ( double )( rgba.r ) / rgba.a );
                rgba.g -= ( int )( ( double )( rgba.g ) / rgba.a );
                rgba.b -= ( int )( ( double )( rgba.b ) / rgba.a );
                PreviewRectData[ i ] = rgba.r | ( rgba.g << 8 ) | ( rgba.b << 16 ) | 0;
            }
        }
        else
            PreviewRectData[ i ] = 0x707070;
    }

    // UPDATE
    WritePixelArray (
        PreviewRectData, 0, 0, w * 4, _rp ( tbxAreaPreview ), l, t, w, h, RECTFMT_RGBA
    );

    return ( IPTR )NULL;
}

DISPATCHERPROTO(tbxPalette)
{
    switch ( msg->MethodID )
    {
        case MUIM_Draw:
            return ( IPTR )tbxPaletteRedraw ();

        case MUIM_HandleInput:
            return ( IPTR )tbxPaletteThink ( ( struct MUIP_HandleInput* )msg );

        case MUIM_Setup:
            MUI_RequestIDCMP( obj, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY );
            return DoSuperMethodA ( cl, obj, msg );

        case MUIM_Luna_Canvas_OpenPaletteEditor:
            set ( paletteWindow, MUIA_Window_Open, TRUE );
            break;

        case MUIM_Luna_Canvas_ClearActiveCanvas:

            // Clear active canvas and redraw...
            if ( globalActiveCanvas != NULL )
            {
                int i; for ( i = 0; i < globalActiveCanvas->height * globalActiveCanvas->width; i++ )
                    globalActiveCanvas->activebuffer[ i ] = TRANSCOLOR;
                globalActiveCanvas->winHasChanged = TRUE;
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
                DoMethod ( WidgetLayers, MUIM_Draw ); // <- force redraw
            }
            break;

        case MUIM_Luna_Canvas_SetPaintMode:

            if ( 1 )
            {
                LONG num = 0; get ( tbxCycPaintMode, MUIA_Cycle_Active, &num );
                switch ( num )
                {
                    case 0:
                        brushTool.paintmode = LUNA_PAINTMODE_NORMAL;
                        break;
                    case 1:
                        brushTool.paintmode = LUNA_PAINTMODE_COLOR;
                        break;
                    case 2:
                        brushTool.paintmode = LUNA_PAINTMODE_BLUR;
                        break;
                    case 3:
                        brushTool.paintmode = LUNA_PAINTMODE_SMUDGE;
                        break;
                    case 4:
                        brushTool.paintmode = LUNA_PAINTMODE_LIGHTEN;
                        break;
                    case 5:
                        brushTool.paintmode = LUNA_PAINTMODE_DARKEN;
                        break;
                    case 6:
                        brushTool.paintmode = LUNA_PAINTMODE_COLORIZE;
                        break;
                    case 7:
                        brushTool.paintmode = LUNA_PAINTMODE_ERASE;
                        break;
                    case 8:
                        brushTool.paintmode = LUNA_PAINTMODE_UNERASE;
                        break;
                    default: return 0;
                }
                if ( globalBrushMode == 0 )
                    makeToolBrush ( );
            }
            break;

        case MUIM_Luna_Canvas_AlterBrushShape:
            {
                // Get brushsize from toolbox and set it
                int tempWidth = 0, tempHeight = 0, tempOpacity = 0, tempPower = 0;
                get ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, &tempWidth );
                get ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, &tempHeight );
                get ( tbxSlider_Brushopacity, MUIA_Numeric_Value, &tempOpacity );
                get ( tbxSlider_Brushincrement, MUIA_Numeric_Value, &tempPower );
                brushTool.step = XGET ( tbxSlider_Brushstep, MUIA_Numeric_Value );

                // Opacity and power always change
                brushTool.opacity = tempOpacity;
                brushTool.power = tempPower;

                // Alter size and start making a brush
                brushTool.width = tempWidth;
                brushTool.height = tempHeight;
                makeToolBrush ( );
                return ( IPTR )NULL;
            }
            break;
        case MUIM_Luna_Canvas_AlterBrushStrength:
            {
                // Get brushsize from toolbox and set it
                int tempOpacity = 0, tempPower = 0;
                get ( tbxSlider_Brushopacity, MUIA_Numeric_Value, &tempOpacity );
                get ( tbxSlider_Brushincrement, MUIA_Numeric_Value, &tempPower );
                brushTool.opacity = tempOpacity;
                brushTool.power = tempPower;
                return ( IPTR )NULL;
            }
            break;
        case MUIM_Luna_Canvas_AlterBrushStep:
            brushTool.step = XGET( tbxSlider_Brushstep, MUIA_Numeric_Value );
            break;

        case MUIM_Luna_Canvas_SetGlobalGrid:
            {
                int num = 0;
                get ( tbxCycGrid, MUIA_Cycle_Active, &num );
                if ( num == 0 )
                    globalGrid = 0;
                else
                    globalGrid = 1;
            }
            break;

        case MUIM_Luna_Canvas_SetGridSize:
            {
                int num = 0;
                get ( tbxCyc_GridSize, MUIA_Cycle_Active, &num );
                globalCurrentGrid = num + 2;
            }
            break;

        case MUIM_Luna_NewProject:
            {
                int width = 0, height = 0, frames = 0;
                get ( nwStringWidth, MUIA_String_Integer, &width );
                get ( nwStringHeight, MUIA_String_Integer, &height );
                get ( nwStringFrames, MUIA_String_Integer, &frames );

                // TODO: Add memory checks!!!
                if ( width > 0 && height > 0 && frames > 0 )
                {
                    // TODO: Could projectName remain used?
                    addCanvaswindow ( width, height, 1, frames, TRUE );
                    STRPTR txtbuf = ( STRPTR )NULL;
                    get ( nwStringPrjName, MUIA_String_Contents, &txtbuf );
                    set ( globalActiveWindow->win, MUIA_Window_Title, ( IPTR )txtbuf );
                    set ( globalActiveWindow->win, MUIA_Window_Open, TRUE );
                    set ( nwWindow, MUIA_Window_Open, FALSE );
                }
            }
            break;

        case MUIM_Luna_Canvas_EffectOffset:

            if ( globalActiveCanvas != NULL )
            {
                int x = 0, y = 0;
                get ( offsetWindowX, MUIA_String_Integer, &x );
                get ( offsetWindowY, MUIA_String_Integer, &y );
                effectOffset ( x, y, globalActiveCanvas );
                globalActiveCanvas->winHasChanged = TRUE;
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
            }
            break;

        case MUIM_Luna_Canvas_SetColorMode:
            {
                int num = 0; get ( tbxCyc_PaletteSnap, MUIA_Cycle_Active, &num );
                if ( num == 0 )
                    globalColorMode = LUNA_COLORMODE_64BIT;
                else if ( num == 1 )
                    globalColorMode = LUNA_COLORMODE_SNAP;
            }
            break;

        default:
            return DoSuperMethodA ( cl, obj, msg );
    }
    return ( IPTR )0;
}


IPTR tbxPaletteRedraw ( )
{
    if ( !XGET ( toolbox, MUIA_Window_Open ) ) return 0;

    int topedge = 0, leftedge = 0, w = 0, h = 0;
    get ( tbxAreaPalette, MUIA_TopEdge, &topedge );
    get ( tbxAreaPalette, MUIA_LeftEdge, &leftedge );
    get ( tbxAreaPalette, MUIA_Width, &w );
    get ( tbxAreaPalette, MUIA_Height, &h );

    unsigned int *tmpRect = AllocVec ( 4 * ( w * h ), MEMF_ANY );

    // Set how broad each color rect is
    float segW = ( float )w / 16;
    int segH = ( float )h / 16;
    int x1 = segW * ( currColor % 16 );
    int y1 = segH * ( currColor / 16 );

    // Draw color rects
    int y = 0; for ( ; y < h; y++ )
    {
        int posy = y * w;
        int x = 0; for ( ; x < w; x++ )
        {
            int offset = ( ( y / segH ) * ( w / segW ) ) + ( x / segW );
            if (
                ( x >= x1 && x < x1 + segW && ( y == y1 || y == y1 + segH - 1 ) ) ||
                ( y >= y1 && y < y1 + segH && ( x == x1 || x == x1 + ( int )segW ) )
            )
            {
                int sw = ( y + x ) % 2;
                tmpRect[ posy + x ] = ( sw == 1 ) ? 0x00ffffff : 0x00000000;
            }
            else
            {
                tmpRect[ posy + x ] = *( globalPalette + offset );
            }
        }
    }

    WritePixelArray (
        tmpRect, 0, 0, w * 4,
        _rp ( tbxAreaPalette ), leftedge, topedge, w, h,
        RECTFMT_RGBA
    );

    FreeVec ( tmpRect );

    // Set color
    globalColor = globalPalette[ currColor ];

    return ( IPTR )NULL;
}

IPTR tbxPaletteThink ( struct MUIP_HandleInput *msg )
{
    if ( !XGET ( toolbox, MUIA_Window_Open ) ) return 0;

    int topedge = 0, leftedge = 0, w = 0, h = 0;
    get ( tbxAreaPalette, MUIA_TopEdge, &topedge );
    get ( tbxAreaPalette, MUIA_LeftEdge, &leftedge );
    get ( tbxAreaPalette, MUIA_Width, &w );
    get ( tbxAreaPalette, MUIA_Height, &h );

    // Click counter for doubleclicking
    if ( mouseClickCount == 0 )
        tbxPaletteClickMode = 0;

    if ( msg->imsg )
    {
        switch ( msg->imsg->Class )
        {
            case IDCMP_MOUSEBUTTONS:
                if (
                    ( int )msg->imsg->MouseX - leftedge >= 0 &&
                    ( int )msg->imsg->MouseX - leftedge < w &&
                    ( int )msg->imsg->MouseY - topedge >= 0 &&
                    ( int )msg->imsg->MouseY - topedge < h
                )
                {
                    // Set how broad each color rect is
                    float w = ( float )XGET ( tbxAreaPalette, MUIA_Width );
                    int y = msg->imsg->MouseY - topedge;
                    int x = msg->imsg->MouseX - leftedge;
                    float wu = w / 16;
                    int segh = h / 16;

                    currColor = ( 16 * ( y / segh ) ) + ( x / wu );
                    tbxPaletteRedraw ( );
                    DoMethod ( paletteRect, MUIM_Draw );

                    // If we are using normal brushes, make a new
                    // brush with the new color
                    if ( globalBrushMode == 0 ) makeToolBrush ( );

                    if ( msg->imsg->Code == SELECTDOWN )
                    {
                        // On singleclick, make double click possible
                        if ( tbxPaletteClickMode == 0 )
                        {
                            tbxPaletteClickMode = 1;
                            mouseClickCount = MOUSE_DoubleClickTime;
                        }
                        // On doubleclick, open palette editor and make single click possible
                        else if ( tbxPaletteClickMode == 1 && mouseClickCount > 0 )
                        {
                            tbxPaletteClickMode = 0;
                            mouseClickCount = 0;
                            set ( paletteWindow, MUIA_Window_Open, TRUE );
                        }
                    }
                }
                break;

            default: break;
        }
    }
    return ( IPTR )NULL;
}


void Init_ToolboxWindow ( )
{
    // Palette widget
    struct MUI_CustomClass *mcc = MUI_CreateCustomClass (
        NULL, MUIC_Rectangle, NULL, 0, &tbxPalette
    );

    // Brush preview widget
    struct MUI_CustomClass *mcc_preview = MUI_CreateCustomClass (
        NULL, MUIC_Rectangle, NULL, 0, &tbxPreview
    );

    arr_fillmode[0] = _(MSG_TOOLBOX_DRAW);
    arr_fillmode[1] = _(MSG_TOOLBOX_FILL);
    arr_fillmode[2] = _(MSG_TOOLBOX_BOTH);
    arr_colorctrl[0] = _(MSG_TOOLBOX_ADD);
    arr_colorctrl[1] = _(MSG_TOOLBOX_LOCK);
    arr_grid[0] = _(MSG_TOOLBOX_GRIDOFF);
    arr_grid[1] = _(MSG_TOOLBOX_GRIDON);
    arr_grdz[0] = (STRPTR) "2";
    arr_grdz[1] = (STRPTR) "3";
    arr_grdz[2] = (STRPTR) "4";
    arr_grdz[3] = (STRPTR) "5";
    arr_grdz[4] = (STRPTR) "6";
    arr_grdz[5] = (STRPTR) "7";
    arr_grdz[6] = (STRPTR) "8";
    arr_grdz[7] = (STRPTR) "9";
    arr_grdz[8] = (STRPTR) "10";
    arr_drawmodes[0] = _(MSG_TOOLBOX_NORMAL);
    arr_drawmodes[1] = _(MSG_TOOLBOX_COLOR);
    arr_drawmodes[2] =  _(MSG_TOOLBOX_BLUR);
    arr_drawmodes[3] =  _(MSG_TOOLBOX_SMUDGE);
    arr_drawmodes[4] =  _(MSG_TOOLBOX_LIGHTEN);
    arr_drawmodes[5] =  _(MSG_TOOLBOX_DARKEN);
    arr_drawmodes[6] =  _(MSG_TOOLBOX_COLORIZE);
    arr_drawmodes[7] =  _(MSG_TOOLBOX_ERASE);
    arr_drawmodes[8] =  _(MSG_TOOLBOX_UNERASE);

    arr_pltsnap[0] = _(MSG_TOOLBOX_64B);
    arr_pltsnap[1] = _(MSG_TOOLBOX_PALETTE);

    offsetWindow = WindowObject,
        MUIA_Window_Title, __(MSG_TOOLBOX_WIN),
        MUIA_Window_ScreenTitle, (IPTR) VERSION, //__(MSG_TOOLBOX_SCR),
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','O','W'),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, ( IPTR )TextObject,
                    MUIA_Text_Contents, __(MSG_TOOLBOX_COOR),
                End,
                Child, ( IPTR )HGroup,
                    Child, ( IPTR )TextObject,
                        MUIA_Text_Contents, __(MSG_TOOLBOX_XOFFSET),
                        MUIA_Weight, 50,
                    End,
                    Child, ( IPTR )( offsetWindowX = StringObject,
                        MUIA_Weight, 60,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Format, MUIV_String_Format_Right,
                        MUIA_String_MaxLen, 4,
                        MUIA_CycleChain, 1,
                    End ),
                End,
                Child, ( IPTR )HGroup,
                    Child, ( IPTR )TextObject,
                        MUIA_Text_Contents, __(MSG_TOOLBOX_YOFFSET),
                        MUIA_Weight, 50,
                    End,
                    Child, ( IPTR )( offsetWindowY = StringObject,
                        MUIA_Weight, 60,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Format, MUIV_String_Format_Right,
                        MUIA_String_MaxLen, 4,
                        MUIA_CycleChain, 1,
                    End ),
                End,
            End,
            Child, ( IPTR )HGroup,
                Child, ( IPTR )( offsetWindowOk = SimpleButton ( _(MSG_TOOLBOX_OK) ) ),
                Child, ( IPTR )( offsetWindowCancel = SimpleButton ( __(MSG_TOOLBOX_CANCEL) ) ),
            End,
        End,
    End;

    // The toolbox
    toolbox = WindowObject,
        MUIA_Window_Title, __(MSG_TOOLBOX),
        MUIA_Window_ScreenTitle, ( IPTR )VERSION,
        MUIA_Window_ID, MAKE_ID('L','P','T','B'),
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_SizeGadget, FALSE,
//        MUIA_Window_Width, 150,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
//        MUIA_Window_LeftEdge, 0,
//        MUIA_Window_TopEdge, ( lunaPubScreen->BarHeight + 1 ),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )VGroup,
                Child, ( IPTR )( toolboxTopGroup = GroupObject,
                    Child, ( IPTR )GroupObject,
                        Child, ( IPTR )HGroup,
                            Child, ( IPTR )GroupObject,
                                MUIA_Frame, MUIV_Frame_Button,
                                Child, ( IPTR )( tbxAreaPreview = NewObject (
                                    mcc_preview->mcc_Class, NULL,
                                    InnerSpacing( 0, 0 ),
                                    MUIA_FixHeight, 54,
                                    MUIA_FixWidth, 54,
                                    TAG_DONE
                                ) ),
                            End,
                            Child, ( IPTR )VGroup,
                                MUIA_Group_HorizSpacing, 0,
                                MUIA_Group_VertSpacing, 0,
                                Child, ( IPTR )HGroup,
                                    MUIA_Group_HorizSpacing, 0,
                                    Child, ( IPTR )( tbxBtn_draw = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/draw.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_line = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/line.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_rectangle = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/rectangle.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_circle = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/circle.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                End,
                                Child, ( IPTR )HGroup,
                                    MUIA_Group_HorizSpacing, 0,
                                    Child, ( IPTR )( tbxBtn_polygon = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/polygon.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_fill = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/fill.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_getbrush = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/getbrush.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                    Child, ( IPTR )( tbxBtn_pickcolor = HGroup,
                                        MUIA_Frame, MUIV_Frame_Button,
                                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                                        Child, ImageObject,
                                            MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/pick_color.png",
                                            MUIA_Frame, MUIV_Frame_None,
                                        End,
                                    End ),
                                End,
                            End,
                        End,
                    End,
                    Child, ( IPTR )RectangleObject,
                        MUIA_Rectangle_HBar, TRUE,
                    End,
                    Child, ( IPTR )VGroup,
                        Child, ( IPTR )HGroup,
                            MUIA_Frame, MUIV_Frame_None,
                            Child, ( IPTR )VGroup,
                                MUIA_Weight, 64,
                                Child, ( IPTR )TextObject, MUIA_Text_Contents, __(MSG_TOOLBOX_BRUSHS), End,
                                Child, ( IPTR )( tbxSlider_Brushdiameter = SliderObject,
                                    MUIA_Numeric_Min, 1,
                                    MUIA_Numeric_Max, 100,
                                    MUIA_Numeric_Value, 1,
                                End ),
                            End,
                            Child, ( IPTR )VGroup,
                                MUIA_Weight, 32,
                                Child, ( IPTR )TextObject, MUIA_Text_Contents,__(MSG_TOOLBOX_STEP), End,
                                Child, ( IPTR )( tbxSlider_Brushstep = SliderObject,
                                    MUIA_Numeric_Min, 1,
                                    MUIA_Numeric_Max, 50,
                                    MUIA_Numeric_Value, 1,
                                End ),
                            End,
                        End,
                        Child, ( IPTR )HGroup,
                            Child, ( IPTR )VGroup,
                                MUIA_Weight, 64,
                                Child, ( IPTR )TextObject, MUIA_Text_Contents, __(MSG_TOOLBOX_OPACITY), End,
                                Child, ( IPTR )( tbxSlider_Brushopacity = SliderObject,
                                    MUIA_Numeric_Min, 1,
                                    MUIA_Numeric_Max, 255,
                                    MUIA_Numeric_Value, 255,
                                End ),
                            End,
                            Child, ( IPTR )VGroup,
                                MUIA_Weight, 32,
                                Child, ( IPTR )TextObject, MUIA_Text_Contents, __(MSG_TOOLBOX_POWER), End,
                                Child, ( IPTR )( tbxSlider_Brushincrement = SliderObject,
                                    MUIA_Numeric_Min, 1,
                                    MUIA_Numeric_Max, 100,
                                    MUIA_Numeric_Value, 100,
                                End ),
                            End,
                        End,
                    End,
                    Child, ( IPTR )RectangleObject,
                        MUIA_Rectangle_HBar, TRUE,
                    End,
                    Child, ( IPTR )VGroup,
                        Child, ( IPTR )( brushOpGroup = GroupObject,
                            MUIA_Group_Horiz, TRUE,
                            MUIA_Group_HorizSpacing, 0,
                            MUIA_Frame, MUIV_Frame_None,
                            MUIA_Group_SameSize, TRUE,
                            Child, ( IPTR )VGroup,
                                MUIA_Weight, 50,
                                Child, ( IPTR )TextObject,
                                    MUIA_Text_Contents, __(MSG_TOOLBOX_PAINT),
                                End,
                                Child, ( IPTR )( tbxCycPaintMode = CycleObject,
                                    MUIA_Cycle_Entries, arr_drawmodes,
                                End ),
                                Child, ( IPTR )HGroup,
                                    Child, ( IPTR )TextObject,
                                        MUIA_Weight, 50,
                                        MUIA_Text_Contents, __(MSG_TOOLBOX_DRAW),
                                    End,
                                    Child, ( IPTR )TextObject,
                                        MUIA_Weight, 50,
                                        MUIA_Text_Contents, __(MSG_TOOLBOX_ALPHA),
                                    End,
                                End,
                                Child, ( IPTR )HGroup,
                                    Child, ( IPTR )( tbxCycFillmode = CycleObject,
                                        MUIA_Cycle_Entries, arr_fillmode,
                                        MUIA_Cycle_Active, 0,
                                        MUIA_Weight, 50,
                                    End ),
                                    Child, ( IPTR )( tbxCycColorCtrl = CycleObject,
                                        MUIA_Cycle_Entries, arr_colorctrl,
                                        MUIA_Cycle_Active, 0,
                                        MUIA_Weight, 50,
                                    End ),
                                End,
                            End,
                            Child, ( IPTR )VGroup,
                                Child, ( IPTR )VGroup,
                                    Child, ( IPTR )RectangleObject,
                                        MUIA_FixWidth, 4,
                                    End,
                                End,
                            End,
                            Child, ( IPTR )VGroup,
                                Child, ( IPTR )TextObject,
                                    MUIA_Text_Contents, __(MSG_TOOLBOX_BRUSHSH),
                                End,
                                Child, ( IPTR )HGroup,
                                    MUIA_Weight, 25,
                                    Child, ( IPTR )(
                                        antiImage = HGroup,
                                            MUIA_Frame, MUIV_Frame_Button,
                                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                                            Child, ImageObject,
                                                MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/antialias.png",
                                                MUIA_Frame, MUIV_Frame_None,
                                            End,
                                        End
                                    ),
                                    Child, ( IPTR )(
                                        antiOffImage = HGroup,
                                            MUIA_Frame, MUIV_Frame_Button,
                                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                                            Child, ImageObject,
                                                MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/antialias_off.png",
                                                MUIA_Frame, MUIV_Frame_None,
                                            End,
                                        End
                                    ),
                                End,
                                Child, ( IPTR )HGroup,
                                    MUIA_Weight, 25,
                                    Child, ( IPTR )(
                                        featherImage = HGroup,
                                            MUIA_Frame, MUIV_Frame_Button,
                                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                                            Child, ImageObject,
                                                MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/feathered.png",
                                                MUIA_Frame, MUIV_Frame_None,
                                            End,
                                        End
                                    ),
                                    Child, ( IPTR )(
                                        solidImage  = HGroup,
                                            MUIA_Frame, MUIV_Frame_Button,
                                            MUIA_InputMode, MUIV_InputMode_RelVerify,
                                            Child, ImageObject,
                                                MUIA_Image_Spec, ( IPTR )"3:PROGDIR:data/solid.png",
                                                MUIA_Frame, MUIV_Frame_None,
                                            End,
                                        End
                                    ),
                                End,
                            End,
                        End ),
                    End,
                End ),
                Child, ( IPTR )RectangleObject,
                    MUIA_Rectangle_HBar, TRUE,
                End,
                Child, ( IPTR )HGroup,
                    MUIA_Frame, MUIV_Frame_None,
                    Child, ( IPTR )( tbxCycGrid = CycleObject,
                        MUIA_Cycle_Entries, arr_grid,
                        MUIA_Weight, 60,
                    End ),
                    Child, ( IPTR )( tbxCyc_GridSize = CycleObject,
                        MUIA_Cycle_Entries, arr_grdz,
                        MUIA_Weight, 40,
                    End ),
                End,
                Child, ( IPTR )RectangleObject,
                    MUIA_Rectangle_HBar, TRUE,
                End,
                Child, ( IPTR )GroupObject,
                    Child, ( IPTR )HGroup,
                        InnerSpacing(0,0),
                        MUIA_Frame, MUIV_Frame_Group,
                        Child, ( IPTR )( tbxAreaPalette = NewObject (
                            mcc->mcc_Class, NULL,
                            MUIA_FixHeight, 96,
                            TAG_DONE
                        ) ),
                    End,
                    Child, ( IPTR )( tbxCyc_PaletteSnap = CycleObject,
                        MUIA_Cycle_Entries, &arr_pltsnap,
                    End ),
                End,
            End,
        End,
    End;

    // Move to left edge of screen (todo: improve this!)
//    int toolboxWidth = 0; get ( toolbox, MUIA_Window_Width, &toolboxWidth );
//    set ( toolbox, MUIA_Window_LeftEdge, ( lunaPubScreen->Width - toolboxWidth ) );

    // set buttons
    set ( antiImage, MUIA_Background, MUII_FILL );
    set ( featherImage, MUIA_Background, MUII_FILL );

    // Set initial gridsize
    set ( tbxCyc_GridSize, MUIA_Cycle_Active, 8 );
    globalCurrentGrid = 10;
}

void Exit_ToolboxWindow ( )
{
    if ( PreviewRectData != NULL )
        FreeVec ( PreviewRectData );
}

void Init_ToolboxMethods ( )
{
    // Set up our hooks
    getMenu_hook.h_Entry = ( HOOKFUNC )&getMenu_func;
    brushOptions_hook.h_Entry = ( HOOKFUNC )&brushOptions_func;
    getFill_hook.h_Entry = ( HOOKFUNC )&getFill_func;
    getOpacMode_hook.h_Entry = ( HOOKFUNC )&getOpacMode_func;

    // Methods
    DoMethod (
        antiImage, MUIM_Notify, MUIA_Pressed, TRUE,
        antiImage, 3, MUIM_CallHook, &brushOptions_hook, SET_ANTIALIAS
    );
    DoMethod (
        antiOffImage, MUIM_Notify, MUIA_Pressed, TRUE,
        antiOffImage, 3, MUIM_CallHook, &brushOptions_hook, SET_ANTIALIASOFF
    );
    DoMethod (
        featherImage, MUIM_Notify, MUIA_Pressed, FALSE,
        featherImage, 3, MUIM_CallHook, &brushOptions_hook, SET_FEATHERED
    );
    DoMethod (
        solidImage, MUIM_Notify, MUIA_Pressed, FALSE,
        solidImage, 3, MUIM_CallHook, &brushOptions_hook, SET_FEATHEREDOFF
    );
    DoMethod (
        tbxCycColorCtrl, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
        tbxCycColorCtrl, 2, MUIM_CallHook, &getOpacMode_hook
    );

    /*
        Draw tools and bubble help
    */
    DoMethod (
        tbxBtn_draw, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Draw
    );
    DoMethod ( tbxBtn_draw, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_DRAWFILL) );

    DoMethod (
        tbxBtn_fill, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Fill
    );
    DoMethod ( tbxBtn_fill, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_FLOOD) );

    DoMethod (
        tbxBtn_line, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Line
    );
    DoMethod ( tbxBtn_line, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_LINE) );

    DoMethod (
        tbxBtn_rectangle, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Rectangle
    );
    DoMethod ( tbxBtn_rectangle, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_RECTANGLE) );

    DoMethod (
        tbxBtn_circle, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Circle
    );
    DoMethod ( tbxBtn_circle, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_CIRCLE) );

    DoMethod (
        tbxBtn_getbrush, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_ClipBrush
    );
    DoMethod ( tbxBtn_getbrush, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_CUT) );

    DoMethod (
        tbxBtn_pickcolor, MUIM_Notify,
        MUIA_Pressed, FALSE,
        paletteRect, 1, MUIM_Luna_SetTool_Colorpicker
    );
    DoMethod ( tbxBtn_pickcolor, MUIM_Set, MUIA_ShortHelp, _(MSG_TOOLBOX_PICK) );

    /*
        Other
    */
    DoMethod (
        tbxSlider_Brushdiameter, MUIM_Notify,
        MUIA_Numeric_Value, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_AlterBrushShape
    );
    DoMethod (
        tbxSlider_Brushopacity, MUIM_Notify,
        MUIA_Numeric_Value, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_AlterBrushStrength
    );
    DoMethod (
        tbxSlider_Brushincrement, MUIM_Notify,
        MUIA_Numeric_Value, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_AlterBrushStrength
    );
    DoMethod (
        tbxSlider_Brushstep, MUIM_Notify,
        MUIA_Numeric_Value, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_AlterBrushStep
    );

    DoMethod (
        offsetWindowOk, MUIM_Notify, MUIA_Pressed, FALSE,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_EffectOffset
    );

    DoMethod (
        tbxCyc_PaletteSnap, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_SetColorMode
    );

    // When you access the menu
    DoMethod (
        PaintApp, MUIM_Notify, MUIA_Application_MenuAction, MUIV_EveryTime,
        ( IPTR )toolbox, 3, MUIM_CallHook, ( IPTR )&getMenu_hook, MUIV_EveryTime
    );

    DoMethod (
        tbxCycPaintMode, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_SetPaintMode
    );
    DoMethod (
        tbxCycGrid, MUIM_Notify,
        MUIA_Cycle_Active, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_SetGlobalGrid
    );
    DoMethod (
        tbxCyc_GridSize, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
        tbxAreaPalette, 1, MUIM_Luna_Canvas_SetGridSize
    );

    // Offset window
    DoMethod (
        offsetWindowCancel, MUIM_Notify, MUIA_Pressed, FALSE,
        offsetWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
    DoMethod (
        offsetWindow, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        offsetWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );

    // Fill state
    DoMethod (
        tbxCycFillmode, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
        tbxCycFillmode, 2, MUIM_CallHook, &getFill_hook
    );


    // Set default brush options and update the brush
    brushTool.antialias = 1;
    brushTool.feather = 1;
    brushTool.width = 1;
    brushTool.height = 1;
    brushTool.opacity = 255;
    brushTool.step = 1;
    brushTool.power = 100;
}

void checkMenuEvents ( int udata )
{
    switch ( udata )
    {
        case 597:
            // Load
            if ( 1 )
            {
                char *filename = getFilename ( );
                if ( filename != NULL )
                    LoadProject ( filename, FALSE );
                if ( filename != NULL ) FreeVec ( filename );
            }
            break;

        case 594:
            // Load datatype image
            if ( loadDatatypeImage ( ) )
            {
                setActiveBuffer ( globalActiveCanvas );
                globalActiveCanvas->winHasChanged = TRUE;
                set ( globalActiveWindow->win, MUIA_Window_Open, TRUE );
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw, NULL );
            }
            break;

        case 595:
            // Revert
            if ( globalActiveCanvas != NULL && globalActiveWindow != NULL )
            {
                int currentFrame = globalActiveCanvas->currentFrame;
                int currentLayer = globalActiveCanvas->currentLayer;

                if ( globalActiveWindow->filename == NULL )
                    globalActiveWindow->filename = getFilename ( );
                if ( globalActiveWindow->filename != NULL )
                    LoadProject ( globalActiveWindow->filename, TRUE );
                else
                    return;

                if ( globalActiveCanvas->totalFrames <= currentFrame )
                    currentFrame = globalActiveCanvas->totalFrames - 1;
                globalActiveCanvas->currentFrame = currentFrame;
                if ( globalActiveCanvas->totalLayers <= currentLayer )
                    currentLayer = globalActiveCanvas->totalLayers - 1;
                globalActiveCanvas->currentLayer = currentLayer;

                setActiveBuffer ( globalActiveCanvas );
                globalActiveCanvas->winHasChanged = TRUE;

                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
            }
            break;

        case 598:
            // Save
            if ( globalActiveWindow != NULL )
            {
                if ( globalActiveWindow->filename == NULL )
                    globalActiveWindow->filename = getFilename ( );
                if ( globalActiveWindow->filename != NULL )
                    SaveProject ( globalActiveWindow );
            }
            break;

        case 596:
            // Save AS
            if ( globalActiveWindow != NULL )
            {
                char *newFilename = getFilename ( );
                if ( newFilename != NULL )
                {
                    if ( globalActiveWindow->filename != NULL )
                        FreeVec ( globalActiveWindow->filename );
                    globalActiveWindow->filename = newFilename;
                }
                checkMenuEvents ( 598 );
            }
            break;

        case 599:
            set ( nwWindow, MUIA_Window_Open, TRUE );
            keyboardEnabled = FALSE;
            break;

        case 600:
            if ( globalActiveCanvas != NULL )
                set ( importWindow, MUIA_Window_Open, TRUE );
            break;

        /* Export */
        case 601:
            if ( globalActiveCanvas != NULL )
            {
                set ( exportWindow, MUIA_Window_Open, TRUE );
            }
            break;

        case 602:
            set ( aboutWindow, MUIA_Window_Open, TRUE );
            break;

        case 604:
            if ( globalActiveWindow != NULL )
            {
                set ( globalActiveWindow->projectWin, MUIA_Window_Open, TRUE );
            }
            break;

        case 700:
            DoMethod (
                PaintApp, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
            );
            break;

        case 730:
            DoMethod ( globalActiveWindow->area, MUIM_Luna_ZoomIn );
            break;

        case 731:
            DoMethod ( globalActiveWindow->area, MUIM_Luna_ZoomOut );
            break;

        case 732:
            DoMethod ( globalActiveWindow->area, MUIM_Luna_ShowAll );
            break;

        case 740:
            if ( globalActiveCanvas != NULL )
            {
                copyToSwap ( globalActiveCanvas );
            }
            break;

        case 741:
            if ( globalActiveCanvas != NULL )
            {
                swapCanvasBuffers ( globalActiveCanvas );
                globalActiveWindow->rRectW = 0;
                globalActiveWindow->rRectX = 0;
                globalActiveWindow->rRectH = 0;
                globalActiveWindow->rRectY = 0;
                globalActiveCanvas->winHasChanged = TRUE;
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
            }
            break;

        case 742:
            if ( globalActiveWindow != NULL )
            {
                set ( offsetWindow, MUIA_Window_Open, TRUE );
            }
            break;

        case 743:
            if ( globalActiveCanvas != NULL )
            {
                effectFlipVert ( globalActiveCanvas );
                globalActiveCanvas->winHasChanged = TRUE;
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
            }
            break;

        case 744:
            if ( globalActiveCanvas != NULL )
            {
                effectFlipHoriz ( globalActiveCanvas );
                globalActiveCanvas->winHasChanged = TRUE;
                DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_Redraw );
            }
            break;

        case 745:
            if ( globalActiveCanvas != NULL )
            {
                DoMethod ( tbxAreaPalette, MUIM_Luna_Canvas_ClearActiveCanvas );
            }
            break;

        // next palette color
        case 746:
            nextPaletteColor ( );
            break;

        // prev palette color
        case 747:
            prevPaletteColor ( );
            break;

        // Flip brush vert
        case 748:
            flipBrushVert ( );
            break;

        // Flip brush vert
        case 749:
            flipBrushHoriz ( );
            break;

        // Rotate brush 90 degrees
        case 751:
            rotateBrush90 ( );
            break;

        // Palette editor
        case 750:
            set ( paletteWindow, MUIA_Window_Open, TRUE );
            break;

        // The text to brush window
        case 755:
            set ( textToBrushWindow, MUIA_Window_Open, TRUE );
            break;

        // Paint modes

        case 800:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 0 );
            break;

        case 801:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 6 );
            break;

        case 802:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 7 );
            break;

        case 807:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 8 );
            break;

        case 803:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 3 );
            break;

        case 804:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 4 );
            break;

        case 805:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 5 );
            break;

        case 806:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 2 );
            break;

        case 808:
            set ( tbxCycPaintMode, MUIA_Cycle_Active, 1 );
            break;

        /* Paint tools */

        case 850:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Draw );
            break;

        case 851:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Line );
            break;

        case 852:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Fill );
            break;

        case 853:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Rectangle );
            break;

        case 855:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Circle );
            break;

        case 854:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_ClipBrush );
            break;

        case 856:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Colorpicker );
            break;

        /* Animation */

        case 820:
            DoMethod ( WidgetLayers, MUIM_Luna_Canvas_NextFrame );
            break;

        case 821:
            DoMethod ( WidgetLayers, MUIM_Luna_Canvas_PrevFrame );
            break;

        case 825:
            {
                int num = 0;
                get ( CycleOnionSkin, MUIA_Cycle_Active, &num );
                set (	CycleOnionSkin, MUIA_Cycle_Active, ( num == 0 ) ? 1 : 0 );
            }
            break;

        /* Windows */

        case 400:
            {
                BOOL opened = FALSE;
                get ( WindowAnimation, MUIA_Window_Open, &opened );
                if ( opened )
                    set ( WindowAnimation, MUIA_Window_Open, FALSE);
                else
                    set ( WindowAnimation, MUIA_Window_Open, TRUE );
            }
            break;

        case 401:
            {
                BOOL opened = FALSE;
                get ( WindowLayers, MUIA_Window_Open, &opened );
                if ( opened )
                    set ( WindowLayers, MUIA_Window_Open, FALSE );
                else
                    set ( WindowLayers, MUIA_Window_Open, TRUE );
            }
            break;

        case 402:
            {
                if ( !fullscreenEditing )
                {
                    if ( !globalActiveWindow ) return;
                    showFullscreenWindow ( globalActiveCanvas );
                }
                else
                {
                    hideFullscreenWindow ( );
                }
            }
            break;

        // Program preferences
        case 900:
            set ( PrefsWindow, MUIA_Window_Open, TRUE );
            break;

        default:
            break;
    }
}

void setToolbarActive ( )
{
    // Draw:
    if ( globalCurrentTool == LUNA_TOOL_BRUSH )
        set ( tbxBtn_draw, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_draw, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_FILL )
        set ( tbxBtn_fill, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_fill, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_LINE )
        set ( tbxBtn_line, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_line, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_RECTANGLE )
        set ( tbxBtn_rectangle, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_rectangle, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_CIRCLE )
        set ( tbxBtn_circle, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_circle, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_CLIPBRUSH )
        set ( tbxBtn_getbrush, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_getbrush, MUIA_Background, MUII_ButtonBack );

    if ( globalCurrentTool == LUNA_TOOL_COLORPICKER )
        set ( tbxBtn_pickcolor, MUIA_Background, MUII_FILL );
    else
        set ( tbxBtn_pickcolor, MUIA_Background, MUII_ButtonBack );
}

void GetDrawFillState ( )
{
    filledDrawing = XGET ( tbxCycFillmode, MUIA_Cycle_Active );
}
