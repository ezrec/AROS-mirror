WORD SharedBordersPairs0[] = {
  0,0,0,13,1,13,1,0,73,0,0,0 };
WORD SharedBordersPairs1[] = {
  1,13,73,13,73,1,74,0,74,13 };
WORD SharedBordersPairs2[] = {
  0,0,0,14,1,14,1,0,27,0,0,0 };
WORD SharedBordersPairs3[] = {
  1,14,27,14,27,1,28,0,28,14 };

struct Border SharedBorders[] = {
  {0,0,1,1,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[1]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs0[0],NULL},
  {0,0,1,1,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[3]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs2[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[5]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs2[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[7]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs0[0],NULL} };

struct IntuiText Gadget35_text = {
  1,0,JAM1,10,3,NULL,(UBYTE *)"Default",NULL };

#define Gadget35_ID	35

struct Gadget Gadget35 = {
  NULL,26,75,75,14,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Gadget35_text,0L,NULL,Gadget35_ID,NULL };

struct IntuiText Gadget34_text = {
  1,0,JAM1,22,3,NULL,(UBYTE *)"Save",NULL };

#define Gadget34_ID	34

struct Gadget Gadget34 = {
  &Gadget35,26,55,75,14,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Gadget34_text,0L,NULL,Gadget34_ID,NULL };

struct Gadget g[] = {
  {&g[1],200,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,1,NULL},

  {&g[2],234,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,2,NULL},

  {&g[3],268,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,3,NULL},

  {&g[4],200,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,4,NULL},

  {&g[5],234,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,5,NULL},

  {&g[6],268,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,6,NULL},

  {&g[7],131,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,7,NULL},

  {&g[8],166,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,8,NULL},

  {&g[9],200,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,9,NULL},

  {&g[10],234,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,10,NULL},

  {&g[11],268,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,11,NULL},

  {&g[12],302,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,12,NULL},

  {&g[13],337,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,13,NULL},

  {&g[14],131,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,14,NULL},

  {&g[15],166,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,15,NULL},

  {&g[16],200,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,16,NULL},

  {&g[17],234,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,17,NULL},

  {&g[18],268,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,18,NULL},

  {&g[19],302,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,19,NULL},

  {&g[20],337,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,20,NULL},

  {&g[21],131,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,21,NULL},

  {&g[22],166,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,22,NULL},

  {&g[23],200,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,23,NULL},

  {&g[24],234,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,24,NULL},

  {&g[25],268,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,25,NULL},

  {&g[26],302,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,26,NULL},

  {&g[27],337,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,27,NULL},

  {&g[28],200,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,28,NULL},

  {&g[29],234,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,29,NULL},

  {&g[30],268,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,30,NULL},

  {&g[31],200,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,31,NULL},

  {&g[32],234,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,32,NULL},

  {&Gadget34,268,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,0L,NULL,33,NULL}};

struct IntuiText load_text = {
  1,0,JAM1,22,3,NULL,(UBYTE *)"Load",NULL };

struct Gadget load = {
  &g[0],26,34,75,14,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &load_text,0L,NULL,0,NULL };

#define FIRSTGADGET	&load

struct NewWindow new_window = {
  1,0,521,195,0,1,
  IDCMP_GADGETUP+IDCMP_MOUSEBUTTONS+IDCMP_CLOSEWINDOW+IDCMP_RAWKEY,
  WFLG_DRAGBAR+WFLG_DEPTHGADGET+WFLG_CLOSEGADGET+WFLG_NOCAREREFRESH+WFLG_SMART_REFRESH+WFLG_ACTIVATE+WFLG_RMBTRAP,
  FIRSTGADGET,NULL,
  (UBYTE *)"    Jump!    Default-Editor",NULL,NULL,
  150,50,640,282,WBENCHSCREEN };

#define NEWWINDOW	&new_window
