#ifndef CLIB_STUFF_H
#define CLIB_STUFF_H

#include "dos/bptr.h"

char *StrCpy(char *, char *);
char *StrCat(char *, char *);
int StrLen(char *);
char *StrChr(char *, int);
char *StrStr(char *, char *);
int StrCmp(char *, char *);
int StrNCmp(char *, char *, int);
void *MemSet(void *, int, int);
int MemCmp(void *, void *, int);

#ifdef AROS_FAST_BPTR
#	define AROS_BSTR_StrLen(s)	(StrLen(s))
#else
#	define AROS_BSTR_StrLen(s) (AROS_BSTR_ADDR(s)[-1])
#endif

#endif

