void MaleMenue()
{
char Zahl[4];

StopMsg();

LoescheWin();
  WinSize(Window,350,200);
/*  LoescheWin();*/
  maleFeld(40,60,130,75);
  maleFeld(40,90,130,105);
  maleFeld(40,120,130,135);
  sprintf(Zahl,"%3d",breite);
  schreibe(50,72,Zahl,1);
  sprintf(Zahl,"%3d",hoehe);
  schreibe(50,102,Zahl,1);
  sprintf(Zahl,"%3d",Anzahl);
  schreibe(50,132,Zahl,1);
  schreibe(80,72,"Breite",1);
  schreibe(80,102,"Hoehe",1);
  schreibe(80,132,"Bomben",1);
  maleFeld(250,80,325,115);
  schreibe(260,102,"Weiter",2);

  maleFeld(175,135,140,120);
  maleFeld(180,120,215,135);
  schreibe(145,132,"± 1",1);
  schreibe(185,132,"±10",1);

  maleFeld(40,20,115,40);
  schreibe(45,35,"Anfänger",3);
  maleFeld(120,20,260,40);
  schreibe(125,35,"Fortgeschrittene",3);
  maleFeld(265,20,325,40);
  schreibe(270,35,"Profis",3);

  switch(Spielart)
  {
    case ANFAENGER        : maleFeld(115,40,40,20);
                            break;
    case FORTGESCHRITTENE : maleFeld(260,40,120,20);
                            break;
    case PROFIS           : maleFeld(325,40,265,20);
    			    break;
    default               : break;
  }

  maleFeld(130,150,230,165);
  schreibe(140,162,"HighScores",3);

  schreibe(160,87,"Menü",2);

ContMsg();

}

void zeigehighscores()
{
char outtext[27];
BOOL weiter=FALSE;

LoescheWin();
  WinSize(Window,250,200);
/*  LoescheWin();*/
  schreibe(75,25,"HighScores:",1);
  sprintf(outtext,"%3d sek. Anfänger",Zeiten[ANFAENGER]);
  schreibe(30,45,outtext,2);
  schreibe(62,57,Namen[1],2);
  sprintf(outtext,"%3d sek. Fortgeschrittene",Zeiten[FORTGESCHRITTENE]);
  schreibe(30,75,outtext,2);
  schreibe(62,87,Namen[2],2);
  sprintf(outtext,"%3d sek. Profis",Zeiten[PROFIS]);
  schreibe(30,105,outtext,2);
  schreibe(62,117,Namen[3],2);
  maleFeld(100,125,150,150);
  schreibe(102,142,"Weiter",2);

  while(!weiter)
  {
    Wait(1L<<Window->UserPort->mp_SigBit);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX;/* -3;  AROS*/
    mausy=msg->MouseY;/* -13; AROS*/
    ReplyMsg((struct Message *)msg);
    if((class==IDCMP_MOUSEBUTTONS)&&(code==SELECTUP)&&(mausx>100)&&(mausx<150)&&(mausy>125)&&(mausy<150))
      weiter=TRUE;
  }
  MaleMenue();
}

void Menue()
{
BOOL weiter=FALSE;
char Zahl[4];
SHORT deltab=1;

  MaleMenue();
  while(!weiter)
  {
    Wait(1L<<Window->UserPort->mp_SigBit);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX;/* -3; AROS*/
    mausy=msg->MouseY;/* -13; AROS*/
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_RAWKEY :
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
                                if(breite++==maxbreite)
                                  breite=5;
                              }
                              if(mausy>90&&mausy<105)
                              {
                                Spielart=EIGENDEF;
                                if(hoehe++==maxhoehe)
                                  hoehe=5;
                              }
                              if(mausy>120&&mausy<135)
                              {
                                Spielart=EIGENDEF;
                                Anzahl=Anzahl+deltab;
                                if(Anzahl>breite*hoehe-1)
                                  Anzahl=1;
                              }
                            }
                            if(mausx>140&&mausx<175&&mausy>120&&mausy<135)
                            {
                              deltab=1;
                              maleFeld(175,135,140,120);
                              maleFeld(180,120,215,135);
                            }
                            if(mausx>180&&mausx<215&&mausy>120&&mausy<135)
                            {
                              deltab=10;
                              maleFeld(140,120,175,135);
                              maleFeld(215,135,180,120);
                            }
                            if(mausy>20&&mausy<40)
                            {
          		      if(mausx>40&&mausx<115)
                              {
                                Spielart=ANFAENGER;
                                breite=8;
                                hoehe=8;
                                Anzahl=10;
                              }
                              if(mausx>120&&mausx<260)
                              {
                                Spielart=FORTGESCHRITTENE;
                                breite=16;
                                hoehe=16;
                                Anzahl=40;
                              }
                              if(mausx>265&&mausx<325)
                              {
                                Spielart=PROFIS;
                                breite=30;
                                hoehe=16;
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
                              if(--breite==4)
                              {
                                Spielart=EIGENDEF;
                                breite=maxbreite;
                              }
                            if(mausy>90&&mausy<105)
                              if(--hoehe==4)
                              {
                                Spielart=EIGENDEF;
                                hoehe=maxhoehe;
                              }
                            if(mausy>120&&mausy<135)
                            {
                              Spielart=EIGENDEF;
                              Anzahl=Anzahl-deltab;
                              if(Anzahl<1)
                                Anzahl=breite*hoehe-1;
                            }
                          }
                          }
                          break;
      default           : break;
    }

    maleFeld(40,20,115,40);
    maleFeld(120,20,260,40);
    maleFeld(265,20,325,40);
    switch(Spielart)
    {
      case ANFAENGER        : maleFeld(115,40,40,20);
                              break;
      case FORTGESCHRITTENE : maleFeld(260,40,120,20);
                              break;
      case PROFIS           : maleFeld(325,40,265,20);
    	                      break;
      default               : break;
    }

    if(Anzahl>breite*hoehe-2)
      Anzahl=breite*hoehe-1;
    sprintf(Zahl,"%3d",breite);
    schreibe(50,72,Zahl,1);
    sprintf(Zahl,"%3d",hoehe);
    schreibe(50,102,Zahl,1);
    sprintf(Zahl,"%3d",Anzahl);
    schreibe(50,132,Zahl,1);
  }
}

void MaleSpielfeld()
{
SHORT x,y;
  LoescheWin();
/* AROS */
for(y=0;y<hoehe;y++)
  for(x=0;x<breite;x++)
    maleFeld(x*feldbreite+links+1,oben+1+y*feldbreite,(x+1)*feldbreite+links-1,feldbreite+oben-1+y*feldbreite);

/*    maleFeld(x*feldbreite+links+1,oben+1,(x+1)*feldbreite+links-1,feldbreite+oben-1); */
/*  for(y=1;y<hoehe;y++) */
/* AROS unimplemented */
/*    copypic(links,oben,breite*feldbreite,feldbreite,links,oben+y*feldbreite); */

  maleFeld(links-6,oben-6,breite*feldbreite+5+links,hoehe*feldbreite+5+oben);
  maleFeld(breite*feldbreite+6+links,hoehe*feldbreite+6+oben,links-7,oben-7);
  maleFeld(links+feldbreite*breite/2-25,5,links+feldbreite*breite/2+25,35);
}

void GameInit()
{
  SHORT x,y,a;

  if(menuean)
    Menue();
  if(!ende)
  {

StopMsg();

LoescheWin();
    WinSize(Window,breite*feldbreite+links+rechts,hoehe*feldbreite+oben+unten);
/*    LoescheWin();*/


/* --------------------------------- Var-Init ------------------------------- */

    for(x=1;x<=maxbreite;x++)
      for(y=1;y<=maxhoehe;y++)
      {
        Karte[x][y]=FALSE;
        Spielfeld[x][y]=GESCHLOSSEN;
      }

    for(x=0;x<=(maxbreite+1);x++)
    {
      Karte[x][maxhoehe+1]=FALSE;
      Karte[x][0]=FALSE;
    }

    for(y=1;y<=maxhoehe;y++)
    {
      Karte[maxbreite+1][y]=FALSE;
      Karte[0][y]=FALSE;
    }

    for(a=1;a<=Anzahl;a++)
    {
      while(Karte[x=random(1,breite)][y=random(1,hoehe)]);
        Karte[x][y]=TRUE;
    }

    maxx=breite*feldbreite-1+links;
    maxy=hoehe*feldbreite-1+oben;

    Fehler=FALSE;
    SpielAbbr=FALSE;
    Rest=breite*hoehe;
    AnzMarken=0;

    MaleSpielfeld();
    schreibe(links+feldbreite*breite/2-16,25,"Stop",2);

ContMsg();

  }
}

SHORT AnzNachbMinen(x,y)
SHORT x,y;
{
  SHORT Anz=0,dx,dy;
  for(dy=-1;dy<2;dy++)
    for(dx=-1;dx<2;dx++)
      Anz=Anz+Karte[x+dx][y+dy];
  return(Anz);
}

void FeldZahl(x,y,anz)
SHORT x,y,anz;
{
  char outtext[2];
  sprintf(outtext,"%d",anz);
  schreibe((x-1)*feldbreite+6+links,(y-1)*feldbreite+14+oben,outtext,2);
}

void aufdecken()
{
SHORT naechster[maxhoehe*maxbreite][2],neuer[9][2],anzmin,a,b,x,y;
int nr=0,neu;

StopMsg();

  naechster[nr][0]=Feldx;
  naechster[nr][1]=Feldy;

SetAPen(rp,0);
  RectFill(rp,(Feldx-1)*feldbreite+1+links,(Feldy-1)*feldbreite+1+oben,Feldx*feldbreite-1+links,Feldy*feldbreite+oben-1);
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
          if(!((a==0&&b==0)||(naechster[nr][0]+a<1)||(naechster[nr][0]+a>breite)||(naechster[nr][1]+b<1)||(naechster[nr][1]+b>hoehe)||(!(Spielfeld[naechster[nr][0]+a][naechster[nr][1]+b]==GESCHLOSSEN))))
          {
            Rest--;
            x=naechster[nr][0];
            y=naechster[nr][1];
SetAPen(rp,0);
            RectFill(rp,(x+a-1)*feldbreite+1+links,(y+b-1)*feldbreite+1+oben,(x+a)*feldbreite-1+links,(y+b)*feldbreite-1+oben);
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

ContMsg();

}

void markeentfernen()
{
SetAPen(rp,0);
  RectFill(rp,(Feldx-1)*feldbreite+2+links,(Feldy-1)*feldbreite+2+oben,Feldx*feldbreite-2+links,Feldy*feldbreite-2+oben);
  Spielfeld[Feldx][Feldy]=GESCHLOSSEN;
  AnzMarken--;
}

void markesetzen()
{
SetAPen(rp,3);
  RectFill(rp,(Feldx-1)*feldbreite+2+links,(Feldy-1)*feldbreite+2+oben,Feldx*feldbreite-2+links,Feldy*feldbreite-2+oben);
  Spielfeld[Feldx][Feldy]=MARKE;
  AnzMarken++;
}

void Anzeige()
{
char outtext[4];
  time(&tende);
  sprintf(outtext,"%3d",Anzahl-AnzMarken);
  schreibe(links+feldbreite*breite/2-55,25,outtext,1);
  sprintf(outtext,"%3d",(int)(tende-tstart));
  schreibe(links+feldbreite*breite/2+35,25,outtext,1);
}

SHORT anzfelder(x,y)
SHORT x,y;
{
SHORT a,b,wert;
  wert=0;
  for(a=-1;a<2;a++)
    for(b=-1;b<2;b++)
      if(Spielfeld[x+a][y+b]<0&&x+a<=breite&&x+a>0&&y+b<=hoehe&&y+b>0)
        wert++;
  return(wert);
}

SHORT anzminen(x,y)
SHORT x,y;
{
SHORT a,b,wert;
  wert=0;
  for(a=-1;a<2;a++)
    for(b=-1;b<2;b++)
      if(Spielfeld[x+a][y+b]==MARKE&&x+a<=breite&&x+a>0&&y+b<=hoehe&&y+b>0)
        wert++;
  return(wert);
}

BOOL loesen()
{
SHORT a,b,c,d;
BOOL weiter=TRUE,Abbr=FALSE;
  while(weiter&&!Abbr)
  {
    weiter=FALSE;
    for(a=1;a<=breite;a++)
      for(b=1;b<=hoehe;b++)
        if(Spielfeld[a][b]>0)
          if(Spielfeld[a][b]==anzfelder(a,b))
            for(c=-1;c<2;c++)
              for(d=-1;d<2;d++)
                if(Spielfeld[a+c][b+d]==GESCHLOSSEN&&a+c<=breite&&a+c>0&&b+d<=hoehe&&b+d>0)
                {
                  Feldx=a+c;
                  Feldy=b+d;
                  markesetzen();
                  weiter=TRUE;
                }

    for(a=1;a<=breite;a++)
      for(b=1;b<=hoehe;b++)
        if(Spielfeld[a][b]>0)
          if(Spielfeld[a][b]==anzminen(a,b))
            for(c=-1;c<2;c++)
              for(d=-1;d<2;d++)
                if(Spielfeld[a+c][b+d]==GESCHLOSSEN&&a+c<=breite&&a+c>0&&b+d<=hoehe&&b+d>0)
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
  time(&tstart);
  tende=tstart;
  Anzeige();
  while(!Fehler && Rest>Anzahl && !ende && !SpielAbbr)
  {
    Wait(1L<<Window->UserPort->mp_SigBit);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mausx=msg->MouseX;/* -3; AROS*/
    mausy=msg->MouseY;/* -13; AROS*/
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_CLOSEWINDOW  : Fehler=loesen();
                          break;
      case IDCMP_MOUSEBUTTONS : if(start==FALSE)
                          {
                            start=TRUE;
                            time(&tstart);
                          }
if(mausx<5&&mausy<5)Fehler=loesen();
                          if((code==SELECTUP)&&(mausx>links)&&(mausy>oben)&&(mausx<maxx)&&(mausy<maxy)&&((mausx-links) % feldbreite)&&(((mausx-links) % feldbreite)<(feldbreite-1))&&((mausy-oben) % feldbreite)&&(((mausy-oben) % feldbreite)<(feldbreite-1)))
                          {
                            Feldx=(mausx-links) / feldbreite +1;
                            Feldy=(mausy-oben) / feldbreite +1;
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
                          if((code==SELECTUP)&&(mausx>links+feldbreite*breite/2-25)&&(mausy>5)&&(mausx<links+feldbreite*breite/2+25)&&(mausy<35))
                          {
                            SpielAbbr=TRUE;
                          }
                          if(code==MENUUP&&(mausx>links)&&(mausy>oben)&&(mausx<maxx)&&(mausy<maxy)&&((mausx-links) % feldbreite)&&(((mausx-links) % feldbreite)<(feldbreite-1))&&((mausy-oben) % feldbreite)&&(((mausy-oben) % feldbreite)<(feldbreite-1)))
                          {
                            Feldx=(mausx-links) / feldbreite +1;
                            Feldy=(mausy-oben) / feldbreite +1;
                            switch(Spielfeld[Feldx][Feldy])
                            {
                              case GESCHLOSSEN : markesetzen();
                                                 break;
                              case MARKE       : markeentfernen();
                                                 break;
                              default          : break;
                            }
                          }
                          Anzeige();
                          break;
    }
  }
  time(&tende);
}

void Auswertung()
{
BOOL weiter=FALSE;
SHORT x,y;
char Zeit[30];
  if(!ende&&!SpielAbbr)
  {

StopMsg();

    if(Fehler)
    {
      SetAPen(rp,1);
      RectFill(rp,(Feldx-1)*feldbreite+links+3,(Feldy-1)*feldbreite+oben+3,Feldx*feldbreite+links-3,Feldy*feldbreite+oben-3);    
      for(y=1;y<=hoehe;y++)
        for(x=1;x<=breite;x++)
        {
          if(Karte[x][y]&&Spielfeld[x][y]==GESCHLOSSEN)
            RectFill(rp,(x-1)*feldbreite+3+links,(y-1)*feldbreite+3+oben,x*feldbreite-3+links,y*feldbreite-3+oben);
          if(Karte[x][y]==FALSE&&Spielfeld[x][y]==MARKE)
          {
            Move(rp,(x-1)*feldbreite+6+links,(y-1)*feldbreite+6+oben);
            Draw(rp,x*feldbreite-6+links,y*feldbreite-6+oben);
            Move(rp,(x-1)*feldbreite+6+links,y*feldbreite-6+oben);
            Draw(rp,x*feldbreite-6+links,(y-1)*feldbreite+6+oben);
          }
        }
    }
    else
    {
      for(y=1;y<=hoehe;y++)
        for(x=1;x<=breite;x++)
          if(Spielfeld[x][y]==GESCHLOSSEN)
	    SetAPen(rp,3),
            RectFill(rp,(x-1)*feldbreite+2+links,(y-1)*feldbreite+2+oben,x*feldbreite-2+links,y*feldbreite-2+oben);

      sprintf(Zeit,"Fertig! %4d Sek.",(int)(tende-tstart));
      schreibe(links+breite*feldbreite/2-65,oben+hoehe*feldbreite+20,Zeit,1);
      if((tende-tstart)<Zeiten[Spielart])
      {
        LoescheWin();

ContMsg();

        Name_Gad.LeftEdge=(breite*feldbreite+links+rechts)/2-100;
        AddGadget(Window,&Name_Gad,0);
        RefreshGadgets(&Name_Gad,Window,NULL);
        sprintf(Zeit,"Fertig! %4d Sek.",(int)(tende-tstart));
        schreibe(links+breite*feldbreite/2-65,oben+hoehe*feldbreite+20,Zeit,1);
        while(!weiter)
        {
          Wait(1L<<Window->UserPort->mp_SigBit);
          msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
          class=msg->Class;
          code=msg->Code;
          ReplyMsg((struct Message *)msg);
          if((class==IDCMP_MOUSEBUTTONS)&&(code==SELECTUP))
            weiter=TRUE;
        }

        Zeiten[Spielart]=tende-tstart;
        strcpy(Namen[Spielart],Name_Gad_buf);
        RemoveGadget(Window,&Name_Gad);

StopMsg();

        LoescheWin();
        maleFeld(links+feldbreite*breite/2-25,5,links+feldbreite*breite/2+25,35);
      }
    }
    schreibe(links+feldbreite*breite/2-23,25,"Weiter",2);

ContMsg();

    while(!weiter)
    {
      Wait(1L<<Window->UserPort->mp_SigBit);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      code=msg->Code;
      mausx=msg->MouseX;/* -3; AROS*/
      mausy=msg->MouseY;/* -13; AROS*/
      ReplyMsg((struct Message *)msg);
      if((class==IDCMP_MOUSEBUTTONS)&&(code==SELECTUP)&&(mausx>links+feldbreite*breite/2-25)&&(mausy>5)&&(mausx<links+feldbreite*breite/2+25)&&(mausy<35))
        weiter=TRUE;
    }
  }
}
