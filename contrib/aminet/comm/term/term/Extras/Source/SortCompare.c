/*
**	SortCompare.c
**
**	Phonebook sort comparison routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC LONG
ComparePhone(PhoneEntry **A,PhoneEntry **B)
{
		/* Has entry A been selected? */

	if((*A)->Count == -1)
	{
			/* If entry B isn't selected either, compare the
			 * names lexically, else entry B is supposed
			 * to be `smaller' than entry A.
			 */

		if((*B)->Count == -1)
			return(Stricmp((*A)->Header->Name,(*B)->Header->Name));
		else
			return(1);
	}
	else
	{
			/* If entry B isn't selected, entry A is supposed
			 * to be `smaller' than entry B, else return
			 * the difference between both entries.
			 */

		if((*B)->Count == -1)
			return(-1);
		else
			return((*A)->Count - (*B)->Count);
	}
}

STATIC LONG
CompareName(PhoneEntry **A,PhoneEntry **B)
{
	return(Stricmp((*A)->Header->Name,(*B)->Header->Name));
}

STATIC LONG
CompareNumber(PhoneEntry **A,PhoneEntry **B)
{
	return(Stricmp((*A)->Header->Number,(*B)->Header->Number));
}

STATIC LONG
CompareComment(PhoneEntry **A,PhoneEntry **B)
{
	return(Stricmp((*A)->Header->Comment,(*B)->Header->Comment));
}

/****************************************************************************/

STATIC LONG
CompareInversePhone(PhoneEntry **A,PhoneEntry **B)
{
	return(ComparePhone(B,A));
}

STATIC LONG
CompareInverseName(PhoneEntry **A,PhoneEntry **B)
{
	return(CompareName(B,A));
}

STATIC LONG
CompareInverseNumber(PhoneEntry **A,PhoneEntry **B)
{
	return(CompareNumber(B,A));
}

STATIC LONG
CompareInverseComment(PhoneEntry **A,PhoneEntry **B)
{
	return(CompareComment(B,A));
}

/****************************************************************************/

SORTFUNC
GetSortFunc(LONG How,BOOL ReverseOrder)
{
	SORTFUNC SortFunc;

	switch(How)
	{
		case SORT_NAME:

			if(ReverseOrder)
				SortFunc = (SORTFUNC)CompareInverseName;
			else
				SortFunc = (SORTFUNC)CompareName;

			break;

		case SORT_COMMENT:

			if(ReverseOrder)
				SortFunc = (SORTFUNC)CompareInverseComment;
			else
				SortFunc = (SORTFUNC)CompareComment;

			break;

		case SORT_NUMBER:

			if(ReverseOrder)
				SortFunc = (SORTFUNC)CompareInverseNumber;
			else
				SortFunc = (SORTFUNC)CompareNumber;

			break;

		case SORT_SELECTION:

			if(ReverseOrder)
				SortFunc = (SORTFUNC)CompareInversePhone;
			else
				SortFunc = (SORTFUNC)ComparePhone;

			break;

		default:

			SortFunc = (SORTFUNC)CompareName;
			break;
	}

	return(SortFunc);
}
