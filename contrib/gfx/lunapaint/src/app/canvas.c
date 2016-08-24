/****************************************************************************
*                                                                           *
* canvas.c -- Lunapaint,                                                    *
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

#include "canvas.h"

BOOL isZooming;
BOOL isScrolling;
BOOL fullscreenEditing;
int AskMinMaxTimes;
Object *windowFullscreen;
Object *fullscreenGroup;

//---------------------------------------------------------------------

/*
    Change project info
*/

HOOKPROTONHNO(projFunc, void, int *param)
{
    // We allocate but do not deallocate as
    // this var is freed by Zune when it becomes part
    // of it
    unsigned char *projectName = NULL;

    struct WindowList *lst = *( struct WindowList **)param;

    get ( lst->projName, MUIA_String_Contents, &projectName );
    set ( lst->win, MUIA_Window_Title, projectName );
    set ( lst->projectWin, MUIA_Window_Open, FALSE );
}



/*
    Dispatcher for our RGBitmapObject
*/
BOOPSI_DISPATCHER(IPTR, RGBitmapDispatcher, cl, obj, msg)
{
    switch ( msg->MethodID )
    {
        case MUIM_Draw:                         return MUIM_RGB_Draw ( cl, obj, msg );
        case MUIM_Luna_Canvas_Redraw:           return MUIM_RGB_Redraw ( );
        case MUIM_Luna_Canvas_RedrawArea:       return MUIM_RGB_RedrawArea ( );
        case MUIM_HandleInput:                  return MUIM_RGB_HandleInput ( cl, obj, msg );
        case MUIM_Setup:                        return MUIM_RGB_Setup ( cl, obj, msg );
        case MUIM_Cleanup:                      return MUIM_RGB_Cleanup ( cl, obj, msg );
        case MUIM_Luna_Canvas_ScrollingNotify:  return MUIM_RGB_ScrollingNotify ( );
        case MUIM_Luna_CanvasActivate:          return MUIM_RGB_CanvasActivate ( cl, obj, msg );
        case MUIM_Luna_CanvasDeactivate:        return MUIM_RGB_CanvasDeactivate ( cl, obj, msg );
        case MUIM_Luna_ZoomIn:                  return MUIM_RGB_ZoomIn ( );
        case MUIM_Luna_ZoomOut:                 return MUIM_RGB_ZoomOut ( );
        case MUIM_Luna_ShowAll:                 return MUIM_RGB_ShowAll ( );
        case MUIM_AskMinMax:                    return MUIM_RGB_AskMinMax ( cl, obj, msg );
        case MUIM_Luna_CloseCanvasWin:          return MUIM_RGB_CloseCanvasWin ( cl, obj, msg );
        default:                                return DoSuperMethodA ( cl, obj, msg );
    }
    return ( IPTR )NULL;
}
BOOPSI_DISPATCHER_END


/******************************************************************************
    Public functions
******************************************************************************/

IPTR MUIM_RGB_Draw ( Class *CLASS, Object *self, Msg message )
{
    // Redrawing
    if ( globalActiveCanvas != NULL )
    {
        AskMinMaxTimes = 0;
        globalActiveWindow->prevBlit.w = 0;
        _RGBitmapRedraw ( CLASS, self );
        return ( IPTR )NULL;
    }
    return DoSuperMethodA ( CLASS, self, message );
}
IPTR MUIM_RGB_Redraw ( )
{
    if ( globalActiveCanvas )
    {
        MUI_Redraw ( globalActiveWindow->area, MADF_DRAWUPDATE );
    }
    return ( IPTR )NULL;
}

// Called from outside the object
IPTR MUIM_RGB_RedrawArea ( )
{
    if ( globalActiveCanvas != NULL )
    {
        globalActiveCanvas->winHasChanged = TRUE;
        if ( redrawTimes < 5 )
            redrawTimes++;
    }
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_HandleInput ( Class *CLASS, Object *self, Msg message )
{
    if ( CLASS == NULL || isZooming || globalActiveCanvas == NULL )
        return ( IPTR )NULL;

    struct MUIP_HandleInput *Message = ( struct MUIP_HandleInput *)message;

    struct RGBitmapData *data = ( struct RGBitmapData *)INST_DATA ( CLASS, self );
    char movement = 0;

    // Immediately set this window as active window!
    if ( globalActiveCanvas != data->window->canvas )
    {
        globalActiveCanvas = data->window->canvas;
        globalActiveWindow = data->window;
    }

    // Get current zoom in a simpler to write variable
    int zoom = globalActiveCanvas->zoom;

    // Get correct offset coordinates
    // and align them to zoom "grid"
    int ox = XGET ( globalActiveWindow->scrollH, MUIA_Prop_First );
    int oy = XGET ( globalActiveWindow->scrollV, MUIA_Prop_First );
    ox = ( int )( ( double )ox / zoom ) * zoom;
    oy = ( int )( ( double )oy / zoom ) * zoom;

    // If the offset has changed (with a window resize etc) or we got a scroll event
    // notification
    if (
        ( ox != globalActiveCanvas->offsetx || oy != globalActiveCanvas->offsety ) &&
        isScrolling
    )
    {
        ScrollCanvas ( ox, oy );
        isScrolling = FALSE;
    }

    // Get dimensions
    ULONG areaWidth = _getAreaWidth ( );

    // Make sure we have a message
    if ( Message->imsg )
    {
        switch ( Message->imsg->Class )
        {
            case IDCMP_MOUSEMOVE:

                if ( !data->window->isActive )
                {
                    data->window->isActive = TRUE;
                    break;
                }

                // Signalize that we have movement
                movement = 1;
                MouseHasMoved = TRUE;

                // Get the mose coordinates relative to top and left widget edges
                data->zuneAreaLeft = XGET ( self, MUIA_LeftEdge );
                data->zuneAreaTop = XGET ( self, MUIA_TopEdge );

                // Update window edge coordinates
                data->window->canvas->winEdgeWidth = data->zuneAreaLeft;
                data->window->canvas->winEdgeHeight = data->zuneAreaTop;

                // Update coordinates on display
                STRPTR coord = AllocVec ( 20, MEMF_CLEAR );
                sprintf ( coord, "X: %d ", ( int )cMouseX );
                set ( data->window->txtCoordX, MUIA_Text_Contents, ( IPTR )coord );
                FreeVec ( coord );
                coord = AllocVec ( 20, MEMF_CLEAR );
                sprintf ( coord, "Y: %d ", ( int )cMouseY );
                set ( data->window->txtCoordY, MUIA_Text_Contents, ( IPTR )coord );
                FreeVec ( coord );


            case IDCMP_MOUSEBUTTONS:

                // Signalize that we have movement
                movement = 1;

                // And that the window has changed
                data->window->canvas->winHasChanged = TRUE;

                if ( Message->imsg->Code == SELECTDOWN )
                {
                    if ( cMouseX >= ox || cMouseX < ox + areaWidth || cMouseY >= oy || cMouseY < oy + areaWidth )
                    {
                        // Register the click!
                        if ( !MouseButtonL ) MouseHasMoved = TRUE;
                        MouseButtonL = TRUE;
                        // Paintbrush tool filled
                        if ( globalCurrentTool == LUNA_TOOL_BRUSH ) brushTool.RecordContour = TRUE;
                    }
                }

                // Inactivizers happen everywhere
                if ( Message->imsg->Code == SELECTUP )
                {
                    MouseHasMoved = TRUE;
                    MouseButtonL = FALSE;
                    if ( globalCurrentTool == LUNA_TOOL_BRUSH ) brushTool.RecordContour = FALSE;
                    globalActiveWindow->layersChg = TRUE;
                    MUI_Redraw ( WidgetLayers, MADF_DRAWUPDATE );
                }
                break;
        }

    }
    return 0;
}

IPTR MUIM_RGB_Setup ( Class *CLASS, Object *self, Msg message )
{
    AskMinMaxTimes = 0;
    isScrolling = FALSE;
    isZooming = FALSE;
    MUI_RequestIDCMP( self, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY );
    return DoSuperMethodA ( CLASS, self, message );
}

IPTR MUIM_RGB_Cleanup ( Class *CLASS, Object *self, Msg message )
{
    MUI_RejectIDCMP( self, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY );
    return DoSuperMethodA ( CLASS, self, message );
}

IPTR MUIM_RGB_ScrollingNotify ( )
{
    isScrolling = TRUE;
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_CanvasActivate ( Class *CLASS, Object *self, Msg message )
{
    struct RGBitmapData *data = INST_DATA ( CLASS, self );

    // We can use keyboard shortcuts
    keyboardEnabled = TRUE;

    // Quicky update these two globals
    globalActiveWindow = data->window;
    globalActiveCanvas = data->window->canvas;

    // Yeah =)
    AskMinMaxTimes = 0;

    // Set this
    Update_AnimValues ( );

    // Set window is unactive to force an up
    data->window->isActive = TRUE;

    // Redraw first time
    MUI_Redraw ( WidgetLayers, MADF_DRAWUPDATE );

    return ( IPTR )NULL;
}

IPTR MUIM_RGB_CanvasDeactivate ( Class *CLASS, Object *self, Msg message )
{
    if ( globalActiveWindow != NULL && !fullscreenEditing )
    {
        struct RGBitmapData *data = INST_DATA ( CLASS, self );
        data->window->isActive = FALSE;
    }
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_ZoomIn ( )
{
    if ( globalActiveCanvas->zoom + 1 <= 32 && !isZooming )
    {
        isZooming = TRUE;
        AskMinMaxTimes = 2;

        // Remove any previous tool preview
        removePrevToolPreview ( );

        // Move scrollbars
        int ox = globalActiveCanvas->offsetx;
        int oy = globalActiveCanvas->offsety;

        // Set new zoom
        globalActiveCanvas->zoom++;

        // Snap to zoom
        globalActiveCanvas->offsetx = ( ox * 2 );
        globalActiveCanvas->offsety = ( oy * 2 );
        constrainOffset ( globalActiveCanvas );

        // Update GUI
        winHasChanged ( );
        isZooming = FALSE;
    }
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_ZoomOut ( )
{
    if ( globalActiveCanvas->zoom - 1 >= 1 && !isZooming )
    {
        isZooming = TRUE;
        AskMinMaxTimes = 2;

        // Remove any previous tool preview
        removePrevToolPreview ( );

        // Set new zoom
        globalActiveCanvas->zoom--;

        // Move scrollbars
        globalActiveCanvas->offsetx = globalActiveCanvas->offsetx * 0.5;
        globalActiveCanvas->offsety = globalActiveCanvas->offsety * 0.5;
        constrainOffset ( globalActiveCanvas );

        // Update GUI
        winHasChanged ( );
        isZooming = FALSE;
    }
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_ShowAll ( )
{
    // Remove any previous tool preview
    isZooming = TRUE;
    removePrevToolPreview ( );
    AskMinMaxTimes = 2;
    globalActiveCanvas->zoom = 1;
    globalActiveCanvas->offsetx = 0;
    globalActiveCanvas->offsety = 0;
    winHasChanged ( );
    isZooming = FALSE;
    return ( IPTR )NULL;
}

IPTR MUIM_RGB_AskMinMax ( Class *CLASS, Object *self, Msg message )
{
    DoSuperMethodA ( CLASS, self, message );
    struct RGBitmapData *data = INST_DATA ( CLASS, self );
    struct MUIP_AskMinMax *Message = ( struct MUIP_AskMinMax *)message;

    // This one is needed when askminmax is asked with reopening windows that
    // have been _hidden_ (not disposed), like when going into fullscreen mode
    if ( !globalActiveWindow )
    {
        globalActiveWindow = data->window;
        globalActiveCanvas = data->window->canvas;
    }

    int calcwidth = data->window->canvas->width * data->window->canvas->zoom;
    int calcheight = data->window->canvas->height * data->window->canvas->zoom;

    int tWidth = _getAreaWidth ( ),
        tHeight = _getAreaHeight ( ),
        minWidth = calcwidth,
        minHeight = calcheight;

    if ( minWidth > tWidth ) minWidth = tWidth;
    if ( minHeight > tHeight ) minHeight = tHeight;

    if ( AskMinMaxTimes > 0 )
    {
        // Dunno why I need to add the lunaPubScreen size or run this the second
        // time askminmax is executed
        Message->MinMaxInfo->MinWidth += minWidth;
        Message->MinMaxInfo->MinHeight += minHeight;
        AskMinMaxTimes--;
    }

    Message->MinMaxInfo->DefWidth += calcwidth;
    Message->MinMaxInfo->DefHeight += calcheight;
    Message->MinMaxInfo->MaxWidth += lunaPubScreen->Width;
    Message->MinMaxInfo->MaxHeight += lunaPubScreen->Height;

    return ( IPTR )NULL;
}

IPTR MUIM_RGB_CloseCanvasWin ( Class *CLASS, Object *self, Msg message )
{
    if ( !fullscreenEditing )
    {
        removeActiveWindow ( CLASS, self );
        layerRenderBlank ( );
    }
    return ( IPTR )NULL;
}


void checkKeyboardShortcuts ( UWORD valu )
{
    switch ( valu )
    {
        case RAWKEY_A:
            if ( brushTool.antialias == FALSE )
            {
                DoMethod ( antiOffImage, MUIM_CallHook, &brushOptions_hook, SET_ANTIALIAS );
            }
            else
            {
                DoMethod ( antiOffImage, MUIM_CallHook, &brushOptions_hook, SET_ANTIALIASOFF );
            }
            makeToolBrush ( );
            break;
        case RAWKEY_SPACE:
            {
                int VAL = ( int )XGET( tbxCyc_PaletteSnap, MUIA_Cycle_Active );
                if ( VAL == 0 )
                    set ( tbxCyc_PaletteSnap, MUIA_Cycle_Active, 1 );
                else set ( tbxCyc_PaletteSnap, MUIA_Cycle_Active, 0 );
            }
            break;
        case RAWKEY_F:
            globalCurrentTool = LUNA_TOOL_FILL;
            setToolbarActive ( );
            break;
        case RAWKEY_D:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Draw );
            break;
        case RAWKEY_L:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Line );
            break;
        case RAWKEY_E:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Circle );
            break;
        case RAWKEY_R:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Rectangle );
            break;
        case RAWKEY_O:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_Colorpicker );
            break;
        case RAWKEY_B:
            DoMethod ( paletteRect, MUIM_Luna_SetTool_ClipBrush );
            break;
        case RAWKEY_G:
            {
                ULONG curr = XGET ( tbxCycGrid, MUIA_Cycle_Active );
                if ( curr == 0 )
                    set ( tbxCycGrid, MUIA_Cycle_Active, 1 );
                else
                    set ( tbxCycGrid, MUIA_Cycle_Active, 0 );
            }
            break;
        case RAWKEY_J:
            if ( !globalActiveCanvas ) return;
            swapCanvasBuffers ( globalActiveCanvas );
            globalActiveWindow->rRectW = 0;
            globalActiveWindow->rRectX = 0;
            globalActiveWindow->rRectH = 0;
            globalActiveWindow->rRectY = 0;
            globalActiveCanvas->winHasChanged = TRUE;
            globalActiveWindow->layersChg = TRUE;
            MUI_Redraw ( globalActiveWindow->area, MADF_DRAWUPDATE );
            break;
        case RAWKEY_J | RAWKEY_RSHIFT:
            if ( !globalActiveCanvas ) return;
            copyToSwap ( globalActiveCanvas );
            break;
        case RAWKEY_Y:
            flipBrushVert ( );
            break;
        case RAWKEY_X:
            flipBrushHoriz ( );
            break;
        case RAWKEY_Z:
            rotateBrush90 ( );
            break;
        case RAWKEY_PERIOD:
            nextPaletteColor ( );
            break;
        case RAWKEY_COMMA:
            prevPaletteColor ( );
            break;
        case RAWKEY_F1: set ( tbxCycPaintMode, MUIA_Cycle_Active, 0 ); break;
        case RAWKEY_F2: set ( tbxCycPaintMode, MUIA_Cycle_Active, 1 ); break;
        case RAWKEY_F3: set ( tbxCycPaintMode, MUIA_Cycle_Active, 6 ); break;
        case RAWKEY_F4: set ( tbxCycPaintMode, MUIA_Cycle_Active, 4 ); break;
        case RAWKEY_F5: set ( tbxCycPaintMode, MUIA_Cycle_Active, 5 ); break;
        case RAWKEY_F6: set ( tbxCycPaintMode, MUIA_Cycle_Active, 7 ); break;
        case RAWKEY_F7: set ( tbxCycPaintMode, MUIA_Cycle_Active, 8 ); break;
        case RAWKEY_F8: set ( tbxCycPaintMode, MUIA_Cycle_Active, 2 ); break;
        case RAWKEY_F9: set ( tbxCycPaintMode, MUIA_Cycle_Active, 3 ); break;

        // Toggle fullscreen mode
        case RAWKEY_F11:
            if ( !globalActiveCanvas ) return;
            if ( !fullscreenEditing )
            {
                if ( !globalActiveWindow ) return;
                showFullscreenWindow ( globalActiveCanvas );
            }
            else  hideFullscreenWindow ( );
            break;

        // Toggle toolbox
        case RAWKEY_F12:
            {
                // Don't hide the toolbox if we're not in fullscreen and we're not having a canvas window
                if ( GlobalPrefs.ScreenmodeType == 0 && !fullscreenEditing && !globalActiveWindow ) break;

                BOOL boxopen = XGET ( toolbox, MUIA_Window_Open );
                if ( boxopen ) set ( toolbox, MUIA_Window_Open, FALSE );
                else set ( toolbox, MUIA_Window_Open, TRUE );
            }
            break;

        case RAWKEY_UP: moveScrollbarUp (  ); break;
        case RAWKEY_DOWN: moveScrollbarDown (  ); break;
        case RAWKEY_LEFT: moveScrollbarLeft (  ); break;
        case RAWKEY_RIGHT: moveScrollbarRight (  ); break;

        // Opacity values
        case RAWKEY_KP_1: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 5 ); break;
        case RAWKEY_KP_2: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 10 ); break;
        case RAWKEY_KP_3: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 30 ); break;
        case RAWKEY_KP_4: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 60 ); break;
        case RAWKEY_KP_5: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 128 ); break;
        case RAWKEY_KP_6: set ( tbxSlider_Brushopacity, MUIA_Numeric_Value, 255 ); break;

        // Brush sizes
        case RAWKEY_KP_PLUS:
            {
                int bsz = XGET ( tbxSlider_Brushdiameter, MUIA_Numeric_Value );
                set ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, bsz + 2 );
                break;
            }
        case RAWKEY_MINUS:
            {
                int bsz = XGET ( tbxSlider_Brushdiameter, MUIA_Numeric_Value );
                set ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, bsz - 2 );
                break;
            }
        case RAWKEY_KP_DECIMAL: set ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, 1 ); break;
        case RAWKEY_KP_ENTER: set ( tbxSlider_Brushdiameter, MUIA_Numeric_Value, 100 ); break;

        default: break;
    }
    lastKeyPressed = valu;
}

void moveScrollbarUp ( )
{
    if ( globalActiveWindow == NULL ) return;
    int pos = XGET ( globalActiveWindow->scrollV, MUIA_Prop_First );
    int dist = globalActiveCanvas->zoom * 10;
    set ( globalActiveWindow->scrollV, MUIA_Prop_First, pos - dist );
}

void moveScrollbarDown ( )
{
    if ( globalActiveWindow == NULL ) return;
    int pos = XGET ( globalActiveWindow->scrollV, MUIA_Prop_First );
    int dist = globalActiveCanvas->zoom * 10;
    set ( globalActiveWindow->scrollV, MUIA_Prop_First, pos + dist );
}

void moveScrollbarLeft ( )
{
    if ( globalActiveWindow == NULL ) return;
    int pos = XGET ( globalActiveWindow->scrollH, MUIA_Prop_First );
    int dist = globalActiveCanvas->zoom * 10;
    set ( globalActiveWindow->scrollH, MUIA_Prop_First, pos - dist );
}

void moveScrollbarRight ( )
{
    if ( globalActiveWindow == NULL ) return;
    int pos = XGET ( globalActiveWindow->scrollH, MUIA_Prop_First );
    int dist = globalActiveCanvas->zoom * 10;
    set ( globalActiveWindow->scrollH, MUIA_Prop_First, pos + dist );
}

void constrainOffset ( struct oCanvas *canvas )
{
    // Get some vars
    int zoom = canvas->zoom;

    int vWidth = _getAreaWidth ( ),
        vHeight = _getAreaHeight ( );

    vWidth = ( double )vWidth / zoom;
    vHeight = ( double )vHeight / zoom;

    int cWidth = canvas->width;
    int cHeight = canvas->height;

    int ox = ( double )canvas->offsetx / zoom;
    int oy = ( double )canvas->offsety / zoom;

    // Bounds
    if ( ox < 0 ) canvas->offsetx = 0;
    else if ( ox + vWidth >= cWidth )
        canvas->offsetx = ( cWidth - vWidth ) * zoom;
    if ( oy < 0 ) canvas->offsety = 0;
    else if ( oy + vHeight >= cHeight )
        canvas->offsety = ( cHeight - vHeight ) * zoom;

    // Snap to zoom
    canvas->offsetx = ( int )( ( double )canvas->offsetx / zoom ) * zoom;
    canvas->offsety = ( int )( ( double )canvas->offsety / zoom ) * zoom;
}

void snapToBounds ( int *x, int *y )
{
    // Get some vars
    int zoom = globalActiveCanvas->zoom;

    int vWidth = _getAreaWidth ( ),
        vHeight = _getAreaHeight ( );

    int cWidth = globalActiveCanvas->width * zoom;
    int cHeight = globalActiveCanvas->height * zoom;

    // Adapt x/y in bounds
    if ( *x < 0 ) *x = 0;
    else if ( *x + vWidth >= cWidth )
        *x = ( int )( ( cWidth - vWidth ) / zoom ) * zoom;
    if ( *y < 0 ) *y = 0;
    else if ( *y + vHeight >= cHeight )
        *y = ( int )( ( cHeight - vHeight ) / zoom ) * zoom;
}

void winHasChanged ( )
{
    globalActiveCanvas->winHasChanged = TRUE;
    DoMethod ( globalActiveWindow->area, MUIM_Draw );
}

IPTR _RGBitmapRedraw ( Class *CLASS, Object *self )
{
    // Get stored data
    struct RGBitmapData *data = INST_DATA ( CLASS, self );

    if ( globalActiveCanvas == data->window->canvas )
    {
        // Get image dimensions multiplied by zoom
        ULONG imageWidth = data->window->canvas->width * data->window->canvas->zoom;
        ULONG imageHeight = data->window->canvas->height * data->window->canvas->zoom;

        // Check if the dimensions of the visible width has changed
        ULONG ZuneWidth = XGET ( data->window->area, MUIA_Width );
        ULONG ZuneHeight = XGET ( data->window->area, MUIA_Height );
        if ( data->zuneAreaWidth != ZuneWidth || data->zuneAreaHeight != ZuneHeight )
            data->window->canvas->winHasChanged = TRUE;

        // Get top and left edge of area
        data->zuneAreaTop = XGET ( data->window->area, MUIA_TopEdge );
        data->zuneAreaLeft = XGET ( data->window->area, MUIA_LeftEdge );
        data->zuneAreaWidth = ZuneWidth;
        data->zuneAreaHeight = ZuneHeight;
        data->rgbAreaTop = _getAreaOffsetY ( ) + data->zuneAreaTop;
        data->rgbAreaLeft = _getAreaOffsetX ( ) + data->zuneAreaLeft;

        // Update known container size
        // This needs cleaning up
        int cvisWidth = _getAreaWidth ( ),
            cvisHeight = _getAreaHeight ( );
        data->window->contWidth = cvisWidth;
        data->window->contHeight = cvisHeight;
        data->window->canvas->visibleWidth = cvisWidth;
        data->window->canvas->visibleHeight = cvisHeight;

        // we're only using a maximum area of the container for
        // the drawing itself
        if ( cvisWidth > imageWidth ) cvisWidth = imageWidth;
        if ( cvisHeight > imageHeight ) cvisHeight = imageHeight;

        // Update window edge coordinates (do we need this? find out)
        data->window->canvas->winEdgeWidth = _getAreaLeft ( );
        data->window->canvas->winEdgeHeight = _getAreaTop ( );


        // Update scrollbars if we're changing zoom
        if (
            globalActiveCanvas->zoom != data->currentzoom ||
            data->window->canvas->winHasChanged
        )
        {
            // Set scrollbars
            set ( data->window->scrollH, MUIA_Prop_Visible, ( IPTR )cvisWidth );
            set ( data->window->scrollV, MUIA_Prop_Visible, ( IPTR )cvisHeight );
            set ( data->window->scrollH, MUIA_Prop_Entries, ( IPTR )imageWidth );
            set ( data->window->scrollV, MUIA_Prop_Entries, ( IPTR )imageHeight );
            set ( data->window->scrollH, MUIA_Prop_First, ( IPTR )globalActiveCanvas->offsetx );
            set ( data->window->scrollV, MUIA_Prop_First, ( IPTR )globalActiveCanvas->offsety );

            // Set dimensions
            data->currentzoom = globalActiveCanvas->zoom;
            data->rgbAreaWidth = cvisWidth;
            data->rgbAreaHeight = cvisHeight;

            // In case triggered by zoom
            data->window->canvas->winHasChanged = TRUE;
        }

        // Update the screen buffer
        if ( data->window->canvas->winHasChanged )
        {
            int Tool = globalCurrentTool;   // <- disables drawing tool preview in rendering func
            globalCurrentTool = -1;         //

            // Accelerated redraw
            if ( data->window->rRectW || data->window->rRectH )
            {
                struct oCanvas *canvas = data->window->canvas;
                int offsetx 	= ( canvas->offsetx + data->window->rRectX ) / canvas->zoom;
                int offsety 	= ( canvas->offsety + data->window->rRectY ) / canvas->zoom;
                int width 		= ( double )data->window->rRectW / canvas->zoom;
                int height 		= ( double )data->window->rRectH / canvas->zoom;

                D(bug("1a. Redrawing accelerated %dx%d\n", width, height));

                blitAreaRect (
                    offsetx, offsety,
                    width, height,
                    canvas, _rp ( data->window->area )
                );

                // Reset accelerator parameters
                data->window->rRectX = 0;
                data->window->rRectY = 0;
                data->window->rRectW = 0;
                data->window->rRectH = 0;

                D(bug("1b. Redraw complete\n"));
            }
            // Slow redraw (with zoom etc)
            else
            {
                FillPixelArray (
                    _rp ( data->window->area ),
                    data->zuneAreaLeft, data->zuneAreaTop,
                    data->zuneAreaWidth,
                    data->zuneAreaHeight,
                    0x333333
                );
                D(bug("2a. Redrawing anew %dx%d\n", cvisWidth, cvisHeight));
                if ( redrawScreenbuffer ( data->window->canvas ) )
                {
                    WritePixelArray (
                        data->window->canvas->screenbuffer,
                        0, 0, data->window->canvas->scrStorageWidth * 4, _rp ( data->window->area ),
                        data->rgbAreaLeft, data->rgbAreaTop, cvisWidth, cvisHeight, RECTFMT_RAW
                    );
                }
                D(bug("2b. Redraw complete\n"));
                UpdateCanvasInfo ( data->window );
                Update_AnimValues ( );
            }
            data->window->canvas->winHasChanged = FALSE;

            globalCurrentTool = Tool; // Resets tool setting
        }
        else
        {
            if ( data->window->canvas->screenstorage != NULL )
            {
                // Write all data to the rastport!
                FillPixelArray (
                    _rp ( data->window->area ),
                    data->zuneAreaLeft, data->zuneAreaTop,
                    data->zuneAreaWidth,
                    data->zuneAreaHeight,
                    0x333333
                );
                WritePixelArray (
                    data->window->canvas->screenstorage,
                    0, 0, data->window->canvas->scrStorageWidth * 4, _rp ( data->window->area ),
                    data->rgbAreaLeft, data->rgbAreaTop, cvisWidth, cvisHeight, RECTFMT_RAW
                );
            }
        }
        if ( globalActiveWindow->layersChg ) MUI_Redraw ( WidgetLayers, MADF_DRAWUPDATE );
    }
    // For some reason, we're updatated and not actived
    else
    {
        if ( data->window->canvas->screenstorage )
        {
            // Write all data to the rastport!
            WritePixelArray (
                data->window->canvas->screenstorage,
                0, 0, data->window->canvas->scrStorageWidth * 4,
                _rp ( data->window->area ),
                data->rgbAreaLeft, data->rgbAreaTop, data->rgbAreaWidth, data->rgbAreaHeight, RECTFMT_RAW
            );
        }
    }

    return ( IPTR )NULL;
}

void SnapOffsetToZoom ( struct oCanvas *canv )
{
    if ( canv == NULL ) canv = globalActiveCanvas;
    // Contstrain offset to zoom
    int	*ofx = &canv->offsetx, *ofy = &canv->offsety, zoom = canv->zoom;
    *ofx = ( int )( *ofx / zoom ) * zoom;
    *ofy = ( int )( *ofy / zoom ) * zoom;
}

void UpdateCanvasInfo ( struct WindowList *win )
{
    unsigned char frameText[ 32 ];
    unsigned char layerText[ 32 ];
    int frame = ( int )( win->canvas->currentFrame + 1 );
    int frams = ( int )win->canvas->totalFrames;
    int layer = ( int )( win->canvas->currentLayer + 1 );
    int layrs = ( int )win->canvas->totalLayers;
    sprintf ( ( unsigned char *)&frameText, _(MSG_CANVAS_FRAME), frame, frams );
    set ( win->txtFrameInf, MUIA_Text_Contents, ( STRPTR )&frameText );
    sprintf ( ( unsigned char *)&layerText, _(MSG_CANVAS_LAYER), layer, layrs );
    set ( win->txtLayerInf, MUIA_Text_Contents, ( STRPTR )&layerText );

    // Update zoom display
    STRPTR zlevel = AllocVec ( 20, MEMF_CLEAR );
    sprintf ( zlevel, _(MSG_CANVAS_ZOOM), ( int )win->canvas->zoom );
    set ( win->txtZoomLevel, MUIA_Text_Contents, ( IPTR )zlevel );
    FreeVec ( zlevel );
}

IPTR ScrollCanvas ( int x, int y )
{
    // Ajust to not collide with canvas bounds
    snapToBounds ( &x, &y );

    // Remove any prevous toolpreview
    removePrevToolPreview ( );

    // Get coordinates
    ULONG areaWidth =   _getAreaWidth ( );
    ULONG areaHeight =  _getAreaHeight ( );
    ULONG areaLeft =    _getAreaLeft ( ) + _getAreaOffsetX ( );
    ULONG areaTop =     _getAreaTop ( ) + _getAreaOffsetY ( );

    // find diff and align to zoom
    int diffx = x -  ( int )globalActiveCanvas->offsetx;
    int diffy = y - ( int )globalActiveCanvas->offsety;
    int hght = areaHeight - abs ( diffy );
    int wdth = areaWidth - abs ( diffx );

    // Scroll storage buffer
    if ( globalActiveCanvas->screenstorage )
    {
        removePrevToolPreview ( );
        scrollScreenStorage ( globalActiveCanvas, diffx, diffy );
        WritePixelArray (
            globalActiveCanvas->screenstorage,
            0, 0,
            globalActiveCanvas->scrStorageWidth * 4,
            _rp ( globalActiveWindow->area ),
            areaLeft, areaTop, globalActiveCanvas->scrStorageWidth, globalActiveCanvas->scrStorageHeight,
            RECTFMT_RAW
        );
    }

    // Draw emptied areas on y
    if ( abs ( diffy ) > 0 )
    {
        // If scrolling inside viewable area
        if ( hght > 0 )
        {
            globalActiveWindow->rRectW = areaWidth;
            globalActiveWindow->rRectX = 0;
            globalActiveWindow->rRectH = abs ( diffy ) ? abs ( diffy ) : areaHeight;
            globalActiveWindow->rRectY = ( diffy > 0 ) ? ( areaHeight - diffy ) : 0;
        }
        // If jumping a page
        else
        {
            globalActiveWindow->rRectW = 0;
            globalActiveWindow->rRectX = 0;
            globalActiveWindow->rRectH = 0;
            globalActiveWindow->rRectY = 0;
        }
        globalActiveCanvas->offsety = y;
        globalActiveCanvas->winHasChanged = TRUE;
        MUI_Redraw ( globalActiveWindow->area, MADF_DRAWUPDATE );
    }
    // Draw emptied areas on x
    if ( abs ( diffx ) > 0 )
    {
        // If scrolling inside viewable area
        if ( wdth > 0 )
        {
            globalActiveWindow->rRectW = abs ( diffx ) ? abs ( diffx ) : areaWidth;
            globalActiveWindow->rRectX = ( diffx > 0 ) ? ( areaWidth - diffx ) : 0;
            globalActiveWindow->rRectH = areaHeight;
            globalActiveWindow->rRectY = 0;
        }
        // Jumping a page
        else
        {
            globalActiveWindow->rRectW = 0;
            globalActiveWindow->rRectX = 0;
            globalActiveWindow->rRectH = 0;
            globalActiveWindow->rRectY = 0;
        }

        globalActiveCanvas->offsetx = x;
        globalActiveCanvas->winHasChanged = TRUE;
        MUI_Redraw ( globalActiveWindow->area, MADF_DRAWUPDATE );
    }

    return ( IPTR )NULL;
}

void getMouseCoordinates ( )
{
    if ( !globalActiveCanvas || !globalActiveWindow ) return;

    int ox = globalActiveCanvas->offsetx;
    int oy = globalActiveCanvas->offsety;

    eMouseX = (
        ( int )lunaPubScreen->MouseX -
        XGET ( globalActiveWindow->win, MUIA_Window_LeftEdge ) - _getAreaOffsetX ( ) - _getAreaLeft ( )
    );
    eMouseY = (
        ( int )lunaPubScreen->MouseY -
        XGET ( globalActiveWindow->win, MUIA_Window_TopEdge ) - _getAreaOffsetY ( ) - _getAreaTop ( )
    );

    cMouseX = ( eMouseX + ox ) / globalActiveCanvas->zoom;
    cMouseY = ( eMouseY + oy ) / globalActiveCanvas->zoom;

    if ( globalGrid )
    {
        cMouseX = ( int )( cMouseX / globalCurrentGrid ) * globalCurrentGrid;
        cMouseY = ( int )( cMouseY / globalCurrentGrid ) * globalCurrentGrid;
    }

    // Unantialiased coords..
    if ( !brushTool.antialias )
    {
        cMouseX = ( int )cMouseX;
        cMouseY = ( int )cMouseY;
    }

    if ( !MouseButtonL && !MouseButtonR ) dMouseX = cMouseX, dMouseY = cMouseY;
}

void addCanvaswindow (
    unsigned int width, unsigned int height,
    unsigned int layers, unsigned int frames,
    BOOL generateCanvas
)
{
    struct WindowList *temp = canvases; // Put current in temp
    canvases = AllocVec ( sizeof ( WindowList ), MEMF_CLEAR );

    struct MUI_CustomClass *mcc =
        MUI_CreateCustomClass (
            NULL, MUIC_Group, NULL,
            sizeof ( struct RGBitmapData ),
            RGBitmapDispatcher
        );

    canvases->win = MUI_NewObject ( MUIC_Window,
        MUIA_Window_Title, __(MSG_CANVAS_UNNAMED),
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','C'),
        MUIA_Window_MouseObject, ( IPTR )canvases->mouse,
        MUIA_Window_ScreenTitle, ( IPTR )VERSION,
        MUIA_Window_UseRightBorderScroller, TRUE,
        MUIA_Window_UseBottomBorderScroller, TRUE,
        MUIA_Window_IsSubWindow, TRUE,
        WindowContents, ( IPTR )VGroup,
            InnerSpacing( 0, 0 ),
            MUIA_Group_HorizSpacing, 0,
            MUIA_Group_VertSpacing, 0,
            Child, ( IPTR )VGroup,
                InnerSpacing( 0, 0 ),
                MUIA_Group_HorizSpacing, 0,
                MUIA_Group_VertSpacing, 0,
                Child, ( IPTR )( canvases->container = VGroup,
                    InnerSpacing( 0, 0 ),
                    MUIA_Group_HorizSpacing, 0,
                    MUIA_Group_VertSpacing, 0,
                    Child, ( IPTR )( canvases->area = NewObject(
                            mcc->mcc_Class, NULL,
                            MUIA_FillArea, FALSE,
                            MUIA_Frame, MUIV_Frame_None,
                            InnerSpacing( 0, 0 ),
                            TAG_DONE
                    ) ),
                End ),
                Child, ( IPTR )HGroup,
                    MUIA_Group_SameHeight, TRUE,
                    MUIA_Frame, MUIV_Frame_Group,
                    InnerSpacing ( 4, 4 ),
                    MUIA_Background, MUII_FILL,
                    MUIA_Group_Child, ( IPTR )HGroup,
                        MUIA_Frame, MUIV_Frame_None,
                        InnerSpacing ( 3, 3 ),
                        MUIA_Weight, 100,
                        Child, ( IPTR )( canvases->txtLayerInf = TextObject,
                            MUIA_Text_Contents, _(MSG_CANVAS_LAYER2),
                        End ),
                        Child, ( IPTR )( canvases->txtFrameInf = TextObject,
                            MUIA_Text_Contents, _(MSG_CANVAS_FRAME2),
                        End ),
                        Child, ( IPTR )( canvases->txtZoomLevel = TextObject,
                            MUIA_Text_Contents, _(MSG_CANVAS_ZOOM2),
                        End ),
                        Child, ( IPTR )( canvases->txtCoordX = TextObject,
                            MUIA_Text_Contents, "X: 0 " ,
                        End ),
                        Child, ( IPTR )( canvases->txtCoordY = TextObject,
                            MUIA_Text_Contents, "Y: 0 ",
                        End ),
                        Child, ( IPTR )RectangleObject, MUIA_Weight, 20, End,
                    End,
                    Child, ( IPTR )HGroup,
                        MUIA_InnerTop, 0,
                        MUIA_InnerLeft, 0,
                        MUIA_InnerRight, 0,
                        MUIA_InnerBottom, 0,
                        MUIA_Weight, 40,
                        Child, ( IPTR )( canvases->btnZoom = SimpleButton ( ( IPTR )"+" ) ),
                        Child, ( IPTR )( canvases->btnUnZoom = SimpleButton ( ( IPTR )"-" ) ),
                        Child, ( IPTR )( canvases->btnShowAll = SimpleButton ( ( IPTR )"1:1" ) ),
                    End,
                End,
            End,
            Child, ( IPTR )( canvases->scrollH = ScrollbarObject,
                MUIA_Prop_UseWinBorder, MUIV_Prop_UseWinBorder_Bottom,
            End ),
            Child, ( IPTR )( canvases->scrollV = ScrollbarObject,
                MUIA_Prop_UseWinBorder, MUIV_Prop_UseWinBorder_Right,
            End ),
        End,
    TAG_END );

    // Some values in the area object
    struct RGBitmapData *areaData   = INST_DATA ( mcc->mcc_Class, canvases->area );
    areaData->window                = canvases;
    areaData->mousepressed          = FALSE;
    canvases->id                    = globalWindowIncrement;
    canvases->layersChg             = TRUE; // initially, say yes layers changed
    canvases->prevBlit              = ( struct RectStruct ){ 0, 0, 0, 0 };
    canvases->filename              = NULL; // set the filename to null *obviously*
    canvases->rRectX                = 0;
    canvases->rRectY                = 0;
    canvases->rRectW                = 0;
    canvases->rRectH                = 0;
    canvases->contWidth             = 0;
    canvases->contHeight            = 0;

    // Add project func to hook
    canvases->projHook.h_Entry = ( HOOKFUNC )projFunc;

    // Setup canvas

    canvases->canvas = Init_Canvas ( width, height, layers, frames, generateCanvas );
    canvases->nextwin = temp;

    if ( canvases->canvas->buffer != NULL )
        setActiveBuffer ( canvases->canvas );

    // Add the canvas window to the application gui
    DoMethod ( PaintApp, OM_ADDMEMBER, ( IPTR )canvases->win );

    // Generate window object and add it to the application gui
    CreateProjectWindow ( canvases );

    // Add some methods
    DoMethod (
        canvases->btnZoom, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )canvases->area, 1, MUIM_Luna_ZoomIn
    );
    DoMethod (
        canvases->btnUnZoom, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )canvases->area, 1, MUIM_Luna_ZoomOut
    );
    DoMethod (
        canvases->btnShowAll, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )canvases->area, 1, MUIM_Luna_ShowAll
    );
    DoMethod (
        canvases->win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        ( IPTR )canvases->area, 1, MUIM_Luna_CloseCanvasWin
    );

    DoMethod (
        canvases->win, MUIM_Notify, MUIA_Window_Activate, TRUE,
        ( IPTR )canvases->area, 1, MUIM_Luna_CanvasActivate
    );

    DoMethod (
        canvases->win, MUIM_Notify, MUIA_Window_Activate, FALSE,
        ( IPTR )canvases->area, 1, MUIM_Luna_CanvasDeactivate
    );

    DoMethod (
        canvases->scrollV, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
        ( IPTR )canvases->area, 1, MUIM_Luna_Canvas_ScrollingNotify
    );

    DoMethod (
        canvases->scrollH, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
        ( IPTR )canvases->area, 1, MUIM_Luna_Canvas_ScrollingNotify
    );

    DoMethod ( canvases->win, MUIM_Notify, MUIA_Window_Open, TRUE,
        canvases->win, 2, MUIM_CallHook, &EnableKeyboard_hook );

    // Set global thingies
    globalActiveWindow = canvases;
    globalActiveCanvas = canvases->canvas;

    globalWindowIncrement++;
}

void showFullscreenWindow ( struct oCanvas *canvas )
{
    // Prevent tool preview
    int Tool = globalCurrentTool;
    globalCurrentTool = -1;

    // Set some vars, we need no interference
    fullscreenEditing = TRUE;
    keyboardEnabled = TRUE;

    // Hide all other canvas windows
    struct WindowList *l = canvases;
    do
    {
        set ( l->win, MUIA_Window_Open, FALSE );
    }
    while ( ( l = l->nextwin ) );

    if ( !windowFullscreen )
    {
        windowFullscreen = WindowObject,
            MUIA_Window_Title, ( IPTR )NULL,
            MUIA_Window_SizeGadget, FALSE,
            MUIA_Window_DepthGadget, FALSE,
            MUIA_Window_CloseGadget, FALSE,
            MUIA_Window_TopEdge, 0,
            MUIA_Window_LeftEdge, 0,
            MUIA_Window_DragBar, FALSE,
            MUIA_Window_Screen, ( IPTR ) lunaPubScreen,
            MUIA_Window_MouseObject, ( IPTR ) canvases->mouse,
            MUIA_Window_ScreenTitle, ( IPTR ) VERSION,
            MUIA_Window_UseRightBorderScroller, FALSE,
            MUIA_Window_UseBottomBorderScroller, FALSE,
            MUIA_Window_IsSubWindow, FALSE,
            MUIA_Window_Backdrop, TRUE,
            MUIA_Window_Borderless, TRUE,
            WindowContents, ( IPTR )( fullscreenGroup = VGroup,
                InnerSpacing ( 0, 0 ),
                MUIA_Group_HorizSpacing, 0,
                MUIA_Group_VertSpacing, 0,
                MUIA_Background, ( IPTR )"5:PROGDIR:data/backdrop.png",
            End ),
        End;
        DoMethod ( PaintApp, OM_ADDMEMBER, ( IPTR )windowFullscreen );
    }

    DoMethod ( globalActiveWindow->container, MUIM_Group_InitChange );
    DoMethod ( globalActiveWindow->container, OM_REMMEMBER, ( IPTR )globalActiveWindow->area );
    DoMethod ( globalActiveWindow->container, MUIM_Group_ExitChange );

    DoMethod ( fullscreenGroup, MUIM_Group_InitChange );
    DoMethod ( fullscreenGroup, OM_ADDMEMBER, ( IPTR )globalActiveWindow->area );
    DoMethod ( fullscreenGroup, MUIM_Group_ExitChange );

    set ( windowFullscreen, MUIA_Window_Open, TRUE );

    // Ideal width/height
    WORD width = lunaPubScreen->Width;
    WORD height = lunaPubScreen->Height;
    struct Window *win = ( struct Window *)XGET ( windowFullscreen, MUIA_Window_Window );
    ChangeWindowBox( win, 0, 0, width, height );

    globalCurrentTool = Tool;
}

void hideFullscreenWindow ( )
{
    // Prevent tool preview
    int Tool = globalCurrentTool;
    globalCurrentTool = -1;

    // Let us hide the fullscreen window
    set ( windowFullscreen, MUIA_Window_Open, FALSE );

    // Remove from fullscreen window and add to container
    DoMethod ( fullscreenGroup, MUIM_Group_InitChange );
    DoMethod ( fullscreenGroup, OM_REMMEMBER, ( IPTR )globalActiveWindow->area );
    DoMethod ( fullscreenGroup, MUIM_Group_ExitChange );
    DoMethod ( globalActiveWindow->container, MUIM_Group_InitChange );
    DoMethod ( globalActiveWindow->container, OM_ADDMEMBER, ( IPTR )globalActiveWindow->area );
    DoMethod ( globalActiveWindow->container, MUIM_Group_ExitChange );


    // Show all other canvas windows
    struct WindowList *l = canvases;
    while ( l != NULL )
    {
        set ( l->win, MUIA_Window_Open, TRUE );
        l = l->nextwin;
    }

    // Last part
    fullscreenEditing = FALSE;

    // Ideal width/height
    WORD width = 640;
    WORD height = 480;
    struct Window *win = ( struct Window *)XGET ( globalActiveWindow->win, MUIA_Window_Window );
    ChangeWindowBox( win, 0, 0, width, height );

    globalCurrentTool = Tool;
}

Object *getCanvaswindowById ( unsigned int id )
{
    struct WindowList *tmp = canvases;

    while ( tmp != NULL )
    {
        if ( tmp->id == id )
            return tmp->win;
        tmp = tmp->nextwin;
    }
    return NULL;
}

struct WindowList *getCanvasDataById ( unsigned int id )
{
    struct WindowList *tmp = canvases;

    while ( tmp != NULL )
    {
        if ( tmp->id == id )
            return tmp;
        tmp = tmp->nextwin;
    }
    return NULL;
}

void deleteCanvaswindows ( )
{
    struct WindowList *tmp = NULL;

    while ( canvases != NULL )
    {
        tmp = canvases->nextwin;
        set ( canvases->win, MUIA_Window_Open, FALSE );

        DoMethod ( PaintApp, OM_REMMEMBER, ( IPTR )canvases->win );
        if ( canvases->win != NULL ) DoMethod ( PaintApp, OM_REMMEMBER, ( IPTR )canvases->win );

        Destroy_Canvas ( canvases->canvas );
        DestroyProjectWindow ( canvases );

        if ( canvases->filename != NULL )
            FreeVec ( canvases->filename );
        if ( canvases != NULL )
            FreeVec ( canvases );

        canvases = tmp;
    }
}

IPTR removeActiveWindow ( Class *CLASS, Object *self )
{
    struct RGBitmapData *data = INST_DATA ( CLASS, self );
    set ( data->window->win, MUIA_Window_Open, FALSE );
    DoMethod ( PaintApp, OM_REMMEMBER, ( IPTR )data->window->win );

    struct WindowList *tmp = NULL;
    struct WindowList *ptr = canvases;
    struct WindowList *prev = NULL;

    while ( ptr != NULL )
    {
        tmp = ptr->nextwin;

        if ( ptr == globalActiveWindow )
        {
            // Free memory
            Destroy_Canvas ( ptr->canvas );
            DestroyProjectWindow ( ptr );
            if ( ptr->filename != NULL )
                FreeVec ( ptr->filename );
            if ( ptr != NULL )
                FreeVec ( ptr );

            // Disconnect ptr from canvases chain
            if ( prev == NULL )
            {
                canvases = tmp;
                break;
            }
            prev->nextwin = tmp;
            ptr = tmp;
        }
        prev = ptr;
        ptr = tmp;
    }

    globalActiveWindow = canvases;
    globalActiveCanvas = NULL;

    //TODO: Lets reuse hidden windows in the future
    data->window->win = NULL;
    return ( IPTR )NULL;
}

void blitAreaRect (
    int x, int y, int w, int h,
    struct oCanvas* canvas, struct RastPort *rp
)
{
    // Get zoom - bork bork
    int zoom = canvas->zoom;

    // Look for conflict with pixel coordinates and canvas dimensions
    // was 0 instead of offset on the two next lines

    int testofx = canvas->offsetx / zoom;
    int testofy = canvas->offsety / zoom;
    if ( x < testofx ){ w -= testofx - x; x = testofx; }
    if ( y < testofy ){ h -= testofy - y; y = testofy; }
    if ( x >= ( int )canvas->width ) w = 0; // terminates func
    if ( y >= ( int )canvas->height ) h = 0; // terminates func
    if ( x + w >= ( int )canvas->width ) w = ( int )canvas->width - x;
    if ( y + h >= ( int )canvas->height ) h = ( int )canvas->height - y;

    // Look for conflict with onscreen coordinates and widget
    // dimentions

    double sx = ( x * zoom ) - ( ( canvas->offsetx / zoom ) * zoom );
    double sy = ( y * zoom ) - ( ( canvas->offsety / zoom ) * zoom );
    double sw = w * zoom;
    double sh = h * zoom;
    if ( sx < 0 ){ sw += sx; sx = 0; }
    if ( sy < 0 ){ sh += sy; sy = 0; }
    if ( sx >= canvas->visibleWidth ) sw = 0;
    if ( sy >= canvas->visibleHeight ) sh = 0;
    if ( sx + sw > canvas->visibleWidth ) sw = canvas->visibleWidth - sx;
    if ( sy + sh > canvas->visibleHeight ) sh = canvas->visibleHeight - sy;

    // TODO: Check for tool preview size and add to update area
    // dimensions!

    if ( sw > 0 && sh > 0 )
    {
        // Update screenbuffer
        if ( redrawScreenbufferRect ( canvas, x, y, w, h, FALSE ) )
        {
            WritePixelArray (
                canvas->screenbuffer, 0, 0, canvas->visibleWidth * 4, rp,
                ( int )sx + _getAreaOffsetX ( ) + _getAreaLeft ( ),
                ( int )sy + _getAreaOffsetY ( ) + _getAreaTop ( ),
                ( int )sw, ( int )sh, RECTFMT_RAW
            );
        }
    }
}

void importImageRAW ( unsigned int w, unsigned int h, unsigned long long int *buffer )
{
    // We have a filename!!
    char *filename = getFilename ( );
    if ( filename != NULL  )
    {
        if ( getFilesize ( filename ) == 8 * w * h )
        {
            // The aros way!
            BPTR myfile;
            if ( ( myfile = Open ( filename, MODE_OLDFILE ) ) != BNULL )
            {
                Read ( myfile, buffer, 8 * w * h );
                Close ( myfile );
                MUI_Redraw ( globalActiveWindow->area, MADF_DRAWUPDATE );
            }
        }
        FreeVec ( filename );
    }
}

BOOL loadDatatypeImage ( )
{
    char *filename = getFilename ( );
    BOOL result = FALSE;

    if ( filename != NULL )
    {
        unsigned int size = getFilesize ( filename );

        if ( size > 0 )
        {

            Object *myDtObj = NewDTObject (
                ( APTR )filename,
                DTA_GroupID, GID_PICTURE,
                PDTA_Remap, TRUE,
                PDTA_DestMode, PMODE_V43,
                PDTA_Screen, ( IPTR )lunaPubScreen,
                TAG_END
            );

            if ( myDtObj != NULL )
            {

                struct BitMapHeader *bHead = NULL;
                struct pdtBlitPixelArray message;

                GetDTAttrs ( myDtObj, PDTA_BitMapHeader, &bHead, TAG_END );

                // Do some memtests before you commence
                BOOL proceed = TRUE;

                unsigned long long int *memtest =
                    AllocVec ( bHead->bmh_Width * bHead->bmh_Height * 8 * 3, MEMF_ANY );
                if ( memtest == NULL )
                    proceed = FALSE;
                else FreeVec ( memtest );

                unsigned int *bitmap = AllocVec ( bHead->bmh_Width * bHead->bmh_Height * 4, MEMF_ANY );
                if ( bitmap == NULL )
                    proceed = FALSE;

                // We are proceeding!
                if ( proceed )
                {
                    message.MethodID = PDTM_READPIXELARRAY;
                    message.pbpa_PixelData = bitmap;
                    message.pbpa_PixelFormat = PBPAFMT_RGBA;
                    message.pbpa_PixelArrayMod = bHead->bmh_Width * 4;
                    message.pbpa_Left = 0;
                    message.pbpa_Top = 0;
                    message.pbpa_Width = bHead->bmh_Width;
                    message.pbpa_Height = bHead->bmh_Height;

                    DoMethodA ( myDtObj, ( Msg )&message );

                    addCanvaswindow (
                        bHead->bmh_Width, bHead->bmh_Height, 1, 1, TRUE
                    );

                    int y = 0; for ( ; y < bHead->bmh_Height; y++ )
                    {
                        int pixy = y * bHead->bmh_Width;
                        int x = 0; for ( ; x < bHead->bmh_Width; x++ )
                        {
                            unsigned long long int r = 0ULL, g = 0ULL, b = 0ULL, a = 0ULL;

                            unsigned int p = bitmap[ pixy + x ];

                            r = ( ( p << 24 ) >> 24 ) 	* 256;
                            g = ( ( p << 16 ) >> 24 ) 	* 256;
                            b = ( ( p << 8 ) >> 24 ) * 256;
                            if ( bHead->bmh_Depth == 24 )
                                a = 65535;
                            else
                                a = ( p >> 24 ) * 256;

                            globalActiveCanvas->activebuffer[ pixy + x ] =
                                ( r << 48 ) | ( g << 32 ) | ( b << 16 ) | a;
                        }
                    }

                    FreeVec ( bitmap );
                    DisposeDTObject ( myDtObj );
                    set ( globalActiveWindow->win, MUIA_Window_Title, ( STRPTR )filename );
                    set ( globalActiveWindow->projName, MUIA_String_Contents, ( STRPTR )filename );
                    result = TRUE;
                }
                else printf ( "Not enough memory.\n" );
            }

        }
        FreeVec ( filename );
    }
    return result;
}

void removePrevToolPreview ( )
{
    if ( globalActiveWindow->prevBlit.w > 0 && globalActiveWindow->prevBlit.h > 0 )
    {
        int Tool = globalCurrentTool;
        globalCurrentTool = -1;

        // Blit over prev pos
        blitAreaRect (
            globalActiveWindow->prevBlit.x, globalActiveWindow->prevBlit.y,
            globalActiveWindow->prevBlit.w, globalActiveWindow->prevBlit.h,
            globalActiveCanvas, _rp ( globalActiveWindow->area )
        );

        globalCurrentTool = Tool;

        // Clear prevblit coords
        globalActiveWindow->prevBlit.x = 0;
        globalActiveWindow->prevBlit.y = 0;
        globalActiveWindow->prevBlit.w = 0;
        globalActiveWindow->prevBlit.h = 0;
    }
}

void callToolPreview ( )
{
    if ( globalActiveWindow == NULL )
        return;

    // Some vars to be used
    int x = 0, y = 0, w = 1, h = 1; double offx = 0.0, offy = 0.0;

    switch ( globalCurrentTool )
    {
        case LUNA_TOOL_FILL:
        case LUNA_TOOL_COLORPICKER:

            x = cMouseX;
            y = cMouseY;
            w = 1;
            h = 1;

            break;

        case LUNA_TOOL_BRUSH:

            offx = ( double )brushTool.width / 2;
            offy = ( double )brushTool.height / 2;
            x = cMouseX - offx;
            y = cMouseY - offy;
            w = brushTool.width;
            h = brushTool.height;
            break;

        case LUNA_TOOL_LINE:

            if ( lineTool.mode == 0 )
            {
                offx = ( double )brushTool.width / 2;
                offy = ( double )brushTool.height / 2;
                x = cMouseX - offx;
                y = cMouseY - offy;
                w = brushTool.width;
                h = brushTool.height;
            }
            else if ( lineTool.mode == 1 )
            {
                x = lineTool.ox;
                y = lineTool.oy;
                w = lineTool.w;
                h = lineTool.h;
            }
            break;

        case LUNA_TOOL_RECTANGLE:

            if ( rectangleTool.mode == 0 )
            {
                offx = ( double )brushTool.width / 2;
                offy = ( double )brushTool.height / 2;
                x = cMouseX - offx;
                y = cMouseY - offy;
                w = brushTool.width;
                h = brushTool.height;
            }
            else if ( rectangleTool.mode == 1 )
            {
                x = rectangleTool.ox;
                y = rectangleTool.oy;
                w = rectangleTool.w;
                h = rectangleTool.h;
            }
            break;

        case LUNA_TOOL_CIRCLE:

            if ( circleTool.mode == 0 )
            {
                offx = ( double )brushTool.width / 2;
                offy = ( double )brushTool.height / 2;
                x = cMouseX - offx;
                y = cMouseY - offy;
                w = brushTool.width;
                h = brushTool.height;
            }
            else if ( circleTool.mode == 1 )
            {
                x = circleTool.ox;
                y = circleTool.oy;
                w = circleTool.bufwidth;
                h = circleTool.bufheight;
            }
            break;

        case LUNA_TOOL_CLIPBRUSH:

            if ( clipbrushTool.mode == 0 )
            {
                x = cMouseX;
                y = cMouseY;
                w = 1;
                h = 1;
            }
            else if ( clipbrushTool.mode == 1 )
            {
                x = clipbrushTool.ox;
                y = clipbrushTool.oy;
                w = clipbrushTool.w;
                h = clipbrushTool.h;
            }
            break;

        default: break;
    }

    if ( !w || !h ) return;
    blitToolPreview ( x - 1, y - 1, w + 2, h + 2 );
}

void blitToolPreview ( int x, int y, int w, int h )
{
    removePrevToolPreview ( );
    blitAreaRect ( x, y, w, h, globalActiveCanvas, _rp ( globalActiveWindow->area ) );
    globalActiveWindow->prevBlit = ( struct RectStruct ){ x, y, w, h };
}

ULONG _getAreaWidth ( )
{
    ULONG ideal = globalActiveCanvas->width * globalActiveCanvas->zoom;
    ULONG view = XGET ( globalActiveWindow->area, MUIA_Width );
    if ( ideal < view )
        return ideal;
    return view;

}

ULONG _getAreaHeight ( )
{
    ULONG ideal = globalActiveCanvas->height * globalActiveCanvas->zoom;
    ULONG view = XGET ( globalActiveWindow->area, MUIA_Height );
    if ( ideal < view )
        return ideal;
    return view;
}

ULONG _getAreaTop ( )
{
    return XGET ( globalActiveWindow->area, MUIA_TopEdge );
}

ULONG _getAreaLeft ( )
{
    return XGET ( globalActiveWindow->area, MUIA_LeftEdge );
}

ULONG _getAreaOffsetX ( )
{
    ULONG screenOffX = XGET ( globalActiveWindow->area, MUIA_Width );
    ULONG view = _getAreaWidth ( );
    if ( screenOffX > view )
        screenOffX = ( screenOffX / 2.0 ) - ( view / 2.0 );
    else screenOffX = 0;
    return screenOffX;
}

ULONG _getAreaOffsetY ( )
{
    ULONG screenOffY = XGET ( globalActiveWindow->area, MUIA_Height );
    ULONG view = _getAreaHeight ( );
    if ( screenOffY > view )
        screenOffY = ( screenOffY / 2.0 ) - ( view / 2.0 );
    else screenOffY = 0;
    return screenOffY;
}
