#include "find.h"

#define ComputeXY(x,y) (((x)*info.data.size) + (y))

void DisableAll(void)
{
 putmsg(7);
 remappmenus();
 ActivateCxObj(info.broker.broker,0L);
 if(Project0Wnd)
 {
  info.misc.disable = TRUE;
  Project0Render();
  if ( Project0Menus)
  {
		 ClearMenuStrip( Project0Wnd );
		 FreeMenus( Project0Menus );
		 Project0Menus = NULL;
  }
 }
}

void EnableAll(void)
{
 info.misc.disable = FALSE;
 ActivateCxObj(info.broker.broker,1L);
 addappmenus();
 if(Project0Wnd)
 {
  if(!Project0Menus)
  {
   if(Project0Menus = CreateMenus( Project0NewMenu, GTMN_FrontPen, 0L, TAG_DONE ))
   {
   	LayoutMenus( Project0Menus, VisualInfo, TAG_DONE );
    SetMenuStrip(Project0Wnd,Project0Menus);
    Project0Render();
   }
  }
 }
}

void WLDMatch(void)
{
 ULONG line;
 struct Node *n;
 UBYTE m[32];
 Freematchlist();
 DisableAll();
 info.match.count = 0;
 if(Readfile())
 {
  for(line = 1,n = info.misc.lines.lh_Head;n->ln_Succ;n = n->ln_Succ,line++)
  {
   if(line < info.misc.fromline)continue;
   if(line > info.misc.toline)goto end;
   if(CheckPorts())goto end;
   sprintf(m,"Line %ld",line);
   GT_SetGadgetAttrs(Project0Gadgets[GDX_errors],Project0Wnd,NULL,GTTX_Text,(ULONG)m,TAG_DONE);
   matchline(n,line);   
  }
 }
 end:
 Freefile();
 EnableAll();
 outputlist();
}

int CheckPorts()
{
 struct IntuiMessage *msg;
 int rc = FALSE;
 if(Project0Wnd)
 {
  while(msg = (struct IntuiMessage *)GT_GetIMsg(Project0Wnd->UserPort))
  {
   switch(msg->Class)
   {
    case IDCMP_GADGETUP    : 
         if( ((struct Gadget *)msg->IAddress)->GadgetID == GDX_stop)rc = TRUE;
         break;
    case IDCMP_CLOSEWINDOW :
         CloseProject0Window();
         break;
    case IDCMP_VANILLAKEY  :
         if((msg->Code == 'q') || (msg->Code == 'Q'))rc = TRUE;
         break;
    default                : break;               
   }
   GT_ReplyIMsg(msg);
  }
 }
 return rc;
}

void matchline(struct Node *n,ULONG line)
{
 UBYTE *w;
 UBYTE wordbuffer[STRINGSIZE],linenum[24],wordnum[24],distnum[24];
 UBYTE m[STRINGSIZE];
 ULONG word = 1;
 BYTE  dist;
 struct Node *mn;
 int leave = FALSE;
 w    = stpblk(n->ln_Name);
 while(!leave)
 {
  w = stptok(w,info.strings.buffer,info.data.size,info.strings.delim);
  if((w != NULL) && (*w != '\0'))
  {
   dist = wld();
   if(dist <= info.match.filter)
   {
    info.match.count++;
    if(mn = (struct Node *)calloc(1,sizeof(struct Node)))
    {
     if(info.flags.dist)sprintf(distnum,"Dist=%d",dist);else *distnum = '\0';
     if(info.flags.linenum )sprintf(linenum,"Line=%ld",line);else *linenum = '\0';
     if(info.flags.word)sprintf(wordbuffer,"%s",info.strings.buffer);else *wordbuffer = '\0';
     if(info.flags.wnum)sprintf(wordnum,"Word=%ld",word);else *wordnum = '\0';
     sprintf(m,"%s %s %s %s",distnum,linenum,wordnum,wordbuffer);
     mn->ln_Name = strdup(m);
     mn->ln_Pri  = -dist;
     if(Project0Wnd)
     {
      GT_SetGadgetAttrs(Project0Gadgets[GDX_Gadget00],Project0Wnd,NULL,GTLV_Labels,-1L,TAG_DONE);
      Enqueue(&info.match.list,mn);
      GT_SetGadgetAttrs(Project0Gadgets[GDX_Gadget00],Project0Wnd,NULL,GTLV_Labels,(ULONG)&info.match.list,TAG_DONE);
     }
     else Enqueue(&info.match.list,mn);
    }
   }
  }
  else leave = TRUE;
  if(w)w = stpblk(++w);
  word++;
 }
}

int Readfile()
{
 int rc = FALSE;
 struct Node *n;
 Freefile();
 if(info.misc.inp = fopen(info.strings.filename,"r"))
 {
  rc = TRUE;
  putmsg(10);
  while(fgets(info.strings.buffer,STRINGSIZE,info.misc.inp))
  {
   if(n = calloc(1,sizeof(struct Node)))
   {
    info.strings.buffer[STRINGSIZE - 1] = '\0';
    n->ln_Name = strdup(info.strings.buffer);
    AddTail(&info.misc.lines,n);
   }
  }
  fclose(info.misc.inp);
 }
 else putmsg(8);
 putmsg(0);
 return rc;
}

void Freefile()
{
 struct Node *n;
 while(n = RemHead(&info.misc.lines))
 {
  free(n->ln_Name);
  free(n);
 }
}

ULONG wld_min(ULONG x,ULONG y,ULONG z)
{
 if(x < y) y = x;
 if(y < z) z = y;
 return z;
}

/* ----------------------------------------------------------------------------------------------------- */

ULONG compare(ULONG x, ULONG y)
{
 char *w1,*w2;
 w1 = info.strings.search;
 w2 = info.strings.buffer;
 return (tolower(*(w1+x)) == tolower(*(w2+y))) ? 0: info.weight.sub;
}

/* ----------------------------------------------------------------------------------------------------- */


BYTE wld()
{
 ULONG i,j;
 BYTE rc;
 *(info.data.data) = 0;
 for(j = 1;j <= info.data.size;j++)
    *(info.data.data + ComputeXY(0,j))  = *(info.data.data +ComputeXY(0,j-1)) + info.weight.ins;
 for(i = 1;i <= info.data.size;i++)
    *(info.data.data + ComputeXY(i,0)) = *(info.data.data + ComputeXY(i-1,0)) + info.weight.del;
 for(i = 1; i <= strlen(info.strings.search);i++)
 {
     for(j = 1;j <= strlen(info.strings.buffer);j++)
       {
         *(info.data.data + ComputeXY(i,j)) =
           wld_min(*(info.data.data + ComputeXY(i-1,j-1)) + compare(i-1,j-1),
                   *(info.data.data + ComputeXY(i-1,j)) +  info.weight.del,
                   *(info.data.data + ComputeXY(i,j-1)) +  info.weight.ins

                  );
       }
 }
 i = *(info.data.data + ComputeXY(strlen(info.strings.search),strlen(info.strings.buffer)));
 if(i >  127)rc =  127; else   rc = (BYTE)i;
 return(rc);
}

