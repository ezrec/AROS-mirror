#ifndef SETGET_H
#define SETGET_H

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

struct TB_Element *FindTool(struct Toolbar_Data *data, ULONG number);

IPTR Toolbar_Edge(struct IClass *cl, Object *obj, struct MUIP_Toolbar_Edge *msg);
IPTR Toolbar_GetAttrib(struct IClass *cl, Object *obj, struct opGet *msg);
IPTR Toolbar_SetAttrib(struct IClass *cl, Object *obj, struct opSet *msg);
IPTR Toolbar_MultiSet(struct IClass *cl, Object *obj, struct MUIP_Toolbar_MultiSet *msg);
IPTR Toolbar_Set(struct IClass *cl, Object *obj, struct MUIP_Toolbar_Set *msg);


#endif
