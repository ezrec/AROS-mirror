/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:38  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lsign --------------------- */
int
Lsign( const PLstr num )
{
	L2NUM(num);

	switch (LTYPE(*num)) {
		case LINTEGER_TY:
			if (LINT(*num)<0)
				return -1;
			else
			if (LINT(*num)>0)
				return  1;
			else
				return  0;

		case LREAL_TY:
			if (LREAL(*num)<0)
				return -1;
			else
			if (LREAL(*num)>0)
				return  1;
			else
				return  0;
	}
	return 0;
} /* Lsign */
