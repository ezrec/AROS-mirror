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
 * $Id$
 *
 *****************************************************************************/

#include <stdlib.h>
#include <proto/utility.h>
#include <mui.h>

#include "debug.h"
#include "classes.h"
#include "gtk.h"
#include "gtk_globals.h"

struct Data
{
  GtkWidget *widget;
  Object *registergroup;
  Object **mychild;
  char   **mylabel;
  unsigned int nrchilds;
};

static Object *mgtk_note_new(Object **child,char **label) {
  Object *reg;
  int i;

  DebOut("mgtk_note_new(%lx,%lx)\n",child,label);

  reg=RegisterGroup(label),
          MUIA_Register_Frame, TRUE,
      End;
 
  i=0;
  while(label[i]) {
    DoMethod(reg,OM_ADDMEMBER,child[i]);
    i++;
  }

  return reg;
}

static void mgtk_note_add_child(Object *obj,struct Data *data,GtkWidget *newchild, int prepend) {
  struct List *list;
  struct Node *state;
  Object *reg;
  Object *o;
  Object **childs;
  char **labels;
  int i;

  DebOut("mgtk_add_child(%lx,%lx,%d)\n",obj,newchild,prepend);

  reg=data->registergroup;

  DoMethod(obj,MUIM_Group_InitChange);

  if(!data->nrchilds) {
    /* this is the first child, so remove dummy child */
    list=(struct List *) xget(reg,MUIA_Group_ChildList);
    state = list->lh_Head;
    o = NextObject( &state );
    DoMethod(reg,OM_REMMEMBER,o);
    MUI_DisposeObject(o);
    g_free(data->mylabel[0]);
    DebOut("  removed dummy object\n");
  }

  childs=g_new(Object *,(data->nrchilds)+2);
  labels=g_new(char *,(data->nrchilds)+2);

  /* copy old content, and remove children from old register */
  DebOut("  copy old content and remove children\n");
  i=prepend;

  while(i<prepend+data->nrchilds) {
    DebOut("   %d: %lx,%s\n",i,data->mychild[i],data->mylabel[i-prepend]);
    childs[i]=data->mychild[i-prepend];
    DoMethod(reg,OM_REMMEMBER,data->mychild[i-prepend]);
    labels[i]=data->mylabel[i-prepend];
    i++;
  }
  DebOut("  add new child\n");
  DebOut("    newchild->MuiObject=%lx\n",newchild->MuiObject);
  DebOut("    newchild->title=>%s<\n",newchild->title);

#warning g_strdup(2+newchild->title) .. why +2 ??

  if(!prepend) {
    childs[i]=newchild->MuiObject;
    if(newchild->title) {
      /* ??? TODO : labels[i]=g_strdup(2+newchild->title);*/
      labels[i]=g_strdup(2+newchild->title);
    }
    else {
      labels[i]=g_strdup_printf("page %d",i+1);
    }
  }
  else {
    childs[0]=newchild->MuiObject;
    if(newchild->title) {
      /* ??? TODO : labels[0]=g_strdup(2+newchild->title);*/
      labels[0]=g_strdup(2+newchild->title);
    }
    else {
      labels[0]=g_strdup_printf("page 0");
    }
  }

  i++;
  childs[i]=NULL;
  labels[i]=NULL;

  /* now we have everything,
   * so get rid of the old (now empty) register 
   */

  DoMethod(obj,OM_REMMEMBER,reg);
  MUI_DisposeObject(reg);
  g_free(data->mychild);
  g_free(data->mylabel);

  /* create and add new reg */
  reg=mgtk_note_new(childs,labels);
  DoMethod(obj,OM_ADDMEMBER,reg);

  data->nrchilds++;

  data->mychild=childs;
  data->mylabel=labels;

  DoMethod(obj,MUIM_Group_ExitChange);
}

static void mgtk_append_child(Object *obj,struct Data *data,GtkWidget *newchild) {
  mgtk_note_add_child(obj,data,newchild, FALSE);
}

static void mgtk_prepend_child(Object *obj,struct Data *data,GtkWidget *newchild) {
  mgtk_note_add_child(obj,data,newchild, TRUE);
}

/*******************************************
 * MUIHook_notebook
 *******************************************/
HOOKPROTO(MUIHook_notebook, ULONG, MGTK_HOOK_OBJECT_POINTER obj, MGTK_HOOK_APTR hookpointer)
{
  MGTK_USERFUNC_INIT
  GtkWidget *widget;

  DebOut("MUIHook_notebook(%x) called\n", (int) obj);
  widget=mgtk_get_widget_from_obj(obj);

/*TODO
  mgtk_notebook_value_update(widget);
  */

  return 0;
  MGTK_USERFUNC_EXIT
}
MakeHook(MyMuiHook_notebook, MUIHook_notebook);

/*******************************************
 * mNew
 * 
 * requires MA_Widget!
 * we create a empty Notebook, as MUI does
 * not like empty Registers, we fill it
 * with a dummy.
 *******************************************/
static ULONG mNew(struct IClass *cl, APTR obj, Msg msg)
{
  GtkWidget *widget;
  struct TagItem *tstate, *tag;
  Object *registergroup;
  Object **childs;
  char **labels;

  DebOut("mNew (notebook)\n");

  obj = (APTR)DoSuperMethodA(cl, obj, msg);

  if(!obj) {
    ErrOut("notebook: unable to create object!");
    return (ULONG) NULL;
  }

  DebOut("  new obj=%lx\n",obj);

  tstate=((struct opSet *)msg)->ops_AttrList;

  widget=NULL;
  while ((tag = (struct TagItem *) NextTagItem((APTR) &tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Widget:
        widget = (GtkWidget *) tag->ti_Data;
        break;
    }
  }

  if(!widget) {
    ErrOut("classes/spinbutton.c: mNew: MA_Widget not supplied!\n");
    return (ULONG) NULL;
  }

  DebOut("  widget=%d\n",widget);

  /* setup internal hooks */
/* TODO   DoMethod(text,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime, text,2,MUIM_CallHook,&MyMuiHook_spinbutton);*/

  childs=g_new(Object *,2);
  labels=g_new(char *,2);

#ifdef MGTK_DEBUG
  childs[0]=TextObject,MUIA_Text_Contents,"DUMMY OBJECT",End;
  labels[0]=g_strdup("DUMMY LABEL");
#else
  childs[0]=TextObject,MUIA_Text_Contents,"",End;
  labels[0]=g_strdup("");
#endif

  childs[1]=NULL;
  labels[1]=NULL;

  registergroup=mgtk_note_new(childs,labels);

  DoMethod(obj,OM_ADDMEMBER,registergroup);

  if (obj)
  {
    GETDATA;

    data->widget=widget;
    data->registergroup=registergroup;
    data->mylabel=labels;
    data->mychild=childs;
    data->nrchilds=0;
  }

  return (ULONG)obj;
}

/*******************************************
 * mSet
 * 
 * MA_Spin_Value only usefull TAG here
 * MA_Spin_String_Value *private*
 *******************************************/
static VOID mSet(struct Data *data, APTR obj, struct opSet *msg) {
  struct TagItem *tstate, *tag;

  DebOut("mSet(%lx,%lx,%lx)\n",data,obj,msg);

  tstate = msg->ops_AttrList;

  while ((tag = (struct TagItem *) NextTagItem(&tstate)))
  {
    switch (tag->ti_Tag)
    {
      case MA_Note_Append_Child:
        /* data needs to be a widget */
        DebOut("mSet append to %lx: %lx\n",obj,tag->ti_Data);
        mgtk_append_child(obj,data,(GtkWidget *) tag->ti_Data);
        break;
      case MA_Note_Prepend_Child:
        /* data needs to be a widget */
        DebOut("mSet append to %lx: %lx\n",obj,tag->ti_Data);
        mgtk_prepend_child(obj,data,(GtkWidget *) tag->ti_Data);
        break;
      case MUIA_Group_ActivePage:
        set(data->registergroup,MUIA_Group_ActivePage,tag->ti_Data);
        break;
    }

  }
}

/*******************************************
 * mGet
 * 
 * MA_Widget: GtkWidget of this spin
 *******************************************/
static ULONG mGet(struct Data *data, APTR obj, struct opGet *msg, struct IClass *cl)
{
  ULONG rc;

  switch (msg->opg_AttrID) {
    case MA_Widget : 
      DebOut("mGet: data->widget=%x\n",(int) data->widget);
      rc = (ULONG) data->widget;
      break;
    case MUIA_Group_ActivePage:
      return xget(data->registergroup,MUIA_Group_ActivePage);
      break;
    default: 
      return DoSuperMethodA(cl, obj, (Msg)msg);
  }

  *msg->opg_Storage = rc;
  return TRUE;
}

/*******************************************
 * Dispatcher
 *******************************************/
BEGINMTABLE
GETDATA;

  switch (msg->MethodID)
  {
    case OM_NEW         : return mNew       (cl, obj, msg);
    case OM_SET         :        mSet       (data, obj, (APTR)msg); break;
    case OM_GET         : return mGet       (data, obj, (APTR)msg, cl);
  }

ENDMTABLE

/*******************************************
 * Custom class create/delete
 *******************************************/
int mgtk_create_notebook_class(void)
{
  DebOut("mgtk_create_notebook_class()\n");

  CL_NoteBook = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct Data), (APTR)&mDispatcher);

  DebOut("CL_NoteBook=%lx\n",CL_NoteBook);

  return CL_NoteBook ? 1 : 0;
}

void mgtk_delete_notebook_class(void)
{
  if (CL_NoteBook)
  {
    MUI_DeleteCustomClass(CL_NoteBook);
  }
}
