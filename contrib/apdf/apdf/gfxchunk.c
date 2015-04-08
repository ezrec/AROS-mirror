/*************************************************************************\
 *                                                                       *
 * gfxchunk.c                                                            *
 *                                                                       *
 * Copyright 2000-2001 Emmanuel Lesueur                                  *
 *                                                                       *
\*************************************************************************/

#include "config.h"
#include <stdlib.h>
#include <exec/types.h>
#include <intuition/classusr.h>
#include <graphics/rastport.h>
#include <libraries/mui.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/cybergraphics.h>
#include "Apdf.h"
#include "AComm.h"
#include "gfxchunk.h"

#define DB(x)       //x
#define Static      static
#define MEMDBG(x)   //x

#define USE_PENS	0

void kprintf(const char *,...);

#if USE_PENS
ULONG get_pen(void* dat,ULONG r,ULONG g,ULONG b);
void get_pens(void* dat,ULONG* t,int num);
#endif

struct GfxChunkData {
    unsigned char *pixarray;
    struct BitMap *bitmap;
    struct MYS_GfxChunk_Params prms;
    APTR color_allocator;
    struct async_msg msg;
    int returned_level;
    int last_state;
    struct RastPort rp;
    BOOL valid;
    BOOL cleared;
    BOOL drawing;
};
typedef struct GfxChunkData GfxChunkData;


Static ULONG gfxNew(struct IClass *cl,Object *obj,struct opSet *msg) {
    if(obj=(Object*)DoSuperMethodA(cl,obj,(APTR)msg)) {
	GfxChunkData *dat=INST_DATA(cl,obj);
	dat->pixarray=NULL;
	dat->bitmap=NULL;
	dat->color_allocator=(APTR)GetTagData(MYATTR_GfxChunk_ColorAllocator,0,msg->ops_AttrList);
	dat->msg.replied=TRUE;
	dat->msg.obj=obj;
	dat->msg.init_methodid=MYM_GfxChunk_SendInit;
	dat->msg.reply_methodid=MYM_GfxChunk_Replied;
	dat->msg.ci=(*sv.create_comm_info)(&sv,sizeof(union msg_max));
	dat->valid=FALSE;
	dat->drawing=FALSE;
	dat->cleared=TRUE;
	dat->last_state=TRUE;
	InitRastPort(&dat->rp);
	if(!dat->msg.ci || !dat->color_allocator) {
	    CoerceMethod(cl,obj,OM_DISPOSE);
	    obj=NULL;
	}
    }
    return (ULONG)obj;
}


Static ULONG gfxDispose(struct IClass *cl,Object *obj,Msg msg) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    if(dat->msg.ci) {
	abort_async_msg(&dat->msg);
	(*sv.delete_comm_info)(dat->msg.ci);
    }
    if(dat->pixarray)
	(*sv.MyFreeSharedMem)(&sv,dat->pixarray);
    FreeBitMap(dat->bitmap);
    return DoSuperMethodA(cl,obj,msg);
}


Static ULONG gfxSet(struct IClass *cl,Object *obj,struct opSet *msg) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    struct TagItem *tags=msg->ops_AttrList;
    struct TagItem *tag;
    while(tag=NextTagItem(&tags)) {
	switch(tag->ti_Tag) {
	  case MYATTR_GfxChunk_Params: {
	    struct MYS_GfxChunk_Params *p=&dat->prms;
	    DB(kprintf("old_params(%lx): w=%ld, h=%ld, mod=%ld, bpp=%ld\n"
		       "doc=%lx, p=%ld, x=%ld, y=%ld, dpi=%ld, r=%ld, lev=%ld\n",
		       obj,p->width,p->height,p->modulo,p->bpp,p->pdfdoc,
		       p->page,p->xoffset,p->yoffset,p->dpi,p->rotate,p->level);)
	    p=(APTR)tag->ti_Data;
	    DB(kprintf("new_params(%lx): w=%ld, h=%ld, mod=%ld, bpp=%ld\n"
		       "doc=%lx, p=%ld, x=%ld, y=%ld, dpi=%ld, r=%ld, lev=%ld\n",
		       obj,p->width,p->height,p->modulo,p->bpp,p->pdfdoc,
		       p->page,p->xoffset,p->yoffset,p->dpi,p->rotate,p->level);)
	    dat->cleared=FALSE;
	    if(dat->prms.width!=p->width ||
	       dat->prms.height!=p->height ||
	       dat->prms.modulo!=p->modulo ||
	       dat->prms.bpp!=p->bpp) {
		abort_async_msg(&dat->msg);
		if(dat->pixarray) {
		    (*sv.MyFreeSharedMem)(&sv,dat->pixarray);
		    dat->pixarray=NULL;
		}
		FreeBitMap(dat->bitmap);
		dat->bitmap=NULL;
		DB(kprintf("bitmap reset\n");)
	    } else if(dat->prms.pdfdoc!=p->pdfdoc ||
		      dat->prms.aout!=p->aout ||
		      dat->prms.page!=p->page ||
		      dat->prms.xoffset!=p->xoffset ||
		      dat->prms.yoffset!=p->yoffset ||
		      dat->prms.dpi!=p->dpi ||
		      dat->prms.rotate!=p->rotate ||
		      dat->prms.pixfmt!=p->pixfmt ||
		      dat->prms.level<p->level) {
		abort_async_msg(&dat->msg);
		DB(kprintf("invalidated\n");)
	    } else
		break; /* nothing changed, except maybe dat->prms.level.
			  Don't change it. */
	    dat->valid=FALSE;
	    dat->last_state=0;
	    dat->prms=*p;
	    break;
	  }

	  case MYATTR_GfxChunk_Drawing:
	    dat->drawing=tag->ti_Data;
	    break;
	}
    }
    return DoSuperMethodA(cl,obj,msg);
}


Static ULONG gfxGet(struct IClass *cl,Object *obj,struct opGet *msg) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    switch(msg->opg_AttrID) {
      case MYATTR_GfxChunk_Data:
	*msg->opg_Storage=(ULONG)(dat->valid?dat->pixarray:NULL);
	DB(kprintf("%lx: data = %lx\n",obj,*msg->opg_Storage);)
	return TRUE;
      case MYATTR_GfxChunk_Done:
	*msg->opg_Storage=dat->valid && dat->prms.level>=dat->returned_level;
	return TRUE;
      case MYATTR_GfxChunk_Ready:
	*msg->opg_Storage=FALSE;
	return TRUE;
      case MYATTR_GfxChunk_PartialData: {
	int state=*sv.draw_state_ptr;
	if(state>dat->last_state) {
	    *msg->opg_Storage=(ULONG)dat->pixarray;
	    dat->last_state=state;
	} else
	    *msg->opg_Storage=0;
	DB(kprintf("%lx: partial data = %lx\n",obj,*msg->opg_Storage);)
	return TRUE;
      }
      case MYATTR_GfxChunk_Drawing:
	*msg->opg_Storage=dat->drawing;
	return TRUE;
      case MYATTR_GfxChunk_BitMap:
	if(!dat->bitmap && dat->valid && dat->pixarray && CyberGfxBase &&
	   dat->prms.friendbm) {
	    int ok=0;
	    dat->bitmap=AllocBitMap(dat->prms.width,dat->prms.height,dat->prms.bpp*8,BMF_MINPLANES,dat->prms.friendbm);
	    if(dat->bitmap) {
		ULONG pixfmt,bpr;
		unsigned char *base;
		APTR handle;
		struct TagItem tags[4];
		tags[0].ti_Tag=LBMI_PIXFMT;
		tags[0].ti_Data=(ULONG)&pixfmt,
		tags[1].ti_Tag=LBMI_BASEADDRESS;
		tags[1].ti_Data=(ULONG)&base,
		tags[2].ti_Tag=LBMI_BYTESPERROW;
		tags[2].ti_Data=(ULONG)&bpr;
		tags[3].ti_Tag=TAG_END;
		handle=LockBitMapTagList(dat->bitmap,tags);
		if(handle) {
		    if(pixfmt==dat->prms.pixfmt) {
			int h=dat->prms.height;
			unsigned char *p=base;
			unsigned char *q=dat->pixarray;
			int mod1=bpr;
			int mod2=dat->prms.modulo*dat->prms.bpp;
			int w=dat->prms.width*dat->prms.bpp;
			if(mod1==mod2 && mod1==w) {
			    /* Optimize this special case where just one
			     * big copy is needed.
			     */
			    CopyMem(q,p,w*h);
			} else {
			    while(--h>=0) {
				memcpy(p,q,w);
				p+=mod1;
				q+=mod2;
			    }
			}
			ok=1;
		    }
		    UnLockBitMap(handle);
		    DB(kprintf("gfxchunk: lockbm -> %ld, %ld\n",pixfmt,dat->prms.pixfmt);)
		}
	    }
	    DB(kprintf("gfxchunk: ok = %ld\n",ok);)
	    if(ok) {
		(*sv.MyFreeSharedMem)(&sv,dat->pixarray);
		dat->pixarray=NULL;
	    } else {
		FreeBitMap(dat->bitmap);
		dat->bitmap=NULL;
	    }
	}
	*msg->opg_Storage=(ULONG)dat->bitmap;
	return TRUE;
    }
    return DoSuperMethodA(cl,obj,(Msg)msg);
}


Static ULONG gfxClear(struct IClass *cl,Object *obj) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    DB(kprintf("%lx cleared\n",obj);)
    abort_async_msg(&dat->msg);
    if(dat->pixarray) {
	(*sv.MyFreeSharedMem)(&sv,dat->pixarray);
	dat->pixarray=NULL;
    }
    FreeBitMap(dat->bitmap);
    dat->bitmap=NULL;
    dat->cleared=TRUE;
    dat->last_state=0;
    return 0;
}

Static ULONG gfxStartDrawing(struct IClass *cl,Object *obj) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    if(!dat->msg.replied)
	return TRUE;
    dat->valid=FALSE;
    if(dat->cleared)
	return FALSE;
    //if(!is_sync())
    //    return FALSE;
    if(!dat->pixarray) {
	FreeBitMap(dat->bitmap);
	dat->bitmap=NULL;
	if(dat->prms.width>=1 && dat->prms.height>=1)
	    dat->pixarray=(*sv.MyAllocSharedMem)(&sv,dat->prms.modulo*dat->prms.height*dat->prms.bpp MEMDBG(+4));
	if(!dat->pixarray)
	    return TRUE; /* Don't try to restart */
    }
    MEMDBG(*(ULONG*)((int)dat->pixarray+dat->prms.modulo*dat->prms.height*dat->prms.bpp)=0xbbbbbbbb;)
    dat->msg.data=dat->pixarray;
    dat->msg.sz=dat->prms.modulo*dat->prms.height*dat->prms.bpp MEMDBG(+4);
    dat->msg.id=MSGID_PAGE;
    send_async_msg(&dat->msg);
    return TRUE;
}

Static ULONG gfxRestartDrawing(struct IClass *cl,Object *obj) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    if(dat->valid)
	++dat->prms.level;
    return gfxStartDrawing(cl,obj);
}

Static ULONG gfxSendInit(struct IClass *cl,Object* obj) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    struct comm_info *ci=dat->msg.ci;
    struct msg_page *p=ci->cmddata;

    if(dat->cleared)
	return FALSE;
    dat->last_state=0;

    set(obj,MYATTR_GfxChunk_Drawing,TRUE);

    p->aout=dat->prms.aout;
    p->doc=dat->prms.pdfdoc;
    p->page_num=dat->prms.page;
    p->zoom=dat->prms.dpi;
    p->rotate=dat->prms.rotate;
    p->xoffset=dat->prms.xoffset;
    p->yoffset=dat->prms.yoffset;
    p->width=dat->prms.width;
    p->height=dat->prms.height;
    p->modulo=dat->prms.modulo;
    p->bpp=dat->prms.bpp;
    p->pixfmt=dat->prms.pixfmt;
    p->level=dat->prms.level;
    p->returned_level=-1;
    DB(kprintf("start_drawing(%lx,%ld,%ld,%ld,%ld,%ld,%ld,%lx)\n",obj,
	       p->xoffset,p->yoffset,p->width,p->height,
	       p->modulo,p->level,dat->pixarray);)
    (*ci->sv->exchange_msg)(ci,&p->base,sizeof(*p),MSGID_PAGE);
    return TRUE;
}

Static ULONG gfxReplied(struct IClass *cl,Object* obj) {
    GfxChunkData *dat=INST_DATA(cl,obj);
    switch(dat->msg.id) {
      case MSGID_PAGE: {
	struct comm_info *ci=dat->msg.ci;
	struct msg_page *p=ci->cmddata;
	MEMDBG(if(*(ULONG*)((int)dat->pixarray+dat->modulo*dat->height*dat->bpp)!=0xbbbbbbbb)
	       kprintf("bytes past bm trashed: %08lx\n",
		       *(ULONG*)((int)dat->pixarray+dat->modulo*dat->height*dat->bpp));)
	(*sv.exchange_msg)(ci,&p->base,sizeof(*p),MSGID_PAGE);
	dat->returned_level=p->returned_level;
	dat->valid=p->returned_level!=-1;
	DB(kprintf("finished(%ld)\n",p->returned_level);)
	set(obj,MYATTR_GfxChunk_Drawing,FALSE);
	set(obj,MYATTR_GfxChunk_Ready,TRUE);
	return dat->valid;
      }

#if USE_PENS
      case MSGID_GETCOLORS: {
	struct msg_getcolors *m=dat->msg.data;
	get_pens(dat->color_allocator,m->table,m->num);
	break;
      }

      case MSGID_GETCOLOR: {
	struct msg_getcolor *m=dat->msg.data;
	m->base.success=get_pen(dat->color_allocator,m->r,m->g,m->b);
	break;
      }
#endif
    }
    reply_async_msg(&dat->msg,dat->msg.msg);
    return TRUE;
}

BEGIN_DISPATCHER(gfxDispatcher,cl,obj,msg) {
    switch(msg->MethodID) {
      case OM_NEW:                      return gfxNew           (cl,obj,(APTR)msg);
      case OM_DISPOSE:                  return gfxDispose       (cl,obj,(APTR)msg);
      case OM_SET:                      return gfxSet           (cl,obj,(APTR)msg);
      case OM_GET:                      return gfxGet           (cl,obj,(APTR)msg);
      case MYM_GfxChunk_Clear:          return gfxClear         (cl,obj);
      case MYM_GfxChunk_StartDrawing:   return gfxStartDrawing  (cl,obj);
      case MYM_GfxChunk_RestartDrawing: return gfxRestartDrawing(cl,obj);
      case MYM_GfxChunk_Replied:        return gfxReplied       (cl,obj);
      case MYM_GfxChunk_SendInit:       return gfxSendInit      (cl,obj);
    }
    return DoSuperMethodA(cl,obj,msg);
}
END_DISPATCHER(gfxDispatcher)

struct MUI_CustomClass *gfxchunk_mcc;
static int count;

BOOL createGfxChunkClass(void) {
    if(count++==0) {
	if(gfxchunk_mcc=MUI_CreateCustomClass(NULL,MUIC_Notify,NULL,sizeof(GfxChunkData),(APTR)&gfxDispatcher))
	    return TRUE;
	count=0;
	return FALSE;
    }
    return TRUE;
}

BOOL deleteGfxChunkClass(void) {
    if(count && --count==0)
	MUI_DeleteCustomClass(gfxchunk_mcc);
    return TRUE;
}

