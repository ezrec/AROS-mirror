/*
    Copyright © 1995-98, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Quad Game
    Lang: english
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
        16-Sep-1997     hkiel     Fixed all casts

******************************************************************************/

static const char version[] __attribute__((used)) = "$VER: Quad 0.2 (16.09.1997)\n";

#include "../prec.c"
#include "Quad.h"
#include "QuadIncl.h"

int quit_game=0,go_on,field[2][9];
int nummer;

/* Check if at least one button isn't pressed on left=initial side
   and at least on is pressed on right=target side                 */
int at_least_one()
{
int i;
int z=0;
  for(i=0;i<9;i++)
    z+=field[0][i];
  if(z!=9)
  {
    z=0;
    for(i=0;i<9;i++)
      z+=field[1][i];
    if(z>0)
      return(1);
    else
      return(0);
  }
  else
    return(0);
}

/* Check if final position is reached */
int test()
{
int i;
int z=0;
  for(i=0;i<9;i++)
    if(field[0][i]==field[1][i])z++;
  if(z==9)
    return(1);
  else
    return(0);
}

/* Refresh the buttons, needed to have them selected */
void updatebuttons()
{
 initial1.Flags=(GFLG_GADGHIMAGE+field[0][0]*GFLG_SELECTED);
 initial2.Flags=(GFLG_GADGHIMAGE+field[0][1]*GFLG_SELECTED);
 initial3.Flags=(GFLG_GADGHIMAGE+field[0][2]*GFLG_SELECTED);
 initial4.Flags=(GFLG_GADGHIMAGE+field[0][3]*GFLG_SELECTED);
 initial5.Flags=(GFLG_GADGHIMAGE+field[0][4]*GFLG_SELECTED);
 initial6.Flags=(GFLG_GADGHIMAGE+field[0][5]*GFLG_SELECTED);
 initial7.Flags=(GFLG_GADGHIMAGE+field[0][6]*GFLG_SELECTED);
 initial8.Flags=(GFLG_GADGHIMAGE+field[0][7]*GFLG_SELECTED);
 initial9.Flags=(GFLG_GADGHIMAGE+field[0][8]*GFLG_SELECTED);
 target1.Flags=(GFLG_GADGHIMAGE+field[1][0]*GFLG_SELECTED);
 target2.Flags=(GFLG_GADGHIMAGE+field[1][1]*GFLG_SELECTED);
 target3.Flags=(GFLG_GADGHIMAGE+field[1][2]*GFLG_SELECTED);
 target4.Flags=(GFLG_GADGHIMAGE+field[1][3]*GFLG_SELECTED);
 target5.Flags=(GFLG_GADGHIMAGE+field[1][4]*GFLG_SELECTED);
 target6.Flags=(GFLG_GADGHIMAGE+field[1][5]*GFLG_SELECTED);
 target7.Flags=(GFLG_GADGHIMAGE+field[1][6]*GFLG_SELECTED);
 target8.Flags=(GFLG_GADGHIMAGE+field[1][7]*GFLG_SELECTED);
 target9.Flags=(GFLG_GADGHIMAGE+field[1][8]*GFLG_SELECTED);
 RefreshGList(FIRSTGADGET,Window,NULL,20);
}

int main()
{
int i;
  open_lib();
  open_window();
/* initialize the playfield */
  for(i=0;i<9;i++)
  {
    field[0][i]=0;
    field[1][i]=0;
  }
/* Start of the game */
  while(quit_game==0)
  {
    go_on=0;
/* wait for message and update buttons until quit or start game */
    while(go_on==0&&quit_game==0)
    {
      WaitPort(Window->UserPort);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      switch(class)
      {
/* message to quit game */
        case IDCMP_CLOSEWINDOW : quit_game=1;
                    		 break;
/* handle gadgets */
        case IDCMP_GADGETUP    : nummer=(((struct Gadget *)(msg->IAddress))->GadgetID);
/* left=play buttons */
                                 if(nummer>0&&nummer<10)
                                   field[0][nummer-1]=1-(field[0][nummer-1]);
/* right=destination buttons */
                                 if(nummer>10)
                                   field[1][nummer-11]=1-(field[1][nummer-11]);
/* start button */
                                 if(nummer==0)
                                   go_on=at_least_one();
                                 break;
        default          : break;
      }
      ReplyMsg((struct Message *)msg);
      updatebuttons();
    }
    OffGadget(&Start,Window,NULL);
    OnGadget(&Stop,Window,NULL);
    go_on=0;
/* play until match or stop */
    while(go_on==0&&quit_game==0&&(test()==0))
    {
      WaitPort(Window->UserPort);
      msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
      class=msg->Class;
      switch(class)
      {
        case IDCMP_CLOSEWINDOW : quit_game=1;
                                 break;
        case IDCMP_GADGETUP    : nummer=(((struct Gadget *)(msg->IAddress))->GadgetID);
/* invert buttons */
                                 switch(nummer)
                                 {
                                   case 1 : if(field[0][0]==0)
                                            {
                                              field[0][0]=1-(field[0][0]);
                                              field[0][1]=1-(field[0][1]);
                                              field[0][3]=1-(field[0][3]);
                                              field[0][4]=1-(field[0][4]);
                                            }
                                            break;
                                   case 2 : if(field[0][1]==0)
                                            {
                                              field[0][1]=1-(field[0][1]);
                                              field[0][0]=1-(field[0][0]);
                                              field[0][2]=1-(field[0][2]);
                                            }
                                            break;
                                   case 3 : if(field[0][2]==0)
                                            {
                                              field[0][2]=1-(field[0][2]);
                                              field[0][1]=1-(field[0][1]);
                                              field[0][5]=1-(field[0][5]);
                                              field[0][4]=1-(field[0][4]);
                                            }
                                            break;
                                   case 4 : if(field[0][3]==0)
                                            {
                                              field[0][3]=1-(field[0][3]);
                                              field[0][0]=1-(field[0][0]);
                                              field[0][6]=1-(field[0][6]);
                                            }
                                            break;
                                   case 5 : if(field[0][4]==0)
                                            {
                                              field[0][4]=1-(field[0][4]);
                                              field[0][1]=1-(field[0][1]);
                                              field[0][3]=1-(field[0][3]);
                                              field[0][5]=!(field[0][5]);
                                              field[0][7]=!(field[0][7]);
                                            }
                                            break;
                                   case 6 : if(field[0][5]==0)
                                            {
                                              field[0][5]=1-(field[0][5]);
                                              field[0][8]=1-(field[0][8]);
                                              field[0][2]=1-(field[0][2]);
                                            }
                                            break;
                                   case 7 : if(field[0][6]==0)
                                            {
                                              field[0][6]=1-(field[0][6]);
                                              field[0][7]=1-(field[0][7]);
                                              field[0][3]=1-(field[0][3]);
                                              field[0][4]=1-(field[0][4]);
                                            }
                                            break;
                                   case 8 : if(field[0][7]==0)
                                            {
                                              field[0][7]=1-(field[0][7]);
                                              field[0][6]=1-(field[0][6]);
                                              field[0][8]=1-(field[0][8]);
                                            }
                                            break;
                                   case 9 : if(field[0][8]==0)
                                            {
                                              field[0][8]=1-(field[0][8]);
                                              field[0][7]=1-(field[0][7]);
                                              field[0][5]=1-(field[0][5]);
                                              field[0][4]=1-(field[0][4]);
                                            }
                                            break;
/* stop game */
                                   case 10: go_on=1;
                                            break;
                                   default: break;
                                 }
                                 break;
        default                : break;
      }
      ReplyMsg((struct Message *)msg);
      updatebuttons();
    }
    OffGadget(&Stop,Window,NULL);
    OnGadget(&Start,Window,NULL);
  }
/* game terminated - clean up */
  close_window();
  close_lib();
  return(0);
}
