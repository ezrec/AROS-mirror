/****************************************************************************
*                                                                           *
* animation.c -- Lunapaint,                                                 *
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

#include "animation.h"

Object *WindowAnimation;
Object *CycleOnionSkin;
Object *ButtonAnimNext;
Object *ButtonAnimPrev;
Object *AnimationSlider;
BOOL    IgnoreFramechange;
static STRPTR arr_onion[] = {NULL, NULL, NULL};
void Init_AnimationWindow ( )
{
    arr_onion[0] = _(MSG_ANIM_NOONION);
    arr_onion[1] = _(MSG_ANIM_SIMPLEONION);

    WindowAnimation = WindowObject,
        MUIA_Window_Title, __(MSG_ANIM_CONTROL),
        MUIA_Window_ScreenTitle, ( IPTR )VERSION,
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_ID, MAKE_ID('L','P','W','A'),
//        MUIA_Window_LeftEdge, 0,
//        MUIA_Window_TopEdge, ( lunaPubScreen->BarHeight + 1 ),
        WindowContents, ( IPTR )HGroup,
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_None,
                MUIA_Weight, 1,
                Child, ( IPTR )VGroup,
                    Child, ( IPTR )( CycleOnionSkin = CycleObject,
                        MUIA_Cycle_Entries, arr_onion,
                    End ),
                End,
            End,
            Child, ( IPTR )( AnimationSlider = SliderObject,
                MUIA_Group_Horiz, TRUE,
                MUIA_Numeric_Min, 1,
                MUIA_Numeric_Max, 1,
                MUIA_Numeric_Value, 1,
                MUIA_Weight, 100,
            End ),
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_None,
                MUIA_Weight, 1,
                Child, ( IPTR )HGroup,
                    Child, ( IPTR )( ButtonAnimPrev = SimpleButton ( ( IPTR )"«" ) ),
                    Child, ( IPTR )( ButtonAnimNext = SimpleButton ( ( IPTR )"»" ) ),
                End,
            End,
        End,
    End;

    // Animation
    DoMethod (
        ButtonAnimNext, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_NextFrame
    );
    DoMethod (
        ButtonAnimPrev, MUIM_Notify, MUIA_Pressed, FALSE,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_PrevFrame
    );
    DoMethod (
        AnimationSlider, MUIM_Notify,
        MUIA_Numeric_Value, MUIV_EveryTime,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_GotoFrame
    );
    DoMethod (
        CycleOnionSkin, MUIM_Notify,
        MUIA_Cycle_Active, MUIV_EveryTime,
        ( IPTR )WidgetLayers, 1, MUIM_Luna_Canvas_ChangeOnionskin
    );
}

void Update_AnimValues ( )
{
    if ( !globalActiveCanvas ) return;

    int currAnimMin = ( int )XGET ( AnimationSlider, MUIA_Numeric_Min );
    int currAnimMax = ( int )XGET ( AnimationSlider, MUIA_Numeric_Max );
    int currAnimTot = ( int )XGET ( AnimationSlider, MUIA_Numeric_Value );
    int currOnionSk = ( int )XGET ( CycleOnionSkin, MUIA_Cycle_Active );

    IgnoreFramechange = TRUE;
    if ( currAnimMin != 1 )
        set ( AnimationSlider, MUIA_Numeric_Min, 1 );
    if ( currAnimMax != globalActiveCanvas->totalFrames )
        set ( AnimationSlider, MUIA_Numeric_Max, ( IPTR )globalActiveCanvas->totalFrames );
    if ( currAnimTot != globalActiveCanvas->currentFrame + 1 )
        set ( AnimationSlider, MUIA_Numeric_Value, ( IPTR )globalActiveCanvas->currentFrame + 1 );
    if ( currOnionSk != ( int )globalActiveCanvas->onion )
        set ( CycleOnionSkin, MUIA_Cycle_Active, ( IPTR )globalActiveCanvas->onion );
    IgnoreFramechange = FALSE;
}
