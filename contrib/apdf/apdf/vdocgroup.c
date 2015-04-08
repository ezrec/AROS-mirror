/*************************************************************************\
 *                                                                       *
 * vdocgroup.c                                                           *
 *                                                                       *
 * Copyright 1999-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <libraries/iffparse.h>
#include <libraries/gadtools.h>
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
#include "pageslider.h"
#include "pageset.h"
#include "prefs.h"
#include "msgs.h"

#define DB(x)   //x
#define Static static


struct VDocGroupData {
    struct comm_info *comm_info;
    Object *doc;
    Object *docchg;
    Object *pageobj;
    Object *docscrollobj;
    Object *pagesetobj;
    Object *menu;
    //Object *linkobj;
};
typedef struct VDocGroupData VDocGroupData;


#define ID_SAVEAS   1
#define ID_COPY     2
#define ID_CROP     3
#define ID_UNCROP   4
#define ID_TEXTAA   5
#define ID_FILLAA   6
#define ID_STROKEAA 7
#define ID_PRINT    8

#define ID_ZOOM     100
static struct NewMenu menudata[]={
    {NM_TITLE,NULL,0,0,0,NULL},
    {NM_ITEM ,NULL,0,0,0,(APTR)ID_SAVEAS},
    {NM_ITEM ,NULL,0,0,0,(APTR)ID_PRINT},
    {NM_ITEM ,NULL,0,0,0,NULL},
    {NM_ITEM ,NULL,0,0,0,(APTR)ID_COPY},
    {NM_ITEM ,NULL,0,0,0,NULL},
    {NM_ITEM ,NULL,0,0,0,(APTR)ID_CROP},
    {NM_ITEM ,NULL,0,0,0,(APTR)ID_UNCROP},
    {NM_ITEM ,NULL,0,0,0,NULL},
    {NM_ITEM ,NULL,0,0,0,NULL},
    {NM_SUB  ,NULL,0,CHECKIT,0x1ffe,(APTR)(ID_ZOOM+0)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1ffd,(APTR)(ID_ZOOM+1)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1ffb,(APTR)(ID_ZOOM+2)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1ff7,(APTR)(ID_ZOOM+3)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1fef,(APTR)(ID_ZOOM+4)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1fdf,(APTR)(ID_ZOOM+5)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1fbf,(APTR)(ID_ZOOM+6)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1f7f,(APTR)(ID_ZOOM+7)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1eff,(APTR)(ID_ZOOM+8)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1dff,(APTR)(ID_ZOOM+9)},
    {NM_SUB  ,NULL,0,CHECKIT,0x1bff,(APTR)(ID_ZOOM+10)},
    {NM_SUB  ,NULL,0,CHECKIT|CHECKED,0x17ff,(APTR)(ID_ZOOM+11)},
    {NM_SUB  ,NULL,0,CHECKIT,0x0fff,(APTR)(ID_ZOOM+12)},
#if HAS_AA
    {NM_ITEM ,NULL,0,CHECKIT|MENUTOGGLE|CHECKED,0,(APTR)ID_TEXTAA},
    {NM_ITEM ,NULL,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_STROKEAA},
    {NM_ITEM ,NULL,0,CHECKIT|MENUTOGGLE,0,(APTR)ID_FILLAA},
#endif
    {NM_END  ,NULL,0,0,0,NULL}
};
static int menulabels[]={
    MSG_PDF_OPERATIONS_MENU,
    MSG_SAVE_FILE_TO_MENU,
    MSG_PRINT_MENU,
    (int)NM_BARLABEL,
    MSG_COPY_MENU,
    (int)NM_BARLABEL,
    MSG_CROP_MENU,
    MSG_UNCROP_MENU,
    (int)NM_BARLABEL,
    MSG_ZOOM_MENU,
    (int)"-5",
    (int)"-4",
    (int)"-3",
    (int)"-2",
    (int)"-1",
    (int)" 0",
    (int)"+1",
    (int)"+2",
    (int)"+3",
    (int)"+4",
    (int)"+5",
    MSG_FIT_PAGE,
    MSG_FIT_WIDTH,
#if HAS_AA
    MSG_TEXT_AA_MENU,
    MSG_STROKE_AA_MENU,
    MSG_FILL_AA_MENU,
#endif
};

/*
 *  DocGroup Methods.
 */

Static ULONG vdgNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    VDocGroupData* dat;
    struct TagItem tags[8];
    Object *pagesetobj,*docscrollobj,*pageobj=NULL;
    Object /**zoomobj,*linkobj,*/*menu;
    Object *backobj=NULL,*fwdobj=NULL;
    Object *doc=(Object*)GetTagData(MYATTR_DocGroup_Document,0,msg->ops_AttrList);
    struct comm_info *ci=(struct comm_info*)GetTagData(MYATTR_DocGroup_CommInfo,0,msg->ops_AttrList);
    struct PDFDoc *pdfdoc;
    int num_pages,numtags,zoom;
    if(!doc || !ci)
	return 0;
    DoMethod2(doc,MYM_Document_IncRef);
    get(doc,MYATTR_Document_NumPages,&num_pages);
    get(doc,MYATTR_Document_PDFDoc,&pdfdoc);

    numtags=0;
    tags[numtags].ti_Tag=MUIA_Group_Child;
    tags[numtags++].ti_Data=(ULONG)(docscrollobj=DocScrollObject,
	MYATTR_DocScroll_CommInfo,ci,
	MYATTR_DocScroll_Contents,pagesetobj=PageSetObject,
	    MYATTR_PageSet_CommInfo,ci,
	    MYATTR_PageSet_Document,doc,
	    MYATTR_PageSet_VGroup,NULL,
	    End,
	End);
    if(num_pages>1) {
	/*tags[numtags].ti_Tag=MUIA_Frame;
	tags[numtags++].ti_Data=MUIV_Frame_Group;
	tags[numtags].ti_Tag=MUIA_Background;
	tags[numtags++].ti_Data=MUII_GroupBack;*/
	tags[numtags].ti_Tag=MUIA_Group_Child;
	tags[numtags++].ti_Data=(ULONG)HGroup,
	    Child,backobj=SimpleButton(STR(MSG_BACK_GAD)),
	    Child,fwdobj=SimpleButton(STR(MSG_FORWARD_GAD)),
	    Child,Label2(STR(MSG_PAGE_GAD)),
	    Child,pageobj=PageSliderObject,
		MYATTR_PageSlider_Document,doc,
		MYATTR_PageSlider_CommInfo,ci,
		MUIA_Weight,1000,
		End,
	    End;
    }
    /*tags[numtags].ti_Tag=MUIA_Group_Child;
    tags[numtags++].ti_Data=(ULONG)HGroup,
	Child,Label2("Zoom"),
	Child,zoomobj=CycleObject,
	    MUIA_Cycle_Entries,zoomEntries,
	    MUIA_Cycle_Active,defZoom,
	    MUIA_CycleChain,TRUE,
	    MUIA_Weight,10,
	    End,
	Child,linkobj=TextObject,
	    TextFrame,
	    MUIA_Background,MUII_TextBack,
	    End,
	End;*/
    tags[numtags].ti_Tag=MUIA_ContextMenu;
    tags[numtags++].ti_Data=(ULONG)(menu=MUI_MakeObject(MUIO_MenustripNM,menudata,0));
    tags[numtags].ti_Tag=TAG_MORE;
    tags[numtags].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	int n;
	Object *tmp;
	dat=INST_DATA(cl,obj);
	dat->comm_info=ci;
	dat->doc=doc;
	dat->docchg=doc;
	dat->pagesetobj=pagesetobj;
	dat->pageobj=pageobj;
	//dat->zoomobj=zoomobj;
	dat->docscrollobj=docscrollobj;
	//dat->linkobj=linkobj;
	dat->menu=menu;

	if(num_pages>1) {
	    DoMethod9(pageobj,MUIM_Notify,MYATTR_PageSlider_Value,MUIV_EveryTime,
		      docscrollobj,3,MUIM_Set,MYATTR_DocScroll_Page,MUIV_TriggerValue);
	    DoMethod9(docscrollobj,MUIM_Notify,MYATTR_DocScroll_Page,MUIV_EveryTime,
		      pageobj,3,MUIM_NoNotifySet,MYATTR_PageSlider_Value,MUIV_TriggerValue);
	    DoMethod7(backobj,MUIM_Notify,MUIA_Selected,FALSE,
		      docscrollobj,1,MYM_DocScroll_Backward);
	    DoMethod7(fwdobj,MUIM_Notify,MUIA_Selected,FALSE,
		      docscrollobj,1,MYM_DocScroll_Forward);
	}
	/*DoMethod9(zoomobj,MUIM_Notify,MUIA_Cycle_Active,MUIV_EveryTime,
		 bitmapobj,3,MUIM_NoNotifySet,MYATTR_DocBitmap_Zoom,MUIV_TriggerValue);
	DoMethod9(bitmapobj,MUIM_Notify,MYATTR_DocBitmap_Zoom,MUIV_EveryTime,
		 zoomobj,3,MUIM_NoNotifySet,MUIA_Cycle_Active,MUIV_TriggerValue);
	DoMethod9(bitmapobj,MUIM_Notify,MYATTR_DocBitmap_Message,MUIV_EveryTime,
		 linkobj,3,MUIM_Set,MUIA_Text_Contents,MUIV_TriggerValue);*/
	DoMethod9(docscrollobj,MUIM_Notify,MYATTR_DocScroll_DocChanged,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocGroup_Document,MUIV_TriggerValue);
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
	/*DoMethod7(obj,MUIM_Notify,MUIA_ContextMenuTrigger,DoMethod(menu,MUIM_FindUData,ID_COPY),
		 bitmapobj,1,MYM_DocBitmap_ToClip);
	DoMethod7(obj,MUIM_Notify,MUIA_ContextMenuTrigger,DoMethod(menu,MUIM_FindUData,ID_CROP),
		 bitmapobj,1,MYM_DocBitmap_Crop);
	DoMethod7(obj,MUIM_Notify,MUIA_ContextMenuTrigger,DoMethod(menu,MUIM_FindUData,ID_UNCROP),
		 bitmapobj,1,MYM_DocBitmap_UnCrop);*/
#if HAS_AA
	n=DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_AntiAliasing);
	if(n)
	    n=*(int*)n;
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_TEXTAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		  pagesetobj,3,MUIM_Set,MYATTR_PageSet_TextAA,MUIV_TriggerValue);
	set(tmp,MUIA_Menuitem_Checked,(n&1)!=0);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_STROKEAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		  pagesetobj,3,MUIM_Set,MYATTR_PageSet_StrokeAA,MUIV_TriggerValue);
	set(tmp,MUIA_Menuitem_Checked,(n&65536)!=0);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_FILLAA);
	DoMethod9(tmp,MUIM_Notify,MUIA_Menuitem_Checked,MUIV_EveryTime,
		  pagesetobj,3,MUIM_Set,MYATTR_PageSet_FillAA,MUIV_TriggerValue);
	set(tmp,MUIA_Menuitem_Checked,(n&256)!=0);
#endif
	get(pagesetobj,MYATTR_PageSet_Zoom,&n);
	tmp=(Object*)DoMethod3(menu,MUIM_FindUData,ID_ZOOM+n);
	nnset(tmp,MUIA_Menuitem_Checked,TRUE);
    } else {
	if(menu)
	    MUI_DisposeObject(menu);
	if(DoMethod2(doc,MYM_Document_DecRef)==0)
	    MUI_DisposeObject(doc);
    }
    msg->ops_AttrList=(struct TagItem*)tags[numtags].ti_Data;
    return (ULONG)obj;
}


Static ULONG vdgDispose(struct IClass *cl,Object *obj,Msg msg) {
    VDocGroupData *dat=INST_DATA(cl,obj);
    if(dat->doc && DoMethod2(dat->doc,MYM_Document_DecRef)==0)
	MUI_DisposeObject(dat->doc);
    MUI_DisposeObject(dat->menu);
    return DoSuperMethodA(cl,obj,msg);
}

/*Static ULONG vdgGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    VDocGroupData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_DocGroup_Document:
	*(Object**)msg->opg_Storage=dat->doc;
	return TRUE;
      case MYATTR_DocGroup_DocChanged:
	*(Object**)msg->opg_Storage=dat->docchg;
	return TRUE;
      case MYATTR_DocGroup_Page:
	get(dat->bitmapobj,MYATTR_DocBitmap_Page,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_Zoom:
	get(dat->bitmapobj,MYATTR_DocBitmap_Zoom,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_Rotate:
	get(dat->bitmapobj,MYATTR_DocBitmap_Rotate,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_CropBox:
	get(dat->bitmapobj,MYATTR_DocBitmap_CropBox,msg->opg_Storage);
	return TRUE;
      case MYATTR_DocGroup_Run:
      case MYATTR_DocGroup_URL:
      case MYATTR_DocGroup_Open:
      case MYATTR_DocGroup_OpenNew:
      case MYATTR_DocGroup_PageNew:
	*msg->opg_Storage=0;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG vdgSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    VDocGroupData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocGroup_DocChanged:
	    dat->docchg=(Object*)tag->ti_Data;
	    break;

	  case MYATTR_DocGroup_Page:
	    set(dat->bitmapobj,MYATTR_DocBitmap_Page,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_Zoom:
	    set(dat->bitmapobj,MYATTR_DocBitmap_Zoom,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_Rotate:
	    set(dat->bitmapobj,MYATTR_DocBitmap_Rotate,tag->ti_Data);
	    break;

	  case MYATTR_DocGroup_CropBox:
	    set(dat->bitmapobj,MYATTR_DocBitmap_CropBox,tag->ti_Data);
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}*/


Static void vdgSaveAs(struct IClass *cl,Object *obj) {
    VDocGroupData *dat=INST_DATA(cl,obj);
    struct Screen *scr=NULL;
    Object *win;
    struct Process *proc=(APTR)FindTask(NULL);
    struct FileRequester *req;
    APTR oldwindowptr=proc->pr_WindowPtr;
    sleep(obj,TRUE);
    get(obj,MUIA_WindowObject,&win);
    if(win) {
	APTR w;
	get(win,MUIA_Window_Screen,&scr);
	get(win,MUIA_Window_Window,&w);
	if(w)
	    proc->pr_WindowPtr=w;
    }
    if((req=MUI_AllocAslRequestTags(ASL_FileRequest,
				    ASLFR_TitleText,STR(MSG_SAVEAS_REQ),
				    ASLFR_PositiveText,STR(MSG_SAVEAS_REQ_POS),
				    ASLFR_NegativeText,STR(MSG_SAVEAS_REQ_NEG),
				    ASLFR_RejectIcons,TRUE,
				    ASLFR_DoSaveMode,TRUE,
				    TAG_END)) &&
	MUI_AslRequestTags(req,
			   scr?ASLFR_Screen:TAG_IGNORE,scr,
			   TAG_END)) {
	BPTR olddir,dir,dir2;
	BPTR dest,src;
	char *name;
	BOOL ok=FALSE;
	if(dir=Lock(req->fr_Drawer,ACCESS_READ)) {
	    get(dat->doc,MYATTR_Document_Dir,&dir2);
	    olddir=CurrentDir(dir);
	    dest=Open(req->fr_File,MODE_NEWFILE);
	    CurrentDir(dir2);
	    UnLock(dir);
	    if(dest) {
		get(dat->doc,MYATTR_Document_Name,&name);
		/* kludge... */
		if(name[0]=='\001' && name[1]==':' && name[2]==':') {
		    unsigned x=0,y=0;
		    int n = 0;
		    name+=3;
		    while (*name != ':') {
			x |= *name++ - '0' << n;
			n += 3;
		    }
		    ++name;
		    n = 0;
		    while (*name != '\0') {
			y |= *name++ - '0' << n;
			n += 3;
		    }
		    ok=Write(dest,(char*)x,y)==y;
		} else if(src=Open(name,MODE_OLDFILE)) {
		    char *buf;
		    const size_t bufsize=65536;
		    if(buf=MyAllocMem(bufsize)) {
			LONG sz;
			do {
			    sz=Read(src,buf,bufsize);
			    if(sz<=0)
				break;
			    ok=Write(dest,buf,sz)==sz;
			} while(ok);
			MyFreeMem(buf);
		    }
		    if(!Close(src))
			ok=FALSE;
		}
		if(!Close(dest))
		    ok=FALSE;
	    }
	    CurrentDir(olddir);
	}
	if(!ok) {
	    Object *app;
	    get(obj,MUIA_ApplicationObject,&app);
	    MUI_RequestA(app,obj,0,(char*)STR(MSG_ERROR_REQ),(char*)STR(MSG_REQ_OK),(char*)STR(MSG_CANT_SAVE_ERR),NULL);
	}
    }
    if(req)
	MUI_FreeAslRequest(req);
    sleep(obj,FALSE);
    proc->pr_WindowPtr=oldwindowptr;
}

Static ULONG vdgContextMenuChoice(struct IClass *cl,Object *obj,struct MUIP_ContextMenuChoice *msg) {
    VDocGroupData *dat=INST_DATA(cl,obj);
    ULONG id;
    get(msg->item,MUIA_UserData,&id);
    if(id>=ID_ZOOM) {
	set(dat->docscrollobj,MYATTR_DocScroll_Zoom,id-ID_ZOOM);
    } else {
	switch(id) {
	  case ID_SAVEAS:
	    vdgSaveAs(cl,obj);
	    break;
	  case ID_PRINT:
	    DoMethod2(dat->doc,MYM_Document_OpenPrintWindow);
	    break;
	  case ID_COPY:
	    DoMethod2(dat->pagesetobj,MYM_PageSet_ToClip);
	    break;
	  case ID_CROP:
	    DoMethod2(dat->pagesetobj,MYM_PageSet_Crop);
	    break;
	  case ID_UNCROP:
	    DoMethod2(dat->pagesetobj,MYM_PageSet_UnCrop);
	    break;
	}
    }
    return 0;
}

BEGIN_DISPATCHER(vdgDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                    return vdgNew    (cl,obj,(APTR)msg);
      case OM_DISPOSE:                return vdgDispose(cl,obj,msg);
      /*case OM_GET:                    return vdgGet    (cl,obj,(APTR)msg);
      case OM_SET:                    return vdgSet    (cl,obj,(APTR)msg);*/
      case MUIM_ContextMenuChoice:    return vdgContextMenuChoice(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(vdgDispatcher)

struct MUI_CustomClass *vdocgroup_mcc;
static int count;

BOOL createVDocGroupClass(void) {
    if(count++==0) {
	{
	    struct NewMenu *p=menudata;
	    int *q=menulabels;
	    while(p->nm_Type!=NM_END) {
		const char *s;
		if(*q!=(int)NM_BARLABEL &&
		   ((int)p->nm_UserData<ID_ZOOM ||
		    (int)p->nm_UserData>ID_ZOOM+10)) {
		    s=getString(*q);
		    if(s[1]=='\0') {
			p->nm_CommKey=(STRPTR)s;
			s+=2;
		    }
		} else
		    s=(const char*)*q;
		p->nm_Label=(STRPTR)s;
		++p; ++q;
	    }
	}
	if(createPageSliderClass()) {
	    if(createDocScrollClass()) {
		if(createPageSetClass()) {
		    if(vdocgroup_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(VDocGroupData),(APTR)&vdgDispatcher))
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

BOOL deleteVDocGroupClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(vdocgroup_mcc);
	deletePageSetClass();
	deleteDocScrollClass();
	deletePageSliderClass();
    }
    return TRUE;
}

