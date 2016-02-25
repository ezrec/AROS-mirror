/****************************************************************************
*                                                                           *
* about_window.c -- Lunapaint,                                              *
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


#include "about_window.h"

Object *aboutWindow, *aboutWindowOk;

void Init_AboutWindow ( )
{
    aboutWindow = WindowObject,
        MUIA_Window_Title, __(MSG_ABOUT_WIN),
        MUIA_Window_ScreenTitle, __(MSG_ABOUT_SCR),
        MUIA_Window_Screen, ( IPTR )lunaPubScreen,
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_ID, MAKE_ID('L','P','A','B'),
        WindowContents, ( IPTR )VGroup,
            Child, ( IPTR )GroupObject,
                InnerSpacing(0,0),
                MUIA_Frame, MUIV_Frame_Group,
                Child, ( IPTR )ImageObject,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_Image_Spec, (IPTR)"3:PROGDIR:data/lunaabout_logo.png",
                End,
            End,
            Child, ( IPTR )GroupObject,
                MUIA_Frame, MUIV_Frame_Group,
                Child, ( IPTR )TextObject,
                    MUIA_Text_Contents, ( IPTR )LUNA_ABOUT_TEXT,
                End,
            End,
            Child, ( IPTR )( aboutWindowOk = SimpleButton ( _(MSG_ABOUT_CLOSE) )),
        End,
    End;
    DoMethod (
        aboutWindowOk, MUIM_Notify, MUIA_Pressed, FALSE,
        aboutWindow, 3, MUIM_Set, MUIA_Window_Open, FALSE
    );
}
