/*
    Copyright © 2002-2003, The AROS Development Team. All rights reserved.
    $Id: miamipanel_funcs.c 19316 2003-08-20 10:35:20Z chodorowski $
*/

#include <proto/exec.h>

#include "miamipanel_intern.h"

/*****************************************************************************

    NAME */
	AROS_LH8(BOOL, MiamiPanelInit,

/*  SYNOPSIS */
	AROS_LHA(IPTR, callback1, A0),
	AROS_LHA(IPTR, callback2, A1),
	AROS_LHA(ULONG, showflags, D0),
	AROS_LHA(ULONG, font, D1),
	AROS_LHA(ULONG, unknown, D2),
	AROS_LHA(ULONG, panelx, D3),
	AROS_LHA(ULONG, panely, D4),
	AROS_LHA(ULONG, guimask, D5),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelCleanup */

/*****************************************************************************

    NAME */
	AROS_LH5(void, MiamiPanelAddInterface,

/*  SYNOPSIS */
	AROS_LHA(ULONG, if_index, D0),
	AROS_LHA(char *, ifname, A0),
	AROS_LHA(ULONG, ifstate, D1),
	AROS_LHA(ULONG, secs, D2),
	AROS_LHA(ULONG, ifspeed, D3),
	
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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelAddInterface */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelDelInterface,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelDelInterface */

/*****************************************************************************

    NAME */
	AROS_LH3(void, MiamiPanelSetInterfaceState,

/*  SYNOPSIS */
	AROS_LHA(ULONG, if_index, D0),
	AROS_LHA(ULONG, state, D1),
	AROS_LHA(ULONG, secs, D2),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelSetInterfaceState */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelSetInterfaceSpeed,

/*  SYNOPSIS */
	AROS_LHA(ULONG, if_index, D0),
	AROS_LHA(ULONG, ifspeed, D1),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelSetInterfaceSpeed */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelInterfaceReport,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelInterfaceReport */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelToFront,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelToFront */

/*****************************************************************************

    NAME */
	AROS_LH1(void, MiamiPanelInhibitRefresh,

/*  SYNOPSIS */
	AROS_LHA(BOOL, inihbit, D0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelInhibitRefresh */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelGetCoord,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelGetCoord */

/*****************************************************************************

    NAME */
	AROS_LH1(void, MiamiPanelEvent,

/*  SYNOPSIS */
	AROS_LHA(ULONG, signal, D0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelEvent */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelRefreshName,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelRefreshName */

/*****************************************************************************

    NAME */
	AROS_LH0(ULONG, MiamiPanelGetVersion,

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

	return 1;
	
    AROS_LIBFUNC_EXIT

} /* MiamiPanelGetVersion */

/*****************************************************************************

    NAME */
	AROS_LH2(void, MiamiPanelKill,

/*  SYNOPSIS */
	AROS_LHA(ULONG, imageid, D0),
	AROS_LHA(struct TagItem *, tags, A0),

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

    AROS_LIBFUNC_EXIT

} /* MiamiPanelKill */
