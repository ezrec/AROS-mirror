#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/alib.h>

/*  Functions for creating and manipulating the list of memos
 *
 *  Functions in this file are :-
 *
 *	struct MinList *LoadData()  \   Not yet included/complete
 *	int SaveData()              /
 *	struct MinNode *MoveBackOne(struct MinNode *p)
 *	struct MinNode *MoveBackBlock(struct MinNode *p)
 *	struct MinNode *MoveEnd(struct MinNode *p)
 *	struct MinNode *MoveFwdOne(struct MinNode *p)
 *	struct MinNode *MoveFwdBlock(struct MinNode *p)
 *	void DisplayBlock()
 *	void Toggle(int t_gadget)
 *
 *	    (to the memo currently top of the display)
 *  The pointer ^ is generally held as a pointer to a MinNode structure since
 *    this makes the code clearer and more efficient(?) since it does not
 *    need to be cast into this form from the Memo_In_Mem form all over the
 *    place.
 *
 *  Use them like so:-
 *    struct MinNode *x,*DisplayFirst;
 *    x=MoveFwdBlock(DisplayFirst);
 *    if (x != DisplayFirst)
 *	{
 *	DisplayFirst=x;
 *	DisplayBlock(DisplayFirst);
 *	}
 */

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

/*#include "mm2_test.h"*/

#include "mm2.h"
#include <stdio.h>
#include <string.h>

#ifdef __AROS__
#define remove DeleteFile
#define rename Rename
#endif

extern void Display_One(), Display_Blank();

extern struct MinNode *DisplayFirst;
extern struct Remember *RK;
extern struct MinList *MemListPtr;

void Toggle();

/* ==========================================================================
 * LoadData() - version using exec list handling
 */

struct MinList *LoadData()
  {
  FILE *fp;
  struct MinList *mlp;
  struct Memo_Item mi;
  struct MI_Mem *p;
  int OK;

  mlp=(struct MinList *)AllocRemember(&RK,sizeof(struct MinList),MEMF_PUBLIC);
  if (mlp == NULL) return mlp;  /* Failed. No memory to start list  */
  NewList(mlp);
  fp=fopen("memos:memodata.dat","r");

  if (fp == NULL ) return mlp;  /* File isn't there. Return empty list */
  OK=fread((char *)&mi,sizeof(struct Memo_Item),1,fp);
  while (OK)
    {
    p=(struct MI_Mem *)AllocRemember(&RK,sizeof(struct MI_Mem),MEMF_PUBLIC);
    if (p == NULL)
      {
      /* Take some ABORT!!!! type action here due to loss of memory */
      }
    memcpy((char *)&(p->mim_MI),(char *)&mi,sizeof(struct Memo_Item) );
    p->mim_Select = 0;	/*  All memos start off deselected  */
    p->mim_MI.mi_Text[60]='\0';
    AddTail(mlp,p);
    OK=fread((char *)&mi,sizeof(struct Memo_Item),1,fp);
    }
  fclose(fp);
  return mlp;
  }

/*=====================================================================
 * SaveData()
 *
 * Writes structures in list to file memodata.dat . Previous version of
 *   memodata.dat is renamed .bak
 *
 * !!!!!!! Currently no error checking !!!!!!!
 */
SaveData()
  {
  FILE *fp;
  int Save_Status=1;	 /* Assume success */
  int errr;
  struct MinNode *n;
  struct MI_Mem *m;

  errr=remove("memos:memodata.bak");
  errr=rename("memos:memodata.dat","memos:memodata.bak");
  if (LISTEMPTY) return Save_Status;
  fp=fopen("memos:memodata.dat","w");
  n=MemListPtr->mlh_Head;
  while (n->mln_Succ)
    {
    m=(struct MI_Mem *)n;
    errr=fwrite((char *)&(m->mim_MI),sizeof(struct Memo_Item),1,fp);
    n = n->mln_Succ;
    }
  fclose(fp);
  return Save_Status;
  }



/*  =========================================================================
 *  Move the 'current' memo back one
 */
struct MinNode *MoveBackOne(struct MinNode *p)
  {
  if (p->mln_Pred != (struct MinNode *)MemListPtr)
    {
    p=p->mln_Pred;
    }
  return p;
  }

/*  =========================================================================
 *Move the 'current' memo back a block
 */

struct MinNode *MoveBackBlock(struct MinNode *p)
  {
  int moved;

  moved=0;
  while ((moved < MEMOS_IN_BLOCK) && (p->mln_Pred != (struct MinNode *)MemListPtr))
    {
    p=p->mln_Pred;
    moved ++;
    }
  return p;
  }

/*=========================================================================
 * Move the 'current' memo forward one. More complicated than at first apparent
 *   because the current pointer is the one at the start of the block and the
 *   end of the list has to be checked relative to the last in the block.
 *   Actually move the current pointer MEMOS_IN_BLOCK+1 forward (or until
 *   the end of the list) and then MEMOS_IN_BLOCK backward (or the number
 *   moved forwards if smaller)  */

struct MinNode *MoveFwdOne(struct MinNode *p)
  {
  int cf;

  cf=0;
  while ((cf<MEMOS_IN_BLOCK+1) &&  (p->mln_Succ))
    {
    p=p->mln_Succ;
    cf++;
    }
  cf=MIN(cf,MEMOS_IN_BLOCK);
  while (cf > 0)
    {
    p=p->mln_Pred;
    cf--;
    }
  return p;
  }

/*=========================================================================
 * Move the 'current' memo forward one block.
 *   Actually move the current pointer MEMOS_IN_BLOCK*2 forward (or until
 *   the end of the list) and then MEMOS_IN_BLOCK backward (or the number
 *   moved forwards if smaller)  */

struct MinNode *MoveFwdBlock(struct MinNode *p)
  {
  int cf;

  cf=0;
  while ((cf < MEMOS_IN_BLOCK+MEMOS_IN_BLOCK) && (p->mln_Succ))
    {
    p=p->mln_Succ;
    cf++;
    }
  cf=MIN(cf,MEMOS_IN_BLOCK);
  while (cf > 0)
    {
    p=p->mln_Pred;
    cf--;
    }
  return p;
  }
/*========================================================================
 *  Makes succesive calls to Display_One() to display a block of memos.
 *    Param is pointer to a node structure attached to first memo
 *    to be displayed. Value of parameter is DisplayFirst which may need to
 *    be reset prior to call to Display_Block(). If param is NULL then list
 *    must be empty so display all blank memos
 =========================================================================*/
void Display_Block(struct MinNode *m)
  {
  int p;

  p=0;
  if (m)
    {
/*    while ( (p<MEMOS_IN_BLOCK) && ( m != NULL ) ) */
    while ( (p<MEMOS_IN_BLOCK) && ( m->mln_Succ ) )
	{
	Display_One( p, (struct MI_mem *)m );
	p++;
	m=m->mln_Succ;
	}
    }

  while ( p<MEMOS_IN_BLOCK )
	{
	Display_Blank( p );
	p++;
	}
  }
/*=========================================================================
 * If memo display gadget clicked on is currently showing a memo, toggle
 *   the memos selected indicator and redisplay.
 */
void Toggle(int t_gadget)
  {
  int g;
  struct MinNode *n;

  n=DisplayFirst;
  for (g=0 ; g < t_gadget ; g++)
    {
    n=n->mln_Succ;
    if (!n) break;
    }
  if (n) if (n->mln_Succ)
    {
    if ( ((struct MI_Mem *)n)->mim_Select == 0) /* ie if not currently selected */
      ((struct MI_Mem *)n)->mim_Select=1;
    else
      ((struct MI_Mem *)n)->mim_Select=0;
    Display_One(g,(struct MI_Mem *)n);
    }
  }


/*=========================================================================
 * Move the 'current' memo forward to end
 *   Actually move the current pointer forward to
 *   the end of the list) and then MEMOS_IN_BLOCK backward
 */

struct MinNode *MoveEnd(struct MinNode *p)
  {
  int b;

  while (p->mln_Succ)
    {
    p=p->mln_Succ;
    }
  b=MEMOS_IN_BLOCK;
  while (b > 0)
    {
    p=p->mln_Pred;
    b--;
    }
  return p;
  }

