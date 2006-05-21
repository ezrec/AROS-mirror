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

// ANSI C
#include <string.h>

// Protos
#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>

// Other
#include <libraries/mui.h>
#include <graphics/gfxmacros.h>

// Toolbar inclusions
#include "Toolbar_mcc.h"
#include "InstanceData.h"
#include "ConfigValues.h"
//#include "MinList.h"
#include "Draw.h"

#define THREE_D     TRUE
#define NOT_THREE_D FALSE

/*
TODO:
Omskriv ->
*/

VOID UpperFrame (struct RastPort *rport, UWORD MinX, UWORD MinY, UWORD MaxX,
           UWORD MaxY, UWORD color)
{
  SetAPen(rport, color);
  RectFill(rport, MinX, MinY, MaxX, MinY);
  RectFill(rport, MinX, MinY, MinX, MaxY);
}

VOID LowerFrame (struct RastPort *rport, UWORD MinX, UWORD MinY, UWORD MaxX,
           UWORD MaxY, UWORD color)
{
  SetAPen(rport, color);
  RectFill(rport, MinX, MaxY, MaxX, MaxY);
  RectFill(rport, MaxX, MinY, MaxX, MaxY);
}

VOID DrawText (struct RastPort *rport, WORD x, WORD y, STRPTR text, char hotkey, UWORD textcolor, BOOL parse)
{
  ULONG length = strlen(text);
  y += rport->TxBaseline;

  SetAPen(rport, textcolor);
  Move(rport, x, y);

  if(hotkey)
  {
    if(parse)
    {
      char *textpnt = text;
      UWORD delta = 0;
      while(*textpnt != '\0')
      {
        if(*textpnt == '_')
        {
          Text(rport, text, delta);
          x += TextLength(rport, text, delta);
          text = textpnt+1;
          RectFill(rport, x, y+2, x+TextLength(rport, text, 1)-2, y+2);
          Move(rport, x, y);
          break;
        }
        delta++;
        textpnt++;
      }
      Text(rport, text, strlen(text));
    }
    else
    {
      ULONG i=0;
      Text(rport, text, length);

      for(i=0; i != length; i++)
      {
        if(hotkey == ToLower(text[i]))
        {
          ULONG width = TextLength(rport, text, i);
          RectFill(rport, x+width, y+2, x+width+TextLength(rport, &text[i], 1)-2, y+2);
          break;
        }

      }
    }
  }
  else
  {
    Text(rport, text, length);
  }
}

void Draw3DText (struct RastPort *rport, WORD x, WORD y, STRPTR text, char hotkey, UWORD textcolor, UWORD shadowcolor, BOOL parse)
{
  DrawText(rport, x+1, y+1, text, hotkey, shadowcolor, parse);
  DrawText(rport, x, y, text, hotkey, textcolor, parse);
}

VOID DrawButton(struct TB_Element *tool, struct Toolbar_Data *data, Object *obj, BOOL selected, BOOL three_d)
{
  // This routine depends heavily on selected being either 0 or 1.
  WORD x = tool->DstX, y  = tool->DstY;
  ULONG delta = (data->ButtonWidth - data->IconWidth)/2;
  struct RastPort *rport = &data->RP;
  UBYTE number = tool->Number;
  Object *par;
  ULONG back;

  LONG trans = selected ? data->SelectTrans : data->NormalTrans;
  struct BitMap *bitmap = selected ? data->SelectBM : data->NormalBM;
  PLANEPTR mask =  three_d ? data->GhostMask : ( selected ? data->SelectMask : data->NormalMask );

  // Clear background
  SetAPen(rport, three_d ? data->GhostTrans : trans);
//rf  RectFill(rport,  x+1, y+1, x+data->ButtonWidth-2+selected, y+data->ButtonHeight-2+selected);
  if (data->BorderType == BORDERTYPE_OLD) {
    RectFill(rport,  x+1, y+1, x+data->ButtonWidth-1, y+data->ButtonHeight-1);
  } else if (data->BorderType == BORDERTYPE_OFF) {
    get(obj, MUIA_Parent, &par);
    if (par) {
      get(par, MUIA_Background, &back);
      set(obj, MUIA_Background, back);
    }
    DoMethod(obj, MUIM_DrawBackground, x,   y, data->ButtonWidth, data->ButtonHeight, x, y);
  } else if (data->BorderType == BORDERTYPE_NEW) {
    RectFill(rport,  x+1, y+1, x+data->ButtonWidth-1, y+data->ButtonHeight-1);
  }

  // Borders

  if (data->BorderType == BORDERTYPE_OLD) {
    UpperFrame(rport, x,   y, x+data->ButtonWidth-2, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHADOW]);
    LowerFrame(rport, x+1, y, x+data->ButtonWidth-1, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHADOW]);
  } else  if (data->BorderType == BORDERTYPE_NEW) {
    UpperFrame(rport, x,   y, x+data->ButtonWidth-2, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHINE]);
    LowerFrame(rport, x+1, y, x+data->ButtonWidth-1, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHADOW]);
  }

  if (data->SelectionMode != SELECTIONMODE_OFF) {
    if(selected)
    {
      if (data->SelectionMode == SELECTIONMODE_OLD) {
        UpperFrame(rport, x+1, y+1, x+data->ButtonWidth-2, y+data->ButtonHeight-2, _pens(obj)[MPEN_HALFSHADOW]);
        UpperFrame(rport, x+2, y+2, x+data->ButtonWidth-2, y+data->ButtonHeight-2, _pens(obj)[MPEN_BACKGROUND]);
      } else {
        UpperFrame(rport, x, y, x+data->ButtonWidth-2, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHADOW]);
        LowerFrame(rport, x+1, y, x+data->ButtonWidth-1, y+data->ButtonHeight-1, _pens(obj)[MPEN_SHINE]);
      }
    } else  {
      if (data->BorderType == BORDERTYPE_OLD) {
        UpperFrame(rport, x+1, y+1, x+data->ButtonWidth-3, y+data->ButtonHeight-3, _pens(obj)[MPEN_SHINE]);
        LowerFrame(rport, x+2, y+2, x+data->ButtonWidth-2, y+data->ButtonHeight-2, _pens(obj)[MPEN_HALFSHADOW]);
      }
    }
  } else  {
    if (data->BorderType == BORDERTYPE_OLD) {
      UpperFrame(rport, x+1, y+1, x+data->ButtonWidth-3, y+data->ButtonHeight-3, _pens(obj)[MPEN_SHINE]);
      LowerFrame(rport, x+2, y+2, x+data->ButtonWidth-2, y+data->ButtonHeight-2, _pens(obj)[MPEN_HALFSHADOW]);
    }
  }

  if(data->ToolTexts)
  {
    BOOL parse = tool->ParseUnderscore;
    STRPTR tooltext = data->Desc[number].ToolText;
//    SetFont(rport, data->ToolFont); Flyttet til MUIM_Show
    ULONG width = TextLength(rport, tooltext, strlen(tooltext)) - (parse ? data->UnderscoreSize : 0);

    UWORD horiz_offset = selected + (data->ButtonWidth - width)/2;
    UWORD vert_offset  = selected + data->IconHeight + data->InnerSpace + 3;
    if(three_d)
      Draw3DText(rport, x + horiz_offset, y + vert_offset, tooltext, data->Desc[number].Key, data->HalfShadow, data->HalfShine, parse);
    else
      DrawText(rport, x + horiz_offset, y + vert_offset, tooltext, data->Desc[number].Key, MUIPEN(data->TextPen), parse);
  }

  if(data->ToolImages)
  {
    if(bitmap) {
      if (mask && (data->BorderType != BORDERTYPE_OLD)) {
        BltMaskBitMapRastPort((three_d ? data->GhostBM : bitmap), tool->SrcOffset, 0, rport, x+delta+selected, y+data->InnerSpace+2+selected, data->IconWidth, data->IconHeight, 0xE0, mask);
      } else {
        BltBitMapRastPort((three_d ? data->GhostBM : bitmap), tool->SrcOffset, 0, rport, x+delta+selected, y+data->InnerSpace+2+selected, data->IconWidth, data->IconHeight, 0xC0);
      }
    }
    else
    {
      x += data->ButtonWidth/2 + selected - 3;
      y += data->InnerSpace + data->IconHeight/2 + selected;
      SetAPen(rport, _pens(obj)[MPEN_HALFSHADOW]);
      RectFill(rport, x, y, x+4, y+4);
    }
  }
}

VOID DrawGhosted(struct TB_Element *tool, struct Toolbar_Data *data, Object *obj, BOOL selected)
{
  switch(data->GhostEffect)
  {
    case GHOST_LIGHT:
    case GHOST_HEAVY:
    {
      DrawButton(tool, data, obj, selected, !data->ToolImages);

      if(data->ToolImages)
      {
        struct RastPort *rport = &data->RP;
        UWORD ditherData[2];

        if(data->GhostEffect == GHOST_LIGHT)
        {
          ditherData[0] = 0x8888;
          ditherData[1] = 0x2222;
        }
        else
        {
          ditherData[0] = 0x5555;
          ditherData[1] = 0xAAAA;
        }

        SetAPen(rport, _pens(obj)[MPEN_SHADOW]);
        SetAfPt(rport, ditherData, 1);
        RectFill(rport, tool->DstX, tool->DstY, tool->DstX+data->ButtonWidth-1, tool->DstY + data->ButtonHeight-1);
        SetAfPt(rport, NULL, 0);
      }
    }
    break;
    case GHOST_SUPERLIGHT:
    {
      DrawButton(tool, data, obj, selected, THREE_D);

      if(data->ToolImages)
      {
        struct RastPort *rport = &data->RP;
        UWORD ditherData[] = {0x5555, 0xAAAA};
        SetAPen(rport, _pens(obj)[MPEN_HALFSHINE]);
        SetAfPt(rport, ditherData, 1);
        RectFill(rport, tool->DstX+2, tool->DstY+2, tool->DstX+data->ButtonWidth-3, tool->DstY+1+data->InnerSpace+data->IconHeight);
        SetAfPt(rport, NULL, 0);
      }
    }
    break;
    case GHOST_BITMAP:
//    case GHOST_DIMMED:
    {
      DrawButton(tool, data, obj, selected, THREE_D);
    }
    break;
  }
}

VOID DrawButtons(struct Toolbar_Data *data, Object *obj, BOOL complete)
{
  struct MUIP_Toolbar_Description *desc = data->Desc;

  Object *par;
  ULONG back;

  get(obj, MUIA_Parent, &par);
  if (par) {
    get(par, MUIA_Background, &back);
    set(obj, MUIA_Background, back);
  }

  if(data->SingleGone) // Just a single button which is going to disappear
  {
    struct TB_Element *tool = data->SingleGone;
    // Should the seperator-lines before and after be deleted?
    UBYTE before = 0;
    UBYTE after = 0;
    WORD x,y;

    if(data->ToolSpace < 0)
    {
      struct MinNode *node = (struct MinNode *)tool;
      struct MinNode *pred = node->mln_Pred;
      struct MinNode *succ = node->mln_Succ;

      if(data->ToolList.mlh_Head != node) // Not the first
        before =     (((struct TB_Element *)pred)->Type == TDT_SPACE && data->GroupSpace != -1)
              ||  (desc[((struct TB_Element *)pred)->Number].Flags & TDF_GONE)     ? 0 : 1;

      after = before + ( (((struct TB_Element *)succ)->Type == TDT_SPACE && data->GroupSpace != -1)
              ||     (desc[((struct TB_Element *)succ)->Number].Flags & TDF_GONE  )    ? 0 : 1);
    }

    x = tool->DstX;
    y = tool->DstY;

    if(data->Horizontal)
      DoMethod(obj, MUIM_DrawBackground, x + before, y, data->ButtonWidth-after, data->ButtonHeight, x+before, y, 0);
    else
      DoMethod(obj, MUIM_DrawBackground, x, y + before, data->ButtonWidth, data->ButtonHeight-after, x, y+before, 0);

    data->SingleGone = NULL;
  }
  else
  {
    UWORD drawback_begin = _mleft(obj);
    UWORD drawback_end = 0;

    struct MinList *list = (struct MinList *)&data->ToolList;
    struct MinNode *node;

    for(node = list->mlh_Head; node->mln_Succ; node = *(struct MinNode **)node)
    {
      struct TB_Element *elem = (struct TB_Element *)node;
      if(complete || elem->Redraw)
      {
        elem->Redraw = FALSE;
        if(elem->Type == TDT_BUTTON)
        {
          struct TB_Element *tool = (struct TB_Element *)elem;
          struct MUIP_Toolbar_Description tdesc = desc[tool->Number];
// KPrintF("Drawing button %ld at offset %ld\n", tool->Number, tool->DstOffset);

          if(tdesc.Flags & TDF_GONE)
          {
            drawback_end = tool->DstOffset + 1 + data->ButtonSize;
          }
          else
          {
            BOOL selected;

            if(drawback_begin<drawback_end)
            {
// KPrintF("Clearing area: %ld to %ld (%ld)\n", drawback_begin, drawback_end, drawback_end-drawback_begin);
              if(data->Horizontal)
                DoMethod(obj, MUIM_DrawBackground, drawback_begin, _mtop(obj), drawback_end-drawback_begin, data->ButtonHeight, drawback_begin, _mtop(obj), 0);
              else
                DoMethod(obj, MUIM_DrawBackground, _mleft(obj), drawback_begin, data->ButtonWidth, drawback_end-drawback_begin, _mleft(obj), drawback_begin, 0);
            }

            drawback_begin = tool->DstOffset + data->ButtonSize;

            if(data->ToolSpace > 0)
              drawback_end = drawback_begin + data->ToolSpace;

            selected = tdesc.Flags & TDF_SELECTED ? TRUE : FALSE;

            if(tdesc.Flags & TDF_GHOSTED)
                DrawGhosted(tool, data, obj, selected);
            else  DrawButton(tool, data, obj, selected, NOT_THREE_D);
          }
        }
        else // TDT_SPACE
        {
          struct TB_Element *space = (struct TB_Element *)elem;
          if(data->GroupSpace > 0)
            drawback_end = space->DstOffset + space->Size;
        }
      }
    }

    // In case of 'trailing' spaces...
    if(complete)
    {
      if(drawback_begin < (data->Horizontal ? _mright(obj) : _mbottom(obj)))
      {
        if(data->Horizontal)
          DoMethod(obj,MUIM_DrawBackground, drawback_begin, _mtop(obj), _mright(obj)-drawback_begin, data->ButtonHeight, drawback_begin, _mtop(obj), 0);
        else
          DoMethod(obj,MUIM_DrawBackground, _mleft(obj), drawback_begin, data->ButtonWidth, _mbottom(obj)-drawback_begin, _mleft(obj), drawback_begin, 0);
      }
    }
  }
}

ULONG Toolbar_Draw(struct IClass *cl, Object *obj, struct MUIP_Draw *msg)
{
  struct Toolbar_Data *data = (struct Toolbar_Data *)INST_DATA(cl, obj);

  DoSuperMethodA(cl, obj, (Msg)msg);

  if (msg->flags & MADF_DRAWUPDATE)
  {

    if(data->UpdateFlag)
      DrawButton(data->Update, data, obj, data->UpdateFlag == UPDATE_DOWN, NOT_THREE_D);

    if(data->RedrawFlag)
    {
      DrawButtons(data, obj, FALSE);
      data->RedrawFlag = FALSE;
    }
  }
  else if (msg->flags & MADF_DRAWOBJECT)
  {
    DrawButtons(data, obj, TRUE);
  }

  return(0);
}
