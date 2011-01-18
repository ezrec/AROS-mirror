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
 * Revision 42.0  2000/05/09 22:21:38  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:23  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:23  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:45  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:36  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
**********************  TREEVIEW CLASS: GETENTRY  ***********************
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

//ASM ULONG TV_GetEntry(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvGet *tvg);
ASM REGFUNCPROTO3(ULONG, TV_GetEntry,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvGet *, tvg));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
****************************  TV_GETENTRY()  ****************************
*************************************************************************
* Return an entry in the treeview. In the message structure for this
* method, the user supplies a reference to an entry and a code to
* indicate which entries to consider. This method returns the matching
* entry if successful, NULL on failure.
*
* Special values allowed for the reference entry are:
*	TV_ROOT			to represent the root of the tree (a dummy entry).
*	TV_SELECTED		for the (first) currently selected entry.
*
* If the relation entry is TV_SELECTED, but no entry is currently
* selected, nothing is done and zero is returned.
*
* Permitted values for the relationship code are as follows:
*	TVW_ENTRY				specified entry only (not useful)
*	TVW_PARENT				parent of entry
*	TVW_CHILD_FIRST			first child of entry
*	TVW_CHILD_LAST			last child of entry
*	TVW_SIBLING_FIRST		first sibling of entry
*	TVW_SIBLING_LAST		last sibling of entry
*	TVW_SIBLING_NEXT		next sibling of entry
*	TVW_SIBLING_PREV		prev. sibling of entry
*	TVW_TREE_NEXT			next in tree from entry
*	TVW_TREE_PREV			prev. in tree from entry
*	TVW_TREE_PAGE_UP		page up in tree from entry
*	TVW_TREE_PAGE_DOWN		page down in tree from entry
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes are
* permitted. To obtain all entries in the treeview via consecutive
* calls, pass TV_ROOT as the initial reference entry with TVW_TREE_NEXT
* as the code. Then make the return value from this method the next
* reference entry, until the return is NULL.
*
* Permitted bits for the flags parameter are:
*	TVF_SELECTED			affect only selected entries
*	TVF_VISIBLE				affect only visible entries
*
*************************************************************************/

//ASM ULONG TV_GetEntry(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvGet *tvg)
ASM REGFUNC3(ULONG, TV_GetEntry,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvGet *, tvg))
{
struct tvAnchor	tva;
TVData			*tv;
TNPTR			tn;
ULONG			rc;

TV_DebugDumpMethod((Msg) tvg);

tv = (TVData *) INST_DATA(cl,obj);
tn = NULL;
rc = 0;
memset(&tva,0,sizeof(tva));

/*
 * Find the treenode corresponding to this tvGet.
 */

if (tn = TV_MatchNextEntry(tv,tvg->tvg_Entry,tvg->tvg_Which,tvg->tvg_Flags,&tva))
	rc = (ULONG) tn->tn_Entry;

return(rc);
}
REGFUNC_END

