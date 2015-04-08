/*************************************************************************\
 *                                                                       *
 * pageset.c                                                             *
 *                                                                       *
 * Copyright 2000-2002 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libraries/mui.h>
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
#include "pageset.h"
#include "msgs.h"
#include "prefs.h"

#define DB(x)   //x
#define Static  static

#define kprintf dprintf

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

#define SPACING  0
#define SPACING2 4

struct PageSetData {
    int columns;
    int rows;
    int page;
    int cur_page;
    int num_pages;
    int num_page_objs;
    int skip;
    /*int rotate;
    int textAA;
    int fillAA;
    int strokeAA;*/
    Object *doc;
    Object *docchg;
    Object *bitmapobj;
    struct comm_info *comm_info;
    WORD cell_width;
    WORD cell_height;
    Object *vgroup;
    int locked;
    int quiet;
    int zoom;
    int dpi;
    int rotate;
    int fillaa;
    int textaa;
    int strokeaa;
    struct MYS_DocBitmap_CropBox box;
    int yoffset;
    BOOL update;
    BOOL strip_mode;
    enum DrawMode draw_mode;
    BOOL full_screen;
    BOOL ready;
};
typedef struct PageSetData PageSetData;

struct MUI_CustomClass *pageset_mcc;

BEGIN_STATIC_HOOK(void,ready,
		  struct Hook *,h,
		  Object *,obj,
		  Object **,p) {
    APTR state;
    struct List *list;
    Object *o;
    get(obj,MUIA_Group_ChildList,&list);
    state=list->lh_Head;
    while(o=NextObject(&state)) {
	int ready;
	get(o,MYATTR_DocBitmap_Ready,&ready);
	if(!ready)
	    return;
    }
    set(obj,MYATTR_PageSet_Ready,TRUE);
}
END_STATIC_HOOK(ready)

BEGIN_STATIC_HOOK(void,busy,
		  struct Hook *,h,
		  Object *,obj,
		  Object **,p) {
    set(obj,MYATTR_PageSet_Ready,FALSE);
}
END_STATIC_HOOK(busy)

#if 0
Static ULONG ASM layout_func(REG(a2,Object *obj),REG(a1,struct MUI_LayoutMsg *lm)) {
    PageSetData *dat=INST_DATA(pageset_mcc->mcc_Class,obj);
    switch(lm->lm_Type) {
      case MUILM_MINMAX: {
	Object *state=(Object *)lm->lm_Children->mlh_Head;
	Object *child;
	WORD minw=0;
	WORD minh=0;

	while(child=NextObject(&state)) {
	    if(_minwidth(child)>minw)
		minw=_minwidth(child);
	    if(_minheight(child)>minh)
		minh=_minheight(child);
	}

	dat->cell_width=minw;
	dat->cell_height=minh;
	minw*=dat->columns;
	minh*=dat->rows;

	lm->lm_MinMax.MinWidth=minw;
	lm->lm_MinMax.MinHeight=minh;
	lm->lm_MinMax.DefWidth=minw;
	lm->lm_MinMax.DefHeight=minh;
	lm->lm_MinMax.MaxWidth=MUI_MAXMAX;
	lm->lm_MinMax.MaxHeight=MUI_MAXMAX;
	return 0;
      }

      case MUILM_LAYOUT: {
	Object *state=(Object *)lm->lm_Children->mlh_Head;
	Object *child;
	int w=lm->lm_Layout.Width/dat->columns;
	int h=lm->lm_Layout.Height/dat->rows;
	int x1=(w-dat->cell_width)>>1;
	int y1=(h-dat->cell_height)>>1;
	int k=0;
	int x=0;
	int y=0;

	while(child=NextObject(&state)) {
	    if(!MUI_Layout(child,x+x1,y+y1,dat->cell_width,dat->cell_height,0))
		return FALSE;
	    ++k;
	    x+=w;
	    if(k>=dat->columns) {
		x=0;
		k=0;
		y+=h;
	    }
	}

	return TRUE;
      }
    }
    return MUILM_UNKNOWN;
}

Static struct Hook layout_hook={{NULL,NULL},(ULONG(*)())layout_func};
#endif

/*     page     p           page+num_page_objs
 *      |       |                   |
 *      v       v                   v
 *    | a | b | c | d | e | f | g |
 *
 * -> | c | d | e | f | g | a | b |
 */
Static void psSwap(struct IClass *cl,Object *obj,int p) {
    PageSetData *dat=INST_DATA(cl,obj);
    int k;
    APTR state;
    struct List *list;
    Object **q;
    get(obj,MUIA_Group_ChildList,&list);
    DB(kprintf("psSwap(%ld,%ld)\n",dat->page,p);)
    if(q=MyAllocMem(sizeof(*q)*(dat->num_page_objs+2))) {
	state=list->lh_Head;
	for(k=dat->page;k<p;++k)
	    q[k+dat->num_page_objs-p+1]=NextObject(&state);
	for(k=p;k<dat->page+dat->num_page_objs;++k)
	    q[k-p+1]=NextObject(&state);
	q[0]=(Object*)MUIM_Group_Sort;
	q[dat->num_page_objs+1]=NULL;
	//dat->bitmapobj=q[1];
	DoMethodA(obj,(Msg)q);
	MyFreeMem(q);
    }
}

Static void psSetSpacing(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(!dat->strip_mode) {
	APTR state;
	struct List *list;
	int j;
	get(obj,MUIA_Group_ChildList,&list);
	state=list->lh_Head;
	for(j=0;j<dat->rows;++j) {
	    int i;
	    for(i=0;i<dat->columns;++i) {
		Object *o=NextObject(&state);
		SetAttrs(o,
			 MYATTR_DocBitmap_XSpacing,i<dat->columns-1?SPACING2:0,
			 MYATTR_DocBitmap_YSpacing,j<dat->rows-1?SPACING2:0,
			 TAG_END);
	    }
	}
    }
    set(obj,MYATTR_PageSet_YOffset,dat->yoffset);
}

Static ULONG psSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    int columns=dat->columns;
    int rows=dat->rows;

    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_PageSet_Columns:
	    columns=tag->ti_Data;
	    break;

	  case MYATTR_PageSet_Rows:
	    rows=tag->ti_Data;
	    break;

	  case MYATTR_PageSet_DocChanged:
	    dat->docchg=(Object*)tag->ti_Data;
	    break;

	  case MYATTR_PageSet_Page:
	    DB(kprintf("PageSet::set page %d (was %d)\n",tag->ti_Data,dat->cur_page);)
	    if(tag->ti_Data!=dat->cur_page) {
		set(obj,MYATTR_PageSet_Quiet,TRUE);
		if(DoMethod2(obj,MUIM_Group_InitChange)) {
		    int p=tag->ti_Data;
		    APTR state;
		    struct List *list;
		    Object* o;
		    if(p<dat->page || p>=dat->page+dat->num_page_objs) {
			if(dat->strip_mode)
			    p=(p-1)/dat->columns*dat->columns-dat->skip+1;
			else if(p>=dat->page+dat->num_page_objs) {
			    p-=dat->num_page_objs-1;
			    if(p<1)
				p=1;
			}
			if(!dat->locked) {
			    if(p>dat->page && p<dat->page+dat->num_page_objs)
				psSwap(cl,obj,p);
			    else if(dat->page>p && dat->page<p+dat->num_page_objs)
				psSwap(cl,obj,p+dat->num_page_objs);
			}
			dat->page=p;
		    }
		    p=dat->page;
		    dat->cur_page=tag->ti_Data;
		    get(obj,MUIA_Group_ChildList,&list);
		    state=list->lh_Head;
		    DB(kprintf("pageset:page=%ld, cur=%ld (%ld)\n",dat->page,dat->cur_page,dat->strip_mode);)
		    while(o=NextObject(&state)) {
			nnset(o,MYATTR_DocBitmap_Page,p);
			if(p==dat->cur_page)
			    set(obj,MYATTR_PageSet_ActivePage,o);
			++p;
		    }
		    psSetSpacing(cl,obj);
		    DoMethod2(obj,MUIM_Group_ExitChange);
		}
		set(obj,MYATTR_PageSet_Quiet,FALSE);
	    }
	    break;

	  case MYATTR_PageSet_Zoom:
	    DB(kprintf("PageSet: zoom=%ld new %ld\n",dat->zoom,tag->ti_Data);)
	    dat->zoom=tag->ti_Data;
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		nnset(obj,MYATTR_DocBitmap_Zoom,tag->ti_Data);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_DPI:
	    DB(kprintf("PageSet: dpi=%ld new %ld\n",dat->dpi,tag->ti_Data);)
	    dat->dpi=tag->ti_Data;
	    dat->zoom=zoomCustom;
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		nnset(obj,MYATTR_DocBitmap_DPI,tag->ti_Data);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_Rotate:
	    dat->rotate=tag->ti_Data;
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		nnset(obj,MYATTR_DocBitmap_Rotate,tag->ti_Data);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_CropBox:
	    dat->box=*(struct MYS_DocBitmap_CropBox *)tag->ti_Data;
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		nnset(obj,MYATTR_DocBitmap_CropBox,tag->ti_Data);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_DrawMode:
	    dat->draw_mode=tag->ti_Data;
	    set(obj,MYATTR_DocBitmap_DrawMode,tag->ti_Data);
	    break;

	  case MYATTR_PageSet_Document:
	    DB(kprintf("PageSet::set document %p (was %p)\n",tag->ti_Data,dat->doc);)
	    if(dat->doc!=(Object *)tag->ti_Data) {
		int p,t;
		APTR state;
		struct List *list;
		Object* o;
		++dat->locked;
		set(obj,MYATTR_PageSet_Quiet,TRUE);
		dat->doc=(Object*)tag->ti_Data;
		get(dat->doc,MYATTR_Document_NumPages,&dat->num_pages);
		set(obj,MYATTR_DocBitmap_Document,tag->ti_Data);
		if(get(dat->doc,MYATTR_Document_OpenColumns,&t)) {
		    set(obj,MYATTR_PageSet_Columns,t);
		    columns=dat->columns;
		}
		if(get(dat->doc,MYATTR_Document_OpenRows,&t)) {
		    set(obj,MYATTR_PageSet_Rows,t);
		    rows=dat->rows;
		}
		if(get(dat->doc,MYATTR_Document_OpenPage,&t))
		    dat->cur_page=t;
		get(obj,MUIA_Group_ChildList,&list);
		get(dat->bitmapobj,MYATTR_DocBitmap_Page,&dat->cur_page);
		get(dat->bitmapobj,MYATTR_DocBitmap_Zoom,&t);
		DB(kprintf("PageSet::set document zoom %d\n",t);)
		nnset(dat->bitmapobj,MYATTR_DocBitmap_Zoom,t);
		p=dat->cur_page;
		if(dat->strip_mode)
		    p=(p-1)/columns*columns-dat->skip+1;
		dat->page=p;
		state=list->lh_Head;
		while(o=NextObject(&state)) {
		    nnset(o,MYATTR_DocBitmap_Page,p);
		    if(p==dat->cur_page)
			set(obj,MYATTR_PageSet_ActivePage,o);
		    ++p;
		}
		--dat->locked;
		set(obj,MYATTR_PageSet_Quiet,FALSE);
		set(obj,MYATTR_PageSet_DocChanged,dat->doc);
	    }
	    break;

#if HAS_AA
	  case MYATTR_PageSet_TextAA:
	    dat->textaa=tag->ti_Data;
	    nnset(obj,MYATTR_DocBitmap_TextAA,tag->ti_Data);
	    break;

	  case MYATTR_PageSet_StrokeAA:
	    dat->strokeaa=tag->ti_Data;
	    nnset(obj,MYATTR_DocBitmap_StrokeAA,tag->ti_Data);
	    break;

	  case MYATTR_PageSet_FillAA:
	    dat->fillaa=tag->ti_Data;
	    nnset(obj,MYATTR_DocBitmap_FillAA,tag->ti_Data);
	    break;
#endif

	  case MYATTR_PageSet_VGroup:
	    dat->vgroup=tag->ti_Data?(Object *)tag->ti_Data:obj;
	    break;

	  case MYATTR_PageSet_Ready:
	    dat->ready=tag->ti_Data;
	    break;

	  case MYATTR_PageSet_Quiet:
	    if(tag->ti_Data)
		++dat->quiet;
	    else {
		--dat->quiet;
		/*if(!dat->quiet && dat->update) {
		    MUI_Redraw(obj,MADF_DRAWUPDATE);
		    dat->update=FALSE;
		}*/
	    }
	    DB(kprintf("pageset %lx: quiet=%ld\n",obj,dat->quiet);)
	    set(obj,MYATTR_DocBitmap_Quiet,tag->ti_Data);
	    break;

	  case MYATTR_PageSet_ActivePage:
	    DB(kprintf("active=%lx (%lx)\n",tag->ti_Data,dat->bitmapobj);)
	    if(dat->bitmapobj!=(Object *)tag->ti_Data) {
		Object *o=dat->bitmapobj;
		dat->bitmapobj=(Object *)tag->ti_Data;
		if(o)
		    set(o,MYATTR_DocBitmap_Active,FALSE);
		if(dat->bitmapobj) {
		    int p;
		    set(dat->bitmapobj,MYATTR_DocBitmap_Active,TRUE);
		    get(dat->bitmapobj,MYATTR_DocBitmap_Page,&p);
		    set(obj,MYATTR_PageSet_Page,p);
		}
	    }
	    break;

	  case MYATTR_PageSet_VisWidth:
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		set(obj,MYATTR_DocBitmap_VisWidth,(tag->ti_Data-(dat->columns-1)*(SPACING+SPACING2))/dat->columns);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_VisHeight:
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
		set(obj,MYATTR_DocBitmap_VisHeight,dat->strip_mode?tag->ti_Data:
		    (tag->ti_Data-(dat->rows-1)*(SPACING+SPACING2))/dat->rows);
		DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	    break;

	  case MYATTR_PageSet_YOffset:
	    dat->yoffset=tag->ti_Data;
	    /*if(dat->yoffset<0)*/ {
		APTR state;
		struct List *list;
		int j;
		int yoffset=dat->yoffset;
		get(obj,MUIA_Group_ChildList,&list);
		state=list->lh_Head;
		for(j=0;j<dat->rows;++j) {
		    int max_height=0;
		    int i;
		    for(i=0;i<dat->columns;++i) {
			int h;
			Object *o=NextObject(&state);
			set(o,MYATTR_DocBitmap_YOffset,yoffset);
			get(o,MUIA_Height,&h);
			if(h>max_height)
			    max_height=h;
		    }
		    yoffset+=max_height+SPACING;
		}
	    }
	    /*if(dat->quiet)
		dat->update=TRUE;
	    else
		MUI_Redraw(obj,MADF_DRAWUPDATE);*/
	    break;

	  case MYATTR_PageSet_StripMode:
	    dat->strip_mode=tag->ti_Data;
	    if(dat->strip_mode)
		SetAttrs(obj,
			 MYATTR_DocBitmap_XSpacing,SPACING2,
			 MYATTR_DocBitmap_YSpacing,SPACING2,
			 TAG_END);
	    else
		psSetSpacing(cl,obj);
	    break;

	  case MYATTR_PageSet_FullScreen:
	    dat->full_screen=tag->ti_Data;
	    set(obj,MYATTR_DocBitmap_FullScreen,tag->ti_Data);
	    break;
	}
    }

    if(columns!=dat->columns || rows!=dat->rows) {
	int n1=dat->num_page_objs;
	int n2=columns*rows;

	set(obj,MYATTR_PageSet_Quiet,TRUE);
	if(n1!=n2) {
	    DB(kprintf("num_objs=%ld, new=%ld\n",n1,n2);)
	    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
	      if(obj==dat->vgroup || DoMethod2(obj,MUIM_Group_InitChange)) {
		Object *o;
		APTR state;
		struct List *list;
		int k;
		set(obj,MYATTR_PageSet_Ready,FALSE);
		while(n1<n2) {
		    o=DocBitmapObject,
			MYATTR_DocBitmap_CommInfo,dat->comm_info,
			MYATTR_DocBitmap_Document,dat->doc,
			MYATTR_DocBitmap_FullScreen,dat->full_screen,
			dat->page==-1?TAG_IGNORE:MYATTR_DocBitmap_Page,dat->page+n1,
			dat->strip_mode?MYATTR_DocBitmap_XSpacing:TAG_IGNORE,SPACING2,
			dat->strip_mode?MYATTR_DocBitmap_YSpacing:TAG_IGNORE,SPACING2,
			MYATTR_DocBitmap_DrawMode,dat->draw_mode,
			/*MYATTR_DocBitmap_Zoom,dat->zoom,
			MYATTR_DocBitmap_Rotate,dat->rotate,
			MYATTR_DocBitmap_CropBox,&dat->box,
			MYATTR_DocBitmap_TextAA,dat->textaa,
			MYATTR_DocBitmap_StrokeAA,dat->strokeaa,
			MYATTR_DocBitmap_FillAA,dat->fillaa,*/
			End;
		    if(o) {
			if(dat->bitmapobj) {
			    SetAttrs(o,
				     dat->zoom==zoomCustom?TAG_IGNORE:MYATTR_DocBitmap_Zoom,dat->zoom,
				     dat->zoom!=zoomCustom?TAG_IGNORE:MYATTR_DocBitmap_DPI,dat->dpi,
				     MYATTR_DocBitmap_Rotate,dat->rotate,
				     AA(MYATTR_DocBitmap_TextAA,dat->textaa,)
				     AA(MYATTR_DocBitmap_FillAA,dat->fillaa,)
				     AA(MYATTR_DocBitmap_StrokeAA,dat->strokeaa,)
				     MYATTR_DocBitmap_CropBox,&dat->box,
				     TAG_END);
			} else /* only happens in OM_NEW */
			    dat->bitmapobj=o;
			for(k=0;k<dat->quiet;++k)
			    set(o,MYATTR_DocBitmap_Quiet,TRUE);
			DoMethod3(obj,OM_ADDMEMBER,o);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Page,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Page,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Zoom,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Zoom,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Rotate,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Rotate,MUIV_TriggerValue);
#if HAS_AA
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_TextAA,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_TextAA,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_FillAA,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_FillAA,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_StrokeAA,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_StrokeAA,MUIV_TriggerValue);
#endif
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Message,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Message,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Run,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Run,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_URL,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_URL,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Open,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_Open,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_OpenNew,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_OpenNew,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_PageNew,MUIV_EveryTime,
				 obj,3,MUIM_Set,MYATTR_PageSet_PageNew,MUIV_TriggerValue);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Back,TRUE,
				 obj,3,MUIM_Set,MYATTR_PageSet_Back,TRUE);
			//DoMethod7(o,MUIM_Notify,MYATTR_DocBitmap_SizeChanged,TRUE,
			//         obj,1,MYM_PageSet_Resize);
			DoMethod8(o,MUIM_Notify,MYATTR_DocBitmap_Selected,TRUE,
				 obj,2,MYM_PageSet_Select,o);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Ready,TRUE,
				 obj,3,MUIM_CallHook,(APTR)&ready_hook,o);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Ready,FALSE,
				 obj,3,MUIM_CallHook,(APTR)&busy_hook,o);
			DoMethod9(o,MUIM_Notify,MYATTR_DocBitmap_Active,TRUE,
				 obj,3,MUIM_Set,MYATTR_PageSet_ActivePage,o);
		    } else {
			columns=dat->columns;
			rows=dat->rows;
			n2=dat->columns*dat->rows;
			break;
		    }
		    ++n1;
		}
		get(obj,MUIA_Group_ChildList,&list);
		state=list->lh_Head;
		for(k=0;k<n2;++k) {
		    o=NextObject(&state);
		    DB(kprintf("skip(%lx)\n",o);)
		}
		while(n2<n1) {
		    o=NextObject(&state);
		    DB(kprintf("delete(%lx)\n",o);)
		    DoMethod3(obj,OM_REMMEMBER,o);
		    if(dat->bitmapobj==o)
			dat->bitmapobj=NULL;
		    MUI_DisposeObject(o);
		    --n1;
		}
		dat->columns=columns;
		dat->rows=rows;
		dat->num_page_objs=n2;
		if(!dat->bitmapobj) {
		    state=list->lh_Head;
		    set(obj,MYATTR_PageSet_ActivePage,NextObject(&state));
		}
		set(obj,MYATTR_DocBitmap_ShowActive,n2>1);
		DB(kprintf("new num_objs=%ld\n",n2);)
		set(obj,MUIA_Group_Columns,columns);
		set(obj,MYATTR_PageSet_Ready,TRUE);
		if(obj!=dat->vgroup)
		    DoMethod2(obj,MUIM_Group_ExitChange);
	      }
	      DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
	    }
	} else {
	    dat->columns=columns;
	    dat->rows=rows;
	    set(obj,MUIA_Group_Columns,columns);
	}
	dat->skip=!(columns&1);
	psSetSpacing(cl,obj);
	set(obj,MYATTR_PageSet_Quiet,FALSE);
    }
    if(msg->MethodID==OM_NEW)
	return 0;
    else
	return DoSuperMethodA(cl,obj,(APTR)msg);
}


Static ULONG psNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    PageSetData *dat;
    struct TagItem tags[3];
    //Object* page=(Object*)GetTagData(MYATTR_PageSet_DocBitmap,0,msg->ops_AttrList);
    struct comm_info* ci=(struct comm_info *)GetTagData(MYATTR_PageSet_CommInfo,0,msg->ops_AttrList);
    if(!ci)
	return 0;

    /*tags[0].ti_Tag=MUIA_Group_LayoutHook;
    tags[0].ti_Data=(ULONG)&layout_hook;*/
    tags[0].ti_Tag=TAG_IGNORE;//MUIA_FillArea;
    tags[0].ti_Data=FALSE;
    tags[1].ti_Tag=MUIA_Group_Spacing;
    tags[1].ti_Data=SPACING;
    tags[2].ti_Tag=TAG_MORE;
    tags[2].ti_Data=(ULONG)msg->ops_AttrList;
    msg->ops_AttrList=tags;
    obj=(Object*)DoSuperMethodA(cl,obj,(APTR)msg);
    if(obj) {
	static struct TagItem tags2[]={
	    MYATTR_PageSet_Columns,1,
	    MYATTR_PageSet_Rows,1,
	    TAG_END,
	};
	dat=INST_DATA(cl,obj);
	memset(dat,0,sizeof(*dat));
	dat->comm_info=ci;
	/* first force the creation of a docbitmap */
	dat->doc=(Object *)GetTagData(MYATTR_PageSet_Document,0,tags);
	dat->page=GetTagData(MYATTR_PageSet_Page,-1,tags);
	dat->cur_page=dat->page;
	dat->vgroup=obj;
	dat->draw_mode=DrPartialUpdates;
	msg->ops_AttrList=tags2;
	psSet(cl,obj,msg);
	if(dat->columns==0 || dat->rows==0) {
	    CoerceMethod(cl,obj,OM_DISPOSE);
	    obj=NULL;
	} else {
	    struct List *list;
	    APTR state;
	    dat->doc=NULL;
	    msg->ops_AttrList=tags;
	    psSet(cl,obj,msg);
	    get(obj,MUIA_Group_ChildList,&list);
	    state=list->lh_Head;
	    dat->bitmapobj=NextObject(&state);
	    get(dat->bitmapobj,MYATTR_DocBitmap_Page,&dat->page);
	    get(dat->bitmapobj,MYATTR_DocBitmap_Zoom,&dat->zoom);
	    get(dat->bitmapobj,MYATTR_DocBitmap_Rotate,&dat->rotate);
#if HAS_AA
	    get(dat->bitmapobj,MYATTR_DocBitmap_TextAA,&dat->textaa);
	    get(dat->bitmapobj,MYATTR_DocBitmap_FillAA,&dat->fillaa);
	    get(dat->bitmapobj,MYATTR_DocBitmap_StrokeAA,&dat->strokeaa);
#endif
	    get(dat->bitmapobj,MYATTR_DocBitmap_CropBox,&dat->box);
	    DB(kprintf("PageSet::new -> zoom %d, page %d\n",dat->zoom,dat->page);)
	    dat->cur_page=dat->page;
	}
    }
    msg->ops_AttrList=(struct TagItem *)tags[2].ti_Data;
    return (ULONG)obj;
}


Static ULONG psGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_PageSet_Document:
	*(Object**)msg->opg_Storage=dat->doc;
	return TRUE;
      case MYATTR_PageSet_DocChanged:
	get(dat->bitmapobj,MYATTR_DocBitmap_DocChanged,msg->opg_Storage);
	return TRUE;
      case MYATTR_PageSet_Page:
	*msg->opg_Storage=dat->cur_page;
	return TRUE;
      case MYATTR_PageSet_Zoom:
	*msg->opg_Storage=dat->zoom;
	return TRUE;
      case MYATTR_PageSet_Rotate:
	*msg->opg_Storage=dat->rotate;
	return TRUE;
      case MYATTR_PageSet_CropBox:
	*(struct MYS_DocBitmap_CropBox *)msg->opg_Storage=dat->box;
	return TRUE;
#if HAS_AA
      case MYATTR_PageSet_TextAA:
	*msg->opg_Storage=dat->textaa;
	return TRUE;
      case MYATTR_PageSet_StrokeAA:
	*msg->opg_Storage=dat->strokeaa;
	return TRUE;
      case MYATTR_PageSet_FillAA:
	*msg->opg_Storage=dat->fillaa;
	return TRUE;
#endif
      /*case MYATTR_PageSet_NeedVGroup:
	*msg->opg_Storage=TRUE;//dat->needvgroup;
	return TRUE;*/
      case MYATTR_PageSet_Run:
      case MYATTR_PageSet_URL:
      case MYATTR_PageSet_Open:
      case MYATTR_PageSet_OpenNew:
      case MYATTR_PageSet_PageNew:
      case MYATTR_PageSet_Message:
      case MYATTR_PageSet_Back:
	*msg->opg_Storage=0;
	return TRUE;
      case MYATTR_PageSet_Ready:
	*msg->opg_Storage=dat->ready;
	return TRUE;
      case MYATTR_PageSet_Columns:
	*msg->opg_Storage=dat->columns;
	return TRUE;
      case MYATTR_PageSet_Rows:
	*msg->opg_Storage=dat->rows;
	return TRUE;
      case MYATTR_PageSet_NumPages:
	*msg->opg_Storage=dat->num_page_objs;
	return TRUE;
      case MYATTR_PageSet_ActivePage:
	*msg->opg_Storage=(ULONG)dat->bitmapobj;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

/*Static ULONG psDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    int j,y,yoffset,width;
    APTR state;
    struct List *list;

    DoSuperMethodA(cl,obj,(APTR)msg);

    if(dat->quiet)
	return 0;

    get(obj,MUIA_Group_ChildList,&list);
    state=list->lh_Head;

    y=_mtop(obj);
    width=_mwidth(obj);
    yoffset=(dat->yoffset<0?0:dat->yoffset)-y;
    DB(kprintf("psDraw: width=%ld, x=%ld, y=%ld\n", width, _mleft(obj), y);)
    for(j=0;j<dat->rows;++j) {
	int h;
	int x=_mleft(obj);
	const int xoffset=0;
	int i;
	if(j!=0) {
	    DoMethod9(obj,MUIM_DrawBackground,x,y,width,SPACING,xoffset+x,yoffset+y,0);
	    DB(kprintf("bg(%ld,%ld,%ld,%ld,%ld,%ld)\n",x,y,width,SPACING,xoffset+x,yoffset+y);)
	    y+=SPACING;
	}
	for(i=0;i<dat->columns;++i) {
	    int w;
	    Object *o=NextObject(&state);
	    get(o,MUIA_Width,&w);
	    if(i==0)
		get(o,MUIA_Height,&h);
	    else {
		DoMethod9(obj,MUIM_DrawBackground,x,y,SPACING,h,xoffset+x,yoffset+y,0);
		DB(kprintf("bg(%ld,%ld,%ld,%ld,%ld,%ld)\n",x,y,SPACING,h,xoffset+x,yoffset+y);)
		x+=SPACING;
	    }
	    x+=w;
	}
	y+=h;
    }

    return 0;
}*/

/*Static ULONG psDrawBackground(struct IClass *cl,Object *obj,struct MUIP_DrawBackground *msg) {
    struct RastPort *rp=_rp(obj);
    SetAPen(rp,0);
    RectFill(rp,msg->left,msg->top,msg->left+msg->width-1,msg->top+msg->height-1);
    return 0;
}*/

/*Static ULONG psResize(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    //if(dat->yoffset==0)
	set(obj,MYATTR_PageSet_Quiet,TRUE);
    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange))
	DoMethod(dat->vgroup,MUIM_Group_ExitChange);
    //if(dat->yoffset==0)
	SetAttrs(obj,MYATTR_PageSet_YOffset,dat->yoffset,MYATTR_PageSet_Quiet,FALSE,TAG_END);
    return 0;
}*/

Static ULONG psAddRow(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(!dat->strip_mode)
	set(obj,MYATTR_PageSet_Rows,dat->rows+1);
    return 0;
}

Static ULONG psRemRow(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(dat->rows>1 && !dat->strip_mode)
	set(obj,MYATTR_PageSet_Rows,dat->rows-1);
    return 0;
}

Static ULONG psAddColumn(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    set(obj,MYATTR_PageSet_Columns,dat->columns+1);
    return 0;
}

Static ULONG psRemColumn(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(dat->columns>1)
	set(obj,MYATTR_PageSet_Columns,dat->columns-1);
    return 0;
}

struct MYP_PageSet_Select {
    ULONG MethodID;
    Object *docbm;
};

Static ULONG psSelect(struct IClass *cl,Object *obj,struct MYP_PageSet_Select *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    APTR state;
    struct List *list;
    Object *o;

    get(obj,MUIA_Group_ChildList,&list);
    state=list->lh_Head;
    while(o=NextObject(&state)) {
	if(o!=msg->docbm)
	    nnset(o,MYATTR_DocBitmap_Selection,NULL);
    }

    set(obj,MYATTR_PageSet_ActivePage,msg->docbm);
    return 0;
}

Static ULONG psCrop(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
	struct MYS_DocBitmap_CropBox box;
	APTR state;
	struct List *list;
	Object *o;

	DoMethod2(dat->bitmapobj,MYM_DocBitmap_Crop);
	get(dat->bitmapobj,MYATTR_DocBitmap_CropBox,&box);
	get(obj,MUIA_Group_ChildList,&list);
	state=list->lh_Head;
	while((o=NextObject(&state)))
	    set(o,MYATTR_DocBitmap_CropBox,&box);
	DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
    }
    return 0;
}

Static ULONG psUnCrop(struct IClass *cl,Object *obj) {
    PageSetData *dat=INST_DATA(cl,obj);
    if(DoMethod2(dat->vgroup,MUIM_Group_InitChange)) {
	APTR state;
	struct List *list;
	Object *o;

	get(obj,MUIA_Group_ChildList,&list);
	state=list->lh_Head;
	while((o=NextObject(&state)))
	    DoMethod2(o,MYM_DocBitmap_UnCrop);
	DoMethod2(dat->vgroup,MUIM_Group_ExitChange);
    }
    return 0;
}

struct MYP_PageSet_Action {
    ULONG MethodID;
    struct Action *action;
    int shift;
};

Static ULONG psAction(struct IClass *cl,Object *obj,struct MYP_PageSet_Action *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    return DoMethod4(dat->bitmapobj,MYM_DocBitmap_Action,msg->action,msg->shift);
}

struct MYP_PageSet_VStrip {
    ULONG MethodID;
    LONG first_page;
    LONG height;
    LONG yoffset;
    LONG cur_page;
};

Static ULONG psVStrip(struct IClass *cl,Object *obj,struct MYP_PageSet_VStrip *msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    int p=msg->first_page;
    int h=0,rows=0;
    int yoffset=msg->yoffset;
    APTR state,old_state=NULL,state1;
    struct List *list;

    DB(kprintf("vstrip(%ld, %ld)\n",p,msg->height);)
    set(obj,MYATTR_PageSet_Quiet,TRUE);

    if(p>dat->page && p<dat->page+dat->num_page_objs)
	psSwap(cl,obj,p);
    else if(dat->page>p && dat->page<p+dat->num_page_objs)
	psSwap(cl,obj,p+dat->num_page_objs);

    dat->page=p;
    dat->cur_page=msg->cur_page;

    get(obj,MUIA_Group_ChildList,&list);
    state=list->lh_Head;

    while(p<=dat->num_pages && h<msg->height) {
	LONG max_height=0;
	int k;
	for(k=0;k<dat->columns;++k) {
	    Object *o;
	    LONG height;
	    state1=state;
	    o=NextObject(&state);
	    if(!o) {
		DB(kprintf("Adding an object\n");)
		set(obj,MYATTR_PageSet_Rows,dat->rows+1);
		if(old_state) {
		    state=old_state;
		    NextObject(&state);
		} else
		    state=list->lh_Head;
		state1=state;
		o=NextObject(&state);
		if(!o) {
		    h=msg->height;
		    break;
		}
	    } else {
		DB({int t;get(o,MYATTR_DocBitmap_Page,&t);kprintf("old_page=%ld\n",t);})
		nnset(o,MYATTR_DocBitmap_Page,p);
	    }
	    set(o,MYATTR_DocBitmap_YOffset,yoffset);
	    get(o,MYATTR_DocBitmap_Height,&height);
	    if(p==msg->cur_page)
		dat->bitmapobj=o;
	    nnset(o,MYATTR_DocBitmap_Active,p==msg->cur_page);
	    DB(kprintf("o=%lx, page=%ld, h=%ld, page_height=%ld\n",o,p,h,height);)
	    if(height>max_height)
		max_height=height;
	    ++p;
	    old_state=state1;
	}
	++rows;
	h+=max_height+SPACING;
	yoffset+=max_height+SPACING;
    }
    nnset(obj,MYATTR_PageSet_Rows,rows);
    //set(obj,MYATTR_PageSet_Page,msg->first_page);
    set(obj,MYATTR_PageSet_Quiet,FALSE);
    return 0;
}

BEGIN_DISPATCHER(psDispatcher,cl,obj,msg) {
    PageSetData *dat=INST_DATA(cl,obj);
    switch(msg->MethodID) {
      case OM_NEW:                    return psNew    (cl,obj,(APTR)msg);
      //case OM_DISPOSE:                return psDispose(cl,obj,msg);
      case OM_GET:                    return psGet    (cl,obj,(APTR)msg);
      case OM_SET:                    return psSet    (cl,obj,(APTR)msg);
      //case MUIM_Draw:                 return psDraw   (cl,obj,(APTR)msg);
      //case MUIM_DrawBackground:       return psDrawBackground(cl,obj,(APTR)msg);
      case MYM_PageSet_ToClip:        return DoMethod2   (dat->bitmapobj,MYM_DocBitmap_ToClip);
      //case MYM_PageSet_Search:        return DoMethod2 (dat->bitmapobj,MYM_DocBitmap_Search);
      case MYM_PageSet_RLeft:         return DoMethod2 (dat->bitmapobj,MYM_DocBitmap_RLeft);
      case MYM_PageSet_RRight:        return DoMethod2 (dat->bitmapobj,MYM_DocBitmap_RRight);
      case MYM_PageSet_Crop:          return psCrop     (cl,obj);
      case MYM_PageSet_UnCrop:        return psUnCrop   (cl,obj);
      case MYM_PageSet_Refresh:       return DoMethod2 (obj,MYM_DocBitmap_Refresh);
      case MYM_PageSet_Action:        return psAction (cl,obj,(APTR)msg);
      //case MYM_PageSet_Resize:        return psResize   (cl,obj);
      case MYM_PageSet_AddRow:        return psAddRow   (cl,obj);
      case MYM_PageSet_RemRow:        return psRemRow   (cl,obj);
      case MYM_PageSet_AddColumn:     return psAddColumn(cl,obj);
      case MYM_PageSet_RemColumn:     return psRemColumn(cl,obj);
      case MYM_PageSet_Select:        return psSelect   (cl,obj,(APTR)msg);
      case MYM_PageSet_VStrip:        return psVStrip   (cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(psDispatcher)

static int count;

BOOL createPageSetClass(void) {
    if(count++==0) {
	pageset_mcc=MUI_CreateCustomClass(NULL,MUIC_Group,NULL,sizeof(PageSetData),(APTR)&psDispatcher);
	if(pageset_mcc)
	    return TRUE;
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deletePageSetClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(pageset_mcc);
    }
    return TRUE;
}


