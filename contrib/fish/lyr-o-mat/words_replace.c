
#include "words.h"


void ReplaceLine()
{
 long   count;
 UBYTE *p;
 ULONG  level = 8;
 ULONG  end   = FALSE;
 while((level--) && (!end))
 {
  count = 0L;
  p = winfo.nach;
  while(*p)
  {
   if(*p == '>')
   {
    count++;
   }
   p++;
  }
  if(count)
  {
   while(count)
   {
    ReplaceLineA(winfo.nach);
    --count;
   }
  }
  else end = TRUE;
 }
}

void ReplaceLineA(UBYTE *von)
{
 UBYTE *tline;
 long len;
 struct classnode *cl,*cl1;
 ULONG  word;
 UBYTE *rep;
 UBYTE *orig = von;
 while(*orig)
 {
  if(*orig == '>')  /* prefix found */
  {
   cl1  = NULL;
   cl = (struct classnode *)winfo.class.lh_Head;
   while(cl->cl_Node.ln_Succ != NULL)
   {
    if((*(orig + 1) == *(cl->cl_Node.ln_Name)) && (*(orig + 2) == *(cl->cl_Node.ln_Name + 1)))
    {
     if(!strncmp(orig + 1,cl->cl_Node.ln_Name,(long)strlen(cl->cl_Node.ln_Name)))
     {
      cl1 = cl;
      break;
     }
    }
    cl = (struct classnode *)cl->cl_Node.ln_Succ;
   }
   if(cl1)    /* cmd found */
   {
    word = (ULONG)RangeRand(cl1->cl_NumWords);
    rep =  (NumToNode(&cl->cl_Words,word))->ln_Name;
    len = (long)(orig - von);
    if(len >= 0)
    {
     if(tline = (UBYTE *)calloc(1,strlen(von) + strlen(cl->cl_Node.ln_Name) + strlen(rep) + 2L))
     {
      memcpy(tline,von,len);
      strcat(tline,rep);
      strcat(tline,von + len + strlen(cl->cl_Node.ln_Name) + 1);
      if(winfo.nach)
      {
       free(winfo.nach);
       winfo.nach = strdup(tline);
      }
      free(tline);
      return;
     }
    }
   }
  }
  orig++;
 }
}

