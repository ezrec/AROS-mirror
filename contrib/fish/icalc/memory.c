/*
*	memory routines to ease garbage collection when errors occur
*	with parsing.
*
*	MWS, March 20, 1991.
*/

#include <stdlib.h>
#include "memory.h"
#include <exec/types.h>

static RemKey **gkey;

void *emalloc(n)		/* check return from malloc() */
	unsigned n;
{
	extern void execerror(char *, char *);
	char *p;

	if ((p = malloc(n)) == NULL)
		execerror("out of memory", NULL);

	return p;
}

RemKey **rem_setkey(k)		/* set key to be used for malloc's */
	RemKey **k;
{
	RemKey **oldkey = gkey;
	gkey = k;
	return oldkey;
}

void *rem_malloc(size)		/* allocate memory, tag to list */ 
	unsigned int size;
{
	RemKey *k;

	k = emalloc(sizeof(RemKey));
	k->ptr = emalloc(size);
	k->next = *gkey;
	*gkey = k;

	return k->ptr;
}

void rem_freeall()		/* free all memory associated with a key */
{
	RemKey *k;

	while (*gkey)
	{
		k = (*gkey)->next;
		free((*gkey)->ptr);
		free(*gkey);
		*gkey = k;
	}
}


/* just free 'remember' info */
/* not being used in current implementation */
/********
void rem_freelinks()	
{
	RemKey *k;

	while (*gkey)
	{
		k = (*gkey)->next;
		free(*gkey);
		*gkey = k;
	}
}
*********/
