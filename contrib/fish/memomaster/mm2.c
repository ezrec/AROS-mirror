#include <aros/oldprograms.h>
#include <proto/exec.h>
/*  MM2.C
 *	Looks for files on assigned device MEMOS:
 *	    Files are MEMODATA.DAT and MMASTER.CONFIG
 */

#include <stdio.h>
#include <stdlib.h>
#include <intuition/intuition.h>
#include <dos/dos.h>
#include <time.h>
#include <ctype.h>
#include <exec/memory.h>
#include <string.h>
#include <exec/nodes.h>
#include <exec/lists.h>

#include "mm2.h"
#include "main_win.c"

#include <aros/debug.h>

struct IntuitionBase *IntuitionBase;
struct Window *mm_w;
struct Remember *RK;

void Action(),MemoBrs(),Display_Heads();
void Colours(char c0, char c1, char c2, char c3);
void Display_One(),Display_Blank(),Display_Block(),Mode_Help();
void Toggle(),DisplayT(),Today();
int Shrink(),SaveData(),date2days(),Month2Num(),CleanUp(),SetColours();
BOOL MemoChk(BOOL loud);
struct Window *MainWindow();
extern struct MinNode *MoveBackOne(struct MinNode *);
extern struct MinNode *MoveBackBlock(struct MinNode *);
extern struct MinNode *MoveFwdOne(struct MinNode *);
extern struct MinNode *MoveFwdBlock(struct MinNode *);
extern struct MinNode *MoveEnd(struct MinNode *);
extern	BOOL Edit(void);
extern	BOOL Add(void);
extern	BOOL Del(void);
extern	BOOL MemoReq(struct Memo_Item *);
extern	BOOL MemIns(struct Memo_Item *);
extern	BOOL MemoDel(struct MinNode *);
extern BOOL DataAmended;

int CPLine;
int Colour[4];
struct MinList *MemListPtr,*LoadData();
struct MinNode *DisplayFirst;

struct MM_Config mm_prefs;
BOOL User_Config;
BOOL Check;
BOOL WBStart;

char date_today[10];
char MWTitle[60];

#warning Deactivated the following lines
#if 0
/*========================================================================
 *###======   wbmain() function   ========================================
 * DICE specific function. Program starts here if executed from WorkBench
 *========================================================================*/
int wbmain(struct WBStartup *wbs)
  {
  WBStart = TRUE;
  return main();
  }
#endif

/*==========================================================================*/
/*###======   main() function   ============================================*/
/*==========================================================================*/
main(int argc, char *argv[])
  {
  BOOL MFound;
  FILE *fp;

  if ((IntuitionBase=(struct IntuitionBase *)
	    OpenLibrary("intuition.library",0))==NULL)
    {
    printf("Failed to open intuition library\n");
    CleanUp();
    exit(0);
    }

/* First, try to get any config values from file MEMOS:MMASTER.CONFIG
 */

  fp=fopen("memos:mmaster.config","r");
  if (fp)
    {
    fread((char *)&mm_prefs,4,1,fp);
    fclose(fp);
    User_Config=TRUE;
    }
  else
    User_Config=FALSE;

  MemListPtr = LoadData();
  if (!MemListPtr)
    {
    printf("Failed to create list of memos\n");
    CleanUp();
    exit(0);
    }
  SetColours();
  Today(date_today);      /* Set up todays date string */
  sprintf(MWTitle, "MemoMaster V2  -  %s", date_today);

/* If started from WorkBench or if request on command line check for memos
 *  for today.
 */
  MFound=TRUE;
  if (argc > 1) if (strcmp(argv[1],"check")==0) Check = TRUE;
  if (((argc > 1) && (Check)) || (WBStart))
    MFound=MemoChk(FALSE);

  if (!Check)
    {
    mm_w=MainWindow();
    if(!mm_w)
      {
      printf("Failed to open MemoMaster window\n");
      CleanUp();
      exit(0);
      }
    Action();
    }
  CleanUp();
  }
/*=====================================================================*/
CleanUp()
  {
  if(mm_w)CloseWindow(mm_w);
  FreeRemember(&RK,TRUE);
  if(IntuitionBase)CloseLibrary((struct Library *)IntuitionBase);
  return 1;
  }
/*=====================================================================*/
void Action()
  {
  int Class,ID,x;
  struct IntuiMessage *msg;
  APTR address;
  BOOL finished,result;
  ULONG SignalMask;
  struct MinNode *NewFirst;

  Display_Heads();
  if ( (MemListPtr == NULL) || (MemListPtr->mlh_Head == NULL ) )
    DisplayFirst=NULL;
  else
    DisplayFirst=MemListPtr->mlh_Head;

  Display_Block(DisplayFirst);
  SignalMask = 1L << mm_w->UserPort->mp_SigBit;
  finished=FALSE;
  while(!finished)
    {
    Wait( SignalMask );
    msg = (struct IntuiMessage *) GetMsg( mm_w->UserPort );
    if(msg)
      {
      Class = msg->Class;
      address = msg->IAddress;
      ReplyMsg( (struct Message *)msg );
      switch( Class )
	{
	case CLOSEWINDOW:
	  finished=TRUE;
	  break;
	case GADGETUP:
	  ID=((struct Gadget *)address)->GadgetID;
kprintf("GADGETUP: ID = %d\n", ID);
	  switch (ID)
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
	      Toggle(ID);
	      break;
	    case 6:
	      result=Add();
	      break;
	    case 7:
	      result=Del();
	      break;
	    case 8:
	      result=Edit();
	      break;
	    case 9:
	      result = MemoChk(TRUE);
	      break;
	    case 10:
	      CloseWindow(mm_w);
	      Shrink();
	      mm_w=MainWindow();
	      if (!mm_w)
		{
		printf("Failed reopening main window\n");
		CleanUp();
		exit(0);
		}
	      Display_Heads();
	      Display_Block(DisplayFirst);
	      break;
	    case 11:
/*		DisplayH(Help_Intro);*/
	      Mode_Help();
	      break;
	    case 12:
	      if (LISTEMPTY) {;}
	      else
		{
		DisplayFirst=MemListPtr->mlh_Head;
		Display_Block(DisplayFirst);
		}
	      break;
	    case 13:
	      if (LISTEMPTY) {;}
	      else
		{
		NewFirst=MoveBackBlock(DisplayFirst);
		if (NewFirst != DisplayFirst)
			{
			DisplayFirst=NewFirst;
			Display_Block(DisplayFirst);
			}
		}
	      break;
	    case 14:
	      if (LISTEMPTY) {;}
	      else
		{
		NewFirst=MoveBackOne(DisplayFirst);
		if (NewFirst != DisplayFirst)
			{
			DisplayFirst=NewFirst;
			Display_Block(DisplayFirst);
			}
		}
	  break;
	    case 15:
	      if (LISTEMPTY) {;}
	      else
		{
		NewFirst=MoveFwdOne(DisplayFirst);
		if (NewFirst != DisplayFirst)
			{
			DisplayFirst=NewFirst;
			Display_Block(DisplayFirst);
			}
		}
	  break;
	    case 16:
	      if (LISTEMPTY) {;}
	      else
		{
		NewFirst=MoveFwdBlock(DisplayFirst);
		if (NewFirst != DisplayFirst)
			{
			DisplayFirst=NewFirst;
			Display_Block(DisplayFirst);
			}
		}
	  break;
	    case 17:
	      if (LISTEMPTY) {;}
	      else
		{
		NewFirst=MoveEnd(DisplayFirst);
		if (NewFirst != DisplayFirst)
			{
			DisplayFirst=NewFirst;
			Display_Block(DisplayFirst);
			}
		}
	  break;
	    }
	  break;
	}
      }
    }
  if (DataAmended) x=SaveData();
  }

/*========================================================================
 * BOOL MemoChk(BOOL loud)
 *   Checks through list of memos and displays those specified by date and
 *   notice values compared to todays date (supplied by caller).
 * Rather than display memos individually, which could be annoying if there
 *   several, display in blocks of 5
 * Optionally, if no memos displayed, display a message confirming that none
 *   found (parameter 'loud' is non-zero for confirmation, otherwise zero)
 * Return TRUE if one or more memos displayed, otherwise return FALSE
 */
BOOL MemoChk(BOOL loud)
  {
  char *cp_array[8]; /* One heading, up to 5 memos plus spacing */
  int cp_array_sub;
  char mtype;
  char dbuf[3];
  char ybuf[3];
  int today_days, memo_days, notice_days;
  struct MinNode *n;
  BOOL MFound = FALSE;
  char *empty = "";
  char *heading = "   Memos for today   ";
  char memobuf[5][80];

  if (LISTEMPTY)
    {
    if (loud)
      {
      cp_array[0] = "No memo for today";
      cp_array[1] = '\0';
      DisplayT(cp_array);
      }
    return FALSE;
    }

/*
 * Work out days from 1 Jan 1970 until today for comparison
 */
  dbuf[0] = *(date_today);
  dbuf[1] = *(date_today+1);
  dbuf[2] = '\0';
  ybuf[0] = *(date_today+7);
  ybuf[1] = *(date_today+8);
  ybuf[2] = '\0';
  today_days = date2days(atoi(dbuf),
			Month2Num((char *)date_today+3),
			atoi(ybuf));

  n=MemListPtr->mlh_Head;
  cp_array_sub = 2;
  while (n->mln_Succ)
    {
    dbuf[0] = ((struct MI_Mem *)n)->mim_MI.mi_Date[0];
    dbuf[1] = ((struct MI_Mem *)n)->mim_MI.mi_Date[1];
    dbuf[2] = '\0';
    mtype=((struct MI_Mem *)n)->mim_MI.mi_Type[0];
    if (( mtype == 'a') || (mtype == 'A'))
      {
      ybuf[0] = *(date_today+7);
      ybuf[1] = *(date_today+8);
      }
    else
      {
      ybuf[0] = ((struct MI_Mem *)n)->mim_MI.mi_Date[7];
      ybuf[1] = ((struct MI_Mem *)n)->mim_MI.mi_Date[8];
      }
    ybuf[2] = '\0';

    memo_days = date2days(atoi(dbuf),
		  Month2Num((char *)&(((struct MI_Mem *)n)->mim_MI.mi_Date[3])),
		  atoi(ybuf));
    if ( (memo_days < today_days) && ( (mtype == 'a') || (mtype == 'A') ))
      {
      sprintf(ybuf, "%d", atoi(ybuf)+1); /* Doesn't cater for going beyond 1999!!*/
      memo_days = date2days(atoi(dbuf),
		  Month2Num((char *)&(((struct MI_Mem *)n)->mim_MI.mi_Date[3])),
		  atoi(ybuf));
      }
    notice_days = memo_days -
		    atoi((char *)&(((struct MI_Mem *)n)->mim_MI.mi_Notice[0]));

    if ((today_days < notice_days) || (today_days > memo_days))
      {
      ; /* Do not display */
      }
    else
      {
      /* Add memo to list for display*/
      sprintf(memobuf[cp_array_sub-2], "  %s : %s  ",
		(char *)&(((struct MI_Mem *)n)->mim_MI.mi_Date[0]),
		(char *)&(((struct MI_Mem *)n)->mim_MI.mi_Text[0])   );
      cp_array[cp_array_sub] = (memobuf[cp_array_sub-2]);
      cp_array_sub++;
      if (cp_array_sub > 6)
	{
	cp_array[0] = heading;
	cp_array[1] = empty;
	cp_array[cp_array_sub] = empty;
	cp_array[cp_array_sub+1] = '\0';
	DisplayT(cp_array);
	cp_array_sub = 2;
	}
      MFound = TRUE;
      }
    n = n->mln_Succ;
    }
/* End of loop through memos */

  if (MFound)
    {
    /* Display remaining memos on display list */
    if (cp_array_sub > 2)
      {
      cp_array[0] = heading;
      cp_array[1] = empty;
      cp_array[cp_array_sub] = empty;
      cp_array[cp_array_sub+1] = '\0';
      DisplayT(cp_array);
      }
    }
  else
    {
    if (loud)
      {
      cp_array[0] = "No memo for today";
      cp_array[1] = '\0';
      DisplayT(cp_array);
      }
    }
  return MFound;

  }

/*------------------------------------------------------------------------*/
/*				Display_Heads()                           */
/*		      Display field headings in memo gadgets		  */
/*------------------------------------------------------------------------*/
void Display_Heads()
  {
  char field_heads_str[61] =
      "Date :             Notice :        Type : ";
  struct IntuiText field_heads =
      { 1, 0, JAM1, 5, 2, &my_font, NULL, NULL };
  int i;

  field_heads.IText=field_heads_str;
  for (i=0 ; i<6 ; i++) PrintIText(mm_w->RPort,
				    &field_heads,
				    M_GAD_X,
				    gadget_display[i].TopEdge);
  }

/*------------------------------------------------------------------------*/
/*				 Display_One()                            */
/*		Move one memo into IntuiText structures and print	  */
/*------------------------------------------------------------------------*/
void Display_One(int gadg_no, struct MI_Mem *mim)
  {
  int fc, bc;

  Display_Blank(gadg_no);

  m_gad_date[gadg_no].IText=mim->mim_MI.mi_Date;
  m_gad_notice[gadg_no].IText=mim->mim_MI.mi_Notice;
  m_gad_type[gadg_no].IText=mim->mim_MI.mi_Type;
  mem_t_line[gadg_no].IText=mim->mim_MI.mi_Text;

  bc=Colour[0];
  if (mim->mim_Select == 0)
    fc=Colour[2];
  else
    fc=Colour[1];

  m_gad_date[gadg_no].FrontPen=fc;
  m_gad_notice[gadg_no].FrontPen=fc;
  m_gad_type[gadg_no].FrontPen=fc;
  mem_t_line[gadg_no].FrontPen=fc;
  m_gad_date[gadg_no].BackPen=bc;
  m_gad_notice[gadg_no].BackPen=bc;
  m_gad_type[gadg_no].BackPen=bc;
  mem_t_line[gadg_no].BackPen=bc;

  PrintIText(mm_w->RPort,
	&m_gad_date[gadg_no],
	M_GAD_X,
	gadget_display[gadg_no].TopEdge );
  }

/*------------------------------------------------------------------------*/
/*			       Display_Blank()                            */
/*	    Move blank 'memo' into IntuiText structures and print         */
/*------------------------------------------------------------------------*/
void Display_Blank(int gadg_no )
  {
  PrintIText(mm_w->RPort,
	&blank_details[0],
	M_GAD_X,
	gadget_display[gadg_no].TopEdge );
  }

/* ============================================================================
 * Function to display 'iconified' window to user and wait until user
 *   ready to continue with main window.
 *
 * Calling sequence should be something like :-
 *   CloseWindow(mm_w);
 *   Shrink();
 *   mm_w=OpenWindow(nw);
 *   if (!mm_w) panic!!!
 *   Display_Heads();
 *   Display_Block(DisplayFirst);
 *
 */
Shrink()
  {
  SHORT Points[]= {
	19, 2,
	33, 2,
	33, 8,
	19, 8,
	19, 2,
	22, 2,
	22, 5,
	19, 5  };
  struct Border Bdr = { 0, 0, 1, 2, JAM1, 8, NULL, NULL };
  struct Gadget g_exp=
    {
    NULL,		    /* *NextGadget */
    1, 12, 53, 11,	    /* LeftEdge, TopEdge, Width, Height */
    GADGHCOMP,		    /* Flags */
    RELVERIFY,		    /* Activation */
    BOOLGADGET, 	    /* GadgetType */
    NULL,		    /* GadgetRender */
    NULL,		    /* SelectRender */
    NULL,		    /* *GadgetText */
    NULL,		    /* MutualExclude */
    NULL,		    /* SpecialInfo */
    NULL,		    /* GadgetID */
    NULL		    /* UserData */
    };

  struct NewWindow nsw=
    {
    0,0,	      /* LeftEdge, TopEdge */
    55, 24,	      /* Width, Height */
    0,0,	      /* DetailPen, BlockPen */
    GADGETUP,
    WINDOWDEPTH|
    WINDOWDRAG|
    SMART_REFRESH,
    NULL,
    NULL,		/* *CheckMark */
    "MM2 ",             /* *Title */
    NULL,		/* *Screen */
    NULL,		/* *BitMap */
    0,0,		/* MinWidth, MinHeight */
    0,0,		/* MaxWidth, MaxHeight */
    WBENCHSCREEN	/* Type */
    };
  struct Window *sw;
  struct IntuiMessage *msg;
  static int wx,wy = 0;

  nsw.LeftEdge=wx;
  nsw.TopEdge=wy;
  nsw.FirstGadget=&g_exp;
  g_exp.GadgetRender=(APTR)&Bdr;
  Bdr.XY = Points;
  Bdr.FrontPen =Colour[1];
  Bdr.BackPen  =Colour[0];
  nsw.DetailPen=Colour[2];
  nsw.BlockPen =Colour[0];
  sw=(struct Window *)OpenWindow(&nsw);
  if(!sw)
    {
    return -1;
    }
  Wait(1 << sw->UserPort->mp_SigBit);
  while (msg = (struct IntuiMessage *) GetMsg( sw->UserPort ) )
    {
    ReplyMsg( (struct Message *)msg );
    }
  wx = sw->LeftEdge;   /* Save location of shrunk window for next*/
  wy = sw->TopEdge;    /*   time user requests shrink		 */
  CloseWindow(sw);
  return 1;
  }

/* ============================================================================
 * Function SetColours puts values into Colour[] array. Algorithm will probably
 *  need to be changed.
 *
 * Making colours used in program acceptable whatever version of op sys
 *  running (or user amended colours set up).
 *
 * Set up global table as
 *	int Colour[4];
 *  Insert values into table if program will open windows etc and use values
 *  in table where FrontPen, BackPen etc required eg IntuiText structures.
 * Colour[0] used for normal background
 * Colour[1] used for highlighted background - eg warnings
 * Colour[2] used for normal text
 * Colour[3] used for highlighted text - eg selected memos
 *
 */
SetColours()
/* Current algorithm -
 *	0. If file S:MMASTER.CONFIG exists, colours are first 4 bytes.
 *	1. If standard WB1.3 colours then use as .....
 *	2. If standard WB2.04 then use as ......
 *	3. Otherwise, as 1 or 2 above except test which using default
 *	   preference values.
 */
  {
  struct Preferences pref;
  char *u;

/* First, try to get values from config file - if this is present user has
 *  made a definite choice and should be obeyed!
 */

  if (User_Config)
    {
    u=(char *)&mm_prefs;
    Colours(*u,*(u+1),*(u+2),*(u+3));
    return 0;
    }

/* No config file. Use our own scheme
 */

  GetPrefs( &pref, sizeof(pref) );
  if ((   pref.color0 == 0x005a ) &&
	  (pref.color1 == 0x0fff ) &&
	  (pref.color2 == 0x0002 ) &&
	  (pref.color3 == 0x0f80 ) )
	      {
	      Colours('0','1','2','3');
	      return 0;
	      }

  if ((   pref.color0 == 0x0aaa ) &&
	  (pref.color1 == 0x0000 ) &&
	  (pref.color2 == 0x0fff ) &&
	  (pref.color3 == 0x068b ) )
	      {
	      Colours('0','2','1','3');
	      return 0;
	      }

  GetDefPrefs( &pref, sizeof(pref) );
  if ((   pref.color0 == 0x005a ) &&
	  (pref.color1 == 0x0fff ) &&
	  (pref.color2 == 0x0002 ) &&
	  (pref.color3 == 0x0f80 ) )
	      {
	      Colours('0','1','2','3');
	      return 0;
	      }

  if ((   pref.color0 == 0x0aaa ) &&
	  (pref.color1 == 0x0000 ) &&
	  (pref.color2 == 0x0fff ) &&
	  (pref.color3 == 0x068b ) )
	      {
	      Colours('0','2','1','3');
	      return 0;
	      }
  Colours('0','1','2','3');
  return 0;
  }
/* ============================================================================
 * This function actually puts the colour register numbers into the Colours
 *  array in the chosen order. The numbers also need conversion from character
 *  into integer format.
 */
void Colours(char c0, char c1, char c2, char c3)
  {
  Colour[0]=(int)c0-48;
  Colour[1]=(int)c1-48;
  Colour[2]=(int)c2-48;
  Colour[3]=(int)c3-48;
  }

/* ======================================================================
 * Function defining and opening main window
 */
struct Window *MainWindow()
  {
  struct NewWindow n=
    {
    0,0,	      /* LeftEdge, TopEdge */
    640, 200,	      /* Width, Height */
    NULL,NULL,		    /* DetailPen, BlockPen */
    CLOSEWINDOW|      /* IDCMPFlags */
    GADGETDOWN|
    GADGETUP,
    ACTIVATE|	      /* Flags */
    WINDOWDEPTH|
    WINDOWDRAG|
    WINDOWCLOSE|
    SMART_REFRESH,
    &gadget_display[0], /* *FirstGadget */
    NULL,		/* *CheckMark */
    NULL,		/* *Title */
    NULL,		/* *Screen */
    NULL,		/* *BitMap */
    0,0,		/* MinWidth, MinHeight */
    0,0,		/* MaxWidth, MaxHeight */
    WBENCHSCREEN	/* Type */
    };
  n.DetailPen=Colour[0];
  n.BlockPen=Colour[1];
  n.Title = MWTitle;
  border6a_tl.FrontPen = Colour[1];
  border6a_br.FrontPen = Colour[2];
  border6b_tl.FrontPen = Colour[2];
  border6b_br.FrontPen = Colour[1];
  border6a_tl.BackPen = Colour[2];
  border6a_br.BackPen = Colour[1];
  border6b_tl.BackPen = Colour[1];
  border6b_br.BackPen = Colour[2];
  return ((struct Window *)OpenWindow(&n));
  }
