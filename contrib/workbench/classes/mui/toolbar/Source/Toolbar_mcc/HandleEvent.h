#ifndef HANDLEEVENT_H
#define HANDLEEVENT_H

/***************************************************************************

 Toolbar MCC - MUI Custom Class for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

VOID Exclude(struct Toolbar_Data *data, ULONG exclude);
ULONG Toolbar_HandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg);

#define UPDATE_UP   1
#define UPDATE_DOWN 2

#endif
