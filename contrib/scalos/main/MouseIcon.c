// Gadgets.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <utility/hooks.h>
#include <intuition/gadgetclass.h>
#include <intuition/newmouse.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <dos/dostags.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

//----------------------------------------------------------------------------

// local functions

//----------------------------------------------------------------------------

// check whether (x, y) lies inside inner Bounds of icon (Left, Top, Width, Height)
struct ScaIconNode *CheckMouseIcon(struct ScaIconNode **IconList, 
	struct internalScaWindowTask *iwt,
	LONG x, LONG y)
{
	struct ScaIconNode *in;
	LONG x0, y0;

	d1(kprintf("\n" "%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	x0 = x - iwt->iwt_WindowTask.wt_XOffset;
	y0 = y - iwt->iwt_WindowTask.wt_YOffset;

	if (x0 < 0 || x0 >= iwt->iwt_InnerWidth
		|| y0 < 0 || y0 >= iwt->iwt_InnerHeight)
		return NULL;	// Mouse Click was outside of inner window borders

	for (in=*IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		d1(kprintf("%s/%s/%ld:  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));
		d1(kprintf("%s/%s/%ld:  x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, gg->LeftEdge, gg->TopEdge, gg->Width, gg->Height));

		if (x >= gg->LeftEdge && y >= gg->TopEdge
			&& x < (gg->LeftEdge + gg->Width)
			&& y < (gg->TopEdge + gg->Height))
			{
			ULONG Hit = GMR_GADGETHIT;

			d1(kprintf("%s/%s/%ld: icon found,  prefClickTransp=%ld\n", __FILE__, __FUNC__, __LINE__, prefClickTransp));

			if (CurrentPrefs.pref_ClickTransp || !IsIwtViewByIcon(iwt))
				{
				struct gpHitTest ht = { GM_HITTEST, NULL, { 0, 0 } };

				ht.gpht_Mouse.X = x;
				ht.gpht_Mouse.Y = y;

				Hit = DoMethodA((Object *) gg, (Msg)(APTR) &ht);
				}

			d1(kprintf("%s/%s/%ld: icon found,  Hit=%ld\n", __FILE__, __FUNC__, __LINE__, Hit));

			if (GMR_GADGETHIT == Hit)
				return in;
			}
		}

	d1(kprintf("%s/%s/%ld:  no icon found\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
}


// check whether (x, y) lies inside outer Bounds of icon (BoundsLeft, BoundsTop, BoundsWidth, BoundsHeight)
struct ScaIconNode *CheckMouseIconOuterBounds(struct ScaIconNode **IconList, 
	LONG x, LONG y)
{
	struct ScaIconNode *in;

	d1(kprintf("\n" "%s/%s/%ld: x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, x, y));

	for (in=*IconList; in; in = (struct ScaIconNode *) in->in_Node.mln_Succ)
		{
		struct ExtGadget *gg = (struct ExtGadget *) in->in_Icon;

		d1(kprintf("%s/%s/%ld:  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));
		d1(kprintf("%s/%s/%ld:  x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
			gg->BoundsLeftEdge, gg->BoundsTopEdge, gg->BoundsWidth, gg->BoundsHeight));

		if (x >= gg->BoundsLeftEdge && y >= gg->BoundsTopEdge
			&& x < (gg->BoundsLeftEdge + gg->BoundsWidth)
			&& y < (gg->BoundsTopEdge + gg->BoundsHeight))
			{
			d1(kprintf("%s/%s/%ld:  in=%08lx  <%s>\n", __FILE__, __FUNC__, __LINE__, in, in->in_Name ? in->in_Name : (STRPTR) ""));
			d1(kprintf("%s/%s/%ld:  x=%ld  y=%ld  w=%ld  h=%ld\n", __FILE__, __FUNC__, __LINE__, \
				gg->BoundsLeftEdge, gg->BoundsTopEdge, gg->BoundsWidth, gg->BoundsHeight));

			d1(kprintf("%s/%s/%ld: icon found,  Hit=%ld\n", __FILE__, __FUNC__, __LINE__));
			return in;
			}
		}

	d1(kprintf("%s/%s/%ld:  no icon found\n", __FILE__, __FUNC__, __LINE__));

	return NULL;
}

