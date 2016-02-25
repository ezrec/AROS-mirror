/****************************************************************************
*                                                                           *
* morphos.c -- Lunapaint,                                                   *
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

#include <proto/intuition.h>
#include <proto/muimaster.h>

ULONG XGET(APTR obj, ULONG attr)
{
    ULONG val;
    GetAttr(attr, obj, &val);
    return val;
}


APTR ImageButton(CONST_STRPTR text, CONST_STRPTR image)
{
    APTR obj, img;

    img = MUI_NewObject("Dtpic.mui", MUIA_Dtpic_Name, image, TAG_DONE);

    if (text)
    {
        obj = VGroup,
            Child, HCenter(img),
            Child, TextObject, text, MUIA_Text_PreParse, "\033c", End,
        End;
    }
    else
    {
        obj = img;
    }

    return obj;
}
