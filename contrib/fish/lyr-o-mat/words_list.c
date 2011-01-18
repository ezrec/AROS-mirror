
#include "words.h"


void newpattern(UBYTE *name)
{
 struct patternnode *p;
 UBYTE              *s;
 if(edit.pattern)
 {
  if(winfo.currentpattern)
  {
   free(winfo.currentpattern->pt_Node.ln_Name);
   winfo.currentpattern->pt_Node.ln_Name = strdup(name);
  }
 }
 else if(p = (struct patternnode *)calloc(1,sizeof(struct patternnode)))
 {
  p->pt_Node.ln_Name = strdup(name);
  s = p->pt_Node.ln_Name + strlen(p->pt_Node.ln_Name) - 1;
  while((*s == ' ') || (*s == '\n'))*s-- = '\0';
  ++winfo.numpattern;
  AddTail(&winfo.pattern,(struct Node *)p);
  winfo.currentpattern = p;
 }
}
void newclass(UBYTE *name)
{
 UBYTE            *s;
 struct classnode *c;
 if(edit.class)
 {
  if(winfo.currentclass)
  {
   free(winfo.currentclass->cl_Node.ln_Name);
   winfo.currentclass->cl_Node.ln_Name = strdup(name);
  }
 }
 else if(c = (struct classnode *)calloc(1,sizeof(struct classnode)))
 {
  c->cl_Node.ln_Name = strdup(name);
  c->cl_NumWords = 0L;
  s = c->cl_Node.ln_Name + strlen(c->cl_Node.ln_Name) - 1;
  while((*s == ' ') || (*s == '\n'))*s-- = '\0';
  NewList(&c->cl_Words);
  ++winfo.numclass;
  c->cl_Node.ln_Pri = strlen(c->cl_Node.ln_Name);
  Enqueue(&winfo.class,(struct Node *)c);
  winfo.currentclass = c;
 }
}
void newword(UBYTE *name)
{
 UBYTE            *s;
 struct wordnode  *w;
 if(winfo.currentclass)
 {
  if(edit.word)
  {
   if(winfo.currentword)
   {
    free(winfo.currentword->wn_Node.ln_Name);
    winfo.currentword->wn_Node.ln_Name = strdup(name);
   }
  }
  else  if(w = (struct wordnode *)calloc(1,sizeof(struct wordnode)))
  {
   w->wn_Node.ln_Name = strdup(name);
   s = w->wn_Node.ln_Name + strlen(w->wn_Node.ln_Name) - 1;
   while((*s == ' ') || (*s == '\n'))*s-- = '\0';
   winfo.currentclass->cl_NumWords++;
   w->wn_Class = winfo.currentclass;
   w->wn_Node.ln_Pri = strcmp("",w->wn_Node.ln_Name);
   Enqueue(&winfo.currentclass->cl_Words,(struct Node *)w);
   winfo.currentword = w;
  }
 } 
}
void delpattern(void)
{
 if(winfo.currentpattern)
 {
  list_off(LIST_PT);
  Remove((struct Node *)winfo.currentpattern);
  free(winfo.currentpattern->pt_Node.ln_Name);
  winfo.numpattern -= (winfo.numpattern > 0) ? 1 :0 ;
  free(winfo.currentpattern);
  winfo.currentpattern = NULL;
  list_on(LIST_PT);
 }
}
void delclass(void)
{
 if(winfo.currentclass)
 {
  list_off(LIST_CL);
  Remove((struct Node *)winfo.currentclass);
  free(winfo.currentclass->cl_Node.ln_Name);
  winfo.numclass -= (winfo.numclass > 0) ? 1 :0 ;
  delwordlist(winfo.currentclass);
  free(winfo.currentclass);
  winfo.currentclass = NULL;
  list_on(LIST_CL);
 }
}
void delword(void)
{
 if(winfo.currentword)
 {
  list_off(LIST_WD);
  Remove((struct Node *)winfo.currentword);
  free(winfo.currentword->wn_Node.ln_Name);
  winfo.currentword->wn_Class->cl_NumWords -= 
       (winfo.currentword->wn_Class->cl_NumWords > 0) ? 1 : 0;
  free(winfo.currentword);
  winfo.currentword = NULL;
  list_on(LIST_WD);
 }
}

void delwordlist(struct classnode *cl)
{
 struct Node *n;
 list_off(LIST_WD);
 while(n = RemHead(&cl->cl_Words))
 {
  free(n->ln_Name);
  free(n);
 }
 list_on(LIST_WD);
}

void delAll(void)
{
 struct Node *n;
 struct classnode *cl;
 winfo.currentpattern = NULL;
 winfo.currentclass   = NULL;
 winfo.currentword    = NULL;
 winfo.numpattern     = 0;
 winfo.numclass       = 0;
 edit.pattern = 0;
 edit.class   = 0;
 edit.word    = 0;
 if(winfo.nach){free(winfo.nach);winfo.nach = NULL;};
 list_off(LIST_PT);list_off(LIST_CL);list_off(LIST_WD);
 while(n = RemHead(&winfo.pattern))
 {
  free(n->ln_Name);
  free(n);
 }
 while(cl = (struct classnode *)RemHead(&winfo.class))
 {
  while(n = RemHead(&cl->cl_Words))
  {
   free(n->ln_Name);
   free(n);
  }
  free(cl->cl_Node.ln_Name);
  free(cl);
 }
 list_on(LIST_PT);list_on(LIST_CL);list_on(LIST_WD);
}

struct Node *NumToNode(struct List *list,UWORD Num)
{
 struct Node *n = list->lh_Head;
 for(;Num;Num--)
 {
  n = n->ln_Succ;
 }
 return n;
}

void list_off(ULONG which)
{
 switch(which)
 {
  case LIST_PT : 
    GT_SetGadgetAttrs(Project0Gadgets[GDX_templatelist],Project0Wnd,NULL,GTLV_Labels,-1, TAG_DONE);
    break;
  case LIST_CL :
    GT_SetGadgetAttrs(Project0Gadgets[GDX_classlist],Project0Wnd,NULL,GTLV_Labels,-1, TAG_DONE);
    break;
  case LIST_WD :
    GT_SetGadgetAttrs(Project0Gadgets[GDX_wordlist],Project0Wnd,NULL,GTLV_Labels,-1, TAG_DONE);
    break;
 }
}
void list_on(ULONG which)
{
 ULONG num;
 switch(which)
 {
  case LIST_PT : num = (winfo.currentpattern) ? NodeToNum(&winfo.pattern,(struct Node *)winfo.currentpattern) : NodeToNum(&winfo.pattern,winfo.pattern.lh_TailPred);
     GT_SetGadgetAttrs(Project0Gadgets[GDX_templatelist],Project0Wnd,NULL,GTLV_Labels,&winfo.pattern,GTLV_Top, num ,GTLV_Selected,num,TAG_DONE);
     break;
  case LIST_CL : num = (winfo.currentclass) ? NodeToNum(&winfo.class,(struct Node *)winfo.currentclass) : NodeToNum(&winfo.class,winfo.class.lh_TailPred);
     GT_SetGadgetAttrs(Project0Gadgets[GDX_classlist],Project0Wnd,NULL,GTLV_Labels,&winfo.class, GTLV_Top, num,GTLV_Selected,num,TAG_DONE);
     if(winfo.currentclass)
     {
      num = (winfo.currentword) ? NodeToNum(&winfo.currentclass->cl_Words,(struct Node *)winfo.currentword) : NodeToNum(&winfo.currentclass->cl_Words,winfo.currentclass->cl_Words.lh_TailPred);
      GT_SetGadgetAttrs(Project0Gadgets[GDX_wordlist],Project0Wnd,NULL,GTLV_Labels,&winfo.currentclass->cl_Words,GTLV_Top,num, GTLV_Selected,num,TAG_DONE);
     }
     break;
  case LIST_WD :
     if(winfo.currentclass)
     {
      num = (winfo.currentword) ? NodeToNum(&winfo.currentclass->cl_Words,(struct Node *)winfo.currentword) : NodeToNum(&winfo.currentclass->cl_Words,winfo.currentclass->cl_Words.lh_TailPred);
      GT_SetGadgetAttrs(Project0Gadgets[GDX_wordlist],Project0Wnd,NULL,GTLV_Labels,&winfo.currentclass->cl_Words,GTLV_Top,num, GTLV_Selected,num,TAG_DONE);
     }
     break;
 }
}

ULONG NodeToNum(struct List *list,struct Node *node)
{
 ULONG num = 0;
 struct Node *n;
 for(n = list->lh_Head;n->ln_Succ;n = n->ln_Succ,num++)
  if(node == n) return num;
}

