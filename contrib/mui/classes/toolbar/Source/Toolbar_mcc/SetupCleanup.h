#ifndef SETUPCLEANUP_H
#define SETUPCLEANUP_H

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

ULONG Toolbar_Setup(struct IClass *cl, Object *obj, Msg msg);
ULONG Toolbar_Cleanup(struct IClass *cl, Object *obj, Msg msg);
VOID DumpKeyValues(struct Toolbar_Data *data);

#endif
