#include <aros/oldprograms.h>

#include <intuition/intuition.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include "mm2.h"

extern int Month2Num(char *);
extern int DisplayC(char **);
extern void DisplayE(char **);
extern void DisplayH(char **);
extern BOOL CheckMemoDate(char *);

extern struct MinList *MemListPtr;
extern struct MinNode *DisplayFirst;
extern struct Remember *RK;
extern int Colour[4];

/*--------------------------------------------------------------------------
 * Prototypes for functions in this module
 */

  BOOL Edit(void);
  BOOL Add(void);
  BOOL Del(void);
  BOOL MemoReq(struct Memo_Item *);
  BOOL MemIns(struct MI_Mem *);
  BOOL MemoDel(struct MI_Mem *);

BOOL DataAmended = FALSE;
extern struct IntuitionBase *IntuitionBase;
extern void Display_Block(struct MinNode *);
BOOL MemoReq(struct Memo_Item *);

static struct IntuiText memo_string_text=
  {
  1, 0, JAM1, 0, -10, NULL, "Memo:", NULL
  };
static struct IntuiText date_string_text=
  {
  1, 0, JAM1, -50, 0, NULL, "Date:", NULL
  };
static struct IntuiText notice_string_text=
  {
  1, 0, JAM1, -80, 0, NULL, "Notice:", NULL
  };
static struct IntuiText type_string_text=
  {
  1, 0, JAM1, -50, 0, NULL, "Type:", NULL
  };
static struct IntuiText ok_text=
  {
  1, 0, JAM1, 4, 2, NULL, "  OK  ", NULL
  };
static struct IntuiText help_text=
  {
  1, 0, JAM1, 4, 2, NULL, " HELP ", NULL
  };
static struct IntuiText cancel_text=
  {
  1, 0, JAM1, 4, 2, NULL, "CANCEL", NULL
  };
static struct IntuiText requester_text=
  {
  1, 0, JAM1, 14, 3, NULL, "Please enter your memo:", NULL
  };

static UBYTE memo_buffer[61];
static UBYTE undo_buffer[61];
static UBYTE date_buffer[10];
static UBYTE notice_buffer[3];
static UBYTE type_buffer[2];

static struct StringInfo memo_string_info=
  {
  memo_buffer, undo_buffer, 0, 61, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL
  };
static struct StringInfo date_string_info=
  {
  date_buffer, undo_buffer, 0, 10, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL
  };
static struct StringInfo notice_string_info=
  {
  notice_buffer, undo_buffer, 0, 3, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL
  };
static struct StringInfo type_string_info=
  {
  type_buffer, undo_buffer, 0, 2, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL
  };

/*----------------------------------------------------------------------
 * data for string gadgets etc
 */

static SHORT type_bpts[]=
  {
   -2,	-1,
  DEA_TYPE_W-2,  -1,
  DEA_TYPE_W-2, DEA_SGAD_H,
   -2, DEA_SGAD_H,
   -2,	-1
  };

static SHORT memo_bpts[]=
  {
   -2,	-1,
  DEA_MEMO_W-2,  -1,
  DEA_MEMO_W-2, DEA_SGAD_H,
   -2, DEA_SGAD_H,
   -2,	-1
  };

static SHORT date_bpts[]=
  {
   -2,	-1,
  DEA_DATE_W-2,  -1,
  DEA_DATE_W-2, DEA_SGAD_H,
   -2, DEA_SGAD_H,
   -2,	-1
  };

static SHORT noti_bpts[]=
  {
   -2,	-1,
  DEA_NOTI_W-3,  -1,
  DEA_NOTI_W-3, DEA_SGAD_H,
   -2, DEA_SGAD_H,
   -2,	-1
  };

static struct Border type_brd=
  {
  0, 0,  NULL, NULL, JAM1, 5, type_bpts, NULL
  };

static struct Border memo_brd=
  {
  0, 0,  NULL, NULL, JAM1, 5, memo_bpts, NULL
  };

static struct Border date_brd=
  {
  0, 0,  NULL, NULL, JAM1, 5, date_bpts, NULL
  };

static struct Border noti_brd=
  {
  0, 0,  NULL, NULL, JAM1, 5, noti_bpts, NULL
  };

static struct Gadget memo_gadget=
  {
  NULL, 20, 32, DEA_MEMO_W, DEA_SGAD_H, GADGHCOMP, RELVERIFY,
  STRGADGET|REQGADGET,(APTR)&memo_brd,NULL,&memo_string_text,NULL,
  (APTR) &memo_string_info,0,NULL
  };

static struct Gadget date_gadget=
  {
  &memo_gadget,100, 14, DEA_DATE_W, DEA_SGAD_H, GADGHCOMP, RELVERIFY,
  STRGADGET|REQGADGET, (APTR)&date_brd, NULL, &date_string_text, NULL,
  (APTR) &date_string_info, 0, NULL
  };
static struct Gadget notice_gadget=
  {
  &date_gadget,300, 14, DEA_NOTI_W, DEA_SGAD_H, GADGHCOMP, RELVERIFY,
  STRGADGET|REQGADGET, (APTR)&noti_brd, NULL, &notice_string_text, NULL,
  (APTR) &notice_string_info, 0, NULL
  };
static struct Gadget type_gadget=
  {
  &notice_gadget,400, 14, DEA_TYPE_W, DEA_SGAD_H, GADGHCOMP, RELVERIFY,
  STRGADGET|REQGADGET, (APTR)&type_brd, NULL, &type_string_text, NULL,
  (APTR) &type_string_info, 0, NULL
  };

/*---------------------------------------------------------------------
 * data for BOOLEAN gadgets
 */
static SHORT bb_pts_tr[]=   /*Top left points*/
  {
   0,  10,
   0,	0,
  54,	0
  };

static SHORT bb_pts_bl[]=   /*Bottom right points*/
  {
  54,  0,
  54, 10,
   0, 10
  };

static struct Border bb0_tr=
  {
  0, 0,  NULL, NULL, JAM1, 3, bb_pts_tr, NULL
  };

static struct Border bb0_bl=
  {
  0, 0,  NULL, NULL, JAM1, 3, bb_pts_bl, &bb0_tr
  };

static struct Border bb1_tr=
  {
  0, 0,  NULL, NULL, JAM1, 3, bb_pts_tr, NULL
  };

static struct Border bb1_bl=
  {
  0, 0,  NULL, NULL, JAM1, 3, bb_pts_bl, &bb1_tr
  };

static struct Gadget ok_gadget=
  {
  &type_gadget,  /* NextGadget */
  14,		 /* LeftEdge */
  47,		 /* TopEdge */
  DEA_BGAD_W,		 /* Width */
  DEA_BGAD_H,
  GADGHIMAGE,	  /* Flags */
  RELVERIFY|	 /* Activation */
  ENDGADGET,
  BOOLGADGET|	 /* GadgetType */
  REQGADGET,
  (APTR) &bb0_bl, /* GadgetRender */
  (APTR) &bb1_bl,          /* SelectRender */
  &ok_text,	 /* GadgetText */
  NULL, 	 /* MutualExclude */
  NULL, 	 /* SpecialInfo */
  0,		 /* GadgetID */
  NULL		 /* UserData */
  };

static struct Gadget help_gadget=
  {
  &ok_gadget,314, 47, DEA_BGAD_W, DEA_BGAD_H, GADGHIMAGE, RELVERIFY,
  BOOLGADGET|REQGADGET, (APTR)&bb0_bl, (APTR)&bb1_bl, &help_text,
  NULL, NULL, 0, NULL
  };

static struct Gadget cancel_gadget=
  {
  &help_gadget,514, 47, DEA_BGAD_W, DEA_BGAD_H, GADGHIMAGE, RELVERIFY|ENDGADGET,
  BOOLGADGET|REQGADGET, (APTR)&bb0_bl, (APTR)&bb1_bl, &cancel_text,
  NULL, NULL, 0, NULL
  };

static SHORT requester_border_points[]=
  {
    0,	0,
  639,	0,
  639, 64,
    0, 64,
    0,	0
  };

static struct Border requester_border=
  {
  0, 0,        /* LeftEdge, TopEdge. */
  1,	       /* FrontPen */
  0,	       /* BackPen */
  JAM1,        /* DrawMode */
  5,	       /* Count */
  requester_border_points, /* XY */
  NULL,        /* NextBorder */
  };


static struct Requester my_requester=
  {
  NULL, 	     /* OlderRequester */
  0, 10,	    /* LeftEdge, TopEdge */
  640, 65,	     /* Width, Height */
  0, 0, 	     /* RelLeft, RelTop */
  &cancel_gadget,    /* ReqGadget */
  &requester_border, /* ReqBorder */
  &requester_text,   /* ReqText */
  NULL, 	     /* Flags */
  NULL, 		/* BackFill */
  NULL, 	     /* ReqLayer */
  NULL, 	     /* ReqPad1 */
  NULL, 	     /* ImageBMap */
  NULL, 	     /* RWindow */
  NULL		     /* ReqPad2 */
  };

static struct Window *my_window;

static struct NewWindow my_new_window=
  {
  0,		 /* LeftEdge */
  0,		 /* TopEdge */
  640,		 /* Width */
  75,		/* Height */
  NULL, 	    /* DetailPen */
  NULL, 	    /* BlockPen */
  GADGETUP|	 /* IDCMPFlags */
  REQCLEAR,
  SMART_REFRESH| /* Flags */
  WINDOWDRAG|
  WINDOWDEPTH|
  ACTIVATE,
  NULL, 	 /* FirstGadget */
  NULL, 	 /* CheckMark */
  "Amigaholics!!!",  /* Title */
  NULL, 	 /* Screen */
  NULL, 	 /* BitMap */
  0,	       /* MinWidth */
  0,		/* MinHeight */
  0,	       /* MaxWidth */
  0,	       /* MaxHeight */
  WBENCHSCREEN	 /* Type */
  };

static char *MAdd_Help[]={
  "",
  "   Date format is \"dd-mmm-yy\"  ",
  "       eg \"21-JAN-91\"   ",
  "   Notice is number, 1 to 99. The ",
  "       number of days notice you want",
  "   Type is A (or a) for Annual   ",
  "           anything else is up to you   ",
  "",
  '\0'
  };
static char *err_txt_reqfail[]={
  "",
  "   Failed to open ADD requester   ",
  '\0'
  };
static char *err_txt_fopenfail[]={
  "",
  "   Failed to open memo file   ",
  '\0'
  };
static char *err_txt_dateinv2[]={
  "",
  "   Couldn\'t write since date was invalid   ",
  '\0'
  };
/*--------------------------------------------------------------------------
 * Function Edit() calls MemoReq() for each selected memo in the list. Prior
 *  to the call, Edit() sets up the current field values in a Memo_Item
 *  structure. Returns TRUE if no errors found, otherwise FALSE. (Actually,
 *  it currently returns TRUE under all circumstances.
 */
BOOL Edit()
  {
  struct Memo_Item m;
  struct MinNode *n;
  BOOL result;


  if (LISTEMPTY) return TRUE;
  n=MemListPtr->mlh_Head;
  while (n->mln_Succ)
    {
    if ( ((struct MI_Mem *)n)->mim_Select == 1)
      {
      strcpy(m.mi_Date , ((struct MI_Mem *)n)->mim_MI.mi_Date);
      strcpy(m.mi_Notice , ((struct MI_Mem *)n)->mim_MI.mi_Notice);
      strcpy(m.mi_Type , ((struct MI_Mem *)n)->mim_MI.mi_Type);
      strcpy(m.mi_Text , ((struct MI_Mem *)n)->mim_MI.mi_Text);
      result = MemoReq(&m);
      if (result)
	{
	strcpy( ((struct MI_Mem *)n)->mim_MI.mi_Date, m.mi_Date);
	strcpy( ((struct MI_Mem *)n)->mim_MI.mi_Notice, m.mi_Notice);
	strcpy( ((struct MI_Mem *)n)->mim_MI.mi_Type, m.mi_Type);
	strcpy( ((struct MI_Mem *)n)->mim_MI.mi_Text, m.mi_Text);
	Remove((struct List *)n);
	MemIns((struct MI_Mem *)n);
	DataAmended=TRUE;
	}
      }
    ((struct MI_Mem *)n)->mim_Select=0;
    n=n->mln_Succ;
    }
  Display_Block(DisplayFirst);
  return TRUE;
  }

/*--------------------------------------------------------------------------
 * Function Add() calls MemoReq() to get data for new memo.
 *  Returns TRUE if no errors found, otherwise FALSE. (Actually,
 *  it currently returns TRUE under all circumstances.
 */
BOOL Add()
  {
  struct Memo_Item m;
  struct MI_Mem *mim;
  BOOL result;

  m.mi_Date[0]='\0';
  m.mi_Notice[0]='\0';
  m.mi_Type[0]='\0';
  m.mi_Text[0]='\0';
  result=MemoReq(&m);
  if (result)
    {
    mim=(struct MI_Mem *)AllocRemember(&RK,sizeof(struct MI_Mem),MEMF_PUBLIC);
    if (!mim) return FALSE;
    strcpy( mim->mim_MI.mi_Date, m.mi_Date);
    strcpy( mim->mim_MI.mi_Notice, m.mi_Notice);
    strcpy( mim->mim_MI.mi_Type, m.mi_Type);
    strcpy( mim->mim_MI.mi_Text, m.mi_Text);
    mim->mim_Select=0;
    result=MemIns(mim);
    if (!result) return FALSE;
    Display_Block(DisplayFirst);
    DataAmended=TRUE;
    }
  return (TRUE);
  }

/*--------------------------------------------------------------------------
 * Function Del() reads through Memo list and, with the confirmation of the
 *  user, deletes selected memos. The memos are actually just removed from
 *  the list, the memory is not released until the program terminates. (Later
 *  deleted memos could be organised as a separate list and a process made
 *  available for the user to change his/her mind and reinsert any memos?)
 *  Returns TRUE if no errors found, otherwise FALSE. (Actually,
 *  it currently returns TRUE under all circumstances.
 */
BOOL Del()
  {
  struct MinNode *n;
  int result;


  if (LISTEMPTY) return TRUE;
  n=MemListPtr->mlh_Head;
  while (n->mln_Succ)
    {
    if ( ((struct MI_Mem *)n)->mim_Select == 1)
      {
      result = MemoDel((struct MI_Mem *)n);
      if (result == 1)
	{
	Remove((struct Node *)n);
	DataAmended=TRUE;
	if ( n = DisplayFirst)
	  {
	  if (LISTEMPTY)
	    DisplayFirst = NULL;
	  else
	    DisplayFirst = MemListPtr->mlh_Head;
	  }
	Display_Block(DisplayFirst);
	}
      }
    n=n->mln_Succ;
    }
  return TRUE;
  }

/*--------------------------------------------------------------------------
 * Function MemIns() allocates a new MI_Mem structure in memory, copies
 *  data into it from a Memo_Item structure whose address was given as a
 *  parameter and then returns TRUE if no errors found, otherwise FALSE.
 */
BOOL MemIns(struct MI_Mem *m)
  {
  char d1[5],d2[5];
  struct MinNode *n;
  BOOL found;
  int cmp,mnum;

  if (LISTEMPTY)
    {
    AddHead(MemListPtr,(struct MinNode *)m);
    }
  else
    {
    mnum = Month2Num((char *)&(m->mim_MI.mi_Date[3]));
    sprintf(d1, "%02d", mnum);
    d1[2]=m->mim_MI.mi_Date[0];
    d1[3]=m->mim_MI.mi_Date[1];
    d1[4]='\0';
    n=MemListPtr->mlh_Head;
    found=FALSE;
    while ( (!found) && (n->mln_Succ) )
      {
      mnum = Month2Num((char *)&(((struct MI_Mem *)n)->mim_MI.mi_Date[3]));
      sprintf(d2, "%02d", mnum);
      d2[2]=((struct MI_Mem *)n)->mim_MI.mi_Date[0];
      d2[3]=((struct MI_Mem *)n)->mim_MI.mi_Date[1];
      d2[4]='\0';
      cmp=strcmp(d1,d2);
      if ( cmp > 0 )
	{
	n=n->mln_Succ;
	}
      else
	{
	Insert( (struct List *)MemListPtr,(struct Node *)m,n->mln_Pred);
	found=TRUE;
	}
      }
      if (!found) AddTail( (struct List *)MemListPtr, (struct Node *)m );
    }
  return TRUE;
  }


/* -------------------------------------------------------------------
 *	Function handling Requester allowing addition of new memos and
 *	editing of existing ones
 */
BOOL MemoReq(struct Memo_Item *mim)
  {
  BOOL close_me;
  ULONG class;
  APTR address;
  struct IntuiMessage *my_message;
  BOOL result;
  int MemoState,DateState;
  char *err_txt_dateinv1[] = {
    "",
    "   Invalid date - try again   ",
    '\0'
    };

  my_new_window.DetailPen = Colour[0];
  my_new_window.BlockPen  = Colour[1];
  requester_border.FrontPen = Colour[2];
  requester_border.BackPen  = Colour[0];
  type_brd.FrontPen = Colour[2];
  type_brd.BackPen =  Colour[0];
  memo_brd.FrontPen = Colour[2];
  memo_brd.BackPen =  Colour[0];
  date_brd.FrontPen = Colour[2];
  date_brd.BackPen =  Colour[0];
  noti_brd.FrontPen = Colour[2];
  noti_brd.BackPen =  Colour[0];
  bb0_bl.FrontPen = Colour[2];
  bb0_bl.BackPen =  Colour[1];
  bb0_tr.FrontPen = Colour[1];
  bb0_tr.BackPen =  Colour[2];
  bb1_bl.FrontPen = Colour[1];
  bb1_bl.BackPen =  Colour[2];
  bb1_tr.FrontPen = Colour[2];
  bb1_tr.BackPen =  Colour[1];
  my_requester.BackFill   = Colour[0];
  strcpy(date_buffer,mim->mi_Date);
  strcpy(memo_buffer,mim->mi_Text);
  strcpy(notice_buffer,mim->mi_Notice);
  strcpy(type_buffer,mim->mi_Type);
  my_window = (struct Window *) OpenWindow( &my_new_window );
  if(my_window == NULL)
    {
    CloseLibrary( (struct Library *)IntuitionBase );
    exit(0);
    }

  result=Request( &my_requester, my_window );
  if( !result )
    {
    DisplayE(err_txt_reqfail);
    }

  close_me = FALSE;
  MemoState=0;
  DateState=0;
  ActivateGadget(&date_gadget,
      my_window,
      &my_requester);
  while( !close_me )
    {
    Wait( 1 << my_window->UserPort->mp_SigBit );
    while(my_message=(struct IntuiMessage *) GetMsg(my_window->UserPort))
      {
      class = my_message->Class;
      address = my_message->IAddress;
      ReplyMsg( (struct Message *)my_message );
      switch( class )
	{
	case GADGETUP:
	  if(address == (APTR) &ok_gadget )
	    {
	    MemoState=1;
	    }
	  if(address == (APTR) &help_gadget )
	    {
	    DisplayH(MAdd_Help);
	    ActivateGadget(&date_gadget,
			    my_window,
			    &my_requester);
	    }
	  if(address == (APTR) &cancel_gadget )
	    {
	    MemoState=0;
	    }
	  if(address == (APTR) &date_gadget )
	    {
	    if(CheckMemoDate(date_buffer) == TRUE )
	      {
	      DateState=1;
	      ActivateGadget(&notice_gadget,
			    my_window,
			    &my_requester);
	      }
	    else
	      {
	      DateState=0;
	      DisplayE(err_txt_dateinv1);
	      ActivateGadget(&date_gadget,
			    my_window,
			    &my_requester);
	      }
	    }
	  if(address == (APTR) &memo_gadget )
	    {
	    ActivateGadget(&date_gadget,
			    my_window,
			    &my_requester);

	    }
	  if(address == (APTR) &notice_gadget )
	    {
	    ActivateGadget(&type_gadget,
			    my_window,
			    &my_requester);

	    }
	  if(address == (APTR) &type_gadget )
	    {

	    ActivateGadget(&memo_gadget,
			    my_window,
			    &my_requester);

	    }

	  break;

	case REQCLEAR:
	  close_me=TRUE;
	}
      }
    }

    if(CheckMemoDate(date_buffer) == TRUE )
      DateState=1;
    else
      DateState=0;
  CloseWindow( my_window );
  if (MemoState==1)
    {
    if(DateState==1)
      {
      strcpy(mim->mi_Date,date_buffer);
      strcpy(mim->mi_Notice,notice_buffer);
      strcpy(mim->mi_Type,type_buffer);
      strcpy(mim->mi_Text,memo_buffer);
      return TRUE;
      }
    else
      {
      DisplayE(err_txt_dateinv2);
      return FALSE;
      }
    }
  else
    {
    return FALSE;
    }

  }

/*======================================================================*/
BOOL MemoDel(struct MI_Mem *memo)
  {
  char *Query[] = {
    "",
    "   Do you want to delete this memo?   ",
    "",
    "", /* Date */
    "", /* Type */
    "", /* Memo Text */
    "",
    '\0' };
  char dbuff[18];   /* "Date : dd-mmm-yy" */
  char tbuff[10];    /* "Type : X" */

  sprintf(tbuff, "Type : %s", memo->mim_MI.mi_Type);
  sprintf(dbuff, "Date : %s", memo->mim_MI.mi_Date);
  Query[3]=dbuff;
  Query[4]=tbuff;
  Query[5]=memo->mim_MI.mi_Text;
  return DisplayC(Query);
  }

