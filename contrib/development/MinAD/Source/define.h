/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#ifndef DEFINE_H
#define DEFINE_H 1

/****************************************************************************/

#define MINSYS 			39
#define MINMUI 			19

#define LINEFEED			12 					//
#define HIGHLIGHT 		'b'					//bold
#define FUNCSTRFIX 		(31 * (2*2) + 1)	//31 possible keywords
														//* 2*2 (\033b<txt>\033n)
														//+ nullbyte

#define PREFSFILE			"PROGDIR:"APP_PROGNAME".cfg"
#define DEFAULTPATH1		"AutoDocs:"
#define DEFAULTPATH2		"RAM:"

#define SIZE_NAME 		128
#define SIZE_PATH 		1024
#define SIZE_OVERVIEW	(1024*64)
#define SIZE_EXALL		(1024*64)

/*------------------------------------------------------------------------*/

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))

#define setbit(var, bit) ((var) |=  (1ul << (bit)))
#define clrbit(var, bit) ((var) &= ~(1ul << (bit)))
#define isset(var, bit)  ((var) &	(1ul << (bit)))
#define isclr(var, bit)	(((var) &	(1ul << (bit))) == 0)

/*------------------------------------------------------------------------*/

#if !defined(__AROS__)
#define GetSucc(node) \
({ struct Node *n = (struct Node *)(node);  \
	n->ln_Succ->ln_Succ ? n->ln_Succ : (struct Node *)NULL;  \
})

#define ForeachNode(l,n) \
	for (n = (void *)(((struct List *)(l))->lh_Head); \
	((struct Node *)(n))->ln_Succ; \
	n = (void *)(((struct Node *)(n))->ln_Succ))

#define ForeachNodeSafe(l,n,n2)  \
	for (n = (void *)(((struct List *)(l))->lh_Head); \
	(n2 = (void *)((struct Node *)(n))->ln_Succ); \
	n = (void *)n2)
#endif

/* List casting macros */
#define myNewList(list) NewList((struct List *)(list))
#define myInsert(list, node1, node2) Insert((struct List *)(list), (struct Node *)(node1), (struct Node *)(node2))
#define myAddHead(list, node) AddHead((struct List *)(list), (struct Node *)(node))
#define myAddTail(list, node) AddTail((struct List *)(list), (struct Node *)(node))
#define myRemove(node) Remove((struct Node *)(node))

/*------------------------------------------------------------------------*/

#define myAlloc(size) AllocVecPooled(pool, (ULONG)(size))
#define myFree(buffer) FreeVecPooled(pool, (APTR)(buffer))

/*------------------------------------------------------------------------*/
/* String casting macros */

#define _stricmp(dst, src) stricmp((const char *)dst, (const char *)src)
#define _strlen(str) strlen((const char *)str)
#define _strcpy(dst, src) strcpy((char *)dst, (const char *)src)
#define _strncpy(dst, src, size) strncpy((char *)dst, (const char *)src, (size_t)size)
#define _strncmp(dst, src, size) strncmp((char *)dst, (const char *)src, (size_t)size)
#define _strcat(dst, src) strcat((char *)dst, (const char *)src)
#define _strstr(str1, str2) strstr((const char *)str1, (const char *)str2)
#define _memcmp(a, b, size) memcmp((char *)a, (const char *)b, (size_t)size)

/*------------------------------------------------------------------------*/
/* MUI */

#define DTPicObject		MUI_NewObject("Dtpic.mui"
#ifndef MUIA_DTPic_Name
#define MUIA_DTPic_Name	0x80423d72
#endif

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG)(a) << 24 | (ULONG)(b) << 16 | (ULONG)(c) << 8 | (ULONG)(d))
#endif

#define PrintfText(text) set(MUI_Obj[MAIN][M_Text], MUIA_NFloattext_Text, text)

/*------------------------------------------------------------------------*/

#endif /* DEFINE_H */
