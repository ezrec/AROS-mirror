extern struct GfxBase *GfxBase;
struct Screen   *Screen=NULL;
struct Window   *Window=NULL,*iWindow=NULL;
struct RastPort *rp,*irp;
ULONG iflags;

struct TextAttr StdFont = {"XEN.font",9,0,0};


UWORD Pens_3D[] = {-1};

struct TagItem ScreenItem[] =
{ {SA_Title,     (ULONG)((ULONG *)"Rotate!")},
  {SA_Width,     724},
  {SA_Height,    480},
  {SA_Depth,     MAXBITMAP},
  {SA_Font,      (ULONG) &StdFont},
  {SA_Type,      CUSTOMSCREEN},
/*  {SA_DisplayID, NTSC_MONITOR_ID|HIRESLACE_KEY}, */
  {SA_Pens,      (ULONG) Pens_3D},
/*  {SA_Overscan,  OSCAN_MAX}, */
  {TAG_DONE,0L}
};

WORD SharedBordersPairs0[] = {
  -2,-1,-2,14,-1,14,-1,-1,174,-1 };
WORD SharedBordersPairs1[] = {
  -1,14,174,14,174,0,175,-1,175,14 };
WORD SharedBordersPairs2[] = {
  0,0,0,25,1,25,1,0,57,0 };
WORD SharedBordersPairs3[] = {
  1,25,57,25,57,1,58,0,58,25 };

struct Border SharedBorders[] = {
  {0,0,1,0,JAM1,5,&SharedBordersPairs0[0],&SharedBorders[1]},
  {0,0,2,0,JAM1,5,&SharedBordersPairs1[0],NULL},
  {0,0,2,0,JAM1,5,&SharedBordersPairs2[0],&SharedBorders[3]},
  {0,0,1,0,JAM1,5,&SharedBordersPairs3[0],NULL},
  {0,0,1,0,JAM1,5,&SharedBordersPairs2[0],&SharedBorders[5]},
  {0,0,2,0,JAM1,5,&SharedBordersPairs3[0],NULL}
};

struct IntuiText okgad_text = {
  1,0,JAM1,23,9,NULL,(UBYTE *)"OK",NULL };

#define okgad_ID 2

struct Gadget okgad = {
  NULL,43,58,60,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  &okgad_text,0L,NULL,okgad_ID,NULL };

struct IntuiText cancelgad_text = {
  1,0,JAM1,8,9,NULL,(UBYTE *)"Cancel",NULL };

#define cancelgad_ID 1

struct Gadget cancelgad = {
  &okgad,156,58,60,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  &cancelgad_text,0L,NULL,cancelgad_ID,NULL };

UBYTE numbergad_buf[4]="15";

struct StringInfo numbergad_info = {
  numbergad_buf,NULL,0,4,0,0,0,0,0,0,NULL,0L,NULL };

#define numbergad_ID 0

struct Gadget numbergad = {
  &cancelgad,43,27,174,14,
  GFLG_GADGHBOX,
  GACT_RELVERIFY+GACT_STRINGCENTER+GACT_LONGINT,
  GTYP_STRGADGET,
  (APTR)&SharedBorders[0],NULL,
  NULL,0L,(APTR)&numbergad_info,numbergad_ID,NULL };

struct TagItem InfoWindowItem[] =
{ {WA_Top,        11},
  {WA_Left,       5},
  {WA_Width,      255},
  {WA_Height,     110},
  {WA_CustomScreen,  0L},
/*  {WA_PubScreen,  NULL}, */
  {WA_IDCMP,      IDCMP_CLOSEWINDOW|IDCMP_GADGETUP},
  {WA_Flags,      WFLG_ACTIVATE|WFLG_SMART_REFRESH|WFLG_CLOSEGADGET|WFLG_DEPTHGADGET|WFLG_DRAGBAR|WFLG_RMBTRAP},
  {WA_Title,      (ULONG)((ULONG *)"Preferences")},
  {WA_Gadgets,    (ULONG)((struct Gadget *)&numbergad)},
  {TAG_DONE,0L}
};

UBYTE restxt[53];

struct IntuiText result_text = {
  1,0,JAM1,20,9,NULL,(UBYTE *)restxt,NULL };

struct TagItem ResultWindowItem[] =
{ {WA_Top,        11},
  {WA_Left,       5},
  {WA_Width,      400},
  {WA_Height,     110},
  {WA_CustomScreen,  (IPTR)NULL},
/*  {WA_PubScreen,  NULL}, */
  {WA_IDCMP,      IDCMP_CLOSEWINDOW|IDCMP_GADGETUP},
  {WA_Flags,      WFLG_ACTIVATE|WFLG_SMART_REFRESH|WFLG_CLOSEGADGET|WFLG_DEPTHGADGET|WFLG_DRAGBAR|WFLG_RMBTRAP},
  {WA_Gadgets,    (ULONG)((struct Gadget *)&okgad)},
  {WA_Title,      (ULONG)((ULONG *)"Result")},
  {TAG_DONE,0L}
};

struct TagItem WindowItem[] =
{ {WA_AutoAdjust, TRUE},
  {WA_CustomScreen,  0L},
/*  {WA_PubScreen,  NULL}, */
  {WA_Width,     724},
  {WA_Height,    480},
  {WA_IDCMP,      IDCMP_CLOSEWINDOW|IDCMP_GADGETUP|IDCMP_MOUSEBUTTONS|IDCMP_MENUPICK},
  {WA_Flags,      WFLG_ACTIVATE|WFLG_SMART_REFRESH|WFLG_CLOSEGADGET|WFLG_DEPTHGADGET},
  {WA_Title,      (ULONG)((ULONG *)"Rotate!")},
  {WA_NewLookMenus,TRUE},
  {TAG_DONE,0L}
};

struct MenuItem menuX[MAXX-MINX+1],menuY[MAXY-MINY+1],menuE[MAXY],menuC[MAXCOLORS-1];
struct IntuiText txtX[MAXX-MINX+1],txtY[MAXY-MINY+1],txtE[MAXY],txtC[MAXCOLORS-1];
char tt[MAXX+1][3];

struct IntuiText itemxye =
{ 1,0,JAM1,18,-1,NULL,NULL,NULL
};

struct MenuItem menuxye =
{ NULL,
  50,0,
  40,MENHI,
  MENUFLAGSN,
  0xFFFF,
  (APTR)&itemxye,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText txtM2 =
{ 1,0,JAM1,15,-1,NULL,"Rotate",NULL
};

struct MenuItem menuM2 =
{ NULL,
  50,MENHI*2,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT,
  0xFFFB,
  (APTR)&txtM2,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText txtM1 =
{ 1,0,JAM1,15,-1,NULL,"Remove",NULL
};

struct MenuItem menuM1 =
{ &menuM2,
  50,MENHI,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT,
  0xFFFD,
  (APTR)&txtM1,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText txtM0 =
{ 1,0,JAM1,15,-1,NULL,"Normal",NULL
};

struct MenuItem menuM0 =
{ &menuM1,
  50,0,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHBOX|CHECKIT|CHECKED,
  0xFFFE,
  (APTR)&txtM0,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText item14 =
{ 1,0,JAM1,0,-1,NULL,"Mode",NULL
};

struct MenuItem menue14 =
{ NULL,
  0,MENHI*3,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item14,
  NULL,
  0,
  &menuM0,
  0
};

struct IntuiText item13 =
{ 1,0,JAM1,0,-1,NULL,"Colors",NULL
};

struct MenuItem menue13 =
{ &menue14,
  0,MENHI*4,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item13,
  NULL,
  0,
  &menuC[0],
  0
};

struct IntuiText item12 =
{ 1,0,JAM1,0,-1,NULL,"<-E->",NULL
};

struct MenuItem menue12 =
{ &menue13,
  0,MENHI*2,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item12,
  NULL,
  0,
  &menuE[0],
  0
};

struct IntuiText item11 =
{ 1,0,JAM1,0,-1,NULL,"<-Y->",NULL
};

struct MenuItem menue11 =
{ &menue12,
  0,MENHI,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item11,
  NULL,
  0,
  &menuY[0],
  0
};

struct IntuiText item10 =
{ 1,0,JAM1,0,-1,NULL,"<-X->",NULL
};

struct MenuItem menue10 =
{ &menue11,
  0,0,
  60,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item10,
  NULL,
  0,
  &menuX[0],
  0
};

struct Menu menu1 =
{ NULL,
  100,0,
  50,5,
  MENUENABLED,
  "Prefs",
  &menue10
};

struct IntuiText item04 =
{ 1,0,JAM1,0,-1,NULL,"Stop!",NULL
};

struct MenuItem menue04 =
{ NULL,
  0,MENHI*2,
  100,MENHI,
  ITEMTEXT|HIGHCOMP,
  0,
  (APTR)&item04,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText item03 =
{ 1,0,JAM1,0,-1,NULL,"Backdrop",NULL
};

struct MenuItem menue03 =
{ &menue04,
  0,MENHI*5,
  100,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item03,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText item02 =
{ 1,0,JAM1,0,-1,NULL,"Start",NULL
};

struct MenuItem menue02 =
{ &menue03,
  0,MENHI,
  100,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item02,
  NULL,
  0,
  NULL,
  0
};

struct IntuiText item01 =
{ 1,0,JAM1,0,-1,NULL,"Quit...",NULL
};

struct MenuItem menue01 =
{ &menue02,
  0,MENHI*3,
  100,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP|ITEMTEXT|COMMSEQ,
  0,
  (APTR)&item01,
  NULL,
  'Q',
  NULL,
  0
};

struct IntuiText item00 =
{ 1,0,JAM1,0,-1,NULL,"Info!",NULL
};

struct MenuItem menue00 =
{ &menue01,
  0,0,
  100,MENHI,
  ITEMTEXT|ITEMENABLED|HIGHCOMP,
  0,
  (APTR)&item00,
  NULL,
  0,
  NULL,
  0
};

struct Menu menu0 =
{ &menu1,
  0,0,
  100,5,
  MENUENABLED,
  "Rotate!",
  &menue00
};

#define FIRSTMENU menu0

void open_screen()
{
int i;

  if((Screen=(struct Screen *) OpenScreenTagList(NULL,ScreenItem))==NULL)
    error=TRUE;
  else
  {
    WindowItem[1].ti_Data = (ULONG)Screen;
    InfoWindowItem[4].ti_Data = (ULONG)Screen;
    ResultWindowItem[4].ti_Data = (ULONG)Screen;
    SetRGB4(&Screen->ViewPort,7,0,7,0);

    if((Window=(struct Window *) OpenWindowTagList(NULL,WindowItem))==NULL)
      error=TRUE;
    else
    {
      iflags = Window->IDCMPFlags;
      rp=Window->RPort;

      for(i=0;i<(MAXX+1);i++)
        sprintf(tt[i],"%2d",i);

      for(i=0;i<(MAXX-MINX+1);i++)
      {
        menuX[i]=menuxye;
        txtX[i]=itemxye;
        txtX[i].IText=(UBYTE *)tt[i+MINX];
        menuX[i].NextItem=&menuX[i+1];
        menuX[i].TopEdge=MENHI*i;
        menuX[i].ItemFill=(APTR)&txtX[i];
        menuX[i].MutualExclude-=(1<<i);
      }
      menuX[MAXX-MINX].NextItem=NULL;
      menuX[newlevel.b-MINX].Flags=MENUFLAGSC;

      for(i=0;i<(MAXY-MINY+1);i++)
      {
        menuY[i]=menuxye;
        txtY[i]=itemxye;
        txtY[i].IText=(UBYTE *)tt[i+MINY];
        menuY[i].NextItem=&menuY[i+1];
        menuY[i].TopEdge=MENHI*i;
        menuY[i].ItemFill=(APTR)&txtY[i];
        menuY[i].MutualExclude-=(1<<i);
      }
      menuY[MAXY-MINY].NextItem=NULL;
      menuY[newlevel.h-MINY].Flags=MENUFLAGSC;

      for(i=0;i<MAXY;i++)
      {
        menuE[i]=menuxye;
        txtE[i]=itemxye;
        txtE[i].IText=(UBYTE *)tt[i];
        menuE[i].NextItem=&menuE[i+1];
        menuE[i].TopEdge=MENHI*i;
        menuE[i].ItemFill=(APTR)&txtE[i];
        menuE[i].MutualExclude-=(1<<i);
      }
      menuE[MAXY-1].NextItem=NULL;
      menuE[newlevel.h-1].NextItem=NULL;
      menuE[newlevel.e].Flags=MENUFLAGSC;

      for(i=0;i<(MAXCOLORS-1);i++)
      {
        menuC[i]=menuxye;
        txtC[i]=itemxye;
        txtC[i].IText=(UBYTE *)tt[i+2];
        menuC[i].NextItem=&menuC[i+1];
        menuC[i].TopEdge=MENHI*i;
        menuC[i].ItemFill=(APTR)&txtC[i];
        menuC[i].MutualExclude-=(1<<i);
      }
      menuC[MAXCOLORS-2].NextItem=NULL;
      menuC[newlevel.c-2].Flags=MENUFLAGSC;

      SetMenuStrip(Window,&FIRSTMENU);
    }
  }
}

void clearwin()
{
/*
  SetAPen(rp, 0);
  RectFill(rp,4,12,Window->Width-5,Window->Height-3);
*/
}

void close_screen()
{
  if(Window!=NULL)
  {
    ClearMenuStrip(Window);
    CloseWindow(Window);
  }
  else
    printf("Couldn't open window!!!\n");

  if(Screen!=NULL)
    CloseScreen(Screen);
  else
    printf("Couldn't open screen!!!\n");
}

void open_iWindow()
{
  if((iWindow=(struct Window *) OpenWindowTagList(NULL,InfoWindowItem))==NULL)
    error=TRUE;
  else
    irp=iWindow->RPort;
}

void close_iWindow()
{
  if(iWindow!=NULL)
    CloseWindow(iWindow);
  else
    printf("Couldn't open window!!!\n");

  iWindow=NULL;
}

void open_xWindow()
{
  if((iWindow=(struct Window *) OpenWindowTagList(NULL,ResultWindowItem))==NULL)
    error=TRUE;
  else
    irp=iWindow->RPort;
}

void close_xWindow()
{
  if(iWindow!=NULL)
    CloseWindow(iWindow);
  else
    printf("Couldn't open window!!!\n");

  iWindow=NULL;
}
