#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

/***************************************************************************

 Toolbar MCP - MUI Custom Class Preferences for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

#include <mcc_common.h>
#include <mcc_debug.h>

enum
{
  Look = 0,
  GroupSpace,
  ToolSpace,
  InnerSpace_Text,
  InnerSpace_NoText,
  UseImages,
  Precision,
  GhostEffect,
  ToolPen,
  ToolFont,
  BorderType,
  SelectionMode,
  AutoActive,
  NumberOfGadgets
};

struct Toolbar_DataP
{
  struct Catalog *catalog;
  Object *Gadgets[NumberOfGadgets];
  Object  *PrefsGroup;
  Object  *FontASL;
};

#define TB_OBSOLETE

#endif
