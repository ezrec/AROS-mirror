SHORT SharedBordersPairs0[] = {
  0,0,0,13,1,13,1,0,73,0,0,0 };
SHORT SharedBordersPairs1[] = {
  1,13,73,13,73,1,74,0,74,13 };
SHORT SharedBordersPairs2[] = {
  0,0,0,14,1,14,1,0,27,0,0,0 };
SHORT SharedBordersPairs3[] = {
  1,14,27,14,27,1,28,0,28,14 };

struct Border SharedBorders[] = {
  0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[1],
  0,0,2,2,JAM1,6,(SHORT *)&SharedBordersPairs0[0],NULL,
  0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs3[0],&SharedBorders[3],
  0,0,2,2,JAM1,6,(SHORT *)&SharedBordersPairs2[0],NULL,
  0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs3[0],&SharedBorders[5],
  0,0,1,1,JAM1,6,(SHORT *)&SharedBordersPairs2[0],NULL,
  0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[7],
  0,0,1,1,JAM1,6,(SHORT *)&SharedBordersPairs0[0],NULL};

struct IntuiText Gadget34_text = {
  1,0,JAM1,27,12,NULL,(UBYTE *)"<-",NULL };

#define Gadget34_ID    34

struct Gadget Gadget34 = {
  NULL,26,55,75,14,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Gadget34_text,NULL,NULL,Gadget34_ID,NULL };

#define g33_ID    33

struct Gadget g33 = {
  &Gadget34,268,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g33_ID,NULL };

#define g32_ID    32

struct Gadget g32 = {
  &g33,234,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g32_ID,NULL };

#define g31_ID    31

struct Gadget g31 = {
  &g32,200,144,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g31_ID,NULL };

#define g30_ID    30

struct Gadget g30 = {
  &g31,268,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g30_ID,NULL };

#define g29_ID    29

struct Gadget g29 = {
  &g30,234,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g29_ID,NULL };

#define g28_ID    28

struct Gadget g28 = {
  &g29,200,126,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g28_ID,NULL };

#define g27_ID    27

struct Gadget g27 = {
  &g28,337,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g27_ID,NULL };

#define g26_ID    26

struct Gadget g26 = {
  &g27,302,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g26_ID,NULL };

#define g25_ID    25

struct Gadget g25 = {
  &g26,268,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g25_ID,NULL };

#define g24_ID    24

struct Gadget g24 = {
  &g25,234,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g24_ID,NULL };

#define g23_ID    23

struct Gadget g23 = {
  &g24,200,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g23_ID,NULL };

#define g22_ID    22

struct Gadget g22 = {
  &g23,166,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g22_ID,NULL };

#define g21_ID    21

struct Gadget g21 = {
  &g22,131,108,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g21_ID,NULL };

#define g20_ID    20

struct Gadget g20 = {
  &g21,337,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g20_ID,NULL };

#define g19_ID    19

struct Gadget g19 = {
  &g20,302,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g19_ID,NULL };

#define g18_ID    18

struct Gadget g18 = {
  &g19,268,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g18_ID,NULL };

#define g17_ID    17

struct Gadget g17 = {
  &g18,234,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g17_ID,NULL };

#define g16_ID    16

struct Gadget g16 = {
  &g17,200,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g16_ID,NULL };

#define g15_ID    15

struct Gadget g15 = {
  &g16,166,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g15_ID,NULL };

#define g14_ID    14

struct Gadget g14 = {
  &g15,131,89,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g14_ID,NULL };

#define g13_ID    13

struct Gadget g13 = {
  &g14,337,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g13_ID,NULL };

#define g12_ID    12

struct Gadget g12 = {
  &g13,302,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g12_ID,NULL };

#define g11_ID    11

struct Gadget g11 = {
  &g12,268,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g11_ID,NULL };

#define g10_ID    10

struct Gadget g10 = {
  &g11,234,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g10_ID,NULL };

#define g9_ID    9

struct Gadget g9 = {
  &g10,200,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g9_ID,NULL };

#define g8_ID    8

struct Gadget g8 = {
  &g9,166,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g8_ID,NULL };

#define g7_ID    7

struct Gadget g7 = {
  &g8,131,70,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g7_ID,NULL };

#define g6_ID    6

struct Gadget g6 = {
  &g7,268,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g6_ID,NULL };

#define g5_ID    5

struct Gadget g5 = {
  &g6,234,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g5_ID,NULL };

#define g4_ID    4

struct Gadget g4 = {
  &g5,200,52,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g4_ID,NULL };

#define g3_ID    3

struct Gadget g3 = {
  &g4,268,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g3_ID,NULL };

#define g2_ID    2

struct Gadget g2 = {
  &g3,234,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g2_ID,NULL };

#define g1_ID    1

struct Gadget g1 = {
  &g2,200,34,29,15,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[2],(APTR)&SharedBorders[4],
  NULL,NULL,NULL,g1_ID,NULL };

struct IntuiText Start_text = {
  1,0,JAM1,25,12,NULL,(UBYTE *)"Neu",NULL };

#define Start_ID    0

struct Gadget Start = {
  &g1,26,34,75,14,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[6],
  &Start_text,NULL,NULL,Start_ID,NULL };

struct NewWindow new_window = {
  1,0,521,195,0,1,
  IDCMP_GADGETUP+IDCMP_GADGETDOWN+IDCMP_CLOSEWINDOW+IDCMP_RAWKEY,
  WFLG_GIMMEZEROZERO,
  &Start,NULL,
  (UBYTE *)"Jump!",NULL,NULL,
  150,50,640,282,WBENCHSCREEN };


#define NEWWINDOW   &new_window
#define WDBACKFILL  0
#define FIRSTGADGET &Start

