/*
*	the whole point of the program, really: user-functions.
*	These are the routines to implement them.
*	Structure definition is in complex.h.
*
*	MWS, March 21, 1991.
*/
#include "complex.h"
#include "memory.h"

void clear_ufunc(func)
	UserFunc *func;
{
	RemKey **oldkey = rem_setkey(&func->remkey);

	rem_freeall();
	free(func->param->name);
	free(func->param);
	func->tree = NULL;
	rem_setkey(oldkey);
}
