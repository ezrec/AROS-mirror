/*************************************************************************\
 *                                                                       *
 * simpledoc.c                                                           *
 *                                                                       *
 * Copyright 1999-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <dos/dostags.h>
#include <exec/nodes.h>
#include <exec/execbase.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include "AComm.h"
#include "Apdf.h"
#include "document.h"
#include "docbitmap.h"
#include "pageslider.h"
#include "prefs.h"
#include "msgs.h"

#define DB(x)   //x
#define Static static

#define kprintf dprintf

#ifndef MAKE_ID
#   define MAKE_ID(a,b,c,d) ((ULONG)(a)<<24 | (ULONG)(b)<<16 | \
			     (ULONG)(c)<<8 | (ULONG)(d))
#endif

struct Screen *get_screen(Object* app) {
    struct Screen *scr=NULL;
    APTR state;
    struct List *list;
    Object* w;
    get(app,MUIA_Application_WindowList,&list);
    state=list->lh_Head;
    while(!scr && (w=NextObject(&state)))
	get(w,MUIA_Window_Screen,&scr);
    return scr;
}

struct SimpleDocData {
    struct comm_info *comm_info;
    BPTR dir;
    char *name;
    struct PDFDoc *pdfdoc;
    int num_pages;
    int open_page;
    int open_zoom;
    int open_rotate;
    struct MYS_DocBitmap_CropBox open_box;
    Object *app;
    Object *pswin;
    Object *psfilenameobj;
    Object *pstypeobj;
    Object *psstartobj;
    Object *psstopobj;
    Object *psrotateobj;
    Object *pszoomobj;
    Object *pspopobj;
    Object *pssidesobj;
};
typedef struct SimpleDocData SimpleDocData;

extern Object *config;

Object *create_gauge(Object *app,const char *title,int n) {
    Object *win,*gauge,*abortobj;
    set(app,MUIA_Application_Sleep,TRUE);
    win=WindowObject,
	MUIA_Window_ID,MAKE_ID('P','R','G','S'),
	MUIA_Window_CloseGadget,FALSE,
	MUIA_Window_Title,title,
	MUIA_Window_Screen,get_screen(app),
	MUIA_Window_IsSubWindow,TRUE,
	WindowContents,VGroup,
	    Child,gauge=GaugeObject,
		GaugeFrame,
		MUIA_Gauge_Horiz,TRUE,
		MUIA_Gauge_Max,n,
		End,
	    Child,HGroup,
		Child,RectangleObject,
		    End,
		Child,abortobj=SimpleButton(STR(MSG_ABORT_GAD)),
		Child,RectangleObject,
		    End,
		End,
	    End,
	End;
    if(win) {
	DoMethod8(abortobj,MUIM_Notify,MUIA_Selected,FALSE,
		 app,2,MUIM_Application_ReturnID,ID_ABORT);
	DoMethod3(app,OM_ADDMEMBER,win);
	set(win,MUIA_Window_Open,TRUE);
    } else
	gauge=NULL;
    return gauge;
}

void delete_gauge(Object *gauge) {
    if(gauge) {
	Object *app,*win;
	get(gauge,MUIA_ApplicationObject,&app);
	get(gauge,MUIA_WindowObject,&win);
	set(win,MUIA_Window_Open,FALSE);
	DoMethod3(app,OM_REMMEMBER,win);
	MUI_DisposeObject(win);
	set(app,MUIA_Application_Sleep,FALSE);
    }
}

/*
 *  Server interface functions
 */

int get_page_index(struct comm_info *ci,struct PDFDoc *doc,const char *lab);

Static struct PDFDoc* create_doc(struct comm_info *ci,BPTR dir,const char *filename,
		int *num_pages,const char *ownerpw,const char *userpw) {
    struct msg_createdoc* p=ci->cmddata;
    sync();
    p->dir=dir;
    strncpy(p->filename,filename,sizeof(p->filename)-1);
    p->filename[sizeof(p->filename)-1]='\0';
    strncpy(p->ownerpw,ownerpw,sizeof(p->ownerpw)-1);
    p->ownerpw[sizeof(p->ownerpw)-1]='\0';
    strncpy(p->userpw,userpw,sizeof(p->userpw)-1);
    p->userpw[sizeof(p->userpw)-1]='\0';
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_CREATEDOC)) {
	*num_pages=p->num_pages;
	return p->doc;
    }
    *num_pages=p->num_pages;
    return NULL;
}

Static void delete_doc(struct comm_info *ci,struct PDFDoc* doc) {
    struct msg_deletedoc* p=ci->cmddata;
    sync();
    p->doc=doc;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_DELETEDOC);
}

Static struct OutputDev *init_write(struct comm_info *ci,struct PDFDoc *doc,const char *filename,int type,int first_page,int last_page,int zoom,int rotate) {
    struct msg_write *p=ci->cmddata;
    sync();
    p->doc=doc;
    strncpy(p->filename,filename,sizeof(p->filename)-1);
    p->filename[sizeof(p->filename)-1]='\0';
    p->type=type;
    p->first_page=first_page;
    p->last_page=last_page;
    p->zoom=zoom;
    p->rotate=rotate;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_INITWRITE))
	return p->savedev;
    return NULL;
}

Static int writefile(struct comm_info *ci,struct PDFDoc *doc,struct OutputDev *sv,int first_page,int last_page,int zoom,int rotate) {
    struct msg_write *p=ci->cmddata;
    sync();
    p->doc=doc;
    p->savedev=sv;
    p->first_page=first_page;
    p->last_page=last_page;
    p->zoom=zoom;
    p->rotate=rotate;
    return (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_WRITE);
}

Static void end_write(struct comm_info *ci,struct OutputDev *sv) {
    struct msg_write* p=ci->cmddata;
    sync();
    p->savedev=sv;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_ENDWRITE);
}


/*
 *  Hooks.
 */

BEGIN_STATIC_HOOK(ULONG,start,
		  struct Hook *,h,
		  Object *,obj,
		  Object **,p) {
    int start,stop;
    get(obj,MYATTR_PageSlider_Value,&start);
    get(*p,MYATTR_PageSlider_Value,&stop);
    if(start>stop)
	set(*p,MYATTR_PageSlider_Value,start);
    return 0;
}
END_STATIC_HOOK(start)

BEGIN_STATIC_HOOK(ULONG,stop,
		  struct Hook *,h,
		  Object *,obj,
		  Object **,p) {
    int start,stop;
    get(obj,MYATTR_PageSlider_Value,&stop);
    get(*p,MYATTR_PageSlider_Value,&start);
    if(start>stop)
	set(*p,MYATTR_PageSlider_Value,stop);
    return 0;
}
END_STATIC_HOOK(stop)

BEGIN_STATIC_HOOK(ULONG,psmodechg,
		  struct Hook *,h,
		  Object *,obj,
		  Object **,p) {
    int mode;
    get(obj,MUIA_Cycle_Active,&mode);
    set(p[0],MUIA_Disabled,mode>1);
    set(p[1],MUIA_Disabled,mode>1);
    return 0;
}
END_STATIC_HOOK(psmodechg)

/*
 *  Custom rotate slider class: displays multiples of 90°
 */

/*struct RotateSliderData {
    char buf[5];
};
typedef struct RotateSliderData RotateSliderData;*/

BEGIN_DISPATCHER(mrDispatcher,cl,obj,msg) {
    if(msg->MethodID==MUIM_Numeric_Stringify) {
	//RotateSliderData *dat=INST_DATA(cl,obj);
	struct MUIP_Numeric_Stringify *m=(APTR)msg;
	static const char *strs[]={"0°","90°","180°","270°"};
	return (ULONG)strs[m->value&3]; /* &3 should be useless, but... */
    } else
	return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(mrDispatcher)

/*
 *  Document Methods.
 */

Static ULONG sdocNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    SimpleDocData *dat;
    BPTR dir=GetTagData(MYATTR_Document_Dir,0,msg->ops_AttrList);
    const char *name=(const char *)GetTagData(MYATTR_Document_Name,0,msg->ops_AttrList);
    const char *ownerpw=(const char *)GetTagData(MYATTR_Document_OwnerPW,(LONG)"",msg->ops_AttrList);
    const char *userpw=(const char *)GetTagData(MYATTR_Document_UserPW,(LONG)"",msg->ops_AttrList);
    LONG *need_password=(LONG *)GetTagData(MYATTR_Document_NeedPassword,0,msg->ops_AttrList);
    struct comm_info *ci=(struct comm_info *)GetTagData(MYATTR_Document_CommInfo,0,msg->ops_AttrList);
    if(need_password)
	*need_password=FALSE;
    if(!name || !ci)
	return 0;
    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	ULONG d;
	dat=INST_DATA(cl,obj);
	dat->comm_info=ci;
	dat->dir=DupLock(dir);
	dat->name=NULL;
	CopyStr(&dat->name,name);
	dat->pdfdoc=create_doc(ci,dir,name,&dat->num_pages,ownerpw,userpw);
	dat->open_page=1;
	d=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_Zoom);
	dat->open_zoom=d?*(int*)d:defZoom;
	dat->open_rotate=0;
	dat->open_box.left=0;
	dat->open_box.top=0;
	dat->open_box.right=0;
	dat->open_box.bottom=0;
	dat->pswin=NULL;
	dat->app=(Object*)GetTagData(MYATTR_Document_Application,0,msg->ops_AttrList);
	if(!dat->pdfdoc || !dat->name) {
	    if(need_password && dat->name && !dat->pdfdoc && dat->num_pages==-1)
		*need_password=TRUE;
	    CoerceMethod(cl,obj,OM_DISPOSE);
	    obj=NULL;
	}
    }
    return (ULONG)obj;
}

Static ULONG sdocDispose(struct IClass *cl,Object *obj,Msg msg) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    if(dat->pswin) {
	set(dat->pswin,MUIA_Window_Open,FALSE);
	DoMethod3(dat->app,OM_REMMEMBER,dat->pswin);
	MUI_DisposeObject(dat->pswin);
    }
    delete_doc(dat->comm_info,dat->pdfdoc);
    MyFreeMem(dat->name);
    UnLock(dat->dir);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG sdocSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_Document_OpenPage:
	    DB(kprintf("SimpleDoc::set openpage %d num_pages %d\n",tag->ti_Data,dat->num_pages);)
	    if(tag->ti_Data>=1 && tag->ti_Data<=dat->num_pages)
		dat->open_page=tag->ti_Data;
	    break;
	  case MYATTR_Document_OpenPageLabel: {
	    int p=get_page_index(dat->comm_info,dat->pdfdoc,(const char*)tag->ti_Data);
	    DB(kprintf("SimpleDoc::set openpagelabel <%s> (%d) num_pages %d\n",tag->ti_Data,p,dat->num_pages);)
	    if(p>=1 && p<=dat->num_pages)
		dat->open_page=p;
	    break;
	  }
	  case MYATTR_Document_OpenZoom:
	    if(tag->ti_Data>=minZoom && tag->ti_Data<=maxZoom)
		dat->open_zoom=tag->ti_Data;
	    break;
	  case MYATTR_Document_OpenRotate:
	    if(tag->ti_Data==0 || tag->ti_Data==90 ||
	       tag->ti_Data==180 || tag->ti_Data==270)
		dat->open_rotate=tag->ti_Data;
	    break;
	  case MYATTR_Document_OpenCropBox:
	    if(tag->ti_Data)
		dat->open_box=*(struct MYS_DocBitmap_CropBox *)tag->ti_Data;
	    else {
		dat->open_box.left=0;
		dat->open_box.top=0;
		dat->open_box.right=0;
		dat->open_box.bottom=0;
	    }
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG sdocGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_Document_Name:
	*msg->opg_Storage=(ULONG)dat->name;
	return TRUE;
      case MYATTR_Document_Dir:
	*msg->opg_Storage=(ULONG)dat->dir;
	return TRUE;
      case MYATTR_Document_NumPages:
	*msg->opg_Storage=dat->num_pages;
	return TRUE;
      case MYATTR_Document_PDFDoc:
	*msg->opg_Storage=(ULONG)dat->pdfdoc;
	return TRUE;
      case MYATTR_Document_OpenPage:
	*msg->opg_Storage=dat->open_page;
	return TRUE;
      case MYATTR_Document_OpenZoom:
	*msg->opg_Storage=dat->open_zoom;
	return TRUE;
      case MYATTR_Document_OpenRotate:
	*msg->opg_Storage=dat->open_rotate;
	return TRUE;
      case MYATTR_Document_OpenCropBox:
	*(struct MYS_DocBitmap_CropBox *)msg->opg_Storage=dat->open_box;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static BOOL sdocCreateSaveWindow(struct IClass *cl,Object *obj) {
    LONG x;
    SimpleDocData *dat=INST_DATA(cl,obj);
    if(!dat->pswin && dat->app) {
	Object *pssaveobj,*pscancelobj;
	static const char *save_types[6];
	static const char *sides_choices[4];
	//int num_pages;
	save_types[0]=STR(MSG_SAVE_MODE_PS_LEV2);
	save_types[1]=STR(MSG_SAVE_MODE_PS_LEV1);
	save_types[2]=STR(MSG_SAVE_MODE_TEXT);
	save_types[3]=STR(MSG_SAVE_MODE_PBM_PPM_IMAGES);
	save_types[4]=STR(MSG_SAVE_MODE_PBM_PPM_JPEG_IMAGES);
	sides_choices[0]=STR(MSG_SIDES_BOTH);
	sides_choices[1]=STR(MSG_SIDES_EVEN);
	sides_choices[2]=STR(MSG_SIDES_ODD);
	//get(obj,MYATTR_Document_NumPages,&num_pages);
	dat->pswin=WindowObject,
	    MUIA_Window_Title,STR(MSG_SAVE_WIN),
	    MUIA_Window_ID,MAKE_ID('S','A','V','E'),
	    MUIA_Window_Screen,get_screen(dat->app),
	    MUIA_Window_IsSubWindow,TRUE,
	    MUIA_HelpNode,"savewin",
	    WindowContents,VGroup,
		Child,ColGroup(2),
		    Child,Label2(STR(MSG_SAVE_FILE_GAD)),
		    Child,dat->pspopobj=PopaslObject,
			MUIA_ShortHelp,STR(MSG_SAVE_FILE_HELP),
			MUIA_Popasl_Type,ASL_FileRequest,
			MUIA_Popstring_String,dat->psfilenameobj=StringObject,
			    StringFrame,
			    MUIA_String_MaxLen,255,
			    MUIA_String_AdvanceOnCR,TRUE,
			    MUIA_CycleChain,TRUE,
			    End,
			MUIA_Popstring_Button,PopButton(MUII_PopFile),
			ASLFR_TitleText,STR(MSG_SELECT_FILE_NAME_REQ),
			End,
		    Child,Label2(STR(MSG_SAVE_MODE_GAD)),
		    Child,dat->pstypeobj=CycleObject,
			MUIA_Cycle_Entries,save_types,
			MUIA_CycleChain,TRUE,
			MUIA_ShortHelp,STR(MSG_SAVE_MODE_HELP),
			End,
		    Child,Label2(STR(MSG_FIRST_PAGE_GAD)),
		    Child,dat->psstartobj=PageSliderObject,
			MYATTR_PageSlider_Document,obj,
			MYATTR_PageSlider_CommInfo,dat->comm_info,
			MYATTR_PageSlider_Value,1,
			End,
		    Child,Label2(STR(MSG_LAST_PAGE_GAD)),
		    Child,dat->psstopobj=PageSliderObject,
			MYATTR_PageSlider_Document,obj,
			MYATTR_PageSlider_CommInfo,dat->comm_info,
			MYATTR_PageSlider_Value,1,
			End,
		    Child,Label2(STR(MSG_SIDES_GAD)),
		    Child,dat->pssidesobj=CycleObject,
			MUIA_Cycle_Entries,sides_choices,
			MUIA_CycleChain,TRUE,
			MUIA_ShortHelp,STR(MSG_SIDES_HELP),
			End,
		    Child,Label2(STR(MSG_SAVE_ZOOM_GAD)),
		    Child,dat->pszoomobj=SliderObject,
			SliderFrame,
			MUIA_Background,MUII_SliderBack,
			MUIA_Slider_Horiz,TRUE,
			MUIA_Numeric_Min,-5,
			MUIA_Numeric_Max,5,
			MUIA_Numeric_Value,0,
			MUIA_CycleChain,TRUE,
			End,
		    Child,Label2(STR(MSG_SAVE_ROTATE_GAD)),
		    Child,dat->psrotateobj=MyRotateSliderObject,
			SliderFrame,
			MUIA_Background,MUII_SliderBack,
			MUIA_Slider_Horiz,TRUE,
			MUIA_Numeric_Min,0,
			MUIA_Numeric_Max,3,
			MUIA_Numeric_Value,0,
			MUIA_CycleChain,TRUE,
			End,
		    End,
		Child,HGroup,
		    Child,pssaveobj=SimpleButton(STR(MSG_SAVE_SAVE_GAD)),
		    Child,pscancelobj=SimpleButton(STR(MSG_SAVE_CANCEL_GAD)),
		    End,
		End,
	    End;

	if(!dat->pswin)
	    return FALSE;

	DoMethod3(dat->app,OM_ADDMEMBER,dat->pswin);

	DoMethod9(dat->pswin,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
		 dat->pswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	DoMethod9(dat->psstartobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		 dat->psstartobj,3,MUIM_CallHook,&start_hook,dat->psstopobj);
	DoMethod9(dat->psstopobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		 dat->psstopobj,3,MUIM_CallHook,&stop_hook,dat->psstartobj);
	DoMethod9(pscancelobj,MUIM_Notify,MUIA_Pressed,FALSE,
		 dat->pswin,3,MUIM_Set,MUIA_Window_Open,FALSE);
	DoMethod7(pssaveobj,MUIM_Notify,MUIA_Pressed,FALSE,
		 obj,1,MYM_Document_Save);
	DoMethod10(dat->pstypeobj,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
		 obj,4,MUIM_CallHook,&psmodechg_hook,dat->pszoomobj,dat->psrotateobj);
    }
    /*get(dat->pageobj,MYATTR_PageSlider_Value,&x);
    set(dat->psstartobj,MYATTR_PageSlider_Value,x);
    set(dat->psstopobj,MYATTR_PageSlider_Value,x);*/
    set(dat->psrotateobj,MUIA_Numeric_Value,0);
    set(dat->pswin,MUIA_Window_ActiveObject,dat->psfilenameobj);
    return TRUE;
}

Static ULONG sdocOpenSaveWindow(struct IClass *cl,Object *obj) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    if(sdocCreateSaveWindow(cl,obj))
	set(dat->pswin,MUIA_Window_Open,TRUE);
    return 0;
}

Static ULONG sdocOpenPrintWindow(struct IClass *cl,Object *obj) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    if(sdocCreateSaveWindow(cl,obj)) {
	set(dat->psfilenameobj,MUIA_String_Contents,get_prt_dev());
	set(dat->pstypeobj,MUIA_Cycle_Active,0);
	set(dat->pswin,MUIA_Window_Open,TRUE);
    }
    return 0;
}


Static ULONG sdocSave(struct IClass *cl,Object *obj) {
    SimpleDocData *dat=INST_DATA(cl,obj);
    const char* filename;
    LONG type,first_page,last_page,zoom,rotate,sides;
    int k,l=0;
    ULONG sigs=0;
    const char *ret="Failed !";
    struct OutputDev *sv;
    LONG active;
    Object *gauge;

    if(!dat->app)
	return 0;
    get(dat->psfilenameobj,MUIA_String_Contents,&filename);
    get(dat->pstypeobj,MUIA_Cycle_Active,&type);
    get(dat->psstartobj,MYATTR_PageSlider_Value,&first_page);
    get(dat->psstopobj,MYATTR_PageSlider_Value,&last_page);
    get(dat->pszoomobj,MUIA_Numeric_Value,&zoom);
    get(dat->psrotateobj,MUIA_Numeric_Value,&rotate);
    get(dat->pspopobj,MUIA_Popasl_Active,&active);
    get(dat->pssidesobj,MUIA_Cycle_Active,&sides);

    zoom=zoomDPI[zoom+5];
    rotate*=90;
    if(active || !filename || last_page<first_page) {
	DisplayBeep(NULL);
	return 0;
    }

    gauge=create_gauge(dat->app,STR(MSG_SAVING),last_page-first_page+1);

    if(sv=init_write(dat->comm_info,dat->pdfdoc,filename,type,first_page,last_page,zoom,rotate)) {
	LONG r;
	ret=STR(MSG_SAVED);
	for(k=first_page;k<=last_page;++k) {
	    if(sides!=0 && (k&1)!=sides-1)
		continue;
	    if(!writefile(dat->comm_info,dat->pdfdoc,sv,k,k+1,zoom,rotate))
		ret=STR(MSG_SAVE_ERR);
	    if(gauge) {
		set(gauge,MUIA_Gauge_Current,++l);
		r=DoMethod3(dat->app,MUIM_Application_NewInput,&sigs);
		if(r==MUIV_Application_ReturnID_Quit ||
		   r==ID_ABORT) {
		    ret=STR(MSG_ABORTED);
		    break;
		}
	    }
	}
	end_write(dat->comm_info,sv);
    }
    set(dat->pswin,MUIA_Window_Open,FALSE);
    delete_gauge(gauge);
    return (ULONG)ret;
}

BEGIN_DISPATCHER(sdocDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                       return sdocNew      (cl,obj,(APTR)msg);
      case OM_DISPOSE:                   return sdocDispose  (cl,obj,msg);
      case OM_GET:                       return sdocGet      (cl,obj,(APTR)msg);
      case OM_SET:                       return sdocSet      (cl,obj,(APTR)msg);
      case MYM_Document_OpenPrintWindow: return sdocOpenPrintWindow(cl,obj);
      case MYM_Document_OpenSaveWindow:  return sdocOpenSaveWindow (cl,obj);
      case MYM_Document_Save:            return sdocSave     (cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(sdocDispatcher)

struct MUI_CustomClass *simpledoc_mcc;
struct MUI_CustomClass *rotate_slider_mcc;

static int count;

BOOL createSimpleDocClass(void) {
    if(count++==0) {
	if(rotate_slider_mcc=MUI_CreateCustomClass(NULL,MUIC_Slider,NULL,0/*sizeof(RotateSliderData)*/,(APTR)&mrDispatcher)) {
	    if(simpledoc_mcc=MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(SimpleDocData),(APTR)&sdocDispatcher))
		return TRUE;
	    MUI_DeleteCustomClass(rotate_slider_mcc);
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteSimpleDocClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(simpledoc_mcc);
	MUI_DeleteCustomClass(rotate_slider_mcc);
    }
    return TRUE;
}

