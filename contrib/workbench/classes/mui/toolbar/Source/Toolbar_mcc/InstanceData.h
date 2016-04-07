#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

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

#include <mcc_common.h>
#include <mcc_debug.h>

struct Toolbar_Data
{
  // Initialization data (attributes)
  BOOL Dragable;
  BOOL Horizontal;       // Orientation
  BOOL Reusable;         // Should the structure be copied?
  BOOL ParseUnderscore;  // Should underscores be parsed?

  STRPTR Path; // File path for images
  UBYTE ImageType;
  APTR  NormalData;
  APTR  SelectData;
  APTR  GhostData;

  struct MUIP_Toolbar_Description *Desc;

  // Setup/Cleanup
  struct MUI_EventHandlerNode EHNode;
  BOOL Setup;

  // Image related
  BOOL ToolImages;
  ULONG Precision;

  struct BitMap *NormalBM, *SelectBM, *GhostBM;
  PLANEPTR NormalMask, SelectMask, GhostMask;
  struct IP_Image *NormalIP, *SelectIP, *GhostIP;
  Object *NormalDTO, *SelectDTO, *GhostDTO;
  LONG NormalTrans, SelectTrans, GhostTrans; // Background text colours
  UWORD ImageWidth, ImageHeight; // Size of the supplied image
  UBYTE GhostEffect;

  // Text related
  BOOL ToolTexts, CloseToolFont;
  struct TextFont *ToolFont;
  LONG TextPen;
  LONG HalfShine, HalfShadow;
  WORD UnderscoreSize;

  // Layout related
  WORD GroupSpace, ToolSpace; // Size of space-types
  UBYTE GroupSpaces, ToolSpaces; // Number of space-types
  UBYTE Buttons;
  UBYTE InnerSpace;
  BOOL FixSize; // Should the spaces be fixed in size?
  BOOL NeverShrink; // If fixed in size should it be possible to make the spaces smaller?
  UWORD IconWidth, IconHeight; // Size of a single icon
  UWORD ButtonWidth, ButtonHeight, ButtonSize; // Size of a single button including frame
  BYTE  *Permutation;
  BOOL  PermutationCreated;
  ULONG BorderType;
  ULONG SelectionMode;
  BOOL AutoActive;
  struct MinList ToolList;

//  UBYTE Placement; // Inside/Outside?

  // Show/Hide/Draw
  struct RastPort RP;
  BOOL Active;

  // HandleEvent/Draw
  struct TB_Element *Update;    // Button that needs updating
  struct TB_Element *LastUpdate; // Position at last update
  BOOL AcceptShift; // Special handling of the shift-keys
  ULONG UpdateFlag; // UPDATE_UP/DOWN
  BOOL RedrawFlag; // Does any buttons need to be redrawed
  struct TB_Element *SingleGone; // Special case for the GONE-case

  STRPTR HelpString;
  UWORD KeyCode; // The most recently pressed key (mapped).
  UWORD KeyQualifier; // The qualifiers used with the most recently pushed button.

  // Notify
  struct MinList NotifyList;
  BOOL PositionNotify; // Are there any MUIV_Toolbar_Notify_*Edge notifies
};

#endif
