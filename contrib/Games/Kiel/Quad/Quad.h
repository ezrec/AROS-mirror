SHORT SharedBordersPairs0[] = {
  0,0,0,17,1,16,1,0,90,0,0,0 };
SHORT SharedBordersPairs1[] = {
  0,17,90,17,90,1,91,0,91,17 };
SHORT SharedBordersPairs2[] = {
  0,0,0,25,1,25,1,0,47,0,0,0 };
SHORT SharedBordersPairs3[] = {
  1,25,47,25,47,1,48,0,48,25 };

struct Border SharedBorders[] = {
  0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[1],
  0,0,2,2,JAM1,6,(SHORT *)&SharedBordersPairs0[0],NULL,
  0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs1[0],&SharedBorders[3],
  0,0,1,1,JAM1,6,(SHORT *)&SharedBordersPairs0[0],NULL,
  0,0,1,1,JAM1,5,(SHORT *)&SharedBordersPairs3[0],&SharedBorders[5],
  0,0,2,2,JAM1,6,(SHORT *)&SharedBordersPairs2[0],NULL,
  0,0,2,2,JAM1,5,(SHORT *)&SharedBordersPairs3[0],&SharedBorders[7],
  0,0,1,1,JAM1,6,(SHORT *)&SharedBordersPairs2[0],NULL };

#define Ziel9_ID    19

struct Gadget Ziel9 = {
  NULL,441,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel9_ID,NULL };

#define Ziel8_ID    18

struct Gadget Ziel8 = {
  &Ziel9,389,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel8_ID,NULL };

#define Ziel7_ID    17

struct Gadget Ziel7 = {
  &Ziel8,337,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel7_ID,NULL };

#define Ziel6_ID    16

struct Gadget Ziel6 = {
  &Ziel7,441,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel6_ID,NULL };

#define Ziel5_ID    15

struct Gadget Ziel5 = {
  &Ziel6,389,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel5_ID,NULL };

#define Ziel4_ID    14

struct Gadget Ziel4 = {
  &Ziel5,337,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel4_ID,NULL };

#define Ziel3_ID    13

struct Gadget Ziel3 = {
  &Ziel4,441,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel3_ID,NULL };

#define Ziel2_ID    12

struct Gadget Ziel2 = {
  &Ziel3,389,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel2_ID,NULL };

#define Ziel1_ID    11

struct Gadget Ziel1 = {
  &Ziel2,337,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Ziel1_ID,NULL };

char stoptext[5]="----";
struct IntuiText Stop_text = {
  1,0,JAM1,27,13,NULL,(UBYTE *)stoptext,NULL };

#define Stop_ID    10

struct Gadget Stop = {
  &Ziel1,226,57,92,18,
  GFLG_GADGHIMAGE+GFLG_DISABLED,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Stop_text,NULL,NULL,Stop_ID,NULL };

#define Anfang9_ID    9

struct Gadget Anfang9 = {
  &Stop,151,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang9_ID,NULL };

#define Anfang8_ID    8

struct Gadget Anfang8 = {
  &Anfang9,99,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang8_ID,NULL };

#define Anfang7_ID    7

struct Gadget Anfang7 = {
  &Anfang8,47,76,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang7_ID,NULL };

#define Anfang6_ID    6

struct Gadget Anfang6 = {
  &Anfang7,151,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang6_ID,NULL };

#define Anfang5_ID    5

struct Gadget Anfang5 = {
  &Anfang6,99,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang5_ID,NULL };

#define Anfang4_ID    4

struct Gadget Anfang4 = {
  &Anfang5,47,46,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang4_ID,NULL };

#define Anfang3_ID    3

struct Gadget Anfang3 = {
  &Anfang4,151,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang3_ID,NULL };

#define Anfang2_ID    2

struct Gadget Anfang2 = {
  &Anfang3,99,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang2_ID,NULL };

#define Anfang1_ID    1

struct Gadget Anfang1 = {
  &Anfang2,47,16,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,NULL,NULL,Anfang1_ID,NULL };

char starttext[6]="Start";
struct IntuiText Start_text = {
  1,0,JAM1,24,13,NULL,(UBYTE *)starttext,NULL };

#define Start_ID    0

struct Gadget Start = {
  &Anfang1,225,35,92,18,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Start_text,NULL,NULL,Start_ID,NULL };

#define WDBACKFILL  0
#define FIRSTGADGET &Start
