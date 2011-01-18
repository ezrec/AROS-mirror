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
 * Revision 42.0  2000/05/09 22:21:43  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:28  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:27  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:49  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:42  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********  TREEVIEW CLASS: GOACTIVE/HANDEINPUT/GOINACTIVE  ************
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

//ASM ULONG TV_GoActive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi);
ASM REGFUNCPROTO3(ULONG, TV_GoActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi));

//ASM ULONG TV_HandleInput(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi);
ASM REGFUNCPROTO3(ULONG, TV_HandleInput,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi));


//ASM ULONG TV_GoInactive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpGoInactive *gpgi);
ASM REGFUNCPROTO3(ULONG, TV_GoInactive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpGoInactive *, gpgi));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/

/************************************************************************
****************************  TV_GOACTIVE()  ****************************
*************************************************************************
* We intercept the goactive msg in order to record the coordinates of
* LMB down events and pass them on to the embedded listview.
*
*************************************************************************/

//ASM ULONG TV_GoActive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi)
ASM REGFUNC3(ULONG, TV_GoActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
TVData		*tv;
ULONG		rc;

tv = (TVData *) INST_DATA(cl,obj);
rc = GMR_NOREUSE;
tv->tv_GoingActive = TRUE;
tv->tv_ImageClicked = NULL;

/*
 * Refuse to go active if disabled.
 */

if (!(((GADPTR) obj)->Flags & GFLG_DISABLED))
	{
	if (gpi->gpi_IEvent && (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE) &&
			(gpi->gpi_IEvent->ie_Code == SELECTDOWN))
		{
		tv->tv_LastClickX = gpi->gpi_Mouse.X + ((GADPTR) obj)->LeftEdge;
		tv->tv_LastClickY = gpi->gpi_Mouse.Y + ((GADPTR) obj)->TopEdge;

		/* debug
		KPrintF("TV_GoActive: SelectDown: %ld,%ld\n",tv->tv_LastClickX,
				tv->tv_LastClickY);
		*/
		}

	rc = DoMethodA(tv->tv_Listview,(Msg) gpi);
	}

tv->tv_GoingActive = FALSE;

return(rc);
}
REGFUNC_END

/************************************************************************
**************************  TV_HANDLEINPUT()  ***************************
*************************************************************************
* We intercept this method only to pass it on to the listview.
*
*************************************************************************/

//ASM ULONG TV_HandleInput(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpInput *gpi)
ASM REGFUNC3(ULONG, TV_HandleInput,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpInput *, gpi))
{
TVData	*tv;

tv = (TVData *) INST_DATA(cl,obj);

return(DoMethodA(tv->tv_Listview,(Msg) gpi));
}
REGFUNC_END

/************************************************************************
***************************  TV_GOINACTIVE()  ***************************
************************************************************************/

//ASM ULONG TV_GoInactive(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct gpGoInactive *gpgi)
ASM REGFUNC3(ULONG, TV_GoInactive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct gpGoInactive *, gpgi))
{
TVData	*tv;

tv = (TVData *) INST_DATA(cl,obj);

return(DoMethodA(tv->tv_Listview,(Msg) gpgi));
}
REGFUNC_END

