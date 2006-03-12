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

#ifndef GTKMUI_CLASSES_H
#define GTKMUI_CLASSES_H

#ifndef SDI_HOOK_H
#ifndef __AROS__ /* AROS misses SDI_hook.h ..? */
#define MGTK_HOOK_OBJECT_POINTER Object *
#define MGTK_HOOK_APTR APTR
#define MGTK_USERFUNC_INIT
#define MGTK_USERFUNC_EXIT
#include <SDI_hook.h>
#else
#include "mgtk_aros.h"
#endif
#endif

#ifndef MUII_ImageButtonBack
#define MUII_ImageButtonBack MUII_ButtonBack
#endif

#include "gtk.h"

/* Some great VaporWare ideas.
**
*/

#define GETDATA struct Data *data = (struct Data *)INST_DATA(cl, obj)
#define DOSUPER DoSuperMethodA(cl, obj, (Msg)msg);

/**********************************************************************
  Some macros
**********************************************************************/

#ifdef __AMIGAOS4__
  /* does this work ? */
  #define MGTK_NEWLIST(MyList) NEWMINLIST(MyList)
#else /* !OS4 */
  #ifndef NEWLIST
    #define	NEWLIST(MyList)	\
    do { \
      struct MinList *_MyList = (struct MinList *)(MyList); \
      _MyList->mlh_TailPred = (struct MinNode *)_MyList; \
      _MyList->mlh_Tail = (struct MinNode *)NULL; \
      _MyList->mlh_Head = (struct MinNode *)&_MyList->mlh_Tail; \
    } while (0)
  #endif /* !NEWLIST */
  #ifndef MGTK_NEWLIST
    #define MGTK_NEWLIST(MyList) NEWLIST(&MyList)
  #endif
#endif /* OS4 */

#ifndef ADDTAIL
#define	ADDTAIL(MyList,MyNode) \
do { \
	struct MinList *_MyList = (struct MinList *)(MyList); \
	struct MinNode *_MyNode = (struct MinNode *)(MyNode); \
	struct MinNode *OldPredNode; \
	OldPredNode = _MyList->mlh_TailPred; \
	_MyNode->mln_Succ = (struct MinNode *)&_MyList->mlh_Tail; \
	_MyNode->mln_Pred = OldPredNode; \
	OldPredNode->mln_Succ = _MyNode; \
	_MyList->mlh_TailPred = _MyNode; \
} while (0)
#endif /* !ADDTAIL */

#ifndef REMOVE
#define	REMOVE(MyNode) \
({ \
	struct MinNode *_MyNode = (struct MinNode *)(MyNode); \
	struct MinNode *PredNode; \
	struct MinNode *SuccNode; \
	PredNode = _MyNode->mln_Pred; \
	SuccNode = _MyNode->mln_Succ; \
	PredNode->mln_Succ = SuccNode; \
	SuccNode->mln_Pred = PredNode; \
	_MyNode; \
})
#endif /* !REMOVE */

#ifndef ForeachNode
#define ForeachNode(l,n)  \
for (  \
	n = (void *)(((struct List *)(l))->lh_Head);  \
	((struct Node *)(n))->ln_Succ;  \
	n = (void *)(((struct Node *)(n))->ln_Succ)  \
)
#endif


#ifdef __MORPHOS__
/*************** MorphOS ******************/

  #define DISPATCHERARG struct IClass *cl = (struct IClass *)REG_A0; APTR obj = (APTR)REG_A2; Msg msg = (Msg)REG_A1;
  #define BEGINMTABLE static ULONG mDispatcherPPC(void); static struct EmulLibEntry mDispatcher = { TRAP_LIB, 0, (void (*)())&mDispatcherPPC }; static ULONG mDispatcherPPC(void) { DISPATCHERARG
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)&HookEntry; h->h_SubEntry = (HOOKFUNC)&func; h->h_Data = (APTR)data; }

#else 

#ifdef __AROS__
/*************** AROS ********************/

  #define BEGINMTABLE AROS_UFH3(static ULONG,mDispatcher,AROS_UFHA(struct IClass*, cl, a0), AROS_UFHA(APTR, obj, a2), AROS_UFHA(Msg, msg, a1))
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)&func; h->h_Data = (APTR)data; }

#else
/************* AmigaOS 3/4 ***************/
  #define BEGINMTABLE static ULONG mDispatcher(REG(a0, struct IClass *cl), REG(a2, APTR obj), REG(a1, Msg msg)) {

/************* AmigaOS 4 ***************/
#ifdef __AMIGAOS4__
  #define SETUPHOOK(x, func, data) { struct Hook *h = (x); h->h_Entry = (HOOKFUNC)func; h->h_Data = (APTR)data; }
#else 
/************* AmigaOS 3 ***************/

#endif 
#endif
#endif

#define ENDMTABLE return DoSuperMethodA(cl, obj, msg); }

/**********************************************************************
	Classes
**********************************************************************/

int  mgtk_create_rootgroup_class(void);
void mgtk_delete_rootgroup_class(void);
int  mgtk_create_table_class(void);
void mgtk_delete_table_class(void);
int  mgtk_create_spinbutton_class(void);
void mgtk_delete_spinbutton_class(void);
int  mgtk_create_notebook_class(void);
void mgtk_delete_notebook_class(void);
int  mgtk_create_progressbar_class(void);
void mgtk_delete_progressbar_class(void);
int  mgtk_create_timer_class(void);
void mgtk_delete_timer_class(void);
int  mgtk_create_fixed_class(void);
void mgtk_delete_fixed_class(void);
int  mgtk_create_checkbutton_class(void);
void mgtk_delete_checkbutton_class(void);
int  mgtk_create_togglebutton_class(void);
void mgtk_delete_togglebutton_class(void);
int  mgtk_create_radiobutton_class(void);
void mgtk_delete_radiobutton_class(void);
int  mgtk_create_button_class(void);
void mgtk_delete_button_class(void);
int  mgtk_create_menu_class(void);
void mgtk_delete_menu_class(void);
int  mgtk_create_entry_class(void);
void mgtk_delete_entry_class(void);
int  mgtk_create_toolbar_class(void);
void mgtk_delete_toolbar_class(void);
int  mgtk_create_custom_class(void);
void mgtk_delete_custom_class(void);


ULONG mgtk_askminmax(struct IClass *cl, APTR obj, struct MUIP_AskMinMax *msg, LONG defwidth, LONG defheight);

/**********************************************************************
	Tags & methods
**********************************************************************/

#define GTK_TAGBASE 0xfece0000

enum
{
	MA_RootGroup_Resizable = GTK_TAGBASE,

  /* These are extensions to standard area class tags */

  MA_DefWidth,
  MA_DefHeight,

  /* Tables */

  MM_Table_Attach,
  MA_Table_Columns,
  MA_Table_Rows,
  MA_Table_VertSpacing,
  MA_Table_HorizSpacing,

  /* common */
  MA_Widget,

  /* spinbutton */
  MA_Spin_Value,
  MA_Spin_String_Value,

  /* notebook */
  MA_Note_Append_Child,
  MA_Note_Prepend_Child,
  MA_Note_Label,
  MA_Note_Get_Child_Nr,

  /* timer */
  MA_Timer_Intervall,
  MA_Timer_Function,
  MA_Timer_Data,
  MUIM_Class5_Trigger,

  /* progressbar */
  MA_Pulse,

  /* fixed */
  MA_Fixed_Move,

  /* checkbutton */
  MA_Check_Label,
  /* radiobutton */
  MA_Radio_Label,

  /* checkbutton */
  /* togglebutton */
  /* radiobutton */
  MA_Toggle_Value,

  /* button */
  MA_Button_Label,

};

struct MUIP_Table_Attach { 
  ULONG MethodID; 
  GtkWidget *widget; 
  ULONG left; 
  ULONG right; 
  ULONG top; 
  ULONG bottom; 
};

struct MA_Fixed_Move_Data {
  GtkWidget *widget;
  int x;
  int y;
};

GtkWidget *mgtk_get_widget_from_obj(Object *obj);

#endif /* GTKMUI_CLASSES_H */

