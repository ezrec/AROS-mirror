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
 * Revision 42.2  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:53  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:39  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:38  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:52  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:53  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
************************  TREEVIEW CLASS: MOVE  *************************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Functions from TVUtil are listed in TVUtil.h
 */

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

//ASM ULONG TV_Move(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi);
ASM REGFUNCPROTO3(ULONG, TV_Move,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
******************************  TV_MOVE()  ******************************
************************************************************************/

//ASM ULONG TV_Move(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi)
ASM REGFUNC3(ULONG, TV_Move,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi))
{
return(0);
}
REGFUNC_END

