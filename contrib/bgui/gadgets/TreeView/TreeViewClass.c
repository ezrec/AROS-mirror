/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.6  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.5  2003/01/18 19:10:20  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.4  2002/02/19 22:08:06  stegerg
 * The demo programs for the external bgui gadget classes did not work,
 * because they were not linked against autoinit lib. And in TreeViewClass.c
 * there was a printf -> crash.
 *
 * Revision 42.3  2000/08/09 11:48:33  chodorowski
 * Fixed missing prototypes.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:22:14  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:36:02  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:57  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:20:55  mlemos
 * Made the class be used as gadget shared library.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:16  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***************************  TREEVIEW CLASS  ****************************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

#include <stdio.h>

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Method functions in TVNewDispose, TVGetSet, etc:
 */

//extern ASM ULONG TV_New(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops);
extern ASM REGFUNCPROTO3(ULONG, TV_New,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops));

//extern ASM ULONG TV_Dispose(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg);
extern ASM REGFUNCPROTO3(ULONG, TV_Dispose,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg));

//extern ASM ULONG TV_Get(REG(a0) Class *cl, REG(a2) Object *obj,REG(a1) struct opGet *opg);
extern ASM REGFUNCPROTO3(ULONG, TV_Get,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg));

//extern ASM ULONG TV_Set(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops);
extern ASM REGFUNCPROTO3(ULONG, TV_Set,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops));

//extern ASM ULONG TV_GoActive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi);
extern ASM REGFUNCPROTO3(ULONG, TV_GoActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi));

//extern ASM ULONG TV_HandleInput(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi);
extern ASM REGFUNCPROTO3(ULONG, TV_HandleInput,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi));

//extern ASM ULONG TV_GoInactive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpGoInactive *gpgi);
extern ASM REGFUNCPROTO3(ULONG, TV_GoInactive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpGoInactive *, gpgi));

//extern ASM ULONG TV_Insert(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi);
extern ASM REGFUNCPROTO3(ULONG, TV_Insert,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi));

//extern ASM ULONG TV_Remove(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
extern ASM REGFUNCPROTO3(ULONG, TV_Remove,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

//extern ASM ULONG TV_Replace(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvReplace *tvr);
extern ASM REGFUNCPROTO3(ULONG, TV_Replace,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvReplace *, tvr));

//extern ASM ULONG TV_Move(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi);
extern ASM REGFUNCPROTO3(ULONG, TV_Move,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi));

//extern ASM ULONG TV_GetEntry(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvGet *tvg);
extern ASM REGFUNCPROTO3(ULONG, TV_GetEntry,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvGet *, tvg));

//extern ASM ULONG TV_Select(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
extern ASM REGFUNCPROTO3(ULONG, TV_Select,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

//extern ASM ULONG TV_Visible(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
extern ASM REGFUNCPROTO3(ULONG, TV_Visible,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

//extern ASM ULONG TV_Expand(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
extern ASM REGFUNCPROTO3(ULONG, TV_Expand,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

//extern ASM ULONG TV_Clear(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Clear,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//extern ASM ULONG TV_Lock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg);
extern ASM REGFUNCPROTO3(ULONG, TV_Lock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg));

//extern ASM ULONG TV_Unlock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Unlock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//extern ASM ULONG TV_Sort(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Sort,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//extern ASM ULONG TV_Redraw(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Redraw,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//extern ASM ULONG TV_Refresh(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Refresh,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//extern ASM ULONG TV_Rebuild(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
extern ASM REGFUNCPROTO3(ULONG, TV_Rebuild,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   OM_NEW,           TV_New,
   OM_DISPOSE,       TV_Dispose,
   OM_GET,           TV_Get,
   OM_SET,           TV_Set,
   OM_UPDATE,        TV_Set,
   GM_GOACTIVE,      TV_GoActive,
   GM_HANDLEINPUT,   TV_HandleInput,
   GM_GOINACTIVE,    TV_GoInactive,
   TVM_INSERT,       TV_Insert,
   TVM_REMOVE,       TV_Remove,
   TVM_REPLACE,      TV_Replace,
   TVM_MOVE,         TV_Move,
   TVM_GETENTRY,     TV_GetEntry,
   TVM_SELECT,       TV_Select,
   TVM_VISIBLE,      TV_Visible,
   TVM_EXPAND,       TV_Expand,
   TVM_CLEAR,        TV_Clear,
   TVM_LOCK,         TV_Lock,
   TVM_UNLOCK,       TV_Unlock,
   TVM_SORT,         TV_Sort,
   TVM_REDRAW,       TV_Redraw,
   TVM_REFRESH,      TV_Refresh,
   TVM_REBUILD,      TV_Rebuild,
   DF_END,           NULL,
};

#ifdef __AROS__
UBYTE _LibName[]   = "bgui_treeview.gadget";
UBYTE _LibID[]     = "\0$VER: bgui_treeview.gadget 41.10 (29.5.99) ©1996 Nick Christie ©1999 BGUI Developers Team";
UWORD _LibVersion  = 41;
UWORD _LibRevision = 10;
#else
extern UBYTE _LibName[]   = "bgui_treeview.gadget";
extern UBYTE _LibID[]     = "\0$VER: bgui_treeview.gadget 41.10 (29.5.99) ©1996 Nick Christie ©1999 BGUI Developers Team";
extern UWORD _LibVersion  = 41;
extern UWORD _LibRevision = 10;
#endif

/*--------------------------------LIBARY CODE FOLLOWS-----------------------------------*/

/*
 * The following code is used for the shared library.
 */
static Class *ClassBase = NULL;

/*
 * Initialize the class.
 */
SAVEDS ASM Class *BGUI_ClassInit(void)
{
   ClassBase = BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_GROUP_GADGET,
			    CLASS_ObjectSize,     sizeof(TVData),
			    CLASS_DFTable,        ClassFunc,
			    TAG_DONE);
   return ClassBase;
}

/*
 * Called each time the library is closed. It simply closes
 * the required libraries and frees the class.
 */
SAVEDS ASM BOOL BGUI_ClassFree(void)
{
   return BGUI_FreeClass(ClassBase);
}

