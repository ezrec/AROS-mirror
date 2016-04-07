#ifndef DRAW_H
#define DRAW_H

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

#define UPDATE_UP   1
#define UPDATE_DOWN 2

ULONG Toolbar_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg);

struct TB_Element
{
  struct MinNode MNode;
  ULONG Type;
  WORD DstX, DstY;
  WORD DstOffset;
  BOOL Redraw;

  // the following three if for a TB_Tool
  WORD SrcOffset;
  UBYTE Number; // Placement in toolbar-structure (backwards compatibility).
  BOOL ParseUnderscore;

  // the Size is just used if it is a TB_Space element
  WORD Size;
};

#endif
