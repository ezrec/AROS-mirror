struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

void open_libs()
{
  if((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",37L))==NULL)
    error=TRUE;
  if((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",37L))==NULL)
    error=TRUE;
}

void close_libs()
{
  if(IntuitionBase!=NULL)
    CloseLibrary((struct Library *)IntuitionBase);
  else
    printf("Need Intuition.library >V37!!!\n");
  if(GfxBase!=NULL)
    CloseLibrary((struct Library *)GfxBase);
  else
    printf("Need Graphics.library >V37!!!\n");
}

struct IntuiMessage *msg;
ULONG class;
unsigned short code;

void drawfield(x,y,f)
int x,y,f;
{
int x1,y1,x2,y2;

  x1=x*fwidth+bleft;
  y1=Window->Height-y*fheight-bbottom;
  x2=x1+fwidth-1;
  y2=y1-fheight+1;
  if(f==EMPTY)
  {
    SetAPen(rp, 0);
    RectFill(rp,x1,y2,x2,y1);
    if(currlevel.t==MODE_NORMAL&&y==currlevel.e)
    {
      SetAPen(rp, 1);
      Move(rp,x1,y1-(fheight/2));
      Draw(rp,x2,y1-(fheight/2));
      SetAPen(rp, 2);
      Move(rp,x1,y1-(fheight/2)+2);
      Draw(rp,x2,y1-(fheight/2)+2);
    }
  }
  else
  {
    SetAPen(rp, 2);
    Move(rp,x1,y1);
    Draw(rp,x1,y2);
    Draw(rp,x2,y2);
    SetAPen(rp, 1);
    Move(rp,x2,y2);
    Draw(rp,x2,y1);
    Draw(rp,x1,y1);
    SetAPen(rp, f);
    RectFill(rp,x1+1,y2+1,x2-1,y1-1);
  }
}

void push(x,y)
int x,y;
{
int x1,y1,x2,y2,c=0,dx,dy;

  for(dx=-1;dx<2;dx++)
    for(dy=-1;dy<2;dy++)
      if(playfield[x+dx][y+dy]==EMPTY)
        c++;

  if(c==0)
  {
    x1=x*fwidth+bleft;
    y1=Window->Height-y*fheight-bbottom;
    x2=x1+fwidth-1;
    y2=y1-fheight+1;

    SetAPen(rp, 1);
    Move(rp,x1,y1);
    Draw(rp,x1,y2);
    Draw(rp,x2,y2);
    SetAPen(rp, 2);
    Move(rp,x2,y2);
    Draw(rp,x2,y1);
    Draw(rp,x1,y1);
  }
}

int bremove()
{
int x,y,c,i,j,f,count=0;
BOOL abbruch=FALSE;

  while(!abbruch)
  {
    abbruch=TRUE;
    for(y=currlevel.h;y>0;y--)
    {
      for(x=0;x<currlevel.b;x++)
      {
        c=0;
        while((f=playfield[x][y-1])==EMPTY&&x<(currlevel.b-1))x++;
        while((f==playfield[x+1][y-1])&&((x+1)<currlevel.b))
        {
          c++;
          x++;
        }
        if(c>2)
        {
          count+=c;
          abbruch=FALSE;
          for(i=0;i<(c+1);i++)
          {
            for(j=y;j<currlevel.h;j++)
              drawfield(x-i,j-1,playfield[x-i][j-1]=playfield[x-i][j]);
            if(replace)
              playfield[x-i][currlevel.h-1]=random(currlevel.c);
            else
              playfield[x-i][currlevel.h-1]=EMPTY;
            drawfield(x-i,currlevel.h-1,playfield[x-i][currlevel.h-1]);
          }
        }
      }
    }
  }
return(count);
}

BOOL win()
{
int x,y=0,c=1;
BOOL back=FALSE;

  switch(currlevel.t)
  {
    case MODE_NORMAL:
      while(c!=0&&y<currlevel.h)
      {
        c=0;
        for(x=0;x<currlevel.b;x++)
          if(playfield[x][y]!=EMPTY)
            c++;
        y++;
      }
      back=(y<(currlevel.e+2)&&c==0);
      break;

    case MODE_REMOVE:
      back=remcount>(currlevel.e2-1);
      break;

    case MODE_ROTATE:
      back=rotcount>(currlevel.e2-1);
      break;

    default:
      break;
  }
return(back);
}

void rotate(mx,my)
int mx,my;
{
BOOL dreh;
int m,x,y;

  if((mx>0)&&(mx<(currlevel.b-1))&&(my>0)&&(my<(currlevel.h-1)))
  {
    dreh=TRUE;
    if(replace==FALSE)
      for(x=-1;x<2;x++)
        dreh=dreh&&((playfield[mx+x][my+1])!=EMPTY);
    if(dreh==TRUE)
    {
      m=playfield[mx+1][my+1];
      playfield[mx+1][my+1]=playfield[mx+1][my];
      playfield[mx+1][my]=playfield[mx+1][my-1];
      playfield[mx+1][my-1]=playfield[mx][my-1];
      playfield[mx][my-1]=playfield[mx-1][my-1];
      playfield[mx-1][my-1]=playfield[mx-1][my];
      playfield[mx-1][my]=playfield[mx-1][my+1];
      playfield[mx-1][my+1]=playfield[mx][my+1];
      playfield[mx][my+1]=m;
      for(x=-1;x<2;x++)
        for(y=-1;y<2;y++)
          drawfield(mx+x,my+y,playfield[mx+x][my+y]);
    }
  }
}

void go()
{
BOOL result;
  remcount+=bremove();
  if(win())
  {
    result=AutoRequest(Window,&w_body,&p,&w,0L,0L,200,75);

    open_xWindow();

    switch(currlevel.t)
    {
      case MODE_NORMAL:
       sprintf(restxt,"Sie brauchten %4d Clicks und entfernten %3d Steine!",rotcount,remcount);
       break;

      case MODE_REMOVE:
        sprintf(restxt,"Für %3d Steine brauchten Sie %4d Clicks!",currlevel.e2,rotcount);
        break;

      case MODE_ROTATE:
        sprintf(restxt,"Mit %3d Clicks entfernten Sie %4d Steine!",currlevel.e2,remcount);
        break;

      default:
        break;
    }
    PrintIText(irp,&result_text,15,17);

    WaitPort(iWindow->UserPort);
    msg=(struct IntuiMessage *)GetMsg(iWindow->UserPort);
    ReplyMsg((struct Message *)msg);

    close_xWindow();

    game=FALSE;
    menue04.Flags=ITEMTEXT|HIGHBOX;
    menue02.Flags=ITEMTEXT|HIGHBOX|ITEMENABLED;
    MenuOn();

    if(result)
      clearwin();
  }
}

void startgame()
{
BOOL result=FALSE;
int i,j;

  if(game)
    result=AutoRequest(Window,&s_body,&p,&n,0L,0L,200,75);

  if(!game||result)
  {
    MenuOff();
    clearwin();
    currlevel=newlevel;
    if(currlevel.t==MODE_NORMAL)
    {
      j=Window->Height-currlevel.e*fheight-(fheight/2)-bbottom;
      replace=FALSE;
      i=bleft+(currlevel.b*fwidth)+(fwidth/2);
      SetAPen(rp, 1);
      Move(rp,bleft-(fwidth/2),j);
      Draw(rp,i,j);
      SetAPen(rp, 2);
      Move(rp,i,j+2);
      Draw(rp,bleft-(fwidth/2),j+2);
    }
    else
      replace=TRUE;
    rotcount=0;
    remcount=0;
    for(j=0;j<currlevel.h;j++)
      for(i=0;i<currlevel.b;i++)
      {
        playfield[i][j]=random(currlevel.c);
        drawfield(i,j,playfield[i][j]);
      }
    game=TRUE;
    menue04.Flags=ITEMTEXT|HIGHBOX|ITEMENABLED;
    menue02.Flags=ITEMTEXT|HIGHBOX;
    bremove();
    go();
  }
}

void getmenu()
{
BOOL weiter=FALSE;
ULONG iclass;
int xx=-1;
struct Gadget *gad;

  if(MENUNUM(code)!=MENUNULL)
    switch(MENUNUM(code))
    {
      case 0:
        switch(ITEMNUM(code))
        {
          case 0:
            if(!game)
              MenuOff();
            AutoRequest(Window,&i_body,&p,&p,0L,0L,200,75);
            if(!game)
              MenuOn();
            break;

          case 1:
            if(!game)
              MenuOff();
            ende=AutoRequest(Window,&q_body,&p,&n,0L,0L,200,75);
            if(!game)
              MenuOn();
            break;

          case 2:
            startgame();
            break;

          case 3:
            ScreenToBack(Screen);
            break;

          case 4:
            if(!game)
              MenuOff();
            if(AutoRequest(Window,&a_body,&p,&n,0L,0L,200,75))
            {
              game=FALSE;
              menue04.Flags=ITEMTEXT|HIGHBOX;
              menue02.Flags=ITEMTEXT|HIGHBOX|ITEMENABLED;
              MenuOn();
              clearwin();
            }
            if(!game)
              MenuOn();
            break;

          default:
            break;
        }
        break;

      case 1:
        switch(ITEMNUM(code))
        {
          case 0:
            newlevel.b=SUBNUM(code)+MINX;
            break;

          case 1:
            if(newlevel.h<MAXY)
              menuE[newlevel.h-1].NextItem=&menuE[newlevel.h];
            newlevel.h=SUBNUM(code)+MINY;
            menuE[newlevel.h-1].NextItem=NULL;
            if(newlevel.e>(newlevel.h-1))
            {
              menuE[newlevel.e].Flags=MENUFLAGSN;
              newlevel.e=newlevel.h-1;
              menuE[newlevel.e].Flags=MENUFLAGSC;
            }
            ClearMenuStrip(Window);
            SetMenuStrip(Window,&FIRSTMENU);
           break;

          case 2:
            newlevel.e=SUBNUM(code);
            break;

          case 3:
            newlevel.c=SUBNUM(code)+2;
            break;

          case 4:
            newlevel.t=SUBNUM(code);
            if(SUBNUM(code)==MODE_NORMAL)
              menue12.Flags=MENUFLAGSE;
            else
            {
              menue12.Flags=MENUFLAGSD;

              open_iWindow();

              while(!weiter)
              {
                WaitPort(iWindow->UserPort);
                msg=(struct IntuiMessage *)GetMsg(iWindow->UserPort);
                iclass=msg->Class;
                gad = (struct Gadget *)(msg->IAddress);
                ReplyMsg((struct Message *)msg);
                switch(iclass)
                {
                  case IDCMP_CLOSEWINDOW:
                    weiter=TRUE;
                    break;

                  case IDCMP_GADGETUP:
                    switch(gad->GadgetID)
                    {
                      case 0:
                        xx=atoi(numbergad_buf);
                        break;

                      case 1:
                        if(xx!=-1)
                          newlevel.e2=xx;
                        weiter=TRUE;
                        break;

                      case 2:
                        weiter=TRUE;
                        break;

                     default:
                        break;
                    }
                    break;

                  default:
                    break;
                }

              }

              close_iWindow();

            }
          break;

          default:
            break;
        }

      default :
        break;
    }
}
