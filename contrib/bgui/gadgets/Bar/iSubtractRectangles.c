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
 * Revision 41.11  2000/05/09 20:34:28  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:32  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:08  mlemos
 * Initial revision.
 *
 *
 */

#include "include/lGraphic.h"

lgCARDINAL liSubtractRectangles(rectangle_1,rectangle_2,subtraction)
liRECTANGLE *rectangle_1,*rectangle_2,*subtraction;
{
	liRECTANGLE intersection;
	lgCARDINAL rectangles;

	if(liIntersectRectangles(rectangle_1,rectangle_2,&intersection)==lgFALSE)
	{
		subtraction[0]= *rectangle_1;
		return(1);
	}
	rectangles=0;
	if(intersection.Origin.Y>rectangle_1->Origin.Y)
	{
		subtraction[0].Origin=rectangle_1->Origin;
		subtraction[0].Dimension.Width=rectangle_1->Dimension.Width;
		subtraction[0].Dimension.Height=intersection.Origin.Y-rectangle_1->Origin.Y;
		rectangles=1;
	}
	if(intersection.Origin.X>rectangle_1->Origin.X)
	{
		subtraction[rectangles].Origin.X=rectangle_1->Origin.X;
		subtraction[rectangles].Origin.Y=intersection.Origin.Y;
		subtraction[rectangles].Dimension.Width=intersection.Origin.X-rectangle_1->Origin.X;
		subtraction[rectangles].Dimension.Height=intersection.Dimension.Height;
		rectangles++;
	}
	if(rectangle_1->Origin.X+rectangle_1->Dimension.Width>intersection.Origin.X+intersection.Dimension.Width)
	{
		subtraction[rectangles].Origin.X=intersection.Origin.X+intersection.Dimension.Width;
		subtraction[rectangles].Origin.Y=intersection.Origin.Y;
		subtraction[rectangles].Dimension.Width=rectangle_1->Origin.X+rectangle_1->Dimension.Width-(intersection.Origin.X+intersection.Dimension.Width);
		subtraction[rectangles].Dimension.Height=intersection.Dimension.Height;
		rectangles++;
	}
	if(intersection.Origin.Y+intersection.Dimension.Height<rectangle_1->Origin.Y+rectangle_1->Dimension.Height)
	{
		subtraction[rectangles].Origin.X=rectangle_1->Origin.X;
		subtraction[rectangles].Origin.Y=intersection.Origin.Y+intersection.Dimension.Height;
		subtraction[rectangles].Dimension.Width=rectangle_1->Dimension.Width;
		subtraction[rectangles].Dimension.Height=rectangle_1->Origin.Y+rectangle_1->Dimension.Height-(intersection.Origin.Y+intersection.Dimension.Height);
		rectangles++;
	}
	return(rectangles);
}
