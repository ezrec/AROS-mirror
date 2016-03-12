void MaleMenue()
{
char number[4];

clearwin();
  if(WinSize(Window,350,200)==FALSE)
    fprintf(stderr,"%s",_(MSG_ERRORWINRESIZE));
/*  clearwin();*/
  drawfield(40,60,130,75);
  drawfield(40,90,130,105);
  drawfield(40,120,130,135);
  sprintf(number,"%3d",width);
  write_text(50,71,number,1);
  sprintf(number,"%3d",height);
  write_text(50,101,number,1);
  sprintf(number,"%3d",Anzahl);
  write_text(50,131,number,1);
  write_text(80,71,_(MSG_WIDTH),1);
  write_text(80,101,_(MSG_HEIGHT),1);
  write_text(80,131,_(MSG_BOMBS),1);
  drawfield(250,80,325,115);
  write_text(265,100,_(MSG_WEITER),2);

  drawfield(175,135,140,120);
  drawfield(180,120,215,135);
  write_text(145,131,"± 1",1);
  write_text(185,131,"±10",1);

  drawfield(40,20,115,40);
  write_text(45,33,_(MSG_BEGINNER),3);
  drawfield(120,20,260,40);
  write_text(125,33,_(MSG_ADVANCED),3);
  drawfield(265,20,325,40);
  write_text(270,33,_(MSG_PROFESSIONAL),3);

  switch(Spielart)
  {
    case ANFAENGER        : drawfield(115,40,40,20);
                            break;
    case FORTGESCHRITTENE : drawfield(260,40,120,20);
                            break;
    case PROFIS           : drawfield(325,40,265,20);
    			    break;
    default               : break;
  }

  drawfield(130,150,230,165);
  write_text(140,161,_(MSG_HIGHSCORES),3);

  drawfield(250,150,265,165);
  write_text(275,161,_(MSG_CHEAT),2);
  if(Cheat)
  {
    SetAPen(rp,1);
  }
  else
  {
    SetAPen(rp,0);
  }
  Move(rp,253,158);
  Draw(rp,257,161);
  Draw(rp,262,153);

  write_text(160,85,"Menü",2);

}

void zeigehighscores()
{
char outtext[27];
BOOL weiter=FALSE;

clearwin();
  if(WinSize(Window,250,200)==FALSE)
    fprintf(stderr,"%s",_(MSG_ERRORWINRESIZE));
/*  clearwin();*/
  write_text(75,25,"HighScores:",1);
  sprintf(outtext,"%3d %s %s",Zeiten[ANFAENGER],_(MSG_SEC),_(MSG_BEGINNER));
  write_text(30,45,outtext,2);
  write_text(62,57,names[1],2);
  sprintf(outtext,"%3d %s %s",Zeiten[FORTGESCHRITTENE],_(MSG_SEC),_(MSG_ADVANCED));
  write_text(30,75,outtext,2);
  write_text(62,87,names[2],2);
  sprintf(outtext,"%3d %s %s",Zeiten[PROFIS],_(MSG_SEC),_(MSG_PROFESSIONAL));
  write_text(30,105,outtext,2);
  write_text(62,117,names[3],2);
  drawfield(100,125,150,150);
  write_text(102,140,_(MSG_WEITER),2);

  while(!weiter)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX-Window->BorderLeft;
    mausy=msg->MouseY-Window->BorderTop;
    ReplyMsg((struct Message *)msg);
    if((class==IDCMP_MOUSEBUTTONS)&&(code==SELECTUP)&&(mausx>100)&&(mausx<150)&&(mausy>125)&&(mausy<150))
      weiter=TRUE;
  }
  MaleMenue();
}

void Menue()
{
BOOL weiter=FALSE;
char number[4];
BYTE deltab=1;

  MaleMenue();
  while(!weiter)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX-Window->BorderLeft;
    mausy=msg->MouseY-Window->BorderTop;
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_CLOSEWINDOW  : weiter=TRUE;
                          ende=TRUE;
                          break;
      case IDCMP_MOUSEBUTTONS : if(code==SELECTUP)
                          {
                            if(mausx>40&&mausx<130)
                            {
                              if(mausy>60&&mausy<75)
                              {
                                Spielart=EIGENDEF;
                                if(width++==maxwidth)
                                  width=5;
                              }
                              if(mausy>90&&mausy<105)
                              {
                                Spielart=EIGENDEF;
                                if(height++==maxheight)
                                  height=5;
                              }
                              if(mausy>120&&mausy<135)
                              {
                                Spielart=EIGENDEF;
                                Anzahl=Anzahl+deltab;
                                if(Anzahl>width*height-1)
                                  Anzahl=1;
                              }
                            }
                            if(mausx>250&&mausx<265&&mausy>150&&mausy<165)
                            {
                              Cheat=1-Cheat;
                              if(Cheat)
                              {
                                SetAPen(rp,1);
                              }
                              else
                              {
                                SetAPen(rp,0);
                              }
                              Move(rp,253,158);
                              Draw(rp,257,161);
                              Draw(rp,262,153);
                            }
                            if(mausx>140&&mausx<175&&mausy>120&&mausy<135)
                            {
                              deltab=1;
                              drawfield(175,135,140,120);
                              drawfield(180,120,215,135);
                            }
                            if(mausx>180&&mausx<215&&mausy>120&&mausy<135)
                            {
                              deltab=10;
                              drawfield(140,120,175,135);
                              drawfield(215,135,180,120);
                            }
                            if(mausy>20&&mausy<40)
                            {
          		      if(mausx>40&&mausx<115)
                              {
                                Spielart=ANFAENGER;
                                width=8;
                                height=8;
                                Anzahl=10;
                              }
                              if(mausx>120&&mausx<260)
                              {
                                Spielart=FORTGESCHRITTENE;
                                width=16;
                                height=16;
                                Anzahl=40;
                              }
                              if(mausx>265&&mausx<325)
                              {
                                Spielart=PROFIS;
                                width=30;
                                height=16;
                                Anzahl=99;
                              }
                            }
                            if((mausx>250)&&(mausx<325)&&(mausy>80)&&(mausy<115))
                              weiter=TRUE;
                            if((mausx>130)&&(mausx<230)&&(mausy>150)&&(mausy<165))
                              zeigehighscores();
                          }
                          else if(code==MENUUP)
                          {
                          if(mausx>40&&mausx<130)
                          {
                            if(mausy>60&&mausy<75)
                              if(--width==4)
                              {
                                Spielart=EIGENDEF;
                                width=maxwidth;
                              }
                            if(mausy>90&&mausy<105)
                              if(--height==4)
                              {
                                Spielart=EIGENDEF;
                                height=maxheight;
                              }
                            if(mausy>120&&mausy<135)
                            {
                              Spielart=EIGENDEF;
                              Anzahl=Anzahl-deltab;
                              if(Anzahl<1)
                                Anzahl=width*height-1;
                            }
                          }
                          }
                          break;
      default           : break;
    }

    drawfield(40,20,115,40);
    drawfield(120,20,260,40);
    drawfield(265,20,325,40);
    switch(Spielart)
    {
      case ANFAENGER        : drawfield(115,40,40,20);
                              break;
      case FORTGESCHRITTENE : drawfield(260,40,120,20);
                              break;
      case PROFIS           : drawfield(325,40,265,20);
    	                      break;
      default               : break;
    }

    if(Anzahl>width*height-2)
      Anzahl=width*height-1;
    sprintf(number,"%3d",width);
    write_text(50,71,number,1);
    sprintf(number,"%3d",height);
    write_text(50,101,number,1);
    sprintf(number,"%3d",Anzahl);
    write_text(50,131,number,1);
  }
}

void MaleSpielfeld()
{
BYTE x,y;
  clearwin();

  for(x=0;x<width;x++)
    drawfield(x*box_width+left+1,oben+1,(x+1)*box_width+left-1,box_width+oben-1);
  for(y=1;y<height;y++)
    copypic(left,oben+1,width*box_width,box_width-1,left,oben+y*box_width+1);

  drawfield(left-6,oben-6,width*box_width+5+left,height*box_width+5+oben);
  drawfield(width*box_width+6+left,height*box_width+6+oben,left-7,oben-7);
  drawfield(left+box_width*width/2-25,5,left+box_width*width/2+25,35);
}

void GameInit()
{
  BYTE x,y,a;

  if(menuean)
    Menue();
  if(!ende)
  {

clearwin();
    if(WinSize(Window,width*box_width+left+right,height*box_width+oben+unten+5)==FALSE)
      fprintf(stderr,"%s",_(MSG_ERRORWINRESIZE));
/*    clearwin();*/


/* --------------------------------- Var-Init ------------------------------- */

    for(x=1;x<=maxwidth;x++)
      for(y=1;y<=maxheight;y++)
      {
        Karte[x][y]=FALSE;
        Spielfeld[x][y]=GESCHLOSSEN;
      }

    for(x=0;x<=(maxwidth+1);x++)
    {
      Karte[x][maxheight+1]=FALSE;
      Karte[x][0]=FALSE;
    }

    for(y=1;y<=maxheight;y++)
    {
      Karte[maxwidth+1][y]=FALSE;
      Karte[0][y]=FALSE;
    }

    for(a=1;a<=Anzahl;a++)
    {
      while(Karte[x=random(1,width)][y=random(1,height)])
        Karte[x][y]=TRUE;
    }

    maxx=width*box_width-1+left;
    maxy=height*box_width-1+oben;

    Fehler=FALSE;
    SpielAbbr=FALSE;
    Rest=width*height;
    AnzMarken=0;

    MaleSpielfeld();
    write_text(left+box_width*width/2-16,23,_(MSG_STOP),2);

  }
}

int AnzNachbMinen(int x, int y)
{
  int Anz=0,dx,dy;
  for(dy=-1;dy<2;dy++)
    for(dx=-1;dx<2;dx++)
      Anz=Anz+Karte[x+dx][y+dy];
  return(Anz);
}

void FeldZahl(int x, int y, int anz)
{
  char outtext[2];
  sprintf(outtext,"%d",anz);
  write_text((x-1)*box_width+6+left,(y-1)*box_width+13+oben,outtext,2);
}

void aufdecken()
{
BYTE naechster[maxheight*maxwidth][2],neuer[9][2],anzmin,a,b,x,y;
int nr=0,neu;

  naechster[nr][0]=Feldx;
  naechster[nr][1]=Feldy;

  EraseRect(rp,(Feldx-1)*box_width+1+left,(Feldy-1)*box_width+1+oben,Feldx*box_width-1+left,Feldy*box_width+oben-1);
  anzmin=AnzNachbMinen(Feldx,Feldy);
  Spielfeld[Feldx][Feldy]=anzmin;
  Rest--;

  if(anzmin==0)
  {
    while(nr>-1)
    {
      neu=0;
      for(a=-1;a<2;a++)
        for(b=-1;b<2;b++)
        {
          if(!((a==0&&b==0)||(naechster[nr][0]+a<1)||(naechster[nr][0]+a>width)||(naechster[nr][1]+b<1)||(naechster[nr][1]+b>height)||(!(Spielfeld[naechster[nr][0]+a][naechster[nr][1]+b]==GESCHLOSSEN))))
          {
            Rest--;
            x=naechster[nr][0];
            y=naechster[nr][1];
            EraseRect(rp,(x+a-1)*box_width+1+left,(y+b-1)*box_width+1+oben,(x+a)*box_width-1+left,(y+b)*box_width-1+oben);
            anzmin=AnzNachbMinen((x+a),(y+b));
            Spielfeld[x+a][y+b]=anzmin;
            if(anzmin==0)
            {
              neuer[neu][0]=x+a;
              neuer[neu][1]=y+b;
              neu++;
            }
            else
            {
              FeldZahl((x+a),(y+b),anzmin);
            }
          }
        }
      for(a=0;a<neu;a++)
      {
        naechster[nr][0]=neuer[a][0];
        naechster[nr][1]=neuer[a][1];
        nr++;
      }
      nr--;
    }
  }
  else
  {
    FeldZahl(Feldx,Feldy,anzmin);
  }

}

void markeentfernen()
{
  EraseRect(rp,(Feldx-1)*box_width+2+left,(Feldy-1)*box_width+2+oben,Feldx*box_width-2+left,Feldy*box_width-2+oben);
  Spielfeld[Feldx][Feldy]=GESCHLOSSEN;
  AnzMarken--;
}

void markesetzen()
{
SetAPen(rp,3);
  RectFill(rp,(Feldx-1)*box_width+2+left,(Feldy-1)*box_width+2+oben,Feldx*box_width-2+left,Feldy*box_width-2+oben);
  Spielfeld[Feldx][Feldy]=MARKE;
  AnzMarken++;
}

int anzfelder(int x, int y)
{
int a,b,wert;
  wert=0;
  for(a=-1;a<2;a++)
    for(b=-1;b<2;b++)
      if(Spielfeld[x+a][y+b]<0&&x+a<=width&&x+a>0&&y+b<=height&&y+b>0)
        wert++;
  return(wert);
}

int anzminen(int x, int y)
{
int a,b,wert;
  wert=0;
  for(a=-1;a<2;a++)
    for(b=-1;b<2;b++)
      if(Spielfeld[x+a][y+b]==MARKE&&x+a<=width&&x+a>0&&y+b<=height&&y+b>0)
        wert++;
  return(wert);
}

BOOL loesen()
{
int a,b,c,d;
BOOL weiter=TRUE,Abbr=FALSE;
  while(weiter&&!Abbr)
  {
    weiter=FALSE;
    for(a=1;a<=width;a++)
      for(b=1;b<=height;b++)
        if(Spielfeld[a][b]>0)
          if(Spielfeld[a][b]==anzfelder(a,b))
            for(c=-1;c<2;c++)
              for(d=-1;d<2;d++)
                if(Spielfeld[a+c][b+d]==GESCHLOSSEN&&a+c<=width&&a+c>0&&b+d<=height&&b+d>0)
                {
                  Feldx=a+c;
                  Feldy=b+d;
                  markesetzen();
                  weiter=TRUE;
                }

    for(a=1;a<=width;a++)
      for(b=1;b<=height;b++)
        if(Spielfeld[a][b]>0)
          if(Spielfeld[a][b]==anzminen(a,b))
            for(c=-1;c<2;c++)
              for(d=-1;d<2;d++)
                if(Spielfeld[a+c][b+d]==GESCHLOSSEN&&a+c<=width&&a+c>0&&b+d<=height&&b+d>0)
                {
                  Feldx=a+c;
                  Feldy=b+d;
                  if(Karte[Feldx][Feldy])
                    Abbr=TRUE;
                  else
                  {
                    aufdecken();
                    weiter=TRUE;
                  }
                }
  }
  return(Abbr);
}

void Spiel()
{
BOOL start=FALSE;
int oldx=-1, oldy=-1;

  /* Init timer */
  finish = FALSE;

  while(!Fehler && Rest>Anzahl && !ende && !SpielAbbr)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX-Window->BorderLeft;
    mausy=msg->MouseY-Window->BorderTop;
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_CLOSEWINDOW  : ende=AutoRequest(Window,&q_gbody,&p,&n,0L,0L,200,75);
                                SpielAbbr=ende;
                          break;
      case IDCMP_MOUSEBUTTONS : if(start==FALSE)
                          {
                            time(&tstart);
                            start=TRUE;
                            Signal(t,1<<sigbit2); /* Tell timer to start off */
                            Wait(1<<sigbit1); /* Wait for his signal */
                            time(&tstart);
                          }
                          if(code==SELECTUP&&oldx!=-1)
                          {
                            Feldx=(mausx-left) / box_width +1;
                            Feldy=(mausy-oben) / box_width +1;
                            drawfield((oldx-1)*box_width+left+1,(oldy-1)*box_width+oben+1,oldx*box_width+left-1,oldy*box_width+oben-1);
                            if(Feldx==oldx&&Feldy==oldy)
                            {
                              if(Spielfeld[Feldx][Feldy]==GESCHLOSSEN) 
                              {
                                if(Karte[Feldx][Feldy])
                                {
                                  Fehler=TRUE;
                                }
                                else
                                {
                                  aufdecken();
                                }
                              }
                            }
                            oldx=-1;
                            oldy=-1;
                            if(Cheat)Fehler=(Fehler?Fehler:loesen());
                          }
                          if((code==SELECTUP)&&(mausx>left+box_width*width/2-25)&&(mausy>5)&&(mausx<left+box_width*width/2+25)&&(mausy<35))
                          {
                            SpielAbbr=TRUE;
                          }
                          if(code==MENUUP&&oldx!=-1)
                          {
                            Feldx=(mausx-left) / box_width +1;
                            Feldy=(mausy-oben) / box_width +1;
                            if(Feldx==oldx&&Feldy==oldy)
                            {
                              switch(Spielfeld[Feldx][Feldy])
                              {
                                case GESCHLOSSEN : markesetzen();
                                                   break;
                                case MARKE       : markeentfernen();
                                                   break;
                                default          : break;
                              }
                            }
                            else
                            {
                              if(Spielfeld[oldx][oldy]==MARKE)
                              {
                                SetAPen(rp,3);
                              }
                              else
                              {
                                SetAPen(rp,0);
                              }
                              RectFill(rp,(oldx-1)*box_width+2+left,(oldy-1)*box_width+2+oben,oldx*box_width-2+left,oldy*box_width-2+oben);
                            }
                            oldx=-1;
                            oldy=-1;
                            if(Cheat)Fehler=(Fehler?Fehler:loesen());
                          }
                          if(code==MENUDOWN&&(mausx>left)&&(mausy>oben)&&(mausx<maxx)&&(mausy<maxy)&&((mausx-left) % box_width)&&(((mausx-left) % box_width)<(box_width-1))&&((mausy-oben) % box_width)&&(((mausy-oben) % box_width)<(box_width-1)))
                          {
                            Feldx=(mausx-left) / box_width +1;
                            Feldy=(mausy-oben) / box_width +1;
                            if(Spielfeld[Feldx][Feldy]==GESCHLOSSEN||Spielfeld[Feldx][Feldy]==MARKE) 
                            {
                              SetAPen(rp,2);
                              RectFill(rp,(Feldx-1)*box_width+2+left,(Feldy-1)*box_width+2+oben,Feldx*box_width-2+left,Feldy*box_width-2+oben);
                              oldx=Feldx;
                              oldy=Feldy;
                            }
                          }
                          if((code==SELECTDOWN)&&(mausx>left)&&(mausy>oben)&&(mausx<maxx)&&(mausy<maxy)&&((mausx-left) % box_width)&&(((mausx-left) % box_width)<(box_width-1))&&((mausy-oben) % box_width)&&(((mausy-oben) % box_width)<(box_width-1)))
                          {
                            Feldx=(mausx-left) / box_width +1;
                            Feldy=(mausy-oben) / box_width +1;
                            if(Spielfeld[Feldx][Feldy]==GESCHLOSSEN) 
                            {
                              drawfield(Feldx*box_width+left-1,Feldy*box_width+oben-1,(Feldx-1)*box_width+left+1,(Feldy-1)*box_width+oben+1);
                              oldx=Feldx;
                              oldy=Feldy;
                            }
                          }
                          break;
    }
  }
  /* stop timer */
  if(start)
  {
    finish = TRUE;
    Wait(1<<sigbit1); /* Wait for timer to stop */
  }
}

void Auswertung()
{
BOOL weiter=FALSE;
int x,y;
char Zeit[30];
  if(!ende&&!SpielAbbr)
  {

    if(Fehler)
    {
      SetAPen(rp,1);
      RectFill(rp,(Feldx-1)*box_width+left+3,(Feldy-1)*box_width+oben+3,Feldx*box_width+left-3,Feldy*box_width+oben-3);    
      for(y=1;y<=height;y++)
        for(x=1;x<=width;x++)
        {
          if(Karte[x][y]&&Spielfeld[x][y]==GESCHLOSSEN)
            RectFill(rp,(x-1)*box_width+3+left,(y-1)*box_width+3+oben,x*box_width-3+left,y*box_width-3+oben);
          if(Karte[x][y]==FALSE&&Spielfeld[x][y]==MARKE)
          {
            Move(rp,(x-1)*box_width+6+left,(y-1)*box_width+6+oben);
            Draw(rp,x*box_width-6+left,y*box_width-6+oben);
            Move(rp,(x-1)*box_width+6+left,y*box_width-6+oben);
            Draw(rp,x*box_width-6+left,(y-1)*box_width+6+oben);
          }
        }
    }
    else
    {
      for(y=1;y<=height;y++)
        for(x=1;x<=width;x++)
          if(Spielfeld[x][y]==GESCHLOSSEN)
	    SetAPen(rp,3),
            RectFill(rp,(x-1)*box_width+2+left,(y-1)*box_width+2+oben,x*box_width-2+left,y*box_width-2+oben);

      sprintf(Zeit,"%s %4d %s",_(MSG_FINISHED),(int)(tend-tstart),_(MSG_SEC));
      write_text(left+width*box_width/2-65,oben+height*box_width+19,Zeit,1);
      if((tend-tstart)<Zeiten[Spielart]&&!Cheat)
      {
        clearwin();

        Name_Gad.LeftEdge=(width*box_width+left+right)/2-100;
        AddGadget(Window,&Name_Gad,0);
        RefreshGadgets(&Name_Gad,Window,NULL);
        write_text(left+width*box_width/2-65,oben+height*box_width+15,Zeit,1);

        while(!weiter)
        {
          WaitPort(Window->UserPort);
          msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
          class=msg->Class;
          code=msg->Code;
          ReplyMsg((struct Message *)msg);
          if(class==IDCMP_GADGETUP)
            weiter=TRUE;
        }

        Zeiten[Spielart]=tend-tstart;
        strcpy(names[Spielart],Name_Gad_buf);
        RemoveGadget(Window,&Name_Gad);
        clearwin();
        drawfield(left+box_width*width/2-25,5,left+box_width*width/2+25,35);
      }
    }
    write_text(left+box_width*width/2-23,23,_(MSG_WEITER),2);

    while(!weiter)
    {
      WaitPort(Window->UserPort);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      code=msg->Code;
      mausx=msg->MouseX-Window->BorderLeft;
      mausy=msg->MouseY-Window->BorderTop;
      ReplyMsg((struct Message *)msg);
      if((class==IDCMP_MOUSEBUTTONS)&&(code==SELECTUP)&&(mausx>left+box_width*width/2-25)&&(mausy>5)&&(mausx<left+box_width*width/2+25)&&(mausy<35))
        weiter=TRUE;
      if(class==IDCMP_CLOSEWINDOW)
      {
        weiter=WEnde=ende=AutoRequest(Window,&q_body,&p,&n,0L,0L,200,75);
      }
    }
  }
}
