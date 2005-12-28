/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2005 Oliver Brunner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Oliver Brunner
 *
 * E-Mail: mui-gtk "at" oliver-brunner.de
 *
 * $Id: table.c,v 1.12 2005/12/05 19:05:28 afxgroup Exp $
 *
 *****************************************************************************/

/* table class
**
** emulates table widget in gtk
**
** supports:
**
** gtk_table_attach() (no options, no padding)
** gtk_table_attach_defaults()
** gtk_table_set_row_spacings()
** gtk_table_set_col_spacings()
** gtk_table_get_default_col_spacing()
** gtk_table_get_default_row_spacing()
**
** todo:
**
** gtk_table_set_homogeneous()
** gtk_table_get_col_spacing()
** gtk_table_get_row_spacing()
**
** in short, cells are homogenous
**
*/

#include <clib/alib_protos.h>
#include <proto/utility.h>
#include <exec/types.h>
#include <mui.h>

#include "classes.h"
#include "gtk_globals.h"
#include "gtk.h"
#include "debug.h"

/*********************************************************************/

struct Data
{
  struct Hook LayoutHook;
  struct MinList ChildList;

  ULONG columns, rows, vertspacing, horizspacing;
  LONG  defwidth, defheight;
};

struct TableNode
{
  struct MinNode node;
  GtkWidget *widget;
  UWORD left, right, top, bottom;
};

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new);

#ifndef __AROS__
HOOKPROTO(LayoutHook, ULONG, APTR obj, struct MUI_LayoutMsg *lm)
{
#else 
AROS_UFH3(static ULONG, LayoutHook, AROS_UFHA(struct Hook *, hook, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(struct MUI_LayoutMsg *, lm, a1))
#endif
  switch (lm->lm_Type)
  {
    case MUILM_MINMAX:
    {
      struct Data *data = hook->h_Data;
      struct TableNode *node;
      ULONG mincw, minch, defcw, defch, maxcw, maxch;

      mincw = 0;
      minch = 0;
      defcw = data->horizspacing;
      defch = data->vertspacing;
      maxcw = 0;
      maxch = 0;

      ForeachNode(&data->ChildList, node)
      {
        ULONG w, h, cols, rows;

        w = _minwidth(node->widget->MuiObject);
        h = _minheight(node->widget->MuiObject);

        cols = node->right - node->left;
        rows = node->bottom - node->top;

        if (cols)
        {
          ULONG s = w / cols;

          if (s > mincw)
          {
            mincw = s + (s % cols ? 1 : 0);
          }
        }

        if (rows)
        {
          ULONG s = h / rows;

          if (s > minch)
          {
            minch = s + (s % rows ? 1 : 0);
          }
        }

        // calculate default cell width/height

        w = _defwidth(node->widget->MuiObject);
        h = _defheight(node->widget->MuiObject);

        if (cols)
        {
          ULONG s = w / cols;

          if (s > defcw)
          {
            defcw = s + (s % cols ? 1 : 0);
          }
        }

        if (rows)
        {
          ULONG s = h / rows;

          if (s > defch)
          {
            defch = s + (s % rows ? 1 : 0);
          }
        }

        // calculate max cell width/height

        w = _maxwidth(node->widget->MuiObject);
        h = _maxheight(node->widget->MuiObject);

        if (cols)
        {
          ULONG s = w / cols;

          if (s > maxcw)
          {
            maxcw = s + (s % cols ? 1 : 0);
          }
        }

        if (rows)
        {
          ULONG s = h / rows;

          if (s > maxch)
          {
            maxch = s + (s % rows ? 1 : 0);
          }
        }
      }

      lm->lm_MinMax.MinWidth  = mincw * data->columns;
      lm->lm_MinMax.MinHeight = minch * data->rows;
      lm->lm_MinMax.DefWidth  = defcw * data->columns;
      lm->lm_MinMax.DefHeight = defch * data->rows;
      lm->lm_MinMax.MaxWidth  = maxcw * data->columns;
      lm->lm_MinMax.MaxHeight = maxch * data->rows;
    }
    return 0;

    case MUILM_LAYOUT:
    {
      struct Data *data = hook->h_Data;

      if (data->columns && data->rows)
      {
        struct TableNode *node;
        ULONG cw, ch;

        cw = lm->lm_Layout.Width / data->columns; 
        ch = lm->lm_Layout.Height / data->rows; /* +1 ? */

        ForeachNode(&data->ChildList, node)
        {
          if (!MUI_Layout(node->widget->MuiObject,
            node->left * cw,
            node->top * ch ,
            (node->right - node->left) * cw,
            (node->bottom - node->top ) * ch,
            0))
          {
            return FALSE;
          }
        }
      }
    }
    return TRUE;
  }

  return MUILM_UNKNOWN;
}

static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if (obj)
  {
    GETDATA;

    SETUPHOOK(&data->LayoutHook, LayoutHook, data);
//afxgroup - Is this a typo? (&data->ChildList)
    MGTK_NEWLIST(data->ChildList);

    data->vertspacing = 0;
    data->horizspacing = 0;
    data->defwidth = -1;
    data->defheight = -1;

    set(obj, MUIA_Group_LayoutHook, &data->LayoutHook);
    mSet(data, obj, (APTR)msg, TRUE);
  }

  return (ULONG)obj;
}

static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID)
  {
    case MA_Table_VertSpacing : rc = data->vertspacing; break;
    case MA_Table_HorizSpacing: rc = data->horizspacing; break;

    default: return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

static VOID mSet(struct Data *data, APTR obj, struct opSet *msg, ULONG is_new)
{
  struct TagItem *tstate, *tag;
  ULONG relayout = 0;

  tstate = msg->ops_AttrList;

  while ((tag = NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Table_Columns:
        data->columns = tag->ti_Data;
        break;

      case MA_Table_Rows:
        data->rows = tag->ti_Data;
        break;

      case MA_Table_VertSpacing:
        if (data->vertspacing != tag->ti_Data)
        {
          data->vertspacing = tag->ti_Data;
          relayout = 1;
        }
        break;

      case MA_Table_HorizSpacing:
        if (data->horizspacing != tag->ti_Data)
        {
          data->horizspacing = tag->ti_Data;
          relayout = 1;
        }
        break;

      case MA_DefWidth:
        if (data->defwidth != tag->ti_Data)
        {
          data->defwidth = tag->ti_Data;
          relayout = 1;
        }
        break;

      case MA_DefHeight:
        if (data->defheight != tag->ti_Data)
        {
          data->defheight = tag->ti_Data;
          relayout = 1;
        }
        break;
    }
  }

  if (relayout)
  {
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, MUIM_Group_ExitChange);
  }
}

static VOID mRemMember(struct Data *data, struct opMember *msg)
{
  struct TableNode *node;

  ForeachNode(&data->ChildList, node)
  {
    if (node->widget->MuiObject == msg->opam_Object)
    {
      REMOVE(node);
      mgtk_freemem(node, sizeof(*node));
      return;
    }
  }
}

static ULONG mAttach(struct Data *data, APTR obj, struct MUIP_Table_Attach *msg) {
  struct TableNode *node;
//afxgroup - msg redeclared as different kind of symbol

  DebOut("mAttach(..)\n");

  DebOut("msg=%lx\n",msg);

  node = mgtk_allocmem(sizeof(*node), MEMF_ANY);

  if (node)
  {
    ADDTAIL(&data->ChildList, node);

    node->widget = msg->widget;
    node->left = msg->left;
    node->right = msg->right;
    node->top = msg->top;
    node->bottom = msg->bottom;

    DebOut("  msg->widget: %lx\n",msg->widget);
    DebOut("  msg->widget->MuiObject: %lx\n",msg->widget->MuiObject);
    DebOut("  obj: %lx\n",obj);
    DoMethod(obj, MUIM_Group_InitChange);
    DoMethod(obj, OM_ADDMEMBER, msg->widget->MuiObject);
    DoMethod(obj, MUIM_Group_ExitChange);
  }

  return 0;
}

BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew           (cl, obj, msg);
  	case OM_SET         :        mSet           (data, obj, (APTR)msg, FALSE); break;
    case OM_GET         : return mGet           (data, obj, (APTR)msg, cl);
    case OM_REMMEMBER   :        mRemMember     (data,      (APTR)msg); break;
    case MM_Table_Attach: return mAttach        (data, obj, (APTR)msg);
  }

ENDMTABLE

int mgtk_create_table_class(void)
{
  CL_Table = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);
  return CL_Table ? 1 : 0;
}

void mgtk_delete_table_class(void)
{
  if (CL_Table)
  {
    MUI_DeleteCustomClass(CL_Table);
  }
}
