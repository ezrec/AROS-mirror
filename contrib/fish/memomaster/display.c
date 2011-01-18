#include <aros/oldprograms.h>

/* Display.c
 *   Contains functions for displaying text, eg help, messages, errors
 *   and choices (returning 1 for Yes or 2 for No)
 */
#include <intuition/intuition.h>
#include <exec/memory.h>
#include "mm2.h"

#include <stdio.h>

#define MINWINWIDTH 170

extern struct IntuitionBase *IntuitionBase;

static SHORT gbp_tl[]=
  {  0, 10,  0,  0, 70, 0 };

static SHORT gbp_br[]=
  {  70,  0, 70, 10,  0, 10, };

static struct Border gbu_tl=
  {
  0, 0, NULL, NULL, JAM1, 3, gbp_tl, NULL,
  };

static struct Border gbu_br=
  {
  0, 0, NULL, NULL, JAM1, 3, gbp_br, &gbu_tl,
  };

static struct Border gbs_tl=
  {
  0, 0, NULL, NULL, JAM1, 3, gbp_tl, NULL,
  };

static struct Border gbs_br=
  {
  0, 0, NULL, NULL, JAM1, 3, gbp_br, &gbs_tl,
  };

static struct IntuiText req_g_txt_2=
  {
  1,	     /* FrontPen */
  0,	     /* BackPen */
  JAM1,      /* DrawMode */
  4, 2,      /* LeftEdge, TopEdge */
  NULL,      /* ITextFont */
  NULL,      /* IText */
  NULL,      /* NextText */
  };

static struct IntuiText req_g_txt_1=
  {
  1,	     /* FrontPen */
  0,	     /* BackPen */
  JAM1,      /* DrawMode */
  4, 2,      /* LeftEdge, TopEdge */
  NULL,      /* ITextFont */
  NULL,      /* IText */
  NULL,      /* NextText */
  };

static struct Gadget req_g_2=
  {
  NULL, 	 /* NextGadget */
  40,		 /* LeftEdge */
  20,		 /* TopEdge */
  71,		 /* Width */
  11,		 /* Height */
  GADGHIMAGE,	  /* Flags */
  GADGIMMEDIATE| /* Activation */
  RELVERIFY|
  ENDGADGET,
  BOOLGADGET|	 /* GadgetType, */
  REQGADGET,
  (APTR) &gbu_br, /* GadgetRender */
  (APTR) &gbs_br, /* SelectRender */
  &req_g_txt_2,  /* GadgetText */
  NULL, 	 /* MutualExclude */
  NULL, 	 /* SpecialInfo */
  2,		 /* GadgetID */
  NULL		 /* UserData */
  };

static struct Gadget req_g_1=
  {
  NULL, 	 /* NextGadget */
  40,		 /* LeftEdge */
  20,		 /* TopEdge */
  71,		 /* Width */
  11,		 /* Height */
  GADGHIMAGE,	  /* Flags */
  GADGIMMEDIATE| /* Activation */
  RELVERIFY|
  ENDGADGET,
  BOOLGADGET|	 /* GadgetType, */
  REQGADGET,
  (APTR) &gbu_br, /* GadgetRender */
  (APTR) &gbs_br, /* SelectRender */
  &req_g_txt_1,  /* GadgetText */
  NULL, 	 /* MutualExclude */
  NULL, 	 /* SpecialInfo */
  1,		 /* GadgetID */
  NULL		 /* UserData */
  };

static struct Requester my_requester=
  {
  NULL, 	     /* OlderRequester */
  3, 11,	     /* LeftEdge, TopEdge */
  0, 0, 	     /* Width, Height */
  0, 0, 	     /* RelLeft, RelTop */
  &req_g_1,	     /* ReqGadget */
  NULL, 	     /* ReqBorder */
  NULL, 	     /* ReqText */
  NULL, 	     /* Flags */
  0,		     /* BackFill */
  NULL, 	     /* ReqLayer */
  NULL, 	     /* ReqPad1 */
  NULL, 	     /* ImageBMap */
  NULL, 	     /* RWindow */
  NULL		     /* ReqPad2 */
  };

static struct NewWindow my_new_window=
  {
  0,		 /* LeftEdge */
  0,		 /* TopEdge  */
  0,		 /* Width    */
  0,		 /* Height   */
  0,		 /* DetailPen*/
  1,		 /* BlockPen */
  GADGETUP,	 /* IDCMP */
  SMART_REFRESH| /* Flags */
  WINDOWDRAG|
  WINDOWDEPTH|
  ACTIVATE,
  NULL, 	 /* FirstGadget */
  NULL, 	 /* CheckMark */
  NULL, 	 /* Title */
  NULL, 	 /* Screen */
  NULL, 	 /* BitMap */
  0, 0, 0, 0,
  WBENCHSCREEN	 /* Type */
  };


Display(char **txt, int Mode, int FColour, int BColour)
{
  extern int Colour[4];
  BOOL close_me,failed;
  struct IntuiMessage *my_message;
  BOOL result;
  int linelen,maxlen,txtline,choice;
  struct Window *my_window=NULL;
  struct IntuiText *IT1,*IT2;
  struct Remember *txt_RK=NULL;
  struct Border rb1;
  struct Gadget *gp;
  SHORT rbp1[10];

  maxlen=0;
  IT2=NULL;
  txtline=0;
  failed=FALSE;
  while((*(txt+txtline)) && (!failed) )
    {
    IT1=(struct IntuiText *)
	AllocRemember(&txt_RK,sizeof(struct IntuiText),MEMF_PUBLIC);
    if(IT1)
      {
      IT1->FrontPen=Colour[FColour];
      IT1->DrawMode=JAM1;
      IT1->BackPen=0;
      IT1->LeftEdge=10;
      IT1->TopEdge=4+10*txtline;
      IT1->ITextFont=NULL;
      IT1->IText=(UBYTE *)*(txt+txtline);
      IT1->NextText=NULL;
      if(IT2==NULL)
	{
	my_requester.ReqText=IT1;
	IT2=IT1;
	}
      else
	{
	IT2->NextText=IT1;
	IT2=IT1;
	}
      linelen=IntuiTextLength(IT1);
      if(linelen>maxlen)maxlen=linelen;
      txtline++;
      }
    else
      {
      failed=TRUE;
      }
    }
  if(!failed)
    {
    if(maxlen<MINWINWIDTH)maxlen=MINWINWIDTH;
    my_requester.Width=maxlen+20;
    my_requester.Height=10*txtline+20;
    my_requester.BackFill=Colour[BColour];
    req_g_txt_1.FrontPen = Colour[FColour];
    req_g_txt_2.FrontPen = Colour[FColour];
    my_new_window.Width=my_requester.Width;
    my_new_window.Height=my_requester.Height+10;
    my_new_window.LeftEdge=((640-my_requester.Width)/2)-1;
    my_new_window.TopEdge=((200-my_requester.Height)/2)-1;
    my_new_window.Title="Amigaholics!!";
    if (Mode == 0)      /*Choice*/
      {
      req_g_1.NextGadget = &req_g_2;
      req_g_1.TopEdge = req_g_2.TopEdge = my_requester.Height - 15;
      req_g_1.LeftEdge = 10;
      req_g_2.LeftEdge = my_requester.Width - 80;
      req_g_txt_1.FrontPen=Colour[FColour];
      req_g_txt_2.FrontPen=Colour[FColour];
      req_g_txt_1.IText = "  YES  ";
      req_g_txt_2.IText = "   NO  ";
      }
    else
      {
      req_g_1.NextGadget = NULL;
      req_g_1.TopEdge = my_requester.Height - 15;
      req_g_1.LeftEdge = (my_requester.Width - 70) / 2;
      req_g_txt_1.FrontPen=Colour[FColour];
      req_g_txt_1.IText = "   OK  ";
      }
    rb1.LeftEdge=0;
    rb1.TopEdge =0;
    rb1.FrontPen=Colour[FColour];
    rb1.DrawMode=JAM1;
    rb1.Count=5;
    rb1.XY=rbp1;
    rb1.NextBorder=NULL;
    rbp1[0]=1;			    rbp1[1]=1;
    rbp1[2]=1;			    rbp1[3]=my_requester.Height-4;
    rbp1[4]=my_requester.Width-4;   rbp1[5]=my_requester.Height-4;
    rbp1[6]=my_requester.Width-4;   rbp1[7]=1;
    rbp1[8]=1;			    rbp1[9]=1;
    gbu_tl.FrontPen=Colour[1];
    gbu_br.FrontPen=Colour[2];
    gbs_tl.FrontPen=Colour[2];
    gbs_br.FrontPen=Colour[1];
    my_requester.ReqBorder=&rb1;

    my_window = (struct Window *) OpenWindow( &my_new_window );
    if(my_window == NULL)
      {
      if(txt_RK)FreeRemember(&txt_RK,TRUE);
      return 0;
      }

    result=Request( &my_requester, my_window );
    if( !result )
      {
      printf("Could not activate the requester!\n");
      if(my_window)CloseWindow(my_window);
      if(txt_RK)FreeRemember(&txt_RK,TRUE);
      return 0;
      }

    if (Mode == 2)      /*Error*/
      {
      DisplayBeep(my_window->WScreen);
      }
    close_me = FALSE;
    while( !close_me )
      {
      Wait( 1 << my_window->UserPort->mp_SigBit );
      while(my_message=(struct IntuiMessage *) GetMsg(my_window->UserPort))
	{
	gp = (struct Gadget *)my_message->IAddress;
	ReplyMsg( (struct Message *)my_message );
	choice = gp->GadgetID;
	  close_me=TRUE;
	}
      }
    }
  if(txt_RK)FreeRemember(&txt_RK,TRUE);
  if(my_window)CloseWindow( my_window );
  return choice;
}


/*====================================================================*/
void DisplayE(char **txt)
  {
  Display(txt, 2, 0, 3);
  }
/*====================================================================*/
void DisplayT(char **txt)
  {
  Display(txt, 1, 1, 0);
  }
/*====================================================================*/
void DisplayH(char **txt)
  {
  Display(txt, 1, 2, 3);
  }
/*====================================================================*/
DisplayC(char **txt)
  {
  return Display(txt, 0, 2, 3);
  }
/*====================================================================*/
