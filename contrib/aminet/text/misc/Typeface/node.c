/*************************/
/*			 */
/* Node and list handing */
/*			 */
/*************************/

#include "typeface.h"

struct List *GetNewList(struct List **list)
{
  if ((*list = AllocVec(sizeof(struct List),MEMF_CLEAR)))
  {
    NewList(*list);
    return(*list);
  }
  else return(NULL);
}

struct Node *CreateNode(ULONG size,struct List *list)
{
struct Node *node = NULL;

  if ((node = AllocVec(size,MEMF_CLEAR))) AddTail(list,node);
  return(node);
}

void RemoveList(struct List **list,BOOL all,HOOKFUNC hook)
{
struct Node *node;

  if (*list != NULL)
  {
    while ((node = RemHead(*list)))
    {
      #ifdef _AROS
      if (hook)
      {
      	 ((void (*)(struct Node *))hook)(node);
      }
      #else
      if (hook) hook(node);
      #endif
      FreeVec(node);
    }
    if (all)
    {
      FreeVec(*list);
      *list = NULL;
    }
  }
}
