#ifndef CLIB_STUFF_H
#define CLIB_STUFF_H

#ifdef __AROS__
#include "dos/bptr.h"
#endif

#if 0
char *StrCpy(char *, char *);
char *StrCat(char *, char *);
int   StrLen(char *);
char *StrChr(char *, int);
char *StrStr(char *, char *);
int   StrCmp(char *, char *);
int   StrNCmp(char *, char *, int);
void *MemSet(void *, int, int);
int   MemCmp(void *, void *, int);
#else
#define StrCpy(s1,s2)    strcpy(s1,s2)
#define StrCat(s1,s2)    strcat(s1,s2)
#define StrLen(s)        strlen(s)
#define StrChr(s,c)      strchr(s,c)
#define StrStr(s1,s2)    strstr(s1,s2)
#define StrCmp(s1,s2)    strcmp(s1,s2)
#define StrNCmp(s1,s2,n) strncmp(s1,s2,n)
#define MemSet(p,c,n)    memset(p,c,n)
#define MemCmp(p1,p2,n)  memcmp(p1,p2,n)
#endif

#ifdef AROS_FAST_BPTR
#	define AROS_BSTR_StrLen(s)	(StrLen(s))
#else
#	define AROS_BSTR_StrLen(s) (AROS_BSTR_ADDR(s)[-1])
#endif

#endif

