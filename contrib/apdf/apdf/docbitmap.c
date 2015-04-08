/*************************************************************************\
 *                                                                       *
 * docbitmap.c                                                           *
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
#include <mui/textinput_mcc.h>
#include <cybergraphx/cybergraphics.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <devices/inputevent.h>
#include <proto/muimaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
#include <proto/utility.h>
#include <proto/iffparse.h>
#include <proto/keymap.h>
#include <proto/cybergraphics.h>
#include "AComm.h"
#include "Apdf.h"
#include "docbitmap.h"
#include "document.h"
#include "doclens.h"
#include "prefs.h"
#include "gfxchunk.h"

/* New for MUI 3.9+ */
#ifndef MUI_EHF_GUIMODE
#define MUI_EHF_GUIMODE (1<<1)
#endif

#define DB(x)       //x
#define Static      static
#define MEMDBG(x)   //x

#define kprintf dprintf

#if HAS_AA
#   define AA(x,y,z) x,y,z
#else
#   define AA(x,y,z)
#endif

#define USE_PENS	0
#define USE_MAP		1

#define SPACING 4

struct rect_update {
    struct rect_update *next;
    int x,y,w,h;
    int done;
};

#define ANNOT_ENTER     0
#define ANNOT_EXIT      1
#define ANNOT_DOWN      2
#define ANNOT_UP        3

struct annot_change {
    struct annot_change *next;
    struct Annot *annot;
    int state;
};

#define DRAW_ANNOT      1
#define DRAW_SELECTION  2
#define DRAW_ALL        4
#define DRAW_PARTIAL    8
#define DRAW_FRAME      16

struct DocBitmapData {
    int num_pages;
    struct ColorMap* colormap;
    struct comm_info *comm_info;
    Object *pagegfx;
    Object *update_gfx;
    Object *doc;
    Object *docchanged;
    Object *lens;
    struct Rectangle selection;
    struct Rectangle old_selection;
    struct MYS_GfxChunk_Params prms;
    int page_width;
    int page_height;
    int page_rotate;
    int zoom;
    int zoomdpi;
    int last_width;
    int last_height;
    unsigned char *pixarray;
    struct BitMap *bitmap;
    struct BitMap *tempbm1;
    struct BitMap *tempbm2;
    int tempbmwidth;
    struct Annot *annot;
    struct AnnotInfo *annots;
    struct annot_change *annot_change;
    struct annot_change **last_annot_change;
    struct rect_update *updates;
    struct rect_update **last_update;
    int num_annots;
    void* last_action;
#if USE_PENS
    ULONG* pens;
    int curpen;
    int maxpen;
#endif
    int depth;
    BOOL hidden;
    BOOL selected;
    BOOL selecting;
    BOOL select_displayed;
    int cropx1;
    int cropy1;
    int cropx2;
    int cropy2;
#if HAS_AA
    int textAAlevel;
    int strokeAAlevel;
    int fillAAlevel;
#endif
    LONG select_color;
    int last_lens_x;
    int last_lens_y;
    BOOL need_annots;
    BOOL adjusting;
    int quiet;
    int draw_flags;
    Object *text_obj;
    Object *str_obj;
    Object *list_obj;
    void *field;
    BOOL redraw_pending;
    BOOL update_pending;
    int bleft;
    int btop;
    int bright;
    int bbottom;
    int xspacing;
    int yspacing;
    int vis_width;
    int vis_height;
    BOOL is_cgfx;
    BOOL ihn_added;
    struct MUI_InputHandlerNode ihn;
    BOOL ready;
    BOOL lens_mode;
    BOOL active;
    BOOL show_active;
    int bgx;
    int bgy;
    enum DrawMode draw_mode;
    BOOL shown;
    BOOL full_screen;
    struct MUI_EventHandlerNode ehn;
#if USE_MAP
    UBYTE map[16];
#endif
};

/* zoom factor is 1.2 (similar to DVI magsteps) */
int zoomDPI[maxStdZoom-minZoom+1]={
  29, 35, 42, 50, 60,
  72,
  86, 104, 124, 149, 179
};

void sleep(Object *obj,BOOL x) {
    if(obj)
	get(obj,MUIA_ApplicationObject,&obj);
    if(obj)
	set(obj,MUIA_Application_Sleep,x);
}

/* workaround for a bug in egcs */
void MyWritePixelArray(APTR src,UWORD sx,UWORD sy,UWORD mod,struct RastPort* rp,UWORD x,UWORD y,UWORD w,UWORD h,UBYTE fmt);
void MyWritePixelArray8(struct RastPort *rp,int x1,int y1,int x2,int y2,UBYTE *p,struct RastPort *temprp);
void MyBltBitMapRastPort(struct BitMap *bm,int x0,int y0,struct RastPort *rp,int x,int y,int w,int h,int minterm);

/*
 *  Server interface functions.
 */

Static int get_page_size(DocBitmapData *dat) {
    int r;
    if(dat->prms.pdfdoc && dat->prms.page>=1 && dat->prms.page<=dat->num_pages) {
	struct comm_info *ci=dat->comm_info;
	struct msg_pagesize* p=ci->cmddata;
	sync();
	p->doc=dat->prms.pdfdoc;
	p->page=dat->prms.page;
	r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_PAGESIZE);
	dat->page_width=p->width;
	dat->page_height=p->height;
	dat->page_rotate=p->rotate;
    } else {
	dat->page_width=10;
	dat->page_height=10;
	dat->page_rotate=0;
	r=0;
    }
    return r;
}

Static void create_output_dev(DocBitmapData* dat) {
    struct comm_info *ci=dat->comm_info;
    struct msg_create_output_dev* p=ci->cmddata;
    sync();
    p->depth=dat->depth;
    p->map_mode=0;
    memcpy(p->map,dat->map,16);
    p->port=ci->sv->reply_port;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_CREATEOUTPUTDEV))
	dat->prms.aout=p->out;
    else
	dat->prms.aout=NULL;
}

Static void delete_output_dev(DocBitmapData *dat) {
    struct comm_info *ci=dat->comm_info;
    struct msg_delete_output_dev* p=ci->cmddata;
    sync();
    p->out=dat->prms.aout;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_DELETEOUTPUTDEV);
}

#if HAS_AA
Static void set_AA_levels(DocBitmapData *dat) {
    struct comm_info *ci=dat->comm_info;
    struct msg_setaalevels* p=ci->cmddata;
    sync();
    p->out=dat->prms.aout;
    p->text=dat->textAAlevel;
    p->stroke=dat->strokeAAlevel;
    p->fill=dat->fillAAlevel;
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SETAALEVELS);
}
#endif

Static int simple_find(DocBitmapData *dat,const char* str,int top,int* xmin,int* ymin,int* xmax,int* ymax) {
    struct comm_info *ci=dat->comm_info;
    struct msg_find* p=ci->cmddata;
    int r=0;
    sync();
    if(dat->prms.aout) {
	p->aout=dat->prms.aout;
	p->top=top;
	p->xmin=*xmin;
	p->ymin=*ymin;
	p->xmax=*xmax;
	p->ymax=*ymax;
	strcpy(p->str,str);
	r=(*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SIMPLEFIND);
	*xmin=p->xmin;
	*ymin=p->ymin;
	*xmax=p->xmax;
	*ymax=p->ymax;
    }
    return r;
}

Static char* gettext(DocBitmapData *dat,int xmin,int ymin,int xmax,int ymax) {
    char* r=NULL;
    struct comm_info *ci=dat->comm_info;
    struct msg_gettext* p=ci->cmddata;
    if(dat->prms.aout) {
	sync();
	p->aout=dat->prms.aout;
	p->xmin=xmin;
	p->ymin=ymin;
	p->xmax=xmax;
	p->ymax=ymax;
	LOCK; /*init_exchange(dat->comm_info);*/
	if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETTEXT)) {
	    char* buf=NULL;
	    size_t sz=p->size+1;
	    char* q;
	    if(sz>sizeof(union msg_max)) {
		buf=(*ci->sv->MyAllocSharedMem)(ci->sv,sz);
		if(buf)
		    q=buf;
		else {
		    q=dat->comm_info->cmddata;
		    sz=sizeof(union msg_max);
		}
	    } else
		q=dat->comm_info->cmddata;
	    (*ci->sv->exchange_raw_msg)(ci,q,sz,MSGID_GETTEXT);
	    if(r=MyAllocMem(sz))
		memcpy(r,q,sz);
	    (*ci->sv->MyFreeSharedMem)(ci->sv,buf);
	}
	UNLOCK; /*end_exchange(dat->comm_info);*/
    }
    return r;
}

Static struct AnnotInfo *get_annots(struct comm_info *ci,
				    struct PDFDoc *pdfdoc,
				    struct AOutputDev *aout,
				    int page,int *nump) {
    struct AnnotInfo * r=NULL;
    struct msg_getannots* p=ci->cmddata;
    sync();
    p->doc=pdfdoc;
    p->aout=aout;
    p->page=page;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETANNOTS)) {
	*nump=p->num;
	if(p->num) {
	    size_t sz=p->size;
	    r=(*ci->sv->MyAllocSharedMem)(ci->sv,sz);
	    (*ci->sv->exchange_raw_msg)(ci,r,sz,MSGID_GETANNOTS);
	}
    }
    return r;
}

Static struct Action *set_annot_state(struct comm_info *ci,
				      struct PDFDoc *pdfdoc,
				      struct Annot *annot,
				      int state) {
    if(annot) {
	struct msg_setannotstate* p=ci->cmddata;
	sync();
	p->pdfdoc=pdfdoc;
	p->annot=annot;
	p->state=state;
	if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SETANNOTSTATE))
	    return p->action;
    }
    return NULL;
}

Static struct Action *doaction(struct comm_info *ci,
			       struct PDFDoc *doc,
			       struct AOutputDev *aout,
			       struct Action *action,
			       size_t *sz) {
    struct msg_doaction* p=ci->cmddata;
    sync();
    p->pdfdoc=doc;
    p->aout=aout;
    p->action=action;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_DOACTION)) {
	*sz=p->size;
	return p->action;
    } else {
	*sz=0;
	return NULL;
    }
}

Static void get_events(struct comm_info *ci,
		       struct ActionInfo *buf,
		       size_t sz,
		       struct AOutputDev *aout) {
    struct msg_getevents* p=ci->cmddata;
    sync();
    p->aout=aout;
    if((*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_GETEVENTS))
	(*ci->sv->exchange_raw_msg)(ci,buf,sz,MSGID_GETEVENTS);
}

Static size_t set_textfield(struct comm_info *ci,
			    struct TextField *field,
			    const char *txt) {
    struct msg_settextfield* p=ci->cmddata;
    sync();
    p->field=field;
    strncpy(p->txt,txt,sizeof(p->txt));
    p->txt[sizeof(p->txt)-1]='\0';
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SETTEXTFIELD);
    return p->size;
}

Static size_t set_choicefield(struct comm_info *ci,
			      struct ChoiceField *field,
			      const char *txt) {
    struct msg_setchoicefield* p=ci->cmddata;
    sync();
    p->field=field;
    strncpy(p->txt,txt,sizeof(p->txt));
    p->num=-1;
    p->txt[sizeof(p->txt)-1]='\0';
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_SETCHOICEFIELD);
    return p->size;
}

#if USE_PENS
/*
 *  Pen sharing functions.
 */

/*ULONG get_pen(DocBitmapData* dat,short n) {
    DB(printf("--getpen(%d)\n",n);)
    return n<dat->maxpen?dat->pens[n]:1;
}*/

Static void clear_pens(DocBitmapData* dat) {
    ULONG* p=dat->pens;
    int n;
    for(n=dat->curpen<dat->maxpen?dat->curpen:dat->maxpen;--n>=0;)
	ReleasePen(dat->colormap,p[n]);
    dat->curpen=0;
}

ULONG get_pen(DocBitmapData* dat,ULONG r,ULONG g,ULONG b) {
    if(dat->curpen>=dat->maxpen) {
	int maxpen=((dat->curpen*3)>>1)+16;
	ULONG* p=MyAllocMem(maxpen*sizeof(*dat->pens));
	if(p) {
	    memcpy(p,dat->pens,dat->maxpen*sizeof(*dat->pens));
	    MyFreeMem(dat->pens);
	    dat->maxpen=maxpen;
	    dat->pens=p;
	}
    }
    if(dat->curpen<dat->maxpen)
	return dat->pens[dat->curpen++]=ObtainBestPenA(dat->colormap,r,g,b,NULL);
    else
	return 1;
}

void get_pens(DocBitmapData* dat,ULONG* t,int num) {
    while(--num>=0) {
	t[0]=get_pen(dat,t[1],t[2],t[3]);
	GetRGB32(dat->colormap,t[0],1,t+1);
	t+=4;
    }
}

/*void reset_pens(DocBitmapData* dat) {
    clear_pens(dat);
    add_pen(dat,0,0,0);
    add_pen(dat,0xffffffffUL,0xffffffffUL,0xffffffffUL);
}*/
#endif


Static void clear_updates(DocBitmapData *dat) {
    struct rect_update *p;
    DoMethod2(dat->update_gfx,MYM_GfxChunk_Clear);
    p=dat->updates;
    while(p) {
	struct rect_update *q=p->next;
	MyFreeMem(p);
	p=q;
    }
    dat->updates=NULL;
    dat->last_update=&dat->updates;
}

Static void clear_annots(DocBitmapData *dat) {
    if(dat->annots) {
	(dat->comm_info->sv->MyFreeSharedMem)(dat->comm_info->sv,dat->annots);
	dat->annots=NULL;
	dat->num_annots=0;
    }
}

Static void update_annot(struct IClass *cl,Object *obj,struct AnnotInfo *p,int state) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    struct rect_update *q;
    struct Action *action;
    DB(kprintf("update_annot(%lx,%ld)\n",p,state);)
    set(obj,MYATTR_DocBitmap_Message,state!=ANNOT_EXIT?p->title:NULL);
    action=set_annot_state(dat->comm_info,dat->prms.pdfdoc,p->annot,state);
    DoMethod4(obj,MYM_DocBitmap_Action,action,0);
}

Static struct AnnotInfo *find_annot_info(DocBitmapData *dat,struct Annot *annot) {
    struct AnnotInfo *p=dat->annots+dat->num_annots;
    int k;
    for(k=dat->num_annots;--k>=0;) {
	--p;
	if(p->annot==annot)
	    return p;
    }
    return NULL;
}

Static void handle_annots_changes(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    while(dat->annots && dat->annot_change) {
	struct annot_change *ac=dat->annot_change;
	struct AnnotInfo *p;
	struct Annot *annot;
	int k,state;
	dat->annot_change=dat->annot_change->next;
	if(dat->annot_change==NULL)
	    dat->last_annot_change=&dat->annot_change;
	annot=ac->annot;
	state=ac->state;
	MyFreeMem(ac);
	p=find_annot_info(dat,annot);
	if(p)
	    update_annot(cl,obj,p,state);
    }
}

Static void add_annot_change(struct IClass *cl,Object *obj,struct Annot *annot,int state) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    if(dat->annots) {
	struct AnnotInfo *p=find_annot_info(dat,annot);
	if(p)
	    update_annot(cl,obj,p,state);
    } else {
	struct annot_change *ac=MyAllocMem(sizeof(*ac));
	if(ac) {
	    ac->next=NULL;
	    ac->annot=annot;
	    ac->state=state;
	    *dat->last_annot_change=ac;
	    dat->last_annot_change=&ac->next;
	}
    }
}

Static void clear_annots_changes(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    struct annot_change *ac=dat->annot_change;
    while(ac) {
	struct annot_change *ac2=ac;
	ac=ac->next;
	MyFreeMem(ac2);
    }
    dat->last_annot_change=&dat->annot_change;
    dat->annot_change=NULL;
    dat->annot=NULL;
}

Static int mFit(struct IClass *cl,Object *obj,int *x1, int *y1, int x2, int y2, int mode) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    int dpi=-1;
    int w=x2-*x1+1;
    int h=y2-*y1+1;
    int x=*x1;
    int y=*y1;
    BOOL err;
    //kprintf("fit(%ld,%ld,%ld,%ld,%ld)\n",*x1,*y1,x2,y2,mode);
    if(w<0) {
	w=-w;
	x=x2;
    }
    if(w<1)
	w=1;
    if(h<0) {
	h=-h;
	y=y2;
    }
    if(h<1)
	h=1;
    if((dat->prms.rotate+dat->page_rotate)%180!=0) {
	int t=w;
	w=h;
	h=t;
    }
    if(mode&1)
	dpi=((dat->vis_width-dat->bleft-dat->bright/*-dat->xspacing*/)*72)/w;
    if(mode&2) {
	int hdpi=((dat->vis_height-dat->btop-dat->bbottom/*-dat->yspacing*/)*72)/h;
	if(dpi<=0 || hdpi<dpi)
	    dpi=hdpi;
    }
    if(dat->page_width*dpi>0x1fff*72)
	dpi=0x1fff*72/dat->page_width;
    if(dat->page_height*dpi>0x1fff*72)
	dpi=0x1fff*72/dat->page_height;
    *x1=(x*dpi)/72;
    *y1=(y*dpi)/72;
    //printf("dpi=%ld x1=%ld y1=%ld\n",dpi,*x1,*y1);
    return dpi;
}


Static void handle_events(struct IClass *cl,Object *obj,struct ActionInfo *p) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    while(p->id!=actionEnd) {
//kprintf("handle_events(%ld)\n",p->id);
	switch(p->id) {
	  case actionOpenDoc: {
	    struct mActionOpenDoc *t=p->action;
	    set(obj,/*t->new_window?MYATTR_DocBitmap_OpenNew:*/MYATTR_DocBitmap_Open,t->file);
	    break;
	  }
	  case actionGoTo: {
	    struct mActionGoTo *t=p->action;
	    SetAttrs(obj,
		     MYATTR_DocBitmap_Page,t->page,
		     MYATTR_DocBitmap_Rotate,0,
		     MYATTR_DocBitmap_Quiet,TRUE,
		     TAG_END);
#if 0
	    switch(t->mode) {
	      case 0: /* FitXYZ */
		//**********
		/*if(dat->vgroup)
		    SetAttrs(dat->vgroup,
			     t->x1<0?TAG_IGNORE:MUIA_Virtgroup_Left,t->x1,
			     t->y1<0?TAG_IGNORE:MUIA_Virtgroup_Top,t->y1,
			     TAG_END);*/
		break;

	      case 1: { /* FitPage */
		int x=t->x1,y=t->y1;
		int dpi=mFit(cl,obj,&x,&y,t->x2,t->y2,3);
		set(obj,MYATTR_DocBitmap_DPI,dpi);
		//**********
		/*if(dat->vgroup)
		    SetAttrs(dat->vgroup,
			     MUIA_Virtgroup_Left,x,
			     MUIA_Virtgroup_Top,y,
			     TAG_END);*/
		break;
	      }
	      case 2: { /* FitH */
		int x=t->x1,y=t->y1;
		int dpi=mFit(cl,obj,&x,&y,t->x2,t->y2,1);
		set(obj,MYATTR_DocBitmap_DPI,dpi);
		//**************
		/*if(dat->vgroup)
		    SetAttrs(dat->vgroup,
			     MUIA_Virtgroup_Left,x,
			     MUIA_Virtgroup_Top,y,
			     TAG_END);*/
		break;
	      }
	      case 3: { /* FitV */
		int x=t->x1,y=t->y1;
		int dpi=mFit(cl,obj,&x,&y,t->x2,t->y2,2);
		set(obj,MYATTR_DocBitmap_DPI,dpi);
		//********************
		/*if(dat->vgroup)
		    SetAttrs(dat->vgroup,
			     MUIA_Virtgroup_Left,x,
			     MUIA_Virtgroup_Top,y,
			     TAG_END);*/
		break;
	      }
	    }
#endif
	    set(obj,MYATTR_DocBitmap_Quiet,FALSE);
	    break;
	  }

	  case actionShowText: {
	    Object *w=WindowObject,
		MUIA_Window_Title,"Annotation",//***
		MUIA_Window_RootObject,ListviewObject,
		    MUIA_Listview_Input,FALSE,
		    MUIA_Listview_List,FloattextObject,
			ReadListFrame,
			MUIA_Floattext_Text,p->action,
			End,
		    End,
		End;
	    if(w) {
		DoMethod11(w,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			 _app(obj),5,MUIM_Application_PushMethod,obj,2,MYM_DocBitmap_DeleteObj,w);
		DoMethod3(_app(obj),OM_ADDMEMBER,w);
		set(w,MUIA_Window_Open,TRUE);
	    }
	    break;
	  }

	  case actionRefresh:
	    DoMethod2(obj,MYM_DocBitmap_Refresh);
	    break;

	  case actionUpdateAnnots: {
	    int done;
	    clear_annots(dat);
	    dat->need_annots=TRUE;
	    get(dat->pagegfx,MYATTR_GfxChunk_Done,&done);
	    if(done) {
		dat->annots=get_annots(dat->comm_info,dat->prms.pdfdoc,
				       dat->prms.aout,dat->prms.page,&dat->num_annots);
		dat->need_annots=FALSE;
		handle_annots_changes(cl,obj);
	    }
	    break;
	  }

	  case actionUpdateRect: {
	    struct mActionUpdateRect *t=p->action;
	    int x=t->x;
	    int y=t->y;
	    int w=t->width;
	    int h=t->height;
//kprintf("rect_update(%ld,%ld,%ld,%ld)\n",x,y,w,h);
	    if(x<dat->prms.xoffset)
		x=dat->prms.xoffset;
	    if(y<dat->prms.yoffset)
		y=dat->prms.yoffset;
	    if(x+w>dat->prms.xoffset+dat->prms.width)
		w=dat->prms.xoffset+dat->prms.width-x;
	    if(y+h>dat->prms.yoffset+dat->prms.height)
		h=dat->prms.yoffset+dat->prms.height-y;
//kprintf("rect_update(%ld,%ld,%ld,%ld)\n",x,y,w,h);
	    if(w>0 && h>0) {
		struct rect_update *q;
		if(q=MyAllocMem(sizeof(*q))) {
		    q->next=NULL;
		    q->x=x;
		    q->y=y;
		    q->w=w;
		    q->h=h;
		    q->done=FALSE;
		    *dat->last_update=q;
		    dat->last_update=&q->next;
		    if(q==dat->updates) {
			struct MYS_GfxChunk_Params prm;
			prm.aout=dat->prms.aout;
			prm.width=q->w;
			prm.height=q->h;
			prm.modulo=q->w;
			prm.bpp=dat->prms.bpp;
			prm.pixfmt=dat->prms.pixfmt;
			prm.friendbm=NULL;
			prm.pdfdoc=dat->prms.pdfdoc;
			prm.page=dat->prms.page;
			prm.xoffset=q->x;
			prm.yoffset=q->y;
			prm.dpi=dat->prms.dpi;
			prm.rotate=dat->prms.rotate;
			prm.level=10;
			set(dat->update_gfx,MYATTR_GfxChunk_Params,&prm);
//kprintf("starting\n");
			DoMethod2(dat->update_gfx,MYM_GfxChunk_StartDrawing);
//kprintf("done\n");
		    }
		}
	    }
	    break;
	  }

	  case actionNamed:
	    switch(((struct mActionNamed *)p->action)->id) {
	      case actionFirstPage:
		break;

	      case actionLastPage:
		break;

	      case actionNextPage:
		set(obj,MYATTR_DocBitmap_Page,dat->prms.page+1);
		//set(obj,MYATTR_DocBitmap_Message,NULL);
		break;

	      case actionPrevPage:
		set(obj,MYATTR_DocBitmap_Page,dat->prms.page-1);
		//set(obj,MYATTR_DocBitmap_Message,NULL);
		break;

	      case actionGoBack:
		set(obj,MYATTR_DocBitmap_Back,TRUE);
		//set(obj,MYATTR_DocBitmap_Message,NULL);
		break;
	    }
	    break;

	  case actionLaunch:
	    set(obj,MYATTR_DocBitmap_Run,((struct mActionLaunch *)p->action)->cmd);
	    break;

	  case actionURI:
	    set(obj,MYATTR_DocBitmap_URL,((struct mActionURI *)p->action)->uri);
	    break;

	  case actionEditText: {
	    struct mActionEditText *t=p->action;
	    int x,y,w,h;
	    if(dat->text_obj) {
		set(dat->text_obj,MUIA_Window_Open,FALSE);
		DoMethod3(_app(obj),OM_REMMEMBER,dat->text_obj);
		MUI_DisposeObject(dat->text_obj);
		dat->text_obj=NULL;
		dat->field=NULL;
	    }
	    get(obj,MUIA_LeftEdge,&x);
	    get(obj,MUIA_TopEdge,&y);
	    get(obj,MUIA_Width,&w);
	    get(obj,MUIA_Height,&h);
	    x+=(w-dat->prms.width-dat->bleft-dat->bright-dat->xspacing)/2+dat->bleft+t->x-dat->prms.xoffset;
	    y+=(h-dat->prms.height-dat->btop-dat->bbottom-dat->yspacing)/2+dat->btop+t->y-dat->prms.yoffset;
	    dat->str_obj=MUI_NewObject(t->multiline?MUIC_Textinputscroll:MUIC_Textinput,
		MUIA_Frame,/*t->multiline?MUIV_Frame_None:*/MUIV_Frame_String,
		MUIA_Textinputscroll_UseWinBorder,TRUE,
		MUIA_Textinput_Multiline,t->multiline,
		MUIA_Textinput_Contents,t->text,
		MUIA_Textinput_MaxLen,t->maxlen<1024?t->maxlen+1:1024,
		MUIA_Textinput_Secret,t->password,
		MUIA_Textinput_Format,t->quadding==1?MUIV_Textinput_Format_Center:(t->quadding==2?MUIV_Textinput_Format_Right:MUIV_Textinput_Format_Left),
		MUIA_CycleChain,TRUE,
	    TAG_END);
	    dat->text_obj=WindowObject,
		MUIA_Window_Borderless,!t->multiline,
		MUIA_Window_CloseGadget,t->multiline,
		MUIA_Window_DragBar,t->multiline,
		MUIA_Window_DepthGadget,t->multiline,
		MUIA_Window_SizeGadget,t->multiline,
		MUIA_Window_IsSubWindow,TRUE,
		MUIA_Window_RefWindow,_win(obj),
		MUIA_Window_LeftEdge,x,
		MUIA_Window_TopEdge,y,
		MUIA_Window_Width,t->width,
		MUIA_Window_Height,t->height,
		MUIA_Window_RootObject,/*HGroup,
		    MUIA_Group_Spacing,0,
		    Child,*/dat->str_obj,
		    //End,
		t->multiline?MUIA_Window_UseRightBorderScroller:TAG_IGNORE,TRUE,
		t->multiline?MUIA_Window_UseBottomBorderScroller:TAG_IGNORE,TRUE,
		End;
	    if(dat->text_obj) {
		dat->field=t->field;
		DoMethod3(_app(obj),OM_ADDMEMBER,dat->text_obj);
		DoMethod7(dat->str_obj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
			  obj,1,MYM_DocBitmap_UpdateTextField);
		if(t->multiline) {
		    DoMethod7(dat->text_obj,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,
			      obj,1,MYM_DocBitmap_UpdateTextField);
		} else {
		    DoMethod7(dat->text_obj,MUIM_Notify,MUIA_Window_Activate,FALSE,
			      obj,1,MYM_DocBitmap_UpdateTextField);
		}
		/*DoMethod9(dat->text_obj,MUIM_Notify,MUIA_Window_Activate,FALSE,
			  dat->text_obj,3,MUIM_Set,MUIA_Window_Open,FALSE);*/
		set(dat->text_obj,MUIA_Window_ActiveObject,dat->str_obj);
		set(dat->text_obj,MUIA_Window_Open,TRUE);
	    }
	    break;
	  }

	  case actionEditChoice: {
	    struct mActionEditChoice *t=p->action;
	    int x,y,w,h;
	    if(dat->text_obj) {
		set(dat->text_obj,MUIA_Window_Open,FALSE);
		DoMethod3(_app(obj),OM_REMMEMBER,dat->text_obj);
		MUI_DisposeObject(dat->text_obj);
	    }
	    dat->text_obj=NULL;
	    dat->field=NULL;
	    dat->str_obj=NULL;
	    dat->list_obj=NULL;
	    get(obj,MUIA_LeftEdge,&x);
	    get(obj,MUIA_TopEdge,&y);
	    get(obj,MUIA_Width,&w);
	    get(obj,MUIA_Height,&h);
	    x+=(w-dat->prms.width-dat->bleft-dat->bright-dat->xspacing)/2+dat->bleft+t->x-dat->prms.xoffset;
	    y+=(h-dat->prms.height-dat->btop-dat->bbottom-dat->yspacing)/2+dat->btop+t->y-dat->prms.yoffset;
	    if(t->editflag) {
		dat->str_obj=TextinputObject,
		    StringFrame,
		    MUIA_Textinput_Contents,t->value,
		    MUIA_Textinput_Format,t->quadding==1?MUIV_Textinput_Format_Center:(t->quadding==2?MUIV_Textinput_Format_Right:MUIV_Textinput_Format_Left),
		    MUIA_CycleChain,TRUE,
		    End;
	    }
	    dat->text_obj=WindowObject,
		MUIA_Window_Borderless,TRUE,
		MUIA_Window_CloseGadget,FALSE,
		MUIA_Window_DragBar,FALSE,
		MUIA_Window_DepthGadget,FALSE,
		MUIA_Window_SizeGadget,FALSE,
		MUIA_Window_IsSubWindow,TRUE,
		MUIA_Window_RefWindow,_win(obj),
		MUIA_Window_LeftEdge,x,
		MUIA_Window_TopEdge,y,
		MUIA_Window_Width,t->width,
		t->popupflag?TAG_IGNORE:MUIA_Window_Height,t->height,
		MUIA_Window_RootObject,VGroup,
		    MUIA_Group_Spacing,0,
		    Child,dat->list_obj=ListviewObject,
			MUIA_CycleChain,TRUE,
			MUIA_Listview_List,ListObject,
			    InputListFrame,
			    MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,
			    MUIA_List_DestructHook,MUIV_List_DestructHook_String,
			    MUIA_List_SourceArray,t->entries,
			    End,
			End,
		    t->editflag?MUIA_Group_Child:TAG_IGNORE,dat->str_obj,
		    End,
		End;
	    if(dat->text_obj) {
		dat->field=t->field;
		set(dat->text_obj,MUIA_Window_DefaultObject,dat->list_obj);
		DoMethod3(_app(obj),OM_ADDMEMBER,dat->text_obj);
		if(dat->str_obj) {
		    set(dat->str_obj,MUIA_String_AttachedList,dat->list_obj);
		    DoMethod7(dat->list_obj,MUIM_Notify,MUIA_List_Active,MUIV_EveryTime,
			      obj,1,MYM_DocBitmap_ChoiceActiveChg);
		    DoMethod7(dat->text_obj,MUIM_Notify,MUIA_Window_Activate,FALSE,
			      obj,1,MYM_DocBitmap_UpdateChoiceField);
		    set(dat->text_obj,MUIA_Window_ActiveObject,dat->str_obj);
		    DoMethod7(dat->str_obj,MUIM_Notify,MUIA_String_Acknowledge,MUIV_EveryTime,
			      obj,1,MYM_DocBitmap_UpdateChoiceField);
		}
		DoMethod7(dat->list_obj,MUIM_Notify,MUIA_Listview_DoubleClick,TRUE,
			  obj,1,MYM_DocBitmap_ChoiceDblClick);
		set(dat->text_obj,MUIA_Window_Open,TRUE);
	    }
	    break;
	  }
	}
	++p;
    }
}

Static void update_textfield(struct IClass *cl,Object *obj,struct TextField *f,const char *txt) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    size_t sz=set_textfield(dat->comm_info,f,txt);
    if(sz) {
	struct ActionInfo *buf=(*dat->comm_info->sv->MyAllocSharedMem)(dat->comm_info->sv,sz MEMDBG(+sizeof(unsigned)));
	if(buf) {
	    MEMDBG(*(unsigned*)((char*)buf+sz)=0xbbbbbbbb;)
	    get_events(dat->comm_info,buf,sz MEMDBG(+sizeof(unsigned)),dat->prms.aout);
	    MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow: %lx\n",*(unsigned*)((char*)buf+sz));)
	    handle_events(cl,obj,buf);
	    MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow2: %lx\n",*(unsigned*)((char*)buf+sz));)
	    (*dat->comm_info->sv->MyFreeSharedMem)(dat->comm_info->sv,buf);
	}
    }
}

Static void update_choicefield(struct IClass *cl,Object *obj,struct ChoiceField *f,const char *txt) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    size_t sz=set_choicefield(dat->comm_info,f,txt);
    if(sz) {
	struct ActionInfo *buf=(*dat->comm_info->sv->MyAllocSharedMem)(dat->comm_info->sv,sz MEMDBG(+sizeof(unsigned)));
	if(buf) {
	    MEMDBG(*(unsigned*)((char*)buf+sz)=0xbbbbbbbb;)
	    get_events(dat->comm_info,buf,sz MEMDBG(+sizeof(unsigned)),dat->prms.aout);
	    MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow: %lx\n",*(unsigned*)((char*)buf+sz));)
	    handle_events(cl,obj,buf);
	    MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow2: %lx\n",*(unsigned*)((char*)buf+sz));)
	    (*dat->comm_info->sv->MyFreeSharedMem)(dat->comm_info->sv,buf);
	}
    }
}

/*
 *  Methods.
 */

Static ULONG mUpdateTextField(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    if(dat->text_obj) {
	const char *str;
	get(dat->str_obj,MUIA_String_Contents,&str);
	update_textfield(cl,obj,dat->field,str);
	set(dat->text_obj,MUIA_Window_Open,FALSE);
	/* Don't delete, since MUI doesn't like when an object is deleted
	 * in a notification.
	DoMethod3(_app(obj),OM_REMMEMBER,dat->text_obj);
	MUI_DisposeObject(dat->text_obj);
	dat->text_obj=NULL;
	dat->field=NULL;*/
    }
    return 0;
}

Static ULONG mUpdateChoiceField(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    if(dat->text_obj) {
	const char *str;
	if(dat->str_obj)
	    get(dat->str_obj,MUIA_String_Contents,&str);
	else
	    DoMethod4(dat->list_obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&str);
	update_choicefield(cl,obj,dat->field,str);
	set(dat->text_obj,MUIA_Window_Open,FALSE);
	/* Don't delete, since MUI doesn't like when an object is deleted
	 * in a notification.
	DoMethod3(_app(obj),OM_REMMEMBER,dat->text_obj);
	MUI_DisposeObject(dat->text_obj);
	dat->text_obj=NULL;
	dat->field=NULL;*/
    }
    return 0;
}

Static ULONG mChoiceActiveChg(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    if(dat->text_obj && dat->str_obj) {
	char* entry;
	DoMethod4(dat->list_obj,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&entry);
	set(dat->str_obj,MUIA_String_Contents,entry);
    }
    return 0;
}

Static ULONG mChoiceDblClick(struct IClass *cl,Object *obj) {
    mChoiceActiveChg(cl,obj);
    return mUpdateChoiceField(cl,obj);
}

Static ULONG mRecalc(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
//    int oldw=dat->prms.width;
//    int oldh=dat->prms.height;
//static int lev=0;
//printf("recalc(%d,%d,%d)\n",lev,oldw,oldh);
    if(dat->prms.pdfdoc && dat->prms.page>=1 && dat->prms.page<=dat->num_pages) {
	int dpi;
	int w=dat->page_width-dat->cropx1+dat->cropx2;
	int h=dat->page_height-dat->cropy1+dat->cropy2;
	int cx;
	int cy;
	BOOL err;
	int valid;

	if(w<1)
	    w=1;
	if(h<1)
	    h=1;
	if((dat->prms.rotate+dat->page_rotate)%180!=0) {
	    int t=w;
	    w=h;
	    h=t;
	}
	switch((dat->prms.rotate+dat->page_rotate)%360) {
	  case 90:
	    cx=-dat->cropy2;
	    cy=dat->cropx1;
	    break;
	  case 180:
	    cx=-dat->cropx2;
	    cy=-dat->cropy2;
	    break;
	  case 270:
	    cx=dat->cropy1;
	    cy=-dat->cropx2;
	    break;
	  default:
	    cx=dat->cropx1;
	    cy=dat->cropy1;
	    break;
	}
	if(dat->zoom==zoomPage || dat->zoom==zoomWidth) {
	    if(dat->vis_width>0 && (dat->zoom==zoomWidth || dat->vis_height>0)) {
		int a=0,b=0;
		dpi=mFit(cl,obj,&a,&b,
			 dat->page_width-dat->cropx1+dat->cropx2-1,
			 dat->page_height-dat->cropy1+dat->cropy2-1,
			 dat->zoom==zoomPage?3:1);
	    } else
		dpi=zoomDPI[defZoom-minZoom];
	    dat->zoomdpi=dpi;
	} else if(dat->zoom==zoomCustom)
	    dpi=dat->zoomdpi;
	else
	    dpi=zoomDPI[dat->zoom-minZoom];
	err=dpi<=0;
	if(dat->page_width*dpi>0x1fff*72) {
	    dpi=0x1fff*72/dat->page_width;
	    err=TRUE;
	}
	if(dat->page_height*dpi>0x1fff*72) {
	    dpi=0x1fff*72/dat->page_height;
	    err=TRUE;
	}
	/*if(err)
	    printf("Zoom factor too large.\n");*/
	dat->prms.dpi=dpi;
	dat->prms.xoffset=(cx*dpi)/72;
	dat->prms.yoffset=(cy*dpi)/72;
	dat->prms.width=(w*dpi)/72;
	dat->prms.height=(h*dpi)/72;
	dat->prms.modulo=dat->prms.width;
	dat->prms.level=dat->draw_mode==DrTextFirst?0:10;
	set(dat->pagegfx,MYATTR_GfxChunk_Params,&dat->prms);
	get(dat->pagegfx,MYATTR_GfxChunk_Data,&valid);
#if USE_PENS
	if(!valid)
	    clear_pens(dat);
#endif
    } else {
	dat->prms.dpi=-1;
	dat->prms.width=10;
	dat->prms.height=10;
    }
    if(!dat->quiet) {
	clear_annots(dat);
	if(!dat->lens_mode)
	    dat->need_annots=TRUE;
    }
//    if(dat->prms.width!=oldw || dat->prms.height!=oldh)
//       set(obj,MYATTR_DocBitmap_SizeChanged,TRUE);
    return 0;
}

Static ULONG mSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    BOOL redraw=FALSE,recalc=FALSE,update=FALSE;

    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_DocBitmap_Page: {
	    int p=(int)tag->ti_Data;
	    /*if(p<1)
		p=1;
	    else if(p>dat->num_pages)
		p=dat->num_pages;*/
	    DB(kprintf("DocBitmap: page %d (was %d)\n",p,dat->prms.page);)
	    if(dat->prms.page!=p) {
		dat->prms.page=p;
		redraw=TRUE;
		recalc=TRUE;
		clear_annots_changes(cl,obj);
		dat->selecting=FALSE;
		dat->selected=FALSE;
	    }
	    break;
	  }
	  case MYATTR_DocBitmap_Zoom:
	    DB(kprintf("DocBitmap: zoom %d -> %d\n", dat->zoom, tag->ti_Data);)
	    if(dat->zoom!=(int)tag->ti_Data &&
	       tag->ti_Data>=minZoom && tag->ti_Data<maxZoom) {
		dat->zoom=(int)tag->ti_Data;
		dat->selecting=FALSE;
		dat->selected=FALSE;
		redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_DPI:
	    DB(kprintf("DocBitmap: zoomdpi %d -> %d\n", dat->zoomdpi, tag->ti_Data);)
	    if(dat->zoom!=zoomCustom || dat->prms.dpi!=tag->ti_Data) {
		dat->zoomdpi=(int)tag->ti_Data;
		dat->zoom=zoomCustom;
		redraw=TRUE;
		dat->selecting=FALSE;
		dat->selected=FALSE;
	    }
	    break;

	  case MYATTR_DocBitmap_Rotate:
	    if(dat->prms.rotate!=(int)tag->ti_Data) {
		dat->prms.rotate=(int)tag->ti_Data;
		dat->selecting=FALSE;
		dat->selected=FALSE;
		redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_CropBox: {
	    struct MYS_DocBitmap_CropBox *box=(APTR)tag->ti_Data;
	    static struct MYS_DocBitmap_CropBox zero={0,0,0,0};
	    if(!box)
		box=&zero;
	    if(dat->cropx1!=box->left || dat->cropy1!=box->top ||
	       dat->cropx2!=-box->right || dat->cropy2!=-box->bottom) {
		dat->cropx1=box->left;
		dat->cropy1=box->top;
		dat->cropx2=-box->right;
		dat->cropy2=-box->bottom;
		redraw=TRUE;
	    }
	    break;
	  }

	  case MYATTR_DocBitmap_Selection:
	    if(tag->ti_Data) {
		dat->selection=*(struct Rectangle*)tag->ti_Data;
		if(dat->selection.MinX<0)
		    dat->selection.MinX=0;
		else if(dat->selection.MinX>=dat->prms.width)
		    dat->selection.MinX=dat->prms.width-1;
		if(dat->selection.MaxX<0)
		    dat->selection.MaxX=0;
		else if(dat->selection.MaxX>=dat->prms.width)
		    dat->selection.MaxX=dat->prms.width-1;
		if(dat->selection.MinY<0)
		    dat->selection.MinY=0;
		else if(dat->selection.MinY>=dat->prms.height)
		    dat->selection.MinY=dat->prms.height-1;
		if(dat->selection.MaxY<0)
		    dat->selection.MaxY=0;
		else if(dat->selection.MaxY>=dat->prms.height)
		    dat->selection.MaxY=dat->prms.height-1;
		dat->selected=dat->selection.MinX!=dat->selection.MaxX && dat->selection.MinY!=dat->selection.MaxY;
		//redraw=TRUE;
	    } else if(dat->selected) {
		dat->selected=FALSE;
		//redraw=TRUE;
	    }
	    set(obj,MYATTR_DocBitmap_Selected,dat->selected);
	    MUI_Redraw(obj,MADF_DRAWUPDATE);
	    break;

	  case MYATTR_DocBitmap_Document:
	    if(dat->doc!=(Object*)tag->ti_Data) {
		dat->doc=(Object*)tag->ti_Data;
		dat->selected=FALSE;
		clear_updates(dat);
		DoMethod2(dat->pagegfx,MYM_GfxChunk_Clear);
		get(dat->doc,MYATTR_Document_PDFDoc,&dat->prms.pdfdoc);
		get(dat->doc,MYATTR_Document_NumPages,&dat->num_pages);
		clear_annots_changes(cl,obj);
		dat->prms.page=-1;
		redraw=TRUE;
		recalc=TRUE;
		set(obj,MYATTR_DocBitmap_DocChanged,dat->doc);
	    }
	    break;

	  case MYATTR_DocBitmap_DocChanged:
	    dat->docchanged=(Object*)tag->ti_Data;
	    break;

#if HAS_AA
	  case MYATTR_DocBitmap_TextAA:
	    if(dat->textAAlevel!=(int)tag->ti_Data) {
		dat->textAAlevel=(int)tag->ti_Data;
		set_AA_levels(dat);
		redraw=TRUE;
	    }
	    break;
	  case MYATTR_DocBitmap_StrokeAA:
	    if(dat->strokeAAlevel!=(int)tag->ti_Data) {
		dat->strokeAAlevel=(int)tag->ti_Data;
		set_AA_levels(dat);
		redraw=TRUE;
	    }
	    break;
	  case MYATTR_DocBitmap_FillAA:
	    if(dat->fillAAlevel!=(int)tag->ti_Data) {
		dat->fillAAlevel=(int)tag->ti_Data;
		set_AA_levels(dat);
		redraw=TRUE;
	    }
	    break;
#endif

	  case MYATTR_DocBitmap_DrawMode:
	    dat->draw_mode=tag->ti_Data;
	    break;

	  case MYATTR_DocBitmap_Quiet:
	    DB(kprintf("DocBitmap: quiet %d, count %d\n",tag->ti_Data,dat->quiet);)
	    if(tag->ti_Data) {
		if(redraw)
		    dat->redraw_pending=TRUE;
		++dat->quiet;
	    } else if(--dat->quiet<=0) {
		if(dat->redraw_pending) {
		    redraw=TRUE;
		    dat->redraw_pending=FALSE;
		    dat->update_pending=FALSE;
		} else if(dat->update_pending) {
		    update=TRUE;
		    dat->update_pending=FALSE;
		}
		dat->quiet=0;
	    }
	    break;

	  case MYATTR_DocBitmap_VisWidth:
	    if(dat->vis_width!=tag->ti_Data) {
		dat->vis_width=tag->ti_Data;
		redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_VisHeight:
	    if(dat->vis_height!=tag->ti_Data) {
		dat->vis_height=tag->ti_Data;
		redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_Ready:
	    dat->ready=tag->ti_Data;
	    break;

	  case MYATTR_DocBitmap_ShowActive:
	    if(dat->show_active!=tag->ti_Data) {
		dat->show_active=tag->ti_Data;
		if(dat->show_active!=dat->active) {
		    dat->draw_flags|=DRAW_FRAME;
		    update=TRUE;
		}
	    }
	    break;

	  case MYATTR_DocBitmap_Active:
	    if(dat->active!=tag->ti_Data) {
	      dat->active=tag->ti_Data;
	      dat->draw_flags|=DRAW_FRAME;
	      update=TRUE;
	    }
	    break;

	  /*case MYATTR_DocBitmap_XOffset:
	    dat->bgx=tag->ti_Data;
	    break;*/

	  case MYATTR_DocBitmap_YOffset:
	    //if(dat->bgy!=tag->ti_Data) {
		dat->bgy=tag->ti_Data;
		dat->draw_flags|=DRAW_FRAME;
		update=TRUE;
	    //}
	    break;

	  /*case MYATTR_DocBitmap_DrawBg:
	    dat->dontdrawbg=!tag->ti_Data;
	    break;*/

	  case MYATTR_DocBitmap_XSpacing:
	    if(dat->xspacing!=tag->ti_Data) {
		dat->xspacing=tag->ti_Data;
		//redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_YSpacing:
	    if(dat->yspacing!=tag->ti_Data) {
		dat->yspacing=tag->ti_Data;
		//redraw=TRUE;
	    }
	    break;

	  case MYATTR_DocBitmap_FullScreen:
	    if(tag->ti_Data!=dat->full_screen) {
		dat->full_screen=tag->ti_Data;
		if(tag->ti_Data) {
		    dat->bleft=0;
		    dat->btop=0;
		    dat->bright=0;
		    dat->bbottom=0;
		} else {
		    dat->bleft=borderpixels;
		    dat->btop=borderpixels;
		    dat->bright=borderpixels+shadowpixels;
		    dat->bbottom=borderpixels+shadowpixels;
		}
		redraw=TRUE;
	    }
	    break;
	}

    }
    if(redraw) {
	DB(kprintf("set->redraw\n");)
	set(obj,MYATTR_DocBitmap_Ready,FALSE);
	if(!dat->quiet) {
	    clear_updates(dat);
	    DoMethod2(dat->pagegfx,MYM_GfxChunk_Clear);
	}
	if(recalc)
	    get_page_size(dat);
	mRecalc(cl,obj); /* also clears annotations */
	if(!dat->quiet) {
/*kprintf("dpi=%lx, w=%ld, h=%ld, lw=%ld, lh=%ld\n",
dat->prms.dpi,dat->prms.width,dat->prms.height,dat->last_width,dat->last_height);*/
	    /*if(dat->prms.dpi==-1 || msg->MethodID==OM_NEW ||
	       (dat->prms.width==dat->last_width && dat->prms.height==dat->last_height))*/
		MUI_Redraw(obj,MADF_DRAWOBJECT);
	    /*else
		set(obj,MYATTR_DocBitmap_SizeChanged,TRUE);*/
	} else
	    dat->redraw_pending=TRUE;
    } else if(update)
	MUI_Redraw(obj,MADF_DRAWUPDATE);
    if(msg->MethodID==OM_NEW)
	return 0;
    else
	return DoSuperMethodA(cl,obj,(Msg)msg);
}


Static ULONG mNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    if(obj=(Object *)DoSuperMethodA(cl,obj,(Msg)msg)) {
	DocBitmapData* dat=INST_DATA(cl,obj);
	memset(dat,0,sizeof(*dat));
	dat->hidden=TRUE;
	dat->prms.dpi=-1;
	dat->comm_info=(struct comm_info*)GetTagData(MYATTR_DocBitmap_CommInfo,0,msg->ops_AttrList);
	dat->lens_mode=GetTagData(MYATTR_DocBitmap_LensMode,FALSE,msg->ops_AttrList);
	dat->draw_mode=GetTagData(MYATTR_DocBitmap_DrawMode,DrPartialUpdates,msg->ops_AttrList);
#if HAS_AA
	dat->textAAlevel=1;
#endif
	dat->pagegfx=GfxChunkObject,
	    MYATTR_GfxChunk_ColorAllocator,dat,
	    End;
	dat->update_gfx=GfxChunkObject,
	    MYATTR_GfxChunk_ColorAllocator,dat,
	    End;
	set(obj,MUIA_FillArea,FALSE);
	dat->bleft=borderpixels;
	dat->btop=borderpixels;
	dat->bright=borderpixels;
	dat->bbottom=borderpixels;
	if(!dat->lens_mode) {
	    dat->bright+=shadowpixels;
	    dat->bbottom+=shadowpixels;
	}
	dat->last_annot_change=&dat->annot_change;
	if(dat->comm_info && dat->pagegfx && dat->update_gfx) {
	    DoMethod7(dat->pagegfx,MUIM_Notify,MYATTR_GfxChunk_Ready,TRUE,
		      obj,1,MYM_DocBitmap_UpdatePage);
	    DoMethod9(dat->pagegfx,MUIM_Notify,MYATTR_GfxChunk_Drawing,MUIV_EveryTime,
		      obj,3,MUIM_Set,MYATTR_DocBitmap_Ready,FALSE);
	    //DoMethod9(dat->pagegfx,MUIM_Notify,MYATTR_GfxChunk_Drawing,MUIV_EveryTime,
	    //          obj,3,MUIM_Set,MYATTR_DocBitmap_Ready,MUIV_NotTriggerValue);
	    DoMethod7(dat->update_gfx,MUIM_Notify,MYATTR_GfxChunk_Ready,TRUE,
		      obj,1,MYM_DocBitmap_UpdateAnnot);
	    DoMethod9(dat->update_gfx,MUIM_Notify,MYATTR_GfxChunk_Drawing,MUIV_EveryTime,
		      obj,3,MUIM_Set,MYATTR_DocBitmap_Ready,FALSE);
	    //DoMethod9(dat->update_gfx,MUIM_Notify,MYATTR_GfxChunk_Drawing,MUIV_EveryTime,
	    //          obj,3,MUIM_Set,MYATTR_DocBitmap_Ready,MUIV_NotTriggerValue);
	    mSet(cl,obj,msg);
	} else {
	    CoerceMethod(cl,obj,OM_DISPOSE);
	    obj=NULL;
	}
    }
    return (ULONG)obj;
}

Static ULONG mDispose(struct IClass *cl,Object *obj,Msg msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    MUI_DisposeObject(dat->pagegfx);
    MUI_DisposeObject(dat->update_gfx);
#if USE_PENS
    MyFreeMem(dat->pens);
#endif
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG mGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_DocBitmap_Page:
	*msg->opg_Storage=dat->prms.page;
	return TRUE;
      case MYATTR_DocBitmap_Zoom:
	*msg->opg_Storage=dat->zoom;
	return TRUE;
      case MYATTR_DocBitmap_DPI:
	*msg->opg_Storage=dat->zoomdpi;
	return TRUE;
      case MYATTR_DocBitmap_CropBox: {
	struct MYS_DocBitmap_CropBox *box=(APTR)msg->opg_Storage;
	box->left=dat->cropx1;
	box->top=dat->cropy1;
	box->right=-dat->cropx2;
	box->bottom=-dat->cropy2;
	break;
      }
      case MYATTR_DocBitmap_Rotate:
	*msg->opg_Storage=dat->prms.rotate;
	return TRUE;
      case MYATTR_DocBitmap_Selected:
	*msg->opg_Storage=dat->selected;
	break;
      case MYATTR_DocBitmap_Selection: {
	struct Rectangle *rect=(APTR)msg->opg_Storage;
	if(dat->selected)
	    *rect=dat->selection;
	else {
	    rect->MinX=-1;
	    rect->MinY=-1;
	    rect->MaxX=-1;
	    rect->MaxY=-1;
	}
	return TRUE;
      }
      case MYATTR_DocBitmap_Document:
	*(Object**)msg->opg_Storage=dat->doc;
	return TRUE;
      case MYATTR_DocBitmap_DocChanged:
	*(Object**)msg->opg_Storage=dat->docchanged;
	return TRUE;
#if HAS_AA
      case MYATTR_DocBitmap_TextAA:
	*msg->opg_Storage=dat->textAAlevel;
	return TRUE;
      case MYATTR_DocBitmap_StrokeAA:
	*msg->opg_Storage=dat->strokeAAlevel;
	return TRUE;
      case MYATTR_DocBitmap_FillAA:
	*msg->opg_Storage=dat->fillAAlevel;
	return TRUE;
#endif
      case MYATTR_DocBitmap_Run:
      case MYATTR_DocBitmap_URL:
      case MYATTR_DocBitmap_Open:
      case MYATTR_DocBitmap_OpenNew:
      case MYATTR_DocBitmap_PageNew:
      /*case MYATTR_DocBitmap_SizeChanged:
      case MYATTR_DocBitmap_Up:
      case MYATTR_DocBitmap_Down:
      case MYATTR_DocBitmap_Left:
      case MYATTR_DocBitmap_Right:*/
      case MYATTR_DocBitmap_Back:
	*msg->opg_Storage=0;
	return TRUE;
      /*case MYATTR_DocBitmap_VisTop:
	*msg->opg_Storage=dat->vis_top;
	return TRUE;
      case MYATTR_DocBitmap_VisLeft:
	*msg->opg_Storage=dat->vis_left;
	return TRUE;*/
      case MYATTR_DocBitmap_Ready:
	*msg->opg_Storage=dat->ready;
	return TRUE;
      case MYATTR_DocBitmap_Active:
	*msg->opg_Storage=dat->active;
	return TRUE;
      case MYATTR_DocBitmap_Width:
	*msg->opg_Storage=dat->prms.width+dat->bright+dat->bleft+dat->xspacing;
	return TRUE;
      case MYATTR_DocBitmap_Height:
	*msg->opg_Storage=dat->prms.height+dat->btop+dat->bbottom+dat->yspacing;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}

Static ULONG mSetup(struct IClass *cl,Object *obj,Msg msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    struct Screen *scr;
    struct BitMap *bm;

    DB(kprintf("setup(%lx)\n",obj);)

    if(!DoSuperMethodA(cl,obj,(Msg)msg))
	return FALSE;

    scr=_screen(obj);
    dat->colormap=scr->ViewPort.ColorMap;
    bm=scr->RastPort.BitMap;

    /* Select the pixel format to use for the page gfx. The graphics
     * engine can use directly all kinds of formats from LUT8 to
     * RGBA32. If the screen's bitmap is not in a known format, fall
     * back on RGB24 and let WritePixelArray() do the conversion.
     * We could do that in all cases, but I don't want to waste memory
     * in the case of 15/16 bits modes (e.g. 1MB would be wasted in the
     * case of a 1024x1024x16 bitmap).
     */
    dat->is_cgfx=FALSE;
    if(CyberGfxBase && GetCyberMapAttr(bm,CYBRMATTR_ISCYBERGFX))
	dat->is_cgfx=TRUE;
    dat->depth=GetBitMapAttr(bm,BMA_DEPTH);
    if(dat->depth>8 && !dat->is_cgfx) // Just in case...
	dat->depth=8;
    if(dat->depth<=8) {
	dat->prms.bpp=1;
	dat->prms.pixfmt=15; // gray8
	//dat->prms.pixfmt=0;//PIXFMT_LUT8
    }
    else {
	dat->prms.bpp=GetCyberMapAttr(bm,CYBRMATTR_BPPIX);
	dat->prms.pixfmt=GetCyberMapAttr(bm,CYBRMATTR_PIXFMT);
	/* If we know about the pixel format, use it for our
	 * private pixel arrays. Otherwise, use RGB24.
	 */
	if(dat->prms.pixfmt<PIXFMT_LUT8 ||
	   dat->prms.pixfmt>PIXFMT_RGBA32) {
	    dat->prms.bpp=3;
	    dat->prms.pixfmt=PIXFMT_RGB24;
	}
	if(spare_mem && dat->prms.bpp>3) {
	    dat->prms.bpp=3;
	    dat->prms.pixfmt=PIXFMT_RGB24;
	}
    }
#if USE_MAP
    if(dat->depth<=8) {
	int k;
	for(k=0;k<16;++k)
	    dat->map[k]=ObtainBestPenA(dat->colormap,k*0x11111111,k*0x11111111,k*0x11111111,NULL);
    }
#endif
//kprintf("bpp=%ld, pixfmt=%ld, cgx=%ld\n",dat->prms.bpp,dat->prms.pixfmt,dat->is_cgfx);

    if(dat->is_cgfx)
	dat->prms.friendbm=bm;
    else
	dat->prms.friendbm=NULL;
    create_output_dev(dat);
    //set(dat->pagegfx,MYATTR_GfxChunk_Params,&dat->prms);
    dat->hidden=FALSE;
    dat->selected=FALSE;
    dat->selecting=FALSE;
    dat->pixarray=NULL;
    dat->bitmap=NULL;
#if HAS_AA
    set_AA_levels(dat);
#endif
    dat->select_color=MUI_ObtainPen(muiRenderInfo(obj),(struct MUI_PenSpec *)DoMethod3(config,MUIM_Dataspace_Find,MYCFG_Apdf_SelectColor),0);
    if(!dat->lens_mode) {
	if(is38)
	    MUI_RequestIDCMP(obj,IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS);
	else {
	    dat->ehn.ehn_Priority=0;
	    dat->ehn.ehn_Flags=MUI_EHF_GUIMODE;
	    dat->ehn.ehn_Object=obj;
	    dat->ehn.ehn_Class=NULL;
	    dat->ehn.ehn_Events=IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS;
	    DoMethod3(_win(obj),MUIM_Window_AddEventHandler,&dat->ehn);
	}
    }
    return TRUE;
}

Static ULONG mCleanup(struct IClass *cl,Object *obj,Msg msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    DB(kprintf("cleanup(%lx)\n",obj);)
    if(!dat->lens_mode) {
	if(is38)
	    MUI_RejectIDCMP(obj,IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS);
	else
	    DoMethod3(_win(obj),MUIM_Window_RemEventHandler,&dat->ehn);
    }
    MUI_ReleasePen(muiRenderInfo(obj),dat->select_color);
    FreeBitMap(dat->tempbm1);
    FreeBitMap(dat->tempbm2);
    dat->tempbm1=NULL;
    dat->tempbm2=NULL;
    dat->tempbmwidth=0;
#if USE_MAP
    if(dat->depth<=8) {
	int k;
	for(k=0;k<16;++k)
	    ReleasePen(dat->colormap,dat->map[k]);
    }
#endif
#if USE_PENS
    clear_pens(dat);
#endif
    DoMethod2(dat->pagegfx,MYM_GfxChunk_Clear);
    dat->hidden=TRUE;
    clear_updates(dat);
    clear_annots_changes(cl,obj);
    delete_output_dev(dat);
    clear_annots(dat);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG mShow(struct IClass *cl,Object *obj,Msg msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    DB(kprintf("show(%lx)\n",obj);)
    dat->shown=TRUE;
    dat->pixarray=NULL;
    dat->bitmap=NULL;
    set(dat->pagegfx,MYATTR_GfxChunk_Params,&dat->prms);
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG mHide(struct IClass *cl,Object *obj,Msg msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    DB(kprintf("hide(%lx)\n",obj);)
    dat->shown=FALSE;
    DoMethod2(obj,MYM_DocBitmap_DestroyLens);
    if(dat->ihn_added) {
	DoMethod3(_app(obj),MUIM_Application_RemInputHandler,&dat->ihn);
	dat->ihn_added=FALSE;
    }
    if(dat->text_obj) {
	set(dat->text_obj,MUIA_Window_Open,FALSE);
	DoMethod3(_app(obj),OM_REMMEMBER,dat->text_obj);
	MUI_DisposeObject(dat->text_obj);
	dat->text_obj=NULL;
	dat->field=NULL;
    }
    return DoSuperMethodA(cl,obj,msg);
}

Static ULONG mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    int w,h;
    DoSuperMethodA(cl,obj,(Msg)msg);
    mRecalc(cl,obj);
    w=dat->prms.width+dat->bright+dat->bleft+dat->xspacing;
    h=dat->prms.height+dat->bbottom+dat->btop+dat->yspacing;
    msg->MinMaxInfo->MinWidth+=/*dat->zoom==zoomPage ||
(dat->zoom==zoomWidth && !dat->adjusting)?10:*/w;
    msg->MinMaxInfo->DefWidth+=w;
    msg->MinMaxInfo->MaxWidth+=MUI_MAXMAX;
    msg->MinMaxInfo->MinHeight+=/*dat->zoom==zoomPage ||
(dat->zoom==zoomWidth && !dat->adjusting)?10:*/h;
    msg->MinMaxInfo->DefHeight+=h;
    msg->MinMaxInfo->MaxHeight+=MUI_MAXMAX;
    return 0;
}


struct DHCRMsg {
    struct Layer *layer;
    struct Rectangle bounds;
    int xoffset;
    int yoffset;
};
struct DHCRExtra {
    int x;
    int y;
    UBYTE *array;
    int bpr;
    int pixfmt;
};

BEGIN_HOOK(void,blit,struct Hook *,h,struct RastPort *,rp,struct DHCRMsg *,msg) {
    ULONG pixfmt,bpr,bpp;
    unsigned char *base;
    APTR handle;
    struct TagItem tags[5];
    struct DHCRExtra *e=h->h_Data;
    tags[0].ti_Tag=LBMI_PIXFMT;
    tags[0].ti_Data=(ULONG)&pixfmt,
    tags[1].ti_Tag=LBMI_BASEADDRESS;
    tags[1].ti_Data=(ULONG)&base,
    tags[2].ti_Tag=LBMI_BYTESPERROW;
    tags[2].ti_Data=(ULONG)&bpr;
    tags[3].ti_Tag=LBMI_BYTESPERPIX;
    tags[3].ti_Data=(ULONG)&bpp;
    tags[4].ti_Tag=TAG_END;
    handle=LockBitMapTagList(rp->BitMap,tags);
/*    kprintf("blithook: x1 = %ld, y1 = %ld, x2 = %ld, y2 = %ld, x0 = %ld, y0 = %ld\n"
	    "rp = %lx, l = %lx, bm1 = %lx, bm2 = %lx, bpp = %ld, bpr = %ld, fmt = %ld, x = %ld, y = %ld\n",
	    msg->bounds.MinX,msg->bounds.MinY,msg->bounds.MaxX,msg->bounds.MaxY,
	    msg->xoffset, msg->yoffset, rp, msg->layer, rp->BitMap,
	    msg->layer->rp->BitMap, bpp, bpr, pixfmt, e->x, e->y);*/
    if(handle) {
	if(pixfmt==e->pixfmt) {
	    int x=msg->xoffset-e->x;
	    int y=msg->yoffset-e->y;
	    int w=(msg->bounds.MaxX-msg->bounds.MinX+1)*bpp;
	    int h=msg->bounds.MaxY-msg->bounds.MinY+1;
	    int mod1=bpr;
	    int mod2=e->bpr;
	    unsigned char *p=base+msg->bounds.MinY*bpr+msg->bounds.MinX*bpp;
	    unsigned char *q=e->array+mod2*y+x*bpp;
	    while(--h>=0) {
		memcpy(p,q,w);
		p+=mod1;
		q+=mod2;
	    }
	}
	UnLockBitMap(handle);
    }
    //kprintf("handle: %lx\n", handle);
}
END_HOOK(blit)

Static void blit_bitmap(DocBitmapData* dat,int x1,int y1,struct RastPort* rp,
			int x,int y,int w,int h) {
    if(x<0) {
	w+=x;
	x1-=x;
	x=0;
    }
    if(y<0) {
	h+=y;
	y1-=y;
	y=0;
    }
    if(w<=0 || h<=0)
	return;
    DB(kprintf("blit_bitmap(%lx,%lx,%ld,%ld,%ld,%ld,%ld,%ld)\n",
	       dat->bitmap,dat->pixarray,x1,y1,x,y,w,h);)

    /* If the page is in a BitMap, blit it with BltBitMapRastPort. */
    if(dat->bitmap)
	MyBltBitMapRastPort(dat->bitmap,x1,y1,rp,x,y,w,h,0xc0);
    else if(dat->is_cgfx) {
	/* Otherwise, if we are using Cybergraphics, either blit it with
	   WritePixelArray() or copy it with DoHookClipRects(). The former
	   doesn't allow all kinds of source formats (in particular, no 16
	   bits mode is allowed, and I don't want to waste 4 bytes per
	   pixel when only 2 are needed). The latter requires us to support
	   all kinds of pixel formats (which we do ATM, but who knows, new
	   formats might appear). Use WritePixelArray() when possible,
	   and fall back on DoHookClipRects() in other cases. */
	if(dat->prms.pixfmt==PIXFMT_RGB24 ||
	   dat->prms.pixfmt==PIXFMT_ARGB32 ||
	   dat->prms.pixfmt==PIXFMT_RGBA32 ||
	   dat->prms.bpp==1)
	    MyWritePixelArray(dat->pixarray,x1,y1,dat->prms.modulo*dat->prms.bpp,
			      rp,x,y,w,h,dat->prms.bpp==1?RECTFMT_LUT8:
			      (dat->prms.pixfmt==PIXFMT_ARGB32?RECTFMT_ARGB
			      :(dat->prms.pixfmt==PIXFMT_RGBA32?RECTFMT_RGBA:
			      RECTFMT_RGB)));
	else {
	    struct Rectangle rect;
	    struct DHCRExtra e;
	    rect.MinX=x;
	    rect.MinY=y;
	    rect.MaxX=x+w-1;
	    rect.MaxY=y+h-1;
	    e.x=x;
	    e.y=y;
	    e.bpr=dat->prms.modulo*dat->prms.bpp;
	    e.pixfmt=dat->prms.pixfmt;
	    e.array=dat->pixarray+(dat->prms.modulo*y1+x1)*dat->prms.bpp;
	    blit_hook.h_Data=&e;
/*kprintf("dhcr(x0 = %ld, y0 = %ld, m = %ld, x1 = %ld, y1 = %ld, x2 = %ld, y2 = %ld)\n",
	x1, y1, dat->prms.modulo * dat->prms.bpp, x, y, x+w-1, y+h-1);*/
	    DoHookClipRects(&blit_hook, rp, &rect);
	}
    }
    else {
	/* dat->prms.bpp==1 here, and Cybergraphics is not available. */
	/*
	 *  WritePixelArray8 destroys the array, so we must copy it
	 *  first. It also doesn't allow blitting a sub-array, so we
	 *  had to do something anyway.
	 */
	/*
	 *  WritePixelArray8 doesn't work properly when the array is a
	 *  vertical line, which can happen in an indirect way through
	 *  clipping (e.g. when the user scrolls the vgroup).
	 *  To avoid that problem, we use a temporary bitmap, without
	 *  clipping, to which we blit the array. Then we blit the bitmap
	 *  to the final rastport.
	 *  This is slow, but I don't really care about AGA anymore.
	 */
	int m=(w+15)&~15;
	unsigned char *q;
	if(q=MyAllocMem(32*m)) {
	    if(dat->tempbmwidth<w) {
		WaitBlit();
		FreeBitMap(dat->tempbm1);
		FreeBitMap(dat->tempbm2);
		dat->tempbm1=AllocBitMap(m,32,dat->depth,BMF_MINPLANES,rp->BitMap);
		dat->tempbm2=AllocBitMap(m,1,dat->depth,BMF_MINPLANES,dat->tempbm1);
		if(dat->tempbm1 && dat->tempbm2)
		    dat->tempbmwidth=m;
		else {
		    dat->tempbmwidth=0;
		    FreeBitMap(dat->tempbm1);
		    FreeBitMap(dat->tempbm2);
		    dat->tempbm1=NULL;
		    dat->tempbm2=NULL;
		}
	    }
	    if(dat->tempbmwidth) {
		int m2=dat->prms.modulo;
		unsigned char *p=dat->pixarray+y1*m2+x1;
		struct RastPort temprp1=*rp;
		struct RastPort temprp2;

		temprp1.Layer=NULL;
		temprp2=temprp1;
		temprp1.BitMap=dat->tempbm1;
		temprp2.BitMap=dat->tempbm2;

		while(h>0) {
		    unsigned char *r=q;
		    int l=h<32?h:32;
		    int k=l;
		    do {
			memcpy(r,p,w);
			r+=m;
			p+=m2;
		    } while(--k);
		    WaitBlit();
		    MyWritePixelArray8(&temprp1,0,0,w-1,l-1,q,&temprp2);
		    MyBltBitMapRastPort(dat->tempbm1,0,0,rp,x,y,w,l,0xc0);
		    y+=l;
		    h-=l;
		}
	    }
	    MyFreeMem(q);
	}
    }
}

/* Workaround for a bug in gcc-2.95.1 */
Static __inline__ getwidth(Object *obj) {
    //return _width(obj);
    return _mwidth(obj);
}
Static __inline__ getheight(Object *obj) {
    //return _height(obj);
    return _mheight(obj);
}


Static ULONG mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    int x,y,w,h;
    struct RastPort *rp;
    struct rect_update *q;

    DoSuperMethodA(cl,obj,(Msg)msg);

    if(!(msg->flags&(MADF_DRAWOBJECT|MADF_DRAWUPDATE)))
	return 0;

    if(dat->quiet) {
	if(msg->flags&MADF_DRAWOBJECT)
	    dat->draw_flags|=DRAW_FRAME|DRAW_ALL;
	dat->update_pending=TRUE;
	return 0;
    }

    DB(kprintf("%lx: draw(%lx,%lx)\n",obj,msg->flags,dat->draw_flags);)

    rp=_rp(obj);
    //x=_left(obj);
    //y=_top(obj);
    x=_mleft(obj);
    y=_mtop(obj);
    //w=_mwidth(obj);
    //h=_mheight(obj);
    w=getwidth(obj);
    h=getheight(obj);

    if(dat->prms.dpi==-1)
	mRecalc(cl,obj);

    if(dat->prms.dpi==-1) {
	DB(kprintf("bad page\n");)
	DoMethod9(obj,MUIM_DrawBackground,x,y,w,h,x,dat->bgy,0);
	dat->draw_flags=0;
	return 0;
    }

    if(msg->flags&MADF_DRAWOBJECT || dat->draw_flags&DRAW_FRAME) {
	int lev,lev1=0,pen;

	/*
	 *  Draw the page frame and the surrounding background.
	 */

	if(dat->prms.width>0 && dat->prms.height>0) {
	    int bgx=x/*dat->bgx*/;
	    int bgy=dat->bgy;

	    //kprintf("draw(x=%ld, y=%ld, w=%ld, h=%ld, pw=%ld, ph=%ld)\n",x,y,w,h,dat->prms.width,dat->prms.height);
	    //kprintf("xspacing=%ld, yspacing=%ld, btop=%ld, bbottom=%ld, bleft=%ld, bright=%ld\n",
	    //        dat->xspacing,dat->yspacing,dat->btop,dat->bbottom,dat->bleft,dat->bright);

	    /*
	     * Draw the surrouning background, if any.
	     */
	    DB(kprintf("draw_frame\n");)
	    if(w>dat->prms.width+dat->bleft+dat->bright || h>dat->prms.height+dat->btop+dat->bbottom) {
		int h1=(h-dat->prms.height-dat->btop-dat->bbottom-dat->yspacing)/2;
		int h2=h-dat->prms.height-dat->btop-dat->bbottom/*-dat->yspacing*/-h1;
		int w1=(w-dat->prms.width-dat->bleft-dat->bright-dat->xspacing)/2;
		int w2=w-dat->prms.width-dat->bleft-dat->bright/*-dat->xspacing*/-w1;
		h-=h1+h2;
		if(w>0) {
		    if(h1>0)
			DoMethod9(obj,MUIM_DrawBackground,x,y,w,h1,bgx,bgy,0);
		    if(h2>0)
			DoMethod9(obj,MUIM_DrawBackground,x,y+h1+h,w,h2,bgx,bgy+h1+h,0);
		}
		w-=w1+w2;
		y+=h1;
		bgy+=h1;
		if(h>0) {
		    if(w1>0)
			DoMethod9(obj,MUIM_DrawBackground,x,y,w1,h,bgx,bgy,0);
		    if(w2>0)
			DoMethod9(obj,MUIM_DrawBackground,x+w1+w,y,w2,h,bgx+w1+w,bgy,0);
		}
		x+=w1;
		bgx+=w1;
	    }

	    /*
	     * Draw the page frame.
	     */
	    if(!dat->full_screen) {
		pen=dat->active && dat->show_active?MUIPEN(dat->select_color):_dri(obj)->dri_Pens[SHADOWPEN];
		if(dat->lens_mode) {
		    if(w>=2*borderpixels && h>=2*borderpixels) {
			SetAPen(rp,pen);
			RectFill(rp,x,y,x+w-1,y+borderpixels-1);
			RectFill(rp,x,y+borderpixels,x+borderpixels-1,y+h-borderpixels-1);
			RectFill(rp,x+w-borderpixels,y+borderpixels,x+w-1,y+h-borderpixels-1);
			RectFill(rp,x,y+h-borderpixels,x+w-1,y+h-1);
		    }
		} else if(w>=shadowpixels+2*borderpixels &&
			  h>=shadowpixels+2*borderpixels) {
		    DoMethod9(obj,MUIM_DrawBackground,x+w-shadowpixels,y,shadowpixels,shadowpixels,bgx+w-shadowpixels,bgy,0);
		    DoMethod9(obj,MUIM_DrawBackground,x,y+h-shadowpixels,shadowpixels,shadowpixels,bgx,bgy+h-shadowpixels,0);
		    SetAPen(rp,pen);
		    RectFill(rp,x,y,x+w-shadowpixels-borderpixels-1,y+borderpixels-1);
		    RectFill(rp,x,y+borderpixels,x+borderpixels-1,y+h-shadowpixels-borderpixels-1);
		    RectFill(rp,x+w-shadowpixels-borderpixels,y,x+w-shadowpixels-1,y+shadowpixels-1);
		    RectFill(rp,x,y+h-shadowpixels-borderpixels,x+shadowpixels-1,y+h-shadowpixels-1);
		    RectFill(rp,x+w-shadowpixels-borderpixels,y+shadowpixels,x+w-1,y+h-shadowpixels-borderpixels-1);
		    RectFill(rp,x+shadowpixels,y+h-shadowpixels-borderpixels,x+w-1,y+h-1);
		}
	    }
	    x+=dat->bleft;
	    y+=dat->btop;
	    //w-=dat->bleft+dat->bright;
	    //h-=dat->btop+dat->bbottom;
	}
	dat->draw_flags|=DRAW_ALL;
    } else {
	x+=(w-dat->prms.width-dat->bleft-dat->bright-dat->xspacing)/2+dat->bleft;
	y+=(h-dat->prms.height-dat->btop-dat->bbottom-dat->yspacing)/2+dat->btop;
    }
    w=dat->prms.width;
    h=dat->prms.height;

    /*
     *  Get the picture pointer. If not available, the
     *  server has not yet replied, or has not started.
     *  In that case, tell it to start rendering if needed,
     *  and leave.
     */
    dat->bitmap=NULL;
    dat->pixarray=NULL;
    if(dat->is_cgfx && !dat->lens_mode && make_bitmap)
	get(dat->pagegfx,MYATTR_GfxChunk_BitMap,&dat->bitmap);
    if(!dat->bitmap)
	get(dat->pagegfx,MYATTR_GfxChunk_Data,&dat->pixarray);
    if(!dat->bitmap && !dat->pixarray) {
	DB(kprintf("%lx: pixarray=0\n",obj);)
	if (!dat->ihn_added && dat->draw_mode==DrPartialUpdates) {
	    DoMethod2(dat->pagegfx,MYM_GfxChunk_StartDrawing);
	    dat->ihn.ihn_Object=obj;
	    dat->ihn.ihn_Method=MYM_DocBitmap_DrawPartial;
	    dat->ihn.ihn_Flags=MUIIHNF_TIMER;
	    dat->ihn.ihn_Millis=update_time;
	    DoMethod3(_app(obj),MUIM_Application_AddInputHandler,&dat->ihn);
	    dat->ihn_added=TRUE;
	    return 0;
	}
	if(dat->draw_flags&DRAW_PARTIAL && !dat->ready) {
	    get(dat->pagegfx,MYATTR_GfxChunk_PartialData,&dat->pixarray);
	}
	if(!dat->pixarray) {
	    if (!dat->lens_mode)
	        DoMethod(obj, MUIM_DrawBackground, _mleft(obj) + borderpixels, _mtop(obj) + borderpixels, _mwidth(obj) - borderpixels*2, _mheight(obj) - borderpixels*2, 0, 0, 0);
	    DoMethod2(dat->pagegfx,MYM_GfxChunk_StartDrawing);
	    return 0;
	}
    }

    /*
     *  Erase the selection box.
     */
    if(dat->select_displayed && !(dat->draw_flags&DRAW_ALL)) {
	int x1=dat->old_selection.MinX;
	int y1=dat->old_selection.MinY;
	int x2=dat->old_selection.MaxX;
	int y2=dat->old_selection.MaxY;
	DB(kprintf("erase_selection\n");)
	if(x1>x2) {
	    int t=x2;
	    x2=x1;
	    x1=t;
	}
	if(y1>y2) {
	    int t=y2;
	    y2=y1;
	    y1=t;
	}
	if(x1>=0 && x1<w && y2>=0 && y1<h) {
	    int y3=y1<0?0:y1;
	    int y4=y2>=h?h-1:y2;
	    blit_bitmap(dat,x1,y3,rp,x+x1,y+y3,1,y4-y3+1);
	} else if(x1<0)
	    x1=0;
	if(x2>=0 && x2<w && y2>=0 && y1<h) {
	    int y3=y1<0?0:y1;
	    int y4=y2>=h?h-1:y2;
	    blit_bitmap(dat,x2,y3,rp,x+x2,y+y3,1,y4-y3+1);
	} else if(x2>=w)
	    x2=w-1;
	if(x1<=x2) {
	    if(y1>=0 && y1<h)
		blit_bitmap(dat,x1,y1,rp,x+x1,y+y1,x2-x1+1,1);
	    if(y2>=0 && y2<h)
		blit_bitmap(dat,x1,y2,rp,x+x1,y+y2,x2-x1+1,1);
	}
    }

    if(dat->draw_flags&(DRAW_ALL|DRAW_PARTIAL)) {
	LONG done;
	/*
	 *  Blit the whole page.
	 */

	DB(kprintf("draw_all\n");)
	blit_bitmap(dat,0,0,rp,x,y,w,h);

	get(dat->pagegfx,MYATTR_GfxChunk_Done,&done);
	if(dat->draw_mode==DrPartialUpdates) {
	    if(!done) {
		dat->draw_flags=0;
		return 0;
	    }
	} else if(!done)
	    DoMethod2(dat->pagegfx,MYM_GfxChunk_RestartDrawing);

    } else if(dat->draw_flags&DRAW_ANNOT) {
	/*
	 *  Blit the updated rectangles.
	 */

	q=dat->updates;
	while(q && q->done) {
	    int x1=x+q->x-dat->prms.xoffset;
	    int y1=y+q->y-dat->prms.yoffset;
	    int w1=q->w;
	    int h1=q->h;
	    if(x1+w1>x+w)
		w1=x+w-x1;
	    if(y1+h1>y+h)
		h1=y+h-y1;
	    DB(kprintf("draw_annot(%ld,%ld,%ld,%ld)\n",x1,y1,w1,h1);)
	    if(w1>0 && h1>0)
		blit_bitmap(dat,q->x-dat->prms.xoffset,q->y-dat->prms.yoffset,rp,x1,y1,w1,h1);
	    q=q->next;
	}
    }

    /*
     *  Free the updated rectangles.
     */

    while((q=dat->updates) && q->done) {
	dat->updates=q->next;
	if(!dat->updates)
	    dat->last_update=&dat->updates;
	MyFreeMem(q);
    }

    /*
     *  Draw the selection box.
     */
    dat->select_displayed=dat->selected || dat->selecting;
    if(dat->select_displayed) {
	DB(kprintf("draw_selection\n");)
	SetAPen(rp,MUIPEN(dat->select_color));
	Move(rp,x+dat->selection.MinX,y+dat->selection.MinY);
	Draw(rp,x+dat->selection.MinX,y+dat->selection.MaxY);
	Draw(rp,x+dat->selection.MaxX,y+dat->selection.MaxY);
	Draw(rp,x+dat->selection.MaxX,y+dat->selection.MinY);
	Draw(rp,x+dat->selection.MinX,y+dat->selection.MinY);
	dat->old_selection=dat->selection;
    }

    /*
     *  If there are updates pending, start rendering the first one.
     */
    if(q=dat->updates) {
	struct MYS_GfxChunk_Params prm;
	DB(kprintf("starting new update\n");)
	prm.aout=dat->prms.aout;
	prm.width=q->w;
	prm.height=q->h;
	prm.modulo=q->w;
	prm.bpp=dat->prms.bpp;
	prm.pixfmt=dat->prms.pixfmt;
	prm.friendbm=NULL;
	prm.pdfdoc=dat->prms.pdfdoc;
	prm.page=dat->prms.page;
	prm.xoffset=q->x;
	prm.yoffset=q->y;
	prm.dpi=dat->prms.dpi;
	prm.rotate=dat->prms.rotate;
	prm.level=10;
	set(dat->update_gfx,MYATTR_GfxChunk_Params,&prm);
	DoMethod2(dat->update_gfx,MYM_GfxChunk_StartDrawing);
    } else
	set(obj,MYATTR_DocBitmap_Ready,TRUE);

    dat->draw_flags=0;
    return 0;
}

/*
 *  Method called when the server has finished rendering the
 *  page. We need to update the display. Beware though, it may have
 *  been stopped by sync(), so calling MUI_Redraw() directly
 *  here would cause an infinite loop. Delay that to the next
 *  time we go in the main loop.
 */
Static ULONG mUpdatePage(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    DB(kprintf("mUpdatePage\n");)
    get(dat->pagegfx,MYATTR_GfxChunk_Data,&dat->pixarray);
    if(dat->pixarray) {
	if(dat->need_annots) {
	    LONG done;
	    get(dat->pagegfx,MYATTR_GfxChunk_Done,&done);
	    if(done) {
		dat->annots=get_annots(dat->comm_info,dat->prms.pdfdoc,
				       dat->prms.aout,dat->prms.page,&dat->num_annots);
		dat->need_annots=FALSE;
		handle_annots_changes(cl,obj);
	    }
	}
	dat->draw_flags|=DRAW_ALL;
    }
    if(dat->ihn_added) {
	DoMethod3(_app(obj),MUIM_Application_RemInputHandler,&dat->ihn);
	dat->ihn_added=FALSE;
    }
    DoMethod5(_app(obj),MUIM_Application_PushMethod,obj,1,MYM_DocBitmap_Redraw);
    return 0;
}

/*
 *  Method called when the server has finished rendering a
 *  rectangle update. Copy the update in the main gfx buffer,
 *  and trigger a redraw. As above, we have to delay the redraw.
 */
Static ULONG mUpdateAnnot(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    unsigned char *p=NULL;
    struct rect_update *r=dat->updates;
    DB(kprintf("mUpdateAnnot\n");)
    while(r && r->done)
	r=r->next;
    get(dat->update_gfx,MYATTR_GfxChunk_Data,&p);
    DB(kprintf("update=%lx\n",p));
    if(p && r) {
	unsigned char *q;
	struct BitMap *bm=NULL;
	int n,mod1,mod2;

	r->done=TRUE;
	if(dat->is_cgfx && make_bitmap)
	    get(dat->pagegfx,MYATTR_GfxChunk_BitMap,&bm);
	if(bm) {
	    struct RastPort rp;
	    InitRastPort(&rp);
	    rp.BitMap=bm;
	    /* cf. blit_bitmap */
	    if(dat->prms.pixfmt==PIXFMT_RGB24 ||
	       dat->prms.pixfmt==PIXFMT_ARGB32 ||
	       dat->prms.pixfmt==PIXFMT_RGBA32 ||
	       dat->prms.pixfmt==PIXFMT_LUT8)
		MyWritePixelArray(p,0,0,r->w*dat->prms.bpp,&rp,
				  r->x-dat->prms.xoffset,r->y-dat->prms.yoffset,
				  r->w,r->h,dat->prms.bpp==1?RECTFMT_LUT8:
				  (dat->prms.pixfmt==PIXFMT_ARGB32?RECTFMT_ARGB
				   :(dat->prms.pixfmt==PIXFMT_RGBA32?RECTFMT_RGBA:
				     RECTFMT_RGB)));
	    else {
		struct Rectangle rect;
		struct DHCRExtra e;
		rect.MinX=r->x-dat->prms.xoffset;
		rect.MinY=r->y-dat->prms.yoffset;
		rect.MaxX=r->x-dat->prms.xoffset+r->w-1;
		rect.MaxY=r->y-dat->prms.yoffset+r->h-1;
		e.x=r->x-dat->prms.xoffset;
		e.y=r->y-dat->prms.yoffset;
		e.bpr=r->w*dat->prms.bpp;
		e.pixfmt=dat->prms.pixfmt;
		e.array=p;
		blit_hook.h_Data=&e;
		DoHookClipRects(&blit_hook,&rp,&rect);
	    }
	} else {
	    get(dat->pagegfx,MYATTR_GfxChunk_Data,&q);
	    if(q) {
		DB(kprintf("copying pixmap\n"));
		mod1=dat->prms.modulo*dat->prms.bpp;
		q+=(r->y-dat->prms.yoffset)*mod1+(r->x-dat->prms.xoffset)*dat->prms.bpp;
		n=r->h;
		mod2=r->w*dat->prms.bpp;
		do {
		    memcpy(q,p,mod2);
		    p+=mod2;
		    q+=mod1;
		} while(--n);
	    }
	}
	DoMethod2(dat->update_gfx,MYM_GfxChunk_Clear);
	dat->draw_flags|=DRAW_ANNOT;
    }
    DoMethod5(_app(obj),MUIM_Application_PushMethod,obj,1,MYM_DocBitmap_Redraw);
    return 0;
}

/*
 * This method is called in 'progressive' display mode, when the delay
 * between refreshes has expired.
 */
Static ULONG mDrawPartial(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    DB(kprintf("mDrawPartial\n");)
    if(!dat->ready) {
	dat->draw_flags|=DRAW_PARTIAL;
	MUI_Redraw(obj,MADF_DRAWUPDATE);
    }
    return 0;
}

Static LONG mRLeft(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    set(obj,MYATTR_DocBitmap_Rotate,(dat->prms.rotate+270)%360);
    return 0;
}

Static LONG mRRight(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    set(obj,MYATTR_DocBitmap_Rotate,(dat->prms.rotate+90)%360);
    return 0;
}

struct MYP_DocBitmap_Action {
    ULONG MethodID;
    struct Action *action;
    int shift;
};

Static ULONG mAction(struct IClass *cl,Object *obj,struct MYP_DocBitmap_Action *msg) {
    DocBitmapData* dat=INST_DATA(cl,obj);
    size_t sz;
    struct Action *a=msg->action;
    do {
	a=doaction(dat->comm_info,dat->prms.pdfdoc,dat->prms.aout,a,&sz);
	if(sz) {
	    struct ActionInfo *buf=(*dat->comm_info->sv->MyAllocSharedMem)(dat->comm_info->sv,sz MEMDBG(+sizeof(unsigned)));
	    if(buf) {
		MEMDBG(*(unsigned*)((char*)buf+sz)=0xbbbbbbbb;)
		get_events(dat->comm_info,buf,sz MEMDBG(+sizeof(unsigned)),dat->prms.aout);
		MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow: %lx\n",*(unsigned*)((char*)buf+sz));)
		handle_events(cl,obj,buf);
		MEMDBG(if(*(unsigned*)((char*)buf+sz)!=0xbbbbbbbb) kprintf("buf overflow2: %lx\n",*(unsigned*)((char*)buf+sz));)
		(*dat->comm_info->sv->MyFreeSharedMem)(dat->comm_info->sv,buf);
	    }
	}
    } while(a);
    return 0;
}

Static void compute_cropbox(DocBitmapData *dat,struct MYS_DocBitmap_CropBox *p,
			    int x,int y,int dpi) {
    int x0=(x+dat->prms.xoffset)*72/dat->prms.dpi;
    int y0=(y+dat->prms.yoffset)*72/dat->prms.dpi;
    int w=dat->page_width;
    int h=dat->page_height;
    int w0=30;//*72/dat->prms.dpi;
    int h0=30;//*72/dat->prms.dpi;
    dat->last_lens_x=x;
    dat->last_lens_y=y;
    switch((dat->prms.rotate+dat->page_rotate)%360) {
      case 90: {
	int t=x0;
	x0=y0;
	y0=h-t;
	break;
      }
      case 180:
	x0=w-x0;
	y0=h-y0;
	break;
      case 270: {
	int t=x0;
	x0=w-y0;
	y0=t;
	break;
      }
    }
    if(x0-w0<0)
	x0=w0;
    if(x0+w0>w)
	x0=w-w0;
    if(y0-h0<0)
	y0=h0;
    if(y0+h0>h)
	y0=h-h0;
    p->left=x0-w0;
    if(p->left<0)
	p->left=0;
    p->top=y0-h0;
    if(p->top<0)
	p->top=0;
    p->right=w-x0-w0;
    if(p->right<0)
	p->right=0;
    p->bottom=h-y0-h0;
    if(p->bottom<0)
	p->bottom=0;
}

Static ULONG mDestroyLens(struct IClass *cl,Object *obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    Object *app;
    get(obj,MUIA_ApplicationObject,&app);
    DoMethod3(app,OM_REMMEMBER,dat->lens);
    MUI_DisposeObject(dat->lens);
    dat->lens=NULL;
}

Static struct Annot *find_annot(DocBitmapData *dat,int x,int y) {
    struct AnnotInfo *p=dat->annots+dat->num_annots;
    int k;
    for(k=dat->num_annots;--k>=0;) {
	--p;
	if((unsigned)(x-p->x)<p->width &&
	   (unsigned)(y-p->y)<p->height)
	    return p->annot;
    }
    return NULL;
}

/* Under MUI 3.8-, this is actually a MUIM_HandleInput method */
Static ULONG mHandleEvent(struct IClass *cl,Object *obj,struct MUIP_HandleEvent *msg) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    int x=_mleft(obj);
    int y=_mtop(obj);
    int w=_mwidth(obj);
    int h=_mheight(obj);
    int containerw, containerh;
    int containerx, containery;
    Object *container;
    LONG ret=0;

    if(dat->lens_mode)
	return is38?DoSuperMethodA(cl,obj,(Msg)msg):0;

    x+=(w-dat->prms.width-dat->bleft-dat->bright-dat->xspacing)/2+dat->bleft;
    w=dat->prms.width;
    y+=(h-dat->prms.height-dat->btop-dat->bbottom-dat->yspacing)/2+dat->btop;
    h=dat->prms.height;

    // lookup containing virtgroup object

    get(obj, MUIA_Parent, &container);
    for(;;){
	int dummy;
	if (container == NULL || get(container, MUIA_Virtgroup_Left, &dummy))
	    break;
	else
	    get(container, MUIA_Parent, &container);
    }

    if (container == NULL)
	get(obj, MUIA_Parent, &container);

    containerx = _mleft(container);
    containery = _mtop(container);
    containerw = _mwidth(container);
    containerh = _mheight(container);

    #define _between(a,x,b) ((x)>=(a) && (x)<(b))
    #define _isinobject(u,v) (_between(x,(u),x+w) && _between(y,(v),y+h))
    #define _isincontainer(u,v) (_between(containerx,(u),containerx+containerw) && _between(containery,(v),containery+containerh))

    if(msg->imsg) {
	if(dat->lens) {
	    switch(msg->imsg->Class) {
	      case IDCMP_MOUSEBUTTONS:
	      case IDCMP_RAWKEY:
		DoMethod2(obj,MYM_DocBitmap_DestroyLens);
		return is38?DoSuperMethodA(cl,obj,(APTR)msg):ret;

	      case IDCMP_MOUSEMOVE: {
		int dx=dat->last_lens_x-msg->imsg->MouseX;
		int dy=dat->last_lens_y-msg->imsg->MouseY;
		if(dx<-5 || dx>5 || dy<-5 || dy>5) {
		    struct MYS_DocBitmap_CropBox box;
		    compute_cropbox(dat,&box,msg->imsg->MouseX-x,msg->imsg->MouseY-y,300);
		    set(dat->lens,MYATTR_DocLens_CropBox,&box);
		}
		return is38?DoSuperMethodA(cl,obj,(APTR)msg):ret;
	      }
	    }
	}

	switch(msg->imsg->Class) {
	  case IDCMP_MOUSEBUTTONS: {
	    int mx=msg->imsg->MouseX;
	    int my=msg->imsg->MouseY;
	    if(msg->imsg->Code==MIDDLEDOWN ||
	       (msg->imsg->Code==SELECTDOWN &&
		msg->imsg->Qualifier&IEQUALIFIER_CONTROL)) {
		if(_isinobject(mx,my)) {
		    struct MYS_DocBitmap_CropBox box;
		    int dpi=300;
		    compute_cropbox(dat,&box,mx-x,my-y,dpi);
		    dat->lens=DocLensObject,
			MYATTR_DocLens_CommInfo,dat->comm_info,
			MYATTR_DocLens_Document,dat->doc,
			MYATTR_DocLens_Page,dat->prms.page,
			MYATTR_DocLens_DPI,dpi,
			MYATTR_DocLens_Rotate,dat->prms.rotate,
			MYATTR_DocLens_CropBox,&box,
			AA(MYATTR_DocLens_TextAA,dat->textAAlevel,)
			AA(MYATTR_DocLens_StrokeAA,dat->strokeAAlevel,)
			AA(MYATTR_DocLens_FillAA,dat->fillAAlevel,)
			MUIA_Window_Screen,_screen(obj),
			MUIA_Window_IsSubWindow,TRUE,
			End;
		    if(dat->lens) {
			Object *app;
			get(obj,MUIA_ApplicationObject,&app);
			if(app) {
			    DoMethod3(app,OM_ADDMEMBER,dat->lens);
			    set(dat->lens,MUIA_Window_Open,TRUE);
			}
		    }
		}
	    } else if(msg->imsg->Code==SELECTDOWN) {
		if(_isinobject(mx,my)) {
		    struct Action* action=dat->last_action;
		    char str[256];
		    struct Annot *annot;

		    set(obj,MYATTR_DocBitmap_Active,TRUE);
		    mx-=x;
		    my-=y;
		    dat->selection.MinX=mx;
		    dat->selection.MinY=my;
		    dat->selection.MaxX=dat->selection.MinX;
		    dat->selection.MaxY=dat->selection.MinY;
		    mx+=dat->prms.xoffset;
		    my+=dat->prms.yoffset;
		    annot=find_annot(dat,mx,my);
		    if(dat->annot && annot!=dat->annot) {
			add_annot_change(cl,obj,dat->annot,ANNOT_EXIT);
			dat->annot=NULL;
		    }
		    if(annot) {
			if(!dat->annot) {
			    dat->annot=annot;
			    add_annot_change(cl,obj,annot,ANNOT_ENTER);
			}
			add_annot_change(cl,obj,annot,ANNOT_DOWN);
		    } else {
			dat->selecting=TRUE;
			dat->selected=FALSE;
			dat->draw_flags|=DRAW_SELECTION;
			MUI_Redraw(obj,MADF_DRAWUPDATE);
		    }
		} else {
		    dat->selected=FALSE;
		    dat->selecting=FALSE;
		}
	    } else if(msg->imsg->Code==SELECTUP) {
		if(dat->selecting) {
		    dat->selection.MaxX=mx-x;
		    if(dat->selection.MaxX<0)
			dat->selection.MaxX=0;
		    else if(dat->selection.MaxX>=w)
			dat->selection.MaxX=w-1;
		    dat->selection.MaxY=my-y;
		    if(dat->selection.MaxY<0)
			dat->selection.MaxY=0;
		    else if(dat->selection.MaxY>=h)
			dat->selection.MaxY=h-1;
		    dat->selected=dat->selection.MinX!=dat->selection.MaxX && dat->selection.MinY!=dat->selection.MaxY;
		    set(obj,MYATTR_DocBitmap_Selected,TRUE);
		    dat->selecting=FALSE;
		    dat->draw_flags|=DRAW_SELECTION;
		    MUI_Redraw(obj,MADF_DRAWUPDATE);
		}
		if(dat->annot && _isinobject(mx,my)) {
		    struct Annot *annot=dat->annot;
		    struct AnnotInfo *p=find_annot_info(dat,annot);
		    int k,state;

		    if(p) {
			mx-=x;
			my-=y;
			mx+=dat->prms.xoffset;
			my+=dat->prms.yoffset;
			state=(unsigned)(mx-p->x)<p->width &&
			      (unsigned)(my-p->y)<p->height;
		    } else
			state=0;
		    if(!state)
			dat->annot=NULL;
		    if(dat->annot)
			add_annot_change(cl,obj,annot,state?ANNOT_UP:ANNOT_EXIT);
		}
	    }
	    break;
	  }
	  case IDCMP_MOUSEMOVE: {
	    int mx=msg->imsg->MouseX;
	    int my=msg->imsg->MouseY;
	    if(!is_sync())
		;
	    else if(dat->selecting) {
		dat->selection.MaxX=mx-x;
		if(dat->selection.MaxX<0)
		    dat->selection.MaxX=0;
		else if(dat->selection.MaxX>=w)
		    dat->selection.MaxX=w-1;
		dat->selection.MaxY=my-y;
		if(dat->selection.MaxY<0)
		    dat->selection.MaxY=0;
		else if(dat->selection.MaxY>=h)
		    dat->selection.MaxY=h-1;
		dat->draw_flags|=DRAW_SELECTION;
		MUI_Redraw(obj,MADF_DRAWUPDATE);
	    } else if(_isincontainer(mx,my) && _isinobject(mx,my)) {
		int k;
		struct Annot *annot;

		mx-=x;
		my-=y;
		mx+=dat->prms.xoffset;
		my+=dat->prms.yoffset;
		annot=find_annot(dat,mx,my);
		if(dat->annot && annot!=dat->annot) {
		    add_annot_change(cl,obj,dat->annot,ANNOT_EXIT);
		    dat->annot=NULL;
		}
		if(annot) {
		    struct Action* action=dat->last_action;
		    if(!dat->annot)
			add_annot_change(cl,obj,annot,ANNOT_ENTER);
		    dat->annot=annot;
		    if(msg->imsg->Qualifier&IEQUALIFIER_LEFTBUTTON)
			add_annot_change(cl,obj,annot,ANNOT_DOWN);
		}
	    } else if(dat->annot) {
		add_annot_change(cl,obj,dat->annot,ANNOT_EXIT);
		dat->annot=NULL;
	    }
	    break;
	  }
	}
    }
    return is38?DoSuperMethodA(cl,obj,(APTR)msg):ret;
    #undef _between
    #undef _isinobject
}

struct MYP_DocBitmap_Search {
    ULONG MethodID;
    const char *str;
    int top;
    int *xmin;
    int *ymin;
    int *xmax;
    int *ymax;
};

Static ULONG mSearch(struct IClass *cl,Object* obj,struct MYP_DocBitmap_Search *p) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    int r;
    *p->xmin+=dat->prms.xoffset;
    *p->ymin+=dat->prms.yoffset;
    *p->xmax+=dat->prms.xoffset;
    *p->ymax+=dat->prms.yoffset;
    r=simple_find(dat,p->str,p->top,p->xmin,p->ymin,p->xmax,p->ymax);
    if(r) {
	*p->xmin-=dat->prms.xoffset;
	*p->ymin-=dat->prms.yoffset;
	*p->xmax-=dat->prms.xoffset;
	*p->ymax-=dat->prms.yoffset;
    }
    return r;
}

Static ULONG mRefresh(struct IClass *cl,Object* obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    DB(kprintf("mRefresh\n");)
    clear_updates(dat);
    clear_annots(dat);
    dat->need_annots=TRUE;
    DoMethod2(dat->pagegfx,MYM_GfxChunk_Clear);
    set(dat->pagegfx,MYATTR_GfxChunk_Params,&dat->prms);
    if(!dat->quiet)
	MUI_Redraw(obj,MADF_DRAWOBJECT);
    else {
	dat->draw_flags|=DRAW_ALL;
	dat->redraw_pending=TRUE;
    }
    return 0;
}

Static ULONG mCrop(struct IClass *cl,Object* obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    if(dat->selected) {
	int x1=dat->selection.MinX;
	int y1=dat->selection.MinY;
	int x2=dat->selection.MaxX;
	int y2=dat->selection.MaxY;
	int t1,t2;
	if(x1>x2) {
	    int t=x1;
	    x1=x2;
	    x2=t;
	}
	if(y1>y2) {
	    int t=y1;
	    y1=y2;
	    y2=t;
	}
	x1=(x1*72)/dat->prms.dpi;
	y1=(y1*72)/dat->prms.dpi;
	x2=((x2-dat->prms.width)*72)/dat->prms.dpi;
	y2=((y2-dat->prms.height)*72)/dat->prms.dpi;
	switch((dat->prms.rotate+dat->page_rotate)%360) {
	  case 90:
	    t1=x1;
	    t2=x2;
	    x1=y1;
	    x2=y2;
	    y1=-t2;
	    y2=-t1;
	    break;
	  case 180:
	    t1=x1;
	    x1=-x2;
	    x2=-t1;
	    t2=y1;
	    y1=-y2;
	    y2=-t2;
	    break;
	  case 270:
	    t1=x1;
	    t2=x2;
	    x1=-y2;
	    x2=-y1;
	    y1=t1;
	    y2=t2;
	    break;
	}
	dat->cropx1+=x1;
	dat->cropy1+=y1;
	dat->cropx2+=x2;
	dat->cropy2+=y2;
	dat->selected=FALSE;
	mRefresh(cl,obj);
    }
    return 0;
}

Static ULONG mUnCrop(struct IClass *cl,Object* obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    dat->cropx1=0;
    dat->cropy1=0;
    dat->cropx2=0;
    dat->cropy2=0;
    dat->selected=FALSE;
    mRefresh(cl,obj);
    return 0;
}

Static ULONG mToClip(struct IClass *cl,Object* obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    char* txt;
    ULONG ret=0;
    if(dat->selected) {
	int x1=dat->selection.MinX+dat->prms.xoffset;
	int y1=dat->selection.MinY+dat->prms.yoffset;
	int x2=dat->selection.MaxX+dat->prms.xoffset;
	int y2=dat->selection.MaxY+dat->prms.yoffset;
	if(x1>x2) {
	    int t=x1;
	    x1=x2;
	    x2=t;
	}
	if(y1>y2) {
	    int t=y1;
	    y1=y2;
	    y2=t;
	}
	if(txt=gettext(dat,x1,y1,x2,y2)) {
	    struct IFFHandle *iff;
	    if(iff=AllocIFF()) {
		if(iff->iff_Stream=(ULONG)OpenClipboard(PRIMARY_CLIP)) {
		    size_t sz=strlen(txt);
		    InitIFFasClip(iff);
		    if(!OpenIFF(iff,IFFF_WRITE)) {
			if(!PushChunk(iff,MAKE_ID('F','T','X','T'),ID_FORM,IFFSIZE_UNKNOWN) &&
			   !PushChunk(iff,0,MAKE_ID('C','H','R','S'),sz) &&
			   WriteChunkBytes(iff,txt,sz)==sz &&
			   !PopChunk(iff) &&
			   !PopChunk(iff))
			    ret=1;
			CloseIFF(iff);
		    }
		    CloseClipboard((struct ClipboardHandle *)iff->iff_Stream);
		}
		FreeIFF(iff);
	    }
	    MyFreeMem(txt);
	}
    }
    return ret;
}

Static ULONG mDragQuery(struct IClass *cl,Object* obj,struct MUIP_DragDrop *msg) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    Object *doc;
    get(msg->obj,MYATTR_DocBitmap_BoxGrabber,&doc);
    return doc==dat->doc?MUIV_DragQuery_Accept:MUIV_DragQuery_Refuse;
}

Static ULONG mDragDrop(struct IClass *cl,Object* obj,struct MUIP_DragDrop *msg) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    Object *doc;
    get(msg->obj,MYATTR_DocBitmap_BoxGrabber,&doc);
    if(doc==dat->doc) {
	struct MYS_DocBitmap_CropBox box;
	box.left=dat->cropx1;
	box.top=dat->cropy1;
	box.right=-dat->cropx2;
	box.bottom=-dat->cropy2;
	set(msg->obj,MYATTR_DocBitmap_CropBox,&box);
    }
    return 0;
}

Static ULONG mRedraw(struct IClass *cl,Object* obj) {
    DocBitmapData *dat=INST_DATA(cl,obj);
    DB(kprintf("mRedraw\n");)
    if(!dat->quiet) {
	LONG busy;
	get(dat->pagegfx,MYATTR_GfxChunk_Drawing,&busy);
	if(!busy) {
	    get(dat->update_gfx,MYATTR_GfxChunk_Drawing,&busy);
	    if(!busy)
		MUI_Redraw(obj,MADF_DRAWUPDATE);
	}
    } else
	dat->redraw_pending=TRUE;
    return 0;
}

struct MYP_DocBitmap_DeleteObj {
    LONG MethodID;
    Object *obj;
};

Static ULONG mDeleteObj(struct IClass *cl,Object* obj,struct MYP_DocBitmap_DeleteObj *msg) {
    set(msg->obj,MUIA_Window_Open,FALSE);
    DoMethod3(_app(obj),OM_REMMEMBER,msg->obj);
    MUI_DisposeObject(msg->obj);
    return 0;
}

BEGIN_DISPATCHER(dispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW          : return mNew        (cl,obj,(APTR)msg);
      case OM_DISPOSE      : return mDispose    (cl,obj,(APTR)msg);
      case OM_SET          : return mSet        (cl,obj,(APTR)msg);
      case OM_GET          : return mGet        (cl,obj,(APTR)msg);
      case MUIM_Setup      : return mSetup      (cl,obj,(APTR)msg);
      case MUIM_Cleanup    : return mCleanup    (cl,obj,(APTR)msg);
      case MUIM_Show       : return mShow       (cl,obj,(APTR)msg);
      case MUIM_Hide       : return mHide       (cl,obj,(APTR)msg);
      case MUIM_AskMinMax  : return mAskMinMax  (cl,obj,(APTR)msg);
      case MUIM_Draw       : return mDraw       (cl,obj,(APTR)msg);
      case MUIM_HandleInput: if(is38) return mHandleEvent(cl,obj,(APTR)msg); break;
      case MUIM_HandleEvent: if(!is38)return mHandleEvent(cl,obj,(APTR)msg); break;
      case MUIM_DragQuery  : return mDragQuery  (cl,obj,(APTR)msg);
      case MUIM_DragDrop   : return mDragDrop   (cl,obj,(APTR)msg);
      case MYM_DocBitmap_Search:        return mSearch(cl,obj,(APTR)msg);
      case MYM_DocBitmap_ToClip:        return mToClip(cl,obj);
      case MYM_DocBitmap_RLeft:         return mRLeft(cl,obj);
      case MYM_DocBitmap_RRight:        return mRRight(cl,obj);
      case MYM_DocBitmap_Crop:          return mCrop(cl,obj);
      case MYM_DocBitmap_UnCrop:        return mUnCrop(cl,obj);
      case MYM_DocBitmap_Refresh:       return mRefresh(cl,obj);
      case MYM_DocBitmap_Action:        return mAction(cl,obj,(APTR)msg);
      case MYM_DocBitmap_DestroyLens:   return mDestroyLens(cl,obj);
      case MYM_DocBitmap_Redraw:        return mRedraw(cl,obj);
      case MYM_DocBitmap_UpdatePage:    return mUpdatePage(cl,obj);
      case MYM_DocBitmap_UpdateAnnot:   return mUpdateAnnot(cl,obj);
      case MYM_DocBitmap_UpdateTextField:return mUpdateTextField(cl,obj);
      case MYM_DocBitmap_UpdateChoiceField:return mUpdateChoiceField(cl,obj);
      case MYM_DocBitmap_ChoiceDblClick:return mChoiceDblClick(cl,obj);
      case MYM_DocBitmap_ChoiceActiveChg:return mChoiceActiveChg(cl,obj);
      case MYM_DocBitmap_DrawPartial:   return mDrawPartial(cl,obj);
      case MYM_DocBitmap_DeleteObj:     return mDeleteObj(cl,obj,(APTR)msg);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(dispatcher)


struct MUI_CustomClass *docbitmap_mcc;
static int count;

BOOL createDocBitmapClass(void) {
    if(count++==0) {
	if(createGfxChunkClass()) {
	    if(docbitmap_mcc=MUI_CreateCustomClass(NULL,MUIC_Area,NULL,sizeof(DocBitmapData),(APTR)&dispatcher))
		return TRUE;
	    deleteGfxChunkClass();
	}
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteDocBitmapClass(void) {
    if(count && --count==0) {
	MUI_DeleteCustomClass(docbitmap_mcc);
    }
    return TRUE;
}

void MyWritePixelArray(APTR src,UWORD sx,UWORD sy,UWORD mod,struct RastPort* rp,UWORD x,UWORD y,UWORD w,UWORD h,UBYTE fmt) {
    WritePixelArray(src,sx,sy,mod,rp,x,y,w,h,fmt);
}

void MyWritePixelArray8(struct RastPort *rp,int x1,int y1,int x2,int y2,UBYTE *p,struct RastPort *temprp) {
    WritePixelArray8(rp,x1,y1,x2,y2,p,temprp);
}

void MyBltBitMapRastPort(struct BitMap *bm,int x0,int y0,struct RastPort *rp,int x,int y,int w,int h,int minterm) {
    BltBitMapRastPort(bm,x0,y0,rp,x,y,w,h,minterm);
}

