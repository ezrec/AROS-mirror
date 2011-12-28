#include "includes.h"
#include "globals.h"
#include "htmlgad.h"
#include "NewGroup.h"

typedef struct opSet * opSetP;
typedef struct opGet * opGetP;

struct NewGroupData
{
	ULONG	NewLeft,NewTop;
	ULONG	NewWidth,NewHeight;
	ULONG	InitialLeft,InitialTop;
	ULONG	ChildWidth,ChildHeight;
};

#ifndef __AROS__
long kprintf(char *,...);
#endif

SAVEDS IPTR NewGroupSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct NewGroupData *inst=INST_DATA(cl,obj);
	const struct TagItem *tstate;
	struct TagItem *ti;
  
	ti = ((opSetP)msg)->ops_AttrList;
	tstate = ti;
  
	while (ti = NextTagItem(&tstate)) {
    switch (ti->ti_Tag) {
    	case MUIA_NewGroup_Left: inst->NewLeft=ti->ti_Data; break;
    	case MUIA_NewGroup_Top:  inst->NewTop=ti->ti_Data; break;
    	}
    }
	return DoSuperMethodA(cl, obj, msg);
}

SAVEDS IPTR NewGroupGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct NewGroupData *inst=INST_DATA(cl,obj);
  
	switch (((opGetP)msg)->opg_AttrID) {
		case MUIA_NewGroup_Left:   *(((opGetP)msg)->opg_Storage) = (ULONG)(inst->NewLeft); break;
		case MUIA_NewGroup_Top:    *(((opGetP)msg)->opg_Storage) = (ULONG)(inst->NewTop); break;
		case MUIA_NewGroup_Width:  *(((opGetP)msg)->opg_Storage) = (ULONG)(inst->NewWidth); break;
		case MUIA_NewGroup_Height: *(((opGetP)msg)->opg_Storage) = (ULONG)(inst->NewHeight); break;
	    }
	return DoSuperMethodA(cl, obj, msg);
}

SAVEDS IPTR NewGroupNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct NewGroupData *inst;
	if (!(obj= (Object *)DoSuperMethodA(cl,obj,msg)))
		return 0L;

	inst = INST_DATA(cl, obj);

	inst->NewWidth    = GetTagData(MUIA_NewGroup_Width, 0,((opSetP)msg)->ops_AttrList);
	inst->NewHeight   = GetTagData(MUIA_NewGroup_Height, 0,((opSetP)msg)->ops_AttrList);

	inst->NewLeft     = GetTagData(MUIA_NewGroup_Left, 0x80000000,((opSetP)msg)->ops_AttrList);
	inst->NewTop      = GetTagData(MUIA_NewGroup_Top, 0x80000000,((opSetP)msg)->ops_AttrList);

	inst->ChildWidth  = GetTagData(MUIA_NewGroup_ChildWidth, 0,((opSetP)msg)->ops_AttrList);
	inst->ChildHeight = GetTagData(MUIA_NewGroup_ChildHeight, 0,((opSetP)msg)->ops_AttrList);

	inst->InitialLeft = 0x80000000;
	inst->InitialTop  = 0x80000000;

	return (IPTR)obj;
}

SAVEDS IPTR NewGroupAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct NewGroupData *inst = INST_DATA(cl, obj);

	DoSuperMethodA(cl,obj,(Msg)msg);

	if(inst->NewWidth){
		msg->MinMaxInfo->MinWidth  = inst->NewWidth;
		msg->MinMaxInfo->DefWidth  = inst->NewWidth;
		msg->MinMaxInfo->MaxWidth  = inst->NewWidth;
	}
	if(inst->NewHeight){
		msg->MinMaxInfo->MinHeight = inst->NewHeight;
		msg->MinMaxInfo->DefHeight = inst->NewHeight;
		msg->MinMaxInfo->MaxHeight = inst->NewHeight;
	}

	if(inst->ChildWidth){
		msg->MinMaxInfo->MinWidth  = msg->MinMaxInfo->DefWidth;
		msg->MinMaxInfo->MaxWidth  = msg->MinMaxInfo->DefWidth;
		inst->NewWidth = msg->MinMaxInfo->DefWidth;
		}
	if(inst->ChildHeight){
		msg->MinMaxInfo->MinHeight = msg->MinMaxInfo->DefHeight;
		msg->MinMaxInfo->MaxHeight = msg->MinMaxInfo->DefHeight;
		inst->NewHeight = msg->MinMaxInfo->DefHeight;
		}
	

	return(0);
}

SAVEDS IPTR NewGroupPlaceObject(struct IClass *cl,Object *obj,struct MUIP_PlaceObject *msg)
{
	struct NewGroupData *inst = INST_DATA(cl, obj);
	ULONG res,old_x=0,old_y=0;

	if(MUIMasterBase->lib_Version<11)
		{
//		kprintf("Obj: left: %ld top: %ld\n",inst->NewLeft,inst->NewTop);


	inst->NewWidth=msg->width;
	inst->NewHeight=msg->height;
	if(inst->NewLeft!=0x80000000){
		old_x=msg->x;
		if(inst->InitialLeft==0x80000000){
//			inst->InitialLeft=msg->x;
			msg->x=inst->NewLeft;
			}
		else{
			msg->x=inst->NewLeft+msg->x-inst->InitialLeft;
			}
		}
	if(inst->NewTop!=0x80000000){
		old_y=msg->y;
		if(inst->InitialTop==0x80000000){
//			inst->InitialTop=msg->y;
			msg->y=inst->NewTop;
			}
		else{
			msg->y=inst->NewTop+msg->y-inst->InitialTop;
			}
		}

	res=DoSuperMethodA(cl,obj,(Msg)msg);

	if(inst->NewLeft!=0x80000000)
		msg->x=old_x;
	if(inst->NewTop!=0x80000000)
		msg->y=old_y;
	return res;
		}
	else
		return DoSuperMethodA(cl,obj,(Msg)msg);
}

SAVEDS IPTR NewGroupLayout(struct IClass *cl,Object *obj,struct MUIP_PlaceObject *msg)
{
	struct NewGroupData *inst = INST_DATA(cl, obj);

	if(inst->NewLeft!=0x80000000 && inst->NewTop!=0x80000000){
		inst->NewWidth=_defwidth(obj);
		inst->NewHeight=_defheight(obj);
		MUI_Layout(obj,inst->NewLeft,inst->NewTop,_defwidth(obj),_defheight(obj),0);
//		MUI_Layout(obj,0,0,_defwidth(obj),_defheight(obj),0);
		}
//	kprintf("Placing object at x: %ld y: %ld width: %ld height: %ld\n",inst->NewLeft,inst->NewTop,_defwidth(obj),_defheight(obj));


	return TRUE;
}

DISPATCHER(NewGroupDispatcher)
{
	switch (msg->MethodID){
		case OM_NEW              : return(NewGroupNew        (cl,obj,(APTR)msg));
		case MUIM_AskMinMax      : return(NewGroupAskMinMax  (cl,obj,(APTR)msg));
		case MUIM_PlaceObject    : return(NewGroupPlaceObject(cl,obj,(APTR)msg));
		case OM_SET			     : return(NewGroupSet        (cl,obj,(APTR)msg));
		case OM_GET			     : return(NewGroupGet        (cl,obj,(APTR)msg));
		case MUIM_NewGroup_Layout: return(NewGroupLayout     (cl,obj,(APTR)msg));
	}
	return DoSuperMethodA(cl,obj,msg);
}

struct MUI_CustomClass *NewGroupClInit(void)
{
  struct MUI_CustomClass *cl;

  /* Create the HTML gadget class */
  if (!(cl = MUI_CreateCustomClass(NULL,MUIC_Group,NULL,
			      sizeof(struct NewGroupData),ENTRY(NewGroupDispatcher))))
    fail(NULL, "Failed to create New Group class."); /* Failed, cleanup */
  return cl;
}

BOOL NewGroupClFree(struct MUI_CustomClass *mcc)
{
  MUI_DeleteCustomClass(mcc);
  return 0;
}
