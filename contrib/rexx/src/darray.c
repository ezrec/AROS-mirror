/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:39  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.2  1999/03/10 16:53:32  bnv
 * Added MSC support
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <bmem.h>
#include <darray.h>

/* ------------- DA_Add --------------- */
long
DA_Add( DArray *array, void *dat)
{
	long newsize,pos;

	if (array->lastitem==array->allocateditems) {		/* Increase size */
		newsize = array->allocateditems + array->increase;
		if (array->pdata)
			array->pdata = REALLOC(array->pdata,newsize*sizeof(void*));
		else
			array->pdata = MALLOC(newsize*sizeof(void*),"DArray");
		MEMSET(array->pdata + array->lastitem, 0,
			(newsize-array->lastitem)*sizeof(void*));
		array->allocateditems = newsize;
	}
	pos = array->lastitem;		/* keep position of addition */
	array->pdata[array->lastitem++] = dat;
	array->items++;
	return pos;			/* return position */
} /* DA_Add */

/* ------------- DA_AddAtFree --------------- */
long
DA_AddAtFree( DArray *array, void *dat)
{
	long pos;

	if (array->items==array->allocateditems)
		return DA_Add(array,dat);
	else {		/* search for a free position */
		for (pos=0; pos<array->lastitem; pos++)
			if (array->pdata[pos]==NULL)
				break;
		array->pdata[pos] = dat;
		if (pos==array->lastitem)
			array->lastitem++;
	}
	array->items++;
	return pos;
} /* DA_AddAtFree */

/* ---------------- DA_Del ------------------ */
void
DA_Del( DArray *array, long it )
{
	if (array->pdata[it]) {
		array->pdata[it] = 0;
		if (it==array->lastitem-1)
			array->lastitem--;
		array->items--;
	}
} /* DA_Del */
