/*
    (C) 1995-98 AROS - The Amiga Research OS
    $Id$

    Desc: Wumpus Game
    Lang: english
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
        16-Sep-1997     hkiel     Fixed all casts

******************************************************************************/

static const char version[] = "$VER: Wumpus 0.2 (16.09.1997)\n";

#include "WumpusIncl.h"

#define NONE 0
#define LITTLE 1
#define MUCH 2

#define MPAUSE 50

struct cavity
{
  BYTE tunnel[3];
};

struct cavity cave[20]=
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
BYTE wumpus,spears,batman[3],abyss[3];
BOOL end_hunt,end_game=FALSE;


/* ---  gadget of map of cave-net  --- */

WORD SharedBordersPairs0[] = {
  0,0,0,18,1,18,1,0,37,0 };
WORD SharedBordersPairs1[] = {
  1,18,37,18,37,1,38,0,38,18 };

struct Border SharedBorders[] = {
  {0,0,1,1,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[1]},
  {0,0,2,2,JAM1,5,&SharedBordersPairs0[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[3]},
  {0,0,1,1,JAM1,5,&SharedBordersPairs0[0],NULL} };

struct IntuiText cave20_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"20",NULL };

#define cave20_ID    19

struct Gadget cave20 = {
  NULL,12,262,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave20_text,0L,NULL,cave20_ID,NULL };

struct IntuiText cave19_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"19",NULL };

#define cave19_ID    18

struct Gadget cave19 = {
  &cave20,285,316,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave19_text,0L,NULL,cave19_ID,NULL };

struct IntuiText cave18_text = {
  1,0,JAM1,9,4,NULL,(UBYTE *)"18",NULL };

#define cave18_ID    17

struct Gadget cave18 = {
  &cave19,517,249,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave18_text,0L,NULL,cave18_ID,NULL };

struct IntuiText cave17_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"17",NULL };

#define cave17_ID    16

struct Gadget cave17 = {
  &cave18,472,39,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave17_text,0L,NULL,cave17_ID,NULL };

struct IntuiText cave16_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"16",NULL };

#define cave16_ID    15

struct Gadget cave16 = {
  &cave17,104,41,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave16_text,0L,NULL,cave16_ID,NULL };

struct IntuiText cave15_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"15",NULL };

#define cave15_ID    14

struct Gadget cave15 = {
  &cave16,173,101,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave15_text,0L,NULL,cave15_ID,NULL };

struct IntuiText cave14_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"14",NULL };

#define cave14_ID    13

struct Gadget cave14 = {
  &cave15,106,143,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave14_text,0L,NULL,cave14_ID,NULL };

struct IntuiText cave13_text = {
  1,0,JAM1,11,4,NULL,(UBYTE *)"13",NULL };

#define cave13_ID    12

struct Gadget cave13 = {
  &cave14,97,217,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave13_text,0L,NULL,cave13_ID,NULL };

struct IntuiText cave12_text = {
  1,0,JAM1,10,4,NULL,(UBYTE *)"12",NULL };

#define cave12_ID    11

struct Gadget cave12 = {
  &cave13,151,260,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave12_text,0L,NULL,cave12_ID,NULL };

struct IntuiText cave11_text = {
  1,0,JAM1,12,4,NULL,(UBYTE *)"11",NULL };

#define cave11_ID    10

struct Gadget cave11 = {
  &cave12,271,265,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave11_text,0L,NULL,cave11_ID,NULL };

struct IntuiText cave10_text = {
  1,0,JAM1,9,4,NULL,(UBYTE *)"10",NULL };

#define cave10_ID    9

struct Gadget cave10 = {
  &cave11,386,252,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave10_text,0L,NULL,cave10_ID,NULL };

struct IntuiText cave9_text = {
  1,0,JAM1,14,4,NULL,(UBYTE *)"9",NULL };

#define cave9_ID    8

struct Gadget cave9 = {
  &cave10,438,203,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave9_text,0L,NULL,cave9_ID,NULL };

struct IntuiText cave8_text = {
  1,0,JAM1,16,4,NULL,(UBYTE *)"8",NULL };

#define cave8_ID    7

struct Gadget cave8 = {
  &cave9,439,134,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave8_text,0L,NULL,cave8_ID,NULL };

struct IntuiText cave7_text = {
  1,0,JAM1,15,4,NULL,(UBYTE *)"7",NULL };

#define cave7_ID    6

struct Gadget cave7 = {
  &cave8,403,79,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave7_text,0L,NULL,cave7_ID,NULL };

struct IntuiText cave6_text = {
  1,0,JAM1,14,4,NULL,(UBYTE *)"6",NULL };

#define cave6_ID    5

struct Gadget cave6 = {
  &cave7,274,78,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave6_text,0L,NULL,cave6_ID,NULL };

struct IntuiText cave5_text = {
  1,0,JAM1,15,4,NULL,(UBYTE *)"5",NULL };

#define cave5_ID    4

struct Gadget cave5 = {
  &cave6,189,158,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave5_text,0L,NULL,cave5_ID,NULL };

struct IntuiText cave4_text = {
  1,0,JAM1,15,4,NULL,(UBYTE *)"4",NULL };

#define cave4_ID    3

struct Gadget cave4 = {
  &cave5,217,206,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave4_text,0L,NULL,cave4_ID,NULL };

struct IntuiText cave3_text = {
  1,0,JAM1,15,4,NULL,(UBYTE *)"3",NULL };

#define cave3_ID    2

struct Gadget cave3 = {
  &cave4,314,207,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave3_text,0L,NULL,cave3_ID,NULL };

struct IntuiText cave2_text = {
  1,0,JAM1,15,4,NULL,(UBYTE *)"2",NULL };

#define cave2_ID    1

struct Gadget cave2 = {
  &cave3,354,160,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave2_text,0L,NULL,cave2_ID,NULL };

struct IntuiText cave1_text = {
  1,1,JAM1,16,4,NULL,(UBYTE *)"1",NULL };

#define cave1_ID    0

struct Gadget cave1 = {
  &cave2,276,125,39,19,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &cave1_text,0L,NULL,cave1_ID,NULL };

#define FIRSTGADGETL &cave1


/* ---  gadgets of caves  --- */

WORD SharedBordersPairs10[] = {
  0,0,0,98,1,98,1,0,147,0 };
WORD SharedBordersPairs11[] = {
  1,98,147,98,147,1,148,0,148,98 };
WORD SharedBordersPairs12[] = {
  0,0,0,28,1,28,1,0,147,0 };
WORD SharedBordersPairs13[] = {
  1,28,147,28,147,1,148,0,148,28 };
WORD SharedBordersPairs14[] = {
  0,0,0,28,1,28,1,0,77,0 };
WORD SharedBordersPairs15[] = {
  1,28,77,28,77,1,78,0,78,28 };

struct Border SharedBorders1[] = {
  {0,0,1,0,JAM1,5,&SharedBordersPairs11[0],&SharedBorders1[1]},
  {0,0,2,0,JAM1,5,&SharedBordersPairs10[0],NULL},
  {0,0,2,0,JAM1,5,&SharedBordersPairs11[0],&SharedBorders1[3]},
  {0,0,1,0,JAM1,5,&SharedBordersPairs10[0],NULL},
  {0,0,1,0,JAM1,5,&SharedBordersPairs13[0],&SharedBorders1[5]},
  {0,0,2,0,JAM1,5,&SharedBordersPairs12[0],NULL},
  {0,0,2,0,JAM1,5,&SharedBordersPairs13[0],&SharedBorders1[7]},
  {0,0,1,0,JAM1,5,&SharedBordersPairs12[0],NULL},
  {0,0,1,0,JAM1,5,&SharedBordersPairs15[0],&SharedBorders1[9]},
  {0,0,2,0,JAM1,5,&SharedBordersPairs14[0],NULL},
  {0,0,2,0,JAM1,5,&SharedBordersPairs15[0],&SharedBorders1[11]},
  {0,0,1,0,JAM1,5,&SharedBordersPairs14[0],NULL} };

struct IntuiText shout_text = {
  1,0,JAM1,18,8,NULL,NULL,NULL };

#define shout_ID    4

struct Gadget shout_gad = {
  NULL,349,81,79,29,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[8],(APTR)&SharedBorders1[10],
  &shout_text,0L,NULL,shout_ID,NULL };

struct IntuiText spear_text = {
  1,0,JAM1,27,8,NULL,NULL,NULL };

#define spear_ID    3

struct Gadget spear_gad = {
  &shout_gad,128,81,149,29,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[4],(APTR)&SharedBorders1[6],
  &spear_text,0L,NULL,spear_ID,NULL };

struct IntuiText tunnel_text[3] = {
  {1,0,JAM1,18,18,NULL,(UBYTE *)"xx",NULL},
  {1,0,JAM1,18,18,NULL,(UBYTE *)"yy",NULL},
  {1,0,JAM1,18,18,NULL,(UBYTE *)"zz",NULL}
};

#define tunnel3_ID    2

struct Gadget tunnel3 = {
  &spear_gad,430,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(tunnel_text[2]),0L,NULL,tunnel3_ID,NULL };

#define tunnel2_ID    1

struct Gadget tunnel2 = {
  &tunnel3,270,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(tunnel_text[1]),0L,NULL,tunnel2_ID,NULL };

#define tunnel1_ID    0

struct Gadget tunnel1 = {
  &tunnel2,111,127,149,99,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders1[0],(APTR)&SharedBorders1[2],
  &(tunnel_text[0]),0L,NULL,tunnel1_ID,NULL };


#define FIRSTGADGETH &tunnel1

struct NewWindow new_window = {
  0,0,640,400,0,1,
  IDCMP_GADGETUP | IDCMP_CLOSEWINDOW,
  WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE
	| WFLG_RMBTRAP | WFLG_SMART_REFRESH | WFLG_GIMMEZEROZERO,
  NULL,NULL,
  (UBYTE *)"   »» Wumpus-Quest ««        by Henning Kiel",NULL,NULL,
  150,50,640,400,WBENCHSCREEN };

#define NEWWINDOW   &new_window

void draw_tunnels()
{
int i;
LONG tunnels[30][4]={
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
    Move(rp,tunnels[i][0],tunnels[i][1]);
    Draw(rp,tunnels[i][2],tunnels[i][3]);
  }
}

BYTE map_of_caves()
{
BYTE number=0;
BOOL finish=FALSE;
struct Gadget *gad;

StopMsg();
  AddGList(Window,FIRSTGADGETL,0,20,NULL);
  clear_win();
  RefreshGadgets(FIRSTGADGETL,Window,NULL);
  draw_tunnels();
ContMsg();
  while(!finish)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    gad = (struct Gadget *)(msg->IAddress);
    ReplyMsg((struct Message *)msg);

    switch(class)
    {
      case IDCMP_CLOSEWINDOW : end_hunt=TRUE;
                         end_game=TRUE;
                         finish=TRUE;
                         break;
      case IDCMP_GADGETUP    : number=(gad->GadgetID)+1;
                         finish=TRUE;
                         break;
      default          : break;
    }
  }
  RemoveGList(Window,FIRSTGADGETL,20);
  return(number);
}

void draw_cave(nr)
BYTE nr;
{
char outtext[9];
BYTE a,b,stinks;

StopMsg();
  clear_win();
  OnGadget(&spear_gad,Window,NULL);
  for(a=0;a<3;a++)
    sprintf(tunnel_text[a].IText,"%2d",cave[nr-1].tunnel[a]);
  RefreshGadgets(FIRSTGADGETH,Window,NULL);
  sprintf(outtext,_(MSG_CAVE),nr);
  write_text(100,50,outtext,1);
  draw_rect(238,239,450,259);
  draw_rect(447,257,241,241);
  stinks=NONE;
  if(wumpus!=nr-1)
  {
    for(a=0;a<3;a++)
    {
      for(b=0;b<3;b++)
      {
        if(wumpus==cave[(cave[nr-1].tunnel[a])-1].tunnel[b])
          stinks=LITTLE;
      }
      if(wumpus==cave[nr-1].tunnel[a])
        stinks=MUCH;
    }
    switch(stinks)
    {
      case NONE   : write_text(245,252,_(MSG_NOWT),0);
                    break;
      case LITTLE : write_text(245,252,_(MSG_STINKING),3);
                    break;
      case MUCH   : write_text(245,252,_(MSG_STINKINGTERRIBLE),2);
                    break;
    }
  }
ContMsg();
}

BOOL throw(nr)
BYTE nr;
{
BOOL cancel=FALSE,ist=TRUE;
struct Gadget *gad;

  write_text(245,252,_(MSG_THROUGHINTO),1);
  while(!cancel)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    gad = (struct Gadget *)(msg->IAddress);
    ReplyMsg((struct Message *)msg);
    switch(class)
    {
      case IDCMP_CLOSEWINDOW  : end_hunt=TRUE;
                    		cancel=TRUE;
                    		return(FALSE);
            			break;
      case IDCMP_GADGETUP     : switch(gad->GadgetID)
        	                {
                        	  case 0  : cancel=TRUE;
                                	    ist=(cave[nr-1].tunnel[0]==wumpus);
                                	    break;
                        	  case 1  : cancel=TRUE;
                                	    ist=(cave[nr-1].tunnel[1]==wumpus);
                                	    break;
                        	  case 2  : cancel=TRUE;
                                	    ist=(cave[nr-1].tunnel[2]==wumpus);
                                	    break;
                        	  default : break;
                    		}
                    		break;
    }
  }
  if(!ist)
  {
    write_text(245,252,_(MSG_MISSED),1);
    Delay(MPAUSE);
  }
  spears--;
  if(spears==0)
  {
StopMsg();
    write_text(245,252,_(MSG_LAST),1);
    end_hunt=TRUE;
    Delay(MPAUSE);
ContMsg();
  }
  OffGadget(&spear_gad,Window,NULL);
  return(ist);
}

void shout(nr)
BYTE nr;
{
BYTE i,j;
BOOL is_abyss=FALSE;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      if(abyss[i]==cave[nr-1].tunnel[j])
        is_abyss=TRUE;
  if(is_abyss)
    write_text(245,252,_(MSG_ABYSS),2);
  else
    write_text(245,252,_(MSG_SILENT),2);

}

void game()
{
BYTE cave_number,i,count=0;
BOOL cancel,killed=FALSE,eaten_up,displace,fallen=FALSE,test=TRUE;
struct Gadget *gad;

  end_hunt=FALSE;
  spears=3;
  wumpus=random(20);
  for(i=0;i<3;i++)
  {
    batman[i]=random(20);
    abyss[i]=random(20);
  }
  cave_number=map_of_caves();
  if(!end_hunt)
  {
    clear_win();
    AddGList(Window,FIRSTGADGETH,0,5,NULL);
    RefreshGadgets(FIRSTGADGETH,Window,NULL);
    draw_cave(cave_number);
    eaten_up=(cave_number==wumpus);

    while(!end_hunt&&!eaten_up&&!killed&&!fallen)
    {
      cancel=FALSE;
      if(test)
      {
StopMsg();
        if(count++==3)
        {
          count=0;
          wumpus=cave[wumpus-1].tunnel[random(3)-1];
        }

        while(test)
        {
          test=FALSE;
          if(!(eaten_up=(cave_number==wumpus)))
          {
            displace=FALSE;
            fallen=FALSE;
            for(i=0;i<3;i++)
            {
              if(cave_number==batman[i])
                displace=TRUE;
              if(cave_number==abyss[i])
                fallen=TRUE;
            }
            if(displace)
            {
              test=TRUE;
              write_text(245,252,_(MSG_DISPLACED),2);
              Delay(MPAUSE);
              cave_number=random(20);
              draw_cave(cave_number);
            }
          }
        }
ContMsg();
      }
      while(!cancel&&!eaten_up&&!fallen)
      {
        WaitPort(Window->UserPort);
        msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
        class=msg->Class;
        gad = (struct Gadget *)(msg->IAddress);
        ReplyMsg((struct Message *)msg);
        switch(class)
        {
          case IDCMP_CLOSEWINDOW  : end_hunt=TRUE;
                              cancel=TRUE;
                              break;
          case IDCMP_GADGETUP     : switch(gad->GadgetID)
                              {
                                case 0 : cave_number=cave[cave_number-1].tunnel[0];
                                         draw_cave(cave_number);
                                         test=TRUE;
                                         break;
                                case 1 : cave_number=cave[cave_number-1].tunnel[1];
                                         draw_cave(cave_number);
                                         test=TRUE;
                                         break;
                                case 2 : cave_number=cave[cave_number-1].tunnel[2];
                                         draw_cave(cave_number);
                                         test=TRUE;
                                         break;
                                case 3 : killed=throw(cave_number);
                                         test=FALSE;
                                         break;
                                case 4 : shout(cave_number);
                                         test=FALSE;
                                         break;
                              }
                              cancel=TRUE;
                              break;
        }
      }
    }
StopMsg();
    if(eaten_up)
    {
      write_text(245,252,_(MSG_EATEN),2);
    }
    if(fallen)
    {
      write_text(245,252,_(MSG_FALLEN),2);
    }
    if(killed)
    {
      write_text(245,252,_(MSG_SHOT),1);
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
  Locale_Initialize();
  shout_text.IText = (STRPTR)_(MSG_SHOUT);
  spear_text.IText = (STRPTR)_(MSG_THROW);
  open_window(&new_window);

  while(!end_game)
    game();

  close_window();
  Locale_Deinitialize();
  close_lib();
  return(0);
}
