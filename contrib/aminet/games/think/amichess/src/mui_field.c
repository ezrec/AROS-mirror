#ifdef __AROS__
#include <libraries/mui.h>
#include <aros/asmcall.h>
#include <aros/debug.h>

#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include <hardware/blit.h>

Object *DoSuperNew(struct IClass *cl,Object *obj,ULONG tag1,...);

#else

#include <clib/alib_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <clib/utility_protos.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

void PlaySound(Object *snd,BOOL);
void MoveSound(char *);

extern ULONG pix_x,col_white,col_black;
extern struct BitMap *pieces_bm;
extern PLANEPTR pieces_mask;
extern Object *board_light;
extern struct BitMap *board_light_bm;
extern Object *board_dark;
extern struct BitMap *board_dark_bm;
extern Object *snd_move;
extern ULONG promotepiece;

struct Data
{
int num;
};

struct MUI_CustomClass *MUI_Field_Class;

static char bw[64]={
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0
};

static char nboard[64]={
56,57,58,59,60,61,62,63,
48,49,50,51,52,53,54,55,
40,41,42,43,44,45,46,47,
32,33,34,35,36,37,38,39,
24,25,26,27,28,29,30,31,
16,17,18,19,20,21,22,23,
8,9,10,11,12,13,14,15,
0,1,2,3,4,5,6,7
};

static char rboard[64]={
7,6,5,4,3,2,1,0,
15,14,13,12,11,10,9,8,
23,22,21,20,19,18,17,16,
31,30,29,28,27,26,25,24,
39,38,37,36,35,34,33,32,
47,46,45,44,43,42,41,40,
55,54,53,52,51,50,49,48,
63,62,61,60,59,58,57,56
};

static ULONG mFieldNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
obj=DoSuperNew(cl,obj,
	MUIA_FixWidth,pix_x,
	MUIA_FixHeight,pix_x,
	MUIA_Draggable,1,
	MUIA_Dropable,1,
#ifdef __AROS__
#warning "Drag and drop not working in Zune here if inputmode is <none>!"
    	MUIA_InputMode, MUIV_InputMode_RelVerify,
#endif
TAG_MORE,msg->ops_AttrList);
if(obj)
	{
	struct Data *data=(struct Data *)INST_DATA(cl,obj);
	data->num=GetTagData(MUIA_UserData,0,msg->ops_AttrList);
	}
return(ULONG)obj;
}

static ULONG mFieldAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
struct MUI_MinMax *mm;
DoSuperMethodA(cl,obj,(Msg)msg);
mm=msg->MinMaxInfo;
mm->MinWidth+=pix_x;
mm->MinHeight+=pix_x;
mm->MaxWidth+=pix_x;
mm->MaxHeight+=pix_x;
mm->DefWidth+=pix_x;
mm->DefHeight+=pix_x;
return 0;
}

static ULONG mFieldDragQuery(struct IClass *cl,Object *obj,struct MUIP_DragQuery *msg)
{
ULONG from,to,retval=MUIV_DragQuery_Refuse;
GetAttr(MUIA_UserData,msg->obj,&from);
GetAttr(MUIA_UserData,obj,&to);
from=flags&REVERSEBOARD?rboard[from]:nboard[from];
to=flags&REVERSEBOARD?rboard[to]:nboard[to];

if(from!=to)
	{
	int side=board.side;
	if(board.b[side][pawn]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	else if(board.b[side][knight]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	else if(board.b[side][bishop]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	else if(board.b[side][rook]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	else if(board.b[side][queen]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	else if(board.b[side][king]&BitPosArray[from]) retval=MUIV_DragQuery_Accept;
	if(retval==MUIV_DragQuery_Accept)
		{
		if(board.b[side][pawn]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		else if(board.b[side][knight]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		else if(board.b[side][bishop]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		else if(board.b[side][rook]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		else if(board.b[side][queen]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		else if(board.b[side][king]&BitPosArray[to]) retval=MUIV_DragQuery_Refuse;
		}
	}
return retval;
}

static ULONG mFieldDragDrop(struct IClass *cl,Object *obj,struct MUIP_DragDrop *msg)
{
int side=board.side;
ULONG from,to;
leaf *ptr;
char cmd[6];
GetAttr(MUIA_UserData,msg->obj,&from);
GetAttr(MUIA_UserData,obj,&to);
from=flags&REVERSEBOARD?rboard[from]:nboard[from];
to=flags&REVERSEBOARD?rboard[to]:nboard[to];
cmd[0]='a'+(from%8);
cmd[1]='1'+(from/8);
cmd[2]='a'+(to%8);
cmd[3]='1'+(to/8);
cmd[4]=0;
if((side==white&&cmd[1]=='7'&&cmd[3]=='8')&&(board.b[side][pawn]&BitPosArray[from])||(cmd[1]=='2'&&cmd[3]=='1')&&board.b[side][pawn]&BitPosArray[from])
	{
	promotepiece=0;
	DoMethod(_app(obj),MUIM_Chess_Promote);
	switch(promotepiece)
		{
		case 0:
			strcat(cmd,"Q");
			break;
		case 1:
			strcat(cmd,"R");
			break;
		case 2:
			strcat(cmd,"B");
			break;
		case 3:
			strcat(cmd,"N");
		}
	}
if(ptr=ValidateMove(cmd))
	{
	char text[50];
/*	DoMethod(_app(obj),MUIM_Chess_Side); */
	SANMove(ptr->move,1);
	MakeMove(board.side,&ptr->move);
	strcpy(Game[GameCnt].SANmv,SANmv);
	sprintf(text,"%d: %-5s",GameCnt/2+1,SANmv);
	DoMethod(_app(obj),MUIM_Chess_AddMove,1^board.side,text);
	DoMethod(_app(obj),MUIM_Chess_ShowBoard);
	DoMethod(_app(obj),MUIM_Chess_Side);
	MoveSound(SANmv);
	if(!(flags&SUPERVISOR))
		{
		DoMethod(_app(obj),MUIM_Chess_Side);
		Iterate();
		if(flags&ENDED) DoMethod(_app(obj),MUIM_Chess_ClearFlag,AUTOPLAY);
		}
	}
else DisplayBeep(0);
return 0;
}

static ULONG mFieldDraw(struct IClass *cl,Object *obj,Msg msg)
{
long sq,x;
struct Data *data=(struct Data *)INST_DATA(cl,obj);
struct RastPort *rp=_rp(obj);
sq=flags&REVERSEBOARD?rboard[data->num]:nboard[data->num];
if(board_light_bm&&board_dark_bm) BltBitMapRastPort(bw[sq]?board_light_bm:board_dark_bm,0,0,rp,_left(obj),_top(obj),_width(obj),_height(obj),0xC0);
else
	{
	SetAPen(rp,bw[sq]?col_white:col_black);
	RectFill(rp,_left(obj),_top(obj),_right(obj),_bottom(obj));
	}
if(board.b[white][pawn]&BitPosArray[sq])        x=6;
else if(board.b[white][knight]&BitPosArray[sq]) x=8;
else if(board.b[white][bishop]&BitPosArray[sq]) x=9;
else if(board.b[white][rook]&BitPosArray[sq])   x=7;
else if(board.b[white][queen]&BitPosArray[sq])  x=10;
else if(board.b[white][king]&BitPosArray[sq])   x=11;
else if(board.b[black][pawn]&BitPosArray[sq])   x=0;
else if(board.b[black][knight]&BitPosArray[sq]) x=2;
else if(board.b[black][bishop]&BitPosArray[sq]) x=3;
else if(board.b[black][rook]&BitPosArray[sq])   x=1;
else if(board.b[black][queen]&BitPosArray[sq])  x=4;
else if(board.b[black][king]&BitPosArray[sq])   x=5;
else x=-1;
if(x>=0)
{
    if (pieces_mask)
    {
    	BltMaskBitMapRastPort(pieces_bm,x*pix_x,0,rp,_left(obj),_top(obj)+1,pix_x,pix_x-2,ANBC|ABNC|ABC,pieces_mask);
    }
    else
    {
    	BltBitMapRastPort(pieces_bm,x*pix_x,0,rp,_left(obj),_top(obj)+1,pix_x,pix_x-2,192);
    }
}
return 0;
}

#ifdef __AROS__
AROS_UFH3S(ULONG, Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
    AROS_USERFUNC_INIT
#else
#ifdef __GNUC__
static ULONG Dispatcher(struct IClass *cl __asm("a0"),Object *obj  __asm("a2"),Msg msg  __asm("a1"))
#elif defined __VBCC__
static ULONG Dispatcher(__reg("a0") struct IClass *cl,__reg("a2") Object *obj,__reg("a1") Msg msg)
#else
static ULONG Dispatcher(register __a0 struct IClass *cl,register __a2 Object *obj,register __a1 Msg msg)
#endif
{
#endif

ULONG retval;
switch(msg->MethodID)
	{
	case OM_NEW:
		retval=mFieldNew(cl,obj,(struct opSet *)msg);
		break;
	case MUIM_AskMinMax:
		retval=mFieldAskMinMax(cl,obj,(struct MUIP_AskMinMax *)msg);
		break;
	case MUIM_DragQuery:
		retval=mFieldDragQuery(cl,obj,(struct MUIP_DragQuery *)msg);
		break;
	case MUIM_DragDrop:
		retval=mFieldDragDrop(cl,obj,(struct MUIP_DragDrop *)msg);
		break;
	case MUIM_Draw:
		retval=mFieldDraw(cl,obj,msg);
		break;
	default:
		retval=DoSuperMethodA(cl,obj,msg);
	}
return retval;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __cplusplus
extern "C"
#endif
void INIT_6_MUI_Field_Class()
{
if(!(MUI_Field_Class=MUI_CreateCustomClass(0,MUIC_Area,0,sizeof(struct Data),(APTR)Dispatcher))) exit(20);
}

#ifdef __cplusplus
extern "C"
#endif
void EXIT_6_MUI_Field_Class()
{
if(MUI_Field_Class) MUI_DeleteCustomClass(MUI_Field_Class);
}
