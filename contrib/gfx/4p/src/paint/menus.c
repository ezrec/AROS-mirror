/*
 * menus
 *
 *   menu stuff
 */

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@absynth.com
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <stdio.h>
#include "menus.h"


//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_4p[] = 
{
    {"About 4p",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_BAR
    {"Preferences",	menu_stub, NULL, 0, NULL},
    {"Quit 4p",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_file_flip[] =
{
    {"Horiz",		menu_stub, NULL, 0, NULL},
    {"Vert",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_file_color[] =
{
    {"&Palette",	menu_stub, NULL, 0, NULL},
    {"Use Brush Palette",	menu_stub, NULL, 0, NULL},
    {"Restore Palette",	menu_stub, NULL, 0, NULL},
    {"Default Palette",	menu_stub, NULL, 0, NULL},
    {"Cycle [Tab]",	menu_stub, NULL, 0, NULL},
    {"BG  -> FG",	menu_stub, NULL, 0, NULL},
    {"BG <-> FG",	menu_stub, NULL, 0, NULL},
    {"Remap",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_file_spare[] =
{
    {"Swap [J]",	menu_stub, NULL, 0, NULL},
    {"Copy To Spare",	menu_stub, NULL, 0, NULL},
    {"Merge In Front",	menu_stub, NULL, 0, NULL},
    {"Merge In Back",	menu_stub, NULL, 0, NULL},
    {"Delete This",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_file[] = 
{
    {"&Load",		menu_stub, NULL, 0, NULL},
    {"&Save",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_BAR
    {"&Flip",		NULL, menu_file_flip, 0, NULL},
    {"&Change Color",	NULL, menu_file_color, 0, NULL},
    {"Spare",		NULL, menu_file_spare, 0, NULL},
    {"Page Size",	menu_stub, NULL, 0, NULL},
    {"Show Page",	menu_stub, NULL, 0, NULL},
    {"Screen Format",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};



//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_brush_size[] =
{
    {"&Stretch",	menu_stub, NULL, 0, NULL},
    {"&Halve",		menu_stub, NULL, 0, NULL},
    {"&Double",		menu_stub, NULL, 0, NULL},
    {"&Double Horiz",	menu_stub, NULL, 0, NULL},
    {"&Double Vert",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_flip[] =
{
    {"&Horiz",		menu_stub, NULL, 0, NULL},
    {"&Vert",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_outline[] =
{
    {"&Outline",	menu_stub, NULL, 0, NULL},
    {"&Trim",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_rotate[] =
{
    {"&90 Degrees",	menu_stub, NULL, 0, NULL},
    {"&Any Angle",	menu_stub, NULL, 0, NULL},
    {"&Shear",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_color[] =
{
    {"&BG  -> FG",	menu_stub, NULL, 0, NULL},
    {"&BG <-> FG",	menu_stub, NULL, 0, NULL},
    {"&Remap",		menu_stub, NULL, 0, NULL},
    {"&Transparency",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_bend[] =
{
    {"&Horiz",		menu_stub, NULL, 0, NULL},
    {"&Vert",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush_handle[] =
{
    {"&Center",		menu_stub, NULL, 0, NULL},
    {"&Corner",		menu_stub, NULL, 0, NULL},
    {"&Place",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_brush[] =
{
    {"&Load",		menu_stub, NULL, 0, NULL},
    {"&Save",		menu_stub, NULL, 0, NULL},
    {"&Restore",	menu_stub, NULL, 0, NULL},
    {"&Size",		NULL, menu_brush_size, 0, NULL},
    {"&Flip",		NULL, menu_brush_flip, 0, NULL},
    {"&Outline",	NULL, menu_brush_outline, 0, NULL},
    {"&Rotate",		NULL, menu_brush_rotate, 0, NULL},
    {"&Change Color",	NULL, menu_brush_color, 0, NULL},
    {"&Bend",		NULL, menu_brush_bend, 0, NULL},
    {"&Handle",		NULL, menu_brush_handle, 0, NULL},
    JSUI_MENU_END
};

//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_mode[] =
{
    {"&Matte     [F1]",	menu_stub, NULL, 0, NULL},
    {"&Color     [F2]",	menu_stub, NULL, 0, NULL},
    {"&Replace   [F3]",	menu_stub, NULL, 0, NULL},
    {"&Smear     [F4]",	menu_stub, NULL, 0, NULL},
    {"&Shade     [F5]",	menu_stub, NULL, 0, NULL},
    {"&Blend     [F6]",	menu_stub, NULL, 0, NULL},
    {"&Cycle     [F7]",	menu_stub, NULL, 0, NULL},
    {"&Smooth    [F8]",	menu_stub, NULL, 0, NULL},
    {"&Tint     ",	menu_stub, NULL, 0, NULL},
    {"&HalfBrite",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_anim[] =
{
    {"&Load",		menu_stub, NULL, 0, NULL},
    {"&Save",		menu_stub, NULL, 0, NULL},
    {"&Move",		menu_stub, NULL, 0, NULL},
    {"&Frames",		menu_stub, NULL, 0, NULL},
    {"&Control",	menu_stub, NULL, 0, NULL},
    {"&Anim Brush",	menu_stub, NULL, 0, NULL},
    {"&Method",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_effects_stencil[] =
{
    {"&Load",		menu_stub, NULL, 0, NULL},
    {"&Save",		menu_stub, NULL, 0, NULL},
    {"&Make",		menu_stub, NULL, 0, NULL},
    {"&Remake",		menu_stub, NULL, 0, NULL},
    {"&Lock FG",	menu_stub, NULL, 0, NULL},
    {"&Reverse",	menu_stub, NULL, 0, NULL},
    {"&On/Off [~]",	menu_stub, NULL, 0, NULL},
    {"&Free",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_effects_background[] =
{
    {"&Fix",		menu_stub, NULL, 0, NULL},
    {"&Off",		menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_effects[] =
{
    {"&Stencil",	NULL, menu_effects_stencil, 0, NULL},
    {"&Background",	NULL, menu_effects_background, 0, NULL},
    JSUI_MENU_END
};

//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_font_style[] =
{
    {"&Bold",		menu_stub, NULL, 0, NULL},
    {"&Italic",		menu_stub, NULL, 0, NULL},
    {"&Underline",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

JSUI_MENU menu_font[] =
{
    {"&Style",		NULL, menu_font_style, 0, NULL},
    {"&Scan Fonts",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};

//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_prefs[] =
{
    {"&Coords",		menu_stub, NULL, 0, NULL},
    {"&Multicycle",	menu_stub, NULL, 0, NULL},
    {"&Excl Brush",	menu_stub, NULL, 0, NULL},
    {"&Auto Transp",	menu_stub, NULL, 0, NULL},
    {"&Autogrid",	menu_stub, NULL, 0, NULL},
    JSUI_MENU_END
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

JSUI_MENU menu_title[] =
{
    {"4p",		NULL, menu_4p, 0, NULL},
    {"File",		NULL, menu_file, 0, NULL},
    {"Brush",		NULL, menu_brush, 0, NULL},
    {"Mode",		NULL, menu_mode, 0, NULL},
    {"Anim",		NULL, menu_anim, 0, NULL},
    {"Effects",		NULL, menu_effects, 0, NULL},
    {"Font",		NULL, menu_font, 0, NULL},
    {"Prefs",		NULL, menu_prefs, 0, NULL},
    JSUI_MENU_END
};
