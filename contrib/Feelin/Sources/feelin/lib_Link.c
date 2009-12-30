/*
**    $VER: lib_Link.c 04.00 (2003/10/12)
**
**    Linking functions
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

*/

#include "Private.h"

/****************************************************************************
*** Functions ***************************************************************
****************************************************************************/

///f_link_tail
F_LIB_LINK_TAIL
{
   if (List && Node)
   {
      if (Node -> Next || Node -> Prev)
      {
         #ifdef DEBUG_CODE
         F_Log(0,"Node 0x%08lx seams to be already linked (0x%08lx,0x%08lx)",Node,Node -> Next,Node -> Prev);
         #endif
      }
      else
      {
         if (List -> Tail)
         {
            Node -> Prev = List -> Tail;
            Node -> Prev -> Next = Node;
            List -> Tail = Node;
         }
         else
         {
            List -> Head = Node;
            List -> Tail = Node;
         }
         return Node;
      }
   }
   return NULL;
}
F_LIB_END
//+
///f_link_head
F_LIB_LINK_HEAD
{
   if (List && Node)
   {
      if (Node -> Next || Node -> Prev)
      {
         #ifdef DEBUG_CODE
         F_Log(0,"Node 0x%08lx seams to be already linked (0x%08lx,0x%08lx)",Node,Node -> Next,Node -> Prev);
         #endif
      }
      else
      {
         if (List -> Head)
         {
            Node -> Next = List -> Head;
            Node -> Next -> Prev = Node;
            List -> Head = Node;
         }
         else
         {
            List -> Head = Node;
            List -> Tail = Node;
         }
         return Node;
      }
   }
   return NULL;
}
F_LIB_END
//+
///f_link_insert
F_LIB_LINK_INSERT
{
   if (List && Node)
   {
      if (Prev)
      {
         if (Prev -> Next)
         {
            Node -> Next = Prev -> Next;
            Node -> Prev = Prev;
            Prev -> Next -> Prev = Node;
            Prev -> Next = Node;

            return Node;
         }
         else
         {
            return F_LinkTail(List,Node);
         }
      }
      else
      {
         return F_LinkHead(List,Node);
      }
   }
   return NULL;
}
F_LIB_END
//+
///f_link_move
F_LIB_LINK_MOVE
{
   if (List && Node)
   {
      #ifdef DB_LINK_SECURE
      if (F_LinkMember(List,Node))
      {
      #endif

         F_LinkRemove(List,Node);
         return F_LinkInsert(List,Node,Prev);

      #ifdef DB_LINK_SECURE
      }
      #endif
   }
   return NULL;
}
F_LIB_END
//+
///f_link_remove
F_LIB_LINK_REMOVE
{
   if (List && Node)
   {
      #ifdef DB_LINK_SECURE
      FNode *node;
      
      for (node = List -> Head ; node ; node = node -> Next)
      {
         if (node == Node) break;
      }

      if (node)
      {
      #endif
         if (Node -> Next)
         {
            if (Node -> Prev)
            {
               Node -> Prev -> Next = Node -> Next;
               Node -> Next -> Prev = Node -> Prev;
            }
            else
            {
               List -> Head = Node -> Next;
               Node -> Next -> Prev = NULL;
            }
         }
         else
         {
            if (Node -> Prev)
            {
               List -> Tail = Node -> Prev;
               Node -> Prev -> Next = NULL;
            }
            else
            {
               List -> Head = NULL;
               List -> Tail = NULL;
            }
         }

         Node -> Next = NULL;
         Node -> Prev = NULL;

         return Node;
      #ifdef DB_LINK_SECURE
      }
      #endif
   }
   return NULL;
}
F_LIB_END
//+
///f_link_member
F_LIB_LINK_MEMBER
{
   if (List && Node)
   {
      FNode *node;

      for (node = List -> Head ; node ; node = node -> Next)
      {
         if (node == Node) break;
      }

      return node;
   }
   return NULL;
}
F_LIB_END
//+
///f_next_node
F_LIB_NEXT_NODE
{
   FNode *next = *Nodeptr;

   if (next)
   {
      *Nodeptr = next -> Next;
   }
   return next;
}
F_LIB_END
//+

