/****************************************************************************
*                                                                           *
* layers.c -- Lunapaint,                                                    *
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
#include "layers.h"

#define LAYERTHUMBSIZE 84
#define LAYERIMAGESIZE 80


Object *WindowLayers;
Object *WidgetLayers;
Object *ScrollLayers;
Object *BtnAddLayer;
Object *BtnDelLayer;
Object *BtnSwapLayer;
Object *BtnMergeLayer;
Object *BtnCopyLayer;
Object *LayerOpacity;
Object *LayerName;
Object *LayerVisible;
Object *LayerControlGroup;
Object *LayerOpacityValue;

unsigned int *LayersWidgetTmpBuf;
unsigned int layersWidgetWidth;
unsigned int layersWidgetHeight;
int layersLastScrollPos;
int layersUpdateX, layersUpdateY, layersUpdateW, layersUpdateH;

struct Hook changeOpacityHook;
struct Hook acknowledgeOpacity;
struct Hook acknowledgeLayName;
struct Hook changeVisibilityHook;
struct oCanvas* lastDrawnCanvas;
BOOL forceLayerRedraw;

HOOKPROTONHNO(changeOpacityFunc, void, int *param)
{

    // Fix it
    if ( globalActiveCanvas != NULL )
    {
        struct gfxbuffer *buf = globalActiveCanvas->buffer;
        char opacity = ( char )XGET( LayerOpacity, MUIA_Numeric_Value );
        int max = globalActiveCanvas->totalFrames * globalActiveCanvas->totalLayers;

        int i = 0; for ( ; i < max; i++ )
        {
            int layer = i % globalActiveCanvas->totalLayers;
            if ( layer == globalActiveCanvas->currentLayer )
                buf->opacity = opacity;
            buf = buf->nextbuf;
        }
        set ( LayerOpacityValue, MUIA_String_Integer, ( IPTR )( ( int )opacity ) );
        globalActiveWindow->layersChg = TRUE;
        DoMethod ( globalActiveWindow->area, MUIM_Luna_Canvas_RedrawArea );
    }

}



HOOKPROTONHNO(changeVisibilityFunc, void, int *param)
{
    // Fix it
    if ( globalActiveCanvas != NULL )
    {
        struct gfxbuffer *buf = globalActiveCanvas->buffer;
        BOOL visible = FALSE;
        int max = globalActiveCanvas->totalFrames * globalActiveCanvas->totalLayers;

        int i = 0; for ( ; i < max; i++ )
        {
            int layer = i % globalActiveCanvas->totalLayers;
            if ( layer == globalActiveCanvas->currentLayer )
            {
                if ( buf->visible )
                    buf->visible = FALSE, visible = FALSE;
                else
                    buf->visible = TRUE, visible = TRUE;
            }
            buf = buf->nextbuf;
        }

        STRPTR message_hidden = AllocVec ( strlen(_(MSG_LAYERS_HIDDEN)), MEMF_CLEAR );
        STRPTR message_shown  = AllocVec ( strlen(_(MSG_LAYERS_SHOWN) ), MEMF_CLEAR );
        if ( visible )
        {
            sprintf ( message_hidden, _(MSG_LAYERS_HIDDEN) );
            set ( LayerVisible, MUIA_Text_Contents, ( IPTR )message_hidden );
        }
        else
        {
            sprintf ( message_shown, _(MSG_LAYERS_SHOWN) );
            set ( LayerVisible, MUIA_Text_Contents, ( IPTR )message_shown );
        }
        FreeVec ( message_hidden );
        FreeVec ( message_shown  );

        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }

}

HOOKPROTONHNO (acknowledgeOpacityFunc, void, int *param)
{

    // Fix it
    if ( globalActiveCanvas != NULL )
    {
        int integer = ( int )XGET ( LayerOpacityValue, MUIA_String_Integer );
        if ( integer < 0 ) integer = 0;
        if ( integer > 100 ) integer = 100;
        set ( LayerOpacity, MUIA_Numeric_Value, integer );
    }

 
}

HOOKPROTONHNO(acknowledgeLayNameFunc, void, int *param)
{
    // Fix it
    if ( globalActiveCanvas )
    {
        struct gfxbuffer *buf = globalActiveCanvas->buffer;
        int max = globalActiveCanvas->totalFrames * globalActiveCanvas->totalLayers;
        int i = 0; for ( ; i < max; i++ )
        {
            int layer = i % globalActiveCanvas->totalLayers;
            if ( layer == globalActiveCanvas->currentLayer )
            {
                unsigned char *nm = ( unsigned char *)XGET ( LayerName, MUIA_String_Contents );
                if ( buf->name != NULL ) FreeVec ( buf->name );
                buf->name = AllocVec ( strlen ( nm ) + 1, MEMF_CLEAR );
                sprintf ( buf->name, "%s", nm );
            }
            buf = buf->nextbuf;
        }
        forceLayerRedraw = TRUE;
        DoMethod ( WidgetLayers, MUIM_Draw );
    }

}

IPTR _Layers_MUIM_Setup ( Class *CLASS, Object *self, Msg message )
{
    MUI_RequestIDCMP( self, IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY );
    return DoSuperMethodA ( CLASS, self, message );
}

IPTR _Layers_MUIM_Draw ( Class *CLASS, Object *self, Msg message )
{
    // Some rules for when to redraw
    if ( !globalActiveWindow || !globalActiveCanvas )
    {
        layerRenderBlank ( );
        set ( LayerName, MUIA_String_Contents, ( IPTR )"\0" );
        return ( IPTR )NULL;
    }
    else
    {
        // Make sure the layer window shows correct opacity
        struct gfxbuffer *buf = getActiveGfxbuffer ( globalActiveCanvas );
        int opa = XGET( LayerOpacity, MUIA_Numeric_Value );
        if ( opa != buf->opacity )
        {
            set ( LayerOpacity, MUIA_Numeric_Value, ( IPTR )buf->opacity );
            return ( IPTR )NULL;
        }
        int testWidth = XGET ( WidgetLayers, MUIA_Width );
        unsigned int posV = 0; get ( ScrollLayers, MUIA_Prop_First, &posV );
        if ( posV != globalActiveCanvas->layerScrollPosV )
            posV = 1;
        else posV = 0;

        // If we changed active window
        if ( lastDrawnCanvas != globalActiveCanvas )
        {
            lastDrawnCanvas = globalActiveCanvas;
            layerRender ( CLASS, self );
            layersRepaintWindow ( CLASS, self );
            set ( LayerName, MUIA_String_Contents, ( IPTR )buf->name );

            STRPTR vistx_shown  = AllocVec ( strlen( _(MSG_LAYERS_SHOWN) ), MEMF_CLEAR );
            STRPTR vistx_hidden = AllocVec ( strlen( _(MSG_LAYERS_HIDDEN)), MEMF_CLEAR );
            if ( buf->visible )
            {
                sprintf ( vistx_shown , _(MSG_LAYERS_SHOWN) );
                set ( LayerVisible, MUIA_Text_Contents, ( IPTR )vistx_shown );
            }
            else
            {
                sprintf ( vistx_hidden, _(MSG_LAYERS_HIDDEN) );
                set ( LayerVisible, MUIA_Text_Contents, ( IPTR )vistx_hidden );
            }
            FreeVec ( vistx_shown );
            FreeVec ( vistx_hidden);
        }
        // If we changed anything important
        else if
        (
            posV == 1 ||
            LayersWidgetTmpBuf == NULL ||
            globalActiveWindow->layersChg ||
            layersWidgetWidth != testWidth ||
            forceLayerRedraw
        )
        {
            layerRender ( CLASS, self );
            forceLayerRedraw = FALSE;
            layersRepaintWindow ( CLASS, self );
            set ( LayerName, MUIA_String_Contents, ( IPTR )buf->name );
            STRPTR vistx_shown  = AllocVec ( strlen( _(MSG_LAYERS_SHOWN) ), MEMF_CLEAR );
            STRPTR vistx_hidden = AllocVec ( strlen( _(MSG_LAYERS_HIDDEN)), MEMF_CLEAR );
            if ( buf->visible )
            {
                sprintf ( vistx_shown , _(MSG_LAYERS_SHOWN)  );
                set ( LayerVisible, MUIA_Text_Contents, ( IPTR )vistx_shown  );
            }
            else
            {
                sprintf ( vistx_hidden, _(MSG_LAYERS_HIDDEN) );
                set ( LayerVisible, MUIA_Text_Contents, ( IPTR )vistx_hidden );
            }
            FreeVec ( vistx_shown );
            FreeVec ( vistx_hidden);
        }
        UpdateCanvasInfo ( globalActiveWindow );
        globalActiveWindow->layersChg = FALSE;
        return ( IPTR )NULL;
    }
}

IPTR _Layers_MUIM_HandleInput ( Class *CLASS, Object *self, Msg message )
{
    if ( globalActiveCanvas == NULL || globalActiveWindow == NULL )
        return 0;

    struct MUIP_HandleInput *msg = ( struct MUIP_HandleInput *)message;

    int topEdge = 0, leftEdge = 0, areaWidth = 0, areaHeight = 0, propFirst = 0;

    get ( WidgetLayers, MUIA_Width, &areaWidth );
    get ( WidgetLayers, MUIA_Height, &areaHeight );
    get ( WidgetLayers, MUIA_TopEdge, &topEdge );
    get ( WidgetLayers, MUIA_LeftEdge, &leftEdge );
    get ( ScrollLayers, MUIA_Prop_First, &propFirst );

    if ( msg->imsg )
    {
        switch ( msg->imsg->Class )
        {
            case IDCMP_MOUSEBUTTONS:
                if (
                    ( int )msg->imsg->MouseX - leftEdge >= 0 &&
                    ( int )msg->imsg->MouseX - leftEdge < areaWidth &&
                    ( int )msg->imsg->MouseY - topEdge >= 0 &&
                    ( int )msg->imsg->MouseY - topEdge < areaHeight
                )
                {
                    // TODO: Subtract vertical scroll offset...
                    int wantedLayer = ( ( double )( ( int )msg->imsg->MouseY - topEdge + propFirst ) / LAYERTHUMBSIZE );
                    wantedLayer = globalActiveCanvas->totalLayers - wantedLayer - 1;
                    if ( wantedLayer < 0 ) wantedLayer = 0;
                    if ( wantedLayer >= globalActiveCanvas->totalLayers )
                        wantedLayer = globalActiveCanvas->totalLayers - 1;
                    if ( wantedLayer != globalActiveCanvas->currentLayer )
                    {
                        globalActiveCanvas->previousLayer = globalActiveCanvas->currentLayer;
                        globalActiveCanvas->currentLayer = wantedLayer;
                        struct gfxbuffer *buf = getActiveGfxbuffer ( globalActiveCanvas );
                        set ( LayerOpacity, MUIA_Numeric_Value, ( IPTR )buf->opacity );
                        set ( LayerName, MUIA_String_Contents, ( STRPTR )buf->name );
                        setActiveBuffer ( globalActiveCanvas );
                    }
                    forceLayerRedraw = TRUE;
                    DoMethod ( WidgetLayers, MUIM_Draw );
                }
            break;

            default: break;
        }
    }
    return 0;
}

IPTR _Layers_MUIM_AskMinMax ( Class *CLASS, Object *self, Msg msg )
{
    DoSuperMethodA ( CLASS, self, msg );
    struct MUIP_AskMinMax *message = ( struct MUIP_AskMinMax * )msg;
    message->MinMaxInfo->MinWidth  += 140;
    message->MinMaxInfo->MinHeight  += 30;
    message->MinMaxInfo->DefWidth   += 150;
    message->MinMaxInfo->DefHeight  += 200;
    message->MinMaxInfo->MaxWidth   += 300;
    message->MinMaxInfo->MaxHeight  += 4096;
    return 0;
}

IPTR _Layers_MUIM_PrevFrame ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas && !IgnoreFramechange )
    {
        PrevFrame ( globalActiveCanvas );
        set ( AnimationSlider, MUIA_Numeric_Value, ( globalActiveCanvas->currentFrame + 1 ) );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_NextFrame ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas && !IgnoreFramechange )
    {
        NextFrame ( globalActiveCanvas );
        set ( AnimationSlider, MUIA_Numeric_Value, ( globalActiveCanvas->currentFrame + 1 ) );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_GotoFrame ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas && !IgnoreFramechange )
    {
        int num = XGET ( AnimationSlider, MUIA_Numeric_Value );
        num = ( num - 1 < 0 ) ? 0 : ( num - 1 );
        globalActiveCanvas->currentFrame = num;
        setActiveBuffer ( globalActiveCanvas );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_CanvasAddLayer ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        addLayer ( globalActiveCanvas );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_CanvasSwapLayer ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        if ( globalActiveCanvas->currentLayer != globalActiveCanvas->previousLayer )
        {
            swapLayers ( globalActiveCanvas );
            globalActiveWindow->layersChg = TRUE;
            winHasChanged ( );
        }
    }
    return 0;
}

IPTR _Layers_MUIM_CanvasDeleteLayer ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        deleteLayer ( globalActiveCanvas );
        setActiveBuffer ( globalActiveCanvas );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_CanvasMergeLayer ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        mergeLayers ( globalActiveCanvas );
        setActiveBuffer ( globalActiveCanvas );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_CanvasCopyLayer ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        copyLayers ( globalActiveCanvas );
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

IPTR _Layers_MUIM_ChangeOnionskin ( Class *CLASS, Object *self, Msg msg )
{
    if ( globalActiveCanvas )
    {
        int num = 0;
        get ( CycleOnionSkin, MUIA_Cycle_Active, &num );
        globalActiveCanvas->onion = ( char )num;
        globalActiveWindow->layersChg = TRUE;
        winHasChanged ( );
    }
    return 0;
}

BOOPSI_DISPATCHER(IPTR, LayersClass, cl, obj, msg)
{
    switch ( msg->MethodID )
    {
        case MUIM_Setup:                            return _Layers_MUIM_Setup ( cl, obj, msg );
        case MUIM_Draw:                             return _Layers_MUIM_Draw ( cl, obj, msg );
        case MUIM_HandleInput:                      return _Layers_MUIM_HandleInput ( cl, obj, msg );
        case MUIM_AskMinMax:                        return _Layers_MUIM_AskMinMax ( cl, obj, msg );
        case MUIM_Luna_Canvas_PrevFrame:            return _Layers_MUIM_PrevFrame ( cl, obj, msg );
        case MUIM_Luna_Canvas_NextFrame:            return _Layers_MUIM_NextFrame ( cl, obj, msg );
        case MUIM_Luna_Canvas_GotoFrame:            return _Layers_MUIM_GotoFrame ( cl, obj, msg );
        case MUIM_Luna_Canvas_CanvasAddLayer:       return _Layers_MUIM_CanvasAddLayer ( cl, obj, msg );
        case MUIM_Luna_Canvas_CanvasSwapLayer:      return _Layers_MUIM_CanvasSwapLayer ( cl, obj, msg );
        case MUIM_Luna_Canvas_CanvasDeleteLayer:    return _Layers_MUIM_CanvasDeleteLayer ( cl, obj, msg );
        case MUIM_Luna_Canvas_CanvasMergeLayer:     return _Layers_MUIM_CanvasMergeLayer ( cl, obj, msg );
        case MUIM_Luna_Canvas_CanvasCopyLayer:      return _Layers_MUIM_CanvasCopyLayer ( cl, obj, msg );
        case MUIM_Luna_Canvas_ChangeOnionskin:      return _Layers_MUIM_ChangeOnionskin ( cl, obj, msg );
        default:                                    return DoSuperMethodA ( cl, obj, msg );
    }

}
BOOPSI_DISPATCHER_END


void Init_LayersWindow ( )
{
    lastDrawnCanvas = NULL;
    LayersWidgetTmpBuf = NULL;

    struct MUI_CustomClass *lay = MUI_CreateCustomClass (
        NULL, MUIC_Area, NULL, 0, &LayersClass
    );

    WindowLayers = WindowObject,
        MUIA_Window_Title, __(MSG_LAYERS_WIN),
        MUIA_Window_ScreenTitle, ( IPTR )VERSION,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','L'),
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_LeftEdge, 0,
        MUIA_Window_TopEdge, ( lunaPubScreen->BarHeight + 1 ),
        MUIA_Window_UseRightBorderScroller, TRUE,
        WindowContents, ( IPTR )VGroup,
            MUIA_Group_HorizSpacing, 0,
            Child, ( IPTR )GroupObject,
                Child, ( IPTR )HGroup,
                    MUIA_Group_VertSpacing, 0,
                    MUIA_Group_HorizSpacing, 0,
                    Child, ( IPTR )GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        InnerSpacing( 0, 0 ),
                        MUIA_Group_VertSpacing, 0,
                        MUIA_Group_HorizSpacing, 0,
                        Child, ( IPTR )( WidgetLayers = NewObject (
                            lay->mcc_Class, NULL,
                            MUIA_FillArea, FALSE,
                            MUIA_Frame, MUIV_Frame_None,
                            TAG_DONE
                        ) ),
                    End,
                    Child, ( IPTR )( ScrollLayers = ScrollbarObject,
                        MUIA_Prop_UseWinBorder, MUIV_Prop_UseWinBorder_Right,
                    End ),
                End,
            End,
            Child, ( IPTR )( LayerControlGroup = VGroup,
                Child, ( IPTR )( LayerName = StringObject,
                    MUIA_String_Contents, ( IPTR )"",
                    MUIA_Frame, MUIV_Frame_String,
                End ),
                Child, ( IPTR )HGroup,
                    Child, ( IPTR )( LayerOpacity = SliderObject,
                        MUIA_Weight, 50,
                        MUIA_Numeric_Min, 0,
                        MUIA_Numeric_Max, 100,
                        MUIA_Numeric_Value, 100,
                        MUIA_Prop_Slider, TRUE,
                        MUIA_Prop_Horiz, TRUE,
                        MUIA_Slider_Horiz, TRUE,
                    End ),
                    Child, ( IPTR )( LayerOpacityValue = StringObject,
                        MUIA_Weight, 25,
                        MUIA_String_Integer, 100,
                        MUIA_String_Accept, ( IPTR )"0123456789",
                        MUIA_String_MaxLen, 4,
                        MUIA_String_Format, MUIV_String_Format_Right,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_CycleChain, 1,
                    End ),
                    Child, ( IPTR )VGroup,
                        MUIA_Weight, 25,
                        InnerSpacing ( 0, 0 ),
                        Child, ( IPTR )( LayerVisible = SimpleButton ( _(MSG_LAYERS_SHOWN) ) ),
                    End,
                End,
            End ),
            Child, ( IPTR )HGroup,
                Child, ( IPTR )( BtnAddLayer = ImageButton ( "", "PROGDIR:data/btn_add.png" ) ),
                Child, ( IPTR )( BtnDelLayer = ImageButton ( "", "PROGDIR:data/btn_sub.png" ) ),
                Child, ( IPTR )( BtnSwapLayer = ImageButton ( "", "PROGDIR:data/btn_swap.png" ) ),
                Child, ( IPTR )( BtnCopyLayer = ImageButton ( "", "PROGDIR:data/btn_copy.png" ) ),
                Child, ( IPTR )( BtnMergeLayer = ImageButton ( "", "PROGDIR:data/btn_merge.png" ) ),
            End,
        End,
    End;

    // Disable the keyboard when this window is activated
    DoMethod (
        WindowLayers, MUIM_Notify, MUIA_Window_Activate, TRUE,
        WindowLayers, 2, MUIM_CallHook, &DisableKeyboard_hook
    );
    DoMethod ( WindowLayers, MUIM_Notify, MUIA_Window_Open, TRUE,
        WindowLayers, 2, MUIM_CallHook, &DisableKeyboard_hook );
    // Enable the keyboard when this window is deactivated
    DoMethod (
        WindowLayers, MUIM_Notify, MUIA_Window_Activate, FALSE,
        WindowLayers, 2, MUIM_CallHook, &EnableKeyboard_hook
    );
    DoMethod ( WindowLayers, MUIM_Notify, MUIA_Window_Open, FALSE,
        WindowLayers, 2, MUIM_CallHook, &EnableKeyboard_hook );

    /* Bubble help */
    DoMethod ( BtnAddLayer, MUIM_Set, MUIA_ShortHelp, _(MSG_LAYERs_BB_ADD) );
    DoMethod ( BtnDelLayer, MUIM_Set, MUIA_ShortHelp, _(MSG_LAYERs_BB_DEL) );
    DoMethod ( BtnSwapLayer, MUIM_Set, MUIA_ShortHelp, _(MSG_LAYERs_BB_SWAP) );
    DoMethod ( BtnCopyLayer, MUIM_Set, MUIA_ShortHelp, _(MSG_LAYER_BB_COPY) );
    DoMethod ( BtnMergeLayer, MUIM_Set, MUIA_ShortHelp, _(MSG_LAYER_BB_MERGE) );

    /*
        Layers
    */
    DoMethod (
        BtnAddLayer, MUIM_Notify,
        MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_CanvasAddLayer
    );
    DoMethod (
        BtnSwapLayer, MUIM_Notify,
        MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_CanvasSwapLayer
    );
    DoMethod (
        BtnDelLayer, MUIM_Notify,
        MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_CanvasDeleteLayer
    );
    DoMethod (
        BtnMergeLayer, MUIM_Notify,
        MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_CanvasMergeLayer
    );
    DoMethod (
        BtnCopyLayer, MUIM_Notify,
        MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_CanvasCopyLayer
    );
    DoMethod (
        ScrollLayers, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
        ( IPTR )WidgetLayers, 1, MUIM_Draw
    );
    DoMethod (
        WindowLayers, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        ( IPTR )WindowLayers, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );

    // Layer opacity
    MakeStaticHook(changeOpacityHook, &changeOpacityFunc);
    MakeStaticHook(changeVisibilityHook, &changeVisibilityFunc);
    DoMethod (
        LayerOpacity, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
        ( IPTR )LayerOpacity, 2, MUIM_CallHook, &changeOpacityHook
    );
    MakeStaticHook(acknowledgeOpacity, &acknowledgeOpacityFunc);
    DoMethod (
        LayerOpacityValue, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
        ( IPTR )LayerOpacityValue, 2, MUIM_CallHook, &acknowledgeOpacity
    );
    MakeStaticHook(acknowledgeLayName, &acknowledgeLayNameFunc);
    DoMethod (
        LayerName, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
        ( IPTR )LayerName, 2, MUIM_CallHook, &acknowledgeLayName
    );
    DoMethod (
        LayerVisible, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )LayerVisible, 2, MUIM_CallHook, &changeVisibilityHook
    );

}

void layerRenderBlank ( )
{
    BOOL winopen = XGET ( WindowLayers, MUIA_Window_Open );
    if ( winopen )
    {
        unsigned int areaWidth = XGET ( WidgetLayers, MUIA_Width );
        unsigned int areaHeight = XGET ( WidgetLayers, MUIA_Height );
        unsigned int areaTop = XGET ( WidgetLayers, MUIA_TopEdge );
        unsigned int areaLeft = XGET ( WidgetLayers, MUIA_LeftEdge );
        int size = areaWidth * areaHeight;
        unsigned int *buf = AllocVec ( size * 4, MEMF_ANY );
        int i = 0; for ( ; i < size; i++ ) buf[ i ] = 0x777777;
        WritePixelArray (
            buf, 0, 0, areaWidth * 4,
            _rp ( WidgetLayers ), areaLeft, areaTop, areaWidth, areaHeight, RECTFMT_RGBA
        );
        FreeVec ( buf );
    }
}

BOOL layerRender ( Class *CLASS, Object *self )
{
    BOOL opened = FALSE;
    get ( WindowLayers, MUIA_Window_Open, &opened );
    if ( !opened || globalActiveCanvas == NULL || globalActiveWindow == NULL )
        return FALSE;

    unsigned int areaWidth = 0, areaHeight = 0;
    unsigned int topEdge = 0, leftEdge = 0;
    int propFirst = 0;

    get ( WidgetLayers, MUIA_Width, &areaWidth );
    get ( WidgetLayers, MUIA_Height, &areaHeight );
    get ( WidgetLayers, MUIA_TopEdge, &topEdge );
    get ( WidgetLayers, MUIA_LeftEdge, &leftEdge );
    get ( ScrollLayers, MUIA_Prop_First, &propFirst );

    int diffy = propFirst - layersLastScrollPos;
    layersLastScrollPos = propFirst;

    // Update rect
    int ux = 0, uy = 0, uw = areaWidth, uh = 0;

    if ( diffy != 0 && abs (diffy ) < areaHeight )
    {
        // source / dest / moveheight
        int sy = 0, dy = 0, mh = 0;

        if ( diffy > 0 )
        {
            sy = topEdge + diffy;
            dy = topEdge;
            mh = areaHeight - diffy;
            uy = areaHeight - diffy - 1;
            uh = diffy + 1;
        }
        else
        {
            sy = topEdge;
            dy = topEdge - diffy;
            mh = areaHeight + diffy;
            uy = 0;
            uh = 0 - diffy + 1;
        }

        if ( uy < 0 ) uy = 0;
        if ( uh > areaHeight ) uh = areaHeight;

        layersUpdateX = 0;
        layersUpdateY = uy;
        layersUpdateW = areaWidth;
        layersUpdateH = uh;

        MovePixelArray (
            leftEdge,
            sy,
            _rp ( WidgetLayers ),
            leftEdge,
            dy,
            areaWidth,
            mh
        );

    }
    else
    {
        uy = 0, uh = areaHeight;
        layersUpdateX = 0;
        layersUpdateY = 0;
        layersUpdateW = areaWidth;
        layersUpdateH = areaHeight;
    }

    layersWidgetWidth = areaWidth; //<- remember width

    int thumbHeight = LAYERTHUMBSIZE, thumbWidth = LAYERTHUMBSIZE; // <- frame
    int imageHeight = LAYERIMAGESIZE, imageWidth = LAYERIMAGESIZE; // <- cropped/centered image
    if ( globalActiveCanvas->width > globalActiveCanvas->height )
        imageHeight = ( double )globalActiveCanvas->height / globalActiveCanvas->width * LAYERIMAGESIZE;
    else
        imageWidth = ( double )globalActiveCanvas->width / globalActiveCanvas->height * LAYERIMAGESIZE;
    int HSpace = ( double )( thumbWidth - imageWidth ) / 2;
    int VSpace = ( double )( thumbHeight - imageHeight ) / 2;

    if ( LayersWidgetTmpBuf != NULL ) FreeVec ( LayersWidgetTmpBuf );
    LayersWidgetTmpBuf = AllocVec ( areaWidth * areaHeight * 4, MEMF_ANY|MEMF_CLEAR );

    struct gfxbuffer *buf = globalActiveCanvas->buffer;

    int uxuw = ux + uw;
    int uyuh = uy + uh;

    int f = 0; for ( ; f < globalActiveCanvas->totalFrames; f++ )
    {
        int l = 0; for ( ; l < globalActiveCanvas->totalLayers; l++ )
        {
            if ( f == globalActiveCanvas->currentFrame )
            {
                // Get draw offset with highest layernumber shown on top (newest on top of stack)
                int lOffsetY = ( LAYERTHUMBSIZE * ( globalActiveCanvas->totalLayers - l - 1 ) ) - propFirst;

                int y = 0; for ( ; y < LAYERTHUMBSIZE; y ++ )
                {
                    int py = ( double )( y - VSpace ) / imageHeight * globalActiveCanvas->height;
                    int lOffsetYy = y + lOffsetY;

                    int x = ux; for ( ; x < uw; x++ )
                    {
                        int px = ( double )( x - HSpace ) / imageWidth * globalActiveCanvas->width;

                        // Draw only what's in view in the widget
                        if ( x >= ux && x < uxuw && lOffsetYy >= uy && lOffsetYy < uyuh )
                        {
                            // offset to plot in tmpBuf
                            int off = lOffsetYy * areaWidth + x;

                            // within the frame
                            if ( x >= HSpace && x < imageWidth + HSpace && y >= VSpace && y < imageHeight + VSpace )
                            {
                                // Frame
                                if (
                                    x == HSpace || x == HSpace + imageWidth - 1 ||
                                    y == VSpace || y == VSpace + imageHeight - 1
                                )
                                {
                                    if ( l == globalActiveCanvas->currentLayer )
                                        LayersWidgetTmpBuf[ off ] = 0xffffff;
                                    else LayersWidgetTmpBuf[ off ] = 0xaaaaaa;
                                }
                                // Image
                                else
                                {
                                    struct rgbaData rgba = canvasColorToRGBA (
                                        buf->buf[ py * globalActiveCanvas->width + px ]
                                    );
                                    // Checks
                                    unsigned int colr = *( unsigned int *)&( ( struct rgba32 ){ 128, 128, 128, 255 } );
                                    int checkCol = ( ( x % 16 ) + ( int )( y / 8 ) * 8 ) % 16;
                                    if ( checkCol >= 8 )
                                        colr = *( unsigned int *)&( ( struct rgba32 ){ 90, 90, 90, 255 } );
                                    // Blend
                                    if ( rgba.a > 0 )
                                    {
                                        struct rgba32 blend = *( struct rgba32 *)&colr;
                                        double balph = ( double )rgba.a / 255;
                                        // Convert to safe mode
                                        int r = ( int )blend.r, g = ( int )blend.g, b = ( int )blend.b;
                                        int r2 =  (int )rgba.r, g2 = ( int )rgba.g, b2 =  (int )rgba.b;
                                        // Blend
                                        r -= ( r - r2 ) * balph;
                                        g -= ( g - g2 ) * balph;
                                        b -= ( b - b2 ) * balph;
                                        // write
                                        LayersWidgetTmpBuf[ off ] = r | ( g << 8 ) | ( b << 16 ) | 0;
                                    }
                                    else LayersWidgetTmpBuf[ off ] = colr;
                                }
                            }
                            else
                            {
                                // Switch background
                                if ( l % 2 == 0 )
                                    LayersWidgetTmpBuf[ off ] = 0x222222;
                                else
                                    LayersWidgetTmpBuf[ off ] = 0x444444;
                            }
                        }
                    }
                }
            }
            buf = buf->nextbuf;
        }
    }
    return TRUE;
}

void RenderLayerNames ( int x, int y, int w, int h )
{
    // illegal w/h
    if ( !w || !h ) return;

    struct oCanvas *canv = globalActiveCanvas;
    struct gfxbuffer *buf = canv->buffer;
    int size = canv->totalLayers * canv->totalFrames;
    Object *obj = WidgetLayers;
    int areaHeight = ( int )XGET( WidgetLayers, MUIA_Height );
    int areaWidth = ( int )XGET( WidgetLayers, MUIA_Width );
    int areaLeft =  ( int )XGET( WidgetLayers, MUIA_LeftEdge );
    int areaTop = ( int )XGET( WidgetLayers, MUIA_TopEdge );

    // out of bounds
    if ( x >= areaWidth ) return;
    if ( y >= areaHeight ) return;
    if ( x + w < 0 ) return;
    if ( y + h < 0 ) return;

    // snap to borders
    if ( x + w >= areaWidth ) w = ( areaWidth - 1 - x );
    if ( y + h >= areaHeight ) h = ( areaHeight - 1 - y );

    // put into place
    x += areaLeft;
    y += areaTop;

    // Add clipping rect (don't draw outside it)
    APTR clip = MUI_AddClipping (
        muiRenderInfo( obj ),
        x, y, w, h
    );

    SetAPen( _rp( obj ), _pens( obj )[MPEN_SHINE] );
    int ioff = ( int )XGET ( ScrollLayers, MUIA_Prop_First );
    int yoffset = 20;
    int xoffset = 4;
    struct TextFont *fnt = _font ( LayerVisible );
    SetFont ( _rp ( obj ), fnt );

    int i = 0; for ( ; i < size; i++ )
    {
        int l = i % canv->totalLayers;
        int f = i / canv->totalLayers;
        if ( f == canv->currentFrame )
        {
            int y = ( canv->totalLayers - l - 1 ) * LAYERTHUMBSIZE - ioff;
            // Layer name:
            Move ( _rp ( obj ), areaLeft + LAYERTHUMBSIZE + xoffset, y + areaTop + yoffset );
            Text ( _rp ( obj ), buf->name, strlen ( buf->name ) );
            // Layer opacity
            Move ( _rp ( obj ), areaLeft + LAYERTHUMBSIZE + xoffset, y + areaTop + yoffset + fnt->tf_YSize + 4 );
            STRPTR percent = "%";
            STRPTR str = AllocVec ( strlen( _(MSG_LAYERS_OPACITY)) + 5, MEMF_ANY|MEMF_CLEAR );
            sprintf ( str, _(MSG_LAYERS_OPACITY), buf->opacity, percent );
            Text ( _rp ( obj ), str, strlen ( str ) );
            FreeVec ( str );
            // Visibility
            Move ( _rp ( obj ), areaLeft + LAYERTHUMBSIZE + xoffset, y + areaTop + yoffset + ( ( fnt->tf_YSize + 4 ) * 2 ) );
            if ( buf->visible )
                Text ( _rp ( obj ), _(MSG_LAYERS_SHOWN), strlen( _(MSG_LAYERS_SHOWN) ) );
            else Text ( _rp ( obj ), _(MSG_LAYERS_HIDDEN), strlen( _(MSG_LAYERS_HIDDEN) ) );
        }
        buf = buf->nextbuf;
    }
    MUI_RemoveClipping( muiRenderInfo( obj ), clip );
}

void layersRepaintWindow ( Class *CLASS, Object *self )
{
    BOOL opened = FALSE;
    get ( WindowLayers, MUIA_Window_Open, &opened );
    if ( !opened ) return;

    unsigned int areaWidth = 0, areaHeight = 0;
    unsigned int topEdge = 0, leftEdge = 0;

    get ( WidgetLayers, MUIA_Width, &areaWidth );
    get ( WidgetLayers, MUIA_Height, &areaHeight );
    get ( WidgetLayers, MUIA_TopEdge, &topEdge );
    get ( WidgetLayers, MUIA_LeftEdge, &leftEdge );

    // Write the layer data
    WritePixelArray (
        LayersWidgetTmpBuf, 0, layersUpdateY, areaWidth * 4,
        _rp ( WidgetLayers ), leftEdge + layersUpdateX, topEdge + layersUpdateY, layersUpdateW, layersUpdateH, RECTFMT_RGBA
    );
    RenderLayerNames ( layersUpdateX, layersUpdateY, layersUpdateW, layersUpdateH );
    UpdateCanvasInfo ( globalActiveWindow );

    // Update layers scrollbar
    if (
    globalActiveCanvas->winHasChanged || globalActiveWindow->layersChg ||
    areaWidth != layersWidgetWidth || areaHeight != layersWidgetHeight
    )
    {
        globalActiveWindow->layersChg = FALSE;
        set ( ScrollLayers, MUIA_Prop_Visible, areaHeight );
        set ( ScrollLayers, MUIA_Prop_Entries, ( LAYERTHUMBSIZE * globalActiveCanvas->totalLayers ) );
        layersWidgetHeight = areaHeight;
        layersWidgetWidth = areaWidth;
    }
}

void Exit_LayersWindow ( )
{
    if ( LayersWidgetTmpBuf != NULL )
        FreeVec ( LayersWidgetTmpBuf );
}
