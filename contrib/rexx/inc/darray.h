/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:36  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __DARRAY_H__
#define __DARRAY_H__

#include <lstring.h>

/* ============= type definitions ================= */
typedef struct {
	long	increase;		/* increase size             */
	long	lastitem;		/* last item in array        */
	long	items;			/* number of items           */
	long	allocateditems;		/* number of allocated items */
	void	**pdata;		/* pointer to items space    */
} DArray;

#define DAINIT(a,i)	{(a).increase=(i); (a).lastitem=0; \
			(a).items=0; (a).allocateditems=0; (a).pdata = NULL;}
#define DAAT(a,i)	((a).pdata[i])
#define DAFREE(a)	{if ((a).pdata) free((a).pdata);}

/* ============= function prototypes =============== */
long	DA_Add( DArray *array, void *dat);
long	DA_AddAtFree( DArray *array, void *dat);
void	DA_Del( DArray *array, long it );

#endif
