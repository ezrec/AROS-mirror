/*
** menus.c
*/

/*
 * 4p - Pixel Pushing Paint Program
 * Copyright (C) 2000 Scott "Jerry" Lawrence
 *                    jsl.4p@umlautllama.com
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

#include "jsui.h"
#include "menus.h"


JSUI_DIALOG menus_dialog[] =
{
   /* (dialog proc)     (x)   (y)   (w)   (h)   
		(key) (flags)  (d1)  (d2)  (dp) (dp2) (dp3) */

    { jsui_widget_menu,  0, 0, -1,1, 0, 0, 0, 0, menu_title, 0, NULL},

    JSUI_DLG_END
};

