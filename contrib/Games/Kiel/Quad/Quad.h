WORD SharedBordersPairs0[] = {
  0,0,0,17,1,16,1,0,90,0,0,0 };
WORD SharedBordersPairs1[] = {
  0,17,90,17,90,1,91,0,91,17 };
WORD SharedBordersPairs2[] = {
  0,0,0,25,1,25,1,0,47,0,0,0 };
WORD SharedBordersPairs3[] = {
  1,25,47,25,47,1,48,0,48,25 };

struct Border SharedBorders[] = {
  {0,0,1,1,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[1]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs0[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs1[0],&SharedBorders[3]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs0[0],NULL},
  {0,0,1,1,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[5]},
  {0,0,2,2,JAM1,6,&SharedBordersPairs2[0],NULL},
  {0,0,2,2,JAM1,5,&SharedBordersPairs3[0],&SharedBorders[7]},
  {0,0,1,1,JAM1,6,&SharedBordersPairs2[0],NULL} };

#define target9_ID	19

struct Gadget target9 = {
  NULL,441,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target9_ID,NULL };

#define target8_ID	18

struct Gadget target8 = {
  &target9,389,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target8_ID,NULL };

#define target7_ID	17

struct Gadget target7 = {
  &target8,337,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target7_ID,NULL };

#define target6_ID	16

struct Gadget target6 = {
  &target7,441,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target6_ID,NULL };

#define target5_ID	15

struct Gadget target5 = {
  &target6,389,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target5_ID,NULL };

#define target4_ID	14

struct Gadget target4 = {
  &target5,337,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target4_ID,NULL };

#define target3_ID	13

struct Gadget target3 = {
  &target4,441,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target3_ID,NULL };

#define target2_ID	12

struct Gadget target2 = {
  &target3,389,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target2_ID,NULL };

#define target1_ID	11

struct Gadget target1 = {
  &target2,337,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,target1_ID,NULL };

char stoptext[5]="Stop";
struct IntuiText Stop_text = {
  1,0,JAM1,27,5,NULL,(UBYTE *)stoptext,NULL };

#define Stop_ID		10

struct Gadget Stop = {
  &target1,226,67,92,18,
  GFLG_GADGHIMAGE+GFLG_DISABLED,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Stop_text,0L,NULL,Stop_ID,NULL };

#define initial9_ID	9

struct Gadget initial9 = {
  &Stop,151,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial9_ID,NULL };

#define initial8_ID	8

struct Gadget initial8 = {
  &initial9,99,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial8_ID,NULL };

#define initial7_ID	7

struct Gadget initial7 = {
  &initial8,47,86,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial7_ID,NULL };

#define initial6_ID	6

struct Gadget initial6 = {
  &initial7,151,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial6_ID,NULL };

#define initial5_ID	5

struct Gadget initial5 = {
  &initial6,99,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial5_ID,NULL };

#define initial4_ID	4

struct Gadget initial4 = {
  &initial5,47,56,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial4_ID,NULL };

#define initial3_ID	3

struct Gadget initial3 = {
  &initial4,151,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial3_ID,NULL };

#define initial2_ID	2

struct Gadget initial2 = {
  &initial3,99,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial2_ID,NULL };

#define initial1_ID	1

struct Gadget initial1 = {
  &initial2,47,26,49,26,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[4],(APTR)&SharedBorders[6],
  NULL,0L,NULL,initial1_ID,NULL };

char starttext[6]="Start";
struct IntuiText Start_text = {
  1,0,JAM1,24,5,NULL,(UBYTE *)starttext,NULL };

#define Start_ID	0

struct Gadget Start = {
  &initial1,225,45,92,18,
  GFLG_GADGHIMAGE,
  GACT_RELVERIFY,
  GTYP_BOOLGADGET,
  (APTR)&SharedBorders[0],(APTR)&SharedBorders[2],
  &Start_text,0L,NULL,Start_ID,NULL };

#define FIRSTGADGET	&Start
