/*************************************************************************\
 *                                                                       *
 * docgroup.c                                                            *
 *                                                                       *
 * Copyright 1999-2005 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <mui/Listtree_mcc.h>
#ifndef MUIV_Listtree_Insert_PrevNode_Tail
#   define MUIV_Listtree_Insert_PrevNode_Tail MUIV_Lt_Insert_PrevNode_Tail
#   define MUIV_Listtree_DoubleClick_Off MUIV_Lt_DoubleClick_Off
#   define MUIV_Listtree_Insert_ListNode_Root MUIV_Lt_Insert_ListNode_Root
#   define MUIV_Listtree_Active_Off MUIV_Lt_Active_Off
#endif
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <exec/execbase.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
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
#include "docgroup.h"
#include "docscroll.h"
#include "pageset.h"
#include "pageslider.h"
#include "prefs.h"
#include "msgs.h"

#define DB(x)   //x
#define Static  static

#define kprintf dprintf

struct DocGroupData {
    struct comm_info *comm_info;
    Object *doc;
    Object *docchg;
    Object *pageobj;
    Object *zoomobj;
    Object *pagesetobj;
    Object *docscrollobj;
    Object *linkobj;
    Object *searchobj;
    Object *outlineswin;
    Object *outlineslv;
    Object *toolbar;
    Object *navbar;
    Object *stateobj;
    Object *subgroupobj;
    Object *balanceobj;
    BOOL use_toolbar;
    BOOL use_navbar;
    BOOL full_screen;
    BOOL dosearch;
    BOOL insearch;
};
typedef struct DocGroupData DocGroupData;

extern Object *App;

/*
 *  Server interface functions.
 */

Static int init_find(struct comm_info *ci,const char* str) {
    struct msg_find* p=ci->cmddata;
    sync();
    strcpy(p->str,str);
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_INITFIND);
}

Static int find(struct comm_info *ci,struct PDFDoc *doc,int start,int stop,int* xmin,int* ymin,int* xmax,int* ymax,int* page) {
    struct msg_find* p=ci->cmddata;
    int r;
    sync();
    p->doc=doc;
    p->top=start;
    p->bottom=stop;
    /*p->xmin=*xmin;
    p->ymin=*ymin;
    p->xmax=*xmax;
    p->ymax=*ymax;
    p->page=*page;*/
    r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_FIND);
    *xmin=p->xmin;
    *ymin=p->ymin;
    *xmax=p->xmax;
    *ymax=p->ymax;
    *page=p->page;
    return r;
}

Static void end_find(struct comm_info *ci) {
    sync();
    (*ci->sv->exchange_msg)(ci,NULL,0,MSGID_ENDFIND);
}


extern const char *zoomEntries[];

/*
 *  DocGroup Methods.
 */

Static ULONG dgNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocGroupData* dat;
    struct TagItem tags[5];
    Object *pagesetobj,*docscrollobj,*pageobj;
    Object *zoomobj,*nxtsrchobj,*linkobj,*searchobj,*stateobj;
    Object *backobj,*fwdobj,*menu,*toolbar,*navbar,*subgroupobj;
    Object *doc=(Object*)GetTagData(MYATTR_DocGroup_Document,0,msg->ops_AttrList);
    struct comm_info *ci=(struct comm_info*)GetTagData(MYATTR_DocGroup_CommInfo,0,msg->ops_AttrList);
    BOOL full_screen=GetTagData(MYATTR_DocGroup_FullScreen,0,msg->ops_AttrList);
    struct PDFDoc *pdfdoc;
    int num_pages,page=-1;
    if(!doc || !ci)
	return 0;
    DoMethod2(doc,MYM_Document_IncRef);
    get(doc,MYATTR_Document_NumPages,&num_pages);
    get(doc,MYATTR_Document_PDFDoc,&pdfdoc);

    pageobj=PageSliderObject,
	MYATTR_PageSlider_Document,doc,
	MYATTR_PageSlider_CommInfo,ci,
	MUIA_Weight,1000,
	End;

    if(pageobj) {
	const char *pagelabel;
	int zoom,rotate;
	struct MYS_PageSet_CropBox *box;
	pagelabel=(const char*)GetTagData(MYATTR_DocGroup_PageLabel,0,msg->ops_AttrList);
	DB(kprintf("DocGroup::new pagelabel %p (%s)\n",pagelabel,pagelabel?pagelabel:"");)
	if(pagelabel) {
	    set(pageobj,MYATTR_PageSlider_Label,pagelabel);
	    get(pageobj,MYATTR_PageSlider_Value,&page);
	} else
	    page=GetTagData(MYATTR_DocGroup_Page,-1,msg->ops_AttrList);
	DB(kprintf("DocGroup::new page %d\n",page);)
	zoom=GetTagData(MYATTR_DocGroup_Zoom,-1,msg->ops_AttrList);
	rotate=GetTagData(MYATTR_DocGroup_Rotate,-1,msg->ops_AttrList);
	box=(APTR)GetTagData(MYATTR_DocGroup_CropBox,0,msg->ops_AttrList);
	DB(kprintf("DocGroup::new zoom %d\n",zoom);)
	pagesetobj=PageSetObject,
	    MYATTR_PageSet_CommInfo,ci,
	    MYATTR_PageSet_Document,doc,
	    MYATTR_PageSet_VGroup,NULL,
	    MYATTR_PageSet_FullScreen,full_screen,
	    page==-1?TAG_IGNORE:MYATTR_PageSet_Page,page,
	    zoom==-1?TAG_IGNORE:MYATTR_PageSet_Zoom,zoom,
	    rotate==-1?TAG_IGNORE:MYATTR_PageSet_Rotate,rotate,
	    box?MYATTR_PageSet_CropBox:TAG_IGNORE,box,
	    End;
    }

/*#ifdef HAVE_MUI_BUSY_H
    stateobj=HGroup,
	MUIA_Group_PageMode,TRUE,
	Child,BusyBar,
	Child,linkobj=TextObject,
	    TextFrame,
	    MUIA_Background,MUII_TextBack,
	    End,
	End;
#else
    linkobj=TextObject,
	TextFrame,
	MUIA_Background,MUII_TextBack,
	End;
    stateObj=TextObj,
	TextFrame,
	MUIA_Background,MUII_TextBack,
	MUIA_Text_SetMax,TRUE,
	MUIA_Text_Contents," ",
	End;
#endif*/

    tags[0].ti_Tag=MUIA_Group_Child;
    tags[0].ti_Data=(ULONG)(subgroupobj=HGroup,
	Child,(ULONG)(docscrollobj=DocScrollObject,
	    MYATTR_DocScroll_CommInfo,ci,
	    MYATTR_DocScroll_Contents,pagesetobj,
	    MYATTR_DocScroll_FullScreen,full_screen,
	    page==-1?TAG_IGNORE:MYATTR_DocScroll_Page,page,
	    End),
	End);
    tags[1].ti_Tag=MUIA_Group_Child;
    tags[1].ti_Data=(ULONG)(navbar=HGroup,
	Child,backobj=SimpleButton(STR(MSG_BACK_GAD)),
	Child,fwdobj=SimpleButton(STR(MSG_FORWARD_GAD)),
	Child,Label2(STR(MSG_PAGE_GAD)),
	Child,pageobj,
	End);
    tags[2].ti_Tag=MUIA_Group_Child;
    tags[2].ti_Data=(ULONG)(toolbar=HGroup,
	Child,HGroup,
	    Child,KeyLabel2(KSTR(MSG_ZOOM_GAD),KEY(MSG_ZOOM_GAD)),
	    Child,zoomobj=CycleObject,
		MUIA_Cycle_Entries,zoomEntries,
		MUIA_Cycle_Active,defZoom,
		MUIA_CycleChain,TRUE,
		MUIA_ControlChar,KEY(MSG_ZOOM_GAD),
		End,
	    Child,KeyLabel2(KSTR(MSG_SEARCH_GAD),KEY(MSG_SEARCH_GAD)),
	    Child,searchobj=StringObject,
		StringFrame,
		MUIA_String_MaxLen,99,
		MUIA_Weight,200,
		MUIA_CycleChain,TRUE,
		MUIA_ControlChar,KEY(MSG_SEARCH_GAD),
		MUIA_Weight,1000,
		End,
	    Child,nxtsrchobj=SimpleButton(STR(MSG_NEXT_GAD)),
	    End,
	Child,BalanceObject,
	    End,
	Child,linkobj=TextObject,
	    TextFrame,
	    MUIA_Background,MUII_TextBack,
	    End,
	Child,stateobj=HGroup,
	    TextFrame,
	    MUIA_Background,MUII_TextBack,
	    MUIA_Group_PageMode,TRUE,
	    Child,ImageObject,
		MUIA_Image_Spec,"2:ffffffff,00000000,00000000",
		MUIA_Image_FontMatch,TRUE,
		End,
	    Child,RectangleObject,
		End,
	    End,
	/*Child,stateobj=TextObject,
	    TextFrame,
	    MUIA_Background,MUII_TextBack,
	    MUIA_Text_SetMax,TRUE,
	    MUIA_Text_Contents,"  ",
	    End,*/
	End);
    tags[3].ti_Tag=full_screen?MUIA_Background:TAG_IGNORE;
    tags[3].ti_Data=(ULONG)"2:00000000,00000000,00000000";
    tags[4].ti_Tag=TAG_MORE;
    tags[4].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

#if 0
    if(docscrollobj) {
	get(docscrollobj,MYATTR_DocScroll_Page,&page);
	set(pageobj,MYATTR_PageSlider_Value,page);
    } else {
	/* the call to DoSuperMethod below will fail.*/
    }
#endif

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	LONG t;
	dat=INST_DATA(cl,obj);
	dat->comm_info=ci;
	dat->doc=doc;
	dat->docchg=doc;
	dat->pagesetobj=pagesetobj;
	dat->pageobj=pageobj;
	dat->zoomobj=zoomobj;
	dat->docscrollobj=docscrollobj;
	dat->searchobj=searchobj;
	dat->linkobj=linkobj;
	dat->outlineswin=NULL;
	dat->toolbar=toolbar;
	dat->navbar=navbar;
	dat->stateobj=stateobj;
	dat->subgroupobj=subgroupobj;
	dat->use_toolbar=TRUE;
	dat->use_navbar=TRUE;
	dat->full_screen=full_screen;

	DoMethod9(pageobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		  docscrollobj,3,MUIM_Set,MYATTR_DocScroll_Page,MUIV_TriggerValue);
	DoMethod9(docscrollobj,MUIM_Notify,MYATTR_DocScroll_Page,MUIV_EveryTime,
		  pageobj,3,MUIM_NoNotifySet,MYATTR_PageSlider_Value,MUIV_TriggerValue);
	DoMethod9(zoomobj,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
		  docscrollobj,3,MUIM_Set,MYATTR_DocScroll_Zoom,MUIV_TriggerValue);
	DoMethod9(docscrollobj,MUIM_Notify,MYATTR_DocScroll_Zoom,MUIV_EveryTime,
		  zoomobj,3,MUIM_Set,MUIA_Cycle_Active,MUIV_TriggerValue);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_Message,MUIV_EveryTime,
		  linkobj,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);
	DoMethod9(docscrollobj,MUIM_Notify,MYATTR_DocScroll_DocChanged,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_Document,MUIV_TriggerValue);
	DoMethod7(searchobj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
		  obj,1,MYM_DocGroup_Search);
	DoMethod7(nxtsrchobj,MUIM_Notify,MUIA_Selected,FALSE,
		  obj,1,MYM_DocGroup_Search);
	DoMethod7(backobj,MUIM_Notify,MUIA_Selected,FALSE,
		  docscrollobj,1,MYM_DocScroll_Backward);
	DoMethod7(fwdobj,MUIM_Notify,MUIA_Selected,FALSE,
		  docscrollobj,1,MYM_DocScroll_Forward);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_Run,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_Run,MUIV_TriggerValue);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_URL,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_URL,MUIV_TriggerValue);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_Open,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_Open,MUIV_TriggerValue);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_OpenNew,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_OpenNew,MUIV_TriggerValue);
	DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_PageNew,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_PageNew,MUIV_TriggerValue);
	/*DoMethod9(pagesetobj,MUIM_Notify,MYATTR_PageSet_Ready,MUIV_EveryTime,
		  stateobj,3,MUIM_Set,MUIA_Group_ActivePage,MUIV_TriggerValue);*/
	DoMethod10(pagesetobj,MUIM_Notify,MYATTR_PageSet_Ready,MUIV_EveryTime,
		   App,4,MUIM_Application_PushMethod,
		   obj,1,MYM_DocGroup_Ready);
	get(docscrollobj,MYATTR_DocScroll_Zoom,&t);
	DB(kprintf("DocGroup::new -> zoom %d\n",t);)
	nnset(zoomobj,MUIA_Cycle_Active,t);

	get(doc,MYATTR_Document_OpenOutlines,&t);
	if(t)
	  DoMethod2(obj,MYM_DocGroup_OpenOutlines);
    } else if(DoMethod2(doc,MYM_Document_DecRef)==0)
	MUI_DisposeObject(doc);

    msg->ops_AttrList=(struct TagItem*)tags[4].ti_Data;
    DB(dprintf("dgNew: %p\n", obj));
    return (ULONG)obj;
}

Static ULONG dgDispose(struct IClass *cl,Object *obj,Msg msg) {
    DocGroupData *dat=INST_DATA(cl,obj);
    DB(dprintf("dgDispose: %p\n", obj));
    DoMethod2(obj,MYM_DocGroup_CloseOutlines);
    if(dat->doc && DoMethod2(dat->doc,MYM_Document_DecRef)==0)
	MUI_DisposeObject(dat->doc);
    if(!dat->use_toolbar)
	MUI_DisposeObject(dat->toolbar);
    if(!dat->use_navbar)
	MUI_DisposeObject(dat->navbar);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG dgGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    DocGroupData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_DocGroup_Document:
	*(Object**)msg->opg_Storage=dat->doc;
	return TRUE;
      case MYATTR_DocGroup_DocChanged:
	*(Object**)msg->opg_Storage=dat->docchg;
	return TRUE;
      case MYATTR_DocGroup_Page:
	get(dat->docscrollobj,MYATTR_DocScroll_Page,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_Zoom:
	get(dat->docscrollobj,MYATTR_DocScroll_Zoom,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_Rotate:
	get(dat->pagesetobj,MYATTR_PageSet_Rotate,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_CropBox:
	get(dat->pagesetobj,MYATTR_PageSet_CropBox,msg->opg_Storage);
	return TRUE;
#if HAS_AA
      case MYATTR_DocGroup_TextAA:
	get(dat->pagesetobj,MYATTR_PageSet_TextAA,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_StrokeAA:
	get(dat->pagesetobj,MYATTR_PageSet_StrokeAA,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_FillAA:
	get(dat->pagesetobj,MYATTR_PageSet_FillAA,msg->opg_Storage);
	return TRUE;
#endif
      case MYATTR_DocGroup_Run:
      case MYATTR_DocGroup_URL:
      case MYATTR_DocGroup_Open:
      case MYATTR_DocGroup_OpenNew:
      case MYATTR_DocGroup_PageNew:
	*msg->opg_Storage=0;
	return TRUE;
      case MYATTR_DocGroup_MainObject:
	*msg->opg_Storage=(ULONG)dat->docscrollobj;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG dgSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocGroupData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocGroup_DocChanged:
	    dat->docchg=(Object*)tag->ti_Data;
	    break;

	  case MYATTR_DocGroup_Page:
	    set(dat->docscrollobj,MYATTR_DocScroll_Page,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_PageLabel: {
	    ULONG x;
	    set(dat->pageobj,MYATTR_PageSlider_Label,tag->ti_Data);
	    get(dat->pageobj,MYATTR_PageSlider_Value,&x);
	    set(dat->docscrollobj,MYATTR_DocScroll_Page,x);
	    break;
	  }
	  case MYATTR_DocGroup_Zoom:
	    set(dat->docscrollobj,MYATTR_DocScroll_Zoom,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_Rotate:
	    set(dat->pagesetobj,MYATTR_PageSet_Rotate,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_CropBox:
	    set(dat->pagesetobj,MYATTR_PageSet_CropBox,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_Document: {
	    Object *doc=(Object*)tag->ti_Data;
	    const char *name;
	    if(doc && doc!=dat->doc) {
		int page;
		int t,u;
		Object *olddoc;
		DoMethod2(obj,MYM_DocGroup_CloseOutlines);
		olddoc=dat->doc;
		dat->doc=doc;
		page=GetTagData(MYATTR_DocGroup_Page,-1,msg->ops_AttrList);
		SetAttrs(dat->docscrollobj,
			 MYATTR_DocScroll_Document,doc,
			 page==-1?TAG_IGNORE:MYATTR_DocScroll_Page,page,
			 TAG_END);
		get(dat->docscrollobj,MYATTR_DocScroll_Page,&page);
		set(dat->pageobj,MYATTR_PageSlider_Document,doc);
		set(dat->pageobj,MYATTR_PageSlider_Value,page);
		/*SetAttrs(dat->pageobj,
			 //MUIA_NoNotify,TRUE,
			 MYATTR_PageSlider_Document,doc,
			 MYATTR_PageSlider_Value,page,
			 TAG_END);*/
		if(!dat->full_screen) {
		    get(doc,MYATTR_Document_OpenNavBar,&t);
		    get(doc,MYATTR_Document_OpenToolBar,&u);
		    SetAttrs(obj,MYATTR_DocGroup_ShowNavBar,t,
			     MYATTR_DocGroup_ShowToolBar,u,TAG_END);
		}
		DoMethod2(doc,MYM_Document_IncRef);
		set(obj,MYATTR_DocGroup_DocChanged,doc);
		if(olddoc && DoMethod2(olddoc,MYM_Document_DecRef)==0)
		    MUI_DisposeObject(olddoc);
	    }
	    break ;
	  }
#if HAS_AA
	  case MYATTR_DocGroup_TextAA:
	    set(dat->pagesetobj,MYATTR_PageSet_TextAA,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_StrokeAA:
	    set(dat->pagesetobj,MYATTR_PageSet_StrokeAA,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_FillAA:
	    set(dat->pagesetobj,MYATTR_PageSet_FillAA,tag->ti_Data);
	    break;
#endif

	  case MYATTR_DocGroup_ShowToolBar:
	    if(tag->ti_Data!=dat->use_toolbar) {
		if(DoMethod2(obj,MUIM_Group_InitChange)) {
		    dat->use_toolbar=tag->ti_Data;
		    if(dat->use_toolbar) {
			DoMethod3(obj,OM_ADDMEMBER,dat->toolbar);
		    } else {
			DoMethod3(obj,OM_REMMEMBER,dat->toolbar);
		    }
		    DoMethod2(obj,MUIM_Group_ExitChange);
		}
	    }
	    break;

	  case MYATTR_DocGroup_ShowNavBar:
	    if(tag->ti_Data!=dat->use_navbar) {
		if(DoMethod2(obj,MUIM_Group_InitChange)) {
		    dat->use_navbar=tag->ti_Data;
		    if(dat->use_navbar) {
			DoMethod4(obj,MUIM_Family_Insert,dat->navbar, dat->subgroupobj);
		    } else {
			DoMethod3(obj,OM_REMMEMBER,dat->navbar);
		    }
		    DoMethod2(obj,MUIM_Group_ExitChange);
		}
	    }
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}


Static ULONG dgSearch(struct IClass *cl,Object *obj) {
    DocGroupData* dat=INST_DATA(cl,obj);
    struct Rectangle rect;
    int pg,pages;
    int page,top,bottom,xmin,ymin,xmax,ymax;
    BOOL found=FALSE;
    LONG ready;
    const char *str;
    Object *pageobj;

    if(dat->insearch)
	return 0;

    get(dat->pagesetobj,MYATTR_PageSet_Ready,&ready);
    if (!ready) {
	dat->dosearch=TRUE;
	return 0;
    }

    dat->dosearch=FALSE;
    dat->insearch=TRUE;

    get(dat->searchobj,MUIA_String_Contents,&str);
    get(dat->pagesetobj,MYATTR_PageSet_ActivePage,&pageobj);
    get(pageobj,MYATTR_DocBitmap_Page,&page);
    get(pageobj,MYATTR_DocBitmap_Selection,&rect);
    bottom=0;

    if(rect.MinX==-1)
	top=1;
    else {
	top=0;
	xmin=rect.MaxX;
	ymin=(rect.MinY+rect.MaxY)/2;
	xmax=0;
	ymax=0;
    }

    if(DoMethod8(pageobj,MYM_DocBitmap_Search,str,top,&xmin,&ymin,&xmax,&ymax))
	found=TRUE;

    if(!found) {
	int k,l=0;
	struct List* winlist;
	Object* o;
	struct Node* state;
	ULONG sigs=0;
	struct PDFDoc *doc;
	Object *app,*gauge;
	get(dat->doc,MYATTR_Document_NumPages,&pages);
	get(dat->doc,MYATTR_Document_PDFDoc,&doc);
	get(obj,MUIA_ApplicationObject,&app);
	gauge=create_gauge(app,STR(MSG_SEARCHING),pages);
	pg=page;
	if(init_find(dat->comm_info,str)) {
	    LONG r=0;
	    for(k=pg+1;k<=pages;++k) {
		if(find(dat->comm_info,doc,k,k+1,&xmin,&ymin,&xmax,&ymax,&page)) {
		    found=TRUE;
		    break;
		}
		if(gauge) {
		    set(gauge,MUIA_Gauge_Current,++l);
		    r=DoMethod3(app,MUIM_Application_NewInput,&sigs);
		    if(r==MUIV_Application_ReturnID_Quit ||
		       r==ID_ABORT)
			break;
		}
	    }
	    if(!found && r!=ID_ABORT && r!=MUIV_Application_ReturnID_Quit) {
		for(k=1;k<pg;++k) {
		    if(find(dat->comm_info,doc,k,k+1,&xmin,&ymin,&xmax,&ymax,&page)) {
			found=TRUE;
			break;
		    }
		    if(gauge) {
			set(gauge,MUIA_Gauge_Current,++l);
			r=DoMethod3(app,MUIM_Application_NewInput,&sigs);
			if(r==MUIV_Application_ReturnID_Quit ||
			   r==ID_ABORT)
			    break;
		    }
		}
	    }
	    end_find(dat->comm_info);
	}
	if(found) {
	    set(dat->pageobj,MYATTR_PageSlider_Value,page);
	    /*get(dat->pagesetobj,MYATTR_PageSet_Ready,&ready);
	    if (ready)
		DoMethod8(pageobj,MYM_DocBitmap_Search,str,TRUE,&xmin,&ymin,&xmax,&ymax);
	    else */{
		dat->dosearch=TRUE;
		found=FALSE;
	    }
	}
	delete_gauge(gauge);
    }
    if(found) {
	LONG left,width,top,height,t;
	rect.MinX=xmin;
	rect.MinY=ymin;
	rect.MaxX=xmax;
	rect.MaxY=ymax;
	set(pageobj,MYATTR_DocBitmap_Selection,&rect);
	/*if(dat->use_vgroup) {
	    xmin+=borderpixels; //*****
	    ymin+=borderpixels;
	    xmax+=borderpixels;
	    ymax+=borderpixels;
	    get(dat->vgroup,MUIA_Width,&width);
	    get(dat->vgroup,MUIA_InnerRight,&t);
	    width-=t;
	    get(dat->vgroup,MUIA_InnerLeft,&t);
	    width-=t;
	    get(dat->vgroup,MUIA_Height,&height);
	    get(dat->vgroup,MUIA_InnerBottom,&t);
	    height-=t;
	    get(dat->vgroup,MUIA_InnerTop,&t);
	    height-=t;
	    get(dat->vgroup,MUIA_Virtgroup_Left,&left);
	    get(dat->vgroup,MUIA_Virtgroup_Top,&top);
	    if(xmin<left)
		left=xmin;
	    else if(xmax>left+width)
		left=xmax-width;
	    if(ymin<top)
		top=ymin;
	    else if(ymax>top+height)
		top=ymax-height;
	    SetAttrs(dat->vgroup,
		     MUIA_Virtgroup_Left,left,
		     MUIA_Virtgroup_Top,top,
		     TAG_END);
	}*/
    } else if(!dat->dosearch) {
	set(pageobj,MYATTR_DocBitmap_Selection,NULL);
	set(dat->linkobj,MUIA_Text_Contents,STR(MSG_NOT_FOUND));
    }
    dat->insearch=FALSE;
    return 0;
}

Static ULONG dgReady(struct IClass *cl,Object *obj) {
    DocGroupData* dat=INST_DATA(cl,obj);
    LONG ready;
    get(dat->pagesetobj,MYATTR_PageSet_Ready,&ready);
    set(dat->stateobj,MUIA_Group_ActivePage,ready);
    if(ready && dat->dosearch)
	dgSearch(cl,obj);
    return 0;
}

Static int dgInsertOutlines(Object *listtree,
			    struct MUIS_Listtree_TreeNode *tn,
			    const char **titles,
			    struct Action **actions,
			    int level) {
    struct MUIS_Listtree_TreeNode *last=NULL;
    int n=0;
    while(*titles) {
	const char *p=*titles;
	int limit=last?level+1:level;
	int l=*p++-'a';
	if(l<level)
	    break;
	else if(l==level) {
	    ULONG f=0;
	    if(*p=='T' || *p=='t') {
		f=TNF_LIST;
		if(*p=='T')
		    f|=TNF_OPEN;
		++p;
	    }
	    ++p;
	    last=(struct MUIS_Listtree_TreeNode*)DoMethod7(listtree,MUIM_Listtree_Insert,p,*actions,tn,MUIV_Listtree_Insert_PrevNode_Tail,f);
	    ++titles;
	    ++actions;
	    ++n;
	} else {
	    int k=dgInsertOutlines(listtree,last,titles,actions,level+1);
	    n+=k;
	    titles+=k;
	    actions+=k;
	}
    }
    return n;
}

Static ULONG dgOpenOutlines(struct IClass *cl,Object *obj) {
    DocGroupData *dat=INST_DATA(cl,obj);
    Object *win=dat->outlineswin;
    if(dat->outlineslv) {
	DoMethod2(obj,MYM_DocGroup_CloseOutlines);
    	return 0;
    }
    if(!dat->outlineslv) {
	struct OutlinesInfo *outlines;
	get(dat->doc,MYATTR_Document_Outlines,&outlines);
	if(outlines) {
	    Object *listtree;
	    dat->outlineslv=ListviewObject,
		//MUIA_Prop_UseWinBorder,MUIV_Prop_UseWinBorder_Right,
		MUIA_Listview_List,listtree=ListtreeObject,
		    InputListFrame,
		    MUIA_Font,MUIV_Font_Fixed,
		    MUIA_Background,MUII_ListBack,
		    MUIA_Listtree_EmptyNodes,TRUE,
		    MUIA_Listtree_DuplicateNodeName,FALSE,
		    MUIA_Listtree_DoubleClick,MUIV_Listtree_DoubleClick_Off,
		    End,
		MUIA_Weight,20,
		End;
	    dgInsertOutlines(listtree,(struct MUIS_Listtree_TreeNode*)MUIV_Listtree_Insert_ListNode_Root,
		    outlines->titles,outlines->actions,0);
	    DoMethod7(dat->outlineslv,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
		    obj,1,MYM_DocGroup_GoToOutline);
	}
    }
    if(dat->outlineslv && !dat->balanceobj && !dat->outlineswin) {
	ULONG t=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_ExtOutlines);
	if(!t||!*(int*)t) {
	    if(!dat->balanceobj) {
		dat->balanceobj=BalanceObject,End;
		if(dat->balanceobj) {
		    if(DoMethod2(dat->subgroupobj,MUIM_Group_InitChange)) {
			DoMethod3(dat->subgroupobj,OM_ADDMEMBER,dat->outlineslv);
			DoMethod3(dat->subgroupobj,OM_ADDMEMBER,dat->balanceobj);
			DoMethod6(dat->subgroupobj,MUIM_Group_Sort,dat->outlineslv,dat->balanceobj,dat->docscrollobj,NULL);
			DoMethod2(dat->subgroupobj,MUIM_Group_ExitChange);
		    } else {
			MUI_DisposeObject(dat->balanceobj);
			dat->balanceobj=NULL;
		    }
		}
	    }
	} else if(!win) {
	    win=dat->outlineswin=WindowObject,
		MUIA_Window_Title,STR(MSG_OUTLINES_WIN),
		MUIA_Window_ID,MAKE_ID('O','U','T','L'),
		MUIA_Window_RootObject,dat->outlineslv,
		MUIA_Window_DefaultObject,dat->outlineslv,
		//MUIA_Window_Screen,_screen(obj),
		MUIA_Window_IsSubWindow,TRUE,
		//MUIA_Window_UseRightBorderScroller,TRUE,
		End;
	    if(win) {
		Object* app;
		get(dat->doc,MYATTR_Document_Application,&app);
		DoMethod3(app,OM_ADDMEMBER,win);
		DoMethod7(win,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			obj,2,MYM_DocGroup_CloseOutlines);
	    }
	}
    }
    if(dat->outlineswin)
	set(dat->outlineswin,MUIA_Window_Open,TRUE);
    return 0;
}

Static ULONG dgCloseOutlines(struct IClass *cl,Object *obj) {
    DocGroupData *dat=INST_DATA(cl,obj);
    if(dat->balanceobj) {
	if(DoMethod2(dat->subgroupobj,MUIM_Group_InitChange)) { /* FIXME: what if that fails? */
	    DoMethod3(dat->subgroupobj,OM_REMMEMBER,dat->outlineslv);
	    DoMethod3(dat->subgroupobj,OM_REMMEMBER,dat->balanceobj);
	    DoMethod2(dat->subgroupobj,MUIM_Group_ExitChange);
	    MUI_DisposeObject(dat->balanceobj);
	    dat->balanceobj=NULL;
	    MUI_DisposeObject(dat->outlineslv);
	    dat->outlineslv=NULL;
	}
    } else if(dat->outlineswin) {
	Object *app;
	set(dat->outlineswin,MUIA_Window_Open,FALSE);
	get(dat->outlineswin,MUIA_ApplicationObject,&app);
	DoMethod3(app,OM_REMMEMBER,dat->outlineswin);
	MUI_DisposeObject(dat->outlineswin);
	dat->outlineswin=NULL;
	dat->outlineslv=NULL;
    }
    return 0;
}

Static ULONG dgGoToOutline(struct IClass *cl,Object *obj) {
    DocGroupData *dat=INST_DATA(cl,obj);
    struct MUIS_Listtree_TreeNode *node;
    get(dat->outlineslv,MUIA_Listtree_Active,&node);
    if(node!=MUIV_Listtree_Active_Off)
	DoMethod4(dat->pagesetobj,MYM_PageSet_Action,node->tn_User,0);
    return 0;
}

Static ULONG dgRefresh(struct IClass *cl,Object *obj) {
    DocGroupData *dat=INST_DATA(cl,obj);
    if(dat->outlineslv) {
	ULONG t=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_ExtOutlines);
	BOOL update=FALSE;
	if(!t||!*(int*)t) {
	    if (!dat->balanceobj)
		update=TRUE;
	} else {
	    if (!dat->outlineswin)
		update=TRUE;
	}
	if(update) {
	    DoMethod2(obj,MYM_DocGroup_CloseOutlines);
	    DoMethod2(obj,MYM_DocGroup_OpenOutlines);
	}
    }
    DoMethod2(dat->pagesetobj,MYM_PageSet_Refresh);
    return 0;
}

BEGIN_DISPATCHER(dgDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                    return dgNew    (cl,obj,(APTR)msg);
      case OM_DISPOSE:                return dgDispose(cl,obj,msg);
      case OM_GET:                    return dgGet    (cl,obj,(APTR)msg);
      case OM_SET:                    return dgSet    (cl,obj,(APTR)msg);
      case MYM_DocGroup_Search:       return dgSearch (cl,obj);
      case MYM_DocGroup_Ready:        return dgReady  (cl,obj);
      case MYM_DocGroup_OpenOutlines: return dgOpenOutlines(cl,obj);
      case MYM_DocGroup_GoToOutline:  return dgGoToOutline(cl,obj);
      case MYM_DocGroup_CloseOutlines:return dgCloseOutlines(cl,obj);
      case MYM_DocGroup_Refresh:      return dgRefresh(cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(dgDispatcher)

struct MUI_CustomClass *docgroup_mcc;
static int count;

BOOL createDocGroupClass(void) {
    if(count++==0) {
	if(createPageSliderClass()) {
	    if(createDocScrollClass()) {
		if(createPageSetClass()) {
		    if(docgroup_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(DocGroupData),(APTR)&dgDispatcher))
			return TRUE;
		    deletePageSetClass();
		}
		deleteDocScrollClass();
	    }
	    deletePageSliderClass();
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteDocGroupClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(docgroup_mcc);
	deletePageSetClass();
	deleteDocScrollClass();
	deletePageSliderClass();
    }
    return TRUE;
}

