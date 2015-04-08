/*************************************************************************\
 *                                                                       *
 * docscroll.c                                                           *
 *                                                                       *
 * Copyright 2000-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
#include <devices/rawkeycodes.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#include "Apdf.h"
#include "docscroll.h"
#include "pageset.h"
#include "docbitmap.h"
#include "document.h"

#define DB(x)       //x
#define Static      static
#define MEMDBG(x)   //x

#define kprintf dprintf

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

#define BROKEN_KILLNOTIFY

#define SPACING  0
#define SPACING2 4

#define historySize 50
#define numBookmarks 10

struct History {
    int page;
    BPTR dir;
    char *name;
};
typedef struct History History;

struct page_info {
    int width;
    int height;
};

struct DocScrollData {
    struct comm_info *comm_info;
    struct page_info *pages;
    int num_pages;
    int page;
    int total_width;
    int total_height;
    int average_width;
    int average_height;
    int max_width;
    int max_height;
    int columns;
    int orig_rows;
    int skip;
    Object *doc;
    Object *pageset;
    Object *pgroup;
    Object *fgroup;
    Object *vgroup;
    Object *hscroll;
    Object *vscroll;
    Object *rect;
    enum ViewMode mode;
    int quiet;
    BOOL has_hscroll;
    BOOL has_vscroll;
    int zoom;
    int dpi;
    Object *docchanged;
    int histCur;
    int histTot;
    History history[historySize];
    History bookmark[numBookmarks];
    struct MUI_EventHandlerNode ehn;
};

typedef struct DocScrollData DocScrollData;


/*
 *  Virtual group class that notifies its dimensions changes.
 */

struct VGroupData {
    int width;
    int height;
    int vwidth;
    int vheight;
};
typedef struct VGroupData VGroupData;

Static ULONG vgNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    if(obj=(Object*)DoSuperMethodA(cl,obj,(APTR)msg)) {
	VGroupData* dat=INST_DATA(cl,obj);
	memset(dat,0,sizeof(*dat));
    }
    return (ULONG)obj;
}

Static ULONG vgGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    if(msg->opg_AttrID==MYATTR_Virtgroup_GeometryChange) {
	*msg->opg_Storage=0;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(APTR)msg);
}

Static ULONG vgDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg) {
    VGroupData *dat=INST_DATA(cl,obj);
    int w,h,w1,h1;
    w1=_width(obj);
    h1=_height(obj);
    get(obj,MUIA_Virtgroup_Width,&w);
    get(obj,MUIA_Virtgroup_Height,&h);
    if(w1!=dat->width || h1!=dat->height || w!=dat->vwidth || h!=dat->vheight) {
	dat->vwidth=w;
	dat->vheight=h;
	dat->width=w1;
	dat->height=h1;
	set(obj,MYATTR_Virtgroup_GeometryChange,TRUE);
    }
    return DoSuperMethodA(cl,obj,(APTR)msg);
}

#if 0
Static ULONG vgHandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg) {
    /* Don't pass middle button events to the superclass */
    /* We could use MUIA_Virtgroup_Input=FALSE, but then mouse events aren't
       'clipped' to the virtual frame */
    if(msg->imsg && msg->imsg->Class==IDCMP_MOUSEBUTTONS &&
       (msg->imsg->Code==MIDDLEDOWN || msg->imsg->Code==MIDDLEUP)) {
	struct List *list;
	APTR state;
	Object *o;
	get(obj,MUIA_Group_ChildList,&list);
	state=list->lh_Head;
	while(o=NextObject(&state))
	    DoMethodA(o,(APTR)msg);
	return 0;
    }
    return DoSuperMethodA(cl,obj,msg);
}
#else
/* Only called with MUI 3.8- */
Static ULONG vgHandleInput(struct IClass *cl,Object *obj,struct MUIP_HandleInput *msg) {
    /* Don't pass mouse events outside the clip box to the children */
    if(msg->imsg && msg->imsg->Class==IDCMP_MOUSEBUTTONS &&
       (msg->imsg->Code==SELECTDOWN || msg->imsg->Code==MIDDLEDOWN ||
	msg->imsg->Code==MENUDOWN) &&
       (msg->imsg->MouseX<_mleft(obj) || msg->imsg->MouseX>=_mright(obj) ||
	msg->imsg->MouseY<_mtop(obj) || msg->imsg->MouseY>=_mbottom(obj)))
	return 0;
    return DoSuperMethodA(cl,obj,msg);
}
#endif

BEGIN_DISPATCHER(vgDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:              return vgNew(cl,obj,(APTR)msg);
      //case OM_SET:              return vgSet(cl,obj,(APTR)msg);
      case OM_GET:              return vgGet(cl,obj,(APTR)msg);
      case MUIM_Draw:           return vgDraw(cl,obj,(APTR)msg);
      case MUIM_HandleInput:    if(is38) return vgHandleInput(cl,obj,(APTR)msg); break;
    }
    return DoSuperMethodA(cl,obj,(APTR)msg);
}
END_DISPATCHER(vgDispatcher)

struct MUI_CustomClass *vgroup_mcc;

Static void update_sizes(struct IClass *cl,Object *obj);

Static void update_dpi(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    int dpi=dat->dpi==-1?0x7fffffff:dat->dpi;
    struct List *list;
    APTR state;
    Object *o;
    static in_update_dpi;
    if(in_update_dpi)
	return;
    ++in_update_dpi;
    DB(kprintf("update_dpi() dpi=%ld\n",dat->dpi);)
    get(dat->pageset,MUIA_Group_ChildList,&list);
    state=list->lh_Head;
    while(o=NextObject(&state)) {
	int dpi1;
	get(o,MYATTR_DocBitmap_DPI,&dpi1);
	DB(kprintf("o=%lx, dpi=%ld\n",o,dpi1);)
	if(dpi1>0 && dpi1<dpi)
	    dpi=dpi1;
    }
    DB(kprintf("-> dpi=%ld\n",dpi);)
    if(dpi!=dat->dpi) {
	struct page_info *pages=dat->pages;
	dat->dpi=dpi;
	set(dat->pageset,MYATTR_PageSet_DPI,dpi);
	if(pages) {
	    int k;
	    for(k=0;k<dat->num_pages;++k) {
		pages[k].width=-1;
		pages[k].height=-1;
	    }
	    update_sizes(cl,obj);
	}
    }
    --in_update_dpi;
}

Static void calc_sizes(DocScrollData *dat) {
    int k;
    int total_height=0,num=0,max_width=1,num2=0;
    struct page_info *pages=dat->pages;
    if(!pages)
	return;
    get(dat->pageset,MYATTR_PageSet_Columns,&dat->columns);
    dat->skip=!(dat->columns&1);
    for(k=-dat->skip;k<dat->num_pages;) {
	int i;
	int max_height=0;
	int total_width=0;
	for(i=0;i<dat->columns && k<dat->num_pages;++i) {
	    if(k>=0 && pages[k].width>0) {
		total_width+=pages[k].width;
		if(pages[k].height>max_height)
		    max_height=pages[k].height;
	    }
	    ++k;
	}
	if(max_height) {
	    total_height+=max_height;
	    ++num;
	} else
	    ++num2;
	if(total_width>max_width)
	    max_width=total_width;
    }
    if(num) {
	dat->average_height=total_height/num;
	dat->total_height=total_height+dat->average_height*num2+SPACING*(num+num2-1);
    } else {
	dat->average_height=1;
	dat->total_height=1;
    }
    dat->average_width=1;
    dat->total_width=1;
    dat->max_width=max_width;
    DB(kprintf("num=%ld, num2=%ld, num_pages=%ld, skip=%ld\n"
	       "total_width=%ld, max_width=%ld, avg_width=%ld\n"
	       "total_height=%ld, max_height=%ld, avg_height=%ld\n",
	       num,num2,dat->num_pages,dat->skip,
	       dat->total_width,dat->max_width,dat->average_width,
	       dat->total_height,dat->max_height,dat->average_height);)
    set(dat->hscroll,MUIA_Prop_Entries,dat->max_width);
    set(dat->vscroll,MUIA_Prop_Entries,dat->total_height);
}

Static void update_sizes(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    BOOL change=FALSE;
    APTR state;
    struct List *list;
    Object *o;
    //DB(kprintf("update_sizes: num_pages=%ld\n",dat->num_pages);)
    if(dat->mode==vmContinuous &&
       (dat->zoom==zoomPage || dat->zoom==zoomWidth))
       update_dpi(cl,obj);
    if(!dat->pages)
	return;
    get(dat->pageset,MUIA_Group_ChildList,&list);
    state=list->lh_Head;
    while(o=NextObject(&state)) {
	int page,width,height;
	get(o,MYATTR_DocBitmap_Page,&page);
	//DB(kprintf("o=%lx, page=%ld\n",o,page);)
	--page;
	if(page<dat->num_pages) {
	    get(o,MYATTR_DocBitmap_Width,&width);
	    get(o,MYATTR_DocBitmap_Height,&height);
	    //DB(kprintf("width=%ld, height=%ld\n",width,height);)
	    if(dat->pages[page].width!=width ||
	       dat->pages[page].height!=height) {
		change=TRUE;
		dat->pages[page].width=width;
		dat->pages[page].height=height;
	    }
	}
    }
    if(change)
	calc_sizes(dat);
}


Static ULONG dsSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    BOOL no_history=FALSE;

    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocScroll_NoHistory:
	    no_history=tag->ti_Data;
	    break;

	  case MYATTR_DocScroll_DocChanged:
	    dat->docchanged=(Object *)tag->ti_Data;
	    break;

	  case MYATTR_DocScroll_Document:
	    if(tag->ti_Data!=(LONG)dat->doc) {
		struct MYS_DocBitmap_CropBox box;
		ULONG page,zoom,rotate,textaa,fillaa,strokeaa,page1;
		get((Object*)tag->ti_Data,MYATTR_Document_NumPages,&dat->num_pages);
		get((Object*)tag->ti_Data,MYATTR_Document_OpenPage,&page);
		if(!no_history && dat->doc) {
		    const char *name;
		    BPTR dir;
		    get(dat->doc,MYATTR_Document_Dir,&dir);
		    get(dat->doc,MYATTR_Document_Name,&name);
		    DoMethod5(obj,MYM_DocScroll_HistoryPush,dat->page,dir,name);
		}
		set(obj,MYATTR_DocScroll_Quiet,TRUE);
		dat->doc=(Object *)tag->ti_Data;
		DB(kprintf("DocScroll: open page %d\n",page);)
		page1=GetTagData(MYATTR_DocScroll_Page,-1,msg->ops_AttrList);
		if(page1!=-1)
		  page=page1;
		get(dat->doc,MYATTR_Document_OpenZoom,&zoom);
		get(dat->doc,MYATTR_Document_OpenRotate,&rotate);
		get(dat->doc,MYATTR_Document_OpenCropBox,&box);
#if HAS_AA
		get(dat->doc,MYATTR_Document_OpenTextAA,&textaa);
		get(dat->doc,MYATTR_Document_OpenFillAA,&fillaa);
		get(dat->doc,MYATTR_Document_OpenStrokeAA,&strokeaa);
#endif
		DB(kprintf("DocScroll: set document. openzoom %d openpage %d\n",zoom,page);)
		dat->page=page;
		DB(kprintf("DocScroll: set pageset params\n"));
		SetAttrs(dat->pageset,
			 MYATTR_PageSet_Document,dat->doc,
			 MYATTR_PageSet_Page,page,
			 MYATTR_PageSet_Zoom,zoom,
			 MYATTR_PageSet_Rotate,rotate,
			 MYATTR_PageSet_CropBox,&box,
			 AA(MYATTR_PageSet_TextAA,textaa,)
			 AA(MYATTR_PageSet_FillAA,fillaa,)
			 AA(MYATTR_PageSet_StrokeAA,strokeaa,)
			 TAG_END);
		DB(kprintf("DocScroll: set self params\n"));
		SetAttrs(obj,
			 MYATTR_DocScroll_NoHistory,TRUE,
			 MYATTR_DocScroll_Page,page,
			 MYATTR_DocScroll_Zoom,zoom,
			 TAG_END);
		set(obj,MYATTR_DocScroll_DocChanged,dat->doc);
		DoMethod2(obj,MYM_DocScroll_ResetData);
		set(obj,MYATTR_DocScroll_Quiet,FALSE);
	    }
	    break;

	  case MYATTR_DocScroll_Page: {
	    int p=tag->ti_Data;
	    if(p==-1)
	      break;
	    if(p<1)
		p=1;
	    else if(p>dat->num_pages)
		p=dat->num_pages;
	    if(p!=dat->page) {
		DB(kprintf("docscroll: set(page,%ld)\n",p);)
		if(!no_history && dat->doc)
		    DoMethod5(obj,MYM_DocScroll_HistoryPush,dat->page,0,NULL);
		if(dat->mode==vmContinuous) {
		    int k;
		    int t=0;
		    int q;
		    struct page_info *pages=dat->pages;
		    //set(dat->pageset,MYATTR_PageSet_Page,p);
		    --p;
		    if(dat->pages) {
			//set(dat->vgroup,MUIA_Virtgroup_Top,0);
			for(k=-dat->skip;k+dat->columns<p;) {
			    int i,max_height=0;
			    for(i=0;i<dat->columns && k<dat->num_pages;++i) {
				if(k>=0) {
				    int h=pages[k].height<0 ? dat->average_height : pages[k].height;
				    if(h>max_height)
					max_height=h;
				}
				++k;
			    }
			    t+=max_height+SPACING;
			}
			++p;
			dat->page=p;
			DB(kprintf("set_page(%ld) -> top=%ld\n",dat->page,t);)
			set(obj,MYATTR_DocScroll_Quiet,TRUE);
			nnset(dat->vscroll,MUIA_Prop_First,t);
			nnset(dat->pageset,MYATTR_PageSet_Page,p);
			set(obj,MYATTR_DocScroll_Quiet,FALSE);
		    }
		} else {
		    dat->page=p;
		    if(dat->mode!=vmSpecial)
			set(dat->pageset,MYATTR_PageSet_Page,p);
		}
	    }
	    break;
	  }

	  case MYATTR_DocScroll_ViewMode:
	    if(tag->ti_Data!=dat->mode) {
		int old_mode=dat->mode;
		set(obj,MYATTR_DocScroll_Quiet,TRUE);
		dat->mode=tag->ti_Data;
		if(dat->mode==vmContinuous) {
		    get(dat->pageset,MYATTR_PageSet_Rows,&dat->orig_rows);
		    set(dat->pageset,MYATTR_PageSet_StripMode,TRUE);
		    DB(kprintf("orig_rows=%ld\n",dat->orig_rows);)
		}
#ifndef BROKEN_KILLNOTIFY
		DoMethod3(dat->vscroll,MUIM_KillNotify,MUIA_Prop_First);
		DoMethod3(dat->vgroup,MUIM_KillNotify,MUIA_Virtgroup_Top);
		DoMethod3(dat->vgroup,MUIM_KillNotify,MUIA_Virtgroup_Height);
		if(dat->mode==vmContinuous) {
		    DoMethod7(dat->vscroll,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,
			       //_app(obj),4,MUIM_Application_PushMethod,
			       obj,1,MYM_DocScroll_PosChanged);
		} else {
		    DoMethod9(dat->vscroll,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,
			      dat->vgroup,3,MUIM_Set,MUIA_Virtgroup_Top,MUIV_TriggerValue);
		    DoMethod9(dat->vgroup,MUIM_Notify,MUIA_Virtgroup_Top,MUIV_EveryTime,
			      dat->vscroll,3,MUIM_Set,MUIA_Prop_First,MUIV_TriggerValue);
		    DoMethod9(dat->vgroup,MUIM_Notify,MUIA_Virtgroup_Height,MUIV_EveryTime,
			      dat->vscroll,3,MUIM_Set,MUIA_Prop_Entries,MUIV_TriggerValue);
		}
#endif
		DoMethod2(obj,MYM_DocScroll_UpdateState);
		if(dat->mode==vmContinuous) {
		    int p=dat->page;
		    dat->page=-1;
		    SetAttrs(obj,
			     MYATTR_DocScroll_NoHistory,TRUE,
			     MYATTR_DocScroll_Page,p,
			     TAG_END);
		}
		if(old_mode==vmContinuous) {
		    DB(kprintf("rows reset to %ld\n",dat->orig_rows);)
		    SetAttrs(dat->pageset,
			     MYATTR_PageSet_Rows,dat->orig_rows,
			     MYATTR_PageSet_StripMode,FALSE,
			     MYATTR_PageSet_YOffset,0,
			     TAG_END);
		}
		set(obj,MYATTR_DocScroll_Quiet,FALSE);
	    }
	    break;

	  case MYATTR_DocScroll_Quiet:
	    /*
	     * When this attribute is set, no drawing operation is done,
	     * neither by the pageset object, nor by the virtual group that
	     * contains it. Unfortunately, when the position of the
	     * virtual group is changed, the visible area is scrolled
	     * without going through the MUIM_Draw method of the virtual
	     * group. So a 'quiet' mode for the virtual group can not be
	     * implemented by subclassing the MUIC_Virtgroup class.
	     * Instead, we do it in the following way: we put the virtual group
	     * in a page group together with a RectangleObject that has
	     * FillArea=FALSE. Switching to the RectangleObject page prevents
	     * the virtual group from drawing anything.
	     */
	    set(dat->pageset,MYATTR_PageSet_Quiet,tag->ti_Data);
	    if(tag->ti_Data) {
		set(obj,MUIA_Group_ActivePage,1);
		++dat->quiet;
	    } else {
		if(!--dat->quiet)
		    set(obj,MUIA_Group_ActivePage,0);
	    }
	    DB(kprintf("docscroll %lx: quiet=%ld\n",obj,dat->quiet);)
	    break;

	  case MYATTR_DocScroll_Zoom:
	    if(dat->zoom!=tag->ti_Data) {
		DB(kprintf("docscroll: zoom=%ld (was %d)\n",tag->ti_Data,dat->zoom);)
		dat->zoom=tag->ti_Data;
		set(obj,MYATTR_DocScroll_Quiet,TRUE);
		if(dat->zoom==zoomPage) {
		    if(dat->mode!=vmContinuous)
			set(obj,MYATTR_DocScroll_ViewMode,vmFitPage);
		} else if(dat->mode==vmFitPage) {
		    set(obj,MYATTR_DocScroll_ViewMode,vmFix);
		}
		if(dat->zoom==zoomPage || dat->zoom==zoomWidth)
		    dat->dpi=-1;
		set(dat->pageset,MYATTR_PageSet_Zoom,dat->zoom);
		DoMethod2(obj,MYM_DocScroll_UpdateState);
		/*if(dat->mode==vmContinuous &&
		   (dat->zoom==zoomPage || dat->zoom==zoomWidth)) {
		    dat->dpi=-1;
		    set(dat->pageset,MYATTR_PageSet_DPI,dat->dpi);
		}*/
		set(obj,MYATTR_DocScroll_Quiet,FALSE);
	    }
	    break;
	}
    }
    if(msg->MethodID==OM_NEW)
	return 0;
    else
	return DoSuperMethodA(cl,obj,(APTR)msg);
}

Static Object *getParent(Object *o) {
    Object *p;
    get(o, MUIA_Parent, &p);
    return p;
}

Static ULONG dsNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    Object *vscroll,*hscroll,*vgroup,*pageset,*pgroup,*rect,*fgroup;
    struct TagItem tags[4];
    struct comm_info *ci;
    BOOL full_screen;

    DB(kprintf("new DocScrollObject\n");)

    pageset=(Object*)GetTagData(MYATTR_DocScroll_Contents,0,msg->ops_AttrList);
    ci=(struct comm_info *)GetTagData(MYATTR_DocScroll_CommInfo,0,msg->ops_AttrList);
    full_screen=GetTagData(MYATTR_DocScroll_FullScreen,0,msg->ops_AttrList);

    if(!pageset || !ci)
	return 0;

    vgroup=myVirtgroupObject,
	//MUIA_FillArea,FALSE,
	MUIA_Virtgroup_Input,FALSE,
	Child,pageset,
	End;

    tags[0].ti_Tag=MUIA_Group_PageMode;
    tags[0].ti_Data=TRUE;
    tags[1].ti_Tag=MUIA_Group_Child;
    tags[1].ti_Data=(LONG)(pgroup=VGroup,
	MUIA_Group_Columns,2,
	MUIA_Group_Spacing,0,
	Child,fgroup=HGroup,
	    /* put the frame in another group, so that the size
	       of the inner area is the size of vgroup */
	    MUIA_Frame,full_screen?MUIV_Frame_None:MUIV_Frame_Virtual,
	    Child,vgroup,
	    End,
	Child,vscroll=ScrollbarObject,
	    End,
	Child,hscroll=ScrollbarObject,
	    MUIA_Group_Horiz,TRUE,
	    End,
	Child,rect=RectangleObject,
	    End,
	End);
    tags[2].ti_Tag=MUIA_Group_Child;
    tags[2].ti_Data=(LONG)RectangleObject, /* hack for MYATTR_DocScroll_Quiet */
	MUIA_FillArea,FALSE,
	End;
    tags[3].ti_Tag=TAG_MORE;
    tags[3].ti_Data=(LONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;

    if(obj=(Object*)DoSuperMethodA(cl,obj,(APTR)msg)) {
	DocScrollData* dat=INST_DATA(cl,obj);
	LONG t;

	memset(dat,0,sizeof(*dat));
	dat->comm_info=ci;
	dat->pageset=pageset;
	dat->pgroup=pgroup;
	dat->fgroup=fgroup;
	dat->vgroup=vgroup;
	dat->hscroll=hscroll;
	dat->vscroll=vscroll;
	dat->rect=rect;
	dat->mode=-1;
	dat->zoom=defZoom;
	dat->dpi=-1;

	// kiero: use parent as it seems obj is not real parent of these objects. this one should be safe
	if (getParent(hscroll))
		DoMethod3(getParent(hscroll),OM_REMMEMBER,hscroll);
	if (getParent(vscroll))
		DoMethod3(getParent(vscroll),OM_REMMEMBER,vscroll);
	if (getParent(rect))
		DoMethod3(getParent(rect),OM_REMMEMBER,rect);

	dat->has_hscroll=FALSE;//TRUE;
	dat->has_vscroll=FALSE;//TRUE;

	get(dat->pageset,MYATTR_PageSet_Page,&dat->page);
	get(dat->pageset,MYATTR_PageSet_Rows,&dat->orig_rows);
	set(dat->pageset,MYATTR_PageSet_VGroup,vgroup);
	set(obj,MYATTR_DocScroll_ViewMode,vmFix);
	get(dat->pageset,MYATTR_PageSet_Zoom,&t);
	DB(kprintf("DocScroll: zoom %d pageset zoom %d\n",dat->zoom,t);)
	set(obj,MYATTR_DocScroll_Zoom,t);

	DoMethod9(pageset,MUIM_Notify,MYATTR_PageSet_Page,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocScroll_Page,MUIV_TriggerValue);
	DoMethod9(pageset,MUIM_Notify,MYATTR_PageSet_DocChanged,MUIV_EveryTime,
		  obj,3,MUIM_Set,MYATTR_DocScroll_Document,MUIV_TriggerValue);
	//DoMethod7(vscroll,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,
	//         obj,1,MYM_DocScroll_PosChanged);
	//DoMethod9(pageset,MUIM_Notify,MYATTR_PageSet_Zoom,MUIV_EveryTime,
	//         obj,3,MUIM_Set,MYATTR_DocScroll_Zoom,MUIV_TriggerValue);
	DoMethod7(pageset,MUIM_Notify,MYATTR_PageSet_Rotate,MUIV_EveryTime,
		   //app,3,MUIM_Application_PushMethod,
		   obj,1,MYM_DocScroll_UpdateState);
	DoMethod7(pageset,MUIM_Notify,MYATTR_PageSet_Columns,MUIV_EveryTime,
		   //app,3,MUIM_Application_PushMethod,
		   obj,1,MYM_DocScroll_Layout);
	DoMethod7(pageset,MUIM_Notify,MYATTR_PageSet_Rows,MUIV_EveryTime,
		   //app,3,MUIM_Application_PushMethod,
		   obj,1,MYM_DocScroll_Layout);
	DoMethod7(pageset,MUIM_Notify,MYATTR_PageSet_Back,TRUE,
		  obj,1,MYM_DocScroll_Backward);
	/*DoMethod9(vgroup,MUIM_Notify,MUIA_Height,MUIV_EveryTime,
		 vscroll,3,MUIM_Set,MUIA_Prop_Visible,MUIV_TriggerValue);*/
	DoMethod9(vgroup,MUIM_Notify,MUIA_Virtgroup_Width,MUIV_EveryTime,
		 hscroll,3,MUIM_Set,MUIA_Prop_Entries,MUIV_TriggerValue);
	DoMethod9(vgroup,MUIM_Notify,MUIA_Virtgroup_Left,MUIV_EveryTime,
		 hscroll,3,MUIM_Set,MUIA_Prop_First,MUIV_TriggerValue);
	DoMethod9(hscroll,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,
		 vgroup,3,MUIM_Set,MUIA_Virtgroup_Left,MUIV_TriggerValue);
	/*DoMethod9(vgroup,MUIM_Notify,MUIA_Width,MUIV_EveryTime,
		 hscroll,3,MUIM_Set,MUIA_Prop_Visible,MUIV_TriggerValue);*/
#ifdef BROKEN_KILLNOTIFY
	DoMethod7(vscroll,MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,
		  //app,4,MUIM_Application_PushMethod,
		  obj,1,MYM_DocScroll_PosChanged);
#endif
	DoMethod7(vgroup,MUIM_Notify,MYATTR_Virtgroup_GeometryChange,MUIV_EveryTime,
		  //app,3,MUIM_Application_PushMethod,
		  obj,1,MYM_DocScroll_UpdateScroll);
	get(pageset,MYATTR_PageSet_Document,&t);
	DB(kprintf("doc=%lx\n",t);)
	if(t)
	    SetAttrs(obj,
		     MYATTR_DocScroll_Document,t,
		     MYATTR_DocScroll_Page,dat->page,
		     TAG_END);
	dsSet(cl,obj,msg);
    }

    msg->ops_AttrList=(APTR)tags[3].ti_Data;

    return (LONG)obj;
}

Static ULONG dsDispose(struct IClass *cl,Object *obj,Msg msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    int n;
    MyFreeMem(dat->pages);
    for(n=0;n<dat->histTot;++n) {
	MyFreeMem(dat->history[n].name);
	UnLock(dat->history[n].dir);
    }
    for(n=0;n<numBookmarks;++n) {
	MyFreeMem(dat->bookmark[n].name);
	UnLock(dat->bookmark[n].dir);
    }
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG dsGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_DocScroll_ViewMode:
	*msg->opg_Storage=dat->mode;
	return TRUE;
      case MYATTR_DocScroll_DocChanged:
	*msg->opg_Storage=(LONG)dat->docchanged;
	return TRUE;
      case MYATTR_DocScroll_Document:
	*msg->opg_Storage=(LONG)dat->doc;
	return TRUE;
      case MYATTR_DocScroll_Page:
	*msg->opg_Storage=dat->page;
	return TRUE;
      case MYATTR_DocScroll_Zoom:
	*msg->opg_Storage=dat->zoom;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(APTR)msg);
}

Static ULONG dsSetup(struct IClass *cl,Object *obj,Msg msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    if(!DoSuperMethodA(cl,obj,msg))
	return FALSE;
    //MUI_RequestIDCMP(obj,IDCMP_RAWKEY);
    dat->ehn.ehn_Priority=0;
    dat->ehn.ehn_Flags=0;
    dat->ehn.ehn_Object=obj;
    dat->ehn.ehn_Class=NULL;
    dat->ehn.ehn_Events=IDCMP_RAWKEY;
    DoMethod3(_win(obj),MUIM_Window_AddEventHandler,&dat->ehn);
    return TRUE;
}

Static ULONG dsCleanup(struct IClass *cl,Object *obj,Msg msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    DoMethod3(_win(obj),MUIM_Window_RemEventHandler,&dat->ehn);
    //MUI_RejectIDCMP(obj,IDCMP_RAWKEY);
    return DoSuperMethodA(cl,obj,msg);
}

/*Static ULONG dsAskMinMax(struct IClass *cl,Object *obj,Msg msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    ULONG r=DoSuperMethodA(cl,obj,msg);
    int w,h;
    get(dat->vgroup,MUIA_Width,&w);
    get(dat->vgroup,MUIA_Height,&h);
    DB(kprintf("askminmax-> %ld/%ld, %ld/%ld\n",w,dat->max_width,h,dat->total_height);)
    SetAttrs(dat->hscroll,
	     MUIA_Prop_Entries,dat->max_width,
	     MUIA_Prop_Visible,w,
	     TAG_END);
    SetAttrs(dat->vscroll,
	     MUIA_Prop_Entries,dat->total_height,
	     MUIA_Prop_Visible,h,
	     TAG_END);
    return r;
}*/

/*Static ULONG dsDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg) {
    ULONG r=DoSuperMethodA(cl,obj,msg);
    if(msg->flags&MADF_DRAWOBJECT) {
    }
    return r;
} */

Static ULONG dsHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    ULONG rc = 0;
    #define _between(a,x,b) ((x)>=(a) && (x)<=(b))
    #define _isinobject(x,y) (_between(_mleft(obj),(x),_mright(obj)) && _between(_mtop(obj),(y),_mbottom(obj)))

    if(msg->imsg) {

	ULONG mouse_inside = _isinobject(msg->imsg->MouseX, msg->imsg->MouseY);

	switch(msg->imsg->Class) {
	  case IDCMP_RAWKEY:
	    if(msg->imsg->Code>=0x50 && msg->imsg->Code<=0x59) {
		/* Function keys */
		int n=msg->imsg->Code-0x50;
		if(msg->imsg->Qualifier&IEQUALIFIER_LSHIFT ||
		   msg->imsg->Qualifier&IEQUALIFIER_RSHIFT) {
		    BPTR dir;
		    const char *name;
		    MyFreeMem(dat->bookmark[n].name);
		    UnLock(dat->bookmark[n].dir);
		    get(dat->doc,MYATTR_Document_Dir,&dir);
		    get(dat->doc,MYATTR_Document_Name,&name);
		    dat->bookmark[n].dir=DupLock(dir);
		    CopyStr(&dat->bookmark[n].name,name);
		    dat->bookmark[n].page=dat->page;
		} else {
		    if(dat->bookmark[n].name) {
			Object *app,*win,*doc;
			get(obj,MUIA_ApplicationObject,&app);
			get(obj,MUIA_WindowObject,&win);
			doc=getDocument(dat->comm_info,app,win,dat->bookmark[n].dir,dat->bookmark[n].name);
			if(doc)
			    SetAttrs(obj,
				     MYATTR_DocScroll_Document,doc,
				     MYATTR_DocScroll_Page,dat->bookmark[n].page,
				     TAG_END);
		    }
		}
	    } else if (mouse_inside && msg->imsg->Code == RAWKEY_NM_WHEEL_UP) {
		DoMethod3(obj,MYM_DocScroll_PageUp, 15);
	    } else if (mouse_inside && msg->imsg->Code == RAWKEY_NM_WHEEL_DOWN) {
		DoMethod3(obj,MYM_DocScroll_PageDown, 15);
	    } else if (mouse_inside && msg->imsg->Code == RAWKEY_NM_WHEEL_LEFT) {
		int t;
		get(dat->hscroll,MUIA_Prop_First,&t);
		set(dat->hscroll,MUIA_Prop_First,t-30);
	    } else if (mouse_inside && msg->imsg->Code == RAWKEY_NM_WHEEL_RIGHT) {
		int t;
		get(dat->hscroll,MUIA_Prop_First,&t);
		set(dat->hscroll,MUIA_Prop_First,t+30);
	    } else {
		struct InputEvent ie={0};
		char buf[4];
		int n;
		ie.ie_Class=IECLASS_RAWKEY;
		ie.ie_SubClass=0;
		ie.ie_Code=msg->imsg->Code;
		ie.ie_Qualifier=msg->imsg->Qualifier;
		ie.ie_EventAddress=*(APTR*)msg->imsg->IAddress;
		n=MapRawKey(&ie,buf,sizeof(buf),NULL);
		if(n==1) {
		    ULONG prevrc = rc;
		    rc = MUI_EventHandlerRC_Eat; // eat event when we processed the key

		    switch(buf[0]) {
		      case 8:  /* BS */
			DoMethod3(obj,MYM_DocScroll_PageUp, 80);
			break;
		      case ' ':
			DoMethod3(obj,MYM_DocScroll_PageDown, 80);
			break;
		      case 'l':
			DoMethod2(dat->pageset,MYM_PageSet_RLeft);
			break ;
		      case 'r':
			DoMethod2(dat->pageset,MYM_PageSet_RRight);
			break ;
		      case 'c':
			DoMethod2(dat->pageset,MYM_PageSet_Crop);
			break;
		      case 'u':
			DoMethod2(dat->pageset,MYM_PageSet_UnCrop);
			break;
		      case '+':
			if(dat->zoom<maxZoom-1)
			    set(obj,MYATTR_DocScroll_Zoom,dat->zoom+1);
			break;
		      case '-':
			if(dat->zoom>minZoom)
			    set(obj,MYATTR_DocScroll_Zoom,dat->zoom-1);
			break;
		      default:
			rc = prevrc; // restore event state
		    }
		}
	    }
	    break;
	}
    }
    switch(msg->muikey) {
      case MUIKEY_UP: 
	DoMethod3(obj,MYM_DocScroll_PageUp, 10);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_DOWN:
	DoMethod3(obj,MYM_DocScroll_PageDown, 10);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_LEFT: {
	int t;
	get(dat->hscroll,MUIA_Prop_First,&t);
	set(dat->hscroll,MUIA_Prop_First,t-20);
	rc = MUI_EventHandlerRC_Eat;
	break;
      }
      case MUIKEY_RIGHT: {
	int t;
	get(dat->hscroll,MUIA_Prop_First,&t);
	set(dat->hscroll,MUIA_Prop_First,t+20);
	rc = MUI_EventHandlerRC_Eat;
	break;
      }
      case MUIKEY_PAGEUP:
	DoMethod3(obj,MYM_DocScroll_PageUp, 80);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_PAGEDOWN:
	DoMethod3(obj,MYM_DocScroll_PageDown, 80);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_TOP:
	set(obj,MYATTR_DocScroll_Page,1);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_BOTTOM:
	set(obj,MYATTR_DocScroll_Page,dat->num_pages);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_LINESTART:
	set(dat->hscroll,MUIA_Prop_First,0);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_LINEEND:
	set(dat->hscroll,MUIA_Prop_First,0x7fffffff);
	rc = MUI_EventHandlerRC_Eat;
	break;
      case MUIKEY_WORDLEFT: {
	int f,v;
	get(dat->hscroll,MUIA_Prop_First,&f);
	get(dat->hscroll,MUIA_Prop_Visible,&v);
	set(dat->hscroll,MUIA_Prop_First,f-v+30);
	rc = MUI_EventHandlerRC_Eat;
	break;
      }
      case MUIKEY_WORDRIGHT: {
	int f,v;
	get(dat->hscroll,MUIA_Prop_First,&f);
	get(dat->hscroll,MUIA_Prop_Visible,&v);
	set(dat->hscroll,MUIA_Prop_First,f+v-30);
	rc = MUI_EventHandlerRC_Eat;
	break;
      }
    }

    #undef _between
    #undef _isinobject


    return rc;//DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG dsUpdateScroll(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    int w,h,w1,h1,w2,h2,w3,h3;
    static int in_update_scroll;
    BOOL rem_hscroll,rem_vscroll;

    if(in_update_scroll>=2 || dat->mode==vmSpecial)
	return;
    ++in_update_scroll;

    DB(kprintf("update_scroll\n");)

    if(dat->zoom==zoomWidth) {
	set(obj,MYATTR_DocScroll_Quiet,TRUE);
	get(dat->vgroup,MUIA_Width,&w);
	DB(kprintf("zoomWidth(%ld)\n",w);)
	SetAttrs(dat->pageset,
		 MYATTR_PageSet_Zoom,zoomWidth,
		 MYATTR_PageSet_VisWidth,w,
		 TAG_END);
	update_dpi(cl,obj);
	set(obj,MYATTR_DocScroll_Quiet,FALSE);
    } else if(dat->zoom==zoomPage && dat->mode==vmContinuous) {
	set(obj,MYATTR_DocScroll_Quiet,TRUE);
	get(dat->vgroup,MUIA_Width,&w);
	get(dat->vgroup,MUIA_Height,&h);
	DB(kprintf("cont. zoomPage(%ld,%ld)\n",w,h);)
	SetAttrs(dat->pageset,
		 MYATTR_PageSet_Zoom,zoomPage,
		 MYATTR_PageSet_VisWidth,w,
		 MYATTR_PageSet_VisHeight,h,
		 TAG_END);
	update_dpi(cl,obj);
	set(obj,MYATTR_DocScroll_Quiet,FALSE);
    }

    get(obj,MUIA_Width,&w2);
    get(obj,MUIA_Height,&h2);
    if(w2==0) {
	DB(kprintf("not visible\n");)
	--in_update_scroll;
	return;
    }
    get(dat->vgroup,MUIA_Width,&w);
    get(dat->vgroup,MUIA_Height,&h);
    get(dat->pgroup,MUIA_Width,&w3);
    get(dat->pgroup,MUIA_Height,&h3);
    get(dat->vgroup,MUIA_Virtgroup_Width,&w1);
    SetAttrs(dat->hscroll,
	     MUIA_Prop_Entries,w1,
	     MUIA_Prop_Visible,w,
	     TAG_END);

    if(dat->mode==vmFitPage) {
	rem_hscroll=TRUE;
	rem_vscroll=TRUE;
	DB(kprintf("zoomPage(%ld,%ld)\n",w2-w3+w,h2-h3+h);)
	SetAttrs(dat->pageset,
		 MYATTR_PageSet_VisWidth,w2-(w3-w),
		 MYATTR_PageSet_VisHeight,h2-(h3-h),
		 TAG_END);
    } else {
	if(dat->mode==vmContinuous) {
	    update_sizes(cl,obj);
	    SetAttrs(dat->vscroll,
		     MUIA_Prop_Entries,h1=dat->total_height,
		     MUIA_Prop_Visible,h,
		     TAG_END);
	} else {
	    get(dat->vgroup,MUIA_Virtgroup_Height,&h1);
	    SetAttrs(dat->vscroll,
		     MUIA_Prop_Entries,h1,
		     MUIA_Prop_Visible,h,
		     TAG_END);
	}

	DB(kprintf("w=%ld, h=%ld, w1=%ld, h1=%ld, w2=%ld, h2=%ld, w3=%ld, h3=%ld\n",w,h,w1,h1,w2,h2,w3,h3);)
	if(w2-(w3-w)>=w1 && h2-(h3-h)>=h1) {
	    rem_hscroll=TRUE;
	    rem_vscroll=TRUE;
	} else if(w>=w1) {
	    rem_hscroll=TRUE;
	    rem_vscroll=FALSE;
	} else if(h>=h1) {
	    rem_hscroll=FALSE;
	    rem_vscroll=TRUE;
	} else {
	    rem_hscroll=FALSE;
	    rem_vscroll=FALSE;
	}
    }

    if(in_update_scroll>1) {
	--in_update_scroll;
	return 0;
    }

    if(rem_hscroll==dat->has_hscroll || rem_vscroll==dat->has_vscroll) {
	Object* g=dat->pgroup;
	//set(obj,MYATTR_DocScroll_Quiet,TRUE);
	if(DoMethod2(g,MUIM_Group_InitChange)) {

	    if(rem_hscroll) {
		if(dat->has_hscroll) {
		    DB(kprintf("removing hscroll\n");)
		    DoMethod3(g,OM_REMMEMBER,dat->hscroll);
		    if(dat->has_vscroll)
			DoMethod3(g,OM_REMMEMBER,dat->rect);
		    dat->has_hscroll=FALSE;
		}
	    } else {
		if(!dat->has_hscroll) {
		    DB(kprintf("adding hscroll\n");)
		    DoMethod3(g,OM_ADDMEMBER,dat->hscroll);
		    if(dat->has_vscroll)
			DoMethod3(g,OM_ADDMEMBER,dat->rect);
		    dat->has_hscroll=TRUE;
		}
	    }
	    if(rem_vscroll) {
		if(dat->has_vscroll) {
		    DB(kprintf("removing vscroll\n");)
		    DoMethod3(g,OM_REMMEMBER,dat->vscroll);
		    if(dat->has_hscroll)
			DoMethod3(g,OM_REMMEMBER,dat->rect);
		    set(g,MUIA_Group_Columns,1);
		    dat->has_vscroll=FALSE;
		}
	    } else {
		if(!dat->has_vscroll) {
		    DB(kprintf("adding vscroll\n");)
		    DoMethod3(g,OM_ADDMEMBER,dat->vscroll);
		    if(dat->has_hscroll) {
			DoMethod3(g,OM_ADDMEMBER,dat->rect);
			DoMethod7(g,MUIM_Group_Sort,dat->fgroup,dat->vscroll,dat->hscroll,dat->rect,NULL);
		    }
		    set(g,MUIA_Group_Columns,2);
		    dat->has_vscroll=TRUE;
		}
	    }

	    DoMethod2(g,MUIM_Group_ExitChange);
	}
	//set(obj,MYATTR_DocScroll_Quiet,FALSE);
    }
    DB(kprintf("ok\n");)
    --in_update_scroll;
    return 0;
}

Static ULONG dsPageDown(struct IClass *cl,Object *obj,struct MYP_DocScroll_PageUpDown* msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    LONG f,v,e;
    get(dat->vscroll,MUIA_Prop_First,&f);
    get(dat->vscroll,MUIA_Prop_Visible,&v);
    get(dat->vscroll,MUIA_Prop_Entries,&e);
    if(dat->mode==vmContinuous || (dat->mode==vmFix && f+v<e))
	set(dat->vscroll,MUIA_Prop_First,f+v*msg->percent/100);
	else if (dat->page < dat->num_pages) {
	SetAttrs(obj,
		 MYATTR_DocScroll_Quiet,TRUE,
		 MYATTR_DocScroll_Page,dat->page+1,
		 TAG_END);
	set(dat->vscroll,MUIA_Prop_First,0);
	set(obj,MYATTR_DocScroll_Quiet,FALSE);
    }
    return 0;
}

Static ULONG dsPageUp(struct IClass *cl,Object *obj,struct MYP_DocScroll_PageUpDown* msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    LONG f,v;
    get(dat->vscroll,MUIA_Prop_First,&f);
    get(dat->vscroll,MUIA_Prop_Visible,&v);
    if(dat->mode==vmContinuous || (dat->mode==vmFix && f>0))
	set(dat->vscroll,MUIA_Prop_First,f-v*msg->percent/100);
	else if (dat->page > 1)
	{
	LONG e;
	get(dat->vscroll,MUIA_Prop_Entries,&e);
	SetAttrs(obj,
		 MYATTR_DocScroll_Quiet,TRUE,
		 MYATTR_DocScroll_Page,dat->page-1,
		 TAG_END);
	set(dat->vscroll,MUIA_Prop_First,e);
	set(obj,MYATTR_DocScroll_Quiet,FALSE);
	}
    return 0;
}

Static ULONG dsPosChanged(struct IClass *cl,Object* obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    int top,t=0,k,l,vis_height,bottom,s;
    struct page_info *pages=dat->pages;
    static int in_pos_changed;

    get(dat->vscroll,MUIA_Prop_First,&top);
    if(dat->mode!=vmContinuous) {
	set(dat->vgroup,MUIA_Virtgroup_Top,top);
	return;
    }
    if(!pages || in_pos_changed || ((dat->zoom==zoomPage || dat->zoom==zoomWidth) && dat->dpi<=0))
	return;
    DB(kprintf("pos_changed:\n");)
    get(dat->vgroup,MUIA_Height,&vis_height);
    DB({int total; int vis;
       get(dat->vscroll,MUIA_Prop_Entries,&total);
       get(dat->vscroll,MUIA_Prop_Visible,&vis);
       kprintf("prop: total=%ld, vis=%ld, top=%ld,  vgroup:vis=%ld\n",total,vis,top,vis_height);
       })
    if(vis_height==0)
	return;
    ++in_pos_changed;
    for(k=-dat->skip;k<dat->num_pages && t<=top;) {
	int i;
	int max_height=0;
	s=t;
	l=k;
	for(i=0;i<dat->columns && k<dat->num_pages;++i) {
	    if(k>=0) {
		int h=pages[k].height<=0 ? dat->average_height : pages[k].height;
		if(h>max_height)
		    max_height=h;
	    }
	    ++k;
	}
	t+=max_height+SPACING;
    }
    ++l;
    ++k;
    if(t==top)
	k=l;
    DB(kprintf("->t=%ld, page=%ld, cur=%ld, vis=%ld\n",s,l,k,vis_height+top-s);)
    /*if(dat->page!=k) {
	DoMethod5(obj,MYM_DocScroll_HistoryPush,dat->page,0,NULL);
	dat->page=k;
    }*/
    set(obj,MYATTR_DocScroll_Quiet,TRUE);
    DoMethod6(dat->pageset,MYM_PageSet_VStrip,l,vis_height+top-s,s,k);
    //set(dat->pageset,MYATTR_PageSet_Page,k);
    dat->mode=vmSpecial;
    set(obj,MYATTR_DocScroll_Page,k);
    dat->mode=vmContinuous;
    //set(dat->pageset,MYATTR_PageSet_YOffset,s);
    //DB(kprintf("yoffset=%ld\n",s);)
    set(dat->vgroup,MUIA_Virtgroup_Top,top-s);
    set(obj,MYATTR_DocScroll_Quiet,FALSE);
    update_sizes(cl,obj);
    DB(kprintf("~pos_changed(%ld):\n",dat->page);)
    --in_pos_changed;
    return 0;
}

Static ULONG dsLayout(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    if(dat->mode==vmContinuous) {
	calc_sizes(dat);
	dsPosChanged(cl,obj);
    }
    return 0;
}

Static ULONG dsResetData(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    dat->dpi=-1;
    MyFreeMem(dat->pages);
    if(dat->mode==vmContinuous)
	dat->pages=MyAllocMem(dat->num_pages*sizeof(*dat->pages));
    else
	dat->pages=NULL;
    if(dat->pages) {
	int k,p=dat->page;
	struct page_info *pages=dat->pages;
	for(k=0;k<dat->num_pages;++k) {
	    pages[k].width=-1;
	    pages[k].height=-1;
	}
	set(obj,MYATTR_DocScroll_Quiet,TRUE);
	update_sizes(cl,obj);
	dat->page=-1;
	SetAttrs(obj,
		 MYATTR_DocScroll_NoHistory,TRUE,
		 MYATTR_DocScroll_Page,p,
		 TAG_END);
	dsPosChanged(cl,obj);
	set(obj,MYATTR_DocScroll_Quiet,FALSE);
    }
    return 0;
}

Static ULONG dsUpdateState(struct IClass *cl,Object *obj) {
    set(obj,MYATTR_DocScroll_Quiet,TRUE);
    dsResetData(cl,obj);
    dsLayout(cl,obj);
    dsUpdateScroll(cl,obj);
    set(obj,MYATTR_DocScroll_Quiet,FALSE);
    return 0;
}

Static ULONG dsHistoryPush(struct IClass *cl,Object *obj,struct MYP_DocScroll_HistoryPush *msg) {
    DocScrollData *dat=INST_DATA(cl,obj);
    DB(kprintf("historyPush(%ld,%lx,%s)\n",msg->page,msg->dir,msg->name?msg->name:"NULL"));
    while(dat->histCur<dat->histTot) {
	struct History *h;
	--dat->histTot;
	h=dat->history+dat->histTot;
	MyFreeMem(h->name);
	UnLock(h->dir);
    }
    if(dat->histCur==historySize) {
	MyFreeMem(dat->history->name);
	UnLock(dat->history->dir);
	memmove(dat->history,dat->history+1,(historySize-1)*sizeof(struct History));
	--dat->histCur;
	--dat->histTot;
    }
    dat->history[dat->histCur].page=msg->page;
    dat->history[dat->histCur].dir=DupLock(msg->dir);
    dat->history[dat->histCur].name=NULL;
    if(msg->name)
	CopyStr(&dat->history[dat->histCur].name,msg->name);;
    ++dat->histCur;
    ++dat->histTot;
    return 0;
}

Static ULONG dsHistory(struct IClass *cl,Object *obj,struct History *h) {
    DocScrollData *dat=INST_DATA(cl,obj);
    Object *doc=dat->doc;
    int p=h->page;
    if(h->name) {
	const char* name;
	BPTR dir;
	Object *app,*win;
	get(doc,MYATTR_Document_Dir,&dir);
	get(doc,MYATTR_Document_Name,&name);
	get(obj,MUIA_ApplicationObject,&app);
	get(obj,MUIA_WindowObject,&win);
	doc=getDocument(dat->comm_info,app,win,h->dir,h->name);
	if(!doc)
	    return 0;
	CopyStr(&h->name,name);
	UnLock(h->dir);
	h->dir=DupLock(dir);
    }
    h->page=dat->page;
    SetAttrs(obj,
	     MYATTR_DocScroll_NoHistory,TRUE,
	     MYATTR_DocScroll_Document,doc,
	     MYATTR_DocScroll_Page,p,
	     TAG_END);
    return 1;
}

Static ULONG dsBackward(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    if(dat->histCur>0 && dsHistory(cl,obj,dat->history+dat->histCur-1)) {
	--dat->histCur;
	return 1;
    }
    return 0;
}

Static ULONG dsForward(struct IClass *cl,Object *obj) {
    DocScrollData *dat=INST_DATA(cl,obj);
    if(dat->histCur<dat->histTot && dsHistory(cl,obj,dat->history+dat->histCur)) {
	++dat->histCur;
	return 1;
    }
    return 0;
}

BEGIN_DISPATCHER(dsDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW          : return dsNew        (cl,obj,(APTR)msg);
      case OM_DISPOSE      : return dsDispose    (cl,obj,(APTR)msg);
      case OM_SET          : return dsSet        (cl,obj,(APTR)msg);
      case OM_GET          : return dsGet        (cl,obj,(APTR)msg);
//      case MUIM_AskMinMax  : return dsAskMinMax  (cl,obj,(APTR)msg);
//      case MUIM_Draw       : return dsDraw       (cl,obj,(APTR)msg);
      case MUIM_Setup                   : return dsSetup(cl,obj,(APTR)msg);
      case MUIM_Cleanup                 : return dsCleanup(cl,obj,(APTR)msg);
      case MUIM_HandleEvent             : return dsHandleEvent(cl,obj,(APTR)msg);
      case MYM_DocScroll_PageDown       : return dsPageDown(cl,obj,(APTR)msg);
      case MYM_DocScroll_PageUp         : return dsPageUp(cl,obj,(APTR)msg);
      case MYM_DocScroll_PosChanged     : return dsPosChanged(cl,obj);
      case MYM_DocScroll_Layout         : return dsLayout(cl,obj);
      case MYM_DocScroll_ResetData      : return dsResetData(cl,obj);
      case MYM_DocScroll_UpdateScroll   : return dsUpdateScroll(cl,obj);
      case MYM_DocScroll_UpdateState    : return dsUpdateState(cl,obj);
      case MYM_DocScroll_HistoryPush    : return dsHistoryPush(cl,obj,(APTR)msg);
      case MYM_DocScroll_Backward       : return dsBackward(cl,obj);
      case MYM_DocScroll_Forward        : return dsForward(cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(dsDispatcher)


struct MUI_CustomClass *docscroll_mcc;
static int count;

BOOL createDocScrollClass(void) {
    if(count++==0) {
	if(vgroup_mcc=MUI_CreateCustomClass(NULL,MUIC_Virtgroup,NULL,sizeof(VGroupData),(APTR)&vgDispatcher)) {
	    if(docscroll_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(DocScrollData),(APTR)&dsDispatcher))
		return TRUE;
	    MUI_DeleteCustomClass(vgroup_mcc);
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteDocScrollClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(docscroll_mcc);
	MUI_DeleteCustomClass(vgroup_mcc);
    }
    return TRUE;
}

