/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: Quad Game
    Lang: german
*/

/*****************************************************************************

    NAME

        Quad

    SYNOPSIS

    LOCATION

        Workbench:Games

    BUGS

    SEE ALSO

        Jump JumpEd Mine Wumpus

    INTERNALS

    HISTORY

        24-Aug-1997     hkiel     Initial inclusion into the AROS tree

******************************************************************************/

static const char version[] = "$VER: Quad 0.1 (29.08.1997)\n";

#include "../prec.c"
#include "Quad.h"
#include "QuadIncl.h"

int ende=0,weiter,feld[2][9];
int nummer;

int mineins()
{
int i;
int z=0;
  for(i=0;i<9;i++)
    z+=feld[0][i];
if(z!=9)
  return(1);
else
  return(0);
}

int test()
{
int i;
int z=0;
  for(i=0;i<9;i++)
    if(feld[0][i]==feld[1][i])z++;
if(z==9)
  return(1);
else
  return(0);
}

updatebuttons()
{
 Anfang1.Flags=(GFLG_GADGHIMAGE+feld[0][0]*GFLG_SELECTED);
 Anfang2.Flags=(GFLG_GADGHIMAGE+feld[0][1]*GFLG_SELECTED);
 Anfang3.Flags=(GFLG_GADGHIMAGE+feld[0][2]*GFLG_SELECTED);
 Anfang4.Flags=(GFLG_GADGHIMAGE+feld[0][3]*GFLG_SELECTED);
 Anfang5.Flags=(GFLG_GADGHIMAGE+feld[0][4]*GFLG_SELECTED);
 Anfang6.Flags=(GFLG_GADGHIMAGE+feld[0][5]*GFLG_SELECTED);
 Anfang7.Flags=(GFLG_GADGHIMAGE+feld[0][6]*GFLG_SELECTED);
 Anfang8.Flags=(GFLG_GADGHIMAGE+feld[0][7]*GFLG_SELECTED);
 Anfang9.Flags=(GFLG_GADGHIMAGE+feld[0][8]*GFLG_SELECTED);
 Ziel1.Flags=(GFLG_GADGHIMAGE+feld[1][0]*GFLG_SELECTED);
 Ziel2.Flags=(GFLG_GADGHIMAGE+feld[1][1]*GFLG_SELECTED);
 Ziel3.Flags=(GFLG_GADGHIMAGE+feld[1][2]*GFLG_SELECTED);
 Ziel4.Flags=(GFLG_GADGHIMAGE+feld[1][3]*GFLG_SELECTED);
 Ziel5.Flags=(GFLG_GADGHIMAGE+feld[1][4]*GFLG_SELECTED);
 Ziel6.Flags=(GFLG_GADGHIMAGE+feld[1][5]*GFLG_SELECTED);
 Ziel7.Flags=(GFLG_GADGHIMAGE+feld[1][6]*GFLG_SELECTED);
 Ziel8.Flags=(GFLG_GADGHIMAGE+feld[1][7]*GFLG_SELECTED);
 Ziel9.Flags=(GFLG_GADGHIMAGE+feld[1][8]*GFLG_SELECTED);
 RefreshGList(FIRSTGADGET,Window,NULL,20);
}

main()
{
int i;
  oeffnelib();
  oeffnewindow();
  for(i=0;i<9;i++)
  {
    feld[0][i]=0;
    feld[1][i]=0;
  }
  while(ende==0)
  {
    weiter=0;
    while(weiter==0&&ende==0)
    {
      Wait(1L<<Window->UserPort->mp_SigBit);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      switch(class)
      {
        case IDCMP_RAWKEY      :
        case IDCMP_CLOSEWINDOW : ende=1;
                           break;
        case IDCMP_GADGETUP    : nummer=(((struct Gadget *)(msg->IAddress))->GadgetID);
                           if(nummer>0&&nummer<10)
                             feld[0][nummer-1]=1-(feld[0][nummer-1]);
                           if(nummer>10)
                             feld[1][nummer-11]=1-(feld[1][nummer-11]);
                           if(nummer==0)
                             weiter=mineins();
                           break;
        default          : break;
      }
      ReplyMsg((struct Message *)msg);
      updatebuttons();
    }
    sprintf(starttext,"-----");
    sprintf(stoptext,"Stop");
    RefreshGList(FIRSTGADGET,Window,NULL,20);
    weiter=0;
    while(weiter==0&&ende==0&&(test()==0))
    {
      Wait(1L<<Window->UserPort->mp_SigBit);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      switch(class)
      {
        case IDCMP_RAWKEY      :
        case IDCMP_CLOSEWINDOW : ende=1;
                           break;
        case IDCMP_GADGETUP    : nummer=(((struct Gadget *)(msg->IAddress))->GadgetID);
                           switch(nummer)
                           {
                             case 1 : if(feld[0][0]==0)
                                      {
                                        feld[0][0]=1-(feld[0][0]);
                                        feld[0][1]=1-(feld[0][1]);
                                        feld[0][3]=1-(feld[0][3]);
                                        feld[0][4]=1-(feld[0][4]);
                                      }
                                      break;
                             case 2 : if(feld[0][1]==0)
                                      {
                                        feld[0][1]=1-(feld[0][1]);
                                        feld[0][0]=1-(feld[0][0]);
                                        feld[0][2]=1-(feld[0][2]);
                                      }
                                      break;
                             case 3 : if(feld[0][2]==0)
                                      {
                                        feld[0][2]=1-(feld[0][2]);
                                        feld[0][1]=1-(feld[0][1]);
                                        feld[0][5]=1-(feld[0][5]);
                                        feld[0][4]=1-(feld[0][4]);
                                      }
                                      break;
                             case 4 : if(feld[0][3]==0)
                                      {
                                        feld[0][3]=1-(feld[0][3]);
                                        feld[0][0]=1-(feld[0][0]);
                                        feld[0][6]=1-(feld[0][6]);
                                      }
                                      break;
                             case 5 : if(feld[0][4]==0)
                                      {
                                        feld[0][4]=1-(feld[0][4]);
                                        feld[0][1]=1-(feld[0][1]);
                                        feld[0][3]=1-(feld[0][3]);
                                        feld[0][5]=!(feld[0][5]);
                                        feld[0][7]=!(feld[0][7]);
                                      }
                                      break;
                             case 6 : if(feld[0][5]==0)
                                      {
                                        feld[0][5]=1-(feld[0][5]);
                                        feld[0][8]=1-(feld[0][8]);
                                        feld[0][2]=1-(feld[0][2]);
                                      }
                                      break;
                             case 7 : if(feld[0][6]==0)
                                      {
                                        feld[0][6]=1-(feld[0][6]);
                                        feld[0][7]=1-(feld[0][7]);
                                        feld[0][3]=1-(feld[0][3]);
                                        feld[0][4]=1-(feld[0][4]);
                                      }
                                      break;
                             case 8 : if(feld[0][7]==0)
                                      {
                                        feld[0][7]=1-(feld[0][7]);
                                        feld[0][6]=1-(feld[0][6]);
                                        feld[0][8]=1-(feld[0][8]);
                                      }
                                      break;
                             case 9 : if(feld[0][8]==0)
                                      {
                                        feld[0][8]=1-(feld[0][8]);
                                        feld[0][7]=1-(feld[0][7]);
                                        feld[0][5]=1-(feld[0][5]);
                                        feld[0][4]=1-(feld[0][4]);
                                      }
                                      break;
                             case 10: weiter=1;
                                      break;
                             default: break;
                           }
                           break;
        default          : break;
      }
      ReplyMsg((struct Message *)msg);
      updatebuttons();
    }
    sprintf(starttext,"Start");
    sprintf(stoptext,"----");
    RefreshGList(FIRSTGADGET,Window,NULL,20);
  }

  schliessewindow();
  schliesselib();
}
