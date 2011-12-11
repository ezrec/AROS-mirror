#ifdef __AROS__

#include <libraries/mui.h>

#include <proto/alib.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>

Object *DoSuperNew(struct IClass *cl,Object *obj,Tag tag1,...);

#else

#include <clib/alib_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>

#endif

#include <stdlib.h>
#include "common.h"

#include <stdio.h>
 
extern struct MUI_CustomClass *MUI_Field_Class;
extern ULONG pix_x;

struct MUI_CustomClass *MUI_Board_Class;

struct Data
{
Object *field[64];
Object *texta[8],*text1[8];
APTR font;
};

static IPTR mNew(struct IClass *cl,Object *obj,struct opSet *msg)
{
obj=DoSuperNew(cl,obj,
	MUIA_Group_Columns,9,
	MUIA_Group_Spacing,0,
TAG_MORE,msg->ops_AttrList);
if(obj)
	{
	struct Data *data=(struct Data *)INST_DATA(cl,obj);
	ULONG i;
	for(i=0;i<64;i++)
		{
		if(i%8==0) DoMethod(obj,OM_ADDMEMBER,data->text1[i/8]=TextObject,MUIA_FixWidthTxt,"W",MUIA_Font,MUIV_Font_Big,MUIA_Text_PreParse,MUIX_B,End);
		DoMethod(obj,OM_ADDMEMBER,data->field[i]=NewObject(MUI_Field_Class->mcc_Class,0,MUIA_UserData,i,TAG_END));
		}
	DoMethod(obj,OM_ADDMEMBER,HVSpace);
	for(i=0;i<8;i++) DoMethod(obj,OM_ADDMEMBER,data->texta[i]=TextObject,MUIA_Font,MUIV_Font_Big,MUIA_Text_PreParse,"\033c\033b",MUIA_FixWidthTxt,"W",End);
	}
return (IPTR)obj;
}

static IPTR mDispose(struct IClass *cl,Object *obj,Msg msg)
{
return DoSuperMethodA(cl,obj,msg);
}

static void	mChessShowBoard(struct IClass *cl,Object *obj)
{
struct Data *data=(struct Data *)INST_DATA(cl,obj);
char t[2];
int i;
t[1]=0;
for(i=0;i<8;i++)
	{
	if(flags&REVERSEBOARD) t[0]='h'-i;
	else t[0]='a'+i;
	SetAttrs(data->texta[i],MUIA_Text_Contents,t,TAG_END);
	if(flags&REVERSEBOARD) t[0]='1'+i;
	else t[0]='8'-i;
	SetAttrs(data->text1[i],MUIA_Text_Contents,t,TAG_END);
	}
/*
for(i=white;i<=black;i++)
	{
	for(j=pawn;j<=king;j++)
		{
		BitBoard *b=&board.b[i][j];
#ifdef HAVE64BIT
		printf("%08x%08x\n",*b);
#else
		printf("%08x%08x\n",b->hi,b->lo);
#endif
		}
	}
puts("");
*/
MUI_Redraw(obj,MADF_DRAWOBJECT);
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

ULONG retval = 0;
switch(msg->MethodID)
	{
	case OM_NEW:
		retval=mNew(cl,obj,(struct opSet *)msg);
		break;
	case OM_DISPOSE:
		retval=mDispose(cl,obj,msg);
		break;
	case MUIM_Chess_ShowBoard:
		mChessShowBoard(cl,obj);
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
void INIT_6_MUI_Board_Class()
{
if(!(MUI_Board_Class=MUI_CreateCustomClass(0,MUIC_Group,0,sizeof(struct Data),(APTR)Dispatcher))) exit(20);
}

#ifdef __cplusplus
extern "C"
#endif
void EXIT_6_MUI_Board_Class()
{
if(MUI_Board_Class) MUI_DeleteCustomClass(MUI_Board_Class);
}

