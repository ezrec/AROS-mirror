/*
** active
**
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

#ifndef __ACTIVE_H__
#define __ACTIVE_H__

#include <stdlib.h>
#include "jsui.h"

extern JSUI_DIALOG_RUNNING * jsui_active_list;

void jsui_dialog_list_free(void);

JSUI_DIALOG_RUNNING * jsui_dialog_find(JSUI_DIALOG * dialog);

int jsui_dialog_run(JSUI_DIALOG * dialog);

JSUI_DIALOG_RUNNING * __jsui_dr_find_and_remove(JSUI_DIALOG * dialog);

void jsui_dialog_close(JSUI_DIALOG * dialog);

void jsui_dialog_raise(JSUI_DIALOG * dialog);

void jsui_dialog_lower(JSUI_DIALOG * dialog);

void jsui_dialog_broadcast(int msg, int value);

#endif
