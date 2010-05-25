#ifndef MUI_NLISTTREE_priv_MCP_H
#define MUI_NLISTTREE_priv_MCP_H

/***************************************************************************

 NListtree.mcc - New Listtree MUI Custom Class
 Copyright (C) 1999-2001 by Carsten Scholling
 Copyright (C) 2001-2005 by NList Open Source Team

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#include "amiga-align.h"
#include "../nlisttree_mcc/private.h"
#include "default-align.h"

#include "Debug.h"

#include <mcc_common.h>

#define MCPMAXRAWBUF 64

struct NListtreeP_Data
{
	Object *NLT_Sample;
  Object *BT_Sample_Expand;
  Object *BT_Sample_Collapse;
  Object *GR_Prefs;
	Object *PI_ImageClosed;
	Object *PI_ImageOpen;
  Object *PI_ImageSpecial;
  Object *PP_LinePen;
  Object *PP_ShadowPen;
  Object *PP_DrawPen;
  Object *PP_Draw2Pen;
  Object *CY_Style;
  Object *SL_Space;
  Object *CH_RememberStatus;
  Object *CH_OpenAutoScroll;
};

#endif /* MUI_NLISTTREE_priv_MCP_H */
