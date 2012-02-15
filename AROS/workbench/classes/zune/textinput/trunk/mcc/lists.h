/*
** $Id$
**
** Exec List Support Macros to overcome type quirks.
**/

#ifndef MBX
#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif
#endif

#define FIRSTNODE(l) ((APTR)((struct List*)l)->lh_Head)
#define LASTNODE(l) ((APTR)((struct List*)l)->lh_TailPred)

#define NEXTNODE(n) ((APTR)((struct Node*)n)->ln_Succ)
#define PREVNODE(n) ((APTR)((struct Node*)n)->ln_Pred)

#define FINDNAME(l,n) ((APTR)FindName((struct List*)l,n))
#define REMHEAD(l) ((APTR)RemHead((struct List*)l))
#define REMTAIL(l) ((APTR)RemTail((struct List*)l))
#define REMOVE(n) Remove((struct Node*)n)

#define ADDHEAD(l,n) AddHead((struct List*)l,(struct Node*)n)
#define ADDTAIL(l,n) AddTail((struct List*)l,(struct Node*)n)
#define ENQUEUE(l,n) Enqueue((struct List*)l,(struct Node*)n)

#define NEWLIST(l) NewList((struct List*)l)
#define ISLISTEMPTY(l) IsListEmpty(((struct List*)l))
