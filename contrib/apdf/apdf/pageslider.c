/*************************************************************************\
 *                                                                       *
 * pageslider.c                                                          *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include "AComm.h"
#include "Apdf.h"
#include "document.h"
#include "pageslider.h"

#define DB(x)   //x
#define Static static


/*
 *  Server interface functions.
 */

#if 0
Static void get_page_label(struct comm_info *ci,struct PDFDoc *doc,int index,char *buf,size_t sz) {
    struct msg_pagelabel *p=ci->cmddata;
    sync();
    p->doc=doc;
    p->index=index;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETPAGELABEL);
    strncpy(buf,p->label,sz-1);
    buf[sz-1]='\0';
}
#endif

Static const char **get_page_labels(struct comm_info *ci,struct PDFDoc *doc,size_t *sz) {
    struct msg_pagelabels *p=ci->cmddata;
    const char **r=NULL;
    sync();
    p->doc=doc;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETPAGELABELS);
    *sz=p->size;
    if(p->size) {
	r=(*ci->sv->MyAllocSharedMem)(ci->sv,p->size);
	(*ci->sv->exchange_raw_msg)(ci,r,p->size,MSGID_GETPAGELABELS);
    }
    return r;
}

int get_page_index(struct comm_info *ci,struct PDFDoc *doc,const char *lab) {
    struct msg_pagelabel *p=ci->cmddata;
    sync();
    p->doc=doc;
    strncpy(p->label,lab,sizeof(p->label)-1);
    p->label[sizeof(p->label)-1]='\0';
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETPAGEINDEX);
    return p->index;
}

/*
 * Custom slider class: maintains MYATTR_PageSlider_Value1, that
 * is similar to MUIA_Numeric_Value, except that it is
 * updated only when the slider is released.
 */

static struct MUI_CustomClass* slider1_mcc;

struct SliderData1 {
    struct PDFDoc *pdfdoc;
    struct comm_info *ci;
    Object *strobj;
    LONG value;
    BOOL pressed;
    BOOL lock;
    const char **labels;
    struct MUI_EventHandlerNode ehn;
    char fmt[16];
};
typedef struct SliderData1 SliderData1;


Static ULONG ms1Set(struct IClass *cl,Object *obj,struct opSet *msg) {
    SliderData1 *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_PageSlider_PDFDoc: {
	    size_t size;
	    if(dat->labels) {
		(*dat->ci->sv->MyFreeSharedMem)(dat->ci->sv,dat->labels);
		dat->labels=NULL;
	    }
	    dat->pdfdoc=(struct PDFDoc *)tag->ti_Data;
	    dat->labels=get_page_labels(dat->ci,dat->pdfdoc,&size);
	    break;
	  }
	  case MYATTR_PageSlider_Value1:
	    if(!dat->lock) {
		dat->lock=TRUE;
		set(obj,MUIA_Numeric_Value,tag->ti_Data);
		dat->lock=FALSE;
	    }
	    get(obj,MUIA_Numeric_Value,&dat->value);
	    break;
	  case MUIA_Numeric_Max: {
	    /* Don't use sprintf as we don't want it linked in the plugin. Oh well. */
	    char *p=dat->fmt+sizeof(dat->fmt);
	    ULONG x=tag->ti_Data;
            *--p='\0';
	    do {
		*--p='0'+x%10;
		x/=10;
	    } while(x);
	    *--p='/';
	    *--p='d';
	    *--p='l';
	    *--p='%';
	    set(obj,MUIA_Numeric_Format,p);
	    break;
	  }
	  case MUIA_Numeric_Value: {
	    LONG r=DoSuperMethodA(cl,obj,(Msg)msg);
	    if(!dat->pressed && !dat->lock) {
		dat->lock=TRUE;
		set(obj,MYATTR_PageSlider_Value1,tag->ti_Data);
		dat->lock=FALSE;
	    }
	    get(obj,MUIA_Numeric_Value,&dat->value);
	    break;
	  }
	  case MYATTR_PageSlider_Label:
	    if(tag->ti_Data)
		set(obj,MYATTR_PageSlider_Value1,get_page_index(dat->ci,dat->pdfdoc,(const char*)tag->ti_Data));
	    break;
	}
    }
    return msg->MethodID==OM_NEW?0:DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG ms1New(struct IClass *cl,Object *obj,struct opSet *msg) {
    SliderData1* dat;
    struct comm_info *ci=(APTR)GetTagData(MYATTR_PageSlider_CommInfo,0,msg->ops_AttrList);
    if(!ci)
	return 0;
    if(obj=(Object *)DoSuperMethodA(cl,obj,(APTR)msg)) {
	dat=INST_DATA(cl,obj);
	dat->pressed=FALSE;
	dat->lock=FALSE;
	dat->value=GetTagData(MYATTR_PageSlider_Value1,1,msg->ops_AttrList);
	set(obj,MUIA_Numeric_Value,dat->value);
	dat->pdfdoc=NULL;
	dat->ci=ci;
	dat->strobj=(Object *)GetTagData(MYATTR_PageSlider_StringObj,0,msg->ops_AttrList);
	dat->labels=NULL;
	ms1Set(cl,obj,msg);
    }
    DB(dprintf("ms1New: %p\n", obj));
    return (ULONG)obj;
}

Static ULONG ms1Dispose(struct IClass *cl,Object *obj,Msg msg) {
    SliderData1 *dat=INST_DATA(cl,obj);
    DB(dprintf("ms1Dispose: %p\n", obj));
    if(dat->labels)
	(*dat->ci->sv->MyFreeSharedMem)(dat->ci->sv,dat->labels);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG ms1Get(struct IClass *cl,Object *obj,struct opGet *msg) {
    switch(msg->opg_AttrID) {
      case MYATTR_PageSlider_Value1: {
	SliderData1* dat=INST_DATA(cl,obj);
	*msg->opg_Storage=dat->value;
	return TRUE;
      }
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}


Static ULONG ms1Setup(struct IClass *cl,Object *obj,Msg msg) {
    SliderData1 *dat=INST_DATA(cl,obj);
    if(DoSuperMethodA(cl,obj,msg)) {
	dat->ehn.ehn_Priority=1;
	dat->ehn.ehn_Flags=0;
	dat->ehn.ehn_Object=obj;
	dat->ehn.ehn_Class=NULL;
	dat->ehn.ehn_Events=IDCMP_MOUSEBUTTONS;
	DoMethod3(_win(obj),MUIM_Window_AddEventHandler,&dat->ehn);
	return TRUE;
    } else
	return FALSE;
}


Static ULONG ms1Cleanup(struct IClass *cl,Object *obj,Msg msg) {
    SliderData1 *dat=INST_DATA(cl,obj);
    DoMethod3(_win(obj),MUIM_Window_RemEventHandler,&dat->ehn);
    return DoSuperMethodA(cl,obj,msg);
}


Static ULONG ms1HandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg) {
    #define _between(a,x,b) ((x)>=(a) && (x)<=(b))
    #define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))
    SliderData1* dat=INST_DATA(cl,obj);

    if(msg->imsg) {
	switch(msg->imsg->Class) {
	  case IDCMP_MOUSEBUTTONS:
	    if(msg->imsg->Code==SELECTDOWN) {
		if(_isinobject(msg->imsg->MouseX,msg->imsg->MouseY))
		    dat->pressed=TRUE;
	    } else if(msg->imsg->Code==SELECTUP && dat->pressed) {
		ULONG r;
		LONG x;
		dat->pressed=FALSE;
		get(obj,MUIA_Numeric_Value,&x);
		set(obj,MYATTR_PageSlider_Value1,x);
	    }
	    break;
	}
    }
    return 0;
    #undef _between
    #undef _isinobject
}

ULONG ms1Stringify(struct IClass *cl,Object *obj,struct MUIP_Numeric_Stringify *msg) {
    SliderData1 *dat=INST_DATA(cl,obj);
    const char *str;
    if(!dat->labels) {
	str=(const char *)DoSuperMethodA(cl,obj,(APTR)msg);
	if(dat->strobj)
	    set(dat->strobj,MUIA_String_Integer,msg->value);
    } else {
	str=dat->labels[msg->value-1];
	if(dat->strobj)
	    set(dat->strobj,MUIA_String_Contents,str);
    }
    return (ULONG)str;
}

BEGIN_DISPATCHER(ms1Dispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW          : return ms1New        (cl,obj,(APTR)msg);
      case OM_DISPOSE      : return ms1Dispose    (cl,obj,(APTR)msg);
      case OM_SET          : return ms1Set        (cl,obj,(APTR)msg);
      case OM_GET          : return ms1Get        (cl,obj,(APTR)msg);
      case MUIM_Setup      : return ms1Setup      (cl,obj,(APTR)msg);
      case MUIM_Cleanup    : return ms1Cleanup    (cl,obj,(APTR)msg);
      case MUIM_HandleEvent: return ms1HandleEvent(cl,obj,(APTR)msg);
      case MUIM_Numeric_Stringify: return ms1Stringify(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(ms1Dispatcher)

#define PageSlider1Object NewObject(slider1_mcc->mcc_Class,NULL

struct SliderData {
    Object *sliderobj;
    Object *strobj;
    LONG value;
};
typedef struct SliderData SliderData;


Static ULONG msNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    SliderData* dat;
    struct comm_info *ci=(APTR)GetTagData(MYATTR_PageSlider_CommInfo,0,msg->ops_AttrList);
    struct TagItem tags[5];
    Object *doc=(Object*)GetTagData(MYATTR_PageSlider_Document,0,msg->ops_AttrList);
    Object *slider,*str;
    int num_pages,open_page,page;
    struct PDFDoc *pdfdoc;
    if(!ci || !doc)
	return 0;
    get(doc,MYATTR_Document_NumPages,&num_pages);
    get(doc,MYATTR_Document_OpenPage,&open_page);
    get(doc,MYATTR_Document_PDFDoc,&pdfdoc);
    page=GetTagData(MYATTR_PageSlider_Value,open_page,msg->ops_AttrList);
    str=StringObject,
	StringFrame,
	MUIA_String_MaxLen,32,
	MUIA_CycleChain,TRUE,
	End;
    tags[0].ti_Tag=MUIA_Group_Horiz;
    tags[0].ti_Data=TRUE;
    tags[1].ti_Tag=MUIA_Group_Child;
    tags[1].ti_Data=(ULONG)(slider=PageSlider1Object,
	SliderFrame,
	MYATTR_PageSlider_CommInfo,ci,
	MYATTR_PageSlider_PDFDoc,pdfdoc,
	MYATTR_PageSlider_StringObj,str,
	MYATTR_PageSlider_Value1,page,
	MUIA_Background,MUII_SliderBack,
	MUIA_Slider_Horiz,TRUE,
	MUIA_Numeric_Min,1,
	MUIA_Numeric_Max,num_pages,
	MUIA_Weight,500,
	End);
    tags[2].ti_Tag=MUIA_Group_Child;
    tags[2].ti_Data=(ULONG)BalanceObject,
	End;
    tags[3].ti_Tag=MUIA_Group_Child;
    tags[3].ti_Data=(ULONG)str;
    tags[4].ti_Tag=TAG_MORE;
    tags[4].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;
    if(obj=(Object *)DoSuperMethodA(cl,obj,(APTR)msg)) {
	dat=INST_DATA(cl,obj);
	dat->sliderobj=slider;
	dat->strobj=str;
	dat->value=page;
	DoMethod9(str,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
		  slider,3,MUIM_Set,MYATTR_PageSlider_Label,MUIV_TriggerValue);
	DoMethod9(slider,MUIM_Notify,MYATTR_PageSlider_Value1,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_PageSlider_Value,MUIV_TriggerValue);
    }
    msg->ops_AttrList=(struct TagItem *)tags[4].ti_Data;
    DB(dprintf("msNew: %p\n", obj));
    return (ULONG)obj;
}

Static ULONG msSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    SliderData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_PageSlider_Document: {
	    int num_pages;
	    struct PDFDoc *pdfdoc;
	    get((Object*)tag->ti_Data,MYATTR_Document_NumPages,&num_pages);
	    get((Object*)tag->ti_Data,MYATTR_Document_PDFDoc,&pdfdoc);
	    SetAttrs(dat->sliderobj,
		     MUIA_Numeric_Max,num_pages,
		     MYATTR_PageSlider_PDFDoc,pdfdoc,
		     TAG_END);
	    break;
	  }
	  case MYATTR_PageSlider_Value:
	    if(tag->ti_Data!=dat->value) {
		dat->value=tag->ti_Data;
		nnset(dat->sliderobj,MYATTR_PageSlider_Value1,tag->ti_Data);
	    }
	    break;

	  case MYATTR_PageSlider_Label:
	    set(dat->sliderobj,tag->ti_Tag,tag->ti_Data);
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG msGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    switch(msg->opg_AttrID) {
      case MYATTR_PageSlider_Value: {
	SliderData* dat=INST_DATA(cl,obj);
	*msg->opg_Storage=dat->value;
	return TRUE;
      }
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

BEGIN_DISPATCHER(msDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW          : return msNew        (cl,obj,(APTR)msg);
      case OM_SET          : return msSet        (cl,obj,(APTR)msg);
      case OM_GET          : return msGet        (cl,obj,(APTR)msg);
      case OM_DISPOSE      : DB(dprintf("msDispose: %p\n", obj)); break;
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(msDispatcher)

struct MUI_CustomClass *pageslider_mcc;
static int count;

BOOL createPageSliderClass(void) {
    if(count++==0) {
	if(slider1_mcc=MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,sizeof(SliderData1),(APTR)&ms1Dispatcher)) {
	    if(pageslider_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(SliderData),(APTR)&msDispatcher)) {
		return TRUE;
	    }
	    MUI_DeleteCustomClass(slider1_mcc);
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deletePageSliderClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(pageslider_mcc);
	MUI_DeleteCustomClass(slider1_mcc);
    }
    return TRUE;
}


