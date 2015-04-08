/*************************************************************************\
 *                                                                       *
 * doclens.c                                                             *
 *                                                                       *
 * Copyright 1999-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <exec/execbase.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include "AComm.h"
#include "Apdf.h"
#include "docbitmap.h"
#include "document.h"
#include "doclens.h"

#define DB(x)   //x
#define Static static

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

struct DocLensData {
    struct comm_info *comm_info;
    Object *doc;
    Object *bmobj;
};
typedef struct DocLensData DocLensData;

Static ULONG dlNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocLensData* dat;
    struct TagItem tags[11];
    Object *menu,*bmobj;
    Object *doc=(Object*)GetTagData(MYATTR_DocLens_Document,0,msg->ops_AttrList);
    struct comm_info *ci=(struct comm_info *)GetTagData(MYATTR_DocLens_CommInfo,0,msg->ops_AttrList);
    int num_pages;
    if(!doc || !ci)
	return 0;

    tags[0].ti_Tag=MUIA_Window_DragBar;
    tags[0].ti_Data=FALSE;
    tags[1].ti_Tag=MUIA_Window_CloseGadget;
    tags[1].ti_Data=FALSE;
    tags[2].ti_Tag=MUIA_Window_DepthGadget;
    tags[2].ti_Data=FALSE;
    tags[3].ti_Tag=MUIA_Window_SizeGadget;
    tags[3].ti_Data=FALSE;
    tags[4].ti_Tag=MUIA_Window_Borderless;
    tags[4].ti_Data=TRUE;
    tags[5].ti_Tag=MUIA_Window_Activate;
    tags[5].ti_Data=FALSE;
    tags[6].ti_Tag=MUIA_Window_LeftEdge;
    tags[6].ti_Data=MUIV_Window_LeftEdge_Moused;
    tags[7].ti_Tag=MUIA_Window_TopEdge;
    tags[7].ti_Data=MUIV_Window_TopEdge_Moused;
    tags[8].ti_Tag=MUIA_Window_RootObject;
    tags[8].ti_Data=(ULONG)(bmobj=DocBitmapObject,
	MUIA_InnerLeft,0,
	MUIA_InnerRight,0,
	MUIA_InnerTop,0,
	MUIA_InnerBottom,0,
	MYATTR_DocBitmap_CommInfo,ci,
	MYATTR_DocBitmap_LensMode,TRUE,
	MYATTR_DocBitmap_Document,doc,
	MYATTR_DocBitmap_Page,GetTagData(MYATTR_DocLens_Page,1,msg->ops_AttrList),
	MYATTR_DocBitmap_DPI,GetTagData(MYATTR_DocLens_DPI,200,msg->ops_AttrList),
	MYATTR_DocBitmap_Rotate,GetTagData(MYATTR_DocLens_Rotate,0,msg->ops_AttrList),
	MYATTR_DocBitmap_CropBox,GetTagData(MYATTR_DocLens_CropBox,0,msg->ops_AttrList),
	AA(MYATTR_DocBitmap_TextAA,GetTagData(MYATTR_DocLens_TextAA,0,msg->ops_AttrList),)
	AA(MYATTR_DocBitmap_StrokeAA,GetTagData(MYATTR_DocLens_StrokeAA,0,msg->ops_AttrList),)
	AA(MYATTR_DocBitmap_FillAA,GetTagData(MYATTR_DocLens_FillAA,0,msg->ops_AttrList),)
	End);
    tags[9].ti_Tag=MUIA_Window_DefaultObject;
    tags[9].ti_Data=(ULONG)bmobj;
    tags[10].ti_Tag=TAG_MORE;
    tags[10].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	int n;
	dat=INST_DATA(cl,obj);
	dat->comm_info=ci;
	dat->doc=doc;
	dat->bmobj=bmobj;
    }
    msg->ops_AttrList=(struct TagItem*)tags[10].ti_Data;
    return (ULONG)obj;
}

Static ULONG dlSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocLensData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocLens_CropBox:
	    set(dat->bmobj,MYATTR_DocBitmap_CropBox,tag->ti_Data);
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

BEGIN_DISPATCHER(dlDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                    return dlNew    (cl,obj,(APTR)msg);
      /*case OM_DISPOSE:                return dlDispose(cl,obj,msg);
      case OM_GET:                    return dlGet    (cl,obj,(APTR)msg);*/
      case OM_SET:                    return dlSet    (cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(dlDispatcher)

struct MUI_CustomClass *doclens_mcc;
static int count;

BOOL createDocLensClass(void) {
    if(count++==0) {
	doclens_mcc=NULL;
	doclens_mcc=MUI_CreateCustomClass(NULL,MUIC_Window,NULL,sizeof(DocLensData),(APTR)&dlDispatcher);
	if(doclens_mcc==NULL)
	    count=0;
    }
    return count;
}

BOOL deleteDocLensClass(void) {
    if(count && --count==0)
	MUI_DeleteCustomClass(doclens_mcc);
    return TRUE;
}

