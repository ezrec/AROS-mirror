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
 * Revision 41.11  2000/05/09 20:34:35  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:59:38  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  2000/05/04 05:08:20  mlemos
 * Initial revision.
 *
 *
 */

/* graphic_tools.h */

#ifndef L_GRAPHIC_H
#define L_GRAPHIC_H

typedef signed long liCOORDINATE;
typedef unsigned long liMEASURE;

typedef int lgBOOLEAN;
#define lgTRUE ((lgBOOLEAN)(1==1))
#define lgFALSE ((lgBOOLEAN)(0!=0))
typedef unsigned long lgCARDINAL;

typedef struct
{
	liCOORDINATE X,Y;
}
liPOINT;

typedef struct
{
	liMEASURE Width,Height;
}
liDIMENSION;

typedef struct
{
	liPOINT Origin;
	liDIMENSION Dimension;
}
liRECTANGLE;

typedef struct
{
	liPOINT LowerLeft,UpperRight;
}
liBOUNDING_BOX;

lgBOOLEAN liIntersectRectangles(liRECTANGLE *rectangle,liRECTANGLE *intersected,liRECTANGLE *intersection);
lgCARDINAL liSubtractRectangles(liRECTANGLE *rectangle_1,liRECTANGLE *rectangle_2,liRECTANGLE *subtraction);

#endif
