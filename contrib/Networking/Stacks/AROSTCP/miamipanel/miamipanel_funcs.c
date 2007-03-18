/*
    Copyright © 2002-2003, The AROS Development Team. All rights reserved.
    $Id: miamipanel_funcs.c 19316 2003-08-20 10:35:20Z chodorowski $
*/

#include <proto/exec.h>

#include "miamipanel_intern.h"

/*****************************************************************************

    NAME */
	AROS_LH8(LONG, MiamiPanelInit,

/*  SYNOPSIS */
	AROS_LHA(IPTR, synccb, A0),
	AROS_LHA(IPTR, asynccb, A1),
	AROS_LHA(LONG, flags, D0),
	AROS_LHA(STRPTR, font, A2),
	AROS_LHA(STRPTR, screen, A3),
	AROS_LHA(LONG, xo, D1),
	AROS_LHA(LONG, yo, D2),
	AROS_LHA(ULONG, sigbit, A4),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 5, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelInit()\n"));
	
	return TRUE;
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelInit */


/*****************************************************************************

    NAME */
	AROS_LH0(void, MiamiPanelCleanup,

/*  SYNOPSIS */

/*  LOCATION */
	struct Library *, MiamiPanelBase, 6, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelCleanup()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelCleanup */

/*****************************************************************************

    NAME */
	AROS_LH5(void, MiamiPanelAddInterface,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),
	AROS_LHA(STRPTR, name, A0),
	AROS_LHA(LONG, state, D1),
	AROS_LHA(LONG, ontime, D2),
	AROS_LHA(STRPTR, speed, A1),
	
/*  LOCATION */
	struct Library *, MiamiPanelBase, 7, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelAddInterface()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelAddInterface */

/*****************************************************************************

    NAME */
	AROS_LH1(void, MiamiPanelDelInterface,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 8, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelDelInterface()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelDelInterface */

/*****************************************************************************

    NAME */
	AROS_LH3(void, MiamiPanelSetInterfaceState,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),
	AROS_LHA(LONG, state, D1),
	AROS_LHA(LONG, ontime, D2),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 9, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelSetInterfaceState()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelSetInterfaceState */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelSetInterfaceSpeed,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),
	AROS_LHA(UBYTE *, speed, A0),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 10, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelSetInterfaceSpeed()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelSetInterfaceSpeed */

/*****************************************************************************

    NAME */
	AROS_LH5(void, MiamiPanelInterfaceReport,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),
	AROS_LHA(LONG, rate, D1),
	AROS_LHA(LONG, now, D2),
	AROS_LHA(LONG, totalhi, D3),
	AROS_LHA(ULONG, totallo, D4),
		
/*  LOCATION */
	struct Library *, MiamiPanelBase, 11, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelInterfaceReport()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelInterfaceReport */

/*****************************************************************************

    NAME */
	AROS_LH0(void, MiamiPanelToFront,

/*  SYNOPSIS */

/*  LOCATION */
	struct Library *, MiamiPanelBase, 12, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelToFront()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelToFront */

/*****************************************************************************

    NAME */
	AROS_LH1(void, MiamiPanelInhibitRefresh,

/*  SYNOPSIS */
	AROS_LHA(LONG, val, D0),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 13, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelInhibitRefresh()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelInhibitRefresh */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelGetCoord,

/*  SYNOPSIS */
	AROS_LHA(LONG *, xp, A0),
	AROS_LHA(LONG *, yp, A1),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 14, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelGetCoord()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelGetCoord */

/*****************************************************************************

    NAME */
	AROS_LH1(void, MiamiPanelEvent,

/*  SYNOPSIS */
	AROS_LHA(ULONG, sigs, D0),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 15, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelEvent()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelEvent */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelRefreshName,

/*  SYNOPSIS */
	AROS_LHA(LONG, unit, D0),
	AROS_LHA(UBYTE *, name, A0),

/*  LOCATION */
	struct Library *, MiamiPanelBase, 16, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelRefreshName()\n"));
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelRefreshName */

/*****************************************************************************

    NAME */
	AROS_LH0(LONG, MiamiPanelGetVersion,

/*  SYNOPSIS */

/*  LOCATION */
	struct Library *, MiamiPanelBase, 17, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelGetVersion()\n"));
	
	return 1;
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelGetVersion */

/*****************************************************************************

    NAME */
	AROS_LH0(ULONG, MiamiPanelKill,

/*  SYNOPSIS */

/*  LOCATION */
	struct Library *, MiamiPanelBase, 26, MiamiPanel)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

*****************************************************************************/
{
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,MiamiPanelBase)

D(bug("[MiamiPanel] MiamiPanelKill()\n"));

	return TRUE;
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelKill */
