/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: Wumpus Game
    Lang: german
*/

/*****************************************************************************

    NAME

        Wumpus

    SYNOPSIS

    LOCATION

        Workbench:Games

    BUGS

    SEE ALSO

        Jump JumpEd Mine Quad

    INTERNALS

    HISTORY

        24-Aug-1997     hkiel     Initial inclusion into the AROS tree

******************************************************************************/

static const char version[] = "$VER: Wumpus 0.1 (29.08.1997)\n";

#include "WumpusIncl.h"

#define NICHT 0
#define WENIG 1
#define STARK 2

#define MPAUSE 300000

struct Hoehle
{
  SHORT Ausgang[3];
};

struct Hoehle Hoehlensystem[20]=
{
  {{ 2, 5, 6}},
  {{ 1, 3, 8}},
  {{ 2, 4,10}},
  {{ 3, 5,12}},
  {{ 1, 4,14}},
  {{ 1, 7,15}},
  {{ 6, 8,17}},
  {{ 2, 7, 9}},
  {{ 8,10,18}},
  {{ 3, 9,11}},
  {{10,12,19}},
  {{ 4,11,13}},
  {{12,14,20}},
  {{ 5,13,15}},
  {{ 6,14,16}},
  {{15,17,20}},
  {{ 7,16,18}},
  {{ 9,17,19}},
  {{11,18,20}},
  {{13,16,19}}
};
SHORT Wumpus,Speere,Fledermaus[3],Abgrund[3];
BOOL ende,wende=FALSE;


/* ---------------------------- Landkarte Gadgets --------------------------- */

SHORT SharedBordersPairs0[] = {
  0,0,0,18,1,18,1,0,37,0 };
SHORT SharedBordersPairs1[] = {
  1,18,37,18,37,1,38,0,38,18 };

struct Border SharedBorders[] = {
  {0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[1]},
  {0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs0[0],NULL},
  {0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[3]},
  {0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs0[0],NULL} };

struct IntuiText Eingang20_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"20",NULL };

#define Eingang20_ID    19

struct Gadget Eingang20 = {
  NULL,12,262,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang20_text,0L,NULL,Eingang20_ID,NULL };

struct IntuiText Eingang19_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"19",NULL };

#define Eingang19_ID    18

struct Gadget Eingang19 = {
  &Eingang20,285,316,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang19_text,0L,NULL,Eingang19_ID,NULL };

struct IntuiText Eingang18_text = {
  1,0,JAM1,9,14,NULL,(UBYTE *)"18",NULL };

#define Eingang18_ID    17

struct Gadget Eingang18 = {
  &Eingang19,517,249,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang18_text,0L,NULL,Eingang18_ID,NULL };

struct IntuiText Eingang17_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"17",NULL };

#define Eingang17_ID    16

struct Gadget Eingang17 = {
  &Eingang18,472,39,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang17_text,0L,NULL,Eingang17_ID,NULL };

struct IntuiText Eingang16_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"16",NULL };

#define Eingang16_ID    15

struct Gadget Eingang16 = {
  &Eingang17,104,41,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang16_text,0L,NULL,Eingang16_ID,NULL };

struct IntuiText Eingang15_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"15",NULL };

#define Eingang15_ID    14

struct Gadget Eingang15 = {
  &Eingang16,173,101,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang15_text,0L,NULL,Eingang15_ID,NULL };

struct IntuiText Eingang14_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"14",NULL };

#define Eingang14_ID    13

struct Gadget Eingang14 = {
  &Eingang15,106,143,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang14_text,0L,NULL,Eingang14_ID,NULL };

struct IntuiText Eingang13_text = {
  1,0,JAM1,11,14,NULL,(UBYTE *)"13",NULL };

#define Eingang13_ID    12

struct Gadget Eingang13 = {
  &Eingang14,97,217,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang13_text,0L,NULL,Eingang13_ID,NULL };

struct IntuiText Eingang12_text = {
  1,0,JAM1,10,14,NULL,(UBYTE *)"12",NULL };

#define Eingang12_ID    11

struct Gadget Eingang12 = {
  &Eingang13,151,260,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang12_text,0L,NULL,Eingang12_ID,NULL };

struct IntuiText Eingang11_text = {
  1,0,JAM1,12,14,NULL,(UBYTE *)"11",NULL };

#define Eingang11_ID    10

struct Gadget Eingang11 = {
  &Eingang12,271,265,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang11_text,0L,NULL,Eingang11_ID,NULL };

struct IntuiText Eingang10_text = {
  1,0,JAM1,9,14,NULL,(UBYTE *)"10",NULL };

#define Eingang10_ID    9

struct Gadget Eingang10 = {
  &Eingang11,386,252,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang10_text,0L,NULL,Eingang10_ID,NULL };

struct IntuiText Eingang9_text = {
  1,0,JAM1,14,14,NULL,(UBYTE *)"9",NULL };

#define Eingang9_ID    8

struct Gadget Eingang9 = {
  &Eingang10,438,203,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang9_text,0L,NULL,Eingang9_ID,NULL };

struct IntuiText Eingang8_text = {
  1,0,JAM1,16,14,NULL,(UBYTE *)"8",NULL };

#define Eingang8_ID    7

struct Gadget Eingang8 = {
  &Eingang9,439,134,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang8_text,0L,NULL,Eingang8_ID,NULL };

struct IntuiText Eingang7_text = {
  1,0,JAM1,15,14,NULL,(UBYTE *)"7",NULL };

#define Eingang7_ID    6

struct Gadget Eingang7 = {
  &Eingang8,403,79,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang7_text,0L,NULL,Eingang7_ID,NULL };

struct IntuiText Eingang6_text = {
  1,0,JAM1,14,14,NULL,(UBYTE *)"6",NULL };

#define Eingang6_ID    5

struct Gadget Eingang6 = {
  &Eingang7,274,78,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang6_text,0L,NULL,Eingang6_ID,NULL };

struct IntuiText Eingang5_text = {
  1,0,JAM1,15,14,NULL,(UBYTE *)"5",NULL };

#define Eingang5_ID    4

struct Gadget Eingang5 = {
  &Eingang6,189,158,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang5_text,0L,NULL,Eingang5_ID,NULL };

struct IntuiText Eingang4_text = {
  1,0,JAM1,15,14,NULL,(UBYTE *)"4",NULL };

#define Eingang4_ID    3

struct Gadget Eingang4 = {
  &Eingang5,217,206,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang4_text,0L,NULL,Eingang4_ID,NULL };

struct IntuiText Eingang3_text = {
  1,0,JAM1,15,14,NULL,(UBYTE *)"3",NULL };

#define Eingang3_ID    2

struct Gadget Eingang3 = {
  &Eingang4,314,207,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang3_text,0L,NULL,Eingang3_ID,NULL };

struct IntuiText Eingang2_text = {
  1,0,JAM1,15,14,NULL,(UBYTE *)"2",NULL };

#define Eingang2_ID    1

struct Gadget Eingang2 = {
  &Eingang3,354,160,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang2_text,0L,NULL,Eingang2_ID,NULL };

struct IntuiText Eingang1_text = {
  1,1,JAM1,16,14,NULL,(UBYTE *)"1",NULL };

#define Eingang1_ID    0

struct Gadget Eingang1 = {
  &Eingang2,276,125,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Eingang1_text,0L,NULL,Eingang1_ID,NULL };

#define FIRSTGADGETL &Eingang1


/* ----------------------------- HoehlenGadgets ----------------------------- */

SHORT SharedBordersPairs10[] = {
  0,0,0,98,1,98,1,0,147,0 };
SHORT SharedBordersPairs11[] = {
  1,98,147,98,147,1,148,0,148,98 };
SHORT SharedBordersPairs12[] = {
  0,0,0,28,1,28,1,0,147,0 };
SHORT SharedBordersPairs13[] = {
  1,28,147,28,147,1,148,0,148,28 };
SHORT SharedBordersPairs14[] = {
  0,0,0,28,1,28,1,0,77,0 };
SHORT SharedBordersPairs15[] = {
  1,28,77,28,77,1,78,0,78,28 };

struct Border SharedBorders1[] = {
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs11[0],&SharedBorders1[1]},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs10[0],NULL},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs11[0],&SharedBorders1[3]},
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs10[0],NULL},
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs13[0],&SharedBorders1[5]},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs12[0],NULL},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs13[0],&SharedBorders1[7]},
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs12[0],NULL},
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs15[0],&SharedBorders1[9]},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs14[0],NULL},
  {0,0,2,0,JAM1,5,(SHORT *)&SharedBordersPairs15[0],&SharedBorders1[11]},
  {0,0,1,0,JAM1,5,(SHORT *)&SharedBordersPairs14[0],NULL} };

struct IntuiText Rufen_text = {
  1,0,JAM1,18,18,NULL,(UBYTE *)"Rufen!",NULL };

#define Rufen_ID    4

struct Gadget Rufen = {
  NULL,349,81,79,29,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[8],(APTR)&SharedBorders1[10],
  &Rufen_text,0L,NULL,Rufen_ID,NULL };

struct IntuiText Speer_text = {
  1,0,JAM1,27,18,NULL,(UBYTE *)"Speer werfen",NULL };

#define Speer_ID    3

struct Gadget Speer = {
  &Rufen,128,81,149,29,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[4],(APTR)&SharedBorders1[6],
  &Speer_text,0L,NULL,Speer_ID,NULL };

struct IntuiText Ausgang_text[3] = {
  {1,0,JAM1,18,18,NULL,(UBYTE *)"xx",NULL},
  {1,0,JAM1,18,18,NULL,(UBYTE *)"yy",NULL},
  {1,0,JAM1,18,18,NULL,(UBYTE *)"zz",NULL}
};

#define Ausgang3_ID    2

struct Gadget Ausgang3 = {
  &Speer,430,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(Ausgang_text[2]),0L,NULL,Ausgang3_ID,NULL };

#define Ausgang2_ID    1

struct Gadget Ausgang2 = {
  &Ausgang3,270,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(Ausgang_text[1]),0L,NULL,Ausgang2_ID,NULL };

#define Ausgang1_ID    0

struct Gadget Ausgang1 = {
  &Ausgang2,111,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(Ausgang_text[0]),0L,NULL,Ausgang1_ID,NULL };


#define FIRSTGADGETH &Ausgang1

struct NewWindow new_window = {
  0,0,640,400,0,1,
  IDCMP_GADGETUP|IDCMP_CLOSEWINDOW|IDCMP_RAWKEY,
  WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_ACTIVATE|WFLG_GIMMEZEROZERO|WFLG_RMBTRAP|WFLG_SMART_REFRESH,
  NULL,NULL,
  (UBYTE *)"   »» Wumpus-Quest ««        by Henning Kiel",NULL,NULL,
  150,50,640,400,WBENCHSCREEN };

#define NEWWINDOW   &new_window

void verbindungen()
{
int i,wege[30][4]={
  {313,130,352,162},
  {376,179,353,217},
  {313,217,254,217},
  {224,204,215,176},
  {212,157,275,134},
  {291,124,295,96},
  {312,88,402,86},
  {437,98,450,133},
  {457,153,455,200},
  {450,223,412,250},
  {384,261,311,273},
  {268,274,190,269},
  {151,259,128,237},
  {122,216,129,162},
  {129,141,172,112},
  {211,105,273,83},
  {173,99,141,60},
  {141,48,469,47},
  {475,59,443,79},
  {502,58,534,249},
  {516,268,324,324},
  {516,255,477,221},
  {303,315,295,284},
  {284,324,50,281},
  {50,264,98,236},
  {33,260,108,60},
  {439,151,393,172},
  {388,250,353,226},
  {189,260,218,226},
  {145,160,187,172}
};
  SetAPen(rp,1);
  for(i=0;i<30;i++)
  {
    Move(rp,wege[i][0],wege[i][1]);
    Draw(rp,wege[i][2],wege[i][3]);
  }
}

SHORT Landkarte()
{
SHORT nummer=0;
BOOL weiter=FALSE;

StopMsg();
  AddGList(Window,FIRSTGADGETL,0,10,NULL);
  LoescheWin();
  RefreshGadgets(FIRSTGADGETL,Window,NULL);
  verbindungen();
ContMsg();
  while(!weiter)
  {
    Wait(1L<<Window->UserPort->mp_SigBit);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    ReplyMsg((struct Message *)msg);

    switch(class)
    {
      case IDCMP_RAWKEY      :
      case IDCMP_CLOSEWINDOW : ende=TRUE;
                         wende=TRUE;
                         weiter=TRUE;
                         break;
      case IDCMP_GADGETUP    : nummer=(((struct Gadget *)(msg->IAddress))->GadgetID)+1;
                         weiter=TRUE;
                         break;
      default          : break;
    }
  }
  RemoveGList(Window,FIRSTGADGETL,20);
  return(nummer);
}

void Hoehlezeichnen(nr)
SHORT nr;
{
char outtext[9];
SHORT a,b,stinken;

StopMsg();
  LoescheWin();
  OnGadget(&Speer,Window,NULL);
  for(a=0;a<3;a++)
    sprintf(Ausgang_text[a].IText,"%2d",Hoehlensystem[nr-1].Ausgang[a]);
  RefreshGadgets(FIRSTGADGETH,Window,NULL);
  sprintf(outtext,"Höhle %2d",nr);
  schreibe(100,50,outtext,1);
  draw_rect(238,239,450,257);
  draw_rect(447,255,241,241);
  stinken=NICHT;
  if(Wumpus!=nr-1)
  {
    for(a=0;a<3;a++)
    {
      for(b=0;b<3;b++)
      {
        if(Wumpus==Hoehlensystem[(Hoehlensystem[nr-1].Ausgang[a])-1].Ausgang[b])
          stinken=WENIG;
      }
      if(Wumpus==Hoehlensystem[nr-1].Ausgang[a])
        stinken=STARK;
    }
    switch(stinken)
    {
      case NICHT : schreibe(245,253,"                         ",0);
                   break;
      case WENIG : schreibe(245,253,"Es Stinkt...             ",3);
                   break;
      case STARK : schreibe(245,253,"Es stinkt fürchterlich!!!",2);
                   break;
    }
  }
ContMsg();
}

BOOL Speerwurf(nr)
SHORT nr;
{
BOOL abbruch=FALSE,ist=TRUE;

  schreibe(245,253,"In welche Höhle werfen ? ",1);
  while(!abbruch)
  {
    Wait(1L<<Window->UserPort->mp_SigBit);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_RAWKEY       :
      case IDCMP_CLOSEWINDOW  : ende=TRUE;
                          abbruch=TRUE;
                          return(FALSE);
                          break;
      case IDCMP_GADGETUP     : switch(((struct Gadget *)(msg->IAddress))->GadgetID)
                          {
                            case 0  : abbruch=TRUE;
                                      ist=(Hoehlensystem[nr-1].Ausgang[0]==Wumpus);
                                      break;
                            case 1  : abbruch=TRUE;
                                      ist=(Hoehlensystem[nr-1].Ausgang[1]==Wumpus);
                                      break;
                            case 2  : abbruch=TRUE;
                                      ist=(Hoehlensystem[nr-1].Ausgang[2]==Wumpus);
                                      break;
                            default : break;
                          }
                          break;
    }
  }
  if(!ist)
  {
    schreibe(245,253,"Leider daneben ...       ",1);
    Delay(MPAUSE);
  }
  Speere--;
  if(Speere==0)
  {
StopMsg();
    schreibe(245,253,"Das war Ihr letzter Speer",1);
    ende=TRUE;
    Delay(MPAUSE);
ContMsg();
  }
  OffGadget(&Speer,Window,NULL);
  return(ist);
}

void rufen(nr)
SHORT nr;
{
SHORT i,j;
BOOL ist=FALSE;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      if(Abgrund[i]==Hoehlensystem[nr-1].Ausgang[j])
        ist=TRUE;
  if(ist)
    schreibe(245,253,"Vorsicht Abgrund !!!     ",2);
  else
    schreibe(245,253,"Nichts zu hören !!!      ",2);

}

void Spiel()
{
SHORT HoehleNr,i,count=0;
BOOL abbruch,erlegt=FALSE,gefressen,verschleppen,fallen=FALSE,test=TRUE;

  ende=FALSE;
  Speere=3;
  Wumpus=random(20);
  for(i=0;i<3;i++)
  {
    Fledermaus[i]=random(20);
    Abgrund[i]=random(20);
  }
  HoehleNr=Landkarte();
  if(!ende)
  {
    LoescheWin();
    AddGList(Window,FIRSTGADGETH,0,5,NULL);
    RefreshGadgets(FIRSTGADGETH,Window,NULL);
    Hoehlezeichnen(HoehleNr);
    gefressen=(HoehleNr==Wumpus);

    while(!ende&&!gefressen&&!erlegt&&!fallen)
    {
      abbruch=FALSE;
      if(test)
      {
StopMsg();
        if(count++==3)
        {
          count=0;
          Wumpus=Hoehlensystem[Wumpus-1].Ausgang[random(3)-1];
        }

        while(test)
        {
          test=FALSE;
          if(!(gefressen=(HoehleNr==Wumpus)))
          {
            verschleppen=FALSE;
            fallen=FALSE;
            for(i=0;i<3;i++)
            {
              if(HoehleNr==Fledermaus[i])
                verschleppen=TRUE;
              if(HoehleNr==Abgrund[i])
                fallen=TRUE;
            }
            if(verschleppen)
            {
              test=TRUE;
              schreibe(245,253,"Sie wurden verschleppt ! ",2);
              Delay(MPAUSE);
              HoehleNr=random(20);
              Hoehlezeichnen(HoehleNr);
            }
          }
        }
ContMsg();
      }
      while(!abbruch&&!gefressen&&!fallen)
      {
        Wait(1L<<Window->UserPort->mp_SigBit);
        msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
        class=msg->Class;
        ReplyMsg((struct Message *)msg);
        switch(class)
        {
          case IDCMP_RAWKEY       :
          case IDCMP_CLOSEWINDOW  : ende=TRUE;
                              abbruch=TRUE;
                              break;
          case IDCMP_GADGETUP     : switch(((struct Gadget *)(msg->IAddress))->GadgetID)
                              {
                                case 0 : HoehleNr=Hoehlensystem[HoehleNr-1].Ausgang[0];
                                         Hoehlezeichnen(HoehleNr);
                                         test=TRUE;
                                         break;
                                case 1 : HoehleNr=Hoehlensystem[HoehleNr-1].Ausgang[1];
                                         Hoehlezeichnen(HoehleNr);
                                         test=TRUE;
                                         break;
                                case 2 : HoehleNr=Hoehlensystem[HoehleNr-1].Ausgang[2];
                                         Hoehlezeichnen(HoehleNr);
                                         test=TRUE;
                                         break;
                                case 3 : erlegt=Speerwurf(HoehleNr);
                                         test=FALSE;
                                         break;
                                case 4 : rufen(HoehleNr);
                                         test=FALSE;
                                         break;
                              }
                              abbruch=TRUE;
                              break;
        }
      }
    }
StopMsg();
    if(gefressen)
    {
      schreibe(245,253,"Sie sind gefressen worden",2);
    }
    if(fallen)
    {
      schreibe(245,253,"Sie fallen in eine Grube!",2);
    }
    if(erlegt)
    {
      schreibe(245,253,"Sie haben Wumpus erlegt !",1);
    }
    Delay(MPAUSE);
    RemoveGList(Window,FIRSTGADGETH,5);
ContMsg();
  }
}

int main()
{
  srand((unsigned)time(NULL));
  open_lib();
  open_window(new_window);

  while(!wende)
    Spiel();

  close_window();
  close_lib();
  return(0);
}
