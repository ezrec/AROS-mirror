WORD SharedBordersPairs0[] = {
  0,0,0,23,1,23,1,0,62,0,0,0 };
WORD SharedBordersPairs1[] = {
  1,23,62,23,62,1,63,0,63,23 };
WORD SharedBordersPairs2[] = {
  0,0,0,24,1,24,1,0,27,0,0,0 };
WORD SharedBordersPairs3[] = {
  1,24,27,24,27,1,28,0,28,24 };

struct Border SharedBorders[] = {
  {0,0,1,1,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[1]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs0[0],NULL},
  {0,0,1,1,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[3]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs2[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[5]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs2[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[7]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs0[0],NULL} };

struct IntuiText Gadget34_text = {
  1,0,JAM1,17,5,NULL,(UBYTE *)"Back",NULL };

#define Gadget34_ID	34

struct Gadget Gadget34 = {
  NULL,21,58,64,24,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Gadget34_text,0L,NULL,Gadget34_ID,NULL };

struct Gadget g[] = {
  {&g[1],100,34,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,1,NULL},

  {&g[2],134,34,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,2,NULL},

  {&g[3],168,34,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,3,NULL},

  {&g[4],100,62,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,4,NULL},

  {&g[5],134,62,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,5,NULL},

  {&g[6],168,62,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,6,NULL},

  {&g[7],31,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,7,NULL},

  {&g[8],66,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,8,NULL},

  {&g[9],100,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,9,NULL},

  {&g[10],134,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,10,NULL},

  {&g[11],168,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,11,NULL},

  {&g[12],202,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,12,NULL},

  {&g[13],237,90,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,13,NULL},

  {&g[14],31,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,14,NULL},

  {&g[15],66,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,15,NULL},

  {&g[16],100,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,16,NULL},

  {&g[17],134,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,17,NULL},

  {&g[18],168,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,18,NULL},

  {&g[19],202,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,19,NULL},

  {&g[20],237,119,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,20,NULL},

  {&g[21],31,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,21,NULL},

  {&g[22],66,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,22,NULL},

  {&g[23],100,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,23,NULL},

  {&g[24],134,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,24,NULL},

  {&g[25],168,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,25,NULL},

  {&g[26],202,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,26,NULL},

  {&g[27],237,148,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,27,NULL},

  {&g[28],100,176,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,28,NULL},

  {&g[29],134,176,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,29,NULL},

  {&g[30],168,176,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,30,NULL},

  {&g[31],100,204,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,31,NULL},

  {&g[32],134,204,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,32,NULL},

  {&Gadget34,168,204,29,25,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,33,NULL}
  };

struct IntuiText Start_text = {
  1,0,JAM1,11,6,NULL,(UBYTE *)"Restart",NULL };

#define Start_ID	0

struct Gadget Start = {
  &g[0],21,29,64,24,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Start_text,0L,NULL,Start_ID,NULL };

#define FIRSTGADGET	&Start

struct NewWindow new_window = {
  0,0,300,250,0,1,
  IDCMP_GADGETUP+IDCMP_MOUSEBUTTONS+IDCMP_CLOSEWINDOW+IDCMP_RAWKEY,
  WFLG_DRAGBAR+WFLG_DEPTHGADGET+WFLG_CLOSEGADGET+WFLG_NOCAREREFRESH+WFLG_SMART_REFRESH+WFLG_ACTIVATE+WFLG_RMBTRAP,
  FIRSTGADGET,NULL,
  (UBYTE *)"Jump!",NULL,NULL,
  150,50,640,282,WBENCHSCREEN };

#define NEWWINDOW	&new_window
