#include <aros/oldprograms.h>

/* Defines for memo gadget position and size */
#define M_GAD_X  18
#define M_GAD0_Y  14
#define M_GAD1_Y  38
#define M_GAD2_Y  62
#define M_GAD3_Y  86
#define M_GAD4_Y  110
#define M_GAD5_Y  134
#define DATE_X 75
#define NOTICE_X 250
#define TYPE_X 350
#define DATE_NOTICE_TYPE_Y 2
#define MEMO_TEXT_X 5
#define MEMO_TEXT_Y 14

#include <intuition/intuition.h>

struct TextAttr my_font=
{
  "topaz.font", /* Font Name */
  TOPAZ_EIGHTY, /* Font Height */
  FS_NORMAL,	/* Style */
  FPF_ROMFONT	/* Preferences */
};

/* Values for a 6-letter box: */
SHORT pts6_tl[]=
{
   -1, 14,
   -1,	-1,
  60,  -1
};
SHORT pts6_br[]=
{
  60,  -1,
  60, 14,
   -1, 14
};

/* Borders for a 6-letter 3d box: */
struct Border border6a_tl=
{
  0, 0, 	/* LeftEdge, TopEdge */
  NULL, NULL, JAM1,   /* FrontPen, BackPen, DrawMode */
  3,		/* Count */
  pts6_tl,	/* *XY */
  NULL		/* *NextBorder */
};
struct Border border6a_br=
{
  0, 0, 	/* LeftEdge, TopEdge */
  NULL, NULL, JAM1,   /* FrontPen, BackPen, DrawMode */
  3,		/* Count */
  pts6_br,	/* *XY */
  &border6a_tl		/* *NextBorder */
};
struct Border border6b_tl=
{
  0, 0, 	/* LeftEdge, TopEdge */
  NULL, NULL, JAM1,   /* FrontPen, BackPen, DrawMode */
  3,		/* Count */
  pts6_tl,	/* *XY */
  NULL		/* *NextBorder */
};
struct Border border6b_br=
{
  0, 0, 	/* LeftEdge, TopEdge */
  NULL, NULL, JAM1,   /* FrontPen, BackPen, DrawMode */
  3,		/* Count */
  pts6_br,	/* *XY */
  &border6b_tl		/* *NextBorder */
};

/* Values for a 60-letter box: */
SHORT points60[]=
{
   0,  0,
  599,	0,
  599, 22,
   0, 22,
   0,  0
};

/* A border for a 60-letter box: */
struct Border border_text60=
{
  0, 0, 	/* LeftEdge, TopEdge */
  1, 2, JAM1,	/* FrontPen, BackPen, DrawMode */
  5,		/* Count */
  points60,	 /* *XY */
  NULL		/* *NextBorder */
};

/* Details for box surrounding memo gadgets */
SHORT points_big_box[]=
{
   0, 0,
 608, 0,
 608, 149,
   0, 149,
   0, 0
};
struct Border border_big_box=
{
  -4, -3,	    /* LeftEdge, TopEdge */
  1, 2, JAM1,	  /* FrontPen, BackPen, DrawMode */
  5,		  /* Count */
  points_big_box, /* *XY */
  &border_text60	    /* *NextBorder */
};

/* Gadgets in window :- 			data names	ID
     1. "  Add "                            g_add + t_add       6
     2. "Delete"                            g_del + t_del       7
     3. " Edit "                            g_edi + t_edi       8
     4. "Check "                            g_chk + t_chk       9
     5. "Shrink"                            g_shr + t_shr       10
     6. " Help "                            g_hlp + t_hlp       11
     7. "  |<  " (Beginning of list)        g_beg + t_beg       12
     8. "  <<  " (Backwards one block)      g_bkb + t_bkb       13
     9. "  <   " (Backwards one item)       g_bki + t_bki       14
    10. "   >  " (Forwards one item)        g_fwi + t_fwi       15
    11. "  >>  " (Forwards one block)       g_fwb + t_fwb       16
    12. "  >|  " (End of list)              g_eol + t_eol       17
    plus 6 gadgets for display/selection of memos.		(0-5)

    Data for each gadget headed by *Gnn* marker eg *G01* or *G11* */

/* *G01***************************************************************** */
/* * Information for the gadget "  Add "                               * */
/* ********************************************************************* */

struct IntuiText t_add=
{ 1, 2, JAM1, 7,4, &my_font, "  Add ", NULL };

struct Gadget g_add=
{
  NULL, 	/* *NextGadget */
  40, 182, 61, 15,	  /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_add,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  6,		       /* GadgetID */
  NULL			  /* UserData */
};
/* *G02***************************************************************** */
/* * Information for the gadget "Delete"                         * */
/* ********************************************************************* */

struct IntuiText t_del=
{ 1, 2, JAM1, 7,4, &my_font, "Delete", NULL };

struct Gadget g_del=
{
  &g_add,	  /* *NextGadget */
  140, 182, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_del,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  7,		       /* GadgetID */
  NULL			  /* UserData */
};
/* *G03***************************************************************** */
/* * Information for the gadget " Edit "                         * */
/* ********************************************************************* */

struct IntuiText t_edi=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  " Edit ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_edi=
{
  &g_del,	  /* *NextGadget */
  240, 182, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_edi,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  8,		       /* GadgetID */
  NULL			  /* UserData */
};
/* *G04***************************************************************** */
/* * Information for the gadget "Check "                         * */
/* ********************************************************************* */

struct IntuiText t_chk=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  "Check ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_chk=
{
  &g_edi,	  /* *NextGadget */
  340, 182, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_chk,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  9,		       /* GadgetID */
  NULL			  /* UserData */
};
/* *G05***************************************************************** */
/* * Information for the gadget "Shrink"                       * */
/* ********************************************************************* */

struct IntuiText t_shr=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  "Shrink",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_shr=
{
  &g_chk,	  /* *NextGadget */
  440, 182, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_shr,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  10,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G06***************************************************************** */
/* * Information for the gadget " Help "                       * */
/* ********************************************************************* */

struct IntuiText t_hlp=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  " Help ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_hlp=
{
  &g_shr,	  /* *NextGadget */
  540, 182, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHCOMP,		  /* Flags */
  RELVERIFY|TOGGLESELECT,	       /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_hlp,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  11,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G07***************************************************************** */
/* * Information for the gadget "  |<  "                         * */
/* ********************************************************************* */

struct IntuiText t_beg=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  "  |<  ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_beg=
{
  &g_hlp,	  /* *NextGadget */
  40, 166, 61, 15,	  /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_beg,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  12,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G08***************************************************************** */
/* * Information for the gadget "  <<  "                       * */
/* ********************************************************************* */

struct IntuiText t_bkb=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  "  <<  ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_bkb=
{
  &g_beg,	  /* *NextGadget */
  140, 166, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_bkb,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  13,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G09***************************************************************** */
/* * Information for the gadget "  <   "                         * */
/* ********************************************************************* */

struct IntuiText t_bki=
{
  1, 2,     /* FrontPen, BackPen */
  JAM1,     /* DrawMode */
  7,4,	    /* LeftEdge, TopEdge */
  &my_font, /* *ITextFont, (Topaz, 80) */
  "  <   ",   /* *IText */
  NULL	    /* *NextText */
};

struct Gadget g_bki=
{
  &g_bkb,	  /* *NextGadget */
  240, 166, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_bki,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  14,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G10***************************************************************** */
/* * Information for the gadget "   >  "                         * */
/* ********************************************************************* */

struct IntuiText t_fwi=
{ 1, 2, JAM1, 7,4, &my_font, "   >  ", NULL };

struct Gadget g_fwi=
{
  &g_bki,	  /* *NextGadget */
  340, 166, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_fwi,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  15,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G11***************************************************************** */
/* * Information for the gadget "  >>  "                         * */
/* ********************************************************************* */

struct IntuiText t_fwb=
{ 1, 2, JAM1, 7,4, &my_font, "  >>  ", NULL };

struct Gadget g_fwb=
{
  &g_fwi,	  /* *NextGadget */
  440, 166, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_fwb,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  16,			/* GadgetID */
  NULL			  /* UserData */
};
/* *G12***************************************************************** */
/* * Information for the gadget "  >|  "                         * */
/* ********************************************************************* */

struct IntuiText t_eol=
{ 1, 2, JAM1, 7,4, &my_font, "  >|  ", NULL };

struct Gadget g_eol=
{
  &g_fwb,	  /* *NextGadget */
  540, 166, 61, 15,	   /* LeftEdge, TopEdge, Width, Height */
  GADGHIMAGE,		   /* Flags */
  RELVERIFY,		  /* Activation */
  BOOLGADGET,		  /* GadgetType */
  (APTR)&border6a_br,   /* GadgetRender */
  (APTR)&border6b_br,                   /* SelectRender */
  &t_eol,	       /* *GadgetText */
  NULL, 		  /* MutualExclude */
  NULL, 		  /* SpecialInfo */
  17,			/* GadgetID */
  NULL			  /* UserData */
};

struct IntuiText mem_t_line[6] =
    {
      { 1,0,JAM2,5,14,&my_font,NULL,NULL },
      { 1,0,JAM2,5,14,&my_font,NULL,NULL },
      { 1,0,JAM2,5,14,&my_font,NULL,NULL },
      { 1,0,JAM2,5,14,&my_font,NULL,NULL },
      { 1,0,JAM2,5,14,&my_font,NULL,NULL },
      { 1,0,JAM2,5,14,&my_font,NULL,NULL }
    };

struct IntuiText m_gad_type[6] =
    {
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[0] },
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[1] },
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[2] },
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[3] },
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[4] },
      { 1,0,JAM2,350,2,&my_font,NULL,&mem_t_line[5] }
    };

struct IntuiText m_gad_notice[6] =
    {
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[0] },
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[1] },
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[2] },
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[3] },
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[4] },
      { 1,0,JAM2,250,2,&my_font,NULL,&m_gad_type[5] }
    };

struct IntuiText m_gad_date[6] =
    {
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[0] },
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[1] },
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[2] },
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[3] },
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[4] },
      { 1,0,JAM2,75,2,&my_font,NULL,&m_gad_notice[5] }
    };


char blank_date_str[7] = "      ";
char blank_notice_str[3] = "  ";
char blank_type_str[2] = " ";
char blank_memo_str[61] =
    "                                                            ";
struct IntuiText blank_details[4] =
  {
    { 1,0,JAM2,DATE_X,DATE_NOTICE_TYPE_Y,&my_font,blank_date_str,&blank_details[1] },
    { 1,0,JAM2,NOTICE_X,DATE_NOTICE_TYPE_Y,&my_font,blank_notice_str,&blank_details[2] },
    { 1,0,JAM2,TYPE_X,DATE_NOTICE_TYPE_Y,&my_font,blank_type_str,&blank_details[3] },
    { 1,0,JAM2,MEMO_TEXT_X,MEMO_TEXT_Y,&my_font,blank_memo_str,NULL }
  };


struct Gadget gadget_display[6]=
{
  {
    &gadget_display[1], /* *NextGadget */
    M_GAD_X, M_GAD0_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_big_box,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    0,		     /* GadgetID */
    NULL		/* UserData */
  },
  {
    &gadget_display[2], /* *NextGadget */
    M_GAD_X, M_GAD1_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_text60,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    1,		     /* GadgetID */
    NULL		/* UserData */
  },
  {
    &gadget_display[3], /* *NextGadget */
    M_GAD_X, M_GAD2_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_text60,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    2,		     /* GadgetID */
    NULL		/* UserData */
  },
  {
    &gadget_display[4], /* *NextGadget */
    M_GAD_X, M_GAD3_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_text60,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    3,		     /* GadgetID */
    NULL		/* UserData */
  },
  {
    &gadget_display[5], /* *NextGadget */
    M_GAD_X, M_GAD4_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_text60,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    4,		     /* GadgetID */
    NULL		/* UserData */
  },
  {
    &g_eol, /* *NextGadget */
    M_GAD_X, M_GAD5_Y, 600, 23,
    GADGHNONE,		/* Flags */
    RELVERIFY,	    /* Activation */
    BOOLGADGET, 	/* GadgetType */
    (APTR) &border_text60,               /* GadgetRender */
    NULL,		/* SelectRender */
    NULL,		/* *GadgetText */
    NULL,		/* MutualExclude */
    NULL,		/* SpecialInfo */
    5,		     /* GadgetID */
    NULL		/* UserData */
  }
};


/* *********************************************************************
 * Structure for the main window now in function MainWindow() in mm2.c
 * ********************************************************************* */

char *Help_Intro[]= {
    "",
    "                  MemoMaster V2.0",
    "",
    "   Click on gadgets to display specific help information.   ",
    "",
    "   Click on HELP again to return to normal operation.",
    "",
    '\0' };

