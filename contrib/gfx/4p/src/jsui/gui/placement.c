/*
 * placement
 * 
 */

/*
 * JSUI - Jerry's SDL User Interface
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

#include "jsui.h"
#include "active.h"


// reposition the dialog.  this is the absolute position
int 
jsui_dialog_position (
	JSUI_DIALOG * dlg, 
	int x, 
	int y
)
{
    JSUI_DIALOG_RUNNING * jdr;

    jdr = jsui_dialog_find(dlg);

    if (!jdr) return -1;

    //Page_DR_Dirtify(jsui_screen, jdr->x, jdr->y, jdr->x+dlg->w, jdr->y+dlg->h);

    jdr->x = MAX(x,0);
    jdr->x = MIN(jdr->x, jsui_screen->w-22);
    jdr->y = MAX(y,0);
    jdr->y = MIN(jdr->y, jsui_screen->h-11);

    jdr->flags |= JSUI_F_DIRTY;

    return 0;
}



// center the dialog in the screen
int
jsui_dialog_center (
	JSUI_DIALOG * dlg
)
{
    JSUI_DIALOG_RUNNING * jdr;
    int x,y;

    jdr = jsui_dialog_find(dlg);

    if (!jdr) return -1;

    x = jsui_screen->w/2-(dlg[0].w/2);
    y = jsui_screen->h/2-(dlg[0].h/2);

    return jsui_dialog_position(dlg, x,y);
}
