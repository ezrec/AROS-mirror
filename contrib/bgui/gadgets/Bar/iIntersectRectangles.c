/*
 * @(#) $Header$
 *
 * BGUI library
 *
 * (C) Copyright 2000 BGUI Developers Team.
 * (C) Copyright 1998 Manuel Lemos.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.11  2000/05/09 20:34:25  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:30  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:05  mlemos
 * Initial revision.
 *
 *
 */

#include <math.h>

#ifndef max
#define max(x,y) ((x)>(y) ? (x) : (y))
#endif

#ifndef min
#define min(x,y) ((x)<(y) ? (x) : (y))
#endif

#include "include/lGraphic.h"

lgBOOLEAN liIntersectRectangles(rectangle,intersected,intersection)
liRECTANGLE *rectangle,*intersected,*intersection;
{
	liBOUNDING_BOX intersection_box;

	intersection_box.LowerLeft.X=max(rectangle->Origin.X,intersected->Origin.X);
	intersection_box.UpperRight.X=min(rectangle->Origin.X+rectangle->Dimension.Width,intersected->Origin.X+intersected->Dimension.Width);
	if(intersection_box.LowerLeft.X>=intersection_box.UpperRight.X)
		return(lgFALSE);
	intersection_box.LowerLeft.Y=max(rectangle->Origin.Y,intersected->Origin.Y);
	intersection_box.UpperRight.Y=min(rectangle->Origin.Y+rectangle->Dimension.Height,intersected->Origin.Y+intersected->Dimension.Height);
	if(intersection_box.LowerLeft.Y>=intersection_box.UpperRight.Y)
		return(lgFALSE);
	intersection->Dimension.Width=intersection_box.UpperRight.X-(intersection->Origin.X=intersection_box.LowerLeft.X);
	intersection->Dimension.Height=intersection_box.UpperRight.Y-(intersection->Origin.Y=intersection_box.LowerLeft.Y);
	return(lgTRUE);
}
